/*=============================================================================
  @file sns_smgr_ddf_if.c

  This file implements the interface between SMGR and DDF

*******************************************************************************
* Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_ddf_if_uimg.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-11-03  pn   Moved DDF timer handling to DDF
  2015-10-14  jtl  Remove heartbeat check after sensors generate samples
  2015-09-22  pn   Checks heartbeats after self-sched sensors generate samples
  2015-08-25  tc   Allow the SMGR to handle DAF indications while in uImage
  2015-07-06  hw   Adding sampling latency measurement support
  2015-04-29  AH   Added input validation in sns_ddf_smgr_notify_data and sns_ddf_smgr_notify_event
  2015-04-09  pn   Updated timestamps from uint32_t to sns_ddf_time_t
  2015-02-24  pn   Updated QDSS events
  2015-01-22  BD   Changed the way of putting functions in uImage to use sections
  2014-09-08  pn   Remembers and votes for current image mode when exiting big image
  2014-08-25  vy   Enabled QDSS logging
  2014-08-24  vy   Added an api to get uimage status of driver using sns handle
  2014-08-05  pn   Let RH task processs ODR_CHANGED event before SMGR task does
  2014-07-31  pn   Obsoleted FEATURE_TEST_DRI
  2014-07-27  vy   Moved sns_ddf_smgr_notify_event to uImage code
  2014-07-14  VY   Fixed a compiler error
  2014-07-20  VY   Disabled logging in uImage
  2014-07-09  vy   Refactored to support uImage
  2014-04-23  pn   Initial version

============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#ifdef SNS_USES_ISLAND
#include "qurt_island.h"
#endif

#include "sns_em.h"
#include "sns_profiling.h"
#include "sns_rh_util.h"
#include "sns_smgr_ddf_priv.h"
#include "sns_smgr_ddf_if.h"
#include "sns_smgr_main.h"
#include "sns_smgr_pm_if.h"
#include "sns_smgr_priv.h"
#include "sns_smgr_reg.h"
#include "sns_smgr_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Global Variables
 * -------------------------------------------------------------------------*/
extern sns_smgr_s              sns_smgr;

#ifdef SNS_QDSP_SIM
extern sns_dri_sim_s sns_dri_sim;
extern bool md_int_happened;
#endif

/*----------------------------------------------------------------------------
 *  Local Functions
 * -------------------------------------------------------------------------*/

/*===========================================================================

  FUNCTION:   sns_ddf_smgr_notify_timer

===========================================================================*/
/*!
  @brief DDF, device driver call back. Entered on interrupt.
    Queue the argument

  @detail

  @param[i]  timer
  @return   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_ddf_smgr_notify_timer(void* timer)
{
  sns_profiling_log_qdss(SNS_SMGR_DD_SERVICE_ENTER, 1, SNS_QDSS_DDF_NOTIFY_TIMER);
  /* SNS_SMGR_DD_SERVICE_EXIT is logged in sns_smgr_dd_service(). */

  sns_ddf_timer_post(timer);
  sns_smgr_signal_me(SNS_SMGR_DD_SERVICE_SIG);
}


/*===========================================================================

  FUNCTION:   sns_ddf_smgr_notify_irq

===========================================================================*/
/*!
  @brief DDF, device driver call back. Entered on interrupt.
    Queue the argument

  @detail  After Driver registers callback with DDF.
    ISR calls sns_ddf_smgr_notify_irq, irq gets queued
    sns_smgr_dd_service gets called in SMGR context, and calls sns_ddf_signal_dispatch()
    DDF calls the driver's registered callback
    Finally, if the driver has an event (like motion detect)
    it'll call sns_ddf_smgr_notify_event to let SMGR know

  @param[i]  param  interrupt param
  @return   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_ddf_smgr_notify_irq(uint32_t param)
{
  sns_profiling_log_qdss(SNS_SMGR_DD_SERVICE_ENTER, 1, SNS_QDSS_DDF_NOTIFY_IRQ);
  /* SNS_SMGR_DD_SERVICE_EXIT is logged in sns_smgr_dd_service(). */

  sns_ddf_signal_post(param);

  /* Notify the SMGR task that a driver interrupt requires handling. */
  sns_smgr_signal_me(SNS_SMGR_DD_SERVICE_SIG);
}


