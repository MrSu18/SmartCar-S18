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
* �ļ�����          zf_device_dl1a
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
*                   ------------------------------------
*                   ģ��ܽ�            ��Ƭ���ܽ�
*                   SCL                 �鿴 zf_device_dl1a.h �� DL1A_SCL_PIN  �궨��
*                   SDA                 �鿴 zf_device_dl1a.h �� DL1A_SDA_PIN  �궨��
*                   VCC                 5V ��Դ
*                   GND                 ��Դ��
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_driver_delay.h"
#include "zf_driver_soft_iic.h"
#include "zf_device_dl1a.h"

uint8 dl1a_finsh_flag;
uint16 dl1a_distance_mm;

#if DL1A_USE_SOFT_IIC
static soft_iic_info_struct dl1a_iic_struct;

#define dl1a_write_array(data, len)          (soft_iic_write_8bit_array(&dl1a_iic_struct, (data), (len)))
#define dl1a_write_register(reg, data)       (soft_iic_write_8bit_register(&dl1a_iic_struct, (reg), (data)))
#define dl1a_read_register(reg)              (soft_iic_read_8bit_register(&dl1a_iic_struct, (reg)))
#define dl1a_read_registers(reg, data, len)  (soft_iic_read_8bit_registers(&dl1a_iic_struct, (reg), (data), (len)))
#else
#define dl1a_write_array(data, len)          (iic_write_8bit_array(DL1A_IIC, DL1A_DEV_ADDR, (data), (len)))
#define dl1a_write_register(reg, data)       (iic_write_8bit_register(DL1A_IIC, DL1A_DEV_ADDR, (reg), (data)))
#define dl1a_read_register(reg)              (iic_read_8bit_register(DL1A_IIC, DL1A_DEV_ADDR, (reg)))
#define dl1a_read_registers(reg, data, len)  (iic_read_8bit_registers(DL1A_IIC, DL1A_DEV_ADDR, (reg), (data), (len)))
#endif

// ������ʱ�ʾ��Ŀ�귴�䲢���豸��⵽���źŵ����
// ���ô����ƿ���ȷ��������������Ч�����������С����ֵ
// ����һ���ϵ͵����ƿ������Ӵ������Ĳ�����Χ
// ���ƺ�Ҳ������ <��������Ŀ�����������Ĳ���Ҫ�ķ��䵼��> �õ���׼ȷ�����Ŀ�����
// Ĭ��Ϊ 0.25 MCPS ��Ԥ�跶ΧΪ 0 - 511.99
#define DL1A_DEFAULT_RATE_LIMIT  (0.25)

// �ӼĴ������ݽ��� PCLKs �� VCSEL (vertical cavity surface emitting laser) ����������
#define decode_vcsel_period(reg_val)            (((reg_val) + 1) << 1)

