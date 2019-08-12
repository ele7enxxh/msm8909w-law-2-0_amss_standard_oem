#ifndef SNS_PM_H
#define SNS_PM_H

/*============================================================================

  @file sns_pm.h

  @brief
  This file contains definitions for use by other Sensor Modules.


  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/


/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:
//source/qcom/qct/core/sensors/dsps/dsps/main/latest/pm/inc/sns_pm.h#1
$


when         who     what, where, why
----------   ---     ---------------------------------------------------------
08-18-2015   rt      Removed uImage entry hysteresis timer
06-29-2015   agk     Added sns_pm_pwrrail_init
06-21-2015   hw      Adding SSM client ID to PM framework
09-03-2014   jhh     Clean up client_init
08-26-2014   jhh     Moved sns_em.h to common header
05-08-2014   jhh     Include sns_pm_init into sensors init sequence
03-03-2014   jhh     Initial version
========================================================================*/

/*=======================================================================
                              INCLUDES
========================================================================*/
#include "sns_em.h"
#ifdef FAST_PLAYBACK
#include "com_dtypes.h" //PENDING, investigate how scons compile w/o explicit def
#endif
/*=======================================================================
                  INTERNAL DEFINITIONS AND TYPES
========================================================================*/
#define NUM_MAX_CLIENT_NAME_LENGTH 10
#define  UIMG_ENTER_HYSTERESIS_TICKS   (uint32_t)(7000/30.51)  /* 7 mill sec uimg enter delay */

/* Handle to be used for all clients to PM */
typedef void* sns_pm_handle_t;
typedef void (*suspend_cb_t)(bool kernel_suspend);
typedef void (*uimg_exit_cb_t)(void);

/* Return type indicating casues for error */
typedef enum
{
  SNS_PM_SUCCESS,
  SNS_PM_ERR_NO_MEM,
  SNS_PM_ERR_NOT_ALLOWED,
  SNS_PM_ERR_NOT_SUPPORTED,
  SNS_PM_ERR_INVALID_PARAM,
  SNS_PM_ERR_FAILED,
  SNS_PM_ERR_UNKNOWN
} sns_pm_err_code_e;

/* Overall power state for Sesnros */
typedef enum
{
  SNS_PWR_STATE_SVS2,
  SNS_PWR_STATE_SVS,
  SNS_PWR_STATE_NOMINAL,
  SNS_PWR_STATE_TURBO
} sns_pm_pwr_state_e;

/* Type of resource needed for Sensors clients */
typedef enum
{
  SNS_CORE_Q6,
  SNS_CORE_DDR,
  SNS_CORE_SRAM,
  SNS_PNOC_BUS_I2C,
  SNS_PNOC_BUS_SPI
} sns_pm_core_e;

/* Operational mode for Sensors */
typedef enum
{
  SNS_IMG_MODE_UNKNOWN,
  SNS_IMG_MODE_MICRO,
  SNS_IMG_MODE_BIG,
  SNS_IMG_MODE_NOCLIENT
} sns_pm_img_mode_e;

/* Power rail options available for PMIC */
typedef enum
{
  SNS_PWR_RAIL_INVALID,
  SNS_PWR_RAIL_OFF,
  SNS_PWR_RAIL_ON_LPM,
  SNS_PWR_RAIL_ON_NPM
} sns_pm_pwr_rail_e;

/* Client list for PM */
typedef enum
{
  SNS_PM_CLIENT_ID_SMGR = 0,
  SNS_PM_CLIENT_ID_RH,
  SNS_PM_CLIENT_ID_SAM,
  SNS_PM_CLIENT_ID_UTIL,
  SNS_PM_CLIENT_ID_DEBUG,
  SNS_PM_CLIENT_ID_SSM,
  SNS_PM_CLIENT_ID_QFP,
  SNS_PM_CLIENT_ID_MAX
} sns_pm_client_id_e;

/* Data structure for current clinet information and status */
typedef struct sns_pm_client_info_s
{
  char client_name[NUM_MAX_CLIENT_NAME_LENGTH];
  sns_pm_client_id_e client_id;
  uint32 mips; /* Million Instructions / sec */
  uint32 bw;   /* Bytes / sec */
  uint32 max_tolerable_delay;
  sns_pm_img_mode_e current_vote;
  suspend_cb_t callback;
  uimg_exit_cb_t uimg_exit_cb;
} sns_pm_client_info_s;

