#ifndef DS_MPPM_EFS_PARSER_H
#define DS_MPPM_EFS_PARSER_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                      D S  M P P M  E F S  P A R S E R

GENERAL DESCRIPTION

  All the declarations and definitions necessary to support parsing  
  of MPPM database related EFS file.

Copyright (c) 2012-2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==================================================================
                        EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/wlan/mapcon/inc/ds_mppm_efs_parser.h#1 $

when       who         what, where, why
--------   -----       ----------------------------------------------------------
06/13/2016  ak         Added support handover between ePDG and 3G(FR 20728)
07/14/2014 scb         Add support for ds_mppm_get_pdn_policy_db_entry
06/30/2014  fn         MPPM queue changes per subscription
24/04/2014  scb        Add support for per PDN T_MINONWWAN_TIMER on IWLAN
11/16/2013  vl         Added changes to write PDN DB Queue contents to EFS
                       file
11/23/2013 scb         Added support for PDN DB UPDATE INFO in MPPM
11/21/2013 scb         Added support for WLAN_MAX_CONN reached indication in MPPM
09/23/2013 scb         Added support for throttling in MPPM.
09/16/2013 scb         Added changes to support LTE_ATTACH_IN_PROGRESS support
10/23/2012 scb         Create MPPM EFS module

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "queue.h"

#ifdef FEATURE_DATA_WLAN_MAPCON

#include "rex.h"
#include "ds_sys.h"
#include "ps_sys.h"
#include "dsutil.h"
#include "data_msg.h"

#include "modem_mem.h"
#include "ds_profile.h"

#define PDN_CONFIG_MAX_SUPPORTED_RATS 3

#define TECH_NON_EPC_WWAN_POLICY_MASK (0x0010000)
#define TECH_NON_EPC_WWAN_POLICY_MASK_WITH_WCDMA (0x00100000)
#define TECH_WCDMA_POLICY_MASK (0x00010000)
#define TECH_LTE_POLICY_MASK (0x00001000)
#define TECH_EHRPD_POLICY_MASK (0x00000100)
#define TECH_WIFI_LB_POLICY_MASK (0x00000010)
#define TECH_WIFI_IWLAN_POLICY_MASK (0x00000001)
#define TECH_EPC_WWAN_WLAN_POLICY_MASK ( TECH_WIFI_IWLAN_POLICY_MASK | \
                                         TECH_WIFI_LB_POLICY_MASK | \
                                         TECH_EHRPD_POLICY_MASK | \
                                         TECH_LTE_POLICY_MASK | \
                                         TECH_NON_EPC_WWAN_POLICY_MASK )

#define TECH_EPC_WWAN_POLICY_MASK  ( TECH_LTE_POLICY_MASK | \
                                     TECH_EHRPD_POLICY_MASK)

#define TECH_WWAN_POLICY_MASK  ( TECH_LTE_POLICY_MASK | \
                                 TECH_EHRPD_POLICY_MASK | \
                                 TECH_NON_EPC_WWAN_POLICY_MASK )

#define DS_MPPM_MAX_TECH_SUPPORTED 6

#define DS_SYS_MAX_APN_STR_LEN   DS_SYS_MAX_APN_LEN-1

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
    MAX PDN COUNT - Total number of profiles + buffer
---------------------------------------------------------------------------*/
#define DS_MAX_PDN_COUNT                    PS_SYS_MAX_APNS

/*===========================================================================
                              DATA STRUCTURES
===========================================================================*/
/*---------------------------------------------------------------------------
  MPPM tech mask enumeration.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_MPPM_TECH_MASK_NON_EPC_WWAN,
  DS_MPPM_TECH_MASK_EPC_WWAN_WLAN,
  DS_MPPM_TECH_MASK_EPC_WWAN,
  DS_MPPM_TECH_MASK_WWAN,
  DS_MPPM_TECH_MASK_MAX 
} ds_mppm_tech_mask_enum_type;

/*---------------------------------------------------------------------------
  Enum for override types
---------------------------------------------------------------------------*/
typedef enum
{
  MPPM_OVERRIDE_TYPE_NONE = 0,
  MPPM_OVERRIDE_TYPE_API,
  MPPM_OVERRIDE_TYPE_OPTION,
  MPPM_OVERRIDE_TYPE_MAX = MPPM_OVERRIDE_TYPE_OPTION
}ds_mppm_override_type;

