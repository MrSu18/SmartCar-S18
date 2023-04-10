#include "ImageConversion.h"
#include "math.h"//��ֵ���㷨����Ҫ�õ�pow����

//�궨��
#define PER_IMG     binary_image   //����͸�ӱ任��ͼ��
#define IMAGE_BAN   255             //��͸�ӽ�ֹ����ĻҶ�ֵ
#define PERSPECTIVE 1               //͸�Ӵ���̶�ѡ�� 0:����ͼ����͸�� 1:ͼ����͸��

//�������
uint8* PerImg_ip[PER_IMAGE_H][PER_IMAGE_W];//��ά���飨Ԫ����ָ��������ڴ洢ӳ������ص�ַ��
uint8 binary_image[MT9V03X_H][MT9V03X_W]={0};//��ֵ��ͼ��
uint8 left_border[USE_IMAGE_H] = {0};//ͼ����߽�
uint8 right_border[USE_IMAGE_H] = {USE_IMAGE_W-1};//ͼ���ұ߽�

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

#if PERSPECTIVE==1    //ֻ������͸�Ӳ�����ȥ���䣬ע����͸�Ӿ����Ƿ���ȷ
void ImagePerspective_Init(void) 
{
    static uint8 BlackColor = IMAGE_BAN;
    //��͸�Ӿ���
    double change_un_Mat[3][3] ={{-5.646070,6.116770,-158.737697},{0.282977,2.882744,-258.992498},{0.004492,0.066291,-4.829664}};
    for (int i = 0; i < PER_IMAGE_W; i++)
    {
        for (int j = 0; j < PER_IMAGE_H; j++)
        {
            int local_x = (int)((change_un_Mat[0][0]*i+change_un_Mat[0][1]*j+change_un_Mat[0][2])
                               /(change_un_Mat[2][0]*i+change_un_Mat[2][1]*j+change_un_Mat[2][2]));
            int local_y = (int)((change_un_Mat[1][0]*i+change_un_Mat[1][1]*j+change_un_Mat[1][2])
                               /(change_un_Mat[2][0]*i+change_un_Mat[2][1]*j+change_un_Mat[2][2]));
            if (local_x>= 0 && local_y >= 0 && local_y < MT9V03X_H && local_x < MT9V03X_W) 
            {
                PerImg_ip[j][i] = &PER_IMG[local_y][local_x];
            }
            else 
            {
                PerImg_ip[j][i] = &BlackColor;          //&PER_IMG[0][0];
            }
        }
    }
}
#else
void ImagePerspective_Init(void){;}
#endif  //PERSPECTIVE

/***********************************************
* @brief : ��͸��ͼ��߽��ʼ��
* @param : ��͸��ͼ�񡢿�ȡ��߶ȡ������������ĻҶ�ֵ
* @return: ��������ͼ��߽��һά����
* @date  : 2022.9.1
* @author: ������
************************************************/
void ImageBorderInit(void)
{
#if PERSPECTIVE==0
    memset(left_border,0,sizeof(left_border[0])*USE_IMAGE_H);
    memset(right_border,USE_IMAGE_W-1,sizeof(right_border[0])*USE_IMAGE_H);
#else
    //��̬�ֲ��������ڿ����߽����������
    static uint8 left_x=USE_IMAGE_W/2,right_x=USE_IMAGE_W/2;
    for (uint8 row = USE_IMAGE_H - 1; row > 0; row--)
    {
        //�м䵽���
        for (uint8 l_column = left_x; l_column > 0; l_column--)
        {
            if (use_image[row][l_column] == IMAGE_BAN)
            {
                left_x = l_column + 2;//�������������ڱ߽�׷��
                left_border[row]= l_column + 1;
                break;
            }
            else if (l_column - 1 == 0)//�������������˻���û�ҵ��߽磬��ô�Ͱ�1��Ϊ��߽�
            {
                left_border[row] = left_border[row + 1];
            }
        }
        //�м䵽�ұ�
        for (uint8 r_column = right_x; r_column < USE_IMAGE_W - 1; r_column++)
        {
            if (use_image[row][r_column] == IMAGE_BAN)
            {
                right_x = r_column - 2;
                right_border[row]= r_column - 2;
                break;
            }
            else if (r_column + 1 == USE_IMAGE_W - 1)//ͬ��������ұ߻�û�ж�������W-1��Ϊ�ұ߽�
            {
                right_border[row] = right_border[row + 1];
            }
        }
    }
#endif  //PERSPECTIVE͸�ӳ̶�
}

