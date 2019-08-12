#ifndef SRV_H
#define SRV_H
/*===========================================================================

              S E R V I C E    C O N F I G U R A T I O N    &

                          N E G O T I A T I O N

DESCRIPTION
  This file contains declarations for use with the Service Configuration
  and Negotiation Services.

  Copyright (c) 1995 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/srv.h_v   1.6   01 Oct 2002 15:24:34   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/srv.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
10/18/07   ag      Fixed compiler warning.
05/11/07   pg      More Lint Cleanup.
03/28/05   fc      Made FCS available to Rel D.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
09/16/04   fc      Added support for reverse DV.
08/09/04   fc      Service negotiation changes for Release C and D.
06/28/04   bkm     Merged the following from IS2000_REL_B_C:
  06/24/04   fc      Separate the event processing for sunfunction timer
                     expiration and SCR transaction completion.
06/21/04   va      Removed scr & nnscr from HHO-RIF cmd to SRV module
06/16/04   fc      Release C addendum changes for Sync ID.
04/20/04   bkm     Added srv_update_nn_scr_switch_params() to set
                   num_soft/softer_switching_frames_chm in NNSCR.
10/01/02   az      Mainlined FEATURE_IS2000
04/17/02   ph      Added a variable to SRV_CON_MSG_EC event payload for return
                   to caller.
01/23/02   ph      Clean up legacy design.
12/11/01   ph      Merged from SVD branch
                   08/23/01   ph      Added new func srv_is_in_norm_subf()
05/04/01   va      Changed the prototype for srv_get_pending_cfg
12/17/00   ht      Add function srv_set_evrc_usage() prototype.
11/02/00   lcc     Added support for FEATURE_COMPLETE_SNM.
07/28/00   fc      Mainline FEATURE_CS_ACTION_TIME_TRANS.
06/12/00   lcc     Added prototype for srv_update_nn_scr_from_pcnm.
04/17/00   va      Made srv_get_pref_RCs function return nothing
03/28/00   ry      Removed the #ifdef FEATURE_IS2000 around #include "mccdma.h"
#include "mccdma_v.h"
                   since the definition of mccdma_chnasn_type is unconditionally
                   needed for the prototype srv_init()
03/21/00   ry      Added IS2000 featurization
12/20/99   va      Support for updating non-neg SCR info from the power control message.
11/15/99   va      IS2000 Changes for voice:
                   New function srv_get_pref_rcs()
                   New function srv_get_pending_cfg()
                   Support for storing Non-Negotiable service config.
11/03/99   lcc     Merged from GD4021:
           ry      Added SRV_RIF_HHO_EC to srv_event_code_type and the
                   corresponding event parameter in srv_event_type
07/30/99   lcc     Exported srv_alt_cfg which is now used in snm.c.
07/19/99   nmn     Added declaration for snm_get_alt_so_list, surrounded by
                   #ifdefs for FEATURE_IS95B or FEATURE_SPECIAL_MDR.
06/15/99   doj     Added support for service config via handoff message.
05/10/99   kmp     Merged the IS-95B changes below.
           jq      Modified srv_init() to include default_config parameter of
                      ECAM
11/30/98   lcc     Obsoleted srv_alt_so and added prototype for srv_cmd_cfg.
06/08/95   rdb     Created file. Deja vu all over again.

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "caii.h"
#include "mccdma.h"
#include "mccdma_v.h"
#include "mccdma_i.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*--------------------------------------------------------------------------
                             STATUS CODES

The following status codes are for use in the 'status' field of the
srv_event_type defined below.
--------------------------------------------------------------------------*/

typedef enum
{
  SRV_DONE_S,      /* Event successfully processed */
  SRV_NOTNOW_S,    /* Inappropriate to process the specified event
                      at this time. Caller may try again later. */
  SRV_BADPARM_S,   /* Illegal parameter(s) specified */
  SRV_NOTSUP_S,    /* Unsupported feature or operation specified */
  SRV_ERR_S        /* Internal error occurred */
} srv_status_type;


