/*
 * common.cpp
 *
 *  Created on: 2020年7月31日
 *      Author: dan
 */
#ifndef COMMON_CPP_
#define COMMON_CPP_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "common.h"
#include "olcPixelGameEngine.h"

void print(char* info)
{
	std::cout << info << std::endl;
}

bool LoadPic(char* pic_path, olc::Pixel* frame_bufs, int pic_width, int pic_height)
{
	FILE* fp;
	char *pic_buf = (char*)malloc(pic_width*pic_height*BYTE_NUM_PER_PIXEL);

	fp = fopen(pic_path,"r");
	if(!fp)
	{
		print("Err: failed to open pic file: ");
		return false;
	}

	int read_size = fread(pic_buf,1,pic_width*pic_height*BYTE_NUM_PER_PIXEL,fp);
	if(read_size != pic_width*pic_height*BYTE_NUM_PER_PIXEL)
	{
		print("Err: failed to read contents from pic file: ");
		return false;
	}

	for(int x = 0; x < pic_width; x++)
		for(int y = 0; y < pic_height; y++)
			frame_bufs[pic_width*y + x] = olc::Pixel(pic_buf[(pic_width*y + x)*BYTE_NUM_PER_PIXEL+0],
													 pic_buf[(pic_width*y + x)*BYTE_NUM_PER_PIXEL+1],
													 pic_buf[(pic_width*y + x)*BYTE_NUM_PER_PIXEL+2],
													 pic_buf[(pic_width*y + x)*BYTE_NUM_PER_PIXEL+3]);

	fclose(fp);
	free(pic_buf);
	return true;
}

#endif /* COMMON_CPP_ */