/*=======================================================================
                             FUNCTIONS
========================================================================*/

/*=======================================================================

  FUNCTION : sns_pm_init

========================================================================*/
/*!
  @brief
  This function initializes pm internal structure and handles needed for
  external power request such as mmpm, npa, and pmic.

  @return
  SNS_SUCCESS if init is successful.
  SNS_ERR_FAILED otherwise.
*/
/*======================================================================*/
sns_err_code_e sns_pm_init(void);

/*=======================================================================

  FUNCTION : sns_pm_client_init_tag

========================================================================*/
/*!
  @brief
  This function returns a handle to the clients. Clients shall use this
  handle in all subsequent PM operations. Cb function is expected to be
  provided if client want to get notification when apps suspend or awake.

  @param[out] client_handle : handle to client
  @param[in] callback : cb function to be notified apps suspend/awake
  @param[in] client_name : name of the client

  @return
  SNS_PM_SUCCESS if init is successful.
  SNS_PM_ERR_NO_MEM if memory allocation fails for client handle
  SNS_PM_ERR_UNKNOWN otherwise.
*/
/*======================================================================*/
sns_pm_err_code_e sns_pm_client_init( sns_pm_handle_t *client_handle,
                                      suspend_cb_t callback,
                                      char* client_name,
                                      sns_pm_client_id_e client_id);

sns_pm_err_code_e sns_pm_client_register_uimg_exit_cb(
  sns_pm_handle_t client_handle,
  uimg_exit_cb_t  callback);

/*=======================================================================

  FUNCTION : sns_pm_client_close

========================================================================*/
/*!
  @brief
  This function deregisters the associated client.

  @param[in] client_handle : handle to client.

  @return
  None
*/
/*======================================================================*/
void sns_pm_client_close( sns_pm_handle_t client_handle );


/*=======================================================================

  FUNCTION : sns_pm_vote_power_rail

========================================================================*/
/*!
  @brief
  This function votes for power rail to be turned ON/OFF.

  @param[in] client_handle : handle of the client.
  @param[in] rail               : pwr rail ID identifying the rail to be voted on.
  @param[in] value         : true to turn ON, false to turn OFF.

  @return
  SNS_PM_SUCCESS if successful.
  SNS_PM_NOT_ALLOWED if attempted to use in uimage mode.
  SNS_PM_ERR_UNKNOWN otherwise.

*/
/*======================================================================*/
sns_pm_err_code_e sns_pm_vote_power_rail( sns_pm_handle_t client_handle, uint8_t rail,
                                          sns_pm_pwr_rail_e state );


/*=======================================================================

  FUNCTION : sns_pm_vote_mips

========================================================================*/
/*!
  @brief
  This function sets the active power state for sensors based on client
  MIPS

  @param[in] client_handle : handle of the client.
  @param[in] mips          : mips requirement of the client (instructions/sec)

  @return
  SNS_PM_SUCCESS if successful.
  SNS_PM_NOT_ALLOWED if attempted to use in uimage mode.
  SNS_PM_ERR_FAILED if total_mips becomes unexpected value
  SNS_PM_ERR_INVALID_PARAM if mmpm request fails
  SNS_PM_ERR_UNKNOWN otherwise.

*/
/*======================================================================*/
sns_pm_err_code_e sns_pm_vote_mips( sns_pm_handle_t client_handle,
                                    uint32 mips );


/*=======================================================================

  FUNCTION : sns_pm_vote_bw

========================================================================*/
/*!
  @brief
  This function sets the active power state for sensors based on client
  BW

  @param[in] client_handle : handle of the client.
  @param[in] bw            : bw requirement of the client (bytes/sec)
  @param[in] core          : core ID that will be voted for BW

  @return
  SNS_PM_SUCCESS if successful.
  SNS_PM_NOT_ALLOWED if attempted to use in uimage mode.
  SNS_PM_ERR_FAILED if total_bw becomes unexpected value
  SNS_PM_ERR_INVALID_PARAM if mmpm request fails
  SNS_PM_ERR_NOT_SUPPORTED if voted core is not supported
  SNS_PM_ERR_UNKNOWN otherwise.

*/
/*======================================================================*/
sns_pm_err_code_e sns_pm_vote_bw( sns_pm_handle_t client_handle,
                                  uint32 bw,
                                  sns_pm_core_e core );


