#ifndef SECIPS_H
#define SECIPS_H

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                        I P S E C   L A Y E R

                 D M S S   I P S E C   I N T E R F A C E

          I N I T I A L I Z A T I O N   F U N C T I O N   A N D

      A C C E S S   C O N T R O L   L I S T   D E C L A R A T I O N S


    Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/**
  @file  secips.h

  Provides an interface to the functions that are used <b>ONLY by internal 
  applications</b> to establish the IPsec tunnel using IKEv2.

  These functions are used for allocating the logical interface and for setting 
  the Secirity Policy, self/peer identity and authentication, and endpoint information, 
  among others.

  Externalized functions:
  None.
*/
  	
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/api/ipsec/secips.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/15   ks      API to reset NATT keep alive timer
10/07/15   ks      Src_port update to SA_info callback
04/15/14   nm      NULL APN Support
1/30/14    nm      Increase iface support to 16
10/07/13   JK      Add support for IKEv2 Trigger DPD API within IPSEC to be used by IPSEC
10/07/13   JK      ADDED new API  to support IPSEC SA Rekey trigger
07/08/13   nm      Abort Tunnel API addition
07/28/10   dm      Re-architectured IPsec API files
07/07/10   sw      (Tech Pubs) edited/added Doxygen comments and markup.
11/20/09   sp      Using new PS naming conventions.
09/22/09   nk      IPsec CMI Decoupling.
08/14/08   sp      Fix for CR 154340 - Added a new API, which can be
                   called by the application to clear the ID list.
08/01/08   sp      Support for mapping EAP-AKA Notification code
                   with PS_NET_DOWN_REASON code.
07/09/08   sp      Added support for IKEv2 Multiple Authentication.
05/14/07   sl      Added cb for IKE SA establishment failure
05/14/07   jg      Added secipsiface_set_selector().
04/25/07   jg      New secipsiface interface functions.
04/23/07   ssm/sl  IKEv2 support
11/23/06   ssm/sv  Manual Keying/Transport Mode support
04/20/05   ssm     Added Phase2 renegotiation timers and code cleanup
01/25/05   sv/ssm  Re-architectured IPsec to support multiple SA's and added
                   support for AH.
01/31/04   usb     Removed externalization of SEC ACL.
05/25/03   om      Created Module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_iface.h"
#include "ps_in.h"
#include "secapi.h"
#include "IPSec.h"
#include "secipssp.h"
#include "dsm.h"


/*===========================================================================

                     DATA TYPES AND DEFINITIONS

===========================================================================*/

#define SECIPSIFACE_IPSEC_MAX_IFACES 16

/* ---------- Not needed in the PDF - Start ---------- */
/** @cond */

#define SECIPSIFACE_IPSEC_MAX_FLOWS  4
#define SECIPSIFACE_MAX_DSCP_MAPS 10

#define SECIPS_CB_F_PTR_V 1000  
/* Introduced to address secips_cb_f_ptr interdependency  with iWLAN */

/* Network state type. */
typedef enum
{
  SECIPS_NET_STATE_DISABLED                   = 0,  /* &nbsp; */
  SECIPS_NET_STATE_DOWN                       = 1,  /* &nbsp; */
  SECIPS_NET_STATE_WAITING_FOR_ISAKMP         = 2,  /* &nbsp; */
  SECIPS_NET_STATE_ISAKMP_ESTABLISHED         = 3,  /* &nbsp; */
  SECIPS_NET_STATE_IP_CONFIGURING             = 4,  /* &nbsp; */
  SECIPS_NET_STATE_IP_ASSIGNED                = 5,  /* &nbsp; */
  SECIPS_NET_STATE_WAITING_FOR_SA             = 6,  /* &nbsp; */
  SECIPS_NET_STATE_UP                         = 7,  /* &nbsp; */
  SECIPS_NET_STATE_WAITING_FOR_SA_RELEASE     = 8,  /* &nbsp; */
  SECIPS_NET_STATE_WAITING_FOR_IP_RELEASE     = 9,  /* &nbsp; */
  SECIPS_NET_STATE_WAITING_FOR_ISAKMP_RELEASE = 10, /* &nbsp; */
  SECIPS_NET_STATE_MANUAL_SA_ASSIGNED         = 11, /* &nbsp; */

  /* IKEv2 related states */
  SECIPS_NET_STATE_WAITING_FOR_IKEV2          = 12, /* &nbsp; */
  SECIPS_NET_STATE_IKE_SA_INIT                = 13, /* &nbsp; */
  SECIPS_NET_STATE_IKE_SA_INIT_DONE           = 14, /* &nbsp; */
  SECIPS_NET_STATE_IKE_AUTH_REQ               = 15, /* &nbsp; */
  SECIPS_NET_STATE_IKE_AUTH_DONE              = 16, /* &nbsp; */
  SECIPS_NET_STATE_CHILD_SA_REQ               = 17, /* &nbsp; */
  SECIPS_NET_STATE_CHILD_SA_DONE              = 18  /* &nbsp; */
} secipsiface_net_state_type;

/* IPsec interface events. */
typedef enum
{
  SECIPSIFACE_MIN_EV           = 0,
  SECIPSIFACE_CHILD_SA_UP_EV   = 1,
  SECIPSIFACE_CHILD_SA_DOWN_EV = 2,
  SECIPSIFACE_MAX_EV
} secipsiface_event_enum_type;

/* ---------- Not needed in the PDF - End ---------- */
 /** @endcond */

/**
  @addtogroup ipsec_internal_ikev2_ds
  @{
*/

/** Holds the fields that are part of an Authentication Header.
 */ 
typedef struct
{
  uint8  next_header;  /**< Protocol of the next header. */
  uint8  payload_len;  /**< Size of the payload. */
  uint16 reserved;     /**< Reserved bytes (value is 0). */
  uint32 spi;          /**< Security parameter index. */
  uint32 seqno;        /**< Sequence number. */
  uint8  auth_data[SECIPSTYPES_AH_AUTH_DATA_SIZE];  /**< Authentication data. */
}secips_ah_hdr;

