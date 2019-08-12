/*!
   @file
   rfdevice_asm_data.cpp

   @brief
   This file contains implementation the rfdevice_asm_data class,
   which will be used by the ASM device driver configure the vendor specific ASM device.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:58:09 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_asm/src/rfdevice_asm_data.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
06/18/14   aca     therm support
01/28/13   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfdevice_asm_data.h"


// Constructor 
rfdevice_asm_data::rfdevice_asm_data()
{
  // do nothing for now
}

// Destructor
rfdevice_asm_data::~rfdevice_asm_data()
{
  // do nothing for now
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  implement this API to support the ASM device command sequence.

*/
boolean rfdevice_asm_data::sequence_data_get( rfdevice_asm_cfg_params_type *cfg, 
                                             rfdevice_asm_cmd_seq_type *cmd_seq)
{
  // to support the ASM cmd Sequence, implement this API in derived clases.
  return FALSE;
}

boolean rfdevice_asm_data::get_temperature_lut(int16 **lut_addr, uint8 *lut_size)
{
  return FALSE;
}
