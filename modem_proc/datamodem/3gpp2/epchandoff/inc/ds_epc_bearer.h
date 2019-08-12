#ifndef DS_EPC_BEARER_H
#define DS_EPC_BEARER_H
/*===========================================================================

                      DS_EPC_SYS_BEARER.H
                   
DESCRIPTION
  

EXTERNALIZED FUNCTIONS

 Copyright (c) 2009-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_bearer.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/01/11   fjia    Create the module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "sys.h"
#include "ps_iface_defs.h"
#include "ds_epc_pdn_ctl.h"

typedef enum
{
  SYS_SERVICE_INACTIVE = 0,
  SYS_SERVICE_ACTIVE
} sys_service_status;

extern ps_iface_ioctl_bearer_tech_changed_type ds_epc_last_bearer_tech_change;
extern sys_service_status system_service_status ;

/*===========================================================================
FUNCTION      DS_EPC_SYTEM_CHANGE

DESCRIPTION   EPC gets notification that system has been changed.
              The execution of this function is protected by critical section.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_system_change
( 
  sys_sys_mode_e_type  new_mode,
  uint32               so_mask, 
  uint32               rat_mask
);

/*===========================================================================
FUNCTION      DS_EPC_GET_LAST_BEARER_TECH_CHANGE_INFO

DESCRIPTION   Gets last bearer tech change info

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_get_last_bearer_tech_change_info
(
  ps_iface_ioctl_bearer_tech_changed_type  * bearer_tech_change_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_LAST_BEARER_TECH_CHANGE_REPORT

DESCRIPTION   Generate Bearer tech notification for bearer tech change. Also
              resync the old and new bearer tech.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_last_bearer_tech_change_report(void);

/*===========================================================================
FUNCTION      DS_EPC_REPORT_IFACE_BEARER_TECH_CHANGE

DESCRIPTION   Generate Bearer tech notification for bearer tech change on a
              particular EPC iface. Also resync the old and new bearer tech.

DEPENDENCIES  EPC iface must be UP or ROUTABLE

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_report_iface_bearer_tech_change
(
  ds_epc_cb_type       * epc_iface_ctrl_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_GET_PDN_RAT_BY_BEARER_TECH_INFO

DESCRIPTION   Translate the bearer tech info to EPC PDN rat type.
              Using the old PS bearer tech excluding IWLAN.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_epc_pdn_rat_type ds_epc_get_pdn_rat_by_bearer_tech_info
(
  ps_iface_network_type    tech_type,
  uint32                   rat_mask,
  uint32                   so_mask
);

/*===========================================================================
FUNCTION      DS_EPC_CONVERT_PDN_RAT_BY_DS_SYS_RAT

DESCRIPTION   Translate the new bearer tech info - ds_sys_rat_ex_enum_type 
              to EPC PDN rat type.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_epc_pdn_rat_type ds_epc_convert_pdn_rat_by_ds_sys_rat
(
  ds_sys_rat_ex_enum_type        rat_value
);
#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_BEARER_H*/
