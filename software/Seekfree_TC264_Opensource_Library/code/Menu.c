/*
 * Menu.c
 *
 *  Created on: 2023年7月7日
 *      Author: L
 */
#if 1
#include "Menu.h"
#include "ImageProcess.h"
#include "ADC.h"
#include "motor.h"
#include "icm20602.h"
#include "zf_common_headfile.h"
#include "zf_device_tft180.h"
#include "zf_driver_flash.h"
#include "pid.h"
#include "ImageTrack.h"
#include "isr.h"
#include "ImageConversion.h"

#define TFT_MAX_LINE 11 //可显示12行 从0开始数起
#define Unknown_sta -5 
#define Unselected -1

uint8 per_image_flag=0,gray_image_flag=0;
uint8 edgeline_flag=0,c_line_flag=0,per_edgeline_flag=0;
uint8 Return_flag=0; //菜单返回标志  待修正
int16 Change_EXP_TIME_DEF=MT9V03X_EXP_TIME_DEF;  //要想曝光生效，必须要改摄像头配置文件里的参数

typedef enum Menu {
    //一级菜单
    HOME,
    //二级菜单
    State_machine,
    Parameter,
    Checkout,
    //三级菜单
    /*Parameter*/
    kPid,
    kState_speed,
    kBasetrack,
    kprocess_speed,
    kEXP_TIME,
    kOutgarage,
    /*Parameter*/
    /*Checkout*/
    kImage,
    kTof,
    kEncoder,
    kMotor,
    kADC,
    kMPU,
    /*Checkout*/
    //四级菜单
    /*kPid*/
    kImagepid,
    kAdcpid,
    kLeftpid,
    kRightpid,
    kGyropid,
    /*kPid*/
    /*kState_speed*/
    kLeftCircle,
    kRightCircle,
    kCut,
    kSlope,
    kBarrier,
    /*kState_speed*/
    /*kImage*/
    kper_image,
    kgray_image,
    ksobel,
    /*kImage*/
}Menu;

int page;
short select;
bool departure;
int key_N=1;//按键步长

/***********************************************
* @brief : 按键初始化
* @param : void
* @return: void
* @date  : 2023.7.7
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
    page = HOME;
    departure=1;
}
/***********************************************
* @brief : 按键扫描程序，扫描是否有按键被按下
* @param : void
* @return: 若有按键按下则返回按下哪个按键，否则返回0
* @date  : 2023.7.7
* @author: L
************************************************/
uint8 KEYScan(void)
{
    //当前有按键被按下，则返回按下的那个按键
    if(KEY1==PRESS||KEY2==PRESS||KEY3==PRESS||KEY4==PRESS||KEY5==PRESS||KEY6==PRESS||KEY7==PRESS)
    {
        if(KEY1==PRESS) return KEY_UP;
        else if(KEY2==PRESS) return KEY_ENTER;
        else if(KEY3==PRESS) return KEY_LEFT;
        else if(KEY4==PRESS) return KEY_RIGHT;
        else if(KEY5==PRESS) return KEY_DOWN;
        else if(KEY6==PRESS) return KEY_TOP;
        else if(KEY7==PRESS) return KEY_LOW;
    }
    else if(KEY1==LOOSE||KEY2==LOOSE||KEY3==LOOSE||KEY4==LOOSE||KEY5==LOOSE||KEY6==LOOSE||KEY7==LOOSE)
        return 0;
    return 0;
}
/***********************************************
* @brief : 获取哪个按键被按下，可以防止按一次按键出现多次响应的情况
* @param : void
* @return: key_down:若有按键按下则返回按下哪个按键，否则返回0
* @date  : 2023.7.7
* @author: L
************************************************/
uint8 KeyGet(void)
{
    static uint8 key_value = 0;
    static uint8 key_down  = 0;
    static uint8 key_old   = 0;

    key_value = KEYScan();//扫描按键
    key_down = key_value & (key_value ^ key_old);//防止按一次按键响应多次
    key_old = key_value;//更新上一次按键的值

    return key_down;
}

/***********************************************
* @brief : 按键选择功能
* @param : Maxsize 当前目录最大行数
* @return: 未确认既为Unselected(-1),未知状态为Unknown_sta(-5)
* @date  : 2023.7.7
* @author: L
************************************************/
uint8 LR_flag;//左右摇杆标志
#define PushLeft 2
#define PushRight 3
#define KEY_EXIT 13
short KeySelect(uint8 Page_Maxline){
    if(select==KEY_EXIT)select = 0;
    tft180_show_string(0, select*10, ">>");
    switch(KeyGet())
        {
            case KEY_UP://光标向上
                tft180_show_string(0, select*10, "  ");
                select--;
                if(select < 0) select = Page_Maxline;
                tft180_show_string(0, select*10, ">>");
                break;
            case KEY_DOWN://光标向下
                tft180_show_string(0, select*10, "  ");
                select++;
                if(select > Page_Maxline) select = 0;
                tft180_show_string(0, select*10, ">>");
                break;
            case KEY_LEFT: LR_flag=PushLeft;return PushLeft;break;//参数减    '-'  = 45
            case KEY_RIGHT:LR_flag=PushRight;return PushRight;break;//参数加   '+'  = 43
            case KEY_ENTER:
            if (select>Page_Maxline)    return Unselected;
            else return select;                     break;//确认键
            case KEY_LOOSE:                         break;//未按下
            case KEY_TOP: select =KEY_EXIT;   return KEY_EXIT;  break;//按下top键退出，滑动菜单前的无奈之举
            case KEY_LOW: key_N = (key_N*10)%1000;if(key_N==0) key_N =1; break;//很蠢的循环，可改进
            default :       return Unknown_sta;   break;//未知状态

        }
        return Unselected;
}
void MenuErrorLog(const char Page[]){
    tft180_show_string(30,10*TFT_MAX_LINE,Page);
    tft180_show_string(80,10*TFT_MAX_LINE,"Error");
}

