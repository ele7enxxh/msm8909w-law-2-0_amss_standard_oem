/*======================================================================

               PDCOMM Application task internal header file

 GENERAL DESCRIPTION
  This file contains MACROS and data-structures internally used by 
  PDCOMM Application.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/lm/src/lm_data.h#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/15   al      Added LM_MSG_HIGH/ERROR macros with 0-3 arguments
05/21/10    gk     RC thresolds were set incorrectly from defaults
06/24/08   ank     Reinstate dynamic hepe logic for tunnel-exit scenario.
07/31/07   ank     Added Dynamic QoS and Override logic.
04/23/07   jw      Add another case for Single SV track to handle no bit edge
                   required before we enter track. 
10/11/06   br/mr   Removed include pdsmi.h and unused z_afltSrchMsgBuffers[].
06/20/06   mr      Initial creation of header file.
======================================================================*/

#ifndef LM_DATA_H
#define LM_DATA_H

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "lm_api.h"
#include "sm_api.h"
#include "aries_os_api.h"
#include "mgp_api.h"

#ifdef LM_DATA_DECLARE
  #define extern
#endif
/****************** Constants ************************************************/

#define LM_MSG_LOW( str, a, b, c )       MSG_LOW( "=LM TASK= " str, a, b, c )
#define LM_MSG_MED( str, a, b, c )       MSG_MED( "=LM TASK= " str, a, b, c )
#define LM_MSG_HIGH( str, a, b, c )      MSG_HIGH( "=LM TASK= " str, a, b, c )
#define LM_MSG_HIGH_0( str )             MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "=LM TASK= " str)
#define LM_MSG_HIGH_1( str, a )          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "=LM TASK= " str, a)
#define LM_MSG_HIGH_2( str, a, b )       MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "=LM TASK= " str, a, b)
#define LM_MSG_HIGH_3( str, a, b, c  )   MSG_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "=LM TASK= " str, a, b, c)

#define LM_MSG_ERROR( str, a, b, c )     MSG_ERROR( "=LM TASK= " str, a, b, c )
#define LM_MSG_ERROR_0( str )            MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "=LM TASK= " str)
#define LM_MSG_ERROR_1( str, a )         MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "=LM TASK= " str, a)
#define LM_MSG_ERROR_2( str, a, b )      MSG_2(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "=LM TASK= " str, a, b)
#define LM_MSG_ERROR_3( str, a, b, c )   MSG_3(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "=LM TASK= " str, a, b, c)

#define LM_ERR_FATAL( str, a, b, c )     ERR_FATAL( "=LM TASK= " str, a, b, c )

#ifdef FEATURE_SMLM_DEBUG
 #define LM_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR_FATAL( "LM: Assertion " #xx_exp " failed", 0, 0, 0 ); \
     }
#else
  #define LM_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR( "LM: Assertion " #xx_exp " failed",0,0,0 ); \
     }
#endif /* FEATURE_SMLM_DEBUG */



/****************** Constants/Enums *******************************************/
#define LM_GOING_ON_TIMEOUT   10000  /*in millisec*/

/* Due to blanking the time to decode ephemerides doubles, 
 * so the maximum timeout period is also doubled. 
 */
#define LM_GOING_OFF_TIMEOUT  60000 /*in millisec*/
#define LM_GOING_IDLE_TIMEOUT 60000/*in millisec*/

#define LM_MAX_RC_ON_THRESHOLD 5000            /*in millisec, should be same as the 
                                                default RC_ON NV itm value */
#define LM_MAX_RC_IDLE_THRESHOLD 15000         /*in millisec, should be same as the 
                                                default RC_IDLE NV item value */
#define LM_MAX_RC_SHUT_OFF_DELAY_MARGIN 3600000 /* Maximum allowed value in millisec*/
#define LM_RC_SHUT_OFF_DELAY_MARGIN_DEFAULT 60000 /*Default value if NV not set. in millisec*/

