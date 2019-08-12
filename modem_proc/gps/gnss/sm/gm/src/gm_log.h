/*===========================================================================

                  GM_LOG Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM Log.

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/13/15   ss     Geofencing Stats support
12/10/14   ssu    DM log for GM Add, Delete, Purge and breach operations.
10/04/13   ss   Initial version
===========================================================================*/
#ifndef GM_LOG_H
#define GM_LOG_H

#include "gm_core.h"

#if defined(T_WINNT)
#error code not present
#endif

/****************** Data Structures*********************************************/
typedef enum
{
  GEOFENCE_MOTION_DETECTOR_NONE = 0,
  GEOFENCE_MOTION_DETECTOR_AMD_RMD_PED = 0x1,
  GEOFENCE_MOTION_DETECTOR_CMC = 0x2,
  GEOFENCE_MOTION_DETECTOR_WIFI = 0x4,
  GEOFENCE_MOTION_DETECTOR_WWAN = 0x8
} geofence_motion_detector_type;

typedef PACKED struct PACKED_POST
{
  uint8 detectors_used; /*Motion detector used*/
  uint32 distance_to_check; /*Distance for motion check*/
} geofence_motion_detection_start_rpt;

typedef PACKED struct PACKED_POST
{
  uint8 motion_detector; /*Motion detector*/
} geofence_motion_detection_stop_rpt;

typedef PACKED struct PACKED_POST
{
  uint8 motion_detector; /*Motion detector*/
} geofence_motion_detection_distance_met_rpt;

typedef PACKED struct PACKED_POST
{
  uint8 motion_detector; /*Motion detector*/
} geofence_motion_detection_error_rpt;

typedef PACKED struct PACKED_POST
{
  uint8 motion_detector; /*Motion detector*/
  uint32 distance_accumulated;   /*Distance accumulated thus far*/
} geofence_motion_detection_distance_accum_rpt;

typedef enum
{
  GEOFENCE_MOTION_DETECTION_START_RPT = 0x1,
  GEOFENCE_MOTION_DETECTION_STOP_RPT,
  GEOFENCE_MOTION_DETECTION_ERROR_RPT,
  GEOFENCE_MOTION_DETECTION_DISTANCE_MET_RPT,
  GEOFENCE_MOTION_DETECTION_DIST_ACCUM_RPT
} geofence_motion_report_type;

typedef PACKED struct PACKED_POST
{
  uint8 report_type;
  union
  {
   geofence_motion_detection_start_rpt start_rpt;   /* Motion Start Report */
   geofence_motion_detection_stop_rpt stop_rpt;   /* Motion Stop Report */
   geofence_motion_detection_error_rpt error_rpt; /* Motion Error Report */
   geofence_motion_detection_distance_met_rpt distance_met_rpt; /* Distance Met Report */
   geofence_motion_detection_distance_accum_rpt dist_accum_rpt; /* Distance Accumulated Report */
  } motion_rpt_type;
} geofence_motion_detection_rpt;

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;
  geofence_motion_detection_rpt motion_detection_rpt;
}geofence_motion_detection_log_rpt;


/* Wifi is used to generate the fix */
#define GM_SRC_USED_FOR_FIX_GEN_WIFI_MASK 0x0001
/* Sensors are used to generate the fix */
#define GM_SRC_USED_FOR_FIX_GEN_SENSORS_MASK 0x0002
/* Satellite is used to generate the fix */
#define GM_SRC_USED_FOR_FIX_GEN_SATELLITE_MASK 0x0004
/* Cell Id is used to generate the fix */
#define GM_SRC_USED_FOR_FIX_GEN_CELLID_MASK 0x0008
/* Coarse position is used to generate the fix */
#define GM_SRC_USED_FOR_FIX_GEN_COARSE_POS_MASK 0x0010


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;         /* Version number of DM log */
  uint32      q_Fcount;          /* Local millisecond counter */
  uint32      q_count;

  uint8             pos_fix_src;        /*Position Fix src*/
  double            latitude;          /*Latitude in Degrees*/
  double            longitude;         /*Longitude in Degrees*/
  double            altitude;          /*Altitude*/
  double            pos_unc_2d;        /*2D position uncertainity*/
  uint8             pos_unc_2d_conf;   /*2D position uncertainity confidence*/
  float             heading_radians;   /*Heading in Radians*/
  uint16            w_GpsWeek;         /* GPS week number of fix */
  uint32            q_GpsTimeMs;       /* GPS time in week (milliseconds) of fix */
  uint64            t_source_used_for_fix_gen_mask; /* The sources used for fix generation */
}geofence_position_log_rpt;


