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
* �ļ�����          zf_device_gps_tau1201
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.8.0
* ����ƽ̨          TC264D
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding             first version
********************************************************************************************************************/
/********************************************************************************************************************
* ���߶��壺
*                  ------------------------------------
*                  ģ��ܽ�             ��Ƭ���ܽ�
*                  RX                  �鿴 zf_device_gps_tau1201.h �� GPS_TAU1201_RX �궨��
*                  TX                  �鿴 zf_device_gps_tau1201.h �� GPS_TAU1201_TX �궨��
*                  VCC                 3.3V��Դ
*                  GND                 ��Դ��
*                  ------------------------------------
********************************************************************************************************************/

#include "zf_common_function.h"
#include "zf_common_fifo.h"
#include "zf_driver_delay.h"
#include "zf_driver_uart.h"
#include "zf_device_type.h"
#include "zf_device_gps_tau1201.h"

#define GPS_TAU1201_BUFFER_SIZE     (128)

uint8                       gps_tau1201_flag;                                       // 1���ɼ���ɵȴ��������� 0��û�вɼ����
gps_info_struct             gps_tau1201;                                            // GPS����֮�������

static  uint8               gps_tau1201_state = 0;                                  // 1��GPS��ʼ�����
static  fifo_struct         gps_tau1201_receiver_fifo;                              //
static  uint8               gps_tau1201_receiver_buffer[GPS_TAU1201_BUFFER_SIZE];   // ���ݴ������

gps_state_enum              gps_gga_state;                                          // gga���״̬
gps_state_enum              gps_rmc_state;                                          // rmc���״̬

