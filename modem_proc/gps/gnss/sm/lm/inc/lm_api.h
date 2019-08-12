#ifndef LM_API_H
#define LM_API_H
/*----------------------------------------------------------------------------
 *  Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*=============================================================================

                        Location Manager API Header File

GENERAL DESCRIPTION
  This file contains contains the structure definitions and function prototypes for 
  the Location Manager software interface.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.

  Copyright (c) 2015, 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/lm/inc/lm_api.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/16   mj      Add a new flag b_e911_session to indicate e911 session in progress
07/15/15   al      Support for prescribed dwell command versions 51 and 53
08/21/14   jv      16-bit SVID support
01/12/10   gk      Force receiver off when end session is called
06/10/09   gk      ODP 2.0 support
08/04/08   gk      Force receiver off
06/04/08   gk      Flg to indicate on-demand session was added to the wrong struct
06/18/08   jlp     Added messaging support so tm_diag can set the Sv No Exist
                    list in cd.
06/12/08   lt      Support for external coarse position injection.
06/04/08   gk      Add a flag to indicate on-demand session
08/01/07   rw      Add support for XTRA feature
12/05/06   mr      change q_qos_accuracy_threshold from uint8 to uint32
06/12/06   mr      Initial version.

=============================================================================*/


/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "gps_common.h"
#include "gnss_common.h"
#include "gnss_consts.h"
#include "sm_api.h"
#include "mgp_api.h"
#include "gnss_common.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

/*=============================================================================

       Macros

=============================================================================*/

/*=============================================================================

       Typedefs

=============================================================================*/
typedef enum {
  LM_GPS_MODE_NONE = -1,
  LM_GPS_MODE_1X_MSA,
  LM_GPS_MODE_MSA,
  LM_GPS_MODE_MSB,
  LM_GPS_MODE_FTM,
  LM_GPS_MODE_SA_RF,
  LM_GPS_MODE_STANDALONE,
  LM_GPS_MODE_ODP
} lm_gps_mode_e_type;

typedef enum {
  LM_UP_CP_MODE_UP=0,
  LM_UP_CP_MODE_CP  
} lm_up_cp_mode_e_type;

typedef enum {

  LM_MO_MT_MODE_MO = 0,   /*session is for Mobile Originated Request*/
  LM_MO_MT_MODE_MT        /*session is for Mobile Terminated Request*/
} lm_mo_mt_mode_e_type;

typedef struct {
  uint32                        q_timeout;
    /* Timeout before which measurements should be ready */
  uint32                        q_rtd;
  /*Expected round trip delay of SA data*/
  boolean                        q_data_availability;
  /* SA data availability */
  uint8                           u_qos;  
  /* Performance QOS-PRQ */
  boolean                       multi_report_msa;
  /* flag for multi-reporting in MSA */
} lm_session_request_gps_s_type;

typedef struct {
  uint32                       q_timeout;
    /* Timeout before which measurements should be ready */
  uint8                        q_qos;
    /* measurement QoS for searching.Usage TBD */
  /*  uint32                        q_tm_mask;*/
  boolean                      u_FTcalFlag;
    /* Flag if this is the Fast TCal session */
    /* GPS measurement report mask for TM as destination */
} lm_session_request_cdma_s_type;

typedef struct {
  uint32                        q_timeout;
    /* Timeout before which measurements should be ready */
  uint32                        q_qos_accuracy_threshold;
  /* measurement QoS for searching */
  boolean                        q_data_availability;
  /* SA data availability */
  uint32                        q_rtd;
  /*Expected round trip delay of SA data*/
  boolean                       u_report_meas;
  /*To report measurements or not*/
  boolean                       u_dynamic_qos_enabled;
  /*Dynamic QoS logic allowed or not*/
  boolean                       u_on_demand;
  /* flag to indicate if the session type is on-demand */
  boolean                       u_force_mgp_off;
  /* flag to indicate if the MGP should be turned off with force flag*/
  uint32                        q_Punc;
  /* current punc value for the ODP session */
  uint32                        q_Tunc;
  /* current Tunc value for the ODP session */
} lm_session_request_fix_s_type;

typedef struct{
  lm_gps_mode_e_type            e_mode;
    /* MSBased, MSA, etc */
  uint32                        q_tbf;                  
    /* When the next GPS request is going to arrive */
  uint32                        q_num_fixes;
    /* Number of fixes */
  lm_up_cp_mode_e_type          e_up_cp;
  /*UP or CP*/
  boolean                       u_do_data_demod;
  /*Data demodulation enabled or not*/
  lm_mo_mt_mode_e_type          e_mo_mt;
  /*Mobile Originated or Mobile Terminated Request*/
  boolean						b_e911_session;
  /* Is this an E911 session or not. TRUE == E911 session */
} lm_session_request_session_s_type;

typedef struct {
  /* LM_SESSION_REQUEST_GPS and 
     (LM_SESSION_REQUEST_START or LM_SESSION_REQUEST_CONTINUE)*/
  lm_session_request_gps_s_type     z_gps_request; 
  /* LM_SESSION_REQUEST_CDMA and 
     (LM_SESSION_REQUEST_START or LM_SESSION_REQUEST_CONTINUE)*/
  lm_session_request_cdma_s_type    z_cdma_request;   
  /* LM_SESSION_REQUEST_FIX and 
     (LM_SESSION_REQUEST_START or LM_SESSION_REQUEST_CONTINUE)*/
  lm_session_request_fix_s_type     z_fix_request; 
  /*LM_SESSION_REQUEST_SESSION and
    (LM_SESSION_REQUEST_START or LM_SESSION_REQUEST_CONTINUE)*/
  lm_session_request_session_s_type z_session_request;
} lm_session_request_s_type;