/**
   @}
*/ 
/* end_group ipsec_internal_ikev2_ds */

/* ---------- Not needed in the PDF - Start ---------- */
  /** @cond */

/* Event type. */
typedef void (*secipsiface_cb_f_ptr_type)
(
  void*   ipsec_flow_cb_ptr,
  secipsiface_event_enum_type  event
);

typedef struct
{
  uint32 inner_dscp;
  uint32 outer_dscp;
  uint8  valid;
}secipsiface_in_out_dscp_map_type;

/* Security suite type.*/
typedef struct
{
  uint32                   ipsec_handle;  /* --Pointer to the SPD information. */
  secipssa_ipsec_sa_type*  esp_sa_ptr;
  secipssa_ipsec_sa_type*  ah_sa_ptr;
} secipsiface_sec_suite_type;

typedef struct
{
  secipssa_isakmp_sa_type*            isakmp_sa_ptr;
  secipsiface_sec_suite_type          sec_suite;
  ps_ip_addr_type                     tunnel_endpoint;
  secipssa_sa_encapsulate_enum_type   mode;
  secipsiface_net_state_type          state;
  secipsiface_cb_f_ptr_type           secips_cb_f_ptr;
  ps_iface_net_down_reason_type       net_down_reason;
  secipsiface_in_out_dscp_map_type    dscp_mapping[SECIPSIFACE_MAX_DSCP_MAPS];
  uint32                              curr_dscp_map_idx;
} secipsiface_ipsec_cb_type;

typedef struct
  {
	void*							handle;   /* Stores the flow pointer. */
	ps_iface_ioctl_type 			ioctl_name;
	ps_iface_ipfltr_subset_id_type	subset_id;
	secipssa_isakmp_sa_type*		isakmp_sa_ptr;
	secipsiface_sec_suite_type		sec_suite;
	secipsflow_cb_state_type		state;
  } secipsflow_ipsec_cb_type;
  
#define SECIPSIFACE_GET_IPSEC_CB_PTR(xx_iface) \
  ((secipsiface_ipsec_cb_type*)\
    (NULL == (xx_iface) ? NULL : (xx_iface)->client_data_ptr))


typedef enum
{
  SECIPS_TRIGGER_REKEY_IKE         = 1,  /* IKE Rekey  */
  SECIPS_TRIGGER_REKEY_IPSEC       = 2   /* IPSec Rekey */
}secips_trigger_rekey_type;

/*---------------------------------------------------------------------------
  The IPsec interface - needed by dssocki routing for now.
---------------------------------------------------------------------------*/
extern ps_iface_type ipsec_iface[SECIPSIFACE_IPSEC_MAX_IFACES];
extern secipsiface_ipsec_cb_type secips_iface_cb[SECIPSIFACE_IPSEC_MAX_IFACES];
extern secipsflow_ipsec_cb_type  secips_flow_cb[SECIPSIFACE_IPSEC_MAX_FLOWS];

/* ---------- Not needed in the PDF - End ---------- */
/** @endcond */

/**
@addtogroup ipsec_internal_ikev2_api

  @{ */

 /**
   Generates IPsec information that includes the interface information from a 
   given IPsec handle.
 
   @param *ipsec_handle [in] -- IPsec handle that is the IPsec filtering result.
   @param *pkt_info_ptr [in] -- IP packet information.
   @param *ipsec_info  [out] -- IPsec interface information.
   @param user_id		[in] -- Information provided by the user of the IPsec.
 
   @return
   secerrno_enum_type -- E_SUCCESS or E_FAILURE.
 
	  @dependencies
   None.
 */
 secerrno_enum_type secips_generate_ipsec_info
 (
   void 				   *ipsec_handle,
   ip_pkt_info_type 	   *pkt_info_ptr,
   ps_ipsec_info_type	   *ipsec_info,
   uint32				   user_id
 );

 /**
   Parses the Encapsulated Security Payload header of an incoming IP packet.
 
   @param **bp	   [in/out] -- Reference to the incoming IP packet.
   @param *offset	   [in] -- offset to the IP packet, from where ESP header is 
							   to be extracted.
   @param *ip_pkt_info [in] -- IP packet information.
 
   @return
   E_SUCCESS or a corresponding failure code.
 
   @dependencies
   None.
 */
 int secipsesp_hdr_parse
 (
   dsm_item_type	 **bp,
   uint16			 *offset,
   ip_pkt_info_type  *ip_pkt_info
 );
 
 /**
   Creates an Encapsulated Security Payload header for a given IP packet.
 
   @param *ipsec_sa_ptr    [in] -- Reference to the ESP SA.
   @param *next_header_ptr [in] -- Header next to the ESP header.
   @param **msg_ptr    [in/out] -- Reference to the IP packet on which the ESP 
								  header is to be created.
 
   @return
   E_SUCCESS or a corresponding failure code.
 
   @dependencies
   None.
 */
 int secipsesp_hdr_create
 (
  void				  *ipsec_sa_ptr,
  uint8 			  *next_header_ptr,
  dsm_item_type 	  **msg_ptr
 );

/**
  Creates an Authentication Header for a given IP Packet.

  @param *ipsec_sa_ptr [in] -- Reference to the AH SA.
  @param next_header   [in] -- Header next to the AH.
  @param ip_pkt_info   [in] -- IP packet information.
  @param **msg_ptr [in/out] -- Reference to the IP packet on which the AH is to 
                               be created.

  @return
  E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
int secipsah_hdr_create
(
  void*                   ipsec_sa_ptr,
  uint8                   next_header,
  ip_pkt_info_type        ip_pkt_info,
  dsm_item_type**         msg_ptr
);

/**
  Parses the Authentication Header of an incoming IP packet.

  @param **msg_ptr [in/out] -- Reference to the incoming IP packet.
  @param *offset       [in] -- Offset to the IP packet, from where the AH is to 
                               be extracted.
  @param *pkt_info_ptr [in] -- IP packet information.

  @return
  E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
int secipsah_hdr_parse
(
 dsm_item_type**         msg_ptr,
 uint16                 * offset,
 ip_pkt_info_type       * pkt_info_ptr
);

/*===========================================================================
FUNCTION          SECIPSIFACE_ALLOC_IFACE()
===========================================================================*/
/**
  Allocates an IPsec interface.

  @return
  Pointer to the IPsec interface on success.\n
  NULL on failure.

  @dependencies
  None.
*/
ps_iface_type* secipsiface_alloc_iface
(
 void
);

