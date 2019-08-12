#ifndef DS707_SO_JCDMA_H
#define DS707_SO_JCDMA_H
/*===========================================================================

                          D S 7 0 7 _ J C D M A _ S O
GENERAL DESCRIPTION

 Copyright (c) 2002-2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_jcdma.h_v   1.4   23 Oct 2002 10:09:26   sramacha  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/jcdma/inc/ds707_so_jcdma.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
11/07/05   as/vrk  Added definitions jcdma async set page rsp and
                   jcdma g3fax set page rsp to remove compile warnings
02/04/04   ak      Extern ds707_so_jcdma_init().
10/28/03   ak      Added extern for func to set page_rsp
10/22/02   rsl     Updated FEATURE_JCDMA_DS_1X to FEATURE_JCDMA_1X
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/10/02   ak      Updated for JCDMA M51/1X.
02/20/02   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "ds707.h"
#include "ds707_cback.h"

/*===========================================================================
                        FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKT_SET_PAGE_RSP

DESCRIPTION   JCDMA Pkt SO's

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_pkt_set_page_rsp(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKT_SET__STATE

DESCRIPTION   Enables/disables packet service options in SNM, based on
              current settings.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_pkt_set_state(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKT_GET_ORIG

DESCRIPTION   Returns the packet service option to originate with.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_so_type ds707_so_jcdma_pkt_get_orig
(
  ds707_rateset_type         rateset
);

#ifdef FEATURE_DATA_IS707_ASYNC
/*===========================================================================
FUNCTION      DS707_SO_JCDMA_ASYNC_SET_STATE

DESCRIPTION   Lookup the state of each async data service option based on the
              current service option set in use. Enable or disable each async
              data service option as per the table.

DEPENDENCIES  Uses the global variable ds_atcop_qcso_val to determine the
              current setting of the service option set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_async_set_state(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_ASYNC_SET_PAGE_RSP

DESCRIPTION   JCDMA Pkt SO's

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_async_set_page_rsp(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_ASYNC_GET_ORIG

DESCRIPTION   Returns the async service option to originate with.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_so_type ds707_so_jcdma_async_get_orig
(
  ds707_rateset_type         rateset
);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_VERIFY_INC_ASYNC_FAX_SO

DESCRIPTION   Given an SO, returns TRUE or FALSE, indicating if the SO
              should be accepted or rejected.  

DEPENDENCIES  None.

RETURN VALUE  TRUE - accept SO.   FALSE - Reject SO

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_so_jcdma_verify_inc_async_fax_so
(
  ds707_so_type     inc_so
);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_G3FAX_SET_STATE

DESCRIPTION   Lookup the state of each g3 fax service option based on the
              current Service Option Set in use. Enable or disable each G3
              fax service option as per the table.

DEPENDENCIES  Uses the global variable ds_atcop_qcso_val to determine the
              current setting of the service option set.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_g3fax_set_state(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_G3FAX_SET_PAGE_RSP

DESCRIPTION   JCDMA FAX SO's

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_g3fax_set_page_rsp(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_G3FAX_GET_ORIG

DESCRIPTION   Returns the G3/FAX service option to originate with.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds707_so_type ds707_so_jcdma_g3fax_get_orig
(
  ds707_rateset_type         rateset
);

#endif /* FEATURE_DATA_IS707_ASYNC */

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_VERIFY_INC_PKT_SO

DESCRIPTION   Given an SO, returns TRUE or FALSE, indicating if the SO
              should be accepted or rejected.  

DEPENDENCIES  None.

RETURN VALUE  TRUE - accept SO.   FALSE - Reject SO

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_so_jcdma_verify_inc_pkt_so
(
  ds707_so_type     inc_so
);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_INIT

DESCRIPTION   Initialie jcdma module.  Register for 707 pkt events.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_init(void);

/*===========================================================================
FUNCTION      DS707_SO_JCDMA_PKTI_CBACK

DESCRIPTION   Callback for 707 pkt phys link events

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_jcdma_pkti_cback
(
  ds707_cback_event_type              event,
  const ds707_cback_event_info_type        *event_info_ptr
);

#endif /* DS707_SO_JCDMA_H */

