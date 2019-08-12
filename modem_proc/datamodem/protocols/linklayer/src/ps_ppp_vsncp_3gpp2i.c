/*===========================================================================
               P S _ P P P _ V S N C P _ 3 G P P 2 I . C

DESCRIPTION
  PPP-Internal definitions for the 3gpp2-VSNCP protocol implementation

  Copyright (c) 2003-2016 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_vsncp_3gpp2i.c#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
05/13/14    cx     Add support for default APN indication option
02/26/14    cx     Fixed NULL dereference in PCO processing
01/28/14    cx     Add ignore MTU option with wrong length in PCO
10/14/13    cx     Fix LLVM warnings
10/11/13    cx     Add support for dropping unsolicited BCM in PCO
09/20/13    cx     Add support for BCM in PCO over eHRPD
09/19/13    cx     Add support for unsolicited MTU in PCO
09/19/13    cx     Set the correct MTU value in IPv6 and IPv4_v6 Call 
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
06/21/13    ash    Support MTU upto 2000
06/07/13    cx 	   Add MTU Size in PCO Over eHRPD
05/22/13    ash    Process fsm_timeout differently for normal network initiated
                   VSNCP termination.
03/21/13    ash    Use Default VSNCP Config values in absence of NV item.
01/08/13    op     Fix to set the v6 MTU value accordingly
12/10/12    ssh    NV provisioning for IPv6 MTU
12/02/12    jz     Support for Operator PCO  
10/25/12    fn     Remove usage of fatal error log messages
05/02/12    jz     allow non VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 ip ver capability
04/24/12    ash    Don't cleanup NCP cb while processing packet.
04/25/12    ash    Replaced memheap with modem_mem APIs
04/20/12    op     Added support to request DNS v4 address using PCO container
03/08/12    ash    Propagate call end reasons correctly
02/27/12    op     Fix to support NULL PAP password in PDN level auth
02/08/12    ash    Added NCPI_CB_IS_INITED Macro to check initilization status 
                   of VSNCP control block.
01/10/12    ash    Added support for IPCP config NAK in VSNCP
10/27/11    op     Added support for PDN level authentication 
08/30/11    vpk    Close PDN connection if IP address is not returned as 
                   part of Config-ack
06/06/11    ash    Added support for PPP Partial Context
06/07/11    ssh    Fix in vsncp timer callback
04/01/11    mga    Fix to gracefully return if NCP block is not allocated 
03/23/11    op     Data SU API cleanup
03/14/11    mga    Merged changes to set the MTU accordingly
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
02/02/11    mga    Made changes to request DNS address accordingly
01/07/11    cp     Made changes for MTU Propogation from the Network to the
                   UM IFACE.
12/08/10    ssh    *Ignore physlink state for PPP termination (eHRPD)*
10/26/10    mga    Fix in VSNCP opening function
10/13/10    mga    Added additional VSNCP specific fail reasons. 
10/11/10    mga    Fix in VSNCP option function
10/05/10    mga    Changes to ensure old values of addresses are cleaned up.
09/20/10    mga    Changes to ensure PDN type option is added correctly.
09/08/10    mga    Changes to propagate correct fail reason to Mode Handler
08/23/10    mga    Added additional VSNCP fail reasons to support throttling
08/18/10    jee    Compiler warning fix merged from 9K 2.0
08/19/10    mga    Added support for sending VSNCP term for last VSNCP
08/16/10    cp     Propagating the first hop mtu value to TE.
05/13/10    ssh    Added support for PDN Inactivity timer
4/19/2010   ash    Added changes to support Dual IP Feature.
01/07/10    ssh    Free NCP control block memory using new macro
11/25/09    mga    Added checks in vsncp_3gpp2i_abort accordingly before
                   calling the event callback
10/05/09    mga    Merged changes related to diag events
08/14/09    ash    Retrive VSNCP 3GPP2 config & fsm constants for STA.
07/23/09    ssh    Free timer when clearing up NCP
07/21/09  mga/ssh  Fix in vsncp_3gpp2i_check()
07/17/09    jdev   Fixed lint errors
07/08/09    mga    Fix in vsncp_3gpp2i_option()
04/15/09    ssh    Fixes to P-CSCF processing; some refactoring
04/06/09    ssh    Now setting the iface MTU; a couple F3 messages.
04/03/09    mga    Fix in vsncp_3gpp2i_check() for PDN address
04/03/09  mga/ssh  Fixes in vsncp_3gpp2i_check(), vsncp_3gpp2_ack()
03/23/09    mga    Removed some initializations and added additional checks.
03/15/09    mga    Made changes to support Multi-PDN
03/12/09    ssh    Some refactoring, code review comments. A cleanup fix.
02/20/09    mga    VSNCP spec updates
02/20/09    ssh    VSNCP spec updates (Config-rej; req_addr_type);
                   Couple fixes (PCO, APN); some additional comments
01/23/09    ssh    Created module
===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#ifdef FEATURE_DATA_PS_EHRPD
#include "modem_mem.h"
#include "ps_ppp_vsncp_3gpp2.h"
#include "ps_ppp_ncp.h"
#include "ps_ppp_ncpi.h"
#include "ps_ppp_fsm.h"
#include "ps_pppi_fsm.h"
#include "ps_pppi_lcp.h"
#include "ps_ppp_ipcp.h"
#include "ps_pppi.h"
#include "ps_iface.h"
#include "ps_ifacei_utils.h"
#include "ps_ip_addr.h"
#include "ps_iface_addr_v6.h"
#include "dsm.h"
#include "ps_mem.h"
#include "pstimer.h"
#include "ps_ppp_events.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_ppp_auth.h"
#include "ps_pppi_auth.h"
#include "ps_ppp_ext.h"
#include "ps_ppp_defs.h"
#include "ps_ip6_addr.h"

/*===========================================================================

                 DEFINITIONS AND DECLARATTIONS FOR MODULE

===========================================================================*/
/*===========================================================================
    TYPEDEFS
===========================================================================*/
/*---------------------------------------------------------------------------
  Masks for supported option types
---------------------------------------------------------------------------*/
typedef enum
{
  VSNCP_3GPP2I_N_PDN_ID           = 1 << 0,
  VSNCP_3GPP2I_N_APN              = 1 << 1,
  VSNCP_3GPP2I_N_PDN_TYPE         = 1 << 2,
  VSNCP_3GPP2I_N_PDN_ADDR         = 1 << 3,
  VSNCP_3GPP2I_N_PCO              = 1 << 4,
  VSNCP_3GPP2I_N_ERROR_CODE       = 1 << 5,
  VSNCP_3GPP2I_N_ATTACH_TYPE      = 1 << 6,
  VSNCP_3GPP2I_N_DEFAULT_ROUTER   = 1 << 7,
  VSNCP_3GPP2I_N_ADDR_ALLOC_CAUSE = 1 << 8,
  VSNCP_3GPP2I_N_DEFAULT_APN_IND  = 1 << 9
} vsncp_3gpp2i_option_type;

/*---------------------------------------------------------------------------
  PCO Container IDs
---------------------------------------------------------------------------*/
typedef enum
{
  /*-------------------------------------------------------------------------
    MS -> Network direction
  -------------------------------------------------------------------------*/
  VSNCP_3GPP2I_PCO_PCSCF_ADDR_REQ_ID        = 0x0001,
  VSNCP_3GPP2I_PCO_DNS_V6_ADDR_REQ_ID       = 0x0003,
  VSNCP_3GPP2I_PCO_MS_BCM_ID                = 0x0005,
  VSNCP_3GPP2I_PCO_IP_ADDR_ALLOC_VIA_NAS_ID = 0x000a,
  VSNCP_3GPP2I_PCO_V4_PCSCF_ADDR_REQ_ID     = 0x000c,
  VSNCP_3GPP2I_PCO_DNS_V4_ADDR_REQ_ID       = 0x000d,
#ifdef FEATURE_DS_MTU_PCO_VSNCP
  VSNCP_3GPP2I_PCO_MTU_REQ_ID               = 0x0010,
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
  /*-------------------------------------------------------------------------
    Network -> MS direction
  -------------------------------------------------------------------------*/
  VSNCP_3GPP2I_PCO_PCSCF_ADDR_ID            = 0x0001,
  VSNCP_3GPP2I_PCO_DNS_V6_ADDR_ID           = 0x0003,
  VSNCP_3GPP2I_PCO_SELECTED_BCM_ID          = 0x0005,
  VSNCP_3GPP2I_PCO_V4_PCSCF_ADDR_ID         = 0x000c,
  VSNCP_3GPP2I_PCO_DNS_V4_ADDR_ID           = 0x000d
#ifdef FEATURE_DS_MTU_PCO_VSNCP
  ,VSNCP_3GPP2I_PCO_MTU_ID                   = 0x0010
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
} vsncp_3gpp2i_pco_container_id_type;

/*---------------------------------------------------------------------------
  VSNCP Config-Rej Error codes
---------------------------------------------------------------------------*/
typedef enum
{
  VSNCP_3GPP2I_ERROR_MIN               = 0,
  VSNCP_3GPP2I_GEN_ERROR               = VSNCP_3GPP2I_ERROR_MIN,
  VSNCP_3GPP2I_UNAUTH_APN              = 1,
  VSNCP_3GPP2I_PDN_LIMIT_EXCEED        = 2,
  VSNCP_3GPP2I_NO_PDN_GW               = 3,
  VSNCP_3GPP2I_PDN_GW_UNREACH          = 4,
  VSNCP_3GPP2I_PDN_GW_REJ              = 5,
  VSNCP_3GPP2I_INSUFF_PARAM            = 6,
  VSNCP_3GPP2I_RESOURCE_UNAVAIL        = 7,
  VSNCP_3GPP2I_ADMIN_PROHIBIT          = 8,
  VSNCP_3GPP2I_PDN_ID_IN_USE           = 9,
  VSNCP_3GPP2I_SUBSCR_LIMITATION       = 10,
  VSNCP_3GPP2I_PDN_EXISTS_FOR_THIS_APN = 11,
  VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED   = 13,
  VSNCP_3GPP2I_ERROR_MAX
} vsncp_3gpp2i_error_code_type;

/*---------------------------------------------------------------------------
  Structure used to hold VSNCP NV configuration info
---------------------------------------------------------------------------*/
typedef struct vsncp_3gpp2i_nv_info_s
{
  uint16 req_try;
  uint16 term_try;
  uint32 ack_timeout;
  uint32 term_timeout;
} vsncp_3gpp2i_nv_info_type;

/*===========================================================================
    CONSTANTS AND MACROS
===========================================================================*/
/*---------------------------------------------------------------------------
  Minimum lengths for various supported options
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_PDN_ID_SIZ            3   /* fixed length             */
#define VSNCP_3GPP2I_APN_SIZ               2   /* NULL APN ok, min length  */
#define VSNCP_3GPP2I_PDN_TYPE_SIZ          3   /* fixed length             */
#define VSNCP_3GPP2I_PDN_ADDR_SIZ          7   /* minimum length           */
#define VSNCP_3GPP2I_PCO_SIZ               3   /* minimum length           */
#define VSNCP_3GPP2I_ERROR_CODE_SIZ        3   /* fixed length             */
#define VSNCP_3GPP2I_ATTACH_TYPE_SIZ       3   /* fixed length             */
#define VSNCP_3GPP2I_DEFAULT_ROUTER_SIZ    6   /* fixed length             */
#define VSNCP_3GPP2I_ADDR_ALLOC_CAUSE_SIZ  3   /* fixed length             */
#define VSNCP_3GPP2I_RECONNECT_IND_SIZ     3   /* fixed length             */
#define VSNCP_3GPP2I_DEFAULT_APN_IND_SIZ   3   /* fixed length             */
/*---------------------------------------------------------------------------
  Size and offset for PDN-ID within a VSNCP Packet
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_PDN_ID_VAL_SIZ  1         /* size of option value     */
#define VSNCP_3GPP2I_PDN_ID_OFFSET   9         /* vsncp pkt pdn-id offset  */

/*---------------------------------------------------------------------------
  PDN ID value range (0 through 14)
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_PDN_ID_MIN  0x00
#define VSNCP_3GPP2I_PDN_ID_MAX  0x0E

/*---------------------------------------------------------------------------
  Protocol Configuration Option (PCO) related defines
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  PCO configuration protocol (PPP with IP PDP-type)
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_PCO_CONFIG_PROTOCOL  0x80

/*---------------------------------------------------------------------------
  PCO Containers header length
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_PCO_CONTAINER_HDR_LEN  3

/*---------------------------------------------------------------------------
  PCO Container Lengths
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_PCO_PCSCF_ADDR_REQ_LEN         0
#define VSNCP_3GPP2I_PCO_V4_PCSCF_ADDR_REQ_LEN      0
#define VSNCP_3GPP2I_PCO_DNS_V4_ADDR_REQ_LEN        0
#define VSNCP_3GPP2I_PCO_DNS_V6_ADDR_REQ_LEN        0
#define VSNCP_3GPP2I_PCO_OP_PCO_REQUEST_LEN         3
#define VSNCP_3GPP2I_PCO_MS_BCM_LEN                 0
#define VSNCP_3GPP2I_PCO_IP_ADDR_ALLOC_VIA_NAS_LEN  0
#define VSNCP_3GPP2I_PCO_SELECTED_BCM_LEN           1
#ifdef FEATURE_DS_MTU_PCO_VSNCP
#define VSNCP_3GPP2I_PCO_MTU_REQ_LEN                0
#define VSNCP_3GPP2I_PCO_MTU_LEN                    2
#endif /* FEATURE_DS_MTU_PCO_VSNCP */

/*---------------------------------------------------------------------------
  Identifier and length of the IPCP Config-Req sent as part of PCO when
  requesting IPv4 primary and secondary DNS addresses
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_PCO_IPCP_C_REQ_ID  1
#define VSNCP_3GPP2I_PCO_IPCP_C_REQ_LEN                                     \
  ( CONFIG_HDR_LEN + IPCP_PRIMARY_DNS_SIZ + IPCP_SECONDARY_DNS_SIZ )

/*---------------------------------------------------------------------------
  Return codes from the _check() function that examines VSNCP options
---------------------------------------------------------------------------*/
#define RECOGNIZED    0
#define UNRECOGNIZED  1

/*---------------------------------------------------------------------------
  Defines for size of addresses
---------------------------------------------------------------------------*/
#define SIZE_OF_V4_ADDR  4
#define SIZE_OF_V6_IID   8
#define SIZE_OF_V6_ADDR  16

/*---------------------------------------------------------------------------
  The VSNCP_3GPP2I_N_OPTION_LIMIT is the number of unique 3GPP2-VSNCP
  options that can be negotiated.
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_N_OPTION_LIMIT 10

/*---------------------------------------------------------------------------
  Size of heap memory
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2I_MEM_BUFFER_SIZE  512

/*---------------------------------------------------------------------------
  Reconnect indication value, 1 -  not allowed
---------------------------------------------------------------------------*/
#define VSNCP_RECONN_IND_NOT_ALLOWED 1
/*---------------------------------------------------------------------------
  3GPP2-VSNCP options numbers based on the option mask
  (see vsncp_3gpp2i_option_type above)
----------------------------------------------------------------------------*/
static uint8 option_number[] =
{
  VSNCP_3GPP2I_PDN_ID,
  VSNCP_3GPP2I_APN,
  VSNCP_3GPP2I_PDN_TYPE,
  VSNCP_3GPP2I_PDN_ADDR,
  VSNCP_3GPP2I_PCO,
  VSNCP_3GPP2I_ERROR_CODE,
  VSNCP_3GPP2I_ATTACH_TYPE,
  VSNCP_3GPP2I_DEFAULT_ROUTER,
  VSNCP_3GPP2I_ADDR_ALLOC_CAUSE,
  VSNCP_3GPP2I_DEFAULT_APN_IND
};

/*---------------------------------------------------------------------------
  minimum lengths required for the supported options
---------------------------------------------------------------------------*/
static uint8 option_length[] =
{
  VSNCP_3GPP2I_PDN_ID_SIZ,
  VSNCP_3GPP2I_APN_SIZ,
  VSNCP_3GPP2I_PDN_TYPE_SIZ,
  VSNCP_3GPP2I_PDN_ADDR_SIZ,
  VSNCP_3GPP2I_PCO_SIZ,
  VSNCP_3GPP2I_ERROR_CODE_SIZ,
  VSNCP_3GPP2I_ATTACH_TYPE_SIZ,
  VSNCP_3GPP2I_DEFAULT_ROUTER_SIZ,
  VSNCP_3GPP2I_ADDR_ALLOC_CAUSE_SIZ,
  VSNCP_3GPP2I_DEFAULT_APN_IND_SIZ
};

/*---------------------------------------------------------------------------
  Macro - copies 'size' bytes from 'src_ptr' into 'buffer' at offset 'offset'
  Advances 'offset' by 'size'
---------------------------------------------------------------------------*/
#define COPY_BYTES( buffer, src_ptr, size, offset )                         \
{                                                                           \
  if(sizeof(buffer) > offset)                                               \
  {                                                                         \
    memscpy( &buffer[offset], sizeof(buffer)-offset,                        \
             (uint8 *)src_ptr, size);                                       \
    offset += size;                                                         \
  }                                                                         \
}

/*---------------------------------------------------------------------------
  Macro - dsm_pull's 'size' bytes from 'dsm_ptr_ptr' into 'dest_ptr'
  Retards 'count' by 'size'. Returns -1 on error.
---------------------------------------------------------------------------*/
#define PULLUP_BYTES( dsm_ptr_ptr, dest_ptr, size, count )                  \
{                                                                           \
  if( dsm_pullup( dsm_ptr_ptr, dest_ptr, size ) != size )                   \
  {                                                                         \
    return -1;                                                              \
  }                                                                         \
  count -= size;                                                            \
}

/*---------------------------------------------------------------------------
  Macro - dsm_pull's 'size' bytes from 'dsm_ptr_ptr' into 'dest_ptr'
  Retards 'count' by 'size'. Returns -1 on error.
---------------------------------------------------------------------------*/
#define TOSS_BYTES( dsm_ptr_ptr, size, count )                              \
{                                                                           \
  if( size > 0 )                                                            \
  {                                                                         \
    if( dsm_pullup( dsm_ptr_ptr, NULL, (uint16)size ) != (uint16)size )     \
    {                                                                       \
      LOG_MSG_ERROR("Unable to toss bytes!", 0, 0, 0);                          \
      return -1;                                                            \
    }                                                                       \
    count -= size;                                                          \
  }                                                                         \
}

/*===========================================================================
    VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  Operating structure for fsm; this gets written to during initialization
  and the function pointers and params are used by fsm.
---------------------------------------------------------------------------*/
static struct fsm_constant_s vsncp_3gpp2i_constants;
static struct cb_constant_s  vsncp_3gpp2i_cb_device_constants;

/*---------------------------------------------------------------------------
  Cached VSNCP NV parameters
---------------------------------------------------------------------------*/
static vsncp_3gpp2i_nv_info_type vsncp_3gpp2_nv_info;

/*---------------------------------------------------------------------------
  Set of must-have options that we propose in the C-Req
---------------------------------------------------------------------------*/
static negotiate_t vsncp_3gpp2i_propose = (negotiate_t) 
              ( (uint16) VSNCP_3GPP2I_N_PDN_ID      |
                (uint16) VSNCP_3GPP2I_N_APN         |
                (uint16) VSNCP_3GPP2I_N_PDN_TYPE    |
                (uint16) VSNCP_3GPP2I_N_PDN_ADDR    |
                (uint16) VSNCP_3GPP2I_N_PCO         |
                (uint16) VSNCP_3GPP2I_N_ATTACH_TYPE |
                (uint16) VSNCP_3GPP2I_N_ADDR_ALLOC_CAUSE );

/*===========================================================================

                           FORWARD DECLARATIONS

===========================================================================*/
static void vsncp_3gpp2i_free
(
  ppp_fsm_type * fsm_ptr
);

static void vsncp_3gpp2i_starting
(
  ppp_fsm_type * fsm_ptr
);

static void vsncp_3gpp2i_opening
(
  ppp_fsm_type * fsm_ptr
);

static void vsncp_3gpp2i_closing
(
  ppp_fsm_type * fsm_ptr
);

static void vsncp_3gpp2i_stopping
(
  ppp_fsm_type * fsm_ptr
);

static void vsncp_3gpp2i_option
(
  ppp_fsm_type      *  fsm_ptr,
  void              *  vp,
  struct dsm_item_s ** pkt_ptr_ptr,
  uint8                o_type
);

static int vsncp_3gpp2i_check
(
  ppp_fsm_type      *  fsm_ptr,
  struct option_hdr *  ohp,
  struct dsm_item_s ** pkt_ptr_ptr,
  int                  request
);

static int vsncp_3gpp2i_reject
(
  ppp_fsm_type      * fsm_ptr,
  struct option_hdr * ohp,
  dsm_item_type     * pkt_ptr
);

/*---------------------------------------------------------------------------
  VSNCP-3GPP2 constants - this MUST follow the forward
  declarations because many of these functions are used in this data
  structure. (The req_tries, term_tries, ack_timeout and term_timeout get
  replaced by values configured in the NV.)
---------------------------------------------------------------------------*/
static struct fsm_constant_s vsncp_3gpp2i_default_constants =
{
  PPP_VSNCP_PROTOCOL,       /* Protocol number                      */
  0x0,                      /* codes, unused in fsm                 */

  VSNCP,                    /* Finite State Machine index           */
  VSNCP_REQ_TRY,            /* # tries for configure                */
  0,                        /* # tries for nak substitutes - 0      */
  VSNCP_TERM_TRY,           /* # tries for terminate                */
  VSNCP_TERM_TIMEOUT,       /* Time for TERM REQ timeouts (msecs)   */
  VSNCP_ACK_TIMEOUT,        /* Time for C_ACK timeouts (msecs)      */

  VSNCP_3GPP2I_OPTION_LIMIT, /* maximum option number                */
  VSNCP_3GPP2I_N_OPTION_LIMIT, /* maximum mask number                */
  option_number,            /* mask table                           */

  PPP_NCP_COMPRESS_IGNORE,  /* compression, ignore for 3gpp2-vsncp  */

  vsncp_3gpp2i_free,         /* to free structure                    */
  fsm_down,                  /* layer layer down event               */

  vsncp_3gpp2i_starting,     /* this layer start: enter starting     */
  vsncp_3gpp2i_opening,      /* this layer opening: enter opening    */
  vsncp_3gpp2i_closing,      /* this layer closing: enter closing    */
  vsncp_3gpp2i_stopping,     /* this layers stopping: leave opened   */

  vsncp_3gpp2i_option,       /* add option to message                */
  vsncp_3gpp2i_check,        /* validate option and store if good    */
  vsncp_3gpp2i_reject        /* process rx'ed config reject          */
};

/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION VSNCP_3GPP2I_MEM_ALLOC()

DESCRIPTION
  This function is used to allocate memory from the heap

PARAMETERS
  (IN) count: Number of bytes to allocate

DEPENDENCIES
  vsncp_3gpp2_powerup_init() must have been called before this

RETURN VALUE
  void * : Pointer to the allocated memory

SIDE EFFECTS
  None
