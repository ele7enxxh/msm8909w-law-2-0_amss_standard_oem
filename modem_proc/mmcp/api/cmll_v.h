#ifndef CMLL_V_H
#define CMLL_V_H
/*===========================================================================

              C A L L   M A N A G E R   L O W E R   L A Y E R

                       H E A D E R   F I L E

DESCRIPTION
GENERAL DESCRIPTION
  This source file contains the function definitions necessary for the
  Call Manager module and the Lower Layers to send commands to one
  another. This file contains only the common interfaces, i.e. interfaces
  shared by all protocol stacks. Individual protocol interfaces are
  defined in their respective header files.


Copyright (c) 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cmll_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/19/13   fj      DSDX L+G interface change.
02/11/13   xs      SD send acq fail to CM during HDR silent redial
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/20/12   gm      Data synchronization for CM Call objects
01/13/12   aj      SC SVLTE SO 33 suspend changes
04/12/11   cl      MDM should not grant permission to MSM if HDR is pending
                   to be attempted for acquisition
03/30/11   cl      Added fix ensure CM only kicks the proper blocked stack
03/03/11   gm      Added separate queue processing for M2M indications
01/26/11   rm      CMI-4: Removing #ifdef's, moving internal def's to
                   private files
11/11/10   rm      Adding WLAN security info interface
10/01/10   ak      Merged the Fusion code from Dev branch.
06/21/10   jqi     FEATURE_MMODE_MDM_MSM_EVENTS & FEATURE_CM_MSM_CDMA_SRV_RPT
03/17/10   cl      Add support for CM_PH_EVENT_PRL_INIT to indicate PRL is loaded
08/07/09   am      Adding mmgsdi_card_rpt
09/16/08   pk      Added support for WLAN UOoS
09/04/08   dm/sv   Added WPS result report type.
07/24/08   rn      Send regn reject ind to clients
07/18/08   ks      Added new info for Current Operator's Cell ID and
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
05/06/08   ak      BCMCS 2.0 changes
03/27/08   rn/ic   Added support for new command CM_PH_STATUS_CHGD_REQ
03/14/08   ks      Updating UI with service status when UE is OOS in Connected Mode
12/13/07   sk/ak   Added DCM.
12/07/07   ra      Added support for IMS VCC in HOMER files
10/03/07   rn      Added CM_PS_SIG_CON_REL_CNF in cm_name_type
08/17/07   ka/dm   New field to report HS call in progress
06/14/07   pk/cl   Added support for power suspend
06/06/07   pk/cl   Added support for LN status notification from HDR
04/15/07   rp      Removing FEATURE_MMGPS
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      Changes to support Voip deregistration.
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
11/17/06   jqi     Added CDMA connect Oder Ack support.
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
05/17/06   pk      Added support for DDTM during BCMCS flows
04/17/06   ka      Adding support for HSDPA and DTM indications.
03/10/06   pk      Added support for high PSIST
03/09/06   pk      Added support for Generic reports
03/02/06   pk      Added VoIP - 1X handoff reports
02/17/06   ka      Adding support for videoshare
01/06/06   ka      Adding Release 5 changes including emerg call list,
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
12/02/05   ka      Adding new report primitive CM_MM_PS_DATA_SUSPEND_IND
11/30/05   ka      CM_HPLMN_SEARCH_CNF is sent from REG in response to
                   CM_HPLMN_SRCH_REQ
11/18/05   sk      Added CM API cm_sd_rpt_free_buf() to free CM SD reports.
09/29/05   pk      Added cm_privacy_setting_type to cm_mc_rpt_type union
09/26/05   ka      Adding CM_IP_CALL_IND_MT_RING_CNF.
09/09/05   ic      Restored CM_INTERCEPT_F enum value accounting for removal
                   of CM_REORDER_F
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/22/05   ic      Obsoleted CM_REORDER_F.
                   Use CM_CALL_ORIG_FAIL_F instead with orig_fail.code set to
                   CM_CALL_ORIG_ERR_REORDER and call_ptr->end_status set to
                   CM_CALL_END_REORDER
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
04/05/05   ka      Adding CM_CIPHER_IND to cm_name_e
03/14/05   ka      Adding report types for IP apps
02/01/05   dk      Added WLAN Support.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/03/04   ws      Remote WMS support.
11/11/04   sj      Added new call end cnf report for HDR.
11/01/04   ka      New primitive for user to user data signaling
09/22/04   ka      Adding counting comments to cm_name_e
09/08/04   jqi     More ps dormant optimization.
07/28/04   ka      Added support for secondary PDP context
07/21/04   sj      Added support for umts to 1x handover.
06/30/04   dk      Added CM_PS_SIG_CON_REL_IND report.
06/25/04   dk      Changed return type of cm_call_id_deallocate() from boolean
                   to void.
06/03/04   ws      Add support for redirecting number.
05/21/04   ka      Added CM_CAMPED_IND sent by REG when phone gets camped.
04/02/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.23 and MSMSHARED_CM.01.00.10.00.28
03/15/04   ka      Merged changes from 6250.
                   Use service domain form service indicator for call orig.
                   Propagated service domain for call origination.
                   Fixed forcing service domain on the fly for GW system.
03/12/04   jqi     Use service domain form service indicator for call orig.
03/10/04   jqi     Propagated service domain for call origination.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/06/04   ws      Initial jaguar/mainline merge.
11/19/03   ic      Marked CM_HDR_RESELECT_EXIT_HDR_F as obsolete
10/14/03   sj      Added CM_HDR_RESELECT_EXIT_HDR_F report.
10/10/03   ic      Modified cm_call_id_allocate() to die if CM ran out of
                   call ids.
05/01/03   jar     Added CM_GSDI_REFRESH_FCN_IND
04/28/03   AT      Lint cleanup.
04/14/03   ws      Added BSID changes
01/21/03   ws      Updated copyright information for 2003
11/14/02   vt      Merged changes from GW mainline.
                   Added card status and deep sleep reports.
10/31/02   vt      Removed handling of NDSS origination failure - really!.
                   Mainlined feature SD20.
10/28/02   vt      Removed handling of NDSS origination failure.
10/23/02   vt      Mainlined FEATURE_CM_USE_UMTS_REG_INTERFACE.
10/23/02   vt      Rearranged the order of LL reports in enum
10/05/02   vt      Made some functions external for SD20. Added new SD20
                   reports.
07/17/02   vt      Updated it for multimode.
07/08/02   vt      Added call id allocate & deallocate functions.
                   Removed inclusion of cmwll.h. cmwll.h should include
                   this file.
05/04/02   PRK     Included cmwll.h.
04/25/02   RI      Added process_uss_data_conf field to cm_rpt_type.
04/24/02   RI      Added CM_RAB_REESTAB_FAIL and renamed CM_RAB_ESTABLISH_IND to
                   CM_RAB_REESTABLISH_IND in cm_name_type.
                   Added  rabm_rab_reestablish_fail_T message and
                   renamed rabm_rab_establish_ind_T to
                   rabm_rab_reestablish_ind_T in cm_rpt_type.
02/06/02   PRK     Added cm_tc_get_buf_else_err_fatal and cm_tc_send_cmd
                   functions.
02/04/02   RI      Modifications based on mn_cm_exp.h for Supplementary Services.
                     - removed mn_cm_exp_t.h
11/21/01   RI      Added rabm_rab_release_ind_T in cm_rpt_type
                   for PS Data.
10/15/01   RI      Added Supplementary Services declarations.
08/23/01   HD      NAS header file reorg changes + RSSI reporting for GSM/WCDMA.
04/27/01   HD      Initial release.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "rex.h"
#include "queue.h"
#include "cm.h"
#include "sd.h"                       /* Interface to System Determination */
#include "sd_v.h"                       /* Interface to System Determination */
#include "cmutil.h"                   /* for cm_ipapp_cause_e_type */
#include "cmll.h"
#include "mmgsdilib.h"

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

