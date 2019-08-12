#ifndef IPSEC_H
#define IPSEC_H

/**
  IPSec.h
  
  <h2>IPSec Common definitions for both Secuiry Policy and Security Association </h2>
  
  <h3>PUBLIC Functions</h3>
    
  <h3>Initialization and sequencing requirements</h3>

  <p>
  Not Applicable
  </p>
*/

/*

    Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

//==========================================================================
//   Includes
//==========================================================================

#define IPSEC_DSS_ERROR    -1
#define IPSEC_DSS_SUCCESS  0

/* =================================================================
                 SECURITY POLICY RELATED DEFINITIONS 
================================================================= */

/*------------------------------------------------------------------------
     I P S E C   K e y   M a n a g e m e n t   O p t i o n s
------------------------------------------------------------------------*/
typedef enum
{
  IPSEC_KEY_MGMT_UNDEFINED   = 0,
  IPSEC_KEY_MGMT_IKEV1       = 1,
  IPSEC_KEY_MGMT_IKEV2       = 2,
  IPSEC_KEY_MGMT_MANUAL      = 3,
  IPSEC_KEY_MGMT_IKEV2_NOW   = 4  /* Bring up the SA immediately */
} IPSecKeyMgmtEType;

/*------------------------------------------------------------------------
   I P S E C    p r o t e c t i o n    s u i t e s
------------------------------------------------------------------------*/
/****
 * ESP Protection types
 ****/
typedef enum
{
  IPSEC_ESP_PROTECTION_TYPE_RESERVED        =  0,
  IPSEC_ESP_PROTECTION_TYPE_DH1024_AES_SHA  =  1,
  IPSEC_ESP_PROTECTION_TYPE_DH1024_AES_MD5  =  2,
  IPSEC_ESP_PROTECTION_TYPE_DH1024_3DES_SHA =  3,
  IPSEC_ESP_PROTECTION_TYPE_DH1024_3DES_MD5 =  4,
  IPSEC_ESP_PROTECTION_TYPE_DH1024_DES_SHA  =  5,
  IPSEC_ESP_PROTECTION_TYPE_DH1024_DES_MD5  =  6,
  IPSEC_ESP_PROTECTION_TYPE_AES_SHA         =  7,
  IPSEC_ESP_PROTECTION_TYPE_AES_MD5         =  8,
  IPSEC_ESP_PROTECTION_TYPE_3DES_SHA        =  9,
  IPSEC_ESP_PROTECTION_TYPE_3DES_MD5        =  10,
  IPSEC_ESP_PROTECTION_TYPE_DES_SHA         =  11,
  IPSEC_ESP_PROTECTION_TYPE_DES_MD5         =  12,
  IPSEC_ESP_PROTECTION_TYPE_AES_NULL        =  13,
  IPSEC_ESP_PROTECTION_TYPE_3DES_NULL       =  14,
  IPSEC_ESP_PROTECTION_TYPE_DES_NULL        =  15,
  IPSEC_ESP_PROTECTION_TYPE_NULL_SHA        =  16,
  IPSEC_ESP_PROTECTION_TYPE_NULL_MD5        =  17,
  IPSEC_ESP_PROTECTION_TYPE_NO_ESP          =  18,
  /* SHA256  related protection types*/
  IPSEC_ESP_PROTECTION_TYPE_DH1024_AES_SHA256  =  19,
  IPSEC_ESP_PROTECTION_TYPE_DH1024_3DES_SHA256 =  20,
  IPSEC_ESP_PROTECTION_TYPE_DH1024_DES_SHA256  =  21,
  IPSEC_ESP_PROTECTION_TYPE_AES_SHA256         =  22,
  IPSEC_ESP_PROTECTION_TYPE_3DES_SHA256        =  23,
  IPSEC_ESP_PROTECTION_TYPE_DES_SHA256         =  24,
  IPSEC_ESP_PROTECTION_TYPE_NULL_SHA256        =  25,

  IPSEC_ESP_PROTECTION_TYPE_COUNT
} IPSecESPProtectionEType;

/****
 * AH Protection types
 ****/
typedef enum
{
  IPSEC_AH_PROTECTION_TYPE_RESERVED   =  0,
  IPSEC_AH_PROTECTION_TYPE_DH1024_SHA =  1,
  IPSEC_AH_PROTECTION_TYPE_DH1024_MD5 =  2,
  IPSEC_AH_PROTECTION_TYPE_SHA        =  3,
  IPSEC_AH_PROTECTION_TYPE_MD5        =  4,
  IPSEC_AH_PROTECTION_TYPE_NO_AH      =  5,
  /* SHA256  related protection types*/
  IPSEC_AH_PROTECTION_TYPE_DH1024_SHA256 =  6,
  IPSEC_AH_PROTECTION_TYPE_SHA256        =  7,
  IPSEC_AH_PROTECTION_TYPE_COUNT
} IPSecAHProtectionEType;

/****
 * Encapsulation Methods 
 ****/
typedef enum
{
  IPSEC_ENCAPSULATE_UNDEFINED  = 0,
  IPSEC_ENCAPSULATE_TUNNEL     = 1,
  IPSEC_ENCAPSULATE_TRANSPORT  = 2
} IPSecEncapsulateEType;

/****
 * ISAKMP SA protocol types - RFC2407 - Section 4.4.1
 ****/
typedef enum
{
  IPSEC_SA_PROTO_RESERVED  = 0,
  IPSEC_SA_PROTO_ISAKMP    = 1,
  IPSEC_SA_PROTO_IPSEC_AH  = 2,
  IPSEC_SA_PROTO_IPSEC_ESP = 3,
  IPSEC_SA_PROTO_IPCOMP    = 4
} IPSecSAProtoEType;

/*---------------------------------------------------------------------------
               Internet-family specific host internet address
---------------------------------------------------------------------------*/
struct InAddr                       /* structure for "historical" reasons */
{
  uint32 socket_addr;                                        /* socket address */
};

/* IPv6 address structure */
struct In6Addr
{
  union
  {
    uint8   u6_addr8[16];
    uint16  u6_addr16[8];
    uint32  u6_addr32[4];
    uint64  u6_addr64[2];
  } in6_u;
#define S6Addr                  in6_u.u6_addr8
#define S6Addr16                in6_u.u6_addr16
#define S6Addr32                in6_u.u6_addr32
#define S6Addr64                in6_u.u6_addr64
};

