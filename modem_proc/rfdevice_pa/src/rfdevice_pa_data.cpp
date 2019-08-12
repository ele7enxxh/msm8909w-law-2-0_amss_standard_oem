/*!
   @file
   rfdevice_pa_data.cpp

   @brief
   This file contains implementation the rfdevice_pa_data class,
   which will be used by the PA device driver configure the vendor specific PA device.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:17 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_pa/src/rfdevice_pa_data.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
01/16/13   sr      Added pa command sequence request support.
01/03/13   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfdevice_pa_data.h"


// Constructor 
rfdevice_pa_data::rfdevice_pa_data()
{
  // do nothing for now
}

// Destructor
rfdevice_pa_data::~rfdevice_pa_data()
{
  // do nothing for now
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  implement this API to support the PA device command sequence.

*/
boolean rfdevice_pa_data::sequence_data_get( rfdevice_pa_cfg_params_type *cfg, 
                                             rfdevice_pa_cmd_seq_type *cmd_seq)
{
  // to support the PA cmd Sequence, implement this API in derived clases.
  return FALSE;
}


boolean rfdevice_pa_data::get_temperature_lut(int16 **lut_addr, uint8 *lut_size)
{
  return FALSE;
}