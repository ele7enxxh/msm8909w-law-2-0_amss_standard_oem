/*!
  @file msgr_msg.h

  @brief Contains message definitions for App Manager
  
*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/msgr_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/09/10   zm      Moved file into mcs/api folder - SU API effort.
06/21/10   gbs     added MCS_MSGR_SMDL_INIT_INDI UMID
12/07/07   enj     Initial Revision

===========================================================================*/

#ifndef __MSGR_MSG_H__
#define __MSGR_MSG_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* UMB App definitions for the ID portion of the UMID's */

/*-----------------------------------------------------------------------*/
/*     MSGR SUPERVISORY ID'S                                             */
/*-----------------------------------------------------------------------*/

/*! @brief ID for snoop registration and deregistration
 */
#define MSGR_SNOOP_ID                   0x01
/*! @brief ID for All MQ's blocked message
 */
#define MSGR_ALL_MQS_BLOCKED_ID         0x02
/*! @brief ID for SMDL initialization 
  */
#define MSGR_SMDL_INIT_ID                0x03


/*-----------------------------------------------------------------------*/
/*     MSGR Supervisory UMID's                                           */
/*-----------------------------------------------------------------------*/

/*! @brief Enumeration of Message Router Supervisory UMID's
 */
enum {
  /*! Snooper */
  MSGR_DEFINE_UMID( MCS, MSGR, SPR, SNOOP, MSGR_SNOOP_ID, msgr_hdr_s ),
  /*! All POSIX MQ's blocked */
  MSGR_DEFINE_UMID( MCS, MSGR, SPR, ALL_MQS_BLOCKED, MSGR_ALL_MQS_BLOCKED_ID, msgr_hdr_s ),
  /*! SMDL initialization */
  MSGR_DEFINE_UMID( MCS, MSGR, INDI, SMDL_INIT, MSGR_SMDL_INIT_ID, msgr_hdr_s )
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR MSGR COMMANDS                                 */
/*-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR MSGR RESPONSES                             */
/*-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR MSGR SUPERVISORY'S                         */
/*-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR MSGR INDICATIONS                           */
/*-----------------------------------------------------------------------*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* __MSGR_MSG_H__ */
