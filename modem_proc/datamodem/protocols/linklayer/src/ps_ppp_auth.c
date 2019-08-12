/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              P S _P P P _ A U T H . C

GENERAL DESCRIPTION
  All of the internal PPP protocol suite functionality.

Copyright (c) 1995-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: MM_DATA/ps_ppp_auth.c_v   1.6   21 Feb 2003 20:42:56 jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_auth.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/09/15    sd     Fallback support for CHAP challenge response calculation.
10/09/14    vl     Stack size optimization changes
04/26/14    cx     Fix null pointer dereference error in 
                   chapi_calc_an_hrpd_md5_ruim_response()
09/12/13    sd     Cryptographic vulnerability - replacing ran_next APIs.
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
05/07/13    cx     Fix compiler errors
10/25/12    fn     Remove usage of fatal error log messages
05/31/12    ash    Free DSM response packet in error case.
05/01/12    sb     Always On UIM Feature Cleanup
03/27/12    ash    Removed FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND
03/08/12    ash    Propagate call end reasons correctly
02/08/12    ash    Added NCPI_CB_IS_INITED Macro to check initilization status 
                   of VSNCP control block.
10/31/11    jee    Fallback to CAVE only if allowed
10/27/11    op     Added helper functions for CHAP Challenge/Response
09/27/11    ash    Terminate PPP on LCP failure
09/27/11    op     Compiler warning fixes
09/14/11    ash    Cancel PPP setup timer after AUTH success
04/29/11    jee    New failure codes - for PAP and CHAP
04/04/11    mga    Changes to ensure if eap_create_instance returns INVALID 
                   HANDLE, PPP gracefully handles the same.
03/23/11    op     Data SU API cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
01/31/11    op     Cleaning up usage of UIM API
08/18/10    jee    Compiler warning fix merged from 9K 2.0
06/03/10    mga    Memset ppp_eap_info to 0 prior to eap_create_instance
03/30/10    ssh    FEATURE_DATA_PS_PAP_INTERNAL_AUTH: Gobi mainlining
02/04/10    jee    Addded task service function support for EAP-USIM
02/02/10    jee    To fix issues seen on 9K target
01/13/10    mga    Added check for ehrpd_usim_present in ppp_auth_eap_init
12/10/09    jee    Changes for the MMGSDI re-arch
09/18/09    jee    Fix for the optimized CAVE fallback 
08/31/09    mga    Merged from eHRPD branch
08/18/09    jee    Fixed lint errors 
07/14/09    pp     ppp_cb_lock changes.
05/07/09    pp     CMI Phase-4: SU Level API Effort.
11/23/08    scb    Added fix for OMH package to read correct nai_entry_index
11/10/08    scb    Fixed QTF compilation warnings.
09/02/08    rnp    Added STA related changes.
02/01/08    scb    Corrected featurization of PPP_UM_AN_DEV to be under
                   FEATURE_HDR_AN_AUTH
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
09/07/07    scb    Added REX signal ext code under FEATURE_REX_SIGS_EXT
08/27/07    scb    Added PPP_STARTING_EV reporting to ppp_auth_start()
08/18/07    scb    Featurized the MMGSDI interaction of RUIM code using the
                   FEATURE_RUIM_FUNC_INTERFACE feature.
07/24/07    scb    Removed the client id registeration from the 
                   auth_powerup_init code and added a check for validity
                   of the client id and registration consequently everytime the 
                   mmgsdi module is called.
07/11/07    scb    Added fix to make both 3GPD and HRPD_AN features 
                   compatible.
05/20/07    scb    Added RUIM support for CHAP authentication in 7500
04/27/07    scb    Removed LINT suppressions
04/19/07    scb    Added a KDDI PPP compliance requirement to configure
                   Application specific AUTH constants
03/04/07    scb    Reverted LINT fixes due to 7200 compilation errors
02/27/07    scb    Fixed high LINT errors
02/08/07    scb    Fixed Klocwork High errors
10/23/06    ssh    Discarding malformed PAP and CHAP packets
04/28/06    ifk    Added PPP critical sections.
11/05/05    rt     Fixed coverity errors.
10/25/05    ssh    Featurize PPP_UM_AN_DEV under FEATURE_HDR_AN_AUTH
08/18/05    kr     Added support for stats update
07/21/05    ssh    Changes pursuant to new ppp_fail_reason_e_type
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
03/15/05  ifk/rr   Fixed handling of chap response with empty name string.
02/10/05    ifk    Took out HDR auth registered callback function.  Using PPP
                   auth events now.
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
12/28/04    ifk    Remove padding bytes from auth message
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
12/03/04    jk     Fixed Compile problem with AN stream check in chapi_send_resp
12/03/04    jk     Added slot information for UIM to support Dual Slots
11/23/04    jk     Added additional support for 3GPD
10/01/04    ifk    Generate auth success/failure and lcp up in auth_complete()
09/10/04    jk     Added support for AN HRPD on an R-UIM card.
09/09/04    ifk    Set PPP phase to pppNETWORK in ppp_auth_complete()
07/31/04    ifk    In ppp_auth_complete() start NCP if active
07/01/04    ifk    Removed setting of dsm kind field
05/18/04    jk     Added support for using the R-UIM card for CHAP.
02/26/04    mvl    Fixed double declaration CR. Added JCDMA Term Req 
                   behaviour rather than Abort()
02/06/04    aku    Print message on receiving PAP ACK
01/28/04    mvl    Now ensuring we only reach auth success/failures if
                   message ID matches.
10/24/03    ifk    Get timeout and retries from auth struct in PPP CB, added
                   ppp_auth_set_device_constants()
10/13/03    mct    After retries fail in ppp_auth_timeout, fixed call so
                   ppp_auth_complete is called with correct protocol.
09/25/03    ifk    In ppp_auth_complete() call ipv6cp_start() if IPv6CP
                   ps_iface is not NULL.
08/13/03    ifk    When auth completes, call ipcp_start only if IPCP is to be
                   negotiated.  Cleanup for adding IP6CP.
07/02/03    mvl    Moved some field length defines from .c to .h
05/06/03    usb    For JCDMA check for UM_SN_DEV instead of UM_AN_DEV when
                   auth fails.
04/09/03    mvl    Fixed a bug in PAP authentication, added support for not
                   moving to the network phase if auth info not received.
04/08/03    mvl    Fixed bug for saving chap information.
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP. 
04/02/03    mvl    Complete modularization of the authentication code.
03/26/03    mvl    mainlined the CHAP and PAP features.
03/17/03    js     Added the prefix PS_PPP_EVENT_ to ppp_event_state_type.
03/07/03    js     Added CHAP/PAP events reporting. 
02/21/03    jd     moved & renamed ppp_auth_get_from_nv() from this file to
                   to is707_get_ppp_auth_info_from_nv() in ps707_dssnet.c
02/06/03    jd     Renamed ppp_auth_get_info() to ppp_auth_get_from_nv()
01/31/03    jd     Added ppp_auth_get_info() to read PPP authentication info
                   into the provided ppp_config 
01/27/03    jd     Merged ppp_send_chap_pkt() optimizations from common tip
11/04/02    mvl    Added code to increment the PAP id field.
09/06/02    mvl    fixed merge issue
08/20/02     vr    Free chap_challenge_ptr if NULL username/password
08/02/02    mvl    moved authentication declarations here.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "amssassert.h"
#include "ps_byte.h"
#include "md5.h"
#include "ps_ppp_auth.h"
#include "ps_pppi_auth.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_events.h"
#include "ps_ppp_events.h"
#include "ps_ppp_ipcp.h"
#include "ps_pppi_ipcp.h"
#include "ps_pppi_fsm.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ppp_ipv6cp.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#include "ps_pppi.h"
#include "pstimer.h"
#include "ran.h"
#include "ps_ppp_lcp.h"
#include "ps_pppi_lcp.h"
#include "ps_stat_common.h"
#include "ps_stat_ppp.h"
#include "uim.h"
#include "psi.h"
#include "ps_utils.h"
#include "nvruimi.h"
#include "auth.h"
#include "auth_v.h"
#include "mccdma.h"
#include "bit.h"
#ifdef FEATURE_RUIM_FUNC_INTERFACE
#include "mmgsdilib.h"
#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdisessionlib.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_RUIM_FUNC_INTERFACE */

#ifdef FEATURE_DATA_PS_PPP_EAP
#include "ps_eap_aka.h"
#include "ps_eap_aka_prime.h"
#include "ps_eap.h"
#include "ps_eap_sim_aka.h"
#include "AKAAlgo.h"
#endif/*FEATURE_DATA_PS_PPP_EAP*/

#ifdef FEATURE_DATA_PS_EHRPD
#include "ps_ppp_ncpi.h"
#endif /* FEATURE_DATA_PS_EHRPD */

#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"


/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_UIM_CAVE_AN_AUTH

/* Bit Shift used for obtaining CHAP Challenge Bits used for AN Cave Auth */
#define PPP_FIRST_32_BITS_USED_FOR_AN_CAVE 4

/* Padding CHAP Response Data with zeros */
#define PPP_CHAP_RESPONSE_PADDING          0

#endif /* FEATURE_UIM_CAVE_AN_AUTH */

/* Status word values for incorrect parameters in data field error sent by UIM
   while calculating CHAP response in card. */
#define PPP_AUTH_CHAP_FAILURE_SW1 0x6A
#define PPP_AUTH_CHAP_FAILURE_SW2 0x80

/* The following structure is used for authentication operations with the R-UIM */

LOCAL struct
{
  /* CHAP challenge response*/
  byte              challenge_response[UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH];
  uim_rpt_status    report_status; /* Status of a UIM Command Status */
#ifdef FEATURE_UIM_CAVE_AN_AUTH
  dword             run_cave_response;
#endif /* FEATURE_UIM_CAVE_AN_AUTH */
  mmgsdi_sw_type    sw_info;       /* Status word response for UIM failure */
} ppp_uim_auth;

#ifdef FEATURE_RUIM_FUNC_INTERFACE

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*---------------------------------------------------------------------------
TYPEDEF PPP_SESSION_INFO_TYPE

DESCRIPTION
  This is structure contains the session related info obtained from 
  MMGSDI
---------------------------------------------------------------------------*/
typedef struct 
{
  mmgsdi_session_type_enum_type  session_type; 
  mmgsdi_session_id_type         session_id;     
  mmgsdi_app_info_type           app_info_type;  
  boolean                        session_active; 
} ppp_session_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PPP_REG_INFO_TYPE

DESCRIPTION
  This is structure contains the client id & session info obtained from 
  MMGSDI
---------------------------------------------------------------------------*/
typedef struct 
{
  mmgsdi_client_id_type  client_id;
  ppp_session_info_type  session_info;
} ppp_reg_info_type;

#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*---------------------------------------------------------------------------
TYPEDEF PPP_MMGSDI_AUTH_CONFIG_TYPE

DESCRIPTION
  This is a structure which contains the client id - obtained from 
  registering with the mmgsdi module, the card_slot used for RUIM
  authentication, the random_type, length of the number of digits,
  the digits used for CAVE authentication, the process control type,
  and the CAVE ESN value obtained from the cave_esn_type NV Item.
  ---------------------------------------------------------------------------*/
typedef struct
{
#ifdef FEATURE_MMGSDI_SESSION_LIB
  ppp_reg_info_type                  reg_info;
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#ifdef FEATURE_UIM_CAVE_AN_AUTH
  mmgsdi_cave_rand_type_type         rand_type;
  mmgsdi_cave_dig_len_type           dig_len;
  mmgsdi_cave_digits_type            digits;
  mmgsdi_cave_process_control_type   process_control;
  mmgsdi_cave_esn_type               esn;
#endif /* FEATURE_UIM_CAVE_AN_AUTH */
} ppp_mmgsdi_auth_config_type;

static ppp_mmgsdi_auth_config_type ppp_mmgsdi_auth_config;
#endif /* FEATURE_RUIM_FUNC_INTERFACE */

#ifdef FEATURE_DATA_PS_PPP_EAP
/*---------------------------------------------------------------------------
  EAP meta info
---------------------------------------------------------------------------*/
static eap_sim_aka_meta_info_type ppp_eap_meta_info;

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*---------------------------------------------------------------------------
TYPEDEF PPP_TASK_INFO_TYPE

DESCRIPTION
  This is structure contains the task info passed on by EAP
---------------------------------------------------------------------------*/
typedef struct
{
  eap_sim_aka_task_srv_req_cb_type req_cb;
  void * user_data;
} ppp_task_info_type;

static ppp_task_info_type ppp_task_info;

#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /*FEATURE_DATA_PS_PPP_EAP*/

/*---------------------------------------------------------------------------
  Critical section used to provide mutual exclusion to ppp_reg_info_type
---------------------------------------------------------------------------*/
#ifdef FEATURE_RUIM_FUNC_INTERFACE
#ifdef FEATURE_MMGSDI_SESSION_LIB
static ps_crit_sect_type  ppp_reg_crit_section;
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_RUIM_FUNC_INTERFACE */

/*---------------------------------------------------------------------------
  Replicated definition from uimi.h to remove uim dependency  
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  byte rand_type[8];     /* rand type */
  byte rand[32];         /* rand */
  byte randu[24];        /* rand for unique challenge */
  byte dig_len[8];       /* number of digits */
  byte digits[8];        /* arrray of digits */
  byte proc_control[8];  /* Process control bits */
  byte esn[32];          /* ESN */
  byte esn_pad1[16];     /* padding for ESN */
  byte esn_pad2[8];      /* padding for ESN */
} ppp_auth_run_cave_req_siz_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
static void authi_send_success
(
  ppp_type *ppp_cb_ptr,
  uint8 code
);

static void papi_send_auth_req
(
  ppp_type *ppp_cb_ptr
);

static boolean papi_save_auth_info
(
  ppp_type       *ppp_cb_ptr,
  dsm_item_type **pap_req_ptr
);

static boolean chapi_save_auth_info
(
  ppp_type       *ppp_cb_ptr,
  dsm_item_type **chap_resp_ptr,
  uint16          length
);

static void chapi_send_resp
(
  ppp_type *ppp_cb_ptr
);

static void chapi_send_challenge
(
  ppp_type *ppp_cb_ptr
);

#ifdef FEATURE_DATA_PS_PPP_EAP
static void ppp_auth_eap_init
(
    ppp_dev_enum_type device
);

static void ppp_eap_result_cb
( 
  eap_handle_type       handle, 
  void                 *ps_result_ind_user_data,
  eap_result_enum_type  result,
  uint8                *master_session_key,
  uint16                master_session_key_len
);

static void ppp_eap_tx_cb
(
  void *userdata,
  dsm_item_type **pdu
);

static void ppp_eap_notification_cb
(
  void * user_data
);

#ifdef FEATURE_MMGSDI_SESSION_LIB
static boolean ppp_auth_eap_task_srv_fct
(
  eap_sim_aka_task_srv_req_cb_type req_cb,
  void * user_data
);

void ppp_auth_process_eap_sim_aka_task_switch
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
);
#endif /* FEATURE_MMGSDI_SESSION_LIB */

#endif/*FEATURE_DATA_PS_PPP_EAP*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                        PUBLIC FUNCTION DECLARATIONS

  -------------------------------------------------------------------------

                              GENERAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#ifdef FEATURE_RUIM_FUNC_INTERFACE
#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION PPP_MMGSDI_CLIENT_ID_REG_RESP_RPT()

DESCRIPTION
  Retrieves the Client ID Response from the mmgsdi response, and 
  copies into variables local to the file.  This is a callback function 
  which is called from the mmgsdi/uim context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the ppp_uim_auth.report_status.
===========================================================================*/
static void ppp_mmgsdi_client_id_reg_resp_rpt
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)

{
  mmgsdi_client_id_and_evt_reg_cnf_type *client_reg_ptr   = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_uim_auth.report_status = UIM_FAIL;
  if( NULL == cnf_ptr )
  {
    LOG_MSG_ERROR_0("mmgsdi_client_id_reg_resp_rpt: MMGSDI CLIENT ID REG "
                    "failure as cnf_ptr is NULL");

    PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);
    return;
  }

  client_reg_ptr = (mmgsdi_client_id_and_evt_reg_cnf_type *) 
                             &(cnf_ptr->client_id_and_evt_reg_cnf);
  /*-------------------------------------------------------------------------
    Copy the uim report status into a variable local to the file
  -------------------------------------------------------------------------*/
  if ( ( MMGSDI_CLIENT_ID_AND_EVT_REG_CNF == cnf ) 
       && ( MMGSDI_CLIENT_ID_AND_EVT_REG_CNF 
       == client_reg_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) 
         && ( MMGSDI_SUCCESS 
         == client_reg_ptr->response_header.mmgsdi_status ) )
    {
      /*--------------------------------------------------------------------
        The mmgsdi client id reg command was successful.  
        This copies the data from the uim report
      ---------------------------------------------------------------------*/
      PS_ENTER_CRIT_SECTION (&ppp_reg_crit_section);
      ppp_mmgsdi_auth_config.reg_info.client_id = 
        client_reg_ptr->response_header.client_id;
      ppp_uim_auth.report_status = UIM_PASS;
      PS_LEAVE_CRIT_SECTION (&ppp_reg_crit_section);

      ps_send_cmd (PS_PPP_CLNT_REG_CMD, NULL);
      return;
      }

    else
    {
      LOG_MSG_ERROR_2("mmgsdi_client_id_reg_resp_rpt: UIM-CLIENT ID REG command"
                      " failure status %d and mmgsdi_status %d are ",
                      status, client_reg_ptr->response_header.mmgsdi_status);
    }
  }
  else
  {
    LOG_MSG_ERROR_2("mmgsdi_client_id_reg_resp_rpt: Improper command confirmation " 
                    " cnf %d response_type %d",
                    cnf, client_reg_ptr->response_header.response_type);
  }

  /*-------------------------------------------------------------------------
    Set the signal to indicate to the PS task that the
    uim command has completed
  -------------------------------------------------------------------------*/
  PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);
} /* mmgsdi_client_id_reg_resp_rpt() */


