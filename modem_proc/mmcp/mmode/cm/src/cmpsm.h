#ifndef CMPSM_H
#define CMPSM_H

/*===========================================================================

          PSM    H E A D E R    F I L E

DESCRIPTION
  This file contains all the definitions necessary for the psm module.

Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmpsm.h#1 $

when       who     what, where, why
--------   ---    -------------------------------------------------------
09/19/15   skk     Initial release.

===========================================================================*/



#include "sys.h"
#include "cmi.h"
#include "cmidbg.h"
#include "cm_msgr_msg.h"
#include "fs_public.h"
#include "cmefs.h"
#include "comdef.h"

#include "sd.h"
#include "cmss.h"

/* PSM OPTIMIZATION MASKS
*/
#define CM_PSM_OPT_MASK_NONE       0
#define CM_PSM_OPT_MASK_QUICK    BM(0)
#define CM_PSM_OPT_MASK_OOS        BM(1)
#define CM_PSM_OPT_MASK_LMTD_SRV   BM(2)

/* PSM Minimum threshold
*/
#define CM_PSM_MIN_THRESHOLD    120   //2 mins, 120 seconds


/* Default Values */
#define CM_PSM_MAX_OOS_SCANS                     0xffff
#define CM_PSM_DEFAULT_RANDOMIZATION_WINDOW     (30 * 60) //30 min, 30*60 seconds
#define CM_PSM_LMTD_SRV_IND_WAIT_TIME       5

#define CM_PSM_MAX_UPTIME                      0xffffffff 


typedef PACKED struct PACKED_POST{
  uint8          version;

  boolean        is_enabled;
  /* Indicate if PSM is enabled or not */
  uint32        threshold;
  /* Duration threshold (in seconds) to enter PSM */

   uint8         psm_version;
  /* PSM version; 0 - w/o Network coordination */

  uint8         reserved;
  /* Reserved Fields */
}cm_psm_config_s_type;

typedef PACKED struct PACKED_POST{
  uint8          version;

  uint32         psm_opt_mask;
  /* PSM Optimization mask, CM_PSM_OPT_MASK_xxx */

  uint16        max_oos_full_scans;
  /* Max full scans to wait before declaring SYS_PSM_STATUS_OOS to clients */

  uint16         psm_duration_due_to_oos;
  /* PSM duration due to outage, in seconds */

  uint16        psm_randomization_window;
  /* PSM wakeup randomization window,in seconds */

  uint16         reserved[3];
  /* Reserved Fields */
}cm_psm_config_ext_s_type;


/* Enum of Protocals/Modules to which CM sends request for PSM enter
*/
typedef enum
{
  /** @cond
  */
  CM_PSM_MODULE_NONE=-1, /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

  CM_PSM_MODULE_NAS,
  /**< NAS */

  CM_PSM_MODULE_MC,
  /**< 1xCP */

  CM_PSM_MODULE_HDR,
  /**< HDR  */

  /** @cond
  */
  CM_PSM_MODULE_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

} cm_psm_module_e_type;

/* Enum of Protocals/Modules to which CM sends request for PSM enter
*/
typedef enum
{
  /** @cond
  */
  CM_PSM_ENTER_REQ_STATUS_NONE=-1, /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

  CM_PSM_ENTER_REQ_STATUS_NOT_SENT,
  /**< Request not yet posted to protocals.
    *  Indicates no psm enter request is in progress.
   */

  CM_PSM_ENTER_REQ_STATUS_SENT,
  /**< Request posted to protocals, psm enter request is in progress */

  CM_PSM_ENTER_REQ_STATUS_REJECTED,
  /**< PSM ENTER voted NOT_OK */

  CM_PSM_ENTER_REQ_STATUS_LPM_IN_PROGRESS,
  /**< All protocals voted OK, Moving to LPM is in progress */

  CM_PSM_ENTER_REQ_STATUS_COMPLETE,
  /**< All protocals/modules responded READY and moved to LPM */

  /** @cond
  */
  CM_PSM_ENTER_REQ_STATUS_MAX  /* FOR INTERNAL CM USE ONLY! */
  /** @endcond
  */

} cm_psm_enter_req_status_e_type;