#define LM_EARLY_RAMP_TIME_MS 4000 /* In ms */
#define LM_EARLY_RAMP_ADDER_MS 1000 /* In ms */
#define LM_EARLY_RAMP_DIVIDER_MS 5000 
#define LM_FLAT_SECTION_TIME_MS 7000 /* In ms */
#define LM_HEPE_THRESHOLD 250 /* in meters */
#define LM_LAST_VALID_FIX_HEPE_LIMIT  50 /* in meters */
#define LM_MIN_NULL_FIXES_WITH_HEPE_LIMIT  2 /* Min number of consecutive null fixes combined w/ last valid fix hepe limit */
#define LM_MIN_NULL_FIXES_WITHOUT_HEPE_LIMIT  10 /* Min number of consecutive null fixes combined w/o last valid fix hepe limit */
#define LM_LAST_VALID_FIX_HEPE_MAX 500 /* Default value 500 m */
#define LM_REINSTATE_TIMEOUT_HEPE_LIMIT 50 /* Use 50m instead of Hmax at timeout in case of reinstate */

typedef enum
{
  LM_SESSION_STATE_NO_SESSION,
  LM_SESSION_STATE_IN_SESSION_REPORT_PENDING,
  LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING,
  //LM_SESSION_STATE_IN_SESSION_REPORT_PENDING_WAITING_AA_DATA,
  LM_SESSION_STATE_IN_SESSION_ERROR_REPORTED
} lm_session_state_e_type;

typedef enum
{
  LM_MGP_STATE_GOING_OFF,
  LM_MGP_STATE_OFF,
  LM_MGP_STATE_GOING_IDLE,
  LM_MGP_STATE_IDLE,
  LM_MGP_STATE_GOING_ON,
  LM_MGP_STATE_ON,

  LM_MGP_STATE_INVALID
} lm_mgp_state_e_type;

typedef enum
{
  LM_MGP_AFLT_STATE_OFF,
  LM_MGP_AFLT_STATE_ON
} lm_mgp_aflt_state_e_type;

typedef enum
{
  LM_DOG_TIMER_ID,
  LM_FIX_TIMER_ID,
  LM_GPS_TIMER_ID,
  LM_CDMA_TIMER_ID,
  LM_GOING_ON_TIMER_ID,
  LM_GOING_OFF_TIMER_ID,
  LM_GOING_IDLE_TIMER_ID,
  LM_RC_ON_TIMER_ID,
  LM_MGP_SHUT_OFF_TIMER_ID,
  LM_IQ_TESTING_TIMER_ID,
  LM_AFLT_DEFERRED_OFF_TIMER_ID 
} lm_timer_ids_e_type;

enum
{
  LM_STANDALONE_TEST_START_CLEAR_ALL = 1,
  LM_STANDALONE_TEST_START_NO_CLEAR,
  LM_STANDALONE_TEST_STOP_CLEAR_ALL,
  LM_STANDALONE_TEST_STOP_NO_CLEAR,
  LM_STANDALONE_TEST_START_CLEAR_ALL_NO_BIT_EDGE, /* No Bit Edge detection required */
  LM_STANDALONE_TEST_START_NO_CLEAR_NO_BIT_EDGE, /* No Bit Edge detection required */
  LM_STANDALONE_TEST_START_CLEAR_ALL_FORCE_DPO,   /* Force DPO clear all */
  LM_STANDALONE_TEST_START_NO_CLEAR_FORCE_DPO,     /* Force DPO no clear */
  LM_STANDALONE_TEST_START_SBAS_CLEAR_ALL,
  LM_STANDALONE_TEST_START_SBAS_NO_CLEAR,
  LM_STANDALONE_TEST_START_SBAS_CLEAR_ALL_NO_BITEDGE_DETECTION,
  LM_STANDALONE_TEST_START_SBAS_NO_CLEAR_NO_BITEDGE_DETECTION
};

