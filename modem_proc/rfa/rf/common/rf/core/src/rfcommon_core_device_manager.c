/*!
   @file
   rfcommon_core_device_manager.c

   @brief
 
   This file contains enum, typedefs, function prototypes internal to the
   RF Common Core Device Manager. Also it shows the implementation of the
   interface APIs.
 
   @addtogroup RF_COMMON_DEVICE_MANAGER
   @{

*/

/*==============================================================================

  Copyright (c) 2012 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_core_device_manager.c#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
10/22/14   shb     Fix compiler warnings due to enum mismatch
08/14/14   pv      Updated FBRX-GPS state machine for GPS vote
07/23/14   bm      Removed the redundant loop in rfcommon_core_detect_gnss_devices()
07/17/14   bm      Updated the state machine to reflect state changes for
                   all physical devices within given RF configuration
05/22/14   bm      Added a new API for light sleep mode support
05/21/14   bm      Updated the physical device status update logic
05/08/14   bm      Modified the device power up/down voting logic to consider 
                   multiple WWAN devices
04/14/14   bm      Changes to decouple GPS and WWAN device ON/OFF state changes 
11/26/13   nrk     Fixed compiler warnings
10/31/13   shb     Switch to C++ TRX common interface
05/06/13   pv      Triton Support for multiple transceivers for GPS.
01/15/13   pv      Support for Dime.
01/03/13   shb     Disable WAN reset temporarily to resolve CA transitions till
                   new TCXO device manager design is implemented
09/05/12   vrb     Cleanup references to deprecated items in TRM interface
06/08/12   cd      Fix for incorrect first HKADC reading in thermistor cal in CDMA mode (SVOC)
02/24/12   pv      Documentation Update. 
02/22/12   shb     Moved variable declarations before executable statements to 
                   fix offtarget compiler errors
02/16/12   pv      Fix for Klocwork warning.
01/16/12   pv      RF Common Core Device Manager Implementation. 
01/16/12   pv      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfa_variation.h"
#include "rfcommon_core_device_manager.h"
#include "rfcommon_msg.h"
#include "msg.h"
#include "timetick.h"
#include "rfc_card_gnss.h"
#include "rfcommon_fbrx_api.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*! One single copy of entire Global RF Common Core device manager */
static rfcmn_core_mgr_type rfcmn_core_mgr;

/* Internal Enums to RFCommon Core Device Manager*/

/*!
  @brief
  Enum for of Power off/wwan reset on a specific Device
*/

typedef enum
{
  /*! var that specifies WWAN reset */
  RFCMN_CORE_DEV_WWAN_RESET = 0,

  /*! var that specifies GPS Chip OFF */
  RFCMN_CORE_DEV_GPS_CHIP_OFF

}rfcmn_core_dev_power_down_type;

/* Internal to the Device Manager Function Prototypes */

boolean rfcommon_core_dev_power_down
( 
  rfm_device_enum_type
);


boolean rfcommon_core_dev_vote_validate
( 
  rfm_device_enum_type,
  rfcmn_core_dev_status_type, 
	rfcmn_core_dev_vote_type
);

/* Internal Function prototypes only */
boolean rfcommon_core_all_phy_dev_power_down(void);

void rfcommon_core_mgr_debug_msgs(void);

void rfcmn_core_dev_detect_critical_vote(void);

