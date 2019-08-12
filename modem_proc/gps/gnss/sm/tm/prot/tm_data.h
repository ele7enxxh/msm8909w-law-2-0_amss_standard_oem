#ifndef TM_DATA_H
#define TM_DATA_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

                  TM_DATA Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for both
  TM & TM protocol tasks.

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/prot/tm_data.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/30/16   skm     Added NV for default LPP12 2 timer feature config 
04/15/16   ld      Added NHz configuration parameter and type 
08/05/15   rh      Added carrier-specific NV 1920 (pos mode support) to TM core 
07/08/15   rh      Changed NV refresh to be done on one specifc sub
06/09/15   ssu     Adding NV for GM filtering of WLAN freq. 
06/01/15   jv      Added QZSS Support
03/14/15   rh      Added support for MultiSIM NV 
11/16/14   rh      Added API to get subs ID from MMSGDI table for SIM swap 
10/29/14   ssu     GM LOWI Integration
3/12/14    gk     Handle OTA reset when session is active. consider as end of the session
11/25/14   rk      NV item for LPP Rel10 grt 24 AD 
10/27/14   gk      NV item for terminating SUPL after final fix 
10/24/14   jv      Added Galileo Support
09/10/14   gk      Added delete throttle timer to prevent B-b delete commands
04/28/14   ah      Added handling of Set Premium Services indication status
04/17/14   skm     NV Async Read changes 
02/21/14   ssu     Adding NV for configuring motion sensing distance in case of challenging GPS env. 
02/20/14   ssu     Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config.  
03/10/14   ah      Added support for Get Avail WWAN Pos
02/10/14   ssu     Adding a NV for modifying the rate at which GM requests CPI injection. 
01/31/14   ss      Supporting Max acceptable GF PUNC confguration from QMI
01/10/14   ss      Supporting High Responsiveness Geofencing Config
12/23/13   mj	     Remove FEATURE_DUAL_SIM and FEATURE_TRIPLE_SIM
12/02/13   ah      Support GPS State Info requests for multiple clients simultaneously
10/16/13   rk      Added generic Glonass AcqAsst processing functions.
09/27/13   mj      TSTS Support: Update DSDS_MAX_SESS_TYPES for TSTS
09/12/13   mj      Add new parameter to function tm_mmgsdi_get_sim_info_from_asid
06/07/13   rh      Removed diag timer and NO_FIX report handling 
02/04/13   kj      Move to the DOG Heartbeat mechanism on DIME and beyond
01/31/13   mj       Add new timer api_callback_timer
11/30/12   mj      Added tm_core_lpp_cp_evt_handler and tm_core_external_cell_db_proc
07/17/12   mjohn   Added premium_services_config into  tm_core_config_s_type
03/01/12   rh      Added tm_core_handle_engine_error_recovery_report()
02/06/12   gk      LPP configuration support
06/03/11   gk      Clear cached position  when delete param or gps lock is sent
01/03/11   gk      Eph throttling rework
10/05/10   ns      Port CL1333689: Save status of gps state info req by SYSD.
06/21/10   gk      Save the cached gps state
03/08/10   gk      Save the eph need list
03/03/10   ns      DSDS Support
09/30/09   gk      Support ODp 2.0
07/23/09   gk      Support ODp 2.0
07/10/09   atien   Spirent requested feature:  allow XTRA dload after MSB dload comm failure
6/10/09    gk      Added support for gps utc offset NV item
06/10/09   gk      ODP 1.1 support
6/04/09    gk      Added support for gps utc offset NV item, other inter-position changes
04/02/09   ns      support for qwip
03/11/09   ns      Add variable to store whether sysd was triggered for
                   this session
03/04/09   ns      Created new Data str. for get gps state
02/04/09   atien   MOLR vs AD Call Flow Select support
01/13/09   atien   XTRA-T support
01/27/09   ns      qWip Integration support changes.
08/23/08   gk      GPS lock to be honored for on-demand
08/12/08   gk      Added force reciever off
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
06/12/08   lt      Support for external coarse position injection.
04/25/08   gk      on-demand changes
01/21/08   cl      Allow overlapping dload and fix (fix and dload) sessions
09/14/07    gk     NMEA CONFIG set parameters support
07/30/07   gk      Add handling for last position
07/17/07   lt      L1 interface.
06/12/07   cl      Only allow PD API access when TM task is active
05/30/07   lt      Mods. to make TM's RRC and MM iface symmetrical to CM and RR.
04/17/07   lt      Added mo_method into tm_core_config_s_type.
04/04/07   gk/mr   Added function for Time injection handling
01/31/07   cl      Added function for LCS handling
12/04/06    cl     Added NV functionality
10/13/06    cl     Change to use SM_TM for dog report
07/12/06    cl     Initial Release
===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#if !defined (WIN32) && !defined (FEATURE_CGPS_PLAYBACK)
#include "task.h"
#endif
#include "aries_os_globals.h"
#include "aries_os_api.h"
#include "tm_common.h"
#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdilib_v.h"
#endif
#include "tm_prtl_iface.h"
#include "pdapi.h"
#include "tm_cm_iface.h"
#include "tm_pdapi_iface.h"
#include "sm_loc_lte_otdoa.h"
#include "tm_api.h"
#include "sm_nv_efs.h"
#ifdef FEATURE_CGPS_LTE_CELLDB
#include "msgr_umid.h"
#include "lte_tlb.h"
#endif

#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH /* this would be defined in custwcdma.h */
#undef FEATURE_WCDMA_GPS_CELL_ID_READING /* this would be defined in custcgps.h */
#endif

#ifdef FEATURE_GPSONE_BTR
   #define GPSONE_SEC_FLAG 1
#else
   #define GPSONE_SEC_FLAG 0
#endif

/* TM-Core Timer Definition Section */
#define TM_CORE_TIMER_DISPATCH_MASK               0xFF000000

/* TM-Core Transaction ID Definition Section */
#define TM_CORE_TRANSACTION_ID_DISPATCH_MASK      0xFF000000


#define TM_CORE_SESS_TIMER_ID_DIAG     0x00000002
#define TM_CORE_TIMER_ID_API_CALLBACK  (0x00000003)
#define TM_CORE_TIMER_ID_DELETE_THROTTLE_ID  (0x00000004)

#define TM_CORE_MIN_VELOCTY            0.15 /* 0.15 m/s */

/* constant for the MSB of msec elapsed from 6 Jan 1980 00:00:00 to 6th Jan 2000*/
#define MSB_MSEC_ELAPSED_SINCE_DEFAULT 75

#define TM_ODP_CLIENT_ID 3333 /* this is defined in pdapiclient.h */
/* TM task info structure */
typedef struct
{
  boolean          task_active;
  os_TimerMsgType  *api_callback_timer;
} tm_task_info_s_type;

/* TM-Core default allowed Session handle mask */
#define TM_CORE_ALLOWED_MT_SESSION_HANDLE_MASK  ( (TM_SESS_HANDLE_1X_CP_MT)\
                                                | (TM_SESS_HANDLE_1X_UP_NI)\
                                                | (TM_SESS_HANDLE_UMTS_CP_NI)\
                                                | (TM_SESS_HANDLE_UMTS_UP_NI)\
                                                | (TM_SESS_HANDLE_FTEST)\
                                                )

/*TM_Core masks to enable AGPS on RAT's*/
#define TM_CORE_NV_RAT_ENABLE_CDMA_MASK  0x1
#define TM_CORE_NV_RAT_ENABLE_HDR_MASK   0x2
#define TM_CORE_NV_RAT_ENABLE_WCDMA_MASK 0x4
#define TM_CORE_NV_RAT_ENABLE_GSM_MASK   0x8
#define TM_CORE_NV_RAT_ENABLE_LTE_MASK   0x10
#define TM_CORE_NV_RAT_ENABLE_TDSCDMA_MASK   0x20
#define TM_CORE_NV_RAT_ENABLE_ALL_MASK   0xFF

/* TM-Core E911 behavior mask. This mask used for NV GNSS_NV_EFS_SM_E911_CONFIG */
/* Do not abort any ongoing GPS session on E911 Orig. Used by Some OEMs who want
 * MO, user plane E911 positioning. */
#define TM_CORE_DONT_ABORT_GPS_SESSION_ON_E911 0x1

/* Used in SUPL & LPP */
#define TM_LPP_MAX_OTDOA_RSTD_MEAS_DATA 3

/* channelNumber INTEGER (-7..13) as per RRLP 8.6.0, LPP 9.6.0, RRC R8 */
#define C_PROT_GLO_FREQ_MIN GLO_LOWEST_FREQ_NUMBER
#define C_PROT_GLO_FREQ_MAX 13

/* GPS, GLONASS, BDS, GAL SV used as position source to compute the fix (by PE) */
#define C_POS_SOURCE_GPS (0x1<<0)
#define C_POS_SOURCE_GLO (0x1<<1)
#define C_POS_SOURCE_BDS (0x1<<2)
#define C_POS_SOURCE_GAL (0x1<<3)
/* Glonass Sv/Channel# sub-buffer structure. Source of data Glo AuxiInfo and/or Glo Almanac */
typedef struct
{
  struct
  {
    unsigned channelNumValid : 1; /* Set to 1 when channel_num is valid */
   } m;
  int8 channel_num;                 /* Insert valid frequency number -7..13 */
} prot_glo_cn_buffer_s_type;