===========================================================================*/
static void * vsncp_3gpp2i_mem_alloc
(
  size_t size
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return modem_mem_alloc( size, MODEM_MEM_CLIENT_DATACOMMON_CRIT );
} /* vsncp_3gpp2i_mem_alloc() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_MEM_FREE()

DESCRIPTION
  This function is used to free memory allocated from the heap

PARAMETERS
  (IN) buf: Pointer to the memory to be freed

DEPENDENCIES
  vsncp_3gpp2_powerup_init() and vsncp_3gpp2i_mem_alloc()
  must have been called before this.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void vsncp_3gpp2i_mem_free
(
  void * buf
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( buf == NULL )
  {
    DATA_ERR_FATAL("Freeing NULL buffer");
    return;
  }

  modem_mem_free( buf, MODEM_MEM_CLIENT_DATACOMMON_CRIT );
} /* vsncp_3gpp2i_mem_free() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_PCO_MEM_FREE()

DESCRIPTION
  This function is used to free memory allocated from the heap for all
  PCOs (these are the IPv4 and IPv6 P-CSCF addresses and the IPv6 DNS
  addresses)

PARAMETERS
  (IN) ncp_cb_ptr : Pointer to the NCP control block

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void vsncp_3gpp2i_pco_mem_free
(
  ncpi_cb_type * ncp_cb_ptr
)
{
  vsncp_3gpp2i_pco_v4_addr * p_cscf_v4_ptr = NULL;
  vsncp_3gpp2i_pco_v4_addr * temp_v4_ptr = NULL;
  vsncp_3gpp2i_pco_v6_addr * p_cscf_v6_ptr = NULL;
  vsncp_3gpp2i_pco_v6_addr * dns_v6_ptr = NULL;
  vsncp_3gpp2i_pco_v6_addr * temp_v6_ptr = NULL;
  vsncp_3gpp2i_type        * vsncp_cb_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NCPI_CB_IS_INITED( ncp_cb_ptr ) == FALSE )
  {
    DATA_ERR_FATAL("Invalid handle for vsncp_3gpp2 pco mem free");
    return;
  }

  vsncp_cb_ptr = &(ncp_cb_ptr->cb_u.vsncp_3gpp2);

  /*-----------------------------------------------------------------------
    Free heap memory for IPv4 P-CSCF addresses
  -----------------------------------------------------------------------*/
  p_cscf_v4_ptr = vsncp_cb_ptr->pco_vals.p_cscf_v4_head_ptr;
  while( p_cscf_v4_ptr != NULL )
  {
    temp_v4_ptr = p_cscf_v4_ptr->next;
    vsncp_3gpp2i_mem_free( p_cscf_v4_ptr );
    p_cscf_v4_ptr = temp_v4_ptr;
  }
  vsncp_cb_ptr->pco_vals.p_cscf_v4_head_ptr = NULL;

  /*-----------------------------------------------------------------------
    Free heap memory for IPv6 P-CSCF addresses
  -----------------------------------------------------------------------*/
  p_cscf_v6_ptr = vsncp_cb_ptr->pco_vals.p_cscf_v6_head_ptr;
  while( p_cscf_v6_ptr != NULL )
  {
    temp_v6_ptr = p_cscf_v6_ptr->next;
    vsncp_3gpp2i_mem_free( p_cscf_v6_ptr );
    p_cscf_v6_ptr = temp_v6_ptr;
  }
  vsncp_cb_ptr->pco_vals.p_cscf_v6_head_ptr = NULL;

  /*-----------------------------------------------------------------------
    Free heap memory for IPv6 DNS addresses
  -----------------------------------------------------------------------*/
  dns_v6_ptr = vsncp_cb_ptr->pco_vals.dns_v6_head_ptr;
  while( dns_v6_ptr != NULL )
  {
    temp_v6_ptr = dns_v6_ptr->next;
    vsncp_3gpp2i_mem_free( dns_v6_ptr );
    dns_v6_ptr = temp_v6_ptr;
  }
  vsncp_cb_ptr->pco_vals.dns_v6_head_ptr = NULL;

} /* vsncp_3gpp2i_pco_mem_free() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_FREE()

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void vsncp_3gpp2i_free
(
  ppp_fsm_type * fsm_ptr
)
{
  LOG_MSG_INFO2_0("vsncp_3gpp2i_free()");
  /* NOOP */
  return;
} /* vsncp_3gpp2i_free() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_ABORT()

DESCRIPTION
  This function is used to free up a VSNCP instance - cancel timers,
  free up memory, post apporpriate events etc.

PARAMETERS
  (IN) ncp_cb_ptr : Pointer to the NCP control block

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2i_abort
(
  ncpi_cb_type * ncp_cb_ptr
)
{
  vsncp_3gpp2i_type * vsncp_cb_ptr;
  ppp_event_vsncp_payload_type ppp_event_vsncp;
  ppp_type               *ppp_cb_ptr;
  ppp_type               *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1("Cleaning up NCP handle %p", ncp_cb_ptr);
  if( ncp_cb_ptr != NULL )
  {
    vsncp_cb_ptr = &(ncp_cb_ptr->cb_u.vsncp_3gpp2);
    ppp_cb_array = fsm_get_ppp_cb_array();
    ppp_cb_ptr = ppp_cb_array + (uint8)vsncp_cb_ptr->fsm.device;
    /*-----------------------------------------------------------------------
      Free FSM timer
    -----------------------------------------------------------------------*/
    if( vsncp_cb_ptr->fsm.timer != 0 )
    {
      (void)ps_timer_free( vsncp_cb_ptr->fsm.timer );
    }
    vsncp_cb_ptr->fsm.mode = PPP_CP_MODE_INVALID;

    /*-----------------------------------------------------------------------
      Reset the activity callback information
    -----------------------------------------------------------------------*/
    vsncp_cb_ptr->config.activity_cb.func = NULL;
    vsncp_cb_ptr->config.activity_cb.data = NULL;

    /*-----------------------------------------------------------------------
      Free PCO-related heap memory (this is a catch-all really, if for some
      reason this did not happen correctly right after negotiation ended
      (i.e. in the _opening() function
    -----------------------------------------------------------------------*/
    vsncp_3gpp2i_pco_mem_free( ncp_cb_ptr );

    /*-----------------------------------------------------------------------
      Post events to clients and DIAG and free NCP control block memory
    -----------------------------------------------------------------------*/
    if( ppp_cb_ptr->phase != pppDEAD )
    {
      /*-------------------------------------------------------------------------
        With new eHRPD MH design DSSNET/Devmgr. DSSNet need to know if VSNCP
        went down because of LCP/AUTH failure to report correct error code.
        Thus propogate LCP fail_reason to VSNCP DOWN event for LCP failure.
        The assumption here is that LCP fail_reason would be set when it's
        going down. There are three possible LCP failure cases as mentioned below.
        1. LCP timeout: fail_reason would be set to PPP_FAIL_REASON_TIMEOUT
                        and LCP will be in going down state.
        2. Auth failure: If network terminates LCP during Auth phase,
                         fail_reason would be set to
                         PPP_FAIL_REASON_AUTH_FAILURE.
        3. No EAP: If Network/UE doesnt agree to do EAP for authentication.
                   LCP will be terminated with fail_reason as
                   PPP_FAIL_REASON_OPTIONS_MISMATCH.
     -------------------------------------------------------------------------*/
      if( ppp_cb_array->fsm[LCP].fail_reason != PPP_FAIL_REASON_NONE &&
          vsncp_cb_ptr->fsm.fail_reason == PPP_FAIL_REASON_NONE )
      {
        vsncp_cb_ptr->fsm.fail_reason = ppp_cb_array->fsm[LCP].fail_reason;
      }

      pppi_call_event_callback( &(vsncp_cb_ptr->fsm),
                              PPP_PROTOCOL_VSNCP,
                              PPP_DOWN_EV );
    }
    pppi_report_event_to_diag( EVENT_PPP,
                               ncp_cb_ptr->device,
                               PPP_PROTOCOL_VSNCP,
                               PS_PPP_EVENT_UNKNOWN,
                               PS_PPP_EVENT_CLOSED );
    /*-------------------------------------------------------------------
      Diag event indicating that VSNCP bring up failed
    ---------------------------------------------------------------------*/
    ppp_event_vsncp.ppp_event_device = (uint8)vsncp_cb_ptr->fsm.device;
    ppp_event_vsncp.pdn_id           =
      vsncp_cb_ptr->config.option_vals.pdn_id;
    ppp_event_vsncp.pdn_type         =
      vsncp_cb_ptr->config.option_vals.req_addr_type;
    ppp_event_vsncp.ppp_event_state  = PS_PPP_EVENT_CLOSED;
    event_report_payload( EVENT_VSNCP,
                          sizeof(ppp_event_vsncp),
                          &ppp_event_vsncp );
    PS_MEM_FREE( ncp_cb_ptr );
  } /* valid ncp_cb_ptr */
} /* vsncp_3gpp2i_abort() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_STARTING()

DESCRIPTION
  This function is used to prepare to begin the 3GPP2-VSNCP negotiation

PARAMETERS
  (IN) fsm_ptr : Pointer to the FSM structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void vsncp_3gpp2i_starting
(
  ppp_fsm_type * fsm_ptr
)
{
  ppp_event_vsncp_payload_type ppp_event_vsncp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Generate a PS_PPP_EVENT_STARTING event.
  -------------------------------------------------------------------------*/
  pppi_report_event_to_diag( EVENT_PPP,
                             fsm_ptr->device,
                             PPP_PROTOCOL_VSNCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_STARTING );

  ppp_event_vsncp.pdn_id           = (((ncpi_cb_type*)fsm_ptr->pdv)->cb_u.
                                      vsncp_3gpp2.config).option_vals.pdn_id;
  ppp_event_vsncp.pdn_type         =
   (((ncpi_cb_type*)fsm_ptr->pdv)->cb_u.vsncp_3gpp2.config).
    option_vals.req_addr_type;
  ppp_event_vsncp.ppp_event_device = (uint8)fsm_ptr->device;
  ppp_event_vsncp.ppp_event_state  = PS_PPP_EVENT_STARTING;

 /*-------------------------------------------------------------------------
   Reset bcm value for both UE-initiated and network-initiated VSNCP resyncs
   -------------------------------------------------------------------------*/
  ((ncpi_cb_type*)fsm_ptr->pdv)->cb_u.vsncp_3gpp2.pco_vals.selected_bcm =0;

  /*-------------------------------------------------------------------------
   Reset default apn flag for resync
  -------------------------------------------------------------------------*/
  ((ncpi_cb_type*)fsm_ptr->pdv)->cb_u.
    vsncp_3gpp2.local_vals.apn_is_default = 0;

  event_report_payload( EVENT_VSNCP,
                        sizeof(ppp_event_vsncp),
                        &ppp_event_vsncp );


  /*-------------------------------------------------------------------------
    We set the local working values (local.work) to the options we want to
    include in our C-Req (local.want) and the remote working values to FALSE
    since we haven't received a C-Req from the peer yet
  -------------------------------------------------------------------------*/
  fsm_ptr->local.work = fsm_ptr->local.want;
  fsm_ptr->remote.work = 0;
} /* vsncp_3gpp2i_starting() */
/*===========================================================================
FUNCTION VSNCP_SET_IFACE_OP_PCO_INFO()

DESCRIPTION
  Set Operator PCO info to iface

PARAMETERS
  (IN) iface_ptr : Pointer to iface
  (IN) op_pco_content_ptr: Pointer to operator PCO content

DEPENDENCIES
  None

RETURN VALUE
  -1: set info fails
  0: success

SIDE EFFECTS
  None
===========================================================================*/
static int vsncp_set_iface_op_pco_info
(
  ps_iface_type            *iface_ptr,
  vsncp_3gpp2i_op_pco_type *op_pco_content_ptr

)
{
  ps_iface_op_reserved_pco_type op_pco_info = {0};
  uint8 *mcc_ptr = 0;
  uint8 *mnc_ptr = 0;

  if(NULL == iface_ptr ||
     NULL == op_pco_content_ptr)
  {
    LOG_MSG_ERROR_0("vsncp_set_iface_op_pco_info: invalid input");
    return -1;
  }
  op_pco_info.container_id = op_pco_content_ptr->container_id;
  if(op_pco_content_ptr->app_specific_info_len <= 
     VSNCP_3GPP2I_MAX_OP_PCO_CONTENT_LEN - VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN)
  {
    memscpy(op_pco_info.app_specific_info, 
           255,
           op_pco_content_ptr->app_specific_info,
           op_pco_content_ptr->app_specific_info_len);
    op_pco_info.app_specific_info_len = 
      op_pco_content_ptr->app_specific_info_len;
  }
  LOG_MSG_INFO2_1("VSNCP: iface set op PCO app info, length:%d",
                  op_pco_content_ptr->app_specific_info_len);

  mnc_ptr = op_pco_content_ptr->mnc;
  if(PPP_MNC_LEN-1 == op_pco_content_ptr->mnc_len)
  {
    op_pco_info.mnc_includes_pcs_digit = FALSE;
    op_pco_info.mnc = mnc_ptr[0]*10 + mnc_ptr[1];
  }
  else if(PPP_MNC_LEN == op_pco_content_ptr->mnc_len)
  {
    op_pco_info.mnc_includes_pcs_digit = TRUE;
    op_pco_info.mnc = mnc_ptr[0]*100 + 
                      mnc_ptr[1]*10 +
                      mnc_ptr[2];
  }
  else
  {
    LOG_MSG_ERROR_0("VSNCP: invalid op PCO mnc length");
    return -1;
  }
  LOG_MSG_INFO2_2("VSNCP: iface set op PCO mnc:%d,%d",
                  op_pco_info.mnc, op_pco_info.mnc_includes_pcs_digit);
  mcc_ptr = op_pco_content_ptr->mcc;
  if(PPP_MCC_LEN == op_pco_content_ptr->mcc_len)
  {
    op_pco_info.mcc = mcc_ptr[0]*100 +
                      mcc_ptr[1]*10 +
                      mcc_ptr[2];
  }
  else
  {
    LOG_MSG_ERROR_0("VSNCP: invalid op PCO MCC length");
    return -1;
  }
  LOG_MSG_INFO2_1("VSNCP: iface set op PCO mcc:%d", op_pco_info.mcc);
  if(-1 == ps_iface_set_operator_reserved_pco(iface_ptr,&op_pco_info))
  {
    LOG_MSG_ERROR_0( "ps_iface_set_operator_reserved_pco failed");
    return -1;
  }
  return 0;
}

/*===========================================================================
FUNCTION VSNCP_3GPP2I_OPENING()

DESCRIPTION
  This function is used to complete the VSNCP negotiation, i.e. apply the
  negotiated values to the iface, post events to indicate VSNCP is UP etc.

PARAMETERS
  (IN) fsm_ptr : Pointer to the FSM structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void vsncp_3gpp2i_opening
(
  ppp_fsm_type * fsm_ptr
)
{
  int32 i;
  uint32 listcount = 0;
  int32 mru;
  #ifdef FEATURE_DS_MTU_PCO_VSNCP
  uint16 pco_mtu = 0;
  #endif /* FEATURE_DS_MTU_PCO_VSNCP */
  ppp_dev_enum_type device;
  errno_enum_type ps_errno;
  ps_ip_addr_type prim_dns;
  ps_ip_addr_type sec_dns;
  ps_ip_addr_type  ip_addr;
  ppp_type * ppp_cb_ptr = NULL;
  ppp_type * ppp_cb_array = NULL;
  vsncp_3gpp2_local_opts_val_type *lworkp = NULL;
  vsncp_3gpp2_local_opts_val_type *lwantp = NULL;
  vsncp_3gpp2i_pco_v4_addr * v4_addr_ptr = NULL;
  vsncp_3gpp2i_pco_v6_addr * v6_addr_ptr = NULL;
  vsncp_3gpp2i_pco_v6_addr * last_ptr = NULL;
  vsncp_3gpp2i_pco_v6_addr * dns_ptr = NULL;
  ps_ip_addr_type * sec_dns_ptr = NULL;
  ps_ip_addr_type * ip_addr_array = NULL;
  vsncp_3gpp2i_pco_opts_val_type * pco_vals_ptr = NULL;
  ppp_event_vsncp_payload_type ppp_event_vsncp;
  boolean v6_iid_conflict = FALSE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Validate input parameters
  -------------------------------------------------------------------------*/
  if( fsm_ptr == NULL                 ||
      fsm_ptr->device >= PPP_MAX_DEV  ||
      fsm_ptr->pdv == NULL            ||
      fsm_ptr->local.work_pdv == NULL ||
      fsm_ptr->remote.work_pdv == NULL )
  {
    LOG_MSG_ERROR_0("Invalid fsm_ptr in vsncp_3gpp2i_opening");
    return;
  }

  device = fsm_ptr->device;
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  /*-------------------------------------------------------------------------
    Get the working pointers for protocol-dependant values (pdv) - these
    hold the values of the negotiated options in both directions
    (local, remote)
  -------------------------------------------------------------------------*/
  lworkp = (vsncp_3gpp2_local_opts_val_type *)(fsm_ptr->local.work_pdv);
  lwantp = (vsncp_3gpp2_local_opts_val_type *)(fsm_ptr->local.want_pdv);

  /*-------------------------------------------------------------------------
    Reset the total setup timer now that the NCP is up
  -------------------------------------------------------------------------*/
  (void)ps_timer_cancel(ppp_cb_ptr->setup_timer);

  /*-------------------------------------------------------------------------
    For the following scenarios, stop this NCP and bail!
    (1) If the PDN-Address option didn't get negotiated during VSNCP; OR
    (2) The PDN Address option was negotiated but the IP address obtained
        was empty. Note that we use the local want to see if we are
        interested in IPv4, IPv6 or IPv4_IPv6.
  -------------------------------------------------------------------------*/
  if( ( fsm_ptr->local.work & (uint16)VSNCP_3GPP2I_N_PDN_ADDR ) == 0 ||
      ( ( fsm_ptr->local.work & (uint16)VSNCP_3GPP2I_N_PDN_ADDR ) && 
          ( ( (lwantp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4) && 
             ( lworkp->ip4.ps_s_addr == 0) ) || 
           ( (lwantp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6) && 
             ( lworkp->iid == 0) ) 
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
            || ( (lwantp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6) &&
             ( lworkp->ip4.ps_s_addr == 0) && 
             ( lworkp->iid == 0) ) 
#endif
      ) ) )
  {
    fsm_ptr->fail_reason = PPP_FAIL_REASON_VSNCP_FAILURE;
    LOG_MSG_ERROR_2("pdn-id %d addr type 0x%x unable to get address! Quitting...",
                    lwantp->pdn_id, lwantp->req_addr_type );
    if( ppp_ncp_stop( (ppp_ncp_handle_type)fsm_ptr->pdv ) != 0 )
    {
      LOG_MSG_ERROR_0("Unable to terminate 3gpp2 VSNCP instance");
    }
    return;
  }

  pco_vals_ptr =
    &(((ncpi_cb_type *)fsm_ptr->pdv)->cb_u.vsncp_3gpp2.pco_vals);

  /*-------------------------------------------------------------------------
    Apply the negotiated address/iid and PCO values to the iface, according
    to the req_addr_type originally specified by the client (MH).
    Check for individual bit of ipv4 and ipv6 in req_addr_type to determine
    whether to apply ipv4 options, ipv6 options or both.
    +--------------------------------+------------------+
    | Bits                           |  x x x x x x x   |
    +--------------------------------+------------------+
    | VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 |  0 0 0 0 0 1 1   |
    +--------------------------------+------------------+
    | VSNCP_3GPP2_PDN_TYPE_IPV4      |  0 0 0 0 0 0 1   |
    +--------------------------------+------------------+
    | VSNCP_3GPP2_PDN_TYPE_IPV6      |  0 0 0 0 0 1 0   |
    +--------------------------------+------------------+
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
  /*------------------------------------------------------------------------- 
    If ipv4 bit (bit 0) of req_addr_type is set and we received ipv4 address,
    then apply ipv4 parameters.
  -------------------------------------------------------------------------*/
  if ( ((uint8)lwantp->req_addr_type & (uint8)VSNCP_3GPP2_PDN_TYPE_IPV4) == 
        (uint8)VSNCP_3GPP2_PDN_TYPE_IPV4 &&
        lworkp->ip4.ps_s_addr != 0 ) 
#else
  if ( lwantp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4 &&
        lworkp->ip4.ps_s_addr != 0 ) 
#endif
  {
    /*---------------------------------------------------------------------
      Configure the iface with the obtained IPv4 Address
    ---------------------------------------------------------------------*/
    LOG_MSG_INFO2_0("VSNCP: Got IPv4 addr");
    IPV4_ADDR_MSG( lworkp->ip4.ps_s_addr );
    ip_addr.type = IPV4_ADDR;
    ip_addr.addr.v4.ps_s_addr = lworkp->ip4.ps_s_addr;

    if( PS_IFACE_IS_VALID(fsm_ptr->IPV4_IFACE_PTR) )
    {
      (void)ps_iface_set_v4_addr( fsm_ptr->IPV4_IFACE_PTR, &ip_addr );
      
      /*---------------------------------------------------------------------
        Configure the iface with peer's IPv4 Address (if negotiated)
      ---------------------------------------------------------------------*/
      if( fsm_ptr->local.work & (uint16)VSNCP_3GPP2I_N_DEFAULT_ROUTER )
      {
        LOG_MSG_INFO2_0("VSNCP: Got gateway v4 addr");
        IPV4_ADDR_MSG( lworkp->default_rtr_ip4.ps_s_addr );
        PS_IFACE_SET_V4_GATEWAY_ADDR( fsm_ptr->IPV4_IFACE_PTR, 
                                      lworkp->default_rtr_ip4.ps_s_addr );
      }
  
      /*---------------------------------------------------------------------
        Configure the iface with PCO values if PCO was negotiated
      ---------------------------------------------------------------------*/
      if( fsm_ptr->local.work & (uint16)VSNCP_3GPP2I_N_PCO )
      {
        /*-------------------------------------------------------------------
          Set the DNS addresses in the interface if non-zero
        -------------------------------------------------------------------*/
        if( pco_vals_ptr->dns_v4_primary.ps_s_addr != 0 )
        {
          LOG_MSG_INFO2_0("VSNCP: Got primary DNS v4 addr");
          IPV4_ADDR_MSG( pco_vals_ptr->dns_v4_primary.ps_s_addr );
          LOG_MSG_INFO2_0("VSNCP: Got secondary DNS v4 addr");
          IPV4_ADDR_MSG( pco_vals_ptr->dns_v4_secondary.ps_s_addr );
          PS_IFACE_SET_V4_DNS_SERVER_ADDR
           ( fsm_ptr->IPV4_IFACE_PTR,
              pco_vals_ptr->dns_v4_primary.ps_s_addr,
              pco_vals_ptr->dns_v4_secondary.ps_s_addr
            );
        }
  
        /*-------------------------------------------------------------------
          Set the P-CSCF addresses (if any) in the interface
        -------------------------------------------------------------------*/
        if( lwantp->pco.p_cscf_is_requested )
        {
          /*-----------------------------------------------------------------
            Count how many IPv4 P-CSCF addresses we got
          -----------------------------------------------------------------*/
          v4_addr_ptr = pco_vals_ptr->p_cscf_v4_head_ptr;
          while( v4_addr_ptr != NULL )
          {
            listcount++;
            v4_addr_ptr = v4_addr_ptr->next;
          }
  
          if( listcount > 0 )
          {
            LOG_MSG_INFO2_1("Got %d P-CSCF addresses from HSGW", listcount);
            /*---------------------------------------------------------------
              Allocate memory for an IP address array to hold the addresses
            ---------------------------------------------------------------*/
            ip_addr_array = vsncp_3gpp2i_mem_alloc
                            ( listcount * sizeof( ps_ip_addr_type ) );
            if( ip_addr_array == NULL )
            {
              LOG_MSG_ERROR_0("Mem error: Unable to set v4 P-CSCF addrs");
            }
            else
            {
              /*---------------------------------------------------------------
                Create an array of IP addresses to set in the iface
              ---------------------------------------------------------------*/
              for( ( i = listcount - 1, \
                     v4_addr_ptr = pco_vals_ptr->p_cscf_v4_head_ptr );
                   ( ( i >= 0 ) && ( NULL != v4_addr_ptr ) );
                   ( i--, v4_addr_ptr = v4_addr_ptr->next ) )
              {
                ip_addr_array[i].type = IPV4_ADDR;
                ip_addr_array[i].addr.v4.ps_s_addr = v4_addr_ptr->addr;
                LOG_MSG_INFO2_0("VSNCP: Got P-CSCF v4 addr");
                IPV4_ADDR_MSG( ip_addr_array[i].addr.v4.ps_s_addr );
              }
  
              /*---------------------------------------------------------------
                Configure the iface with IPv4 P-CSCF addresses
              ---------------------------------------------------------------*/
              if ( -1 == ps_iface_set_sip_serv_addr( fsm_ptr->IPV4_IFACE_PTR,
                                                     ip_addr_array,
                                                     listcount,
                                                     &ps_errno ) )
              {
                LOG_MSG_ERROR_1( "ps_iface_set_sip_serv_addr failed with errno %d",
                                 ps_errno);
              }
  
              /*---------------------------------------------------------------
                Free memory allocated for the IP Address array
              ---------------------------------------------------------------*/
              vsncp_3gpp2i_mem_free( ip_addr_array );
              ip_addr_array = NULL;
            } /* ip_addr_array */
          } /* listcount */
        } /* p_cscf was requested */

#ifdef FEATURE_DS_MTU_PCO_VSNCP
         /*-------------------------------------------------------------------
          Set the mtu (if any) in the interface
        -------------------------------------------------------------------*/
        if ( pco_vals_ptr->mtu != 0 ) 
        {
          PS_IFACE_SET_MTU(fsm_ptr->IPV4_IFACE_PTR, pco_vals_ptr->mtu);
          pco_mtu = pco_vals_ptr->mtu;
          LOG_MSG_INFO2_1("VSNCP: set mtu to iface in IPv4: %d", pco_mtu);
        }                              
#endif /* FEATURE_DS_MTU_PCO_VSNCP */

        /*---------------------------------------------------------------
          Save the operator reserved PCO content to iface. No need to
          validate if requested for the PCO, need to support unsolicitated
          operator reserved PCO
        ---------------------------------------------------------------*/
        if( pco_vals_ptr->op_pco_content.container_id >= PPP_OP_PCO_ID_START )
        {
          if(-1 == vsncp_set_iface_op_pco_info(fsm_ptr->IPV4_IFACE_PTR,
                                      &pco_vals_ptr->op_pco_content))
          {
            LOG_MSG_ERROR_0( "vsncp_set_iface_op_pco_info failed!");
          }
        }
      } /* pco was negotiated */
    }/*v4 iface is VALID*/
    else
    {
      LOG_MSG_INFO2_0("VSNCP: v4 iface ptr NULL, could not set v4 addr");
    }
  } /* ipv4 parameters applied */

#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
  /*------------------------------------------------------------------------- 
    If ipv6 bit (bit 1) of req_addr_type is set & we obtained ipv6 iid,
    then apply ipv6 parameters.
  -------------------------------------------------------------------------*/
  if ( ((uint8)lwantp->req_addr_type & (uint8)VSNCP_3GPP2_PDN_TYPE_IPV6) == 
        (uint8)VSNCP_3GPP2_PDN_TYPE_IPV6 &&
        lworkp->iid != 0 )
#else
  else if ( lwantp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6 &&
       lworkp->iid != 0 )
