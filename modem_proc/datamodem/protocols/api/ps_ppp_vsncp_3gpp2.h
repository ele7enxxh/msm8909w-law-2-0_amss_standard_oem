#ifndef PS_PPP_VSNCP_3GPP2_H
#define PS_PPP_VSNCP_3GPP2_H
/*===========================================================================
                  P S _ P P P _ V S N C P _ 3 G P P 2 . H

DESCRIPTION
  Header file for the 3GPP2 VSNCP Interface

  Copyright (c) 2008-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_vsncp_3gpp2.h#1 $ 
  $Author: mplcsds1 $ 
  $DateTime: 2016/12/13 08:00:03 $
 
when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
05/13/14    cx     Add support for default APN indication option
06/07/13    cx     Add MTU Size in PCO Over eHRPD 


===========================================================================*/
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "ps_iface_defs.h"
#include "ps_in.h"
#include "ps_ppp_ext.h"
#include "ps_ppp_defs.h"

/*===========================================================================

                          MACRO DEFINITIONS

===========================================================================*/
#define PPP_MCC_LEN 3 
#define PPP_MNC_LEN 3

#define PPP_OP_PCO_ID_START 0xFF00
#define PPP_OP_PCO_ID_END   0xFFFF
/*---------------------------------------------------------------------------
  Values for pdn_type. Both ip_ver_capability and req_addr_type (requested
  address type) should use these values during VSNCP configuration
---------------------------------------------------------------------------*/
typedef enum
{
  VSNCP_3GPP2_PDN_TYPE_MIN       = 0,
  VSNCP_3GPP2_PDN_TYPE_EMPTY     = VSNCP_3GPP2_PDN_TYPE_MIN,
  VSNCP_3GPP2_PDN_TYPE_IPV4      = 1,
  VSNCP_3GPP2_PDN_TYPE_IPV6      = 2,
  VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 = 3,
  VSNCP_3GPP2_PDN_TYPE_MAX
} vsncp_3gpp2_pdn_type;

/*---------------------------------------------------------------------------
  Attach type
---------------------------------------------------------------------------*/
typedef enum
{
  VSNCP_3GPP2_ATTACH_TYPE_INITIAL  = 1,
  VSNCP_3GPP2_ATTACH_TYPE_HANDOVER = 3
} vsncp_3gpp2_attach_type;

/*---------------------------------------------------------------------------
  VSNCP Address Allocation cause codes 
---------------------------------------------------------------------------*/
typedef enum
{
  VSNCP_3GPP2_ADDR_ALLOC_CAUSE_NULL              = 0,
  VSNCP_3GPP2_ADDR_ALLOC_CAUSE_SUBSCR_LIMITATION = 1,
  VSNCP_3GPP2_ADDR_ALLOC_CAUSE_SUCCESS           = 255
} vsncp_3gpp2i_addr_alloc_cause_type;

/*---------------------------------------------------------------------------
  Values for DNS v4 Address request types
---------------------------------------------------------------------------*/
typedef enum
{
  VSNCP_3GPP2_DNS_V4_REQ_NONE               = 0,
  VSNCP_3GPP2_DNS_V4_REQ_IPCP               = 1,
  VSNCP_3GPP2_DNS_V4_REQ_CONTAINER          = 2,
  VSNCP_3GPP2_DNS_V4_REQ_IPCP_AND_CONTAINER = 3,
  VSNCP_3GPP2_DNS_V4_REQ_DEFAULT            = VSNCP_3GPP2_DNS_V4_REQ_IPCP_AND_CONTAINER,
  VSNCP_3GPP2_DNS_V4_REQ_MAX
} vsncp_3gpp2i_dns_v4_addr_req_type;

/*---------------------------------------------------------------------------
  Maximum length for APN. NULL APN (apn length zero) is supported.
---------------------------------------------------------------------------*/
#define VSNCP_3GPP2_APN_MAX_VAL_LEN  100

/*---------------------------------------------------------------------------
  DUAL IP iface ptrs.
---------------------------------------------------------------------------*/
#define IPV4_IFACE_PTR rx_iface_ptr
#define IPV6_IFACE_PTR second_rx_iface_ptr

/*---------------------------------------------------------------------------
  DNS requested Bit masks
---------------------------------------------------------------------------*/
#define IPV4_DNS_REQUEST  ( 1 << 0 )
#define IPV6_DNS_REQUEST  ( 1 << 1 )


/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  3GPP2 VSNCP ioctls
---------------------------------------------------------------------------*/
typedef enum
{
  VSNCP_3GPP2_IOCTL_MIN  = 0,
  VSNCP_3GPP2_GET_PARAMS = VSNCP_3GPP2_IOCTL_MIN,
  VSNCP_3GPP2_IOCTL_MAX
} vsncp_3gpp2_ioctl_type;

/*---------------------------------------------------------------------------
  PDN level authentication parameters
---------------------------------------------------------------------------*/
typedef struct ppp_vsncp_3gpp2_pdn_auth_s
{
  ppp_auth_proto_e_type protocol;

  char  user_id[PPP_MAX_USER_ID_LEN];
  uint8 user_id_len;

  char  password[PPP_MAX_PASSWD_LEN];
  uint8 password_len;
} vsncp_3gpp2_pdn_auth_type;

