#ifndef GL1_MSGRIF_INIT_H
#define GL1_MSGRIF_INIT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GL1 MSGR Interface Initialisation
                       Header File

GENERAL DESCRIPTION
   This header file contains declarations relevant to L1 task processing.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmsgrif/inc/gl1_msgrif_init.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
17/11/14   pg       CR729410 make DRX exit mode MSGR RSP handling non-blocking 
27/10/14   am       CR646584 WLAN coex
07/04/14   cja      CR644445 Mainline feature FEATURE_GSM_ASYNC_FW_INTF
19/03/14   cja      CR632037 Use RF Task for X2G
15/02/14   ip/npt  CR547955: Define multi thread usage on wake up from sleep
01/11/13   pa	   CR569692: X2G via RF Task. 
14/10/13   pa      CR536820: Added GL1 Msgr If Task. Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "comdef.h"
#include "msgr_types.h"
#include "queue.h"
#include "rfgsm_msg.h"


/*===========================================================================

                     DATA STRUCTURES

===========================================================================*/

#define GL1_MSGRIF_RF_TASK_MSGR_QUEUE_SIZE 13
#ifdef FEATURE_WLAN_COEX_SW_CXM
#define NUM_GERAN_GFW_ROUTER_MSGS               4
#else
#define NUM_GERAN_GFW_ROUTER_MSGS               3
#endif

typedef union
{
  msgr_hdr_struct_type                   hdr;
#if defined FEATURE_GSM_MULTI_THREAD_WAKEUP
  rfa_rf_gsm_wakeup_cnf_s                rfa_rf_gsm_wakeup_cnf;
#endif
  rfa_rf_gsm_rx_burst_cnf_s              rfa_rf_gsm_rx_burst_cnf;
  rfa_rf_gsm_set_tx_band_cnf_s           rfa_rf_gsm_tx_band_cnf;
  rfa_rf_gsm_tx_burst_cnf_s              rfa_rf_gsm_set_tx_burst_cnf;
  rfa_rf_gsm_get_timing_infor_cnf_s      rfa_rf_gsm_get_timing_infor_cnf;
  rfa_rf_gsm_idle_frame_processing_cnf_s rfa_rf_gsm_idle_frame_processing_cnf;
  rfa_rf_gsm_vreg_on_cnf_s               rfa_rf_gsm_vreg_on_cnf;
  rfa_rf_gsm_vreg_off_cnf_s              rfa_rf_gsm_vreg_off_cnf;
  rfa_rf_gsm_cm_enter_cnf_s              rfa_rf_gsm_cm_enter_cnf;
  rfa_rf_gsm_cm_exit_cnf_s               rfa_rf_gsm_cm_exit_cnf;
#ifdef GERAN_L1_IP2CAL
  rfa_rf_gsm_start_ip2_cal_cnf_s         rfa_rf_gsm_start_ip2_cal_cnf;
#endif
#ifdef GERAN_L1_HLLL_LNASTATE
  rfa_rf_gsm_process_burst_metrics_cnf_s rfa_rf_gsm_get_metrics_cnf;
#endif
} u_gl1_msgrif_rf_task_msgr;


typedef struct
{
  q_link_type           link;
  u_gl1_msgrif_rf_task_msgr  msg;

} gl1_msgrif_rf_task_msgr_t;

#define  GL1_MSGRIF_GFW_ASYNC_MSGR_QUEUE_SIZE  15

typedef union
{
  msgr_hdr_struct_type              hdr;
  GfwHostAsyncRsp                   gfwHostAsyncRsp;
  GfwHostSleepRsp                   gfwHostSleepRsp;
  GfwHostRfmExitModeRsp             gfwHostRfmExitModeRsp;
#ifdef FEATURE_WLAN_COEX_SW_CXM
  GfwWlanCxmConfigRsp               gfwWlanCxmConfigRsp;
#endif
} u_gl1_msgrif_gfw_async_msgr;

typedef struct
{
  q_link_type       link;
  u_gl1_msgrif_gfw_async_msgr  msg;
} gl1_msgrif_gfw_async_msgr_t;

/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/

void gl1_msgrif_initialise(gas_id_t gas_id);
void gl1_msgrif_task_mutex_unlock(gas_id_t gas_id);
void gl1_msgrif_task_mutex_lock(gas_id_t gas_id);

void gl1_msgrif_init_rf_task_msgr_intf(gas_id_t gas_id);
void gl1_msgrif_init_async_msgr_intf(gas_id_t gas_id);
extern void gl1_msgrif_init_teardown(gas_id_t gas_id);

#endif /* GL1_MSGRIF_INIT_H */
