#ifndef RF_L1_INTF_H
#define RF_L1_INTF_H

/*!
  @file
  rf_l1_intf.h

  @brief
  RF and L1 interface header file

  @details
  The file defines all the interfaces between WCDMA RF and L1
*/

/*===========================================================================
Copyright (c) 1999 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/mc/inc/rfwcdma_l1_intf.h#1 $ 
  $DateTime: 2016/12/13 07:59:54 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/23/13   vs      Cleanup
01/28/11   dw      Initial TxLM support for WCDMA
01/26/10   dw      updates for 8960 build 
01/06/10   kguo     Merged from mdm9k dev branch to main\latest
09/11/09   sar      Updated ADC interface for CMI.
09/01/09   kguo     added function prototype for rfm_enable_tx/rfm_disable_tx
05/26/09   kguo     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "rex.h"
#include "sys.h"
#include "rfcom.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define RF_WCDMA_CMD_DONE_SIG          0x01

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Main control command ID.

  @details
  This enumeration defines all the main control command IDs for RF and L1 interface
*/

typedef enum
{
  RF_WCDMA_CMD_MIN = 0, 
  RF_WCDMA_ENTER_MODE_CMD,
  RF_WCDMA_EXIT_MODE_CMD,
  RF_WCDMA_TUNE_TO_CHAN_CMD,
  RF_WCDMA_WAKEUP_CMD,
  RF_WCDMA_SLEEP_CMD,
  RF_WCDMA_CONFIG_TX_CMD,
  RF_WCDMA_SET_PRIMARY_PATH_CMD,
  RF_WCDMA_TECH_SWITCH_UPDATE_CMD,
  RF_WCDMA_CMD_MAX
} rf_wcdma_cmd_id_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Main control response ID.

  @details
  This enumeration defines all the main control response IDs for RF and L1 interface
*/

typedef enum
{
  RF_WCDMA_RSP_MIN = 0, 
  RF_WCDMA_ENTER_MODE_RSP,
  RF_WCDMA_EXIT_MODE_RSP,
  RF_WCDMA_TUNE_TO_CHAN_RSP,
  RF_WCDMA_WAKEUP_RSP,
  RF_WCDMA_SLEEP_RSP,
  RF_WCDMA_CONFIG_TX_RSP,
  RF_WCDMA_SET_PRIMARY_PATH_RSP,
  RF_WCDMA_TECH_SWITCH_UPDATE_RSP,
  RF_WCDMA_RSP_MAX
} rf_wcdma_rsp_id_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Defines carrier type.

  @details
  This enumeration defines carrier types such as single carrier mode and dual-carrier mode.
*/
typedef enum
{
  WCDMA_SC_MODE,
  WCDMA_DC_MODE,
  WCDMA_MC_MODE_MAX
}rf_wcdma_mc_mode_type;

/*----------------------------------------------------------------------------*/
/*!
    @brief
    Command data structure for RF_WCDMA_ENTER_MODE_CMD.

    @details
    This data structure defines parameters for RF_WCDMA_ENTER_MODE_CMD
*/

typedef struct
{
  /*! @brief indicates device type to use */
  rfcom_device_enum_type device;
  /*! @brief indicates operating mode and frequency band to use */
  rfcom_mode_enum_type   rf_mode;
  /*! @brief RF sets this signal when the command operation is done. If it's NULL, 
             then RF does not set any signal when the command operation is done.*/
  rex_sigs_type          complete_sig;
  /*! @brief pointer to user data passed into callback. Not used */
  void                  *const user_data_ptr;
  /*! @brief callback handler. Not used */
  rfm_cb_handler_type    cb_handler;
}rf_wcdma_enter_mode_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
    @brief
    Command data structure for RF_WCDMA_EXIT_MODE_CMD.

    @details
    This data structure defines parameters for RF_WCDMA_EXIT_MODE_CMD
*/

typedef struct
{
  /*! @brief indicates device type to use */
  rfcom_device_enum_type device;
  /*! @brief indicates operating mode and frequency band to use */
  rfcom_mode_enum_type   rf_mode;
  /*! @brief RF sets this signal when the command operation is done. If it's NULL, 
             then RF does not set any signal when the command operation is done.*/
  rex_sigs_type          complete_sig;
}rf_wcdma_exit_mode_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
    @brief
    Command data structure for RF_WCDMA_TUNE_TO_CHAN_CMD.

    @details
    This data structure defines parameters for RF_WCDMA_TUNE_TO_CHAN_CMD
*/

typedef struct
{
  /*! @brief indicates if it's for SC, DC-HSPA or MC */
  rf_wcdma_mc_mode_type    mode_type;
  /*! @brief indicates device type to use for SC mode or for primary carrier if it's DC mode */
  rfcom_device_enum_type   device;
  /*! @brief indicates channel number to tune to for SC mode or DC mode.
      channel[0] is the channel number to tune for SC, and both channel[0] (for primary carrier) and 
      channel[1] (for secondary carrier) are the channel numbers to tune to DC mode. This can be 
      expanded to supoort MC mode in the future.  */
  uint16                   channel[WCDMA_MC_MODE_MAX];
  /*! @brief indicates whether Rx, Tx, or both synthesizers should be tuned by RF driver */
  rfm_synth_tune_enum_type synths;
  /*! @brief RF sets this signal when the command operation is done. If it's NULL, 
             then RF does not set any signal when the command operation is done.*/
  rex_sigs_type            complete_sig;
  /*! @brief pointer to user data passed into callback. Not used */
  const void               *user_data_ptr; 
  /*! @brief callback handler. Not used */
  rfm_cb_handler_type      cb_handler;
}rf_wcdma_tune_to_chan_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
    @brief
    Command data structure for RF_WCDMA_WAKEUP_CMD.

    @details
    This data structure defines parameters for RF_WCDMA_WAKEUP_CMD
