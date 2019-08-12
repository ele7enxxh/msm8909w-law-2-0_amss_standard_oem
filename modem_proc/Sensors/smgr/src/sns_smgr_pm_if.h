#ifndef SNS_SMGR_PM_IF_H
#define SNS_SMGR_PM_IF_H
/*=============================================================================
  @file sns_smgr_pm_if.h

  This header file contains the data definitions for SMGR to interface with PM

*******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_pm_if.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-12-04  pn   update_total_sampling_freq() refactored from update_bw_reqd()
  2015-10-20  pn   Changed how effective report rates are estimated
  2015-06-29  agk  Power rail API changes
  2014-10-16  vy   Updated EWUR api to handle interrupt latency est
  2014-09-17  vy   Extern sns_smgr_update_stay_in_bigimage api
  2014-09-08  pn   Absorbs all PM interface functions from other modules
  2014-06-05  vy   Fixed a typo in a function name
  2014-05-13  vy   Initial version , SMGR-PM interface 
 
============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "fixed_point.h"
#include "sns_common.h"
#include "sns_pm.h"
#include "sns_smgr_main.h"
#include "sns_smgr_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SNS_SMGR_PM_IF_DEBUG
#ifdef SNS_SMGR_PM_IF_DEBUG
#define SNS_SMGR_PM_IF_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_PM_IF_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_PM_IF_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_PM_IF_DBG3 SNS_SMGR_PRINTF3
#define SNS_SMGR_PM_IF_DBG4 SNS_SMGR_PRINTF4
#else
#define SNS_SMGR_PM_IF_DBG0(level,msg)
#define SNS_SMGR_PM_IF_DBG1(level,msg,p1)
#define SNS_SMGR_PM_IF_DBG2(level,msg,p1,p2)
#define SNS_SMGR_PM_IF_DBG3(level,msg,p1,p2,p3)
#define SNS_SMGR_PM_IF_DBG4(level,msg,p1,p2,p3,p4)
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct 
{
   q16_t              eff_wakeup_rate_hz_est;
   q16_t              eff_wakeup_rate_hz_est_big;
   q16_t              tot_int_rate;
   uint32_t           latency_est_ticks;
   q16_t              eff_report_rate_est;
   q16_t              eff_report_rate_est_int;
   q16_t              eff_report_rate_est_aps;
   q16_t              eff_report_rate_est_nonaps;
   uint32_t           total_sampling_freq;
   uint32_t           bw_reqd_est;
   uint32_t           avg_items_per_sens;
   uint32_t           max_sens_per_sampling;
   uint32_t           mips_reqd_est_smgr;
   uint32_t           mips_reqd_est_rh;
   sns_pm_handle_t    pm_client_handle;
   sns_pm_img_mode_e  img_mode;
   bool               stay_in_bigimage;
   bool               stay_in_bigimage_ext;
} sns_smgr_pm_stats_s;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
void sns_smgr_pm_stats_init
(
  void
);

void sns_smgr_update_ewur
(
  q16_t   in_freq_Hz,
  q16_t   out_freq_Hz,
  bool    is_uimg_refac,
  bool    intr_based
);

void sns_smgr_update_bw_reqd
(
  void
);

void sns_smgr_update_total_sampling_freq
(
  uint16_t            in_freq_Hz,
  uint16_t            out_freq_Hz
);

void sns_smgr_update_avgitems
(
  void
);

void sns_smgr_update_maxsensors
(
  void
);

void sns_smgr_update_eff_rept_rate_est
(
 q16_t               max_rep_rate_Hz,
 q16_t               sum_rep_rate_Hz,
 sns_proc_type_e_v01 proc_type
);

void sns_smgr_vote_mips_bw_reqd
(
  void
);

extern void sns_smgr_vote_image_mode(sns_pm_img_mode_e new_mode);

extern sns_pm_img_mode_e sns_smgr_get_curr_image_vote(void);

extern void sns_smgr_enter_bigimage(void);

extern void sns_smgr_exit_bigimage(void);

extern void sns_smgr_update_stay_in_bigimage(void);

extern void sns_smgr_set_stay_in_big_image(bool stay);

extern sns_err_code_e sns_smgr_pm_vote_powerrail(uint8_t rail, sns_pm_pwr_rail_e state);

#endif /* #ifndef SNS_SMGR_PM_IF_H */
