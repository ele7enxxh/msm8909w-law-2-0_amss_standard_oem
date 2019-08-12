/*============================================================================

   @file sns_pm_unit_test.c

   @brief
   This file contains the implementation of the Sensors Power Manager
   Unit Tests

   @detail


  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/pm/src/sns_pm_unit_test.c#1 $


when         who     what, where, why
----------   ---     ---------------------------------------------------------
09-03-2014   jhh     Added client name on pm client
05-28-2014   jhh     Add support for timer
03-19-2014   jhh     Initial Version
============================================================================*/

/*=====================================================================
                               INCLUDES
=======================================================================*/
#include "stdbool.h"   /* For true or false defn */
#include "sns_init.h"
#include "sns_common.h"
#include "sns_pm.h"
#include "sns_pm_priv.h"
#include "sns_memmgr.h"
#include "sns_em.h"

#ifdef SNS_PM_UNIT_TEST
/*=====================================================================
                         INTERNAL DATA TYPES
=======================================================================*/
#define NUM_SUPPORTED_IMAGE_MODE 1
#define NUM_MAX_TEST_CASES 30
#define NUM_MAX_TEST_CLIENTS 4
#define MAX_MSG_LENGTH 32

typedef enum
{
  CLIENT_INIT,
  VOTE_PWR_RAIL,
  VOTE_MIPS,
  VOTE_BW,
  SET_MAX_DURATION,
  SET_LATENCY,
  VOTE_IMAGE_MODE,
  CLIENT_CLOSE,
  TIMER_SLEEP,
  TIMER_PEND,
  UTIMER_PEND
} sns_pm_ut_test_fn_e;

typedef struct sns_pm_ut_test_param_s {
  //for CLIENT_INIT
  suspend_cb_t callback;
  //for CLIENT_CLOSE
  //for VOTE_PWR_RAIL
  sns_pm_pwr_rail_e pwr_rail;
  //for VOTE_MIPS
  uint32 mips;
  //for VOTE_BW
  uint32 bw;
  sns_pm_core_e core;
  //for SET_MAX_DURATION
  uint32 max_duration;
  //for SET_LATENCY
  uint32 latency;
  //for VOTE_IMAGE_MODE
  sns_pm_img_mode_e image_mode;
  //for TIMER_PEND and UTIMER_PEND
  uint32 timer_ticks;
  uint32 num_timer_pend;
  //for TIMER_SLEEP
  uint32 timer_sleep_us;
} sns_pm_ut_test_param_s;

typedef struct ut_testcase_s {
  char *description;
  sns_pm_ut_test_fn_e fn_type;
  sns_pm_ut_test_param_s fn_param;
} ut_testcase_s;

typedef struct ut_pack_s {
  int num_test_cases;
  ut_testcase_s testcases[NUM_MAX_TEST_CASES];
  int num_test_clients;
  sns_pm_handle_t test_clients[NUM_MAX_TEST_CLIENTS];
  sns_em_timer_obj_t sns_pm_ut_tmr_obj;
  sns_em_timer_obj_t sns_pm_ut_utmr_obj;
  OS_FLAG_GRP *sig_group;
} ut_pack_s;

typedef struct ut_status_s {
  int test_index;
  char testcase_msg[MAX_MSG_LENGTH];
  sns_pm_internal_s status;
  sns_pm_client_info_s clients[SNS_PM_CLIENT_ID_MAX];
} ut_status_s;

/*=====================================================================
                          GLOBAL VARIABLES
=======================================================================*/
static OS_STK sns_pm_test_stk_1[SNS_MODULE_STK_SIZE_DSPS_TEST];
static OS_STK sns_pm_test_stk_2[SNS_MODULE_STK_SIZE_DSPS_TEST];
sns_pm_img_mode_e ut_image_mode[NUM_SUPPORTED_IMAGE_MODE] = { SNS_IMG_MODE_BIG,
                                                              SNS_IMG_MODE_MICRO,
                                                              SNS_IMG_MODE_UNKNOWN,
                                                              SNS_IMG_MODE_NOCLIENT };

ut_pack_s test_package_t1;
ut_pack_s test_package_t2;
ut_status_s sns_pm_ut_status[NUM_MAX_TEST_CASES*2];
int ut_status_index = 0;

