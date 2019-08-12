#ifndef CMDRVCC_H
#define CMDRVCC_H
/*===========================================================================

         C A L L   M A N A G E R   C A L L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMCALL.C


Copyright (c) 1991 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmdrvcc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/14   xs      Reset ssac timer upon cell change

===========================================================================*/


#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "cmmmgsdi.h"
#include "lte_rrc_ext_msg.h"
                       /* Definition of LTE RRC SRV indication */

#if defined(FEATURE_CM_LTE)
#include "cm_esm.h"
#endif

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "cmxll.h"      /* MC to CM interface*/
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#ifdef CM_GW_SUPPORTED
#include "cmss.h"
#endif
#include "sys_v.h"

#include <stdarg.h>
#include "gstk_exp.h"

/** Enumeration of drvcc progress state 
*/
typedef enum cm_drvcc_prog_state_e {
  CM_DRVCC_STATE_NONE =-1,   
  /* when DRVCC is completed or in idle */
  
  CM_DRVCC_STATE_SETUP_IN_PROG,
  /* when DRVCC setup is on going */
  
  CM_DRVCC_STATE_MAX   
} cm_drvcc_prog_state;


typedef struct
{
  sys_drvcc_type_e_type drvcc_type;
  /* to store drvcc type during drvcc setup req */
  cm_drvcc_prog_state state;
  byte                        stn_nr[CM_MAX_STN_DIGITS];
  /* STN/VDN number to be used, max length can be CM_MAX_STN_DIGITS */
  boolean drvcc_allowed_in_roaming;
  /* drvcc is allowed or not in roaming, TRUE allowed FALSE not allowed */ 
} cmcall_drvcc_cache_s_type;


/*===========================================================================

FUNCTION cmcall_cmd_drvcc_setup_req_proc

DESCRIPTION

  This function handles the DRVCC setup command.
  This function sends the HO_START to clients.
  Fills the preferences for DRVCC call origination
  set the context in switch in call id.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_cmd_drvcc_setup_req_proc
(
  cm_call_cmd_s_type  *call_cmd_ptr                       /* pointer to a CM command */
);

/*===========================================================================

FUNCTION cmcall_drvcc_init_switch_in_call_object

DESCRIPTION

  initializes switch in call object 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_init_switch_in_call_object(cmcall_s_type *switch_in_call_ptr, cm_call_cmd_s_type *call_cmd_ptr);


/*===========================================================================

FUNCTION cmcall_drvcc_copy_fields_from_switch_out_to_switch_in_call

DESCRIPTION

  Copies the required fields from switch out call to switch in call 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_copy_fields_from_switch_out_to_switch_in_call(cmcall_s_type *switch_in_call_ptr,
                                                                cmcall_s_type *switch_out_call_ptr );



/*===========================================================================

FUNCTION cmcall_get_active_drvcc_volte_call

DESCRIPTION
  Find VOWIFI  call which is in active CONV state for DRVCC

  3GPP2 : Active Voice call is the one that is to be DRVCC'ed

DEPENDENCIES
  None

RETURN VALUE
  valid call id of call that is to be DRVCC'ed, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_get_active_drvcc_vowlan_call
(
   cm_call_type_mask_type         call_type_mask
        /* Call type to search */
);