/*===========================================================================

  FUNCTION:   sns_ddf_smgr_notify_data

===========================================================================*/
/*!
  @brief  A device driver calls this function to deliver data that was previously
    requested using get_data()

  @Detail

  @param[i] smgr_handle - A value supplied when initializing the device driver. It is the
                         index to the sensor structures
  @param[i] data - pointer to an array of data types (known as sensors in DDF)
  @param[i] data_len - Number of data types
  @return
   SNS_DDF_SUCCESS
   SNS_DDF_EINVALID_PARAM
*/
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE sns_ddf_status_e sns_ddf_smgr_notify_data (
    sns_ddf_handle_t       smgr_handle,
    sns_ddf_sensor_data_s  data[],
    uint32_t               data_len)
{
  uint8_t i;
  sns_smgr_sensor_s* sensor_ptr = (sns_smgr_sensor_s*)smgr_handle;

  /* ---------------------------------------------------------------------- */
  if (!SMGR_HANDLE_VALID(sensor_ptr) ||
  	 (data == NULL) ||
  	 (data_len > SMGR_MAX_DATA_TYPES_PER_DEVICE))
  {
     return SNS_DDF_EINVALID_PARAM;
  }

  if ( sensor_ptr->const_ptr->flags & SNS_REG_SSI_FLAG_LTCY_ENABLE )
  {
    sns_pm_img_mode_e curr_mode = sns_smgr_get_curr_image_vote();
    sns_latency.dri_get_data_end_ts = sns_ddf_get_timestamp();
    sns_latency.dri_notify_irq_ts = (uint64_t)data[0].end_timestamp;

    sns_smgr_vote_image_mode(SNS_IMG_MODE_BIG);
    sns_profiling_log_latency_dri(sns_latency, data[0].sensor);
    sns_smgr_vote_image_mode(curr_mode);
  }

  for ( i = 0; i < data_len; i++ )
  {
    sns_smgr_ddf_sensor_s* ddf_sensor_ptr =
      sns_smgr_match_ddf_sensor(sensor_ptr, data[i].sensor);
    if ( ddf_sensor_ptr != NULL )
    {
       smgr_process_dri_fifo_async_data(ddf_sensor_ptr, &data[i]);
    }
  }
  return SNS_DDF_SUCCESS;
 }


/*===========================================================================

  FUNCTION:   sns_ddf_smgr_notify_event

===========================================================================*/
/*!
  @brief  A device driver calls this function to deliver an event

  @Detail

  @param[i] smgr_handle  smgr handle passed to DD
  @param[i] sensor  sensor enum
  @param[i] event   event name
  @return
   SNS_DDF_SUCCESS
   SNS_DDF_EINVALID_PARAM if event with the sensor is not expected
*/
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE sns_ddf_status_e sns_ddf_smgr_notify_event(
    sns_ddf_handle_t  smgr_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_event_e   event)
{
  sns_smgr_sensor_s*    sensor_ptr = (sns_smgr_sensor_s*)smgr_handle;
  sns_ddf_status_e  rc = SNS_DDF_SUCCESS;

  /* ---------------------------------------------------------------------- */
  if (!SMGR_HANDLE_VALID(sensor_ptr))
  {
     return SNS_DDF_EINVALID_PARAM;
  }

  if (event == SNS_DDF_EVENT_FIFO_WM_INT )
  {
    rc = sns_smgr_fifo_on_event_fifo_wm_int( sensor_ptr, sensor );
  }
  else
  {
    sns_pm_img_mode_e curr_mode = sns_smgr_get_curr_image_vote();
    sns_smgr_vote_image_mode(SNS_IMG_MODE_BIG);
    rc = sns_smgr_notify_event_in_bigimage(smgr_handle, sensor, event);
    sns_smgr_vote_image_mode(curr_mode);
  }
  
  return rc;
}


