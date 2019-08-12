#ifndef RFDEVICE_QXSW_TYPEDEF_H
#define RFDEVICE_QXSW_TYPEDEF_H

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qxsw/api/rfdevice_qxsw_typedef.h#1 $
  $DateTime: 2016/12/13 07:58:05 $
  $Author: mplcsds1 $
 
when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
09/24/14   px    Added QXSW EFS support
08/14/14   dbc   Add support for  PRX_DRX TX Path Swap 
04/01/14   dbc   Initial version

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

#define QXSW_MAX_SCRIPT_SIZE 10
#define RFC_DEFINED_RFFE_TRANSACTION_TIME 0
#define RFDEVICE_QXSW_MAX_REG_ADDR 0xFF

#define RFDEVICE_QXSW_EFS_DEBUG_PATH_STR "/qfe/qxsw/dev%d_rev%d"
#define RFDEVICE_QXSW_EFS_DEBUG_MAX_PATH_LENGTH 40

#define RFDEVICE_QXSW_REG_INVALID -1


/*==========================================================================*/
/* DATA STRUCTURES  */
/*==========================================================================*/

typedef struct
{
  const int16** data_tbl; /* int to denote invalid by -ve */
  const uint16** reg_tbl; /* uint16 for extended addressing */
} rfdevice_qxsw_ag_settings_type;

typedef struct
{
  const uint8* data_tbl;
  const uint8* reg_tbl;
} rfdevice_qxsw_efs_settings_type;

typedef struct
{
  uint8 gsm[RFCOM_NUM_GSM_BANDS];
  uint8 tdscdma[RFCOM_NUM_TDSCDMA_BANDS];
  uint8 cdma[RFM_CDMA_MAX_BAND];
  uint8 wcdma[RFCOM_NUM_WCDMA_BANDS];
  uint8 lte[RFCOM_NUM_LTE_BANDS];
} rfdevice_qxsw_port_info_type;

typedef struct
{
  boolean extended_cmd;  /*!< RFFE Indicator to use extended cmd or not */
  rf_hal_bus_resource_script_settings_type settings;
} rfdevice_qxsw_ag_cfg_type;

typedef struct
{
  /* PRX_DRX Path Swap */
  uint8                           xsw_prx_drx_path_script_size;
  rfdevice_qxsw_ag_settings_type  xsw_prx_drx_path;

  /* PRX Only Path */
  uint8                           xsw_prx_only_path_script_size;
  rfdevice_qxsw_ag_settings_type  xsw_prx_only_path;
    
  /* SRS Only */
  uint8                           xsw_srs_only_script_size;
  rfdevice_qxsw_ag_settings_type  xsw_srs_only;

  /* PRX_DRX TX Path Swap */
  uint8                           xsw_prx_drx_path_tx_script_size;
  rfdevice_qxsw_ag_settings_type  xsw_prx_drx_path_tx;

} rfdevice_qxsw_port_settings_type;

typedef struct
{
  /* Init */
  uint8                           xsw_init_script_size;
  rfdevice_qxsw_ag_settings_type  xsw_init;

  /* Trigger */
  uint8                           xsw_trigger_script_size;
  rfdevice_qxsw_ag_settings_type  xsw_trigger;
  
} rfdevice_qxsw_common_settings_type;


typedef struct
{ 
  /* Number of active ports */
  uint8 num_ports;
 
  /* Pointer to port data */
  rfdevice_qxsw_port_settings_type port_settings_ag;

  /* Pointer to port independent data */
  rfdevice_qxsw_common_settings_type common_settings_ag;

} rfdevice_qxsw_settings_type;


/* The following structure is used for debug/ OFF target */
typedef struct
{
  /* Flag to track if we are in debug mode */
  boolean debug_mode;
  /* Internal buffer showing chip data */
  uint8 *reg_data;
  /* Internal address buffer to store sequence of wrties */
  rf_buffer_intf *rf_buff;

} rfdevice_qxsw_settings_debug_type;

/*==========================================================================*/
/* Function Pointers  */
/*==========================================================================*/

typedef boolean (*qxsw_sleep_fn_ptr)( void );
typedef boolean (*qxsw_wakeup_fn_ptr)( void );
typedef void (*qxsw_convert_port_mapping_ptr)(uint8 input_port, uint8 ant_sel, uint8 *port_to_use);


typedef struct 
{ 
  qxsw_sleep_fn_ptr sleep;
  qxsw_wakeup_fn_ptr wakeup;
  qxsw_convert_port_mapping_ptr convert_port_map;    
}rfdevice_qxsw_func_tbl_type;

#endif /*RFDEVICE_QXSW_TYPEDEF */

