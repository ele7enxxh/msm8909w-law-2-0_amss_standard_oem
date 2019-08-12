/*!
   @file
   rfm_gnss.c

   @brief
   This file contains all declarations and definitions necessary to use the RF
   gnss drivers.

*/

/*===========================================================================
Copyright (c) 1999 - 2002, 2006, 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2011      by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/gen8/rf/mc/src/rfm_gnss.c#1 $
$DateTime: 2016/12/13 07:59:49 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/28/13   dbz     Enable GNSS FE and PGA gain cal at bootup during online modes
05/21/13   dbz     Enabled GNSS boootup PGA gain cal
04/30/13   gy      Added API for GPS to vote rfc common to wakeup or sleep
04/09/13   dbz     Add new bootup FE and gain calibration API
02/11/13   dbz/gy  Added API to disable/enable gnss pga gain backoff
02/11/13   gy      Added gnss rfsw repartition
07/17/12   jr      Added support for non-ELNA configuration. 
01/05/12   jn      Remove support for switching ELNA on/off since always present in mainline hardware.
08/02/11   dbz     Add new API for GNSS PGA tracking loop control
03/31/11   sty     Deleted redundant update of rfm_mode[] in rfm_gnss_exit..
                   rfm_mode[] is updated at end of rfm_exit_mode()
09/03/10   hp      Update to LTE GPS B13 Gain Adjust Algorithm  
06/30/10   hp      Added Support for LTE GPS B13 Gain Adj for SVLTE-Fusion
06/25/10   gy      Added GNSS Demod Quality API 
02/26/10   dbz     Clean up RFCOM_RECEIVER_2 state when GNSS exits, fix
                   1x/HDR enter mode check
02/17/10   gy      Klocwork warning fix
01/26/10   sar     Lint fixes.
01/19/10   gy      Removed Gen7 rfm api
01/06/10   dbz     Added rfm_gnss_debug support
11/30/09   gy      Revert the Gen7 API clean up change
11/30/09   gy      Cleaned up Gen7 API
10/16/09   dbz     Added support for GNSS spur mitigation algorithms
10/16/09   hp      Fixed Compiler Warnings
10/15/09   hp      Added function needed to support ELNA API
03/10/09   gy      Code clean up
12/20/08   gy      Initial version to port gnss specific external interface
10/14/08   sr      Initial version to separate gps specific external interface.

============================================================================*/

#include "rfcom.h"
#include "rfm_types.h"
#include "rfgnss_mc.h"
#include "msg.h"
int32 ret_val = 0;
 extern rfcom_mode_enum_type rfm_mode[];
/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Configure GNSS mode operation after entering gnss mode.

   @param device : RF Multimode device
   @param gnss_mode : GNSS Mode
*/
void rfm_gnss_configure (rfcom_device_enum_type device, rfgnss_mode_cfg_enum_type rfgnss_mode)
{
  RFGNSS_MC(configure, ret_val)( device, rfgnss_mode );
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Exit gnss operation after entering gnss mode.

   @param device : RF Multimode device
*/
void rfm_gnss_exit (rfcom_device_enum_type device)
{
  RFGNSS_MC(exit, ret_val)( device );
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Resume gnss operation after entering gnss pause mode.

   @param device : RF Multimode device
*/
void rfm_gnss_resume(rfcom_device_enum_type device)
{
  RFGNSS_MC(resume, ret_val)(device);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Pause gnss operation for fts or dpo.

   @param device : RF Multimode device

   @pre
   At least one call to rfm_enter_mode() to start a GNSS session must have been
   executed prior calling this pause function
*/
void rfm_gnss_pause(rfcom_device_enum_type device)
{
  RFGNSS_MC(pause, ret_val)(device);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function is periodically called by gnss software. The RF driver will then
   query the gnss software for dc and amplitude values and update the RF as needed.

   @param device: none

*/
void rfm_gnss_trigger_bbc_update(void)
{
  RFGNSS_MC(trigger_bbc_update, ret_val)();
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This API sets up the environment based of presence or absence or an external LNA.
   It is the OEM's responsibility to properly call the API before the start of GNSS session.
   If this API is not called, then the RFSW assumes no external LNA

   @param device : GNSS_HAS_EXT_LNA

*/

void rfm_gnss_has_ext_lna (boolean gnss_has_ext_lna)
{
  RFGNSS_MC(has_ext_lna, ret_val)(gnss_has_ext_lna);
  //MSG_ERROR("RF GNSS: has_ext_lna - API no longer supported",0,0,0);

  /* rfgnss_mc_has_ext_lna (gnss_has_ext_lna) */
}


/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
  This function is used for debug purposes. The function queries
  the GPS state and returns TRUE if GPS is ON and FALSE if GPS is
  turned OFF.

   @param device : RF Multimode device

          Boolean - TRUE if GPS State is ON and FALSE if turned OFF.
*/
boolean rfm_gnss_debug(boolean verbose, rfcom_device_enum_type device)
{
  boolean ret_value = FALSE;
  RFGNSS_MC(debug, ret_value)(verbose, device);

  return ret_value;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Perform GNSS demod quality control

   @param device : demod quality control
     RFGNSS_DEMOD_QUALITY_HIGH: Error-free demod is required
     RFGNSS_DEMOD_QUALITY_NORM: Error-free demod is not required


   @pre
   At least one call to rfm_enter_mode() to start a GNSS session must have been
   executed prior calling this pause function
*/
void rfm_gnss_demod_quality_control(rfgnss_demod_quality_enum_type rfgnss_demod_quality)
{
  RFGNSS_MC(demod_quality_control, ret_val)(rfgnss_demod_quality);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS PGA tracking loop control

*/
void rfm_gnss_pga_tracking_loop_control(boolean enable)
{
  RFGNSS_MC_V(pga_tracking_loop_control)(enable);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS vote for dev on/off

*/
boolean rfm_gnss_resource_vote(rfgnss_device_status_type dev_status, rfgnss_device_vote_type dev_vote)
{
  boolean ret_value = FALSE;

  RFGNSS_MC(resource_vote, ret_value)(dev_status, dev_vote);
  return ret_value;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS device sequence execute command dispatch

*/
boolean rfm_gnss_device_seq_execute(rfm_gnss_device_seq_cmd_type cmd, void *data)
{
  boolean ret_value = FALSE;

  RFGNSS_MC(device_seq_execute, ret_value)(cmd, data);
  return ret_value;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS vote for ccs enable

*/
boolean rfm_gnss_hal_bus_enable(boolean enable)
{
  boolean ret_value = FALSE;
  RFGNSS_MC(hal_bus_enable, ret_value)(enable);
  return ret_value;
}
/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
  This function is used to enable/disable gnss pga gain backoff.

   @param Boolean - TRUE to enable and FALSE to disable.

*/
void rfm_gnss_pga_gain_backoff_control(boolean enable)
{
  RFGNSS_MC_V(pga_gain_backoff_control)(enable);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
 
   @details
   This function is used to initiate GNSS boot-up calibration from
   GNSS SW once NPA resources are available and GNSS SW can turn clocks
   ON.

*/

void rfm_gnss_bootup_cal (rfcom_device_enum_type device)
{
  RFGNSS_MC_V(bootup_cal)(device);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   API for NavRF to vote for RFC wakeup or sleep

*/
boolean rfm_gnss_rfc_common_enable(boolean enable)
{
  boolean ret_value = FALSE;
  RFGNSS_MC(rfc_common_enable, ret_value)(enable);
  return ret_value;
}
