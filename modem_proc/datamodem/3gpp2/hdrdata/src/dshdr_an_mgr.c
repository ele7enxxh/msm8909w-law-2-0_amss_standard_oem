/*===========================================================================

                        D S H D R _ A N _ M G R . C
GENERAL DESCRIPTION

  This file is the controller for HDR Access Network Authentication calls.

EXTERNALIZED FUNCTIONS

  dshdr_an_mgr_session_init
    This function is called when an HDR session gets opened and a AN stream
    has been negotiated. It sets up and initializes various RLP/PPP data
    structures for  the AN authentication manager.

  dshdr_an_mgr_session_close
    This function is called when an HDR session is closed. This function
    cleans up the various RLP/PPP data structures for the AN authentication
    manager.

  dshdr_an_mgr_powerup_init
    Does the powerup initialization for the HDR AN Authentication module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  dshdr_an_mgr_powerup_init() needs to be called at powerup. It should be
  called from the DS task. When a HDR session is opened, if the AN stream is
  negotiated, dshdr_an_mgr_session_init is called. When the HDR session is
  closed, dshdr_an_mgr_session_close is called. Callbacks are registered with
  CM that get called when a Call gets connected or Ends.

Copyright (c) 2003-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/dshdr_an_mgr.c#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
07/04/14    sc     Optimize the code to reduce the static memory
06/30/14    niv    Added wrapper functions & API change for 
                   NV_REFRESH 
10/28/13    sd     LLVM Compiler Warnings clean-up - 3GPP2
06/26/13    sc     Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
06/03/13    sd     ATCoP decoupling changes
04/19/12    sb     Feature Cleanup: Always On Flag FEATURE_UIM_SUPPORT_HRPD_AN
04/18/12    jz      Seperate old JCDMA and Simplified JCDMA
03/26/12    msh    API for an auth status
03/21/12    msh    TCB changes due to Core changes  
11/18/11    vpk    HDR SU API cleanup
09/26/11    msh    Fix compiler warnings 
09/09/11    vpk    Fix for empty username during AN authentication
06/06/11    ash    Added support for PPP Partial Context
05/12/11    op     Migrated to MSG 2.0 macros
03/04/11    jee    Merged compiler warning fixes
02/25/11    ms     Support for KDDI special NAI and Password for AN HDR
                   authentication.
02/08/11    ls     Global variable cleanup
03/30/10    ssh    Added dshdr_an_set_user_id()
07/17/09    ls     Removed check HRPD and update RUIM table for CSIM,
                   since it is done by UIM team. Change FEATURE_CSIM
                   to FEATURE_MMGSDI_SESSION_LIB
03/27/09    ls     Set AN authentication status when calling
                   dshdr_an_handle_ppp_auth_ev_cback(). Provided
                   dshdr_an_auth_status_init() to initialize an_auth_status.
                   Provided dshdr_get_an_auth_status() if QMI module or
                   other module wants to get AN authentication status
03/19/09    ms     Fixed Lint errors.
01/20/09    pp     Metainfo optimizations.
01/22/08    scb/sy Mainlined features under FEATURE_JCDMA
09/11/07  sq/mga   Merged changes to support FEATURE_RUIM_FUNC_INTERFACE
08/27/07    mga    Fixed High Lint Errors
06/11/07    as     Added support for DS_HDR_AN_PPP_START_CMD and using it if
                   session is not initialized in ds task context
07/25/06    ksu    EMPA: 1x RLP code review comments
05/30/06    ksu    EMPA: RLP init code review comments
03/06/06   sq      dshdr_an_handle_ppp_ev_cback function, instead of calling
                   dshdr_an_ppp_start(), we post a an_ppp_ev to DS task and
                   let DS task to start an_ppp.
02/14/06   ssh/vas Now reading PPP auth information from NV every time PPP
                   is started
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to
                   dsrlp_reg_srvc() and RX callback function
08/29/05    ksu/sy Remove dsrlp_reg_hdlc() in favor of uint32 app_field
                   parameter to dsrlp_reg_srvc().
07/22/05    kvd    Added fail_reason to ppp_event_callbacks().
06/30/05    sy     Moved the code in dshdr_an_cm_ph_event_cb() to allocate
                   the cmd from DS buffer only if there is a cmd to queue
                   up not otherwise.
03/02/05    ks     Added support for Data Protocol Logging on this iface.
01/18/05    ifk    Register for PPP auth events to determine when auth succeeds
09/09/04    vas    Preliminary QOS support
09/02/04    vas/jkmonenda  Added RUIM support for reading AN auth values from
                   RUIM/supporting CAVE in RUIM
02/04/04    vas    Added code to set ppp config item passive_wakeup_on_data
12/17/03    vas    Do not abort PPP when the call ends
11/17/03    igt    Changes in dshdr_an_get_auth_info_from_nv() to support
                   long uid/passwd fields for AN hdr logins.
10/13/03    vas    Restructured code. hdrrlp_resume() no longer used. The Call
                   End and Connected events are not received from CM, but from
                   HDR protocols.
09/18/03    vas    RLP init and cleanup done when traffic channel brought up
                   or torn down.
09/10/03    mpa    Upgrade F3 messages to use new SSIDs
08/21/03    vas    Code changes to be compatible with the latest PS VU
08/08/03    vas    Cleanup. Code review changes. Abort PPP when call ended.
06/18/03    vas    Added code to maintain the stream AN mgr is initalized on.
02/19/03    vas    Created module

===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include <stringl/stringl.h>

#ifdef FEATURE_HDR

#include "err.h"
#include "queue.h"
#include "rex.h"
#include "amssassert.h"

#include "dsatctab.h"
#include "nv.h"
#include "dstaski.h"
#include "ds3gmgr.h"
#include "ds3gcfgmgr.h"
#include "ps_ppp.h"
#include "dshdr_an_wmk.h"

#include "hdrcom_api.h"

#include "hdrpac.h"
#include "dsrlp_v.h"
#include "hdrrlp.h"
#include "dshdr_an_mgr.h"
#include "ds707_pkt_mgr.h"
#include "ds707_nv_util.h"
#include "ds707.h"
#include "cm.h"
#include "ds707_roaming.h"
#include "nvruimi.h"
#include "uim.h"
#include "uim_v.h"
#include "ps_iface_logging.h"
#include "ps_logging_defs.h"
#include "ps_sys_conf.h"
#include "ds3gmmgsdiif.h"

/* Suppress Lint Errors */
#include "data_err_suppr.h"

#ifdef FEATURE_DATA_PKT_CLASSIFIER
#include "ds707_pkt_classifier.h"
#endif/* FEATURE_DATA_PKT_CLASSIFIER */

/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/

static void dshdr_set_an_auth_status
(
  dshdr_an_auth_status_e_type auth_status
);

/* CDMA service table read and update is handled by UIM team during
 * power on stage. It must be present if session based API is supported
 */
#ifndef FEATURE_MMGSDI_SESSION_LIB

static void dshdr_uim_check_cdma_svc_table
(
  boolean *support_flag,
  cm_rtre_control_e_type rtre_control
);

static void uim_an_hrpd_cdma_svc_init_report
(
  uim_rpt_type *report           /* R-UIM command status report */
);

#ifdef FEATURE_RUIM_FUNC_INTERFACE
static void dshdr_an_ruim_clientid_reg_cb
(

  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr

);
static void dshdr_an_ruim_hrpd_cdma_svc_init_report_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);
#endif /* FEATURE_RUIM_FUNC_INTERFACE */
#endif /* !FEATURE_MMGSDI_SESSION_LIB */