/*===========================================================================  

  FUNCTION:   sns_ddf_smgr_notify_daf_ind

===========================================================================*/
/**
 * @brief Notifies a Sensor1 client of a Driver Access message from the driver.
 *        Asynchronous API.
 *
 * @param[in]  ind_id        Indication identifier.
 * @param[in]  ind_msg       Pointer to the Indication message. This is allocated
 *                           by the device driver and must be freed by the driver
 *                           upon returning from this function!
 * @param[in]  ind_size      The number of bytes in @ind_msg. If there is no
 *                           indication message for the indication, then this
 *                           will be 0 to show that the DAF indication is not
 *                           present. This cannot be larger than
 *                           @SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01 bytes. If it is
 *                           the indication message will be truncated.
 * @param[in]  trans_id_ptr  Pointer to the transaction identifier. If there is
                             no transaction ID, this field will be null.
 * @param[in]  conn_handle   The connection handle for the request message.
 *                           This value must be saved by the device driver if the
 *                           corresponding request is expected to generate
 *                           indications. Upon notifying the SMGR of an
 *                           indication, this value must be provided to the SMGR
 *                           so the SMGR can forward the indication to the client.
 *
 * @return Success if the message was correctly sent. Otherwise a specific
 *         error code is returned.
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE sns_ddf_status_e sns_ddf_smgr_notify_daf_ind(
  uint32_t               ind_id,
  const void*            ind_msg,
  uint32_t               ind_size,
  const uint8_t*         trans_id_ptr,
  void*                  conn_handle)
{
  sns_smgr_indication_s* smgr_ind_ptr;
  sns_smgr_driver_access_ind_msg_v01* ind_ptr;

  // PEND: Vote for BigImage. At the moment, the DAF does not support uImage
  sns_pm_img_mode_e prev_mode = sns_smgr_get_curr_image_vote();
  sns_smgr_vote_image_mode(SNS_IMG_MODE_BIG);
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF2(MED, "DAF Indication ID: %d  size: %d", ind_id, ind_size);

  if ( conn_handle == NULL || (ind_msg == NULL && ind_size > 0) )
  {
    SNS_SMGR_PRINTF3(
      ERROR, "notify_daf_ind - conn_handle=0x%x ind_msg=0x%x ind_size=%u",
      conn_handle, ind_msg, ind_size);
    return SNS_DDF_EINVALID_PARAM;
  }

  // Allocate the indication message in DDR. It will be handled in big image.
  smgr_ind_ptr = SMGR_ALLOC_STRUCT(sns_smgr_indication_s);

  if (smgr_ind_ptr == NULL)
  {
    return SNS_DDF_ENOMEM;
  }

  smgr_ind_ptr->conn_handle = conn_handle;
  smgr_ind_ptr->msg_id = SNS_SMGR_DRIVER_ACCESS_IND_V01;
  smgr_ind_ptr->ind_len = sizeof(sns_smgr_driver_access_ind_msg_v01);

  ind_ptr = &smgr_ind_ptr->ind.driver_access;
  ind_ptr->IndicationId = ind_id;

  if ( ind_size > 0 )
  {
    ind_ptr->IndicationMsg_valid = true;
    ind_ptr->IndicationMsg_len =
      SNS_OS_MEMSCPY(ind_ptr->IndicationMsg, sizeof(ind_ptr->IndicationMsg),
                     ind_msg, ind_size);
    // Check for overflow condition
    if ( ind_ptr->IndicationMsg_len < ind_size )
    {
      // PEND: CONSIDER ADDING AN ERROR CODE FOR "MESSAGE TOO LARGE FOR BUFFER"
      SNS_SMGR_PRINTF2(ERROR, "Indication size of %d is truncated to %d",
                       ind_size, ind_ptr->IndicationMsg_len);
    }
  }

  if ( trans_id_ptr != NULL )
  {
    ind_ptr->TransactionId_valid = true;
    ind_ptr->TransactionId = *trans_id_ptr;
  }

  // Forward the indication message to Request Handler
  sns_rh_put_next_indication(smgr_ind_ptr);

  // Re-vote for the previous image mode
  sns_smgr_vote_image_mode(prev_mode);

  return SNS_DDF_SUCCESS;
}


/*===========================================================================  

  FUNCTION:   sns_ddf_smgr_is_in_uimage

===========================================================================*/  
/*!
 * @brief Gets from SMGR if it is in uImage mode
 *
 * @param[in]  None.
 *
 * @return True if in uImage mode, False otherwise
 * */ 
/*=========================================================================*/ 
SNS_SMGR_UIMAGE_CODE bool sns_ddf_smgr_is_in_uimage(
  void)
{
  unsigned int retval = 0; 

#ifdef SNS_USES_ISLAND
  retval = qurt_island_get_status(); 
  // qurt_island_get_status = {1=uImage, 0=BigImage}
#endif

  return (bool)retval;
}


/*===========================================================================

  FUNCTION:   sns_smgr_get_uimg_refac

===========================================================================*/
/*!
  @brief This function returns the uimage refac status for a particular sensor

  @detail Must return false if handle is NULL

  @param   pointer to sensor data 
  @return  return TRUE if refac for uimage else FALSE
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE bool sns_smgr_get_uimg_refac(sns_ddf_handle_t sensor_handle)
{
  sns_smgr_sensor_s *sensor_ptr;
  if (sensor_handle == NULL)
  {
    return false;
  }
  else
  {
    sensor_ptr = (sns_smgr_sensor_s *)sensor_handle; 
    return (sensor_ptr->is_uimg_refac);
  }  
}

