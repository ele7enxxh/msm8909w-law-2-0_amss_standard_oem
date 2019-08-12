#ifndef RFDEVICE_QTHERM_TYPEDEF_H
#define RFDEVICE_QTHERM_TYPEDEF_H

/*=============================================================================

Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qtherm/api/rfdevice_qtherm_typedef.h#1 $
  $DateTime: 2016/12/14 01:58:16 $
  $Author: mplcsds1 $

when         who   what, where, why
--------   ---   ---------------------------------------------------------------
09/05/14   vv    Initial version

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

/*==========================================================================*/
/* DATA STRUCTURES  */
/*==========================================================================*/


typedef struct
{
  /* Number of active ports */
  uint8 num_ports;

} rfdevice_qtherm_settings_type;


typedef struct
{
  boolean extended_cmd;  /*!< RFFE Indicator to use extended cmd or not */
  rf_hal_bus_resource_script_settings_type settings;
} rfdevice_qtherm_ag_cfg_type;



/*==========================================================================*/
/* Function Pointers  */
/*==========================================================================*/

typedef void (*qtherm_therm_setup_fn_ptr)(uint8 bus_id, uint8 slave_id);

typedef void (*qtherm_therm_cleanup_fn_ptr)(uint8 bus_id, uint8 slave_id);

typedef void (*qtherm_therm_read_fn_ptr)(uint8 bus_id, uint8 slave_id,int16* therm_degrees_C);


typedef struct
{

  qtherm_therm_setup_fn_ptr therm_setup;

  qtherm_therm_cleanup_fn_ptr therm_cleanup;

  qtherm_therm_read_fn_ptr read_therm;

} rfdevice_qtherm_func_tbl_type;

#endif /*RFDEVICE_QTHERM_TYPEDEF */

