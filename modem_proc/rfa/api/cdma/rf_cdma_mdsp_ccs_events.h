#ifndef RF_CDMA_MDSP_CCS_EVENTS_H
#define RF_CDMA_MDSP_CCS_EVENTS_H

/*!
  @file
  rf_cdma_mdsp_ccs_events.h

  @brief
  This file defines the rf cdma mdsp CCS event IDs
 
  @details
  This file exports the definitions and declarations to be used by MC Layer.

*/

/*===========================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================
                 EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rf_cdma_mdsp_ccs_events.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/20/15   vr      Autopin check-in
12/01/15   vr      Synchronized Therm Read with TxAGC event
05/08/14   spa     Add LNA state 5 to keep event defs inline with RFLM
03/12/14   zhw     CDMA Tuner CL support
12/11/13   zhw     Tuner AOL Event Support
10/01/13   APU     Removed unused events.  
05/29/13   spa     Added support for Rx on/off events
05/28/13   APU     Added function to query the CCS event table.
05/24/13   APU     Eliminated unwanted event information structures.
05/23/13   APU     The CCS IDs no longer rely on fragile bit mask.
05/22/13   APU     Added the abilty to have max number of tasks for a given 
                   event.
05/17/13   sty     deleted unused RF_CDMA_MDSP_CCS_EVENT_RF_IRAT_TUNE_OUT_ID
04/30/13   sty     Added prototype for rf_cdma_mdsp_ccs_write_immediate
02/20/13   zhw     Remove unused API
01/12/13   cd      RxAGC LNA SM event configuration support
11/30/12   spa     Added dynamic event for HDET
11/29/12   APU     Changed the name of IRAT events as per Code Review.
11/26/12   APU     Added static events for IRAT tune in and tune out scripts. 
10/31/12   zhw     Better support for event abort. Use handle now
10/29/12   zhw     Support for event abort
09/24/12   spa     Split ON-OFF into two seperate events
09/11/12   zhw     Extract CDMA event transaction params into static struct
08/15/12   zhw     Added RF event handle in rf_cdma_data, which are used to 
                   control CCS event flow
07/24/12   spa     Added event ID and settings for temp_comp
07/24/12   zhw     CDMA. API for executing CCS event sequence.
06/12/12   Saul    CDMA. doxygen documentation.
06/12/12   Saul    CDMA. Providing RF ONOFF event info to FW.
06/01/12   saul    Initial version

===========================================================================*/