static  uint8               gps_gga_buffer[GPS_TAU1201_BUFFER_SIZE];
static  uint8               gps_rmc_buffer[GPS_TAU1201_BUFFER_SIZE];

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡָ�� ',' ���������
// ����˵��     num             �ڼ�������
// ����˵��     *str            �ַ���
// ���ز���     uint8           ��������
// ʹ��ʾ��     get_parameter_index(1, s);
// ��ע��Ϣ     �ڲ�ʹ��
//-------------------------------------------------------------------------------------------------------------------
static uint8 get_parameter_index (uint8 num, char *str)
{
    uint8 i, j = 0;
    char *temp;
    uint8 len = 0, len1;

    temp = strchr(str, '\n');
    if(NULL != temp)
    {
        len = (uint8)((uint32)temp - (uint32)str + 1);
    }

    for(i = 0; i < len; i ++)
    {
        if(str[i] == ',')
        {
            j ++;
        }
        if(j == num)
        {
            len1 =  i + 1;
            break;
        }
    }

    return len1;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �����ַ�����һ�� ',' ֮ǰ������ת��Ϊint
// ����˵��     *s              �ַ���
// ���ز���     float           ������ֵ
// ʹ��ʾ��     get_int_number(&buf[get_parameter_index(7, buf)]);
// ��ע��Ϣ     �ڲ�ʹ��
//-------------------------------------------------------------------------------------------------------------------
static int get_int_number (char *s)
{
    char buf[10];
    uint8 i;
    int return_value;
    i = get_parameter_index(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    return_value = func_str_to_int(buf);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �����ַ�����һ�� ',' ֮ǰ������ת��Ϊfloat
// ����˵��     *s              �ַ���
// ���ز���     float           ������ֵ
// ʹ��ʾ��     get_float_number(&buf[get_parameter_index(8, buf)]);
// ��ע��Ϣ     �ڲ�ʹ��
//-------------------------------------------------------------------------------------------------------------------
static float get_float_number (char *s)
{
    uint8 i;
    char buf[15];
    float return_value;

    i = get_parameter_index(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    return_value = (float)func_str_to_double(buf);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �����ַ�����һ�� ',' ֮ǰ������ת��Ϊdouble
// ����˵��     *s              �ַ���
// ���ز���     double          ������ֵ
// ʹ��ʾ��     get_double_number(&buf[get_parameter_index(3, buf)]);
// ��ע��Ϣ     �ڲ�ʹ��
//-------------------------------------------------------------------------------------------------------------------
static double get_double_number (char *s)
{
    uint8 i;
    char buf[15];
    double return_value;

    i = get_parameter_index(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    return_value = func_str_to_double(buf);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ʱ��ת��Ϊ����ʱ��
// ����˵��     *time           �����ʱ��
// ���ز���     void
// ʹ��ʾ��     utc_to_btc(&gps->time);
// ��ע��Ϣ     �ڲ�ʹ��
//-------------------------------------------------------------------------------------------------------------------
static void utc_to_btc (gps_time_struct *time)
{
    uint8 day_num;

    time->hour = time->hour + 8;
    if(time->hour > 23)
    {
        time->hour -= 24;
        time->day += 1;

        if(2 == time->month)
        {
            day_num = 28;
            if((time->year % 4 == 0 && time->year % 100 != 0) || time->year % 400 == 0) // �ж��Ƿ�Ϊ����
            {
                day_num ++;                                                     // ���� 2��Ϊ29��
            }
        }
        else
        {
            day_num = 31;                                                       // 1 3 5 7 8 10 12��Щ�·�Ϊ31��
            if(4  == time->month || 6  == time->month || 9  == time->month || 11 == time->month )
            {
                day_num = 30;
            }
        }

        if(time->day > day_num)
        {
            time->day = 1;
            time->month ++;
            if(time->month > 12)
            {
                time->month -= 12;
                time->year ++;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     RMC������
// ����˵��     *line           ���յ��������Ϣ
// ����˵��     *gps            ��������������
// ���ز���     uint8           1�������ɹ� 0�����������ⲻ�ܽ���
// ʹ��ʾ��     gps_gnrmc_parse((char *)data_buffer, &gps_tau1201);
// ��ע��Ϣ     �ڲ�ʹ��
//-------------------------------------------------------------------------------------------------------------------
static uint8 gps_gnrmc_parse (char *line, gps_info_struct *gps)
{
    uint8 state, temp;

    double  latitude;                                                           // γ��
    double  longitude;                                                          // ����

    float lati_cent_tmp, lati_second_tmp;
    float long_cent_tmp, long_second_tmp;
    float speed_tmp;
    char *buf = line;
    uint8 return_state = 0;

    state = buf[get_parameter_index(2, buf)];

    gps->state = 0;
    if (state == 'A')                                                           // ���������Ч ���������
    {
        return_state = 1;
        gps->state = 1;
        gps -> ns               = buf[get_parameter_index(4, buf)];
        gps -> ew               = buf[get_parameter_index(6, buf)];

        latitude                = get_double_number(&buf[get_parameter_index(3, buf)]);
        longitude               = get_double_number(&buf[get_parameter_index(5, buf)]);

        gps->latitude_degree    = (int)latitude / 100;                          // γ��ת��Ϊ�ȷ���
        lati_cent_tmp           = (latitude - gps->latitude_degree * 100);
        gps->latitude_cent      = (int)lati_cent_tmp;
        lati_second_tmp         = (lati_cent_tmp - gps->latitude_cent) * 10000;
        gps->latitude_second    = (int)lati_second_tmp;

        gps->longitude_degree   = (int)longitude / 100;                         // ����ת��Ϊ�ȷ���
        long_cent_tmp           = (longitude - gps->longitude_degree * 100);
        gps->longitude_cent     = (int)long_cent_tmp;
        long_second_tmp         = (long_cent_tmp - gps->longitude_cent) * 10000;
        gps->longitude_second   = (int)long_second_tmp;

        gps->latitude   = gps->latitude_degree + (double)gps->latitude_cent / 60 + (double)gps->latitude_second / 600000;
        gps->longitude  = gps->longitude_degree + (double)gps->longitude_cent / 60 + (double)gps->longitude_second / 600000;

        speed_tmp       = get_float_number(&buf[get_parameter_index(7, buf)]);  // �ٶ�(����/Сʱ)
        gps->speed      = speed_tmp * 1.85f;                                    // ת��Ϊ����/Сʱ
        gps->direction  = get_float_number(&buf[get_parameter_index(8, buf)]);  // �Ƕ�
    }

    // �ڶ�λû����ЧǰҲ����ʱ�����ݵģ�����ֱ�ӽ���
    gps->time.hour    = (buf[7] - '0') * 10 + (buf[8] - '0');                   // ʱ��
    gps->time.minute  = (buf[9] - '0') * 10 + (buf[10] - '0');
    gps->time.second  = (buf[11] - '0') * 10 + (buf[12] - '0');
    temp = get_parameter_index(9, buf);
    gps->time.day     = (buf[temp + 0] - '0') * 10 + (buf[temp + 1] - '0');     // ����
    gps->time.month   = (buf[temp + 2] - '0') * 10 + (buf[temp + 3] - '0');
    gps->time.year    = (buf[temp + 4] - '0') * 10 + (buf[temp + 5] - '0') + 2000;

    utc_to_btc(&gps->time);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     GGA������
// ����˵��     *line           ���յ��������Ϣ
// ����˵��     *gps            ��������������
// ���ز���     uint8           1�������ɹ� 0�����������ⲻ�ܽ���
// ʹ��ʾ��     gps_gngga_parse((char *)data_buffer, &gps_tau1201);
// ��ע��Ϣ     �ڲ�ʹ��
//-------------------------------------------------------------------------------------------------------------------
static uint8 gps_gngga_parse (char *line, gps_info_struct *gps)
{
    uint8 state;
    char *buf = line;
    uint8 return_state = 0;

    state = buf[get_parameter_index(2, buf)];

    if (state != ',')
    {
        gps->satellite_used = (uint8)get_int_number(&buf[get_parameter_index(7, buf)]);
        gps->height         = get_float_number(&buf[get_parameter_index(9, buf)]) + get_float_number(&buf[get_parameter_index(11, buf)]);  // �߶� = ���θ߶� + ������������Դ��ˮ׼��ĸ߶�
        return_state = 1;
    }

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ӵ�һ���㵽�ڶ�����ľ���
// ����˵��     latitude1       ��һ�����γ��
// ����˵��     longitude1      ��һ����ľ���
// ����˵��     latitude2       �ڶ������γ��
// ����˵��     longitude2      �ڶ�����ľ���
// ���ز���     double          �����������
// ʹ��ʾ��     get_two_points_distance(latitude1_1, longitude1, latitude2, longitude2);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
double get_two_points_distance (double latitude1, double longitude1, double latitude2, double longitude2)
{
    const double EARTH_RADIUS = 6378137;                                        // ����뾶(��λ��m)
    double rad_latitude1;
    double rad_latitude2;
    double rad_longitude1;
    double rad_longitude2;
    double distance;
    double a;
    double b;

    rad_latitude1 = ANGLE_TO_RAD(latitude1);                                    // ���ݽǶȼ��㻡��
    rad_latitude2 = ANGLE_TO_RAD(latitude2);
    rad_longitude1 = ANGLE_TO_RAD(longitude1);
    rad_longitude2 = ANGLE_TO_RAD(longitude2);

    a = rad_latitude1 - rad_latitude2;
    b = rad_longitude1 - rad_longitude2;

    distance = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(rad_latitude1) * cos(rad_latitude2) * pow(sin(b / 2), 2)));   // google maps ����ʵ�ֵ��㷨
    distance = distance * EARTH_RADIUS;

    return distance;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ӵ�һ���㵽�ڶ�����ķ�λ��
// ����˵��     latitude1       ��һ�����γ��
// ����˵��     longitude1      ��һ����ľ���
// ����˵��     latitude2       �ڶ������γ��
// ����˵��     longitude2      �ڶ�����ľ���
// ���ز���     double          ���ط�λ�ǣ�0��360��
// ʹ��ʾ��     get_two_points_azimuth(latitude1_1, longitude1, latitude2, longitude2);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
double get_two_points_azimuth (double latitude1, double longitude1, double latitude2, double longitude2)
{
    latitude1 = ANGLE_TO_RAD(latitude1);
    latitude2 = ANGLE_TO_RAD(latitude2);
    longitude1 = ANGLE_TO_RAD(longitude1);
    longitude2 = ANGLE_TO_RAD(longitude2);

    double x = sin(longitude2 - longitude1) * cos(latitude2);
    double y = cos(latitude1) * sin(latitude2) - sin(latitude1) * cos(latitude2) * cos(longitude2 - longitude1);
    double angle = RAD_TO_ANGLE(atan2(x, y));
    return ((angle > 0) ? angle : (angle + 360));
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����GPS����
// ����˵��     void
// ���ز���     uint8           0-�����ɹ� 1-����ʧ�� �������ݰ�����
// ʹ��ʾ��     gps_data_parse();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 gps_data_parse (void)
{
    uint8 return_state = 0;
    uint8 check_buffer[5] = {'0', 'x', 0x00, 0x00, 0x00};
    uint8 bbc_xor_origin = 0;
    uint8 bbc_xor_calculation = 0;
    uint32 data_len;

    do
    {
        if(GPS_STATE_RECEIVED == gps_rmc_state)
        {
            gps_rmc_state = GPS_STATE_PARSING;
            strncpy((char *)&check_buffer[2], strchr((const char *)gps_rmc_buffer, '*')+1, 2);
            bbc_xor_origin = (uint8)func_str_to_hex((char *)check_buffer);
            for(bbc_xor_calculation = gps_rmc_buffer[1], data_len = 2; gps_rmc_buffer[data_len] != '*'; data_len ++)
            {
                bbc_xor_calculation ^= gps_rmc_buffer[data_len];
            }
            if(bbc_xor_calculation != bbc_xor_origin)
            {
                // ����У��ʧ��
                return_state = 1;
                break;
            }

            gps_gnrmc_parse((char *)gps_rmc_buffer, &gps_tau1201);
        }
        gps_rmc_state = GPS_STATE_RECEIVING;

        if(GPS_STATE_RECEIVED == gps_gga_state)
        {
            gps_gga_state = GPS_STATE_PARSING;
            strncpy((char *)&check_buffer[2], strchr((const char *)gps_gga_buffer, '*')+1, 2);
            bbc_xor_origin = (uint8)func_str_to_hex((char *)check_buffer);

            for(bbc_xor_calculation = gps_gga_buffer[1], data_len = 2; gps_gga_buffer[data_len] != '*'; data_len ++)
            {
                bbc_xor_calculation ^= gps_gga_buffer[data_len];
            }
            if(bbc_xor_calculation != bbc_xor_origin)
            {
                // ����У��ʧ��
                return_state = 1;
                break;
            }

            gps_gngga_parse((char *)gps_gga_buffer, &gps_tau1201);
        }
        gps_gga_state = GPS_STATE_RECEIVING;

    }while(0);
    return return_state;
}


//-------------------------------------------------------------------------------------------------------------------
// �������     GPS���ڻص�����
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     gps_uart_callback();
// ��ע��Ϣ     �˺�����Ҫ�ڴ��ڽ����ж��ڽ��е���
//-------------------------------------------------------------------------------------------------------------------
void gps_uart_callback (void)
{
    uint8 temp_gps[6];
    uint32 temp_length;

    if(gps_tau1201_state)
    {
        uint8 dat;
        while(uart_query_byte(GPS_TAU1201_UART, &dat))
        {
            fifo_write_buffer(&gps_tau1201_receiver_fifo, &dat, 1);
        }

        if('\n' == dat)
        {
            // ��ȡǰ6������ �����ж��������
            temp_length = 6;
            fifo_read_buffer(&gps_tau1201_receiver_fifo, temp_gps, &temp_length, FIFO_READ_ONLY);

            // ���ݲ�ͬ���ͽ����ݿ�������ͬ�Ļ�����
            if(0 == strncmp((char *)&temp_gps[3], "RMC", 3))
            {
                // ���û���ڽ�����������»�����������
                if(GPS_STATE_PARSING != gps_rmc_state)
                {
                    gps_rmc_state = GPS_STATE_RECEIVED;
                    temp_length = fifo_used(&gps_tau1201_receiver_fifo);
                    fifo_read_buffer(&gps_tau1201_receiver_fifo, gps_rmc_buffer, &temp_length, FIFO_READ_AND_CLEAN);
                }
            }
            else if(0 == strncmp((char *)&temp_gps[3], "GGA", 3))
            {
                // ���û���ڽ�����������»�����������
                if(GPS_STATE_PARSING != gps_gga_state)
                {
                    gps_gga_state = GPS_STATE_RECEIVED;
                    temp_length = fifo_used(&gps_tau1201_receiver_fifo);
                    fifo_read_buffer(&gps_tau1201_receiver_fifo, gps_gga_buffer, &temp_length, FIFO_READ_AND_CLEAN);
                }
            }

            // ͳһ��FIFO���
            fifo_clear(&gps_tau1201_receiver_fifo);

            gps_tau1201_flag = 1;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     GPS��ʼ��
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     gps_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 gps_init (void)
{
    const uint8 set_rate[]      = {0xF1, 0xD9, 0x06, 0x42, 0x14, 0x00, 0x00, 0x0A, 0x05, 0x00, 0x64, 0x00, 0x00, 0x00, 0x60, 0xEA, 0x00, 0x00, 0xD0, 0x07, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0xB8, 0xED};
    const uint8 open_gga[]      = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x00, 0x01, 0xFB, 0x10};
    const uint8 open_rmc[]      = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x05, 0x01, 0x00, 0x1A};

    const uint8 close_gll[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x00, 0xFB, 0x11};
    const uint8 close_gsa[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x02, 0x00, 0xFC, 0x13};
    const uint8 close_grs[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15};
    const uint8 close_gsv[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x04, 0x00, 0xFE, 0x17};
    const uint8 close_vtg[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x06, 0x00, 0x00, 0x1B};
    const uint8 close_zda[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x07, 0x00, 0x01, 0x1D};
    const uint8 close_gst[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x08, 0x00, 0x02, 0x1F};
    const uint8 close_txt[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x40, 0x00, 0x3A, 0x8F};
    const uint8 close_txt_ant[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x20, 0x00, 0x1A, 0x4F};

    fifo_init(&gps_tau1201_receiver_fifo, FIFO_DATA_8BIT, gps_tau1201_receiver_buffer, GPS_TAU1201_BUFFER_SIZE);
    system_delay_ms(500);                                                           // �ȴ�GPS������ʼ��ʼ��
    uart_init(GPS_TAU1201_UART, 115200, GPS_TAU1201_RX, GPS_TAU1201_TX);

    set_wireless_type(GPS_TAU1201, gps_uart_callback);

    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)set_rate, sizeof(set_rate));       // ����GPS��������Ϊ10hz ��������ô������Ĭ��Ϊ1hz
    system_delay_ms(200);

    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)open_rmc, sizeof(open_rmc));       // ����rmc���
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)open_gga, sizeof(open_gga));       // ����gga���
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_gll, sizeof(close_gll));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_gsa, sizeof(close_gsa));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_grs, sizeof(close_grs));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_gsv, sizeof(close_gsv));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_vtg, sizeof(close_vtg));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_zda, sizeof(close_zda));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_gst, sizeof(close_gst));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_txt, sizeof(close_txt));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_txt_ant, sizeof(close_txt_ant));
    system_delay_ms(50);

    gps_tau1201_state = 1;
    uart_rx_interrupt(GPS_TAU1201_UART, 1);

    return 0;
}
