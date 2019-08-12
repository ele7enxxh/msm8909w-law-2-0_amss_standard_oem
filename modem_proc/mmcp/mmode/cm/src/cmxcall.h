#ifndef CMXCALL_H
#define CMXCALL_H
/*===========================================================================

    C A L L   M A N A G E R   C D M A   C A L L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMXCALL.C


Copyright (c) 2002 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmxcall.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/12   cl      Support SO-33 CALL_END_CNF_F from 1X-CP
01/17/12   aj/cl   SC SVLTE SO 33 suspend changes
07/24/11   aj      SC SVLTE 1.0 merge to mainline
06/06/08   sv      Removing unneccesary snd.h
06/11/07   pk      VoIP -> 1x hho enahncements / CR fixes
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
07/14/06   ka      Fixing the issue where call cmd cb for call end api was
                   being called more than once.
12/03/05   ic      Lint clean up 
11/08/05   sk      Separated CM reports and commands.
03/24/05   dk      Externalize cmxcall_send_end_cmd.
02/01/05   dk      Added WLAN Support.
06/25/04   dk      Moved certain generic call obj array walk through functions
                   from cmxcall.c.
06/14/04   ic      Renamed cmxcall_is_there_a_hdrdata_call() to
                   cmxcall_is_hdr_call()
                   Modified cmxcall_is_hybr_hdr_call() to also return call id
                   of the HDR call (if any).
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
10/01/03   ws      Merged from GW Targets.
07/29/03   ws      Added Automatic Silent ReDial.
01/21/03   ws      Updated copyright information for 2003
05/05/02   vt      Fixed compile problem with new mm cm.h
04/28/02   vt      Initial version.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "cm.h"                              /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"                             /* Internal interface to cm.c */

#include "cmcall.h"

#define CM_NV_MAX_STN_SR_DIGITS 15
  /* Maximum number of digits allowed for called party number
     in SRVCC origiation request */

/* Enumeration of the feature applicability conditions - BSR higher priority 
** than 3GPP2 data calls based on the acquired system (Home/roam)
*/
typedef enum {

  CMXCALL_PS_OPT_DISABLED,

  CMXCALL_PS_OPT_ENABLED_ROAM_ONLY,

  CMXCALL_PS_OPT_ENABLED_HOME_ROAM

}cmxcall_ps_call_opt_e_type;


typedef PACKED struct PACKED_POST {

  uint8                     version;

  /* During the timer duration, BSR is given more priority than 3GPP2 PS call */
  uint32                    timer_value;

  /* Enumeration when the feature-BSR higher priority than PS call,
  ** is applicable */
  cmxcall_ps_call_opt_e_type allowed;

  uint32                     reserved[4];

}cmxcall_ps_call_optimize_s_type;

/* This holds information on prefixed called party number to be
   used during 1xSRVCC origination request
 */
typedef PACKED struct PACKED_POST{

  uint8  version;

  byte e911_cnt;         /* number of digits in dialed_digits array for e911 call */

  char e911_dialed_digits[CM_NV_MAX_STN_SR_DIGITS];
     /* e911 called party number for 1xsrvcc orig */  

  byte reg_cnt;         /* number of digits in dialed_digits array for regular call */

  char regular_dialed_digits[CM_NV_MAX_STN_SR_DIGITS];
   /* Normal called party number for 1xsrvcc orig */

  uint8                           reserved[3];
  /* Reserved Fields */


}cmxcall_cdma_srvcc_stn_sr_num;


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

#ifdef FEATURE_HDR_HYBRID



/*===========================================================================

FUNCTION cmxcall_is_auto_pkt_data_so

DESCRIPTION
  Check whether the data service option is a packet data service
  option can be used by the automaitc service type


DEPENDENCIES
  none

RETURN VALUE
  TRUE if Automatic service type is allowed
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_is_auto_pkt_data_so(

    word                   srv_opt
        /* service option */
);

#endif /* FEATURE_HDR_HYBRID */


/*===========================================================================

FUNCTION cmxcall_handle_page_proceed_response

DESCRIPTION
  Take action based on DS indication about whether to proceed with 1x page.
  If page proceed = TRUE, proceed to shutdown hybrid operation ( DO is SVDO
  mode and LTE in SVLTE mode).
  If page proceed = FALSE, inform 1xcp to drop the call.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cmxcall_handle_page_proceed_response(

    boolean   page_proceed
    /* DS indication about whether to proceed with 1x page
    */
);