/*---------------------------------------------------------------------------
TYPEDEF IPSecAddrEType

DESCRIPTION
  An enum that defines all of the address types supported - used to
  discriminate the union below.

  NOTE: The values are chosen to easy debugging.
---------------------------------------------------------------------------*/
typedef enum
{
  IPSEC_IP_ANY_ADDR               = 0,
  IPSEC_IPV4_ADDR                 = 4,
  IPSEC_IPV6_ADDR                 = 6,
  IPSEC_IP_ADDR_INVALID           = 255,
  IPSEC_IFACE_ANY_ADDR_FAMILY     = IPSEC_IP_ANY_ADDR,
  IPSEC_IFACE_IPV4_ADDR_FAMILY    = IPSEC_IPV4_ADDR,
  IPSEC_IFACE_IPV6_ADDR_FAMILY    = IPSEC_IPV6_ADDR,
  IPSEC_IFACE_INVALID_ADDR_FAMILY = IPSEC_IP_ADDR_INVALID
} IPSecAddrEType;

/*---------------------------------------------------------------------------
TYPEDEF IPSecAddrType

DESCRIPTION
  structure which is a discriminated union that defines the IP addresses that
  we support.
---------------------------------------------------------------------------*/
typedef struct
{
  IPSecAddrEType Type; 

  union
  {
    struct InAddr  V4;
    struct In6Addr V6;
  } Addr;
} IPSecAddrType;

/*
 * IP Next Protocol   
 */
typedef enum
{
  IPSEC_IPV6_BASE_HDR        = 0,   
  IPSEC_IPPROTO_ICMP         = 1,         /* ICMP protocol */
  IPSEC_IPPROTO_IP           = 4,
  IPSEC_IPPROTO_TCP          = 6,         /* TCP Protocol */
  IPSEC_IPPROTO_UDP          = 17,        /* UDP Protocol */
  IPSEC_ROUTING_HDR          = 43,
  IPSEC_FRAG_HDR             = 44,
  IPSEC_IPPROTO_ESP          = 50,        /* ESP Protocol */
  IPSEC_IPPROTO_AH           = 51,
  IPSEC_IPPROTO_ICMP6        = 58,
  IPSEC_NO_NEXT_HDR          = 59
} IPSecIPProtoEType;

/****
 * Protocol Type
 ****/
typedef uint8 IPSecProtocolType;

/****
 * Port Type
 ****/
typedef uint16 IPSecPortType;

/****
 * Pre-Shared key for ISAKMP Gateway Authentication
 ****/
typedef struct
{
  uint8  *pKey;
  uint32  dwKeyLen;
} IPSecPSKParamDataType;

typedef enum
{
  IPSEC_AUTH_TYPE_INVALID_PARAM = 0,
  IPSEC_AUTH_TYPE_PSK           = 1,
  IPSEC_AUTH_TYPE_HYBRID        = 2,
  IPSEC_AUTH_TYPE_EAP_SIM_3GPP  = 3,
  IPSEC_AUTH_TYPE_EAP_AKA       = 4,
  IPSEC_AUTH_TYPE_EAP_TLS       = 5,
  IPSEC_AUTH_TYPE_CERTIFICATE   = 6
} IPSecAuthMethodEType;

typedef struct
{
  IPSecAuthMethodEType   AuthType;
  void                  *pAuthData;
} IPSecAuthParamType;

/* ========================================================================
   Definitions for the ISAKMP Identification payload - RFC2407 - 4.6.2
======================================================================== */
typedef enum
{
  IPSEC_IDENT_TYPE_RESERVED         =  0,
  IPSEC_IDENT_TYPE_IPV4_ADDR        =  1,
  IPSEC_IDENT_TYPE_FQDR             =  2,
  IPSEC_IDENT_TYPE_USER_FQDR        =  3,
  IPSEC_IDENT_TYPE_IPV4_ADDR_SUBNET =  4,
  IPSEC_IDENT_TYPE_IPV6_ADDR        =  5,
  IPSEC_IDENT_TYPE_IPV6_ADDR_SUBNET =  6,
  IPSEC_IDENT_TYPE_IPV4_ADDR_RANGE  =  7,
  IPSEC_IDENT_TYPE_IPV6_ADDR_RANGE  =  8,
  IPSEC_IDENT_TYPE_DER_ASN1_DN      =  9,
  IPSEC_IDENT_TYPE_DER_ASN1_GN      = 10,
  IPSEC_IDENT_TYPE_KEY_ID           = 11,
  IPSEC_IDENT_TYPE_MAX
} IPSecIdentificationEType;

/****
 * Option:   IDII
 *           IDIR
 * Val Type: IPSecIdentificationType
 ****/
typedef struct
{
  IPSecIdentificationEType  Type;
  void                     *pIdVal;
  uint32                    dwLen;
} IPSecIdentificationType;

/****
 * ISAKMP Negotiation mode with ISAKMP Gateway
 ****/
typedef enum
{
  IPSEC_ISAKMP_MODE_INVALID_PARAM = 0,
  IPSEC_ISAKMP_MODE_MAIN          = 1,
  IPSEC_ISAKMP_MODE_AGGRESSIVE    = 2
} IPSecIsakmpModeEType;

/****
 * Application name as known by ISAKMP Gateway
 ****/
typedef struct
{
  char   *sName;
  uint32  dwGroupLen;
} IPSecGroupNameType;

typedef struct
{
  IPSecAuthParamType    Auth;
  IPSecAuthParamType    PeerAuth;
  IPSecIsakmpModeEType  IsakmpMode;
  IPSecGroupNameType    GroupName;
} IPSecGwParamType;

/* RFC 4306: Configuration Attributes that can be requested via IKEv2 */
#define IPSEC_CFG_MAX_ATTRS 14
typedef enum 
{
  IPSEC_CFG_ATTR_RESERVED               = 0,
  IPSEC_CFG_INTERNAL_IP4_ADDRESS        = 1,
  IPSEC_CFG_INTERNAL_IP4_NETMASK        = 2,
  IPSEC_CFG_INTERNAL_IP4_DNS            = 3,
  IPSEC_CFG_INTERNAL_IP4_NBNS           = 4,
  IPSEC_CFG_INTERNAL_ADDRESS_EXPIRY     = 5,
  IPSEC_CFG_INTERNAL_IP4_DHCP           = 6,
  IPSEC_CFG_APPLICATION_VERSION         = 7,
  IPSEC_CFG_INTERNAL_IP6_ADDRESS        = 8,
  /* 9 is Reserved as per RFC 4306 */
  IPSEC_CFG_INTERNAL_IP6_DNS            = 10,
  IPSEC_CFG_INTERNAL_IP6_NBNS           = 11,
  IPSEC_CFG_INTERNAL_IP6_DHCP           = 12,
  IPSEC_CFG_INTERNAL_IP4_SUBNET         = 13,
  IPSEC_CFG_SUPPORTED_ATTRIBUTES        = 14,
  IPSEC_CFG_INTERNAL_IP6_SUBNET         = 15,
  /* from 3GPP2 X.S0028-200 */
  IPSEC_CFG_3GPP2_MIP4_HA               = 16384,
  IPSEC_CFG_3GPP2_MIP4_HOA              = 16385,
  IPSEC_CFG_3GPP2_MIP6_HA               = 16387,
  IPSEC_CFG_3GPP2_MIP6_HOA              = 16388,
  IPSEC_CFG_INTERNAL_IP4_PCSCF          = 16389,
  IPSEC_CFG_INTERNAL_IP6_PCSCF          = 16390,
  /* max value allowed for private use */
  IPSEC_CFG_ATTR_MAX                    = 32767 
} IPSecConfigAttrEType;

