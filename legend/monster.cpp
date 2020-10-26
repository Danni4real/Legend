/*
 * monster.cpp
 *
 *  Created on: 2020年8月1日
 *      Author: dan
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "common.cpp"
#include "common.h"
#include "olcPixelGameEngine.h"

#define MONSTER_WALK_FRAMES_NUM 4
#define MONSTER_PIXELS_SHIFT_PER_WALK 3
#define MONSTER_DEMON_BLOOD   2
#define MONSTER_DEMON_FRAME_WIDTH   32
#define MONSTER_DEMON_FRAME_HEIGHT  36

class Monster
{
private:
	char pic_path[128];
	int frame_width = 0;
	int frame_height = 0;

	int blood_left = 0;
	int dying_cost = MONSTER_DYING_COST_TIME;
	int pos_x = 0;
	int pos_y = 0;

	int cmd_direction = NONE;
	int current_direction = RIGHT;
	int current_state = FREE;

	int walk_frame_index = FRAME_INDEX_RESET_VALUE;

	olc::Pixel* blank_frame_bufs;
	olc::Pixel* current_active_frame_buf = 0;
	olc::Pixel* idle_frame_bufs[LEFT_PLUS_RIGHT];
	olc::Pixel* walk_frame_bufs[LEFT_PLUS_RIGHT][MONSTER_WALK_FRAMES_NUM];

	void LoadAllPic()
	{
		char path_tmp[256];
		strcpy(path_tmp, pic_path);
		strcat(path_tmp, "/Idle_Left.rgba") ;
		LoadPic(path_tmp, idle_frame_bufs[LEFT], frame_width, frame_height);

		strcpy(path_tmp, pic_path);
		strcat(path_tmp, "/Idle_Right.rgba") ;
		LoadPic(path_tmp, idle_frame_bufs[RIGHT], frame_width, frame_height);

		strcpy(path_tmp, pic_path);
		strcat(path_tmp, "/Walk_Left_0.rgba") ;
		LoadPic(path_tmp, walk_frame_bufs[LEFT][0], frame_width, frame_height);
		strcpy(path_tmp, pic_path);
		strcat(path_tmp, "/Walk_Left_1.rgba") ;
		LoadPic(path_tmp, walk_frame_bufs[LEFT][1], frame_width, frame_height);
		strcpy(path_tmp, pic_path);
		strcat(path_tmp, "/Walk_Left_2.rgba") ;
		LoadPic(path_tmp, walk_frame_bufs[LEFT][2], frame_width, frame_height);
		strcpy(path_tmp, pic_path);
		strcat(path_tmp, "/Walk_Left_3.rgba") ;
		LoadPic(path_tmp, walk_frame_bufs[LEFT][3], frame_width, frame_height);

		strcpy(path_tmp, pic_path);
		strcat(path_tmp, "/Walk_Right_0.rgba") ;
		LoadPic(path_tmp, walk_frame_bufs[RIGHT][0], frame_width, frame_height);
		strcpy(path_tmp, pic_path);
		strcat(path_tmp, "/Walk_Right_1.rgba") ;
		LoadPic(path_tmp, walk_frame_bufs[RIGHT][1], frame_width, frame_height);
		strcpy(path_tmp, pic_path);
		strcat(path_tmp, "/Walk_Right_2.rgba") ;
		LoadPic(path_tmp, walk_frame_bufs[RIGHT][2], frame_width, frame_height);
		strcpy(path_tmp, pic_path);
		strcat(path_tmp, "/Walk_Right_3.rgba") ;
		LoadPic(path_tmp, walk_frame_bufs[RIGHT][3], frame_width, frame_height);
	}
	void Dying()
	{
		if(current_state == DYING && --dying_cost == 0)
		{
			current_state = DIED;
			return;
		}
		if(dying_cost%2 == 0)
			current_active_frame_buf = idle_frame_bufs[current_direction];
		else
			current_active_frame_buf = blank_frame_bufs;
	}
	void UpdateBlood()
	{
		if(current_state == BE_ATTACK)
			if(--blood_left == 0)
				current_state = DYING;
	}

	void UpdatePosition()
	{
		if(cmd_direction == LEFT && current_state == FREE)
			pos_x -= MONSTER_PIXELS_SHIFT_PER_WALK;
		if(cmd_direction == LEFT && current_state == BE_ATTACK)
			pos_x += MONSTER_PIXELS_SHIFT_PER_WALK*3;

		if(cmd_direction == RIGHT && current_state == FREE)
			pos_x += MONSTER_PIXELS_SHIFT_PER_WALK;
		if(cmd_direction == RIGHT && current_state == BE_ATTACK)
			pos_x -= MONSTER_PIXELS_SHIFT_PER_WALK*3;
	}
	void UpdateIdle()
	{
		current_active_frame_buf = idle_frame_bufs[current_direction];
	}
	void UpdateWalk()
	{
		if(cmd_direction == NONE)
		{
			walk_frame_index = FRAME_INDEX_RESET_VALUE;
			return;
		}

		if(cmd_direction == current_direction &&
		   walk_frame_index == MONSTER_WALK_FRAMES_NUM)
			walk_frame_index = FRAME_INDEX_RESET_VALUE;

		if(cmd_direction != current_direction)
			walk_frame_index = FRAME_INDEX_RESET_VALUE;

		current_active_frame_buf = walk_frame_bufs[cmd_direction][walk_frame_index];
		walk_frame_index++;
	}
	void ExecuteCmd()
	{
		// behavior priority walk > idle
		UpdateIdle();
		UpdateWalk();
		UpdatePosition();
		UpdateBlood();
	}
public:
	Monster(char* pic_path, int blood, int pos_x, int frame_width, int frame_height)
	{
		strcpy(this->pic_path, pic_path);
		this->pos_x = pos_x;
		this->frame_width = frame_width;
		this->frame_height = frame_height;
		blood_left = blood;
		pos_y = GameWindowHeight - MEADOW_HEIGHT - frame_height + 4;

		for(int i = 0; i <LEFT_PLUS_RIGHT; i++)
				idle_frame_bufs[i] = (olc::Pixel*)malloc(frame_width*frame_height*sizeof(olc::Pixel));
		for(int i = 0; i <LEFT_PLUS_RIGHT; i++)
			for(int n = 0; n <MONSTER_WALK_FRAMES_NUM; n++)
				walk_frame_bufs[i][n] = (olc::Pixel*)malloc(frame_width*frame_height*sizeof(olc::Pixel));

		blank_frame_bufs = (olc::Pixel*)malloc(frame_width*frame_height*sizeof(olc::Pixel));
		for (int x = 0; x < frame_width; x++)
			for (int y = 0; y < frame_height; y++)
				blank_frame_bufs[y*frame_width + x] = olc::BLANK;

		LoadAllPic();
		UpdateIdle();
	}

	void SlidePixels(int direction, int slide_pixels)
	{
		if(direction == LEFT)
			this->pos_x -= slide_pixels;
		if(direction == RIGHT)
			this->pos_x += slide_pixels;
	}

	void Control(int direction, int state)
	{
		if(current_state == DIED)
			return;
		if(current_state == DYING)
		{
			Dying();
			return;
		}

		current_state = state;
		cmd_direction = direction;

		ExecuteCmd();

		if(direction != NONE)
			current_direction = direction;
	}

	int GetPosX()
	{
		return pos_x;
	}

	int GetPosY()
	{
		return pos_y;
	}

	int GetFrameWidth()
	{
		return frame_width;
	}

	int GetFrameHeight()
	{
		return frame_height;
	}

	int GetState()
	{
		return current_state;
	}

	olc::Pixel* GetFrameBuf()
	{
		return current_active_frame_buf;
	}
};

Monster* CreateMonster(int monster_type, int pos_x)
{
	switch(monster_type)
	{
		case DEMON: return new Monster("rgba/monster/demon",
				                       MONSTER_DEMON_BLOOD,
									   pos_x,
									   MONSTER_DEMON_FRAME_WIDTH,
									   MONSTER_DEMON_FRAME_HEIGHT);
	}

	return 0;
}



