/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM - QWiP Core

General Description
  This file contains implementation for TM System Determination module
  
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_qwip_core.h#1 $  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/10   ns      CR 235086: Fix for Crash due to memory leak in timer deallocation
03/27/09   jd      Added a generic abort handling mechanism in SysD
03/16/09   ns      Send 0x1370 if Wifi fix reported to App
03/04/09   ns      Remove Quick GPS fix possible param from tm_sysd_params
02/19/09   ns      Ported interface pieces to tm_qwip_iface.h
02/06/09   ns      Modified timers for gnss and wiper sleep
12/04/08   ns      CREATED
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gps_variation.h"
#include "tm_qwip_iface.h"
#include "tm_data.h"

/* Enum for the different sysd state machine*/
typedef enum
{
  TM_SYSD_CONTROLLER_SM,
  TM_SYSD_GNSS_SM,
  TM_SYSD_WIPER_SM
}tm_sysd_sm_e_type;

/* Enum for GNSS Mode trasnsitions*/
typedef enum
{
  GNSS_MODE_TRANS_STRONG,
  GNSS_MODE_TRANS_WEAK,
  GNSS_MODE_TRANS_KF_FAIL,
  GNSS_MODE_TRANS_MEAS_FAIL,
  GNSS_MODE_TRANS_DEFAULT
}tm_sysd_gnss_mode_trans_e_type;

/* Enum for all GNSS Transition timer types*/
typedef enum
{
  GNSS_TRANS_TIMER_STRONG,
  GNSS_TRANS_TIMER_WEAK,
  GNSS_TRANS_TIMER_KF_FAIL,
  GNSS_TRANS_TIMER_MEAS_FAIL,
  GNSS_TRANS_TIMER_WUP_TIMER,
  GNSS_TRANS_TIMER_SLEEP_TIMER,
  GNSS_TRANS_TIMER_NOTSET
}tm_sysd_gnss_trans_timer_e_type;

typedef enum
{
  WIPER_TRANS_TIMER_NOTSET,
  WIPER_TRANS_TIMER_SLEEP,
  WIPER_TRANS_TIMER_WUP,
  WIPER_TRANS_TICK_TIMER
}tm_sysd_wiper_trans_timer_e_type;

typedef enum
{
  GNSS_SLEEP_DISABLED,
  GNSS_SLEEP_ALLOWED,
  GNSS_SLEEP_ACTIVE
}tm_sysd_gnss_sleep_status_e_type;

/* Enum for SYSD module timers */
typedef enum
{
  TM_SYSD_TIMER_ID_START = TM_SYSD_TIMER_ID_RANGE,
  TM_SYSD_TIMER_ID_QUICK_TTFF_TIMER,
  TM_SYSD_TIMER_ID_GNSS_SLEEP_TIMER,
  TM_SYSD_TIMER_ID_GNSS_WUP_TIMER,
  TM_SYSD_TIMER_ID_WIPER_TRANS_TIMER,
  TM_SYSD_TIMER_ID_GNSS_TRANS_TIMER,
  TM_SYSD_TIMER_ID_WIPER_STOP_TIMER,
  TM_SYSD_TIMER_ID_WIPER_TICK_TIMER,
  TM_SYSD_TIMER_ID_WIPER_SESSION_TIMER
} tm_sysd_timer_id_e_type;

typedef enum
{
  WIPER_FIX_AVAIL_MEETS_QOS,   /* 0 */
  WIPER_FIX_AVAIL_NOT_MEET_QOS, /* 1 */
  WIPER_FIX_NOT_AVAIL      /* 2*/
}tm_sysd_wiper_fix_qual;

typedef enum
{
  COMB_LOC_SRC_INVALID,
  COMB_LOC_SRC_GNSS,
  COMB_LOC_SRC_WIPER
}tm_sysd_com_src_e_type;

/* 2.A SysD GNSS States*/
typedef enum
{
  TM_SYSD_GNSS_OFF = 0,    /* 0 */
  TM_SYSD_GNSS_START,      /* 1 */
  TM_SYSD_GNSS_STRONG,     /* 2 */
  TM_SYSD_GNSS_WEAK,       /* 3 */
  TM_SYSD_GNSS_KF_FAIL,    /* 4 */
  TM_SYSD_GNSS_MEAS_FAIL,  /* 5 */
  TM_SYSD_GNSS_DORMANT,    /* 6 */
  TM_SYSD_GNSS_MAX_STATES  /* 7 */
} tm_sysd_gnss_state_e_type; 

