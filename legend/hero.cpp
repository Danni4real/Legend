#include <stdio.h>
#include <iostream>
#include "common.cpp"
#include "common.h"
#include "olcPixelGameEngine.h"

#define HERO_BLOOD 100
#define HERO_BLOOD_TO_HEART_RATIO 20
#define HERO_HEARTS HERO_BLOOD/HERO_BLOOD_TO_HEART_RATIO
#define HERO_HEARTS_POX_X 0
#define HERO_HEARTS_POX_Y 0
#define HERO_HEARTS_FRAME_WIDTH  16
#define HERO_HEARTS_FRAME_HEIGHT 16

#define HERO_FRAME_WIDTH  16
#define HERO_FRAME_HEIGHT 16

#define IDLE_FRAMES_NUM   4
#define WALK_FRAMES_NUM   4
#define FALL_FRAMES_NUM   4
#define DIED_FRAMES_NUM   4
#define JUMP_FRAMES_NUM   8
#define ATTACK_FRAMES_NUM 8

#define HERO_POS_X_DEFAULT  GameWindowWidth/5
#define HERO_POS_Y_DEFAULT  GameWindowHeight - MEADOW_HEIGHT - HERO_FRAME_HEIGHT + 4

#define HERO_POS_X_MIN GameWindowWidth/5
#define HERO_POS_X_MAX GameWindowWidth*4/5

#define PIXELS_SHIFT_PER_JUMP 5
#define PIXELS_SHIFT_PER_WALK 5

class Blood
{
private:
	int blood_left = HERO_BLOOD;
	int half_hearts_left = HERO_HEARTS*2;

	olc::Pixel blood_full_frame_buf[HERO_HEARTS_FRAME_WIDTH*HERO_HEARTS_FRAME_HEIGHT];
	olc::Pixel blood_half_frame_buf[HERO_HEARTS_FRAME_WIDTH*HERO_HEARTS_FRAME_HEIGHT];
	olc::Pixel blood_empty_frame_buf[HERO_HEARTS_FRAME_WIDTH*HERO_HEARTS_FRAME_HEIGHT];
	olc::Pixel blood_frame_bufs[HERO_HEARTS*HERO_HEARTS_FRAME_WIDTH*HERO_HEARTS_FRAME_HEIGHT];

	void LoadAllPic()
	{
		LoadPic("rgba/status/hero_blood_full.rgba", blood_full_frame_buf, HERO_HEARTS_FRAME_WIDTH, HERO_HEARTS_FRAME_HEIGHT);
		LoadPic("rgba/status/hero_blood_half.rgba", blood_half_frame_buf, HERO_HEARTS_FRAME_WIDTH, HERO_HEARTS_FRAME_HEIGHT);
		LoadPic("rgba/status/hero_blood_empty.rgba", blood_empty_frame_buf, HERO_HEARTS_FRAME_WIDTH, HERO_HEARTS_FRAME_HEIGHT);
	}

	void Update()
	{
		static int old_half_hearts_left = HERO_HEARTS*2;

		half_hearts_left = blood_left*2/HERO_BLOOD_TO_HEART_RATIO;
		int whole_hearts_left = half_hearts_left/2;

		if(half_hearts_left != old_half_hearts_left)
		{
			old_half_hearts_left = half_hearts_left;

			if(whole_hearts_left*2 == half_hearts_left)
			{
				for (int x = 0; x < HERO_HEARTS_FRAME_WIDTH; x++)
					for (int y = 0; y < HERO_HEARTS_FRAME_HEIGHT; y++)
						blood_frame_bufs[y*HERO_HEARTS_FRAME_WIDTH*HERO_HEARTS+x+HERO_HEARTS_FRAME_WIDTH*whole_hearts_left] = blood_empty_frame_buf[y*HERO_HEARTS_FRAME_WIDTH+x];
			}
			else
			{
				for (int x = 0; x < HERO_HEARTS_FRAME_WIDTH; x++)
					for (int y = 0; y < HERO_HEARTS_FRAME_HEIGHT; y++)
						blood_frame_bufs[y*HERO_HEARTS_FRAME_WIDTH*HERO_HEARTS+x+HERO_HEARTS_FRAME_WIDTH*whole_hearts_left] = blood_half_frame_buf[y*HERO_HEARTS_FRAME_WIDTH+x];
			}
		}
	}

public:
	Blood()
	{
		LoadAllPic();
		for(int i=0;i<HERO_HEARTS;i++)
			for (int x = 0; x < HERO_HEARTS_FRAME_WIDTH; x++)
				for (int y = 0; y < HERO_HEARTS_FRAME_HEIGHT; y++)
					blood_frame_bufs[y*HERO_HEARTS_FRAME_WIDTH*HERO_HEARTS+x+HERO_HEARTS_FRAME_WIDTH*i] = blood_full_frame_buf[y*HERO_HEARTS_FRAME_WIDTH+x];
	}

