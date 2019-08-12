#ifndef SECIPSSP_H
#define SECIPSSP_H

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                        I P S E C   L A Y E R

         E X T E R N A L   A P I   A N D   D E F I N I T I O N S

    Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/**
  @file secipssp.h
  
  Provides an interface to the internal applications for IKEv2 security association. 
  These functions are used to establish an IPSEC tunnel which can further support the communication between the hosts 
  using their security policies.
  
 */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secipssdb.h_v   1.5   30 Aug 2003 20:04:06   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/api/ipsec/secipssp.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/17/16   ks      IPSec EAP failure with NULL APN fix
10/07/15   ks      Dynamic_src port for IKE negotiation support
09/29/15   nm      SHA256 support
04/28/14   nm      EAP_AKA FRA and pseudonym support
04/15/14   nm      NULL APN Support
10/31/13   nm      DHgroup 5 and 14 support
07/28/10   dm      Re-architectured IPsec API files
07/07/10   sw     (Tech Pubs) Edited/added Doxygen comments and markup.
06/11/10   ml      Removed seccrypt.h from header definitions.
09/22/09   nk      IPsec CMI Decoupling.
07/16/08   sp      Support for using configured DPD timer value.
07/09/08   sp      Added support for IKEv2 Multiple Authentication.
04/25/07   jjg     Interface changes for IWLAN and IKEv2
04/23/07   ssm/sl  IKEv2 support
02/26/07   ssm     Changes to support multiple selectors per SP
11/23/06   ssm/sv  Manual Keying/Transport Mode support
09/29/06   ssm     Fixes to make lint happy
05/17/05   sv      Added ipfliter handle to policy type.
04/20/05   ssm     Renamed secipssp_init to secipssp_new
01/25/05   sv/ssm  Re-architectured IPSEC to support multiple SA's and added
                   support for AH.
10/08/04   sv      Added support for creating multiple entries in SPD.
10/23/00   et      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "secerrno.h"
#include "IPSec.h"
#include "secapi.h"
#include "dssocket_defs.h"
#include "ps_iface.h"
#include "ps_iface_ipfltr.h"
#include "ps_pkt_info.h"
#include "dsm.h"

#define SECIPS_MAX_TUNNELS 2
#define SECIPS_MAX_IFACE_LIST (SECIPS_MAX_TUNNELS +2)

/* ---------- Not needed in the PDF - Start ---------- */
/** @cond */

#define SECIPSTYPES_ESP_AUTH_DATA_SIZE    12
#define SECIPSTYPES_ESP_SEQ_NUM_SIZE      4
#define SECIPSTYPES_IPSEC_SPI_SIZE      4
#define SECIPSTYPES_ESP_IV_SIZE           16
#define SECIPSTYPES_ESP_HDR_SIZE \
           (SECIPSTYPES_IPSEC_SPI_SIZE + SECIPSTYPES_ESP_SEQ_NUM_SIZE)
#define SECIPSTYPES_ESP_ALIGN_VALUE      16 
#define SECIPSTYPES_MAX_PAD_LEN          (SECIPSTYPES_ESP_ALIGN_VALUE - 1)
#define SECIPSTYPES_ESP_TRAILER_SIZE     2
#define SECIPSTYPES_MAX_ESP_HDR_SIZE \
           (SECIPSTYPES_ESP_HDR_SIZE + \
            SECIPSTYPES_ESP_IV_SIZE  + \
            SECIPSTYPES_MAX_PAD_LEN + \
            SECIPSTYPES_ESP_TRAILER_SIZE + \
            SECIPSTYPES_ESP_AUTH_DATA_SIZE)
#define SECIPSTYPES_GATEWAY_NATT_PORT    4500
#define SECIPSTYPES_AH_AUTH_DATA_SIZE    12
#define SECIPSTYPES_AH_HDR_SIZE  (SECIPSTYPES_AH_AUTH_DATA_SIZE + 12)
#define SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE 32
#define SECIPSTYPES_HASH_MAX_SIZE \
         (MAX(SECAPI_HSH_MD5_DIGEST_SIZE,SECAPI_HSH_SHA256_DIGEST_SIZE))
#define SECIPSTYPES_IKEV2_SPI_SIZE      8
#define SECIPSPKTDEFS_MAX_ATTRIBUTES 10
#define SECIPSPKTDEFS_NONCE_MAX_SIZE          256
#define SECIPSPKTDEFS_MAX_TRANSFORMS 16
#define SECIPSSA_NUM_MAX_CHILD_SA   16
#define SECIPSSP_PSK_KEY_MAX_SIZE    32
#define SECIPSSP_MD5_PASSWORD_MAX_SIZE    255

