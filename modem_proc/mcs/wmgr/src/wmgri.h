#ifndef WMGRI_H
#define WMGRI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                    W A K E U P    M A N A G E R

                Wakeup Manager Internal Header File

GENERAL DESCRIPTION

  This header file is used only inside wakeup manager module.


EXTERNALIZED FUNCTIONS

  None


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None 
  

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wmgr/src/wmgri.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
03/05/2015   mn      Wakeup Manager sends redundant wakeup cancel callbacks 
                      to 1x. (CR: 801007).
10/23/2014   mn      Changed to status info to global variable. (CR: 719311).
10/23/2014   mn      Add remaining RAT groups in the RAT group array (CR: 740277).
02/21/2014   mn      WMGR should recognize GSM subs as the same tech.
11/26/2013   mn      Fixing an issue with WMGR ignoring collision
                     clients for a subsequent wakeup client.
08/10/2013   mn      Fixing duplicate callbacks being sent by WMGR.
03/26/2013   mn      Adding support for Wakeup Manager on Triton DSDS.
05/23/2011   sg      Initial Revision

============================================================================*/


/*=========================================================================== 
 
                     INCLUDE FILES FOR MODULE
 
===========================================================================*/
#include "wmgr.h"
#include "trm.h"
#include <comdef.h>
#include <timetick.h>
#include "trm_wmgr.h"
#include "msg.h"

 
/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/* Critical section macros */
#define WMGR_INIT_CRIT_SECT(lock)    rex_init_crit_sect(&(lock))
#define WMGR_ENTER_CRIT_SECT(lock)   rex_enter_crit_sect(&(lock))
#define WMGR_LEAVE_CRIT_SECT(lock)   rex_leave_crit_sect(&(lock))

/* macro to prevent compiler warnings when the payload parameter is not used
   by state entry, exit, and transition functions, or when a variable is only
   used in MSG_XXXX calls that may be compiled out */
#define WMGR_NOTUSED(i)              if(i){}

#define WMGR_COMPILE_ASSERT(a)       switch(0){case 0: case (a):;}

/*----------------------------------------------------------------------------
  Wakeup Manager Client State
    - This enum will indicate the state of Wakeup manager client
    - The clients can be scheduled for wakeup, already awake or neither 
----------------------------------------------------------------------------*/
typedef enum
{
  /* Client is neither active nor scheduled to wakeup */
  WMGR_CLIENT_STATE_RELEASED,

  /* Client is active on one of the TRM chains */
  WMGR_CLIENT_STATE_LOCKED,
    
  /* Client is scheduled for wakeup on one of the TRM chains */
  WMGR_CLIENT_STATE_RESERVED,

  /* Client is colliding with a higher priority client */
  WMGR_CLIENT_STATE_COLLISION,
  
  /* Initial state and any error in client state */
  WMGR_CLIENT_STATE_ERROR

} wmgr_client_state_type;

/*----------------------------------------------------------------------------
  Wakeup Manager Client Groups
----------------------------------------------------------------------------*/
typedef enum
{
  WMGR_RAT_GROUP_1X,

  WMGR_RAT_GROUP_HDR,

  WMGR_RAT_GROUP_GERAN,

  WMGR_RAT_GROUP_WCDMA,

  WMGR_RAT_GROUP_TDSCDMA,

  WMGR_RAT_GROUP_LTE,

  WMGR_RAT_GROUP_NONE,

  WMGR_RAT_GROUP_MAX = WMGR_RAT_GROUP_NONE
} wmgr_rat_group_type;


/*----------------------------------------------------------------------------
  TRM information that Wakeup Manager needs
    - This structure holds the TRM information for each client
    - Wakeup Manager will use the information in this structure to determine
      next wakeups, collisions and L1 transitions
----------------------------------------------------------------------------*/
typedef struct
{
  /* TRM group allocated to the client */
  int8               category;
  
  /* TRM chain allocated to the client */
  uint8              chain;
  
  /* TRM Client ID */
  trm_client_enum_t  client;
  
  /* The TRM chain reservation time 
     (measured from current time) */
  trm_time_t         time;
  
  /* The duration of TRM reservation */
  trm_duration_t     duration;

  /* The absolute reservation time */
  trm_timestamp_t    reservation_time;
    
} wmgr_trm_client_info_type;