/*===========================================================================

FUNCTION cmcall_trigger_drvcc_ho_fail_ipapp

DESCRIPTION
  Send DRVCC_HO_FAIL_NOTIFY to IMS APP

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_trigger_ho_fail_ipapp
(
   boolean orm_send_status, 
   sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION cmcall_drvcc_validate_active_call_in_context

DESCRIPTION
  To validate if switched out call id is part of context sent by IMS 

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if call id is part of context
  FALSE : if call id is not part of context sent by IMS

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_validate_active_call_in_context(cm_call_id_type call_id, cm_drvcc_setup_req_s_type *drvcc_setup_info_ptr);

/*===========================================================================

FUNCTION cmcall_drvcc_skip_call_event

DESCRIPTION
  This function tells if call events are to be suppressed for switch in and switch out call id
  when DRVCC is in progress
  1. Suppress Call events for switch in call, when DRVCC is in progress
  2. Suppress Call end event for switch out call id of WLAN, i.e. call that is being drvcc'ed

DEPENDENCIES
  None

RETURN VALUE
  True : Skip sending call event
  False : Do not skip sending call event.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_skip_call_event(cm_call_event_e_type call_event, cmcall_s_type *call_ptr);



/*===========================================================================

FUNCTION cmcall_drvcc_is_valid_drvcc_type

DESCRIPTION
  To check if drvcc type is valid

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_is_valid_drvcc_type(sys_drvcc_type_e_type drvcc_type);

/*===========================================================================

FUNCTION cmcall_drvcc_is_3gpp2_drvcc_type

DESCRIPTION
  To check if drvcc type is 3gpp2 

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_is_3gpp2_drvcc_type(sys_drvcc_type_e_type drvcc_type);


/*===========================================================================

FUNCTION cmcall_drvcc_set_favored_mode_pref_for_setup

DESCRIPTION
  sets favored mode preference for DRVCC setup call 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if favored mode and hybrid preference were computed successfully,
  in which case they are written into fav_mode_pref_ptr and
  fav_hybr_pref_ptr, FALSE otherwise.

  If the function retured FALSE then CM wrote the values of CM_HYBR_PREF_NONE
  and CM_MODE_PREF_NONE into fav_hybr_pref_ptr and fav_mode_pref_ptr

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_set_favored_mode_pref_for_setup (cm_ss_e_type ss,
                                                      sys_drvcc_type_e_type drvcc_type,
                                                      cm_mode_pref_e_type *fav_mode_pref_ptr,
                                                      cm_hybr_pref_e_type *fav_hybr_pref_ptr );


/*===========================================================================

FUNCTION cmcall_drvcc_determine_mode_usage

DESCRIPTION
  To determine mode usage for the drvcc setup call

DEPENDENCIES
  None

RETURN VALUE
  cm_cc_mode_pref_e_type to be used for DRVCC call setup, as per drvcc type

SIDE EFFECTS
  none

===========================================================================*/
cm_cc_mode_pref_e_type cmcall_drvcc_determine_mode_usage(sys_drvcc_type_e_type drvcc_type);


/*===========================================================================

FUNCTION cmcall_drvcc_get_setup_call_drvcc_type

DESCRIPTION
  To determine if call in concern is drvcc setup call

DEPENDENCIES
  None

RETURN VALUE
  sys_drvcc_type_e_type what kind of drvcc call type is in progress.

SIDE EFFECTS
  none

===========================================================================*/
sys_drvcc_type_e_type cmcall_drvcc_get_setup_call_drvcc_type(cmcall_s_type *call_ptr);


/*===========================================================================

FUNCTION cmcall_drvcc_send_drvcc_orig

DESCRIPTION
  To send call orig for drvcc

DEPENDENCIES
  None

RETURN VALUE
  cm_cc_mode_pref_e_type to be used for DRVCC call setup, as per drvcc type

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_send_drvcc_orig(cmcall_s_type *call_ptr, sys_drvcc_type_e_type drvcc_type);

/*===========================================================================

FUNCTION cmcall_drvcc_send_drvcc_end

DESCRIPTION
  To send DRVCC End 

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_send_drvcc_end(cmcall_s_type *call_ptr);

/*===========================================================================

FUNCTION cmcall_drvcc_send_xdrvcc_end

DESCRIPTION
  To send DRVCC_END_F for ending DRVCC setup

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_send_xdrvcc_end(cmcall_s_type *call_ptr);




/*===========================================================================

FUNCTION cmcall_drvcc_send_xdrvcc_orig

DESCRIPTION
  To send call orig for drvcc

DEPENDENCIES
  None

RETURN VALUE
  cm_cc_mode_pref_e_type to be used for DRVCC call setup, as per drvcc type

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_send_xdrvcc_orig(cmcall_s_type *call_ptr);

/*===========================================================================

FUNCTION cmcall_drvcc_check_and_abort_drvcc_setup

DESCRIPTION
  To check if the call being ended shall abort the DRVCC setup
  If call being ended is switch out call ID, and DRVCC_SETUP_MSG has already been sent to 1X
  then End drvcc setup call over 1X.
  Send DRVCC FAIL to IMS.
  and HO_FAIL call event to CM clients for switch out call id.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_check_and_abort_drvcc_setup(cmcall_s_type *call_ptr);

/*===========================================================================

FUNCTION cmcall_drvcc_abort_drvcc_setup_due_to_emerg_orig

DESCRIPTION
  Due to E911 call origination need to abort DRVCC process

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_abort_drvcc_setup_due_to_emerg_orig(void);


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) 

/*===========================================================================

FUNCTION cmcall_drvcc_fill_stn_nr

DESCRIPTION

 Fills STN Number in orig drvcc request.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_fill_stn_nr(
    /* Pointer to MC Orig Param */
  mc_origination_type    *orig_param
);

