#ifndef RFDEVICE_PAPM_TYPES
#define RFDEVICE_PAPM_TYPES

/*!
  @file
  rfdevice_papm_types.h

  @brief
  This file contains the type definitions required by PAPM driver.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_papm/api/rfdevice_papm_types.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
10/23/14   vv      Add QFE3100 support
10/03/14   vv      Add alarm register support in AG
12/12/13   vv      Adding support for ag register settings for Device calibration mode.
                   updated papm data enum with RFDEVICE_PAPM_CAL_STATE_DATA
10/30/13   sb      Added support for delay after standby
03/22/13   sr      init version
06/09/13   vs      updated papm data enum with RFDEVICE_PAPM_DISABLE_AUTO_BYPASS_DAT
============================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RFDEVICE_PAPM_REG_INVALID -1
#define RFDEVICE_PAPM_REG_ADDR_INVALID 0xFFFF

typedef enum
{
  RFDEVICE_PAPM_COMMON_INIT_DATA,
  RFDEVICE_PAPM_BYPASS_DATA,
  RFDEVICE_PAPM_BW_DATA,
  RFDEVICE_PAPM_STANDBY_DATA,
  RFDEVICE_PAPM_APT_DATA,
  RFDEVICE_PAPM_ET_DATA,
  RFDEVICE_PAPM_RESET_DATA,
  RFDEVICE_PAPM_SLEEP_DATA,
  RFDEVICE_PAPM_ALARM_REG_READ_ONLY_DATA,
  RFDEVICE_PAPM_TRIGGER_DATA,
  RFDEVICE_PAPM_DISABLE_AUTO_BYPASS_DATA,
  RFDEVICE_PAPM_CAL_STATE_DATA,
  RFDEVICE_PAPM_INVALID_DATA,
} rfdevice_papm_data_req_enum_type;


typedef struct
{
  rfdevice_papm_data_req_enum_type req;
  uint8 bw_idx;
  uint8 port;
} rfdevice_papm_cfg_params_type;


typedef struct
{
  uint8 num_regs;
  uint16 *addr;
  int16 *data;
  uint8 bias_reg_idx;
} rfdevice_reg_settings_type;

typedef enum
{
  RFDEVICE_PAPM_CMD_WRITE,
  RFDEVICE_PAPM_CMD_WRITE_EXT,
  RFDEVICE_PAPM_CMD_DELAY,
  RFDEVICE_PAPM_CMD_READ,
  RFDEVICE_PAPM_CMD_INVALID
} rfdevice_papm_cmd_enum_type;

typedef struct
{
  rfdevice_papm_cmd_enum_type cmd;
  uint8 addr;
  uint8 data;
} rfdevice_papm_cmd_struct_type;

typedef struct
{
  uint8 num_cmds;
  rfdevice_papm_cmd_struct_type *cmds;
} rfdevice_papm_cmd_seq_type;

typedef struct
{
  int16 standby_settling_delay;
}rfdevice_papm_timing_info_type ;


#ifdef __cplusplus
}
#endif

#endif


