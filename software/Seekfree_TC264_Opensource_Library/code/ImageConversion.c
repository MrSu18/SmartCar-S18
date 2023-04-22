#include "ImageConversion.h"
#include "math.h"//��ֵ���㷨����Ҫ�õ�pow����

//�������
uint8 binary_image[MT9V03X_H][MT9V03X_W]={0};//��ֵ��ͼ��
uint8 left_border[USE_IMAGE_H] = {0};//ͼ����߽�
uint8 right_border[USE_IMAGE_H] = {USE_IMAGE_W-1};//ͼ���ұ߽�
uint8 otsu_thr=0;//��򷨶�sobel�㷨֮���ͼ�������ֵ

/***********************************************
* @brief : ��򷨶�ֵ��0.8ms����ʵ�ʲ���4ms��TC264�У�
* @param : *image ��ͼ���ַ
*           width:  ͼ���
*           height��ͼ���
* @return: ��
* @date  : 2018-10
* @author: ZС��
************************************************/
uint8 otsuThreshold(uint8* image, uint16 width, uint16 height)
{
#define GrayScale 256 //��߻Ҷȼ�
    int pixelCount[GrayScale] = { 0 };//ÿ���Ҷ�ֵ��ռ���ظ���
    float pixelPro[GrayScale] = { 0 };//ÿ���Ҷ�ֵ��ռ�����ر���
    int i, j;
    int Sumpix = (width/2 * (106-55));   //�����ص�
    uint8 threshold = 0;
    uint8* data = image;  //ָ���������ݵ�ָ��

    //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    for (i = 55; i < 106; i++)
    {
        for (j = 0; j < width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //������ֵ��Ϊ����������±�
          //   pixelCount[(int)image[i][j]]++;    ������ָ�������
        }
    }
    float u = 0;
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / Sumpix;   //����ÿ������������ͼ���еı���
        u += i * pixelPro[i];  //��ƽ���Ҷ�
    }

    float maxVariance = 0.0;  //�����䷽��
    float w0 = 0, avgValue = 0;  //w0 ǰ������ ��avgValue ǰ��ƽ���Ҷ�
    for (int i = 0; i < 256; i++)     //ÿһ��ѭ������һ��������䷽����� (����for����Ϊ1��)
    {
        w0 += pixelPro[i];  //���赱ǰ�Ҷ�iΪ��ֵ, 0~i �Ҷ�������ռ����ͼ��ı�����ǰ������
        avgValue += i * pixelPro[i];

        float variance = pow((avgValue / w0 - u), 2) * w0 / (1 - w0);    //��䷽��
        if (variance > maxVariance)
        {
            maxVariance = variance;
            threshold = (uint8)i;
        }
    }
    return threshold;
}
/***********************************************
* @brief : ȫͼ������ֵ��ȫͼ���ж�ֵ��
* @param : �Ҷ�ͼ��ȫ�ֱ�����
* @return: �Ҷ�ͼ���ɶ�ֵ��ͼ��ȫ�ֱ�����
* @date  : 2021.12.15
* @author: ������
************************************************/
void ImageBinary(void)
{
    //ѹ��ͼ��Ķ�ֵ��
    uint8 Image_Threshold = otsuThreshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);//ʹ�ô�򷨵õ���ֵ����ֵ
    for (int i = 0; i < MT9V03X_H; i++)
    {
        for (int j = 0; j < MT9V03X_W; j++)
        {
            if (mt9v03x_image[i][j] <= Image_Threshold)//���ж�ֵ��֮ǰֻ�ǵõ���ֵ
                binary_image[i][j] = IMAGE_BLACK;//0�Ǻ�ɫ  //ͼ��ԭ�㲻��
            else
                binary_image[i][j] = IMAGE_WHITE;//1�ǰ�ɫ  //ͼ��ԭ�㲻��
        }
    }
}

void sobel(uint8_t imag[MT9V03X_H][MT9V03X_W],uint8_t imag1[MT9V03X_H][MT9V03X_W])
{
    int tempx=0,tempy=0,temp=0,i=0,j=0;
    for(i=1;i <MT9V03X_H-1; i++)
    {
        for(j=1;j<MT9V03X_W-1;j++)
        {

            tempx=((-  imag[i-1][j-1])+(-2*imag[i  ][j-1])+(-  imag[i+1][j-1])
                  +(   imag[i-1][j+1])+( 2*imag[i  ][j+1])+(   imag[i+1][j+1]))/4;
            if(tempx<0)
                tempx=-tempx;

            tempy=((   imag[i+1][j-1])
                  +( 2*imag[i+1][j  ])
                  +(   imag[i+1][j+1])
                  +(-  imag[i-1][j-1])
                  +(-2*imag[i-1][j  ])
                  +(-  imag[i-1][j+1]))/4;
            if(tempy<0)
                tempy=-tempy;
            temp=(tempx+tempy)/2;
            if(temp>255) temp=255;
            if(otsu_thr!=0)
            {
                if(temp>otsu_thr) imag1[i][j]=0;
                else          imag1[i][j]=255;
            }
            else
                imag1[i][j]=temp;
        }
    }
}
