
#ifndef RFDEVICE_ASM_CXA4422GC_2_TS1_DATA_AG
#define RFDEVICE_ASM_CXA4422GC_2_TS1_DATA_AG


#ifdef __cplusplus
extern "C" {
#endif
/*
WARNING: This file is auto-generated.

Generated using: asm_autogen.pl
Generated from:  v2.3.149 of RFDevice_ASM.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2009, 2010, 2011, 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //source/qcom/qct/modem/rfdevice/asm/main/latest/etc/asm_autogen.pl#11 n

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfdevice_asm_data.h"

class rfdevice_asm_cxa4422gc_2_ts1_data_ag:public rfdevice_asm_data
{
public:
    static rfdevice_asm_data * get_instance();
    boolean settings_data_get( rfdevice_asm_cfg_params_type *cfg, 
                               rfdevice_asm_reg_settings_type *settings);
    boolean sequence_data_get( rfdevice_asm_cfg_params_type *cfg, 
                               rfdevice_asm_cmd_seq_type *cmd_seq);
    boolean device_info_get( rfdevice_asm_info_type *asm_info );
	boolean get_temperature_lut( int16 **lut_addr, uint8 *lut_size );
    ~rfdevice_asm_cxa4422gc_2_ts1_data_ag(void);  /*  Destructor  */
protected:
  rfdevice_asm_cxa4422gc_2_ts1_data_ag(void);  /*  Constructor  */

private:
  static rfdevice_asm_data *rfdevice_asm_cxa4422gc_2_ts1_data_ptr;

};


#ifdef __cplusplus
}
#endif



#endif

