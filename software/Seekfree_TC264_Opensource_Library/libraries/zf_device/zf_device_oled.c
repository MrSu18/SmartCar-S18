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
* �ļ�����          zf_device_oled
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
*                  D0                 �鿴 zf_device_oled.h �� OLED_D0_PIN �궨��
*                  D1                 �鿴 zf_device_oled.h �� OLED_D1_PIN �궨��
*                  RES                �鿴 zf_device_oled.h �� OLED_RES_PIN �궨��
*                  DC                 �鿴 zf_device_oled.h �� OLED_DC_PIN �궨��
*                  CS                 �鿴 zf_device_oled.h �� OLED_CS_PIN �궨��
*                  VCC                3.3V��Դ
*                  GND                ��Դ��
*                  ------------------------------------
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_common_font.h"
#include "zf_common_function.h"
#include "zf_driver_delay.h"
#include "zf_driver_soft_spi.h"
#include "zf_driver_spi.h"
#include "zf_device_oled.h"

#if OLED_USE_SOFT_SPI
static soft_spi_info_struct             oled_spi;
#define oled_spi_write_8bit(data)       (soft_spi_write_8bit(&oled_spi, (data)))
#else
#define oled_spi_write_8bit(data)       (spi_write_8bit(OLED_SPI, (data)))
#endif

static oled_dir_enum        oled_display_dir    = OLED_DEFAULT_DISPLAY_DIR;
static oled_font_size_enum  oled_display_font   = OLED_DEFAULT_DISPLAY_FONT;