/* 2.B SysD GNSS Events*/
typedef enum
{
  SYSD_GNSS_EVT_INIT = 0,         /* 0 */
  SYSD_GNSS_EVT_SLEEP,            /* 1 */
  SYSD_GNSS_EVT_RUDE_WUP,         /* 2 */
  SYSD_GNSS_EVT_END,              /* 3 */
  GNSS_EVT_TRANS_TIMER_EXP,       /* 4 */
  GNSS_EVT_FIX_REPORT,            /* 5 */
  GNSS_EVT_FIX_FAIL,              /* 6 */
  SYSD_GNSS_EVT_COARSE_POS_INJ,   /* 7 */
  GNSS_EVT_TRANSITION_TO_STRONG,  /* 8 */
  GNSS_EVT_TRANSITION_TO_WEAK,    /* 9 */
  GNSS_EVT_TRANSITION_TO_KF_FAIL,  /* 10 */
  GNSS_EVT_TRANSITION_TO_MEAS_FAIL, /* 11 */
  SYSD_GNSS_EVT_ABORT,             /* 12 */
  TM_SYSD_GNSS_MAX_EVTS             /* 13 */

} tm_sysd_gnss_evts_e_type;

/* 2.C SysD GNSS function payload*/

typedef struct
{
  uint8 num_svs;
}sysd_gnss_fix_rpt_type;

typedef struct
{
  uint8 num_meas;
}sysd_gnss_fix_fail_rpt_type;

typedef  union
{
  pdsm_cmd_s_type *pdsm_cmd_type;
  sysd_gnss_fix_rpt_type fix_report;
  sysd_gnss_fix_fail_rpt_type fix_fail;
  tm_sysd_gnss_trans_timer_e_type gnss_timer_type;
}sysd_gnss_payload_u_type;


typedef struct
{
  tm_sysd_gnss_evts_e_type event;
  sysd_gnss_payload_u_type cmd;
}sysd_gnss_func_packet_type;

/* 3.A SysD WiPer States*/
typedef enum
{
  TM_SYSD_WIPER_OFF = 0,    /* 0 */
  TM_SYSD_WIPER_LOW_FREQ,   /* 1 */
  TM_SYSD_WIPER_HIGH_FREQ,  /* 2 */
  TM_SYSD_WIPER_DORMANT,    /* 3 */
  TM_SYSD_WIPER_MAX_STATES  /* 4 */
} tm_sysd_wiper_state_e_type;

/* 3.B SysD WiPer Events*/

typedef enum
{
  SYSD_WIPER_EVT_SWITCH_OFF = 0,  /* 0 */
  SYSD_WIPER_EVT_SWITCH_LF,       /* 1 */
  SYSD_WIPER_EVT_SWITCH_HF,       /* 2 */
  SYSD_WIPER_EVT_RUDE_WUP,        /* 3 */
  WIPER_EVT_WUP_TIMER_EXP,        /* 4 */
  WIPER_EVT_SLEEP_TIMER_EXP,      /* 5 */
  WIPER_EVT_FIX_REPORT,           /* 6 */
  WIPER_EVT_FIX_FAIL,             /* 7 */
  SYSD_WIPER_EVT_ABORT,           /* 8 */
  TM_WIPER_MAX_EVTS               /* 9 */
} tm_sysd_wiper_evts_e_type;

/* 3.C Sysd Wiper function payload*/
typedef struct
{
  uint32  q_SlowClockCount;  /* slow clock tick at fix report */
  int32   n_Lat;  
  int32   n_Lon;  
  uint16  w_HEPE;
  uint16  w_GpsWeek;
  uint32  q_GpsMsec;
  uint8   num_of_aps_used;
}sysd_wiper_fix_rpt_type;

typedef union
{
  sysd_wiper_fix_rpt_type fix_report;
}sysd_wiper_payload_u_type;

typedef struct
{
  tm_sysd_wiper_evts_e_type event;
  sysd_wiper_payload_u_type rpt;
}sysd_wiper_func_packet_type;


