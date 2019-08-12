/*===========================================================================

                          P S _ E A P _ P E E R . C
                                
DESCRIPTION
  This software unit holds the PS_EAP peer layer of the EAP system.
  
  The functions externalized by this module are to be called ONLY by the EAP
  modules.


EXTERNALIZED FUNCTIONS


DESCRIPTION

  The EAP Peer is implemented as a state machine, derived from the behavior
  described in RFC 3748. Note that the RFC does not mandate/recommend such
  a design.
  
  The main reasons for the state machine are as follows:
  - Method type is not passed in by the method itself; instead it is stored
    at the Peer
  - Once authentication with a particular method has commenced, only that 
    method is permissible until the end of the conversation (see exception
    below)
  - The only exception is the Notification, which may be received after
    authentication has started
  - Once an authentication result indication has been received, the Peer
    must discard packets of all methods and may only process Success/Failure
  
  States:
  INIT_S (I)
  This is the initial state and all methods are processed in this state.At
  this stage, all authentication indications and Success/Failure packets
  are discarded as spurious.
  
  AUTHENTICATING_S (A)
  Once an authentication method has been responded to with a Non-NAK response
  no other method may be processed by the Peer. This state ensures that a 
  single authentication method is handled per conversation.
  
  NOTIFYING_S (N)
  The only method that MAY be processed after authentication has commenced is
  the NOTIFICATION method. However, as we need to preserve the prevailing
  authentication method, this state determines that we are handling a 
  notification while authenticating.
  
  CLOSING_S (C)
  This state indicates the end of the authentication exchanges, and implies
  that the Peer is waiting ONLY for the Success/Failure packets. No other
  methods may be handled in this state.
  
  CLOSED_S (CD)
  This state is one in which no further EAP transactions take place. However,
  an authentication method such as TLS might still be active.
  This is particularly relevant for inner methods which run on top of TLS.
  EAP instance is cleared when eap_delete_instance() is invoked.
  
  
  Events:
  IDENTITY_REQ_EV
  Identity request received
  
  NOTIFY_REQ_EV
  Notification request received
  
  NAK_EV
  Unsupported method request received
  
  AUTH_REQ_EV
  Auth request received
  
  AUTH_IND_EV
  Authentication result indication from method
  
  SUCCESS_EV
  Success packet received from authenticator
  
  FAILURE_EV
  Failure packet received from authenticator
  
  AUTH_RES_TIMEOUT_EV
  Timeout while waiting for Success/Failure - packets may have been lost
  
  
LIMITATIONS
  
  - Success/Failure packets are only processed after an auth indication.
    Some methods may not provide an indication . e.g. MD5 challenge directly
    results in a Success/Failure packet.
  
  - Notifications are always processed; some methods may prohibit this in 
    their specification; for such methods, notification requests must be 
    silently discarded.
  
  


 Copyright (c)2005-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_eap_peer.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/04/15    fn     Changes to notify EAP failures to client
10/06/13    fn     Added fix for crash in peeri_success/peeri_failure
09/01/13    fn      Added support for multi active EAP clients
07/15/13    fn     Made changes to ensure reauth/pseudo id cache is intialize
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
05/21/13    fn     Fixed caching of reauth id and pseudonym id
03/18/13    fn     Added multi-sim support
10/26/12    fn     Dynamic allocation of eap_peer_lyr_cb
08/17/12    fn     Cleanup ASSERT() and dsm_pushdown 
03/02/12    scb    Fixed compiler warnings to replace ASSERT() calls.
08/26/11    op     Added changes to obtain supplementary SIM/AKA auth info 
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
12/17/10    mga    Fix in eap_peeri_c_auth_ind, fix to set 
                   authenticator_rsp_required accordingly
10/27/09    jee    Handling of pseudonym/reauth-id when imsi is changed
10/26/09    jee    Fixed the handling of NOTIFY_REQ in closing state
10/01/09    jee    Removed the assert in case of invalid method in re-auth id
09/29/09    jee    Fixed klocwork errors
08/31/09    mga    Merged from eHRPD branch
03/06/09    scb    Added changes for memory reduction initiative.
01/14/09  scb/mga  Made changes to add case EAP_PEER_CLOSED_S in 
                   eap_peer_xmit_pkt.
12/12/08    scb    Set incoming SDU to NULL in the request_input() call.
10/21/08    scb    Added function to obtain the reauth id from SFS/EFS
07/18/08    rnp    Fixed Compiler warnings
04/11/08    scb    Klocwork fixes
11/28/07    lti    Added changes for supporting EAP Ext
11/13/06    lti    Modified handling of EAP-Success and EAP-Failure pkts
08/25/06    lti    Added function for setting the resp required flag
                   and critical section for this setting
08/25/06    lti    Added support for result cback 
                   Added support for additional auth schemes EAP-SIM/AKA
02/27/06    lyr    WLAN 3.0 support for additional auth schemes, raw mode
                   support
03/30/05    lyr    Created module

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"         /* Customer Specific Features                */
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_PS_EAP
#include "ps_eap.h"

#include "ps_eapi.h"

#include "dsm.h"

#include "msg.h"

#include "ps_system_heap.h"

#include "ps_eap_diag.h"      /* For event logging support                 */
#include "ps_eap_sim_aka_ume.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Enumeration of all EAP methods supported
  
  See RFC 3748
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_PEER_MIN_TYPE           =  -1,
  /*-------------------------------------------------------------------------
    Types 1-4 are REQUIRED to be supported by RFC
  -------------------------------------------------------------------------*/
  EAP_PEER_IDENTITY_TYPE      =   1,
  EAP_PEER_NOTIFICATION_TYPE  =   2,
  EAP_PEER_NAK_TYPE           =   3,
  EAP_PEER_MD5_CHALLENGE_TYPE =   4,

  EAP_PEER_TLS_TYPE           =  13,

  EAP_PEER_SIM_TYPE           =  18,

  EAP_PEER_TTLS_TYPE          =  21,

  EAP_PEER_AKA_TYPE           =  23,

  EAP_PEER_PEAP_TYPE          =  25,

  EAP_PEER_MSCHAP_V2_TYPE     =  26,

  EAP_PEER_TLV_TYPE           =  33,

  EAP_PEER_AKA_PRIME_TYPE     =  50,

  EAP_PEER_EXT_TYPE           = 254,

  EAP_PEER_MAX_TYPE
} eap_peer_type_enum_type;


/*---------------------------------------------------------------------------
  Peer state enumeration
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_PEER_MIN_S             = -1,
  
  EAP_PEER_INIT_S            =  0,      /* Peer is ready to receive reqs   */
  EAP_PEER_AUTHENTICATING_S  =  1,      /* Peer has started an auth method */
  EAP_PEER_NOTIFYING_S       =  2,      /* Peer is processing notification */
  EAP_PEER_CLOSING_S         =  3,      /* Peer is closing current session */
  EAP_PEER_CLOSED_S          =  4,      /* Auth has completed (succ/fail)  */

  EAP_PEER_MAX_S

} eap_peer_state_enum_type;


/*---------------------------------------------------------------------------
  Peer event enumeration
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_PEER_MIN_EV              = -1,

  EAP_PEER_IDENTITY_REQ_EV     =  0,
  EAP_PEER_NOTIFY_REQ_EV       =  1,
  EAP_PEER_NAK_EV              =  2,

  EAP_PEER_AUTH_REQ_EV         =  3,

  EAP_PEER_AUTH_IND_EV         =  4,
  
  EAP_PEER_SUCCESS_EV          =  5,      /* Success EAP frame received    */
  EAP_PEER_FAILURE_EV          =  6,      /* Failure EAP frame received    */
  
  EAP_PEER_AUTH_RES_TIMEOUT_EV =  7,      /* Timeout receiving Succ/Fail   */

  EAP_PEER_MAX_EV

} eap_peer_ev_enum_type;


/*---------------------------------------------------------------------------
  Prototype for event handling function
---------------------------------------------------------------------------*/
typedef void (* eap_peer_ev_hdlr_type)(eap_peer_handle_type);


/*---------------------------------------------------------------------------
  Prototype for code-specific input function
---------------------------------------------------------------------------*/
typedef void (* eap_peer_code_input_fnc_type)( eap_peer_handle_type,
                                               dsm_item_type **);

/*---------------------------------------------------------------------------
  Peer layer header
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8 type;
} eap_peer_frm_hdr_type;


/*---------------------------------------------------------------------------
  Peer layer information block - this stores the Peer state machine and
  related data structures
---------------------------------------------------------------------------*/
typedef struct
{
  eap_handle_type           eap_handle;       /* Handle to the EAP instance*/
  
  eap_peer_state_enum_type  state;            /* Current state of the peer */

  /*-------------------------------------------------------------------------
    This holds the currently processed method type; a response type may be
    determined by this value (see note below)
    
    NOTE: Notification is the ONLY exception to this as it may occur while
    another method is in progress
    For this reason, the state is also used to determine the response
  -------------------------------------------------------------------------*/
  eap_peer_type_enum_type   current_method_type;

  /*-------------------------------------------------------------------------
    Current packet being processed
  -------------------------------------------------------------------------*/
  dsm_item_type            *pending_pkt;

  eap_peer_type_enum_type   pending_pkt_type;

  /*-------------------------------------------------------------------------
    Bitmask of methods currently supported by the EAP peer
  -------------------------------------------------------------------------*/
  uint32                    eap_auth_prot_mask;  /* auth protocol mask     */

  /*-------------------------------------------------------------------------
    Identity of the peer - this is needed by the Identity method
  -------------------------------------------------------------------------*/
  eap_identity_type         user_id;             /* User Id                */

  boolean                   raw_mode;            /* Pass-through?          */

  /*-------------------------------------------------------------------------
    Wait for server result?
  -------------------------------------------------------------------------*/
  boolean                   authenticator_rsp_required;

  /*-------------------------------------------------------------------------
    SUCCESS/FAILURE packet received from authenticator?
  -------------------------------------------------------------------------*/
  boolean                   recvd_server_result;

  /*-------------------------------------------------------------------------
    The auth info is passed into EAP as part of the authentication indication
    from the auth method (e.g. TLS). However, this is not immediately passed
    down to client (e.g. 802.1x). Peer waits for a Success of Failure before
    passing this down.
    Hence the need to buffer this information.
  -------------------------------------------------------------------------*/
  struct
  {
    eap_result_enum_type result;
    uint8                pre_master_key[EAP_MAX_PRE_MASTER_KEY_SIZE];
    uint16               pre_master_key_len;
  } auth_info;

} eap_peer_lyr_cb_type;


/*---------------------------------------------------------------------------
  Macro to check if the Peer layer handle is valid
---------------------------------------------------------------------------*/
#define EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle )           \
  ( (peer_lyr_handle != -1) &&                                \
    (eap_peer_lyr_cb[peer_lyr_handle] != NULL)                \
    ? TRUE : FALSE )

/*---------------------------------------------------------------------------
  Internal method type definitions
  Corresponds to the initial (0-2) values for eap_method_enum_type
  
  Defines to index into the array of callbacks:
---------------------------------------------------------------------------*/
typedef enum
{
  /* NOTE: >=3 values are for external methods                             */

  EAP_IDENTITY_METHOD       = 0,

  EAP_NOTIFICATION_METHOD   = 1,
  EAP_NAK_METHOD            = 2

} eap_int_method_enum_type;

/*===========================================================================

                          FORWARD DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
Functions common to multiple states
---------------------------------------------------------------------------*/
/* Common to all states                                                    */
static void eap_peeri_cleanup(eap_peer_handle_type);