/* Glonass Sv/Channel# buffer structure. Source of data Glo AuxiInfo and/or Glo Almanac */
typedef struct
{
  uint8 u_n;                                            /* Number of valid Sv/ChannelNum */
  prot_glo_cn_buffer_s_type z_glo_sat_elem[N_GLO_SV];   /* Array index is Glonass Sv Id of 0..23 */
} prot_glo_svcn_buffer_s_type;

/* Glonass Acquisition Assistance structure */
typedef struct
{
  struct
  {
    unsigned gnssMsecOfWeekValid : 1;    /* Set 1 for z_steering_glo_data.z_SvSteerInfo.q_Ms to be valid */
    unsigned svDirInjected : 1;          /* When this is 1, the SV direction AD is successfully injected into MGP  */
    unsigned svSteeringInjected : 1;     /* When this is 1, the SV steering AD is successfully injected into MGP */
  } m;

  boolean                          u_freq_num_valid;   /* TRUE if Glonass frequency number in below svdir & steering are valid */
  gnss_SvDirStructType             gnss_sv_dir;         /* SV direction info */
  gnss_SvAcqAssistCommonStructType z_steering_glo_data;/* Steering info */
} prot_glo_acqast_buffer_s_type;

/* Subset of Glonas Acquisition Assistance parameters (Satellite ID/Integer code phase pair) */
typedef PACKED struct PACKED_POST
{
  uint8 satellite_id;         /* 0..63 */
  uint32 integer_code_phase;  /* 0..127 */
} prot_glo_acqasst_subset_s_type;

/* Store subset of Glonass Acquisition Assistance */
typedef struct
{
  uint8 u_cnt;        /* Array size of p_svid_icp */
  prot_glo_acqasst_subset_s_type *p_svid_icp;
} prot_glo_acqasst_sat_buffer_s_type;

/* Glonass Navigation Model structure */
typedef struct
{
  boolean             u_freq_num_valid;
  glo_EphStructType   z_eph_glo_data;
} prot_glo_eph_buffer_s_type;

typedef struct tm_otdoa_data_store
{
  struct
  {
    unsigned uMultiOtdoaMeasValid : 1; /* If set to 1, there are more than 1 OTDOA measurement sets (i.e. >24 measurements) */
  }m;

  uint8 u_num_otdoa_meas_set_avail;   /* Total num of OTDOA data added to the list (i.e. pz_otdoa_data_list) */
  uint8 u_tot_otdoa_meas_set_sent;    /* Total OTDOA measurement set (of 24) sent to server/network from the list (i.e. pz_otdoa_data_list).
                                       When all measurements are sent to network "u_tot_otdoa_meas_set_sent == u_num_otdoa_meas_set_avail" is TRUE */
  sm_loc_lte_otdoa_meas_info_type *pz_otdoa_data_list[TM_LPP_MAX_OTDOA_RSTD_MEAS_DATA];   /* List of OTDOA data. Max 3. */
} tm_otdoa_data_store; 

/*===========================================================================
           Definition of TM-Core Data Structure
===========================================================================*/
typedef void (prtl_init_f_type)(void);

typedef enum
{
  TM_CORE_SESS_STATE_INIT = 0,
  TM_CORE_SESS_STATE_WAIT_AUTH,
  TM_CORE_SESS_STATE_ACCESS_GRANTED,
  TM_CORE_SESS_STATE_RESOURCE_CLEANUP,
  TM_CORE_SESS_STATE_COMPLETED
} tm_core_session_state_e_type;

/* Enum for all location engine combinations allowed on sysd*/
typedef enum
{
  SYSD_AUTOMATIC,
  SYSD_WIFI_ONLY,
  SYSD_WIFI_OFF,
  SYSD_AUTOMATIC_IN_MSBASED_ONLY,
  SYSD_ALL_LOC_ENG_ALWAYS_ON
}tm_sysd_loc_engine_cfg_e_type;

/* Enum for sysd status on gps state info req pending*/
typedef enum
{
  SYSD_STATE_INFO_REQ_NOT_SET,
  SYSD_STATE_INFO_REQ_PENDING,
  SYSD_STATE_INFO_REQ_DISCARD
}tm_sysd_st_info_req_status_e_type;

/* Struct for get pos request*/
typedef struct
{
  pdsm_client_type_e_type       client_type;
  pdsm_client_id_type           client_id;
  pdsm_mt_initiator_e_type      mt_initiator;  /* 0 (not applicable), (1) CP, or (2) UP */
  pdsm_pd_option_s_type         option;        /* PD option type */
  pdsm_pd_qos_type              qos;           /* Ptr to user QOS for this request */
  boolean                       internal_session;
} tm_queued_get_pos_param_s_type;


/* union for tm core get gps state payload*/
typedef  union
{
  pdsm_cmd_s_type *p_cmd_type;
}tm_get_gps_state_payload_u_type;

/* Struct for tm core get gps state payload*/
typedef struct
{
  sm_GpsStateInfoReqEnumType source;
  tm_get_gps_state_payload_u_type cmd;
} tm_core_get_gps_state_param_s_type;

typedef tm_core_get_gps_state_param_s_type tm_core_get_best_avail_pos_param_s_type;
typedef tm_core_get_gps_state_param_s_type tm_core_get_avail_wwan_pos_param_s_type;
typedef tm_core_get_gps_state_param_s_type tm_core_get_available_pos_param_s_type;


typedef struct
{
  uint32 q_ClientId;
  uint16 w_TmRcvdDataMask;     /* Bitmask to indicate which data type has been received so far */
} tm_GpsStateInfoClientType;

#define TM_CORE_SESS_INFO_CACHED_INFO_STATE_VALID 0x0001

typedef struct
{
  /* TM-Core Active Session Handle */
  tm_sess_handle_type         active_sess_handle;

  /* Current Operation Mode */
  tm_sess_operation_mode_e_type  op_mode;

  /* Operation already Requested */
  tm_op_req_type                 op_requested;

  /* Protocol sub-module the active session handle belongs to */
  tm_prtl_type                from_protocol;

  /* TM-Core State */
  tm_core_session_state_e_type session_state;

  /* MO session handle seed */
  unsigned short              mo_sess_handle_seed;

  /* Client information */
  prtl_start_sess_req_param_s_type pd_param;

  /* Session start/done Event firing */
  boolean                    session_event_firing;

  /* Cached eph and alm have-list */
  uint32                     eph_svmask;
  uint32                     alm_svmask;
  uint32                     iono;
  uint32                     utc;
  uint32                     health_svmask;
  uint32                     valid_svmask;

  uint32                     sbas_eph_svmask;
  uint32                     sbas_alm_svmask;

  uint32                     glo_eph_svmask;
  uint32                     glo_alm_svmask;
  uint32                     glo_iono;
  uint32                     glo_utc;
  uint32                     glo_health_svmask;

  uint64                     bds_eph_svmask;
  uint64                     bds_alm_svmask;
  uint32                     bds_iono;
  uint32                     bds_utc;
  uint64                     bds_health_svmask;

  uint32                     qzss_eph_svmask;
  uint32                     qzss_alm_svmask;
  uint32                     qzss_iono;
  uint32                     qzss_utc;
  uint32                     qzss_health_svmask;
  uint64                     gal_eph_svmask;
  uint64                     gal_alm_svmask;
  uint32                     gal_iono;
  uint32                     gal_utc;
  uint64                     gal_health_svmask;

  boolean                         queued_request;
  tm_queued_get_pos_param_s_type  queued_get_pos_param;
  boolean                         force_eph_download;
  boolean                         glo_force_eph_download;
  boolean                         bds_force_eph_download;
  boolean                         gal_force_eph_download;
  uint32 q_FixTimeRemainingMSec;

  /* Allow XTRA Dload after MSB DLOAD COMM Failure */
  boolean                    msb_xtra_allowed;
  boolean                    end_odp_session;
  boolean                    pos_inj_requested;
  /* save the GPS eph need request coming from PE */
  uint32                     eph_need_sv_mask;
  /* save the GLONASS eph need request coming from PE */
  uint32                     glo_eph_need_sv_mask;
  /* save the BDS eph need request coming from PE */
  uint64                     bds_eph_need_sv_mask;
  /* save the GAL eph need request coming from PE */
  uint64                     gal_eph_need_sv_mask;
  /* state info flags. use this instead of adding new booleans */
  uint32                     sess_info_valid_flags; /* see the flag definitions above */
  /* Was the utc time decoded over the air or from the network */
  boolean                    trusted_utc_time;

  /* Request for MGP receiver off */
  boolean                    receiver_off_needed;

  /* Bitmask to indicate if request for client (1<<sm_GpsStateInfoReqEnumType) is on-going */
  uint32                     q_ReqClientMask;

  /* Structure that holds requesting client specific data (multiple client support) */
  tm_GpsStateInfoClientType  z_ReqClients[SM_GPS_STATE_INFO_REQ_TYPE_MAX];

  /* TRUE if Request XTRA download triggered by AGPS session(SUPL/V2 etc) */
  boolean                    req_xtra_trig_by_agps;

} tm_core_sess_info_s_type;

typedef struct
{
  /* Save client_id for returning back to PD API module */
  pdsm_client_id_type   client_id;
} tm_core_pa_client_info_s_type;

typedef struct
{
  prtl_init_f_type                  *init_fp;
  prtl_start_sess_req_f_type        *start_sess_req_fp;
  prtl_stop_sess_req_f_type         *stop_sess_req_fp;
  prtl_session_req_f_type           *sess_req_data_fp;
  prtl_session_info_f_type          *sess_info_fp;
  prtl_data_pipe_handle_f_type      *data_pipe_handle_fp;
  prtl_timer_cb_f_type              *timer_cb_fp;
  prtl_event_cb_f_type              *event_cb_fp;
  prtl_general_query_handle_f_type  *general_query_fp;
  prtl_gm_event_handle_f_type       *gm_event_handle_fp;
} tm_core_prtl_cb_s_type;