#define GM_WWAN_MOT_CLASS_LOG_MAX_CELLS  50

typedef PACKED struct PACKED_POST
{
  uint8  is_active_set_info_incl;
  uint8  is_active_set;
  uint32 cell_freq;
  uint32 cell_psc;
  uint32 cell_rsp;
  uint8  is_cell_ec_no_incl;
  uint32 cell_ec_no;
  uint8  is_delta_rscp_incl;
  uint32 delta_rscp;
  uint8 is_cell_rsrq_incl;
  uint32 cell_rsrq;
} gm_meas_cell_info_log_s;

typedef PACKED struct PACKED_POST
{
  uint32   measurement_rat;
  uint32   num_cells;
  uint32   motion_class_output;
  uint8    is_cell_overlap_pct_incl;
  uint32   cell_overlap_pct;
  gm_meas_cell_info_log_s meas_cell_info[GM_WWAN_MOT_CLASS_LOG_MAX_CELLS];
  
}geofence_wwan_mot_class_rpt;

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
//  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;

  geofence_wwan_mot_class_rpt wwan_mot_class_rpt;
} geofence_wwan_mot_class_log_rpt;

typedef PACKED struct PACKED_POST
{
  uint8           mac_addr[WIPER_MAC_ADDR_LENGTH];
  uint8           ssid[WIPER_SSID_STR_LENGTH];
} geofence_wifi_ap_set_rpt;