short my_sel;
void Func_Home(Menu Father,uint8 Maxsize){
    //  for(int i = 0;i < Maxsize;i++)
    // tft180_show_string(0, i*10, home[i]);//这里可以集成为一个显示内容的字符数组，但节约内存暂时不考虑
    //-----------Display-------------------//

     tft180_show_string(20,10*Line1,"State_machine");
     tft180_show_string(20,10*Line2,"Parameter");
     tft180_show_string(20,10*Line3,"Checkout");
     tft180_show_string(20,10*Maxsize,"EXIT");
    //------------FUNC-----------------------//
  do{
         my_sel=KeySelect(Maxsize);
     }
    while(my_sel<0);
    switch (select)
    {

    case Line1:
        page = State_machine;
        LR_flag = PushRight;//默认展开
        tft180_clear();
        break;
    case Line2:
        page = Parameter;
        tft180_clear();
        break;
     case Line3:
        page = Checkout;
        tft180_clear();
        break;
    case Line4:
        page = Father;
        tft180_clear();
        departure=0;
        break;
    default:
        select=0;//宏定义
        tft180_clear();
        break;
    }

    //-------EXIT-----------//
   
}
uint8 No_States;
void Func_State_machine(Menu Father,uint8 Maxsize){
    //  for(int i = 0;i < Maxsize;i++)
    // tft180_show_string(0, i*10, home[i]);//这里可以集成为一个显示内容的字符数组，但节约内存暂时不考虑
    //-----------Display-------------------//
     tft180_show_string(20,10*Line1 ,"LeftCircle");
     tft180_show_string(20,10*Line2 ,"RightCircle");
     tft180_show_string(20,10*Line3 ,"Cross");
     tft180_show_string(20,10*Line4 ,"Cut");
     tft180_show_string(20,10*Line5 ,"Slope");
     tft180_show_string(20,10*Line6 ,"Barrier");
     tft180_show_string(20,10*Line7 ,"LeftGarage");
     tft180_show_string(20,10*Line8 ,"RightGarage");
     tft180_show_string(20,10*Line9 ,"PassGarage");
     tft180_show_string(20,10*Line10 ,"Stop");
     tft180_show_string(20,10*Line11 ,"Encoder");
     tft180_show_string(20,10*Line12 ,"Gyro");
     for(int i=0;i<30;i++){
         if(process_status[i]<10)//0-9的状态
         tft180_show_int(110+10*(i%3),10*(Line3+i/3) ,process_status[i],1);
         else
         tft180_show_char(110+10*(i%3),10*(Line3+i/3),process_status[i]);
         if(process_status[i]==0) break;
     }
     //------------------EXIT----------------//
//     tft180_show_string(20,10*Maxsize,"Exit");
    //-------------------FUNC----------------//
    while(KeySelect(Maxsize)<0){
        tft180_show_int(110,10*Line1,No_States,2); //显示第几个状态
    }
    switch (select)
    {
        case Line1:
        case Line2:
        case Line3:
        case Line4:
        case Line5:
        case Line6:
        case Line7:
        case Line8:
        case Line9:
            process_status[No_States]=select+1;//选择的是行数 与具体行数是-1的关系
            ProcessPropertyDefault(No_States);
          //  tft180_show_int(110+10*(No_States%3),10*(Line3+No_States/3) ,process_status[No_States],1);
            if(LR_flag == PushRight) {
                page = kState_speed;
                tft180_clear();
            }
            No_States++;
            break;
        case Line10:
            process_status[No_States]='S';
            ProcessPropertyDefault(No_States);
            tft180_show_char(110+10*(No_States%3),10*(Line3+No_States/3) ,process_status[No_States]);
            if(LR_flag == PushRight) {
                page = kState_speed;
                tft180_clear();
            }
            No_States++;
            break;
        case Line11:
            process_status[No_States]='E';
            ProcessPropertyDefault(No_States);
            tft180_show_char(110+10*(No_States%3),10*(Line3+No_States/3) ,process_status[No_States]);
            if(LR_flag == PushRight) {
                page = kState_speed;
                tft180_clear();
            }
            No_States++;
            break;
        case Line12:
            process_status[No_States]='G';
            ProcessPropertyDefault(No_States);
            tft180_show_char(110+10*(No_States%3),10*(Line3+No_States/3) ,process_status[No_States]);
            if(LR_flag == PushRight) {
                page = kState_speed;
                tft180_clear();
            }
            No_States++;
            break;
//--------------EXIT---------------------------------//
        case Line14:
            page = Father;
//            process_status_cnt=No_States;
            No_States=0;
            LR_flag =0;
            tft180_clear();
            break;
        default:
            MenuErrorLog("state_machine");
            break;
    }
}
void Func_SetStateSpeed(uint8 No_States){}//待废除
void Func_Parameter(Menu Father,uint8 Maxsize){
     //-----------Display-------------------//
     tft180_show_string(20,10*Line1,"Pid");
     tft180_show_string(20,10*Line2,"process_speed");
     tft180_show_string(20,10*Line3,"Basetrack");
     tft180_show_string(20,10*Line4,"EXP_TIME");
     tft180_show_string(20,10*Line5,"OutGarage_dir");
     tft180_show_string(20,10*Maxsize,"EXIT");
    
    //------------FUNC-----------------------//
  do{
         my_sel=KeySelect(Maxsize);
     }
    while(my_sel<0);
    switch (select)
    {
    case Line1:
        page = kPid;
        tft180_clear();
        break;
    case Line2:
        page = kprocess_speed;
        tft180_clear();
        break;
    case Line3:
        page = kBasetrack;
        tft180_clear();
        break;
    case Line4:
        page = kEXP_TIME;
        tft180_clear();
        break;
    case Line5:
        page = kOutgarage;
        tft180_clear();
        break;
    case Line6:
        page = Father;
        tft180_clear();
        break;
    default:
        select=0;//宏定义
        break;
    }
}
void Func_Checkout(Menu Father,uint8 Maxsize){
       //-----------Display-------------------//
     tft180_show_string(20,10*Line1,"kImage");
     tft180_show_string(20,10*Line2,"kTof");
     tft180_show_string(20,10*Line3,"kEncoder");
     tft180_show_string(20,10*Line4,"kMotor");
     tft180_show_string(20,10*Line5,"kAdc");
     tft180_show_string(20,10*Line6,"kMpu");
     tft180_show_string(20,10*Maxsize,"EXIT");
    //------------FUNC-----------------------//
  do{
         tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
         my_sel=KeySelect(Maxsize);
     }
    while(my_sel<0);
     tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");
    switch (select)
    {

    case Line1:
        page = kImage;
        tft180_clear();
        break;
    case Line2:
        page = kTof;
        tft180_clear();
        break;
        case Line3:
        page = kEncoder;
        tft180_clear();
        break;
    case Line4:
        page = kMotor;
        tft180_clear();
        break;
    case Line5:
        page = kADC;
        tft180_clear();
        break;
    case Line6:
        page = kMPU;
        tft180_clear();
        break;
    case Line7:
        page = Father;
        tft180_clear();
        break;
    default:
        select=0;//宏定义
        break;
    }
    //-------EXIT-----------//
}
void Func_Pid(Menu Father,uint8 Maxsize){
      //-----------Display-------------------//
     tft180_show_string(20,10*Line1,"kLeftpid");
     tft180_show_string(20,10*Line2,"kRightpid");
     tft180_show_string(20,10*Line3,"Gyro_pid");
     tft180_show_string(20,10*Line4,"kImagepid");
     tft180_show_string(20,10*Line5,"kAdcpid");
     tft180_show_string(20,10*Maxsize,"EXIT");
    
    //------------FUNC-----------------------//
  do{
         tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
         my_sel=KeySelect(Maxsize);
     }
    while(my_sel<0);
     tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

    switch (select)
    {

    case Line1:
        page = kLeftpid;
        tft180_clear();
        break;
    case Line2:
        page = kRightpid;
        tft180_clear();
        break;
    case Line3:
        page = kGyropid;
        tft180_clear();
        break;
    case Line4:
        page = kImagepid;
        tft180_clear();
        break;
    case Line5:
        page = kAdcpid;
        tft180_clear();
        break;
    case Line6:
        page = Father;
        tft180_clear();
        break;

    default:
        select=0;//宏定义
        break;
    }
}
void Func_State_speed(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
    switch(process_status[No_States-1]){//因为每次选择完之后nostate都会+1，于是到这里就进行-1操作
        case 1:
            tft180_show_string(20,10*Line1,"LeftCircle");
            break;
        case 2:
            tft180_show_string(20,10*Line1,"RightCircle");
            break;
        case 3:
            tft180_show_string(20,10*Line1,"Cross");
            break;
        case 4:
            tft180_show_string(20,10*Line1,"Cut");
            break;
        case 5:
            tft180_show_string(20,10*Line1,"Slope");
            break;
        case 6:
            tft180_show_string(20,10*Line1,"Barrier");
            break;
        case 7:
            tft180_show_string(20,10*Line1,"LeftGarage");
            break;
        case 8:
            tft180_show_string(20,10*Line1,"RightGarage");
            break;
        case 9:
            tft180_show_string(20,10*Line1,"PassGarage");
            break;
        case 'S':
            tft180_show_string(20,10*Line1,"Stop");
            break;
        case 'E':
            tft180_show_string(20,10*Line1,"Encoder");
            break;
        case 'G':
            tft180_show_string(20,10*Line1,"Gyro");
            break;
        default:
            tft180_show_string(20,10*Line1,"unkown");
            break;
    }
        tft180_show_string(20,10*Line2,"MaxSpeed:");
        tft180_show_int(90,10*Line2,process_property[No_States-1].max_speed,4);
        tft180_show_string(20,10*Line3,"MinSpeed:");
        tft180_show_int(90,10*Line3,process_property[No_States-1].min_speed,4);
        tft180_show_string(20,10*Line4,"integral:");
        tft180_show_int(90,10*Line4,process_property[No_States-1].integral,4);
        tft180_show_string(20,10*Maxsize,"EXIT");
       //------------FUNC-----------------------//
     do{
        }
       while(KeySelect(Maxsize)<0);

       switch (select)
       {
       case Line2:
           if(LR_flag==PushRight) process_property[No_States-1].max_speed+=key_N;
           else if(LR_flag==PushLeft) process_property[No_States-1].max_speed-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line3:
           if(LR_flag==PushRight) process_property[No_States-1].min_speed+=key_N;
           else if(LR_flag==PushLeft) process_property[No_States-1].min_speed-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line4:
           if(LR_flag==PushRight) process_property[No_States-1].integral+=key_N;
           else if(LR_flag==PushLeft) process_property[No_States-1].integral-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line5:
           if(Father==kprocess_speed){
               Return_flag =1;
           }
           page = Father;
           tft180_clear();
           break;
       default:
           select=Line2;//宏定义
           break;
       }
}

