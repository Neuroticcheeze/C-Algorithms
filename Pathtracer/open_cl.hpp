#ifndef _H_COMPUTE
#define _H_COMPUTE

#include <initializer_list>
#include <vector>
#include <CL\cl.hpp>
#include <GLFW\glfw3.h>

namespace OpenCLWrap
{
	void initialise();
	void finalise();

	cl::Program makeProgram(std::initializer_list<const char *> cl_files, const char * options = "");
	cl::Buffer makeBuffer(cl_mem_flags flags, ::size_t offset, ::size_t size, const void* ptr = nullptr);
	std::vector<cl::Buffer> makeBuffers(int count, std::initializer_list<cl_mem_flags> flags, std::initializer_list<::size_t> offset, std::initializer_list<::size_t> size, std::initializer_list<const void*> ptr);
	void readBuffer(const cl::Buffer &buffer, ::size_t offset, ::size_t size, void* ptr);
	void execute(const cl::Kernel &kernel, int x, int y = 1, int z = 1);
	cl::ImageGL *OpenCLWrap::referenceImageGL(GLuint glTexHandle);
	void OpenCLWrap::dereferenceImageGL(cl::ImageGL **clImage);
}

#endif//_H_COMPUTE