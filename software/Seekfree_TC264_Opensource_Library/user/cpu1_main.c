/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          cpu1_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#pragma section all "cpu1_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU1的RAM中
#include "ImageConversion.h"
#include "ImageBasic.h"
#include "ImageTrack.h"

// 工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
// 工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
// 然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
// 一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

// 对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用 enableInterrupts(); 来开启中断嵌套
// 简单点说实际上进入中断后TC系列的硬件自动调用了 disableInterrupts(); 来拒绝响应任何的中断，因此需要我们自己手动调用 enableInterrupts(); 来开启中断的响应。

// **************************** 代码区域 ****************************
void core1_main(void)
{
    disable_Watchdog();                     // 关闭看门狗
    interrupt_global_enable(0);             // 打开全局中断
    // 此处编写用户代码 例如外设初始化代码等


    ImagePerspective_Init();//逆透视初始化
    ImageBorderInit();//透视后的图像边界初始化

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();                 // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码

        if(mt9v03x_finish_flag)
        {
            ImageBinary();
//            tft180_show_binary_image(0, 0, mt9v03x_image[0], USE_IMAGE_W, USE_IMAGE_H, 96, 60);
            EdgeDetection();
            //对边线进行滤波
            myPoint_f f_left_line[EDGELINE_LENGTH],f_right_line[EDGELINE_LENGTH];
            BlurPoints(left_line, l_line_count, f_left_line, 5);
            BlurPoints(right_line, r_line_count, f_right_line, 5);
            //等距采样
            myPoint_f f_left_line1[EDGELINE_LENGTH],f_right_line1[EDGELINE_LENGTH];
            int l_count=200,r_count=200;
            ResamplePoints(f_left_line, l_line_count, f_left_line1, &l_count, 0.04*50);
            ResamplePoints(f_right_line, r_line_count, f_right_line1, &r_count, 0.04*50);
            //局部曲率
            float l_angle[l_count],r_angle[r_count];
            local_angle_points(f_left_line1,l_count,l_angle,0.1/0.04);
            local_angle_points(f_right_line1,r_count,r_angle,0.1/0.04);
            //极大值点抑制
            float l_angle_1[l_count],r_angle_1[r_count];
            nms_angle(l_angle,l_count,l_angle_1,(0.1/0.04)*2+1);
            nms_angle(r_angle,r_count,r_angle_1,(0.1/0.04)*2+1);
            //跟踪左右边线
            track_leftline(f_left_line1, l_count, center_line_l, (int) round(0.1/0.04), 50*(0.4/2));
            track_rightline(f_right_line1, r_count, center_line_r, (int) round(0.1/0.04), 50*(0.4/2));
            cl_line_count=l_count;cr_line_count=r_count;
            // 预瞄点求偏差
            float Bias=GetAnchorPointBias(0.3,cr_line_count,center_line_r);
//            tft180_show_float(0, 0, Bias, 3, 3);
//            for(int i=0;i<cl_line_count;i++)
//            {
//                tft180_draw_point((uint16)center_line_l[i].X, (uint16)center_line_l[i].Y, RGB565_BLUE);
//            }
//            for(int i=0;i<cr_line_count;i++)
//            {
//                tft180_draw_point((uint16)center_line_r[i].X, (uint16)center_line_r[i].Y, RGB565_RED);
//            }
            TrackBasicClear();
            mt9v03x_finish_flag=0;
        }


        // 此处编写需要循环执行的代码
    }
}
#pragma section all restore