#endif
  {
    /*---------------------------------------------------------------------
      Configure the iface with the obtained IPv6 IID
    ---------------------------------------------------------------------*/
    LOG_MSG_INFO2_1( "VSNCP: Got interface ID = 0x%x", lworkp->iid);
    if( PS_IFACE_IS_VALID(fsm_ptr->IPV6_IFACE_PTR) )
    {
      if ( FALSE == PS_IFACE_SET_IP_V6_IID(fsm_ptr->IPV6_IFACE_PTR, lworkp->iid) )
      {
        v6_iid_conflict = TRUE;
  
        /*---------------------------------------------------------------------
          If IID conflicts then stop VSNCP on conditions:
          1) If only v6 was requested or
          2) if v4_v6 was requested, but network only assgined v6.
        ---------------------------------------------------------------------*/
        if ( (lwantp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6) ||
             (lwantp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 && 
              lworkp->ip4.ps_s_addr == 0) )
        {
          LOG_MSG_ERROR_1("IPv6 IID conflicts for pdn-id %d! Quitting...",
                          lwantp->pdn_id );
          fsm_ptr->fail_reason = PPP_FAIL_REASON_VSNCP_FAILURE;
          if( ppp_ncp_stop( (ppp_ncp_handle_type)fsm_ptr->pdv ) != 0 )
          {
            LOG_MSG_ERROR_0("Unable to terminate 3gpp2 VSNCP instance");
          }
          return;
        }
      }    
      
      ppp_report_event_iid( fsm_ptr->device, lworkp->iid );
      IPV6_IID_MSG (lworkp->iid);
      /*---------------------------------------------------------------------
        Configure the iface with PCO values if PCO was negotiated
      ---------------------------------------------------------------------*/
      if( fsm_ptr->local.work & (uint16)VSNCP_3GPP2I_N_PCO )
      {
        /*-------------------------------------------------------------------
          Set the DNS addresses in the interface
        -------------------------------------------------------------------*/
        memset( &prim_dns.addr.v6, 0, sizeof( struct ps_in6_addr ) );
        memset( &sec_dns.addr.v6, 0, sizeof( struct ps_in6_addr ) );
  
        /*-------------------------------------------------------------------
          For IPv6 DNS, we only use the first two that arrived from the
          network (so the *last* one in the IPv6 DNS list is the primary,
          and the last-but-one is the secondary.
        -------------------------------------------------------------------*/
        dns_ptr = pco_vals_ptr->dns_v6_head_ptr;
        if( dns_ptr != NULL )
        {
          /*-----------------------------------------------------------------
            Navigate to the end of the list, keeping track of prev
          -----------------------------------------------------------------*/
          while ( dns_ptr->next != NULL )
          {
            last_ptr = dns_ptr;
            dns_ptr = dns_ptr->next;
          }
  
          prim_dns.type = IPV6_ADDR;
          prim_dns.addr.v6 = dns_ptr->addr;
  
          if ( last_ptr != NULL )
          {
            sec_dns.type    = IPV6_ADDR;
            sec_dns.addr.v6 = last_ptr->addr;
            sec_dns_ptr     = &sec_dns;
          }
        }
  
        /*-------------------------------------------------------------------
          ps_iface_set_v6_dns_addrs allows sec_dns_ptr to be NULL, but not
          prim_dns
        -------------------------------------------------------------------*/
        if( !PS_IN6_IS_ADDR_UNSPECIFIED( &(prim_dns.addr.v6) ) )
        {
          LOG_MSG_INFO2_0("VSNCP: Got primary DNS v6 addr");
          IPV6_ADDR_MSG( prim_dns.addr.v6.ps_s6_addr64 );
          LOG_MSG_INFO2_0("VSNCP: Got secondary DNS v6 addr");
          IPV6_ADDR_MSG( sec_dns.addr.v6.ps_s6_addr64 );
          if ( ps_iface_set_v6_dns_addrs( fsm_ptr->IPV6_IFACE_PTR, 
                                          &prim_dns,
                                          sec_dns_ptr ) == -1 )
          {
            LOG_MSG_ERROR_0("ps_iface_set_v6_dns_addrs failed");
          }
        }
  
        /*-------------------------------------------------------------------
          Set the P-CSCF addresses (if any) in the interface
        -------------------------------------------------------------------*/
        if( lwantp->pco.p_cscf_is_requested )
        {
          /*-----------------------------------------------------------------
            Count how many IPv6 P-CSCF addresses we got
          -----------------------------------------------------------------*/
          v6_addr_ptr = pco_vals_ptr->p_cscf_v6_head_ptr;
          listcount = 0;
          while( v6_addr_ptr != NULL )
          {
            listcount++;
            v6_addr_ptr = v6_addr_ptr->next;
          }
  
          if( listcount > 0 )
          {
            LOG_MSG_INFO2_1("Got %d P-CSCF addresses from HSGW", listcount);
            /*---------------------------------------------------------------
              Allocate memory for an IP address array to hold the addresses
            ---------------------------------------------------------------*/
            ip_addr_array = vsncp_3gpp2i_mem_alloc
                            ( listcount * sizeof( ps_ip_addr_type ) );
            if( ip_addr_array == NULL )
            {
              LOG_MSG_ERROR_0("Mem error: Unable to set v6 P-CSCF addrs");
            }
            else
            {
              /*---------------------------------------------------------------
                Create an array of IP addresses to set in the iface
              ---------------------------------------------------------------*/
              for( ( i = listcount - 1, \
                     v6_addr_ptr = pco_vals_ptr->p_cscf_v6_head_ptr );
                   ( ( i >= 0 ) && ( NULL != v6_addr_ptr ) );
                   ( i--, v6_addr_ptr = v6_addr_ptr->next ) )
              {
                ip_addr_array[i].type = IPV6_ADDR;
                ip_addr_array[i].addr.v6 = v6_addr_ptr->addr;
                LOG_MSG_INFO2_0("VSNCP: Got P-CSCF v6 addr");
                IPV6_ADDR_MSG( ip_addr_array[i].addr.v6.ps_s6_addr64 );
              }
  
              /*---------------------------------------------------------------
                Configure the iface with IPv6 P-CSCF addresses
              ---------------------------------------------------------------*/
              if ( -1 == ps_iface_set_sip_serv_addr( fsm_ptr->IPV6_IFACE_PTR,
                                                     ip_addr_array,
                                                     listcount,
                                                     &ps_errno ) )
              {
                LOG_MSG_ERROR_1("ps_iface_set_sip_serv_addr failed with errno %d",
                                ps_errno);
              }
  
              /*---------------------------------------------------------------
                Free memory allocated for the IP Address array
              ---------------------------------------------------------------*/
              vsncp_3gpp2i_mem_free( ip_addr_array );
              ip_addr_array = NULL;
            } /* ip_addr_array */
          } /* listcount */
        } /* p_cscf */

#ifdef FEATURE_DS_MTU_PCO_VSNCP
         /*-------------------------------------------------------------------
          Set the mtu (if any) in the interface
        -------------------------------------------------------------------*/
	    if ( pco_vals_ptr->mtu != 0 ) 
	    {
	      PS_IFACE_SET_MTU(fsm_ptr->IPV6_IFACE_PTR, pco_vals_ptr->mtu);
			       pco_mtu = pco_vals_ptr->mtu;
	      LOG_MSG_INFO2_1("VSNCP: set mtu to iface in IPv6: %d", pco_mtu);
	    }
#endif /* FEATURE_DS_MTU_PCO_VSNCP */

        /*---------------------------------------------------------------
          Save the operator reserved PCO content to iface. No need to
          validate if requested for the PCO, need to support unsolicitated
          operator reserved PCO
        ---------------------------------------------------------------*/
        if( pco_vals_ptr->op_pco_content.container_id >= PPP_OP_PCO_ID_START )
        {
          if(-1 == vsncp_set_iface_op_pco_info(fsm_ptr->IPV6_IFACE_PTR,
                                      &pco_vals_ptr->op_pco_content))
          {
            LOG_MSG_ERROR_0( "vsncp_set_iface_op_pco_info v6 iface failed!");
          }
        }
      } /* pco */
    }/* v6 iface is valid*/    
    else
    {
      LOG_MSG_INFO2_0("VSNCP: v6 iface ptr NULL");
    }

  } /* ipv6 parameters applied */
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
  if ( lwantp->req_addr_type <= VSNCP_3GPP2_PDN_TYPE_MIN || 
       lwantp->req_addr_type >= VSNCP_3GPP2_PDN_TYPE_MAX )  
#else
  else 
#endif
  {
    LOG_MSG_ERROR_1("Unexpected req_addr_type 0x%x", lwantp->req_addr_type);
    ASSERT(0);
  }


  /*-------------------------------------------------------------------------
    Free heap memory PCOs (v4/v6 P-CSCF addrs and v6 DNS addrs) - the addrs
    have been set in the iface appropriately
  -------------------------------------------------------------------------*/
  vsncp_3gpp2i_pco_mem_free( (ncpi_cb_type *)fsm_ptr->pdv );

  /*-------------------------------------------------------------------------
    Set the MTU: If negotiated, overwrite the previously configured MTU with
                 the network negotiated value
  -------------------------------------------------------------------------*/
  mru = lcp_mru_get( fsm_ptr->device );
  switch( lwantp->req_addr_type )
  {
    case VSNCP_3GPP2_PDN_TYPE_IPV4:
      /*---------------------------------------------------------------------
        For IPv4 PDN, reduce the negotiated value by 1 to allow for VSNP
      ---------------------------------------------------------------------*/
      if( mru != -1 )
      {
        if( PS_IFACE_IS_VALID(fsm_ptr->IPV4_IFACE_PTR) )
        {
#ifdef FEATURE_DS_MTU_PCO_VSNCP
          if ( pco_mtu !=0 ) 
          {
            PS_IFACE_SET_MTU( fsm_ptr->IPV4_IFACE_PTR, (MIN((uint16)(mru - 1),pco_mtu)) );
          }
          else
          {
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
            PS_IFACE_SET_MTU( fsm_ptr->IPV4_IFACE_PTR, (uint16)(mru - 1) );
#ifdef FEATURE_DS_MTU_PCO_VSNCP
          }
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
          LOG_MSG_INFO1_1("VSNCP: v4 PDN type mtu: %d",
                          PS_IFACE_GET_MTU(fsm_ptr->IPV4_IFACE_PTR));
        }
        else
        {
          LOG_MSG_INFO2_0("VSNCP: v4 iface ptr NULL");
        }
      }
      break;

    case VSNCP_3GPP2_PDN_TYPE_IPV6:
      if ( mru != -1 )
      {
        /* Set the MTU value as it is received from the network */
        if( PS_IFACE_IS_VALID(fsm_ptr->IPV6_IFACE_PTR) )
        {
#ifdef FEATURE_DS_MTU_PCO_VSNCP
          if ( pco_mtu !=0 ) 
          {
            PS_IFACE_SET_MTU( fsm_ptr->IPV6_IFACE_PTR, (MIN((uint16)(mru-1),pco_mtu)) );
          }
          else
          {
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
            PS_IFACE_SET_MTU( fsm_ptr->IPV6_IFACE_PTR, (uint16)(mru-1) );
#ifdef FEATURE_DS_MTU_PCO_VSNCP
          }
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
          LOG_MSG_INFO1_1("VSNCP: v6 PDN type  mtu: %d",
                          PS_IFACE_GET_MTU(fsm_ptr->IPV6_IFACE_PTR));
        }
        else
        {
          LOG_MSG_INFO2_0("VSNCP: v6 iface ptr NULL");
        }
      }
      break;

    case VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6:
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
      if( mru != -1 )
      {
        if( PS_IFACE_IS_VALID(fsm_ptr->IPV4_IFACE_PTR) )
        {
#ifdef FEATURE_DS_MTU_PCO_VSNCP
          if ( pco_mtu !=0 ) 
          {
          /* For IPv4, reduce the negotiated value by 1 to allow for VSNP */
            PS_IFACE_SET_MTU( fsm_ptr->IPV4_IFACE_PTR, (MIN((uint16)(mru - 1),pco_mtu)) );		
          }
	  else
	  {
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
          /* For IPv4, reduce the negotiated value by 1 to allow for VSNP */
            PS_IFACE_SET_MTU( fsm_ptr->IPV4_IFACE_PTR, (uint16)(mru - 1) );
#ifdef FEATURE_DS_MTU_PCO_VSNCP
          }
#endif/* FEATURE_DS_MTU_PCO_VSNCP */		  
          LOG_MSG_INFO1_1("VSNCP: v4_v6 PDN type  v4 MTU: %d",
                          PS_IFACE_GET_MTU(fsm_ptr->IPV4_IFACE_PTR));
        }
        else
        {
          LOG_MSG_INFO2_0("VSNCP: v4 iface ptr NULL");
        }
        
        if( PS_IFACE_IS_VALID(fsm_ptr->IPV6_IFACE_PTR) )
        {
#ifdef FEATURE_DS_MTU_PCO_VSNCP
          if ( pco_mtu !=0 ) 
          {
            PS_IFACE_SET_MTU( fsm_ptr->IPV6_IFACE_PTR, (MIN((uint16)(mru-1),pco_mtu)) );
          }
          else
          {
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
          /* For IPv6, set the MTU value as it is received from the network */
            PS_IFACE_SET_MTU( fsm_ptr->IPV6_IFACE_PTR, (uint16)(mru-1) );
 #ifdef FEATURE_DS_MTU_PCO_VSNCP
          }
#endif/* FEATURE_DS_MTU_PCO_VSNCP */
          LOG_MSG_INFO1_1("VSNCP: v4_v6 PDN type  v6 MTU: %d",
                          PS_IFACE_GET_MTU(fsm_ptr->IPV6_IFACE_PTR));
        }
        else
        {
          LOG_MSG_INFO2_0("VSNCP: v6 iface ptr NULL");
        }
      }
       
      break;
#endif

    default:
      /* unexpected! */
      LOG_MSG_ERROR_1("Unexpected req_addr_type 0x%x", lwantp->req_addr_type);
      ASSERT(0);
      break;
  } /* req_addr_type */

  /*-------------------------------------------------------------------------
    VSNCP is now up, set attach_type to HANDOVER in case a resync occurs
  -------------------------------------------------------------------------*/
  lworkp->attach_type = VSNCP_3GPP2_ATTACH_TYPE_HANDOVER;

  LOG_MSG_INFO1_2("3gpp2-VSNCP pdn id 0x%x established on dev %d",
                  lwantp->pdn_id, device );

  /*-------------------------------------------------------------------------
    Post events to clients and to DIAG indicating VSNCP is now UP.
    In case of subscription limited scenario clean up the other iface.
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
  if( ( lwantp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 ) &&
      ( ( lworkp->iid == 0 ) || ( TRUE == v6_iid_conflict ) ) )
  {
    pppi_call_event_callback( fsm_ptr, PPP_PROTOCOL_VSNCP, 
                              PPP_SUBS_LIMITED_TO_V4_EV); 
    fsm_ptr->IPV6_IFACE_PTR = NULL; 
  }
  else if( ( lwantp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 ) &&
      ( lworkp->ip4.ps_s_addr == 0 ) )
  {
    pppi_call_event_callback( fsm_ptr, PPP_PROTOCOL_VSNCP, 
                              PPP_SUBS_LIMITED_TO_V6_EV);
    fsm_ptr->IPV4_IFACE_PTR = NULL;   
  }
  else 
  {
    pppi_call_event_callback( fsm_ptr, PPP_PROTOCOL_VSNCP, PPP_UP_EV );
  }
#else
  pppi_call_event_callback( fsm_ptr, PPP_PROTOCOL_VSNCP, PPP_UP_EV );
#endif
  
  pppi_report_event_to_diag( EVENT_PPP,
                             device,
                             PPP_PROTOCOL_VSNCP,
                             PS_PPP_EVENT_UNKNOWN,
                             PS_PPP_EVENT_OPEN );

  /*-------------------------------------------------------------------
    Diag event indicating that VSNCP bring up succeeded.
  ---------------------------------------------------------------------*/
  ppp_event_vsncp.ppp_event_device     = (uint8)device;
  ppp_event_vsncp.pdn_id               = lwantp->pdn_id;
  ppp_event_vsncp.pdn_type             = lwantp->req_addr_type;
  ppp_event_vsncp.ppp_event_state      = PS_PPP_EVENT_OPEN;
  event_report_payload( EVENT_VSNCP,
                        sizeof(ppp_event_vsncp),
                        &ppp_event_vsncp );
} /* vsncp_3gpp2i_opening() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_CLOSING()

DESCRIPTION
  This function will initiate the closing of the 3GPP2-VSNCP instance.

PARAMETERS
  (IN) fsm_ptr : Pointer to the FSM structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void vsncp_3gpp2i_closing
(
  ppp_fsm_type * fsm_ptr
)
{
  LOG_MSG_INFO2_0("vsncp_3gpp2i_closing()");
  /* NOOP */
  return;
} /* vsncp_3gpp2i_closing() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_STOPPING()

DESCRIPTION
  This function will perform cleanup for the 3GPP2-VSNCP instance

PARAMETERS
  (IN) fsm_ptr : Pointer to the FSM structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void vsncp_3gpp2i_stopping
(
  ppp_fsm_type *fsm_ptr
)
{
  ppp_event_vsncp_payload_type ppp_event_vsncp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_0("vsncp_3gpp2i_stopping()");

  /*-------------------------------------------------------------------------
    Validate arguments; fsm_ptr should be non-NULL and so should
    fsm_ptr->pdv (it's the NCP control block ptr).
  -------------------------------------------------------------------------*/
  if( ( fsm_ptr == NULL ) ||
      ( fsm_ptr->device >= PPP_MAX_DEV ) ||
      ( fsm_ptr->pdv == NULL ) )
  {
    LOG_MSG_ERROR_0("Invalid fsm_ptr in ehrpd_vsncp_stopping");
    return;
  }

  ppp_event_vsncp.pdn_id    = (((ncpi_cb_type*)fsm_ptr->pdv)->cb_u.
                               vsncp_3gpp2.config).option_vals.pdn_id;
  ppp_event_vsncp.pdn_type  = (((ncpi_cb_type*)fsm_ptr->pdv)->cb_u.
                               vsncp_3gpp2.config).option_vals.req_addr_type;
  ppp_event_vsncp.ppp_event_device = (uint8)fsm_ptr->device;
  ppp_event_vsncp.ppp_event_state  = PS_PPP_EVENT_CLOSING;
  event_report_payload( EVENT_VSNCP,
                        sizeof(ppp_event_vsncp),
                        &ppp_event_vsncp );

  ppp_ncp_abort( (ppp_ncp_handle_type) fsm_ptr->pdv );
} /* vsncp_3gpp2i_stopping() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_ENCODE_MCCMNC()

DESCRIPTION
  Encode mccmnc according to section 10.5.1.3 in 3GPP TS 24.008
    |--8-7-6-5--|--4-3-2-1--|
    |MCC Digit2 | MCC Digit1|
    |MNC Digit3 | MCC Digit3|
    |MNC Digit2 | MNC Digit1|
    The MNC shall consist of 2 or 3 digits,a network operator may decide to use
    only two digits in the MNC, in this case, MNC Digit3 shall be coded as "1111".  

PARAMETERS
  (IN) op_pco_ptr  : Pointer to operator reserved PCO information
  (OUT) mccmnc_ptr : Pointer to the encoded mccmnc.
  (IN) mccmnc_len  : Pointer to the length of the buffer for mccmnc

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Success
  FALSE: Fail

SIDE EFFECTS
  None
===========================================================================*/

static boolean vsncp_3gpp2i_encode_mccmnc(
                                          vsncp_3gpp2_op_pco_type *op_pco_ptr,
                                          uint8 *mccmnc_ptr,
                                          uint8 mccmnc_len
                                          )
{
  if( NULL == op_pco_ptr ||
      NULL == mccmnc_ptr ||
      VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN != mccmnc_len )
  {
    LOG_MSG_ERROR_3("vsncp_3gpp2i_encode_mccmnc, Invalid input: %x, %x, %d",
                    op_pco_ptr, mccmnc_ptr, mccmnc_len );
    return FALSE;
  }
  if( op_pco_ptr->mcc_len != PPP_MCC_LEN ||
               (op_pco_ptr->mnc_len != PPP_MNC_LEN && 
                op_pco_ptr->mnc_len != PPP_MNC_LEN - 1)
      )
  {
    LOG_MSG_ERROR_2("vsncp_3gpp2i_encode_mccmnc, Invalid mcc/mnc length: %d, %d",
                    op_pco_ptr->mcc_len, op_pco_ptr->mnc_len );
    return FALSE;
  }
  if(op_pco_ptr->mnc_len == PPP_MCC_LEN-1)
  {
    op_pco_ptr->mnc[2] = 0x0F;
  }
  mccmnc_ptr[0] = 
               op_pco_ptr->mcc[0] | (op_pco_ptr->mcc[1] << 4);
  mccmnc_ptr[1] = 
               op_pco_ptr->mcc[2] | (op_pco_ptr->mnc[2] << 4);
  mccmnc_ptr[2] = 
               op_pco_ptr->mnc[0] | (op_pco_ptr->mnc[1] << 4);

  return TRUE;

}
/*===========================================================================
FUNCTION VSNCP_3GPP2I_OPTION()

DESCRIPTION
  This function appends in the passed option into the passed DSM item

PARAMETERS
  (IN) fsm_ptr     : Pointer to the FSM structure
  (IN) vp          : Pointer to structure containing option values.
  (IN) pkt_ptr_ptr : Pointer to pointer to the DSM item
  (IN) opt_num     : Option to be added

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void vsncp_3gpp2i_option
(
  ppp_fsm_type   * fsm_ptr,
  void           * vp,
  dsm_item_type ** pkt_ptr_ptr,
  uint8            opt_num
)
{
  uint8 buf[256];
  uint16 offset = 0;
  uint16 opthdr_offset = 0;
  uint16 pco_cfg_proto;
  struct config_hdr config_hdr;
  ppp_opt_hdr_type opthdr;
  ppp_opt_hdr_type ipcp_opthdr;
  struct ps_in_addr addr;
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
  vsncp_3gpp2_pdn_type req_addr_type;
#endif /* FEATURE_DATA_EHRPD_DUAL_IP */
  uint16 identifier;
  uint8 length;
  uint8 pdn_type_empty;
  vsncp_3gpp2_local_opts_val_type *value_p =
    (vsncp_3gpp2_local_opts_val_type *)vp;
  vsncp_3gpp2_local_opts_val_type * lworkp = NULL;
#ifdef FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH  
  vsncp_3gpp2i_pco_opts_val_type * pco_vals_ptr = NULL;
  uint16 auth_pkt_size;
  uint8  auth_user_id_len;
  uint8  auth_password_len;
  uint8         auth_chap_value_size = 0;
  unsigned char auth_chap_chal_value[PPP_CHAP_CHAL_LEN];
  unsigned char auth_chap_resp_value[PPP_CHAP_MD5_SIZE];
