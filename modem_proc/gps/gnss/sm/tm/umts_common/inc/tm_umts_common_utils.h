#ifndef TM_COMMON_UMTS_UTILS_H
#define TM_COMMON_UMTS_UTILS_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

                  TM_COMMON Header File

DESCRIPTION
  This header file contains definitions of common data structure necessary for
  TM module.

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_common/inc/tm_umts_common_utils.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  08/26/15   gk      90% conf reporting changes
  08/05/15   rh      Moved carrier-specific NV 1920 (pos mode support) to TM core 
  06/09/15   mj     Add support for W+W feature
  12/08/14   gk     add new events to PDAPI
  09/11/14   mj     Modify tm_umts_common_utils_rr_session_status_notify to handle asid
  10/23/13   rk      Removed redundant NV position method types & used PDAPI defs.
  09/30/13    rk     Removed reference to deprecated NV item 3649 & use RRC API to get
                     the selected RRC protocol release major version.
  02/01/13   rk      Added global helper function to check on glonass enablement.
  09/07/12   mj      Added z_external_cell_info_cached
  04/01/10   atien   Add NMR feature
  08/22/08   jlp     Added support of emergency call simulation for simulating E911 A-GPS
  02/08/08    lt     Added time stamp to the cached reference position of the session.
  07/17/07    lt     Moved NV item reading of refTime and refLoc from TmCore to here.
  06/20/07    lt     Added handling of NV Item aagps_gps_lock_control.
  05/25/07    lt     Added more utility function prototypes.
  05/11/07    lt     Moved variables and constants common to RRLP and RRC in here.
  05/01/07    lt     Initial Release
===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */

#include "gps_common.h"
#include "gps_consts.h"

#include "pdapiclient.h"
#include "tm_pdapi_iface.h"
#include "tm_common.h"

#include "tm_rrlp.h" 
#include "tm_data.h"

/*####################################################

        Data Structure Definition Section

####################################################*/


/* Gps Assistance Wishlist Bitmap: the order is of 
   the lower 9 elements from GSM09.31 */
#define C_G0931_GPS_ASSIST_ALM      0x0001  
#define C_G0931_GPS_ASSIST_UTC      0x0002  
#define C_G0931_GPS_ASSIST_IONO     0x0004  
#define C_G0931_GPS_ASSIST_NAVMODL  0x0008  
#define C_G0931_GPS_ASSIST_DGPS     0x0010  
#define C_G0931_GPS_ASSIST_REFLOC   0x0020  
#define C_G0931_GPS_ASSIST_REFTIME  0x0040  
#define C_G0931_GPS_ASSIST_ACQ      0x0080  
#define C_G0931_GPS_ASSIST_RTI      0x0100 

#define C_FOUR_HRS_MSEC     14400000L          /* 4 hours worth of msec */
#define C_MSEC_PER_CHIP   (1.0 / 1023.0 )      /* msec per chip */
#define C_MSEC_FRAC_CHIP  (C_MSEC_PER_CHIP / 1024 )  /* msec per 1024th of a chip */

#define C_NUM_16SEC_IN_ONE_HOUR 225 /* 225 16-sec. in one hour: 3600/16 */

/* GPS Acquisition Assistance message Code Phase Search Window parameters.
   The parameters are assumed to be one sided search window
   and conversion is made from units of chips to ms. (1023 chips = 1ms) */
static const float SvTimeUnc[16] = {(FLT)0.5,
                                    (FLT)C_MSEC_PER_CHIP,
                                    (FLT)2.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)3.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)4.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)6.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)8.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)12.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)16.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)24.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)32.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)48.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)64.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)96.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)128.0 * (FLT)C_MSEC_PER_CHIP,
                                    (FLT)192.0 * (FLT)C_MSEC_PER_CHIP };


/* GAD shape defined shapes of reference location */
#define C_POS_SHAPE_ELLIP                0   /* ellipsoid point */
#define C_POS_SHAPE_ELLIP_UNC_CIR        1   /* ellipsoid point w/ unc circle */
#define C_POS_SHAPE_ELLIP_UNC_ELLIP      3   /* ellipsoid point w/ unc ellipse */
#define C_POS_SHAPE_POLYGON              5   /* polygon */
#define C_POS_SHAPE_ELLIP_ALT            8   /* ellip. point w/ alt */
#define C_POS_SHAPE_ELLIP_ALT_UNC_ELLIP  9   /* ellip. point w/ alt & unc. elli */
#define C_POS_SHAPE_ELLIP_ARC            10  /* ellipsoid arc */

