#ifndef RFDEVICE_QASM_2G_TYPEDEF_H
#define RFDEVICE_QASM_2G_TYPEDEF_H

/*=============================================================================

Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qasm/api/rfdevice_qasm_2g_typedef.h#1 $
  $DateTime: 2016/12/14 01:58:13 $
  $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
12/07/15   dbc   Added support for QPA/QASM common tab voting
12/03/15   dbc   Added QPA4340 TDD rx/tx trigger
02/11/15   sn    Initial version

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

#define RFDEVICE_QASM_2G_MAX_SCRIPT_SIZE 50

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
} rfdevice_qasm_2g_ag_settings_type;

typedef struct
{
  const uint8* data_tbl;
  const uint8* reg_tbl;
} rfdevice_qasm_2g_efs_settings_type;

typedef enum
{
  ASM_RX_PORT_NUM,
  ASM_TX_PORT_NUM,
} rfdevice_qasm_2g_port_type;

typedef struct
{
  uint8 gsm[RFM_MAX_DEVICES][RFCOM_NUM_GSM_BANDS];
  uint8 tdscdma[RFM_MAX_DEVICES][RFCOM_NUM_TDSCDMA_BANDS];
  uint8 cdma[RFM_MAX_DEVICES][RFM_CDMA_MAX_BAND];
  uint8 wcdma[RFM_MAX_DEVICES][RFCOM_NUM_WCDMA_BANDS];
  uint8 lte[RFM_MAX_DEVICES][RFCOM_NUM_LTE_BANDS];
} rfdevice_qasm_2g_port_info_type;

typedef struct
{
  boolean extended_cmd;  /*!< RFFE Indicator to use extended cmd or not */
  rf_hal_bus_resource_script_settings_type settings;
} rfdevice_qasm_2g_ag_cfg_type;

typedef struct
{
  /* Enable Rx */
  uint8 asm_enable_rx_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_enable_rx;

  /* Enable Tx */
  uint8 asm_enable_tx_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_enable_tx;

  /* Disable Rx */
  uint8 asm_disable_rx_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_disable_rx;

  /* Disable Tx */
  uint8 asm_disable_tx_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_disable_tx;

} rfdevice_qasm_2g_port_settings_type;

typedef struct
{
  /* Init */
  uint8 asm_init_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_init;

  /* Trigger */
  uint8 asm_trigger_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_trigger;

  /* Trigger_enable_rx    TDD trigger workaround*/
  uint8 asm_trigger_enable_rx_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_trigger_enable_rx;

  /* Trigger_enable_tx    TDD trigger workaround*/
  uint8 asm_trigger_enable_tx_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_trigger_enable_tx;
  
  /* Sleep */
  uint8 asm_sleep_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_sleep;

  /* Wakeup */
  uint8 asm_wake_up_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_wake_up;

  /* Common Tab Power Up Init */
  uint8 asm_common_power_up_init_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_common_power_up_init;

  /* Common Tab Sleep */
  uint8 asm_common_sleep_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_common_sleep;

  /* Common Tab Wake Up */
  uint8 asm_common_wake_up_script_size;
  rfdevice_qasm_2g_ag_settings_type asm_common_wake_up;

} rfdevice_qasm_2g_common_settings_type;


typedef struct
{
  /* Number of active ports */
  uint8 num_ports;

  /* Pointer to port data */
  rfdevice_qasm_2g_port_settings_type port_settings_ag;

  /* Pointer to port independent data */
  rfdevice_qasm_2g_common_settings_type common_settings_ag;

} rfdevice_qasm_2g_settings_type;


#endif /*RFDEVICE_QASM_TYPEDEF */