extern sns_pm_internal_s sns_pm;

#define SNS_PM_T1_SIG 0x01
#define SNS_PM_T2_SIG 0x01

/*=====================================================================
                             FUNCTIONS
=======================================================================*/

/*===========================================================================

  FUNCTION:   sns_pm_ut_timer_cb_t1

===========================================================================*/
void sns_pm_ut_timer_cb_t1(void *arg)
{
  PM_MSG_1(LOW, "[T1 : %d] sns_pm_ut_timer_cb", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
  uint8_t err;

  int i;
  int dummy = 0;
  int addition = 5;
  int subtraction = 5;
  for(i = 0; i < 100; i++)
  {
    dummy = dummy + addition;
  }
  for(i = 0; i < 100; i++)
  {
    dummy = dummy - subtraction;
  }

  sns_os_sigs_post(test_package_t1.sig_group, SNS_PM_T1_SIG, OS_FLAG_SET, &err);
}
/*===========================================================================

  FUNCTION:   sns_pm_ut_timer_cb_t2

===========================================================================*/
void sns_pm_ut_timer_cb_t2(void *arg)
{
  PM_MSG_1(LOW, "[T2 : %d] sns_pm_ut_timer_cb", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
  uint8_t err;

  int i;
  int dummy = 0;
  int addition = 5;
  int subtraction = 5;
  for(i = 0; i < 100; i++)
  {
    dummy = dummy + addition;
  }
  for(i = 0; i < 100; i++)
  {
    dummy = dummy - subtraction;
  }

  sns_os_sigs_post(test_package_t2.sig_group, SNS_PM_T2_SIG, OS_FLAG_SET, &err);
}

/*===========================================================================

  FUNCTION:   sns_pm_ut_log_status

===========================================================================*/
void sns_pm_ut_log_status(char *msg)
{
  uint8 os_err;
  sns_os_mutex_pend(sns_pm.pm_mutex_ptr, 0, &os_err);
  int i;

  SNS_OS_MEMCOPY(sns_pm_ut_status[ut_status_index].testcase_msg, msg, MAX_MSG_LENGTH);

  sns_pm_ut_status[ut_status_index].test_index = ut_status_index;
  sns_pm_ut_status[ut_status_index].status.core_handle.adsppm_ddr_client_id = sns_pm.core_handle.adsppm_ddr_client_id;
  sns_pm_ut_status[ut_status_index].status.core_handle.adsppm_sram_client_id = sns_pm.core_handle.adsppm_sram_client_id;
  sns_pm_ut_status[ut_status_index].status.core_handle.pmic_client_ldo = sns_pm.core_handle.pmic_client_ldo;
  sns_pm_ut_status[ut_status_index].status.core_handle.pmic_client_lvs = sns_pm.core_handle.pmic_client_lvs;
  sns_pm_ut_status[ut_status_index].status.core_handle.req_max_duration_client = sns_pm.core_handle.req_max_duration_client;
  sns_pm_ut_status[ut_status_index].status.core_handle.req_sync_latency_client = sns_pm.core_handle.req_sync_latency_client;
  sns_pm_ut_status[ut_status_index].status.core_handle.req_usleep_client = sns_pm.core_handle.req_usleep_client;

  for(i = 0; i < SNS_PM_CLIENT_ID_MAX; i++)
  {
    sns_pm_ut_status[ut_status_index].status.status.client_handle[i] = sns_pm.status.client_handle[i];
    if(sns_pm.status.client_handle[i] != NULL)
    {
      sns_pm_ut_status[ut_status_index].clients[i].bw = sns_pm.status.client_handle[i]->bw;
      sns_pm_ut_status[ut_status_index].clients[i].callback = sns_pm.status.client_handle[i]->callback;
      sns_pm_ut_status[ut_status_index].clients[i].current_vote = sns_pm.status.client_handle[i]->current_vote;
      sns_pm_ut_status[ut_status_index].clients[i].max_tolerable_delay = sns_pm.status.client_handle[i]->max_tolerable_delay;
      sns_pm_ut_status[ut_status_index].clients[i].mips = sns_pm.status.client_handle[i]->mips;
    }
  }
  sns_pm_ut_status[ut_status_index].status.status.current_mode = sns_pm.status.current_mode;
  sns_pm_ut_status[ut_status_index].status.status.max_tolerable_delay = sns_pm.status.max_tolerable_delay;
  sns_pm_ut_status[ut_status_index].status.status.num_clients = sns_pm.status.num_clients;
  sns_pm_ut_status[ut_status_index].status.status.total_bw = sns_pm.status.total_bw;
  sns_pm_ut_status[ut_status_index].status.status.total_mips = sns_pm.status.total_mips;

  sns_pm_ut_status[ut_status_index].status.pm_mutex_ptr = sns_pm.pm_mutex_ptr;
  sns_pm_ut_status[ut_status_index].status.pm_suspend_resume_mutex_ptr = sns_pm.pm_suspend_resume_mutex_ptr;

  ut_status_index++;
  if(ut_status_index == NUM_MAX_TEST_CASES*2)
  {
    ut_status_index = 0;
  }

  sns_os_mutex_post(sns_pm.pm_mutex_ptr);
}

/*===========================================================================

  FUNCTION:   sns_pm_configure_ut_set1

===========================================================================*/
/*!
  @brief
  Configures unit tests for power manager.

  @detail

  @return
  No return code

*/
/*=========================================================================*/
void sns_pm_configure_ut_set1(void)
{
  uint8_t err;
  test_package_t1.num_test_cases = 0;
  test_package_t1.num_test_clients = 0;
  sns_em_create_timer_obj( sns_pm_ut_timer_cb_t1, NULL, SNS_EM_TIMER_TYPE_ONESHOT, &test_package_t1.sns_pm_ut_tmr_obj );
  sns_em_create_utimer_obj( sns_pm_ut_timer_cb_t1, NULL, SNS_EM_TIMER_TYPE_ONESHOT, &test_package_t1.sns_pm_ut_utmr_obj );
  test_package_t1.sig_group = sns_os_sigs_create(SNS_PM_T1_SIG, &err);

  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] CLIENT_INIT";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = CLIENT_INIT;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.callback = NULL;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_IMAGE_MODE BIG";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_IMAGE_MODE;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.image_mode = SNS_IMG_MODE_BIG;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_PWR_RAIL NPM";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_PWR_RAIL;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.pwr_rail = SNS_PWR_RAIL_ON_NPM;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_PWR_RAIL LPM";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_PWR_RAIL;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.pwr_rail = SNS_PWR_RAIL_ON_LPM;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] TIMER_PEND 100ms";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = TIMER_PEND;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.timer_ticks = (100000/30.51);
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.num_timer_pend = 1;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] UTIMER_PEND 100ms";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = UTIMER_PEND;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.timer_ticks = (100000/30.51);
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.num_timer_pend = 1;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] TIMER_SLEEP 100ms";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = TIMER_SLEEP;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.timer_sleep_us = 100000;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_MIPS 100";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_MIPS;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.mips = 100;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_MIPS 200";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_MIPS;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.mips = 200;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_MIPS 300";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_MIPS;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.mips = 300;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_MIPS 400";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_MIPS;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.mips = 400;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_BW DDR 5000000";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_BW;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.bw = 5000000;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.core = SNS_CORE_DDR;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_BW SRAM 500000";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_BW;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.bw = 500000;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.core = SNS_CORE_SRAM;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] I2C 500000";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_BW;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.bw = 500000;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.core = SNS_PNOC_BUS_I2C;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] SET_MAX_DURATION 10";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = SET_MAX_DURATION;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.max_duration = 10;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] SET_LATENCY 3";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = SET_LATENCY;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.latency = 3;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] SET_LATENCY 50000";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = SET_LATENCY;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.latency = 50000;
  test_package_t1.num_test_cases++;
