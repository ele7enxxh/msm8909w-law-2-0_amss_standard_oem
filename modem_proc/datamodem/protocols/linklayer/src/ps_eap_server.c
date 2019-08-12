/*===========================================================================

                          P S _ E A P _ S E R V E R. C
                                
DESCRIPTION
  This software unit holds the PS_EAP server layer of the EAP system.
  
  The functions externalized by this module are to be called ONLY by the EAP
  modules.


EXTERNALIZED FUNCTIONS


DESCRIPTION

  The EAP Server is implemented as a state machine, derived from the behavior
  described in RFC 3748. Note that the RFC does not mandate/recommend such
  a design. EAP Server is currently used in soft AP where the AT acts as the 
  EAP server for the supplicants (ex: Laptops)
  
  States:
  INIT_S (I)
  This is the initial state. At this stage, all authentication indications 
  and Success/Failure packets are discarded as spurious.
  
  IDENTITY_S (ID)
  IDENTITY request is sent & IDENTITY response is received in this state. 
  All other packets are discarded

  AUTHENTICATING_S (A)
  Right now EAP server acts as a pass through in this state, sending the 
  authentication packet between the EAP-EXT layer & the supplicant. 
  NAK/Expanded-NAK are processed by EAP server itself.
  
  CLOSED_S (C)
  This state indicates the end of the authentication exchanges, the server 
  sends out the Success/Failure packets. It also indicates the lower layer 
  (ex: LAN 1.x) about the result. EAP instance is cleared when 
  eap_delete_instance() is invoked.
  
LIMITATIONS
  
  - NAK/Expanded-NAK means failure since we don't care about the other 
    authentication types supported by the peer
  
  - Notification & re-transmissions are not implemented in the first stage
  
 

 Copyright (c)2011-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_eap_server.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/14/14   vl      Adding rex_undef_timer() instead of rex_clr_timer to 
                   deallocate the timer memory
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
10/26/12    fn     Dynamic allocation of eap_server_lyr_cb 
08/17/12    fn     Cleanup ASSERT() and dsm_pushdown 
01/06/12    scb    Fix to resolve buffer overflow issues wrt. EAP server impl
09/26/11    op     Compiler warning fix
08/26/11    op     Added changes to obtain supplementary SIM/AKA auth info 
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
07/30/10    jee    Added retransmission functionality for EAP server
05/20/10  jee/mga  Created module

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
#include "ps_crit_sect.h"
#include "ps_utils.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#ifndef MAX
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#endif

/*---------------------------------------------------------------------------
  Re-transmission config info, RFC 2988
---------------------------------------------------------------------------*/
#define EAP_SERVER_MAX_RE_TX  3        /* Maximum re-transmissions allowed */
#define EAP_SERVER_DEFAULT_RTO_VAL 3000 /* Default RTO value in msec       */
#define EAP_SERVER_MIN_RTO_VAL 1000     /* Minimum RTO value in msec       */

/*---------------------------------------------------------------------------
  Round Trip Time calculation parameters.
---------------------------------------------------------------------------*/
#define EAP_SERVER_AGAIN   8                /* Average RTT gain = 1/8      */
#define EAP_SERVER_LAGAIN  3                /* Log2(AGAIN)                 */
#define EAP_SERVER_DGAIN   4                /* Mean deviation gain = 1/4   */
#define EAP_SERVER_LDGAIN  2                /* log2(DGAIN)                 */
#define EAP_SERVER_CONST_K 4         /* constant K used in RTO calculation */
#define EAP_SERVER_CONST_G 100              /* timer granularity           */
/*---------------------------------------------------------------------------
  Enumeration of all EAP methods supported
  
  See RFC 3748
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SERVER_MIN_TYPE           =  -1,
  /*-------------------------------------------------------------------------
    Types 1-4 are REQUIRED to be supported by RFC
  -------------------------------------------------------------------------*/
  EAP_SERVER_IDENTITY_TYPE      =   1,
  EAP_SERVER_NOTIFICATION_TYPE  =   2,
  EAP_SERVER_NAK_TYPE           =   3,
  EAP_SERVER_MD5_CHALLENGE_TYPE =   4,

  EAP_SERVER_TLS_TYPE           =  13,
  EAP_SERVER_SIM_TYPE           =  18,
  EAP_SERVER_TTLS_TYPE          =  21,
  EAP_SERVER_AKA_TYPE           =  23,
  EAP_SERVER_PEAP_TYPE          =  25,
  EAP_SERVER_MSCHAP_V2_TYPE     =  26,
  EAP_SERVER_TLV_TYPE           =  33,
  EAP_SERVER_AKA_PRIME_TYPE     =  50,
  EAP_SERVER_EXT_TYPE           = 254,

  EAP_SERVER_MAX_TYPE
} eap_server_type_enum_type;


/*---------------------------------------------------------------------------
  Server state enumeration
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SERVER_MIN_S             = -1,
  
  EAP_SERVER_INIT_S            =  0, /* Server is ready to send identity req */
  EAP_SERVER_IDENTITY_S        =  1, /* Server has started identity stage  */
  EAP_SERVER_AUTHENTICATING_S  =  2, /* Server has started an auth method  */
  EAP_SERVER_CLOSED_S          =  3, /* Server is done with current session*/

  EAP_SERVER_MAX_S
} eap_server_state_enum_type;

/*---------------------------------------------------------------------------
  Enumeration of all the types/methods supported by the EAP Server
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SERVER_IDENTITY_METHOD      = 0,
  EAP_SERVER_NOTIFY_METHOD        = 1, 
  EAP_SERVER_NAK_METHOD           = 2,
  EAP_SERVER_MD5_METHOD           = 3,
  EAP_SERVER_TLS_METHOD           = 4,
  EAP_SERVER_PEAP_METHOD          = 5,
  EAP_SERVER_TTLS_METHOD          = 6,
  EAP_SERVER_TLV_METHOD           = 7,
  EAP_SERVER_MSCHAP_V2_METHOD     = 8,
  EAP_SERVER_SIM_METHOD           = 9,
  EAP_SERVER_AKA_METHOD           = 10,
  EAP_SERVER_AKA_PRIME_METHOD     = 11,
  EAP_SERVER_EXT_METHOD           = 12,
  EAP_SERVER_MAX_METHOD
} eap_server_method_enum_type;

/*---------------------------------------------------------------------------
  MAX number of times to try sending EAP ID Request if the previous 
  response fails. This value is as per Implementaion Note in RFC 3748
---------------------------------------------------------------------------*/
#define EAP_MAX_SERVER_ID_TRIES 4

/*---------------------------------------------------------------------------
  Prototype for code-specific input function
---------------------------------------------------------------------------*/
typedef void (* eap_server_code_input_fnc_type)( eap_server_handle_type,
                                               dsm_item_type **);

/*---------------------------------------------------------------------------
  Server layer header
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8 type;
} eap_server_frm_hdr_type;

/*---------------------------------------------------------------------------
  Re-transmission timeout info
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 rtt;                                          /* round-trip time */
  uint32 begin_time;          /* Initial time when the packet was sent out */
  uint32 srtt;                                 /* Smoothed round-trip time */
  uint32 rttvar;                              /* round-trip time variation */
  uint32 rto;                                   /* re-transmission timeout */
  uint8 retrans_tries;         /* No. of times the re-transmission is done */
  boolean init_flag;            /* To keep track of initial RTO calculation*/
  rex_timer_type  timer_hdl;                               /* Timer handle */
  eap_client_timer_cback_type timer_f_ptr;        /* client timer callback */
  void                       *timer_user_data; /*user data for timer cback */

} eap_server_retransmission_info;