static int dshdr_an_ppp_start
(
  void
);

static void dshdr_on_card_ready_event
(
  boolean is_ready
);

/*===========================================================================
                            TYPEDEFS
===========================================================================*/
#define DSHDR_AN_MAX_PHYS_LINKS 1

#ifdef FEATURE_DATA_PPP_DEFAULT_PASSWD
/*---------------------------------------------------------------------------
  Definitions for default AN PPP password
  Note: The length of the password, PPP_DEFAULT_AN_PASSWORD_LEN is expected
  to not exceed:
  (1) The maximum size of the NV item to hold it,
      NV_MAX_AN_AUTH_PASSWD_LONG_LENGTH (127 bytes); and
  (2) The size of PPP structure to hold it, PPP_MAX_PASSWD_LEN (127 bytes)
  So exercise care in changing this definition so that it fits.
---------------------------------------------------------------------------*/
#define PPP_DEFAULT_AN_PASSWORD       "0000000000000000"       /* 16 bytes */
#define PPP_DEFAULT_AN_PASSWORD_LEN   (sizeof( PPP_DEFAULT_AN_PASSWORD ) - 1)
#endif /* FEATURE_DATA_PPP_DEFAULT_PASSWD */

#ifdef FEATURE_RUIM_FUNC_INTERFACE
#define INVALID_RUIM_CLIENTID 0xFFFFFFFF
#endif /* FEATURE_RUIM_FUNC_INTERFACE */


/*---------------------------------------------------------------------------
  Overall Packet state, for one CM call.  Contains info about the Traffic
  channel and the ps_iface.
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_watermark_type      *tx_watermark_ptr;         /* RLP Tx watermark   */
  void                    (*post_rx_func_ptr)(uint32  rx_func_ptr_cb_data); /* RLP Rx callback    */
  dsm_watermark_type      *post_rx_wmk_ptr;            /* RLP Rx wmk       */
  ps_iface_type            ps_iface;            /* PS Iface used by AN mgr */
  ps_phys_link_type        ps_phys_link[DSHDR_AN_MAX_PHYS_LINKS];
} dshdr_an_state_type;

/*-------------------------------------------------------------------------
  PPP event notification handle
--------------------------------------------------------------------------*/
static void *ppp_event_handle;


/*===========================================================================
                            VARIABLES
===========================================================================*/

static struct
{
  dshdr_an_state_type  state; /* Structure maintaining state of AN mgr     */
  cm_client_id_type    cm_client_id; /* Client Id assigned by CM to AN mgr */
  ppp_auth_info_type   auth_info;          /* PPP auth info used by AN mgr */
  dshdr_an_auth_status_e_type an_auth_status; /* AN authentication status  */

  hdrcom_hai_stream_enum_type active_stream_id;
  /* This variable maintaind which stream the Def Packet App Bound to AN is
     negotiated. It is required mainly because when the HDR session gets
     closed, we need to perform cleanup operation based on the stream. Since
     the AN mgr runs in the DS task which is lower priority than HDR MC, once
     the HDR session is closed, it cannot query the HDR stream protocol to
     determine if a stream had been mapped to the AN, since HDR stream may
     have already cleared this mapping upon processing HDR session close.

     NOTE: Here HDRHAI_STREAM0 means that there is no stream mapped for the
           AN mgr. We can use this because STREAM0 is reserved for signaling
           and can never be used for a Packet Application */
} dshdr_an;

#ifndef FEATURE_MMGSDI_SESSION_LIB

/* This struct contains UIM related Global Variables for this file */
static struct
{
#ifndef FEATURE_RUIM_FUNC_INTERFACE
  uim_cmd_type      uim_cmd;
#else
  mmgsdi_client_id_type client_id;
#endif
  boolean           report_status;
  byte              cdma_svc_table_buffer[UIM_CDMA_SVC_TABLE_SIZE];
} dshdr_uim;

#endif /*!FEATURE_MMGSDI_SESSION_LIB */

/* This variable is used to keep track of the previous CDMA subscription avail
status.  If the status changes, queue a CDMA subs available event so that the
HDR NAI is re-read */

static boolean dshdr_cdma_subs_available=FALSE;

/* Flag to indicate UIM card is ready  */
static boolean dshdr_card_ready = FALSE;

/* This variable is used to keep track if a PPP start is pending  */
static boolean dshdr_an_ppp_start_pending = FALSE;

/*===========================================================================
                       INTERNAL MACRO DEFINITIONS
===========================================================================*/


/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DSHDR_AN_UM_RLP_FWD_FUNC

DESCRIPTION   Called (via function pointer) by RLP when data is received on
              AN authentication stream. This will signal PS task that there
              is reverse link RLP/PPP data to process.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void dshdr_an_um_rlp_fwd_func
(
  uint32 unused
)
{
  /* Suppress Lint Error */
  DATA_MDM_ARG_NOT_USED(unused);

  PS_SET_SIGNAL(PS_PPP_UM_AN_RX_SIGNAL);
} /* dshdr_an_um_rlp_fwd_func() */