*/

typedef struct
{
  /*! @brief indicates device type to use */
  rfcom_device_enum_type device;
  /*! @brief indicates operating mode and frequency band to use */
  rfcom_mode_enum_type   rf_mode;
  /*! @brief pointer to user data passed into callback. Not used */
  /*! @brief RF sets this signal when the command operation is done. If it's NULL, 
             then RF does not set any signal when the command operation is done.*/
  rex_sigs_type          complete_sig;
  const void             *user_data_ptr;
  /*! @brief callback handler. Not used */
  rfm_cb_handler_type    cb_handler;
}rf_wcdma_wakeup_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
    @brief
    Command data structure for RF_WCDMA_SLEEP_CMD.

    @details
    This data structure defines parameters for RF_WCDMA_SLEEP_CMD
*/
typedef struct
{
  /*! @brief indicates device type to use */
  rfcom_device_enum_type device;
  /*! @brief pointer to user data passed into callback. Not used */
  /*! @brief RF sets this signal when the command operation is done. If it's NULL, 
             then RF does not set any signal when the command operation is done.*/
  rex_sigs_type          complete_sig;
  const void             *user_data_ptr;
  /*! @brief callback handler. Not used */
  rfm_cb_handler_type    cb_handler;
}rf_wcdma_sleep_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
    @brief
    Command data structure for RF_WCDMA_CONFIG_TX_CMD.

    @details
    This data structure defines parameters for RF_WCDMA_CONFIG_TX_CMD
*/
typedef struct
{
  /*! @brief indicates device type to use */
  rfcom_device_enum_type device;
  /*! @brief indicates if enable or disable tx. TRUE to enable tx. FALSE to disable tx */
  boolean                enable_tx;
  /*! @brief pointer to user data passed into callback. Not used */
  /*! @brief RF sets this signal when the command operation is done. If it's NULL, 
             then RF does not set any signal when the command operation is done.*/
  rex_sigs_type          complete_sig;
  const void             *user_data_ptr;
  /*! @brief callback handler. Not used */
  rfm_cb_handler_type    cb_handler;
}rf_wcdma_config_tx_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
    @brief
    Command data structure for RF_WCDMA_SET_PRIMARY_PATH_CMD.

    @details
    This data structure defines parameters for RF_WCDMA_SET_PRIMARY_PATH_CMD
*/
typedef struct
{
  /*! @brief indicates RF path to set as primary path */
  rf_path_enum_type rf_path;
  /*! @brief RF sets this signal when the command operation is done. If it's NULL, 
             then RF does not set any signal when the command operation is done.*/
  rex_sigs_type     complete_sig;
}rf_wcdma_set_primary_path_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
    @brief
    Command data structure for RF_WCDMA_TECH_SWITCH_UPDATE_CMD.

    @details
    This data structure defines parameters for RF_WCDMA_TECH_SWITCH_UPDATE_CMD
*/
typedef struct
{
  /*! @brief indicates if it's R99 or HS */
  rfcom_tech_type tech;
  /*! @brief RF sets this signal when the command operation is done. If it's NULL, 
             then RF does not set any signal when the command operation is done.*/
  rex_sigs_type   complete_sig;
}rf_wcdma_tech_switch_update_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
    @brief
    Command data structure.

    @details
    This data structure defines all the command data structures for RF and L1 interface
*/
typedef union
{
  rf_wcdma_enter_mode_cmd_type enter_mode_params;
  rf_wcdma_exit_mode_cmd_type exit_mode_params;
  rf_wcdma_tune_to_chan_cmd_type tune_to_chan_params;
  rf_wcdma_wakeup_cmd_type wakeup_params;
  rf_wcdma_sleep_cmd_type sleep_params;
  rf_wcdma_config_tx_cmd_type config_tx_params;
  rf_wcdma_set_primary_path_cmd_type set_primary_path_params;
  rf_wcdma_tech_switch_update_cmd_type tech_switch_update_params;
} rf_cmd_params_type;

/*----------------------------------------------------------------------------*/
/*!
    @brief
    Command status enum.

    @details
    This enum defines all the main control command execution status. 
*/
typedef enum
{
  /*! @brief if the CMD was executed with success*/
  RF_WCDMA_SUCCESS,
  /*! @brief if the error is due to L1 calling this CMD when RFSW is not in the right 
       state or with missing arguments */
  RF_WCDMA_STATE_MACHINE_ERROR,
  /*! @brief This is an error that occurs because of the execution of the command, 
       and the CMD was sent in the right RFSW state with valid arguments */
  RF_WCDMA_EXECUTION_ERROR,
  RF_WCDMA_CMD_STATUS_MAX
}rf_wcdma_cmd_status_type;

#endif  /* RF_L1_INTF_H */
