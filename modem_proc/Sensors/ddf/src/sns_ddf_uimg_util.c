/**
 * @file sns_ddf_util.c
 * @brief Utility library providing common services to drivers.
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ddf/src/sns_ddf_uimg_util.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-11-03 pn   Moved timer service to its own module
  2015-04-09 pn   Changed timestamp size to 56 bits
  2015-02-24 pn   Updated QDSS events
  2015-01-21 BD   Moved F3 constant string from uImage to bigImage
  2014-12-10 MW   Included sns_ddf_signal_priv.h
  2014-09-17 vy   Updated mfree/malloc_ex to handle NULL sensor_handle
  2014-09-14 vy   Moved ddf_mfree to BigImage
  2014-09-08 pn   Fixed crash caused by double freeing memory
  2014-08-23 vy   Added QDSS logs and added data access protection in uImage
  2014-08-23 vy   Used new DDF apis to allocate/free mem supported in uImage
  2014-07-22 ps   Fix compiler warnings
  2014-07-09 vy   Added support for non-uimage dd
  2014-06-05 vy   Refactored for uImage
==============================================================================*/

#include "sns_ddf_util.h"
#include "sns_smgr_ddf_if.h"


#define __SNS_MODULE__ SNS_DDF


SNS_DDF_UIMAGE_CODE sns_ddf_status_e sns_ddf_malloc_ex(void** ptr, uint16_t size, sns_ddf_handle_t sensor_handle)
{
    bool  is_uimg_refac = sns_smgr_get_uimg_refac(sensor_handle);

    if ( sensor_handle == NULL || is_uimg_refac  )
    {
      *ptr = SNS_OS_ANY_MALLOC(SNS_DBG_MOD_DSPS_DDF, size);
    }
    else
    {
      if( sns_ddf_smgr_is_in_uimage() )
          return SNS_DDF_EFAIL;
      *ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_DDF, size);
    }
    return (*ptr != NULL) ? SNS_DDF_SUCCESS : SNS_DDF_ENOMEM;
}


SNS_DDF_UIMAGE_CODE sns_ddf_status_e sns_ddf_mfree_ex(void* ptr, sns_ddf_handle_t sensor_handle)
{
    bool  is_uimg_refac = sns_smgr_get_uimg_refac(sensor_handle);

    if(ptr == NULL )
    {
      return SNS_DDF_EFAIL;
    }
    if ( sensor_handle == NULL || is_uimg_refac )
    {
        SNS_OS_ANY_FREE(ptr);
    }
    else
    {
      if( sns_ddf_smgr_is_in_uimage() )
          return SNS_DDF_EFAIL;
      SNS_OS_FREE(ptr);
    }
    return SNS_DDF_SUCCESS;
}


/** Helper function to convert common error codes to DDF error codes. */
SNS_DDF_UIMAGE_CODE sns_ddf_status_e sns_ddf_convert_common_status(sns_err_code_e err)
{
    /* For now all we need is pass/fail. */
    return (err == SNS_SUCCESS) ? SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}


SNS_DDF_UIMAGE_CODE sns_ddf_time_t sns_ddf_get_timestamp(void)
{
  return sns_em_get_system_time();
}


SNS_DDF_UIMAGE_CODE uint32_t sns_ddf_convert_usec_to_tick(uint32_t usec)
{
    return sns_em_convert_usec_to_dspstick(usec);
}
SNS_DDF_UIMAGE_CODE sns_ddf_time_t sns_ddf_convert_usec_to_tick_64(uint64_t usec)
{
    return sns_em_convert_usec_to_dspstick64(usec);
}
SNS_DDF_UIMAGE_CODE uint64_t sns_ddf_convert_tick_to_usec_64(sns_ddf_time_t tick)
{
    return sns_em_convert_dspstick_to_usec64(tick);
}


SNS_DDF_UIMAGE_CODE void sns_ddf_delay(uint32_t usec)
{
  uint64_t  usec_long;
  sns_ddf_time_t start_tick, wait_tick;

#ifdef VIRTIO_BRINGUP
  /* it takes long time to busy-wait on Virtio */
  if (usec > 1000)
    usec = 1000;
#endif

#if !defined(SNS_PCSIM)
# if !defined(QDSP6)
    DALSYS_BusyWait(usec);
# else
    usec_long = usec;
    start_tick = sns_ddf_get_timestamp();
    wait_tick = (usec_long * DSPS_SLEEP_CLK + (1000000-1)) / 1000000;  /* at least one tick */
    while ( ( sns_ddf_get_timestamp() - start_tick ) < wait_tick )
    {
      ;
    }
# endif
#endif
}


SNS_DDF_UIMAGE_CODE void sns_ddf_map_axes(sns_ddf_axes_map_s* axes_map, q16_t* device_data)
{
    SNS_DDF_UIMAGE_DATA static q16_t tmp[3];

    tmp[0] = device_data[axes_map->indx_x] * axes_map->sign_x;
    tmp[1] = device_data[axes_map->indx_y] * axes_map->sign_y;
    tmp[2] = device_data[axes_map->indx_z] * axes_map->sign_z;
    device_data[0] = tmp[0];
    device_data[1] = tmp[1];
    device_data[2] = tmp[2];
}


SNS_DDF_UIMAGE_CODE void sns_ddf_map_axes_multiple(const sns_ddf_axes_map_s* axes_map,
                               sns_ddf_sensor_data_s* data_ptr)
{
    q16_t tmp[3];
    uint16_t i;
    for (i=0; i<data_ptr->num_samples; i+=3) /* num_samples is multiple of 3 */
    {
        tmp[0] = data_ptr->samples[i].sample;
        tmp[1] = data_ptr->samples[i+1].sample;
        tmp[2] = data_ptr->samples[i+2].sample;
        data_ptr->samples[i].sample   = tmp[axes_map->indx_x] * axes_map->sign_x;
        data_ptr->samples[i+1].sample = tmp[axes_map->indx_y] * axes_map->sign_y;
        data_ptr->samples[i+2].sample = tmp[axes_map->indx_z] * axes_map->sign_z;
    }
}

SNS_DDF_UIMAGE_CODE sns_ddf_status_e sns_ddf_mutex_lock(sns_ddf_handle_t handle)
{
    return SNS_DDF_EFAIL;
}


SNS_DDF_UIMAGE_CODE sns_ddf_status_e sns_ddf_mutex_release(sns_ddf_handle_t handle)
{
    return SNS_DDF_EFAIL;
}

/*===========================================================================

  FUNCTION:   get_bit_position

===========================================================================*/
/*!
  @brief This function returns the position of lowest bit set 

  @detail

  @param[i]  none
  @return   none
 */
/*=========================================================================*/
SNS_DDF_UIMAGE_CODE uint32_t get_bit_position (uint32_t flags)
{
  uint32_t bit_pos = 0;

  while (!(flags & 1)) 
  {   
    flags >>= 1;
    ++bit_pos;
  }
  return bit_pos;
}


