#ifndef RFDEVICE_ASM_TYPES
#define RFDEVICE_ASM_TYPES

/*!
  @file
  rfdevice_asm_types.h

  @brief
  This file contains the type definitions required by ASM driver.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_asm/api/rfdevice_asm_types.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
08/05/14   cc      Common Setting Script support 
05/05/14   sn      ASM class based API
01/11/14   vv      Added an enum to support init ag settings on certain ASMs
02/19/13   sr      Split the autogen with on/off/trigger scripts.
01/28/13   sr      init version
============================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RFDEVICE_ASM_REG_INVALID -1
#define RFDEVICE_ASM_DEFAULT_MASK 0xFF

  typedef enum
  {
    RFDEVICE_ASM_RX,
    RFDEVICE_ASM_TX,
    RFDEVICE_ASM_RXTX_MAX,
    RFDEVICE_ASM_RXTX_INVALID = RFDEVICE_ASM_RXTX_MAX,
  }
  rfdevice_asm_rxtx_enum_type;

  typedef enum
  {
    RFDEVICE_ASM_ON_DATA,
    RFDEVICE_ASM_OFF_DATA,
    RFDEVICE_INIT_DATA,
    RFDEVICE_ASM_TRIGGER_DATA,
    RFDEVICE_ASM_RESET_DATA,
	RFDEVICE_ASM_THERM_READ_DATA,
	RFDEVICE_ASM_THERM_ON_DATA,
	RFDEVICE_ASM_THERM_OFF_DATA,
    RFDEVICE_ASM_SLEEP_DATA,
    RFDEVICE_ASM_DATA_REQ_INVALID
  } rfdevice_asm_data_req_enum_type;

  typedef struct
  {
    int32 mfg_id;
    int32 prd_id;
    int32 prd_rev;
    int32 num_ports;
  } rfdevice_asm_info_type;

  typedef struct
  {
    rfdevice_asm_data_req_enum_type req;
    uint8 port;
  } rfdevice_asm_cfg_params_type;


  typedef struct
  {
    uint8 num_regs;
    uint8 *addr;
    int16 *data;
  } rfdevice_asm_reg_settings_type;

  typedef enum
  {
    RFDEVICE_ASM_CMD_WRITE,
    RFDEVICE_ASM_CMD_WRITE_EXT,
    RFDEVICE_ASM_CMD_DELAY,
    RFDEVICE_ASM_CMD_READ,
    RFDEVICE_ASM_CMD_INVALID
  } rfdevice_asm_cmd_enum_type;

  typedef struct
  {
    rfdevice_asm_cmd_enum_type cmd;
    uint8 addr;
    uint8 data;
	uint8 mask;
  } rfdevice_asm_cmd_struct_type;

  typedef struct
  {
    uint8 num_cmds;
    rfdevice_asm_cmd_struct_type *cmds;
  } rfdevice_asm_cmd_seq_type;


#ifdef __cplusplus
}
#endif

#endif


