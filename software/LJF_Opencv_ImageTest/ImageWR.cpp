/*===========================================
== ��װOpenCV�������ڶ�дͼ�����DEBUG
== ���ߣ�������
=============================================*/

#include<opencv2/opencv.hpp>
#include "ImageWR.h"
#include "ImageConversion.h"

using namespace cv;

/***********************************************
* @brief : ����ά������ΪͼƬ��ʾ
* @param : cv::Mat img��Ҫ��ʾ��mat��
* @return: ��ʾͼ��
* @date  : 2022.12.31
* @author: ������
************************************************/
void PrintImage(cv::Mat img)
{
	//��ʾͼ��
    namedWindow("use_img",WINDOW_NORMAL);//����imag���ڵ�������Ϊ���������Ƶ������ڴ�С������ʹ��imshowĬ�ϵĲ�����WINDOW_AUTOSIZE��ֻ�̶ܹ���С
	imshow("use_img", img);
	waitKey(0);//�ȴ����̰���
}

/***********************************************
* @brief : ��ȡͼ��ģ�����������ͷ�ɼ�ͼƬ
* @param : String str��ͼƬ�ľ���·��
* @return: ��ʾͼ��
* @date  : 2022.12.2
* @author: ������
************************************************/
void ScanImage(cv::String str)
{
    //��ȡͼ��
    Mat gray_img = imread(str,IMREAD_GRAYSCALE);//�ԻҶ�ͼ����ʽ��ȡͼ��
    //��ͼ��ת��Ϊ��ά����
    int i = 0, j = 0;
    int row = gray_img.rows;
    int col = gray_img.cols;
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
        {
            mt9v03x_image[i][j] = gray_img.at<uchar>(i, j);//img�ľ������ݴ�����ά����ptr[][]
        }
    }
    namedWindow("ԭʼ�Ҷ�ͼ",WINDOW_NORMAL);
    imshow("ԭʼ�Ҷ�ͼ",gray_img);
}

/***********************************************
* @brief : ������matֵ��͸�Ӿ����ֵ
* @param : ��
* @return: ��
* @date  : 2022.12.31
* @author: ������
************************************************/
Mat use_mat;//��ͼ������ʹ�õĶ�άָ��������������
void UseImageDataToUseMat()
{
    Mat img=Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC1);//��ʱ�ı��������ڽ���ͼ������
    use_mat=Mat(USE_IMAGE_H, USE_IMAGE_W, CV_8UC3);//��ȫ�ֱ���mat����ʵ����

    for (int i = 0; i < USE_IMAGE_H; i++)
    {
        for (int j = 0; j < USE_IMAGE_W; j++)
        {
            img.at<uchar>(i, j)=*PerImg_ip[i][j];//��ά�������ݴ���img2�ĵ�i�е�j��
        }
    }
    //��ɫͨ��ת�� Gray-->BGR!!!!,��������û��ɫ
    cvtColor(img, use_mat, COLOR_GRAY2BGR);
}

void LCDDrawPoint(uint8 row, uint8 column,uint8 R,uint8 G,uint8 B)
{
    //����
    Point a;
    a.x = column; a.y = row;
    circle(use_mat, a, 0, Scalar(B, G, R), -1); //�������������Ϊ-1���������Ǹ�ʵ�㡣
    //��ʾͼ��
    imshow("use_img", use_mat);
    waitKey(0);//�ȴ����̰���
}

void LCDDrowColumn(uint8 column,uint8 R,uint8 G,uint8 B)
{
    for (int i = 0; i < USE_IMAGE_H; ++i)
    {
        LCDDrawPoint(i,column,R,G,B);
    }
}

void LCDDrowRow(uint8 row,uint8 R,uint8 G,uint8 B)
{
    for (int i = 0; i < USE_IMAGE_W; ++i)
    {
        LCDDrawPoint(row,i,R,G,B);
    }
}