void Func_process_speed(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
           select=my_sel;
           tft180_show_string(50,10*Line1,"Process_speed");

           tft180_show_string(20,10*Maxsize,"EXIT");

           for(int i=0;i<30||process_status[i]!=0;i++){
                   if(process_status[i]<10)//0-9的状态
                   tft180_show_int(20+10*(i%8),10*(Line2+((i/8)*2)),process_status[i],1);
                   else
                   tft180_show_char(20+10*(i%8),10*(Line2+((i/8)*2)),process_status[i]);
               }
           if(select<TFT_MAX_LINE&&select!=Line2)
           tft180_show_char(20+10*(No_States%8),10*select,'1');

          //------------FUNC-----------------------//
           //屎山第一步，有机会优化底层key_selete
        do
        {
          }while(KeySelect(Maxsize)<0);
        if(select<TFT_MAX_LINE)
          my_sel=select;
          switch (select)
          {
          case Line3:
          case Line5:
          case Line7:
          case Line9:
              if(LR_flag==PushRight) No_States=(No_States+1)%8;
              else if(LR_flag==PushLeft) No_States=(No_States-1)%8;
              else  MenuErrorLog("PRO_SPEED"); //Loading
              break;
          case Line10:
              page = Father;
              No_States=0;

              break;
          case KEY_EXIT://暂时充当跳转位
              Return_flag =0;
              uint8 CacheOfNo_States =No_States;
              No_States = No_States+1+((my_sel/2-1)*8);//特殊译码进入状态机
              tft180_clear();

              do{
              Func_State_speed(kprocess_speed,4);
              }while(Return_flag !=1);
              No_States=CacheOfNo_States;

              break;
          default:
              //select=Line2;//宏定义
              break;
          }
}
void Func_Basetrack(Menu Father,uint8 Maxsize){
        //-----------Display-------------------//
         tft180_show_string(50,10*Line1,"Basetrack");
         tft180_show_string(20,10*Line2,"aim_distance:");
         tft180_show_float(110,10*Line2,aim_distance,2,3);
         tft180_show_string(20,10*Maxsize,"EXIT");
        //------------FUNC-----------------------//
      do{
             my_sel=KeySelect(Maxsize);
         }
        while(my_sel<0);
        switch (select)
        {
        case Line2:
            if(LR_flag==PushRight) aim_distance+=key_N*0.001;
            else if(LR_flag==PushLeft) aim_distance-=key_N*0.001;
            else  MenuErrorLog("aim"); //Loading
            break;
        case Line3:
            page = Father;
            tft180_clear();
            break;
        default:
            select=1;//宏定义
            break;
        }
}
void Func_Imagepid(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
      tft180_show_string(20,10*Line1,"Img.P:");
      tft180_show_float(80,10*Line1,turnpid_image.P,4,2);
      tft180_show_string(20,10*Line2,"Img.I:");
      tft180_show_float(80,10*Line2,turnpid_image.I,4,2);
      tft180_show_string(20,10*Line3,"Img.D:");
      tft180_show_float(80,10*Line3,turnpid_image.D,4,2);
      tft180_show_string(20,10*Maxsize,"EXIT");

     //------------FUNC-----------------------//
   do{
          tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
          my_sel=KeySelect(Maxsize);
      }
     while(my_sel<0);
      tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

     switch (select)
     {
     case Line1:
         if(LR_flag==PushRight) turnpid_image.P+=key_N;
         else if(LR_flag==PushLeft) turnpid_image.P-=key_N;
         else   MenuErrorLog("ImaPID"); //Loading
         break;
     case Line2:
         if(LR_flag==PushRight) turnpid_image.I+=key_N;
         else if(LR_flag==PushLeft) turnpid_image.I-=key_N;
         else  MenuErrorLog("ImaPID"); //Loading
         break;
     case Line3:
         if(LR_flag==PushRight) turnpid_image.D+=key_N;
         else if(LR_flag==PushLeft) turnpid_image.D-=key_N;
         else  MenuErrorLog("ImaPID"); //Loading
         break;
     case Line4:
         page = Father;
         tft180_clear();
         break;

     default:
         select=0;//宏定义
         break;
     }
}
void Func_ADCpid(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
         tft180_show_string(20,10*Line1,"PIDADC.P:");
         tft180_show_float(80,10*Line1,turnpid_adc.P,4,2);
         tft180_show_string(20,10*Line2,"PIDADC.I:");
         tft180_show_float(80,10*Line2,turnpid_adc.I,4,2);
         tft180_show_string(20,10*Line3,"PIADC.D:");
         tft180_show_float(80,10*Line3,turnpid_adc.D,4,2);
         tft180_show_string(20,10*Maxsize,"EXIT");
        //------------FUNC-----------------------//
      do{
             tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
             my_sel=KeySelect(Maxsize);
         }
        while(my_sel<0);
         tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

        switch (select)
        {
        case Line1:
            if(LR_flag==PushRight) turnpid_adc.P+=key_N;
            else if(LR_flag==PushLeft) turnpid_adc.P-=key_N;
            else  MenuErrorLog("ADCPID"); //Loading
            break;
        case Line2:
            if(LR_flag==PushRight) turnpid_adc.I+=key_N;
            else if(LR_flag==PushLeft) turnpid_adc.I-=key_N;
            else  MenuErrorLog("ADCPID"); //Loading
            break;
        case Line3:
            if(LR_flag==PushRight) turnpid_adc.D+=key_N;
            else if(LR_flag==PushLeft) turnpid_adc.D-=key_N;
            else  MenuErrorLog("ADCPID"); //Loading
            break;
        case Line4:
            page = Father;
            tft180_clear();
            break;
        default:
            select=0;//宏定义
            break;
        }
}
void Func_Leftpid(Menu Father,uint8 Maxsize){
            //-----------Display-------------------//
            tft180_show_string(20,10*Line1,"PIDLeft.P:");
            tft180_show_float(80,10*Line1,speedpid_left.P,4,2);
            tft180_show_string(20,10*Line2,"PIDLeft.I:");
            tft180_show_float(80,10*Line2,speedpid_left.I,4,2);
            tft180_show_string(20,10*Line3,"PIDLeft.D:");
            tft180_show_float(80,10*Line3,speedpid_left.D,4,2);
            tft180_show_string(20,10*Maxsize,"EXIT");
            //------------FUNC-----------------------//
         do{
                tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
                my_sel=KeySelect(Maxsize);
            }
           while(my_sel<0);
            tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

           switch (select)
           {
           case Line1:
               if(LR_flag==PushRight) speedpid_left.P+=key_N;
               else if(LR_flag==PushLeft) speedpid_left.P-=key_N;
               else  MenuErrorLog("speedpid_left"); //Loading
               break;
           case Line2:
               if(LR_flag==PushRight) speedpid_left.I+=key_N;
               else if(LR_flag==PushLeft) speedpid_left.I-=key_N;
               else  MenuErrorLog("speedpid_left"); //Loading
               break;
           case Line3:
               if(LR_flag==PushRight) speedpid_left.D+=key_N;
               else if(LR_flag==PushLeft) speedpid_left.D-=key_N;
               else  MenuErrorLog("speedpid_left"); //Loading
               break;
           case Line4:
               page = Father;
               tft180_clear();
               break;
           default:
               select=0;//宏定义
               break;
           }
}
void Func_Rightpid(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
            tft180_show_string(20,10*Line1,"PIDRIGHT.P:");
            tft180_show_float(80,10*Line1,speedpid_right.P,4,2);
            tft180_show_string(20,10*Line2,"PIDRIGHT.I:");
            tft180_show_float(80,10*Line2,speedpid_right.I,4,2);
            tft180_show_string(20,10*Line3,"PIDRIGHT.D:");
            tft180_show_float(80,10*Line3,speedpid_right.D,4,2);
            tft180_show_string(20,10*Maxsize,"EXIT");
           //------------FUNC-----------------------//
         do{
                tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
                my_sel=KeySelect(Maxsize);
            }
           while(my_sel<0);
            tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

           switch (select)
           {
           case Line1:
               if(LR_flag==PushRight) speedpid_right.P+=key_N;
               else if(LR_flag==PushLeft) speedpid_right.P-=key_N;
               else  MenuErrorLog("speedpid_right"); //Loading
               break;
           case Line2:
               if(LR_flag==PushRight) speedpid_right.I+=key_N;
               else if(LR_flag==PushLeft) speedpid_right.I-=key_N;
               else  MenuErrorLog("speedpid_right"); //Loading
               break;
           case Line3:
               if(LR_flag==PushRight) speedpid_right.D+=key_N;
               else if(LR_flag==PushLeft) speedpid_right.D-=key_N;
               else  MenuErrorLog("speedpid_right"); //Loading
               break;
           case Line4:
               page = Father;
               tft180_clear();
               break;
           default:
               select=0;//宏定义
               break;
           }
}
void Func_Gyropid(Menu Father,uint8 Maxsize){
            //-----------Display-------------------//
            tft180_show_string(20,10*Line1,"Gyro.P:");
            tft180_show_float(80,10*Line1,gyropid.P,4,5);
            tft180_show_string(20,10*Line2,"Gyro.I:");
            tft180_show_float(80,10*Line2,gyropid.I,4,5);
            tft180_show_string(20,10*Line3,"Gyro.D:");
            tft180_show_float(80,10*Line3,gyropid.D,4,5);
            tft180_show_string(20,10*Maxsize,"EXIT");
           //------------FUNC-----------------------//
         do{
                tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
                my_sel=KeySelect(Maxsize);
            }
           while(my_sel<0);
            tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

           switch (select)
           {
           case Line1:
               if(LR_flag==PushRight) gyropid.P+=key_N;
               else if(LR_flag==PushLeft) gyropid.P-=key_N;
               else  MenuErrorLog("gyropid"); //Loading
               break;
           case Line2:
               if(LR_flag==PushRight) gyropid.I+=key_N;
               else if(LR_flag==PushLeft) gyropid.I-=key_N;
               else  MenuErrorLog("gyropid"); //Loading
               break;
           case Line3:
               if(LR_flag==PushRight) gyropid.D+=key_N;
               else if(LR_flag==PushLeft) gyropid.D-=key_N;
               else  MenuErrorLog("gyropid"); //Loading
               break;
           case Line4:
               page = Father;
               tft180_clear();
               break;
           default:
               select=0;//宏定义
               break;
           }
}
void Func_Image(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//

    tft180_show_string(20,10*Line1,"Image");
    tft180_show_string(20,10*Line2,"per_image");
    tft180_show_string(20,10*Line3,"gray_image");
    tft180_show_string(20,10*Line4,"Sobel");
    tft180_show_string(20,10*Maxsize,"EXIT");
   //------------FUNC-----------------------//
     do{

            my_sel=KeySelect(Maxsize);
        }
       while(my_sel<0);

       switch (select)
       {
           case Line2 :
               page = kper_image;
               per_image_flag = 1;
               break;
           case Line3 :
              page = kgray_image;
              gray_image_flag=1;
              break;
           case Line4:
              page = ksobel;
              break;
       case Line5:
           page = Father;
           tft180_clear();
           break;
       default:
           select=Line2;//宏定义
           break;
       }

}
void Func_Tof(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//

        tft180_show_string(20,10*Line1,"TOF");
        tft180_show_string(20,10*Maxsize,"EXIT");
       //------------FUNC-----------------------//
         do{
             dl1a_get_distance();
             tft180_show_int(20, 10*Line2, dl1a_distance_mm, 5);
                tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
                my_sel=KeySelect(Maxsize);
            }
           while(my_sel<0);
            tft180_show_string(00,10*TFT_MAX_LINE,"Selectend");
           switch (select)
           {
           case Line3:
               page = Father;
               tft180_clear();
               break;
           default:
               //select=0;//宏定义
               break;
           }
}
void Func_Encoder(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
    tft180_show_string(20,10*Line1,"L_Encoder:");
    tft180_show_string(20,10*Line2,"R_Encoder:");
    tft180_show_string(20,10*Maxsize,"EXIT");
   //------------FUNC-----------------------//
     do{
         EncoderGetCount(&speed_left, &speed_right);
         system_delay_ms(2);
         tft180_show_int(80,10*Line2,speed_right,4);
         tft180_show_int(80,10*Line1,speed_left,4);
         my_sel=KeySelect(Maxsize);
        }
       while(my_sel<0);
       switch (select)
       {

       case Line3:
           page = Father;
           tft180_clear();
           break;
       default:
           //select=0;//宏定义
           break;
       }
}
void Func_Motor(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
        tft180_show_string(20,10*Line1,"Motor:");
        tft180_show_int(80,10*Line1,speed_left,4);
        tft180_show_string(20,10*Line2,"Motor:");
        tft180_show_int(80,10*Line2,speed_left,4);
        tft180_show_string(20,10*Maxsize,"EXIT");
        MotorSetPWM(3000,3000);
       //------------FUNC-----------------------//
         do{
                tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
                my_sel=KeySelect(Maxsize);
            }
           while(my_sel<0);
            tft180_show_string(00,10*TFT_MAX_LINE,"Selectend");
           switch (select)
           {

           case Line3:
               page = Father;
               tft180_clear();
               MotorSetPWM(0,0);
               break;
           default:
               //select=0;//宏定义
               break;
           }
}
void Func_ADC(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//

        tft180_show_string(20,10*Line1,"ADC:");
//        tft180_show_int(80,10*Line2,adc_value[0],4);
//        tft180_show_int(80,10*Line3,adc_value[1],4);
//        tft180_show_int(80,10*Line4,adc_value[2],4);
//        tft180_show_int(80,10*Line5,adc_value[3],4);
//        tft180_show_int(80,10*Line6,adc_value[4],4);
        tft180_show_string(20,10*Maxsize,"EXIT");
       //------------FUNC-----------------------//
         do{
             ADCGetValue(adc_value);
             tft180_show_int(80,10*Line2,adc_value[0],4);
             tft180_show_int(80,10*Line3,adc_value[1],4);
             tft180_show_int(80,10*Line4,adc_value[2],4);
             tft180_show_int(80,10*Line5,adc_value[3],4);
             tft180_show_int(80,10*Line6,adc_value[4],4);
                tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
                my_sel=KeySelect(Maxsize);
            }
           while(my_sel<0);
            tft180_show_string(00,10*TFT_MAX_LINE,"Selectend");
           switch (select)
           {

           case Line7:
               page = Father;
               tft180_clear();
               break;
           default:
               //select=0;//宏定义
               break;
           }
}
void Func_MPU(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
        int16 menu_gyro = GetICM20602Gyro_Y();
        tft180_show_string(80,10*Line1,"MPU");
        tft180_show_int(20,10*Line2,menu_gyro,5);
        tft180_show_string(20,10*Maxsize,"EXIT");
       //------------FUNC-----------------------//
         do{
             int16 menu_gyro = GetICM20602Gyro_Y();
             tft180_show_int(20,10*Line2,menu_gyro,5);
                tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
                my_sel=KeySelect(Maxsize);
            }
           while(my_sel<0);
            tft180_show_string(00,10*TFT_MAX_LINE,"Selectend");
           switch (select)
           {

           case Line3:
               page = Father;
               tft180_clear();
               break;
           default:
               //select=0;//宏定义
               break;
           }
}
#define LeftCircle 1
void Func_PropLeftCircle(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
    tft180_show_string(20,10*Line1,"LeftCircle");
             tft180_show_string(20,10*Line2,"MaxSpeed:");
             tft180_show_int(90,10*Line2,process_property[LeftCircle].max_speed,4);
             tft180_show_string(20,10*Line3,"MinSpeed:");
             tft180_show_int(90,10*Line3,process_property[LeftCircle].min_speed,4);
             tft180_show_string(20,10*Line4,"integral:");
             tft180_show_int(90,10*Line4,process_property[LeftCircle].integral,4);
             tft180_show_string(20,10*Maxsize,"EXIT");
            //------------FUNC-----------------------//
          do{
                 tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
                 my_sel=KeySelect(Maxsize);
             }
            while(my_sel<0);
             tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

            switch (select)
            {
            case Line2:
                if(LR_flag==PushRight) process_property[LeftCircle].max_speed+=key_N;
                else if(LR_flag==PushLeft) process_property[LeftCircle].max_speed-=key_N;
                else  MenuErrorLog("Prop"); //Loading
                break;
            case Line3:
                if(LR_flag==PushRight) process_property[LeftCircle].min_speed+=key_N;
                else if(LR_flag==PushLeft) process_property[LeftCircle].min_speed-=key_N;
                else  MenuErrorLog("Prop"); //Loading
                break;
            case Line4:
                if(LR_flag==PushRight) process_property[LeftCircle].integral+=key_N;
                else if(LR_flag==PushLeft) process_property[LeftCircle].integral-=key_N;
                else  MenuErrorLog("Prop"); //Loading
                break;
            case Line5:
                page = Father;
                tft180_clear();
                break;
            default:
                select=1;//宏定义
                break;
            }
}
#define RightCircle 2
void Func_PropRightCircle(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
       tft180_show_string(20,10*Line1,"RightCircle");
        tft180_show_string(20,10*Line2,"MaxSpeed:");
        tft180_show_int(90,10*Line2,process_property[RightCircle].max_speed,4);
        tft180_show_string(20,10*Line3,"MinSpeed:");
        tft180_show_int(90,10*Line3,process_property[RightCircle].min_speed,4);
        tft180_show_string(20,10*Line4,"integral:");
        tft180_show_int(90,10*Line4,process_property[RightCircle].integral,4);
        tft180_show_string(20,10*Maxsize,"EXIT");
       //------------FUNC-----------------------//
     do{
            tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
            my_sel=KeySelect(Maxsize);
        }
       while(my_sel<0);
        tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

       switch (select)
       {
       case Line2:
           if(LR_flag==PushRight) process_property[RightCircle].max_speed+=key_N;
           else if(LR_flag==PushLeft) process_property[RightCircle].max_speed-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line3:
           if(LR_flag==PushRight) process_property[RightCircle].min_speed+=key_N;
           else if(LR_flag==PushLeft) process_property[RightCircle].min_speed-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line4:
           if(LR_flag==PushRight) process_property[RightCircle].integral+=key_N;
           else if(LR_flag==PushLeft) process_property[RightCircle].integral-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line5:
           page = Father;
           tft180_clear();
           break;
       default:
           select=1;//宏定义
           break;
       }
}
#define Cut 4
void Func_PropCut(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
       tft180_show_string(20,10*Line1,"Cut");
        tft180_show_string(20,10*Line2,"MaxSpeed:");
        tft180_show_int(90,10*Line2,process_property[Cut].max_speed,4);
        tft180_show_string(20,10*Line3,"MinSpeed:");
        tft180_show_int(90,10*Line3,process_property[Cut].min_speed,4);
        tft180_show_string(20,10*Line4,"integral:");
        tft180_show_int(90,10*Line4,process_property[Cut].integral,4);
        tft180_show_string(20,10*Maxsize,"EXIT");
       //------------FUNC-----------------------//
     do{
            tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
            my_sel=KeySelect(Maxsize);
        }
       while(my_sel<0);
        tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

       switch (select)
       {
       case Line2:
           if(LR_flag==PushRight) process_property[Cut].max_speed+=key_N;
           else if(LR_flag==PushLeft) process_property[Cut].max_speed-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line3:
           if(LR_flag==PushRight) process_property[Cut].min_speed+=key_N;
           else if(LR_flag==PushLeft) process_property[Cut].min_speed-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line4:
           if(LR_flag==PushRight) process_property[Cut].integral+=key_N;
           else if(LR_flag==PushLeft) process_property[Cut].integral-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line5:
           page = Father;
           tft180_clear();
           break;
       default:
           select=1;//宏定义
           break;
       }
}
#define Slope 5
void Func_PropSlope(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
       tft180_show_string(20,10*Line1,"Slope");
        tft180_show_string(20,10*Line2,"MaxSpeed:");
        tft180_show_int(90,10*Line2,process_property[Slope].max_speed,4);
        tft180_show_string(20,10*Line3,"MinSpeed:");
        tft180_show_int(90,10*Line3,process_property[Slope].min_speed,4);
        tft180_show_string(20,10*Line4,"integral:");
        tft180_show_int(90,10*Line4,process_property[Slope].integral,4);
        tft180_show_string(20,10*Maxsize,"EXIT");
       //------------FUNC-----------------------//
     do{
            tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
            my_sel=KeySelect(Maxsize);
        }
       while(my_sel<0);
        tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

       switch (select)
       {
       case Line2:
           if(LR_flag==PushRight) process_property[Slope].max_speed+=key_N;
           else if(LR_flag==PushLeft) process_property[Slope].max_speed-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line3:
           if(LR_flag==PushRight) process_property[Slope].min_speed+=key_N;
           else if(LR_flag==PushLeft) process_property[Slope].min_speed-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line4:
           if(LR_flag==PushRight) process_property[Slope].integral+=key_N;
           else if(LR_flag==PushLeft) process_property[Slope].integral-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line5:
           page = Father;
           tft180_clear();
           break;
       default:
           select=1;//宏定义
           break;
       }
}
#define Barrier 6
void Func_PropBarrier(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
       tft180_show_string(20,10*Line1,"Barrier");
        tft180_show_string(20,10*Line2,"MaxSpeed:");
        tft180_show_int(90,10*Line2,process_property[Barrier].max_speed,4);
        tft180_show_string(20,10*Line3,"MinSpeed:");
        tft180_show_int(90,10*Line3,process_property[Barrier].min_speed,4);
        tft180_show_string(20,10*Line4,"integral:");
        tft180_show_int(90,10*Line4,process_property[Barrier].integral,4);
        tft180_show_string(20,10*Maxsize,"EXIT");
       //------------FUNC-----------------------//
     do{
            tft180_show_string(00,10*TFT_MAX_LINE,"Selecting"); //Loading
            my_sel=KeySelect(Maxsize);
        }
       while(my_sel<0);
        tft180_show_string(80,10*TFT_MAX_LINE,"Selectend");

       switch (select)
       {
       case Line2:
           if(LR_flag==PushRight) process_property[Barrier].max_speed+=key_N;
           else if(LR_flag==PushLeft) process_property[Barrier].max_speed-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line3:
           if(LR_flag==PushRight) process_property[Barrier].min_speed+=key_N;
           else if(LR_flag==PushLeft) process_property[Barrier].min_speed-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line4:
           if(LR_flag==PushRight) process_property[Barrier].integral+=key_N;
           else if(LR_flag==PushLeft) process_property[Barrier].integral-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line5:
           page = Father;
           tft180_clear();
           break;
       default:
           select=1;//宏定义
           break;
       }
}
void Func_per_image(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
            tft180_show_string(50,10*Line1,"per_image");
            tft180_show_string(20,10*Line2,"per_edgeline:");
            tft180_show_string(20,10*Line3,"c_line");
            tft180_show_string(20,10*Maxsize,"EXIT");
           //------------FUNC-----------------------//
             do{
                    my_sel=KeySelect(Maxsize);
                }
               while(my_sel<0);
               switch (select)
               {
               case Line2:
                   per_edgeline_flag=1;
                   break;
               case Line3:
                   c_line_flag=1;
                   break;
               case Line4:
                   page = Father;
                   tft180_clear();
                   break;
               case KEY_EXIT:
                   page = HOME;
                   tft180_clear();
                   break;
               default:
                   select=2;//宏定义
                   break;
               }
}
void Func_gray_image(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
            tft180_show_string(50,10*Line1,"gray_image");
            tft180_show_string(20,10*Line2,"edgeline");
            tft180_show_string(20,10*Line3,"per_edgeline");
            tft180_show_string(20,10*Line4,"c_line");
            tft180_show_string(20,10*Maxsize,"EXIT");
           //------------FUNC-----------------------//
             do{
                    my_sel=KeySelect(Maxsize);
                }
               while(my_sel<0);
               switch (select)
               {
               case Line2:
                   per_edgeline_flag=1;
                   break;
               case Line3:
                   edgeline_flag=1;
                   break;
               case Line4:
                   c_line_flag=1;
                   break;
               case Line5:
                   page = Father;
                   tft180_clear();
                   break;
               case KEY_EXIT:
                   page = HOME;
                   tft180_clear();
                   break;
               default:
                   select=2;//宏定义
                   break;
               }
}
void Func_sobel(Menu Father,uint8 Maxsize){

    while(KeySelect(Maxsize)<0){
        sobel(mt9v03x_image,binary_image);
        tft180_show_binary_image(0, 0,binary_image[0],MT9V03X_W, MT9V03X_H, 160, 120);
    }
    switch (select){
        case KEY_EXIT:
            page = kImage ;
            break;
        default :
            break;
    }
}

