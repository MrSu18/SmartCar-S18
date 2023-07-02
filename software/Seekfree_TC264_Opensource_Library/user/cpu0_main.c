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
* 文件名称          cpu0_main
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
#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中
#include "zf_driver_gpio.h"
#include "motor.h"
#include "adc.h"
#include "filter.h"
#include "bluetooth.h"
#include "pid.h"
#include "KeyMenu.h"
#include "icm20602.h"
#include "Control.h"
#include "debug.h"
#include "ADRC.h"
#include "ImageProcess.h"

// 工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
// 工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
// 然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
// 一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

// 对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用 interrupt_global_enable(0); 来开启中断嵌套
// 简单点说实际上进入中断后TC系列的硬件自动调用了 interrupt_global_disable(); 来拒绝响应任何的中断，因此需要我们自己手动调用 interrupt_global_enable(0); 来开启中断的响应。

// 本例程是开源库移植用空工程
// 本例程是开源库移植用空工程
// 本例程是开源库移植用空工程

// **************************** 代码区域 ****************************
int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    //debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等

/*************************模块初始化***************************/
    MotorInit();
    EncoderInit();
    ADCInit();
    KEYInit();
    mt9v03x_init();//初始化摄像头
    tft180_init();//初始化tft
    UARTInit();//初始化蓝牙模块
    icm20602_init();
    GyroOffsetInit();
    dl1a_init();
    //初始化debug的led
    LedInit();
    //给蜂鸣器低电平
    gpio_init(BEER,GPO,GPIO_LOW,GPO_PUSH_PULL);
    //初始化中断
    pit_ms_init(CCU60_CH0,2);pit_disable(CCU60_CH0);//速度环
    pit_ms_init(CCU60_CH1,8);pit_disable(CCU60_CH1);//方向环
    //陀螺仪中断2ms
    pit_ms_init(CCU61_CH0,2);pit_disable(CCU61_CH0);
/*************************参数初始化***************************/
    ADRC_Init();
    KalmanInit(&kalman_adc,25,5);
    KalmanInit(&kalman_gyro,1,100);
    //速度环PID初始化
    PIDInit(&speedpid_left,317.90,0.69,0);//185.8,0.50
    PIDInit(&speedpid_right,313.16,0.74,0);//164.8,0.45
    //转向环PID初始化
    PIDInit(&turnpid_image,13,0,0);//700,40有角速度只能跑50
    PIDInit(&turnpid_adc,9,0,4);
    PIDInit(&gyropid,0.01,0,0);//角速度环的P用于转向环的微分项
    //前馈控制
    FFCInit(&speedffc_left,24255,716743,44.5);
    FFCInit(&speedffc_right,24409,628773,44.42);

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         //等待所有核心初始化完毕
//    ADCScan();
    while (TRUE)
    {
//        int16 a = GetICM20602Gyro_Y();
//        tft180_show_int(0, 0, a, 5);
//        dl1a_get_distance();
//        tft180_show_int(0, 0, dl1a_distance_mm, 5);
        ADCGetValue(adc_value);
//        ChaBiHe(TRACK);
//        if(gyro_flag == 1)
//        {
//            printf("%f,%d,%d,%f\n",image_bias,5000,real_gyro,gyropid.integer_err);
//            gyro_flag = 0;
//        }
        // 此处编写需要循环执行的代码
       if(c0h0_isr_flag==1)
       {
           printf("%d,%d,%d,%d\r\n",speed_left,target_left,speed_right,target_right);
           c0h0_isr_flag=0;
       }
//       if(c0h1_isr_flag==1)
//       {
//           printf("%d,%f,%d,%d\r\n",s,image_bias,process_status_cnt,base_speed);
//           c0h1_isr_flag=0;
//       }
        // 此处编写需要循环执行的代码
    }
}

#pragma section all restore
// **************************** 代码区域 ****************************

