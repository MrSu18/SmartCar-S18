#include "ImageConversion.h"
#include "ImageWR.h"
#include "math.h"//二值化算法里面要用到pow函数

//定义变量
uint8* PerImg_ip[PER_IMAGE_H][PER_IMAGE_W];//二维数组（元素是指针变量用于存储映射的像素地址）
uint8 left_border[USE_IMAGE_H] = {0};//图像左边界
uint8 right_border[USE_IMAGE_H] = {USE_IMAGE_W-1};//图像右边界
uint8 binary_image[MT9V03X_H][MT9V03X_W];

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
    int Sumpix = (width * height)/4;   //总像素点
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针

    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i+=2)
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
    uint8 Image_Threshold = otsuThreshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H)-10 ;//使用大津法得到二值化阈值
    for (int i = 0; i < MT9V03X_H; i++)
    {
        for (int j = 0; j < MT9V03X_W; j++)
        {
            if (mt9v03x_image[i][j] <= Image_Threshold)//进行二值化之前只是得到阈值
                mt9v03x_image[i][j] = IMAGE_BLACK;//0是黑色  //图像原点不变
            else
                mt9v03x_image[i][j] = IMAGE_WHITE;//1是白色  //图像原点不变
        }
    }
}

#if PERSPECTIVE==2  //先去畸变后逆透视
//140度
//畸变参数
double cameraMatrix[3][3]={{59.727551,0.000000,89.234010},{0.000000,60.159434,50.691969},{0.000000,0.000000,1.000000}};
double distCoeffs[5]={0.239129,-0.289288,-0.001345,-0.001779,0.090761};
int move_xy[2]={-1,-1};
//逆透视参数
double change_un_Mat[3][3] ={{-2.124767,3.123039,-217.537647},{-0.087254,1.274068,-194.734797},{-0.000566,0.034856,-4.575237}};
void find_xy(int x, int y, int local[2])
{
    double fx = cameraMatrix[0][0],
           fy = cameraMatrix[1][1],
           ux = cameraMatrix[0][2],
           uy = cameraMatrix[1][2],
           k1 = distCoeffs[0],
           k2 = distCoeffs[1],
           k3 = distCoeffs[4],
           p1 = distCoeffs[2],
           p2 = distCoeffs[3];
    double xCorrected = (x - ux) / fx;
    double yCorrected = (y - uy) / fy;
    double xDistortion, yDistortion;
    double r2 = xCorrected * xCorrected + yCorrected * yCorrected;
    double deltaRa = 1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2;
    double deltaRb = 1 / (1.);
    double deltaTx = 2. * p1 * xCorrected * yCorrected + p2 * (r2 + 2. * xCorrected * xCorrected);
    double deltaTy = p1 * (r2 + 2. * yCorrected * yCorrected) + 2. * p2 * xCorrected * yCorrected;
    xDistortion = xCorrected * deltaRa * deltaRb + deltaTx;
    yDistortion = yCorrected * deltaRa * deltaRb + deltaTy;
    xDistortion = xDistortion * fx + ux;
    yDistortion = yDistortion * fy + uy;
    if (yDistortion >= 0 && yDistortion < MT9V03X_H && xDistortion >= 0 && xDistortion < MT9V03X_W)
    {
        local[0] = (int) yDistortion;
        local[1] = (int) xDistortion;
    }
    else
    {
        local[0] = -1;
        local[1] = -1;
    }
}
void find_xy1(int x, int y, int local[2])
{
    int local_x = (int) ((change_un_Mat[0][0] * x + change_un_Mat[0][1] * y + change_un_Mat[0][2])
                        /(change_un_Mat[2][0] * x + change_un_Mat[2][1] * y + change_un_Mat[2][2]));
    int local_y = (int) ((change_un_Mat[1][0] * x + change_un_Mat[1][1] * y + change_un_Mat[1][2])
                        /(change_un_Mat[2][0] * x + change_un_Mat[2][1] * y + change_un_Mat[2][2]));
    if (local_x >= 0 && local_x<181 && local_y >= 0 && local_y<114)
    {
        local[0] = local_y;
        local[1] = local_x;
    }
    else
    {
        local[0] = -1;
        local[1] = -1;
    }
}
/***********************************************
* @brief : 初始化逆透视指针数组，得到对应的图像像素点映射
* @param : 原图像（全局变量）
* @return: 逆透视之后的二维指针映射（全局变量）
* @date  : 2022.8.28
* @author: 萝狮虎
************************************************/
void ImagePerspective_Init(void)
{
    static uint8 BlackColor = IMAGE_BAN;
    for (int i = 0; i < USE_IMAGE_H; i++)
    {
        for (int j = 0; j < USE_IMAGE_W; j++)
        {
            int local_xy[2] = {-1};
            find_xy1(j, i, local_xy);
            if (local_xy[0] != -1 && local_xy[0] != -1)
            {
                int local_xy1[2] = {-1};
                find_xy(local_xy[1] - move_xy[0], local_xy[0] - move_xy[1], local_xy1);
                if (local_xy1[0] != -1 && local_xy1[1] != -1)
                {
                    PerImg_ip[i][j] = &PER_IMG[local_xy1[0]][local_xy1[1]];
                }
                else PerImg_ip[i][j] = &BlackColor;
            }
            else PerImg_ip[i][j] = &BlackColor;
        }
    }
}
#elif PERSPECTIVE==1    //只进行逆透视不进行去畸变，注意逆透视矩阵是否正确
void ImagePerspective_Init(void) 
{
    static uint8 BlackColor = IMAGE_BAN;
    //逆透视矩阵
    double change_un_Mat[3][3] ={{-2.823035,3.058385,-152.620928},{0.141489,1.441372,-256.109755},{0.002246,0.033146,-4.763373}};
//    double change_un_Mat[3][3] ={{-2.897137,2.967769,-150.453872},{0.069071,1.394489,-249.171423},{0.001096,0.032185,-4.688751}};
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
* @brief : 逆透视图像边界初始化
* @param : 逆透视图像、宽度、高度、代表禁用区域的灰度值
* @return: 代表左右图像边界的一维数组
* @date  : 2022.9.1
* @author: 刘骏帆
************************************************/
void ImageBorderInit(void)
{
#if PERSPECTIVE==0
    memset(left_border,0,sizeof(left_border[0])*USE_IMAGE_H);
    memset(right_border,USE_IMAGE_W-1,sizeof(right_border[0])*USE_IMAGE_H);
#else
    //static uint8 black=IMAGE_BLACK;//要给个静态局部变量，确保内存不会被释放
    //静态局部变量用于靠近边界减少运算量
    static uint8 left_x=USE_IMAGE_W/2,right_x=USE_IMAGE_W/2;
    for (uint8 row = USE_IMAGE_H - 1; row > 0; row--)
    {
        //中间到左边
        for (uint8 l_column = left_x; l_column > 0; l_column--)
        {
            if (use_image[row][l_column] == IMAGE_BAN)
            {
                left_x = l_column + 2;//更新列坐标用于边界追踪
                left_border[row]= l_column + 1;
                break;
            }
            else if (l_column - 1 == 0)//如果读到最左边了还是没找到边界，那么就把1作为左边界
            {
                left_border[row] = left_border[row + 1];
            }
        }
        //中间到右边
        for (uint8 r_column = right_x; r_column < USE_IMAGE_W - 1; r_column++)
        {
            if (use_image[row][r_column] == IMAGE_BAN)
            {
                right_x = r_column - 2;
                right_border[row]= r_column - 1;
                break;
            }
            else if (r_column + 1 == USE_IMAGE_W - 1)//同理读到最右边还没有读到就用W-1作为右边界
            {
                right_border[row] = right_border[row + 1];
            }
        }
    }
#endif  //PERSPECTIVE透视程度
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
//  assert(block % 2 == 1); // block必须为奇数
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

void sobel(uint8_t imag[MT9V03X_H][MT9V03X_W],uint8_t imag1[MT9V03X_H][MT9V03X_W])
{
    int tempx=0,tempy=0,temp=0,i=0,j=0;
    for(i=1;i <MT9V03X_H-1; i++)
    {
        for(j=1;j<MT9V03X_W-1;j++)
        {

            tempx=(-  imag[i-1][j-1])
                  +(-2*imag[i  ][j-1])
                  +(-  imag[i+1][j-1])
                  +(   imag[i-1][j+1])
                  +( 2*imag[i  ][j+1])
                  +(   imag[i+1][j+1]);
            if(tempx<0)
                tempx=-tempx;

            tempy=(   imag[i+1][j-1])
                  +( 2*imag[i+1][j  ])
                  +(   imag[i+1][j+1])
                  +(-  imag[i-1][j-1])
                  +(-2*imag[i-1][j  ])
                  +(-  imag[i-1][j+1]);
            if(tempy<0)
                tempy=-tempy;
            temp=tempx+tempy;
            if(temp>100)
                temp=255;
            else temp=0;
            imag1[i][j]=temp;

        }
    }
}



