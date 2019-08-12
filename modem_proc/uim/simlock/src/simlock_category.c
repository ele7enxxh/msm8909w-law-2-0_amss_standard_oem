/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   C A T E G O R Y   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock category functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_category.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/08/16   bcho    Added support for ICCID simlock
06/22/16   bcho    Removal of KW warning
06/17/16   vdc     Added support for SPN based SIMLOCK feature
05/23/16   vdc     Remove F3 messages for memory allocation failure
07/01/15   bcho    Support for whitelist and blacklist codes in same category
05/04/15   vv      Support for more than 1 bytes of GID1/GID2
01/19/15   kr      Fixed compilation warnings
09/29/14   vv      Added support for the remote SFS
04/14/14   vv      Added support for identifying conflicting codes
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_util.h"
#include "simlock_file.h"
#include "simlock_category.h"
#include "simlock_modem.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define SIMLOCK_CATEGORY_FILE_HEADER_SIZE      64

#define SIMLOCK_CATEGORY_ADD_VERSION_INFO(category, version)                   \
                                           ((((uint8)version & 0x0F) << 4) |   \
                                            category)

#define SIMLOCK_CATEGORY_GET_VERSION(category)                                 \
              (simlock_category_file_version_enum_type)((category & 0xF0) >> 4)

#define SIMLOCK_CATEGORY_GET_CATEGORY(category)                                \
              (category & 0x0F)

/*---------------------------------------------------------------------------
  The number of lists needed to accomodate the lock codes.
  Each list can fit SIMLOCK_NUM_CODES_MAX (20) codes.
---------------------------------------------------------------------------*/
#define SIMLOCK_CATEGORY_GET_LIST_COUNT(x)  ((x > 0) ? ((x - 1) / SIMLOCK_NUM_CODES_MAX + 1) : 0)

/*===========================================================================

                        DATA TYPES

===========================================================================*/

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_CATEGORY_FILE_VERSION_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the version of the category file supported
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_CATEGORY_FILE_VERSION_0 = 0,
  SIMLOCK_CATEGORY_FILE_VERSION_1 = 1,
  SIMLOCK_CATEGORY_FILE_VERSION_2 = 2,
  SIMLOCK_CATEGORY_FILE_VERSION_3 = 3,
  SIMLOCK_CATEGORY_FILE_VERSION_5 = 5
} simlock_category_file_version_enum_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

typedef PACKED struct PACKED_POST
{
  uint8                      category_type_and_version;
  uint8                      auto_lock;
  uint8                      blacklist;
  uint32                     num_retries_max;
  uint32                     curr_retries;
  uint32                     iteration_cnt;
  uint8                      salt[SIMLOCK_SALT_LEN];
  uint8                      hck[SIMLOCK_HCK_LEN];
  uint8                      num_of_codes;
} simlock_header_data_type;

typedef PACKED struct PACKED_POST
{
  uint8                      code_type;
  uint8                      mcc[SIMLOCK_MCC_LEN];
  uint8                      mnc_length;
  uint8                      mnc[SIMLOCK_MNC_MAX];
  uint8                      iccid_length;
  uint8                      iccid[SIMLOCK_ICCID_DIGITS_MAX];
} simlock_code_data_v5_type;

typedef PACKED struct PACKED_POST
{
  uint8                      code_type;
  uint8                      mcc[SIMLOCK_MCC_LEN];
  uint8                      mnc_length;
  uint8                      mnc[SIMLOCK_MNC_MAX];
  uint8                      spn_data[SIMLOCK_SPN_LEN];
} simlock_code_data_v3_type;

typedef PACKED struct PACKED_POST
{
  uint8                      code_type;
  uint8                      mcc[SIMLOCK_MCC_LEN];
  uint8                      mnc_length;
  uint8                      mnc[SIMLOCK_MNC_MAX];
  uint8                      gid1_len;
  uint8                      gid1_data[SIMLOCK_MAX_GID_LEN];
  uint8                      gid2_len;
  uint8                      gid2_data[SIMLOCK_MAX_GID_LEN];
} simlock_code_data_v2_type;

typedef PACKED struct PACKED_POST
{
  uint8                      code_type;
  uint8                      mcc[SIMLOCK_MCC_LEN];
  uint8                      mnc_length;
  uint8                      mnc[SIMLOCK_MNC_MAX];
  uint8                      msin_length;
  uint8                      msin[SIMLOCK_MSIN_MAX];
  uint8                      gid1;
  uint8                      gid2;
} simlock_code_data_v1_type;

typedef PACKED struct PACKED_POST
{
  uint8                      mcc[SIMLOCK_MCC_LEN];
  uint8                      mnc_length;
  uint8                      mnc[SIMLOCK_MNC_MAX];
  uint8                      msin_length;
  uint8                      msin[SIMLOCK_MSIN_MAX];
  uint8                      gid1;
  uint8                      gid2;
} simlock_code_data_v0_type;

/* ----------------------------------------------------------------------------
  STRUCTURE:      SIMLOCK_CATEGORY_MAPPING

  DESCRIPTION:
    This structure maps simlock category to category index stored in SFS
-------------------------------------------------------------------------------*/
static PACKED struct PACKED_POST
{
  simlock_category_enum_type   category;
  uint8                        code;
} simlock_category_mapping[] =
{
  { SIMLOCK_CATEGORY_3GPP_NW,        0 },
  { SIMLOCK_CATEGORY_3GPP_NS,        1 },
  { SIMLOCK_CATEGORY_3GPP_SP,        2 },
  { SIMLOCK_CATEGORY_3GPP_CP,        3 },
  { SIMLOCK_CATEGORY_3GPP_SIM,       4 },
  { SIMLOCK_CATEGORY_3GPP2_NW_TYPE1, 5 },
  { SIMLOCK_CATEGORY_3GPP2_NW_TYPE2, 6 },
  { SIMLOCK_CATEGORY_3GPP2_HRPD,     7 },
  { SIMLOCK_CATEGORY_3GPP2_SP,       8 },
  { SIMLOCK_CATEGORY_3GPP2_CP,       9 },
  { SIMLOCK_CATEGORY_3GPP2_RUIM,    10 },
  { SIMLOCK_CATEGORY_3GPP_SPN,      11 },
  { SIMLOCK_CATEGORY_3GPP_ICCID,    13 }
};

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_FREE_CODE_DATA

DESCRIPTION
  This function frees the code data in the category file data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_category_free_code_data
(
  simlock_category_file_data_type  * category_data_ptr
)
{
  if(category_data_ptr != NULL)
  {
    if(category_data_ptr->code_data_ptr != NULL)
    {
      SIMLOCK_MEM_FREE(category_data_ptr->code_data_ptr);
    }
  }
} /* simlock_category_free_code_data */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_MAP_CATEGORY_TO_CATEGORY_INDEX