/*===========================================================================
FUNCTION PPP_MMGSDI_CLIENT_ID_REG_EVT_RPT

DESCRIPTION
  Function registered with the MMGSDI events, needed in order to know 
  the state of SIM/USIM.

  We are interested in events like : CARD_INSERTED, CARD_REMOVE and 
                                     CARD_ERROR

DEPENDENCIES
  None.
  
PARAMETERS
  event - the event information 
  
RETURN VALUE
  None 
  
SIDE EFFECTS
    
===========================================================================*/
void ppp_mmgsdi_client_id_reg_evt_rpt
(
  const mmgsdi_event_data_type *event
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Sanity Check
  -------------------------------------------------------------------------*/
  if ( NULL == event ) 
  {
    LOG_MSG_ERROR_0("PPP Client Reg Error: Invalid input Parameter");
    return;
  }

  LOG_MSG_INFO1_1("PPP Client Reg: event CB - %d", event->evt);
  /*-----------------------------------------------------------------------
    For now only interested in CARD_INSERTED, CARD_ERROR, CARD_REMOVED
  -------------------------------------------------------------------------*/
  switch ( event->evt ) 
  {
    case MMGSDI_CARD_INSERTED_EVT:
    case MMGSDI_CARD_ERROR_EVT:
    case MMGSDI_CARD_REMOVED_EVT:
    default:
      LOG_MSG_INFO2_1("Received Event in UME : %d", event->evt);
      break;
  }

  return;
}/*ppp_mmgsdi_client_id_reg_evt_rpt()*/


/*===========================================================================
FUNCTION PPP_MMGSDI_SESSION_EVT_CB()

DESCRIPTION
  Callback for MMGSDI session related events

PARAMETERS
  event - the event information 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ppp_mmgsdi_session_evt_cb 
(
  const mmgsdi_event_data_type *event
)
{
  if (NULL == event)
  {
    LOG_MSG_ERROR_0("ppp_mmgsdi_session_evt_cb: MMGSDI event callback "
                    "failure as event pointer is NULL");
    return;
  }
  
  switch (event->evt)
  {
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      LOG_MSG_INFO2_1("ppp_mmgsdi_session_evt_cb: Processing " 
              "MMGSDI_SUBSCRIPTION_READY_EVT for client_id %d", 
                      event->client_id);
      PS_ENTER_CRIT_SECTION (&ppp_reg_crit_section);
      memscpy (&(ppp_mmgsdi_auth_config.reg_info.session_info.app_info_type), 
               sizeof (mmgsdi_app_info_type),
               &(event->data.subscription_ready.app_info),
               sizeof (mmgsdi_app_info_type));
      ppp_mmgsdi_auth_config.reg_info.session_info.session_active = TRUE;
      PS_LEAVE_CRIT_SECTION (&ppp_reg_crit_section);
      break;

    default:
      LOG_MSG_INFO2_2("ppp_mmgsdi_session_evt_cb: Processing " 
              "event %d for client_id %d", 
                      event->evt, event->client_id);
  }

}


/*===========================================================================
FUNCTION PPP_MMGSDI_SESSION_OPEN_CB()

DESCRIPTION
  MMGSDI Session open callback

PARAMETERS
  status, cnf - success or failure, confirmation type
  cnf_ptr - response, union member session_open_cnf, will contain valid data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ppp_mmgsdi_session_open_cb 
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  mmgsdi_session_open_cnf_type *ssn_open_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == cnf_ptr )
  {
    LOG_MSG_ERROR_0("ppp_mmgsdi_session_open_cb: MMGSDI session open "
                    "failure as cnf_ptr NULL");
    return;
  }

  ssn_open_ptr = 
    (mmgsdi_session_open_cnf_type *) &(cnf_ptr->session_open_cnf);

  /*-------------------------------------------------------------------------
    Copy the session info into a variable local to the file
  -------------------------------------------------------------------------*/
  if ( ( MMGSDI_SESSION_OPEN_CNF == cnf ) 
         && ( MMGSDI_SESSION_OPEN_CNF 
         == ssn_open_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) 
         && ( MMGSDI_SUCCESS 
         == ssn_open_ptr->response_header.mmgsdi_status ) )
    {
      PS_ENTER_CRIT_SECTION (&ppp_reg_crit_section);
      ppp_mmgsdi_auth_config.reg_info.session_info.session_id 
        = ssn_open_ptr->session_id;
      PS_LEAVE_CRIT_SECTION (&ppp_reg_crit_section);
    }

    else
    {
        LOG_MSG_ERROR_2("ppp_mmgsdi_session_open_cb: Session open cb "
                        "failure status %d and mmgsdi_status %d ",
                        status, ssn_open_ptr->response_header.response_type);
    }
  }

  else
  {
      LOG_MSG_ERROR_2("ppp_mmgsdi_session_open_cb: Improper command confirmation "
                      "cnf %d response_type %d",
                      cnf, ssn_open_ptr->response_header.response_type);
  }
  
}


/*===========================================================================
FUNCTION PPP_CLIENT_REG_CMD_HDLR()

DESCRIPTION
  This function handles the PS_PPP_REG_CLNT_CMD command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_client_reg_cmd_hdlr
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
  mmgsdi_evt_callback_type              *mmgsdi_ssn_evt_cb = NULL;
  mmgsdi_callback_type                   mmgsdi_ssn_open_cb = NULL;
  mmgsdi_return_enum_type                status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd != PS_PPP_CLNT_REG_CMD)
  {
    LOG_MSG_ERROR_1("ppp_client_reg_cmd_hdlr: Invalid cmd %d", cmd);
    ASSERT(0);
    return;
  }

  mmgsdi_ssn_evt_cb  = ppp_mmgsdi_session_evt_cb;
  mmgsdi_ssn_open_cb = ppp_mmgsdi_session_open_cb;

  LOG_MSG_INFO2_0("PS processing PS_PPP_CLNT_REG_CMD cmd");
  /*-----------------------------------------------------------------------
    open 1X provisioning mmgsdi session 
  -----------------------------------------------------------------------*/
  status = mmgsdi_session_open (ppp_mmgsdi_auth_config.reg_info.client_id,
                                MMGSDI_1X_PROV_PRI_SESSION, 
                                NULL, 
                                mmgsdi_ssn_evt_cb,
                                FALSE, 
                                mmgsdi_ssn_open_cb, 
                                (mmgsdi_client_data_type) 0);

  if ( status != MMGSDI_SUCCESS )
  {
    LOG_MSG_ERROR_1("ppp_client_reg_cmd_hdlr: Session Open 1x "
                    "Registration Failed %d ", status);
  }

  return;
} /* ppp_client_reg_cmd_hdlr() */

#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_RUIM_FUNC_INTERFACE */

#ifdef FEATURE_RUIM_FUNC_INTERFACE
#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION PPP_AUTH_REGISTER_CLIENT_ID()

DESCRIPTION
  This function is used to regsiter a client id for the authentication 
  mechanism that uses the mmgsdi module. As a part of the authentication 
  procedure the MMGDSI library is called to register a client id.


RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  To be used only when the FEATURE_MMGSDI_SESSION_LIB 
  and FEATURE_RUIM_FUNC_INTERFACE features are enabled.

SIDE EFFECTS
  None
===========================================================================*/
static int ppp_auth_register_client_id
(
  void
)
{
  mmgsdi_return_enum_type            retval;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  retval = mmgsdi_client_id_and_evt_reg (ppp_mmgsdi_client_id_reg_evt_rpt,
                                         ppp_mmgsdi_client_id_reg_resp_rpt,
                                         (mmgsdi_client_data_type) 0);

  if (MMGSDI_SUCCESS != retval) 
  {
    LOG_MSG_ERROR_1("mmgsdi_client_id_and_evt_reg() returned error = %d",
                    retval);
    return -1;
  }

  return 0;
}
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_RUIM_FUNC_INTERFACE */

#ifdef FEATURE_RUIM_FUNC_INTERFACE
/*===========================================================================
FUNCTION PPP_AUTH_INIT()

DESCRIPTION
  This function is called at system initialization time.  It initializes the
  the authentication configuration parameters for 3GPD, HRPD_AN or CAVE
  authentication on a one time basis.

RETURN VALUE
  int: -1 on failure
        0 on success

DEPENDENCIES
  To be used only when the FEATURE_UIM_CAVE_AN_AUTH feature is enabled.

SIDE EFFECTS
  None
===========================================================================*/
int ppp_auth_init
(
  void
)
{
#ifdef FEATURE_UIM_CAVE_AN_AUTH
  nv_item_type                       ps_cave_chap_esn;
  uint8                              index = 0;
#endif /* FEATURE_UIM_CAVE_AN_AUTH */

#ifdef FEATURE_UIM_CAVE_AN_AUTH
  ppp_mmgsdi_auth_config.rand_type = 
                     (mmgsdi_cave_rand_type_type) UIM_GLOBAL_RAND_CHAL;

  ppp_mmgsdi_auth_config.dig_len = (mmgsdi_cave_dig_len_type) 0;

  for (index = 0; index < MMGSDI_CAVE_DIGITS_SIZE ; index++)
  {
     ppp_mmgsdi_auth_config.digits[index] = 0;
  }
 
  ppp_mmgsdi_auth_config.process_control = 
               (mmgsdi_cave_process_control_type)RUIM_DEFLT_PROC_CNTRL;

  LOG_MSG_INFO2_1("PPP process control type = %d",
                  ppp_mmgsdi_auth_config.process_control);

  if ( NV_DONE_S == ps_get_nv_item( NV_ESN_I, &ps_cave_chap_esn) )
  {
    ppp_mmgsdi_auth_config.esn[0] = ps_cave_chap_esn.esn.esn;
    ppp_mmgsdi_auth_config.esn[1] = 0;
  }
  else
  {
    LOG_MSG_ERROR_0("ppp_auth_init: Could not read NV item to initialize"
                    " ESN");
  }
#endif /* FEATURE_UIM_CAVE_AN_AUTH */

#ifdef FEATURE_MMGSDI_SESSION_LIB
  PS_INIT_CRIT_SECTION (&ppp_reg_crit_section);

  /*-------------------------------------------------------------------------
    register the handlers for the various PPP commands that must be handled
    in the task context. Then register the PPP client with MMGSDI
  -------------------------------------------------------------------------*/
  (void)ps_set_cmd_handler(PS_PPP_CLNT_REG_CMD, ppp_client_reg_cmd_hdlr);
#ifdef FEATURE_DATA_PS_PPP_EAP
  (void)ps_set_cmd_handler(PS_PPP_EAP_TASK_CMD, 
                           ppp_auth_process_eap_sim_aka_task_switch);
#endif /* FEATURE_DATA_PS_PPP_EAP */

  if (0 != ppp_auth_register_client_id())
  {
    LOG_MSG_ERROR_0("Could not obtain valid client id");
  } 
#endif /* FEATURE_MMGSDI_SESSION_LIB */

  return 0;
} /* ppp_auth_init() */
#endif /* FEATURE_RUIM_FUNC_INTERFACE */