typedef enum
{
  LM_RFDEV_TEST_START_CLEAR_ALL = 0,
  LM_RFDEV_TEST_START_NO_CLEAR = 1,
  LM_RFDEV_TEST_STOP_CLEAR_ALL = 2,
  LM_RFDEV_TEST_STOP_NO_CLEAR = 3
} lm_rfdev_cmd_e_type;

typedef enum
{
  LM_SARF_MODE_SLEEP,
  LM_SARF_MODE_GOING_OFF,
  LM_SARF_MODE_OFF,
  LM_SARF_MODE_GOING_ON,
  LM_SARF_MODE_ON
} lm_sarf_state_e_type;

typedef enum
{
  LM_SARF_CLIENT_DIAG,
  LM_SARF_CLIENT_FTM,
  LM_SARF_CLIENT_MAX
} lm_sarf_client_e_type;

typedef enum
{
  LM_TIME_FROM_ENGINE_ON,
  LM_TIME_FROM_SESSION_START
} lm_get_time_e_type;

/************************LOG events*********************************************/
typedef enum
{
  LM_LOG_EVENT_ID_MGP_STATE_CHANGE = 2,
} lm_log_event_id;

typedef enum
{
  LM_LOG_EVENT_DATA_MGP_STATE_UNKNOWN = 0,  
  LM_LOG_EVENT_DATA_MGP_STATE_OFF     = 1,
  LM_LOG_EVENT_DATA_MGP_STATE_IDLE    = 2,
  LM_LOG_EVENT_DATA_MGP_STATE_ON      = 3    
} lm_log_event_data_mgp_state;


/****************** Data Structures*********************************************/

/*UTC information*/
typedef struct
{
  sm_UtcInfoRptStructType z_utcInfo;
  boolean                 u_utcModelValid;
    
} lm_utc_data_type;

typedef enum
{
  LM_EARLY_EXIT_DECISION_WAIT_FIX,
  LM_EARLY_EXIT_DECISION_REPORT_FIX,
  LM_EARLY_EXIT_DECISION_REPORT_FIX_TIMEOUT,
  LM_EARLY_EXIT_DECISION_REPORT_NO_FIX_TIMEOUT
} lm_early_exit_decision_e_type;

typedef struct
{
  lm_early_exit_decision_e_type e_decision;
  uint16  w_QoSThresholdOverride;
  uint32  q_QoSTimeoutOverride;
  boolean u_UseQoSOverride;
  qword   d_TimeEngOn;
  qword   d_TimeSessionStart;
  uint32  q_TimeOfFirstValidFixAfterEngineOn;
  uint32  q_TimeOfFirstValidFixAfterSessStart;
  boolean u_FirstValidFixAfterEngineOn;
  boolean u_FirstValidFixAfterSessStart;
  FLT     f_dynamic_hepe_limit;
  boolean u_FirstReportedFixAfterEngineOn;
  FLT     f_LastValidFixHepe;
  uint32  q_NullFixReportCount;
  boolean u_ReinstateDynamicHepe;
  boolean u_TimeoutAfterReinstate;
  boolean u_OverrideTimeOfFirstValidFixAfterEngineOn;
  int32   q_ShutOffTimerInitValue;
} lm_early_exit_decision_info_s_type;

