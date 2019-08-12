/*===========================================================================
                      DS_ANDSF_LOCATION_UPDATE.H

DESCRIPTION
 Location update header file for ANDSF.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    -------                ----------------------------------------
10/17/14    Youjunf                          First version of file
===========================================================================*/
#ifndef DS_ANDSF_LOCATION_UPDATE_H
#define DS_ANDSF_LOCATION_UPDATE_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "comdef.h"
#include "ds_sys.h"
#include "cm.h"
#include "ds_andsf_ext_i.h"

#define DS_ANDSF_3GPP_PLMN_SIZE      3
#define DS_ANDSF_3GPP2_HRPD_SID_SIZE 16

/*===========================================================================
                   EXTERNAL DEFINITIONS AND TYPES
===========================================================================*/

/*------------------------------------------------------------------------------
  Structure of  3gpp location to be updated
------------------------------------------------------------------------------*/
typedef struct
{
  uint8   plmn[DS_ANDSF_3GPP_PLMN_SIZE];
  uint16  tac;
  uint16  lac;
  uint32  geran_ci;
  uint32  utran_ci;
  uint32  eutra_ci;
  boolean tac_avail;
  boolean lac_avail;
  boolean geran_ci_avail;
  boolean utran_ci_avail;
  boolean eutra_ci_avail;
} ds_andsf_3gpp_loc_info_s_type;


/*------------------------------------------------------------------------------
  Structure of  3gpp2 cdma 1x location to be updated
------------------------------------------------------------------------------*/
typedef struct
{
  uint16  sid;
  uint16  nid;
  uint16  base_id;
  boolean nid_avail;
  boolean base_id_avail;
} ds_andsf_3gpp2_1x_loc_info_s_type;




/*------------------------------------------------------------------------------
  Structure of  3gpp2 hrpd  location to be updated
------------------------------------------------------------------------------*/
typedef struct
{
  uint8 sector_id[DS_ANDSF_3GPP2_HRPD_SID_SIZE];
  uint8 netmask;
} ds_andsf_3gpp2_hrpd_loc_info_s_type;



/*------------------------------------------------------------------------------
  enum of andsf wwan location type
------------------------------------------------------------------------------*/
typedef enum
{
  DS_ANDSF_WWAN_NO_LOC = 0,
  DS_ANDSF_WWAN_3GPP_LOC,
  DS_ANDSF_WWAN_3GPP2_1X_LOC,
  DS_ANDSF_WWAN_3GPP2_HRPD_LOC
} ds_andsf_wwan_loc_e_type;



typedef struct
{
  ds_andsf_wwan_loc_e_type              loc_type;
  union
  {
    ds_andsf_3gpp_loc_info_s_type       loc_3gpp;
    ds_andsf_3gpp2_1x_loc_info_s_type   loc_3gpp2_1x;
    ds_andsf_3gpp2_hrpd_loc_info_s_type loc_3gpp2_hrpd;
  } mode_loc_info;
} ds_andsf_wwan_loc_stack_info_s_type;



/*------------------------------------------------------------------------------
  Structure of  wwan location location to be updated
------------------------------------------------------------------------------*/
typedef struct
{
  ds_sys_subscription_enum_type       subs_id;
  ds_andsf_wwan_loc_stack_info_s_type stack_info[2];
} ds_andsf_wwan_loc_info_s_type;


/*------------------------------------------------------------------------------
  Structure of  wlan location to be updated
------------------------------------------------------------------------------*/
typedef struct
{
  boolean is_wlan_srv_avail;
  char    ssid[DS_ANDSF_WLAN_MAX_SSID_LEN+1];
  uint8   hessid[DS_SYS_MAC_ADDR_LEN];
  uint8   bssid[DS_SYS_MAC_ADDR_LEN];
  boolean ssid_avail;
  boolean hessid_avail;
  boolean bssid_avail;
} ds_andsf_wlan_loc_info_s_type;


/*------------------------------------------------------------------------------
  Structure of  andsf policy file info to be updated
------------------------------------------------------------------------------*/
typedef struct
{
  ds_sys_subscription_enum_type subs_id;
  char*                         file_path_ptr;
} ds_andsf_file_info_s_type;


/*===========================================================================
                           FUNCTION DECLARATIONS
===========================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif
/*===========================================================================
FUNCTION      DS_ANDSF_LOCATION_UPDATE_INIT

DESCRIPTION   This function initializes the andsf location update module. This is during 
              power up. 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_andsf_location_update_init(void);

void ds_andsf_process_wwan_location_event(cm_mm_msim_ss_info_s_type *wwan_location_event_ptr);

/*------------------------------------------------------------------------------
  Post andsf policy file update information to ANDSF rule manager
------------------------------------------------------------------------------*/
void ds_andsf_notify_update_file(const ds_andsf_file_info_s_type* file_info_ptr);

/*------------------------------------------------------------------------------
  Post the 3gpp and 3gpp2 location information to ANDSF rule manager
------------------------------------------------------------------------------*/
void ds_andsf_notify_update_wwan_location(const ds_andsf_wwan_loc_info_s_type* wwan_loc_info_ptr);

/*------------------------------------------------------------------------------
  Post the wlan location information to ANDSF rule manager
------------------------------------------------------------------------------*/
void ds_andsf_notify_update_wlan_location(const ds_andsf_wlan_loc_info_s_type* wlan_loc_info_ptr);


#ifdef __cplusplus
}
#endif

#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_LOCATION_UPDATE_H */

