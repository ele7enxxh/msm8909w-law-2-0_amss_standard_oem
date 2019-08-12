/*	Date: 2015/11/10 14:00:00
 *	Revision: 1.4.2
 */






/*******************************************************************************
* Copyright (c) 2013, Bosch Sensortec GmbH
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Bosch Sensortec GmbH nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/*-----------------------------------------------------------------------------
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
  -----------------------------------------------------------------------------*/


/*==============================================================================

    S E N S O R S   P R E S S U R E   D R I V E R

DESCRIPTION

   Impelements the driver for the BMP280 driver


                      EDIT HISTORY FOR FILE


when         who     what, where, why
----------   ---     -----------------------------------------------------------
11/26/12     Albert   normal mode get pressure and temperature
03/15/13	Albert   self test
10/29/14	Albert    refactor to uimg driver
12/09/14 	Albert    added log packets
01/08/15	Albert    change resolution and max ODR according to latest datasheet.
06/25/15	  Albert	init for 20 bit resolution v1.3.4
07/02/15	 Albert     support bmp285 v1.3.5
08/10/15	 Albert     set ODR 10hz for 20bit v1.3.6
08/17/15	Albert      add data log v1.3.7
08/21/15	Albert       supported ODRs list attribute in get_attr(). to configure correct on-chip ODR as requested by SMGR v1.3.8


==============================================================================*/

#include "sns_dd_alt_bmp280.h"



/*!
 * @brief  Reported Temprature resolution is 0.01°C (Celsius).
 */
 const q16_t bmp280_temperature_resolution = FX_FLTTOFIX_Q16(0.01);

/*!
 * @brief Reported Pressure resolution is 1.0 Pa (Pascal) = 0.01 hPa.
 * Expected range 80K to 120K Pascals.
 */
 const q16_t bmp280_pressure_resolution = FX_FLTTOFIX_Q16(0.01);


extern sns_ddf_status_e sns_dd_alt_bmp280_init(
   sns_ddf_handle_t*		dd_handle_ptr,
   sns_ddf_handle_t 		smgr_handle,
   sns_ddf_nv_params_s* 	nv_params,
   sns_ddf_device_access_s	device_info[],
   uint32_t 				num_devices,
   sns_ddf_memhandler_s*	memhandler,
   sns_ddf_sensor_e*		sensors[],
   uint32_t*				num_sensors);


extern sns_ddf_status_e sns_dd_alt_bmp280_set_attr(
   sns_ddf_handle_t 	dd_handle,
   sns_ddf_sensor_e 	sensor,
   sns_ddf_attribute_e	attrib,
   void*				value);

extern sns_ddf_status_e sns_dd_alt_bmp280_get_attr(
   sns_ddf_handle_t 	  dd_handle,
   sns_ddf_sensor_e 	  sensor,
   sns_ddf_attribute_e	  attrib,
   sns_ddf_memhandler_s*  memhandler,
   void**				  value,
   uint32_t*			  num_elems);

extern sns_ddf_status_e sns_dd_alt_bmp280_reset(sns_ddf_handle_t dd_handle);
extern sns_ddf_status_e sns_dd_alt_bmp280_self_test(
               sns_ddf_handle_t dd_handle,
               sns_ddf_sensor_e sensor,
               sns_ddf_test_e test,
               uint32_t* err);
extern sns_ddf_status_e sns_dd_alt_bmp280_probe(
                        sns_ddf_device_access_s* device_info,
                        sns_ddf_memhandler_s*    memhandler,
                        uint32_t*                num_sensors,
                        sns_ddf_sensor_e**       sensors );

