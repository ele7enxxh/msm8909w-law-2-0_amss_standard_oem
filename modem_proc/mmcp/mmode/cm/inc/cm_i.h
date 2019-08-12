#ifndef CM_I_H
#define CM_I_H
/*===========================================================================

              C A L L   M A N A G E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions used internally by the Call Manager.
  It provides internal interface to the CM.C file.
  It should NOT be included by any source file outside the Call Manager


Copyright (c) 1991 - 2012 by Qualcomm Technologies Incorporated.. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/inc/cm_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/12   xs      SD send acq fail to CM during HDR silent redial
01/17/11   aj/cl   SC SVLTE SO 33 suspend changes
04/12/11   cl      MDM should not grant permission to MSM if HDR is pending
                   to be attempted for acquisition
03/30/11   cl      Added fix ensure CM only kicks the proper blocked stack
01/24/11   rm      Initial release.

===========================================================================*/


#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "customer.h"  /* Customer configuration file */

#include "sd_i.h"
#include "cmll_v.h"


/* Type for CM_SRV_IND_INFO_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  sd_ss_e_type       ss;                /* System selectrion - MAIN or HDR */
  sd_si_info_s_type  si_info;            /* Service Indicators information */
} cm_srv_ind_info_type;

/* Type for CM_ATTACH_COMPLETE_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  sd_ss_e_type       ss;                /* System selectrion - MAIN or HYBR_2 or HYBR_3*/
} cm_attach_complete_type;

/* Union of all SD report types.  The header is always present and it
** specifies the command type and attributes. If the command has
** arguments they follow in the union.
*/

typedef union cm_sd_rpt_u
{
  cm_hdr_type                     hdr;
  cm_srv_ind_info_type            srv_ind_info;
  cm_acq_fail_type                acq_fail;
  cm_ok_to_orig_type              ok_to_orig;
  cm_e911_callback_type           e911_callback;
  cm_nam_changed_type             nam_changed;
  cm_ip_app_reg_status_type       ip_app_reg_status;
  cm_srv_lost_type                srv_lost;
  cm_prl_load_complete_type       prl_load_complete;
  cm_scan_permission_blocked_type permission_blocked;
  cm_ss_stack_status_type         ss_stack_status;
  cm_suspend_ss_s_type            suspend_ss;
  cm_opt_sr_hdr_acq_fail_f        sr_hdr_acq_fail;
  cm_full_srv_lost_type           full_srv_lost;
  cm_hybr_bsr_to_hdr_type         hybr_bsr_to_hdr;
  cm_cdma_bsr_stat_type           cdma_bsr_stat;
  cm_kick_hybr2_type              kick_hybr2;
  cm_cs_emerg_scan_fail_type      cs_emerg_scan_fail;
  cm_attach_complete_type         attach_complete;
} cm_sd_rpt_u_type;


/*===========================================================================

FUNCTION cm_sd_rpt_get_buf_else_err_fatal

DESCRIPTION
  Allocate a CM report command buffer from the cm_mc_rpt_free_q. Error
  fatal if allocation is NOT successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated CM report command buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_sd_rpt_u_type *cm_sd_rpt_get_buf_else_err_fatal
(
  void
);


/*===========================================================================

FUNCTION cm_sd_rpt_free_buf

DESCRIPTION
  Free a SD report buffer allocated from CM. Do NOT use to function to
  indicate the completion of processing of the report. Use cm_ret_rpt_buf for
  that.

DEPENDENCIES
  none

RETURN VALUE
  none.

SIDE EFFECTS
  none

===========================================================================*/
void cm_sd_rpt_free_buf( cm_sd_rpt_u_type *sd_rpt_ptr );


/*===========================================================================

FUNCTION CM_SD_RPT

DESCRIPTION
  Allows MC to queue up messages to the CM.  Grab a buffer from the
  cm_mc_rpt_free_q, fill it in, and call this function with it. All this will
  do is queue it and set a signal for the CM task.  When the command
  is done, the buffer will be queued as directed in the command header,
  and a task you specify may be signaled.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cm_sd_rpt
(
  cm_sd_rpt_u_type *cmd_ptr               /* Pointer to a CM report buffer */
);

#endif /* CM_I_H */
