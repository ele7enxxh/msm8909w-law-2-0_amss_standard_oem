
#ifndef RFDEVICE_PA_RFMD_7389_DATA_AG
#define RFDEVICE_PA_RFMD_7389_DATA_AG


#ifdef __cplusplus
extern "C" {
#endif
/*
WARNING: This file is auto-generated.

Generated using: pa_autogen.pl
Generated from:  4.0.60 of RFDevice_PA.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2009 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_pa/src/rfdevice_pa_rfmd_7389_data_ag.h#1 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfdevice_pa_data.h"

class rfdevice_pa_rfmd_7389_data_ag:public rfdevice_pa_data
{
public:
    static rfdevice_pa_data * get_instance();
    boolean settings_data_get( rfdevice_pa_cfg_params_type *cfg, 
                               rfdevice_pa_reg_settings_type *settings);
    boolean sequence_data_get( rfdevice_pa_cfg_params_type *cfg, 
                               rfdevice_pa_cmd_seq_type *cmd_seq);
    boolean device_info_get( rfdevice_pa_info_type *pa_info );
    boolean get_temperature_lut( int16 **lut_addr, uint8 *lut_size );
    ~rfdevice_pa_rfmd_7389_data_ag(void);  /*  Destructor  */
protected:
  rfdevice_pa_rfmd_7389_data_ag(void);  /*  Constructor  */

private:
  static rfdevice_pa_data *rfdevice_pa_rfmd_7389_data_ptr;

};


#ifdef __cplusplus
}
#endif



#endif

