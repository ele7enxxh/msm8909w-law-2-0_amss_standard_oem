#ifndef SNS_PM_PRIV_H
#define SNS_PM_PRIV_H

/*============================================================================

  @file sns_pm_priv.h

  @brief
  This file contains definitions for use by only Sensors Power Manager

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/


/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:
//source/qcom/qct/core/sensors/dsps/dsps/core/dev/pcsim/ucos-ii_pc/dsps/smr/inc/sns_pm.h#1
$


when         who     what, where, why
----------   ---     ---------------------------------------------------------
02-05-2016   sc      Use correct rail numbers on 8909w target
06-29-2015   agk     Added more PMIC clients
05-04-2015   hw      Remove usage of SMSM
03-19-2015   hw      Add sensors' usage of SMP2P api
01-28-2015   agk     Added PM internal API for MIPS voting
11-19-2014   jhh     Add minimum BW information to data structure
11-11-2014   jhh     Add percentage parameter for internal DDR BW vote API
10-10-2014   agk     Add PM internal API for setting DDR BW
09-10-2014   jhh     Move spare feature into micro image
08-24-2014   jhh     Change logging macro to use UMSG rather than MSG
08-15-2014   asj     Added tracker for last vote to debug
07-28-2014   jhh     Add support for micro image log packet
06-24-2014   jhh     Disable logging until uimg loggins is enabled
06-11-2014   ps      Fix compilation issues with VIRTIO_BRINGUP flag
03-03-2014   jhh     Initial version
============================================================================*/

/*=======================================================================
                  EXTERNAL DEFINITIONS AND TYPES
========================================================================*/
#include "sns_common.h"
#include "sns_pm.h"
#include "npa.h"
#include "smsm.h"

/*=======================================================================
                  INTERNAL DEFINITIONS AND TYPES
========================================================================*/
/* Threshold value (MIPs) of starting each mode */
#define SVS2_THRESHOLD     1
#define SVS_THRESHOLD      50
#define NOMINAL_THRESHOLD  70
#define TURBO_THRESHOLD    100

#define SMP2P_PROC_AWAKE    0x00001000

#define MAX_TOLERABLE_LATENCY (0xffff - 1)

/* Sensor power rails, MSB represents type (LVS=0, LDO=1), rest of the bits represent rail number */
/* This rail type and number should match the one in PAM table */
/* In other words, if this rail needs to change, please ensure updating PAM table to refect the appropriate rail num */

/* VDD1 and VDDIO1 are the rails used in reference platform, PAM table will already reflect this in qcom's deliverabels*/
#define SNS_PM_SENSOR_VDD1   0x91
#define SNS_PM_SENSOR_VDDIO1 0x86

/* These numbers are legacy rail numbers inherited from earlier targets */
/* We keep them now for backward compatiblity. Once we update
   rail assignment from apps registry, these can be removed */
#define SNS_PM_SENSOR_VDD1_LEGACY   0x93
#define SNS_PM_SENSOR_VDDIO1_LEGACY 0x2

/* Please remember to update PAM table appropriately with the rails defined here */
/* Setting it to a large number for now, update as required */
#define SNS_PM_SENSOR_VDD2   0xfe
#define SNS_PM_SENSOR_VDDIO2 0xfd

sns_em_timer_obj_t resume_tmr_obj;

/* Contains current status of aggregated information from all clients */
typedef struct sns_pm_instance_s
{
  uint8 num_clients;
  sns_pm_client_info_s *client_handle[SNS_PM_CLIENT_ID_MAX];
  sns_pm_img_mode_e current_mode;
  uint32 total_mips;
  uint32 total_bw;
  uint32 max_tolerable_delay; /* Tolerable latency in unit of us*/
  uint32 client_vote_status; /* Shows current status of all client */
  uint64_t     state_vote_timestamp;
  sns_pm_img_mode_e     current_vote;

  uint64_t     last_uimg_vote_ts;
  uint64_t     first_uimg_entry_ts;
  uint64_t     last_uimg_entry_ts;
  uint64_t     time_spent_in_uimg;
  uint64_t     bigImage_entry_ts;
  uint32_t     num_exits_from_uimg;

  uint8_t      min_bw;
} sns_pm_instance_s;