/*===========================================================================
FUNCTION               SECIPSIFACE_SET_ENDPOINT
===========================================================================*/
/**
  Sets the tunnel endpoint address.

  @param *ipsec_iface   [in] -- Pointer to the IPsec interface.
  @param *endpoint_ptr  [in] -- Endpoint's IPv4 or IPv6 address pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secipsiface_set_endpoint
(
  ps_iface_type*          ipsec_iface,
  const ps_ip_addr_type*  endpoint_ptr
);

/*===========================================================================
FUNCTION               secipsiface_set_phys_iface
===========================================================================*/
/**
  Sets the physical interface that is used for sending the IKE traffic.

  @param *ipsec_iface [in] -- Pointer to the IPsec interface.
  @param phys_iface	  [in] -- Reference to the physical interface ID.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secipsiface_set_phys_iface
(
  ps_iface_type*     ipsec_iface,
  dss_iface_id_type  phys_iface
);

/*===========================================================================
FUNCTION               secipsiface_set_protection
===========================================================================*/
/**
  Sets the IKE and IPsec parameters.

  @param *ipsec_iface      [in] -- Pointer to the IPsec interface.
  @param *protection_ptr   [in] -- IKE/IPsec parameters pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secipsiface_set_protection
(
  ps_iface_type*                  ipsec_iface,
  const secipssp_protection_type* protection_ptr
);

/*===========================================================================
FUNCTION               secipsiface_add_cfg_attr
===========================================================================*/
/**
  Adds an attribute to the policy to be requested during IKEv2.

  To add multiple instances of the attribute, this function must be called 
  multiple times. Attributes are sent in the order in which they are added. 
  Attribute values are optional and are only necessary to request a specific 
  value in the CFG_REQUEST payload.

  Expected values for each type are:
  - IPSEC_CFG_INTERNAL_IP4_ADDRESS -- struct ps_in_addr.
  - IPSEC_CFG_INTERNAL_IP4_NETMASK -- secips_cfg_attr_ip4_subnet_type.
  - IPSEC_CFG_INTERNAL_IP4_DNS -- struct ps_in_addr.
  - IPSEC_CFG_INTERNAL_IP4_NBNS --  struct ps_in_addr.
  - IPSEC_CFG_INTERNAL_ADDRESS_EXPIRY uint32.
  - IPSEC_CFG_INTERNAL_IP4_DHCP --  struct ps_in_addr.
  - IPSEC_CFG_APPLICATION_VERSION --  0 or more uint8.
  - IPSEC_CFG_INTERNAL_IP6_ADDRESS -- secips_cfg_attr_ip6_subnet_type.
  - IPSEC_CFG_INTERNAL_IP6_DNS -- struct ps_in6_addr.
  - IPSEC_CFG_INTERNAL_IP6_NBNS --  struct ps_in6_addr.
  - IPSEC_CFG_INTERNAL_IP6_DHCP --  struct ps_in6_addr.
  - IPSEC_CFG_INTERNAL_IP4_SUBNET --  secips_cfg_attr_ip4_subnet_type.
  - IPSEC_CFG_SUPPORTED_ATTRIBUTES -- 0 or more uint16.
  - IPSEC_CFG_INTERNAL_IP6_SUBNET --  secips_cfg_attr_ip6_subnet_type.
  - IPSEC_CFG_3GPP2_MIP4_HA --  struct ps_in_addr.
  - IPSEC_CFG_3GPP2_MIP4_HOA -- struct ps_in_addr.
  - IPSEC_CFG_3GPP2_MIP6_HA --  struct ps_in6_addr.
  - IPSEC_CFG_3GPP2_MIP6_HOA -- struct ps_in6_addr.

  @param *ipsec_iface    [in] -- Pointer to the IPsec interface.
  @param attr_type		 [in] -- Attribute type.
  @param *attr_value_ptr [in] -- Attribute value pointer. This is optional 
                                 and depends on the attribute type.
  @param attr_value_len	 [in] -- Attribute value length.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secipsiface_add_cfg_attr
(
  ps_iface_type*        ipsec_iface,
  IPSecConfigAttrEType  attr_type,
  const void*           attr_value_ptr,
  uint32                attr_value_len
);

/*===========================================================================
FUNCTION               secipsiface_set_self_id
===========================================================================*/
/**
  Sets the Initiator ID (IDi) payload to be sent during the authentication 
  exchange.\ Expected data types for each ID type are:
  - SECIPSPKTDEFS_IDENT_TYPE_IPV4_ADDR -- struct ps_in_addr.
  - SECIPSPKTDEFS_IDENT_TYPE_IPV6_ADDR -- struct ps_in6_addr.
  - SECIPSPKTDEFS_IDENT_TYPE_KEY_ID -- ASCII characters.
  - SECIPSPKTDEFS_IDENT_TYPE_FQDR -- ASCII characters.
  - SECIPSPKTDEFS_IDENT_TYPE_USER_FQDR -- ASCII characters.

  @param *ipsec_iface   [in] -- Pointer to the IPsec interface.
  @param id_type		[in] -- ID type.
  @param *id_value_ptr	[in] -- ID value pointer. The contents depend on 
                                the ID type.
  @param id_value_len	[in] -- ID value length. For string ID values, the length 
                                should not include the NULL terminator.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secipsiface_set_self_id
(
  ps_iface_type*                      ipsec_iface,
  secipspktdefs_ident_type_enum_type  id_type,
  const void*                         id_value_ptr,
  uint32                              id_value_len
);

/*===========================================================================
FUNCTION               secipsiface_set_peer_id
===========================================================================*/
/**
  Sets the Responder ID (IDr) payload to be sent during the authentication 
  exchange.\ Expected data types for each ID type are:
  - SECIPSPKTDEFS_IDENT_TYPE_IPV4_ADDR -- struct ps_in_addr.
  - SECIPSPKTDEFS_IDENT_TYPE_IPV6_ADDR -- struct ps_in6_addr.
  - SECIPSPKTDEFS_IDENT_TYPE_KEY_ID -- ASCII characters.
  - SECIPSPKTDEFS_IDENT_TYPE_FQDR -- ASCII characters.
  - SECIPSPKTDEFS_IDENT_TYPE_USER_FQDR -- ASCII characters.

  @param *ipsec_iface   [in] -- Pointer to the IPsec interface.
  @param id_type		[in] -- ID type.
  @param *id_value_ptr	[in] -- ID value pointer.
  @param id_value_len	[in] -- ID value length. For string ID values, the length 
                                should not include the NULL terminator.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secipsiface_set_peer_id
(
  ps_iface_type*                      ipsec_iface,
  secipspktdefs_ident_type_enum_type  id_type,
  const void*                         id_value_ptr,
  uint32                              id_value_len
);

/*===========================================================================
FUNCTION               secipsiface_set_auth_param
===========================================================================*/
/**
  Sets the authentication data.\ Expected values for each authentication type are:
  - SECIPSSP_AUTH_TYPE_PSK           -- secipssp_psk_param_data_type.
  - SECIPSSP_AUTH_TYPE_EAP_SIM_3GPP  -- NULL.
  - SECIPSSP_AUTH_TYPE_EAP_AKA       -- NULL.

  @param *ipsec_iface     [in] -- Pointer to the IPsec interface.
  @param auth_type		  [in] -- Authentication type.
  @param *auth_param_data [in] -- Authentication data.
  @param auth_param_len	  [in] -- Authentication data length.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secipsiface_set_auth_param
(
  ps_iface_type*                        ipsec_iface,
  secipssp_auth_type_param_data_type    auth_type,
  const void*                           auth_param_data,
  uint32                                auth_param_len
);

/*===========================================================================
FUNCTION               secipsiface_set_id_info
===========================================================================*/
/**
  Sets the IDi and IDr payloads that are used during the authentication exchange 
  and during the computation of authentication data.

  Expected data types for id_type and peer_id_type are:
  - SECIPSPKTDEFS_IDENT_TYPE_IPV4_ADDR -- struct ps_in_addr.
  - SECIPSPKTDEFS_IDENT_TYPE_IPV6_ADDR -- struct ps_in6_addr.
  - SECIPSPKTDEFS_IDENT_TYPE_KEY_ID    -- ASCII characters.
  - SECIPSPKTDEFS_IDENT_TYPE_FQDR      -- ASCII characters.
  - SECIPSPKTDEFS_IDENT_TYPE_USER_FQDR -- ASCII characters.

  Expected values for auth_type are:
  - SECIPSSP_AUTH_TYPE_PSK           -- secipssp_psk_param_data_type.
  - SECIPSSP_AUTH_TYPE_EAP_SIM_3GPP  -- NULL.
  - SECIPSSP_AUTH_TYPE_EAP_AKA       -- NULL.
  - id_auth_ptr->id_type           -- Type of Self ID.
  - id_auth_ptr->id_value_ptr      -- Pointer to the ID data. The contents
                                      depend on the type of ID.
  - id_auth_ptr->id_value_len      -- Length of ID data. For string ID values, 
                                      the length should not include the NULL 
                                      terminator.
  - id_auth_ptr->peer_id_type      -- Type of peer ID.
  - id_auth_ptr->peer_id_value_ptr -- pointer to the ID data. The contents 
                                      depend on the type of ID.
  - id_auth_ptr->peer_id_value_len -- Length of ID data. For string ID values, 
                                      the length should not include the NULL 
                                      terminator.
  - id_auth_ptr->auth_type         -- Type of authentication.
  - id_auth_ptr->auth_param_ptr    -- Pointer to the authentication data.
                                       The contents depends on the type
                                      of authentication.
  - id_auth_ptr->auth_param_len    -- Length of the ID data. For string ID
                                       values, the length should not
                                      include the NULL terminator.

  @param *ipsec_iface [in] -- Pointer to the IPsec interface.
  @param id_auth_ptr  [in] -- ID information pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secipsiface_set_id_info
(
  ps_iface_type*          ipsec_iface,
  secipssp_id_auth_type*  id_auth_ptr
);

/*===========================================================================
FUNCTION               secipsiface_clear_id_info
===========================================================================*/
/**
  Clears the entire ID list.

  @param *ipsec_iface [in] -- Pointer to the IPsec interface.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secipsiface_clear_id_info
(
  ps_iface_type*          ipsec_iface
);

/*===========================================================================
FUNCTION               secipsiface_set_segw_fqdn
===========================================================================*/
/**
  Sets the Fully Qualified Domain Name (FQDN) of the security gateway.\ 
  If the gateway's FQDN is set, compare the FQDN with the common name in the 
  certificate.

  @param *ipsec_iface [in] -- Pointer to the IPsec interface.
  @param *fqdn_ptr    [in] -- FQDN of the security gateway.
  @param fqdn_len     [in] -- Length of the FQDN.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secipsiface_set_segw_fqdn
(
  ps_iface_type*          ipsec_iface,
  uint8*                  fqdn_ptr,
  uint32                  fqdn_len
);

/*===========================================================================
FUNCTION               secipsiface_get_cfg_attr
===========================================================================*/
/**
  Gets the CFG attributes value negotiated during IKEv2.\ 
  For attributes with multiple values, all values are returned as an array.

  @param ipsec_iface	[in]     -- Pointer to the IPsec interface.
  @param attr_type		[in]     -- CFG attribute type.
  @param attr_value_ptr	[out]    -- Attribute value pointer.
  @param attr_len_ptr	[in/out] -- Attribute value length pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  IPsec interface must be in the PS_IFACE_UP state.
*/
secerrno_enum_type secipsiface_get_cfg_attr
(
  ps_iface_type*             ipsec_iface,
  IPSecConfigAttrEType       attr_type,
  void*                      attr_value_ptr,
  uint32*                    attr_len_ptr
);