/*===========================================================================
FUNCTION PPP_AUTH_SET_DEVICE_CONSTANTS()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  AUTH on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for AUTH.
===========================================================================*/
void ppp_auth_set_device_constants
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  nv_ppp_configure_data_type *config
)
{
  ppp_type *ppp_cb_ptr;
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( PPP_MAX_DEV <= device || NULL == config )
  {
    LOG_MSG_ERROR_2( "Invalid arguments passed: device %d, config 0x%x",
                     device, config );
    ASSERT( 0 );
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;
  /*-------------------------------------------------------------------------
    Initialize the protocol dependent constants that are configurable
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  if( 0 < config->auth_timeout )
  {
    ppp_cb_ptr->auth.timeout = config->auth_timeout;
  }

  ppp_cb_ptr->auth.retry_limit = config->auth_retry;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );


  return;
} /* ppp_auth_set_device_constants() */


/*===========================================================================
FUNCTION PPP_AUTH_INIT_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  AUTH on the interface specified.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Set configurable protocol constants for AUTH.
===========================================================================*/
void ppp_auth_init_device_constant
(
  ppp_dev_enum_type           device,       /* the interface to initialize */
  ppp_init_consts_enum_type constant,
  uint32 value
)
{
  ppp_type *ppp_cb_ptr =  NULL;
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( device >= PPP_MAX_DEV || constant >= PPP_MAX_INIT_CONSTS )
  {
    LOG_MSG_ERROR_2( "Invalid args: device %d, constant %d",
                     device, constant );
    ASSERT(0);
    return ;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = (ppp_type *) (ppp_cb_array + (uint8)device);

  /*-------------------------------------------------------------------------
    Initialize the protocol dependent constants that are configurable
  -------------------------------------------------------------------------*/
  switch(constant)
  {
    case AUTH_TIMEOUT:
      PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      if( 0 < value )
      {
        ppp_cb_ptr->auth.timeout = (uint16)value;
      }
      PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      break;

    case AUTH_RETRY: 
      PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      if( 0 < value )
      {
        ppp_cb_ptr->auth.retry_limit =(uint8)value;
      }
      PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
      break;

    default:
      LOG_MSG_ERROR_1("ppp_auth_init_device_constant: Unknown constant %d",
                      constant);
      break;
  }
  
  return;
} /* ppp_auth_init_device_constant() */


/*===========================================================================
FUNCTION PPP_AUTH_START()

DESCRIPTION
  This function checks for authentication and kicks it off if it was
  negotiated, otherwise it starts IPCP.

PARAMETERS
  device: the device on which auth is being started.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_auth_start
(
  ppp_dev_enum_type device
)
{
  ppp_event_payload_type    ppp_event_payload;
  ppp_type *ppp_cb_ptr;
  ppp_type *ppp_cb_array = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;
  /*-------------------------------------------------------------------------
    Now in auth phase: Initialize the retry counter and the active ID.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  ppp_cb_ptr->phase = pppAUTHENTICATE;
  ppp_cb_ptr->auth.retry_counter = ppp_cb_ptr->auth.retry_limit;
  ppp_cb_ptr->auth.active_id = 0;
  PS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

  /*-------------------------------------------------------------------------
    Are we supposed to authenticate?
  -------------------------------------------------------------------------*/
  switch(ppp_cb_ptr->auth.mode)
  {
  case PPP_DO_CHAP:
    ppp_event_payload.ppp_event_protocol = PPP_EV_CHAP;

    /*-----------------------------------------------------------------------
      Are we the authenticatee or the authenticator?  If we are the
      authenticator send the challenge - otherwise wait for it.
    -----------------------------------------------------------------------*/
    if(ppp_cb_ptr->auth.flags & PPP_AP_REMOTE)
    {
      ppp_cb_ptr->auth.retry_proc = chapi_send_challenge;
      chapi_send_challenge(ppp_cb_ptr);
      pppi_call_event_callback( &ppp_cb_ptr->fsm[LCP],
                                PPP_PROTOCOL_AUTH,
                                PPP_STARTING_EV );
    }
    else
    {
      ppp_cb_ptr->auth.retry_proc = chapi_send_resp;
    }
    break;  

  case PPP_DO_PAP:
    ppp_event_payload.ppp_event_protocol = PPP_EV_PAP;

    /*-----------------------------------------------------------------------
      Are we the authenticatee or the authenticator?  If we are the
      authenticator we don't send anything, but wait for a pkt from our peer.
    -----------------------------------------------------------------------*/
    if(ppp_cb_ptr->auth.flags & PPP_AP_LOCAL)
    {
      ppp_cb_ptr->auth.retry_proc = papi_send_auth_req;
      papi_send_auth_req(ppp_cb_ptr);
    }
    else
    {
      ppp_cb_ptr->auth.retry_proc = NULL;
    }
    pppi_call_event_callback( &ppp_cb_ptr->fsm[LCP],
                              PPP_PROTOCOL_AUTH,
                              PPP_STARTING_EV );
    break;


#ifdef FEATURE_DATA_PS_PPP_EAP
  case PPP_DO_EAP:
    ppp_event_payload.ppp_event_protocol = PPP_EV_EAP;  
    /*------------------------------------------------------------
      For EAP, if we are not the peer then wait for the 
      authenticator to send the challenge.
    ------------------------------------------------------------*/   
    if(ppp_cb_ptr->auth.flags & PPP_AP_REMOTE)
    {
      DATA_ERR_FATAL("EAP in Authenticator mode is not supported");
      return;        
    }
    else
    {
      ppp_auth_eap_init(device);
      ppp_cb_ptr->auth.retry_proc= NULL;        
    }
    pppi_call_event_callback( &ppp_cb_ptr->fsm[LCP],
                              PPP_PROTOCOL_AUTH,
                              PPP_STARTING_EV );
    break;

#endif /*FEATURE_DATA_PS_PPP_EAP*/


  default:
    /*-----------------------------------------------------------------------
      NOTE: this is intentionally not last as we want to start IPCP when
      assert()s are disabled.
    -----------------------------------------------------------------------*/
    ASSERT(0);
    /* fall through */

  case PPP_NO_AUTH:
    ppp_event_payload.ppp_event_protocol = PPP_EV_INVALID_PROTO;
    /*-----------------------------------------------------------------------
      there was no authentication required and so is complete - clear flags
      so it doesn't look like it failed.
    -----------------------------------------------------------------------*/
    ppp_cb_ptr->auth.flags = 0;
    ppp_auth_complete(ppp_cb_ptr, PPP_NO_PROTOCOL);
    break;

  } /* switch(auth mode) */

  if(ppp_event_payload.ppp_event_protocol != PPP_EV_INVALID_PROTO)
  {
    ppp_event_payload.ppp_event_device = (int)ppp_cb_ptr->fsm[LCP].device;
    ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
    ppp_event_payload.ppp_event_state = PS_PPP_EVENT_STARTING;
    event_report_payload(EVENT_PPP,
                         sizeof(ppp_event_payload),
                         &ppp_event_payload);
  }

} /* ppp_auth_start() */



/*===========================================================================
FUNCTION PPP_AUTH_COMPLETE()

DESCRIPTION
  This function completes the PPP authentication phase which includes
  cleaning up of any data that was stored.  If authentication was
  sucessful it starts the PPP network phase.  Note that if the protocol
  passed in is PPP_NO_PROTOCOL this indicates that no authentication is to be
  attemted and that is why authentication is complete.

PARAMETERS
  ppp_cb_ptr: ppp control block pointer for completed authentication
  protocol:   protocol which completed (for protocol specific cleanup).  

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_auth_complete
(
  ppp_type *ppp_cb_ptr,
  uint16    protocol
)
{
  ppp_event_payload_type ppp_event_payload;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Authentication complete - cancel the auth timer.
  -------------------------------------------------------------------------*/
  (void)ps_timer_cancel(ppp_cb_ptr->fsm[LCP].timer);

  /*-------------------------------------------------------------------------
    do protocol specific handling - free data and specify protocol for event
  -------------------------------------------------------------------------*/
  switch(protocol)
  {
  case PPP_CHAP_PROTOCOL:
    /*-----------------------------------------------------------------------
      Free the packet in chap_challenge_ptr.
    -----------------------------------------------------------------------*/
    dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    ppp_event_payload.ppp_event_protocol = PPP_EV_CHAP;
    break;
  
  case PPP_PAP_PROTOCOL:
    ppp_event_payload.ppp_event_protocol = PPP_EV_PAP;
    break;
  
#ifdef FEATURE_DATA_PS_PPP_EAP
  case PPP_EAP_PROTOCOL:
    ppp_event_payload.ppp_event_protocol = PPP_EV_EAP;
    break;
#endif /*FEATURE_DATA_PS_PPP_EAP*/

  case PPP_NO_PROTOCOL:
    ppp_event_payload.ppp_event_protocol = PPP_EV_INVALID_PROTO;
    break;

  default:
    ASSERT(0);
    LOG_MSG_ERROR_1("Invalid protocol auth %d!", protocol);
    return;
  } /* switch(protocol) */

  /*-------------------------------------------------------------------------
    Fill in the fileds of ppp_event_payload and post the CHAP open event. 
  -------------------------------------------------------------------------*/
  if(ppp_event_payload.ppp_event_protocol != PPP_EV_INVALID_PROTO)
  {
    ppp_event_payload.ppp_event_device = (int)ppp_cb_ptr->fsm[LCP].device;
    ppp_event_payload.ppp_event_prev_state = PS_PPP_EVENT_UNKNOWN;
    if(ppp_cb_ptr->auth.flags & PPP_AP_FAILED)
    {
      ppp_event_payload.ppp_event_state = PS_PPP_EVENT_CLOSED;
    }
    else
    {
      ppp_event_payload.ppp_event_state = PS_PPP_EVENT_OPEN;
    }
    event_report_payload(EVENT_PPP, 
                         sizeof(ppp_event_payload),
                         &ppp_event_payload);
  }

  /*-------------------------------------------------------------------------
    If auth was successful change phase to pppNETWORK and start any
    active NCPs
  -------------------------------------------------------------------------*/
  if(!(ppp_cb_ptr->auth.flags & PPP_AP_FAILED))
  {
    lcp_up( ppp_cb_ptr->fsm[LCP].device );
    ppp_cb_ptr->phase = pppNETWORK;

    /*------------------------------------------------------------------------- 
      If final phase for setup timer is set to authenticate then cancel
      the timer.
    -------------------------------------------------------------------------*/
    if (pppAUTHENTICATE == ppp_cb_ptr->setup_final_phase)
    {
      (void)ps_timer_cancel(ppp_cb_ptr->setup_timer);
    }

    pppi_call_event_callback( &ppp_cb_ptr->fsm[LCP],
                              PPP_PROTOCOL_AUTH,
                              PPP_SUCCESS_EV );
    pppi_call_event_callback( &ppp_cb_ptr->fsm[LCP],
                              PPP_PROTOCOL_LCP,
                              PPP_UP_EV );

#ifdef FEATURE_DATA_PS_EHRPD
  /*-------------------------------------------------------------------------
    For eHRPD, ncpi_network_start() handles ipxcp_start() also.    
  -------------------------------------------------------------------------*/
    ncpi_network_start( ppp_cb_ptr->fsm[LCP].device ); 
#else
    if( PPP_CP_MODE_ACTIVE == ppp_cb_ptr->fsm[IPCP].mode )
    {
      ipcp_start(ppp_cb_ptr);
    }
#ifdef FEATURE_DATA_PS_IPV6
    if( PPP_CP_MODE_ACTIVE == ppp_cb_ptr->fsm[IPV6CP].mode )
    {
      ipv6cp_start(ppp_cb_ptr);
    }
#endif /* FEATURE_DATA_PS_IPV6 */
#endif /* FEATURE_DATA_PS_EHRPD */
  }
  else
  {
    /*-------------------------------------------------------------------------
      Handle CHAP/PAP failures with seperate fail reason codes
    -------------------------------------------------------------------------*/
    switch(protocol)
    {
      case PPP_CHAP_PROTOCOL:
        ppp_cb_ptr->fsm[LCP].fail_reason = PPP_FAIL_REASON_AUTH_CHAP_FAILURE;
        break;
      
      case PPP_PAP_PROTOCOL:
        ppp_cb_ptr->fsm[LCP].fail_reason = PPP_FAIL_REASON_AUTH_PAP_FAILURE;
        break;

      default:
        ppp_cb_ptr->fsm[LCP].fail_reason = PPP_FAIL_REASON_AUTH_FAILURE;
        break;
    }

    pppi_call_event_callback( &ppp_cb_ptr->fsm[LCP],
                              PPP_PROTOCOL_AUTH,
                              PPP_FAILURE_EV );
  }
#ifdef FEATURE_DATA_PS_PPP_EAP
  if( ppp_cb_ptr->auth.ps_eap_handle != EAP_INVALID_HANDLE 
      && protocol == PPP_EAP_PROTOCOL)
  {
  /*-------------------------------------------------------------------------
    Delete the existing EAP handle associated with the control block
    to indicate end of session and reinitialize the same to ensure
    that further calls to EAP trigger a new session. 
  -------------------------------------------------------------------------*/
    LOG_MSG_INFO2_0("ppp_auth_complete: Deleting an existing eap_handle");
    eap_delete_instance(ppp_cb_ptr->auth.ps_eap_handle);
    ppp_cb_ptr->auth.ps_eap_handle = EAP_INVALID_HANDLE;

    LOG_MSG_INFO2_1("ppp_auth_complete:Reinitialising a new eap_handle for "
                    " device %d", ppp_cb_ptr->fsm[LCP].device);
    ppp_auth_eap_init(ppp_cb_ptr->fsm[LCP].device);
  }
#endif /*FEATURE_DATA_PS_PPP_EAP*/

} /* ppp_auth_complete() */



/*===========================================================================
FUNCTION PPP_AUTH_TIMEOUT()

DESCRIPTION
  This function is the equivalent of fsm_timeout, but since the auth code
  doesn't use the FSM it has its own function.  It is called when an auth
  timeout occurs.  If there are still retries left, then it retries send the
  appropriate auth packet.  Otherwise if there are no requests left, then it
  closes the PPP link.

PARAMETERS
  ppp_cb_ptr: ppp control block pointer for timeout.

RETURN VALUE
  TRUE:  Informs the calling function to retry
  FALSE: Informs calling function, retries are over and to
         terminate the call.

DEPENDENCIES
  Same as dependencies of the functions ppp_chap_timeout

SIDE EFFECTS
  If number of the retries are non-zero then an authentication packet is
  transmitted.
===========================================================================*/
boolean ppp_auth_timeout
(
  ppp_type *ppp_cb_ptr
)
{
  ppp_dev_enum_type device = ppp_cb_ptr->fsm[LCP].device;
  uint16 protocol = PPP_NO_PROTOCOL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    print a message indicating which protocol timed out - this should
    completely dissapear if LOG_MSG_INFO2 is undefined.
  -------------------------------------------------------------------------*/
   switch(ppp_cb_ptr->auth.mode)
   {
   case PPP_DO_PAP:
     LOG_MSG_INFO2_1("dev %d PAP timeout occured", device);
     protocol = PPP_PAP_PROTOCOL;
     break;

   case PPP_DO_CHAP:
     LOG_MSG_INFO2_1("dev %d CHAP timeout occured", device);
     protocol = PPP_CHAP_PROTOCOL;
     break;

   default:
     LOG_MSG_INFO2_1("dev %d Auth timeout with no auth!", device);
     return FALSE;

   } /* switch(auth type) */

  /*-------------------------------------------------------------------------
    make sure that there are still retries available - if not call the
    function for the retry (registered in auth_start()).
  -------------------------------------------------------------------------*/
  if((ppp_cb_ptr->auth.retry_counter--) != 0)
  {
    ppp_cb_ptr->auth.retry_proc(ppp_cb_ptr);
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    If no more retries, then terminate the PPP connection by closing the link
    and cancelling the timer.
  -------------------------------------------------------------------------*/
  else
  {
    LOG_MSG_INFO1_1( "dev %d auth exhausted reties. Closing PPP", device);
    ppp_cb_ptr->auth.flags |= PPP_AP_FAILED;
    ppp_auth_complete(ppp_cb_ptr, protocol);

    if( ppp_cb_ptr->ppp_failure_graceful_close == TRUE )
    {
    /*-----------------------------------------------------------------------
      JCDMA requires that an LCP Term req is sent in this case.
    -----------------------------------------------------------------------*/
      (void)ppp_stop(device, PPP_PROTOCOL_LCP);
    }
    else
    {
    /*-----------------------------------------------------------------------
      Abort - other side is not responding so no point ins sending a Term Req
    -----------------------------------------------------------------------*/
      (void)ppp_abort(device);
    }
    return FALSE;
  }
} /* ppp_auth_timeout() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                        PROTOCOL PROCESSING FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION CHAP_PROC()

DESCRIPTION
  This function does the protocol processing for CHAP.

PARAMETERS
  device:       the PPP interface the packet arrived on
  item_ref_ptr: reference to pointer to dsm chain containing the CHAP pkt

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void chap_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
)
{
  ppp_config_hdr_type auth_hdr;
  ppp_type           *ppp_cb_ptr;
  ppp_type           *ppp_cb_array = NULL;
  int16               challenge_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("cannot process CHAP for device %d!", device);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  /*-------------------------------------------------------------------------
    The rxed message format looks like:
      Code: 1 Octet (the message type)
      Identifier: 1 Octet
      Length: 2 Octets
  
    *item_ref_ptr will be pointing to the code field so starting here extract
    the first three fields.
  -------------------------------------------------------------------------*/
  if( -1 == ntohcnf(&auth_hdr, item_ref_ptr) )
  {
    LOG_MSG_ERROR_1("ppp%d Invalid CHAP pkt", device );
    dsm_free_packet(item_ref_ptr);
    ASSERT(0);
    return;
  }

  auth_hdr.length -= CONFIG_HDR_LEN;             /* Length includes header */
  
  /*-------------------------------------------------------------------------
    If bytes received are less than the length indicated in the header, 
    discard it (Note that the header has already been stripped above (in
    ntohcnf)), so the pkt_length is length of the data portion.

    RFC 1994 Section 4. Packet Format
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |     Code      |  Identifier   |            Length             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |    Data ...
    +-+-+-+-+
  -------------------------------------------------------------------------*/
  if( auth_hdr.length > dsm_length_packet( *item_ref_ptr ) )
  {
    LOG_MSG_ERROR_1("Dev %d rxed malformed CHAP pkt, discarding", device );
    dsm_free_packet( item_ref_ptr );
    return;
  }

  dsm_trim_packet(item_ref_ptr, auth_hdr.length);      /* Trim off padding */

  PPP_INC_INSTANCE_STATS(device,chap_pkts_rx, 1);
  /*-------------------------------------------------------------------------
    do procession based on the message code.
  -------------------------------------------------------------------------*/
  switch(auth_hdr.code)
  {
    case PPP_CHAP_CHALLENGE:
    {
      /*---------------------------------------------------------------------
        Recieved Challenge. Process the Challenge and genarate the Response.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("dev %d recieved CHAP challenge", device);

      /*---------------------------------------------------------------------
        store the id of this packet as the active on:  for use in retries.
      ---------------------------------------------------------------------*/
      ppp_cb_ptr->auth.active_id = auth_hdr.identifier;

      /*---------------------------------------------------------------------
        Free the packet in chap_challenge_ptr if it already exists
      ---------------------------------------------------------------------*/
      if(ppp_cb_ptr->auth.chap_challenge_ptr != NULL)
      {
        dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
      }
      else
      {
        pppi_call_event_callback( &ppp_cb_ptr->fsm[LCP],
                                  PPP_PROTOCOL_AUTH,
                                  PPP_STARTING_EV );
      }

      /*---------------------------------------------------------------------
        Extract the length of the challenge from the packet.  Obtain an
        dsm_buffer for the chap_challenge_ptr.  This buffer will be freed
       	after receiving SUCCESS, FAILURE or all the time outs occur.  Store
       	in the used field of the chap_challenge_ptr.
      ---------------------------------------------------------------------*/
      challenge_len = dsm_pull8(item_ref_ptr);
      if( -1 == challenge_len )
      {
        LOG_MSG_ERROR_0( "Malformed CHAP packet received" );
        dsm_free_packet(item_ref_ptr);
        return;
      }

      LOG_MSG_INFO3_2("dev %d CHAP chal_len = %d", device, challenge_len);

      ppp_cb_ptr->auth.chap_challenge_ptr =
        dsm_offset_new_buffer(DSM_DS_POOL_SIZE(challenge_len + PPP_HDR_LEN),
                            PPP_HDR_LEN);
      if(ppp_cb_ptr->auth.chap_challenge_ptr == NULL)
      {
        /*-------------------------------------------------------------------
          give up at this point - but don't bail - when we get another
          challenge there may be more memory.
        -------------------------------------------------------------------*/
        LOG_MSG_INFO1_1("dev %d chap_proc() alloc failed", device);
        break;
      }

      ppp_cb_ptr->auth.chap_challenge_ptr->used = (uint16)challenge_len;
      ASSERT(ppp_cb_ptr->auth.chap_challenge_ptr->used <=
             ppp_cb_ptr->auth.chap_challenge_ptr->size);

      /*---------------------------------------------------------------------
        Copy the challenge from incoming packet into the area pointed by
        chap_challenge_ptr. The data can be linked through several dsm_items
        so only dsm_extract() should be used.  No offset is needed as the
        data pointer is now pointing to the challenge.
      ---------------------------------------------------------------------*/
      if( 0 == dsm_extract(*item_ref_ptr,
                           0,
                           ppp_cb_ptr->auth.chap_challenge_ptr->data_ptr,
                           ppp_cb_ptr->auth.chap_challenge_ptr->used) )
      {
        LOG_MSG_ERROR_0( "Malformed CHAP packet received" );
        dsm_free_packet(item_ref_ptr);
        return;
      }

      /*---------------------------------------------------------------------
        Transmit the CHAP packet after building the response.
      ---------------------------------------------------------------------*/
      chapi_send_resp(ppp_cb_ptr);
    } /* CHAP_CHALLENGE */
    break;

    case PPP_CHAP_RESPONSE:
    {
      /*-------------------------------------------------------------------
        Got response - make sure that we are trying to authenticate the
        peer - if not free packet and bail out.
      -------------------------------------------------------------------*/
      if(!(ppp_cb_ptr->auth.flags & PPP_AP_REMOTE))
      {
        LOG_MSG_INFO1_1("dev %d got CHAP response when not authenticating peer!",
                        device);
        break;
      }

      /*---------------------------------------------------------------------
        make sure that the id in the header is the current active one.
      ---------------------------------------------------------------------*/
      else if(auth_hdr.identifier != ppp_cb_ptr->auth.active_id)
      {
        LOG_MSG_INFO2_3("dev %d CHAP response has wrong ID %d != %d - dicarding",
                device,
                auth_hdr.identifier,
                ppp_cb_ptr->auth.active_id);
        break;
      }

      /*---------------------------------------------------------------------
        Save the authentication information and send the success message.
        NOTE: it is not necessary to stop the retry timer as ps_timers run in
        PS context and so cannot interrupt us.
      ---------------------------------------------------------------------*/
      else
      {
        if(chapi_save_auth_info(ppp_cb_ptr,
                                item_ref_ptr,
                                auth_hdr.length))
        {
          authi_send_success(ppp_cb_ptr, PPP_CHAP_SUCCESS);
          /*-----------------------------------------------------------------
            call auth_complete() - if success is lost, peer should send another
            response with old packet information, which will contain the same
            information - also since chap can happen whenever it doesn't matter
            if we are in the IPCP stage.
          -----------------------------------------------------------------*/
          ppp_auth_complete(ppp_cb_ptr, PPP_CHAP_PROTOCOL);
        }
      }
    } /* CHAP_RESPONSE */
    break;

    case PPP_CHAP_SUCCESS:
    {
      /*---------------------------------------------------------------------
        Got CHAP Success - if the ID matches, authentication is complete
      ---------------------------------------------------------------------*/
      if(auth_hdr.identifier == ppp_cb_ptr->auth.active_id)
      {
        LOG_MSG_INFO2_1( "dev %d got CHAP Success", device);
        ppp_auth_complete(ppp_cb_ptr, PPP_CHAP_PROTOCOL);
      }
      else
      {
        LOG_MSG_INFO1_3("dev %d got CHAP Success with bad id %d, expecting %d",
                 device,
                 auth_hdr.identifier,
                 ppp_cb_ptr->auth.active_id);
      }
    } /* CHAP_SUCCESS */
    break;

    case PPP_CHAP_FAILURE:
    {
      /*---------------------------------------------------------------------
        Got CHAP Failure - if the ID matches, authentication failed.
      ---------------------------------------------------------------------*/
      if(auth_hdr.identifier == ppp_cb_ptr->auth.active_id)
      {
        PPP_INC_INSTANCE_STATS(device,chap_auth_failures, 1);
        LOG_MSG_INFO1_1( "dev %d got CHAP Failure.", device);
        ppp_cb_ptr->auth.flags |= PPP_AP_FAILED;
        ppp_auth_complete(ppp_cb_ptr, PPP_CHAP_PROTOCOL);

        /*-------------------------------------------------------------------
          When authentication fails on UM SN device, JCDMA requires that the 
          mobile actively terminates the call.
        -------------------------------------------------------------------*/
        if( ppp_cb_ptr->ppp_failure_graceful_close == TRUE )
        {
          if(device == PPP_UM_SN_DEV)
          {
            (void)ppp_stop(ppp_cb_ptr->fsm[LCP].device, PPP_PROTOCOL_LCP);
          }
        }
      }
      else
      {
        LOG_MSG_INFO1_3("dev %d got CHAP Failure with bad id %d, expecting %d",
                 device,
                 auth_hdr.identifier,
                 ppp_cb_ptr->auth.active_id);
      }
    } /* CHAP_FAILURE */
    break;

    default:
      LOG_MSG_INFO2_2 ("dev %d got unknown CHAP msg %d.",
                       device, auth_hdr.code);

  } /* switch(chap msg type) */

  /*-------------------------------------------------------------------------
    As the processing is complete - free the incoming packet.
  -------------------------------------------------------------------------*/
  dsm_free_packet(item_ref_ptr);

} /* chap_proc() */



/*===========================================================================
FUNCTION PAP_PROC()

DESCRIPTION
  This function does the protocol processing for PAP.

PARAMETERS
  device:       the PPP interface the packet arrived on
  item_ref_ptr: reference to pointer to dsm chain containing the PAP pkt.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pap_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
)
{
  ppp_config_hdr_type auth_hdr;
  ppp_type           *ppp_cb_ptr;
  ppp_type           *ppp_cb_array = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(device >= PPP_MAX_DEV)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("cannot process PAP for device %d!", device);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  /*-------------------------------------------------------------------------
    The rxed message format looks like:
      Code: 1 Octet (the message type)
      Identifier: 1 Octet
      Length: 2 Octets
  
    *item_ref_ptr will be pointing to the code field so starting here extract
    the first three fields.
  -------------------------------------------------------------------------*/
  if( -1 == ntohcnf(&auth_hdr, item_ref_ptr) )
  {
    LOG_MSG_ERROR_1("ppp%d Invalid PAP pkt", device );
    dsm_free_packet(item_ref_ptr);
    ASSERT(0);
    return;
  }
  
  auth_hdr.length -= CONFIG_HDR_LEN;             /* Length includes header */
  
  /*-------------------------------------------------------------------------
    If bytes received are less than the length indicated in the header, 
    discard it (Note that the header has already been stripped above (in
    ntohcnf)), so the pkt_length is length of the data portion.

    RFC 1334 Section 2.2 Packet Format
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |     Code      |  Identifier   |            Length             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |    Data ...
    +-+-+-+-+
  -------------------------------------------------------------------------*/
  if( auth_hdr.length > dsm_length_packet( *item_ref_ptr ) )
  {
    LOG_MSG_ERROR_1("Dev %d rxed malformed PAP pkt, discarding", device );
    dsm_free_packet( item_ref_ptr );
    return;
  }

  dsm_trim_packet(item_ref_ptr, auth_hdr.length);      /* Trim off padding */
  
  PPP_INC_INSTANCE_STATS(device,pap_pkts_rx, 1);
  /*-------------------------------------------------------------------------
    do procession based on the message code.
  -------------------------------------------------------------------------*/
  switch(auth_hdr.code)
  {
  case PPP_PAP_REQ:
    /*-----------------------------------------------------------------------
      got PAP request - make sure that we are trying to authenticate the
      peer.
    -----------------------------------------------------------------------*/
    if(!(ppp_cb_ptr->auth.flags & PPP_AP_REMOTE))
    {
      LOG_MSG_INFO1_1("dev %d got PAP Req when not authenticating peer!",
                      device);
      break;
    }

    /*-----------------------------------------------------------------------
      Save the ID and auth information and send the success message if the
      same is successful.
      NOTE: it is not necessary to stop the retry timer as ps_timers run in
      PS context and so cannot interrupt us.
    -----------------------------------------------------------------------*/
    else
    {
      ppp_cb_ptr->auth.active_id = auth_hdr.identifier;
      if(papi_save_auth_info(ppp_cb_ptr, item_ref_ptr) == TRUE)
      {
        authi_send_success(ppp_cb_ptr, PPP_PAP_ACK);
      }
      /*---------------------------------------------------------------------
        don't call auth_complete() as will move to network phase, and peer
        may not receive success.  Since PAP is rejected in the network phase
        we need to stay in the authenticate phase until we received IPCP
        packet (which will move us to the network phase).
      ---------------------------------------------------------------------*/
    }
    break;

  case PPP_PAP_NAK:
    /*-----------------------------------------------------------------------
      Got a PAP Nak - if the ID matches, auth is completed - but has failed.
    -----------------------------------------------------------------------*/
    if(auth_hdr.identifier == ppp_cb_ptr->auth.active_id)
    {
      PPP_INC_INSTANCE_STATS(device,pap_auth_failures, 1);
      LOG_MSG_INFO2_1("dev %d got PAP Nak.", device);
      ppp_cb_ptr->auth.flags |= PPP_AP_FAILED;
      ppp_auth_complete(ppp_cb_ptr, PPP_PAP_PROTOCOL);
      /*---------------------------------------------------------------------
        When authentication fails, JCDMA requires that the mobile actively
        terminate the call.
      ---------------------------------------------------------------------*/
      if( ppp_cb_ptr->ppp_failure_graceful_close == TRUE )
      {
        (void)ppp_stop(ppp_cb_ptr->fsm[LCP].device, PPP_PROTOCOL_LCP);
      }
    }
    else
    {
      LOG_MSG_INFO1_3("dev %d got PAP Nak with bad id %d, expecting %d",
                      device, auth_hdr.identifier, ppp_cb_ptr->auth.active_id);
    }
    break;

  case PPP_PAP_ACK:
    if(auth_hdr.identifier == ppp_cb_ptr->auth.active_id)
    {
      /*---------------------------------------------------------------------
        Recieved Auth-Ack - if the ID matches, Authentication is complete.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("dev %d got PAP Ack.", device);
      ppp_auth_complete(ppp_cb_ptr, PPP_PAP_PROTOCOL);
    }
    else
    {
      LOG_MSG_INFO1_3("dev %d got PAP Ack with bad id %d, expecting %d",
                      device, auth_hdr.identifier, ppp_cb_ptr->auth.active_id);
    }
    break;

  default:
    LOG_MSG_INFO2_2 ("dev %d unknown PAP message %d.",
                     device, auth_hdr.identifier);

  } /* switch(pap msg type) */

  dsm_free_packet(item_ref_ptr);

} /* pap_proc() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION authi_send_success()

DESCRIPTION
  This function builds and transmits the auth success message.  It uses the
  code field to differentiate between PAP and CHAP (as the messages are
  almost the same).

PARAMETERS
  ppp_cb_ptr: ppp_control block pointer
  code: the code to transmit.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  the PPP_AP_SUCCESS flag is added to ppp_cb_ptr->auth.flags
===========================================================================*/
static void authi_send_success
(
  ppp_type *ppp_cb_ptr,
  uint8     code
)
{
  dsm_item_type *auth_success;
  uint8         *insert_ptr;
  uint16         success_len;
  uint16         protocol;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    determine the length of the packet - if the code indicates PAP then add
    one for the msg_length field - which we will set to 0.
  -------------------------------------------------------------------------*/
  success_len =  PPP_AUTH_CODE_SIZE
    + PPP_AUTH_ID_SIZE
    + PPP_AUTH_LENGTH_SIZE;

  if(code == PPP_PAP_ACK)
  {
    success_len++;
  }

  /*-------------------------------------------------------------------------
    Get an item that is big enough to fit the entire packet.  If it fails
    don't do anything special just return - the client presumably will
    retransmit response.
  -------------------------------------------------------------------------*/
  auth_success =
    dsm_offset_new_buffer(DSM_DS_POOL_SIZE(success_len + PPP_HDR_LEN),
                          PPP_HDR_LEN);
  if(auth_success == NULL || auth_success->data_ptr == NULL)
  {
    LOG_MSG_ERROR_1("dev %d auth-ack mem alloc failed",
                    ppp_cb_ptr->fsm[LCP].device);
    return;
  }

  /*-------------------------------------------------------------------------
    Fill in the following fields in the dsm item:
        used field with the total response length;
        Priority as Highest.
  -------------------------------------------------------------------------*/
  auth_success->used     = success_len;
  auth_success->priority = DSM_HIGHEST;
  DSM_TOUCH_ITEM(auth_success);

  /*-------------------------------------------------------------------------
    Both PAP and CHAP success messages look like:

    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |     Code      |  Identifier   |            Length             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |  Msg-Length   |  Message  ...
    +-+-+-+-+-+-+-+-+-+-+-+-+-

    Note for CHAP msg-length doesn't exist and is part of the message field -
    the code field will allow us to omit this.
  -------------------------------------------------------------------------*/
  insert_ptr    = auth_success->data_ptr;
  *insert_ptr++ = code;
  *insert_ptr++ = ppp_cb_ptr->auth.active_id; /* this was stored on rx */
  insert_ptr    = put16(insert_ptr, success_len);

  if (insert_ptr == NULL)
  {
    LOG_MSG_ERROR_0("authi_send_success():"
                    "NULL ptr returned by put16");
    return;
  }
  /*-------------------------------------------------------------------------
    if this is a PAP auth-ack then set the msg-length to zero, in either case
    set the kind field to the correct protocol type.
  -------------------------------------------------------------------------*/
  if(code == PPP_PAP_ACK)
  {
    *insert_ptr++ = 0;
    protocol = PPP_PAP_PROTOCOL;
    PPP_INC_INSTANCE_STATS_PTR(ppp_cb_ptr,pap_pkts_tx, 1);
  }
  else
  {
    protocol = PPP_CHAP_PROTOCOL;
    PPP_INC_INSTANCE_STATS_PTR(ppp_cb_ptr, chap_pkts_tx, 1);
  }

  /*-------------------------------------------------------------------------
    we are done - so transmit the packet - if it fails we wait for the client
    to retrnsmit generating another success message.
  -------------------------------------------------------------------------*/
  if(pppi_outgoing(ppp_cb_ptr->fsm[LCP].device,
                   protocol,
                   &auth_success,
                   NULL) == -1)
  {
    LOG_MSG_INFO1_1("dev %d sending auth success failed!",
                    ppp_cb_ptr->fsm[LCP].device);
  }
  else
  {
    ppp_cb_ptr->auth.flags |= PPP_AP_SUCCESS;
  }

} /* authi_send_success() */



/*===========================================================================
FUNCTION papi_send_auth_req()

DESCRIPTION
  Builds and transfers the PAP packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void papi_send_auth_req
(
  ppp_type *ppp_cb_ptr
)
{
  dsm_item_type *auth_req;          /* Pointer for the PAP packet          */
  int            pap_pkt_len;       /* Length of the PAP Packet            */
  uint8         *insert_ptr;        /* Pointer to the data in the packet   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Since id-len and passwd-len do not include their own size, we add 2
  -----------------------------------------------------------------------*/
  pap_pkt_len = PPP_AUTH_CODE_SIZE
    + PPP_AUTH_ID_SIZE
    + PPP_AUTH_LENGTH_SIZE
    + ppp_cb_ptr->auth.info.user_id_len
    + ppp_cb_ptr->auth.info.passwd_len + 2;

  /*-------------------------------------------------------------------------
    Get an item that is big enough to fit the entire packet.  If the
    allocation fails skip forward to the timer - this will trigger another
    attempt.
  -------------------------------------------------------------------------*/
  auth_req =
    dsm_offset_new_buffer(DSM_DS_POOL_SIZE(pap_pkt_len + PPP_HDR_LEN),
                          PPP_HDR_LEN);
  if(NULL == auth_req || NULL == auth_req->data_ptr)
  {
    LOG_MSG_ERROR_1("dev %d auth-req mem alloc failed",
                    ppp_cb_ptr->fsm[LCP].device);
    goto give_up;
  }

  /*-------------------------------------------------------------------------
    Fill in the following fields in the dsm item:
        used field with the total response length;
        Priority as Highest.
  -------------------------------------------------------------------------*/
  auth_req->used     = (uint16)pap_pkt_len;
  auth_req->priority = DSM_HIGHEST;
  DSM_TOUCH_ITEM(auth_req);

  /*-----------------------------------------------------------------------
    An auth request looks like:

      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | 1 (the value) |  Identifier   |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | Peer-ID Length|  Peer-Id ...
      +-+-+-+-+-+-+-+-+-+-+-+-+
      | Passwd-Length |  Password  ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+

    fill in the various fields.
  -----------------------------------------------------------------------*/
  insert_ptr    = auth_req->data_ptr;
  *insert_ptr++ = PPP_PAP_REQ;
  *insert_ptr++ = ++ppp_cb_ptr->auth.active_id;
  insert_ptr    = put16( insert_ptr, (uint16)pap_pkt_len);

  if (insert_ptr == NULL)
  {
    LOG_MSG_ERROR_0("papi_send_auth_req():"
                    "NULL ptr returned by put16");
    goto give_up;
  }
  /*-------------------------------------------------------------------------
    insert the peer id length and the id itself, as well as the password
    length and the password.
  -------------------------------------------------------------------------*/
  *insert_ptr++ = ppp_cb_ptr->auth.info.user_id_len;
  memscpy(insert_ptr,
          auth_req->data_ptr + auth_req->used - insert_ptr, 
         ppp_cb_ptr->auth.info.user_id_info,
         ppp_cb_ptr->auth.info.user_id_len);
  insert_ptr += ppp_cb_ptr->auth.info.user_id_len;
  *insert_ptr++ = ppp_cb_ptr->auth.info.passwd_len;
  /*-------------------------------------------------------------------------
    The code tries to access dsm item without dsm API:
    1. data_ptr: point to the beginning the the dsm buffer
    2. used: the size of buffer it expect to use
    3. insert_ptr: point to the current avaliable buffer 
    so, the remaining valid buffer size equals 
                 the beginning of  buffer + used size - the current pointer 
  -------------------------------------------------------------------------*/
  memscpy(insert_ptr,
          auth_req->data_ptr + auth_req->used - insert_ptr,
          ppp_cb_ptr->auth.info.passwd_info,
          ppp_cb_ptr->auth.info.passwd_len);

  PPP_INC_INSTANCE_STATS_PTR(ppp_cb_ptr, pap_pkts_tx, 1);
  /*-------------------------------------------------------------------------
    we are done - so transmit the packet
  -------------------------------------------------------------------------*/
  if(pppi_outgoing(ppp_cb_ptr->fsm[LCP].device,
                   PPP_PAP_PROTOCOL,
                   &auth_req,
                   NULL) == -1)
  {
    /*---------------------------------------------------------------------
      pppi_outgoing() frees the packet in case of an error - let the timer
      to trigger another attempt.
    ---------------------------------------------------------------------*/
    LOG_MSG_INFO1_1("dev %d sending PAP auth req failed!",
                    ppp_cb_ptr->fsm[LCP].device);
  }

  /*-------------------------------------------------------------------------
    Start the retry timer.
    NOTE: we use the LCP timer as authentication doesn't have it's own timer.
  --------------------------------------------------------------------------*/
 give_up:
  (void)ps_timer_start(ppp_cb_ptr->fsm[LCP].timer,
                       (int64)ppp_cb_ptr->auth.timeout);

} /* papi_send_auth_req() */



/*===========================================================================
FUNCTION PAPI_SAVE_AUTH_INFO()

DESCRIPTION
  This function will parse the PAP packet and store the authentication
  information that was contained with in.  It assumes that the first byte in
  the item is the length.

PARAMETERS
  ppp_cb_ptr: pointer to the relevant PPP control block
  pap_req_ptr: pointer to the dsm_item containing the pap request.

RETURN VALUE
  TRUE  on success
  FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean papi_save_auth_info
(
  ppp_type       *ppp_cb_ptr,
  dsm_item_type **pap_req_ref_ptr
)
{
  ppp_auth_info_type *auth_info_ptr;
  int                 length;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  auth_info_ptr = &(ppp_cb_ptr->auth.info);
  /*-------------------------------------------------------------------------
    A PAP auth request looks like:

      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |     Code      |  Identifier   |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | Peer-ID Length|  Peer-Id ...
      +-+-+-+-+-+-+-+-+-+-+-+-+
      | Passwd-Length |  Password  ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+

    The code, ID and length have been stripped off at this point so we need
    to extract and copy the remaining fields.
    NOTE: if the passed in username or password is bigger than we can handle
    we return an error.
  -------------------------------------------------------------------------*/
  length = dsm_pull8(pap_req_ref_ptr);
  if(-1 == length || PPP_MAX_USER_ID_LEN < length)
  {
    return FALSE;
  }
  if(dsm_pullup(pap_req_ref_ptr,
                auth_info_ptr->user_id_info,
                (uint16)length) != length)
  {
    return FALSE;
  }
  auth_info_ptr->user_id_len = (uint8)length;

  length = dsm_pull8(pap_req_ref_ptr);
  if(-1 == length || PPP_MAX_PASSWD_LEN < length)
  {
    return FALSE;
  }
  if(dsm_pullup(pap_req_ref_ptr,
                auth_info_ptr->passwd_info,
                (uint16)length) != length)
  {
    return FALSE;
  }
  auth_info_ptr->passwd_len = (uint8)length;

  return TRUE;
} /* papi_save_auth_info() */



