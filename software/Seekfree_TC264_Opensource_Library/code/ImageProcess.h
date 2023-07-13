/*
 * ImageProcess.h
 *
 *  Created on: 2023��3��1��
 *      Author: L
 */

#ifndef CODE_IMAGEPROCESS_H_
#define CODE_IMAGEPROCESS_H_

#include "zf_common_headfile.h"
#include "zf_device_mt9v03x.h"
#include "motor.h"
#include "ImageTrack.h"

//============================����================================
#define TRACK_WIDTH         0.4             //�������(m)
#define OUT_THRESHOLD       100             //�����ж���ֵ
#define PROCESS_LENGTH      30              //״̬���ĳ���
//===============================================================

typedef struct ProcessProperty//����Ԫ��֮���һЩ��������
{
    uint8 max_speed;//Ԫ��������Ҫ���ٵ�����
    uint8 min_speed;//Ԫ�ؽ���Ҫ���ٵ�����
    uint16 integral;//���������������ǻ��ֵ�������һ��״̬
    uint8 speed_detaction_flag;//�ٶȾ��ߵķ�ʽ������˵��һ����Ԫ��״̬�������ٶȾ���
}ProcessProperty;

extern uint8 process_status[PROCESS_LENGTH];
extern uint16 process_speed[PROCESS_LENGTH];
extern ProcessProperty process_property[PROCESS_LENGTH];
extern uint8 process_status_cnt;//Ԫ��״̬����ļ�����

void OutProtect(void);
void ImageProcess(void);
void TrackBasicClear(void);//����������Ϣ�������ã�Ϊ��һ֡��׼��
void ProcessPropertyInit(void);//״̬���������Գ�ʼ��
void ProcessPropertyDefault(uint8 i);

#endif /* CODE_IMAGEPROCESS_H_ */