typedef q_type          cm_cmd_q_type;


/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/


/*
** Status from the CM task, in command buffer
*/

typedef enum
{
  CM_DONE_S,                      /* General OK return */
  CM_BUSY_S,                      /* Command is being processed */
  CM_BADCMD_S,                    /* Invalid command */
  CM_BADPARM_S                    /* Bad Parameters */
} cm_status_type;


/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

/* Macros to decide the type of a report
*/
#define CMLL_RPT_IS_SD( event ) \
                    ( ((event) > CM_RPT_SD_BEGIN) && \
                    ((event) < CM_RPT_SD_END) )

#define CMLL_RPT_IS_1X( event ) \
                    ( ((event) > CM_RPT_1X_BEGIN) && \
                    ((event) < CM_RPT_1X_END) )

#define CMLL_RPT_IS_GW( event ) \
                    ( ((event) > CM_RPT_GW_BEGIN) && \
                    ((event) < CM_RPT_GW_END) )

#define CMLL_RPT_IS_IP( event ) \
                    ( ((event) > CM_RPT_IP_BEGIN) && \
                    ((event) < CM_RPT_IP_END) )

#define CMLL_RPT_IS_IPSMS( event ) \
                    ( ((event) > CM_RPT_IP_SMS_BEGIN) && \
                    ((event) < CM_RPT_IP_SMS_END) )

