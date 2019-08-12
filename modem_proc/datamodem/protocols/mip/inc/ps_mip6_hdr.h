#ifndef PS_MIP6_HDR_H
#define PS_MIP6_HDR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ M I P 6 _ H D R . H 


GENERAL DESCRIPTION
  This module implements Mobile IPv6 Mobility Headers (MH)

EXTERNALIZED FUNCTIONS

  ps_mip6_hdr_create()
    This function adds the MIPv6 mobility header to the outgoing packet
  
  ps_mip6_hdr_parse()
    This function parses the MIPv6 mobility header in the incoming packet

  ps_mip6_option_create()
    This function adds a MIPv6 mobility option to the outgoing packet.
  
  ps_mip6_option_parse()
    This function parses a MIPv6 mobility option in the incoming packet

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
    Copyright (c)2006-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/ps_mip6_hdr.h#1 $
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
#include "dsm.h"
#include "ps_pkt_info.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_HDR_ENUM_TYPE

DESCRIPTION
  Enum of MIPv6 Mobility Headers
---------------------------------------------------------------------------*/
typedef enum 
{
  MIP6_BIND_REFRESH_REQUEST_MSG  = 0,      /* Binding Rfrsh Req            */
  MIP6_HOME_TEST_INIT_MSG        = 1,      /* HoTI Message                 */
  MIP6_CARE_OF_TEST_INIT_MSG     = 2,      /* CoTI Message                 */
  MIP6_HOME_TEST_MSG             = 3,      /* HoT Message                  */
  MIP6_CARE_OF_TEST_MSG          = 4,      /* CoT Message                  */
  MIP6_BIND_UPDATE_MSG           = 5,      /* Binding Update               */
  MIP6_BIND_ACK_MSG              = 6,      /* Binding Ack                  */
  MIP6_BIND_ERROR_MSG            = 7,      /* Binding Error                */
  MIP6_FAST_BIND_UPDATE_MSG      = 8,      /* Fast Binding Update          */
  MIP6_FAST_BIND_ACK_MSG         = 9,      /* Fast Binding Ack             */
  MIP6_FAST_NEIGHBOR_ADV_MSG     = 10      /* Fast Neighbor Adv            */
} ps_mip6_hdr_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_OPTION_TYPE

DESCRIPTION
  Enum of MIPv6 Mobility Options
---------------------------------------------------------------------------*/
typedef enum
{
  MIP6_PAD1_OPTION               = 0,      /* Pad1                         */
  MIP6_PADN_OPTION               = 1,      /* PadN                         */
  MIP6_BIND_RFRSH_ADVICE_OPTION  = 2,      /* Binding Refresh Advice       */
  MIP6_ALTERNATE_COA_OPTION      = 3,      /* Alternate CoA                */
  MIP6_NONCE_INDICES_OPTION      = 4,      /* Nonce Indices                */
  MIP6_BIND_AUTH_DATA_OPTION     = 5,      /* Binding Authorization Data   */
  MIP6_MOBILE_NW_PREFIX_OPTION   = 6,      /* Mobile Network Prefix        */
  MIP6_MOBILITY_HDR_LLA_OPTION   = 7,      /* Mobility HDR Link-Layer Addr */
  MIP6_MN_ID_OPTION              = 8,      /* MN ID                        */
  MIP6_AUTH_OPTION               = 9,      /* Authentication               */
  MIP6_MESG_ID_OPTION            = 10,     /* Mesd-id option               */
  MIP6_VS_OPTION                 = 19      /* X.S0011 VS option            */
} ps_mip6_option_enum_type;


/*---------------------------------------------------------------------------
  Common defines
---------------------------------------------------------------------------*/
#define MIP6_ALTERNATE_COA_OPTION_LEN          16
#define MIP6_BIND_RFRSH_ADVICE_OPTION_LEN      2

#define MIP6_HA_OPTION_LEN                     16
#define MIP6_HA_OPTION_PADDING_LEN             4
#define MIP6_HA_OPTION_DEST_OPT_EXT_HDR_LEN    \
    ( ( sizeof( ip6_dest_opt_hdr_type )   +    \
        MIP6_HA_OPTION_PADDING_LEN        +    \
        sizeof( ps_mip6_option_hdr_type ) +    \
        MIP6_HA_OPTION_LEN  - 8 ) >> 3 )