/* Contains all handles for external core power management */
typedef struct sns_pm_core_interface_s
{
  npa_client_handle         req_max_duration_client; /* Known as effective wake up node to clients */
  npa_client_handle         req_sync_latency_client; /* Use this for all latency voting for use cases */
  npa_client_handle         req_usleep_client;
  npa_client_handle         pmic_client_vdd1;
  npa_client_handle         pmic_client_vddio1;
  npa_client_handle         pmic_client_vdd2;
  npa_client_handle         pmic_client_vddio2;
  uint32                    adsppm_ddr_client_id;
  uint32                    adsppm_sram_client_id;
} sns_pm_core_interface_s;

typedef struct sns_pm_internal_s
{
  OS_EVENT *pm_mutex_ptr;    /* Used in all shared function */
  OS_EVENT *pm_suspend_resume_mutex_ptr; /* Used in smsm cb function */
  sns_pm_instance_s status;
  sns_pm_core_interface_s core_handle;
} sns_pm_internal_s;


/*=======================================================================
                              MACROS
========================================================================*/
/*
#if defined(VIRTIO_BRINGUP)
#define PM_MSG_0(xx_pri, xx_fmt)                            qurt_printf("\n"); qurt_printf(xx_fmt)
#define PM_MSG_1(xx_pri, xx_fmt, xx_arg1)                   qurt_printf("\n"); qurt_printf(xx_fmt, xx_arg1)
#define PM_MSG_2(xx_pri, xx_fmt, xx_arg1, xx_arg2)          qurt_printf("\n"); qurt_printf(xx_fmt, xx_arg1, xx_arg2)
#define PM_MSG_3(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3) qurt_printf("\n"); qurt_printf(xx_fmt, xx_arg1, xx_arg2, xx_arg3)
#define PM_MSG_4(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) qurt_printf("\n"); qurt_printf(xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)
#elif defined(QDSP6)
#define PM_MSG_0(xx_pri, xx_fmt)                            UMSG(MSG_SSID_SNS, DBG_##xx_pri##_PRIO, xx_fmt)
#define PM_MSG_1(xx_pri, xx_fmt, xx_arg1)                   UMSG_1(MSG_SSID_SNS, DBG_##xx_pri##_PRIO, xx_fmt, xx_arg1)
#define PM_MSG_2(xx_pri, xx_fmt, xx_arg1, xx_arg2)          UMSG_2(MSG_SSID_SNS, DBG_##xx_pri##_PRIO, xx_fmt, xx_arg1, xx_arg2)
#define PM_MSG_3(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3) UMSG_3(MSG_SSID_SNS, DBG_##xx_pri##_PRIO, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
#define PM_MSG_4(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) UMSG_4(MSG_SSID_SNS, DBG_##xx_pri##_PRIO, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)
#else
#endif
*/
#define PM_MSG_0(xx_pri, xx_fmt)
#define PM_MSG_1(xx_pri, xx_fmt, xx_arg1)
#define PM_MSG_2(xx_pri, xx_fmt, xx_arg1, xx_arg2)
#define PM_MSG_3(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
#define PM_MSG_4(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

/*=======================================================================
                         GLOBAL VARIABLES
========================================================================*/

/*=======================================================================
                          LOCAL VARIABLES
========================================================================*/

/*=======================================================================
                             FUNCTIONS
========================================================================*/

sns_err_code_e sns_pm_log_uimg_mode(void);

void sns_pm_resume_timer_cb(void *arg);

void sns_pm_internal_vote_ddr_bw ( uint32_t bw, int percentage );

void sns_pm_internal_vote_mips(uint32_t mips);

void sns_pm_smsm_cb(smsm_entry_type entry, smsm_state_type prevstate,
                    smsm_state_type currstate,  void * userData);

/*=======================================================================

  FUNCTION : sns_pm_find_client_index

========================================================================*/
/*!
  @brief
  This function finds client index from array of client_handle in sns_pm
  data structure

  @param[in] client_handle : handle of the client.

  @return
  index of client handle
  -1 if cannot be found

*/
/*======================================================================*/
int sns_pm_find_client_index( sns_pm_handle_t client_handle );

#endif /* SNS_PM_PRIV_H */