/* ---------- Not needed in the PDF - End ---------- */
/** @endcond */

 /**
   @addtogroup ipsec_internal_ikev2_ds
 
   @{ */
 
 /* =================================================================
	  I P S E C   K e y   M a n a g e m e n t	O p t i o n s
 ================================================================= */
 /**
  * Key Managment Type
  */ 
 typedef enum
 {
   SECIPSSP_KEY_MGMT_UNDEFINED	 = IPSEC_KEY_MGMT_UNDEFINED,	/**< -- Initialization value */
   SECIPSSP_KEY_MGMT_IKEV1		 = IPSEC_KEY_MGMT_IKEV1,		/** &nbsp; */
   SECIPSSP_KEY_MGMT_IKEV2		 = IPSEC_KEY_MGMT_IKEV2,		/** &nbsp; */
   SECIPSSP_KEY_MGMT_MANUAL 	 = IPSEC_KEY_MGMT_MANUAL,		/**< -- Manual keying */
   SECIPSSP_KEY_MGMT_COUNT		/**< -- Maximum count */
 } secipssp_key_mgmt_enum_type;
 
 
 /**
  * Encapsulation method
  */
 typedef enum
 {
   SECIPSSP_ENCAPSULATE_UNDEFINED,	/** &nbsp; */
   SECIPSSP_ENCAPSULATE_TUNNEL,		/** &nbsp; */
   SECIPSSP_ENCAPSULATE_TRANSPORT	/** &nbsp; */
 } secipssp_encapsulate_enum_type;	
 
 /**
  * Authentication Type
  */ 
 typedef enum
 {
   SECIPSSP_AUTH_TYPE_INVALID_PARAM = 0,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_PSK			= 1,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_HYBRID		= 2,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_SIM_3GPP	= 3,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_AKA		= 4,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_TLS		= 5,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_CERTIFICATE	= 6,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_MD5		= 7,	/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_AKA_CAVE	= 8,		/** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_AKA_SHA1  = 9,   /** &nbsp; */
   SECIPSSP_AUTH_TYPE_EAP_AKA_MILENAGE  = 10   /** &nbsp; */
 } secipssp_auth_type_param_data_type;
 
 /* ========================================================================
	Definitions for the ISAKMP Identification payload - RFC2407 - 4.6.2
 ======================================================================== */
 /**
  * Identity Type
  */ 
 typedef enum
 {
   SECIPSPKTDEFS_IDENT_TYPE_RESERVED		 =	0,	/**< --Default ID type */
   SECIPSPKTDEFS_IDENT_TYPE_IPV4_ADDR		 =	1,	/** &nbsp; */
   SECIPSPKTDEFS_IDENT_TYPE_FQDR			 =	2,	/** &nbsp; */
   SECIPSPKTDEFS_IDENT_TYPE_USER_FQDR		 =	3,	/** &nbsp; */
   SECIPSPKTDEFS_IDENT_TYPE_IPV4_ADDR_SUBNET =	4,	/** &nbsp; */
   SECIPSPKTDEFS_IDENT_TYPE_IPV6_ADDR		 =	5,	/** &nbsp; */
   SECIPSPKTDEFS_IDENT_TYPE_IPV6_ADDR_SUBNET =	6,	/** &nbsp; */
   SECIPSPKTDEFS_IDENT_TYPE_IPV4_ADDR_RANGE  =	7,	/** &nbsp; */
   SECIPSPKTDEFS_IDENT_TYPE_IPV6_ADDR_RANGE  =	8,	/** &nbsp; */
   SECIPSPKTDEFS_IDENT_TYPE_DER_ASN1_DN 	 =	9,	/**< -- ASN.1 Distinguished Name */
   SECIPSPKTDEFS_IDENT_TYPE_DER_ASN1_GN 	 = 10,	/**< -- ASN.1 General Name */
   SECIPSPKTDEFS_IDENT_TYPE_KEY_ID			 = 11	/** &nbsp; */
 } secipspktdefs_ident_type_enum_type;
 
  /**
   @}
*/ 
/* end_group ipsec_internal_ikev2_ds */

/* ---------- Not needed in the PDF - Start ---------- */
/** @cond */
  