/***
 * Config attribute values
 ***/
typedef struct
{
  struct InAddr Addr;
  struct InAddr SubnetMask;
} IPSecCfgAttrV4SubnetType;

typedef struct
{
  struct In6Addr Addr;
  uint8  PrefixLen;
} IPSecCfgAttrV6SubnetType;

/* Config Attribute data types */
typedef struct 
{
  IPSecConfigAttrEType  Type;
  uint32                ValueLen;
  uint32                PrefixLen;
  union 
  {
    uint8*                    pData;
    struct InAddr*            pAddrV4;
    struct In6Addr*           pAddrV6;
    IPSecCfgAttrV4SubnetType* pSubnetV4;
    IPSecCfgAttrV6SubnetType* pSubnetV6;
  } Value;
} IPSecCfgAttrType;

/* List of Error Codes that could be returned by IPsec */
typedef enum
{
  IPSEC_STATUS_RESERVED      = 0,
  IPSEC_IFACE_UP             = 1,
  IPSEC_IFACE_DOWN           = 2,
  IPSEC_DHCP_FAILED          = 3,
  IPSEC_DNS_FAILED           = 4,
  IPSEC_IKE_EAP_AUTH_FAILED  = 5,
  IPSEC_IKE_CERT_INVALID     = 6,
  IPSEC_INTERNAL_ERROR       = 7,
  IPSEC_SEGW_UNREACHABLE     = 8,
  IPSEC_STATUS_MAX
} IPSecStatusEType;

typedef struct
{
  uint32              CfgAttrCnt; /* Number of Config Attributes */
  IPSecCfgAttrType   *pCfgAttrs;  /* List of Config Attribute values */
} IPSecCfgDataType;

/* NotifyData for the IxCallback function */
typedef struct
{
  IPSecStatusEType    Status;
  IPSecCfgDataType    CfgData;
} IPSecInfoType;
/*------------------------------------------------------------------------
                    I K E v 2     D e f i n i t i o n s
------------------------------------------------------------------------*/
/* Maximum num of algorithms in IKE proposal */
#define IPSEC_PROP_MAX_TRANSF_CNT        8

/* Transform IDs for Transform Type 1 (Encryption Algorithm) */
#define IKEV2TRANSF_ENCR_RESERVED_START  22
#define IKEV2TRANSF_ENCR_RESERVED_END    1023
#define IKEV2TRANSF_ENCR_PRIVATE_START   1024
#define IKEV2TRANSF_ENCR_PRIVATE_END     65535
typedef enum
{
  IKEV2TRANSF_ENCR_RESERVED             = 0,
  IKEV2TRANSF_ENCR_DES_IV64             = 1,       /* (RFC1827) */
  IKEV2TRANSF_ENCR_DES                  = 2,       /* (RFC2405) */
  IKEV2TRANSF_ENCR_3DES                 = 3,       /* (RFC2451) */
  IKEV2TRANSF_ENCR_RC5                  = 4,       /* (RFC2451) */
  IKEV2TRANSF_ENCR_IDEA                 = 5,       /* (RFC2451) */
  IKEV2TRANSF_ENCR_CAST                 = 6,       /* (RFC2451) */
  IKEV2TRANSF_ENCR_BLOWFISH             = 7,       /* (RFC2451) */
  IKEV2TRANSF_ENCR_3IDEA                = 8,       /* (RFC2451) */
  IKEV2TRANSF_ENCR_DES_IV32             = 9,
  /*      RESERVED                        10 */
  IKEV2TRANSF_ENCR_NULL                 = 11,      /* (RFC2410) */
  IKEV2TRANSF_ENCR_AES_CBC              = 12,      /* (RFC3602) */
  IKEV2TRANSF_ENCR_AES_CTR              = 13,      /* (RFC3686) */
  IKEV2TRANSF_ENCR_AES_CCM_8            = 14,      /* (RFC4309) */
  IKEV2TRANSF_ENCR_AES_CCM_12           = 15,      /* (RFC4309) */
  IKEV2TRANSF_ENCR_AES_CCM_16           = 16,      /* (RFC4309) */
  /*      Unassigned                      17 */
  IKEV2TRANSF_ENCR_AES_GCM_ICV8         = 18,      /* (RFC4106) */
  IKEV2TRANSF_ENCR_AES_GCM_ICV12        = 19,      /* (RFC4106) */
  IKEV2TRANSF_ENCR_AES_GCM_ICV16        = 20,      /* (RFC4106) */
  IKEV2TRANSF_ENCR_NULL_AUTH_AES_GMAC   = 21,     /* (RFC4543) */
  IKEV2TRANSF_ENCR_MAX
} IKETransf_ENCR_EType; 


/* Transform IDs for Transform Type 2 (Pseudo-random Function) */
#define IKEV2TRANSF_PRF_RESERVED_START  9
#define IKEV2TRANSF_PRF_RESERVED_END    1023
#define IKEV2TRANSF_PRF_PRIVATE_START   1024
#define IKEV2TRANSF_PRF_PRIVATE_END     65535
typedef enum
{
  IKEV2TRANSF_PRF_RESERVED      = 0,
  IKEV2TRANSF_PRF_HMAC_MD5      = 1,       /* (RFC2104) */
  IKEV2TRANSF_PRF_HMAC_SHA1     = 2,       /* (RFC2104) */
  IKEV2TRANSF_PRF_HMAC_TIGER    = 3,       /* (RFC2104) */
  IKEV2TRANSF_PRF_AES128_XCBC   = 4,       /* (RFC4434) */
  IKEV2TRANSF_PRF_HMAC_SHA2_256 = 5,       /* (RFC4868) */
  /* Reserved to IANA             5-7 */
  IKEV2TRANSF_PRF_AES128_CMAC   = 8,       /* (RFC4615) */
  /* Reserved to IANA             9-1023 */
  IKEV2TRANSF_PRF_MAX
} IKETransf_PRF_EType;


