/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K   U T I L   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock utility functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_util.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   bcho    Added support for ICCID simlock
09/07/16   vdc     Use correct IMEI in OTA unlock and RSU blob validation
06/28/16   vdc     Derive correct category file for SPN & SIM categories
06/17/16   vdc     Added support for SPN based SIMLOCK feature
05/11/16   av      Skip checking simlock policies when no category is enabled
03/09/16   av      Support for autolock whitelist
01/14/16   vdc     Add more test MCC-MNCs to support remote simlock
01/06/16   stv     Remote simlock support
12/31/15   stv     Send events to client from engine msg handling
07/09/15   bcho    Set the upper limit to the token ids
05/15/15   vv      Support for RSU set configuration in simlock
05/15/15   vv      Support for RSU in simlock
05/15/15   stv     SIMLOCK time module related changes
05/04/15   vv      Support for more than 1 bytes of GID1/GID2
02/17/15   vv      Indicate the reason for invalid subscription
01/19/15   kr      Fixed compilation warnings
01/19/15   bcho    Support for identifying test SIMs
12/17/14   vv      Relocate the simlock data files
11/10/14   vv      Add support for setting emergency mode for all the RATs
09/29/14   vv      Added support for the remote SFS
09/29/14   vv      Added support for emergency only mode
09/17/14   tl      Add support for perso and deperso secure
06/20/14   vv      Added support for diag protection
05/12/14   vv      Fix compiler errors
04/15/14   vv      Added support for quad SIM
04/14/14   vv      Added support for identifying conflicting codes
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK
#include <stdlib.h>
#include "string.h"

#include "simlock_util.h"
#include "simlock_platform.h"
#include "simlock_modem.h"
#include "simlock_timer.h"

#include "nv.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define IS_VALID_CODE_CHAR(c)              \
  (((c >= '0' && c <= '9') || (c == SIMLOCK_LOCK_CODE_WILD_CHAR)) ? TRUE : FALSE)

/* Min MNC length is 2 */
#define SIMLOCK_UTIL_MIN_MNC_LEN       0x02

/* Min MSIN length is 2 (atleast d6 and d7)  */
#define SIMLOCK_UTIL_MIN_MSIN_LEN      0x02

/* Min and Max token ids that can be used in remote requests */
#define SIMLOCK_UTIL_TOKEN_ID_MIN      0x1
#define SIMLOCK_UTIL_TOKEN_ID_MAX      0x7FFF

typedef struct {
  simlock_mcc_type        mcc;
  simlock_mnc_type        mnc;
} simlock_mcc_mnc_data_type;

/* Table for test MCC/MNCs */
static const simlock_mcc_mnc_data_type simlock_test_mcc_mnc_tbl[] =
{
  /* Carrier specific test MCC/MNCs are added as per the requirement from carrier */
#ifdef FEATURE_SIMLOCK_RSU
  {{'3', '1', '0'}, {0x03, {'3', '1', '0'}}},
  {{'3', '1', '0'}, {0x03, {'3', '0', '0'}}},
#endif /* FEATURE_SIMLOCK_RSU */

  {{'0', '0', '1'}, {0x02, {'0', '1', 'F'}}}
};

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_UTIL_COMPOSE_CATEGORY_FILE_ENUM

DESCRIPTION
  This is a utility function that composes the category file enum for a
  category on a slot

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_data_file_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_data_file_enum_type simlock_util_compose_category_file_enum
(
  simlock_category_enum_type  category,
  simlock_slot_enum_type      slot
)
{
  simlock_data_file_enum_type  file_enum[SIMLOCK_SLOT_COUNT_MAX][SIMLOCK_CATEGORY_COUNT_MAX] =
  {{SIMLOCK_FILE_3GPP_NW_1, SIMLOCK_FILE_3GPP_NS_1, SIMLOCK_FILE_3GPP_SP_1, SIMLOCK_FILE_3GPP_CP_1,
    SIMLOCK_FILE_3GPP_SPN_1, SIMLOCK_FILE_3GPP_SP_EHPLMN_1, SIMLOCK_FILE_3GPP_ICCID_1, SIMLOCK_FILE_3GPP_SIM_1,
    SIMLOCK_FILE_3GPP2_NW_TYPE1_1, SIMLOCK_FILE_3GPP2_NW_TYPE2_1, SIMLOCK_FILE_3GPP2_HRPD_1, SIMLOCK_FILE_3GPP2_SP_1,
    SIMLOCK_FILE_3GPP2_CP_1, SIMLOCK_FILE_3GPP2_RUIM_1},
   {SIMLOCK_FILE_3GPP_NW_2, SIMLOCK_FILE_3GPP_NS_2, SIMLOCK_FILE_3GPP_SP_2, SIMLOCK_FILE_3GPP_CP_2,
    SIMLOCK_FILE_3GPP_SPN_2, SIMLOCK_FILE_3GPP_SP_EHPLMN_2, SIMLOCK_FILE_3GPP_ICCID_2, SIMLOCK_FILE_3GPP_SIM_2,
    SIMLOCK_FILE_3GPP2_NW_TYPE1_2, SIMLOCK_FILE_3GPP2_NW_TYPE2_2, SIMLOCK_FILE_3GPP2_HRPD_2, SIMLOCK_FILE_3GPP2_SP_2,
    SIMLOCK_FILE_3GPP2_CP_2, SIMLOCK_FILE_3GPP2_RUIM_2},
   {SIMLOCK_FILE_3GPP_NW_3, SIMLOCK_FILE_3GPP_NS_3, SIMLOCK_FILE_3GPP_SP_3, SIMLOCK_FILE_3GPP_CP_3,
    SIMLOCK_FILE_3GPP_SPN_3, SIMLOCK_FILE_3GPP_SP_EHPLMN_3, SIMLOCK_FILE_3GPP_ICCID_3, SIMLOCK_FILE_3GPP_SIM_3,
    SIMLOCK_FILE_3GPP2_NW_TYPE1_3, SIMLOCK_FILE_3GPP2_NW_TYPE2_3, SIMLOCK_FILE_3GPP2_HRPD_3, SIMLOCK_FILE_3GPP2_SP_3,
    SIMLOCK_FILE_3GPP2_CP_3, SIMLOCK_FILE_3GPP2_RUIM_3},
   {SIMLOCK_FILE_3GPP_NW_4, SIMLOCK_FILE_3GPP_NS_4, SIMLOCK_FILE_3GPP_SP_4, SIMLOCK_FILE_3GPP_CP_4,
    SIMLOCK_FILE_3GPP_SPN_4, SIMLOCK_FILE_3GPP_SP_EHPLMN_4, SIMLOCK_FILE_3GPP_ICCID_4, SIMLOCK_FILE_3GPP_SIM_4,
    SIMLOCK_FILE_3GPP2_NW_TYPE1_4, SIMLOCK_FILE_3GPP2_NW_TYPE2_4, SIMLOCK_FILE_3GPP2_HRPD_4, SIMLOCK_FILE_3GPP2_SP_4,
    SIMLOCK_FILE_3GPP2_CP_4, SIMLOCK_FILE_3GPP2_RUIM_4}};

  if(simlock_util_check_slot(slot) != SIMLOCK_SUCCESS)
  {
    return SIMLOCK_FILE_ENUM_INVALID;
  }

  if(category > SIMLOCK_CATEGORY_COUNT_MAX)
  {
    return SIMLOCK_FILE_ENUM_INVALID;
  }

  return file_enum[slot][category];
} /* simlock_util_compose_category_file_enum */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_GET_FILE_PATH_FROM_ENUM