#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

/*===========================================================================

FUNCTION cmcall_drvcc_get_voice_ho_type_from_drvcc_type

DESCRIPTION
  To determine voice_ho_type from drvcc_type

DEPENDENCIES
  None

RETURN VALUE
  cm_cc_mode_pref_e_type to be used for DRVCC call setup, as per drvcc type

SIDE EFFECTS
  none

===========================================================================*/
sys_voice_handover_e_type cmcall_drvcc_get_voice_ho_type_from_drvcc_type(sys_drvcc_type_e_type drvcc_type);



/*===========================================================================

FUNCTION cmcall_drvcc_cache_init

DESCRIPTION

  initialize drvcc cache, and will be used to reset as well once process completes.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_cache_init(void);

/*===========================================================================

FUNCTION cmcall_drvcc_fill_cache_from_setup_info

DESCRIPTION

  Fills DRVCC cache from drvcc setup info coming from IMS.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_fill_cache_from_setup_info(cm_drvcc_setup_req_s_type *drvcc_setup_info_ptr);


/*===========================================================================

FUNCTION cmcall_drvcc_cache_update_drvcc_type

DESCRIPTION

  updaets drvcc type in drvcc cache.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_cache_update_drvcc_type(sys_drvcc_type_e_type drvcc_type);

/*===========================================================================

FUNCTION cmcall_drvcc_cache_get_drvcc_type

DESCRIPTION

  returns drvcc type in drvcc cache.

DEPENDENCIES
  none

RETURN VALUE
  drvcc type in drvcc cache

SIDE EFFECTS
  none

===========================================================================*/
sys_drvcc_type_e_type cmcall_drvcc_cache_get_drvcc_type(void);

/*===========================================================================

FUNCTION cmcall_drvcc_cache_get_drvcc_state

DESCRIPTION

  returns state in drvcc cache

DEPENDENCIES
  none

RETURN VALUE
  drvcc state in drvcc cache

SIDE EFFECTS
  none

===========================================================================*/
cm_drvcc_prog_state cmcall_drvcc_cache_get_drvcc_state(void);

/*===========================================================================

FUNCTION cmcall_drvcc_cache_update_state

DESCRIPTION

  Updates state of drvcc cache.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_drvcc_cache_update_state(cm_drvcc_prog_state drvcc_state);


/*===========================================================================

FUNCTION cmcall_drvcc_send_x_audio_rel_ind

DESCRIPTION
  To send audio release ind to 1XCP

DEPENDENCIES
  None

RETURN VALUE
  cm_cc_mode_pref_e_type to be used for DRVCC call setup, as per drvcc type

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_send_x_audio_rel_ind(void);

/*===========================================================================

FUNCTION cmcall_drvcc_is_drvcc_allowed_per_roam_status

DESCRIPTION

  returns TRUE if DRVCC is allowed or not per Roam status and custom home status

DEPENDENCIES
  none

RETURN VALUE
  drvcc state in drvcc cache

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_is_drvcc_allowed_per_roam_status(sys_srv_status_e_type srv_status,
                        sys_roam_status_e_type roam_status,
                        boolean is_cdma_custom_home);

/*===========================================================================

FUNCTION cmcall_drvcc_cache_get_drvcc_allowed_in_roaming

DESCRIPTION

  returns drvcc allowed in roaming state from drvcc cache

DEPENDENCIES
  none

RETURN VALUE
  drvcc state in drvcc cache

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_drvcc_cache_get_drvcc_allowed_in_roaming(void);


#endif //CMDRVCC_H