DESCRIPTION
  This function maps SIMLOCK category enum to category index stored in SFS

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_map_category_to_category_index
(
  simlock_category_enum_type category,
  uint8                     *code_ptr
)
{
  uint8 map_index = 0;

  if(code_ptr == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  for(map_index = 0; map_index < sizeof(simlock_category_mapping)/sizeof(simlock_category_mapping[0]); map_index++)
  {
    if(simlock_category_mapping[map_index].category == category)
    {
      *code_ptr = simlock_category_mapping[map_index].code;
      return SIMLOCK_SUCCESS;
    }
  }

  return SIMLOCK_GENERIC_ERROR;
} /* simlock_category_map_category_to_category_index */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_MAP_CATEGORY_INDEX_TO_CATEGORY

DESCRIPTION
  This function maps category index stored in SFS to SIMLOCK category enum

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_map_category_index_to_category
(
  uint8                       code,
  simlock_category_enum_type *category_ptr
)
{
  uint8 map_index = 0;

  if(category_ptr == NULL)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  for(map_index = 0; map_index < sizeof(simlock_category_mapping)/sizeof(simlock_category_mapping[0]); map_index++)
  {
    if(simlock_category_mapping[map_index].code == code)
    {
      *category_ptr = simlock_category_mapping[map_index].category;
      return SIMLOCK_SUCCESS;
    }
  }

  return SIMLOCK_GENERIC_ERROR;
} /* simlock_category_map_category_index_to_category */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_READ_LOCK_DATA

DESCRIPTION
  This function reads the lock data for a category and provides the data in
  a packed structure. It allocates memory based on the number of lock codes.
  The caller of this function is responsible to free that memory.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_read_lock_data
(
  simlock_category_enum_type         category,
  simlock_slot_enum_type             slot,
  simlock_category_file_data_type *  out_category_data_ptr
)
{
  simlock_data_file_enum_type                category_file_enum      = SIMLOCK_FILE_3GPP_NW_1;
  simlock_result_enum_type                   simlock_status          = SIMLOCK_SUCCESS;
  uint32                                     category_file_size      = 0;
  uint32                                     lock_data_size          = 0;
  uint16                                     offset                  = 0;
  uint8                                      num_of_codes            = 0;
  uint8                                      code_index              = 0;
  simlock_category_file_version_enum_type    file_version            = SIMLOCK_CATEGORY_FILE_VERSION_0;
  uint8                                   *  category_file_data_ptr  = NULL;
  simlock_sfs_enum_type                      sfs_type                = SIMLOCK_SFS_LOCAL;
  uint8                                      category_index          = 0;
  simlock_header_data_type                   header_data;
  simlock_code_data_v0_type                  code_data_v0;
  simlock_code_data_v1_type                  code_data_v1;
  simlock_code_data_v2_type                  code_data_v2;
  simlock_code_data_v3_type                  code_data_v3;
  simlock_code_data_v5_type                  code_data_v5;

  if(out_category_data_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(&header_data, 0x00, sizeof(simlock_header_data_type));
  memset(&code_data_v0, 0x00, sizeof(simlock_code_data_v0_type));
  memset(&code_data_v1, 0x00, sizeof(simlock_code_data_v1_type));
  memset(&code_data_v2, 0x00, sizeof(simlock_code_data_v2_type));
  memset(&code_data_v3, 0x00, sizeof(simlock_code_data_v3_type));
  memset(&code_data_v5, 0x00, sizeof(simlock_code_data_v5_type));

  /* compose the category file */
  category_file_enum = simlock_util_compose_category_file_enum(category, slot);
  if(category_file_enum == SIMLOCK_FILE_ENUM_INVALID)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* get the SFS being used for storing the data */
  sfs_type = simlock_config_read_sfs_type();

  /* read the entire file data. category_file_data_ptr holds the
     raw data */
  simlock_status = simlock_file_read(&category_file_data_ptr,
                                     &category_file_size,
                                     category_file_enum,
                                     sfs_type);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("read category file failed, status: 0x%x",
                       simlock_status);
    SIMLOCK_MEM_FREE(category_file_data_ptr);
    return simlock_status;
  }

  SIMLOCK_MSG_MED_1("category file size is, 0x%x",
                     category_file_size);

  /* if the file size is less than the minimum expected size,
     then it is not valid */
  if(category_file_size < SIMLOCK_CATEGORY_FILE_HEADER_SIZE)
  {
    SIMLOCK_MSG_ERR_1("category file size is less than minimum size, 0x%x",
                       category_file_size);
    SIMLOCK_MEM_FREE(category_file_data_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* copy the category header data (raw data) to packed struct */
  simlock_memscpy((uint8 *)&header_data,
                  sizeof(header_data),
                  category_file_data_ptr, sizeof(header_data));

  /* copy the header data from packed struct */
  file_version = SIMLOCK_CATEGORY_GET_VERSION(header_data.category_type_and_version);
  if((file_version != SIMLOCK_CATEGORY_FILE_VERSION_0) &&
     (file_version != SIMLOCK_CATEGORY_FILE_VERSION_1) &&
     (file_version != SIMLOCK_CATEGORY_FILE_VERSION_2) &&
     (file_version != SIMLOCK_CATEGORY_FILE_VERSION_3) &&
     (file_version != SIMLOCK_CATEGORY_FILE_VERSION_5))
  {
    SIMLOCK_MSG_ERR_1("Unsupported file version, 0x%x",
                      file_version);
    SIMLOCK_MEM_FREE(category_file_data_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  category_index = SIMLOCK_CATEGORY_GET_CATEGORY(header_data.category_type_and_version);

  simlock_status = simlock_category_map_category_index_to_category(
                     category_index,
                     &out_category_data_ptr->category_header.category_type);
  if(simlock_status != SIMLOCK_SUCCESS ||
     category != out_category_data_ptr->category_header.category_type)
  {
    SIMLOCK_MEM_FREE(category_file_data_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  if(header_data.auto_lock != 0)
  {
    out_category_data_ptr->category_header.auto_lock      = TRUE;
  }
  else
  {
    out_category_data_ptr->category_header.auto_lock      = FALSE;
  }
  if(header_data.blacklist != 0)
  {
    out_category_data_ptr->category_header.code_type      = SIMLOCK_CODE_BLACKLIST;
  }
  else
  {
    out_category_data_ptr->category_header.code_type      = SIMLOCK_CODE_WHITELIST;
  }
  out_category_data_ptr->category_header.num_retries_max  = header_data.num_retries_max;
  out_category_data_ptr->category_header.curr_retries     = header_data.curr_retries;
  out_category_data_ptr->category_header.iteration_cnt    = header_data.iteration_cnt;

  simlock_memscpy(out_category_data_ptr->category_header.salt,
                  SIMLOCK_SALT_LEN,
                  header_data.salt, SIMLOCK_SALT_LEN);

  simlock_memscpy(out_category_data_ptr->category_header.hck,
                  SIMLOCK_HCK_LEN,
                  header_data.hck, SIMLOCK_HCK_LEN);

  out_category_data_ptr->category_header.num_of_codes  = header_data.num_of_codes;

  num_of_codes = out_category_data_ptr->category_header.num_of_codes;

  lock_data_size = category_file_size - sizeof(header_data);

  /* if the file size is not expected size i.e.
     (header_size + (num_of_codes * code_size)), then it is not valid */
  /* check the code data length in the file
     The code data length should be either num codes times the size
     of version-0 of code data type or new code data type */
  if(((file_version == SIMLOCK_CATEGORY_FILE_VERSION_0) && (lock_data_size != (num_of_codes * sizeof(code_data_v0)))) ||
     ((file_version == SIMLOCK_CATEGORY_FILE_VERSION_1) && (lock_data_size != (num_of_codes * sizeof(code_data_v1)))) ||
     ((file_version == SIMLOCK_CATEGORY_FILE_VERSION_2) && (lock_data_size != (num_of_codes * sizeof(code_data_v2)))) ||
     ((file_version == SIMLOCK_CATEGORY_FILE_VERSION_3) && (lock_data_size != (num_of_codes * sizeof(code_data_v3)))) ||
     ((file_version == SIMLOCK_CATEGORY_FILE_VERSION_5) && (lock_data_size != (num_of_codes * sizeof(code_data_v5)))))
  {
    SIMLOCK_MSG_ERR_0("category file code data size is incorrect");
    SIMLOCK_MEM_FREE(category_file_data_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  if(num_of_codes > 0)
  {
    out_category_data_ptr->code_data_length = num_of_codes * sizeof(simlock_category_code_data_type);

    /* caller is responsible to FREE this memory */
    SIMLOCK_CHECK_AND_MEM_MALLOC(out_category_data_ptr->code_data_ptr,
                                 out_category_data_ptr->code_data_length);
    if(out_category_data_ptr->code_data_ptr == NULL)
    {
      SIMLOCK_MEM_FREE(category_file_data_ptr);
      return SIMLOCK_GENERIC_ERROR;
    }

    offset = sizeof(header_data);

    for(code_index = 0; code_index < num_of_codes; code_index++)
    {
      if(file_version == SIMLOCK_CATEGORY_FILE_VERSION_5)
      {
        if(category != SIMLOCK_CATEGORY_3GPP_ICCID)
        {
          SIMLOCK_MEM_FREE(category_file_data_ptr);
          return SIMLOCK_GENERIC_ERROR;
        }
        
        /* copy the category code data (raw data) to packed struct */
        simlock_memscpy((uint8 *)&code_data_v5,
                        sizeof(code_data_v5),
                        &category_file_data_ptr[offset],
                        sizeof(code_data_v5));

        if(code_data_v5.code_type != 0)
        {
          out_category_data_ptr->code_data_ptr[code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }
        else
        {
          out_category_data_ptr->code_data_ptr[code_index].code_type = SIMLOCK_CODE_WHITELIST;
        }

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].mcc,
                        SIMLOCK_MCC_LEN,
                        code_data_v5.mcc,
                        SIMLOCK_MCC_LEN);

        out_category_data_ptr->code_data_ptr[code_index].mnc.mnc_len = code_data_v5.mnc_length;

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        code_data_v5.mnc,
                        SIMLOCK_MNC_MAX);

        out_category_data_ptr->code_data_ptr[code_index].iccid.iccid_len = code_data_v5.iccid_length;

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].iccid.iccid_data,
                        SIMLOCK_ICCID_DIGITS_MAX,
                        code_data_v5.iccid,
                        SIMLOCK_ICCID_DIGITS_MAX);

        offset += sizeof(code_data_v5);
      }
      else if(file_version == SIMLOCK_CATEGORY_FILE_VERSION_3)
      {
        if(category != SIMLOCK_CATEGORY_3GPP_SPN)
        {
          SIMLOCK_MEM_FREE(category_file_data_ptr);
          return SIMLOCK_GENERIC_ERROR;
        }
        
        /* copy the category code data (raw data) to packed struct */
        simlock_memscpy((uint8 *)&code_data_v3,
                        sizeof(code_data_v3),
                        &category_file_data_ptr[offset],
                        sizeof(code_data_v3));

        if(code_data_v3.code_type != 0)
        {
          out_category_data_ptr->code_data_ptr[code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }
        else
        {
          out_category_data_ptr->code_data_ptr[code_index].code_type = SIMLOCK_CODE_WHITELIST;
        }

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].mcc,
                        SIMLOCK_MCC_LEN,
                        code_data_v3.mcc,
                        SIMLOCK_MCC_LEN);

        out_category_data_ptr->code_data_ptr[code_index].mnc.mnc_len = code_data_v3.mnc_length;

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        code_data_v3.mnc,
                        SIMLOCK_MNC_MAX);

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].spn.spn_data,
                        SIMLOCK_SPN_LEN,
                        code_data_v3.spn_data,
                        SIMLOCK_SPN_LEN);

        out_category_data_ptr->code_data_ptr[code_index].spn.spn_valid = TRUE;

        offset += sizeof(code_data_v3);
      }
      else if(file_version == SIMLOCK_CATEGORY_FILE_VERSION_2)
      {
        if(category != SIMLOCK_CATEGORY_3GPP_CP &&
           category != SIMLOCK_CATEGORY_3GPP_SP)
        {
          SIMLOCK_MEM_FREE(category_file_data_ptr);
          return SIMLOCK_GENERIC_ERROR;
        }
        
        /* copy the category code data (raw data) to packed struct */
        simlock_memscpy((uint8 *)&code_data_v2,
                        sizeof(code_data_v2),
                        &category_file_data_ptr[offset],
                        sizeof(code_data_v2));

        if(code_data_v2.code_type != 0)
        {
          out_category_data_ptr->code_data_ptr[code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }
        else
        {
          out_category_data_ptr->code_data_ptr[code_index].code_type = SIMLOCK_CODE_WHITELIST;
        }

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].mcc,
                        SIMLOCK_MCC_LEN,
                        code_data_v2.mcc,
                        SIMLOCK_MCC_LEN);

        out_category_data_ptr->code_data_ptr[code_index].mnc.mnc_len = code_data_v2.mnc_length;

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        code_data_v2.mnc,
                        SIMLOCK_MNC_MAX);

        out_category_data_ptr->code_data_ptr[code_index].gid1.gid_len = code_data_v2.gid1_len;

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].gid1.gid_data,
                        SIMLOCK_MAX_GID_LEN,
                        code_data_v2.gid1_data,
                        SIMLOCK_MAX_GID_LEN);

        out_category_data_ptr->code_data_ptr[code_index].gid2.gid_len = code_data_v2.gid2_len;

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].gid2.gid_data,
                        SIMLOCK_MAX_GID_LEN,
                        code_data_v2.gid2_data,
                        SIMLOCK_MAX_GID_LEN);

        offset += sizeof(code_data_v2);
      }
      else if(file_version == SIMLOCK_CATEGORY_FILE_VERSION_1)
      {
        /* copy the category code data (raw data) to packed struct */
        simlock_memscpy((uint8 *)&code_data_v1,
                        sizeof(code_data_v1),
                        &category_file_data_ptr[offset],
                        sizeof(code_data_v1));

        if(code_data_v1.code_type != 0)
        {
          out_category_data_ptr->code_data_ptr[code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }
        else
        {
          out_category_data_ptr->code_data_ptr[code_index].code_type = SIMLOCK_CODE_WHITELIST;
        }

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].mcc,
                        SIMLOCK_MCC_LEN,
                        code_data_v1.mcc,
                        SIMLOCK_MCC_LEN);

        out_category_data_ptr->code_data_ptr[code_index].mnc.mnc_len = code_data_v1.mnc_length;

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        code_data_v1.mnc,
                        SIMLOCK_MNC_MAX);

        out_category_data_ptr->code_data_ptr[code_index].msin.msin_len = code_data_v1.msin_length;

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].msin.msin_data,
                        SIMLOCK_MSIN_MAX,
                        code_data_v1.msin,
                        SIMLOCK_MSIN_MAX);

        out_category_data_ptr->code_data_ptr[code_index].gid1.gid_data[0] =
          code_data_v1.gid1;
        out_category_data_ptr->code_data_ptr[code_index].gid1.gid_len =
          SIMLOCK_MIN_GID_LEN;
        out_category_data_ptr->code_data_ptr[code_index].gid2.gid_data[0] =
          code_data_v1.gid2;
        out_category_data_ptr->code_data_ptr[code_index].gid2.gid_len =
          SIMLOCK_MIN_GID_LEN;

        offset += sizeof(code_data_v1);
      }
      else if (file_version == SIMLOCK_CATEGORY_FILE_VERSION_0)
      {
        /* copy the category code data (raw data) to packed struct */
        simlock_memscpy((uint8 *)&code_data_v0,
                        sizeof(code_data_v0),
                        &category_file_data_ptr[offset],
                        sizeof(code_data_v0));

        if(header_data.blacklist != 0)
        {
          out_category_data_ptr->code_data_ptr[code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }
        else
        {
          out_category_data_ptr->code_data_ptr[code_index].code_type = SIMLOCK_CODE_WHITELIST;
        }

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].mcc,
                        SIMLOCK_MCC_LEN,
                        code_data_v0.mcc,
                        SIMLOCK_MCC_LEN);

        out_category_data_ptr->code_data_ptr[code_index].mnc.mnc_len = code_data_v0.mnc_length;

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        code_data_v0.mnc,
                        SIMLOCK_MNC_MAX);

        out_category_data_ptr->code_data_ptr[code_index].msin.msin_len = code_data_v0.msin_length;

        simlock_memscpy(out_category_data_ptr->code_data_ptr[code_index].msin.msin_data,
                        SIMLOCK_MSIN_MAX,
                        code_data_v0.msin,
                        SIMLOCK_MSIN_MAX);

        out_category_data_ptr->code_data_ptr[code_index].gid1.gid_data[0] =
          code_data_v0.gid1;
        out_category_data_ptr->code_data_ptr[code_index].gid1.gid_len =
          SIMLOCK_MIN_GID_LEN;
        out_category_data_ptr->code_data_ptr[code_index].gid2.gid_data[0] =
          code_data_v0.gid2;
        out_category_data_ptr->code_data_ptr[code_index].gid2.gid_len =
          SIMLOCK_MIN_GID_LEN;

        offset += sizeof(code_data_v0);
      }
    }
  }

  SIMLOCK_MEM_FREE(category_file_data_ptr);
  return simlock_status;
} /* simlock_category_read_lock_data */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_WRITE_LOCK_DATA

DESCRIPTION
  This function writes the lock data for a category. It takes the lock data
  in a packed structure format and constructs a data buffer.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_write_lock_data
