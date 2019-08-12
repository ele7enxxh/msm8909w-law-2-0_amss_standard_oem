
#ifndef SNS_SMGR_FIFO_H
#define SNS_SMGR_FIFO_H

/*=============================================================================
  @file sns_smgr_fifo.h

  This file contains the definitions used in SMGR FIFO

******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_fifo.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-01-07  pn   Changed sns_smgr_fifo_flush() signature
  2015-10-15  pn   Fixed handling of ODR_CHANGED event
  2015-08-19  MW   Update desired_watermark to uint32 to prevent data overflow
                   in internal WM calculations
  2015-06-05  pn   Same code path for Discrete and Unified FIFOs
  2015-03-10  jms  Removed unused/ineffective discrete FIFO synch code
  2015-02-17  pn   Changed sns_smgr_fifo_configure() signature
  2014-09-14  vy   Dynamically allocate Shared fifo data struct 
  2014-04-23  pn   Initial version

============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_smgr.h"
#include "sns_smgr_sensor_config.h"
#include "sns_ddf_common.h"
#include "sns_log_types.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/


//--------------- Unified fifo -----------------
typedef struct
{
  sns_q_link_s        sibling_link;
  sns_smgr_sensor_s*  sensor_ptr;
} smgr_fifo_shared_sensor_s;

//------------ General fifo support --------------
typedef struct
{
  bool                      is_fifo_enabled;        //true if ( WM > 0 )
  bool                      can_fifo_be_used;
  uint16_t                  current_watermark;
  uint32_t                  desired_watermark;      //newly requested WM size to set

  //TODO: ----- smgr_fifo_cfg_const_s ------
  //Set only once in sns_smgr_fifo_init_fifo_cfg on get_attr
  bool                      is_fifo_supported;
  bool                      is_hw_watermark_int_supported;
  uint16_t                  max_watermark;          //hardware FIFO size in samples
  uint16_t                  suitable_max_watermark; //reserves space for time to read fifo
  uint16_t                  depot_max;              //depot depth minus overflow margin
  sns_q_s                   shared_ddf_sens_q;      //queue of smgr_fifo_shared_sensor_s

} smgr_sensor_fifo_cfg_s;


/*----------------------------------------------------------------------------
 * Public Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
sns_err_code_e sns_smgr_fifo_init_fifo_cfg( sns_smgr_sensor_s *sensor_ptr );

sns_ddf_status_e sns_smgr_fifo_on_event_fifo_wm_int
(
  const sns_smgr_sensor_s*  sensor_ptr,
  sns_ddf_sensor_e          sensor
);
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_overflow
(
  sns_smgr_sensor_s*        sensor_ptr,
  sns_ddf_sensor_e          sensor
);
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_available
(
  sns_smgr_sensor_s*        sensor_ptr,
  sns_ddf_sensor_e          sensor
);
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_unavailable
(
  sns_smgr_sensor_s*        sensor_ptr,
  sns_ddf_sensor_e          sensor
);


void     sns_smgr_fifo_flush( sns_smgr_sensor_s* sensor_ptr, bool force_flush );

void     sns_smgr_fifo_configure( sns_smgr_sensor_s* sensor_ptr, bool force_wm1 );

void sns_smgr_fifo_log_event
(
   const sns_smgr_ddf_sensor_s *ddf_sensor_ptr,
   sns_fifo_event_e             fifo_event
);

/*----------------------------------------------------------------------------
 * Extern Function Declarations
 * -------------------------------------------------------------------------*/
void smgr_get_data(sns_smgr_ddf_sensor_s* in_ddf_sensor_ptr);




#endif /* #ifndef SNS_SMGR_FIFO_H */
