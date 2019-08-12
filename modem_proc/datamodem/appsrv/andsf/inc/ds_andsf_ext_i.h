/*==============================================================================

                        ds_andsf_ext_i.h

GENERAL DESCRIPTION
  ANDSF Rule Priority Query API

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/20/15    ml     Decrease max tech size.
01/26/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_ANDSF_EXT_I_H
#define DS_ANDSF_EXT_I_H

#include "comdef.h"
#include "ds_sys.h"

#define DS_ANDSF_MAX_TECHS         5
#define DS_ANDSF_WLAN_MAX_SSID_LEN 32


/* Bitmasks indicating which fields are changed in ds_andsf_priority_info_s_type structure.*/
#define DS_ANDSF_APN_PRIORITY_MASK      0x01
#define DS_ANDSF_1X_CS_MASK             0x02


/*==============================================================================
ENUM          ds_andsf_tech_e_type

DESCRIPTION   Definition of technology types.
==============================================================================*/
typedef enum
{
  DS_ANDSF_TECH_NONE = -1,
  DS_ANDSF_TECH_WWAN,
  DS_ANDSF_TECH_IWLAN,
  DS_ANDSF_TECH_WLAN_LB,
  DS_ANDSF_TECH_MAX
} ds_andsf_tech_e_type;


/*==============================================================================
STRUCT        ds_andsf_wlan_info_s_type

DESCRIPTION   Addition information given for WLAN

PARAMETERS    ssid          - SSID to use
              hessid        - HESSID to use
              ssid_avail    - TRUE if SSID is available, else FALSE
              hessid_avail  - TRUE if HESSID is available, else FALSE

DEPENDENCIES  None
==============================================================================*/
typedef struct
{
  char    ssid[DS_ANDSF_WLAN_MAX_SSID_LEN+1];
  uint8   hessid[DS_SYS_MAC_ADDR_LEN];
  boolean ssid_avail;
  boolean hessid_avail;
} ds_andsf_wlan_info_s_type;


/*==============================================================================
STRUCT        ds_andsf_access_tech_info_s_type

DESCRIPTION   Technology information. WLAN_LB could contain additional
              information for WLAN

PARAMETERS    access_tech - Technology type
              wlan_info   - Additional information only valid when access_tech
                            is IWLAN or WLAN_LBO

DEPENDENCIES  None
==============================================================================*/
typedef struct
{
  ds_andsf_tech_e_type      access_tech;
  ds_andsf_wlan_info_s_type wlan_info;
} ds_andsf_access_tech_info_s_type;


/*==============================================================================
STRUCT        ds_andsf_apn_name_s_type

DESCRIPTION   Technology priority list information for a particular APN.

PARAMETERS    apn_str     - APN string
              apn_str_len - APN string length

DEPENDENCIES  None
==============================================================================*/
typedef struct
{
  char   apn_str[DS_SYS_MAX_APN_LEN];
  uint32 apn_str_len;
} ds_andsf_apn_name_s_type;


/*==============================================================================
STRUCT        ds_andsf_apn_priority_info_s_type

DESCRIPTION   Technology priority list information for a particular APN.

PARAMETERS    apn_name            - APN name
              is_api_override     - TRUE if API Override, else FALSE
              tech_priority_list  - Technology priority list. This list is
                                    sorted in order of priority.
              num_tech            - tech_priority_list size

DEPENDENCIES  None
==============================================================================*/
typedef struct
{
  ds_andsf_apn_name_s_type          apn_name;
  boolean                           is_api_override;
  ds_andsf_access_tech_info_s_type  tech_priority_list[DS_ANDSF_MAX_TECHS];
  uint32                            num_tech;
} ds_andsf_apn_priority_info_s_type;


