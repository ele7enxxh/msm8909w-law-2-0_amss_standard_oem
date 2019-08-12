#ifndef SIMLOCK_H
#define SIMLOCK_H
/*===========================================================================


            S I M L O C K   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/08/16   bcho    Added support for ICCID simlock
06/17/16   vdc     Added support for SPN based SIMLOCK feature
01/06/16   stv     Remote simlock support
07/01/15   bcho    Support for whitelist and blacklist codes in same category
05/15/15   vv      Support for RSU set configuration in simlock
05/15/15   vv      Support for RSU in simlock
05/15/15   stv     SIMLOCK time module related changes
05/04/15   vv      Support for more than 1 bytes of GID1/GID2
03/19/15   at      Introducing new relock feature in SIMLock
12/17/14   vv      Relocate the simlock data files
09/29/14   vv      Added support for the remote SFS
09/29/14   vv      Added support for emergency only mode
08/29/14   vv      Added support for temporary unlock
04/15/14   vv      Added support for quad SIM
04/14/14   vv      Added support for identifying conflicting codes
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_common.h"
#include "simlock_platform.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* valid wild character for lock code is '*' */
#define SIMLOCK_LOCK_CODE_WILD_CHAR      '*'

/* valid wild digit for lock code (e.g. gid1) is FF */
#define SIMLOCK_LOCK_CODE_WILD_DIGIT     0xFF

/* Minimum length of GID data required in SIMLOCK */
#define SIMLOCK_MIN_GID_LEN              1 

/* Number of milli seconds in one second */
#define SIMLOCK_MILLISECONDS_IN_A_SECOND    1000

/* Number of milli seconds in one minute */
#define SIMLOCK_MILLISECONDS_IN_A_MINUTE    60000

/*=============================================================================

                   ENUMERATED DATA DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_DATA_FILE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the simlock data files
-----------------------------------------------------------------------------*/
typedef enum {
  /* Simlock Configuration File */
  SIMLOCK_FILE_CONFIG,

  /* Simlock Lock Category files for slot 1 */
  SIMLOCK_FILE_3GPP_NW_1,
  SIMLOCK_FILE_3GPP_NS_1,
  SIMLOCK_FILE_3GPP_SP_1,
  SIMLOCK_FILE_3GPP_CP_1,
  SIMLOCK_FILE_3GPP_SIM_1,
  SIMLOCK_FILE_3GPP_SPN_1,
  SIMLOCK_FILE_3GPP_SP_EHPLMN_1,
  SIMLOCK_FILE_3GPP_ICCID_1,

  SIMLOCK_FILE_3GPP2_NW_TYPE1_1,
  SIMLOCK_FILE_3GPP2_NW_TYPE2_1,
  SIMLOCK_FILE_3GPP2_HRPD_1,
  SIMLOCK_FILE_3GPP2_SP_1,
  SIMLOCK_FILE_3GPP2_CP_1,
  SIMLOCK_FILE_3GPP2_RUIM_1,

  /* Simlock Lock Category files for slot 2 */
  SIMLOCK_FILE_3GPP_NW_2,
  SIMLOCK_FILE_3GPP_NS_2,
  SIMLOCK_FILE_3GPP_SP_2,
  SIMLOCK_FILE_3GPP_CP_2,
  SIMLOCK_FILE_3GPP_SIM_2,
  SIMLOCK_FILE_3GPP_SPN_2,
  SIMLOCK_FILE_3GPP_SP_EHPLMN_2,
  SIMLOCK_FILE_3GPP_ICCID_2,

  SIMLOCK_FILE_3GPP2_NW_TYPE1_2,
  SIMLOCK_FILE_3GPP2_NW_TYPE2_2,
  SIMLOCK_FILE_3GPP2_HRPD_2,
  SIMLOCK_FILE_3GPP2_SP_2,
  SIMLOCK_FILE_3GPP2_CP_2,
  SIMLOCK_FILE_3GPP2_RUIM_2,

  /* Simlock Lock Category files for slot 3 */
  SIMLOCK_FILE_3GPP_NW_3,
  SIMLOCK_FILE_3GPP_NS_3,
  SIMLOCK_FILE_3GPP_SP_3,
  SIMLOCK_FILE_3GPP_CP_3,
  SIMLOCK_FILE_3GPP_SIM_3,
  SIMLOCK_FILE_3GPP_SPN_3,
  SIMLOCK_FILE_3GPP_SP_EHPLMN_3,
  SIMLOCK_FILE_3GPP_ICCID_3,

  SIMLOCK_FILE_3GPP2_NW_TYPE1_3,
  SIMLOCK_FILE_3GPP2_NW_TYPE2_3,
  SIMLOCK_FILE_3GPP2_HRPD_3,
  SIMLOCK_FILE_3GPP2_SP_3,
  SIMLOCK_FILE_3GPP2_CP_3,
  SIMLOCK_FILE_3GPP2_RUIM_3,

  /* Simlock Lock Category files for slot 4 */
  SIMLOCK_FILE_3GPP_NW_4,
  SIMLOCK_FILE_3GPP_NS_4,
  SIMLOCK_FILE_3GPP_SP_4,
  SIMLOCK_FILE_3GPP_CP_4,
  SIMLOCK_FILE_3GPP_SIM_4,
  SIMLOCK_FILE_3GPP_SPN_4,
  SIMLOCK_FILE_3GPP_SP_EHPLMN_4,
  SIMLOCK_FILE_3GPP_ICCID_4,

  SIMLOCK_FILE_3GPP2_NW_TYPE1_4,
  SIMLOCK_FILE_3GPP2_NW_TYPE2_4,
  SIMLOCK_FILE_3GPP2_HRPD_4,
  SIMLOCK_FILE_3GPP2_SP_4,
  SIMLOCK_FILE_3GPP2_CP_4,
  SIMLOCK_FILE_3GPP2_RUIM_4,

  /* Time from various sources is stored in this file */
  SIMLOCK_FILE_TIME_INFO,

  /* Files to support RSU */
  SIMLOCK_FILE_RSU_PUBKEY,
  SIMLOCK_FILE_RSU_KEY_MOD,

  /* File to store temporary unlock's start time
     and duration for RSU */
  SIMLOCK_FILE_TEMP_UNLOCK,

  /* File to store remote lock blob timestamp to avoid replay attack */
  SIMLOCK_FILE_REMOTE_BLOB_TIME,

  SIMLOCK_FILE_ENUM_INVALID = 0x7F
} simlock_data_file_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_SFS_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the sfs location being used to store the data
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_SFS_LOCAL  = 0,
  SIMLOCK_SFS_REMOTE
} simlock_sfs_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_DIR_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the various directories simlock uses
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_DIR_SAFE,
  SIMLOCK_DIR_SFS,
  SIMLOCK_DIR_UIM,
  SIMLOCK_DIR_CONFIG
} simlock_dir_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_CODE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the code type
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_CODE_WHITELIST = 0,
  SIMLOCK_CODE_BLACKLIST
} simlock_code_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_CODE_IN_LIST_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the list in which the code is present
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_CODE_IN_NOLIST,
  SIMLOCK_CODE_IN_WHITELIST,
  SIMLOCK_CODE_IN_BLACKLIST,
  SIMLOCK_CODE_IN_WHITELIST_AND_BLACKLIST
} simlock_code_in_list_enum_type;