typedef PACKED struct PACKED_POST
{
  /**************************************************************************** 
   * State associated with the WiFi distance accumulation algorithm           *
   ****************************************************************************/
  uint8 b_accumulating_distance;        /* Is WiFi accumulating distance?     */
  uint8 cdh_accumulating_distance;      /* Is CDH accumulating distance?      */
  uint8 scan_accumulating_distance;     /* Is SCAN accumulating distance      */
  /**************************************************************************** 
   * State associated with the CDH algorithm                                  *
   ****************************************************************************/ 
  uint8 b_cdh_accumulating_distance;    /* CDH algorithm accumulating distane */
  uint8 cdh_sensor_state;               /* state of CDH algorithm         
                                           0 = STATE_DISCONNECTED_VMAX,
                                           1 = STATE_CONNECTED,
                                           2 = STATE_DISCONNECTED_STATIONARY,
                                           3 = STATE_DISCONNECTED_VRUN */
  uint8 cdh_wifi_attach_state;          /* Do we think WiFi is dis/connected
                                           0 = Connected
                                           1 = Disconnected  */
  float  f_cdh_dist_orig;               /* distance CDH was asked to detect */
  float  f_cdh_dist_left;               /* distance CDH is left to detect */
  /**************************************************************************** 
   * State associated with the AP that CDH is connected to. Valid only if     *
   * cdh_wifi_attach_state indicates WiFi is connected                        *
   ****************************************************************************/ 
  uint8 ap_mac[WIPER_MAC_ADDR_LENGTH];  /* If connected, the MAC   */
  uint8 ap_ssid[WIPER_SSID_STR_LENGTH]; /* If connected, the SSID  */
  uint8 reliable_ho_available;          /* 1 = Reliable HO events available
                                           0 = Reliable HO events unavailable */
  uint16 q_max_wifi_ap_range_dia;       /* CDH algos range of the AP is */
  uint8  is_ap_in_network;              /* 2 = In Network
                                           1 = Not in Network
                                           0 = Unknown */
  uint8  u_cell_set_changes;            /* The number of WWAN cell set changes 
                                           seen while connected to this AP */
  uint8  u_ap_set_changes;              /* The number of AP set changes seen
                                           while connected to this AP      */
  uint8  ap_pos_fix_valid;              /* is there a GPS fix associaed with 
                                           connected AP */
  uint16 fix_src;                       /* Fix source
                                             0x1 = GEOFENCE_POS_FIX_SRC_CPI
                                             0x2 = GEOFENCE_POS_FIX_SRC_GNSS
                                             0x4 = GEOFENCE_POS_FIX_SRC_BEST_AVAIL_POS
                                             0x8 = GEOFENCE_POS_FIX_SRC_NO_FIX */
  uint8  fix_type;                      /* Fix type
                                             0x0 = GEOFENCE_POS_FIX_TYPE_INTERMEDIATE
                                             0x1 = GEOFENCE_POS_FIX_TYPE_FINAL */
  
  uint32 cached_pos_fix_timestamp;      /* Cached pos fix Timestamp */
  uint32 fix_hepe;                      /* Position Fix Hepe */
  
  sint31 lat;
  sint31 lon;
  uint16 vel_hor;

  /**************************************************************************** 
   * State associated with the SCAN algorithm                                 *
   ****************************************************************************/ 
  uint8  wifi_scan_state;    /* 0 = GM_MOTION_SENSOR_STATE_NOT_RUNNING
                                1 = GM_MOTION_SENSOR_STATE_WAIT_TO_RUN
                                2 = GM_MOTION_SENSOR_STATE_RUNNING_ACTIVE
                                4 = GM_MOTION_SENSOR_STATE_RUNNING_NOT_ACTIVE */

  /* State related to the distance we are tracking */
  float   gm_motion_sensor_wifi_scan_cumulative_distance;
  float   gm_motion_sensor_wifi_scan_distance_to_check; 
  uint32  gm_motion_sensor_wifi_scan_dist_accum_start_time_stamp;
  
  boolean gm_motion_sensor_wifi_scan_last_ap_set_valid;
  uint32  gm_motion_sensor_wifi_scan_last_valid_injection_time_stamp;
  uint32  gm_motion_sensor_wifi_scan_last_ap_set_time_stamp;
} geofence_wifi_mot_detection_state;

typedef PACKED struct PACKED_POST
{
  /* State related to the last AP set that we recieved in a Scan */
  uint8   num_aps;
  geofence_wifi_ap_set_rpt ap_set[50];
} geofence_wifi_mot_detection_scan_ap_set;

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8        u_Version;               /* Version number of DM log */
	uint32      q_count;
  
  geofence_wifi_mot_detection_state state;
  /* State related to the last AP set that we recieved in a Scan */
  uint8   num_aps;
  geofence_wifi_ap_set_rpt ap_set[1];
} geofence_wifi_mot_detection_state_log_rpt;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;

  /* Client handle */
  sm_gm_client_handle   z_client_handle;

  /*  Transaction ID */
  uint32 q_tid;
    
  /* Breach Event Mask */
  uint8 q_breach_mask;

  /* Circular geofence dimensions */
  sm_gm_circular_target_area z_circular_target;

  /* Flag to include the position */
  uint8 b_include_position;

  /* The responsiveness value of the geofence */
  uint8 geofence_responsiveness;

  /* The confidence value of the geofence */
  uint8 geofence_confidence;

  /*start time of geofence in secs*/
  uint32 q_start_time; 

  /*stop time relative to start time of geofence in secs*/
  uint32 q_stop_time; 

  /*If persistency needed for the geofence*/
  boolean b_persistency_needed; 

  /*Minimum time interval between breaches*/
  uint32 q_min_interval_time; 

  /*Maximum number of breach reports*/  
  uint32 q_max_num_reports; 

  /*When dynamic responsiveness is enabled, higher user experience is provided
    while adhering to responsiveness setting where ever possible*/
  boolean b_enable_dyn_resp; 

  /* The geofence id allocated by the engine */
  uint32 q_geofenceId;

}geofence_add_info_log_rpt;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;

  /* Client handle */
  sm_gm_client_handle   z_client_handle;

  /* The geofence to be deleted */
  uint32 q_geofenceId;

  /*  Transaction ID */
  uint32 q_tid;
}geofence_del_info_log_rpt;

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;

  /* Client handle */
  sm_gm_client_handle  z_client_handle;

  /*  Transaction ID */
  uint32 q_tid;
}geofence_purge_info_log_rpt;

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;

  /*Unique Transaction Id provided by the client*/
  uint32 q_transaction_id; 

  /*Unique Geofence id indentifying the geofence*/
  uint32  geofence_id;

  /*Geofence Origin Type MO/NI/etc*/
  uint8 geofence_type;

  /*Breach event: Entering/Leaving/Inside/Outside/etc*/
  uint8 breach_type;

  /*Breach confidence assosicated with the fix*/
  uint8 breach_confidence;

  /*Whether fix is included with the report*/
  boolean fix_included;
}geofence_breach_info_log_rpt;

