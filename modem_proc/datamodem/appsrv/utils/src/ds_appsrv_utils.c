/*===========================================================================

                           DS_APPSRV_UTILS.C

DESCRIPTION
   
   Collection of utility functions being used by various modules in APPSRV.
                
EXTERNALIZED FUNCTIONS

 Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================
                       EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/appsrv/utils/src/ds_appsrv_utils.c#1 $
  $Author: mplcsds1 $  $DateTime: 2016/12/13 08:00:03 $
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/17/15   hr      Initial version.
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "data_msg.h"
#include "datamodem_variation.h"
#include "ds_appsrv_utils.h"
#include <stringl/stringl.h>
#include "ds3geventmgr.h"
#include "ps_sys.h"
#include "ds_andsf_api.h"
#include "modem_mem.h"
#include "ds_appsrv_task.h"

/*===========================================================================
                       LOCAL VARIABLE DEFINITIONS
===========================================================================*/
static const char ds_appsrv_nv_conf_efs_path[] = 
"/nv/item_files/data/appsrv/wqe_prof_wait_timer\n";

/*===========================================================================
                       LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS_APPSRV_NV_REFRESH_EV_HDLR

DESCRIPTION   Handles MCFG NV refresh event

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ds_appsrv_nv_refresh_ev_hdlr
(
  uint32 *msg
)
{
  ds_sys_subscription_enum_type subs_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  subs_id = (ds_sys_subscription_enum_type)(*msg);

  /*------------------------------------------------------------------------
    Reset and re-initialize ANDSF
  ------------------------------------------------------------------------*/
  ds_andsf_nv_refresh(subs_id);

  modem_mem_free((void*)msg, MODEM_MEM_CLIENT_DATA_CRIT);
  return;
} /* ds_appsrv_nv_refresh_ev_hdlr */

/*===========================================================================
FUNCTION      DS_APPSRV_DS3G_EVENT_CB

DESCRIPTION   Called by clients to register to get DS3G event.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_appsrv_ds3g_event_cb
(
  ds3geventmgr_event_type   event_id,
  ds3geventmgr_filter_type *filter_info_ptr,
  void                     *event_info_ptr,
  void                     *data_ptr
)
{
  uint32 *msg = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( filter_info_ptr == NULL )
  { 
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "Invalid input data: No Filter Info");
    return;
  }

  if((filter_info_ptr->ps_subs_id < PS_SYS_PRIMARY_SUBS || 
     filter_info_ptr->ps_subs_id > PS_SYS_TERTIARY_SUBS))
  { 
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "Invalid input data, Subs ID: %d",
               filter_info_ptr->ps_subs_id);
    return;
  }

  switch (event_id)
  {
  case DS3GEVENTMGR_NV_REFRESH_EV:

    msg = (uint32*)modem_mem_alloc(sizeof(ps_sys_subscription_enum_type),
                                   MODEM_MEM_CLIENT_DATA_CRIT);

    if (msg == NULL)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "Failed to allocate memory");
      break;
    }

    *msg = filter_info_ptr->ps_subs_id;

    if(!ds_appsrv_put_cmd_ex(&ds_appsrv_nv_refresh_ev_hdlr, msg))
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "Failed to send dstask msg");

      modem_mem_free((void*)msg, MODEM_MEM_CLIENT_DATA_CRIT);
    }
    break;

  default:
    break;
  }

  return;
}/* ds_appsrv_ds3g_event_cb */

/*===========================================================================
                    EXTERNALIZED FUNCTIONS DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS_APPSRV_EFS_ITEM_FILE_PATH_SIZE

DESCRIPTION   Returns the total APPSRV EFS item file paths size
 
PARAMETERS    None
 
DEPENDENCIES  None

RETURN VALUE  uint32: total APPSRV EFS item file paths size

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_appsrv_efs_item_file_path_size
(
  void
)
{
  return sizeof(ds_appsrv_nv_conf_efs_path);
}/* ds_appsrv_efs_item_file_path_size */

/*===========================================================================
FUNCTION      DS_APPSRV_UPDATE_DATA_CONFIG_INFO

DESCRIPTION   Puts all the required nv item file paths in the 
              data_config_info.conf file. Would be called during powerup.
 
DEPENDENCIES  None

RETURN VALUE  length of string added to buffer for success. 
              -1 for failure.

SIDE EFFECTS  None
===========================================================================*/
int32 ds_appsrv_update_data_config_info
(
  char   *file_paths_buf
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL == file_paths_buf)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "file_paths_buf is NULL");
    return -1;
  }

  memscpy(file_paths_buf, strlen(ds_appsrv_nv_conf_efs_path),
          ds_appsrv_nv_conf_efs_path, strlen(ds_appsrv_nv_conf_efs_path));

  return strlen(ds_appsrv_nv_conf_efs_path);
} /* ds_appsrv_update_data_config_info() */

/*===========================================================================
FUNCTION      DS_APPSRV_UTILS_INIT

DESCRIPTION   This function initializes all APPSRV common services.
              The function is called during power up.
 
PARAMETERS    None 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_appsrv_utils_init
(
  void
)
{
  uint32 subs_id;
  ds3geventmgr_filter_type filter_info = {0};
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  filter_info.tech = PS_SYS_TECH_ALL;

  for(subs_id = PS_SYS_PRIMARY_SUBS; subs_id < PS_SYS_SUBS_MAX; subs_id++)
  {
    filter_info.ps_subs_id = (ps_sys_subscription_enum_type)subs_id;
    if(ds3geventmgr_event_reg(DS3GEVENTMGR_NV_REFRESH_EV,
                              DS3GEVENTMGR_CLIENT_ID_APPSRV,
                              &filter_info,
                              ds_appsrv_ds3g_event_cb,
                              NULL) == FALSE)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "Registration for NV REFRESH EV failed!");
    }
  }
}/* ds_appsrv_utils_init */