/*=============================================================================

                   ABSTARCT DATA TYPES

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_CATEGORY_HEADER_DATA_TYPE

   DESCRIPTION:
     This structure contains the header for the category data

     category_type   : type of the category
     auto_lock       : status of autolock
     code_type       : indicates if the code preference is blacklist or whitelist
     num_retries_max : maximum retries allowed. A value of 0 indicates
                       unlimited retries
     curr_retries    : current attempted retries
     iteration_cnt   : iteration count used for generating the hash of
                       control key
     salt            : salt used for generating the hash of control key
     hck             : hash of the control key
     num_of_codes    : number of the lock codes
-------------------------------------------------------------------------------*/
typedef struct
{
  simlock_category_enum_type category_type;
  boolean                    auto_lock;
  simlock_code_enum_type     code_type;
  uint32                     num_retries_max;
  uint32                     curr_retries;
  uint32                     iteration_cnt;
  simlock_salt_type          salt;
  simlock_hck_type           hck;
  uint8                      num_of_codes;
} simlock_category_header_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_CATEGORY_CODE_DATA_TYPE

   DESCRIPTION:
     This structure contains the code for the category data

     code_type   : indicates if the codes are blacklist or whitelist
     mcc         : mobile country code digits in ASCII
     mnc_length  : mobile network code length
     mnc         : mobile network code digits in ASCII
     msin_length : mobile station identification number length
     msin        : msin digits in ASCII
     gid1        : group identity-1
     gid2        : group identity-2
     spn         : Service provider name
     iccid       : iccid digits
-------------------------------------------------------------------------------*/
typedef struct {
  simlock_code_enum_type  code_type;
  simlock_mcc_type        mcc;
  simlock_mnc_type        mnc;
  simlock_msin_type       msin;
  simlock_gid_type        gid1;
  simlock_gid_type        gid2;
  simlock_spn_type        spn;
  simlock_iccid_type      iccid;
} simlock_category_code_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_CATEGORY_FILE_DATA_TYPE

   DESCRIPTION:
     This structure contains the entire lock data for a category
 
     category_header  : header data for the category
     code_data_length : size of the code data
     code_data_ptr    : code data