typedef struct
{
  /* Timeout in seconds after which GNSS unavail indication is sent out from GM in a bad GNSS environment */
  uint32 sm_gm_gnss_unavail_ind_timeout;

  /*Geofence Motion Detection Sources*/
  uint32 sm_gm_motion_detection_sources;
  /*Geofence GNSS session timeout*/
  uint8 sm_gm_gnss_pos_qos_sess_timeout;
  /*Geofence Position Sources*/
  uint32 sm_gm_position_sources;
  /*Geofence Max pos unc accepted */
  uint32 sm_gm_gnss_max_pos_unc_accepted;

  /* Geofence use network assisted fixes */
  boolean sm_gm_use_nw_asst_fixes;
   
  /*High Responsiveness Config*/
  uint32 sm_gm_high_responsiveness_config;

  /*GM OEM Control Mask*/
  boolean sm_gm_oem_control_mask;
  /* GM CPI request rate */
  uint32 sm_gm_cpi_request_rate;

  /*GM Medium resp. backoff in seconds */
  uint32 sm_gm_med_resp_backoff;

  /* Backoff config when in Challenging GPS Env in seconds */
  gm_chal_gps_env_backoff_config_s_type sm_gm_chal_gps_env_bo_config;

  /* The motion sensing distance config in case of challenging GPS env */
  gm_chal_gps_env_ms_dist_config_s_type sm_gm_chal_gps_env_ms_dist_config;

  /*The motion state speed config used by Geofencing*/
  gm_ms_speed_config_s_type sm_gm_motion_state_speed_config;
  
  /*Comprehensive Logging Rate*/
  uint32 q_gm_comp_logging_rate;  

  /* Enable GM LOWI registration */
  boolean sm_gm_lowi_reg_enable;

  /* Mask to subscribe to different services from LOWI */
  uint32 sm_gm_lowi_sub_mask;

  /* Configure how GM registers with LOWI */
  gm_lowi_reg_config_s_type sm_gm_lowi_reg_config;

  /*The ALS IOD sensor config mask used by Geofencing*/
  uint32 q_sm_gm_iod_config_switch_mask;

  /*GM filtering of Wlan freq mask*/
  uint32 q_sm_gm_filter_wlan_freq_mask;
  
} geofence_engine_config_s_type;

typedef struct
{
  /* Set Premium Services Config ongoing (TRUE) or not (FALSE) */
  boolean                          prem_svc_cfg_waiting;

  /* If Set Premium Services Config is ongoing, below parameters are valid */
  pdsm_client_id_type              client_id;
  pdsm_pa_premium_svc_cfg_s_type   premium_svc_cfg;
} tm_core_prem_svc_cfg_info;


/* Below are carrier-specific EFS NVs */
typedef struct
{
/* Below from legacy/non-EFS NV */
  /* GPS Lock */
  pdsm_gps_lock_e_type       gps_lock;

  /* NMEA String Output Format */
  uint32                      nmea_output_format;

/* Below from EFS NV */
  /*AGPS rat preference*/
  cgps_nv_agps_rat_pref_config  agps_rat_pref;

  /* Selected protocol (RRLP, RRC, LPP etc) for A-Glonass */
  uint32                      asst_glonass_pos_protocol; // Use PDSM_PA_ASST_GLO_PROTOCOL_XXX to know the choices

  /* LPP enable */
  uint8                       gnss_lte_lpp_enable;

  /*1X UP SUPL Enable*/
  uint8                       gnss_1x_up_supl_enable;

  /* SUPL UDP protocol setting to control UDP BIND */
  uint8                       gnss_sm_udp_allow;

  /* Enable/Disable CPI injections and Early Exit logic */
  uint32                      gnss_sm_emergency_support_config;

  /* Positioning Modes Supported  (NV 1920) */
  uint32                      positioning_modes_supported;

} tm_core_config_carrier_specific_s_type;

/* macro for easier access to carrier-specific configuration */
#define TM_CORE_CONFIG_DD(x)  (tm_core_info.config_info.subs_cfgs[tm_core_info.config_info.active_subs_cfg].x)

#define TM_CONFIG_POS_MODE_BIT_IS_SET(x) \
    ( (tm_core_info.config_info.subs_cfgs[tm_core_info.config_info.active_subs_cfg].positioning_modes_supported) & (x) )


typedef struct
{
  /* UMTS' MO type: CP-LocEstimate, CP-AssistData, and UP:
     initialized to NV_Item cgps_mo_method and can be updated by pdsm_setparam() */
  uint8                      u_mo_method;

  /* GLONASS support indicator */
  uint8                      gnss_glo_control;

  pdsm_pa_nmea_config_s_type nmea_config_type;
  /* smart msb enable/disable */
  boolean                    smart_msb;
  /* ODP related NV items */
  boolean                    on_demand_lpm;
  boolean                    on_demand_ready;

  /* UMTS CP MOLR vs AD all Flow Select Mask */
  /*  initialized to NV_Item: aagps_2g_mo_lrsupport and aagps_3g_mo_lrsupport */
  /*  can be updated by pdsm_setparam() */
  uint8                      call_flow_select_mask;
  /* Default UTC offset */
  uint8                      nv_gps_utc_offset;
  /* ODP 2.0 enable QWIP for ODP */
  boolean                    use_qwip_for_odp;
  /* User plane 1xMSA sessions may be turned off using nv if required, to
   * conserve repeated reqs. for assistance data to the network */
  boolean                    up_1x_msa_disable;

  /* emergency services support */
  byte                       aagps_emergency_services_spprt;
  /* preferred xtra data validity age */
  uint16                     preferred_xtra_age;
  /* Only bit 2 used for simulating emergency call.  Bits 6-8 used by PE */
  uint32                     aagps_development_test_control5;
  /* if set to 1, TM will send PE's NO_FIX report as IntermediateReport */
  uint8                      send_no_fix_report;

  /* NMEA callback counter */
  uint8                       nmea_cb_count;

  /* TRUE to accept 1X CP MT even if MO session is active
     Controlled by NV NV_GNSS_OEM_FEATURE_MASK_I bit 2(0x4) */
  boolean                    accept_1xcp_mt_over_mo;

  /*QMI Services config*/
  pdsm_QMIServiceConfig         qmi_cfg;

  /* Config for Premium Services (Enhanced Cell ID) */
  uint32                      premium_services_config;

  /* Set Premium Services (SAP) Config request while waiting for indication status */
  tm_core_prem_svc_cfg_info   prem_svc_cfg_info;

  /* LPP CP Transport transmission timeout in seconds */
  uint8                       gnss_lpp_cp_timeout_secs;

  /* OEM DRE Tunnel Detection Enablement control flag */
  boolean                     oemdre_td_ctrl;

  /* Emergency support on CP or UP for non-lTE RAT */
  uint32   gnss_emergency_protocol_non_lte;

  /* OEM feature bitmask NV: bit 0x08/0x10 to indicate to pdsm for auto-loop SUPL notification-verification
     back to TM directly. See sm_nv.h for complete OEM feature bit definitions  */
  uint32                    q_gnss_oem_feature_mask;

  /* notify SUPLINIT to AP if GPS locked */
  boolean b_supl_notify_to_ap;

  /*Geofence Engine Config*/
  geofence_engine_config_s_type gm_config;

  /* Timeout value for SM to wait for WiFi scan: if 0, the feature is disabled */
  uint8    wifi_wait_timeout_sec;
  /* auto accept MT-LR during emergency */
  boolean b_auto_accept_mtlr;

  /* end ongoing LPP periodic session after emergency end */
  boolean b_end_lpp_periodic;

  /* E911 Config mask */
  uint32 sm_e911_config;

  /* GNSS Measurement report satellite constellation config */
  uint16	 gnss_meas_rep_constell_config;

  /* SV Polynomial report satellite constellation config */  
  uint16	 sv_poly_rep_constell_config;

  /* GNSS Measurement report QMI output control via NV */
  uint8		gnss_meas_rep_qmi_out_control;

  /* GNSS Transmission Delays for Protocols  from NV */
  tm_config_agnss_transmission_delay_s_type agnss_trans_delay_config;

  /* end SUPL session after a final fix is reported even if SUPL is communication with server */
  boolean                    b_end_supl_after_final_fix; 

  /* Advertise UE support for >24 OTDOA AD processing & generate measurements */     
  uint8 u_gnss_lpp_grt_24_otdoa;

  /* Below are for MultiSIM EFS NV support */
  /* index for currently active */
  uint32 active_subs_cfg;

  /* array to store configurations for all Subs */
  tm_core_config_carrier_specific_s_type  subs_cfgs[SM_CONFIG_MAX_SUBS];

  /* last valid Subscription ID (0 for Single SIM) */
  sys_modem_as_id_e_type max_valid_subs_id; 

  /* GNSS NHz Config */
  /* For normal operation, this would have value defined in gnss_common.h
     #define GNSS_NHZ_ENABLED 0x0001
     #define GNSS_NHZ_MODE_AUTOMOTIVE 0x0002
     #define GNSS_NHZ_MODE_DRONE 0x0004
  */
  uint32 gnss_nhz_config;

  /* Process earlyTimer value in QOS for LPP Rel12 */ 
  uint8 u_gnss_lpp_rel12_2timer_support; 

} tm_core_config_s_type;

