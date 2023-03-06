//
// Created by 30516 on 2023/3/6.
//

#ifndef LJF_OPENCV_IMAGETEST_IMAGEPROCESS_H
#define LJF_OPENCV_IMAGETEST_IMAGEPROCESS_H

//============================参数================================
#define TRACK_WIDTH         0.4             //赛道宽度(m)
#define OUT_THRESHOLD       100             //出界判断阈值
//===============================================================

void ImageProcess(void);
void TrackBasicClear(void);//赛道基础信息变量重置，为下一帧做准备

#endif //LJF_OPENCV_IMAGETEST_IMAGEPROCESS_H