/*===========================================================================
FUNCTION CHAPI_SAVE_AUTH_INFO()

DESCRIPTION
  This function will parse the CHAP packet and store the authentication
  information that was contained with in.  It assumes that the first byte in
  the item is the length.

PARAMETERS
  ppp_cb_ptr: pointer to the relevant PPP control block
  chap_resp_ptr: pointer to the dsm_item containing the chap request.
  length: the total length of the chap packet - needed to get the name.

RETURN VALUE
  TRUE  on success
  FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean chapi_save_auth_info
(
  ppp_type       *ppp_cb_ptr,
  dsm_item_type **chap_resp_ref_ptr,
  uint16          length
)
{
  ppp_auth_info_type *auth_info_ptr;
  int                 hash_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  auth_info_ptr = &(ppp_cb_ptr->auth.info);
  /*-------------------------------------------------------------------------
    A CHAP response looks like:

      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |     Code      |  Identifier   |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  Hash length  |  Hash
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  Client name ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    The code, ID and length have been stripped off at this point so we need
    to extract and copy the remaining fields.  The hash length and hash are
    stored in the password section.
  -------------------------------------------------------------------------*/
  hash_len = dsm_pull8(chap_resp_ref_ptr);
  if(-1 == hash_len || PPP_MAX_PASSWD_LEN < hash_len)
  {
    return FALSE;
  }
  if(dsm_pullup(chap_resp_ref_ptr,
                auth_info_ptr->passwd_info,
                (uint16)hash_len) != hash_len)
  {
    return FALSE;
  }
  auth_info_ptr->passwd_len = (uint8)hash_len;

  /*-------------------------------------------------------------------------
    calculate the length of the client name field (i.e. the user ID).
  -------------------------------------------------------------------------*/
  length -= PPP_CHAP_VALUE_SIZE + (uint16)hash_len;
  if(PPP_MAX_USER_ID_LEN < length)
  {
    return FALSE;
  }
  if(dsm_pullup(chap_resp_ref_ptr,
                auth_info_ptr->user_id_info,
                length) != length)
  {
    return FALSE;
  }
  auth_info_ptr->user_id_len = (uint8)length;

  return TRUE;

} /* chapi_save_auth_info() */


