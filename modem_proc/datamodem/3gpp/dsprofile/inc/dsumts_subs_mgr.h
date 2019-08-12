#ifndef DSUMTSSUBSMGR_H
#define DSUMTSSUBSMGR_H
/*===========================================================================

                        D S U M T S P S  S U B S  M G R 

                          H E A D E R   F I L E

DESCRIPTION
  This file contains common data declarations and function prototypes specific 
  to DUAL SIM feature. 
  
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/dsprofile/inc/dsumts_subs_mgr.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/16/11   ttv     Added DSDS changes for consolidated profile family.
11/22/10   ttv     Creation of file.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "cm_dualsim.h"
#include "dsumtspdpreg.h"
#include "ds_profile.h"
#include "ds_profile_db.h"

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_LTE)

//Define random default values for each profile family to start with
//This will be overwritten after user selection
#define DSUMTS_DEF_PROFILE_FOR_EMBEDDED_SUB1 1
#define DSUMTS_DEF_PROFILE_FOR_EMBEDDED_SUB2 2
#define DSUMTS_DEF_PROFILE_FOR_EMBEDDED_SUB3 3

#define DSUMTS_DEF_PROFILE_FOR_TETHERED_SUB1 1
#define DSUMTS_DEF_PROFILE_FOR_TETHERED_SUB2 2
#define DSUMTS_DEF_PROFILE_FOR_TETHERED_SUB3 3

/*===========================================================================
                   EXTERNAL FUNCTION PROTOTYPES
===========================================================================*/
/*===========================================================================
FUNCTION DSUMTS_SUBS_MGR_INIT

DESCRIPTION
  This function initializes the DS Subscription manager. It creates the mapping 
  table in EFS with default profile numbers for socket/Rmnet calls
   
PARAMETERS 
  none

DEPENDENCIES 
  Should be called only ONCE during startup.

RETURN VALUE 
  DS_UMTS_PDP_SUCCESS  - initialization was successfull
  DS_UMTS_PDP_FAIL     - initialization failed

SIDE EFFECTS 
  Creates the profiles in EFS if the profiles are not present at startup
===========================================================================*/
ds_umts_pdp_profile_status_etype dsumts_subs_mgr_init(void);


/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_SET_DEFAULT_PROFILE_NUM_PER_SUBS_FOR_EMBEDDED

DESCRIPTION
  This function is called by dsumtspdpregint to set the default profile
  that the user specifies for embedded family and given subscription.

PARAMETERS
  profile_num  -   Holds the default profile number set by user
  subs_id      -   The subscription for which this profile is default

DEPENDENCIES
  None

RETURN VALUE
  ds_umts_pdp_profile_status_etype

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_profile_status_etype 
   dsumts_subs_mgr_set_default_profile_num_per_subs_for_embedded
(
  ds_umts_pdp_subs_e_type           subs_id, 
  uint16                            profile_num
);

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_SET_DEFAULT_PROFILE_NUM_PER_SUBS_FOR_TETHERED

DESCRIPTION
  This function is called by dsumtspdpregint to set the default profile
  that the user specifies for tethered family and given subscription.

PARAMETERS
  profile_num  -   Holds the default profile number set by user
  subs_id      -   The subscription for which this profile is default

DEPENDENCIES
  None

RETURN VALUE
  DS_UMTS_PDP_FAIL          : suns id is invalid
  DS_UMTS_PDP_ACCESS_ERROR  : Error in updating the file
  DS_UMTS_PDP_SUCCESS       : The operation completed successfully

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_profile_status_etype 
dsumts_subs_mgr_set_default_profile_num_per_subs_for_tethered
(
  ds_umts_pdp_subs_e_type           subs_id,
  uint16                            profile_num
);


/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_DEFAULT_PROFILE_NUM_PER_SUBS_FOR_EMBEDDED

DESCRIPTION

  This function is called by dsumtspdpregint to get the default profile
  number of embedded family for a specific subscription.

PARAMETERS
  profile_num   -  OUT parameter. Stores profile number
  subs_id      -   The subscription for which the profile num is requested

DEPENDENCIES
  None

RETURN VALUE
  ds_umts_pdp_profile_status_etype

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_profile_status_etype 
   dsumts_subs_mgr_get_default_profile_num_per_subs_for_embedded