#define TM_WIFI_WAIT_TIMEOUT_SEC_MAX  10  /* Wait-for-WIFI-scan-timeout caps at 10 seconds */

#define  TM_RCVD_POS_FROM_PE         0x01
#define  TM_RCVD_TUNC_FROM_ME        0x02
#define  TM_RCVD_ASSIST_DATA_FROM_PE 0x04
#define  TM_RCVD_ALL (TM_RCVD_POS_FROM_PE | TM_RCVD_TUNC_FROM_ME | TM_RCVD_ASSIST_DATA_FROM_PE)

typedef struct
{
  uint32 flags;
  int32  lat;                  /* Latitude:  LSB = Pi/2^32, Range -Pi/2 to Pi/2 */
  int32  lon;                  /* Longitude: LSB = Pi/2^31, Range -Pi to Pi */
  int16  altitude;             /* Altitude in meters */
  uint32 pos_unc;              /* Position Unc in meters */
  uint16 loc_unc_vertical;     /* vertical unc in meters */
  dword  timestamp_tow_ms;     /* Timestamp in GPS TOW milliseconds */
  uint16 timestamp_gps_week;   /* GPS week number */
  uint32 time_unc_us;          /* Time uncertainty in micro seconds */
  uint32 frequency_unc;        /* 1 sided max freq bias uncertainty [m/s] */
  uint32 clk_drift;            /* Clock Drift in meters/sec */
  uint32 clk_bias;             /* Clock frequency bias in meters/sec */
  uint8  engine_state;         /* GPS engine state on/off */
  uint32 eph_svmask;           /* SV Mask for the ephemeris */
  uint32 alm_svmask;           /* SV Mask for the Almanac */
  uint32 iono_valid;           /* IONO is valid */
  uint32 health_svmask;        /* SV Mask for Health */
  uint32 health_bad_svmask;    /* SV Mask for Health */
  uint32 health_noex_svmask;   /* SV Mask for Health */
  uint32 health_unk_svmask;    /* SV Mask for Health */
  uint32 visible_svmask;       /* SV mask for visible SVs */
  uint16 start_gps_week;       /* Current XTRA info is good starting this GPS week */
  uint16 start_gps_minutes;    /* Current XTRA information is good starting this many minutes into the week */
  uint16 valid_duration_hours; /* XTRA info is valid for this many hours */

} tm_core_gps_state_info_s_type;

#ifdef FEATURE_GNSS_PERIODIC_PPM
#error code not present
#endif

typedef struct
{
  /* TM task Info */
  tm_task_info_s_type           tm_task_info;

  /* TM Configuration Info (NV settings) */
  tm_core_config_s_type         config_info;

  /* Session Relelated Info */
  tm_core_sess_info_s_type      session_info;

  /* Session Relelated Info */
  tm_core_pa_client_info_s_type pa_client_info;

  /* TM-Core Function Table */
  tm_core_prtl_cb_s_type        prtl_func_cb_table[TM_PRTL_NUM];

  /* Extended data */
  pdsm_pd_data_ext_s_type       pd_ext_data;
  pdsm_pd_opt_field_ext         ext_opt_field_mask; /* validity bits for more fields in the struct pd_ext_data*/
  gnss_MeasBlkStructType        gps_meas_blk;
  gnss_MeasBlkStructType        glo_meas_blk;
  gnss_MeasBlkStructType        bds_meas_blk;
  gnss_MeasBlkStructType        qzss_meas_blk;
  gnss_MeasBlkStructType        gal_meas_blk;
  tm_core_gps_state_info_s_type gps_state_info;

  /*Cached MGP receiver state*/
  gps_RcvrStateEnumType         e_cached_rcvr_state;

  /*QWiP Location Engine Configurations*/
  tm_sysd_loc_engine_cfg_e_type loc_engine_config;
  /* QWiP Triggered for this session or not */
  boolean                       qwip_sysd_triggered;
  tm_sysd_st_info_req_status_e_type  qwip_st_info_req_status;

#ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#endif
  /*If this session is internal*/
  boolean                       internal_session;
  boolean b_pending_ni_resp_from_client;
  os_TimerMsgType*               tm_delete_param_throttle_timer;     /*  throttle timer to throttle back to back delete requests from PDAPI */
} tm_core_param_s_type;

#ifdef FEATURE_UIM_SUPPORT_LBS
typedef struct
{
   boolean dynamic_mode;
   uint16  seed_pos_options;
}tm_core_ruim_read_buffer;
#ifdef FEATURE_CGPS_XTRA 
typedef struct
{
  boolean  xtra_enabled;
  uint16   download_interval;
  uint8    num_download_attempts;
  uint8    time_between_attempts;
  boolean  auto_download_enabled;
  char     xtra_server_primary[128];     // URL of primary XTRA server
  char     xtra_server_secondary[128];   // URL of secondary XTRA server
  char     xtra_server_tertiary[128];    // URL of tertiary XTRA server
}tm_xtra_ruim_nv_buffer;
#endif /* FEATURE_CGPS_XTRA */
#endif /* FEATURE_UIM_SUPPORT_LBS */

typedef struct
{
  boolean             cached_pos_valid;
  gnss_NavSlnFlagsStructType z_pos_flags;
  pdsm_pd_event_type  pd_event_type;
  pdsm_pd_info_s_type pd_info;
} tm_core_cached_pos_s_type;

typedef struct
{
  boolean                  cached_meas_valid ;
  sm_GpsMeasRptStructType  z_meas;
} tm_core_cached_meas_s_type;


typedef struct
{
  boolean           cached_ref_pos_valid ;
  pdsm_ext_status_info_s_type   pd_ext_status_info;
  boolean           ref_pos_reported;
} tm_core_cached_ref_pos_s_type;

#ifdef FEATURE_MMGSDI_SESSION_LIB
    #define DSDS_MAX_SESS_TYPES   6


typedef struct tm_mmgsdi_session_id_table_entry_s
{
  mmgsdi_session_type_enum_type session_type;
    /* MMGSDI session type GW/1X and PRI/SEC */

  mmgsdi_session_id_type        session_id;
    /* Session-ID assigned by MMGSDI */

  sys_modem_as_id_e_type        asubs_id;
    /* Active Subscription ID assigned by to CM
    ** If not available, set to NONE
    */
  boolean session_id_set;
  mmgsdi_app_enum_type         app_type;
} tm_mmgsdi_session_id_table_entry_s_type;

#endif /*  FEATURE_MMGSDI_SESSION_LIB*/

typedef struct
{
  uint8 					 no_of_sims;
  uint8						 no_of_active_subs;
  tm_core_device_mode_e_type tm_cm_device_info;
  sys_overall_feature_t      tm_cm_overall_device_feature;
  sys_subs_feature_t		 tm_cm_subs_feature[TM_CM_MAX_NO_SUBS];
  sys_ue_mode_e_type		 tm_cm_subs_ue_mode[TM_CM_MAX_NO_SUBS];
} tm_core_device_config_s_type;

/* Gobal variable which store device configuration info such as SS/DSDS/DSDA/TSTS */
tm_core_device_config_s_type tm_core_device_config;

typedef enum
{
  TM_LPPM_REQ_TYPE_EXTERNAL,
  TM_LPPM_REQ_TYPE_INTERNAL
} tm_lppm_requestor_e_type;

#define SM_TOT_GNSS_MEAS_SYSTEM         5     /// Total GNSS system measurement types supported (GPS + GLONASS + BDS +GAL +QZSS)
#define SM_GNSS_MEAS_SYSTEM_GPS         0
#define SM_GNSS_MEAS_SYSTEM_GLONASS     1
#define SM_GNSS_MEAS_SYSTEM_BDS         2
#define SM_GNSS_MEAS_SYSTEM_GAL         3
#define SM_GNSS_MEAS_SYSTEM_QZSS        4
#define SM_GNSS_MEAS_SYSTEM_INVALID     -1

/* Bitmap for measurement report */
#define SM_GNSS_MEAS_REPORTS_BMAP_GPS     (1<<0)
#define SM_GNSS_MEAS_REPORTS_BMAP_GLONASS (1<<1)
#define SM_GNSS_MEAS_REPORTS_BMAP_BDS     (1<<2)
#define SM_GNSS_MEAS_REPORTS_BMAP_GAL     (1<<3)
#define SM_GNSS_MEAS_REPORTS_BMAP_QZSS    (1<<4)

typedef struct
{
  uint8                     u_meas_reports_bmap; /* Use SM_GNSS_MEAS_REPORTS_BMAP_XXX to check which measurement
                                                    report is filled */
  sm_GpsMeasRptStructType   z_meas_reports[SM_TOT_GNSS_MEAS_SYSTEM]; /* Use u_meas_reports_bmap to check which measurement
                                                                        report element is populated. */ 
} SmGnssMeasReportStructType;

/*--------------------------------------

      Extern Valuables

---------------------------------------*/
extern tm_core_param_s_type     tm_core_info;