#define HEADING_SCALE (128.0 / 45.0)

/* convert RRLP / RRC azimuth/elevation to radius */
#define C_RRLP_RRC_AZIMUTH_TO_RAD   C_PI / 16.0f                 /* (360.0/32.0) * C_DEG_TO_RAD */
#define C_RRLP_RRC_ELEV_TO_RAD      C_RRLP_RRC_AZIMUTH_TO_RAD    /* (90.0/8.0) * C_DEG_TO_RAD */

#define LAT_IS801_SCALING   (90.0/(DBL)(1L<<23))
#define LON_IS801_SCALING   (180.0/(DBL)(1L<<23))
#define LATITUDE_SCALING    (LAT_IS801_SCALING/256.0)
#define LONGITUDE_SCALING   (LON_IS801_SCALING/256.0)
#define MAX_UNCERTAINTY     (9999000.0F)
#define DEG_TO_RADIANS      (2.0*C_PI/360.0)

/* NvItem cgps_pde_server_address_url has limit of 128 bytes */
#define C_MAX_URL_SIZE_BYTE      128  

#define C_DEFER_ALM_REQ_MSEC     300000 /* 5 * 60000 ms = 5 minutes */
#define C_DEFER_EPH_REQ_MSEC     300000 /* 5 * 60000 ms = 5 minutes */
#define C_DEFER_IONO_REQ_MSEC    300000 /* 5 * 60000 ms = 5 minutes */
#define C_DEFER_UTC_REQ_MSEC     300000 /* 5 * 60000 ms = 5 minutes */
#define C_DEFER_REFLOC_REQ_MSEC  300000 /* 5 * 60000 ms = 5 minutes */
#define C_DEFER_GLO_AUXI_INFO_REQ_MSEC    300000 /* 5 * 60000 ms = 5 minutes */
#define C_DEFER_GLO_TIME_MODEL_REQ_MSEC   300000 /* 5 * 60000 ms = 5 minutes */
#define C_LPP_OTDOA_REQ_MSEC     1000    /* 1000 ms */

#define C_DEFER_GM_REQ_MSEC     2 * 60 * 60 * 1000 /*2 hours*/

/* default confidence value is 90%. Change to 68% if legacy behavior is required.
if any other values are used, 68% will be used in the code*/
#define TM_CP_DEFAULT_CONFIDENCE (90) 
#define TM_CONF_REPORTED_FROM_PE (39) /* PE fixes are reported with confidence 39% */

typedef enum
{
  C_COMM_BEGIN  =     PDSM_PD_EVENT_COMM_BEGIN,
  C_COMM_CONNECTED =   PDSM_PD_EVENT_COMM_CONNECTED,
  C_COMM_FAILURE  =   PDSM_PD_EVENT_COMM_FAILURE,
  C_COMM_DONE    =    PDSM_PD_EVENT_COMM_DONE
}pd_event_comm_state_e_type;


typedef enum
{
  C_EMERGENCY_HANDLING_NULL,                      /* not in emergency call */
  C_EMERGENCY_HANDLING_REJ_ALL,                   /* in emergency call, reject all */
  C_EMERGENCY_HANDLING_ACCEPT_ONLY_HIGH_PRIORITY, /* in emergency call, accept only high priority NI */
  C_EMERGENCY_HANDLING_ACCEPT_ALL                 /* in emergency call, accept all NI */
}emergency_handling_e_type;

typedef enum
{
  C_UMTS_SESSION_UNDEF, /* undefined */
  C_UMTS_SESSION_WCDMA_CP_NI,  /* WCDMA Control Plane, Network Initiated */
  C_UMTS_SESSION_GSM_CP_NI     /* GSM Control plane, Network Initiated */
}umts_session_type_e_type;

/* GAD shape of Ellipsoid-alt-unc-ellipsoid for RRLP,
   Do not change this structure */
typedef struct
{
  uint8 u_ShapeCode; /* use 0x90 */
  uint8 u_Lat0;
  uint8 u_Lat1;
  uint8 u_Lat2;
  uint8 u_Long0;
  uint8 u_Long1;
  uint8 u_Long2;
  uint8 u_Alt0;
  uint8 u_Alt1;
  uint8 u_UncMajor; /* unc semi-major */
  uint8 u_UncMinor; /* unc semi-minor */
  uint8 u_OrientMajor; /* orientation of major axis */
  uint8 u_UncAlt;   /* unc. altitude */
  uint8 u_Confidence;
}gad_ellip_alt_unc_ellip_type;