//-------------------------------------------------------------------------------------------------------------------
// �������     д8λ����
// ����˵��     data            ����
// ���ز���     void
// ʹ��ʾ��     oled_write_data(color);
// ��ע��Ϣ     �ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void oled_write_data (const uint8 data)
{
    OLED_DC(1);
    oled_spi_write_8bit(data);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     д����
// ����˵��     cmd             ����
// ���ز���     void
// ʹ��ʾ��     oled_write_command(0xb0 + y);
// ��ע��Ϣ     �ڲ����� �û��������
//-------------------------------------------------------------------------------------------------------------------
static void oled_write_command (const uint8 command)
{
    OLED_DC(0);
    oled_spi_write_8bit(command);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED��ʾ��������
// ����˵��     x               x����������0-127
// ����˵��     y               y����������0-7
// ���ز���     void
// ʹ��ʾ��     oled_set_coordinate(x, y);
// ��ע��Ϣ     �ڲ�ʹ���û��������
//-------------------------------------------------------------------------------------------------------------------
static void oled_set_coordinate (uint16 x, uint16 y)
{
    // �������������˶�����Ϣ ������ʾ����λ��������
    // ��ôһ������Ļ��ʾ��ʱ�򳬹���Ļ�ֱ��ʷ�Χ��
    // ���һ�������ʾ���õĺ��� �Լ�����һ�����ﳬ������Ļ��ʾ��Χ
    zf_assert(x < 128);
    zf_assert(y < 8);

    oled_write_command(0xb0 + y);
    oled_write_command(((x & 0xf0) >> 4) | 0x10);
    oled_write_command((x & 0x0f) | 0x00);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED��ʾDEBUG��Ϣ��ʼ��
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     oled_debug_init();
// ��ע��Ϣ     �ڲ�ʹ���û��������
//-------------------------------------------------------------------------------------------------------------------
static void oled_debug_init (void)
{
    debug_output_struct info;
    debug_output_struct_init(&info);

    info.type_index = 1;

    info.display_x_max = OLED_X_MAX;
    info.display_y_max = OLED_Y_MAX;
    switch(oled_display_font)
    {
        case OLED_6X8_FONT:
            info.font_x_size = 6;
            info.font_y_size = 1;
            break;
        case OLED_8X16_FONT:
            info.font_x_size = 8;
            info.font_y_size = 2;
            break;
        case OLED_16X16_FONT:
            // �ݲ�֧��
            break;
    }
    info.output_screen = oled_show_string;
    info.output_screen_clear = oled_clear;

    debug_output_init(&info);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED ��������
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     oled_clear();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void oled_clear (void)
{
    uint8 y, x;

    OLED_CS(0);
    for(y = 0; y < 8; y ++)
    {
        oled_write_command(0xb0 + y);
        oled_write_command(0x01);
        oled_write_command(0x10);
        for(x = 0; x < OLED_X_MAX; x ++)
        {
            oled_write_data(0x00);
        }
    }
    OLED_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED ��Ļ��亯��
// ����˵��     color           �����ɫѡ��(0x00 or 0xff)
// ���ز���     void
// ʹ��ʾ��     oled_full(0x00);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void oled_full (const uint8 color)
{
    uint8 y, x;

    OLED_CS(0);
    for(y = 0; y < 8; y ++)
    {
        oled_write_command(0xb0 + y);
        oled_write_command(0x01);
        oled_write_command(0x10);
        for(x = 0; x < OLED_X_MAX; x ++)
        {
            oled_write_data(color);
        }
    }
    OLED_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������ʾ����
// ����˵��     dir             ��ʾ����  ���� zf_device_oled.h �� oled_dir_enum ö���嶨��
// ���ز���     void
// ʹ��ʾ��     oled_set_dir(OLED_CROSSWISE);
// ��ע��Ϣ     �������ֻ���ڳ�ʼ����Ļ֮ǰ���ò���Ч
//-------------------------------------------------------------------------------------------------------------------
void oled_set_dir (oled_dir_enum dir)
{
    oled_display_dir = dir;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������ʾ����
// ����˵��     dir             ��ʾ����  ���� zf_device_oled.h �� oled_font_size_enum ö���嶨��
// ���ز���     void
// ʹ��ʾ��     oled_set_font(OLED_8x16_FONT);
// ��ע��Ϣ     ���������ʱ�������� ���ú���Ч ������ʾ�����µ������С
//-------------------------------------------------------------------------------------------------------------------
void oled_set_font (oled_font_size_enum font)
{
    oled_display_font = font;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ���㺯��
// ����˵��     x               x ���������� 0-127
// ����˵��     y               y ���������� 0-7
// ����˵��     color           8 ��������
// ���ز���     void
// ʹ��ʾ��     oled_draw_point(0, 0, 1);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void oled_draw_point (uint16 x, uint16 y, const uint8 color)
{
    // �������������˶�����Ϣ ������ʾ����λ��������
    // ��ôһ������Ļ��ʾ��ʱ�򳬹���Ļ�ֱ��ʷ�Χ��
    // ���һ�������ʾ���õĺ��� �Լ�����һ�����ﳬ������Ļ��ʾ��Χ
    zf_assert(x < 128);
    zf_assert(y < 8);

    OLED_CS(0);
    oled_set_coordinate(x, y);
    oled_write_command(0xb0 + y);
    oled_write_command(((x & 0xf0) >> 4) | 0x10);
    oled_write_command((x & 0x0f) | 0x00);
    oled_write_data(color);
    OLED_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED ��ʾ�ַ���
// ����˵��     x               x ���������� 0-127
// ����˵��     y               y ���������� 0-7
// ����˵��     ch[]            �ַ���
// ���ز���     void
// ʹ��ʾ��     oled_show_string(0, 0, "SEEKFREE");
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void oled_show_string (uint16 x, uint16 y, const char ch[])
{
    // �������������˶�����Ϣ ������ʾ����λ��������
    // ��ôһ������Ļ��ʾ��ʱ�򳬹���Ļ�ֱ��ʷ�Χ��
    // ���һ�������ʾ���õĺ��� �Լ�����һ�����ﳬ������Ļ��ʾ��Χ
    zf_assert(x < 128);
    zf_assert(y < 8);

    OLED_CS(0);
    uint8 c = 0, i = 0, j = 0;
    while (ch[j] != '\0')
    {
        switch(oled_display_font)
        {
            case OLED_6X8_FONT:
                c = ch[j] - 32;
                if(x > 126)
                {
                    x = 0;
                    y ++;
                }
                oled_set_coordinate(x, y);
                for(i = 0; i < 6; i ++)
                {
                    oled_write_data(ascii_font_6x8[c][i]);
                }
                x += 6;
                j ++;
                break;
            case OLED_8X16_FONT:
                c = ch[j] - 32;
                if(x > 120)
                {
                    x = 0;
                    y ++;
                }
                oled_set_coordinate(x, y);
                for(i = 0; i < 8; i ++)
                {
                    oled_write_data(ascii_font_8x16[c][i]);
                }

                oled_set_coordinate(x, y + 1);
                for(i = 0; i < 8; i ++)
                {
                    oled_write_data(ascii_font_8x16[c][i + 8]);
                }
                x += 8;
                j ++;
                break;
            case OLED_16X16_FONT:
                // �ݲ�֧��
                break;
        }
    }
    OLED_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED ��ʾ32λ�з��� (ȥ������������Ч��0)
// ����˵��     x               x���������� 0-127
// ����˵��     y               y���������� 0-7
// ����˵��     dat             ��Ҫ��ʾ�ı��� �������� int32
// ����˵��     num             ��Ҫ��ʾ��λ�� ���10λ  ������������
// ���ز���     void
// ʹ��ʾ��     oled_show_int(0, 0, x, 3);                      // x ����Ϊ int32 int16 int8 ����
// ��ע��Ϣ     ��������ʾһ�� ��-����   ������ʾһ���ո�
//-------------------------------------------------------------------------------------------------------------------
void oled_show_int (uint16 x, uint16 y, const int32 dat, uint8 num)
{
    // �������������˶�����Ϣ ������ʾ����λ��������
    // ��ôһ������Ļ��ʾ��ʱ�򳬹���Ļ�ֱ��ʷ�Χ��
    // ���һ�������ʾ���õĺ��� �Լ�����һ�����ﳬ������Ļ��ʾ��Χ
    zf_assert(x < 128);
    zf_assert(y < 8);

    zf_assert(num > 0);
    zf_assert(num <= 10);

    int32 dat_temp = dat;
    int32 offset = 1;
    char data_buffer[12];

    memset(data_buffer, 0, 12);
    memset(data_buffer, ' ', num + 1);

    if(num < 10)
    {
        for(; num > 0; num --)
        {
            offset *= 10;
        }
        dat_temp %= offset;
    }
    func_int_to_str(data_buffer, dat_temp);
    oled_show_string(x, y, (const char *)&data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED ��ʾ32λ�޷��� (ȥ������������Ч��0)
// ����˵��     x               x ���������� 0-127
// ����˵��     y               y ���������� 0-7
// ����˵��     dat             ��Ҫ��ʾ�ı��� �������� uint32
// ����˵��     num             ��Ҫ��ʾ��λ�� ���10λ  ������������
// ���ز���     void
// ʹ��ʾ��     oled_show_uint(0, 0, x, 3);                     // x ����Ϊ uint32 uint16 uint8 ����
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void oled_show_uint (uint16 x,uint16 y,const uint32 dat,uint8 num)
{
    // �������������˶�����Ϣ ������ʾ����λ��������
    // ��ôһ������Ļ��ʾ��ʱ�򳬹���Ļ�ֱ��ʷ�Χ��
    // ���һ�������ʾ���õĺ��� �Լ�����һ�����ﳬ������Ļ��ʾ��Χ
    zf_assert(x < 128);
    zf_assert(y < 8);

    zf_assert(num > 0);
    zf_assert(num <= 10);

    uint32 dat_temp = dat;
    int32 offset = 1;
    char data_buffer[12];
    memset(data_buffer, 0, 12);
    memset(data_buffer, ' ', num);

    if(num < 10)
    {
        for(; num > 0; num --)
        {
            offset *= 10;
        }
        dat_temp %= offset;
    }
    func_uint_to_str(data_buffer, dat_temp);
    oled_show_string(x, y, (const char *)&data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED ��ʾ������ (ȥ������������Ч��0)
// ����˵��     x               x ���������� 0-127
// ����˵��     y               y ���������� 0-7
// ����˵��     dat             ��Ҫ��ʾ�ı�������������float��double
// ����˵��     num             ����λ��ʾ����   ���8λ
// ����˵��     pointnum        С��λ��ʾ����   ���6λ
// ���ز���     void
// ʹ��ʾ��     oled_show_float(0, 0, x, 2, 3);                 // ��ʾ������   ������ʾ2λ   С����ʾ��λ
// ��ע��Ϣ     �ر�ע�⵱����С��������ʾ��ֵ����д���ֵ��һ����ʱ��
//              ���������ڸ��������ȶ�ʧ���⵼�µģ��Ⲣ������ʾ���������⣬
//              �й���������飬�����аٶ�ѧϰ   ���������ȶ�ʧ���⡣
//              ��������ʾһ�� ��-����   ������ʾһ���ո�
//-------------------------------------------------------------------------------------------------------------------
void oled_show_float (uint16 x,uint16 y,const float dat,uint8 num,uint8 pointnum)
{
    // �������������˶�����Ϣ ������ʾ����λ��������
    // ��ôһ������Ļ��ʾ��ʱ�򳬹���Ļ�ֱ��ʷ�Χ��
    // ���һ�������ʾ���õĺ��� �Լ�����һ�����ﳬ������Ļ��ʾ��Χ
    zf_assert(x < 128);
    zf_assert(y < 8);

    zf_assert(num > 0);
    zf_assert(num <= 8);
    zf_assert(pointnum > 0);
    zf_assert(pointnum <= 6);

    float dat_temp = dat;
    float offset = 1.0;
    char data_buffer[17];
    memset(data_buffer, 0, 17);
    memset(data_buffer, ' ', num + pointnum + 2);

    if(num < 10)
    {
        for(; num > 0; num --)
        {
            offset *= 10;
        }
        dat_temp = dat_temp - ((int)dat_temp / (int)offset) * offset;
    }
    func_float_to_str(data_buffer, dat_temp, pointnum);
    oled_show_string(x, y, data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED ��ʾ��ֵͼ�� ����ÿ�˸������һ���ֽ�����
// ����˵��     x               x ���������� 0-127
// ����˵��     y               y ���������� 0-7
// ����˵��     *image          ͼ������ָ��
// ����˵��     width           ͼ��ʵ�ʿ��
// ����˵��     height          ͼ��ʵ�ʸ߶�
// ����˵��     dis_width       ͼ����ʾ��� ������Χ [0, 128]
// ����˵��     dis_height      ͼ����ʾ�߶� ������Χ [0, 64]
// ���ز���     void
// ʹ��ʾ��     oled_show_binary_image(0, 0, ov7725_image_binary[0], OV7725_W, OV7725_H, OV7725_W, OV7725_H);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void oled_show_binary_image (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height)
{
    // �������������˶�����Ϣ ������ʾ����λ��������
    // ��ôһ������Ļ��ʾ��ʱ�򳬹���Ļ�ֱ��ʷ�Χ��
    // ���һ�������ʾ���õĺ��� �Լ�����һ�����ﳬ������Ļ��ʾ��Χ
    zf_assert(x < 128);
    zf_assert(y < 8);
    zf_assert(image != NULL);

    uint32 i = 0, j = 0, z = 0;
    uint8 dat;
    uint32 width_index = 0, height_index = 0;

    OLED_CS(0);
    dis_height = dis_height - dis_height % 8;
    dis_width = dis_width - dis_width % 8;
    for(j = 0; j < dis_height; j += 8)
    {
        oled_set_coordinate(x + 0, (uint16)(y + j / 8));
        height_index = j * height / dis_height;
        for(i = 0; i < dis_width; i += 8)
        {
            width_index = i * width / dis_width / 8;
            for(z = 0; z < 8; z ++)
            {
                dat = 0;
                if(*(image + height_index * width / 8 + width_index + width / 8 * 0) & (0x80 >> z))
                {
                    dat |= 0x01;
                }
                if(*(image + height_index * width / 8 + width_index + width / 8 * 1) & (0x80 >> z))
                {
                    dat |= 0x02;
                }
                if(*(image + height_index * width / 8 + width_index + width / 8 * 2) & (0x80 >> z))
                {
                    dat |= 0x04;
                }
                if(*(image + height_index * width / 8 + width_index + width / 8 * 3) & (0x80 >> z))
                {
                    dat |= 0x08;
                }
                if(*(image + height_index * width / 8 + width_index + width / 8 * 4) & (0x80 >> z))
                {
                    dat |= 0x10;
                }
                if(*(image + height_index * width / 8 + width_index + width / 8 * 5) & (0x80 >> z))
                {
                    dat |= 0x20;
                }
                if(*(image + height_index * width / 8 + width_index + width / 8 * 6) & (0x80 >> z))
                {
                    dat |= 0x40;
                }
                if(*(image + height_index * width / 8 + width_index + width / 8 * 7) & (0x80 >> z))
                {
                    dat |= 0x80;
                }
                oled_write_data(dat);
            }
        }
    }
    OLED_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED ��ʾ 8bit �Ҷ�ͼ�� ����ֵ����ֵ
// ����˵��     x               x ���������� 0-127
// ����˵��     y               y ���������� 0-7
// ����˵��     *image          ͼ������ָ��
// ����˵��     width           ͼ��ʵ�ʿ��
// ����˵��     height          ͼ��ʵ�ʸ߶�
// ����˵��     dis_width       ͼ����ʾ��� ������Χ [0, 128]
// ����˵��     dis_height      ͼ����ʾ�߶� ������Χ [0, 64]
// ����˵��     threshold       ��ֵ����ʾ��ֵ 0-��������ֵ��
// ���ز���     void
// ʹ��ʾ��     oled_show_gray_image(0, 0, mt9v03x_image[0], width, height, 128, 64, x);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void oled_show_gray_image (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 threshold)
{
    // �������������˶�����Ϣ ������ʾ����λ��������
    // ��ôһ������Ļ��ʾ��ʱ�򳬹���Ļ�ֱ��ʷ�Χ��
    // ���һ�������ʾ���õĺ��� �Լ�����һ�����ﳬ������Ļ��ʾ��Χ
    zf_assert(x < 128);
    zf_assert(y < 8);
    zf_assert(image != NULL);

    int16 i, j;
    uint8 dat;
    uint32 width_index = 0, height_index = 0;

    OLED_CS(0);
    dis_height = dis_height - dis_height % 8;
    for(j = 0; j < dis_height; j += 8)
    {
        oled_set_coordinate(x + 0, y + j / 8);
        height_index = j * height / dis_height;
        for(i = 0; i < dis_width; i ++)
        {
            width_index = i * width / dis_width;
            dat = 0;
            if(*(image + height_index * width + width_index + width * 0) > threshold)
            {
                dat |= 0x01;
            }
            if(*(image + height_index * width + width_index + width * 1) > threshold)
            {
                dat |= 0x02;
            }
            if(*(image + height_index * width + width_index + width * 2) > threshold)
            {
                dat |= 0x04;
            }
            if(*(image + height_index * width + width_index + width * 3) > threshold)
            {
                dat |= 0x08;
            }
            if(*(image + height_index * width + width_index + width * 4) > threshold)
            {
                dat |= 0x10;
            }
            if(*(image + height_index * width + width_index + width * 5) > threshold)
            {
                dat |= 0x20;
            }
            if(*(image + height_index * width + width_index + width * 6) > threshold)
            {
                dat |= 0x40;
            }
            if(*(image + height_index * width + width_index + width * 7) > threshold)
            {
                dat |= 0x80;
            }
            oled_write_data(dat);
        }
    }
    OLED_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED ��ʾ����
// ����˵��     x               x ���������� 0-127
// ����˵��     y               y ���������� 0-7
// ����˵��     *wave           ��������ָ��
// ����˵��     width           ����ʵ�ʿ��
// ����˵��     value_max       ����ʵ�����ֵ
// ����˵��     dis_width       ������ʾ��� ������Χ [0, 128]
// ����˵��     dis_value_max   ������ʾ���ֵ ������Χ [0, 64]
// ���ز���     void
// ʹ��ʾ��     oled_show_wave(0, 0, data, 128, 64, 128, 64);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void oled_show_wave (uint16 x, uint16 y, const uint16 *wave, uint16 width, uint16 value_max, uint16 dis_width, uint16 dis_value_max)
{
    // �������������˶�����Ϣ ������ʾ����λ��������
    // ��ôһ������Ļ��ʾ��ʱ�򳬹���Ļ�ֱ��ʷ�Χ��
    // ���һ�������ʾ���õĺ��� �Լ�����һ�����ﳬ������Ļ��ʾ��Χ
    zf_assert(x < 128);
    zf_assert(y < 8);
    zf_assert(wave != NULL);

    uint32 i = 0;
    uint32 width_index = 0, value_max_index = 0;
    uint8 dis_h = 0;

    uint32 x_temp = 0;
    uint32 y_temp = 0;

    OLED_CS(0);
    for(y_temp = 0; y_temp < dis_value_max; y_temp += 8)
    {
        oled_set_coordinate(x + 0, (uint16)(y + y_temp / 8));
        for(x_temp = 0; x_temp < dis_width; x_temp ++)
            oled_write_data(0x00);
    }
    for(i = 0; i < dis_width; i ++)
    {
        width_index = i * width / dis_width;
        value_max_index = *(wave + width_index) * (dis_value_max - 1) / value_max;

        dis_h = (uint8)((dis_value_max - 1) - value_max_index);
        oled_set_coordinate((uint16)(i + x), dis_h / 8 + y);
        dis_h = (0x01 << dis_h % 8);
        oled_write_data(dis_h);
    }
    OLED_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������ʾ
// ����˵��     x               ������ 0-127
// ����˵��     y               ������ 0-7
// ����˵��     size            ȡģ��ʱ�����õĺ��������С��Ҳ����һ������ռ�õĵ��󳤿�Ϊ���ٸ��㣬ȡģ��ʱ����Ҫ������һ���ġ�
// ����˵��     *chinese_buffer ��Ҫ��ʾ�ĺ�������
// ����˵��     number          ��Ҫ��ʾ����λ
// ���ز���     void
// ʹ��ʾ��     oled_show_chinese(0, 6, 16, (const uint8 *)oled_16x16_chinese, 4);
// ��ע��Ϣ     ʹ��PCtoLCD2002���ȡģ       ���롢����ʽ��˳��       16*16
//-------------------------------------------------------------------------------------------------------------------
void oled_show_chinese (uint16 x, uint16 y, uint8 size, const uint8 *chinese_buffer, uint8 number)
{
    // �������������˶�����Ϣ ������ʾ����λ��������
    // ��ôһ������Ļ��ʾ��ʱ�򳬹���Ļ�ֱ��ʷ�Χ��
    // ���һ�������ʾ���õĺ��� �Լ�����һ�����ﳬ������Ļ��ʾ��Χ
    zf_assert(x < 128);
    zf_assert(y < 8);
    zf_assert(chinese_buffer != NULL);

    int16 i, j, k;

    OLED_CS(0);
    for(i = 0; i < number; i ++)
    {
        for(j = 0; j < (size / 8); j ++)
        {
            oled_set_coordinate(x + i * size, y + j);
            for(k = 0; k < 16; k ++)
            {
                oled_write_data(*chinese_buffer);
                chinese_buffer ++;
            }
        }
    }
    OLED_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     OLED��ʼ������
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     oled_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void oled_init (void)
{
#if OLED_USE_SOFT_SPI
    soft_spi_init(&oled_spi, 0, OLED_SOFT_SPI_DELAY, OLED_D0_PIN, OLED_D1_PIN, SOFT_SPI_PIN_NULL, SOFT_SPI_PIN_NULL);
#else
    spi_init(OLED_SPI, SPI_MODE0, OLED_SPI_SPEED, OLED_D0_PIN, OLED_D1_PIN, OLED_D1_PIN_IN, SPI_CS_NULL);
#endif
    gpio_init(OLED_RES_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(OLED_DC_PIN , GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(OLED_CS_PIN , GPO, GPIO_HIGH, GPO_PUSH_PULL);

    oled_set_dir(oled_display_dir);
    oled_debug_init();

    OLED_CS(0);
    OLED_RES(0);
    system_delay_ms(50);
    OLED_RES(1);

    oled_write_command(0xae);                                                   // --turn off oled panel
    oled_write_command(0x00);                                                   // ---set low column address
    oled_write_command(0x10);                                                   // ---set high column address
    oled_write_command(0x40);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         // --set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    oled_write_command(0x81);                                                   // --set contrast control register
    oled_write_command(OLED_BRIGHTNESS);                                        //  Set SEG Output Current Brightness

    if (oled_display_dir == OLED_CROSSWISE)
    {
        oled_write_command(0xa1);                                               // --Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
        oled_write_command(0xc8);                                               // Set COM/Row Scan Direction   0xc0���·��� 0xc8����
    }
    else
    {
        oled_write_command(0xa0);                                               // --Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
        oled_write_command(0xc0);                                               // Set COM/Row Scan Direction   0xc0���·��� 0xc8����
    }

    oled_write_command(0xa6);                                                   // --set normal display
    oled_write_command(0xa8);                                                   // --set multiplex ratio(1 to 64)
    oled_write_command(0x3f);                                                   // --1/64 duty
    oled_write_command(0xd3);                                                   // -set display offset  Shift Mapping RAM Counter (0x00~0x3F)
    oled_write_command(0x00);                                                   // -not offset
    oled_write_command(0xd5);                                                   // --set display clock divide ratio/oscillator frequency
    oled_write_command(0x80);                                                   // --set divide ratio, Set Clock as 100 Frames/Sec
    oled_write_command(0xd9);                                                   // --set pre-charge period
    oled_write_command(0xf1);                                                   // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    oled_write_command(0xda);                                                   // --set com pins hardware configuration
    oled_write_command(0x12);
    oled_write_command(0xdb);                                                   // --set vcomh
    oled_write_command(0x40);                                                   // Set VCOM Deselect Level
    oled_write_command(0x20);                                                   // -Set Page Addressing Mode (0x00/0x01/0x02)
    oled_write_command(0x02);                                                   //
    oled_write_command(0x8d);                                                   // --set Charge Pump enable/disable
    oled_write_command(0x14);                                                   // --set(0x10) disable
    oled_write_command(0xa4);                                                   //  Disable Entire Display On (0xa4/0xa5)
    oled_write_command(0xa6);                                                   //  Disable Inverse Display On (0xa6/a7)
    oled_write_command(0xaf);                                                   // --turn on oled panel
    OLED_CS(1);

    oled_clear();                                                               // ��ʼ����
    oled_set_coordinate(0, 0);
}
