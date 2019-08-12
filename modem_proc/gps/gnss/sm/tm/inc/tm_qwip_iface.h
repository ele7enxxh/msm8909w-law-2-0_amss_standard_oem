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
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/inc/tm_qwip_iface.h#1 $   
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/09   gk      ODP 2.0 changes
04/02/09   ns      Added comments
03/27/09   jd      Added a generic abort handling mechanism in SysD
03/04/09   ns      Added Support to handle GPS State info
02/19/09   ns      CREATED
===========================================================================*/
#ifndef TM_QWIP_IFACE_H
#define TM_QWIP_IFACE_H

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* 1.A SysD Controller States*/
typedef enum
{
  TM_SYSD_CNTRL_INIT = 0,        /* 0 */
  TM_SYSD_CNTRL_GNSS_ONLY,       /* 1 */
  TM_SYSD_CNTRL_GNSS_WIPER_LF,   /* 2 */ 
  TM_SYSD_CNTRL_GNSS_WIPER_HF, /* 3 */
  TM_SYSD_CNTRL_WIPER_ONLY,    /* 4 */
  TM_SYSD_CNTRL_MAX_STATES     /* 5 */
} tm_sysd_controller_state_e_type;

/* 1.B SysD Controller Events*/
typedef enum
{
  SYSD_CNTRL_EVT_SESS_START = 0,  /* 0 */
  SYSD_CNTRL_EVT_SESS_END,        /* 1 */
  SYSD_CNTRL_EVT_GET_POS,         /* 2 */
  SYSD_CNTRL_EVT_GNSS_MODE_TRANS, /* 3 */
  SYSD_CNTRL_EVT_WIPER_FIX_FAIL,  /* 4 */
  SYSD_CNTRL_EVT_COARSE_POSN_INJ, /* 5 */
  SYSD_CNTRL_EVT_WIPER_WUP_TRANS, /* 6 */
  SYSD_CNTRL_EVT_QUICK_TTFF_TIMER_EXP,/* 7 */
  SYSD_CNTRL_EVT_TRIG_GNSS_ONLY,      /* 8 */
  SYSD_CNTRL_EVT_TRIG_GNSS_WIPER_LF,  /* 9 */
  SYSD_CNTRL_EVT_TRIG_GNSS_WIPER_HF,  /* 10 */
  SYSD_CNTRL_EVT_TRIG_WIPER_ONLY,     /* 11*/
  SYSD_CNTRL_EVT_GNSS_SLEEP_TIMER_EXP, /* 12 */
  SYSD_CNTRL_EVT_GPS_STATE_INFO,       /* 13 */
  SYSD_CNTRL_EVT_SESS_ABORT,           /* 14 */
  SYSD_CNTRL_EVT_SESS_ABORT_DONE,      /* 15 */
  SYSD_CNTRL_MAX_EVTS                  /* 16 */
} tm_sysd_controller_evts_e_type;

/* 1.C Sysd cntrl function payload*/
typedef struct tm_SysdPosPuncTunc
{
  boolean b_Valid; /* if Punc Tunc are valid */
  FLT f_Tunc; /* in us */
  FLT f_Punc; /* in meters */
} tm_SysdPosPuncTunc;

typedef enum
{
  SYSD_CNTRL_POS_REQ_TYPE_PDAPI,
  SYSD_CNTRL_POS_REQ_TYPE_ODP 
} tm_sysd_cntrl_payload_e_type;


typedef struct {
  pdsm_cmd_s_type pdsm_cmd_type; // session is a standalone session, with QOS parameters set based on the ODP mode
  tm_SysdPosPuncTunc p_SysdPosPuncTunc; // current PUNC, TUNC values from the ODP module
  boolean odp_session; // true if ODP session
  uint8   odp_mode_type;
} tm_pdsm_cmd_s_type;

typedef  union
{
  pdsm_cmd_s_type *pdsm_cmd_type;
  tm_SysdPosPuncTunc *p_SysdPosPuncTunc;
  tm_sess_stop_reason_e_type abort_reason; /* Applicable only for event = SYSD_CNTRL_EVT_SESS_ABORT */
  tm_pdsm_cmd_s_type *tm_pdsm_cmd_type;
}sysd_cntrl_payload_u_type;

typedef struct
{
  tm_sysd_controller_evts_e_type event;
  tm_sysd_cntrl_payload_e_type   req_type;
  sysd_cntrl_payload_u_type cmd;
}sysd_cntrl_func_packet_type;


/*===========================================================================

FUNCTION tm_sysd_controller_handler

DESCRIPTION
  HAndle all controller state transitions
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_sysd_controller_handler(sysd_cntrl_func_packet_type *p_cntrl_payload);

#ifdef __cplusplus
}
#endif

#endif /* TM_QWIP_IFACE_H */