typedef struct
{
  uint8 u_Octet2_LengthIndicator;
  uint8 u_Octet3_Whishlist_AH; /* A to H */
  uint8 u_Octet4_Whishlist_IP; /* I to P */
  uint8 u_Octet5_GpsWeek_MSB2; /* 2 MSB of the 10-bit GpsWeek */
  uint8 u_Octet6_GpsWeek_LSB8; /* 8 LSB of the 10-bit GpsWeek */
  uint8 u_Octet7_GpsToe;
  uint8 u_Octet8_NSAT_ToeLimit;
  uint16 w_SvId_Iode[N_SV]; /* two octets for each SV */
}GpsAssisData_Gsm0931; /* formatted as GSM 09.31 requires */

typedef struct
{
  uint8 u_Octet2_LengthIndicator;
  uint8 u_Octet3_Whishlist_AH; /* A to H */
  uint8 u_Octet4_Whishlist_IP; /* I to P */
  uint8 u_Octet5_GanssWeek_MSB4; /* 4 MSB of the 12-bit GanssWeek */
  uint8 u_Octet6_GanssWeek_LSB8; /* 8 LSB of the 12-bit GanssWeek */
  uint8 u_Octet7_GanssToe;
  uint8 u_Octet8_NSAT_ToeLimit;
  uint16 w_SvId_Iode[N_SV]; /* two octets for each SV */
}GpsAssisData_ganss_Gsm0931; /* formatted as GSM 09.31 requires */

typedef struct
{
  boolean u_pos_valid;
  boolean u_timestamp_valid;
  uint16 w_week;
  uint32 q_tow_ms;
  gad_ellip_alt_unc_ellip_type gad_ellip_alt_unc_ellip;
}RefPos_GAD; /* reference position in GAD shape */ 


typedef struct
{
  cgps_CellInfoCachedStruct z_cell_info_refloc;
  RefPos_GAD                z_RefPos_GAD;
}last_refloc_info; /* last reference location info, with cell id */


typedef enum
{
  C_RADIO_LINK_PROT_NULL,
  C_RRLP,
  C_RRC,
  C_LPP
}radio_link_prot_enum_type;

typedef struct
{
  uint8                        u_EarlyResponse_time_sec;
  uint8                        u_measure_response_time_sec; /* 1...128 */
} lpp_params_s_type;

typedef union
{
  rrlp_params_s_type z_rrlp_params;
  lpp_params_s_type  z_lpp_params;
  //rrc_params_type    z_rrc_params;
} radio_link_prot_params_u_type;

typedef struct
{
  radio_link_prot_enum_type       u_radio_link_prot;  /* RRLP vs. RRC */

  radio_link_prot_params_u_type   radio_link_prot_params;   

  uint8                       u_MoreToCome;         /* FALSE: No more SMLC data on
                                                    the way; TRUE: More SMLC
                                                    data on the way */
} tm_network_assist_s_type;  /* this keeps a copy of assistance data from SMLC */


