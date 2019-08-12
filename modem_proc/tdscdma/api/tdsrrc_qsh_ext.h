#ifndef TDSRRC_QSH_EXT_H
#define TDSRRC_QSH_EXT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            TDSRRC QSH EXT . H                 

GENERAL DESCRIPTION
   Contains declarations for function which are QUALCOMM proprietary 
   and may or may not be shipped as source code.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tdsrrc_qsh_ext.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
2/12/14    cdf     Initial version for tdsrrc_qsh_ext.h

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include <string.h>
#include "trm.h"
#include <qsh.h>
#include <sys.h>

#define TDSRRC_QSH_MAJOR_VER 0
#define TDSRRC_QSH_MINOR_VER 1

/* Below data structures are defined for metrics */
/* TDSRRC doesn't maintain any arrays. QSH takes care of maintaining the arrays for each of the metrics */
/* TDSRRC just logs the events whenever they are triggered */

typedef enum
{
  TDSRRC_QSH_METRIC_RESEL_CEL_SEL,  /* tdsrrc_qsh_metric_resel_cel_sel_s */
  TDSRRC_QSH_METRIC_RLF_OOS,        /* tdsrrc_qsh_metric_rlf_oos_s */
  TDSRRC_QSH_METRIC_HO,             /* tdsrrc_qsh_metric_ho_s */
  TDSRRC_QSH_METRIC_TIMERS_AND_CONSTANTS,  /* tdsrrc_qsh_metric_tmr_and_const_s */
  TDSRRC_QSH_METRIC_SERVING,        /* tdsrrc_qsh_metric_serving_cell_s */
  TDSRRC_QSH_METRIC_RRC_STATE,      /* tdsrrc_qsh_metric_rrc_state_s */
  TDSRRC_QSH_METRIC_OTA,            /* tdsrrc_qsh_metric_ota_s */
  TDSRRC_QSH_METRIC_CONN_END_INFO,
  TDSRRC_QSH_METRIC_MULTI_RAB_STATUS,
  TDSRRC_QSH_METRIC_MAX
}tdsrrc_qsh_metric_e;

/* Reselection and cell selection metrics */
typedef struct {
  qsh_metric_hdr_s hdr;  /* This header has the timestamp (uint16) */
  uint8 num_t_to_l_resel;
  uint8 num_t_to_t_resel;
  uint8 num_t_to_g_resel;
  uint8 num_resel_fail;
  uint8 num_cel_sel_fail;  
}tdsrrc_qsh_metric_resel_cel_sel_s; 


/* RLF and OOS metrics */
typedef struct {
  qsh_metric_hdr_s hdr;  /* This header has the timestamp (uint16) */
  uint8 num_rlf;
  uint8 num_oos;
  uint8 num_con_reject;
}tdsrrc_qsh_metric_rlf_oos_s; 


/* HO metrics */
typedef struct {
  qsh_metric_hdr_s hdr;  /* This header has the timestamp (uint16) */
  uint8 num_t_to_l_ho;
  uint8 num_t_to_g_ho;
  uint8 num_t_to_t_ho;
  uint8 num_inter_rat_ho_fail;
}tdsrrc_qsh_metric_ho_s; 

typedef struct {
  qsh_metric_hdr_s hdr;
  uint8 t313; 
  uint8 t300;
  uint8 t302;
  uint8 n313;
  uint8 n315;
  uint8 n300;
  uint8 n302;
}tdsrrc_qsh_metric_tmr_and_const_s;

typedef struct {
  qsh_metric_hdr_s hdr; /* This header has the timestamp (uint16) */
  uint32  cell_id;
  uint16  drx_cycle; /* OTA or sib configured */
  uint16  uarfcn;
  uint16  cpid;
  uint8   rrc_state;
  boolean dpch_ul_tdm_status; 
  boolean dpch_dl_tdm_status; 
}tdsrrc_qsh_metric_serving_cell_s;

typedef struct {
  qsh_metric_hdr_s hdr; /* This header has the timestamp (uint16) */
  uint8 rrc_state; 
}tdsrrc_qsh_metric_rrc_state_s;

typedef struct {
  qsh_metric_hdr_s hdr; /* This header has the timestamp (uint16) */
  uint8  msg_id;
  uint8  param0;
  uint32 param1;
  uint32 param2;
  uint32 param3;
}tdsrrc_qsh_metric_ota_s;

/* sys_rlf_weak_signal_e_type, sys_handover_failure_e_type, sys_call_end_event_e_type 
and sys_rab_status_e_type are defined in modem_proc/mmcp/api/public/sys.h */
typedef union
{
  uint8                           conn_rel_cause;
  sys_rlf_weak_signal_e_type      rlf_weak_signal_type;
  sys_handover_failure_e_type     ho_fail_type;
}tdsrrc_conn_end_cause_u_type;

/* conn end info metrics */
typedef struct{
  qsh_metric_hdr_s              hdr; /* This header has the timestamp (uint16) */
  sys_call_end_event_e_type     type;
  tdsrrc_conn_end_cause_u_type  cause;
}tdsrrc_qsh_metric_conn_end_info_s_type;

/* RAB status metrics */
typedef struct{
  qsh_metric_hdr_s        hdr; /* This header has the timestamp (uint16) */
  sys_rab_status_e_type   rab_status;
}tdsrrc_qsh_metric_multi_rab_status_s_type;

#endif

