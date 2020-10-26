/*
 * common.h
 *
 *  Created on: 2020年7月30日
 *      Author: dan
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <iostream>

#define BYTE_NUM_PER_PIXEL 4

#define GameWindowWidth  384
#define GameWindowHeight 216

#define MEADOW_HEIGHT 30
#define MEADOW_SLIDE_SPEED  5

#define LEFT_PLUS_RIGHT 2
#define FRAME_INDEX_RESET_VALUE 0

#define SAFE_DISTANCE 100
#define ENCOUNTER_AJUST 5
#define MONSTER_DYING_COST_TIME 20


// don't add item at left end, add it at right end before NONE
enum DIRECTION {LEFT, RIGHT, NONE};
enum ACTION    {IDLE,  JUMP, ATTACK};
enum STATE     {FREE, STUCK, BE_ATTACK, DYING, DIED};
enum MONSTER_TYPE   {DEMON};


#endif /* COMMON_H_ */