/*===========================================================================
FUNCTION               secipsiface_set_inbound_sa_endpoint
===========================================================================*/
/**
  Changes the SADB entry for an inbound IPsec Security Association to a new local
  address.

  This function is used only by mobile IP.

  @param *ipsec_iface		[in] -- Pointer to the IPsec interface.
  @param *endpoint_addr_ptr	[in] -- New endpoint address pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  IPsec interface must be in the PS_IFACE_UP state.
*/
secerrno_enum_type secipsiface_set_inbound_sa_endpoint
(
  ps_iface_type*             ipsec_iface,
  const ps_ip_addr_type*     endpoint_addr_ptr
);

/** 
   @}
*/ /* end_group ipsec_internal_ikev2_api */

/* ---------- Not needed in the PDF - Start ---------- */
/** @cond */

/*===========================================================================
FUNCTION BRING_UP_CMD()
===========================================================================*/
/*
  Establishes Security Association.

  @param *iface_ptr      -- Pointer to the transmitting interface.
  @param *user_data_ptr  -- Pointer to the IPSEFC information.

  @return
  None.

  @dependencies
  None.
*/
int secipsiface_bring_up_cmd
(
  ps_iface_type*       iface_ptr,
  void*                user_data_ptr
);

/*===========================================================================
FUNCTION TEAR_DOWN_CMD()
===========================================================================*/
/*
  Tears down the Security Association.

  @param *this_iface_ptr -- Pointer to the transmitting interface.
  @param *user_data_ptr  -- Pointer to the IPsec information.

  @return
  None.

  @dependencies
  None.
*/
int secipsiface_tear_down_cmd
(
  ps_iface_type*       this_iface_ptr,
  void*                user_data_ptr
);