struct bmp280_t *p_bmp280;
sns_ddf_handle_t bmp280_port_handle;




 sns_ddf_status_e bmp_read_wrapper(uint8_t dev_addr, uint8_t reg_addr, uint8_t *bosch_read_data, uint8_t len)
{

    sns_ddf_status_e stat;
    uint8_t out;

    if ( (stat = sns_ddf_read_port(bmp280_port_handle,
                          reg_addr,
                          bosch_read_data,
                          len,
                          &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }
    return stat;
}

 sns_ddf_status_e bmp_write_wrapper(uint8_t dev_addr, uint8_t reg_addr, uint8_t *bosch_write_buffer, uint8_t len)
{

    sns_ddf_status_e stat;
    uint8_t out;

    if ( (stat = sns_ddf_write_port(bmp280_port_handle,
                         reg_addr,
                         bosch_write_buffer,
                         len,
                         &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }
    return stat;
}
 void bmp_delay(uint32_t msec)
{
    sns_ddf_delay(1000*msec);
}



BMP280_RETURN_FUNCTION_TYPE bmp280_read_ut(BMP280_S32_t *utemperature)
{
    BMP280_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char a_data_u8r[3] = {0};
    if (p_bmp280 == BMP280_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres += p_bmp280->BMP280_BUS_READ_FUNC(p_bmp280->dev_addr, \
            BMP280_TEMPERATURE_MSB_REG, a_data_u8r, 3);
        *utemperature = (BMP280_S32_t)((( \
        (BMP280_U32_t) (a_data_u8r[0])) << SHIFT_LEFT_12_POSITION) | \
        (((BMP280_U32_t)(a_data_u8r[1])) << SHIFT_LEFT_4_POSITION) \
        | ((BMP280_U32_t)a_data_u8r[2] >> SHIFT_RIGHT_4_POSITION));
    }
    return comres;
}

/*!
 * @brief  Reads actual temperature from uncompensed temperature
 *							Unit -> Degree Centigrade
 *
 *
 *
 * @param signed long : value of uncompensated temperature
 *
 *
 *
 *  @return signed long : actual temperature
 *
*/
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
    BMP280_S32_t v_x1_u32r = 0;
    BMP280_S32_t v_x2_u32r = 0;
    BMP280_S32_t temperature = 0;

    v_x1_u32r  = ((((adc_T >> 3) - ((BMP280_S32_t)
        p_bmp280->cal_param.dig_T1 << 1))) * \
        ((BMP280_S32_t)p_bmp280->cal_param.dig_T2)) >> 11;
    v_x2_u32r  = (((((adc_T >> 4) - \
        ((BMP280_S32_t)p_bmp280->cal_param.dig_T1)) * ((adc_T >> 4) - \
        ((BMP280_S32_t)p_bmp280->cal_param.dig_T1))) >> 12) * \
        ((BMP280_S32_t)p_bmp280->cal_param.dig_T3)) >> 14;
    p_bmp280->cal_param.t_fine = v_x1_u32r + v_x2_u32r;
    temperature  = (p_bmp280->cal_param.t_fine * 5 + 128) >> 8;

    return temperature;
}

/*!
 * @brief reads uncompensated pressure
 *
 *
 *
 *
 *  @param  unsigned long pressure : Pointer to the data
 *
 *
 *
 *  @return
 *
*/
BMP280_RETURN_FUNCTION_TYPE bmp280_read_up(BMP280_S32_t *upressure)
{
    BMP280_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char a_data_u8r[3] = {0};
    if (p_bmp280 == BMP280_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres += p_bmp280->BMP280_BUS_READ_FUNC(p_bmp280->dev_addr, \
        BMP280_PRESSURE_MSB_REG, a_data_u8r, 3);
        *upressure = (BMP280_S32_t)((((BMP280_U32_t)(a_data_u8r[0])) \
        << SHIFT_LEFT_12_POSITION) | (((BMP280_U32_t)(a_data_u8r[1])) \
        << SHIFT_LEFT_4_POSITION) | ((BMP280_U32_t)a_data_u8r[2] >>\
        SHIFT_RIGHT_4_POSITION));
    }
    return comres;
}

/*!
 * @brief Reads actual pressure from uncompensed pressure
 *							and returns the value in Pascal(Pa)
 *
 *
 *
 *  @param signed long : value of uncompensated pressure
 *
 *
 *
 *  @return
 *			unsigned long : actual pressure
*/
BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P)
{
    BMP280_S32_t v_x1_u32r = 0;
    BMP280_S32_t v_x2_u32r = 0;
    BMP280_U32_t pressure = 0;

    v_x1_u32r = (((BMP280_S32_t)p_bmp280->cal_param.t_fine) >> 1) - \
        (BMP280_S32_t)64000;
    v_x2_u32r = (((v_x1_u32r >> 2) * (v_x1_u32r >> 2)) >> 11) * \
        ((BMP280_S32_t)p_bmp280->cal_param.dig_P6);
    v_x2_u32r = v_x2_u32r + ((v_x1_u32r * \
        ((BMP280_S32_t)p_bmp280->cal_param.dig_P5)) << 1);
    v_x2_u32r = (v_x2_u32r >> 2) + \
        (((BMP280_S32_t)p_bmp280->cal_param.dig_P4) << 16);
    v_x1_u32r = (((p_bmp280->cal_param.dig_P3 * (((v_x1_u32r >> 2) * \
        (v_x1_u32r >> 2)) >> 13)) >> 3) + \
        ((((BMP280_S32_t)p_bmp280->cal_param.dig_P2) * \
        v_x1_u32r) >> 1)) >> 18;
    v_x1_u32r = ((((32768+v_x1_u32r)) * \
        ((BMP280_S32_t)p_bmp280->cal_param.dig_P1))	>> 15);
    if (v_x1_u32r == 0)
        return 0; /* Avoid exception caused by division by zero */
    pressure = (((BMP280_U32_t)(((BMP280_S32_t)1048576) - adc_P) - \
        (v_x2_u32r >> 12))) * 3125;
    if (pressure < 0x80000000)
        pressure = (pressure << 1) / ((BMP280_U32_t)v_x1_u32r);
    else
        pressure = (pressure / (BMP280_U32_t)v_x1_u32r) * 2;
    v_x1_u32r = (((BMP280_S32_t)p_bmp280->cal_param.dig_P9) * \
        ((BMP280_S32_t)(((pressure >> 3) * (pressure >> 3)) >> 13))) \
        >> 12;
    v_x2_u32r = (((BMP280_S32_t)(pressure >> 2)) * \
        ((BMP280_S32_t)p_bmp280->cal_param.dig_P8)) >> 13;
    pressure = (BMP280_U32_t)((BMP280_S32_t)pressure + \
        ((v_x1_u32r + v_x2_u32r + p_bmp280->cal_param.dig_P7) >> 4));

    return pressure;
}

/*!
 * @brief reads uncompensated pressure and temperature
 *
 *
 *
 *
 *  @param unsigned long utemperature : Address of uncompensated temperature
 *		   unsigned long upressure : Address of uncompensated pressure
 *
 *
 *  @return
 */
BMP280_RETURN_FUNCTION_TYPE bmp280_read_uput(BMP280_S32_t *upressure,\
        BMP280_S32_t *utemperature)
{
    BMP280_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char a_data_u8r[6] = {0};
    if (p_bmp280 == BMP280_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres += p_bmp280->BMP280_BUS_READ_FUNC(p_bmp280->dev_addr, \
        BMP280_PRESSURE_MSB_REG, a_data_u8r, 6);
        *upressure = (BMP280_S32_t)((((BMP280_U32_t)(a_data_u8r[0])) \
        << SHIFT_LEFT_12_POSITION) | (((BMP280_U32_t)(a_data_u8r[1])) \
        << SHIFT_LEFT_4_POSITION) | ((BMP280_U32_t)a_data_u8r[2] >>\
        SHIFT_RIGHT_4_POSITION));

        /* Temperature */
        *utemperature = (BMP280_S32_t)((( \
        (BMP280_U32_t) (a_data_u8r[3])) << SHIFT_LEFT_12_POSITION) | \
        (((BMP280_U32_t)(a_data_u8r[4])) << SHIFT_LEFT_4_POSITION) \
        | ((BMP280_U32_t)a_data_u8r[5] >> SHIFT_RIGHT_4_POSITION));
    }
    return comres;
}

/*!
 * @briefreads pressure and temperature
 *
 *
 *
 *
 *  @param unsigned long temperature : Address of temperature
 *		   unsigned long pressure : Address of pressure
 *
 *
 *  @return
*/
BMP280_RETURN_FUNCTION_TYPE bmp280_read_pt(BMP280_U32_t *pressure,\
        BMP280_S32_t *temperature)
{
    BMP280_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    BMP280_S32_t upressure = 0;
    BMP280_S32_t utemperature = 0;
    if (p_bmp280 == BMP280_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres = bmp280_read_uput(&upressure, &utemperature);
        *temperature = bmp280_compensate_T_int32(utemperature);
        *pressure = bmp280_compensate_P_int32(upressure);
    }
    return comres;
}




#ifdef BMP280_ENABLE_FLOAT
/*!
 * @brief Reads actual temperature from uncompensed temperature
 *							and returns the value in Degree centigrade
 *
 *
 *
 *  @param signed long : value of uncompensated temperature
 *
 *
 *
 *  @return
 *			double : actual temperature in floating point
 *
*/
double bmp280_compensate_T_double(BMP280_S32_t adc_T)
{
    double v_x1_u32r = 0;
    double v_x2_u32r = 0;
    double temperature = 0;

    v_x1_u32r  = (((double)adc_T) / 16384.0 - \
        ((double)p_bmp280->cal_param.dig_T1) / 1024.0) * \
        ((double)p_bmp280->cal_param.dig_T2);
    v_x2_u32r  = ((((double)adc_T) / 131072.0 - \
        ((double)p_bmp280->cal_param.dig_T1) / 8192.0) * \
        (((double)adc_T) / 131072.0 - \
        ((double)p_bmp280->cal_param.dig_T1) / 8192.0)) * \
        ((double)p_bmp280->cal_param.dig_T3);
    p_bmp280->cal_param.t_fine = (BMP280_S32_t)(v_x1_u32r + v_x2_u32r);
    temperature  = (v_x1_u32r + v_x2_u32r) / 5120.0;


    return temperature;
}

/*!
 * @brief Reads actual pressure from uncompensed pressure
 *							and returns the value in Pascal(Pa)
 *
 *
 *
 *  @param signed int : value of uncompensated pressure
 *
 *
 *
 *  @return
 *			double : actual pressure in floating point
 *
*/
double bmp280_compensate_P_double(BMP280_S32_t adc_P)
{
    double v_x1_u32r = 0;
    double v_x2_u32r = 0;
    double pressure = 0;

    v_x1_u32r = ((double)p_bmp280->cal_param.t_fine/2.0) - 64000.0;
    v_x2_u32r = v_x1_u32r * v_x1_u32r * \
        ((double)p_bmp280->cal_param.dig_P6) / 32768.0;
    v_x2_u32r = v_x2_u32r + v_x1_u32r * \
        ((double)p_bmp280->cal_param.dig_P5) * 2.0;
    v_x2_u32r = (v_x2_u32r / 4.0) + \
        (((double)p_bmp280->cal_param.dig_P4) * 65536.0);
    v_x1_u32r = (((double)p_bmp280->cal_param.dig_P3) * \
        v_x1_u32r * v_x1_u32r / 524288.0 + \
        ((double)p_bmp280->cal_param.dig_P2) * v_x1_u32r) / 524288.0;
    v_x1_u32r = (1.0 + v_x1_u32r / 32768.0) * \
        ((double)p_bmp280->cal_param.dig_P1);
    if (v_x1_u32r == 0.0)
        return 0; /* Avoid exception caused by division by zero */
    pressure = 1048576.0 - (double)adc_P;
    pressure = (pressure - (v_x2_u32r / 4096.0)) * 6250.0 / v_x1_u32r;
    v_x1_u32r = ((double)p_bmp280->cal_param.dig_P9) * \
        pressure * pressure / 2147483648.0;
    v_x2_u32r = pressure * ((double)p_bmp280->cal_param.dig_P8) / 32768.0;
    pressure = pressure + (v_x1_u32r + v_x2_u32r + \
        ((double)p_bmp280->cal_param.dig_P7)) / 16.0;

    return pressure;
}
#endif

#if defined(BMP280_ENABLE_INT64) && defined(BMP280_64BITSUPPORT_PRESENT)

/*!
 * @brief Reads actual pressure from uncompensed pressure
 *							and returns the value in Pascal(Pa)
 *
 *
 *
 *  @param signed long : value of uncompensated temperature
 *
 *
 *
 *  @return
 *			unsigned long : actual pressure
*/
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
    BMP280_S64_t v_x1_u32r = 0;
    BMP280_S64_t v_x2_u32r = 0;
	BMP280_S64_t v_x3_s64r = 0;
    BMP280_S64_t pressure = 0;
    v_x1_u32r = ((BMP280_S64_t)p_bmp280->cal_param.t_fine) - 128000;
    v_x2_u32r = v_x1_u32r * v_x1_u32r * \
        (BMP280_S64_t)p_bmp280->cal_param.dig_P6;
    v_x2_u32r = v_x2_u32r + ((v_x1_u32r * \
        (BMP280_S64_t)p_bmp280->cal_param.dig_P5) << 17);
    v_x2_u32r = v_x2_u32r + \
        (((BMP280_S64_t)p_bmp280->cal_param.dig_P4) << 35);
    v_x1_u32r = ((v_x1_u32r * v_x1_u32r * \
        (BMP280_S64_t)p_bmp280->cal_param.dig_P3) >> 8) + \
        ((v_x1_u32r * (BMP280_S64_t)p_bmp280->cal_param.dig_P2) << 12);
    v_x1_u32r = (((((BMP280_S64_t)1) << 47) + v_x1_u32r)) * \
        ((BMP280_S64_t)p_bmp280->cal_param.dig_P1) >> 33;
    if (v_x1_u32r == 0)
        return 0; /* Avoid exception caused by division by zero */
    pressure = 1048576 - adc_P;
    pressure = (((pressure << 31) - v_x2_u32r) * 3125) / v_x1_u32r;
    v_x1_u32r = (((BMP280_S64_t)p_bmp280->cal_param.dig_P9) * \
        (pressure >> 13) * (pressure >> 13)) >> 25;
    v_x2_u32r = (((BMP280_S64_t)p_bmp280->cal_param.dig_P8) * \
        pressure) >> 19;



	/* calculate x3*/
	v_x3_s64r = (BMP280_S64_t)(((BMP280_S64_t)(((pressure
	>> 4)
	* (pressure >> 4))
	>> 30) * (BMP280_S64_t)((pressure
	* (BMP280_S64_t)p_bmp280->cal_param.dig_P10)
	>> 10))
	>> 29);
	/* calculate true pressure*/

	
	pressure = ((pressure + v_x1_u32r + v_x2_u32r + v_x3_s64r) >> 8) + \
        (((BMP280_S64_t)p_bmp280->cal_param.dig_P7) << 4);
    return (BMP280_U32_t)pressure;
}
#endif

/*!
 * @brief Computing waiting time for sensor data read
 *
 *
 *
 *
 *  @param
 *			unsigned char : value of time
 *
 *
 *  @return
*/
BMP280_RETURN_FUNCTION_TYPE bmp280_compute_wait_time(unsigned char \
    *v_delaytime_u8r)
{
    BMP280_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;

    *v_delaytime_u8r = (T_INIT_MAX + T_MEASURE_PER_OSRS_MAX * \
        (((1 << p_bmp280->osrs_t) >> 1) + ((1 << p_bmp280->osrs_p) \
        >> 1)) + (p_bmp280->osrs_t ? T_SETUP_PRESSURE_MAX : 0) + 15) \
        / 16;
    return comres;
}



/*!
 * @brief get compersated temperature value
 *
 * @param data the pointer of bmp client data
 * @param temperature the pointer of temperature value
 *
 * @return zero success, non-zero failed
 * @retval zero success
 * @retval non-zero failed
*/
 sns_ddf_status_e bmp_get_temperature(struct bmp_client_data *data, int32_t *temperature)
{
    int32_t utemperature;
     sns_ddf_status_e err = SNS_DDF_SUCCESS;

    err = BMP_CALL_API(read_ut)(&utemperature);
    if (err)
        return err;
//	*temperature = BMP_CALL_API(compensate_T_double)(utemperature);

    *temperature = BMP_CALL_API(compensate_T_int32)(utemperature);
    return err;
}

/*!
 * @brief get compersated pressure value
 *
 * @param data the pointer of bmp client data
 * @param pressure the pointer of pressure value
 *
 * @return zero success, non-zero failed
 * @retval zero success
 * @retval non-zero failed
*/
 sns_ddf_status_e bmp_get_pressure(struct bmp_client_data *data, uint32_t *pressure)
{
    int32_t temperature;
    int32_t upressure;
    sns_ddf_status_e err = SNS_DDF_SUCCESS;

    /*get current temperature to compersate pressure value
    via global variable p_bmp280->cal_param.t_fine, which defined in sensor function API*/
    err = bmp_get_temperature(data, &temperature);
    if (err)
        return err;

    err = BMP_CALL_API(read_up)(&upressure);
    if (err)
        return err;

   // *pressure = BMP_CALL_API(compensate_P_int64)(upressure);
   // *pressure = *pressure/256;

	//BMP280_MSG_1(HIGH, " pressure value double is pa %d\n",(int)((BMP_CALL_API(compensate_P_double)(upressure))*1000));

	/* mutiple 1000 for keep the resolution */
    *pressure = (int)((BMP_CALL_API(compensate_P_double)(upressure))*1000);
    //	*pressure = BMP_CALL_API(compensate_P_int32)(upressure);
    return err;
}

 /*===========================================================================
 
 FUNCTION:	 sns_dd_bmp280_log_data
 
 ===========================================================================*/
 
 /*!
  *  @brief log sensor data
  *
  *  @detail
  *   Logs latest set of sensor data sampled from the sensor
  *
  *  @param[in] state: ptr to the driver structure
  *  @param[in] sample_time: Time that the sensor was sampled
  *
  *
  *  @return
  *
  */
 /*=========================================================================*/
 void sns_dd_bmp280_log_data(
         sns_dd_bmp280_state_t *state,
         sns_ddf_time_t sample_time,
         sns_ddf_sensor_e sensor)
 {
     sns_err_code_e err_code;
     sns_log_sensor_data_pkt_s* log_struct_ptr;
     log_pkt_t						 log_pkt_type;
 
#if BMP280_CONFIG_RUN_ON_OSSC
     log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC;
#else
     log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA;
#endif
 
     BMP280_MSG_0(HIGH, "Data[0] : Pressure in hPa (Q16)");
 
     //Allocate log packet
     err_code = sns_logpkt_malloc(log_pkt_type,
             sizeof(sns_log_sensor_data_pkt_s),
             (void**)&log_struct_ptr);
 
     if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
     {
         log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
         log_struct_ptr->sensor_id = sensor;
         log_struct_ptr->vendor_id = SNS_DDF_VENDOR_BOSCH;
 
         //Timestamp the log with sample time
         log_struct_ptr->timestamp = sample_time;
 
         //Log the sensor data
         log_struct_ptr->num_data_types = 1;

         if (sensor == SNS_DDF_SENSOR_PRESSURE)
         {
            log_struct_ptr->data[0]  = state->data_cache[0];
         }
         else
         {
            log_struct_ptr->data[0]  = state->data_cache[1];
         }
 
         //Commit log (also frees up the log packet memory)
         err_code = sns_logpkt_commit(log_pkt_type,
                 log_struct_ptr);
     }
 
     if (err_code != SNS_SUCCESS)
     {
         BMP280_MSG_1(ERROR, "BMP280 pressure Log Data - logpkt_malloc failed with err: %d", err_code);
     }
 }
 


/*!
*  @brief Called by the SMGR to get data
*
*  @detail
*   Requests a single sample of sensor data from each of the specified
*    sensors.
*
*  @param[in] dd_handle     Handle to a driver instance.
*  @param[in] sensors       List of sensors for which data isrequested.
*  @param[in] num_sensors   Length of @a sensors.
*  @param[in] memhandler    Memory handler used to dynamically allocate
*                           output parameters, if applicable.
*  @param[out] data         Sampled sensor data. The number of elements
*                           must match @a num_sensors.
*
*  @return
*    The error code definition within the DDF
*    SNS_DDF_SUCCESS on success.
*
*/


 sns_ddf_status_e sns_dd_alt_bmp280_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[]
)

