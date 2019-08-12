#ifndef RFDEVICE_QASM_TYPEDEF_H
#define RFDEVICE_QASM_TYPEDEF_H

/*=============================================================================

Copyright (c) 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S.
  Government. Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qasm/api/rfdevice_qasm_typedef.h#1 $
  $DateTime: 2016/12/14 01:58:13 $
  $Author: mplcsds1 $

when         who   what, where, why
--------   ---   ---------------------------------------------------------------
05/31/16   dbc    Add common tab settings
11/19/14   vv     rfdevice_reg_dump() API  for debug
09/17/14   px     Add QASM EFS support
08/14/14   dbc    Add support for XSW scpripts for ASMs with cross switches
09/11/13   aca    Common settings support
08/05/13   aca    Initial version

=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "rfcom.h"

#include "rf_hal_bus_types.h"
#include "rf_buffer_intf.h"


/*==========================================================================*/
/* DEFINES and ENUMS  */
/*==========================================================================*/

#define RFDEVICE_QASM_MAX_SCRIPT_SIZE 50
#define RFC_DEFINED_RFFE_TRANSACTION_TIME 0
#define RFDEVICE_QASM_MAX_REG_ADDR 0xFF
#define RFDEVICE_QPA_MAX_PORTS 0x20 /* Max 8 bit per RFC limitations- need to change to dynamic allocation*/

#define RFDEVICE_QASM_EFS_DEBUG_PATH_STR "/qfe/qasm/dev%d_rev%d"
#define RFDEVICE_QASM_EFS_DEBUG_MAX_PATH_LENGTH 40

#define RFDEVICE_QASM_REG_INVALID -1


/*==========================================================================*/
/* DATA STRUCTURES  */
/*==========================================================================*/

typedef struct
{
  const int16** data_tbl; /* int to denote invalid by -ve */
  const uint16** reg_tbl; /* uint16 for extended addressing */
} rfdevice_qasm_ag_settings_type;

typedef struct
{
  const uint8* data_tbl;
  const uint8* reg_tbl;
} rfdevice_qasm_efs_settings_type;

typedef enum
{
  ASM_RX_PORT,
  ASM_TX_PORT,
  ASM_XSW_PORT,
} rfdevice_qasm_port_type;

typedef struct
{
  uint8 gsm[RFM_MAX_DEVICES][RFCOM_NUM_GSM_BANDS];
  uint8 tdscdma[RFM_MAX_DEVICES][RFCOM_NUM_TDSCDMA_BANDS];
  uint8 cdma[RFM_MAX_DEVICES][RFM_CDMA_MAX_BAND];
  uint8 wcdma[RFM_MAX_DEVICES][RFCOM_NUM_WCDMA_BANDS];
  uint8 lte[RFM_MAX_DEVICES][RFCOM_NUM_LTE_BANDS];
} rfdevice_qasm_port_info_type;

typedef struct
{
  boolean extended_cmd;  /*!< RFFE Indicator to use extended cmd or not */
  rf_hal_bus_resource_script_settings_type settings;
} rfdevice_qasm_ag_cfg_type;

typedef struct
{
  /* Enable Rx */
  uint8 asm_enable_rx_script_size;
  rfdevice_qasm_ag_settings_type asm_enable_rx;

  /* Enable Tx */
  uint8 asm_enable_tx_script_size;
  rfdevice_qasm_ag_settings_type asm_enable_tx;

  /* Disable Rx */
  uint8 asm_disable_rx_script_size;
  rfdevice_qasm_ag_settings_type asm_disable_rx;

  /* Disable Tx */
  uint8 asm_disable_tx_script_size;
  rfdevice_qasm_ag_settings_type asm_disable_tx;

  /* XSW Ant Def Rx */
  uint8 asm_xsw_ant_def_rx_script_size;
  rfdevice_qasm_ag_settings_type asm_xsw_ant_def_rx;

  /* XSW Ant Swap Rx */
  uint8 asm_xsw_ant_swap_rx_script_size;
  rfdevice_qasm_ag_settings_type asm_xsw_ant_swap_rx;

  /* XSW Ant Def Tx */
  uint8 asm_xsw_ant_def_tx_script_size;
  rfdevice_qasm_ag_settings_type asm_xsw_ant_def_tx;

  /* XSW Ant Swap Tx */
  uint8 asm_xsw_ant_swap_tx_script_size;
  rfdevice_qasm_ag_settings_type asm_xsw_ant_swap_tx;

  /* XSW Port */
  uint8 asm_xsw_port_script_size;
  rfdevice_qasm_ag_settings_type asm_xsw_port;

} rfdevice_qasm_port_settings_type;

typedef struct
{
  /* Init */
  uint8 asm_init_script_size;
  rfdevice_qasm_ag_settings_type asm_init;

  /* Trigger */
  uint8 asm_trigger_script_size;
  rfdevice_qasm_ag_settings_type asm_trigger;

  /* Sleep */
  uint8 asm_sleep_script_size;
  rfdevice_qasm_ag_settings_type asm_sleep;

  /* Power Down */
  uint8 asm_power_down_script_size;
  rfdevice_qasm_ag_settings_type asm_power_down;

  /* Wake Up */
  uint8 asm_wake_up_script_size;
  rfdevice_qasm_ag_settings_type asm_wake_up;

  /* WLAN Port */
  uint8 asm_wlan_port_script_size;
  rfdevice_qasm_ag_settings_type asm_wlan_port;

  /* Common Tab Power Up Init */
  uint8 asm_common_power_up_init_script_size;
  rfdevice_qasm_ag_settings_type asm_common_power_up_init;

  /* Common Tab Sleep */
  uint8 asm_common_sleep_script_size;
  rfdevice_qasm_ag_settings_type asm_common_sleep;

  /* Common Tab Wake Up */
  uint8 asm_common_wake_up_script_size;
  rfdevice_qasm_ag_settings_type asm_common_wake_up;

} rfdevice_qasm_common_settings_type;


typedef struct
{
  /* Number of active ports */
  uint8 num_ports;

  /* Pointer to port data */
  rfdevice_qasm_port_settings_type port_settings_ag;

  /* Pointer to port independent data */
  rfdevice_qasm_common_settings_type common_settings_ag;

} rfdevice_qasm_settings_type;

/* The following structure is used for debug/ OFF target */
typedef struct
{
  /* Flag to track if we are in debug mode */
  boolean debug_mode;
  /* Internal buffer showing chip data */
  uint8 *reg_data;
  /* Internal address buffer to store sequence of wrties */
  rf_buffer_intf *rf_buff;

} rfdevice_qasm_settings_debug_type;

/*==========================================================================*/
/* Function Pointers  */
/*==========================================================================*/

typedef boolean (*qasm_sleep_fn_ptr)( void );
typedef boolean (*qasm_wakeup_fn_ptr)( void );


typedef struct
{
  qasm_sleep_fn_ptr sleep;
  qasm_wakeup_fn_ptr wakeup;
} rfdevice_qasm_func_tbl_type;

#endif /*RFDEVICE_QASM_TYPEDEF */