typedef struct
{
  cm_psm_config_s_type              psm_cfg_info;
  /* psm cfg info */

  cm_psm_config_ext_s_type        psm_cfg_ext_info;
  /* psm cfg info */

  cm_psm_enter_req_status_e_type   psm_enter_req_status;
  /* Status of psm enter request */

  sys_psm_status_e_type        psm_enter_prot_status[CM_PSM_MODULE_MAX];
  /* psm enter state info at protocal/module level */

  dword                             psm_enter_uptime;

  uint16                          full_service_lost_count; 
  dword                           psm_lmtd_service_ind_uptime; 

  uint8                             psm_uim_opt_enabled;

}cm_psm_info_s_type;


/*===========================================================================
FUNCTION cmpsm_init

DESCRIPTION
  PSM initialization. Read the NV items related to PSM and reset internal 
  data structures.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_init(void);

/*===========================================================================
FUNCTION cmpsm_ptr

DESCRIPTION
  To get the pointer to psm object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_psm_info_s_type *cmpsm_ptr(void);

/*===========================================================================
FUNCTION cmpsm_msgr_cmd_proc

DESCRIPTION
  Process PSM msgr reports.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_msgr_cmd_proc( const void   *cmd_ptr );

/*===========================================================================
FUNCTION cmpsm_process_ready_rsp

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_ready_rsp( cm_psm_module_e_type module, 
                             sys_psm_status_e_type psm_status );

/*===========================================================================
FUNCTION cmpsm_process_ready_ind

DESCRIPTION
  Process READY_IND from NAS/MC/HDR. Move to LPM if all the modules are 
  ready to enter PSM.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_ready_ind( cm_psm_module_e_type module );

#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
/*===========================================================================
FUNCTION cm_psm_wrpt_handle

DESCRIPTION
  This function should be called to process the PSM reports from NAS like
  psm_ready_rsp, psm_ready_ind.


DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/

boolean                        cm_psm_wrpt_handle
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);
#endif
#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*===========================================================================
FUNCTION cm_psm_xrpt_handle

DESCRIPTION
  This function should be called to process the PSM reports from MC & HDR like
  psm_ready_rsp, psm_ready_ind.

DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean                        cm_psm_xrpt_handle
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);
#endif

/*===========================================================================
FUNCTION cmpsm_process_lpm

DESCRIPTION
 Send PSM_STATUS_COMPLETE Indication to clients.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_process_lpm( void );

/*===========================================================================

FUNCTION cmpsm_proc_full_srv_lost

DESCRIPTION
   Process the full service lost indication from SD.


DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern  void  cmpsm_proc_full_srv_lost
(
        sd_ss_e_type            ss
);

/*===========================================================================

FUNCTION cmpsm_proc_srv_ind

DESCRIPTION
   Process service indication from SD.


DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern  void  cmpsm_proc_srv_ind
(
 const sd_si_info_s_type   *si_info_ptr
 );

/*===========================================================================

FUNCTION cmpsm_timer_proc

DESCRIPTION
  Process timer for PSM.

DEPENDENCIES
  PSM object must have already been initialized with cmpsm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_timer_proc(
  cm_timer_event_type    timer_event   /* Indicate specific timer event */
);

/*===========================================================================

FUNCTION cmpsm_set_lmtd_srv_ind_timer_on_card_error

DESCRIPTION
  Set limited service indication timer if no subscription is available.

DEPENDENCIES
  PSM object must have already been initialized with cmpsm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmpsm_set_psm_lmtd_srv_ind_timer( );

/*===========================================================================
FUNCTION cmpsm_pwr_up_uim_opt_enabled_and_no_subs_avail

DESCRIPTION
  To check whether PSM UIM Optimization is enabled and no subscription available.

DEPENDENCIES
  cmpsm_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmpsm_pwr_up_uim_opt_enabled_and_no_subs_avail( void );

#endif  /* CMPSM_H */