/* For DSDS, EFS cert can be updated anytime to
   program SUPL CERT for a new subscription.
   This boolean will force an EFS read of SUPL
   cert if PDAPI wrote new SUPL CERT into EFS.

*/
extern boolean  tm_use_efs_for_supl_cert;
/* Store the current dd subscription id*/
extern sys_modem_as_id_e_type      tm_current_dd_subs_id;
/*-----------------------------------------------------------------------------

                          Extern Functions

-----------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION tm_core_init

DESCRIPTION
  This function is to initialize TM-Core data structures.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_init(void);

/*===========================================================================

FUNCTION tm_core_session_active

DESCRIPTION
  This function tells if tmCore is in any active session (MI or MT)

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_session_active(void);

/*===========================================================================

FUNCTION tm_core_abort_recover

DESCRIPTION
  This function performs TM-Core software recovery.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_abort_recover
(
  boolean                       ask_prtl,
  tm_sess_stop_reason_e_type    abort_reason
);

/*===========================================================================

FUNCTION tm_core_delete_inject_aiding_data

DESCRIPTION
  This function is called to delete or inject aiding data.

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
extern void tm_core_delete_inject_aiding_data
(
  pdsm_delete_parms_type *delete_param_ptr,
  boolean                 force_rcvr_off,
  boolean                 delete_all,
  pdsm_pa_e_type delete_type
);

/*===========================================================================

FUNCTION tm_core_inject_ext_time

DESCRIPTION
  This function is called to inject external time into MGP through LM

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
extern void tm_core_inject_ext_time
(
  pdsm_pd_external_time_info_type *ext_time_ptr
);

/*===========================================================================

FUNCTION tm_core_inject_ext_pos

DESCRIPTION
  This function is called to inject external position into MGP through LM

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_inject_ext_pos
(
  pdsm_cmd_s_type *p_cmd_type
);


/*===========================================================================

FUNCTION tm_core_get_gps_status

DESCRIPTION
  This function is called to get the GPS status information

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gps_status
(
  pdsm_cmd_s_type *p_cmd_type
);


/*===========================================================================

FUNCTION tm_core_update_session_state

DESCRIPTION
  Determine if the state transition is valid and update the tm session
  state.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
extern void tm_core_update_session_state
(
  tm_core_session_state_e_type new_state
);

/*===========================================================================

FUNCTION tm_core_new_mo_sess_handle

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern tm_sess_handle_type tm_core_new_mo_sess_handle(void);

/*===========================================================================

FUNCTION tm_core_timer_dispatcher

DESCRIPTION
  This function is to figure out which TM software sub-module the timer expiry event
  belongs to and call the proper timer handler (provided to TM-Core by each sub-module)

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_timer_dispatcher(void *timer_data);

/*===========================================================================

FUNCTION tm_core_lm_sess_req_handler

DESCRIPTION
  This function is to handle sess request from LM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_lm_sess_req_handler(void *lm_sess_req_data);

/*===========================================================================

FUNCTION tm_core_lm_sess_update_handler

DESCRIPTION
  This function is to handle sess update from LM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_lm_sess_update_handler(void *lm_sess_update_data);

/*===========================================================================

FUNCTION tm_core_lm_sess_info_handler

DESCRIPTION
  This function is to handle sess info from LM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_lm_sess_info_handler(void *lm_sess_info_data);

/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_type

DESCRIPTION
  This function is to get protocol array index by protcol type

DEPENDENCIES

RETURN VALUE
  -1, if protocol is not supported.
  Otherwise, array index for the protocol type

SIDE EFFECTS

===========================================================================*/
extern int  tm_core_get_prtl_idx_by_type(tm_prtl_type prtl_type);

/*===========================================================================

FUNCTION tm_core_get_prtl_idx_by_lcs_cmd_type

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  protol array index if lcs_cmd_type is supported
  -1, otherwise

SIDE EFFECTS

===========================================================================*/
extern int tm_core_get_prtl_idx_by_lcs_cmd_type(pdsm_lcs_info_cmd_e_type  lcs_info_cmd_type);

/*===========================================================================

FUNCTION tm_core_sess_req_start

DESCRIPTION
  This function is to figure out if TM-Core accepts session start request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_sess_req_start(tm_prtl_type                    prtl_type,
                                      tm_sess_handle_type             sess_handle,
                                      tm_sess_req_start_param_s_type  sess_start_param);
/*===========================================================================

FUNCTION tm_core_sess_req_continue

DESCRIPTION
  This function is to figure out if TM-Core accepts session continue request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_sess_req_continue(tm_prtl_type                       prtl_type,
                                         tm_sess_handle_type                sess_handle,
                                         tm_sess_req_continue_param_s_type  sess_continue_param);

/*===========================================================================

FUNCTION tm_core_sess_req_polling_data

DESCRIPTION
  This function is used to poll LM is data is available

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_sess_req_polling_data(tm_prtl_type                           prtl_type,
                                             tm_sess_handle_type                    sess_handle,
                                             tm_sess_req_polling_data_param_s_type  polling_data_param);


/*===========================================================================

FUNCTION tm_core_sess_req_stop

DESCRIPTION
  This function is to figure out if TM-Core accepts stop request from
  protocol sub-module.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_sess_req_stop(tm_prtl_type                       prtl_type,
                                     tm_sess_handle_type                sess_handle,
                                     tm_sess_req_stop_param_s_type      sess_stop_param);


/*===========================================================================

FUNCTION tm_core_recv_data

DESCRIPTION
  This function is to handle data sent from protocol sub-modules.

DEPENDENCIES

RETURN VALUE
  TURE: If TM-Core sub-module accepts it
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_recv_data(tm_prtl_type                     prtl_type,
                                 tm_sess_handle_type              sess_handle,
                                 tm_post_data_e_type              data_type,
                                 const tm_post_data_payload_type  *data_payload_ptr
                                );

/*===========================================================================

FUNCTION tm_core_recv_event

DESCRIPTION
  This function is to handle PD API Events sent from protocol sub-modules.

DEPENDENCIES

RETURN VALUE
  TRUE: If TM-Core sub-module accepts it
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_recv_event( tm_prtl_type         prtl_type,
                                   tm_sess_handle_type  sess_handle,
                                   pdsm_pd_event_type   pd_event,
                                   pdsm_pd_info_s_type* pd_info_ptr,
                                   gnss_NavSlnFlagsStructType *p_PosSrcFlags
                                 );

/*===========================================================================

FUNCTION tm_core_get_active_prtl_module

DESCRIPTION


DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern tm_prtl_type  tm_core_get_active_prtl_module (pdsm_mt_initiator_e_type  mt_initiator);

/*===========================================================================

FUNCTION tm_core_pdcomm_evt_handler

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_pdcomm_evt_handler(uint32 msg_id, void *writeAckData);


/*===========================================================================

FUNCTION tm_core_cm_notify_event

DESCRIPTION
  This function is called by TM-CM module to inform TM-Core phone events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_cm_notify_event
(
  tm_phone_event_e_type          cm_event_type,
  tm_phone_event_payload_u_type  *evt_payload_ptr
);

/*===========================================================================

FUNCTION tm_core_start_emergency_session

DESCRIPTION
  This function starts a TM_CORE session to handle emergency positioning

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_start_emergency_session(void);

#ifdef FEATURE_CGPS_UMTS_CP_GSM
/*===========================================================================

FUNCTION tm_core_rr_notify_event

DESCRIPTION
  This function is called by TM-RR IFACE to inform TM-Core of RR events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_rr_notify_event
(
  prtl_rr_event_s_type  *   p_event
);
#endif

/*===========================================================================

FUNCTION tm_core_rrc_notify_event

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming RRC message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_rrc_notify_event( prtl_rrc_event_s_type *p_event );


/*===========================================================================

FUNCTION tm_core_mm_notify_event

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming MM message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_mm_notify_event( prtl_mm_event_s_type  *p_event );


#ifdef  FEATURE_CGPS_LTE_CELLDB
/*===========================================================================

FUNCTION tm_core_lte_nas_ota_reset_notify_event

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming LTE OTA Reset message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_lte_nas_ota_reset_notify_event( prtl_event_e_type const p_PrtlEvent );
#endif

/*===========================================================================

FUNCTION tm_core_cell_db_proc

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming cell database update message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_cell_db_proc( void *cell_db_info_ptr );

/*===========================================================================

FUNCTION tm_core_l1_msg_proc

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming message from L1.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

extern void tm_core_l1_notify_event ( prtl_l1_event_s_type *p_event );

/*===========================================================================

FUNCTION tm_core_lte_l1_msg_proc

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming message from LTE L1.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

extern void tm_core_lte_l1_notify_event ( prtl_lte_l1_event_s_type *p_event );



/*===========================================================================

FUNCTION tm_core_get_last_position

DESCRIPTION
  This function is called by TmCore to return a cached position

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_last_position (tm_core_cached_pos_s_type *pd_cached_pos );


#ifdef FEATURE_CGPS_XTRA
/*===========================================================================
FUNCTION tm_core_is_xtra_operation_allowed

DESCRIPTION
Check if assisted mode GPS session are in progress.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_xtra_operation_allowed
(
  void
);
#ifdef FEATURE_UIM_SUPPORT_LBS
/*===========================================================================

FUNCTION tm_xtra_ruim_init_and_read

DESCRIPTION
  This function reads NV items Asynchronusly to set TM-XTRA configuration
  info structure.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_xtra_ruim_init_and_read(void);

/*===========================================================================

FUNCTION tm_xtra_ruim_read_complete

DESCRIPTION
  This function is called when Async-read for TM_XTRA NV items is complete,
  it sets TM-XTRA configuration info structure.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

extern void tm_xtra_ruim_read_complete(tm_xtra_ruim_nv_buffer* ruim_nv_buffer);

#endif /* FEATURE_UIM_SUPPORT_LBS */

#endif /* FEATURE_CGPS_XTRA */