	olc::Pixel* GetFrameBuf()
	{
		return blood_frame_bufs;
	}


	void Add()
	{
		blood_left++;
		Update();

	}

	void Minus()
	{
		blood_left--;
		Update();
	}

	int GetBloodLeft()
	{
		return blood_left;
	}

	int GetPosX()
	{
		return HERO_HEARTS_POX_X;
	}

	int GetPosY()
	{
		return HERO_HEARTS_POX_Y;
	}

	int GetFrameWidth()
	{
		return HERO_HEARTS_FRAME_WIDTH*HERO_HEARTS;
	}

	int GetFrameHeight()
	{
		return HERO_HEARTS_FRAME_HEIGHT;
	}
};

class Hero
{

private:

	bool hit_right_edge = false;
	bool hit_left_edge = false;

	int cmd_state = FREE;
	int cmd_action = IDLE;
	int cmd_direction = NONE;

	bool current_jumping = false;
	bool current_attacking = false;
	int current_action = IDLE;
	int current_state = FREE;
	int current_direction = RIGHT;

	int idle_frame_index = FRAME_INDEX_RESET_VALUE;
	int walk_frame_index = FRAME_INDEX_RESET_VALUE;
	int fall_frame_index = FRAME_INDEX_RESET_VALUE;
	int died_frame_index = FRAME_INDEX_RESET_VALUE;
	int jump_frame_index = FRAME_INDEX_RESET_VALUE;
	int attack_frame_index = FRAME_INDEX_RESET_VALUE;

	int hero_pos_x = HERO_POS_X_DEFAULT;
	int hero_pos_y = HERO_POS_Y_DEFAULT;

	olc::Pixel* current_active_frame_buf;
	olc::Pixel blank_frame_bufs[HERO_FRAME_WIDTH*HERO_FRAME_HEIGHT];
	olc::Pixel idle_frame_bufs[LEFT_PLUS_RIGHT][IDLE_FRAMES_NUM][HERO_FRAME_WIDTH*HERO_FRAME_HEIGHT];
	olc::Pixel walk_frame_bufs[LEFT_PLUS_RIGHT][WALK_FRAMES_NUM][HERO_FRAME_WIDTH*HERO_FRAME_HEIGHT];
	olc::Pixel fall_frame_bufs[LEFT_PLUS_RIGHT][FALL_FRAMES_NUM][HERO_FRAME_WIDTH*HERO_FRAME_HEIGHT];
	olc::Pixel died_frame_bufs[LEFT_PLUS_RIGHT][DIED_FRAMES_NUM][HERO_FRAME_WIDTH*HERO_FRAME_HEIGHT];
	olc::Pixel jump_frame_bufs[LEFT_PLUS_RIGHT][JUMP_FRAMES_NUM][HERO_FRAME_WIDTH*HERO_FRAME_HEIGHT];
	olc::Pixel attack_frame_bufs[LEFT_PLUS_RIGHT][ATTACK_FRAMES_NUM][HERO_FRAME_WIDTH*HERO_FRAME_HEIGHT];


	void UpdatePosition()
	{
		if(cmd_direction == LEFT)
			hero_pos_x -= PIXELS_SHIFT_PER_WALK;
		if(cmd_direction == RIGHT)
			hero_pos_x += PIXELS_SHIFT_PER_WALK;

		if(current_jumping)
		{
			//jump_frame_index=1,2,3,4 is jumping up
			if(jump_frame_index <= JUMP_FRAMES_NUM/2 )
				hero_pos_y -= PIXELS_SHIFT_PER_JUMP;
			//jump_frame_index=4,5,6,7 is falling down
			else if(jump_frame_index <= JUMP_FRAMES_NUM)
				hero_pos_y += PIXELS_SHIFT_PER_JUMP;
		}

		hit_right_edge = false;
		hit_left_edge = false;

		if(hero_pos_x > HERO_POS_X_MAX)
		{
			hero_pos_x = HERO_POS_X_MAX;
			hit_right_edge = true;
		}
		if(hero_pos_x < HERO_POS_X_MIN)
		{
			hero_pos_x = HERO_POS_X_MIN;
			hit_left_edge = true;
		}

	}

	void UpdateIdle()
	{
		current_active_frame_buf = idle_frame_bufs[current_direction][idle_frame_index];
	}

