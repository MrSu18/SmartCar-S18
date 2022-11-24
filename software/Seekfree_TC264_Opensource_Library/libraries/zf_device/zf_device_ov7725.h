/*********************************************************************************************************************
* TC264 Opensourec Library ����TC264 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC264 ��Դ���һ����
*
* TC264 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          zf_device_ov7725
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.8.0
* ����ƽ̨          TC264D
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* ���߶��壺
*                  ------------------------------------
*                  ģ��ܽ�             ��Ƭ���ܽ�
*                  TXD/SCL            �鿴 zf_device_ov7725.h �� OV7725_COF_UART_TX �� OV7725_COF_IIC_SCL �궨��
*                  RXD/SDA            �鿴 zf_device_ov7725.h �� OV7725_COF_UART_RX �� OV7725_COF_IIC_SDA �궨��
*                  PCLK               �鿴 zf_device_ov7725.h �� OV7725_PCLK_PIN �궨��
*                  VSY                �鿴 zf_device_ov7725.h �� OV7725_VSYNC_PIN �궨��
*                  D0-D7              �鿴 zf_device_ov7725.h �� OV7725_DATA_PIN �궨�� �Ӹö��忪ʼ�������˸�����
*                  VCC                3.3V��Դ
*                  GND                ��Դ��
*                  ������������
*                  ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_ov7725_h_
#define _zf_device_ov7725_h_

#include "zf_device_type.h"

//=================================================OV7725 ��������====================================================
#define OV7725_COF_UART         (UART_1)                                        // С������ô���
#define OV7725_COF_BAUR         (9600)                                          // С������ô��ڲ�����
#define OV7725_COF_UART_TX      (UART1_RX_P02_3)                                // С��� UART-TX ���� Ҫ���ڵ�Ƭ�� RX ��
#define OV7725_COF_UART_RX      (UART1_TX_P02_2)                                // С��� UART-RX ���� Ҫ���ڵ�Ƭ�� TX ��

#define OV7725_COF_IIC_DELAY    (640 )                                          // С��� IIC ��ʱ
#define OV7725_COF_IIC_SCL      (P02_3)                                         // С��� IIC-SCL ����
#define OV7725_COF_IIC_SDA      (P02_2)                                         // С��� IIC-SDA ����

#define OV7725_DMA_CH           (IfxDma_ChannelId_5)                            // ERU����DMAͨ����ֹ�����޸�

#define OV7725_PCLK_PIN         (ERU_CH2_REQ14_P02_1)                           // GPIO����TIM���Ž�ֹ�����޸�

#define OV7725_VSYNC_PIN        (ERU_CH3_REQ6_P02_0 )                           // ���ж�����
#define OV7725_VSYNC_PORT_PIN   (P02_0 )                                        // ���ж�����

#define OV7725_DATA_PIN         (P00_0 )                                        // �������� ������ ֻ���� GPIOx0 ���� GPIOx8 ��ʼ �����˸��������� F0-F7
#define OV7725_DATA_ADD         get_port_in_addr(OV7725_DATA_PIN)

#define OV7725_INIT_TIMEOUT     (0x0080)                                        // Ĭ�ϵ�����ͷ��ʼ����ʱʱ�� ����Ϊ��λ
//=================================================OV7725 ��������====================================================

//=================================================OV7725 ��������====================================================
#define OV7725_W                (160)                                           // ͼ���� 80/160/240/320
#define OV7725_H                (120)                                           // ͼ��߶� 60/120/180/240
#define OV7725_IMAGE_SIZE       (OV7725_W * OV7725_H / 8)                       // ����ͼ���С OV7725_IMAGE_SIZE ���ܳ��� 65535

#define OV7725_CONTRAST_DEF     (0x30)                                          // ��ֵ���� ����ͷ��ֵ����ֵ ����͹�С����ֵ�ᱻ����ͷ�Զ�����
#define OV7725_FPS_DEF          (50 )                                           // ֡������ ��� 150 ֡ ����С�ֱ��ʲ��ܴﵽ���֡��
//=================================================OV7725 ��������====================================================

//==========================================IIC �汾С���Ĵ��� �����ֲ������û�����============================================
#define OV7725_ID               (0x21)                                          // ����ͷID��
#define OV7725_GAIN             (0x00)                                          // ����Ϊ����ͷ�Ĵ���
#define OV7725_BLUE             (0x01)
#define OV7725_RED              (0x02)
#define OV7725_GREEN            (0x03)
#define OV7725_BAVG             (0x05)
#define OV7725_GAVG             (0x06)
#define OV7725_RAVG             (0x07)
#define OV7725_AECH             (0x08)
#define OV7725_COM2             (0x09)
#define OV7725_PID              (0x0A)
#define OV7725_VER              (0x0B)
#define OV7725_COM3             (0x0C)
#define OV7725_COM4             (0x0D)
#define OV7725_COM5             (0x0E)
#define OV7725_COM6             (0x0F)
#define OV7725_AEC              (0x10)
#define OV7725_CLKRC            (0x11)
#define OV7725_COM7             (0x12)
#define OV7725_COM8             (0x13)
#define OV7725_COM9             (0x14)
#define OV7725_COM10            (0x15)
#define OV7725_REG16            (0x16)
#define OV7725_HSTART           (0x17)
#define OV7725_HSIZE            (0x18)
#define OV7725_VSTRT            (0x19)
#define OV7725_VSIZE            (0x1A)
#define OV7725_PSHFT            (0x1B)
#define OV7725_MIDH             (0x1C)
#define OV7725_MIDL             (0x1D)
#define OV7725_LAEC             (0x1F)
#define OV7725_COM11            (0x20)
#define OV7725_BDBase           (0x22)
#define OV7725_BDMStep          (0x23)
#define OV7725_AEW              (0x24)
#define OV7725_AEB              (0x25)
#define OV7725_VPT              (0x26)
#define OV7725_REG28            (0x28)
#define OV7725_HOutSize         (0x29)
#define OV7725_EXHCH            (0x2A)
#define OV7725_EXHCL            (0x2B)
#define OV7725_VOutSize         (0x2C)
#define OV7725_ADVFL            (0x2D)
#define OV7725_ADVFH            (0x2E)
#define OV7725_YAVE             (0x2F)
#define OV7725_LumHTh           (0x30)
#define OV7725_LumLTh           (0x31)
#define OV7725_HREF             (0x32)
#define OV7725_DM_LNL           (0x33)
#define OV7725_DM_LNH           (0x34)
#define OV7725_ADoff_B          (0x35)
#define OV7725_ADoff_R          (0x36)
#define OV7725_ADoff_Gb         (0x37)
#define OV7725_ADoff_Gr         (0x38)
#define OV7725_Off_B            (0x39)
#define OV7725_Off_R            (0x3A)
#define OV7725_Off_Gb           (0x3B)
#define OV7725_Off_Gr           (0x3C)
#define OV7725_COM12            (0x3D)
#define OV7725_COM13            (0x3E)
#define OV7725_COM14            (0x3F)
#define OV7725_COM16            (0x41)
#define OV7725_TGT_B            (0x42)
#define OV7725_TGT_R            (0x43)
#define OV7725_TGT_Gb           (0x44)
#define OV7725_TGT_Gr           (0x45)
#define OV7725_LC_CTR           (0x46)
#define OV7725_LC_XC            (0x47)
#define OV7725_LC_YC            (0x48)
#define OV7725_LC_COEF          (0x49)
#define OV7725_LC_RADI          (0x4A)
#define OV7725_LC_COEFB         (0x4B)
#define OV7725_LC_COEFR         (0x4C)
#define OV7725_FixGain          (0x4D)
#define OV7725_AREF1            (0x4F)
#define OV7725_AREF6            (0x54)
#define OV7725_UFix             (0x60)
#define OV7725_VFix             (0x61)
#define OV7725_AWBb_blk         (0x62)
#define OV7725_AWB_Ctrl0        (0x63)
#define OV7725_DSP_Ctrl1        (0x64)
#define OV7725_DSP_Ctrl2        (0x65)
#define OV7725_DSP_Ctrl3        (0x66)
#define OV7725_DSP_Ctrl4        (0x67)
#define OV7725_AWB_bias         (0x68)
#define OV7725_AWBCtrl1         (0x69)
#define OV7725_AWBCtrl2         (0x6A)
#define OV7725_AWBCtrl3         (0x6B)
#define OV7725_AWBCtrl4         (0x6C)
#define OV7725_AWBCtrl5         (0x6D)
#define OV7725_AWBCtrl6         (0x6E)
#define OV7725_AWBCtrl7         (0x6F)
#define OV7725_AWBCtrl8         (0x70)
#define OV7725_AWBCtrl9         (0x71)
#define OV7725_AWBCtrl10        (0x72)
#define OV7725_AWBCtrl11        (0x73)
#define OV7725_AWBCtrl12        (0x74)
#define OV7725_AWBCtrl13        (0x75)
#define OV7725_AWBCtrl14        (0x76)
#define OV7725_AWBCtrl15        (0x77)
#define OV7725_AWBCtrl16        (0x78)
#define OV7725_AWBCtrl17        (0x79)
#define OV7725_AWBCtrl18        (0x7A)
#define OV7725_AWBCtrl19        (0x7B)
#define OV7725_AWBCtrl20        (0x7C)
#define OV7725_AWBCtrl21        (0x7D)
#define OV7725_GAM1             (0x7E)
#define OV7725_GAM2             (0x7F)
#define OV7725_GAM3             (0x80)
#define OV7725_GAM4             (0x81)
#define OV7725_GAM5             (0x82)
#define OV7725_GAM6             (0x83)
#define OV7725_GAM7             (0x84)
#define OV7725_GAM8             (0x85)
#define OV7725_GAM9             (0x86)
#define OV7725_GAM10            (0x87)
#define OV7725_GAM11            (0x88)
#define OV7725_GAM12            (0x89)
#define OV7725_GAM13            (0x8A)
#define OV7725_GAM14            (0x8B)
#define OV7725_GAM15            (0x8C)
#define OV7725_SLOP             (0x8D)
#define OV7725_DNSTh            (0x8E)
#define OV7725_EDGE0            (0x8F)
#define OV7725_EDGE1            (0x90)
#define OV7725_DNSOff           (0x91)
#define OV7725_EDGE2            (0x92)
#define OV7725_EDGE3            (0x93)
#define OV7725_MTX1             (0x94)
#define OV7725_MTX2             (0x95)
#define OV7725_MTX3             (0x96)
#define OV7725_MTX4             (0x97)
#define OV7725_MTX5             (0x98)
#define OV7725_MTX6             (0x99)
#define OV7725_MTX_Ctrl         (0x9A)
#define OV7725_BRIGHT           (0x9B)
#define OV7725_CNST             (0x9C)
#define OV7725_UVADJ0           (0x9E)
#define OV7725_UVADJ1           (0x9F)
#define OV7725_SCAL0            (0xA0)
#define OV7725_SCAL1            (0xA1)
#define OV7725_SCAL2            (0xA2)
#define OV7725_SDE              (0xA6)
#define OV7725_USAT             (0xA7)
#define OV7725_VSAT             (0xA8)
#define OV7725_HUECOS           (0xA9)
#define OV7725_HUESIN           (0xAA)
#define OV7725_SIGN             (0xAB)
#define OV7725_DSPAuto          (0xAC)
#define OV7725_DEV_ADD          (0x42 >> 1)
//==========================================IIC �汾С���Ĵ��� �����ֲ������û�����============================================

//==============================================���� OV7725 ����ö����==================================================
typedef enum
{
    OV7725_INIT                 = 0x00,
    OV7725_RESERVE,
    OV7725_CONTRAST,
    OV7725_FPS,
    OV7725_COL,
    OV7725_ROW,
    OV7725_CONFIG_FINISH,

    OV7725_GET_WHO_AM_I         = 0xEF,
    OV7725_GET_STATUS           = 0xF1,
    OV7725_GET_VERSION          = 0xF2,

    OV7725_SET_ADDR             = 0xFE,
    OV7725_SET_DATA             = 0xFF,
}ov7725_cmd_enum;
//==============================================���� OV7725 ����ö����==================================================

//==============================================���� OV7725 ���ݴ洢����=================================================
extern vuint8       ov7725_finish_flag;                                     // һ��ͼ��ɼ���ɱ�־λ
extern uint8        ov7725_image_binary[OV7725_H][OV7725_W / 8];            // ͼ�񱣴�����
//==============================================���� OV7725 ���ݴ洢����=================================================

//=================================================OV7725 ��������===================================================
uint16      ov7725_uart_get_id      (void);                                 // ��ȡ����ͷ�̼� ID
uint16      ov7725_get_version      (void);                                 // ��ȡ����ͷ�̼��汾
uint8       ov7725_init             (void);                                 // OV7725 ����ͷ��ʼ��
//=================================================OV7725 ��������===================================================


#endif
