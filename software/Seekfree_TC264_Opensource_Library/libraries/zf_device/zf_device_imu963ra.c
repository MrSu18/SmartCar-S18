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
* �ļ�����          zf_device_imu963ra
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
/********************************************************************************************************************
* ���߶��壺
*                  ------------------------------------
*                  ģ��ܽ�             ��Ƭ���ܽ�
*                  // Ӳ�� SPI ����
*                  SCL/SPC            �鿴 zf_device_imu963ra.h �� IMU963RA_SPC_PIN �궨��
*                  SDA/DSI            �鿴 zf_device_imu963ra.h �� IMU963RA_SDI_PIN �궨��
*                  SA0/SDO            �鿴 zf_device_imu963ra.h �� IMU963RA_SDO_PIN �궨��
*                  CS                 �鿴 zf_device_imu963ra.h �� IMU963RA_CS_PIN  �궨��
*                  VCC                3.3V��Դ
*                  GND                ��Դ��
*                  ������������
*
*                  // ��� IIC ����
*                  SCL/SPC            �鿴 zf_device_imu963ra.h �� IMU963RA_SCL_PIN �궨��
*                  SDA/DSI            �鿴 zf_device_imu963ra.h �� IMU963RA_SDA_PIN �궨��
*                  VCC                3.3V��Դ
*                  GND                ��Դ��
*                  ������������
*                  ------------------------------------
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_driver_delay.h"
#include "zf_driver_spi.h"
#include "zf_driver_soft_iic.h"
#include "zf_device_imu963ra.h"

int16 imu963ra_gyro_x = 0, imu963ra_gyro_y = 0, imu963ra_gyro_z = 0;
int16 imu963ra_acc_x = 0,  imu963ra_acc_y = 0,  imu963ra_acc_z = 0;
int16 imu963ra_mag_x = 0,  imu963ra_mag_y = 0,  imu963ra_mag_z = 0;

#if IMU963RA_USE_SOFT_IIC
static soft_iic_info_struct imu963ra_iic_struct;

//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA д�Ĵ���
// ����˵��     reg             �Ĵ�����ַ
// ����˵��     data            ����
// ���ز���     void
// ʹ��ʾ��     imu963ra_write_acc_gyro_register(IMU963RA_SLV0_CONFIG, 0x00);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
#define imu963ra_write_acc_gyro_register(reg,data)       (soft_iic_write_8bit_register(&imu963ra_iic_struct,reg,data))

//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA ���Ĵ���
// ����˵��     reg             �Ĵ�����ַ
// ���ز���     uint8           ����
// ʹ��ʾ��     imu963ra_read_acc_gyro_register(IMU963RA_STATUS_MASTER);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
#define imu963ra_read_acc_gyro_register(reg)             (soft_iic_sccb_read_register(&imu963ra_iic_struct,reg))

//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA ������ �ڲ�����
// ����˵��     reg             �Ĵ�����ַ
// ����˵��     data            ���ݻ�����
// ����˵��     len             ���ݳ���
// ���ز���     void
// ʹ��ʾ��     imu963ra_read_acc_gyro_registers(IMU963RA_OUTX_L_A, dat, 6);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
#define imu963ra_read_acc_gyro_registers(reg,data,len)   (soft_iic_read_8bit_registers(&imu963ra_iic_struct,reg,data,len))
#else
//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA д�Ĵ���
// ����˵��     reg             �Ĵ�����ַ
// ����˵��     data            ����
// ���ز���     void
// ʹ��ʾ��     imu963ra_write_acc_gyro_register(IMU963RA_SLV0_CONFIG, 0x00);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static void imu963ra_write_acc_gyro_register(uint8 reg, uint8 data)
{
    IMU963RA_CS(0);
    spi_write_8bit_register(IMU963RA_SPI, reg | IMU963RA_SPI_W, data);

    IMU963RA_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA ���Ĵ���
// ����˵��     reg             �Ĵ�����ַ
// ���ز���     uint8           ����
// ʹ��ʾ��     imu963ra_read_acc_gyro_register(IMU963RA_STATUS_MASTER);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu963ra_read_acc_gyro_register(uint8 reg)
{
    uint8 data = 0;
    IMU963RA_CS(0);
    data = spi_read_8bit_register(IMU963RA_SPI, reg | IMU963RA_SPI_R);

    IMU963RA_CS(1);
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA ������ �ڲ�����
// ����˵��     reg             �Ĵ�����ַ
// ����˵��     data            ���ݻ�����
// ����˵��     len             ���ݳ���
// ���ز���     void
// ʹ��ʾ��     imu963ra_read_acc_gyro_registers(IMU963RA_OUTX_L_A, dat, 6);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static void imu963ra_read_acc_gyro_registers(uint8 reg, uint8 *data, uint32 len)
{
    IMU963RA_CS(0);
    spi_read_8bit_registers(IMU963RA_SPI, reg | IMU963RA_SPI_R, data, len);

    IMU963RA_CS(1);
}
#endif

//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA ��Ϊ IIC �����������д����
// ����˵��     addr            Ŀ���ַ
// ����˵��     reg             Ŀ��Ĵ���
// ����˵��     data            ����
// ���ز���     uint8           1-ʧ�� 0-�ɹ�
// ʹ��ʾ��     imu963ra_write_mag_register(IMU963RA_MAG_ADDR, IMU963RA_MAG_CONTROL2, 0x80);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu963ra_write_mag_register (uint8 addr, uint8 reg, uint8 data)
{
    uint8 return_state = 0;
    uint16 timeout_count = 0;

    addr = addr << 1;
    imu963ra_write_acc_gyro_register(IMU963RA_SLV0_CONFIG, 0x00);               // �ӻ�0�������
    imu963ra_write_acc_gyro_register(IMU963RA_SLV0_ADD, addr | 0);              // ���õشżƵ�ַ��ע��������Ҫ����8λ��I2C��ַ�� 0x2C
    imu963ra_write_acc_gyro_register(IMU963RA_SLV0_SUBADD, reg);                // ��Ҫд��ļĴ�����ַ
    imu963ra_write_acc_gyro_register(IMU963RA_DATAWRITE_SLV0, data);            // ��Ҫд�������
    imu963ra_write_acc_gyro_register(IMU963RA_MASTER_CONFIG, 0x4C);             // ���ڵ�һ����������ͨѶ �������� I2C����ʹ��

    // �ȴ�ͨѶ�ɹ�
    while(0 == (0x80 & imu963ra_read_acc_gyro_register(IMU963RA_STATUS_MASTER)))
    {
        if(timeout_count ++ > IMU963RA_TIMEOUT_COUNT)
        {
            return_state = 1;
            break;
        }
        system_delay_ms(2);
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA ��Ϊ IIC ����������ƶ�����
// ����˵��     addr            Ŀ���ַ
// ����˵��     reg             Ŀ��Ĵ���
// ���ز���     uint8           ��ȡ������
// ʹ��ʾ��     imu963ra_read_mag_register(IMU963RA_MAG_ADDR, IMU963RA_MAG_CHIP_ID);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu963ra_read_mag_register (uint8 addr, uint8 reg)
{
    uint16 timeout_count = 0;

    addr = addr << 1;
    imu963ra_write_acc_gyro_register(IMU963RA_SLV0_ADD, addr | 1);              // ���õشżƵ�ַ��ע��������Ҫ����8λ��I2C��ַ�� 0x2C
    imu963ra_write_acc_gyro_register(IMU963RA_SLV0_SUBADD, reg);                // ��Ҫ��ȡ�ļĴ�����ַ
    imu963ra_write_acc_gyro_register(IMU963RA_SLV0_CONFIG, 0x01);
    imu963ra_write_acc_gyro_register(IMU963RA_MASTER_CONFIG, 0x4C);             // ���ڵ�һ����������ͨѶ �������� I2C����ʹ��

    // �ȴ�ͨѶ�ɹ�
    while(0 == (0x01 & imu963ra_read_acc_gyro_register(IMU963RA_STATUS_MASTER)))
    {
        if(timeout_count ++ > IMU963RA_TIMEOUT_COUNT)
        {
            break;
        }
        system_delay_ms(2);
    }

    return (imu963ra_read_acc_gyro_register(IMU963RA_SENSOR_HUB_1));            // ���ض�ȡ��������
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA ��Ϊ IIC ������������Զ�д����
// ����˵��     addr            Ŀ���ַ
// ����˵��     reg             Ŀ��Ĵ���
// ���ز���     void
// ʹ��ʾ��     imu963ra_connect_mag(IMU963RA_MAG_ADDR, IMU963RA_MAG_OUTX_L);
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static void imu963ra_connect_mag (uint8 addr, uint8 reg)
{
    addr = addr << 1;

    imu963ra_write_acc_gyro_register(IMU963RA_SLV0_ADD, addr | 1);              // ���õشżƵ�ַ��ע��������Ҫ����8λ��I2C��ַ�� 0x2C
    imu963ra_write_acc_gyro_register(IMU963RA_SLV0_SUBADD, reg);                // ��Ҫ��ȡ�ļĴ�����ַ
    imu963ra_write_acc_gyro_register(IMU963RA_SLV0_CONFIG, 0x06);
    imu963ra_write_acc_gyro_register(IMU963RA_MASTER_CONFIG, 0x6C);             // ���ڵ�һ����������ͨѶ �������� I2C����ʹ��
}


//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA �����Լ� �ڲ�����
// ����˵��     void
// ���ز���     uint8           1-�Լ�ʧ�� 0-�Լ�ɹ�
// ʹ��ʾ��     imu963ra_acc_gyro_self_check();
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu963ra_acc_gyro_self_check (void)
{
    uint8 return_state = 0;
    uint8 dat = 0;
    uint16 timeout_count = 0;

    while(0x6B != dat)                                                          // �ж� ID �Ƿ���ȷ
    {
        if(timeout_count++ > IMU963RA_TIMEOUT_COUNT)
        {
            return_state = 1;
            break;
        }
        dat = imu963ra_read_acc_gyro_register(IMU963RA_WHO_AM_I);
        system_delay_ms(10);
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IMU963RA �������Լ� �ڲ�����
// ����˵��     void
// ���ز���     uint8           1-�Լ�ʧ�� 0-�Լ�ɹ�
// ʹ��ʾ��     imu963ra_mag_self_check();
// ��ע��Ϣ     �ڲ�����
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu963ra_mag_self_check (void)
{
    uint8 return_state = 0;
    uint8 dat = 0;
    uint16 timeout_count = 0;

    while(0xff != dat)                                                          // �ж� ID �Ƿ���ȷ
    {
        if(timeout_count++ > IMU963RA_TIMEOUT_COUNT)
        {
            return_state = 1;
            break;
        }
        dat = imu963ra_read_mag_register(IMU963RA_MAG_ADDR, IMU963RA_MAG_CHIP_ID);
        system_delay_ms(10);
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ IMU963RA ���ٶȼ�����
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     imu963ra_get_acc();
// ��ע��Ϣ     ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
//-------------------------------------------------------------------------------------------------------------------
void imu963ra_get_acc (void)
{
    uint8 dat[6];

    imu963ra_read_acc_gyro_registers(IMU963RA_OUTX_L_A, dat, 6);
    imu963ra_acc_x = (int16)(((uint16)dat[1]<<8 | dat[0]));
    imu963ra_acc_y = (int16)(((uint16)dat[3]<<8 | dat[2]));
    imu963ra_acc_z = (int16)(((uint16)dat[5]<<8 | dat[4]));
}


//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ IMU963RA ����������
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     imu963ra_get_gyro();
// ��ע��Ϣ     ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
//-------------------------------------------------------------------------------------------------------------------
void imu963ra_get_gyro (void)
{
    uint8 dat[6];

    imu963ra_read_acc_gyro_registers(IMU963RA_OUTX_L_G, dat, 6);
    imu963ra_gyro_x = (int16)(((uint16)dat[1]<<8 | dat[0]));
    imu963ra_gyro_y = (int16)(((uint16)dat[3]<<8 | dat[2]));
    imu963ra_gyro_z = (int16)(((uint16)dat[5]<<8 | dat[4]));
}


//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ IMU963RA ����������
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     imu963ra_get_mag();
// ��ע��Ϣ     ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
//-------------------------------------------------------------------------------------------------------------------
void imu963ra_get_mag (void)
{
    uint8 temp_status;
    uint8 dat[6];

    imu963ra_write_acc_gyro_register(IMU963RA_FUNC_CFG_ACCESS, 0x40);
    temp_status = imu963ra_read_acc_gyro_register(IMU963RA_STATUS_MASTER);
    if(0x01 & temp_status)
    {
        imu963ra_read_acc_gyro_registers(IMU963RA_SENSOR_HUB_1, dat, 6);
        imu963ra_mag_x = (int16)(((uint16)dat[1]<<8 | dat[0]));
        imu963ra_mag_y = (int16)(((uint16)dat[3]<<8 | dat[2]));
        imu963ra_mag_z = (int16)(((uint16)dat[5]<<8 | dat[4]));
    }
    imu963ra_write_acc_gyro_register(IMU963RA_FUNC_CFG_ACCESS, 0x00);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �� IMU963RA ���ٶȼ�����ת��Ϊʵ����������
// ����˵��     gyro_value              // ������ļ��ٶȼ�����
// ���ز���     void
// ʹ��ʾ��     float data = imu963ra_acc_transition(imu963ra_acc_x);  //��λΪ g(m/s^2)
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
float imu963ra_acc_transition (int16 acc_value)
{
    float acc_data = 0;
    switch(IMU963RA_ACC_SAMPLE)
    {
        case 0x30: acc_data = (float)acc_value / 16393; break; // 0x30 ���ٶ�����Ϊ:��2G      ��ȡ���ļ��ٶȼ����� ����16393������ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        case 0x38: acc_data = (float)acc_value / 8197; break;  // 0x38 ���ٶ�����Ϊ:��4G      ��ȡ���ļ��ٶȼ����� ����8197�� ����ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        case 0x3C: acc_data = (float)acc_value / 4098; break;  // 0x3C ���ٶ�����Ϊ:��8G      ��ȡ���ļ��ٶȼ����� ����4098�� ����ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        case 0x34: acc_data = (float)acc_value / 2049; break;  // 0x34 ���ٶ�����Ϊ:��16G     ��ȡ���ļ��ٶȼ����� ����2049�� ����ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        default: break;
    }
    return acc_data;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �� IMU963RA ����������ת��Ϊʵ����������
// ����˵��     gyro_value              // �����������������
// ���ز���     void
// ʹ��ʾ��     float data = imu963ra_gyro_transition(imu963ra_gyro_x);  // ��λΪ��/s
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
float imu963ra_gyro_transition (int16 gyro_value)
{
    float gyro_data = 0;
    switch(IMU963RA_GYR_SAMPLE)
    {
        case 0x52: gyro_data = (float)gyro_value / 228.6f; break;  //  0x52 ����������Ϊ:��125dps  ��ȡ�������������ݳ���228.6��   ����ת��Ϊ������λ�����ݣ���λΪ����/s
        case 0x50: gyro_data = (float)gyro_value / 114.3f; break;  //  0x50 ����������Ϊ:��250dps  ��ȡ�������������ݳ���114.3��   ����ת��Ϊ������λ�����ݣ���λΪ����/s
        case 0x54: gyro_data = (float)gyro_value / 57.1f;  break;  //  0x54 ����������Ϊ:��500dps  ��ȡ�������������ݳ���57.1��    ����ת��Ϊ������λ�����ݣ���λΪ����/s
        case 0x58: gyro_data = (float)gyro_value / 28.6f;  break;  //  0x58 ����������Ϊ:��1000dps ��ȡ�������������ݳ���28.6��    ����ת��Ϊ������λ�����ݣ���λΪ����/s
        case 0x5C: gyro_data = (float)gyro_value / 14.3f;  break;  //  0x5C ����������Ϊ:��2000dps ��ȡ�������������ݳ���14.3��    ����ת��Ϊ������λ�����ݣ���λΪ����/s
        case 0x51: gyro_data = (float)gyro_value / 7.1f;   break;  //  0x51 ����������Ϊ:��4000dps ��ȡ�������������ݳ���7.1��     ����ת��Ϊ������λ�����ݣ���λΪ����/s
        default: break;
    }
    return gyro_data;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �� IMU963RA �شż�����ת��Ϊʵ����������
// ����˵��     gyro_value              // ������ĵشż�����
// ���ز���     void
// ʹ��ʾ��     float data = imu963ra_mag_transition(imu963ra_mag_x);  // ��λΪ��/s
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
float imu963ra_mag_transition (int16 mag_value)
{
    float mag_data = 0;
    switch(IMU963RA_MAG_SAMPLE)
    {
        case 0x19: mag_data = (float)mag_value / 3000;   break;  //  0x19 ����������Ϊ:8G     ��ȡ���ļ��ٶȼ����� ����3000�� ����ת��Ϊ������λ�����ݣ���λ��G(��˹)
        case 0x09: mag_data = (float)mag_value / 12000;  break;  //  0x09 ����������Ϊ:2G     ��ȡ���ļ��ٶȼ����� ����12000������ת��Ϊ������λ�����ݣ���λ��G(��˹)
        default: break;
    }
    return mag_data;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ʼ�� IMU963RA
// ����˵��     void
// ���ز���     uint8           1-��ʼ��ʧ�� 0-��ʼ���ɹ�
// ʹ��ʾ��     imu963ra_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 imu963ra_init (void)
{
    uint8 return_state = 0;
    system_delay_ms(10);                                                        // �ϵ���ʱ

#if IMU963RA_USE_SOFT_IIC
    soft_iic_init(&imu963ra_iic_struct, IMU963RA_DEV_ADDR, IMU963RA_SOFT_IIC_DELAY, IMU963RA_SCL_PIN, IMU963RA_SDA_PIN);
#else
    spi_init(IMU963RA_SPI, SPI_MODE0, IMU963RA_SPI_SPEED, IMU963RA_SPC_PIN, IMU963RA_SDI_PIN, IMU963RA_SDO_PIN, SPI_CS_NULL);
    gpio_init(IMU963RA_CS_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
#endif

    do
    {
        imu963ra_write_acc_gyro_register(IMU963RA_FUNC_CFG_ACCESS, 0x00);       // �ر�HUB�Ĵ�������
        imu963ra_write_acc_gyro_register(IMU963RA_CTRL3_C, 0x01);               // ��λ�豸
        system_delay_ms(2);
        imu963ra_write_acc_gyro_register(IMU963RA_FUNC_CFG_ACCESS, 0x00);       // �ر�HUB�Ĵ�������
        if(imu963ra_acc_gyro_self_check())
        {
            zf_log(0, "IMU963RA acc and gyro self check error.");
            return_state = 1;
            break;
        }

        imu963ra_write_acc_gyro_register(IMU963RA_INT1_CTRL, 0x03);             // ���������� ���ٶ����ݾ����ж�
        imu963ra_write_acc_gyro_register(IMU963RA_CTRL1_XL, IMU963RA_ACC_SAMPLE);              // ���ü��ٶȼ����̡�8G�Լ������������52hz �Լ����ٶ���Ϣ�ӵ�һ���˲������
        // IMU963RA_CTRL1_XL �Ĵ���
        // ����Ϊ:0x30 ���ٶ�����Ϊ:��2G      ��ȡ���ļ��ٶȼ����� ����16393������ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        // ����Ϊ:0x38 ���ٶ�����Ϊ:��4G      ��ȡ���ļ��ٶȼ����� ����8197�� ����ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        // ����Ϊ:0x3C ���ٶ�����Ϊ:��8G      ��ȡ���ļ��ٶȼ����� ����4098�� ����ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        // ����Ϊ:0x34 ���ٶ�����Ϊ:��16G     ��ȡ���ļ��ٶȼ����� ����2049�� ����ת��Ϊ������λ�����ݣ���λ��g(m/s^2)

        imu963ra_write_acc_gyro_register(IMU963RA_CTRL2_G, IMU963RA_GYR_SAMPLE);               // ���������Ǽ����� ��2000dps �Լ������������ 208hz
        // IMU963RA_CTRL2_G �Ĵ���
        // ����Ϊ:0x52 ����������Ϊ:��125dps  ��ȡ�������������ݳ���228.6��   ����ת��Ϊ������λ�����ݣ���λΪ����/s
        // ����Ϊ:0x50 ����������Ϊ:��250dps  ��ȡ�������������ݳ���114.3��   ����ת��Ϊ������λ�����ݣ���λΪ����/s
        // ����Ϊ:0x54 ����������Ϊ:��500dps  ��ȡ�������������ݳ���57.1��    ����ת��Ϊ������λ�����ݣ���λΪ����/s
        // ����Ϊ:0x58 ����������Ϊ:��1000dps ��ȡ�������������ݳ���28.6��    ����ת��Ϊ������λ�����ݣ���λΪ����/s
        // ����Ϊ:0x5C ����������Ϊ:��2000dps ��ȡ�������������ݳ���14.3��    ����ת��Ϊ������λ�����ݣ���λΪ����/s
        // ����Ϊ:0x51 ����������Ϊ:��4000dps ��ȡ�������������ݳ���7.1��     ����ת��Ϊ������λ�����ݣ���λΪ����/s

        imu963ra_write_acc_gyro_register(IMU963RA_CTRL3_C, 0x44);               // ʹ�����������ֵ�ͨ�˲���
        imu963ra_write_acc_gyro_register(IMU963RA_CTRL4_C, 0x02);               // ʹ�����ֵ�ͨ�˲���
        imu963ra_write_acc_gyro_register(IMU963RA_CTRL5_C, 0x00);               // ���ٶȼ�����������������
        imu963ra_write_acc_gyro_register(IMU963RA_CTRL6_C, 0x00);               // �������ٶȼƸ�����ģʽ �����ǵ�ͨ�˲� 133hz
        imu963ra_write_acc_gyro_register(IMU963RA_CTRL7_G, 0x00);               // ���������Ǹ�����ģʽ �رո�ͨ�˲�
        imu963ra_write_acc_gyro_register(IMU963RA_CTRL9_XL, 0x01);              // �ر�I3C�ӿ�

        imu963ra_write_acc_gyro_register(IMU963RA_FUNC_CFG_ACCESS, 0x40);       // ����HUB�Ĵ������� �������õشż�
        imu963ra_write_acc_gyro_register(IMU963RA_MASTER_CONFIG, 0x80);         // ��λI2C����
        system_delay_ms(2);
        imu963ra_write_acc_gyro_register(IMU963RA_MASTER_CONFIG, 0x00);         // �����λ��־
        system_delay_ms(2);

        imu963ra_write_mag_register(IMU963RA_MAG_ADDR, IMU963RA_MAG_CONTROL2, 0x80);    // ��λ���ӵ�����
        system_delay_ms(2);
        imu963ra_write_mag_register(IMU963RA_MAG_ADDR, IMU963RA_MAG_CONTROL2, 0x00);
        system_delay_ms(2);


        if(imu963ra_mag_self_check())
        {
            zf_log(0, "IMU963RA mag self check error.");
            return_state = 1;
            break;
        }

        imu963ra_write_mag_register(IMU963RA_MAG_ADDR, IMU963RA_MAG_CONTROL1, IMU963RA_MAG_SAMPLE);    // ���ô���������8G �������100hz ����ģʽ
        // IMU963RA_MAG_ADDR �Ĵ���
        // ����Ϊ:0x19 ����������Ϊ:8G     ��ȡ���ļ��ٶȼ����� ����3000�� ����ת��Ϊ������λ�����ݣ���λ��G(��˹)
        // ����Ϊ:0x09 ����������Ϊ:2G     ��ȡ���ļ��ٶȼ����� ����12000������ת��Ϊ������λ�����ݣ���λ��G(��˹)

        imu963ra_write_mag_register(IMU963RA_MAG_ADDR, IMU963RA_MAG_FBR, 0x01);
        imu963ra_connect_mag(IMU963RA_MAG_ADDR, IMU963RA_MAG_OUTX_L);

        imu963ra_write_acc_gyro_register(IMU963RA_FUNC_CFG_ACCESS, 0x00);       // �ر�HUB�Ĵ�������

        system_delay_ms(20);                                                    // �ȴ������ƻ�ȡ����
    }while(0);
    return return_state;
}