void Func_EXP_TIME(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
                tft180_show_string(20,10*Line1,"EXP_TIME=");
                tft180_show_int(80,10*Line1,Change_EXP_TIME_DEF,3);

                tft180_show_string(20,10*Maxsize,"EXIT");
               //------------FUNC-----------------------//
    while(KeySelect(Maxsize)<0){

          }
      switch (select){
          case Line1:
              if(LR_flag==PushRight) Change_EXP_TIME_DEF+=key_N;
              else if(LR_flag==PushLeft) Change_EXP_TIME_DEF-=key_N;
              break;
          case Line2:
              page = Parameter ;
              mt9v03x_set_exposure_time(Change_EXP_TIME_DEF);
             // mt9v03x_init();//初始化摄像头
              break;
          case KEY_EXIT:
              page = HOME ;
              mt9v03x_set_exposure_time(Change_EXP_TIME_DEF);
//              mt9v03x_init();//初始化摄像头
              break;
          default :
              break;
      }
}
bool Outgarage_dir=0;//0为左 1为右

void Func_Outgarage(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
                   tft180_show_string(30,10*Line1,"Left or Right");
                   switch (Outgarage_dir)
                   {
                       case 0: tft180_show_string(20,10*Line2,"Left"); break;
                       case 1: tft180_show_string(20,10*Line2,"Right"); break;
                       break;
                   }
                   tft180_show_string(20,10*Maxsize,"EXIT");
                  //------------FUNC-----------------------//
       while(KeySelect(Maxsize)<0){

             }
         switch (select){
             case Line2:
                 Outgarage_dir=!Outgarage_dir;
                 break;
             case Line3:
                 page = Father;
                 break;

             default :
                 break;
         }
}
/***********************************************
* @brief : 按键控制状态机
* @param : void
* @return: void
* @date  : 2023.7.7
* @author: L
************************************************/
void KeyStateMachine(void)
{
    while(departure){
        tft180_clear();
        switch(page){
            //-------------一级菜单-----------------//
            case HOME:
                Func_Home(HOME,3);
                break;
            //-------------二级菜单-----------------//
            case State_machine:
                Func_State_machine(HOME,11);
                break;
            case Parameter:
                Func_Parameter(HOME,5);
                break;
            case Checkout:
                Func_Checkout(HOME,6);
                break;
            //----------------三级菜单---------------//
            case kState_speed:
                Func_State_speed(State_machine,4);
                break;
            case kPid:
                Func_Pid(Parameter,5);
                break;
            case kBasetrack:
                Func_Basetrack(Parameter,2);
                break;
            case kEXP_TIME:
                Func_EXP_TIME(Parameter,1);
                break;
            case kprocess_speed:
                Func_process_speed(Parameter, 9);
                break;
            case kOutgarage:
                Func_Outgarage(Parameter, 2);
                break;
            case kImage:
                Func_Image(Checkout, 4);
                break;
            case kTof:
                Func_Tof(Checkout, 2);
                break;
            case kEncoder:
                Func_Encoder(Checkout, 2);
                break;
            case kMotor:
                Func_Motor(Checkout, 2);
                break;
            case kADC:
                Func_ADC(Checkout, 6);
                break;
            case kMPU:
                Func_MPU(Checkout, 2);
                break;
            /*四级菜单*/
            case kImagepid:
                Func_Imagepid(kPid, 3);
                break;
            case kAdcpid:
                Func_ADCpid(kPid, 3);
                break;
            case kLeftpid:
                Func_Leftpid(kPid,3);
                break;
            case kRightpid:
                Func_Rightpid(kPid, 3);
                break;
            case kGyropid:
                Func_Gyropid(kPid, 3);
                break;
            case kper_image:
                Func_per_image(kImage,3);
                break;
            case kgray_image:
                Func_gray_image(kImage,4);
                break;
            case ksobel:
                Func_sobel(kImage,4);
                break;
            default:break;
        }
    }
    system_delay_ms(1000);//延时一秒方便发车人离开
    encoder_clear_count(ENCODER_LEFT);                                      //清空左边编码器计数
    encoder_clear_count(ENCODER_RIGHT);                                     //清空右边编码器计数
    pit_enable(CCU60_CH0);//速度环
    pit_enable(CCU60_CH1);//方向环
}


/***********************************************
* @brief : 按下按键唤醒屏幕查看跑车时间，为了应对赛场上调试不给用计时器
* @param : void
* @return: void
* @date  : 2023.7.5
* @author: 刘骏帆
************************************************/
void WakeUpScreen(void)
{
    float time=0;
    while(1)
    {
        pit_disable(CCU60_CH0);//关闭电机中断
        pit_disable(CCU60_CH1);
        if(KeyGet()==KEY_ENTER)
        {
            tft180_init();
            tft180_clear();
            time=(float)(elapsed_time*2)/1000;//换算成s
            tft180_show_float(0, 0,time,3,2);
            tft180_show_char(30, 0, 's');
        }
    }
}

#endif