#define CMLL_RPT_IS_GENERIC( event ) \
                    ( ((event) > CM_RPT_GENERIC_BEGIN) && \
                    ((event) < CM_RPT_GENERIC_END) )

/*
** Command header type.  Each command to CM starts with this header.
** These are the common fields (base class) that all command headers to CM_
** must !start! with.  They are defined as a macro to insure that all
** headers contain the same set.
*/
#define CM_CMD_HDR_COMMON_FIELDS                                             \
  q_link_type        link;         /* Queue link */                          \
  cm_name_type       cmd;          /* Command to execute */                  \
  rex_tcb_type       *task_ptr;    /* Pointer to requesting task TCB */      \
  rex_sigs_type      sigs;         /* Signal to set upon cmd completion */   \
  cm_status_type     status;       /* Completion code status */              \
  q_type             *done_q_ptr   /* Queue to place this cmd on when done */


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/*
** Generic command header type - all cmds to CM must start with this or one
** of its supersets.
*/
typedef struct {
  CM_CMD_HDR_COMMON_FIELDS;       /* All CM headers must !start! with this */
} cm_hdr_type;

/*
** Info Record Command header type.  This provides more grouping info needed
** to process info records.  Any fields added must be added after the fields
** that cm_info_hdr_type and cm_xll_rpt_hdr_s_type share in common.
*/
typedef struct {
  CM_CMD_HDR_COMMON_FIELDS;       /* All CM headers must !start! with this */
  boolean            last_record;     /* Marks last msg in a group of msgs */
} cm_info_hdr_type;


/*-------------------------------------------------------------------------*/

/* Type for CM_ACQ_FAIL_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  sd_ss_e_type       ss;                /* System selectrion - MAIN or HDR */
  sd_ss_mode_pref_e_type     mode_pref;    /* Acq failed on above mode*/
} cm_acq_fail_type;

/* Type for CM_OPT_SR_HDR_ACQ_FAIL_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
} cm_opt_sr_hdr_acq_fail_f;

/* Type for CM_HYBR_BSR_TO_HDR_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  sd_hybr_bsr_to_hdr_s_type     hybr_bsr_to_hdr_status;
                       /* HYBR BSR to HDR status */
} cm_hybr_bsr_to_hdr_type;

/* Type for CM_KICK_HYBR2_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
} cm_kick_hybr2_type;

/* Type for CM_OK_TO_ORIG_F
*/
typedef struct {

  cm_hdr_type              hdr;         /* Header */
  sd_ss_e_type             ss;          /* System selectrion - MAIN or HDR */
  sd_si_ok_to_orig_s_type  si_ok_to_orig;
                         /* System indications related to call origination */
} cm_ok_to_orig_type;


/* Type for CM_E911_CALLBACK_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  sd_emerg_cb_mode_e_type  emerg_cb_mode;       /* emergency callback mode */
} cm_e911_callback_type;


/* Type for CM_NAM_CHANGED_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  byte nam;                                              /* New NAM number */
} cm_nam_changed_type;



/* Type for CM_IP_APP_REG_STATUS
*/
typedef struct {
  cm_hdr_type                    hdr;                    /* header */
  cm_ipapp_reg_status_e_type     ipapp_reg_status;       /* Reg, de-reg, failure */
  cm_call_type_mask_type         call_type_mask[CM_CALL_TYPE_MAX];         /* Array of Bit mask of CM call type */
  cm_ipapp_cause_e_type          ipapp_cause;            /* ipapp registration cause values */
  cm_ipapp_warn_info_s_type      warn_info;              /* warning info structure */
  sys_sys_mode_mask_e_type       sys_mode[CM_CALL_TYPE_MAX];               /* Array of System modes app can support for calls (corresponding to call type) */
  sd_ss_e_type                   ss;
} cm_ip_app_reg_status_type;



