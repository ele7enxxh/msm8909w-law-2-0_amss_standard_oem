/*=============================================================================
  @file sns_smgr_pm_if_uimg.c

  This file contains interface between SMGR and PM.

*******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_pm_if.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-03-30  tc   Changed wakeup interval to single precision float calculation
  2015-12-04  pn   update_total_sampling_freq() refactored from update_bw_reqd()
  2015-10-20  pn   Changed how effective report rates are estimated
  2015-09-24  pn   Code cleanup
  2015-09-04  rt   Remove SMGR vote to PM for Sleep Latency as its redundant
  2015-08-13  rt   Change SMGR island mode threshold for better power
  2015-06-29  agk  Power rail API changes
  2015-03-03  pn   Increased MIPS vote levels
  2015-03-02  pn   Fixed MIPS voting of 0
  2014-10-30  vy   Fixed a typo in a macro def
  2014-10-16  vy   Updated EWUR api to handle interrupt latency est
  2014-09-17  vy   Update sns_smgr_update_stay_in_bigimage for self-test info
  2014-09-07  vy   Updates to make MIPS/BW votes correct and meaningful
  2014-07-31  pn   Removed POWER_INTERFACE featurization
  2014-05-13  vy   Initial version , SMGR-PM interface

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_smgr_pm_if.h"
#include "sns_em.h"

#define SMGR_USEC_PER_SEC             1000000

#define SMGR_MAX_BW_REQ               100000
#define SMGR_MAX_WAKEUP_INTVL_USEC    1000000
#define SMGR_MAX_LATENCY_TOL_USEC     100000
#define SMGR_MIN_SENSOR_PER_SAMPLING  1


extern sns_smgr_pm_stats_s smgr_pm_stats;
extern sns_pm_handle_t sns_rh_get_pm_client_handle(void);


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function prototypes
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_smgr_pm_stats_init

===========================================================================*/
/*!
  @brief This function intializes smgr pm stat variables.

  @detail

  @param[i]  None
  @return   None
 */
/*=========================================================================*/
void sns_smgr_pm_stats_init
(
  void
)
{
  sns_pm_err_code_e pm_err;
  SNS_OS_MEMZERO(&smgr_pm_stats, sizeof(sns_smgr_pm_stats_s));

  pm_err = sns_pm_client_init(&smgr_pm_stats.pm_client_handle,
                              sns_smgr_app_state_change, "SMGR", SNS_PM_CLIENT_ID_SMGR);
  SNS_ASSERT(pm_err == SNS_PM_SUCCESS && smgr_pm_stats.pm_client_handle != NULL);
  /* Initialize individual fields */
  smgr_pm_stats.max_sens_per_sampling = SMGR_MIN_SENSOR_PER_SAMPLING;
  SNS_SMGR_PM_IF_DBG0(HIGH, "sns_smgr_pm_stats_init done");
}  /* sns_smgr_pm_stats_init */


/*===========================================================================

  FUNCTION:   sns_smgr_update_ewur_local

===========================================================================*/
/*!
  @brief This function increase or decreases the estimated Effective wakeup
  rate (EWUR). This is called by SMGR whenever a polling or DRI sensor
  is added or removed.
  Whenever, EWUR_est is updated, it is conveyed to the PM via a PM API.

  @detail

  @param[i]  infreq_Hz: sampling freq of added sensor
  @param[i]  out_freq_Hz: sampling freq of removed sensor
  @return   None
 */
/*=========================================================================*/
static void sns_smgr_update_ewur_local
(
  q16_t   in_freq_Hz,
  q16_t   out_freq_Hz,
  q16_t  *pm_ewur_est
)
{
  q16_t prev_eff_wakeup_rate = *pm_ewur_est;
  q16_t new_eff_wakeup_rate = prev_eff_wakeup_rate;

    /* Update EWUR */
  new_eff_wakeup_rate += (in_freq_Hz - out_freq_Hz);

  /* Handle overflow error */
  if ( (uint32)in_freq_Hz >= (uint32)out_freq_Hz )
  {
    if ( (uint32)new_eff_wakeup_rate < (uint32)prev_eff_wakeup_rate )
    {
      new_eff_wakeup_rate = prev_eff_wakeup_rate;
    }
  }
  else if ( (uint32)new_eff_wakeup_rate > (uint32)prev_eff_wakeup_rate )
  {
    new_eff_wakeup_rate = 0;
  }
  *pm_ewur_est = new_eff_wakeup_rate;

}