/* =================================================================
	   I P S E C S P  D i r e c t i o n
  ================================================================= */
  typedef enum
  {
	SECIPSSP_DIRECTION_RESERVED   = 0,
	SECIPSSP_DIRECTION_IN		  = 1,
	SECIPSSP_DIRECTION_OUT		  = 2,
	SECIPSSP_DIRECTION_INOUT	  = 3,
	SECIPSSP_DIRECTION_COUNT
  } secipssp_direction_enum_type;
    
  typedef enum
  {
	 SECIPSFLOW_CB_STATE_FREE	   = 0, /* &nbsp; */
	 SECIPSFLOW_CB_STATE_IN_USE    = 1	/* &nbsp; */
  }secipsflow_cb_state_type;

  /*
  * ISAKMP Negotiation mode with ISAKMP Gateway
  */
 typedef enum
 {
   SECIPSSP_ISAKMP_MODE_INVALID_PARAM = 0,
   SECIPSSP_ISAKMP_MODE_MAIN		  = 1,
   SECIPSSP_ISAKMP_MODE_AGGRESSIVE	  = 2,
   SECIPSSP_ISAKMP_MODE_COUNT
 } secipssp_isakmp_mode_param_data_type;
 
 /* =================================================================
	ISAKMP SA protocol types - RFC2407 - Section 4.4.1
 ================================================================= */
 typedef enum
 {
   SECIPSPKTDEFS_ISAKMP_SA_PROTO_RESERVED  = 0,
   SECIPSPKTDEFS_ISAKMP_SA_PROTO_ISAKMP    = 1,
   SECIPSPKTDEFS_IKEV2_SA_PROTO_IKE 	   = SECIPSPKTDEFS_ISAKMP_SA_PROTO_ISAKMP,
   SECIPSPKTDEFS_ISAKMP_SA_PROTO_IPSEC_AH  = 2,
   SECIPSPKTDEFS_ISAKMP_SA_PROTO_IPSEC_ESP = 3,
   SECIPSPKTDEFS_ISAKMP_SA_PROTO_IPCOMP    = 4
 } secipspktdefs_isakmp_sa_proto_enum_type;

 typedef enum
 {
   SECIPSSA_SA_STATE_UNDEFINED,
   SECIPSSA_SA_STATE_DOWN,
   SECIPSSA_SA_STATE_LARVAL,
   SECIPSSA_SA_STATE_PENDING_REKEY,
   SECIPSSA_SA_STATE_PENDING_DELETE,
   SECIPSSA_SA_STATE_COMPLETED
 } secipssa_sa_state_enum_type;
  
 typedef enum
 {
   SECIPSSA_SA_PROTOCOL_UNDEFINED,
   SECIPSSA_SA_PROTOCOL_ISAKMP	 = SECIPSPKTDEFS_ISAKMP_SA_PROTO_ISAKMP,
   SECIPSSA_SA_PROTOCOL_AH		 = SECIPSPKTDEFS_ISAKMP_SA_PROTO_IPSEC_AH,
   SECIPSSA_SA_PROTOCOL_ESP 	 = SECIPSPKTDEFS_ISAKMP_SA_PROTO_IPSEC_ESP
 } secipssa_sa_protocol_enum_type;
 
 typedef enum
 {
   SECIPSPKTDEFS_IKE_CV_GD_MODP768       = 1,
   SECIPSPKTDEFS_IKE_CV_GD_MODP1024      = 2,
   SECIPSPKTDEFS_IKE_CV_GD_GP155         = 3,
   SECIPSPKTDEFS_IKE_CV_GD_GP185         = 4,
   SECIPSPKTDEFS_IKE_CV_GD_MODP1536      = 5,
   SECIPSPKTDEFS_IKE_CV_GD_MODP2048      = 14
 } secipspktdefs_ike_cv_gd_type;
 
 typedef enum
 {
   SECIPSPKTDEFS_IPSEC_CV_GD_MODP768       = 1,
   SECIPSPKTDEFS_IPSEC_CV_GD_MODP1024      = 2,
   SECIPSPKTDEFS_IPSEC_CV_GD_GP155         = 3,
   SECIPSPKTDEFS_IPSEC_CV_GD_GP185         = 4
 } secipspktdefs_ipsec_cv_gd_type;

 /* IKEv2 Child SA exchange types */
 typedef enum
 {
   SECIPSPKTDEFS_CHILDSA_REKEY_IKEV2_SA,
   SECIPSPKTDEFS_CHILDSA_REKEY_IPSEC_SA,
   SECIPSPKTDEFS_CHILDSA_CREATE_IPSEC_SA,
   SECIPSPKTDEFS_CHILDSA_NETWORK_INITIATED
 } secipspktdefs_child_sa_enum_type;
  
 /* IKEv2 Notofication payload types */
 typedef enum
 {
   SECIPSPKTDEFS_IKEV2_NOTIFY_RESERVED							= 0,
   SECIPSPKTDEFS_IKEV2_NOTIFY_UNSUPPORTED_CRITICAL_PAYLOAD		= 1,
   SECIPSPKTDEFS_IKEV2_NOTIFY_INVALID_IKE_SPI					= 4,
   SECIPSPKTDEFS_IKEV2_NOTIFY_INVALID_MAJOR_VERSION 			= 5,
   SECIPSPKTDEFS_IKEV2_NOTIFY_INVALID_SYNTAX					= 7,
   SECIPSPKTDEFS_IKEV2_NOTIFY_INVALID_MESSAGE_ID				= 9,
   SECIPSPKTDEFS_IKEV2_NOTIFY_INVALID_SPI						= 11,
   SECIPSPKTDEFS_IKEV2_NOTIFY_NO_PROPOSAL_CHOSEN				= 14,
   SECIPSPKTDEFS_IKEV2_NOTIFY_INVALID_KE_PAYLOAD				= 17,
   SECIPSPKTDEFS_IKEV2_NOTIFY_AUTHENTICATION_FAILED 			= 24,
   SECIPSPKTDEFS_IKEV2_NOTIFY_SINGLE_PAIR_REQUIRED				= 34,
   SECIPSPKTDEFS_IKEV2_NOTIFY_NO_ADDITIONAL_SAS 				= 35,
   SECIPSPKTDEFS_IKEV2_NOTIFY_INTERNAL_ADDRESS_FAILURE			= 36,
   SECIPSPKTDEFS_IKEV2_NOTIFY_FAILED_CP_REQUIRED				= 37,
   SECIPSPKTDEFS_IKEV2_NOTIFY_TS_UNACCEPTABLE					= 38,
   SECIPSPKTDEFS_IKEV2_NOTIFY_INVALID_SELECTORS 				= 39,
   /* Reserved to IANA - Error Types:  40 - 8191 */
   /* Private Use - Errors: 		   8192 - 16383 */
   SECIPSPKTDEFS_IKEV2_NOTIFY_PDN_CONN_REJCET             = 8192,
   SECIPSPKTDEFS_IKEV2_NOTIFY_IKE_AUTH_MAX_CONN           = 8193,
   
   SECIPSPKTDEFS_IKEV2_NOTIFY_NO_SUBSCRIPTION          = 16374,
   SECIPSPKTDEFS_IKEV2_NOTIFY_RAT_DISALLOWED           = 16375,
   SECIPSPKTDEFS_IKEV2_NOTIFY_ROAMING_NOT_ALLOWED      = 16376,
   SECIPSPKTDEFS_IKEV2_NOTIFY_NETWORK_FAILURE          = 16377,
   SECIPSPKTDEFS_IKEV2_NOTIFY_NETWORK_TOO_BUSY         = 16378,

   SECIPSPKTDEFS_IKEV2_NOTIFY_INITIAL_CONTACT					= 16384,
   SECIPSPKTDEFS_IKEV2_NOTIFY_SET_WINDOW_SIZE					= 16385,
   SECIPSPKTDEFS_IKEV2_NOTIFY_ADDITIONAL_TS_POSSIBLE			= 16386,
   SECIPSPKTDEFS_IKEV2_NOTIFY_IPCOMP_SUPPORTED					= 16387,
   SECIPSPKTDEFS_IKEV2_NOTIFY_NAT_DETECTION_SOURCE_IP			= 16388,
   SECIPSPKTDEFS_IKEV2_NOTIFY_NAT_DETECTION_DESTINATION_IP		= 16389,
   SECIPSPKTDEFS_IKEV2_NOTIFY_COOKIE							= 16390,
   SECIPSPKTDEFS_IKEV2_NOTIFY_USE_TRANSPORT_MODE				= 16391,
   SECIPSPKTDEFS_IKEV2_NOTIFY_HTTP_CERT_LOOKUP_SUPPORTED		= 16392,
   SECIPSPKTDEFS_IKEV2_NOTIFY_REKEY_SA							= 16393,
   SECIPSPKTDEFS_IKEV2_NOTIFY_ESP_TFC_PADDING_NOT_SUPPORTED 	= 16394,
   SECIPSPKTDEFS_IKEV2_NOTIFY_NON_FIRST_FRAGMENTS_ALSO			= 16395,
   /* IKEv2 Multi Auth Notify Payloads */
   SECIPSPKTDEFS_IKEV2_NOTIFY_MULTIPLE_AUTH_SUPPORTED			= 16404,
   SECIPSPKTDEFS_IKEV2_NOTIFY_ANOTHER_AUTH_FOLLOWS				= 16405
   /* Reserved to IANA - Status Types:	16393 - 40959 */
   /* Private Use - Status Types:		40960 - 65535 */
 } secipspktdefs_ikev2_notify_enum_type;
 
 typedef enum
 {
   SECIPSSA_SA_ENCAPSULATE_UNDEFINED = SECIPSSP_ENCAPSULATE_UNDEFINED,
   SECIPSSA_SA_ENCAPSULATE_TUNNEL	 = SECIPSSP_ENCAPSULATE_TUNNEL,
   SECIPSSA_SA_ENCAPSULATE_TRANSPORT = SECIPSSP_ENCAPSULATE_TRANSPORT
 } secipssa_sa_encapsulate_enum_type;

  /* ---------- Not needed in the PDF - End ---------- */
 /** @endcond */

 /**
   @addtogroup ipsec_internal_ikev2_ds
 
   @{ */
 
  /**
   * IPsec/IKE SA lifetimes in seconds
   */
  typedef struct
  {
	uint32	  soft_life_sec; /**< -- Rekeying would start after this */
	uint32	  hard_life_sec; /**< -- Rekeying should end before this */
  } secipssa_life_sec_type;
  
  /**
   * IPsec SA lifetimes in bytes
   */
  typedef struct
  {
	uint32	  soft_life_bytes; /**< -- Rekeying would start after this */
	uint32	  hard_life_bytes; /**< -- Rekeying should end before this */
  } secipssa_life_bytes_type; 
  
 /**
  * This structure is used to store the IKE Protection Info including IKE Proposal,
  * NAT, IKE Message retransmit, DPD timer value
  */
 typedef struct
 {
   IKETransf_ENCR_EType         encr_algo[IPSEC_PROP_MAX_TRANSF_CNT]; /**< -- Encryption Algo */
   IKETransf_AUTH_EType         hash_algo[IPSEC_PROP_MAX_TRANSF_CNT]; /**< -- Hash Algo */
   IKETransf_DH_EType           dh_group[IPSEC_PROP_MAX_TRANSF_CNT];  /**< -- DH Group */
   IKETransf_PRF_EType          prf_algo[IPSEC_PROP_MAX_TRANSF_CNT];  /**< -- PRF Algo */
 
   secipssp_isakmp_mode_param_data_type ikev1_mode;
 
   boolean                natt_enabled; /**< -- NAT Configuration - Enabled/Disabled */
   uint32                 natt_keepalive_secs; /**< -- NAT KeepAlive Timer value */
 
   secipssa_life_sec_type       life_sec; /**< -- IKE SA Rekey Timer value */
 
   uint32                 retransmit_interval_secs; /**< -- IKE Message Retransmit timer value */
   uint32                 max_retransmissions; /**< --  IKE Message Max number of retransmissions */
 
   boolean                multi_auth_supported;  /**< -- Multi Auth configuration - Supported or not */
 
   boolean                dpd_enabled; /**< -- DPD configuration - Enabled/Disabled */
   uint32                 dpd_time_secs;  /**< -- DPD Timer value */
 
   uint32                 max_tunnel_time_secs;  /**< -- Max Tunnel Timer value */
 
   boolean                cert_req_enabled; /**< -- CERTREQ Payload configuration Flag */
   IKE_AES_Keysize        aes_key_size[IPSEC_PROP_MAX_TRANSF_CNT]; /**< -- AES key size */
   boolean                ke_payload_enabled; /**< -- KE Payload enabled/Disabled  for IPSec Rekey payload*/
   boolean                v6_pcscf_with_pf_len; /** -- V6 PCSCF with Prefix Length, if TRUE pcscf length 17, else 16 */
   boolean                use_fra_pseudo_id; /**< --if TRUE use Fra/pseudonym from EAP module as IDi value,   else self_id as IDi */ 
   uint32                 eap_session_type; /**< -- EAP session type */
   uint32                 eap_client_version; /** EAP client version */
   uint32                 v4_pcscf_attr_type_value; /**< -- configurable V4 PCSCF type value */
   uint32                 v6_pcscf_attr_type_value; /**< -- configurable V6 PCSCF type value */
   uint32                 configured_ike_port;      /**< -- configurable port number for ike exchange */
 } secipssp_ike_protection_type;

 /**
  * Key Information for PSK
  */
 typedef struct
 {
   uint8  key_buffer[SECIPSSP_PSK_KEY_MAX_SIZE]; /**< -- Key */
   uint16 key_len; /**< -- Key Length */
 } secipssp_psk_param_data_type; 
 
 
 /**
  * Key Information for EAP-AKA
  */ 
 typedef struct
 {
   uint8  key_buffer[SECIPSSP_PSK_KEY_MAX_SIZE]; /**< -- Key */
   uint16 key_len; /**< -- Key Length */
 } secipssp_eap_aka_param_data_type;
 
 /**
  * Key Information for EAP-MD5
  */ 
 typedef struct
 {
   uint8  key_buffer[SECIPSSP_MD5_PASSWORD_MAX_SIZE]; /**< -- Key */
   uint16 key_len; /**< -- Key Length */
 } secipssp_eap_md5_param_data_type;

 /**
  * Self ID, Peer ID and AUTH Info	
  */
 typedef struct
 {
   secipspktdefs_ident_type_enum_type  id_type; /**< -- Self ID Type */
   void*							   id_value_ptr; /**< -- Self ID value */
   uint32							   id_value_len; /**< -- Self ID Length */
 
   secipspktdefs_ident_type_enum_type  peer_id_type; /**< -- Peer ID type */
   void*							   peer_id_value_ptr; /**< -- Peer ID value */
   uint32							   peer_id_value_len; /**< -- Peer ID Length */
 
   secipspktdefs_ident_type_enum_type  eap_id_type; /**< -- EAP ID */
   void*							   eap_id_value_ptr; /**< -- EAP ID value */
   uint32							   eap_id_value_len; /**< -- EAP ID Length */
 
   secipssp_auth_type_param_data_type  auth_type; /**< -- Self Auth Type */
   void*							   auth_param_ptr; /**< -- Self Auth Data */
   uint32							   auth_param_len; /**< -- Self Auth Data Length */
 
   secipssp_auth_type_param_data_type  peer_auth_type; /**< -- Peer Auth Type */
   void*							   peer_auth_param_ptr; /**< -- Peer Auth Data */
   uint32							   peer_auth_param_len; /**< -- Peer Auth Data Length */

   secipspktdefs_ident_type_enum_type  peer_certId_type; /**< -- Peer IKEv2 ID Type */
   void*                 peer_certId_value_ptr; /**< -- Peer IKEv2 ID value */
   uint32                peer_certId_value_len; /**< -- Peer IKEv2 ID Length */
 } secipssp_id_auth_type;