typedef enum {
  LM_SESSION_REQUEST_FIX                  =   0x01,  /*stop/start/continue position fix*/         
  LM_SESSION_REQUEST_GPS                  =   0x02,  /*stop/start/conitnue PRM*/         
  LM_SESSION_REQUEST_CDMA                 =   0x04,  /*stop/start/continue PPM*/
  LM_SESSION_REQUEST_SESSION              =   0x08,  /*stop/start LM-TM session*/
  LM_SESSION_REQUEST_CDMA_GET_RESULTS     =   0x10,  /*request for PPM results. 
                                                       Used with LM_SESSION_REQUEST_CONTINUE*/
  LM_SESSION_REQUEST_CDMA_DEFER_OFF       =   0x20   /*request for PPM results. 
                                                       Used with LM_SESSION_REQUEST_START*/
} lm_session_type_e_type;

typedef enum{
   LM_SESSION_REQUEST_STOP,
   LM_SESSION_REQUEST_START,
   LM_SESSION_REQUEST_CONTINUE
} lm_session_request_type_e_type;

typedef struct {
  uint32                         q_handle;
  lm_session_request_type_e_type e_req_type;
  uint32                         q_req_mask; /* flags defined in lm_session_type_e_type*/
  lm_session_request_s_type      z_request;
} lm_session_request_info_s_type;


typedef struct
{
  gnss_DeleteInfoStructType z_mgp_info;
  boolean delete_all;
  boolean force_rcvr_off;
} lm_request_delete_mgp_info_s_type;

typedef enum
{
  LM_NMEA_GGA  = 0x01,
  LM_NMEA_RMC  = 0x02,
  LM_NMEA_GSV  = 0x04,
  LM_NMEA_GSA  = 0x08,
  LM_NMEA_VTG  = 0x10
} lm_request_nmea_flag_e_type;

typedef struct
{
  uint32 q_bitmask; /*flags defined in lm_request_nmea_flag_e_type*/
} lm_request_nmea_s_type;

typedef enum
{
  LM_SET_PARAM_APP_TRK_GPS_ON_THRESHOLD,
  LM_SET_PARAM_APP_TRK_GPS_IDLE_THRESHOLD,
  LM_SET_PARAM_APP_TRK_DELAY_MARGIN
} lm_request_set_param_type_e_type;

typedef union
{
  uint32 q_gpsOnThreshold; /*LM_SET_PARAM_APP_TRK_GPS_ON_THRESHOLD*/
  uint32 q_gpsIdleThreshold;/*LM_SET_PARAM_APP_TRK_GPS_IDLE_THRESHOLD*/
  uint32 q_delayMargin;/*LM_SET_PARAM_APP_TRK_DELAY_MARGIN*/
} lm_request_set_param_value_u_type;

typedef struct
{
  lm_request_set_param_type_e_type  e_param_type;
  lm_request_set_param_value_u_type z_param_value;
} lm_request_set_param_s_type;


typedef struct
{
  gps_SlowClkTimeInjectionType z_slow_clk_time;
  gnss_SlowClkSrcEnumType        e_source;
} lm_request_slow_clk_time_inject_s_type;

typedef struct
{
  //gps_ExternCoarsePosInjectionType z_extern_coarse_pos;
  gnss_ExternCoarsePosInjectionType z_extern_coarse_pos;
} lm_request_extern_coarse_pos_inject_s_type;  

typedef struct
{
  cgps_SrvSystemType e_serv_sys_type;
} lm_request_rf_info_s_type;

typedef struct
{
  gps_TimeChkSourceType e_time_chk_type;
} lm_request_gps_info_s_type;

typedef enum
{
  LM_ASSIST_DATA_REQ_INVALID = 0,
  LM_ASSIST_DATA_REQ_MSB,   /* WL req src MSB (statefull), next req in 
                               session will have eph flag suppressed */
  LM_ASSIST_DATA_REQ_QUERY, /* WL req src general QUERY (stateless) */
  LM_ASSIST_DATA_REQ_MAX
} lm_wishlist_src_req_e_type;

typedef enum {
  LM_REQUEST_KEEP_WARM_ON,
  LM_REQUEST_KEEP_WARM_OFF,                   
  LM_REQUEST_NMEA_ON,           
  LM_REQUEST_NMEA_OFF,         
  LM_REQUEST_AIDING_DATA_WISHLIST, 
  LM_REQUEST_DELETE_MGP_INFO,
  LM_REQUEST_SET_PARAM,
  LM_REQUEST_POS_ESTIMATE,
  LM_REQUEST_SLOW_CLK_TIME_INJECT,
  LM_REQUEST_RF_INFO,
  LM_REQUEST_GNSS_TIME,
  LM_REQUEST_EXTERNAL_COARSE_POS_INJECT,
  LM_REQUEST_FORCE_RECVR_OFF,
  LM_REQUEST_RECVR_OFF,
  LM_REQUEST_RESET_LOCATION_SERVICE,
} lm_request_e_type;

