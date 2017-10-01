#pragma once
#ifndef SCREEN_H_
#define SCREEN_H_

#include "colour.hpp"
#include <Windows.h>
#include <algorithm>
#include <memory>
#include <glm\common.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::uvec2;
using glm::min;

// Save the bitmap to a bmp file  
extern void SaveBitmapToFile(BYTE* pBitmapBits,
	LONG lWidth,
	LONG lHeight,
	WORD wBitsPerPixel,
	const unsigned long& padding_size,
	LPCTSTR lpszFileName);

extern BYTE* LoadBMP(int* width, int* height, unsigned long* size, LPCTSTR bmpfile);

extern std::unique_ptr<BYTE[]> CreateNewBuffer(unsigned long& padding,
	BYTE* pmatrix,
	const int& width,
	const int& height);

extern void initialiseScreen(unsigned width, unsigned height);

extern void setScreenColour(unsigned x, unsigned y, const colour &colour_, bool andIllumination = true);
extern const colour& getScreenColour(unsigned x, unsigned y);

extern uvec2 getScreenDims();

extern void finaliseScreen(const char* output = "render.bmp");

#endif//SCREEN_H_