#ifdef FEATURE_RUIM_FUNC_INTERFACE
/*===========================================================================
FUNCTION CHAPI_UIM_3GPD_CALC_CHAP_RESP_RPT()

DESCRIPTION
  Retrieves the 16-byte Hash/CHAP Response from the mmgsdi response, 
  and copies into variables local to the file.  This is a callback 
  function which is called from the mmgsdi/uim context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the ppp_uim_auth.report_status and ppp_uim_auth.challenge_response.
===========================================================================*/
static void chapi_uim_3gpd_calc_chap_resp_rpt
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  mmgsdi_compute_ip_auth_cnf_type         *compute_ip_cnf_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_uim_auth.report_status = UIM_FAIL;
  ppp_uim_auth.sw_info.present = FALSE;
  ppp_uim_auth.sw_info.sw1 = 0;
  ppp_uim_auth.sw_info.sw2 = 0;
  if( NULL == cnf_ptr )
  {
    LOG_MSG_ERROR_0("chapi_uim_3gpd_calc_chap_resp_rpt: UIM 3GPD CHAP"
                    " command failure cnf_ptr NULL");
    PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);
    return;
  }
  compute_ip_cnf_ptr = (mmgsdi_compute_ip_auth_cnf_type *) 
                       &(cnf_ptr->compute_ip_cnf);

  /*-------------------------------------------------------------------------
    Copy the uim report status into a variable local to the file
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_SESSION_LIB
  if (compute_ip_cnf_ptr->response_header.client_id 
        != ppp_mmgsdi_auth_config.reg_info.client_id)
  {
    LOG_MSG_ERROR_2("chapi_uim_3gpd_calc_chap_resp_rpt: UIM 3GPD CHAP"
              " command failure : Client ids do not match! "
              " response client id %d, client id %d", 
              compute_ip_cnf_ptr->response_header.client_id, 
                    ppp_mmgsdi_auth_config.reg_info.client_id);
    PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);
    return;
  }
#endif /* FEATURE_MMGSDI_SESSION_LIB */

  if ( ( MMGSDI_COMPUTE_IP_AUTH_CNF == cnf ) && 
       ( MMGSDI_COMPUTE_IP_AUTH_CNF == 
         compute_ip_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) && 
         ( MMGSDI_SUCCESS == 
           compute_ip_cnf_ptr->response_header.mmgsdi_status ) 
       )
    {
      ppp_uim_auth.report_status = UIM_PASS;
    /*-----------------------------------------------------------------------
      The uim calc chap command was successful.  This copies the data from
      the uim report
    -----------------------------------------------------------------------*/
      memscpy(ppp_uim_auth.challenge_response,
             UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH,
             compute_ip_cnf_ptr->cmpt_ip_response_data.data_ptr,
             compute_ip_cnf_ptr->cmpt_ip_response_data.data_len);
    }
    else
    {
      ppp_uim_auth.sw_info.present = 
      	  compute_ip_cnf_ptr->response_header.status_word.present;
      ppp_uim_auth.sw_info.sw1 = 
      	  compute_ip_cnf_ptr->response_header.status_word.sw1;
      ppp_uim_auth.sw_info.sw2 = 
      	  compute_ip_cnf_ptr->response_header.status_word.sw2;
      LOG_MSG_ERROR_2("chapi_uim_3gpd_calc_chap_resp_rpt: UIM 3GPD CHAP"
                " command failure status %d and mmgsdi_status %d are ",
                status, 
                      compute_ip_cnf_ptr->response_header.mmgsdi_status);
    }
  }
  else
  {
    LOG_MSG_ERROR_2("chapi_uim_3gpd_calc_chap_resp_rpt:"
              " Improper command confirmation cnf %d response_type %d",
                    cnf, compute_ip_cnf_ptr->response_header.response_type);
  }

  /*-------------------------------------------------------------------------
    Set the signal to indicate to the PS task that the
    uim command has completed
  -------------------------------------------------------------------------*/
  PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);

} /* chapi_uim_3gpd_calc_chap_resp_rpt() */
#endif /* FEATURE_RUIM_FUNC_INTERFACE */

#ifdef FEATURE_RUIM_FUNC_INTERFACE
#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION CHAPI_UIM_CALC_CHAP_RESP_RPT()

DESCRIPTION
  Retrieves the 16-byte Hash/CHAP Response from the mmgsdi response, and 
  copies into variables local to the file.  This is a callback function 
  which is called from the mmgsdi/uim context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the ppp_uim_auth.report_status and ppp_uim_auth.challenge_response.
===========================================================================*/
static void chapi_uim_calc_chap_resp_rpt
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)

{
  mmgsdi_compute_ip_auth_cnf_type         *compute_ip_cnf_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_uim_auth.report_status = UIM_FAIL;
  if( NULL == cnf_ptr )
  {
    LOG_MSG_ERROR_0("chapi_uim_calc_chap_resp_rpt: UIM-CHAP command failure"
                    " cnf_ptr NULL");
    PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);
    return;
  }
  compute_ip_cnf_ptr = (mmgsdi_compute_ip_auth_cnf_type *) 
                     &(cnf_ptr->compute_ip_cnf);
  /*-------------------------------------------------------------------------
    Copy the uim report status into a variable local to the file
  -------------------------------------------------------------------------*/
  if (compute_ip_cnf_ptr->response_header.client_id 
        != ppp_mmgsdi_auth_config.reg_info.client_id)
  {
    LOG_MSG_ERROR_2("chapi_uim_calc_chap_resp_rpt: UIM-CHAP command failure"
             " Client ids do not match! response client id %d, client id %d", 
             compute_ip_cnf_ptr->response_header.client_id, 
                    ppp_mmgsdi_auth_config.reg_info.client_id);
    PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);
    return;
  }

  if ( ( MMGSDI_COMPUTE_IP_AUTH_CNF == cnf ) && 
       ( MMGSDI_COMPUTE_IP_AUTH_CNF == 
         compute_ip_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) && 
         ( MMGSDI_SUCCESS == 
           compute_ip_cnf_ptr->response_header.mmgsdi_status ) 
       )
    {
      ppp_uim_auth.report_status = UIM_PASS;
/*-----------------------------------------------------------------------
      The uim calc chap command was successful.  
      This copies the data from the uim report
-----------------------------------------------------------------------*/
      memscpy(ppp_uim_auth.challenge_response,
             UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH,
             compute_ip_cnf_ptr->cmpt_ip_response_data.data_ptr,
             compute_ip_cnf_ptr->cmpt_ip_response_data.data_len);

    }
    else
    {
      LOG_MSG_ERROR_2("chapi_uim_calc_chap_resp_rpt: UIM - CHAP command failure"
                      " status %d and mmgsdi_status %d are ",
                      status,
                      compute_ip_cnf_ptr->response_header.mmgsdi_status);
    }
  }
  else
  {
    LOG_MSG_ERROR_2("chapi_uim_calc_chap_resp_rpt: Improper command confirmation " 
                    " cnf %d response_type %d",
                    cnf, compute_ip_cnf_ptr->response_header.response_type);
  }

  /*-------------------------------------------------------------------------
    Set the signal to indicate to the PS task that the
    uim command has completed
  -------------------------------------------------------------------------*/
  PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);

} /* chapi_uim_calc_chap_resp_rpt() */
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_RUIM_FUNC_INTERFACE */


#ifdef FEATURE_UIM_CAVE_AN_AUTH
#ifdef FEATURE_RUIM_FUNC_INTERFACE
#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION CHAPI_UIM_AN_CAVE_RESP_RPT()

DESCRIPTION
  Retrieves the AN Cave Response from the mmgdsi callback, and copies
  into variables local to the file.  This is a callback function which
  is called from the mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the ppp_uim_auth.report_status and 
  ppp_uim_auth.challenge_response, and ppp_uim_auth.run_cave_response
===========================================================================*/
static void chapi_uim_an_cave_resp_rpt
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)

{
  mmgsdi_run_cave_cnf_type *run_cave_cnf_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ppp_uim_auth.report_status = UIM_FAIL;
  if( NULL == cnf_ptr )
  {
    LOG_MSG_ERROR_0("chapi_uim_an_cave_resp_rpt: RUN CAVE command failure"
                    " cnf_ptr NULL");
    PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);
    return;
  }

  run_cave_cnf_ptr =(mmgsdi_run_cave_cnf_type *) &(cnf_ptr->run_cave_cnf);
  /*-------------------------------------------------------------------------
    Copy the uim report status into a variable local to the file
  -------------------------------------------------------------------------*/
  if (run_cave_cnf_ptr->response_header.client_id 
        != ppp_mmgsdi_auth_config.reg_info.client_id)
  {
    LOG_MSG_ERROR_2("chapi_uim_an_cave_resp_rpt: RUN CAVE command failure"
              " Client ids do not match! response client id %d, client id %d", 
              run_cave_cnf_ptr->response_header.client_id, 
                    ppp_mmgsdi_auth_config.reg_info.client_id);
    PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);
    return;
  }

  if ( ( MMGSDI_RUN_CAVE_CNF == cnf ) && 
       ( MMGSDI_RUN_CAVE_CNF == 
         run_cave_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) && 
       ( MMGSDI_SUCCESS == run_cave_cnf_ptr->response_header.mmgsdi_status ) )
    {
      ppp_uim_auth.report_status = UIM_PASS;

      ppp_uim_auth.run_cave_response =
              run_cave_cnf_ptr->run_cave_response_data;
      /* Shift the 18-bit AUTHR to the MSB */
      ppp_uim_auth.run_cave_response <<= 14;

      b_packd(ppp_uim_auth.run_cave_response,
              ppp_uim_auth.challenge_response,
              0,
              FSIZ(ppp_auth_run_cave_req_siz_type, rand));
    }
    else
    {
      LOG_MSG_ERROR_2("chapi_uim_an_cave_resp_rpt: RUN CAVE command failure"
                      " status %d and mmgsdi_status %d are ",
                      status, run_cave_cnf_ptr->response_header.mmgsdi_status);
    }
  }
  else
  {
    LOG_MSG_ERROR_2("chapi_uim_an_cave_resp_rpt: Improper command confirmation"
             " cnf %d response_type %d", 
                    cnf, run_cave_cnf_ptr->response_header.response_type);
  }

  /*-------------------------------------------------------------------------
    Set the signal to indicate to the PS task that the
    uim command has completed
  -------------------------------------------------------------------------*/

  PS_SET_SIGNAL(PS_UIM_CMD_SIGNAL);

} /* chapi_uim_an_cave_resp_rpt() */
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_RUIM_FUNC_INTERFACE */
#endif /* FEATURE_UIM_CAVE_AN_AUTH */


/*===========================================================================
FUNCTION CHAPI_CALC_CHAP_MD5_SW_RESPONSE()

DESCRIPTION
  Generates the 16-byte Hash/CHAP Response.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static byte * chapi_calc_chap_md5_sw_response
(
  ppp_type *ppp_cb_ptr,
  dsm_item_type *response_ptr,
  int response_len
)
{
  MD5_CTX context; /* will contain the seed for making MD5 output. */
  byte* insert_ptr;
  uint8 msg_id = ppp_cb_ptr->auth.active_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == response_ptr || NULL == response_ptr->data_ptr)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_md5_sw_response():"
                    " response_ptr or response_ptr->data_ptr is NULL");
    return NULL;
  }

  if(NULL == ppp_cb_ptr->auth.chap_challenge_ptr)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_md5_sw_response():"
                    " auth.chap_challenge_ptr is NULL");
    return NULL;
  }

  insert_ptr    = response_ptr->data_ptr;
  *insert_ptr++ = PPP_CHAP_RESPONSE;
  *insert_ptr++ = msg_id;
  insert_ptr    = put16(insert_ptr, (uint16)response_len);
  if (insert_ptr == NULL)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_md5_sw_response():"
                    "NULL ptr returned by put16");
    return NULL;
  }
  *insert_ptr++ = PPP_CHAP_MD5_SIZE;

  /*-------------------------------------------------------------------------
    Initialize MD5 with a new context, then generate hash.  Values are:
      CHAP ID
      CHAP/PAP password
      CHAP challenge
  --------------------------------------------------------------------------*/
  MD5Init(&context);
  MD5Update(&context, &(msg_id), 1);
  MD5Update(&context,
            (unsigned char *) ppp_cb_ptr->auth.info.passwd_info,
            ppp_cb_ptr->auth.info.passwd_len);
  /*--------------------------------------------------------------------------
    NOTE:  We are currently limited to a single DSM item to hold the
           CHAP challenge value.  We may need to traverse a dsm chain here.
  --------------------------------------------------------------------------*/
  MD5Update(&context,
            ppp_cb_ptr->auth.chap_challenge_ptr->data_ptr,
            ppp_cb_ptr->auth.chap_challenge_ptr->used);

  /*--------------------------------------------------------------------------
    The hash has been computed
    CHAP responses look like:

      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | 2 (the value) |  Identifier   |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | 16 (hash len) |  Hash value
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |  username ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    fill in the fields.
  --------------------------------------------------------------------------*/
  MD5Final(insert_ptr, &context);

  /*--------------------------------------------------------------------------
    Finalize MD5 hash, providing the CHAP response and insert
  --------------------------------------------------------------------------*/
  insert_ptr += PPP_CHAP_MD5_SIZE; /* move ptr forward by hash length */
  return insert_ptr;

} /* chapi_calc_chap_md5_sw_response */



/*===========================================================================
FUNCTION CHAPI_CALC_CHAP_MD5_RUIM_RESPONSE()

DESCRIPTION
  Generates the 16-byte Hash/CHAP Response by sending a command to the
  RUIM with the chap challenge and profile # as input.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static byte * chapi_calc_chap_md5_ruim_response
(
  ppp_type *       ppp_cb_ptr,
  dsm_item_type *  response_ptr,
  int *            response_len
)
{
  byte* insert_ptr;
  uint8 msg_id = 0;
  mmgsdi_compute_ip_data_type        cmpt_ip_data;
  mmgsdi_callback_type               response_cb_ptr = NULL;
  mmgsdi_return_enum_type            retval;
  nv_stat_enum_type                  nv_status;
  nv_item_type                       nv_item;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == ppp_cb_ptr )
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_md5_ruim_response():"
                    " ppp_cb_ptr is NULL");
    return NULL;
  }

  if(NULL == response_ptr || NULL == response_ptr->data_ptr)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_md5_ruim_response():"
                    " response_ptr or response_ptr->data_ptr is NULL");
    return NULL;
  }

  if(NULL == ppp_cb_ptr->auth.chap_challenge_ptr)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_md5_ruim_response():"
                    " auth.chap_challenge_ptr is NULL");
    return NULL;
  }
  
#ifdef FEATURE_RUIM_FUNC_INTERFACE
  /*------------------------------------------------------------------------
    Check whether a session has already been registered with mmgsdi module
  ------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_SESSION_LIB
  if (FALSE == ppp_mmgsdi_auth_config.reg_info.session_info.session_active)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_md5_ruim_response:"
                    " PPP Client not registered");
    return NULL;
  }  
#endif /* FEATURE_MMGSDI_SESSION_LIB */
#endif /* FEATURE_RUIM_FUNC_INTERFACE */
  msg_id = ppp_cb_ptr->auth.active_id;

  cmpt_ip_data.operation_type = MMGSDI_CMPT_IP_SIP_CHAP;
  cmpt_ip_data.data.sip_chap_data.chap_id = msg_id;

  cmpt_ip_data.data.sip_chap_data.nai_entry_index = 
           ppp_cb_ptr->auth.info.nai_entry_index;
  cmpt_ip_data.data.sip_chap_data.chap_challenge_length = 
           (mmgsdi_len_type)ppp_cb_ptr->auth.chap_challenge_ptr->used;
  memscpy(cmpt_ip_data.data.sip_chap_data.chap_challenge,
          MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH,
          ppp_cb_ptr->auth.chap_challenge_ptr->data_ptr,
          ppp_cb_ptr->auth.chap_challenge_ptr->used);

  response_cb_ptr = chapi_uim_3gpd_calc_chap_resp_rpt;

  /*------------------------------------------------------------------------
    Run the loop to calculate CHAP response at least once and fallback if
    CHAP response calculation fails for non-zeo index with 6A/80 SW failure
  ------------------------------------------------------------------------*/
  do
  {	
    /*------------------------------------------------------------------------
      Build the CHAP response in the provided DSM item
    ------------------------------------------------------------------------*/
    insert_ptr    = response_ptr->data_ptr;
    *insert_ptr++ = PPP_CHAP_RESPONSE;
    *insert_ptr++ = msg_id;
    insert_ptr    = put16(insert_ptr,(uint16)*response_len);
    
    if (NULL == insert_ptr)
    {
      LOG_MSG_ERROR_0("chapi_calc_chap_md5_ruim_response():"
                      "NULL ptr returned by put16");
      return NULL;
    }
    
    *insert_ptr++ = PPP_CHAP_MD5_SIZE;

    /*-------------------------------------------------------------------------
      Send the command to the R-UIM:
      Clear the "command done signal"
      Call the relevant mmgsdi COMPUTE AN IP AUTH function
      Wait for the command to be done
    -------------------------------------------------------------------------*/
    PS_CLR_SIGNAL(PS_UIM_CMD_SIGNAL);

    retval = mmgsdi_session_compute_ip_auth ( 
                     ppp_mmgsdi_auth_config.reg_info.session_info.session_id, 
                     cmpt_ip_data,
                     response_cb_ptr,
                     (mmgsdi_client_data_type) 0
             );

    if(MMGSDI_INCORRECT_PARAMS == retval) 
    {
      DATA_ERR_FATAL("chapi_calc_chap_md5_ruim_response: Incorrect Paramter values supplied to mmgsdi_session_compute_ip_auth algo");
      return NULL;
    }
    else if (MMGSDI_CMD_QUEUE_FULL == retval)
    {
      LOG_MSG_INFO2_0("chapi_calc_chap_md5_ruim_response:"
                      " Command Queue Full when trying to run" 
                      " mmgsdi_session_compute_ip_auth algo");
      return NULL;
    }
    LOG_MSG_INFO3_0("Blocking PS Task on UIM cmd signal");
    (void)ps_wait((rex_sigs_type) 1 << PS_UIM_CMD_SIGNAL); /* Wait for completion*/
    LOG_MSG_INFO3_0("Unblocking PS TASK on UIM cmd signal");
    if(ppp_uim_auth.report_status == UIM_PASS)
    {
    /*-------------------------------------------------------------------------
      The code tries to access dsm item without dsm API:
      1. data_ptr: point to the beginning the the dsm buffer
      2. used: the size of buffer it expect to use
      3. insert_ptr: point to the current avaliable buffer 
         so, the remaining valid buffer size equals 
         the beginning of  buffer + used size - the current pointer 
    -------------------------------------------------------------------------*/
      LOG_MSG_INFO1_0("UIM CHAP challenge response calculation success");
      memscpy(insert_ptr,
              response_ptr->data_ptr + response_ptr->used - insert_ptr,
              &ppp_uim_auth.challenge_response,
              UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH);
      break;
    }
    else
    {
      LOG_MSG_INFO1_1("UIM-CHAP challenge response calculation failure "
                      "for index %d",
      	              ppp_cb_ptr->auth.info.nai_entry_index);

      if( ppp_cb_ptr->auth.info.nai_entry_index > 0 &&
          (ppp_uim_auth.sw_info.present == TRUE &&
           ppp_uim_auth.sw_info.sw1 == PPP_AUTH_CHAP_FAILURE_SW1 &&
           ppp_uim_auth.sw_info.sw2 == PPP_AUTH_CHAP_FAILURE_SW2) )
      { 
        // Fallback to index 0.       
        cmpt_ip_data.data.sip_chap_data.nai_entry_index = 0;
        ppp_cb_ptr->auth.info.nai_entry_index = 0;

        LOG_MSG_INFO1_1("Trying CHAP for index %d",
        	        cmpt_ip_data.data.sip_chap_data.nai_entry_index);
                
        // Check if client has configured user-id.        
        if(ppp_cb_ptr->auth.info.is_userid_set_by_client)
        {
          LOG_MSG_INFO1_0("User-id set by client. Trying CHAP.");  
          continue;
        }
        else
        {
          /*-----------------------------------------------------------
            Read the PPP NAI ID from NV FOR DEFAULT PROFILE.
          -----------------------------------------------------------*/
          nv_item.ds_sip_nai_info.index = 
                                 ppp_cb_ptr->auth.info.nai_entry_index;
          nv_status = ps_get_nv_item( NV_DS_SIP_NAI_INFO_I, 
                                      &nv_item );  
         /*-------------------------------------------------------------
           If NV read succeeds, load the user ID into the provided ppp config
         -------------------------------------------------------------*/
          if( nv_status == NV_DONE_S )
          {
            ppp_cb_ptr->auth.info.user_id_len = 
            	            nv_item.ds_sip_nai_info.nai_length;
            memscpy( ppp_cb_ptr->auth.info.user_id_info,
                     ppp_cb_ptr->auth.info.user_id_len,
                     (char *)nv_item.ds_sip_nai_info.nai,
                     ppp_cb_ptr->auth.info.user_id_len );
            LOG_MSG_INFO1_0("User id read successfully");
          }
          else
          {
            ppp_cb_ptr->auth.info.user_id_len = 0;
            LOG_MSG_INFO1_0("User-id not set set in default index. Not trying CHAP");
            return NULL;
          }
     
          // Free up the old response packet.
          dsm_free_packet(&response_ptr);

          /*-----------------------------------------------------------
            Calculate the length of new response packet.
          -----------------------------------------------------------*/
          *response_len =  PPP_AUTH_CODE_SIZE
                          + PPP_AUTH_ID_SIZE
                          + PPP_AUTH_LENGTH_SIZE
                          + PPP_CHAP_VALUE_SIZE
                          + PPP_CHAP_MD5_SIZE
                          + ppp_cb_ptr->auth.info.user_id_len;
          
          // Allocate new buffer based on new response length.
          response_ptr = dsm_offset_new_buffer(DSM_DS_POOL_SIZE(
                                                *response_len + PPP_HDR_LEN),
                                               PPP_HDR_LEN);
          if(response_ptr == NULL || response_ptr->data_ptr == NULL)
          {
            LOG_MSG_ERROR_1("dev %d CHAP resp mem alloc failed",
                            ppp_cb_ptr->fsm[LCP].device);
            return NULL;
          }

          response_ptr->used     = (uint16)*response_len;
          response_ptr->priority = DSM_HIGHEST;
          DSM_TOUCH_ITEM(response_ptr);
          continue;
        }
      }
      else
      {
        return NULL;
      }
    }
  }while(1);

  /*--------------------------------------------------------------------------
    Finalize MD5 hash, providing the CHAP response and insert
  --------------------------------------------------------------------------*/
  insert_ptr += PPP_CHAP_MD5_SIZE; /* move ptr forward by hash length */
  return insert_ptr;
} /* chapi_calc_chap_md5_ruim_response() */