typedef union {
  /* LM_REQUEST_DELETE_MGP_INFO*/
  lm_request_delete_mgp_info_s_type z_delete_request;
  /* LM_REQUEST_NMEA_ON*/
  lm_request_nmea_s_type            z_nmea_on_request;
  /* LM_REQUEST_SET_PARAM*/
  lm_request_set_param_s_type       z_set_param_request;
  /*LM_REQUEST_SLOW_CLK_TIME_INJECT*/
  lm_request_slow_clk_time_inject_s_type z_slow_clk_inject_request;
  /*LM_REQUEST_RF_INFO*/
  lm_request_rf_info_s_type         z_rf_info_request;
  /*LM_REQUEST_GNSS_TIME*/
  lm_request_gps_info_s_type        z_gps_time_request;
  /* LM REQUEST_EXTERN_COARSE_POS_INJECT */
  lm_request_extern_coarse_pos_inject_s_type z_extern_coarse_pos_inject_request;  
  /* LM_REQUEST_AIDING_DATA_WISHLIST */
  lm_wishlist_src_req_e_type        e_wishlist_req_type;
}lm_request_u_type;

typedef struct {
  lm_request_e_type    e_req_type;
  lm_request_u_type    z_request;
} lm_request_info_s_type;


typedef enum {
  LM_SESSION_INFO_AIDING_DATA,

  LM_SESSION_INFO_AIDING_DATA_DELAY,
  LM_SESSION_INFO_POS_UNC,  
  LM_SESSION_INFO_POS_OFFSET,
  LM_SESSION_INFO_TIME_UNC,
  LM_SESSION_INFO_TIME_OFFSET,
  LM_SESSION_INFO_SESSION_TYPE      
} lm_info_e_type;

typedef struct
{
  gnss_AssistDataInfoStructType z_assist_data; 
  boolean ref_loc_is_pos_fix;
  /*if z_assist_data contains reference location, is it a position fix 
    calculated by network or just a seed/reference position. This is used 
    to findout if the fix if to be reported through NMEA or not */
} lm_info_aiding_data_s_type;

typedef struct lm_info_session_info_type
{
  lm_gps_mode_e_type            e_mode;
  cgps_SrvSystemType            e_srv_system;
}lm_info_session_info_type;

typedef union {
  /*LM_SESSION_INFO_AIDING_DATA*/
  lm_info_aiding_data_s_type z_aiding_data; 

  /*LM_SESSION_INFO_AIDING_DATA_DELAY*/   
  uint32                          q_delay;

  /*LM_SESSION_INFO_POS_UNC*/
  FLT                             f_new_pos_unc;
  
  /*LM_SESSION_INFO_POS_OFFSET*/
  uint32                          q_pos_offset; 
  
  /*LM_SESSION_INFO_TIME_UNC*/
  FLT                             f_new_time_unc;
  
  /*LM_SESSION_INFO_TIME_OFFSET*/
  FLT                             f_time_offset;
  lm_info_session_info_type       z_sess_info;
}lm_info_u_type;

typedef struct {
  lm_info_e_type      e_info_type;
  lm_info_u_type      z_info;
} lm_info_s_type;

typedef enum
{
  LM_NOEXIST_ACTION_SET_TO_MASK = 0 /* Command to set No Exist List to mask */
} sv_no_exist_action_type;

typedef struct
{
  uint32 q_SvNoExistMask;           /* Mask of Sv's that don't exist */
  sv_no_exist_action_type e_Action; /* Action to perform on the Sv No Exist List */
} lm_sv_no_exist_cmd_type;

typedef struct
{
  uint8 u_ForceDpoForPowerMeas;   /* TRUE when DPO is forced for Power measurements */
} lm_force_dpo_for_power_meas_cmd_type;

/* List of ids for TM diag->LM messages. Any change to this
   enum should be reflected accordingly in tm_diag.h enum as well */
typedef enum
{
  ARIES_MSG_ID_TM_LM_FIRST = ARIES_MSG_ID_MGP_LM_END, 
  ARIES_MSG_ID_TM_LM_SARF_MODE_SWITCH,
  ARIES_MSG_ID_TM_LM_PRESC_DWELL,
  ARIES_MSG_ID_TM_LM_IQ_TEST,
  ARIES_MSG_ID_TM_LM_1SV_STANDALONE, /* Also serves single sv */
  ARIES_MSG_ID_TM_LM_MEM_BW_TEST,
  ARIES_MSG_ID_TM_LM_TSG_CONFIG,
  ARIES_MSG_ID_TM_LM_RF_CONFIG,
  ARIES_MSG_ID_TM_LM_RFDEV_TEST, /* Callisto RF Dev Multichan Track test command message */
  ARIES_MSG_ID_TM_LM_PRX_RF_CONFIG,
  ARIES_MSG_ID_TM_LM_RF_LINEARITY_CONFIG,
  ARIES_MSG_ID_TM_LM_RF_DEV_FEATURE_CAPABILITIES, /* Query for build's RF Dev feature capabilities*/  
  ARIES_MSG_ID_TM_LM_GET_LATEST_WB_FFT_RESULTS,
  /* Following enums are reserved for OEMs usage */
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_1,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_2,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_3,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_4,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_5,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_6,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_7,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_8,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_9,
  ARIES_MSG_ID_TM_LM_OEM_RESERVED_10,
  ARIES_MSG_ID_TM_LM_SPECTRUM_ANALYZER_CMD,
  ARIES_MSG_ID_TM_LM_END
} tm_diag_lm_msg_id_type;