/*---------------------------------------------------------------------------
  Server layer information block - this stores the Server state machine and
  related data structures
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                   is_assigned;      /* Instance already assigned?*/
  
  eap_handle_type           eap_handle;       /* Handle to the EAP instance*/
  
  eap_server_state_enum_type  state;          /*Current state of the server*/

  /*-------------------------------------------------------------------------
    This holds the currently processed method type; a response type may be
    determined by this value (see note below)
    
    NOTE: Notification is the ONLY exception to this as it may occur while
    another method is in progress
    For this reason, the state is also used to determine the response
  -------------------------------------------------------------------------*/
  eap_server_type_enum_type   current_method_type;

  /*-------------------------------------------------------------------------
    Current packet being processed
  -------------------------------------------------------------------------*/
  dsm_item_type            *pending_pkt;

  eap_server_type_enum_type pending_pkt_type;

  /*-------------------------------------------------------------------------
    Identity of the server - this is needed by the Identity method
  -------------------------------------------------------------------------*/
  eap_identity_type         user_id;             /* User Id                */

  boolean                   raw_mode;            /* Pass-through?          */

  /*-------------------------------------------------------------------------
    Number of times to try sending an Id req if previous req received 
    a wrong response
  -------------------------------------------------------------------------*/
  uint8                     eap_id_tries;  

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

  eap_server_retransmission_info timer_info;
} eap_server_lyr_cb_type;

/*---------------------------------------------------------------------------
  Macro to check if the Server layer handle is valid
---------------------------------------------------------------------------*/
#define EAP_SERVER_HANDLE_IS_VALID( server_lyr_handle )\
  ( (server_lyr_handle != -1) &&                       \
    (eap_server_lyr_cb[server_lyr_handle] != NULL)     \
    ? TRUE : FALSE )

/*===========================================================================

                          FORWARD DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  Functions for code-specific input handling
---------------------------------------------------------------------------*/ 
static void eap_serveri_response_input
( 
  eap_server_handle_type   server_lyr_handle,
  dsm_item_type        **sdu
); 

/*---------------------------------------------------------------------------
  Function to change the state of the Server
---------------------------------------------------------------------------*/
static void eap_serveri_transition_state
(
  eap_server_handle_type      server_lyr_handle,
  eap_server_state_enum_type  state
);

static void eap_handle_identity_response
( 
  eap_handle_type   eap_handle,
  dsm_item_type   **sdu
);

static void eap_handle_nak_response
( 
  eap_handle_type   eap_handle,
  dsm_item_type   **sdu
);

static void eap_serveri_handle_response
( 
  eap_server_handle_type   server_lyr_handle
);

/*---------------------------------------------------------------------------
  Helper functions
---------------------------------------------------------------------------*/
static eap_server_method_enum_type eap_serveri_get_type_index 
(
  eap_server_type_enum_type type
);

/*---------------------------------------------------------------------------
  Functions involved in re-transmission of packets
---------------------------------------------------------------------------*/
void eap_server_rto_cback_task
(
  void *user_data
);

void eap_server_rto_cback
(
  unsigned long user_data
);

void eap_server_calculate_rto
(
  eap_server_handle_type  server_lyr_handle
);

/*===========================================================================
                              
                              DATA DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  This forms the pool of Server layer control blocks that can be assigned 
  using eap_server_create_instance()
---------------------------------------------------------------------------*/
static eap_server_lyr_cb_type* eap_server_lyr_cb[EAP_MAX_INSTANCES];

/*---------------------------------------------------------------------------
  Table of code-specific input functions
  
  NOTE: Code values are taken from RFC, hence the non-usage of 0th element
---------------------------------------------------------------------------*/
static const eap_server_code_input_fnc_type 
eap_server_code_input_fnc[EAP_MAX_CODE] = 
{
  /* 0th element is EAP_MIN_CODE  */
  NULL,
  /* Requests should never reach the Server */  
  NULL,
  eap_serveri_response_input,
  /* Success/failure should never reach the Server */  
  NULL,
  NULL
};

/*---------------------------------------------------------------------------
  The table of method input function pointers
  
  This holds input functions for both internal and external methods
---------------------------------------------------------------------------*/
static eap_method_input_cback_type eap_server_method_input_cback[
                                                          EAP_MAX_METHOD] = 
{
  eap_handle_identity_response,
  NULL, /* Notification */
  eap_handle_nak_response,
  NULL,
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
static eap_method_close_cback_type eap_server_method_close_cback[
                                                          EAP_MAX_METHOD] = 
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
  For now, it only has the result pointer for EAP extended method.
  In server role this result callback is overloaded to pass the NAK
  indication to extended method. After calling this callback, EAP will 
  wait for the extended method to call auth_complete_ind before sending 
  out the SUCCESS/FAILURE packet.
---------------------------------------------------------------------------*/
static eap_method_result_cback_type eap_server_method_result_cback[
                                                          EAP_MAX_METHOD] = 
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
  
  This holds supp result functions for both internal and external methods
---------------------------------------------------------------------------*/
static eap_method_supp_result_cback_type
                       eap_server_method_supp_result_cback[EAP_MAX_METHOD] = 
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
  NULL,  /* EAP_AKA_METHOD */
  NULL,  /* EAP_AKA_PRIME_METHOD */
  NULL   /* EAP_EXT_METHOD */
};

/*---------------------------------------------------------------------------
  Mapping between the method types enumeration and types as per RFC
  
  NOTE: This has to be synced between eap_server_type_enum_type and
  eap_method_enum_type
---------------------------------------------------------------------------*/
static const eap_server_type_enum_type eap_method_to_type_map[
                                                          EAP_MAX_METHOD] = 
{
  EAP_SERVER_IDENTITY_TYPE,
  EAP_SERVER_NOTIFICATION_TYPE,
  EAP_SERVER_NAK_TYPE,
  EAP_SERVER_MD5_CHALLENGE_TYPE,
  EAP_SERVER_TLS_TYPE,
  EAP_SERVER_PEAP_TYPE,
  EAP_SERVER_TTLS_TYPE,
  EAP_SERVER_TLV_TYPE,
  EAP_SERVER_MSCHAP_V2_TYPE,
  EAP_SERVER_SIM_TYPE,
  EAP_SERVER_AKA_TYPE,
  EAP_SERVER_AKA_PRIME_TYPE,
  EAP_SERVER_EXT_TYPE
};

/*---------------------------------------------------------------------------
  Note: Event handler function table - this table holds the event handler 
  function for each state in Server, This is not required in server right now
---------------------------------------------------------------------------*/

/*===========================================================================

                          EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION EAP_SERVER_INIT

DESCRIPTION
  Initializes the Server layer control blocks.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Initialize the Server layer control blocks.
===========================================================================*/
void eap_server_init
(
  void
)
{
  int server_lyr_cb_cnt;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  LOG_MSG_INFO2_0("eap_server_init() called ");

  /*-------------------------------------------------------------------------
    Initialize the Server layer
  -------------------------------------------------------------------------*/
  for (server_lyr_cb_cnt=0; server_lyr_cb_cnt<EAP_MAX_INSTANCES; 
       server_lyr_cb_cnt++)
  {
    eap_server_lyr_cb[server_lyr_cb_cnt] = NULL;
  }
  
} /* eap_server_init()  */