/*===========================================================================
FUNCTION DSHDR_AN_GET_AUTH_INFO_FROM_NV

DESCRIPTION
  This function gets the Authentication info from the NV items for AN
  authentication and stores them in the appropriate variables.

DEPENDENCIES
  The NV task must be started and running.

RETURN VALUE
  None

SIDE EFFECTS
 Each call to 'dsi_get_auth_from_nv' results in a Wait and watchdog kicking
===========================================================================*/
static void dshdr_an_get_auth_info_from_nv
(
  void
)
{
  nv_stat_enum_type    nv_status;             /* status from NV call       */
  boolean              long_uid_pass = FALSE; /* long user/passwd fields?  */
  nv_item_type         ds707_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Read the NAI. This can be read from NV. If NV is not written,
    then NAI len is loaded as zero. First try the long userid/passwd and if
    not available then try the short fields
  -------------------------------------------------------------------------*/
  nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_HDR_AN_AUTH_USER_ID_LONG_I, &ds707_nv_item, ds707_curr_subs_id());

  /*-------------------------------------------------------------------------
    Check if the NV has valid data inside and load the data from NV to local
    item
  -------------------------------------------------------------------------*/
  if((nv_status == NV_DONE_S) &&
     (ds707_nv_item.hdr_an_auth_user_id_long.user_id_len <=
         NV_MAX_AN_AUTH_USER_ID_LONG_LENGTH))
  {
    long_uid_pass = TRUE;
    dshdr_an.auth_info.user_id_len =
        ds707_nv_item.hdr_an_auth_user_id_long.user_id_len;

    memscpy( dshdr_an.auth_info.user_id_info,
             PPP_MAX_USER_ID_LEN,
             (char *)ds707_nv_item.hdr_an_auth_user_id_long.user_id,
             ds707_nv_item.hdr_an_auth_user_id_long.user_id_len );
  }
  else
  {
    /*-----------------------------------------------------------------------
      In this case set this to FALSE as we should not even try to read the
      long passwd field.
    -----------------------------------------------------------------------*/
    long_uid_pass = FALSE;

    /*-----------------------------------------------------------------------
      If NV was never written then the length of the User_id is made as 0.
      This will ensure that the lengths in the CHAP packet are made zero.
    -----------------------------------------------------------------------*/
    dshdr_an.auth_info.user_id_len = 0;
    DATA_HDR_MSG0 (MSG_LEGACY_ERROR, "AN CHAP LONG NAI Invalid/never written");
  }

  if (long_uid_pass)
  {

    /*-----------------------------------------------------------------------
      Read the CHAP password. This can be read from NV. If NV is not written,
      then default values are loaded.
    -----------------------------------------------------------------------*/
    nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_HDR_AN_AUTH_PASSWD_LONG_I, &ds707_nv_item, ds707_curr_subs_id());

    /*-----------------------------------------------------------------------
      Check if the NV has valid data inside and load the data from NV to
      local item
    -----------------------------------------------------------------------*/
    if(( nv_status == NV_DONE_S) &&
       (ds707_nv_item.hdr_an_auth_passwd_long.passwd_len <=
        NV_MAX_AN_AUTH_PASSWD_LONG_LENGTH ))
    {
      dshdr_an.auth_info.passwd_len =
        ds707_nv_item.hdr_an_auth_passwd_long.passwd_len;

      // IGT make sure the receiving data structure has enough space or redefine!!
      memscpy( dshdr_an.auth_info.passwd_info,
               PPP_MAX_PASSWD_LEN,
               (char *) ds707_nv_item.hdr_an_auth_passwd_long.passwd,
               ds707_nv_item.hdr_an_auth_passwd_long.passwd_len );
    }
    else
    {

      /*---------------------------------------------------------------------
        If we failed to read the long PASSWD then set this to FALSE
      ---------------------------------------------------------------------*/
      long_uid_pass = FALSE;

      /*---------------------------------------------------------------------
        If NV was never written then the length password is made as 0. This
        will ensure that the lengths in the CHAP packet are made zero.
      ---------------------------------------------------------------------*/
      dshdr_an.auth_info.passwd_len = 0;
      DATA_HDR_MSG0 (MSG_LEGACY_ERROR, "AN CHAP Long Passwd Invalid/never written");
    }

  }/* if (long_uid_pass) read long PASSWD */


  /*-------------------------------------------------------------------------
    If ither the long UID or the long PASSWD nv reads failed then try to
    read from the legacy fields.
  -------------------------------------------------------------------------*/
  if (long_uid_pass == FALSE)
  {
    /*-----------------------------------------------------------------------
      Read the NAI. This can be read from NV. If NV is not written,
      then NAI len is loaded as zero.
    -----------------------------------------------------------------------*/
    nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_HDR_AN_AUTH_NAI_I, &ds707_nv_item, ds707_curr_subs_id());

    /*-----------------------------------------------------------------------
      Check if the NV has valid data inside and load the data from NV to
      local item
    -----------------------------------------------------------------------*/
    if(( nv_status == NV_DONE_S) &&
       (ds707_nv_item.hdr_an_auth_nai.nai_len <= NV_MAX_AN_AUTH_NAI_LENGTH ))
    {
      dshdr_an.auth_info.user_id_len = ds707_nv_item.hdr_an_auth_nai.nai_len;
      memscpy( dshdr_an.auth_info.user_id_info,
               PPP_MAX_USER_ID_LEN,
               (char *) ds707_nv_item.hdr_an_auth_nai.nai,
               ds707_nv_item.hdr_an_auth_nai.nai_len );
    }
    else
    {
      /*---------------------------------------------------------------------
        If NV was never written then the length of the User_id is made as 0.
        This will ensure that the lengths in the CHAP packet are made zero.
      ---------------------------------------------------------------------*/
      dshdr_an.auth_info.user_id_len = 0;
      DATA_HDR_MSG0 (MSG_LEGACY_ERROR, "AN CHAP NAI Invalid or never written");
    }

    /*-----------------------------------------------------------------------
      Read the CHAP password. This can be read from NV. If NV is not written,
      then default values are loaded.
    -----------------------------------------------------------------------*/
    nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_HDR_AN_AUTH_PASSWORD_I, &ds707_nv_item, ds707_curr_subs_id());

    /*-----------------------------------------------------------------------
      Check if the NV has valid data inside and load the data from NV to
      local item
    -----------------------------------------------------------------------*/
    if(( nv_status == NV_DONE_S) &&
       (ds707_nv_item.hdr_an_auth_password.password_len <=
        NV_MAX_AN_AUTH_PASSWORD_LENGTH ))
    {
      dshdr_an.auth_info.passwd_len =
        ds707_nv_item.hdr_an_auth_password.password_len;
      memscpy( dshdr_an.auth_info.passwd_info,
               PPP_MAX_PASSWD_LEN,
               (char *) ds707_nv_item.hdr_an_auth_password.password,
               ds707_nv_item.hdr_an_auth_password.password_len );
    }
    else
    {
      /*---------------------------------------------------------------------
        If NV was never written then the length password is made as 0. This
        will ensure that the lengths in the CHAP packet are made zero.
       --------------------------------------------------------------------*/
      dshdr_an.auth_info.passwd_len = 0;
      DATA_HDR_MSG0 (MSG_LEGACY_ERROR, "AN CHAP Passwd Invalid or never written");
    }
  }/* if (long_uid_pass == FALSE) */
#ifdef FEATURE_DATA_PPP_DEFAULT_PASSWD
  /*-------------------------------------------------------------------------
    If password length is 0, configure default AN PPP password.
  -------------------------------------------------------------------------*/
  if( dshdr_an.auth_info.passwd_len == 0 )
  {
    /*-------------------------------------------------------------------------
      Set PPP config with default values
    -------------------------------------------------------------------------*/
    if( PPP_DEFAULT_AN_PASSWORD_LEN <= PPP_MAX_PASSWD_LEN )
    {
      dshdr_an.auth_info.passwd_len = PPP_DEFAULT_AN_PASSWORD_LEN;
      memscpy( dshdr_an.auth_info.passwd_info,
               PPP_MAX_PASSWD_LEN,
               PPP_DEFAULT_AN_PASSWORD,
               PPP_DEFAULT_AN_PASSWORD_LEN );
    }
    else
    {
      dshdr_an.auth_info.passwd_len = 0;
      DATA_HDR_MSG0(MSG_LEGACY_ERROR,
                    "Default PPP password too long to fit in PPP config. "
                    "AN PPP password not configured");
    }
  }

#endif /* FEATURE_DATA_PPP_DEFAULT_PASSWD */



} /* dshdr_an_get_auth_info_from_nv() */

/*===========================================================================
FUNCTION      DSHDR_AN_SETUP_RLP

DESCRIPTION
  Sets up RLP with a TX watermark and RLP RX func and RLP RX queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dshdr_an_setup_rlp( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  dshdr_an_wmk_setup_wmks();

  dshdr_an.state.post_rx_wmk_ptr    = get_dshdr_an_wmk_um_fwd_wmk_ptr();
  dshdr_an.state.tx_watermark_ptr = get_dshdr_an_wmk_um_rev_wmk_ptr();
  dshdr_an.state.post_rx_func_ptr = dshdr_an_um_rlp_fwd_func;

  dsrlp_reg_srvc( DSRLP_HDR_AN_AUTH_SR_ID_INDEX,
                  DSRLP_VER_HDR,
                  get_dshdr_an_wmk_um_rev_wmk_ptr(),
                  dshdr_an_um_rlp_fwd_func,
                  get_dshdr_an_wmk_um_fwd_wmk_ptr(),
                  DSRSP_ROUTE_A,
                  0 );
} /* dshdr_an_setup_rlp() */

/*===========================================================================
                 FUNCTIONS FOR PPP/RLP INTERACTION
===========================================================================*/