/**
 Encapsulated Security Payload proposal information.
*/
typedef struct
{
    void*				 sa_index;
    /**< Security Association index into the SADB -- manual keying only. */
	IKETransf_ENCR_EType  sa_encr_algo[IPSEC_PROP_MAX_TRANSF_CNT];
	/**< Encryption algorithm. */
	IKETransf_AUTH_EType  sa_auth_algo[IPSEC_PROP_MAX_TRANSF_CNT];
	/**< Authorization algorithm. */
        IKETransf_DH_EType    dh_group[IPSEC_PROP_MAX_TRANSF_CNT]; 
        /**< -- DH Group */
	IKE_AES_Keysize              aes_key_size[IPSEC_PROP_MAX_TRANSF_CNT]; 
	/**< -- AES key size */
} secipssp_esp_protection_type;

/**
Authentication Header proposal information.
*/
typedef struct
{
    void*				 sa_index;
	/**< Security Association index into the SADB -- manual keying only. */
	IKETransf_AUTH_EType  sa_auth_algo[IPSEC_PROP_MAX_TRANSF_CNT];
	/**< Authentication algorithm. */
} secipssp_ah_protection_type;
	 
/**
Stores a security policy's information.
*/
typedef struct
{
   secipssp_key_mgmt_enum_type			  key_mgmt;
   /**< Key management type. */
   secipssp_ike_protection_type 		  ike_protection;
   /**< IKE protection including proposal and timer values. */
   secipssp_encapsulate_enum_type		  encaps_mode;
   /**< Encapsulation mode. */
   secipssp_esp_protection_type 		  esp_protection;
   /**< Encapsulated Security Payload proposal. */
   secipssp_ah_protection_type			  ah_protection;
   /**< Authentication Header proposal. */
   secipssa_life_sec_type				  ipsec_life_sec;
   /**< IPsec SA re-key time value, in seconds. */
   secipssa_life_bytes_type 			  ipsec_life_bytes;
   /**< IPsec SA re-key time value, in bytes. */
   ip_version_enum_type                           sa_type; 
   /**< IKE/IPsec SA type */
 } secipssp_protection_type;

