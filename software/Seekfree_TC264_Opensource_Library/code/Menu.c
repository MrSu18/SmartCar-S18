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
#include "ImageBasic.h"

#define TFT_MAX_LINE 11 //可显示12行 从0开始数起
#define Unknown_sta -5 //未知状态标志位，可用于检错
#define Unselected -1 //未确定标志
#define PushLeft 2 //推左 仅用于判断，可改值
#define PushRight 3 //推右 仅用于判断，可改值
#define KEY_EXIT 13 //按下keytop select返回值

uint8 per_image_flag=0,gray_image_flag=0;
uint8 edgeline_flag=0,c_line_flag=0,per_edgeline_flag=0;
uint8 Return_flag=0; //菜单返回标志  待修正
uint8 LR_flag;//左右摇杆标志
uint8 No_States;//状态机状态计数器
uint8 States_amount;//状态机总数
int16 Change_EXP_TIME_DEF=MT9V03X_EXP_TIME_DEF;  //要想曝光生效，必须要改摄像头配置文件里的参数
bool Outgarage_dir=0;//出库方向 0为左 1为右
int page; //页数--即状态机状态
short select;//实时获取行数
short my_sel;//保存上一个select
bool Departure_flag;//退出按键状态机标志位_0即退出
int key_N=1;//按键步长


