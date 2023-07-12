#include "ImageConversion.h"
#include "math.h"//��ֵ���㷨����Ҫ�õ�pow����

//�������
uint8 binary_image[MT9V03X_H][MT9V03X_W]={0};//��ֵ��ͼ��
uint8 gray_image[MT9V03X_H][MT9V03X_W]={0};//ʹ�õĻ���Ҷ�ͼ��,������Ϊdma��ͼ�����
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
* @brief : sobel��ֵ��
* @param : uint8_t imag[MT9V03X_H][MT9V03X_W]:�Ҷ�ͼ
*          uint8_t imag1[MT9V03X_H][MT9V03X_W]:��ֵ��ͼ
* @return: ��
* @date  : 2023.4.16
* @author: ������
************************************************/
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

/********************************************************************************************
 ** ��������: ����Ӧ��ֵ��ֵ��ͼ��
 ** ��    ��: uint8* img_data���Ҷ�ͼ��
 **           uint8* output_data����ֵ��ͼ��
 **           int width��ͼ����
 **           int height��ͼ��߶�
 **           int block���ָ�ֲ���ֵ�ķ����С����7*7
 **           uint8 clip_value: �ֲ���ֵ��ȥ�ľ���ֵһ��Ϊ��2~5��
 ** �� �� ֵ: ��
 ** ��    ��: �Ϻ�����16�����ܳ������Ӿ���SJTUAuTop
 **           https://zhuanlan.zhihu.com/p/391051197
 ** ע    �⣺adaptiveThreshold(mt9v03x_image[0],BinaryImage[0],MT9V03X_W,MT9V03X_H,5,1);//����ûd�ø����һ��
 *********************************************************************************************/
void myadaptiveThreshold(uint8 *img_data, uint8 *output_data, int width, int height, int block, uint8 clip_value)
{
    int half_block = block / 2;
    for(int y=half_block; y<height-half_block; y++)
    {
        for(int x=half_block; x<width-half_block; x++)
        {
            // ����ֲ���ֵ
            int thres = 0;
            for(int dy=-half_block; dy<=half_block; dy++)
            {
                for(int dx=-half_block; dx<=half_block; dx++)
                {
                    thres += img_data[(x+dx)+(y+dy)*width];
                }
            }
            thres = thres / (block * block) - clip_value;
            // ���ж�ֵ��
            output_data[x+y*width] = img_data[x+y*width]>thres ? 255 : 0;
        }
    }
}
