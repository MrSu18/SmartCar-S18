/*
 * KeyMenu.c
 *
 *  Created on: 2023��5��11��
 *      Author: L
 */
#include "KeyMenu.h"
#include "zf_common_headfile.h"
#include "zf_device_tft180.h"
#include "pid.h"
#include "ImageTrack.h"
#include "ImageProcess.h"
#include "Control.h"
#include "zf_driver_flash.h"

static Menu menu={0,0,0};
static int index=0;
static uint16 speed[8]={70,70,70,70,70,70,70,70};
uint8 per_image_flag=0,gray_image_flag=0;
uint8 edgeline_flag,c_line_flag=0,per_edgeline_flag=0;

uint8 home[12][30]=
{
        "   TurnPID_Image",
        "   TurnPID_ADC",
        "   SpeedPID_Left",
        "   SpeedPID_Right",
        "   GyroPID",
        "   StateMachine",
        "   ProcessSpeed",
        "   BaseTrack",
        "   ShowImage",
        "   WriteToFlash",
        "   ClearFlash",
        "   Exit"
};
uint8 imagepid[3][30]=
{
        "   PIDImage.P:",
        "   PIDImage.D:",
        "   Exit"
};
uint8 adcpid[3][30]=
{
        "   PIDADC.P:",
        "   PIDADC.D:",
        "   Exit"
};
uint8 leftpid[3][30]=
{
        "   PIDLeft.P:",
        "   PIDLeft.I:",
        "   Exit"
};
uint8 rightpid[3][30]=
{
        "   PIDRight.P:",
        "   PIDRight.I:",
        "   Exit"
};
uint8 gyro_pid[4][30]=
{
        "   GyroPID.P:",
        "   GyroPID.I:",
        "   GyroPID.D:",
        "   Exit"
};
uint8 state_machine[10][30]=
{
        " ",
        "   LeftCircle",
        "   RightCircle",
        "   Cross",
        "   Cut",
        "   Slope",
        "   Barrier",
        "   Garage",
        "   Stop",
        "   Exit"
};
uint8 state_speed[9][30]=
{
        "   LeftCircle:",
        "   RightCircle:",
        "   Cross:",
        "   Cut:",
        "   Slope:",
        "   Barrier:",
        "   Garage:",
        "   Stop:",
        "   Exit"
};
uint8 basetrack[3][30]=
{
        "   base_speed:",
        "   aim_distance:",
        "   Exit"
};
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
    gpio_init(P33_9,GPI,LOOSE,GPI_PULL_DOWN);
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
    //��ǰ�а��������£��򷵻ذ��µ��Ǹ�����
    if((KEY1==PRESSDOWN||KEY2==PRESSDOWN||KEY3==PRESSDOWN||KEY4==PRESSDOWN||KEY5==PRESSDOWN))
    {
        if(KEY1==PRESSDOWN) return KEY_UP;
        else if(KEY2==PRESSDOWN) return KEY_ENTER;
        else if(KEY3==PRESSDOWN) return KEY_LEFT;
        else if(KEY4==PRESSDOWN) return KEY_RIGHT;
        else if(KEY5==PRESSDOWN) return KEY_DOWN;
        else if(KEY6==PRESSDOWN) return KEY_OWN;
    }
    else if(KEY1==LOOSE||KEY2==LOOSE||KEY3==LOOSE||KEY4==LOOSE||KEY5==LOOSE||KEY6==LOOSE)
        return 0;
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

    key_value = KEYScan();//ɨ�谴��
    key_down = key_value & (key_value ^ key_old);//��ֹ��һ�ΰ�����Ӧ���
    key_old = key_value;//������һ�ΰ�����ֵ

    return key_down;
}
/***********************************************
* @brief : ��ʾ��ͬҳ�Ĳ˵�
* @param : page:Ҫ��ʾ�Ĳ˵�ҳ
* @return: void
* @date  : 2023.5.13
* @author: L
************************************************/
void ShowFunction(uint8 page)
{
    switch(page)
    {
        case 0:
            for(int i = 0;i < 12;i++)
                tft180_show_string(0, i*10, home[i]);
            break;
        case 1:
            for(int i = 0;i < 3;i++)
                tft180_show_string(0, i*10, imagepid[i]);
            break;
        case 2:
            for(int i = 0;i < 3;i++)
                tft180_show_string(0, i*10, adcpid[i]);
            break;
        case 3:
            for(int i = 0;i < 3;i++)
                tft180_show_string(0, i*10, leftpid[i]);
            break;
        case 4:
            for(int i = 0;i < 3;i++)
                tft180_show_string(0, i*10, rightpid[i]);
            break;
        case 5:
            for(int i = 0;i < 4;i++)
                tft180_show_string(0, i*10, gyro_pid[i]);
            break;
        case 6:
            for(int i = 0;i < 10;i++)
                tft180_show_string(0, i*10, state_machine[i]);
            break;
        case 7:
            for(int i = 0;i < 9;i++)
                tft180_show_string(0, i*10, state_speed[i]);
            break;
        case 8:
            for(int i = 0;i < 3;i++)
                tft180_show_string(0, i*10, basetrack[i]);
            break;
        case 9:
            tft180_show_string(0, 0, "  show per_image");
            tft180_show_string(0, 10,"  show gray_image");
            tft180_show_string(0, 20,"  exit");
            break;
        case 10:
            tft180_show_string(0, 0, "  show peredge_line");
            tft180_show_string(0, 10,"  show center_line");
            tft180_show_string(0, 20,"  exit");
            break;
        case 11:
            tft180_show_string(0, 0, "  show peredge_line");
            tft180_show_string(0, 10,"  show edge_line");
            tft180_show_string(0, 20,"  show center_line");
            tft180_show_string(0, 30,"  exit");
            break;
    }
}
/***********************************************
* @brief : �����������
* @param : void
* @return: void
* @date  : 2023.5.13
* @author: L
************************************************/
void KeyCtrl(void)
{
    while(1)
    {
        uint8 exit_flag = 0;
        ShowFunction(menu.page);//��ʾ��Ӧҳ�������
        tft180_show_string(0, menu.updown*10, ">>");
        switch(KeyGet())
        {
            case KEY_UP://�������
                tft180_show_string(0, menu.updown*10, "  ");
                menu.updown--;
                if(menu.updown < 0) menu.updown = 11;
                tft180_show_string(0, menu.updown*10, ">>");
                break;
            case KEY_DOWN://�������
                tft180_show_string(0, menu.updown*10, "  ");
                menu.updown++;
                if(menu.updown > 11) menu.updown = 0;
                tft180_show_string(0, menu.updown*10, ">>");
                break;
            case KEY_LEFT: SubParameter();break;//������
            case KEY_RIGHT:AddParameter();break;//������
            case KEY_ENTER:EnterKey(&exit_flag);break;//ȷ�ϼ�
        }
        if(exit_flag == 1)//�˳���������
        {
            tft180_clear();
            system_delay_ms(1000);
            encoder_clear_count(ENCODER_LEFT);                                      //�����߱���������
            encoder_clear_count(ENCODER_RIGHT);                                     //����ұ߱���������
            pit_enable(CCU60_CH0);
            pit_enable(CCU60_CH1);
     //       pit_enable(CCU61_CH1);
            break;
        }
    }
}
/***********************************************
* @brief : ��Ӧ����������
* @param : void
* @return: void
* @date  : 2023.5.13
* @author: L
************************************************/
void SubParameter(void)
{
    switch(menu.updown)
    {
        case 0:
            switch(menu.page)
            {
                case 1:
                    turnpid_image.P-=5;
                    tft180_show_float(100, menu.updown*10, turnpid_image.P, 4, 2);
                    break;
                case 2:
                    turnpid_adc.P-=5;
                    tft180_show_float(100, menu.updown*10, turnpid_adc.P, 4, 2);
                    break;
                case 3:
                    speedpid_left.P-=10;
                    tft180_show_float(100, menu.updown*10, speedpid_left.P, 4, 2);
                    break;
                case 4:
                    speedpid_right.P-=10;
                    tft180_show_float(100, menu.updown*10, speedpid_right.P, 4, 2);
                    break;
                case 5:
                    gyropid.P+=0.01;
                    tft180_show_float(100, menu.updown*10, gyropid.P, 2, 5);
                    break;
                case 8:
                    original_speed-=5;
                    tft180_show_int(100, menu.updown*10, original_speed, 3);
                    break;
            }
            break;
        case 1:
            switch(menu.page)
            {
                case 1:
                    turnpid_image.D-=1;
                    tft180_show_float(100, menu.updown*10, turnpid_image.D, 4, 2);
                    break;
                case 2:
                    turnpid_adc.D-=1;
                    tft180_show_float(100, menu.updown*10, turnpid_adc.D, 4, 2);
                    break;
                case 3:
                    speedpid_left.I-=0.1;
                    tft180_show_float(100, menu.updown*10, speedpid_left.I, 4, 2);
                    break;
                case 4:
                    speedpid_right.I-=0.1;
                    tft180_show_float(100, menu.updown*10, speedpid_right.I, 4, 2);
                    break;
                case 5:
                    gyropid.I+=0.0001;
                    tft180_show_float(100, menu.updown*10, gyropid.I, 2, 5);
                    break;
                case 8:
                    aim_distance-=0.01;
                    tft180_show_float(100, menu.updown*10, aim_distance, 1,3);
                    break;
            }
            break;
        case 2:
            switch(menu.page)
            {
                case 5:
                    gyropid.D+=0.00005;
                    tft180_show_float(100, menu.updown*10, gyropid.D, 2, 6);
                    break;
            }
            break;
    }
    if(menu.page==7)
    {
        speed[menu.updown]-=2;
        tft180_show_uint(100, menu.updown*10, speed[menu.updown], 2);
    }
}
/***********************************************
* @brief : ��Ӧ�����Ӳ���
* @param : void
* @return: void
* @date  : 2023.5.13
* @author: L
************************************************/
void AddParameter(void)
{
    switch(menu.updown)
    {
        case 0:
            switch(menu.page)
            {
                case 1:
                    turnpid_image.P+=5;
                    tft180_show_float(100, menu.updown*10, turnpid_image.P, 4, 2);
                    break;
                case 2:
                    turnpid_adc.P+=5;
                    tft180_show_float(100, menu.updown*10, turnpid_adc.P, 4, 2);
                    break;
                case 3:
                    speedpid_left.P+=10;
                    tft180_show_float(100, menu.updown*10, speedpid_left.P, 4, 2);
                    break;
                case 4:
                    speedpid_right.P+=10;
                    tft180_show_float(100, menu.updown*10, speedpid_right.P, 4, 2);
                    break;
                case 5:
                    gyropid.P-=0.01;
                    tft180_show_float(100, menu.updown*10, gyropid.P, 2, 5);
                    break;
                case 8:
                    original_speed+=5;
                    tft180_show_int(100, menu.updown*10, original_speed, 3);
                    break;
            }
            break;
        case 1:
            switch(menu.page)
            {
                case 1:
                    turnpid_image.D+=1;
                    tft180_show_float(100, menu.updown*10, turnpid_image.D, 4, 2);
                    break;
                case 2:
                    turnpid_adc.D+=1;
                    tft180_show_float(100, menu.updown*10, turnpid_adc.D, 4, 2);
                    break;
                case 3:
                    speedpid_left.I+=0.1;
                    tft180_show_float(100, menu.updown*10, speedpid_left.I, 4, 2);
                    break;
                case 4:
                    speedpid_right.I+=0.1;
                    tft180_show_float(100, menu.updown*10, speedpid_right.I, 4, 2);
                    break;
                case 5:
                    gyropid.I-=0.0001;
                    tft180_show_float(100, menu.updown*10, gyropid.I, 2, 5);
                    break;
                case 8:
                    aim_distance+=0.01;
                    tft180_show_float(100, menu.updown*10, aim_distance, 1,3);
                    break;
            }
            break;
        case 2:
            switch(menu.page)
            {
                case 5:
                    gyropid.D-=0.00005;
                    tft180_show_float(100, menu.updown*10, gyropid.D, 2, 6);
                    break;
            }
            break;
    }
    if(menu.page==7)
    {
        speed[menu.updown]+=2;
        tft180_show_uint(100, menu.updown*10, speed[menu.updown], 2);
    }
}
/***********************************************
* @brief : ����ȷ�ϰ�����ʵ�ֲ�ͬ�Ĺ���
* @param : exit_flag:�˳��������εı�־λ
* @return: void
* @date  : 2023.5.14
* @author: L
************************************************/
void EnterKey(uint8* exit_flag)
{
    if(menu.enter==0)//һ���˵�
    {
        uint8 exit_flag_1 = 0;//
        switch(menu.page)
        {
            case 0://��������
                if(menu.updown<9)//�����˳���
                {
                    menu.page=menu.updown+1;//��ʾ��Ӧ����һҳ������
                    if(menu.updown==8) menu.enter=1;//�������˵�
                    menu.updown=0;
                    tft180_clear();
                }
                else if(menu.updown==9)//д��flash
                    WriteToFlash();
                else if(menu.updown==10)//����flash
                {
                    if(flash_check(0, 0)==1)//���Flash��Ϊ�գ������
                        flash_erase_page(0, 0);
                    flash_buffer_clear();//��ջ�����
                }
                else if(menu.updown==11) *exit_flag = 1;//�˳���������
                break;
            //��ͬҳ�淵��������
            case 1:
            case 2:
            case 3:
            case 4:
            case 8:
                if(menu.updown==2) exit_flag_1 = 1;
                break;
            case 5:
                if(menu.updown==3) exit_flag_1 = 1;
                break;
            case 6://����״̬��˳��
                if(menu.updown==8)
                {
                    process_status[index]='S';
                    tft180_show_uint((index+1)*10, 0, menu.updown, 1);
                }
                else if(menu.updown==9) exit_flag_1=1;//�˳�����״̬��
                else
                {
                    process_status[index]=(uint8)menu.updown;
                    tft180_show_uint((index+1)*10, 0, process_status[index], 1);
                    index++;
                }
                break;
            case 7://�޸�״̬����Ӧ���ٶ�
                //����Ӧ�ٶȸ�����Ӧ��״̬��˳��
                if(menu.updown==8)
                {
                    exit_flag_1=1;
                    for(int i=0;i<15;i++)
                    {
                        if(process_status[i]==0) break;
                        else if(process_status[i]=='S') process_speed[i]=speed[7];
                        else process_speed[i]=speed[process_status[i]-1];
                    }
                }
                break;
        }
        if(exit_flag_1)//����������
        {
            index=0;
            menu.page=0;
            menu.updown=0;
            tft180_clear();
        }
    }
    else if(menu.enter==1)//�����˵�����ʾͼ��
    {
        if(menu.updown<2)
        {
            menu.page=menu.updown+10;
            menu.updown=0;
            tft180_clear();
            menu.enter=2;
        }
        else if(menu.updown==2)
        {
            menu.page=0;
            menu.updown=0;
            tft180_clear();
            menu.enter=0;
        }
    }
    else if(menu.enter==2)//�����˵�����ʾ���ߺ�����
    {
        if(menu.page==10)
        {
            per_image_flag=1;
            switch(menu.updown)
            {
                case 0:per_edgeline_flag=1;break;
                case 1:c_line_flag=1;break;
                case 2:*exit_flag=1;break;
            }
        }
        else if(menu.page==11)
        {
            gray_image_flag=1;
            switch(menu.updown)
            {
                case 0:per_edgeline_flag=1;break;
                case 1:edgeline_flag=1;break;
                case 2:c_line_flag=1;break;
                case 3:*exit_flag=1;break;
            }
        }
    }
}
/***********************************************
* @brief : ����Ҫ�޸ĵı���д��Flash
* @param : void
* @return: void
* @date  : 2023.5.14
* @author: L
************************************************/
void WriteToFlash(void)
{
    if(flash_check(0, 0)==1)//���Flash��ֵ������
        flash_erase_page(0, 0);
    flash_buffer_clear();//��ջ�����
    //����Ҫ�޸ĵı���д��������
    flash_union_buffer[0].uint32_type=*(uint32*)&turnpid_image.P;
    flash_union_buffer[1].uint32_type=*(uint32*)&turnpid_image.D;
    flash_union_buffer[2].uint32_type=*(uint32*)&turnpid_adc.P;
    flash_union_buffer[3].uint32_type=*(uint32*)&turnpid_adc.D;
    flash_union_buffer[4].uint32_type=*(uint32*)&speedpid_left.P;
    flash_union_buffer[5].uint32_type=*(uint32*)&speedpid_left.I;
    flash_union_buffer[6].uint32_type=*(uint32*)&speedpid_right.P;
    flash_union_buffer[7].uint32_type=*(uint32*)&speedpid_right.I;
    flash_union_buffer[8].uint32_type=*(uint32*)&gyropid.P;
    flash_union_buffer[9].uint32_type=*(uint32*)&gyropid.I;
    flash_union_buffer[10].uint32_type=*(uint32*)&gyropid.D;
    flash_union_buffer[11].uint32_type=*(uint32*)&original_speed;
    flash_union_buffer[12].uint32_type=*(uint32*)&aim_distance;
    for(int i=13;i<13+15;i++)
    {
        flash_union_buffer[i].uint32_type=*((uint32*)process_status+(i-13));
        flash_union_buffer[i+15].uint32_type=*((uint32*)process_speed+(i-13));
    }
    flash_write_page_from_buffer(0, 0);//���������е�ֵд��Flash
}
/***********************************************
* @brief : ��Flash�ж�ȡֵ����Ӧ�ı���
* @param : void
* @return: void
* @date  : 2023.5.14
* @author: L
************************************************/
void ReadFromFlash(void)
{
    if(flash_check(0, 0)==1)//���Flash����ֵ�����ȡ�����򲻶�ȡ
    {
        flash_read_page_to_buffer(0, 0);//��ȡFlash��ֵ
        //�ӻ�������ö�Ӧ������ֵ
        turnpid_image.P=flash_union_buffer[0].float_type;
        turnpid_image.D=flash_union_buffer[1].float_type;
        turnpid_adc.P=flash_union_buffer[2].float_type;
        turnpid_adc.D=flash_union_buffer[3].float_type;
        speedpid_left.P=flash_union_buffer[4].float_type;
        speedpid_left.I=flash_union_buffer[5].float_type;
        speedpid_right.P=flash_union_buffer[6].float_type;
        speedpid_right.I=flash_union_buffer[7].float_type;
        gyropid.P=flash_union_buffer[8].float_type;
        gyropid.I=flash_union_buffer[9].float_type;
        gyropid.D=flash_union_buffer[10].float_type;
        original_speed=flash_union_buffer[11].uint16_type;
        aim_distance=flash_union_buffer[12].float_type;
        for(int i=13;i<13+15;i++)
        {
            process_status[i-13]=flash_union_buffer[i].uint8_type;
            process_speed[i-13]=flash_union_buffer[i+15].uint16_type;
        }
        flash_buffer_clear();//��ջ�����
    }
}