(
  ds_umts_pdp_subs_e_type             subs_id,
  uint16*                             profile_num
);

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_DEFAULT_PROFILE_NUM_PER_SUBS_FOR_TETHERED

DESCRIPTION
  This function is called by dsumtspdpregint to get the default profile
  number of tethered family for a specified subscription.

PARAMETERS
  profile_num  -  OUT paramenter. Returns the default profile
  subs_id      -  The subscription for which default profile is required

DEPENDENCIES
  None

RETURN VALUE
  DS_UMTS_PDP_FAIL     : Subs id is invalid
  DS_UMTS_PDP_SUCCESS  : The operation completed successfully

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_profile_status_etype 
dsumts_subs_mgr_get_default_profile_num_per_subs_for_tethered
(
  ds_umts_pdp_subs_e_type             subs_id,
  uint16*                             profile_num
);

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_SUBS_ID

DESCRIPTION

  This function returns the subscription ID internal to Data services that maps
  to the id from CM.
  CM : DS subscription ID mapping

PARAMETERS
  as_id   - Subscription ID from CM

DEPENDENCIES
  None

RETURN VALUE
  ds_umts_pdp_subs_e_type

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_subs_e_type dsumts_subs_mgr_get_subs_id
(
  sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_AS_ID
DESCRIPTION

  This function returns sys_modem_as_id_e_type from 
  ds_umts_pdp_subs_e_type


PARAMETERS
  ds_umts_pdp_subs_e_type   -- Internal Subscription ID

DEPENDENCIES
  None

RETURN VALUE
 sys_modem_as_id_e_type   - Subscription ID from CM

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type dsumts_subs_mgr_get_as_id
(
   ds_umts_pdp_subs_e_type subs_id
);

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_SUBS_ID_FROM_DB_SUBS_ID 
 
DESCRIPTION

  This function returns ds_umts_pdp_subs_e_type from 
  ds_profile_db_subs_type


PARAMETERS
  ds_profile_db_subs_type   -ds profile db subscrition type 

DEPENDENCIES
  None

RETURN VALUE
 ds_umts_pdp_subs_e_type   -- Internal Subscription ID

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_subs_e_type dsumts_subs_mgr_get_subs_id_from_db_subs_id
(
   ds_profile_db_subs_type subs_id
);


/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_SUBS_ID_FROM_DB_SUBS_ID 
 
DESCRIPTION

  This function returns ds_umts_pdp_subs_e_type from 
  ds_profile_db_subs_type


PARAMETERS
  uint32   - ps policy

DEPENDENCIES
  None

RETURN VALUE
 ds_umts_pdp_subs_e_type   -- Internal Subscription ID

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_subs_e_type dsumts_subs_mgr_get_subs_id_from_policy_subs_id
(
   uint32 subs_id
);

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_DB_SUBS_ID

DESCRIPTION

  This function returns ds_profile_db_subs_type from internal
  ds_umts_pdp_subs_e_type


PARAMETERS
  ds_umts_pdp_subs_e_type   -- Internal Subscription ID

DEPENDENCIES
  None

RETURN VALUE
 ds_profile_db_subs_type   -ds profile db subscrition type

SIDE EFFECTS
  None

===========================================================================*/
ds_profile_db_subs_type dsumts_subs_mgr_get_db_subs_id
(
   ds_umts_pdp_subs_e_type subs_id
)
;

/*===========================================================================

FUNCTION DSUMTS_SUBS_MGR_GET_DS_PROFILE_SUBS_ID

DESCRIPTION

  This function returns ds_profile_db_subs_type from internal
  ds_umts_pdp_subs_e_type


PARAMETERS
  ds_umts_pdp_subs_e_type   -- Internal Subscription ID

DEPENDENCIES
  None

RETURN VALUE
 ds_profile_subs_etype   -ds profile subscrition type

SIDE EFFECTS
  None

===========================================================================*/
ds_umts_pdp_subs_e_type dsumts_subs_mgr_get_subs_id_from_ds_profile_subs_id
(
   ds_profile_subs_etype subs_id
)
;


#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */
#endif /* DSUMTSSUBSMGR_H */
