#ifndef RFDEVICE_QCOUPLER_TYPEDEF_H
#define RFDEVICE_QCOUPLER_TYPEDEF_H

/*=============================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qcoupler/api/rfdevice_qcoupler_typedef.h#1 $
  $DateTime: 2016/12/14 01:58:12 $
  $Author: mplcsds1 $
 
when       who   what, where, why
--------   ---   ---------------------------------------------------------------
10/21/15   dbc   Adde port member to rfdevice_qcoupler_rfc_cfg_type
09/24/14   px    Added QCoupler EFS support
04/08/14   yb    Coupler driver integration with RFC and AG files
                 Added tech band information to config API
03/17/14   vv    Updating type defs to support perl scrip update 
02/07/14   yb    Initial version
 
=============================================================================*/
  
/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rf_rffe_common.h"
#include "rf_hal_bus_types.h"
#include "rf_buffer_intf.h"
#include "rfdevice_comm_intf.h"
#include "rfdevice_coupler.h"

/*==========================================================================*/
/* DEFINES and ENUMS  */
/*==========================================================================*/

#define QCOUPLER_MAX_SCRIPT_SIZE 50 /* ? */
#define RFC_DEFINED_RFFE_TRANSACTION_TIME 0
#define RFDEVICE_QCOUPLER_MAX_REG_ADDR 0xFF
#define RFDEVICE_QCOUPLER_MAX_PORTS 0x20 /* Max 8 bit per RFC limitations- need to change to dynamic allocation*/

#define RFDEVICE_QCOUPLER_REG_INVALID -1


/*==========================================================================*/
/* DATA STRUCTURES  */
/*==========================================================================*/

typedef struct
{
  const int16** data_tbl; /* int to denote invalid by -ve */
  const uint16** reg_tbl; /* uint16 for extended addressing */
  uint8 script_size; /* Size of script */
} rfdevice_qcoupler_ag_settings_type;

typedef struct
{
  const uint8* data_tbl;
  const uint8* reg_tbl;
} rfdevice_qcoupler_efs_settings_type;

typedef struct
{
  boolean extended_cmd;  /*!< RFFE Indicator to use extended cmd or not */
  rf_hal_bus_resource_script_settings_type settings;
} rfdevice_qcoupler_ag_cfg_type;

typedef struct
{
  /* Forward */
  uint8 coupler_fwd_script_size;
  rfdevice_qcoupler_ag_settings_type coupler_fwd;

  /* Reverse */
  uint8 coupler_rev_script_size;
  rfdevice_qcoupler_ag_settings_type coupler_rev;
  
  /* Open */
  uint8 coupler_open_script_size;
  rfdevice_qcoupler_ag_settings_type coupler_open;

} rfdevice_qcoupler_port_settings_type;

typedef struct
{
  /* Init */
  uint8 coupler_init_script_size;
  rfdevice_qcoupler_ag_settings_type coupler_init;

  /* Trigger */
  uint8 coupler_trigger_script_size;
  rfdevice_qcoupler_ag_settings_type coupler_trigger;
  
  /* Power Down */
  uint8 coupler_power_down_script_size;
  rfdevice_qcoupler_ag_settings_type coupler_power_down;

} rfdevice_qcoupler_common_settings_type;


typedef struct
{ 
  /* Number of active ports */
  uint8 num_ports;
 
  /* Pointer to port data */
  rfdevice_qcoupler_port_settings_type port_settings_ag;

  /* Pointer to port independent data */
  rfdevice_qcoupler_common_settings_type common_settings_ag;

} rfdevice_qcoupler_settings_type;

/* The following structure is used for debug/ OFF target */
typedef struct
{
  /* Flag to track if we are in debug mode */
  boolean debug_mode;
  /* Internal buffer showing chip data */
  uint8 *reg_data;
  /* Internal address buffer to store sequence of wrties */
  rf_buffer_intf *rf_buff;

} rfdevice_qcoupler_settings_debug_type;

typedef struct
{
  /* Current mode config of Coupler */
  rfcom_mode_enum_type mode;
  /* Current band config of Coupler */
  rfcom_band_type_u band;
  /* Attenuation to be programmed */
  rfdevice_coupler_attenuation_type attenuation;
  /* Direction to be programmed */
  rfdevice_coupling_direction_type direction;

} rfdevice_qcoupler_cfg_type;

typedef struct
{
  /* Attenuation in forward direction */
  rfdevice_coupler_attenuation_type fwd_attn;
  /* Attenuation in reverse direction */
  rfdevice_coupler_attenuation_type rev_attn;
  /* Default direction for power measurements - forward or internal - to differentiate b/w TDET and HDET */
  rfdevice_coupling_direction_type default_fwd_direction;
  /* Port to select coupler and filter setting */
  uint8 port;
} rfdevice_qcoupler_rfc_cfg_type;

typedef struct
{
  rfdevice_qcoupler_rfc_cfg_type gsm[RFCOM_NUM_GSM_BANDS];
  rfdevice_qcoupler_rfc_cfg_type tdscdma[RFCOM_NUM_TDSCDMA_BANDS];
  rfdevice_qcoupler_rfc_cfg_type cdma[RFM_CDMA_MAX_BAND];
  rfdevice_qcoupler_rfc_cfg_type wcdma[RFCOM_NUM_WCDMA_BANDS];
  rfdevice_qcoupler_rfc_cfg_type lte[RFCOM_NUM_LTE_BANDS];
} rfdevice_qcoupler_rfc_cfg_band_map_type;

/*==========================================================================*/
/* Function Pointers  */
/*==========================================================================*/

typedef boolean (*qcoupler_sleep_fn_ptr)( void );
typedef boolean (*qcoupler_wakeup_fn_ptr)( void );
typedef boolean (*qcoupler_get_config_data_fn_ptr)( rfdevice_qcoupler_ag_settings_type* port_settings, rfdevice_qcoupler_cfg_type qcoupler_cfg);

typedef struct 
{ 
  qcoupler_sleep_fn_ptr sleep;
  qcoupler_wakeup_fn_ptr wakeup;
  qcoupler_get_config_data_fn_ptr get_config_data;
}rfdevice_qcoupler_func_tbl_type;

#endif /*RFDEVICE_QCOUPLER_TYPEDEF_H */