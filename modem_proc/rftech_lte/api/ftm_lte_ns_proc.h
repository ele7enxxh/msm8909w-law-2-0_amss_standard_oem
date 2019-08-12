/*!
  @file ftm_lte_ns_proc.h

  @brief
  This file contains the interface to the FTM LTE Non-signaling Processor

*/

/*============================================================================
                Copyright (c) 2008 - 2009 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/ftm_lte_ns_proc.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 07:59:25 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/10   pl      Adding support for dynamic message router registration
10/01/09   pl      Adding support for diag response packet in LTE 
                   Non-signaling commands processing
05/14/09   mkv	   Explicitly adding LTE Non-Signaling message definition file.
04/23/09   pl      Initial Check-in
===========================================================================*/

#ifndef FTM_LTE_NS_PROC_H
#define FTM_LTE_NS_PROC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_lte_ns_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  ftm_lte_ns_processor

===========================================================================*/
/*!
  @brief
  This is the FTM LTE non-signaling processor. It processes the incoming
  Non-Signaling command

  @return
  
*/
/*=========================================================================*/
void ftm_lte_ns_processor( ftm_msgr_u *cmd_data_ptr, ftm_rsp_pkt_type *rsp_pkt_ptr );

/*===========================================================================

  FUNCTION:  ftm_lte_ns_register_msgr

===========================================================================*/
/*!
  @brief
  This function is used by FTM Task to register messages handled by the
  FMT LTE non-signaling processor to the message router

  @return
  TRUE if all registrations were successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean ftm_lte_ns_msgr_register( void );

/*===========================================================================

  FUNCTION:  ftm_lte_ns_deregister_msgr

===========================================================================*/
/*!
  @brief
  This function is used by FTM Task to register messages handled by the
  FMT LTE non-signaling processor to the message router


  @return
  TRUE if all de-registration were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean ftm_lte_ns_msgr_deregister( void );

/*===========================================================================

  FUNCTION:  ftm_lte_ns_proc_init  

===========================================================================*/
/*!
  @brief
  This function initializes the FTM LTE non-signaling processor internal 
  structure

  @param priority Thread priority passed in by App Mgr.

  @return
*/
/*=========================================================================*/
void ftm_lte_ns_proc_init( void );

/*===========================================================================

  FUNCTION:  ftm_lte_ns_proc_msgr_store

===========================================================================*/
/*!
  @brief
  This function is used by FTM Task to store the message router information
  into FTM LTE NS module for dynamic message registration.

  @param
  client_id : Client_ID that points to message router client
  id:         Mail-box ID 

  @return
  None
*/
/*=========================================================================*/
void ftm_lte_ns_proc_msgr_store( msgr_client_t *client_id, msgr_id_t id );

#endif /*  FTM_LTE_NS_PROC_H */

