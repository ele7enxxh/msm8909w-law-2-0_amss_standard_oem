#ifndef RFDEVICE_ASM_CFG_H
#define RFDEVICE_ASM_CFG_H
/*!
   @file
   rfdevice_asm_cfg.h

   @brief


*/

/*==============================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_asm_cfg.h#1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
05/05/14   sn    Initial version
==============================================================================*/

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_type_defs.h"

#ifdef __cplusplus
class rfdevice_asm_cfg: public rfa
{
public:

  rfdevice_asm_cfg();

  rfdevice_asm_cfg(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rfcom_band_type_u band);

  virtual ~rfdevice_asm_cfg();

  rfm_device_enum_type rfm_device;
  rfcom_mode_enum_type mode;
  rfcom_band_type_u band;
  rfdevice_asm_rx_tx_type asm_rx_tx;
  rfdevice_asm_cell_type asm_cell;
  rfdevice_asm_trigger_enum_type  trigger_type;

};

#endif
#endif /* RFDEVICE_PA_CFG_H */


