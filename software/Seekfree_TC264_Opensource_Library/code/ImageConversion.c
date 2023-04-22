#include "ImageConversion.h"
#include "math.h"//二值化算法里面要用到pow函数

//定义变量
uint8 binary_image[MT9V03X_H][MT9V03X_W]={0};//二值化图像
uint8 left_border[USE_IMAGE_H] = {0};//图像左边界
uint8 right_border[USE_IMAGE_H] = {USE_IMAGE_W-1};//图像右边界
uint8 otsu_thr=0;//大津法对sobel算法之后的图像求得阈值

/***********************************************
* @brief : 大津法二值化0.8ms程序（实际测试4ms在TC264中）
* @param : *image ：图像地址
*           width:  图像宽
*           height：图像高
* @return: 无
* @date  : 2018-10
* @author: Z小旋
************************************************/
uint8 otsuThreshold(uint8* image, uint16 width, uint16 height)
{
#define GrayScale 256 //最高灰度级
    int pixelCount[GrayScale] = { 0 };//每个灰度值所占像素个数
    float pixelPro[GrayScale] = { 0 };//每个灰度值所占总像素比例
    int i, j;
    int Sumpix = (width/2 * (106-55));   //总像素点
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针

    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 55; i < 106; i++)
    {
        for (j = 0; j < width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将像素值作为计数数组的下标
          //   pixelCount[(int)image[i][j]]++;    若不用指针用这个
        }
    }
    float u = 0;
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / Sumpix;   //计算每个像素在整幅图像中的比例
        u += i * pixelPro[i];  //总平均灰度
    }

    float maxVariance = 0.0;  //最大类间方差
    float w0 = 0, avgValue = 0;  //w0 前景比例 ，avgValue 前景平均灰度
    for (int i = 0; i < 256; i++)     //每一次循环都是一次完整类间方差计算 (两个for叠加为1个)
    {
        w0 += pixelPro[i];  //假设当前灰度i为阈值, 0~i 灰度像素所占整幅图像的比例即前景比例
        avgValue += i * pixelPro[i];

        float variance = pow((avgValue / w0 - u), 2) * w0 / (1 - w0);    //类间方差
        if (variance > maxVariance)
        {
            maxVariance = variance;
            threshold = (uint8)i;
        }
    }
    return threshold;
}
/***********************************************
* @brief : 全图计算阈值对全图进行二值化
* @param : 灰度图像（全局变量）
* @return: 灰度图像变成二值化图像（全局变量）
* @date  : 2021.12.15
* @author: 刘骏帆
************************************************/
void ImageBinary(void)
{
    //压缩图像的二值化
    uint8 Image_Threshold = otsuThreshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);//使用大津法得到二值化阈值
    for (int i = 0; i < MT9V03X_H; i++)
    {
        for (int j = 0; j < MT9V03X_W; j++)
        {
            if (mt9v03x_image[i][j] <= Image_Threshold)//进行二值化之前只是得到阈值
                binary_image[i][j] = IMAGE_BLACK;//0是黑色  //图像原点不变
            else
                binary_image[i][j] = IMAGE_WHITE;//1是白色  //图像原点不变
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