/*===========================================================================
FUNCTION EAP_SERVER_CREATE_INSTANCE

DESCRIPTION
  Initializes Server layer instance and returns handle to this instance.

DEPENDENCIES
  Peer must have been initialized

RETURN VALUE
  Handle to Peer instance.

SIDE EFFECTS
  None.
===========================================================================*/
eap_server_handle_type eap_server_create_instance
(  
  eap_server_info_type *server_lyr_info
)
{
  eap_server_handle_type server_lyr_handle;

  int server_lyr_cb_cnt;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Initialize handle to invalid value
  -------------------------------------------------------------------------*/
  server_lyr_handle = -1;

  /*-------------------------------------------------------------------------
    Find a free Server layer control block and reserve it
  -------------------------------------------------------------------------*/
  for (server_lyr_cb_cnt=0; server_lyr_cb_cnt<EAP_MAX_INSTANCES; 
       server_lyr_cb_cnt++)
  {
    if (NULL == eap_server_lyr_cb[server_lyr_cb_cnt])
    {
      server_lyr_handle = server_lyr_cb_cnt;
      break;
    }
  }

  if (EAP_MAX_INSTANCES == server_lyr_cb_cnt)
  {
    LOG_MSG_ERROR_0("Could not find free server layer control block");
    return -1;
  }

  if( -1 == server_lyr_handle )
  {
    /*-----------------------------------------------------------------------
      Could not find a free instance
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("Could not assign Server layer instance" );
    return -1;
  }

  /*-----------------------------------------------------------------------
    Dynamic allocation of control block
  -----------------------------------------------------------------------*/
  eap_server_lyr_cb[server_lyr_handle] = ( eap_server_lyr_cb_type*)
       ps_system_heap_mem_alloc(sizeof(eap_server_lyr_cb_type));

  if( eap_server_lyr_cb[server_lyr_handle] == NULL)
  {
    LOG_MSG_ERROR_0("EAP server dynamic ctx allocation failed");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Initialize the Server layer instance
  -------------------------------------------------------------------------*/  
  memset(eap_server_lyr_cb[server_lyr_handle], 0, 
         sizeof(eap_server_lyr_cb_type));

  eap_server_lyr_cb[server_lyr_handle]->eap_handle = server_lyr_info->eap_handle;

  eap_server_lyr_cb[server_lyr_handle]->current_method_type 
                                                       = EAP_SERVER_MIN_TYPE;

  eap_server_lyr_cb[server_lyr_handle]->state = EAP_SERVER_INIT_S;

  eap_server_lyr_cb[server_lyr_handle]->user_id = server_lyr_info->eap_identity;

  eap_server_lyr_cb[server_lyr_handle]->is_assigned = TRUE;

  eap_server_lyr_cb[server_lyr_cb_cnt]->eap_id_tries = 0;

  eap_server_lyr_cb[server_lyr_handle]->pending_pkt = NULL;

  eap_server_lyr_cb[server_lyr_cb_cnt]->pending_pkt_type = EAP_SERVER_MIN_TYPE;

  eap_server_lyr_cb[server_lyr_handle]->raw_mode = server_lyr_info->raw_mode;

  /*-------------------------------------------------------------------------
    In raw mode, just set the method to the first authentication protocol
    requested
  -------------------------------------------------------------------------*/
  if (TRUE == eap_server_lyr_cb[server_lyr_handle]->raw_mode)
  {
    if (EAP_SERVER_MIN_TYPE 
                != eap_server_lyr_cb[server_lyr_handle]->current_method_type)
    {
      LOG_MSG_ERROR_0("Invalid current method type in raw mode" );
      PS_SYSTEM_HEAP_MEM_FREE( eap_server_lyr_cb[server_lyr_handle] );
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Re-transmission timer config
  -------------------------------------------------------------------------*/
  if (NULL == server_lyr_info->timer_f_ptr)
  {
    LOG_MSG_ERROR_0("Client timer callback function is NULL" );
    PS_SYSTEM_HEAP_MEM_FREE( eap_server_lyr_cb[server_lyr_handle] );
    return -1;
  }

  eap_server_lyr_cb[server_lyr_handle]->timer_info.timer_f_ptr
                                       = server_lyr_info->timer_f_ptr;
  eap_server_lyr_cb[server_lyr_handle]->timer_info.timer_user_data
                                       = server_lyr_info->timer_user_data;

  rex_def_timer_ex(
               &(eap_server_lyr_cb[server_lyr_handle]->timer_info.timer_hdl),
               eap_server_rto_cback, 
               (unsigned long) (eap_server_lyr_cb[server_lyr_handle]));

  eap_server_lyr_cb[server_lyr_handle]->timer_info.init_flag = FALSE;
  eap_server_lyr_cb[server_lyr_handle]->timer_info.rto 
                                              = EAP_SERVER_DEFAULT_RTO_VAL;

  /*-------------------------------------------------------------------------
    Return a handle if all required steps were successful
  -------------------------------------------------------------------------*/
  return server_lyr_handle;

} /* eap_server_create_instance() */

/*===========================================================================
FUNCTION EAP_SERVER_DELETE_INSTANCE

DESCRIPTION
   Shuts down the Server layer instance.

DEPENDENCIES
  EAP Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_delete_instance
(  
  eap_server_handle_type server_lyr_handle
)
{
  eap_server_method_enum_type curr_method;
  eap_server_type_enum_type   curr_type;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  LOG_MSG_INFO2_1("eap_server_delete_instance() called , server_lyr_handle : %d ",
                  server_lyr_handle);

  /*-------------------------------------------------------------------------
    Free the Server instance
  -------------------------------------------------------------------------*/
  if (TRUE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    if (NULL != eap_server_lyr_cb[server_lyr_handle]->pending_pkt)
    {
      dsm_free_packet(&eap_server_lyr_cb[server_lyr_handle]->pending_pkt);
      eap_server_lyr_cb[server_lyr_handle]->pending_pkt = NULL;
    }

    /*-----------------------------------------------------------------------
      For ease of understanding code: convert the current type into method
      
      (Type is Per RFC; method is local enumeration)
    -----------------------------------------------------------------------*/
    curr_type = eap_server_lyr_cb[server_lyr_handle]->current_method_type;

    curr_method = eap_serveri_get_type_index(curr_type);

    /*-----------------------------------------------------------------------
      Identify the current method and invoke its close callback
    -----------------------------------------------------------------------*/
    if ((EAP_SERVER_MAX_METHOD != curr_method) &&
          (NULL != eap_server_method_close_cback[curr_method]))
    {
        eap_server_method_close_cback[curr_method]
          (eap_server_lyr_cb[server_lyr_handle]->eap_handle);
    }

    /*-----------------------------------------------------------------------
      Clears the re-transmission timer if it is running and 
      deallocate the memory allocated for this timer
    -----------------------------------------------------------------------*/
    rex_undef_timer(
                &eap_server_lyr_cb[server_lyr_handle]->timer_info.timer_hdl);

    PS_SYSTEM_HEAP_MEM_FREE(eap_server_lyr_cb[server_lyr_handle]);
  }
  else
  {
    LOG_MSG_INFO1_1("EAP server delete instance - invalid handle: %d",
                    server_lyr_handle  );
  }
} /* eap_server_delete_instance() */

/*===========================================================================
FUNCTION EAP_SERVER_RESULT

DESCRIPTION
  To send EAP Sucess/Failure packet to the peer

DEPENDENCIES
  None.

PARAMETERS
  eap_handle - EAP handle

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void eap_server_result
(
  eap_handle_type eap_handle
)
{
  dsm_item_type      *tx_pkt = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  /*-------------------------------------------------------------------------
    Create a new pacekt & pass it to lower layers
  -------------------------------------------------------------------------*/
  tx_pkt = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  if (NULL == tx_pkt)
  {
    LOG_MSG_INFO1_0("Could not assign a free DSM item" );
    return;
  }

  eap_xmit_pkt(eap_handle, &tx_pkt);
} /* eap_server_result() */
  
/*===========================================================================
FUNCTION EAP_SERVER_IDENTITY_REQ

DESCRIPTION
  Function to send Identity request packet to the peer

DEPENDENCIES
  None.

PARAMETERS
  eap_handle - EAP handle

RETURN VALUE
  0 - Success
 -1 - Failure

SIDE EFFECTS
  None
===========================================================================*/
int32 eap_server_identity_req
(
  eap_handle_type eap_handle
)
{
  dsm_item_type              *tx_pkt = NULL;
  eap_server_handle_type      server_lyr_handle;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
    Create a new pacekt & pass it to lower layers
  -----------------------------------------------------------------------*/
  tx_pkt = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  if (NULL == tx_pkt)
  {
    LOG_MSG_ERROR_0("Could not assign a free DSM item" );
    return -1;
  }

  server_lyr_handle = eap_get_server_lyr_handle(eap_handle);
  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_ERROR_0("Invalid EAP Server layer handle");
    return -1;
  }

  eap_serveri_transition_state(server_lyr_handle, EAP_SERVER_IDENTITY_S);

  eap_xmit_pkt(eap_handle, &tx_pkt);

  eap_server_lyr_cb[server_lyr_handle]->eap_id_tries++;

  return 0;
} /* eap_server_identity_req() */

/*===========================================================================
FUNCTION EAP_SERVER_XMIT_PKT

DESCRIPTION
  Tx function for the Server layer. Adds server header and send the packet to 
  the EAP layer.

DEPENDENCIES
  Server instance must be created

PARAMETERS
  server_lyr_handle - Server layer handle
  sdu               - Tx Packet

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void eap_server_xmit_pkt
(
  eap_server_handle_type   server_lyr_handle,
  dsm_item_type          **sdu
)
{
  eap_server_frm_hdr_type  server_frm_hdr;
  uint16                   write_bytes;
  eap_code_enum_type       code;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_ERROR_0("Invalid Server layer instance");
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Check if the packet size greater than the MTU
  -------------------------------------------------------------------------*/
  if (EAP_DEFAULT_MTU_SIZE < dsm_length_packet(*sdu))
  {
    LOG_MSG_INFO1_0("EAP method using MTU higher than permissible" );
  }

  /*-------------------------------------------------------------------------
    Determine what to do with the packet
  -------------------------------------------------------------------------*/
  switch(eap_server_lyr_cb[server_lyr_handle]->state)
  {
    case EAP_SERVER_IDENTITY_S:
      server_frm_hdr.type = (uint8)EAP_SERVER_IDENTITY_TYPE;
      code = EAP_REQUEST_CODE;
      eap_server_lyr_cb[server_lyr_handle]->current_method_type 
                                                  = EAP_SERVER_IDENTITY_TYPE;
      break;  
    
    case EAP_SERVER_AUTHENTICATING_S:
      /*---------------------------------------------------------------------
        Right now the authentication happens in expanded type with EAP acting
        as a pass through 
      ---------------------------------------------------------------------*/
      server_frm_hdr.type = (uint8)EAP_SERVER_EXT_TYPE;
      code = EAP_REQUEST_CODE;
      eap_server_lyr_cb[server_lyr_handle]->current_method_type 
                                                       = EAP_SERVER_EXT_TYPE;
      break;
  
    case EAP_SERVER_CLOSED_S:
      /*---------------------------------------------------------------------
        Needs to send out success/failure packet based on result
        provided by EAP-EXT.
      ---------------------------------------------------------------------*/
      if (EAP_RESULT_SUCCESS == eap_server_lyr_cb[
                                        server_lyr_handle]->auth_info.result)
      {
        code = EAP_SUCCESS_CODE;
      }
      else
      {
        code = EAP_FAILURE_CODE;
      }
      break;

    case EAP_SERVER_INIT_S:
      LOG_MSG_INFO1_0("Packet can't be sent in INIT state" );
      dsm_free_packet(sdu);
      return;

    default:
      /*---------------------------------------------------------------------
        SANITY check
      ---------------------------------------------------------------------*/
      LOG_MSG_ERROR_1("Unknown EAP Server state %d",
                      eap_server_lyr_cb[server_lyr_handle]->state);
      dsm_free_packet(sdu);
      return;
  }
  
  /*-------------------------------------------------------------------------
    Send REQUEST down to EAP layer. EAP Success/Failure packet won't have
    the type field & they are not re-transmitted
  -------------------------------------------------------------------------*/
  if (EAP_SERVER_CLOSED_S != eap_server_lyr_cb[server_lyr_handle]->state)
  {
    write_bytes = dsm_pushdown_packed(sdu,
                                      (void *)&server_frm_hdr,
                                      sizeof(eap_server_frm_hdr_type),
                                      DSM_DS_SMALL_ITEM_POOL );
  
    if( sizeof(eap_server_frm_hdr_type) != write_bytes )
    {
      LOG_MSG_ERROR_0("Failed to pushdown Server header");
      dsm_free_packet( sdu );
      return;
    }

    /*-----------------------------------------------------------------------
      Note the time when the packet is sent out, this will be used for RTT 
      calculation, reset the re-transmission retries to 0 & start the timer
    -----------------------------------------------------------------------*/
    eap_server_lyr_cb[server_lyr_handle]->timer_info.begin_time = msclock();
    eap_server_lyr_cb[server_lyr_handle]->timer_info.retrans_tries = 0;

    (void)rex_set_timer(
                &(eap_server_lyr_cb[server_lyr_handle]->timer_info.timer_hdl),
                  eap_server_lyr_cb[server_lyr_handle]->timer_info.rto);
  }

  eap_eap_xmit_pkt(eap_server_lyr_cb[server_lyr_handle]->eap_handle,
                   code,
                   sdu);

} /* eap_server_xmit_pkt() */