#ifdef FEATURE_UIM_SUPPORT_LBS
/*===========================================================================

FUNCTION tm_core_ruim_read

DESCRIPTION
  This function read NV items to set TM-Core configuration info structure.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_ruim_read(void);
/*===========================================================================

FUNCTION tm_core_ruim_read_complete

DESCRIPTION
  This function is called  to set TM-Core configuration info structure, after
  Async NV read for NV items is complete

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_ruim_read_complete(tm_core_ruim_read_buffer *tm_core_ruim_buff);
#endif /* FEATURE_UIM_SUPPORT_LBS */


/*===========================================================================

FUNCTION tm_core_get_gps_lock_state

DESCRIPTION
  This function determines if gps is locked

RETURN VALUE
  TRUE if gps is locked, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_gps_lock_state(void);

/*===========================================================================

FUNCTION tm_core_get_on_demand_state

DESCRIPTION
  This function determines if on-dmeand positioning is enabled

RETURN VALUE
  TRUE if on-demand is enabled, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_on_demand_state(void);

#ifdef FEATURE_GNSS_PERIODIC_PPM
#error code not present
#endif

/*===========================================================================

FUNCTION tm_core_get_on_demand_ready_state

DESCRIPTION
  This function determines if on-dmeand ready positioning is enabled

RETURN VALUE
  TRUE if on-demand is enabled, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_on_demand_ready_state(void);

/*===========================================================================

FUNCTION tm_core_is_on_demand_session_on

DESCRIPTION
  This function determines if sess type is ON_DEMAND

RETURN VALUE
  TRUE if on-demand session is enabled, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_on_demand_session_on(void);

/*===========================================================================

FUNCTION tm_core_is_internal_session_on

DESCRIPTION
  This function determines if internal session is ongoing

RETURN VALUE
  TRUE if internal session is running, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_is_internal_session_on(void);

/*===========================================================================

FUNCTION tm_core_on_demand_pos_punc_tunc_proc

DESCRIPTION
  This function is called by MGP to return the position and its associated punc
  tunc for on-demand positioning

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_on_demand_pos_punc_tunc_proc( void *pos_data);


/*===========================================================================

FUNCTION tm_core_on_demand_charger_connected

DESCRIPTION
 this function is called when the battery charger is connected

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_on_demand_charger_connected( void );


/*===========================================================================

FUNCTION tm_core_on_demand_charger_disconnected

DESCRIPTION
 this function is called when the battery charger is disconnected

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_on_demand_charger_disconnected( void );
/*===========================================================================

FUNCTION tm_core_on_demand_charger_charged

DESCRIPTION
 this function is called when the battery charger is connected and the battery
 is fully charged

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_on_demand_charger_charged( void );

/*===========================================================================

FUNCTION tm_core_on_demand_abort

DESCRIPTION
 this function is called when tm core wants to abort an on-demand session
 fully charged

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_on_demand_abort(tm_sess_stop_reason_e_type reason);


/*===========================================================================
FUNCTION tm_core_generate_ext_pd_data

DESCRIPTION
  This function generates extended pd data

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_generate_ext_pd_data( const sm_GnssFixRptStructType *fix_ptr);

/*===========================================================================

FUNCTION tm_core_get_gps_state

DESCRIPTION
  This function is called to get the GPS Receiver state information

DEPENDENCIES

RETURN

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gps_state(
  tm_core_get_gps_state_param_s_type *p_get_gps_state_param
);

/*===========================================================================

FUNCTION tm_core_get_gps_state_para_check

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_pd_cmd_err_e_type tm_core_get_gps_state_err_check ( void );


/*===========================================================================

FUNCTION tm_core_get_best_avail_pos_err_check

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_pd_cmd_err_e_type tm_core_get_best_avail_pos_err_check ( void );


/*===========================================================================

FUNCTION tm_core_report_gps_state_info

DESCRIPTION
  This function is called by MGP to return the gps state info
  for on-demand positioning and user requested state info

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void  tm_core_report_gps_state_info( void *pos_data);

/*===========================================================================

FUNCTION tm_core_get_call_flow_select_mask

DESCRIPTION
  This function returns the UMTS CP MOLR vs AD call flow select mask in TM data

RETURN VALUE
  uint8 call flow select mask

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
uint8 tm_core_get_call_flow_select_mask(void);

/*===========================================================================

FUNCTION tm_core_xlate_pos_to_ext_status

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_xlate_pos_to_ext_status
(
  pdsm_pd_ext_pos_s_type  *dest_buf_ptr,
  sm_GnssFixRptStructType *source_buf_ptr,
  uint32                  remaing_fix_time
);

/*===========================================================================

FUNCTION tm_core_xlate_best_avail_pos_to_ext_status

DESCRIPTION convert a BestAvailPos fix report to pd_ext_status report

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_xlate_best_avail_pos_to_ext_status
(
  pdsm_pd_ext_pos_s_type    *dest_buf_ptr,
  sm_GnssFixRptStructType   *source_buf_ptr,
  uint32                    remaing_fix_time
);

/*===========================================================================

FUNCTION tm_core_get_gps_measurement_data

DESCRIPTION
  This function returns the cachec gps measurement block

RETURN VALUE
  uint8 call flow select mask

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gps_measurement_data(gnss_MeasBlkStructType *gps_meas_blk);

/*===========================================================================

FUNCTION tm_core_get_nv_utc_offset

DESCRIPTION
  This function returns the nv value of gps UTC offset

RETURN VALUE
  uint8 call flow select mask

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_nv_utc_offset(uint8 *p_utc_offset);

/*===========================================================================

FUNCTION tm_core_save_ref_position

DESCRIPTION
  This function saves the reference position received from the network so
   it can be reported at the next time tick

RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS

==========================================================================*/
void tm_core_save_ref_position( pdsm_ext_status_info_s_type   *pd_ext_status_info);
/*===========================================================================

FUNCTION tm_core_get_efs_supl_cert_status

DESCRIPTION
  This function tells the caller whether it must use SUPL Cert from EFS or not.

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_efs_supl_cert_status( void);
/*===========================================================================

FUNCTION tm_core_set_efs_supl_cert_status

DESCRIPTION
 IF tm_use_efs_for_supl_cert == TRUE, supl session must read supl cert from EFS.

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_set_efs_supl_cert_status(boolean new_efs_cert_status );

/*===========================================================================

FUNCTION tm_core_handle_on_demand_sess_start

DESCRIPTION
  This function calls the SysD controller to start an ODP session

RETURN VALUE
  uint8 call flow select mask

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_handle_on_demand_sess_start(void* ipc_msg_ptr);

/*===========================================================================

FUNCTION tm_on_demand_get_punc_tunc

DESCRIPTION
  This function returns the PUNC tunc values

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_on_demand_get_punc_tunc(FLT *p_punc, FLT *p_tunc);


/*===========================================================================

FUNCTION tm_core_get_xtra_validity_age

DESCRIPTION
  This function returns the NV item for XTRA data validity age

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_xtra_validity_age(uint16 *p_age);


/*===========================================================================

FUNCTION tm_core_check_mt_up_allowed

DESCRIPTION
  Checks whether a mobile terminated User plane
  session can be allowed or not based on the service set as Designated Data
  by the user. E.g. MT V1/V2 will only be allowed if designated data is 1x.
  Similarly MT SUPL will only be allowed if designated data is GSM/W.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_check_mt_up_allowed
(
  tm_prtl_type  prtl_type
);

/*===========================================================================

FUNCTION tm_core_must_use_supl_cert_frm_efs

DESCRIPTION
  This function tells the caller whether it must use SUPL Cert from EFS or not.

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_must_use_supl_cert_from_efs( void);
/*===========================================================================

FUNCTION tm_core_get_current_dd_subs_id

DESCRIPTION
  This function returns the ID for current DD subscription

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
sys_modem_as_id_e_type tm_core_get_current_dd_subs_id( void);
/*===========================================================================

FUNCTION tm_core_set_current_dd_subs_id

DESCRIPTION
  This function updates the stored value for current DD subscription

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_set_current_dd_subs_id( sys_modem_as_id_e_type asid);
/*===========================================================================

FUNCTION tm_core_set_efs_supl_cert_status

DESCRIPTION
 IF tm_use_efs_for_supl_cert == TRUE, supl session must read supl cert from EFS.

RETURN VALUE


DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_set_efs_supl_cert_status(boolean new_efs_cert_status );

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*====================================================================
FUNCTION tm_mmgsdi_set_session_id

DESCRIPTION

  This function updates the tm mmgsdi array with session id info for
  the given session type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE


SIDE EFFECTS
  none

=====================================================================*/
void tm_mmgsdi_set_session_id
(
   mmgsdi_session_type_enum_type tm_mmgsdi_session_type,
   mmgsdi_session_id_type session_id
);

/*====================================================================
FUNCTION tm_mmgsdi_get_session_id_status

DESCRIPTION

  This function informs whether the session id for a given
  mmgsdi session type has been set or not.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : Session ID was programmed earlier
  FALSE : Session ID not programmed.

SIDE EFFECTS
  none

=====================================================================*/
boolean tm_mmgsdi_get_session_id_status
(
   mmgsdi_session_type_enum_type tm_mmgsdi_session_type
);

/*====================================================================
FUNCTION tm_mmgsdi_close_session_id

DESCRIPTION

  This function close a particular session id and reset its status

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : Session ID was closed successfully
  FALSE : Session ID not found

SIDE EFFECTS
  none

=====================================================================*/
boolean tm_mmgsdi_close_session_id
(
   mmgsdi_session_id_type session_id
);

