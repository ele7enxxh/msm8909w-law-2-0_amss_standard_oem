
#ifndef PDAPI_H
#define PDAPI_H

/*===========================================================================

    Position Determination Session Manager Header File

DESCRIPTION
  This header file contains all the definitions necessary for
  clients to interface with the Position Determination module.

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/inc/pdapi.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- .
04/27/16   ak      Added ENU velocity and uncertainity fields to pdsm_pd_data_ext_s_type.
11/11/15   ms      Add NHz Support
09/22/15   rk      Rename AGLO 3G UP position method type similar to AGPS (i.e. protocol centric)
09/16/15   skm     BDS Health delete fix 
09/07/15   jv      Removed obsolete comment for iode in pdsm_GnssSvInfoType
07/30/15   js      Remove NMEA conflict between GAGGA and GSV_EXTENDED  
07/03/15   jv      Added QZSS Support
06/10/2015 muk	   Added support for Secure Get Available Location
05/26/15   jv      sbas_user_preference changed from bool to uint8
05/19/15   ssu     Adding GM in memory logging.
05/22/15   skm     Added new DPO disengage reason
05/01/15   rh      Added get gps lock synchrous API for LBS
03/27/15   ssu     GM LOWI Integration
01/14/15   rk      Added LPP >24 AD NV support.
01/08/15   skm     Return End event (NO_SESS) when session end called, when there is not active session. 
09/15/14   jv      Added support for Galio GNSS
08/21/14   jv      Added support for 16-bit SVID
05/12/14   ah      Added support for Set XTRA Version Check
04/14/14   ah      Added support for DPO disengage reason for GSM
03/19/14   yy      Change prisoner sv to gnss sv blacklist
03/19/14   rh      Added PrisonerSV support 
03/10/14   ah      Added support for Set Premium Services Cfg & Get Avail WWAN Pos
01/18/14   msk     Added PA commands support for PDSM_PA_SUPL_UDP_ENABLE_CONFIG 
06/13/13   mj      Add BEST_AVAIL_POS into the status event mask define.
06/13/13   rh      Added pdsm_GnssNavSvInfo in PDAPI for SV info
06/12/13   ah      Updated PDSM_GNSS_MAX_SATELLITES to include BDS (for SVs in view)
06/07/13   rh      Changed PA_DELETE bitmask to 64 bit for BDS support
05/01/13   ss      Added support for SUPL certificate operation with NO EFS access
04/01/13   ss      Added support to report SUPL Session End Indication
03/18/13   ss      Added support to report Network Initiated Session Reject Reason
12/19/12   rk      Added pdsm sensor (gyro/accel) bias parameter structure
12/04/12   rk      Extended SV parameters in the PD Fix Report structure
11/25/13   msk     Added SUPL end reasons to pdsm_pd_end_e_type enum structure 
11/08/12   ssu     Added pdsm_end_session_ex()
10/09/12   rh      Added a few error codes for IS801 sessions 
10/03/12   rh      Added a few PA commands for SUPL configuration  
08/22/12   rh      Added Cell Info and NI message injection API   
06/13/12   rk      Added new parameter 'PDSM_PA_ASST_GLONASS_POS_PROTOCOL' to get/set 
                   positioning protocols (RRLP, RRC, LPP) for A-Glonass U & C plane.
06/11/12   rk      Added new extended 'pdsm_client_pa_reg_ex' function to register 
                   PDSM PA event parameter callbacks with PDSM.
04/18/12   ssu     Added pdsm_pd_get_best_avail_pos() 
04/05/12   gk      Added floating point value for time unc in delete params API
03/29/12   ssu     Adding data type for engine DPO status report. 
02/22/12   rh      Added pdsm_pd_inject_external_position()
02/16/12   gk      Added floating point value for altitude
12/06/11   jb      Added PDSM_PD_CMD_INJECT_MOTION_DATA command
10/06/11   rh      Added PDSM_PA_PE_CONFIG_PARAM command
06/02/11   rh      Added pdsm_lcs_set_vx_config() and Get/Set SUPL security
04/13/11   rh      Added support for Get/Set SUPL version
04/08/11   ss      Added support for enabling/disabling data
04/08/11   ss      Added support for Get/Set PDCOMM APN profiles.
03/28/11   rh      Added heading untertainty, azimuth and elevation in rad.
02/16/11   rh      Added time untertainty gps_time_unc_ms as float value
02/10/11   ad      Added altitude source, altitude linkage (with horizontal position)
                   and altitude uncertainty coverage in position injection
01/07/11   vp      Added extended time source variable and values
12/01/10   ad      Added PDSM_PD_POSITION_SOURCE_EPI
11/08/10   ad      Added 2 new APIs for coarse position injection, 2 XTRA-T set/get parameters. Fix report
                   enhanced with 2 new reliability fields and 2 new sensor fields.
09/13/10   rh      Added new error codes for PDSM's PD/PA/LCS/XTRA/XTRA-T commands
                   Added 0xF0/0xF1 values to support DPO Control update without NV change
07/26/10   sj      Added PDSM_PD_HEADING_VALID flag to indicate valid heading for position fix,
                   Added new COMM protocol type, PDSM_PD_COMM_PROTOCOL_1X_CP.
12/07/09   kc      Added new XSPI event and related api
09/01/09   gk      Added comments for EFS_DATA operation
04/02/10   gk      Added an event callback to inject position
01/11/10   rb      Add a flag to indicate that UTC time is valid in the position structure

12/07/09   gk      Added comments for position strucutres, unified header files 
                   accross all branches 4.1 and higher
10/26/09   gk      Added comments for position strucutres, addes SENSORS client
10/09/09   rb      Added Operation Mode for Cell ID
09/01/09   gk      Added comments for EFS_DATA operation
08/23/09   gk      Added new event for guard timer expiry and MM ind
08/10/09   gk      Added new validity flags in the position report
07/23/09   gk      Added DELETE_SV_NO_EXIST flag for delete params
07/23/09   gk      Added new END reason
05/21/09   gk      Add new defines for position modes
05/14/09   gk      fixed some typos
02/10/09   jd      Added WPS fix and scan log reporting
02/03/09   atien   Support MOLR vs AD call flow
                   Remove unused client_data_ptr in pdsm_xtra_t_get_celldb_report_s_type
01/02/09   atien   Support XTRA-T
01/05/09   ns      Add WPS Event
06/15/08   kj      De-Featurize this Public API header file
06/12/08   lt      Support for external coarse position injection.
04/26/08   gk      On-Demand Changes
11/13/07   gk      SUPL certivicate upload via pdapi
09/14/07   gk      NMEA CONFIG set parameters support
07/20/07   gk      Removed obsoleted functions
06/26/07   rw      XTRA related changes
06/18/07   ld      Added support for Deleting Cell DB Information.
05/27/07   rw      Additonal support for JGPS (KDDI) protocol
05/23/07   gk      removed multiple definition of msec.
05/09/07   gk      KDDI related changes.
05/01/07   lt      Restored mo_method enum. back to MO_CP and MO_UP.
04/25/07   gk      Added security and WM related changes.
04/16/07   gk      Renumbered delete params.
04/17/07   lt      Change to expand mo_method enum to include request type of LocEst and AssistData.
04/16/07   gk      Renumbered delete params.
04/02/07   gk      Added Clock  injection API, PDE network address
03/02/07   gk      Added SUPL QOP to the notify verify and VX changes
02/15/07   gk      fixed HTORPC errors for dual proc
02/14/07   gk      Made flags for SUPL and UMTS_CP as 16 bytes 
02/06/07   gk      Modified SUPL notify verify, fixed lint errors 
01/19/07   gk      Fixed HTORPC syntax errors 
01/11/07   gk      Changed Delete parms to include more info 
12/06/06   gk      Changed LCS API, measurement report changes
12/04/06   gk      Added LCS API and minor mods added some of the deleted OR stuff, code review changes
11/08/06   gk      PDAPI for converged GPS
===========================================================================*/


#include "comdef.h"    /* Definition for basic types and macros */
#include "qw.h"
#include "locEng_sm_common_msg_v01.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define PDSM_API_MAJ_V 8
#define PDSM_API_MIN_V 3 /*<<--- QZSS:ATTENTION! Merge version number properly*/

/* ========================================================================
** ================ PDSM Client Interface Block ===========================
** ========================================================================
*/

/**
 * @brief Minimum TBF value for fix rate parameters
 *
 * This value is used as a lower limit for TBF parameter.
 *
 * @sa pdsm_pd_fix_rate_s_type
 */
#define PDSM_MINIMUM_TBF_MS (100) /* 100 milliseconds */


/* Client ID type to differentiate PDSM client objects from one another.
*/
typedef int  pdsm_client_id_type;

typedef enum pdsm_version_e 
{
  PDSM_VERSION_NUMBER_SIX = 6,
  PDSM_VERSION_NUMBER_SEVEN = 7
} pdsm_version_e_type;


typedef enum pdsm_client_type_e {

   PDSM_CLIENT_TYPE_NONE = -1,        /* FOR INTERNAL USE OF PDSM ONLY! */

   PDSM_CLIENT_TYPE_BROWSER,          /* Reserved for browser Client */
   PDSM_CLIENT_TYPE_UI,               /* Reserved for UI running on Same processor */
   PDSM_CLIENT_TYPE_PDA,              /* Reserver for KALI PDA */
   PDSM_CLIENT_TYPE_DM,               /* For DM over RPC */
   PDSM_CLIENT_TYPE_NI,               /* Reserved for NI Clients */
   PDSM_CLIENT_TYPE_BREW,             /* Reserved for BREW clients */
   PDSM_CLIENT_TYPE_JAVA,             /* Reserved for Java Clients */
   PDSM_CLIENT_TYPE_TEST1,            /* Test client types, Added/Deleted in future */
   PDSM_CLIENT_TYPE_TEST2,            /* Test client types, Added/Deleted in future */
   PDSM_CLIENT_TYPE_OEM,              /* Reserved for OEM Clients */
   PDSM_CLIENT_TYPE_TEST3,            /* Reserved for KDDI Clients */
   PDSM_CLIENT_TYPE_XTRA,             /* Reserved for gpsOneXTRA Clients */
   PDSM_CLIENT_TYPE_ATCOP,            /* Reserved for ATCOP Clients */
   PDSM_CLIENT_TYPE_XTRA_T,           /* Reserved for gpsOneXTRA-T Clients */
   PDSM_CLIENT_TYPE_WIPER,            /* Reserved for WIPER Clients */
   PDSM_CLIENT_TYPE_ODP,              /* Reserved for ODP Client */
   PDSM_CLIENT_TYPE_SENSORS,          /* Reserved for SENSOR Client */
   PDSM_CLIENT_TYPE_MAX,              /* FOR INTERNAL USE OF PDSM ONLY! */

   PDSM_CLIENT_TYPE_E_SIZE=0x100000 /* To force enum to int */

} pdsm_client_type_e_type;

/* --------------------------------------------------------------------
** Type declarations for PDSM Session Command.
** --------------------------------------------------------------------
*/
/*
** Service types for Position Determination.
** User is allowed to specify a bitmask of different service types.( Position is
** always provided as part of PD info )
*/

typedef word pdsm_pd_svc_type;

#define PDSM_PD_SVC_TYPE_POSITION  0x01  /* Service Option for Getting Position only */

#define PDSM_PD_SVC_TYPE_VELOCITY  0x02  /* Service Option for Getting Speed only */

#define PDSM_PD_SVC_TYPE_HEIGHT    0x04  /* Service Type Height */

#define PDSM_PD_SVC_TYPE_MASK ((PDSM_PD_SVC_TYPE_POSITION) | (PDSM_PD_SVC_TYPE_VELOCITY) | (PDSM_PD_SVC_TYPE_HEIGHT) )
                                       /* Valid Service type mask */

/*
** Option types for Position Determination.
** Option type is not used as bitmask. Since user is registered he will anyway be
** informed about other PD events asynchronously even if he hasn't called the fnc.
*/

typedef enum pdsm_pd_option_e {
   PDSM_PD_SESS_TYPE_MIN = -1,            /* Internal use of PDSM */
   PDSM_PD_SESS_TYPE_LATEST,              /* Get latest PD info */
   PDSM_PD_SESS_TYPE_NEW,                 /* Get new position */
   PDSM_PD_SESS_TYPE_TRACK_IND,           /* Tracking mode independent fixes */
   PDSM_PD_SESS_TYPE_DATA_DLOAD,          /* Data download option */
   PDSM_PD_SESS_TYPE_MAX,                 /* For internal use of PDSM */
   PDSM_PD_SESS_TYPE_E_SIZE=0x10000000    /* Fix the enum size to int */
} pdsm_pd_session_e_type;

typedef enum {
  PDSM_SERVER_OPTION_MIN = -1,
  PDSM_SERVER_OPTION_USE_DEFAULT,         /* Use Default settings for TLM type+adrs */
  PDSM_SERVER_OPTION_USE_LOCAL,           /* Use param's along with fn call */
  PDSM_SERVER_OPTION_MAX = 0x10000000
} pdsm_server_option_e_type;

typedef enum 
{
   PDSM_SERVER_TYPE_MIN = -1,
   PDSM_SERVER_TYPE_1X_PDE,
   PDSM_SERVER_TYPE_UMTS_PDE,
   PDSM_SERVER_TYPE_MPC,
   PDSM_SERVER_TYPE_MAX = 0x10000000
} pdsm_server_e_type;

typedef enum {
  PDSM_SERVER_ADDR_TYPE_MIN = -1,
  PDSM_SERVER_TYPE_IPV4_ADDRESS,
  PDSM_SERVER_TYPE_URL_ADDRESS,
  PDSM_SERVER_TYPE_IPV6_ADDRESS,
  PDSM_SERVER_ADDR_TYPE_MAX = 0x1000000
} pdsm_server_address_e_type;

/*The reliability indicator is used to show the probability of a position 
  outlier. “Position outlier” is defined as the truth position being grossly 
  inconsistent with the Gaussian-model uncertainty parameters – e.g. not within
  10-standard deviations. A higher reliability metric means lower  
  probability of having a position outlier. Basically, this indicator tells, to 
  what extent the position can be trusted.  
  (Four levels of the reliability metric are defined).

  Benefit: Together with other information in the fix report such as position 
  uncertainty, the reliability indicator provides additional information.  This 
  allows a user of this information to make better use of a given position. For 
  example, a very low reliability output may be used simply for pre-loading 
  likely-needed-soon map information tiles, but not for end-user display.  
  Additionally, if external positioning sources are available, the reliability
  of the GNSS solution (cross-check with injected position if available) may be
  used for other system level comparisons.

  Definition of cross-check: Using redundant information to verify a position
  fix. Information here includes GNSS measurements, externally provided 
  positions, and any other input that can help the positioning process.

  How to set the reliability indicator[input]: It is suggested that reliability
  indicator is not used by customers unless they have an advanced usage plan
  for this input or output.  Pre-existing GPS system performance will remain
  the same if this feature is not used (set to 
  PDSM_POSITION_RELIABILITY_NOT_SET).
*/

typedef enum
{
  PDSM_POSITION_RELIABILITY_NOT_SET = 0, /* Not set, reliability info is not present */
  PDSM_POSITION_RELIABILITY_VERY_LOW,    /* Not checked, use at user's own risk */
  PDSM_POSITION_RELIABILITY_LOW,         /* Limited cross-checked */
  PDSM_POSITION_RELIABILITY_MEDIUM,      /* Medium level cross-checked */
  PDSM_POSITION_RELIABILITY_HIGH,        /* Robust cross-checked */
  PDSM_POSITION_RELIABILITY_MAX = 0x10000000    /* Forced to 32 bit */
} pdsm_pd_reliability_e_type;

/* Altitude source of injected position
*/
typedef enum
{
  PDSM_ALTITUDE_SOURCE_UNKNOWN = 0,
  PDSM_ALTITUDE_SOURCE_GPS,
  PDSM_ALTITUDE_SOURCE_CID,
  PDSM_ALTITUDE_SOURCE_ECID,
  PDSM_ALTITUDE_SOURCE_WIFI,
  PDSM_ALTITUDE_SOURCE_TERRESTRIAL,
  PDSM_ALTITUDE_SOURCE_TERRESTRIAL_HYBRID,
  PDSM_ALTITUDE_SOURCE_ALTITUDE_DATABASE,
  PDSM_ALTITUDE_SOURCE_BAROMETRIC_ALTIMETER,
  PDSM_ALTITUDE_SOURCE_OTHER,
  PDSM_ALTITUDE_SOURCE_MAX = 0x10000000    /* Forced to 32 bit */
} pdsm_pd_altitude_source_e_type;

/* Horizontal position and altitude source linkage.
   This field specifies the dependence between the horizontal
   position and altitude components of injected position.
   This information provides the ability to the position
   engine to remove the bad components of position independently
   or dependently.
*/
typedef enum
{
  PDSM_ALT_HOR_LINK_NOT_SPECIFIED = 0,
  PDSM_ALT_HOR_LINK_FULLY_INTERDEPENDENT,
  PDSM_ALT_HOR_LINK_ALT_DEP_LAT_LONG,
  PDSM_ALT_HOR_LINK_FULLY_INDEPENDENT,
  PDSM_ALT_HOR_LINK_MAX = 0x10000000    /* Forced to 32 bit */
} pdsm_pd_altitude_horizontal_linkage_e_type;

/* Altitude uncertainty coverage
   Point uncertainty means that the altitude 
   uncertainty is valid only at the injected horizontal
   position coordinates. Full uncertainty mean that the
   altitude uncertainty applies to the device regardless
   of the horizontal position (within the horizontal 
   uncertainty region if provided).
*/
typedef enum
{
  PDSM_ALT_COVERAGE_NOT_SPECIFIED = 0,
  PDSM_ALT_COVERAGE_POINT_UNC,
  PDSM_ALT_COVERAGE_FULL_UNC,
  PDSM_ALT_COVERAGE_MAX = 0x10000000    /* Forced to 32 bit */
} pdsm_pd_altitude_unc_coverage_e_type;

typedef enum {
  PDSM_WIFI_SCAN_SRC_HLOS,           /* Wifi scan src HLOS  */
  PDSM_WIFI_SCAN_SRC_LOWI,           /* Wifi scan src LOWI  */
  PDSM_WIFI_SCAN_SRC_MAX = 0x10000000
} pdsm_wifi_scan_src_e_type;


typedef struct {
  dword   server_adrs; /* IPV4 server address */
  dword   port_id;     /* IPV4 Port Id */
} pdsm_server_ipv4_address_type;  /* IPV4 server address */

typedef struct {
  uint16  server_adrs[8]; /* IPV6 server address */
  dword   port_id;        /* IPV6 port id */
} pdsm_server_ipv6_address_type;  /* IP V6 server address */

typedef struct {
  uint8 url_length; /* URL Length */
  char   url[255];  /* URL data */
} pdsm_server_url_address_type; /* Server URL address */

typedef union {
   pdsm_server_ipv4_address_type server_adrs_v4;  /* IPV4 address */ 
   pdsm_server_ipv6_address_type server_adrs_v6;  /* IPV6 address */ 
   pdsm_server_url_address_type  server_adrs_url;  /* URL address */ 
} pdsm_server_address_u_type;
/*~ IF (_DISC_ == PDSM_SERVER_TYPE_IPV4_ADDRESS)      pdsm_server_address_u_type.server_adrs_v4 */ 
/*~ IF (_DISC_ == PDSM_SERVER_TYPE_URL_ADDRESS)      pdsm_server_address_u_type.server_adrs_url */ 
/*~ IF (_DISC_ == PDSM_SERVER_TYPE_IPV6_ADDRESS)      pdsm_server_address_u_type.server_adrs_v6 */ 
/*~ DEFAULT pdsm_server_address_u_type.void */

typedef struct {
   pdsm_server_address_e_type         server_addr_type; /* Type of server address, IPV4, V6 or URL */
   pdsm_server_address_u_type server_adrs;              /* Server address */
} pdsm_server_address_s_type;
/*~ FIELD pdsm_server_address_s_type.server_adrs DISC pdsm_server_address_s_type.server_addr_type */

typedef struct {
   pdsm_server_option_e_type  server_option; /* Server options , default or local */
   pdsm_server_address_s_type server_adrs;   /* server address */
} pdsm_pd_server_info_s_type;
/* define for continuous mode.
*/
#define PDSM_PD_CONT_FIXES      1000
#define PDSM_PD_MAX_T_BETWEEN   1800 * 1000  /* max time between in ms*/

/**
 * @brief Fix rate parameters
 *
 * Parameters include total number of fixes and TBF value in milliseconds.
 *
 * @sa PDSM_MINIMUM_TBF_MS
 */
typedef struct {
  uint32 num_fixes;             /**< @brief Total number of fixes to be done */
  uint32 time_between_fixes_ms; /**< @brief Time between fixes in milliseconds*/
} pdsm_pd_fix_rate_s_type;


#define PDAPI_SUPL_APPLICATION_ID_VALID 0x1 
   /* pdsm_pd_supl_application_id_s_type contains valid data */
#define PDAPI_SUPL_APPLICATION_ID_APP_VERSION_VALID  0x2
  /* pdsm_pd_supl_application_id_s_type->u_app_version[] contains valid data */
  
#define PDAPI_SUPL_APPID_APP_PROVIDER_ARR_SIZE 25  
#define PDAPI_SUPL_APPID_APP_NAME_ARR_SIZE 33
#define PDAPI_SUPL_APPID_APP_VERSION_ARR_SIZE 9
typedef struct {
  uint8 u_validity; /* Bit0 = 1 ( (u_validity & PDAPI_SUPL_APPLICATION_ID_VALID) == TRUE ) :
                       this structure contains valid data; 
                       Bit1 = 1 ( ( u_validity & PDAPI_SUPL_APPLICATION_ID_APP_VERSION_VALID) == TRUE ): 
                       optional appVersion available;  */  
  uint8 u_app_provider[PDAPI_SUPL_APPID_APP_PROVIDER_ARR_SIZE]; /* make sure null terminate this: max. length 24 */
  uint8 u_app_name[PDAPI_SUPL_APPID_APP_NAME_ARR_SIZE];     /* make sure null terminate this: max. length 32 */
  uint8 u_app_version[PDAPI_SUPL_APPID_APP_VERSION_ARR_SIZE];   /* make sure null terminate this: max. length 8 */
} pdsm_pd_supl_application_id_s_type;


typedef enum {
  PDSM_SESSION_OPERATION_MIN = 0,
  PDSM_SESSION_OPERATION_STANDALONE_ONLY,
  PDSM_SESSION_OPERATION_MSBASED,
  PDSM_SESSION_OPERATION_MSASSISTED,
  PDSM_SESSION_OPERATION_OPTIMAL_SPEED,
  PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY,
  PDSM_SESSION_OPERATION_OPTIMAL_DATA,
  PDSM_SESSION_OPERATION_REF_POSITION,
  PDSM_SESSION_OPERATION_CELL_ID,
  PDSM_SESSION_OPERATION_MAX = 0x10000000
} pdsm_pd_session_operation_e_type;

typedef enum {
  PDSM_DATA_DLOAD_MIN = 0,
  PDSM_DATA_DLOAD_SINGLE,
  PDSM_DATA_DLOAD_MAX = 0x10000000
}pdsm_pd_data_dload_e_type;

typedef struct {
  pdsm_pd_data_dload_e_type     dload_option;
} pdsm_pd_session_data_dload_s_type;

typedef struct {
  pdsm_pd_session_operation_e_type   operation_mode;
  pdsm_pd_session_data_dload_s_type  data_download;
} pdsm_pd_session_info_s_type;


/* Shared Security Data length 127 bytes */
#define PDSM_MAX_SSD_LEN 127

/* Random Number length 16 bytes (128 bits) */
#define PDSM_MAX_RAND_NUM_LEN 16

/* SHA1: hash diget lenth is 20 bytes (160 bits) */
#define PDSM_MAX_AUTH_HASH_LEN 20

/* Typedef structure used by PDSM 
** which contains either random number or hash digest data
** (GPS_SECURITY structure)
*/
typedef struct pdsm_pd_sec_data_s {
  uint8 data_id;
  uint8 data_len;
  uint8 data[PDSM_MAX_AUTH_HASH_LEN];
  /*~ FIELD pdsm_pd_sec_data_s_type.data POINTER
      VARRAY PDSM_MAX_AUTH_HASH_LEN
      LENGTH pdsm_pd_sec_data_s_type.data_len
  */
} pdsm_pd_sec_data_s_type;

/* Authentication Hash algorithm (GPS_HASH_ constant)*/
typedef enum pdsm_pd_hash_alg_e {
    PDSM_PD_HASH_ALG_NONE =-1,        /* FOR INTERNAL USE OF PDSM ONLY! */
    PDSM_PD_HASH_ALG_SHA1,            /* SHA-1 hash diget algorithm for authentication */
    PDSM_PD_HASH_ALG_MAX,             /* FOR INTERNAL USE OF PDSM ONLY! */
    PDSM_PD_HASH_ALG_SIZE=0x10000000  /* To force enum to 32 bit for windows NT */
} pdsm_pd_hash_alg_e_type;

/* Typedef structure used by PDSM
** to provide random number updates 
** (GPS_READ_SECURITY structure)
*/
typedef struct pdsm_pd_rand_num_s {
  pdsm_pd_hash_alg_e_type hash_alg;
  pdsm_pd_sec_data_s_type rand_num;
} pdsm_pd_rand_num_s_type;

/* Typedef structure used by PDSM client
** to pass authentication hash 
** (GPS_WRITE_SECURITY structure) 
*/
typedef struct pdsm_pd_auth_s {
  pdsm_pd_sec_data_s_type auth_hash;
} pdsm_pd_auth_s_type;

/* Encryption Algorithm (GPS_ENCRYPT_ constant)*/
typedef enum pdsm_pd_crypt_alg_e {
    PDSM_PD_CRYPT_ALG_NONE =-1,           /* Encryption is disabled */
    PDSM_PD_CRYPT_ALG_AES128_LAT_LONG,    /* AES-128 encrypts Latitude and Longitude */
    PDSM_PD_CRYPT_ALG_MAX,                /* FOR INTERNAL USE OF PDSM ONLY! */
    PDSM_PD_CRYPT_ALG_SIZE=0x10000000  /* To force enum to 32 bit for windows NT */
} pdsm_pd_crypt_alg_e_type;

/* Typedef structure used by PDSM
** to provide encryption scheme/data to client.
** (GPS_ENCRYPT structure)
*/
typedef struct pdsm_pd_crypt_s {
  pdsm_pd_crypt_alg_e_type encrypt_alg;
  uint8 data_len;
  uint8 data[PDSM_MAX_RAND_NUM_LEN];
  /*~ FIELD pdsm_pd_crypt_s_type.data POINTER 
      VARRAY PDSM_MAX_RAND_NUM_LEN
      LENGTH pdsm_pd_crypt_s_type.data_len
  */
} pdsm_pd_crypt_s_type;

/* Shared Secret Data (SSD) */
typedef struct pdsm_pd_ssd_s {
  uint8 ssd_length;
  uint8 ssd_data[PDSM_MAX_SSD_LEN];
  /*~ FIELD pdsm_pd_ssd_s_type.ssd_data POINTER 
      VARRAY PDSM_MAX_SSD_LEN
      LENGTH pdsm_pd_ssd_s_type.ssd_length
  */
} pdsm_pd_ssd_s_type;

/* Security Update Rate */
typedef struct pdsm_pd_sec_update_rate {
  uint8 rate;
} pdsm_pd_sec_update_rate_s_type;

/* Add KDDI monitor mode specific data */
typedef enum{
  PDSM_SESS_JGPS_TYPE_NONE = -1,      /* For internal use */
  PDSM_SESS_JGPS_TYPE_NO,             /* No session is going on currently */
  PDSM_SESS_JGPS_TYPE_AFLT,           /* AFLT only session type */
  PDSM_SESS_JGPS_TYPE_HYBRID,         /* Hybrid session going on */
  PDSM_SESS_JGPS_TYPE_GPS,            /* Session type GPS only */
  PDSM_SESS_JGPS_TYPE_JGPS_TEST,      /* Session type TEST */
  PDSM_SESS_JGPS_TYPE_MAX= 0x10000000 /* For internal use, enum size */
} pdsm_sess_jgps_type_e_type;

typedef struct{
  byte num;        /* Report PPM pilot count */
  byte sort_type;  /* PPM sort type (0: Ascending, 1: Descending) */
} pdsm_srch_jgps_ppm_info_s_type;

typedef struct{
  byte num;         /* Reported PRM count */
  byte sort_type;   /* PRM sort type (0: Ascending, 1: Descending) */
} pdsm_srch_jgps_prm_info_s_type;

typedef struct{
  pdsm_sess_jgps_type_e_type     mode; /* KDDI positioning mode */
  pdsm_srch_jgps_ppm_info_s_type ppm;  /* PPM setting info for test mode */
  pdsm_srch_jgps_prm_info_s_type prm;  /* PRM setting info for test mode */
} pdsm_pd_meas_mode_info_s_type;

typedef struct {
  pdsm_pd_session_e_type         session;       /* PD option type */
  pdsm_pd_session_operation_e_type  operation_mode; /* operation mode */
  pdsm_pd_fix_rate_s_type        fix_rate;      /* Fix rate for tracking sessions. */
  pdsm_pd_server_info_s_type     lsinfo;        /* Location server info */
  uint32                         class_id;       /* ClassId in case of multiple apps */
  pdsm_pd_auth_s_type            auth_info;   /* Authentication data pointer */
  pdsm_pd_meas_mode_info_s_type     mode_info;         /* KDDI specific mode information */
  pdsm_pd_supl_application_id_s_type supl_application_id; /* application ID */
} pdsm_pd_option_s_type;
#define PDSM_NO_ACCURACY_THRESHOLD 0xFFFFFFF0   /* Threshold users can use so that PDSM ignores the check */
#define PDSM_MIN_ACCURACY_THRESHOLD 0x1         /* At least 1m should be specified */

/* QOS to be used for PD sessions.
*/
typedef struct pdsm_pd_qos_struct {
  dword accuracy_threshold;    /* desired accruracy threshold in meters */
  byte  gps_session_timeout;   /* Desired time for positioning engine to work on a fix. (0-255)secs */
} pdsm_pd_qos_type;

/* ------------------------------------------------------------------------
** End of type declarations for position determination session command.
** -------------------------------------------------------------------------
*/

/* Enumeration of commands PDSM.
*/
typedef enum pdsm_pd_cmd_e
{
    PDSM_PD_CMD_NONE =-1,            /* FOR INTERNAL USE OF PDSM ONLY! */
    PDSM_PD_CMD_GET_POS,             /* Get postion Command */
    PDSM_PD_CMD_END_SESSION,         /* End PD session */
    PDSM_PD_CMD_INJECT_TIME,         /* Inject External time */
    PDSM_PD_CMD_INJECT_POS,          /* Inject external position */ 
    PDSM_PD_CMD_GET_GPS_STATE,       /* Get the GPS State information */
    PDSM_PD_CMD_INJECT_EXTERNAL_SPI, /* Inject external stationary position information */
    PDSM_PD_CMD_PERIODIC_PPM_START,  /* Start Periodic PPM reports */
    PDSM_PD_CMD_PERIODIC_PPM_STOP,   /* Start Periodic PPM reports */
    PDSM_PD_CMD_INJECT_MOTION_DATA,  /* Inject external motion data */
    PDSM_PD_CMD_RESET_LOCATION_SERVICE, /* Location Service Reset Command */
    PDSM_PD_CMD_GET_BEST_AVAIL_POS,     /* Get the best available position command */
    PDSM_PD_CMD_END_SESSION_EX,       /* Extended End PD session */
    PDSM_PD_CMD_INJECT_CELL_INFO,    /* Inject external cell info to SM */
    PDSM_PD_CMD_INJECT_NI_MSG,       /* Inject NI message to SM */
    PDSM_PD_CMD_INJECT_WIFI_ON_DEMAND, /* Inject WiFi on-demand scan result */
    PDSM_PD_CMD_GET_AVAIL_WWAN_POS,     /* Get the available WWAN position command */
    PDSM_PD_CMD_GET_AVAIL_POS_FOR_SRC,     /* Get the available  position estimate command */
    PDSM_PD_CMD_MAX,                 /* FOR INTERNAL USE OF PDSM ONLY! */
    PDSM_CMD_E_SIZE=0x10000000       /* To force enum to 32 bit for windows NT */
} pdsm_pd_cmd_e_type;
/*~ SENTINEL pdsm_pd_cmd_e_type.PDSM_PD_CMD_MAX */

/* Type declarations for Set/Get parameters */

/* Enum defined for allowing different levels of GPS locks.
*/

/* typedef for parameter set mask */
typedef enum pdsm_pa_e {
    PDSM_PA_NONE = 0,           /* Internal range */
    PDSM_PA_APP_INFO,           /* Set App Info */
    PDSM_PA_GPS_LOCK,           /* GPS LOCK */ 
    PDSM_PA_KEEP_WARM,          /* Enable/Disable Keep WARM */
    PDSM_PA_DELETE_PARAMS, /* DEPRECATED. use PDSM_PA_DELETE_PARAMS_EX instead */
    PDSM_PA_POSITION_MODE, /* Position modes that are supported on the mobile */
    PDSM_PA_MT_LR_SUPPORT, /* Specifies if MT-LR is supported */
    PDSM_PA_MO_METHOD, /* specifies if a CP or UP protocol is used for MO */
    PDSM_PA_NMEA_SENTENCE_TYPE, /* specifies the NEMA sentence type to be reported to PDSM */
    PDSM_PA_1X_PDE_NETWORK_ADRS,       /* Address of the network entity, can be IP or URL */
    PDSM_PA_1X_MPC_NETWORK_ADRS,       /* Address of the network entity, can be IP or URL */
    PDSM_PA_UMTS_PDE_NETWORK_ADRS,     /* Address of the network entity, can be IP or URL */
    PDSM_PA_SSD_DATA,           /* SSD update */
    PDSM_PA_SEC_UPDATE_RATE,    /* Security update rate */
    PDSM_PA_PDAPI_ENABLE,       /* Enable disable PDAPI commands */
    PDSM_PA_NMEA_CONFIG_INFO,   /* To configure NMEA port and reporting interval */ 
    PDSM_PA_EFS_DATA,           /* Write data to the EFS, currently used to write SUPL certificate */
    PDSM_PA_SBAS_USER_PREFERENCE,/* Enable/Disable SBAS User Preference */
    PDSM_PA_SBAS_STATUS,         /* Get SBAS Status */
    PDSM_PA_DPO_CONTROL,        /* DPO Control */
    PDSM_PA_ON_DEMAND_LOW_POWER_MODE, /* Enable or disable ON_DEMAND in Low Power Mode */
    PDSM_PA_ON_DEMAND_READY_MODE, /* Enable or disable ON_DEMAND in READY mode */
    PDSM_PA_CALL_FLOW_SELECT,            /* Call Flow Select */
    PDSM_PA_DELETE_PARAMS_EX, /* DEPRECATED. use PDSM_PA_DELETE_PARAMS_EX1 instead */
    PDSM_PA_EXTERNAL_SPI,     /* control raising of PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION*/
    PDSM_PA_XTRAT_CLIENT_TOKEN, /* Set XTRA-T Client token */
    PDSM_PA_XTRAT_USER_SESSION_CONTROL,/* Used to set the XTRA-T User session control */
    PDSM_PA_LBS_APN_PROFILE, /*APN profiles for Location Based Services*/
    PDSM_PA_XTRA_APN_PROFILE, /*APN profiles for XTRA*/
    PDSM_PA_DATA_ENABLE,         /*Enable or Disable data*/
    PDSM_PA_SUPL_VERSION,       /* SUPL Version */
    PDSM_PA_SUPL_SECURITY,      /* SUPL Security: SUPL or UPL */
    PDSM_PA_PE_CONFIG_PARAM,    /* PE config parameter */
    PDSM_PA_DELETE_PARAMS_EX1, /* delete GPS / GNSS params from database */
    PDSM_PA_LPP_CONFIG_INFO,    /* Configuration information for LPP Protocol */
    PDSM_PA_ASST_GLONASS_POS_PROTOCOL,  /* Selection of positioning protocols (RRLP, RRC, LPP) for A-Glonass U&C plane */     
    PDSM_PA_SUPL_IS801,         /* configuration for whether IS801 is carried inside SUPL */
    PDSM_PA_SUPL_TLS_VERSION,    /* configuration for SUPL TLS version */
    PDSM_PA_SUPL_HASH_ALGORITHM, /* configuration for SUPL hash algorithm */
    PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG, /* Whether Emergency positioning uses a User plane or Control Plane protocol.
                                              Currently only can select on WCDMA. 
                                              For GSM and 1x UE will always allow only Control plane NI trigger for positioning. 
                                              For LTE, UE Can allow either SUPL or Control plane NI trigger */  
    PDSM_PA_EMERGENCY_LBS_APN_PROFILE, /* APN profiles for Emergency Location Based Services */
    PDSM_PA_CERTIFICATE_IN_MEM, /*Write supl certificate to Memory*/
    PDSM_PA_WIFI_SCAN_TIMEOUT_SEC, /* Timeout waiting for WIFI scan result, in seconds */
    PDSM_PA_SUPL_UDP_ENABLE_CONFIG, /*Configuration for SUPL UDP*/
    PDSM_PA_PREMIUM_SERVICES_CONFIG,   /* Premium Services Configuration */
    PDSM_PA_XTRA_VERSION_CHECK,        /* XTRA Version Check */
    PDSM_GNSS_CONSTELL_REPORT_CONFIG, /* Contesllation configuration for measurement, poly reporting */
    PDSM_PA_LPP_GRT_24_OTDOA_AD_SUPPORT,  /* Advertise UE support for >24 OTDOA AD processing & generate measurements */     
    PDSM_PA_MAX,                       /* Internal use. */
    PDSM_PA_SIZE = 0x10000000          /* Fix the size as int */
} pdsm_pa_e_type;
/*~ SENTINEL pdsm_pa_e_type.PDSM_PA_MAX */

typedef enum {
    PDSM_GPS_LOCK_MIN = -1,
    PDSM_GPS_LOCK_NONE,
    PDSM_GPS_LOCK_MI,
    PDSM_GPS_LOCK_MT,
    PDSM_GPS_LOCK_ALL,
    PDSM_GPS_LOCK_MAX = 0x10000000
} pdsm_gps_lock_e_type;

typedef enum {
    PDSM_SYSTEM_GPS = 0x1,
    PDSM_SYSTEM_GLO = 0x2,
    PDSM_SYSTEM_BDS = 0x4,
    PDSM_SYSTEM_GAL = 0x8,
    PDSM_SYSTEM_QZSS = 0x10,
    PDSM_SYSTEM_MAX  = 0x20
} PDSM_GNSS_CONSTELL_REPORT_CONFIG_e_type;

typedef struct {

  /* Optional */
  /*  GNSS measurement report constellation control */
  uint8 meas_report_config_valid;  /**< Must be set to true if measReportConfig is being passed */
  uint16 e_meas_report_config;
  /**<   GNSS measurement report constellation control \n
 Valid values: \n
      - eQMI_SYSTEM_GPS (0x01) --  Enable GPS
      - eQMI_SYSTEM_GLO (0x02) --  Enable GLONASS
      - eQMI_SYSTEM_BDS (0x04) --  Enable BDS
      - eQMI_SYSTEM_GAL (0x08) --  Enable Galileo
      - eQMI_SYSTEM_QZSS(0x10) --  Enable QZSS
 */

  /* Optional */
  /*  SV Poly Report constellation control */
  uint8 sv_poly_report_config_valid;  /**< Must be set to true if svPolyReportConfig is being passed */
  uint16 e_sv_poly_report_config;
  /**<   SV Poly Report constellation control \n
 Valid values: \n
      - eQMI_SYSTEM_GPS (0x01) --  Enable GPS
      - eQMI_SYSTEM_GLO (0x02) --  Enable GLONASS
      - eQMI_SYSTEM_BDS (0x04) --  Enable BDS
      - eQMI_SYSTEM_GAL (0x08) --  Enable Galileo
      - eQMI_SYSTEM_QZSS(0x10) --  Enable QZSS
 */
}pdsm_LocSetGNSSConfigReqStruct;  /* Message */


/* List of default values to be used in case NV read failure.
*/
#define PDE_ADDRESS_DEFAULT  0
#define PDE_PORT_ID_DEFAULT  4911




typedef struct {
    word base_id;
    /* Base station identification */
    word sid;
    /* System Identification */
    word nid;
    /* Network Identification */
    dword base_lat;
    /* Base latitude */
    dword base_long;
    /* Base longitude */
    qword ts;
    /* Time when BS info was stored */
    boolean stale_info;
    /* BS info is stale or not */
} pdsm_bs_info_s_type;

/* sets the specified bit in a u32 bit mask. bit counts range: 0-31 */
#define PDSM_SET_U32BITMASK_BIT(n) (((uint32)0x1) << (n))

/* sets the specified bit in a u64 bit mask. bit counts range: 0-63 */
#define PDSM_SET_U64BITMASK_BIT(n) (((uint64)0x1) << (n))

#define PDSM_PA_DELETE_BITMASK(n)  PDSM_SET_U64BITMASK_BIT(n)

/* the following flags, where no GNSS system is specified, 
   affect only GPS, unless mentioned otherwise. E.g.
   PDSM_PA_DELETE_EPH would only delete GPS ephemeris. 

   See also: C_DELETE_INFO_xxx flags used by MGP API
*/
#define PDSM_PA_DELETE_EPH    PDSM_PA_DELETE_BITMASK(0) /*0x0001*/
#define PDSM_PA_DELETE_ALM    PDSM_PA_DELETE_BITMASK(1) /*0x0002*/
#define PDSM_PA_DELETE_POS    PDSM_PA_DELETE_BITMASK(2) /*0x0004*/
#define PDSM_PA_DELETE_TIME   PDSM_PA_DELETE_BITMASK(3) /*0x0008*/ 
/* PDSM_PA_DELETE_TIME  will delete all GNSS time For finer control 
   of individual clock elements, use PDSM_PA_DELETE_CLOCK_INFO instead  */

#define PDSM_PA_DELETE_IONO   PDSM_PA_DELETE_BITMASK(4) /*0x0010*/
#define PDSM_PA_DELETE_UTC    PDSM_PA_DELETE_BITMASK(5) /*0x0020*/
#define PDSM_PA_DELETE_HEALTH PDSM_PA_DELETE_BITMASK(6) /*0x0040*/
#define PDSM_PA_DELETE_SVDIR  PDSM_PA_DELETE_BITMASK(7) /*0x0080*/
#define PDSM_PA_DELETE_SVSTEER PDSM_PA_DELETE_BITMASK(8) /*0x0100*/
#define PDSM_PA_DELETE_SADATA PDSM_PA_DELETE_BITMASK(9) /*0x0200*/
#define PDSM_PA_DELETE_RTI    PDSM_PA_DELETE_BITMASK(10) /*0x0400*/

#define PDSM_PA_TIME_UNC      PDSM_PA_DELETE_BITMASK(11) /*0x0800*/ /* this will modify all GNSS time unc */
#define PDSM_PA_POS_UNC       PDSM_PA_DELETE_BITMASK(12) /*0x1000*/
#define PDSM_PA_TIME_OFFSET   PDSM_PA_DELETE_BITMASK(13) /*0x2000*/
#define PDSM_PA_POS_OFFSET    PDSM_PA_DELETE_BITMASK(14) /*0x4000*/

/* PDSM_PA_DELETE_CELLDB_INFO: see also PDSM_PA_DELETE_CELLDB_MASK_xxx below */
#define PDSM_PA_DELETE_CELLDB_INFO PDSM_PA_DELETE_BITMASK(15) /*0x8000*/
#define PDSM_PA_DELETE_ALM_CORR    PDSM_PA_DELETE_BITMASK(16) /*0x10000*/
#define PDSM_PA_DELETE_FREQ_BIAS_EST PDSM_PA_DELETE_BITMASK(17) /*0x20000*/
#define PDSM_PA_DELETE_SV_NO_EXIST  PDSM_PA_DELETE_BITMASK(18) /*0x40000*/
/* additional constellation flags */
#define PDSM_PA_DELETE_EPH_GLO   PDSM_PA_DELETE_BITMASK(19)   
#define PDSM_PA_DELETE_EPH_SBAS  PDSM_PA_DELETE_BITMASK(20)

#define PDSM_PA_DELETE_ALM_GLO   PDSM_PA_DELETE_BITMASK(21)
#define PDSM_PA_DELETE_ALM_SBAS  PDSM_PA_DELETE_BITMASK(22)

#define PDSM_PA_DELETE_SVDIR_GLO    PDSM_PA_DELETE_BITMASK(23)
#define PDSM_PA_DELETE_SVDIR_SBAS   PDSM_PA_DELETE_BITMASK(24)

#define PDSM_PA_DELETE_SVSTEER_GLO     PDSM_PA_DELETE_BITMASK(25)
#define PDSM_PA_DELETE_SVSTEER_SBAS    PDSM_PA_DELETE_BITMASK(26)

#define PDSM_PA_DELETE_ALM_CORR_GLO    PDSM_PA_DELETE_BITMASK(27)

#define PDSM_PA_DELETE_TIME_GPS   PDSM_PA_DELETE_BITMASK(28)
#define PDSM_PA_DELETE_TIME_GLO   PDSM_PA_DELETE_BITMASK(29)

#define PDSM_PA_DELETE_CLOCK_INFO PDSM_PA_DELETE_BITMASK(30) /* indicates delete_clock_info_mask
                                                                 is valid. see PDSM_PA_DELETE_CLOCK_INFOxxx*/

#define PDSM_PA_DELETE_TIME_BDS      PDSM_PA_DELETE_BITMASK(31)
#define PDSM_PA_DELETE_EPH_BDS       PDSM_PA_DELETE_BITMASK(32)   
#define PDSM_PA_DELETE_ALM_BDS       PDSM_PA_DELETE_BITMASK(33)
#define PDSM_PA_DELETE_SVDIR_BDS     PDSM_PA_DELETE_BITMASK(34)
#define PDSM_PA_DELETE_SVSTEER_BDS   PDSM_PA_DELETE_BITMASK(35)
#define PDSM_PA_DELETE_ALM_CORR_BDS  PDSM_PA_DELETE_BITMASK(36)

#define PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GPS  PDSM_PA_DELETE_BITMASK(37)
#define PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GLO  PDSM_PA_DELETE_BITMASK(38)
#define PDSM_PA_DELETE_GNSS_SV_BLACKLIST_BDS  PDSM_PA_DELETE_BITMASK(39)
#define PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GAL  PDSM_PA_DELETE_BITMASK(40)
#define PDSM_PA_DELETE_TIME_GAL               PDSM_PA_DELETE_BITMASK(41)
#define PDSM_PA_DELETE_EPH_GAL                PDSM_PA_DELETE_BITMASK(42)   
#define PDSM_PA_DELETE_ALM_GAL                PDSM_PA_DELETE_BITMASK(43)
#define PDSM_PA_DELETE_SVDIR_GAL              PDSM_PA_DELETE_BITMASK(44)
#define PDSM_PA_DELETE_SVSTEER_GAL            PDSM_PA_DELETE_BITMASK(45)
#define PDSM_PA_DELETE_ALM_CORR_GAL           PDSM_PA_DELETE_BITMASK(46)
#define PDSM_PA_DELETE_GNSS_SV_BLACKLIST_QZSS PDSM_PA_DELETE_BITMASK(47)

#define PDSM_PA_DELETE_IONO_QZSS              PDSM_PA_DELETE_BITMASK(48)
#define PDSM_PA_DELETE_EPH_QZSS               PDSM_PA_DELETE_BITMASK(49)   
#define PDSM_PA_DELETE_ALM_QZSS               PDSM_PA_DELETE_BITMASK(50)
#define PDSM_PA_DELETE_SVDIR_QZSS             PDSM_PA_DELETE_BITMASK(51)
#define PDSM_PA_DELETE_SVSTEER_QZSS           PDSM_PA_DELETE_BITMASK(52)
#define PDSM_PA_DELETE_ALM_CORR_QZSS          PDSM_PA_DELETE_BITMASK(53)
#define PDSM_PA_DELETE_HEALTH_QZSS            PDSM_PA_DELETE_BITMASK(54)

/* Deletes Glonass SV health info for all GLONASS satellites*/
#define PDSM_PA_DELETE_HEALTH_GLO    PDSM_PA_DELETE_BITMASK(40) 
/* Deletes BDS SV health info for all BDS satellites*/ 
#define PDSM_PA_DELETE_HEALTH_BDS    PDSM_PA_DELETE_BITMASK(41)

#define PDSM_PA_DELETE_ALL (\
   PDSM_PA_DELETE_EPH | \
   PDSM_PA_DELETE_ALM | \
   PDSM_PA_DELETE_POS | \
   PDSM_PA_DELETE_TIME | \
   PDSM_PA_DELETE_IONO | \
   PDSM_PA_DELETE_UTC | \
   PDSM_PA_DELETE_HEALTH | \
   PDSM_PA_DELETE_SVDIR | \
   PDSM_PA_DELETE_SVSTEER | \
   PDSM_PA_DELETE_SADATA | \
   PDSM_PA_DELETE_RTI | \
   PDSM_PA_DELETE_CELLDB_INFO | \
   PDSM_PA_DELETE_ALM_CORR |\
   PDSM_PA_DELETE_SV_NO_EXIST |\
   PDSM_PA_DELETE_FREQ_BIAS_EST|\
   PDSM_PA_DELETE_EPH_GLO |\
   PDSM_PA_DELETE_EPH_SBAS |\
   PDSM_PA_DELETE_ALM_GLO |\
   PDSM_PA_DELETE_ALM_SBAS |\
   PDSM_PA_DELETE_SVDIR_GLO |\
   PDSM_PA_DELETE_SVDIR_SBAS |\
   PDSM_PA_DELETE_SVSTEER_GLO |\
   PDSM_PA_DELETE_SVSTEER_SBAS |\
   PDSM_PA_DELETE_ALM_CORR_GLO |\
   PDSM_PA_DELETE_TIME_GPS |\
   PDSM_PA_DELETE_TIME_GLO |\
   PDSM_PA_DELETE_TIME_BDS |\
   PDSM_PA_DELETE_EPH_BDS |\
   PDSM_PA_DELETE_ALM_BDS |\
   PDSM_PA_DELETE_SVDIR_BDS |\
   PDSM_PA_DELETE_SVSTEER_BDS |\
   PDSM_PA_DELETE_ALM_CORR_BDS |\
   PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GPS |\
   PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GLO |\
   PDSM_PA_DELETE_GNSS_SV_BLACKLIST_BDS |\
   PDSM_PA_DELETE_GNSS_SV_BLACKLIST_QZSS |\
   PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GAL |\
   PDSM_PA_DELETE_IONO_QZSS |\
   PDSM_PA_DELETE_EPH_QZSS |\
   PDSM_PA_DELETE_ALM_QZSS |\
   PDSM_PA_DELETE_SVDIR_QZSS |\
   PDSM_PA_DELETE_SVSTEER_QZSS |\
   PDSM_PA_DELETE_ALM_CORR_QZSS|\
   PDSM_PA_DELETE_HEALTH_QZSS \
   PDSM_PA_DELETE_TIME_GAL |\
   PDSM_PA_DELETE_EPH_GAL |\
   PDSM_PA_DELETE_ALM_GAL |\
   PDSM_PA_DELETE_SVDIR_GAL |\
   PDSM_PA_DELETE_SVSTEER_GAL |\
   PDSM_PA_DELETE_ALM_CORR_GAL |\
   PDSM_PA_DELETE_HEALTH_GLO |\
   PDSM_PA_DELETE_HEALTH_QZSS |\
   PDSM_PA_DELETE_HEALTH_BDS\
   )

/* Cell DB info delete bit mask */
#define PDSM_PA_DELETE_CELLDB_MASK_POS_INFO            PDSM_SET_U32BITMASK_BIT(0)
#define PDSM_PA_DELETE_CELLDB_MASK_LATEST_GPS_POS_INFO PDSM_SET_U32BITMASK_BIT(1)
#define PDSM_PA_DELETE_CELLDB_MASK_OTA_POS_INFO        PDSM_SET_U32BITMASK_BIT(2)
#define PDSM_PA_DELETE_CELLDB_MASK_EXT_REF_POS_INFO    PDSM_SET_U32BITMASK_BIT(3)
#define PDSM_PA_DELETE_CELLDB_MASK_TIMETAG_INFO        PDSM_SET_U32BITMASK_BIT(4)
#define PDSM_PA_DELETE_CELLDB_MASK_CELLID_INFO         PDSM_SET_U32BITMASK_BIT(5)
#define PDSM_PA_DELETE_CELLDB_MASK_CACHED_CELLID_INFO  PDSM_SET_U32BITMASK_BIT(6)
#define PDSM_PA_DELETE_CELLDB_MASK_LAST_SRV_CELL_INFO  PDSM_SET_U32BITMASK_BIT(7)
#define PDSM_PA_DELETE_CELLDB_MASK_CUR_SRV_CELL_INFO   PDSM_SET_U32BITMASK_BIT(8)
#define PDSM_PA_DELETE_CELLDB_MASK_NEIGHBOR_INFO_INFO  PDSM_SET_U32BITMASK_BIT(9)

#define PDSM_PA_DELETE_CLOCK_INFO_TIME_EST                 PDSM_SET_U32BITMASK_BIT(0) /* deletes all GNSS time */
#define PDSM_PA_DELETE_CLOCK_INFO_FREQ_EST                 PDSM_SET_U32BITMASK_BIT(1)
#define PDSM_PA_DELETE_CLOCK_INFO_WEEK_NUMBER              PDSM_SET_U32BITMASK_BIT(2)
#define PDSM_PA_DELETE_CLOCK_INFO_RTC_TIME                 PDSM_SET_U32BITMASK_BIT(3)
#define PDSM_PA_DELETE_CLOCK_INFO_TIME_TRANSFER            PDSM_SET_U32BITMASK_BIT(4)
#define PDSM_PA_DELETE_CLOCK_INFO_GPSTIME_EST              PDSM_SET_U32BITMASK_BIT(5)
#define PDSM_PA_DELETE_CLOCK_INFO_GLOTIME_EST              PDSM_SET_U32BITMASK_BIT(6)
#define PDSM_PA_DELETE_CLOCK_INFO_GLODAY_NUMBER            PDSM_SET_U32BITMASK_BIT(7)
#define PDSM_PA_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER          PDSM_SET_U32BITMASK_BIT(8)
#define PDSM_PA_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY         PDSM_SET_U32BITMASK_BIT(9)
#define PDSM_PA_DELETE_CLOCK_INFO_DISABLE_TT               PDSM_SET_U32BITMASK_BIT(10)
#define PDSM_PA_DELETE_CLOCK_INFO_GG_LEAPSEC               PDSM_SET_U32BITMASK_BIT(11)
#define PDSM_PA_DELETE_CLOCK_INFO_GG_GGTB                  PDSM_SET_U32BITMASK_BIT(12)
#define PDSM_PA_DELETE_CLOCK_INFO_BDSTIME_EST              PDSM_SET_U32BITMASK_BIT(13)
#define PDSM_PA_DELETE_CLOCK_INFO_GB_GBTB                  PDSM_SET_U32BITMASK_BIT(14)
#define PDSM_PA_DELETE_CLOCK_INFO_BG_BGTB                  PDSM_SET_U32BITMASK_BIT(15) /* Beidou to Glonass time bias-related */
#define PDSM_PA_DELETE_CLOCK_INFO_BDSWEEK_NUMBER           PDSM_SET_U32BITMASK_BIT(16)
#define PDSM_PA_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY         PDSM_SET_U32BITMASK_BIT(17)
#define PDSM_PA_DELETE_CLOCK_INFO_GPS_GAL_TB               PDSM_SET_U32BITMASK_BIT(18)
#define PDSM_PA_DELETE_CLOCK_INFO_GLO_GAL_TB               PDSM_SET_U32BITMASK_BIT(19)
#define PDSM_PA_DELETE_CLOCK_INFO_GAL_BDS_TB               PDSM_SET_U32BITMASK_BIT(20)

#define PDSM_PA_DELETE_ALL_CLOCK_INFO (\
     PDSM_PA_DELETE_CLOCK_INFO_TIME_EST |\
     PDSM_PA_DELETE_CLOCK_INFO_FREQ_EST |\
     PDSM_PA_DELETE_CLOCK_INFO_WEEK_NUMBER |\
     PDSM_PA_DELETE_CLOCK_INFO_RTC_TIME |\
     PDSM_PA_DELETE_CLOCK_INFO_TIME_TRANSFER |\
     PDSM_PA_DELETE_CLOCK_INFO_GPSTIME_EST |\
     PDSM_PA_DELETE_CLOCK_INFO_GLOTIME_EST |\
     PDSM_PA_DELETE_CLOCK_INFO_GLODAY_NUMBER |\
     PDSM_PA_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER |\
     PDSM_PA_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY |\
     PDSM_PA_DELETE_CLOCK_INFO_DISABLE_TT |\
     PDSM_PA_DELETE_CLOCK_INFO_GG_LEAPSEC |\
     PDSM_PA_DELETE_CLOCK_INFO_GG_GGTB |\
     PDSM_PA_DELETE_CLOCK_INFO_BDSTIME_EST |\
     PDSM_PA_DELETE_CLOCK_INFO_GB_GBTB |\
     PDSM_PA_DELETE_CLOCK_INFO_BG_BGTB |\
     PDSM_PA_DELETE_CLOCK_INFO_BDSWEEK_NUMBER |\
     PDSM_PA_DELETE_CLOCK_INFO_BDS_RF_GRP_DELAY |\
     PDSM_PA_DELETE_CLOCK_INFO_GPS_GAL_TB | \
     PDSM_PA_DELETE_CLOCK_INFO_GLO_GAL_TB | \
     PDSM_PA_DELETE_CLOCK_INFO_GAL_BDS_TB  \
     )

typedef struct {
 uint64 pdsm_delete_parms_flags;  /* 64 bits now with BDS support */
 uint32 time_unc; /* time unc in msec */
 uint32 pos_unc;  /* pos unc in meters */
 uint32 time_offset; /* time offset in msec*/
 uint32 pos_offset; /* pos offset in meters. currently not supported */
 uint32 eph_sv_mask; /* delete targeted GPS EPH*/
 uint32 alm_sv_mask; /* delete targeted GPS ALM*/
 uint32 glo_eph_sv_mask; /* delete targeted GLO EPH */
 uint32 glo_alm_sv_mask; /* delete targeted GLO ALM */
 uint32 sbas_eph_sv_mask; /* delete targeted SBAS EPH */
 uint32 sbas_alm_sv_mask; /* delete targeted SBAS ALM */
 uint64 bds_eph_sv_mask; /* delete targeted BDS EPH */
 uint64 bds_alm_sv_mask; /* delete targeted BDS ALM */
 uint64 gal_eph_sv_mask; /* delete targeted GAL EPH */
 uint64 gal_alm_sv_mask; /* delete targeted GAL ALM */
 uint32 qzss_eph_sv_mask; /* delete targeted QZSS EPH */
 uint32 qzss_alm_sv_mask; /* delete targeted QZSS ALM*/
 uint32 gps_gnss_sv_blacklist_mask;  /* delete targeted GPS SV in GNSS SV blacklist */
 uint32 glo_gnss_sv_blacklist_mask;  /* delete targeted GLO SV in GNSS SV blacklist */
 uint64 bds_gnss_sv_blacklist_mask;  /* delete targeted BDS SV in GNSS SV blacklist */
 uint64 gal_gnss_sv_blacklist_mask;  /* delete targeted GAL SV in GNSS SV blacklist */
 uint32 qzss_gnss_sv_blacklist_mask; /* delete targeted QZSS SV in GNSS SV blacklist */
 uint32 delete_celldb_mask; /* delete cell db */
 uint32 delete_clock_info_mask; /* must set PDSM_PA_DELETE_CLOCK_INFO.
                                see PDSM_PA_DELETE_CLOCK_INFOxxx for 
                                 permitted flags */
 float f_time_unc; /* time unc in msec, can accept sub-ms values in the decimal point*/
 float f_time_offset; /* time offset in msec can accept submeter value in the decimal point */
 float f_pos_unc;  /* pos unc in meters. can accept submeter values in the decimal point */
} pdsm_delete_parms_type; 

/*
* UE supported position method types (a.k.a positioning modes)
*/
typedef uint32    pdsm_position_mode_type;

/* GPS position method types */
#define PDSM_POS_MODE_STANDALONE        0x0001  /* GNSS Standalone */
#define PDSM_POS_MODE_UP_MS_BASED       0x0002  /* if both UP bits 0, no UP */
#define PDSM_POS_MODE_UP_MS_ASSISTED    0x0004  /* if both UP bits 0, no UP */
#define PDSM_POS_MODE_2G_CP_MS_BASED    0x0008  /* Enable/Disable A-GPS CP MS-BASED for 2G */
#define PDSM_POS_MODE_2G_CP_MS_ASSISTED 0x0010  /* Enable/Disable A-GPS CP MS-ASSISTED for 2G */
#define PDSM_POS_MODE_3G_CP_UE_BASED    0x0020  /* Enable/Disable A-GPS CP MS-BASED for 3G */
#define PDSM_POS_MODE_3G_CP_UE_ASSISTED 0x0040  /* Enable/Disable A-GPS CP MS-ASSISTED for 3G */
#define PDSM_POS_MODE_NMR_SUPPORTED     0x0080  /* GSM UP NMR support feature enable/disable flag */
#define PDSM_POS_MODE_4G_UP_MS_BASED    0x0100  /* Enable/Disable A-GPS UP MS-BASED for 4G */
#define PDSM_POS_MODE_4G_UP_MS_ASSISTED 0x0200  /* Enable/Disable A-GPS UP MS-ASSISTED for 4G */
#define PDSM_POS_MODE_4G_CP_MS_BASED    0x0400  /* Enable/Disable A-GPS CP MS-BASED for 4G */
#define PDSM_POS_MODE_4G_CP_MS_ASSISTED 0x0800  /* Enable/Disable A-GPS CP MS-ASSISTED for 4G */
/* Bit 12 to 15 is unusable because default settings have been 0xFF7F, any additional values not
   to be set as default should start at 0x00010000 */
#define PDSM_POS_MODE_MSB_TO_STANDALONE_ALLOWED_IF_NO_SIM  0x00010000 /* Enable/Disable A-GPS UP MS-Based fallback to standalone when no SIM in the device */
/* Glonass position method types */
#define PDSM_AGLO_POS_MODE_UP_MS_BASED          0x00020000        /* Enable/Disable A-GLONASS UP MS-BASED for RRLP */
#define PDSM_AGLO_POS_MODE_UP_MS_ASSISTED       0x00040000        /* Enable/Disable A-GLONASS UP MS-ASSISTED for RRLP */
#define PDSM_AGLO_POS_MODE_3G_CP_MS_BASED       0x00080000        /* Enable/Disable A-GLONASS CP MS-BASED for 3G */
#define PDSM_AGLO_POS_MODE_3G_CP_MS_ASSISTED    0x00100000        /* Enable/Disable A-GLONASS CP MS-ASSISTED for 3G */
#define PDSM_AGLO_POS_MODE_4G_UP_MS_BASED       0x00200000        /* Enable/Disable A-GLONASS UP MS-BASED for 4G */
#define PDSM_AGLO_POS_MODE_4G_UP_MS_ASSISTED    0x00400000        /* Enable/Disable A-GLONASS UP MS-ASSISTED for 4G */
#define PDSM_AGLO_POS_MODE_4G_CP_MS_BASED       0x00800000        /* Enable/Disable A-GLONASS CP MS-BASED for 4G */
#define PDSM_AGLO_POS_MODE_4G_CP_MS_ASSISTED    0x01000000        /* Enable/Disable A-GLONASS CP MS-ASSISTED for 4G */

/* Used to enable MTLR support for the protocols. used by pdsm_pa_mt_lr_support_e_type*/
#define PDSM_MT_LR_CP_GSM_WCDMA_SUPPORTED 0x1 /* inform network of value_added supported if set */
#define PDSM_MT_LR_UP_GSM_WCDMA_BLOCKED   0x2 /* ignore G/W UP NI trigger if set */
#define PDSM_MT_LR_UP_LPP_BLOCKED   0x4 /* ignore LPP UP NI trigger if set */
#define PDSM_MT_LR_CP_LPP_BLOCKED   0x8 /* ignore LPP CP NI trigger if set */
#define PDSM_MT_LR_MAX_VALUE        0x1F /* make sure to update this field*/

/* see the above defines for valid values */
typedef uint32 pdsm_pa_mt_lr_support_e_type;


typedef enum
{
  MO_CP,  /* use control plane */
  MO_UP   /* use user plane */
} pdsm_pa_mo_method_e_type;

typedef uint32 pdsm_pa_nmea_type;
#define PDAPI_NMEA_GGA 0x0001
#define PDAPI_NMEA_RMC 0x0002
#define PDAPI_NMEA_GSV 0x0004
#define PDAPI_NMEA_GSA 0x0008
#define PDAPI_NMEA_VTG 0x0010
#define PDAPI_NMEA_PQXFI  0x0020

#define PDAPI_NMEA_GPGGA PDAPI_NMEA_GGA  /* Fix data */
#define PDAPI_NMEA_GPRMC PDAPI_NMEA_RMC  /* recommended minimum data */
#define PDAPI_NMEA_GPGSV PDAPI_NMEA_GSV  /* GPS SVs in view */
#define PDAPI_NMEA_GPGSA PDAPI_NMEA_GSA  /* GPS SV dop and active SV info */
#define PDAPI_NMEA_GPVTG PDAPI_NMEA_VTG  /* Speed and heading info */

#define PDAPI_NMEA_GLGSV 0x0040  /* Glonass SV in view info */
#define PDAPI_NMEA_GNGSA 0x0080  /* Dop and Active SV info iff Glonass SVs are used */
#define PDAPI_NMEA_GNGNS 0x0100  /* new GGA message for GNSS */
#define PDAPI_NMEA_GARMC 0x0200   /* GAL recommended minimum data */
#define PDAPI_NMEA_GAGSV 0x0400   /* GAL SVs in view */
#define PDAPI_NMEA_GAGSA 0x0800   /* GAL SV dop and active SV info */
#define PDAPI_NMEA_GAVTG 0x1000   /* GAL Speed and heading info */
#define PDAPI_NMEA_PSTIS 0x2000  /* proprietary sentence at beginning of each sess */
#define PDAPI_NMEA_GSV_EXTENDED 0x4000 /*Enable/Disable Extended GGSV*/
#define PDAPI_NMEA_GAGGA 0x8000   /* GAL Fix data */
#define PDAPI_NMEA_PQGSA 0x00010000   /* QZSS Enable PQGSA */
#define PDAPI_NMEA_PQGSV 0x00020000   /* QZSS Enable PQGSV */

/* update the PDAPI_NMEA_MAX_VALUE when new fields are added */
#define PDAPI_NMEA_MAX_VALUE 0x3FFFF 

#define PDSM_PA_APP_INFO_LENGTH 255


typedef enum
{
  PDSM_PA_NMEA_PORT_TYPE_MIN = -1, /* Invalid */
  PDSM_PA_NMEA_PORT_TYPE_UART1,    /* COM PORT */
  PDSM_PA_NMEA_PORT_TYPE_UART2,    /* COM PORT */
  PDSM_PA_NMEA_PORT_TYPE_USB,    /* USB */
  PDSM_PA_NMEA_PORT_TYPE_SHARED_MEM, /* Shared Memory port */
  PDSM_PA_NMEA_PORT_TYPE_NONE,       /* Disable NMEA port */
  PDSM_PA_NMEA_PORT_TYPE_MAX
} pdsm_pa_nmea_port_e_type;

typedef enum
{
  PDSM_PA_SBAS_STATUS_NOT_AVAILABLE,
  PDSM_PA_SBAS_STATUS_ENABLED,
  PDSM_PA_SBAS_STATUS_DISABLED
} pdsm_pa_sbas_status_e_type;

typedef enum
{
  PDSM_PA_NMEA_REPORT_NONE = -1, 
  PDSM_PA_NMEA_REPORT_AT_1HZ_INTERVAL, /* Generate NMEA at 1Hz rate */
  PDSM_PA_NMEA_REPORT_AT_FINAL_POSITION, /* Generate NMEA only when the final position is reported */
  PDSM_PA_NMEA_REPORT_MAX,
} pdsm_pa_nmea_reporting_e_type;

typedef struct 
{
  pdsm_pa_nmea_port_e_type pd_nmea_port_type;
  pdsm_pa_nmea_reporting_e_type pd_nmea_reporting_type;
} pdsm_pa_nmea_config_s_type;

#define PDSM_PA_PE_CONFIG_PARAM_MASK_USE_INJECTED_POSITION  0x00000001
#define PDSM_PA_PE_CONFIG_PARAM_MASK_FILTER_SV_USED         0x00000002
#define PDSM_PA_PE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA      0x00000004
#define PDSM_PA_PE_CONFIG_PARAM_MASK_REPORT_ACCURATE_SVDIR    0x00000008

typedef struct 
{
  /* bitmask from above definition, can config one or multiple parameters
     This field is used by PDAPI client to pass in bitmasks for changing some/all
     PE config parameters. 
     For event PDSM_PA_EVENT_PE_CONFIG_PARAM sent back to PDAPI client, this field
     is used to indicate success(if bit is 1) or failure(if bit is 0) for requested
     PE config parameter SET/GET. For event triggered by SET, the 3 PE config parameters
     below will contain original values passed in by PDAPI client regardless of success
     or failure. For event triggered by GET, the returned PE config parameter value is 
     meaningful only if the corresponding bit in bitmask is set to 1 (i.e. success) */
  uint32  peConfigMask;           

  /* 0x00 (FALSE) -- Do not use the injected position in direct position calculation. 
     0x01 (TRUE) --  Use the injected position in direct position calculation. */
  boolean useInjectedPosition;

  /* 0x00 (FALSE) -- Do not filter the usage of SV's in the fix         
     0x01 (TRUE) -- Filter the usage of SV's in the fix. */
  boolean filterSvUsed;

  /* 0x00 (FALSE) --  Do not store assitance data in persistent memory.  
     0x01 (TRUE) --  Store assitance data in persistent memory */
  boolean storeAssistData;

  /* 0x00 (FALSE) --  Do not report accurate SV direction.  
     0x01 (TRUE) --  Report Accurate SV direction */
  boolean rptAccurateSvDir;

} pdsm_pa_pe_config_s_type;

typedef enum
{
  PDSM_PD_MOTION_STATE_MIN = -1,                  /* validity checking */
  PDSM_PD_MOTION_STATE_UNKNOWN,
  PDSM_PD_MOTION_STATE_STATIONARY,
  PDSM_PD_MOTION_STATE_IN_MOTION,
  PDSM_PD_MOTION_STATE_MAX,                       /* validity checking */
  PDSM_PD_MOTION_STATE_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
} pdsm_pd_motion_state_e_type;

typedef enum
{
  PDSM_PD_MOTION_MODE_MIN = -1,                  /* validity checking */
  PDSM_PD_MOTION_MODE_UNKNOWN,
  PDSM_PD_MOTION_MODE_PEDESTRIAN,
  PDSM_PD_MOTION_MODE_VEHICLE,
  PDSM_PD_MOTION_MODE_MAX,                       /* validity checking */
  PDSM_PD_MOTION_MODE_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
} pdsm_pd_motion_mode_e_type;

typedef enum
{
  PDSM_PD_MOTION_UNK_SUB_MODE_MIN = -1,                  /* validity checking */
  PDSM_PD_MOTION_UNK_SUB_MODE_UNKNOWN,
  PDSM_PD_MOTION_UNK_SUB_MODE_MAX,                       /* validity checking */
  PDSM_PD_MOTION_UNK_SUB_MODE_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
} pdsm_pd_motion_unknown_sub_mode_e_type;

typedef enum
{
  PDSM_PD_MOTION_PED_SUB_MODE_MIN = -1,                  /* validity checking */
  PDSM_PD_MOTION_PED_SUB_MODE_UNKNOWN,
  PDSM_PD_MOTION_PED_SUB_MODE_WALKING,
  PDSM_PD_MOTION_PED_SUB_MODE_RUNNING,
  PDSM_PD_MOTION_PED_SUB_MODE_MAX,                       /* validity checking */
  PDSM_PD_MOTION_PED_SUB_MODE_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
} pdsm_pd_motion_pedestrian_sub_mode_e_type;

typedef enum
{
  PDSM_PD_MOTION_VEH_SUB_MODE_MIN = -1,                  /* validity checking */
  PDSM_PD_MOTION_VEH_SUB_MODE_UNKNOWN,
  PDSM_PD_MOTION_VEH_SUB_MODE_MAX,                       /* validity checking */
  PDSM_PD_MOTION_VEH_SUB_MODE_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
} pdsm_pd_motion_vehicle_sub_mode_e_type;

typedef union
{
  pdsm_pd_motion_unknown_sub_mode_e_type     unknownSubMode;
  pdsm_pd_motion_pedestrian_sub_mode_e_type  pedestrianSubMode;
  pdsm_pd_motion_vehicle_sub_mode_e_type     vehicleSubMode;
} pdsm_pd_motion_sub_mode_u_type;

typedef struct
{
  /* Current user motion state.
   */
  pdsm_pd_motion_state_e_type motionState;

  /* Specific transportation type for current user motion.
   */
  pdsm_pd_motion_mode_e_type motionMode;

  /* Details of transportation type for current user motion.
   * Interpretation of union sub mode field is decided by the motion mode above.
   */
  pdsm_pd_motion_sub_mode_u_type motionSubMode;

  /* Probability that device is undergoing the motion defined by the motion
   * state, motion user mode and motion user sub mode as a percentage. Acceptable
   * values are 0-100. Value is ignored if a mode == UNKNOWN.
   */
  float probabilityMotionCombo;

  /* Age of the motion data in milliseconds at the time of injection. Motion
   * data is allowed to be used by the system until the age reaches the timeout
   * value. If an indefinite timeout is specified then this parameter is ignored.
   */
  uint16 ageMs;

  /* Amount of time in milliseconds that motion data will be actively used by
   * the positioning system. If a value of UINT16_MAX is specified, then the
   * motion data will be used indefinitely. DO NOT specify to use motion data
   * indefinitely unless you plan to send all updates through this API.
   */
  uint16 timeoutMs;

} pdsm_pd_motion_data_s_type;

typedef struct {
  char     *url;           /* Url information */
  /*~ FIELD pdsm_pm_jgpsone_app_s_type.url POINTER */    
  byte     ver;            /* CP-LIS Protocol Version */
  byte     datum;          /* Pos system, 1=WGS, 1=Tokyo datum */ 
  byte     unit;           /* Request format unit 0=dms, 1=degree */
  byte     acry;           /* Request accuracy 0,1,2,3 */
  byte     num;            /* Number of pos times 0=def */
} pdsm_pm_jgpsone_app_s_type;



/*  GPS FS task operations */
enum
{
  PDSM_FS_CREATE_WRITE_FILE,
  PDSM_FS_APPEND_FILE,
  PDSM_FS_DELETE_FILE,
  PDSM_FS_READ_FILE,
  PDSM_FS_PURGE_DIR, /* Delete all files from a directory */
  PDSM_FS_DELETE_FILE_LIST /* Delete specified files from a directory */
};


#define PDSM_PD_FS_MAX_PATH_LEN_BYTES  64   /* Max file name length in bytes that can be written*/
#define PDSM_PD_EFS_MAX_FILE_LEN_BYTES 2000 /* Max file size in bytes that can be written*/
typedef struct pdsm_efs_data_s {
  uint8          efs_data_filename_len;   /* This field is deprecated */
  char          *efs_data_filename;       /* NULL terminated File name information. The length will be 
                                             determined by the length of this string */
  /*~ FIELD pdsm_efs_data_s.efs_data_filename STRING */
  uint8          efs_data_file_operation; /* File Operation. PDSM_FS_READ_FILE not supported */
  uint32         efs_data_reserved;       /* Reserved for future use */
  uint32         efs_data_len;            /* Length (in bytes) of data content, max length defined by  PDSM_PD_EFS_MAX_FILE_LENGTH_BYTES*/
  unsigned char *efs_data_ptr;            /* Data Content */
  /*~ FIELD pdsm_efs_data_s.efs_data_ptr VARRAY LENGTH pdsm_efs_data_s.efs_data_len */
  uint8          efs_data_part_number;    /* data part number */
  uint8          efs_data_total_parts;    /* total data parts */
} pdsm_efs_data_s_type;

/* external SPI related data structures */
typedef enum {
   PDSM_PA_EXTERNAL_SPI_MODE_DISABLE = 0,
   PDSM_PA_EXTERNAL_SPI_MODE_ENABLE = 1,

   /* add more modes above this line */
   PDSM_PA_EXTERNAL_SPI_MODE_MAX,
   PDSM_PA_EXTERNAL_SPI_MODE_MAX_RESERVED = 0x7FFFFFFF
} pdsm_pa_external_spi_mode;
/*~ SENTINEL pdsm_pa_external_spi_mode.PDSM_PA_EXTERNAL_SPI_MODE_MAX */

typedef struct {
   uint32 mode; /* see pdsm_pa_external_spi_mode */
   uint32  q_Reserved[2]; /* set to 0 */
} pdsm_pa_set_external_spi;

#define PDSM_PA_MAX_APN_NAME_LENGTH 100
#define PDSM_PA_MAX_APN_PROFILES    6

/*APN Profile Serving Systems*/
#define PDSM_PA_APN_PROFILE_SRV_SYS_CDMA  0x01
#define PDSM_PA_APN_PROFILE_SRV_SYS_HDR   0x02
#define PDSM_PA_APN_PROFILE_SRV_SYS_GSM   0x04
#define PDSM_PA_APN_PROFILE_SRV_SYS_WCDMA 0x08
#define PDSM_PA_APN_PROFILE_SRV_SYS_LTE   0x10
#define PDSM_PA_APN_PROFILE_SRV_SYS_TDS   0x20
#define PDSM_PA_APN_PROFILE_SRV_SYS_MAX   0x1F


/*PDP type*/
#define PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4    0x01
#define PDSM_PA_APN_PROFILE_PDN_TYPE_IPV6    0x02
#define PDSM_PA_APN_PROFILE_PDN_TYPE_IPV4V6  0x03
#define PDSM_PA_APN_PROFILE_PDN_TYPE_PPP     0x04
#define PDSM_PA_APN_PROFILE_PDN_TYPE_MAX     0x04



typedef struct {  
  uint32  srv_system_type; /*Serving system on which this APN should be used*/
  uint32  pdp_type; /*Pdp type of the APN profile*/
  uint32  reserved; /*Reserved for future use*/
  uint8   apn_name[PDSM_PA_MAX_APN_NAME_LENGTH]; /*APN name*/
} pdsm_pa_apn_profiles_type;

typedef uint32 pdsm_pa_lpp_config_info_type;
#define PDSM_PA_LPP_IN_LTE_UP_ENABLE 0x01
#define PDSM_PA_LPP_IN_LTE_CP_ENABLE 0x02

/* configuration for SUPL TLS version */
typedef enum {
  PDSM_PA_SUPL_TLS_VERSION_1_0,  /* SUPL TLS Version 1.0 */
  PDSM_PA_SUPL_TLS_VERSION_1_1,  /* SUPL TLS Version 1.1 */
  PDSM_PA_SUPL_TLS_VERSION_MAX,      /* validity checking */
  PDSM_PA_SUPL_TLS_VERSION_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
}pdsm_pa_supl_tls_ver_e_type;
/*~ SENTINEL pdsm_pa_supl_tls_ver_e_type.PDSM_PA_SUPL_TLS_VERSION_MAX */
/* used to configure the protocol for emergency services while on W. For LTE, UE can use either UP or CP based on the trigger that arrives.
   for G and 1x, currently only Control plane is supported */
typedef enum
{  
  PDSM_PA_EMERGENCY_PROTOCOL_CONFIG_WCMDA_CP, /* use Control plane protocol during emergency while on WCDMA */  
  PDSM_PA_EMERGENCY_PROTOCOL_CONFIG_WCMDA_UP/* use SUPL 2.0 emergency services during emergency while on WCDMA */ 
} pdsm_pa_emergency_protocol_e_type;

/* configuration for SUPL hash algorithm */
typedef enum {
  PDSM_PA_SUPL_HASH_ALGO_SHA1,     /* SHA1 Hash Algorithm for SUPL Version 2.0 or higher */
  PDSM_PA_SUPL_HASH_ALGO_SHA256,   /* SHA-256 Hash Algorithm for SUPL Version 2.0 or higher */
  PDSM_PA_SUPL_HASH_ALGO_MAX,      /* validity checking */
  PDSM_PA_SUPL_HASH_ALGO_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
}pdsm_pa_supl_hash_algo_e_type;
/*~ SENTINEL pdsm_pa_supl_hash_algo_e_type.PDSM_PA_SUPL_HASH_ALGO_MAX */

/*  GPS FS task operations */
enum
{
  PDSM_PA_CREATE_CERT_IN_MEM = 0, /*Create certificate in memory*/
  PDSM_PA_DELETE_CERT_IN_MEM, /*Delete certificate in memory*/
  PDSM_PA_PURGE_CERT_IN_MEM /* Delete all certificates in memory */
};

typedef enum{
  PDSM_PA_SUPL_BLOCK_UDP_BIND = 0x0, /*SUPL UDP capability OFF*/
  PDSM_PA_SUPL_ALLOW_UDP_BIND,      /*SUPL UDP capability always ON*/
  PDSM_PA_SUPL_ALLOW_UDP_BIND_E911  /*SUPL UDP capability ON during 911*/
}pdsm_pa_supl_udp_enable_e_type;

#define PDSM_PA_CERT_MAX_FILE_LEN_BYTES PDSM_PD_EFS_MAX_FILE_LEN_BYTES /* Max file size in bytes that can be written*/
#define PDSM_PA_MAX_CERTIFICATES (10) /*Max number of certificates*/
typedef struct pdsm_cert_data_s {
  uint8          cert_id;   /* The Certificate ID  Ran Supported are 0 to (PDSM_PA_MAX_CERTIFICATES  -1)*/
  uint8          cert_operation; /* Cert Operation */
  uint32         cert_len;            /* Length (in bytes) of data content, max length defined by  PDSM_PD_EFS_MAX_FILE_LENGTH_BYTES*/
  unsigned char *cert_ptr;            /* Data Content */
} pdsm_cert_data_s_type;

/* configuration for Premium Services Type */
typedef enum
{
  PDSM_PA_PREMIUM_SVC_GTP_CELL,  /* Premium Service: (GTP)
                                    Global Terrestrial Positioning for Cell */
  PDSM_PA_PREMIUM_SVC_SAP,       /* Premium Service: Sensor-Assisted Positioning */
  PDSM_PA_PREMIUM_SVC_GTP_ENH_CELL, /* Premium Service: Enhanced GTP cel 
                                       Terrestrial Downlink Positioning (TDP) */
  PDSM_PA_PREMIUM_SVC_MAX,       /* validity checking */
  PDSM_PA_PREMIUM_SVC_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
} pdsm_pa_premium_svc_e_type;

/* configuration for Premium Services Config Type */
typedef enum
{
  PDSM_PA_PREMIUM_SVC_CFG_DISABLED,        /* Premium Service Disabled */
  PDSM_PA_PREMIUM_SVC_CFG_ENABLED_BASIC,   /* Premium Service Enabled for Basic */
  PDSM_PA_PREMIUM_SVC_CFG_ENABLED_PREMIUM, /* Premium Service Enabled for Premium */
  PDSM_PA_PREMIUM_SVC_CFG_MAX,             /* validity checking */
  PDSM_PA_PREMIUM_SVC_CFG_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
} pdsm_pa_premium_svc_cfg_e_type;

typedef struct 
{
  pdsm_pa_premium_svc_e_type     service; /* Premium Services Type */
  pdsm_pa_premium_svc_cfg_e_type config;  /* Premium Services Config Type */
} pdsm_pa_premium_svc_cfg_s_type;

/* configuration for XTRA Version Check */
typedef enum
{
  PDSM_PA_XTRA_VERSION_CHECK_DISABLED, /* XTRA Version Check Disabled (not required) */
  PDSM_PA_XTRA_VERSION_CHECK_AUTO,     /* XTRA Version Check Automatic (based on pre-provisioned XTRA version config) */
  PDSM_PA_XTRA_VERSION_CHECK_XTRA2,    /* XTRA Version Check with XTRA2 format */
  PDSM_PA_XTRA_VERSION_CHECK_XTRA3,    /* XTRA Version Check with XTRA3 format */
  PDSM_PA_XTRA_VERSION_CHECK_MAX,      /* validity checking */
  PDSM_PA_XTRA_VERSION_CHECK_MAX_RESERVED = 0x10000000, /* force to 4 bytes */
} pdsm_pa_xtra_ver_check_e_type;

/* This is a pdsm parameter info, IP address, PTLM mode and GPS lock status.
** This strucure is passed as it is to different clients whenever
** they are updates. (const takes care of clients not being able to
** modify the data structure.
*/
typedef union pdsm_pa_info
{
  pdsm_pm_jgpsone_app_s_type        app_info;                       /* Application specific info (for JCDMA users only) */
  pdsm_gps_lock_e_type              gps_lock;
  boolean                           keep_warm_enable;               /* Control for Keep Warm Processing */
  pdsm_delete_parms_type            delete_params;
  pdsm_position_mode_type           position_mode;
  pdsm_pa_mt_lr_support_e_type      mt_lr_support;                  /* 0 is to disable, 1 to enable */
  pdsm_pa_mo_method_e_type          mo_method;
  pdsm_pa_nmea_type                 nmea_sentence_type;
  pdsm_server_address_s_type        server_address;
  pdsm_pd_ssd_s_type                ssd_info;
  pdsm_pd_sec_update_rate_s_type    sec_rate;
  boolean                           pdapi_enable;                   /* enable or disable positioning */
  pdsm_pa_nmea_config_s_type        nmea_config_info;               /* enable nmea port type */
  pdsm_efs_data_s_type              efs_data;                       /* EFS data. Currently used only to inject a SUPL security certificate */
  pdsm_pa_sbas_status_e_type        sbas_status;                    /* SBAS status returned via get_parameters */
  uint8                             sbas_user_preference;           /* enable/disable SBAS (user preference) */
  uint8                             dpo_control;                    /* DPO Control. 0 is to Disable(change NV), 1 to Enable(change NV)
                                                                       0xF0 is to Disable(no NV change), 0xF1 is to Enable(no NV change) 
                                                                      (If NV is changed, new setting is applicable across device power cycles) */
  boolean                           on_demand_lpm;                  /* On Demand Low power mode */
  boolean                           on_demand_ready;                /* On Demand READDY mode */
  uint8                             call_flow_select_mask;          /* Call Flow Select Mask */
  pdsm_pa_set_external_spi          external_spi;                   /* control raising of XSPI injection request event */
  uint32                            client_cookie;                  /* XTRA-T client cookie for data transfer with XTRA-T server */
  uint8                             session_control;                /* User Control: (0 - disable)/(1 - Enable) XTRA-T session */
  pdsm_pa_apn_profiles_type         apn_profiles[PDSM_PA_MAX_APN_PROFILES]; /* APN profiles used by PDCOMM */
  boolean                           data_enable;                    /* enable/disable data */
  uint32                            supl_version;                   /* SUPL version(Bytes from MSB to LSB: reserved, major, minor, serviceIndicator) */
  boolean                           supl_security;                  /* TRUE for SUPL, FALSE for UPL */
  pdsm_pa_pe_config_s_type          pe_config_info;                 /* PE configuration parameter */
  pdsm_pa_lpp_config_info_type      lpp_config_info;                /* configuration for LPP protocol related data */
  uint32                            asst_glo_protocol;              /* Assisted Glonass positioning protocol choice (RRLP, RRC, LPP).
                                                                       The corresponding PDAPI PA event: PDSM_PA_ASST_GLONASS_POS_PROTOCOL &
                                                                       the bitmasks to select protocols: PDSM_PA_ASST_GLO_PROTOCOL_RRLP_CP,
                                                                       PDSM_PA_ASST_GLO_PROTOCOL_RRC_CP ... etc */
  boolean                           supl_is801;                     /* TRUE if IS801 is carried inside SUPL, otherwise FALSE */
  pdsm_pa_supl_tls_ver_e_type       supl_tls_version;               /* configuration for SUPL TLS version */
  pdsm_pa_supl_hash_algo_e_type     supl_hash_algorithm;            /* configuration for SUPL hash algorithm */
  pdsm_cert_data_s_type             cert_data;                      /* Certificate Data */
  pdsm_pa_emergency_protocol_e_type gnss_emergency_protocol_select; /* set the protocol during an emergency call */
  uint8                             wifi_wait_timeout_sec;          /* timeout waiting for WIFI scan result, in seconds */
  pdsm_pa_supl_udp_enable_e_type    supl_udp_enable_select;         /* configuration for SUPL UDP capability */
  pdsm_pa_premium_svc_cfg_s_type    premium_svc_cfg;                /* configuration for a specific Premium Service */
  pdsm_pa_xtra_ver_check_e_type     xtra_version_check;             /* XTRA file version check type */
  pdsm_LocSetGNSSConfigReqStruct    z_set_gnss_config;                /* Constellation configuration for measurement and sv poly */
  uint8                             u_gnss_lpp_grt_24_otdoa;        /* Advertise UE support for >24 OTDOA AD processing & generate measurements */
} pdsm_pa_info_type;
/*~ IF (_DISC_ == PDSM_PA_APP_INFO) pdsm_pa_info.app_info */
/*~ IF (_DISC_ == PDSM_PA_GPS_LOCK)    pdsm_pa_info.gps_lock */
/*~ IF (_DISC_ == PDSM_PA_KEEP_WARM)    pdsm_pa_info.keep_warm_enable */
/*~ IF ((_DISC_ ==  PDSM_PA_DELETE_PARAMS) || (_DISC_ ==  PDSM_PA_DELETE_PARAMS_EX) || PDSM_PA_DELETE_PARAMS_EX1) pdsm_pa_info.delete_params */
/*~ IF (_DISC_ ==  PDSM_PA_POSITION_MODE) pdsm_pa_info.position_mode */
/*~ IF (_DISC_ ==  PDSM_PA_MT_LR_SUPPORT) pdsm_pa_info.mt_lr_support */
/*~ IF (_DISC_ ==  PDSM_PA_MO_METHOD) pdsm_pa_info.mo_method */
/*~ IF (_DISC_ == PDSM_PA_NMEA_SENTENCE_TYPE) pdsm_pa_info.nmea_sentence_type */
/*~ IF ((_DISC_ == PDSM_PA_1X_PDE_NETWORK_ADRS) || (_DISC_ == PDSM_PA_1X_MPC_NETWORK_ADRS) || (_DISC_ == PDSM_PA_UMTS_PDE_NETWORK_ADRS))      pdsm_pa_info.server_address */ 
/*~ IF (_DISC_ == PDSM_PA_SSD_DATA)     pdsm_pa_info.ssd_info */
/*~ IF (_DISC_ == PDSM_PA_SEC_UPDATE_RATE)  pdsm_pa_info.sec_rate */
/*~ IF (_DISC_ == PDSM_PA_PDAPI_ENABLE)  pdsm_pa_info.pdapi_enable */
/*~ IF (_DISC_ == PDSM_PA_NMEA_CONFIG_INFO)  pdsm_pa_info.nmea_config_info */
/*~ IF (_DISC_ == PDSM_PA_EFS_DATA)    pdsm_pa_info.efs_data */
/*~ IF (_DISC_ == PDSM_PA_SBAS_STATUS) pdsm_pa_info.sbas_status */
/*~ IF (_DISC_ == PDSM_PA_SBAS_USER_PREFERENCE) pdsm_pa_info.sbas_user_preference */
/*~ IF (_DISC_ == PDSM_PA_DPO_CONTROL)    pdsm_pa_info.dpo_control */
/*~ IF (_DISC_ == PDSM_PA_ON_DEMAND_LOW_POWER_MODE)    pdsm_pa_info.on_demand_lpm */
/*~ IF (_DISC_ == PDSM_PA_ON_DEMAND_READY_MODE)    pdsm_pa_info.on_demand_ready */
/*~ IF (_DISC_ == PDSM_PA_CALL_FLOW_SELECT)        pdsm_pa_info.call_flow_select_mask */
/*~ IF (_DISC_ == PDSM_PA_EXTERNAL_SPI) pdsm_pa_info.external_spi */
/*~ IF (_DISC_ == PDSM_PA_XTRAT_CLIENT_TOKEN) pdsm_pa_info.client_cookie */
/*~ IF (_DISC_ == PDSM_PA_XTRAT_USER_SESSION_CONTROL) pdsm_pa_info.session_control */
/*~ IF ((_DISC_ == PDSM_PA_LBS_APN_PROFILE) || (_DISC_ == PDSM_PA_XTRA_APN_PROFILE))      pdsm_pa_info.apn_profiles */ 
/*~ IF (_DISC_ == PDSM_PA_DATA_ENABLE) pdsm_pa_info.data_enable */
/*~ IF (_DISC_ == PDSM_PA_SUPL_VERSION) pdsm_pa_info.supl_version */
/*~ IF (_DISC_ == PDSM_PA_SUPL_SECURITY) pdsm_pa_info.supl_security */
/*~ IF (_DISC_ == PDSM_PA_PE_CONFIG_PARAM) pdsm_pa_info.pe_config_info */
/*~ IF (_DISC_ == PDSM_PA_LPP_CONFIG_INFO) pdsm_pa_info.lpp_config_info */
/*~ IF (_DISC_ == PDSM_PA_ASST_GLONASS_POS_PROTOCOL) pdsm_pa_info.asst_glo_protocol */
/*~ IF (_DISC_ == PDSM_PA_SUPL_IS801) pdsm_pa_info.supl_is801 */
/*~ IF (_DISC_ == PDSM_PA_SUPL_TLS_VERSION) pdsm_pa_info.supl_hash_algorithm */
/*~ IF (_DISC_ == PDSM_PA_SUPL_HASH_ALGORITHM) pdsm_pa_info.asst_glo_protocol */
/*~ IF (_DISC_ == PDSM_PA_PREMIUM_SERVICES_CONFIG) pdsm_pa_info.premium_svc_cfg */
/*~ IF (_DISC_ == PDSM_PA_XTRA_VERSION_CHECK) pdsm_pa_info.xtra_version_check */
/*~ DEFAULT pdsm_pa_info.void */

/* Bit mask to choose between different A-Glonass positioning protocols in 'asst_glo_protocol' field
**            LSB Bit 0 - RRLP (C-plane), Bit 1 - RRC  (C-plane), Bit 2 - LPP  (C-plane)
**                Bit 3 to 7 - Unused
**                Bit 8 - RRLP (U-plane), Bit 9 - Reserved, Bit 10 - LPP  (U-plane)
**                Bit 11 to 31 - Unused
*/
#define PDSM_PA_ASST_GLO_PROTOCOL_RRLP_CP (0x01)
#define PDSM_PA_ASST_GLO_PROTOCOL_RRC_CP  (0x02)
#define PDSM_PA_ASST_GLO_PROTOCOL_LPP_CP  (0x04)
#define PDSM_PA_ASST_GLO_PROTOCOL_RRLP_UP (0x100)
#define PDSM_PA_ASST_GLO_PROTOCOL_LPP_UP  (0x0400)

/* Bit mask to change call_flow_select_mask setting in pdsm_pa_info_type */
/* bit 0-1 := Call Flow Select for MO-LR or AD in UMTS control plan 
**            bit 0 - 2G call flow select := 0 - AD;   1 - Location Estimate
**            bit 1 - 3G call flow select := 0 - AD;   1 - Location Estimate
*/
#define PDSM_PA_CALL_FLOW_SELECT_2G  (0x01)
#define PDSM_PA_CALL_FLOW_SELECT_3G  (0x02)

/* Enumeration of commands PDSM.
*/
typedef enum pdsm_pa_cmd_e {
    PDSM_PA_CMD_NONE =-1,           /* FOR INTERNAL USE OF PDSM ONLY! */
    PDSM_PA_CMD_SET_PARAM,         /* Set Parameter Command */
    PDSM_PA_CMD_GET_PARAM,         /* Get Parameter command */
    PDSM_PA_CMD_MAX,               /* FOR INTERNAL USE OF PDSM ONLY! */
    PDSM_PA_CMD_E_SIZE=0x10000000  /* To force enum to 32 bit for windows NT */
} pdsm_pa_cmd_e_type;
/*~ SENTINEL pdsm_pa_cmd_e_type.PDSM_PA_CMD_MAX */



/* typedef enum to specify the session type being ended by user application.
*/
typedef enum {
  PDSM_PD_END_SESSION_MIN = -1,
  PDSM_PD_END_SESSION_FIX_REQ,
  PDSM_PD_END_SESSION_DLOAD_REQ,
  PDSM_PD_END_SESSION_MAX = 0x10000000
} pdsm_pd_end_session_e_type;

typedef enum pdsm_lcs_cmd_e 
{
  PDSM_LCS_CMD_NONE = -1,
  PDSM_LCS_CMD_NOTIFY_VERIFY_RESP,
  PDSM_LCS_CMD_CLIENT_APP_INFO_INIT,
  PDSM_LCS_CMD_CLIENT_RELEASE,
  PDSM_LCS_CMD_GET_CLIENT_STATUS,
  PDSM_LCS_CMD_QUERY_VX_CONFIG,
  PDSM_LCS_CMD_SET_VX_CONFIG,
  PDSM_LCS_CMD_MAX,
  PDSM_LCS_CMD_E_SIZE=0x10000000
} pdsm_lcs_cmd_e_type;
/*~ SENTINEL pdsm_lcs_cmd_e_type.PDSM_LCS_CMD_MAX */


typedef enum pdsm_ext_pos_cmd_e
{
 PDSM_EXT_POS_CMD_NONE =-1,           /* FOR INTERNAL USE OF PDSM ONLY! */
 PDSM_EXT_USE_THIS_POS,
 PDSM_EXT_POS_CMD_MAX,
 PDSM_EXT_POS_CMD_E_SIZE=0x10000000
} pdsm_ext_pos_cmd_e_type;
/*~ SENTINEL pdsm_ext_pos_cmd_e_type.PDSM_EXT_POS_CMD_MAX */


/*- - - - - - - - - - - - - - EVENTS - - - - - - - - - - - - - - - - - - - - - -*/


/* Postion Determination Events.
** Clients are able to register with PDSM to be notified of any subsets
** of these events.
*/
typedef uint32 pdsm_pd_event_type;
/* General PDSM Events */
#define PDSM_PD_EVENT_POSITION 0x01
#define PDSM_PD_EVENT_VELOCITY 0x02
#define PDSM_PD_EVENT_HEIGHT   0x04
#define PDSM_PD_EVENT_DONE     0x08
#define PDSM_PD_EVENT_END      0x10    /* Indicating End of session. */
#define PDSM_PD_EVENT_BEGIN    0x20    /* Indicate the start of session. */

#define PDSM_PD_GENERAL_EVENT_MASK ( (PDSM_PD_EVENT_POSITION) | (PDSM_PD_EVENT_VELOCITY)\
                            | (PDSM_PD_EVENT_HEIGHT) | (PDSM_PD_EVENT_DONE)\
                            | (PDSM_PD_EVENT_END) | (PDSM_PD_EVENT_BEGIN)\
                            | (PDSM_PD_EVENT_UPDATE_FAILURE) )
                            
/* Communication related events */
#define PDSM_PD_EVENT_COMM_BEGIN      0x00000040
#define PDSM_PD_EVENT_COMM_CONNECTED  0x00000080
#define PDSM_PD_EVENT_COMM_FAILURE    0x00000100
#define PDSM_PD_EVENT_COMM_DONE       0x00000200

#define PDSM_PD_COMM_EVENT_MASK ( (PDSM_PD_EVENT_COMM_BEGIN) | (PDSM_PD_EVENT_COMM_CONNECTED) \
                                   | (PDSM_PD_EVENT_COMM_FAILURE) | (PDSM_PD_EVENT_COMM_DONE) | (PDSM_PD_EVENT_JGPS_PROGRESS)  )
                                   
/* Keep Warm processing related events */
#define PDSM_PD_EVENT_WARM_PROC_BEGIN  0x00000400
#define PDSM_PD_EVENT_WARM             0x00000800
#define PDSM_PD_EVENT_NOT_WARM         0x00001000
#define PDSM_PD_EVENT_WARM_PROC_DONE   0x00002000

#define PDSM_PD_WARM_EVENT_MASK ( (PDSM_PD_EVENT_WARM_PROC_BEGIN) | (PDSM_PD_EVENT_WARM) \
                                 | (PDSM_PD_EVENT_NOT_WARM) | (PDSM_PD_EVENT_WARM_PROC_DONE) )
                                 
/* Positioning Processing related events */
#define PDSM_PD_EVENT_GPS_BEGIN        0x00004000
#define PDSM_PD_EVENT_GPS_DONE         0x00008000

#define PDSM_PD_GPS_EVENT_MASK ( (PDSM_PD_EVENT_GPS_BEGIN) | (PDSM_PD_EVENT_GPS_DONE) )

/* request on-demand WiFi scan */
#define PDSM_PD_EVENT_REQUEST_WIFI_INFO_ON_DEMAND_INJECTION 0x00010000
#define PDSM_PD_EVENT_ACK_WIFI_INFO_ON_DEMAND_INJECTION 0x00020000

/* Security event: Security events can possibly happen out side of PD sessions. (Only exception of all PD events)
*/
#define PDSM_PD_EVENT_SECURITY    0x00100000  /* Update Security Event for Random Number */

/* Coase Position Injection related */
#define PDSM_PD_EVENT_COARSE_POS_INJ_DONE     0x00200000
#define PDSM_PD_EVENT_COARSE_POS_INJ_FAILED   0x00400000
#define PDSM_PD_EVENT_COARSE_POS_INJ_ERROR    0x00800000
#define PDSM_PD_EVENT_COARSE_POS_INJ_MASK ( (PDSM_PD_EVENT_COARSE_POS_INJ_DONE) \
                                            | (PDSM_PD_EVENT_COARSE_POS_INJ_FAILED) \
                                            | (PDSM_PD_EVENT_COARSE_POS_INJ_ERROR))

#define PDSM_PD_EVENT_UPDATE_FAILURE           0x01000000  /* Update event indicating Failure */

/* Japan-GPS progress events */
#define PDSM_PD_EVENT_JGPS_PROGRESS            0x02000000
#define PDSM_PD_JGPS_EVENT_MASK                (PDSM_PD_EVENT_JGPS_PROGRESS)


#define PDSM_PD_EVENT_WPS_NEEDED  0x04000000


/* request change in XSPI injection status. See also 
pdsm_set_parameters(PDSM_PA_EXTERNAL_SPI) and pdsm_pd_inject_external_spi() */
#define PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION 0x08000000  

#define PDSM_PD_EVENT_MASK  ( (PDSM_PD_GENERAL_EVENT_MASK) \
                             | (PDSM_PD_COMM_EVENT_MASK) \
                             | (PDSM_PD_WARM_EVENT_MASK) \
                             | (PDSM_PD_GPS_EVENT_MASK) \
                             | (PDSM_PD_DLOAD_EVENT_MASK) \
                             | (PDSM_PD_EVENT_SECURITY) \
                             | (PDSM_PD_JGPS_EVENT_MASK) \
                             | (PDSM_PD_EVENT_COARSE_POS_INJ_MASK) \
                             | (PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION) \
                             | (PDSM_PD_EVENT_REQUEST_WIFI_INFO_ON_DEMAND_INJECTION) \
                             | (PDSM_PD_EVENT_ACK_WIFI_INFO_ON_DEMAND_INJECTION) \
                            )

/* Dload specific events.  */
#define PDSM_PD_EVENT_DLOAD_BEGIN   0x10000000UL  /* Dload begin event */
#define PDSM_PD_EVENT_DLOAD         0x20000000UL  /* Dload occurred event */
#define PDSM_PD_EVENT_DLOAD_DONE    0x40000000UL  /* Dload done event */
#define PDSM_PD_DLOAD_EVENT_END     0x80000000UL  /* Indicating End of dload session. */

#define PDSM_PD_DLOAD_EVENT_MASK ((PDSM_PD_EVENT_DLOAD_BEGIN) |\
                                  (PDSM_PD_EVENT_DLOAD)       |\
                                  (PDSM_PD_EVENT_DLOAD_DONE)  |\
                                  (PDSM_PD_DLOAD_EVENT_END))

/* --------------------- EVENT CALLBACK DATA STRUCTURES -------------------*/
typedef uint32 pdsm_pd_positioning_source_type;

#define PDSM_PD_POSITION_SOURCE_GPS         0x0001  /* GPS Positions*/
#define PDSM_PD_POSITION_SOURCE_CELLID      0x0002  /* Cell ID positions from the celldb */
#define PDSM_PD_POSITION_SOURCE_AFLT        0x0004  /* AFLT position */
#define PDSM_PD_LOCATION_SOURCE_HYBRID      0x0008  /* HYBRID position */
#define PDSM_PD_LOCATION_SOURCE_CELL_ERROR  0x0010  /* CELL ERRROR*/
#define PDSM_PD_POSITION_SOURCE_DEFAULT     0x0020  /* REFERENCE position from the network in a MSB/MSA or CELL ID call flow */
#define PDSM_PD_POSITION_SOURCE_UNKNOWN     0x0040  /* source is unknown, could be an externally injected or propaged position */
#define PDSM_PD_POSITION_SOURCE_GLO         0x0080  /* Source is GLONASS */

/*
  EPI stands for External Position Injection. It is a general term used
  to mean any externally provided 2D or 3D position was used to constraint
  the position fix generated by the position engine. EPI examples are Coarse
  position injection, Cell Database position, Reference Location from the 
  AGPS server, etc.
*/
#define PDSM_PD_POSITION_SOURCE_EPI         0x0100  /* Externally Injected Position constraint used */
#define PDSM_PD_POSITION_SOURCE_WIFI        0x0200  /* Source is Wifi*/
#define PDSM_PD_POSITION_SOURCE_SENSORS     0x0400  /* Source is Sensors*/
#define PDSM_PD_POSITION_SOURCE_BDS         0x0800  /* Source is BeiDou */
#define PDSM_PD_POSITION_SOURCE_GAL         0x1000  /* Source is Galileo */
#define PDSM_PD_POSITION_SOURCE_QZSS        0x2000  /* Source is QZSS  */
typedef enum pdsm_pd_gps_position_e {
  PDSM_PD_POSITION_TYPE_INVALID=-1,
  PDSM_PD_POSITION_TYPE_OLD = 1,
  PDSM_PD_POSITION_TYPE_NEW =2
} pdsm_pd_gps_position_e_type;

typedef enum{
  PDSM_PD_POSITION_MODE_INVALID=-1,
  PDSM_PD_POSITION_MODE_MSBASED,
  PDSM_PD_POSITION_MODE_MSASSISTED,
  PDSM_PD_POSITION_MODE_STANDALONE,
  PDSM_PD_POSITION_MODE_UNKNOWN,
  PDSM_PD_POSITION_MODE_OTHER,
  PDSM_PD_POSITION_MODE_MAX
} pdsm_pd_position_mode_e_type;

/* valid Opitonal Fields */
//typedef byte pdsm_pd_opt_field;
typedef char pdsm_pd_opt_field;

#define PDSM_PD_ALTITUDE_VALID 0x01
#define PDSM_PD_VELOCITY_VALID  0x02  /* Velocity means speed and direction */
#define PDSM_PD_UTCOFFSET_VALID 0x04
#define PDSM_PD_UNCERTAINTY_VALID 0x08
#define PDSM_PD_EXT_STATUS_VALID  0x10
#define PDSM_PD_MSEC_VALID 0x20       /* Millisecond field available */
#define PDSM_PM_DATA_AVAIL 0x40       /* PM data available */

#define PDSM_PD_MEAS_DEBUG 0x80       /* Additional measurement info */

#define PDSM_PD_OPT_FIELD_MASK  ( (PDSM_PD_ALTITUDE_VALID) | (PDSM_PD_VELOCITY_VALID) |\
                                  (PDSM_PD_UTCOFFSET_VALID) | (PDSM_PD_UNCERTAINTY_VALID) | \
                                  (PDSM_PD_EXT_STATUS_VALID) | (PDSM_PD_MSEC_VALID) ) 
typedef uint32 pdsm_pd_opt_field_ext;
#define PDSM_PD_LAT_LONG_VALID 0x01 /* indicates if the lat/lon in the position report and ext pos is 
                                       valid */
#define PDSM_PD_GPS_TIME_VALID 0x02 /* indicates if the time stamp in the ext pos has a valid GPS time */
#define PDSM_PD_XTRA_DATA_VALID 0x04 /* indicates if the XTRA data in the ext pos is valid */
#define PDSM_PD_UTC_TIME_VALID 0x08 /* indicates if the UTC time stamp in the ext pos is valid */
#define PDSM_PD_HEADING_VALID  0x10 /*indicates if heading in position report and ext pos is
                                      valid*/
/* Force the enum to be int for optimization.
*/
typedef enum{
  PDSM_PD_DATA_INVALID,
  PDSM_PD_DATA_VALID,
  PDSM_PD_DATA_MAX = 0x10000000
} pdsm_pd_data_valid_e_type;

/*
 * Time between two ppm reports may be specified in the API call 
 * pdsm_start_periodic_ppm_report. The constant PDSM_MAX_PPM_PERIOD
 * gives the maximum value of the time period specified in that API call
 */
#define PDSM_MAX_PPM_PERIOD                   600  
/* following data structure for JGPSONE only.
*/
#define PDSM_SRCH_MAX_PPM_RM_RPT_SAMPLES      25   /* caution: this follows SRCH_MAX_PPM.. */
#define PDSM_MAX_NUM_SATS_IN_AIDING_DATA      32   /* caution: this follows MAX_NUM_SATS.. */
#define PDSM_MAX_DLOAD_DATA                   5

/* Typedef struct for Pilot position.
*/
typedef struct pdsm_pilot_info {
    int4    pilot_pn_phase;
    byte    pilot_rmse;
    byte    pilot_ecio;
} pdsm_pilot_info_type;

/* typedef struct for PPM report from Searcher.
*/
typedef struct pdsm_srch_ppm_rpt {
    qword                  ts;
    boolean                offset_incl;
    word                   mob_sys_t_offset;
    word                   ref_pn;
    byte                   ref_pn_ecio;
    byte                   band_class;
    word                   cdma_freq;
    word                   base_id;
    word                   sid;
    word                   nid;
    byte                   total_rx_pwr;
    byte                   add_band_class;   /* additional band class */
    word                   add_cdma_freq;    /* additional cdma frequency */
    byte                   num_pilots;
    pdsm_pilot_info_type   pilot_results[PDSM_SRCH_MAX_PPM_RM_RPT_SAMPLES];
    /*~ FIELD pdsm_srch_ppm_rpt.pilot_results POINTER 
        VARRAY PDSM_SRCH_MAX_PPM_RM_RPT_SAMPLES
        LENGTH pdsm_srch_ppm_rpt.num_pilots
    */
} pdsm_srch_ppm_rpt_type;

typedef uint16 gnss_sv_id_type;      /* SV IDs for GNSS constellations is given below */
#define  PDSM_PD_SV_ID_GPS_MIN      1
#define  PDSM_PD_SV_ID_GPS_MAX      32
#define  PDSM_PD_SV_ID_SBAS_MIN     33
#define  PDSM_PD_SV_ID_SBAS_MAX     64
#define  PDSM_PD_SV_ID_GLO_MIN      65
#define  PDSM_PD_SV_ID_GLO_MAX      96
#define  PDSM_PD_SV_ID_QZSS_MIN     193
#define  PDSM_PD_SV_ID_QZSS_MAX     197
#define  PDSM_PD_SV_ID_BDS_MIN     201
#define  PDSM_PD_SV_ID_BDS_MAX     237
#define  PDSM_PD_SV_ID_GAL_MIN     301
#define  PDSM_PD_SV_ID_GAL_MAX     336
#define  PDSM_PD_SV_ID_UNKNOWN     0xffff

/* Typedef struct for GNSS channel report.
*/
typedef struct pdsm_gps_chan_rpt {
    gnss_sv_id_type     sv_prn_num;          /* Satellite ID */

    byte     sv_cno;              /* satlellite C/No */
    int32    ps_doppler;          /* Pseudodoppler */
    word     sv_code_ph_wh;       /* Satellite code phase, whole chips */
    word     sv_code_ph_fr;       /* Satellite code phase, fract chips */
    int32    sv_code_ph_fr_ms;    /* Satellite code phase, fract ms, 
                                     units 2^(-21) ms, 
                                     range 0 to (1 - 2^(-21) ) */
    byte     mul_path_ind;        /* Multipath Indicator */
    int32    ps_range_rms_er;     /* Pseudorange RMS error */
} pdsm_gps_chan_rpt_type;

/* Typedef struct for GNSS satellite report.
*/
typedef struct pdsm_srch_gps_sat_rpt {
    byte       num_sats;
    qword      time_ref;
    byte       time_ref_src;
    byte       offset_incl;
    word       ref_pn;
    int2       mob_sys_t_offset;
    pdsm_gps_chan_rpt_type   gps_chan_rpt[ PDSM_MAX_NUM_SATS_IN_AIDING_DATA];
    /*~ FIELD pdsm_srch_gps_sat_rpt.gps_chan_rpt POINTER 
        VARRAY PDSM_MAX_NUM_SATS_IN_AIDING_DATA
        LENGTH pdsm_srch_gps_sat_rpt.num_sats
    */  
} pdsm_srch_gps_sat_rpt_type;

typedef struct{
  pdsm_srch_ppm_rpt_type       ppm_data;  /* Pilot phase measurements */
  pdsm_srch_gps_sat_rpt_type   pr_data;   /* Pseudo range measurements */
} pdsm_pd_meas_debug_type;

/* Fix data Stuct for Pos completetion msg SCU=>MS data in Text format.
*/
typedef struct {
  char  ver;                      /* Protocol Version between CP and LIS */
  char  datum;                    /* Positioning System */
  char  unit;                     /* Request format */
  char  lat[13];    /* latitude */
  char  lon[13];   /* longitude */
  char  alt[4];                   /* Altitude */
  char  time[14];                 /* Positioing time (JST)*4 */
  char  smaj[4];                  /* Major axis error */
  char  smin[4];                  /* Minor axis error */
  char  vert[4];                  /* Altitude error */
  char  majaa[3];                 /* Major/Minor axis tilt error */
  char  fm;                       /* Positioning mode */
} pm_scu_ms_pos_fixdata_s_type;

typedef pm_scu_ms_pos_fixdata_s_type pm_scu_location_resp_type;


typedef struct pdsm_utc_time_s
{
  
                                     /* UTC Time  for Hour, Minute, Sec */
  uint8         hour;                /*  in range of  00..23 */
  uint8         minute;              /*  in range of 00..59  */
  uint16        sec;                 /*  ssss [x100 sec]  in range of  0..5999 */
                                    
  uint32        date;                /* UTC Date ddmmyyyy 
                                         dd  in range of [1..31]
                                         mm in range of [1..12]
                                         yyyy in range of [0..9999]   
                                     */
} pdsm_utc_time_s_type;

typedef uint8 pdsm_gnss_time_source_mask;
#define PDSM_GNSS_TIME_GPS    (0x01<<0)
#define PDSM_GNSS_TIME_GLO    (0x01<<1)
#define PDSM_GNSS_TIME_BDS    (0x01<<2)
#define PDSM_GNSS_TIME_GAL    (0x01<<3)

typedef enum {
 PDSM_GNSS_TIME_SRC_UNKNOWN                  = 0x00, /* Source of the Time is unknown */
 PDSM_GNSS_TIME_SRC_NETWORK_TIME_TRANSFER    = 0x01, /* Time is set by 1x system */
 PDSM_GNSS_TIME_SRC_NETWORK_TIME_TAGGING     = 0x02, /* Time is set by WCDMA/GSM time tagging.
                                                            That is, associating network time with GPS time */
 PDSM_GNSS_TIME_SRC_EXTERNAL_INPUT           = 0x03, /* Time is set by an external injection */
 PDSM_GNSS_TIME_SRC_GPS_TOW_DECODE           = 0x04, /* Time is set after decoding over the air GPS navigation data
                                                            from one GPS satellite */
 PDSM_GNSS_TIME_SRC_TOW_CONFIRMED            = 0x05, /* Time is set after decoding over the air GPS navigation data
                                                            from multiple satellites */
 PDSM_GNSS_TIME_SRC_TOW_WEEK_CONFIRMED       = 0x06, /* Both time of the week and the GPS week number known */
 PDSM_GNSS_TIME_SRC_RESERVED                 = 0x07, /* Reserved */
 PDSM_GNSS_TIME_SRC_NAV_SOLUTION             = 0x08, /* Time is set by position engine
                                                            after the fix is obtained. */
 PDSM_GNSS_TIME_SRC_SOLVE_FOR_TIME           = 0x09, /* Time is set by the position engine after performing SFT.
                                                            This is done when the clock time uncertainty is large */
 PDSM_GNSS_TIME_SRC_GLO_TOW_DECODE           = 0x0A, /* Time is set after decoding GLO satellites */
 PDSM_GNSS_TIME_SRC_TIME_TRANSFORM           = 0x0B, /* Time is set after transforming the GPS to GLO time */
 PDSM_GNSS_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING = 0x0C, /* Time is set by the sleep timetag
                                                            provided by WCDMA network */
 PDSM_GNSS_TIME_SRC_GSM_SLEEP_TIME_TAGGING   = 0x0D, /* Time is set by the sleep timetag
                                                            provided by GSM network */
 PDSM_GNSS_TIME_SRC_QZSS_TOW_DECODE          = 0x0E, /* Time is set after decoding QZSS satellites */
 PDSM_GNSS_TIME_SRC_BDS_TOW_DECODE           = 0x0F, /* Time is set after decoding BDS satellites */
 PDSM_GNSS_TIME_SRC_GAL_TOW_DECODE           = 0x10, /* Time is set after decoding GAL satellites */
 PDSM_GNSS_TIME_SRC_INVALID                  = 0xFF, /* Invalid time */
 PDSM_GNSS_TIME_SRC_MAX                      = 0x10000000  /* force enum to 32 bit */
} pdsm_extended_time_src_e_type;

typedef struct pdsm_pd_ext_pos_data_s 
{
  int32  lat;                 /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2 */
                              /* Valid if ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set*/
  int32  lon;                 /* Longitude: LSB = Pi/2^31, Range -Pi to Pi */
                              /* Valid if ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set*/
  uint16  alongAxisUnc;       /* Along axis Unc in meters. Max value is 65535. If this value is 
                                 greater than 65535, then the opt_field_mask for 
                                 PDSM_PD_UNCERTATINITY_VALID is not set. loc_unc_horizontal value
                                 is used to get the circular UNC */
                              /* Valid if opt_field_mask PDSM_PD_UNCERTAINTY_VALID is set */
  uint16  perpAxisUnc;        /* Perp axis Unc in meters. Max value is 65535. If this value is 
                                 greater than 65535, then the opt_field_mask for 
                                 PDSM_PD_UNCERTATINITY_VALID is not set. loc_unc_horizontal value
                                 is used to get the circular UNC   */
                              /* Valid if field_mask PDSM_PD_UNCERTAINTY_VALID is set */
  int16  velHor;              /* Horizontal Velocity: cm/sec., magnitude only, max at 0x7FFF */
                              /* Valid if opt_field_mask PDSM_PD_VELOCITY_VALID is set */
  int16  velVert;             /* Vertical Velocity: cm/sec., positive for upward */
                              /* Valid if opt_field_mask PDSM_PD_VELOCITY_VALID is set */
  dword  timestamp_tow_ms;    /* gnss time stamp, number of ms this week (TOW MilliSeconds) */
                              /* Valid if ext_opt_field_mask PDSM_PD_GPS_TIME_VALID is set */
  uint16 timestamp_gps_week;  /* gnss time stamp, number of weeks since GPS Week */
                              /* Valid if ext_opt_field_mask PDSM_PD_GPS_TIME_VALID is set */
  uint32 time_stamp_unc;      /* gnss time uncertainty */
  uint16 velUncHoriz;         /* Velocity Uncertainity horizontal: cm/sec. */
                              /* Valid if opt_field_mask PDSM_PD_VELOCITY_VALID is set */
  uint16 velUncVert;          /* Velocity Uncertainity vertical: cm/sec. */
                              /* Valid if opt_field_mask PDSM_PD_VELOCITY_VALID is set */
  boolean pos_reported_to_network; /* flag indicating if this position was reported to the network */
  uint32        h_dop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
                                     /* Valid if ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set*/
  uint32        p_dop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
                                     /* Valid if ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set*/
  uint32        v_dop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */  
                                     /* Valid if ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set*/
  pdsm_utc_time_s_type  utc_time; /*  Utc time containing year,month,utc_days,hours,nin,secs,tenths*/
                                  /* Valid if ext_opt_field_mask PDSM_PD_UTC_TIME_VALID is set */
  uint32  loc_unc_horizontal;     /*  The HEPE value in meters. In case PDSM_PD_UNCERTAINITY_VALID
                                      is set to invalid, use this value for uncertainity. This is a
                                      HEPE value, so to set it to alongAxisUnc and PerpAxisUnc, divide
                                      by sqrt(2) */
  uint16  xtra_start_gps_week;    /*  Current XTRA info is good starting this GPS week */
                                  /* Valid if ext_opt_field_mask PDSM_PD_XTRA_DATA_VALID is set */
  uint16  xtra_start_gps_minutes; /*  Current XTRA information is good starting this many minutes into the week */
                                  /* Valid if ext_opt_field_mask PDSM_PD_XTRA_DATA_VALID is set */
  uint16  xtra_valid_duration_hours; /*  XTRA info is valid for this many hours */
                                     /* Valid if ext_opt_field_mask PDSM_PD_XTRA_DATA_VALID is set */

  pdsm_gnss_time_source_mask  time_src;      /* source of gnss time */
  boolean glo_offset_valid;   /* glonass offset is valid flag, true if we have glonass time
                                 false otherwise */
  uint32  glo_offset;         /* milliseconds offset to be applied to gps time to get
                                 glonass time */  
  pdsm_extended_time_src_e_type    extended_time_source; /* Extended Time source information */

  float gps_time_unc_ms; /* GPS time uncertainty in ms, valid only when PDSM_PD_GPS_TIME_VALID is set. Obsoletes time_stamp_unc. */

  float heading_rad;      /* heading in rad., valid only when PDSM_PD_HEADING_VALID is set */
  float heading_unc_rad;  /* heading uncertainty in rad., valid only when PDSM_PD_HEADING_VALID is set */

  float gnss_heading_rad;     /* GNSS-only heading in rad., valid only when PDSM_PD_HEADING_VALID is set */
  float gnss_heading_unc_rad; /* GNSS-only heading uncertainly in rad., valid only when PDSM_PD_HEADING_VALID is set */
  float f_altitude; /* Floating point value of the altitude in meters. Valid only if  opt_field_mask PDSM_PD_ALTITUDE_VALID is set*/

} pdsm_pd_ext_pos_data_s_type;

/* Originally defined in gnss_consts.h, redefined in pdapi.h to avoid including gnss_consts.h */
#define PDSM_N_ACTIVE_GPS_CHAN     16  /* Total number of active GPS channels */
#define PDSM_N_ACTIVE_GLO_CHAN     14  /* Total number of active Glonass channels */
#define PDSM_N_ACTIVE_BDS_CHAN     16  /* Total number of active BDS channels */
#define PDSM_N_ACTIVE_GAL_CHAN     16  /* Total number of active GAL channels */
#define PDSM_N_ACTIVE_QZSS_CHAN    16  /* Total number of active QZSS channels */
/* If set, GNSS SV measurement is valid (detected by searcher) */
#define PDSM_GNSS_SV_INFO_MEAS_VALID       0x00000001 
/* If set, GNSS SV is usable: {GPS|BDS} PRNs or {GLO} Slot IDs */ 
#define PDSM_GNSS_SV_INFO_USABLE           0x00000002 
/* If set, GNSS SV is used for pos fix:   {GPS|BDS} PRNs or {GLO} Slot IDs */
#define PDSM_GNSS_SV_INFO_USED_IN_POS_FIX  0x00000004 
/* If set, GNSS SV is used for velocity fix: {GPS|BDS} PRNs or {GLO} Slot IDs */
#define PDSM_GNSS_SV_INFO_USED_IN_VEL_FIX  0x00000008 

#ifdef FEATURE_GNSS_LEAN_BUILD
#pragma anon_unions
#endif

typedef struct
{
  uint32 q_SvInfoMask;  /* See SM_GNSS_SV_INFO_XXX definition bits above.
                         * For XXX_VALID bits, if set, then corresponding data is valid, otherwise not valid */

  uint16 w_SvId;        /* GNSS SV: {GPS|BDS|GAL} PRN or {GLO} SlotID. */

  /* GNSS SV-specific data for SVs used in Position Fix (valid if PDSM_GNSS_SV_INFO_USED_POS_FIX is set) */
  float  f_PrResMeters; /* Pseudo-range measurement residuals of GNSS SV */
  float  f_PrUncMeters; /* Pseudo-range weights (uncertainties) of GNSS SV */

  /* GNSS SV-specific data for SVs used in Velocity Fix (valid if SM_GNSS_SV_INFO_VEL_FIX_VALID is set) */
  float  f_PrrResMps;   /* Pseudo-range rate (doppler) measurement residuals of GNSS SV */
  float  f_PrrUncMps;   /* Pseudo-range rate (doppler) uncertainties of GNSS SV */

  union 
  {
    uint16 w_IODE;      // IODE {GPS|BDS|GAL} 
    uint8  u_GloTb;     // Tb for {GLO} SV 
  } ; 

  /* GNSS SV-specific data for SVs detected by searcher (valid if SM_GNSS_SV_INFO_MEAS_VALID is set) */
  int8   b_FreqNum;     /* Frequency number(-7~+6) of the Glonass SV, invalid for GPS/BDS SV
                           valid if PDSM_GNSS_SV_INFO_USED_POS_FIX is set as well*/

  uint8  u_SvFSAvail;   /* Indication for FineSpeed Availability 1:Available 0:NotAvailable */
  uint8  u_SvPrtyGood;  /* Indication for whether any parity errors have been
                         * observed in the last second for each GNSS SV
                         * = 1:Parity Good
                         * = 0:Parity Bad
                         */
  uint16 w_SvPrtyErr;   /* {GPS|BDS} the 10 LSB to indicate parity errors in the 10 words of current subframe
                         *     {GLO} boolean type
                         * (bit0 for word 1, ... bit9 for word 10)
                         * = 1:error
                         * = 0:no error
                         */
} pdsm_GnssSvInfoType;

/* SV-specific data per GNSS for supplemental SV information
   Please note that SvMaskUsed & SvMaskUsable bitmask is redundant information, it can be generated
   by going through the SvInfo array and checking the q_SvInfoMask. It's kept here because of
   backward compatiblity */
typedef struct
{
  uint8               u_SvInfoValid;      /* TRUE if all info in this struct valid, otherwise FALSE */

  uint8               u_GpsNumSvMeas;     /* Number of GPS SVs with valid measurement (detected by searcher) */
  uint8               u_GpsNumSvPosFix;   /* Number of GPS SVs used in Position Fix */
  uint8               u_GpsNumSvVelFix;   /* Number of GPS SVs used in Velocity Fix */
  uint32              q_GpsSvMaskUsed;    /* Bitmask for GPS SVs used for calculating position fix */ 
  uint32              q_GpsSvMaskUsable;  /* Bitmask for GPS SVs usable for calculating position fix */
  pdsm_GnssSvInfoType z_GpsSvInfo[PDSM_N_ACTIVE_GPS_CHAN]; /* SV-specific data for GPS supplemental SV information */

  uint8               u_GloNumSvMeas;     /* Number of GLO SVs with valid measurement (detected by searcher) */
  uint8               u_GloNumSvPosFix;   /* Number of GLO SVs used in Position Fix */
  uint8               u_GloNumSvVelFix;   /* Number of GLO SVs used in Velocity Fix */
  uint32              q_GloSvMaskUsed;    /* Bitmask for GLONASS SVs used for calculating position fix */
  uint32              q_GloSvMaskUsable;  /* Bitmask for GLONASS SVs useable for calculating position fix */
  pdsm_GnssSvInfoType z_GloSvInfo[PDSM_N_ACTIVE_GLO_CHAN]; /* SV-specific data for GLO supplemental SV information */

  uint8               u_BdsNumSvMeas;     /* Number of BDS SVs with valid measurement (detected by searcher) */
  uint8               u_BdsNumSvPosFix;   /* Number of BDS SVs used in Position Fix */
  uint8               u_BdsNumSvVelFix;   /* Number of BDS SVs used in Velocity Fix */
  uint64              t_BdsSvMaskUsed;    /* Bitmask for BDS SVs used for calculating position fix */ 
  uint64              t_BdsSvMaskUsable;  /* Bitmask for BDS SVs usable for calculating position fix */
  pdsm_GnssSvInfoType z_BdsSvInfo[PDSM_N_ACTIVE_BDS_CHAN]; /* SV-specific data for BDS supplemental SV information */

  uint8               u_GalNumSvMeas;     /* Number of GAL SVs with valid measurement (detected by searcher) */
  uint8               u_GalNumSvPosFix;   /* Number of GAL SVs used in Position Fix */
  uint8               u_GalNumSvVelFix;   /* Number of GAL SVs used in Velocity Fix */
  uint64              t_GalSvMaskUsed;    /* Bitmask for GAL SVs used for calculating position fix */ 
  uint64              t_GalSvMaskUsable;  /* Bitmask for GAL SVs usable for calculating position fix */
  pdsm_GnssSvInfoType z_GalSvInfo[PDSM_N_ACTIVE_GAL_CHAN]; /* SV-specific data for GAL supplemental SV information */

  uint8               u_QzssNumSvMeas;     /* Number of QZSS SVs with valid measurement (detected by searcher) */
  uint8               u_QzssNumSvPosFix;   /* Number of QZSS SVs used in Position Fix */
  uint8               u_QzssNumSvVelFix;   /* Number of QZSS SVs used in Velocity Fix */
  uint8               u_QzssSvMaskUsed;    /* Bitmask for QZSS SVs used for calculating position fix */ 
  uint8               u_QzssSvMaskUsable;  /* Bitmask for QZSS SVs usable for calculating position fix */
  pdsm_GnssSvInfoType z_QzssSvInfo[PDSM_N_ACTIVE_QZSS_CHAN]; /* SV-specific data for GPS supplemental SV information */
} pdsm_GnssNavSvInfo;


/* Sensor axes validity definitions */
#define SENSOR_BIAS_VALID_XAXIS   (1<<0) /* TRUE -> X-axis bias is valid */
#define SENSOR_BIAS_VALID_YAXIS   (1<<1) /* TRUE -> Y-axis bias is valid */
#define SENSOR_BIAS_VALID_ZAXIS   (1<<2) /* TRUE -> Z-axis bias is valid */


/**
 * Structure containing Bias information for an individual sensor
 **/
typedef struct 
{
  uint16  w_SensorValidMask;/* Use SENSOR_BIAS_VALID_xxx macro to find which of the axes have valid data */
  float f_XaxisValue;       /*  X-axis Bias
                            - Units gyroscope:(radians)/(seconds)
                            - Units accelerometer:(meters)/(seconds^2)*/
  float f_YaxisValue;       /*  Y-axis Bias
                            - Units gyroscope:(radians)/(seconds)
                            - Units accelerometer:(meters)/(seconds^2)*/
  float f_ZaxisValue;       /*  Z-axis Bias
                            - Units gyroscope:(radians)/(seconds)
                            - Units accelerometer:(meters)/(seconds^2)*/   
} pdsm_pd_sensor_bias_data;


/**
 * On-device Sensor Bias data
 */
typedef struct 
{
  uint16                    w_SeqenceNumber;/* Incremental packet serial number (starting from 0), to identify missing packets. */
  pdsm_pd_sensor_bias_data  z_GyroBias;     /* Contains axis validity mask + x,y,z values for Gyro bias */
  pdsm_pd_sensor_bias_data  z_AccelBias;    /* Contains axis validity mask + x,y,z values for accel bias */
} pdsm_pd_sensor_param_report;

typedef struct pdsm_pd_data {
    int32                    lat; /* location lattitude. */
                                  /* Valid if ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set*/
    int32                    lon; /* location longitude. */
                                  /* Valid if ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set */
    uint32                    time_stamp; /* time stamp for the position fix (seconds).*/
                                         /* Valid if ext_opt_field_mask PDSM_PD_GPS_TIME_VALID is set */
    byte                     loc_uncertainty_ang; /* loc unc angle */
                                                  /* Valid if opt_field_mask PDSM_PD_UNCERTAINTY_VALID is set */
    byte                     loc_uncertainty_a;   /* loc unc along. This field has a limited resolution and should
                                                     not be used. Use alongAxisUnc in ext_pos_data structure */
                                                  /* Valid if opt_field_mask PDSM_PD_UNCERTAINTY_VALID is set */
    byte                     loc_uncertainty_p;   /* loc unc perp. This field has a limited resolution and should
                                                     not be used. Use perpAxisUnc in ext_pos_data structure  */
                                                  /* Valid if opt_field_mask PDSM_PD_UNCERTAINTY_VALID is set */

    pdsm_pd_opt_field        opt_field_mask;       /* Optional fields are valid */
    int16                    altitude;             /* altitiude. Valid if opt_field_mask PDSM_PD_ALTITUDE_VALID is set */ 
    word                     heading;              /* heading */  
                                                   /* Valid if ext_opt_field_mask PDSM_PD_HEADING_VALID is set */
    word                     velocity_hor;         /* horizontal velocity */
                                                   /* Valid if opt_field_mask PDSM_PD_VELOCITY_VALID is set */
    boolean                  fix_type;             /* fix type, TRUE if 3D */
    int8                     velocity_ver;         /* vertical velocity */
                                                   /* Valid if opt_field_mask PDSM_PD_VELOCITY_VALID is set */
    byte                     loc_uncertainty_v; /* loc uncertainity vertical */
                                                   /* Valid if opt_field_mask PDSM_PD_ALTITUDE_VALID is set */
    pdsm_pd_meas_debug_type  *meas_data;
    /*~ FIELD pdsm_pd_data.meas_data POINTER*/ /* no longer used */ 
    pm_scu_location_resp_type *pm_data;
    /*~ FIELD pdsm_pd_data.pm_data POINTER */ 
    int16                     time_stamp_msec;  /* msec time stamp */
                                                /* Valid if opt_field_mask PDSM_PD_MSEC_VALID is set */
    pdsm_pd_positioning_source_type positioning_source; /*gps/cellid/default */
    pdsm_pd_gps_position_e_type position_type; /* new/invalid */
    int8                      gpsUtcOffset; /* Time utc offset */ 
                                            /* Valid if PDSM_PD_UTCOFFSET_VALID is set */
    byte                     loc_uncertainty_conf; /* confidence of the loc unc */
                                            /* Valid if opt_field_mask PDSM_PD_UNCERTAINTY_VALID is set */
    pdsm_pd_position_mode_e_type      position_mode; /* MS BASED/MSASSISTED etc */
    pdsm_pd_ext_pos_data_s_type           ext_pos_data; /* extended position report */    
                                                        /* Valid if opt_field_mask PDSM_PD_EXT_STATUS_VALID is set */
    pdsm_pd_opt_field_ext     ext_opt_field_mask; /* validity bits for more fields in the struct */
    uint16 w_SensorAidingIndicator;  /* This is a bit mask. A bit set to to 1
                                     indicates that corresponding field 
                                     (position, velocity, heading, etc) in 
                                     fix report was aided with sensor data.
                                     0x0001 – Heading aided with sensor data
                                     0x0002 – Speed aided with sensor data
                                     0x0004 – Position aided with sensor data
                                     0x0008 – Velocity aided with sensor data
                                     0FFF0 – Reserved
                                     Future versions of the GPS service may use 
                                     reserved bits to represent new methods of 
                                     sensor aiding (e.g. tightly coupled 
                                     measurement combinations). A PDAPI client
                                     should ignore any bits that it considers 
                                     to be reserved. */

    uint16 w_SensorDataUsageMask;    /* This is a bit mask.
                                     A bit set to 1 indicates that data from
                                     the corresponding sensor was used in order
                                     to compute the content(position, velocity, 
                                     heading) in the fix report.
                                     0x0001 – Accelerometer
                                     0x0002 – Gyro
                                     0xFFFC - Reserved
                                     Future versions of the GPS service may use
                                     reserved bits to represent new sensor 
                                     types. A PDAPI client should ignore any 
                                     bits that it considers to be reserved. */

    pdsm_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
    pdsm_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/

    pdsm_GnssNavSvInfo z_NavSvInfo;       /* Extra SV info valid if zNavSvInfo.u_SvInfoValid is TRUE */

    pdsm_pd_sensor_param_report   z_OnDeviceSensorParam;  /* On-device sensor bias data */
} pdsm_pd_data_type;


typedef enum {
  PDSM_PD_DLOAD_EPHEMERIS = 0x01,
  PDSM_PD_DLOAD_ALMANAC   = 0x02,
  PDSM_PD_DLOAD_LOCATION  = 0x04,
  PDSM_PD_DLOAD_AA        = 0x08,
  PDSM_PD_DLOAD_SA        = 0x10
} pdsm_pd_dload_e_type;

typedef enum{
  PDSM_PD_DLOAD_SOURCE_MIN=-1,
  PDSM_PD_DLOAD_SOURCE_MS,
  PDSM_PD_DLOAD_SOURCE_PDE,
  PDSM_PD_DLOAD_SOURCE_STANDALONE,
  PDSM_PD_DLOAD_SOURCE_MAX
} pdsm_pd_dload_source_e_type;

typedef struct {
  pdsm_pd_dload_e_type        data;
  pdsm_pd_dload_source_e_type source;
} pdsm_pd_dload_info_s_type;

typedef struct {
  uint16    year;
  uint16    month;
  uint16    day;
  uint16    hour;
  uint16    minute;
  uint16    second;
  uint16    day_of_week;
} pdsm_pd_time_julian_type;

typedef struct{
  boolean              run;        /* Flag to indicate the dload progress 
                                      (TRUE: execute, FALSE: not executed yet)*/
  pdsm_pd_time_julian_type      timestamp;  /* Time when data dload starts to execute 
                                      (Only valid when run = TRUE */
  boolean              msa_seed;   /* Dload MS-A seed position */
  pdsm_pd_dload_e_type data_type;  /* Bit mask for each data dload info
                                      (Only valid when run = TRUE */
} pdsm_pd_jgps_msdload_info_s_type; 

typedef struct {
  byte                       num_data;
  pdsm_pd_dload_info_s_type  dload_info[PDSM_MAX_DLOAD_DATA];
  /* KDDI specific info */
  pdsm_pd_jgps_msdload_info_s_type jgps_msdload_info;
} pdsm_pd_dload_data_s_type;

typedef enum pdsm_pd_clk_src_e 
{
  PDSM_PD_CLK_SRC_HOST,
  PDSM_PD_CLK_SRC_NETWORK
} pdsm_pd_clk_src_e_type;
typedef struct pdsm_pd_external_time_info 
{
  uint64  time_msec;       /* if src is host, then this field is "relative" not abs time. 
                 if src is network, then this is msec since 00:00:00 Jan 6., 1980 */
  uint32  time_unc_msec;    /* Assoc. time uncertainty */
  pdsm_pd_clk_src_e_type time_source; /* time soruce of the slow clock */
  boolean time_is_gps; /* TRUE if time base is GPS, FALSE if time base is UTC . valid only if src is Network */
  uint8 discontinuity_count;   /* if source is Host, then signal any time discontinuties with this flag */
  boolean force_flag;      /* Force acceptance of data */
} pdsm_pd_external_time_info_type;

/* start Coase Position Injection related */

typedef uint32 pdsm_pd_extern_coarse_pos_opt_field;
#define PDSM_PD_EXTERN_COARSE_POS_GPS_TIME_VALID 0x0001
#define PDSM_PD_EXTERN_COARSE_POS_LAT_LONG_VALID 0x0002
#define PDSM_PD_EXTERN_COARSE_POS_ALT_VALID      0x0004
#define PDSM_PD_EXTERN_COARSE_POS_ELLIPTICAL_UNC_VALID 0x0008
#define PDSM_PD_EXTERN_COARSE_POS_CIRCULAR_UNC_VALID   0x0010
#define PDSM_PD_EXTERN_COARSE_POS_HORIZ_CONF_VALID 0x0020
#define PDSM_PD_EXTERN_COARSE_POS_RAW_CIRCULAR_UNC_VALID 0x0040
#define PDSM_PD_EXTERN_COARSE_POS_RAW_HORIZ_CONF_VALID 0x0080



typedef enum
{
  PDSM_PD_EXTERN_COARSE_POS_SOURCE_UNKNOWN,
  PDSM_PD_EXTERN_COARSE_POS_SOURCE_GPS,
  PDSM_PD_EXTERN_COARSE_POS_SOURCE_CELLID,
  PDSM_PD_EXTERN_COARSE_POS_SOURCE_ENH_CELLID, 
  PDSM_PD_EXTERN_COARSE_POS_SOURCE_WIFI,
  PDSM_PD_EXTERN_COARSE_POS_SOURCE_TERRESTRIAL,
  PDSM_PD_EXTERN_COARSE_POS_SOURCE_GPS_TERRESTRIAL_HYBRID,
  PDSM_PD_EXTERN_COARSE_POS_SOURCE_OTHER,
  PDSM_PD_EXTERN_COARSE_POS_SOURCE_MAX = 0x10000000  /* force enum to 32 bit */
}pdsm_pd_extern_coarse_pos_source_e_type; 

typedef enum
{
  PDSM_PD_EXTERN_COARSE_POS_TIME_GPS,
  PDSM_PD_EXTERN_COARSE_POS_TIME_UTC,
  PDSM_PD_EXTERN_COARSE_POS_TIME_AGE,
  PDSM_PD_EXTERN_COARSE_POS_TIME_MAX = 0x10000000  /* force enum to 32 bit */
}pdsm_pd_extern_coarse_pos_time_stamp_e_type; 

typedef enum
{
  PDSM_PD_EXTERN_COARSE_POS_ALT_HAE,  /* height above ellipsoid */
  PDSM_PD_EXTERN_COARSE_POS_ALT_MSL,  /* height above mean sea level */
  PDSM_PD_EXTERN_COARSE_POS_ALT_MAX = 0x10000000  /* force enum to 32 bit */ 
}pdsm_pd_extern_coarse_pos_alt_def_e_type; /* definition of altitude */ 

typedef struct pdsm_pd_horizontal_unc_elliptical
{
  float semimajor; /* semi-major axis length of elliptical horizontal uncertainty */
  float semiminor; /* semi-minor axis length*/
  float semimajor_azimuth; /* angle formed from the north */
} pdsm_pd_horizontal_unc_elliptical_type;

typedef struct pdsm_pd_external_pos_info
{
  /* indicating the presence/absence of the optional fields */
  pdsm_pd_extern_coarse_pos_opt_field          opt_field_mask; 

  /* info. of the time associated with the position. */
  pdsm_pd_extern_coarse_pos_time_stamp_e_type  time_info_type;
  uint32                                       timestamp_sec; /** time of week 
                                                                  or age, in 
                                                                  seconds. */

  pdsm_pd_extern_coarse_pos_alt_def_e_type alt_def; /* def. of altitude */

  pdsm_pd_extern_coarse_pos_source_e_type   source;

  int32   latitude;  /* Lat.: rad. LSB = pi/(2^32), 2's comp. */
  int32   longitude; /* Long.: rad, LSB = pi /(2^31), 2's comp. */ 

  /* horizontal unc: circular shape, radial */
  uint32  loc_unc_hor;   /* horizontal location unc.: meters */

  int16   altitude; /* alt: meters, positive height, negative depth  */ 

  /* vertical unc */
  uint16   loc_unc_vert;   /* loc unc vertical: meters */

  /* confidence horizontal: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_hor; 

  /* confidence vertical: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_vert; 
 
  float loc_unc_hor_raw; /*Non optimized Horizontal uncertainity*/

  /* Raw confidence horizontal: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_hor_raw; /*Non optimized Horizontal Confidence*/

}pdsm_pd_external_pos_info_type;   

typedef struct pdsm_pd_external_pos_info_ex
{
  pdsm_pd_external_pos_info_type pos_info;
  pdsm_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/
  
  /* source of altitude in injected position */
  pdsm_pd_altitude_source_e_type             e_AltitudeSource;
  /* linkage / dependence between injected horizontal position and altitude */
  pdsm_pd_altitude_horizontal_linkage_e_type e_AltHorLinkage;
  /* uncertainty coverage of the injected altitude */
  pdsm_pd_altitude_unc_coverage_e_type       e_AltUncCoverage;

 } pdsm_pd_external_pos_info_ex_type;


/* Conversion factor for lat/long rad. <-> degree */
#define PDSM_COARSE_LAT_SCALE_FACTOR  23860929.4222   // 2^32/180
#define PDSM_COARSE_LON_SCALE_FACTOR  11930464.7111   // 2^31/180

#define PDSM_COARSE_LAT_RAD_TO_DEG_SCALE_FACTOR  (1.0/PDSM_COARSE_LAT_SCALE_FACTOR)
#define PDSM_COARSE_LON_RAD_TO_DEG_SCALE_FACTOR  (1.0/PDSM_COARSE_LON_SCALE_FACTOR)

typedef struct pdsm_pd_external_position_info
{
  /* indicating the presence/absence of the optional fields */
  pdsm_pd_extern_coarse_pos_opt_field          opt_field_mask; 

  /* info. of the time associated with the position. */
  pdsm_pd_extern_coarse_pos_time_stamp_e_type  time_info_type;
  uint32                                       timestamp_sec; /** time of week 
                                                                  or age, in 
                                                                  seconds. */

  pdsm_pd_extern_coarse_pos_alt_def_e_type alt_def; /* def. of altitude */

  pdsm_pd_extern_coarse_pos_source_e_type   source;

  double   latitude;  /* Lat.: in degrees -90.0 to 90.0, positive for northern */
  double   longitude; /* Long.: in degrees -180.0 to 180.0, positive for eastern */ 

  /* horizontal unc: circular shape, radial */
  float  loc_unc_hor;   /* horizontal location unc.: meters */

  /* horizontal unc: circular shape, radial */
  float  loc_unc_hor_raw;   /*Non optimized  horizontal location unc.: meters */

  float   altitude; /* alt: meters, positive height, negative depth  */ 

  /* vertical unc */
  float   loc_unc_vert;   /* loc unc vertical: meters */

  /* confidence horizontal: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_hor; 

  /* Non optimized confidence horizontal: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_hor_raw; 

  /* confidence vertical: percentage, as defined by  ETSI TS 101 109. 
     0 - no info. to be rejected; 100-127 not used; if 100 is
     received, re-interpret to 99 */
  uint8    confidence_vert; 
 
  pdsm_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/
  
  /* source of altitude in injected position */
  pdsm_pd_altitude_source_e_type             e_AltitudeSource;
  /* linkage / dependence between injected horizontal position and altitude */
  pdsm_pd_altitude_horizontal_linkage_e_type e_AltHorLinkage;
  /* uncertainty coverage of the injected altitude */
  pdsm_pd_altitude_unc_coverage_e_type       e_AltUncCoverage;

  /* Elliptical CPI info */
  pdsm_pd_horizontal_unc_elliptical_type     horizontal_unc_elliptical;

 } pdsm_pd_external_position_info_type;
/* end Coarse Position Injection related */ 


/* Position Determination Parameter event related. Do we need to combine the events together.
 *  
 * Caution: Don't add any new PA event bitmask. Use 'pdsm_pa_e_type' to define PA events as enums.
*/
typedef int pdsm_pa_event_type;

#define PDSM_PA_EVENT_NONE                        0x00       /* No PA event */
#define PDSM_PA_EVENT_LPP_CONFIG_TYPE             0x02
#define PDSM_PA_EVENT_PE_CONFIG_PARAM             0x04       /* PE config parameters */
#define PDSM_PA_EVENT_SUPL_SECURITY               0x08       /* SUPL security */
#define PDSM_PA_EVENT_SUPL_VERSION                0x10       /* SUPL version */
#define PDSM_PA_EVENT_APP_INFO                    0x20       /* APP INFO */
#define PDSM_PA_EVENT_GPS_LOCK                    0x40       /* GPS lock change */
#define PDSM_PA_EVENT_KEEP_WARM                   0x0080     /* Keep Warm Status */
#define PDSM_PA_EVENT_DELETE_PARAMS               0x0100     /* Delete parameters */
#define PDSM_PA_EVENT_POSITION_MODE               0x0200     /* Position Mode supported */
#define PDSM_PA_EVENT_MT_LR_SUPPORT               0x0400     /* MT LR Supported */
#define PDSM_PA_EVENT_MO_METHOD                   0x0800     /* MO Method to be used */
#define PDSM_PA_EVENT_EXTERNAL_SPI                0x1000     /* external SPI mode */
#define PDSM_PA_EVENT_NMEA_SENTENCE_TYPE          0x2000     /* NMEA Sentence type to be enabled */
#define PDSM_PA_EVENT_1X_PDE_NETWORK_ADDRESS      0x4000     /* Change in IP address */
#define PDSM_PA_EVENT_1X_MPC_NETWORK_ADDRESS      0x8000     /* Change in IP address */
#define PDSM_PA_EVENT_UMTS_PDE_NETWORK_ADDRESS    0x10000    /* Change in IP address */
#define PDSM_PA_EVENT_SSD_INFO                    0x20000    /* SSD information */
#define PDSM_PA_EVENT_SEC_UPDATE_RATE             0x40000    /* Security Update Rate */
#define PDSM_PA_EVENT_PDAPI_ENABLE                0x80000    /* PDAPI Enable */
#define PDSM_PA_EVENT_NMEA_CONFIG_INFO            0x100000   /* NMEA configuration info */
#define PDSM_PA_EVENT_EFS_DATA                    0x200000   /* Write data to EFS - initially for SUPL cert */
#define PDSM_PA_EVENT_SBAS_STATUS                 0x400000   /* Get SBAS status */
#define PDSM_PA_EVENT_SBAS_USER_PREFERENCE        0x800000   /* Enable/disable SBAS user preference */
#define PDSM_PA_EVENT_DPO_CONTROL                 0x1000000  /* DPO Control */
#define PDSM_PA_EVENT_ON_DEMAND_LPM               0x2000000  /* ON_DEMAND Low Power mode */
#define PDSM_PA_EVENT_ON_DEMAND_READY_MODE        0x4000000  /* ON_DEMAND READY mode */
#define PDSM_PA_EVENT_PA_CALL_FLOW_SELECT         0x8000000  /* Call Flow Select */
#define PDSM_PA_EVENT_XTRAT_CLIENT_TOKEN          0x10000000 /*XTRAT client token*/
#define PDSM_PA_EVENT_XTRAT_SESSION_CONTROL       0x20000000 /*XTRAT session control*/
#define PDSM_PA_EVENT_LBS_APN_PROFILE_SELECT      0x40000000 /*LSB APN profile select*/
#define PDSM_PA_EVENT_XTRA_APN_PROFILE_SELECT     0x80000000 /*XTRA APN profile select*/


#define PDSM_PA_EVENT_MASK  ((PDSM_PA_EVENT_APP_INFO) \
                             | (PDSM_PA_EVENT_GPS_LOCK)\
                             | (PDSM_PA_EVENT_KEEP_WARM) \
                             | (PDSM_PA_EVENT_DELETE_PARAMS) \
                             | (PDSM_PA_EVENT_POSITION_MODE) \
                             | (PDSM_PA_EVENT_MT_LR_SUPPORT) \
                             | (PDSM_PA_EVENT_MO_METHOD) \
                             | (PDSM_PA_EVENT_EXTERNAL_SPI) \
                             | (PDSM_PA_EVENT_NMEA_SENTENCE_TYPE) \
                             | (PDSM_PA_EVENT_1X_PDE_NETWORK_ADDRESS)\
                             | (PDSM_PA_EVENT_1X_MPC_NETWORK_ADDRESS) \
                             | (PDSM_PA_EVENT_UMTS_PDE_NETWORK_ADDRESS) \
                             | (PDSM_PA_EVENT_SSD_INFO) \
                             | (PDSM_PA_EVENT_SEC_UPDATE_RATE) \
                             | (PDSM_PA_EVENT_PDAPI_ENABLE) \
                             | (PDSM_PA_EVENT_NMEA_CONFIG_INFO) \
                             | (PDSM_PA_EVENT_EFS_DATA) \
                             | (PDSM_PA_EVENT_SBAS_STATUS) \
                             | (PDSM_PA_EVENT_SBAS_USER_PREFERENCE) \
                             | (PDSM_PA_EVENT_DPO_CONTROL) \
                             | (PDSM_PA_EVENT_ON_DEMAND_LPM) \
                             | (PDSM_PA_EVENT_ON_DEMAND_READY_MODE) \
                             | (PDSM_PA_EVENT_PA_CALL_FLOW_SELECT) \
                             | (PDSM_PA_EVENT_XTRAT_CLIENT_TOKEN) \
                             | (PDSM_PA_EVENT_XTRAT_SESSION_CONTROL) \
                             | (PDSM_PA_EVENT_LBS_APN_PROFILE_SELECT) \
                             | (PDSM_PA_EVENT_XTRA_APN_PROFILE_SELECT) \
                             | (PDSM_PA_EVENT_SUPL_VERSION) \
                             | (PDSM_PA_EVENT_SUPL_SECURITY) \
                             | (PDSM_PA_EVENT_PE_CONFIG_PARAM) \
                             | (PDSM_PA_EVENT_LPP_CONFIG_TYPE) \
                            )


typedef struct pdsm_pa_info_s {
    pdsm_client_id_type      client_id;   /* PDSM originating client */
    pdsm_pa_e_type           pa_event;
    pdsm_pa_info_type        pa_info;     /* Info based on Event set */
    qword                    time_stamp;  /* system time stamp for the event */
} pdsm_pa_info_s_type;
/*~ FIELD pdsm_pa_info_s.pa_info DISC pdsm_pa_info_s.pa_event */

/* Typedef structure used by user in providing PA set parameters to be set.
*/
typedef struct pdsm_pa_set_s {
    pdsm_pa_e_type     pa_set;            /* Parameter mask to be set */
    pdsm_pa_info_type  info;

} pdsm_pa_set_type;
/*~ FIELD pdsm_pa_set_s.info DISC pdsm_pa_set_s.pa_set */ 



/* ---------------------------- LCS EVENTS --------------------------------------------------*/



typedef int pdsm_lcs_event_type;
#define PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ  0x0010
#define PDSM_LCS_EVENT_SUPL_NOTIFY_VERIFY_REQ  0x0020
#define PDSM_LCS_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ  0x0040
#define PDSM_LCS_VX_EVENT_USER_NOTIFY_SERVICE_INTERACTION   0x0080
#define PDSM_LCS_EVENT_CLIENT_STATUS           0x0100
#define PDSM_LCS_EVENT_CLIENT_VX_CONFIG           0x0200
#define PDSM_LCS_EVENT_CLIENT_SET_VX_CONFIG       0x0400
#define PDSM_LCS_EVENT_MASK  ((PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ)  \
                              | (PDSM_LCS_EVENT_SUPL_NOTIFY_VERIFY_REQ)  \
                              | (PDSM_LCS_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ) \
                              | (PDSM_LCS_VX_EVENT_USER_NOTIFY_SERVICE_INTERACTION) \
                              | (PDSM_LCS_EVENT_CLIENT_STATUS) \
                              | (PDSM_LCS_EVENT_CLIENT_VX_CONFIG) \
                              | (PDSM_LCS_EVENT_CLIENT_SET_VX_CONFIG)) 


typedef enum pdsm_lcs_notify_verify_e
{
  PDSM_LCS_NO_NOTIFY_NO_VERIFY =0,
  PDSM_LCS_USER_NOTIFY_ONLY =1,
  PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP=2, //n/w goes ahead with call flow. ACCEPT.
  PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP=3, //n/w doesn't allow call flow. DENY
  PDSM_LCS_PRIVACY_NEEDED=4, /* Deprecated. Use PRIVACY_OVERRIDE*/
  PDSM_LCS_PRIVACY_OVERRIDE=5 // override any privacy
} pdsm_lcs_notify_verify_e_type;
  
typedef enum pdsm_lcs_supl_format_indicator_e
{
   PDSM_SUPL_FORMAT_LOGICAL_NAME = 0,
   PDSM_SUPL_FORMAT_EMAIL_ADDRESS = 1,
   PDSM_SUPL_FORMAT_MSISDN = 2,
   PDSM_SUPL_FORMAT_URL = 3,
   PDSM_SUPL_FORMAT_SIP_URL = 4,
   PDSM_SUPL_FORMAT_MIN = 5,
   PDSM_SUPL_FORMAT_MDN = 6,
   PDSM_SUPL_FORMAT_IMSPUBLIC_IDENTITY = 7,
   PDSM_SUPL_FORMAT_OSS_UNKNOWN = 2147483647,
} pdsm_lcs_supl_format_indicator_e_type;

typedef enum pdsm_lcs_notify_verify_datacoding_scheme_e 
{
    PDSM_PRESUPL_ISO646IRV = 0,
    PDSM_PRESUPL_ISO8859 = 1,
    PDSM_PRESUPL_UTF8 = 2,
    PDSM_PRESUPL_UTF16 = 3,
    PDSM_PRESUPL_UCS2 = 4,
    PDSM_PRESUPL_GSM_DEFAULT = 5,
    PDSM_PRESUPL_SHIFT_JIS = 6,
    PDSM_PRESUPL_JIS = 7,
    PDSM_PRESUPL_EUC = 8,
    PDSM_PRESUPL_GB2312 = 9,
    PDSM_PRESUPL_CNS11643 = 10,
    PDSM_PRESUPL_KSC1001 = 11,
    PDSM_PRESUPL_NOTIF_DISPLAYENCODINGTYPE_UNKNOWN_ENUMERATOR = 2147483647,

    PDSM_SS_GERMAN = 12,
    PDSM_SS_ENGLISH = 13,
    PDSM_SS_ITALIAN = 14,
    PDSM_SS_FRENCH = 15,
    PDSM_SS_SPANISH = 16,
    PDSM_SS_DUTCH = 17,
    PDSM_SS_SWEDISH = 18,
    PDSM_SS_DANISH = 19,
    PDSM_SS_PORTUGUESE = 20,
    PDSM_SS_FINNISH = 21,
    PDSM_SS_NORWEGIAN = 22,
    PDSM_SS_GREEK = 23,
    PDSM_SS_TURKISH = 24,
    PDSM_SS_HUNGARIAN = 25,
    PDSM_SS_POLISH = 26,
    PDSM_SS_LANGUAGE_UNSPEC = 27,

    PDSM_SUPL_UTF8 = 28,
    PDSM_SUPL_UCS2 = 29,
    PDSM_SUPL_GSM_DEFAULT = 30,
    PDSM_SUPL_NOTIF_DISPLAYENCODINGTYPE_UNKNOWN_ENUMERATOR = 2147483647,

} pdsm_lcs_notify_verify_datacoding_scheme_e_type;

typedef enum pdsm_lcs_requester_id_encoding_scheme_e 
{
    PDSM_VX_OCTET = 0,
    PDSM_VX_EXN_PROTOCOL_MSG = 1,
    PDSM_VX_ASCII = 2,
    PDSM_VX_IA5 = 3,
    PDSM_VX_UNICODE = 4,
    PDSM_VX_SHIFT_JIS = 5,
    PDSM_VX_KOREAN = 6,
    PDSM_VX_LATIN_HEBREW = 7,
    PDSM_VX_LATIN = 8,
    PDSM_VX_GSM = 9,
    PDSM_VX_ENCODING_TYPE_MAX  
} pdsm_lcs_requester_id_encoding_scheme_e_type;
typedef enum
{
   PDSM_LOCATIONTYPE_CURRENT_LOCATION,
   PDSM_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION,
   PDSM_LOCATIONTYPE_INITIAL_LOCATION
} pdsm_lcs_location_type_e_type;

typedef enum pos_tech_indicator_e
{
   PDSM_LCS_POS_TECH_S_SHOT,
   PDSM_LCS_POS_TECH_TRACK,
   PDSM_LCS_POS_TECH_CELL_S_SHOT,
   PDSM_LCS_POS_TECH_CELL_TRACK,
   PDSM_LCS_POS_TECH_CACHED,
   PDSM_LCS_POS_TECH_MAX
} pdsm_lcs_pos_tech_indicator_e_type;

typedef enum pdsm_lcs_pos_mode_e
{
   PDSM_LCS_MS_ASSISTED_ONLY = 0,
   PDSM_LCS_MS_BASED_ONLY = 1,
   PDSM_LCS_MS_ASSISTED_PREF_MSBASED_ALLWD = 2,
   PDSM_LCS_MS_BASED_PREF_ASSISTED_ALLWD = 3,
   PDSM_LCS_POS_MODE_MAX
} pdsm_lcs_pos_mode_e_type;

/* this maps supl_PosMethod enum. */
typedef enum pdsm_lcs_pos_method_e 
{
  POSMETHOD_AGPS_SETASSISTED = 0,
  POSMETHOD_AGPS_SETBASED = 1,
  POSMETHOD_AGPS_SETASSISTED_PREF = 2,
  POSMETHOD_AGPS_SETBASED_PREF = 3,
  POSMETHOD_AUTONOMOUS_GPS = 4,
  POSMETHOD_AFLT = 5,
  POSMETHOD_ECID = 6,
  POSMETHOD_EOTD = 7,
  POSMETHOD_OTDOA = 8,
  POSMETHOD_NO_POSITION = 9
} pdsm_lcs_pos_method_e_type;  

typedef struct pdsm_lcs_reporting_criteria 
{
  boolean periodic; /* set to true if this is a periodic request, false otherwise */
  uint32  reportingInterval;
  uint32  duration;
} pdsm_lcs_reporting_criteria_s_type;

typedef struct pdsm_lcs_requester_id_s
{
  byte requester_id_length;
  byte requester_id[200];
} pdsm_lcs_requester_id_s_type;

typedef struct pdsm_lcs_v1_notify_verify_req_s {
  pdsm_lcs_notify_verify_e_type notification_priv_type;
  pdsm_lcs_pos_tech_indicator_e_type pos_tech;
  char *requester_id;   //Requester Identifier V1
} pdsm_lcs_v1_notify_verify_req_s_type;

typedef struct pdsm_lcs_vx_notify_verify_req_s {
  pdsm_lcs_notify_verify_e_type notification_priv_type;
/* todo : Remove pos_tech */
  pdsm_lcs_pos_tech_indicator_e_type pos_tech; 
  boolean pos_qos_incl;
  uint8 pos_qos;
  dword num_fixes;
  dword tbf;
  pdsm_lcs_pos_mode_e_type pos_mode;
  pdsm_lcs_requester_id_encoding_scheme_e_type encoding_scheme;
  pdsm_lcs_requester_id_s_type requester_id;
  uint16 user_resp_timer_val; // in seconds
} pdsm_lcs_vx_notify_verify_req_s_type;

typedef struct 
{
  uint8  notification_length;
  char *notification_text;     /* Notification Text valid only if the flags indicate notification text present */
  /*~ FIELD  pdsm_lcs_notification_text_s_type.notification_text VARRAY LENGTH pdsm_lcs_notification_text_s_type.notification_length */
  pdsm_lcs_notify_verify_datacoding_scheme_e_type datacoding_scheme; /* Type of Data encoding scheme for the text */
} pdsm_lcs_notification_text_s_type;

#define PDSM_LCS_MAX_EXT_CLIENT_ADDRESS 20
typedef struct
{
  byte ext_client_address_len; /* External Client Address Length */
  byte ext_client_address[PDSM_LCS_MAX_EXT_CLIENT_ADDRESS]; /* This field is valid only the flags indicate ext client id pres. Supported only for CP */ 
  /*~ FIELD pdsm_lcs_ext_client_address_s_type.ext_client_address POINTER 
      VARRAY  PDSM_LCS_MAX_EXT_CLIENT_ADDRESS
      LENGTH pdsm_lcs_ext_client_address_s_type.ext_client_address_len
  */  
  
} pdsm_lcs_ext_client_address_s_type;

/* Defererred location */
typedef struct
{
   byte     unused_bits;
   byte     ms_available;  
} pdsm_lcs_deferred_location_s_type;

#define PDSM_LCS_REQUESTOR_ID_LEN 63
/* Data coding scheme */
typedef struct 
{
   byte         data_coding_scheme;
   uint8        requestor_id_string[PDSM_LCS_REQUESTOR_ID_LEN];  
   uint8        string_len;
  /*~ FIELD pdsm_lcs_requestor_id_s_type.requestor_id_string POINTER 
      VARRAY PDSM_LCS_REQUESTOR_ID_LEN 
      LENGTH pdsm_lcs_requestor_id_s_type.string_len
  */  
} pdsm_lcs_requestor_id_s_type;

#define PDSM_LCS_CLIENT_NAME_LEN 63
/* LCS client name */
typedef struct 
{
   byte         data_coding_scheme;
   uint8        client_name_string[PDSM_LCS_CLIENT_NAME_LEN];  
   uint8        string_len;
  /*~ FIELD pdsm_lcs_client_name_s_type.client_name_string POINTER 
      VARRAY PDSM_LCS_CLIENT_NAME_LEN 
      LENGTH pdsm_lcs_client_name_s_type.string_len
  */  
} pdsm_lcs_client_name_s_type;

#define PDSM_LCS_CODEWORD_LEN 20
/* LCS code word */
typedef struct 
{
   byte         data_coding_scheme;
   uint8        lcs_codeword_string[PDSM_LCS_CODEWORD_LEN]; 
   uint8        string_len;
  /*~ FIELD pdsm_lcs_codeword_string_s_type.lcs_codeword_string POINTER 
      VARRAY PDSM_LCS_CODEWORD_LEN 
      LENGTH pdsm_lcs_codeword_string_s_type.string_len
  */  
} pdsm_lcs_codeword_string_s_type;

/* LCS Service ID */
typedef struct
{
   byte         lcs_service_type_id;
} pdsm_lcs_service_type_ID_s_type;



typedef struct 
{
  pdsm_server_address_e_type choice;
  pdsm_server_address_u_type supl_slp_address;
} pdsm_supl_slp_address;
/*~ FIELD pdsm_supl_slp_address.supl_slp_address DISC pdsm_supl_slp_address.choice */

#define PDSM_SUPL_SLP_SESSION_ID_BYTE_LENGTH 4
typedef struct pdsm_supl_slp_session_id_s
{
  boolean presence; /* not really needed for pdapi, but to keep it consistent with the corresponding structure in SUPL */
  uint8 session_id[PDSM_SUPL_SLP_SESSION_ID_BYTE_LENGTH];   
  /*~ FIELD pdsm_supl_slp_session_id_s_type.session_id POINTER 
  VARRAY PDSM_SUPL_SLP_SESSION_ID_BYTE_LENGTH 
  LENGTH PDSM_SUPL_SLP_SESSION_ID_BYTE_LENGTH*/
  pdsm_supl_slp_address slp_address;
} pdsm_supl_slp_session_id_s_type;

#define PDSM_LCS_SUPL_QOP_VALID 0x01
#define PDSM_LCS_SUPL_QOP_VERACC_VALID 0x02
#define PDSM_LCS_SUPL_QOP_MAXAGE_VALID 0x04
#define PDSM_LCS_SUPL_QOP_DELAY_VALID 0x08
typedef struct pdsm_lcs_supl_qop
{
  uint8      bit_mask; /* bit mask indicating which fields are valid in this struct */
  uint8      horacc;
  uint8      veracc;  /* optional; set in bit_mask supl_veracc_present if present */
  uint16      maxLocAge;  /* optional; set in bit_mask supl_maxLocAge_present if present */
  uint8 delay;  /* optional; set in bit_mask supl_delay_present if  present */
} pdsm_lcs_supl_qop_s_type;


#define PDSM_SUPL_HASH_LEN 8

#define PDAPI_SUPLINIT_VER2_EXT_BITMAP_SUPTDNTWK_VALID     0x0001
#define PDAPI_SUPLINIT_VER2_EXT_BITMAP_TRIGTYPE_VALID      0x0002
#define PDAPI_SUPLINIT_VER2_EXT_BITMAP_GNSSPOSTECH_VALID   0x0004

#define PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_WLAN 0x0001
#define PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_GSM  0x0002
#define PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_WCDMA 0x0004
#define PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_CDMA 0x0008
#define PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_HRDP 0x0010
#define PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_UMB  0x0020
#define PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_LTE  0x0040
#define PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_WIMAX 0x0080
#define PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_HISTORIC  0x0100
#define PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_NONSVRV  0x0200

#define PDAPI_SUPLINIT_TRIGGER_TYPE_PERIODIC 0
#define PDAPI_SUPLINIT_TRIGGER_TYPE_AREA_EVENT 1 

#define PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_GPS      0x0001
#define PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_GALILEO  0x0002
#define PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_SBAS     0x0004
#define PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_MODERN_GPS 0x0008
#define PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_QZSS       0x0010
#define PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_GLONASS    0x0020


typedef struct  {

  uint32 validity; /* Bit0 = 1:  supportedNetworkInformation is valid (PDAPI_SUPLINIT_VER2_EXT_SUPTDNTWK_VALID)
                      Bit1 = 1:  triggerType is valid (PDAPI_SUPLINIT_VER2_EXT_TRIGTYPE_VALID)
                      Bit2 = 1:  gnssPosTechnology is valid (PDAPI_SUPLINIT_VER2_EXT_GNSSPOSTECH_VALID)
                      Bit 3 - 31: reserved */
                      
  uint32 supl_supported_ntwkinfo; /* see PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_ above */
  
  uint8  trigger_type;  /* see PDAPI_SUPLINIT_TRIGGER_TYPE_AREA_ above */
  
  uint32 gnss_pos_tech; /* see PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_ above */
  

}pdsm_lcs_supl_suplinit_ver2_ext_s_type;

typedef struct 
{
  boolean b_eslp_address_present;
  pdsm_supl_slp_address e_slp_address;
} pdsm_lcs_supl_suplinit_ver2_ext_emergency_s_type;

/* Notify_Verify_optional_field_bit_map 
   ("flag" used in both 
   pdsm_lcs_supl_notify_verify_req_s_type and
   pdsm_lcs_umts_cp_notify_verify_req_s )
*/
#define  PDSM_LCS_CLIENT_NAME_PRESENT       0x0001  /* Both CP and SUPL */
#define  PDSM_LCS_CLIENT_EXTADDR_PRESENT    0x0002  /* CP only */
#define  PDSM_LCS_DEF_LOCATION_TYPE_PRESENT 0x0010  /* CP only */
#define  PDSM_LCS_REQUESTOR_ID_PRESENT      0x0020  /* Both CP and SUPL */
#define  PDSM_LCS_CODEWORD_PRESENT          0x0040  /* CP only */
#define  PDSM_LCS_SERVICE_TYPE_ID_PRESENT   0x0080  /* CP only */
#define  PDSM_LCS_ENCODING_TYPE_PRESENT     0x0100  /* Both CP and SUPL */
#define  PDSM_LCS_SUPLINIT_VER2_EXT_PRESENT 0x0200  /* SUPL only */
#define  PDSM_LCS_SUPLINIT_VER2_EXT_EMERGENCY_PRESENT 0x0400  /* SUPL only */

/* SUPL INIT Source Types */     
typedef enum
{
  PDSM_SUPL_INIT_SOURCE_TYPE_UNKNOWN = 0, /* Unknown (for initialized value) */
  PDSM_SUPL_INIT_SOURCE_TYPE_SMS_1X,      /* SUPL INIT came over 1X SMS */
  PDSM_SUPL_INIT_SOURCE_TYPE_SMS_GSM,     /* SUPL INIT came over GSM SMS */
  PDSM_SUPL_INIT_SOURCE_TYPE_WAP_PUSH,    /* SUPL INIT came over WAP-Push */
  PDSM_SUPL_INIT_SOURCE_TYPE_UDP          /* SUPL INIT came over UDP */
}pdsm_lcs_supl_init_src_e_type;


/* SUPL Notify verify structure */
typedef struct pdsm_lcs_supl_notify_verify_req_s {
  pdsm_lcs_notify_verify_e_type notification_priv_type; /* Notification Type */
  uint16 flags; /* optional field bitmap: see Notify_Verify_optional_field_bit_map definition above */ 
  pdsm_supl_slp_session_id_s_type   supl_slp_session_id; /* SLP Session ID */  
  uint8                             supl_hash[PDSM_SUPL_HASH_LEN]; /* HASH for SUPL_INIT */
  /*~ FIELD  pdsm_lcs_supl_notify_verify_req_s.supl_hash POINTER
    VARRAY PDSM_SUPL_HASH_LEN
    LENGTH PDSM_SUPL_HASH_LEN */
  pdsm_lcs_notify_verify_datacoding_scheme_e_type datacoding_scheme; /* Optional: Type of Data encoding scheme for the text */
  pdsm_lcs_pos_method_e_type  pos_method; /* The GPS Mode to be used for the fix. */
  pdsm_lcs_requestor_id_s_type requestor_id;      /* Optional: Requestor ID */
  pdsm_lcs_client_name_s_type client_name;        /* Optional: Client Name */
  pdsm_lcs_supl_qop_s_type supl_qop; /* supl qop */
  uint16  user_response_timer; /* Time before which a user has to respond. Not used currently */
  pdsm_lcs_supl_suplinit_ver2_ext_s_type  supl_init_ver2_ext; /* Optional: SUPL_INIT Ver2 extension */
  pdsm_lcs_supl_init_src_e_type  supl_init_source;

  pdsm_lcs_supl_suplinit_ver2_ext_emergency_s_type  supl_init_ver2_ext_emergency;
    /* Optional: If the flags indicate PDSM_LCS_SUPLINIT_VER2_EXT_EMERGENCY_PRESENT, then the GPS session is treated
       as an emergency session. Additional fields in this structure may be present for Emergency */
} pdsm_lcs_supl_notify_verify_req_s_type;

/* UMTS CP notify verify structure */
typedef struct pdsm_lcs_umts_cp_notify_verify_req_s {
  pdsm_lcs_notify_verify_e_type notification_priv_type; /* Notification Type */
  uint8 invoke_id; /* SS Invoke Id  */
  uint16 flags; /* optional field bitmap: see Notify_Verify_optional_field_bit_map definition above */ 
  uint8  notification_length;
  char *notification_text;     /* Notification Text valid only if the flags indicate notification text present */
  /*~ FIELD  pdsm_lcs_umts_cp_notify_verify_req_s.notification_text VARRAY LENGTH pdsm_lcs_umts_cp_notify_verify_req_s.notification_length */
  pdsm_lcs_notify_verify_datacoding_scheme_e_type datacoding_scheme; /* Type of Data encoding scheme for the text */
  pdsm_lcs_ext_client_address_s_type ext_client_address_data; /* Optional: client external address -- CP only */
  pdsm_lcs_location_type_e_type location_type; /* Location Type sent only for CP */
  pdsm_lcs_deferred_location_s_type deferred_location; /* Optional: Deferred Location - CP Only */
  pdsm_lcs_requestor_id_s_type requestor_id;      /* Optional: Requestor ID */
  pdsm_lcs_codeword_string_s_type codeword_string;   /* Optional Codeword String - CP Only */
  pdsm_lcs_service_type_ID_s_type service_type_id;   /* Optional: Service Type ID - CP Only */
  uint16  user_response_timer; /* Time before which a user has to respond. Not used currently */
} pdsm_lcs_umts_cp_notify_verify_req_s_type;

typedef enum {
  PDSM_LCS_AGENT_DISABLED = 0,                       /* V1/V2 disabled */
  PDSM_LCS_AGENT_VERSION_V1_ONLY,
  PDSM_LCS_AGENT_VERSION_V2_ONLY,
  PDSM_LCS_AGENT_VERSION_MAX
} pdsm_lcs_agent_version_e_type;


/* LCS Agent Query VX Config type */
typedef struct {
  pdsm_lcs_agent_version_e_type   vxVersion;
  boolean                         trustedApp;
} pdsm_lcs_agent_vx_config_s_type;


/* LCS Agent Set VX Config type (runtime change of NV 1993:VxVersion ) */
typedef struct {
  pdsm_lcs_agent_version_e_type   vxVersion;
} pdsm_lcs_agent_set_vx_config_s_type;

/* Notify verify response type */
typedef enum pdsm_lcs_agent_notification_resp_e 
{
  PDSM_LCS_NOTIFY_VERIFY_MIN = -1,
  PDSM_LCS_NOTIFY_VERIFY_ACCEPT = 0,
  PDSM_LCS_NOTIFY_VERIFY_DENY   =1,
  PDSM_LCS_NOTIFY_VERIFY_NORESP = 2,
  PDSM_LCS_NOTIFY_VERIFY_MAX
} pdsm_lcs_agent_notification_resp_e_type;

typedef enum pdsm_lcs_info_cmd_e 
{
   PDSM_LCS_CMD_TYPE_VX_REQ,
   PDSM_LCS_CMD_TYPE_SUPL_REQ,
   PDSM_LCS_CMD_TYPE_UMTS_CP_REQ,
   PDSM_LCS_CMD_TYPE_VX_SERVICE_INTERACTION,
   PDSM_LCS_CMD_TYPE_CLIENT_TYPE,
   PDSM_LCS_CMD_TYPE_VX_CONFIG,
   PDSM_LCS_CMD_TYPE_SET_VX_CONFIG,
   PDSM_LCS_CMD_TYPE_MAX
} pdsm_lcs_info_cmd_e_type ;
/*~ SENTINEL pdsm_lcs_info_cmd_e_type.PDSM_LCS_CMD_TYPE_MAX */


typedef union {
  pdsm_lcs_vx_notify_verify_req_s_type vx_req;           /* VX request */
  pdsm_lcs_supl_notify_verify_req_s_type supl_req;       /* SUPL request */  
  pdsm_lcs_umts_cp_notify_verify_req_s_type umts_cp_req; /* UMTS CP request */ 
  pdsm_client_type_e_type client_type;
  pdsm_lcs_agent_vx_config_s_type vx_config;             /* Query VX Config */
  pdsm_lcs_agent_set_vx_config_s_type set_vx_config;     /* Set VX Config */
} pdsm_lcs_info_u_type;
/*~ IF (_DISC_ == PDSM_LCS_CMD_TYPE_SUPL_REQ)     pdsm_lcs_info_u_type.supl_req */
/*~ IF (_DISC_ == PDSM_LCS_CMD_TYPE_VX_REQ) || (_DISC_ == PDSM_LCS_CMD_TYPE_VX_SERVICE_INTERACTION)  pdsm_lcs_info_u_type.vx_req */
/*~ IF (_DISC_ == PDSM_LCS_CMD_TYPE_UMTS_CP_REQ)  pdsm_lcs_info_u_type.umts_cp_req */
/*~ IF (_DISC_ == PDSM_LCS_CMD_TYPE_CLIENT_TYPE)  pdsm_lcs_info_u_type.client_type */
/*~ IF (_DISC_ == PDSM_LCS_CMD_TYPE_VX_CONFIG)  pdsm_lcs_info_u_type.vx_config */
/*~ IF (_DISC_ == PDSM_LCS_CMD_TYPE_SET_VX_CONFIG)  pdsm_lcs_info_u_type.set_vx_config */
/*~ DEFAULT pdsm_lcs_info_u_type.void */

/* When notified of PD events, clients are given
** a reference to this structure.
*/
typedef struct pdsm_lcs_info_s 
{
  pdsm_client_id_type client_id;               /* Client Id */
  pdsm_lcs_info_cmd_e_type lcs_cb_cmd_type;    /* LCS info cmd type */
  pdsm_lcs_info_u_type lcs_info;               /* LCS Info data */
  qword                time_stamp;             /* time stamp  */
} pdsm_lcs_info_s_type;
/*~ FIELD pdsm_lcs_info_s.lcs_info DISC pdsm_lcs_info_s.lcs_cb_cmd_type */

/* LCS Notify verify Response type */
typedef struct pdsm_lcs_agent_notification_resp_s
{
  pdsm_lcs_agent_notification_resp_e_type  resp;    /* Response type */
  pdsm_lcs_info_cmd_e_type  lcs_info_cmd_type;      /* Protocol type */
  pdsm_lcs_info_u_type lcs_info;                    /* Lcs info data */
} pdsm_lcs_notification_resp_s_type; 
/*~ FIELD pdsm_lcs_agent_notification_resp_s.lcs_info DISC pdsm_lcs_agent_notification_resp_s.lcs_info_cmd_type */

typedef struct pdsm_lcs_agent_client_init_s
{
  uint32 class_id; 
  pdsm_client_type_e_type client_type; 
  char* app_info_ptr;
  boolean is_brew_app;
} pdsm_lcs_agent_client_init_s_type;

typedef struct pdsm_lcs_agent_client_release_s
{
  uint32 class_id;
  pdsm_client_type_e_type client_type;
} pdsm_lcs_agent_client_release_s_type;

typedef union pdsm_lcs_cmd_info_u
{
  pdsm_lcs_notification_resp_s_type resp_info;
  pdsm_lcs_agent_client_init_s_type client_init_type;
  pdsm_lcs_agent_client_release_s_type client_release_type;
  uint32 client_type;
  pdsm_lcs_agent_set_vx_config_s_type set_vx_config;
} pdsm_lcs_cmd_info_u_type;
/*~ IF (_DISC_ == PDSM_LCS_CMD_NOTIFY_VERIFY_RESP))      pdsm_lcs_cmd_info_u_type.resp_info */ 
/*~ IF (_DISC_ == PDSM_LCS_CMD_CLIENT_INIT))      pdsm_lcs_cmd_info_u_type.client_init_type */ 
/*~ IF (_DISC_ == PDSM_LCS_CMD_CLIENT_RELEASE))      pdsm_lcs_cmd_info_u_type.release_type */ 
/*~ IF (_DISC_ == PDSM_LCS_CMD_QUERY_VX_CONFIG))      pdsm_lcs_cmd_info_u_type.client_type */ 
/*~ IF (_DISC_ == PDSM_LCS_CMD_SET_VX_CONFIG))      pdsm_lcs_cmd_info_u_type.set_vx_config */ 
/*~ DEFAULT pdsm_lcs_cmd_info_u_type.void */

typedef enum pdsm_ext_status_e
{
   PDSM_EXT_STATUS_NMEA_POS_REPORT,           /* NMEA DATA */
   PDSM_EXT_STATUS_POS_REPORT,                /* GPS position information */
   PDSM_EXT_STATUS_MEASUREMENT,               /* GPS measurement information */
   PDSM_EXT_STATUS_GPS_STATE_INFO,            /* GPS State information */
   PDSM_EXT_STATUS_UNIFIED_NMEA_POS_REPORT,   /* Deprecated */
   PDSM_EXT_STATUS_PERIODIC_PPM_REPORT,       /* Periodic PPM report */
   PDSM_EXT_STATUS_POS_INJECT_REQUEST,        /* External posn injected */
   PDSM_EXT_STATUS_GENERIC_EVENT,             /* Generic event */
   PDSM_EXT_STATUS_RESET_LOCATION_SERVICE,    /* Reset Location Service */
   PDSM_EXT_STATUS_DPO_STATUS,                 /* DPO engage status */
   PDSM_EXT_STATUS_NI_SESSION_REJECT_REPORT,    /*NI Call flow Reject Reason*/
   PDSM_EXT_STATUS_BEST_AVAIL_POS_REPORT,      /* Best available position report */
   PDSM_EXT_STATUS_EMERG_POS_SESSION_TYPE,    /* Indication of an session during an emergency call */
   PDSM_EXT_STATUS_SUPL_SESSION_END,          /* SUPL Session End Indication */
   PDSM_EXT_STATUS_AVAIL_WWAN_POS_REPORT,      /* Available WWAN position report */
   PDSM_EXT_STATUS_AVAIL_POS_REPORT,      /* Available  position report */
   PDSM_EXT_STATUS_GNSS_MEASUREMENT,          /* Raw measurement report */
   PDSM_EXT_STATUS_SV_POLY_REPORT,            /* SV Polynomial report */
} pdsm_ext_status_e_type;

typedef int pdsm_ext_status_event_type;

#define PDSM_EXT_STATUS_EVENT_NMEA_POS_REPORT         0x0001
#define PDSM_EXT_STATUS_EVENT_POS_REPORT              0x0002
#define PDSM_EXT_STATUS_EVENT_MEASUREMENT             0x0004
#define PDSM_EXT_STATUS_EVENT_GPS_STATE_INFO          0x0008
#define PDSM_EXT_STATUS_EVENT_POS_INJECT_REQ          0x0010
#define PDSM_EXT_STATUS_EVENT_UNIFIED_NMEA_POS_REPORT 0x0020
#define PDSM_EXT_STATUS_EVENT_PERIODIC_PPM_REPORT     0x0040
#define PDSM_EXT_STATUS_EVENT_RESET_LOCATION_SERVICE_DONE   0x0080
#define PDSM_EXT_STATUS_EVENT_BEST_AVAIL_POS_REPORT   0x0100
#define PDSM_EXT_STATUS_EVENT_DPO_STATUS_REPORT       0x0200
#define PDSM_EXT_STATUS_EVENT_NI_SESSION_REJECT_REPORT    0x0400
#define PDSM_EXT_STATUS_EVENT_GENERIC_IND             0x1000
#define PDSM_EXT_STATUS_EVENT_EMERG_POS_SESSION_TYPE  0x2000   /* indication of an session during an emergency call */
#define PDSM_EXT_STATUS_EVENT_SUPL_SESSION_END         0x4000
#define PDSM_EXT_STATUS_EVENT_AVAIL_WWAN_POS_REPORT       0x8000
#define PDSM_EXT_STATUS_EVENT_GNSS_MEASUREMENT         0x10000
#define PDSM_EXT_STATUS_EVENT_SV_POLY                  0x20000
#define PDSM_EXT_STATUS_EVENT_AVAIL_POS_REPORT       0x40000

#define PDSM_EXT_STATUS_EVENT_MASK ( (PDSM_EXT_STATUS_EVENT_NMEA_POS_REPORT)          | \
                                     (PDSM_EXT_STATUS_EVENT_POS_REPORT)               | \
                                     (PDSM_EXT_STATUS_EVENT_MEASUREMENT )             | \
                                     (PDSM_EXT_STATUS_EVENT_GPS_STATE_INFO )          | \
                                     (PDSM_EXT_STATUS_EVENT_GENERIC_IND)              | \
                                     (PDSM_EXT_STATUS_EVENT_UNIFIED_NMEA_POS_REPORT)  | \
                                     (PDSM_EXT_STATUS_EVENT_RESET_LOCATION_SERVICE_DONE) | \
                                     (PDSM_EXT_STATUS_EVENT_POS_INJECT_REQ)              | \
                                     (PDSM_EXT_STATUS_EVENT_BEST_AVAIL_POS_REPORT)    | \
                                     (PDSM_EXT_STATUS_EVENT_EMERG_POS_SESSION_TYPE)      | \
                                     (PDSM_EXT_STATUS_EVENT_SUPL_SESSION_END)            | \
                                     (PDSM_EXT_STATUS_EVENT_NI_SESSION_REJECT_REPORT)    | \
                                     (PDSM_EXT_STATUS_EVENT_AVAIL_WWAN_POS_REPORT)       | \
                                     (PDSM_EXT_STATUS_EVENT_GNSS_MEASUREMENT)            |\
                                     (PDSM_EXT_STATUS_EVENT_SV_POLY)					|\
                                      (PDSM_EXT_STATUS_EVENT_AVAIL_POS_REPORT))

#define PDSM_NMEA_DATA_LENGTH 200

typedef struct
{
  pdsm_pa_nmea_type nmea_type;
  uint16            nmea_length;
  byte              nmea_data[PDSM_NMEA_DATA_LENGTH];
} pdsm_pd_ext_nmea_s_type;

#define PDSM_UNIFIED_NMEA_DATA_LENGTH 512
typedef struct
{
  uint16            nmea_length;
  byte              nmea_data[PDSM_UNIFIED_NMEA_DATA_LENGTH];
} pdsm_pd_ext_unified_nmea_s_type;

/* Max Active Channels: GPS 16, GLO 14, BDS 16, GAL 16 less than 80 (defined in QMI-LOC) */
#define PDSM_EXT_MEAS_NUM_SVS 64


typedef enum
{
  PDSM_SV_STATE_IDLE,
  PDSM_SV_STATE_SEARCH,
  PDSM_SV_STATE_SEARCH_VERIFY,
  PDSM_SV_STATE_BIT_EDGE,
  PDSM_SV_STATE_TRACK,
  PDSM_SV_STATE_MAX
} pdsm_sv_state;


/* Typedef struct for GNSS channel report.
*/
typedef struct pdsm_cgps_chan_rpt {
    int32    ps_doppler;          /* Pseudodoppler */
    word     sv_code_ph_wh;       /* Satellite code phase, whole chips */
    word     sv_code_ph_fr;       /* Satellite code phase, fract chips */
    byte     mul_path_ind;        /* Multipath Indicator */
    int32    ps_range_rms_er;     /* Pseudorange RMS error */
} pdsm_cgps_chan_rpt_type;

#define PDSM_MEAS_CHAN_RPT_TYPE_VALID 0x0001

typedef struct 
{
  uint16           flags;     /* flags to indicate if the chan_rpt_type is valid */
  gnss_sv_id_type  sv_id;     /* SVId unique to each GNSS SV */

  uint16  c_no;               /* CNo. Units of 0.1 dB */
  int16   latency_ms;         /* Age of the measurement in msecs (+ve meas Meas precedes ref time) */
  int32   azimuth;            /* Azimuth (0 to 359 degrees) */
  int32   elevation;          /* Elevation (0-90 degrees) */

  pdsm_sv_state  sv_state;    /* SV Observation state */

  int8  sv_freq_num;          /* glonass SV frequency number */

  pdsm_cgps_chan_rpt_type chan_rpt_type; 

  float   azimuth_rad;        /* Azimuth (radians) */
  float   elevation_rad;      /* Elevation (radians) */

} pdsm_pd_meas_s_type;

#define PDSM_MEAS_PPM_RPT_VALID 0x0001

typedef struct
{
  uint16      flags;           /* flags to indicate if ppm is valid */
  uint8       num_svs;         /* num svs in this measurement */
  uint32      eph_svmask;      /* sv mask for the ephemeris available (GPS only) */
  uint32      alm_svmask;      /* sv mask for almanac available (GPS only) */
  uint32      glo_eph_svmask;  /* sv mask for the ephemeris available (Glonass only) */
  uint32      glo_alm_svmask;  /* sv mask for almanac available (Glonass only) */
  uint32      sbas_eph_mask;   /* available ephemeris mask (sbas only) */
  uint32      sbas_alm_mask;   /* available almanac mask (sbas only) */
  uint64      bds_eph_svmask;  /* available ephemeris mask (BDS only) */
  uint64      bds_alm_svmask;  /* available almanac mask (BDS only) */
  uint64      gal_eph_svmask;  /* available ephemeris mask (GAL only) */
  uint64      gal_alm_svmask;  /* available almanac mask (GAL only) */
  uint32      qzss_eph_svmask; /* available ephemeris mask (QZSS only) */
  uint32      qzss_alm_svmask; /* available almanac mask (QZSS only) */
  pdsm_srch_ppm_rpt_type ppm_data; /*  ppm data */ 
  pdsm_pd_meas_s_type ext_meas_report_type[PDSM_EXT_MEAS_NUM_SVS];
  uint32  iono_valid; /* GPS IONO is Valid */
  uint32  health_svmask; /* GPS SV mask for the health. if the bit is set, health for that SV is good */
  uint32  qzss_iono; /* QZSS IONO is Valid */
  uint32  qzss_health_svmask; /* QZSS SV mask for the health. if the bit is set, health for that SV is good */
  uint32  visible_svmask; /* GPS SV mask for the visibility. if the bit is set, SV is visiable */
  uint32  time_unc_ms; /* Time uncertainty in milli seconds, floor at 20 ms */
} pdsm_pd_ext_meas_s_type;

#define PDSM_PD_EXT_GPS_STATE_POS_VALID 0x0001 /* indicates lat/long and pos_unc_horizontal fields are valid */
#define PDSM_PD_EXT_GPS_STATE_ALT_VALID 0x0002 /* indicates alt and loc_Unc_vertical are valid */
#define PDSM_PD_EXT_GPS_STATE_TIME_MS_VALID 0x0004 /* indicates time stamp ms field is valid */
#define PDSM_PD_EXT_GPS_STATE_TIME_WN_VALID 0x0008 /* indicates timestamp week number field is valid */
#define PDSM_PD_EXT_GPS_STATE_TUNC_VALID    0x0010 /* indicates timestamp tunc field is valid */
#define PDSM_PD_EXT_GPS_STATE_EPH_VALID    0x0020 /* indicates eph svmask field is valid */
#define PDSM_PD_EXT_GPS_STATE_ALM_VALID    0x0040 /* indicates alm svmask field is valid */
#define PDSM_PD_EXT_GPS_STATE_XTRA_VALID    0x0080 /* indicates xtra data  svmask field is valid */
#define PDSM_PD_EXT_GPS_STATE_IONO_VALID    0x0100
#define PDSM_PD_EXT_GPS_STATE_HEALTH_VALID  0x0200
#define PDSM_PD_EXT_GPS_STATE_VISIBLE_SV_VALID 0x0400

typedef struct
{
  uint32 flags; /* flags to indicate which fields are valid */
  int32 lat;  /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2; +Ve values indicate North and -Ve values indiate South. valid if flags is set to PDSM_PD_EXT_GPS_STATE_POS_VALID */
  int32 lon;  /* Longitude: LSB = Pi/2^31, Range -Pi to Pi;+ve values indicate East and -ve values indicate West  . valid if flags is set to PDSM_PD_EXT_GPS_STATE_POS_VALID*/
  int16 altitude; /* Altitude in meters. valid if flags is set to PDSM_PD_EXT_GPS_STATE_ALT_VALID */
  uint32  loc_unc_horizontal; /* horizontal Position Unc in meters.  valid if flags is set to PDSM_PD_EXT_GPS_STATE_POS_VALID */
  uint16  loc_unc_vertical; /* vertical unc in meters.  valid if flags is set to PDSM_PD_EXT_GPS_STATE_ALT_VALID  */
  dword timestamp_tow_ms; /* Timestamp in GPS TOW milliseconds.  valid if flags is set to PDSM_PD_EXT_GPS_STATE_TIME_MS_VALID*/
  uint16  timestamp_gps_week;  /*GPS week number. valid if flags is set to PDSM_PD_EXT_GPS_STATE_TIME_WN_VALID */
  uint32  time_unc_ms; /* Time uncertainty in milli seconds, floor at 20 ms. valid if flags is set to PDSM_PD_EXT_GPS_STATE_TUNC_VALID */
  uint8 engine_state; /*  GPS engine state on/off. always present, no flag */
  uint32  eph_svmask; /* SV Mask for the ephemeris. if the bit is set, eph for that SV is available. valid if flags is set to PDSM_PD_EXT_GPS_STATE_EPH_VALID */
  uint32  alm_svmask; /* SV Mask for the Almanac. if the bit is set, alm for that SV is available. valid if flags is set to PDSM_PD_EXT_GPS_STATE_ALM_VALID */
  uint32  iono_valid; /* IONO is Valid. valid if flags is set to PDSM_PD_EXT_GPS_STATE_IONO_VALID */
  uint32  health_svmask; /* SV mask for the health. if the bit is set, health for that SV is good. valid if flags is set to PDSM_PD_EXT_GPS_STATE_HEALTH_VALID */
  uint16  xtra_start_gps_week; /*  Current XTRA info is good starting this GPS week.valid if flags is set to PDSM_PD_EXT_GPS_STATE_XTRA_VALID  */
  uint16  xtra_start_gps_minutes; /*  Current XTRA information is good starting this many minutes into the week . valid if flags is set to PDSM_PD_EXT_GPS_STATE_XTRA_VALID */
  uint16  xtra_valid_duration_hours; /*  XTRA info is valid for this many hours. valid if flags is set to PDSM_PD_EXT_GPS_STATE_XTRA_VALID */
  
  uint32  visible_svmask; /* SV mask for visible SVs. if bit is set, SV is visible. valid if flags is set to PDSM_PD_EXT_GPS_STATE_VISIBLE_SV_VALID */
} pdsm_pd_ext_gps_state_s_type;

/*- - - - - - - - - - - - - -COMMAND ERRORS - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of PD command errors returned by command
** callback functions.
*/
typedef enum pdsm_pd_cmd_err_e {

    PDSM_PD_CMD_ERR_NONE=-1,
        /* FOR INTERNAL USE OF PDSM ONLY! */

    PDSM_PD_CMD_ERR_NOERR,
        /* No errors found */

    /* Error codes resulting from clients feeding a PD command
    ** function with bad or inappropriate parameters.
    */

    PDSM_PD_CMD_ERR_CLIENT_ID_P,
        /* invalid client ID */

    PDSM_PD_CMD_ERR_SRV_TYPE_P,
        /* Bad service parameter (Not used anymore)*/

    PDSM_PD_CMD_ERR_SESS_TYPE_P,
        /* Bad session type parameter */
    PDSM_PD_CMD_ERR_PRIVACY_P, /* not used anymore */
    PDSM_PD_CMD_ERR_DLOAD_P, /* not used anymore */
    PDSM_PD_CMD_ERR_NET_ACCESS_P, /* not used anymore */
    PDSM_PD_CMD_ERR_OPERATION_P,
    PDSM_PD_CMD_ERR_NUM_FIXES_P,
    PDSM_PD_CMD_ERR_LSINFO_P,
        /* Wrong server Information parameters */

    PDSM_PD_CMD_ERR_TIMEOUT_P,
        /* Error in timeout parameter */

    PDSM_PD_CMD_ERR_QOS_P,
        /* Error in QOS accuracy thershold param */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Error codes resulting from clients trying to instruct
    ** PDSM to perform inappropriate actions relative to the current
    ** state.
    */

    PDSM_PD_CMD_ERR_NO_SESS_S,
         /* No session Active, while trying to end Session */

    PDSM_PD_CMD_ERR_SESS_ACT_S,
         /* Session active for this client  not used anymore */

    PDSM_PD_CMD_ERR_BUSY_S,
         /* Session Busy status */

    PDSM_PD_CMD_ERR_OFFLINE_S,
        /* Phone is offline */

    PDSM_PD_CMD_ERR_CDMA_LOCK_S,
        /* Phone is CDMA locked */

    PDSM_PD_CMD_ERR_GPS_LOCK_S,
        /* GPS is locked */

    PDSM_PD_CMD_ERR_STATE_S,
        /* The command is invalid in this state. (Ex When is phone is in E911CB) */

    PDSM_PD_CMD_ERR_NO_CONNECT_S,
        /* Connection Failure with PDE. */

    PDSM_PD_CMD_ERR_NO_BUF_L,
        /* No available PDSM command buffers to queue the command */

    PDSM_PD_CMD_ERR_SEARCH_COM_L,
        /* Communication problems with Search - e.g. SRCH buffer shortage */

    PDSM_PD_CMD_ERR_CANT_RPT_NOW_S,
        /* PD Results cannot be reported at this time, retry later or wait. not used anymore */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    PDSM_PD_CMD_ERR_MODE_NOT_SUPPORTED_S, /* not used anymore */
    PDSM_PD_CMD_ERR_PERIODIC_NI_IN_PROGRESS_S,

    PDSM_PD_CMD_ERR_AUTH_FAIL_S,
        /* Client Authentication Failure */

    PDSM_PD_CMD_ERR_OTHER,
        /* A problem other than the above was found */

    PDSM_PD_CMD_ERR_FEATURE_NOT_ENABLED,
        /* feature not enabled */

    PDSM_PD_CMD_ERR_UNKNOWN_CMD,
        /* unknown command */

    PDSM_PD_CMD_ERR_INVALID_PARAM,
        /* invalid parameters */

    PDSM_PD_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S,
       /*Location Service Reset in progress*/
    PDSM_PD_CMD_EMERGENCY_CALL_IN_PROGRESS_S,
      /*Emergency Call is in Progress*/

    PDSM_PD_CMD_ERR_MAX,
        /* FOR INTERNAL USE OF PDSM ONLY! */

    PDSM_PD_CMD_ERR_E_SIZE = 0x10000000
        /* To force enum to 32 bit */

} pdsm_pd_cmd_err_e_type;
/*~ SENTINEL pdsm_pd_cmd_err_e_type.PDSM_PD_CMD_ERR_MAX */

/* Enumeration of PA command errors returned by command
** callback functions.
*/
typedef enum pdsm_pa_cmd_err_e {
    PDSM_PA_CMD_ERR_NONE=-1,        /* FOR INTERNAL USE OF PDSM ONLY! */
    PDSM_PA_CMD_ERR_NOERR,          /* No errors found */
    PDSM_PA_CMD_ERR_NO_BUF_L,       /* No buffers available */
    PDSM_PA_CMD_ERR_CLIENT_ID_P,    /* Invalid Client id */
    PDSM_PA_CMD_ERR_PARAM_TYPE_P,   /* Error in Parameters */
    PDSM_PA_CMD_ERR_GPS_LOCK_P,     /* Error in Lock type */
    PDSM_PA_CMD_ERR_OFFLINE_S,      /* If phone is offline state */
    PDSM_PA_CMD_ERR_PA_ACTIVE_S,    /* An active pa_cmd is running */
    PDSM_PA_CMD_ERR_APP_INFO_P,     /* Wrong app info */
    PDSM_PA_CMD_ERR_NETWORK_ADDRS_P, /* wrong network address type */
    PDSM_PA_CMD_ERR_DELETE_PARM_P,  /* wrong delete param type */
    PDSM_PA_CMD_ERR_POSITION_MODE_P, /* wrong position mode type */
    PDSM_PA_CMD_ERR_MT_LR_P,        /* wrong MT_LR support type */
    PDSM_PA_CMD_ERR_MO_METHOD_P,    /* wrong MO method type */
    PDSM_PA_CMD_ERR_NMEA_SENT_P,    /* wrong NMEA sentence type */
    PDSM_PA_CMD_ERR_PDP_TYPE_P,     /* wrong PDP type*/
    PDSM_PA_CMD_ERR_SRV_SYS_P,      /* wrong serving system type*/
    PDSM_PA_CMD_ERR_OTHER,          /* A problem other than the above was found */
    PDSM_PA_CMD_ERR_UNKNOWN_CMD,    /* Unknown command */
    PDSM_PA_CMD_ERR_NOT_ALLOWED,    /* Command not allowed in current state */    
    PDSM_PA_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S, /*Location Service Reset in progress*/
    PDSM_PA_CMD_ERR_MAX,          /* A problem other than the above was found */
    PDSM_PA_CMD_ERR_E_SIZE = 0x10000000   /* Fix the size as int */
} pdsm_pa_cmd_err_e_type;
/*~ SENTINEL pdsm_pa_cmd_err_e_type.PDSM_PA_CMD_ERR_MAX */

typedef enum pdsm_lcs_cmd_err_e 
{
  PDSM_LCS_CMD_ERR_NONE = -1,
  PDSM_LCS_CMD_ERR_NOERR,
  PDSM_LCS_CMD_ERR_CLIENT_ID_P,
  PDSM_LCS_CMD_ERR_RESP_TYPE_P,
  PDSM_LCS_CMD_ERR_NO_BUF_L,
  PDSM_LCS_CMD_ERR_APP_INFO_P,
  PDSM_LCS_CMD_ERR_OTHER,
  PDSM_LCS_CMD_ERR_UNKNOWN_CMD,  
  PDSM_LCS_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S,
  PDSM_LCS_CMD_ERR_MAX,
  PDSM_LCS_CMD_ERR_E_SIZE =0x10000000 
} pdsm_lcs_cmd_err_e_type;
/*~ SENTINEL pdsm_lcs_cmd_err_e_type.PDSM_LCS_CMD_ERR_MAX */


typedef enum pdsm_ext_pos_cmd_err_e 
{
  PDSM_EXT_POS_CMD_ERR_NONE = -1,
  PDSM_EXT_POS_CMD_ERR_NOERR,
  PDSM_EXT_POS_CMD_ERR_NO_BUF_L,
  PDSM_EXT_POS_CMD_ERR_CLIENT_ID_P,
  PDSM_EXT_POS_CMD_ERR_OTHER,  
  PDSM_EXT_POS_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S,
  PDSM_EXT_POS_CMD_ERR_MAX,
  PDSM_EXT_POS_CMD_ERR_E_SIZE = 0x10000000
} pdsm_ext_pos_cmd_err_e_type;
/*~ SENTINEL pdsm_ext_pos_cmd_err_e_type.PDSM_EXT_POS_CMD_ERR_MAX */

/* Enum for gpsOneXTRA command errors
*/
typedef enum pdsm_xtra_cmd_err_e 
{
  PDSM_XTRA_CMD_ERR_NONE = -1,
  
  PDSM_XTRA_CMD_ERR_NOERR,
  PDSM_XTRA_CMD_ERR_CLIENT_ID_P,
  PDSM_XTRA_CMD_ERR_NO_BUF_L,
  PDSM_XTRA_CMD_ERR_BAD_CRC,
  PDSM_XTRA_CMD_ERR_TIME_OF_VALIDITY_OUT_OF_RANGE,
  PDSM_XTRA_CMD_ERR_DOWNLOAD_INTRVL_OUT_OF_RANGE,
  PDSM_XTRA_CMD_ERR_INTERNAL_RESOURCE_ERROR,
  PDSM_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY,
  PDSM_XTRA_CMD_ERR_TIME_INFO_ERROR,
  PDSM_XTRA_CMD_ERR_OTHER,
  PDSM_XTRA_CMD_ERR_GPS_LOCK_S,           /* GPS is locked */
  PDSM_XTRA_CMD_ERR_STATE_S,              /* Command is invalid in this state. (Ex When is phone is in E911CB) */
  PDSM_XTRA_CMD_ERR_UNKNOWN_CMD,          /* Unknown command */
  PDSM_XTRA_CMD_ERR_FEATURE_NOT_ENABLED,  /* Feature not enabled */
  PDSM_XTRA_CMD_ERR_INVALID_PARAM,        /* Invalid parameter */
  PDSM_XTRA_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S, /*Location Service Reset in progress*/  
  PDSM_XTRA_CMD_ERR_MAX,
  PDSM_XTRA_CMD_ERR_E_SIZE = 0x10000000
} pdsm_xtra_cmd_err_e_type;
/*~ SENTINEL pdsm_xtra_cmd_err_e_type.PDSM_XTRA_CMD_ERR_MAX */

/* Enum for XTRA-T command errors
*/
typedef enum 
{
  PDSM_XTRA_T_CMD_ERR_NONE = -1,
  PDSM_XTRA_T_CMD_ERR_SUCCESS,
  PDSM_XTRA_T_CMD_ERR_BUSY ,          /* Another XTRA-T session is in progress */
  PDSM_XTRA_T_CMD_ERR_OTHER,          /* Ending due to internal errors - data pointer error, E911, GPS lock, cmd not supported  */
  PDSM_XTRA_T_CMD_ERR_CLIENT_ID_P,    /* Ending due to XTRA-T client not registered/available */
  PDSM_XTRA_T_CMD_FEATURE_NOT_ENABLED,/* Ending due to XTRA-T feature is not enabled */
  PDSM_XTRA_T_CMD_ERR_BAD_GET_DATA_PARAM, /* Ending due to bad XTRA-T get data parameters */
  PDSM_XTRA_T_CMD_ERR_INTERNAL_RESOURCE, /* Ending due to internal resource errors - TM task not active */
  PDSM_XTRA_T_CMD_ERR_GPS_LOCK_S,           /* GPS is locked */
  PDSM_XTRA_T_CMD_ERR_STATE_S,              /* Command is invalid in this state. (Ex When is phone is in E911CB) */
  PDSM_XTRA_T_CMD_ERR_UNKNOWN_CMD,          /* Unknown command */
  PDSM_XTRA_T_CMD_ERR_FEATURE_NOT_ENABLED,  /* Feature not enabled */
  PDSM_XTRA_T_CMD_ERR_INVALID_PARAM,        /* Invalid parameter */
  PDSM_XTRA_T_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S, /*Location Service Reset in progress*/  
  PDSM_XTRA_T_CMD_END_MAX,                /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_XTRA_T_CMD_END_E_SIZE=0x10000000   /* To force enum to 32 bit for windows NT */
} pdsm_xtra_t_cmd_err_e_type;
/*~ SENTINEL pdsm_xtra_t_cmd_err_e_type.PDSM_XTRA_T_CMD_END_MAX */

/*
** Enumerations for end of session reasons.
*/

typedef enum pdsm_pd_end_e {
    PDSM_PD_END_SESS_NONE = -1,     /* For internal use of PDSM */
    PDSM_PD_END_OFFLINE,            /* Phone Offline */
    PDSM_PD_END_NO_SRV,             /* No servcie */
    PDSM_PD_END_NO_CON,             /* No connection with PDE */
    PDSM_PD_END_NO_DATA,            /* No data available */
    PDSM_PD_END_SESS_BUSY,          /* Session Manager Busy */
    PDSM_PD_END_CDMA_LOCK,          /* Phone is CDMA locked */
    PDSM_PD_END_GPS_LOCK,           /* Phone is GPS locked */
    PDSM_PD_END_CON_FAIL,           /* Connection failure with PDE */
    PDSM_PD_END_ERR_STATE,          /* PDSM Ended session because of Error condition */
    PDSM_PD_END_CLIENT_END,         /* User ended the session */
    PDSM_PD_END_UI_END,             /* End key pressed from UI */
    PDSM_PD_END_NT_END,             /* Network Session was ended */
    PDSM_PD_END_TIMEOUT,            /* Timeout (viz., for GPS Search) */
    PDSM_PD_END_PRIVACY_LEVEL,      /* Conflicting reques for session and level of privacy */
    PDSM_PD_END_NET_ACCESS_ERR,     /* Could not connect to the Network */
    PDSM_PD_END_FIX_ERROR,             /* Error in Fix */
    PDSM_PD_END_PDE_REJECT,         /* Reject from PDE */
    PDSM_PD_END_TC_EXIT,            /* Ending session due to TC exit. */
    PDSM_PD_END_E911,               /* Ending session due to E911 call */
    PDSM_PD_END_SERVER_ERROR,      /* Added protocol specific error type */
    PDSM_PD_END_STALE_BS_INFO,      /* Ending because BS info is stale */
    PDSM_PD_END_VX_AUTH_FAIL,       /* VX lcs agent auth fail */
    PDSM_PD_END_UNKNWN_SYS_ERROR,   /* Unknown System Error */
    PDSM_PD_END_UNSUPPORTED_SERVICE, /* Unsupported Service */
    PDSM_PD_END_SUBSRIPTION_VIOLATION,  /* Subscription Violation */
    PDSM_PD_END_FIX_METHOD_FAILURE,     /* The desired fix method failed */
    PDSM_PD_END_ANTENNA_SWITCH,   /* Antenna switch */
    PDSM_PD_END_NO_FIX_NO_TX_CONFIRM, /* No fix reported due to no tx confirmation rcvd */
    PDSM_PD_END_NORMAL_ENDING,            /* Network indicated a Normal ending of the session */
    PDSM_PD_END_NONSPECIFIED_ERROR,  /* No error specified by the network */
    PDSM_PD_END_RESOURCE_SHORTAGE,    /* No resources left on the network */
    PDSM_PD_END_POS_SERVER_NOT_AVAILABLE, /* Position server not available */
    PDSM_PD_END_UNSUPPORTED_VERSION, /* Network reported an unsupported version of protocol*/
    PDSM_PD_END_CLIENT_HAS_NO_SESS,  /* Current session doesn't belong to this client */

    PDSM_PD_END_SS_MOLR_ERRORS_SYSTEM_FAILURE, /* mapped to corresponding SS-molr-error error code */           
    PDSM_PD_END_SS_MOLR_ERRORS_UNEXPECTED_DATA_VALUE,
    PDSM_PD_END_SS_MOLR_ERRORS_DATA_MISSING,
    PDSM_PD_END_SS_MOLR_ERRORS_FACILITY_NOT_SUPPORTED,
    PDSM_PD_END_SS_MOLR_ERRORS_SS_SUBSCRIPTION_VIOLATION,
    PDSM_PD_END_SS_MOLR_ERRORS_POSITION_METHOD_FAILURE,
    PDSM_PD_END_SS_MOLR_ERRORS_UNDEFINED,

    PDSM_PD_END_CP_CF_DISRUPT_SMLC_TO, /* control plane's smlc timeout, may or may not end pd */
    PDSM_PD_END_MT_GUARD_TIMER_EXPIRY, /* control plane's MT guard time expires */
    PDSM_PD_END_WAIT_ADDITION_ASSIST_EXPIRY, /* end waiting for addtional assistance */

    /* KDDI specific error codes */
    PDSM_PD_END_JGPS_POSRESP_NG_LIS,   /* Position response Non Good(NG) reception ( LIS side system anomaly )  */
    PDSM_PD_END_JGPS_POSRESP_NG_LSU,   /* Position response NG reception (Beyond the LSU maximum session count */
    PDSM_PD_END_JGPS_POSRESP_NG_MSI,   /* Position response NG reception (MS side setting information failure? */
    PDSM_PD_END_JGPS_STOPPOS_NG_LIS,   /* Session interruption NG reception ( LIS side system anomaly?   */
    PDSM_PD_END_JGPS_STOPPOS_NG_MSI,   /* Session interruption NG reception (MS side setting information failure?  */
    PDSM_PD_END_JGPS_POSCOMP_NG,       /* Abnormal response reception  */
    PDSM_PD_END_JGPS_TIMEOUT_T04,      /* T04 timer timed out */
    PDSM_PD_END_JGPS_TIMEOUT_STOPPOS,  /* T03 timer timed out */
    PDSM_PD_END_JGPS_TIMEOUT_T02,      /* T02 timer timed out */
    PDSM_PD_END_JGPS_TIMEOUT_IS801,    /* IS801 timer timed out */
    PDSM_PD_END_JGPS_IS801_LRREJ,      /* LR Reject  reception */
    PDSM_PD_END_JGPS_IS801_AAREJ,      /* AA Reject  reception */    
    PDSM_PD_END_JGPS_IS801_EPHREJ,     /* EPH Reject  reception */
    PDSM_PD_END_JGPS_IS801_ALMREJ,     /* ALM Reject  reception */
    PDSM_PD_END_JGPS_IS801_SEEDREJ,    /* SEED Reject  reception */
    PDSM_PD_END_JGPS_IS801_AA_FAIL,    /* IS801 sequence error  */
    PDSM_PD_END_JGPS_PPP,              /* PPP establish  trial  failure */
    PDSM_PD_END_JGPS_TCP_MS,           /* Network link disconnection after PPP  established ( MS initiated) */     
    PDSM_PD_END_JGPS_TCP_SERV,         /* Network link disconnection after PPP established ( Server initiated) */
    PDSM_PD_END_JGPS_ACQRESP_NG_LIS,   /* GPS  data  request  response NG reception (LIS side system anomaly) */
    PDSM_PD_END_JGPS_ACQRESP_NG_LSU,   /* GPS data request response NG reception (Beyond LSU maximum session count)  */
    PDSM_PD_END_JGPS_ACQRESP_NG_MSI,   /* GPS data request response NG reception (MS side setting information NG? */
    PDSM_PD_END_JGPS_STOPACQ_NG_LIS,   /* GPS data request interruption NG reception (LIS side system anomaly? */
    PDSM_PD_END_JGPS_STOPACQ_NG_MSI,   /* GPS data request interruption NG reception (MS side setting information NG? */
    PDSM_PD_END_JGPS_TIMEOUT_T20,      /* T20  timer timed out */
    PDSM_PD_END_JGPS_TIMEOUT_STOPACQ,  /* T21 timer timed out */
    PDSM_PD_END_MO_LR_NOT_DELIVERED,   /* When MO LR COMPLETED message is recvd */
    PDSM_PD_END_JGPS_DMSB_ERROR = 901, /* MSB:No fix  with download the data */
    PDSM_PD_END_JGPS_DPOSCOMP_NG = 911,/* MSA(MSB auto): No fix  with download the data */

    PDSM_PD_END_RESET_LOCATION_SERVICE, /*Request for reset location service*/

    PDSM_PD_END_PPM_POST_FIX_TIMEOUT,  /* PPM calculation started at the request of IS801 PDE, timed out */
    PDSM_PD_END_PPM_PRE_FIX_TIMEOUT,   /* PPM calculation started at the start of the IS801 session timed out */
    PDSM_PD_END_IS801_SERVER_TIMEOUT,  /* PDE does not send respond to UE within IS801 session timer */
    PDSM_PD_END_IS801_PAYLOAD_ERROR,   /* PDDM received from PDE has errors and decoding fails */
    PDSM_PD_END_IS801_SEND_FAILURE,    /* Fail to send PDDM back to PDE */
    PDSM_PD_END_PDE_END_SESSION,       /* PDE requested to end session */

   /* SUPL END error codes */
    PDSM_PD_END_SUPL_COMM_ERROR = 1000,/* SUPL data connection error*/
    PDSM_PD_END_SUPL_VER_MISMATCH,     /* SUPL verions mismatch in request*/
    PDSM_PD_END_SUPL_INTERNAL_ERROR,   /* SUPL START failure due to internal error*/
    PDSM_PD_END_SUPL_ASN1_DECODE_ERROR,/* SUPL ASN.1 decoding of message failed */
    PDSM_PD_END_SUPL_MESSAGE_INVALID_SESSION_ID, /* SUPL error Invalide Session ID*/
    PDSM_PD_END_SUPL_UNEXPECTED_MESSAGE,   /* SUPL error, cannot handle message in current state*/
    PDSM_PD_END_SUPL_POS_METHOD_MISMATCH,  /* SUPL Positioning method mismatch*/
    PDSM_PD_END_SUPL_PROTOCOL_ERROR,  /* SUPL RRPL protocol error*/
    PDSM_PD_END_SUPL_NO_POS_FROM_SLP,      /* SUPL error, no Location Estimate from SLP after MSA*/
    PDSM_PD_END_SUPL_NO_CELL_ID_FROM_SLP,  /* SUPL error, no Cell ID from SLP*/
    PDSM_PD_END_SUPL_METHOD_NOT_SUPPORTED,  /* SUPL error,  method not supported*/
    PDSM_PD_END_SUPL_NOT_ENOUGH_SAT,    /* SUPL error, MGP/TM-core, failed to provide Location Estimate or PRM*/

    PDSM_PD_END_MAX,                /* Maximum for PDSM internal use */
    PDSM_PD_END_E_SIZE = 0x10000000 /* To fix enum Size as int */
} pdsm_pd_end_e_type;

typedef enum 
{
  PDSM_PD_COMM_PROTOCOL_UMTS_UP_SUPL,
  PDSM_PD_COMM_PROTOCOL_1X,
  PDSM_PD_COMM_PROTOCOL_UMTS_CP_WCDMA,
  PDSM_PD_COMM_PROTOCOL_UMTS_CP_GSM,
  PDSM_PD_COMM_PROTOCOL_V1V2,
  PDSM_PD_COMM_PROTOCOL_KDDI,       /* Deprecated, use PDSM_PD_COMM_PROTOCOL_JGPS instead */
  PDSM_PD_COMM_PROTOCOL_JGPS = PDSM_PD_COMM_PROTOCOL_KDDI,
  PDSM_PD_COMM_PROTOCOL_1X_CP,
  PDSM_PD_COMM_PROTOCOL_LPP_CP,
  PDSM_PD_COMM_PROTOCOL_LTE_UP,
} pdsm_pd_comm_protocol_e_type; 

typedef struct {
   pdsm_pd_comm_protocol_e_type protocol_type;
} pdsm_pd_comm_data_s_type;

/* JGPS progress status */
typedef enum {
  PDSM_PD_JGPS_PROG_SEND_POS_REQ,
  PDSM_PD_JGPS_PROG_SEND_ACQ_REQ,
  PDSM_PD_JGPS_PROG_SEND_LR_REQ
} pdsm_pd_jgps_progress_e_type;


#define PDSM_LATLONG_SCALE       (100000000)
#define PDSM_MIN_SCALE           (1000000)

/* Currently deprecated in GNSS code */

/* For SVs in view, active channels only, using 16*3=48 for now */
#define PDSM_GNSS_MAX_SATELLITES PDSM_EXT_MEAS_NUM_SVS

/* Enum defined in gpsOne API for Third-Party WinMobile OS 80-VC723-1 rev C */
typedef enum pdsm_pd_fix_quality_e{
   PDSM_PD_FIX_QUALITY_NONE = -1,      /* FOR INTERNAL USE OF PDSM ONLY! */
   PDSM_PD_FIX_QUALITY_UNKNOWN = 0,    /* Quality of fix is unknown. */
   PDSM_PD_FIX_QUALITY_GPS,            /* Fix uses information from GNSS satellites only. */
   PDSM_PD_FIX_QUALITY_DGPS,           /* Fix uses information from GNSS satellites and also a differential GPS (DGPS) station. */
   PDSM_PD_FIX_QUALITY_MAX,            /* FOR INTERNAL USE OF PDSM ONLY! */
   PDSM_PD_FIX_QUALITY_E_SIZE=0x100000 /* To force enum to int */
} pdsm_pd_fix_quality_e_type;


typedef enum pdsm_pd_fix_type_e{
   PDSM_PD_FIX_TYPE_NONE = -1,      /* FOR INTERNAL USE OF PDSM ONLY! */
   PDSM_PD_FIX_TYPE_UNKNOWN = 0,    /* Type of fix is unknown.  */
   PDSM_PD_FIX_TYPE_2D,             /* Fix is two-dimensional and only provides latitude and longitude. This occurs when only three GNSS satellites are used.  */
   PDSM_PD_FIX_TYPE_3D,             /* Fix is three-dimensional and provides latitude, longitude, and elevation. This occurs when four or more GNSS satellites are used. */
   PDSM_PD_FIX_TYPE_MAX,            /* FOR INTERNAL USE OF PDSM ONLY! */
   PDSM_PD_FIX_TYPE_E_SIZE=0x100000 /* To force enum to int */ 
} pdsm_pd_fix_type_e_type;


typedef enum pdsm_pd_fix_selection_e{
   PDSM_PD_FIX_SELECTION_NONE = -1,      /* FOR INTERNAL USE OF PDSM ONLY! */
   PDSM_PD_FIX_SELECTION_UNKNOWN = 0,    /* Selection type is unknown.  */
   PDSM_PD_FIX_SELECTION_AUTO,           /* Selection mode is automatic.*/
   PDSM_PD_FIX_SELECTION_MANUAL,         /* Selection mode is manual.   */
   PDSM_PD_FIX_SELECTION_MAX,            /* FOR INTERNAL USE OF PDSM ONLY! */
   PDSM_PD_FIX_SELECTION_E_SIZE=0x100000 /* To force enum to int */ 
} pdsm_pd_fix_selection_e_type;


typedef struct pdsm_sv_inview_s
{
  gnss_sv_id_type   sv_num;              /* SV Ids (GPS 1..32, SBAS 33..64, GLO 65..96, BDS 201..237, GAL 301-336 ) */

  uint8   elev;           /* [deg] 90 max */
  uint16  azimuth_snr;    /* If azimuth_snr is the decimal number AAASS
                             then 
                               AAA = azimuth in [deg] 000 to 359 
                               SS = snr in [dB]  00 to 99, null when not tracking  */
} pdsm_sv_inview_s_type;

/* Extended PD Info Data */
typedef struct pdsm_pd_data_ext_s
{
  int64         lat;  /* If Security registration is enabled by the GPS Session Manager, */
                      /* Valid if pdsm_pd_data_type.ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set*/
  int64         lon;  /* then Latitude and Longitude will be encyrpted into a 128 bit 
                         ciphertext.  The upper 64 bits of the ciphertext will be placed in 
                         pdsm_pd_data_ext_s.lat and the lower 64 bits will be placed in
                         pdsm_pd_data_ext_s.lon

                         Unit is in 10^8 degree. 

                         Data will be processed into NMEA format :
                         [+/-]DD [degree] 
                                 Two digits of degrees. 
                                 Range is +/- 90 degree
                                 mm.mmmmmm - [minutes]
                         1X implementation is 6 decimal digits.
                         UMTS implementation has 5 decimal digits
                         Range is 59.999999 minutes 
                      */
  /* Valid if pdsm_pd_data_type.ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set*/
  int32         altitude;                 /* [x10 m]  Height Above Ellipsoid (HAE) */  
                                          /* Valid if pdsm_pd_data_type.flags is set to PDSM_PD_EXT_GPS_STATE_ALT_VALID */
  uint32        speed_knot;              /* [x10 knot]. Valid if velocity_include is set to 1 */
  uint32        speed_kmh;               /* [x10 kmh]. Valid if velocity_include is set to 1 */
  uint16        heading_true;            /* [x10 deg]   Range: 0..359.9 degree. Valid if pdsm_pd_data_type.ext_opt_field_mask
                                            PDSM_PD_HEADING_VALID is set*/
  uint8         velocity_include;        /* velocity information available */

  float         f_VelocityENUMps[3];          /* East, North, Up velocity (m/s) */
  float         f_VelocityUncENUMps[3];    /* East, North, Up velocity uncertainty (m/s) */

  pdsm_pd_fix_type_e_type  fix_type;     /* no fix, 2D or 3D fix */
                                     /* UTC Time  for Hour, Minute, Sec */
  uint8         hour;                /*  in range of  00..23 */
  uint8         minute;              /*  in range of 00..59  */
  uint16        sec;                 /*  ssss [x100 sec]  in range of  0..5999 */
                                    
  uint32        date;                /* UTC Date ddmmyyyy 
                                         dd  in range of [1..31]
                                         mm in range of [1..12]
                                         yyyy in range of [0..9999]   
                                     */
                                     /* the above time fields are 
                                       valid if ext_opt_field_mask PDSM_PD_GPS_TIME_VALID is set */
  uint32        h_dop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  uint32        p_dop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  uint32        v_dop;               /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */  
                                     /* DOP fields above are valid if pdsm_pd_data_type.ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set*/
  int64         magnetic_variation;  /* [x10 degree]  */
                                     /* Valid if pdsm_pd_data_type.ext_opt_field_mask PDSM_PD_LAT_LONG_VALID is set*/

  pdsm_pd_fix_selection_e_type selection_type; /* Manual or Auto 2D/3D mode */
  pdsm_pd_fix_quality_e_type   fix_quality;    /* Fix uses information from GNSS SVs or differential data also */    

  uint8                        num_sv_in_view;          /* number of SVs in view */
  pdsm_sv_inview_s_type        sv[PDSM_GNSS_MAX_SATELLITES];  /* 0..num_sv_inview-1 contain the valid data */
} pdsm_pd_data_ext_s_type;

typedef enum 
{
  PDSM_PD_QWIP_START_PERIODIC_HI_FREQ_FIXES,
  PDSM_PD_QWIP_START_PERIODIC_KEEP_WARM,
  PDSM_PD_QWIP_STOP_PERIODIC_FIXES,
  PDSM_PD_QWIP_SUSPEND
}pdsm_pd_qwip_request_e_type;

typedef struct {
  pdsm_pd_qwip_request_e_type request_type;
  int16                       tbf_ms;
}pdsm_pd_qwip_data_s_type;

/* external SPI injection request event data */
/* see also PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION */
typedef enum {
   PDSM_PD_EXTERNAL_SPI_STREAM_STATUS_DISABLE = 0,
   PDSM_PD_EXTERNAL_SPI_STREAM_STATUS_ENABLE,

   /* add more status above this line */
   PDSM_PD_EXTERNAL_SPI_STREAM_STATUS_MAX,
   PDSM_PD_EXTERNAL_SPI_STREAM_STATUS_MAX_RESERVED = 0x7FFFFFFF /* to force size to at least 4 bytes */
} pdsm_pd_external_spi_stream_status_type;
/*~ SENTINEL pdsm_pd_external_spi_stream_status_type.PDSM_PD_EXTERNAL_SPI_STREAM_STATUS_MAX */

typedef struct {
   uint32 q_stream_status; /* see pdsm_pd_external_spi_stream_status_type */
   uint32 q_Reserved[2];   /* set to 0 */
} pdsm_pd_external_spi_event_info_type;


typedef enum
{
  PD_LOC_SV_SYSTEM_GPS                    = 1,
    /**< GPS satellite. */

  PD_LOC_SV_SYSTEM_GALILEO                = 2,
    /**< GALILEO satellite. */

  PD_LOC_SV_SYSTEM_SBAS                   = 3,
    /**< SBAS satellite. */

  PD_LOC_SV_SYSTEM_COMPASS                = 4,
    /**< COMPASS satellite. */

  PD_LOC_SV_SYSTEM_GLONASS                = 5,
    /**< GLONASS satellite. */

  PD_LOC_SV_SYSTEM_BDS                    = 6,
    /**< BDS satellite. */
  PD_LOC_SV_SYSTEM_QZSS                   = 7,
  PD_LOC_SV_SYSTEM_MAX                    = 8
    /**< QZSS satellite. */
} pdsm_LocSvSystemEnumType;

typedef enum
{
        PD_LOC_FREQ_SOURCE_INVALID = 0,
        /**< Source of the frequency is invalid */
        PD_LOC_FREQ_SOURCE_EXTERNAL = 1,
        /**< Source of the frequency is from external injection */
        PD_LOC_FREQ_SOURCE_PE_CLK_REPORT = 2,
        /**< Source of the frequency is from Navigation engine */
        PD_LOC_FREQ_SOURCE_UNKNOWN = 3
        /**< Source of the frequency is unknown */
} pdsm_LocSourceofFreqEnumType;

typedef struct
{
        float                                        f_clockDrift;
        /**< Receiver clock Drift \n
             - Units: meter per sec \n
        */
        float                                        f_clockDriftUnc;
        /**< Receiver clock Drift uncertainty \n
             - Units: meter per sec \n
        */

        pdsm_LocSourceofFreqEnumType        e_sourceOfFreq;
        /**< Source of the frequency \n
                 @ENUM()                        \n
        */
}pdsm_LocRcvrClockFrequencyInfoStructType;

typedef struct
{
        uint8                                        u_leapSec;
        /**< GPS time leap second delta to UTC time  \n
                     - Units: sec \n
        */
        uint8                                        u_leapSecUnc;
        /**< Uncertainty for GPS leap second \n
                     - Units: sec \n
        */
}pdsm_LocLeapSecondInfoStructType;

typedef enum
{
   PD_LOC_SYS_TIME_BIAS_VALID                = 0x01,
   /**< System time bias valid */

   PD_LOC_SYS_TIME_BIAS_UNC_VALID            = 0x02,
   /**< System time bias uncertainty valid */

}pdsm_LocInterSystemBiasValidMaskType;

typedef struct
{
        uint32                                        u_validMask;        /* Validity mask as per enum pdsm_LocInterSystemBiasValidMaskType */
         /**< Fields that are valid.

       Valid bitmasks: \begin{itemize1}
       \item    0x01 -- SYS_TIME_BIAS_VALID
       \item    0x02 -- SYS_TIME_BIAS_UNC_VALID
        */
        float                                        f_sys1Sys2TB;
        /**< System-1 to System-2 Time Bias  \n
                     - Units: msec \n
        */
        float                                        f_sys1Sys2TBUnc;
        /**< System-1 to System-2 Time Bias uncertainty  \n
                     - Units: msec \n
        */
}pdsm_InterSystemBiasStructType;

typedef struct
{

        uint32  q_refFCount;
        /**< Receiver frame counter value at reference tick. Mandatory field if strcture is present */

        boolean b_SystemRtcValid;
        /**< Validity indicator for System RTC */

        uint64  t_systemRtcMs;
        /**< System RTC value at clock validity \n
                - Units: msec \n
        */

        uint16                 w_systemWeek;
        /**< System week number for GPS, BDS and GAL satellite systems. \n
                Set to 65535 when invalid or not available. \n
                Not valid for GLONASS system. \n
                Mandatory field if strcture is present
        */

        uint8       u_gloFourYear;
        /**< GLONASS four year number from 1996. \n
                Applicable only for GLONASS and shall be ignored for other constellations. \n
                Mandatory field if strcture is present
        */

        uint16      w_gloDays;
        /**< GLONASS day number in four years.
                Applicable only for GLONASS and shall be ignored for other constellations. \n
                Mandatory field if strcture is present
        */

        uint32                 q_systemMsec;
        /**< System time msec. Time of Week for GPS, BDS, GAL and Time of Day for GLONASS.
                - Units: msec \n
                Mandatory field if strcture is present. Interpretation of value based on SourceOfTime field.
        */

        float                  f_systemClkTimeBias;
        /**< System clock time bias \n
                        - Units: msec \n
                System time = systemMsec - systemClkTimeBias \n
                Mandatory field if strcture is present. Interpretation of value based on SourceOfTime field.
        */

        float                  f_systemClkTimeUncMs;
        /**< Single sided maximum time bias uncertainty \n
                                - Units: msec \n
                 Mandatory if structure is present.                                
        */

        uint8                 e_SourceOfTime;
        /**< Source of the time. Mandatory if structure is present */

}pdsm_LocSystemTimeStructType;


typedef enum
{
        PD_LOC_MEAS_STATUS_NULL                                  = 0x00000000,
        /**< No information state */

        PD_LOC_MEAS_STATUS_SM_VALID                          = 0x00000001,
        /**< Code phase is known */

        PD_LOC_MEAS_STATUS_SB_VALID                          = 0x00000002,
        /**< Sub-bit time is known */

        PD_LOC_MEAS_STATUS_MS_VALID                          = 0x00000004,
        /**< Satellite time is known */

        PD_LOC_MEAS_STATUS_BE_CONFIRM                        = 0x00000008,
        /**< Bit edge is confirmed from signal        */

        PD_LOC_MEAS_STATUS_VELOCITY_VALID                = 0x00000010,
        /**< Satellite Doppler measured */

        PD_LOC_MEAS_STATUS_VELOCITY_FINE                = 0x00000020,
        /**< TRUE: Fine Doppler measured, FALSE: Coarse Doppler measured */

        PD_LOC_MEAS_STATUS_FROM_RNG_DIFF                = 0x00000200,
        /**< Range update from Satellite differences */

        PD_LOC_MEAS_STATUS_FROM_VE_DIFF                 = 0x00000400,
        /**< Doppler update from Satellite differences */

        PD_LOC_MEAS_STATUS_DONT_USE_X                   = 0x00000800,

        PD_LOC_MEAS_STATUS_DONT_USE_M                   = 0x00001000,

        PD_LOC_MEAS_STATUS_DONT_USE_D                   = 0x00002000,

        PD_LOC_MEAS_STATUS_DONT_USE_S                   = 0x00004000,

        PD_LOC_MEAS_STATUS_DONT_USE_P                   = 0x00008000,

}pdsm_LocSvMeasStatusMaskType;

#define PD_LOC_MEAS_STATUS2_DONT_USE_BITS  0xFFC00000

typedef enum
{
        PD_LOC_MEAS_STATUS2_DONT_USE_X                         = 0x00400000,
        /**< Don't use measurement if bit is set */

        PD_LOC_MEAS_STATUS2_DONT_USE_M                         = 0x00800000,
        /**< Don't use measurement if bit is set */

        PD_LOC_MEAS_STATUS2_DONT_USE_D                         = 0x01000000,
        /**< Don't use measurement if bit is set */

        PD_LOC_MEAS_STATUS2_DONT_USE_S                         = 0x02000000,
        /**< Don't use measurement if bit is set */

        PD_LOC_MEAS_STATUS2_DONT_USE_P                         = 0x04000000,
        /**< Don't use measurement if bit is set */

}pdsm_LocSvMeasStatus2DontUseMaskType;

typedef enum

{
   PD_LOC_MEAS_STATUS2_NULL                = 0x00000000
   /**< No information state */

}pdsm_LocSvMeasStatus2MaskType;

typedef struct
{
        uint32                                                         q_svMs;
        /**<         Satellite time milisecond.\n
                        For GPS, BDS, GAL range of 0 thru (604800000-1) \n
                        For GLONASS range of 0 thru (86400000-1) \n
                        Valid when PD_LOC_MEAS_STATUS_MS_VALID bit is set in measurement status \n
                        Note: All SV times in the current measurement block are alredy propagated to common reference time epoch. \n
                                - Units: msec \n
                        Mandtory field, interpretation based on measurement status
        */

        float                                                         f_svSubMs;
        /**<         Satellite time sub-millisecond. \n
                        Total SV Time = svMs + svSubMs \n
                                - Units: msec \n
                         Mandtory field, interpretation based on measurement status
        */

        float      f_svTimeUncMs;
        /**<  Satellite Time uncertainty \n
                                - Units: msec \n
                         Mandtory field, interpretation based on measurement status
        */

   float                                                        f_dopplerShift;
        /**< Satellite Doppler \n
                     - Units: meter per sec \n
                         Mandtory field, interpretation based on measurement status
        */

   float                                                        f_dopplerShiftUnc;
        /**< Satellite Doppler uncertainty\n
                     - Units: meter per sec \n
                         Mandtory field, interpretation based on measurement status
        */

}pdsm_LocSVTimeSpeedStructType;

typedef enum
{
  PD_GNSS_SV_STATE_IDLE = 0,
  PD_GNSS_SV_STATE_SEARCH = 1,
  PD_GNSS_SV_STATE_SEARCH_VERIFY = 2,
  PD_GNSS_SV_STATE_BIT_EDGE = 3,
  PD_GNSS_SV_STATE_VERIFY_TRACK = 4,
  PD_GNSS_SV_STATE_TRACK = 5,
  PD_GNSS_SV_STATE_RESTART = 6,
  PD_GNSS_SV_STATE_DPO_TRACK = 7
} pdsm_LocSVStateEnumType;

typedef enum
{
  PD_LOC_SVINFO_MASK_HAS_EPHEMERIS   = 0x01,
  /**< Ephemeris is available for this SV */
  PD_LOC_SVINFO_MASK_HAS_ALMANAC     = 0x02
  /**< Almanac is available for this SV */
}pdsm_LocSvInfoMaskT;


typedef enum
{
  PD_LOC_SV_SRCH_STATUS_IDLE                   = 1,
    /**< SV is not being actively processed */

  PD_LOC_SV_SRCH_STATUS_SEARCH                 = 2,
    /**< The system is searching for this SV */

  PD_LOC_SV_SRCH_STATUS_TRACK                  = 3,
    /**< SV is being tracked */

}pdsm_LocSvSearchStatusEnumT;


typedef struct
{
  uint16                             w_gnssSvId;
    /**< GNSS SV ID.
         \begin{itemize1}
         \item Range:  \begin{itemize1}
           \item For GPS:      1 to 32
           \item For GLONASS:  1 to 32
           \item For SBAS:     120 to 151
           \item For BDS:      201 to 237
         \end{itemize1} \end{itemize1}

        The GPS and GLONASS SVs can be disambiguated using the system field. */

   uint8                                                        u_gloFrequency;
        /**< GLONASS frequency number + 7 \n
             Valid only for GLONASS System \n
                 Shall be ignored for all other systems \n
                     - Range: 1 to 14 \n
        */

   pdsm_LocSvSearchStatusEnumT                                e_svStatus;
   /**< Satellite search state \n
                @ENUM()
        */

   boolean                                                        b_healthStatus_valid;
           /**< SV Health Status validity flag\n
                         - 0: Not valid \n
                         - 1: Valid \n
        */

   uint8                             u_healthStatus;
    /**< Health status.
         \begin{itemize1}
         \item    Range: 0 to 1; 0 = unhealthy, \n 1 = healthy, 2 = unknown
         \vspace{-0.18in} \end{itemize1}*/

    pdsm_LocSvInfoMaskT                 e_svInfoMask;
    /**< Indicates whether almanac and ephemeris information is available. \n

        @MASK()
    */


        uint64                         t_measurementStatusValid;
        /**< Bitmask indicating SV measurement status bits that are valid

     */

   uint64                    t_measurementStatus;
    /**< Bitmask indicating SV measurement status.

         Valid bitmasks: \n
         @MASK()  */

   uint16                                                        w_CNo;
        /**< Carrier to Noise ratio  \n
                     - Units: 0.1 dBHz \n
        */

   uint16                                                  w_gloRfLoss;
        /**< GLONASS Rf loss reference to Antenna. \n
                             - Units: dB, Scale: 0.1 \n
        */

   boolean                                                        b_lossOfLockValid;
        /**< Loss of lock validity flag\n
                         - 0: Loss of lock not valid \n
                         - 1: Loss of lock valid \n
        */
   boolean                                                        b_lossOfLock;
        /**< Loss of signal lock indicator  \n
                         - 0: Signal in continuous track \n
                         - 1: Signal not in track \n
        */
    int16                                                   x_measLatency;
        /**< Age of the measurement. Positive value means measurement precedes ref time. \n
                             - Units: msec \n
        */

        pdsm_LocSVTimeSpeedStructType        z_svTimeSpeed;
        /**< Unfiltered SV Time and Speed information */


        boolean                                                  b_dopplerAccelValid;
         /**< Doppler acceleartion validity flag\n
                          - 0: Doppler acceleation not valid \n
                          - 1: Doppler acceleation estimate valid \n
         */

   float     f_dopplerAccel;
        /**< Satellite Doppler Accelertion\n
                     - Units: Hz/s \n
        */


   boolean                                                        b_multipathEstValid;
        /**< Multipath estimate validity flag\n
                         - 0: Multipath estimate not valid \n
                         - 1: Multipath estimate valid \n
        */
   float                                                        f_multipathEstimate;
        /**< Estimate of multipath in measurement\n
                     - Units: Meters \n
        */
   boolean                                                        b_fineSpeedValid;
        /**< Fine speed validity flag\n
                         - 0: Fine speed not valid \n
                         - 1: Fine speed valid \n
        */
   float                                                        f_fineSpeed;
        /**< Carrier phase derived speed \n
                     - Units: m/s \n
        */
   boolean                                                        b_fineSpeedUncValid;
        /**< Fine speed uncertainty validity flag\n
                         - 0: Fine speed uncertainty not valid \n
                         - 1: Fine speed uncertainty valid \n
        */
   float                                                        f_fineSpeedUnc;
        /**< Carrier phase derived speed \n
                     - Units: m/s \n
        */
   boolean                                                        b_carrierPhaseValid;
        /**< Carrier Phase measurement validity flag\n
                         - 0: Carrier Phase not valid \n
                         - 1: Carrier Phase valid \n
        */
   double                                                        d_carrierPhase;
        /**< Carrier phase measurement [L1 cycles] \n
        */

        boolean                                                  b_cycleSlipCountValid;
         /**< Cycle slup count validity flag\n
                          - 0: Not valid \n
                          - 1: Valid \n
         */

   uint8                                                           u_cycleSlipCount;
   /**< Increments when a CSlip is detected */

   boolean                                                         b_svDirectionValid;
   /**< Validity flag for SV direction */

   float                                                           f_svAzimuth;
   /**< Satellite Azimuth
                        - Units: radians \n
        */

   float                                                           f_svElevation;
    /**< Satellite Elevation
                        - Units: radians \n
        */
} pdsm_LocSVMeasurementStructType;


#define PDSM_EXT_GNSS_MEAS_NUM_SVS             16
/**< Maximum number of satellites in measurement block for given system. */

typedef struct
{
  pdsm_LocSvSystemEnumType                                  e_system;
  /**< Specifies the Satellite System Type
  */
  boolean                                                         b_isSystemTimeValid;
    /**< Indicates whether System Time is Valid:\n
         - 0x01 (TRUE) --  System Time is valid \n
         - 0x00 (FALSE) -- System Time is not valid */
  pdsm_LocSystemTimeStructType                         z_systemTime;
        /**< System Time Information \n
        */

  uint8                u_NumSvs;
  /* Number of SVs in this report block */

  pdsm_LocSVMeasurementStructType                 z_svMeasurement[PDSM_EXT_GNSS_MEAS_NUM_SVS];
  /**< Satellite measurement Information \n
  */
} pdsm_gnss_clockMeasurementStructType;

typedef struct
{
  uint8                                                  u_seqNum;                         /**< Current message Number */
  uint8                                           u_maxMessageNum;         /**< Maximum number of message that will be sent for present time epoch. */

  boolean                                                        b_LeapSecValid;
  pdsm_LocLeapSecondInfoStructType        z_LeapSec;

  pdsm_InterSystemBiasStructType    z_gpsGloInterSystemBias; /* Inter GNSS time bias structure */

  pdsm_InterSystemBiasStructType    z_gpsBdsInterSystemBias; /* Inter GNSS time bias structure */

  pdsm_InterSystemBiasStructType    z_gpsGalInterSystemBias; /* Inter GNSS time bias structure */

  pdsm_InterSystemBiasStructType    z_bdsGloInterSystemBias; /* Inter GNSS time bias structure */

  pdsm_InterSystemBiasStructType    z_galGloInterSystemBias; /* Inter GNSS time bias structure */

  pdsm_InterSystemBiasStructType    z_galBdsInterSystemBias; /* Inter GNSS time bias structure */

  boolean                                                        b_clockFreqValid;
  pdsm_LocRcvrClockFrequencyInfoStructType z_clockFreq;   /* Freq */

  boolean                                                        b_GnssMeasValid;
  pdsm_gnss_clockMeasurementStructType           z_GnssMeas;
} pdsm_pd_ext_gnss_meas_s_type;



typedef struct
{
  uint8    u_Sv;           /* Sv ID */
                        /* GPS: 1-32, GLO: 65-96, 0: Invalid
                           All others are reserved */
  int8    b_FreqNum;      /* Freq index, only valid if u_SysInd is GLO */
  uint16    w_SvPolyFlags;  /* Indicates the validity of data components, see those
                           C_SV_POLY_... defintions above */
  uint8    u_Iode;         /* Ephemeris reference time
                           GPS:Issue of Data Ephemeris used [unitless].
                           GLO: Tb 7-bit, refer to ICD02 */
  double   d_T0;           /* Reference time for polynominal calculations
                           GPS: Secs in week.
                           GLO: Full secs since Jan/01/96 */
  double   pd_xyz0[3];     /* C0X, C0Y, C0Z */
  double   pd_xyzN[9];     /* C1X, C2X ... C2Z, C3Z */
  float   pf_other[4];    /* C0T, C1T, C2T, C3T */
  float   f_PosUnc;       /* SV position uncertainty [m]. */
  float   f_IonoDelay;    /* Ionospheric delay at d_T0 [m]. */
  float   f_IonoDot;      /* Iono delay rate [m/s].  */
  float   f_sbasIonoDelay;/* SBAS Ionospheric delay at d_T0 [m]. */
  float   f_sbasIonoDot;  /* SBAS Iono delay rate [m/s].  */
  float   f_TropoDelay;   /* Tropospheric delay [m]. */
  float   f_Elevation;    /* Elevation [rad] at d_T0 */
  float   f_ElevDot;      /* Elevation rate [rad/s] */
  float   f_ElevUnc;      /* SV elevation [rad] uncertainty */
  double   d_VelCoef[12];  /* Coefficients of velocity poly */
} pdsm_pd_ext_SvPoly_s_type;


typedef struct {
    pdsm_pd_end_e_type        end_status;        /* Session End status */
    pdsm_pd_data_type         pd_data;           /* PD info data */
    pdsm_pd_dload_data_s_type dload_info;        /* information about Data downloaded */
    pdsm_pd_comm_data_s_type  comm_data;         /* Infomration about communication state */
    pdsm_pd_data_ext_s_type   pd_ext_data;       /* Extended PD Info Data */
    pdsm_pd_rand_num_s_type   rand_num_info;    /* Random number update */
    pdsm_pd_crypt_s_type      encrypt_data;     /* Encryption Data */
    qword                     time_stamp;  /* time stamp */
    pdsm_pd_jgps_progress_e_type jgps_prog_status;   /* JGPS progress status */
    pdsm_pd_qwip_data_s_type    qwip_data;         /* Data relevant for qWip */
    pdsm_pd_external_spi_event_info_type  xspi_data; /* external stationary position indicator status */
    locEngInjectWifiApDataIndMsgT_v01  wifi_inj_ack; /* ack to wifi injection */
} pdsm_pd_client_info_s_type;

/* When notified of PD events, clients are given a reference to this structure.
*/
typedef struct pdsm_pd_info_s {

    pdsm_client_id_type         client_id;   /* PDSM originating client */

    pdsm_pd_client_info_s_type  pd_info;     /* Info based on Event set */
} pdsm_pd_info_s_type;



/* ------------------ EXTENDED POSITION STATUS EVENT DATA STRUCTURES ------------------------------*/
typedef struct pdsm_pd_ext_pos_s
{
  pdsm_pd_data_type  pd_data;
  uint32 fixTimeRemaining;    /* Time remaining for the Fix to Timeout in seconds */
} pdsm_pd_ext_pos_s_type;

typedef struct pdsm_ext_use_this_pos_data {
    int32                    lat;
    int32                    lon;
    dword                    time_stamp;
    byte                     loc_uncertainty_ang;
    byte                     loc_uncertainty_a;
    byte                     loc_uncertainty_p;
    pdsm_pd_opt_field        opt_field_mask;       /* Optional fields are valid */
    int16                    altitude;
    word                     heading;
    word                     velocity_hor;
    boolean                  fix_type;
    int8                     velocity_ver;
    byte                     loc_uncertainty_v;
    pdsm_pd_positioning_source_type positioning_source; /*gps/cellid/default */
    pdsm_pd_gps_position_e_type position_type; /* new/invalid */
    int8                      gpsUtcOffset;  
    byte                     loc_uncertainty_conf;
    pdsm_pd_position_mode_e_type      position_mode; /* MS BASED/MSASSISTED etc */
    pdsm_pd_ext_pos_data_s_type           ext_pos_data; /* extended position report. contains some of the above fields with extended accuracy */    
} pdsm_ext_use_this_pos_data_type;


typedef enum
{
  PDSM_PD_EXT_STATUS_MM_ACTIVE_IND,
  PDSM_PD_EXT_STATUS_MM_ABORT_IND,
  PDSM_PD_EXT_STATUS_GUARD_TIMER_EXP,
} pdsm_ext_generic_event_e_type;

typedef union
{
  byte data_active;
  byte data_abort;
  byte guard_timer_expiry;
} pdsm_ext_generic_event_info_u_type;
/*~ IF (_DISC_ == PDSM_PD_EXT_STATUS_MM_ACTIVE_IND) pdsm_ext_generic_event_info_u_type.data_active */
/*~ IF (_DISC_ == PDSM_PD_EXT_STATUS_MM_ABORT_IND) pdsm_ext_generic_event_info_u_type.data_abort */
/*~ IF (_DISC_ == PDSM_PD_EXT_STATUS_GUARD_TIMER_EXP) pdsm_ext_generic_event_info_u_type.guard_timer_expiry */

typedef struct pdsm_pd_ext_generic_ind_s
{
  pdsm_ext_generic_event_e_type event_type;
  pdsm_ext_generic_event_info_u_type event_info;
} pdsm_pd_ext_generic_ind_s_type;
/*~ FIELD pdsm_pd_ext_generic_ind_s.event_info DISC pdsm_pd_ext_generic_ind_s.event_type */

#define  PDSM_PD_EXT_POS_INJ_LAT_LONG_VALID 0x00000001
#define  PDSM_PD_EXT_POS_INJ_POS_UNC_VALID  0x00000002
#define  PDSM_PD_EXT_POS_INJ_TIME_STAMP_VALID 0x00000004

typedef struct pdsm_pd_ext_pos_inj_req_s
{
  uint32 flags; /* validity bits for the fields below., */
  int32  lat;           /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2. 
                           valid if flags has 
                           PDSM_PD_EXT_POS_INJ_LAT_LONG_VALID is set */
  int32  lon;           /* Longitude: LSB = Pi/2^31, Range -Pi to Pi 
                           valid if flags has 
                           PDSM_PD_EXT_POS_INJ_LAT_LONG_VALID set */
  uint32 pos_unc;       /* pos_unc - position uncertainty. valid if flags has 
                           PDSM_PD_EXT_POS_INJ_POS_UNC_VALID dwis set */
  dword timestamp_tow_ms; /* Timestamp of the position in GPS TOW milliseconds.valid if flags 
                             has PDSM_PD_EXT_POS_INJ_TIME_STAMP_VALID set*/
  uint16  timestamp_gps_week;  /* GPS week number of the position. valid if flags has 
                                 PDSM_PD_EXT_POS_INJ_TIME_STAMP_VALID */
} pdsm_pd_ext_pos_inj_req_s_type;

typedef enum {
  PDSM_PD_PERIODIC_PPM_EVT_NONE = -1,        /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_PD_PERIODIC_PPM_EVT_REPORT,           /* PPM data reported              */
  PDSM_PD_PERIODIC_PPM_EVT_SUSPEND,          /* PPM report suspended because of 
                                                OOS/Airplane mode etc.         */
  PDSM_PD_PERIODIC_PPM_EVT_STOPPED,          /* PPM data reporting stopped     */
  PDSM_PD_PERIODIC_PPM_EVT_MAX,              /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_PD_PERIODIC_PPM_EVT_SIZE =0x100000    /* To force enum to int           */ 
} pdsm_pd_periodic_ppm_event_e_type;

typedef enum {
  PPM_RPT_SUSPEND_NONE = -1,
  PPM_RPT_SUSPEND_OOS,
  PPM_RPT_SUSPEND_LPM,
  PPM_RPT_SUSPEND_OTHER,
  PPM_RPT_SUSPEND_MAX,
  PPM_RPT_SUSPEND_SIZE = 0x100000
} pdsm_ppm_rpt_suspend_reason_e_type;

typedef enum {
  PPM_RPT_STOP_NONE = -1,
  PPM_RPT_STOP_USER_TERMINATED,
  PPM_RPT_STOP_OTHER,
  PPM_RPT_STOP_MAX,
  PPM_RPT_STOP_SIZE = 0x100000
} pdsm_ppm_rpt_stop_reason_e_type;

/*
 * Data from last page to be received
 */
typedef struct
{
  uint32  page_time_ms;
  uint16  page_pilot_pn_offset;
  uint8   page_band_class;
  uint16  page_band_freq;
  uint16  page_base_id;
  uint16  page_sid;
  uint16  page_nid;
  boolean stale;
} pdsm_pd_srch_ppm_page_info;

typedef struct
{
  pdsm_pd_srch_ppm_page_info         ppm_page; 
  pdsm_srch_ppm_rpt_type             ppm_meas;
} pdsm_pd_periodic_ppm_rept_s_type;

typedef union {
  pdsm_pd_periodic_ppm_rept_s_type   ppm_data;
  pdsm_ppm_rpt_suspend_reason_e_type suspend_reason;
  pdsm_ppm_rpt_stop_reason_e_type    stop_reason;
} pdsm_pd_periodic_ppm_event_u_type;
/*~ IF (_DISC_ == PDSM_PD_PERIODIC_PPM_EVT_REPORT) pdsm_pd_periodic_ppm_event_u_type.ppm_data */
/*~ IF (_DISC_ == PDSM_PD_PERIODIC_PPM_EVT_SUSPEND) pdsm_pd_periodic_ppm_event_u_type.suspend_reason */
/*~ IF (_DISC_ == PDSM_PD_PERIODIC_PPM_EVT_STOPPED) pdsm_pd_periodic_ppm_event_u_type.stop_reason */
/*~ DEFAULT pdsm_pd_periodic_ppm_event_u_type.void */

typedef struct {
  pdsm_pd_periodic_ppm_event_e_type  event_type;
  pdsm_pd_periodic_ppm_event_u_type  event_info;
} pdsm_pd_ext_periodic_ppm_rpt;
/*~ FIELD pdsm_pd_ext_periodic_ppm_rpt.event_info DISC pdsm_pd_ext_periodic_ppm_rpt.event_type */

typedef enum {
  PDSM_PD_RESET_LOCATION_SERVICE_SUCCESS,
  /*add more hard reset status above this line*/
  PDSM_PD_RESET_LOCATION_SERVICE_ERROR_GENERAL,
  PDSM_PD_RESET_LOCATION_SERVICE_ERROR_MAX = 0x10000000 /* To force enum to 32 bit*/
} pdsm_pd_ext_reset_location_service_status_e_type;

typedef uint64 pds_ext_dpo_disengage_reason_mask_type;
#define PDSM_DPO_DISENGAGE_REASON_SIGNAL_CONDITIONS        0x0000000000000001
#define PDSM_DPO_DISENGAGE_REASON_POSITION_ACCURACY        0x0000000000000002
#define PDSM_DPO_DISENGAGE_REASON_NAV_DATA_NEEDED          0x0000000000000004
#define PDSM_DPO_DISENGAGE_REASON_DPO_DISABLED             0x0000000000000008
#define PDSM_DPO_DISENGAGE_REASON_INSUFF_RESOURCES         0x0000000000000010
#define PDSM_DPO_DISENGAGE_REASON_UNSUPPORTED_MODE         0x0000000000000020
#define PDSM_DPO_DISENGAGE_REASON_UNCALIB_CLOCK            0x0000000000000040
#define PDSM_DPO_DISENGAGE_REASON_HEALTH_DATA_DECODE       0x0000000000000080
#define PDSM_DPO_DISENGAGE_REASON_CLOCK_ACCURACY           0x0000000000000100
#define PDSM_DPO_DISENGAGE_REASON_POSITION_NOT_COMPUTED    0x0000000000000200
#define PDSM_DPO_DISENGAGE_REASON_LTE_B13_TX               0x0000000000000400
#define PDSM_DPO_DISENGAGE_REASON_GSM_TX                   0x0000000000000800
#define PDSM_DPO_DISENGAGE_REASON_UNCALIB_XO               0x0000000000001000

typedef uint32 pds_ext_dpo_status_mask_type;
#define PDSM_DPO_STATUS_ENGAGED        0x00000001   /* DPO is engaged */
#define PDSM_DPO_STATUS_TOW_VALID      0x00000002   /* Timestamp of the status report in GPS TOW milliseconds is valid */

typedef locEngPositionReportMsgT_v01 pdsm_pd_ext_pos_rpt_s_type;

typedef struct {
pds_ext_dpo_status_mask_type dpo_status_mask;  /* Mask to provide information on the DPO status */
dword    timestamp_tow_ms;   /* Timestamp of the status report in GPS TOW milliseconds. Valid if gps_tow_valid set */
uint16   timestamp_gps_week; /* GPS week number of the position. valid if gps_tow_valid set */
pds_ext_dpo_disengage_reason_mask_type dpo_disengage_reason;
} pdsm_pd_ext_dpo_status_rpt;


typedef enum {
  PDSM_PD_NI_CALL_FLOW_TYPE_1X_CP,   /*1X CP call flow*/
  PDSM_PD_NI_CALL_FLOW_TYPE_MAX = 0x10000000 /* To force enum to 32 bit*/
} pdsm_pd_ni_call_flow_type;

typedef enum {
  PDSM_PD_NI_REJECT_REASON_IS801_DECODE_ERROR, /*IS801 Decoding eror*/
  PDSM_PD_NI_REJECT_REASON_IS801_NO_RESOURCES, /*NO Resources*/
  PDSM_PD_NI_REJECT_REASON_MT_GPS_LOCKED,      /*MT GPS is locked*/  
  PDSM_PD_NI_REJECT_REASON_LOCATION_RESET_IN_PROGRESS, /*Location Reset in progress*/
  PDSM_PD_NI_REJECT_REASON_MO_PREF_OVER_MT,   /*MO in progress and NV is set to preffer MO over MT*/
  PDSM_PD_NI_REJECT_REASON_INTERNAL_ERROR, /*Internal Error like null value check, resource issue, invalid state etc*/
  PDSM_PD_NI_REJECT_REASON_MAX = 0x10000000 /* To force enum to 32 bit*/
} pdsm_pd_ni_reject_reason_type;

typedef struct {
  pdsm_pd_ni_call_flow_type ni_call_flow_type;
  pdsm_pd_ni_reject_reason_type ni_reject_reason;
} pdsm_pd_ext_ni_session_reject_report;

typedef enum
{
  PDSM_PD_EXT_STATUS_EMERG_SESS_START,
  PDSM_PD_EXT_STATUS_EMERG_SESS_GSM_CP,
  PDSM_PD_EXT_STATUS_EMERG_SESS_UMTS_CP,
  PDSM_PD_EXT_STATUS_EMERG_SESS_UMTS_UP,
  PDSM_PD_EXT_STATUS_EMERG_SESS_LPP_CP,
  PDSM_PD_EXT_STATUS_EMERG_SESS_LTE_UP
}pdsm_ext_status_emerg_sess_e_type;

typedef struct
{
  pdsm_ext_status_emerg_sess_e_type sess_type;
} pdsm_pd_ext_emerg_status_sess_type;


typedef enum {
  PDSM_PD_SUPL_SESSION_END_RESULT_SUCCESS, /*SUPL Session was succesfull*/
  PDSM_PD_SUPL_SESSION_END_RESULT_FAIL,    /*SUPL Session was a failure*/
  PDSM_PD_SUPL_SESSION_END_RESULT_MAX = 0x10000000 /* To force enum to 32 bit*/
} pdsm_pd_supl_session_end_result;

typedef struct {
  pdsm_pd_supl_session_end_result supl_end_result; /*SUPL Session end result*/
  pdsm_pd_end_e_type supl_end_reason; /*if SUPL session fails. end_reason will carry the reason for failure*/
}pdsm_pd_supl_session_end_ind;

typedef union
{
  pdsm_pd_ext_pos_s_type           ext_pos_report_type;
  pdsm_pd_ext_nmea_s_type          ext_nmea_pos_report_type;
  pdsm_pd_ext_unified_nmea_s_type  ext_unified_nmea_report_type;
  pdsm_pd_ext_meas_s_type          ext_meas_report_type;
  pdsm_pd_ext_gps_state_s_type     ext_gps_state_type;
  pdsm_pd_ext_generic_ind_s_type   ext_generic_event_info;
  pdsm_pd_ext_pos_inj_req_s_type   ext_pos_inj_req_type;
  pdsm_pd_ext_periodic_ppm_rpt     ext_periodic_ppm_report_type;
  pdsm_pd_ext_reset_location_service_status_e_type ext_reset_location_service_status_type;
  pdsm_pd_ext_dpo_status_rpt       ext_dpo_status_report_type;
  pdsm_pd_ext_ni_session_reject_report ext_ni_session_reject_report_type;
  pdsm_pd_supl_session_end_ind     ext_supl_session_end_ind_type;
  pdsm_pd_ext_pos_rpt_s_type         ext_pos_report_qmi_type;
  pdsm_pd_ext_emerg_status_sess_type ext_emerg_sess_type;
  pdsm_pd_ext_SvPoly_s_type                        ext_sv_poly_type;
  pdsm_pd_ext_gnss_meas_s_type       gnss_meas_report_type;
} pdsm_ext_status_info_u_type;
/*~ IF (_DISC_ == PDSM_EXT_STATUS_POS_REPORT) pdsm_ext_status_info_u_type.ext_pos_report_type */
/*~ IF (_DISC_ == PDSM_EXT_STATUS_NMEA_POS_REPORT) pdsm_ext_status_info_u_type.ext_nmea_pos_report_type */
/*~ IF (_DISC_ == PDSM_EXT_STATUS_UNIFIED_NMEA_POS_REPORT) pdsm_ext_status_info_u_type.ext_unified_nmea_report_type */
/*~ IF (_DISC_ == PDSM_EXT_STATUS_MEASUREMENT) pdsm_ext_status_info_u_type.ext_meas_report_type */
/*~ IF (_DISC_ == PDSM_EXT_STATUS_GPS_STATE_INFO) pdsm_ext_status_info_u_type.ext_gps_state_type*/
/*~ IF (_DISC_ == PDSM_EXT_STATUS_GENERIC_EVENT) pdsm_ext_status_info_u_type.ext_generic_event_info */
/*~ IF (_DISC_ == PDSM_EXT_STATUS_POS_INJECT_REQUEST) pdsm_ext_status_info_u_type.ext_pos_inj_req_type */
/*~ IF (_DISC_ == PDSM_EXT_STATUS_PERIODIC_PPM_REPORT) pdsm_ext_status_info_u_type.ext_periodic_ppm_report_type */
/*~ IF (_DISC_ == PDSM_EXT_STATUS_RESET_LOCATION_SERVICE) pdsm_ext_status_info_u_type.ext_reset_location_service_status_type */
/*~ DEFAULT pdsm_ext_status_info_u_type.void */

typedef struct pdsm_ext_status_info_s {
  pdsm_client_id_type client_id;                /* client Id */
  pdsm_ext_status_e_type ext_status_type;       /* event type */
  pdsm_ext_status_info_u_type ext_status_info;  /* event data */
  qword                       time_stamp;       /* timestamp */
} pdsm_ext_status_info_s_type;
/*~ FIELD pdsm_ext_status_info_s.ext_status_info DISC pdsm_ext_status_info_s.ext_status_type */


/*------------------------- eXtended Receiver Assistance (XTRA) -------------------------*/

/* Enumeration of gpsOneXTRA commands
*/
typedef enum pdsm_xtra_cmd_e {

  PDSM_XTRA_CMD_NONE =-1,                 /* FOR INTERNAL USE OF PDSM ONLY! */

  PDSM_XTRA_CMD_SET_DATA,                 /* gpsOneXTRA client is injecting XTRA data to PDAPI */
  PDSM_XTRA_CMD_INITIATE_DOWNLOAD,        /* gpsOneXTRA client is requesting XTRA download */
  PDSM_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS, /* gpsOneXTRA client is setting download parameters */
  PDSM_XTRA_CMD_QUERY_DATA_VALIDITY,      /* gpsOneXTRA client is querying data validity */ 
  PDSM_XTRA_CMD_TIME_INFO_INJ,            /* gpsOneXTRA client is injecting SNTP time information */

  PDSM_XTRA_CMD_MAX,                /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_XTRA_CMD_E_SIZE=0x10000000  /* To force enum to 32 bit for windows NT */
} pdsm_xtra_cmd_e_type;
/*~ SENTINEL pdsm_xtra_cmd_e_type.PDSM_XTRA_CMD_MAX */

/* Enumeration of gpsOneXTRA Client Status
*/
typedef enum pdsm_xtra_dc_status_e {
  PDSM_XTRA_STATUS_FAILURE,
  PDSM_XTRA_STATUS_SUCCESS
} pdsm_xtra_dc_status_e_type;

/* Enumeration of gpsOneXTRA Download status
*/
typedef enum pdsm_xtra_status_e
{
  XTRA_DOWNLOAD_STATUS_SUCCESS,
  XTRA_DOWNLOAD_STATUS_BAD_CRC,
  XTRA_DOWNLOAD_STATUS_BAD_BUFFER_LENGTH,
  XTRA_DOWNLOAD_STATUS_TOA_BAD_RANGE,
  XTRA_DOWNLOAD_STATUS_FAILURE,
  XTRA_DOWNLOAD_STATUS_GPS_BUSY,
  XTRA_DOWNLOAD_STATUS_END_E911,
  XTRA_DOWNLOAD_STATUS_RETRY_EXCEEDED,      /* Ending due to maximum retries have been attempted */
  XTRA_DOWNLOAD_STATUS_DATA_TIMEOUT,        /* Ending due to timeout on waiting for data */
  XTRA_DOWNLOAD_STATUS_FILE_TIME_UNCHANGED,
  XTRA_DOWNLOAD_STATUS_END_RESET_LOCATION_SERVICE, /* Ending due to reset location service */
  XTRA_DOWNLOAD_STATUS_VERSION_MISMATCH,    /* Ending due to XTRA Version mismatch */
  XTRA_DOWNLOAD_STATUS_MAX = 0x10000000
} pdsm_xtra_status_e_type;


/* Enumeration for gpsOneXTRA retrieve command
*/
typedef enum pdsm_xtra_command_e
{
  XTRA_COMMAND_RETRIEVE_FILE,
  XTRA_COMMAND_RETRIEVE_TIME_INFO
} pdsm_xtra_command_e_type;

/* Events for gpsOneXTRA 
*/
typedef int pdsm_xtra_event_type;
#define PDSM_XTRA_EVENT_NONE               0x00
#define PDSM_XTRA_EVENT_DOWNLOAD_REQ       0x01
#define PDSM_XTRA_EVENT_STATUS             0x02
#define PDSM_XTRA_EVENT_TIME_REQ           0x04

#define PDSM_XTRA_EVENT_MASK   (   (PDSM_XTRA_EVENT_DOWNLOAD_REQ) \
                                 | (PDSM_XTRA_EVENT_STATUS) \
                                 | (PDSM_XTRA_EVENT_TIME_REQ) \
                                )

/* Download info field validiy bit flags
*/
#define XTRA_DOWNLOAD_INFO_FLAGS_STATUS             0x00000001
#define XTRA_DOWNLOAD_INFO_FLAGS_COMMAND            0x00000002
#define XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK     0x00000004
#define XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS     0x00000008
#define XTRA_DOWNLOAD_INFO_FLAGS_VALID_DURATION     0x00000010
#define XTRA_DOWNLOAD_INFO_FLAGS_SERVER1            0x00000020
#define XTRA_DOWNLOAD_INFO_FLAGS_SERVER2            0x00000040
#define XTRA_DOWNLOAD_INFO_FLAGS_SERVER3            0x00000080
#define XTRA_DOWNLOAD_INFO_FLAGS_MAX_FILE_PART_SIZE 0x00000100
#define XTRA_DOWNLOAD_INFO_FLAGS_MAX_FILE_SIZE      0x00000200

/* Structure used as payload for gpsOneXTRA events 
** 1. PDSM_XTRA_EVENT_DOWNLOAD_REQ
** 2. PDSM_XTRA_EVENT_STATUS
*/
typedef struct pdsm_xtra_download_info_s
{
  uint32                   flags;        // indicates which fields are valid
  pdsm_xtra_status_e_type  status;       // enum: success, bad crc, xtra data file too old
  pdsm_xtra_command_e_type command;      // enum: retrieve xtra data file from internet
  uint16                   startGpsWeek; // current XTRA info is good starting this GPS week
  uint16                   startGpsMinutes; // current XTRA information is good starting
                                            // this many minutes into the week
  uint16   validDurationHours;           // XTRA info is valid for this many hours
  char     xtra_server_primary[128];     // URL of primary XTRA server
  char     xtra_server_secondary[128];   // URL of secondary (backup) XTRA server
  char     xtra_server_tertiary[128];    // URL of tertiary (backup) XTRA server
  uint32   maxFilePartSize;              // due to RPC limitations, the XTRA file can be xferred
                                         // through PDAPI in parts if the file is too large
  uint32   maxFileSize;                  // max total size of XTRA file
} pdsm_xtra_download_info_s_type;

/* Structure used as payload for gpsOneXTRA events 
** 1. PDSM_XTRA_EVENT_TIME_REQ
*/
typedef struct pdsm_xtra_time_info_req_s
{
  pdsm_xtra_command_e_type command;    // enum: retrieve SNTP time information from internet
  uint32 oneway_delay_failover_thresh; // one-way delay threshold for failover to backup servers
  char xtra_server_primary[128];         // URL of primary XTRA server
  char xtra_server_secondary[128];     // URL of secondary (backup) XTRA server
  char xtra_server_tertiary[128];      // URL of tertiary (backup) XTRA server
} pdsm_xtra_time_info_req_s_type;


/* Union of the gpsOneXTRA download or time data
*/
typedef union pdsm_xtra_req_u
{
  pdsm_xtra_download_info_s_type  xtra_download_info;
  pdsm_xtra_time_info_req_s_type  xtra_time_info;
} pdsm_xtra_req_u_type;
/*~ IF ((_DISC_ == PDSM_XTRA_EVENT_DOWNLOAD_REQ) || (_DISC_ == PDSM_XTRA_EVENT_STATUS)) pdsm_xtra_req_u.xtra_download_info */
/*~ IF (_DISC_ == PDSM_XTRA_EVENT_TIME_REQ)     pdsm_xtra_req_u.xtra_time_info */
/*~ DEFAULT pdsm_xtra_req_u.void */


/* Structure for gpsOneXTRA events and requests
*/
typedef struct pdsm_xtra_info_s 
{
  pdsm_client_id_type   client_id;
  pdsm_xtra_event_type  xtra_event;
  pdsm_xtra_req_u_type  xtra_union;
} pdsm_xtra_info_s_type;
/*~ FIELD pdsm_xtra_info_s.xtra_union DISC pdsm_xtra_info_s.xtra_event */


typedef struct pdsm_xtra_time_info 
{
  /* cummulative GPS time (reference date: Jan 6, 1980) (msec) */
  uint64  d_TimeMsec;       
  uint32  f_TimeUncMsec;    /* Assoc. time uncertainty */
  boolean b_RefToUtcTime;   /* Referenced to UTC/GPS time*/
  boolean b_ForceFlag;      /* Force acceptance of data */
} pdsm_xtra_time_info_type;

/*-------------- End eXtended Receiver Assistance (XTRA) ------------------*/

/*-------------- eXtended Receiver Assistance Terrestrial (XTRA-T) ------------------*/
/* XTRA-T Command Type */
typedef enum pdsm_xtra_t_cmd_e 
{
    PDSM_XTRA_T_CMD_NONE = -1,          /* FOR INTERNAL USE OF PDSM ONLY! */
    PDSM_XTRA_T_CMD_GET_DATA,           /* XTRA-T Client is getting XTRA-T data from PDAPI */
    PDSM_XTRA_T_CMD_SET_DATA,           /* XTRA-T Client is injecting XTRA-T data to PDAPI */
    PDSM_XTRA_T_CMD_INITIATE_DOWNLOAD,  /* Requesting XTRA-T client to download XTRA-T data */
    PDSM_XTRA_T_CMD_INITIATE_UPLOAD,    /* Requesting XTRA-T client to upload XTRA-T data */

    PDSM_XTRA_T_CMD_MAX,                /* FOR INTERNAL USE OF PDSM ONLY! */
    PDSM_XTRA_T_CMD_E_SIZE=0x10000000   /* To force enum to 32 bit for windows NT */
} pdsm_xtra_t_cmd_e_type;
/*~ SENTINEL pdsm_xtra_t_cmd_e_type.PDSM_XTRA_T_CMD_MAX */


/* XTRA-T Event Mask */
typedef int pdsm_xtra_t_event_type;
#define PDSM_XTRA_T_EVENT_NONE                          0x00
#define PDSM_XTRA_T_EVENT_SESS_BEGIN                    0x01  /*  GPS engine initiates  XTRA-T */
#define PDSM_XTRA_T_EVENT_DATA                          0x02  /*  GPS engine XTRA-T data transfer */
#define PDSM_XTRA_T_EVENT_SESS_DONE                     0x04  /*  GPS engine requests a termination of XTRA-T session */
#define PDSM_XTRA_T_EVENT_SESS_END                      0x08  /*  GPS engine XTRA-T  session abort due to errors */
#define PDSM_XTRA_T_EVENT_DOWNLOAD_REQ                  0x10  /* GPS engine initiates XTRA-T download  - Aries 5.0 */ 
#define PDSM_XTRA_T_EVENT_UPLOAD_REQ                    0x20  /* GPS engine initiates XTRA-T upload  - Aries 5.0 */ 

#define PDSM_XTRA_T_EVENT_MASK (PDSM_XTRA_T_EVENT_SESS_BEGIN | PDSM_XTRA_T_EVENT_DATA | PDSM_XTRA_T_EVENT_SESS_DONE \
                                 | PDSM_XTRA_T_EVENT_SESS_END | PDSM_XTRA_T_EVENT_DOWNLOAD_REQ | PDSM_XTRA_T_EVENT_UPLOAD_REQ)

/* XTRA-T payload for PDSM_XTRA_T_EVENT_SESS_END: data part report to XTRA-T client */
typedef struct pdsm_xtra_t_get_celldb_report_s 
{
    uint32           xtra_t_data_len;  /* Length of XTRA-T data */
    unsigned char   *xtra_t_data_ptr;  /* XTRA-T Data Transfer Report or DATA_TRANSFER_REPORT */
    uint8            part_number;      /* Part number n of "total_parts" being sent by the application */
    uint8            total_parts;      /* Total number of parts that the cell database data was broken into for the report */
} pdsm_xtra_t_get_celldb_report_s_type;
/*~ FIELD pdsm_xtra_t_get_celldb_report_s.xtra_t_data_ptr POINTER */


/* XTRA-T payload for PDSM_XTRA_T_EVENT_DATA: XTRA-T session end reason */
/* This is also used for sending status of individual Segmented CellDB message to TM XTRA-T Module */
typedef enum pdsm_xtra_t_status_e 
{
  PDSM_XTRA_T_END_NONE    = -1,
  PDSM_XTRA_T_END_SUCCESS = 0,       /* XTRA-T session success */
  PDSM_XTRA_T_END_BUSY,
  PDSM_XTRA_T_END_OTHER,
  PDSM_XTRA_T_END_NO_CLIENT,
  PDSM_XTRA_T_END_FEATURE_NOT_ENABLED,
  PDSM_XTRA_T_END_BAD_BUFFER_LENGTH,
  PDSM_XTRA_T_END_INTERNAL_RESOURCE,
  PDSM_XTRA_T_END_CLIENT_REPORT_ERR,
  PDSM_XTRA_T_END_DTR_PROC_ERR,
  PDSM_XTRA_T_END_MAX,               /* FOR INTERNAL USE OF PDSM ONLY! */
  PDSM_XTRA_T_END_E_SIZE=0x10000000  /* To force enum to 32 bit for windows NT */
} pdsm_xtra_t_status_e_type;


/*
** TM XTRA-T Session Type
*/
typedef enum{
  PDSM_XTRA_T_SESSION_NONE       = 0,  /* Invalid session type                             */
  PDSM_XTRA_T_SESSION_UPLOAD_DTR,      /* Upload Session using Cell DB data in DTR packets */
  PDSM_XTRA_T_SESSION_MAX,
  PDSM_XTRA_T_SESSION_E_SIZE     = 0x10   /* Force to 1 byte */
} pdsm_xtra_t_session_e_type;


/* XTRA-T Event Callback: payload definition */
typedef union pdsm_xtra_t_req_u
{
 /* event: PDSM_XTRA_T_EVENT_DATA  */
 /* payload: data part report to XTRA-T client */
 pdsm_xtra_t_get_celldb_report_s_type             data_report;

/* event:  PDSM_XTRA_T_EVENT_SESS_END */
/* payload: XTRA-T session end reason */
 pdsm_xtra_t_status_e_type                        end_reason;    
} pdsm_xtra_t_req_u_type;
/*~ IF (_DISC_ == PDSM_XTRA_T_EVENT_DATA) pdsm_xtra_t_req_u.data_report */
/*~ IF (_DISC_ == PDSM_XTRA_T_EVENT_SESS_END)     pdsm_xtra_t_req_u.end_reason */
/*~ DEFAULT pdsm_xtra_t_req_u.void */

/* XTRA-T Event Callback Data: event + payload */
typedef struct pdsm_xtra_t_info_s
{
  pdsm_client_id_type        client_id;
  pdsm_xtra_t_event_type     xtra_t_event;           /* event in the callback */
  pdsm_xtra_t_req_u_type     xtra_t_event_payload;   /* event payload in the callback */
} pdsm_xtra_t_info_s_type;
/*~ FIELD pdsm_xtra_t_info_s.xtra_t_event_payload DISC pdsm_xtra_t_info_s.xtra_t_event */


/* XTRA-T data upload reason (Aries 5.0) */
typedef enum pdsm_xtra_t_upload_reason_type_e 
{
  PDSM_XTRA_T_UPLOAD_REASON_TYPE_NONE  = -1,         /* For internal use only */
  /* TBC in Aries 5.0  */
  PDSM_XTRA_T_UPLOAD_REASON_TYPE_MAX,                /* For internal use */
  PDSM_XTRA_T_UPLOAD_REASON_TYPE_E_SIZE = 0x10000000 /* Fix the enum as int */
} pdsm_xtra_t_upload_reason_type_e_typ;


/* XTRA-T:  pdsm_xtra_t_get_data() related */
typedef enum 
{
   PDSM_XTRA_T_GET_ALL,                  /* All cells in CellDB */
   PDSM_XTRA_T_GET_ALL_CHANGED,          /* All changed cells in CellDB (changed = dirty bit is ON) */
   PDMS_XTRA_T_GET_CHANGED_IN_N_MINUTES, /* Changed cells in CellDB for the last N minutes */
   PDSM_XTRA_T_GET_LAST_N_CHANGED,      /* The last N changed cells in CellDB  */
   PDSM_XTRA_T_GET_MAX,                  /* FOR INTERNAL USE OF PDSM ONLY! */
   PDSM_XTRA_T_GET_E_SIZE=0x10000000    /* To force enum to 32 bit for windows NT */ 
} pdsm_xtra_t_get_e_type ;         

/* XTRA-T:  pdsm_xtra_t_get_data() related */
typedef struct 
{
  uint32                    changed_info_type;   /* Used for passing the value of "N" in these get requests:
                                                    PDMS_XTRA_T_GET_CHANGED_IN_N_MINUTES
                                                    PDSM_XTRAT_T_GET_LAST_N_CHANGED 
                                                    N > 0
                                                 */
  pdsm_xtra_t_get_e_type    get_type;
} pdsm_xtra_t_get_s_type;
/*-------------- End eXtended Receiver Assistance Terrestrial (XTRA-T) ------------------*/

/*-------------- Start (WIPER) ------------------*/

/* WIPER valid information flag in log report */
#define WIPER_LOG_TIME_VALID 0x01
#define WIPER_LOG_POS_VALID 0x02
#define WIPER_LOG_AP_SET_VALID 0x04

/* General WIPER defines */
#define WIPER_MAC_ADDR_LENGTH  6              // Do not change this number since it affects RPC and log packet sizes
#define WIPER_MAX_REPORTED_APS_PER_LOG_MSG 50 // Do not change this number since it affects RPC and log packet sizes
#define WIPER_SSID_STR_LENGTH 32

/* WIPER AP Qualifier */
#define WIPER_AP_QUALIFIER_BEING_USED 0x1  /* AP is being used by WPS */
#define WIPER_AP_QUALIFIER_HIDDEN_SSID 0x2 /* AP does not broadcast SSID */
#define WIPER_AP_QUALIFIER_PRIVATE 0x4     /* AP has encryption turned on */
#define WIPER_AP_QUALIFIER_INFRASTRUCTURE_MODE 0x8     /* AP is in infrastructure mode and not in ad-hoc/unknown mode */

/* Enum for WIPER command errors
*/
typedef enum pdsm_wiper_cmd_err_e 
{
  PDSM_WIPER_CMD_ERR_NONE = -1,  
  PDSM_WIPER_CMD_ERR_NOERR,
  PDSM_WIPER_CMD_ERR_CLIENT_ID_P,
  PDSM_WIPER_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S,
  PDSM_WIPER_CMD_ERR_OTHER
} pdsm_wiper_cmd_err_e_type;

/* Enumeration of WIPER commands 
*/
typedef enum pdsm_wiper_cmd_e 
{
  PDSM_WIPER_CMD_NONE = -1,
  PDSM_WIPER_CMD_LOG_POSITION_REPORT,
  PDSM_WIPER_CMD_MAX,
  PDSM_WIPER_CMD_E_SIZE=0x10000000
} pdsm_wiper_cmd_e_type;
/*~ SENTINEL pdsm_wiper_cmd_e_type.PDSM_WIPER_CMD_MAX */

/* For fix or scan time information */
typedef struct {
  uint32  slow_clock_count;
} t_wiper_fix_time_struct_type;

/* For fix information */
typedef struct {
  int32   lat;
  int32   lon;
  uint16  HEPE;
  uint8   num_of_aps_used;
  uint8   fix_error_code;   
} t_wiper_fix_pos_struct_type;

/* The minimum and maximum frequencies for the 2.4 GHz WLAN band */
#define PDSM_WLAN_CHANNEL_2_4_GHZ_FREQ_MIN 2412
#define PDSM_WLAN_CHANNEL_2_4_GHZ_FREQ_MAX 2484

/* The minimum and maximum frequencies for the 5 GHz WLAN band */
#define PDSM_WLAN_CHANNEL_5_GHZ_FREQ_MIN 4915
#define PDSM_WLAN_CHANNEL_5_GHZ_FREQ_MAX 5825

/* For WiFi AP information */
typedef struct {  
    uint8           mac_addr[WIPER_MAC_ADDR_LENGTH];  // Associated MAC Address
    int32           rssi;
    uint16          channel;                          // Channel on which beacon was recieved.
    uint8           ap_qualifier;                     // A bit mask of boolean qualifiers for AP.    
    uint32          freq;                             // The frequency at which node is detected  
    char            ssid[WIPER_SSID_STR_LENGTH];
} t_wiper_ap_info_struct_type;

/* This structure is used to provide the WLAN channel info */
typedef struct
{
  /* Each bit in the mask represents the channel number for a particular frequency in the 2.4Ghz band.
     Bit 0 : Channel 1 - 2412 Mhz
     Bit 1 : Channel 2 - 2417 Mhz
     Bit 2 : Channel 3 - 2422 Mhz
     Bit 3 : Channel 4 - 2427 Mhz
     Bit 4 : Channel 5 - 2432 Mhz
     Bit 5 : Channel 6 - 2437 Mhz
     Bit 6 : Channel 7 - 2442 Mhz
     Bit 7 : Channel 8 - 2447 Mhz
     Bit 8 : Channel 9 - 2452 Mhz
     Bit 9 : Channel 10 - 2457 Mhz
     Bit 10 : Channel 11 - 2462 Mhz
     Bit 11 : Channel 12 - 2467 Mhz
     Bit 12: Channel 13 - 2472 Mhz
     Bit 13 : Channel 14 - 2484 Mhz   
  */
  uint32 q_2_4_Ghz_channel_mask;
}t_wlan_channel_info_type;
/* For AP scan list information */
typedef struct {
  uint8   num_of_aps;
  t_wiper_ap_info_struct_type ap_info[WIPER_MAX_REPORTED_APS_PER_LOG_MSG];
  pdsm_wifi_scan_src_e_type e_wifi_scan_src;
  t_wlan_channel_info_type    z_wlan_channel_info;
} t_wiper_ap_set_struct_type;

/* For log results information - any one of the following is populated and indicated by wiper_valid_info_flag:
   wiper_fix_time - Time of WiFi position fix
   wiper_fix_position - When Wi-Fi fix is available.
   wiper_ap_set - When WiFi AP scan list is available or for informaton of APs being used in the fix above.
*/
typedef struct {
  uint8                                     wiper_valid_info_flag; 
  t_wiper_fix_time_struct_type              wiper_fix_time;
  t_wiper_fix_pos_struct_type               wiper_fix_position;
  t_wiper_ap_set_struct_type                wiper_ap_set;
} t_wiper_position_report_struct_type;

typedef t_wiper_position_report_struct_type * p_wiper_position_report_struct_type;

typedef struct t_wiper_position_report_ex {
  t_wiper_position_report_struct_type wiper_pos_report;
  pdsm_pd_reliability_e_type e_HoriRelIndicator;        /*horizontal reliability*/
  pdsm_pd_reliability_e_type e_VertRelIndicator;        /*vertical reliability*/

} t_wiper_position_report_ex_struct_type;

typedef t_wiper_position_report_ex_struct_type * p_wiper_position_report_ex_struct_type;

/*-------------- End (WIPER) ------------------*/

/*---------------------------------------------------------------------------
		      QMI Services Enable Type
---------------------------------------------------------------------------*/

typedef uint32 pdsm_QMIServiceEnableMaskType;
/* definitions for QMI service configuration */
#define PDSM_QMI_CONFIG_ENABLE_LOC           (0x00000001)
#define PDSM_QMI_CONFIG_ENABLE_PDS           (0x00000002)

typedef struct
{
  pdsm_QMIServiceEnableMaskType enableMask;
} pdsm_QMIServiceConfig;

/*-------------- End QMI Services Enable Type ------------------*/

/* <EJECT> */
/*---------------------------------------------------------------------------
                      CLIENT RELATED TYPES & FUNCTIONS
---------------------------------------------------------------------------*/

/* Types for PDSM event callback functions.
**
** A client that want to be notified of PD events needs to register
** with the appropriate type of callback function with PDSM.
**
** PDSM calls on the client's callback functions when events to which the
** client is registered for occur. Each client is responsible for the
** implementation of its callback functions. Note that since callback
** functions are being called from PDSM context they are expected to
** only queue commands to their corresponding client and return. No extra
** processing is allowed and in particular no BLOCKING OS calls 
** like os_WaitLocalSignal().
**
** NOTE! callback functions are not allowed to change the content
** of the info structure which they are given a pointer.
**
** NOTE! clients are not allowed to access the info structure after
** the their callback functions return.
*/


/* Type for PD event callback function.
**
** A client that want to be notified of PD events needs to register
** with such a function with PDSM.
*/

typedef void (pdsm_pd_event_f_type) (
    void                         *user_data,  /* user data pointer */

    pdsm_pd_event_type           pd_event,    /* Indicate PD event */

    const pdsm_pd_info_s_type    *pd_info_ptr /* A pointer to a PD state information struct */
    /*~ PARAM pd_info_ptr POINTER */

);

/* Type for PD event callback function.
**
** A client that want to be notified of PD events needs to register
** with such a function with PDSM.
*/

typedef void (pdsm_pa_event_f_type) (
    void                         *user_data,  /* user data pointer */

    pdsm_pa_event_type           pa_event,    /* Indicate PD event */

    const pdsm_pa_info_s_type    *pa_info_ptr /* A pointer to a PD state information struct */
    /*~ PARAM pa_info_ptr POINTER */

);

/* After PA event max-out, this is the new PA event-reporting function, no need to define new PA bitmask-style
*  event for each new PDSM_PA_xxx enum, the enum will be used here directly. client will not be able to register
*  certain events selectively, all PA events will be reported to registered PA client
*/
typedef void (pdsm_pa_event_ex_f_type) (
                                       void                         *user_data,  /* user data pointer */

                                       pdsm_pa_e_type               pa_event,    /* Indicate PA event, PDSM_PA_xxx enum */

                                       const pdsm_pa_info_s_type    *pa_info_ptr, /* A pointer to a PD state information struct */
                                       /*~ PARAM pa_info_ptr POINTER */

                                       pdsm_pa_cmd_err_e_type        status    /* Optional field to indicate success/failure status of PA operation */
                                       );

typedef void (pdsm_lcs_event_f_type) (
  void *user_data,
  pdsm_lcs_event_type  lcs_event,
  const pdsm_lcs_info_s_type *lcs_info_ptr
  /*~ PARAM lcs_info_ptr POINTER */
);


typedef void (pdsm_ext_status_event_f_type) (
   void   *data_block_ptr,
   pdsm_ext_status_event_type  ext_status_cmd,
   const pdsm_ext_status_info_s_type *ext_status_info_ptr
  /*~ PARAM ext_status_info_ptr POINTER */
);


typedef void (pdsm_xtra_event_f_type) (
   void *user_data,
   pdsm_xtra_event_type  xtra_event,
   const pdsm_xtra_info_s_type *xtra_info_ptr
   /*~ PARAM xtra_info_ptr POINTER */
);


/* Type for XTRA-T event callback function */
typedef void (pdsm_xtra_t_event_f_type) (
   void                          *user_data,
   pdsm_xtra_t_event_type         xtra_t_event,     /* See XTRA-T event mask */
   const pdsm_xtra_t_info_s_type *xtra_t_info_ptr   /* see XTRA-T event info */
  /*~ PARAM xtra_t_info_ptr POINTER */
);


/* Type for PD command error callback function.
**
** A client that want to be notified of OTHER clients PD command errors
** needs to register with such a function with PDSM.
*/
typedef void (pdsm_pd_cmd_err_f_type) (

    pdsm_client_id_type            client_id,
        /* Indicate which client caused the error */

    pdsm_pd_cmd_e_type             cmd,
        /* Indicate for which command this error is reported */

    pdsm_pd_cmd_err_e_type         cmd_err
        /* Indicate nature of error */

);

/* Type for PD command error callback function.
**
** A client that want to be notified of OTHER clients PD command errors
** needs to register with such a function with PDSM.
*/
typedef void (pdsm_pa_cmd_err_f_type) (

    pdsm_client_id_type         client_id,
        /* Indicate which client caused the error */

    pdsm_pa_cmd_e_type             cmd,
        /* Indicate for which command this error is reported */

    pdsm_pa_cmd_err_e_type         cmd_err
        /* Indicate nature of error */

);

/* Type for PD command error callback function.
**
** A client that want to be notified of OTHER clients PD command errors
** needs to register with such a function with PDSM.
*/
typedef void (pdsm_lcs_cmd_err_f_type) (

    pdsm_client_id_type         client_id,
        /* Indicate which client caused the error */

    pdsm_lcs_cmd_e_type             cmd,
        /* Indicate for which command this error is reported */

    pdsm_lcs_cmd_err_e_type         cmd_err
        /* Indicate nature of error */

);

typedef void (pdsm_ext_status_cmd_err_f_type) (

    pdsm_client_id_type         client_id,
        /* Indicate which client caused the error */

    pdsm_ext_pos_cmd_e_type             cmd,
        /* Indicate for which command this error is reported */

    pdsm_ext_pos_cmd_err_e_type         cmd_err
        /* Indicate nature of error */

);

typedef void (pdsm_xtra_cmd_err_f_type) (

   pdsm_client_id_type         client_id,
        /* Indicate which client caused the error */

   pdsm_xtra_cmd_e_type             cmd,
       /* Indicate for which command this error is reported */

   pdsm_xtra_cmd_err_e_type         cmd_err
       /* Indicate nature of error */
);


typedef void (pdsm_xtra_t_cmd_err_f_type) (

   pdsm_client_id_type         client_id,
        /* Indicate which client caused the error */

   pdsm_xtra_t_cmd_e_type             cmd,
       /* Indicate for which command this error is reported */

   pdsm_xtra_t_cmd_err_e_type         cmd_err
       /* Indicate nature of error */
);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of client API return status*/

typedef enum pdsm_client_status_e {

    PDSM_CLIENT_ERR_NONE=-1,          /* FOR INTERNAL USE OF PDSM ONLY! */

    PDSM_CLIENT_OK,                   /* No error, request was completed successfully */

    PDSM_CLIENT_ERR_CLIENT_PTR,        /* Invalid client ID pointer*/

    PDSM_CLIENT_ERR_CLIENT_ID,        /* Invalid client ID */

    PDSM_CLIENT_ERR_CLIENT_TYPE,      /* Invalid Client type */

    PDSM_CLIENT_ERR_EVENT_REG_TYPE,   /* Invalid event registration type */

    PDSM_CLIENT_ERR_EVENT_MASK,       /* Invalid event mask */

    PDSM_CLIENT_ERR_CLIENT_INIT_FAIL, /* Unable to initialize the client */

    PDSM_CLIENT_ERR_SESSION_ACTIVE,   /* Unable to release a client due to active session */

    PDSM_CLIENT_ERR_CLIENT_ACTIVE,    /* Unable to release an active client */

    PDSM_CLIENT_ERR_MAX,              /* FOR INTERNAL USE OF PDSM ONLY! */

    PDSM_CLIENT_ERR_E_SIZE=0x10000000    /* To force enum to 32 bit for windows NT */

} pdsm_client_status_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of client event registration type */

typedef enum pdsm_client_event_reg_e {

    PDSM_CLIENT_EVENT_NONE=-1,          /* FOR INTERNAL USE OF PDSM ONLY! */

    PDSM_CLIENT_EVENT_REG,              /* event registration */

    PDSM_CLIENT_EVENT_DEREG,            /* event de-registration */

    PDSM_CLIENT_EVENT_MAX,              /* FOR INTERNAL USE OF PDSM ONLY! */

    PDSM_CLIENT_EVENT_BIG=0x10000000    /* To force enum to int */

} pdsm_client_event_reg_e_type;

/* Typedef structure pdsm_session_stat_type is used to store session statisctics.
*/
typedef struct pdsm_session_stat_s {
  word         num_sessions;            /* Num of session started */
  word         num_succ_sessions;       /* Num of Successful Sessions */
  word         sessions_aborted;        /* Num of sessions aborted by Phone */
  word         worst_time_to_fix;       /* Worst time to provide Position Fix */
  dword        total_time_to_fix;       /* Total time to Fix */
  word         num_aa_gets;             /* Num of times AA data is requested from PDE */
  word         num_sa_gets;             /* Num of times SA data is requested from PDE */
  word         num_multiple_sa_gets;    /* Num of times multiple SA gets in one Session */
  word         num_tune_away;           /* Number of times Searcher tuned away to GNSS */
  word         avg_session_tune_away;   /* Average tune away time per session */
  word         num_100ms;               /* Num of times GNSS searches done within 100 ms */
  word         num_500ms;               /* num of times GNSS searches done within 500 ms */
  word         num_2100ms;              /* Num of times GNSS searches done within 2.1 secs */
  word         avg_error;               /* Average Position Error */
} pdsm_session_stat_type;

/* Typdef enum pd_is801_type used by DIAG to send IS801 packets to PDSM.
*/
typedef enum {
  PD_IS801_TYPE_NONE,
  PD_IS801_TYPE_AA,                     /* Aquisition data */
  PD_IS801_TYPE_SA,                     /* Sensitivity Assistance: Not used : */
  PD_IS801_TYPE_MAX
} pd_is801_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*
	This structure contains the options for and end request. 

	If the receiver off is set to false, then if no session is 
	running and receiver is ON due to extended ON feature or due 
	to LM apptracking feature, the receiver wont be turned off. 

	If the boolean was set, it will result in sending receiver off
	to MGP.
*/
typedef struct{
 boolean receiver_off; /*Turn the receiver off*/
} pdsm_pd_end_session_opt_s_type;
/*===========================================================================

FUNCTION pdsm_client_init

DESCRIPTION
  Initializing a client object.

  This function must be called exactly once on a client object before
  it is being used, in any way, place, or form. If this function is called
  more that once the same data structure get reassigned and reinitialized.

  Note that in order to get notified of PDSM events a client still needs
  to do the following:
  1. Register its callback functions and specific events with each functional group
  2. Activate its client object with pdsm_client_act()

DEPENDENCIES
  none

RETURN VALUE
  Assigned Handle (client id) to be used for interfacing with PDSM.

SIDE EFFECTS
  none

===========================================================================*/
extern pdsm_client_id_type pdsm_client_init(

    pdsm_client_type_e_type           client_type
        /* Type of client */
);
/*~ FUNCTION pdsm_client_init 
    RELEASE_FUNC pdsm_client_release(_RESULT_) */

/*===========================================================================

FUNCTION pdsm_client_release

DESCRIPTION
  Release a client object.

  This function should be called if a client doesn't need PDSM anymore.
  This will release the client object. That object can be reused for
  another client. If the client needs to use PDSM again, it should PD
  pdsm_client_init function to establish connection.

DEPENDENCIES
  none

RETURN VALUE
  request staus

SIDE EFFECTS
  none

===========================================================================*/
extern pdsm_client_status_e_type pdsm_client_release(

    pdsm_client_id_type               client_id
        /* Client ID pointer*/
);
/*~ FUNCTION pdsm_client_release */


/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_pd_reg

DESCRIPTION
  This function allows a client to register the following PD-related
  callbacks and events with PDSM:

   1. Callback function to receive registered PD events
   2. Register and de-register PD events
   3. Callback function to receive other clients' PD-command errors.

   If a client registers a callback function to receive PD event
   notification, it also needs to register one or more PD events it is
   interested in receiving.

  If a client registers a callback function to receive other clients'
  PD-command errors, it will receive notifications on all possible
  PD-command errors caused by other clients. The NULL function pointer can
  also be used for the callback function if a client is not interested in
  receiving that callback. For example, if a client is not interested in
  receiving other clients' PD-command errors, it can set
  call_cmd_err_func = NULL. However, if the call_event_func is set to NULL,
  all registered PD events are also de-registered and the event_mask_type
  parameters are ignored.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_pd_reg(

    pdsm_client_id_type           client_id,
        /* Requesting client */
    void                          *client_data_ptr,

    pdsm_pd_event_f_type          *pd_event_func,
        /* Pointer to a callback function to notify the client of PD
        ** events */
    /*~ PARAM   pd_event_func POINTER */      

    pdsm_client_event_reg_e_type  event_reg_type,

    pdsm_pd_event_type            event_mask,
        /* register from this event (including) */

    pdsm_pd_cmd_err_f_type        *pd_cmd_err_func
      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
    /*~ PARAM pd_cmd_err_func POINTER */
);
/*~ FUNCTION pdsm_client_pd_reg */

/*===========================================================================

FUNCTION pdsm_client_pa_reg

DESCRIPTION
  This function allows a client to register the following Parameter-related
  callbacks and events with PDSM:

   1. Callback function to receive registered Parameter events
   2. Register and de-register PArameter write events events
   3. Callback function to receive other clients' PD-command errors.

  Event reporting in this function only works for PA operation upto & including
  PDSM_PA_LPP_CONFIG_INFO. Afterwards, client should call pdsm_client_pa_reg_ex()
  to register for PA events
 
DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_pa_reg(

    pdsm_client_id_type              client_id,
        /* Requesting client */
    void                             *client_data_ptr,

    pdsm_pa_event_f_type             *pa_event_func,
        /* Pointer to a callback function to notify the client of PA
        ** events */
    /*~ PARAM pa_event_func POINTER */
    pdsm_client_event_reg_e_type     event_reg_type,

    pdsm_pa_event_type               event_mask,
        /* register from this event (including) */

    pdsm_pa_cmd_err_f_type           *pa_cmd_err_func
      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
      /*~ PARAM pa_cmd_err_func POINTER */ 
);
/*~ FUNCTION pdsm_client_pa_reg */


/*===========================================================================

FUNCTION pdsm_client_pa_reg_ex

DESCRIPTION
  This function allows a client to register the following Parameter-related
  callbacks with PDSM:

   1. Callback function to receive Parameter events, all PA events will be
      reported, client will not be able to selectively register certain
      events.
   2. Register and de-register Parameter events
   3. Callback function to receive other clients' PD-command errors.

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  If client calls this function to register PA events, all PA events 
  will be reported via the registered callback function "pa_event_func_ex", 
  including all PA operations upto & including PDSM_PA_LPP_CONFIG_INFO. 

  If client also calls legacy pdsm_client_pa_reg() function to register for the
  old bitmask-style PA event reporting, client will get event via the registered
  callback function "pa_event_func" only for PA operation upto & including
  PDSM_PA_LPP_CONFIG_INFO (client will get event twice in this case, client
  needs to properly handle such case).
 
  If client register using both methods (pdsm_client_pa_reg() & pdsm_client_pa_reg_ex())
  it should also call both legacy & new interface to deregister completely.

===========================================================================*/
pdsm_client_status_e_type pdsm_client_pa_reg_ex(

    pdsm_client_id_type              client_id,
        /* Requesting client */
    void                             *client_data_ptr,

    pdsm_pa_event_ex_f_type          *pa_event_func_ex,
        /* Pointer to a callback function to notify the client of PA
        ** events */
    /*~ PARAM pa_event_func_ex POINTER */
    pdsm_client_event_reg_e_type     event_reg_type,

    pdsm_pa_cmd_err_f_type           *pa_cmd_err_func
      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
      /*~ PARAM pa_cmd_err_func POINTER */ 
);
/*~ FUNCTION pdsm_client_pa_reg_ex */

/*===========================================================================

FUNCTION pdsm_client_lcs_reg 

DESCRIPTION
This function allows a client to register the following UI-related
callbacks and events with PDSM:

DEPENDENCIES
Client must have already been initialized with pdsm_client_init().

RETURN VALUE
request status

SIDE EFFECTS
none

===========================================================================*/
extern pdsm_client_status_e_type pdsm_client_lcs_reg(

    pdsm_client_id_type           client_id,
        /* Requesting client */
    void                          *client_data_ptr,

    pdsm_lcs_event_f_type          *lcs_event_func,
        /* Pointer to a callback function to notify the client of PD
        ** events */
    /*~ PARAM lcs_event_func POINTER */
    pdsm_client_event_reg_e_type  event_reg_type,

    pdsm_lcs_event_type            event_mask,
        /* register from this event (including) */

    pdsm_lcs_cmd_err_f_type        *lcs_cmd_err_func
      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
      /*~ PARAM lcs_cmd_err_func POINTER */
);
/*~ FUNCTION pdsm_client_lcs_reg */


/*===========================================================================

FUNCTION pdsm_client_xtra_reg 

DESCRIPTION
This function allows a client to register the following UI-related
callbacks and events with PDSM:

DEPENDENCIES
Client must have already been initialized with pdsm_client_init().

RETURN VALUE
request status

SIDE EFFECTS
none

===========================================================================*/
extern pdsm_client_status_e_type pdsm_client_xtra_reg
(
  pdsm_client_id_type           client_id,
  /* Requesting client */
  void                          *client_data_ptr,

  pdsm_xtra_event_f_type          *xtra_event_func,
  /* Pointer to a callback function to notify the client of XTRA events */
  /*~ PARAM xtra_event_func POINTER */
  pdsm_client_event_reg_e_type  event_reg_type,

  pdsm_xtra_event_type            event_mask,
  /* register from this event (including) */

  pdsm_xtra_cmd_err_f_type        *xtra_cmd_err_func
  /* pointer to a callback function to notify the client of XTRA
  ** command errors of OTHER clients */
  /*~ PARAM xtra_cmd_err_func POINTER */
);
/*~ FUNCTION pdsm_client_xtra_reg */

/*===========================================================================

FUNCTION pdsm_client_xtra_t_reg 

DESCRIPTION
This function allows a XTRA-T client to register the following UI-related
callbacks and events with PDSM:

DEPENDENCIES
Client must have already been initialized with pdsm_client_init().

RETURN VALUE
request status

SIDE EFFECTS
none

===========================================================================*/
extern pdsm_client_status_e_type pdsm_client_xtra_t_reg
(
  pdsm_client_id_type           client_id,
  /* Requesting client */
  void                          *client_data_ptr,

  pdsm_xtra_t_event_f_type          *xtra_t_event_func,
  /* Pointer to a callback function to notify the client of PD
  ** events */
  /*~ PARAM xtra_t_event_func POINTER */
  
  pdsm_client_event_reg_e_type  event_reg_type,
  /* Register or deregister events */
  
  pdsm_xtra_t_event_type            xtra_t_event_mask,
  /* register from this event (including) */
  
  pdsm_xtra_t_cmd_err_f_type        *xtra_t_cmd_err_func
  /* pointer to a callback function to notify the client of XTRA-T  
  ** command errors of OTHER clients */
  /*~ PARAM xtra_t_cmd_err_func POINTER */
);
/*~ FUNCTION pdsm_client_xtra_t_reg */
/* <EJECT> */


/*===========================================================================

FUNCTION pdsm_client_ext_status_reg

DESCRIPTION
  This function allows a client to register the following extended status-related
  callbacks and events with PDSM:

   1. Callback function to receive registered extended status events

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  none

===========================================================================*/
pdsm_client_status_e_type pdsm_client_ext_status_reg(

    pdsm_client_id_type              client_id,
        /* Requesting client */
    void                             *client_data_ptr,

    pdsm_ext_status_event_f_type             *ext_status_event_func,
        /* Pointer to a callback function to notify the client of ext status
        ** events */
    /*~ PARAM ext_status_event_func POINTER */
    pdsm_client_event_reg_e_type     event_reg_type,

    pdsm_ext_status_event_type               event_mask,
        /* register from this event (including) */
    pdsm_ext_status_cmd_err_f_type        *ext_status_cmd_err_func
      /* pointer to a callback function to notify the client of PD
      ** command errors of OTHER clients */
      /*~ PARAM ext_status_cmd_err_func POINTER */
);
/*~ FUNCTION pdsm_client_ext_status_reg */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_act

DESCRIPTION
  Activate the client object, so that the client will be notified
  of postion determination session manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
   request status

SIDE EFFECTS
  Add the client object to the client list.

===========================================================================*/
extern pdsm_client_status_e_type pdsm_client_act(

    pdsm_client_id_type               client_id
        /* Requesting client */
);
/*~ FUNCTION pdsm_client_act */

/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_client_deact

DESCRIPTION
  Deactivate a client object, so that the client will NOT be notified
  of Position Determination Session Manager events (for which it is registered).

DEPENDENCIES
  Client must have already been initialized with pdsm_client_init().

RETURN VALUE
  request status

SIDE EFFECTS
  Remove the client object from the client list. If a session is active for this
  client only then the session is terminated otherwise session is made active with
  activation client ID as next requesting client.

===========================================================================*/
extern pdsm_client_status_e_type pdsm_client_deact(

    pdsm_client_id_type               client_id
        /* Requesting client */

);

/*~ FUNCTION pdsm_client_deact */


/* <EJECT> */
/*---------------------------------------------------------------------------
                      COMMAND RELATED TYPES & FUNCTIONS
---------------------------------------------------------------------------*/


/* Type for PD command callback functions.
**
** A client that wants to be notified of the result of a  PD command
** needs to supply a pointer to a function of this type as the first
** parameter to a PD-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef void (pdsm_pd_cmd_cb_f_type) (

    void                      *data_block_ptr,
        /* Pointer to client provided data block.
        **
        ** NOTE! Depending on the client implementation of this function
        ** the data block pointed by this pointer might be accessed from
        ** client, as well as, PDSM context simultaneously */

    pdsm_pd_cmd_e_type        pd_cmd,
        /* Indicate which command is this error status for */

    pdsm_pd_cmd_err_e_type    pd_cmd_err
        /* Command error code, indicating whether command
        ** is rejected and the reason */
);


/* Type for PA command callback functions.
**
** A client that wants to be notified of the result of a  PA command
** needs to supply a pointer to a function of this type as the first
** parameter to a PA-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef void (pdsm_pa_cmd_cb_f_type) (

    void                      *data_block_ptr,
        /* Pointer to client provided data block.
        **
        ** NOTE! Depending on the client implementation of this function
        ** the data block pointed by this pointer might be accessed from
        ** client, as well as, PDSM context simultaneously */

    pdsm_pa_cmd_e_type        pa_cmd,
        /* Indicate which command is this error status for */

    pdsm_pa_cmd_err_e_type    pa_cmd_err
        /* Command error code, indicating whether command
        ** is rejected and the reason */
);

/* Type for UI command callback functions.
**
** A UI client that wants to be notified of the UI event
** needs to supply a pointer to a function of this type as the first
** parameter to a UI-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef void (pdsm_lcs_cmd_cb_f_type) (

    void                      *data_block_ptr,
        /* Pointer to client provided data block.
        **
        ** NOTE! Depending on the client implementation of this function
        ** the data block pointed by this pointer might be accessed from
        ** client, as well as, PDSM context simultaneously */

    pdsm_lcs_cmd_e_type        lcs_cmd,
        /* Indicate which command is this call back is for */

    pdsm_lcs_cmd_err_e_type    lcs_cmd_error
        /* Command error code, indicating whether command
        ** is rejected and the reason */
);


typedef void (pdsm_ext_pos_cmd_cb_f_type) ( 

    void                      *data_block_ptr,
        /* Pointer to client provided data block.
        **
        ** NOTE! Depending on the client implementation of this function
        ** the data block pointed by this pointer might be accessed from
        ** client, as well as, PDSM context simultaneously */

    pdsm_ext_pos_cmd_e_type        ext_pos_cmd,
        /* Indicate which command is this call back is for */

    pdsm_ext_pos_cmd_err_e_type    ext_pos_cmd_error
        /* Command error code, indicating whether command
        ** is rejected and the reason */
);



/* Type for XTRA command callback functions.
**
** A client that wants to be notified of the result of an XTRA command
** needs to supply a pointer to a function of this type as the first
** parameter to a XTRA-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef void (pdsm_xtra_cmd_cb_f_type) (
 
    void                      *data_block_ptr,
        /* Pointer to client provided data block.
        **
        ** NOTE! Depending on the client implementation of this function
        ** the data block pointed by this pointer might be accessed from
        ** client, as well as, PDSM context simultaneously */

    pdsm_xtra_cmd_e_type        xtra_cmd,
        /* Indicate which command is this error status for */

    pdsm_xtra_cmd_err_e_type    xtra_cmd_err
       /* Command error code, indicating whether command
       ** is rejected and the reason */
);

/* Type for  XTRA-T Command Callback Function
**
** A client that wants to be notified of the result of an XTRA-T command
** needs to supply a pointer to a function of this type as the first
** parameter to a XTRA-T-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef void (pdsm_xtra_t_cmd_cb_f_type) 
(
  void                         *data_block_ptr,
  /* Pointer to client provided data block.
  **
  ** NOTE! Depending on the client implementation of this function
  ** the data block pointed by this pointer might be accessed from
  ** client, as well as, PDSM context simultaneously */

  pdsm_xtra_t_cmd_e_type        xtra_t_cmd,
  /* Indicate which command is this error status for */

  pdsm_xtra_t_cmd_err_e_type    xtra_t_cmd_err
  /* Command error code, indicating whether command is rejected and the reason */
);

/* Type for WIPER command callback functions.
**
** A client that wants to be notified of the result of a WIPER command
** needs to supply a pointer to a function of this type as the first
** parameter to a WIPER-command-function.
**
** NOTE! Client's provided callback functions must be reentrant since they
** might be called from client, as well as, PDSM context simultaneously.
*/
typedef void (pdsm_wiper_cmd_cb_f_type) (
 
    void                      *data_block_ptr,
        /* Pointer to client provided data block.
        **
        ** NOTE! Depending on the client implementation of this function
        ** the data block pointed by this pointer might be accessed from
        ** client, as well as, PDSM context simultaneously */

    pdsm_wiper_cmd_e_type        wiper_cmd,
        /* Indicate which command is this error status for */

    pdsm_wiper_cmd_err_e_type    wiper_cmd_err
       /* Command error code, indicating whether command
       ** is rejected and the reason */
);

/* <EJECT> */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* PDSM command functions
**
** The PDSM command functions defines the Client->PDSM interface.
**
** Each command function takes a pointer to a client provided callback
** function, a client provided void pointer and the command's required set
** of fields.
**
** If the command function find one or more of the parameters to contain
** invalid values ( e.g. not one of the enum values for an enum parameter),
** it calls on the client's callback function IMMEDIATELY with the
** appropriate command error code. NOTE that in this case the callback
** function is actually being called from the client context - NOT PDSM.
**
** If all parameters found to be valid, the command function queues a
** command to PDSM. When PDSM gets the command it checks whether the command
** is valid relative to the current state of the phone.
**
** In case the command is not valid, PDSM calls on the client callback function
** with the appropriate command error code. NOTE that in this case the callback
** function is indeed being called from the PDSM context.
**
** If the command is successful, PDSM calls on the client callback function
** with an error code that indicates successful command. In addition PDSM
** notified registered clients of the corresponding PDSM event (e.g. if a
** client successfully ends a session, PDSM notify clients that are registered
** for the pd-end-event).
*/


/* <EJECT> */
/*===========================================================================

FUNCTION pdsm_get_position

DESCRIPTION
  Command PDSM to obtain PD information.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
extern boolean pdsm_get_position(
   pdsm_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_pd_option_s_type    *option_ptr,        /* PD option type */
  /*~ PARAM option_ptr POINTER */
   pdsm_pd_qos_type         *qos_ptr,           /* Ptr to user QOS for this request */
  /*~ PARAM qos_ptr POINTER */
   pdsm_client_id_type      client_id           /* Client id of user */
);
/*~ FUNCTION pdsm_get_position */

/*===========================================================================

FUNCTION pdsm_start_periodic_ppm_report

DESCRIPTION
  Ask PDSM to start a periodic Pilot Phase Measurement Report. A periodic
  PPM report will be pumped out as a PDAPI EXT event once this API is called.

  The client must register for this PDAPI EXT event by calling 
  pdsm_client_ext_status_reg and registering for the appropriate event.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
extern boolean pdsm_start_periodic_ppm_report(
   pdsm_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   uint16                   period,             /* time bet. two ppm repts */
                                                /* range 1 second to 1 hour */
   pdsm_client_id_type      client_id           /* Client id of user */
);
/*~ FUNCTION pdsm_start_periodic_ppm_report */

/*===========================================================================

FUNCTION pdsm_stop_periodic_ppm_report

DESCRIPTION
  Ask PDSM to stop a previously started periodic Pilot Phase Measurement 
  Report.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_client_init( ) must have been already done.
  pdsm_start_periodic_ppm_report( ) must already have been done

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
extern boolean pdsm_stop_periodic_ppm_report(
   pdsm_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_client_id_type      client_id           /* Client id of user */
);
/*~ FUNCTION pdsm_stop_periodic_ppm_report */

/*===========================================================================

FUNCTION pdsm_end_session

DESCRIPTION
  Command PDSM to end PD session.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/

extern boolean pdsm_end_session(
   pdsm_pd_cmd_cb_f_type        pd_cb_func,        /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   pdsm_pd_end_session_e_type   session_type,      /* End session type */
   void                         *client_data_ptr,  /* Client specified ptr */
   pdsm_client_id_type          client_id          /* requesting client ID */
);
/*~ FUNCTION pdsm_end_session */


/*===========================================================================

FUNCTION pdsm_end_session_ex

DESCRIPTION
  Command PDSM to end PD session.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

  PDSM End Session Ex can be used by even non OEM client to turn the receiver off.
  It takes in additional end session optional parameters.

DEPENDENCIES
  pdsm_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/

extern boolean pdsm_end_session_ex(
   pdsm_pd_cmd_cb_f_type        	pd_cb_func,        /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   pdsm_pd_end_session_e_type   	session_type,      /* End session type */
   pdsm_pd_end_session_opt_s_type 	*end_session_opt,  /*End Session Option type*/
   void                         	*client_data_ptr,  /* Client specified ptr */
   pdsm_client_id_type          	client_id          /* requesting client ID */
);
/*~ FUNCTION pdsm_end_session_ex */

/*===========================================================================

FUNCTION pdsm_pd_get_best_avail_pos

DESCRIPTION
  This API initiates a command to the engine to return the best available position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix .  

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_get_best_avail_pos
(
   pdsm_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
    /*~ PARAM pa_motion_data_cb_func POINTER */
   pdsm_client_id_type   client_id,
   void   *client_data_ptr /* client provided reference data pointer */
);

/*===========================================================================

FUNCTION pdsm_pd_get_avail_wwan_pos

DESCRIPTION
  This API initiates a command to the engine to return the available WWAN position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix.

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_get_avail_wwan_pos
(
  pdsm_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
  /*~ PARAM pa_motion_data_cb_func POINTER */
  pdsm_client_id_type     client_id,
  void                   *client_data_ptr /* client provided reference data pointer */
);

/*===========================================================================

FUNCTION pdsm_pd_get_available_pos

DESCRIPTION
  This API initiates a command to the engine to return the available  position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix.

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS


===========================================================================*/
boolean pdsm_pd_get_available_pos
(
  pdsm_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
  /*~ PARAM pa_motion_data_cb_func POINTER */
  pdsm_client_id_type     client_id,
  void                   *client_data_ptr /* client provided reference data pointer */
);

/*===========================================================================
FUNCTION pdsm_pd_inject_external_time
DESCRIPTION
  Command PDSM to inject time information from an external clock.
  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_client_init( ) must have been already done.
RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request
SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.
===========================================================================*/
extern boolean pdsm_pd_inject_external_time(
   pdsm_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_pd_external_time_info_type    *ext_time_ptr,        /* external time */
  /*~ PARAM ext_time_ptr POINTER */
   pdsm_client_id_type      client_id           /* Client id of user */
);
/*~ FUNCTION pdsm_pd_inject_external_time */


/*===========================================================================

FUNCTION pdsm_pd_inject_external_pos

DESCRIPTION
  !! DEPRECATED API !!, use pdsm_pd_inject_external_position() instead.
  Command PDSM to inject coarse position information from an external source.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
extern boolean pdsm_pd_inject_external_pos(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_pd_external_pos_info_type    *ext_pos_ptr, /* external position */
   /*~ PARAM ext_pos_ptr POINTER */
   pdsm_client_id_type      client_id              /* Client id of user */
);
/*~ FUNCTION pdsm_pd_inject_external_pos */

/*===========================================================================
DESCRIPTION
  !! DEPRECATED API !!, use pdsm_pd_inject_external_position() instead.
  Command PDSM to inject coarse position information from an external source
  with reliability associated with the injected position.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
extern boolean pdsm_pd_inject_external_pos_ex(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_pd_external_pos_info_ex_type    *ext_pos_ptr, /* external position */
   /*~ PARAM ext_pos_ptr POINTER */
   pdsm_client_id_type      client_id              /* Client id of user */
);
/*~ FUNCTION pdsm_pd_inject_external_pos_ex */

/*===========================================================================
DESCRIPTION
  Command PDSM to inject coarse position information from an external source
  with reliability associated with the injected position.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
extern boolean pdsm_pd_inject_external_position(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_pd_external_position_info_type  *ext_pos_ptr, /* external position */
   /*~ PARAM ext_pos_ptr POINTER */
   pdsm_client_id_type      client_id              /* Client id of user */
);
/*~ FUNCTION pdsm_pd_inject_external_position */


/*===========================================================================
FUNCTION pdsm_pd_query_gps_state
DESCRIPTION
  Command PDSM to return the state information of the GPS engine.
  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_client_init( ) must have been already done.
RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request
SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM extended status events.
===========================================================================*/
extern boolean pdsm_pd_query_gps_state(
   pdsm_pd_cmd_cb_f_type     cmd_cb_func,       /* Command callback */
   /*~ PARAM cmd_cb_func POINTER */
   void                      *client_data_ptr,  /* Not sure if this is required */
   pdsm_pd_ext_gps_state_s_type  *gps_state_ptr,           /* Pointer to Parameter values to query. Not used */
  /*~ PARAM gps_state_ptr POINTER */
   pdsm_client_id_type       client_id          /* Requesting client id */
);
/*~ FUNCTION pdsm_pd_query_gps_state */

/*============================================================================

FUNCTION pdsm_set_parameters

DESCRIPTION
  Command to set PDSM Parameters

DEPENDENCIES
  pdsm_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/

extern boolean pdsm_set_parameters(
   pdsm_pa_cmd_cb_f_type     cmd_cb_func,       /* Command callback */
   /*~ PARAM cmd_cb_func POINTER */
   void                      *client_data_ptr,  /* Not sure if this is required */
   pdsm_pa_e_type            pa_set,            /* Parameter mask to be set */
   pdsm_pa_info_type         *pa_ptr,           /* Pointer to Parameter values to bet set to */
   /*~ PARAM pa_ptr POINTER DISC pa_set*/

   pdsm_client_id_type       client_id          /* Requesting client id */
);
/*~ FUNCTION pdsm_set_parameters */


/*===========================================================================

FUNCTION PDSM_GET_PARAMETERS

DESCRIPTION
  This function is used to get PDSM session parameters.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_get_parameters
(
  pdsm_pa_cmd_cb_f_type    cmd_cb_func,        /* Command callback fn */
  /*~ PARAM cmd_cb_fund POINTER */
  void                     *client_data_ptr,   /* Client provided data pointer */
  pdsm_pa_e_type           pa_get,             /* Type of parameter to Get */
  pdsm_client_id_type      client_id
);
/*~ FUNCTION pdsm_get_parameters */

/*===========================================================================

FUNCTION PDSM_NOTIFICATION_VERIFICATION_RESPONSE

DESCRIPTION
  This function is used by the UI application to respond to a notification
  verification request.

RETURN VALUE
  None

DEPENDENCIES
  Intended to be called by registered UI clients.

===========================================================================*/
extern boolean pdsm_lcs_agent_client_response(
  pdsm_lcs_cmd_cb_f_type   lcs_cb_func,
  void                    *client_data_ptr,
  pdsm_lcs_notification_resp_s_type *resp_info_ptr,
  /*~ PARAM resp_info_ptr POINTER */
  pdsm_client_id_type      client_id

);
/*~ FUNCTION pdsm_lcs_agent_client_response */

/*===========================================================================
FUNCTION pdsm_use_this_position
DESCRIPTION
  Command PDSM to use the supplied position to report a fix. This is used for
  the Position Shopping feature
  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_client_init( ) must have been already done.
RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request
SIDE EFFECTS
===========================================================================*/
extern boolean pdsm_use_this_position(
   pdsm_ext_pos_cmd_cb_f_type    cmd_cb_func,         /* Command callback */
   pdsm_ext_use_this_pos_data_type *position_to_use,   /* Ptr to the Position data */
   /*~ PARAM position_to_use POINTER */
   void                     *client_data_ptr,  /* Client specified ptr */
   pdsm_client_id_type      client_id           /* Client id of user */
);
/*~ FUNCTION pdsm_use_this_position */

/*===========================================================================

FUNCTION pdsm_lcs_client_init

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
extern boolean pdsm_lcs_client_init(uint32 class_id, 
                                    pdsm_client_type_e_type client_type, 
                                    boolean is_brew_app);
/*~ FUNCTION pdsm_lcs_client_init */

/*===========================================================================

FUNCTION pdsm_lcs_client_app_info_init

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
extern boolean pdsm_lcs_client_app_info_init(uint32 class_id, 
                                    pdsm_client_type_e_type client_type, 
                                    char* app_info, 
                                    /*~ PARAM app_info POINTER */
                                    boolean is_brew_app);
/*~ FUNCTION pdsm_lcs_client_app_info_init */


/*===========================================================================

FUNCTION pdsm_lcs_client_release

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
extern boolean pdsm_lcs_client_release(  
  pdsm_lcs_cmd_cb_f_type   cmd_cb_func, 
  void                    *client_data_ptr,
  /*~ PARAM client_data_ptr POINTER */
  uint32 class_id, 
  pdsm_client_type_e_type client_type);
/*~ FUNCTION pdsm_lcs_client_release */

/*===========================================================================

FUNCTION pdsm_lcs_query_vx_config

DESCRIPTION  Query current Vx version configuration and return asynchronously
             via event PDSM_LCS_EVENT_CLIENT_VX_CONFIG

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none

===========================================================================*/
extern boolean pdsm_lcs_query_vx_config(pdsm_client_type_e_type client_type); 
/*~ FUNCTION pdsm_lcs_query_vx_config */

/*===========================================================================

FUNCTION pdsm_lcs_set_vx_config

DESCRIPTION  Set Vx version configuration (PDSM_LCS_AGENT_VERSION_V1_ONLY or
             PDSM_LCS_AGENT_VERSION_V2_ONLY) and return new version configuration
             asynchronously via event PDSM_LCS_EVENT_CLIENT_SET_VX_CONFIG

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS 
  Besides the run-time Vx version configuration, NV 1993 (VxVersion) will be
  written as well (Vx version configuration persistent across reboot)

===========================================================================*/
extern boolean pdsm_lcs_set_vx_config(pdsm_client_type_e_type client_type,
                                      pdsm_lcs_agent_set_vx_config_s_type set_vx_config); 
/*~ FUNCTION pdsm_lcs_set_vx_config */

/*===========================================================================

FUNCTION pdsm_get_client_type

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
pdsm_client_type_e_type pdsm_lcs_get_client_type(uint32 class_id);
/*~ FUNCTION pdsm_lcs_get_client_type */

/*===========================================================================
FUNCTION pdsm_lcs_get_status

DESCRIPTION
   This returns the LCS status asynchronously via a LCS Event callback 
RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
boolean pdsm_lcs_get_status(void);
/*~ FUNCTION pdsm_lcs_get_status */

/*===========================================================================

FUNCTION pdsm_get_pdapi_version

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
uint32 pdsm_get_pdapi_version(void);
/*~ FUNCTION pdsm_get_pdapi_version */


/*===========================================================================

FUNCTION pdsm_xtra_set_data

DESCRIPTION
  gpsOneXTRA client calls this function to send parts of the XTRA file to GPS 
  engine.  
  
DEPENDENCIES

RETURN VALUE
  TRUE - Data part has been accepted
  FALSE - Data part has been rejected
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_set_data(
    pdsm_xtra_cmd_cb_f_type    xtra_cb_func,       /* Command callback */
    /*~ PARAM xtra_cb_func POINTER */
    pdsm_client_id_type        client_id,          /* Client id of user */
    void                       *client_data_ptr,   /* user data pointer */
    uint32                     xtra_data_len,      /* Length of XTRA data */
    unsigned char              *xtra_data_ptr,     /* XTRA data */
    /*~ PARAM xtra_data_ptr VARRAY LENGTH xtra_data_len */
    uint8                      part_number,        /* data part number */
    uint8                      total_parts,        /* total data parts */
    pdsm_xtra_dc_status_e_type xtra_dc_status      /* status of download */
);
/*~ FUNCTION pdsm_xtra_set_data */


/*===========================================================================

FUNCTION pdsm_xtra_client_initiate_download_request

DESCRIPTION
   gpsOneXTRA client calls this function when it needs to download an extended 
   assistance file but it does not have necessary information 
   (i.e. server URL, etc). Upon getting the request, TM should send 
   gpsOneXTRA PD API event which contains required information to the 
   gpsOneXTRA client.
   
DEPENDENCIES

RETURN VALUE
   TRUE - Initiate download request success
   FALSE - Initiate download request failed
   
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_client_initiate_download_request(
    pdsm_xtra_cmd_cb_f_type    xtra_cb_func,      /* Command callback */
    /*~ PARAM xtra_cb_func POINTER */
    pdsm_client_id_type        client_id,         /* Client id of user */
    void                       *client_data_ptr   /* user data pointer */
);
/*~ FUNCTION pdsm_xtra_client_initiate_download_request */


/*===========================================================================

FUNCTION pdsm_xtra_set_auto_download_params

DESCRIPTION
  gpsOneXTRA client calls this function to modify parameters related to the 
  automatic download functionality.  enable_auto_download flag is used 
  to enable or disable the functionality.  The download_interval parameter 
  specifies the number of hours between automatic downloads and its range 
  is 1-168 (168 = number of hours in 1 week).
  
DEPENDENCIES

RETURN VALUE
  TRUE - Auto Download Parameters are updated
  FALSE - Auto Download Parameters are NOT updated
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_set_auto_download_params(
    pdsm_xtra_cmd_cb_f_type    xtra_cb_func,         /* Command callback */
    /*~ PARAM xtra_cb_func POINTER */
    pdsm_client_id_type        client_id,            /* Client id of user */
    void                       *client_data_ptr,     /* user data pointer */
    boolean                    enable_auto_download, /* turn on/off auto download */
    uint16                     download_interval     /* time between downloads */
);
/*~ FUNCTION pdsm_xtra_set_auto_download_params */


/*===========================================================================

FUNCTION pdsm_xtra_query_data_validity

DESCRIPTION
  gpsOneXTRA client calls this function to request validity information. 
  Upon getting the request, the starting GPS week and GPS minutes 
  along with the duration of the current extended assistance data 
  will be sent to the gpsOneXTRA client.  
  
  If there is no data in the subsystem, the values of the start time and 
  duration will be set to 0.
  
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_query_data_validity(
    pdsm_xtra_cmd_cb_f_type    xtra_cb_func,       /* Command callback */
    /*~ PARAM xtra_cb_func POINTER */
    pdsm_client_id_type        client_id,          /* Client id of user */
    void                       *client_data_ptr    /* user data pointer */
);
/*~ FUNCTION pdsm_xtra_query_data_validity */


/*===========================================================================

FUNCTION pdsm_xtra_inject_time_info

DESCRIPTION
   gpsOneXTRA client calls this function to inject XTRA Time
   
DEPENDENCIES

RETURN VALUE
    TRUE - Time injection success
    FALSE - Time injection failed
    
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_inject_time_info(
    pdsm_xtra_cmd_cb_f_type  xtra_cb_func,       /* Client command callback funct. */
    /*~ PARAM xtra_cb_func POINTER */
    pdsm_client_id_type      client_id,          /* Client id of user */
    void                     *client_data_ptr,   /* Pointer to client data block */
    pdsm_xtra_time_info_type *time_info_ptr      /* Pointer to time information data */
    /*~ PARAM time_info_ptr POINTER */
);
/*~ FUNCTION pdsm_xtra_inject_time_info */

extern void pdsm_notify_wiper_status(
boolean wps_available
);
/*~ FUNCTION pdsm_notify_wiper_status */


/*===========================================================================

FUNCTION pdsm_xtra_t_get_data

DESCRIPTION
   XTRA-T client calls this function to get CellDB data from GPS Engine.
   
DEPENDENCIES

RETURN VALUE
    TRUE  - XTRA-T get data command success
    FALSE - XTRA-T get data command failure

SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_xtra_t_get_data
(
  pdsm_xtra_t_cmd_cb_f_type   xtra_t_cb_func,      /* Command callback */
  void                       *client_data_ptr,     /* Not sure if this is required */
  /*~ PARAM client_data_ptr POINTER */
  pdsm_xtra_t_get_s_type      xtra_t_get_type,     /* Parameter mask to be set */
  pdsm_client_id_type         client_id            /* Requesting client id */
);
/*~ FUNCTION pdsm_xtra_t_get_data */

/*===========================================================================

FUNCTION pdsm_send_wiper_position_report

DESCRIPTION
   !! DEPRECATED API !! use pdsm_send_wiper_position_report_ex() instead.
   gpsOne WIPER client calls this function to report Wiper position report
   
DEPENDENCIES
   pdsm_client_init() should be already done.

RETURN VALUE
   none
    
SIDE EFFECTS
   none
===========================================================================*/
extern void pdsm_send_wiper_position_report(
    pdsm_wiper_cmd_cb_f_type  wiper_cb_func,       /* Client command callback funct. */
    /*~ PARAM wiper_cb_func POINTER */
    pdsm_client_id_type      client_id,          /* Client id of user */
    void                     *client_data_ptr,   /* Pointer to client data block */
    /*~ PARAM client_data_ptr POINTER */
    t_wiper_position_report_struct_type * position_report_ptr      /* Pointer to position report data */
    /*~ PARAM position_report_ptr POINTER */
);
/*~ FUNCTION pdsm_send_wiper_position_report */

/*===========================================================================

FUNCTION pdsm_send_wiper_position_report_ex

DESCRIPTION
   gpsOne WIPER client calls this function to report Wiper position report
   with position reliability.
   
DEPENDENCIES
   pdsm_client_init() should be already done.

RETURN VALUE
   none
    
SIDE EFFECTS
   none
===========================================================================*/
extern void pdsm_send_wiper_position_report_ex(
    pdsm_wiper_cmd_cb_f_type  wiper_cb_func,       /* Client command callback funct. */
    /*~ PARAM wiper_cb_func POINTER */
    pdsm_client_id_type      client_id,          /* Client id of user */
    void                     *client_data_ptr,   /* Pointer to client data block */
    /*~ PARAM client_data_ptr POINTER */
    t_wiper_position_report_ex_struct_type * position_report_ptr      /* Pointer to position report data */
    /*~ PARAM position_report_ptr POINTER */
);
/*~ FUNCTION pdsm_send_wiper_position_report_ex */


typedef enum {
   PDSM_PD_EXTERNAL_SPI_STATE_UNKNOWN = 0,
   PDSM_PD_EXTERNAL_SPI_STATE_STATIONARY = 1,
   PDSM_PD_EXTERNAL_SPI_STATE_NON_STATIONARY = 2,

   /* add more states above this line */
   PDSM_PD_EXTERNAL_SPI_STATE_MAX,
   PDSM_PD_EXTERNAL_SPI_STATE_MAX_RESERVED = 0x7FFFFFFF /* to force size to 32 bits  */
} pdsm_pd_external_spi_state_type;
/*~ SENTINEL pdsm_pd_external_spi_state_type.PDSM_PD_EXTERNAL_SPI_STATE_MAX */

typedef struct {
   uint8   u_confidence;   /* confidence expressed as percentage */
   uint32  q_state;        /* see pdsm_pd_external_spi_mode_type */
   uint32 q_reserved[2]; /* set to 0 */
} pdsm_pd_external_spi_type;

/*===========================================================================

FUNCTION pdsm_pd_inject_external_spi

DESCRIPTION
   gpsOne PDAPI client calls this to inject motion information (XSPI) in 
   response to PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION
   
DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise
    
SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_inject_external_spi(
   pdsm_pd_cmd_cb_f_type   pd_xspi_cb_func, /* command result callback function */
    /*~ PARAM pd_xspi_cb_func POINTER */
   pdsm_client_id_type   client_id,
   void   *client_data_ptr,
   pdsm_pd_external_spi_type    *ext_spi_ptr
    /*~ PARAM ext_spi_ptr POINTER */
);
/*~ FUNCTION pdsm_pd_inject_external_spi */


/*===========================================================================

FUNCTION pdsm_pd_inject_motion_data

DESCRIPTION
   Inject motion data information from client. This is meant to replace XSPI
   as a more detailed API.

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_pd_inject_motion_data(
   pdsm_pd_cmd_cb_f_type   pd_motion_data_cb_func, /* command result callback function */
    /*~ PARAM pa_motion_data_cb_func POINTER */
   pdsm_client_id_type   client_id,
   void   *client_data_ptr,
   pdsm_pd_motion_data_s_type    *motion_data_ptr
    /*~ PARAM motion_data_ptr POINTER */
);
/*~ FUNCTION pdsm_pd_inject_motion_data */

/*===========================================================================

FUNCTION pdsm_reset_location_service

DESCRIPTION
   Command PDSM to Location Service Reset GPS.
   This command will not be accepted when emergency call is in progress

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS
  If the command is acepted, PDSM_EXT_STATUS_EVENT_RESET_LOCATION_SERVICE_DONE
  ext status event will be delivered to pdapi client upon completion of reset.
    

===========================================================================*/
boolean pdsm_reset_location_service(
  pdsm_pd_cmd_cb_f_type pd_reset_location_service_cb_func,  /* command result callback function */
  /*~ PARAM pd_reset_location_service_cb_func POINTER */
  pdsm_client_id_type   client_id,
  void   *client_data_ptr
);
/*~ FUNCTION pdsm_reset_location_service */

/*===========================================================================

FUNCTION pdsm_enable_dpo_status_report

DESCRIPTION
    This function enables periodic DPO status reporting from MGP through SM layer.
    When enabled, the DPO status is reported at a nominal 1 Hz rate when receiver
    is active. This control is persistent across GNSS sessions, i.e. when the
    configuration is set, it stays till the phone is powered off (or changed
    by a new configuration).
 
DEPENDENCIES 
   * Client must have already been initialized with pdsm_client_init().
   * Client must have been registered using API pdsm_client_ext_status_reg() for the
     event PDSM_EXT_STATUS_EVENT_DPO_STATUS_REPORT. 
 
PARAMETERS
   enable : TRUE - Send the command to enable DPO status report.
            FALSE - Send the command to disable DPO status report.
 
RETURN VALUE
   TRUE - If the command to enable DPO status report is succesfull
   FALSE - If the command to enable DPO status report is not succesfull

SIDE EFFECTS 
  Only clients registered with event PDSM_EXT_STATUS_EVENT_DPO_STATUS_REPORT would receive this report .
  Clients not regsitered for this event would not get any report if even if they call this API with parameter TRUE
 
===========================================================================*/
boolean pdsm_enable_dpo_status_report(
  boolean enable
);
/*~ FUNCTION pdsm_pd_enable_dpo_report */

/*
******************************************************************************
* pdsm_read_qmi_services_config
*
* Function description:
*
*  This synchronous API is called by QMI services to read QMI configuration at 
*  bootup. This function may be called only after PDAPI is initialized. The QMI 
*  configuration is used by the caller to register with PDAPI 
*
* Parameters:
*  qmi_Config : Configuration for QMI initialization
*
* Return value:
*  TRUE  - Request processed successfully
*  FALSE - Request could not be processed
*
* ******************************************************************************
*/
boolean pdsm_read_qmi_services_config( pdsm_QMIServiceConfig *qmi_Config );


/* Define different types of cell database */
typedef enum
{
  CELL_DB_NONE = 0,
  CELL_DB_MAIN_MODEM,
  CELL_DB_EXTERNAL_MODEM
}cell_db_type;


/* Enumeration of the Cell Service Status */
typedef enum
{
  STATUS_UNKNOWN = 0,
  STATUS_OUT_OF_SERVICE,
  STATUS_CAMPED_NOT_ROAMING,
  STATUS_CAMPED_ROAMING
} pdsm_cell_service_status_e_type;


/* GSM Cell Information  (activeRAT is CELLDB_ACTIVERAT_GSM)
   The fields should be all valid when activeRAT is GSM */
typedef struct 
{
  uint32  MCC;      // Mobile Country Code
  uint32  MNC;      // Mobile Network Code
  uint32  LAC;      // Location Area Code
  uint32  CID;      // Cell Identification
} pdsm_gsm_cell_info_s_type;

/* Enumeration of RAT(Radio Access Type) */
typedef enum
{
  PDSM_ACTIVERAT_NONE,
  PDSM_ACTIVERAT_CDMA,
  PDSM_ACTIVERAT_WCDMA,
  PDSM_ACTIVERAT_GSM,
  PDSM_ACTIVERAT_LTE,  
  PDSM_ACTIVERAT_TDSCDMA,
  PDSM_ACTIVERAT_MAX
} pdsm_active_RAT_e_type;

typedef union 
{
  /* activeRAT = CELLDB_ACTIVERAT_GSM */
  pdsm_gsm_cell_info_s_type gsmCell;

  /* Can add other RATs (WCDMA, CDMA, TD-SCDMA, LTE) as needed */
}pdsm_cell_info_u_type;


/* Cell Inforamtion */
typedef struct
{
  /* service status: if it's UNKNOWN or OOS, activeRAT should be set to
     CELLDB_ACTIVERAT_NONE to indicate that there is no active RAT and
     cellInfo is invalid. When service status is CAMPED_ROAMING or
     CAMPED_NOT_ROAMING, activeRAT should be set to the current active
     RAT */
  pdsm_cell_service_status_e_type serviceStatus; // service status

  /* Currently active RAT for the cell, depending on activeRAT,
     corresponding cell_info field in cellInfo union will be valid */
  pdsm_active_RAT_e_type  activeRAT;   

  /* cell info union, currently only GSM RAT is supported */
  pdsm_cell_info_u_type   cellInfo;    

} pdsm_cell_info_s_type;


/*===========================================================================

FUNCTION pdsm_pd_inject_external_cell_info

DESCRIPTION
  PDSM client calls this function to inject external cell information to SM
  Typically this is the cell info updated from HLOS for external/3rd-party
  modem. This external cell info will be used in SUPL if it's valid, otherwise
  CM's cell info will be used (typically for internal modem)
 
  The supplied callback function pd_cb_func will be called to report this
  command's success/failure status. There will be no PDAPI event for this command.
 
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_pd_inject_external_cell_info(
  pdsm_pd_cmd_cb_f_type    pd_cb_func,         /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
  void                     *client_data_ptr,    /* user data pointer */
  pdsm_client_id_type      client_id,           /* Client id of user */
  pdsm_cell_info_s_type    *cell_info           /* Pointer to external cell info */
  /*~ PARAM cell_info POINTER */
);
/*~ FUNCTION pdsm_pd_inject_external_cell_info */


/* Enumeration of the NI message type, can add Vx message as needed */
typedef enum
{
  NI_MESSAGE_UNKNOWN = 0,
  NI_MESSAGE_SUPL,
} pdsm_ni_msg_type_e_type;

/* For now maximum length for NI message is set to 1024 bytes */
#define PDSM_MAX_NI_MSG_LEN 1024

/* Short Message  */
typedef struct
{
  pdsm_ni_msg_type_e_type   type;      // NI message type
  uint16                    len;       // up to 1024 bytes message
  uint8                     *ni_data;  // pointer to NI message data
  /*~ FIELD pdsm_ni_message_s_type.data POINTER */ 
} pdsm_ni_message_s_type;


/*===========================================================================

FUNCTION pdsm_pd_inject_ni_message

DESCRIPTION
  PDSM client calls this function to inject NI message to SM
  This is typically used to inject SUPL NI SMS received from HLOS.
  It can be expanded to support other SMS like V1/V2, or NI message from
  other transport like UDP, etc.
 
  The supplied callback function pd_cb_func will be called to report this
  command's success/failure status. There will be no PDAPI event for this command.
 
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_pd_inject_ni_message(
  pdsm_pd_cmd_cb_f_type    pd_cb_func,        /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
  void                     *client_data_ptr,   /* user data pointer */
  pdsm_client_id_type      client_id,          /* Client id of user */
  pdsm_ni_message_s_type   *ni_msg             /* NI message */
  /*~ PARAM ni_info POINTER */
); 
/*~ FUNCTION pdsm_pd_inject_ni_message */


typedef struct
{
  uint32 q_num_wifi_aps; /* number of wifi access points */
  locEngWifiApInfoStructT_v01 *first_wifi_ap_ptr;
} pdsm_wifi_on_demand_info_list_s_type;

/*===========================================================================

FUNCTION pdsm_pd_inject_wifi_info_on_demand()

DESCRIPTION
  PDSM client calls this function to inject on-demand WiFi scan results to SM
 
  The supplied callback function pd_cb_func will be called to report this
  command's success/failure status. There will be no PDAPI event for this command.
 
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/
extern boolean pdsm_pd_inject_wifi_info_on_demand (
   pdsm_pd_cmd_cb_f_type    pd_cb_func,        /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
  void                     *client_data_ptr,   /* user data pointer */
  pdsm_client_id_type      client_id,          /* Client id of user */
  pdsm_wifi_on_demand_info_list_s_type  *wifi_info_on_demand_data_list_ptr /* Pointer to wifi on-demand scan result */  
  /*~ PARAM ni_info POINTER */
); 
/*~ FUNCTION pdsm_pd_inject_wifi_info_on_demand */

/* Wifi enabled status of the device. */
typedef enum
{
  WIFI_ENABLED_STATUS_FALSE, /*Wifi is disabled on the device */
  WIFI_ENABLED_STATUS_TRUE, /*Wifi is enabled on the device */
  WIFI_ENABLED_STATUS_MAX = 0xFFFFFFFF, 
} pdsm_wifi_enabled_status_ind_type;

typedef enum
{
  WIFI_ATTACHMENT_STATUS_CONNECTED, /*Wifi Acess Point Connect Event*/
  WIFI_ATTACHMENT_STATUS_DISCONNECTED, /*Wifi Acess Point DisConnect Event*/
  WIFI_ATTACHMENT_STATUS_HANDOVER, /*Wifi Acess Point Handover Event*/
  WIFI_ATTACHMENT_MAX = 0xFFFFFFFF,
} pdsm_wifi_attachment_status_ind_type;

typedef struct
{
  uint8 mac_addr[WIPER_MAC_ADDR_LENGTH]; /*AP Mac Addr*/
  char  ssid[WIPER_SSID_STR_LENGTH];  
} pdsm_wifi_ap_info;


typedef struct
{
  pdsm_wifi_attachment_status_ind_type wifi_attachment_status_ind_type;
  union
  {
    pdsm_wifi_ap_info wifi_ap_info;
  }u;
} pdsm_wifi_attachment_status_ind_info;

/*===========================================================================

FUNCTION pdsm_wifi_attachment_status_ind_report

DESCRIPTION
    This function is used to inject Wifi attachment status indication like:
    . Connected to Access point
    . Disconnected to Access point
    . Handover from one acces point to another
 
DEPENDENCIES 
   * Client must have already been initialized with pdsm_client_init().
 
PARAMETERS
 *wifi_attachment_status_ind_info: Pointer to struct containing wifi attachment status indicaiton info 
RETURN VALUE
  Void
SIDE EFFECTS  
===========================================================================*/
void pdsm_wifi_attachment_status_ind_report(
  pdsm_wifi_attachment_status_ind_info *wifi_attachment_status_ind_info
);

/* Maximum log size permissible for GM in memory logging */
#define PDSM_GM_IN_MEM_MAX_LOG_SIZE_B 1024 * 1000 
/* Minimum periodicity for GM in mem logs */
#define PDSM_GM_IN_MEM_MIN_PERIOD_S 120
/*===========================================================================

FUNCTION pdsm_gm_in_mem_logging_start

DESCRIPTION
  This function is for start GM in memory logging.

DEPENDENCIES

RETURN VALUE
  TRUE/FAIL
SIDE EFFECTS
  None.

===========================================================================*/
boolean pdsm_gm_in_mem_logging_start(uint32 max_log_size, uint32 q_period_s);

/*===========================================================================

FUNCTION pdsm_gm_in_mem_logging_stop

DESCRIPTION
  This function is for stopping GM in memory logging.

DEPENDENCIES

RETURN VALUE
  TRUE/FAIL
SIDE EFFECTS
  None.

===========================================================================*/
boolean pdsm_gm_in_mem_logging_stop(void);
/*===========================================================================

FUNCTION pdsm_wifi_enabled_status_report

DESCRIPTION
    This function is used to inject Wifi enabled status information into the modem.
 
DEPENDENCIES 
   Client must have already been initialized with pdsm_client_init().
 
PARAMETERS
 e_wifi_enabled_status : Indicates whether the wifi is enabled or disabled on the device.
RETURN VALUE
  Void
SIDE EFFECTS  
  None
===========================================================================*/
void pdsm_wifi_enabled_status_report(
  pdsm_wifi_enabled_status_ind_type e_wifi_enabled_status
);

/*===========================================================================

FUNCTION pdsm_get_gps_lock_status()

DESCRIPTION
  PDSM client calls this function to get current GPS lock status
 
  This is a synchronous API, will return immediately 
 
DEPENDENCIES

RETURN VALUE
  Current GPS lock status
  
SIDE EFFECTS

===========================================================================*/
extern pdsm_gps_lock_e_type pdsm_get_gps_lock_status(void);

#ifdef __cplusplus
}
#endif

#endif /* PDAPI_H */