-------------------------------------------------------------------------------*/
typedef struct
{
  simlock_category_header_data_type   category_header;
  uint32                              code_data_length;
  simlock_category_code_data_type   * code_data_ptr;
} simlock_category_file_data_type;

/*===========================================================================
FUNCTION SIMLOCK_INIT

DESCRIPTION
  This is the init function expected to be called during the power-up. If
  config file is not already present, it will create the file.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_init
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_SET_SIMLOCK_POLICY

DESCRIPTION
  This function sets the slot and subscription policy for the SIMLock. It
  checks if the SIMLock is disabled. If yes, it writes the policy to the
  config file located in SFS. If not, it'll return an error.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_simlock_policy
(
  const simlock_set_config_policy_msg_req_type  * policy_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_RUN_ALGORITHM

DESCRIPTION
  This is the core function for the SIMLock feature. It validates a SIM
  card present, by verifing the card data against the lock data for all the
  enabled categories. It then applies the configuration policy
  (slot and sub policy), to determine whether a subscription is valid on the
  card.
  If the SIMLock feature is not enabled, all the subcriptions on all the
  cards are valid

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_run_algorithm
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  simlock_run_algorithm_msg_resp_type        * resp_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_SET_LOCK_CK

DESCRIPTION
  This function sets and enables the SIMLock for a category. It takes the ck
  and the iteration count as the inputs and generates HCK needed for the
  lock. It also generates salt needed for generating the HCK. It then writes
  the lock data to a category file. Also, it updates the config file 
  indicating that the lock is enabled for the category, upon successfully
  updating the lock data.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_lock_ck
(
  const simlock_set_lock_ck_msg_req_type  * req_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_RELOCK_DEVICE

DESCRIPTION
  This function reenables the SIMLock for the passed category with the input
  CK. As a first step it makes sure that the category is disabled & its
  data - like salt, HCK is present in the file. Then HCK is generated using
  the input CK, stored salt & iteration count. Relock is allowed only when
  the generated & stored HCK matches. Finally, data related to retries & the
  category is updated in the respective files.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_relock_device
(
  const simlock_relock_device_msg_req_type  * req_msg_ptr,
  simlock_relock_device_msg_resp_type       * resp_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_UNLOCK_DEVICE

DESCRIPTION
  This function disables the SIMLock for a category. It takes the ck
  as the input. It reads the category file to retrieve the salt,
  iteration count and the HCK. It generates the HCK using the input ck and
  retrieved salt and iteration count. If the generated and retrieved HCK
  matches, it unlocks the device for the category. Also, it updates the
  config file indicating that the lock is disabled for the category,
  upon successfully unlocking.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_unlock_device
(
  const simlock_unlock_device_msg_req_type   * req_msg_ptr,
  simlock_unlock_device_msg_resp_type        * resp_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_GET_LOCK_STATUS

DESCRIPTION
  This function retrieves the status of all the categories on all the slots
  supported

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_get_lock_status
(
  simlock_get_status_msg_resp_type * resp_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_GET_CATEGORY_DATA

DESCRIPTION
  This function retrieves the code data of the requested category on 
  a slot

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_get_lock_category_data
(
  const simlock_get_category_data_msg_req_type * req_msg_ptr,
  simlock_get_category_data_msg_resp_type      * resp_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_SET_LOCK_HCK

DESCRIPTION
  This function sets and enables the SIMLock for a category. It writes the
  lock data to a category file. Also, it updates the config file indicating
  that the lock is enabled for the category, upon successfully updating
  the lock data.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_lock_hck
(
  const simlock_set_lock_hck_msg_req_type  * req_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_TEMPORARY_UNLOCK_REQUEST

DESCRIPTION
  This function sets the temporary unlock for a category or unsets a
  previous temporary unlock.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_temporary_unlock_request
(
  const simlock_temporary_unlock_msg_req_type  * req_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_SET_DEVICE_MODE_REQUEST

DESCRIPTION
  This function sets the device mode to emergency only or normal mode

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_set_device_mode_request
(
  const simlock_set_device_mode_msg_req_type  * req_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_ADD_LOCK_CODES_CK_REQUEST

DESCRIPTION
  This function processes the request to add lock codes to a category, which
  is already enabled. It reads the lock data of the requested category and
  checks if the CK matches. If it matches, the new lock codes are added to
  the existing codes and lock data is written to the category file.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_add_lock_codes_ck_request
(
  const simlock_add_lock_codes_ck_msg_req_type  * req_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_ADD_LOCK_CODES_HCK_REQUEST

DESCRIPTION
  This function processes the request to add lock codes to a category, which
  is already enabled. It reads the lock data of the requested category and
  checks if the HCK matches. If it matches, the new lock codes are added to
  the existing codes and lock data is written to the category file.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_add_lock_codes_hck_request
(
  const simlock_add_lock_codes_hck_msg_req_type  * req_msg_ptr
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_H */
