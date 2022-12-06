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
* @param : uint8* image[]:�洢ͼ���е�ַ��һάָ�����飨����ά����Ԫ��Ϊָ�������
*		   uint16 width�����
*		   uint16 height���߶�
* @return: ��ʾͼ��
* @date  : 2022.8.29
* @author: ������
************************************************/
void PrintImage(uint8* image[], uint16 width, uint16 height)//��һ����ά������ʾͼ��
{
	//����ά����ת��Ϊͼ����ʾ
	uchar* ptmp = NULL;//���ǹؼ���ָ�룡��
	Mat img = Mat(height, width, CV_8UC1);//ͼ��img2��row*col��С
	for (int i = 0; i < height; i++)
	{
		ptmp = img.ptr<uchar>(i);//ָ��ָ��img2�ĵ�i��
		for (int j = 0; j < width; j++)
		{
			ptmp[j] = *(image[i * width + j]);//��ά�������ݴ���img2�ĵ�i�е�j��
			//ptmp[j] = binary_image[i][j];//��ά�������ݴ���img2�ĵ�i�е�j��
		}
	}
	// �Ŵ�ͼ�񷽱�鿴
	int down_width = USE_IMAGE_W * 5;
	int down_height = USE_IMAGE_H * 5;
	Mat resized_image;//������С֮���ͼ��
	resize(img, resized_image, Size(down_width, down_height), INTER_LINEAR);
	//��ʾͼ��
	imshow("imag", resized_image);
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
    Mat img = imread(str);
    Mat gray_img;
    //��ɫͨ��ת�� BGR-->Gray
    cvtColor(img, gray_img, COLOR_BGR2GRAY);//�����㶼�������ǲ��Ƕ�ҪתΪ��ͨ����ͼ���У���������Ƕ�bmpҲ����ͨ��
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
    imshow("ԭʼ�Ҷ�ͼ",img);
}