/* Prescribed dwell command version 51.
 * This version is used with GNSS Engine rev 54 through 55. */
typedef PACKED struct PACKED_POST
{
  uint8   u_Version;                 /* Set to 51 for this type */
  uint8   u_CmdControlFlag;
  uint32  q_SeqNum;                  /* Seq number from cmd */
  
  uint8   u_GpsRxDFlag;              /* 0: No RxD
                                     ** 1: Primary + RxD linked
                                     ** 2: RxD ONLY
                                     ** 4-255: Reserved */

  uint8   u_GpsControlFlag;          /* BIT0:-
                                     ** 0 = As Prescribed. Applicable to 
                                     ** TRK dwells ONLY.No trunctation of code 
                                     ** phase center
                                     ** 1 = Automatic. Automatic code phase
                                     ** and doppler centering. Code phase and 
                                     ** doppler parameters from the DM command 
                                     ** are ignored. The GNSS software propagates
                                     ** the values from prior measurements stored
                                     ** in software.
                                     ** 
                                     ** BIT1: Applicable to TRK dwells ONLY.
                                     ** 0 = No Truncation of code phase center 
                                     ** 1 = Truncation of code phase center
                                     ** so that the peak is randomly sampled 
                                     ** within the chip.
                                     ** 
                                     ** BIT2: Applicable to TRK dwells ONLY.
                                     ** 0 = No randomization of Doppler
                                     ** 1 = Randomization of Doppler.
                                     ** 
                                     ** BIT3-BIT7:
                                     ** Reserved */

  uint8   u_GpsSv;                   /* GPS SV id or SBAS Satellite id */
  int32   l_GpsDoppMps;              /* Doppler in m/s */
  int32   l_GpsDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_GpsCpCenterCx40;         /* Code phase center in chipx40 */
  uint16  w_GpsCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_GpsSrchMode;             /* Search mode */
  uint16  w_GpsNumPostD;             /* Number of PostD */
  uint8   u_GpsPfa;                  /* PFA number */
  uint32  q_GpsReserved;
  
  uint8   u_GloControlFlag;          /* BIT0:-
                                     ** 0 = As Prescribed. Applicable to 
                                     ** TRK dwells ONLY.No trunctation of code 
                                     ** phase center
                                     ** 1 = Automatic. Automatic code phase
                                     ** and doppler centering. Code phase and 
                                     ** doppler parameters from the DM command 
                                     ** are ignored. The GNSS software propagates
                                     ** the values from prior measurements stored
                                     ** in software.
                                     ** 
                                     ** BIT1: Applicable to TRK dwells ONLY.
                                     ** 0 = No Truncation of code phase center 
                                     ** 1 = Truncation of code phase center
                                     ** so that the peak is randomly sampled 
                                     ** within the chip.
                                     ** 
                                     ** BIT2: Applicable to TRK dwells ONLY.
                                     ** 0 = No randomization of Doppler
                                     ** 1 = Randomization of Doppler.
                                     ** 
                                     ** BIT3-BIT7:
                                     ** Reserved */
  
  uint8   u_GloSv;                   /* Glo SV id [1..14] */
  int32   l_GloDoppMps;              /* Doppler in m/s */
  int32   l_GloDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_GloCpCenterCx40;         /* Code phase center in chipx40 */
  uint16  w_GloCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_GloSrchMode;             /* Search mode */
  uint16  w_GloNumPostD;             /* Number of PostD */
  uint8   u_GloHwChan;               /* Glo HW chan */
  uint8   u_GloPfa;                  /* PFA number */
  uint32  q_GloReserved;

  uint16  w_DwellCnt;
  uint16  w_TargetDwellCnt;
  uint16  w_PassDwellCnt;
  uint32  q_Reserved;

  uint8   u_BdsControlFlag;          /* BIT0:-
                                     ** 0 = As Prescribed. Applicable to 
                                     ** TRK dwells ONLY.No trunctation of code 
                                     ** phase center
                                     ** 1 = Automatic. Automatic code phase
                                     ** and doppler centering. Code phase and 
                                     ** doppler parameters from the DM command 
                                     ** are ignored. The GNSS software propagates
                                     ** the values from prior measurements stored
                                     ** in software.
                                     ** 
                                     ** BIT1: Applicable to TRK dwells ONLY.
                                     ** 0 = No Truncation of code phase center 
                                     ** 1 = Truncation of code phase center
                                     ** so that the peak is randomly sampled 
                                     ** within the chip.
                                     ** 
                                     ** BIT2: Applicable to TRK dwells ONLY.
                                     ** 0 = No randomization of Doppler
                                     ** 1 = Randomization of Doppler.
                                     ** 
                                     ** BIT3-BIT7:
                                     ** Reserved */

  uint8   u_BdsSv;                   /* GPS SV id or Glo Satellite id */
  int32   l_BdsDoppMps;               /* Doppler in m/s */
  int32   l_BdsDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_BdsCpCenterCx40;         /* Code phase center in chipx40 */
  uint16  w_BdsCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_BdsSrchMode;             /* Search mode */
  uint16  w_BdsNumPostD;             /* Number of PostD */
  uint8   u_BdsPfa;                  /* PFA number */
  uint32  q_BdsReserved;             /* Not really needed, but maintain it similar to GPS/GLO */
} lm_GnssPrescDwellType_V51;