/*===========================================================================

FUNCTION cmxcall_unsuspend_hybrid_operation

DESCRIPTION
    Since CM suspended HYBR-1 and HYBR-2 stacks for 1X SO33/32 data call,
    this function resumes the suspension.

    Do this based on whether SVLTE and SVDO are supported.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cmxcall_unsuspend_hybrid_operation( void );

/*===========================================================================

FUNCTION cmcall_outgoing_data_call_proceed

DESCRIPTION
  Take action based on DS indication about whether to proceed with 1x page.
  If page proceed = TRUE, proceed to shutdown hybrid operation ( DO is SVDO
  mode and LTE in SVLTE mode).
  If page proceed = FALSE, inform 1xcp to drop the call.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmxcall_outgoing_data_call_proceed( void );


/*===========================================================================

FUNCTION cmxcall_is_end_suspend_status

DESCRIPTION
  checks if there are any 1x SO 33/32 calls active. If true, UE should still
  suspend hybr 1 / hybr2 staks.

DEPENDENCIES
  None.

RETURN VALUE
   CM_CALL_ID_INVALID- 1x SO 33/32 calls not active;
  valid call id  - 1x SO 33/32 calls active

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_id_type cmxcall_is_end_suspend_status
(
  void
);
/*===========================================================================

FUNCTION CMCALL_IS_1x_DATA_SRV_OPT

DESCRIPTION
  Check whether a specified service option is a 1x Data service option.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if specified service option is a 1x Data service option,
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmcall_is_1x_data_srv_opt
(
  word    srv_opt                               /* service option to check */
);
/*===========================================================================

FUNCTION cmxcall_send_orig

DESCRIPTION
  Send an origination command to MC.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_send_orig(

    cmcall_s_type    *call_ptr
        /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmxcall_reset

DESCRIPTION
  Reset call fields to default values reflecting a call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_reset
(
  cmcall_s_type  *call_ptr
    /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmxcall_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_info_get
(
  const cmcall_s_type          *call_ptr,
    /* Point at call object */

  cm_mm_call_info_s_type    *call_info_ptr
    /* Copy call state info into this buffer */
);



/*===========================================================================

FUNCTION cmxcall_end

DESCRIPTION
  Does call end necessary activities.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_end
(
  cmcall_s_type  *call_ptr
    /* pointer to a call object */
);



/*===========================================================================

FUNCTION cmxcall_timer_proc

DESCRIPTION
  Process timer events

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_timer_proc
(
  cmcall_s_type           *call_ptr
      /* Point at call object */
);



/*===========================================================================

FUNCTION CMXCALL_CLIENT_CMD_PROC

DESCRIPTION
  Process clients' call commands

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_client_cmd_proc
(
  cm_call_cmd_s_type  *call_cmd_ptr                       /* pointer to a CM command */
);


/*===========================================================================

FUNCTION CMXCALL_CLIENT_ORIG_PROC

DESCRIPTION
  Process clients' origination call command.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type cmxcall_client_orig_proc
(
  cmcall_s_type  *call_ptr

);


/*===========================================================================

FUNCTION cmxcall_mc_rpt_proc

DESCRIPTION
  Process MC reports


DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS^
  none

===========================================================================*/
extern void cmxcall_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);

/*===========================================================================

FUNCTION cmxcall_determine_call_type_from_so

DESCRIPTION
  Determine the call type according to current call service option.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cm_call_type_e_type cmxcall_determine_call_type_from_so(

    uint16    srv_opt
        /* SO */
);


#ifdef FEATURE_HDR_HANDOFF

/*===========================================================================

FUNCTION cmcall_is_pkt_data_call

DESCRIPTION
  This function check the current call is a packet data call or not

DEPENDENCIES
  Should only be called after call is connected and sevice option is
  assigned

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_pkt_data_call
(
  const cmcall_s_type             *call_ptr
);
#endif // FEATURE_HDR_HANDOFF

#ifdef FEATURE_HDR_HYBRID
/*===========================================================================

FUNCTION cmxcall_is_hdr_call

DESCRIPTION
  Checks whether there is an HDR call up.

DEPENDENCIES
  None

RETURN VALUE
  Valid call id of a HDR call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  None

===========================================================================*/
 cm_call_id_type cmxcall_is_hdr_call