/* Common to authenticating and notifying states                           */
static void eap_peeri_a_n_auth_ind(eap_peer_handle_type);
static void eap_peeri_a_n_result(eap_peer_handle_type);

/*---------------------------------------------------------------------------
  Functions for init state
---------------------------------------------------------------------------*/
static void eap_peeri_i_identity_req(eap_peer_handle_type);
static void eap_peeri_i_notify_req(eap_peer_handle_type);
static void eap_peeri_i_nak(eap_peer_handle_type);
static void eap_peeri_i_auth_req(eap_peer_handle_type);

/*---------------------------------------------------------------------------
  Functions for authenticating state
---------------------------------------------------------------------------*/
static void eap_peeri_a_notify_req(eap_peer_handle_type);
static void eap_peeri_a_auth_req(eap_peer_handle_type);

/*---------------------------------------------------------------------------
  Functions for notifying state
---------------------------------------------------------------------------*/
static void eap_peeri_n_notify_req(eap_peer_handle_type);
static void eap_peeri_n_auth_req(eap_peer_handle_type);

/*---------------------------------------------------------------------------
  Functions for closing state
---------------------------------------------------------------------------*/
static void eap_peeri_c_notify_req(eap_peer_handle_type);
static void eap_peeri_c_auth_req(eap_peer_handle_type);
static void eap_peeri_c_auth_ind(eap_peer_handle_type);
static void eap_peeri_c_success(eap_peer_handle_type);
static void eap_peeri_c_failure(eap_peer_handle_type);
static void eap_peeri_c_auth_res_timeout(eap_peer_handle_type);


/*---------------------------------------------------------------------------
  Functions for code-specific input handling
---------------------------------------------------------------------------*/
static void eap_peeri_request_input
( 
  eap_peer_handle_type   peer_lyr_handle,
  dsm_item_type        **sdu
);

static void eap_peeri_success_input
( 
  eap_peer_handle_type   peer_lyr_handle,
  dsm_item_type        **sdu
);

static void eap_peeri_failure_input
( 
  eap_peer_handle_type   peer_lyr_handle,
  dsm_item_type        **sdu
);


/*---------------------------------------------------------------------------
  Function to process events
---------------------------------------------------------------------------*/
static void eap_peeri_process_event
(
  eap_peer_handle_type   peer_lyr_handle,
  eap_peer_ev_enum_type  event
);


/*---------------------------------------------------------------------------
  Function to change the state of the peer
---------------------------------------------------------------------------*/
static void eap_peeri_transition_state
(
  eap_peer_handle_type      peer_lyr_handle,
  eap_peer_state_enum_type  state
);


/*---------------------------------------------------------------------------
  Helper functions
---------------------------------------------------------------------------*/
static eap_method_enum_type eap_peeri_get_method_from_type
(
  eap_peer_type_enum_type type
);


/*===========================================================================
                              
                              DATA DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  EAP Peer Layer Critical Section
---------------------------------------------------------------------------*/
static ps_crit_sect_type eap_peer_crit_sect;

/*---------------------------------------------------------------------------
  This forms the pool of Peer layer control blocks that can be assigned using
  eap_peer_create_instance()
---------------------------------------------------------------------------*/
static eap_peer_lyr_cb_type* eap_peer_lyr_cb[EAP_MAX_INSTANCES];


/*---------------------------------------------------------------------------
  Table of code-specific input functions
  
  NOTE: Code values are taken from RFC, hence the non-usage of 0th element
---------------------------------------------------------------------------*/
static const eap_peer_code_input_fnc_type 
eap_peer_code_input_fnc[EAP_MAX_CODE] = 
{
  /* 0th element is EAP_MIN_CODE  */
  NULL,
  
  eap_peeri_request_input,

  /* Responses should never reach the Peer */
  NULL,
  
  eap_peeri_success_input,
  eap_peeri_failure_input
};

/*---------------------------------------------------------------------------
  The table of method input function pointers
  
  This holds input functions for both internal and external methods
---------------------------------------------------------------------------*/
static eap_method_input_cback_type eap_method_input_cback[EAP_MAX_METHOD] = 
{
  eap_identity_input,
  eap_notification_input,
  eap_nak_input,
  eap_md5_input,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,  /* EAP_SIM_METHOD */
  NULL,     /* EAP_AKA_METHOD */
  NULL,  /* EAP_AKA_PRIME_METHOD */
  NULL     /* EAP_EXT_METHOD */
};

/*---------------------------------------------------------------------------
  The table of method close function pointers
  
  This holds close functions for both internal and external methods
---------------------------------------------------------------------------*/
static eap_method_close_cback_type eap_method_close_cback[EAP_MAX_METHOD] = 
{
  NULL, // identity
  NULL, // notification
  NULL, // nak
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,  /* EAP_SIM_METHOD */
  NULL,    /* EAP_AKA_METHOD */
  NULL,  /* EAP_AKA_PRIME_METHOD */
  NULL    /* EAP_EXT_METHOD */
};

/*---------------------------------------------------------------------------
  The table of method result function pointers
  
  This holds result functions for both internal and external methods
---------------------------------------------------------------------------*/
static eap_method_result_cback_type eap_method_result_cback[EAP_MAX_METHOD] = 
{
  NULL,  // identity
  NULL,  // notification
  NULL,  // nak
  NULL,  /* EAP_MD5_METHOD */
  NULL,    /* EAP_TLS_METHOD */
  NULL,    /* EAP_PEAP_METHOD */
  NULL,    /* EAP_TTLS_METHOD */
  NULL,    /* EAP_TLV_METHOD */
  NULL,     /* EAP_MSCHAP_V2_METHOD */
  NULL,  /* EAP_SIM_METHOD */
  NULL,     /* EAP_AKA_METHOD */
  NULL,  /* EAP_AKA_PRIME_METHOD */
  NULL     /* EAP_EXT_METHOD */
};

/*---------------------------------------------------------------------------
  The table of method result function pointers
  
  This holds supp result functions for both internal and external methods
---------------------------------------------------------------------------*/
static eap_method_supp_result_cback_type 
                              eap_method_supp_result_cback[EAP_MAX_METHOD] = 
{
  NULL,  // identity
  NULL,  // notification
  NULL,  // nak
  NULL,  /* EAP_MD5_METHOD */
  NULL,  /* EAP_TLS_METHOD */
  NULL,  /* EAP_PEAP_METHOD */
  NULL,  /* EAP_TTLS_METHOD */
  NULL,  /* EAP_TLV_METHOD */
  NULL,  /* EAP_MSCHAP_V2_METHOD */
  NULL,  /* EAP_SIM_METHOD */
  NULL,  /* EAP_AKA_METHOD */
  NULL,  /* EAP_AKA_PRIME_METHOD */
  NULL   /* EAP_EXT_METHOD */
};

/*---------------------------------------------------------------------------
  Mapping between the method types enumeration and types as per RFC
  
  NOTE: This has to be synced between eap_peer_type_enum_type and
  eap_method_enum_type
---------------------------------------------------------------------------*/
static const eap_peer_type_enum_type eap_method_to_type_map[EAP_MAX_METHOD] = 
{
  EAP_PEER_IDENTITY_TYPE,
  EAP_PEER_NOTIFICATION_TYPE,
  EAP_PEER_NAK_TYPE,
  EAP_PEER_MD5_CHALLENGE_TYPE,
  EAP_PEER_TLS_TYPE,
  EAP_PEER_PEAP_TYPE,
  EAP_PEER_TTLS_TYPE,
  EAP_PEER_TLV_TYPE,
  EAP_PEER_MSCHAP_V2_TYPE,
  EAP_PEER_SIM_TYPE,
  EAP_PEER_AKA_TYPE,
  EAP_PEER_AKA_PRIME_TYPE,
  EAP_PEER_EXT_TYPE
};


/*---------------------------------------------------------------------------
  Event handler function table - this table holds the event handler fnc for
  each state
  
  IMPORTANT NOTE:
  eap_peeri_cleanup *must* be registered for method handling functions - this
  frees the packet stored in Peer which will otherwise not be consumed
---------------------------------------------------------------------------*/
static const eap_peer_ev_hdlr_type
eap_peer_state_ev_fnc[EAP_PEER_MAX_S][EAP_PEER_MAX_EV] = 
{
  /* EAP_PEER_INIT_S  */
  {
    /* EAP_PEER_IDENTITY_REQ_EV     */
    eap_peeri_i_identity_req,
    /* EAP_PEER_NOTIFY_REQ_EV       */
    eap_peeri_i_notify_req,
    /* EAP_PEER_NAK_EV              */
    eap_peeri_i_nak,
    /* EAP_PEER_AUTH_REQ_EV          */
    eap_peeri_i_auth_req,
    /* EAP_PEER_AUTH_IND_EV         */
    NULL,
    /* EAP_PEER_SUCCESS_EV          */
    NULL,
    /* EAP_PEER_FAILURE_EV          */
    eap_peeri_c_failure,
    /* EAP_PEER_AUTH_RES_TIMEOUT_EV */
    NULL,
  },
  /* EAP_PEER_AUTHENTICATING_S  */
  {
    /* EAP_PEER_IDENTITY_REQ_EV     */
    eap_peeri_cleanup,
    /* EAP_PEER_NOTIFY_REQ_EV       */
    eap_peeri_a_notify_req,
    /* EAP_PEER_NAK_EV              */
    eap_peeri_cleanup,
    /* EAP_PEER_AUTH_REQ_EV          */
    eap_peeri_a_auth_req,
    /* EAP_PEER_AUTH_IND_EV         */
    eap_peeri_a_n_auth_ind,
    /* EAP_PEER_SUCCESS_EV          */
    eap_peeri_a_n_result,
    /* EAP_PEER_FAILURE_EV          */
    eap_peeri_c_failure,
    /* EAP_PEER_AUTH_RES_TIMEOUT_EV */
    NULL,
  },
  /* EAP_PEER_NOTIFYING_S  */
  {
    /* EAP_PEER_IDENTITY_REQ_EV     */
    eap_peeri_cleanup,
    /* EAP_PEER_NOTIFY_REQ_EV       */
    eap_peeri_n_notify_req,
    /* EAP_PEER_NAK_EV              */
    eap_peeri_cleanup,
    /* EAP_PEER_AUTH_REQ_EV          */
    eap_peeri_n_auth_req,
    /* EAP_PEER_AUTH_IND_EV         */
    eap_peeri_a_n_auth_ind,
    /* EAP_PEER_SUCCESS_EV          */
    eap_peeri_a_n_result,
    /* EAP_PEER_FAILURE_EV          */
    eap_peeri_c_failure,
    /* EAP_PEER_AUTH_RES_TIMEOUT_EV */
    NULL,
  },
  /* EAP_PEER_CLOSING_S  */
  {
    /* EAP_PEER_IDENTITY_REQ_EV     */
    eap_peeri_cleanup,
    /* EAP_PEER_NOTIFY_REQ_EV       */
    eap_peeri_c_notify_req,
    /* EAP_PEER_NAK_EV              */
    eap_peeri_cleanup,
    /* EAP_PEER_AUTH_REQ_EV          */
    eap_peeri_c_auth_req,
    /* EAP_PEER_AUTH_IND_EV         */
    eap_peeri_c_auth_ind,
    /* EAP_PEER_SUCCESS_EV          */
    eap_peeri_c_success,
    /* EAP_PEER_FAILURE_EV          */
    eap_peeri_c_failure,
    /* EAP_PEER_AUTH_RES_TIMEOUT_EV */
    eap_peeri_c_auth_res_timeout,
  },
  /* EAP_PEER_CLOSED_S  */
  {
    /* EAP_PEER_IDENTITY_REQ_EV     */
    eap_peeri_cleanup,
    /* EAP_PEER_NOTIFY_REQ_EV       */
    eap_peeri_cleanup,
    /* EAP_PEER_NAK_EV              */
    eap_peeri_cleanup,
    /* EAP_PEER_AUTH_REQ_EV          */
    eap_peeri_cleanup,
    /* EAP_PEER_AUTH_IND_EV         */
    NULL,
    /* EAP_PEER_SUCCESS_EV          */
    NULL,
    /* EAP_PEER_FAILURE_EV          */
    NULL,
    /* EAP_PEER_AUTH_RES_TIMEOUT_EV */
    NULL,
  }
};