typedef struct
{

  uint32 aagps_default_ref_time_unc_50ms; /*  copied from NV itme aagps_default_ref_time_unc -- unit: 50 msec */
  uint32 aagps_default_ref_position_unc;

  /* Only bit 2 used for simulating emergency call.  Bits 6-8 used by PE */
  uint32 aagps_development_test_control5;

  byte   aagps_emergency_services_spprt;

  uint32 aagps_default_qos_time;        /* copied from NV item aagps_default_qos_time */
  uint32 aagps_default_qos_unc;         /* copied from NV item aagps_default_qos_unc */
  uint32 aagps_rti_validity_dur_msec;   /* copied from NV item aagps_rti_validity_dur */


  #if defined(FEATURE_CGPS_UMTS_CP) || defined (FEATURE_FSM_V2) || defined(FEATURE_CGPS_NO_UMTS_TDS)
  pdsm_gps_lock_e_type        gps_lock;
  uint32  default_smlc_comm_timeout_sec; /* init. to NvItems.aagps_default_smlc_comm_timeout, in seconds */
  uint32  default_allow_rrc;   
  uint32  default_mtlr_guard_timer_sec;
  uint8   mt_lr_support; /* used by WCDMA UE capability: LCS value_added_lcs_supported: 0: not supported */
  uint8   mo_lr_support; /* HIGH and LOW accuracy support, demand AssistData / LocEstimate */
  uint8   mo_lr_2g_support; /* mo_lr_support for 2G instead of 3G */
  #endif

  #ifdef FEATURE_CGPS_UMTS_UP_SUPL
  char   slp_address[C_MAX_URL_SIZE_BYTE]; /* copied from sanity checked URL portion of NV item aagps_defalut_url[] */
  uint32 slp_ip_addr;                      /* copied from the NV item aagps_default_ip_addr */
  uint16 slp_ipv6_addr[8];                 /*copied from the NV item*/
  uint16 slp_port_number;              /* copied from the port number portion of NV item aagps_default_url[] */
  uint32 aagps_ut1_value; /* copied from NV item aagps_default_presupl_ue_timer1_value */
  uint32 aagps_ut2_value; /* copied from NV item aagps_default_presupl_ue_timer2_value */
  uint32 aagps_ut3_value; /* copied from NV item aagps_default_presupl_ue_timer3_value */
  uint8  aagps_use_transport_security;  /* copied from NV item aagps_use_transport_security */ 
  uint8  set_imsi[MAX_IMSI_LEN_BYTES];  /* Used to store a temporary copy of the IMSI that is read from the USIM
                                        in tm_umts_up_supl_comm_retrieve_imsi() */

  uint32 gnss_supl_version; /* from MSB to LSB: rsvd, major, minor, serviceIndicator */

  uint32 gnss_rrlp8_rrc8_supported; /* The least significant bit (Bit 0) is for RRLP8 UMTS UP (SUPL), 
                                       Bit 1 is for RRLP8 UMTS CP (GSM), 
                                       and Bit 2 is for RRC8 UMTS CP (WCDMA). All other bits are reserved. */

  uint8 gnss_glo_control; /* Glonass supported: 1; otherwise 0 */

  #endif   
} umts_nv_items_s_type;

#define C_TM_F3_MSG_BUF_SIZE  50 /* max. F3 msg. buffer size in byte */

#define TM_UMTS_COMMON_VARIABLES_DEFINED
#ifdef TM_UMTS_COMMON_VARIABLES_DEFINED
#define extern
#endif

extern pdsm_pd_option_s_type    z_pdsm_pd_option;
extern pdsm_pd_qos_type         z_pdsm_pd_qos;
extern nv_item_type             z_tm_umts_nv_read_buffer; /* for reading from NV */
extern umts_nv_items_s_type     z_umts_nv_item_ram_copy; 
extern char                     tm_f3_msg_buffer[C_TM_F3_MSG_BUF_SIZE]; /* for F3 diag. */
extern cgps_CellInfoCachedStruct z_cell_info_cached;
extern last_refloc_info          z_last_refloc_info; 
extern pdsm_cell_info_s_type z_external_cell_info_cached;


#ifdef extern
#undef extern
#endif

extern void tm_umts_common_utils_read_nv (void);
extern void tm_umts_common_utils_refresh_carrier_specific_nv (void);
extern uint8 tm_umts_common_utils_meter_to_GADK( boolean u_Is_Horiz, uint32 q_hor_acc_meters );
extern float tm_umts_common_utils_GADK_to_meter( uint8 u_Horiz, uint8 u_UncK );
extern uint8 tm_umts_common_utils_microsec_to_GADK( float q_time_unc_microseconds );
extern uint32 tm_umts_common_utils_NBytesGet_bigEndian (const uint8**  p_B, uint8 u_NumBytes );
extern int32 tm_umts_common_utils_S32Convert (uint32 q_Word, uint8 u_MsbBitPos);
extern void tm_umts_common_utils_GpsMsecWeekLimit( int32 *p_GpsMsecs, uint16 *p_GpsWeek );
extern uint64 tm_umts_common_utils_full_gps_ms( uint16 w_WeekNum, uint32 q_Ms );

extern boolean tm_umts_common_utils_construct_ellip_alt_unc_ellip_from_lr
(
  gad_ellip_alt_unc_ellip_type* p_zEllipAltUncEllip,
  sm_GnssFixRptStructType*      lr_data_ptr,
  uint8                          u_unc_confidence,
  boolean                       v_network_init
);

extern boolean tm_umts_common_utils_cm_emergency_call_status(void);
extern emergency_handling_e_type tm_umts_common_utils_emergency_effect (void);

extern uint16 tm_umts_common_utils_build_eph_week_from_ref_time ( uint16 w_toe, uint32 q_ref_msec, uint16 w_ref_week );

extern void tm_umts_common_utils_gsm0931_gps_assist_data_ie_build 
(
  sm_InternalAssistDataStatusStructType* p_assist_data_wishlist,
  GpsAssisData_Gsm0931*                  p_gps_assist_wishlist_gsm_0931
);
  