/**
   @}
*/ 
/* end_group ipsec_internal_ikev2_ds */
  
/* ---------- Not needed in the PDF - Start ---------- */
/** @cond */

/* Protection Selector*/
typedef ip_filter_spec_type	secipssp_protection_selector_type;

 typedef struct
 {
   uint16 type;
   uint32 coding;		 /* TV or TLV */
   uint16 length;		 /* Only useful if TLV otherwise always = 2 */
   uint32 value;
 } secipspktdefs_attribute_payload_type;
  
 /* A transform is made of a set of attributes */
 typedef struct
 {
   uint8								transform_number; /* IKEv1 only */
   uint8								transform_type;   /* IKEv2 only */
   uint8								transform_id;
   uint8								attribute_count;
   secipspktdefs_attribute_payload_type attribute[SECIPSPKTDEFS_MAX_ATTRIBUTES];
 } secipspktdefs_transform_payload_type;
  
 typedef struct
 {
   secipssp_auth_type_param_data_type auth_type;
   union
   {
	  secipssp_psk_param_data_type psk;
	  secipssp_eap_aka_param_data_type eap_aka;
	  secipssp_eap_md5_param_data_type eap_md5;
 /*  secipssp_hybrid_param_data_type hybrid;
	 secipssp_auth_type_eap_sim 	 eap_sim;
	 secipssp_auth_type_eap_tls 	 eap_tls;
	 secipssp_auth_type_certificate  cert; */
   }auth_data;
 } secipssp_auth_param_data_type;
 
 /* 
  *Define the SADB SA
  */
 typedef struct
 {
   uint32		  length;
   uint8*		  data_ptr;
 } secipssa_variable_data;
 
 typedef struct
 {
   uint32 type;
   union 
   {
	 uint32 				ipv4_addr;
	 struct  ps_in6_addr	ipv6_addr;
	 secipssa_variable_data key_id;
	 secipssa_variable_data rfc822_addr_id;
   } value;
 } secipssa_sa_identification;
 
 
 /*
  * ID Information - Self ID, Peer ID, Auth Data
  */
 typedef struct
 {
   secipssa_sa_identification	  self_id;
   secipssa_sa_identification	  peer_id;
   secipssa_sa_identification	  peer_cert_id;
   secipssa_sa_identification	  eap_id;
   secipssp_auth_param_data_type  auth_param;
   secipssp_auth_param_data_type  peer_auth_param;
 } secipssp_id_info;
  
 typedef struct
 {
	uint32 Low;
	uint32 High;
 } secipssa_replay_window_type;
  
  typedef struct
  {
	uint32						spi;
	uint32						sequence_number;
	uint32						used_bytes;
	uint32						time_last_used;
	secipssa_replay_window_type replay_window;
  } secipssa_sa_type;
    
 /***
	  * Config attribute values
  ***/
 typedef struct
 {
   struct ps_in_addr addr;
   struct ps_in_addr subnet_mask;
 } secips_cfg_attr_ip4_subnet_type;
 
 /**
  * Config Attribute for INTERNAL_IP6_ADDRESS
  */
 typedef struct
 {
   struct ps_in6_addr addr; /**< -- IPv6 Address */
   uint8  prefix_len; /**< -- IPv6 Address Prefix Length */
 } secips_cfg_attr_ip6_subnet_type;

 typedef struct 
 {
   IPSecConfigAttrEType 			  type;
   uint32							  value_len;
   uint32							  prefix_len;
   uint8                                          v6_pcscf_with_pf; // if TRUE IPV6 Pcscf with Prefix length 17 else 16
   union {
	 uint8* 						  data_ptr;
	 struct ps_in_addr* 			  ip4_address_ptr;
	 struct ps_in6_addr*			  ip6_address_ptr;
	 secips_cfg_attr_ip4_subnet_type* ip4_subnet_ptr;
	 secips_cfg_attr_ip6_subnet_type* ip6_subnet_ptr;
   } value;
 } secips_cfg_attr_type;
  
 /*---------------------------------------------------------------------------
  * 				I S A K M P   P R O P O S A L	T Y P E 
  --------------------------------------------------------------------------*/
 typedef struct
 {
   uint16  protocol;
   //secipssdb_protection_type_enum_type transform[12];
   uint32 transform[12];
   uint16 transform_count;
   uint32 lifetime_seconds;
   uint32 lifetime_bytes;
 
   uint8  encapsulation_mode;	 /* For IPSEC SA only */
   uint32 authentication_method; /* For ISAKMP only */
 } secipstypes_sa_proposal_type;
 