/* LM state information */
typedef struct
{
  /*Session and MGP states*/
  lm_session_state_e_type  e_sessionState; 
  lm_mgp_state_e_type      e_currentMgpState;
  lm_mgp_state_e_type      e_desiredMgpState;
  lm_mgp_aflt_state_e_type e_afltState;
  /* flags defined in lm_session_type_e_type in lm_api.h*/
  uint32                  q_reportsPending;
  uint32                  q_sessHandle;
  boolean                 u_waitingAAData;
  boolean                 u_ppm_request_pending;
  boolean                 u_ppm_get_results_pending;

  /*Session request params*/
  lm_session_request_session_s_type  z_session_request; 
  lm_session_request_gps_s_type      z_gps_request; 
  lm_session_request_cdma_s_type     z_cdma_request;   
  lm_session_request_fix_s_type      z_fix_request; 

  /*Session related timers*/
  os_TimerMsgType* p_gpsTimer;
  os_TimerMsgType* p_cdmaTimer;
  os_TimerMsgType* p_fixTimer;

  /*MGP state timers*/
  os_TimerMsgType* p_goingONTimer;
  os_TimerMsgType* p_goingOFFTimer;
  os_TimerMsgType* p_goingIDLETimer;

  /*MGP state timers related to App based tracking*/
  os_TimerMsgType* p_rcOnTimer;
  os_TimerMsgType* p_mgpShutOffTimer;
  os_TimerMsgType* p_IqTestingTimer;

  /*MGP state maintenance thresholds*/
  uint32 q_gpsOnThreshold; /*LM_SET_PARAM_APP_TRK_GPS_ON_THRESHOLD*/
  uint32 q_gpsIdleThreshold;/*LM_SET_PARAM_APP_TRK_GPS_IDLE_THRESHOLD*/
  uint32 q_delayMargin;/*LM_SET_PARAM_APP_TRK_DELAY_MARGIN*/
  /*Keep warm state*/

  /*NMEA state*/
  uint32 q_NmeaBitmask; /*flags defined in lm_request_nmea_flag_e_type*/
  
  /*UTC data*/
  lm_utc_data_type  z_utcData;
  
  /*Last position reported by MGP for this session*/
  boolean                       u_lastFixWasNoFixRpt;
  boolean                       u_lastFixRptValid;
  sm_GnssFixRptStructType       z_lastFixRpt;

  /*Best HEPE fix for the session*/
  boolean                       u_sessBestFixValid;
  sm_GnssFixRptStructType       z_sessBestFixRpt;
  float                         f_sessBestHepe;


  /*MGP ON parameters*/
  boolean                       u_MgpONParamsValid;
  mgp_ConfigStructType          z_MgpONParams;

  /*1x FTM Mode parameters*/
  uint8 u_searchMode;
  boolean u_DisableJammerDetect;
  lm_early_exit_decision_info_s_type  z_early_exit_decision;

  /* oemdre control flag, used to send data from
   * ME, PE and SV poly to OEM */
  boolean u_oemdre_onoff;
  uint32  q_gnss_oem_feature;

  boolean          u_FTCalEnabled;
  uint32           q_HepeThreshold;
} lm_control_data_s_type;


typedef struct
{
  void (*p_client_cb) ( boolean );
  lm_sarf_state_e_type e_sarf_state;
  lm_sarf_client_e_type e_client_type;
} lm_sarf_mode_switch_s_type;

LOG_RECORD_DEFINE(LOG_CGPS_SESSION_EARLY_EXIT_DECISION_C)
  uint8   u_PacketVersion;
  uint8   e_decision;
  uint16  w_QoSTimeoutOverride;
  uint32  q_TimeEngOn;
  uint32  q_TimeOfFirstValidFixAfterEngineOn;
  uint32  q_TimeSessionStart;
  uint32  q_TimeOfFirstValidFixAfterSessStart;
  uint32  q_FCount;
  uint16  w_QoSHepeOverride;
  uint16  w_HepeLimitCurrent;
  uint16  w_HepeLimitDynamic;
  uint16  w_HepeMax;
  uint16  w_HepeMaxDefault;
  uint8   u_FixValid;
  uint16  w_Hepe;
LOG_RECORD_END

/****************** Global variables *************************/

extern lm_control_data_s_type z_lmControl;

extern lm_sarf_mode_switch_s_type z_lm_sarf_state_db;


/****************** Function declarations *******************/