/*===========================================================================
FUNCTION      SECIPSIFACE_SET_SET_SELECTOR
===========================================================================*/
/*
  Adds selector rules to the SPDB for this interface.

  This call is for testing purposes only and should not be used in iWLAN mode.

  @param *ipsec_iface -- ID of the IPsec interface.
  @param diretion     -- Direction for the IPsec SA.
  @param selector_ptr -- Selector to insert into the SPDB.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  ipsec_iface must be down.
*/
secerrno_enum_type secipsiface_set_selector
(
  ps_iface_type*                           ipsec_iface,
  secipssp_direction_enum_type				direction,
  const secipssp_protection_selector_type* selector_ptr
);

/*===========================================================================
FUNCTION      SECIPSIFACE_CREATE_CHILD_CA
===========================================================================*/
/*
  Creates a child Security Association based on the flow control block pointer.

  This function is intended for IWLAN use.

  @param *ipsec_flow_cb_ptr -- IPsec flow control block pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  ipsec_iface must be up.
*/
secerrno_enum_type secipsiface_create_child_sa
(
  secipsflow_ipsec_cb_type*  ipsec_flow_cb_ptr
);

/*===========================================================================
FUNCTION      SECIPSIFACE_DELETE_CHILD_SA
===========================================================================*/
/*
  Deletes a child Security Association based on an ipsec_cb_ptr.

  @param *ipsec_flow_cb_ptr -- IPsec flow control block pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  ipsec_iface must be up.
*/
secerrno_enum_type secipsiface_delete_child_sa
(
  secipsflow_ipsec_cb_type*  ipsec_flow_cb_ptr
);

/*===========================================================================
FUNCTION SECIPSIFACE_ANOTHER_AUTH_CB()
===========================================================================*/
/*
  Sets the interface net state to SECIPS_NET_STATE_IKE_AUTH_REQ.

  @param *iface_ptr -- Pointer to the IPsec interface.
  @param new_state  -- SECIPS_NET_STATE_IKE_AUTH_REQ.

  @return
  None.

  @dependencies
  None.
*/
void secipsiface_another_auth_cb
(
  ps_iface_type*              iface_ptr,
  secipsiface_net_state_type  new_state
);

/*===========================================================================
FUNCTION SECIPSIFACE_GET_NEXT_IFACE()
===========================================================================*/
/*
  Returns the physical interface associated with the IPsec interface.

  @param iface_ptr  -- Pointer to the IPsec interface.

  @return
  Pointer to the physical interface.

  @dependencies
  None.
*/
ps_iface_type * secipsiface_get_next_iface
(
  ps_iface_type* iface_ptr
);


/*
  Returns the Gateway's IP address.

  @param *iface_ptr -- Pointer to the IPsec interface.
  @param *pdg_addr -- Gateway's IP Address

  @return
  Gateway's IP address.

  @dependencies
  None.
*/
void secipsiface_get_pdg_addr
(
  ps_iface_type*   iface_ptr,
  ps_ip_addr_type* pdg_addr
);

/* ---------- Not needed in the PDF - End ---------- */
  /** @endcond */

/*===========================================================================

                     IWALAN CHANGES 

===========================================================================*/
 
#define SECIPS_MAX_IKE_SA_CNT           8
#define SECIPS_MAX_IPSEC_SA_CNT         16
#define SECIPS_MAX_TS_CNT               4

#define SECIPS_IPSEC_MAX_FLOWS  4
#define SECIPS_MAX_DSCP_MAPS 10

#define SECIPS_MAX_IKE_HANDLE 8

/**
  Indicates the TS IP version type.
*/
typedef enum
{
  TS_IP_V4_ADDR = 7,  /**< IPv4 address range. */
  TS_IP_V6_ADDR = 8   /**< IPv6 address range. */
} ts_ip_addr_enum_type;