DESCRIPTION
  This is a utility function that returns the simlock data file local path
  corresponding to the file enum

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_util_get_file_path_from_enum
(
  simlock_data_file_enum_type  file_enum,
  simlock_filename_type        file_name
)
{
  char  * append_file_ptr = NULL;
  boolean category_file   = FALSE;

  if(strlcpy(file_name, "/safe/sfs/uim/simlock", SIMLOCK_FILE_PATH_LEN) >= SIMLOCK_FILE_PATH_LEN)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  switch(file_enum)
  {
     case SIMLOCK_FILE_3GPP_NW_1:
       append_file_ptr = "3gpp_nw_1";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_NS_1:
       append_file_ptr = "3gpp_ns_1";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SP_1:
       append_file_ptr = "3gpp_sp_1";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_CP_1:
       append_file_ptr = "3gpp_cp_1";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SIM_1:
       append_file_ptr = "3gpp_sim_1";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SPN_1:
       append_file_ptr = "3gpp_spn_1";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_ICCID_1:
       append_file_ptr = "3gpp_iccid_1";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_NW_TYPE1_1:
       append_file_ptr = "3gpp2_nw_type1_1";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_NW_TYPE2_1:
       append_file_ptr = "3gpp2_nw_type2_1";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_RUIM_1:
       append_file_ptr = "3gpp2_ruim_1";
       category_file   = TRUE;
       break;

     case SIMLOCK_FILE_3GPP_NW_2:
       append_file_ptr = "3gpp_nw_2";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_NS_2:
       append_file_ptr = "3gpp_ns_2";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SP_2:
       append_file_ptr = "3gpp_sp_2";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_CP_2:
       append_file_ptr = "3gpp_cp_2";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SIM_2:
       append_file_ptr = "3gpp_sim_2";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SPN_2:
       append_file_ptr = "3gpp_spn_2";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_ICCID_2:
       append_file_ptr = "3gpp_iccid_2";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_NW_TYPE1_2:
       append_file_ptr = "3gpp2_nw_type1_2";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_NW_TYPE2_2:
       append_file_ptr = "3gpp2_nw_type2_2";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_RUIM_2:
       append_file_ptr = "3gpp2_ruim_2";
       category_file   = TRUE;
       break;

     case SIMLOCK_FILE_3GPP_NW_3:
       append_file_ptr = "3gpp_nw_3";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_NS_3:
       append_file_ptr = "3gpp_ns_3";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SP_3:
       append_file_ptr = "3gpp_sp_3";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_CP_3:
       append_file_ptr = "3gpp_cp_3";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SIM_3:
       append_file_ptr = "3gpp_sim_3";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SPN_3:
       append_file_ptr = "3gpp_spn_3";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_ICCID_3:
       append_file_ptr = "3gpp_iccid_3";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_NW_TYPE1_3:
       append_file_ptr = "3gpp2_nw_type1_3";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_NW_TYPE2_3:
       append_file_ptr = "3gpp2_nw_type2_3";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_RUIM_3:
       append_file_ptr = "3gpp2_ruim_3";
       category_file   = TRUE;
       break;

     case SIMLOCK_FILE_3GPP_NW_4:
       append_file_ptr = "3gpp_nw_4";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_NS_4:
       append_file_ptr = "3gpp_ns_4";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SP_4:
       append_file_ptr = "3gpp_sp_4";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_CP_4:
       append_file_ptr = "3gpp_cp_4";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SIM_4:
       append_file_ptr = "3gpp_sim_4";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_SPN_4:
       append_file_ptr = "3gpp_spn_4";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP_ICCID_4:
       append_file_ptr = "3gpp_iccid_4";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_NW_TYPE1_4:
       append_file_ptr = "3gpp2_nw_type1_4";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_NW_TYPE2_4:
       append_file_ptr = "3gpp2_nw_type2_4";
       category_file   = TRUE;
       break;
     case SIMLOCK_FILE_3GPP2_RUIM_4:
       append_file_ptr = "3gpp2_ruim_4";
       category_file   = TRUE;
       break;

     case SIMLOCK_FILE_CONFIG:
       append_file_ptr = "config";
       break;

     case SIMLOCK_FILE_TIME_INFO:
       append_file_ptr = "time_info";
       break;

     case SIMLOCK_FILE_RSU_PUBKEY:
       append_file_ptr = "rsu_pubkey";
       break;

     case SIMLOCK_FILE_RSU_KEY_MOD:
       append_file_ptr = "rsu_keymod";
       break;

     case SIMLOCK_FILE_TEMP_UNLOCK:
       append_file_ptr = "tunlock";
       break;

     case SIMLOCK_FILE_REMOTE_BLOB_TIME:
       append_file_ptr = "remote_blob_time";
       break;

     default:
       break;
  }

  if(append_file_ptr == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  if(strlcat(file_name, "/", SIMLOCK_FILE_PATH_LEN) >= SIMLOCK_FILE_PATH_LEN)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  if(category_file)
  {
    if(strlcat(file_name, "category_", SIMLOCK_FILE_PATH_LEN) >= SIMLOCK_FILE_PATH_LEN)
    {
      return SIMLOCK_GENERIC_ERROR;
    }
  }

  if(strlcat(file_name, append_file_ptr, SIMLOCK_FILE_PATH_LEN) >= SIMLOCK_FILE_PATH_LEN)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_util_get_file_path_from_enum */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_CODE_DATA

DESCRIPTION
  This is a utility function that checks if the lock codes are present

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_util_check_code_data
(
  const simlock_category_data_type * category_data_ptr,
  boolean                            auto_lock
)
{
  simlock_result_enum_type simlock_status = SIMLOCK_SUCCESS;
  uint8                    code_cnt       = 0;

  if(category_data_ptr == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  SIMLOCK_MSG_MED_0("simlock_util_check_code_data");

  switch(category_data_ptr->category_type)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      /* check the number of codes */
      if((category_data_ptr->code_data.nw_3gpp_code_list.num_nw_codes == 0 &&
          !auto_lock) ||
         (category_data_ptr->code_data.nw_3gpp_code_list.num_nw_codes > SIMLOCK_NUM_CODES_MAX))
      {
        return SIMLOCK_INVALID_LOCK_DATA;
      }

      /* check if the MCC/MNCs are valid */
      for(code_cnt = 0;
          code_cnt < category_data_ptr->code_data.nw_3gpp_code_list.num_nw_codes &&
          code_cnt < SIMLOCK_NUM_CODES_MAX;
          code_cnt++)
      {
        if(!simlock_util_check_is_mcc_valid(category_data_ptr->code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mcc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_mnc_valid(category_data_ptr->code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mnc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_NS:
      /* check the number of codes */
      if((category_data_ptr->code_data.ns_3gpp_code_list.num_ns_codes == 0 &&
          !auto_lock) ||
         (category_data_ptr->code_data.ns_3gpp_code_list.num_ns_codes > SIMLOCK_NUM_CODES_MAX))
      {
        return SIMLOCK_INVALID_LOCK_DATA;
      }

      /* check if the MCC/MNCs and d_6/d_7 are valid */
      for(code_cnt = 0;
          code_cnt < category_data_ptr->code_data.ns_3gpp_code_list.num_ns_codes &&
          code_cnt < SIMLOCK_NUM_CODES_MAX;
          code_cnt++)
      {
        if(!simlock_util_check_is_mcc_valid(category_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mcc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_mnc_valid(category_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mnc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!IS_VALID_CODE_CHAR(category_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[code_cnt].imsi_digit6) ||
           !IS_VALID_CODE_CHAR(category_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[code_cnt].imsi_digit7))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SP:
      /* check the number of codes */
      if((category_data_ptr->code_data.sp_3gpp_code_list.num_sp_codes == 0 &&
          !auto_lock) ||
         (category_data_ptr->code_data.sp_3gpp_code_list.num_sp_codes > SIMLOCK_NUM_CODES_MAX))
      {
        return SIMLOCK_INVALID_LOCK_DATA;
      }

      /* check if the MCC/MNCs are valid */
      for(code_cnt = 0;
          code_cnt < category_data_ptr->code_data.sp_3gpp_code_list.num_sp_codes &&
          code_cnt < SIMLOCK_NUM_CODES_MAX;
          code_cnt++)
      {
        if(!simlock_util_check_is_mcc_valid(category_data_ptr->code_data.sp_3gpp_code_list.sp_code_data[code_cnt].nw_code.mcc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_mnc_valid(category_data_ptr->code_data.sp_3gpp_code_list.sp_code_data[code_cnt].nw_code.mnc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        /* gid1 can be any uint8 value from 0 - FF
           (FF being allowed wild char). So no specific check performed */
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_CP:
      /* check the number of codes */
      if((category_data_ptr->code_data.cp_3gpp_code_list.num_cp_codes == 0 &&
          !auto_lock) ||
         (category_data_ptr->code_data.cp_3gpp_code_list.num_cp_codes > SIMLOCK_NUM_CODES_MAX))
      {
        return SIMLOCK_INVALID_LOCK_DATA;
      }

      /* check if the MCC/MNCs are valid */
      for(code_cnt = 0;
          code_cnt < category_data_ptr->code_data.cp_3gpp_code_list.num_cp_codes &&
          code_cnt < SIMLOCK_NUM_CODES_MAX;
          code_cnt++)
      {
        if(!simlock_util_check_is_mcc_valid(category_data_ptr->code_data.cp_3gpp_code_list.cp_code_data[code_cnt].nw_code.mcc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_mnc_valid(category_data_ptr->code_data.cp_3gpp_code_list.cp_code_data[code_cnt].nw_code.mnc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        /* gid1 and gid2 can be any uint8 value from 0 - FF
           (FF being allowed wild char). So no specific check performed */
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SIM:
      /* check the number of codes */
      if((category_data_ptr->code_data.sim_3gpp_code_list.num_sim_codes == 0 &&
          !auto_lock) ||
         (category_data_ptr->code_data.sim_3gpp_code_list.num_sim_codes > SIMLOCK_NUM_CODES_MAX))
      {
        return SIMLOCK_INVALID_LOCK_DATA;
      }

      /* check if the MCC/MNCs and MSINs are valid */
      for(code_cnt = 0;
          code_cnt < category_data_ptr->code_data.sim_3gpp_code_list.num_sim_codes &&
          code_cnt < SIMLOCK_NUM_CODES_MAX;
          code_cnt++)
      {
        if(!simlock_util_check_is_mcc_valid(category_data_ptr->code_data.sim_3gpp_code_list.sim_code_data[code_cnt].nw_code.mcc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_mnc_valid(category_data_ptr->code_data.sim_3gpp_code_list.sim_code_data[code_cnt].nw_code.mnc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_msin_valid(category_data_ptr->code_data.sim_3gpp_code_list.sim_code_data[code_cnt].imsi_msin))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SPN:
      /* check the number of codes */
      if((category_data_ptr->code_data.spn_3gpp_code_list.num_spn_codes == 0 &&
          !auto_lock) ||
         (category_data_ptr->code_data.spn_3gpp_code_list.num_spn_codes > SIMLOCK_NUM_CODES_MAX))
      {
        return SIMLOCK_INVALID_LOCK_DATA;
      }

      /* check if the MCC/MNCs are valid */
      for(code_cnt = 0;
          code_cnt < category_data_ptr->code_data.spn_3gpp_code_list.num_spn_codes &&
          code_cnt < SIMLOCK_NUM_CODES_MAX;
          code_cnt++)
      {
        if(!simlock_util_check_is_mcc_valid(category_data_ptr->code_data.spn_3gpp_code_list.spn_code_data[code_cnt].nw_code.mcc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_mnc_valid(category_data_ptr->code_data.spn_3gpp_code_list.spn_code_data[code_cnt].nw_code.mnc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_spn_valid(category_data_ptr->code_data.spn_3gpp_code_list.spn_code_data[code_cnt].spn))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_ICCID:
      /* check the number of codes */
      if((category_data_ptr->code_data.iccid_3gpp_code_list.num_iccid_codes == 0 &&
          !auto_lock) ||
         (category_data_ptr->code_data.iccid_3gpp_code_list.num_iccid_codes > SIMLOCK_NUM_CODES_MAX))
      {
        return SIMLOCK_INVALID_LOCK_DATA;
      }

      /* check if the MCC/MNCs are valid */
      for(code_cnt = 0;
          code_cnt < category_data_ptr->code_data.iccid_3gpp_code_list.num_iccid_codes &&
          code_cnt < SIMLOCK_NUM_CODES_MAX;
          code_cnt++)
      {
        if(!simlock_util_check_is_mcc_valid(category_data_ptr->code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].nw_code.mcc) ||
           !simlock_util_check_is_mnc_valid(category_data_ptr->code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].nw_code.mnc) ||
           !simlock_util_check_is_iccid_valid(category_data_ptr->code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].iccid))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
      /* check the number of codes */
      if((category_data_ptr->code_data.nw_type1_3gpp2_code_list.num_nw_codes == 0 &&
          !auto_lock) ||
         (category_data_ptr->code_data.nw_type1_3gpp2_code_list.num_nw_codes > SIMLOCK_NUM_CODES_MAX))
      {
        return SIMLOCK_INVALID_LOCK_DATA;
      }

      /* check if the MCC/MNCs are valid */
      for(code_cnt = 0;
          code_cnt < category_data_ptr->code_data.nw_type1_3gpp2_code_list.num_nw_codes &&
          code_cnt < SIMLOCK_NUM_CODES_MAX;
          code_cnt++)
      {
        if(!simlock_util_check_is_mcc_valid(category_data_ptr->code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mcc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_mnc_valid(category_data_ptr->code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mnc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
      /* check the number of codes */
      if((category_data_ptr->code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes == 0 &&
          !auto_lock) ||
         (category_data_ptr->code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes > SIMLOCK_NUM_CODES_MAX))
      {
        return SIMLOCK_INVALID_LOCK_DATA;
      }

      /* check if the IRMs are valid */
      for(code_cnt = 0;
          code_cnt < category_data_ptr->code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes &&
          code_cnt < SIMLOCK_NUM_CODES_MAX;
          code_cnt++)
      {
        if(!simlock_util_check_is_irm_valid(category_data_ptr->code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[code_cnt].irm))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_RUIM:
      /* check the number of codes */
      if((category_data_ptr->code_data.ruim_3gpp2_code_list.num_sim_codes == 0 &&
          !auto_lock) ||
         (category_data_ptr->code_data.ruim_3gpp2_code_list.num_sim_codes > SIMLOCK_NUM_CODES_MAX))
      {
        return SIMLOCK_INVALID_LOCK_DATA;
      }

      /* check if the MCC/MNCs and MSINs are valid */
      for(code_cnt = 0;
          code_cnt < category_data_ptr->code_data.ruim_3gpp2_code_list.num_sim_codes &&
          code_cnt < SIMLOCK_NUM_CODES_MAX;
          code_cnt++)
      {
        if(!simlock_util_check_is_mcc_valid(category_data_ptr->code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mcc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_mnc_valid(category_data_ptr->code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mnc))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
        if(!simlock_util_check_is_msin_valid(category_data_ptr->code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin))
        {
          return SIMLOCK_INVALID_LOCK_DATA;
        }
      }
      break;

    default:
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
  }

  return simlock_status;
} /* simlock_util_check_code_data */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_CATEGORY_SUPPORTED

DESCRIPTION
  This is a utility function that checks if a category is supported

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_util_check_category_supported
(
  simlock_category_enum_type category_type
)
{
  if((category_type == SIMLOCK_CATEGORY_3GPP2_HRPD)  ||
     (category_type == SIMLOCK_CATEGORY_3GPP2_SP)    ||
     (category_type == SIMLOCK_CATEGORY_3GPP2_CP))
  {
    SIMLOCK_MSG_MED_1("category 0x%x not supported", category_type);
    return SIMLOCK_UNSUPPORTED;
  }
  return SIMLOCK_SUCCESS;
} /* simlock_util_check_category_supported */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_SLOT

DESCRIPTION
  This is a utility function that checks if a slot is supported

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_util_check_slot
(
  simlock_slot_enum_type  slot
)
{
  if((slot != SIMLOCK_SLOT_1) &&
     (slot != SIMLOCK_SLOT_2) &&
     (slot != SIMLOCK_SLOT_3) &&
     (slot != SIMLOCK_SLOT_4))
  {
    SIMLOCK_MSG_ERR_1("invalid slot, 0x%x", slot);
    return SIMLOCK_UNSUPPORTED;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_util_check_slot */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IF_MCC_MATCHES

DESCRIPTION
  This is a utility function that checks if the MCCs match

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the MCCs match
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_if_mcc_matches
(
  const simlock_mcc_type  lock_mcc,
  const simlock_mcc_type  sim_mcc
)
{
  uint8    mcc_index = 0;
  boolean  match     = TRUE;

  for(mcc_index = 0; mcc_index < SIMLOCK_MCC_LEN;
      mcc_index++)
  {
    if(lock_mcc[mcc_index] == SIMLOCK_LOCK_CODE_WILD_CHAR)
    {
      continue;
    }
    if(lock_mcc[mcc_index] != sim_mcc[mcc_index])
    {
      match = FALSE;
      break;
    }
  }

  return match;
} /* simlock_util_check_if_mcc_matches */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IF_MNC_MATCHES

DESCRIPTION
  This is a utility function that checks if the MNCs match

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the MNCs match
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_if_mnc_matches
(
  simlock_mnc_type  lock_mnc,
  simlock_mnc_type  sim_mnc
)
{
  uint8    mnc_index = 0;
  boolean  match     = TRUE;

  if((lock_mnc.mnc_len > SIMLOCK_MNC_MAX) ||
     (sim_mnc.mnc_len > SIMLOCK_MNC_MAX))
  {
    return FALSE;
  }

  /* check the lengths */
  if(lock_mnc.mnc_len != sim_mnc.mnc_len)
  {
    return FALSE;
  }

  for(mnc_index = 0; mnc_index < lock_mnc.mnc_len &&
                     mnc_index < SIMLOCK_MNC_MAX; mnc_index++)
  {
    if(lock_mnc.mnc_data[mnc_index] == SIMLOCK_LOCK_CODE_WILD_CHAR)
    {
      continue;
    }
    if(lock_mnc.mnc_data[mnc_index] !=
         sim_mnc.mnc_data[mnc_index])
    {
      match = FALSE;
      break;
    }
  }

  return match;
} /* simlock_util_check_if_mnc_matches */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IF_MSIN_MATCHES

DESCRIPTION
  This is a utility function that checks if the MSINs match

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the MSINs match
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_if_msin_matches
(
  simlock_msin_type   lock_msin,
  simlock_msin_type   sim_msin
)
{
  uint8    msin_index = 0;
  boolean  match      = TRUE;

  if((lock_msin.msin_len > SIMLOCK_MSIN_MAX) ||
     (sim_msin.msin_len > SIMLOCK_MSIN_MAX))
  {
    return FALSE;
  }

  /* check the lengths */
  if(lock_msin.msin_len != sim_msin.msin_len)
  {
    return FALSE;
  }

  for(msin_index = 0; msin_index < lock_msin.msin_len &&
                      msin_index < SIMLOCK_MSIN_MAX; msin_index++)
  {
    if(lock_msin.msin_data[msin_index] == SIMLOCK_LOCK_CODE_WILD_CHAR)
    {
      continue;
    }
    if(lock_msin.msin_data[msin_index] !=
         sim_msin.msin_data[msin_index])
    {
      match = FALSE;
      break;
    }
  }

  return match;
} /* simlock_util_check_if_msin_matches */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IF_IRM_MATCHES

DESCRIPTION
  This is a utility function that checks if the IRMs match

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the IRMs match
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_if_irm_matches
(
  simlock_msin_type    lock_msin,
  simlock_msin_type    sim_msin
)
{
  uint8    irm_index = 0;
  boolean  match     = TRUE;

  if((lock_msin.msin_len < SIMLOCK_IRM_LEN) ||
     (sim_msin.msin_len < SIMLOCK_IRM_LEN))
  {
    return FALSE;
  }

  for(irm_index = 0; irm_index < SIMLOCK_IRM_LEN;
      irm_index++)
  {
    if(lock_msin.msin_data[irm_index] == SIMLOCK_LOCK_CODE_WILD_CHAR)
    {
      continue;
    }
    if(lock_msin.msin_data[irm_index] != sim_msin.msin_data[irm_index])
    {
      match = FALSE;
      break;
    }
  }

  return match;
} /* simlock_util_check_if_irm_matches */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IF_GID_MATCHES

DESCRIPTION
  This is a utility function that checks if the GIDs match

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the GIDs match
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_if_gid_matches
(
  simlock_gid_type       lock_gid,
  simlock_gid_type       sim_gid
)
{
  uint8 index = 0;

  /* The GID on the card must be at least as long as the GID in the SimLock
     configuration */
  if (sim_gid.gid_len < lock_gid.gid_len)
  {
    return FALSE;
  }

  for(index = 0; index < lock_gid.gid_len; index++)
  {
    if(lock_gid.gid_data[index] == SIMLOCK_LOCK_CODE_WILD_DIGIT)
    {
      continue;
    }

    if(sim_gid.gid_data[index] != lock_gid.gid_data[index])
    {
      return FALSE;
    }
  }

  return TRUE;
} /* simlock_util_check_if_gid_matches */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IF_IMSI_DIGIT_MATCHES

DESCRIPTION
  This is a utility function that checks if the IMSI digits (d6 or d7)
  match

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the GIDs match
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_if_imsi_digit_matches
(
  char        lock_digit,
  char        sim_digit
)
{
  if(lock_digit == SIMLOCK_LOCK_CODE_WILD_CHAR)
  {
    return TRUE;
  }
  if(lock_digit == sim_digit)
  {
    return TRUE;
  }

  return FALSE;
} /* simlock_util_check_if_imsi_digit_matches */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IF_SPN_MATCHES

DESCRIPTION
  This is a utility function that checks if the SPNs match

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the SPNs match
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_if_spn_matches
(
  const simlock_spn_type *lock_spn_ptr,
  const simlock_spn_type *sim_spn_ptr
)
{
  if(lock_spn_ptr == NULL     ||
     sim_spn_ptr  == NULL     ||
     !sim_spn_ptr->spn_valid  ||
     !lock_spn_ptr->spn_valid ||
     memcmp(lock_spn_ptr->spn_data, sim_spn_ptr->spn_data, SIMLOCK_SPN_LEN) != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* simlock_util_check_if_spn_matches */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IF_ICCID_MATCHES

DESCRIPTION
  This is a utility function that checks if the ICCIDs match

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the ICCIDs match
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_if_iccid_matches
(
  simlock_iccid_type lock_iccid,
  simlock_iccid_type sim_iccid
)
{
  uint8    iccid_index = 0;

  /* check the lengths */
  if(lock_iccid.iccid_len > SIMLOCK_ICCID_DIGITS_MAX ||
     sim_iccid.iccid_len > SIMLOCK_ICCID_DIGITS_MAX ||
     sim_iccid.iccid_len < lock_iccid.iccid_len)
  {
    return FALSE;
  }

  for(iccid_index = 0; iccid_index < lock_iccid.iccid_len; iccid_index++)
  {
    if(lock_iccid.iccid_data[iccid_index] == SIMLOCK_LOCK_CODE_WILD_CHAR)
    {
      continue;
    }

    if(sim_iccid.iccid_data[iccid_index] != lock_iccid.iccid_data[iccid_index])
    {
      return FALSE;
    }
  }

  return TRUE;
} /* simlock_util_check_if_iccid_matches */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_GET_SLOT_INDEX

DESCRIPTION
  This is a utility function that gets the slot index corresponding to a
  slot id

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_util_get_slot_index
(
  simlock_slot_enum_type    slot,
  uint8                   * slot_index
)
{
  if(slot_index == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  switch(slot)
  {
     case SIMLOCK_SLOT_1:
       *slot_index = 0;
       break;
     case SIMLOCK_SLOT_2:
       *slot_index = 1;
       break;
     case SIMLOCK_SLOT_3:
       *slot_index = 2;
       break;
     case SIMLOCK_SLOT_4:
       *slot_index = 3;
       break;
     default:
       SIMLOCK_MSG_ERR_1("invalid slot, 0x%x", slot);
       return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_util_get_slot_index */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_GET_CATEGORY_INDEX

DESCRIPTION
  This is a utility function that gets the category index corresponding to a
  category

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type  simlock_util_get_category_index
(
  simlock_category_enum_type     category,
  uint8                       *  category_index
)
{
  if(category_index == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  switch(category)
  {
     case SIMLOCK_CATEGORY_3GPP_NW:
       *category_index = 0;
       break;
     case SIMLOCK_CATEGORY_3GPP_NS:
       *category_index = 1;
       break;
     case SIMLOCK_CATEGORY_3GPP_SP:
       *category_index = 2;
       break;
     case SIMLOCK_CATEGORY_3GPP_CP:
       *category_index = 3;
       break;
     case SIMLOCK_CATEGORY_3GPP_SIM:
       *category_index = 4;
       break;
     case SIMLOCK_CATEGORY_3GPP_SPN:
       *category_index = 5;
       break;
     case SIMLOCK_CATEGORY_3GPP_SP_EHPLMN:
       *category_index = 6;
       break;
     case SIMLOCK_CATEGORY_3GPP_ICCID:
       *category_index = 7;
       break;

     case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
       *category_index = 8;
       break;
     case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
       *category_index = 9;
       break;
     case SIMLOCK_CATEGORY_3GPP2_HRPD:
       *category_index = 10;
       break;
     case SIMLOCK_CATEGORY_3GPP2_SP:
       *category_index = 11;
       break;
     case SIMLOCK_CATEGORY_3GPP2_CP:
       *category_index = 12;
       break;
     case SIMLOCK_CATEGORY_3GPP2_RUIM:
       *category_index = 13;
       break;
     default:
       SIMLOCK_MSG_ERR_1("invalid category, 0x%x", category);
       return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_util_get_category_index */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_CATEGORY_ENABLED

DESCRIPTION
  This is a utility function that checks if a category is enabled

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE  : If the category for the slot is enabled
  FALSE : Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_category_enabled
(
  const simlock_config_data_type   * config_data_ptr,
  simlock_category_enum_type         category,
  simlock_slot_enum_type             slot
)
{
  simlock_result_enum_type simlock_status  = SIMLOCK_SUCCESS;
  uint8                    slot_index      = 0;
  uint8                    category_index  = 0;

  if(config_data_ptr == NULL)
  {
    return FALSE;
  }

  simlock_status = simlock_util_get_slot_index(slot, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    return FALSE;
  }

  simlock_status = simlock_util_get_category_index(category, &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    return FALSE;
  }

  if(config_data_ptr->category_status[slot_index][category_index].status == SIMLOCK_ENABLED)
  {
    return TRUE;
  }

  return FALSE;
} /* simlock_util_check_category_enabled */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IS_MCC_VALID

DESCRIPTION
  This is a utility function that checks if the MCCs is valid
  The acceptable MCC digits are 0 - 9 and '*' ASCII characters

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the MCC is valid
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_is_mcc_valid
(
  const simlock_mcc_type  lock_mcc
)
{
  if((IS_VALID_CODE_CHAR(lock_mcc[0])) &&
     (IS_VALID_CODE_CHAR(lock_mcc[1])) &&
     (IS_VALID_CODE_CHAR(lock_mcc[2])))
  {
    return TRUE;
  }

  SIMLOCK_MSG_MED_3("mcc invalid - mcc[0]:0x%x, mcc[1]:0x%x, mcc[2]:0x%x",
                    lock_mcc[0], lock_mcc[1], lock_mcc[2]);

  return FALSE;
} /* simlock_util_check_is_mcc_valid */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IS_MNC_VALID

DESCRIPTION
  This is a utility function that checks if the MNC is valid
  The acceptable MNC digits are 0 - 9 and '*' ASCII characters

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the MNC is valid
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_is_mnc_valid
(
  simlock_mnc_type  lock_mnc
)
{
  uint8  mnc_index  = 0;

  /* valid mnc_len is 2 or 3 */
  if((lock_mnc.mnc_len < SIMLOCK_UTIL_MIN_MNC_LEN) ||
     (lock_mnc.mnc_len > SIMLOCK_MNC_MAX))
  {
    return FALSE;
  }

  for(mnc_index = 0; mnc_index < lock_mnc.mnc_len &&
                     mnc_index < SIMLOCK_MNC_MAX; mnc_index++)
  {
    if(!IS_VALID_CODE_CHAR(lock_mnc.mnc_data[mnc_index]))
    {
      SIMLOCK_MSG_MED_1("mnc digit 0x%x invalid",
                        lock_mnc.mnc_data[mnc_index]);
      return FALSE;
    }
  }

  return TRUE;
} /* simlock_util_check_is_mnc_valid */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IS_MSIN_VALID

DESCRIPTION
  This is a utility function that checks if the MSIN is valid
  The acceptable MSIN digits are 0 - 9 and '*' ASCII characters

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the MSIN is valid
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_is_msin_valid
(
  simlock_msin_type  lock_msin
)
{
  uint8  msin_index  = 0;

  if((lock_msin.msin_len < SIMLOCK_UTIL_MIN_MSIN_LEN) ||
     (lock_msin.msin_len > SIMLOCK_MSIN_MAX))
  {
    return FALSE;
  }

  for(msin_index = 0; msin_index < lock_msin.msin_len &&
                      msin_index < SIMLOCK_MSIN_MAX; msin_index++)
  {
    if(!IS_VALID_CODE_CHAR(lock_msin.msin_data[msin_index]))
    {
      SIMLOCK_MSG_MED_1("msin digit 0x%x invalid",
                        lock_msin.msin_data[msin_index]);
      return FALSE;
    }
  }

  return TRUE;
} /* simlock_util_check_is_msin_valid */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IS_SPN_VALID

DESCRIPTION
  This is a utility function that checks if the SPN is valid

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the SPN is valid
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_is_spn_valid
(
  simlock_spn_type spn
)
{
  /* Check for validity flag as of now. It is possible that additional checks
     can be extended in future */
  return spn.spn_valid;
} /* simlock_util_check_is_spn_valid */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IS_IRM_VALID

DESCRIPTION
  This is a utility function that checks if the IRM is valid
  The acceptable IRM digits are 0 - 9 and '*' ASCII characters

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the IRM is valid
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_is_irm_valid
(
  const simlock_irm_type  lock_irm
)
{
  if((IS_VALID_CODE_CHAR(lock_irm[0])) &&
     (IS_VALID_CODE_CHAR(lock_irm[1])) &&
     (IS_VALID_CODE_CHAR(lock_irm[2])) &&
     (IS_VALID_CODE_CHAR(lock_irm[3])))
  {
    return TRUE;
  }

  SIMLOCK_MSG_MED_4("irm invalid - irm[0]:0x%x, irm[1]:0x%x, irm[2]:0x%x, irm[3]:0x%x",
                    lock_irm[0], lock_irm[1], lock_irm[2], lock_irm[3]);

  return FALSE;
} /* simlock_util_check_is_irm_valid */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IS_ICCID_VALID

DESCRIPTION
  This is a utility function that checks if the ICCID is valid
  The acceptable ICCID digits are 0 - 9 and '*' ASCII characters

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the ICCID is valid
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_is_iccid_valid
(
  simlock_iccid_type  lock_iccid
)
{
  uint8  iccid_index  = 0;

  if(lock_iccid.iccid_len > SIMLOCK_ICCID_DIGITS_MAX)
  {
    return FALSE;
  }

  for(iccid_index = 0; iccid_index < lock_iccid.iccid_len; iccid_index++)
  {
    if(!IS_VALID_CODE_CHAR(lock_iccid.iccid_data[iccid_index]))
    {
      SIMLOCK_MSG_MED_1("iccid digit 0x%x invalid",
                        lock_iccid.iccid_data[iccid_index]);
      return FALSE;
    }
  }

  return TRUE;
} /* simlock_util_check_is_iccid_valid */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IF_CODES_MATCH

DESCRIPTION
  This function checks if the code from a category that needs to be enabled
  matches with the code in the already enabled category.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If the codes match
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_if_codes_match
(
  const simlock_category_code_data_type   * enabled_code_data_ptr,
  const simlock_category_code_data_type   * lock_code_data_ptr,
  simlock_category_enum_type                enabled_category,
  simlock_category_enum_type                lock_category
)
{
  if((enabled_code_data_ptr == NULL) ||
     (lock_code_data_ptr == NULL))
  {
    return FALSE;
  }

  switch(lock_category)
  {
     case SIMLOCK_CATEGORY_3GPP_NW:
     case SIMLOCK_CATEGORY_3GPP_SP:
     case SIMLOCK_CATEGORY_3GPP_SPN:
     case SIMLOCK_CATEGORY_3GPP_ICCID:
       if(simlock_util_check_if_mcc_matches(enabled_code_data_ptr->mcc,
                                            lock_code_data_ptr->mcc)     &&
          simlock_util_check_if_mnc_matches(enabled_code_data_ptr->mnc,
                                            lock_code_data_ptr->mnc))
       {
         SIMLOCK_MSG_MED_0("match found for nw code");
         return TRUE;
       }
       break;

     case SIMLOCK_CATEGORY_3GPP_NS:
       if(simlock_util_check_if_mcc_matches(enabled_code_data_ptr->mcc,
                                            lock_code_data_ptr->mcc)     &&
          simlock_util_check_if_mnc_matches(enabled_code_data_ptr->mnc,
                                            lock_code_data_ptr->mnc))
       {
         if(enabled_category == SIMLOCK_CATEGORY_3GPP_SIM)
         {
           SIMLOCK_MSG_MED_0("checking SIM codes");
           if(enabled_code_data_ptr->msin.msin_len >= SIMLOCK_UTIL_MIN_MSIN_LEN)
           {
             if(simlock_util_check_if_imsi_digit_matches(enabled_code_data_ptr->msin.msin_data[0],
                                                         lock_code_data_ptr->msin.msin_data[0])       &&
               (simlock_util_check_if_imsi_digit_matches(enabled_code_data_ptr->msin.msin_data[1],
                                                         lock_code_data_ptr->msin.msin_data[1])))
             {
               SIMLOCK_MSG_MED_0("match found for ns code");
               return TRUE;
             }
           }
         } /* category == SIMLOCK_CATEGORY_3GPP_SIM */
         else
         {
           SIMLOCK_MSG_MED_0("match found for nw code");
           return TRUE;
         }
       }
       break;

     case SIMLOCK_CATEGORY_3GPP_CP:
       if(simlock_util_check_if_mcc_matches(enabled_code_data_ptr->mcc,
                                            lock_code_data_ptr->mcc)     &&
          simlock_util_check_if_mnc_matches(enabled_code_data_ptr->mnc,
                                            lock_code_data_ptr->mnc))
       {
         if(enabled_category == SIMLOCK_CATEGORY_3GPP_SP)
          {
            if(simlock_util_check_if_gid_matches(enabled_code_data_ptr->gid1,
                                                 lock_code_data_ptr->gid1))
            {
              SIMLOCK_MSG_MED_0("match found for cp code");
              return TRUE;
            }
          }
          else
          {
            SIMLOCK_MSG_MED_0("match found for cp code");
            return TRUE;
          }
       }
       break;

     case SIMLOCK_CATEGORY_3GPP_SIM:
       if(simlock_util_check_if_mcc_matches(enabled_code_data_ptr->mcc,
                                            lock_code_data_ptr->mcc)     &&
          simlock_util_check_if_mnc_matches(enabled_code_data_ptr->mnc,
                                            lock_code_data_ptr->mnc))
       {
         if(enabled_category == SIMLOCK_CATEGORY_3GPP_NS)
         {
           if(lock_code_data_ptr->msin.msin_len >= SIMLOCK_UTIL_MIN_MSIN_LEN)
           {
             if(simlock_util_check_if_imsi_digit_matches(enabled_code_data_ptr->msin.msin_data[0],
                                                         lock_code_data_ptr->msin.msin_data[0]) &&
                simlock_util_check_if_imsi_digit_matches(enabled_code_data_ptr->msin.msin_data[1],
                                                         lock_code_data_ptr->msin.msin_data[1]))
             {
               SIMLOCK_MSG_MED_0("match found for sim code");
               return TRUE;
             }
           }
         }
         else
         {
           SIMLOCK_MSG_MED_0("match found for nw code");
           return TRUE;
         }
       }
       break;

     case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
       /* nw_type1 and nw_type2 are independent */
       if(enabled_category == SIMLOCK_CATEGORY_3GPP2_NW_TYPE2)
       {
         return TRUE;
       }
       if(simlock_util_check_if_mcc_matches(enabled_code_data_ptr->mcc,
                                            lock_code_data_ptr->mcc)     &&
          simlock_util_check_if_mnc_matches(enabled_code_data_ptr->mnc,
                                            lock_code_data_ptr->mnc))
       {
         SIMLOCK_MSG_MED_0("match found for nw code");
         return TRUE;
       }
       break;

     case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
       /* nw_type2 and nw_type1 are independent */
       if(enabled_category == SIMLOCK_CATEGORY_3GPP2_NW_TYPE1)
       {
         SIMLOCK_MSG_MED_0("match found for nw_type2 code");
         return TRUE;
       }
       if(enabled_category == SIMLOCK_CATEGORY_3GPP2_RUIM)
       {
         if(simlock_util_check_if_irm_matches(enabled_code_data_ptr->msin,
                                              lock_code_data_ptr->msin))
         {
           SIMLOCK_MSG_MED_0("match found for nw_type2 code");
           return TRUE;
         }
       }
       break;

     case SIMLOCK_CATEGORY_3GPP2_RUIM:
       if(enabled_category == SIMLOCK_CATEGORY_3GPP2_NW_TYPE1)
       {
         if(simlock_util_check_if_mcc_matches(enabled_code_data_ptr->mcc,
                                              lock_code_data_ptr->mcc)     &&
            simlock_util_check_if_mnc_matches(enabled_code_data_ptr->mnc,
                                              lock_code_data_ptr->mnc))
         {
           SIMLOCK_MSG_MED_0("match found for ruim code");
           return TRUE;
         }
       }
       if(enabled_category == SIMLOCK_CATEGORY_3GPP2_NW_TYPE2)
       {
         if(simlock_util_check_if_irm_matches(enabled_code_data_ptr->msin,
                                              lock_code_data_ptr->msin))
         {
           SIMLOCK_MSG_MED_0("match found for ruim code");
           return TRUE;
         }
       }
       break;

     default:
       break;
  }

  return FALSE;
} /* simlock_util_check_if_codes_match */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_IS_DEVICE_IN_EMERGENCY_MODE

DESCRIPTION
  This is a utility function that checks if the device is in the emergency
  only mode

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE  : If the device is in the emergency mode
  FALSE : Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_is_device_in_emergency_mode
(
  const simlock_config_data_type   * config_data_ptr
)
{
  if(config_data_ptr == NULL)
  {
    return TRUE;
  }

  if(config_data_ptr->device_mode != SIMLOCK_DEVICE_NORMAL_OPERATION_MODE)
  {
    return TRUE;
  }

  return FALSE;
} /* simlock_util_is_device_in_emergency_mode */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_FILE_ENUM_TO_FILE_TYPE

DESCRIPTION
  This is a utility function that converts a simlock data file to file type
  (config or category file), slot and category

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_util_file_enum_to_file_type
(
  simlock_data_file_enum_type    data_file_enum,
  simlock_file_type            * file_type_ptr
)
{
  if(file_type_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  switch(data_file_enum)
  {
    case SIMLOCK_FILE_CONFIG:
     file_type_ptr->file_type = SIMLOCK_FILE_TYPE_CONFIG;
     break;

    case SIMLOCK_FILE_3GPP_NW_1:
      file_type_ptr->file_type                   = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category = SIMLOCK_CATEGORY_3GPP_NW;
      file_type_ptr->data.category_data.slot     = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP_NS_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_NS;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP_SP_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP_CP_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_CP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP_SIM_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SIM;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP_SPN_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SPN;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP_ICCID_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_ICCID;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP2_NW_TYPE1_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP2_NW_TYPE2_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_NW_TYPE2;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP2_HRPD_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_HRPD;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP2_SP_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_SP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP2_CP_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_CP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP2_RUIM_1:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_RUIM;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_1;
      break;

    case SIMLOCK_FILE_3GPP_NW_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_NW;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP_NS_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_NS;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP_SP_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP_CP_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_CP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP_SIM_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SIM;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP_SPN_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SPN;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP_ICCID_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_ICCID;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP2_NW_TYPE1_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP2_NW_TYPE2_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_NW_TYPE2;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP2_HRPD_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_HRPD;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP2_SP_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_SP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP2_CP_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_CP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP2_RUIM_2:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_RUIM;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_2;
      break;

    case SIMLOCK_FILE_3GPP_NW_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_NW;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP_NS_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_NS;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP_SP_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP_CP_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_CP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP_SIM_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SIM;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP_SPN_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SPN;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP_ICCID_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_ICCID;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP2_NW_TYPE1_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP2_NW_TYPE2_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_NW_TYPE2;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP2_HRPD_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_HRPD;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP2_SP_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_SP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP2_CP_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_CP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP2_RUIM_3:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_RUIM;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_3;
      break;

    case SIMLOCK_FILE_3GPP_NW_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_NW;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP_NS_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_NS;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP_SP_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP_CP_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_CP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP_SIM_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SIM;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP_SPN_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_SPN;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP_ICCID_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP_ICCID;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP2_NW_TYPE1_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP2_NW_TYPE2_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_NW_TYPE2;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP2_HRPD_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_HRPD;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP2_SP_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_SP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP2_CP_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_CP;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    case SIMLOCK_FILE_3GPP2_RUIM_4:
      file_type_ptr->file_type                    = SIMLOCK_FILE_TYPE_CATEGORY;
      file_type_ptr->data.category_data.category  = SIMLOCK_CATEGORY_3GPP2_RUIM;
      file_type_ptr->data.category_data.slot      = SIMLOCK_SLOT_4;
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_util_file_enum_to_file_type */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_DEEP_COPY

DESCRIPTION
  The function allocates memory and does a deep copy. The caller of this
  function is responsible to de-allocate the memory

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void simlock_util_deep_copy
(
  void       ** dest_pptr,
  const void  * src_ptr,
  uint32        len
)
{
  if(dest_pptr != NULL)
  {
    *dest_pptr = NULL;
    if(len > 0 && src_ptr != NULL)
    {
      SIMLOCK_CHECK_AND_MEM_MALLOC(*dest_pptr, len);
      if(*dest_pptr != NULL)
      {
        simlock_memscpy(*dest_pptr, len, src_ptr, len);
      }
    }
  }
} /* simlock_util_deep_copy */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_GENERATE_REQUEST_TOKEN

DESCRIPTION
  This function generates a token to be used in a request.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_token_id_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_token_id_type simlock_util_generate_request_token
(
  void
)
{
  /* token used in request */
  static simlock_token_id_type   simlock_current_token = SIMLOCK_UTIL_TOKEN_ID_MIN;
  simlock_token_id_type          result                = simlock_current_token;

  simlock_current_token++;

  /* For the requests originating from modem,
     the token id range is 1 to 0x7FFF */
  if(simlock_current_token > SIMLOCK_UTIL_TOKEN_ID_MAX)
  {
    simlock_current_token = SIMLOCK_UTIL_TOKEN_ID_MIN;
  }

  return result;
} /* simlock_util_generate_request_token */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IS_TEST_MCC_MNC

DESCRIPTION
  This function checks if the MCC/MNC in the code data matches with the
  test MCC/MNCs

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  TRUE   : if the MCC/MNC in the code data matches one of the test MCC/MNCs
  FALSE  : if otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean simlock_util_check_is_test_mcc_mnc
(
  simlock_mcc_type         sim_mcc,
  simlock_mnc_type         sim_mnc
)
{
  uint8 table_index = 0;

  for(table_index = 0;
      table_index < (sizeof(simlock_test_mcc_mnc_tbl)/
                     sizeof(simlock_mcc_mnc_data_type));
      table_index++)
  {
    if(simlock_util_check_if_mcc_matches(simlock_test_mcc_mnc_tbl[table_index].mcc,
                                         sim_mcc)                           &&
       simlock_util_check_if_mnc_matches(simlock_test_mcc_mnc_tbl[table_index].mnc,
                                         sim_mnc))
    {
      return TRUE;
    }
  }

  return FALSE;
}/* simlock_util_check_is_test_mcc_mnc */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_CK_STATUS

DESCRIPTION
  This is a utility function that checks if a category is enabled with the
  CK or not

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_util_check_ck_status
(
  const simlock_config_data_type   * config_data_ptr,
  simlock_category_enum_type         category,
  simlock_slot_enum_type             slot,
  simlock_ck_present_type         *  ck_status_ptr
)
{
  simlock_result_enum_type    simlock_status       = SIMLOCK_SUCCESS;
  uint8                       slot_index           = 0;
  uint8                       category_index       = 0;

  if((config_data_ptr == NULL) ||
     (ck_status_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_status = simlock_util_get_slot_index(slot, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_util_get_category_index(category,
                                                   &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  *ck_status_ptr = config_data_ptr->category_status[slot_index][category_index].ck_present;

  return SIMLOCK_SUCCESS;
} /* simlock_util_check_ck_status */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_CHECK_IMEI_MATCHES

DESCRIPTION
  This function checks if the IMEIs provided matches

DEPENDENCIES

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If both the IMEIs match
  FALSE: If otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_check_imei_matches(
  const simlock_imei_type    device_imei,
  const simlock_imei_type    req_imei
)
{
  if(device_imei.imei_len != req_imei.imei_len)
  {
    return FALSE;
  }

  if(memcmp(device_imei.imei_data,
            req_imei.imei_data, device_imei.imei_len) != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* simlock_util_check_imei_matches */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_READ_ITEM_FROM_NV

DESCRIPTION
  This is a generic function that reads the NV item requested

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_util_read_item_from_nv(
  nv_items_enum_type        item,
  nv_item_type            * data_ptr,
  simlock_slot_enum_type    slot
)
{
  simlock_result_enum_type   simlock_status     = SIMLOCK_SUCCESS;
  nv_cmd_type               *simlock_nv_cmd_ptr = NULL;
  nv_cmd_ext_type            simlock_nv_cmd_ext = {{0}};
  uint8                      context            = 0;

  if(data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_status = simlock_util_get_slot_index(slot, &context);
  if(simlock_status != SIMLOCK_SUCCESS ||
     (context != 0 && context != 1))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(simlock_nv_cmd_ptr, sizeof(nv_cmd_type));

  if(simlock_nv_cmd_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* set the context */
  simlock_nv_cmd_ext.nvcmd = simlock_nv_cmd_ptr;
  simlock_nv_cmd_ext.context = context;

  /* Notify this task when done */
  simlock_nv_cmd_ptr->tcb_ptr = rex_self();
  simlock_nv_cmd_ptr->sigs = SIMLOCK_NV_READ_SIG;

  /* Command goes on no queue when done */
  simlock_nv_cmd_ptr->done_q_ptr = NULL;

  /* Item to read */
  simlock_nv_cmd_ptr->item = item;
  simlock_nv_cmd_ptr->cmd = NV_READ_F;

  /* Set the data ptr for NV Item to read */
   simlock_nv_cmd_ptr->data_ptr = data_ptr;

  /* Clear REX Signals */
  (void) rex_clr_sigs( rex_self(), SIMLOCK_NV_READ_SIG );

  /* Send Command to NV */
  nv_cmd_ext(&simlock_nv_cmd_ext);

  /* Wait for Signal from NV that it's complete */
  (void)simlock_wait(SIMLOCK_NV_READ_SIG);

  /* Clear Signal */
  (void) rex_clr_sigs( rex_self(), SIMLOCK_NV_READ_SIG );

  SIMLOCK_MSG_MED_2("Get NV Status = 0x%x for item: 0x%x",
                    simlock_nv_cmd_ptr->status, simlock_nv_cmd_ptr->item);

  if (simlock_nv_cmd_ptr->status != NV_DONE_S)
  {
    simlock_status = SIMLOCK_GENERIC_ERROR;
  }

  SIMLOCK_MEM_FREE(simlock_nv_cmd_ptr);

  return simlock_status;
} /* simlock_util_read_item_from_nv */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_GET_IMEI

DESCRIPTION
  This function reads the context specific IMEI from the NV and provides
  it in a ASCII format

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_util_get_imei(
  simlock_slot_enum_type    slot,
  simlock_imei_type       * imei_data_ptr
)
{
  uint8              nv_imei_len     = 0;
  uint8              nv_imei_index   = 0;
  uint8              imei_data_index = 0;
  uint8              imei_digit      = 0;
  nv_item_type    *  nv_data_buf_ptr = NULL;

  if(imei_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(nv_data_buf_ptr,
                               sizeof(nv_item_type));
  if (nv_data_buf_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* get the device IMEI */
  if(simlock_util_read_item_from_nv(NV_UE_IMEI_I,
                                    nv_data_buf_ptr,
                                    slot) != SIMLOCK_SUCCESS)
  {
    imei_data_ptr->imei_len = 0;
    SIMLOCK_MEM_FREE(nv_data_buf_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* the byte 0 indicates the length */
  /* the ue_imei array size is NV_UE_IMEI_SIZE,
     which is set to be 9 */
  nv_imei_len = nv_data_buf_ptr->ue_imei.ue_imei[0];
  if((nv_imei_len == 0) ||
     (nv_imei_len > (NV_UE_IMEI_SIZE - 1)))
  {
    SIMLOCK_MSG_ERR_1("invalid imei len, 0x%x", nv_imei_len);
    SIMLOCK_MEM_FREE(nv_data_buf_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* IMEI is 15 digits long and in NV it is coded in
     (NV_UE_IMEI_SIZE - 1) i.e. 8 bytes. The low nibble of
     byte 1 is the IMEI identifier, which is 0xA */
  /* the low nibble of byte 1 must be set to
     0xA, indicating that IMEI is valid */
  if((nv_data_buf_ptr->ue_imei.ue_imei[1] & 0x0F) != 0x0A)
  {
    SIMLOCK_MSG_ERR_0("NV imei has invalid format");
    SIMLOCK_MEM_FREE(nv_data_buf_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* parse device IMEI and convert to ASCII format*/
  for(nv_imei_index = 1; nv_imei_index <= nv_imei_len; nv_imei_index++)
  {
    imei_digit = nv_data_buf_ptr->ue_imei.ue_imei[nv_imei_index] & 0x0F;
    if((imei_digit <= 9) &&
       (imei_data_index < SIMLOCK_IMEI_MAX))
    {
      imei_data_ptr->imei_data[imei_data_index] = imei_digit + '0';
      imei_data_index++;
    }
    else
    {
      /* Ignore IMEI identifier 0xA, which is
         expected at index 1 */
      if(nv_imei_index != 1)
      {
        break;
      }
    }

    imei_digit = ((nv_data_buf_ptr->ue_imei.ue_imei[nv_imei_index] & 0xF0) >> 4);
    if((imei_digit <= 9) &&
       (imei_data_index < SIMLOCK_IMEI_MAX))
    {
      imei_data_ptr->imei_data[imei_data_index] = imei_digit + '0';
      imei_data_index++;
    }
    else
    {
      break;
    }
  }

  imei_data_ptr->imei_len = imei_data_index;
  SIMLOCK_MEM_FREE(nv_data_buf_ptr);

  return SIMLOCK_SUCCESS;
} /* simlock_util_get_imei */


/*===========================================================================
FUNCTION SIMLOCK_UTIL_ALL_CATEGORIES_DISABLED

DESCRIPTION
  This function returns TRUE in if all the categories are either disabled or
  are temporarily unlocked. FALSE otherwise.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_util_all_categories_disabled
(
  simlock_slot_enum_type                    slot,
  const simlock_config_data_type          * config_data_ptr
)
{
  simlock_category_enum_type        category                = SIMLOCK_CATEGORY_3GPP_NW;

  if(config_data_ptr == NULL)
  {
    return FALSE;
  }

  for(category = SIMLOCK_CATEGORY_3GPP_NW; category <= SIMLOCK_CATEGORY_3GPP2_RUIM;
      category++)
  {
    /* if the category is not enabled, continue */
    if(!simlock_util_check_category_enabled(config_data_ptr, category, slot))
    {
      continue;
    }

    /* if the category is temporarily unlocked, continue */
    if(simlock_timer_check_is_temporary_unlock_set(slot, category))
    {
      continue;
    }

    return FALSE;
  }

  return TRUE;
} /* simlock_util_all_categories_disabled */

#endif /* FEATURE_SIMLOCK */