/*===========================================================================

                          EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION EAP_PEER_INIT

DESCRIPTION
  Initializes the Peer layer control blocks.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Initialize the Peer layer control blocks.

===========================================================================*/
void eap_peer_init
(
  void
)
{
  int peer_lyr_cb_cnt;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  PS_INIT_CRIT_SECTION( &eap_peer_crit_sect );

  /*-------------------------------------------------------------------------
    Initialize the Peer layer
  -------------------------------------------------------------------------*/
  for(peer_lyr_cb_cnt=0;peer_lyr_cb_cnt<EAP_MAX_INSTANCES;peer_lyr_cb_cnt++)
  {
    eap_peer_lyr_cb[peer_lyr_cb_cnt] = NULL; 

    eap_reauth_id[peer_lyr_cb_cnt] = NULL;

    eap_pseudonym_id[peer_lyr_cb_cnt] = NULL;	
  }
  
} /* eap_peer_init()  */

/*===========================================================================
FUNCTION EAP_PEER_SET_AUTHENTICATOR_RSP_REQUIRED

DESCRIPTION
  Offers support for dynamically setting the authenticator_rsp_required flag

DEPENDENCIES

  
RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void 
eap_peer_set_authenticator_rsp_required
(
  eap_peer_handle_type     peer_lyr_handle,
  boolean                  authenticator_rsp_required 
)
{
  /*-----------------------------------------------------------------------
    Check for handle validity and retrieve the Peer layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  PS_ENTER_CRIT_SECTION( &eap_peer_crit_sect );
  eap_peer_lyr_cb[peer_lyr_handle]->authenticator_rsp_required = 
                                                  authenticator_rsp_required;
  PS_LEAVE_CRIT_SECTION( &eap_peer_crit_sect );

}/* eap_peer_set_authenticator_rsp_required() */

/*===========================================================================

FUNCTION EAP_PEER_METHOD_INPUT_CBACK_REG

DESCRIPTION
  Registers the method-specific input function with Peer
  
  NOTE: This function should only be called if the method specifies that
  it supports the Peer role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  input function for a given method.
  

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_method_input_cback_reg
(
  eap_method_input_cback_type cback,
  eap_method_enum_type        method
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( cback == NULL )
  {
    LOG_MSG_ERROR_0("Unregister EAP cback" );
    return;
  }

  if( (EAP_MIN_EXT_METHOD > method) || (EAP_MAX_METHOD <= method) )
  {
    LOG_MSG_ERROR_0("Invalid method specified for registering EAP cback" );
    return;
  }

  eap_method_input_cback[method] = cback;

} /* eap_peer_method_input_cback_reg()  */


/*===========================================================================

FUNCTION EAP_PEER_METHOD_CLOSE_CBACK_REG

DESCRIPTION
  Registers the method-specific close function with Peer
  
  NOTE: This function should only be called if the method specifies that
  it supports the Peer role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  close function for a given method.
  

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_method_close_cback_reg
(
  eap_method_close_cback_type cback,
  eap_method_enum_type        method
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( cback == NULL )
  {
    LOG_MSG_ERROR_0("Unregister EAP cback" );
    return;
  }

  if( (EAP_MIN_EXT_METHOD > method) || (EAP_MAX_METHOD <= method) )
  {
    LOG_MSG_ERROR_0("Invalid method specified for registering EAP cback");
    return;
  }

  eap_method_close_cback[method] = cback;

} /* eap_peer_method_close_cback_reg()  */

/*===========================================================================

FUNCTION EAP_PEER_METHOD_RESULT_CBACK_REG

DESCRIPTION
  Registers the method-specific result function with Peer
  
  If a method registers this function it gains complete control over the 
  handling of EAP-Success and EAP-Failure Packets. 
  
  NOTE: This function should only be called if the method specifies that
  it supports the Peer role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  close function for a given method.
  

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_method_result_cback_reg
(
  eap_method_result_cback_type cback,
  eap_method_enum_type        method
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( cback == NULL )
  {
    LOG_MSG_ERROR_0("Unregister EAP cback" );
    return;
  }

  if( (EAP_MIN_EXT_METHOD > method) || (EAP_MAX_METHOD <= method) )
  {
    LOG_MSG_ERROR_0("Invalid method specified for registering EAP cback");
    return;
  }

  eap_method_result_cback[method] = cback;

} /* eap_peer_method_result_cback_reg()  */

/*===========================================================================

FUNCTION EAP_PEER_METHOD_SUPP_RESULT_CBACK_REG

DESCRIPTION
  Registers the method-specific supplementary result function with Peer
  
  NOTE: This function should only be called if the method specifies that
  it supports the Peer role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  close function for a given method.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_method_supp_result_cback_reg
(
  eap_method_supp_result_cback_type cback,
  eap_method_enum_type              method
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL == cback )
  {
    LOG_MSG_ERROR_0("Invalid callback");
    return;
  }

  if( (EAP_MIN_EXT_METHOD > method) || (EAP_MAX_METHOD <= method) )
  {
    LOG_MSG_ERROR_0("Invalid method specified for registering EAP cback");
    return;
  }

  eap_method_supp_result_cback[method] = cback;

} /* eap_peer_method_supp_result_cback_reg()  */