#endif /* FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH */
  uint8 mccmnc[VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN] = {0};
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( vp == NULL )
  {
    DATA_ERR_FATAL("Invalid vp pointer in vsncp_3gpp2i_option");
  }

  opthdr.type = opt_num;
  memset( buf, 0, 256 );
  switch( opt_num )
  {
    case VSNCP_3GPP2I_PDN_ID:
      opthdr.length = OPTION_HDR_LEN + sizeof( uint8 );
      COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
      COPY_BYTES( buf, &value_p->pdn_id, sizeof( uint8), offset );
      break;

    case VSNCP_3GPP2I_APN:
      opthdr.length = OPTION_HDR_LEN + value_p->apn_len;
      COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
      if( value_p->apn_len > 0 )
      {
        COPY_BYTES( buf, &value_p->apn[0], value_p->apn_len, offset );
      }
      break;

    case VSNCP_3GPP2I_PDN_TYPE:
      opthdr.length = OPTION_HDR_LEN + sizeof( uint8 );
      COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
      COPY_BYTES( buf, &value_p->ip_ver_capability, sizeof( uint8), offset );
      break;

    case VSNCP_3GPP2I_PDN_ADDR:
      /*---------------------------------------------------------------------
        As per X.P0057 if the attach type is Initial Attach, the PDN type
        field of PDN address option should have the PDN type as 000.
      ---------------------------------------------------------------------*/
      if( value_p->attach_type == VSNCP_3GPP2_ATTACH_TYPE_INITIAL )
      {
        opthdr.length = OPTION_HDR_LEN + sizeof( uint8 );
        pdn_type_empty = (uint8)VSNCP_3GPP2_PDN_TYPE_EMPTY;
        COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
        COPY_BYTES( buf, &pdn_type_empty, sizeof( uint8), offset );
        break;
      }

      /*---------------------------------------------------------------------
        For Handover Attach verify that we have proper IP addresses.
      ---------------------------------------------------------------------*/
      if ( ( value_p->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4 && 
             value_p->ip4.ps_s_addr == 0 ) ||
           ( value_p->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6 && 
             value_p->iid == 0 ) 
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
        || ( value_p->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 && 
             value_p->ip4.ps_s_addr == 0 &&
             value_p->iid == 0 ) 
#endif
         )
      {
        DATA_ERR_FATAL("Unexpected: no IP address found.");
      }

      switch( value_p->req_addr_type )
      {
        case VSNCP_3GPP2_PDN_TYPE_IPV4:
          opthdr.length  =
            OPTION_HDR_LEN + sizeof( uint8 ) + sizeof(struct ps_in_addr);
          COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
          COPY_BYTES( buf, &value_p->req_addr_type, sizeof( uint8), offset );
          COPY_BYTES( buf,
                      &value_p->ip4.ps_s_addr,
                      sizeof( struct ps_in_addr ),
                      offset );
          break;

        case VSNCP_3GPP2_PDN_TYPE_IPV6:
          opthdr.length  =
            OPTION_HDR_LEN + sizeof( uint8 ) + sizeof ( uint64 );
          COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
          COPY_BYTES( buf, &value_p->req_addr_type, sizeof( uint8), offset );
          COPY_BYTES( buf, &value_p->iid, sizeof( uint64 ), offset );
          break;

        case VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6:
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
          if( value_p->iid == 0 )
          {
            opthdr.length  = 
            OPTION_HDR_LEN + sizeof( uint8 ) + sizeof(struct ps_in_addr);
            COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
            req_addr_type = VSNCP_3GPP2_PDN_TYPE_IPV4;
            COPY_BYTES( buf, &req_addr_type, sizeof( uint8), offset );
            COPY_BYTES( buf, 
                        &value_p->ip4.ps_s_addr, 
                        sizeof( struct ps_in_addr ), 
                        offset );
          }
          else if ( value_p->ip4.ps_s_addr == 0 )
          {
            opthdr.length  = 
            OPTION_HDR_LEN + sizeof( uint8 ) + sizeof ( uint64 );
            COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
            req_addr_type = VSNCP_3GPP2_PDN_TYPE_IPV6;
            COPY_BYTES( buf, &req_addr_type, sizeof( uint8), offset );
            COPY_BYTES( buf, &value_p->iid, sizeof( uint64 ), offset );
          }
          else 
          {
            opthdr.length  = 
            OPTION_HDR_LEN + sizeof( uint8 ) + sizeof ( uint64 ) + 
            sizeof(struct ps_in_addr);
            COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
            COPY_BYTES( buf, &value_p->req_addr_type, sizeof( uint8), offset );
            COPY_BYTES( buf, &value_p->iid, sizeof( uint64 ), offset );
            COPY_BYTES( buf, 
                        &value_p->ip4.ps_s_addr, 
                        sizeof( struct ps_in_addr ), 
                        offset );
          } 
          break;
#endif
        default:
          /* shouldn't reach here! client options are checked earlier! */
          LOG_MSG_ERROR_1("Unexpected req_addr_type 0x%x",
                          value_p->req_addr_type);
          ASSERT(0);
          break;
      }/* End of switch */
      break;

    case VSNCP_3GPP2I_PCO:
      /*---------------------------------------------------------------------
        Start copying at OPTION_HDR_LEN, will copy opthdr later since
        we need to figure opthdr.length
      ---------------------------------------------------------------------*/
      offset += OPTION_HDR_LEN;
      pco_cfg_proto = VSNCP_3GPP2I_PCO_CONFIG_PROTOCOL;
      COPY_BYTES( buf, &pco_cfg_proto, sizeof( uint8 ), offset );
      lworkp = (vsncp_3gpp2_local_opts_val_type *)(fsm_ptr->local.work_pdv);

#ifdef FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH
      /*---------------------------------------------------------------------
        Perform PDN Level Authentication if specified
      ---------------------------------------------------------------------*/
      if( lworkp->pco.pdn_level_auth.protocol == PPP_AUTH_PAP )
      {
        /*---------------------------------------------------------------
          PDN Level Authentication using PAP
         
          Configuration protocol option information element created
          containing a PAP Auth-Request packet
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |       Config protocol option header (PAP)     |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          | Code(1-request)|  Identifier   |            Length          |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          | User-ID Length |  User-Id ...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          | Passwd-Length  |  Password ...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        ----------------------------------------------------------------*/
        LOG_MSG_INFO2_0("PDN Level Authentication requested using PAP");

        /* Config protocol option header */
        identifier = ps_htons( PPP_PAP_PROTOCOL );
        auth_pkt_size = PPP_AUTH_CODE_SIZE
                      + PPP_AUTH_ID_SIZE
                      + PPP_AUTH_LENGTH_SIZE
                      + PPP_PAP_UID_SIZE
                      + lworkp->pco.pdn_level_auth.user_id_len
                      + PPP_PAP_PWD_SIZE
                      + lworkp->pco.pdn_level_auth.password_len;
        COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
        COPY_BYTES( buf, &auth_pkt_size, sizeof( uint8 ), offset );

        pco_vals_ptr =
          &(((ncpi_cb_type *)fsm_ptr->pdv)->cb_u.vsncp_3gpp2.pco_vals);

        /* PAP Auth-Request Packet Header */
        memset( &config_hdr, 0, CONFIG_HDR_LEN );
        config_hdr.code = PPP_PAP_REQ;
        config_hdr.identifier = pco_vals_ptr->pdn_level_auth_active_id++;
        config_hdr.length = ps_htons( auth_pkt_size );
        COPY_BYTES( buf, &config_hdr, CONFIG_HDR_LEN, offset );

        /* PAP Auth-Request Packet Contents */
        auth_user_id_len = lworkp->pco.pdn_level_auth.user_id_len;
        COPY_BYTES( buf, &auth_user_id_len, sizeof( uint8 ), offset );
        COPY_BYTES( buf, 
                    lworkp->pco.pdn_level_auth.user_id, 
                    auth_user_id_len,
                    offset );

        auth_password_len = lworkp->pco.pdn_level_auth.password_len;
        COPY_BYTES( buf, &auth_password_len, sizeof( uint8 ), offset );
        if( auth_password_len != 0 )
        {
        COPY_BYTES( buf,
                    lworkp->pco.pdn_level_auth.password,
                    auth_password_len,
                    offset );
        }
      } /* end PDN level auth with PAP */

      if( lworkp->pco.pdn_level_auth.protocol == PPP_AUTH_CHAP )
      {
        /*---------------------------------------------------------------
          PDN Level Authentication using CHAP
         
          Two Configuration protocol option information elements created:
           1. CHAP Challenge Packet
           2. CHAP Response Packet
         
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |       Config protocol option header (CHAP)    |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |  Code(1-chal) |  Identifier   |            Length           |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |  Chal Length  |  Challenge ...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |  Username ...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |       Config protocol option header (CHAP)    |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |  Code(2-resp) |  Identifier   |            Length           |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |  Hash Length  |  Hash value ...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |  Username ...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               
        ----------------------------------------------------------------*/
        LOG_MSG_INFO2_0("PDN Level Authentication requested using CHAP");

        /*--------------------------------------------------------------- 
          Create CHAP Challenge Packet
        ----------------------------------------------------------------*/
        /* Config protocol option header */
        identifier = ps_htons( PPP_CHAP_PROTOCOL );
        auth_pkt_size = PPP_AUTH_CODE_SIZE
                      + PPP_AUTH_ID_SIZE
                      + PPP_AUTH_LENGTH_SIZE
                      + PPP_CHAP_VALUE_SIZE
                      + PPP_CHAP_CHAL_LEN
                      + lworkp->pco.pdn_level_auth.user_id_len;
        COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
        COPY_BYTES( buf, &auth_pkt_size, sizeof( uint8 ), offset );

        pco_vals_ptr =
          &(((ncpi_cb_type *)fsm_ptr->pdv)->cb_u.vsncp_3gpp2.pco_vals);

        /* CHAP Challenge Packet Header */
        memset( &config_hdr, 0, CONFIG_HDR_LEN );
        config_hdr.code = PPP_CHAP_CHALLENGE;
        config_hdr.identifier = pco_vals_ptr->pdn_level_auth_active_id;
        config_hdr.length = ps_htons(auth_pkt_size);
        COPY_BYTES( buf, &config_hdr, CONFIG_HDR_LEN, offset );

        /* CHAP Challenge Packet Contents */
        auth_chap_value_size = PPP_CHAP_CHAL_LEN;
        COPY_BYTES( buf, &auth_chap_value_size, sizeof( uint8 ), offset );

        ppp_auth_get_rand_chap_chal_value(auth_chap_chal_value,
                                          auth_chap_value_size);
        COPY_BYTES( buf,
                    auth_chap_chal_value,
                    auth_chap_value_size,
                    offset );
        COPY_BYTES( buf,
                    lworkp->pco.pdn_level_auth.user_id,
                    lworkp->pco.pdn_level_auth.user_id_len,
                    offset );

        /*--------------------------------------------------------------- 
          Create CHAP Response Packet
        ----------------------------------------------------------------*/
        /* Config protocol option header */
        identifier = ps_htons( PPP_CHAP_PROTOCOL );
        auth_pkt_size = PPP_AUTH_CODE_SIZE
                      + PPP_AUTH_ID_SIZE
                      + PPP_AUTH_LENGTH_SIZE
                      + PPP_CHAP_VALUE_SIZE
                      + PPP_CHAP_MD5_SIZE
                      + lworkp->pco.pdn_level_auth.user_id_len;
        COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
        COPY_BYTES( buf, &auth_pkt_size, sizeof( uint8 ), offset );

        /* CHAP Response Packet Header */
        memset( &config_hdr, 0, CONFIG_HDR_LEN );
        config_hdr.code = PPP_CHAP_RESPONSE;
        config_hdr.identifier = pco_vals_ptr->pdn_level_auth_active_id++;
        config_hdr.length = ps_htons(auth_pkt_size);
        COPY_BYTES( buf, &config_hdr, CONFIG_HDR_LEN, offset );

        /* CHAP Response Packet Contents */
        auth_chap_value_size = PPP_CHAP_MD5_SIZE;
        COPY_BYTES( buf, &auth_chap_value_size, sizeof( uint8 ), offset );

        ppp_auth_calc_md5_chap_resp_value(config_hdr.identifier,
                    (unsigned char *)lworkp->pco.pdn_level_auth.password,
                    lworkp->pco.pdn_level_auth.password_len,
                    auth_chap_chal_value,
                    auth_chap_value_size,
                    auth_chap_resp_value);

        COPY_BYTES( buf,
                    auth_chap_resp_value,
                    auth_chap_value_size,
                    offset );
        COPY_BYTES( buf,
                    lworkp->pco.pdn_level_auth.user_id,
                    lworkp->pco.pdn_level_auth.user_id_len,
                    offset );
      }/* end PDN level auth with CHAP */
#endif /* FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH */

      /*---------------------------------------------------------------------
        Request DNS if needed
      ---------------------------------------------------------------------*/
      if( ( ( lworkp->pco.dns_is_requested & IPV4_DNS_REQUEST ) )&&
          ( ( lworkp->pco.dns_v4_addr_req < VSNCP_3GPP2_DNS_V4_REQ_MAX ) ) &&
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
        ( ((uint8)lworkp->req_addr_type & (uint8)VSNCP_3GPP2_PDN_TYPE_IPV4)== 
            (uint8)VSNCP_3GPP2_PDN_TYPE_IPV4 )
#else
        ( lworkp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4 ) 
#endif
      )
      {
        if ( lworkp->pco.dns_v4_addr_req & VSNCP_3GPP2_DNS_V4_REQ_IPCP )
        {
          LOG_MSG_INFO2_1("Adding IPv4 DNS request to PCO via IPCP. "
                          "request type : %d ", lworkp->pco.dns_v4_addr_req);
        /*---------------------------------------------------------------
          Form an IPCP C-Req requesting DNS addresses
        ---------------------------------------------------------------*/
        identifier = ps_htons( PPP_IPCP_PROTOCOL );
        length = VSNCP_3GPP2I_PCO_IPCP_C_REQ_LEN;
        COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
        COPY_BYTES( buf, &length, sizeof( uint8 ), offset );

        /*---------------------------------------------------------------
          Add C-req config hdr. We send the IPCP C-Req with id 1 and
          expect the same in the reply (IPCP C-Ack within the PCO)
        ---------------------------------------------------------------*/
        memset( &config_hdr, 0, CONFIG_HDR_LEN );
        config_hdr.code = CONFIG_REQ;
        config_hdr.identifier = VSNCP_3GPP2I_PCO_IPCP_C_REQ_ID;
        config_hdr.length = ps_htons( VSNCP_3GPP2I_PCO_IPCP_C_REQ_LEN );
        COPY_BYTES( buf, &config_hdr, CONFIG_HDR_LEN, offset );

        /*---------------------------------------------------------------
          Add primary v4 DNS server option
        ---------------------------------------------------------------*/
        memset( &ipcp_opthdr, 0, OPTION_HDR_LEN );
        ipcp_opthdr.type = IPCP_PRIMARY_DNS;
        ipcp_opthdr.length = IPCP_PRIMARY_DNS_SIZ;
        addr.ps_s_addr = 0;
        COPY_BYTES( buf, &ipcp_opthdr, OPTION_HDR_LEN, offset );
        COPY_BYTES( buf, &addr.ps_s_addr, sizeof( struct ps_in_addr ), offset );

        /*---------------------------------------------------------------
          Add secondary v4 DNS server option
        ---------------------------------------------------------------*/
        memset( &ipcp_opthdr, 0, OPTION_HDR_LEN );
        ipcp_opthdr.type = IPCP_SECONDARY_DNS;
        ipcp_opthdr.length = IPCP_SECONDARY_DNS_SIZ;
        COPY_BYTES( buf, &ipcp_opthdr, OPTION_HDR_LEN, offset );
        COPY_BYTES( buf, &addr.ps_s_addr, sizeof( struct ps_in_addr ), offset );
      }

        if ( lworkp->pco.dns_v4_addr_req & VSNCP_3GPP2_DNS_V4_REQ_CONTAINER )
        {
          LOG_MSG_INFO2_1("Adding IPv4 DNS request to PCO via Container. "
                          "request type : %d ", lworkp->pco.dns_v4_addr_req);
          /*---------------------------------------------------------------
            Form a DNS IPv4 Address Request container
          ---------------------------------------------------------------*/
          identifier = ps_htons( VSNCP_3GPP2I_PCO_DNS_V4_ADDR_REQ_ID );
          length = VSNCP_3GPP2I_PCO_DNS_V4_ADDR_REQ_LEN;
          COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
          COPY_BYTES( buf, &length, sizeof( uint8 ), offset );
        }
      }

      if( ( ( lworkp->pco.dns_is_requested & IPV6_DNS_REQUEST ) )&&
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
        ( ((uint8)lworkp->req_addr_type & (uint8)VSNCP_3GPP2_PDN_TYPE_IPV6) == 
            (uint8)VSNCP_3GPP2_PDN_TYPE_IPV6 )
#else
        ( lworkp->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6 )
#endif
      )
      {
        LOG_MSG_INFO2_1("Adding IPv6 DNS request to PCO dns mask : %d ",
                        lworkp->pco.dns_is_requested);
        /*---------------------------------------------------------------
          Form a DNS Addr Req container
        ---------------------------------------------------------------*/
        identifier = ps_htons( VSNCP_3GPP2I_PCO_DNS_V6_ADDR_REQ_ID );
        length = VSNCP_3GPP2I_PCO_DNS_V6_ADDR_REQ_LEN;
        COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
        COPY_BYTES( buf, &length, sizeof( uint8 ), offset );
      }
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
      if ( lworkp->req_addr_type <= VSNCP_3GPP2_PDN_TYPE_MIN ||
           lworkp->req_addr_type >= VSNCP_3GPP2_PDN_TYPE_MAX )  
#else
      else 
#endif
      {
        LOG_MSG_ERROR_1("Unexpected req_addr_type 0x%x",
                        lworkp->req_addr_type);
        ASSERT(0);
      }

      /*---------------------------------------------------------------------
        Add P-CSCF request if needed
      ---------------------------------------------------------------------*/
      if( lworkp->pco.p_cscf_is_requested )
      {
        switch( lworkp->req_addr_type )
        {
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
          case VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6:
#endif
          case VSNCP_3GPP2_PDN_TYPE_IPV4:
            /*---------------------------------------------------------------
              Form a v4 P-CSCF Addr Req container
            ---------------------------------------------------------------*/
            identifier = ps_htons( VSNCP_3GPP2I_PCO_V4_PCSCF_ADDR_REQ_ID );
            length = VSNCP_3GPP2I_PCO_V4_PCSCF_ADDR_REQ_LEN;
            COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
            COPY_BYTES( buf, &length, sizeof( uint8 ), offset );
            /*Fall through to form a v6 P-CSCF req as well*/
          case VSNCP_3GPP2_PDN_TYPE_IPV6:
            /*---------------------------------------------------------------
              Form a v6 P-CSCF Addr Req container
            ---------------------------------------------------------------*/
            identifier = ps_htons( VSNCP_3GPP2I_PCO_PCSCF_ADDR_REQ_ID );
            length = VSNCP_3GPP2I_PCO_PCSCF_ADDR_REQ_LEN;
            COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
            COPY_BYTES( buf, &length, sizeof( uint8 ), offset );
            break;

          default:
            LOG_MSG_ERROR_1("Unexpected req_addr_type 0x%x",
                            lworkp->req_addr_type);
            ASSERT(0);
            break;
        } /* req_addr_type */
      } /* p_cscf */
      /*---------------------------------------------------------------------
        Add operator reserved PCO request if needed
      ---------------------------------------------------------------------*/
      if( lworkp->pco.op_pco.container_id >= PPP_OP_PCO_ID_START )
      {
        LOG_MSG_INFO2_1("op pco with id: %x is needed",
                        lworkp->pco.op_pco.container_id);
        identifier = ps_htons( lworkp->pco.op_pco.container_id );
        length = VSNCP_3GPP2I_PCO_OP_PCO_REQUEST_LEN;
        /*---------------------------------------------------------------------
          Encode MCC MNC
        ---------------------------------------------------------------------*/
        if(vsncp_3gpp2i_encode_mccmnc(&lworkp->pco.op_pco, 
                                      mccmnc, 
                                      VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN))
        {
          LOG_MSG_INFO2_3("op pco id:%x, len:%x, mccmnc:%x%x%x",
                          identifier, length, mccmnc);
          LOG_MSG_INFO2_3("op pco mccmnc:%x%x%x",
                          mccmnc[0], mccmnc[1], mccmnc[2]);
          COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
          COPY_BYTES( buf, &length, sizeof( uint8 ), offset );
          COPY_BYTES( buf, mccmnc, 
                      VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN, offset );
        }
      }
      /*---------------------------------------------------------------------
        Include the BCM parameter (MS support of Network Requested
        Bearer Control Indicator) only if needed
      ---------------------------------------------------------------------*/
      if( lworkp->pco.bcm_is_supported )
      {
        identifier = ps_htons( VSNCP_3GPP2I_PCO_MS_BCM_ID );
        length = VSNCP_3GPP2I_PCO_MS_BCM_LEN;
        COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
        COPY_BYTES( buf, &length, sizeof( uint8 ), offset );
      }

      /*---------------------------------------------------------------------
        Include the PCO of IP address allocation via NAS signalling
        if needed.
      ---------------------------------------------------------------------*/
      if(lworkp->pco.ip_addr_via_nas)
      {
        identifier = ps_htons( VSNCP_3GPP2I_PCO_IP_ADDR_ALLOC_VIA_NAS_ID );
        length = VSNCP_3GPP2I_PCO_IP_ADDR_ALLOC_VIA_NAS_LEN;
        COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
        COPY_BYTES( buf, &length, sizeof( uint8 ), offset );
      }

#ifdef FEATURE_DS_MTU_PCO_VSNCP     
       /*---------------------------------------------------------------------
        Request MTU if needed
      ---------------------------------------------------------------------*/
      if( lworkp->pco.mtu_is_requested )
      {
        switch( lworkp->req_addr_type )
        {
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
         case VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6:
#endif
         case VSNCP_3GPP2_PDN_TYPE_IPV4:
         case VSNCP_3GPP2_PDN_TYPE_IPV6:
            /*---------------------------------------------------------------
              Form a mtu container
            ---------------------------------------------------------------*/
            identifier = ps_htons( VSNCP_3GPP2I_PCO_MTU_REQ_ID );
            length = VSNCP_3GPP2I_PCO_MTU_REQ_LEN;
            COPY_BYTES( buf, &identifier, sizeof( uint16 ), offset );
            COPY_BYTES( buf, &length, sizeof( uint8 ), offset );
            break;

          default:
            LOG_MSG_ERROR_1("Unexpected req_addr_type 0x%x",
                            lworkp->req_addr_type);
							
            break;
        } /* req_addr_type */
      } /* MTU */
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
      opthdr.length = (uint8)offset; 
      COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, opthdr_offset );
      break;

    case VSNCP_3GPP2I_ATTACH_TYPE:
      opthdr.length = OPTION_HDR_LEN + sizeof( uint8 );
      COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
      COPY_BYTES( buf, &value_p->attach_type, sizeof( uint8), offset );
      break;

    case VSNCP_3GPP2I_DEFAULT_ROUTER:
      opthdr.length = OPTION_HDR_LEN + sizeof( struct ps_in_addr );
      COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
      COPY_BYTES( buf,
                  &value_p->default_rtr_ip4.ps_s_addr,
                  sizeof( struct ps_in_addr ),
                  offset );
      break;

    case VSNCP_3GPP2I_ADDR_ALLOC_CAUSE:
      opthdr.length = OPTION_HDR_LEN + sizeof( uint8 );
      COPY_BYTES( buf, &opthdr, OPTION_HDR_LEN, offset );
      /*---------------------------------------------------------------------
        As per X.P0057 the address allocation cause should be set to NULL
        for the VSNCP Config request going out in case of both Initial
        attach and Handover attach.
      ---------------------------------------------------------------------*/
      value_p->addr_alloc_cause = (uint8)VSNCP_3GPP2_ADDR_ALLOC_CAUSE_NULL;
      COPY_BYTES( buf, &value_p->addr_alloc_cause, sizeof( uint8), offset );
      break;

    default:
      LOG_MSG_INFO1_1( "Got invalid 3GPP2-VSNCP option %d", opt_num);
      ASSERT(0);
      break;
  } /* switch(option) */;

  if( offset > 0 )
  {
    if( dsm_pushdown_tail( pkt_ptr_ptr,
                           buf,
                           offset,
                           DSM_DS_SMALL_ITEM_POOL ) != offset )
    {
      LOG_MSG_ERROR_1("Error adding option 0x%x to pkt", opt_num);
    }
  }
} /* vsncp_3gpp2i_option() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_DECODE_MCCMNC()

DESCRIPTION
  To decode MCCMNC in operator reserved PCO received from remote

PARAMETERS
  (IN) mccmnc_ptr  : Pointer to the mccmnc in pco containter content
  (IN) mccmnc_len  : Length of the mccmnc in pco container content
  (OUT) op_pco_ptr  : Pointer to the structure for saving op reserved PCO content

RETURN VALUE
  TRUE: Success
  FALSE: Fail

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean vsncp_3gpp2i_decode_mccmnc(
                                   uint8 *mccmnc_ptr,
                                   uint8 mccmnc_len,
                                   vsncp_3gpp2i_op_pco_type *op_pco_ptr
                                   )
{
  if( NULL == mccmnc_ptr ||
      VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN != mccmnc_len ||
      NULL == op_pco_ptr )
  {
    LOG_MSG_ERROR_3("vsncp_3gpp2i_decode_mccmnc, Invalid input: %x, %d, %x",
                    mccmnc_ptr, mccmnc_len, op_pco_ptr );
    return FALSE;
  }
  /*--------------------------------------------------------------------------- 
    decode MCC MNC: 
    |--8-7-6-5--|--4-3-2-1--|
    |MCC Digit2 | MCC Digit1|
    |MNC Digit3 | MCC Digit3|
    |MNC Digit2 | MNC Digit1|
    The MNC shall consist of 2 or 3 digits,a network operator may decide to use
    only two digits in the MNC, in this case, MNC Digit3 shall be coded as "1111".  
    ---------------------------------------------------------------------------*/
  op_pco_ptr->mcc[0] = mccmnc_ptr[0] & 0x0F;
  op_pco_ptr->mcc[1] = (mccmnc_ptr[0] & 0xF0) >> 4;
  op_pco_ptr->mcc[2] = mccmnc_ptr[1] & 0x0F;
  op_pco_ptr->mcc_len = PPP_MCC_LEN;
  LOG_MSG_INFO2_3("vsncp decoded mcc: %x%x%x",
                  op_pco_ptr->mcc[0], op_pco_ptr->mcc[1], op_pco_ptr->mcc[2]);

  op_pco_ptr->mnc[0] = mccmnc_ptr[2] & 0x0F;
  op_pco_ptr->mnc[1] = (mccmnc_ptr[2] & 0xF0) >> 4;
  op_pco_ptr->mnc[2] = (mccmnc_ptr[1] & 0xF0) >> 4;
  if( 0x0F == op_pco_ptr->mnc[2] )
  {
    op_pco_ptr->mnc_len = PPP_MNC_LEN - 1;
  }
  else
  {
    op_pco_ptr->mnc_len = PPP_MNC_LEN;
  }
  LOG_MSG_INFO2_1("vsncp decoded mnc_len: %d", op_pco_ptr->mnc_len);
  LOG_MSG_INFO2_3("vsncp decoded mcc: %x%x%x",
                  op_pco_ptr->mnc[0], op_pco_ptr->mnc[1], op_pco_ptr->mnc[2]);
  return TRUE;
}
/*===========================================================================
FUNCTION VSNCP_3GPP2I_CHECK()

DESCRIPTION
  This function will check options in INCOMING C-Req and C-Ack packets,
  updating the working values.
  - Remote work values are updated for a C-Req (request == TRUE )
  - Local work values are updated for a C-Ack (request == FALSE)

  The following semantic is used once an option has been checked:
  RECOGNIZED - Option is valid and is appropriate per the 'request' param
  UNRECOGNIZED - Option is invalid or inappropriate per the 'request' param
  UNACCEPTABLE - NCP setup cannot continue! The function will initiate
  connection termination and return a failure (-1)

  The table below lists option result for various option types.
  +-------------------------------+------------------+------------------+
  | Option                        |    In C-Req      |     In C-Ack     |
  +-------------------------------+------------------+------------------+
  | VSNCP_3GPP2I_PDN_ID           |   RECOGNIZED     |    RECOGNIZED    |
  +-------------------------------+------------------+------------------+
  | VSNCP_3GPP2I_APN              |  UNRECOGNIZED    |    RECOGNIZED    |
  +-------------------------------+------------------+------------------+
  | VSNCP_3GPP2I_PDN_TYPE         |  UNRECOGNIZED    |    RECOGNIZED    |
  +-------------------------------+------------------+------------------+
  | VSNCP_3GPP2I_PDN_ADDR         |  UNRECOGNIZED    |    RECOGNIZED    |
  +-------------------------------+------------------+------------------+
  | VSNCP_3GPP2I_PCO              |  UNRECOGNIZED    |    RECOGNIZED    |
  +-------------------------------+------------------+------------------+
  | VSNCP_3GPP2I_ERROR_CODE       |  UNRECOGNIZED    |   UNRECOGNIZED   |
  | (only valid for a C-Rej)      |                  |                  |
  +-------------------------------+------------------+------------------+
  | VSNCP_3GPP2I_ATTACH_TYPE      |  UNRECOGNIZED    |    RECOGNIZED    |
  +-------------------------------+------------------+------------------+
  | VSNCP_3GPP2I_DEFAULT_ROUTER   |  UNRECOGNIZED    |    RECOGNIZED    |
  |                               |    (for v4)      |     (for v4)     |
  +-------------------------------+------------------+------------------+
  | VSNCP_3GPP2I_ADDR_ALLOC_CAUSE |  UNRECOGNIZED    |    RECOGNIZED    |
  +-------------------------------+------------------+------------------+
  | Unknown option                |  UNRECOGNIZED    |   UNRECOGNIZED   |
  +-------------------------------+------------------+------------------+

PARAMETERS
  (IN) fsm_ptr     : Pointer to the FSM structure
  (IN) ohp         : Pointer to the option header
  (IN) pkt_ptr_ptr : Pointer to pointer to the DSM item
  (IN) request     : TRUE if this is a C-Req, FALSE if this is a C-Ack

RETURN VALUE
  -1: if an error occurs
  RECOGNIZED: if option is as expected
  UNRECOGNIZED: if this option should be ignored

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int vsncp_3gpp2i_check
(
  ppp_fsm_type       * fsm_ptr,
  struct option_hdr  * ohp,
  struct dsm_item_s ** pkt_ptr_ptr,
  int                  request
)
{
  int used, n_type;
  int v6iid_size;
  uint8 apn_len, pdn_id, pco_cfg_proto;
  int32 signed_len;
  struct config_hdr cnf_hdr;
  struct option_hdr opt_hdr;
  uint16 container_id;
  uint8 container_len;
  int32 signed_container_len;
  int option_result = UNRECOGNIZED;
  ppp_fsm_side_type * remote_p = NULL;
  ppp_fsm_side_type * local_p = NULL;
  vsncp_3gpp2_local_opts_val_type  * lwant_ptr = NULL;
  vsncp_3gpp2_local_opts_val_type  * lwork_ptr = NULL;
  vsncp_3gpp2i_pco_opts_val_type * pco_vals_ptr = NULL;
  vsncp_3gpp2i_pco_v4_addr * v4_addr_ptr = NULL;
  vsncp_3gpp2i_pco_v6_addr * v6_addr_ptr = NULL;
#ifdef FEATURE_DS_MTU_PCO_VSNCP
  uint16 mtu = 0;
#endif /* FEATURE_DS_MTU_PCO_VSNCP */
#ifdef FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH
  ppp_auth_proto_e_type pdn_level_auth_protocol;
