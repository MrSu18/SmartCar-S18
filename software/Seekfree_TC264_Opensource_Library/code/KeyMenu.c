/*
 * KeyMenu.c
 *
 *  Created on: 2023��5��11��
 *      Author: L
 */
#include "key.h"
#include "zf_common_headfile.h"

typedef struct Menu
{
        uint8 MenuNum;
        uint8 MenuFun[];
}Menu;