typedef struct
{
  ip_version_enum_type  ip_vsn;
  uint8                 protocol;
  uint16                start_port;
  uint16                end_port;
  union
  {
    uint32             ipv4_addr;
    struct ps_in6_addr ipv6_addr;
  } start_address;
  union
  {
    uint32             ipv4_addr;
    struct ps_in6_addr ipv6_addr;
  } end_address;
} secips_ts_type;

/* Traffic Selector List */
typedef struct
{
  uint8           ts_count;
  secips_ts_type  ts[SECIPS_MAX_TS_CNT];
} secips_ts_list_type;

/* IPSEC Security Association data that is returned to IKE client */
typedef struct
{
  uint32                            spi_in;
  uint32                            spi_out;
  secipssa_sa_protocol_enum_type    type;
  secipssa_sa_encapsulate_enum_type encapsulation_mode;
  ps_ip_addr_type                   dest_addr;
  ps_ip_addr_type                   local_addr;

  /* Crypto material */
  secapi_algo_enum_type             hash_algo;
  uint16                            hash_size;
  uint8                             hash_key_in[SECIPSTYPES_HASH_MAX_SIZE];
  uint8                             hash_key_out[SECIPSTYPES_HASH_MAX_SIZE];

  secapi_algo_enum_type             crypto_algo;
  uint16                            crypto_key_size;
  uint8                             crypto_key_in[SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE];
  uint8                             crypto_key_out[SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE];

  uint16                            iv_size;
  uint8                             iv[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];

  /* NAT-T-specific data. */
  boolean                           is_udp_encaps;
  ps_ip_addr_type                   natoa_i;
  ps_ip_addr_type                   natoa_r;

  /* Config Attributes */
  uint32                            cfg_attr_cnt;
  secips_cfg_attr_type              cfg_attr[IPSEC_CFG_MAX_ATTRS];

  /* Traffic Selectors */
  secips_ts_list_type               ts_list;

  /* Traffic Selector Responder */
  secips_ts_list_type               tsr;

  /* idr value received from netwrok when NULL APN set */
  secipssa_sa_identification        idr;
  /* AES mode uses*/
  secipssa_aes_mode_type            aes_mode;
  uint16                            src_port; /* to store IKE Source prot number */
} secips_ipsec_sa_type;

/* IKEv2 events. */
typedef enum
{
  SECIPS_MIN_EV           = 0,
  SECIPS_CHILD_SA_UP_EV   = 1,
  SECIPS_CHILD_SA_DOWN_EV = 2,
  SECIPS_IKE_SA_REKEY     = 3,
  SECIPS_CHILD_SA_REKEY   = 4, 
  SECIPS_MAX_EV
} secips_event_enum_type;

/* IKE Failure Cause Codes. */
typedef enum
{
  SECIPS_IKE_FAILURE_CAUSE_NONE           = 0,
  SECIPS_IKE_FAILURE_CAUSE_IKE_REKEY      = 1,
  SECIPS_IKE_FAILURE_CAUSE_IPSEC_REKEY    = 2,
  SECIPS_IKE_FAILURE_CAUSE_GW_UNREACHABLE = 3,
  SECIPS_IKE_FAILURE_CAUSE_EAP_AUTH       = 4, 
  SECIPS_IKE_FAILURE_CAUSE_IKE_AUTH       = 5,
  SECIPS_IKE_FAILURE_CAUSE_IKE_SA         = 6,
  SECIPS_IKE_FAILURE_CAUSE_IPSEC_SA       = 7,
  SECIPS_IKE_FAILURE_CAUSE_IKE_PROTOCOL   = 8,
  SECIPS_IKE_FAILURE_CAUSE_CONFIG_FAILURE = 9,
  SECIPS_IKE_FAILURE_CAUSE_IKE_AUTH_PDN_CONN_REJECT = 10,
  SECIPS_IKE_FAILURE_CAUSE_IKE_AUTH_PDN_CONN_REJECT_NO_NOTIFY = 11,
  SECIPS_IKE_FAILURE_CAUSE_IKE_AUTH_MAX_CONN_REACHED   = 12,
  SECIPS_IKE_FAILURE_CAUSE_IKE_AUTH_CERT_INVALID = 13,
  SECIPS_IKE_FAILURE_CAUSE_IKE_AUTH_CERT_EXPIRY = 14,
  SECIPS_IKE_FAILURE_CAUSE_NW_INIT_CONN_DOWN = 15,
  SECIPS_IKE_FAILURE_CAUSE_DPD_FAILURE = 16,
  SECIPS_IKE_FAILURE_CAUSE_MAX
} secips_ike_failure_cause_enum_type;

/**
   @}
*/ 
/* end_group ipsec_internal_ikev2_ds */

/* Event type. */
typedef void (*secips_cb_f_ptr_type)
(
  void*                               ike_sa_handle,
  void*                               child_sa_handle,
  secips_event_enum_type              event,
  secips_ike_failure_cause_enum_type  cause,
  secips_ipsec_sa_type*               ipsec_sa_info_ptr,
  secipssa_sa_identification          idr,
  secipspktdefs_ikev2_notify_enum_type ike_srv_notify
);

/* IKE and IPsec Handle types */
typedef void* secips_ike_sa_handle_type;
typedef void* secips_child_sa_handle_type;


/*===========================================================================
FUNCTION          SECIPS_NEW_SESSION()
===========================================================================*/
/**
  Creates a new IKE handle.

  @return
  Returns secerrno_enum_type, which is the security services standard error
  number. A possible error condition is: \n
  - E_NOT_AVAILABLE -- All IKE sessions are already in use.

  @dependencies
  None.
*/
secerrno_enum_type  secips_new_session
( 
  secips_ike_sa_handle_type*  ike_sa_handle_ptr,
  secips_cb_f_ptr_type ike_cb_fn_ptr
);

