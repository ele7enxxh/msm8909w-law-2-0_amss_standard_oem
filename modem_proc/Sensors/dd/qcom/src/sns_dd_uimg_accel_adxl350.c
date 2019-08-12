/*==============================================================================

    S E N S O R S   A C C E L E R O M E T E R    D R I V E R

DESCRIPTION

   Impelements the driver for the ADXL350 accelerometer driver
   Code is based on ADXL350_DS_PrA_v5_23_Sept_2010.pdf data sheet.

Copyright (c) 2010 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_uimg_accel_adxl350.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
==============================================================================*/

/*============================================================================

                                INCLUDE FILES

============================================================================*/
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd.h"

#include "sns_dd_accel_adxl350_priv.h"

extern sns_ddf_status_e adxl350_log_data(
   sns_ddf_sensor_data_s *data_ptr);


/*=======================================================================

                   FUNCTION PROTOTYPES

========================================================================*/
sns_ddf_status_e sns_dd_adxl350_get_data(
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s*  data[]);


/*=======================================================================

                   STRUCTURE DEFINITIONS

========================================================================*/



/*=========================================================================

  Local Data Definitions

==========================================================================*/

/*
 * scale factor for FULL resolution.
 * Maintain 3.9 mg/LSB on all ranges.
 */
static q16_t adxl350_scale_full_res = FX_FLTTOFIX_Q16(16 * G / 4095);

/*
 * scale factor for FIXED resolution.
 * Varies according to range..
 */
static q16_t adxl350_scale_fix_res[ADXL350_NUM_RANGE] = {
    FX_FLTTOFIX_Q16(2 * G / 511),
    FX_FLTTOFIX_Q16(4 * G / 511),
    FX_FLTTOFIX_Q16(8 * G / 511),
    FX_FLTTOFIX_Q16(16 * G / 511)
};


/*===========================================================================

  FUNCTION:   sns_dd_adxl350_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  - Requests a single sample of sensor data from each of the specified
    sensors. Data is returned immediately after being read from the
    sensor, in which case data[] is populated in the same order it was
    requested,

  @param[in] dd_handle     Handle to a driver instance.
  @param[in] sensors       List of sensors for which data isrequested.
  @param[in] num_sensors   Length of @a sensors.
  @param[in] memhandler    Memory handler used to dynamically allocate
                           output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements
                           must match @a num_sensors.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO: use FIFO for multiple samples per axis.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_adxl350_get_data(
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s*  data[])
{
    sns_ddf_status_e status = SNS_DDF_EINVALID_PARAM;
    adxl350_drv_t *drv = (adxl350_drv_t *)dd_handle;
    #ifdef ADXL_CHECK_DATA_READY
    uint8_t intr_status = 0; /* updated regaldless if interrupts are enabled */
    #endif
    uint8_t read_count = 0;
    sns_ddf_sensor_data_s *data_ptr = NULL;
    int num_samples = 1; /* Shall be used in FIFO mode. */
    int i;

    if ((num_sensors != 1) || (sensors[0] != SNS_DDF_SENSOR_ACCEL))
    {
        ADXL350_ERR_MSG("invalid get_data params");
        return SNS_DDF_EINVALID_PARAM;
    }

    data_ptr = sns_ddf_memhandler_malloc(memhandler,
                                         sizeof(sns_ddf_sensor_data_s));
    if (data_ptr == NULL)
    {
        ADXL350_ERR_MSG("malloc failure sensor_data");
        return SNS_DDF_ENOMEM;
    }

    data_ptr->samples = sns_ddf_memhandler_malloc(memhandler,
                                                 num_samples*ADXL350_NUM_AXES*sizeof(sns_ddf_sensor_sample_s));

    if (data_ptr->samples == NULL)
    {
        ADXL350_ERR_MSG("malloc failure samples");
        return SNS_DDF_ENOMEM;
    }

    /*
     * Skip checking data-ready to save time.
     * The HW returns the last sample value if there is no new data ready.
     * Assuming that the SMGR flush the first sample or has some delay until the
     * first sample is ready.
     */
    #ifdef ADXL_CHECK_DATA_READY
    for (i = 0; i < DATA_READY_MAX_RETRY ; i++)
    {
        /* Check if data ready */
        status = sns_ddf_read_port(drv->i2c_handle,
                                   ADXL350_REG_INTERRUPT_SOURCE,
                                   &intr_status, ONE_BYTE, &read_count);

        if (status != SNS_DDF_SUCCESS)
        {
            ADXL350_ERR_MSG("fail to read intr_status");
            return status;
        }

        if ((intr_status & ADXL350_INT_DATA_READY))
        {
            break; /* status ok. No retry needed. */
        }

        /*
         * Delay according to the accel sampling rate.
         * Note: This shouldn't happen, the sensor manager should set attribute
         * for data rate that is higher than the rate
         * he is calling the get_data() API.
         */
        sns_ddf_delay(drv->retry_delay_usec);

#ifdef ADXL350_DEBUG
        drv->retry_counter++;
#endif
    }
    if ((intr_status & ADXL350_INT_DATA_READY) == 0)
    {
        ADXL350_ERR_MSG("samples not ready");
        return SNS_DDF_EDEVICE;
    }
#endif /* ADXL_CHECK_DATA_READY */

    /* Read all samples in one burst , FIFO in bypass mode. */
    status = sns_ddf_read_port(drv->i2c_handle,
                               ADXL350_REG_DATA_X0,
                               (uint8_t*) drv->data_raw, sizeof(drv->data_raw),
                               &read_count);

    if (status != SNS_DDF_SUCCESS)
    {
        ADXL350_ERR_MSG("fail to read samples");
        return status;
    }

    if (drv->resolution == ADXL350_FULL_RES_13_BIT)
    {
        for (i=0; i<ADXL350_NUM_AXES; i++)
        {
            /* Convert to m/s^2 , Convert int32_t to q16_t fixpoint type */
            drv->data_cache[i] = (int32_t) drv->data_raw[i] *
                adxl350_scale_full_res;
        }
        // $TODO: This is just for the testing with FFA
        // X = Y,  Y = -X, Z= Z
        drv->data_cache[0] = (int32_t) drv->data_raw[1] *
                adxl350_scale_full_res;
        drv->data_cache[1] = (int32_t) drv->data_raw[0] * (-1) *
                adxl350_scale_full_res;
        drv->data_cache[2] = (int32_t) drv->data_raw[2] *
                adxl350_scale_full_res;
    }
    else
    {
        for (i=0; i<ADXL350_NUM_AXES; i++)
        {
            /* Scale factor varies according to range */
            drv->data_cache[i] = (int32_t) drv->data_raw[i] *
                adxl350_scale_fix_res[drv->range];
        }
    }



    /* Fill the returned data */
    data_ptr->sensor = SNS_DDF_SENSOR_ACCEL;
    data_ptr->status = SNS_DDF_SUCCESS;
    data_ptr->timestamp = sns_ddf_get_timestamp();

    for (i=0; i<ADXL350_NUM_AXES; i++)
    {
        data_ptr->samples[i].sample = drv->data_cache[i];
        data_ptr->samples[i].status = SNS_DDF_SUCCESS;
    }

    data_ptr->num_samples = num_samples * ADXL350_NUM_AXES;

    data[0] = data_ptr;

    adxl350_log_data(data_ptr);

#ifdef ADXL350_DEBUG
    drv->samples_counter++;
#endif

    return SNS_DDF_SUCCESS;
}



