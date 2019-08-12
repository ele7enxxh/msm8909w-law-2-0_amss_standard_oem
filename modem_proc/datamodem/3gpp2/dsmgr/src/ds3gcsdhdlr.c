/*===========================================================================

               D A T A   S E R V I C E S   C S D   H A N D L E R

GENERAL DESCRIPTION
  Data service's CS Data handler.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2010 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gcsdhdlr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/13   var     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#include "ds3gcsdhdlr.h"
#include "ds3gstkproc.h"

/*===========================================================================

                    PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                    GLOBAL VARIABLES

===========================================================================*/
static ds3gcsdhdlr_radio_info_type
      ds3gcsdhdlr_radio_info[DS3GSUBSMGR_SUBS_ID_COUNT][DS3G_RADIO_INDEX_MAX] = {{{-1}}};

/*===========================================================================

                    INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GCSDHDLR_INIT

DESCRIPTION   This function intializes DS3G CS Data handler module.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcsdhdlr_init(void)
{
  ds3g_radio_index_type       radio;
  ds3gsubsmgr_subs_id_e_type  subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize radio information
  -------------------------------------------------------------------------*/
  memset(&ds3gcsdhdlr_radio_info,
         0,
         (sizeof(ds3gcsdhdlr_radio_info_type) *
          DS3GSUBSMGR_SUBS_ID_COUNT * DS3G_RADIO_INDEX_MAX));

  for(subs_id = DS3GSUBSMGR_SUBS_ID_MIN; subs_id < DS3GSUBSMGR_SUBS_ID_MAX;
      subs_id++)
  {
    for(radio = DS3G_RADIO_INDEX_MIN; radio < DS3G_RADIO_INDEX_MAX; radio++)
    {
      ds3gcsdhdlr_radio_info[subs_id][radio].sys_mode
        = ds3gstkproc_convert_radio_index_to_sys_mode(radio);
      ds3gcsdhdlr_radio_info[subs_id][radio].srv_status= SYS_SRV_STATUS_NO_SRV;
      ds3gcsdhdlr_radio_info[subs_id][radio].srv_domain= SYS_SRV_DOMAIN_NO_SRV;
    }
  }
  return;
} /* ds3gcsdhdlr_init() */

/*===========================================================================
FUNCTION      DS3GCSDHDLR_EVENT_NOTIFY

DESCRIPTION   This external function called by clients to notify the DS3G CSD
              Hdlr module.

PARAMETERS    event_info: structure containing event information

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcsdhdlr_event_notify
(
  ds3gcsdhdlr_radio_info_type  *event_info,
  ds3gsubsmgr_subs_id_e_type    subs_id
)
{
  ds3g_radio_index_type index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(event_info == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Null pointer passed");
    return;
  }

  /*-------------------------------------------------------------------------
    Validate subs_id
  -------------------------------------------------------------------------*/
  if(ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id) == FALSE)
  {
    return;
  }

  DATA_MSG4(MSG_LEGACY_LOW,
            "ds3gcsdhdlr_event_notify subs_id:%d sys_mode:%d srv_status:%d "
            "srv_domain:%d",
            subs_id,
            event_info->sys_mode,
            event_info->srv_status,
            event_info->srv_domain);

  /*-------------------------------------------------------------------------
    Validate input parameters
  -------------------------------------------------------------------------*/
  if(event_info->sys_mode   <  SYS_SYS_MODE_NO_SRV   ||
     event_info->sys_mode   >= SYS_SYS_MODE_MAX      ||
     event_info->srv_status <  SYS_SRV_STATUS_NO_SRV ||
     event_info->srv_status >= SYS_SRV_STATUS_MAX    ||
     event_info->srv_domain <  SYS_SRV_DOMAIN_NO_SRV ||
     event_info->srv_domain >= SYS_SRV_DOMAIN_MAX)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "Invalid parameters");
    return;
  }

  /*-------------------------------------------------------------------------
    Update cache with event info
  -------------------------------------------------------------------------*/
  index = ds3gstkproc_convert_sys_mode_to_radio_index(event_info->sys_mode);
  if(index != DS3G_RADIO_INDEX_MAX)
  {
    ds3gcsdhdlr_radio_info[subs_id][index].srv_status = event_info->srv_status;
    ds3gcsdhdlr_radio_info[subs_id][index].srv_domain = event_info->srv_domain;
  }

  return;
}/* ds3gcsdhdlr_event_notify() */

/*===========================================================================
FUNCTION      DS3GCSDHDLR_GET_PREFERRED_CSD_MODE

DESCRIPTION   This function returns the mode in which cs domain is registered.

PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  preferred CS domain mode

SIDE EFFECTS  None.
===========================================================================*/
sys_sys_mode_e_type ds3gcsdhdlr_get_preferred_csd_mode
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds3g_radio_index_type            indx = DS3G_RADIO_INDEX_MAX;
  sys_sys_mode_e_type              sys_mode = SYS_SYS_MODE_NO_SRV;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate input parameters
  -------------------------------------------------------------------------*/
  if(ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id) == FALSE)
  {
    return SYS_SYS_MODE_NO_SRV;
  }

  for(indx = DS3G_RADIO_INDEX_MIN; indx < DS3G_RADIO_INDEX_MAX; indx++)
  {
    if((ds3gcsdhdlr_radio_info[subs_id][indx].srv_status
          == SYS_SRV_STATUS_SRV) &&
       ((ds3gcsdhdlr_radio_info[subs_id][indx].srv_domain
          == SYS_SRV_DOMAIN_CS_ONLY) ||
        (ds3gcsdhdlr_radio_info[subs_id][indx].srv_domain
          == SYS_SRV_DOMAIN_CS_PS)))
    {
      sys_mode = ds3gcsdhdlr_radio_info[subs_id][indx].sys_mode;
      break;
    }
  }

  DATA_MSG1(MSG_LEGACY_LOW, "ds3gcsdhdlr_get_preferred_csd_mode sys_mode:%d",
            sys_mode);
  return sys_mode;
}/* ds3gcsdhdlr_get_preferred_csd_mode() */