#endif /* FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH */
  uint32  dns_v4_addr;
  boolean is_dns_v4_primary_addr_obtained = FALSE;
  boolean is_dns_v4_secondary_addr_obtained = FALSE;
  uint8 mccmnc[VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN] = {0};

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( pkt_ptr_ptr == NULL || *pkt_ptr_ptr == NULL )
  {
    LOG_MSG_ERROR_0("Invalid DSM in vsncp_3gpp2i_check");
    return -1;
  }

  if( fsm_ptr == NULL              ||
      ohp == NULL                  ||
      ohp->length < OPTION_HDR_LEN ||
      fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid args in vsncp_3gpp2i_check");
    dsm_free_packet(pkt_ptr_ptr);
    return -1;
  }

  used = ohp->length - OPTION_HDR_LEN;

  remote_p = &(fsm_ptr->remote);
  local_p = &(fsm_ptr->local);
  lwant_ptr = (vsncp_3gpp2_local_opts_val_type *)(local_p->want_pdv);
  lwork_ptr = (vsncp_3gpp2_local_opts_val_type *)(local_p->work_pdv);

  /*-------------------------------------------------------------------------
    Just skip this option if it's an unrecognized option
  -------------------------------------------------------------------------*/
  n_type = fsm_n_option( fsm_ptr->pdc, ohp->type );
  if( ( -1 == n_type ) ||
      ( ohp->type > VSNCP_3GPP2I_OPTION_LIMIT )  ||
      ( !( ( local_p->will | local_p->want ) & ( 1 << n_type ) ) &&
        !( ( remote_p->will | remote_p->want ) & ( 1 << n_type ) ) ) )
  {
    /*-----------------------------------------------------------------------
      Unrecognized option toss any bytes in option
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_1("Invalid option 0x%x in vsncp_3gpp2i_check", ohp->type);
    if( used > 0 )
    {
      if( dsm_pullup( pkt_ptr_ptr, NULL, (uint16)used ) != (uint16)used )
      {
        LOG_MSG_ERROR_0("Unable to toss bytes!");
        return -1;
      }
    }
    return UNRECOGNIZED;
  } /* if(invalid option) */

  /*-------------------------------------------------------------------------
    Check the option length.  The length is bad if:
    1. It is less than the minimum length OR
    2. Greater than the maximum length, unless it's APN, PDN_ADDR or PCO.
  -------------------------------------------------------------------------*/
  if( ( ohp->length < option_length[n_type] ) ||
      ( ( ohp->length > option_length[n_type] )  &&
        ( ohp->type != VSNCP_3GPP2I_PDN_ADDR ) &&
        ( ohp->type != VSNCP_3GPP2I_PCO ) &&
        ( ohp->type != VSNCP_3GPP2I_APN ) ) )
  {
    /*-----------------------------------------------------------------------
      toss any bytes in option
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("Invalid option length in vsncp_3gpp2i_check");
    if( used > 0 )
    {
      if( dsm_pullup( pkt_ptr_ptr, NULL, (uint16)used ) != (uint16)used )
      {
        LOG_MSG_ERROR_0("Unable to toss bytes!");
        return -1;
      }
    }
    return UNRECOGNIZED;
  }

  switch( ohp->type )
  {
    case VSNCP_3GPP2I_PDN_ID:
      /*---------------------------------------------------------------------
        Expected in both a C-Req and a C-Ack; drop if it doesn't match ours
      ---------------------------------------------------------------------*/
      PULLUP_BYTES( pkt_ptr_ptr, &pdn_id, sizeof( uint8 ), used );
      if( pdn_id != lwant_ptr->pdn_id )
      {
        /*-------------------------------------------------------------------
          This can happen only for a C-Ack that came in with a wrong pdn_id.
          If a Config-Req came in with a wrong pdn-id, it would get handled
          (send a Config-Rej) in vsncp_3gpp2_receive().
        -------------------------------------------------------------------*/
        return -1;
      }
      option_result = RECOGNIZED;
      break;

    case VSNCP_3GPP2I_APN:
      /*---------------------------------------------------------------------
        Expected in a C-Ack C-Req
      ---------------------------------------------------------------------*/
      apn_len = ohp->length - OPTION_HDR_LEN;
      if( ( apn_len > 0 ) && ( apn_len <= VSNCP_3GPP2_APN_MAX_VAL_LEN ) )
      {
        PULLUP_BYTES( pkt_ptr_ptr, &(lwork_ptr->apn[0]), apn_len, used );
      lwork_ptr->apn_len = apn_len;
      }
      option_result = RECOGNIZED;
      
      break;

    case VSNCP_3GPP2I_DEFAULT_APN_IND:
      /*---------------------------------------------------------------------
        Expected in a C-Req.
      ---------------------------------------------------------------------*/
      if(request == TRUE)
      {
        /*-----------------------------------------------------------------
           Default APN: 1;
           None-default APN: 0;
        -----------------------------------------------------------------*/
        PULLUP_BYTES( pkt_ptr_ptr, 
                      &(lwork_ptr->apn_is_default),
                      sizeof(boolean), 
                      used );
        option_result = RECOGNIZED;       
        break;
      }
      
    case VSNCP_3GPP2I_PDN_TYPE:
      /*---------------------------------------------------------------------
        Expected in a C-Ack. Lwork’s IP version capability need not be 
        updated with the pdn_type value since lwork contains the AT’s 
        capability and the capability should not change per incoming Config 
        ACK. This also ensures that during resync AT’s capability is correctly 
        advertised i.e. as done during initial establishment.
      ---------------------------------------------------------------------*/
      if( request == FALSE )
      {
        TOSS_BYTES( pkt_ptr_ptr,
                    sizeof( uint8 ),
                    used );
        option_result = RECOGNIZED;
      }
      break;

    case VSNCP_3GPP2I_PDN_ADDR:
      /*---------------------------------------------------------------------
        Expected in a C-Ack. Extract the address and store locally.
        Once negotiation is complete, we will continue (or not) based on
        what address type we got and what we need etc. (see _opening())
      ---------------------------------------------------------------------*/
      if( request == FALSE )
      {
        PULLUP_BYTES( pkt_ptr_ptr,
                      &(lwork_ptr->req_addr_type),
                      sizeof( uint8 ),
                      used );

        switch( lwork_ptr->req_addr_type )
        {
          case VSNCP_3GPP2_PDN_TYPE_IPV4:
            if( used != SIZE_OF_V4_ADDR )
            {
              LOG_MSG_ERROR_1("Error! v4 Invalid PDN address len %d", used);
              return -1;
            }
            PULLUP_BYTES( pkt_ptr_ptr,
                          &(lwork_ptr->ip4.ps_s_addr),
                          sizeof(struct ps_in_addr),
                          used );
            option_result = RECOGNIZED;
            break;

          case VSNCP_3GPP2_PDN_TYPE_IPV6:
            if( used != SIZE_OF_V6_IID )
            {
              LOG_MSG_ERROR_1("Error! Invalid v6 PDN address len %d", used);
              return -1;
            }
            PULLUP_BYTES( pkt_ptr_ptr,
                          &lwork_ptr->iid,
                          sizeof( uint64 ),
                          used );
            option_result = RECOGNIZED;
            break;

          case VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6:
            if( used != ( SIZE_OF_V4_ADDR + SIZE_OF_V6_IID ) )
            {
              LOG_MSG_ERROR_1("Error! Invalid v4_v6 PDN address len %d", used);
              return -1;
            }

            /*-------------------------------------------------------------
              If v6 was the requested address type pull out the v6
              address.
            -------------------------------------------------------------*/
            if( lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6 )
            {
              PULLUP_BYTES( pkt_ptr_ptr,
                            &lwork_ptr->iid,
                            sizeof( uint64 ),
                            used );
              option_result = RECOGNIZED;
              LOG_MSG_INFO2_0("Tossing remaining bytes since req_addr_type v6 ");
              TOSS_BYTES( pkt_ptr_ptr, used, used );
             }

            /*-------------------------------------------------------------
              If v4 was the requested address type pull out the v4
              address.
            -------------------------------------------------------------*/
            else if( lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4 )
            {
              LOG_MSG_INFO2_0("Tossing v6 address - req_addr_type is v4 ");
              v6iid_size = SIZE_OF_V6_IID;
              TOSS_BYTES( pkt_ptr_ptr, v6iid_size, used );
              PULLUP_BYTES( pkt_ptr_ptr,
                            &(lwork_ptr->ip4.ps_s_addr),
                            sizeof(struct ps_in_addr),
                            used );
              option_result = RECOGNIZED;
             }

            /*-------------------------------------------------------------
              If v4_v6 was the requested address type pull out both the
              address.
            -------------------------------------------------------------*/
            else if( lwant_ptr->req_addr_type ==
                     VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 )
            {
              PULLUP_BYTES( pkt_ptr_ptr,
                            &lwork_ptr->iid,
                            sizeof( uint64 ),
                            used );

              PULLUP_BYTES( pkt_ptr_ptr,
                            &(lwork_ptr->ip4.ps_s_addr),
                            sizeof(struct ps_in_addr),
                            used );

              option_result = RECOGNIZED;
            }
            break;

          default:
            /* unexpected */
            LOG_MSG_ERROR_0("Unexpected addr type!");
            option_result = UNRECOGNIZED;
            return -1;
        } /* switch( req_addr_type ) */
      }
      break;

    case VSNCP_3GPP2I_PCO:
      /*---------------------------------------------------------------------
        Expected in a C-Ack
      ---------------------------------------------------------------------*/
      if( request == FALSE )
      {
        /*-------------------------------------------------------------------
          Format of PCO option value field
          pco configuration protocol (1 byte), followed by  0 or more
          "containers" with the format:
          2 byte container id
          1 byte container length
          0 or more bytes of container contents

            +----+----+----+----+----+----+----+----+
            |           PCO Option Type             | Octet1 (already pulled)
            +----+----+----+----+----+----+----+----+
            |           PCO Option Length           | Octet2 (already pulled)
         ==>+----+----+----+----+----+----+----+----+
            | 1  | 0  | 0  | 0  | 0  |   cfg_proto  | Octet3 = 0x80
            +----+----+----+----+----+----+----+----+
            |                                       | Octet4
            +            Protocol ID 1              +
            |                                       | Octet5
            +----+----+----+----+----+----+----+----+
            |   Length of Protocol ID 1 contents    | Octet6
            +----+----+----+----+----+----+----+----+
            |                                       | Octet7
            +         Protocol ID 1 contents        +
            |                ...                    | Octetm
            +----+----+----+----+----+----+----+----+
        -------------------------------------------------------------------*/
        PULLUP_BYTES( pkt_ptr_ptr, &pco_cfg_proto, sizeof( uint8 ), used );
        if( pco_cfg_proto != VSNCP_3GPP2I_PCO_CONFIG_PROTOCOL )
        {
          LOG_MSG_ERROR_1("Unrecognized config proto 0x%x in PCO",
                          pco_cfg_proto);
          /* throw away the remainder of this option */
          break;
        }

        pco_vals_ptr =
          &(((ncpi_cb_type *)fsm_ptr->pdv)->cb_u.vsncp_3gpp2.pco_vals);
        /*-------------------------------------------------------------------
          Process individual containers if any. A PCO container must have
          atleast 3 bytes
        -------------------------------------------------------------------*/
        signed_len = used;
        while (signed_len >= VSNCP_3GPP2I_PCO_CONTAINER_HDR_LEN )
        {
          PULLUP_BYTES( pkt_ptr_ptr, &container_id, sizeof( uint16 ), used );
          PULLUP_BYTES( pkt_ptr_ptr, &container_len, sizeof( uint8 ), used );
          signed_len -= container_len;
          if( signed_len < 0 )
          {
            LOG_MSG_ERROR_0("pco container len > pco option len");
            return -1;
          }

          signed_container_len = 0;
          signed_container_len = container_len;
          container_id = ps_ntohs( container_id );
          /*---------------------------------------------------------------
            Handling Operator Reserved PCO
            Note: No need to validate the PCO ID, since we need to support 
            unsolicitated operator reserved PCO. Note that, for current
            design, iface only save one op reserved PCO container content,
            so the op PCO that comes later will overwrite the previous one.           
          ---------------------------------------------------------------*/
          if( container_id >= PPP_OP_PCO_ID_START )
          {
            LOG_MSG_INFO2_1("op pco with ID: %x received", container_id);
            /*---------------------------------------------------------------
              An operator reserved PCO must contain MCCMNC encoded according
              to section 10.5.1.3 in 3GPP TS 24.008, which is 3 bytes in total
            ---------------------------------------------------------------*/
            if(container_len < VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN)
            {
              LOG_MSG_ERROR_0("op pco container len < mccmnc length, throw away this PCO" );
              TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
              signed_len = used;
              break;
            }
            pco_vals_ptr->op_pco_content.container_id = container_id;
            /*---------------------------------------------------------------
              Pull up MCCMNC
            ---------------------------------------------------------------*/
            PULLUP_BYTES( pkt_ptr_ptr,
                          mccmnc,
                          VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN,
                          used );
            LOG_MSG_INFO2_3("op pco mccmnc: %x %x %x",
                            mccmnc[0], mccmnc[1], mccmnc[2]);
            if(!vsncp_3gpp2i_decode_mccmnc(mccmnc, 
                                       VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN, 
                                       &pco_vals_ptr->op_pco_content
                                       ))
            {
              LOG_MSG_ERROR_0("op pco container decode mccmnc fails, throw away this PCO" );
              TOSS_BYTES( pkt_ptr_ptr, 
                          container_len - VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN, 
                          used 
                          );
              signed_len = used;
              break;

            }
            /*---------------------------------------------------------------
              Pull up application specific information which comes after MCCMNC
            ---------------------------------------------------------------*/
            if( container_len > VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN )
            {
              memset(pco_vals_ptr->op_pco_content.app_specific_info,
                     0,
                     VSNCP_3GPP2I_MAX_OP_PCO_CONTENT_LEN - VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN);

              PULLUP_BYTES(pkt_ptr_ptr,
                           pco_vals_ptr->op_pco_content.app_specific_info,
                           container_len - VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN,
                           used
                           );
              pco_vals_ptr->op_pco_content.app_specific_info_len = 
                               container_len - VSNCP_3GPP2I_OP_PCO_MCCMNC_LEN;
            }
          }
          else
          {
            switch( container_id )
            {
  #ifdef FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH
              case PPP_PAP_PROTOCOL:
              case PPP_CHAP_PROTOCOL:
                pdn_level_auth_protocol = lwork_ptr->pco.pdn_level_auth.protocol;
                /*---------------------------------------------------------------
                  Did we even request PAP/CHAP Auth ? If not, toss this container
                ---------------------------------------------------------------*/
                if( pdn_level_auth_protocol != PPP_AUTH_PAP &&
                    pdn_level_auth_protocol != PPP_AUTH_CHAP )
                {
                  LOG_MSG_ERROR_0("Tossing PAP/CHAP auth container, not requested!");
                  TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                  break;
                }

              /*---------------------------------------------------------------
                Did we request PAP and got a CHAP response? or vice versa
              ---------------------------------------------------------------*/
              if( ( (pdn_level_auth_protocol == PPP_AUTH_PAP) &&
                    (container_id != PPP_PAP_PROTOCOL) ) ||
                  ( (pdn_level_auth_protocol == PPP_AUTH_CHAP) &&
                    (container_id != PPP_CHAP_PROTOCOL) ) )
              {
                LOG_MSG_ERROR_0("Tossing PAP/CHAP auth container, incorrect auth!");
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }

              /*-------------------------------------------------------------
                Pullup the PAP/CHAP packet config header
              -------------------------------------------------------------*/
              if( ntohcnf( &cnf_hdr, pkt_ptr_ptr ) == -1 )
              {
                LOG_MSG_ERROR_0("Error grabbing PAP/CHAP cnf hdr");
                return -1;
              }

              /*-------------------------------------------------------------
                The config hdr length should match length of the PCO
                container, else return failure
              -------------------------------------------------------------*/
              if( cnf_hdr.length != signed_container_len )
              {
                LOG_MSG_ERROR_2("Error! PAP/CHAP hdr len %d, container len %d",
                                cnf_hdr.length, signed_container_len );
                return -1;
              }

              /*-------------------------------------------------------------
                Decrement values since header has been grabbed (ntohcnf)
              -------------------------------------------------------------*/
              used -= CONFIG_HDR_LEN;
              signed_container_len -= CONFIG_HDR_LEN;

              /*-------------------------------------------------------------
                If the ID matches, authentication is complete
              -------------------------------------------------------------*/
              if( cnf_hdr.identifier != 
                          (pco_vals_ptr->pdn_level_auth_active_id-1) )
              {
                LOG_MSG_ERROR_3("Error! CHAP/PAP Auth packet id does not match "
                              "code=%d, id=%d, len=%d",
                              cnf_hdr.code,
                              cnf_hdr.identifier,
                              cnf_hdr.length );
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }

              if( container_id == PPP_PAP_PROTOCOL )
              {
                switch( cnf_hdr.code )
                {
                  case PPP_PAP_ACK:
                    LOG_MSG_INFO2_0("PDN Level Auth Success using PAP");
                    break;

                  case PPP_PAP_NAK:
                    LOG_MSG_INFO2_0("PDN Level Auth Failure using PAP");
                    break;

                  default:
                    LOG_MSG_ERROR_1("PDN Level Auth using PAP,"
                                  "Received incorrect code=%d",
                                    cnf_hdr.code);
                    break;
                }
              }
              else /* container_id == PPP_CHAP_PROTOCOL */
              {
                switch( cnf_hdr.code )
                {
                  case PPP_CHAP_SUCCESS:
                    LOG_MSG_INFO2_0("PDN Level Auth Success using CHAP");
                    break;
                
                  case PPP_CHAP_FAILURE:
                    LOG_MSG_INFO2_0("PDN Level Auth Failure using CHAP");
                    break;

                  default:
                    LOG_MSG_ERROR_1("PDN Level Auth using CHAP,"
                                  "Received incorrect code=%d",
                                    cnf_hdr.code);
                    break;
                }
              }

              /* Discard rest of packet */
              TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
              break;
#endif /* FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH */

            case PPP_IPCP_PROTOCOL:
              /*-------------------------------------------------------------
                The IPCP C-Ack coming in as part of this PCO container
                *should* have both the primary and secondary DNS options.
                However, we don't enforce this ("generous in receivng") -
                so long as there is atleast one of them present, we will
                process this container.
              -------------------------------------------------------------*/
              if( signed_container_len <
                    ( CONFIG_HDR_LEN + IPCP_PRIMARY_DNS_SIZ ) )
              {
                LOG_MSG_INFO2_1("IPCP container too short! Only %d bytes. Tossing.",
                                signed_container_len);
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }

              /*-------------------------------------------------------------
                Toss this container if we did not request a DNS v4 address via
                IPCP or if this is a V6 only PDN.
              -------------------------------------------------------------*/
              if( ( lwant_ptr->pco.dns_is_requested == FALSE ) ||
                 !( lwant_ptr->pco.dns_v4_addr_req & VSNCP_3GPP2_DNS_V4_REQ_IPCP ) ||
                  ( lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6 ) )
              {
                LOG_MSG_ERROR_0("Invalid DNS v4 PCO request parameters. Tossing "
                                "DNS v4 IPCP Response container.");
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }

              /*-------------------------------------------------------------
                Pullup the IPCP config header
              -------------------------------------------------------------*/
              if( ntohcnf( &cnf_hdr, pkt_ptr_ptr ) == -1 )
              {
                LOG_MSG_ERROR_0("Error grabbing IPCP PCO cnf hdr");
                return -1;
              }

              /*-------------------------------------------------------------
                The config hdr length should match length of the PCO
                container, else return failure
              -------------------------------------------------------------*/
              if( cnf_hdr.length != signed_container_len )
              {
                LOG_MSG_ERROR_2("Error! IPCP hdr len %d, container len %d",
                                cnf_hdr.length, signed_container_len );
                return -1;
              }

              /*-------------------------------------------------------------
                Decrement values since header has been grabbed (ntohcnf)
              -------------------------------------------------------------*/
              used -= CONFIG_HDR_LEN;
              signed_container_len -= CONFIG_HDR_LEN;

              /*-------------------------------------------------------------
                The IPCP config hdr should indicate appropriate values
              -------------------------------------------------------------*/
              if( ( ( cnf_hdr.code != CONFIG_ACK ) && 
                    ( cnf_hdr.code != CONFIG_NAK ) ) ||
                  ( cnf_hdr.identifier != 1 ) )
              {
                LOG_MSG_ERROR_3("Error! IPCP hdr code %d, id %d, len %d",
                          cnf_hdr.code,
                          cnf_hdr.identifier,
                          cnf_hdr.length );
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }

              /*-------------------------------------------------------------
                Process the options within the IPCP C-Ack packet
              -------------------------------------------------------------*/
              while( signed_container_len > 0 )
              {
                /*-----------------------------------------------------------
                  Pullup the IPCP option header
                -----------------------------------------------------------*/
                if( ntohopt( &opt_hdr, pkt_ptr_ptr ) == -1 )
                {
                  LOG_MSG_ERROR_0("Error grabbing IPCP PCO option");
                  return -1;
                }
                used -= OPTION_HDR_LEN;

                /*-----------------------------------------------------------
                  If option header indicates a length larger than the number
                  of bytes left in the container then flag an error.
                -----------------------------------------------------------*/
                signed_container_len -= opt_hdr.length;
                if( signed_container_len < 0 )
                {
                  LOG_MSG_ERROR_1("Error! IPCP PCO signed_container_len = %d",
                                  signed_container_len);
                  return -1;
                }

                /*-----------------------------------------------------------
                  Skip the option if it is empty
                -----------------------------------------------------------*/
                if( opt_hdr.length == 0 )
                {
                  continue;
                }

                switch( opt_hdr.type )
                {
                  case IPCP_PRIMARY_DNS:
                    /*-------------------------------------------------------
                      Validate option length and update local PCO vals with
                      the received primary IPv4 DNS address
                    -------------------------------------------------------*/
                    if( opt_hdr.length != IPCP_PRIMARY_DNS_SIZ )
                    {
                      LOG_MSG_ERROR_1("prim DNS opt len %d", opt_hdr.length);
                      return -1;
                    }
                    LOG_MSG_INFO2_0("Received DNS v4 primary address in PCO IPCP");
                    PULLUP_BYTES( pkt_ptr_ptr,
                                  &(pco_vals_ptr->dns_v4_primary.ps_s_addr),
                                  sizeof( struct ps_in_addr ),
                                  used );
                    break;

                  case IPCP_SECONDARY_DNS:
                    /*-------------------------------------------------------
                      Validate option length and update local PCO vals with
                      the received secondary IPv4 DNS address
                    -------------------------------------------------------*/
                    if( opt_hdr.length != IPCP_SECONDARY_DNS_SIZ )
                    {
                      LOG_MSG_ERROR_1("sec DNS opt len %d", opt_hdr.length);
                      return -1;
                    }
                    LOG_MSG_INFO2_0("Received DNS v4 secondary address in PCO IPCP");
                    PULLUP_BYTES( pkt_ptr_ptr,
                                  &(pco_vals_ptr->dns_v4_secondary.ps_s_addr),
                                  sizeof( struct ps_in_addr ),
                                  used );
                    break;

                  default:
                    LOG_MSG_ERROR_1("Unrecognized ipcp option 0x%x in IPCP C-Ack",
                                    opt_hdr.type);
                    TOSS_BYTES( pkt_ptr_ptr,
                                (opt_hdr.length - OPTION_HDR_LEN),
                                used );
                    break;
                } /* ipcp option type */
              } /* while more ipcp options */
              break;

          case VSNCP_3GPP2I_PCO_DNS_V4_ADDR_ID:
              /*-------------------------------------------------------------
                Check that the length matches that of an IPv4 address
              -------------------------------------------------------------*/
              if( signed_container_len != SIZE_OF_V4_ADDR )
              {
                LOG_MSG_ERROR_1("Invalid DNS PCO length %d",
                                signed_container_len);
                return -1;
              }

              /*-------------------------------------------------------------
                Toss this container if we did not request a DNS v4 address via
                PCO container or if this is a V6 only PDN.
              -------------------------------------------------------------*/
              if( ( lwant_ptr->pco.dns_is_requested == FALSE ) ||
                 !( lwant_ptr->pco.dns_v4_addr_req & VSNCP_3GPP2_DNS_V4_REQ_CONTAINER ) ||
                  ( lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6 ) )
              {
                LOG_MSG_ERROR_0("Invalid DNS v4 PCO request parameters. Tossing "
                                "DNS v4 Response container. ");
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }

              PULLUP_BYTES( pkt_ptr_ptr,
                            &dns_v4_addr,
                            sizeof( uint32 ),
                            used );

              /*------------------------------------------------------------- 
                Ignore the DNS v4 address if it contains 0.0.0.0 adderss
              -------------------------------------------------------------*/
              if ( 0 == dns_v4_addr )
              {
                LOG_MSG_INFO2_0("DNS v4 Response contains 0.0.0.0 address. "
                                "Ignoring this address.");
                break;
              }

              /*-------------------------------------------------------------
                Fill in the Primary v4 DNS address. If the primary address
                is already filled in, then store the address as the Secondary
                v4 DNS address. If both primary and secondary v4 DNS addresses
                have been obtained, ignore all other addresses that arrive
                through PCO DNS v4 Response containers.
               
                Notes:
                 1) Network will always send a container with the Primary
                    DNS v4 address BEFORE sending a container with the
                    Secondary DNS v4 address.
               
                 2) If requested, the network could send DNS v4 addresses in
                    both an IPCP response in the PCO and in a DNS v4 Response
                    container. In this case, the DNS v4 addresses obtained
                    through the IPCP response will be overwritten by the
                    addresses in the DNS v4 Response container.               
              -------------------------------------------------------------*/
              if( !is_dns_v4_primary_addr_obtained ) 
              {
                LOG_MSG_INFO2_0("Received DNS v4 primary address in PCO");
                pco_vals_ptr->dns_v4_primary.ps_s_addr = dns_v4_addr;
                is_dns_v4_primary_addr_obtained = TRUE;
              }
              else if( !is_dns_v4_secondary_addr_obtained )
              {
                LOG_MSG_INFO2_0("Received DNS v4 secondary address in PCO");
                pco_vals_ptr->dns_v4_secondary.ps_s_addr = dns_v4_addr;
                is_dns_v4_secondary_addr_obtained = TRUE;
              }
              else
              {
                LOG_MSG_ERROR_0("Already received primary and secondary DNS v4 "
                                "addresses. Ignoring any subsequent addresses.");
              }
              break;
#ifdef FEATURE_DS_MTU_PCO_VSNCP
             case VSNCP_3GPP2I_PCO_MTU_ID:              
                /*-------------------------------------------------------------
                  We now support MTU in unsolicited PCO. No need to check
                  if it is requested or not                  		
                -------------------------------------------------------------*/

                /*------------------------------------------------------------
                  Check the length of the option, ignore the option if the 
                  length is incorrect
                --------------------------------------------------------------*/
                if(signed_container_len != VSNCP_3GPP2I_PCO_MTU_LEN)
                {
                  LOG_MSG_ERROR_1("Invalid MTU PCO length %d, MTU ignored!",
                                   signed_container_len);
                  TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used);
                  break;              
                }
                /*-------------------------------------------------------------
                  Pullup the MTU value 
                -------------------------------------------------------------*/
                PULLUP_BYTES( pkt_ptr_ptr,
                              &(mtu),
                              sizeof( uint16 ),
                              used );
                mtu= ps_ntohs(mtu);
                LOG_MSG_INFO2_1("VSNCP received mtu value: %d", mtu);

                /*-------------------------------------------------------------
                  Stored into PCO if it is not 0
                -------------------------------------------------------------*/               
                if ( mtu !=0 ) 
                {
                  pco_vals_ptr->mtu = mtu;
                } 
                else
                {
                  LOG_MSG_ERROR_0("VSNCP received mtu values 0!");
                }
                break;
#endif /* FEATURE_DS_MTU_PCO_VSNCP */

            case VSNCP_3GPP2I_PCO_PCSCF_ADDR_ID:
              /*-------------------------------------------------------------
                This might be a v4-mapped-v6 address or a v6 address.
                The length must match that of an IPv6 address in both cases.
              -------------------------------------------------------------*/
              if( signed_container_len != SIZE_OF_V6_ADDR )
              {
                LOG_MSG_ERROR_1("Invalid P-CSCF PCO length %d",
                                signed_container_len);
                return -1;
              }

              /*-------------------------------------------------------------
                Throw away this container if we never requested P-CSCF addr
              -------------------------------------------------------------*/
              if( lwant_ptr->pco.p_cscf_is_requested == FALSE )
              {
                LOG_MSG_ERROR_0("Tossing P-CSCF PCO!");
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }

              /*-------------------------------------------------------------
                Allocate heap memory and pullup the v6 P-CSCF address into it
              -------------------------------------------------------------*/
              v6_addr_ptr =
                vsncp_3gpp2i_mem_alloc( sizeof(vsncp_3gpp2i_pco_v6_addr) );
              if( v6_addr_ptr == NULL )
              {
                LOG_MSG_ERROR_0("Out of heap mem, tossing v6 P-CSCF!");
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }
              PULLUP_BYTES( pkt_ptr_ptr,
                            &(v6_addr_ptr->addr),
                            sizeof( struct ps_in6_addr ),
                            used );

              /*-------------------------------------------------------------
                If this is a v4-mapped-v6 address, add to the v4 list.
                else this is a v6 address, add to the v6 list
              -------------------------------------------------------------*/
              if( PS_IN6_IS_ADDR_V4MAPPED( &v6_addr_ptr->addr ) )
              {
                /*-----------------------------------------------------------
                  If this is an IPv6-only PDN, drop this container!
                -----------------------------------------------------------*/
                if( lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6 )
                {
                  LOG_MSG_ERROR_0("v4 P-CSCF for v6 PDN! Ignoring!");
                  vsncp_3gpp2i_mem_free( v6_addr_ptr );
                  break;
                }

                /*-----------------------------------------------------------
                  Allocate heap memory to hold the v4 P-CSCF address
                -----------------------------------------------------------*/
                v4_addr_ptr =
                  vsncp_3gpp2i_mem_alloc( sizeof(vsncp_3gpp2i_pco_v4_addr) );
                if( v4_addr_ptr == NULL )
                {
                  LOG_MSG_ERROR_0("Out of heap mem, Ignoring v4 P-CSCF!");
                  vsncp_3gpp2i_mem_free( v6_addr_ptr );
                  break;
                }

                /*-----------------------------------------------------------
                  Extract the v4 address into allocated heap memory and add
                  it to the IPv4 P-CSCF address list
                -----------------------------------------------------------*/
                v4_addr_ptr->addr =
                  PS_IN6_GET_V4_FROM_V4_MAPPED_V6( &v6_addr_ptr->addr );
                v4_addr_ptr->next = pco_vals_ptr->p_cscf_v4_head_ptr;
                pco_vals_ptr->p_cscf_v4_head_ptr = v4_addr_ptr;
                v4_addr_ptr = NULL;

                /*-----------------------------------------------------------
                  Done with heap memory originally allocate for v6; free it
                -----------------------------------------------------------*/
                vsncp_3gpp2i_mem_free( v6_addr_ptr );
              }
              else
              {
                /*-----------------------------------------------------------
                  This is a regular IPv6 P-CSCF address - if this is an
                  IPv4-only PDN, drop this container.
                -----------------------------------------------------------*/
                if( lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4 )
                {
                  LOG_MSG_ERROR_0("v6 P-CSCF for v4 PDN! Ignoring!");
                  vsncp_3gpp2i_mem_free( v6_addr_ptr );
                  break;
                }

                /*-----------------------------------------------------------
                  Add to the IPv6 P-CSCF address list
                -----------------------------------------------------------*/
                v6_addr_ptr->next = pco_vals_ptr->p_cscf_v6_head_ptr;
                pco_vals_ptr->p_cscf_v6_head_ptr = v6_addr_ptr;
                v6_addr_ptr = NULL;
              }
              break;
            case VSNCP_3GPP2I_PCO_V4_PCSCF_ADDR_ID:
              {
                if( signed_container_len != SIZE_OF_V4_ADDR )
                {
                  LOG_MSG_ERROR_1("Invalid V4 P-CSCF PCO length %d",
                                  signed_container_len);
                  return -1;
                }

                /*-------------------------------------------------------------
                  Throw away this container if we never requested P-CSCF addr
                -------------------------------------------------------------*/
                if( lwant_ptr->pco.p_cscf_is_requested == FALSE )
                {
                  LOG_MSG_ERROR_0("Tossing V4 P-CSCF PCO!");
                  TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                  break;
                }
                /*-----------------------------------------------------------
                  If this is an IPv6-only PDN, drop this container!
                -----------------------------------------------------------*/
                if( lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6 )
                {
                  LOG_MSG_ERROR_0("v4 P-CSCF for v6 PDN! Ignoring!");
                  TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                  break;
                }

                /*-----------------------------------------------------------
                  Allocate heap memory to hold the v4 P-CSCF address
                -----------------------------------------------------------*/
                v4_addr_ptr =
                  vsncp_3gpp2i_mem_alloc( sizeof(vsncp_3gpp2i_pco_v4_addr) );
                if( v4_addr_ptr == NULL )
                {
                  LOG_MSG_ERROR_0("Out of heap mem, Ignoring v4 P-CSCF!");
                  TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                  break;
                }

                /*-----------------------------------------------------------
                  Extract the v4 address into allocated heap memory and add
                  it to the IPv4 P-CSCF address list
                -----------------------------------------------------------*/
                PULLUP_BYTES( pkt_ptr_ptr,
                              &(v4_addr_ptr->addr),
                              sizeof( struct ps_in_addr ),
                              used );
                v4_addr_ptr->next = pco_vals_ptr->p_cscf_v4_head_ptr;
                pco_vals_ptr->p_cscf_v4_head_ptr = v4_addr_ptr;
                v4_addr_ptr = NULL;

              }
              break;
            case VSNCP_3GPP2I_PCO_DNS_V6_ADDR_ID:
              /*-------------------------------------------------------------
                Check that the length matches that of an IPv6 address
              -------------------------------------------------------------*/
              if( signed_container_len != SIZE_OF_V6_ADDR )
              {
                LOG_MSG_ERROR_1("Invalid DNS PCO length %d",
                                signed_container_len);
                return -1;
              }

              /*-------------------------------------------------------------
                If we didn't request a DNS address or this is an IPv4-only
                PDN, drop this container.
              -------------------------------------------------------------*/
              if( ( lwant_ptr->pco.dns_is_requested == FALSE ) ||
                  ( lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4 ) )
              {
                LOG_MSG_ERROR_0("Tossing v6 DNS PCO!");
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }

              /*-------------------------------------------------------------
                Allocate heap memory and pullup the v6 DNS address into it
              -------------------------------------------------------------*/
              v6_addr_ptr =
                vsncp_3gpp2i_mem_alloc( sizeof(vsncp_3gpp2i_pco_v6_addr) );
              if( v6_addr_ptr == NULL )
              {
                LOG_MSG_ERROR_0("Out of heap mem, tossing v6 DNS!");
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }
              PULLUP_BYTES( pkt_ptr_ptr,
                            &(v6_addr_ptr->addr),
                            sizeof( struct ps_in6_addr ),
                            used );

              /*-------------------------------------------------------------
                If this is a v4-mapped-v6 address, throw the container away
              -------------------------------------------------------------*/
              if( PS_IN6_IS_ADDR_V4MAPPED( &v6_addr_ptr->addr ) )
              {
                LOG_MSG_ERROR_0("Rx v4 address in v6 DNS PCO! Ignoring");
                vsncp_3gpp2i_mem_free( v6_addr_ptr );
                break;
              }

              /*-------------------------------------------------------------
                Add to the IPv6 DNS address list
              -------------------------------------------------------------*/
              v6_addr_ptr->next = pco_vals_ptr->dns_v6_head_ptr;
              pco_vals_ptr->dns_v6_head_ptr = v6_addr_ptr;
              v6_addr_ptr = NULL;
              break;

            case VSNCP_3GPP2I_PCO_SELECTED_BCM_ID:
              /*-------------------------------------------------------------
                Check  the length of BCM
              -------------------------------------------------------------*/
              if( signed_container_len != 
                    VSNCP_3GPP2I_PCO_SELECTED_BCM_LEN )
              {
                LOG_MSG_ERROR_1("Invalid selected bcm PCO length %d",
                                signed_container_len);
                return -1;
              }

              /*-------------------------------------------------------------
                Throw away this container if we never request BCM
              -------------------------------------------------------------*/
              if( lwant_ptr->pco.bcm_is_supported == FALSE )
              {
                LOG_MSG_ERROR_0("Tossing BCM PCO!");
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
                break;
              }

              /*-------------------------------------------------------------
                Store the BCM value if we requested it 
              -------------------------------------------------------------*/
              PULLUP_BYTES( pkt_ptr_ptr,
                            &(pco_vals_ptr->selected_bcm),
                            sizeof( uint8 ),
                            used );
              break;

            case VSNCP_3GPP2I_PCO_IP_ADDR_ALLOC_VIA_NAS_ID:
              /*-------------------------------------------------------------
                Check that the length matches the ip-via-NAS length
              -------------------------------------------------------------*/
              if( signed_container_len !=
                    VSNCP_3GPP2I_PCO_IP_ADDR_ALLOC_VIA_NAS_LEN )
              {
                LOG_MSG_ERROR_1("Invalid ip4_via_nas PCO len %d",
                                signed_container_len);
                return -1;
              }
              else
              {
                LOG_MSG_INFO2_1("Ignoring IPv4 via NAS PCO 0x%x",
                                container_id);
                TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
              }
              break;

            default:
              LOG_MSG_ERROR_1("Unrecognized container id 0x%x", container_id);
              TOSS_BYTES( pkt_ptr_ptr, signed_container_len, used );
              break;
          } /* container id */
          }
          signed_len = used;
        } /* while more containers */
        option_result = RECOGNIZED; /* in a C-Ack */
      }
      break;

    case VSNCP_3GPP2I_ATTACH_TYPE:
      /*---------------------------------------------------------------------
        Expected in a C-Ack
      ---------------------------------------------------------------------*/
      if( request == FALSE )
      {
        PULLUP_BYTES( pkt_ptr_ptr,
                      &(lwork_ptr->attach_type),
                      sizeof( uint8 ),
                      used );
        option_result = RECOGNIZED;
      }
      break;

    case VSNCP_3GPP2I_DEFAULT_ROUTER:
      /*---------------------------------------------------------------------
        Expected only in a C-Ack
      ---------------------------------------------------------------------*/
      if( request == FALSE )
      {
        PULLUP_BYTES( pkt_ptr_ptr,
                      &(lwork_ptr->default_rtr_ip4.ps_s_addr),
                      sizeof(struct ps_in_addr),
                      used );
        option_result = RECOGNIZED;
      }
      break;

    case VSNCP_3GPP2I_ADDR_ALLOC_CAUSE:
     /*---------------------------------------------------------------------
       Expected in a C-Ack
     ---------------------------------------------------------------------*/
     if( request == FALSE )
     {
       PULLUP_BYTES( pkt_ptr_ptr,
                     &(lwork_ptr->addr_alloc_cause),
                     sizeof( uint8 ),
                     used );
       option_result = RECOGNIZED;
     }
     break;

    default:
      LOG_MSG_ERROR_1("3gpp2-VSNCP saw unknown option 0x%x", ohp->type);
      break;
  } /* switch(option) */

  if( used > 0 )
  {
    /*-----------------------------------------------------------------------
      toss extra bytes in option
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_1("Tossing extra bytes in option 0x%x", ohp->type);
    if( dsm_pullup( pkt_ptr_ptr, NULL, (uint16)used ) != (uint16)used )
    {
      LOG_MSG_ERROR_0("Unable to toss bytes!");
      return -1;
    }
  }

  return option_result;
} /* vsncp_3gpp2i_check() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_REJECT()

DESCRIPTION
  Process an option seen in the incoming VSNCP Configure-Reject packet.
  If this is the VSNCP_3GPP2I_ERROR_CODE option, the VSNCP instance will
  be terminated and -1 returned to indicate that the whole packet should be
  thrown away and no further processing is needed. All other options are
  treated as benign.

PARAMETERS
  (IN) fsm_ptr : Pointer to the FSM structure
  (IN) ohp     : Pointer to the option header
  (IN) pkt_ptr : Pointer to pointer to the DSM item

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int vsncp_3gpp2i_reject
(
  ppp_fsm_type      * fsm_ptr,
  struct option_hdr * ohp,
  dsm_item_type     * pkt_ptr
)
{
  uint8 error_code;
  int retval;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1("vsncp_3gpp2i_reject() - option 0x%x", ohp->type);

  switch( ohp->type )
  {
    case VSNCP_3GPP2I_ERROR_CODE:
      /*---------------------------------------------------------------------
        Error code option seen. Terminate the 3GPP2-VSNCP instance.
      ---------------------------------------------------------------------*/
      if( dsm_extract( pkt_ptr, 0, &error_code, sizeof(uint8) )
            != sizeof(uint8) )
      {
        LOG_MSG_ERROR_0("Unable to extract error code");
        retval = -1;
        break;
      }

      if( error_code > VSNCP_3GPP2I_ERROR_MAX  )
      {
        LOG_MSG_ERROR_1("Invalid error code %d ", error_code);
        fsm_ptr->fail_reason = PPP_FAIL_REASON_VSNCP_FAILURE;        
      }
      else
      {
        LOG_MSG_INFO1_1("Rxed Conf-Rej - error code = 0x%x", error_code);
        fsm_ptr->fail_reason = (ppp_fail_reason_e_type)(
                               (uint8)PPP_FAIL_REASON_VSNCP_3GPP2I_ERROR_MIN + 
                               error_code );
      }
      LOG_MSG_INFO1_1("Terminating 3GPP2-VSNCP instance. fail reason=%d",
                      fsm_ptr->fail_reason);
      if( ppp_ncp_stop( (ppp_ncp_handle_type)fsm_ptr->pdv ) != 0 )
      {
        LOG_MSG_ERROR_0("Unable to terminate 3gpp2 VSNCP instance");
      }

      /*---------------------------------------------------------------------
        Return -1 so caller will throw away this packet
      ---------------------------------------------------------------------*/
      retval = -1;
      break;

    default:
      /*---------------------------------------------------------------------
        All options other than VSNCP_3GPP2I_ERROR_CODE are treated as benign
      ---------------------------------------------------------------------*/
      retval = 0;
      break;
  }

  return retval;
} /* vsncp_3gpp2i_reject() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_TIMER_CB()

DESCRIPTION
  This is the handler function for the VSNCP fsm timer - used to retry
  VSNCP Configure-Requests and Terminate-Requests (depending on FSM state)
  If the FSM has given up, this will lead to termination of the VSNCP
  instance

PARAMETERS
  (IN) fsm_ptr : Pointer to the FSM structure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void vsncp_3gpp2i_timer_cb
(
  void * arg_ptr
)
{
  ppp_fsm_type * fsm_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Validate the fsm_ptr
  -------------------------------------------------------------------------*/
  fsm_ptr = (ppp_fsm_type *)arg_ptr;
  if( ( fsm_ptr == NULL ) ||
      ( VSNCP_3GPP2I_FSM_IS_VALID( fsm_ptr) == FALSE ) )
  {
    LOG_MSG_ERROR_1("Invalid fsm 0x%x in vsncp timer cb", fsm_ptr);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO1_2("dev %d 3gpp2 VSNCP Timer Expired state =%d",
                  fsm_ptr->device, fsm_ptr->state);

  /*-------------------------------------------------------------------------
    If the FSM has given up, terminate this VSNCP instance
  -------------------------------------------------------------------------*/
  if( fsm_timeout( fsm_ptr ) == FALSE )
  {
    /*-----------------------------------------------------------------------
      For normal network initiated termination, PPP has to wait for 1 timeout
      before cleanup. fsm_timeout() == false in fsmCLOSED & fsmSTOPPED
      indicates that fsm has cleaned up the vsncp. Thus there is no reason
      to issue ppp_ncp_stop and the fail_reason should be none for this case.
    -----------------------------------------------------------------------*/
    if( fsm_ptr->state != fsmCLOSED && fsm_ptr->state != fsmSTOPPED )
    {
      LOG_MSG_ERROR_1("3gpp2-VSNCP on dev %d failed to establish!",
                      fsm_ptr->device);
      /*-----------------------------------------------------------------------
        Stop this NCP.  This will stop LCP if there are no other NCPs
      -----------------------------------------------------------------------*/
      fsm_ptr->fail_reason = PPP_FAIL_REASON_VSNCP_TIMEOUT;
      if( ppp_ncp_stop( (ppp_ncp_handle_type)fsm_ptr->pdv ) != 0 )
      {
        LOG_MSG_ERROR_0("Unable to terminate 3gpp2 VSNCP instance");
      }
    }
  }
} /* vsncp_3gpp2i_timer_cb() */