/*---------------------------------------------------------------------------
  Per PDN timer pointer.
---------------------------------------------------------------------------*/
typedef enum
{
  MPPM_PDN_TIMER_TYPE_NONE = 0,
  MPPM_PDN_TIMER_TYPE_T_MINONWWAN,
  MPPM_PDN_TIMER_TYPE_MAX
}ds_mppm_pdn_timer_e_type;

/*---------------------------------------------------------------------------
  MPPM APN NAME structure
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                              apn_string_len;
  char                                apn_string[DS_SYS_MAX_APN_LEN];
}ds_mppm_apn_name_type;

/*---------------------------------------------------------------------------
  MPPM PDN TIMER INFO TYPE structure
  //TODO-SAP: Adding dsd_subs_id, remove when DSDA support is added.
---------------------------------------------------------------------------*/
typedef struct
{
  ps_sys_subscription_enum_type  dsd_subs_id;
  ds_mppm_pdn_timer_e_type       pdn_timer_type;
  rex_timer_type                 pdn_timer;
}ds_mppm_pdn_timer_info_type;

/*---------------------------------------------------------------------------
  Policy database entry element structure.
  The link element is supposed to be the first field for every element of
  the queue structure.
  The APNname contains a valid APN string with length > 0.
  NULL APNs are indicated by the "NULL" string.
  overide_type is used to indicate the override type of type
  ds_mppm_override_type.
  supported_rat_priority_list is a priority list of utmost 3 RATs which
  typically indicates the priority order in which the APN supports
  WWAN, IWLAN and WLAN_LB RAT types.
  Added boolean to indicate lte_attach_in_progress for a particular entry
  Additional override_mask which will be changed for a particular PDN
  depending on inputs. The inputs that will manipulate this mask currently
  are: MPPM_TECH_THROTTLE, IWLAN_MAX_PDN, T_MINONWWAN_TIMER.
  rat, profile_num, dsd_subs_id are valid when PDN is up. profile_num
  is used to register for profile updates and the profile handle is stored
  in profile_handle. iwlan_to_lte_roaming_ho_supported and 
  lte_to_iwlan_roaming_ho_supported flags are read from profile and indicate
  whether handover in roaming is supported for the respective technology. 
   
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                   link;
  ds_mppm_apn_name_type         apn_name;
  boolean                       default_apn;
  ds_mppm_override_type         override_type;
  uint32                         
                  supported_rat_priority_list[PDN_CONFIG_MAX_SUPPORTED_RATS];
  boolean                       lte_attach_in_progress;
  ps_sys_pdn_status_enum_type   v4_pdn_status;
  ps_sys_pdn_status_enum_type   v6_pdn_status;
  ps_sys_tech_enum_type         tech_type;
  uint32                        override_mask;
  uint32                      override_cause_list[DS_MPPM_MAX_TECH_SUPPORTED];
  ds_mppm_pdn_timer_info_type   *pdn_timer_info_ptr;
  boolean                       emergency_calls_supported;
  ps_sys_rat_ex_enum_type       rat;
  uint16                        profile_num;
  ps_sys_subscription_enum_type dsd_subs_id;
  ds_profile_cb_handle_type     profile_handle;
  boolean                       iwlan_to_lte_roaming_ho_supported;
  boolean                       lte_to_iwlan_roaming_ho_supported;
}mppm_pdn_policy_database_entry_s_type;

/*---------------------------------------------------------------------------
  Enum for parameter names specified in the profile file in EFS.
  Any update to this enum requires a corresponding update to
  mppm_pdn_policy_db_param_name_id.
---------------------------------------------------------------------------*/
typedef enum
{
  MPPM_PDN_POLICY_DB_PARAM_ID_MIN = 0,
  PDN_APN_STRING = MPPM_PDN_POLICY_DB_PARAM_ID_MIN,
  DEFAULT_APN,
  OVERRIDE_TYPE,
  SUPPORTED_RAT_PRIORITY_LIST,
  MPPM_PDN_POLICY_DB_PARAM_ID_MAX = SUPPORTED_RAT_PRIORITY_LIST
}mppm_pdn_policy_db_param_name_type;

/*===========================================================================
                      EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS_MPPM_INIT_TECH_MASKS

DESCRIPTION   

PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  .
===========================================================================*/
void ds_mppm_init_tech_masks();