/*===========================================================================
FUNCTION CHAPI_CALC_AN_HRPD_MD5_RUIM_RESPONSE()

DESCRIPTION
  Generates the 16-byte Hash/CHAP HRPD Response by sending a command to the
  RUIM with the chap challenge and profile # as input.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static byte * chapi_calc_an_hrpd_md5_ruim_response
(
  ppp_type *       ppp_cb_ptr,
  dsm_item_type *  response_ptr,
  int              response_len 
)
{
  byte* insert_ptr;
  uint8 msg_id = 0;
  mmgsdi_compute_ip_data_type        cmpt_ip_data;
  mmgsdi_callback_type               response_cb_ptr = NULL;
  mmgsdi_return_enum_type            retval;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == ppp_cb_ptr )
  {
    LOG_MSG_ERROR_0("chapi_calc_an_hrpd_md5_ruim_response():"
                    " ppp_cb_ptr is NULL");
    return NULL;
  }

  if(NULL == response_ptr || NULL == response_ptr->data_ptr)
  {
    LOG_MSG_ERROR_0("chapi_calc_an_hrpd_md5_ruim_response():"
                    " response_ptr or response_ptr->data_ptr is NULL");
    return NULL;
  }

  if(NULL == ppp_cb_ptr->auth.chap_challenge_ptr)
  {
    LOG_MSG_ERROR_0("chapi_calc_an_hrpd_md5_ruim_response():"
                    " auth.chap_challenge_ptr is NULL");
    return NULL;
  }
  /* The RUIM card can handle CHAP Challenges up to 254 bytes in length */
  if( ppp_cb_ptr->auth.chap_challenge_ptr->used > 
      MMGSDI_MAX_HRPD_CHAP_CHALLENGE_LENGTH )
  {
    LOG_MSG_ERROR_1("chapi_calc_an_hrpd_md5_ruim_response : "
              "CHAP Challenge is greater than %d for AN R-UIM",
                    MMGSDI_MAX_HRPD_CHAP_CHALLENGE_LENGTH);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Check whether a session has already been registered with mmgsdi module
  -------------------------------------------------------------------------*/
  if (FALSE == ppp_mmgsdi_auth_config.reg_info.session_info.session_active)
  {
    LOG_MSG_ERROR_0("chapi_calc_an_hrpd_md5_ruim_response:"
                    " PPP Client not registered");
       return NULL;
  }
  msg_id = ppp_cb_ptr->auth.active_id;

  /*------------------------------------------------------------------------
    Build the CHAP response in the provided DSM item
  ------------------------------------------------------------------------*/
  insert_ptr    = response_ptr->data_ptr;
  *insert_ptr++ = PPP_CHAP_RESPONSE;
  *insert_ptr++ = msg_id;
  insert_ptr    = put16(insert_ptr,(uint16)response_len);

  if (NULL == insert_ptr)
  {
    LOG_MSG_ERROR_0("chapi_calc_an_hrpd_md5_ruim_response():"
                    "NULL ptr returned by put16");
    return NULL;
  }

  *insert_ptr++ = PPP_CHAP_MD5_SIZE;
  cmpt_ip_data.operation_type = MMGSDI_CMPT_IP_HRPD_CHAP;
  cmpt_ip_data.data.hrpd_chap_data.chap_id = msg_id;
  cmpt_ip_data.data.hrpd_chap_data.chap_challenge_length = 
                             ppp_cb_ptr->auth.chap_challenge_ptr->used;
  cmpt_ip_data.data.hrpd_chap_data.chap_challenge_ptr = 
                         ppp_cb_ptr->auth.chap_challenge_ptr->data_ptr;
  response_cb_ptr = chapi_uim_calc_chap_resp_rpt;
  /*--------------------------------------------------------------------------
      Send the command to the R-UIM:
      Clear the "command done signal"
      Call the relevant mmgsdi COMPUTE AN IP AUTH function
      Wait for the command to be done
  --------------------------------------------------------------------------*/
  PS_CLR_SIGNAL(PS_UIM_CMD_SIGNAL);

  retval = mmgsdi_session_compute_ip_auth ( 
                     ppp_mmgsdi_auth_config.reg_info.session_info.session_id,
                     cmpt_ip_data,
                     response_cb_ptr,
                     (mmgsdi_client_data_type) 0);

  if(MMGSDI_INCORRECT_PARAMS == retval) 
  {
    DATA_ERR_FATAL("chapi_calc_an_hrpd_md5_ruim_response: Incorrect Paramter values supplied to mmgsdi_compute_ip_auth algo");
    return NULL;
  }
  else if (MMGSDI_CMD_QUEUE_FULL == retval)
  {
    LOG_MSG_INFO2_0("chapi_calc_an_hrpd_md5_ruim_response:"
            " Command Queue Full"
                    " when trying to run mmgsdi_compute_ip_auth algo");
    return NULL;
  }
  LOG_MSG_INFO3_0("Blocking PS Task on UIM cmd signal");
  (void)ps_wait((rex_sigs_type) 1 << PS_UIM_CMD_SIGNAL); /* Wait for completion*/
  LOG_MSG_INFO3_0("Unblocking PS TASK on UIM cmd signal");
  if(ppp_uim_auth.report_status == UIM_PASS)
  {
    memscpy(insert_ptr,
            response_ptr->data_ptr + response_ptr->used - insert_ptr,
            &ppp_uim_auth.challenge_response,
            UIM_COMPUTE_IP_AUTH_RESPONSE_LENGTH);
  }
  else
  {
    LOG_MSG_INFO1_0("UIM-HRPD FAILURE");
    return NULL;
  }

  /*--------------------------------------------------------------------------
    Finalize MD5 hash, providing the HRPD CHAP response and insert
  --------------------------------------------------------------------------*/
  insert_ptr += PPP_CHAP_MD5_SIZE; /* move ptr forward by hash length */
  return insert_ptr;
} /* chapi_calc_an_hrpd_md5_ruim_response() */


#ifdef FEATURE_UIM_CAVE_AN_AUTH
/*===========================================================================
FUNCTION CHAPI_CALC_CHAP_CAVE_RUIM_RESPONSE()

DESCRIPTION
  Generates the 16-byte Hash/CHAP Response by sending a command to the
  RUIM to do CAVE with the chap challenge and profile # as input.  Format the
  output appropriately as well.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static byte * chapi_calc_chap_cave_ruim_response
(
  ppp_type *       ppp_cb_ptr,
  dsm_item_type *  response_ptr,
  int              response_len
)
{
  byte* insert_ptr;
  uint8 msg_id = 0;
  mmgsdi_cave_rand_chal_type         rand_chal;
  mmgsdi_callback_type               response_cb_ptr;
  mmgsdi_return_enum_type            retval;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == ppp_cb_ptr )
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_cave_ruim_response():"
                    " ppp_cb_ptr is NULL");
    return NULL;
  }

  if(NULL == response_ptr || NULL == response_ptr->data_ptr)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_cave_ruim_response():"
                    " response_ptr or response_ptr->data_ptr is NULL");
    return NULL;
  }

  if(NULL == ppp_cb_ptr->auth.chap_challenge_ptr)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_cave_ruim_response():"
                    " auth.chap_challenge_ptr is NULL");
    return NULL;
  }
  
#if defined(FEATURE_RUIM_FUNC_INTERFACE) && defined(FEATURE_MMGSDI_SESSION_LIB)
  /*------------------------------------------------------------------------
    Check whether a session has already been registered with mmgsdi module
  ------------------------------------------------------------------------*/
  if (FALSE == ppp_mmgsdi_auth_config.reg_info.session_info.session_active)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_cave_ruim_response:"
                    " PPP Client not registered");
       return NULL;
  }  
#endif /* FEATURE_RUIM_FUNC_INTERFACE && FEATURE_MMGSDI_SESSION_LIB */

  msg_id = ppp_cb_ptr->auth.active_id;
  /*------------------------------------------------------------------------
    Build the CHAP response in the provided DSM item
  ------------------------------------------------------------------------*/
  insert_ptr    = response_ptr->data_ptr;
  *insert_ptr++ = PPP_CHAP_RESPONSE;
  *insert_ptr++ = msg_id;
  insert_ptr    = put16(insert_ptr,(uint16)response_len);

  if (NULL == insert_ptr)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_cave_ruim_response():"
                    "NULL ptr returned by put16");
    return NULL;
  }
  *insert_ptr++ = PPP_CHAP_MD5_SIZE;

  /*------------------------------------------------------------------------
      Build the UIM command to perform CAVE algorithm.
  ------------------------------------------------------------------------*/
  rand_chal = (mmgsdi_cave_rand_chal_type) 0;
  if (ppp_cb_ptr->auth.chap_challenge_ptr->used >= 
                                        PPP_FIRST_32_BITS_USED_FOR_AN_CAVE)
  {
    rand_chal = (mmgsdi_cave_rand_chal_type)
      b_unpackd(ppp_cb_ptr->auth.chap_challenge_ptr->data_ptr,0,32);
  }
  else
  {
    rand_chal = (mmgsdi_cave_rand_chal_type)
      b_unpackd(ppp_cb_ptr->auth.chap_challenge_ptr->data_ptr,0,
                ppp_cb_ptr->auth.chap_challenge_ptr->used*8);
  }

  response_cb_ptr = chapi_uim_an_cave_resp_rpt; 
  /*--------------------------------------------------------------------------
      Send the command to the R-UIM:
      Clear the "command done signal"
      Call the relevant mmgsdi RUN CAVE function
      Wait for the command to be done
  --------------------------------------------------------------------------*/
  PS_CLR_SIGNAL(PS_UIM_CMD_SIGNAL);

  retval = mmgsdi_session_run_cave_algo ( 
                     ppp_mmgsdi_auth_config.reg_info.session_info.session_id,
                     ppp_mmgsdi_auth_config.rand_type,
                     rand_chal,
                     ppp_mmgsdi_auth_config.dig_len,
                     ppp_mmgsdi_auth_config.digits,
                     ppp_mmgsdi_auth_config.process_control,
                     ppp_mmgsdi_auth_config.esn,
                     response_cb_ptr,
                     (mmgsdi_client_data_type) 0);

  if(MMGSDI_INCORRECT_PARAMS == retval) 
  {
    DATA_ERR_FATAL("chapi_calc_chap_cave_ruim_response: Incorrect Paramter values supplied to mmgsdi_run_cave_algo");
    return NULL;
  }
  else if (MMGSDI_CMD_QUEUE_FULL == retval)
  {
    LOG_MSG_ERROR_0("chapi_calc_chap_cave_ruim_response: Command Queue Full"
                    " wen trying to run mmgsdi_run_cave_algo");
    return NULL;
  }
  LOG_MSG_INFO3_0("Blocking PS Task on UIM cmd signal");
  (void)ps_wait((rex_sigs_type) 1 << PS_UIM_CMD_SIGNAL); /* Wait for completion*/
  LOG_MSG_INFO3_0("Unblocking PS TASK on UIM cmd signal");

  /*------------------------------------------------------------------------
     RUN CAVE command returned with the callback function getting called
  ------------------------------------------------------------------------*/

  if(ppp_uim_auth.report_status == UIM_PASS)
  {
    /* Place Zero padding in CHAP Response place */
    memset(insert_ptr,
           PPP_CHAP_RESPONSE_PADDING,
           PPP_CHAP_MD5_SIZE);
    /* Put first 18 bits of cave data into first 18 bits of response */
    *insert_ptr++ = ppp_uim_auth.challenge_response[0];
    *insert_ptr++ = ppp_uim_auth.challenge_response[1];
    *insert_ptr++ = (ppp_uim_auth.challenge_response[2] & 0xC0);
    insert_ptr += PPP_CHAP_MD5_SIZE - 3;
  }
  else
  {
    LOG_MSG_INFO1_0("UIM-CAVE FAILURE");
    return NULL;
  }

  return insert_ptr;

}
#endif /* FEATURE_UIM_CAVE_AN_AUTH */



/*===========================================================================
FUNCTION CHAPI_SEND_RESP()

DESCRIPTION
  Builds and transfers the CHAP packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void chapi_send_resp
(
  ppp_type *ppp_cb_ptr
)
{
  dsm_item_type *response_ptr;
  byte *insert_ptr;
  int response_len;
#ifdef FEATURE_HDR_AN_AUTH
  ppp_type *ppp_cb_array = NULL;
#endif /* FEATURE_HDR_AN_AUTH */
  nv_item_type* ps_cave_nai_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check if the user_id or secret lengths are 0. If yes, then return without
    transmitting any data - assume the peer will send another challenge or
    tear down the link.  Free the chap challenge packet.  If HRPD, CAVE, or 
    3GPD is being used on the R-UIM, then the password is not stored in the
    ME, and this value could be zero.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->auth.info.user_id_len == 0)
  {
    LOG_MSG_INFO1_1("dev %d empty username", ppp_cb_ptr->fsm[LCP].device);
    dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    return;
  }

#ifdef FEATURE_HDR_AN_AUTH
  ppp_cb_array = fsm_get_ppp_cb_array();
  /*------------------------------------------------------------------------
    The following check is for AN calls.  If AN HRPD is supported in the 
    R-UIM, allow a zero password 
    -----------------------------------------------------------------------*/
  if((ppp_cb_ptr->auth.info.passwd_len == 0)
     && (ppp_cb_ptr == &ppp_cb_array[PPP_UM_AN_DEV])
      && (UIM_AN_HRPD_NO_SUPPORT == (uim_an_hrpd_support_status)uim_an_hrpd_support())
  )
  {
    LOG_MSG_INFO1_1("dev %d empty secret", ppp_cb_ptr->fsm[LCP].device);
    dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    return;
  }