/* Transform IDs for Transform Type 3 (Integrity Algorithm) */
#define IKEV2TRANSF_AUTH_RESERVED_START  12
#define IKEV2TRANSF_AUTH_RESERVED_END    1023
#define IKEV2TRANSF_AUTH_PRIVATE_START   1024
#define IKEV2TRANSF_AUTH_PRIVATE_END     65535
typedef enum
{
  IKEV2TRANSF_AUTH_RESERVED        =  0,
  IKEV2TRANSF_AUTH_HMAC_MD5_96     =  1,       /* (RFC2403) */
  IKEV2TRANSF_AUTH_HMAC_SHA1_96    =  2,       /* (RFC2404) */
  IKEV2TRANSF_AUTH_DES_MAC         =  3,
  IKEV2TRANSF_AUTH_KPDK_MD5        =  4,       /* (RFC1826) */
  IKEV2TRANSF_AUTH_AES_XCBC_96     =  5,       /* (RFC3566) */
  IKEV2TRANSF_AUTH_HMAC_MD5_128    =  6,       /* (RFC4595) */
  IKEV2TRANSF_AUTH_HMAC_SHA1_160   =  7,       /* (RFC4595) */
  IKEV2TRANSF_AUTH_AES_CMAC_96     =  8,       /* (RFC4494) */
  IKEV2TRANSF_AUTH_AES_128_GMAC    =  9,       /* (RFC4543) */
  IKEV2TRANSF_AUTH_AES_192_GMAC    =  10,      /* (RFC4543) */
  IKEV2TRANSF_AUTH_AES_256_GMAC    =  11,      /* (RFC4543) */
  IKEV2TRANSF_AUTH_HMAC_SHA2_256   =  12,
  IKEV2TRANSF_AUTH_MAX
} IKETransf_AUTH_EType;


/* AES key size to use */
typedef enum
{
  AES_KEY_SIZE_128         = 0x80,   /* 128 */
  AES_KEY_SIZE_256         = 0x0100  /* 256 */
} IKE_AES_Keysize;

/* Transform IDs for Transform Type 4 (Diffie Hellman Group) */
#define IKEV2TRANSF_DH_RESERVED_START  22
#define IKEV2TRANSF_DH_RESERVED_END    1023
#define IKEV2TRANSF_DH_PRIVATE_START   1024
#define IKEV2TRANSF_DH_PRIVATE_END     65535
typedef enum
{
  /* Transform IDs for Transform Type 4 (Diffie-Hellman Group) */
  IKEV2TRANSF_DH_RESERVED       = 0,    /* Appendix B */
  IKEV2TRANSF_DH_MODP768        = 1,    /* Appendix B */
  IKEV2TRANSF_DH_MODP1024       = 2,
  IKEV2TRANSF_DH_EC2N155        = 3, 
  IKEV2TRANSF_DH_EC2N185        = 4,
  IKEV2TRANSF_DH_MODP1536       = 5,    /* (RFC3526) */
  /*      Reserved to IANA      6-13 */
  IKEV2TRANSF_DH_MODP2048       = 14,   /* (RFC3526) */
  IKEV2TRANSF_DH_MODP3072       = 15,   /* (RFC3526) */
  IKEV2TRANSF_DH_MODP4096       = 16,   /* (RFC3526) */
  IKEV2TRANSF_DH_MODP6144       = 17,   /* (RFC3526) */
  IKEV2TRANSF_DH_MODP8192       = 18,   /* (RFC3526) */
  IKEV2TRANSF_DH_ECP256         = 19,   /* (draft-ietf-ipsec-ike-ecp-groups-02) */
  IKEV2TRANSF_DH_ECP384         = 20,   /* (draft-ietf-ipsec-ike-ecp-groups-02) */
  IKEV2TRANSF_DH_ECP521         = 21,   /* (draft-ietf-ipsec-ike-ecp-groups-02) */
  IKEV2TRANSF_DH_MAX
} IKETransf_DH_EType;

/****
 * IPSec/IKEv2 Life time value in Seconds
 ****/
typedef struct
{
  uint32   SoftLifeSec;
  uint32   HardLifeSec;
} IPSecLifeSecType;

/****
 * IPSec Life time value in Bytes
 ****/
typedef struct
{
  uint32   SoftLifeBytes;
  uint32   HardLifeBytes;
} IPSecLifeBytesType;

/****
 * ESP Protection Type
 ****/
typedef struct
{
  void                      *SAIndex;
  IPSecESPProtectionEType    Protection;
  IKETransf_ENCR_EType       EncrAlgo[IPSEC_PROP_MAX_TRANSF_CNT];   /* IPSec Proposal...ENCR Algo */
  IKETransf_AUTH_EType       AuthAlgo[IPSEC_PROP_MAX_TRANSF_CNT];   /* IPSec Proposal...Auth Algo */
  IPSecLifeSecType           LifeSecs;                              /* Life time in seconds */
  IPSecLifeBytesType         LifeBytes;                             /* Life time in bytes */
} IPSecESPProtectionType;

/****
 * AH Protection Type
 ****/
typedef struct
{
  void                     *SAIndex;
  IPSecAHProtectionEType    Protection;
  IKETransf_AUTH_EType      AuthAlgo[IPSEC_PROP_MAX_TRANSF_CNT];    /* IPSec Proposal...Auth Algo */
  IPSecLifeSecType          LifeSecs;                               /* Life time in seconds */ 
  IPSecLifeBytesType        LifeBytes;                              /* Life time in Bytes */
} IPSecAHProtectionType;

/*------------------------------------------------------------------------
                 SECURITY ASSOCIATION RELATED DEFINITIONS 
------------------------------------------------------------------------*/

/****
 * Option:   SPI
 * Val Type: IPSecSPIType
 ****/
typedef struct 
{
  uint32   SPI;
} IPSecSPIType;

/****
 * Option:   REPLAY_WINDOW
 * Val Type: IPSecReplayWinType
 ****/
typedef struct
{
  uint32 Low;
  uint32 High;
} IPSecReplayWinType;

/****
 * Option:   HASH_ALGO
 *           CRYPTO_ALGO
 * Val Type: AlgoEType
 ****/