/* Sysd Parameters structure */
typedef struct
{
  tm_sysd_loc_engine_cfg_e_type loc_engine_config;


  tm_sysd_controller_state_e_type sysd_cntrl_curr_state;
  tm_sysd_gnss_state_e_type sysd_gnss_curr_state;
  tm_sysd_wiper_state_e_type sysd_wiper_curr_state;
  tm_sysd_controller_state_e_type sysd_cntrl_prev_state;
  tm_sysd_gnss_state_e_type sysd_gnss_prev_state;
  tm_sysd_wiper_state_e_type sysd_wiper_prev_state;

  tm_sysd_gnss_sleep_status_e_type gnss_sleep_status;
  boolean tm_sysd_cntrl_quick_ttff_timer_set;
  tm_sysd_gnss_mode_trans_e_type gnss_mode_trans_type;

  boolean wiper_available;


  tm_sysd_gnss_trans_timer_e_type gnss_trans_timer_type;
  tm_sysd_wiper_trans_timer_e_type wiper_trans_timer_type;

  os_TimerMsgType*              quick_ttff_timer;
  os_TimerMsgType*              wiper_trans_timer;
  os_TimerMsgType*              gnss_trans_timer;  
  os_TimerMsgType*              wiper_tick_timer;
  os_TimerMsgType*              wiper_session_timer;
  boolean                       session_active;

  os_TimerMsgType*              wiper_stop_timer;
  boolean                       sysd_timers_created;
}tm_sysd_params_s_type;

extern tm_sysd_params_s_type tm_sysd_params;
/*===========================================================================

FUNCTION tm_sysd_gnss_handler

DESCRIPTION
    HAndle all GNSS SM state transitions
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/ 
extern void tm_sysd_gnss_handler(sysd_gnss_func_packet_type *p_gnss_payload);

/*===========================================================================

FUNCTION tm_sysd_wiper_handler

DESCRIPTION
      HAndle all Wiper SM state transitions
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/   
extern void tm_sysd_wiper_handler(sysd_wiper_func_packet_type *p_wiper_payload);

/*===========================================================================

FUNCTION tm_sysd_handle_timer_expiry

DESCRIPTION
  This function is called by TM-core to inform SYSD module about timer
  expiry event.
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_sysd_handle_timer_expiry(const os_TimerExpiryType *timer_data);

/*===========================================================================

FUNCTION tm_sysd_comb_inject_wiper_lr_data

DESCRIPTION
 Function to add wiper fix information to the Combiner data structure.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/  
extern void tm_sysd_comb_inject_wiper_lr_data(double d_latitude,double d_longitude,
                               sysd_wiper_fix_rpt_type *p_wiper_fix_rpt);
/*===========================================================================

FUNCTION tm_sysd_inject_gnss_lr_data

DESCRIPTION
 Function to add wiper fix information to the Combiner data structure.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_sysd_inject_gnss_lr_data(prtl_sess_info_param_u_type *p_sess_info_param,
                            prtl_sess_info_e_type * p_sess_info_type);
/*===========================================================================

FUNCTION tm_sysd_handle_gnss_fix_timeout

DESCRIPTION
 Function to add wiper fix information to the Combiner data structure.
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tm_sysd_handle_gnss_fix_timeout(sm_GnssFixRptStructType * gnss_lr_data);

/* If we are compiling on AMSS (i.e. not WIN32), OR
 * If we are compiling on SMART, use MSG_ macros. */
#if (!defined(WIN32) || defined(FEATURE_SMART_OFFLINE_COMPILATION))
#define SYSD_MSG_ERROR(a,b,c,d) MSG_ERROR(a,b,c,d)
#define SYSD_MSG_HIGH(a,b,c,d) MSG_HIGH(a,b,c,d)
#define SYSD_MSG_MED(a,b,c,d) MSG_MED(a,b,c,d)
#define SYSD_MSG_LOW(a,b,c,d) MSG_LOW(a,b,c,d)
#define SYSD_ERR_FATAL(a,b,c,d) ERR_FATAL(a,b,c,d)
#else
extern void SysDPbWriteMSG( char * p_Prefix, uint32 q_level, char a[], S32 b, S32 c, S32 d);

#define SYSD_DBG_ERRORS     0x00000001
#define SYSD_DBG_WARNING    0x00000002
#define SYSD_MSG_ERROR(a,b,c,d)  SysDPbWriteMSG("MSG_ERROR :",SYSD_DBG_ERRORS,a,b,c,d)
#define SYSD_MSG_HIGH(a,b,c,d)  SysDPbWriteMSG("MSG_HIGH :",SYSD_DBG_WARNING,a,b,c,d)
#define SYSD_MSG_MED(a,b,c,d)  SysDPbWriteMSG("MSG_MED :",SYSD_DBG_WARNING,a,b,c,d)
#define SYSD_MSG_LOW(a,b,c,d) SysDPbWriteMSG("MSG_LOW :",SYSD_DBG_WARNING,a,b,c,d)
#define SYSD_ERR_FATAL(a,b,c,d) SysDPbWriteMSG("MSG_ERROR :", SYSD_DBG_ERRORS,a,b,c,d)
#endif

/*===========================================================================

FUNCTION tm_sysd_is_wiper_running

DESCRIPTION
 Function to get whether Wiper is running
  
DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tm_sysd_is_wiper_running(void);