/*===========================================================================

FUNCTION EAP_PEER_CREATE_INSTANCE

DESCRIPTION
  Initializes Peer layer instance and returns handle to this instance.

DEPENDENCIES
  Peer must have been initialized

RETURN VALUE
  Handle to Peer instance.

SIDE EFFECTS
  None.

===========================================================================*/
eap_peer_handle_type eap_peer_create_instance
(  
  eap_peer_info_type *peer_lyr_info
)
{
  eap_peer_handle_type peer_lyr_handle;

  int peer_lyr_cb_cnt;
  int auth_prot_cnt;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Initialize handle to invalid value
  -------------------------------------------------------------------------*/
  peer_lyr_handle = -1;

  /*-------------------------------------------------------------------------
    Find a free Peer layer control block and reserve it
  -------------------------------------------------------------------------*/
  for(peer_lyr_cb_cnt=0;peer_lyr_cb_cnt<EAP_MAX_INSTANCES;peer_lyr_cb_cnt++)
  {
    if( NULL == eap_peer_lyr_cb[peer_lyr_cb_cnt])
    {
      peer_lyr_handle = peer_lyr_cb_cnt;
      break;
    }
  }

  if( -1 == peer_lyr_handle )
  {
    /*-----------------------------------------------------------------------
      Could not find a free instance
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("Could not assign Peer layer instance" );
    return -1;
  }

  if ( NULL == peer_lyr_info )
  {
    LOG_MSG_ERROR_0 ("Invalid EAP Peer info ");
    return -1;
  }

  /*-----------------------------------------------------------------------
    Dynamic allocation of control block
  -----------------------------------------------------------------------*/
  eap_peer_lyr_cb[peer_lyr_handle] = ( eap_peer_lyr_cb_type*)
       ps_system_heap_mem_alloc(sizeof(eap_peer_lyr_cb_type));

  if( eap_peer_lyr_cb[peer_lyr_handle] == NULL)
  {
    LOG_MSG_ERROR_0("EAP peer dynamic ctx allocation failed");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Initialize the Peer layer instance
  -------------------------------------------------------------------------*/  

  memset( eap_peer_lyr_cb[peer_lyr_handle ],
          0,
          sizeof(eap_peer_lyr_cb_type) ); 

  eap_peer_lyr_cb[peer_lyr_handle ]->eap_handle = peer_lyr_info->eap_handle;

  eap_peer_lyr_cb[peer_lyr_handle ]->current_method_type = EAP_PEER_MIN_TYPE;


  /*-------------------------------------------------------------------------
    TODO: Change this to use the authentication protocol mask passed in
    
    For now, client (802.1x) does NOT provide this information.
  -------------------------------------------------------------------------*/
  // eap_peer_lyr_cb[peer_lyr_handle]->eap_auth_prot_mask = ~0;

  // YAH: Adjust to remove MD5 from list because it is defined, but not
  // supported


  eap_peer_lyr_cb[peer_lyr_handle]->eap_auth_prot_mask = 
    peer_lyr_info->eap_auth_prot_mask;

  /*-------------------------------------------------------------------------
    If authentication protocol EAP_AKA_PRIME is supported add the 
    EAP_AKA authentication protocol type to the authentication protocol
    mask automatically. This will ensure that EAP_AKA packets are 
    processed when EAP_AKA_PRIME is the supported method. 

    Further, BIDDING_DOWN attacks are prevented by making AT_BIDDING 
    as mandatory if EAP_AKA_PRIME is supported.
  -------------------------------------------------------------------------*/
  if( 0 < ( ( 1 << EAP_AKA_PRIME_METHOD ) & 
            eap_peer_lyr_cb[peer_lyr_handle]->eap_auth_prot_mask ) )
  {
    eap_peer_lyr_cb[peer_lyr_handle]->eap_auth_prot_mask |= 
                                      ( 1 << EAP_AKA_METHOD );
  }

  eap_peer_lyr_cb[peer_lyr_handle]->state = EAP_PEER_INIT_S;

  eap_peer_lyr_cb[peer_lyr_handle]->user_id = peer_lyr_info->eap_identity;

  eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt = NULL;

  eap_peer_lyr_cb[peer_lyr_cb_cnt]->pending_pkt_type = EAP_PEER_MIN_TYPE;

  eap_peer_lyr_cb[peer_lyr_handle]->raw_mode = peer_lyr_info->raw_mode;

  eap_peer_lyr_cb[peer_lyr_handle]->authenticator_rsp_required = 
    peer_lyr_info->authenticator_rsp_required;

  eap_peer_lyr_cb[peer_lyr_handle]->recvd_server_result = FALSE;

  /*-------------------------------------------------------------------------
    In raw mode, just set the method to the first authentication protocol
    requested
  -------------------------------------------------------------------------*/
  if( TRUE == eap_peer_lyr_cb[peer_lyr_handle]->raw_mode )
  {
    for( auth_prot_cnt = (int)EAP_MIN_EXT_METHOD; 
         auth_prot_cnt < (int)EAP_MAX_METHOD; auth_prot_cnt++ )
    {
      if( ( 1 << auth_prot_cnt ) & peer_lyr_info->eap_auth_prot_mask )
      {
        eap_peer_lyr_cb[peer_lyr_handle]->current_method_type = 
          eap_method_to_type_map[auth_prot_cnt];
        
        eap_peer_lyr_cb[peer_lyr_handle]->state = EAP_PEER_AUTHENTICATING_S;

        break;
      }
    }

    if( EAP_PEER_MIN_TYPE == 
            eap_peer_lyr_cb[peer_lyr_handle]->current_method_type )
    {
      LOG_MSG_ERROR_0("eap_peer_create_instance(): Invalid Method Type");
      PS_SYSTEM_HEAP_MEM_FREE(eap_peer_lyr_cb[peer_lyr_handle]);
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Initialize pseudonym and reauth ids
  -------------------------------------------------------------------------*/ 
  eap_reauth_id[peer_lyr_info->eap_handle] = NULL;
  eap_pseudonym_id[peer_lyr_info->eap_handle] = NULL;

  /*-------------------------------------------------------------------------
    Return a handle if all required steps were successful
  -------------------------------------------------------------------------*/
  return peer_lyr_handle;

} /* eap_peer_create_instance() */


/*===========================================================================

FUNCTION EAP_PEER_DELETE_INSTANCE

DESCRIPTION
   Shuts down the Peer layer instance.

DEPENDENCIES
  EAP Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_delete_instance
(  
  eap_peer_handle_type peer_lyr_handle
)
{
  eap_method_enum_type curr_method;
  eap_peer_type_enum_type curr_type;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  LOG_MSG_INFO1_1("EAP peer delete instance - EAP peer handle: %d",
                  peer_lyr_handle  );

  /*-------------------------------------------------------------------------
    Free the Peer instance
  -------------------------------------------------------------------------*/
  if( TRUE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    if( eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt != NULL )
    {
      dsm_free_packet( &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
      eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt = NULL;
    }

    /*-----------------------------------------------------------------------
      For ease of understanding code: convert the current type into method
      
      (Type is Per RFC; method is local enumeration)
    -----------------------------------------------------------------------*/
    curr_type = eap_peer_lyr_cb[peer_lyr_handle]->current_method_type;

    curr_method = eap_peeri_get_method_from_type(curr_type);

    /*-----------------------------------------------------------------------
      Identify the current method and invoke its close callback
    -----------------------------------------------------------------------*/
    if( (EAP_MAX_METHOD != curr_method) &&
          (NULL != eap_method_close_cback[curr_method]) )
    {
        eap_method_close_cback[curr_method]
          ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle );
    }

    if ( NULL != eap_pseudonym_id[eap_peer_lyr_cb[peer_lyr_handle]->eap_handle] )
    {
      PS_SYSTEM_HEAP_MEM_FREE(eap_pseudonym_id[eap_peer_lyr_cb[peer_lyr_handle]->eap_handle]);
    }

    if ( NULL != eap_reauth_id[eap_peer_lyr_cb[peer_lyr_handle]->eap_handle] )
    {
      PS_SYSTEM_HEAP_MEM_FREE(eap_reauth_id[eap_peer_lyr_cb[peer_lyr_handle]->eap_handle]);
    }

    PS_SYSTEM_HEAP_MEM_FREE(eap_peer_lyr_cb[peer_lyr_handle]);
  }
  else
  {
    LOG_MSG_ERROR_1("Peer handle: %d not valid", peer_lyr_handle  );
  }
} /* eap_peer_delete_instance() */


/*===========================================================================

FUNCTION EAP_PEER_INPUT

DESCRIPTION
  Rx function for EAP Peer layer. Parse the Peer layer header and calls the
  method's input function if necessary.

DEPENDENCIES
  EAP peer instance must have been created

RETURN VALUE
  None

SIDE EFFECTS
  Calls EAP method's RX function.

===========================================================================*/
void eap_peer_input
( 
  eap_handle_type      eap_handle,
  eap_code_enum_type   code,
  dsm_item_type      **sdu
)
{
  eap_peer_handle_type peer_lyr_handle;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the Peer layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("eap_peer_input(): Invalid EAP instance" );

    dsm_free_packet(sdu);
    return;
  }

  if( EAP_MAX_CODE <= code )
  {
    LOG_MSG_ERROR_1("Invalid code %d passed to eap_peer_input", code );
    dsm_free_packet(sdu);
    return;
  } 

  peer_lyr_handle = eap_get_peer_lyr_handle(eap_handle);

  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );

    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    In raw mode, just the packet up the stack to the registered method
  -------------------------------------------------------------------------*/
  if( TRUE == eap_peer_lyr_cb[peer_lyr_handle]->raw_mode )
  {
    eap_method_enum_type     curr_method;
    eap_peer_type_enum_type  curr_type;

    LOG_MSG_INFO2_0("eap_peer_input(): Handling packet in raw mode");

    /*-----------------------------------------------------------------------
      For ease of understanding code: convert the current type into method
      
      (Type is Per RFC; method is local enumeration)
    -----------------------------------------------------------------------*/
    curr_type = eap_peer_lyr_cb[peer_lyr_handle]->current_method_type;

    curr_method = eap_peeri_get_method_from_type(curr_type);

    /*-----------------------------------------------------------------------
      Identify the current method and pass the packet up
    -----------------------------------------------------------------------*/
    if( (EAP_MAX_METHOD != curr_method) &&
        (NULL != eap_method_input_cback[curr_method]) )
    {
      eap_method_input_cback[curr_method]
        ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
          sdu );
      return;
    }

    /*-----------------------------------------------------------------------
      If we reach this point, EAP has been configured in pass through mode
      with a method that has not registered a callback!!
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("eap_peer_input(): No callback register for method"); 
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Pass the packet on to the code-specific input function
  -------------------------------------------------------------------------*/
  if( NULL != eap_peer_code_input_fnc[code] )
  {
    eap_peer_code_input_fnc[code]( peer_lyr_handle,
                                   sdu );
  }
  else
  {
    LOG_MSG_ERROR_1( "Peer unable to process EAP pkt code: %d. Received in error",
                     code );

    dsm_free_packet(sdu);
    return;
  }

} /* eap_peer_input() */


/*===========================================================================
FUNCTION EAP_PEER_XMIT_PKT

DESCRIPTION
  Tx function for the Peer layer. Adds peer header and send the packet to the
  EAP layer.

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_xmit_pkt
(
   eap_peer_handle_type   peer_lyr_handle,
   dsm_item_type        **sdu
)
{
  eap_peer_frm_hdr_type   peer_frm_hdr; 
  uint16                  write_bytes;
  EAP_DIAG_EVENT_ALLOC (eap_diag_event);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the Peer layer handle
  -------------------------------------------------------------------------*/

  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );

    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    In raw mode, just the packet down the stack to the registered client
  -------------------------------------------------------------------------*/
  if( TRUE == eap_peer_lyr_cb[peer_lyr_handle]->raw_mode )
  {
    eap_eap_xmit_pkt(eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
                     EAP_MIN_CODE,
                     sdu);

    return;
  }

  if( EAP_DEFAULT_MTU_SIZE < dsm_length_packet(*sdu) )
  {
    /*-----------------------------------------------------------------------
      Peer layer enforces the MTU
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO1_0("EAP method using MTU higher than permissible" );

    // dsm_free_packet( sdu );
    // return;
  }

  /*-------------------------------------------------------------------------
    Record the Response event
  -------------------------------------------------------------------------*/
  EAP_DIAG_EVENT_SET( 
    &eap_diag_event,
    (int32)(eap_peer_lyr_cb[peer_lyr_handle]->current_method_type),
    (uint8)EAP_PEER_DIAG_RSP_EV,
    (uint8)eap_peer_lyr_cb[peer_lyr_handle]->state,
    (uint8)EAP_DIAG_EVENT_SUCCESS );

  EAP_DIAG_EVENT_REPORT( eap_diag_event );

  /*-------------------------------------------------------------------------
    Initialize pkt type to what is in Peer control block
  -------------------------------------------------------------------------*/
  peer_frm_hdr.type = 
    (uint8)eap_peer_lyr_cb[peer_lyr_handle]->current_method_type;

  /*-------------------------------------------------------------------------
    Determine what to do with the packet
  -------------------------------------------------------------------------*/
  switch(eap_peer_lyr_cb[peer_lyr_handle]->state)
  {
    case EAP_PEER_NOTIFYING_S:
      peer_frm_hdr.type = (uint8)EAP_PEER_NOTIFICATION_TYPE;
  
      /* Fall through to send the packet */
    
    case EAP_PEER_INIT_S:
    case EAP_PEER_AUTHENTICATING_S:
  
      /*---------------------------------------------------------------------
        Send RESPONSE down to EAP layer
      ---------------------------------------------------------------------*/
      write_bytes = dsm_pushdown_packed(sdu,
                                        (void *)&peer_frm_hdr,
                                        sizeof(eap_peer_frm_hdr_type),
                                        DSM_DS_SMALL_ITEM_POOL );
  
      if( sizeof(eap_peer_frm_hdr_type) != write_bytes )
      {
        LOG_MSG_ERROR_0("Failed to pushdown Peer header" );
  
        dsm_free_packet( sdu );
        return;
      }
  
      eap_eap_xmit_pkt(eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
                       EAP_RESPONSE_CODE,
                       sdu);
      break;
  
    case EAP_PEER_CLOSING_S:
      /*---------------------------------------------------------------------
        No packets should be transmitted in this state: discard silently
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0("Discarding outgoing EAP response in Closing State");
      dsm_free_packet(sdu);
      break;

    case EAP_PEER_CLOSED_S:
      /*---------------------------------------------------------------------
        No packets should be transmitted in this state: discard silently
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0("Discarding outgoing EAP response in CLOSED State");
      dsm_free_packet(sdu);
      break;

    default:
      /*---------------------------------------------------------------------
        SANITY check
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Unknown EAP Peer state %d",
                      eap_peer_lyr_cb[peer_lyr_handle]->state );
      dsm_free_packet(sdu);

  } /* end switch */


} /* eap_peer_xmit_pkt()  */


/*===========================================================================

FUNCTION EAP_PEER_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify Peer of
  the result of the authentication process.
  
  The Peer layer still waits for either a Success/Failure packet or a timeout
  before declaring an authentication indication to EAP layer (and therefore
  lower layer)

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_auth_complete_ind 
(
  eap_peer_handle_type     peer_lyr_handle,
  eap_result_enum_type     result,
  uint8                   *pre_master_key,
  uint16                   pre_master_key_len
)
{
  EAP_DIAG_EVENT_ALLOC( eap_diag_event );
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the Peer layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );

    return;
  }

  /*-------------------------------------------------------------------------
    Record the Response event
  -------------------------------------------------------------------------*/
  EAP_DIAG_EVENT_SET( 
    &eap_diag_event,
    (int32)eap_peer_lyr_cb[peer_lyr_handle]->current_method_type,
    (uint8)EAP_PEER_DIAG_AUTH_LOCAL_IND_EV,
    (uint8)eap_peer_lyr_cb[peer_lyr_handle]->state,
    (uint8)result );

  EAP_DIAG_EVENT_REPORT( eap_diag_event );


  /*-------------------------------------------------------------------------
    Store the authentication information in control block
  -------------------------------------------------------------------------*/
  eap_peer_lyr_cb[peer_lyr_handle]->auth_info.result = result;

  if( EAP_MAX_PRE_MASTER_KEY_SIZE >= pre_master_key_len )
  {
  eap_peer_lyr_cb[peer_lyr_handle]->auth_info.pre_master_key_len = 
    pre_master_key_len;
  }
  else
  {
    LOG_MSG_ERROR_0("EAP Peer received larger pre-master key than expected" );
    eap_peer_lyr_cb[peer_lyr_handle]->auth_info.pre_master_key_len =
      EAP_MAX_PRE_MASTER_KEY_SIZE;
  }

  if( NULL != pre_master_key)
  {
    memscpy(eap_peer_lyr_cb[peer_lyr_handle]->auth_info.pre_master_key,
           EAP_MAX_PRE_MASTER_KEY_SIZE,
           pre_master_key,
           eap_peer_lyr_cb[peer_lyr_handle]->auth_info.pre_master_key_len );
  }

  /*----------------------------------------------------------------------- 
    Authenticator Rsp Required can be set dynamically - need to guard reading
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( &eap_peer_crit_sect );
  if( FALSE ==  eap_peer_lyr_cb[peer_lyr_handle]->authenticator_rsp_required )
  {
    /* Leave here if response required is false*/
    PS_LEAVE_CRIT_SECTION( &eap_peer_crit_sect );
    eap_peeri_transition_state( peer_lyr_handle, 
                                EAP_PEER_CLOSED_S);

    /*-----------------------------------------------------------------------
       No need to do any further processing in EAP
    -----------------------------------------------------------------------*/
    eap_eap_auth_complete_ind( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
                               eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                               result,
                               eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                               pre_master_key,
                               eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                               pre_master_key_len );


    return;
  }


  /* Leave here if response required is true*/
  PS_LEAVE_CRIT_SECTION( &eap_peer_crit_sect );
  /*-------------------------------------------------------------------------
    Process the corresponding event
  -------------------------------------------------------------------------*/
  eap_peeri_process_event( peer_lyr_handle,
                           EAP_PEER_AUTH_IND_EV);

} /* eap_peer_auth_complete_ind() */