/*  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_IMAGE_MODE MICRO";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_IMAGE_MODE;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.image_mode = SNS_IMG_MODE_MICRO;
  test_package_t1.num_test_cases++;
*/  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] VOTE_IMAGE_MODE NOCLIENT";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = VOTE_IMAGE_MODE;
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_param.image_mode = SNS_IMG_MODE_NOCLIENT;
  test_package_t1.num_test_cases++;
  test_package_t1.testcases[test_package_t1.num_test_cases].description = "[T1] CLIENT_CLOSE";
  test_package_t1.testcases[test_package_t1.num_test_cases].fn_type = CLIENT_CLOSE;
  test_package_t1.num_test_cases++;

} /* end of sns_pm_configure_ut_set1 */

/*===========================================================================

  FUNCTION:   sns_pm_configure_ut_set2

===========================================================================*/
/*!
  @brief
  Configures unit tests for power manager.

  @detail

  @return
  No return code

*/
/*=========================================================================*/
void sns_pm_configure_ut_set2(void)
{
  uint8_t err;
  test_package_t2.num_test_cases = 0;
  test_package_t2.num_test_clients = 0;
  sns_em_create_timer_obj( sns_pm_ut_timer_cb_t2, NULL, SNS_EM_TIMER_TYPE_ONESHOT, &test_package_t2.sns_pm_ut_tmr_obj );
  sns_em_create_utimer_obj( sns_pm_ut_timer_cb_t2, NULL, SNS_EM_TIMER_TYPE_ONESHOT, &test_package_t2.sns_pm_ut_utmr_obj );
  test_package_t2.sig_group = sns_os_sigs_create(SNS_PM_T2_SIG, &err);
/*
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] CLIENT_INIT";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = CLIENT_INIT;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.callback = NULL;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] VOTE_IMAGE_MODE BIG";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = VOTE_IMAGE_MODE;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.image_mode = SNS_IMG_MODE_BIG;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] VOTE_PWR_RAIL NPM";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = VOTE_PWR_RAIL;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.pwr_rail = SNS_PWR_RAIL_ON_NPM;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] VOTE_PWR_RAIL LPM";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = VOTE_PWR_RAIL;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.pwr_rail = SNS_PWR_RAIL_ON_LPM;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] TIMER_PEND 100ms";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = TIMER_PEND;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.timer_ticks = (100000/30.51);
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.num_timer_pend = 1;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] UTIMER_PEND 100ms";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = UTIMER_PEND;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.timer_ticks = (100000/30.51);
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.num_timer_pend = 1;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] TIMER_SLEEP 100ms";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = TIMER_SLEEP;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.timer_sleep_us = 100000;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] VOTE_MIPS 30";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = VOTE_MIPS;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.mips = 30;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] VOTE_BW DDR 1000000";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = VOTE_BW;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.bw = 1000000;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.core = SNS_CORE_DDR;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] VOTE_BW SRAM 1000000";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = VOTE_BW;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.bw = 1000000;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.core = SNS_CORE_SRAM;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] I2C 1000000";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = VOTE_BW;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.bw = 1000000;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.core = SNS_PNOC_BUS_I2C;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] SET_MAX_DURATION 50";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = SET_MAX_DURATION;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.max_duration = 50;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] SET_LATENCY 5";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = SET_LATENCY;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.latency = 5;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] SET_LATENCY 100000";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = SET_LATENCY;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.latency = 100000;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] VOTE_IMAGE_MODE MICRO";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = VOTE_IMAGE_MODE;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.image_mode = SNS_IMG_MODE_MICRO;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] SET_LATENCY 2";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = SET_LATENCY;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.latency = 2;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] SET_LATENCY 100000";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = SET_LATENCY;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.latency = 100000;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] VOTE_IMAGE_MODE NOCLIENT";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = VOTE_IMAGE_MODE;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.image_mode = SNS_IMG_MODE_NOCLIENT;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] CLIENT_CLOSE";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = CLIENT_CLOSE;
  test_package_t2.num_test_cases++;
*/

  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] TIMER_PEND 100ms";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = TIMER_PEND;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.timer_ticks = (100000/30.51);
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.num_timer_pend = 2;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] UTIMER_PEND 100ms";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = UTIMER_PEND;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.timer_ticks = (100000/30.51);
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.num_timer_pend = 2;
  test_package_t2.num_test_cases++;
  test_package_t2.testcases[test_package_t2.num_test_cases].description = "[T2] TIMER_SLEEP 100ms";
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_type = TIMER_SLEEP;
  test_package_t2.testcases[test_package_t2.num_test_cases].fn_param.timer_sleep_us = 100000;
  test_package_t2.num_test_cases++;

} /* end of sns_pm_configure_ut_set2 */