typedef enum
{
  IPSEC_ALGO_NULL_KEY_X      = 0,      /* No key exchange protocol used        */
  IPSEC_ALGO_MODEXP          = 1,      /* Generic modulo exponentiation        */
  IPSEC_ALGO_DIFFIE_HELLMAN  = 2,      /* Diffie-Helman Key exchange protocol  */
  IPSEC_ALGO_RSA             = 3,      /* RSA Encrypt / Key exchange protocol  */
  IPSEC_ALGO_NULL_HASH       = 4,      /* No simultaneous hash with enc/dec    */
  IPSEC_ALGO_SHA             = 5,      /* SHA Hash function                    */
  IPSEC_ALGO_SHA256          = 6,      /* SHA256 Hash function                 */
  IPSEC_ALGO_MD5             = 7,      /* MD5 Hash function                    */
  IPSEC_ALGO_NULL_CIPHER     = 8,      /* No encryption selected               */
  IPSEC_ALGO_DES             = 9,      /* DES Encryption-Decryption function   */
  IPSEC_ALGO_3DES            = 10,      /* 3DES Encryption-Decryption function  */
  IPSEC_ALGO_ARC4            = 11,     /* ARC4 Encryption-Decryption function  */
  IPSEC_ALGO_AES128          = 12,     /* AES cipher, 128-bit key              */
  IPSEC_ALGO_C2              = 13,     /* C2 cipher */
  IPSEC_ALGO_MAX,                      /* Must be last in list, to size array  */
} IPSecAlgoEType;

/****
 * Option:   HASH_KEYIN
 *           HASH_KEYOUT
 *           CRYPTO_KEYIN
 *           CRYPTO_KEYOUT
 * Val Type: IPSecKeyType
 ****/
typedef struct
{
  uint8  *pData;
  uint32  dwLen;
} IPSecKeyType;

/****
 * Option:   IV
 * Val Type: IPSecIVType
 ****/
typedef struct
{
  uint8  *pData;
  uint32  dwLen;
} IPSecIVType;


/****
 * Option:   LIFE_KB
 *           LIFE_SEC
 * Val Type: IPSecLifeType
 ****/
typedef uint32 IPSecLifeType;


/* Max number of SA's in case of Multiple SA */
#define IPSECSA_MAX_NUM_SA  3 

  /**
   * Options that can be Set/Retrieved by BREW IPSec Security Association 
   * Interface
   */
  typedef enum
  {
    IPSECSA_OPTION_RESERVED     = 0,  
    IPSECSA_SPI_IN              = 1,  /*! security parameter index */
    IPSECSA_SPI_OUT             = 2,  /*! security parameter index */
    IPSECSA_PROTOCOL            = 3,  /*! esp or ah protocol */
    IPSECSA_ENCAPSULATION_MODE  = 4,  /*! transport or tunnel mode */
    IPSECSA_TUNNEL_DESTINATION  = 5,  /*! Tunnel endpoint */
    IPSECSA_REPLAY_WINDOW       = 6, 
    IPSECSA_LOCAL_ADDR          = 7, 
    IPSECSA_HASH_ALGO           = 8,  /*! hash algorithm */
    IPSECSA_HASH_KEYIN          = 9,  /*! key for input processing */
    IPSECSA_HASH_KEYOUT         = 10, /*! key for output processing */
    IPSECSA_CRYPTO_ALGO         = 11, /*! Encryption algorithm */
    IPSECSA_CRYPTO_KEYIN        = 12, /*! key for input proc */
    IPSECSA_CRYPTO_KEYOUT       = 13, /*! key for output proc */
    IPSECSA_IV                  = 14, /*! Initialization vector */
    IPSECSA_IDII                = 15,
    IPSECSA_IDIR                = 16,
    IPSECSA_LIFE_KB             = 17, /*! Life time in KB */
    IPSECSA_LIFE_SEC            = 18, /*! Life time in Seconds */
    IPSECSA_SA                  = 19, /*! Entire SA */
    IPSECSA_NUM_SA              = 20, /*! Number of SA's */
    IPSECSA_OPTION_COUNT,             /*! THIS MUST BE LAST */
    IPSECSA_OPTION_MAX          = 0x0F000000
  } IPSecSA_OptionEType;

/** @defgroup ipsec_datatypes_SAO2 Security Association Object SetOpt and GetOpt Data Types
  <p>
  The Following table lists the data types that need to be passed when
  calling SetOpt or GetOpt methods on a Security Association Object
  </p>
 
  <pre>
  IPSECSA_SPI_IN            -> IIPSecSPIType
  IPSECSA_SPI_OUT           -> IIPSecSPIType
  IPSECSA_PROTOCOL          -> IIPSecSAProtoEnumType
  IPSECSA_ENCAPSULATION_MODE-> IIPSecEncapsulateEnumType
  IPSECSA_TUNNEL_DESTINATION-> IIPSecAddrType
  IPSECSA_REPLAY_WINDOW     -> IIPsecReplayWindowType
  IPSECSA_LOCAL_ADDR        -> IIPSecAddrType
  IPSECSA_HASH_ALGO         -> IIPSecAlgoEType
  IPSECSA_HASH_KEYIN        -> IIPSecKeyType
  IPSECSA_HASH_KEYOUT       -> IIPSecKeyType
  IPSECSA_CRYPTO_ALGO       -> IIPSecAlgoEType
  IPSECSA_CRYPTO_KEYIN      -> IIPSecKeyType
  IPSECSA_CRYPTO_KEYOUT     -> IIPSecKeyType
  IPSECSA_IV                -> IIPSecIVType
  IPSECSA_IDII              -> IIPSecIdentificationType
  IPSECSA_IDIR              -> IIPSecIdentificationType
  IPSECSA_LIFE_KB           -> IIPSecLifeType
  IPSECSA_LIFE_SEC          -> IIPSecLifeType
  IPSECSA_SA                -> IIPSecSAType
  IPSECSA_NUM_SA            -> uint32
  </pre>
*/
     
   /****
    * Option:   SA
    * Val Type: SAType
    ****/
   typedef struct {
     IPSecSPIType              SPI_IN;
     IPSecSPIType              SPI_OUT;
     IPSecSAProtoEType         Proto;  
     IPSecEncapsulateEType     EncapsMode;
     IPSecAddrType             EndPoint;
     IPSecReplayWinType        ReplayWin;
     IPSecAddrType             LocalAddr;
     IPSecAlgoEType            HashAlgo;
     IPSecKeyType              HashKeyIn;
     IPSecKeyType              HashKeyOut;
     IPSecAlgoEType            CryptoAlgo;
     IPSecKeyType              CryptoKeyIn;
     IPSecKeyType              CryptoKeyOut;
     IPSecIVType               IV;
     IPSecIdentificationType   IDII;
     IPSecIdentificationType   IDIR;
     IPSecLifeType             LifeKB;
     IPSecLifeType             LifeSec;
   } IPSecSAType;

//==========================================================================
//   Type definitions
//==========================================================================
typedef void * IPSecSADBIndexType;