extern void tm_umts_common_utils_gsm0931_ganss_assist_data_ie_build 
(
  sm_InternalAssistDataStatusStructType* p_assist_data_wishlist,
  GpsAssisData_ganss_Gsm0931*            p_gps_assist_wishlist_gsm_0931
);

extern uint8 tm_umts_common_utils_psudoRMS_formater ( float f_pr_rms_meters );

extern boolean tm_umts_common_utils_gad_to_pdsm_pd_info 
(
  uint32                          q_client_id,
  uint8                               *p_bssmap_le_location_estimate, 
  pdsm_pd_gps_position_e_type         pos_type,
  pdsm_pd_position_mode_e_type        pos_mode,
  pdsm_pd_positioning_source_type     pos_source,
  uint32                              q_time_stamp_sec,
  pdsm_pd_info_s_type                 *pd_info_ptr,
  pdsm_pd_event_type                  *pd_event_ptr
);

boolean tm_umts_common_utils_gad_to_ref_loc
( 
  const uint8                         *p_bssmap_le_location_estimate, 
  uint32                              q_time_stamp_sec,
  gps_RefLocStructType                *p_ref_loc
);

void tm_umts_common_send_pdapi_comm_event 
(
  pdsm_pd_comm_protocol_e_type comm_protocol,
   pd_event_comm_state_e_type      comm_event, 
   tm_sess_handle_type             tm_active_session_handle,
   pdsm_client_id_type             client_id
);

#ifdef FEATURE_CGPS_DUAL_WCDMA
extern void tm_umts_common_utils_mm_session_status_notify( uint8 started, sys_modem_as_id_e_type e_as_id );
#else
extern void tm_umts_common_utils_mm_session_status_notify( uint8 started );
#endif

extern void tm_umts_common_utils_rr_session_status_notify( uint8 started, sys_modem_as_id_e_type  as_id );


extern boolean tm_umts_common_ni_client_start_session
(
  const tm_sess_operation_mode_e_type e_op_mode,
   const uint8                            u_accuracy_threshold,   
   const byte                             gps_session_timeout,
   const dword                            num_fixes,
   const dword                            time_between_fixes,   
   const pdsm_mt_initiator_e_type         e_mt_initiator,   
   boolean                                u_internal_session
);

extern void tm_umts_common_assist_wishlist_build
(
  sm_InternalAssistDataStatusStructType* p_assist_data_status
);

extern void tm_umts_common_glo_assist_wishlist_build
(
  sm_InternalAssistDataStatusStructType* p_assist_data_status
);

uint8 tm_umts_common_next_int_log2_max_128 ( uint8 u_num );

boolean tm_umts_common_srv_interact( umts_session_type_e_type e_session_type );

/*=========================================================================== 
FUNCTION tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal

DESCRIPTION
  This function converts 2's complement number to negative value (only if sign bit isn't zero).
  Then convert the negative value to a decimal number without adding negative sign in the front.
 
  Generally the input is from network/server & the final value is fed to MGP, where the negative sign
  is added to the front based on the sign bit.
 
  E.g.
  Input : -63 in the bit string form '100 0001b' (i.e. 65) with sign bit as '100 0000b'
  This function : Converts to negative value '1111 1111 1111 1111 1111 1111 1100 0001b'
                  Then 2's complement to get the decimal '111 1111' & ensure the sign bit is set.
 
DEPENDENCIES 
  None.
 
PARAMETERS
  uq2sComp: 32-bit 2's complement number to be converted.
  qSignBit: Sign bit indicator in the 2's complement value.
 
RETURN VALUE
  Return the decimal number when sign bit is TRUE (1), else return
  the same value.
    
SIDE EFFECTS
  None

===========================================================================*/

uint32 tm_umts_common_utils_2s_comp_to_sign_mag_to_decimal( uint32 uq_2sComp, uint32 q_SignBit );
uint32 tm_umts_common_utils_2s_comp_to_sign_mag
(
  int32  l_2s_comp,
  uint32 q_sign_bit
);

/*=========================================================================== 
FUNCTION tm_umts_common_glonass_enabled

DESCRIPTION
  Check if Glonass is enabled from the NV item.

DEPENDENCIES 
  Depends on correct population of z_umts_nv_item_ram_copy struct.

RETURN VALUE

  True if Glonass is enable.
    
SIDE EFFECTS

===========================================================================*/
static GNSS_INLINE boolean tm_umts_common_glonass_enabled(void)
{
  #ifdef FEATURE_CGPS_UMTS_UP_SUPL
  if ( z_umts_nv_item_ram_copy.gnss_glo_control != 0)
  {
    return TRUE;
  }
  else
  #endif /* FEATURE_CGPS_UMTS_UP_SUPL */
  {
    return FALSE;
  }
}