/*====================================================================
FUNCTION tm_mmgsdi_get_session_id_ptr

DESCRIPTION

  This function returns a pointer to the session ID for the specified
  session type.  At most one session ID is associated with each session
  type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : If session id was found
  FALSE : Session id for this session type not found.

SIDE EFFECTS
  none

=====================================================================*/
boolean tm_mmgsdi_get_session_id_ptr
(
   mmgsdi_session_type_enum_type tm_mmgsdi_session_type,
   mmgsdi_session_id_type     ** tm_mmgsdi_session_id_ptr
);
/*====================================================================
FUNCTION tm_mmgsdi_set_subscription_id

DESCRIPTION
  This function updates the tm mmgsdi session info table with the
  subscription id assigned to a given session type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE


SIDE EFFECTS
  none

=====================================================================*/
void tm_mmgsdi_set_subscription_id
(
   mmgsdi_session_type_enum_type tm_mmgsdi_session_type,
   sys_modem_as_id_e_type        asubs_id
);

/*====================================================================
FUNCTION tm_mmgsdi_get_subscription_id

DESCRIPTION
  This function gets the subscription id assigned to a given session ID
  from tm mmgsdi session info table.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : subscription id had been found and returned
  FALSE : failed to find subscription id for a particular session id.

SIDE EFFECTS
  none

=====================================================================*/
boolean tm_mmgsdi_get_subscription_id
(
   mmgsdi_session_id_type     session_id,
   sys_modem_as_id_e_type     *asubs_id
);

/*====================================================================
FUNCTION tm_mmgsdi_set_app_type

DESCRIPTION
  This function updates the tm mmgsdi session info table with the
  app type for a given session id.


DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE


SIDE EFFECTS
  none

=====================================================================*/
void tm_mmgsdi_set_app_type
(
   mmgsdi_session_id_type session_id,
   mmgsdi_app_enum_type         app_type
);
/*====================================================================
FUNCTION tm_mmgsdi_get_sim_info_from_asid

DESCRIPTION

  This function returns a pointer to the session ID and a pointer to
  the application type. for the specified
  subscription ID.  At most one session ID is associated with each session
  type.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  TRUE : session id had been set
  FALSE : Session id had not been set.

SIDE EFFECTS
  none

=====================================================================*/
boolean tm_mmgsdi_get_sim_info_from_asid
(
   sys_modem_as_id_e_type      asubs_id,
   mmgsdi_session_id_type     *tm_mmgsdi_session_id_ptr,
  mmgsdi_app_enum_type *app_type_ptr,
  mmgsdi_app_enum_type app_type
);

#endif /*FEATURE_MMGSDI_SESSION_LIB*/



/*===========================================================================

FUNCTION tm_core_get_xtra_validity_age

DESCRIPTION
  This function returns the NV item for XTRA data validity age

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_xtra_validity_age(uint16 *p_age);

/*====================================================================
FUNCTION tm_core_time_validity

DESCRIPTION
   Reports whether the time from time_get_ms is valid or not

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_core_time_validity(void);

/*===========================================================================

FUNCTION tm_agps_emergency_call_simulate

DESCRIPTION
   Reports whether emergency call simulation is enabled or not

DEPENDENCIES

RETURN VALUE
  boolean: TRUE - emergency call simulation enabled
           FALSE - emergency call simulation disabled

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_agps_emergency_call_simulate(void);

/*===========================================================================

FUNCTION tm_core_get_trusted_utctime

DESCRIPTION
  This function determines if the utcoffset is

 (i.e. decoded over the air)

RETURN VALUE
  TRUE if utcoffset is trusted, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_get_trusted_utctime(void);

/*===========================================================================
FUNCTION tm_core_clear_cached_pos

DESCRIPTION Clears the RAM copy of the position

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_clear_cached_pos(void);

/*===========================================================================
FUNCTION tm_core_is_standalone_active

DESCRIPTION
Check if Standalone session is in progress.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_standalone_active(void);

/*===========================================================================

FUNCTION tm_core_translate_end_status

DESCRIPTION
  This function translate internal abort reason to external end status.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_pd_end_e_type tm_core_translate_end_status
(
  tm_sess_stop_reason_e_type  abort_reason
);

/*===========================================================================

FUNCTION tm_core_gm_evt_handler

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_gm_evt_handler(uint32 msg_id, void *p_Data);

/*===========================================================================
FUNCTION tm_core_get_lte_lpp_up_enable

DESCRIPTION
  This function determines if LPP protocol is enabled in NV



RETURN VALUE
  TRUE if LPP is enabled, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_lte_lpp_up_enable(void);

/*===========================================================================
FUNCTION tm_core_get_asst_glo_lpp_up_enable

DESCRIPTION
  This function determines the positioning protocol for Assisted GLONASS U-plane
  is LPP.

RETURN VALUE
  TRUE if LPP is selected, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_asst_glo_lpp_up_enable(void);

/*===========================================================================
FUNCTION tm_core_get_asst_glo_lpp_cp_enable

DESCRIPTION
  This function determines if Assisted GLONASS is enabled for U-plane LPP 
  protocol.

RETURN VALUE
  TRUE if LPP is selected, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/

boolean tm_core_get_asst_glo_lpp_cp_enable(void);

/*===========================================================================

FUNCTION tm_core_otdoa_evt_handler

DESCRIPTION
  This function receives OTDOA measurements from the OTDOA interface and sends
  them to the appropriate protocol


RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_otdoa_evt_handler(sm_loc_lte_otdoa_meas_info_type *p_otdoa_measurement_info);


#ifdef FEATURE_WCDMA_GPS_CELL_ID_READING
/*===========================================================================

FUNCTION tm_core_rrc_cell_info_dch_report_handler

DESCRIPTION
  This function handles the rrc cell info DCH report.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_rrc_cell_info_dch_report_handler(tm_rrc_cell_info_dch_report_type* rrc_cell_info_p);
#endif /*FEATURE_WCDMA_GPS_CELL_ID_READING */

/*===========================================================================

FUNCTION tm_core_reset_location_service_done_handler

DESCRIPTION
  This function handles the reset locaiton service done indication

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_reset_location_service_done_handler(pdsm_pd_ext_reset_location_service_status_e_type* status);


/*===========================================================================
FUNCTION tm_core_is_agps_allowed_on_serv_sys

DESCRIPTION
Check if AGPS is allowed on a serving system

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_agps_allowed_on_serv_sys(cgps_SrvSystemType serv_sys);

/*===========================================================================
FUNCTION tm_core_is_umts_prefered

DESCRIPTION
Check if UMTS/LTE is prefered over other RAT's

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
XTRA command status
===========================================================================*/
boolean tm_core_is_umts_prefered(void);


/*===========================================================================
FUNCTION tm_core_is_ni_standalone

DESCRIPTION
  This function checks if this is an 1x UP NI triggered session which has fallen
  back to standalone

DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_is_ni_standalone(void);

/*===========================================================================
FUNCTION tm_core_reset_location_service_in_progress

DESCRIPTION
  This function checks if reset location service is in progress

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE: Whether reset location service is in progress.

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_is_reset_location_service_in_progress(void);

/*===========================================================================
FUNCTION tm_core_reset_location_service_in_progress

DESCRIPTION
  This function sets whether reset location service is in progress

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_set_reset_location_service_in_progress(boolean reset_location_service_in_progress);

/*===========================================================================
FUNCTION tm_core_handle_engine_error_recovery_report

DESCRIPTION
  This function handles engine error recovery report (usually it'll be sent to
  QMI)

DEPENDENCIES

RETURN VALUE
  TRUE if success

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_engine_error_recovery_report(gnss_engine_err_recovery_rpt *pErrReport);

/*===========================================================================
FUNCTION tm_core_handle_engine_dpo_status_report

DESCRIPTION
  This function handles engine DPO status report (usually it'll be sent to
  QMI)

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
extern void tm_core_handle_engine_dpo_status_report(sm_DpoStatusReportStructType *p_DpoStatusReport);

/*===========================================================================
FUNCTION tm_core_handle_best_avail_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_best_avail_pos
(
  tm_core_get_best_avail_pos_param_s_type *p_get_best_avail_pos_param
);

/*===========================================================================
FUNCTION tm_core_handle_avail_wwan_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_avail_wwan_pos
(
  tm_core_get_avail_wwan_pos_param_s_type *p_GetAvailWwanPosParam
);

/*===========================================================================
FUNCTION tm_core_handle_available_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_available_pos
(
  tm_core_get_available_pos_param_s_type *p_GetAvailablePosParam
);

/*===========================================================================
FUNCTION tm_core_get_lpp_cp_enable

DESCRIPTION
  This function determines if LPP Control Plane protocol is enabled in NV



RETURN VALUE
  TRUE if LPP is enabled, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_get_lpp_cp_enable(void);

/*===========================================================================
FUNCTION tm_core_enable_DpoStatusReport

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE is DPO Status Report was enabled

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_enable_DpoStatusReport(boolean enable);

/*===========================================================================

FUNCTION tm_core_api_callback_handler

DESCRIPTION
  This function handles calling the client callbacks.
  It also starts a non-deferred timer that will fire even when the processor is sleeping.
  The timer runs for 10 sec and firing off every 1 sec.

DEPENDENCIES

RETURN VALUE TRUE/FALSE

SIDE EFFECTS

===========================================================================*/

boolean tm_core_api_callback_handler(const tm_api_notifier_type *p_api_notifier);