void rfcommon_core_detect_gnss_devices(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the interface API used to get the status of power ON/OFF vote on RF Device

  @param rfm_dev_val
  Logical RFM Device which needs the RF Device Resources.
  
  @return
  Status of vote (ON/OFF) on RF Device 

*/

boolean rfcommon_get_dev_power_voting_status
(
  rfm_device_enum_type rfm_dev_val
)
{
  if (rfcmn_core_mgr.voting_status[rfm_dev_val] == RFCMN_CORE_DEV_NO_VOTE)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*============================================================================*/
/*!
  @name RFCommon Core Device Manager Power Enable/Disable Interface

  @brief
  This section holds all Power Up/Down Interfaces to be used by All techs.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the interface API used by all techs to vote ON/OFF for RFM Logical
  Device.Also the techs need to provide the Voting type if they are
  turning ON the vote.

  @details
  This function is the interface to the RF SW technology specific driver
  for e.g. CDMA (1x and HDR), WCDMA etc to enable the power for any Logical
  RFM Device, which maps to phyisical RF Device. Once this step is done,
  RF Device[s] are available to write to/read from. This is done based on
  Device Manager which will take the responsiblity to turn ON/OFF the 
  necessary RF Devices for the Technology to work with. This is also 
  driven by Performance based requirements which help avoid HW oriented issues
  such as XO Phase shift etc. This function is used by all technologies and this
  manager manages all the votes simutaneous/sequential received from techs.
  It is also responsible to turn OFF the RF Devices which are not in use
  and are not required to be ON. The RF Common Core Device manager is
  capable to manage more than 1 RF Device simutaneously. This function will
  execute only if the Device Driver init has completed successfully.
 
  @image html rf_common_tech-wakeup_sequence.jpg
  @image html rf_common_timed_sleep_rx_or_sleep_rx_sequence.jpg
  @image html rf_common_device_manager_vote_ON_sequnce_FC.jpg
  @image html rf_common_device_manager_vote_OFF_sequence_FC.jpg
  @image html rfcommon_core_device_manager-Non_concurrency_scenario_SM.jpg
  @image html rfcommon_core_device_manager-Concurrency_scenario_SM.jpg
 
  @param rfm_dev_val
  Logical RFM Device which needs the RF Device Resources.
  
  @param dev_status
  Device Stat tell the Device Manager the RF Device status it
  needs to be after this voting request for Technology to
  use the services provided by the RF Devce.
 
  @param dev_voting_type
  The Device voting type is important to discriminate between
 
  @return
  Flag indicating whether power enable/disable was successful. This
  suggests that the intended Vote ON/OFF didn't succeed as expected,
  and any Device activity from the technology can cause Fatal operation,
  if proceeded without the success of this API execution.
*/

boolean 
rfcommon_core_dev_power_vote
(
  rfm_device_enum_type rfm_dev_val, 
  rfcmn_core_dev_status_type dev_status,
  rfcmn_core_dev_vote_type dev_voting_type
)
{
  uint8 dev_mgr_index, cntr;
  boolean rfcmn_core_dev_is_vote_valid = FALSE;
  boolean status = FALSE;

  /* Init Time */
  timetick_type dev_mgr_power_vote_time = 0;      

  RF_MSG_3(RF_MED,
           "Entering rfcmn_core_dev_power_vote()"
           "RF dev Power Vote with: rfm_dev_val: %d, "
           " dev_stat: %d,  dev_votg_type: %d!!",
           rfm_dev_val, dev_status, dev_voting_type);
 
  /* Start Profiling of Dev Power Vote Time */
  dev_mgr_power_vote_time = timetick_get();

  /* Check if RF Device Manager initialization went through */
  if (rfcmn_core_mgr.init_done == FALSE)
  {
    RF_MSG(RF_ERROR,
           "RF Device Manager not Initialized Abort!!");
    status = FALSE;
  }
  else
  { 
    if ((rfcmn_core_mgr.gps_devices_detection_done == FALSE) &&
        (rfm_dev_val == rfcmn_core_mgr.rfm_dev_for_gps))
    {
      rfcommon_core_detect_gnss_devices();
    }

    rf_common_enter_critical_section(&(rfcmn_core_mgr.lock));
    /* Check if the vote is a valid vote based on certain pre-known specs */
    rfcmn_core_dev_is_vote_valid =  
    rfcommon_core_dev_vote_validate (rfm_dev_val,dev_status,dev_voting_type);
  
    if (rfcmn_core_dev_is_vote_valid == FALSE)
    {
      RF_MSG_3(RF_ERROR,
               " RF dev Enabled/disd with INVALID VOTE Type, rfm_dev_val: %d, "
               " dev_stat: %d,  dev_votg_type: %d!!",
               rfm_dev_val,dev_status, dev_voting_type);
      status = FALSE;
    }
    else
    {
      status = TRUE;

	  /* Detect votes for only the RF Device required to operate on */
	  rfcmn_core_dev_detect_critical_vote();
	  
      switch (dev_status)
      {
      /* The Vote is to Turn on the RF Device connected to rfm_dev_val
      to use the services provided by the RF Device */
      case RFCMN_CORE_DEV_ON:
        switch (dev_voting_type)
        {
        /* Different operations needs to be performed for different
         type of voting. */
        case RFCMN_CORE_DEV_NORMAL_VOTE:
		  if (rfm_dev_val == rfcmn_core_mgr.rfm_dev_for_gps)
		  {
            for (cntr = 0; cntr < rfcmn_core_mgr.phy_dev_count; cntr++ )
            {
              if (rfcmn_core_mgr.gps_dev_mgr[cntr] != NULL)
              {
                if ((rfcmn_core_mgr.voting_status[rfm_dev_val] == RFCMN_CORE_DEV_NO_VOTE) &&
                    (rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_id != RFCMN_CORE_MAX_PHY_DEVICES))
                {
                  RF_MSG_6(RF_MED,
                           "Entering rfcmn_core_dev_power_vote() for GPS "
                           "rfm_dev: %d, Device#: %d, Dev_vote_type: %d rfdev_voting_type: %d, is_wwan_vote_on: %d, is_gps_vote_on: %d",
                           rfm_dev_val, rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_id,
                           RFCMN_CORE_DEV_ON, dev_voting_type,
                           rfcmn_core_mgr.is_wwan_vote_on,
                           rfcmn_core_mgr.is_gps_vote_on);

                  /* power up GPS from chip OFF state*/
                  if (rfcmn_core_mgr.is_wwan_vote_on == FALSE)
                  {
                    rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_status = 
                      (rfdevice_cmn_power_up_gps(rfcmn_core_mgr.gps_dev_mgr[cntr]->dev,
                       RFDEV_VOTE_ON, RFDEV_CMN_NORMAL_VOTING) == RFDEV_OFF) ?
                      RFCMN_CORE_DEV_OFF :
                      RFCMN_CORE_DEV_ON;
					
                    if (rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_status == RFCMN_CORE_DEV_OFF)
                    {
                      RF_MSG(RF_ERROR,"All Devices Fail to Power UP, Abort!!"); 
                      status = FALSE;
                    }
                  }
                  else /* enable GPS when WWAN is already ON */
                  {
                    /* do not update device status with the return value from the below function call
                    for GPS only enable, when WWAN is already ON */
                    rfdevice_cmn_enable_gps(rfcmn_core_mgr.gps_dev_mgr[cntr]->dev,
                    RFDEV_VOTE_ON,RFDEV_CMN_NORMAL_VOTING);
                  }
                }
                else
                {
                 RF_MSG_3(RF_MED, 
                          "Entering rfcmn_core_dev_power_vote()GPS based Device#: %d, "
                          "already ON, Dev Status: %d, voting_status: %d",
                          rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_id,
                          rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_status,
                          rfcmn_core_mgr.voting_status[rfm_dev_val]);
                }
              }
              else
              {
                RF_MSG_1(RF_MED, 
                         "Entering rfcmn_core_dev_power_vote()GPS based Device#: %d, "
                         "doesn't exist ",cntr); 

              }
            }		  
          }
          else
          {
            for (dev_mgr_index = 0; dev_mgr_index < rfcmn_core_mgr.phy_dev_count;
                 dev_mgr_index++ )
            {
              if ((rfcmn_core_mgr.voting_status[rfm_dev_val] == RFCMN_CORE_DEV_NO_VOTE) &&
                  (rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_id != RFCMN_CORE_MAX_PHY_DEVICES))
              {
                RF_MSG_6(RF_MED, 
                         "Entering rfcmn_core_dev_power_vote() for WWAN "
                         "rfm_dev:%d, Device#: %d, Dev_vote_type: %d, rfdev_voting_type: %d, is_wwan_vote_on: %d, is_gps_vote_on: %d",
                         rfm_dev_val,
                         rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_id,
                         RFCMN_CORE_DEV_ON, dev_voting_type,
                         rfcmn_core_mgr.is_wwan_vote_on,
                         rfcmn_core_mgr.is_gps_vote_on);

              if (rfcmn_core_mgr.is_wwan_vote_on == FALSE)
              {
                if (rfcmn_core_mgr.is_gps_vote_on == FALSE) /* enable WWAN from chip OFF state */
                {
                  rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_status = 
                  (rfdevice_cmn_power_up(rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev,
                                         RFDEV_VOTE_ON, RFDEV_CMN_NORMAL_VOTING) == RFDEV_OFF) ?
                    RFCMN_CORE_DEV_OFF:
                    RFCMN_CORE_DEV_ON;
				  
                  if (rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_status ==  RFCMN_CORE_DEV_OFF)
                  {
                    RF_MSG(RF_ERROR,"All Devices Fail to Power UP, Abort!!");  
                    status = FALSE;
                  }
                }
                else /* enable WWAN when GPS is already ON */
                {
                  /* do not update device status with the return value from the below function call
                  for WWAN enable, when GPS is already ON */
                  rfdevice_cmn_enable_wwan(rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev,
                                           RFDEV_VOTE_ON,RFDEV_CMN_NORMAL_VOTING);
                }
              }
              else
              {
                RF_MSG_2(RF_MED, 
                        "WWAN is already ON; "
                        "Device#: %d, Dev Status: %d",
                        rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_id,
			rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_status);
              }
            }
            else
            {
              RF_MSG_3(RF_MED, 
                       "Entering rfcmn_core_dev_power_vote() Device#: %d, already ON; "
                       "Dev Status: %d, voting_status: %d",
                       rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_id,
                       rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_status,
                       rfcmn_core_mgr.voting_status[rfm_dev_val]);
            }
          }
        }
          /* Update the master Voting only.
           If this has a vote present all the Devices in the system should be ON. */
          rfcmn_core_mgr.voting_status[rfm_dev_val] = dev_voting_type;
  
          break;
          /* Currently this is only used by GPS */
        case RFCMN_CORE_DEV_NON_CRITICAL_VOTE:
        if (rfm_dev_val == rfcmn_core_mgr.rfm_dev_for_gps)
        {
          for (cntr = 0; cntr < rfcmn_core_mgr.phy_dev_count; cntr++ )
          {
            if (rfcmn_core_mgr.gps_dev_mgr[cntr] != NULL)
            {
              if ((rfcmn_core_mgr.voting_status[rfm_dev_val] == RFCMN_CORE_DEV_NO_VOTE) &&
                  (rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_id != RFCMN_CORE_MAX_PHY_DEVICES))
              {
                RF_MSG_6(RF_MED,
                         "Entering rfcmn_core_dev_power_vote() for GPS "
                         "rfm_dev: %d, Device#: %d, Dev_vote_type: %d rfdev_voting_type: %d, is_wwan_vote_on: %d, is_gps_vote_on: %d",
                         rfm_dev_val, rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_id,
                         RFCMN_CORE_DEV_ON, dev_voting_type,
                         rfcmn_core_mgr.is_wwan_vote_on,
                         rfcmn_core_mgr.is_gps_vote_on);
			
                if (rfcmn_core_mgr.is_wwan_vote_on == FALSE) /* power up GPS from chip OFF state*/
                {
                  rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_status = 
                  (rfdevice_cmn_power_up_gps(rfcmn_core_mgr.gps_dev_mgr[cntr]->dev,
                                            RFDEV_VOTE_ON, RFDEV_CMN_NON_CRITICAL_VOTING) == RFDEV_OFF) ?
                    RFCMN_CORE_DEV_OFF:
                    RFCMN_CORE_DEV_ON;
				  
                  if (rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_status == RFCMN_CORE_DEV_OFF)
                  {
                    RF_MSG(RF_ERROR,"All Devices Fail to Power UP, Abort!!"); 
                    status = FALSE;
                  }
                }
                else /* enable GPS when WWAN is already ON */
                {
                  /* do not update device status with the return value from the below function call
                                for GPS only enable, when WWAN is already ON */
                  rfdevice_cmn_enable_gps(rfcmn_core_mgr.gps_dev_mgr[cntr]->dev,
                                          RFDEV_VOTE_ON,RFDEV_CMN_NON_CRITICAL_VOTING);
                }
              }
              else
              {
                RF_MSG_3(RF_MED, 
                         "Entering rfcmn_core_dev_power_vote()GPS based Device#: %d, "
                         "already ON, Dev status: %d, voting status: %d",
                         rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_id,
                         rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_status,
                         rfcmn_core_mgr.voting_status[rfm_dev_val]);
              }
            }
            else
            {
               RF_MSG_1(RF_MED, 
                         "Entering rfcmn_core_dev_power_vote()GPS based Device#: %d, "
                         "doesn't exist ",cntr); 

            }
          }
        }
        else
        {
          RF_MSG_1(RF_ERROR,"NON_CRITICAL_VOTE is not supported for "
                   "rfm dev %d", rfm_dev_val);
          status = FALSE;              
        }
  
          /* Update GPS voting Type. */
          rfcmn_core_mgr.voting_status[rfm_dev_val] = dev_voting_type;

          /* Check for Transceivers which don't need to be ON */
          status &= rfcommon_core_all_phy_dev_power_down();

          break;
        default:
          RF_MSG_3(RF_ERROR,
                   " RF dev Enabled/disd with BAD VOTING Type, rfm_dev_val: %d, "
                   " dev_stat: %d,  dev_votg_type: %d!!",
                   rfm_dev_val, dev_status, dev_voting_type);
          break;
        }
        break;
        /* To Turn off the voting for the rfm_dev_val */
      case RFCMN_CORE_DEV_OFF:
      /* turning off the chip */
	  status = rfcommon_core_dev_power_down(rfm_dev_val);
	  status &= rfcommon_core_all_phy_dev_power_down();
        
        break;

      default:
        RF_MSG_3(RF_ERROR,
                 " RF dev Enabled/disd with BAD Device VOTE, rfm_dev_val: %d,"
                 "dev_stat: %d,  dev_votg_type: %d!!",
                 rfm_dev_val, dev_status, dev_voting_type);
        break;
      } 
    }
    /* Release the lock for Device manager */
    rf_common_leave_critical_section(&rfcmn_core_mgr.lock);
  }

  if ( rfcmn_core_mgr.debug_flag == TRUE)
  {
    /* Print RF Device Manager info in full details */
    rfcommon_core_mgr_debug_msgs();
  }

  RF_MSG_1(RF_LOW,
           " Time Spent during rfcmn_core_dev_power_vote() : %d ",
           timetick_get_elapsed( dev_mgr_power_vote_time, T_USEC ));

  if (((rfm_dev_val == RFM_DEVICE_GPS) && 
      (dev_status == RFCMN_CORE_DEV_VOTE_ON))&&
      ((dev_voting_type == RFCMN_CORE_DEV_CRITICAL_VOTE)|| 
      (dev_voting_type == RFCMN_CORE_DEV_NON_CRITICAL_VOTE)))
  {
    rfcommon_fbrx_mc_set_gps_state(RFLM_GPS_ACTIVE); 
    RF_MSG(RF_HIGH," FBRx GPS ACTIVE triggered ");
  }
  else if ((rfm_dev_val == RFM_DEVICE_GPS) && 
           ((dev_status == RFCMN_CORE_DEV_VOTE_OFF)||
           (dev_voting_type == RFCMN_CORE_DEV_NO_VOTE))) 
  {
    rfcommon_fbrx_mc_set_gps_state(RFLM_GPS_INACTIVE); 
    RF_MSG(RF_HIGH," FBRx GPS DeACTIVE triggered ");
  }

  return status;
} /* rfcommon_core_dev_power_vote() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the interface API used by all techs to vote light sleep for RFM 
  Logical Device.

  @details
  This function is the interface to the RF SW technology specific driver
  for e.g. LTE etc to put the physical RF device corresponding to any Logical
  RFM Device in a light sleep/partial shutdown mode. This function is used
  by RF techs when they dont want to turn OFF the physical RF deivice 
  completely while going to sleep mode. 
  Assumption is this API will be invoked only in the scenarios of
  one WWAN device and one GPS device are active at any time.
  This API should not be invoked in the scenarios with multiple
  WWAN devices.  This function will execute only if the Device Driver init 
  has completed successfully.
  
  @return
  Flag indicating whether light sleep is successful. FALSE
  suggests that the intended light sleep didn't succeed as expected.
*/

boolean 
rfcommon_core_dev_light_sleep()
{
  uint8 dev_mgr_index = 0;
  boolean status = FALSE;

  /* Init Time */
  timetick_type dev_mgr_light_sleep_time = 0; 

  /* Start Profiling of light sleep Time */
  dev_mgr_light_sleep_time = timetick_get();
  
  /* Detect all the current WWAN and GPS votes */
  rfcmn_core_dev_detect_critical_vote();

  /* Release the lock for Device manager */
  rf_common_enter_critical_section(&(rfcmn_core_mgr.lock));

  if (rfcmn_core_mgr.is_gps_vote_on == FALSE) 
  {
    for (dev_mgr_index = 0; dev_mgr_index < rfcmn_core_mgr.phy_dev_count;
         dev_mgr_index++ )
    {
      status = rfdevice_cmn_light_sleep (
               rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev,
               RFDEVICE_EXECUTE_IMMEDIATE, NULL);
    }
  }
  else
  {
    RF_MSG(RF_MED, 
    "rfcommon_core_dev_light_sleep: GPS is ON. Light sleep call ignored");
  }
  
  /* Release the lock for Device manager */
  rf_common_leave_critical_section(&rfcmn_core_mgr.lock);

  RF_MSG_1(RF_LOW,
           " Time Spent during rfcmn_core_dev_light_sleep() : %d ",
           timetick_get_elapsed( dev_mgr_light_sleep_time, T_USEC ));

  return status;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the interface API to Initialize the RF Common Core Device Manager from
  RF Common Core Init.

  @details
  This function is the interface to the RF SW driver. This function should
  only be called ONCE.This API defines the global device manager structure.
  Along with that, RF Device Manager specific structures are also initialized.
  All the default values are assigned to these structures and based on RF Card,
  different possible numbers of RF Device manager[s] could be created. This function
  also defines the critical section for Device manager to be used for any future actions
  on global structures. This function will execute only if the RF Card init has
  completed successfully.
 
  @return
  Flag indicating whether init was successful. This suggests that the intended
  Initialization of the RF Common Core Device Manager didn't succeed as expected,
  and any Device activity from the technology can cause Fatal operation,
  if proceeded without the success of this API execution.
 
  @image html rf_common_online_bootup_sequence.jpg
  @image html rf_common_device_manager_initialization_FC.jpg
 
*/

boolean rfcommon_core_dev_mgr_init()
{
  boolean status = TRUE; 
  uint8 dev_id;
  rfdevice_rxtx_common_class* temp_cmn_dev_param = NULL;

  /* Init Time */
  timetick_type dev_mgr_init_time = 0;      

  /* Entering in Dev Mgr Initialization */
  RF_MSG(RF_MED," Entering rfcmn_core_dev_mgr_init()");

  /* Start Profiling of Dev MGR Init */
  dev_mgr_init_time = timetick_get();

  if ( rfcmn_core_mgr.init_done == TRUE)
  {
    RF_MSG(RF_ERROR,
           " RF dev rfcmn_core_dev_mgr_init Already Done!!");
    return FALSE;
  }

  /* By Default the Debug flag is OFF*/
  rfcmn_core_mgr.debug_flag = FALSE;

  /* Count for Device manager[s] created to control RF Devices.*/
  rfcmn_core_mgr.phy_dev_count = 0;

  /* One time RF Device manager Init Done? */
  rfcmn_core_mgr.init_done = FALSE;

  /* Static initialization of RFM device mapped logically for GPS */
  rfcmn_core_mgr.rfm_dev_for_gps = RFM_DEVICE_GPS;

  /* The below Algorithm is to look for most unique RF Devices associated
   with the RF Card. A RFCMN_CORE_MAX_PHY_DEVICES is a macro defined in
   rfc_class.h. if there is a mis-match between the macro and physical
   devices connected in RFC, Below Device Manager will crash.
   */

  for (dev_id = 0; dev_id < RFCMN_CORE_MAX_PHY_DEVICES; dev_id++)
  {
    /* Default Initialize to Max Device ID */
    rfcmn_core_mgr.dev_mgr[dev_id].dev_id = RFCMN_CORE_MAX_PHY_DEVICES;
    
    /* Base class Get_Instance will always return NULL if not instantiated */
    temp_cmn_dev_param  = rfc_common_get_cmn_device_param(RFDEVICE_TRANSCEIVER, 
                                                          dev_id );
    /* Check if RFC instantiation is completed successfully,
    This check is to make sure the logical to physical device mapping
    is completed before accessing them */

    if (temp_cmn_dev_param != NULL)
    {
      /* First instance of RF Device Manager is default, as atleast one RF
       Device will be present on the RF Card, Find a unique RF Device 
       associated in the RF Card and create a Device Manager for 
       Physical Device */
      if (rfcmn_core_mgr.phy_dev_count >(RFCMN_CORE_MAX_PHY_DEVICES-1) )
        {
          RF_MSG_1(RF_FATAL,
                   "RF Device Manager crossed MAX RF Device/s Limit: %d Abort!!"
                 ,RFCMN_CORE_MAX_PHY_DEVICES); 
          status = FALSE;
          break;
        }
        else
        {
          /* Initialize all the default parameters for the structure. */
          rfcmn_core_mgr.dev_mgr[rfcmn_core_mgr.phy_dev_count].dev 
          = temp_cmn_dev_param;
          rfcmn_core_mgr.dev_mgr[rfcmn_core_mgr.phy_dev_count].dev_id 
          = rfcmn_core_mgr.phy_dev_count;
          rfcmn_core_mgr.dev_mgr[rfcmn_core_mgr.phy_dev_count].dev_status 
          = RFCMN_CORE_DEV_OFF;
  
          /* Initialize all the Boolean flags to FALSE */
          rfcmn_core_mgr.phy_dev_count++;
        }

    } /* temp_log_dev_param !=NULL && temp_log_dev_param->cmn_device !=NULL */
  } /* for (rfm_dev_val = RFM_DEVICE_0; rfm_dev_val < RFM_MAX_DEVICES; rfm_dev_val++)  */

  if ((rfcmn_core_mgr.phy_dev_count > 0) && (status == TRUE))
  {
    /* Define the Device Manager critical section only once */
    rf_common_init_critical_section(&rfcmn_core_mgr.lock);
    rfcmn_core_mgr.init_done = TRUE;
    if ( rfcmn_core_mgr.debug_flag == TRUE)
    {
      /* Print RF Device Manager info in full details */
      rfcommon_core_mgr_debug_msgs();
    }
  }

  RF_MSG_1(RF_LOW,
           " Time Spent during RF Device Mgr Init: %d ",
           timetick_get_elapsed( dev_mgr_init_time, T_USEC ));

  return status;
} /* rfcmn_core_dev_mgr_init()*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*! @} */

/*----------------------------------------------------------------------------*/
/*!
  @name RFCommon Core Device Manager Internal Functions

  @brief
  This section holds all internal functions used within RF Common Core Device Manager.
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is an internal RF Common Core Device Manager function to validate multiple
  votes on any RF Device .

  @details
  This function updates the RF Common core Device Manager structure with the
  particular flags which provide critical information on what is the state of the
  votes on a particular RF Device. This function cannot be called from outside of
  RF Common Core Device Manager APIs.
 
  @param temp_rfcmn_core_dev_mgr
  Its takes a pointer to the Device Manager structure and based on local and
  global voting scheme, it updates the boolean flags in the device Manager structure.
 
  @return
   void

*/

void rfcmn_core_dev_detect_critical_vote()
{
  uint8 wwan_vote_counter = 0, gps_vote_counter = 0;
  rfm_device_enum_type temp_rfm_dev_val = RFM_INVALID_DEVICE;

  /* Elapse Time */
  timetick_type dev_mgr_time = 0;      

  /* Start Profiling of Dev MGR Fns */
  dev_mgr_time = timetick_get();

  RF_MSG(RF_MED, 
         "Entering rfcmn_core_dev_detect_critical_vote()");

  /* Do the Voting on entire rfcmn dev manager */
  for (temp_rfm_dev_val = RFM_DEVICE_0; temp_rfm_dev_val < RFM_MAX_DEVICES; 
      ++temp_rfm_dev_val)
  {
    if ((rfcmn_core_mgr.voting_status[temp_rfm_dev_val] 
        == RFCMN_CORE_DEV_NORMAL_VOTE) || 
        (rfcmn_core_mgr.voting_status[temp_rfm_dev_val] 
        == RFCMN_CORE_DEV_NON_CRITICAL_VOTE))
    {
      if (temp_rfm_dev_val != rfcmn_core_mgr.rfm_dev_for_gps)
      {
        ++wwan_vote_counter;
      }
      else if (temp_rfm_dev_val == rfcmn_core_mgr.rfm_dev_for_gps)
      {
        ++gps_vote_counter;
      }
    }
  }
  /* Any vote is a critical vote hence, need to account all  WWAN votes */
  if ( wwan_vote_counter > 0)
  {
    rfcmn_core_mgr.is_wwan_vote_on  = TRUE;
  }
  else
  {
    rfcmn_core_mgr.is_wwan_vote_on  = FALSE;
  } 

  /* Need to account all GPS votes */
  if ( gps_vote_counter > 0)
  {
    rfcmn_core_mgr.is_gps_vote_on  = TRUE;
  }
  else
  {
    rfcmn_core_mgr.is_gps_vote_on  = FALSE;
  } 
  
  RF_MSG_1(RF_LOW, 
           " Time Spent during rfcmn_core_dev_detect_critical_vote() : %d ",
           timetick_get_elapsed( dev_mgr_time, T_USEC ));

} /* rfcmn_core_dev_detect_multiple_votes() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
   This is an internal RF Common Core Device Manager function to Power Down/reset
   the device as its currently, voted OFF by a tech. 

  @details
  This function should only be used within RF Common Core Device Manager, 
  NO techs can call this API explicitly. This API defines the Power 
  down/GNSS Seq Chip OFF/WWAN Reset module. It also check the state of the
  RF Device which is currently being voted OFF.
 
  @param rfm_dev_val
  Logical RFM Device which is being used by GPS.
 
  @param dev_voting_type
  Currently Device Voting type is not used, but is kept for possible
  future use case.
 
  @return
  Flag indicating whether the operation was successful or not.
*/

boolean
rfcommon_core_dev_power_down
( 
  rfm_device_enum_type rfm_dev_val
)

{
  uint8 dev_mgr_index = 0,cntr;
  boolean status_of_opr = TRUE;

  /* Elapse Time */
  timetick_type dev_mgr_time = 0;      

  /* Start Profiling of Dev MGR Init */
  dev_mgr_time = timetick_get();

  RF_MSG_1(RF_MED,
           "Entering rfcmn_core_dev_power_down() with: rfm_dev_val: %d, "
           "dev_votg_type: %d!!",rfm_dev_val);

  /* Perform the power down vote for the RF Device which is
   tied to the logical device. This operation only involved the
   tech specific power down/reset based on the the current
   voting on this device.
  */
  if (rfm_dev_val == rfcmn_core_mgr.rfm_dev_for_gps)
  {
    /* Perform GNSS Chip OFF Sequence if voted for GNSS */
    for (cntr = 0; cntr < rfcmn_core_mgr.phy_dev_count; cntr++ )
    {
      if (rfcmn_core_mgr.gps_dev_mgr[cntr] != NULL)
      {
        RF_MSG_2(RF_MED, 
                 "Entering rfdevice_cmn_power_reset() for GPS, "
                 "Device#: %d, rfdev_voting_type: %d",
                 rfcmn_core_mgr.gps_dev_mgr[cntr]->dev,
                 RFCMN_CORE_DEV_GPS_CHIP_OFF );

	/* do not update device status with the return value from the below function call
        for GPS only disable, when WWAN is still ON */
	rfdevice_cmn_enable_gps(rfcmn_core_mgr.gps_dev_mgr[cntr]->dev,
                                RFDEV_VOTE_OFF, RFDEV_CMN_NO_VOTING);
	
    status_of_opr &= 
        rfdevice_cmn_power_reset(rfcmn_core_mgr.gps_dev_mgr[cntr]->dev,
                                 RFDEV_CMN_GPS_CHIP_OFF);
      }
      else
      {
        RF_MSG_1(RF_MED,"Entering rfcmn_core_dev_power_vote()GPS based Device#: %d, "
                        "doesn't exist ", cntr);
      }
    }

    /* The Reset/Power OFF shouldn't respond with a FALSE, if so the reset/chip OFF
       didn't happen successfully. */
    if (status_of_opr == FALSE)
    {
      RF_MSG(RF_ERROR,
             "RF Device Fail to perform GNSS CHIP Seq OFF for Cmn Device, Abort!!"); 
    }
  }

  /* Need to register the vote turning off for overall Device[s] Manager */
  rfcmn_core_mgr.voting_status[rfm_dev_val] = RFCMN_CORE_DEV_NO_VOTE;

  /* Detect votes for only the RF Device required to operate on */
  rfcmn_core_dev_detect_critical_vote();

  if (rfcmn_core_mgr.is_wwan_vote_on == FALSE)
  {
    /* Perform WWAN Reset on all devices if voted for WWAN tech OFF */
    for (dev_mgr_index = 0; dev_mgr_index < rfcmn_core_mgr.phy_dev_count;
            dev_mgr_index++ )
  {
      if (rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_status == RFCMN_CORE_DEV_ON)
    {
        RF_MSG_1(RF_MED, 
                 "Entering rfdevice_cmn_power_reset() "
                 "Device#: %d WWAN Reset called",
                 rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_id);

		/* do not update device status with the return value from the below function call
              for WWAN only disable, when GPS is still ON */
      rfdevice_cmn_enable_wwan(rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev,
                               RFDEV_VOTE_OFF, RFDEV_CMN_NO_VOTING);

        status_of_opr &= rfdevice_cmn_power_reset(rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev,
                                                  RFDEV_CMN_WWAN_RESET);

        if (status_of_opr == FALSE )
        {
          RF_MSG(RF_ERROR,"All Devices Fail to WWAN Reset, Abort!!");  
        }
      }
      else
      {
        RF_MSG_1(RF_MED, 
                 "Entering rfcmn_core_dev_power_vote()Device#: %d, already OFF, "
                 "WWAN Reset not needed ",
                 rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_id);
      }
    }
  }

  RF_MSG_1(RF_LOW, 
           " Time Spent during rfcmn_core_dev_power_down() : %d ",
           timetick_get_elapsed( dev_mgr_time, T_USEC ));

  return status_of_opr;

} /* rfcommon_core_dev_power_down () */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RFCommon Core Device Manager All Physical RF Devices Power down.

  @details
  This function is internal function to the RF Common Core Device Manager.
  This function should only be used within RF Common Core Device Manager, 
  NO techs can call this API explicitly. This API defines the mechanism 
  to check and voting shutdown for all RF devices which are not being used. 

  @return
  Flag indicating whether the operation was successful or not.
*/

boolean rfcommon_core_all_phy_dev_power_down()
{

  uint8 dev_index = 0, cntr;
  boolean return_status = TRUE, transceiver_turn_off = FALSE;

  /* Elapse Time */
  timetick_type dev_mgr_time = 0;      


  /* Start Profiling of Dev MGR Fns */
  dev_mgr_time = timetick_get();

  if (rfcmn_core_mgr.debug_flag == TRUE)
  {
    RF_MSG(RF_MED," Entering rfcmn_core_dev_check_all_phy_dev_power_down()!");
  }

  /* Detect votes for only the RF Device required to operate on */
  rfcmn_core_dev_detect_critical_vote();

  /* Turn OFF all transceivers expect the 1 used by GPS Non-critical demod */
  if ((rfcmn_core_mgr.is_wwan_vote_on == FALSE) &&
      ((rfcmn_core_mgr.is_gps_vote_on == FALSE) ||
        (rfcmn_core_mgr.voting_status[rfcmn_core_mgr.rfm_dev_for_gps] == 
       RFCMN_CORE_DEV_NON_CRITICAL_VOTE)))
  {
    for (dev_index = 0; dev_index < rfcmn_core_mgr.phy_dev_count; 
          ++dev_index)
    {
      /* Set the default status for a transceiver to be TRUE */
      transceiver_turn_off = TRUE;

      /* Search for all the transceivers in the system and compare for valid status */
      for (cntr = 0; cntr < rfcmn_core_mgr.phy_dev_count; ++cntr)
      {
        if (rfcmn_core_mgr.gps_dev_mgr[cntr] != NULL)
        {
          /* Check the below:
          1) If the transceiver is ON & 
          2) If no valid GPS vote present or 
          3) If this is a Non GPS transceiver */
          if ((rfcmn_core_mgr.dev_mgr[dev_index].dev_status == RFCMN_CORE_DEV_ON) && 
              (rfcmn_core_mgr.is_gps_vote_on == FALSE))
          {
            transceiver_turn_off = TRUE;
            break;
          }
          else if (rfcmn_core_mgr.is_gps_vote_on == TRUE)
          {
            if (rfcmn_core_mgr.gps_dev_mgr[cntr]->dev_id == rfcmn_core_mgr.dev_mgr[dev_index].dev_id)
            {
              transceiver_turn_off = FALSE;
              break;
            }
            else
            {
              transceiver_turn_off = TRUE;
            }
          }
        }
      }
      if (transceiver_turn_off == TRUE)
      {
        RF_MSG_3(RF_MED, 
                 "Entering rfcommon_core_all_phy_dev_power_down() Voting Down "
                 "Device#: %d, Dev_vote_type: %d rfdev_voting_type: %d",
                 rfcmn_core_mgr.dev_mgr[dev_index].dev_id,
                 RFCMN_CORE_DEV_OFF, RFCMN_CORE_DEV_NO_VOTE );
        rfcmn_core_mgr.dev_mgr[dev_index].dev_status = 
        (rfdevice_cmn_power_up(rfcmn_core_mgr.dev_mgr[dev_index].dev,
                              RFDEV_VOTE_OFF,RFDEV_CMN_NO_VOTING) == RFDEV_OFF) ?
          RFCMN_CORE_DEV_OFF:
          RFCMN_CORE_DEV_ON;

        if (rfcmn_core_mgr.dev_mgr[dev_index].dev_status 
            != RFCMN_CORE_DEV_OFF)
        {
          RF_MSG(RF_ERROR,"RF Device Fail to Power Down, Abort!!"); 
          return_status = FALSE;
        }
      }
    }
  }

  RF_MSG_1(RF_LOW,
           " Time Spent during rfcmn_core_dev_check_all_phy_dev_power_down() : %d ",
           timetick_get_elapsed( dev_mgr_time, T_USEC ));

  return return_status;

} /* rfcommon_core_all_phy_dev_power_down() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is an internal function for RF Common Core Device Manager. This function
  validates the vote which is sent from a tech to make sure the vote is currently
  valid to be executed.

  @details
  This function is internal function to the RF Common Core Device Manager.
  This function should only be used within RF Common Core Device Manager, 
  NO techs can call this API explicitly. This function defines the mechanism to
  check the current vote request for a specific Logical RFM device against the
  state of the Global and Local device manager for that RFM Device.
 
  @param rfm_dev_val
  Logical RFM Device which is being voted for.
 
  @param dev_status
  The Vote Status whether its ON/OFF.
 
  @param dev_voting_type
  The voting type has to match based on the requirements doc. GPS is allowed
  to vote for RFCMN_CORE_DEV_CRITICAL_VOTE or RFCMN_CORE_DEV_NON_CRITICAL_VOTE
  or RFCMN_CORE_DEV_NO_VOTE. All other techs can only use RFCMN_CORE_DEV_NORMAL_VOTE
  or RFCMN_CORE_DEV_NO_VOTE.
 
  @return
  Flag indicating whether the vote is valid or not on that RFM Logical Device.
*/

boolean rfcommon_core_dev_vote_validate 
(
  rfm_device_enum_type rfm_dev_val,
  rfcmn_core_dev_status_type dev_status, 
  rfcmn_core_dev_vote_type dev_voting_type
)
{
  boolean vote_validity_status = FALSE;

  /* Elapse Time */
  timetick_type dev_mgr_time = 0;      

  /* Start Profiling of Dev MGR Fns */
  dev_mgr_time = timetick_get();

  RF_MSG_3(RF_MED,
           " RF dev vote_validate(), rfm_dev_val: %d,"
           " dev_stat: %d, dev_votg_type: %d!!",
           rfm_dev_val,dev_status, dev_voting_type);

  if (rfm_dev_val >= RFM_MAX_DEVICES)
  {
    RF_MSG_3(RF_ERROR, 
             " RF dev vote_validity_status FAILED whong RFM_Device: %d,"
			 " dev_stat: %d, dev_votg_type: %d!!",
             rfm_dev_val,dev_status,dev_voting_type);
    return FALSE;
  }

  vote_validity_status = FALSE;

  switch (dev_status)
  {
  case RFCMN_CORE_DEV_ON:
    switch (dev_voting_type)
    {
    case RFCMN_CORE_DEV_NON_CRITICAL_VOTE:
      if (rfm_dev_val == rfcmn_core_mgr.rfm_dev_for_gps)
      {
        vote_validity_status = TRUE;
      }
      break;
    case RFCMN_CORE_DEV_NORMAL_VOTE:
      vote_validity_status = TRUE;
      break;

    default:
      vote_validity_status = FALSE;
      RF_MSG_3(RF_MED,
               " RF dev vote_validity_status FAILED, rfm_dev_val: %d,"
			   " dev_stat: %d, dev_votg_type: %d!!",
               rfm_dev_val,dev_status,dev_voting_type);
      break;
    }
    break;
  case RFCMN_CORE_DEV_OFF:
    if ((rfcmn_core_mgr.voting_status[rfm_dev_val] 
         == RFCMN_CORE_DEV_CRITICAL_VOTE) 
        || (rfcmn_core_mgr.voting_status[rfm_dev_val] 
            == RFCMN_CORE_DEV_NON_CRITICAL_VOTE))
    {
      vote_validity_status = TRUE;
      break;
    }
    break;
  default:
    RF_MSG_3(RF_ERROR,
             " RF dev vote_validity_status FAILED, rfm_dev_val: %d,"
			 " dev_stat: %d,  dev_votg_type: %d!!",rfm_dev_val, 
             dev_status, dev_voting_type);
    vote_validity_status = FALSE;
    break;
  }

  RF_MSG_1(RF_LOW,
           " Time Spent during rfcmn_core_dev_vote_validate() : %d ",
           timetick_get_elapsed( dev_mgr_time, T_USEC ));

  return vote_validity_status;

} /* rfcmn_core_dev_vote_validity*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RFCommon Core Device Manager Debug msg print API.

  @details
  This function is internal function to the RF Common Core Device Manager.
  This function should only be used within RF Common Core Device Manager, 
  NO techs can call this API explicitly. This API prints out F3s with
  debug information. This function is only called when debug flag is 
  enabled
 
  @return
  void
 
*/

void rfcommon_core_mgr_debug_msgs()
{
  uint8 dev_mgr_index = 0;
  uint8 temp_rfm_dev_val = RFM_DEVICE_0;

  RF_MSG(RF_MED, 
         " Entering rfcommon_core_rfdevice_mgr_debug_msgs()"
				 " RF Device Manager Structures!!");

  for ( dev_mgr_index = 0; dev_mgr_index < rfcmn_core_mgr.phy_dev_count;
      ++dev_mgr_index )
  {
    RF_MSG_2(RF_MED,
             "rfcmn_core_mgr.rfcmn_core_dev_mgr[%d].rfcmn_core_dev = %d",
             dev_mgr_index,rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev);
    RF_MSG_2(RF_MED,
             "rfcmn_core_mgr.rfcmn_core_dev_mgr[%d].rfcmn_core_dev_id = %d",
             dev_mgr_index,rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_id);
    RF_MSG_2(RF_MED,
             "rfcmn_core_mgr.rfcmn_core_dev_mgr[%d].rfcmn_core_dev_stat = %d",
             dev_mgr_index,rfcmn_core_mgr.dev_mgr[dev_mgr_index].dev_status);
    RF_MSG_2(RF_MED,
             "rfcmn_core_mgr.rfcmn_core_dev_mgr[%d].is_multi_votes_on = %d",
             dev_mgr_index,rfcmn_core_mgr.is_wwan_vote_on);
  }
    for (temp_rfm_dev_val = RFM_DEVICE_0; temp_rfm_dev_val < RFM_MAX_DEVICES; 
        ++ temp_rfm_dev_val )
    {
    RF_MSG_2(RF_MED,
             "rfcmn_core_mgr.voting_status[%d] = %d ",
             temp_rfm_dev_val,
             rfcmn_core_mgr.voting_status[temp_rfm_dev_val]);   
  }

  RF_MSG(RF_MED," RF Global Manager Structure!!");

  RF_MSG_1(RF_MED,
           "rfcmn_core_mgr.rfcmn_core_dev_mgr_cnt = %d"
           ,rfcmn_core_mgr.phy_dev_count);
  RF_MSG_1(RF_MED,
           "rfcmn_core_mgr.rfcmn_core_dev_mgr_init_done = %d"
           ,rfcmn_core_mgr.init_done);

  for (temp_rfm_dev_val = RFM_DEVICE_0; temp_rfm_dev_val < RFM_MAX_DEVICES; 
      ++ temp_rfm_dev_val )
  {
    RF_MSG_2(RF_MED,
             "rfcmn_core_mgr.rfcmn_core_dev_votg_stat[%d] = %d",
             temp_rfm_dev_val,
             rfcmn_core_mgr.voting_status[temp_rfm_dev_val]);
  }
} /* rfcommon_core_rfdevice_mgr_debug_msgs() */

void rfcommon_core_detect_gnss_devices(void)
{
  uint8 dev_id = 0;

//  rfdevice_rxtx_common_class* temp_gps_dev_param = NULL;
  if (rfcmn_core_mgr.gps_devices_detection_done == FALSE)
  {
    for (dev_id = 0; dev_id < rfcmn_core_mgr.phy_dev_count; dev_id++)
    {
     /* hack to initialize GPS dev mgr for all devices in the given RFC to keep all the
     devices in the same state at any instance with 
     GPS only enable power optimization changes */
     rfcmn_core_mgr.gps_dev_mgr[dev_id] = &rfcmn_core_mgr.dev_mgr[dev_id];
   }
   rfcmn_core_mgr.gps_devices_detection_done = TRUE;
 }
  else
  {
    RF_MSG(RF_MED,"detect_gnns_devices(): GNSS devices detected");
  }
}

/*! @} */
/*! @} */