/*
 ******************************************************************************
 * lm_update_session_state 
 *
 * Function description:
 *
 *
 * Parameters: 
 * e_newSessionState - new session state to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_update_session_state(lm_session_state_e_type  e_newSessionState);

/*
 ******************************************************************************
 * lm_set_waitingAAData 
 *
 * Function description:
 *
 *
 * Parameters: 
 * u_waitingAAData - new value for waitingAAData variable
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_waitingAAData(boolean  u_waitingAAData);

/*
 ******************************************************************************
 * lm_get_rc_on_threshold_period 
 *
 * Function description:
 *
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * uint32 - returns the value of parameter AAGPS_APP_TRACK_RC_ON_THRESHOLD_PERIOD
 *
 ******************************************************************************
*/

uint32 lm_get_rc_on_threshold_period(void);

/*
 ******************************************************************************
 * lm_get_rc_idle_threshold 
 *
 * Function description:
 *
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * uint32 - returns the value of parameter AAGPS_APP_TRACKING_RC_IDLE_THRESHOLD
 *
 ******************************************************************************
*/
uint32 lm_get_rc_idle_threshold(void);


/*
 ******************************************************************************
 * lm_get_rc_shut_off_delay_margin 
 *
 * Function description:
 *
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * uint32 - returns the value of parameter AAGPS_APP_TRACKING_RC_SHUT_OFF_GRACE_PERIOD
 *
 ******************************************************************************
*/
uint32 lm_get_rc_shut_off_delay_margin(void);

/*
 ******************************************************************************
 * lm_set_rc_on_threshold_period 
 *
 * Function description:
 *
 *
 * Parameters: 
 * the value of parameter AAGPS_APP_TRACK_RC_ON_THRESHOLD_PERIOD to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_rc_on_threshold_period(uint32 q_gpsOnThreshold);

/*
 ******************************************************************************
 * lm_set_rc_idle_threshold 
 *
 * Function description:
 *
 *
 * Parameters: 
 * the value of parameter AAGPS_APP_TRACKING_RC_IDLE_THRESHOLD to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_rc_idle_threshold(uint32 q_gpsIdleThreshold);


/*
 ******************************************************************************
 * lm_set_rc_shut_off_delay_margin 
 *
 * Function description:
 *
 *
 * Parameters: 
 * the value of parameter LM_SET_PARAM_APP_TRK_DELAY_MARGIN to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_rc_shut_off_delay_margin(uint32 q_delayMargin);

/*
 ******************************************************************************
 * lm_get_utc_leap_secs
 *
 * Function description:
 * Returns the UTC leap seconds
 *
 * Parameters: 
 * None
 *
 * Return value: 
 * utc leap seconds
 *
 ******************************************************************************
*/

uint8 lm_get_utc_leap_secs(void);


/*
 ******************************************************************************
 * lm_log_early_exit_decision
 *
 * Function description:
 *
 * This function generates a DM log message
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
extern void lm_log_early_exit_decision(
  const uint32 q_TimeFromEngOn, 
  const uint32 q_TimeFromSessStart, 
  const uint32 q_FCount, 
  const uint16 w_HepeLimitCurrent, 
  const uint16 w_HepeLimitDynamic,
  const uint16 w_HepeMax,
  const uint16 w_Hepe,
  const boolean u_FixValid
);


/*
 ******************************************************************************
 * lm_get_time_from_start 
 *
 * Function description:
 *
 * This function gets corresponding time from Engine ON/ Fix start depending on enum
 *
 * Parameters: 
 *
 * Return value: Time in seconds
 *
 *
 ******************************************************************************
*/
extern uint32 lm_get_time_from_start(lm_get_time_e_type e_StartType);

#ifdef LM_DATA_DECLARE
  #undef extern
#endif

/*
 ******************************************************************************
 * lm_mgp_aflt_FTCalEnable 
 *
 * Function description:
 *
 * This function indirectly enables Fast TCal in the PPM module. 
 *
 * Parameters: 
 *
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
extern void lm_mgp_aflt_FTCalEnable( void );

#endif /* LM_DATA_H */