/*===========================================================================
FUNCTION VSNCP_3GPP2I_CONFIG_CHECK()

DESCRIPTION
  This function is used to validate the configuration passed by the PPP
  VSNCP client.

PARAMETERS
  (IN) config_ptr : Pointer to configuration structure

RETURN VALUE
   0 : Passed configuration is valid
  -1 : Passed configuration is invalid

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2i_config_check
(
  ppp_vsncp_3gpp2_config_type * config_ptr
)
{
  vsncp_3gpp2_local_opts_val_type * option_vals_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( config_ptr == NULL )
  {
    LOG_MSG_ERROR_0("Invalid args!");
    return -1;
  }

  option_vals_ptr = &(config_ptr->option_vals);

  /*-------------------------------------------------------------------------
    Check that the pdn-id specified is in range
  -------------------------------------------------------------------------*/
  if( option_vals_ptr->pdn_id > VSNCP_3GPP2I_PDN_ID_MAX )
  {
    LOG_MSG_ERROR_1("Invalid PDN-ID 0x%x", option_vals_ptr->pdn_id);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check that the APN length specified is within range
  -------------------------------------------------------------------------*/
  if( option_vals_ptr->apn_len > VSNCP_3GPP2_APN_MAX_VAL_LEN )
  {
    LOG_MSG_ERROR_1("Invalid apn length 0x%x", option_vals_ptr->apn_len);
    return -1;
  }

  /*-------------------------------------------------------------------------
    ip_ver_capability is used to indicate the capability of the UE stack
    (i.e. whether only IPv4 or IPv6 is supported or both). By default,
    it will be overwritten as VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6.
    But if the NV disable_iptype_overwrite is set to
    TRUE, ip_ver_capability will be set according to what the profile
    specifies. 
  -------------------------------------------------------------------------*/
    if(
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
        (option_vals_ptr->ip_ver_capability != VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6) &&
#endif
        (option_vals_ptr->ip_ver_capability != VSNCP_3GPP2_PDN_TYPE_IPV4) &&
        (option_vals_ptr->ip_ver_capability != VSNCP_3GPP2_PDN_TYPE_IPV6) )
  {
    LOG_MSG_ERROR_1("Invalid ip_ver_capability 0x%x",
                    option_vals_ptr->ip_ver_capability);
    return -1;
  }

  /*-------------------------------------------------------------------------
    req_addr_type is used internally within the UE to determine which
    address family is relevant for this PDN connection. e.g. req_addr_type 
    should be either VSNCP_3GPP2_PDN_TYPE_IPV4 or VSNCP_3GPP2_PDN_TYPE_IPV6 
    or VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6.
  -------------------------------------------------------------------------*/
  if( ( option_vals_ptr->req_addr_type != VSNCP_3GPP2_PDN_TYPE_IPV4 ) &&
      ( option_vals_ptr->req_addr_type != VSNCP_3GPP2_PDN_TYPE_IPV6 ) 
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
       &&
      ( option_vals_ptr->req_addr_type != VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 ) 
#endif
     )
  {
    LOG_MSG_ERROR_1("Invalid req_addr_type 0x%x",
                    option_vals_ptr->req_addr_type);
    return -1;
  }

  if( ( ( (uint8)option_vals_ptr->req_addr_type & (uint8)VSNCP_3GPP2_PDN_TYPE_IPV4 ) == 
          (uint8)VSNCP_3GPP2_PDN_TYPE_IPV4  && 
          config_ptr->IPV4_IFACE_PTR == NULL ) || 
      ( ( (uint8)option_vals_ptr->req_addr_type & (uint8)VSNCP_3GPP2_PDN_TYPE_IPV6 ) == 
          (uint8)VSNCP_3GPP2_PDN_TYPE_IPV6  && 
          config_ptr->IPV6_IFACE_PTR == NULL ) )
  {
    LOG_MSG_ERROR_0("NULL PS Iface ptrs");
    return -1;
  }


  /*-------------------------------------------------------------------------
    Validate attach_type
  -------------------------------------------------------------------------*/
  if( ( option_vals_ptr->attach_type != VSNCP_3GPP2_ATTACH_TYPE_INITIAL ) &&
      ( option_vals_ptr->attach_type != VSNCP_3GPP2_ATTACH_TYPE_HANDOVER ) )
  {
    LOG_MSG_ERROR_1("Invalid attach_type 0x%x", option_vals_ptr->attach_type);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Addresses specified MUST be 0 if attach_type is INITIAL
  -------------------------------------------------------------------------*/
  if( ( option_vals_ptr->attach_type == VSNCP_3GPP2_ATTACH_TYPE_INITIAL ) &&
      ( ( option_vals_ptr->ip4.ps_s_addr !=  0 ) ||
        ( option_vals_ptr->iid != 0 ) ) )
  {
    LOG_MSG_ERROR_0("Non-zero addrs for initial attach");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Addresses specified MUST be non-zero if attach_type is HANDOVER
    TODO: The semantic of attach_type as HANDOVER should be considered,
    especially in light of whether UE can initiate a resync, and whether
    we receive these "option_vals" from the client (MH) in that case.
    However, the check below holds true per specification.
  -------------------------------------------------------------------------*/
  if( ( option_vals_ptr->attach_type == VSNCP_3GPP2_ATTACH_TYPE_HANDOVER ) &&
      ( ( ( option_vals_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4 ) &&
          ( option_vals_ptr->ip4.ps_s_addr == 0) ) ||
        ( ( option_vals_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6 ) &&
          ( option_vals_ptr->iid == 0 ) )  
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
          || ( (option_vals_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 ) && 
          ( option_vals_ptr->ip4.ps_s_addr == 0) && 
          ( option_vals_ptr->iid == 0 ) ) 
#endif
        ) )
  {
    LOG_MSG_ERROR_0("Incorrect 3gpp2 VSNCP configuration");
    return -1;
  }

  /*-------------------------------------------------------------------------
    For ip_ver_capability v4 or v4-v6:
      1. The ip_addr_via_nas boolean MUST be set to TRUE
      2. In case of INITIAL_ATTACH, the default_rtr_ip4 MUST be 0.
  -------------------------------------------------------------------------*/
  if( ( option_vals_ptr->ip_ver_capability == VSNCP_3GPP2_PDN_TYPE_IPV4 )||
      ( option_vals_ptr->ip_ver_capability ==
          VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 ) )
  {
    if( option_vals_ptr->pco.ip_addr_via_nas != TRUE )
    {
      LOG_MSG_ERROR_0("ip4_via_nas set to FALSE!");
      return -1;
    }

    if( ( option_vals_ptr->attach_type ==
            VSNCP_3GPP2_ATTACH_TYPE_INITIAL ) &&
        ( option_vals_ptr->default_rtr_ip4.ps_s_addr != 0 ) )
    {
      LOG_MSG_ERROR_0("Non-zero default rtr addr in init attach!");
      return -1;
    }
  }

#ifdef FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH
  /*-------------------------------------------------------------------------
    For PDN level authentication, if a PDN level auth protocol is specified:
      1. A valid user id must be configured
      2. A valid password must be configured
  -------------------------------------------------------------------------*/
  if ( PPP_AUTH_PAP == option_vals_ptr->pco.pdn_level_auth.protocol ||
       PPP_AUTH_CHAP == option_vals_ptr->pco.pdn_level_auth.protocol )
  {
    if( ( NULL == option_vals_ptr->pco.pdn_level_auth.user_id ) ||
        ( PPP_MAX_USER_ID_LEN < 
          option_vals_ptr->pco.pdn_level_auth.user_id_len ) ||
        ( option_vals_ptr->pco.pdn_level_auth.user_id_len == 0 ) )
    {
      LOG_MSG_ERROR_0( "Invalid PDN level auth user id" );
      return -1;
    }

    if( ( PPP_MAX_PASSWD_LEN < option_vals_ptr->pco.pdn_level_auth.password_len ) ||
        ( ( PPP_AUTH_CHAP == option_vals_ptr->pco.pdn_level_auth.protocol ) &&
          ( ( NULL == option_vals_ptr->pco.pdn_level_auth.password ) ||
            ( option_vals_ptr->pco.pdn_level_auth.password_len == 0 ) ) )
      )
    {
      LOG_MSG_ERROR_0( "Invalid PDN level auth password" );
      return -1;
    }
  }
#endif /* FEATURE_DATA_EHRPD_PDN_LEVEL_AUTH */

  return 0;
} /* vsncp_3gpp2i_config_check() */



/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION VSNCP_3GPP2_SET_CONSTANT()

DESCRIPTION
  This function is used to cache the NV settings for general VSNCP params
  (retries etc.) so they don't need to be read from NV every time an
  NCP needs to be set up. The global structure vsncp_3gpp2_nv_info is used
  to hold this information.

PARAMETERS
  (IN) constant: The VSNCP NV parameter to be updated
  (IN) value: Value to be written to the constant

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_set_constant
(
  ppp_init_consts_enum_type constant,
  uint32                    value
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(constant)
  {
    case VSNCP_3GPP2_REQ_TRY:
      vsncp_3gpp2_nv_info.req_try = (uint16)value;
      break;

    case VSNCP_3GPP2_TERM_TRY:
      vsncp_3gpp2_nv_info.term_try = (uint16)value;
      break;

    case VSNCP_3GPP2_VSNCP_ACK_TIMEOUT:
      vsncp_3gpp2_nv_info.ack_timeout = (uint32)value;
      break;

    case VSNCP_3GPP2_TERM_TIMEOUT:
      vsncp_3gpp2_nv_info.term_timeout = (uint32)value;
      break;

    default:
      LOG_MSG_ERROR_1("Unknown constant %d", constant);
      break;
  }
} /* vsncp_3gpp2_set_constant() */

/*===========================================================================
FUNCTION VSNCP_3GPP2_GET_CONSTANT()

DESCRIPTION
  This funtion is used to return the general VSNCP related constant values,
  like number of retries, timeout values, etc.

PARAMETERS
  (IN) constant: The VSNCP NV parameter whose value is to be returned.
  (OUT) value: Storage for the constant.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_get_constant
(
  ppp_init_consts_enum_type   constant,
  uint32                    * value
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(constant)
  {
    case VSNCP_3GPP2_REQ_TRY:
      *value = vsncp_3gpp2_nv_info.req_try ;
      break;

    case VSNCP_3GPP2_TERM_TRY:
      *value = vsncp_3gpp2_nv_info.term_try;
      break;

    case VSNCP_3GPP2_VSNCP_ACK_TIMEOUT:
      *value = vsncp_3gpp2_nv_info.ack_timeout;
      break;

    case VSNCP_3GPP2_TERM_TIMEOUT:
      *value = vsncp_3gpp2_nv_info.term_timeout;
      break;

    default:
      LOG_MSG_ERROR_1("Unknown constant %d", constant);
      break;
  }
} /* vsncp_3gpp2_get_constant() */

#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION VSNCP_3GPP2_GET_FSM_CONSTANT()

DESCRIPTION
  This funtion is used to return the VSNCP instance related fsm constant
  values, like fsm state, number of retries, timeout values, etc.

PARAMETERS
  (IN) device: PPP device on which the VSNCP connection is established.
  (IN) constant: The VSNCP NV parameter whose value is to be returned.
  (IN) pdn id: PDN ID associated with the VSNCP instance.
  (OUT) value: Storage for the constant.

RETURN VALUE
   0: success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_get_fsm_constant
(
  ppp_dev_enum_type           device,
  ppp_fsm_type_enum_type      fsm_enum,
  uint8                       pdn_id
)
{
  uint8 index = 0;
  ppp_type * ppp_cb_ptr = NULL;
  ppp_type * ppp_cb_array = NULL;
  ncpi_cb_type * instance_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &(ppp_cb_array[device]);

  /*-------------------------------------------------------------------------
    Iterate through all available VSNCP instances to find corresponding FSM.
  -------------------------------------------------------------------------*/
  for( index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    instance_ptr =
      (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                           PS_MEM_PPP_NCP_CB_TYPE );
    if( ( NCPI_CB_IS_INITED( instance_ptr ) == TRUE )  &&
        ( instance_ptr->ncp == PPP_NCP_VSNCP_3GPP2 ) &&
        ( instance_ptr->cb_u.vsncp_3gpp2.config.option_vals.pdn_id == pdn_id ) )
    {
      LOG_MSG_INFO2_1("VSNCP: Matching NCP handle found: %p", instance_ptr);
      return fsm_getconst( &(instance_ptr->cb_u.vsncp_3gpp2.fsm), fsm_enum );
    }
  } /* for */

  return -1;
} /* vsncp_3gpp2_get_fsm_constant() */
#endif /*FEATURE_STA_PPP*/
/*===========================================================================
FUNCTION VSNCP_3GPP2_POWERUP_INIT()

DESCRIPTION
  This function is used to do power-up initialization for the 3GPP2-VSNCP
  module.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_powerup_init( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*-------------------------------------------------------------------------
      Memset the structure to hold NV values and set global variable to
      indicate we are now initialized
    -------------------------------------------------------------------------*/
    memset( &vsncp_3gpp2_nv_info, 0, sizeof( vsncp_3gpp2i_nv_info_type ) );
} /* vsncp_3gpp2_powerup_init() */

/*===========================================================================
FUNCTION VSNCP_3GPP2_ALLOC()

DESCRIPTION
  This function is used to create a 3GPP2-VSNCP instance and return a
  handle to it.

PARAMETERS
  (IN) device: PPP device
  (IN) vsncp_config_ptr: Pointer to the 3gpp2 vsncp configuration

RETURN VALUE
  ppp_ncp_handle_type: Handle to the allocated NCP instance on success
  NULL on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ppp_ncp_handle_type vsncp_3gpp2_alloc
(
  ppp_dev_enum_type             device,
  ppp_vsncp_3gpp2_config_type * vsncp_config_ptr
)
{
  ncpi_cb_type * ncp_cb_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check configuration
  -------------------------------------------------------------------------*/
  if( vsncp_3gpp2i_config_check( vsncp_config_ptr ) != 0 )
  {
    LOG_MSG_ERROR_0("Invalid 3gpp2 vsncp config");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Allocate memory to hold the control block
  -------------------------------------------------------------------------*/
  ncp_cb_ptr = (ncpi_cb_type *)ps_mem_get_buf( PS_MEM_PPP_NCP_CB_TYPE );
  if( ncp_cb_ptr == NULL )
  {
    LOG_MSG_ERROR_0("3gpp2-vsncp: Mem alloc error");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Per X.P0057, the address allocation cause must be set to 0
    since UE initiated.
  -------------------------------------------------------------------------*/
  vsncp_config_ptr->option_vals.addr_alloc_cause =
    (uint8)VSNCP_3GPP2_ADDR_ALLOC_CAUSE_NULL;

  /*-------------------------------------------------------------------------
    Initialize control block contents - set the device and protocol,
    copy the client configuration, setup the cleanup function and self ptr
  -------------------------------------------------------------------------*/
  memset( ncp_cb_ptr, 0, sizeof(ncpi_cb_type) );
  ncp_cb_ptr->device = device;
  ncp_cb_ptr->ncp = PPP_NCP_VSNCP_3GPP2;
  memscpy( &(ncp_cb_ptr->cb_u.vsncp_3gpp2.config),
           sizeof( ppp_vsncp_3gpp2_config_type ) ,
           vsncp_config_ptr,
           sizeof( ppp_vsncp_3gpp2_config_type ) );
  ncp_cb_ptr->abort_f_ptr = vsncp_3gpp2i_abort;
  ncp_cb_ptr->this_cb_ptr = ncp_cb_ptr;

  return ncp_cb_ptr;
} /* vsncp_3gpp2_alloc() */

/*===========================================================================
FUNCTION VSNCP_3GPP2_INIT()

DESCRIPTION
  This function is used to initialize the 3GPP2-VSNCP control block

PARAMETERS
  (IN) ncp_cb_ptr: Pointer to the NCP control block

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_init
(
  ncpi_cb_type * ncp_cb_ptr
)
{
  ppp_fsm_type           * fsm_ptr = NULL;
  vsncp_3gpp2i_type  * vsncp_cb_ptr = NULL;
  vsncp_3gpp2i_nv_info_type  nv_constants;
  vsncp_3gpp2_local_opts_val_type * lwork_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate the NCP control block
  -------------------------------------------------------------------------*/
  if( NCPI_CB_IS_VALID( ncp_cb_ptr ) == FALSE )
  {
    DATA_ERR_FATAL("Invalid handle for vsncp_3gpp2 init");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Obtain a handle to the 3gpp2-vsncp control block
  -------------------------------------------------------------------------*/
  vsncp_cb_ptr = &(ncp_cb_ptr->cb_u.vsncp_3gpp2);
  LOG_MSG_INFO2_1("vsncp_3gpp2_init(), pdn_id 0x%x",
                  vsncp_cb_ptr->config.option_vals.pdn_id);

  fsm_ptr = &(vsncp_cb_ptr->fsm);
  memset( fsm_ptr, 0, sizeof( ppp_fsm_type ) );

  /*-------------------------------------------------------------------------
    Allocate the FSM timer - this handles the C-Req (and T-Req) retries
  -------------------------------------------------------------------------*/
  fsm_ptr->timer = ps_timer_alloc( vsncp_3gpp2i_timer_cb, fsm_ptr );
  if( fsm_ptr->timer == PS_TIMER_INVALID_HANDLE )
  {
    DATA_ERR_FATAL("Unable to start a timer for 3gpp2 VSNCP");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Copy all the default constants to vsncp_3gpp2i_constants. The ones
    available in NV will be applied next.
  -------------------------------------------------------------------------*/
  memscpy( &vsncp_3gpp2i_constants,
           sizeof(vsncp_3gpp2i_constants),
           &vsncp_3gpp2i_default_constants,
           sizeof( vsncp_3gpp2i_default_constants ) );

  memscpy(&vsncp_3gpp2i_cb_device_constants,
          sizeof(vsncp_3gpp2i_cb_device_constants),
          &pppi_fsm_cb_default_constants,
          sizeof(pppi_fsm_cb_default_constants));

  /*-------------------------------------------------------------------------
    Obtain the protocol constant values as read from NV during powerup and
    apply them to the default values contained in vsncp_3gpp2i_constants.
    If the values configured in NV are zero, they are not used and the
    default values are used, i.e. values from NV are only applied
    if they are non-zero.
  -------------------------------------------------------------------------*/
  memscpy( &nv_constants,
           sizeof(vsncp_3gpp2i_nv_info_type), 
           &vsncp_3gpp2_nv_info,
           sizeof(vsncp_3gpp2i_nv_info_type) );

  if( nv_constants.req_try > 0)
  {
    vsncp_3gpp2i_constants.try_configure =
             (uint8)nv_constants.req_try;
  }

  if( nv_constants.term_try > 0)
  {
    vsncp_3gpp2i_constants.try_terminate =
            (uint8)nv_constants.term_try;
  }

  if( nv_constants.ack_timeout > 0)
  {
    vsncp_3gpp2i_constants.ack_timeout =
      (uint16)nv_constants.ack_timeout;
  }

  if( nv_constants.term_timeout > 0)
  {
    vsncp_3gpp2i_constants.term_timeout =
      (uint16)nv_constants.term_timeout;
  }

  /*-------------------------------------------------------------------------
    Initialize fsm variables
  -------------------------------------------------------------------------*/
  fsm_ptr->pdc              = &vsncp_3gpp2i_constants;
  fsm_ptr->cbdc             = &vsncp_3gpp2i_cb_device_constants;
  fsm_ptr->pdv              = ncp_cb_ptr;
  fsm_ptr->mode             = PPP_CP_MODE_ACTIVE;
  fsm_ptr->device           = ncp_cb_ptr->device;
  /*------------------------------------------------------------------------- 
    For VSNCP Dual IP or single IP case, rx_iface_ptr i.e. IPV4_IFACE_PTR 
    will be used as v4 iface and the second_rx_iface_ptr i.e. IPV6_IFACE_PTR
    will be used as a v6 iface ptr.
  ------------------------------------------------------------------------*/
  fsm_ptr->IPV4_IFACE_PTR   = vsncp_cb_ptr->config.IPV4_IFACE_PTR;
  fsm_ptr->IPV6_IFACE_PTR   = vsncp_cb_ptr->config.IPV6_IFACE_PTR;
  fsm_ptr->iface_input      = (ppp_iface_input_f_type) ps_iface_input;
  fsm_ptr->timer_alloc_fcn  = (timer_alloc_f_type)ps_timer_alloc;
  fsm_ptr->timer_start_fcn  = (timer_start_f_type)ps_timer_start;
  fsm_ptr->timer_cancel_fcn = (timer_cancel_f_type)ps_timer_cancel;

  /*-------------------------------------------------------------------------
    Set up the option masks. Per X.P0057:
    1. There are a set of options we are REQUIRED to propose
       (these are the local.want)
    2. We don't send any C-Naks (so remote.want is empty)
    3. We accept all incoming options, (and ignore the unrecognized ones)
       (so remote.will is full)
    4. We don't expect to receive any incoming C-Naks (so local.will is
       irrelevant, empty)
  -------------------------------------------------------------------------*/
  fsm_ptr->local.want  = vsncp_3gpp2i_propose;

  /*-------------------------------------------------------------------------
    Per X.P0057 if the PDN type is IPv4 or IPv4v6 the IPv4 default router
    address config option must be included.
  -------------------------------------------------------------------------*/
  if( ( vsncp_cb_ptr->config.option_vals.ip_ver_capability ==
          VSNCP_3GPP2_PDN_TYPE_IPV4 ) ||
      ( vsncp_cb_ptr->config.option_vals.ip_ver_capability ==
          VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 ) )
  {
    fsm_ptr->local.want |= (uint16)VSNCP_3GPP2I_N_DEFAULT_ROUTER;
  }

  fsm_ptr->remote.want = 0;      /* no outgoing C-Naks              */
  fsm_ptr->remote.will = 0xffff; /* we accept all options from peer */
  fsm_ptr->local.will  = 0;      /* we ignore incoming C-Naks       */

  /*-------------------------------------------------------------------------
    Set the fsm protocol dependant value (pdv) pointers appropriately.
    - local.want_pdv is set to the configuration values received from the MH
    - remote.want_pdv is set to NULL since we don't send Config-Naks
    - local.work_pdv and remote.work_pdv are set to point to appropriate
      scratch memory in the control block
  -------------------------------------------------------------------------*/
  fsm_ptr->local.want_pdv  = &(vsncp_cb_ptr->config.option_vals);
  fsm_ptr->remote.want_pdv = NULL;
  fsm_ptr->local.work_pdv  = &(vsncp_cb_ptr->local_vals);
  fsm_ptr->remote.work_pdv  = &(vsncp_cb_ptr->remote_vals);

  /*-------------------------------------------------------------------------
    Copy local.want to the local.work (that's where the Config-Req values
    come from). The local.work will be updated once the Config-ack comes in.
    Also, to begin with, the local.work_pdv gets a copy of the
    local.want_pdv; it gets updated as the incoming Config-Ack is processed.
  -------------------------------------------------------------------------*/
  fsm_ptr->local.work = fsm_ptr->local.want;
  fsm_ptr->remote.work = fsm_ptr->remote.want;
  lwork_ptr = (vsncp_3gpp2_local_opts_val_type *)fsm_ptr->local.work_pdv;
  memscpy( lwork_ptr,
           sizeof(vsncp_3gpp2_local_opts_val_type),
           fsm_ptr->local.want_pdv,
           sizeof(vsncp_3gpp2_local_opts_val_type) );

  /*-------------------------------------------------------------------------
    Initialize the FSM
  -------------------------------------------------------------------------*/
  fsm_init( fsm_ptr );
  return 0;
} /* vsncp_3gpp2_init() */

/*===========================================================================
FUNCTION VSNCP_3GPP2_START()

DESCRIPTION
  This function is used to start the 3GPP2-VSNCP instance.

PARAMETERS
  (IN) ncp_cb_ptr: Pointer to the NCP control block

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_start
(
  ncpi_cb_type * ncp_cb_ptr
)
{
  ppp_fsm_type * fsm_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("vsncp_3gpp2_start - handle %p", ncp_cb_ptr);
  if( NCPI_CB_IS_INITED( ncp_cb_ptr ) == FALSE )
  {
    DATA_ERR_FATAL("Invalid handle for vsncp_3gpp2 init");
    return;
  }

  fsm_ptr = &(ncp_cb_ptr->cb_u.vsncp_3gpp2.fsm);
  fsm_up( fsm_ptr );
} /* vsncp_3gpp2_start() */

/*===========================================================================
FUNCTION VSNCP_3GPP2_PROT_REJ

DESCRIPTION
  This function is used to handle an incoming LCP protocol-reject for
  VSNCP protocol. pkt_ptr points to the rejected VSNCP packet
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Code      |  Identifier   |            Length             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    OUI                        |    Data ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

PARAMETERS
  (IN) device: PPP device on which the packet was received
  (IN) pkt_ptr: Pointer to the DSM item containing the rejected packet

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_prot_rej
(
  ppp_dev_enum_type   device,
  dsm_item_type     * pkt_ptr
)
{
  uint8 index = 0;
  ncpi_cb_type * instance_ptr = NULL;
  ppp_fsm_type * tmp_fsm_ptr = NULL;
  uint8 num_active_ncps = 0;
  ppp_type * ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("vsncp_3gpp2_prot_rej()");
  /*-------------------------------------------------------------------------
    Apply prot-rej to all VSNCP FSMs
  -------------------------------------------------------------------------*/
  for( index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    instance_ptr =
      (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                           PS_MEM_PPP_NCP_CB_TYPE );
    if( ( NCPI_CB_IS_INITED( instance_ptr ) == TRUE )  &&
        ( instance_ptr->ncp == PPP_NCP_VSNCP_3GPP2 ) &&
        ( instance_ptr->device == device ) )
    {
      LOG_MSG_INFO2_1("Matching NCP handle found: %p", instance_ptr);
      tmp_fsm_ptr = &(instance_ptr->cb_u.vsncp_3gpp2.fsm);
      /*-------------------------------------------------------------------------
        Count number of active NCPs based on the fsm state i.e.
        is the fsm active or is in the process of being torn down or is down.
      -------------------------------------------------------------------------*/
      if(!( PS_PPP_IS_FSM_INACTIVE( tmp_fsm_ptr->state ) ) )
      {
        num_active_ncps++;
      }
    }
  } /* for */

  /*-------------------------------------------------------------------------
    Locate appropriate FSM
  -------------------------------------------------------------------------*/
  if( num_active_ncps == 1 )
  {
    LOG_MSG_INFO2_1("dev %d, single active NCP, proto_rej on LCP!", device);
    ppp_cb_array = fsm_get_ppp_cb_array();
    tmp_fsm_ptr = &(ppp_cb_array[device].fsm[LCP]);
  }

  /*-------------------------------------------------------------------------
    Update the fail reason for the FSM. In case of last NCP, the LCP FSM's
    fail reason gets updated.
  -------------------------------------------------------------------------*/
  if( tmp_fsm_ptr != NULL )
  {  
    tmp_fsm_ptr->fail_reason = PPP_FAIL_REASON_VSNCP_FAILURE;  
    switch ( tmp_fsm_ptr->state )
    {
      case fsmCLOSING:
        /* TLF */
        tmp_fsm_ptr->pdc->stopping( tmp_fsm_ptr );
        tmp_fsm_ptr->state = fsmCLOSED;
        break;
  
      case fsmSTOPPING:
      case fsmREQ_Sent:
      case fsmACK_Rcvd:
      case fsmACK_Sent:
        /* TLF */
        tmp_fsm_ptr->pdc->stopping( tmp_fsm_ptr );
        tmp_fsm_ptr->state = fsmSTOPPED;
        break;
  
      case fsmOPENED:
        /*TLD*/
        (*tmp_fsm_ptr->pdc->closing)(tmp_fsm_ptr);
        fsm_irc(tmp_fsm_ptr);
        (void)fsm_termreq(tmp_fsm_ptr);
        tmp_fsm_ptr->state = fsmSTOPPING;
        break;
  
      default:
        break;
    };
  }

  dsm_free_packet(&pkt_ptr);
} /* vsncp_3gpp2_prot_rej() */

/*===========================================================================
FUNCTION VSNCP_3GPP2_RECEIVE()

DESCRIPTION
  This function is used to process an incoming VSNCP packet
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Code      |  Identifier   |            Length             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    OUI                        |    Data ...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

PARAMETERS
  (IN) device: PPP device on which the packet was received
  (IN) pkt_ptr: Pointer to the DSM item containing the rejected packet

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void vsncp_3gpp2_receive
(
  ppp_dev_enum_type   device,
  dsm_item_type     * pkt_ptr
)
{
  uint8 index = 0;
  ncpi_cb_type * instance_ptr = NULL;
  uint8 pdn_id;
  struct config_hdr cnf_hdr;
  struct option_hdr opt_hdr;
  uint8 buf[8];
  uint8 error_code;
  uint8 offset = 0;
  vsncp_3gpp2i_type * vsncp_cb_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("vsncp_3gpp2_receive()");

  /*-------------------------------------------------------------------------
    Peek through packet to find pdn-id and corresponding fsm
  -------------------------------------------------------------------------*/
  if( dsm_extract( pkt_ptr,
                   VSNCP_3GPP2I_PDN_ID_OFFSET,
                   &pdn_id,
                   VSNCP_3GPP2I_PDN_ID_VAL_SIZ )
      != VSNCP_3GPP2I_PDN_ID_VAL_SIZ )
  {
    LOG_MSG_ERROR_1("dsm_extract() failed, pkt = 0x%x", pkt_ptr);
    dsm_free_packet( &pkt_ptr );
    return;
  }

  for( index = 0; index < PS_PPP_NCP_CB_NUM; index++ )
  {
    instance_ptr =
      (ncpi_cb_type *)ps_mem_index_to_buf( index,
                                           PS_MEM_PPP_NCP_CB_TYPE );
    if( ( NCPI_CB_IS_INITED( instance_ptr ) == TRUE )  &&
        ( instance_ptr->ncp == PPP_NCP_VSNCP_3GPP2 ) &&
        ( instance_ptr->cb_u.vsncp_3gpp2.config.option_vals.pdn_id == pdn_id ) )
    {
      LOG_MSG_INFO2_1("VSNCP: Matching NCP handle found: %p", instance_ptr);
      fsm_proc( &(instance_ptr->cb_u.vsncp_3gpp2.fsm), pkt_ptr );
      /*---------------------------------------------------------------------
        PDN activity seen, call the callback if registered
      ---------------------------------------------------------------------*/
      vsncp_cb_ptr = &(instance_ptr->cb_u.vsncp_3gpp2);
      if( vsncp_cb_ptr->config.activity_cb.func != NULL )
      {
        LOG_MSG_INFO2_0("VSNCP RX: Calling activity cback");
        vsncp_cb_ptr->config.activity_cb.func
          ( vsncp_cb_ptr->config.activity_cb.data );
      }      
      return;
    }
  } /* for */

  /*-------------------------------------------------------------------------
    No matching PDN-ID was found for this VSNCP packet. Check to see if this
    is a Config-Req. If so, add the error code option to the tail end of
    the packet, change it to a Config-Reject and send it back.
         0                   1                   2                   3
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |     Code      |  Identifier   |            Length             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                    OUI                        |    Data ...
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Pull the configuration header
  -------------------------------------------------------------------------*/
  if( ntohcnf( &cnf_hdr, &pkt_ptr ) == -1)
  {
    LOG_MSG_ERROR_0("Malformed VSNCP packet");
    pppi_skipped( device, &pkt_ptr );
    dsm_free_packet( &pkt_ptr );
    return;
  }

  if( cnf_hdr.code == CONFIG_REQ )
  {
    /*---------------------------------------------------------------------
      Create the error code option and add it at the tail end
    ---------------------------------------------------------------------*/
    opt_hdr.type = VSNCP_3GPP2I_ERROR_CODE;
    opt_hdr.length = OPTION_HDR_LEN + sizeof(uint8);
    error_code = (uint8)VSNCP_3GPP2I_GEN_ERROR;
    COPY_BYTES( buf, &opt_hdr, OPTION_HDR_LEN, offset );
    COPY_BYTES( buf, &error_code, sizeof( uint8), offset );
    if( dsm_pushdown_tail( &pkt_ptr,
                           buf,
                           VSNCP_3GPP2I_ERROR_CODE_SIZ,
                           DSM_DS_SMALL_ITEM_POOL
                         ) != VSNCP_3GPP2I_ERROR_CODE_SIZ )
    {
      LOG_MSG_ERROR_0("Dropping VSNCP pkt, error adding error code");
      pppi_skipped( device, &pkt_ptr );
      return;
    }

    /*-----------------------------------------------------------------------
      Change the header code to indicate this is a CONFIG_REJ and
      put header back on after updating header length appropriately
    -----------------------------------------------------------------------*/
    cnf_hdr.code = CONFIG_REJ;
    cnf_hdr.length += VSNCP_3GPP2I_ERROR_CODE_SIZ;
    pkt_ptr = (dsm_item_type*)htoncnf( &cnf_hdr, pkt_ptr);
    if( pkt_ptr != NULL )
    {
      /*---------------------------------------------------------------------
        Send the packet out
      ---------------------------------------------------------------------*/
      if( pppi_outgoing( device,
                         PPP_VSNCP_PROTOCOL,
                         &pkt_ptr,
                         NULL ) == -1 )
      {
        /*-------------------------------------------------------------------
          pppi_outgoing() frees the packet in case of an error
        -------------------------------------------------------------------*/
        LOG_MSG_ERROR_0( "pppi_outgoing() Error");
        return;
      }
    } /* outbound config-rej */
  } /* incoming config-req */

  /*-------------------------------------------------------------------------
    Catch-all; drop the packet
  -------------------------------------------------------------------------*/
  pppi_skipped( device, &pkt_ptr );
} /* vsncp_3gpp2_receive() */

/*===========================================================================
FUNCTION VSNCP_3GPP2_REQUEST()

DESCRIPTION
  This function processes the VSNCP C-Req packet from the HSGW.
  If all the options contained in the C-Req are acceptable (either expected
  or unrecognized), this should lead to sending a C-Ack back to the HSGW.
  If the C-Req contains some unacceptable options (e.g. an empty IP address,
  the C-Req will be dropped and VSNCP termination initiated.)

PARAMETERS
  (IN) fsm_ptr        : Pointer to the FSM structure
  (IN) config_hdr_ptr : Pointer to the config header for the C-Req
  (IN) pkt_ptr        : Pointer to the DSM item - data_ptr points to the
                        first option within the C-Req

DEPENDENCIES
  None

RETURN VALUE
  0: Success (a C-Ack message was sent)
 -1: Failure (an error occured, no C-Ack message was sent)

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_request
(
  ppp_fsm_type      * fsm_ptr,
  struct config_hdr * config_hdr_ptr,
  dsm_item_type     * pkt_ptr
)
{
  struct option_hdr  option;                      /* option header storage */
  int                option_result;               /* option reply          */
  int                remaining;
  int                option_index;
  int32              signed_len;
  dsm_item_type    * reply_pkt_ptr = NULL;        /* reply packet          */
  dsm_item_type    * option_pkt_ptr = NULL;       /* option copy           */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_0("vsncp_3gpp2_request()");

  /*-------------------------------------------------------------------------
    Set remote.work to 0 (clear all options), we start afresh for this C-Req
    and update remote.work as we work through the options
  -------------------------------------------------------------------------*/
  fsm_ptr->remote.work = 0;
  reply_pkt_ptr = dsm_offset_new_buffer( DSM_DS_SMALL_ITEM_POOL,
                                         PPP_MAX_HDR );
  if( reply_pkt_ptr == NULL )
  {
    LOG_MSG_ERROR_0 ("no dsm item for reply!");
    dsm_free_packet( &pkt_ptr );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Process options requested by remote host
  -------------------------------------------------------------------------*/
  signed_len = config_hdr_ptr->length;
  while( ( signed_len > 0 )  &&  ( ntohopt(&option, &pkt_ptr) != -1 ) )
  {
    signed_len -= option.length;
    if( signed_len < 0 )
    {
      LOG_MSG_ERROR_0("3gpp2 vsncp(): option length > packet length");
      dsm_free_packet( &pkt_ptr );
      dsm_free_packet( &reply_pkt_ptr );
      return -1;
    }

    /*-----------------------------------------------------------------------
      save rest of option for later
    -----------------------------------------------------------------------*/
    remaining = option.length - OPTION_HDR_LEN;
    if( remaining > 0 )
    {
      (void)dsm_dup_packet( &option_pkt_ptr, pkt_ptr, 0, (uint16)remaining );
      if ( option_pkt_ptr == NULL )
      {
        LOG_MSG_ERROR_0("3gpp2 vsncp(): dsm_dup failed for c-req");
        dsm_free_packet( &pkt_ptr );
        dsm_free_packet( &reply_pkt_ptr );
        return -1;
      }
    }

    /*-----------------------------------------------------------------------
      The check function should usually just update the remote work vals and
      return one of:
      (1) RECOGNIZED: implying include this option in the C-ack;
      (2) UNRECOGNIZED: implying exclude this option from the C-Ack; OR
      (3) -1: implying give up on processing this, something *BAD* happened
              (like a memory error)
    -----------------------------------------------------------------------*/
    option_result = (*fsm_ptr->pdc->check)( fsm_ptr,
                                            &option,
                                            &pkt_ptr,
                                            TRUE );
    if( option_result < 0 )
    {
      LOG_MSG_ERROR_1("3gpp2 vsncp(): option check failed for opt 0x%x",
                      option.type);
      dsm_free_packet( &pkt_ptr );
      dsm_free_packet(&reply_pkt_ptr);
      dsm_free_packet(&option_pkt_ptr);
      return -1;
    }

    /*-----------------------------------------------------------------------
      If the option is RECOGNIZED, update remote.work and include option
      in the C-Ack
    -----------------------------------------------------------------------*/
    if( option_result == RECOGNIZED )
    {
      option_index = fsm_n_option( fsm_ptr->pdc, option.type );
      fsm_ptr->remote.work |= (1 << option_index);
      htonopt_copy( &reply_pkt_ptr, &option, &option_pkt_ptr );
    }

    dsm_free_packet( &option_pkt_ptr );
  } /* while more options */

  /*-------------------------------------------------------------------------
    Send C-ACK to remote host (this should be the common case)
  -------------------------------------------------------------------------*/
  if( fsm_send( fsm_ptr,
                CONFIG_ACK,
                config_hdr_ptr->identifier,
                reply_pkt_ptr ) != 0 )
  {
    LOG_MSG_ERROR_0("Unable to send C-Ack message");
    dsm_free_packet( &pkt_ptr );
    return -1;
  }

  dsm_free_packet( &pkt_ptr );
  return 0;
} /* vsncp_3gpp2_request() */

/*===========================================================================
FUNCTION VSNCP_3GPP2_TERM_REQ()

DESCRIPTION
  This function processes the VSNCP Term-Req packet from the HSGW. 
PARAMETERS
  (IN) fsm_ptr        : Pointer to the FSM structure
  (IN) config_hdr_ptr : Pointer to the config header for the C-Ack
  (IN) pkt_ptr_ptr    : Pointer to the DSM item pointer - data_ptr points to the
                        first option within the C-Ack

DEPENDENCIES
  None

RETURN VALUE
  0: Success (the Term-Req was succesfully processed)
 -1: Failure (an error occured during Term-Req processing)

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_term_req
(
  ppp_fsm_type      * fsm_ptr,
  struct config_hdr * config_hdr_ptr,
  dsm_item_type     **pkt_ptr_ptr
)
{
  struct option_hdr option = {0};   
  int32             signed_len = 0;
  uint32            opt_val_len = 0;
  uint8             reconnect_ind_value = 0;

  signed_len = config_hdr_ptr->length;
  LOG_MSG_INFO2_1("vsncp_3gpp2_term_req, length: %d", signed_len );
  while( ( signed_len > 0 )  &&  ( ntohopt( &option, pkt_ptr_ptr ) != -1 ) )
  {
    signed_len -= OPTION_HDR_LEN;
    if( signed_len < 0 )
    {
      LOG_MSG_ERROR_0("3gpp2 vsncp(): option length > packet length");
      dsm_free_packet( pkt_ptr_ptr );
      return -1;
    }
    opt_val_len = option.length - OPTION_HDR_LEN;
    LOG_MSG_INFO2_2("vsncp_3gpp2_term_req, opt len: %d, opt type:%d",
                    option.length, option.type );

    if( VSNCP_3GPP2I_RECONNECT_IND==option.type )
    {
        if( option.length != VSNCP_3GPP2I_RECONNECT_IND_SIZ)
        {
          LOG_MSG_ERROR_0("3gpp2 vsncp(): wrong option length for reconnect ind option");
          dsm_free_packet( pkt_ptr_ptr );
          return -1;
        }
        PULLUP_BYTES( pkt_ptr_ptr, &reconnect_ind_value, sizeof( uint8 ), signed_len );
        if( VSNCP_RECONN_IND_NOT_ALLOWED == reconnect_ind_value )
        {
          fsm_ptr->fail_reason = PPP_FAIL_REASON_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED;
        }
        LOG_MSG_INFO2_1("recved vsncp term req with reconnect indication: %d",
                        reconnect_ind_value );
    }
    else
    {
      PULLUP_BYTES( pkt_ptr_ptr, NULL, (uint16)opt_val_len, signed_len );

      LOG_MSG_INFO2_1("recved vsncp term req with option: %d, discard",
                      option.type );
    }
  }
  dsm_free_packet( pkt_ptr_ptr );
  return 0;
}
/*===========================================================================
FUNCTION VSNCP_3GPP2_ACK()

DESCRIPTION
  This function processes the VSNCP C-Ack packet from the HSGW.
  If all the options contained in the C-Ack are acceptable (either expected
  or unrecognized), this should lead to updating the local working values
  for the options (recall that a VSNCP C-Ack contains assigned values for the
  options previously requested via the C-Req.

PARAMETERS
  (IN) fsm_ptr        : Pointer to the FSM structure
  (IN) config_hdr_ptr : Pointer to the config header for the C-Ack
  (IN) pkt_ptr        : Pointer to the DSM item - data_ptr points to the
                        first option within the C-Ack

DEPENDENCIES
  None

RETURN VALUE
  0: Success (the C-Ack was succesfully processed)
 -1: Failure (an error occured during C-Ack processing)

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_ack
(
  ppp_fsm_type      * fsm_ptr,
  struct config_hdr * config_hdr_ptr,
  dsm_item_type     * pkt_ptr
)
{
  struct option_hdr option;                       /* option header storage */
  int               option_result;                /* option reply          */
  int               option_index;
  int32             signed_len;
  vsncp_3gpp2_local_opts_val_type  * lwork_ptr = NULL;
  vsncp_3gpp2_local_opts_val_type  * lwant_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_0("vsncp_3gpp2_ack()");

  /*-------------------------------------------------------------------------
    ID field must match last request we sent
  -------------------------------------------------------------------------*/
  if (config_hdr_ptr->identifier != fsm_ptr->lastid)
  {
    LOG_MSG_ERROR_0("Mismatched id in VSNCP Config-Ack, dropping pkt");
    dsm_free_packet( &pkt_ptr );
    return -1;
  }

  lwork_ptr=(vsncp_3gpp2_local_opts_val_type *)((&(fsm_ptr->local))->work_pdv);
  lwant_ptr = (vsncp_3gpp2_local_opts_val_type *)((&(fsm_ptr->local))->want_pdv);
  /*-------------------------------------------------------------------------
    Set local.work to 0 (clear all options), we start afresh for this C-Ack
    and update local.work as we work through the options
  -------------------------------------------------------------------------*/
  fsm_ptr->local.work = 0;

  /*-------------------------------------------------------------------------
    Process options in the C-Ack
  -------------------------------------------------------------------------*/
  signed_len = config_hdr_ptr->length;
  while( ( signed_len > 0 )  &&  ( ntohopt( &option, &pkt_ptr ) != -1 ) )
  {
    signed_len -= option.length;
    if( signed_len < 0 )
    {
      LOG_MSG_ERROR_0("3gpp2 vsncp(): option length > packet length");
      dsm_free_packet( &pkt_ptr );
      return -1;
    }

    /*-----------------------------------------------------------------------
      The check function should usually just update the local work vals and
      return one of:
      (1) RECOGNIZED: implying the option is appropriate for the C-Ack;
      (2) UNRECOGNIZED: implying the option should be ignored; OR
      (3) -1: implying give up on processing this packet, something *BAD*
              happened.
    -----------------------------------------------------------------------*/
    option_result = (*fsm_ptr->pdc->check)( fsm_ptr,
                                            &option,
                                            &pkt_ptr,
                                            FALSE );
    if( option_result < 0 )
    {
      LOG_MSG_ERROR_0("3gpp2 vsncp option check failed, dropping C-Ack!");
      /*---------------------------------------------------------------------
        An error occured. Throwing away the C-Ack. Reset the local.work
        back to local.want, so in case we send another C-Req after this
        point, it will be just like the original one. (fsm_makereq uses the
        work values to form a C-Req, so this will ensure that we send the
        original C-Req values)
      ---------------------------------------------------------------------*/
      fsm_ptr->local.work = fsm_ptr->local.want;
      memscpy( fsm_ptr->local.work_pdv,
               sizeof(vsncp_3gpp2_local_opts_val_type),
               fsm_ptr->local.want_pdv,
               sizeof(vsncp_3gpp2_local_opts_val_type) );

      /*---------------------------------------------------------------------
        Throw away the packet
      ---------------------------------------------------------------------*/
      dsm_free_packet( &pkt_ptr );
      return -1;
    }

    if( option_result == RECOGNIZED )
    {
      option_index = fsm_n_option( fsm_ptr->pdc, option.type );
      fsm_ptr->local.work |= (1 << option_index);
    }
  } /* while more options */

  dsm_free_packet( &pkt_ptr );
  /*---------------------------------------------------------------------
    Clean up the non negotiated values of the address.     
  ---------------------------------------------------------------------*/
  if( lwork_ptr->req_addr_type ==  VSNCP_3GPP2_PDN_TYPE_IPV4 )
  {    
    memset( &(lwork_ptr->iid), 0, sizeof( uint64 ));
  }
  else if( lwork_ptr->req_addr_type ==  VSNCP_3GPP2_PDN_TYPE_IPV6 )
  {
    memset( &(lwork_ptr->ip4.ps_s_addr), 0, sizeof(struct ps_in_addr));
  }

  /*-------------------------------------------------------------------------
  For the following scenarios, stop this NCP and bail!
  (1) If the PDN-Address option didn't get negotiated during VSNCP; OR
  (2) The PDN Address option was negotiated but the IP address obtained
  was empty. Note that we use the local want to see if we are
  interested in IPv4, IPv6 or IPv4_IPv6.
 -------------------------------------------------------------------------*/
  if( ( fsm_ptr->local.work & (uint16)VSNCP_3GPP2I_N_PDN_ADDR ) == 0 ||
        ( ( fsm_ptr->local.work & (uint16)VSNCP_3GPP2I_N_PDN_ADDR ) &&
          ( ( (lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4) &&
              ( lwork_ptr->ip4.ps_s_addr == 0) ) ||
            ( (lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV6) &&
              ( lwork_ptr->iid == 0) )
#ifdef FEATURE_DATA_EHRPD_DUAL_IP
            || ( (lwant_ptr->req_addr_type == VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6) &&
               ( lwork_ptr->ip4.ps_s_addr == 0) &&
               ( lwork_ptr->iid == 0) )
#endif
          ) ) )
  {
     fsm_ptr->fail_reason = PPP_FAIL_REASON_VSNCP_FAILURE;
     LOG_MSG_ERROR_2("pdn-id %d addr type 0x%x unable to get address! Quitting...",
                     lwant_ptr->pdn_id, lwant_ptr->req_addr_type );

     fsm_close( fsm_ptr );
     if( fsm_ptr->state != fsmCLOSING )
     {
        fsm_ptr->mode = PPP_CP_MODE_INVALID;
     }

     return -1;
  }
  return 0;
} /* vsncp_3gpp2_ack() */

