#ifndef PDSM_STUBS_H
#define PDSM_STUBS_H

#define PDSM_PA_MAX_APN_NAME_LENGTH 100

/* Client ID type to differentiate PDSM client objects from one another.pdsm_pa_apn_profiles_type

*/
typedef int  pdsm_client_id_type;
typedef uint32 pdsm_atl_type;
typedef struct {  
  uint32  srv_system_type; /*Serving system on which this APN should be used*/
  uint32  pdp_type; /*Pdp type of the APN profile*/
  uint32  reserved; /*Reserved for future use*/
  uint8   apn_name[PDSM_PA_MAX_APN_NAME_LENGTH]; /*APN name*/
} pdsm_pa_apn_profiles_type;

/*---------------------------------------------------------------------------
                          Multiple PDP context
                     (Copy from "dsumtspdpreg.h")
---------------------------------------------------------------------------*/
#define  PDSM_ATL_MAX_APN_STRING_LEN    100   /* Max length of APN string    */

typedef enum
{
  PDSM_ATL_CONNECTION_LBS=0x0,        /*LBS connection*/
  PDSM_ATL_CONNECTION_WWAN_INTERNET,  /*ANY WWAN Internet connection*/
  PDSM_ATL_CONNECTION_MAX = 0x1000000
} pdsm_atl_connection_e_type;

typedef enum
{
  PDSM_ATL_PDP_IP=0x0,                     /* PDP type IP (IPV4)          */
  PDSM_ATL_PDP_PPP,                        /* PDP type PPP                */
  PDSM_ATL_PDP_IPV6,                       /* PDP type IPV6               */
  PDSM_ATL_PDP_IPV4V6,                     /* PDP type IPV4V6             */
  PDSM_ATL_PDP_MAX=0x10000000              /* Force enum to be 32 bit     */
} pdsm_atl_pdp_type_e_type;



/*---------------------------------------------------------------------------
TYPEDEF PDSM_ATL_ADDR_FAMILY_ENUM_TYPE

DESCRIPTION
  enum which discriminates the union that defines the IP addresses type 
  in the structure pdsm_atl_ip_addr_type
---------------------------------------------------------------------------*/
typedef enum
{
  PDSM_ATL_IP_V4  = 1,                    /* Protocol Family - Internet v4 */
  PDSM_ATL_IP_V6  = 2                     /* Protocol Family - Internet v6 */
} pdsm_atl_addr_family_e_type;

/*---------------------------------------------------------------------------
TYPEDEF PDSM_ATL_IP_ADDR_TYPE

DESCRIPTION
  structure which is a discriminated union that defines the IP addresses that
  we support.
---------------------------------------------------------------------------*/
typedef struct
{
  pdsm_atl_addr_family_e_type type; 

  union
  {
    uint32 v4_addr;   
    /*~ IF (_DISC_ == PDSM_ATL_IP_V4) pdsm_atl_ip_addr_type.addr.v4_addr*/
    uint8 v6_addr[16];
    /*~ IF (_DISC_ == PDSM_ATL_IP_V6) pdsm_atl_ip_addr_type.addr.v6_addr */
    /*~ DEFAULT pdsm_atl_ip_addr_type.addr.void */
  } addr; 
  /*~ FIELD pdsm_atl_ip_addr_type.addr DISC pdsm_atl_ip_addr_type.type*/
} pdsm_atl_ip_addr_type;

/*---------------------------------------------------------------------------
  Definition of iface names
---------------------------------------------------------------------------*/
typedef enum pdsm_atl_iface_e {
  PDSM_ATL_IFACE_NONE = -1, 
  PDSM_ATL_IFACE_CDMA_SN = 0,
  PDSM_ATL_IFACE_CDMA_AN,
  PDSM_ATL_IFACE_UMTS,
  PDSM_ATL_IFACE_SIO,
  PDSM_ATL_IFACE_LO,
  PDSM_ATL_IFACE_WWAN,
  PDSM_ATL_IFACE_ANY_DEFAULT,
  PDSM_ATL_IFACE_ANY,
  PDSM_ATL_IFACE_RM,
  PDSM_ATL_IFACE_MAX
} pdsm_atl_iface_name_e_type;



/*---------------------------------------------------------------------------
TYPEDEF PDSM_ATL_OPEN_PARAM_TYPE

DESCRIPTION
  structure which used by the pdsm_atl_open_f_type functions
---------------------------------------------------------------------------*/
typedef struct
{
  pdsm_atl_iface_name_e_type   iface_name;    /* Interface name */
  pdsm_atl_addr_family_e_type  iface_family;  /* Interface family */
  pdsm_atl_ip_addr_type        iface_ip_addr; /* Interface IP address, IPv4 or IPv6 */
  pdsm_atl_pdp_type_e_type     pdp_type;      /* PDP call type*/
  pdsm_atl_connection_e_type   connection_type;  /* Connection type*/
  char                         apn_name[PDSM_ATL_MAX_APN_STRING_LEN+1];  /* APN Name */
  /*~ FIELD pdsm_atl_open_params_type.apn_name STRING 101 */
} pdsm_atl_open_params_type;

/*---------------------------------------------------------------------------
                  PDSM ATL Events from Communication Manager
---------------------------------------------------------------------------*/
typedef int pdsm_atl_session_handle_type;
typedef uint32 pdsm_atl_event_type;
typedef uint32 pdsm_atl_open_f_type;
typedef uint32 pdsm_atl_close_f_type;
/** System modes.
*/
typedef enum
{
  /** @cond
  */
  SYS_SYS_MODE_NONE = -1,  /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

  SYS_SYS_MODE_NO_SRV=0,
    /**< No service; NV_MODE_INACTIVE. */

  SYS_SYS_MODE_AMPS=1,
    /**< Analog Mobile Phone System (AMPS) mode. */

  SYS_SYS_MODE_CDMA=2,
    /**< CDMA mode. */

  SYS_SYS_MODE_GSM=3,
    /**< GSM mode. */

  SYS_SYS_MODE_HDR=4,
    /**< HDR mode. */

  SYS_SYS_MODE_WCDMA=5,
    /**< WCDMA mode. */

  SYS_SYS_MODE_GPS=6,
    /**< GPS mode. */

  SYS_SYS_MODE_GW=7,
    /**< GSM and WCDMA mode. */

  SYS_SYS_MODE_WLAN=8,
    /**< WLAN mode. */

  SYS_SYS_MODE_LTE=9,
    /**< LTE mode. */

  SYS_SYS_MODE_GWL=10,
    /**< GSM, WCDMA, and LTE mode. */

  SYS_SYS_MODE_TDS=11,
  /**< TDS mode. */

  /** @cond
  */
  SYS_SYS_MODE_MAX   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} sys_sys_mode_e_type;

#endif /* PDSM_STUBS_H */