#endif /* FEATURE_HDR_AN_AUTH */

  /*------------------------------------------------------------------------
    The following check is for SN calls.  If Simple IP is supported in the 
    R-UIM, allow a zero password 
    -----------------------------------------------------------------------*/
  if((ppp_cb_ptr->auth.info.passwd_len == 0)
#ifdef FEATURE_HDR_AN_AUTH
      && (ppp_cb_ptr != &ppp_cb_array[PPP_UM_AN_DEV])
#endif /* FEATURE_HDR_AN_AUTH */
      && (UIM_3GPD_MIP_NV_SIP_NV == (uim_3gpd_support_status)uim_3gpd_support() ||
       UIM_3GPD_MIP_RUIM_SIP_NV == (uim_3gpd_support_status)uim_3gpd_support())
  )
  {
    LOG_MSG_INFO1_1("dev %d empty secret", ppp_cb_ptr->fsm[LCP].device);
    dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    return;
  }

  /*-------------------------------------------------------------------------
    Calculate the length of the response packet.
  -------------------------------------------------------------------------*/
  response_len =  PPP_AUTH_CODE_SIZE
    + PPP_AUTH_ID_SIZE
    + PPP_AUTH_LENGTH_SIZE
    + PPP_CHAP_VALUE_SIZE
    + PPP_CHAP_MD5_SIZE
    + ppp_cb_ptr->auth.info.user_id_len;

  /*-------------------------------------------------------------------------
    Get a dsm_item to store the response that will be generated. If there is
    no memory to allocate, then return to inform that allocation failed.
    Free the challenge pointer.  Assume we will be rechallenged or the peer
    will terminate the connection.
  -------------------------------------------------------------------------*/
  response_ptr =
    dsm_offset_new_buffer(DSM_DS_POOL_SIZE(response_len + PPP_HDR_LEN),
                          PPP_HDR_LEN);
  if(response_ptr == NULL)
  {
    LOG_MSG_ERROR_1("dev %d CHAP resp mem alloc failed",
                    ppp_cb_ptr->fsm[LCP].device);
    dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    return;
  }

  /*-------------------------------------------------------------------------
    Fill in the following fields in the dsm item:
        used field with the total response length;
        Priority as Highest.
  -------------------------------------------------------------------------*/
  response_ptr->used     = (uint16)response_len;
  response_ptr->priority = DSM_HIGHEST;
  DSM_TOUCH_ITEM(response_ptr);

  /*-------------------------------------------------------------------------
    Check if password is configured by client. If set, then calculate CHAP
    response through software else by card.
    Call the appropriate function to calculate the CHAP response
  -------------------------------------------------------------------------*/
  if((UIM_3GPD_MIP_RUIM_SIP_RUIM == (uim_3gpd_support_status)uim_3gpd_support() ||
      UIM_3GPD_MIP_NV_SIP_RUIM == (uim_3gpd_support_status)uim_3gpd_support())  &&
     (ppp_cb_ptr->auth.info.is_pwd_set_by_client == FALSE)
#ifdef FEATURE_HDR_AN_AUTH
      && ( ppp_cb_ptr != &ppp_cb_array[PPP_UM_AN_DEV] )
#endif /* FEATURE_HDR_AN_AUTH */
    )
  {
    /*-----------------------------------------------------------------------
      If using RUIM, go to the RUIM to get the CHAP response
      This DOES NOT CHECK if it an SN Stream, only that it is NOT an AN Stream.
      If Streams other than SN and AN begin to seek authentication functionality
      through chapi_send_resp, then some additional checks might be needed 
      here in the future
    -----------------------------------------------------------------------*/
    insert_ptr = chapi_calc_chap_md5_ruim_response( ppp_cb_ptr,
                                                    response_ptr,
                                                    &response_len);
  }
  else
  if ( UIM_AN_HRPD_SUPPORT == (uim_an_hrpd_support_status)uim_an_hrpd_support()
#ifdef FEATURE_HDR_AN_AUTH
       && ppp_cb_ptr == &ppp_cb_array[PPP_UM_AN_DEV]
#endif /* FEATURE_HDR_AN_AUTH */
  )
  {
    /*-----------------------------------------------------------------------
       Use HRPD AN Authentication on Mobile
    -----------------------------------------------------------------------*/
    insert_ptr = chapi_calc_an_hrpd_md5_ruim_response( ppp_cb_ptr,
                                                     response_ptr,
                                                     response_len);
    if (NULL == insert_ptr)
    {
      LOG_MSG_INFO1_0("cwc debug: Run MD5 fail, fallback to CAVE");

      /*-----------------------------------------------------------------------
         Notify RUIM about MD5 failure and read the NV item to get cave
         user ID (NAI)
      -----------------------------------------------------------------------*/
      nvruim_ppp_cave_fallback();

      if (UIM_AN_HRPD_USE_CAVE == (uim_an_hrpd_support_status)uim_an_hrpd_support())
      {      
        ps_cave_nai_ptr = (nv_item_type*)ps_system_heap_mem_alloc(sizeof(nv_item_type));
        if(NULL == ps_cave_nai_ptr)
        {
          LOG_MSG_ERROR_0("chapi_send_resp: Allocation of ps_cave_nai failed");
          dsm_free_packet(&response_ptr);
          dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
          return;
        }
        if ( NV_DONE_S == ps_get_nv_item(NV_HDR_AN_AUTH_NAI_I, ps_cave_nai_ptr))
        {
          if ((NV_MAX_AN_AUTH_NAI_LENGTH 
               < ps_cave_nai_ptr->hdr_an_auth_nai.nai_len)
              || (ps_cave_nai_ptr->hdr_an_auth_nai.nai_len == 0))
          {
            LOG_MSG_ERROR_0("chapi_send_resp: Invalid NV item (CAVE user ID)");
          }

          else
          {
            ppp_cb_ptr->auth.info.user_id_len = ps_cave_nai_ptr->hdr_an_auth_nai.nai_len;
            memscpy (ppp_cb_ptr->auth.info.user_id_info, 
                     PPP_MAX_USER_ID_LEN,
                     (char*)ps_cave_nai_ptr->hdr_an_auth_nai.nai,
                     ppp_cb_ptr->auth.info.user_id_len);

            insert_ptr = chapi_calc_chap_cave_ruim_response( ppp_cb_ptr,
                                                             response_ptr,
                                                             response_len);   
            if (NULL != insert_ptr)
            {  
              LOG_MSG_INFO1_0("cwc debug: fallback to CAVE success"); 
            }

            else
            {  
               LOG_MSG_INFO1_0("cwc debug: fallback to CAVE fail"); 
            }
          }
        }
        else
        {
          LOG_MSG_ERROR_0("chapi_send_resp: Could not read NV item to get CAVE user"
                          " ID");
        } 
        PS_SYSTEM_HEAP_MEM_FREE(ps_cave_nai_ptr);
      }
      else
      {
        LOG_MSG_INFO1_0("cwc debug: MD5 failed and Fallback to Cave is not allowed");
      }        
    }
    else
    {
      LOG_MSG_INFO1_0("cwc debug: MD5 return OK, go with MD5");
    }
  }
  else
#ifdef FEATURE_UIM_CAVE_AN_AUTH
  if( UIM_AN_HRPD_USE_CAVE == (uim_an_hrpd_support_status)uim_an_hrpd_support()
#ifdef FEATURE_HDR_AN_AUTH
      &&  ppp_cb_ptr == &ppp_cb_array[PPP_UM_AN_DEV]
#endif /* FEATURE_HDR_AN_AUTH */
  )
  {
    /*-----------------------------------------------------------------------
      If network uses CAVE for AN authentication, use CAVE on the RUIM
    -----------------------------------------------------------------------*/
    insert_ptr = chapi_calc_chap_cave_ruim_response( ppp_cb_ptr,
                                                     response_ptr,
                                                     response_len);
    LOG_MSG_INFO1_0("cwc debug: run CAVE");
  }
  else
#endif /* FEATURE_UIM_CAVE_AN_AUTH */
  {
    /*-----------------------------------------------------------------------
      Use the software MD-5 algorithm to calculate CHAP response
    -----------------------------------------------------------------------*/
    insert_ptr = chapi_calc_chap_md5_sw_response( ppp_cb_ptr,
                                                  response_ptr,
                                                  response_len );
  }

  /*-------------------------------------------------------------------------
    Check if the calculation failed.  If so, do the right thing.
  -------------------------------------------------------------------------*/
  if (insert_ptr == NULL)
  {
    LOG_MSG_ERROR_1("dev %d CHAP resp calculation failed",
                    ppp_cb_ptr->fsm[LCP].device);
    if(response_ptr != NULL)
    {
      dsm_free_packet(&response_ptr);
    }
    dsm_free_packet(&(ppp_cb_ptr->auth.chap_challenge_ptr));
    return;
  }

  /*-------------------------------------------------------------------------
    Copy the user_id into the response packet.
  -------------------------------------------------------------------------*/
  memscpy(insert_ptr,
          response_ptr->data_ptr + response_ptr->used - insert_ptr,
          ppp_cb_ptr->auth.info.user_id_info,
          ppp_cb_ptr->auth.info.user_id_len);

  PPP_INC_INSTANCE_STATS_PTR(ppp_cb_ptr, chap_pkts_tx, 1);
  /*-------------------------------------------------------------------------
    we are done - so transmit the packet
  -------------------------------------------------------------------------*/
  if(pppi_outgoing(ppp_cb_ptr->fsm[LCP].device,
                   PPP_CHAP_PROTOCOL,
                   &response_ptr,
                   NULL) == -1)
  {
    /*---------------------------------------------------------------------
      pppi_outgoing() frees the packet in case of an error - let the timer
      to trigger another attempt.
    ---------------------------------------------------------------------*/
    LOG_MSG_INFO1_1("dev %d sending CHAP reponse failed!",
                    ppp_cb_ptr->fsm[LCP].device);
  }

  /*-------------------------------------------------------------------------
    Start the retry timer.
    NOTE: we use the LCP timer as authentication doesn't have it's own timer.
  --------------------------------------------------------------------------*/
  (void)ps_timer_start(ppp_cb_ptr->fsm[LCP].timer,
                       (int64)ppp_cb_ptr->auth.timeout);

} /* chapi_send_resp() */



/*===========================================================================
FUNCTION CHAPI_SEND_CHALLENGE()

DESCRIPTION
  This function builds a chap challenge and sends it out.

PARAMETERS
  ppp_cb_ptr: ppp control block

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  if there isn't enough memory then PPP will be closed.
===========================================================================*/
static void chapi_send_challenge
(
  ppp_type *ppp_cb_ptr
)
{
  dsm_item_type *chal_ptr = NULL;
  uint8 *insert_ptr = NULL;
  ppp_auth_info_type *auth_info_ptr;
  uint32 random_value;
  int chal_msg_len;
  char *ppp_auth = NULL;
  char *ppp_end = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == ppp_cb_ptr )
  {
    DATA_ERR_FATAL("NULL ppp_cb_ptr passed");
    return;
  }

  auth_info_ptr = &(ppp_cb_ptr->auth.info);
  /*-------------------------------------------------------------------------
    calculate the total message length.
  -------------------------------------------------------------------------*/
  chal_msg_len = PPP_AUTH_CODE_SIZE
    + PPP_AUTH_ID_SIZE
    + PPP_AUTH_LENGTH_SIZE
    + PPP_CHAP_VALUE_SIZE
    + PPP_CHAP_CHAL_LEN
    + auth_info_ptr->challenge_name_len;

  /*-------------------------------------------------------------------------
    get a dsm item to build the challenge packet in.
  -------------------------------------------------------------------------*/
  chal_ptr =
    dsm_offset_new_buffer(DSM_DS_POOL_SIZE(chal_msg_len + PPP_HDR_LEN),
                          PPP_HDR_LEN);

  /*-------------------------------------------------------------------------
    if the allocation  fails skip forward to the timer - this will trigger
    another attempt.
  -------------------------------------------------------------------------*/
  if(NULL == chal_ptr || NULL == chal_ptr->data_ptr)
  {
    LOG_MSG_ERROR_1("dev %d CHAP challenge mem alloc failed",
                    ppp_cb_ptr->fsm[LCP].device);
    goto give_up;
  }

  /*-------------------------------------------------------------------------
    Fill in the following fields in the dsm item:
        used field with the total response length;
        Priority as Highest.
  -------------------------------------------------------------------------*/
  chal_ptr->used     = (uint16)chal_msg_len;
  chal_ptr->priority = DSM_HIGHEST;
  DSM_TOUCH_ITEM(chal_ptr);

  /*-------------------------------------------------------------------------
    Chap challenge looks like the following:

     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     | 1 (the value) |  Identifier   |            Length             |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |  chal len     |  challenge
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |  MS name
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   The challenge length will always be 16 bytes long.  Fill in all of the
   fields preceeding the challenge.
  -------------------------------------------------------------------------*/
  insert_ptr    = chal_ptr->data_ptr;
  *insert_ptr++ = PPP_CHAP_CHALLENGE;               /* code                */
  *insert_ptr++ = ++ppp_cb_ptr->auth.active_id;     /* id field            */
  insert_ptr    = put16(insert_ptr,
                        (uint16)chal_msg_len);      /* total length of pkt */

  if (insert_ptr == NULL)
  {
    LOG_MSG_ERROR_0("chapi_send_challenge():"
                    "NULL ptr returned by put16");
    goto give_up;
  }
  *insert_ptr++ = PPP_CHAP_CHAL_LEN;                /* challenge length    */

  /*-------------------------------------------------------------------------
    Build the challenge in the auth info field: first set the length to the
    length we are filling - then loop over the entire size of the challenge
    area writting the 4 bytes we get from ran_next(). Then copy it into the
    challenge packet.
  -------------------------------------------------------------------------*/
  auth_info_ptr->challenge_len = PPP_CHAP_CHAL_LEN;
  ppp_auth = (char *)auth_info_ptr->challenge_info;
  ppp_end = (char *)(ppp_auth + PPP_CHAP_CHAL_LEN);

  do
  {
    (void)ps_utils_generate_rand_num( (void*)&random_value,
                                      sizeof(random_value) );
    memscpy(ppp_auth, PPP_CHAP_CHAL_LEN, &random_value, sizeof(random_value));
    ppp_auth += sizeof(random_value);
  }while (ppp_auth < ppp_end);

  memscpy(insert_ptr, 
          chal_ptr->data_ptr + chal_ptr->used - insert_ptr,
          auth_info_ptr->challenge_info, 
          PPP_CHAP_CHAL_LEN);
  insert_ptr += PPP_CHAP_CHAL_LEN;

  /*-------------------------------------------------------------------------
    copy the challenge name and increment the used field appropriately
  -------------------------------------------------------------------------*/
  memscpy(insert_ptr,
          chal_ptr->data_ptr + chal_ptr->used - insert_ptr,
         auth_info_ptr->challenge_name,
         auth_info_ptr->challenge_name_len);
  insert_ptr += auth_info_ptr->challenge_name_len;

  PPP_INC_INSTANCE_STATS_PTR(ppp_cb_ptr, chap_pkts_tx, 1);
  /*-------------------------------------------------------------------------
    we are done - so transmit the packet
  -------------------------------------------------------------------------*/
  if(pppi_outgoing(ppp_cb_ptr->fsm[LCP].device,
                   PPP_CHAP_PROTOCOL,
                   &chal_ptr,
                   NULL) == -1)
  {
    /*---------------------------------------------------------------------
      pppi_outgoing() frees the packet in case of an error - let the timer
      to trigger another attempt.
    ---------------------------------------------------------------------*/
    LOG_MSG_INFO1_1("dev %d sending CHAP challenge failed!",
                    ppp_cb_ptr->fsm[LCP].device);
  }

  /*-------------------------------------------------------------------------
    Start the retry timer.
    NOTE: we use the LCP timer as authentication doesn't have it's own timer.
  --------------------------------------------------------------------------*/
 give_up:
  (void)ps_timer_start(ppp_cb_ptr->fsm[LCP].timer,
                       (int64)ppp_cb_ptr->auth.timeout);

} /* chapi_send_challenge() */

