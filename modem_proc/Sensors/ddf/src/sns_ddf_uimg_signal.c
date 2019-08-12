/**
 * Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ddf/src/sns_ddf_uimg_signal.c#1 $
  $DateTime: 2016/12/13 08:00:23 $
  $Author: mplcsds1 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-11-03 pn   Moved get_bit_position() to utility module
  2015-04-09 pn   Replaced calls to sns_em_get_timestamp() with sns_ddf_get_timestamp()
  2015-02-24 pn   Updated QDSS events
  2015-01-21 BD   Moved F3 constant string from uImage to bigImage
  2014-12-10 MW   Included sns_ddf_signal_priv.h
  2014-09-03 vy   Added QDSS logs
  2014-07-29 vy   Moved register and deregister apis to bigImage
  2014-07-29 vy   Moved sns_ddf_signal_irq_enabled to uImage
  2014-07-22 ps   Fix compiler warnings
  2014-07-21 rt   Fixed race condition by using QURT atomic ops
  2014-07-09 vy   Added support for non-uimage dd
  2014-04-23 pn   Updated signal_dispatch() to work around HW bug
  2014-01-27 ps   Added sns_ddf_signal_gpio_read
  2013-12-17 ps   Branched out from 8974 directory
  2013-11-26 pn   Prevents dispatching signals with same timestamp
  2013-10-31 hw   add QDSS SW events
  2013-08-19 pn   Clears signal bit when deregistering signal
  2013-07-12 lka  Added compile-time configurations
  2013-07-01 hw   Add latency measurement support for DRI mode
  2013-06-12 pn   Added debug logging to catch first time interrupt is triggered
  2013-06-11 ae   Added QDSP SIM playback support
  2013-04-06 ag   Remove APDS interrupt workaround
  2013-01-18 ag   
             pn   APDS workaround
  2012-11-07 ag   Removed debug printf
  2012-10-25 vh   Eliminated compiler warnings
  2012-10-19 ag   Port code to work on ADSP
  2012-09-13 vh   Eliminated compiler warnings
  2012-04-13 br   Fixed racing condition between ISR and sns_ddf_signal_dispatch()
  2012-03-05 br   change the type fo gpio_num from 32 bits to 16 bits
  2012-02-03 sd   moved DRI gpio control to DD
  2011-09-01 sd   Changed MD int detect GPIO bin to 67 to use INT2
==============================================================================*/
#include "sns_common.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_signal_priv.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_util_priv.h"
#include "sns_smgr_sensor_config.h"
#include "sns_smgr_ddf_if.h"

#if !defined(SNS_PCSIM)
SNS_DDF_UIMAGE_DATA sns_ddf_sig_tb_s sns_ddf_sig_tb;
#endif


/*===========================================================================

  FUNCTION:   sns_ddf_signal_post

===========================================================================*/
/*!
  @brief This function post the signal into the siganl database 

  @detail also this function saves the timestamp

  @param[i]  none
  @return   none
 */
/*=========================================================================*/
SNS_DDF_UIMAGE_CODE void sns_ddf_signal_post (uint32_t sig_num)
{
#if !defined(SNS_PCSIM) && !defined(SNS_QDSP_SIM)
  SNS_ASSERT(sig_num < SNS_DDF_SIGNAL_MAX_CNT);
  qurt_atomic_set_bit((unsigned int *)&sns_ddf_sig_tb.pending_flags, sig_num);
  sns_ddf_sig_tb.sig_info[sig_num].timestamp = sns_ddf_get_timestamp();
  if (!sns_ddf_sig_tb.sig_info[sig_num].is_registered)
  {
     sns_ddf_sig_tb.sig_info[sig_num].warning_cnt++; 
     /*UMSG_3(MSG_SSID_SNS,DBG_ERROR_PRIO,"sig_post - idx(%d) gpio(%d) warns(%d)",
           sig_num, sns_ddf_sig_tb.sig_info[sig_num].gpio_num, 
           sns_ddf_sig_tb.sig_info[sig_num].warning_cnt);*/
  }
  if (!sns_ddf_sig_tb.sig_info[sig_num].int_fired)
  {
    /*UMSG_4(MSG_SSID_SNS, DBG_HIGH_PRIO, "sig_post - idx(%d) gpio(%d) ts(%u/0x%x)",
           sig_num, sns_ddf_sig_tb.sig_info[sig_num].gpio_num, 
           sns_ddf_sig_tb.sig_info[sig_num].timestamp,
           sns_ddf_sig_tb.sig_info[sig_num].timestamp);*/
  }
#endif
}

/*===========================================================================

  FUNCTION:   sns_ddf_signal_dispatch

===========================================================================*/
/*!
  @brief see the description in the header file

 */
/*=========================================================================*/
SNS_DDF_UIMAGE_CODE void sns_ddf_signal_dispatch (void)
{
#if !defined(SNS_PCSIM) && !defined(SNS_QDSP_SIM)
  uint32_t flags;

  while (0 != (flags = sns_ddf_sig_tb.pending_flags))
  {
    uint32_t sig_num;
    sns_ddf_time_t timestamp;
    sig_num = get_bit_position(flags);    /* will return the bit position which is set */
    timestamp = sns_ddf_sig_tb.sig_info[sig_num].timestamp;

    sns_profiling_log_qdss(
      SNS_DDF_SIGNAL_DISPATCH_ENTER, 3, sig_num, 
      sns_ddf_sig_tb.sig_info[sig_num].gpio_num, timestamp);

    if ( sns_latency.ltcy_measure_enabled )
    {
      /* latency measurement under DRI mode */
      sns_latency.dri_get_data_start_ts = sns_em_get_system_time(); 
    }
    if (!sns_ddf_sig_tb.sig_info[sig_num].int_fired)
    {
      sns_ddf_sig_tb.sig_info[sig_num].int_fired = true;
      /*UMSG_4(MSG_SSID_SNS, DBG_HIGH_PRIO, "sig_dispatch - idx(%d) gpio(%d) ts(%u/0x%x)",
             sig_num, sns_ddf_sig_tb.sig_info[sig_num].gpio_num, timestamp, timestamp);*/
    }

    if (sns_ddf_sig_tb.sig_info[sig_num].prev_timestamp == timestamp)
    {
      /*UMSG_2(MSG_SSID_SNS, DBG_ERROR_PRIO,
             "sig_dispatch - idx(%d) repeated ts(%u)", sig_num, timestamp);*/
      continue;
    }
    sns_ddf_sig_tb.sig_info[sig_num].prev_timestamp = timestamp;

    qurt_atomic_clear_bit((unsigned int *)&sns_ddf_sig_tb.pending_flags, sig_num);
    if ( sns_ddf_sig_tb.sig_info[sig_num].is_uimg_refac == FALSE )
    {
      sns_smgr_enter_bigimage();
    }
    sns_ddf_sig_tb.sig_info[sig_num].driver->handle_irq(
          sns_ddf_sig_tb.sig_info[sig_num].handle,
          sns_ddf_sig_tb.sig_info[sig_num].gpio_num,
          timestamp);
    if ( sns_ddf_sig_tb.sig_info[sig_num].is_uimg_refac == FALSE )
    {
      sns_smgr_exit_bigimage();
    }
    sns_profiling_log_qdss(SNS_DDF_SIGNAL_DISPATCH_EXIT, 0);
  }
#elif defined(SNS_QDSP_SIM)
  sns_dd_qdsp_playback_if.handle_irq(NULL,0,0);
#endif
}

