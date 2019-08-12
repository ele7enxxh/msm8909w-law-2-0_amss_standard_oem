#ifndef PS_MIP6_RSM_H
#define PS_MIP6_RSM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ M I P 6 _ R S M . H


GENERAL DESCRIPTION
  The implementation of the Mobile IPv6 Registration State Machine (RSM).

EXTERNALIZED FUNCTIONS

  ps_mip6_rsm_powerup_init()
    Performs the power up initialization for RSM.

  ps_mip6_rsm_alloc()
    Allocates an instance of the Registration State Machine and returns
    a handle to it.

  ps_mip6_rsm_config()
    Configures the RSM instance with the provided parameters.

  ps_mip6_rsm_free()
    Frees the allocated memory for RSM

  ps_mip6_rsm_post_event()
    Posts an event to the RSM state machine.

  ps_mip6_rsm_ind_cback_reg()
    Registers to receive indications from state machine

  ps_mip6_rsm_ind_cback_dereg()
    De-Registers the indication callback registered via reg function

  ps_mip6_input()
    Processes inbound packets for MIPv6 RSM

  ps_mip6_notify()
     Handles inbound ICMPv6 error packets for MIPv6

INITIALIZATION AND SEQUENCING REQUIREMENTS
  TBD
  
Copyright (c) 2006-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/ps_mip6_rsm.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_DATA_PS_MIPV6
#include "list.h"
#include "pstimer.h"
#include "dsm.h"
#include "ps_in.h"
#include "ps_mip6.h"
#include "ps_icmp6_msg.h"
#include "ps_tx_meta_info.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
struct ps_mip6_rsm_cb_s;

typedef struct ps_mip6_rsm_cb_s ps_mip6_rsm_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_RSM_STATE_TYPE

DESCRIPTION
  Enum of RSM states
---------------------------------------------------------------------------*/
typedef enum
{
  MIP6_RSM_MIN_STATE            = 0,
  MIP6_RSM_UNREGISTERED         = 0,
  MIP6_RSM_INITIAL_REGISTRATION = 1,
  MIP6_RSM_REGISTERED           = 2,
  MIP6_RSM_RE_REGISTRATION      = 3,
  MIP6_RSM_DE_REGISTRATION      = 4,
  MIP6_RSM_MAX_STATE
} ps_mip6_rsm_state_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_RSM_EVENT_TYPE

DESCRIPTION
  Enum of RSM events
---------------------------------------------------------------------------*/
typedef enum
{
  MIP6_RSM_MIN_EV         = 0,
  MIP6_RSM_REG_EV         = 0,
  MIP6_RSM_DEREG_EV       = 1,
  MIP6_RSM_REG_SUCCESS_EV = 2, /* cannot be posted by MSM  */
  MIP6_RSM_REG_FAIL_EV    = 3,    /* cannot be posted by MSM  */
  MIP6_RSM_CLOSE_EV       = 4,
  MIP6_RSM_MAX_EV
} ps_mip6_rsm_event_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_RSM_INDICATION_TYPE

DESCRIPTION
  Enum of RSM indications
---------------------------------------------------------------------------*/
typedef enum
{
  MIP6_RSM_MIN_IND  = 0,
  MIP6_RSM_DOWN_IND = 0,
  MIP6_RSM_UP_IND   = 1,
  MIP6_RSM_MAX_IND
} ps_mip6_rsm_indication_type;

/*---------------------------------------------------------------------------
   #defines for constants and default values
---------------------------------------------------------------------------*/
#define MIP6_RSM_DEFAULT_REG_RETRIES         3
#define MIP6_RSM_DEFAULT_DEREG_RETRIES       1

#define MIP6_RSM_BASE_BA_TIMEOUT             1000          /* milliseconds */
#define MIP6_RSM_BASE_BA_TIMEOUT_FIRST_REG   1500          /* milliseconds */
#define MIP6_RSM_MAX_BA_TIMEOUT              32000         /* milliseconds */

#define MIP6_MAX_NAI_LENGTH                  72
#define MIP6_RSM_MN_AAA_KEY_LENGTH           16
#define MIP6_RSM_MN_HA_KEY_LENGTH            16
#define MIP6_RSM_HMAC_SHA1_SPI               3
#define MIP6_RSM_3GPP2_SPI                   5

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_RSM_AUTH_PARAMS_TYPE

DESCRIPTION
  The type that contains the RSM configuration parameters for the
  authentication protocol
---------------------------------------------------------------------------*/
typedef struct ps_mip6_rsm_auth_params_type
{
  uint8   nai[MIP6_MAX_NAI_LENGTH];               /* NAI                   */
  uint8   nai_len;                                /* NAI Length            */
  boolean use_autogen_mn_ha_key;                  /* Autogen MN-HA Key ?   */
  uint8   mn_ha_key[MIP6_RSM_MN_HA_KEY_LENGTH];   /* MN-HA shared secret   */
  uint8   mn_ha_key_len;                          /* MN-HA SS Length       */
  uint8   mn_aaa_key[MIP6_RSM_MN_AAA_KEY_LENGTH]; /* MN-AAA shared secret  */
  uint8   mn_aaa_key_len;                         /* MN-AAA SS Length      */
  uint32  mn_ha_spi;                              /* MN-HA SPI             */
  uint32  mn_aaa_spi;                             /* MN-AAA SPI            */
} ps_mip6_rsm_auth_params_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_RSM_CONFIG_TYPE