/*---------------------------------------------------------------------------
  Operator Reserved PCO parameters
---------------------------------------------------------------------------*/
typedef struct ppp_vsncp_3gpp2_op_pco_s
{
  uint16  container_id;
  uint8   mcc[PPP_MCC_LEN];
  uint8   mcc_len;
  uint8   mnc[PPP_MNC_LEN];
  uint8   mnc_len;
} vsncp_3gpp2_op_pco_type;
/*---------------------------------------------------------------------------
  3GPP2 VSNCP Protool Configuration Options (PCO)
  This structure contains booleans to indicate which parameters should be 
  included in the PCO configuration option sent by UE to the HSGW. 
  The following parameters are available:

  1. MS Support of Network Requested Bearer Control indicator (BCM) 
     As per X.P0057 this must be included only if the UE supports Network 
     initiated QOS. 
     This is indicated to PPP by Mode handler. 
  2. DNS Address: This is an optional parameter.
  3. P-CSCF Address: This is an optional parameter.
  4. IP Addr via NAS: This is a required parameter.
  5. PDN Level Authentication: This is an optional parameter.
  6. DNS v4 Address Request Method: This is an optional parameter.
  7. Subscription Status PCO: an operator reserved PCO used by upperlayer.
---------------------------------------------------------------------------*/
typedef struct vsncp_3gpp2_pco_opts_s
{
  boolean bcm_is_supported;
  uint8 dns_is_requested;
  boolean p_cscf_is_requested;
  boolean ip_addr_via_nas;
  vsncp_3gpp2_pdn_auth_type pdn_level_auth;
  vsncp_3gpp2i_dns_v4_addr_req_type dns_v4_addr_req;
  vsncp_3gpp2_op_pco_type op_pco;
  boolean mtu_is_requested;
} vsncp_3gpp2_pco_opts_type;

/*---------------------------------------------------------------------------
  Structure used by the PPP client to pass values for options to include
  in the C-Req.
  1. Re: ip_ver_capability and reqaddr_type.
  - ip_ver_capability is used to indicate the capability of the UE stack
  (i.e. whether only IPv4 or IPv6 are supported or both). For now
  VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6 should be passed for this.

  - req_addr_type is used to request an addr type (IPv4, IPv6, or both). 
  For now, VSNCP_3GPP2_PDN_TYPE_IPV4 or VSNCP_3GPP2_PDN_TYPE_IPV6 should be passed for 
  this (but not VSNCP_3GPP2_PDN_TYPE_IPV4_IPV6).

  2. If attach_type is initial, zero address(es) must be passed. If 
  attach_type is handover, existing addresses must be passed.
  The options listed below are required per X.P0057
---------------------------------------------------------------------------*/
typedef struct vsncp_3gpp2_local_opts_val_s
{
  uint8 pdn_id;                           /* REQD: 0x00-0x0e               */
  uint8 apn[VSNCP_3GPP2_APN_MAX_VAL_LEN]; /* REQD: APN Value if len > 0    */
  uint8 apn_len;                          /* REQD: APN Value len, 0 ok     */
  vsncp_3gpp2_pdn_type ip_ver_capability; /* REQD: IPV4, IPV6 or IPV4_V6   */
  vsncp_3gpp2_pdn_type req_addr_type;     /* REQD: IPV4 or IPV6            */
  struct ps_in_addr ip4;                  /* REQD: per req_addr_type       */
  uint64 iid;                             /* REQD: per req_addr_type       */
  vsncp_3gpp2_pco_opts_type pco;          /* REQD: IP addr via NAS         */
  vsncp_3gpp2_attach_type attach_type;    /* REQD: initial, handover       */
  uint8 addr_alloc_cause;                 /* Ignore                        */
  struct ps_in_addr default_rtr_ip4;         /* REQD: for IPv4 and IPv4v6     */
  boolean apn_is_default;                 /* REQD: APN is default flag     */ 
} vsncp_3gpp2_local_opts_val_type;

/*---------------------------------------------------------------------------
  Callback function called when activity is seen for a PDN
  (either a VSNCP or VSNP packet is sent or received)
---------------------------------------------------------------------------*/
typedef void (*vsncp_3gpp2_pdn_activity_cb_f_ptr_type)
(
  void * data
);

/*---------------------------------------------------------------------------
  Structure supplied by client to indicate activity callback func and data
---------------------------------------------------------------------------*/
typedef struct vsncp_3gpp2_pdn_activity_cb_type_s
{
  vsncp_3gpp2_pdn_activity_cb_f_ptr_type func;
  void * data;
} vsncp_3gpp2_pdn_activity_cb_type;

/*---------------------------------------------------------------------------
  Structure used by the PPP client to pass configuration information
---------------------------------------------------------------------------*/
typedef struct ppp_vsncp_3gpp2_config_s
{
  vsncp_3gpp2_local_opts_val_type   option_vals;  /* option vals for C-Req */
  ps_iface_type                  * rx_iface_ptr;  /* REQD: assoc interface */
  ps_iface_type    * second_rx_iface_ptr;  /* second iface ptr for DUAL IP */
  vsncp_3gpp2_pdn_activity_cb_type activity_cb;   /* pdn activity callback */
} ppp_vsncp_3gpp2_config_type;


#endif /* PS_PPP_VSNCP_3GPP2_H */
