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
* �ļ�����          zf_driver_flash
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

#include "IfxScuWdt.h"
#include "IfxFlash.h"
#include "zf_common_debug.h"
#include "zf_driver_flash.h"



flash_data_union flash_union_buffer[EEPROM_PAGE_LENGTH];                    // FLASH ���������ݻ�����

//-------------------------------------------------------------------------------------------------------------------
//  �������      У��FLASHҳ�Ƿ�������
//  ����˵��      sector_num    ������д0  �˴�������Ų���ʵ�����ã�ֻ�������ӿ�
//  ����˵��      page_num      ��Ҫд���ҳ���   ������Χ0-11
//  ���ز���      ����1�����ݣ�����0û�����ݣ������Ҫ�������ݵ�����д���µ�������Ӧ�ö������������в�������
//  ʹ��ʾ��      flash_check(0, 0); // У��0ҳ�Ƿ�������
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 flash_check (uint32 sector_num, uint32 page_num)
{
    zf_assert(EEPROM_PAGE_NUM > page_num);

    uint32 sector_addr = IfxFlash_dFlashTableEepLog[page_num].start;

    uint32 num = 0;

    for(num = 0; num < EEPROM_PAGE_LENGTH && *(uint32 *)(sector_addr + num  * FLASH_DATA_SIZE) == 0; num ++);

    return num == EEPROM_PAGE_LENGTH ? 0 : 1;
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ����ҳ
//  ����˵��      sector_num    ������д0  �˴�������Ų���ʵ�����ã�ֻ�������ӿ�
//  ����˵��      page_num      ��Ҫд���ҳ���   ������Χ0-11
//  ���ز���      void
//  ʹ��ʾ��      flash_erase_page(0, 0);
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void flash_erase_page (uint32 sector_num, uint32 page_num)
{
    zf_assert(EEPROM_PAGE_NUM > page_num);

    uint32 flash = 0;
    uint16 end_init_sfty_pw;
    uint32 sector_addr = IfxFlash_dFlashTableEepLog[page_num].start;

    end_init_sfty_pw   = IfxScuWdt_getSafetyWatchdogPassword();

    IfxScuWdt_clearSafetyEndinit(end_init_sfty_pw);
    IfxFlash_eraseSector        (sector_addr);
    IfxScuWdt_setSafetyEndinit  (end_init_sfty_pw);

    IfxFlash_waitUnbusy(flash, IfxFlash_FlashType_D0);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡһҳ
// ����˵��     sector_num      ������д0  �˴�������Ų���ʵ�����ã�ֻ�������ӿ�
// ����˵��     page_num        ��ǰ����ҳ�ı��   ������Χ <0 - 11>
// ����˵��     buf             ��Ҫ��ȡ�����ݵ�ַ   ������������ͱ���Ϊuint32
// ����˵��     len             ��Ҫд������ݳ���   ������Χ 1-1023
// ���ز���     void
// ʹ��ʾ��     flash_read_page(0, 11, data_buffer, 256);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void flash_read_page(uint32 sector_num, uint32 page_num, uint32 *buf, uint16 len)
{
    uint32 data_cont = 0;
    zf_assert(EEPROM_PAGE_NUM > page_num);
    zf_assert(EEPROM_PAGE_LENGTH >= len);

    for(data_cont = 0; data_cont < len; data_cont ++)
    {
        *buf ++ = *((uint32 *)((EEPROM_BASE_ADDR + page_num * EEPROM_PAGE_SIZE) + (data_cont * FLASH_DATA_SIZE)));
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ���һҳ
//  ����˵��      sector_num      ������д0  �˴�������Ų���ʵ�����ã�ֻ�������ӿ�
//  ����˵��      page_num        ��ǰ����ҳ�ı��    ������Χ <0 - 11>
//  ����˵��      buf             ��Ҫд������ݵ�ַ   ������������ͱ���Ϊ uint32
//  ����˵��      len             ��Ҫд������ݳ���   ������Χ 1-1024
//  ���ز���      void
//  ʹ��ʾ��      flash_write_page(0, 0, buf, 10);
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void flash_write_page (uint32 sector_num, uint32 page_num, const uint32 *buf, uint16 len)
{
    zf_assert(EEPROM_PAGE_NUM > page_num);
    zf_assert(EEPROM_PAGE_LENGTH >= len);

    uint16 end_init_sfty_pw;
    uint32 flash_addr = IfxFlash_dFlashTableEepLog[page_num].start;
    uint32 data_addr = 0;
    uint32 data_cont = 0;
    end_init_sfty_pw   = IfxScuWdt_getSafetyWatchdogPassword();

    for(data_cont = 0; data_cont < len; data_cont ++)
    {
        data_addr  = flash_addr + data_cont * FLASH_DATA_SIZE;

        zf_assert(0 == IfxFlash_enterPageMode(data_addr));

        IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

        IfxFlash_loadPage(data_addr, *buf ++, 0);

        IfxScuWdt_clearSafetyEndinit(end_init_sfty_pw);
        IfxFlash_writePage          (data_addr);
        IfxScuWdt_setSafetyEndinit  (end_init_sfty_pw);

        IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ָ�� FLASH ��������ָ��ҳ���ȡ���ݵ�������
// ����˵��     sector_num      ������д0  �˴�������Ų���ʵ�����ã�ֻ�������ӿ�
// ����˵��     page_num        ��ǰ����ҳ�ı��   ������Χ <0 - 11>
// ���ز���     void
// ʹ��ʾ��     flash_read_page_to_buffer(0, 11);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void flash_read_page_to_buffer (uint32 sector_num, uint32 page_num)
{
    uint32 data_cont = 0;
    zf_assert(EEPROM_PAGE_NUM > page_num);

    uint32 flash_addr = IfxFlash_dFlashTableEepLog[page_num].start; // ��ȡ��ǰ Flash ��ַ

    for(data_cont = 0; data_cont < EEPROM_PAGE_LENGTH; data_cont ++)
    {
        flash_union_buffer[data_cont].uint32_type = *((uint32 *)(flash_addr + (data_cont * FLASH_DATA_SIZE)));
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ָ�� FLASH ��������ָ��ҳ��д�뻺����������
// ����˵��     sector_num      ������д0  �˴�������Ų���ʵ�����ã�ֻ�������ӿ�
// ����˵��     page_num        ��ǰ����ҳ�ı��   ������Χ <0 - 11>
// ���ز���     uint8           1-��ʾʧ�� 0-��ʾ�ɹ�
// ʹ��ʾ��     flash_write_page_from_buffer(0, 11);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 flash_write_page_from_buffer (uint32 sector_num, uint32 page_num)
{
    uint32 *data_pointer = (uint32 *)flash_union_buffer;

    zf_assert(EEPROM_PAGE_NUM > page_num);

    flash_write_page(0, page_num, data_pointer, EEPROM_PAGE_LENGTH);

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������ݻ�����
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     flash_buffer_clear();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void flash_buffer_clear (void)
{
    memset(flash_union_buffer, 0xFF, EEPROM_PAGE_LENGTH);
}