/*===========================================================================

FUNCTION EAP_PEER_SUPP_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify Peer of
  the supplementary info of the authentication process.

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_supp_auth_complete_ind 
(
  eap_peer_handle_type             peer_lyr_handle,
  eap_result_enum_type             result,
  eap_sim_aka_supp_auth_mask_type  supp_auth_info_mask,
  uint16                           supp_auth_info_size,
  void                            *supp_auth_info_ptr
)
{
  
  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the Peer layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  /*-----------------------------------------------------------------------
     No need to do any further processing in EAP
  -----------------------------------------------------------------------*/
  eap_eap_supp_auth_complete_ind(eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
                                 result,
                                 supp_auth_info_mask,
                                 supp_auth_info_size,
                                 supp_auth_info_ptr );
  return;

} /* eap_peer_supp_auth_complete_ind() */

/*===========================================================================
FUNCTION EAP_PEER_TRANSACTION_CLOSING

DESCRIPTION
  This function is called by SIM and AKA modules to move the peer to
  CLOSING state whereupon it can process only SUCCESS, FAILURE and
  Notification packets.

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_transaction_closing
(
  eap_peer_handle_type  peer_lyr_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_0("eap_peer_transaction_closing(): called" );
  /*-------------------------------------------------------------------------
    Check for handle validity
  -------------------------------------------------------------------------*/
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  /*-------------------------------------------------------------------------
    Forcing state to move to CLOSING, so that success and failure packets
    can be handled appropriately.
  -------------------------------------------------------------------------*/
  eap_peeri_process_event( peer_lyr_handle,
                           EAP_PEER_SUCCESS_EV);
} /* eap_peer_transaction_closing()  */


/*===========================================================================

FUNCTION EAP_PEER_GET_IDENTITY

DESCRIPTION
  This function is called by the Identity method to retrieve the identity
  of the peer

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_get_identity
(
  eap_peer_handle_type  peer_lyr_handle,
  eap_identity_type*    identity
)
{

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the Peer layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) ||
      NULL == identity )
  {
    LOG_MSG_ERROR_1("Invalid Input parameters peer hdl %d", peer_lyr_handle );
    return;
  }

  memscpy( identity->name, EAP_IDENTITY_MAX_CHAR,
          eap_peer_lyr_cb[peer_lyr_handle]->user_id.name,
          eap_peer_lyr_cb[peer_lyr_handle]->user_id.len );
  identity->len = eap_peer_lyr_cb[peer_lyr_handle]->user_id.len;

  return;

} /* eap_peer_get_identity()  */


/*===========================================================================

FUNCTION EAP_PEER_GET_AUTH_PROT

DESCRIPTION
  This function is called by the Nak method to identify the supported
  authentication protocols

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  Number of protocols supported
  0 : no protocols supported
  
  List of protocols supported is provided in buf

SIDE EFFECTS
  None.

===========================================================================*/
uint16 eap_peer_get_auth_prot
(
  eap_peer_handle_type  peer_lyr_handle,
  uint8           *buf
)
{
  int                  method_cnt;
  uint16               buf_index;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the Peer layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );

    return 0;
  }

  if( NULL == buf )
  {
    LOG_MSG_ERROR_0("NULL buf ptr to retrieve EAP peer auth protocol list" );
    return 0;
  }

  /*-------------------------------------------------------------------------
    Initialize
  -------------------------------------------------------------------------*/
  buf_index = 0;

  /*-------------------------------------------------------------------------
    For each of the external methods, if authentication protocol is supported
    add the type to the list to be returned
  -------------------------------------------------------------------------*/
  for( method_cnt = (int)EAP_MIN_EXT_METHOD; method_cnt < (int)EAP_MAX_METHOD;
       method_cnt++ )
  {
    if( 0 < ( (1 << method_cnt) & 
              eap_peer_lyr_cb[peer_lyr_handle]->eap_auth_prot_mask ) )
    {
      buf[buf_index++] = (uint8)eap_method_to_type_map[method_cnt];

    }
  }

  return buf_index;

} /* eap_peer_get_auth_prot() */

/*===========================================================================

FUNCTION EAP_PEER_GET_REAUTH_ID

DESCRIPTION
  Calls the UME layer function with the appropriate mode depending upon
  the method specified in the eap_handle
  
DEPENDENCIES
  None.

RETURN VALUE
  The reauthentication id along with the length if present, -1 otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/
int eap_peer_get_reauth_id
(
  eap_handle_type                        eap_handle,
  uint8*                                 data,
  uint16*                                max_data_len
)
{
  eap_peer_handle_type peer_lyr_handle;
  eap_peer_type_enum_type  current_method_type = EAP_PEER_MIN_TYPE;  
  int                      auth_prot_cnt = 0;
  eap_method_mask_enum_type  auth_prot_cnt_mask = (eap_method_mask_enum_type)0;
  int                      result = -1;
  eap_sim_aka_ume_mode_types mode_type = EAP_SIM_AKA_UME_MODE_AKA;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the Peer layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("eap_peer_get_reauth_id(): Invalid EAP instance" );
    return result;
  }

  peer_lyr_handle = eap_get_peer_lyr_handle(eap_handle);

  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return result;
  }

  /*-----------------------------------------------------------------------
     Obtain the current method type associate with the  peer_lyr_handle
     and retrieve the reauthentication id based on the same.
    -----------------------------------------------------------------------*/

  for( auth_prot_cnt = (int)EAP_MIN_EXT_METHOD; 
       auth_prot_cnt < (int)EAP_MAX_METHOD; auth_prot_cnt++ )
  {
    auth_prot_cnt_mask =  (eap_method_mask_enum_type)(1 << auth_prot_cnt);
    if( ( auth_prot_cnt_mask & 
          eap_peer_lyr_cb[peer_lyr_handle]->eap_auth_prot_mask ) == 
          auth_prot_cnt_mask )
    {
      current_method_type = eap_method_to_type_map[auth_prot_cnt];
      /*-------------------------------------------------------------------
        If the current_method_type is EAP_AKA_METHOD, continue and 
        verify if EAP_AKA_PRIME_METHOD is set as well. In which case,
        the current_method_type will get overwritten to EAP_AKA_PRIME
      -------------------------------------------------------------------*/
      if(current_method_type == EAP_PEER_AKA_TYPE)
      {
        continue;
      }
      break;
    }
  }

  LOG_MSG_INFO2_1("eap_peer_get_reauth_id(): Retrieving reauth id based on"
                  " current type %d", current_method_type);
 
  switch(current_method_type)
  {
    case EAP_PEER_AKA_TYPE:
      mode_type = EAP_SIM_AKA_UME_MODE_AKA;
      break;
    case EAP_PEER_AKA_PRIME_TYPE:
      mode_type = EAP_SIM_AKA_UME_MODE_AKA_PRIME;
      break;
    case EAP_PEER_SIM_TYPE:
      mode_type = EAP_SIM_AKA_UME_MODE_SIM;
      break;
    default:
      LOG_MSG_ERROR_1("eap_peer_get_reauth_id(): Reauth id retrieved for wrong"
                      " current method type %d", current_method_type);
      return result;
  }

  result = eap_sim_aka_ume_get_reauth_id( mode_type, 
                                          data, 
                                          max_data_len,
                                          eap_handle);

  return result;
  
} /* eap_peer_get_reauth_id() */


/*===========================================================================
                        
FUNCTION EAP_PEER_GET_PSEUDONYM_ID

DESCRIPTION
  Calls the UME layer function with the appropriate mode depending upon
  the method specified in the eap_handle
  
DEPENDENCIES
  None.

RETURN VALUE
  The pseudonym id along with the length if present, -1 otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/
int eap_peer_get_pseudonym_id
(
  eap_handle_type                        eap_handle,
  uint8*                                 data,
  uint16*                                max_data_len
)
{
  eap_peer_handle_type       peer_lyr_handle;
  eap_peer_type_enum_type    current_method_type = EAP_PEER_MIN_TYPE;  
  int                        auth_prot_cnt = 0;
  eap_method_mask_enum_type  auth_prot_cnt_mask = (eap_method_mask_enum_type)0;
  int                        result = -1;
  eap_sim_aka_ume_mode_types mode_type = EAP_SIM_AKA_UME_MODE_AKA;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the Peer layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("eap_peer_get_pseudonym_id(): Invalid EAP instance");
    return result;
  }

  peer_lyr_handle = eap_get_peer_lyr_handle(eap_handle);

  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return result;
  }

  /*-----------------------------------------------------------------------
     Obtain the current method type associate with the  peer_lyr_handle
     and retrieve the reauthentication id based on the same.
    -----------------------------------------------------------------------*/

  for( auth_prot_cnt = EAP_MIN_EXT_METHOD; auth_prot_cnt < EAP_MAX_METHOD;
         auth_prot_cnt++ )
  {
    auth_prot_cnt_mask =  (eap_method_mask_enum_type)(1 << auth_prot_cnt);
    if( ( auth_prot_cnt_mask & 
          eap_peer_lyr_cb[peer_lyr_handle]->eap_auth_prot_mask ) == 
          auth_prot_cnt_mask )
    {
      current_method_type = eap_method_to_type_map[auth_prot_cnt];
      /*-------------------------------------------------------------------
        If the current_method_type is EAP_AKA_METHOD, continue and 
        verify if EAP_AKA_PRIME_METHOD is set as well. In which case,
        the current_method_type will get overwritten to EAP_AKA_PRIME
      -------------------------------------------------------------------*/
      if(current_method_type == EAP_PEER_AKA_TYPE)
      {
        continue;
      }
      break;
    }
  }

  LOG_MSG_INFO2_1("eap_peer_get_pseudonym_id(): Retrieving pseudonym id based on"
                  " current type %d", current_method_type);
 
  switch(current_method_type)
  {
    case EAP_PEER_AKA_TYPE:
      mode_type = EAP_SIM_AKA_UME_MODE_AKA;
      break;
    case EAP_PEER_AKA_PRIME_TYPE:
      mode_type = EAP_SIM_AKA_UME_MODE_AKA_PRIME;
      break;
    case EAP_PEER_SIM_TYPE:
      mode_type = EAP_SIM_AKA_UME_MODE_SIM;
      break;
    default:
      LOG_MSG_ERROR_1("eap_peer_get_pseudonym_id(): Pseudonym id retrieved for "
                      "wrong current method type %d", current_method_type);
    return result;
  }

  result = eap_sim_aka_ume_get_pseudonym_id( mode_type, 
                                             data, 
                                             max_data_len,
                                             eap_handle);

  return result;
  
} /* eap_peer_get_pseudonym_id() */