/*===========================================================================

  FUNCTION:   sns_pm_perform_ut_thread_1

===========================================================================*/
/*!
  @brief
  Performs unit tests for power manager.

  @param[i] p_arg : Pointer to arguments

  @detail

  @return
  No return code

*/
/*=========================================================================*/
void sns_pm_perform_ut_thread_1(void *p_arg)
{
  int mode;
  int test_itr;
  int client_index = 0;
  sns_pm_err_code_e rv_err;

  PM_MSG_0(LOW, "[T1] start of test thread");
  PM_MSG_0(LOW, "[T1] sns_pm_configure_ut_set1");
  sns_pm_configure_ut_set1();

  for(mode = 0; mode < NUM_SUPPORTED_IMAGE_MODE; mode++)
  {
    if(ut_image_mode[mode] == SNS_IMG_MODE_BIG)
    {
      // Make sure Sensor is in BIG MODE
      //sns_pm_vote_img_mode(ut_client_handle, SNS_IMG_MODE_BIG);
    }
    else if(ut_image_mode[mode] == SNS_IMG_MODE_MICRO)
    {
      // Make sure Sensor is in MICRO MODE
      //sns_pm_vote_img_mode(ut_client_handle, SNS_IMG_MODE_MICRO);
    }
    else if(ut_image_mode[mode] == SNS_IMG_MODE_UNKNOWN)
    {
    }
    else if(ut_image_mode[mode] == SNS_IMG_MODE_NOCLIENT)
    {
    }

    for(test_itr = 0; test_itr < test_package_t1.num_test_cases; test_itr++)
    {
      sns_em_sleep(1000);

      sns_pm_handle_t ut_client_handle;
      OS_FLAGS sig_flags = 0;
      int i = 0;
      uint8_t err;

      // Pick handle to use
      if(test_package_t1.num_test_clients != 0)
      {
        ut_client_handle = test_package_t1.test_clients[client_index];
        client_index++;

        if(client_index == test_package_t1.num_test_clients)
        {
          client_index = 0;
        }
      }
      else
      {
        ut_client_handle = NULL;
        PM_MSG_0(LOW, "[T1] no test client");
      }

      switch(test_package_t1.testcases[test_itr].fn_type)
      {
      case CLIENT_INIT :
        rv_err = sns_pm_client_init(&test_package_t1.test_clients[test_package_t1.num_test_clients], test_package_t1.testcases[test_itr].fn_param.callback, "TEST1");
        if(rv_err == SNS_PM_SUCCESS)
        {
          test_package_t1.num_test_clients++;
        }
        PM_MSG_1(LOW, "[T1] CLIENT_INIT       returned [%d]", (int)rv_err);
        break;
      case VOTE_PWR_RAIL :
        rv_err = sns_pm_vote_power_rail(ut_client_handle, test_package_t1.testcases[test_itr].fn_param.pwr_rail);
        PM_MSG_1(LOW, "[T1] VOTE_PWR_RAIL     returned [%d]", (int)rv_err);
        break;
      case VOTE_MIPS :
        rv_err = sns_pm_vote_mips(ut_client_handle, test_package_t1.testcases[test_itr].fn_param.mips);
        PM_MSG_1(LOW, "[T1] VOTE_MIPS         returned [%d]", (int)rv_err);
        break;
      case VOTE_BW :
        rv_err = sns_pm_vote_bw(ut_client_handle, test_package_t1.testcases[test_itr].fn_param.bw, test_package_t1.testcases[test_itr].fn_param.core);
        PM_MSG_1(LOW, "[T1] VOTE_BW           returned [%d]", (int)rv_err);
        break;
      case SET_MAX_DURATION :
        rv_err = sns_pm_set_wakeup_interval(ut_client_handle, test_package_t1.testcases[test_itr].fn_param.max_duration);
        PM_MSG_1(LOW, "[T1] SET_MAX_DURATION  returned [%d]", (int)rv_err);
        break;
      case SET_LATENCY :
        rv_err = sns_pm_set_latency(ut_client_handle, test_package_t1.testcases[test_itr].fn_param.latency);
        PM_MSG_1(LOW, "[T1] SET_LATENCY       returned [%d]", (int)rv_err);
        break;
      case VOTE_IMAGE_MODE :
        rv_err = sns_pm_vote_img_mode(ut_client_handle, test_package_t1.testcases[test_itr].fn_param.image_mode);
        PM_MSG_1(LOW, "[T1] VOTE_IMAGE_MODE   returned [%d]", (int)rv_err);
        break;
      case CLIENT_CLOSE :
        if(test_package_t1.num_test_clients != 0)
        {
          sns_pm_client_close(test_package_t1.test_clients[test_package_t1.num_test_clients-1]);
          test_package_t1.num_test_clients--;
          client_index = 0;
          PM_MSG_0(LOW, "[T1] CLIENT_CLOSE      done");
        }
        else
        {
          PM_MSG_0(LOW, "[T1] CLIENT_CLOSE      no client to close");
        }
        break;
      case TIMER_PEND :
        PM_MSG_1(LOW, "[T1 : %d] TIMER_PEND", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
        sns_em_register_timer(test_package_t1.sns_pm_ut_tmr_obj, test_package_t1.testcases[test_itr].fn_param.timer_ticks);
        for(i = 0; i < test_package_t1.testcases[test_itr].fn_param.num_timer_pend; i++)
        {
          sig_flags = sns_os_sigs_pend(test_package_t1.sig_group, SNS_PM_T1_SIG,
                                       OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);

          PM_MSG_1(LOW, "[T1 : %d] received ping", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));

          if(i < test_package_t1.testcases[test_itr].fn_param.num_timer_pend - 1)
          {
            sns_em_register_timer(test_package_t1.sns_pm_ut_tmr_obj, test_package_t1.testcases[test_itr].fn_param.timer_ticks);
          }
        }
        break;
      case UTIMER_PEND :
        PM_MSG_1(LOW, "[T1 : %d] UTIMER_PEND", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
        sns_em_register_utimer(test_package_t1.sns_pm_ut_utmr_obj, test_package_t1.testcases[test_itr].fn_param.timer_ticks);
        for(i = 0; i < test_package_t1.testcases[test_itr].fn_param.num_timer_pend; i++)
        {
          sig_flags = sns_os_sigs_pend(test_package_t1.sig_group, SNS_PM_T1_SIG,
                                       OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);

          PM_MSG_1(LOW, "[T1 : %d] received ping", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));

          if(i < test_package_t1.testcases[test_itr].fn_param.num_timer_pend - 1)
          {
            sns_em_register_utimer(test_package_t1.sns_pm_ut_utmr_obj, test_package_t1.testcases[test_itr].fn_param.timer_ticks);
          }
        }
        break;
      case TIMER_SLEEP :
        PM_MSG_1(LOW, "[T1 : %d] TIMER_SLEEP", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
        sns_em_sleep(test_package_t1.testcases[test_itr].fn_param.timer_sleep_us);
        PM_MSG_1(LOW, "[T1 : %d] after TIMER_SLEEP", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
        break;
      }

      //sns_pm_ut_log_status(test_package_t1.testcases[test_itr].description);
    }

  }

  PM_MSG_0(LOW, "[T1] end of test thread");

} //end of function sns_pm_perform_ut_thread_1

/*===========================================================================

  FUNCTION:   sns_pm_perform_ut_thread_2

===========================================================================*/
/*!
  @brief
  Performs unit tests for power manager.

  @param[i] p_arg : Pointer to arguments

  @detail

  @return
  No return code

*/
/*=========================================================================*/
void sns_pm_perform_ut_thread_2(void *p_arg)
{
  int mode;
  int test_itr;
  int client_index = 0;
  sns_pm_err_code_e rv_err;

  PM_MSG_0(LOW, "[T2] start of test thread");
  PM_MSG_0(LOW, "[T2] sns_pm_configure_ut_set2");
  sns_pm_configure_ut_set2();


  for(mode = 0; mode < NUM_SUPPORTED_IMAGE_MODE; mode++)
  {
    if(ut_image_mode[mode] == SNS_IMG_MODE_BIG)
    {
      // Make sure Sensor is in BIG MODE
      //sns_pm_vote_img_mode(ut_client_handle, SNS_IMG_MODE_BIG);
    }
    else if(ut_image_mode[mode] == SNS_IMG_MODE_MICRO)
    {
      // Make sure Sensor is in MICRO MODE
      //sns_pm_vote_img_mode(ut_client_handle, SNS_IMG_MODE_MICRO);
    }
    else if(ut_image_mode[mode] == SNS_IMG_MODE_UNKNOWN)
    {
    }
    else if(ut_image_mode[mode] == SNS_IMG_MODE_NOCLIENT)
    {
    }

    for(test_itr = 0; test_itr < test_package_t2.num_test_cases; test_itr++)
    {
      sns_em_sleep(1000);

      sns_pm_handle_t ut_client_handle;
      OS_FLAGS sig_flags = 0;
      int i = 0;
      uint8_t err;

      // Pick handle to use
      if(test_package_t2.num_test_clients != 0)
      {
        ut_client_handle = test_package_t2.test_clients[client_index];
        client_index++;

        if(client_index == test_package_t2.num_test_clients)
        {
          client_index = 0;
        }
      }
      else
      {
        ut_client_handle = NULL;
        PM_MSG_0(LOW, "[T2] no test client");
      }

      switch(test_package_t2.testcases[test_itr].fn_type)
      {
      case CLIENT_INIT :
        rv_err = sns_pm_client_init(&test_package_t2.test_clients[test_package_t2.num_test_clients], test_package_t2.testcases[test_itr].fn_param.callback, "TEST2");
        if(rv_err == SNS_PM_SUCCESS)
        {
          test_package_t2.num_test_clients++;
        }
        PM_MSG_1(LOW, "[T2] CLIENT_INIT       returned [%d]", (int)rv_err);
        break;
      case VOTE_PWR_RAIL :
        rv_err = sns_pm_vote_power_rail(ut_client_handle, test_package_t2.testcases[test_itr].fn_param.pwr_rail);
        PM_MSG_1(LOW, "[T2] VOTE_PWR_RAIL     returned [%d]", (int)rv_err);
        break;
      case VOTE_MIPS :
        rv_err = sns_pm_vote_mips(ut_client_handle, test_package_t2.testcases[test_itr].fn_param.mips);
        PM_MSG_1(LOW, "[T2] VOTE_MIPS         returned [%d]", (int)rv_err);
        break;
      case VOTE_BW :
        rv_err = sns_pm_vote_bw(ut_client_handle, test_package_t2.testcases[test_itr].fn_param.bw, test_package_t2.testcases[test_itr].fn_param.core);
        PM_MSG_1(LOW, "[T2] VOTE_BW           returned [%d]", (int)rv_err);
        break;
      case SET_MAX_DURATION :
        rv_err = sns_pm_set_wakeup_interval(ut_client_handle, test_package_t2.testcases[test_itr].fn_param.max_duration);
        PM_MSG_1(LOW, "[T2] SET_MAX_DURATION  returned [%d]", (int)rv_err);
        break;
      case SET_LATENCY :
        rv_err = sns_pm_set_latency(ut_client_handle, test_package_t2.testcases[test_itr].fn_param.latency);
        PM_MSG_1(LOW, "[T2] SET_LATENCY       returned [%d]", (int)rv_err);
        break;
      case VOTE_IMAGE_MODE :
        rv_err = sns_pm_vote_img_mode(ut_client_handle, test_package_t2.testcases[test_itr].fn_param.image_mode);
        PM_MSG_1(LOW, "[T2] VOTE_IMAGE_MODE   returned [%d]", (int)rv_err);
        break;
      case CLIENT_CLOSE :
        if(test_package_t2.num_test_clients != 0)
        {
          sns_pm_client_close(test_package_t2.test_clients[test_package_t2.num_test_clients-1]);
          test_package_t2.num_test_clients--;
          client_index = 0;
          PM_MSG_0(LOW, "[T2] CLIENT_CLOSE      done");
        }
        else
        {
          PM_MSG_0(LOW, "[T2] CLIENT_CLOSE      no client to close");
        }
        break;
      case TIMER_PEND :
        PM_MSG_1(LOW, "[T2 : %d] TIMER_PEND", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
        sns_em_register_timer(test_package_t2.sns_pm_ut_tmr_obj, test_package_t2.testcases[test_itr].fn_param.timer_ticks);
        for(i = 0; i < test_package_t2.testcases[test_itr].fn_param.num_timer_pend; i++)
        {
          sig_flags = sns_os_sigs_pend(test_package_t2.sig_group, SNS_PM_T2_SIG,
                                       OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);

          PM_MSG_1(LOW, "[T2 : %d] received ping", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
          if(i < test_package_t2.testcases[test_itr].fn_param.num_timer_pend - 1)
          {
            sns_em_register_timer(test_package_t2.sns_pm_ut_tmr_obj, test_package_t2.testcases[test_itr].fn_param.timer_ticks);
          }
        }
        break;
      case UTIMER_PEND :
        PM_MSG_1(LOW, "[T2 : %d] UTIMER_PEND", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
        sns_em_register_utimer(test_package_t2.sns_pm_ut_utmr_obj, test_package_t2.testcases[test_itr].fn_param.timer_ticks);
        for(i = 0; i < test_package_t2.testcases[test_itr].fn_param.num_timer_pend; i++)
        {
          sig_flags = sns_os_sigs_pend(test_package_t2.sig_group, SNS_PM_T2_SIG,
                                       OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);

          PM_MSG_1(LOW, "[T2 : %d] received ping", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));

          if(i < test_package_t2.testcases[test_itr].fn_param.num_timer_pend - 1)
          {
            sns_em_register_utimer(test_package_t2.sns_pm_ut_utmr_obj, test_package_t2.testcases[test_itr].fn_param.timer_ticks);
          }
        }
        break;
      case TIMER_SLEEP :
        PM_MSG_1(LOW, "[T2 : %d] TIMER_SLEEP", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
        sns_em_sleep(test_package_t2.testcases[test_itr].fn_param.timer_sleep_us);
        PM_MSG_1(LOW, "[T2 : %d] after TIMER_SLEEP", sns_em_convert_dspstick_to_usec(sns_em_get_timestamp()));
        break;
      }

      sns_pm_ut_log_status(test_package_t2.testcases[test_itr].description);
    }

  }

  PM_MSG_0(LOW, "[T2] end of test thread");

} /* end of sns_pm_perform_ut_thread_2 */

/*===========================================================================

  FUNCTION:   sns_pm_ut_task_init

===========================================================================*/
/*!
  @brief
  Sensors Power Manager Unit Test task initialization.

  @param[i]
  No input parameters

  @detail
  This function initializes the sensors power manager unit test
  i.e a new task for power measurement purposes is created.

  @return
  Returns sensors error code
  SNS_SUCCESS: If successful
  Error code:  In case of any errors

*/
/*=========================================================================*/
sns_err_code_e sns_pm_ut_task_init(void)
{
   //create the PM Unit Test
   sns_os_task_create_ext(sns_pm_perform_ut_thread_1,
                          NULL,
                          &sns_pm_test_stk_1[SNS_MODULE_STK_SIZE_DSPS_TEST-1],
                          SNS_MODULE_DSPS_TEST,
                          SNS_MODULE_DSPS_TEST,
                          &sns_pm_test_stk_1[0],
                          SNS_MODULE_STK_SIZE_DSPS_TEST,
                          (void *)0,
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                          (uint8_t *)"SNS_PM_TEST_T1");

/*
   sns_os_task_create_ext(sns_pm_perform_ut_thread_2,
                          NULL,
                          &sns_pm_test_stk_2[SNS_MODULE_STK_SIZE_DSPS_TEST-1],
                          SNS_MODULE_DSPS_TEST,
                          SNS_MODULE_DSPS_TEST,
                          &sns_pm_test_stk_2[0],
                          SNS_MODULE_STK_SIZE_DSPS_TEST,
                          (void *)0,
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                          (uint8_t *)"SNS_PM_TEST_T2");
*/
   sns_init_done();
   return SNS_SUCCESS;
} //end of sns_pm_ut_task_init

#endif /* SNS_PM_UNIT_TEST */