/*===========================================================================
FUNCTION      DSHDR_AN_RX_UM_DATA

DESCRIPTION   Called by PPP to get forward-link data from RLP.  Specific to
              HDR AN authentication Data.

DEPENDENCIES  None.

RETURN VALUE  A pointer to a DSM item.  DSM item may be a packet chain.
              If there is no data to return, then returns NULL.

SIDE EFFECTS  None.
===========================================================================*/
static dsm_item_type* dshdr_an_rx_um_data(void *user_data)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Suppress Lint Error */
  DATA_MDM_ARG_NOT_CONST(user_data);
  return((dsm_item_type *)dsm_dequeue(dshdr_an.state.post_rx_wmk_ptr));
} /* dshdr_an_rx_um_data() */

/*===========================================================================
FUNCTION      DSHDR_AN_TX_UM_DATA

DESCRIPTION   Called by PPP to transmit rev-link data over RLP to the base
              station.  This is for the HDR AN authentication data.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void dshdr_an_tx_um_data
(
  dsm_item_type   **dsm_item_ptr_ptr,
  void             *meta_info_ptr,
  void *user_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Suppress Lint Error */
  DATA_MDM_ARG_NOT_CONST(*dsm_item_ptr_ptr);
  DATA_MDM_ARG_NOT_USED(user_data);
  DATA_MDM_ARG_NOT_CONST(user_data);

  /*------------------------------------------------------------------------
    Clear meta info!
  ------------------------------------------------------------------------*/
  if ((ps_meta_info_type *)meta_info_ptr != NULL)
  {
    PS_META_INFO_FREE((ps_meta_info_type **)&meta_info_ptr);
  }

  dsm_enqueue(dshdr_an.state.tx_watermark_ptr, dsm_item_ptr_ptr);

#ifdef FEATURE_DATA_PKT_CLASSIFIER
  ds707_pkt_classifier_route_an_tx_um_data();
#endif/* FEATURE_DATA_PKT_CLASSIFIER */
} /* dshdr_an_tx_um_data() */

/*===========================================================================
FUNCTION      DSHDR_AN_HANDLE_PPP_EV_CBACK()

DESCRIPTION   Callback function called when a PPP event occurs.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void dshdr_an_handle_ppp_ev_cback
(
  ppp_dev_enum_type   ppp_dev,
  ppp_protocol_e_type protocol,
  ppp_event_e_type    ppp_event,                 /* Down/Up/Resync         */
  void               *user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  fail_reason
)
{
  ds_cmd_type  *cmd_ptr;                      /* Pointer to command buffer */
  ds707_ppp_ev_cmd_type *ds707_ppp_ev_cmd_type_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Suppress Lint Error */
  DATA_MDM_ARG_NOT_CONST(user_data);
  
  ASSERT(ppp_dev == PPP_UM_AN_DEV);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_ppp_ev_cmd_type));
  /** Get command buffer to do task switch */
  if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_HDR_AN_PPP_EV;
    ds707_ppp_ev_cmd_type_ptr = (ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr;
      /** Copy the values into the command buffer */
    ds707_ppp_ev_cmd_type_ptr->ppp_dev = ppp_dev;
    ds707_ppp_ev_cmd_type_ptr->protocol = protocol;
    ds707_ppp_ev_cmd_type_ptr->ppp_event = ppp_event;
    ds707_ppp_ev_cmd_type_ptr->user_data = user_data;
    ds707_ppp_ev_cmd_type_ptr->session_handle = session_handle;
    ds707_ppp_ev_cmd_type_ptr->fail_reason = fail_reason;

    ds_put_cmd( cmd_ptr );
  }
} /* dshdr_an_handle_ppp_ev_cback() */

/*===========================================================================
FUNCTION      DSHDR_AN_HANDLE_PPP_AUTH_EV_CBACK()

DESCRIPTION   Callback function called when a PPP AUTH event occurs.

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void dshdr_an_handle_ppp_auth_ev_cback
(
  ppp_dev_enum_type   ppp_dev,
  ppp_protocol_e_type protocol,
  ppp_event_e_type    ppp_event,                 /* SUCCESS/FAILURE        */
  void               *user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  fail_reason
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Suppress Lint Error */
  DATA_MDM_ARG_NOT_CONST(user_data);
  DATA_MDM_ARG_NOT_USED(session_handle);
  DATA_MDM_ARG_NOT_USED(fail_reason);

  
  ASSERT((ppp_dev == PPP_UM_AN_DEV) &&
         (protocol == PPP_PROTOCOL_AUTH));

  switch(ppp_event)
  {
    case PPP_SUCCESS_EV:
      hdrpac_set_an_auth_status( TRUE );
      dshdr_set_an_auth_status( DSHDR_PASS_AUTH );
      break;

    case PPP_FAILURE_EV:
      hdrpac_set_an_auth_status( FALSE );
      dshdr_set_an_auth_status( DSHDR_FAILED_AUTH );
      break;

    default:
      DATA_HDR_MSG1 (MSG_LEGACY_ERROR, "Unsupported PPP AUTH event %d",
                      ppp_event);
      ASSERT(0);
  }
} /* dshdr_an_handle_ppp_auth_ev_cback() */

/*===========================================================================
FUNCTION      DSHDR_AN_PPP_START

DESCRIPTION   Called when IFACE is coming up, to intialize PPP for the
              AN PPP.  This the interface used for HDR AN Authentication.

DEPENDENCIES  None

RETURN VALUE  TRUE for successful PPP config, FALSE otherwise

SIDE EFFECTS  This function shall be called only in the DS task.
===========================================================================*/
int dshdr_an_ppp_start
(
  void
)
{
  ppp_dev_opts_type*  ppp_config_info = NULL;
  boolean             result          = FALSE;
  int                 ret_value       = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_config_info = 
            modem_mem_alloc(sizeof(ppp_dev_opts_type), MODEM_MEM_CLIENT_DATA);
  if (ppp_config_info == NULL)
  {
    DATA_DOQOS_MSG1(MSG_LEGACY_ERROR, 
                    "Unable to allocate memory for ppp_config_info %d",
                    ppp_config_info);
    return -1;
  }

  memset((void*)ppp_config_info, 0, sizeof(ppp_dev_opts_type));

  ppp_get_default_opts( ppp_config_info );

  /*-------------------------------------------------------------------------
    Initialize PPP such LCP is in Passive mode (does not send LCP Config
    Request  unless a LCP Config Request is received from the network) and
    IPCP is disabled
  -------------------------------------------------------------------------*/
  ppp_config_info->rx_f_ptr                 = dshdr_an_rx_um_data;
  ppp_config_info->tx_f_ptr                 = dshdr_an_tx_um_data;
  ppp_config_info->ipcp_info.rx_iface_ptr   = &dshdr_an.state.ps_iface;
  ppp_config_info->lcp_info.mode            = PPP_CP_PASSIVE;
  ppp_config_info->ipcp_info.mode           = PPP_CP_DISABLED;
  ppp_config_info->hdlc_mode                = HDLC_SW;
  ppp_config_info->passive_wakeup_on_data   = TRUE;

  /*-------------------------------------------------------------------------
    Read the PPP auth information from NV
  -------------------------------------------------------------------------*/
  dshdr_an_get_auth_info_from_nv();
  memscpy( &ppp_config_info->auth_info, sizeof(ppp_auth_info_type),
           &dshdr_an.auth_info, sizeof(dshdr_an.auth_info) );
  /*Simplified JCDMA: 
    3.3.1 Special NAI and Password for terminal authentication [D2-1 No.39]*/
  if (ds707_roaming_is_curr_mode_jcdma()||
      ds707_roaming_is_simplified_jcdma_mode())
  {
    if(ds707_roaming_is_curr_mode_jcdma())
  {
    ds707_configure_jcdma_params_for_ppp(ppp_config_info);
    }

    if(nv_rtre_control() == NV_RTRE_CONTROL_NO_RUIM)
    {
      result =
          ds707_configure_jcdma_special_an_auth_params(&dshdr_an.auth_info);
      if(result == FALSE)
      {
        DATA_HDR_MSG0(MSG_LEGACY_MED,"Failed reading NAI and Password" );
        modem_mem_free(ppp_config_info, MODEM_MEM_CLIENT_DATA);
        return  -1;
      }
      memscpy( &ppp_config_info->auth_info, sizeof(ppp_auth_info_type),
               &dshdr_an.auth_info, sizeof(dshdr_an.auth_info) );
    }
  }

#ifdef FEATURE_DATA_PKT_CLASSIFIER
  ds707_pkt_classifier_set_an_ppp_config(ppp_config_info);
#endif/* FEATURE_DATA_PKT_CLASSIFIER */

  ret_value = ppp_start( PPP_UM_AN_DEV, ppp_config_info );
  
  modem_mem_free(ppp_config_info, MODEM_MEM_CLIENT_DATA);
  
  return ret_value;
} /* dshdr_an_ppp_start() */