/*==============================================================================
STRUCT        ds_andsf_priority_info_s_type

DESCRIPTION   List of APN priority information. Index 0 will contain priority
              list for NULL APN. APN priority list is sorted by APN name.

PARAMETERS    apn_list        - APN priority list
              num_apn         - apn_list size
              is_1x_cs_avail  - TRUE if 1x CS is available, else FALSE
	      change_mask     - bitmask indicating which info is changed
			        0x01 when priority_list per apn changes
			        0x02 when is_1x_cs_avail changes

DEPENDENCIES  None
==============================================================================*/
typedef struct
{
  ds_andsf_apn_priority_info_s_type apn_list[DS_SYS_MAX_APNS];
  uint32                            num_apn;
  boolean                           is_1x_cs_avail;
  uint32                            change_mask;
} ds_andsf_priority_info_s_type;


/*==============================================================================
ENUM          ds_andsf_event_e_type

DESCRIPTION   List of events for callback function.
==============================================================================*/
typedef enum
{
  DS_ANDSF_INVALID_EV = 0,
  DS_ANDSF_TECH_PRIORITY_CHANGE_EV,
  DS_ANDSF_MAX_EV
} ds_andsf_event_e_type;


/*==============================================================================
UNION         ds_andsf_event_info_u_type

DESCRIPTION   ANDSF event information

PARAMETERS    priority_info - Tech priority information for each APN

DEPENDENCIES  None
==============================================================================*/
typedef union
{
  ds_andsf_priority_info_s_type priority_info;
} ds_andsf_event_info_u_type;


/*==============================================================================
STRUCT        ds_andsf_event_cb_fcn_type

DESCRIPTION   Callback function to notify ANDSF event.

              event_type Values
              -----------------
              DS_ANDSF_TECH_PRIORITY_CHANGE_EV  Tech priority changed.
                                                Use event_info_ptr->priority_info

PARAMETERS    event_type      - ANDSF event type
              subs_id         - Subscription ID
              event_info_ptr  - Contains detailed information of the ANDSF event
              user_data_ptr   - Client data given on callback registration

DEPENDENCIES  None
==============================================================================*/
typedef void (*ds_andsf_event_cb_fcn_type)(
                                           ds_andsf_event_e_type              event_name,
                                           ds_sys_subscription_enum_type      subs_id,
                                           const ds_andsf_event_info_u_type*  event_info_ptr,
                                           void*                              user_data_ptr
                                           );


#ifdef __cplusplus
extern "C"
{
#endif


/*==============================================================================
FUNCTION      ds_andsf_reg_cb

DESCRIPTION   Register callback function for ANDSF events notifications.

PARAMETERS    [In] cb_fcn         - Callback function to register
              [In] user_data_ptr  - User data to return in callback

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
==============================================================================*/
boolean ds_andsf_reg_cb(ds_andsf_event_cb_fcn_type cb_fcn, void* user_data_ptr);


/*==============================================================================
FUNCTION      ds_andsf_dereg_cb

DESCRIPTION   Deregister callback function for ANDSF event notifications.

PARAMETERS    [In] cb_fcn         - Callback function to register
              [In] user_data_ptr  - User data to return in callback

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
==============================================================================*/
boolean ds_andsf_dereg_cb(ds_andsf_event_cb_fcn_type cb_fcn);


/*==============================================================================
FUNCTION      ds_andsf_query_tech_priority_list

DESCRIPTION   Query for APN priority list for the subscription ID.

PARAMETERS    [In]  subs_id           - Subscription ID
              [Out] priority_info_ptr - Priority list object to fill

DEPENDENCIES  Memory for priority_info_ptr must be allocated by caller.

RETURN VALUE  None

SIDE EFFECTS  None
==============================================================================*/
void ds_andsf_query_tech_priority_list(
                                       ds_sys_subscription_enum_type  subs_id,
                                       ds_andsf_priority_info_s_type* priority_info_ptr
                                       );


/*==============================================================================
FUNCTION      ds_andsf_check_is_andsf_enabled

DESCRIPTION   Check if the ANDSF module is enabled.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  TRUE if ANDSF is enabled, else FALSE

SIDE EFFECTS  None
==============================================================================*/
boolean ds_andsf_check_is_andsf_enabled();


#ifdef __cplusplus
}
#endif

#endif /* DS_ANDSF_EXT_I_H */
