
#ifndef RFDEVICE_EFS_ASM_DATA
#define RFDEVICE_EFS_ASM_DATA


#ifdef __cplusplus
extern "C" {
#endif
  /*
  WARNING: This file is auto-generated.

  Generated using: asm_autogen.pl
  Generated from:  3.1.121 of RFDevice_ASM.xlsm
  */

  /*=============================================================================

            R F C     A U T O G E N    F I L E

  GENERAL DESCRIPTION
    This file is auto-generated and it captures the configuration of the RF Card.

  Copyright (c) 2009 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_asm/src/rfdevice_efs_asm_data.h#1 $


  =============================================================================*/

  /*=============================================================================
                             INCLUDE FILES
  =============================================================================*/
#include "comdef.h"

#include "rfdevice_asm_data.h"

  typedef struct
  {
    uint16 mfg_id;
    uint8 prd_id;
    uint8 prd_rev;
    uint8 max_ports;
  } dev_info_s;
  typedef struct
  {
    rfdevice_asm_reg_settings_type init;
    rfdevice_asm_reg_settings_type on;
    rfdevice_asm_reg_settings_type off;
    rfdevice_asm_reg_settings_type trigger;
  } asm_data_s;

  typedef struct
  {
    dev_info_s device_info;
    asm_data_s asm_data;
  } efs_asm_s;


  class rfdevice_efs_asm_data:public rfdevice_asm_data
  {
  public:
    static rfdevice_asm_data * get_instance(uint8* data);
    boolean settings_data_get( rfdevice_asm_cfg_params_type *cfg,
                               rfdevice_asm_reg_settings_type *settings);
    boolean device_info_get( rfdevice_asm_info_type *asm_info );
    ~rfdevice_efs_asm_data(void);  /*  Destructor  */
  protected:
    rfdevice_efs_asm_data(uint8* data);  /*  Constructor  */

  private:
    efs_asm_s efs_asm;

    uint8* efs_asm_ptr;

  };



#ifdef __cplusplus
}
#endif



#endif

