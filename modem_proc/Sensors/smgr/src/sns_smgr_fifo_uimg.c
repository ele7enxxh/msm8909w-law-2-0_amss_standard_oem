/*=============================================================================
  @file sns_smgr_fifo.c

  This file contains the logic for managing sensor FIFO in SMGR

*******************************************************************************
* Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_fifo_uimg.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-09  pn   Replaced calls to sns_em_get_timestamp() with sns_ddf_get_timestamp()
  2015-03-10  jms  Removed unused/ineffective discrete FIFO synch code
  2015-01-22  BD   Changed the way of putting functions in uImage to use sections
  2014-09-08  pn   Turns on QuP clock before triggering FIFO
  2014-08-18  pn   Removed KW error
  2014-08-05  pn   Toggles self-scheduling when flushing FIFO
  2014-07-20  VY   Disabled logging in uImage
  2014-07-09  vy   Added support for non-uimage DD
  2014-06-11  vy   Replaced SNS_OS_FREE by SNS_OS_ANY_FREE
  2014-04-23  pn   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_profiling.h"
#include "sns_rh_util.h"
#include "sns_smgr_ddf_if.h"
#include "sns_smgr_hw.h"
#include "sns_smgr_main.h"
#include "sns_smgr_pm_if.h"
#include "sns_smgr_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
//#define SNS_SMGR_FIFO_DEBUG
#ifdef SNS_SMGR_FIFO_DEBUG
#define SNS_SMGR_FIFO_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_FIFO_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_FIFO_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_FIFO_DBG3 SNS_SMGR_PRINTF3
#else
#define SNS_SMGR_FIFO_DBG0(level,msg)
#define SNS_SMGR_FIFO_DBG1(level,msg,p1)
#define SNS_SMGR_FIFO_DBG2(level,msg,p1,p2)
#define SNS_SMGR_FIFO_DBG3(level,msg,p1,p2,p3)
#endif

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *  Function Prototypes
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
//============================================================================
//============================================================================
//            F I F O     E V E N T     P R O C E S S I N G
//============================================================================
//============================================================================

/*===========================================================================
  FUNCTION:   sns_smgr_fifo_on_sns_ddf_event_fifo_wm_int
===========================================================================*/
/*!
  @brief processing SNS_DDF_EVENT_FIFO_WM_INT event
  @param[in] ddf_sensor_ptr
  @param[in] sensor
  @return sns_ddf_status_e
*/
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE sns_ddf_status_e sns_smgr_fifo_on_event_fifo_wm_int
(
  const sns_smgr_sensor_s* sensor_ptr,
  sns_ddf_sensor_e     sensor
)
{
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = sns_smgr_match_ddf_sensor( sensor_ptr, sensor );
  //SNS_SMGR_FIFO_DBG1( MED,"FIFO SNS_DDF_EVENT_FIFO_WM_INT ddf_sensor=%d", sensor );
  if ( ddf_sensor_ptr == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ( ddf_sensor_ptr->data_type == SNS_SMGR_DATA_TYPE_SECONDARY_V01 )
  {
    smgr_get_data(ddf_sensor_ptr); //sensor_type SMGR_SENSOR_TYPE_SECONDARY
  }

  sns_smgr_fifo_log_event( ddf_sensor_ptr, SNS_DDF_FIFO_WM_INT );
  
  return SNS_DDF_SUCCESS;
}


//============================================================================
//============================================================================
//                      P U B L I C     U T I L S
//============================================================================
//============================================================================

//============================================================================
//============================================================================
//                   D I S C R E T E    F I F O
//============================================================================
//============================================================================

/*===========================================================================
FUNCTION:   sns_smgr_fifo_log_event
===========================================================================*/
/*!
  @brief Log an occurrence of a fifo event.
  @param[in] ddf_sensor_ptr   - the sensor pointer
  @param[in] sns_fifo_event_e - fifo_event
  @return none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_smgr_fifo_log_event
(
  const sns_smgr_ddf_sensor_s* ddf_sensor_ptr,
  sns_fifo_event_e             fifo_event
)
{
  sns_err_code_e        status = SNS_ERR_UNKNOWN;
  sns_log_fifo_event_s* log    = NULL;

  if( ddf_sensor_ptr == NULL )
  {
    SNS_SMGR_FIFO_DBG0(ERROR, "sns_smgr_log_fifo_event - Received NULL Pointer");
    return;
  }
  status = sns_logpkt_malloc( SNS_LOG_SENSOR_FIFO_EVENT,
                              sizeof(sns_log_fifo_event_s), (void**)&log );
  if( status != SNS_SUCCESS || log == NULL )
  {
    return;
  }

  log->version = SNS_LOG_FIFO_EVENT_VERSION;
  log->timestamp = sns_ddf_get_timestamp();
  log->sensor_id = SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                                    SNS_SMGR_DATA_TYPE_PRIMARY_V01);
  log->fifo_event = fifo_event;
  log->watermark_depth =
    ddf_sensor_ptr->sensor_ptr->fifo_cfg.current_watermark;
  log->odr = ddf_sensor_ptr->current_odr;
  log->hw_watermark_int_supported =
    ddf_sensor_ptr->sensor_ptr->fifo_cfg.is_hw_watermark_int_supported;
  log->max_watermark =
    ddf_sensor_ptr->sensor_ptr->fifo_cfg.max_watermark;
  /*Commit log (also frees up the log packet memory)*/
  sns_logpkt_commit(SNS_LOG_SENSOR_FIFO_EVENT, log);
}


