#ifndef RFDEVICE_PA_DATA
#define RFDEVICE_PA_DATA

/*!
  @file
  rfdevice_pa_data.h

  @brief
  This file contains the class definition for the rfdevice_pa_data,
  which serves as base class for the vendor specific PA settings data and configuration.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_pa/api/rfdevice_pa_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------   
05/28/13   vs      API support for PA therm
02/19/13   sr      added device_info_get api to pa data.
01/16/13   sr      Added pa command sequence request support.
01/03/13   sr      init version
============================================================================*/
#include "comdef.h"
#include "rfa.h" 
#include "rfdevice_pa_types.h"

class rfdevice_pa_data : public rfa
{
public:
    virtual boolean device_info_get( rfdevice_pa_info_type *pa_info ) = 0;

    virtual boolean settings_data_get( rfdevice_pa_cfg_params_type *cfg, 
                                       rfdevice_pa_reg_settings_type *settings) = 0;

    virtual boolean sequence_data_get( rfdevice_pa_cfg_params_type *cfg, 
                                       rfdevice_pa_cmd_seq_type *cmd_seq);

    virtual boolean get_temperature_lut(int16 **lut_addr,uint8 *lut_size );

    // Destructor
    virtual ~rfdevice_pa_data();

protected:
  rfdevice_pa_data(void);  /*  Constructor  */

private:
};


#endif


