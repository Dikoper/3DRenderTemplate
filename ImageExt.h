#pragma once
#include "tgaimage.h"

uint32_t tgaColToUInt(TGAColor tC) 
{
	uint32_t clr = (tC.bgra[3] << 24) + (tC.bgra[2] << 16) + (tC.bgra[1] << 8) + tC.bgra[0];
	return clr;
}

uint32_t multByScalar(uint32_t clr, float comp)
{
	int a = (clr & 0xFF000000) >> 24;
	int r = (clr & 0x00FF0000) >> 16;
	int g = (clr & 0x0000FF00) >> 8;
	int b = (clr & 0x000000FF);

	r *= comp; g *= comp; b *= comp;
	
	uint32_t res = (a << 24) + (r << 16) + (g << 8) + b;
	return res;
}

//texture loader