(
  void
);
#endif // FEATURE_HDR_HYBRID


#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

/*===========================================================================

FUNCTION  cmxcall_call_cmd_orig_para_check

DESCRIPTION
  Check parameter errors for call origination command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmxcall_call_cmd_orig_para_check
(
  const cm_call_cmd_s_type       *call_cmd_ptr
);

/*===========================================================================

FUNCTION cmxcall_cmd_copy_fields

DESCRIPTION
  Copy the active CDMA/HDR fields of a call command into a call object.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_cmd_copy_fields(

    cmcall_s_type               *call_ptr,
        /* Pointer to a call object */

    const cm_call_cmd_s_type    *cmd_ptr
        /* Pointer to a call command */

);

#ifdef FEATURE_OTASP_HOLD_CMD_IN_COMMIT

/*===========================================================================

FUNCTION cmxcall_is_conv_pc_otapa_commit

DESCRIPTION
  Finds if any paging channel otapa call is in the process of commit.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - call found; FALSE - Not.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmxcall_is_conv_pc_otapa_commit
(
  void
);
#endif

/*===========================================================================

FUNCTION cmxcall_end_call

DESCRIPTION
  Ends the specified call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_end_call
(
  cm_call_id_type      call_id,
  cm_call_end_e_type   end_status,
  byte                 end_ll_reason
);

/*===========================================================================

FUNCTION cmxcall_send_end_cmd

DESCRIPTION
  Send an end command to either MC or HDRMC task

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_send_end_cmd
(
  cmcall_s_type    *call_ptr,
     /* pointer to a call object */

  byte                   end_reason
     /* call end reason */
);

/*===========================================================================

FUNCTION  cmxcall_client_cmd_check

DESCRIPTION
  Check for call command parameter errors and whether a specified call command
  is allowed in the current state of the call/phone.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if command is allowed in the current state
  of the call/phone, specific cm_call_cmd_err_e_type otherwise.

SIDE EFFECTS
  If originating dial-string matches one of the special numbers that is
  stored in the phone-book, this function changes the call type to E911,
  OTASP, etc.

===========================================================================*/
extern cm_call_cmd_err_e_type  cmxcall_client_cmd_check(

    cm_call_cmd_s_type          *call_cmd_ptr
        /* Pointer to a CM command */
);

/*===========================================================================

FUNCTION cmxcall_is_svdo_capable

DESCRIPTION
  Compare the call objects for the 1X and DO call and check to see if 
  simultaneous operation can be supported.

DEPENDENCIES
  Call objects must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE: Simultaneous operations possible

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmxcall_is_svdo_capable
(
  const cmcall_s_type  *voice_call_ptr         
    /* pointer to voice call Object  */
);

 #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
/*===========================================================================

FUNCTION cmxcall_handle_srvcc_cache

DESCRIPTION
 
 Process SRVCC cache info from IMS and reports HO_ETURA_RESP 
 to 1xcp.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmxcall_handle_srvcc_ims_cache(

  const cm_srvcc_call_context_rsp_s_type *cahce_rpt_ptr
    /* Pointer to MC report */
);

#endif


/*===========================================================================

FUNCTION cmxcall_is_cdma_ps_call_optimization_enabled

DESCRIPTION
  This function returns if the optimization of holding 1x/HDR data calls 
  during BSR is enabled or not.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if enabled
  FALSE if not enabled

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmxcall_is_cdma_ps_call_optimization_enabled
(
	cmcall_s_type  *call_ptr

);


/*===========================================================================

FUNCTION cmcall_mc_incoming_page_proceed

DESCRIPTION
  Sends  MC_CM_INCOMING_PAGE_CNF_F command to MC with payload indicating whether to
  proceed with the call or not.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_mc_incoming_page_proceed
(
    boolean     call_proceed
        /* Tells 1xcp/MC whether to proceed with call or not */
);

#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*===========================================================================

FUNCTION cmxcall_fill_srvcc_orig_param

DESCRIPTION

 Process SRVCC cache info from IMS and fill 1x SRVCC origination
 fields

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmxcall_fill_srvcc_orig_param(

  const cmcall_s_type           *call_ptr,
    /* Pointer to MC report */

  mc_origination_type    *orig_param,
  boolean is_drvcc_call
);
#endif

#endif /* CMXCALL_H */