	void UpdateWalk()
	{
		if(cmd_direction == NONE)
		{
			walk_frame_index = FRAME_INDEX_RESET_VALUE;
			return;
		}

		if(cmd_direction == current_direction &&
		   walk_frame_index == WALK_FRAMES_NUM)
			walk_frame_index = FRAME_INDEX_RESET_VALUE;

		if(cmd_direction != current_direction)
			walk_frame_index = FRAME_INDEX_RESET_VALUE;

		current_active_frame_buf = walk_frame_bufs[cmd_direction][walk_frame_index];
		walk_frame_index++;
	}

	void UpdateJump()
	{
		if(cmd_action != JUMP && !current_jumping)
			return;
		if(cmd_action != JUMP && jump_frame_index == JUMP_FRAMES_NUM)
		{
			current_jumping = false;
			jump_frame_index = FRAME_INDEX_RESET_VALUE;
			return;
		}

		if(cmd_action == JUMP && !current_jumping)
		{
			current_jumping = true;
			jump_frame_index = FRAME_INDEX_RESET_VALUE;
		}

		if(jump_frame_index == JUMP_FRAMES_NUM)
			jump_frame_index = FRAME_INDEX_RESET_VALUE;

		current_active_frame_buf = jump_frame_bufs[current_direction][jump_frame_index];
		jump_frame_index++;
	}

	void UpdateAttack()
	{
		if(cmd_action != ATTACK && !current_attacking)
			return;
		if(cmd_action != ATTACK && attack_frame_index == ATTACK_FRAMES_NUM)
		{
			current_attacking = false;
			attack_frame_index = FRAME_INDEX_RESET_VALUE;
			return;
		}

		if(cmd_action == ATTACK && !current_attacking)
		{
			current_attacking = true;
			attack_frame_index = FRAME_INDEX_RESET_VALUE;
		}

		if(attack_frame_index == ATTACK_FRAMES_NUM)
			attack_frame_index = FRAME_INDEX_RESET_VALUE;

		current_active_frame_buf = attack_frame_bufs[current_direction][attack_frame_index];
		attack_frame_index++;
	}
	void UpdateBlood()
	{
		if(cmd_state == BE_ATTACK)
		{
			blood.Minus();

			if(blood.GetBloodLeft() < 0)
				current_state = DYING;

			if(blood.GetBloodLeft()%2 == 0)
				current_active_frame_buf = blank_frame_bufs;
		}
	}
	void ExecuteCmd()
	{
		// behavior priority be_attack > attack > jump > walk > idle
		UpdateIdle();
		UpdateWalk();
		UpdateJump();
		UpdateAttack();
		UpdateBlood();
		UpdatePosition();
	}

public:
	Blood blood = Blood();
	Hero()
	{
		LoadAllPic();
		for(int i=0;i<HERO_FRAME_WIDTH*HERO_FRAME_HEIGHT;i++)
			blank_frame_bufs[i] = olc::BLANK;
	}

