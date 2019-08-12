#ifndef DS_3GPP_DEVICE_EV_HDLR_H
#define DS_3GPP_DEVICE_EV_HDLR_H
/*===========================================================================

            DS 3GPP ROAMING HDLR

===========================================================================*/
/*!
  @file
  ds_3gpp_roaming_hdlr.h

  @brief
  This header file externalizes 3GPP Device Handler functionality

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/        
/*==============================================================================

  Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_device_ev_hdlr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
1/21/14    vs      Initial File
==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ds3gsubsmgr.h"
#include "dstask_v.h"
#include "ds3gdevmgr.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_INIT

DESCRIPTION   This function initializes the device event handler module
 
PARAMETERS    None
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gpp_device_ev_hdlr_init
(
  void
);

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_INIT

DESCRIPTION   This function initializes device event handler parameters 
              after power up is complete 
 
PARAMETERS    None
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gpp_device_ev_hdlr_init_after_powerup
(
  void
);

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_PROCESS_DATA_SETTINGS

DESCRIPTION   This function processes the data settings
 
PARAMETERS    Device Settings
              Boolean Pointer set to True if the attach apn is unblocked
 
DEPENDENCIES  None

RETURN VALUE  TRUE, if Data is disabled after processing the settings 
              FALSE, if Data is enabled 

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gpp_device_ev_hdlr_process_data_settings
(
  ds3gdevmgr_device_settings_info_type *device_settings_p,
  boolean*                              is_attach_apn_ptr
);

/*===========================================================================
FUNCTION      DS_3GPP_DEVICE_EV_HDLR_IS_APN_ALLOWED

DESCRIPTION   This function checks whether PDN can be brought up for this APN
 
PARAMETERS    1. APN name 
              2. Subs id 
 
DEPENDENCIES  None

RETURN VALUE  TRUE, if PDN can be brought up
              FALSE, otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gpp_device_ev_hdlr_is_apn_allowed
(
  char                           *apn_p,
  sys_modem_as_id_e_type          subs_id,
  ps_iface_net_down_reason_type   *down_reason_p
);

#endif /* DS_3GPP_DEVICE_EV_HDLR_H */