/*===========================================================================

  FUNCTION:   sns_smgr_update_ewur

===========================================================================*/
/*!
  @brief This function increase or decreases the estimated Effective wakeup
  rate (EWUR). This is called by SMGR whenever a polling or DRI sensor
  is added or removed.
  Whenever, EWUR_est is updated, it is conveyed to the PM via a PM API.

  @detail

  @param[i]  infreq_Hz: sampling freq of added sensor
  @param[i]  out_freq_Hz: sampling freq of removed sensor
  @param[i]  is_uimg_refac: true if caller is supported in uimg
  @param[i]  intr_based: true if rate is for interrupt
  @return   None
 */
/*=========================================================================*/
void sns_smgr_update_ewur
(
  q16_t    in_freq_Hz,
  q16_t    out_freq_Hz,
  bool     is_uimg_refac,
  bool     intr_based
)
{
  sns_pm_err_code_e smgr_pm_err;
  uint32_t          eff_wakeup_intvl_usec = 0;

  sns_smgr_update_ewur_local(in_freq_Hz, out_freq_Hz, &smgr_pm_stats.eff_wakeup_rate_hz_est);
  if ( !is_uimg_refac )
  {
    sns_smgr_update_ewur_local(in_freq_Hz, out_freq_Hz, &smgr_pm_stats.eff_wakeup_rate_hz_est_big);
  }
  if ( intr_based )
  {
    sns_smgr_update_ewur_local(in_freq_Hz, out_freq_Hz, &smgr_pm_stats.tot_int_rate);
  }

  if (smgr_pm_stats.eff_wakeup_rate_hz_est > 0)
  {
    eff_wakeup_intvl_usec = (uint32_t)(((float)SMGR_USEC_PER_SEC) / FX_FIXTOFLT_Q16_SP(smgr_pm_stats.eff_wakeup_rate_hz_est));
  }
  else
  {
    eff_wakeup_intvl_usec = SMGR_MAX_WAKEUP_INTVL_USEC;
  }
  smgr_pm_err = sns_pm_set_wakeup_interval(smgr_pm_stats.pm_client_handle,
                                           eff_wakeup_intvl_usec);
  if (smgr_pm_err != SNS_PM_SUCCESS)
  {
    SNS_SMGR_PM_IF_DBG1(HIGH, "SNS_SMGR failed to set_wakeup_interval: %u", smgr_pm_err);
  }
  else
  {
    SNS_SMGR_PM_IF_DBG3(MED, "sns_smgr_update_ewur, SMGR - in_fHz=0x%x, out_fHz=0x%x, wakeup_intvl_us=%u",
     in_freq_Hz, out_freq_Hz, eff_wakeup_intvl_usec);
    /* Vote MIPS requirement */
    sns_smgr_vote_mips_bw_reqd();
  }

}   /* sns_smgr_update_ewur */

/*===========================================================================

  FUNCTION:   sns_smgr_update_eff_rept_rate_est

===========================================================================*/
/*!
  @brief  The effective report generation rate, ERGR_est, is sum of report
  rates (q16_rpt_rate) of all reports handled by the SMGR. It is updated
  every time a report is activated or deactivated

  @detail

  @param[i]  Max Report rate
  @param[i]  Sum Report rate 
  @return   None
 */
/*=========================================================================*/
void sns_smgr_update_eff_rept_rate_est
(
  q16_t               max_rep_rate_Hz,
  q16_t               sum_rep_rate_Hz,
  sns_proc_type_e_v01 proc_type
)
{
  switch ( proc_type ) {
    case SNS_PROC_SSC_V01:
      smgr_pm_stats.eff_report_rate_est_int = max_rep_rate_Hz;
      break;
    case SNS_PROC_APPS_V01:
      smgr_pm_stats.eff_report_rate_est_aps = sum_rep_rate_Hz;
      break;
    case SNS_PROC_MODEM_V01:
      smgr_pm_stats.eff_report_rate_est_nonaps = sum_rep_rate_Hz;
      break;
    default:
      // do nothing
      break;
  }

  SNS_SMGR_PM_IF_DBG2(
    MED, "update_eff_rept_rate_est RH - max_fHz=0x%x sum_fHz=0x%x",
    max_rep_rate_Hz, sum_rep_rate_Hz);
  SNS_SMGR_PM_IF_DBG3(
    MED, "update_eff_rept_rate_est RH - rpt_rate_int=0x%x, rpt_rate_aps=0x%x, rpt_rate_nonaps=0x%x",
    smgr_pm_stats.eff_report_rate_est_int, smgr_pm_stats.eff_report_rate_est_aps,
    smgr_pm_stats.eff_report_rate_est_nonaps);

}   /* sns_smgr_update_eff_rept_rate_est */