/*----------------------------------------------------------------------------
  Wakeup Manager Old Info Structure
    - This structure is used to store old information about a client so that
      we can verify on next iteration if result has changed and a callback
      is needed to the client. That is, this is used to track duplicate
      result callbacks.  
----------------------------------------------------------------------------*/
typedef struct
{
  /* Wake up manager result for the client */
  wmgr_result_type           result;

  /* The TRM chain reservation time */
  trm_timestamp_t            reservation_time;

  /* The duration of TRM reservation */
  trm_duration_t             duration;

  /* The collision type */
  wmgr_collision_enum_type   collision_type;

} wmgr_old_info_type;


/*----------------------------------------------------------------------------
  Structure to store old status information.
----------------------------------------------------------------------------*/
typedef struct
{
  uint32   num_devices_allotted;
  uint32   num_status_info_clients;
} wmgr_old_status_info_type;

/*----------------------------------------------------------------------------
  Wakeup Manager Client Structure
    - This structure holds the information for each client of wake up manager
    - This structure will be used for the client state machine   
----------------------------------------------------------------------------*/
typedef struct
{
  /* ID of the Client RAT */
  trm_client_enum_t         id;

  /* Is client registered ? */
  boolean                    registered;
  
  /* State of the Client RAT */
  wmgr_client_state_type     state;
  
  /* Wake up manager result for the client */
  wmgr_result_type           result;
  
  /* Denotes if collision is detected and 
     the type of collision  */
  wmgr_collision_enum_type   collision_type;

  /* The Layer1 transitions */
  uint32                     transition;
  
  /* Wakeup Call back of the client */
  wmgr_rat_cb_type           cb;
  
  /* Client data pointer passed by client */
  void                       *client_data;

  /* TRM information of the client RAT */
  wmgr_trm_client_info_type  trm_info;

  /* Structure to hold the old info */
  wmgr_old_info_type         old_info;

  /* Client group */
  wmgr_rat_group_type        rat_group;
} wmgr_client_type; 


/*----------------------------------------------------------------------------
  Wakeup Manager History Structure
    - This structure holds the history of each chain
    - This structure will aid to calculate
        L1 transitions,
        RF warmup and
        MDSP download
----------------------------------------------------------------------------*/
typedef struct
{
  /* Holds the previous chain owner.
     This is used to check for lock release */
  trm_cur_info_type   prev_history[TRM_MAX_RF_CHAINS];

  /* Holds the previous active client on any chain.
     If the client releases the chain it still maintains
     the last active client inforamtion. Used for RF warmup */
  trm_cur_info_type   prev_rf_client[TRM_MAX_RF_CHAINS];

} wmgr_history_info_type;

/*----------------------------------------------------------------------------
  TRM Internal structure to hold all the status info clients information 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Is client registered */
  boolean                         registered; 
  
  /* Client ID */
  wmgr_status_info_client_enum_t  client;

  /* Call back pointer */
  wmgr_status_info_cb_type        cb;

  /* Old Status Information that was sent */
  wmgr_old_status_info_type  old_status_info;

} wmgr_status_info_clients_type;

/*----------------------------------------------------------------------------
  Wakeup Manager To TRM Client Identifier Translation Structure
    - This structure holds the mapping between WMGR and TRM client identifiers.
----------------------------------------------------------------------------*/
typedef struct
{
  trm_client_enum_t   trm_cid;
  wmgr_client_enum_t  wmgr_cid;
  wmgr_rat_group_type rat_group;
} wmgr_trm_client_xlate_type;


/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/


/*============================================================================

FUNCTION WMGR_TRM_CB_FN

DESCRIPTION
  This is a call back function that is registered with TRM.
  TRM informs WMgr with the current chain owner information and
  list of all the clients that have a reservation in place.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function triggers the WMgr state machine.
  The time taken in this function should be kept to minimal.

============================================================================*/
void wmgr_trm_cb_fn
( 
  /* Client ID for the call back */
  trm_wakeup_client_enum_t client,
  
  /* TRM information */
  trm_wakeup_info_type     *trm_info
);


/*============================================================================

FUNCTION WMGR_TRM_PARSE_CURR_DATA

DESCRIPTION
  This function reads the current chain information returned by TRM.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  After the current chain information is procesed it copied over to
  the previous chain information structure.

============================================================================*/
void wmgr_trm_parse_curr_data
(
  /* TRM information */
  trm_wakeup_info_type *trm_info
);