#define A_BIT                              0x8000
#define H_BIT                              0x4000
#define L_BIT                              0x2000
#define K_BIT                              0x1000

#define BIND_ACK_K_BIT                     0x80

#define MIP6_MOBILITY_HDR_CKSUM_OFFSET     4

#define MIP6_OPT_BASE_HDR_LEN              2

#define MIP6_AUTH_KEY_LEN                  16

#define MIP6_MN_NAI_SUBTYPE                1

#define MIP6_MN_HA_AUTH_OPT_LEN            17
#define MIP6_MN_AAA_AUTH_OPT_LEN           25
#define MIP6_MN_HA_AUTHENTICATOR_LEN       12
#define MIP6_MN_AAA_AUTHENTICATOR_LEN      20

#define MIP6_VS_3GPP2_VENDOR_ID            5535

#define MIP6_VS_OPT_LEN                    40
#define MIP6_VS_OPT_SUBTYPE_DNS            1
#define MIP6_VS_OPT_VENDOR_ID_OFFSET       0
#define MIP6_VS_OPT_SUBTYPE_OFFSET         4
#define MIP6_VS_OPT_PRIM_DNS_OFFSET        8
#define MIP6_VS_OPT_SEC_DNS_OFFSET         24

#define MIP6_DSM_ERROR                     (-2)

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_AUTH_OPT_SUBTYPE_ENUM_TYPE

DESCRIPTION
  Enum of authentication option subtypes 
---------------------------------------------------------------------------*/
typedef enum
{
  MIP6_MN_HA_AUTH  = 1,
  MIP6_MN_AAA_AUTH = 2
} ps_mip6_auth_opt_subtype_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_BIND_ACK_STATUS_TYPE

DESCRIPTION
  Enum of Binding Acknowledgement status values
---------------------------------------------------------------------------*/
typedef enum
{
  MIP6_BA_ACCEPTED                     = 0,
  MIP6_BA_ACCEPTED_NEED_PREFIX_DISCOV  = 1,
  MIP6_REASON_UNSPECIFIED              = 128,
  MIP6_ADMIN_PROHIBIT                  = 129,
  MIP6_INSUFFICIENT_RESOURCES          = 130,
  MIP6_HOME_REG_NOT_SUPPORTED          = 131,
  MIP6_NOT_HOME_SUBNET                 = 132,
  MIP6_NOT_HOME_AGENT_FOR_THIS_MN      = 133,
  MIP6_DUP_ADDR_DETECT_FAILED          = 134,
  MIP6_SEQNUM_OUT_OF_WINDOW            = 135,
  MIP6_EXPIRED_HOME_NONCE_INDEX        = 136,
  MIP6_EXPIRED_CARE_OF_NONCE_INDEX     = 137,
  MIP6_EXPIRED_NONCES                  = 138,
  MIP6_REG_TYPE_CHG_DISALLOWED         = 139,
  MIP6_ID_MISMATCH                     = 144,
  MIP6_MESG_ID_REQD                    = 145,
  MIP6_AUTH_FAIL                       = 146
} ps_mip6_bind_ack_status_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_BASE_HDR_TYPE

DESCRIPTION
  Format of the Mobility Header (RFC 3775, Sec. 6.1.1)

  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  | Payload Proto |  Header Len   |   MH Type     |   Reserved    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |           Checksum            |                               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
  |                                                               |
  .                                                               .
  .                       Message Data                            .
  .                                                               .
  |                                                               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
---------------------------------------------------------------------------*/
typedef struct ps_mip6_base_hdr_type
{
  uint8   payload_proto;
  uint8   header_len;
  uint8   mh_type;
  uint8   reserved;
  uint16  checksum;
} ps_mip6_base_hdr_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_BIND_UPDATE_HDR_TYPE

DESCRIPTION
  Format of the Binding Update Message (RFC 3775, Sec. 6.1.7)

                                  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  < - - - - - - flags - - - - - > |          Sequence #           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |A|H|L|K|        Reserved       |           Lifetime            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                                                               |
  .                                                               .
  .                        Mobility options                       .
  .                                                               .
  |                                                               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
