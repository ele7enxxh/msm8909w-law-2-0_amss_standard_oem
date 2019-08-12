#ifndef RFDEVICE_PAPM_DATA
#define RFDEVICE_PAPM_DATA

/*!
  @file
  rfdevice_papm_data.h

  @brief
  This file contains the class definition for the rfdevice_papm_data,
  which serves as base class for the vendor specific PAPM settings data and configuration.

*/

/*===========================================================================

Copyright (c) 2011-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_papm/api/rfdevice_papm_data.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------   
11/19/14   vv     rfdevice_reg_dump() API  for debug
10/23/14   vv      Added QFE3100 support
10/30/13   sb      Added support for delay after standby
03/22/13   sr      init version
============================================================================*/
#include "comdef.h"
#include "rfa.h" 
#include "rfdevice_papm.h"
#include "rfdevice_papm_types.h"
#include "rfdevice_chip_ids.h"


#define RFDEVICE_PAPM_INVALID_PORT    0xFF
#define RFDEVICE_PAPM_MAX_AG_REG_SIZE 50
#define RFDEVICE_PAPM_MFG_ID_GET(x) (((x) >> 22) & 0x3FF)
#define RFDEVICE_PAPM_PRD_ID_GET(x) (((x) >> 14) & 0xFF)
#define RFDEVICE_PAPM_PORT_NUM_GET(x) ((x) & 0xFF)

#define RFDEVICE_PAPM_APT_VOUT_MIN	 400
#define RFDEVICE_PAPM_APT_VOUT_MAX	 4200
#define RFDEVICE_PAPM_APT_VOUT_RES	 30
#define RFDEVICE_RFFE_TRANS_TIME_10X 13

#define RFDEVICE_PAPM_HW_PART_MASK       0x01

class rfdevice_papm_data : public rfa
{
public:
    virtual boolean settings_data_get( rfdevice_papm_cfg_params_type *cfg, 
                                       rfdevice_reg_settings_type *settings) = 0;

    virtual boolean sequence_data_get( rfdevice_papm_cfg_params_type *cfg, 
                                       rfdevice_papm_cmd_seq_type *cmd_seq);

    virtual boolean timing_info_get( rfdevice_papm_timing_info_type *papm_timing_info );

    virtual boolean get_ET_voltage_info( rfdevice_papm_misc_info* ET_voltage_info );
    // Destructor
    virtual ~rfdevice_papm_data();

protected:
  rfdevice_papm_data(void);  /*  Constructor  */

private:
};

rfdevice_papm_data* rfdevice_papm_data_create (uint16 mfg_id, uint8 prd_id, uint8 prd_rev, uint8 hw_rev, uint8 hw_part);
boolean get_hw_rev_hw_part_reg_addr(rfdevice_id_enum_type rf_device_id,uint16* hw_rev_reg, uint16* hw_part_reg);

#endif