/*===========================================================================
FUNCTION EAP_SERVER_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify Server of
  the result of the authentication process.
  The Server layer sends out Success/Failure packet based on the result & 
  also notifies the lower layer (eg: LAN 1.x)

DEPENDENCIES
  Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_auth_complete_ind 
(
  eap_server_handle_type   server_lyr_handle,
  eap_result_enum_type     result,
  uint8                   *pre_master_key,
  uint16                   pre_master_key_len
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  /*-------------------------------------------------------------------------
    Check for server layer handle validity
  -------------------------------------------------------------------------*/
  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_INFO1_0("Invalid Server layer instance");
    return;
  }

  /*-------------------------------------------------------------------------
    Store the authentication information in control block
  -------------------------------------------------------------------------*/
  eap_server_lyr_cb[server_lyr_handle]->auth_info.result = result;

  if (EAP_MAX_PRE_MASTER_KEY_SIZE >= pre_master_key_len)
  {
    eap_server_lyr_cb[server_lyr_handle]->auth_info.pre_master_key_len = 
                                                          pre_master_key_len;
  }

  else
  {
    LOG_MSG_ERROR_0("EAP Server received larger pre-master key than expected" );
    eap_server_lyr_cb[server_lyr_handle]->auth_info.pre_master_key_len =
      EAP_MAX_PRE_MASTER_KEY_SIZE;
  }

  if (NULL != pre_master_key)
  {
    memscpy(eap_server_lyr_cb[server_lyr_handle]->auth_info.pre_master_key,
           EAP_MAX_PRE_MASTER_KEY_SIZE,
           pre_master_key,
           eap_server_lyr_cb[server_lyr_handle]->auth_info.pre_master_key_len );
  }

  eap_serveri_transition_state(server_lyr_handle, EAP_SERVER_CLOSED_S);

  /*------------------------------------------------------------------------
    Send EAP Success/Failure packet based on result provided
  -------------------------------------------------------------------------*/
  eap_server_result (eap_server_lyr_cb[server_lyr_handle]->eap_handle);
 
  /*-----------------------------------------------------------------------
     No need to do any further processing in EAP
  -----------------------------------------------------------------------*/
  eap_eap_auth_complete_ind( eap_server_lyr_cb[server_lyr_handle]->eap_handle,
                             eap_server_lyr_cb[server_lyr_handle]->auth_info.
                             result,
                             eap_server_lyr_cb[server_lyr_handle]->auth_info.
                             pre_master_key,
                             eap_server_lyr_cb[server_lyr_handle]->auth_info.
                             pre_master_key_len );

  return;

} /* eap_server_auth_complete_ind() */