{
  uint8_t i;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_ddf_sensor_data_s *data_ptr;

  sns_dd_bmp280_state_t *state = (sns_dd_bmp280_state_t *)dd_handle;
  uint32_t pressure;
  int32_t temperature;

  // This debug message is for testing uImage. When in bigImage operation this message
  // shows up on DIAG but does not show up in uImage mode.
  //UMSG(MSG_SSID_SNS,DBG_HIGH_PRIO, "BMP280 - get_data");

  if (state == NULL)
  {
     return SNS_DDF_EINVALID_PARAM;
  }
  for (i = 0; i < num_sensors; i++)
  {
    if (sensors[i] != SNS_DDF_SENSOR_PRESSURE &&
        sensors[i] != SNS_DDF_SENSOR_TEMP)
    {
      return SNS_DDF_EINVALID_PARAM;
    }
  }

  if( (data_ptr = sns_ddf_memhandler_malloc_ex(memhandler,
        (num_sensors)*(sizeof(sns_ddf_sensor_data_s)), state->smgr_handle)) == NULL)
  {
    return SNS_DDF_ENOMEM;
  }


  *data = data_ptr;

  for (i=0; i <num_sensors; i++)
  {
    data_ptr[i].sensor = sensors[i];
    data_ptr[i].status = SNS_DDF_SUCCESS;
    data_ptr[i].timestamp = sns_ddf_get_timestamp();

    if (sensors[i] == SNS_DDF_SENSOR_PRESSURE)
    {
      if( (data_ptr[i].samples = sns_ddf_memhandler_malloc_ex(memhandler,
            sizeof(sns_ddf_sensor_sample_s), state->smgr_handle)) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      status = bmp_get_pressure(&(state->data), &pressure);
      
    //  BMP280_MSG_3_P(HIGH, "%d %d %d",
    //	  num_sensors, pressure, sensors[i]);

    /*pressue data is mutipled by 1000 pa, and the unit is hpa, so divide by 100000 is need here */
	  data_ptr[i].samples[0].sample  = FX_FLTTOFIX_Q16(pressure/100000.0);
	  state->data_cache[0] = FX_FLTTOFIX_Q16(pressure/100000.0);



     // data_ptr[i].samples[0].sample  = pressure*bmp280_pressure_resolution;
     //  state->data_cache[0] = pressure*bmp280_pressure_resolution;
  
      data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;
      data_ptr[i].num_samples = 1;

      sns_dd_bmp280_log_data(state, data_ptr[i].timestamp, sensors[i]);

#ifdef BMP280_DEBUG
	  BMP280_MSG_2(HIGH, " pressure value in Q16 is %d,   TS is %d \n",state->data_cache[0], data_ptr[i].timestamp);
#endif

    }
    else // TEMPER sensor
    {
      if( (data_ptr[i].samples = sns_ddf_memhandler_malloc_ex(memhandler,
                                   sizeof(sns_ddf_sensor_sample_s), state->smgr_handle)) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }

      status = bmp_get_temperature(&(state->data), &temperature);

      data_ptr[i].samples[0].sample = temperature*bmp280_temperature_resolution;
      data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;

      data_ptr[i].num_samples = 1;

      state->data_cache[1] = data_ptr[i].samples[0].sample;
      sns_dd_bmp280_log_data(state, data_ptr[i].timestamp, sensors[i]);

     }
  }





  return status;
}


//#define sns_alt_bmp280_driver_fn_list sns_dd_vendor_if_1


sns_ddf_driver_if_s sns_alt_bmp280_driver_fn_list =
{
    .init                 = &sns_dd_alt_bmp280_init,
    .get_data             = &sns_dd_alt_bmp280_get_data,
    .set_attrib           = &sns_dd_alt_bmp280_set_attr,
    .get_attrib           = &sns_dd_alt_bmp280_get_attr,
    .handle_irq           = NULL,
    .reset                = &sns_dd_alt_bmp280_reset,
    .run_test             = &sns_dd_alt_bmp280_self_test,
    .enable_sched_data    = NULL,
    .probe                = sns_dd_alt_bmp280_probe,
    .trigger_fifo_data    = NULL
};