/* Prescribed dwell command version 53.
 * This version is used with GNSS Engine rev 56. */
typedef PACKED struct PACKED_POST
{
  uint8   u_Version;                 /* Set to 53 for this type */
  uint8   u_CmdControlFlag;
  uint32  q_SeqNum;                  /* Seq number from cmd */
  
  uint8   u_GpsRxDFlag;              /* 0: No RxD
                                     ** 1: Primary + RxD linked
                                     ** 2: RxD ONLY
                                     ** 4-255: Reserved */

  uint8   u_GpsControlFlag;          /* BIT0:-
                                     ** 0 = As Prescribed. Applicable to 
                                     ** TRK dwells ONLY.No trunctation of code 
                                     ** phase center
                                     ** 1 = Automatic. Automatic code phase
                                     ** and doppler centering. Code phase and 
                                     ** doppler parameters from the DM command 
                                     ** are ignored. The GNSS software propagates
                                     ** the values from prior measurements stored
                                     ** in software.
                                     ** 
                                     ** BIT1: Applicable to TRK dwells ONLY.
                                     ** 0 = No Truncation of code phase center 
                                     ** 1 = Truncation of code phase center
                                     ** so that the peak is randomly sampled 
                                     ** within the chip.
                                     ** 
                                     ** BIT2: Applicable to TRK dwells ONLY.
                                     ** 0 = No randomization of Doppler
                                     ** 1 = Randomization of Doppler.
                                     ** 
                                     ** BIT3-BIT7:
                                     ** Reserved */

  uint8   u_GpsSv;                   /* GPS SV id or SBAS Satellite id */
  int32   l_GpsDoppMps;              /* Doppler in m/s */
  int32   l_GpsDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_GpsCpCenterCx40;         /* Code phase center in chipx40 */
  uint32  q_GpsCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_GpsSrchMode;             /* Search mode */
  uint16  w_GpsNumPostD;             /* Number of PostD */
  uint8   u_GpsPfa;                  /* PFA number */
  uint32  q_GpsReserved;
  
  uint8   u_GloControlFlag;          /* BIT0:-
                                     ** 0 = As Prescribed. Applicable to 
                                     ** TRK dwells ONLY.No trunctation of code 
                                     ** phase center
                                     ** 1 = Automatic. Automatic code phase
                                     ** and doppler centering. Code phase and 
                                     ** doppler parameters from the DM command 
                                     ** are ignored. The GNSS software propagates
                                     ** the values from prior measurements stored
                                     ** in software.
                                     ** 
                                     ** BIT1: Applicable to TRK dwells ONLY.
                                     ** 0 = No Truncation of code phase center 
                                     ** 1 = Truncation of code phase center
                                     ** so that the peak is randomly sampled 
                                     ** within the chip.
                                     ** 
                                     ** BIT2: Applicable to TRK dwells ONLY.
                                     ** 0 = No randomization of Doppler
                                     ** 1 = Randomization of Doppler.
                                     ** 
                                     ** BIT3-BIT7:
                                     ** Reserved */
  
  uint8   u_GloSv;                   /* Glo SV id [1..14] */
  int32   l_GloDoppMps;              /* Doppler in m/s */
  int32   l_GloDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_GloCpCenterCx40;         /* Code phase center in chipx40 */
  uint32  q_GloCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_GloSrchMode;             /* Search mode */
  uint16  w_GloNumPostD;             /* Number of PostD */
  uint8   u_GloHwChan;               /* Glo HW chan */
  uint8   u_GloPfa;                  /* PFA number */
  uint32  q_GloReserved;

  uint16  w_DwellCnt;
  uint16  w_TargetDwellCnt;
  uint16  w_PassDwellCnt;
  uint32  q_Reserved;

  uint8   u_BdsControlFlag;          /* BIT0:-
                                     ** 0 = As Prescribed. Applicable to 
                                     ** TRK dwells ONLY.No trunctation of code 
                                     ** phase center
                                     ** 1 = Automatic. Automatic code phase
                                     ** and doppler centering. Code phase and 
                                     ** doppler parameters from the DM command 
                                     ** are ignored. The GNSS software propagates
                                     ** the values from prior measurements stored
                                     ** in software.
                                     ** 
                                     ** BIT1: Applicable to TRK dwells ONLY.
                                     ** 0 = No Truncation of code phase center 
                                     ** 1 = Truncation of code phase center
                                     ** so that the peak is randomly sampled 
                                     ** within the chip.
                                     ** 
                                     ** BIT2: Applicable to TRK dwells ONLY.
                                     ** 0 = No randomization of Doppler
                                     ** 1 = Randomization of Doppler.
                                     ** 
                                     ** BIT3-BIT7:
                                     ** Reserved */

  uint8   u_BdsSv;                   /* GPS SV id or Glo Satellite id */
  int32   l_BdsDoppMps;               /* Doppler in m/s */
  int32   l_BdsDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_BdsCpCenterCx40;         /* Code phase center in chipx40 */
  uint32  q_BdsCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_BdsSrchMode;             /* Search mode */
  uint16  w_BdsNumPostD;             /* Number of PostD */
  uint8   u_BdsPfa;                  /* PFA number */
  uint32  q_BdsReserved;             /* Not really needed, but maintain it similar to GPS/GLO */
  
  uint8   u_MaxSvDwell; /* Enable max Sv dwell
                                             ** 0 for disable
                                             ** 1 for enable
                                             ** Bit0: Enable for GPS
                                             ** Bit1: Enable for GLO
                                             ** Bit2: Enable for BDS
                                             ** Bit3: Enable for GAL */

  uint8   u_GalControlFlag;          /* BIT0:-
                                     ** 0 = As Prescribed. Applicable to 
                                     ** TRK dwells ONLY.No trunctation of code 
                                     ** phase center
                                     ** 1 = Automatic. Automatic code phase
                                     ** and doppler centering. Code phase and 
                                     ** doppler parameters from the DM command 
                                     ** are ignored. The GNSS software propagates
                                     ** the values from prior measurements stored
                                     ** in software.
                                     ** 
                                     ** BIT1: Applicable to TRK dwells ONLY.
                                     ** 0 = No Truncation of code phase center 
                                     ** 1 = Truncation of code phase center
                                     ** so that the peak is randomly sampled 
                                     ** within the chip.
                                     ** 
                                     ** BIT2: Applicable to TRK dwells ONLY.
                                     ** 0 = No randomization of Doppler
                                     ** 1 = Randomization of Doppler.
                                     ** 
                                     ** BIT3-BIT7:
                                     ** Reserved */

  uint8   u_GalSv;                   /* GPS SV id or Glo Satellite id */
  int32   l_GalDoppMps;               /* Doppler in m/s */
  int32   l_GalDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_GalCpCenterCx40;         /* Code phase center in chipx40 */
  uint32  q_GalCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_GalSrchMode;             /* Search mode */
  uint16  w_GalNumPostD;             /* Number of PostD */
  uint8   u_GalPfa;                  /* PFA number */
  uint32  q_GalReserved;             /* Not really needed, but maintain it similar to GPS/GLO */

} lm_GnssPrescDwellType_V53;