typedef enum Menu {//注册菜单
    //一级菜单
    HOME,
    //二级菜单
    State_machine,
    Parameter,
    Checkout,
    Flash,
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

//    /*kState_speed*/----测试无误后，可删除
//    kLeftCircle,
//    kRightCircle,
//    kCut,
//    kSlope,
//    kBarrier,
//    /*kState_speed*/

    /*kImage*/
    kper_image,
    kgray_image,
    ksobel,
    kadaptiveThreshold,
    /*kImage*/

}Menu;

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
    page = HOME;//首页为HOME
    Departure_flag=1;//进入按键状态机模式
    key_N=1;//步长为1
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
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
            case KEY_LOW:
                if(page == State_machine){//状态机选择特殊处理
                    if(No_States>0)
                        No_States --;
                    break;
                }
                //普遍处理
                key_N = (key_N*10)%1000;
                if(key_N==0) key_N =1;
                break;//很蠢的循环，可改进
            default :       return Unknown_sta;   break;//未知状态

        }
        return Unselected;
}
/***********************************************
* @brief : Menu报错-用于调试
* @param : Page 输入报错字符串
* @return: viod
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void MenuErrorLog(const char Page[]){
    tft180_show_string(30,10*TFT_MAX_LINE,Page);
    tft180_show_string(80,10*TFT_MAX_LINE,"Error");
}
/***********************************************
* @brief : 主菜单函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_Home(Menu Father,uint8 Maxsize){
    //  for(int i = 0;i < Maxsize;i++)
    // tft180_show_string(0, i*10, home[i]);//这里可以集成为一个显示内容的字符数组，但节约内存暂时不考虑
    //-----------Display-------------------//
     tft180_show_string(20,10*Line1,"State_machine");
     tft180_show_string(20,10*Line2,"Parameter");
     tft180_show_string(20,10*Line3,"Checkout");
     tft180_show_string(20,10*Line4,"Flash");
     tft180_show_string(20,10*Maxsize,"EXIT");
    //------------FUNC-----------------------//
     do{
         my_sel=KeySelect(Maxsize);
     }while(my_sel<0);
    switch (select)
    {
    case Line1:
        page = State_machine;
        LR_flag = PushRight;//默认展开
        break;
    case Line2:
        page = Parameter;
        break;
     case Line3:
        page = Checkout;
        break;
     case Line4:
         page = Flash;
         break;
    case Line5:
        page = Father;
        Departure_flag=0;
        break;
    default:
        select=0;//宏定义
        tft180_clear();
        break;
    }
    //-------EXIT-----------//
}
/***********************************************
* @brief : 状态机函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
     tft180_show_int(110,10*Line1,No_States,2); //显示第几个状态
     //------------------EXIT----------------//
     //tft180_show_string(20,10*Maxsize,"Exit");
     //-------------------FUNC----------------//

    while(KeySelect(Maxsize)<0){
        tft180_set_color(RGB565_BLACK,RGB565_WHITE);
        if(process_status[No_States]<10)//0-9的状态
            tft180_show_int(110+10*(No_States%3),10*(Line3+No_States/3) ,process_status[No_States],1);
        else
            tft180_show_char(110+10*(No_States%3),10*(Line3+No_States/3),process_status[No_States]);
        tft180_set_color(RGB565_RED,RGB565_WHITE);
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
            States_amount=No_States;
            No_States=0;
            LR_flag =0;
            tft180_clear();
            break;
        default:
            MenuErrorLog("state_machine");
            break;
    }
}
/***********************************************
* @brief : 选择调整参数种类函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
    }while(my_sel<0);
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
/***********************************************
* @brief : 调试选择函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
    }while(my_sel<0);
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
/***********************************************
* @brief : Pid调节选择函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
             my_sel=KeySelect(Maxsize);
         }while(my_sel<0);
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
/***********************************************
* @brief : 调节单个状态的函数 //如果是从Proc 跳转来的 记得调整proc处的内部调用
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
        if(process_speed[No_States-1]==0) process_speed[No_States-1]=60;
        tft180_show_string(20,10*Line5,"proc_speed:");
        tft180_show_int(90,10*Line5,process_speed[No_States-1],4);
        tft180_show_string(20,10*Maxsize,"EXIT");
       //------------FUNC-----------------------//
     do{
         tft180_show_int(100,10*TFT_MAX_LINE,key_N,3);
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
           if(LR_flag==PushRight) process_speed[No_States-1]+=key_N;
           else if(LR_flag==PushLeft) process_speed[No_States-1]-=key_N;
           else  MenuErrorLog("Prop"); //Loading
           break;
       case Line6:
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
/***********************************************
* @brief : 选择调节第几个状态机函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
              Func_State_speed(kprocess_speed,5);//临时跳转
              }while(Return_flag !=1);
              No_States=CacheOfNo_States;
              break;
          default:
              //select=Line2;//宏定义
              break;
          }
}
/***********************************************
* @brief : 预瞄点调节函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_Basetrack(Menu Father,uint8 Maxsize){
        //-----------Display-------------------//
         tft180_show_string(50,10*Line1,"Basetrack");
         tft180_show_string(20,10*Line2,"aim_distance:");
         tft180_show_float(110,10*Line2,aim_distance,2,3);
         tft180_show_string(20,10*Maxsize,"EXIT");
        //------------FUNC-----------------------//
      do{
          tft180_show_int(100,10*TFT_MAX_LINE,key_N*0.001,3);
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
/***********************************************
* @brief : ImgPID调节函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
       tft180_show_int(100,10*TFT_MAX_LINE,key_N,3);
          my_sel=KeySelect(Maxsize);
      }while(my_sel<0);
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
/***********************************************
* @brief : ADCPID调节函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
          tft180_show_int(100,10*TFT_MAX_LINE,key_N,3);
             my_sel=KeySelect(Maxsize);
         }
        while(my_sel<0);
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
/***********************************************
* @brief : LeftPID调节函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_Leftpid(Menu Father,uint8 Maxsize){
            //-----------Display-------------------//
            tft180_show_string(20,10*Line1,"PIDLeft.P:");
            tft180_show_float(80,10*Line1,speedpid_left.P,4,2);
            tft180_show_string(20,10*Line2,"PIDLeft.I:");
            tft180_show_float(80,10*Line2,speedpid_left.I,4,2);
            tft180_show_string(20,10*Line3,"PIDLeft.D:");
            tft180_show_float(80,10*Line3,speedpid_left.D,4,2);
            tft180_show_int(100,10*TFT_MAX_LINE,key_N,3);
            tft180_show_string(20,10*Maxsize,"EXIT");
            //------------FUNC-----------------------//
         do{
             tft180_show_int(100,10*TFT_MAX_LINE,key_N,3);
                my_sel=KeySelect(Maxsize);
            }while(my_sel<0);
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
/***********************************************
* @brief : RightPID调节函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_Rightpid(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
            tft180_show_string(20,10*Line1,"PIDRIGHT.P:");
            tft180_show_float(80,10*Line1,speedpid_right.P,4,2);
            tft180_show_string(20,10*Line2,"PIDRIGHT.I:");
            tft180_show_float(80,10*Line2,speedpid_right.I,4,2);
            tft180_show_string(20,10*Line3,"PIDRIGHT.D:");
            tft180_show_float(80,10*Line3,speedpid_right.D,4,2);
            tft180_show_int(100,10*TFT_MAX_LINE,key_N,3);
            tft180_show_string(20,10*Maxsize,"EXIT");
           //------------FUNC-----------------------//
         do{
             tft180_show_int(100,10*TFT_MAX_LINE,key_N,3);

             my_sel=KeySelect(Maxsize);
            }while(my_sel<0);
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
/***********************************************
* @brief : GyroPID调节函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
             tft180_show_int(100,10*TFT_MAX_LINE,key_N,3);
             my_sel=KeySelect(Maxsize);
            }
           while(my_sel<0);
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
/***********************************************
* @brief : 图像查看选择函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_Image(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
    tft180_show_string(20,10*Line1,"Image");
    tft180_show_string(20,10*Line2,"per_image");
    tft180_show_string(20,10*Line3,"gray_image");
    tft180_show_string(20,10*Line4,"Sobel");
    tft180_show_string(20,10*Line5,"AdaptiveThreshold");
    tft180_show_string(20,10*Maxsize,"EXIT");
   //------------FUNC-----------------------//
     do{

            my_sel=KeySelect(Maxsize);
        }while(my_sel<0);
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
             page = kadaptiveThreshold;
             break;
           case Line6:
               page = Father;
               tft180_clear();
               break;
           default:
               select=Line2;//宏定义
               break;
       }
}
/***********************************************
* @brief : TOF调试函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
/***********************************************
* @brief : 编码器调试函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_Encoder(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
    tft180_show_string(20,10*Line1,"L_Encoder:");
    tft180_show_string(20,10*Line2,"R_Encoder:");
    tft180_show_string(20,10*Maxsize,"EXIT");
   //------------FUNC-----------------------//
    select=Line3;//因为没有其他功能，所以默认退出
     do{
         EncoderGetCount(&speed_left, &speed_right);
         system_delay_ms(2);
         tft180_show_int(80,10*Line2,speed_right,4);
         tft180_show_int(80,10*Line1,speed_left,4);
         my_sel=KeySelect(Maxsize);
        }while(my_sel<0);
       switch (select)
       {
            case Line3:
                page = Father;
                tft180_clear();
                break;
            default:
            break;
       }
}
/***********************************************
* @brief : 电机调试函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_Motor(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
    tft180_show_string(20,10*Line1,"Motor:");
    tft180_show_int(80,10*Line1,speed_left,4);
    tft180_show_string(20,10*Line2,"Motor:");
    tft180_show_int(80,10*Line2,speed_left,4);
    tft180_show_string(20,10*Maxsize,"EXIT");
    MotorSetPWM(2000,-2000);
   //------------FUNC-----------------------//
     do{
            my_sel=KeySelect(Maxsize);
        }while(my_sel<0);
       switch (select)
       {
           case Line3:
               page = Father;
               tft180_clear();
               MotorSetPWM(0,0);
               break;
           default:
               break;
       }
}
/***********************************************
* @brief : ADC调试函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_ADC(Menu Father,uint8 Maxsize){
        //-----------Display-------------------//
        tft180_show_string(20,10*Line1,"ADC:");
        tft180_show_string(20,10*Maxsize,"EXIT");
       //------------FUNC-----------------------//
        select=Line7;
         do{
             ADCGetValue(adc_value);
             tft180_show_int(80,10*Line2,adc_value[0],4);
             tft180_show_int(80,10*Line3,adc_value[1],4);
             tft180_show_int(80,10*Line4,adc_value[2],4);
             tft180_show_int(80,10*Line5,adc_value[3],4);
             tft180_show_int(80,10*Line6,adc_value[4],4);
                my_sel=KeySelect(Maxsize);
            }while(my_sel<0);
           switch (select)
           {
           case Line7:
               page = Father;
               tft180_clear();
               break;
           default:
               break;
           }
}
/***********************************************
* @brief : MPU调试函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
            my_sel=KeySelect(Maxsize);
        }while(my_sel<0);
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
/***********************************************
* @brief : 逆透视图像查看选择函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_per_image(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
    tft180_show_string(50,10*Line1,"per_image");
    tft180_show_string(20,10*Line2,"per_edgeline:");
    tft180_show_string(20,10*Line3,"c_line");
    tft180_show_string(20,10*Maxsize,"EXIT");
   //------------FUNC-----------------------//
     do{
            my_sel=KeySelect(Maxsize);
        }while(my_sel<0);
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
/***********************************************
* @brief : 灰度图像查看选择函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_gray_image(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
    tft180_show_string(50,10*Line1,"gray_image");
    tft180_show_string(20,10*Line2,"per_edgeline");
    tft180_show_string(20,10*Line3,"edgeline");
    tft180_show_string(20,10*Line4,"c_line");
    tft180_show_string(20,10*Maxsize,"EXIT");
   //------------FUNC-----------------------//
     do{
            my_sel=KeySelect(Maxsize);
        }while(my_sel<0);
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
/***********************************************
* @brief : Sobel查看函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_sobel(Menu Father,uint8 Maxsize){
    while(KeySelect(Maxsize)<0){
        select=Line3;
        if(mt9v03x_finish_flag){
            sobel(mt9v03x_image,binary_image);
            tft180_show_binary_image(0, 0,binary_image[0],MT9V03X_W, MT9V03X_H, 160, 120);
            mt9v03x_finish_flag=0;
        }
        tft180_set_color(RGB565_WHITE,RGB565_BLACK);
        tft180_show_uint(120, 10*Line3,otsu_thr,3);
        tft180_set_color(RGB565_RED,RGB565_WHITE);
    }
    switch (select){
        case Line3 :
            if(LR_flag==PushRight) otsu_thr+=key_N;
            else if(LR_flag==PushLeft) otsu_thr-=key_N;
            else  MenuErrorLog("Sobel"); //Loading
            break;
        case KEY_EXIT:
            page = kImage ;
            break;
        default :
            break;
    }
}
/***********************************************
* @brief : 曝光调整函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_EXP_TIME(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
                tft180_show_string(20,10*Line1,"EXP_TIME=");
                tft180_show_int(80,10*Line1,Change_EXP_TIME_DEF,3);
                tft180_show_string(20,10*Maxsize,"EXIT");
               //------------FUNC-----------------------//
    while(KeySelect(Maxsize)<0){
        tft180_show_int(100,10*TFT_MAX_LINE,key_N,3);

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
/***********************************************
* @brief : 出库方向函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
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
* @brief : Flash函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
bool Read_Flag = 0;
void Func_Flash(Menu Father,uint8 Maxsize){
    //-----------Display-------------------//
      tft180_show_string(20,10*Line1,"Write_Flash");
      tft180_show_string(20,10*Line2,"Read_Flash_Nexttime");
      tft180_show_string(20,10*Line3,"Don't,Read_Nexttime");
      tft180_show_string(20,10*Line4,"Clear Flash");
      tft180_show_string(20,10*Maxsize,"EXIT");
     //------------FUNC-----------------------//
      while(KeySelect(Maxsize)<0){

            }
        switch (select){
            case Line1:
                WriteToFlash(0, 0);
                break;
            case Line2:
                WriteToFlash(1, 1);
                break;
            case Line3:
                WriteToFlash(1, 0);
                break;
            case Line4:
                flash_erase_page(0, 0);
                flash_erase_page(0, 1);
                break;
            case Line5:
                page = Father;
                break;
            default :
                select = Line1;
                break;
        }
}

/***********************************************
* @brief : 自适应图像函数
* @param : Father 父级  Maxsize 最大行数(不包括EXIT，会自动加进去)
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void Func_adaptiveThreshold(Menu Father,uint8 Maxsize){
    while(KeySelect(Maxsize)<0){
        select=Line3;
        if(mt9v03x_finish_flag){

            myadaptiveThreshold(mt9v03x_image, binary_image, MT9V03X_W, MT9V03X_H,GRAY_BLOCK, clip_value);
            tft180_show_binary_image(0, 0,binary_image[0],MT9V03X_W, MT9V03X_H, 160, 120);
            mt9v03x_finish_flag=0;
        }
        tft180_set_color(RGB565_WHITE,RGB565_BLACK);
        tft180_show_uint(120, 10*Line3,clip_value,3);
        tft180_set_color(RGB565_RED,RGB565_WHITE);
    }
    switch (select){
        case Line3 :
            if(LR_flag==PushRight) clip_value+=key_N;
            else if(LR_flag==PushLeft) clip_value-=key_N;
            else  MenuErrorLog("adaptimg"); //Loading
            break;
        case KEY_EXIT:
            page = kImage ;
            break;
        default :
            break;
    }
}

/***********************************************
* @brief : 按键控制状态机
* @param : void
* @return: void
* @date  : 2023.7.12
* @author: 黄诚钜
************************************************/
void KeyStateMachine(void)
{
    while(Departure_flag){
        tft180_clear();
        switch(page){
            //-------------一级菜单-----------------//
            case HOME:
                Func_Home(HOME,4);
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
            case Flash:
                Func_Flash(HOME,4);
                break;
            //----------------三级菜单---------------//
            case kState_speed:
                Func_State_speed(State_machine,5);
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
                Func_Image(Checkout, 5);
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
                Func_sobel(kImage,TFT_MAX_LINE);
                break;
            case kadaptiveThreshold:
                Func_adaptiveThreshold(kImage,TFT_MAX_LINE);
                break;
            default:
                break;
        }
    }
    //Departure_退出后
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

/***********************************************
* @brief : 将需要修改的变量写入Flash
* @param : page:选择写入哪一页
*          read_flag:1表示会读取0页的内容，0表示使用原程序的参数(只有page为1时该项才有效)
* @return: void
* @date  : 2023.7.12
* @author: L
************************************************/
void WriteToFlash(uint32 page,uint32 read_flag)
{
    switch(page)
    {
        case 0:
            if(flash_check(0, 2)==1)//如果Flash的0页有值，擦除0页中的值
                flash_erase_page(0, 2);
            flash_buffer_clear();//清空缓冲区

            int index = 0;//缓冲区数组索引
            //将需要修改的变量写到缓冲区
            for(;index<PROCESS_LENGTH;index++)//状态机和状态机速度
            {
                flash_union_buffer[index+PROCESS_LENGTH].uint8_type=process_status[index];//状态机状态
                flash_union_buffer[index].uint16_type=process_speed[index];//状态机识别到元素前的速度
                flash_union_buffer[index+2*PROCESS_LENGTH].uint8_type=process_property[index].min_speed;//元素中的最低速度
                flash_union_buffer[index+3*PROCESS_LENGTH].uint8_type=process_property[index].max_speed;//元素中的最高速度
                flash_union_buffer[index+4*PROCESS_LENGTH].uint8_type=process_property[index].speed_detaction_flag;//每个元素是否开启速度决策
                flash_union_buffer[index+5*PROCESS_LENGTH].uint8_type=process_property[index].integral;//编码器或陀螺仪积分多少
            }
            index+=5*PROCESS_LENGTH+1;
            flash_union_buffer[index++].float_type=turnpid_image.P;//图像方向环P
            flash_union_buffer[index++].float_type=turnpid_image.I;//图像方向环I
            flash_union_buffer[index++].float_type=turnpid_image.D;//图像方向环D
            flash_union_buffer[index++].float_type=turnpid_adc.P;//电磁方向环P
            flash_union_buffer[index++].float_type=turnpid_adc.I;//电磁方向环I
            flash_union_buffer[index++].float_type=turnpid_adc.D;//电磁方向环D
            flash_union_buffer[index++].float_type=speedpid_left.P;//左轮速度环P
            flash_union_buffer[index++].float_type=speedpid_left.I;//左轮速度环I
            flash_union_buffer[index++].float_type=speedpid_left.D;//左轮速度环D
            flash_union_buffer[index++].float_type=speedpid_right.P;//右轮速度环P
            flash_union_buffer[index++].float_type=speedpid_right.I;//右轮速度环I
            flash_union_buffer[index++].float_type=speedpid_right.D;//右轮速度环D
            flash_union_buffer[index++].float_type=gyropid.P;//角速度环P
            flash_union_buffer[index++].float_type=gyropid.I;//角速度环I
            flash_union_buffer[index++].float_type=gyropid.D;//角速度环D
            flash_union_buffer[index++].float_type=aim_distance;//预瞄点
            flash_union_buffer[index++].int16_type=Change_EXP_TIME_DEF;//摄像头曝光时间
            flash_union_buffer[index++].uint8_type=Outgarage_dir;//出库方向

            flash_write_page_from_buffer(0, 2);//将缓冲区中的值写入Flash
            break;
        case 1:
            if(flash_check(0, 1)==1)//如果Flash的1页有值，擦除1页的内容
                flash_erase_page(0, 1);

            flash_write_page(0, page, &read_flag, 1);//向1页中写入一个值,用来确认是否使用0页中的参数
            break;
        default:break;
    }

}
/***********************************************
* @brief : 从Flash中读取值到对应的变量
* @param : void
* @return: void
* @date  : 2023.7.12
* @author: L
************************************************/
void ReadFromFlash(void)
{
    uint32 read_flag = 0;//存储是否读0页的参数的标志位
    flash_read_page(0, 1, &read_flag, 1);//读取1页的第一个字符
    if(flash_check(0, 2)==1 && flash_check(0, 1)==1 && read_flag==1)//如果Flash中有值，则读取，否则不读取
    {
        flash_buffer_clear();//清空缓冲区
        flash_read_page_to_buffer(0, 2);//读取Flash的0页的参数到缓冲区
        //从缓冲区获得对应变量的值
        int index = 0;//索引值
        for(;index<PROCESS_LENGTH;index++)//状态机和状态机速度
        {
//            process_status[index]=flash_union_buffer[index].uint8_type;//状态机的状态
//            process_speed[index]=flash_union_buffer[index+PROCESS_LENGTH].uint16_type;//识别到元素前的速度
            process_status[index]=flash_union_buffer[index+PROCESS_LENGTH].uint8_type;//状态机的状态
            process_speed[index]=flash_union_buffer[index].uint16_type;//识别到元素前的速度
            process_property[index].min_speed=flash_union_buffer[index+2*PROCESS_LENGTH].uint8_type;//元素中的最低速度
            process_property[index].max_speed=flash_union_buffer[index+3*PROCESS_LENGTH].uint8_type;//元素中的最高速度
            process_property[index].speed_detaction_flag=flash_union_buffer[index+4*PROCESS_LENGTH].uint8_type;//每个元素是否开启速度决策
            process_property[index].integral=flash_union_buffer[index+5*PROCESS_LENGTH].uint8_type;//编码器或陀螺仪积分多少
        }
        index+=5*PROCESS_LENGTH+1;
        turnpid_image.P=flash_union_buffer[index++].float_type;//图像方向环P
        turnpid_image.I=flash_union_buffer[index++].float_type;//图像方向环I
        turnpid_image.D=flash_union_buffer[index++].float_type;//图像方向环D
        turnpid_adc.P=flash_union_buffer[index++].float_type;//电磁方向环P
        turnpid_adc.I=flash_union_buffer[index++].float_type;//电磁方向环I
        turnpid_adc.D=flash_union_buffer[index++].float_type;//电磁方向环D
        speedpid_left.P=flash_union_buffer[index++].float_type;//左轮速度环P
        speedpid_left.I=flash_union_buffer[index++].float_type;//左轮速度环I
        speedpid_left.D=flash_union_buffer[index++].float_type;//左轮速度环D
        speedpid_right.P=flash_union_buffer[index++].float_type;//右轮速度环P
        speedpid_right.I=flash_union_buffer[index++].float_type;//右轮速度环I
        speedpid_right.D=flash_union_buffer[index++].float_type;//右轮速度环D
        gyropid.P=flash_union_buffer[index++].float_type;//角速度环P
        gyropid.I=flash_union_buffer[index++].float_type;//角速度环I
        gyropid.D=flash_union_buffer[index++].float_type;//角速度环D
        aim_distance=flash_union_buffer[index++].float_type;//预瞄点
        Change_EXP_TIME_DEF=flash_union_buffer[index++].int16_type;//摄像头曝光时间
        Outgarage_dir=flash_union_buffer[index++].uint8_type;//出库方向

        flash_buffer_clear();//清空缓冲区
    }
}
#endif