/*---------------------------------------------------------------------------
  Type of IP filters, for ACL_T
---------------------------------------------------------------------------*/

typedef enum
{
  IPSEC_SLCTR_DEFAULT_TYPE = 0,   /* Default filter params and execution rules  */
  IPSEC_SLCTR_MAX_TYPE
} IPSecSlctrTypeEnumType;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                       
                       QOS FILTER VALIDATION RULES
                       
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/** @defgroup filter QOS Filter Validation Rules

<p>
  These are the rules for specifiying a QOS filter spec. If these critera 
  are not met the validation of the filter spec will fail, and QOS
  registration will not succeed.
<p>

<p>
  This section describes the validation rules and semantics for the IP filter 
  spec parameters.
</p>

<ol>
  <li>A bit in field_mask shall be set for each parameter specified, if a bit 
     is not set the corresponding parameter is ignored.
  <li>If one or more parameters are incorrrectly specified, the bits in 
     err_mask indicate those parameters on return.  Hence in case of errors, 
     err_mask should be checked to see if a parameter validation error has 
     occurred which will be cleared otherwise.
  <li>If a parameter consisting of multiple sub fields is specified, each 
     subfield shall also be specified.  Such parameters are,
       a.IP v4/v6 src and dst addr
       b.IP v4 TOS
       c.IP v6 traffic class
       d.TCP/UDP src and dst port
  <li>Each filter will have an IP version associated with it which is either 
     v4 or v6 and is shall be specified by ip_vsn.  It is invalid to specify 
     v6 parameters for a v4 type filter and visa versa. 
  <li>Not all combinations of filter parameters are allowed in each filter.  
     The following table describes the valid combinations.  Only those 
     attributes marked with an "X" may be specified for a single packet 
     filter. All marked attributes may be specified, but at least one shall 
     be specified.

<h3>Table 1: Valid Packet Filter Attribute Combinations</h3>
                 
<pre>
                                         Valid combination types
    Packet filter attribute	                  I   II  III  IV
    Source Address and Subnet Mask	          X   X    X   X
    Destination Address and Subnet Mask	      X   X    X   X
    Protocol Number (IPv4)/Next Header (IPv6)	X	  X    X
    Destination Port Range                    X
    Source Port Range                         X
    ICMP Msg Type                                 X
    ICMP Msg Code                                 X
    IPSec SPI                                          X
    TOS (IPv4)/Traffic Class (IPv6) and Mask	X   X    X   X
    Flow Label (IPv6)                                      X
</pre>
    
  <li>If a parameter from next header protocol (TCP, UDP etc.) is specified in
     a filter, the Protocol Number for v4 filters or Next Header for v6 
     filters shall be specified.
  <li>In most cases IP source address in Tx filters and IP destination address
     in Rx filters is not required to be specified.  In case these values are,
     specified following requirements shall be met.       
<ol mode=alpha>
       <li>IP source address in Tx filters and IP destination address in Rx 
          filters can only have a single address value.  Hence subnet_mask 
          (for v4 filters) or prefix_len (for v6 filters) is ignored for 
          these parameters.
       <li>These address values shall be same as the current IP address 
          assigned to the MS on the IP interface on which the QOS is being 
          requested.
       <li>If the IP address on the interface changes (e.g. during a network 
          handoff), the client which requested QOS  is responsible for 
          updating the filters with the new address or else the flow may not
          be able to receive the desired treatment.
</ol>
  <li>IP destination address in Tx filters and IP source address in Rx filters
     can be specified as address ranges using the subnet_mask (for v4 filters)
     or prefix_len (for v6 filters) parameters.  A subnet_mask may be set to 
     all 1's or prefix_len may be set to 128 to specify a single address 
     value.
  <li>nonzero value for range may be set to specify a range of port numbers 
     starting from the value specified in port [port to port+range], 
     otherwise range shall be set to zero.
  <li>nonzero value for range may be set to specify a range of port numbers 
     starting from the value specified in port [port to port+range], otherwise
     range shall be set to zero.
  <li>fields like address, port numbers etc. shall be specified in 
     network byte order, everything else shall be in host byte order.  
     Following fields shall be specified in netwok byte order:
<ol mode=alpha>
       <li>addresses
       <li>subnet masks
       <li>addresses (prefix length shall be in host order)
       <li>and UDP port numbers (port ranges shall be in host order)
       <li>foe label
</ol>
</ol>
---------------------------------------------------------------------------*/


/* IPV4 hdr fields */
typedef enum
{
  IPSEC_SLCTR_MASK_IP4_NONE          = 0x00,
  IPSEC_SLCTR_MASK_IP4_SRC_ADDR      = 0x01,
  IPSEC_SLCTR_MASK_IP4_DST_ADDR      = 0x02,
  IPSEC_SLCTR_MASK_IP4_NEXT_HDR_PROT = 0x04,
  IPSEC_SLCTR_MASK_IP4_TOS           = 0x08,
  IPSEC_SLCTR_MASK_IP4_ALL           = 0x0f
} IPSecSlctrIP4HdrFieldMaskEnumType;

/* IPV6 hdr fields */
typedef enum
{
  IPSEC_SLCTR_MASK_IP6_NONE          = 0x00,
  IPSEC_SLCTR_MASK_IP6_SRC_ADDR      = 0x01,
  IPSEC_SLCTR_MASK_IP6_DST_ADDR      = 0x02,
  IPSEC_SLCTR_MASK_IP6_NEXT_HDR_PROT = 0x04,
  IPSEC_SLCTR_MASK_IP6_TRAFFIC_CLASS = 0x08,
  IPSEC_SLCTR_MASK_IP6_FLOW_LABEL    = 0x10,
  IPSEC_SLCTR_MASK_IP6_ALL           = 0x1f
} IPSecSlctrIP6HdrFieldMaskEnumType;
		
/* Higher level protocol hdr parameters */
 
/* TCP hdr fields */
typedef enum
{	
  IPSEC_SLCTR_MASK_TCP_NONE          = 0x00,
  IPSEC_SLCTR_MASK_TCP_SRC_PORT      = 0x01,
  IPSEC_SLCTR_MASK_TCP_DST_PORT      = 0x02,
  IPSEC_SLCTR_MASK_TCP_ALL           = 0x03
} IPSecSlctrTCPHdrFieldMaskEnumType;
  
/* UDP hdr fields */
typedef enum
{	
  IPSEC_SLCTR_MASK_UDP_NONE          = 0x00,
  IPSEC_SLCTR_MASK_UDP_SRC_PORT      = 0x01,
  IPSEC_SLCTR_MASK_UDP_DST_PORT      = 0x02,
  IPSEC_SLCTR_MASK_UDP_ALL           = 0x03
} IPSecSlctrUDPHdrFieldMaskEnumType;
    
