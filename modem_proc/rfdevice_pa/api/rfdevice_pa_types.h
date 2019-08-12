#ifndef RFDEVICE_PA_TYPES
#define RFDEVICE_PA_TYPES

/*!
  @file
  rfdevice_pa_types.h

  @brief
  This file contains the type definitions required by PA driver.

*/

/*===========================================================================

Copyright (c) 2011-15 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_pa/api/rfdevice_pa_types.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
01/27/15   sn      Common Setting Script support 
02/17/14   vv      Updated rfdevice_pa_data_req_enum_type to support spreadsheet settings 
01/11/14   vv      Added an enum to support config ag settings on certain PAs 
05/28/13   vs      API support for PA therm
02/19/13   sr      Split the autogen with on/off/range/bias scripts.
01/19/13   vrb     Filter out 'Don't Care' registers when creating script: 2G PA req
01/16/13   sr      Added pa command sequence request support.
01/03/13   sr      init version
============================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RF_REG_INVALID 0xFF
#define RFFE_INVALID_REG_ADDR 0x1D

#define RFDEVICE_PA_NUM_8BIT_ICQ_REGS 1
#define RFDEVICE_PA_NUM_16BIT_ICQ_REGS 2

#define RFDEVICE_PA_DEFAULT_MASK 0xFF

typedef enum
{
  RFDEVICE_PA_ON_DATA,
  RFDEVICE_PA_OFF_DATA,
  RFDEVICE_PA_CONFIG_DATA,
  RFDEVICE_PA_SET_RANGE_DATA,
  RFDEVICE_PA_SET_BIAS_DATA,
  RFDEVICE_PA_RESET_DATA,
  RFDEVICE_PA_THERM_ON_DATA,
  RFDEVICE_PA_THERM_READ_DATA,
  RFDEVICE_PA_THERM_OFF_DATA,
  RFDEVICE_PA_TRIGGER_DATA,
  RFDEVICE_PA_INIT_DATA,
  RFDEVICE_PA_SLEEP_DATA,
  RFDEVICE_PA_WAKEUP_DATA,  
  RFDEVICE_PA_DATA_REQ_INVALID
} rfdevice_pa_data_req_enum_type;

typedef struct 
{
  int32 mfg_id;
  int32 prd_id;
  int32 prd_rev;
  int32 num_ports;
  int32 num_pa_ranges;
  boolean lpm_pm_trig_flag;
} rfdevice_pa_info_type;

typedef struct
{
  rfdevice_pa_data_req_enum_type req;
  uint8 pa_range;
  uint16 pa_bias;
  uint8 port;
} rfdevice_pa_cfg_params_type;


typedef struct
{
  uint8 num_regs;
  uint8 *addr;
  int16 *data;
  uint8 bias_reg_idx;
} rfdevice_pa_reg_settings_type;

typedef enum
{
  RFDEVICE_PA_CMD_WRITE,
  RFDEVICE_PA_CMD_WRITE_EXT,
  RFDEVICE_PA_CMD_DELAY,
  RFDEVICE_PA_CMD_READ,
  RFDEVICE_PA_CMD_INVALID
} rfdevice_pa_cmd_enum_type;

typedef struct
{
  rfdevice_pa_cmd_enum_type cmd;
  uint8 addr;
  uint8 data;
  uint8 mask;
} rfdevice_pa_cmd_struct_type;

typedef struct
{
  uint8 num_cmds;
  rfdevice_pa_cmd_struct_type *cmds;
} rfdevice_pa_cmd_seq_type;


#ifdef __cplusplus
}
#endif

#endif


