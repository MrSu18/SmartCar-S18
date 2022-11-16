/*********************************************************************************************************************
* MM32F327X-G9P Opensourec Library ����MM32F327X-G9P ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� MM32F327X-G9P ��Դ���һ����
*
* MM32F327X-G9P ��Դ�� ��������
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
* �ļ�����          zf_device_key
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          IAR 8.32.4 or MDK 5.37
* ����ƽ̨          MM32F327X_G9P
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-08-10        Teternal            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* ���߶��壺
*                   ------------------------------------
*                   ģ��ܽ�            ��Ƭ���ܽ�
*                   // һ�������尴����Ӧ������
*                   KEY1/S1             �鿴 zf_device_key.h �� KEY_LIST[0]
*                   KEY2/S2             �鿴 zf_device_key.h �� KEY_LIST[1]
*                   KEY3/S3             �鿴 zf_device_key.h �� KEY_LIST[2]
*                   KEY4/S4             �鿴 zf_device_key.h �� KEY_LIST[3]
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_device_key.h"

static uint32               scanner_period = 0;                                 // ������ɨ������
static uint32               key_press_time[KEY_NUMBER];                         // �����źų���ʱ��
static key_state_enum       key_state[KEY_NUMBER];                              // ����״̬

static const gpio_pin_enum  key_index[KEY_NUMBER] = KEY_LIST;

//-------------------------------------------------------------------------------------------------------------------
// �������     ����״̬ɨ��
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     key_scanner();
// ��ע��Ϣ     �������������ѭ������ PIT �ж���
//-------------------------------------------------------------------------------------------------------------------
void key_scanner (void)
{
    uint8 i = 0;
    for(i = 0; i < KEY_NUMBER; i ++)
    {
        switch(key_state[i])
        {
            case KEY_RELEASE:
                if(KEY_RELEASE_LEVEL != gpio_get_level(key_index[i]))
                {
                    if(++ key_press_time[i] >= KEY_MAX_SHOCK_PERIOD / scanner_period)
                    {
                        key_state[i] = KEY_SHORT_PRESS;
                    }
                    else
                    {
                        key_state[i] = KEY_CHECK_SHOCK;
                    }
                }
                break;
            case KEY_CHECK_SHOCK:
                if(KEY_RELEASE_LEVEL != gpio_get_level(key_index[i]))
                {
                    if(++ key_press_time[i] >= KEY_MAX_SHOCK_PERIOD / scanner_period)
                    {
                        key_state[i] = KEY_SHORT_PRESS;
                    }
                }
                else
                {
                    key_state[i] = KEY_RELEASE;
                    key_press_time[i] = 0;
                }
                break;
            case KEY_SHORT_PRESS:
                if(++ key_press_time[i] >= KEY_LONG_PRESS_PERIOD / scanner_period)
                {
                    key_state[i] = KEY_LONG_PRESS;
                }
                if(KEY_RELEASE_LEVEL == gpio_get_level(key_index[i]))
                {
                    key_state[i] = KEY_RELEASE;
                    key_press_time[i] = 0;
                }
                break;
            case KEY_LONG_PRESS:
                if(KEY_RELEASE_LEVEL == gpio_get_level(key_index[i]))
                {
                    key_state[i] = KEY_RELEASE;
                    key_press_time[i] = 0;
                }
                break;

        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ����״̬
// ����˵��     key_n           ��������
// ���ز���     key_state_enum  ����״̬
// ʹ��ʾ��     key_get_state(KEY_1);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
key_state_enum key_get_state (key_index_enum key_n)
{
    return key_state[key_n];
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������ʼ��
// ����˵��     period          ����ɨ������ �Ժ���Ϊ��λ
// ���ز���     void
// ʹ��ʾ��     key_init(10);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 key_init (uint32 period)
{
    uint8 loop_temp = 0;

    zf_assert(0 < period);      // ɨ�����ڱ������0

    for(loop_temp = 0; loop_temp < KEY_NUMBER; loop_temp ++)
    {
        gpio_init(key_index[loop_temp], GPI, GPIO_HIGH, GPI_PULL_UP);
        key_state[loop_temp] = KEY_RELEASE;
    }
    scanner_period = period;
    return 0;
}
