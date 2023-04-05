/*
 * key.c
 *
 *  Created on: 2023年2月2日
 *      Author: L
 */
#include "key.h"
#include "zf_device_tft180.h"
#include "pid.h"
#include "ImageTrack.h"
#include "motor.h"

/***********************************************
* @brief : 按键初始化
* @param : void
* @return: void
* @date  : 2023.4.2
* @author: L
************************************************/
void KEYInit(void)
{
    gpio_init(P33_10,GPI,LOOSE,GPI_PULL_DOWN);
    gpio_init(P33_11,GPI,LOOSE,GPI_PULL_DOWN);
    gpio_init(P33_12,GPI,LOOSE,GPI_PULL_DOWN);
    gpio_init(P33_13,GPI,LOOSE,GPI_PULL_DOWN);
    gpio_init(P32_4,GPI,LOOSE,GPI_PULL_DOWN);
}
/***********************************************
* @brief : 按键扫描程序，扫描是否有按键被按下
* @param : void
* @return: 若有按键按下则返回按下哪个按键，否则返回0
* @date  : 2023.4.2
* @author: L
************************************************/
uint8 KEYScan(void)
{
    static uint8 keypress_flag = LOOSE;             //按键松开标志

    //按键为松开状态，但当前有按键被按下，则返回按下的那个按键
    if(keypress_flag == LOOSE && (KEY1==PRESSDOWN||KEY2==PRESSDOWN||KEY3==PRESSDOWN||KEY4==PRESSDOWN||KEY5==PRESSDOWN))
    {
        system_delay_ms(10);
        keypress_flag = PRESSDOWN;                  //按键按下标志

        if(KEY1==PRESSDOWN) return KEY_UP;
        else if(KEY2==PRESSDOWN) return KEY_DOWN;
        else if(KEY3==PRESSDOWN) return KEY_LEFT;
        else if(KEY4==PRESSDOWN) return KEY_RIGHT;
        else if(KEY5==PRESSDOWN) return KEY_ENTER;
    }
    else if(KEY1==LOOSE||KEY2==LOOSE||KEY3==LOOSE||KEY4==LOOSE||KEY5==LOOSE)
        keypress_flag = LOOSE;                      //按键松开标志
    return 0;
}
/***********************************************
* @brief : 获取哪个按键被按下，可以防止按一次按键出现多次响应的情况
* @param : void
* @return: key_down:若有按键按下则返回按下哪个按键，否则返回0
* @date  : 2023.4.5
* @author: L
************************************************/
uint8 KeyGet(void)
{
    static uint8 key_value = 0;
    static uint8 key_down  = 0;
    static uint8 key_old   = 0;

    key_value = KEYScan();
    key_down = key_value & (key_value ^ key_old);
    key_old = key_value;

    return key_down;
}
/***********************************************
* @brief : 通过按键选择显示哪个PID的参数
* @param : key_num:按下的是哪个按键
* @return: void
* @date  : 2023.4.2
* @author: L
************************************************/
uint8 PIDDisplay(uint8 key_num)
{
//    system_delay_ms(100);

    switch(key_num)
    {
        case KEY_UP://显示左电机速度环PID参数，并可选择调节P或I
        {
            tft180_clear();
            while(1)
            {
                tft180_show_string(0, 0, "speedpid_left.P");
                tft180_show_float(100, 0, speedpid_left.P, 3, 3);
                tft180_show_string(0, 10, "speedpid_left.I");
                tft180_show_float(100, 10, speedpid_left.I, 3, 3);
                tft180_show_string(0, 30, "KEY1:change P");
                tft180_show_string(0, 40, "KEY2:change I");
                tft180_show_string(0, 50, "KEY3:exit");
                switch(KeyGet())
                {
                    case KEY_UP:tft180_clear();return 1;
                    case KEY_DOWN:tft180_clear();return 2;
                    case KEY_LEFT:tft180_clear();return 0;
                    default:break;
                }
            }
            break;
        }
        case KEY_DOWN://显示右电机速度环PID参数，并可选择调节P或I
        {
            tft180_clear();
            while(1)
            {
                tft180_show_string(0, 0, "speedpid_right.P");
                tft180_show_float(100, 0, speedpid_right.P, 3, 3);
                tft180_show_string(0, 10, "speedpid_right.I");
                tft180_show_float(100, 10, speedpid_right.I, 3, 3);
                tft180_show_string(0, 30, "KEY1:change P");
                tft180_show_string(0, 40, "KEY2:change I");
                tft180_show_string(0, 50, "KEY3:exit");
                switch(KeyGet())
                {
                    case KEY_UP:tft180_clear();return 3;
                    case KEY_DOWN:tft180_clear();return 4;
                    case KEY_LEFT:tft180_clear();return 0;
                    default:break;
                }
            }
            break;
        }
        case KEY_LEFT://显示图像方向环PID参数，并可选择调节P或D
        {
            tft180_clear();
            while(1)
            {
                tft180_show_string(0, 0, "turnpid_image.P");
                tft180_show_float(100, 0, turnpid_image.P, 3, 3);
                tft180_show_string(0, 10, "turnpid_image.D");
                tft180_show_float(100, 10, turnpid_image.D, 3, 3);
                tft180_show_string(0, 30, "KEY1:change P");
                tft180_show_string(0, 40, "KEY2:change D");
                tft180_show_string(0, 50, "KEY3:exit");
                switch(KeyGet())
                {
                    case KEY_UP:return 5;
                    case KEY_DOWN:return 6;
                    case KEY_LEFT:return 0;
                    default:break;
                }
            }
            break;
        }
        case KEY_RIGHT://显示电磁方向环PID参数，并可选择调节P或D
        {
            tft180_clear();
            while(1)
            {
                tft180_show_string(0, 0, "turnpid_adc.P");
                tft180_show_float(100, 0, turnpid_adc.P, 3, 3);
                tft180_show_string(0, 10, "turnpid_adc.D");
                tft180_show_float(100, 10, turnpid_adc.D, 3, 3);
                tft180_show_string(0, 30, "KEY1:change P");
                tft180_show_string(0, 40, "KEY2:change D");
                tft180_show_string(0, 50, "KEY3:exit");
                switch(KEYScan())
                {
                    case KEY_UP:return 7;
                    case KEY_DOWN:return 8;
                    case KEY_LEFT:return 0;
                    default:break;
                }
            }
            break;
        }
        case KEY_ENTER:return 0;//退出
        default:return 9;
    }
    return 9;
}
/***********************************************
* @brief : 按键调节PID参数
* @param : void
* @return: void
* @date  : 2023.4.2
* @author: L
************************************************/
void KeyPID(void)
{
//    system_delay_ms(100);

    while(1)
    {
        tft180_show_string(0, 0, "KEY1:speed_pid_left");
        tft180_show_string(0, 10, "KEY2:speed_pid_right");
        tft180_show_string(0, 20, "KEY3:turn_pid_image");
        tft180_show_string(0, 30, "KEY4:turn_pid_adc");
        tft180_show_string(0, 40, "KEY5:exit");

        uint8 change_parameter = PIDDisplay(KeyGet());//0表示退出调参，9表示没检测到按键按下，1~8表示是哪个参数+或-

//        system_delay_ms(100);
        if(change_parameter == 0)//0退出调参
        {
            ShowPIDParameter();
            break;
        }
        else if(change_parameter != 9)//9继续循环
        {
            tft180_clear();
            while(1)
            {
                uint8 exit_flag = 0;//退出调参的标志

                tft180_show_string(0, 0, "KEY1:P+1");
                tft180_show_string(0, 10, "KEY2:P-1");
                tft180_show_string(0, 20, "KEY3:I+0.1/D+0.5");
                tft180_show_string(0, 30, "KEY4:I-0.1/D-0.5");
                tft180_show_string(0, 40, "KEY5:exit");
//                system_delay_ms(100);
                switch(KeyGet())
                {
                    case KEY_UP://参数P+
                    {
                        tft180_clear();
                        switch(change_parameter)//选择哪个PID的P
                        {
                            case 1:
                            {
                                speedpid_left.P += 1;
                                tft180_show_string(0, 60, "speedpid_left.P:");
                                tft180_show_float(100, 60, speedpid_left.P, 3, 3);
                                break;
                            }
                            case 3:
                            {
                                speedpid_right.P += 1;
                                tft180_show_string(0, 60, "speedpid_right.P:");
                                tft180_show_float(100, 60, speedpid_right.P, 3, 3);
                                break;
                            }
                            case 5:
                            {
                                turnpid_image.P += 1;
                                tft180_show_string(0, 60, "turnpid_image.P:");
                                tft180_show_float(100, 60, turnpid_image.P, 3, 3);
                                break;
                            }
                            case 7:
                            {
                                turnpid_adc.P += 1;
                                tft180_show_string(0, 60, "turnpid_adc.P:");
                                tft180_show_float(100, 60, turnpid_adc.P, 3, 3);
                                break;
                            }
                        }
                        break;
                    }
                    case KEY_DOWN://参数P-
                    {
                        tft180_clear();
                        switch(change_parameter)//选择哪个PID的P
                        {
                            case 1:
                            {
                                speedpid_left.P -= 1;
                                tft180_show_string(0, 60, "speedpid_left.P:");
                                tft180_show_float(100, 60, speedpid_left.P, 3, 3);
                                break;
                            }
                            case 3:
                            {
                                speedpid_right.P -= 1;
                                tft180_show_string(0, 60, "speedpid_right.P:");
                                tft180_show_float(100, 60, speedpid_right.P, 3, 3);
                                break;
                            }
                            case 5:
                            {
                                turnpid_image.P -= 1;
                                tft180_show_string(0, 60, "turnpid_image.P:");
                                tft180_show_float(100, 60, turnpid_image.P, 3, 3);
                                break;
                            }
                            case 7:
                            {
                                turnpid_adc.P -= 1;
                                tft180_show_string(0, 60, "turnpid_adc.P:");
                                tft180_show_float(100, 60, turnpid_adc.P, 3, 3);
                                break;
                            }
                        }
                        break;
                    }
                    case KEY_LEFT://参数I+
                    {
                        tft180_clear();
                        switch(change_parameter)//选择哪个PID的I
                        {
                            case 2:
                            {
                                speedpid_left.I += 0.1;
                                tft180_show_string(0, 60, "speedpid_left.I:");
                                tft180_show_float(100, 60, speedpid_left.I, 3, 3);
                                break;
                            }
                            case 4:
                            {
                                speedpid_right.I += 0.1;
                                tft180_show_string(0, 60, "speedpid_right.I:");
                                tft180_show_float(100, 60, speedpid_right.I, 3, 3);
                                break;
                            }
                            case 6:
                            {
                                turnpid_image.D += 0.5;
                                tft180_show_string(0, 60, "turnpid_image.D:");
                                tft180_show_float(100, 60, turnpid_image.D, 3, 3);
                                break;
                            }
                            case 8:
                            {
                                turnpid_adc.D += 0.5;
                                tft180_show_string(0, 60, "turnpid_adc.D:");
                                tft180_show_float(100, 60, turnpid_adc.D, 3, 3);
                                break;
                            }
                        }
                        break;
                    }
                    case KEY_RIGHT://参数I-
                    {
                        tft180_clear();
                        switch(change_parameter)//选择哪个PID的I
                        {
                            case 2:
                            {
                                speedpid_left.I += 0.1;
                                tft180_show_string(0, 60, "speedpid_left.I:");
                                tft180_show_float(100, 60, speedpid_left.I, 3, 3);
                                break;
                            }
                            case 4:
                            {
                                speedpid_right.I += 0.1;
                                tft180_show_string(0, 60, "speedpid_right.I:");
                                tft180_show_float(100, 60, speedpid_right.I, 3, 3);
                                break;
                            }
                            case 6:
                            {
                                turnpid_image.D += 0.5;
                                tft180_show_string(0, 60, "turnpid_image.D:");
                                tft180_show_float(100, 60, turnpid_image.D, 3, 3);
                                break;
                            }
                            case 8:
                            {
                                turnpid_adc.D += 0.5;
                                tft180_show_string(0, 60, "turnpid_adc.D:");
                                tft180_show_float(100, 60, turnpid_adc.D, 3, 3);
                                break;
                            }
                        }
                        break;
                    }
                    case KEY_ENTER:exit_flag = 1;tft180_clear();break;//退出该参数的调参，可继续选择调节下一个参数
                }
                if(exit_flag == 1) break;
            }
        }
    }
}
/***********************************************
* @brief : 前瞻和基础速度按键调参
* @param : void
* @return: void
* @date  : 2023.4.2
* @author: L
************************************************/
void KeyTrack(void)
{
//    system_delay_ms(500);

    while(1)
    {
        uint8 exit_flag = 0;//退出调参的标志
        ShowImageParameter();//显示当前参数
        tft180_show_string(0, 50, "KEY3:change aim_distance");
        tft180_show_string(0, 60, "KEY4:change base_speed");
        tft180_show_string(0, 70, "KEY5:exit");
        switch(KeyGet())
        {
//            system_delay_ms(500);
            case KEY_LEFT://修改预瞄点的值
            {
                tft180_clear();
                while(1)
                {
                    uint8 exit_flag_1 = 0;//退出调节预瞄点的标志位
                    tft180_show_string(0, 0, "KEY1:aim_distance+0.01");
                    tft180_show_string(0, 10, "KEY2:aim_distance+0.02");
                    tft180_show_string(0, 20, "KEY3:aim_distance-0.01");
                    tft180_show_string(0, 30, "KEY4:aim_distance-0.02");
                    tft180_show_string(0, 40, "KEY5:exit");
//                    system_delay_ms(500);
                    switch(KeyGet())
                    {
                        case KEY_UP://预瞄点+0.01
                        {
                            aim_distance+=0.01;
                            tft180_show_string(0, 80, "aim_distance:");
                            tft180_show_float(100, 80, aim_distance, 1, 3);
                            break;
                        }
                        case KEY_DOWN://预瞄点+0.02
                        {
                            aim_distance+=0.02;
                            tft180_show_string(0, 80, "aim_distance:");
                            tft180_show_float(100, 80, aim_distance, 1, 3);
                            break;
                        }
                        case KEY_LEFT://预瞄点-0.01
                        {
                            aim_distance-=0.01;
                            tft180_show_string(0, 80, "aim_distance:");
                            tft180_show_float(100, 80, aim_distance, 1, 3);
                            break;
                        }
                        case KEY_RIGHT://预瞄点-0。02
                        {
                            aim_distance-=0.02;
                            tft180_show_string(0, 80, "aim_distance:");
                            tft180_show_float(100, 80, aim_distance, 1, 3);
                            break;
                        }
                        case KEY_ENTER:exit_flag_1=1;break;
                        default:break;
                    }
                    if(exit_flag_1 == 1) break;
                }
                break;
            }
            case KEY_RIGHT://修改基础速度
            {
                tft180_clear();
                while(1)
                {
                    uint8 exit_flag_1 = 0;//退出调节基础速度的标志
                    tft180_show_string(0, 20, "KEY3:base_speed+10");
                    tft180_show_string(0, 30, "KEY4:base_speed-10");
                    tft180_show_string(0, 40, "KEY5:exit");
                    switch(KeyGet())
                    {
//                        system_delay_ms(500);
                        tft180_show_string(0, 70, "base_speed:");
                        tft180_show_int(100, 80, base_speed, 3);
                        case KEY_LEFT:
                        {
                            base_speed+=10;
                            tft180_show_string(0, 80, "base_speed:");
                            tft180_show_int(100, 80, base_speed, 3);
                            break;
                        }
                        case KEY_RIGHT:
                        {
                            base_speed-=10;
                            tft180_show_string(0, 80, "base_speed:");
                            tft180_show_int(100, 80, base_speed, 3);
                            break;
                        }
                        case KEY_ENTER:exit_flag_1=1;break;
                        default:break;
                    }
                    if(exit_flag_1 == 1) break;
                }
                break;
            }
            case KEY_ENTER://退出调参
            {
                ShowImageParameter();//显示当前参数
                exit_flag=1;
                break;
            }
        }
        if(exit_flag == 1) break;
    }
}
/***********************************************
* @brief : 显示灰度图像或逆透视后的图像，并可选择是否显示边线、中线等
* @param : void
* @return: void
* @date  : 2023.4.2
* @author: L
************************************************/
uint8 binary_image_flag = 0,gray_image_flag = 0;//是否显示灰度或逆透视后的图像，1为显示
uint8 edgeline_flag = 0,c_line_flag = 0;        //是否显示边线或中线，1为显示
void KeyImage(void)
{
//    system_delay_ms(500);
    while(1)
    {
        uint8 exit_flag = 0;//退出选择图像显示的标志位
        tft180_show_string(0, 0, "KEY3:show gray_image");
        tft180_show_string(0, 10, "KEY4:show binary_image");
        switch(KeyGet())
        {
//            system_delay_ms(500);
            case KEY_LEFT://显示灰度图
            {
                gray_image_flag = 1;//显示灰度图的标志
                exit_flag = 1;//退出
                break;
            }
            case KEY_RIGHT://显示逆透视图
            {
                tft180_clear();
                while(1)
                {
                    uint8 exit_flag_1 = 0;//退出选择是否显示边线或中线的标志位
                    binary_image_flag = 1;//显示逆透视图像
                    tft180_show_string(0, 0, "KEY3:show edge_line");
                    tft180_show_string(0, 10, "KEY4:show center_line");
                    tft180_show_string(0, 20, "KEY5:exit");
                    switch(KeyGet())
                    {
                        case KEY_LEFT:edgeline_flag=1;break;
                        case KEY_RIGHT:c_line_flag=1;break;
                        case KEY_ENTER:exit_flag_1=1;break;
                    }
                    if(exit_flag_1 == 1) break;
                }
                exit_flag = 1;
                break;
            }
//            default:break;
        }
        if(exit_flag == 1)
        {
            tft180_clear();
            break;
        }
    }
}
/***********************************************
* @brief : 显示当前所有的PID参数
* @param : void
* @return: void
* @date  : 2023.4.2
* @author: L
************************************************/
void ShowPIDParameter(void)
{
    tft180_clear();
    tft180_show_string(0, 0, "speedpid_left.P:");
    tft180_show_float(100, 0, speedpid_left.P, 3, 3);
    tft180_show_string(0, 10, "speedpid_left.I:");
    tft180_show_float(100, 10, speedpid_left.I, 3, 3);
    tft180_show_string(0, 20, "speedpid_right.P:");
    tft180_show_float(100, 20, speedpid_right.P, 3, 3);
    tft180_show_string(0, 30, "speedpid_right.I:");
    tft180_show_float(100, 30, speedpid_right.I, 3, 3);
    tft180_show_string(0, 40, "turnpid_image.P:");
    tft180_show_float(100, 40, turnpid_image.P, 3, 3);
    tft180_show_string(0, 50, "turnpid_image.D:");
    tft180_show_float(100, 50, turnpid_image.D, 3, 3);
    tft180_show_string(0, 60, "turnpid_adc.P:");
    tft180_show_float(100, 60, turnpid_adc.P, 3, 3);
    tft180_show_string(0, 70, "turnpid_adc.D:");
    tft180_show_float(100, 70, turnpid_adc.D, 3, 3);
}
/***********************************************
* @brief : 前瞻和基础速度参数显示
* @param : void
* @return: void
* @date  : 2023.4.2
* @author: L
************************************************/
void ShowImageParameter(void)
{
    tft180_show_string(0, 0, "aim_distance:");
    tft180_show_float(100, 0, aim_distance, 1, 2);
    tft180_show_string(0, 10, "base_speed:");
    tft180_show_int(100, 10, base_speed, 3);
}
/***********************************************
* @brief : 按键调参整体控制
* @param : void
* @return: void
* @date  : 2023.4.3
* @author: L
************************************************/
void KEYCtrl(void)
{
    while(1)
    {
        uint8 exit_flag_2 = 0;//退出按键模式的标志位
        switch(KeyGet())
        {
            case KEY_DOWN:KeyPID();break;//KEY2表示调PID参数
            case KEY_LEFT:KeyTrack();break;//KEY3表示调循迹参数
            case KEY_RIGHT:KeyImage();exit_flag_2 = 1;break;//KEY4表示选择显示什么图像
            case KEY_ENTER://KEY5表示开启中断，车落地跑
            {
                system_delay_ms(1000);
                exit_flag_2 = 1;//退出按键模式
                encoder_clear_count(ENCODER_LEFT);                                      //清空左边编码器计数
                encoder_clear_count(ENCODER_RIGHT);                                     //清空右边编码器计数
                pit_enable(CCU60_CH0);
                pit_enable(CCU60_CH1);
//                MotorSetPWM(2000, 2000);
                break;
            }
        }
        if(exit_flag_2 == 1) break;
    }
}