/*===========================================================================

  FUNCTION:   sns_smgr_update_total_sampling_freq

===========================================================================*/
/*!
  @brief Updates the total sampling frequency
  @param[i]  infreq_Hz: sampling freq of added sensor
  @param[i]  out_freq_Hz: sampling freq of removed sensor
  @return    None
 */
/*=========================================================================*/
void sns_smgr_update_total_sampling_freq
(
  uint16_t            in_freq_Hz,
  uint16_t            out_freq_Hz
)
{
  uint32_t prev_rate_Hz = smgr_pm_stats.total_sampling_freq;

  smgr_pm_stats.total_sampling_freq += (in_freq_Hz - out_freq_Hz);

  /* Handle overflow situation */
  if( in_freq_Hz > out_freq_Hz )
  {
    if ( smgr_pm_stats.total_sampling_freq < prev_rate_Hz )
    {
      smgr_pm_stats.total_sampling_freq = prev_rate_Hz;
    }
  }
  else
  {
    if ( smgr_pm_stats.total_sampling_freq > prev_rate_Hz )
    {
      smgr_pm_stats.total_sampling_freq = 0;
    }
  }

  SNS_SMGR_PM_IF_DBG3(LOW, "update_total_sampling_freq total=+%u-%u=%u",
                      in_freq_Hz, out_freq_Hz, smgr_pm_stats.total_sampling_freq);
}

/*===========================================================================

  FUNCTION:   sns_smgr_update_bw_reqd

===========================================================================*/
/*!
  @brief Estimates the SMGR bandwith requirement.
 
  @details Whenever a report request is added to the SMGR or deleted from the SMGR,
  the BW requirement (in bytes per sec Bps) is updated based on total sampling rate
  and report rates.

  @param    None
  @return   None
 */
/*=========================================================================*/
void sns_smgr_update_bw_reqd
(
  void
)
{
  uint32_t         new_bw_reqd_est = 0;
  q16_t            eff_rpt_rate_q16 = 0;
  uint32_t         eff_rpt_rate = 0;
#ifdef SNS_SMGR_PM_IF_DEBUG
  uint32_t         prev_bw_reqd_est = smgr_pm_stats.bw_reqd_est;
#endif

  eff_rpt_rate_q16 = 
    smgr_pm_stats.eff_report_rate_est_aps +
    smgr_pm_stats.eff_report_rate_est_nonaps;

  eff_rpt_rate = FX_CONV(eff_rpt_rate_q16, FX_QFACTOR, 0);
  if ( eff_rpt_rate_q16 != 0 && eff_rpt_rate == 0 )
  {
    eff_rpt_rate = 1;
  }

  new_bw_reqd_est = 
    smgr_pm_stats.total_sampling_freq * sizeof(sns_smgr_sample_s) * eff_rpt_rate;
  
  smgr_pm_stats.bw_reqd_est = MIN(SMGR_MAX_BW_REQ, new_bw_reqd_est);

  SNS_SMGR_PM_IF_DBG2(LOW, "update_bw_reqd - bw(was/now)=%u/%u",
                      prev_bw_reqd_est, smgr_pm_stats.bw_reqd_est);

}   /* sns_smgr_update_bw_reqd */


/*===========================================================================

  FUNCTION:   sns_smgr_update_avgitems

===========================================================================*/
/*!
  @brief Computes average/sensor over all report items getting samples

  @detail

  @param[i]  None
  @return   None
 */
/*=========================================================================*/
void sns_smgr_update_avgitems
(
   void
)
{
  uint8_t  i, j;
  uint32_t num_sens = 0;
  uint8_t  num_items = 0;

  smgr_pm_stats.avg_items_per_sens = 0;

  /* Go through all the sensors to take avg */
  for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
  {
    sns_smgr_sensor_s *sensor_ptr = &sns_smgr.sensor[i];
    for ( j=0; j<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); j++ )
    {
      if ( sensor_ptr->ddf_sensor_ptr[j] != NULL )
      {
        num_items = sensor_ptr->ddf_sensor_ptr[j]->client_stat.num_rpt_items;
        if (num_items > 0)
        {
          num_sens++;
          smgr_pm_stats.avg_items_per_sens += num_items;
        }
      }
    }
  }
  if ( num_sens > 0 )
  {
    smgr_pm_stats.avg_items_per_sens /= num_sens;
  }

  SNS_SMGR_PM_IF_DBG1(MED, "sns_smgr_update_avgitems, SMGR - #avg items/sensors = %u",
                   smgr_pm_stats.avg_items_per_sens);

  /* Signal to vote MIPS BW requirement */
  sns_smgr_signal_me(SNS_SMGR_VOTE_MIPS_BW);

}   /* sns_smgr_update_avgitems */