// �� PCLK �е� VCSEL ���ڼ�������� (�� *����Ϊ��λ)
// PLL_period_ps = 1655
// macro_period_vclks = 2304
#define calc_macro_period(vcsel_period_pclks)   ((((uint32)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ�豸 SPAD ��Ϣ
// ����˵��     index           ����
// ����˵��     type            ����ֵ
// ���ز���     uint8           �Ƿ�ɹ� 0-�ɹ� 1-ʧ��
// ʹ��ʾ��     dl1a_get_spad_info(index, type_is_aperture);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
static uint8 dl1a_get_spad_info (uint8 *index, uint8 *type_is_aperture)
{
    uint8 tmp = 0;
    uint8 return_state = 0;
    volatile uint16 loop_count = 0;

    do
    {
        dl1a_write_register(0x80, 0x01);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x00, 0x00);

        dl1a_write_register(0xFF, 0x06);
        dl1a_read_registers(0x83, &tmp, 1);
        dl1a_write_register(0x83, tmp | 0x04);
        dl1a_write_register(0xFF, 0x07);
        dl1a_write_register(0x81, 0x01);

        dl1a_write_register(0x80, 0x01);

        dl1a_write_register(0x94, 0x6b);
        dl1a_write_register(0x83, 0x00);

        tmp = 0x00;
        while(tmp == 0x00 || tmp == 0xFF)
        {
            system_delay_ms(1);
            dl1a_read_registers(0x83, &tmp, 1);
            if(loop_count++ > DL1A_TIMEOUT_COUNT)
            {
                return_state = 1;
                break;
            }

        }
        if(return_state)
        {
            break;
        }
        dl1a_write_register(0x83, 0x01);
        dl1a_read_registers(0x92, &tmp, 1);

        *index = tmp & 0x7f;
        *type_is_aperture = (tmp >> 7) & 0x01;

        dl1a_write_register(0x81, 0x00);
        dl1a_write_register(0xFF, 0x06);
        dl1a_read_registers(0x83, &tmp, 1);
        dl1a_write_register(0x83, tmp);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x00, 0x01);

        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x80, 0x00);
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ʱ��ֵ�� MCLKs ת������Ӧ�� ms
// ����˵��     timeout_period_mclks    ��ʱ���� MCLKs
// ����˵��     vcsel_period_pclks      PCLK ֵ
// ���ز���     uint32                  ���س�ʱ��ֵ
// ʹ��ʾ��     dl1a_timeout_mclks_to_microseconds(timeout_period_mclks, vcsel_period_pclks);
// ��ע��Ϣ     �����в��賬ʱ�Ӿ��и��� VCSEL ���ڵ� MCLK (�� PCLK Ϊ��λ)ת��Ϊ΢��
//-------------------------------------------------------------------------------------------------------------------
static uint32 dl1a_timeout_mclks_to_microseconds (uint16 timeout_period_mclks, uint8 vcsel_period_pclks)
{
    uint32 macro_period_ns = calc_macro_period(vcsel_period_pclks);

    return ((timeout_period_mclks * macro_period_ns) + (macro_period_ns / 2)) / 1000;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ����ʱ��ֵ�� ms ת������Ӧ�� MCLKs
// ����˵��     timeout_period_us   ��ʱ���� ΢�뵥λ
// ����˵��     vcsel_period_pclks  PCLK ֵ
// ���ز���     uint32              ���س�ʱ��ֵ
// ʹ��ʾ��     dl1a_timeout_microseconds_to_mclks(timeout_period_us, vcsel_period_pclks);
// ��ע��Ϣ     �����в��賬ʱ��΢��ת��Ϊ���и��� VCSEL ���ڵ� MCLK (�� PCLK Ϊ��λ)
//-------------------------------------------------------------------------------------------------------------------
static uint32 dl1a_timeout_microseconds_to_mclks (uint32 timeout_period_us, uint8 vcsel_period_pclks)
{
    uint32 macro_period_ns = calc_macro_period(vcsel_period_pclks);

    return (((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �Գ�ʱ��ֵ���н���
// ����˵��     reg_val         ��ʱʱ�� �Ĵ���ֵ
// ���ز���     uint16          ���س�ʱ��ֵ
// ʹ��ʾ��     dl1a_decode_timeout(reg_val);
// ��ע��Ϣ     �ӼĴ���ֵ���� MCLK �е����в��賬ʱ
//-------------------------------------------------------------------------------------------------------------------
static uint16 dl1a_decode_timeout (uint16 reg_val)
{
  // ��ʽ: (LSByte * 2 ^ MSByte) + 1
    return  (uint16)((reg_val & 0x00FF) <<
            (uint16)((reg_val & 0xFF00) >> 8)) + 1;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �Գ�ʱ��ֵ���б���
// ����˵��     timeout_mclks   ��ʱʱ�� -MCLKs ֵ
// ���ز���     uint16          ���ر���ֵ
// ʹ��ʾ��     dl1a_encode_timeout(timeout_mclks);
// ��ע��Ϣ     �� MCLK �жԳ�ʱ�����в��賬ʱ�Ĵ���ֵ���б���
//-------------------------------------------------------------------------------------------------------------------
static uint16 dl1a_encode_timeout (uint16 timeout_mclks)
{
    uint32 ls_byte = 0;
    uint16 ms_byte = 0;
    uint16 return_data = 0;

    if (timeout_mclks > 0)
    {
        // ��ʽ: (LSByte * 2 ^ MSByte) + 1
        ls_byte = timeout_mclks - 1;
        while ((ls_byte & 0xFFFFFF00) > 0)
        {
            ls_byte >>= 1;
            ms_byte++;
        }
        return_data = (ms_byte << 8) | ((uint16)ls_byte & 0xFF);
    }
    return return_data;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ���в���ʹ������
// ����˵��     enables         ����ʹ�ܲ���ṹ��
// ���ز���     void
// ʹ��ʾ��     dl1a_get_sequence_step_enables(enables);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
static void dl1a_get_sequence_step_enables(dl1a_sequence_enables_step_struct *enables)
{
    uint8 sequence_config = 0;
    dl1a_read_registers(DL1A_SYSTEM_SEQUENCE_CONFIG, &sequence_config, 1);

    enables->tcc          = (sequence_config >> 4) & 0x1;
    enables->dss          = (sequence_config >> 3) & 0x1;
    enables->msrc         = (sequence_config >> 2) & 0x1;
    enables->pre_range    = (sequence_config >> 6) & 0x1;
    enables->final_range  = (sequence_config >> 7) & 0x1;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ��������
// ����˵��     type            Ԥ��������
// ���ز���     uint8           ���ص�����ֵ
// ʹ��ʾ��     dl1a_get_vcsel_pulse_period(DL1A_VCSEL_PERIOD_PER_RANGE);
// ��ע��Ϣ     �� PCLKs �л�ȡ�����������͵� VCSEL ��������
//-------------------------------------------------------------------------------------------------------------------
static uint8 dl1a_get_vcsel_pulse_period (dl1a_vcsel_period_type_enum type)
{
    uint8 data_buffer = 0;
    if (type == DL1A_VCSEL_PERIOD_PER_RANGE)
    {
        dl1a_read_registers(DL1A_PRE_RANGE_CONFIG_VCSEL_PERIOD, &data_buffer, 1);
        data_buffer = decode_vcsel_period(data_buffer);
    }
    else if (type == DL1A_VCSEL_PERIOD_FINAL_RANGE)
    {
        dl1a_read_registers(DL1A_FINAL_RANGE_CONFIG_VCSEL_PERIOD, &data_buffer, 1);
        data_buffer = decode_vcsel_period(data_buffer);
    }
    else
    {
        data_buffer = 255;
    }
    return data_buffer;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ���в��賬ʱ����
// ����˵��     enables         ����ʹ�ܲ���ṹ��
// ����˵��     timeouts        ���г�ʱ����ṹ��
// ���ز���     void
// ʹ��ʾ��     dl1a_get_sequence_step_timeouts(enables, timeouts);
// ��ע��Ϣ     ��ȡ���г�ʱ��������������ĳ�ʱ ���һ��洢�м�ֵ
//-------------------------------------------------------------------------------------------------------------------
static void dl1a_get_sequence_step_timeouts (dl1a_sequence_enables_step_struct const *enables, dl1a_sequence_timeout_step_struct *timeouts)
{
    uint8 reg_buffer[2];
    uint16 reg16_buffer = 0;

    timeouts->pre_range_vcsel_period_pclks = dl1a_get_vcsel_pulse_period(DL1A_VCSEL_PERIOD_PER_RANGE);

    dl1a_read_registers(DL1A_MSRC_CONFIG_TIMEOUT_MACROP, reg_buffer, 1);
    timeouts->msrc_dss_tcc_mclks = reg_buffer[0] + 1;
    timeouts->msrc_dss_tcc_us = dl1a_timeout_mclks_to_microseconds(timeouts->msrc_dss_tcc_mclks, (uint8)timeouts->pre_range_vcsel_period_pclks);

    dl1a_read_registers(DL1A_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI, reg_buffer, 2);
    reg16_buffer = ((uint16) reg_buffer[0] << 8) | reg_buffer[1];
    timeouts->pre_range_mclks = dl1a_decode_timeout(reg16_buffer);
    timeouts->pre_range_us = dl1a_timeout_mclks_to_microseconds(timeouts->pre_range_mclks, (uint8)timeouts->pre_range_vcsel_period_pclks);

    timeouts->final_range_vcsel_period_pclks = dl1a_get_vcsel_pulse_period(DL1A_VCSEL_PERIOD_FINAL_RANGE);

    dl1a_read_registers(DL1A_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI, reg_buffer, 2);
    reg16_buffer = ((uint16) reg_buffer[0] << 8) | reg_buffer[1];
    timeouts->final_range_mclks = dl1a_decode_timeout(reg16_buffer);

    if (enables->pre_range)
    {
        timeouts->final_range_mclks -= timeouts->pre_range_mclks;
    }

    timeouts->final_range_us = dl1a_timeout_mclks_to_microseconds(timeouts->final_range_mclks, (uint8)timeouts->final_range_vcsel_period_pclks);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ִ�е��βο�У׼
// ����˵��     vhv_init_byte   Ԥ��У׼ֵ
// ���ز���     uint8           �����Ƿ�ɹ� 0-�ɹ� 1-ʧ��
// ʹ��ʾ��     dl1a_get_vcsel_pulse_period(DL1A_VCSEL_PERIOD_PER_RANGE);
// ��ע��Ϣ     �� PCLKs �л�ȡ�����������͵� VCSEL ��������
//-------------------------------------------------------------------------------------------------------------------
static uint8 dl1a_perform_single_ref_calibration (uint8 vhv_init_byte)
{
    uint8 return_state = 0;
    uint8 data_buffer = 0;
    volatile uint16 loop_count = 0;
    do
    {
        dl1a_write_register(DL1A_SYSRANGE_START, 0x01 | vhv_init_byte);
        dl1a_read_registers(DL1A_MSRC_CONFIG_TIMEOUT_MACROP, &data_buffer, 1);
        while ((data_buffer & 0x07) == 0)
        {
            system_delay_ms(1);
            dl1a_read_registers(DL1A_MSRC_CONFIG_TIMEOUT_MACROP, &data_buffer, 1);
            if (loop_count ++ > DL1A_TIMEOUT_COUNT)
            {
                return_state = 1;
                break;
            }
        }
        if(return_state)
        {
            break;
        }
        dl1a_write_register(DL1A_SYSTEM_INTERRUPT_CLEAR, 0x01);
        dl1a_write_register(DL1A_SYSRANGE_START, 0x00);
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ���ò�����ʱԤ�� (��΢��Ϊ��λ)
// ����˵��     budget_us       �趨�Ĳ��������ʱ��
// ���ز���     uint8           ������� 0-�ɹ� 1-ʧ��
// ʹ��ʾ��     dl1a_set_measurement_timing_budget(measurement_timing_budget_us);
// ��ע��Ϣ     ����һ�β��������ʱ��
//              ���ڲ�����е��Ӳ���֮�����ʱ��Ԥ��
//              ������ʱ��Ԥ���������ȷ�Ĳ���
//              ����һ��N����Ԥ����Լ���һ��sqrt(N)���ķ�Χ������׼ƫ��
//              Ĭ��Ϊ33���� ��СֵΪ20 ms
//-------------------------------------------------------------------------------------------------------------------
static uint8 dl1a_set_measurement_timing_budget (uint32 budget_us)
{
    uint8 return_state = 0;
    uint8 data_buffer[3];
    uint16 data = 0;

    dl1a_sequence_enables_step_struct enables;
    dl1a_sequence_timeout_step_struct timeouts;

    do
    {
        if (budget_us < DL1A_MIN_TIMING_BUDGET)
        {
            return_state = 1;
            break;
        }

        uint32 used_budget_us = DL1A_SET_START_OVERHEAD + DL1A_END_OVERHEAD;
        dl1a_get_sequence_step_enables(&enables);
        dl1a_get_sequence_step_timeouts(&enables, &timeouts);

        if (enables.tcc)
        {
            used_budget_us += (timeouts.msrc_dss_tcc_us + DL1A_TCC_OVERHEAD);
        }

        if (enables.dss)
        {
            used_budget_us += 2 * (timeouts.msrc_dss_tcc_us + DL1A_DSS_OVERHEAD);
        }
        else if (enables.msrc)
        {
            used_budget_us += (timeouts.msrc_dss_tcc_us + DL1A_MSRC_OVERHEAD);
        }

        if (enables.pre_range)
        {
            used_budget_us += (timeouts.pre_range_us + DL1A_PRERANGE_OVERHEAD);
        }

        if (enables.final_range)
        {
            // ��ע�� ���շ�Χ��ʱ�ɼ�ʱԤ�������������������ʱ���ܺ;���
            // ���û�пռ��������շ�Χ��ʱ �����ô���
            // ���� ʣ��ʱ�佫Ӧ�������շ�Χ
            used_budget_us += DL1A_FINALlRANGE_OVERHEAD;
            if (used_budget_us > budget_us)
            {
                // ����ĳ�ʱ̫��
                return_state = 1;
                break;
            }

            // �������ճ�ʱ��Χ �������Ԥ���̷�Χ��ʱ
            // Ϊ�� ���ճ�ʱ��Ԥ���̳�ʱ�����Ժ����� MClks ��ʾ
            // ��Ϊ���Ǿ��в�ͬ�� VCSEL ����
            uint32 final_range_timeout_us = budget_us - used_budget_us;
            uint16 final_range_timeout_mclks =
            (uint16)dl1a_timeout_microseconds_to_mclks(final_range_timeout_us,
                     (uint8)timeouts.final_range_vcsel_period_pclks);

            if (enables.pre_range)
            {
                final_range_timeout_mclks += timeouts.pre_range_mclks;
            }

            data = dl1a_encode_timeout(final_range_timeout_mclks);
            data_buffer[0] = DL1A_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI;
            data_buffer[1] = ((data >> 8) & 0xFF);
            data_buffer[2] = (data & 0xFF);
            dl1a_write_array(data_buffer, 3);
        }
    }while(0);
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ȡ������ʱԤ�� (��΢��Ϊ��λ)
// ����˵��     void
// ���ز���     uint32          ���趨�Ĳ��������ʱ��
// ʹ��ʾ��     dl1a_get_measurement_timing_budget();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
static uint32 dl1a_get_measurement_timing_budget (void)
{
    dl1a_sequence_enables_step_struct enables;
    dl1a_sequence_timeout_step_struct timeouts;

    // ��ʼ�ͽ�������ʱ��ʼ�մ���
    uint32 budget_us = DL1A_GET_START_OVERHEAD + DL1A_END_OVERHEAD;

    dl1a_get_sequence_step_enables(&enables);
    dl1a_get_sequence_step_timeouts(&enables, &timeouts);

    if (enables.tcc)
    {
        budget_us += (timeouts.msrc_dss_tcc_us + DL1A_TCC_OVERHEAD);
    }

    if (enables.dss)
    {
        budget_us += 2 * (timeouts.msrc_dss_tcc_us + DL1A_DSS_OVERHEAD);
    }
    else if (enables.msrc)
    {
        budget_us += (timeouts.msrc_dss_tcc_us + DL1A_MSRC_OVERHEAD);
    }

    if (enables.pre_range)
    {
        budget_us += (timeouts.pre_range_us + DL1A_PRERANGE_OVERHEAD);
    }

    if (enables.final_range)
    {
        budget_us += (timeouts.final_range_us + DL1A_FINALlRANGE_OVERHEAD);
    }

    return budget_us;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ���÷����ź��������� ��ֵ��λΪ MCPS (�����ÿ��)
// ����˵��     limit_mcps      ���õ���С����
// ���ز���     void
// ʹ��ʾ��     dl1a_set_signal_rate_limit(0.25);
// ��ע��Ϣ     ������ʱ�ʾ��Ŀ�귴�䲢���豸��⵽���źŵ����
//              ���ô����ƿ���ȷ��������������Ч�����������С����ֵ
//              ����һ���ϵ͵����ƿ������Ӵ������Ĳ�����Χ
//              ���ƺ�Ҳ������ <��������Ŀ�����������Ĳ���Ҫ�ķ��䵼��> �õ���׼ȷ�����Ŀ�����
//              Ĭ��Ϊ 0.25 MCPS ��Ԥ�跶ΧΪ 0 - 511.99
//-------------------------------------------------------------------------------------------------------------------
static void dl1a_set_signal_rate_limit (float limit_mcps)
{
    zf_assert(limit_mcps >= 0 || limit_mcps <= 511.99);
    uint8 data_buffer[3];
    uint16 limit_mcps_16bit = (limit_mcps * (1 << 7));

    data_buffer[0] = DL1A_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT;
    data_buffer[1] = ((limit_mcps_16bit >> 8) & 0xFF);
    data_buffer[2] = (limit_mcps_16bit & 0xFF);

    dl1a_write_array(data_buffer, 3);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �����Ժ���Ϊ��λ�ķ�Χ����
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     dl1a_get_distance();
// ��ע��Ϣ     �ڿ�ʼ������̲�����Ҳ���ô˺���
//-------------------------------------------------------------------------------------------------------------------
void dl1a_get_distance (void)
{
    uint8 reg_databuffer[3];

    dl1a_read_registers(DL1A_RESULT_INTERRUPT_STATUS, reg_databuffer, 1);
    if((reg_databuffer[0] & 0x07) != 0)
    {
        // �������Զ�У������ΪĬ��ֵ 1000 ��δ���÷�����Χ
        dl1a_read_registers(DL1A_RESULT_RANGE_STATUS + 10, reg_databuffer, 2);
        dl1a_distance_mm = ((uint16_t)reg_databuffer[0] << 8);
        dl1a_distance_mm |= reg_databuffer[1];

        dl1a_write_register(DL1A_SYSTEM_INTERRUPT_CLEAR, 0x01);
        dl1a_finsh_flag = 1;
    }
    if(reg_databuffer[0] & 0x10)
    {
        dl1a_read_registers(DL1A_RESULT_RANGE_STATUS + 10, reg_databuffer, 2);
        dl1a_write_register(DL1A_SYSTEM_INTERRUPT_CLEAR, 0x01);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��ʼ�� DL1A
// ����˵��     void
// ���ز���     uint8           1-��ʼ��ʧ�� 0-��ʼ���ɹ�
// ʹ��ʾ��     dl1a_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint8 dl1a_init (void)
{
    uint32 measurement_timing_budget_us;
    uint8 stop_variable;
    uint8 return_state = 0;
    uint8 reg_data_buffer ;
    uint8 ref_spad_map[6];
    uint8 data_buffer[7];

    memset(ref_spad_map, 0, 6);
    memset(data_buffer, 0, 7);

#if DL1A_USE_SOFT_IIC
    soft_iic_init(&dl1a_iic_struct, DL1A_DEV_ADDR, DL1A_SOFT_IIC_DELAY, DL1A_SCL_PIN, DL1A_SDA_PIN);
#else
    iic_init(DL1A_IIC, DL1A_DEV_ADDR, DL1A_IIC_SPEED, DL1A_SCL_PIN, DL1A_SDA_PIN);
#endif
    gpio_init(DL1A_XSHUT_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    do
    {
        system_delay_ms(100);
        gpio_low(DL1A_XSHUT_PIN);
        system_delay_ms(50);
        gpio_high(DL1A_XSHUT_PIN);
        system_delay_ms(100);

        // -------------------------------- DL1A ������ʼ�� --------------------------------
        reg_data_buffer = dl1a_read_register(DL1A_IO_VOLTAGE_CONFIG);         // ������Ĭ�� IO Ϊ 1.8V ģʽ
        dl1a_write_register(DL1A_IO_VOLTAGE_CONFIG, reg_data_buffer | 0x01);  // ���� IO Ϊ 2.8V ģʽ

        dl1a_write_register(0x88, 0x00);                                         // ����Ϊ��׼ IIC ģʽ

        dl1a_write_register(0x80, 0x01);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x00, 0x00);

        dl1a_read_registers(0x91, &stop_variable , 1);

        dl1a_write_register(0x00, 0x01);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x80, 0x00);

        // ���� SIGNAL_RATE_MSRC(bit1) �� SIGNAL_RATE_PRE_RANGE(bit4) ���Ƽ��
        reg_data_buffer = dl1a_read_register(DL1A_MSRC_CONFIG);
        dl1a_write_register(DL1A_MSRC_CONFIG, reg_data_buffer | 0x12);

        dl1a_set_signal_rate_limit(DL1A_DEFAULT_RATE_LIMIT);                  // �����ź���������
        dl1a_write_register(DL1A_SYSTEM_SEQUENCE_CONFIG, 0xFF);
        // -------------------------------- DL1A ������ʼ�� --------------------------------

        // -------------------------------- DL1A ���ó�ʼ�� --------------------------------
        if (dl1a_get_spad_info(&data_buffer[0], &data_buffer[1]))
        {
            return_state = 1;
            zf_log(0, "DL1A self check error.");
            break;
        }

        // �� GLOBAL_CONFIG_SPAD_ENABLES_REF_[0-6] ��ȡ SPAD map (RefGoodSpadMap) ����
        dl1a_read_registers(DL1A_GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(DL1A_DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
        dl1a_write_register(DL1A_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(DL1A_GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

        data_buffer[2] = data_buffer[1] ? 12 : 0; // 12 is the first aperture spad
        for (uint8 i = 0; i < 48; i++)
        {
            if (i < data_buffer[2] || data_buffer[3] == data_buffer[0])
            {
                // ��λ����Ӧ���õĵ�һ��λ
                // ���� (eference_spad_count) λ������
                // ��˴�λΪ��
                ref_spad_map[i / 8] &= ~(1 << (i % 8));
            }
            else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1)
            {
                data_buffer[3]++;
            }
        }

        data_buffer[0] = DL1A_GLOBAL_CONFIG_SPAD_ENABLES_REF_0;
        for(uint8 i = 1; i < 7; i++)
        {
            data_buffer[1] = ref_spad_map[i-1];
        }
        dl1a_write_array(data_buffer, 7);

        // Ĭ��ת������ version 02/11/2015_v36
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x00, 0x00);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x09, 0x00);
        dl1a_write_register(0x10, 0x00);
        dl1a_write_register(0x11, 0x00);
        dl1a_write_register(0x24, 0x01);
        dl1a_write_register(0x25, 0xFF);
        dl1a_write_register(0x75, 0x00);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x4E, 0x2C);
        dl1a_write_register(0x48, 0x00);
        dl1a_write_register(0x30, 0x20);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x30, 0x09);
        dl1a_write_register(0x54, 0x00);
        dl1a_write_register(0x31, 0x04);
        dl1a_write_register(0x32, 0x03);
        dl1a_write_register(0x40, 0x83);
        dl1a_write_register(0x46, 0x25);
        dl1a_write_register(0x60, 0x00);
        dl1a_write_register(0x27, 0x00);
        dl1a_write_register(0x50, 0x06);
        dl1a_write_register(0x51, 0x00);
        dl1a_write_register(0x52, 0x96);
        dl1a_write_register(0x56, 0x08);
        dl1a_write_register(0x57, 0x30);
        dl1a_write_register(0x61, 0x00);
        dl1a_write_register(0x62, 0x00);
        dl1a_write_register(0x64, 0x00);
        dl1a_write_register(0x65, 0x00);
        dl1a_write_register(0x66, 0xA0);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x22, 0x32);
        dl1a_write_register(0x47, 0x14);
        dl1a_write_register(0x49, 0xFF);
        dl1a_write_register(0x4A, 0x00);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x7A, 0x0A);
        dl1a_write_register(0x7B, 0x00);
        dl1a_write_register(0x78, 0x21);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x23, 0x34);
        dl1a_write_register(0x42, 0x00);
        dl1a_write_register(0x44, 0xFF);
        dl1a_write_register(0x45, 0x26);
        dl1a_write_register(0x46, 0x05);
        dl1a_write_register(0x40, 0x40);
        dl1a_write_register(0x0E, 0x06);
        dl1a_write_register(0x20, 0x1A);
        dl1a_write_register(0x43, 0x40);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x34, 0x03);
        dl1a_write_register(0x35, 0x44);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x31, 0x04);
        dl1a_write_register(0x4B, 0x09);
        dl1a_write_register(0x4C, 0x05);
        dl1a_write_register(0x4D, 0x04);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x44, 0x00);
        dl1a_write_register(0x45, 0x20);
        dl1a_write_register(0x47, 0x08);
        dl1a_write_register(0x48, 0x28);
        dl1a_write_register(0x67, 0x00);
        dl1a_write_register(0x70, 0x04);
        dl1a_write_register(0x71, 0x01);
        dl1a_write_register(0x72, 0xFE);
        dl1a_write_register(0x76, 0x00);
        dl1a_write_register(0x77, 0x00);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x0D, 0x01);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x80, 0x01);
        dl1a_write_register(0x01, 0xF8);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x8E, 0x01);
        dl1a_write_register(0x00, 0x01);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x80, 0x00);

        // ���ж���������Ϊ����Ʒ����
        dl1a_write_register(DL1A_SYSTEM_INTERRUPT_GPIO_CONFIG, 0x04);
        reg_data_buffer = dl1a_read_register(DL1A_GPIO_HV_MUX_ACTIVE_HIGH);
        dl1a_write_register(DL1A_GPIO_HV_MUX_ACTIVE_HIGH, reg_data_buffer & ~0x10);
        dl1a_write_register(DL1A_SYSTEM_INTERRUPT_CLEAR, 0x01);

        measurement_timing_budget_us  = dl1a_get_measurement_timing_budget();

        // Ĭ������½��� MSRC �� TCC
        // MSRC = Minimum Signal Rate Check
        // TCC = Target CentreCheck
        dl1a_write_register(DL1A_SYSTEM_SEQUENCE_CONFIG, 0xE8);
        dl1a_set_measurement_timing_budget(measurement_timing_budget_us);    // ���¼���ʱ��Ԥ��
        // -------------------------------- DL1A ���ó�ʼ�� --------------------------------

        dl1a_write_register(DL1A_SYSTEM_SEQUENCE_CONFIG, 0x01);
        if (dl1a_perform_single_ref_calibration(0x40))
        {
            return_state = 1;
            zf_log(0, "DL1A perform single reference calibration error.");
            break;
        }
        dl1a_write_register(DL1A_SYSTEM_SEQUENCE_CONFIG, 0x02);
        if (dl1a_perform_single_ref_calibration(0x00))
        {
            return_state = 1;
            zf_log(0, "DL1A perform single reference calibration error.");
            break;
        }
        dl1a_write_register(DL1A_SYSTEM_SEQUENCE_CONFIG, 0xE8);           // �ָ���ǰ����������

        system_delay_ms(100);

        dl1a_write_register(0x80, 0x01);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x00, 0x00);
        dl1a_write_register(0x91, stop_variable);
        dl1a_write_register(0x00, 0x01);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x80, 0x00);

        dl1a_write_register(DL1A_SYSRANGE_START, 0x02);
    }while(0);

    return return_state;
}