/* Type for CM_SRV_LOST_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  sd_ss_e_type       ss;                /* System selectrion - MAIN or HDR */
} cm_srv_lost_type;

/* Type for CM_FULL_SRV_LOST_F
*/
typedef struct {
  cm_hdr_type        hdr;               /* header */
  sd_ss_e_type       ss;                /* System selectrion - MAIN or HDR */
} cm_full_srv_lost_type;

/* Type for CM_BSR_STAT_CHGD_F
*/
typedef struct {
  cm_hdr_type   hdr;               /* header */
  boolean       bsr_status;        /* BSR status */
} cm_cdma_bsr_stat_type;


/* Type for CM_CS_EMERG_SCAN_FAIL_F
*/
typedef struct {
  cm_hdr_type           hdr;                                     /* header */
  sd_ss_e_type       ss;                /* SS on which CS emerg Scan fail  */
} cm_cs_emerg_scan_fail_type;

/* Type for CM_PRL_INIT_F
*/
typedef struct {
  cm_hdr_type                    hdr;                    /* header */
  boolean                        is_prl_load_complete;   /* Indicates if PRL is successfully loaded*/
} cm_prl_load_complete_type;

/* Type for CM_MMOC_SCAN_BLOCKED_F
*/
typedef struct {
  cm_hdr_type                    hdr;  /* header*/
  sd_ss_e_type                   ss;   /* ss indicates which stack was blocked */
} cm_scan_permission_blocked_type;

/* Type for CM_MMOC_SCAN_BLOCKED_F
*/
typedef struct {
  cm_hdr_type                    hdr;  /* header*/
  sd_ss_e_type                   ss;   /* ss indicates which stack was blocked */
  boolean                        is_active; /* indicate if the stack status is active*/
} cm_ss_stack_status_type;

/* Type for CM_SUSPEND_SS_F
*/
typedef struct {
  cm_hdr_type                    hdr;  /* header*/
  sd_ss_e_type                   ss;   /* ss indicates which stack was blocked */
  boolean                        is_suspend; /* indicate if the stack status is active*/
} cm_suspend_ss_s_type;

/* DEM report types
*/

typedef enum cm_dem_rpt_e
{
  CM_DEM_RPT_NONE = -1,         /* For internal use of CM */

  CM_DEM_RPT_APPS_PWR_COLLAPSE, /* Apps processor is power collapsed */

  CM_DEM_RPT_APPS_PWR_RESTORED, /* Power is restored to Apps processor */

  CM_DEM_RPT_APPS_SUSPEND,      /* Apps processor goes to standby mode */

  CM_DEM_RPT_MAX                /* For internal use of CM */

}cm_dem_rpt_e_type;

typedef struct cm_dem_rpt{

  cm_dem_rpt_e_type          dem_rpt_type;

} cm_dem_rpt_s_type;

/* Structure to carry payload to end charged calls.
** A generic report is queued to CM to process in it's context
*/
typedef struct cm_drop_calls_info{

  sd_ss_e_type ss;
    /* SS on which calls are active */

}cm_drop_calls_info_s_type;

/*Structure payload to carry callctrl info
*/
typedef struct cm_call_ctrl_rpt_s{

void*  cc_object_ptr;

} cm_call_ctrl_rpt_s_type;

typedef struct cmsoa_mcc_s
{

  sd_ss_e_type          ss;
  /* Stores the stack of found system.
  */

  uint32                mcc;
  /* GWL system: This stores the MCC extracted from plmn_id
  ** 1X/DO systems: This store the MCC in that local GEO
*/

  sys_plmn_id_s_type    plmn_id;
  /* If GWL system, this is the original plmn where MCC is extracted from */

  sys_sys_mode_e_type   mode;
  /* Storing the mode associated with system found */

  sys_srv_status_e_type  srv_status;
  /*service status of corresponding stack */

} cmsoa_mcc_s_type;

typedef struct cmsoa_regprx_srv_req_s
{
  boolean        hyrb2_on;
  /* Store the current SV mode at the time of this service-request */

  uint8      ss;

}cmsoa_regprx_srv_req_s_type;