/*===========================================================================

  FUNCTION:   sns_smgr_update_maxsensors

===========================================================================*/
/*!
  @brief Computes the max number of sensors sampled at any sampling instance

  @detail This is maximum number of sensors getting sampled at any sampling
  instance. It is computed by taking max of all sensors in a sensor queue of
  each schedule block in the schedule queue.
  It is updated every time a new report is activated or deactivated

  @param[i]  None
  @return   None
 */
/*=========================================================================*/
void sns_smgr_update_maxsensors
(
  void
)
{
  smgr_sched_block_s  *sched_blk_ptr;
  int queue_count = 0;

  SMGR_FOR_EACH_Q_ITEM(&sns_smgr.sched_que, sched_blk_ptr, sched_link)
  {
    if (sched_blk_ptr != NULL)
    {
      queue_count = sns_q_cnt(&sched_blk_ptr->ddf_sensor_queue);
      if ( queue_count < 0 )
      {
        SNS_SMGR_PM_IF_DBG1(HIGH, "SNS_SMGR queue count ddf sensor is below 0, queue_cnt = %d",
                            queue_count);
      }
      if( smgr_pm_stats.max_sens_per_sampling < queue_count )
      {
        smgr_pm_stats.max_sens_per_sampling = queue_count;
      }
    }
  }

  SNS_SMGR_PM_IF_DBG1(MED, "sns_smgr_update_maxsensors, Max sensors/sampling (>0) = %u",
                   smgr_pm_stats.max_sens_per_sampling);

  /* Vote MIPS BW requirement */
  sns_smgr_vote_mips_bw_reqd();

}   /* sns_smgr_update_maxsensors */


/*===========================================================================

  FUNCTION:   sns_smgr_update_stay_in_bigimage

===========================================================================*/
/*!
  @brief Updates stay in bigimage info

  @param  none

  @return none
 */
/*=========================================================================*/
void sns_smgr_update_stay_in_bigimage
(
  void
)
{
  #define SNS_SMGR_U_EXIT_HI_THRESH_Q16 FX_CONV_Q16(10,0)

  q16_t eff_exit_rate_est = smgr_pm_stats.eff_report_rate_est_nonaps;
  if ( sns_smgr.app_is_awake )
  {
    eff_exit_rate_est += smgr_pm_stats.eff_report_rate_est_aps;
    eff_exit_rate_est = MAX(eff_exit_rate_est, smgr_pm_stats.eff_report_rate_est_int);
  }
  eff_exit_rate_est += smgr_pm_stats.eff_wakeup_rate_hz_est_big;

  if ((uint32_t)eff_exit_rate_est >= (uint32_t)SNS_SMGR_U_EXIT_HI_THRESH_Q16
      || sns_smgr.self_test.self_test_req != NULL
      || smgr_pm_stats.stay_in_bigimage_ext)
  {
    smgr_pm_stats.stay_in_bigimage = true;
  }
  else
  {
    smgr_pm_stats.stay_in_bigimage = false;
  }
  SNS_SMGR_PM_IF_DBG4(
    MED, "update_stay_in_bimg - exit_rate=0x%x stay_ext=%u st=%u stay=%u", 
    eff_exit_rate_est, smgr_pm_stats.stay_in_bigimage_ext, 
    sns_smgr.self_test.self_test_req, smgr_pm_stats.stay_in_bigimage);
}


/*===========================================================================

  FUNCTION:   sns_smgr_vote_mips_bw_reqd

===========================================================================*/
/*!
  @brief Computed the estimated MIPS requirement based on the
  current sensors and request demand using the current values of EWUR_est,
  Max_sens_per_sampling, Avg_items_per_sens, and ERR_est along with the
  profiled values of SMGR_INSTR_PER_SENSOR, SMGR_INSTR_PER_ITEM,
  and SMGR_INSTR_PER_REPORT.

  @detail The total SMGR work load in terms of CPU instruction can be divided
  into three parts: 1. Data Sampling and Storage for each sensor, 2. Data
  Filtering and processing for each item with each sensor, and 3. Data Reporting
  to clients for each report request.

  @param[i]  None
  @return   None
 */
