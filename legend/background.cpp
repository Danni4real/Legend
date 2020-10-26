/*
 * background.cpp
 *
 *  Created on: 2020年7月31日
 *      Author: dan
 */


#include <stdio.h>
#include <iostream>
#include "common.cpp"
#include "common.h"
#include "olcPixelGameEngine.h"

#define LAYER_1_SLIDE_SPEED 1
#define LAYER_2_SLIDE_SPEED 2
#define LAYER_3_SLIDE_SPEED 3
#define LAYER_4_SLIDE_SPEED 4


class Background
{
private:
	olc::Pixel background[GameWindowWidth*GameWindowHeight];

	olc::Pixel layer_0[GameWindowWidth*GameWindowHeight];
	olc::Pixel layer_1[GameWindowWidth*GameWindowHeight];
	olc::Pixel layer_2[GameWindowWidth*GameWindowHeight];
	olc::Pixel layer_3[GameWindowWidth*GameWindowHeight];
	olc::Pixel layer_4[GameWindowWidth*GameWindowHeight];
	olc::Pixel meadow[GameWindowWidth*MEADOW_HEIGHT];

	int layer_1_slide_value = 0;
	int layer_2_slide_value = 0;
	int layer_3_slide_value = 0;
	int layer_4_slide_value = 0;
	int meadow_slide_value = 0;

	void SlideEachLayer(int left_or_right)
	{
		if(left_or_right == LEFT)
		{
			layer_1_slide_value += LAYER_1_SLIDE_SPEED;
			layer_2_slide_value += LAYER_2_SLIDE_SPEED;
			layer_3_slide_value += LAYER_3_SLIDE_SPEED;
			layer_4_slide_value += LAYER_4_SLIDE_SPEED;
			meadow_slide_value += MEADOW_SLIDE_SPEED;
		}
		if(left_or_right == RIGHT)
		{
			layer_1_slide_value += (GameWindowWidth-LAYER_1_SLIDE_SPEED);
			layer_2_slide_value += (GameWindowWidth-LAYER_2_SLIDE_SPEED);
			layer_3_slide_value += (GameWindowWidth-LAYER_3_SLIDE_SPEED);
			layer_4_slide_value += (GameWindowWidth-LAYER_4_SLIDE_SPEED);
			meadow_slide_value += (GameWindowWidth-MEADOW_SLIDE_SPEED);
		}

		layer_1_slide_value = layer_1_slide_value%GameWindowWidth;
		layer_2_slide_value = layer_2_slide_value%GameWindowWidth;
		layer_3_slide_value = layer_3_slide_value%GameWindowWidth;
		layer_4_slide_value = layer_4_slide_value%GameWindowWidth;
		meadow_slide_value = meadow_slide_value%GameWindowWidth;
	}

	void LayMeadow()
	{
		for(int x = meadow_slide_value; x < GameWindowWidth; x++)
			for(int y = 0; y < MEADOW_HEIGHT; y++)
				background[(y+GameWindowHeight-MEADOW_HEIGHT)*GameWindowWidth+x-meadow_slide_value] = meadow[y*GameWindowWidth+x];

		for(int x = 0; x < meadow_slide_value; x++)
			for(int y = 0; y < MEADOW_HEIGHT; y++)
				background[(y+GameWindowHeight-MEADOW_HEIGHT)*GameWindowWidth+x+GameWindowWidth-meadow_slide_value] = meadow[y*GameWindowWidth+x];
	}

	void LayLayer(olc::Pixel* layer, int slide_value)
	{

		for(int x = slide_value; x < GameWindowWidth; x++)
			for(int y = 0; y < GameWindowHeight; y++)
			{
				if(layer[y*GameWindowWidth+x] != olc::BLANK)
					background[y*GameWindowWidth+x-slide_value] = layer[y*GameWindowWidth+x];
			}

		for(int x = 0; x < slide_value; x++)
			for(int y = 0; y < GameWindowHeight; y++)
			{
				if(layer[y*GameWindowWidth+x] != olc::BLANK)
					background[y*GameWindowWidth+x+GameWindowWidth-slide_value] = layer[y*GameWindowWidth+x];
			}

	}
	void LoadAllPic()
	{
		LoadPic("rgba/background/Background_Layer_0.rgba", layer_0, GameWindowWidth, GameWindowHeight);
		LoadPic("rgba/background/Background_Layer_1.rgba", layer_1, GameWindowWidth, GameWindowHeight);
		LoadPic("rgba/background/Background_Layer_2.rgba", layer_2, GameWindowWidth, GameWindowHeight);
		LoadPic("rgba/background/Background_Layer_3.rgba", layer_3, GameWindowWidth, GameWindowHeight);
		LoadPic("rgba/background/Background_Layer_4.rgba", layer_4, GameWindowWidth, GameWindowHeight);
		LoadPic("rgba/background/Meadow.rgba", meadow, GameWindowWidth, MEADOW_HEIGHT);
	}
public:
	Background()
	{
		LoadAllPic();
		for(int i=0;i<GameWindowWidth*GameWindowHeight;i++)
			background[i] = layer_0[i];

		LayLayer(layer_1, 0);
		LayLayer(layer_2, 0);
		LayLayer(layer_3, 0);
		LayLayer(layer_4, 0);
		LayMeadow();
	}
	void Slide(int left_or_right)
	{
		for(int i=0;i<GameWindowWidth*GameWindowHeight;i++)
			background[i] = layer_0[i];

		SlideEachLayer(left_or_right);

		LayLayer(layer_1, layer_1_slide_value);
		LayLayer(layer_2, layer_2_slide_value);
		LayLayer(layer_3, layer_3_slide_value);
		LayLayer(layer_4, layer_4_slide_value);
		LayMeadow();
	}

	olc::Pixel* GetFrameBuf()
	{
		return background;
	}
};