/* ICMP hdr fields */
typedef enum
{
  IPSEC_SLCTR_MASK_ICMP_NONE         = 0x00,
  IPSEC_SLCTR_MASK_ICMP_MSG_TYPE     = 0x01,
  IPSEC_SLCTR_MASK_ICMP_MSG_CODE     = 0x02,
  IPSEC_SLCTR_MASK_ICMP_ALL          = 0x03
} IPSecSlctrICMPHdrFieldMaskEnumType;

typedef uint8 IPSecSlctrIP4HdrFieldMaskType;
typedef uint8 IPSecSlctrIP6HdrFieldMaskType;
typedef uint8 IPSecSlctrTCPHdrFieldMaskType;
typedef uint8 IPSecSlctrUDPHdrFieldMaskType;
typedef uint8 IPSecSlctrICMPHdrFieldMaskType;

/* ==========================================================================
    Type definitions
========================================================================== */

/* =================================================================
                 SECURITY POLICY RELATED DEFINITIONS 
================================================================= */

/****
 * IP Filter definations that get used for IPSecSP Filters
 ****/

/**
  Type IxIPSecSelectorType (Should be SAME AS ip_filter_type)
  
    This data structure defines the IP filter parameters for a Default filter
    type. A default filter contains all the common parameters required for most 
    of the filtering needs and are processed by a default set of rules 
    (ie pattern matching on parameters).
    
    All the address/port number fields must be specified in network byte 
    order, everything else in host byte order.
  
*/

/*------------------------------------------------------------------------
                               PUBLIC TYPES
------------------------------------------------------------------------*/

typedef struct
{
  /* Mandatory Parameter - IP version of the filter (v4 or v6)	*/
  IPSecAddrEType  IP_Vsn;

  /* Filter parameter values,  the ones set in field masks are only valid */
  /* Correspopnding err mask is set if a parameter value is invalid */
  union
  {
    struct
    {
      IPSecSlctrIP4HdrFieldMaskType     FieldMask;  /* In mask   */
      IPSecSlctrIP4HdrFieldMaskType     ErrMask;    /* Out mask  */

      struct
      {
        struct InAddr  Addr;
        struct InAddr  SubnetMask;
      } Src;

      struct
      {
        struct InAddr  Addr;
        struct InAddr  SubnetMask;
      } Dst;
      
      struct
      {
        uint8 Val;
        uint8 Mask;
      } TOS;      
      
      uint8 NextHdrProt;
    } V4;

    struct
    {
      IPSecSlctrIP6HdrFieldMaskType     FieldMask;  /* In mask   */
      IPSecSlctrIP6HdrFieldMaskType     ErrMask;    /* Out mask  */

      struct
      {
        struct In6Addr   Addr;
        uint8            PrefixLen;
      } Src;
      
      struct
      {
        struct In6Addr  Addr;
        uint8           PrefixLen;
      } Dst;
      
      struct
      {
        uint8   Val;
        uint8   Mask;
      } TrfCls;
      
      uint32   FlowLabel;
      uint8    NextHdrProt;   /* This is transport level protocol header */
    } V6; 
  } IP_Hdr;

  /* next_hdr_prot field in v4 or v6 hdr must be set to specify a    */
  /* parameter from the next_prot_hdr                                */
  union
  {
    struct
    {
      IPSecSlctrTCPHdrFieldMaskType     FieldMask;   /* In mask   */
      IPSecSlctrTCPHdrFieldMaskType     ErrMask;     /* Out mask  */

      struct
      {
        uint16    Port;
        uint16    Range;
      } Src;
    
      struct
      {
        uint16    Port;
        uint16    Range;
      } Dst;    
    } TCP;
    
    struct
    {
      IPSecSlctrUDPHdrFieldMaskType     FieldMask;  /* In mask   */
      IPSecSlctrUDPHdrFieldMaskType     ErrMask;    /* Out mask  */

      struct
      {
        uint16    Port;
        uint16	  Range;
      } Src;
      
      struct
      {
        uint16    Port;
        uint16    Range;
      } Dst;
    } UDP;
   
    struct
    {    
      IPSecSlctrICMPHdrFieldMaskType    FieldMask; /* In mask   */
      IPSecSlctrICMPHdrFieldMaskType    ErrMask;   /* Out mask  */

      uint8   Type;
      uint8   Code;
    } ICMP;
  } NextProtHdr; 
} IPSecSlctrType;

typedef struct
{
  uint8            u8NumSlctr;      /* Num selector in the list     */
  IPSecSlctrType  *pSlctrList;       /* Array of selectors             */
} IPSecSlctrSpecType;

#define IIPSECSP_MAX_FLTR_IP_HDR_LEN     sizeof(FltrIPHdrV6_Type)
#define IIPSECSP_MAX_FLTR_TRANS_HDR_LEN  sizeof(FltrTransportHdrTCP_Type)
#define IIPSECSP_MAX_ADDR_LEN            sizeof(IPSecAddrType)
#define IIPSECSP_MAX_AUTH_DATA_LEN       512
#define IIPSECSP_MAX_GROUPNAME_LEN       128
#define IIPSECSP_MAX_PSK_SIZE            128
#define IIPSECSP_MAX_SELECTORS           10

typedef enum
{
  IPSecSP_DIRECTION_RESERVED = 0,
  IPSecSP_DIRECTION_IN       = 1,
  IPSecSP_DIRECTION_OUT      = 2,
  IPSecSP_DIRECTION_INOUT    = 3,
  IPSecSP_DIRECTION_MAX
} IPSecSP_DirectionEType;

typedef struct
{
  IKETransf_ENCR_EType  EncrAlgo[IPSEC_PROP_MAX_TRANSF_CNT];     /* IKE ENCR Prop */
  IKETransf_AUTH_EType  HashAlgo[IPSEC_PROP_MAX_TRANSF_CNT];     /* IKE Auth Algo */
  IKETransf_PRF_EType   PRF_Algo[IPSEC_PROP_MAX_TRANSF_CNT];     /* PRF Algo */
  IKETransf_DH_EType    DH_Group[IPSEC_PROP_MAX_TRANSF_CNT];     /* DH Group */

  IPSecLifeSecType      LifeSecs; /* IKE life time in secs...no bytes */
  boolean               EnableNATT;
  uint32                KeepAliveSecNAT;

  boolean               EnableDPD;
  uint32                KeepAliveSecDPD;
  uint32                RetransmitIntervalSecs;
  uint32                MaxRetransmissions;
} IPSecSP_IKE_ProtectionType;

