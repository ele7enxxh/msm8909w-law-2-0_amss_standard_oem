#ifndef SECSSLALP_H
#define SECSSLALP_H

#include "sec_variation.h"
#ifdef FEATURE_SEC_SSL

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                S E C U R E   S O C K E T S   L A Y E R

               S S L   V 3   A L E R T   P R O T O C O L

GENERAL DESCRIPTION
  This file contains the SSL Alert Protocol.

EXTERNALIZED FUNCTIONS
  secsslalp_process_alert - Process a received alert message
  secsslalp_send_alert    - Send an alert message

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secsslalp.h_v   1.5   03 Sep 2003 11:08:22   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/ssl/shared/src/secsslalp.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/00   om      Several fixes (first functional version)
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "secerrno.h"

#include "secssldef.h"
#include "secssli.h"

/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/



/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECSSLALP_PROCESS_ALERT

DESCRIPTION
  This function processes an incoming alert.

DEPENDENCIES
  None

PARAMETERS
  session_ptr  - Pointer to an open SSL session
  alert_ptr    - Pointer to alert data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secsslalp_process_alert
(
  secssl_session_type*  session_ptr, 
  dsm_item_type**       alert_ptr
);


/*===========================================================================

FUNCTION SECSSLALP_SEND_ALERT

DESCRIPTION
  This function sends an alert.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr  - Pointer to an open SSL session
  alert_id     - Alert ID to send
  alert_level  - Level of alert (warning or fatal)

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void secsslalp_send_alert
(
  secssl_session_type*              session_ptr,
  ssl_v3_alp_alert_id_enum_type     alert_id,
  ssl_v3_alp_alert_level_enum_type  alert_level
);

#endif /* FEATURE_SEC_SSL */

#endif
