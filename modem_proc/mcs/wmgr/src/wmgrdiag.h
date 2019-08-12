#ifndef WMGRDIAG_H
#define WMGRDIAG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                    W A K E U P    M A N A G E R

             Wakeup Manager Diag Interface Header File

GENERAL DESCRIPTION

  This file provides the interface for wakeup manager with diag.


EXTERNALIZED FUNCTIONS

  None


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None
  

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

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wmgr/src/wmgrdiag.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     --------------------------------------------------------- 
01/10/2012   ag      WMGR Feature Cleanup  
07/06/2011   sg      Initial Revision

============================================================================*/


/*============================================================================

                         INCLUDE FILES FOR MODULE

============================================================================*/

#include "log_codes.h"
#include "log.h"
#include "diagpkt.h"
#include "wmgri.h"

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/*----------------------------------------------------------------------------
  Wakeup Manager information for logging
    - This structure holds the wakeup mgr info for logging
    - This structure is PACKED so there is no padding between members
    - Maintain the members in decending order of size for efficiency
----------------------------------------------------------------------------*/

typedef PACK( struct )
{

  /* Time              */
  uint64          time;
    
  /* Client ID         */
  uint8           client;

  /* Client Chain      */
  uint8           chain;

  /* Client State      */
  uint8           state;

  /* Result            */
  uint8           result;

  /* Result            */
  int8            group;

} wmgr_diag_client_log_type;


/*----------------------------------------------------------------------------
  Diag Log packet type
    - This structure holds the wakeup mgr info that will be sent to diag
    - This structure is PACKED so there is no padding between members
----------------------------------------------------------------------------*/
typedef PACK( struct )
{

  /* Diag Log header                         */
  log_hdr_type                hdr;

  /* Log version                             */
  uint8                       version;
  
  /* Current log data of client              */
  wmgr_diag_client_log_type   client_curr_info;

  /* Previous log data of client             */
  wmgr_diag_client_log_type   client_prev_info;

} LOG_WMGR_C_type;



/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*===========================================================================

FUNCTION WMGR_DIAG_SEND_CLIENT_LOG

DESCRIPTION
  This function will dump the wakeup manager information to an external
  logging interface i.e DIAG.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If the log was sent to diag
  FALSE - If the logging failed
  
SIDE EFFECTS
  None
  
===========================================================================*/
boolean wmgr_diag_send_client_log
(
  /* Current information of client */
  wmgr_client_type *client_curr_data,

  /* History of client information */
  wmgr_client_type *client_prev_data

);


#endif /* WMGRDIAG_H */