/*=========================================================================*/
void sns_smgr_vote_mips_bw_reqd
(
  void
)
{

  #define ONE_MILLION_2s_EXP            20
  #define SMGR_INSTR_PER_SENSOR_2s_EXP  17
  #define SMGR_INSTR_PER_ITEM_2s_EXP    17
  #define SMGR_INSTR_PER_REPORT         200000
  #define MIPS_LEVEL1                   40
  #define MIPS_LEVEL2                   80
  #define MIPS_LEVEL3                   120
  #define MIPS_LEVEL4                   200

#ifdef SNS_SMGR_PM_IF_DEBUG
  uint32_t          prev_mips_reqd_est_smgr = smgr_pm_stats.mips_reqd_est_smgr;
  uint32_t          prev_mips_reqd_est_rh = smgr_pm_stats.mips_reqd_est_rh;
#endif
  uint32_t          eff_rpt_rate = 0;
  uint32_t          total_bw_reqd = 0;
  sns_pm_err_code_e smgr_pm_err;

  smgr_pm_stats.eff_report_rate_est = smgr_pm_stats.eff_report_rate_est_nonaps;
  if ( sns_smgr.app_is_awake )
    smgr_pm_stats.eff_report_rate_est += smgr_pm_stats.eff_report_rate_est_aps;
  smgr_pm_stats.eff_report_rate_est += smgr_pm_stats.eff_report_rate_est_int;

  sns_smgr_update_stay_in_bigimage();

  eff_rpt_rate = FX_CONV(smgr_pm_stats.eff_report_rate_est, FX_QFACTOR, 0);
  if ( smgr_pm_stats.eff_report_rate_est != 0 && eff_rpt_rate == 0 )
    eff_rpt_rate = 1;

  SNS_SMGR_PM_IF_DBG2(MED, "eff_wake_rate=0x%x, max_sens/sampling=%u",
                   smgr_pm_stats.eff_wakeup_rate_hz_est, smgr_pm_stats.max_sens_per_sampling);
  SNS_SMGR_PM_IF_DBG2(MED, "eff_rpt_rate_total=0x%x, avg_items/sens=%u",
                   smgr_pm_stats.eff_report_rate_est, smgr_pm_stats.avg_items_per_sens);

  /* Update MIPS reqd as per updated EWUR and effective rpt rate */
  smgr_pm_stats.mips_reqd_est_rh = smgr_pm_stats.mips_reqd_est_smgr =
    (FX_CONV(smgr_pm_stats.eff_wakeup_rate_hz_est,FX_QFACTOR,0)*smgr_pm_stats.max_sens_per_sampling);
  smgr_pm_stats.mips_reqd_est_smgr <<= SMGR_INSTR_PER_SENSOR_2s_EXP;
  smgr_pm_stats.mips_reqd_est_smgr >>= ONE_MILLION_2s_EXP;

  SNS_SMGR_PM_IF_DBG2(MED, "prev mips_est_smgr=%u, new mips_est_smgr=%u",
                   prev_mips_reqd_est_smgr, smgr_pm_stats.mips_reqd_est_smgr);
  if ( smgr_pm_stats.mips_reqd_est_smgr > 0 )
  {
    if ( smgr_pm_stats.mips_reqd_est_smgr < MIPS_LEVEL1 )
    {
      smgr_pm_stats.mips_reqd_est_smgr = MIPS_LEVEL1;
    }
    else if ( smgr_pm_stats.mips_reqd_est_smgr < MIPS_LEVEL2 )
    {
      smgr_pm_stats.mips_reqd_est_smgr = MIPS_LEVEL2;
    }
    else if ( smgr_pm_stats.mips_reqd_est_smgr < MIPS_LEVEL3 )
    {
      smgr_pm_stats.mips_reqd_est_smgr = MIPS_LEVEL3;
    }
    else
    {
      smgr_pm_stats.mips_reqd_est_smgr = MIPS_LEVEL4;
    }
  }

  smgr_pm_stats.mips_reqd_est_rh *= smgr_pm_stats.avg_items_per_sens;
  smgr_pm_stats.mips_reqd_est_rh <<= SMGR_INSTR_PER_ITEM_2s_EXP;
  smgr_pm_stats.mips_reqd_est_rh += eff_rpt_rate*SMGR_INSTR_PER_REPORT;
  smgr_pm_stats.mips_reqd_est_rh >>= ONE_MILLION_2s_EXP;

  SNS_SMGR_PM_IF_DBG2(MED, "prev mips_est_rh=%u, new mips_est_rh=%u",
                   prev_mips_reqd_est_rh, smgr_pm_stats.mips_reqd_est_rh);
  if ( smgr_pm_stats.mips_reqd_est_rh > 0 )
  {
    if ( smgr_pm_stats.mips_reqd_est_rh < MIPS_LEVEL1 )
    {
      smgr_pm_stats.mips_reqd_est_rh = MIPS_LEVEL1;
    }
    else if ( smgr_pm_stats.mips_reqd_est_rh < MIPS_LEVEL2 )
    {
      smgr_pm_stats.mips_reqd_est_rh = MIPS_LEVEL2;
    }
    else if ( smgr_pm_stats.mips_reqd_est_rh < MIPS_LEVEL3 )
    {
      smgr_pm_stats.mips_reqd_est_rh = MIPS_LEVEL3;
    }
    else
    {
      smgr_pm_stats.mips_reqd_est_rh = MIPS_LEVEL4;
    }
  }

  /* SMGR Vote MIPS */
  smgr_pm_err = sns_pm_vote_mips( smgr_pm_stats.pm_client_handle, smgr_pm_stats.mips_reqd_est_smgr );
  if (smgr_pm_err != SNS_PM_SUCCESS)
  {
    SNS_SMGR_PM_IF_DBG1(HIGH, "SMGR failed to vote_mips: %u", smgr_pm_err);
  }
  else
  {
    SNS_SMGR_PM_IF_DBG1(MED, "sns_smgr_vote_mips_bw_reqd - SMGR - mips voted = %u",
                     smgr_pm_stats.mips_reqd_est_smgr);
  }

  /* RH Vote MIPS */
  smgr_pm_err = sns_pm_vote_mips( sns_rh_get_pm_client_handle(), smgr_pm_stats.mips_reqd_est_rh );
  if (smgr_pm_err != SNS_PM_SUCCESS)
  {
    SNS_SMGR_PM_IF_DBG1(HIGH, "RH failed to vote_mips: %u", smgr_pm_err);
  }
  else
  {
    SNS_SMGR_PM_IF_DBG1(MED, "sns_smgr_vote_mips_bw_reqd - RH - mips voted = %u",
                     smgr_pm_stats.mips_reqd_est_rh);
  }

  /* RH Vote BW */
  total_bw_reqd = smgr_pm_stats.bw_reqd_est;
  smgr_pm_err = sns_pm_vote_bw(sns_rh_get_pm_client_handle(), total_bw_reqd, SNS_CORE_DDR);
  if (smgr_pm_err != SNS_PM_SUCCESS)
  {
    SNS_SMGR_PM_IF_DBG1(HIGH, "SNS_SMGR failed to vote_bw: %u", smgr_pm_err);
  }
  else
  {
    SNS_SMGR_PM_IF_DBG1(MED, "sns_smgr_vote_mips_bw_reqd - RH - bw voted = %u", total_bw_reqd);
  }

}   /* sns_smgr_vote_mips_bw_reqd */