/*===========================================================================
FUNCTION EAP_SERVER_SUPP_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify Server of
  the supplementary info of the authentication process.

DEPENDENCIES
  Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_supp_auth_complete_ind 
(
  eap_server_handle_type           server_lyr_handle,
  eap_result_enum_type             result,
  eap_sim_aka_supp_auth_mask_type  supp_auth_info_mask,
  uint16                           supp_auth_info_size,
  void                            *supp_auth_info_ptr
)
{

  /*-------------------------------------------------------------------------
    Check for server layer handle validity
  -------------------------------------------------------------------------*/
  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_ERROR_0("Invalid Server layer instance");
    return;
  }
 
  /*-----------------------------------------------------------------------
     No need to do any further processing in EAP
  -----------------------------------------------------------------------*/
  eap_eap_supp_auth_complete_ind(
                             eap_server_lyr_cb[server_lyr_handle]->eap_handle,
                             result,
                             supp_auth_info_mask,
                             supp_auth_info_size,
                             supp_auth_info_ptr );
  return;

} /* eap_server_supp_auth_complete_ind() */

/*===========================================================================
FUNCTION EAP_SERVERI_TRANSITION_STATE

DESCRIPTION
  Function to change the state of the Server

DEPENDENCIES
  Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void eap_serveri_transition_state
(
  eap_server_handle_type      server_lyr_handle,
  eap_server_state_enum_type  state
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  
  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_ERROR_0("Invalid Server layer instance" );
    return;
  }

  /*-------------------------------------------------------------------------
    Check validity of state
  -------------------------------------------------------------------------*/
  if ((EAP_SERVER_MIN_S > state) || (EAP_SERVER_MAX_S < state))
  {
    LOG_MSG_ERROR_0("Invalid state change requested" );
    return;
  }

  LOG_MSG_INFO2_2("Changing Server state from %d --> %d",
                  eap_server_lyr_cb[server_lyr_handle]->state, state);

  eap_server_lyr_cb[server_lyr_handle]->state = state;

} /* eap_serveri_transition_state() */

/*===========================================================================
FUNCTION EAP_SERVERI_GET_METHOD_FROM_TYPE

DESCRIPTION
  Converts the passed in RFC-specified TYPE value into the method enumeration
  as per ps_eap_server.h
  
DEPENDENCIES
  None.

RETURN VALUE
  EAP method value      - Success
  EAP_SERVER_MAX_METHOD - Failure (unsupported)

SIDE EFFECTS
  None.
===========================================================================*/
static eap_server_method_enum_type eap_serveri_get_type_index 
(
  eap_server_type_enum_type type
)
{
  eap_server_method_enum_type curr_method;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Identify the current method
  -------------------------------------------------------------------------*/
  for (curr_method = EAP_SERVER_IDENTITY_METHOD; 
       curr_method < EAP_SERVER_MAX_METHOD;
       curr_method++)
  {
    if ( type == eap_method_to_type_map[curr_method] )
    {
      return curr_method;
    }
  }

  return EAP_SERVER_MAX_METHOD;
  
} /* eap_serveri_get_type_index() */

/*===========================================================================
FUNCTION EAP_SERVER_METHOD_INPUT_CBACK_REG

DESCRIPTION
  Registers the method-specific input function with Authenticator
  
  NOTE: This function should only be called if the method specifies that
  it supports the Authenticator role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  input function for a given method.
  

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_method_input_cback_reg
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

  if ((EAP_MIN_EXT_METHOD > method) || (EAP_MAX_METHOD <= method))
  {
    LOG_MSG_ERROR_0("Invalid method specified for registering EAP cback" );
    return;
  }

  eap_server_method_input_cback[method] = cback;

} /* eap_server_method_input_cback_reg()  */

/*===========================================================================
FUNCTION EAP_SERVER_METHOD_CLOSE_CBACK_REG

DESCRIPTION
  Registers the method-specific close function with Server
  
  NOTE: This function should only be called if the method specifies that
  it supports the Server role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  close function for a given method.
  

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_method_close_cback_reg
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

  if ((EAP_MIN_EXT_METHOD > method) || (EAP_MAX_METHOD <= method))
  {
    LOG_MSG_ERROR_0("Invalid method specified for registering EAP cback" );
    return;
  }

  eap_server_method_close_cback[method] = cback;

} /* eap_server_method_close_cback_reg()  */

/*===========================================================================
FUNCTION EAP_SERVER_METHOD_RESULT_CBACK_REG

DESCRIPTION
  Registers the method-specific result function with Server
  NOTE: This function should only be called if the method specifies that
  it supports the Server role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  result function for a given method.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_method_result_cback_reg
(
  eap_method_result_cback_type  cback,
  eap_method_enum_type          method
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( cback == NULL )
  {
    LOG_MSG_ERROR_0("Unregister EAP cback" );
    return;
  }

  if ((EAP_MIN_EXT_METHOD > method) || (EAP_MAX_METHOD <= method))
  {
    LOG_MSG_ERROR_0("Invalid method specified for registering EAP cback");
    return;
  }

  eap_server_method_result_cback[method] = cback;

} /* eap_server_method_result_cback_reg()  */

/*===========================================================================
FUNCTION EAP_SERVER_METHOD_SUPP_RESULT_CBACK_REG

DESCRIPTION
  Registers the method-specific supp result function with Server

  NOTE: This function should only be called if the method specifies that
  it supports the Server role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  result function for a given method.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_method_supp_result_cback_reg
(
  eap_method_supp_result_cback_type  cback,
  eap_method_enum_type               method
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL == cback )
  {
    LOG_MSG_ERROR_0("Invalid callback");
    return;
  }

  if ((EAP_MIN_EXT_METHOD > method) || (EAP_MAX_METHOD <= method))
  {
    LOG_MSG_ERROR_0("Invalid method specified for registering EAP cback");
    return;
  }

  eap_server_method_supp_result_cback[method] = cback;

} /* eap_server_method_supp_result_cback_reg()  */

