#ifndef _SYS_PLMN_SELECTION_H_
#define _SYS_PLMN_SELECTION_H_
/*==============================================================================

       S Y S T E M   P L M N   S E L E C T I O N   H E A D E R   F I L E

DESCRIPTION

  This header file contains the types associated with the process of selecting
  a Public Land Mobile Network (PLMN) for service.


         COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
         EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_plmn_selection.h_v   1.0   10 May 2002 15:26:34   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sys_plmn_selection.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/05/12   sg      API changes for SGLTE requirement 
08/09/11   abhi    Added new cause for stop_mode_reason 
03/22/10   abhi    Updating PLMN service state to include RAC/MME code
09/21/09   MNK     Changes due to IRAT design
07/30/08   rm      CMI defeaturisation
06/25/07   vm      Corrected the comments and the feature name
06/25/07   vm      Added uoos_rem_sleep_time and uoos_rem_awake_time in 
                   sys_plmn_service_state_s_type structure.
04/11/07   ic      Fixed several compiler errors
02/07/02   kwa     Initial version.
10/28/02   kwa     Added SYS_STOP_MODE_REASON_DEEP_SLEEP.
02/28/03   kwa     Added ps_data_suspend to sys_plmn_service_state_s_type.
09/25/03   ks      Added enumeration SYS_STOP_MODE_REASON_MODE_CHANGE_FTM.
09/26/03   kwa     Added sys_band_mask_e_type and sys_lac_type to
                   sys_plmn_service_state_s_type.
09/29/03   kwa     Added SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION
                   to sys_network_selection_mode_e_type to differentiate
                   between attempts to find limited service when an emergency
                   call needs to be made from attempts to simply acquire some
                   form of service.
08/12/04   kwa     Removed the unused enumerated type sys_nreg_reason_e_type
                   and corresponding function sys_nreg_reason_is_valid.
11/04/04   kwa     Added the structures sys_plmn_rat_list_info_s_type
                   and sys_plmn_rat_list_s_type along with the
                   corresponding verification functions
                   sys_plmn_rat_list_info_is_valid and
                   sys_plmn_rat_list_is_valid.
02/16/05   up      Added SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION
                   to sys_network_selection_mode_e_type to acquire service on
                   requested PLMN if available otherwise LIMITED on other PLMN.
01/31/06   up      Use new enum SYS_PRIORITY_PLMN_LIST_MAX_LENGTH in place of
                   SYS_PLMN_LIST_MAX_LENGTH for sys_plmn_rat_list type.
06/09/06   sn      Added STOP_MODE_REASON_LPM.
10/16/06   sn      Added WCDMA_1X implementation.
04/11/07   pk      Reverting the earlier check-in, checked-in the changes by
                   mistake
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/
#include "comdef.h"
#include "customer.h"
#include "sys.h"         /* system wide common types */
#include "sys_v.h"
#include "cm_gw.h"       /* Defintions from NAS required by clients */

/*==============================================================================

                             DATA DECLARATIONS

==============================================================================*/

/*
** The following enumerated type defines the mode preferred
** by the user.
*/
typedef enum sys_mode_pref_e
{
  SYS_MODE_PREF_NONE = -1,                       /* FOR INTERNAL USE ONLY! */
  SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY,
  SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY,
  SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED,
  SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED,
  SYS_MODE_PREF_UMTS_1X_RADIO_ACCESS_ONLY,

  SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY,
  SYS_MODE_PREF_TDS_RADIO_ACCESS_ONLY,
  SYS_MODE_PREF_MAX                              /* FOR INTERNAL USE ONLY! */
} sys_mode_pref_e_type;


/*
** The following enumerated type defines the WCDMA_1X mode preference.
*/
typedef enum sys_wcdma_1x_mode_pref_e
{
  SYS_WCDMA_MODE_NONE = -1,
  SYS_WCDMA_MODE_WCDMA,
  SYS_WCDMA_MODE_WCDMA_1X,
  SYS_WCDMA_MODE_MAX
} sys_wcdma_1x_mode_pref_e_type;


