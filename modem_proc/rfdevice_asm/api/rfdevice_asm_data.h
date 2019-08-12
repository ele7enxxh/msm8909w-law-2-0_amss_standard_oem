#ifndef RFDEVICE_ASM_DATA
#define RFDEVICE_ASM_DATA

/*!
  @file
  rfdevice_asm_data.h

  @brief
  This file contains the class definition for the rfdevice_asm_data,
  which serves as base class for the vendor specific ASM settings data and configuration.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_asm/api/rfdevice_asm_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------   
06/18/14   aca     therm support
02/19/13   sr      added device_info_get() api.
01/28/13   sr      init version
============================================================================*/
#include "comdef.h"
#include "rfa.h" 
#include "rfdevice_asm_types.h"

class rfdevice_asm_data : public rfa
{
public:
    virtual boolean device_info_get( rfdevice_asm_info_type *asm_info ) = 0;
    virtual boolean settings_data_get( rfdevice_asm_cfg_params_type *cfg, 
                                       rfdevice_asm_reg_settings_type *settings) = 0;

    virtual boolean sequence_data_get( rfdevice_asm_cfg_params_type *cfg, 
                                       rfdevice_asm_cmd_seq_type *cmd_seq);
									   
	virtual boolean get_temperature_lut(int16 **lut_addr,uint8 *lut_size );

    // Destructor
    virtual ~rfdevice_asm_data();

protected:
  rfdevice_asm_data(void);  /*  Constructor  */

private:
};


#endif


