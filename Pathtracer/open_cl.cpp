#include "open_cl.hpp"
#include <fstream>
#include <vector>
#include <stdlib.h> 
#include <stdio.h>
#include <iostream>
#include <string>

std::string full_path(const char* rel_path)
{
	char full[_MAX_PATH];
	if (_fullpath(full, rel_path, _MAX_PATH) != NULL)
	{
		return std::string(full);
	}

	return "";
}

std::string read_file(const char* fname)
{
	// Open the stream
	std::ifstream is(fname);
	is.seekg(0, std::ios_base::end);
	std::size_t size = static_cast<::size_t>(is.tellg());
	is.seekg(0, std::ios_base::beg);
	std::vector<char> v(size / sizeof(char));
	is.read((char*)&v[0], size);
	is.close();

	return std::string(&v[0]);
}

cl::Context *ctx;
cl::CommandQueue *cq;
cl::Device *def_d;

void OpenCLWrap::initialise()
{

	//get all platforms (drivers)
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);
	if (all_platforms.size() == 0) {
		std::cout << " No platforms found. Check OpenCL installation!\n";
		exit(1);
	}
	cl::Platform default_platform = all_platforms.back();
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	//get default device of the default platform
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if (all_devices.size() == 0) {
		std::cout << " No devices found. Check OpenCL installation!\n";
		exit(1);
	}
	def_d = new cl::Device(all_devices[0]);
	std::cout << "Using device: " << def_d->getInfo<CL_DEVICE_NAME>() << "\n";

	ctx = new cl::Context({ *def_d });

	//create queue to which we will push commands for the device.
	cq = new cl::CommandQueue(*ctx, *def_d);

	cl::Program program = OpenCLWrap::makeProgram({ "add.cl" });
	int A[50], B[50], C[50];
	for (int i = 0; i < 50; ++i) { A[i] = i; B[i] = i; }
	std::vector<cl::Buffer> buffers = makeBuffers(3, { CL_MEM_READ_WRITE }, { 0 }, { sizeof(int) * 50 }, { A, B, nullptr });
	cl::Kernel kernel_add = cl::Kernel(program, "simple_add");
	kernel_add.setArg(0, buffers[0]); kernel_add.setArg(1, buffers[1]); kernel_add.setArg(2, buffers[2]); kernel_add.setArg(3, 5);
	execute(kernel_add, 50);
	readBuffer(buffers[2], 0, sizeof(int) * 50, C);

	std::cout << " result: \n";
	for (int i = 0; i<50; i++) {
		std::cout << C[i] << " ";
	}
}

cl::Program OpenCLWrap::makeProgram(std::initializer_list<const char *> cl_files, const char * options)
{
	std::vector<std::string> strs;
	cl::Program::Sources sources;
	auto options_ = "-I \"" + full_path("kernels/") + "\"" + options;

	for (const char *filepath : cl_files)
	{
		strs.push_back(read_file((std::string("kernels/") + filepath).c_str()));
		sources.push_back({ strs.back().c_str(),strs.back().length() });
	}

	cl::Program program(*ctx, sources);
	if (program.build({ *def_d }, options_.c_str()) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(*def_d) << "\n";
		return cl::Program();
	}

	return program;
}


cl::Buffer OpenCLWrap::makeBuffer(cl_mem_flags flags, ::size_t offset, ::size_t size, const void* ptr)
{
	cl::Buffer buffer(*ctx, flags, size);
	if (ptr) cq->enqueueWriteBuffer(buffer, CL_TRUE, offset, size, ptr);

	return buffer;
}

std::vector<cl::Buffer> OpenCLWrap::makeBuffers(int count, std::initializer_list<cl_mem_flags> flags, std::initializer_list<::size_t> offset, std::initializer_list<::size_t> size, std::initializer_list<const void*> ptr)
{
#define get_(src) src[min(static_cast<size_t>(i), src.size() - 1)]

	std::vector<cl::Buffer> buffers;

	auto _flags = std::vector<cl_mem_flags>(flags);
	auto _sizes = std::vector<::size_t>(size);
	auto _offsets = std::vector<::size_t>(offset);
	auto _ptrs = std::vector<const void *>(ptr);

	for (int i = 0; i < count; ++i)
	{
		auto size__ = get_(_sizes);
		auto ptr__ = get_(_ptrs);

		buffers.push_back(cl::Buffer(*ctx, get_(_flags), size__));
		if (ptr__) cq->enqueueWriteBuffer(buffers.back(), CL_FALSE, get_(_offsets), size__, ptr__);
	}

	cq->finish();

	return buffers;
}

void OpenCLWrap::readBuffer(const cl::Buffer &buffer, ::size_t offset, ::size_t size, void* ptr)
{
	cq->enqueueReadBuffer(buffer, CL_TRUE, offset, size, ptr);
}

void OpenCLWrap::execute(const cl::Kernel &kernel, int x, int y, int z)
{
	cq->enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(x, y, z), cl::NullRange);
	cq->finish();
}

cl::ImageGL *OpenCLWrap::referenceImageGL(GLuint glTexHandle)
{
	return new cl::ImageGL(*ctx, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, glTexHandle);
}

void OpenCLWrap::dereferenceImageGL(cl::ImageGL **clImage)
{
	delete *clImage;
	clImage = nullptr;
}