DESCRIPTION
  The type that contains the RSM configuration parameters that can be
  passed in to a mip6_rsm_init() call.
---------------------------------------------------------------------------*/
typedef struct ps_mip6_rsm_config_type
{
  uint16  max_reg_retries;                    /* Max BU reg retries        */
  uint16  max_dereg_retries;                  /* Max BU dereg retries      */
  uint32  initial_bindack_timeout_first_reg;  /* Initial BA timeout (ms)   */
  uint32  base_bindack_timeout;               /* Base BA timeout (ms)      */
  boolean home_reg;                           /* True if Regn to HA        */
  boolean use_auth_proto;                     /* Use Auth protocol ?       */
  ps_mip6_rsm_auth_params_type auth_params;   /* Auth proto config         */
} ps_mip6_rsm_config_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_RSM_BINDING_INFO

DESCRIPTION
  This is the type for binding information passed by the client (Meta SM)
  while posting MIP6_RSM_REG_EV or MIP6_RSM_DEREG_EV
---------------------------------------------------------------------------*/
typedef struct ps_mip6_rsm_binding_info
{
  struct ps_in6_addr bu_dest;    /* could be HA or CN */
  struct ps_in6_addr hoa;
  struct ps_in6_addr coa;
} ps_mip6_rsm_binding_info;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_RSM_EVENT_DATA_TYPE

DESCRIPTION
  This is the type for the information passed by the client (Meta SM)
  alongwith the MIP6_RSM_REG_EV or MIP6_RSM_DEREG_EV
---------------------------------------------------------------------------*/
typedef struct ps_mip6_rsm_event_data_type
{
  ps_mip6_rsm_binding_info  binding_info;
  ps_tx_meta_info_type      *meta_info_ptr;
  void                      *esp_sa_ptr;
} ps_mip6_rsm_event_data_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_RSM_INDICATION_INFO_TYPE

DESCRIPTION
  This is the type for the information passed by RSM via indications
  (UP/DOWN) to the MSM.
---------------------------------------------------------------------------*/
typedef struct ps_mip6_rsm_indication_info_type
{
  ps_mip6_rsm_indication_type type;
  boolean                     k_bit;
  ps_mip6_fail_reason_type    fail_reason;

  /*-------------------------------------------------------------------------
    DNS information (obtained via VS option in the BA) - relevant for UP ind
  -------------------------------------------------------------------------*/
  struct ps_in6_addr          primary_dns;   
  struct ps_in6_addr          secondary_dns; 
} ps_mip6_rsm_indication_info_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_RSM_CBACK_TYPE

DESCRIPTION
  This is the type for the callback registered by the client (Meta SM) to
  receive indications from this state machine
---------------------------------------------------------------------------*/
typedef void (*ps_mip6_rsm_cback_type)
(
  ps_mip6_rsm_type                 * instance,
  ps_mip6_rsm_indication_info_type * ind_info_ptr,
  void                             * cback_data_ptr
);

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_RSM_CB_S

DESCRIPTION
  This is the type for the state machine control block. One instance of
  this control block corresponds to one binding in the Meta-SM.
---------------------------------------------------------------------------*/
struct ps_mip6_rsm_cb_s
{
  list_link_type link;                /* Link to next RSM in BU list       */

  ps_mip6_rsm_state_type state;       /* Current RSM state                 */

  uint16 last_seqno;                  /* value of seqno sent in last BU    */

  uint16 max_reg_retries;             /* Max tries allowed for Reg'n       */
  uint16 max_dereg_retries;           /* Max tries allowed for Dereg'n     */
  uint16 retries;                     /* Number of retries attempted       */

  ps_mip6_rsm_binding_info  binding_info; /* Binding Information           */

  uint32 initial_lifetime_val;        /* Initial lifetime val in BU (ms)   */
  uint32 rebind_timeout;              /* Re-bind timeout (ms)              */
  uint32 initial_bindack_timeout_first_reg;  /* Initial BA timeout (ms)    */
  uint32 base_bindack_timeout;        /* base timeout to resend BU (ms)    */
  uint32 bindack_timeout;             /* bindack timeout BU (ms)           */

  ps_timer_handle_type lifetime_timer;/* lifetime timer handle             */
  ps_timer_handle_type bindack_timer; /* bindack timer handle              */
  ps_timer_handle_type rebind_timer;  /* rebind timer handle               */

  ps_tx_meta_info_type *meta_info_ptr; /* Cached meta info                 */
  void              *esp_sa_ptr;      /* ESP SA for binding                */