/*===========================================================================
FUNCTION          SECIPS_BRING_UP_TUNNEL()
===========================================================================*/
/**
  IKE Tunnel is brought up.

  @return
  Returns secerrno_enum_type, which is the security services standard error
  number.

  @dependencies
  Tunnel parameters that include gateway address, protection, ID info should
  have been set prior to calling this API.
*/
secerrno_enum_type secips_bring_up_tunnel
(
  secips_ike_sa_handle_type ike_sa_handle
);

/*===========================================================================
FUNCTION          SECIPS_TEAR_DOWN_TUNNEL()
===========================================================================*/
/**
  IKE Tunnel is brought down.

  @return
  Returns secerrno_enum_type, which is the security services standard error
  number.

  @dependencies
*/
secerrno_enum_type secips_tear_down_tunnel
(
  secips_ike_sa_handle_type ike_sa_handle
);

/*===========================================================================
FUNCTION          SECIPS_ABORT_TUNNEL()
===========================================================================*/
/**
  IKE Tunnel is aborted without sending Informational message to Server.

  @return
  Returns secerrno_enum_type, which is the security services standard error
  number.

  @dependencies
*/
secerrno_enum_type secips_abort_tunnel
(
  secips_ike_sa_handle_type ike_sa_handle
);

/*===========================================================================
FUNCTION               SECIPS_SET_SEGW_ADDR
===========================================================================*/
/**
  Sets the tunnel endpoint address.

  @param ike_sa_handle [in]  -- Handle to IKE session
  @param *endpoint_ptr [in]  -- Endpoint's IPv4 or IPv6 address pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secips_set_segw_addr
(
  secips_ike_sa_handle_type  ike_sa_handle,
  const ps_ip_addr_type*     endpoint_ptr
);


/*===========================================================================
FUNCTION               SECIPS_SET_SEGW_FQDN
===========================================================================*/
/**
  Sets the Fully Qualified Domain Name (FQDN) of the security gateway.\ 
  If the gateway's FQDN is set, compare the FQDN with the common name in the 
  certificate.

  @param ike_sa_handle [in] -- Handle to IKE session
  @param *fqdn_ptr     [in] -- FQDN of the security gateway.
  @param fqdn_len      [in] -- Length of the FQDN.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secips_set_segw_fqdn
(
  secips_ike_sa_handle_type   ike_sa_handle,
  uint8*                      fqdn_ptr,
  uint32                      fqdn_len
);


/*===========================================================================
FUNCTION               SECIPS_SET_PHYS_IFACE
===========================================================================*/
/**
  Sets the physical interface that is used for sending the IKE traffic.

  @param ike_sa_handle [in] -- Handle to IKE session
  @param phys_iface    [in] -- Reference to the physical interface ID.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secips_set_phys_iface
(
  secips_ike_sa_handle_type   ike_sa_handle,
  dss_iface_id_type           phys_iface
);

/*===========================================================================
FUNCTION               SECIPS_SET_PROTECTION
===========================================================================*/
/**
  Sets the IKE and IPsec parameters for SA negotiation.

  @param  ike_sa_handle  [in] -- Handle to IKE session
  @param *protection_ptr [in] -- IKE/IPsec parameters pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secips_set_protection
(
  secips_ike_sa_handle_type        ike_sa_handle,
  const secipssp_protection_type*  protection_ptr
);

/*===========================================================================
FUNCTION               SECIPS_ADD_CFG_ATTR
===========================================================================*/
/**
  Adds an attribute to the policy to be requested during IKEv2.

  To add multiple instances of the attribute, this function must be called 
  multiple times. Attributes are sent in the order in which they are added. 
  Attribute values are optional and are only necessary to request a specific 
  value in the CFG_REQUEST payload.

  Expected values for each type are:
  - IPSEC_CFG_INTERNAL_IP4_ADDRESS -- struct ps_in_addr.
  - IPSEC_CFG_INTERNAL_IP4_NETMASK -- secips_cfg_attr_ip4_subnet_type.
  - IPSEC_CFG_INTERNAL_IP4_DNS -- struct ps_in_addr.
  - IPSEC_CFG_INTERNAL_IP4_DHCP --  struct ps_in_addr.
  - IPSEC_CFG_INTERNAL_IP6_ADDRESS -- secips_cfg_attr_ip6_subnet_type.
  - IPSEC_CFG_INTERNAL_IP6_DNS -- struct ps_in6_addr.
  - IPSEC_CFG_INTERNAL_IP6_DHCP --  struct ps_in6_addr.
  - IPSEC_CFG_INTERNAL_IP4_SUBNET --  secips_cfg_attr_ip4_subnet_type.
  - IPSEC_CFG_INTERNAL_IP6_SUBNET --  secips_cfg_attr_ip6_subnet_type.

  @param ike_sa_handle   [in] -- Handle to IKE session
  @param attr_type       [in] -- Attribute type.
  @param *attr_value_ptr [in] -- Attribute value pointer. This is optional 
                                 and depends on the attribute type.
  @param attr_value_len  [in] -- Attribute value length.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secips_add_cfg_attr
(
  secips_ike_sa_handle_type  ike_sa_handle,
  IPSecConfigAttrEType        attr_type,
  const void*                 attr_value_ptr,
  uint32                      attr_value_len
);

/*===========================================================================
FUNCTION               SECIPS_GET_CFG_ATTR
===========================================================================*/
/**
  Gets the CFG attributes value negotiated during IKEv2.\ 
  For attributes with multiple values, all values are returned as an array.

  @param ike_sa_handle   [in]    -- Handle to IKE session
  @param attr_type       [in]    -- CFG attribute type.
  @param attr_value_ptr [out]    -- Attribute value pointer.
  @param attr_len_ptr   [in/out] -- Attribute value length pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secips_get_cfg_attr
(
  secips_ike_sa_handle_type   ike_sa_handle,
  IPSecConfigAttrEType        attr_type,
  void*                       attr_value_ptr,
  uint32*                     attr_len_ptr
);

/*===========================================================================
FUNCTION               SECIPS_SET_ID_INFO
===========================================================================*/
/**
  Sets the IDi and IDr payloads that are used during the authentication exchange 
  and during the computation of authentication data.

  Expected data types for id_type and peer_id_type are:
  - SECIPSPKTDEFS_IDENT_TYPE_IPV4_ADDR -- struct ps_in_addr.
  - SECIPSPKTDEFS_IDENT_TYPE_IPV6_ADDR -- struct ps_in6_addr.
  - SECIPSPKTDEFS_IDENT_TYPE_KEY_ID    -- ASCII characters.
  - SECIPSPKTDEFS_IDENT_TYPE_FQDR      -- ASCII characters.
  - SECIPSPKTDEFS_IDENT_TYPE_USER_FQDR -- ASCII characters.

  Expected values for auth_type are:
  - SECIPSSP_AUTH_TYPE_PSK           -- secipssp_psk_param_data_type.
  - SECIPSSP_AUTH_TYPE_EAP_SIM_3GPP  -- NULL.
  - SECIPSSP_AUTH_TYPE_EAP_AKA       -- NULL.
  - id_auth_ptr->id_type           -- Type of Self ID.
  - id_auth_ptr->id_value_ptr      -- Pointer to the ID data. The contents
                                      depend on the type of ID.
  - id_auth_ptr->id_value_len      -- Length of ID data. For string ID values, 
                                      the length should not include the NULL 
                                      terminator.
  - id_auth_ptr->peer_id_type      -- Type of peer ID.
  - id_auth_ptr->peer_id_value_ptr -- pointer to the ID data. The contents 
                                      depend on the type of ID.
  - id_auth_ptr->peer_id_value_len -- Length of ID data. For string ID values, 
                                      the length should not include the NULL 
                                      terminator.
  - id_auth_ptr->auth_type         -- Type of authentication.
  - id_auth_ptr->auth_param_ptr    -- Pointer to the authentication data.
                                       The contents depends on the type
                                      of authentication.
  - id_auth_ptr->auth_param_len    -- Length of the ID data. For string ID
                                       values, the length should not
                                      include the NULL terminator.

  @param ike_sa_handle  [in] -- Handle to IKE session
  @param id_auth_ptr    [in] -- ID information pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secips_set_id_info
(
  secips_ike_sa_handle_type   ike_sa_handle,
  secipssp_id_auth_type*      id_auth_ptr
);

/*===========================================================================
FUNCTION               SECIPS_CLEAR_ID_INFO
===========================================================================*/
/**
  Clears the entire ID list.

  @param ike_sa_handle [in] -- Handle to IKE session

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
secerrno_enum_type secips_clear_id_info
(
  secips_ike_sa_handle_type   ike_sa_handle
);

/** 
   @}
*/ /* end_group ipsec_internal_ikev2_api */