#define GEOFENCE_LOG_RESP_MAX_BUCKETS 9
#define GEOFENCE_LOG_RADIUS_MAX_BUCKETS 6

typedef struct
{
  uint32 q_min;
  uint32 q_max;
} geofence_log_bucket_info_type;

extern geofence_log_bucket_info_type z_resp_bucket_info[GEOFENCE_LOG_RESP_MAX_BUCKETS];
extern geofence_log_bucket_info_type z_radius_bucket_info[GEOFENCE_LOG_RADIUS_MAX_BUCKETS];

typedef PACKED struct PACKED_POST
{
  /*Handset Uptime seconds*/
  uint32 q_uptime_secs; 

  /*Goefence Active time seconds: Seconds for which there was atleast one Geofence being 
    handled by the Geofencing engine*/
  uint32 q_gf_active_secs; 

  /*Number of Geofences being handled at present*/
  uint32 q_cur_num_gfs;

  /*Total Number of Geofences handled: From Bootup*/
  uint32 q_num_gfs_handled;

  /*Total Geofence with various: Responsiveness Responsiveness Distribution: From Bootup*/  
  uint32 q_num_gfs_resp_bucket[GEOFENCE_LOG_RESP_MAX_BUCKETS];

  /*Total Geofence with various: Radius Radius Distribution: From Bootup*/  
  uint32 q_num_gfs_radius_bucket[GEOFENCE_LOG_RADIUS_MAX_BUCKETS];

  /*Total number of Mobile Originated Geofences handled: From Bootup*/
  uint32 q_num_mo_gfs;

  /*Total number of Network Originated Geofences handled: From Bootup*/
  uint32 q_num_ni_gfs;
    
  /*Total number of Always On Geofences handled: From Bootup*/
  uint32 q_num_aon_gfs;

  /*Total number of INSIDE Geofence breach rerports: From Bootup*/
  uint32 q_num_in_breaches_rpts;

  /*Total number of Outside Geofence breach rerports: From Bootup*/
  uint32 q_num_out_breaches_rpts;

  /*Total number of GNSS fix requested: From Bootup*/
  uint32 q_num_gnss_fixes;

  /*Total number of Best Avail Pos fix requested: From Bootup*/
  uint32 q_num_zpp_fixes;

  /*Total number of CPI fixes requested: From Bootup*/
  uint32 q_num_cpi_fixes;

  /*Total number of LPW fixes requested: Form Bootup*/
  uint32 q_num_lpw_fixes;

  /*Total time in seconds when Geofencign engine found GNSS unavailable: From Bootup*/
  uint32  q_gnss_unavail_secs; 

  /*Minimum Distance to GF boundary from last rpt*/
  uint32 q_min_dist_from_gf_from_last_rpt;  
} geofence_gm_core_stats_type;