/*===========================================================================
FUNCTION      DS_MPPM_GET_TECH_MASK

DESCRIPTION   

PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  .
===========================================================================*/
uint32 ds_mppm_get_tech_mask
(
  ps_sys_subscription_enum_type    dsd_subs_id,
  ds_mppm_tech_mask_enum_type      tech_mask_type
);

/*===========================================================================
FUNCTION      DS_MPPM_ADD_PDN_TO_POLICY_DB

DESCRIPTION   This function will add a PDN entry to the policy database

PARAMETERS    *pdn_policy_db_q - Pointer to queue to store PDN database 
                                 entries
              *apn_name_ptr - APN name associated with PDN entry.
              *fallback_apn_pdn_policy_db_entry - Fallback PDN data entry

DEPENDENCIES  None

RETURN VALUE  TRUE - if successful, FALSE otherwise

SIDE EFFECTS  The mppm_pdn_policy_db_q will be populated with default values 
              for the PDN specified by the apn_name string or with the
              contents of the fallback_pdn_db_entry if VALID.
===========================================================================*/
boolean ds_mppm_add_pdn_to_policy_db_by_apn
(
  q_type                                *pdn_policy_db_q,
  ds_mppm_apn_name_type                 *apn_name_ptr,
  mppm_pdn_policy_database_entry_s_type *fallback_apn_pdn_policy_db_entry,ps_sys_subscription_enum_type         dsd_subs_id
);

/*===========================================================================
FUNCTION      DS_MPPM_READ_PDN_POLICY_DB_FROM_EFS

DESCRIPTION   This function will read PDN rules associated with options
              from EFS

PARAMETERS    q_type *q - Pointer to queue to store database entries
              char* efs_file_name - EFS configuration file to be read
                and parsed to store database entry.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  The mppm_pdn_policy_db_q will be populated with values read
              from EFS.
===========================================================================*/
boolean ds_mppm_read_pdn_policy_db_from_efs
(
  q_type           *q, 
  char             *efs_file_name
);

/*===========================================================================
FUNCTION      DS_MPPM_PDN_POLICY_DB_Q_COMPARE_FCT

DESCRIPTION   Utility function to compare two items in the pdn db queue.

DEPENDENCIES  None.

RETURN VALUE  1: if the items are equal
              0: if items are not equal

SIDE EFFECTS  None.
===========================================================================*/
int ds_mppm_pdn_policy_db_q_compare_fct
(
  void *item_ptr,
  void *target_val
);

/*===========================================================================
FUNCTION  DS_MPPM_COPY_PDN_POLICY_DB_Q

DESCRIPTION This function is used to copy the contents from one PDN policy DB
            queue to another as is.
      
PARAMETERS
  None.
    
DEPENDENCIES  All elements of the queue should be of type 
              mppm_pdn_policy_database_entry_s_type
  
RETURN VALUE  Number of elements in the from_q_ptr that was copied.
  

SIDE EFFECTS
  None.

===========================================================================*/
uint8 ds_mppm_copy_pdn_policy_db_q
( 
  q_type *to_q_ptr,
  q_type *from_q_ptr
);

/*===========================================================================
FUNCTION      DS_MPPM_GET_PDN_POLICY_DB_ENTRY

DESCRIPTION   Utility function to get a pointer to the pdn_policy_db
              in the pdn db queue.

DEPENDENCIES  None.

RETURN VALUE  1: if the items are equal
              0: if items are not equal

SIDE EFFECTS  None.
===========================================================================*/
mppm_pdn_policy_database_entry_s_type* ds_mppm_get_pdn_policy_db_entry
(
  q_type                           *q_ptr,
  ds_mppm_apn_name_type            *apn_name_ptr,
  ps_sys_subscription_enum_type     dsd_subs_id
);

/*===========================================================================
FUNCTION      DS_MPPM_PDN_DB_WRITE_TO_EFS_FILE

DESCRIPTION   Writes the Policies from PDB DB Queue to EFS Policy DB file

PARAMETERS    pdn_policy_db_q_ptr - Policy DB Queue

DEPENDENCIES  None

RETURN VALUE  TRUE - If PDB DB Queue is written successfully to EFS
              FALSE - EFS writing failed.

SIDE EFFECTS
===========================================================================*/
boolean ds_mppm_write_to_efs_file
(
  q_type* pdn_policy_db_q_ptr,
  ps_sys_subscription_enum_type subscription_id
);

#endif /*FEATURE_DATA_WLAN_MAPCON*/
#endif /* DS_MPPM_EFS_PARSER_H*/