/*============================================================================

FUNCTION WMGR_TRM_PARSE_WAKEUP_DATA

DESCRIPTION
  This function parses the trm information to collate information about
  next wakeup and inform the result to the client
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Wakeup Call back will be called if the client is registered

============================================================================*/
void wmgr_trm_parse_wakeup_data 
(
  /* TRM information */
  trm_wakeup_info_type *trm_info
);


/*============================================================================

FUNCTION WMGR_COMPUTE_WAKEUP_RESULT

DESCRIPTION
  This function calculates the Layer1 transitions and determines
  the result that needs to be communicated to Layer1s
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the result has changed
  FALSE - if the result is same as what was communicated earlier

SIDE EFFECTS
  None

============================================================================*/
boolean wmgr_compute_wakeup_result
(
  /* Client ID */
  trm_client_enum_t  cid,

  /* Chain */
  uint8              chain,

  /* Future info */
  trm_fut_info_type* fut_info
);


/*============================================================================

FUNCTION WMGR_COMPUTE_COLLISION_RESULT

DESCRIPTION
  This function determines if there is any change in the collision info
  and if the result  needs to be communicated to Layer1s
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the result has changed
  FALSE - if the result is same as what was communicated earlier

SIDE EFFECTS
  None

============================================================================*/
boolean wmgr_compute_collision_result
(
  /* Client ID */
  trm_client_enum_t cid,

  /* Chain */
  int8              group,

  /* Time */
  trm_time_t        time,

  /* Future info */
  trm_fut_info_type* fut_info
);


/*============================================================================

FUNCTION WMGR_INFORM_WAKEUP_CLIENT

DESCRIPTION
  Computes the result and calls the registered client call back
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void wmgr_inform_wakeup_client
(
  /* TRM Information */
  trm_wakeup_info_type *trm_info,

  /* Index of the wakeup client */
  uint8                wcindex
);


/*============================================================================

FUNCTION WMGR_INFORM_COLLISION_CLIENT

DESCRIPTION
  Informs the client about the collision using the registered call back
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void wmgr_inform_collision_client
(
  /* TRM information */
  trm_wakeup_info_type *trm_info,

  /* Count of the collision clients */
  uint8                ccount,

  /* Array of indexes of the collision clients */
  uint8                *ccindex
);

/*============================================================================

FUNCTION WMGR_RESET_CLIENT_INFO

DESCRIPTION
  This function resets the client info structure for the indicated client.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void wmgr_reset_client_info
(
  /* Client ID */
  trm_client_enum_t cid  
);

/*============================================================================

FUNCTION WMGR_COMPUTE_COLLISION_TYPE

DESCRIPTION
  Computes if collision will happen and the type of collision 
  given the collision client information and wakeup client information.
  
DEPENDENCIES
  None

RETURN VALUE
  Type of collision

SIDE EFFECTS
  None

============================================================================*/
wmgr_collision_enum_type wmgr_compute_collision_type
(
  const trm_fut_info_type* cc_info,
  const trm_fut_info_type* wc_info      
);

/*===========================================================================

FUNCTION WMGR_PROCESS_STATUS_INFO

DESCRIPTION
  This function will return the current wakeup information that is maintained
  at Wakeup Manager. 
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void wmgr_process_status_info( void );

/*============================================================================

FUNCTION WMGR_INFORM_STATUS_INFO_CLIENTS

DESCRIPTION
  This function translates a given WMGR client Id to a TRM client Id.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void wmgr_inform_status_info_clients( void );

/*============================================================================

FUNCTION WMGR_XLATE_WMGR_CLIENT_TO_TRM_CLIENT

DESCRIPTION
  This function translates a given WMGR client Id to a TRM client Id.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
trm_client_enum_t wmgr_xlate_wmgr_client_to_trm_client( wmgr_client_enum_t wmgr_id );

/*============================================================================

FUNCTION WMGR_XLATE_TRM_CLIENT_TO_WMGR_CLIENT

DESCRIPTION
  This function translates a given TRM client Id to a WMGR client Id.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
wmgr_client_enum_t wmgr_xlate_trm_client_to_wmgr_client( trm_client_enum_t trm_id );

#endif /* WMGRI_H */

