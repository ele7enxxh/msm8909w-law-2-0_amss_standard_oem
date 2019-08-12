#ifndef DS3GCSDHDLR_H
#define DS3GCSDHDLR_H
/*===========================================================================

                D A T A   S E R V I C E S   C S D   H A N D L E R
 
                              H E A D E R   F I L E

DESCRIPTION
  This file contains functions for handling Data service's CS Data handler.
  This header file is intended to be used internal to the 3G Dsmgr module only.

  Copyright (c) 2001-2012 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gcsdhdlr.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/13   var     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "sys.h"

#include "ds3gsubsmgr.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  DS3G CSD Hdlr Radio Info Structure
---------------------------------------------------------------------------*/
typedef struct
{
  sys_sys_mode_e_type     sys_mode;       /* System mode */
  sys_srv_status_e_type   srv_status;     /* Service status */
  sys_srv_domain_e_type   srv_domain;     /* Service domain */
}ds3gcsdhdlr_radio_info_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GCSDHDLR_INIT

DESCRIPTION   This function intializes DS3G CS Data handler module.
  
PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gcsdhdlr_init(void);

/*===========================================================================
FUNCTION      DS3GCSDHDLR_EVENT_NOTIFY

DESCRIPTION   This external function called by clients to notify the DS3G CSD 
              Hdlr module.
  
PARAMETERS    event_info: structure containing event information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gcsdhdlr_event_notify
(
  ds3gcsdhdlr_radio_info_type  *event_info,
  ds3gsubsmgr_subs_id_e_type    subs_id
);

/*===========================================================================
FUNCTION      DS3GCSDHDLR_GET_PREFERRED_CSD_MODE

DESCRIPTION   This function returns the mode in which cs domain is registered.
  
PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3gcsdhdlr_get_preferred_csd_mode
(
  ds3gsubsmgr_subs_id_e_type  subs_id
);


#endif /* DS3GCSDHDLR_H */