  boolean home_reg;                   /* True if a registration to HA      */
  boolean do_not_send_bu;             /* Dont send any further BU's to
                                         bu_dest                           */
  boolean k_bit;                      /* K-Bit (unused right now)          */
  ps_mip6_fail_reason_type fail_reason; /* Fail reason                     */
  struct
  {
    ps_mip6_rsm_cback_type  cback;    /* indication callback function     */
    ps_mip6_rsm_indication_info_type info; /* indication information       */
    void                  * cookie_ptr; /* callback user data              */
  } ind;                              /* Data to send RSM indications     */

  uint32  ntp_secs;                   /* NTP timestamp in seconds         */
  uint32  ntp_fsecs;                  /* NTP timestamp in fractional secs */
  int     time_sync_offset;           /* Offset between HA and MN clock   */
  boolean use_auth_proto;                     /* Use Auth protocol ?       */
  ps_mip6_rsm_auth_params_type auth;          /* Auth parameters           */
  struct ps_mip6_rsm_cb_s *this_rsm_ptr;      /* self pointer              */
};

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_MIP6_RSM_POWERUP_INIT()

DESCRIPTION
  Performs the power up initialization for RSM.
  (1) Initializes memory for use by the RSM engine
  (2) Sets command handlers for commands handled by RSM

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mip6_rsm_powerup_init( void );

/*===========================================================================
FUNCTION PS_MIP6_RSM_ALLOC()

DESCRIPTION
  Allocates an instance of the Registration State Machine and returns
  a handle to it.

PARAMETERS
  None

RETURN VALUE
   Pointer to the state machine instance if successful, NULL otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_mip6_rsm_type * ps_mip6_rsm_alloc( void );

/*===========================================================================
FUNCTION PS_MIP6_RSM_CONFIG()

DESCRIPTION
  Configures the RSM instance with the provided parameters.
  If values are not provided for some parameters, the defaults are used.

PARAMETERS
  instance_ptr: Pointer to the RSM
  config_ptr: Pointer to the configuration parameters structure

RETURN VALUE
  0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_mip6_rsm_config
(
  ps_mip6_rsm_type        * instance_ptr,
  ps_mip6_rsm_config_type * config_ptr
);

/*===========================================================================
FUNCTION PS_MIP6_RSM_FREE()

DESCRIPTION
  Free the allocated memory for RSM

PARAMETERS
  Pointer to pointer to the instance structure

RETURN VALUE
   0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_mip6_rsm_free
(
  void ** instance_ptr_ptr
);

/*===========================================================================
FUNCTION PS_MIP6_RSM_POST_EVENT()

DESCRIPTION
  This function is used to post an event to the state machine.
  Supported events: ps_mip6_rsm_event_type

PARAMETERS
  instance: Pointer to the RSM control block
  event:    Event to be posted to RSM
  event_data: Pointer to the event_data data structure

RETURN VALUE
   0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mip6_rsm_post_event
(
  ps_mip6_rsm_type            * instance_ptr,
  ps_mip6_rsm_event_type        event,
  ps_mip6_rsm_event_data_type * event_data_ptr
);

/*===========================================================================
FUNCTION PS_MIP6_RSM_IND_CBACK_REG()

DESCRIPTION
  This function is used to register a callback function to receive
  indications from the state machine

PARAMETERS
  instance: Pointer to the RSM control block
  cback:    Callback function to be registered
  cback_data_ptr: Opaque pointer to be returned with callback function

RETURN VALUE
   0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_mip6_rsm_ind_cback_reg
(
  ps_mip6_rsm_type       * instance_ptr,
  ps_mip6_rsm_cback_type   cback,
  void                   * cback_data_ptr
);

/*===========================================================================
FUNCTION PS_MIP6_RSM_IND_CBACK_DEREG()

DESCRIPTION
  This function is used to de-register the callback function for receiving
  indications from the state machine

PARAMETERS
  instance: Pointer to the RSM control block

RETURN VALUE
   0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mip6_rsm_ind_cback_dereg
(
  ps_mip6_rsm_type * instance_ptr
);


/*===========================================================================
FUNCTION PS_MIP6_INPUT()

DESCRIPTION
  Processes inbound packets for MIPv6 RSM

PARAMETERS
  instance: Pointer to the RSM control block

RETURN VALUE
   0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_mip6_input
(
  dsm_item_type     * pkt_ptr,
  uint16            * offset_ptr,
  ip_pkt_info_type  * ip_pkt_info_ptr
);
/*===========================================================================
FUNCTION PS_MIP6_NOTIFY()

DESCRIPTION
  Handles inbound ICMPv6 error packets for MIPv6; indicates registration
  failure for the appropriate ICMPv6 error codes.

PARAMETERS
  icmp6_hdr_ptr : Pointer to ICMPv6 header
  src_addr_ptr  : Pointer to source address of ICMPv6 error packet
  dest_addr_ptr : Pointer to destination address of ICMPv6 error packet

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mip6_notify
(
  ps_icmp6_msg_type    * icmp6_hdr_ptr,
  struct ps_in6_addr   * src_addr_ptr,
  struct ps_in6_addr   * dest_addr_ptr
);


#endif /* FEATURE_DATA_PS_MIPV6 */
#endif /* PS_MIP6_RSM_H */