---------------------------------------------------------------------------*/
typedef struct ps_mip6_bind_update_hdr_type
{
  uint16  seqnum;
  uint16  flags; 
  uint16  lifetime;
} ps_mip6_bind_update_hdr_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_BIND_ACK_HDR_TYPE

DESCRIPTION
  Format of the Binding Update Message (RFC 3775, Sec. 6.1.8)
                                                   < -  flags -  >
                                  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                                  |    Status     |K|  Reserved   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |           Sequence #          |           Lifetime            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                                                               |
  .                                                               .
  .                        Mobility options                       .
  .                                                               .
  |                                                               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
---------------------------------------------------------------------------*/
typedef struct ps_mip6_bind_ack_hdr_type
{
  uint8   status;
  uint8   flags; 
  uint16  seqnum;
  uint16  lifetime;
} ps_mip6_bind_ack_hdr_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_HDR_TYPE

DESCRIPTION
  Format of the Mobility Header (RFC 3775, Sec. 6.1.1)

  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  | Payload Proto |  Header Len   |   MH Type     |   Reserved    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |           Checksum            |                               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
  |                                                               |
  .                                                               .
  .                       Message Data                            .
  .                                                               .
  |                                                               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
---------------------------------------------------------------------------*/
typedef struct ps_mip6_hdr_type
{
  ps_mip6_base_hdr_type base_hdr;
  union
  {
    ps_mip6_bind_update_hdr_type  bu;
    ps_mip6_bind_ack_hdr_type     ba;
  } hdr_body;
} ps_mip6_hdr_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_OPTION_HDR_TYPE

DESCRIPTION
  Format of the Mobility Option header (RFC 3775, Sec. 6.2.1)

   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |  Option Type  | Option Length |   Option Data...
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
---------------------------------------------------------------------------*/
typedef struct ps_mip6_option_hdr_type
{
  uint8   type;
  uint8   length;
} ps_mip6_option_hdr_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_AUTH_OPTION_CREATE_DATA_TYPE

DESCRIPTION
  Value required to create the authentication option going in a BU
---------------------------------------------------------------------------*/
typedef struct ps_mip6_auth_option_create_data_type
{
  uint8   subtype;
  uint32  spi;
  uint8   auth_key[MIP6_AUTH_KEY_LEN]; 
  uint16  auth_key_len;
} ps_mip6_auth_option_create_data_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_MIP6_MESG_ID_OPTION_DATA_TYPE

DESCRIPTION
  Value required to create the authentication option going in a BU
---------------------------------------------------------------------------*/
typedef struct ps_mip6_mesg_id_option_data_type
{
  uint32 ntp_secs;     /* Seconds in NTP format */
  uint32 ntp_fsecs;    /* fractional seconds */
} ps_mip6_mesg_id_option_data_type; 
/*===========================================================================
MACRO PS_MIP6I_PUSHDOWN_PADDING()

DESCRIPTION
  This macro is used to pushdown num_padding bytes onto a DSM chain
 
PARAMETERS
  dsm_ptr_ptr         : Pointer to pointer to dsm item
  num_padding : Number of padding bytes to pushdown
  
RETURN VALUE
  None
  
DEPENDENCIES
  None
  
SIDE EFFECTS
===========================================================================*/
#define PS_MIP6I_PUSHDOWN_PADDING( dsm_ptr_ptr, num_padding )               \
{                                                                           \
  ps_mip6_option_hdr_type  hdr;                                             \
  uint8                    buf[5];                                          \
  if( num_padding == 1 )                                                    \
  {                                                                         \
    hdr.type = (uint8)MIP6_PAD1_OPTION;                                            \
    (void)dsm_pushdown_packed( dsm_ptr_ptr,                                 \
                               &(hdr.type),                                 \
                               sizeof( uint8 ),                             \
                               DSM_DS_SMALL_ITEM_POOL );                    \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    hdr.type   = (uint8)MIP6_PADN_OPTION;                                          \
    hdr.length = (uint8)(num_padding - 2);                                  \
    memset( (void *)buf, 0, 5);                                             \
    (void)dsm_pushdown_packed( dsm_ptr_ptr,                                 \
                               buf,                                         \
                               (uint16)(num_padding - 2),                   \
                               DSM_DS_SMALL_ITEM_POOL );                    \
    (void)dsm_pushdown_packed( dsm_ptr_ptr,                                 \
                               &hdr,                                        \
                               sizeof( ps_mip6_option_hdr_type ),           \
                               DSM_DS_SMALL_ITEM_POOL );                    \
  }                                                                         \
}