/* Structure to store Info related to the MCFG refresh request
*/
typedef struct cm_mcfg_refresh_info_s
{
  sys_modem_as_id_e_type     as_id;
  /* Store the as_id for refresh request  */

  uint32                     reference_id;
  /* Store the reference id sent by MCFG   */
}cm_mcfg_refresh_info_s_type;

/* Structure to store Info related to the DS DATA_PACKET_STATE response
*/
typedef struct cm_data_pkt_state_s
{
  uint8                     subs_id_mask;
  /* Store the subs_id bitmask sent by DS   */
}cm_data_pkt_state_s_type;

/* Union of all generic reports
*/
typedef union cm_generic_rpt_u
{
  cm_dem_rpt_s_type               dem_rpt;

  mmgsdi_event_data_type          mmgsdi_card_rpt;
    /* Used for MMGSDI card related events */

  cm_drop_calls_info_s_type       drop_calls_rpt;
    /* Data needed to drop charged calls when MMGSDI/GSDI read/write
    ** results in an error
    */
  cm_call_ctrl_rpt_s_type         call_ctrl_rpt;

  cmsoa_mcc_s_type                mcc_msg;
    /* mcc reports from SD --> CM indicates which MCC we found
    */

  cmsoa_regprx_srv_req_s_type     regprx_srv_req;
    /* regprx srv-request
    */

  cm_mcfg_refresh_info_s_type     refresh_info;
    /* Info related to the MCFG refresh request
    */

  cm_data_pkt_state_s_type        pkt_data;
    /* Payload associated with DS DATA_PACKET_STATE response for oprt_mode
    ** and suscription loss callbacks.
    */

  void*                           qsh_cb_params;
    /* Parameters passed into the callback functions registered with QSH using 
      ** the function qsh_client_reg(...).      */
} cm_generic_rpt_u_type;


/* Generic Report structure
*/

typedef struct cm_generic_rpt_s{

  cm_hdr_type                 hdr;

  cm_generic_rpt_u_type       generic_rpt;

} cm_generic_rpt_s_type;



/**--------------------------------------------------------------------------
** Externs
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION CM_CALL_ID_ALLOCATE

DESCRIPTION
  Allocates a call object.
  Call ids are allocated in round robin fashion, from 0 to
  CM_CALL_ID_MAX - 1, then it wraps around with next call id
  allocated being 0 again.

DEPENDENCIES
  None

RETURN VALUE
  Call id or CM_CALL_ID_INVALID if CM ran out of call ids.
  If CM_ERR_FATAL() is compiled and CM ran out of call ids, this
  function will die.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_call_id_type cm_call_id_allocate
(
  void
);



/*===========================================================================

FUNCTION CM_CALL_ID_DEALLOCATE

DESCRIPTION
  A synchronous function that deallocates the specified call object

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_call_id_deallocate
(
  cm_call_id_type call_id             /* Call id that is to be deallocated */
);


/*===========================================================================

FUNCTION cm_generic_rpt_get_buf_else_err_fatal

DESCRIPTION
  Allocate a CM report command buffer from the heap. Error
  fatal if allocation is NOT successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated CM report command buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_generic_rpt_s_type *cm_generic_rpt_get_buf_else_err_fatal( void );


/*===========================================================================

FUNCTION CM_GENERIC_RPT

DESCRIPTION
  Allows generic clients to queue up messages to the CM. Dynamically allocate
  a buffer, fill it in, and call this function with it. All this will
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
void cm_generic_rpt
(
  cm_generic_rpt_s_type *cmd_ptr               /* Pointer to a CM report buffer */
);

/*===========================================================================

FUNCTION cm_is_esr_allowed_in_srlte

DESCRIPTION
  This function tells whether ESR is supported in 1XSRLTE mode .

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cm_is_esr_allowed_in_srlte
(
  void
);

/*===========================================================================

FUNCTION cmcall_is_there_vowlan_call_or_drvcc_in_prog

DESCRIPTION
  Tells if there is active VoWlan call or DRVCC in progress.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if Vowlan call is in progress or DRVCC is in progress
  FALSE  otherwise
  
SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_there_vowlan_call_or_drvcc_in_prog(void);

#endif /*CMLL_V_H*/



