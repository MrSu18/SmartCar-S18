/*
 * Copyright (C), 1988-1999, Xxxxxx Tech. Co., Ltd.
 * FileName: ZwPlus_lib.c
 * Description: ����ͼ��PLUS�⺯��
 * Change Logs:
  |Date          |Author       |Notes               |version
  |2022-10-29    |���Ļ�ϲ     |Initial build       |1.0.0
 */

/* @include */
#include "ZwPlus_lib.h"

/**
  * @name    Zw_SendImage
  * @brief   ͼ�����ͺ���
  * @param   Zw_Image ͼ���׵�ַ
  */
void Zw_SendImage(Zw_uint8* Zw_Image)
{
    Zw_uint8 dat;
    Zw_uint16 i, j;
    if(Zw_Image == Zw_NULL) return;
    for(i = 0; i < Zw_Image_H * Zw_Image_W; i += 8)
    {
        dat = 0;
        for(j = 0; j < 8; j++)
        {
            if(*(Zw_Image + i + j))    dat |= 0x01 << (7-j);
        }
        Zw_Putchar(dat);
    }
}