/*===========================================================================
MACRO PS_MIP6I_4ALIGN_PAD_BYTES()

DESCRIPTION
  This macro calculates the number of bytes needed for padding given the
  padding requirements as 4n+x and the current length of packet 
 
PARAMETERS
  len         : Current length of packet 
  x           : Offset where the padding requirement is 4n+x 
  
RETURN VALUE
  Number of bytes of padding required
  
DEPENDENCIES
  None
  
SIDE EFFECTS
===========================================================================*/
#define PS_MIP6I_4ALIGN_PAD_BYTES(len, x) (((4 - ((len) & 0x3)) + (x)) & 0x3)

/*===========================================================================
MACRO PS_MIP6I_8ALIGN_PAD_BYTES()

DESCRIPTION
  This macro calculates the number of bytes needed for padding given the
  padding requirements as 8n+x and the current length of packet 
 
PARAMETERS
  len         : Current length of packet 
  x           : Offset where the padding requirement is 8n+x 
  
RETURN VALUE
  Number of bytes of padding required
  
DEPENDENCIES
  None
  
SIDE EFFECTS
===========================================================================*/
#define PS_MIP6I_8ALIGN_PAD_BYTES(len, x) (((8 - ((len) & 0x7)) + (x)) & 0x7)

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_MIP6_HDR_CREATE

DESCRIPTION
  This function is used to create a MIPv6 Mobility Header according to the 
  parameters passed by the caller

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_mip6_hdr_create
( 
  uint8              * pkt_buf_ptr, 
  uint16               pkt_buf_size,
  ps_mip6_hdr_type   * hdr_ptr,
  pseudo_header_type * ph
);

/*===========================================================================
FUNCTION PS_MIP6_HDR_PARSE

DESCRIPTION
  This function is used to parse a MIPv6 Mobility Header

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_mip6_hdr_parse
(
  dsm_item_type     * dsm_ptr,
  uint16            * offset,
  ps_mip6_hdr_type  * hdr_ptr
);

/*===========================================================================
FUNCTION PS_MIP6_OPTION_CREATE

DESCRIPTION
  This function is used to add a mobility option to the message

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_mip6_option_create
(
  uint8                     * pkt_buf_ptr,
  uint16                      pkt_but_len,
  ps_mip6_option_enum_type    option,
  void                      * opt_data_ptr,
  uint16                      opt_val_len,  
  pseudo_header_type        * ph
);

/*===========================================================================
FUNCTION PS_MIP6_OPTION_PARSE

DESCRIPTION
  This function is used to parse a mobility option in the incoming message

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_mip6_option_parse
(
  dsm_item_type             * dsm_ptr,
  uint16                    * offset,
  ps_mip6_option_hdr_type   * opt_hdr_ptr, 
  void                      * opt_data_ptr
);

/*===========================================================================
FUNCTION PS_MIP6_ADD_HA_OPTION

DESCRIPTION
  This function is used to add the Home Address Option to the message.
  This option is carried in a Destination Option Extension Header

PARAMETERS
  pkt_ptr     : Pointer to the packet
  next_hdr    : Protocol to insert in next header field of Destination 
                Option Extension Header
  hoa         : Home Address to insert in the Home Address Option
  length      : Out parameter, gets incremented by total number of bytes 
                added

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_mip6_add_ha_option
(
  dsm_item_type      ** dsm_ptr_ptr,
  uint8                 next_hdr,
  struct ps_in6_addr  * hoa,
  uint16              * length
);
#endif /* FEATURE_DATA_PS_MIPV6 */
#endif /* PS_MIP6_HDR_H */