typedef PACKED struct PACKED_POST
{
  uint8  u_Version;
  uint8  u_TestControl;
  uint16 w_Flags;
  uint16 w_RetransmitPktNum;
  uint8  u_RfStatusLoggingRateHz;
  uint8  u_CollectMode;
  int8   b_GloR1Chan;
  uint8  u_GloHWChan;
  uint16 w_SampleCnt1k;
  uint8  u_Integrations;
  int32  l_CenterFreqNBHz;
  uint32 q_ADCIqSatThresh;
  uint32 q_AdcIQWidebandJammerStartFreqHz;  
  uint32 q_AdcIQWidebandJammerStopFreqHz;
  int32  l_AdcIQRefTonePwrDBm;
} lm_GnssIqTestType;

typedef PACKED struct PACKED_POST
{
  uint8 u_Version;
  uint8 u_Test_Mode;            /* See gnss_RfDevTrackEnumType */
  uint8 u_GLO_HW_Chan_Num;      /* 0 for automatic, 1-12 for specific chan. */
  uint8 u_GPS_SBAS_SV_ID;       /* GPS Satellite PRN [0..32]. If zero, operation
                                 * is track ALL SVs */
  int8  l_GLO_Freq_ID;          /* GLO Satellite Frequency ID -8 for all, -7 ~ +6 
                                 * for specific Satellite */
  uint8 u_GPS_SBAS_BitEdge_Setting;  /* Disables bit-edge detection */
  uint8 u_GLO_BitEdge_Setting;  /* Disables bit-edge detection */
  uint8 u_BDS_BitEdge_Setting;  /* Disables bit-edge detection */
  uint8 u_BDS_SV_ID;            /* BDS satellite PRN [0..37] */
  uint8 u_GAL_BitEdge_Setting;  /* Disables SC code-sync for pilot channel */
  uint8 u_GAL_SV_ID;            /* GAL satellite PRN [0..36] */
  uint8 u_Command;              /* 0 - START_CLEAR_ALL
                                 * 1 - START_NO_CLEAR
                                 * 2 - STOP_CLEAR_ALL 
                                 * 3 - STOP_NO_CLEAR
                                 */
}lm_multichan_track_type;

#define C_GNSS_LM_MULTICHAN_TRACK_VER55 55

/* "Legacy" structure definition for any version below 55 */
typedef PACKED struct PACKED_POST
{
  uint8 u_Version;
  uint8 u_Test_Mode;            /* See gnss_RfDevTrackEnumType */
  uint8 u_GLO_HW_Chan_Num;      /* 0 for automatic, 1-12 for specific chan. */
  uint8 u_GPS_SBAS_SV_ID;       /* GPS Satellite PRN [0..32]. If zero, operation
                                 * is track ALL SVs */
  int8  l_GLO_Freq_ID;          /* GLO Satellite Frequency ID -8 for all, -7 ~ +6 
                                 * for specific Satellite */
  uint8 u_GPS_SBAS_BitEdge_Setting;  /* Disables bit-edge detection */
  uint8 u_GLO_BitEdge_Setting;  /* Disables bit-edge detection */
  uint8 u_Command;              /* 0 - START_CLEAR_ALL
                                 * 1 - START_NO_CLEAR
                                 * 2 - STOP_CLEAR_ALL 
                                 * 3 - STOP_NO_CLEAR
                                 */
  uint8 u_BDS_SV_ID;            /* BDS satellite PRN [0..37] */
  uint8 u_BDS_BitEdge_Setting;  /* Disables bit-edge detection */
}lm_multichan_track_legacy_type;