/*===========================================================================
                        
                        INTERNAL FUNCTION DEFINITIONS
                        
===========================================================================*/


/*===========================================================================

FUNCTION EAP_PEERI_REQUEST_INPUT

DESCRIPTION
  Function to receive the request packet

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_request_input
( 
  eap_peer_handle_type   peer_lyr_handle,
  dsm_item_type        **sdu
)
{
  eap_peer_frm_hdr_type  peer_frm_hdr;
  uint16                 bytes_read;
  EAP_DIAG_EVENT_ALLOC ( eap_diag_event );
  eap_method_enum_type   requested_method;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Retrieve the header from the DSM item
  -------------------------------------------------------------------------*/
  bytes_read = dsm_pullup( sdu,
                           (void *)&peer_frm_hdr,
                           sizeof(eap_peer_frm_hdr_type) );

  if( sizeof(eap_peer_frm_hdr_type) > bytes_read )
  {
    LOG_MSG_INFO2_0("Malformed packet received, freeing" );
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Record the Request event
  -------------------------------------------------------------------------*/
  EAP_DIAG_EVENT_SET( 
    &eap_diag_event,
    (int32)peer_frm_hdr.type,
    (uint8)EAP_PEER_DIAG_REQ_EV,
    (uint8)eap_peer_lyr_cb[peer_lyr_handle]->state,
    (uint8)EAP_DIAG_EVENT_SUCCESS );

  EAP_DIAG_EVENT_REPORT( eap_diag_event );


  /*-------------------------------------------------------------------------
    Store the request packet and type for later use.
    
    NOTE: The event handlers are responsible for discarding packet or
    passing it up to the methods
    
    TODO: Replace this storage mechanism by passing in the values to the
    event functions...
  -------------------------------------------------------------------------*/
  if (eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt)
  {
    LOG_MSG_INFO2_1("eap_peeri_request_input(): Freeing cached pending pkt for %d"
                    " instance", peer_lyr_handle);
    dsm_free_packet(&eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt);
  }

  eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt = *sdu;
  *sdu = NULL;

  eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt_type =
    (eap_peer_type_enum_type) peer_frm_hdr.type;

  /*-------------------------------------------------------------------------
    Determine the event and process it - each request corresponds to a 
    particular event
    
    NOTE: Per RFC, NAK can *only* be a response and hence is not processed
    below in request...
    If an unsupported auth type is received, a NAK has to be sent by peer
  -------------------------------------------------------------------------*/
  switch( peer_frm_hdr.type )
  {
    /*-----------------------------------------------------------------------
      Logic:
      If a request for a supported type is received, forward to the relevant
      method and return immediately.
      
      Otherwise, break to send a NAK response.
    -----------------------------------------------------------------------*/
    case EAP_PEER_IDENTITY_TYPE:
      LOG_MSG_INFO2_0("eap_peeri_request_input(): IDENTITY pkt rcvd");
      eap_peeri_process_event( peer_lyr_handle,
                               EAP_PEER_IDENTITY_REQ_EV);
      return;
  
    case EAP_PEER_NOTIFICATION_TYPE:
      LOG_MSG_INFO2_0("eap_peeri_request_input(): NOTIFICATION pkt rcvd");
      /*---------------------------------------------------------------------
        TODO: 
        Per RFC, some methods MAY prohibit Notification in their 
        specification - Peer must silently discard in such a case.
        
        For now, we always process Notification; however, if we ever support
        a Method which prohibits this, we can handle it here by checking
        against the current method type.
      ---------------------------------------------------------------------*/
      eap_peeri_process_event( peer_lyr_handle,
                               EAP_PEER_NOTIFY_REQ_EV);
      return;
    
    case EAP_PEER_TLS_TYPE:
    case EAP_PEER_SIM_TYPE:
    case EAP_PEER_AKA_TYPE:
    case EAP_PEER_AKA_PRIME_TYPE:
    case EAP_PEER_MD5_CHALLENGE_TYPE:
    case EAP_PEER_TTLS_TYPE:
    case EAP_PEER_PEAP_TYPE:
    case EAP_PEER_MSCHAP_V2_TYPE:
    case EAP_PEER_TLV_TYPE:
    case EAP_PEER_EXT_TYPE:
      LOG_MSG_INFO2_1("eap_peeri_request_input(): AUTH %d type pkt rcvd",
                      peer_frm_hdr.type);

      /*---------------------------------------------------------------------
        Store the type for processing the auth request
        
        Convert the RFC-defined type into local method.
        
        Check if this particular auth method has been enabled for this
        EAP instance
      ---------------------------------------------------------------------*/
      requested_method = (eap_method_enum_type)
        eap_peeri_get_method_from_type(
            (eap_peer_type_enum_type) peer_frm_hdr.type);

      if( EAP_MAX_METHOD == requested_method )
      {
        LOG_MSG_ERROR_0("eap_peeri_request_input(): Invalid request method");
        return;
      }

      if( 0 < ( (1 << (uint32)requested_method) & 
                eap_peer_lyr_cb[peer_lyr_handle]->eap_auth_prot_mask ) )
      {
        eap_peeri_process_event( peer_lyr_handle,
                                 EAP_PEER_AUTH_REQ_EV);

        return;
      }
      break;
    
    /*-----------------------------------------------------------------------
      Unsupported EAP types:
      Per RFC: Send a NAK response with the supported authentication types
      We should gracefully recover from this
    -----------------------------------------------------------------------*/
    default:
      LOG_MSG_INFO1_1("eap_peeri_request_input(): unsupported %d type rcvd",
                      peer_frm_hdr.type );
      break;
  
  } /* end switch */

  /*-------------------------------------------------------------------------
    If we reached this point, we received request for an unsupported type
    Send a NAK
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_1("Unsupported EAP request type : %d received by peer",
                  peer_frm_hdr.type );
  
  eap_peeri_process_event( peer_lyr_handle,
                           EAP_PEER_NAK_EV);

} /* eap_peeri_request_input()  */


/*===========================================================================

FUNCTION EAP_PEERI_SUCCESS_INPUT

DESCRIPTION
  Function to receive the success packet

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_success_input
( 
  eap_peer_handle_type   peer_lyr_handle,
  dsm_item_type        **sdu
)
{
  EAP_DIAG_EVENT_ALLOC( eap_diag_event );

  eap_method_enum_type curr_method;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );

    return;
  }

  /*-------------------------------------------------------------------------
    A success packet has no further info to process
  -------------------------------------------------------------------------*/
  dsm_free_packet( sdu );

  /*---------------------------------------------------------------------
    Set the authenticator response required to FALSE.        
  ---------------------------------------------------------------------*/
  eap_set_authenticator_rsp_required( 
    eap_peer_lyr_cb[peer_lyr_handle]->eap_handle, FALSE);


  /*-------------------------------------------------------------------------
    Record the server success ind event
  -------------------------------------------------------------------------*/
  EAP_DIAG_EVENT_SET( 
    &eap_diag_event,
    (int32)eap_peer_lyr_cb[peer_lyr_handle]->current_method_type,
    (uint8)EAP_PEER_DIAG_AUTH_SERVER_IND_EV,
    (uint8)eap_peer_lyr_cb[peer_lyr_handle]->state,
    (uint8)EAP_DIAG_EVENT_SUCCESS );

  EAP_DIAG_EVENT_REPORT( eap_diag_event );

  eap_peer_lyr_cb[peer_lyr_handle]->auth_info.result = EAP_RESULT_SUCCESS;

  /*-----------------------------------------------------------------------
    For ease of understanding code: convert the current type into method
  -----------------------------------------------------------------------*/
  curr_method = eap_peeri_get_method_from_type(
                       eap_peer_lyr_cb[peer_lyr_handle]->current_method_type);

  /*-----------------------------------------------------------------------
    Identify the current method and invoke its result callback
  -----------------------------------------------------------------------*/
  if( (EAP_MAX_METHOD != curr_method) &&
      (NULL != eap_method_result_cback[curr_method]) )
  {
    eap_method_result_cback[curr_method]
          ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle, 
            eap_peer_lyr_cb[peer_lyr_handle]->auth_info.result );
    /*-----------------------------------------------------------------------
      After method result callback and client callback, eap_handle may
      be deleted, need to re-verify peer lyr handle
    -----------------------------------------------------------------------*/
    if( (TRUE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) ) &&
        (NULL != eap_method_supp_result_cback[curr_method]) &&
        (TRUE == eap_eap_supp_result_f_ind_ptr_is_valid(
                              eap_peer_lyr_cb[peer_lyr_handle]->eap_handle)) )
    {
      eap_method_supp_result_cback[curr_method]
          ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle, 
            eap_peer_lyr_cb[peer_lyr_handle]->auth_info.result );    
    }
  }
  else
  {
    /*--------------------------------------------------------------------
    Post the success to peer
    ----------------------------------------------------------------------*/
    eap_peeri_process_event( peer_lyr_handle,
                           EAP_PEER_SUCCESS_EV);
  }

} /* eap_peeri_success_input()  */


/*===========================================================================

FUNCTION EAP_PEERI_FAILURE_INPUT

DESCRIPTION
  Function to receive the failure packet

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_failure_input
( 
  eap_peer_handle_type   peer_lyr_handle,
  dsm_item_type        **sdu
)
{
  EAP_DIAG_EVENT_ALLOC( eap_diag_event );

  eap_method_enum_type curr_method;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    A failure packet has no further info to process
  -------------------------------------------------------------------------*/
  if ( sdu != NULL )
  {
    dsm_free_packet( sdu );
  }

  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  /*---------------------------------------------------------------------
    Set the authenticator response required to FALSE.        
  ---------------------------------------------------------------------*/
  eap_set_authenticator_rsp_required( 
    eap_peer_lyr_cb[peer_lyr_handle]->eap_handle, FALSE);


  /*-------------------------------------------------------------------------
    Record the server failure ind event
  -------------------------------------------------------------------------*/
  EAP_DIAG_EVENT_SET( 
    &eap_diag_event,
    (int32)eap_peer_lyr_cb[peer_lyr_handle]->current_method_type,
    (uint8)EAP_PEER_DIAG_AUTH_SERVER_IND_EV,
    (uint8)eap_peer_lyr_cb[peer_lyr_handle]->state,
    (uint8)EAP_DIAG_EVENT_FAILURE );

  EAP_DIAG_EVENT_REPORT( eap_diag_event );

  eap_peer_lyr_cb[peer_lyr_handle]->auth_info.result = EAP_RESULT_FAILURE;

  /*-----------------------------------------------------------------------
    For ease of understanding code: convert the current type into method
  -----------------------------------------------------------------------*/
  curr_method = eap_peeri_get_method_from_type(
                       eap_peer_lyr_cb[peer_lyr_handle]->current_method_type);

  /*-----------------------------------------------------------------------
    Identify the current method and invoke its result callback
  -----------------------------------------------------------------------*/
  if( (EAP_MAX_METHOD != curr_method) &&
      (NULL != eap_method_result_cback[curr_method]) )
  {
    eap_method_result_cback[curr_method]
          ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle, 
            eap_peer_lyr_cb[peer_lyr_handle]->auth_info.result );

    /*-----------------------------------------------------------------------
      After method result callback and client callback, eap_handle may
      be deleted, need to re-verify peer lyr handle
    -----------------------------------------------------------------------*/
    if( (TRUE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) ) &&
        (NULL != eap_method_supp_result_cback[curr_method]) &&
        (TRUE == eap_eap_supp_result_f_ind_ptr_is_valid(
                              eap_peer_lyr_cb[peer_lyr_handle]->eap_handle)) )
    {
      eap_method_supp_result_cback[curr_method]
          ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle, 
            eap_peer_lyr_cb[peer_lyr_handle]->auth_info.result );    
    }
  }
  else
  {
    /*-------------------------------------------------------------------------
      Post the failure to peer
    -------------------------------------------------------------------------*/
    eap_peeri_process_event( peer_lyr_handle,
                             EAP_PEER_FAILURE_EV);
  }

} /* eap_peeri_failure_input()  */