#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfcommon_mdsp_event_intf.h"
#include "rfm_mode_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!	
 @note
  Warning.. Warning.. Warning.......
  Please make sure that event_description always lines up with
  rf_mdsp_event_id_type in terms of how elements are ordered
  they need to be SAME otherwise any function using 
  rf_mdsp_event_id_descriptor_type will BREAK :(
  This is avoid iterating through this structure and thus
  keeping the functions simple and fast.
  SO when adding a new event below.. 
  PLEASE UPDATE  rf_mdsp_event_description[].
  Also please append the new events instead of inserting them.
*/	
/*=============================================================================
                        EVENT ID DEFINITIONS
=============================================================================*/
typedef enum 
{
 RF_CDMA_DUMMY_EVENT = RFCOMMON_MDSP_EVENT_ID_BASE_CDMA     ,
/*----------------------------------*/
/*! Wakeup Rx Event.*/
 RF_CDMA_MDSP_CCS_EVENT_WAKEUP_RX_ID  ,  
/*----------------------------------*/
/*! Sleep Rx Event.*/
 RF_CDMA_MDSP_CCS_EVENT_SLEEP_RX_ID  ,  
/*----------------------------------*/
/*! Wakeup Tx Event.*/
 RF_CDMA_MDSP_CCS_EVENT_WAKEUP_TX_ID ,  
/*----------------------------------*/
/*! Slee Tx Event.*/
 RF_CDMA_MDSP_CCS_EVENT_SLEEP_TX_ID  ,  
/*----------------------------------*/
/*! Re-Tune Event.*/
 RF_CDMA_MDSP_CCS_EVENT_RETUNE_ID   ,  
/*----------------------------------*/
/*! RF ON Event.*/
 RF_CDMA_MDSP_CCS_EVENT_RF_ON_ID  ,  
/*----------------------------------*/
/*! RF OFF Event.*/
 RF_CDMA_MDSP_CCS_EVENT_RF_OFF_ID  ,  
/*----------------------------------*/
/*! RF IRAT Build tune in script Event.*/ 
 RF_CDMA_MDSP_CCS_EVENT_RF_IRAT_TUNE_IN_ID  ,  
/*----------------------------------*/
/*! RF HDET read event */ 
 RF_CDMA_MDSP_CCS_EVENT_RF_HDET_CONFIG_ID   ,  
/*----------------------------------*/
/*! Rx LNA State 0 program event */ 
 RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G0_ID  ,  
/*----------------------------------*/
/*! Rx LNA State 1 program event */ 
 RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G1_ID  ,  
/*----------------------------------*/
/*! Rx LNA State 2 program event */ 
 RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G2_ID  ,  
/*----------------------------------*/
/*! Rx LNA State 3 program event */ 
 RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G3_ID  ,  
/*----------------------------------*/
/*! Rx LNA State 4 program event */ 
 RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G4_ID ,  
/*----------------------------------*/
/*! Rx LNA State 5 program event */ 
 RF_CDMA_MDSP_CCS_EVENT_RX_LNA_G5_ID ,  
/*----------------------------------------------------------------------------*/
/*! D-LNA State 0 program event */ 
 RF_CDMA_MDSP_CCS_EVENT_RX_ON_ID ,
/*----------------------------------------------------------------------------*/
/*! D-LNA State 1 program event */ 
 RF_CDMA_MDSP_CCS_EVENT_RX_OFF_ID ,
/*----------------------------------------------------------------------------*/
/*! Antenna Tuner Advanced Open Loop Event */ 
 RF_CDMA_MDSP_CCS_EVENT_AOL_ID,
/*----------------------------------------------------------------------------*/
/*! Antenna Tuner Closed Loop Event */ 
 RF_CDMA_MDSP_CCS_EVENT_CL_ID,
/*----------------------------------*/
/*! Therm Enable Event */
  RF_CDMA_MDSP_CCS_EVENT_THERM_ENABLE_ID ,
/*----------------------------------------------------------------------------*/
/*! Autopin Trigger for Gain State 1 Event */
  RF_CDMA_MDSP_CCS_EVENT_AUTOPIN_TRIGGER_G1_ID,
/*----------------------------------------------------------------------------*/
/*! Autopin Trigger for Gain State 2 Event */
  RF_CDMA_MDSP_CCS_EVENT_AUTOPIN_TRIGGER_G2_ID,
} rf_cdma_mdsp_event_id_type ;

typedef struct 
{
  rf_cdma_mdsp_event_id_type                            event_id ;
  size_t                                                max_tasks_allowed_for_event ;
  const rfcommon_mdsp_event_settings_type*              event_settings ;
} rf_cdma_ccs_event_descriptor_type ;

/*===========================================================================
                  Event Transaction Param DEFINITIONS
===========================================================================*/

/*===========================================================================
                   FUNCTION DEFINITIONS
 ===========================================================================*/

/*----------------------------------------------------------------------------*/
rf_cdma_ccs_event_descriptor_type*
rf_cdma_mdsp_ccs_export_event_tbl ( void ) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_ccs_cleanup_event
(
  rfcommon_mdsp_event_handle** event_handle
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_ccs_abort_event
(
  rfcommon_mdsp_event_handle** event_handle
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_ccs_write_immediate
(
  rf_buffer_intf *settings_buffer,
  rf_hal_bus_resource_type write_type,
  uint16 max_num_trans
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mdsp_event_start_push_and_commit
(
  rf_cdma_mdsp_event_id_type event_id,
  rfcommon_mdsp_event_handle** event_handle,
  const rfcommon_mdsp_event_settings_type* event_settings,
  rfcommon_mdsp_transaction_settings_type* transaction_params,
  void * shared_mem_buf_ptr,
  uint32 shared_mem_max_size_bytes,
  rf_buffer_intf *buf_obj_ptr
) ;


/*----------------------------------------------------------------------------*/
const rfcommon_mdsp_event_settings_type*
rf_cdma_mdsp_get_event_info
(
   rf_cdma_mdsp_event_id_type event_id 
) ;

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_MDSP_CCS_EVENTS_H */