#ifdef FEATURE_DATA_PKT_CLASSIFIER
void ds_hdr_an_ppp_start_pkt_classifier
(
  void
)
{
  (void)dshdr_an_ppp_start();
} /* ds_hdr_an_ppp_start_pkt_classifier */
#endif/* FEATURE_DATA_PKT_CLASSIFIER */

/*===========================================================================
                             OTHER FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DSHDR_CHECK_AND_START_AN_PPP

DESCRIPTION
  This function starts AN PPP, checks if card is available then waits for
  subcription ready to start it, other wise starts PPP session immediately.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dshdr_check_and_start_an_ppp(void)
{

  nv_stat_enum_type status = NV_DONE_S;
  nv_item_type      ds707_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check if its a valid card thing */
  status = ds3gcfgmgr_read_legacy_nv_ex( NV_RTRE_CONFIG_I, &ds707_nv_item, ds707_curr_subs_id() );
  if ( status == NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
     Check if the config is RUIM
    -----------------------------------------------------------------------*/
    if(ds707_nv_item.rtre_config == NV_RTRE_CONFIG_RUIM_ONLY) {

      /* If card is valid and card is not ready wait for ready*/
      if (!dshdr_card_ready)
      {
        dshdr_an_ppp_start_pending = TRUE;
        return;
      }
    }
  }

  /* if card is not used */
  dshdr_an_ppp_start();
  return;
}

/*===========================================================================
FUNCTION      DSHDR_AN_MGR_SESSION_INIT

DESCRIPTION
  This function sets up and initializes various RLP/PPP data structures for
  the AN authentication manager. The function gets called when a HDR session
  is opened.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dshdr_an_mgr_session_init
(
  hdrcom_hai_stream_enum_type stream_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds_cmd_type  *cmd_ptr;

  ASSERT(stream_id >= HDRHAI_STREAM1);

  if(stream_id >= HDRHAI_STREAM1)
  {
    /*---------------------------------------------------------------------
      Upon session configuration complete, enable flow on the AN stream,
      if it has been negotiated.  Flow is always enabled for AN stream to
      support AN-initiated authentication procedures.
    ---------------------------------------------------------------------*/
    if(hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_AN) )
    {
      /* Set the stream negotiated for the AN packet App */
      dshdr_an.active_stream_id = stream_id;

      /* Register the RLP<->PPP watermarks */
      dshdr_an_setup_rlp();

      /* Initialize the AN PPP in passive mode */
      if (IS_IN_DS_TASK())
      {
        (void)dshdr_check_and_start_an_ppp();
      }
      else  /*the curr tast is not DS, post cmd to DS */
      {
        if((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
        {
          ASSERT(0);
          return;
        }
        else
        {
          cmd_ptr->hdr.cmd_id = DS_CMD_HDR_AN_PPP_START;
          ds_put_cmd( cmd_ptr );
        }
      }
    }
  }
  else
  {
    DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Invalid AN stream %d", stream_id);
  }

} /* dshdr_an_mgr_session_init() */

/*===========================================================================
FUNCTION      DSHDR_AN_MGR_SESSION_CLOSE

DESCRIPTION
  This function cleans up the various RLP/PPP data structures for
  the AN authentication manager. The function gets called when a HDR session
  is closed.

DEPENDENCIES
  This function should be called before the PAC actually discards the
  stream mapping for the sesison.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dshdr_an_mgr_session_close
(
  hdrcom_hai_stream_enum_type stream_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(stream_id >= HDRHAI_STREAM1);

  if(stream_id >= HDRHAI_STREAM1)
  {
    /* Ensure that the stream we are cleaning up is the same that we
       activated the AN mgr on */
    if( (dshdr_an.active_stream_id != HDRHAI_STREAM0) &&
        (dshdr_an.active_stream_id == stream_id) )
    {
      /* For sanity, clean RLP watermarks */
      dshdr_an_wmk_cleanup_wmks();

      /* Abort the AN PPP */
      DATA_HDR_MSG0 (MSG_LEGACY_MED, "Abort ppp on HDR session close");
      (void)ppp_abort( PPP_UM_AN_DEV );

      dshdr_an_ppp_start_pending = FALSE;
      /* Reset the active AN stream to be Stream 0, which means invalid
         stream */
      dshdr_an.active_stream_id = HDRHAI_STREAM0;
    }
  }
  else
  {
    DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Invalid AN stream %d", stream_id);
  }
} /* dshdr_an_mgr_session_close() */



/*===========================================================================
FUNCTION      DSHDR_AN_CM_PH_EVENT_CB

DESCRIPTION   This is the callback function called when a phone event occurs
              The events registered for are:
              CM_PH_EVENT_SUBSCRIPTION_AVAILABLE
              CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE
              CM_PH_EVENT_SUBSCRIPTION_CHANGED

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_an_cm_ph_event_cb
(
  cm_ph_event_e_type           ph_event,
  const cm_ph_info_s_type      *ph_info_ptr
)
{
  ds_cmd_type  *cmd_ptr;                      /* Pointer to command buffer */
  cm_rtre_control_e_type *cm_rtre_control_e_type_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  

  switch( ph_event )
  {
    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_CHANGED:
      if(ph_info_ptr->is_cdma_subscription_available != dshdr_cdma_subs_available)
      {
        cmd_ptr = ds_allocate_cmd_buf(sizeof(cm_rtre_control_e_type));
        /** Get command buffer to do task switch */
        if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
        {
          ASSERT(0);
          return;
        }
        else
        {
          /* The CDMA subscription available status has changed, update the variable
          and queue a command to re-read the HDR NAI */
          dshdr_cdma_subs_available = ph_info_ptr->is_cdma_subscription_available;
          /*---------------------------------------------------------------------
          Fill in the command parameters and send the command.
          ---------------------------------------------------------------------*/
          cm_rtre_control_e_type_ptr = 
            (cm_rtre_control_e_type* ) cmd_ptr->cmd_payload_ptr;

          cmd_ptr->hdr.cmd_id = DS_CMD_HDR_CDMA_SUBS_AVAIL_EVENT;
          *cm_rtre_control_e_type_ptr = ph_info_ptr->rtre_control;
          ds_put_cmd( cmd_ptr );
        }
      }
      break;

    default:
      DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Unregistered ph event %d rcvd",
                    ph_event);
      break;
  }
} /* dshdr_an_cm_ph_event_cb() */

