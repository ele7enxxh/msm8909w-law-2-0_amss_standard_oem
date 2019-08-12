#ifndef RFDEVICE_LOGICAL_COMPONENT_TYPES_H
#define RFDEVICE_LOGICAL_COMPONENT_TYPES_H
/*!
  @file
  rfdevice_logical_component_types.h

  @brief
  Type definitions of the classes of logical components in the system.
*/

/*===========================================================================

  Copyright (c) 2012,2013,2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_logical_component_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/25/14   vv      Added lna device support
02/11/14   yb      Added coupler device
08/13/13   kg      Change the order in which the devices get initialized 
07/26/13   as      Added new logical devie: RFDEVICE_ASD_TUNER_MANAGER
02/05/13   vb      Added new logical device: RFDEVICE_TUNER_MANAGER
11/26/12   sr      changed the last entry of the enum to 0xFFFFFFFF to force the 
                   compiler to treat the enum as int32 value to support rfc efs data.
11/06/12   bmg     Created

===========================================================================*/

typedef enum
{
  RFDEVICE_TRANSCEIVER,
  RFDEVICE_TRANSMITTER,
  RFDEVICE_RECEIVER,
  RFDEVICE_ASM,
  RFDEVICE_PAPM,
  RFDEVICE_PA,
  RFDEVICE_TUNER,
  RFDEVICE_TUNER_MANAGER,
  RFDEVICE_THERM,
  RFDEVICE_HDET,
  RFDEVICE_ASD_TUNER_MANAGER,
  RFDEVICE_BUFFER,
  RFDEVICE_COUPLER,
  RFDEVICE_XSW,
  RFDEVICE_LNA,
  RFDEVICE_TYPE_MAX_NUM,
  RFDEVICE_TYPE_INVALID = 0x7FFFFFFF
} rfdevice_type_enum_type;

typedef rfdevice_type_enum_type rfdevice_logical_component_type;

#endif /* RFDEVICE_LOGICAL_COMPONENT_TYPES_H */