typedef PACKED struct PACKED_POST
{
  uint8  u_Version;
  uint16  w_Sv;             /* Satellite GPS PRN [0..32], GAL PRN[300..336]. If zero, operation is track ALL SVs */ 
  uint8   u_Command;       /* 0 - START_CLEAR_ALL
                            * 1 - START_NO_CLEAR
                            * 2 - STOP_CLEAR_ALL 
                            * 3 - STOP_NO_CLEAR
                            */
}lm_1sv_standalone_type;

typedef PACKED struct PACKED_POST
{
  uint8  u_Version;
  uint8 u_sarf_mode_state; /* Enter/exit SARF Verif mode */
} lm_sarf_mode_type;

typedef PACKED struct PACKED_POST
{
  uint8  u_Version;
  uint8  u_TestStopFlag;
  uint8  u_TestType;
  uint8  u_TaskCnt;
  uint16 w_IntegLength;
} lm_mem_bw_test_type;

typedef PACKED struct PACKED_POST
{
  uint8   u_Version;
  uint16  w_Flag;
  uint16  w_SvId; 
  int8    b_GloFreqK;
  int32   l_GpsDoppHz;
  int32   l_GloDoppHz;
  int8    b_GpsCodeDoppQuartCps;
  int8    b_GloCodeDoppQuartCps;
  uint16  w_GpsCodePhaseChips;
  uint16  w_GloCodePhaseChips;
  uint8   u_GpsDataBitsConfig;
  uint8   u_GloDataBitsConfig;
  int8    b_BdsSv;
  int32   l_BdsDoppHz;
  int8    b_BdsCodeDoppQuartCps;
  uint16  w_BdsCodePhaseChips;
  uint8   u_BdsDataBitsConfig;
} lm_TsgCmdType;

typedef PACKED struct PACKED_POST
{
  uint8   u_Version;
  uint8   u_RfMode;
  uint8   u_RfLinearity;
  uint8   u_RxdOnTimeSec;
  uint8   u_RxdOffTimeSec;
  uint8   u_AdcConfig;
  uint32  q_Reserved;
} lm_RfCmdType;

typedef PACKED struct PACKED_POST
{
  uint8   u_Version;
  uint8   u_RfMode;
  uint32  q_Reserved;
} lm_PrxRfCmdType;

typedef PACKED struct PACKED_POST
{
  uint8   u_Version;
  uint8   u_RfLinearity;
  uint32  q_Reserved;
} lm_RfLinearityCmdType;


/*************************************************************************
  GNSS assistance data IPC type                                               
*************************************************************************/
typedef struct 
{
  mgp_GnssType             e_GnssType;
  uint32                   e_AssistanceType;
  uint8                    u_Force; /* When true, the injected data will be accepted un-conditionally */

  /*Keep this as the last element*/
  uint8                    z_AssistData[1]; /*First Byte of the Assistance data*/

} gnss_AssistDataInfoStructType_ipc_type;

typedef struct
{
  boolean ref_loc_is_pos_fix;
  /*if z_assist_data contains reference location, is it a position fix 
    calculated by network or just a seed/reference position. This is used 
    to findout if the fix if to be reported through NMEA or not */

  /*Keep this as the last element*/    
  gnss_AssistDataInfoStructType_ipc_type z_assist_data;     
} lm_info_aiding_data_s_type_ipc_type;

typedef union {
  /*LM_SESSION_INFO_AIDING_DATA_DELAY*/   
  uint32                          q_delay;

  /*LM_SESSION_INFO_POS_UNC*/
  FLT                             f_new_pos_unc;
  
  /*LM_SESSION_INFO_POS_OFFSET*/
  uint32                          q_pos_offset; 
  
  /*LM_SESSION_INFO_TIME_UNC*/
  FLT                             f_new_time_unc;
  
  /*LM_SESSION_INFO_TIME_OFFSET*/
  FLT                             f_time_offset;
  lm_info_session_info_type       z_sess_info;

  /*LM_SESSION_INFO_AIDING_DATA*/
  /*Keep this as the last element*/
  lm_info_aiding_data_s_type_ipc_type z_aiding_data; 
  
}lm_info_u_type_ipc_type;

typedef struct {
  lm_info_e_type               e_info_type;

  /*Keep this as the last element*/  
  lm_info_u_type_ipc_type      z_info;
} lm_info_s_type_ipc_type;

typedef PACKED struct PACKED_POST
{
  uint8   u_Version;
  uint8   u_CmdControlFlag;
  uint8   u_ScanStartStop;
  uint8   u_GpsJobsEnabled;
  uint8   u_GloJobsEnabled;
  uint8   u_ProbePoint;
  uint8   u_TrkMode;
  uint8   u_NotchFilterReset;
  uint8   u_BdsJobsEnabled;
  uint8   u_GalJobsEnabled;
} lm_SpectrumAnalyzerCmdType;

