#ifndef CM_EMGEXT_H
#define CM_EMGEXT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   C A L L   M A N A G E R  LTE Emergency call 

GENERAL DESCRIPTION
  This module contains LTE emergency call access for remote IMS.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmdbm_init() must be called to initialize this module before any other
  function declared in this module is being called.

Copyright (c) 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmemgext.h#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
04/29/13    xs      Initial version

===========================================================================*/
#include "comdef.h"
#include "cmll_v.h"

#define EXT_E911_CALL_ID 0xFE

typedef struct
{
  boolean is_ready;
  cm_ac_reason_e_type reason;
  boolean is_redial;
}cmemg_s_type;


/*===========================================================================

FUNCTION cmemg_get_act_id

DESCRIPTION
  Return act id for access control  

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
uint32 cmemg_get_act_id (cm_ss_e_type cm_ss);

/*===========================================================================

FUNCTION cmemg_init

DESCRIPTION
  Access control initialize

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmemg_init( void );
/*===========================================================================

FUNCTION cmemg_msgr_cmd_proc

DESCRIPTION
  Process reports from the lower layers.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmemg_msgr_cmd_proc(

  const void   *cmd_ptr
    /* Pointer to the report sent by Message Router */
);

/*===========================================================================

FUNCTION cmemg_sd_rpt_proc

DESCRIPTION
  Process System Determination reports


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmemg_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD reports */
);
/*===========================================================================

FUNCTION cmemg_exit_emgergency_state

DESCRIPTION
  Request access control to exit emergency state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmemg_exit_emgergency_state (
  boolean                  is_ecbm_required, 
  void                    *user_data,
  sys_modem_as_id_e_type   asubs_id
);

/*===========================================================================

FUNCTION cmemg_get_reason

DESCRIPTION
  Return the reason for access control

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_ac_reason_e_type cmemg_get_reason(void);



/*===========================================================================

FUNCTION cmemg_send_call_mode_ind

DESCRIPTION
 Sends Volte only mode indication to clients via msgr


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmemg_send_call_mode_ind(sys_modem_as_id_e_type subs_id, sys_call_mode_t call_mode, boolean force_send);

/*===========================================================================

FUNCTION cmemg_get_reason_per_sub

DESCRIPTION
  Return the reason for access control for the specified sub.

DEPENDENCIES
  None

RETURN VALUE
  cm_ac_reason_e_type

SIDE EFFECTS
  none

===========================================================================*/
cm_ac_reason_e_type cmemg_get_reason_per_sub(
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION cmemg_is_ext_e911_mode

DESCRIPTION
  Return if UE in third party E911 mode.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmemg_is_ext_e911_mode(void);

/*===========================================================================

FUNCTION cmemg_handle_esr_rsp

DESCRIPTION
 Handles the ESR response


DEPENDENCIES
  none

RETURN VALUE
  none
===========================================================================*/
void cmemg_handle_esr_rsp(uint8 call_id);

/*===========================================================================

FUNCTION cmemg_set_reason_per_sub

DESCRIPTION
  Set the reason for access control for the specified sub.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmemg_set_reason_per_sub(
  cm_ac_reason_e_type     res,
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION cmemg_is_redial

DESCRIPTION
  Returns if e911 enter is for redial with LTE
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmemg_is_redial(sd_ss_e_type ss);

#endif