/*--------------------------------------------------------------------------
                               EVENT CODES

The following event codes are for use in the 'ec' field of the
srv_event_type defined below.
--------------------------------------------------------------------------*/

typedef enum
{
  SRV_SIG_EC,         /* Service subfunction timer expired */
  SRV_SCR_CMP_EC,     /* SCR transaction completed */
  SRV_UI_EC,          /* Input from user interface */

  SRV_REQ_MSG_EC,     /* Service request message received */
  SRV_RSP_MSG_EC,     /* Service response message received */
  SRV_CON_MSG_EC,     /* Service connect message received */
  SRV_CTL_MSG_EC,     /* Service option control message received */

  SRV_HO_DIR_EC,      /* Any handoff message used as service connect message */

  SRV_TC_WFO_EC,      /* Waiting for order substate entered */
  SRV_TC_WFA_EC,      /* Waiting for answer substate entered */
  SRV_TC_CON_EC,      /* Conversation substate entered */
  SRV_TC_REL_EC,       /* Release substate entered */
  SRV_RIF_HHO_EC,      /* Restore given SCR immediately */
  SRV_PWR_CTRL_EC,     /* Save relevant parameters from PCM into the Non-Neg SCR*/
  SRV_CLAM_MSG_EC,     /* Any call assignment message used as service connect message */
  SRV_TOTAL_COMMANDS
} srv_event_code_type;


typedef struct
{
  srv_event_code_type  ec;      /* Event code  (srv_event_code_type) */
  srv_status_type      status;  /* Status code (srv_status_type) */

  union                         /* Union of all event parameters. Note */
  {                             /* not all events require parameters.  */
    struct  /* Corresponds to SRV_UI_EC */
    {
      word  so;
        /* Service option number */
    } ui;

    struct  /* Corresponds to SRV_REQ_MSG_EC */
    {
      caii_ftc_srv_req_type  *msg_ptr;
        /* Ptr to service request message */
    } req_msg;

    struct  /* Corresponds to SRV_RSP_MSG_EC */
    {
      caii_ftc_srv_rsp_type  *msg_ptr;
        /* Ptr to service response message */
    } rsp_msg;

    struct  /* Corresponds to SRV_CON_MSG_EC */
    {
      caii_srv_con_type  *msg_ptr;
        /* Ptr to service connect message */
      boolean scm_validation_success;
        /* return parameter for caller's use */
    } con_msg;

    struct /* Corresponds to SRV_HO_DIR_EC */
    {
      caii_ho_msg_type *msg_ptr;
        /* Ptr to handoff message */
    } ho_msg;

    struct  /* Corresponds to SRV_CTL_MSG_EC */
    {
      caii_ftc_srv_ctl_type  *msg_ptr;
        /* Ptr to service option control message */
    } ctl_msg;

   struct
   {
     mccdma_pwr_ctrl_info_type *pwr_ctrl_info_ptr;
   }pcm_info;

   struct
   {
     caii_call_assignment_type *msg_ptr;
       /* Pointer to Call Assignment message */
     boolean clam_validation_success;
       /* return parameter for caller's use */
   } clam_msg;

  } ep;  /* Event parameter union */

} srv_event_type;


typedef struct
{
  caii_complete_srv_cfg_type cfg;   /* Holds pending service configuration */
  byte cfg_msg_type;       /* The CAI message responsible for this config. */
} srv_pending_cfg_type;

/*--------------------------------------------------------------------------
                     EXTERNALIZED VARIABLES

--------------------------------------------------------------------------*/

// !!!Caution (see snm.c)!!!
// negotiation_info holds pointers to the following data structure.  Since the
// following structures are legacy code, they are still defined as what they
// were before.  They should eventually be moved into negotiation_info and be
// hidden.

