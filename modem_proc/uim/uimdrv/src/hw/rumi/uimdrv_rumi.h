#ifndef UIMDRV_RUMI_H
#define UIMDRV_RUMI_H

/*============================================================================
  FILE:        uimdrv_rumi.h

  OVERVIEW:    File contains RUMI specific driver code.

  DEPENDENCIES: N/A

                Copyright (c) 2013QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/hw/rumi/uimdrv_rumi.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

  when      who     what, where, why
------      ----    -----------------------------------------------------------
07/09/13    ak      Created    
============================================================================*/
/*===========================================================================
FUNCTION uimdrv_rumi_config_gpios

DESCRIPTION
  Configures the GPIOs connected to the V ENABLE and V SELECT lines.  
  For RUMI only.

===========================================================================*/
void uimdrv_rumi_config_gpios(uim_instance_enum_type uim_instance_id);

/*===========================================================================
FUNCTION uimdrv_rumi_vsel_off

DESCRIPTION
  Disables the V ENABLE line and sets the V SELECT line to 1.8v.  
  For RUMI only.
===========================================================================*/
void uimdrv_rumi_vsel_off(uim_instance_enum_type uim_instance_id);

/*===========================================================================
FUNCTION uimdrv_rumi_vsel_on_class_c

DESCRIPTION
  Turns on the V_ENABLE line and sets the V_SELECT line for 1.8v operation.
  For RUMI only.
===========================================================================*/
void uimdrv_rumi_vsel_on_class_c(uim_instance_enum_type uim_instance_id);

/*===========================================================================
FUNCTION uimdrv_rumi_vsel_on_class_b

DESCRIPTION
  Turns on the V_ENABLE line and sets the V_SELECT line for 3v operation.
  For RUMI only.
===========================================================================*/
void uimdrv_rumi_vsel_on_class_b(uim_instance_enum_type uim_instance_id);

#endif /* UIMDRV_RUMI_H */