/*===========================================================================
FUNCTION      SECIPS_CREATE_CHILD_CA
===========================================================================*/
/*
  Creates a child Security Association based on the flow control block pointer.

  This function is intended for IWLAN use.

  @param *ipsec_flow_cb_ptr -- IPsec flow control block pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  ipsec_iface must be up.
*/
secerrno_enum_type secips_create_child_sa
(
   secips_ike_sa_handle_type        ike_sa_handle,
   secips_child_sa_handle_type*     child_sa_handle,
   const secipssp_protection_type*  protection_ptr
);

/*===========================================================================
FUNCTION      SECIPS_DELETE_CHILD_SA
===========================================================================*/
/*
  Deletes a child Security Association based on an ipsec_cb_ptr.

  @param *ipsec_flow_cb_ptr -- IPsec flow control block pointer.

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  ipsec_iface must be up.
*/
secerrno_enum_type secips_delete_child_sa
(
  secips_child_sa_handle_type  child_sa_handle
);

/*===========================================================================
FUNCTION      SECIPS_GET_IPSEC_SA
===========================================================================*/
/*
  Returns the IPsec SA information.

  @param ike_sa_handle [in] -- Handle to IKE session
  @param child_sa_handle [in] -- Handle to CHILD SA

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None
*/
secerrno_enum_type secips_get_ipsec_sa
(
  secips_ike_sa_handle_type     ike_sa_handle,
  secips_child_sa_handle_type   child_sa_handle,
  secips_ipsec_sa_type**         sa_info
);

/*===========================================================================
FUNCTION      SECIPS_TRIGGER_DPD
===========================================================================*/
/*
  Trigger Dead Peer Detection message.

  @param ike_sa_handle [in] -- Handle to IKE session

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  ipsec tunnel of this handle must be up.
*/
secerrno_enum_type secips_trigger_dpd
(
  secips_ike_sa_handle_type  ike_sa_handle
);

/*===========================================================================
FUNCTION      SECIPS_TRIGGER_REKEY
===========================================================================*/
/*
  Trigger IKE or IPsec Rekey.

  @param ike_sa_handle [in] -- Handle to IKE session
  @param rekey_type    [in] -- IPsec or IKE Rekey
  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  ipsec tunnel of this handle must be up.
*/
secerrno_enum_type secips_trigger_rekey
(
  secips_ike_sa_handle_type  ike_sa_handle, 
  secips_trigger_rekey_type  rekey_type
);

/*===========================================================================
FUNCTION      SECIPS_RESET_NATT_KEEPALIVE_TIMER
===========================================================================*/
/*
  Trigger IKE or IPsec Rekey.

  @param ike_sa_handle [in] -- Handle to IKE session
  @param keepalive_secs    [in] -- keepAlive time

  @return
  secerrno_enum_type -- E_SUCCESS or a corresponding failure code.

  @dependencies
  ipsec tunnel of this handle must be up.
*/
secerrno_enum_type secips_reset_natt_keepalive_timer
(
  secips_ike_sa_handle_type  ike_sa_handle,
  uint32 keepalive_secs
);

#endif /* SECIPS_H */

