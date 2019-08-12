/*!
  @file mmal_ciq_fend.h

  @brief
  Contains functions to initialize, shutdown, and misc function calls to the IQ agent

  @detail
  Contains the wrapper functions which makes the following calls to the IQ agent
  IQ_Init Initializes the instance of the IQ Agent Front-end library.
  IQ_Shutdown Shuts down the instance of IQ Agent Front-end library.
  IQ_GetAgentVersion Enables an integrator to programatically retrieve the IQ Agent's
   software version number.
   
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_ciq_fend.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/10/11   sk     Added prorotype to set the active rat
03/28/11   sk     Initial Checkin
           
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#ifndef MMAL_CIQ_FEND_H
#define MMAL_CIQ_FEND_H

#include "mcs_variation.h"
#include <customer.h>
#include "mmal_cfg.h"
#include "mmal_lte_metrics.h"

#ifdef FEATURE_MMAL_CIQ

  #include <stdio.h>
  #include <diag.h>
  #include <log_codes.h>
  #include <log_codes_gsm.h>
  #include <log_codes_umts.h>
  #include <log_codes_wcdma_int.h>
  #include "mmal_ciq_metrics.h"
  #include <naslog_v.h>
  #include <mmal_util.h>


/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/
typedef enum
{
  NONE=0,
  GSM,
  WCDMA
}mmal_ciq_rat_e;

/*==============================================================================

                         INTERNAL VARIABLES

==============================================================================*/



/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/



/*==============================================================================

                                FUNCTIONS

==============================================================================*/
/*===========================================================================

  FUNCTION:  mmal_ciq_fend_set_active_rat

===========================================================================*/
/*!
    @brief
    This function sets the active RAT so that the log packet 0x713A
	can be appropriately routde

    @detail
    

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_fend_set_active_rat(mmal_ciq_rat_e rat);


/*===========================================================================

  FUNCTION:  mmal_ciq_fend_register_diag_logs_events

===========================================================================*/
/*!
    @brief
    This function registers for the diag logs and events

    @detail
    Different metrics require diff log packets or events
    This function will register those log packets and events that are necessary 
    to produce these metrics

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_fend_register_diag_logs_events(void);

/*===========================================================================

  FUNCTION:  mma_ciq_fend_deregister_diag_logs_events

===========================================================================*/
/*!
    @brief
    This function deregisters for the diag logs and events

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mma_ciq_fend_deregister_diag_logs_events(void);

/*===========================================================================

  FUNCTION:  mmal_ciq_fend_init

===========================================================================*/
/*!
    @brief
    This function initializes the IQ agent 

    @detail
    All the parameters that the metric submission code will use should also 
    be initialized here.

    @return
    TRUE indicates if the IQ agent was initialized successfully, FALSE
    otherwise

    @Note
    This function needs to be called prior to any metric submission related
    calls are made to the IQ agent.

*/
/*=========================================================================*/
boolean mmal_ciq_fend_init( void );

/*===========================================================================

  FUNCTION:  mmal_ciq_fend_shutdown

===========================================================================*/
/*!
    @brief
    This function shuts down the IQ agent 

    @detail
    
    @return
    VOID since the shutdown either completes or its a NOP for the IQ agent since
    the agent was not running earluier.
    
    @Note
    This function needs to be called after all the metric submission related
    calls are made to the IQ agent.

*/
/*=========================================================================*/
void mmal_ciq_fend_shutdown( void );

/*===========================================================================

  FUNCTION:  mmal_ciq_fend_process_log_listener_req

===========================================================================*/
/*!
    @brief
    mmal_ciq_fend_process_log_listener_req

    @detail
    
    @return
    
    
    @Note
    
*/
/*=========================================================================*/
void mmal_ciq_fend_process_log_listener_req
(
const unsigned int              log_code,
dsm_item_type              *log_info_ptr
);

/*===========================================================================

FUNCTION: mmal_ciq_metric_extract_nas_ota_msg

===========================================================================*/
/*!
@brief

This function intercepts the 0x713A log packet

@detail

@return
The log packet wil now contain only the nas_message

LOG_RECORD_DEFINE(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C)

// Indicates if this was sent to or from the UE 
uint8 nas_message_direction;

// Length of the NAS ota message 
uint32 nas_message_length;

// NAS ota Message 
uint8 nas_message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];

LOG_RECORD_END

@Note 

=========================================================================*/
uint8 mmal_ciq_metric_extract_nas_ota_msg
(
  dsm_item_type **dsm_log_ptr
);

#endif /* FEATURE_MMAL_CIQ*/
#endif //MMAL_CIQ_FEND_H
