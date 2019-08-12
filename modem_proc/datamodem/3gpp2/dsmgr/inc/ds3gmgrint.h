#ifndef DS3GMGRINT_H
#define DS3GMGRINT_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

                   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This file contains definitions of 3G Dsmgr state variables, and some
  utility functions. This header file is intended to be used internal to the
  3G Dsmgr module only.


  Copyright (c) 2001 - 2011 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gmgrint.h_v   1.5   06 Nov 2002 10:47:16   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gmgrint.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/02/12   msh     Coan: Feature cleanup
08/09/11   sha     Global variable clean up.
03/23/11   sn      Globals cleanup for free floating of tasks.
11/22/10   ttv     Added support for DSDS.
11/21/04   rc      Changed SYS_MAX_PRIMARY_PDP_CONTEXTS to SYS_MAX_PDP_CONTEXTS.
07/12/04   dwp     Add 1 to DS3GI_MAX_NUM_CALLS for concurrent CS + PS calls.
12/29/03   vsk     define SYS_MAX_PRIMARY_PDP_CONTEXTS if not found : till this 
                   shows up in sys.h
10/07/03   vsk     Tie DS3GI_MAX_NUM_CALLS to Max # of PS/CS calls
07/25/03   ak      Updated call_info struct to include the mode of the call.
11/04/02   tmr     Added call_was_connected field to ds3gi_call_info_type
10/14/02   vsk     Added last_call_mode and last_call_type to track the 
                   previous calls mode & type : Reqd to retrieve stats for 
                   call which has ended
08/19/02   ak      Updated for multiple calls.
06/18/02   tmr     Added DS3G_WAIT_FOR_LL_DISCONNECT_STATE
06/05/02   ar      Removed online-command mode SIO watermark preservation 
                   variables.
05/21/02   smp     Replaced ds3g_mode_e_type with sys_sys_mode_e_type.
04/15/02   rc      Added ds_call_stack_e_type field to ds3gi_call_info_type
03/14/02   smp     Added end_result state variable to call_info.
02/26/02   tmr     Added DS3G_WAIT_FOR_LL_CONNECT_STATE
02/22/02   smp     Externalize variable to store mode-sp. handler
                   sio_rx_func_ptr.
02/19/02   rlp     Code review changes.
02/13/02   vsk     change call_id to be of cm_call_id_type
02/05/02   rlp     Added online-command mode support.
12/14/01   smp     Added state variables: ring_counter to call_info and
                   ri_asserted to serial_info.
08/17/01   smp     Renamed ATCOP files/functions/data types.
06/29/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dsm.h"
#include "cm.h"
#include "ds3gmgr.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  3G Dsmgr Call States
---------------------------------------------------------------------------*/ 
typedef enum
{
  DS3G_IDLE_CALL_STATE,               /* No data call active                */
  DS3G_ORIG_CALL_STATE,               /* Data call origination in progress  */
  DS3G_CALL_ACTIVE_CALL_STATE,        /* Data call currently active         */
  DS3G_RINGING_CALL_STATE,            /* User alert i.e. ringing in progress*/
  DS3G_WAIT_FOR_CONN_CALL_STATE,      /* Waiting for MT call to be connected*/
  DS3G_WAIT_FOR_LL_CONNECT_STATE,     /* In call connect delay state        */
  DS3G_WAIT_FOR_LL_DISCONNECT_STATE,  /* In call disconnect delay state     */
  DS3G_END_CALL_STATE,                /* Waiting for call to end            */
  DS3G_INVALID_CALL_STATE             /* an Invalid call state              */
} ds3gi_call_state_e_type;

/*---------------------------------------------------------------------------
  Maximum number of concurrent calls in the system.
  Depending on the target set to the allowed values : For GPRS/WCMDA enabled
  targets including multi mode, multiple PS calls can be allowed. Set it to
  that value. For all other targets set it to 1. 
  Add 1 (SYS_MAX_PDP_CONTEXTS + 1) for concurrent CS + PS support.
---------------------------------------------------------------------------*/
#if defined(FEATURE_GSM_GPRS) || defined(FEATURE_WCDMA)
#define DS3GI_MAX_NUM_CALLS  (SYS_MAX_PDP_CONTEXTS + 1)
#else
#define DS3GI_MAX_NUM_CALLS  SYS_MAX_PDP_CONTEXTS 
#endif

