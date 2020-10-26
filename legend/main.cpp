#define OLC_PGE_APPLICATION
#include <math.h>
#include <stdio.h>
#include <iostream>
#include "common.h"
#include "olcPixelGameEngine.h"

#include "hero.cpp"
#include "monster.cpp"
#include "background.cpp"


class Legend : public olc::PixelGameEngine
{
private:
	Hero hero = Hero();
	int monster_num = 0;
	Monster* monster_list[100] = {0};

	Background background = Background();

	void Render(int start_x, int start_y, int width, int height, olc::Pixel* frame_buf)
	{
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++)
			{
				if(frame_buf[y*width+x] != olc::BLANK)
					Draw(start_x+x, start_y+y, frame_buf[y*width+x]);
			}
	}

	int DirectionKeyBeHeld()
	{
		olc::HWButton buttonState;

		buttonState = GetKey(olc::A);
		if(buttonState.bPressed || buttonState.bHeld)
			return LEFT;

		buttonState = GetKey(olc::D);
		if(buttonState.bPressed || buttonState.bHeld)
			return RIGHT;

		return NONE;
	}

	int ActionKeyBeHeld()
	{
		olc::HWButton buttonState;

		buttonState = GetKey(olc::J);
		if(buttonState.bPressed || buttonState.bHeld)
			return JUMP;

		buttonState = GetKey(olc::K);
		if(buttonState.bPressed || buttonState.bHeld)
			return ATTACK;

		return IDLE;
	}

public:
	Legend()
	{
		sAppName = "Legend";
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		monster_list[0] = CreateMonster(DEMON,-200);
		monster_list[1] = CreateMonster(DEMON,-100);
		monster_list[2] = CreateMonster(DEMON,0);
		monster_list[3] = CreateMonster(DEMON,100);
		monster_list[4] = CreateMonster(DEMON,200);
		monster_num = 5;

		return true;
	}

	void SlideAllMonsters(int direction)
	{
		for(int i=0; i<monster_num; i++)
			monster_list[i]->SlidePixels(direction,MEADOW_SLIDE_SPEED);
	}

	bool EncounterMonstersFromRight()
	{
		int monster_pos_x = 0;
		int monster_width = 0;
		int hero_pos_x = hero.GetPosX();

		for(int i=0; i<monster_num; i++)
		{
			if(monster_list[i]->GetState() == DIED || monster_list[i]->GetState() == DYING)
				continue;

			monster_pos_x = monster_list[i]->GetPosX();
			monster_width = monster_list[i]->GetFrameWidth();

			if(hero_pos_x - monster_pos_x < monster_width-ENCOUNTER_AJUST &&
			   hero_pos_x - monster_pos_x > 0)
				return true;
		}
		return false;
	}
	bool EncounterMonstersFromLeft()
	{
		int monster_pos_x = 0;
		int hero_pos_x = hero.GetPosX();
		int hero_width = hero.GetFrameWidth();

		for(int i=0; i<monster_num; i++)
		{
			if(monster_list[i]->GetState() == DIED || monster_list[i]->GetState() == DYING)
				continue;

			monster_pos_x = monster_list[i]->GetPosX();

			if(monster_pos_x - hero_pos_x < hero_width-ENCOUNTER_AJUST &&
			   monster_pos_x - hero_pos_x > 0)
				return true;
		}
		return false;
	}

	void ControlMonsters()
	{
		int monster_pos_x = 0;
		int hero_pos_x = hero.GetPosX();

		for(int i=0; i<monster_num; i++)
		{
			if(monster_list[i]->GetState() == DIED)
				continue;

			monster_pos_x = monster_list[i]->GetPosX();

			if ((hero_pos_x - monster_pos_x < SAFE_DISTANCE) &&
				(hero_pos_x - monster_pos_x > 0))
			{
				if(EncounterMonstersFromRight() && hero.GetDirection() == LEFT && hero.Attacking())
					monster_list[i]->Control(RIGHT, BE_ATTACK);
				else if(EncounterMonstersFromRight())
					monster_list[i]->Control(RIGHT, STUCK);
				else
					monster_list[i]->Control(RIGHT, FREE);
			}
			else if ((monster_pos_x - hero_pos_x < SAFE_DISTANCE) &&
					 (monster_pos_x - hero_pos_x > 0))
			{
				if(EncounterMonstersFromLeft() && hero.GetDirection() == RIGHT && hero.Attacking())
					monster_list[i]->Control(LEFT, BE_ATTACK);
				else if(EncounterMonstersFromLeft())
					monster_list[i]->Control(LEFT, STUCK);
				else
					monster_list[i]->Control(LEFT, FREE);
			}
			else
				monster_list[i]->Control(NONE, FREE);
		}
	}

	void ControlHero()
	{
		int cmd_action = ActionKeyBeHeld();
		int cmd_direction = DirectionKeyBeHeld();

		if(EncounterMonstersFromRight() && cmd_direction == LEFT && hero.GetDirection() == LEFT)
			hero.Control(NONE,cmd_action, BE_ATTACK);
		else if(EncounterMonstersFromLeft() && cmd_direction == RIGHT && hero.GetDirection() == RIGHT)
			hero.Control(NONE,cmd_action, BE_ATTACK);
		else if(EncounterMonstersFromRight() || EncounterMonstersFromLeft())
			hero.Control(cmd_direction,cmd_action, BE_ATTACK);
		else
			hero.Control(cmd_direction,cmd_action,FREE);
	}

	void ControlBackground()
	{
		if(hero.HitLeftEdge())
		{
			background.Slide(RIGHT);
			SlideAllMonsters(RIGHT);
		}
		if(hero.HitRightEdge())
		{
			background.Slide(LEFT);
			SlideAllMonsters(LEFT);
		}
	}

	void DisplayBackground()
	{
		Render(0,0,GameWindowWidth,GameWindowHeight,background.GetFrameBuf());
	}

	void DisplayHero()
	{
		Render(hero.GetPosX(),
			   hero.GetPosY(),
			   hero.GetFrameWidth(),
			   hero.GetFrameHeight(),
			   hero.GetFrameBuf());

		Render(hero.blood.GetPosX(),
			   hero.blood.GetPosY(),
			   hero.blood.GetFrameWidth(),
			   hero.blood.GetFrameHeight(),
			   hero.blood.GetFrameBuf());
	}

	void DisplayMonsters()
	{
		for(int i=0; i<monster_num; i++)
		{
			if(monster_list[i]->GetState() != DIED)
				Render(monster_list[i]->GetPosX(),
					   monster_list[i]->GetPosY(),
					   monster_list[i]->GetFrameWidth(),
					   monster_list[i]->GetFrameHeight(),
					   monster_list[i]->GetFrameBuf());
		}
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		static int interval = 6;
		if(--interval == 0)
		{
			interval = 6;

			ControlHero();
			ControlMonsters();
			ControlBackground();

			DisplayBackground();
			DisplayHero();
			DisplayMonsters();
		}

		return true;
	}
};


int main()
{
	Legend demo;
	if (demo.Construct(GameWindowWidth, GameWindowHeight, 4, 4))
		demo.Start();

	return 0;
}