/*
** The following enumerated type defines the reasons for
** stopping the current mode.
*/
typedef enum sys_stop_mode_reason_e
{
  SYS_STOP_MODE_REASON_NONE=-1,                  /* FOR INTERNAL USE ONLY! */
  SYS_STOP_MODE_REASON_MODE_CHANGE,
  SYS_STOP_MODE_REASON_MODE_CHANGE_FTM,
  SYS_STOP_MODE_REASON_POWER_OFF,
  SYS_STOP_MODE_REASON_OFFLINE,
  SYS_STOP_MODE_REASON_DEEP_SLEEP,
  SYS_STOP_MODE_REASON_ALT_SCAN,
  SYS_STOP_MODE_REASON_LPM,
  SYS_STOP_MODE_REASON_IRAT,
  SYS_STOP_MODE_REASON_DUAL_SWITCH,
  SYS_STOP_MODE_REASON_LPM_POWER_OFF,
  SYS_STOP_MODE_REASON_LOCAL_DETACH,
  SYS_STOP_MODE_REASON_NO_SYS,
  SYS_STOP_MODE_REASON_DSDS_SUSPEND,  
  SYS_STOP_MODE_REASON_UE_MODE_CHANGE,
  SYS_STOP_MODE_REASON_SUSPEND_STACK,            
  SYS_STOP_MODE_REASON_MODE_CAP_CHANGE,
  SYS_STOP_MODE_REASON_LOCAL_CS_PS_DETACH,
  SYS_STOP_MODE_REASON_MAX                       /* FOR INTERNAL USE ONLY! */
} sys_stop_mode_reason_e_type;

typedef enum sys_acq_scan_type_e
{
  SYS_ACQ_SCAN_TYPE_NONE = -1,
  SYS_ACQ_SCAN_TYPE_NORMAL,    /*Normal scan */
  SYS_ACQ_SCAN_TYPE_PWR_UP,    /* Pwr up scan */
  SYS_ACQ_SCAN_TYPE_MAX
}sys_acq_scan_type_e_type;

typedef enum
{
SYS_ACQ_STATUS_SUCCESS,
SYS_ACQ_STATUS_NO_RESOURCE
} sys_acq_status_e_type;

/*==============================================================================

              Types for the service state on the selected network

==============================================================================*/

/*
** Define a structure that contains the collection of information that
** fully defines the service state of the mobile on a PLMN.
**
** If the PLMN_service_capability is
** SYS_SVC_CAPABILITY_NOT_AVAILABLE, then the plmn field
** is undefined.
*/
typedef struct sys_plmn_service_state_s
{
  sys_sim_state_e_type                      sim_state;
  sys_plmn_id_s_type                        plmn;
  sys_srv_capability_e_type                 plmn_service_capability;
  boolean                                   plmn_forbidden;
  sys_roam_status_e_type                    roaming_ind;
  sys_srv_status_e_type                     service_status;
  sys_srv_domain_e_type                     reg_service_domain;
  sys_radio_access_tech_e_type              active_rat;
/* Band for a given RAT is defined in a union. One rat,one band approach*/
  sys_active_band_u_type                    active_band;
  sys_lac_type                              lac;
  sys_lac_type                              tac;
  boolean                                   ps_data_suspend;

  sys_csg_info_s_type                     csg_info;
  boolean                                   csg_in_white_list;

  sys_srv_status_e_type                     cs_service_status;
  sys_no_svc_cause_e_type                   no_service_cause;
  dword                                     suitable_search_time;
  byte                                      rac_or_mme_code; 
  boolean                                   sib8_available;
  sys_lte_cs_capability_e_type     lte_cs_capability;
  sys_acq_status_e_type                     acq_status;
  sys_nw_feature_support_s_type              eps_nw_feature_support;
  sys_extend_srv_info_e_type                extend_srv_info;

  /* Should be use to indicate good PLMN found in CAMP_ONLY mode. 
  *  In other mode it should be set to NOT_APPLICABLE.
  */
  sys_reg_domain_e_type                      plmn_reg_type;

  sys_plmn_id_s_type                         registered_plmn;

  boolean                                    emc_attached;

} sys_plmn_service_state_s_type;


/*
** Structure that defines a PLMN-RAT pair.
*/
typedef struct sys_plmn_rat_list_info_s
{
  sys_plmn_id_s_type                        plmn;
  sys_radio_access_tech_e_type              rat;
} sys_plmn_rat_list_info_s_type;


/*
** Define a structure for the PLMN-RAT list.
*/
typedef struct sys_plmn_rat_list_s
{
  uint32                                    length;
  sys_plmn_rat_list_info_s_type             info[SYS_PRIORITY_PLMN_LIST_MAX_LENGTH];
} sys_plmn_rat_list_s_type;

/*
**CSG list structure
*/
#define SYS_CSG_LIST_MAX_LENGTH 50
/*==============================================================================

                           FUNCTION DECLARATIONS

==============================================================================*/

#endif /* #ifndef _SYS_PLMN_SELECTION_H_ */
