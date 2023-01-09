//
// Created by 30516 on 2023/1/9.
//

#ifndef LJF_OPENCV_IMAGETEST_IMAGETRACK_H
#define LJF_OPENCV_IMAGETEST_IMAGETRACK_H

#include "ImageBasic.h"//mypoint结构体

void blur_points(myPoint* in_line, int num, myPoint* out_line, int kernel);
void TrackEdgeGetCenterLine(char choose);//左右半宽巡线得到中线

#endif //LJF_OPENCV_IMAGETEST_IMAGETRACK_H