/*===========================================================================
FUNCTION VSNCP_3GPP2_REJECT

DESCRIPTION
  This function processes the VSNCP Config-Rej packet from the HSGW.

PARAMETERS
  (IN) fsm_ptr        : Pointer to the FSM structure
  (IN) config_hdr_ptr : Pointer to the config header for the Config-Rej
  (IN) pkt_ptr        : Pointer to the DSM item - data_ptr points to the
                        first option within the Config-Rej

DEPENDENCIES
  None

RETURN VALUE
  0: Success (the Config-Rej was succesfully processed)
 -1: Failure (an error occured during Config-Rej processing)

SIDE EFFECTS
  None
===========================================================================*/
int vsncp_3gpp2_reject
(
  ppp_fsm_type      * fsm_ptr,
  struct config_hdr * config_hdr_ptr,
  dsm_item_type     * pkt_ptr
)
{
  int32 signed_length;
  struct option_hdr option;
  int option_index;
  int option_val_bytes;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_0("vsncp_3gpp2_reject()");

  /*-------------------------------------------------------------------------
    ID field must match last request we sent
  -------------------------------------------------------------------------*/
  if( config_hdr_ptr->identifier != fsm_ptr->lastid )
  {
    LOG_MSG_ERROR_0("Mismatched id in VSNCP Config-Rej, dropping pkt");
    dsm_free_packet( &pkt_ptr );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Process in order, checking for errors
  -------------------------------------------------------------------------*/
  signed_length = config_hdr_ptr->length;
  while( ( signed_length > 0 )  &&  ( ntohopt( &option, &pkt_ptr ) != -1 ) )
  {
    signed_length -= option.length;
    if( signed_length < 0 )
    {
      LOG_MSG_ERROR_0("Malformed option in Config-Rej, dropping pkt");
      dsm_free_packet( &pkt_ptr );
      return -1;
    }

    /*-------------------------------------------------------------------------
      Options contained within a Config-Rej packet must be a subset of
      the ones being negotiated (with the exception of the
      VSNCP_3GPP2I_ERROR_CODE option. So, if the option is unrecognized,
      drop this packet and return failure.
    -------------------------------------------------------------------------*/
    option_index = fsm_n_option( fsm_ptr->pdc, option.type );
    if( option_index == -1 )
    {
      LOG_MSG_INFO2_3( "vsncp_3gpp2_reject(): dev %d, fsm %d, "
               "unrecognized option %d",
                       fsm_ptr->device, fsm_ptr->pdc->fsmi, option.type );
      dsm_free_packet( &pkt_ptr );
      return -1;
    }

    if ( option.type > fsm_ptr->pdc->option_limit )
    {
      LOG_MSG_ERROR_0("Unsupported option in Config-Rej, dropping pkt");
      dsm_free_packet( &pkt_ptr );
      return -1;
    }
    else if ((!(fsm_ptr->local.work & (1 << option_index))) &&
              (option.type != VSNCP_3GPP2I_ERROR_CODE))
    {
      LOG_MSG_ERROR_0("Invalid option in Config-Rej, dropping pkt");
      dsm_free_packet( &pkt_ptr );
      return -1;
    }

    /*-------------------------------------------------------------------------
      Process the specific option contained within the Config-Rej
    -------------------------------------------------------------------------*/
    if ( (*fsm_ptr->pdc->reject)(fsm_ptr, &option, pkt_ptr ) != 0 )
    {
      LOG_MSG_ERROR_0("Throwing away Config-Rej packet");
      dsm_free_packet( &pkt_ptr );
      return -1;
    }

    /*-------------------------------------------------------------------------
      Free the value portion of the option before moving on to the next
    -------------------------------------------------------------------------*/
    option_val_bytes = option.length - OPTION_HDR_LEN;
    if ( dsm_pullup( &pkt_ptr , NULL, (uint16)option_val_bytes )
           != option_val_bytes )
    {
      return -1;
    }
  }

  dsm_free_packet( &pkt_ptr );
  return 0;
} /* vsncp_3gpp2_reject() */
#endif /* FEATURE_DATA_PS_EHRPD */