/*=======================================================================

  FUNCTION : sns_pm_set_wakeup_interval

========================================================================*/
/*!
  @brief
  This function provides effective wakeup rate information to sleep
  driver's /core/max_duration node in the appropriate format.This function
  should be thread safe as multiple clients will call the API.
  Effective wake up rate refers to how often ADSP actually wakes up to
  collect data.

  @param[in] client_handle   : handle of the client;
  @param[in] wakeup_interval : effective wake up rate of ADSP in hz.

  @return
  SNS_PM_SUCCESS if successful.
  SNS_PM_NOT_ALLOWED if attempted to use in uimage mode.
  SNS_PM_ERR_UNKNOWN otherwise.

*/
/*======================================================================*/
sns_pm_err_code_e sns_pm_set_wakeup_interval( sns_pm_handle_t client_handle,
                                              uint32 wakeup_interval );


/*=======================================================================

  FUNCTION : sns_pm_set_latency

========================================================================*/
/*!
  @brief
  This function sets latency restriction provided by clients to sleep
  driver's latency node. Clients must use this function to provide their
  latency requirements. This function should be thread safe.
  pm_latency_mutex_ptr shall be used to protect critical sections in
  this function.

  @param[in] client_handle : handle of the client;
  @param[in] latency       : maximum delay in microseconds that the client
                             can tolerate from the time of occurrence of
                             the event (interrupt for example) till the
                             time it's processed.

  @return
  SNS_PM_SUCCESS if successful.
  SNS_PM_ERR_UNKNOWN otherwise.

*/
/*======================================================================*/
sns_pm_err_code_e sns_pm_set_latency( sns_pm_handle_t client_handle,
                                      uint32 latency_us );


/*=======================================================================

  FUNCTION : sns_pm_vote_img_mode

========================================================================*/
/*!
  @brief
  This function votes for image modes (uImage or Big image) based on
  client input. Vote to uImage will be done through NPA node (/core/usleep).
  Vote to big image will be done through a function call to uSleep.The
  function is asymmetric. It is synchronous when entering big image mode
  whereas it is asynchronous when entering uImage mode. If the request is
  to enter uImage mode, there shall be a callback later on when system
  really enters uImage mode.
  Clients are not expected to wait for the callback when entering uImage
  mode. Clients are expected to continue to operate in uImage mode when
  this function returns.

  @param[in] client_handle : handle of the client.
  @param[in] mode          : image mode (SNS_IMG_MODE_MICRO or SNS_IMG_MODE_BIG)

  @return
  SNS_PM_SUCCESS if successful.
  SNS_PM_ERR_FAILED if it fails to exit out of uimage
  SNS_PM_ERR_NOT_SUPPORTED if voted image mode is not supported
  SNS_PM_ERR_UNKNOWN otherwise.


*/
/*======================================================================*/
sns_pm_err_code_e sns_pm_vote_img_mode( sns_pm_handle_t client_handle,
                                        sns_pm_img_mode_e mode);


/*=======================================================================

  FUNCTION : sns_pm_uimg_vote_override

========================================================================*/
/*!
  @brief
  This function sets the 'final' uimage vote PM casts

  @param[in] uimg_override : FALSE = NO OP
                             TRUE = Suppress the final vote to *never* vote
                             for microImage.
  Note that this function does not change the behaviour of any vote to disable
  uImage. Such votes cannot be suppressed.

*/
/*======================================================================*/
void sns_pm_set_uimg_vote_override(bool uimg_override);

/*=======================================================================
 
  FUNCTION : sns_pm_pwrrail_init
 
========================================================================*/
/*!
  @brief 
  This function initializes power rails if not already initialized
  
  @param[in] rail - the power rail that needs to be initialized
  Device will crash if rail initialization fails

*/
/*======================================================================*/

void sns_pm_pwrrail_init (uint8_t rail);

/*===========================================================================

FUNCTION sns_pm_uimg_enter

DESCRIPTION
  Send request to Usleep client to enter uImage and release DDR BW
===========================================================================*/

void sns_pm_ap_wakeup_broadcast(void);

void sns_pm_uimg_enter( void );

#endif /* SNS_PM_H */

