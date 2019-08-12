#ifndef  DS_WLAN_PROXY_CMD_HDLR_H
#define  DS_WLAN_PROXY_CMD_HDLR_H

#include "dstask_v.h"
#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_WLAN_PROXY_PROCESS_CMD

DESCRIPTION

PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_wlan_proxy_process_cmd
(
  ds_cmd_type *cmd_ptr                         /* Pointer to Command */
);

/*===========================================================================
FUNCTION      DS_WLAN_PROXY_POST_CMD

DESCRIPTION

PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_wlan_proxy_post_cmd
(
  ds_cmd_enum_type  cmd_id,
  void              *user_data
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/
#endif