#ifndef FEATURE_MMGSDI_SESSION_LIB
 /*===========================================================================

FUNCTION UIM_AN_HRPD_CDMA_SVC_INIT_REPORT                         INTERNAL FUNCTION

DESCRIPTION
  Accept status reports from R-UIM and prints out message for CDMA SVC table
  check


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void uim_an_hrpd_cdma_svc_init_report
(
  uim_rpt_type *report           /* R-UIM command status report */
)
{
  if(report->rpt_status == UIM_PASS)
  {
    dshdr_uim.report_status = TRUE;
    
  }
  else
  {
    dshdr_uim.report_status = FALSE;
    DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Could Not Read CDMA Service Table!");
  }
  (void)rex_set_sigs( dsi_get_ds_tcb(), DS_UIM_CMD_SIG );
} /* uim_an_hrpd_cdma_svc_init_report */


/*===========================================================================

FUNCTION DSHDR_UIM_CHECK_CDMA_SVC_TABLE

DESCRIPTION
  This function issues a read to the CDMA service table to see if HDR is
  supported on the R-UIM.  It is called during initialization.

PARAMETERS

  boolean   *support_flag       a pointer to a boolean type to be set if support
                                is available

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS


===========================================================================*/
void dshdr_uim_check_cdma_svc_table(boolean *support_flag,
                                    cm_rtre_control_e_type rtre_control)
{
  uim_svc_table_return_type support;

#ifdef FEATURE_RUIM_FUNC_INTERFACE
  mmgsdi_return_enum_type status ;
#endif/*FEATURE_RUIM_FUNC_INTERFACE*/

  *support_flag = FALSE;

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
#ifdef FEATURE_RUIM_FUNC_INTERFACE
  if (CM_RTRE_CONTROL_RUIM!= rtre_control)
#else
  if (NV_RTRE_CONTROL_USE_RUIM != nv_rtre_control())
#endif
  {
    return;
  }
  memset(dshdr_uim.cdma_svc_table_buffer, 0, UIM_CDMA_SVC_TABLE_SIZE);
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

#ifndef FEATURE_RUIM_FUNC_INTERFACE
  /* Read the CDMA service table */
  dshdr_uim.uim_cmd.access_uim.num_bytes = UIM_CDMA_SVC_TABLE_SIZE;
  dshdr_uim.uim_cmd.access_uim.data_ptr  = dshdr_uim.cdma_svc_table_buffer;
  dshdr_uim.uim_cmd.access_uim.offset    = 0;
  dshdr_uim.uim_cmd.access_uim.item      = UIM_CDMA_CDMA_SVC_TABLE;
  dshdr_uim.uim_cmd.access_uim.access    = UIM_READ_F;

  dshdr_uim.uim_cmd.access_uim.hdr.command = UIM_ACCESS_F;
  dshdr_uim.uim_cmd.access_uim.hdr.cmd_hdr.task_ptr
                                      = NULL;
  dshdr_uim.uim_cmd.access_uim.hdr.cmd_hdr.sigs
                                      = 0;
  dshdr_uim.uim_cmd.access_uim.hdr.cmd_hdr.done_q_ptr
                                      = NULL;
  dshdr_uim.uim_cmd.hdr.rpt_function       = uim_an_hrpd_cdma_svc_init_report;
  dshdr_uim.uim_cmd.hdr.protocol           = UIM_CDMA;
  dshdr_uim.uim_cmd.access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;
  dshdr_uim.uim_cmd.access_uim.hdr.slot    = UIM_SLOT_AUTOMATIC;

#endif /* ! FEATURE_RUIM_FUNC_INTERFACE*/

  /*-----------------------------------------------------------------------
    Send the command to the R-UIM:
    Clear the "command done signal"
    Send the command
    Wait for the command to be done
  -----------------------------------------------------------------------*/
  (void) rex_clr_sigs( rex_self(), DS_UIM_CMD_SIG );

#ifndef FEATURE_RUIM_FUNC_INTERFACE
  uim_cmd( &dshdr_uim.uim_cmd );
#else

  if( dshdr_uim.client_id ==(mmgsdi_client_data_type)INVALID_RUIM_CLIENTID)
  {
    DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Register RUIM Client ID");
    status  =  mmgsdi_client_id_reg(dshdr_an_ruim_clientid_reg_cb,(mmgsdi_client_data_type)0);
    if(status != MMGSDI_SUCCESS)
    {
      DATA_HDR_MSG0(MSG_LEGACY_ERROR, "RUIM registration failed");
      return;
    }
    (void) dsi_wait(DS_UIM_CMD_SIG);
    (void) rex_clr_sigs( rex_self(), DS_UIM_CMD_SIG );
   }

  /* If mmgsdi_client_id_reg fails then return */
  if( dshdr_uim.client_id ==(mmgsdi_client_data_type)INVALID_RUIM_CLIENTID)
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Invalid UIM Client ID");
    return;
  }

  status =  mmgsdi_read_transparent(dshdr_uim.client_id,
                                    MMGSDI_SLOT_AUTOMATIC,
                                    MMGSDI_CDMA_SVC_TABLE,
                                    0,
                                    UIM_CDMA_SVC_TABLE_SIZE,
                                    dshdr_an_ruim_hrpd_cdma_svc_init_report_cb,
                                    (mmgsdi_client_data_type)0);

  if (status != MMGSDI_SUCCESS)
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "RUIM read failure ");
    return;
  }

#endif /* !FEATURE_RUIM_FUNC_INTERFACE*/

  DATA_HDR_MSG0(MSG_LEGACY_MED,"BLOCKING DS TASK ON UIM CMD SIGNAL - UIM READ");
  ASSERT(IS_IN_DS_TASK());
  dsi_wait(DS_UIM_CMD_SIG);
  DATA_HDR_MSG0(MSG_LEGACY_MED,"UNBLOCKING DS TASK ON UIM CMD SIGNAL - UIM READ");

  if(dshdr_uim.report_status == TRUE)
  {
    
  }
  else
  {
    DATA_HDR_MSG0(MSG_LEGACY_HIGH, "UIM-READ FAILURE");
    /* Failed reading the service table */
    
   return;
  }

  support = uim_return_cdma_svc_availabililty (UIM_CDMA_SVC_HRPD,
                            dshdr_uim.cdma_svc_table_buffer);

  if(support.allocated && support.activated)
  {
    *support_flag = TRUE;
  }
  return;
} /* dshdr_check_cdma_svc_table */
#endif /* !FEATURE_MMGSDI_SESSION_LIB */
/*===========================================================================

FUNCTION DSHDR_PROCESS_AN_PPP_EV

DESCRIPTION
  This function processes HDR AN PPP event commands. A command processing function is
  called based on the type of command received.

DEPENDENCIES
  HDR AN authentication module should have been initialized prior to calling
  this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void  dshdr_process_an_ppp_ev
(
  ds_cmd_type  *cmd_ptr
)
{
  ds707_ppp_ev_cmd_type *ds707_ppp_ev_cmd_type_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Suppress Lint Error */
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);
  if (cmd_ptr->cmd_payload_ptr == NULL)
  {
    return;
  }
  ds707_ppp_ev_cmd_type_ptr = (ds707_ppp_ev_cmd_type* ) cmd_ptr->cmd_payload_ptr;

  DATA_HDR_MSG1 (MSG_LEGACY_ERROR, "PPP event %d",
                     ds707_ppp_ev_cmd_type_ptr->ppp_event);
  switch(ds707_ppp_ev_cmd_type_ptr->ppp_event)
  {
    case PPP_DOWN_EV:
    {
      /* Clean RLP watermarks */
      
      dshdr_an_wmk_cleanup_wmks();

      /* Restart AN PPP in passive mode if the PPP comes down */
      if(hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_AN) )
      {
        
        (void)dshdr_an_ppp_start();
      }
      break;
    }
    default:
    {
     ASSERT(0);
    }
  }
}  /* dshdr_process_an_ppp_ev */