typedef PACKED struct PACKED_POST
{
  /*Time in seconds Motion Sensing was used when Geofencing was active*/
  uint32 q_motion_sening_usage_secs;

  /*Time in seconds Context was used when Geofencing was active*/
  uint32 q_context_usage_secs;  
  
  /*Time in seconds Context was used when Geofencing was active*/
  uint32 q_iod_usage_secs;  
}geofence_gm_ebee_stats_type;

typedef PACKED struct PACKED_POST
{
  /*Time in seconds Sensor based Motion detection was used when Geofencing was active*/
  uint32 q_sensor_motion_detect_usage_secs;
  /*Time in seconds Wifi based Motion detection was used when Geofencing was active*/  
  uint32 q_wifi_motion_detect_usage_secs;
  /*Time in seconds WWAN based Motion detection was used when Geofencing was active*/  
  uint32 q_wwan_motion_detect_usage_secs;  
  /*Total number of CMC Cross Check fail*/
  uint32 q_num_cmc_crosscheck_fail;
  /*Total number of RMD Cross Check fail*/
  uint32 q_num_rmd_crosscheck_fail;
}geofence_gm_motion_sensing_stats_type;

typedef PACKED struct PACKED_POST
{
  /*Number of Geofences with Context learnt*/
  uint32 q_num_gf_context_learnt;

  /*Number of Wifi context learnt*/
  uint32 q_num_wifi_context_learnt;
}geofence_gm_proximity_stats_type;

typedef PACKED struct PACKED_POST
{  
  /*GM Core Stats*/
  geofence_gm_core_stats_type z_gm_core_stats;

  /*GM Proximity Stats*/
  geofence_gm_proximity_stats_type z_gm_proximity_stats;

  /*GM EbEE Stats*/  
  geofence_gm_ebee_stats_type z_gm_ebee_stats;

  /*GM Motion Sensing Stats*/
  geofence_gm_motion_sensing_stats_type z_gm_motion_sensing_stats;

}geofence_stats_log_info;


typedef PACKED struct PACKED_POST
{  
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;

  geofence_stats_log_info z_stats_log_info;
}geofence_stats_log_rpt;

typedef struct
{
  geofence_stats_log_rpt z_gf_stats_log_rpt;
} geofence_stats_rpts_type;

/*Function to register for providing GF statsr*/
typedef void (gm_gf_stats_prv_cb)
(
  geofence_stats_rpts_type* const zp_gf_state_rpts
);

#define GM_POS_OP_MODE_STANDALONE   0
#define GM_POS_OP_MODE_MS_BASED     1
#define GM_POS_OP_MODE_MS_ASSISTED  2


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;
  uint8       u_pos_fix_src;
  uint8       client_id;
  uint32      q_hor_acc_req;
  uint8       u_num_fixes;
  uint32      q_tbf;
  uint8       u_pos_op_mode;
  uint8       u_power_prof;
  uint8       u_hp_proc_wakeup_prof;
  uint8       u_nw_usage_prof;
  uint32      q_meas_age;
}geofence_pos_req_log_rpt;

#define IOD_SENSOR_STATE_ON  (1)
#define IOD_SENSOR_STATE_OFF (0)

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8        u_Version;                /* Version number of DM log */
  uint8        u_sensor_state;
}geofence_als_iod_sensor_state_log_rpt;

typedef PACKED struct PACKED_POST
{
  uint8       u_backoff_algorithm; /*Backoff algorithm Used*/
  uint32      q_backoff_secs; /*Backoff seconds used*/
  uint32      q_gf_id_for_backoff; /*Geofence Id used for backoff*/
  boolean     u_dpo_can_be_tried; /*Dpo can be tried*/
  boolean     u_need_apptrack; /*Whether apptrack is needed*/
  
}geofence_ebee_log_info;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;
  uint8       u_Version;                /* Version number of DM log */
  uint32      q_Fcount;                 /* Local millisecond counter */
  uint32      q_count;
  geofence_ebee_log_info z_ebee_log_info;
}geofence_ebee_log_rpt;

