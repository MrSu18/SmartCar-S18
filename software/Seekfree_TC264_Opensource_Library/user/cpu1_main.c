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
#include "bluetooth.h"
#include "ImageProcess.h"
#include "pid.h"
#include "KeyMenu.h"
#include "debug.h"
#include "icm20602.h"
#include "Control.h"
#include "ImageSpecial.h"

// 工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
// 工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
// 然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
// 一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化
inline int Limit(int x, int low, int up)//给x设置上下限幅
{
    return x > up ? up : x < low ? low : x;
}
// 对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用 enableInterrupts(); 来开启中断嵌套
// 简单点说实际上进入中断后TC系列的硬件自动调用了 disableInterrupts(); 来拒绝响应任何的中断，因此需要我们自己手动调用 enableInterrupts(); 来开启中断的响应。

// **************************** 代码区域 ****************************
void core1_main(void)
{
    disable_Watchdog();                     // 关闭看门狗
    interrupt_global_enable(0);             // 打开全局中断
    // 此处编写用户代码 例如外设初始化代码等

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();                 // 等待所有核心初始化完毕
    sobel(mt9v03x_image,binary_image);
    otsu_thr=otsuThreshold(binary_image[0], MT9V03X_W, MT9V03X_H);//使用大津法得到二值化阈值
    ReadFromFlash();//从flash中读取值
    base_speed=original_speed=process_speed[0];
    KeyCtrl();//按键控制
    pit_enable(CCU60_CH0);
    OutGarage();//出库
    while (TRUE)
    {
        // 此处编写需要循环执行的代码

        if(mt9v03x_finish_flag)
        {
#if 1
            memcpy(gray_image,mt9v03x_image,USE_IMAGE_H*USE_IMAGE_W);//37us
            //出界保护
            OutProtect();
            //图像处理
            ImageProcess();
//            sobel(mt9v03x_image,binary_image);
            //显示灰度图
            ShowImage();
            gpio_toggle_level(P20_8);
            //显示边线
            ShowLine();
            //赛道基础信息变量重置
            TrackBasicClear();
#else
            static uint8 temp=0;
            if(temp==0)
            {
                StartIntegralAngle_X(360);
                temp=1;
            }
            tft180_show_float(0, 0,my_angle_x, 4, 2);
//            MotorSetPWM(2000,2000);
//            system_delay_ms(3000);
//            while(1)
//            {
//                MotorSetPWM(0, 0);
//            }
//            seekfree_sendimg_03x(UART_2, mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
#endif
            mt9v03x_finish_flag=0;
        }




         //此处编写需要循环执行的代码
    }
}
#pragma section all restore