	void LoadAllPic()
	{
		LoadPic("rgba/hero/Death_Left_0.rgba", died_frame_bufs[LEFT][0], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Death_Left_1.rgba", died_frame_bufs[LEFT][1], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Death_Left_2.rgba", died_frame_bufs[LEFT][2], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Death_Left_3.rgba", died_frame_bufs[LEFT][3], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);

		LoadPic("rgba/hero/Death_Right_0.rgba", died_frame_bufs[RIGHT][0], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Death_Right_1.rgba", died_frame_bufs[RIGHT][1], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Death_Right_2.rgba", died_frame_bufs[RIGHT][2], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Death_Right_3.rgba", died_frame_bufs[RIGHT][3], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);

		LoadPic("rgba/hero/Idle_Left_0.rgba", idle_frame_bufs[LEFT][0], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Idle_Left_0.rgba", idle_frame_bufs[LEFT][1], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Idle_Left_0.rgba", idle_frame_bufs[LEFT][2], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Idle_Left_0.rgba", idle_frame_bufs[LEFT][3], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);

		LoadPic("rgba/hero/Idle_Right_0.rgba", idle_frame_bufs[RIGHT][0], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Idle_Right_0.rgba", idle_frame_bufs[RIGHT][1], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Idle_Right_0.rgba", idle_frame_bufs[RIGHT][2], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Idle_Right_0.rgba", idle_frame_bufs[RIGHT][3], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);

		LoadPic("rgba/hero/Walk_Left_0.rgba", walk_frame_bufs[LEFT][0], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Walk_Left_1.rgba", walk_frame_bufs[LEFT][1], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Walk_Left_2.rgba", walk_frame_bufs[LEFT][2], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Walk_Left_3.rgba", walk_frame_bufs[LEFT][3], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);

		LoadPic("rgba/hero/Walk_Right_0.rgba", walk_frame_bufs[RIGHT][0], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Walk_Right_1.rgba", walk_frame_bufs[RIGHT][1], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Walk_Right_2.rgba", walk_frame_bufs[RIGHT][2], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Walk_Right_3.rgba", walk_frame_bufs[RIGHT][3], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);

		LoadPic("rgba/hero/Jump_Left_0.rgba", jump_frame_bufs[LEFT][0], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Jump_Left_1.rgba", jump_frame_bufs[LEFT][1], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Jump_Left_2.rgba", jump_frame_bufs[LEFT][2], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Jump_Left_3.rgba", jump_frame_bufs[LEFT][3], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Fall_Left_0.rgba", jump_frame_bufs[LEFT][4], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Fall_Left_1.rgba", jump_frame_bufs[LEFT][5], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Fall_Left_2.rgba", jump_frame_bufs[LEFT][6], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Fall_Left_3.rgba", jump_frame_bufs[LEFT][7], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);

		LoadPic("rgba/hero/Jump_Right_0.rgba", jump_frame_bufs[RIGHT][0], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Jump_Right_1.rgba", jump_frame_bufs[RIGHT][1], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Jump_Right_2.rgba", jump_frame_bufs[RIGHT][2], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Jump_Right_3.rgba", jump_frame_bufs[RIGHT][3], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Fall_Right_0.rgba", jump_frame_bufs[RIGHT][4], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Fall_Right_1.rgba", jump_frame_bufs[RIGHT][5], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Fall_Right_2.rgba", jump_frame_bufs[RIGHT][6], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Fall_Right_3.rgba", jump_frame_bufs[RIGHT][7], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);

		LoadPic("rgba/hero/Attack_Left_0.rgba", attack_frame_bufs[LEFT][0], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Left_1.rgba", attack_frame_bufs[LEFT][1], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Left_2.rgba", attack_frame_bufs[LEFT][2], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Left_3.rgba", attack_frame_bufs[LEFT][3], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Left_4.rgba", attack_frame_bufs[LEFT][4], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Left_5.rgba", attack_frame_bufs[LEFT][5], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Left_6.rgba", attack_frame_bufs[LEFT][6], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Left_7.rgba", attack_frame_bufs[LEFT][7], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);

		LoadPic("rgba/hero/Attack_Right_0.rgba", attack_frame_bufs[RIGHT][0], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Right_1.rgba", attack_frame_bufs[RIGHT][1], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Right_2.rgba", attack_frame_bufs[RIGHT][2], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Right_3.rgba", attack_frame_bufs[RIGHT][3], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Right_4.rgba", attack_frame_bufs[RIGHT][4], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Right_5.rgba", attack_frame_bufs[RIGHT][5], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Right_6.rgba", attack_frame_bufs[RIGHT][6], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
		LoadPic("rgba/hero/Attack_Right_7.rgba", attack_frame_bufs[RIGHT][7], HERO_FRAME_WIDTH, HERO_FRAME_HEIGHT);
	}

	void Dying()
	{
		current_active_frame_buf = died_frame_bufs[current_direction][died_frame_index];

		if(died_frame_index < DIED_FRAMES_NUM-1)
			died_frame_index++;
		else
			current_state = DIED;
	}

	void Control(int direction, int action, int state)
	{
		if(current_state == DIED || current_state == DYING)
		{
			Dying();
			return;
		}

		cmd_state = state;
		cmd_action = action;
		cmd_direction = direction;

		ExecuteCmd();

		current_action = action;
		if(current_state != DIED && current_state != DYING)
			current_state = cmd_state;
		if(direction != NONE)
			current_direction = direction;
	}

	bool HitRightEdge()
	{
		return hit_right_edge;
	}

	bool HitLeftEdge()
	{
		return hit_left_edge;
	}

	int GetPosX()
	{
		return hero_pos_x;
	}

	int GetPosY()
	{
		return hero_pos_y;
	}

	int GetFrameWidth()
	{
		return HERO_FRAME_WIDTH;
	}

	int GetFrameHeight()
	{
		return HERO_FRAME_HEIGHT;
	}

	int GetDirection()
	{
		return current_direction;
	}
	bool Attacking()
	{
		// only one frame(blade fully out) counted as attacking, other frames as preparing
		if(current_action == ATTACK && attack_frame_index == ATTACK_FRAMES_NUM/2)
			return true;
		return false;
	}

	olc::Pixel* GetFrameBuf()
	{
		return current_active_frame_buf;
	}

};
