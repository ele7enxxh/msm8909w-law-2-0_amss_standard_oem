#ifndef RFLM_WCDMA_MSG_H
#define RFLM_WCDMA_MSG_H

/*!
  @file
  rflm_wcdma_msg.h

  @brief
  This module is exports the messaging UMIDs for RFLM WCMDA
*/

/*==================================================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

==================================================================================================*/

/*==================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/wcdma/rflm_wcdma_msg.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
11/16/15   ag      Added UMID and associated cmd interface for Autopin  
03/12/15   gh      Added timing info to METER cmd payload
                   Added UMID and associated cmd interface
09/17/14   gh      Update RFLM MSGR payload (rflm_wcdma_do_mtpl_cmd) definition
05/20/14   aro     Doc update
05/01/14   aro     Added internal helper function to schedule trigger event
05/01/14   aro     Interface to process power meter trigger
05/01/14   aro     Initial Release

==================================================================================================*/

#include "rflm_msg.h"
#include "msgr.h"
#include "msgr_umid.h"

/*------------------------------------------------------------------------------------------------*/
/*! Structure to defining the payload  for DO MTPL command*/
typedef struct
{
  boolean uses_ilpc_data;
  /*!< Indicator for whether HDET data is coming from FBRx ILPC or not */

  boolean ilpc_data_valid;
  /*!< Indicator for whether FBRx ILPC data is valid or not */

  int32 sum_i;
  /*!< Sum of I from FBRx ILPC accumulation */

  int32 sum_q;
  /*!< Sum of Q from FBRx ILPC accumulation */

  uint64 sum_i_q_sqr;
  /*!< Sum of I2Q2 from FBRx ILPC accumulation */

  uint32 num_samples;
  /*!< Number of accumulation samples */

  uint32 txagc_boundary_time;
  /*!< Slot boundary timing information associated to this trigger */

  uint32 stable_tx_start_time;
  /*!< Stable Tx start time */

  uint32 stable_tx_stop_time;
  /*!< Stable Tx stop time */

  uint32 trigger_time;
  /*!< Meter event trigger time (action time) */

  uint32 cb_stmr_time;
  /*!< Record of the stmr tick at the point of CB */

} rflm_wcdma_do_mtpl_cmd;

/*------------------------------------------------------------------------------------------------*/
/*! Structure to defining DO MTPL command*/
typedef struct 
{
  msgr_hdr_s msg_hdr;
  /*!< Message router header */

  rflm_wcdma_do_mtpl_cmd msg_payload;
  /*!< Payload for do_mtpl command */

} rflm_wcdma_do_mtpl_cmd_t;

/*------------------------------------------------------------------------------------------------*/
/*! Structure to defining the payload  for DO THERM READ command*/
typedef struct
{
  boolean therm_read_setup_completed;
  /*!< Indicator for whether therm setup and trigger is completed or not */

  uint32 slot_boundary_time;
  /*!< Current tx slot boundary time */

  uint32 stable_tx_start_time;
  /*!< Current tx slot stable tx start time */

  uint32 stable_tx_stop_time;
  /*!< Current tx slot stable tx start time */

  uint32 event_action_time;
  /*!< THERM_ENABLE event action time */

  uint32 cb_stmr_time;
  /*!< Record of the stmr tick at the point of CB */

} rflm_wcdma_do_therm_read_cmd;

/*------------------------------------------------------------------------------------------------*/
/*! Structure to defining DO THERM READ command*/
typedef struct 
{
  msgr_hdr_s msg_hdr;
  /*!< Message router header */

  rflm_wcdma_do_therm_read_cmd msg_payload;
  /*!< Payload for do_mtpl command */

} rflm_wcdma_do_therm_read_cmd_t;


/*------------------------------------------------------------------------------------------------*/
/*! Enumeration for RFLM WCDMA UMID */
enum 
{
  MSGR_DEFINE_UMID( RFLM, WCDMA, CMD,  DO_MTPL, 0x00, rflm_wcdma_do_mtpl_cmd_t )
  /*!< UMID definign the DO MTPL command */ 

};
 
/*------------------------------------------------------------------------------------------------*/
/*! Enumeration for RFLM WCDMA UMID */
enum 
{
  MSGR_DEFINE_UMID( RFLM, WCDMA, CMD,  DO_THERM_READ, 0x01, rflm_wcdma_do_therm_read_cmd_t )
  /*!< UMID definign the DO THERM READ command */ 

};

 /*------------------------------------------------------------------------------------------------*/
/*! Structure to defining Enable Autopin Read command*/
 /*! Structure to define TRIGGER PAR READ*/
 typedef struct 
 {
   msgr_hdr_s msg_hdr;
   /*!< Message router header */
   
 } rflm_wcdma_trigger_par_read_cmd_t;


/*-----------------------------------------
------------------------------------------------------*/
/*! Enumeration for RFLM WCDMA UMID */
enum 
{
  MSGR_DEFINE_UMID( RFLM, WCDMA, CMD,  TRIGGER_AUTO_PIN_READ, 0x02, rflm_wcdma_trigger_par_read_cmd_t )
  /*!< UMID defining the AUTOPIN READ command */ 

};
 
 
#ifdef __cplusplus
}
#endif

#endif /* RFLM_WCDMA_MSG_H */

