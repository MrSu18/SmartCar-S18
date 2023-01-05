#include "ImageConversion.h"
#include "ImageWR.h"
#include "math.h"//��ֵ���㷨����Ҫ�õ�pow����
#include <string>

//�궨��
#define PER_IMG     binary_image//SimBinImage:����͸�ӱ任��ͼ��
#define IMAGE_BAN   127//��͸�ӽ�ֹ����ĻҶ�ֵ

//�������
uint8* PerImg_ip[PER_IMAGE_H][PER_IMAGE_W];//��ά���飨Ԫ����ָ��������ڴ洢ӳ������ص�ַ��
uint8 binary_image[MT9V03X_H][MT9V03X_W]={0};//��ֵ�����ͼ��
uint8 left_border[USE_IMAGE_H] = {0};//ͼ����߽�
uint8 right_border[USE_IMAGE_H] = {USE_IMAGE_W-1};//ͼ���ұ߽�

extern uint8 left_line_x[USE_IMAGE_H], right_line_x[USE_IMAGE_H];//����������

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
    int Sumpix = width * height;   //�����ص�
    uint8 threshold = 0;
    uint8* data = image;  //ָ���������ݵ�ָ��

    //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
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

//���ݳ����������ô�򷨻�ȵ���Сֵ�õ���ֵ����ֵȻ����ݻҶ�ͼ�õ��ڰ�ͼ��
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

/***********************************************
* @brief : ��ʼ����͸��ָ�����飬�õ���Ӧ��ͼ�����ص�ӳ��
* @param : ԭͼ��ȫ�ֱ�����
* @return: ��͸��֮��Ķ�άָ��ӳ�䣨ȫ�ֱ�����
* @date  : 2022.8.28
* @author: ��ʨ��
************************************************/
void ImagePerspective_Init(void) 
{
    static uint8 BlackColor = IMAGE_BAN;
    double change_un_Mat[3][3] = { {0.316220,-0.232893,2.311183},{0.000132,0.012651,3.418808},{0.000307,-0.002723,0.332779} };

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

/***********************************************
* @brief : ��͸��ͼ��߽��ʼ��
* @param : ��͸��ͼ�񡢿�ȡ��߶ȡ������������ĻҶ�ֵ
* @return: ��������ͼ��߽��һά����
* @date  : 2022.9.1
* @author: ������
************************************************/
void ImageBorderInit(void)
{
    static uint8 black=IMAGE_BLACK;//Ҫ������̬�ֲ�������ȷ���ڴ治�ᱻ�ͷ�
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
                //�ҵ��߽�֮���ʣ�µĻ�ɫ����Ϳ�ڣ����������������ܻ���BUG
                for (int i = left_border[row]; i > 0; i--)
                {
                    PerImg_ip[row][i]=&black;
                }
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
                right_border[row]= r_column - 1;
                //�ҵ��߽�֮���ʣ�µĻ�ɫ����Ϳ�ڣ����������������ܻ���BUG
                for (int i = right_border[row]; i < USE_IMAGE_W - 1; i++)
                {
                    PerImg_ip[row][i]=&black;
                }
                break;
            }
            else if (r_column + 1 == USE_IMAGE_W - 1)//ͬ��������ұ߻�û�ж�������W-1��Ϊ�ұ߽�
            {
                right_border[row] = right_border[row + 1];
            }
        }
    }
    //����߽�֮������һ���������������ǵĳ�ʼֵ���ڱ߽�
    for (uint8 row = 0; row < USE_IMAGE_H; row++)
    {
        left_line_x[row] = left_border[row];
        right_line_x[row] = right_border[row];
    }
}

/***********************************************
* @brief : ��ͼ��߽�Ϳ�ڣ��ڳ�ʼ����ʱ������һ�鼴�ɣ�ָ�����ָ��ĵ�ַ��һֱ�Ǻ�ɫ��һȦ��
* @param : ��͸��֮��Ķ�ֵ��ͼ��
* @return: �߽�һȦ�ǺڵĶ�ֵ��ͼ��
* @date  : 2022.9.15
* @author: ������
************************************************/
void BlackBorder(void)
{
    static uint8 black=IMAGE_BLACK;//Ҫ������̬�ֲ�������ȷ���ڴ治�ᱻ�ͷ�
    //Ϳ���������ߵı߽�
    for (uint8 row = 0; row < USE_IMAGE_H; row++)
    {   
        //LCDDrawPoint(row, left_border[row]);
        //LCDDrawPoint(row, right_border[row]);
        PerImg_ip[row][left_border[row]]=&black;
        PerImg_ip[row][right_border[row]]=&black;
    }
    //Ϳ�ڶ����߽�
    for (uint8 column = 0; column < USE_IMAGE_W; column++)
    {
        PerImg_ip[0][column] = &black;
        PerImg_ip[USE_IMAGE_H-1][column] = &black;
    }
}