/*=============================================================================

       Functions

=============================================================================*/

/*****************************LM-TM interface functions***********************/

/*
 ******************************************************************************
 * lm_session_request 
 *
 * Function description:
 *
 * This function is called by TM for:
 * 1.  starting/stopping/updating a session with LM
 * 2.  To request a PPM report, PRM report or position report
 *     for an already established session.
 *
 * Parameters: 
 * p_req - Pointer to structure containing request parameters
 *
 * Return value: 
 *
 * boolean - IPC send successful or not 
 *
 ******************************************************************************
*/

boolean lm_session_request
(const lm_session_request_info_s_type *p_req);

/*
 ******************************************************************************
 * lm_request 
 *
 * Function description:
 *
 * This function is called by TM to send a LM-TM session independent request to LM.
 * The type of request could be:
 * 1.  Turn keep warm processing ON/OFF
 * 2.  Turn NMEA sentence generation ON/OFF
 * 3.  Get MGP's Aiding data wishlist
 * 4.  Delete certain parameters in MGP database
 * 5.  Slow clock time injection data for MGP. 
 *
 * Parameters: 
 * p_req - Pointer to structure containing request parameters
 *
 * Return value: 
 *
 * boolean - IPC send successful or not 
 *
 ******************************************************************************
*/

boolean lm_request
(const lm_request_info_s_type *p_req);

/*
 ******************************************************************************
 * lm_info 
 *
 * Function description:
 *
 * This function is called by TM to send LM-TM session independent info to LM.
 * The information type can be:
 * 1.  Aiding data to be injected in MGP.
 * 2.  Expected network delays for getting aiding data.
 *
 * Parameters: 
 * p_info - Pointer to structure containing information parameters
 *
 * Return value: 
 *
 * boolean - IPC send successful or not 
 *
 ******************************************************************************
*/

boolean lm_info
(const lm_info_s_type      *p_info);


/*
 ******************************************************************************
 * lm_translate_lm_info_ipc_type_to_lm_info 
 *
 * Function description:
 *
 * This function translates from lm_info_s_type_ipc_type to lm_info_s_type
 *
 * Parameters: 
 * p_ipc_type - Pointer to structure lm_info_s_type_ipc_type
 * p_info - Pointer to structurelm_info_s_type
 *
 * Return value: 
 *
 * boolean - If conversion is succesfull or not
 *
 ******************************************************************************
*/
boolean lm_translate_lm_info_ipc_type_to_lm_info(lm_info_s_type_ipc_type *p_ipc_type,
                                                 lm_info_s_type *p_info);

/*****************************LM-GPSDIAG interface functions***********************/

/*===========================================================================

FUNCTION lmDiag_ForwardGSCPkt

DESCRIPTION
  This procedure is called by ARIES GPS Diag to send GPS Session Control 
  DIAG packets to LM.
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean - IPC send successful or not 

SIDE EFFECTS
  None.

===========================================================================*/
boolean lmDiag_ForwardGSCPkt(
  PACKED const void *req_pkt_ptr ,
  word pkt_len
);

/*===========================================================================

FUNCTION lm_start_sbas_test

DESCRIPTION
  This funtion starts the SBAS SV test mode which is very similar to the GPS
  Single SV track.
  
DEPENDENCIES
  None.

PARAMETERS
  p_MgpConfig - MGP Configuration 

RETURN VALUE
  boolean - IPC send successful or not 

SIDE EFFECTS
  None.

===========================================================================*/

boolean lm_start_sbas_test( const mgp_ConfigStructType * const p_MgpConfig );


/*===========================================================================

FUNCTION lm_stop_sbas_test

DESCRIPTION
  This funtion stops the SBAS SV test mode which was initiated by 
  lm_start_sbas_test.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  boolean - IPC send successful or not 

SIDE EFFECTS
  None.

===========================================================================*/

boolean lm_stop_sbas_test( void );

/*===========================================================================
FUNCTION lm_send_sarf_cmd

DESCRIPTION
  Call this function to send SARF cmds to LM:-
  1. Mode switch
  2. Single SV
  3. Presc dwell
  4. IQ test
  5. Mem BW test

PARAMETERS
  p_payload - Pointer to payload
  lm_ipc_msg_id - Enum identifying the SARF cmd. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if msg-send failed, FALSE if otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean lm_send_sarf_cmd (PACKED uint8* p_payload, tm_diag_lm_msg_id_type lm_ipc_msg_id);

/*===========================================================================
FUNCTION lm_send_sv_no_exist_cmd

DESCRIPTION
  Sends the Sv No Exist List on to lm.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if msg-send was successful, FALSE if otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean lm_send_sv_no_exist_cmd(const uint32 cq_NoExistMask,
 const sv_no_exist_action_type ce_Action);

/*===========================================================================
FUNCTION lm_send_force_dpo_for_power_meas_cmd

DESCRIPTION
  Forces DPO Operation for Power Measurements
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if msg-send was successful, FALSE if otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean lm_send_force_dpo_for_power_meas_cmd(const uint8 u_ForceDpoForPowerMeas );
/*===========================================================================
FUNCTION lm_send_xtra_done_msg

DESCRIPTION
  Send XTRA Injection Done IPC to LM
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void lm_send_xtra_done_msg(void);

#endif /* LM_API_H */
