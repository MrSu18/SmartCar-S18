#include "ImageConversion.h"
#include "math.h"//二值化算法里面要用到pow函数

//定义变量
uint8 binary_image[MT9V03X_H][MT9V03X_W]={0};//二值化图像
uint8 gray_image[MT9V03X_H][MT9V03X_W]={0};//使用的缓存灰度图像,避免因为dma把图像冲走
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
* @brief : sobel二值化
* @param : uint8_t imag[MT9V03X_H][MT9V03X_W]:灰度图
*          uint8_t imag1[MT9V03X_H][MT9V03X_W]:二值化图
* @return: 无
* @date  : 2023.4.16
* @author: 刘骏帆
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
 ** 函数功能: 自适应阈值二值化图像
 ** 参    数: uint8* img_data：灰度图像
 **           uint8* output_data：二值化图像
 **           int width：图像宽度
 **           int height：图像高度
 **           int block：分割局部阈值的方块大小例如7*7
 **           uint8 clip_value: 局部阈值减去的经验值一般为（2~5）
 ** 返 回 值: 无
 ** 作    者: 上海交大16届智能车智能视觉组SJTUAuTop
 **           https://zhuanlan.zhihu.com/p/391051197
 ** 注    意：adaptiveThreshold(mt9v03x_image[0],BinaryImage[0],MT9V03X_W,MT9V03X_H,5,1);//但是没d用跟大津法一样
 *********************************************************************************************/
void myadaptiveThreshold(uint8 *img_data, uint8 *output_data, int width, int height, int block, uint8 clip_value)
{
    int half_block = block / 2;
    for(int y=half_block; y<height-half_block; y++)
    {
        for(int x=half_block; x<width-half_block; x++)
        {
            // 计算局部阈值
            int thres = 0;
            for(int dy=-half_block; dy<=half_block; dy++)
            {
                for(int dx=-half_block; dx<=half_block; dx++)
                {
                    thres += img_data[(x+dx)+(y+dy)*width];
                }
            }
            thres = thres / (block * block) - clip_value;
            // 进行二值化
            output_data[x+y*width] = img_data[x+y*width]>thres ? 255 : 0;
        }
    }
}
