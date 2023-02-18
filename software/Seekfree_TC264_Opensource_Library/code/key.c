/*
 * key.c
 *
 *  Created on: 2023Äê2ÔÂ2ÈÕ
 *      Author: L
 */
#include "key.h"

void KEYInit(void)
{
    gpio_init(P33_10,GPI,0,GPI_PULL_DOWN);
    gpio_init(P33_11,GPI,0,GPI_PULL_DOWN);
    gpio_init(P33_12,GPI,0,GPI_PULL_DOWN);
    gpio_init(P33_13,GPI,0,GPI_PULL_DOWN);
    gpio_init(P32_4,GPI,0,GPI_PULL_DOWN);
}