(
  simlock_slot_enum_type             slot,
  simlock_category_file_data_type  * category_data_ptr
)
{
  simlock_result_enum_type          simlock_status       = SIMLOCK_SUCCESS;
  simlock_data_file_enum_type       category_file_enum   = SIMLOCK_FILE_3GPP_NW_1;
  uint32                            category_data_length = 0;
  uint16                            buffer_index         = 0;
  uint8                             code_cnt             = 0;
  uint8                           * buffer_ptr           = NULL;
  uint8                             category_index       = 0;
  simlock_sfs_enum_type             sfs_type             = SIMLOCK_SFS_LOCAL;
  simlock_header_data_type          header_data;
  simlock_code_data_v1_type         code_data_v1;
  simlock_code_data_v2_type         code_data_v2;
  simlock_code_data_v3_type         code_data_v3;
  simlock_code_data_v5_type         code_data_v5;

  if(category_data_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(&header_data, 0x00, sizeof(simlock_header_data_type));

  SIMLOCK_MSG_MED_0("simlock_category_write_lock_data");

  /* calculate the total length of category data */
  if(category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_SPN)
  {
    category_data_length = (sizeof(simlock_header_data_type) +
                            (category_data_ptr->category_header.num_of_codes * sizeof(simlock_code_data_v3_type)));
  }
  else if(category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_CP ||
     category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_SP)
  {
    category_data_length = (sizeof(simlock_header_data_type) +
                            (category_data_ptr->category_header.num_of_codes * sizeof(simlock_code_data_v2_type)));
  }
  else  if(category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_ICCID)
  {
    category_data_length = (sizeof(simlock_header_data_type) +
                            (category_data_ptr->category_header.num_of_codes * sizeof(simlock_code_data_v5_type)));
  }
  else
  {
    category_data_length = (sizeof(simlock_header_data_type) +
                            (category_data_ptr->category_header.num_of_codes * sizeof(simlock_code_data_v1_type)));
  }

  /* sanity check of the length */
  if(category_data_length < SIMLOCK_CATEGORY_FILE_HEADER_SIZE)
  {
    SIMLOCK_MSG_ERR_1("category data length incorrect, 0x%x",
                       category_data_length);
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_category_map_category_to_category_index(
                      category_data_ptr->category_header.category_type,
                     &category_index);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(buffer_ptr, category_data_length);
  if(buffer_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* along with the category type, the version of the file
     is also saved For example, 0x10 indicates, version-1
     of the SIMLOCK_CATEGORY_3GPP_NW category file */
  if(category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_SPN)
  {
    header_data.category_type_and_version = SIMLOCK_CATEGORY_ADD_VERSION_INFO(category_index,
                                                                              SIMLOCK_CATEGORY_FILE_VERSION_3);
  }
  else if(category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_CP ||
     category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_SP)
  {
    header_data.category_type_and_version = SIMLOCK_CATEGORY_ADD_VERSION_INFO(category_index,
                                                                              SIMLOCK_CATEGORY_FILE_VERSION_2);
  }
  else if(category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_ICCID)
  {
    header_data.category_type_and_version = SIMLOCK_CATEGORY_ADD_VERSION_INFO(category_index,
                                                                              SIMLOCK_CATEGORY_FILE_VERSION_5);
  }
  else
  {
    header_data.category_type_and_version = SIMLOCK_CATEGORY_ADD_VERSION_INFO(category_index,
                                                                              SIMLOCK_CATEGORY_FILE_VERSION_1);
  }

  if(category_data_ptr->category_header.auto_lock)
  {
    header_data.auto_lock       = 0x01;
  }
  if(category_data_ptr->category_header.code_type == SIMLOCK_CODE_BLACKLIST)
  {
    header_data.blacklist       = 0x01;
  }
  header_data.num_retries_max   = category_data_ptr->category_header.num_retries_max;
  header_data.curr_retries      = category_data_ptr->category_header.curr_retries;
  header_data.iteration_cnt     = category_data_ptr->category_header.iteration_cnt;
  
  simlock_memscpy(header_data.salt,
                  SIMLOCK_SALT_LEN,
                  category_data_ptr->category_header.salt, SIMLOCK_SALT_LEN);

  simlock_memscpy(header_data.hck,
                  SIMLOCK_HCK_LEN,
                  category_data_ptr->category_header.hck, SIMLOCK_HCK_LEN);

  header_data.num_of_codes      = category_data_ptr->category_header.num_of_codes;
  
  /* copy the packed header data directly to the data buffer */
  /* in case of autolock only header data is present */
  if((buffer_index + sizeof(simlock_header_data_type)) <= 
           category_data_length)
  {
    buffer_ptr[buffer_index++] = header_data.category_type_and_version;
    buffer_ptr[buffer_index++] = header_data.auto_lock;
    buffer_ptr[buffer_index++] = header_data.blacklist;

    buffer_ptr[buffer_index++] = (uint8)(header_data.num_retries_max & 0x000000FF);
    buffer_ptr[buffer_index++] = (uint8)((header_data.num_retries_max & 0x0000FF00) >> 8);
    buffer_ptr[buffer_index++] = (uint8)((header_data.num_retries_max & 0x00FF0000) >> 16);
    buffer_ptr[buffer_index++] = (uint8)((header_data.num_retries_max & 0xFF000000) >> 24);

    buffer_ptr[buffer_index++] = (uint8)(header_data.curr_retries & 0x000000FF);
    buffer_ptr[buffer_index++] = (uint8)((header_data.curr_retries & 0x0000FF00) >> 8);
    buffer_ptr[buffer_index++] = (uint8)((header_data.curr_retries & 0x00FF0000) >> 16);
    buffer_ptr[buffer_index++] = (uint8)((header_data.curr_retries & 0xFF000000) >> 24);

    buffer_ptr[buffer_index++] = (uint8)(header_data.iteration_cnt & 0x000000FF);
    buffer_ptr[buffer_index++] = (uint8)((header_data.iteration_cnt & 0x0000FF00) >> 8);
    buffer_ptr[buffer_index++] = (uint8)((header_data.iteration_cnt & 0x00FF0000) >> 16);
    buffer_ptr[buffer_index++] = (uint8)((header_data.iteration_cnt & 0xFF000000) >> 24);

    simlock_memscpy(&buffer_ptr[buffer_index], 
                    (category_data_length - buffer_index),
                    header_data.salt, SIMLOCK_SALT_LEN);

    buffer_index += SIMLOCK_SALT_LEN;

    simlock_memscpy(&buffer_ptr[buffer_index],
                    (category_data_length - buffer_index),
                    header_data.hck, SIMLOCK_HCK_LEN);

    buffer_index += SIMLOCK_HCK_LEN;

    /* Last member in category header */
    buffer_ptr[buffer_index++] = header_data.num_of_codes;
  }

  /* sanity check the buffer_index. If it is not equal to header size,
     then it is an error */
  if(buffer_index != SIMLOCK_CATEGORY_FILE_HEADER_SIZE)
  {
    SIMLOCK_MSG_ERR_1("build lock data failed, unexpected index, 0x%x",
                       buffer_index);
    SIMLOCK_MEM_FREE(buffer_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  for(code_cnt = 0; code_cnt < category_data_ptr->category_header.num_of_codes;
      code_cnt++)
  {
    if(category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_SPN)
    {
      memset(&code_data_v3, 0x00, sizeof(simlock_code_data_v3_type));

      if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
      {
        code_data_v3.code_type = 0x01;
      }

      simlock_memscpy(code_data_v3.mcc,
                      SIMLOCK_MCC_LEN,
                      category_data_ptr->code_data_ptr[code_cnt].mcc,
                      SIMLOCK_MCC_LEN);

      code_data_v3.mnc_length = category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len;

      simlock_memscpy(code_data_v3.mnc,
                      SIMLOCK_MNC_MAX,
                      category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                      SIMLOCK_MNC_MAX);

      simlock_memscpy(code_data_v3.spn_data,
                      SIMLOCK_SPN_LEN,
                      category_data_ptr->code_data_ptr[code_cnt].spn.spn_data,
                      SIMLOCK_SPN_LEN);

      /* copy the entire code data to the data buffer */
      simlock_memscpy(&buffer_ptr[buffer_index],
                      (category_data_length - buffer_index),
                      (uint8 *)&code_data_v3,
                      sizeof(code_data_v3));

      buffer_index += sizeof(code_data_v3);
    }
    else if(category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_CP ||
       category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_SP)
    {
      memset(&code_data_v2, 0x00, sizeof(simlock_code_data_v2_type));

      if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
      {
        code_data_v2.code_type = 0x01;
      }

      simlock_memscpy(code_data_v2.mcc,
                      SIMLOCK_MCC_LEN,
                      category_data_ptr->code_data_ptr[code_cnt].mcc,
                      SIMLOCK_MCC_LEN);

      code_data_v2.mnc_length = category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len;

      simlock_memscpy(code_data_v2.mnc,
                      SIMLOCK_MNC_MAX,
                      category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                      SIMLOCK_MNC_MAX);

      code_data_v2.gid1_len = category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len;

      simlock_memscpy(code_data_v2.gid1_data,
                      SIMLOCK_MAX_GID_LEN,
                      category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
                      SIMLOCK_MAX_GID_LEN);

      code_data_v2.gid2_len = category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len;

      simlock_memscpy(code_data_v2.gid2_data,
                      SIMLOCK_MAX_GID_LEN,
                      category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
                      SIMLOCK_MAX_GID_LEN);

      /* copy the entire code data to the data buffer */
      simlock_memscpy(&buffer_ptr[buffer_index],
                      (category_data_length - buffer_index),
                      (uint8 *)&code_data_v2,
                      sizeof(code_data_v2));

      buffer_index += sizeof(code_data_v2);
    }
    else if(category_data_ptr->category_header.category_type == SIMLOCK_CATEGORY_3GPP_ICCID)
    {
      memset(&code_data_v5, 0x00, sizeof(simlock_code_data_v5_type));

      if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
      {
        code_data_v5.code_type = 0x01;
      }

      simlock_memscpy(code_data_v5.mcc,
                      SIMLOCK_MCC_LEN,
                      category_data_ptr->code_data_ptr[code_cnt].mcc,
                      SIMLOCK_MCC_LEN);

      code_data_v5.mnc_length = category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len;

      simlock_memscpy(code_data_v5.mnc,
                      SIMLOCK_MNC_MAX,
                      category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                      SIMLOCK_MNC_MAX);

      code_data_v5.iccid_length = category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len;

      simlock_memscpy(code_data_v5.iccid,
                      SIMLOCK_ICCID_DIGITS_MAX,
                      category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data,
                      SIMLOCK_ICCID_DIGITS_MAX);

      /* copy the entire code data to the data buffer */
      simlock_memscpy(&buffer_ptr[buffer_index],
                      (category_data_length - buffer_index),
                      (uint8 *)&code_data_v5,
                      sizeof(code_data_v5));

      buffer_index += sizeof(code_data_v5);
    }
    else
    {
      memset(&code_data_v1, 0x00, sizeof(simlock_code_data_v1_type));

      if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
      {
        code_data_v1.code_type = 0x01;
      }

    simlock_memscpy(code_data_v1.mcc,
                    SIMLOCK_MCC_LEN,
                    category_data_ptr->code_data_ptr[code_cnt].mcc,
                    SIMLOCK_MCC_LEN);

    code_data_v1.mnc_length = category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len;

    simlock_memscpy(code_data_v1.mnc,
                    SIMLOCK_MNC_MAX,
                    category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                    SIMLOCK_MNC_MAX);

    code_data_v1.msin_length = category_data_ptr->code_data_ptr[code_cnt].msin.msin_len;

    simlock_memscpy(code_data_v1.msin,
                    SIMLOCK_MSIN_MAX,
                    category_data_ptr->code_data_ptr[code_cnt].msin.msin_data,
                    SIMLOCK_MSIN_MAX);

      code_data_v1.gid1 = category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data[0];
      code_data_v1.gid2 = category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data[0];

    /* copy the entire code data to the data buffer */
    simlock_memscpy(&buffer_ptr[buffer_index],
                    (category_data_length - buffer_index),
                    (uint8 *)&code_data_v1,
                    sizeof(code_data_v1));

    buffer_index += sizeof(code_data_v1);
    }
  }

  if(buffer_index != category_data_length)
  {
    SIMLOCK_MSG_ERR_1("build lock data failed, unexpected index, 0x%x",
                       buffer_index);
    SIMLOCK_MEM_FREE(buffer_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  category_file_enum = simlock_util_compose_category_file_enum(category_data_ptr->category_header.category_type,
                                                               slot);
  if(category_file_enum == SIMLOCK_FILE_ENUM_INVALID)
  {
    SIMLOCK_MSG_ERR_0("failed to compose file enum");
    SIMLOCK_MEM_FREE(buffer_ptr);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* get the SFS being used for storing the data */
  sfs_type = simlock_config_read_sfs_type();

  /* write the lock data to the catefory file */
  simlock_status = simlock_file_write(buffer_ptr,
                                      category_data_length,
                                      category_file_enum,
                                      sfs_type);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("failed to write category file, status: 0x%x",
                       simlock_status);
  }

  SIMLOCK_MEM_FREE(buffer_ptr);
  return simlock_status;
} /* simlock_category_write_lock_data */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_BUILD_LOCK_DATA

DESCRIPTION
  This function builds the lock data for a category from the request. It
  allocates memory based on the number of lock codes. The caller of this
  function is responsible to free that memory.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_build_lock_data
(
  const simlock_set_lock_ck_msg_req_type   *  msg_type_ptr,
  const simlock_salt_type                     salt,
  const simlock_hck_type                      hck,
  simlock_category_file_data_type          *  category_data_ptr
)
{
  uint32 code_data_length = 0;
  uint8  code_cnt         = 0;

  if((msg_type_ptr == NULL)         ||
     (category_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* build the header data from the request */
  category_data_ptr->category_header.category_type =
    msg_type_ptr->category.category_type;
  category_data_ptr->category_header.auto_lock =
    msg_type_ptr->auto_lock;
  if(msg_type_ptr->blacklist)
  {
    category_data_ptr->category_header.code_type = SIMLOCK_CODE_BLACKLIST;
  }
  category_data_ptr->category_header.num_retries_max =
    msg_type_ptr->num_retries_max;
  category_data_ptr->category_header.iteration_cnt =
    msg_type_ptr->iteration_cnt;
  category_data_ptr->category_header.curr_retries = 0;

  simlock_memscpy(category_data_ptr->category_header.salt,
                  SIMLOCK_SALT_LEN,
                  salt, SIMLOCK_SALT_LEN);

  simlock_memscpy(category_data_ptr->category_header.hck,
                  SIMLOCK_HCK_LEN,
                  hck, SIMLOCK_HCK_LEN);

  /* build the code data from the request */
  switch(msg_type_ptr->category.category_type)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      category_data_ptr->category_header.num_of_codes =
        msg_type_ptr->category.code_data.nw_3gpp_code_list.num_nw_codes;

      /* in case of autolock num of codes can be 0 */
      if(msg_type_ptr->category.code_data.nw_3gpp_code_list.num_nw_codes != 0)
      {
        code_data_length = (msg_type_ptr->category.code_data.nw_3gpp_code_list.num_nw_codes *
                            sizeof(simlock_category_code_data_type));

        /* caller is responsible to free the memory */
        SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr->code_data_ptr,
                                     code_data_length);
        if(category_data_ptr->code_data_ptr == NULL)
        {
          return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
      category_data_ptr->code_data_length = code_data_length;

      for(code_cnt = 0; code_cnt < msg_type_ptr->category.code_data.nw_3gpp_code_list.num_nw_codes;
          code_cnt++)
      {
        SIMLOCK_MSG_LOW_6("From req - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                          msg_type_ptr->category.code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mcc[0],
                          msg_type_ptr->category.code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mcc[1],
                          msg_type_ptr->category.code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mcc[2],
                          msg_type_ptr->category.code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mnc.mnc_data[0],
                          msg_type_ptr->category.code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mnc.mnc_data[1],
                          msg_type_ptr->category.code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mnc.mnc_data[2]);

        if(msg_type_ptr->blacklist)
        {
          category_data_ptr->code_data_ptr[code_cnt].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mcc,
                        SIMLOCK_MCC_LEN,
                        msg_type_ptr->category.code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mcc,
                        SIMLOCK_MCC_LEN);

        category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len =
               msg_type_ptr->category.code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mnc.mnc_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        msg_type_ptr->category.code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mnc.mnc_data,
                        msg_type_ptr->category.code_data.nw_3gpp_code_list.nw_code_data[code_cnt].mnc.mnc_len);

        category_data_ptr->code_data_ptr[code_cnt].msin.msin_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        memset(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len = 0;
        category_data_ptr->code_data_ptr[code_cnt].spn.spn_valid = FALSE;
        memset(category_data_ptr->code_data_ptr[code_cnt].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_NS:
      category_data_ptr->category_header.num_of_codes =
        msg_type_ptr->category.code_data.ns_3gpp_code_list.num_ns_codes;

      /* in case of autolock num of codes can be 0 */
      if(msg_type_ptr->category.code_data.ns_3gpp_code_list.num_ns_codes != 0)
      {
        code_data_length = (msg_type_ptr->category.code_data.ns_3gpp_code_list.num_ns_codes *
                            sizeof(simlock_category_code_data_type));

        /* caller is responsible to free the memory */
        SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr->code_data_ptr,
                                     code_data_length);
        if(category_data_ptr->code_data_ptr == NULL)
        {
          return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
      category_data_ptr->code_data_length = code_data_length;

      for(code_cnt = 0; code_cnt < msg_type_ptr->category.code_data.ns_3gpp_code_list.num_ns_codes;
          code_cnt++)
      {
        SIMLOCK_MSG_LOW_6("From req - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                          msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mcc[0],
                          msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mcc[1],
                          msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mcc[2],
                          msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mnc.mnc_data[0],
                          msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mnc.mnc_data[1],
                          msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mnc.mnc_data[2]);

        if(msg_type_ptr->blacklist)
        {
          category_data_ptr->code_data_ptr[code_cnt].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mcc,
                        SIMLOCK_MCC_LEN,
                        msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mcc,
                        SIMLOCK_MCC_LEN);

        category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len =
             msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mnc.mnc_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mnc.mnc_data,
                        msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code.mnc.mnc_len);

        SIMLOCK_MSG_LOW_2("From req - d6: 0x%x, d7: 0x%x",
                     msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].imsi_digit6,
                     msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].imsi_digit7);

        /* copy d6 and d7 to msin_data[0] and msin_data[1] respectively */
        category_data_ptr->code_data_ptr[code_cnt].msin.msin_data[0] =
          msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].imsi_digit6;

        category_data_ptr->code_data_ptr[code_cnt].msin.msin_data[1] =
          msg_type_ptr->category.code_data.ns_3gpp_code_list.ns_code_data[code_cnt].imsi_digit7;

        category_data_ptr->code_data_ptr[code_cnt].msin.msin_len = 0x02;

        memset(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len = 0;
        category_data_ptr->code_data_ptr[code_cnt].spn.spn_valid = FALSE;
        memset(category_data_ptr->code_data_ptr[code_cnt].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SP:
      category_data_ptr->category_header.num_of_codes =
        msg_type_ptr->category.code_data.sp_3gpp_code_list.num_sp_codes;

      /* in case of autolock num of codes can be 0 */
      if(msg_type_ptr->category.code_data.sp_3gpp_code_list.num_sp_codes != 0)
      {
        code_data_length = (msg_type_ptr->category.code_data.sp_3gpp_code_list.num_sp_codes *
                            sizeof(simlock_category_code_data_type));

        /* caller is responsible to free the memory */
        SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr->code_data_ptr,
                                     code_data_length);
        if(category_data_ptr->code_data_ptr == NULL)
        {
          return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
      category_data_ptr->code_data_length = code_data_length;

      for(code_cnt = 0; code_cnt < msg_type_ptr->category.code_data.sp_3gpp_code_list.num_sp_codes;
          code_cnt++)
      {
        if(msg_type_ptr->blacklist)
        {
          category_data_ptr->code_data_ptr[code_cnt].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mcc,
                        SIMLOCK_MCC_LEN,
                        msg_type_ptr->category.code_data.sp_3gpp_code_list.sp_code_data[code_cnt].nw_code.mcc,
                        SIMLOCK_MCC_LEN);

        category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len =
             msg_type_ptr->category.code_data.sp_3gpp_code_list.sp_code_data[code_cnt].nw_code.mnc.mnc_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        msg_type_ptr->category.code_data.sp_3gpp_code_list.sp_code_data[code_cnt].nw_code.mnc.mnc_data,
                        msg_type_ptr->category.code_data.sp_3gpp_code_list.sp_code_data[code_cnt].nw_code.mnc.mnc_len);

        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len =
                msg_type_ptr->category.code_data.sp_3gpp_code_list.sp_code_data[code_cnt].gid1.gid_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
                        SIMLOCK_MAX_GID_LEN,
                        msg_type_ptr->category.code_data.sp_3gpp_code_list.sp_code_data[code_cnt].gid1.gid_data,
                        msg_type_ptr->category.code_data.sp_3gpp_code_list.sp_code_data[code_cnt].gid1.gid_len);

        category_data_ptr->code_data_ptr[code_cnt].msin.msin_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        memset(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len = 0;
        category_data_ptr->code_data_ptr[code_cnt].spn.spn_valid = FALSE;
        memset(category_data_ptr->code_data_ptr[code_cnt].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_CP:
      category_data_ptr->category_header.num_of_codes =
        msg_type_ptr->category.code_data.cp_3gpp_code_list.num_cp_codes;

      /* in case of autolock, num of codes can be 0 */
      if(msg_type_ptr->category.code_data.cp_3gpp_code_list.num_cp_codes != 0)
      {
        code_data_length = (msg_type_ptr->category.code_data.cp_3gpp_code_list.num_cp_codes *
                            sizeof(simlock_category_code_data_type));

        /* caller is responsible to free the memory */
        SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr->code_data_ptr,
                                     code_data_length);
        if(category_data_ptr->code_data_ptr == NULL)
        {
          return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
      category_data_ptr->code_data_length = code_data_length;

      for(code_cnt = 0; code_cnt < msg_type_ptr->category.code_data.cp_3gpp_code_list.num_cp_codes;
          code_cnt++)
      {
        if(msg_type_ptr->blacklist)
        {
          category_data_ptr->code_data_ptr[code_cnt].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mcc,
                        SIMLOCK_MCC_LEN,
                        msg_type_ptr->category.code_data.cp_3gpp_code_list.cp_code_data[code_cnt].nw_code.mcc,
                        SIMLOCK_MCC_LEN);

        category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len =
             msg_type_ptr->category.code_data.cp_3gpp_code_list.cp_code_data[code_cnt].nw_code.mnc.mnc_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        msg_type_ptr->category.code_data.cp_3gpp_code_list.cp_code_data[code_cnt].nw_code.mnc.mnc_data,
                        msg_type_ptr->category.code_data.cp_3gpp_code_list.cp_code_data[code_cnt].nw_code.mnc.mnc_len);

        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len =
                msg_type_ptr->category.code_data.cp_3gpp_code_list.cp_code_data[code_cnt].gid1.gid_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
                        SIMLOCK_MAX_GID_LEN,
                        msg_type_ptr->category.code_data.cp_3gpp_code_list.cp_code_data[code_cnt].gid1.gid_data,
                        msg_type_ptr->category.code_data.cp_3gpp_code_list.cp_code_data[code_cnt].gid1.gid_len);

        category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len =
             msg_type_ptr->category.code_data.cp_3gpp_code_list.cp_code_data[code_cnt].gid2.gid_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
                        SIMLOCK_MAX_GID_LEN,
                        msg_type_ptr->category.code_data.cp_3gpp_code_list.cp_code_data[code_cnt].gid2.gid_data,
                        msg_type_ptr->category.code_data.cp_3gpp_code_list.cp_code_data[code_cnt].gid2.gid_len);

        category_data_ptr->code_data_ptr[code_cnt].msin.msin_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        category_data_ptr->code_data_ptr[code_cnt].spn.spn_valid = FALSE;
        memset(category_data_ptr->code_data_ptr[code_cnt].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SIM:
      category_data_ptr->category_header.num_of_codes =
        msg_type_ptr->category.code_data.sim_3gpp_code_list.num_sim_codes;

      /* in case of autolock, num of codes can be 0 */
      if(msg_type_ptr->category.code_data.sim_3gpp_code_list.num_sim_codes != 0)
      {
        code_data_length = (msg_type_ptr->category.code_data.sim_3gpp_code_list.num_sim_codes *
                            sizeof(simlock_category_code_data_type));

        /* caller is responsible to free the memory */
        SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr->code_data_ptr,
                                     code_data_length);
        if(category_data_ptr->code_data_ptr == NULL)
        {
          return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
      category_data_ptr->code_data_length = code_data_length;

      for(code_cnt = 0; code_cnt < msg_type_ptr->category.code_data.sim_3gpp_code_list.num_sim_codes;
          code_cnt++)
      {
        if(msg_type_ptr->blacklist)
        {
          category_data_ptr->code_data_ptr[code_cnt].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mcc,
                        SIMLOCK_MCC_LEN,
                        msg_type_ptr->category.code_data.sim_3gpp_code_list.sim_code_data[code_cnt].nw_code.mcc,
                        SIMLOCK_MCC_LEN);

        category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len =
             msg_type_ptr->category.code_data.sim_3gpp_code_list.sim_code_data[code_cnt].nw_code.mnc.mnc_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        msg_type_ptr->category.code_data.sim_3gpp_code_list.sim_code_data[code_cnt].nw_code.mnc.mnc_data,
                        msg_type_ptr->category.code_data.sim_3gpp_code_list.sim_code_data[code_cnt].nw_code.mnc.mnc_len);

        category_data_ptr->code_data_ptr[code_cnt].msin.msin_len =
             msg_type_ptr->category.code_data.sim_3gpp_code_list.sim_code_data[code_cnt].imsi_msin.msin_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].msin.msin_data,
                        SIMLOCK_MSIN_MAX,
                        msg_type_ptr->category.code_data.sim_3gpp_code_list.sim_code_data[code_cnt].imsi_msin.msin_data,
                        msg_type_ptr->category.code_data.sim_3gpp_code_list.sim_code_data[code_cnt].imsi_msin.msin_len);
        memset(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len = 0;
        category_data_ptr->code_data_ptr[code_cnt].spn.spn_valid = FALSE;
        memset(category_data_ptr->code_data_ptr[code_cnt].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SPN:
      category_data_ptr->category_header.num_of_codes =
        msg_type_ptr->category.code_data.spn_3gpp_code_list.num_spn_codes;

      /* in case of autolock, num of codes can be 0 */
      if(msg_type_ptr->category.code_data.spn_3gpp_code_list.num_spn_codes != 0)
      {
        code_data_length = (msg_type_ptr->category.code_data.spn_3gpp_code_list.num_spn_codes *
                            sizeof(simlock_category_code_data_type));

        /* caller is responsible to free the memory */
        SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr->code_data_ptr,
                                     code_data_length);
        if(category_data_ptr->code_data_ptr == NULL)
        {
          return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
      category_data_ptr->code_data_length = code_data_length;

      for(code_cnt = 0; code_cnt < msg_type_ptr->category.code_data.spn_3gpp_code_list.num_spn_codes;
          code_cnt++)
      {
        if(msg_type_ptr->blacklist)
        {
          category_data_ptr->code_data_ptr[code_cnt].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mcc,
                        SIMLOCK_MCC_LEN,
                        msg_type_ptr->category.code_data.spn_3gpp_code_list.spn_code_data[code_cnt].nw_code.mcc,
                        SIMLOCK_MCC_LEN);

        category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len =
             msg_type_ptr->category.code_data.spn_3gpp_code_list.spn_code_data[code_cnt].nw_code.mnc.mnc_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        msg_type_ptr->category.code_data.spn_3gpp_code_list.spn_code_data[code_cnt].nw_code.mnc.mnc_data,
                        msg_type_ptr->category.code_data.spn_3gpp_code_list.spn_code_data[code_cnt].nw_code.mnc.mnc_len);

        simlock_memscpy(&category_data_ptr->code_data_ptr[code_cnt].spn,
                        sizeof(category_data_ptr->code_data_ptr[code_cnt].spn),
                        &msg_type_ptr->category.code_data.spn_3gpp_code_list.spn_code_data[code_cnt].spn,
                        sizeof(msg_type_ptr->category.code_data.spn_3gpp_code_list.spn_code_data[code_cnt].spn));

        memset(category_data_ptr->code_data_ptr[code_cnt].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        category_data_ptr->code_data_ptr[code_cnt].msin.msin_len = 0;

        memset(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len = 0;
        category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_ICCID:
      category_data_ptr->category_header.num_of_codes =
        msg_type_ptr->category.code_data.iccid_3gpp_code_list.num_iccid_codes;

      /* in case of autolock, num of codes can be 0 */
      if(msg_type_ptr->category.code_data.iccid_3gpp_code_list.num_iccid_codes != 0)
      {
        code_data_length = (msg_type_ptr->category.code_data.iccid_3gpp_code_list.num_iccid_codes *
                            sizeof(simlock_category_code_data_type));

        /* caller is responsible to free the memory */
        SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr->code_data_ptr,
                                     code_data_length);
        if(category_data_ptr->code_data_ptr == NULL)
        {
          return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
      category_data_ptr->code_data_length = code_data_length;

      for(code_cnt = 0; code_cnt < msg_type_ptr->category.code_data.iccid_3gpp_code_list.num_iccid_codes;
          code_cnt++)
      {
        if(msg_type_ptr->blacklist)
        {
          category_data_ptr->code_data_ptr[code_cnt].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mcc,
                        SIMLOCK_MCC_LEN,
                        msg_type_ptr->category.code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].nw_code.mcc,
                        SIMLOCK_MCC_LEN);

        category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len =
             msg_type_ptr->category.code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].nw_code.mnc.mnc_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        msg_type_ptr->category.code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].nw_code.mnc.mnc_data,
                        msg_type_ptr->category.code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].nw_code.mnc.mnc_len);

        category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len =
             msg_type_ptr->category.code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].iccid.iccid_len;

        simlock_memscpy(&category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data,
                        SIMLOCK_ICCID_DIGITS_MAX,
                        msg_type_ptr->category.code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].iccid.iccid_data,
                        msg_type_ptr->category.code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].iccid.iccid_len);

        memset(category_data_ptr->code_data_ptr[code_cnt].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        category_data_ptr->code_data_ptr[code_cnt].msin.msin_len = 0;

        memset(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT,
               sizeof(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data));
        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT,
               sizeof(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data));
        category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len = 0;
        category_data_ptr->code_data_ptr[code_cnt].spn.spn_valid = FALSE;
        memset(category_data_ptr->code_data_ptr[code_cnt].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
      category_data_ptr->category_header.num_of_codes =
        msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.num_nw_codes;

      /* in case of autolock, num of codes can be 0 */
      if(msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.num_nw_codes != 0)
      {
        code_data_length = (msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.num_nw_codes *
                            sizeof(simlock_category_code_data_type));

        /* caller is responsible to free the memory */
        SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr->code_data_ptr,
                                     code_data_length);
        if(category_data_ptr->code_data_ptr == NULL)
        {
          return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
      category_data_ptr->code_data_length = code_data_length;

      for(code_cnt = 0;
          code_cnt < msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.num_nw_codes;
          code_cnt++)
      {
        SIMLOCK_MSG_LOW_6("From req - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                          msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mcc[0],
                          msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mcc[1],
                          msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mcc[2],
                          msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mnc.mnc_data[0],
                          msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mnc.mnc_data[1],
                          msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mnc.mnc_data[2]);

        if(msg_type_ptr->blacklist)
        {
          category_data_ptr->code_data_ptr[code_cnt].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mcc,
                        SIMLOCK_MCC_LEN,
                        msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mcc,
                        SIMLOCK_MCC_LEN);

        category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len =
             msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mnc.mnc_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mnc.mnc_data,
                        msg_type_ptr->category.code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt].mnc.mnc_len);

        category_data_ptr->code_data_ptr[code_cnt].msin.msin_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        memset(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len = 0;
        category_data_ptr->code_data_ptr[code_cnt].spn.spn_valid = FALSE;
        memset(category_data_ptr->code_data_ptr[code_cnt].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
      category_data_ptr->category_header.num_of_codes =
        msg_type_ptr->category.code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes;

      /* in case of autolock, num of codes can be 0 */
      if(msg_type_ptr->category.code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes != 0)
      {
        code_data_length = (msg_type_ptr->category.code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes *
                            sizeof(simlock_category_code_data_type));

        /* caller is responsible to free the memory */
        SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr->code_data_ptr,
                                     code_data_length);
        if(category_data_ptr->code_data_ptr == NULL)
        {
          return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
      category_data_ptr->code_data_length = code_data_length;

      for(code_cnt = 0;
          code_cnt < msg_type_ptr->category.code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes;
          code_cnt++)
      {
        if(msg_type_ptr->blacklist)
        {
          category_data_ptr->code_data_ptr[code_cnt].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        category_data_ptr->code_data_ptr[code_cnt].msin.msin_len =
             SIMLOCK_IRM_LEN;

        SIMLOCK_MSG_LOW_4("From req - irm[0]: 0x%x, irm[1]: 0x%x, irm[2]: 0x%x, irm[3]: 0x%x",
                     msg_type_ptr->category.code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[code_cnt].irm[0],
                     msg_type_ptr->category.code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[code_cnt].irm[1],
                     msg_type_ptr->category.code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[code_cnt].irm[2],
                     msg_type_ptr->category.code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[code_cnt].irm[3]);

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].msin.msin_data,
                        SIMLOCK_MSIN_MAX,
                        msg_type_ptr->category.code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[code_cnt].irm,
                        SIMLOCK_IRM_LEN);

        memset(category_data_ptr->code_data_ptr[code_cnt].mcc, 0x7F, SIMLOCK_MCC_LEN);
        memset(category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data, 0x7F, SIMLOCK_MNC_MAX);

        category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len = 0;

        memset(&category_data_ptr->code_data_ptr[code_cnt].msin.msin_data[SIMLOCK_IRM_LEN], 0x7F,
                SIMLOCK_MSIN_MAX - SIMLOCK_IRM_LEN);

        memset(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len = 0;
        category_data_ptr->code_data_ptr[code_cnt].spn.spn_valid = FALSE;
        memset(category_data_ptr->code_data_ptr[code_cnt].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_RUIM:
      category_data_ptr->category_header.num_of_codes =
        (uint8)msg_type_ptr->category.code_data.ruim_3gpp2_code_list.num_sim_codes;

      /* in case of autolock num of codes can be 0 */
      if(msg_type_ptr->category.code_data.ruim_3gpp2_code_list.num_sim_codes != 0)
      {
        code_data_length = (msg_type_ptr->category.code_data.ruim_3gpp2_code_list.num_sim_codes *
                            sizeof(simlock_category_code_data_type));

        /* caller is responsible to free the memory */
        SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr->code_data_ptr,
                                     code_data_length);
        if(category_data_ptr->code_data_ptr == NULL)
        {
          return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
      category_data_ptr->code_data_length = code_data_length;

      for(code_cnt = 0;
          code_cnt < msg_type_ptr->category.code_data.ruim_3gpp2_code_list.num_sim_codes;
          code_cnt++)
      {
        SIMLOCK_MSG_LOW_6("From req - mcc[0]: 0x%x, mcc[1]: 0x%x, mcc[2]: 0x%x, mnc[0]: 0x%x, mnc[1]: 0x%x, mnc[2]: 0x%x",
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mcc[0],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mcc[1],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mcc[2],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mnc.mnc_data[0],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mnc.mnc_data[1],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mnc.mnc_data[2]);

        SIMLOCK_MSG_LOW_6("From req - msin[0]: 0x%x, msin[1]: 0x%x, msin[2]: 0x%x, msin[3]: 0x%x, msin[4]: 0x%x, msin[5]: 0x%x",
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data[0],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data[1],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data[2],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data[3],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data[4],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data[5]);

        SIMLOCK_MSG_LOW_4("From req - msin[6]: 0x%x, msin[7]: 0x%x, msin[8]: 0x%x, msin[9]: 0x%x",
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data[6],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data[7],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data[8],
                          msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data[9]);

        if(msg_type_ptr->blacklist)
        {
          category_data_ptr->code_data_ptr[code_cnt].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mcc,
                        SIMLOCK_MCC_LEN,
                        msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mcc,
                        SIMLOCK_MCC_LEN);

        category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_len =
             msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mnc.mnc_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].mnc.mnc_data,
                        SIMLOCK_MNC_MAX,
                        msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mnc.mnc_data,
                        msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code.mnc.mnc_len);

        category_data_ptr->code_data_ptr[code_cnt].msin.msin_len =
             msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_len;

        simlock_memscpy(category_data_ptr->code_data_ptr[code_cnt].msin.msin_data,
                        SIMLOCK_MSIN_MAX,
                        msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_data,
                        msg_type_ptr->category.code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin.msin_len);

        memset(category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len = 0;
        category_data_ptr->code_data_ptr[code_cnt].spn.spn_valid = FALSE;
        memset(category_data_ptr->code_data_ptr[code_cnt].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len = 0;
        memset(category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }
  return SIMLOCK_SUCCESS;
} /* simlock_category_build_lock_data*/


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_IS_WHITELIST

DESCRIPTION
  This function checks if the lock codes for a category are considered as
  whitelist or blacklist

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE  :  If the codes are whitelist
  FALSE :  If otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_category_is_whitelist
(
  simlock_category_header_data_type  * header_data_ptr
)
{
  if(header_data_ptr == NULL)
  {
    return FALSE;
  }

  return (header_data_ptr->code_type == SIMLOCK_CODE_WHITELIST);
} /* simlock_category_is_whitelist */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_UPDATE_AND_WRITE_LOCK_DATA

DESCRIPTION
  If the auto_lock feature is enabled for a category during the lock 
  activation, the codes are updated later. This function updates the lock data
  for such categories.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_update_and_write_lock_data
(
  simlock_category_header_data_type  * header_data_ptr,
  simlock_category_code_data_type    * code_data_ptr,
  simlock_slot_enum_type               slot
)
{
  simlock_result_enum_type          simlock_status    = SIMLOCK_SUCCESS;
  simlock_category_file_data_type * category_data_ptr = NULL;

  if((header_data_ptr == NULL) ||
     (code_data_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr,
                               sizeof(simlock_category_file_data_type));
  if(category_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* copy the header data from the request */
  simlock_memscpy(&category_data_ptr->category_header,
                  sizeof(simlock_category_header_data_type),
                  header_data_ptr,
                  sizeof(simlock_category_header_data_type));

  /* update the autolock indicator */
  category_data_ptr->category_header.auto_lock = 0;

  /* update the number of codes */
  category_data_ptr->category_header.num_of_codes = 1;

  category_data_ptr->code_data_length = sizeof(simlock_category_code_data_type);
  category_data_ptr->code_data_ptr = code_data_ptr;

  /* update the lock data in category file */
  simlock_status = simlock_category_write_lock_data(slot, category_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_ERR_1("Failed to write lock data, status: 0x%x",
                       simlock_status);
  }

  category_data_ptr->code_data_ptr = NULL;
  SIMLOCK_MEM_FREE(category_data_ptr);
  return simlock_status;
} /* simlock_category_update_and_write_lock_data */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_COPY_NW_CODE_TO_RESPONSE

DESCRIPTION
  This function copies the NW code to the response

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_copy_nw_code_to_response
(
  simlock_nw_code_data_type                  * resp_nw_code_ptr,
  const simlock_category_code_data_type      * code_data_ptr
)
{
  if((code_data_ptr == NULL) ||
     (resp_nw_code_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_memscpy(resp_nw_code_ptr->mcc,
                  SIMLOCK_MCC_LEN,
                  code_data_ptr->mcc,
                  SIMLOCK_MCC_LEN);

  if (code_data_ptr->mnc.mnc_len > SIMLOCK_MNC_MAX)
  {
    SIMLOCK_MSG_ERR_1("MNC data length too long. length: 0x%x",
                      code_data_ptr->mnc.mnc_len);
    return SIMLOCK_GENERIC_ERROR;
  }

  resp_nw_code_ptr->mnc.mnc_len = code_data_ptr->mnc.mnc_len;

  simlock_memscpy(resp_nw_code_ptr->mnc.mnc_data,
                  SIMLOCK_MNC_MAX,
                  code_data_ptr->mnc.mnc_data,
                  code_data_ptr->mnc.mnc_len);

  return SIMLOCK_SUCCESS;
} /* simlock_category_copy_nw_code_to_response */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_COPY_MSIN_CODE_TO_RESPONSE

DESCRIPTION
  This function copies the MSIN code to the response data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_copy_msin_code_to_response
(
  simlock_msin_type                          * resp_msin_code_ptr,
  const simlock_category_code_data_type      * code_data_ptr
)
{
  if((code_data_ptr == NULL) ||
     (resp_msin_code_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if (code_data_ptr->msin.msin_len > SIMLOCK_MSIN_MAX)
  {
    SIMLOCK_MSG_ERR_1("MSIN data length too long. length: 0x%x",
                      code_data_ptr->msin.msin_len);
    return SIMLOCK_GENERIC_ERROR;
  }

  resp_msin_code_ptr->msin_len = code_data_ptr->msin.msin_len;

  simlock_memscpy(resp_msin_code_ptr->msin_data,
                  SIMLOCK_MSIN_MAX,
                  code_data_ptr->msin.msin_data,
                  code_data_ptr->msin.msin_len);

  return SIMLOCK_SUCCESS;
} /* simlock_category_copy_msin_code_to_response */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_GET_LIST_COUNT_FOR_CODE_TYPE

DESCRIPTION
  This function calculates the number of list needed for accomodating
  the codes. Each list can fit SIMLOCK_NUM_CODES_MAX count.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  num_of_lists  : Number of lists needed in the response to fit in
                  the codes

SIDE EFFECTS
  NONE
===========================================================================*/
static uint8 simlock_category_get_list_count_for_code_type
(
  simlock_category_file_data_type  *  category_data_ptr,
  simlock_code_enum_type              code_type
)
{
  uint8     code_cnt         = 0;
  uint8     code_type_count  = 0;
  uint8     list_count       = 0;

  if((category_data_ptr == NULL) ||
     (category_data_ptr->code_data_ptr == NULL))
  {
    return 0;
  }

  /* get the number of codes of the requested type */
  for(code_cnt = 0; code_cnt < category_data_ptr->category_header.num_of_codes;
      code_cnt++)
  {
    if(category_data_ptr->code_data_ptr[code_cnt].code_type == code_type)
    {
      code_type_count++;
    }
  }

  /* get the number of lists needed for the codes */
  list_count = SIMLOCK_CATEGORY_GET_LIST_COUNT(code_type_count);

  return list_count;
} /* simlock_category_get_list_count_for_code_type */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_GET_CODE_DATA

DESCRIPTION
  This function gets the lock data for a category
DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_get_code_data
(
  simlock_slot_enum_type                    slot,
  simlock_category_enum_type                category_type,
  simlock_get_category_data_msg_resp_type * resp_data_ptr
)
{
  simlock_result_enum_type            simlock_status        = SIMLOCK_SUCCESS;
  uint8                               code_cnt              = 0;
  uint8                               whitelist_cnt         = 0;
  uint8                               blacklist_cnt         = 0;
  uint8                               whitelist_code_count  = 0;
  uint8                               blacklist_code_count  = 0;
  simlock_category_file_data_type  *  category_data_ptr     = NULL;

  if(resp_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_0("simlock_category_get_code_data");

  SIMLOCK_CHECK_AND_MEM_MALLOC(category_data_ptr,
                               sizeof(simlock_category_file_data_type));
  if(category_data_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* read the lock code header and data */
  simlock_status = simlock_category_read_lock_data(category_type,
                                                   slot,
                                                   category_data_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    /* this frees the code data in the category */
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    SIMLOCK_MSG_ERR_3("read lock data failed, status: 0x%x,"
                      "category: 0x%x, slot: 0x%x",
                      simlock_status, category_type, slot);
    return simlock_status;
  }

  /* sanity check the file data */
  if(category_data_ptr->category_header.num_of_codes == 0)
  {
    /* this frees the code data in the category */
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    SIMLOCK_MSG_ERR_2("invalid or no lock data for category: 0x%x, slot: 0x%x",
                      category_type, slot);
    return SIMLOCK_GENERIC_ERROR;
  }

  if(category_data_ptr->code_data_ptr == NULL)
  {
    SIMLOCK_MEM_FREE(category_data_ptr);
    SIMLOCK_MSG_ERR_2("NULL code data for category: 0x%x, slot: 0x%x",
                      category_type, slot);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* get the number of lists needed in the response */
  resp_data_ptr->category_data_whitelist.num_of_lists = simlock_category_get_list_count_for_code_type(category_data_ptr,
                                                                                                      SIMLOCK_CODE_WHITELIST);

  resp_data_ptr->category_data_blacklist.num_of_lists = simlock_category_get_list_count_for_code_type(category_data_ptr,
                                                                                                      SIMLOCK_CODE_BLACKLIST);

  if((resp_data_ptr->category_data_whitelist.num_of_lists == 0) &&
     (resp_data_ptr->category_data_blacklist.num_of_lists == 0))
  {
    /* this frees the code data in the category */
    simlock_category_free_code_data(category_data_ptr);
    SIMLOCK_MEM_FREE(category_data_ptr);
    SIMLOCK_MSG_ERR_0("error getting the code count");
    return SIMLOCK_GENERIC_ERROR;
  }

  if(resp_data_ptr->category_data_whitelist.num_of_lists > 0)
  {
    SIMLOCK_CHECK_AND_MEM_MALLOC(resp_data_ptr->category_data_whitelist.whitelist_data_ptr,
                                 (resp_data_ptr->category_data_whitelist.num_of_lists * sizeof(simlock_category_data_type)));
    if(resp_data_ptr->category_data_whitelist.whitelist_data_ptr == NULL)
    {
      simlock_category_free_code_data(category_data_ptr);
      SIMLOCK_MEM_FREE(category_data_ptr);
      return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  if(resp_data_ptr->category_data_blacklist.num_of_lists > 0)
  {
    SIMLOCK_CHECK_AND_MEM_MALLOC(resp_data_ptr->category_data_blacklist.blacklist_data_ptr,
                                 (resp_data_ptr->category_data_blacklist.num_of_lists * sizeof(simlock_category_data_type)));
    if(resp_data_ptr->category_data_blacklist.blacklist_data_ptr == NULL)
    {
      simlock_category_free_code_data(category_data_ptr);
      SIMLOCK_MEM_FREE(category_data_ptr);
      SIMLOCK_MEM_FREE(resp_data_ptr->category_data_whitelist.whitelist_data_ptr);
      return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  switch(category_type)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      for(code_cnt = 0; code_cnt < category_data_ptr->category_header.num_of_codes;
          code_cnt++)
      {
        if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_WHITELIST)
        {
          if((whitelist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists))
          {
            whitelist_code_count = 0;
            whitelist_cnt++;
          }

          if((whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists) &&
             (whitelist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_NW;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.nw_3gpp_code_list.nw_code_data[whitelist_code_count],
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.nw_3gpp_code_list.num_nw_codes++;
            whitelist_code_count++;
          }
        }
        else if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          if((blacklist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists))
          {
            blacklist_code_count = 0;
            blacklist_cnt++;
          }

          if((blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists) &&
             (blacklist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_NW;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.nw_3gpp_code_list.nw_code_data[blacklist_code_count],
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.nw_3gpp_code_list.num_nw_codes++;
            blacklist_code_count++;
          }
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_NS:
      for(code_cnt = 0; code_cnt < category_data_ptr->category_header.num_of_codes;
          code_cnt++)
      {
        if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_WHITELIST)
        {
          if((whitelist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists))
          {
            whitelist_code_count = 0;
            whitelist_cnt++;
          }
          if((whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists) &&
             (whitelist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_NS;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.ns_3gpp_code_list.ns_code_data[whitelist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            /* copy d6 and d7 from msin[0] and msin[1] respectively */
            if(category_data_ptr->code_data_ptr[code_cnt].msin.msin_len >= 0x02)
            {
              resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.ns_3gpp_code_list.ns_code_data[whitelist_code_count].imsi_digit6 =
                   category_data_ptr->code_data_ptr[code_cnt].msin.msin_data[0];

              resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.ns_3gpp_code_list.ns_code_data[whitelist_code_count].imsi_digit7 =
                   category_data_ptr->code_data_ptr[code_cnt].msin.msin_data[1];
            }

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.ns_3gpp_code_list.num_ns_codes++;
            whitelist_code_count++;
          }
        }
        else if (category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          if((blacklist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists))
          {
            blacklist_code_count = 0;
            blacklist_cnt++;
          }

          if((blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists) &&
             (blacklist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_NS;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.ns_3gpp_code_list.ns_code_data[blacklist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            /* copy d6 and d7 from msin[0] and msin[1] respectively */
            if(category_data_ptr->code_data_ptr[code_cnt].msin.msin_len >= 0x02)
            {
              resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.ns_3gpp_code_list.ns_code_data[blacklist_code_count].imsi_digit6 =
                   category_data_ptr->code_data_ptr[code_cnt].msin.msin_data[0];

              resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.ns_3gpp_code_list.ns_code_data[blacklist_code_count].imsi_digit7 =
                   category_data_ptr->code_data_ptr[code_cnt].msin.msin_data[1];
            }

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.ns_3gpp_code_list.num_ns_codes++;
            blacklist_code_count++;
          }
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SP:
      for(code_cnt = 0; code_cnt < category_data_ptr->category_header.num_of_codes;
          code_cnt++)
      {
        if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_WHITELIST)
        {
          if((whitelist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists))
          {
            whitelist_code_count = 0;
            whitelist_cnt++;
          }
          if((whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists) &&
             (whitelist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_SP;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.sp_3gpp_code_list.sp_code_data[whitelist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.sp_3gpp_code_list.sp_code_data[whitelist_code_count].gid1.gid_len =
                category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len;

            simlock_memscpy(resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.sp_3gpp_code_list.sp_code_data[whitelist_code_count].gid1.gid_data,
                            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.sp_3gpp_code_list.sp_code_data[whitelist_code_count].gid1.gid_len,
                            category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
                            category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len);
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.sp_3gpp_code_list.num_sp_codes++;
            whitelist_code_count++;
          }
        }
        else if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          if((blacklist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists))
          {
            blacklist_code_count = 0;
            blacklist_cnt++;
          }

          if((blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists) &&
             (blacklist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_SP;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.sp_3gpp_code_list.sp_code_data[blacklist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.sp_3gpp_code_list.sp_code_data[blacklist_code_count].gid1.gid_len =
                category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len;

            simlock_memscpy(resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.sp_3gpp_code_list.sp_code_data[blacklist_code_count].gid1.gid_data,
                            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.sp_3gpp_code_list.sp_code_data[blacklist_code_count].gid1.gid_len,
                            category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
                            category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len);
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.sp_3gpp_code_list.num_sp_codes++;
            blacklist_code_count++;
          }
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_CP:
      for(code_cnt = 0; code_cnt < category_data_ptr->category_header.num_of_codes;
          code_cnt++)
      {
        if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_WHITELIST)
        {
          if((whitelist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists))
          {
            whitelist_code_count = 0;
            whitelist_cnt++;
          }
          if((whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists) &&
             (whitelist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_CP;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.cp_3gpp_code_list.cp_code_data[whitelist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.cp_3gpp_code_list.cp_code_data[whitelist_code_count].gid1.gid_len =
          category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len;

            simlock_memscpy(resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.cp_3gpp_code_list.cp_code_data[whitelist_code_count].gid1.gid_data,
                            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.cp_3gpp_code_list.cp_code_data[whitelist_code_count].gid1.gid_len,
                            category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
                            category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len);

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.cp_3gpp_code_list.cp_code_data[whitelist_code_count].gid2.gid_len =
              category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len;

            simlock_memscpy(resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.cp_3gpp_code_list.cp_code_data[whitelist_code_count].gid2.gid_data,
                            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.cp_3gpp_code_list.cp_code_data[whitelist_code_count].gid2.gid_len,
                            category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
                            category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len);
 
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.cp_3gpp_code_list.num_cp_codes++;
            whitelist_code_count++;
          }
        }
        else if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          if((blacklist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists))
          {
            blacklist_code_count = 0;
            blacklist_cnt++;
          }

          if((blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists) &&
             (blacklist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_CP;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.cp_3gpp_code_list.cp_code_data[blacklist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.cp_3gpp_code_list.cp_code_data[blacklist_code_count].gid1.gid_len =
              category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len;

            simlock_memscpy(resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.cp_3gpp_code_list.cp_code_data[blacklist_code_count].gid1.gid_data,
                            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.cp_3gpp_code_list.cp_code_data[blacklist_code_count].gid1.gid_len,
                        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_data,
                        category_data_ptr->code_data_ptr[code_cnt].gid1.gid_len);

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.cp_3gpp_code_list.cp_code_data[blacklist_code_count].gid2.gid_len =
          category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len;

            simlock_memscpy(resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.cp_3gpp_code_list.cp_code_data[blacklist_code_count].gid2.gid_data,
                            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.cp_3gpp_code_list.cp_code_data[blacklist_code_count].gid2.gid_len,
                            category_data_ptr->code_data_ptr[code_cnt].gid2.gid_data,
                            category_data_ptr->code_data_ptr[code_cnt].gid2.gid_len);

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.cp_3gpp_code_list.num_cp_codes++;
            blacklist_code_count++;
          }
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SIM:
      for(code_cnt = 0; code_cnt < category_data_ptr->category_header.num_of_codes;
          code_cnt++)
      {
        if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_WHITELIST)
        {
          if((whitelist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists))
          {
            whitelist_code_count = 0;
            whitelist_cnt++;
          }
          if((whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists) &&
             (whitelist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_SIM;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.sim_3gpp_code_list.sim_code_data[whitelist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            simlock_status = simlock_category_copy_msin_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.sim_3gpp_code_list.sim_code_data[whitelist_code_count].imsi_msin,
                                                                         &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.sim_3gpp_code_list.num_sim_codes++;
            whitelist_code_count++;
          }
        }
        else if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          if((blacklist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists))
          {
            blacklist_code_count = 0;
            blacklist_cnt++;
          }

          if((blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists) &&
             (blacklist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_SIM;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.sim_3gpp_code_list.sim_code_data[blacklist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            simlock_status = simlock_category_copy_msin_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.sim_3gpp_code_list.sim_code_data[blacklist_code_count].imsi_msin,
                                                                         &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.sim_3gpp_code_list.num_sim_codes++;
            blacklist_code_count++;
          }
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SPN:
      for(code_cnt = 0; code_cnt < category_data_ptr->category_header.num_of_codes;
          code_cnt++)
      {
        if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_WHITELIST)
        {
          if((whitelist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists))
          {
            whitelist_code_count = 0;
            whitelist_cnt++;
          }
          if((whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists) &&
             (whitelist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_SPN;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.spn_3gpp_code_list.spn_code_data[whitelist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;;
            }

            simlock_memscpy(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.spn_3gpp_code_list.spn_code_data[whitelist_code_count].spn,
                            sizeof(resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.spn_3gpp_code_list.spn_code_data[whitelist_code_count].spn),
                            &category_data_ptr->code_data_ptr[code_cnt].spn,
                            sizeof(category_data_ptr->code_data_ptr[code_cnt].spn));

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.spn_3gpp_code_list.num_spn_codes++;
            whitelist_code_count++;
          }
        }
        else if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          if((blacklist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists))
          {
            blacklist_code_count = 0;
            blacklist_cnt++;
          }

          if((blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists) &&
             (blacklist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_SPN;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.spn_3gpp_code_list.spn_code_data[blacklist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;;
            }

            simlock_memscpy(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.spn_3gpp_code_list.spn_code_data[blacklist_code_count].spn,
                            sizeof(resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.spn_3gpp_code_list.spn_code_data[blacklist_code_count].spn),
                            &category_data_ptr->code_data_ptr[code_cnt].spn,
                            sizeof(category_data_ptr->code_data_ptr[code_cnt].spn));

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.spn_3gpp_code_list.num_spn_codes++;
            blacklist_code_count++;
          }
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_ICCID:
      for(code_cnt = 0; code_cnt < category_data_ptr->category_header.num_of_codes;
          code_cnt++)
      {
        if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_WHITELIST)
        {
          if((whitelist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists))
          {
            whitelist_code_count = 0;
            whitelist_cnt++;
          }
          if((whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists) &&
             (whitelist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_ICCID;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[whitelist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;;
            }

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[whitelist_code_count].iccid.iccid_len =
                category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len;

            simlock_memscpy(resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[whitelist_code_count].iccid.iccid_data,
                            SIMLOCK_ICCID_DIGITS_MAX,
                            category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data,
                            category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len);

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.iccid_3gpp_code_list.num_iccid_codes++;
            whitelist_code_count++;
          }
        }
        else if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          if((blacklist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists))
          {
            blacklist_code_count = 0;
            blacklist_cnt++;
          }

          if((blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists) &&
             (blacklist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].category_type = SIMLOCK_CATEGORY_3GPP_ICCID;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[blacklist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;;
            }

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[blacklist_code_count].iccid.iccid_len =
              category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len;

            simlock_memscpy(resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.iccid_3gpp_code_list.iccid_code_data[blacklist_code_count].iccid.iccid_data,
                            SIMLOCK_ICCID_DIGITS_MAX,
                            category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_data,
                            category_data_ptr->code_data_ptr[code_cnt].iccid.iccid_len);

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.iccid_3gpp_code_list.num_iccid_codes++;
            blacklist_code_count++;
          }
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
      for(code_cnt = 0;
          code_cnt < category_data_ptr->category_header.num_of_codes;
          code_cnt++)
      {
        if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_WHITELIST)
        {
          if((whitelist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists))
          {
            whitelist_code_count = 0;
            whitelist_cnt++;
          }
          if((whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists) &&
             (whitelist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].category_type = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.nw_type1_3gpp2_code_list.nw_code_data[whitelist_code_count],
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.nw_type1_3gpp2_code_list.num_nw_codes++;
            whitelist_code_count++;
          }
        }
        else if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          if((blacklist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists))
          {
            blacklist_code_count = 0;
            blacklist_cnt++;
          }

          if((blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists) &&
             (blacklist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].category_type = SIMLOCK_CATEGORY_3GPP2_NW_TYPE1;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.nw_type1_3gpp2_code_list.nw_code_data[blacklist_code_count],
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.nw_type1_3gpp2_code_list.num_nw_codes++;
            blacklist_code_count++;
          }
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
      for(code_cnt = 0;
          code_cnt < category_data_ptr->category_header.num_of_codes;
          code_cnt++)
      {
        if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_WHITELIST)
        {
          if((whitelist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists))
          {
            whitelist_code_count = 0;
            whitelist_cnt++;
          }

          if((whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists) &&
             (whitelist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].category_type = SIMLOCK_CATEGORY_3GPP2_NW_TYPE2;

            simlock_memscpy(resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[whitelist_code_count].irm,
                            SIMLOCK_IRM_LEN,
                            category_data_ptr->code_data_ptr[code_cnt].msin.msin_data,
                            SIMLOCK_IRM_LEN);

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes++;
            whitelist_code_count++;
          }
        }
        else if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          if((blacklist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists))
          {
            blacklist_code_count = 0;
            blacklist_cnt++;
          }

          if((blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists) &&
             (blacklist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].category_type = SIMLOCK_CATEGORY_3GPP2_NW_TYPE2;

            simlock_memscpy(resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[blacklist_code_count].irm,
                            SIMLOCK_IRM_LEN,
                            category_data_ptr->code_data_ptr[code_cnt].msin.msin_data,
                            SIMLOCK_IRM_LEN);

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes++;
            blacklist_code_count++;
          }
        }
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_RUIM:
      for(code_cnt = 0;
          code_cnt < category_data_ptr->category_header.num_of_codes;
          code_cnt++)
      {
        if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_WHITELIST)
        {
          if((whitelist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists))
          {
            whitelist_code_count = 0;
            whitelist_cnt++;
          }

          if((whitelist_cnt < resp_data_ptr->category_data_whitelist.num_of_lists) &&
             (whitelist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].category_type = SIMLOCK_CATEGORY_3GPP2_RUIM;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[whitelist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            simlock_status = simlock_category_copy_msin_code_to_response(&resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[whitelist_code_count].imsi_msin,
                                                                         &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_whitelist.whitelist_data_ptr[whitelist_cnt].code_data.ruim_3gpp2_code_list.num_sim_codes++;
            whitelist_code_count++;
          }
        }
        else if(category_data_ptr->code_data_ptr[code_cnt].code_type == SIMLOCK_CODE_BLACKLIST)
        {
          if((blacklist_code_count == SIMLOCK_NUM_CODES_MAX) &&
             (blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists))
          {
            blacklist_code_count = 0;
            blacklist_cnt++;
          }

          if((blacklist_cnt < resp_data_ptr->category_data_blacklist.num_of_lists) &&
             (blacklist_code_count < SIMLOCK_NUM_CODES_MAX))
          {
            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].category_type = SIMLOCK_CATEGORY_3GPP2_RUIM;

            simlock_status = simlock_category_copy_nw_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[blacklist_code_count].nw_code,
                                                                       &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            simlock_status = simlock_category_copy_msin_code_to_response(&resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.ruim_3gpp2_code_list.sim_code_data[blacklist_code_count].imsi_msin,
                                                                         &category_data_ptr->code_data_ptr[code_cnt]);
            if(simlock_status != SIMLOCK_SUCCESS)
            {
              return simlock_status;
            }

            resp_data_ptr->category_data_blacklist.blacklist_data_ptr[blacklist_cnt].code_data.ruim_3gpp2_code_list.num_sim_codes++;
            blacklist_code_count++;
          }
        }
      }
      break;

    default:
      /* this frees the code data in the category */
      simlock_category_free_code_data(category_data_ptr);
      SIMLOCK_MEM_FREE(category_data_ptr);
      return SIMLOCK_GENERIC_ERROR;
  }

  /* this frees the code data in the category */
  simlock_category_free_code_data(category_data_ptr);
  SIMLOCK_MEM_FREE(category_data_ptr);

  return SIMLOCK_SUCCESS;
} /* simlock_category_get_code_data */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_COPY_NW_CODE_FROM_REQUEST

DESCRIPTION
  This function copies the NW code from the request data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_copy_nw_code_from_request
(
  simlock_category_code_data_type      * code_data_ptr,
  const simlock_nw_code_data_type      * req_nw_code_ptr
)
{
  if((code_data_ptr == NULL) ||
     (req_nw_code_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  simlock_memscpy(code_data_ptr->mcc,
                  SIMLOCK_MCC_LEN,
                  req_nw_code_ptr->mcc,
                  SIMLOCK_MCC_LEN);

  if (req_nw_code_ptr->mnc.mnc_len > SIMLOCK_MNC_MAX)
  {
    SIMLOCK_MSG_ERR_1("MNC data length too long. length: 0x%x",
                      req_nw_code_ptr->mnc.mnc_len);
    return SIMLOCK_GENERIC_ERROR;
  }

  code_data_ptr->mnc.mnc_len = req_nw_code_ptr->mnc.mnc_len;

  simlock_memscpy(code_data_ptr->mnc.mnc_data,
                  SIMLOCK_MNC_MAX,
                  req_nw_code_ptr->mnc.mnc_data,
                  req_nw_code_ptr->mnc.mnc_len);

  return SIMLOCK_SUCCESS;
} /* simlock_category_copy_nw_code_from_request */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_COPY_MSIN_CODE_FROM_REQUEST

DESCRIPTION
  This function copies the MSIN code from the request data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_copy_msin_code_from_request
(
  simlock_category_code_data_type      * code_data_ptr,
  const simlock_msin_type              * req_msin_code_ptr
)
{
  if((code_data_ptr == NULL) ||
     (req_msin_code_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if (req_msin_code_ptr->msin_len > SIMLOCK_MSIN_MAX)
  {
    SIMLOCK_MSG_ERR_1("MSIN data length too long. length: 0x%x",
                      req_msin_code_ptr->msin_len);
    return SIMLOCK_GENERIC_ERROR;
  }

  code_data_ptr->msin.msin_len = req_msin_code_ptr->msin_len;

  simlock_memscpy(code_data_ptr->msin.msin_data,
                  SIMLOCK_MSIN_MAX,
                  req_msin_code_ptr->msin_data,
                  req_msin_code_ptr->msin_len);

  return SIMLOCK_SUCCESS;
} /* simlock_category_copy_msin_code_from_request */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_COPY_GID_CODE_FROM_REQUEST

DESCRIPTION
  This function copies the GID code from the request data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_category_copy_gid_code_from_request
(
  simlock_gid_type                     * code_data_ptr,
  const simlock_gid_type               * req_gid_code_ptr
)
{
  if((code_data_ptr == NULL) ||
     (req_gid_code_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if (req_gid_code_ptr->gid_len > SIMLOCK_MAX_GID_LEN)
  {
    SIMLOCK_MSG_ERR_1("GID data length too long. length: 0x%x",
                      req_gid_code_ptr->gid_len);
    return SIMLOCK_GENERIC_ERROR;
  }

  code_data_ptr->gid_len = req_gid_code_ptr->gid_len;

  simlock_memscpy(code_data_ptr->gid_data,
                  SIMLOCK_MAX_GID_LEN,
                  req_gid_code_ptr->gid_data,
                  req_gid_code_ptr->gid_len);

  return SIMLOCK_SUCCESS;
} /* simlock_category_copy_gid_code_from_request */


/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_APPEND_LOCK_DATA

DESCRIPTION
  This function appends the lock data for a category from the request.It
  allocates memory based on the total number of lock codes. The caller of
  this function is responsible to free that memory.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_append_lock_data
(
  const simlock_category_data_type      *  new_code_data_ptr,
  const boolean                            blacklist,
  simlock_category_file_data_type       *  category_data_ptr
)
{
  simlock_result_enum_type           simlock_status              = SIMLOCK_SUCCESS;
  uint8                              code_cnt                    = 0;
  uint8                              total_codes_cnt             = 0;
  uint32                             appended_code_data_length   = 0;
  uint8                              appended_code_index         = 0;
  simlock_category_code_data_type  * appended_code_data_ptr      = NULL;

  if((category_data_ptr == NULL)  ||
     (category_data_ptr->code_data_ptr == NULL) ||
     (new_code_data_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameter error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* append the new code data from the request, to the
     existing codes */
  switch(new_code_data_ptr->category_type)
  {
    case SIMLOCK_CATEGORY_3GPP_NW:
      if(new_code_data_ptr->code_data.nw_3gpp_code_list.num_nw_codes == 0)
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      total_codes_cnt = (category_data_ptr->category_header.num_of_codes +
                         new_code_data_ptr->code_data.nw_3gpp_code_list.num_nw_codes);

      appended_code_data_length = (total_codes_cnt *
                                   sizeof(simlock_category_code_data_type));

      /* caller is responsible to free the memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(appended_code_data_ptr,
                                   appended_code_data_length);
      if(appended_code_data_ptr == NULL)
      {
        return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      /* copy the existing codes */
      simlock_memscpy(appended_code_data_ptr,
                      appended_code_data_length,
                      category_data_ptr->code_data_ptr,
                      category_data_ptr->code_data_length);

      /* add the new codes */
      for(code_cnt = 0, appended_code_index = category_data_ptr->category_header.num_of_codes;
          code_cnt < new_code_data_ptr->code_data.nw_3gpp_code_list.num_nw_codes &&
            appended_code_index < total_codes_cnt;
          code_cnt++, appended_code_index++)
      {
        if(blacklist)
        {
          appended_code_data_ptr[appended_code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_status = simlock_category_copy_nw_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                    &new_code_data_ptr->code_data.nw_3gpp_code_list.nw_code_data[code_cnt]);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        appended_code_data_ptr[appended_code_index].msin.msin_len = 0;
        memset(appended_code_data_ptr[appended_code_index].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        memset(appended_code_data_ptr[appended_code_index].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid1.gid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid2.gid_len = 0;
        appended_code_data_ptr[appended_code_index].spn.spn_valid = FALSE;
        memset(appended_code_data_ptr[appended_code_index].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        appended_code_data_ptr[appended_code_index].iccid.iccid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_NS:
      if(new_code_data_ptr->code_data.ns_3gpp_code_list.num_ns_codes == 0)
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      total_codes_cnt = (category_data_ptr->category_header.num_of_codes +
                         new_code_data_ptr->code_data.ns_3gpp_code_list.num_ns_codes);
      appended_code_data_length = (total_codes_cnt *
                                   sizeof(simlock_category_code_data_type));

      /* caller is responsible to free the memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(appended_code_data_ptr,
                                   appended_code_data_length);
      if(appended_code_data_ptr == NULL)
      {
        return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      /* copy the existing codes */
      simlock_memscpy(appended_code_data_ptr,
                      appended_code_data_length,
                      category_data_ptr->code_data_ptr,
                      category_data_ptr->code_data_length);

      /* add the new codes */
      for(code_cnt = 0, appended_code_index = category_data_ptr->category_header.num_of_codes;
          code_cnt < new_code_data_ptr->code_data.ns_3gpp_code_list.num_ns_codes &&
            appended_code_index < total_codes_cnt;
          code_cnt++, appended_code_index++)
      {
        if(blacklist)
        {
          appended_code_data_ptr[appended_code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_status = simlock_category_copy_nw_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                    &new_code_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[code_cnt].nw_code);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        /* copy d6 and d7 to msin_data[0] and msin_data[1] respectively */
        appended_code_data_ptr[appended_code_index].msin.msin_data[0] =
          new_code_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[code_cnt].imsi_digit6;

        appended_code_data_ptr[appended_code_index].msin.msin_data[1] =
          new_code_data_ptr->code_data.ns_3gpp_code_list.ns_code_data[code_cnt].imsi_digit7;

        appended_code_data_ptr[appended_code_index].msin.msin_len = 0x02;

        memset(appended_code_data_ptr[appended_code_index].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid1.gid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid2.gid_len = 0;
        appended_code_data_ptr[appended_code_index].spn.spn_valid = FALSE;
        memset(appended_code_data_ptr[appended_code_index].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        appended_code_data_ptr[appended_code_index].iccid.iccid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SP:
      if(new_code_data_ptr->code_data.sp_3gpp_code_list.num_sp_codes == 0)
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      total_codes_cnt = (category_data_ptr->category_header.num_of_codes +
                         new_code_data_ptr->code_data.sp_3gpp_code_list.num_sp_codes);
      appended_code_data_length = (total_codes_cnt *
                                   sizeof(simlock_category_code_data_type));

      /* caller is responsible to free the memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(appended_code_data_ptr,
                                   appended_code_data_length);
      if(appended_code_data_ptr == NULL)
      {
        return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      /* copy the existing codes */
      simlock_memscpy(appended_code_data_ptr,
                      appended_code_data_length,
                      category_data_ptr->code_data_ptr,
                      category_data_ptr->code_data_length);

      /* add the new codes */
      for(code_cnt = 0, appended_code_index = category_data_ptr->category_header.num_of_codes;
          code_cnt < new_code_data_ptr->code_data.sp_3gpp_code_list.num_sp_codes &&
            appended_code_index < total_codes_cnt;
          code_cnt++, appended_code_index++)
      {
        if(blacklist)
        {
          appended_code_data_ptr[appended_code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_status = simlock_category_copy_nw_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                    &new_code_data_ptr->code_data.sp_3gpp_code_list.sp_code_data[code_cnt].nw_code);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        simlock_status = simlock_category_copy_gid_code_from_request(&appended_code_data_ptr[appended_code_index].gid1,
                                                    &new_code_data_ptr->code_data.sp_3gpp_code_list.sp_code_data[code_cnt].gid1);

        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        appended_code_data_ptr[appended_code_index].msin.msin_len = 0;
        memset(appended_code_data_ptr[appended_code_index].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        memset(appended_code_data_ptr[appended_code_index].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid2.gid_len = 0;
        appended_code_data_ptr[appended_code_index].spn.spn_valid = FALSE;
        memset(appended_code_data_ptr[appended_code_index].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        appended_code_data_ptr[appended_code_index].iccid.iccid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_CP:
      if(new_code_data_ptr->code_data.cp_3gpp_code_list.num_cp_codes == 0)
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      total_codes_cnt = (category_data_ptr->category_header.num_of_codes +
                         new_code_data_ptr->code_data.cp_3gpp_code_list.num_cp_codes);
      appended_code_data_length = (total_codes_cnt *
                                   sizeof(simlock_category_code_data_type));

      /* caller is responsible to free the memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(appended_code_data_ptr,
                                   appended_code_data_length);
      if(appended_code_data_ptr == NULL)
      {
        return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      /* copy the existing codes */
      simlock_memscpy(appended_code_data_ptr,
                      appended_code_data_length,
                      category_data_ptr->code_data_ptr,
                      category_data_ptr->code_data_length);

      /* add the new codes */
      for(code_cnt = 0, appended_code_index = category_data_ptr->category_header.num_of_codes;
          code_cnt < new_code_data_ptr->code_data.cp_3gpp_code_list.num_cp_codes &&
            appended_code_index < total_codes_cnt;
          code_cnt++, appended_code_index++)
      {
        if(blacklist)
        {
          appended_code_data_ptr[appended_code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_status = simlock_category_copy_nw_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                    &new_code_data_ptr->code_data.cp_3gpp_code_list.cp_code_data[code_cnt].nw_code);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        simlock_status = simlock_category_copy_gid_code_from_request(&appended_code_data_ptr[appended_code_index].gid1,
                                                                     &new_code_data_ptr->code_data.cp_3gpp_code_list.cp_code_data[code_cnt].gid1);

        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        simlock_status = simlock_category_copy_gid_code_from_request(&appended_code_data_ptr[appended_code_index].gid2,
                                                                     &new_code_data_ptr->code_data.cp_3gpp_code_list.cp_code_data[code_cnt].gid2);

        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        appended_code_data_ptr[appended_code_index].msin.msin_len = 0;
        memset(appended_code_data_ptr[appended_code_index].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        appended_code_data_ptr[appended_code_index].spn.spn_valid = FALSE;
        memset(appended_code_data_ptr[appended_code_index].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        appended_code_data_ptr[appended_code_index].iccid.iccid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SIM:
      if(new_code_data_ptr->code_data.sim_3gpp_code_list.num_sim_codes == 0)
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      total_codes_cnt = (category_data_ptr->category_header.num_of_codes +
                         new_code_data_ptr->code_data.sim_3gpp_code_list.num_sim_codes);
      appended_code_data_length = (total_codes_cnt *
                                   sizeof(simlock_category_code_data_type));

      /* caller is responsible to free the memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(appended_code_data_ptr,
                                   appended_code_data_length);
      if(appended_code_data_ptr == NULL)
      {
        return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      /* copy the existing codes */
      simlock_memscpy(appended_code_data_ptr,
                      appended_code_data_length,
                      category_data_ptr->code_data_ptr,
                      category_data_ptr->code_data_length);

      /* add the new codes */
      for(code_cnt = 0, appended_code_index = category_data_ptr->category_header.num_of_codes;
          code_cnt < new_code_data_ptr->code_data.sim_3gpp_code_list.num_sim_codes &&
            appended_code_index < total_codes_cnt;
          code_cnt++, appended_code_index++)
      {
        if(blacklist)
        {
          appended_code_data_ptr[appended_code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_status = simlock_category_copy_nw_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                    &new_code_data_ptr->code_data.sim_3gpp_code_list.sim_code_data[code_cnt].nw_code);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        simlock_status = simlock_category_copy_msin_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                      &new_code_data_ptr->code_data.sim_3gpp_code_list.sim_code_data[code_cnt].imsi_msin);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        memset(appended_code_data_ptr[appended_code_index].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid1.gid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid2.gid_len = 0;
        appended_code_data_ptr[appended_code_index].spn.spn_valid = FALSE;
        memset(appended_code_data_ptr[appended_code_index].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        appended_code_data_ptr[appended_code_index].iccid.iccid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_SPN:
      if(new_code_data_ptr->code_data.spn_3gpp_code_list.num_spn_codes == 0)
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      total_codes_cnt = (category_data_ptr->category_header.num_of_codes +
                         new_code_data_ptr->code_data.spn_3gpp_code_list.num_spn_codes);
      appended_code_data_length = (total_codes_cnt *
                                   sizeof(simlock_category_code_data_type));

      /* caller is responsible to free the memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(appended_code_data_ptr,
                                   appended_code_data_length);
      if(appended_code_data_ptr == NULL)
      {
        return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      /* copy the existing codes */
      simlock_memscpy(appended_code_data_ptr,
                      appended_code_data_length,
                      category_data_ptr->code_data_ptr,
                      category_data_ptr->code_data_length);

      /* add the new codes */
      for(code_cnt = 0, appended_code_index = category_data_ptr->category_header.num_of_codes;
          code_cnt < new_code_data_ptr->code_data.spn_3gpp_code_list.num_spn_codes &&
            appended_code_index < total_codes_cnt;
          code_cnt++, appended_code_index++)
      {
        if(blacklist)
        {
          appended_code_data_ptr[appended_code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_status = simlock_category_copy_nw_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                    &new_code_data_ptr->code_data.spn_3gpp_code_list.spn_code_data[code_cnt].nw_code);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        simlock_memscpy(&appended_code_data_ptr[appended_code_index].spn,
                        sizeof(appended_code_data_ptr[appended_code_index].spn),
                        &new_code_data_ptr->code_data.spn_3gpp_code_list.spn_code_data[code_cnt].spn,
                        sizeof(new_code_data_ptr->code_data.spn_3gpp_code_list.spn_code_data[code_cnt].spn));

        memset(appended_code_data_ptr[appended_code_index].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid1.gid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid2.gid_len = 0;

        appended_code_data_ptr[appended_code_index].msin.msin_len = 0;
        memset(appended_code_data_ptr[appended_code_index].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        appended_code_data_ptr[appended_code_index].iccid.iccid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP_ICCID:
      if(new_code_data_ptr->code_data.iccid_3gpp_code_list.num_iccid_codes == 0)
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      total_codes_cnt = (category_data_ptr->category_header.num_of_codes +
                         new_code_data_ptr->code_data.iccid_3gpp_code_list.num_iccid_codes);
      appended_code_data_length = (total_codes_cnt *
                                   sizeof(simlock_category_code_data_type));

      /* caller is responsible to free the memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(appended_code_data_ptr,
                                   appended_code_data_length);
      if(appended_code_data_ptr == NULL)
      {
        return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      /* copy the existing codes */
      simlock_memscpy(appended_code_data_ptr,
                      appended_code_data_length,
                      category_data_ptr->code_data_ptr,
                      category_data_ptr->code_data_length);

      /* add the new codes */
      for(code_cnt = 0, appended_code_index = category_data_ptr->category_header.num_of_codes;
          code_cnt < new_code_data_ptr->code_data.iccid_3gpp_code_list.num_iccid_codes &&
            appended_code_index < total_codes_cnt;
          code_cnt++, appended_code_index++)
      {
        if(blacklist)
        {
          appended_code_data_ptr[appended_code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_status = simlock_category_copy_nw_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                    &new_code_data_ptr->code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].nw_code);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        appended_code_data_ptr[appended_code_index].iccid.iccid_len =
          new_code_data_ptr->code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].iccid.iccid_len;

        simlock_memscpy(appended_code_data_ptr[appended_code_index].iccid.iccid_data,
                        SIMLOCK_ICCID_DIGITS_MAX,
                        new_code_data_ptr->code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].iccid.iccid_data,
                        new_code_data_ptr->code_data.iccid_3gpp_code_list.iccid_code_data[code_cnt].iccid.iccid_len);

        memset(appended_code_data_ptr[appended_code_index].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT,
               sizeof(appended_code_data_ptr[appended_code_index].gid1.gid_data));
        appended_code_data_ptr[appended_code_index].gid1.gid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT,
               sizeof(appended_code_data_ptr[appended_code_index].gid2.gid_data));
        appended_code_data_ptr[appended_code_index].gid2.gid_len = 0;

        appended_code_data_ptr[appended_code_index].msin.msin_len = 0;
        memset(appended_code_data_ptr[appended_code_index].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        appended_code_data_ptr[appended_code_index].spn.spn_valid = FALSE;
        memset(appended_code_data_ptr[appended_code_index].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE1:
      if(new_code_data_ptr->code_data.nw_type1_3gpp2_code_list.num_nw_codes == 0)
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      total_codes_cnt = (category_data_ptr->category_header.num_of_codes +
                         new_code_data_ptr->code_data.nw_type1_3gpp2_code_list.num_nw_codes);
      appended_code_data_length = (total_codes_cnt *
                                   sizeof(simlock_category_code_data_type));

      /* caller is responsible to free the memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(appended_code_data_ptr,
                                   appended_code_data_length);
      if(appended_code_data_ptr == NULL)
      {
        return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      /* copy the existing codes */
      simlock_memscpy(appended_code_data_ptr,
                      appended_code_data_length,
                      category_data_ptr->code_data_ptr,
                      category_data_ptr->code_data_length);

      /* add the new codes */
      for(code_cnt = 0, appended_code_index = category_data_ptr->category_header.num_of_codes;
          code_cnt < new_code_data_ptr->code_data.nw_type1_3gpp2_code_list.num_nw_codes &&
            appended_code_index < total_codes_cnt;
          code_cnt++, appended_code_index++)
      {
        if(blacklist)
        {
          appended_code_data_ptr[appended_code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_status = simlock_category_copy_nw_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                    &new_code_data_ptr->code_data.nw_type1_3gpp2_code_list.nw_code_data[code_cnt]);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        appended_code_data_ptr[appended_code_index].msin.msin_len = 0;
        memset(appended_code_data_ptr[appended_code_index].msin.msin_data, 0x7F, SIMLOCK_MSIN_MAX);
        memset(appended_code_data_ptr[appended_code_index].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid1.gid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid2.gid_len = 0;
        appended_code_data_ptr[appended_code_index].spn.spn_valid = FALSE;
        memset(appended_code_data_ptr[appended_code_index].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        appended_code_data_ptr[appended_code_index].iccid.iccid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_NW_TYPE2:
      if(new_code_data_ptr->code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes == 0)
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      total_codes_cnt = (category_data_ptr->category_header.num_of_codes +
                         new_code_data_ptr->code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes);
      appended_code_data_length = (total_codes_cnt *
                                   sizeof(simlock_category_code_data_type));

      /* caller is responsible to free the memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(appended_code_data_ptr,
                                   appended_code_data_length);
      if(appended_code_data_ptr == NULL)
      {
        return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      /* copy the existing codes */
      simlock_memscpy(appended_code_data_ptr,
                      appended_code_data_length,
                      category_data_ptr->code_data_ptr,
                      category_data_ptr->code_data_length);

      /* add the new codes */
      for(code_cnt = 0, appended_code_index = category_data_ptr->category_header.num_of_codes;
          code_cnt < new_code_data_ptr->code_data.nw_type2_3gpp2_code_list.num_nw_type2_codes &&
            appended_code_index < total_codes_cnt;
          code_cnt++, appended_code_index++)
      {
        if(blacklist)
        {
          appended_code_data_ptr[appended_code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }
        appended_code_data_ptr[appended_code_index].msin.msin_len =
             SIMLOCK_IRM_LEN;
        simlock_memscpy(appended_code_data_ptr[appended_code_index].msin.msin_data,
                        SIMLOCK_MSIN_MAX,
                        new_code_data_ptr->code_data.nw_type2_3gpp2_code_list.nw_type2_code_data[code_cnt].irm,
                        SIMLOCK_IRM_LEN);

        memset(appended_code_data_ptr[appended_code_index].mcc, 0x7F, SIMLOCK_MCC_LEN);
        memset(appended_code_data_ptr[appended_code_index].mnc.mnc_data, 0x7F, SIMLOCK_MNC_MAX);

        appended_code_data_ptr[appended_code_index].mnc.mnc_len = 0;

        memset(&appended_code_data_ptr[appended_code_index].msin.msin_data[SIMLOCK_IRM_LEN], 0x7F,
                SIMLOCK_MSIN_MAX - SIMLOCK_IRM_LEN);

        memset(appended_code_data_ptr[appended_code_index].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid1.gid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid2.gid_len = 0;
        appended_code_data_ptr[appended_code_index].spn.spn_valid = FALSE;
        memset(appended_code_data_ptr[appended_code_index].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        appended_code_data_ptr[appended_code_index].iccid.iccid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    case SIMLOCK_CATEGORY_3GPP2_RUIM:
      if(new_code_data_ptr->code_data.ruim_3gpp2_code_list.num_sim_codes == 0)
      {
        return SIMLOCK_GENERIC_ERROR;
      }
      total_codes_cnt = (category_data_ptr->category_header.num_of_codes +
                         new_code_data_ptr->code_data.ruim_3gpp2_code_list.num_sim_codes);
      appended_code_data_length = (total_codes_cnt *
                                   sizeof(simlock_category_code_data_type));

      /* caller is responsible to free the memory */
      SIMLOCK_CHECK_AND_MEM_MALLOC(appended_code_data_ptr,
                                   appended_code_data_length);
      if(appended_code_data_ptr == NULL)
      {
        return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      /* copy the existing codes */
      simlock_memscpy(appended_code_data_ptr,
                      appended_code_data_length,
                      category_data_ptr->code_data_ptr,
                      category_data_ptr->code_data_length);

      /* add the new codes */
      for(code_cnt = 0, appended_code_index = category_data_ptr->category_header.num_of_codes;
          code_cnt < new_code_data_ptr->code_data.ruim_3gpp2_code_list.num_sim_codes &&
            appended_code_index < total_codes_cnt;
          code_cnt++, appended_code_index++)
      {
        if(blacklist)
        {
          appended_code_data_ptr[appended_code_index].code_type = SIMLOCK_CODE_BLACKLIST;
        }

        simlock_status = simlock_category_copy_nw_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                    &new_code_data_ptr->code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].nw_code);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        simlock_status = simlock_category_copy_msin_code_from_request(&appended_code_data_ptr[appended_code_index],
                                                                      &new_code_data_ptr->code_data.ruim_3gpp2_code_list.sim_code_data[code_cnt].imsi_msin);
        if(simlock_status != SIMLOCK_SUCCESS)
        {
          break;
        }

        memset(appended_code_data_ptr[appended_code_index].gid1.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid1.gid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].gid2.gid_data,
               SIMLOCK_LOCK_CODE_WILD_DIGIT, SIMLOCK_MAX_GID_LEN);
        appended_code_data_ptr[appended_code_index].gid2.gid_len = 0;
        appended_code_data_ptr[appended_code_index].spn.spn_valid = FALSE;
        memset(appended_code_data_ptr[appended_code_index].spn.spn_data, 0xFF, SIMLOCK_SPN_LEN);
        appended_code_data_ptr[appended_code_index].iccid.iccid_len = 0;
        memset(appended_code_data_ptr[appended_code_index].iccid.iccid_data, 0xFF, SIMLOCK_ICCID_DIGITS_MAX);
      }
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }

  if((simlock_status == SIMLOCK_SUCCESS) &&
     (appended_code_data_ptr != NULL))
  {
    /* remove the existing codes */
    simlock_category_free_code_data(category_data_ptr);

    /* update new codes and existing codes in the code data */
    category_data_ptr->code_data_length             = appended_code_data_length;
    category_data_ptr->code_data_ptr                = appended_code_data_ptr;
    category_data_ptr->category_header.num_of_codes = total_codes_cnt;
  }

  return simlock_status;
} /* simlock_category_append_lock_data */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#endif /* FEATURE_SIMLOCK */