#ifdef FEATURE_DATA_PS_PPP_EAP
/*===========================================================================
FUNCTION PPP_AUTH_EAP_INIT()

DESCRIPTION
  This function initializes the EAP structures and updates the EAP handle.

PARAMETERS
  PPP device

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS 
  None  
===========================================================================*/
void ppp_auth_eap_init
(
    ppp_dev_enum_type device
)
{
  ppp_type *ppp_cb_ptr = NULL;
  ppp_type *ppp_cb_array = NULL;
  eap_info_type ppp_eap_info;
  ppp_fsm_type *fsm_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( PPP_MAX_DEV <= device)
  {
    LOG_MSG_ERROR_1( "Invalid argument passed: device %d", device );
    ASSERT( 0 );
    return;
  }
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + device;

  /*-------------------------------------------------------------------------
    Create an EAP instance only if the EAP Handle is NULL.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->auth.ps_eap_handle != EAP_INVALID_HANDLE)
  {
    LOG_MSG_INFO2_0("EAP Handle already present");
    return;
  }

  eap_aka_init();
  eap_aka_prime_init();

  /*-------------------------------------------------------------------------
    Initialize the ppp_eap_info structure
  -------------------------------------------------------------------------*/
  memset((void*)(&ppp_eap_info), 0, sizeof(eap_info_type));
  ppp_eap_info.authenticator_rsp_required = TRUE;
  ppp_eap_info.eap_auth_prot_mask = 1 << EAP_AKA_PRIME_METHOD;
  ppp_eap_info.raw_mode = FALSE;

  ppp_eap_info.result_ind_f_ptr = ppp_eap_result_cb;

  ppp_eap_info.result_ind_user_data = ppp_cb_ptr;    
  ppp_eap_info.trp_tx_f_ptr= ppp_eap_tx_cb ;
  ppp_eap_info.trp_tx_user_data = ppp_cb_ptr;

  /*-------------------------------------------------------------------------
    Populate the EAP user ID 
  -------------------------------------------------------------------------*/
  ppp_eap_info.user_id.len= ppp_cb_ptr->auth.info.eap_user_id_len;
  memscpy(ppp_eap_info.user_id.name,
          EAP_IDENTITY_MAX_CHAR,
          ppp_cb_ptr->auth.info.eap_user_id,
          ppp_eap_info.user_id.len);    

  /*-------------------------------------------------------------------------
    Get eap handle. If the EAP handle is invalid then need to call ncp_stop.
    If EAP is used for non-eHRPD cases then the corresponding stop function
    i.e. ppp_stop will need to be called.
  -------------------------------------------------------------------------*/

  ppp_cb_ptr->auth.ps_eap_handle = eap_create_instance(&ppp_eap_info);
  if( EAP_INVALID_HANDLE == ppp_cb_ptr->auth.ps_eap_handle )
  {    
    LOG_MSG_ERROR_0("Failed to get an EAP instance, Closing LCP");

    fsm_ptr = &ppp_cb_array->fsm[LCP];
    fsm_close(fsm_ptr);
    if(fsm_ptr->state != fsmCLOSING )
    {
      fsm_ptr->mode = PPP_CP_MODE_INVALID;
    }

    return;
  }

  /*-------------------------------------------------------------------------
    Populate ppp_eap_meta_info
  -------------------------------------------------------------------------*/
  memset((void *)(&ppp_eap_meta_info), 0, sizeof(ppp_eap_meta_info));

  if( ( FALSE == ppp_cb_ptr->auth.info.ehrpd_usim_present ) && 
      ( ppp_cb_ptr->auth.info.aka_algo_seqnum_arr_num_elements !=
       EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS ) )
  {
    LOG_MSG_ERROR_1("Invalid aka_algo_seqnum_arr_num_elements %d",
                    ppp_cb_ptr->auth.info.aka_algo_seqnum_arr_num_elements );
    ASSERT(0);
    return;
  }

  ppp_eap_meta_info.id.provided_id_len = ppp_cb_ptr->auth.info.eap_user_id_len;
  memscpy(ppp_eap_meta_info.id.provided_id,
          EAP_SIM_AKA_MAX_ID_LEN,
          ppp_cb_ptr->auth.info.eap_user_id,
          ppp_eap_meta_info.id.provided_id_len);

  ppp_eap_meta_info.notification_rcv_cb_fct = ppp_eap_notification_cb;

  /*-------------------------------------------------------------------------
    task_srv_fct set to NULL since in PS task itself
  -------------------------------------------------------------------------*/
  ppp_eap_meta_info.task_srv_fct = NULL;

  /*-------------------------------------------------------------------------
    Populate the sequence number details
  -------------------------------------------------------------------------*/
  ppp_eap_meta_info.aka_algo_seqnum_arr_num_elements = 
                       ppp_cb_ptr->auth.info.aka_algo_seqnum_arr_num_elements;
  memscpy(ppp_eap_meta_info.aka_algo_seqnum_arr,
          EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS,
          ppp_cb_ptr->auth.info.aka_algo_seqnum_arr,
          ppp_cb_ptr->auth.info.aka_algo_seqnum_arr_num_elements*(sizeof(uint64)));

  /*-------------------------------------------------------------------------
    whether to do EAP-AKA in software or on card
  -------------------------------------------------------------------------*/
  if (FALSE == ppp_cb_ptr->auth.info.ehrpd_usim_present)
  {
    ppp_eap_meta_info.eap_aka_in_sw = TRUE;  
  }

  else
  {
#if !defined(FEATURE_MMGSDI_SESSION_LIB)
    LOG_MSG_ERROR_0("FEATURE_MMGSDI_SESSION_LIB not defined");
    return;
#else
    ppp_eap_meta_info.task_srv_fct = ppp_auth_eap_task_srv_fct;
#endif /* FEATURE_MMGSDI_SESSION_LIB */
  }

  /*-------------------------------------------------------------------------
    Populate the AKA ALGO specific variables only if the AKA
    ALGORITHM is set appropriately
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->auth.info.aka_algo_type < EAP_SIM_AKA_ALGO_MAX )
  {
    ppp_eap_meta_info.sim_aka_algo_type = ppp_cb_ptr->auth.info.aka_algo_type; 
    if(ppp_cb_ptr->auth.info.aka_algo_type == EAP_AKA_ALGO_MILENAGE)
    {
      ppp_eap_meta_info.aka_algo_milenage_op_data_len = 
                     ppp_cb_ptr->auth.info.aka_algo_milenage_op_data_len;

      memscpy( ppp_eap_meta_info.aka_algo_milenage_op_data,
               EAP_AKA_ALGO_MILENAGE_OP_LEN,
               ppp_cb_ptr->auth.info.aka_algo_milenage_op_data,
               ppp_cb_ptr->auth.info.aka_algo_milenage_op_data_len );
    }
  }

  /*-------------------------------------------------------------------------
    Populate the EAP shared secret
  -------------------------------------------------------------------------*/
  memscpy(ppp_eap_meta_info.eap_shared_secret,
          EAP_SHARED_SECRET_LEN,
          ppp_cb_ptr->auth.info.eap_shared_secret,
          PPP_EAP_SHARED_SECRET_LEN);        
  
  /*-------------------------------------------------------------------------
    PS is a new_client therefore set to true
  -------------------------------------------------------------------------*/
  ppp_eap_meta_info.new_client = TRUE;

  eap_set_meta_info( ppp_cb_ptr->auth.ps_eap_handle, 
                    (void *)(&ppp_eap_meta_info));

  return;
}/*ps_ppp_eap_init*/


#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
  FUNCTION PPP_AUTH_PROCESS_EAP_SIM_AKA_TASK_SWITCH()

  DESCRIPTION
    Process the task switch req

  PARAMETERS
    cmd_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void ppp_auth_process_eap_sim_aka_task_switch
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (PS_PPP_EAP_TASK_CMD != cmd)
  {
    LOG_MSG_ERROR_1("Invalid cmd %d", cmd);
    return;
  }

  ppp_task_info.req_cb (ppp_task_info.user_data);
} /* qmi_auth_process_eap_sim_aka_task_switch */


/*===========================================================================
  FUNCTION PPP_AUTH_EAP_TASK_SRV_FCT()

  DESCRIPTION
    Cback function registered with EAP SIM/AKA for task switch

  PARAMETERS
    req_cb - cback to be called after switching tasks
    user_data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean ppp_auth_eap_task_srv_fct
(
  eap_sim_aka_task_srv_req_cb_type req_cb,
  void * user_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_task_info.req_cb = req_cb;
  ppp_task_info.user_data = user_data;

  ps_send_cmd(PS_PPP_EAP_TASK_CMD, NULL);

  return TRUE;
} /* qmi_auth_eap_task_srv_fct() */
#endif /* FEATURE_MMGSDI_SESSION_LIB */


/*===========================================================================
FUNCTION PPP_EAP_REULT_CB()

DESCRIPTION
  This is the callback that EAP will call after authentication.

PARAMETERS
  eap_handle_type       handle, 
  void                 *ps_result_ind_user_data,
  eap_result_enum_type  result,
  uint8                *master_session_key,
  uint16                master_session_key_len

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS 
  None  
===========================================================================*/

void ppp_eap_result_cb
(
  eap_handle_type       handle, 
  void                 *ps_result_ind_user_data,
  eap_result_enum_type  result,
  uint8                *master_session_key,
  uint16                master_session_key_len
)
{
    ppp_type *ppp_cb_ptr= NULL;
    ps_ppp_eap_result_enum_type ps_ppp_eap_result;
    eap_sim_aka_meta_info_type* meta_info_ptr = NULL;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if((handle == EAP_INVALID_HANDLE) ||ps_result_ind_user_data == NULL)
    {
      DATA_ERR_FATAL("Invalid Arguments passed");
      return;
    }

    ppp_cb_ptr = (ppp_type *)ps_result_ind_user_data;

    /*Get the sequence number details from EAP*/

    meta_info_ptr = (eap_sim_aka_meta_info_type*)eap_get_meta_info(handle);
    if(meta_info_ptr == NULL)
    {
      DATA_ERR_FATAL("Invalid meta_info_ptr");
      return;        
    }
    if( ( FALSE == ppp_cb_ptr->auth.info.ehrpd_usim_present ) &&
        ( meta_info_ptr->aka_algo_seqnum_arr_num_elements != 
          EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS ) )
    {
      LOG_MSG_ERROR_1("Invalid aka_algo_seqnum_arr_num_elements=%d",
                      meta_info_ptr->aka_algo_seqnum_arr_num_elements );
      ASSERT(0);
      return;
    }

    ppp_cb_ptr->auth.info.aka_algo_seqnum_arr_num_elements = 
                                   meta_info_ptr->aka_algo_seqnum_arr_num_elements;

    memset(ppp_cb_ptr->auth.info.aka_algo_seqnum_arr, 
           0,
           EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS);

    memscpy(ppp_cb_ptr->auth.info.aka_algo_seqnum_arr,
            EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS,
            meta_info_ptr->aka_algo_seqnum_arr,
            meta_info_ptr->aka_algo_seqnum_arr_num_elements*(sizeof(uint64)));

    if (result == EAP_RESULT_SUCCESS)        
    {
      LOG_MSG_INFO2_0("EAP authentication Succeeded");
      ps_ppp_eap_result = PS_PPP_EAP_RESULT_SUCCESS;      
      ppp_cb_ptr->auth.flags|= PPP_AP_SUCCESS;   
    }
    else
    {
      LOG_MSG_INFO2_0("EAP authentication Failed");
      ps_ppp_eap_result = PS_PPP_EAP_RESULT_FAILURE;        
      master_session_key_len =0;
      master_session_key=NULL;                    
      ppp_cb_ptr->auth.flags |= PPP_AP_FAILED;
    }

    /*-------------------------------------------------------------------------
       Call DS callback
    -------------------------------------------------------------------------*/

    ppp_cb_ptr->auth.info.get_config_data_after_auth_ptr(
                        master_session_key,
                        master_session_key_len,
                        ps_ppp_eap_result,
                        ppp_cb_ptr->auth.info.eap_user_data,
                        ppp_cb_ptr->auth.info.aka_algo_seqnum_arr, 
                        ppp_cb_ptr->auth.info.aka_algo_seqnum_arr_num_elements
                        );


    ppp_auth_complete(ppp_cb_ptr, PPP_EAP_PROTOCOL);
    return;
}/*ppp_eap_result_cb*/


/*===========================================================================
FUNCTION PPP_EAP_TX_CB

DESCRIPTION
  This is the callback that EAP will call to transmit a packet.

PARAMETERS
  void * userdata   
  dsm_item_type **pdu 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS 
  None  
===========================================================================*/

void ppp_eap_tx_cb
(
  void *userdata,
  dsm_item_type **pdu
)
{
  ppp_type *ppp_cb_ptr= NULL;

  if(userdata==NULL || pdu ==NULL || * pdu==NULL)
  {
    DATA_ERR_FATAL("Invalid arguments passed");
    return;
  }
  ppp_cb_ptr=(ppp_type *)userdata;

  if (pppi_outgoing(ppp_cb_ptr->fsm[LCP].device,
                    PPP_EAP_PROTOCOL,
                    pdu,
                    NULL) == -1)
    {
      LOG_MSG_INFO1_1("dev %d transmit EAP packet failed",
                      ppp_cb_ptr->fsm[LCP].device);
    }
  return;
}/*ppp_eap_tx_cb*/

/*===========================================================================
FUNCTION PPP_EAP_NOTOFICATION_CB

DESCRIPTION
  This is the callback that EAP will call to send notifications. 
  This will print the notificaiton result.

PARAMETERS
  void * userdata   
  
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS 
  None  
===========================================================================*/

void ppp_eap_notification_cb
(
  void * user_data
)
{
  eap_sim_aka_notification_enum_type *eap_notificaiton_result;

    if(user_data == NULL)
    {
      DATA_ERR_FATAL("Invalid arguments passed");
      return;
    }

    eap_notificaiton_result = (eap_sim_aka_notification_enum_type *)(user_data);
    LOG_MSG_INFO1_1("Notification result %d", eap_notificaiton_result);
    return;
}/*ppp_eap_notification_cb*/

/*===========================================================================
FUNCTION PPP_EAP_PROC

DESCRIPTION
  This will process EAP packets

PARAMETERS
  ppp_dev_enum_type device
  dsm_item_type **item_ref_ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS 
  None  
===========================================================================*/

void ppp_eap_proc
(
  ppp_dev_enum_type device,
  dsm_item_type **item_ref_ptr
)
{
  ppp_type *ppp_cb_ptr = NULL;
  ppp_type *ppp_cb_array = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( PPP_MAX_DEV <= device)
  {
    LOG_MSG_ERROR_1( "Invalid argument passed: device %d", device );
    ASSERT( 0 );
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + device;

  /* Check if EAP handle is valid*/
  if(ppp_cb_ptr->auth.ps_eap_handle == EAP_INVALID_HANDLE)
  {
    LOG_MSG_ERROR_0("EAP Handle not valid ");
    dsm_free_packet(item_ref_ptr);
    return;
  }

  eap_input(ppp_cb_ptr->auth.ps_eap_handle,
            item_ref_ptr);
  return;
}/*ppp_eap_proc*/

#endif /*FEATURE_DATA_PS_PPP_EAP*/

#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION AUTH_GETCONST()

DESCRIPTION
 To get auth constants from the phone

PARAMETERS
 ppp_cb_ptr    : pointer to ppp_cb
 ppp_auth_enum : the fsm constant to fetch

RETURN VALUE
 Integer value of the constant, -1 in case of an error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int auth_getconst
(
  ppp_type              * ppp_cb_ptr,
  ppp_auth_type_enum_type ppp_auth_enum
)
{

  if (ppp_cb_ptr == NULL || ppp_auth_enum >= PPP_AUTH_MAX_PARAM)
  {
    LOG_MSG_ERROR_2( "Invalid arguments: ppp_cb_ptr %d, ppp_auth_enum %d",
                     ppp_cb_ptr, ppp_auth_enum );
    ASSERT(0);
    return -1;
  }

  switch (ppp_auth_enum)
  {
    case PPP_AUTH_PASSWD_ENCRYPT:
      return ppp_cb_ptr->auth.require_encrypted_passwd;
    case PPP_AUTH_TIMEOUT:
      return ppp_cb_ptr->auth.timeout;
    case PPP_AUTH_RETRY_COUNT:
      return ppp_cb_ptr->auth.retry_counter;
    case PPP_AUTH_RETRY_LIMIT:
      return ppp_cb_ptr->auth.retry_limit;
    default:
      LOG_MSG_ERROR_1( "Unknown ppp_auth_enum %d", ppp_auth_enum );
      return -1;
  }
} /* auth_getconst */

/*===========================================================================
FUNCTION AUTH_SETCONST()

DESCRIPTION
 To set auth constants on the phone

PARAMETERS
 ppp_cb_ptr    : pointer to ppp_cb
 ppp_auth_enum : the auth constant to set
 value         : byte-array of data for new value

RETURN VALUE
 0 for success, -1 in case of an error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int auth_setconst
(
  ppp_type              * ppp_cb_ptr,
  ppp_auth_type_enum_type ppp_auth_enum,
  uint32 value
)
{
  if(ppp_cb_ptr == NULL || ppp_auth_enum >= PPP_AUTH_MAX_PARAM)
  {
    LOG_MSG_ERROR_2( "Invalid arguments: ppp_cb_ptr %d, ppp_auth_enum %d",
                     ppp_cb_ptr, ppp_auth_enum );
    ASSERT(0);
    return -1;
  }

  switch (ppp_auth_enum)
  {
    case PPP_AUTH_TIMEOUT:
      ppp_cb_ptr->auth.timeout = (uint16)value;
      break;
    case PPP_AUTH_RETRY_COUNT:
      ppp_cb_ptr->auth.retry_counter = (uint8)value;
      break;
    case PPP_AUTH_RETRY_LIMIT:
      ppp_cb_ptr->auth.retry_limit = (uint8)value;
      break;
    default:
      LOG_MSG_ERROR_1( "Unknown ppp_auth_enum %d", ppp_auth_enum );
      return -1;
  }
  return 0;
} /* auth_setconst */
#endif /* FEATURE_STA_PPP */

void pap_internal_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
)
{
  ppp_type *      ppp_cb_ptr;
  ppp_type *      ppp_cb_array = NULL;
  ppp_config_hdr_type auth_hdr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + device;

  /*-------------------------------------------------------------------------
    The rxed message format looks like:
      Code: 1 Octet (the message type)
      Identifier: 1 Octet
      Length: 2 Octets
  
    *item_ref_ptr will be pointing to the code field so starting here extract
    the first three fields.
  -------------------------------------------------------------------------*/
  auth_hdr.code        = (uint8)dsm_pull8(item_ref_ptr);
  auth_hdr.identifier  = (uint8)dsm_pull8(item_ref_ptr);

  /*-----------------------------------------------------------------------
        store the id of this packet as the active on:  for use in retries.
  -----------------------------------------------------------------------*/
  ppp_cb_ptr->auth.active_id = auth_hdr.identifier;
  papi_send_auth_req(ppp_cb_ptr);
  
/*-------------------------------------------------------------------------
  We are freeing the item here with the expectation that the Mobile is
  generating the response, so this one is meaningless.
-------------------------------------------------------------------------*/
dsm_free_packet(item_ref_ptr);
}/* void pap_internal_proc() */

/*===========================================================================
FUNCTION PPP_AUTH_GET_RAND_CHAP_CHAL_VALUE()

DESCRIPTION
  This function generates a random value of size challenge_len and places
  this value in challenge_value

PARAMETERS
  challenge_value
  challenge_len

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS 
  None  
===========================================================================*/
void ppp_auth_get_rand_chap_chal_value
(
  unsigned char *challenge_value,
  uint8          challenge_len
)
{
  char  *ppp_auth = NULL;
  char  *ppp_end = NULL;
  uint32 random_value;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == challenge_value ||
      0 == challenge_len )
  {
    DATA_ERR_FATAL("Incorrect args");
  }

  ppp_auth = (char *)challenge_value;
  ppp_end = (char *)(ppp_auth + challenge_len);

  do
  {
    (void)ps_utils_generate_rand_num( (void*)&random_value,
                                      sizeof(random_value) );
    memscpy(ppp_auth, 
             ppp_end - ppp_auth,
             &random_value, 
             sizeof(random_value));
    ppp_auth += sizeof(random_value);
  }while (ppp_auth < ppp_end);

  return;

}

/*===========================================================================
FUNCTION PPP_AUTH_CALC_MD5_CHAP_RESP_VALUE()

DESCRIPTION
  This function calculates the MD5 hash value of the passed in params

PARAMETERS
  uint8          identifier,
  unsigned char *password,
  uint8          password_len,
  unsigned char *chap_chal_value,
  uint8          chap_chal_value_len,
  unsigned char *chap_resp_value

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS 
  Calculated value stored in chap_resp_value
===========================================================================*/
void ppp_auth_calc_md5_chap_resp_value
(
  uint8          identifier,
  unsigned char *password,
  uint8          password_len,
  unsigned char *chap_chal_value,
  uint8          chap_chal_value_len,
  unsigned char *chap_resp_value
)
{
  MD5_CTX context;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == password || NULL == chap_chal_value || NULL == chap_resp_value ||
      0 == password_len || 0 == chap_chal_value_len )
  {
    DATA_ERR_FATAL("Incorrect args");
  }

  MD5Init(&context);
  MD5Update(&context, &identifier, PPP_AUTH_ID_SIZE);
  MD5Update(&context, password, password_len);
  MD5Update(&context, chap_chal_value, chap_chal_value_len);
  MD5Final(chap_resp_value, &context);

  return;
}