/*===========================================================================

FUNCTION EAP_PEERI_PROCESS_EVENT

DESCRIPTION
  Function to process an event to the Peer

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_process_event
(
  eap_peer_handle_type   peer_lyr_handle,
  eap_peer_ev_enum_type  event
)
{
  eap_peer_state_enum_type state;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("eap_peeri_process_event(): Invalid Peer layer instance" );
    return;
  }

  /*-------------------------------------------------------------------------
    Use a smaller local name for *readability*
  -------------------------------------------------------------------------*/
  state = eap_peer_lyr_cb[peer_lyr_handle]->state;

  /*-------------------------------------------------------------------------
    Check validity of state
  -------------------------------------------------------------------------*/
  if(EAP_PEER_MIN_S >= state || EAP_PEER_MAX_S <= state)
  {
    LOG_MSG_ERROR_0("Invalid state" );
    return;
  }
    
  if(EAP_PEER_MIN_EV >= event || EAP_PEER_MAX_EV <= event)
  {
    LOG_MSG_ERROR_0("Invalid event");
    return;
  }

  LOG_MSG_INFO2_2("eap_peeri_process_event(): state %d, event %d",
                  state, event);

  if( NULL != eap_peer_state_ev_fnc[state][event] )
  {
    LOG_MSG_INFO2_1("Posting event %d to EAP peer", event);
    eap_peer_state_ev_fnc[state][event](peer_lyr_handle);
  }


} /* eap_peeri_process_event()  */


/*===========================================================================

FUNCTION EAP_PEERI_TRANSITION_STATE

DESCRIPTION
  Function to change the state of the Peer

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_transition_state
(
  eap_peer_handle_type      peer_lyr_handle,
  eap_peer_state_enum_type  state
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  /*-------------------------------------------------------------------------
    Check validity of state
  -------------------------------------------------------------------------*/
  if( (EAP_PEER_MIN_S > state) || (EAP_PEER_MAX_S < state) )
  {
    LOG_MSG_ERROR_0("Invalid state change requested" );
    return;
  }

  LOG_MSG_INFO2_2("Changing state from %d --> %d",
                  eap_peer_lyr_cb[peer_lyr_handle]->state, state);

  eap_peer_lyr_cb[peer_lyr_handle]->state = state;

} /* eap_peeri_transition_state() */


/*===========================================================================

FUNCTION EAP_PEERI_CLEANUP

DESCRIPTION
  Event handler for all events which need a cleanup
  
  This is equivalent to a NULL handler, except that it frees the pending
  pkt, which normally would have been processed by a method, but in this case
  no method is suitable to handle the packet.

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  Frees the pending pkt in the Peer

===========================================================================*/
static void eap_peeri_cleanup
(
  eap_peer_handle_type peer_lyr_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  LOG_MSG_INFO2_1("Cleaning up peer handle in state %d",
                  eap_peer_lyr_cb[peer_lyr_handle]->state);
  
  dsm_free_packet( &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );

  /*-------------------------------------------------------------------------
    NO state change
  -------------------------------------------------------------------------*/


} /* eap_peeri_cleanup()  */


/*===========================================================================

FUNCTION EAP_PEERI_A_N_AUTH_IND

DESCRIPTION
  Event handler for an auth indication in Auth_S and notifying_s
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_a_n_auth_ind
(
  eap_peer_handle_type peer_lyr_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  LOG_MSG_INFO2_2("eap_peeri_a_n_auth_ind: Changing state from %d --> %d",
                  eap_peer_lyr_cb[peer_lyr_handle]->state, EAP_PEER_CLOSING_S);
  /*-------------------------------------------------------------------------
    Do NOT change method type as authentication has already started
  -------------------------------------------------------------------------*/

  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_CLOSING_S);

} /* eap_peeri_a_n_auth_ind()  */


/*===========================================================================

FUNCTION EAP_PEERI_A_N_RESULT

DESCRIPTION
  Event handler for a server result in Auth_S and notifying_s
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_a_n_result
(
  eap_peer_handle_type peer_lyr_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  LOG_MSG_INFO2_2("eap_peeri_a_n_result: Changing state from %d --> %d",
                  eap_peer_lyr_cb[peer_lyr_handle]->state, EAP_PEER_CLOSING_S);
  /*-------------------------------------------------------------------------
    Log the fact that server responded before upper layers were able to
    complete procedure
  -------------------------------------------------------------------------*/
  if( FALSE != eap_peer_lyr_cb[peer_lyr_handle]->recvd_server_result )
  {
    LOG_MSG_ERROR_0("eap_peeri_a_n_auth_ind(): Server Result already received "); 
    return;
  }

  eap_peer_lyr_cb[peer_lyr_handle]->recvd_server_result = TRUE;

  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_CLOSING_S);

} /* eap_peeri_a_n_auth_ind()  */


/*===========================================================================

FUNCTION EAP_PEERI_I_NAK

DESCRIPTION
  Event handler for NAK event in Init_S

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_i_nak
(
  eap_peer_handle_type peer_lyr_handle
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }
      
  /*-------------------------------------------------------------------------
    Set the current method to NAK
  -------------------------------------------------------------------------*/
  eap_peer_lyr_cb[peer_lyr_handle]->current_method_type = EAP_PEER_NAK_TYPE;

  /*-------------------------------------------------------------------------
    Stay in Init_s
  -------------------------------------------------------------------------*/
  eap_peeri_transition_state( peer_lyr_handle, 
                              EAP_PEER_INIT_S);

  if( NULL != eap_method_input_cback[EAP_NAK_METHOD] )
  {
    /*-----------------------------------------------------------------------
      Pass the pending pkt to NAK method for further processing
    -----------------------------------------------------------------------*/
    eap_method_input_cback[EAP_NAK_METHOD](eap_peer_lyr_cb[peer_lyr_handle]->
                                           eap_handle,
                                           &eap_peer_lyr_cb[peer_lyr_handle]->
                                           pending_pkt);
  }

} /* eap_peeri_i_nak()  */


/*===========================================================================

FUNCTION EAP_PEERI_I_IDENTITY_REQ

DESCRIPTION
  Event handler for an identity request in Init_s
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_i_identity_req
(
  eap_peer_handle_type peer_lyr_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  eap_peer_lyr_cb[peer_lyr_handle]->current_method_type = 
    EAP_PEER_IDENTITY_TYPE;

  /*-------------------------------------------------------------------------
    Stay in Init_s
  -------------------------------------------------------------------------*/
  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_INIT_S);
  
  if( NULL != eap_method_input_cback[EAP_IDENTITY_METHOD] )
  {
    /*-----------------------------------------------------------------------
      Pass the pending packet up to Identity method
    -----------------------------------------------------------------------*/
    eap_method_input_cback[EAP_IDENTITY_METHOD]
      ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
        &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
  }

} /* eap_peeri_i_identity_req()  */


/*===========================================================================

FUNCTION EAP_PEERI_I_NOTIFY_REQ

DESCRIPTION
  Event handler for a notify request in Init_s
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_i_notify_req
(
  eap_peer_handle_type peer_lyr_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }
  
  eap_peer_lyr_cb[peer_lyr_handle]->current_method_type = 
    EAP_PEER_NOTIFICATION_TYPE;

  /*-------------------------------------------------------------------------
    Stay in Init_s
  -------------------------------------------------------------------------*/
  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_INIT_S);
  
  if( NULL != eap_method_input_cback[EAP_NOTIFICATION_METHOD] )
  {
    /*-----------------------------------------------------------------------
      Pass the pending packet up to notification method
    -----------------------------------------------------------------------*/
    eap_method_input_cback[EAP_NOTIFICATION_METHOD]
      ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
        &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
  }

} /* eap_peeri_i_notify_req()  */


/*===========================================================================

FUNCTION EAP_PEERI_I_AUTH_REQ

DESCRIPTION
  Event handler for an authentication request in Init_s
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_i_auth_req
(
  eap_peer_handle_type peer_lyr_handle
)
{
  eap_method_enum_type requested_method;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  eap_peer_lyr_cb[peer_lyr_handle]->current_method_type = 
    eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt_type;

  requested_method = eap_peeri_get_method_from_type( 
    eap_peer_lyr_cb[peer_lyr_handle]->current_method_type );

  if( EAP_MAX_METHOD == requested_method )
  {
    LOG_MSG_ERROR_0("eap_peeri_i_auth_req(): Invalid EAP method");
    return; 
  }

  /*-------------------------------------------------------------------------
    Starting authentication
  -------------------------------------------------------------------------*/
  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_AUTHENTICATING_S);
  
  if( NULL != eap_method_input_cback[requested_method] )
  {
    /*-----------------------------------------------------------------------
      Pass the pending packet up to the concerned method
    -----------------------------------------------------------------------*/
    eap_method_input_cback[requested_method]
      ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
        &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
  }

} /* eap_peeri_i_auth_req()  */


/*===========================================================================

FUNCTION EAP_PEERI_A_NOTIFY_REQ

DESCRIPTION
  Event handler for a notify request in Auth_S
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_a_notify_req
(
  eap_peer_handle_type peer_lyr_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }
  
  /*-------------------------------------------------------------------------
    Do NOT change method type as authentication has already started
  -------------------------------------------------------------------------*/

  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_NOTIFYING_S);
  

  if( NULL != eap_method_input_cback[EAP_NOTIFICATION_METHOD] )
  {
    /*-----------------------------------------------------------------------
      Pass the pending packet up to Notification method
    -----------------------------------------------------------------------*/
     eap_method_input_cback[EAP_NOTIFICATION_METHOD]
        ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
          &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
  }

} /* eap_peeri_a_notify_req()  */


/*===========================================================================

FUNCTION EAP_PEERI_A_AUTH_REQ

DESCRIPTION
  Event handler for an auth request in Auth_s
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_a_auth_req
(
  eap_peer_handle_type peer_lyr_handle
)
{
  eap_method_enum_type requested_method;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }
    
  /*-------------------------------------------------------------------------
    Ensure that the current type we are expecting matches that of the 
    pending pkt we wish to process
    
    Otherwise, silently discard, per RFC
  -------------------------------------------------------------------------*/
  if( eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt_type != 
      eap_peer_lyr_cb[peer_lyr_handle]->current_method_type )
  {
    /*-----------------------------------------------------------------------
      Different authentication method already in progress
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO2_0("Invalid EAP auth request from Authenticator" );

    dsm_free_packet( &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
    return;
  }

  requested_method = eap_peeri_get_method_from_type( 
    eap_peer_lyr_cb[peer_lyr_handle]->current_method_type );

  if( EAP_MAX_METHOD == requested_method )
  {
    LOG_MSG_ERROR_0("eap_peeri_a_auth_req(): Invalid EAP method");
    return; 
  }

  /*-------------------------------------------------------------------------
    Stay in authenticating_s
  -------------------------------------------------------------------------*/
  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_AUTHENTICATING_S);
  
  if( NULL != eap_method_input_cback[requested_method] )
  {
    /*-----------------------------------------------------------------------
      Pass the pending packet up to the concerned method
    -----------------------------------------------------------------------*/
    eap_method_input_cback[requested_method]
      ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
        &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
  }

} /* eap_peeri_a_auth_req()  */