/*===========================================================================
FUNCTION tm_core_is_supl_ni_proc

DESCRIPTION
  This function checks if this is an SUPL NI trigger should be reported to AP always
  
DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_is_supl_ni_proc(void);

/*===========================================================================
FUNCTION tm_core_mtlr_auto_accept

DESCRIPTION
  This function checks if this is an MTLR should be accepted without AP notification
  during emergency

DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_mtlr_auto_accept( void );

/*===========================================================================
FUNCTION tm_core_lpp_cp_evt_handler

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming LPP Control Plane Protocol events.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_core_lpp_cp_evt_handler( uint32 msg_id, void *lpp_cp_evt_info_ptr );

/*===========================================================================

FUNCTION tm_core_external_cell_db_proc

DESCRIPTION
  This function is called by TmCore's IPC processor to inform TmCore of
  an incoming external cell database update message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

boolean tm_core_external_cell_db_proc( pdsm_cell_info_s_type *external_cell_db_info_ptr );

/*===========================================================================

FUNCTION tm_core_ecid_evt_handler

DESCRIPTION
  This function receives ECID measurements from the ECID interface and sends
  them to the appropriate protocol


RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_ecid_evt_handler(gnss_lpp_ecid_meas_info_type *p_ecid_measurement_info);

/*===========================================================================
FUNCTION tm_core_set_ni_resp_pending

DESCRIPTION
  This function checks if this is an SUPL NI trigger should be reported to AP always

DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
extern void tm_core_set_ni_resp_pending( boolean b_pending );

/*===========================================================================
FUNCTION tm_core_is_ni_resp_pending

DESCRIPTION
  This function checks if this is an SUPL NI trigger should be reported to AP always

DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_is_ni_resp_pending( void );


/*===========================================================================
FUNCTION tm_core_retrieve_imsi

DESCRIPTION
  This function is called by TLE to request for IMSI retrieval

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_retrieve_imsi( tm_RetrieveIMSIType * tm_retrieve_imsi_info_ptr );

/*===========================================================================

FUNCTION tm_copy_prm_to_sm_meas_struct

DESCRIPTION
  This function copies the ME provided PRM data to SM GNSS Measurement report struct.

DEPENDENCIES

RETURN VALUE
  Return GPS or GLONASS measurement index upon success, else return invalid if
  PRM data isn't of GPS or GLONASS type.
SIDE EFFECTS

===========================================================================*/
int32 tm_copy_prm_to_sm_meas_struct(const sm_GpsMeasRptStructType *const p_prm_data,
                                                       SmGnssMeasReportStructType *p_gnss_meas_report);

/*===========================================================================

FUNCTION tm_core_get_gm_engine_config

DESCRIPTION
  This function returns the GM Engine configuration

RETURN VALUE
  The timeout value in secs.

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_get_gm_engine_config( geofence_engine_config_s_type **gm_config );

/*===========================================================================

FUNCTION tm_core_is_wiper_running

DESCRIPTION
 Function to get whether Wiper is running

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_core_is_wiper_running( void );

/*===========================================================================

FUNCTION tm_util_map_and_inject_glo_steering_to_tmcore

DESCRIPTION
  Inject Glonass Acquisition Assistance data - Steering - into GNSS engine,
  when deltaT/leap second (from GPS UTCModel or Glonass TimeModel AD), Raw AcqAssist
  (a.k.a Reference Measurement Info) & Glo AuxiInfo is available.

DEPENDENCIES
  Call tm_util_map_glo_acqast_svid_to_freq_num() to associate SvId to FreqNum
  before invoking this function.

RETURN VALUE
  Return when AA Steering isn't injected into GNSS engine.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_util_map_and_inject_glo_steering_to_tmcore( prot_glo_acqast_buffer_s_type *const p_glo_acqasst,
                                                       prot_glo_acqasst_sat_buffer_s_type const *const p_glo_acqasst_subset,
                                                       prot_glo_svcn_buffer_s_type const *const p_glo_auxi_info,
                                                       gnss_time_info const *const p_gnss_time,
                                                       tm_prtl_type prtl_type,
                                                       tm_sess_handle_type sess_handle,
                                                       tm_post_data_payload_type *data_payload_ptr );

/*===========================================================================

FUNCTION tm_util_map_glo_acqast_svid_to_freq_num

DESCRIPTION
 Map Glonass Acquisition Assistance SvId to frequency number using the information
 in stored SV/ChannelNumber buffer structure.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_util_map_glo_acqast_svid_to_freq_num( prot_glo_acqast_buffer_s_type *const p_GloAcqAsstBuf,
                                              prot_glo_svcn_buffer_s_type const *const p_GloSvCnMapBuf );

/*===========================================================================

FUNCTION tm_util_translate_glo_intcodephase

DESCRIPTION
 Compute the Glonass milli-second of the week from interger code phase &
 Glonass Time Of Day in milli-seconds.

DEPENDENCIES

RETURN VALUE
  GNSS milli-second of the week

SIDE EFFECTS
  None

===========================================================================*/
uint32 tm_util_translate_glo_intcodephase( uint32 const q_integer_code_phase, uint32 const q_GloMsecInDay );

/*===========================================================================

FUNCTION tm_util_cp_msa_gnss_mandatory_ad_needed

DESCRIPTION
 Return the mandatory GNSS assistance data needed from network/LS. The returned
 list is used by protocol module to request missing AD from network/LS.
 Use this function only in A-GNSS call flow.

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
void tm_util_cp_msa_gnss_mandatory_ad_needed( uint32 *p_GpsAdNeeded, uint32 *p_GloAdNeeded, uint32 const q_GpsAdDelivered, uint32 const q_GloAdDelivered );

/*===========================================================================

FUNCTION tm_util_cp_msa_gps_mandatory_ad_needed

DESCRIPTION
 Return the mandatory GPS assistance data needed from network/LS. The returned
 list is used by protocol module to request missing GPS AD from network/LS.
 Use this function only in A-GPS only call flow.

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
void tm_util_cp_msa_gps_mandatory_ad_needed( uint32 *p_GpsAdNeeded, uint32 const q_GpsAdDelivered );

/*===========================================================================
FUNCTION tm_core_get_lpp_periodic_end_flag

DESCRIPTION
  This function checks if LPP periodic session should end after emergency call endss
  
DEPENDENCIES

RETURN VALUE
  Active Sess Handle

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_get_lpp_periodic_end_flag (void);

/********************************************************************
*
* tm_get_imei
*
* Function description:
*  Gets the IMEI value from the GHSDI
*
* Parameters: 
*  p_imei_ptr - The retrieved IMEI to return to the caller (output)
*
* Return:
*  TRUE/FALSE
*  
*********************************************************************
*/  
boolean tm_get_imei(byte *p_imei_ptr);

/********************************************************************
*
* tm_handle_lte_ota_gps_reset
*
* Function description:
*  Handles the Test OTA reset message for GPS
*
* Parameters: 
*   
*
* Return:
*  none
*  
*********************************************************************
*/   
void tm_handle_lte_ota_gps_reset(void);

/********************************************************************
* tm_core_handle_wifi_attachment_status_ind
*
* Function description:
* Handles Wifi attachment status indication info
*
* Parameters: 
*  p_wifi_attachment_status_ind_info - Pointer to Wifi attachment status inidcation info stuct
*
* Return:
* void
*  
*********************************************************************
*/  
void tm_core_handle_wifi_attachment_status_ind(pdsm_wifi_attachment_status_ind_info *p_wifi_attachment_status_ind_info);

/*===========================================================================
FUNCTION tm_core_get_end_supl_after_final_fix_nv

DESCRIPTION
  This function returns cached NV value if SUPL can be terminated after final fix is sent
  
DEPENDENCIES

RETURN VALUE
  TRUE if NV is enabled, FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_get_end_supl_after_final_fix_nv (void);

/*===========================================================================
FUNCTION tm_core_lpp_grt_24_admeas_enabled

DESCRIPTION
  Return if the LPP greater than 24 AD & RSTD measurement processing is
  supported by UE.

RETURN VALUE
  TRUE if LPP > 24 AD feature is supported LPP, FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_lpp_grt_24_admeas_enabled(void);

/********************************************************************
*
* tm_core_handle_wifi_enabled_status_ind
*
* Function description:
* Handles Wifi enabled status indication info
*
* Parameters: 
*  e_wifi_enabled_status - Status of wifi enabled.
*
* Return:
* void
*  
*********************************************************************
*/  
void tm_core_handle_wifi_enabled_status_ind(pdsm_wifi_enabled_status_ind_type e_wifi_enabled_status);

/*===========================================================================
FUNCTION tm_core_handle_wifi_fix_response

DESCRIPTION
  This function handles Wifi fix Response
  
DEPENDENCIES

RETURN VALUE
  VOID

SIDE EFFECTS

===========================================================================*/
extern void tm_core_handle_wifi_fix_response(tm_wifi_fix_response_type *pz_wifi_fix_resp);

/*===========================================================================


DESCRIPTION
  This function sends session done idnication to PE for PE


RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_send_sess_done_ind_for_lppm(void);

/*===========================================================================

FUNCTION tm_core_refresh_carrier_specific_nv

DESCRIPTION
  This function refresh carrier-specific NVs for a specific subscription


RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_core_refresh_carrier_specific_nv(void* p_data);

/*===========================================================================
FUNCTION tm_core_lpp_earlytimer_enabled

DESCRIPTION
  Return if the LPP Rel12 2Timer feature is enabled. If enabled the Early
  Response timer in Qos is processed, else it is dropped. By default the
  feature is disabled.

RETURN VALUE
  TRUE  :If LPP Rel12 2Timer feature is enabled.
  FALSE :If disabled

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
boolean tm_core_lpp_earlytimer_enabled(void);

#ifdef __cplusplus
}
#endif

#endif /* TM_DATA_H */