/*===========================================================================

FUNCTION DSHDR_AN_PROCESS_CMD

DESCRIPTION
  This function processes HDR AN commands. A command processing function is
  called based on the type of command received.

  This function should be called by the top-level task dispatcher whenever
  any HDR AN command is received.

DEPENDENCIES
  HDR AN authentication module should have been initialized prior to calling
  this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  dshdr_an_process_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
#ifndef FEATURE_MMGSDI_SESSION_LIB
  boolean support_an_hrpd = FALSE;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

  ASSERT( (cmd_ptr != NULL) );

  DATA_HDR_MSG1 (MSG_LEGACY_MED,"DS AN HDR processing cmd %d",
                 cmd_ptr->hdr.cmd_id );

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  switch( cmd_ptr->hdr.cmd_id )
  {
    case DS_CMD_HDR_CDMA_SUBS_AVAIL_EVENT:
#ifndef FEATURE_MMGSDI_SESSION_LIB
      if( (cmd_ptr->cmd_payload_ptr != NULL) )
      {
        dshdr_uim_check_cdma_svc_table
          (&support_an_hrpd,*((cm_rtre_control_e_type* ) cmd_ptr->cmd_payload_ptr));
      }
      uim_init_an_hrpd_preference(support_an_hrpd);
#endif /* FEATURE_MMGSDI_SESSION_LIB */
      break;

    case DS_CMD_HDR_AN_PPP_EV:
    {
      dshdr_process_an_ppp_ev(cmd_ptr);
      break;
    }
    case DS_CMD_HDR_AN_PPP_START:
    {
      (void)dshdr_check_and_start_an_ppp();
      break;
    }

    default:
      DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Unrecognized cmd %d rcvd",
                    cmd_ptr->hdr.cmd_id);
      break;
  }
} /* dshdr_an_process_cmd() */

#ifndef FEATURE_MMGSDI_SESSION_LIB
#ifdef FEATURE_RUIM_FUNC_INTERFACE

/*===========================================================================
FUNCTION      dshdr_an_ruim_hrpd_cdma_svc_init_report_cb

DESCRIPTION   This is the call back function for mmgsdi_read_transparent

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

void dshdr_an_ruim_hrpd_cdma_svc_init_report_cb (mmgsdi_return_enum_type status,
                                      mmgsdi_cnf_enum_type    cnf,
                                      const mmgsdi_cnf_type  *cnf_ptr)
{
  dshdr_uim.report_status = FALSE;
  if(status == MMGSDI_SUCCESS && cnf == MMGSDI_READ_CNF)
  {
    memscpy(dshdr_uim.cdma_svc_table_buffer, UIM_CDMA_SVC_TABLE_SIZE,
	    cnf_ptr->read_cnf.read_data.data_ptr, cnf_ptr->read_cnf.read_data.data_len);

    
    dshdr_uim.report_status=TRUE;
  }
  else
  {
    DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Could Not Read CDMA Service Table!");
  }
  (void)rex_set_sigs( dsi_get_ds_tcb(), DS_UIM_CMD_SIG );
}


/*===========================================================================
FUNCTION        dshdr_an_ruim_clientid_reg_cb

DESCRIPTION   This is the call back function for mmgsdi_client_id_reg

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

static void dshdr_an_ruim_clientid_reg_cb (mmgsdi_return_enum_type status,
                          mmgsdi_cnf_enum_type    cnf,
                          const mmgsdi_cnf_type  *cnf_ptr)
{
  if (status==MMGSDI_SUCCESS && cnf== MMGSDI_CLIENT_ID_REG_CNF)
  {
    dshdr_uim.client_id = cnf_ptr->client_id_reg_cnf.response_header.client_id;
    
  }
  else
  {
     DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Client ID not registered ");
  }
  (void)rex_set_sigs( dsi_get_ds_tcb(), DS_UIM_CMD_SIG );
}
#endif /*FEATURE_RUIM_FUNC_INTERFACE*/
#endif /* !FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================
FUNCTION      DSHDR_AN_CM_INIT

DESCRIPTION   This function initializes the HDR AN Authentication Call
              Manager client, and registers for call events of interest,
              namely:
                CM_CALL_EVENT_END
                CM_CALL_EVENT_INCOM
                CM_CALL_EVENT_CONNECT

              It then calls the appropriate functions when the event of
              interest occurs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_an_cm_init( void )
{
  cm_client_status_e_type  status;      /* Status returned by Call Manager */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Register as the Data Services client with Call Manager.
  -------------------------------------------------------------------------*/
  status = cm_client_init( CM_CLIENT_TYPE_HDR_AN, &dshdr_an.cm_client_id );

  if(status != CM_CLIENT_OK )
  {
    ERR_FATAL( "HDR AN unable to register as CM client:%d", (int)status, 0, 0 );
  }
  else
  {
    /*-----------------------------------------------------------------------
      Register for call related events of interest. Multiple registrations
      are needed since only one range of events may be registered at a time.
    -----------------------------------------------------------------------*/
    if (cm_client_ph_reg( dshdr_an.cm_client_id,
                      dshdr_an_cm_ph_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_PH_EVENT_SUBSCRIPTION_AVAILABLE,
                      CM_PH_EVENT_SUBSCRIPTION_CHANGED,
                      NULL
                    ) != CM_CLIENT_OK)
    {
      DATA_HDR_MSG0(MSG_LEGACY_ERROR, "cm_client_ph_reg returned error");
    }

    /*-----------------------------------------------------------------------
      Activate the registered callback functions.
    -----------------------------------------------------------------------*/
    if (cm_client_act( dshdr_an.cm_client_id ) != CM_CLIENT_OK)
    {
      DATA_HDR_MSG0(MSG_LEGACY_ERROR, "cm_client_act returned error");
    }
  }
} /* dshdr_an_cm_init() */


/*===========================================================================
FUNCTION      DSHDR_AN_MGR_POWERUP_INIT

DESCRIPTION   Does the powerup initialization for the HDR AN Authentication
              module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void dshdr_an_mgr_powerup_init( void )
{

  /*-------------------------------------------------------------------------
    Initialize Watermarks.
  -------------------------------------------------------------------------*/
  dshdr_an_wmk_init();

  /*-------------------------------------------------------------------------
    Register as a Call Manager client, and register for call events.
    Commenting out registering with CM since it's no longer needed
    Todo: Cleanup code related to this cm init later
  -------------------------------------------------------------------------*/
  //dshdr_an_cm_init();