/*===========================================================================

FUNCTION EAP_PEERI_N_NOTIFY_REQ

DESCRIPTION
  Event handler for a notify request in Notifying_S
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_n_notify_req
(
  eap_peer_handle_type peer_lyr_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }
    
  /*-------------------------------------------------------------------------
    Do NOT change method type as authentication has already started
  -------------------------------------------------------------------------*/

  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_NOTIFYING_S);
  
  if( NULL != eap_method_input_cback[EAP_NOTIFICATION_METHOD] )
  {
    /*-----------------------------------------------------------------------
      Pass the pending packet up to Notification method
    -----------------------------------------------------------------------*/
    eap_method_input_cback[EAP_NOTIFICATION_METHOD]
      ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
        &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
  }

} /* eap_peeri_n_notify_req()  */


/*===========================================================================

FUNCTION EAP_PEERI_N_AUTH_REQ

DESCRIPTION
  Event handler for an AUTH request in Notifying_s
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_n_auth_req
(
  eap_peer_handle_type peer_lyr_handle
)
{
  eap_method_enum_type requested_method;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }  

  /*-------------------------------------------------------------------------
    Ensure that the current type we are expecting matches that of the 
    pending pkt we wish to process
    
    Otherwise, silently discard, per RFC
  -------------------------------------------------------------------------*/
  if( eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt_type != 
      eap_peer_lyr_cb[peer_lyr_handle]->current_method_type )
  {
    /*-----------------------------------------------------------------------
      Different authentication method already in progress
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO2_0("Invalid EAP auth request from Authenticator" );

    dsm_free_packet( &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
    return;
  }

  requested_method = eap_peeri_get_method_from_type( 
    eap_peer_lyr_cb[peer_lyr_handle]->current_method_type );

  if( EAP_MAX_METHOD == requested_method )
  {
    LOG_MSG_ERROR_0("eap_peeri_n_auth_req(): Invalid EAP method");
    return; 
  }

  /*-------------------------------------------------------------------------
    Move back to authenticating_s
  -------------------------------------------------------------------------*/
  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_AUTHENTICATING_S);
  
  if( NULL != eap_method_input_cback[requested_method] )
  {
    /*-----------------------------------------------------------------------
      Pass the pending packet up to the concerned method
    -----------------------------------------------------------------------*/
    eap_method_input_cback[requested_method]
      ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
        &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
  }

} /* eap_peeri_n_auth_req()  */


/*===========================================================================

FUNCTION EAP_PEERI_C_NOTIFY_REQ

DESCRIPTION
  Event handler for an notify request in Closing_s
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_c_notify_req
(
  eap_peer_handle_type peer_lyr_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }

  /*-------------------------------------------------------------------------
    Stay in notifying_s
  -------------------------------------------------------------------------*/
  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_NOTIFYING_S);
  
  if( NULL != eap_method_input_cback[EAP_NOTIFICATION_METHOD] )
  {
    /*-----------------------------------------------------------------------
      Pass the pending packet up to Notification method
    -----------------------------------------------------------------------*/
    eap_method_input_cback[EAP_NOTIFICATION_METHOD]
      ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
        &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
  }

  eap_peeri_transition_state (peer_lyr_handle, EAP_PEER_CLOSING_S);
} /* eap_peeri_c_notify_req()  */


/*===========================================================================

FUNCTION EAP_PEERI_C_AUTH_REQ

DESCRIPTION
  Event handler for an auth request in Closing_s
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_c_auth_req
(
  eap_peer_handle_type peer_lyr_handle
)
{
  eap_method_enum_type requested_method;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }
    
  /*-------------------------------------------------------------------------
    Ensure that the current type we are expecting matches that of the 
    pending pkt we wish to process
    
    Otherwise, silently discard, per RFC
  -------------------------------------------------------------------------*/
  if( eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt_type != 
      eap_peer_lyr_cb[peer_lyr_handle]->current_method_type )
  {
    /*-----------------------------------------------------------------------
      Different authentication method already in progress
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("Invalid EAP auth request from Authenticator" );

    dsm_free_packet( &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
    return;
  }

  requested_method = eap_peeri_get_method_from_type( 
    eap_peer_lyr_cb[peer_lyr_handle]->current_method_type );

  if( EAP_MAX_METHOD == requested_method )
  {
    LOG_MSG_ERROR_0("eap_peeri_c_auth_req(): Invalid EAP method");
    return; 
  }

  /*-------------------------------------------------------------------------
    Stay in authenticating_s
  -------------------------------------------------------------------------*/
  eap_peer_lyr_cb[peer_lyr_handle]->recvd_server_result = FALSE;
  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_AUTHENTICATING_S);
  
  if( NULL != eap_method_input_cback[requested_method] )
  {
    /*-----------------------------------------------------------------------
      Pass the pending packet up to the concerned method
    -----------------------------------------------------------------------*/
    eap_method_input_cback[requested_method]
      ( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
        &eap_peer_lyr_cb[peer_lyr_handle]->pending_pkt );
  }

} /* eap_peeri_c_auth_req()  */

/*===========================================================================

FUNCTION EAP_PEERI_C_AUTH_IND

DESCRIPTION
  Event handler for an auth indication in Closing_S
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_c_auth_ind
(
  eap_peer_handle_type peer_lyr_handle
)
{
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }
    
  /*-------------------------------------------------------------------------
    Check if we already received server response:
    If not, there is an error
  -------------------------------------------------------------------------*/
  if( TRUE == eap_peer_lyr_cb[peer_lyr_handle]->recvd_server_result )
  {
    LOG_MSG_ERROR_0("Duplicate authentication ind received. Ignoring" );
    return;
  }
  
  /*-------------------------------------------------------------------------
    If the result indication was a failure, just pass it on to lower layers
    
    Otherwise,  all is good - indicate success to lower layers and reset Peer
  -------------------------------------------------------------------------*/

  eap_peeri_transition_state( peer_lyr_handle,
                              EAP_PEER_CLOSED_S);

  
  if( EAP_RESULT_FAILURE == 
      eap_peer_lyr_cb[peer_lyr_handle]->auth_info.result )
  {
    LOG_MSG_INFO2_0("Received auth indication after EAP Failure packet" );

    eap_eap_auth_complete_ind( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
                               EAP_RESULT_FAILURE,
                               NULL,
                               0 );
  }
  else
  {
    LOG_MSG_INFO2_0("Received auth indication after EAP SUCCESS packet" );

    eap_eap_auth_complete_ind( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
                               eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                               result,
                               eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                               pre_master_key,
                               eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                               pre_master_key_len );
  }

} /* eap_peeri_c_auth_ind()  */


/*===========================================================================

FUNCTION EAP_PEERI_C_SUCCESS

DESCRIPTION
  Event handler for a Success in Closing_S
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_c_success
(
  eap_peer_handle_type peer_lyr_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }
    
  /*-------------------------------------------------------------------------
    Check if we already received server response:
    If so, this must be a duplicate or security attack: silently discard
  -------------------------------------------------------------------------*/
  if( TRUE == eap_peer_lyr_cb[peer_lyr_handle]->recvd_server_result )
  {
    LOG_MSG_ERROR_0("Duplicate server result received. Ignoring" );
    return;
  }
  
  /*-------------------------------------------------------------------------
    If the result indication was a failure, just pass it on to lower layers
    
    Otherwise,  all is good - indicate success to lower layers and reset Peer
  -------------------------------------------------------------------------*/

  if( EAP_RESULT_FAILURE == 
      eap_peer_lyr_cb[peer_lyr_handle]->auth_info.result )
  {
    LOG_MSG_INFO2_0("Received Fail indication but EAP Success packet" );
  }


  eap_peeri_transition_state( peer_lyr_handle, 
                              EAP_PEER_CLOSED_S);

  eap_eap_auth_complete_ind( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
                             eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                             result,
                             eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                             pre_master_key,
                             eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                             pre_master_key_len );

} /* eap_peeri_c_success()  */


/*===========================================================================

FUNCTION EAP_PEERI_C_FAILURE

DESCRIPTION
  Event handler for a Failure in Closing_S
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_c_failure
(
  eap_peer_handle_type peer_lyr_handle
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  } 
  /*-------------------------------------------------------------------------
    Check if we already received server response:
    If so, this must be a duplicate or security attack: silently discard
  -------------------------------------------------------------------------*/
  if( TRUE == eap_peer_lyr_cb[peer_lyr_handle]->recvd_server_result )
  {
    LOG_MSG_ERROR_0("Duplicate server result received. Ignoring" );
    return;
  }
  
  /*-------------------------------------------------------------------------
    If the result indication was a success, we still need to give failure
    to lower layers
    
    Pass a zero key length to avoid misuse
  -------------------------------------------------------------------------*/

  if( EAP_RESULT_SUCCESS == 
      eap_peer_lyr_cb[peer_lyr_handle]->auth_info.result )
  {
    LOG_MSG_INFO2_0("Received Succ indication but EAP Failure packet" );
  }

  eap_peeri_transition_state( peer_lyr_handle, 
                              EAP_PEER_CLOSED_S);

 
  eap_eap_auth_complete_ind( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
                             EAP_RESULT_FAILURE,
                             NULL,
                             0 );

} /* eap_peeri_c_failure()  */


/*===========================================================================

FUNCTION EAP_PEERI_C_AUTH_RES_TIMEOUT

DESCRIPTION
  Event handler for an authentication result timeout in Closing_S
  
DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void eap_peeri_c_auth_res_timeout
(
  eap_peer_handle_type peer_lyr_handle
)
{
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == EAP_PEER_HANDLE_IS_VALID( peer_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid Peer layer instance" );
    return;
  }
    
  /*-------------------------------------------------------------------------
    Timeout on receiving a Success/Failure packet; 
    
    Wait no more - indicate to lower layers using indication received from
    Authentication Method
    
    Per RFC:
    Treatment of this is left to the implementation, but recommended to avoid
    lengthy timeouts
  -------------------------------------------------------------------------*/

  eap_peeri_transition_state( peer_lyr_handle, 
                              EAP_PEER_CLOSED_S);

  eap_eap_auth_complete_ind( eap_peer_lyr_cb[peer_lyr_handle]->eap_handle,
                             eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                             result,
                             eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                             pre_master_key,
                             eap_peer_lyr_cb[peer_lyr_handle]->auth_info.
                             pre_master_key_len );

} /* eap_peeri_c_auth_res_timeout()  */


/*===========================================================================
                        
                        INTERNAL HELPER FUNCTIONS
                        
===========================================================================*/

/*===========================================================================

FUNCTION EAP_PEERI_GET_METHOD_FROM_TYPE

DESCRIPTION
  Converts the passed in RFC-specified TYPE value into the method enumeration
  as per ps_eap.h
  
DEPENDENCIES
  None.

RETURN VALUE
  The EAP method value as per external API.
  
  If return value is EAP_MAX_METHOD, the type is undefined in ps_eap.h
  (unsupported)

SIDE EFFECTS
  None.

===========================================================================*/
static eap_method_enum_type eap_peeri_get_method_from_type
(
  eap_peer_type_enum_type type
)
{
  eap_method_enum_type curr_method;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Identify the current method
  -------------------------------------------------------------------------*/
  for( curr_method = EAP_MIN_EXT_METHOD; 
       curr_method < EAP_MAX_METHOD;
       curr_method++ )
  {
    if( type == eap_method_to_type_map[curr_method] )
      return curr_method;
  }

  return EAP_MAX_METHOD;
  
} /* eap_peeri_get_method_from_type() */

#endif /* FEATURE_DATA_PS_EAP */