/*===========================================================================
FUNCTION EAP_SERVER_INPUT

DESCRIPTION
  Rx function for EAP Server layer. Parse the Server layer header and call the
  method's input function if necessary.

DEPENDENCIES
  EAP Server instance must have been created

RETURN VALUE
  None

SIDE EFFECTS
  Calls EAP method's RX function.
===========================================================================*/
void eap_server_input
( 
  eap_handle_type      eap_handle,
  eap_code_enum_type   code,
  dsm_item_type      **sdu
)
{
  eap_server_handle_type      server_lyr_handle;
  eap_server_method_enum_type curr_method;
  eap_server_type_enum_type   curr_type;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the Server layer handle
  -------------------------------------------------------------------------*/
  server_lyr_handle = eap_get_server_lyr_handle(eap_handle);

  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_ERROR_0("Invalid Server layer instance" );
    dsm_free_packet(sdu);
    return;
  }

  if (EAP_MAX_CODE <= code)
  {
    LOG_MSG_ERROR_1("Invalid code %d passed to eap_server_input", code );
    dsm_free_packet(sdu);
    return;
  } 

  /*-------------------------------------------------------------------------
    RTT = packet received time - time when packet is sent out 
    Don't use the values if the packet is re-transmitted (Karn's algo)
    Clear the RTO timer
  -------------------------------------------------------------------------*/
  if (!eap_server_lyr_cb[server_lyr_handle]->timer_info.retrans_tries)
  {
    eap_server_lyr_cb[server_lyr_handle]->timer_info.rtt 
      = msclock() - eap_server_lyr_cb[server_lyr_handle]->timer_info.begin_time;
  
    eap_server_calculate_rto(server_lyr_handle);
  }
  
  (void)rex_clr_timer(
                &eap_server_lyr_cb[server_lyr_handle]->timer_info.timer_hdl);

  /*-------------------------------------------------------------------------
    In raw mode, just the packet up the stack to the registered method
  -------------------------------------------------------------------------*/
  if (TRUE == eap_server_lyr_cb[server_lyr_handle]->raw_mode)
  {
    LOG_MSG_INFO2_0("eap_server_input(): Handling packet in raw mode");

    /*-----------------------------------------------------------------------
      For ease of understanding code: convert the current type into method
      
      (Type is Per RFC; method is local enumeration)
    -----------------------------------------------------------------------*/
    curr_type = eap_server_lyr_cb[server_lyr_handle]->current_method_type;

    curr_method = eap_serveri_get_type_index(curr_type);

    /*-----------------------------------------------------------------------
      Identify the current method and pass the packet up
    -----------------------------------------------------------------------*/
    if ((EAP_SERVER_MAX_METHOD != curr_method) &&
        (NULL != eap_server_method_input_cback[curr_method]))
    {
      eap_server_method_input_cback[curr_method](
                      eap_server_lyr_cb[server_lyr_handle]->eap_handle, sdu);
      return;
    }

    /*-----------------------------------------------------------------------
      If we reach this point, EAP has been configured in pass through mode
      with a method that has not registered a callback!!
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("No callback avaliable for pass through method");
    dsm_free_packet(sdu);    
    return;

  }

  /*-------------------------------------------------------------------------
    Pass the packet on to the code-specific input function
  -------------------------------------------------------------------------*/
  if (NULL != eap_server_code_input_fnc[code])
  {
    eap_server_code_input_fnc[code]( server_lyr_handle,
                                     sdu );
  }
  else
  {
    LOG_MSG_ERROR_1("Server unable to process EAP pkt code: %d. Received in "
                    "error", code);
    dsm_free_packet(sdu);
    return;
  }

} /* eap_server_input() */

/*===========================================================================
FUNCTION EAP_SERVERI_RESPONSE_INPUT

DESCRIPTION
  Function to receive the response packet

DEPENDENCIES
  Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void eap_serveri_response_input
( 
  eap_server_handle_type   server_lyr_handle,
  dsm_item_type          **sdu
)
{
  eap_server_frm_hdr_type  server_frm_hdr;
  uint16                   bytes_read;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  
  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_ERROR_0("Invalid Server layer instance" );
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Retrieve the header from the DSM item
  -------------------------------------------------------------------------*/
  bytes_read = dsm_pullup(sdu,
                          (void *)&server_frm_hdr,
                          sizeof(eap_server_frm_hdr_type));

  if (sizeof(eap_server_frm_hdr_type) > bytes_read)
  {
    LOG_MSG_ERROR_0("Malformed packet received, freeing" );
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Store the response packet and type for later use.    
  -------------------------------------------------------------------------*/
  if (eap_server_lyr_cb[server_lyr_handle]->pending_pkt)
  {
    LOG_MSG_INFO2_1("eap_serveri_response_input(): Freeing cached pending pkt "
                    "for %d instance", server_lyr_handle);
    dsm_free_packet(&eap_server_lyr_cb[server_lyr_handle]->pending_pkt);
  }

  eap_server_lyr_cb[server_lyr_handle]->pending_pkt = *sdu;
  *sdu = NULL;

  eap_server_lyr_cb[server_lyr_handle]->pending_pkt_type =
    (eap_server_type_enum_type) server_frm_hdr.type;

  /*-------------------------------------------------------------------------
    Determine the current state. For each state check if this current packet
    can be received in that state. If yes, then give the corresponding packet to
    that packet handler fucntion
    Determine the current type. Check if the current type can be received in
    the current state. If yes, then give the corresponding packet to
    that packet handler fucntion. If for a given state that packet type is
    not supported then ignore the packet.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_1("eap_serveri_request_input(): Packet type : %d", server_frm_hdr.type);
  switch (server_frm_hdr.type)
  {
    case EAP_SERVER_IDENTITY_TYPE:
      if (EAP_SERVER_IDENTITY_S == eap_server_lyr_cb[server_lyr_handle]->state)
      {
        eap_serveri_handle_response(server_lyr_handle);
      }

      else
      {
        LOG_MSG_ERROR_1("IDENTITY type pkt rcvd in unsupported state:%d",
                        eap_server_lyr_cb[server_lyr_handle]->state);
        dsm_free_packet(&(eap_server_lyr_cb[server_lyr_handle]->pending_pkt));
      }
      break;
  
    case EAP_SERVER_EXT_TYPE:
      if (EAP_SERVER_AUTHENTICATING_S 
                              == eap_server_lyr_cb[server_lyr_handle]->state)
      {
        eap_serveri_handle_response(server_lyr_handle);
      }

      else
      {
        LOG_MSG_ERROR_1("EXT pkt rcvd in unsupported state:%d",
                        eap_server_lyr_cb[server_lyr_handle]->state);
        dsm_free_packet(&(eap_server_lyr_cb[server_lyr_handle]->pending_pkt));
      }      
      break;
    
    case EAP_SERVER_NAK_TYPE:

      if (EAP_SERVER_AUTHENTICATING_S 
                              == eap_server_lyr_cb[server_lyr_handle]->state)
      {
        eap_serveri_handle_response(server_lyr_handle);
      }

      else
      {
        LOG_MSG_ERROR_1("NAK pkt rcvd in unsupported state:%d",
                        eap_server_lyr_cb[server_lyr_handle]->state);
        dsm_free_packet(&(eap_server_lyr_cb[server_lyr_handle]->pending_pkt));
      }      
      break;
          
    case EAP_SERVER_NOTIFICATION_TYPE:
      LOG_MSG_ERROR_0("eap_serveri_request_input: "
                      "NOTIFICATION support not present");
      dsm_free_packet(&(eap_server_lyr_cb[server_lyr_handle]->pending_pkt));
      break;    

    /*-----------------------------------------------------------------------
      Unsupported EAP types:
    -----------------------------------------------------------------------*/
    case EAP_SERVER_TLS_TYPE:
    case EAP_SERVER_SIM_TYPE:
    case EAP_SERVER_AKA_TYPE:
    case EAP_SERVER_AKA_PRIME_TYPE:
    case EAP_SERVER_MD5_CHALLENGE_TYPE:
    case EAP_SERVER_TTLS_TYPE:
    case EAP_SERVER_PEAP_TYPE:
    case EAP_SERVER_MSCHAP_V2_TYPE:
    case EAP_SERVER_TLV_TYPE:
    default:
      LOG_MSG_ERROR_1("eap_serveri_response_input(): unsupported %d type rcvd",
                      server_frm_hdr.type );
      dsm_free_packet(&(eap_server_lyr_cb[server_lyr_handle]->pending_pkt));
      break;
  
  } /* end switch */
  return;

} /*   eap_serveri_response_input()  */