#if defined(T_WINNT)
#error code not present
#endif

/****************** Function Declarations**************************************/
/*===========================================================================
FUNCTION gm_log_iod_state

DESCRIPTION
  This function logs Geofencing IOD sensors state

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_iod_state(uint8 u_sensor_state);

/*===========================================================================
FUNCTION gm_log_motion_sensing

DESCRIPTION
  This functionlogs Geofencing motion sensing events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_motion_sensing(geofence_motion_detection_rpt motion_detection_report);

/*===========================================================================
FUNCTION gm_log_position

DESCRIPTION
  This functionlogs logs positions received by Geofencing

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_position(sm_gm_position_notification_type *gm_position);

/*===========================================================================
FUNCTION gm_log_wwan_motion_class_rpt

DESCRIPTION
  This function logs Geofencing WWAN Motion Classification events

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_wwan_motion_class_rpt(geofence_wwan_mot_class_rpt *wwan_motion_class_rpt);

/*===========================================================================
FUNCTION gm_log_wifi_motion_state_detection_rpt

DESCRIPTION
  This function logs Geofencing WIFI Motion Detection state report

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_wifi_motion_state_detection_rpt
(
  geofence_wifi_mot_detection_state *wifi_mot_detection_rpt,
  geofence_wifi_mot_detection_scan_ap_set *ap_set
);

/*===========================================================================
FUNCTION gm_log_geofence_add

DESCRIPTION
  This function logs the information related to geofence add operation.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_geofence_add(geofence_add_info_log_rpt *p_geofence_add_info);

/*===========================================================================
FUNCTION gm_log_geofence_del

DESCRIPTION
  This function logs the information related to geofence delete operation.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_geofence_del(geofence_del_info_log_rpt *p_geofence_del_info);

/*===========================================================================
FUNCTION gm_log_geofence_purge

DESCRIPTION
  This function logs the information related to geofence purge operation.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_geofence_purge(geofence_purge_info_log_rpt *p_geofence_purge_info);

/*===========================================================================
FUNCTION gm_log_geofence_breach

DESCRIPTION
  This function logs the information related to geofence breach operation.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_geofence_breach(geofence_breach_info_log_rpt *p_geofence_breach_info);

/*===========================================================================
FUNCTION gm_log_gf_stats

DESCRIPTION
  This function logs Geofencing stats by collecting the stats from registered GF stats providers

DEPENDENCIES

RETURN VALUE
Void

SIDE EFFECTS
None
===========================================================================*/
void gm_log_gf_stats(void);


/*===========================================================================
FUNCTION gm_log_register_gf_stats_provider

DESCRIPTION
  This function provides the api for client to register to GM logging module as GF stats providers

DEPENDENCIES

RETURN VALUE
Void

SIDE EFFECTS
None
===========================================================================*/
void gm_log_register_gf_stats_provider(gm_gf_stats_prv_cb *p_gf_stats_prv_cb);


/*===========================================================================
FUNCTION gm_log_clear_stats

DESCRIPTION
  This function clears Geofencing stats

DEPENDENCIES

RETURN VALUE
Void

SIDE EFFECTS
None
===========================================================================*/
void gm_log_clear_stats(void);

/*===========================================================================
FUNCTION gm_log_pos_request

DESCRIPTION
  This function logs the position request from Geofencing engine

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_pos_request(sm_gm_client_pos_req_type *pz_pos_req_type,
                        sm_gm_client_id  client_id);

/*===========================================================================
FUNCTION gm_log_ebee_info

DESCRIPTION
  This function logs Geofencing EbEE Info

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_log_ebee_info(geofence_ebee_log_rpt *p_ebee_info);

#endif /*GM_LOG_H*/