extern caii_complete_srv_cfg_type  srv_current_cfg;
  /* Holds current active service configuration */

extern srv_pending_cfg_type  srv_pending_cfg;
    /* Holds pending service configuration. */

extern caii_srv_cfg_type  srv_proposed_cfg;
  /* For building a proposed service configuration to send. */


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SRV_INIT

DESCRIPTION
  This function intializes the Service Configuration and Negotiation
  Services. It should be called whenever the mobile station enters the
  Traffic Channel state for the purpose of making a call.

DEPENDENCIES
  Assumes that srv_timer (a rex timer) has been defined and initialized.

RETURN VALUE
  SRV_DONE_S    - Initialization successful.
  SRV_BADPARM_S - Invalid initialization parameter(s) specified.
  SRV_NOTSUP_S  - Unsupported service option specified.

SIDE EFFECTS

===========================================================================*/

extern srv_status_type  srv_init
(
  boolean  mob_orig,
    /* Specifies whether the call is mobile originated (TRUE) or
       mobile terminated (FALSE). */

  word  so,
    /* Specifies which service option number was included in either the
       origination message or the page response message. */

  mccdma_chnasn_type *chn_assign
     /* Pointer to the channel assignment message */
);


/*===========================================================================

FUNCTION SRV_RPT_EVENT

DESCRIPTION
  This function reports a specified event to the Service Configuration
  and Negotiation Services.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

extern void srv_rpt_event
(
  srv_event_type  *e_ptr
    /* Pointer to a block describing the event */
);


/*===========================================================================

FUNCTION SRV_GET_CUR_CFG

DESCRIPTION
  This function returns the current service configuration.

DEPENDENCIES
  Only valid during Traffic Channel Operation.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void  srv_get_cur_cfg
(
  caii_srv_cfg_type  *cfg_ptr
    /* Pointer to record in which to return the current service
       configuration. */
);


/*===========================================================================

FUNCTION SRV_GET_CUR_NON_NEG_CFG

DESCRIPTION
  This function returns the current non negotiable service configuration.

DEPENDENCIES
  Only valid during Traffic Channel Operation.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void  srv_get_cur_non_neg_cfg
(
  caii_non_neg_srv_cfg_type  *cfg_ptr
    /* Pointer to record in which to return the current non neg service
       configuration. */
);


/*===========================================================================

FUNCTION SRV_GET_PENDING_CFG

DESCRIPTION
  This function returns the pending service configuration.

DEPENDENCIES
  Only valid during Traffic Channel Operation.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern const caii_complete_srv_cfg_type *  srv_get_pending_cfg
(
  void
);

/*===========================================================================

FUNCTION srv_get_pref_fundicated_RCs

DESCRIPTION
  This function returns the preferred RC for the forward and reverse
  fundicated channel. This preference is sent back in the page resp or the
  origination message.
  Once all SOs are migrated to srv, this will be obsolete. Avoid calling this
  function directly. Always call the corresponding function in SNM

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void srv_get_pref_fundicated_RCs
(
  word so, // The SO for which pref RCs are needed
  cai_radio_config_type * fwd_rc_ptr, // Storage for returned fwd RC
  cai_radio_config_type * rev_rc_ptr // Storage for return  rev RC
);

/*===========================================================================

FUNCTION SRV_CMD_CFG

DESCRIPTION
      This function commands the multiplex sublayer and service option
      applications to begin using a specified service configuration at
      a specified action time.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void  srv_cmd_cfg
(
  caii_srv_cfg_type  *cfg_ptr
);

/*===========================================================================

FUNCTION SRV_IS_IN_NORM_SUBF

DESCRIPTION
  This function returns TRUE if SRV is in the normal subfunction; FALSE otherwise.

DEPENDENCIES

RETURN VALUE
  As described above.

SIDE EFFECTS

===========================================================================*/

boolean srv_is_in_norm_subf(void);


#endif /* SRV_H */
