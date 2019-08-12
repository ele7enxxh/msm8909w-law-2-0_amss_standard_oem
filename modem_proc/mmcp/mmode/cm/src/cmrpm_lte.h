#ifndef CMRPM_LTE_H
#define CMRPM_LTE_H
/*===========================================================================

C A L L   M A N A G E R   RADIO POLICY MANAGER   LTE   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMRPM_LTE.C

Copyright (c) 2014 - 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/15   sk      Integrated RPM feature

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "sys.h"       /* System wide definitions */
#include "sys_v.h"
#include "cm_msgr_msg.h"
#include "emm_ext_msg.h"

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/
#define CMRPM_SEC_PER_MIN   60    /* seconds in min */
#define CMRPM_SEC_PER_HOUR  3600  /* seconds in hour */
#define CMRPM_LTE_VERSION   0x01      /* Indicates RPM Version supported */

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

typedef struct cmrpm_lte_s{
  
  boolean          is_rpm_enabled;
  /* Indicate whether RPM functionality should be enabled or not */
  
  uint32          sys_time_efs;
  /* Current system time value read from EFS at power-up*/

  boolean         is_coin_cell_support;
  /* Indicate whether UE have coin cell battery support */

  uint32                   event_timer; 
  /* Timer for tracking RPM EMM reject event. Will store time in secs */

  uint8                    max_event_counter;
  /* Maximum allowable number for LTE RPM EMM reject event */

  uint16                   max_event_timer;
  /* Maximum allowable event tracking time for LTE RPM */

  uint8                    emm_gen_counter;
  /* Counter for received EMM reject cause 3/6/7/8 from NAS; 
   * EMM reject cause 15 will be handled in emm_15_counter
   */

  cmrpm_lte_emm_15_s_type  emm_15_reject[CM_MAX_NUM_OF_TAC];
  /* Per-TAC counter and [tac, plmn] pair for received 
   * EMM reject cause 15 from NAS 
   */

  uint8                    last_emm_reject_cause;
  /* Stores the last EMM reject cause */
    
}cmrpm_lte_s_type;


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmrpm_is_rpm_enabled

DESCRIPTION
  Returns whether rpm is enabled, based on the rpm pointer.
  Meant for use by external functions that don't have much to do with RPM
  but need to know if it is enabled.

DEPENDENCIES
  cmrpm_init should be called before this function.

RETURN VALUE
  TRUE if the rpm pointer indicates that rpm is enabled
  FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmrpm_lte_is_rpm_enabled( void );

/*===========================================================================

FUNCTION cmrpm_ptr

DESCRIPTION
  Return a pointer to the one and only RPM object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cmrpm_lte_s_type *cmrpm_lte_ptr( void );


/*===========================================================================

FUNCTION cmrpm_init

DESCRIPTION
  Initializes the RPM object.

  This function must be called before the before RPM object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmrpm_lte_init( void );

/*===========================================================================

FUNCTION cmrpm_lte_set_rpm_parameters_req

DESCRIPTION
  Updates value of  CM RPM parameters and updates the NV as well.
  If there is any RPM timer running currently it will incorporate new value.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_lte_set_rpm_parameters_req( cm_mm_lte_rpm_parameters_s_type set_cm_rpm_parameters_req);


/*===========================================================================

FUNCTION cmrpm_lte_send_get_rpm_parameters_req_rsp

DESCRIPTION
  Sends the current value of the CM RPM parameters  to 
  CM clients via MSGR interface MM_CM_GET_RPM_PARAMETERS_RSP.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_lte_send_get_rpm_parameters_rsp( void );

/*===========================================================================

FUNCTION cmrpm_lte_set_rpm_config_req

DESCRIPTION
  Updates value of  CM RPM config and updates the EFS as well.
  If there is any RPM timer running currently it will be stopped on 
  disabling rpm config.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_lte_set_rpm_config_req( boolean set_rpm_config_req);

/*===========================================================================

FUNCTION cmrpm_lte_app_reset_rejected_proc

DESCRIPTION
  Processes PH cmd from CM when app reset was rejected 

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/
extern void cmrpm_lte_app_reset_rejected_proc( void );


/*===========================================================================

FUNCTION cmrpm_lte_check_reset_allowed

DESCRIPTION
  Check whether reset is allowed or not when RPM is on

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  CM_PH_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean  cmrpm_lte_check_reset_allowed(
  void
);

/*===========================================================================

FUNCTION cmrpm_lte_process_app_reset

DESCRIPTION
  Process oprt mode change

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
extern boolean  cmrpm_lte_process_app_reset(
  cm_ph_cmd_s_type           *ph_cmd_ptr
);

/*===========================================================================

FUNCTION cmrpm_lte_timer_proc

DESCRIPTION
  Process timer for RPM.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmrpm_lte_timer_proc(
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
);


/*===========================================================================

FUNCTION cmrpm_lte_emm_tai_list_ind_proc

DESCRIPTION
  Update the TAI info based on NAS_EMM_TAI_LIST_IND.

DEPENDENCIES

None

RETURNS
  None

SIDE_EFFECT

None

===========================================================================*/
extern void cmrpm_lte_emm_tai_list_ind_proc(
    emm_tai_list_ind_type  *emm_tai_list_ind
    /* Pointer to emm tai list ind  */    
);

/*===========================================================================

  FUNCTION cmrpm_lte_sd_rpt_proc

  DESCRIPTION
    Process reports from System Determination.

  DEPENDENCIES
    RPM object must have already been initialized with cmrpm_lte_init().

  RETURN VALUE
    none

  SIDE EFFECTS
    none

===========================================================================*/
extern void cmrpm_lte_sd_rpt_proc(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
);

/*===========================================================================

FUNCTION cmrpm_lte_rpt_proc

DESCRIPTION
  Process LL reports related to RPM

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmrpm_lte_rpt_proc(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to a LL report */
);


/*===========================================================================

FUNCTION cmrpm_lte_write_efs_sys_time

DESCRIPTION
   This function writes system time at power-down in EFS

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmrpm_lte_write_efs_sys_time(

  sys_oprt_mode_e_type  prev_oprt_mode,

  sys_oprt_mode_e_type  curr_oprt_mode
);


/*===========================================================================

FUNCTION cmrpm_lte_read_efs_sys_time

DESCRIPTION
 This function reads system time from EFS, that was written at last power-down 
 And updates local variable sys_time_efs with the time read from EFS.
 If time read from EFS is greater than uptime returned by API  
 cm_util_get_curr_systime(), we can consider coin cell battery as absent and
 use EFS itself for maintaining time across resets.

DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmrpm_lte_read_efs_sys_time( boolean );

/*===========================================================================

FUNCTION cmrpm_lte_reset_efs_info

DESCRIPTION
  Reset the EFS info items, and write to EFS.
  
DEPENDENCIES
  RPM object must have already been initialized with cmrpm_lte_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmrpm_lte_reset_efs_info( void );

/*===========================================================================

FUNCTION  cmrpm_lte_send_update_ftai_list

DESCRIPTION
 The function sends the update FTAI List request to MM


DEPENDENCIES
 none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
extern void cmrpm_lte_send_update_ftai_list(sys_rpm_ftai_list_action_e_type action);

#endif /* CMRPM_LTE_H */

