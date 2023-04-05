/*
 * key.c
 *
 *  Created on: 2023��2��2��
 *      Author: L
 */
#include "key.h"
#include "zf_device_tft180.h"
#include "pid.h"
#include "ImageTrack.h"
#include "motor.h"

/***********************************************
* @brief : ������ʼ��
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
* @brief : ����ɨ�����ɨ���Ƿ��а���������
* @param : void
* @return: ���а��������򷵻ذ����ĸ����������򷵻�0
* @date  : 2023.4.2
* @author: L
************************************************/
uint8 KEYScan(void)
{
    static uint8 keypress_flag = LOOSE;             //�����ɿ���־

    //����Ϊ�ɿ�״̬������ǰ�а��������£��򷵻ذ��µ��Ǹ�����
    if(keypress_flag == LOOSE && (KEY1==PRESSDOWN||KEY2==PRESSDOWN||KEY3==PRESSDOWN||KEY4==PRESSDOWN||KEY5==PRESSDOWN))
    {
        system_delay_ms(10);
        keypress_flag = PRESSDOWN;                  //�������±�־

        if(KEY1==PRESSDOWN) return KEY_UP;
        else if(KEY2==PRESSDOWN) return KEY_DOWN;
        else if(KEY3==PRESSDOWN) return KEY_LEFT;
        else if(KEY4==PRESSDOWN) return KEY_RIGHT;
        else if(KEY5==PRESSDOWN) return KEY_ENTER;
    }
    else if(KEY1==LOOSE||KEY2==LOOSE||KEY3==LOOSE||KEY4==LOOSE||KEY5==LOOSE)
        keypress_flag = LOOSE;                      //�����ɿ���־
    return 0;
}
/***********************************************
* @brief : ��ȡ�ĸ����������£����Է�ֹ��һ�ΰ������ֶ����Ӧ�����
* @param : void
* @return: key_down:���а��������򷵻ذ����ĸ����������򷵻�0
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
* @brief : ͨ������ѡ����ʾ�ĸ�PID�Ĳ���
* @param : key_num:���µ����ĸ�����
* @return: void
* @date  : 2023.4.2
* @author: L
************************************************/
uint8 PIDDisplay(uint8 key_num)
{
//    system_delay_ms(100);

    switch(key_num)
    {
        case KEY_UP://��ʾ�����ٶȻ�PID����������ѡ�����P��I
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
        case KEY_DOWN://��ʾ�ҵ���ٶȻ�PID����������ѡ�����P��I
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
        case KEY_LEFT://��ʾͼ����PID����������ѡ�����P��D
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
        case KEY_RIGHT://��ʾ��ŷ���PID����������ѡ�����P��D
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
        case KEY_ENTER:return 0;//�˳�
        default:return 9;
    }
    return 9;
}
/***********************************************
* @brief : ��������PID����
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

        uint8 change_parameter = PIDDisplay(KeyGet());//0��ʾ�˳����Σ�9��ʾû��⵽�������£�1~8��ʾ���ĸ�����+��-

//        system_delay_ms(100);
        if(change_parameter == 0)//0�˳�����
        {
            ShowPIDParameter();
            break;
        }
        else if(change_parameter != 9)//9����ѭ��
        {
            tft180_clear();
            while(1)
            {
                uint8 exit_flag = 0;//�˳����εı�־

                tft180_show_string(0, 0, "KEY1:P+1");
                tft180_show_string(0, 10, "KEY2:P-1");
                tft180_show_string(0, 20, "KEY3:I+0.1/D+0.5");
                tft180_show_string(0, 30, "KEY4:I-0.1/D-0.5");
                tft180_show_string(0, 40, "KEY5:exit");
//                system_delay_ms(100);
                switch(KeyGet())
                {
                    case KEY_UP://����P+
                    {
                        tft180_clear();
                        switch(change_parameter)//ѡ���ĸ�PID��P
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
                    case KEY_DOWN://����P-
                    {
                        tft180_clear();
                        switch(change_parameter)//ѡ���ĸ�PID��P
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
                    case KEY_LEFT://����I+
                    {
                        tft180_clear();
                        switch(change_parameter)//ѡ���ĸ�PID��I
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
                    case KEY_RIGHT://����I-
                    {
                        tft180_clear();
                        switch(change_parameter)//ѡ���ĸ�PID��I
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
                    case KEY_ENTER:exit_flag = 1;tft180_clear();break;//�˳��ò����ĵ��Σ��ɼ���ѡ�������һ������
                }
                if(exit_flag == 1) break;
            }
        }
    }
}
/***********************************************
* @brief : ǰհ�ͻ����ٶȰ�������
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
        uint8 exit_flag = 0;//�˳����εı�־
        ShowImageParameter();//��ʾ��ǰ����
        tft180_show_string(0, 50, "KEY3:change aim_distance");
        tft180_show_string(0, 60, "KEY4:change base_speed");
        tft180_show_string(0, 70, "KEY5:exit");
        switch(KeyGet())
        {
//            system_delay_ms(500);
            case KEY_LEFT://�޸�Ԥ����ֵ
            {
                tft180_clear();
                while(1)
                {
                    uint8 exit_flag_1 = 0;//�˳�����Ԥ���ı�־λ
                    tft180_show_string(0, 0, "KEY1:aim_distance+0.01");
                    tft180_show_string(0, 10, "KEY2:aim_distance+0.02");
                    tft180_show_string(0, 20, "KEY3:aim_distance-0.01");
                    tft180_show_string(0, 30, "KEY4:aim_distance-0.02");
                    tft180_show_string(0, 40, "KEY5:exit");
//                    system_delay_ms(500);
                    switch(KeyGet())
                    {
                        case KEY_UP://Ԥ���+0.01
                        {
                            aim_distance+=0.01;
                            tft180_show_string(0, 80, "aim_distance:");
                            tft180_show_float(100, 80, aim_distance, 1, 3);
                            break;
                        }
                        case KEY_DOWN://Ԥ���+0.02
                        {
                            aim_distance+=0.02;
                            tft180_show_string(0, 80, "aim_distance:");
                            tft180_show_float(100, 80, aim_distance, 1, 3);
                            break;
                        }
                        case KEY_LEFT://Ԥ���-0.01
                        {
                            aim_distance-=0.01;
                            tft180_show_string(0, 80, "aim_distance:");
                            tft180_show_float(100, 80, aim_distance, 1, 3);
                            break;
                        }
                        case KEY_RIGHT://Ԥ���-0��02
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
            case KEY_RIGHT://�޸Ļ����ٶ�
            {
                tft180_clear();
                while(1)
                {
                    uint8 exit_flag_1 = 0;//�˳����ڻ����ٶȵı�־
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
            case KEY_ENTER://�˳�����
            {
                ShowImageParameter();//��ʾ��ǰ����
                exit_flag=1;
                break;
            }
        }
        if(exit_flag == 1) break;
    }
}
/***********************************************
* @brief : ��ʾ�Ҷ�ͼ�����͸�Ӻ��ͼ�񣬲���ѡ���Ƿ���ʾ���ߡ����ߵ�
* @param : void
* @return: void
* @date  : 2023.4.2
* @author: L
************************************************/
uint8 binary_image_flag = 0,gray_image_flag = 0;//�Ƿ���ʾ�ҶȻ���͸�Ӻ��ͼ��1Ϊ��ʾ
uint8 edgeline_flag = 0,c_line_flag = 0;        //�Ƿ���ʾ���߻����ߣ�1Ϊ��ʾ
void KeyImage(void)
{
//    system_delay_ms(500);
    while(1)
    {
        uint8 exit_flag = 0;//�˳�ѡ��ͼ����ʾ�ı�־λ
        tft180_show_string(0, 0, "KEY3:show gray_image");
        tft180_show_string(0, 10, "KEY4:show binary_image");
        switch(KeyGet())
        {
//            system_delay_ms(500);
            case KEY_LEFT://��ʾ�Ҷ�ͼ
            {
                gray_image_flag = 1;//��ʾ�Ҷ�ͼ�ı�־
                exit_flag = 1;//�˳�
                break;
            }
            case KEY_RIGHT://��ʾ��͸��ͼ
            {
                tft180_clear();
                while(1)
                {
                    uint8 exit_flag_1 = 0;//�˳�ѡ���Ƿ���ʾ���߻����ߵı�־λ
                    binary_image_flag = 1;//��ʾ��͸��ͼ��
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
* @brief : ��ʾ��ǰ���е�PID����
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
* @brief : ǰհ�ͻ����ٶȲ�����ʾ
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
* @brief : ���������������
* @param : void
* @return: void
* @date  : 2023.4.3
* @author: L
************************************************/
void KEYCtrl(void)
{
    while(1)
    {
        uint8 exit_flag_2 = 0;//�˳�����ģʽ�ı�־λ
        switch(KeyGet())
        {
            case KEY_DOWN:KeyPID();break;//KEY2��ʾ��PID����
            case KEY_LEFT:KeyTrack();break;//KEY3��ʾ��ѭ������
            case KEY_RIGHT:KeyImage();exit_flag_2 = 1;break;//KEY4��ʾѡ����ʾʲôͼ��
            case KEY_ENTER://KEY5��ʾ�����жϣ��������
            {
                system_delay_ms(1000);
                exit_flag_2 = 1;//�˳�����ģʽ
                encoder_clear_count(ENCODER_LEFT);                                      //�����߱���������
                encoder_clear_count(ENCODER_RIGHT);                                     //����ұ߱���������
                pit_enable(CCU60_CH0);
                pit_enable(CCU60_CH1);
//                MotorSetPWM(2000, 2000);
                break;
            }
        }
        if(exit_flag_2 == 1) break;
    }
}
