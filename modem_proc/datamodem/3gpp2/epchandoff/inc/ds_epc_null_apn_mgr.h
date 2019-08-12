#ifndef DS_EPC_NULL_APN_MGR_H
#define DS_EPC_NULL_APN_MGR_H
/*===========================================================================

                      DS_EPC_NULL_APN_MGR
                   
DESCRIPTION
  DS EPC NULL APN Manager module.
 
  This module handles NULL APN resolution feature. EPC reports NULL resolved APN
  info to through PS SYS event. The NULL resolved APN is shared with all EPC
  techs - iWLAN, LTE and eHRPD.

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_null_apn_mgr.h#1 $
  $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/03/2014  fj     Initial version.
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_EPC_HANDOFF
#include "sys.h"
#include "rex.h"

#include "modem_mem.h"

#include <stringl/stringl.h>
#include "ds_epc_null_apn_mgr.h"
#include "ds_epc_pdn_ctl.h"
#include "ds_sys.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
#define   DS_EPC_NULL_APN_IP_NONE_MASK          0x00
#define   DS_EPC_NULL_APN_IP_V4_MASK            0x01
#define   DS_EPC_NULL_APN_IP_V6_MASK            0x02

/*-------------------------------------------------------------------------
Define NULL resolved APN type 
 
  valid:          Indicate if NULL resolved APN info is valid or not. 
  apn_str:        NULL resolved APN name. 
  apn_len:        NULL resolved APN length.
  ip_type_mask:   indicate NULL resolved APN is UP on which IP type.
 
--------------------------------------------------------------------------*/
typedef struct
{
  char                apn_str[DS_SYS_MAX_APN_LEN];
  uint16              apn_len;
  uint8               ip_type_mask;
} ds_epc_null_apn_info_type;

/*===========================================================================
FUNCTION      DS_EPC_NULL_APN_MGR_SET

DESCRIPTION   Set the NULL resolved APN on a given IP type. 
              If the new NULL resolved APN is different from the cached
              NULL resolved APN, send the PS SYS indication.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_null_apn_mgr_set
(
  char    *apn_str,
  uint16   apn_len,
  uint8    ip_type,
  uint32   subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_NULL_APN_MGR_UNSET

DESCRIPTION   Unset the NULL resolved APN on a given IP type. 
              If both IPv4 and IPv6 IP type are unset, NULL APN manager
              clear the NULL APN cache and send PS SYS event.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_null_apn_mgr_unset
(
  uint8    ip_type,
  uint32   subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_NULL_APN_MGR_IS_NULL_RESOLVED_APN

DESCRIPTION   Check if the passed APN name is NULL resolved APN.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_null_apn_mgr_is_null_resolved_apn
(
  char           *apn_name,
  uint16          apn_len,
  uint32          subs_id
);

#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_NULL_APN_MGR_H*/