/*===========================================================================
FUNCTION EAP_HANDLE_IDENTITY_RESPONSE

DESCRIPTION
  Function to handle the identity response.

DEPENDENCIES
  Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void eap_handle_identity_response
( 
  eap_handle_type   eap_handle,
  dsm_item_type   **sdu
)
{
  eap_identity_type       eap_identity_recvd;
  uint16                  bytes_read = 0;
  uint32                  packet_len = 0;
  eap_server_handle_type  server_lyr_handle;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 
  if ((sdu == NULL) || (*sdu == NULL))
  {
    LOG_MSG_ERROR_0("sdu parameter is NULL" );
    return;
  }

  server_lyr_handle = eap_get_server_lyr_handle (eap_handle);
  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_ERROR_0("Invalid Server layer instance" );
    dsm_free_packet(sdu);
    return;
  }

  memset((void*)(&eap_identity_recvd), 0, sizeof(eap_identity_type));

  packet_len = dsm_length_packet(*sdu);

  if( packet_len == 0 || packet_len > EAP_IDENTITY_MAX_CHAR )
  {
    LOG_MSG_ERROR_1("Invalid identity length %d, freeing", packet_len );
    dsm_free_packet(sdu);
    return;
  }
  else
  {
    eap_identity_recvd.len = (uint16)packet_len;
    /*-------------------------------------------------------------------------
      Fetch the id from the dsm item to eap_identity_recvd
    -------------------------------------------------------------------------*/
    bytes_read = dsm_pullup( (void*)(sdu),
                             (void *)(eap_identity_recvd.name),
                             eap_identity_recvd.len );

    if ( eap_identity_recvd.len != bytes_read )
    {
      LOG_MSG_ERROR_0("Malformed packet received, freeing" );
      dsm_free_packet(sdu);
      return;
    }

    /*-------------------------------------------------------------------------
      Compare the ID value to authneticate the peer. If the identity does not 
      match then it is retried for 3 times before failure is indicated
    -------------------------------------------------------------------------*/
    if ((eap_server_lyr_cb[server_lyr_handle]->user_id.len == 
          eap_identity_recvd.len) &&
        (0 == memcmp((void*)(eap_server_lyr_cb[server_lyr_handle]->user_id.name),
                     (void*)(eap_identity_recvd.name),
                             eap_identity_recvd.len)) 
       ) 
    {
      LOG_MSG_INFO1_0("EAP Identity Success ");
      eap_serveri_transition_state(server_lyr_handle, 
                                   EAP_SERVER_AUTHENTICATING_S);
      eap_client_start_ind (eap_handle);      
      return;
    }/* end if response did match*/
  }

  /*-------------------------------------------------------------------------
    If the Identity is of length 0 or the identity did not match.
    Retry sending new Identity request (3 retries as per RFC3748) or indicate
    EAP server of failure
  -------------------------------------------------------------------------*/
  LOG_MSG_ERROR_0("EAP Identity Failure");
  
  if (eap_server_lyr_cb[server_lyr_handle]->eap_id_tries 
                                                  < EAP_MAX_SERVER_ID_TRIES)
  {
    LOG_MSG_INFO1_1("Invalid ID, retrying %d",
                    eap_server_lyr_cb[server_lyr_handle]->eap_id_tries );
    dsm_free_packet(sdu);
    (void)eap_server_identity_req(eap_handle);
    return;
  }
  
  eap_auth_complete_ind(eap_handle,
                        EAP_RESULT_FAILURE,
                        NULL,
                        0);

}/*eap_handle_identity_response*/

/*===========================================================================
FUNCTION EAP_HANDLE_NAK_RESPONSE

DESCRIPTION
  Function to handle the NAK response.

DEPENDENCIES
  Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void eap_handle_nak_response
( 
  eap_handle_type   eap_handle,
  dsm_item_type   **sdu
)
{

  eap_server_handle_type       server_lyr_handle;
  eap_server_method_enum_type  requested_method;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------- 
    Free the NAK packet since we don't care about its contents as of now
  -------------------------------------------------------------------------*/
  if (!((sdu == NULL) || (*sdu == NULL)))
  {
    dsm_free_packet(sdu);
  }

  server_lyr_handle = eap_get_server_lyr_handle (eap_handle);
  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_ERROR_0("Invalid Server layer instance");
    return;
  }

  /*------------------------------------------------------------------------- 
    As of today in case of NAK the extended method's result is called
   with failure.
  -------------------------------------------------------------------------*/
  eap_server_lyr_cb[server_lyr_handle]->auth_info.result = EAP_RESULT_FAILURE; 

  requested_method = eap_serveri_get_type_index(
                   eap_server_lyr_cb[server_lyr_handle]->current_method_type);
  
  if ((EAP_SERVER_MAX_METHOD == requested_method)||
      (NULL == eap_server_method_result_cback[requested_method]))
  {
    LOG_MSG_ERROR_1("Server result cb is NULL or Invalid method %d",
                    requested_method );
    return;
  }

  /*-------------------------------------------------------------------------
    Pass the pending packet up to the concerned method
  -------------------------------------------------------------------------*/
  eap_server_method_result_cback[requested_method](
                      eap_server_lyr_cb[server_lyr_handle]->eap_handle,
                      eap_server_lyr_cb[server_lyr_handle]->auth_info.result);

  if( (NULL != eap_server_method_supp_result_cback[requested_method]) &&
      (TRUE == eap_eap_supp_result_f_ind_ptr_is_valid(
                            eap_server_lyr_cb[server_lyr_handle]->eap_handle)) )
  {
    eap_server_method_supp_result_cback[requested_method]
        ( eap_server_lyr_cb[server_lyr_handle]->eap_handle, 
          eap_server_lyr_cb[server_lyr_handle]->auth_info.result );    
  }
}/*eap_handle_nak_response*/