#define DS3GI_DEFAULT_CALL_INSTANCE  0
#define DS3GI_INVALID_CALL_INSTANCE  0xFFFF

/*---------------------------------------------------------------------------
  Type for 3G Dsmgr call related information
---------------------------------------------------------------------------*/ 
typedef struct
{
  cm_call_id_type            call_id;      /* Call id assigned by Call Mgr */
  ds_call_type_e_type        call_type;    /* Call type - circuit or pkt   */
  sys_sys_mode_e_type        call_mode;    /* Mode of call (WCDMA, 1x, ..) */
  ds3gi_call_state_e_type    call_state;   /* Call state                   */
  boolean                    call_was_connected; /* Call ever connected    */
  sys_modem_as_id_e_type     subs_id;     /* Sub on which call connected   */
} ds3gi_call_info_type;

/*---------------------------------------------------------------------------
  Table of function ptrs based on Mode (GSM/WCDMA/CDMA) and call type (pkt
  or circuit-switched)
---------------------------------------------------------------------------*/
extern ds3g_hdlr_func_tbl_type  *ds3gi_hdlr_tbl[SYS_SYS_MODE_MAX]
                                                 [DS_NUM_CALL_TYPES];

/*---------------------------------------------------------------------------
  Call state info.  
---------------------------------------------------------------------------*/
extern ds3gi_call_info_type  ds3gi_call_info[DS3GI_MAX_NUM_CALLS];


typedef struct ds3gi_subs_mode_s
{
  sys_modem_as_id_e_type  as_id; 
  sys_sys_mode_e_type     mode;
} ds3gi_subs_mode_type;

/*---------------------------------------------------------------------------
  The available system, with the service status which is indicated
  in the CM SS SRV_CHG cmd.
---------------------------------------------------------------------------*/
typedef struct 
{
  sys_sys_mode_e_type    ds3gi_mode;
  sys_srv_status_e_type  ds3gi_srv_status; 
  boolean                ps_data_suspend;
} ds3gi_available_system_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION      DS3G_GET_CALL_STATE

DESCRIPTION   This function returns the current call state.

DEPENDENCIES  None

RETURN VALUE  See above.

SIDE EFFECTS  None
===========================================================================*/
ds3gi_call_state_e_type ds3g_get_call_state
(
  cm_call_id_type  cm_call_id
);

/*===========================================================================
FUNCTION      DS3GI_ALLOC_CALL_INSTANCE_RECORD

DESCRIPTION   Allocates a call state element

DEPENDENCIES  None

RETURN VALUE  If found, returns the call instance index. 
              If not found, then returns DS3GI_INVALID_CALL INSTANCE

SIDE EFFECTS  None
===========================================================================*/
uint32 ds3gi_alloc_call_instance_record
(
  ds_call_type_e_type  call_type,
  sys_sys_mode_e_type      call_mode,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION      DS3GI_FREE_CALL_INSTANCE_RECORD

DESCRIPTION   Frees a call state element 

DEPENDENCIES  None
  
RETURN VALUE  None

SIDE EFFECTS  Sets the call state to DS3G_IDLE_CALL_STATE
===========================================================================*/
void ds3gi_free_call_instance_record
(
  uint32  call_instance
);

/*===========================================================================
FUNCTION      DS3GI_FIND_CALL_INSTANCE

DESCRIPTION   Given a call_id, will return the index into the call_state array.
              If no instance is found, then returns DS3GI_INVALID_CALL_INSTANCE

DEPENDENCIES  None

RETURN VALUE  If found, returns the call instance. 
              If not found, then returns DS3GI_INVALID_CALL INSTANCE

SIDE EFFECTS  None
===========================================================================*/
uint32 ds3gi_find_call_instance
(
  cm_call_id_type  cm_call_id
);
#endif /* DS3GMGRINT_H */