#ifndef FEATURE_MMGSDI_SESSION_LIB
#ifdef FEATURE_RUIM_FUNC_INTERFACE
  dshdr_uim.client_id=(mmgsdi_client_data_type)INVALID_RUIM_CLIENTID;
  dshdr_uim.report_status=FALSE;
#endif /*FEATURE_RUIM_FUNC_INTERFACE*/
#endif /*#FEATURE_MMGSDI_SESSION_LIB*/
  /*-----------------------------------------------------------------------
    Initialize the PS_IFACE info for this pkt instance.  Set the user
    pointer to point to the pkt_state_ptr.
  -----------------------------------------------------------------------*/
  (void)ps_iface_create( &dshdr_an.state.ps_iface,
                   CDMA_AN_IFACE,
                   NULL,
                   NULL,
                   dshdr_an.state.ps_phys_link,
                   DSHDR_AN_MAX_PHYS_LINKS );

  ps_iface_enable_ind( &dshdr_an.state.ps_iface );

    (void) ps_iface_dpl_support_network_logging(&dshdr_an.state.ps_iface,
                                                DPL_IID_IFNAME_CDMA_AN_IFACE);

    (void) ps_iface_dpl_set_iface_desc(&dshdr_an.state.ps_iface, NULL);

    (void) ps_iface_dpl_support_link_logging(&dshdr_an.state.ps_iface,
                                             DPL_IID_IFNAME_CDMA_AN_IFACE);
    (void) ps_iface_dpl_support_flow_logging(&dshdr_an.state.ps_iface,
                                             DPL_IID_IFNAME_CDMA_AN_IFACE);



  /*-----------------------------------------------------------------------
    Register for PPP callback events
  -----------------------------------------------------------------------*/
  ppp_event_handle = ppp_event_alloc( PPP_UM_AN_DEV,
                dshdr_an_handle_ppp_ev_cback,
                                      NULL,
                                      dshdr_an_handle_ppp_auth_ev_cback,
                                      NULL,
                                      NULL,
                                      NULL,
                                      NULL,
                NULL);
  if( NULL == ppp_event_handle )
  {
    ERR_FATAL( "Unable to acquire a PPP event handle", 0, 0, 0 );
  }
  else
  {
    if( 0 > ppp_event_register( ppp_event_handle,
                                PPP_PROTOCOL_LCP,
                                PPP_DOWN_EV_MASK ) )
    {
      ERR_FATAL ("ppp_event_register returned err", 0, 0, 0 );
    }

    if( 0 > ppp_event_register(ppp_event_handle,
                               PPP_PROTOCOL_AUTH,
                               (ppp_event_mask_e_type)((uint32)PPP_SUCCESS_EV_MASK
                                                  | (uint32)PPP_FAILURE_EV_MASK )))
    {
      ERR_FATAL ("ppp_event_register returned err", 0, 0, 0 );
    }
  }

  /* Here HDRHAI_STREAM0 means that there is no stream mapped for the
     AN mgr. We can use this because STREAM0 is reserved for signaling
     and can never be used for a Packet Application */
  dshdr_an.active_stream_id = HDRHAI_STREAM0;

  /* Register for callback for subscription ready event from mmgsdi */
  ds3g_mmgsdi_register_for_subscription_ready(dshdr_on_card_ready_event);

#ifdef FEATURE_DATA_PKT_CLASSIFIER
  if (ds707_is_pkt_classifier_enabled()) 
  {
    dshdr_an_setup_rlp();
  }
#endif/* FEATURE_DATA_PKT_CLASSIFIER */
} /* dshdr_an_mgr_powerup_init() */

/*===========================================================================
FUNCTION      DSHDR_AN_GET_IFACE_PTR

DESCRIPTION   Returns the pointer to the AN PS iface.

DEPENDENCIES  None.

RETURN VALUE  pointer to the AN ps iface

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type * dshdr_an_get_iface_ptr( void )
{
  return &dshdr_an.state.ps_iface;
}

/*===========================================================================
FUNCTION      dshdr_an_auth_status_init

DESCRIPTION   Initialize AN authentication status. This function is called
              when a new HDR session is opened in hdrpac_process_ind() or
              during power up state

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void dshdr_an_auth_status_init( void )
{
  dshdr_set_an_auth_status(DSHDR_NOT_AUTH);
}

/*===========================================================================
FUNCTION      dshdr_get_an_auth_status

DESCRIPTION   Get AN authentication status. This function may be called
              by QMI module or other module

DEPENDENCIES  None.

RETURN VALUE  DSHDR_NOT_AUTH:    Not Authenticated
              DSHDR_PASS_AUTH:   Authenticated successfully
              DSHDR_FAILED_AUTH: Failed authentication

SIDE EFFECTS  None.
===========================================================================*/
dshdr_an_auth_status_e_type dshdr_get_an_auth_status( void )
{
  return dshdr_an.an_auth_status;
}

/*===========================================================================
FUNCTION      dshdr_an_set_user_id

DESCRIPTION   Set HDR AN user id

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
int dshdr_an_set_user_id( uint8 * user_id_ptr, uint8 user_id_len )
{

  if( ( user_id_ptr == NULL ) ||
      ( user_id_len > NV_MAX_AN_AUTH_USER_ID_LONG_LENGTH ) )
  {
    DATA_HDR_MSG2(MSG_LEGACY_ERROR, "Failed to set HDR AN user id, "
                  "id 0x%x, len %d",
                  user_id_ptr, user_id_len);
    return -1;
  }

  dshdr_an.auth_info.user_id_len = user_id_len;
  memscpy( dshdr_an.auth_info.user_id_info, PPP_MAX_USER_ID_LEN, 
	   user_id_ptr, user_id_len );
  return 0;
} /* dshdr_an_set_user_id */

/*===========================================================================
FUNCTION      dshdr_on_card_ready_event

DESCRIPTION   Callback is called when card is ready. Used for AN PPP
              initialization

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void dshdr_on_card_ready_event(boolean is_ready)
{
  dshdr_card_ready = is_ready;


  DATA_HDR_MSG0(MSG_LEGACY_MED," Received card ready event");

  if (TRUE == dshdr_card_ready
    && TRUE == dshdr_an_ppp_start_pending)
  {
    DATA_HDR_MSG0(MSG_LEGACY_MED," MMGSDI card is ready start PPP");
    (void) dshdr_an_ppp_start();
    dshdr_an_ppp_start_pending = FALSE;
  }
}

/*===========================================================================
FUNCTION      dshdr_set_an_auth_status

DESCRIPTION   Set AN auth status locally and in the PS framework

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
static void dshdr_set_an_auth_status
(
  dshdr_an_auth_status_e_type auth_status
)
{
  ps_sys_conf_3gpp2_an_auth_status_enum_type auth_status_info;
  int16  ps_error = 0;
  int16  rval = 0; 
/*-------------------------------------------------------------------------*/

  dshdr_an.an_auth_status = auth_status;  
  auth_status_info = (ps_sys_conf_3gpp2_an_auth_status_enum_type)auth_status;
  
  rval = ps_sys_conf_set ( PS_SYS_TECH_3GPP2,
                           PS_SYS_CONF_3GPP2_AN_AUTH_STATUS,
                           (void *)&auth_status_info,
                           &ps_error ) ;

  if (( rval != DSS_SUCCESS ) || ps_error )
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                    "Failed to set an auth status %d with ps_error: %d", 
                    rval,
                    ps_error);
  }
}/*dshdr_set_an_auth_status*/

#endif /* FEATURE_HDR */