/*===========================================================================
FUNCTION EAP_SERVERI_HANDLE_RESPONSE

DESCRIPTION
  Function to handle the EAP responses.

DEPENDENCIES
  Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void eap_serveri_handle_response
( 
  eap_server_handle_type   server_lyr_handle
)
{
  eap_server_method_enum_type requested_method;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_ERROR_0("Invalid Server layer instance" );
    return;
  }

  /*-------------------------------------------------------------------------
    Ensure that the current type we are expecting matches that of the 
    pending pkt we wish to process. 
  -------------------------------------------------------------------------*/
  if (eap_server_lyr_cb[server_lyr_handle]->pending_pkt_type != 
      eap_server_lyr_cb[server_lyr_handle]->current_method_type)
  {
    /*-----------------------------------------------------------------------
      NAK can come anytime during AUTH stage. Currently the method type is 
      EXT in auth stage
    -----------------------------------------------------------------------*/
    if (!((EAP_SERVER_NAK_TYPE == eap_server_lyr_cb[
                                        server_lyr_handle]->pending_pkt_type)
        && (EAP_SERVER_EXT_TYPE == eap_server_lyr_cb[
                                     server_lyr_handle]->current_method_type))
       )
    {
      /*---------------------------------------------------------------------
        Different authentication method already in progress
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_0("Invalid EAP auth response from Peer");
      dsm_free_packet(&eap_server_lyr_cb[server_lyr_handle]->pending_pkt);
      return;
    }
  }

  requested_method = eap_serveri_get_type_index( 
    eap_server_lyr_cb[server_lyr_handle]->current_method_type );

  
  if ((EAP_SERVER_MAX_METHOD == requested_method )||
      (NULL == eap_server_method_input_cback[requested_method]))
  {
    LOG_MSG_ERROR_0("Server input cb is NULL");
    dsm_free_packet(&eap_server_lyr_cb[server_lyr_handle]->pending_pkt);
    return;
  }

  /*-------------------------------------------------------------------------
    Pass the pending packet up to the concerned method
  -------------------------------------------------------------------------*/
  eap_server_method_input_cback[requested_method](
                          eap_server_lyr_cb[server_lyr_handle]->eap_handle,
                          &eap_server_lyr_cb[server_lyr_handle]->pending_pkt);

}/*eap_serveri_handle_response*/


/*===========================================================================
FUNCTION EAP_SERVER_RTO_CBACK_TASK

DESCRIPTION
  Function called from EAP client after task context switch

PARAMETERS
  void *user_data - Data passed to the cleint callback before context switch

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_rto_cback_task
(
  void *user_data
)
{
  eap_server_lyr_cb_type *eap_server_cb;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if (NULL == user_data)
  {
    LOG_MSG_ERROR_0("Server layer instance is NULL" );
    return;
  }

  eap_server_cb = (eap_server_lyr_cb_type *)user_data;

  /*-------------------------------------------------------------------------
    Re-transmit the packet & increment the re-transmissions done
    Currently max allowed re-transmission is 3
  -------------------------------------------------------------------------*/
  eap_eap_rexmit_req_pkt (eap_server_cb->eap_handle);
  eap_server_cb->timer_info.retrans_tries++;
  if (EAP_SERVER_MAX_RE_TX <= eap_server_cb->timer_info.retrans_tries)
  {
    LOG_MSG_INFO1_0("Max re-transmissions done");
    return;
  }

  /*-------------------------------------------------------------------------
    According to Karn's algo, RTO <- RTO * 2 after a re-transmission
  -------------------------------------------------------------------------*/
  eap_server_cb->timer_info.rto = eap_server_cb->timer_info.rto * 2;
  (void)rex_set_timer(&(eap_server_cb->timer_info.timer_hdl), 
                      eap_server_cb->timer_info.rto);

} /* eap_server_rto_cback_task() */

/*===========================================================================
FUNCTION EAP_SERVER_RTO_CBACK

DESCRIPTION
  This is callback is called when the EAP re-transmission timer expires

PARAMETERS
  void *user_data - The control block passed when the timer handle is created

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_rto_cback
(
  unsigned long user_data
)
{
  eap_server_lyr_cb_type *eap_server_cb;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  eap_server_cb = (eap_server_lyr_cb_type *)user_data;
  if (NULL == eap_server_cb)
  {
    LOG_MSG_ERROR_0("Server layer instance is NULL" );
    return;
  }

  (void)rex_clr_timer(&eap_server_cb->timer_info.timer_hdl);

  LOG_MSG_INFO2_0("Switching to EAP's client context...");
  if (FALSE == eap_server_cb->timer_info.timer_f_ptr (
                                  eap_server_rto_cback_task,
                                  (void*)eap_server_cb,
                                  eap_server_cb->timer_info.timer_user_data))
  {
    LOG_MSG_ERROR_0("Unable to switch task");
  }

  return;
} /* eap_server_rto_cback() */

/*===========================================================================
FUNCTION EAP_SERVER_CALCULATE_RTO

DESCRIPTION
  This is used to do RTO related calculations

PARAMETERS
  server_lyr_handle - EAP server layer handle

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_calculate_rto
(
  eap_server_handle_type  server_lyr_handle
)
{
  eap_server_retransmission_info *tmr_info = NULL;
  uint32 abs_val;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (FALSE == EAP_SERVER_HANDLE_IS_VALID(server_lyr_handle))
  {
    LOG_MSG_ERROR_0("Invalid Server layer instance" );
    return;
  }

  tmr_info = &eap_server_lyr_cb[server_lyr_handle]->timer_info;
  /*-------------------------------------------------------------------------
    According to RFC 2988
    1st time - SRTT <- R, RTTVAR <- R/2
    Later -  RTTVAR <- (1 - beta) * RTTVAR + beta * |SRTT - R'|
             SRTT <- (1 - alpha) * SRTT + alpha * R'
             where alpha = 1/8 & beta = 1/4

    Re-transmision timeout, RTO <- SRTT + max (G, K*RTTVAR) where K = 4, 
    G <= 100 msec
  -------------------------------------------------------------------------*/
  if (FALSE == tmr_info->init_flag)
  {

    tmr_info->srtt = tmr_info->rtt;
    tmr_info->rttvar = tmr_info->rtt/2;
    tmr_info->init_flag = TRUE;
  }

  else
  {
    abs_val = (tmr_info->rtt > tmr_info->srtt) ? 
             tmr_info->rtt - tmr_info->srtt : tmr_info->srtt - tmr_info->rtt;
    tmr_info->rttvar = ((EAP_SERVER_DGAIN-1)*tmr_info->rttvar + abs_val 
                        + (EAP_SERVER_DGAIN/2)) >> EAP_SERVER_LDGAIN;
    tmr_info->srtt = ((EAP_SERVER_AGAIN-1)*tmr_info->srtt + tmr_info->rtt 
                      + (EAP_SERVER_AGAIN/2)) >> EAP_SERVER_LAGAIN;
  }

  tmr_info->rto = tmr_info->srtt + MAX(EAP_SERVER_CONST_G, 
                                      EAP_SERVER_CONST_K * tmr_info->rttvar);

  /*-------------------------------------------------------------------------
    According to RFC 2988, if RTO is < 1 sec then set it to 1 sec (1000 msec)
  -------------------------------------------------------------------------*/
  if (tmr_info->rto < EAP_SERVER_MIN_RTO_VAL)
  {
    tmr_info->rto = EAP_SERVER_MIN_RTO_VAL;
  }

} /* eap_server_calculate_rto() */

#endif /* FEATURE_DATA_PS_EAP */
