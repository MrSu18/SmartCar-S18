#include "main.h"
#include<opencv2/opencv.hpp>

extern cv::Mat use_mat;

void PrintImage(cv::Mat img);
void ScanImage(cv::String str);//��ȡͼ��ģ�����������ͷ�ɼ�ͼƬ
void UseImageDataToUseMat();//��͸�Ӿ����ֵ��mat����ֱ������������ʹ��opencv�ĺ���
