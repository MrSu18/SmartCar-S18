#include<opencv2/opencv.hpp>
#include<iostream>
#include <string>
#include "main.h"
#include "ImageConversion.h"
#include "ImageBasic.h"
#include"ImageWR.h"

using namespace cv;

uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];

int main()
{
	//��ʼ����͸��ͼ��ѹ��
	ImagePerspective_Init();
	ImageBorderInit();
	BlackBorder();

	/******************************************������ȡ�Ҷ�ͼ***************************************/
    String str = "E:\\nodeanddata\\studio\\FSL\\Complete\\S18\\data\\image\\11.30\\3.B.bmp";
    ScanImage(str);
	/************************************************************************************************/


    //��ֵ��
	ImageBinary();
	PrintImage(use_image, USE_IMAGE_W, USE_IMAGE_H);
	//ɨ��
	EdgeDetection();
	//Ѱ�ڿ�
	CaculateBlackBlock();
	/************************************************************************************************************/


	//����ά����ת��Ϊͼ����ʾ
   	uchar* ptmp = NULL;//���ǹؼ���ָ�룡��
	Mat gray_img2 = Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC1);//ͼ��img2��row*col��С
	for (int i = 0; i < USE_IMAGE_H; i++)
	{
		ptmp = gray_img2.ptr<uchar>(i);//ָ��ָ��img2�ĵ�i��
		for (int j = 0; j < USE_IMAGE_W; j++)
		{
			ptmp[j] = use_image[i][j];//��ά�������ݴ���img2�ĵ�i�е�j��
		}
	}
	Mat img2 = Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC3);//�������ɫ��ͼ���࣬8UC3������ͨ��8bit�޷���
	//��ɫͨ��ת�� Gray-->BGR!!!!,��������û��ɫ
	cvtColor(gray_img2, img2, COLOR_GRAY2BGR);
	//�����߻�����
	Point a;
	for (uint8 i = USE_IMAGE_H - 1; i > 0; i--)//bgr
	{
		a.x = left_line[i]; a.y = i;
		circle(img2, a, 0, Scalar(0, 255, 0), -1); //�������������Ϊ-1���������Ǹ�ʵ�㡣
		a.x = right_line[i]; a.y = i;
		circle(img2, a, 0, Scalar(0, 0, 255), -1);
		a.x = center_line[i]; a.y = i;
		circle(img2, a, 0, Scalar(255, 0, 0), -1);
	}
//	for (uint8 i = USE_IMAGE_W - 1; i > 0; i--)
//	{
//		a.x = i; a.y = left_line_y[i];
//		circle(img2, a, 0, Scalar(255, 255, 0), -1); //�������������Ϊ-1���������Ǹ�ʵ�㡣
//		a.x = i; a.y = right_line_y[i];
//		//circle(img2, a, 0, Scalar(255, 255, 0), -1);
//	}
	// �Ŵ�ͼ�񷽱�鿴
	int down_width = USE_IMAGE_W * 5;
	int down_height = USE_IMAGE_H * 5;
	Mat resized_image;//������С֮���ͼ��
	resize(img2, resized_image, Size(down_width, down_height), INTER_LINEAR);
	//��ʾͼ��
	imshow("imag", resized_image);
	waitKey(0);//�ȴ����̰���
	return 0;
}

void LCDDrawPoint(uint8 row, uint8 column)
{
	//����ά����ת��Ϊͼ����ʾ
	uchar* ptmp = NULL;//���ǹؼ���ָ�룡��
	Mat gray_img2 = Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC1);//ͼ��img2��row*col��С
	for (int i = 0; i < USE_IMAGE_H; i++)
	{
		ptmp = gray_img2.ptr<uchar>(i);//ָ��ָ��img2�ĵ�i��
		for (int j = 0; j < USE_IMAGE_W; j++)
		{
			ptmp[j] = use_image[i][j];//��ά�������ݴ���img2�ĵ�i�е�j��
		}
	}
	Mat img2 = Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC3);//�������ɫ��ͼ���࣬8UC3������ͨ��8bit�޷���
	//��ɫͨ��ת�� Gray-->BGR!!!!,��������û��ɫ
	cvtColor(gray_img2, img2, COLOR_GRAY2BGR);
	//����
	Point a;
	a.x = column; a.y = row;
	circle(img2, a, 0, Scalar(32, 255, 0), -1); //�������������Ϊ-1���������Ǹ�ʵ�㡣
	// �Ŵ�ͼ�񷽱�鿴
	int down_width = USE_IMAGE_W * 5;
	int down_height = USE_IMAGE_H * 5;
	Mat resized_image;//������С֮���ͼ��
	resize(img2, resized_image, Size(down_width, down_height), INTER_LINEAR);
	//��ʾͼ��
	imshow("imag", resized_image);
	waitKey(0);//�ȴ����̰���
}