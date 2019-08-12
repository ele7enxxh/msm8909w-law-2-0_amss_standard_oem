/*=============================================================================
  @file sns_smgr_hw.c

  This file contains hw related features used by SMGR.

*******************************************************************************
* Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_hw_uimg.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-08-19  pn   Simplified powerrails control
  2015-04-29  agk  Power rail changes
  2015-05-14  DK   Added flag SLPI_V1_CR801420_HACK for allowing v2 uImage
  2015-02-25  MW   Added big image vote before I2C QUP clock config
  2015-01-22  BD   Changed the way of putting functions in uImage to use sections
  2014-10-02  jhh  Remove pnoc on/off for every sample
  2014-09-08  ps   Support for virtio bringup
  2014-09-08  pn   Moved PM interface functions to PM_IF module
  2014-07-31  pn   Enabled power control; removed obsolete code
  2014-07-14  VY   Fixed compiler warnings
  2014-06-20  ps   Updates to turn on/off only bus instances used by Sensors
  2014-06-05  vy   Further refactored to support uImage
  2014-05-29  ps   Fixed linux compilation errors
  2014-05-29  pn   Fixed array access out-of-bound
  2014-05-09  sc   Added changes for uImage I2C driver integration
  2014-04-23  pn   Initial version
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DDITlmm.h"

#include "sns_smgr_hw.h"
#include "sns_smgr_main.h"
#include "sns_smgr_util.h"

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
SNS_SMGR_UIMAGE_DATA sns_hw_s   sns_smgr_hw;

/*===========================================================================

  FUNCTION:   sns_hw_set_qup_clk

===========================================================================*/
/*!
  @brief Enable/Disable i2c related clocks & gpio pins

  @detail

  @param[in] enable_flag:  enable/disable flag
  @return
   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_hw_set_qup_clk(bool enable_flag)
{
#if !defined(SNS_QDSP_SIM)
  uint8_t     i;
  int32 i2c_ret = I2C_RES_SUCCESS;
  I2cDrvCmdType   clock_cmd;
#ifdef SLPI_V1_CR801420_HACK
  sns_pm_img_mode_e curr_mode = sns_smgr_get_curr_image_vote();
#endif //SLPI_V1_CR801420_HACK

  if (sns_smgr_hw.i2c_clk.qupclk_on == enable_flag)
  {
    return;
  }

#ifdef SLPI_V1_CR801420_HACK
  /* For 8996 V1, no I2C sensors run in uImage. Hence return here since
     I2C code has been moved out of UImage
     TODO: For 8996 V2 remove this if I2C added back into UImage */
  sns_smgr_vote_image_mode(SNS_IMG_MODE_BIG);
#endif

  if ( enable_flag )
  {
    clock_cmd = I2cDrvCmd_Vote_I2cClocsOn;
  }
  else
  {
    clock_cmd = I2cDrvCmd_Vote_I2cClocsOff;
  }

  for ( i=0; i<sns_smgr_hw.i2c_bus_count && i2c_ret==I2C_RES_SUCCESS; i++ )
  {
    i2c_ret = I2cDrv_Open(sns_smgr_hw.i2c_bus_instances[i].i2c_bus_id,
                          &(sns_smgr_hw.i2c_bus_instances[i].config),
                          0);

    i2c_ret = ( (i2c_ret == I2C_RES_SUCCESS) &&
                 I2cDrv_Cmd ( &(sns_smgr_hw.i2c_bus_instances[i].config),
                             clock_cmd, NULL) );

    if (I2C_RES_SUCCESS == i2c_ret)
    {
      sns_smgr_hw.i2c_clk.qupclk_on = enable_flag;
    }
    else
    {
      SNS_SMGR_PRINTF3(FATAL, "set_qup_clk - bus=%d state=%d ret=%d",
                       i, enable_flag, i2c_ret);
    }
  }

#ifdef SLPI_V1_CR801420_HACK
  sns_smgr_vote_image_mode(curr_mode);
#endif

#endif /* !defined(SNS_QDSP_SIM) */
}


/*===========================================================================

  FUNCTION:   sns_hw_timer_cb

===========================================================================*/
/*!
  @brief timer call back. Entered on interrupt
  @param[i]  cb_arg  unused callback argument
  @return   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_hw_timer_cb(void *cb_arg)
{
  SNS_SMGR_PRINTF1(MED, "hw_timer_cb - rail=%u", 
                   ((sns_pm_pwr_rail_info_s*)cb_arg)->pwrrail);
  sns_smgr_signal_me(SNS_SMGR_PWR_RAIL_SIG);
}

/*===========================================================================

  FUNCTION:   sns_hw_qup_clck_status

===========================================================================*/
/*!
  @brief Returns current qup clock status
  @param  none
  @return current qup clock  status
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE bool sns_hw_qup_clck_status(void)
{
  return sns_smgr_hw.i2c_clk.qupclk_on;
}