typedef enum
{
  AES_MODE_MIN_VALUE = 0,
  AES_CBC,
  AES_CTR,
  AES_MODE_MAX_VALUE
}secipssa_aes_mode_type;

 /*---------------------------------------------------------------------------
  * 					  I S A K M P	 S A
  --------------------------------------------------------------------------*/
 /* ISAKMP Security Association.
 */
 typedef struct
 {
   secipssp_key_mgmt_enum_type	  ike_version;
   uint32						  spi;
   uint32						  sequence_number;
   uint32						  in_sequence_number;
   uint32						  used_bytes;
   uint32						  time_last_used; 
   uint32						  time_last_recv;
   secipssa_replay_window_type	  replay_window;
   ps_ip_addr_type				  end_point;
   ps_ip_addr_type				  local_addr;
   uint8*						  segw_fqdn_ptr;
   secipssa_sa_state_enum_type	  state;
   secipssa_sa_protocol_enum_type type;
   secapi_algo_enum_type		  hash_algo; /* Negotiated hash algorithm. */
   secapi_handle_type			  hash_handle;
   secapi_algo_enum_type		  crypto_algo;
   /* Negotiated cryptographic algorithm. */
   secapi_handle_type			  crypto_handle;
   secapi_algo_enum_type		  prf_algo;
   /* Negotiated prf algorithm.  */
   secapi_handle_type			  prf_handle;	
   secipspktdefs_ike_cv_gd_type   dh_group_id;
   /* Negotiated DH group ID. */
   secipstypes_sa_proposal_type   proposal;
   uint32						  life_max_kilobytes;
   uint32						  life_max_seconds;
 
   /* Needed for subsequent informational and quick exchanges */
   uint8						  skeyid_e[SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE];
   uint8						  skeyid_a[SECIPSTYPES_HASH_MAX_SIZE];
   uint8						  skeyid_d[SECIPSTYPES_HASH_MAX_SIZE];
   uint8						  iv[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];
   uint8                                                  iv_ctr[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];
   uint16						  skeyid_e_size;
   uint16						  hash_size;
   uint16						  prf_size;
   uint16						  iv_size;
   uint16                                                 skeyid_e_stream_size;
   uint32                                                 blockcounter;
 
   /* NAT-T specific data */
   boolean						  is_self_nat; /* This end is behind NAT. */
   boolean						  is_peer_nat; /* Peer is behind NAT. */
 
   /* ISAKMP Internal data */
   void*						  exchange_ptr;
   void*						  collision_exchange_ptr;
   void*						  child_sa_ptr[SECIPSSA_NUM_MAX_CHILD_SA];
   uint32						  num_child_sas;
   boolean						  child_reproducible;
   boolean						  info_exchange_collision;
   boolean						  child_sa_exchange_collision;
   boolean						  child_sa_cleanup_pending;
   boolean						  info_child_sa_exchange_collision;
   /* Proposal Transforms set by the App */
   secipssp_ike_protection_type   ike_protection;
   secipssp_id_info 			  id_info;
   dss_iface_id_type			  iface_id;
 
   /* IKEv2 specific parameters */
   uint8						  spii[SECIPSTYPES_IKEV2_SPI_SIZE];
	   /* IKEv1: cky_i */
   uint8						  spir[SECIPSTYPES_IKEV2_SPI_SIZE];
	   /* IKEv1: cky_r */
   uint8						  SK_d[SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE];
   uint8						  SK_ai[SECIPSTYPES_HASH_MAX_SIZE];
   uint8						  SK_ar[SECIPSTYPES_HASH_MAX_SIZE];
   uint8						  SK_ei[SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE];
   uint8						  SK_er[SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE]; 
   uint8						  SK_pi[SECIPSTYPES_HASH_MAX_SIZE]; 
   uint8						  SK_pr[SECIPSTYPES_HASH_MAX_SIZE]; 
   dsm_item_type*				  msg1;
	 /* Pointer to msg1; used for authentication calculations. */
   dsm_item_type*				  msg2;
	 /* Pointer to msg2; used for authentication calculations. */
   dsm_item_type*				  idir_b_ptr;
	 /* Pointer to IDIR for for authentication calculations. */
   uint8						  ni[ SECIPSPKTDEFS_NONCE_MAX_SIZE ];  
   uint16						  ni_size;
   uint8						  nr[ SECIPSPKTDEFS_NONCE_MAX_SIZE ];
   uint16						  nr_size;
   uint32						  cfg_attr_cnt;
   secips_cfg_attr_type*		  cfg_attr[IPSEC_CFG_MAX_ATTRS];
	 /* Configuration attributes received. */
   uint32						  timeout_value;
	 /* Message timeout value. */
   rex_timer_type*				  natt_keepalive_timer;
	 /* Timeout timer NAT-T keep alives. */
   rex_timer_type*				  rekey_timer;
	 /* IKEv2 Security Association re-key timer. */
   rex_timer_type*				  delete_timer;
	 /* IKEv2 Security Association delete timer. */
   rex_timer_type*				  keepalive_timer;
	 /* IKEv2 Security Association keep alive timer. */
   rex_timer_type*				  max_tunnel_timer;
	 /* IPSEC Security Association maximum tunnel timer. */
   secipspktdefs_child_sa_enum_type child_sa_type;
	 /* Type of child Security Association exchange. */
   boolean						  rekey_pending;
	 /* Whether a re-key is pending for the IKE Security Association. */
   uint8						  transform_count;
   secipspktdefs_transform_payload_type* chosen_transforms[SECIPSPKTDEFS_MAX_TRANSFORMS];
   boolean						  orig_initiator;
	 /* Are we the original initiator for this IKE Security Association? */
   secipspktdefs_ikev2_notify_enum_type  in_notify_err_id;
	 /* Incoming notify error. */
   void*								 in_notify_err_data;
	 /* Data for the incoming notify error. */
   uint16								 in_notify_err_data_len;
	 /* Length of the incoming error data. */
   secipspktdefs_ikev2_notify_enum_type  out_notify_err_id;
	 /* Outgoing notify error. */
   void*								 out_notify_err_data;
	 /* Data for the outgoing notify error. */
   uint16								 out_notify_err_data_len;
	 /* Length of the outgoing error data. */
   boolean						  peer_supports_multi_auth;
	 /* Whether the peer supports multi-authentication. */
   boolean						  another_auth_follows_exchange;
	 /* Another authentication follows the authentication exchange. */
   uint32						  msg_id;
	 /* Current IKE message ID. */
   void*						  new_ikev2_sa_ptr; /* New IKEv2 SA created after the IKE Rekey */
   void*						  coll_ikev2_sa_ptr; /* IKE SA created due to IKE SA REKEY Collision */
   boolean            dh_retry_is_on;    /* To check DH retry is in progress */
   secipssa_sa_identification      idir_recvd; /* Identity (R)  for NULL APN support need to store idir received*/
   secipssp_protection_selector_type   tsr;           /* Traffic selector (R) */
   secipssp_protection_selector_type   tsi;           /* Traffic selector (I) */
   secipssa_aes_mode_type aes_mode;
   uint16             src_port;         /* to store Source prot number */
   boolean            is_null_apn;      /* to check if NULL/NO APN set in IKE */
 } secipssa_isakmp_sa_type;