/*=========================================================================== 
FUNCTION tm_umts_common_is_lte_msb_enabled

DESCRIPTION
  Check if LTE MSB is enabled from the NV item.

DEPENDENCIES 
  Depends on correct population of z_umts_nv_item_ram_copy struct.

RETURN VALUE

  True if Glonass is enable.
    
SIDE EFFECTS

===========================================================================*/
static GNSS_INLINE boolean tm_umts_common_is_lte_msb_enabled(void)
{
  #if defined(FEATURE_CGPS_UMTS_CP) || defined (FEATURE_FSM_V2)
  if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_UP_MS_BASED) ||
       TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_4G_CP_MS_BASED) )
  {
    return TRUE;
  }
  else
  #endif /* defined(FEATURE_CGPS_UMTS_CP) || defined (FEATURE_FSM_V2) */
  {
    return FALSE;
  }
}

/*
 ******************************************************************************
 *
 * tm_umts_common_map_glo_eph_svid_to_freq_num_from_alm 
 *
 * Function description:
 *  Associates SV Id & frequencey number for Glonass eph. data & Glo Almanac model respectively.
 *  
 * Parameters:
 *   gloAlmnA [IN] : Glonass Satellite/Slot ID in the range of 1..24
 *   gloAlmHA [IN] : Glonass channel frequency number. Only the 5-bit LSB (rightmost) is needed.
 *   p_rrc_glo_eph_freq [OUT] : pointer to rrc glonass eph. buffer
 * 
 * Return value: 
 *   None.
 *
 ******************************************************************************
*/

void tm_umts_common_map_glo_eph_svid_to_freq_num_from_alm( uint8 const gloAlmnA, uint8 const gloAlmHA,
                                                           prot_glo_eph_buffer_s_type p_rrc_glo_eph_freq[N_GLO_SV] );



/*
 ******************************************************************************
 *
 * tm_umts_common_glonass_tod_mod_hour 
 *
 * Function description:
 *  Compute the Glonass Time of Day (TOD) modulo 1 hr from the Glonas TOD & after sanity check.
 *  
 * Parameters:
 *   u_GloNumFourYear [IN] : Glonass Satellite/Slot ID in the range of 1..24
 *   w_GloNumDaysInFourYear [IN] : Glonass channel frequency number. Only the 5-bit LSB (rightmost) is needed.
 *   q_GloTimeMs [OUT] : pointer to rrc glonass eph. buffer
 * 
 * Return value: 
 *   On success, Glonass Time of Day (modulo 1 hr) in 0..3599999 milliseconds (rounded down to the nearest
 *   millisecond unit) is returned.
 *   On failure return 0xFFFFFFFF;
 *
 ******************************************************************************
*/
uint32 tm_umts_common_glonass_tod_mod_hour( uint8 const u_GloNumFourYear, uint16 const w_GloNumDaysInFourYear, 
                                            uint32 const q_GloTimeMs );



/*
 ******************************************************************************
 *
 * tm_umts_common_convert_velocity
 *
 * Function description:
 *  Converts the velocity values to a common format used to populate ASN.1
 *  
 * Parameters:
 *  Input: *f_vel_east - East Vel
 *         *f_vel_north - North Vel
 *         *f_vel_vertical - Vertical Vel
 *  Output:*w_hor_veloicty - Horizontal Velocity
 *          *s_bearing - Bearing value
 *          *b_vertical_velocity - Vertical Velocity
 *           *u_ver_direct - Vertical Velocity Direction
 *           *u_ver_velocity_bit_string,
 *
 * Return value: TRUE: if conversion is succesful , FALSE otherwise
 *
 ******************************************************************************
*/
void tm_umts_common_convert_velocity(float  *f_heading_ang,
                                     uint16 *w_hor_velocity,
                                     int16  *s_bearing,
                                     int8   *b_vertical_velocity,
                                     uint8  *u_ver_direct,
                                     uint8  *u_ver_velocity_bit_string,
                                     const float  *f_vel_east,
                                     const float  *f_vel_north,
                                     const float  *f_vel_vertical);


#ifdef __cplusplus
}
#endif

#endif /* TM_COMMON_UMTS_UTILS_H */