typedef struct
{
  IPSecKeyMgmtEType          KeyMgmt;
  IPSecEncapsulateEType      EncapsMode;
  IPSecAddrType              EndPoint;
  IPSecGwParamType           GwParam;
  IPSecSP_IKE_ProtectionType IKEProtection;

  IPSecESPProtectionType     ESPProtection;
  IPSecAHProtectionType      AHProtection;
} IPSecSP_ProtectionType;

/****
 *
 * Protection Policy
 *   Selector defines what is protected
 *   Protection Suite defines how it is protected
 *
 ****/
typedef struct
{
  uint32                   PhysIfaceId; /* Phys IFace to route IKE traffic */
  IPSecIdentificationType  SelfID;
  IPSecIdentificationType  PeerID;
  IPSecSP_DirectionEType   Direction;
  IPSecSlctrSpecType       Selector;
  IPSecSP_ProtectionType   ProtectionSuite;
  IPSecCfgDataType         CfgAttrs;
} IPSecSP_ProtectionPolicyType;

/*****
 * Options that can be Set/Retrieved by BREW IPSec Security Policy Interface
 *****/
typedef enum
{
  IPSECSP_OPTION_RESERVED              = 0,
  IPSECSP_PROTECTION_POLICY            = 1, /* Entire policy in one go */
  IPSECSP_SELECTOR                     = 2, /* Selector or traffic filter info */
  IPSECSP_SELECTOR_SRC_ADDR            = 3,
  IPSECSP_SELECTOR_SRC_ADDR_PREFIX_LEN = 4,
  IPSECSP_SELECTOR_SRC_NETMASK         = 5,
  IPSECSP_SELECTOR_DST_ADDR            = 6,
  IPSECSP_SELECTOR_DST_ADDR_PREFIX_LEN = 7,
  IPSECSP_SELECTOR_DST_NETMASK         = 8,
  IPSECSP_SELECTOR_PROTOCOL            = 9,
  IPSECSP_SELECTOR_SRC_PORT            = 10,
  IPSECSP_SELECTOR_DST_PORT            = 11,
  IPSECSP_PROTECTION                   = 12, /* Entire protection Suite */
  IPSECSP_ENCAPSULATION_MODE           = 13, /* Transport/Tunnel Mode */
  IPSECSP_ENDPOINT                     = 14, /* End Point */
  IPSECSP_GW_PARAM                     = 15, /* Gateway Parameters */
  IPSECSP_ESP_PROTECTION               = 16, /* ESP Protection for this policy */
  IPSECSP_AH_PROTECTION                = 17, /* AH Protection for this policy */
  IPSECSP_ISAKMP_MODE                  = 18,
  IPSECSP_GROUP_NAME                   = 19,
  IPSECSP_AUTH_PARAM_DATA              = 20,
  IPSECSP_KEY_MANAGEMENT               = 21, /* Denotes IKE or Manual Keying */
  IPSECSP_ESP_SA_HANDLE                = 22, /* For Manual Keying Only */
  IPSECSP_AH_SA_HANDLE                 = 23, /* For Manual Keying Only */
  IPSECSP_DIRECTION                    = 24, /* Incoming or Outgoing Policy */
  IPSECSP_IKE_PROTECTION               = 25, /* Encryption algorithm for IKEv2 */
  IPSECSP_SELF_ID                      = 26, /* IDII for IKE negotiation */
  IPSECSP_PEER_ID                      = 27, /* IDIR for IKE negotiation */
  IPSECSP_CONFIG_ATTR                  = 28, /* Configuration attributes */
  IPSECSP_PHYS_IFACE_ID                = 29, /* Physical IFace ID for IKE traffic...needs to be brought up by app */
  IPSECSP_DPD_TIMER                    = 30, /* DPD timer value in secs...uint32 */
  IPSECSP_OPTION_COUNT,                      /* This must be last */
  IPSECSP_OPTION_MAX                   = 0xFFFFFFFF
} IPSecSP_OptionEType;

/**===========================================================================
*  Data types correcponding to each of the enum values above that need to
*  be passed in opt_val in setopt and getopt functions.
*
*  <table border="1">
*
*  <tr> <th> Option Types </th> <th> Option Val Type </th> </tr>
*
*  <tr><td>
*                IPSECSP_PROTECTION_POLICY
*  </td><td>
*                IPSecSP_ProtectionPolicyType
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_PROTECTION_SELECTOR
*  </td><td>
*                IPSecSlctrType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_SELECTOR_SRC_ADDR
*                IPSECSP_SELECTOR_DST_ADDR
*                IPSECSP_SELECTOR_SRC_NETMASK
*                IPSECSP_SELECTOR_DST_NETMASK
*  </td><td>
*                IPSecAddrType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_SELECTOR_SRC_ADDR_PREFIX_LEN
*                IPSECSP_SELECTOR_DST_ADDR_PREFIX_LEN
*  </td><td>
*                IPSecPrefixLenType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_PROTECTION_SELECTOR_PROTOCOL
*  </td><td>
*                IPSecProtocolType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_PROTECTION_SELECTOR_SRC_PORT
*                IPSECSP_PROTECTION_SELECTOR_DST_PORT
*  </td><td>
*                IPSecPortType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_PROTECTION_SUITE
*  </td><td>
*                IIPSecSP_ProtectionType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_ENDPOINT
*  </td><td>
*                IPSecAddrType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_GW_PARAM
*  </td><td>
*                IIPSecSP_GwParamData
*
*
*  </td></tr>
*  <tr><td>
*                IIPSECSP_ESP_PROTECTION_TYPE
*  </td><td>
*                IPSecSP_ESP_ProtectionEnumType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_AH_PROTECTION_TYPE
*  </td><td>
*                IIPSecSP_AH_ProtectionEnumType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_ISAKMP_MODE
*  </td><td>
*                IIPSecSP_IsakmpModeParamDataEnumType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_GROUP_NAME
*  </td><td>
*                IIPSecSP_GroupNameParamData
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_AUTH_PARAM_DATA
*  </td><td>
*                IIPSecSP_AuthParamData
*                Auth Param Data should be NULL in case of EAP-SIM
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_DIRECTION
*  </td><td>
*                IPSecSP_DirectionEType
*
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_SELF_ID
*                IPSECSP_PEER_ID
*  </td><td>
*                IPSecIdentificationType
*
*  </td></tr>
*  <tr><td>
*                IPSECSP_CONFIG_ATTR
*  </td><td>
*                IPSecCfgAttrType
*/

typedef void * IPSecSPDBIndexType;

#endif /* IPSEC_H */