/*---------------------------------------------------------------------------
 *                         I P S E C     S A
 --------------------------------------------------------------------------*/
/* IPSEC Security Association type.
*/
typedef struct
{
  q_link_type                       link;
  secipssa_sa_type                  in;
  secipssa_sa_type                  out;

  ps_ip_addr_type                   dest_addr;
  ps_ip_addr_type                   local_addr;
  secipssa_sa_state_enum_type       state;
  secipssa_sa_protocol_enum_type    type;
  uint32                            hash_algo;
  uint32                            crypto_algo;
  secipssa_sa_encapsulate_enum_type encapsulation_mode;

  secipspktdefs_ipsec_cv_gd_type    dh_group_id;
  /* Negotiated DH group ID. */

  secipstypes_sa_proposal_type      proposal;
  boolean                           rekey_pending;  
  /* Is a rekey pending for this SA */

  void*                             policy_ptr;
  secipssp_esp_protection_type      esp_protection;
  secipssp_ah_protection_type       ah_protection;
  
  secipssa_sa_identification        idii;
  secipssa_sa_identification        idir;
  uint32                            life_max_kilobytes;
  uint32                            life_max_seconds;
  uint32                            life_timeLeft_seconds;
  /* Crypto material */
  uint16                            iv_size;
  uint16                            hash_size;
  uint16                            crypto_key_size;
  uint16                            crypto_key_stream_size;
  uint16                            prf_key_size;
  secapi_handle_type                hash_handle;
  secapi_handle_type                crypto_handle;
  uint8                             iv[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];
  uint8                             iv_ctr[SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN];
  uint8                             hash_key_in[SECIPSTYPES_HASH_MAX_SIZE];
  uint8                             hash_key_out[SECIPSTYPES_HASH_MAX_SIZE];
  uint8                             crypto_key_in[SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE];
  uint8                             crypto_key_out[SECIPSTYPES_CRYPTO_ALGO_KEY_MAX_SIZE];
  uint32                            blockcounter;

  /* NAT-T-specific data. */
  boolean                           is_udp_encaps;
  ps_ip_addr_type                   natoa_i;
  ps_ip_addr_type                   natoa_r;

  /* Private internal data */
  void*                             ike_sa_ptr;
  void*                             ipsec_entry_ptr;
  void*                             exchange_ptr;
  void*                             collision_exchange_ptr;
  void*                             flow_client_data_ptr; /* Stores the flow
                                                               callback pointer. */

  rex_timer_type*                   rekey_timer;
    /* IPSEC Security Association re-ekey timer. */
  rex_timer_type*                   delete_timer;
    /* IPSEC Security Association delete timer. */

  uint8                             transform_count;
  secipspktdefs_transform_payload_type* chosen_transforms[SECIPSPKTDEFS_MAX_TRANSFORMS];
  secipssa_aes_mode_type aes_mode;
} secipssa_ipsec_sa_type;
 
 /* ---------- Not needed in the PDF - End ---------- */
 /** @endcond */

#endif  /*  SECIPSSP_H */