#if defined(SNS_USES_ISLAND)
/*===========================================================================

  FUNCTION:   sns_smgr_set_stay_in_big_image

===========================================================================*/
/*!
  @brief Sets or clears stay_in_big_image.
  @param [in] stay - true to set, false to clear
  @return none
 */
/*=========================================================================*/
void sns_smgr_set_stay_in_big_image(bool stay)
{
  smgr_pm_stats.stay_in_bigimage_ext = stay;
  sns_smgr_update_stay_in_bigimage();
}
#endif /* SNS_USES_ISLAND */

/*===========================================================================

 FUNCTION:   sns_smgr_pm_vote_powerrail

==========================================================================*/
/*!
  @brief Sending RMP message to change the power state of power rail

  @detail
 
 @param[in] rail:   power rail
  @param[in] state:  the state of power rail needs to be updated to.
  @return
   none
 */
/*=========================================================================*/
sns_err_code_e sns_smgr_pm_vote_powerrail(uint8_t rail, sns_pm_pwr_rail_e state)
{
  sns_pm_err_code_e pm_result;

  pm_result = sns_pm_vote_power_rail(smgr_pm_stats.pm_client_handle, rail, state);

  SNS_SMGR_PM_IF_DBG2(HIGH, "vote_powerrail - state=%u result=%u", state, pm_result);

  return (pm_result == SNS_PM_SUCCESS) ? SNS_SUCCESS : SNS_ERR_FAILED;
}

