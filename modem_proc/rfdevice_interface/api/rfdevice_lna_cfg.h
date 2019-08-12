#ifndef RFDEVICE_LNA_CFG_H
#define RFDEVICE_LNA_CFG_H
/*!
   @file
   rfdevice_lna_cfg.h

   @brief


*/

/*==============================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_lna_cfg.h#1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
12/02/14   vv    Initial version
==============================================================================*/

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_type_defs.h"

#ifdef __cplusplus
class rfdevice_lna_cfg: public rfa
{
public:

  rfdevice_lna_cfg();

  virtual ~rfdevice_lna_cfg();

  rfm_device_enum_type rfm_device;
  rfcom_mode_enum_type mode;
  rfcom_band_type_u band;
  rfdevice_lna_trigger_enum_type  trigger_type;
  rfdevice_lna_gain_enum_type lna_gain;
  int8 system_gain;

};

#endif
#endif /* RFDEVICE_LNA_CFG_H */


