/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                  M M G S D I   E O N S   O P L   P N N   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines related to finding a name from OPL PNN
  files on the SIM

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_eons_opl_pnn.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   ar      EONS selective logging
05/24/16   bcho    EONS F3 reduction
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/11/16   sp      Reduce F3 logging
11/19/15   sp      Reduce F3 logging
09/30/15   ar      Fake the fourth byte of EF AD for buggy card
12/03/14   bcho    featurize support to use stored rplmn name across operators
09/29/14   tkl     Added support for persistent cache
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
06/03/14   tl      Remove additional parameter variables
04/03/14   av      Don't allow access if UST/CST can't be read(pin not verified)
03/28/14   ar      Removed un-necessary check with TRUE
03/11/14   bcho    Don't run EONS algo if OPL-PNN not cached
02/13/14   ar      Assign boolean value to country initials in PLMN name
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
11/22/13   bcho    Use roaming status received in CM SS event in EONS algorithm
10/25/13   hh      Send OPL_PNN_CACHE_READY_EVT on off-target build
10/04/13   rp      F3 message reduction
08/22/13   vdc     Added support for TSTS
08/21/13   rp      Format specifier added in F3 message
07/15/13   vs      EONS updates to support single baseband SGLTE

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_session.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_eons_opl_pnn.h"
#include "uim_selective_logging.h"

#ifdef FEATURE_UIM_PERSISTENT_CACHE
#include "mmgsdi_persistent_cache.h"
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_OPL_RECORD_DATA_TYPE

   DESCRIPTION:
     Data Structure to hold OPL record.
-------------------------------------------------------------------------------*/

typedef struct{
  mmgsdi_plmn_id_type        plmn_id;
  uint16                     lac_start;
  uint16                     lac_end;
  uint8                      pnn_record_index;
}mmgsdi_eons_opl_record_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_PNN_RECORD_DATA_TYPE

   DESCRIPTION:
     Data Structure to hold PNN record.
-------------------------------------------------------------------------------*/

typedef struct{
  boolean                    full_name_identifier;
  uint8                      full_name_length;
  uint8                    * full_name_ptr;
  boolean                    short_name_identifier;
  uint8                      short_name_length;
  uint8                    * short_name_ptr;
  uint8                      additional_info_tag;
  uint8                      additional_info_length;
  uint8                    * additional_info_ptr;
}mmgsdi_eons_pnn_record_data_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_EONS_CACHE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the EONS caches
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_EONS_CACHE_PNN,
  MMGSDI_EONS_CACHE_OPL,
  MMGSDI_EONS_CACHE_MAX
} mmgsdi_eons_cache_enum_type;

/* Primary GW opl cache (SIM or USIM) */
static mmgsdi_cache_record_ef_data_type mmgsdi_pri_gw_opl_cache;

/* Secondary GW opl cache (SIM or USIM) */
static mmgsdi_cache_record_ef_data_type mmgsdi_sec_gw_opl_cache;

/* Tertiary GW opl cache (SIM or USIM) */
static mmgsdi_cache_record_ef_data_type mmgsdi_ter_gw_opl_cache;

/* Primary GW pnn cache (SIM or USIM) */
static mmgsdi_cache_record_ef_data_type mmgsdi_pri_gw_pnn_cache;

/* Secondary GW pnn cache (SIM or USIM) */
static mmgsdi_cache_record_ef_data_type mmgsdi_sec_gw_pnn_cache;

/* Tertiary GW pnn cache (SIM or USIM) */
static mmgsdi_cache_record_ef_data_type mmgsdi_ter_gw_pnn_cache;


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_GET_CACHE_PTR

DESCRIPTION
  Function to get OPL-PNN cache ptr

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_get_cache_ptr(
  mmgsdi_session_id_type              session_id,
  mmgsdi_eons_cache_enum_type         cache_type,
  mmgsdi_cache_record_ef_data_type ** opl_pnn_cache_pptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_int_app_info_type        * int_app_info_ptr = NULL;
  mmgsdi_session_info_type        * session_info_ptr = NULL;

  if(opl_pnn_cache_pptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  if(mmgsdi_status == MMGSDI_SUCCESS &&
     int_app_info_ptr != NULL &&
     session_info_ptr != NULL)
  {
    switch(session_info_ptr->session_type)
    {
      case MMGSDI_GW_PROV_PRI_SESSION:
        if(cache_type == MMGSDI_EONS_CACHE_OPL)
        {
          *opl_pnn_cache_pptr = &mmgsdi_pri_gw_opl_cache;
        }
        else if(cache_type == MMGSDI_EONS_CACHE_PNN)
        {
          *opl_pnn_cache_pptr = &mmgsdi_pri_gw_pnn_cache;
        }
        else
        {
          mmgsdi_status = MMGSDI_ERROR;
        }
        break;

      case MMGSDI_GW_PROV_SEC_SESSION:
        if(cache_type == MMGSDI_EONS_CACHE_OPL)
        {
          *opl_pnn_cache_pptr = &mmgsdi_sec_gw_opl_cache;
        }
        else if(cache_type == MMGSDI_EONS_CACHE_PNN)
        {
          *opl_pnn_cache_pptr = &mmgsdi_sec_gw_pnn_cache;
        }
        else
        {
          mmgsdi_status = MMGSDI_ERROR;
        }
        break;

      case MMGSDI_GW_PROV_TER_SESSION:
        if(cache_type == MMGSDI_EONS_CACHE_OPL)
        {
          *opl_pnn_cache_pptr = &mmgsdi_ter_gw_opl_cache;
        }
        else if(cache_type == MMGSDI_EONS_CACHE_PNN)
        {
          *opl_pnn_cache_pptr = &mmgsdi_ter_gw_pnn_cache;
        }
        else
        {
          mmgsdi_status = MMGSDI_ERROR;
        }
        break;

      default:
        mmgsdi_status = MMGSDI_ERROR;
        break;
    }
  }
  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_get_cache_ptr */


#ifdef FEATURE_UIM_PERSISTENT_CACHE
/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_UPDATE_RECORD_EF_INFO

DESCRIPTION
  Function to update record EF info in OPL-PNN cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_update_record_ef_info(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  mmgsdi_len_type          len_of_record,
  mmgsdi_rec_num_type      num_of_records
)
{
  mmgsdi_eons_cache_enum_type        cache_type        = MMGSDI_EONS_CACHE_MAX;
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr = NULL;

  if(file_enum == MMGSDI_USIM_OPL)
  {
    cache_type = MMGSDI_EONS_CACHE_OPL;
  }
  else if(file_enum == MMGSDI_USIM_PNN)
  {
    cache_type = MMGSDI_EONS_CACHE_PNN;
  }
  else
  {
    return MMGSDI_ERROR;
  }

  /*get cache ptr*/
  mmgsdi_status =  mmgsdi_eons_opl_pnn_get_cache_ptr(session_id, cache_type,
                                                     &opl_pnn_cache_ptr);

  if(opl_pnn_cache_ptr != NULL && mmgsdi_status == MMGSDI_SUCCESS)
  {
    /*free valid record list and record list ptr if exists*/
    (void)mmgsdi_eons_opl_pnn_delete_file_cache(session_id, file_enum);

    /*copy length of record, number of records and set cache init state*/
    opl_pnn_cache_ptr->init = MMGSDI_CACHE_NOT_INIT;
    opl_pnn_cache_ptr->record_ef_info.rec_len = len_of_record;
    opl_pnn_cache_ptr->record_ef_info.num_of_rec = num_of_records;
  }
  return mmgsdi_status;
} /* mmgsdi_eons_opl_pnn_update_record_ef_info */


/*===========================================================================
FUNCTION MMGSDI_EONS_POPULATE_FROM_PERSISTENT_CACHE

DESCRIPTION
  Function to populate OPL PNN records from persistent cache

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_populate_from_persistent_cache(
  mmgsdi_session_id_type        session_id,
  mmgsdi_slot_id_enum_type      slot_id,
  mmgsdi_file_enum_type         file_enum
)
{
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr   = NULL;
  mmgsdi_data_type                   cache_data;
  uint16                             i                   = 0;
  mmgsdi_data_type                   invalid_rec_data;
  mmgsdi_data_type                   valid_rec_data;
  mmgsdi_data_type                  *record_list_ptr     = NULL;
  mmgsdi_data_type                   empty_rec_buf;
  uint8                              total_rec           = 0;
  mmgsdi_len_type                    rec_len             = 0;

  memset(&empty_rec_buf, 0, sizeof(mmgsdi_data_type));
  memset(&invalid_rec_data, 0, sizeof(mmgsdi_data_type));
  memset(&valid_rec_data, 0, sizeof(mmgsdi_data_type));

  /* check file enum */
  if(file_enum != MMGSDI_USIM_OPL && file_enum != MMGSDI_USIM_PNN)
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(invalid_rec_data.data_ptr,
                                     sizeof(uint8) * MMGSDI_MAX_REC_NUM);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(valid_rec_data.data_ptr,
                                     sizeof(uint8) * MMGSDI_MAX_REC_NUM);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(empty_rec_buf.data_ptr,
                                     sizeof(uint8) * MMGSDI_MAX_REC_LEN);

  if(invalid_rec_data.data_ptr == NULL ||
     valid_rec_data.data_ptr == NULL ||
     empty_rec_buf.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(invalid_rec_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(valid_rec_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(empty_rec_buf.data_ptr);
    return MMGSDI_ERROR;
  }

  empty_rec_buf.data_len = MMGSDI_MAX_REC_LEN;
  memset(empty_rec_buf.data_ptr, 0xFF, MMGSDI_MAX_REC_LEN);

  /* build the valid / invalid record list */
  for(i = 1; i <= MMGSDI_MAX_REC_NUM; i++)
  {
    memset(&cache_data, 0, sizeof(mmgsdi_data_type));
    mmgsdi_status = mmgsdi_persistent_cache_read(slot_id,
                                                 file_enum,
                                                 i,
                                                 &cache_data.data_len,
                                                 &cache_data.data_ptr);
    if(mmgsdi_status != MMGSDI_SUCCESS ||
       cache_data.data_len == 0 ||
       cache_data.data_ptr == NULL )
    {
      continue;
    }

    if((memcmp(cache_data.data_ptr, empty_rec_buf.data_ptr, cache_data.data_len) == 0))
    {
      invalid_rec_data.data_ptr[invalid_rec_data.data_len] = i;
      invalid_rec_data.data_len++;
    }
    else
    {
      valid_rec_data.data_ptr[valid_rec_data.data_len] = i;
      valid_rec_data.data_len++;
    }
    MMGSDIUTIL_TMC_MEM_FREE(cache_data.data_ptr);
    rec_len = cache_data.data_len;
    total_rec++;
  }
  MMGSDIUTIL_TMC_MEM_FREE(empty_rec_buf.data_ptr);

  /* no record read from persistent cache */
  if(total_rec == 0)
  {
    MMGSDIUTIL_TMC_MEM_FREE(invalid_rec_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(valid_rec_data.data_ptr);
    return mmgsdi_status;
  }

  /* to update the record EF info in OPL-PNN cache with total num of
     rec and rec_len before adding record data for the EF */
  mmgsdi_status = mmgsdi_eons_opl_pnn_update_record_ef_info(session_id,
                                                            file_enum,
                                                            rec_len,
                                                            total_rec);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(invalid_rec_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(valid_rec_data.data_ptr);
    return mmgsdi_status;
  }

  /* use the invalid rec data retrieve from persistent cache to update the valid
     record list in OPL-PNN cache */
  mmgsdi_status = mmgsdi_eons_opl_pnn_update_valid_record_info(session_id,
                                               file_enum,
                                               invalid_rec_data,
                                               MMGSDI_SUCCESS);

  MMGSDIUTIL_TMC_MEM_FREE(invalid_rec_data.data_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(valid_rec_data.data_ptr);
    return mmgsdi_status;
  }

  for(i = 0; i < valid_rec_data.data_len; i++)
  {
    memset(&cache_data, 0, sizeof(mmgsdi_data_type));

    /* read cache_data from presistent cache */
    mmgsdi_status = mmgsdi_persistent_cache_read(slot_id,
                                                 file_enum,
                                                 valid_rec_data.data_ptr[i],
                                                 &cache_data.data_len,
                                                 &cache_data.data_ptr);

    if(mmgsdi_status != MMGSDI_SUCCESS ||
       cache_data.data_len == 0 ||
       cache_data.data_ptr == NULL )
    {
      break;
    }

    /* update record cache */
    mmgsdi_status = mmgsdi_eons_opl_pnn_update_record(session_id,
                                                      file_enum,
                                                      cache_data,
                                                      (mmgsdi_rec_num_type)valid_rec_data.data_ptr[i],
                                                      MMGSDI_CACHE_MAX_ENUM);

    MMGSDIUTIL_TMC_MEM_FREE(cache_data.data_ptr);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }
  }
  MMGSDIUTIL_TMC_MEM_FREE(valid_rec_data.data_ptr);
  return mmgsdi_status;
} /* mmgsdi_eons_populate_from_persistent_cache */


/*==========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_BUILD_PERSISTENT_CACHE_DATA

DESCRIPTION
  Function to build the persistent cache

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
==========================================================================*/
static void mmgsdi_eons_opl_pnn_build_persistent_cache_data (
  mmgsdi_session_id_type          session_id,
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_slot_id_enum_type        slot_id
)
{
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr = NULL;
  mmgsdi_data_type                   empty_data_buf;
  uint8                              i                 = 0;
  uint8                              j                 = 0;
  mmgsdi_file_enum_type              file_enum_list[]  = {MMGSDI_USIM_PNN,
                                                          MMGSDI_USIM_OPL};
  mmgsdi_eons_cache_enum_type        cache_type[]      = {MMGSDI_EONS_CACHE_PNN,
                                                          MMGSDI_EONS_CACHE_OPL};

  /* persistent cache is only available for GW_PRI */
  if(session_type != MMGSDI_GW_PROV_PRI_SESSION ||
     slot_id == MMGSDI_MAX_SLOT_ID_ENUM)
  {
    return;
  }

  /*get cache ptr for EF PNN & EF OPL */
  for(i = 0; i < (sizeof(file_enum_list) / sizeof(file_enum_list[0])); i++)
  {
    mmgsdi_status =  mmgsdi_eons_opl_pnn_get_cache_ptr(session_id,
                                                       cache_type[i],
                                                       &opl_pnn_cache_ptr);

    if(opl_pnn_cache_ptr == NULL || mmgsdi_status != MMGSDI_SUCCESS ||
       opl_pnn_cache_ptr->record_ef_info.rec_len == 0)
    {
      continue;
    }

    memset(&empty_data_buf, 0x00, sizeof(mmgsdi_data_type));
    empty_data_buf.data_len = opl_pnn_cache_ptr->record_ef_info.rec_len;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(empty_data_buf.data_ptr,
                                       empty_data_buf.data_len);

    if(empty_data_buf.data_ptr == NULL)
    {
      return;
    }

    memset(empty_data_buf.data_ptr, 0xFF, empty_data_buf.data_len);
    for(j = 1; j <= opl_pnn_cache_ptr->record_ef_info.num_of_rec; j++)
    {
      (void)mmgsdi_persistent_cache_write(slot_id,
                                          file_enum_list[i],
                                          j,
                                          empty_data_buf.data_len,
                                          empty_data_buf.data_ptr);
    }

    MMGSDIUTIL_TMC_MEM_FREE(empty_data_buf.data_ptr);

    /* populate valid record to persistent cache */
    for(j = 0; j < opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len; j++)
    {
      if((opl_pnn_cache_ptr->record_list_ptr[j].init == MMGSDI_CACHE_INIT) &&
         (opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr[j] ==
          opl_pnn_cache_ptr->record_list_ptr[j].record_number))
      {
        (void)mmgsdi_persistent_cache_write(slot_id,
                                            file_enum_list[i],
                                            opl_pnn_cache_ptr->record_list_ptr[j].record_number,
                                            opl_pnn_cache_ptr->record_ef_info.rec_len,
                                            opl_pnn_cache_ptr->record_list_ptr[j].data_ptr);
      }
    }
  }
} /* mmgsdi_eons_opl_pnn_build_persistent_cache_data */
#endif /* FEATURE_UIM_PERSISTENT_CACHE */


/*===========================================================================
FUNCTION MMGSDI_EONS_PARSE_PNN_DATA

DESCRIPTION
  Function to parse EF-PNN record data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_parse_pnn_data(
  mmgsdi_eons_pnn_record_data_type * pnn_record,
  uint8                            * pnn_data,
  mmgsdi_len_type                    pnn_data_len
)
{
  int32                  short_name_pos      = 0;
  uint16                 additional_info_pos = 0;

  if(pnn_record == NULL || pnn_data == NULL)
  {
    return;
  }

  if(MMGSDI_EONS_PNN_FULL_NAME_IEI_POS < pnn_data_len)
  {
    pnn_record->full_name_identifier =
      pnn_data[MMGSDI_EONS_PNN_FULL_NAME_IEI_POS];
  }
  else
  {
    /*read overflow*/
    UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                            "Accessing data at 0x%x position in pnn data",
                            MMGSDI_EONS_PNN_FULL_NAME_IEI_POS);
    return;
  }

  pnn_record->full_name_length = 0;
  pnn_record->full_name_ptr = NULL;

  if(pnn_record->full_name_identifier == MMGSDI_EONS_PNN_FULL_NAME_IEI_VAL)
  {
    if(MMGSDI_EONS_PNN_NAME_LEN_POS < pnn_data_len)
    {
      pnn_record->full_name_length = pnn_data[MMGSDI_EONS_PNN_NAME_LEN_POS];
    }
    else
    {
      /*read overflow*/
      UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                              "Accessing data at 0x%x position in pnn data",
                              MMGSDI_EONS_PNN_NAME_LEN_POS);
      return;
    }

    if(pnn_record->full_name_length > 0)
    {
      if(MMGSDI_EONS_PNN_NAME_DATA_POS < pnn_data_len)
      {
        pnn_record->full_name_ptr = &pnn_data[MMGSDI_EONS_PNN_NAME_DATA_POS];
      }
      else
      {
        /*read overflow*/
        UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                               "Accessing data at 0x%x position in pnn data",
                               MMGSDI_EONS_PNN_NAME_DATA_POS);
        return;
      }
    }
    else
    {
      pnn_record->full_name_ptr = NULL;
    }
  }

  short_name_pos =
    pnn_data[MMGSDI_EONS_PNN_NAME_LEN_POS] +
    MMGSDI_EONS_PNN_NAME_DATA_POS;

  if(short_name_pos < pnn_data_len)
  {
    pnn_record->short_name_identifier= pnn_data[short_name_pos];
  }
  else
  {
    /*read overflow*/
    UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                            "Accessing data at 0x%x position in pnn data",
                            short_name_pos);
    return;
  }

  pnn_record->short_name_length = 0;
  pnn_record->short_name_ptr = NULL;

  if(pnn_record->short_name_identifier == MMGSDI_EONS_PNN_SHORT_NAME_IEI_VAL)
  {
    if(short_name_pos + MMGSDI_EONS_PNN_NAME_LEN_POS < pnn_data_len)
    {
      pnn_record->short_name_length =
        pnn_data[short_name_pos + MMGSDI_EONS_PNN_NAME_LEN_POS];
    }
    else
    {
      /*read overflow*/
      UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                              "Accessing data at 0x%x position in pnn data",
                              short_name_pos + MMGSDI_EONS_PNN_NAME_LEN_POS);
      return;
    }

    if(pnn_record->short_name_length > 0)
    {
      if(short_name_pos + MMGSDI_EONS_PNN_NAME_DATA_POS < pnn_data_len)
      {
        pnn_record->short_name_ptr =
          &pnn_data[short_name_pos + MMGSDI_EONS_PNN_NAME_DATA_POS];
      }
      else
      {
        /*read overflow*/
        UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                                "Accessing data at 0x%x position in pnn data",
                                short_name_pos + MMGSDI_EONS_PNN_NAME_DATA_POS);
        return;
      }
    }
    else
    {
      pnn_record->short_name_ptr = NULL;
    }
  }

  /* Read additional info, if present */
  additional_info_pos = pnn_record->full_name_length +
                        pnn_record->short_name_length +
                        (MMGSDI_EONS_PNN_NAME_DATA_POS * 2);

  if(additional_info_pos >= pnn_data_len)
  {
    /* Read overflow, possibly PLMN additional info is not present */
    UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                            "Accessing data at 0x%x position in pnn data",
                            additional_info_pos);
    return;
  }

  pnn_record->additional_info_tag = pnn_data[additional_info_pos];

  if(pnn_record->additional_info_tag != MMGSDI_EONS_PNN_ADD_INFO_TAG_VAL)
  {
    pnn_record->additional_info_length = 0;
    pnn_record->additional_info_ptr = NULL;
    return;
  }

  /* Check & update additional info length */
  if(additional_info_pos + MMGSDI_EONS_PNN_ADD_INFO_LEN_POS < pnn_data_len)
  {
    pnn_record->additional_info_length =
      pnn_data[additional_info_pos + MMGSDI_EONS_PNN_ADD_INFO_LEN_POS];
  }
  else
  {
    /* Read overflow */
    UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                            "Accessing data at 0x%x position in pnn data",
                            additional_info_pos + MMGSDI_EONS_PNN_ADD_INFO_LEN_POS);
    return;
  }

  /* Check & update additional info data */
  if(pnn_record->additional_info_length == 0)
  {
    pnn_record->additional_info_ptr = NULL;
    return;
  }

  if(additional_info_pos + MMGSDI_EONS_PNN_ADD_INFO_DATA_POS < pnn_data_len)
  {
    pnn_record->additional_info_ptr =
      &pnn_data[additional_info_pos + MMGSDI_EONS_PNN_ADD_INFO_DATA_POS];
  }
  else
  {
    /* Read overflow */
    UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                            "Accessing data at 0x%x position in pnn data",
                            additional_info_pos + MMGSDI_EONS_PNN_ADD_INFO_DATA_POS);
    return;
  }
}/* mmgsdi_eons_parse_pnn_data */


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_GET_REF_COUNT

DESCRIPTION
  Function to get cache ref count. cache ref count is incremented when caching starts.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_get_ref_count(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  uint32                  *ref_cnt_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr = NULL;
  mmgsdi_eons_cache_enum_type        cache_type        = MMGSDI_EONS_CACHE_MAX;

  if(ref_cnt_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(file_enum == MMGSDI_GSM_OPL||
     file_enum == MMGSDI_USIM_OPL)
  {
    cache_type = MMGSDI_EONS_CACHE_OPL;
  }
  else if(file_enum == MMGSDI_GSM_PNN||
          file_enum == MMGSDI_USIM_PNN)
  {
    cache_type = MMGSDI_EONS_CACHE_PNN;
  }
  else
  {
    return MMGSDI_ERROR;
  }

  /*get cache ptr*/
  mmgsdi_status = mmgsdi_eons_opl_pnn_get_cache_ptr(session_id,
                                                    cache_type,
                                                    &opl_pnn_cache_ptr);

  if(mmgsdi_status == MMGSDI_SUCCESS && opl_pnn_cache_ptr != NULL)
  {
    *ref_cnt_ptr = opl_pnn_cache_ptr->cache_ref_cnt;
  }

  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_get_ref_count */


/*===========================================================================
FUNCTION MMGSDI_EONS_READ_OPL_REC_CB

DESCRIPTION
  Function to be used as callback function while reading OPL/PNN record

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_read_opl_rec_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type  * cnf_ptr)
{
  mmgsdi_return_enum_type           mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_int_app_info_type        * int_app_info_ptr   = NULL;
  mmgsdi_session_info_type        * session_info_ptr   = NULL;
  mmgsdi_file_enum_type             file_enum          = MMGSDI_MAX_FILE_ENUM;
  uint32                            rec_number         = 0;
  uint32                            rec_len            = 0;
  mmgsdi_len_type                   next_rec_len       = 0;
  mmgsdi_rec_num_type               next_valid_rec_num = 0;
  uint32                            cache_ref_cnt      = 0;

  /*return if cnf_ptr is NULL*/
  if(cnf_ptr == NULL)
  {
    return;
  }

  UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS, "opl read record cb status: 0x%x cnf: 0x%x", status, cnf);

  if(cnf == MMGSDI_READ_CNF &&
     cnf_ptr->read_cnf.access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    file_enum = cnf_ptr->read_cnf.access.file.file_enum;

    rec_len = cnf_ptr->read_cnf.read_data.data_len;
    rec_number = cnf_ptr->read_cnf.accessed_rec_num;

    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      cnf_ptr->read_cnf.response_header.session_id,
                      NULL,
                      NULL,
                      &int_app_info_ptr,
                      NULL,
                      &session_info_ptr,
                      NULL);

    if(mmgsdi_status == MMGSDI_SUCCESS &&
       int_app_info_ptr != NULL &&
       session_info_ptr != NULL)
    {
      /*get cache ref count*/
      mmgsdi_status = mmgsdi_eons_opl_pnn_get_ref_count(
                        cnf_ptr->response_header.session_id,
                        file_enum,
                        &cache_ref_cnt);

      /*compare cache ref count with client data*/
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        if(cnf_ptr->read_cnf.response_header.client_data == cache_ref_cnt)
        {
          /*get next valid record number*/
          mmgsdi_status = mmgsdi_eons_opl_pnn_get_next_uncached_valid_record_number(
                            cnf_ptr->read_cnf.response_header.session_id,
                            file_enum,
                            rec_number,
                            &next_valid_rec_num,
                            &next_rec_len);

          if(mmgsdi_status == MMGSDI_SUCCESS)
          {
            if(next_valid_rec_num && next_rec_len)
            {
              /*set cache ref count as client data in read record request*/
              mmgsdi_status = mmgsdi_session_read_record(
                                cnf_ptr->response_header.session_id,
                                cnf_ptr->read_cnf.access,
                                next_valid_rec_num,
                                next_rec_len,
                                mmgsdi_eons_read_opl_rec_cb,
                                cache_ref_cnt);

              UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS, 
                                       "read opl valid record number: 0x%x  status: 0x%x length of record: 0x%x",
                                       next_valid_rec_num, mmgsdi_status, next_rec_len);
            }
            else
            {
#ifdef FEATURE_UIM_PERSISTENT_CACHE
              /* build presistent cache from cache records */
              mmgsdi_eons_opl_pnn_build_persistent_cache_data(cnf_ptr->response_header.session_id,
                                                              session_info_ptr->session_type,
                                                              cnf_ptr->response_header.slot_id);
#endif /* FEATURE_UIM_PERSISTENT_CACHE*/
              /*send opl pnn cache ready event*/
              mmgsdi_eons_send_cache_ready_event(
                cnf_ptr->read_cnf.response_header.session_id);

              mmgsdi_eons_send_event(
                cnf_ptr->read_cnf.response_header.session_id,
                MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE, FALSE);
            }
          }
        }
        else
        {
          UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS, "cache ref counf 0x%x does not match with client data 0x%x",
                        cache_ref_cnt,
                        cnf_ptr->read_cnf.response_header.client_data);
        }
      }
    }
  }

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_eons_send_cache_ready_event(cnf_ptr->response_header.session_id);
  }
}/* mmgsdi_eons_read_opl_rec_cb */


/*===========================================================================
FUNCTION MMGSDI_EONS_READ_PNN_REC_CB

DESCRIPTION
  Function to be used as callback function while reading OPL/PNN record

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_eons_read_pnn_rec_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type  * cnf_ptr)
{
  mmgsdi_return_enum_type           mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_int_app_info_type        * int_app_info_ptr   = NULL;
  mmgsdi_session_info_type        * session_info_ptr   = NULL;
  mmgsdi_file_enum_type             file_enum          = MMGSDI_MAX_FILE_ENUM;
  uint32                            rec_number         = 0;
  uint32                            rec_len            = 0;
  mmgsdi_len_type                   next_rec_len       = 0;
  mmgsdi_rec_num_type               next_valid_rec_num = 0;
  uint32                            cache_ref_cnt      = 0;
  mmgsdi_access_type                access;

  /*return if cnf_ptr is NULL*/
  if(cnf_ptr == NULL)
  {
    return;
  }

  UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS, "pnn read record cb status: 0x%x cnf: 0x%x", status, cnf);

  if(cnf == MMGSDI_READ_CNF &&
     cnf_ptr->read_cnf.access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    file_enum = cnf_ptr->read_cnf.access.file.file_enum;

    UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS, "access method: 0x%x file enum: 0x%x",
                   cnf_ptr->read_cnf.access.access_method, file_enum );

    rec_len = cnf_ptr->read_cnf.read_data.data_len;
    rec_number = cnf_ptr->read_cnf.accessed_rec_num;

    if(rec_len == 0 ||
       cnf_ptr->read_cnf.read_data.data_ptr == NULL)
    {
      UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS, "rec len: 0x%x data ptr: 0x%x",
                  rec_len, cnf_ptr->read_cnf.read_data.data_ptr);
    }

    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      cnf_ptr->read_cnf.response_header.session_id,
                      NULL,
                      NULL,
                      &int_app_info_ptr,
                      NULL,
                      &session_info_ptr,
                      NULL);

    if(mmgsdi_status == MMGSDI_SUCCESS &&
       int_app_info_ptr != NULL &&
       session_info_ptr != NULL)
    {
      /*get cache ref count*/
      mmgsdi_status = mmgsdi_eons_opl_pnn_get_ref_count(
                        cnf_ptr->response_header.session_id,
                        file_enum,
                        &cache_ref_cnt);

      /*compare cache ref count with client data*/
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        if(cnf_ptr->read_cnf.response_header.client_data == cache_ref_cnt)
        {
          /*get next valid record number*/
          mmgsdi_status = mmgsdi_eons_opl_pnn_get_next_uncached_valid_record_number(
                            cnf_ptr->read_cnf.response_header.session_id,
                            file_enum,
                            rec_number,
                            &next_valid_rec_num,
                            &next_rec_len);

          if(mmgsdi_status == MMGSDI_SUCCESS)
          {
            if(next_valid_rec_num && next_rec_len)
            {
              /*set cache ref count as client data in read record request*/
              mmgsdi_status = mmgsdi_session_read_record(
                                cnf_ptr->response_header.session_id,
                                cnf_ptr->read_cnf.access,
                                next_valid_rec_num,
                                next_rec_len,
                                mmgsdi_eons_read_pnn_rec_cb,
                                cache_ref_cnt);

              UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS, "read pnn valid record number: 0x%x  status: 0x%x length of record: 0x%x",
                                       next_valid_rec_num, mmgsdi_status, next_rec_len);
            }
            else
            {
              /*if last valid record of PNN is read, need to cache OPL*/
              UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS, "Valid records for file 0x%x are read, last valid record: 0x%x, start reading OPL records",
                                       file_enum, rec_number);

              if(file_enum == MMGSDI_GSM_PNN)
              {
                file_enum = MMGSDI_GSM_OPL;
              }
              else
              {
                file_enum = MMGSDI_USIM_OPL;
              }
			  
#ifdef FEATURE_UIM_PERSISTENT_CACHE
              if(file_enum == MMGSDI_USIM_OPL)
              {
                  /* check if OPL is found in persistent cache, after successful read from
                     persistent cache, enos event can be sent */

                  mmgsdi_return_enum_type opl_status = MMGSDI_ERROR;
                  opl_status = mmgsdi_eons_populate_from_persistent_cache(cnf_ptr->read_cnf.response_header.session_id,
                                                                          cnf_ptr->read_cnf.response_header.slot_id,
                                                                          file_enum);
                  if(opl_status == MMGSDI_SUCCESS)
                  {
                    /* build presistent cache from cache records
                       OPL is from persistent cache, but in case PNN was read from card */
                    mmgsdi_eons_opl_pnn_build_persistent_cache_data(
                                        cnf_ptr->read_cnf.response_header.session_id,
                                        session_info_ptr->session_type,
                                        cnf_ptr->read_cnf.response_header.slot_id);
                    /*send OPL-PNN cache ready event*/
                    mmgsdi_eons_send_cache_ready_event(
                      cnf_ptr->read_cnf.response_header.session_id);

                    /*Send EONS event */
                    mmgsdi_eons_send_event(
                      cnf_ptr->read_cnf.response_header.session_id,
                      MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE, FALSE);
                    return;
                  }
              }
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

              /*get next valid record for EF-OPL*/
              mmgsdi_status = mmgsdi_eons_opl_pnn_get_first_uncached_valid_record_number(
                                cnf_ptr->read_cnf.response_header.session_id,
                                file_enum,
                                &next_valid_rec_num,
                                &next_rec_len);

              if(mmgsdi_status == MMGSDI_SUCCESS)
              {
                if(next_valid_rec_num && next_rec_len)
                {
                  /*increase opl cache ref count*/
                  mmgsdi_status = mmgsdi_eons_opl_pnn_increase_and_get_ref_cnt(
                                    cnf_ptr->response_header.session_id,
                                    file_enum,
                                    &cache_ref_cnt);

                  if(mmgsdi_status == MMGSDI_SUCCESS)
                  {
                    mmgsdi_memscpy(&access,
                                   sizeof(access),
                                   &cnf_ptr->read_cnf.access,
                                   sizeof(mmgsdi_access_type));

                    access.file.file_enum = file_enum;

                    /*set cache ref count as client data in read record request*/
                    mmgsdi_status = mmgsdi_session_read_record(
                                      cnf_ptr->response_header.session_id,
                                      access,
                                      next_valid_rec_num,
                                      next_rec_len,
                                      mmgsdi_eons_read_opl_rec_cb,
                                      cache_ref_cnt);

                    UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS, "read opl valid record number: 0x%x  status: 0x%x length of record: 0x%x",
                                             next_valid_rec_num,
                                             mmgsdi_status,
                                             next_rec_len);
                  }
                }
                else
                {
                  /*Caching of OPL-PNN files is done*/
                  UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS, "valid records for file 0x%x are read",
                                 file_enum);

#ifdef FEATURE_UIM_PERSISTENT_CACHE
                  /* build presistent cache from cache records */
                  mmgsdi_eons_opl_pnn_build_persistent_cache_data(
                                      cnf_ptr->read_cnf.response_header.session_id,
                                      session_info_ptr->session_type,
                                      cnf_ptr->read_cnf.response_header.slot_id);
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

                  /*send OPL-PNN cache ready event*/
                  mmgsdi_eons_send_cache_ready_event(
                    cnf_ptr->read_cnf.response_header.session_id);

                  /*Send EONS event */
                  mmgsdi_eons_send_event(
                    cnf_ptr->read_cnf.response_header.session_id,
                    MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE, FALSE);
                }
              }
            }
          }
        }
        else
        {
          UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS, "cache ref counf 0x%x does not match with client data 0x%x",
                        cache_ref_cnt,
                        cnf_ptr->read_cnf.response_header.client_data);
        }
      }
    }
  }
  else
  {
    UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS, "cnf type 0x%x or access method is invalid",
                  cnf, cnf_ptr->read_cnf.access.access_method);
  }

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_eons_send_cache_ready_event(cnf_ptr->response_header.session_id);
  }
}/* mmgsdi_eons_read_pnn_rec_cb */


/*===========================================================================
FUNCTION MMGSDI_EONS_GET_ATTR_AND_SEARCH_INVALID_PATTERN

DESCRIPTION
  Function to get file attr and search invalid pattern in EF to identify valid
  records

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_get_attr_and_search_invalid_pattern(
  mmgsdi_session_id_type session_id,
  mmgsdi_file_enum_type  file_enum
)
{
  mmgsdi_slot_id_enum_type            slot_id               = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_client_id_type               client_id             = MMGSDI_CLIENT_ID_ZERO;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_ERROR;
  mmgsdi_session_info_type           *session_info_ptr      = NULL;
  mmgsdi_session_type_enum_type       session_type          = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_get_file_attr_req_type      *get_file_attr_req_ptr = NULL;
  mmgsdi_get_file_attr_cnf_type      *get_file_attr_cnf_ptr = NULL;
  mmgsdi_search_req_type             *search_req_ptr        = NULL;
  mmgsdi_search_cnf_type             *search_cnf_ptr        = NULL;
  mmgsdi_rec_num_type                 num_rec               = 0;
  uint32                              len_of_rec            = 0;
  mmgsdi_data_type                    data;
  mmgsdi_int_app_info_type           *app_info_ptr          = NULL;
  mmgsdi_access_type                  access;

  UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS, "get file attr and search invalid pattern in file enum: 0x%x session id: 0x%x",
                 file_enum, session_id);

  mmgsdi_status = mmgsdi_util_get_client_id(session_id,&client_id);
  if(mmgsdi_status != MMGSDI_SUCCESS || client_id == MMGSDI_CLIENT_ID_ZERO)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    &slot_id,
                    NULL,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_info_ptr == NULL ||
     app_info_ptr == NULL ||
     (session_info_ptr != NULL &&
      (session_info_ptr->session_type != MMGSDI_GW_PROV_PRI_SESSION &&
       session_info_ptr->session_type != MMGSDI_GW_PROV_SEC_SESSION &&
       session_info_ptr->session_type != MMGSDI_GW_PROV_TER_SESSION)))
  {
    return MMGSDI_ERROR;
  }

  session_type = session_info_ptr->session_type;

  mmgsdi_status = mmgsdi_eons_opl_pnn_delete_file_cache(session_id,
                                                        file_enum);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file_enum;

  /* Check status of EF in cached service table */
  mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
                          client_id,
                          session_id,
                          app_info_ptr->app_data.app_type,
                          &access );

  /* if file access is not allowed in svc table, set cache status as NOT FOUND */
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_eons_opl_pnn_update_file_attr(session_id,
                                          file_enum,
                                          num_rec,
                                          len_of_rec,
                                          MMGSDI_ERROR);

    memset(&data, 0x00, sizeof(mmgsdi_data_type));

    mmgsdi_status = mmgsdi_eons_opl_pnn_update_valid_record_info(
                      session_id,
                      file_enum,
                      data,
                      MMGSDI_ERROR);
    return mmgsdi_status;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(get_file_attr_req_ptr,
    sizeof(mmgsdi_get_file_attr_req_type));
  if (get_file_attr_req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* Determine Number of Records and Len in OPL/PNN */
  get_file_attr_req_ptr->request_header.client_id         = client_id;
  get_file_attr_req_ptr->request_header.session_id        = session_id;
  get_file_attr_req_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_req_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_req_ptr->request_header.request_len       = sizeof(mmgsdi_get_file_attr_req_type);
  get_file_attr_req_ptr->request_header.payload_len       = 0;
  get_file_attr_req_ptr->request_header.slot_id           = slot_id;
  get_file_attr_req_ptr->request_header.client_data       = 0;
  get_file_attr_req_ptr->request_header.response_cb       = NULL;
  get_file_attr_req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  get_file_attr_req_ptr->access.file.file_enum            = file_enum;
  get_file_attr_req_ptr->int_client_data                  = 0;
  get_file_attr_req_ptr->activate_aid                     = TRUE;
  /* Skip reading EF-ARR for checking security attribtues of OPL/PNN EFs */
  get_file_attr_req_ptr->skip_uicc_arr                    = TRUE;

  switch(file_enum)
  {
    case MMGSDI_USIM_OPL:
    case MMGSDI_USIM_PNN:
      mmgsdi_status = mmgsdi_uim_uicc_select(
                        get_file_attr_req_ptr,
                        TRUE,
                        &get_file_attr_cnf_ptr,
                        TRUE);
      break;
    case MMGSDI_GSM_OPL:
    case MMGSDI_GSM_PNN:
      mmgsdi_status = mmgsdi_uim_icc_select(
                        get_file_attr_req_ptr,
                        TRUE,
                        &get_file_attr_cnf_ptr);
      break;
    default:
      break;
  }

  MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_req_ptr);

  if(get_file_attr_cnf_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(get_file_attr_cnf_ptr->file_attrib.file_type != MMGSDI_LINEAR_FIXED_FILE)
  {
    /* free the content of the cnf before freeing the cnf_ptr itself */
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)get_file_attr_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_cnf_ptr);
    return MMGSDI_ERROR;
  }

  if(mmgsdi_status == MMGSDI_SUCCESS &&
     get_file_attr_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS)
  {
    num_rec = get_file_attr_cnf_ptr->file_attrib.file_info.linear_fixed_file.num_of_rec;
    len_of_rec = get_file_attr_cnf_ptr->file_attrib.file_info.linear_fixed_file.rec_len;
  }

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = get_file_attr_cnf_ptr->response_header.mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_eons_opl_pnn_update_file_attr(session_id,
                                        file_enum,
                                        num_rec,
                                        len_of_rec,
                                        mmgsdi_status);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)get_file_attr_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_cnf_ptr);
    return mmgsdi_status;
  }

  /* free the content of the cnf before freeing the cnf_ptr itself */
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)get_file_attr_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(get_file_attr_cnf_ptr);

  if(num_rec != 0 && len_of_rec != 0)
  {
    /*search invaid pattern in file*/
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(search_req_ptr,
      sizeof(mmgsdi_search_req_type));

    /* memory allocation failed for search req ptr */
    if (search_req_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    /* Determine record numbers with invalid pattern (empty record)  in OPL/PNN */
    search_req_ptr->request_header.client_id         = client_id;
    search_req_ptr->request_header.session_id        = session_id;
    search_req_ptr->request_header.request_type      = MMGSDI_SEARCH_REQ;
    search_req_ptr->request_header.orig_request_type = MMGSDI_SEARCH_REQ;
    search_req_ptr->request_header.request_len       = sizeof(mmgsdi_search_req_type);
    search_req_ptr->request_header.payload_len       = 0;
    search_req_ptr->request_header.slot_id           = slot_id;
    search_req_ptr->request_header.client_data       = 0;
    search_req_ptr->request_header.response_cb       = NULL;
    search_req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
    search_req_ptr->access.file.file_enum            = file_enum;

    if(file_enum == MMGSDI_USIM_OPL ||
       file_enum == MMGSDI_USIM_PNN)
    {
      search_req_ptr->search_type =  MMGSDI_UICC_SIMPLE_SEARCH;
    }
    else
    {
      search_req_ptr->search_type =  MMGSDI_ICC_SEARCH;
    }

    search_req_ptr->rec_num = 1;
    search_req_ptr->search_direction = MMGSDI_SEARCH_FORWARD_FROM_REC_NUM;
    search_req_ptr->seek_direction = MMGSDI_SEEK_BEGINNING_FORWARDS;
    search_req_ptr->enhanced_search_offset.offset_data = 0;
    search_req_ptr->enhanced_search_offset.offset_type = MMGSDI_SEARCH_OPTION_NONE;

    search_req_ptr->data.data_len = len_of_rec;
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(search_req_ptr->data.data_ptr,
                                       search_req_ptr->data.data_len);
    if(search_req_ptr->data.data_ptr != NULL)
    {
      memset(search_req_ptr->data.data_ptr, 0xFF, search_req_ptr->data.data_len);
    }
    else
    {
      /* free the content of the cnf before freeing the cnf_ptr itself */
      MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr);
      return MMGSDI_ERROR;
    }

    switch(file_enum)
    {
      case MMGSDI_USIM_OPL:
      case MMGSDI_USIM_PNN:
        mmgsdi_status = mmgsdi_uim_uicc_search_record(
                          search_req_ptr,
                          TRUE,
                          &search_cnf_ptr);
        break;
      case MMGSDI_GSM_OPL:
      case MMGSDI_GSM_PNN:
        mmgsdi_status = mmgsdi_uim_icc_search_record(
                          search_req_ptr,
                          TRUE,
                          &search_cnf_ptr);
        break;
      default:
        break;
    }

    MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr->data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr);

    if(search_cnf_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    if(search_cnf_ptr->access.access_method != MMGSDI_EF_ENUM_ACCESS ||
       search_cnf_ptr->access.file.file_enum != file_enum)
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
    else
    {
      mmgsdi_status = mmgsdi_eons_opl_pnn_update_valid_record_info(
                        session_id,
                        file_enum,
                        search_cnf_ptr->searched_record_nums,
                        search_cnf_ptr->response_header.mmgsdi_status);
    }

    /* free the content of the cnf before freeing the cnf_ptr itself */
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)search_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(search_cnf_ptr);
  }
  else
  {
    memset(&data, 0x00, sizeof(mmgsdi_data_type));
    mmgsdi_status = mmgsdi_eons_opl_pnn_update_valid_record_info(
                      session_id,
                      file_enum,
                      data,
                      MMGSDI_ERROR);
  }

  return mmgsdi_status;
}/*mmgsdi_eons_get_attr_and_search_invalid_pattern*/


/*===========================================================================
FUNCTION MMGSDI_EONS_GET_OPL_PNN_ATTR

DESCRIPTION
  Function to be used to get OPL/PNN file attribute

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_get_opl_pnn_attr(
  mmgsdi_session_id_type      session_id
)
{
  mmgsdi_return_enum_type     mmgsdi_status         = MMGSDI_ERROR;
  mmgsdi_int_app_info_type  * int_app_info_ptr      = NULL;
  mmgsdi_session_info_type  * session_info_ptr      = NULL;
  mmgsdi_slot_id_enum_type    slot_id               = MMGSDI_MAX_SLOT_ID_ENUM;
  uint32                      cache_ref_cnt         = 0;
  mmgsdi_file_enum_type       file_enum_opl         = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_file_enum_type       file_enum             = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_file_enum_type       file_enum_pnn         = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_len_type             len_of_rec            = 0;
  mmgsdi_rec_num_type         rec_num               = 0;
  mmgsdi_access_type          access;
  mmgsdi_callback_type        response_cb_ptr;
  mmgsdi_return_enum_type     pnn_status            = MMGSDI_ERROR;

  /*  check whether session type is either GW PRI or GW SEC*/
  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    &slot_id,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);

  if(mmgsdi_status == MMGSDI_SUCCESS &&
     int_app_info_ptr != NULL &&
     session_info_ptr != NULL &&
     slot_id != MMGSDI_MAX_SLOT_ID_ENUM)
  {
    if((session_info_ptr->session_type == MMGSDI_GW_PROV_PRI_SESSION ||
        session_info_ptr->session_type == MMGSDI_GW_PROV_SEC_SESSION ||
        session_info_ptr->session_type == MMGSDI_GW_PROV_TER_SESSION) &&
       (int_app_info_ptr->app_data.app_type == MMGSDI_APP_SIM||
        int_app_info_ptr->app_data.app_type == MMGSDI_APP_USIM))
    {
      if(int_app_info_ptr->app_data.app_type == MMGSDI_APP_SIM)
      {
        file_enum_pnn = MMGSDI_GSM_PNN;
        file_enum_opl = MMGSDI_GSM_OPL;
      }
      else
      {
        file_enum_pnn = MMGSDI_USIM_PNN;
        file_enum_opl = MMGSDI_USIM_OPL;

#ifdef FEATURE_UIM_PERSISTENT_CACHE
        /* populate eons files from persistent cache */
        pnn_status = mmgsdi_eons_populate_from_persistent_cache(session_id,
                                                                slot_id,
                                                                file_enum_pnn);

       /* if reading PNN failed, then proceed reading PNN from card, OPL file from
          persistent cache will be populated after read PNN is completed in
          mmgsdi_eons_read_pnn_rec_cb*/
        if(pnn_status == MMGSDI_SUCCESS)
        {
          /* start reading OPL from persistent cache, after read sucessful, can proceed
             with sending eons event. */
          mmgsdi_status = mmgsdi_eons_populate_from_persistent_cache(session_id,
                                                                     slot_id,
                                                                     file_enum_opl);
          if(mmgsdi_status == MMGSDI_SUCCESS)
          {
            /*send OPL-PNN cache ready event*/
            mmgsdi_eons_send_cache_ready_event(session_id);
            /*Send EONS event */
            mmgsdi_eons_send_event(session_id,
                                   MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE,
                                   FALSE);
            return MMGSDI_SUCCESS;
          }
        }
#endif /* FEATURE_UIM_PERSISTENT_CACHE */
      }

      if(pnn_status != MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_eons_get_attr_and_search_invalid_pattern(
                          session_id,
                          file_enum_pnn);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          mmgsdi_eons_send_cache_ready_event(session_id);
          return mmgsdi_status;
        }
      }

      mmgsdi_status = mmgsdi_eons_get_attr_and_search_invalid_pattern(
                        session_id,
                        file_enum_opl);

      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        /*File attributes and valid records for both files have been retrieved,
          start reading valid PNN records. Get next valid record number for PNN*/

        mmgsdi_status = mmgsdi_eons_opl_pnn_get_first_uncached_valid_record_number(
                          session_id,
                          file_enum_pnn,
                          &rec_num,
                          &len_of_rec);

        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          /*Check if next valid record number for EF-PNN is zero. if it is zero,
            get next valid record number for EF-OPL*/
          if(rec_num == 0 || len_of_rec == 0)
          {
            /*get next valid record number for EF-OPL*/
            mmgsdi_status = mmgsdi_eons_opl_pnn_get_first_uncached_valid_record_number(
                              session_id,
                              file_enum_opl,
                              &rec_num,
                              &len_of_rec);

            if(mmgsdi_status == MMGSDI_SUCCESS)
            {
              file_enum = file_enum_opl;
            }
          }
          else
          {
            file_enum = file_enum_pnn;
          }

          /*if next valid record is not zero, start reading records else send
            cache completion event*/
          if(rec_num && len_of_rec && mmgsdi_status == MMGSDI_SUCCESS)
          {
            /*increase cache ref count*/
            mmgsdi_status = mmgsdi_eons_opl_pnn_increase_and_get_ref_cnt(
                              session_id,
                              file_enum,
                              &cache_ref_cnt);

            if(mmgsdi_status == MMGSDI_SUCCESS)
            {
              access.access_method = MMGSDI_EF_ENUM_ACCESS;
              access.file.file_enum = file_enum;

              /*set cache ref count as client data in read record request*/
              if(file_enum == file_enum_pnn)
              {
                response_cb_ptr = mmgsdi_eons_read_pnn_rec_cb;
              }
              else
              {
                response_cb_ptr = mmgsdi_eons_read_opl_rec_cb;
              }

              mmgsdi_status = mmgsdi_session_read_record(
                                session_id,
                                access,
                                rec_num,
                                len_of_rec,
                                response_cb_ptr,
                                cache_ref_cnt);

              UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS, "read valid record number: 0x%x  status: 0x%x file enum: 0x%x",
                             rec_num, mmgsdi_status, file_enum);
            }
          }
          else
          {
            mmgsdi_status = MMGSDI_ERROR;
          }
        }
      }
    }
    else
    {
      mmgsdi_status = MMGSDI_ERROR;
      UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS, "Invalid session type: 0x%x",
                    session_info_ptr->session_type );
    }
  }
  else
  {
    mmgsdi_status = MMGSDI_ERROR;
    UIM_SELECTIVE_MSG_ERR_3(UIM_LOG_EONS, "get session app info status: 0x%x init app info ptr: 0x%x session info ptr: 0x%x",
                  mmgsdi_status, int_app_info_ptr, session_info_ptr);
  }

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_eons_send_cache_ready_event(session_id);
  }
  return mmgsdi_status;
}/* mmgsdi_eons_get_opl_pnn_attr */


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_INCREASE_REF_COUNT

DESCRIPTION
  Function to increase cache ref count. cache ref count is incremented when caching starts.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_increase_and_get_ref_cnt(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  uint32                  *ref_cnt_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr = NULL;
  mmgsdi_eons_cache_enum_type        cache_type        = MMGSDI_EONS_CACHE_MAX;

  if(file_enum == MMGSDI_GSM_OPL||
     file_enum == MMGSDI_USIM_OPL)
  {
    cache_type = MMGSDI_EONS_CACHE_OPL;
  }
  else if(file_enum == MMGSDI_GSM_PNN||
          file_enum == MMGSDI_USIM_PNN)
  {
    cache_type = MMGSDI_EONS_CACHE_PNN;
  }
  else
  {
    return MMGSDI_ERROR;
  }

  /*get cache ptr*/
  mmgsdi_status = mmgsdi_eons_opl_pnn_get_cache_ptr(session_id,
                                                    cache_type,
                                                    &opl_pnn_cache_ptr);

  if(mmgsdi_status == MMGSDI_SUCCESS && opl_pnn_cache_ptr != NULL)
  {
    opl_pnn_cache_ptr->cache_ref_cnt++;

    if(ref_cnt_ptr != NULL)
    {
      *ref_cnt_ptr = opl_pnn_cache_ptr->cache_ref_cnt;
    }

    UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS,
                             "after increment cache ref count is 0x%x file 0x%x",
                             opl_pnn_cache_ptr->cache_ref_cnt, file_enum);
  }
  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_increase_and_get_ref_cnt */


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_READ_RECORD

DESCRIPTION
  Function to read record from OPL-PNN cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_read_record(
  mmgsdi_session_id_type       session_id,
  mmgsdi_file_enum_type        file_enum,
  mmgsdi_len_type              data_len,
  mmgsdi_rec_num_type          rec_num,
  mmgsdi_data_type            *record_data_ptr,
  mmgsdi_cache_init_enum_type *cache_state_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr = NULL;
  uint32                             temp_data_len     = 0;
  int32                              loop_cnt          = 0;
  mmgsdi_eons_cache_enum_type        cache_type        = MMGSDI_EONS_CACHE_MAX;

  UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                          "read record number 0x%x from file cache 0x%x  rec length: 0x%x",
                          rec_num, file_enum, data_len);

  if(cache_state_ptr != NULL)
  {
    *cache_state_ptr = MMGSDI_CACHE_MAX_ENUM;
  }

  if(file_enum == MMGSDI_GSM_OPL||
     file_enum == MMGSDI_USIM_OPL)
  {
    cache_type = MMGSDI_EONS_CACHE_OPL;
  }
  else if(file_enum == MMGSDI_GSM_PNN||
          file_enum == MMGSDI_USIM_PNN)
  {
    cache_type = MMGSDI_EONS_CACHE_PNN;
  }
  else
  {
    return MMGSDI_ERROR;
  }

  /*get cache ptr*/
  mmgsdi_status =  mmgsdi_eons_opl_pnn_get_cache_ptr(session_id, cache_type,
                                                     &opl_pnn_cache_ptr);

  if(opl_pnn_cache_ptr != NULL && mmgsdi_status == MMGSDI_SUCCESS)
  {
    if(opl_pnn_cache_ptr->init == MMGSDI_CACHE_NOT_FOUND)
    {
      /*return as file was not found during caching*/
      if(cache_state_ptr != NULL)
      {
        *cache_state_ptr = MMGSDI_CACHE_NOT_FOUND;
      }
      return MMGSDI_NOT_FOUND;
    }

    if(opl_pnn_cache_ptr->init == MMGSDI_CACHE_INIT)
    {
      if(rec_num <= opl_pnn_cache_ptr->record_ef_info.num_of_rec)
      {
        /*check if record is valid or not*/
        if(opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr != NULL)
        {
          for(loop_cnt = 0;
              loop_cnt < opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len;
              loop_cnt++)
          {
            if(opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr[loop_cnt] == rec_num)
            {
              break;
            }
          }
        }

        if(loop_cnt !=
           opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len &&
           opl_pnn_cache_ptr->record_list_ptr != NULL)
        {
          if(opl_pnn_cache_ptr->record_list_ptr[loop_cnt].init == MMGSDI_CACHE_NOT_FOUND)
          {
            /*return as record was not found during caching*/
            if(cache_state_ptr != NULL)
            {
              *cache_state_ptr = MMGSDI_CACHE_NOT_FOUND;
            }
            return MMGSDI_NOT_FOUND;
          }

          if(opl_pnn_cache_ptr->record_list_ptr[loop_cnt].init == MMGSDI_CACHE_INIT &&
             opl_pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr != NULL)
          {
            /*copy data from valid index, calculate the actual record len */

            /* set the length of the data to be retrieved */
            if(data_len == 0)
            {
              /*set to the whole record length */
              record_data_ptr->data_len =
                opl_pnn_cache_ptr->record_ef_info.rec_len;
            }
            else
            {
              record_data_ptr->data_len = data_len;
            }

            temp_data_len = int32touint32(record_data_ptr->data_len);
            MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(record_data_ptr->data_ptr,
                                               temp_data_len);
            if (record_data_ptr->data_ptr == NULL)
            {
              return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
            }
            /* copy the data from the cache */
            mmgsdi_memscpy((void *)record_data_ptr->data_ptr,
              temp_data_len,
              opl_pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr,
              temp_data_len);

            /* set the status to SUCCESS as cache read is good */
            mmgsdi_status =  MMGSDI_SUCCESS;
          }
          else
          {
            /*record not initialized*/
            UIM_SELECTIVE_MSG_ERR_3(UIM_LOG_EONS,
                                    "record init status 0x%x, record data ptr 0x%x at rec num 0x%x",
                                    opl_pnn_cache_ptr->record_list_ptr[loop_cnt].init,
                                    opl_pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr,
                                    rec_num);

            mmgsdi_status =  MMGSDI_ERROR;
          }
        }
        else
        {
          /*record number not among valid records, create invalid data buffer*/
          record_data_ptr->data_len =
            opl_pnn_cache_ptr->record_ef_info.rec_len;

          temp_data_len = int32touint32(record_data_ptr->data_len);
          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(record_data_ptr->data_ptr,
                                             temp_data_len);
          if (record_data_ptr->data_ptr == NULL)
          {
            return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
          }
          memset(record_data_ptr->data_ptr, 0xFF, temp_data_len);

          /* set the status to SUCCESS*/
          mmgsdi_status = MMGSDI_SUCCESS;
        }
      }
      else
      {
        UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS,
                                "requested record number 0x%x is greater than total records 0x%x",
                                rec_num,
                                opl_pnn_cache_ptr->record_ef_info.num_of_rec);

        mmgsdi_status = MMGSDI_ERROR;
      }
    }
    else
    {
      UIM_SELECTIVE_MSG_ERR_3(UIM_LOG_EONS,
                              "cache init status 0x%x, record list ptr 0x%x, valid list data ptr 0x%x",
                              opl_pnn_cache_ptr->init,
                              opl_pnn_cache_ptr->record_list_ptr,
                              opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr);

      mmgsdi_status = MMGSDI_ERROR;
    }
  }
  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_read_record */


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_DELETE_FILE_CACHE

DESCRIPTION
  Function to delete OPL/PNN cache associated with GW PRI or GW SEC session

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_delete_file_cache(
  mmgsdi_session_id_type  session_id,
  mmgsdi_file_enum_type   file_enum
)
{
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr = NULL;
  int32                              loop_cnt          = 0;
  mmgsdi_eons_cache_enum_type        cache_type        = MMGSDI_EONS_CACHE_MAX;

  if(file_enum == MMGSDI_GSM_OPL||
     file_enum == MMGSDI_USIM_OPL)
  {
    cache_type = MMGSDI_EONS_CACHE_OPL;
  }
  else if(file_enum == MMGSDI_GSM_PNN||
          file_enum == MMGSDI_USIM_PNN)
  {
    cache_type = MMGSDI_EONS_CACHE_PNN;
  }
  else
  {
    return MMGSDI_ERROR;
  }

  /*get cache ptr*/
  mmgsdi_status =  mmgsdi_eons_opl_pnn_get_cache_ptr(session_id, cache_type,
                                                     &opl_pnn_cache_ptr);

  if(opl_pnn_cache_ptr != NULL && mmgsdi_status == MMGSDI_SUCCESS)
  {
    /*free all allocated record ptr and make cache init state as NOT_FOUND*/
    if(opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len > 0 &&
       opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr != NULL)
    {
          MMGSDIUTIL_TMC_MEM_FREE(
        opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr);

      if(opl_pnn_cache_ptr->record_list_ptr != NULL)
      {
        /*loop to free all member of reocrd list pointer*/
        for(loop_cnt = 0;
            loop_cnt < opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len;
            loop_cnt++)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            opl_pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr);
        }
        MMGSDIUTIL_TMC_MEM_FREE(opl_pnn_cache_ptr->record_list_ptr);
      }
    }

    /*reset different cache params*/
    opl_pnn_cache_ptr->init = MMGSDI_CACHE_NOT_INIT;
    opl_pnn_cache_ptr->record_ef_info.num_of_rec = 0;
    opl_pnn_cache_ptr->record_ef_info.rec_len = 0;
    opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len = 0;
  }
  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_delete_file_cache */


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_DELETE_CACHE_ALL

DESCRIPTION
  Function to delete OPL-PNN cache associated with GW PRI or GW SEC session

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_delete_cache_all(
  mmgsdi_session_id_type  session_id
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  mmgsdi_status = mmgsdi_eons_opl_pnn_delete_file_cache(session_id,
                                                        MMGSDI_USIM_OPL);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                            "Delete OPL cache for session id: 0x%x failed",
                            session_id );
  }

  mmgsdi_status = mmgsdi_eons_opl_pnn_delete_file_cache(session_id,
                                                        MMGSDI_USIM_PNN);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                            "Delete PNN cache for session id: 0x%x failed",
                            session_id );
  }
  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_delete_cache_all */


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_GET_FIRST_UNCACHED_VALID_RECORD_NUMBER

DESCRIPTION
  Function to get first uncached valid record number and record length while caching OPL-PNN.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_get_first_uncached_valid_record_number(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  mmgsdi_rec_num_type     *next_valid_rec_num_ptr,
  mmgsdi_len_type         *next_valid_rec_len_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr  = NULL;
  mmgsdi_len_type                    last_valid_rec_pos = 0;
  int32                              loop_cnt           = 0;
  mmgsdi_eons_cache_enum_type        cache_type         = MMGSDI_EONS_CACHE_MAX;

  UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                           "get first uncached valid record number for file 0x%x",
                           file_enum);

  if(next_valid_rec_num_ptr == NULL||
     next_valid_rec_len_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(file_enum == MMGSDI_GSM_OPL||
     file_enum == MMGSDI_USIM_OPL)
  {
    cache_type = MMGSDI_EONS_CACHE_OPL;
  }
  else if(file_enum == MMGSDI_GSM_PNN||
          file_enum == MMGSDI_USIM_PNN)
  {
    cache_type = MMGSDI_EONS_CACHE_PNN;
  }
  else
  {
    return MMGSDI_ERROR;
  }

  /*get cache ptr*/
  mmgsdi_status =  mmgsdi_eons_opl_pnn_get_cache_ptr(session_id,
                                                     cache_type,
                                                     &opl_pnn_cache_ptr);

  if(opl_pnn_cache_ptr != NULL && mmgsdi_status == MMGSDI_SUCCESS)
  {
    UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS,
                             "num of total records: 0x%x, valid records 0x%x",
                             opl_pnn_cache_ptr->record_ef_info.num_of_rec,
                             opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len);

    if(opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len == 0 ||
       opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr == NULL)
    {
      *next_valid_rec_num_ptr = 0;
      *next_valid_rec_len_ptr = 0;
      return mmgsdi_status;
    }

    last_valid_rec_pos =
      opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len - 1;

    for(loop_cnt = 0; loop_cnt <= last_valid_rec_pos; loop_cnt++)
    {
      /*check if next valid record is already cached*/
      if(opl_pnn_cache_ptr->record_list_ptr[loop_cnt].init == MMGSDI_CACHE_NOT_INIT &&
         opl_pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr == NULL)
      {
        /*read next valid record*/
        *next_valid_rec_len_ptr = opl_pnn_cache_ptr->record_ef_info.rec_len;

        *next_valid_rec_num_ptr =
          opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr[loop_cnt];

        UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                                 "next valid record number : 0x%x",
                                 *next_valid_rec_num_ptr);
        break;
      }
    }
  }
  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_get_first_uncached_valid_record_number */


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_GET_NEXT_UNCACHED_VALID_RECORD_NUMBER

DESCRIPTION
  Function to get next valid record number and record length while caching OPL-PNN.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_get_next_uncached_valid_record_number(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  mmgsdi_rec_num_type      current_rec_number,
  mmgsdi_rec_num_type     *next_valid_rec_num_ptr,
  mmgsdi_len_type         *next_valid_rec_len_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr  = NULL;
  mmgsdi_len_type                    last_valid_rec_pos = 0;
  int32                              loop_cnt           = 0;
  mmgsdi_eons_cache_enum_type        cache_type         = MMGSDI_EONS_CACHE_MAX;

  UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS,
                           "get next valid record number for file 0x%x  current record number: 0x%x",
                           file_enum, current_rec_number);

  if(next_valid_rec_num_ptr == NULL||
     next_valid_rec_len_ptr == NULL||
     current_rec_number     == 0)
  {
    return MMGSDI_ERROR;
  }

  if(file_enum == MMGSDI_GSM_OPL||
     file_enum == MMGSDI_USIM_OPL)
  {
    cache_type = MMGSDI_EONS_CACHE_OPL;
  }
  else if(file_enum == MMGSDI_GSM_PNN||
          file_enum == MMGSDI_USIM_PNN)
  {
    cache_type = MMGSDI_EONS_CACHE_PNN;
  }
  else
  {
    return MMGSDI_ERROR;
  }

  /*get cache ptr*/
  mmgsdi_status =  mmgsdi_eons_opl_pnn_get_cache_ptr(session_id,
                                                     cache_type,
                                                     &opl_pnn_cache_ptr);

  if(opl_pnn_cache_ptr != NULL && mmgsdi_status == MMGSDI_SUCCESS)
  {
    if(opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len == 0 ||
       opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr == NULL)
    {
      *next_valid_rec_num_ptr = 0;
      *next_valid_rec_len_ptr = 0;
      return mmgsdi_status;
    }

    last_valid_rec_pos =
      opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len - 1;

    if(current_rec_number <
       opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr[last_valid_rec_pos])
    {
      /*search for position of current record in valid record list*/
      for(loop_cnt = 0; loop_cnt < last_valid_rec_pos; loop_cnt++)
      {
        if(current_rec_number ==
           opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr[loop_cnt])
        {
          loop_cnt++;
          break;
        }
      }
    }
    else
    {
      return mmgsdi_status;
    }

    for(; loop_cnt <= last_valid_rec_pos; loop_cnt++)
    {
      /*check if next valid record is already cached*/
      if(opl_pnn_cache_ptr->record_list_ptr[loop_cnt].init == MMGSDI_CACHE_NOT_INIT &&
         opl_pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr == NULL)
      {
        /*read next valid record*/
        *next_valid_rec_len_ptr = opl_pnn_cache_ptr->record_ef_info.rec_len;

        *next_valid_rec_num_ptr =
          opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr[loop_cnt];

        break;
      }
    }
  }
  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_get_next_uncached_valid_record_number */


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_UPDATE_RECORD

DESCRIPTION
  Function to update record data in OPL-PNN cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_update_record(
  mmgsdi_session_id_type      session_id,
  mmgsdi_file_enum_type       file_enum,
  mmgsdi_data_type            data,
  mmgsdi_rec_num_type         rec_num,
  mmgsdi_cache_init_enum_type cache_state
)
{
  mmgsdi_return_enum_type           mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_cache_record_ef_data_type *opl_pnn_cache_ptr = NULL;
  int32                             loop_cnt          = 0;
  mmgsdi_eons_cache_enum_type       cache_type        = MMGSDI_EONS_CACHE_MAX;

  UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                           "update record for file 0x%x  record number: 0x%x cache state: 0x%x",
                           file_enum, rec_num, cache_state);

  if(file_enum == MMGSDI_GSM_OPL||
     file_enum == MMGSDI_USIM_OPL)
  {
    cache_type = MMGSDI_EONS_CACHE_OPL;
  }
  else if(file_enum == MMGSDI_GSM_PNN||
          file_enum == MMGSDI_USIM_PNN)
  {
    cache_type = MMGSDI_EONS_CACHE_PNN;
  }
  else
  {
    return MMGSDI_ERROR;
  }

  /*get cache ptr*/
  mmgsdi_status =  mmgsdi_eons_opl_pnn_get_cache_ptr(session_id, cache_type,
                                                     &opl_pnn_cache_ptr);

  if(opl_pnn_cache_ptr != NULL && mmgsdi_status == MMGSDI_SUCCESS)
  {
    /*only update record when cache is in INIT state*/
    if(opl_pnn_cache_ptr->init == MMGSDI_CACHE_INIT)
    {
      /*check if record's length is equal to length retrieved in file attributes*/
      if(opl_pnn_cache_ptr->record_ef_info.rec_len == data.data_len)
      {
        if(opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr == NULL ||
           opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len == 0)
        {
          return MMGSDI_ERROR;
        }

        for(loop_cnt = 0;
            loop_cnt < opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len;
            loop_cnt++)
        {
          /*compare record number with valid record list's record numbers*/
          if(opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr[loop_cnt] == rec_num)
          {
            /*check if record is not already cached*/
            if(opl_pnn_cache_ptr->record_list_ptr[loop_cnt].init == MMGSDI_CACHE_NOT_INIT)
            {
              if(cache_state == MMGSDI_CACHE_NOT_FOUND)
              {
                opl_pnn_cache_ptr->record_list_ptr[loop_cnt].init
                  = MMGSDI_CACHE_NOT_FOUND;
                return MMGSDI_SUCCESS;
              }

              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
                opl_pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr);

              MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
                opl_pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr,
                data.data_len);

              if(opl_pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr != NULL)
              {
                mmgsdi_memscpy(opl_pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr,
                               data.data_len,
                               data.data_ptr,
                               data.data_len);
                opl_pnn_cache_ptr->record_list_ptr[loop_cnt].init = MMGSDI_CACHE_INIT;
                opl_pnn_cache_ptr->record_list_ptr[loop_cnt].record_number = rec_num;
              }
              else
              {
                mmgsdi_status = MMGSDI_ERROR;
              }
            }
            else
            {
              /*record is already cached*/
              UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS,
                                       "record number 0x%x init state is 0x%x",
                                       rec_num,
                                       opl_pnn_cache_ptr->record_list_ptr[loop_cnt].init);
            }
            break;
          }
        }
      }
      else
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
    }
    else
    {
      mmgsdi_status = MMGSDI_ERROR;
      UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS,
                              "file cache init state is 0x%x",
                              opl_pnn_cache_ptr->init);
    }
  }
  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_update_record */


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_UPDATE_VALID_RECORD_INFO

DESCRIPTION
  Function to update valid record info in OPL-PNN cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_update_valid_record_info(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  mmgsdi_data_type         invalid_record_data,
  mmgsdi_return_enum_type  status
)
{
  mmgsdi_return_enum_type            mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr    = NULL;
  uint32                             buffer_size          = 0;
  int32                              loop_cnt             = 0;
  int32                              loop_cnt_2           = 0;
  uint32                             num_of_valid_records = 0;
  int32                              invalid_record_index = 0;
  uint32                             valid_record_index   = 0;
  mmgsdi_eons_cache_enum_type        cache_type           = MMGSDI_EONS_CACHE_MAX;
  uint8                              temp                 = 0;
  mmgsdi_len_type                    num_of_invalid_rec   = 0;

  UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                           "update valid record info for file 0x%x num of invalid records: 0x%x status :0x%x",
                           file_enum, invalid_record_data.data_len, status);

  if(file_enum == MMGSDI_GSM_OPL||
     file_enum == MMGSDI_USIM_OPL)
  {
    cache_type = MMGSDI_EONS_CACHE_OPL;
  }
  else if(file_enum == MMGSDI_GSM_PNN||
          file_enum == MMGSDI_USIM_PNN)
  {
    cache_type = MMGSDI_EONS_CACHE_PNN;
  }
  else
  {
    UIM_SELECTIVE_MSG_ERR_0(UIM_LOG_EONS, "Invalid file enum type");
    return MMGSDI_ERROR;
  }

  /*get cache ptr*/
  mmgsdi_status =  mmgsdi_eons_opl_pnn_get_cache_ptr(session_id, cache_type,
                                                     &opl_pnn_cache_ptr);

  if(opl_pnn_cache_ptr != NULL && mmgsdi_status == MMGSDI_SUCCESS)
  {
    opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len = 0;
    opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr = NULL;

    if(status == MMGSDI_SUCCESS)
    {
      num_of_invalid_rec = invalid_record_data.data_len;
    }

    opl_pnn_cache_ptr->init = MMGSDI_CACHE_INIT;

    UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS,
                             "total number of records: 0x%x, record length 0x%x",
                             opl_pnn_cache_ptr->record_ef_info.num_of_rec,
                             opl_pnn_cache_ptr->record_ef_info.rec_len);

    /*if number of invalid records are less than number of total records*/
    if(num_of_invalid_rec <= opl_pnn_cache_ptr->record_ef_info.num_of_rec)
    {
      /*calculate total number of valid records*/
      num_of_valid_records = opl_pnn_cache_ptr->record_ef_info.num_of_rec -
                             num_of_invalid_rec;

      opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_len =
        num_of_valid_records;

      /*do memory allocation to valid record list data ptr which carries valid
        record numbers*/
      if(num_of_valid_records > 0)
      {
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr,
          (sizeof(uint8) * num_of_valid_records));

        if(opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr == NULL)
        {
          mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
        else
        {
          /*set invalid record numbers in ascending order*/
          if(num_of_invalid_rec > 0 &&
             invalid_record_data.data_ptr!= NULL)
          {
            for(loop_cnt = 0; loop_cnt < invalid_record_data.data_len; loop_cnt++)
            {
              for(loop_cnt_2 = loop_cnt + 1; loop_cnt_2 < invalid_record_data.data_len; loop_cnt_2++)
              {
                if(invalid_record_data.data_ptr[loop_cnt] > invalid_record_data.data_ptr[loop_cnt_2])
                {
                  temp = invalid_record_data.data_ptr[loop_cnt];
                  invalid_record_data.data_ptr[loop_cnt] =
                    invalid_record_data.data_ptr[loop_cnt_2];
                  invalid_record_data.data_ptr[loop_cnt_2] = temp;
                }
              }
            }
          }

          /*copy valid record numbers in valid record list */
          for(loop_cnt = 0;
              loop_cnt < opl_pnn_cache_ptr->record_ef_info.num_of_rec;
              loop_cnt++)
          {
            if(invalid_record_index < num_of_invalid_rec &&
               invalid_record_data.data_ptr!= NULL)
            {
              if(invalid_record_data.data_ptr[invalid_record_index] == loop_cnt + 1)
              {
                invalid_record_index++;
                continue;
              }
            }
            if(valid_record_index < num_of_valid_records)
            {
              opl_pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr[valid_record_index] =
                (uint8)(loop_cnt + 1);
              valid_record_index++;
            }
          }
        }

        /*calculate buffer size to carry all valid records info*/
        buffer_size = sizeof(mmgsdi_cache_record_data_type) *num_of_valid_records;

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(opl_pnn_cache_ptr->record_list_ptr,
                                           buffer_size);

        if(opl_pnn_cache_ptr->record_list_ptr == NULL)
        {
          mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
      }
    }
    else
    {
      opl_pnn_cache_ptr->init = MMGSDI_CACHE_NOT_FOUND;
      UIM_SELECTIVE_MSG_ERR_0(UIM_LOG_EONS,
                              "number of invalid records is greater than total number of records");
    }
  }
  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_update_valid_record_info */


/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_UPDATE_FILE_ATTR

DESCRIPTION
  Function to update file attribute in OPL-PNN cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_update_file_attr(
  mmgsdi_session_id_type        session_id,
  mmgsdi_file_enum_type         file_enum,
  mmgsdi_rec_num_type           num_of_records,
  uint32                        len_of_record,
  mmgsdi_return_enum_type       status
)
{
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_cache_record_ef_data_type  *opl_pnn_cache_ptr = NULL;
  mmgsdi_eons_cache_enum_type        cache_type        = MMGSDI_EONS_CACHE_MAX;

  UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                           "update file attr for file 0x%x len of record: 0x%x  num of records: 0x%x",
                           file_enum, len_of_record, num_of_records);

  if(file_enum == MMGSDI_GSM_OPL||
     file_enum == MMGSDI_USIM_OPL)
  {
    cache_type = MMGSDI_EONS_CACHE_OPL;
  }
  else if(file_enum == MMGSDI_GSM_PNN||
          file_enum == MMGSDI_USIM_PNN)
  {
    cache_type = MMGSDI_EONS_CACHE_PNN;
  }
  else
  {
    UIM_SELECTIVE_MSG_ERR_0(UIM_LOG_EONS, "Invalid file enum type");
    return MMGSDI_ERROR;
  }

  /*get cache ptr*/
  mmgsdi_status =  mmgsdi_eons_opl_pnn_get_cache_ptr(session_id, cache_type,
                                                     &opl_pnn_cache_ptr);

  if(opl_pnn_cache_ptr != NULL && mmgsdi_status == MMGSDI_SUCCESS)
  {
    /*free valid record list and record list ptr if exists*/
    (void)mmgsdi_eons_opl_pnn_delete_file_cache(session_id, file_enum);

    /*copy length of record, number of records and set cache init state*/
    if(status != MMGSDI_SUCCESS)
    {
      opl_pnn_cache_ptr->init = MMGSDI_CACHE_NOT_FOUND;
      return mmgsdi_status;
    }

    opl_pnn_cache_ptr->init = MMGSDI_CACHE_NOT_INIT;
    opl_pnn_cache_ptr->record_ef_info.rec_len = len_of_record;
    opl_pnn_cache_ptr->record_ef_info.num_of_rec = num_of_records;
  }
  return mmgsdi_status;
}/* mmgsdi_eons_opl_pnn_update_file_attr  */


/*===========================================================================
FUNCTION MMGSDI_EONS_SEND_CACHE_READY_EVENT

DESCRIPTION
  Function to send EONS cache ready event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_send_cache_ready_event(
  mmgsdi_session_id_type        session_id
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type      session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_int_app_info_type         * int_app_info_ptr = NULL;
  mmgsdi_slot_id_enum_type           slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_cache_record_ef_data_type * opl_cache_ptr   = NULL;
  mmgsdi_cache_record_ef_data_type * pnn_cache_ptr   = NULL;

  UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "Send EONS cache ready event");

  mmgsdi_status = mmgsdi_util_get_session_type(
                     session_id,
                     &session_type,
                     NULL);

  /*Send eons event only when session type is GW PRI or GW SEC*/
  if(MMGSDI_SUCCESS != mmgsdi_status ||
     (session_type != MMGSDI_GW_PROV_PRI_SESSION &&
      session_type != MMGSDI_GW_PROV_SEC_SESSION &&
      session_type != MMGSDI_GW_PROV_TER_SESSION))
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    &slot_id,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    NULL,
                    NULL);

  if(mmgsdi_status == MMGSDI_SUCCESS && int_app_info_ptr != NULL)
  {
    mmgsdi_eons_opl_pnn_cache_ready_evt_info_type * cache_ready_info_ptr = NULL;
    mmgsdi_event_data_type                        * evt_info_ptr  = NULL;

    /*send eons event to all sessions of session_type*/
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                       sizeof(mmgsdi_event_data_type));
    if(evt_info_ptr == NULL)
    {
      return;
    }

    evt_info_ptr->evt              = MMGSDI_EONS_OPL_PNN_CACHE_READY_EVT;
    cache_ready_info_ptr           = &(evt_info_ptr->data.eons_opl_pnn_cache_ready);

    switch(session_type)
    {
      case MMGSDI_GW_PROV_PRI_SESSION:
        opl_cache_ptr = &mmgsdi_pri_gw_opl_cache;
        pnn_cache_ptr = &mmgsdi_pri_gw_pnn_cache;
        break;
      case MMGSDI_GW_PROV_SEC_SESSION:
        opl_cache_ptr = &mmgsdi_sec_gw_opl_cache;
        pnn_cache_ptr = &mmgsdi_sec_gw_pnn_cache;
        break;
      case MMGSDI_GW_PROV_TER_SESSION:
        opl_cache_ptr = &mmgsdi_ter_gw_opl_cache;
        pnn_cache_ptr = &mmgsdi_ter_gw_pnn_cache;
        break;
      default:
        UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS, "Invalid session type: 0x%x", session_type);
        return;
    }

    if(opl_cache_ptr->init == MMGSDI_CACHE_INIT)
    {
      /*Copy total number of OPL records */
      cache_ready_info_ptr->opl_info.num_of_rec = opl_cache_ptr->record_ef_info.num_of_rec;

      /*Copy length OPL record */
      cache_ready_info_ptr->opl_info.rec_len = opl_cache_ptr->record_ef_info.rec_len;

      /*Copy valid OPL record list*/
      if(opl_cache_ptr->record_ef_info.valid_record_list.data_len > 0)
      {
        /*Copy number of valid OPL records*/
        cache_ready_info_ptr->opl_info.valid_record_list.data_len =
          opl_cache_ptr->record_ef_info.valid_record_list.data_len;

        /*Memory allocation for valid OPL record list ptr*/
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          cache_ready_info_ptr->opl_info.valid_record_list.data_ptr,
          cache_ready_info_ptr->opl_info.valid_record_list.data_len);

        if(cache_ready_info_ptr->opl_info.valid_record_list.data_ptr == NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(evt_info_ptr);
          return;
        }

        /*Copy valid OPL record list*/
        mmgsdi_memscpy(cache_ready_info_ptr->opl_info.valid_record_list.data_ptr,
                       cache_ready_info_ptr->opl_info.valid_record_list.data_len,
                       opl_cache_ptr->record_ef_info.valid_record_list.data_ptr,
                       cache_ready_info_ptr->opl_info.valid_record_list.data_len);
      }
      else
      {
        cache_ready_info_ptr->opl_info.valid_record_list.data_ptr = NULL;
        cache_ready_info_ptr->opl_info.valid_record_list.data_len = 0;
      }
    }

    if(pnn_cache_ptr->init == MMGSDI_CACHE_INIT)
    {
      /*Copy total number of PNN records */
      cache_ready_info_ptr->pnn_info.num_of_rec = pnn_cache_ptr->record_ef_info.num_of_rec;

      /*Copy length PNN record */
      cache_ready_info_ptr->pnn_info.rec_len = pnn_cache_ptr->record_ef_info.rec_len;

      /*Copy valid OPL record list*/
      if(pnn_cache_ptr->record_ef_info.valid_record_list.data_len > 0)
      {
        /*Copy number of valid PNN records*/
        cache_ready_info_ptr->pnn_info.valid_record_list.data_len =
          pnn_cache_ptr->record_ef_info.valid_record_list.data_len;

        /*Memory allocation for valid PNN record list ptr*/
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          cache_ready_info_ptr->pnn_info.valid_record_list.data_ptr,
          cache_ready_info_ptr->pnn_info.valid_record_list.data_len);

        if(cache_ready_info_ptr->pnn_info.valid_record_list.data_ptr == NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            cache_ready_info_ptr->opl_info.valid_record_list.data_ptr);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(evt_info_ptr);
          return;
        }

        /*Copy valid PNN record list*/
        mmgsdi_memscpy(cache_ready_info_ptr->pnn_info.valid_record_list.data_ptr,
                       cache_ready_info_ptr->pnn_info.valid_record_list.data_len,
                       pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr,
                       cache_ready_info_ptr->pnn_info.valid_record_list.data_len);
      }
      else
      {
        cache_ready_info_ptr->pnn_info.valid_record_list.data_ptr = NULL;
        cache_ready_info_ptr->pnn_info.valid_record_list.data_len = 0;
      }
    }

    /*Notify sessions for eons cache ready event*/
    mmgsdi_evt_notify_session(evt_info_ptr, slot_id, int_app_info_ptr, TRUE);

    mmgsdi_evt_free_data(evt_info_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
  }
}/* mmgsdi_eons_send_cache_ready_event */


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_PNN_ONS

DESCRIPTION
  Function to get operator name string from pnn record

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
static boolean mmgsdi_eons_get_pnn_ons(
  uint8                                     record_number,
  const mmgsdi_cache_record_ef_data_type  * pnn_cache_ptr,
  mmgsdi_plmn_info_type                   * plmn_info_ptr)
{
  boolean                          is_ons_retrieved   = FALSE;
  int32                            loop_cnt           = 0;
  mmgsdi_len_type                  last_valid_rec_pos = 0;
  mmgsdi_eons_pnn_record_data_type pnn_record;

  memset(&pnn_record, 0x00, sizeof(mmgsdi_eons_pnn_record_data_type));

  if(pnn_cache_ptr == NULL || plmn_info_ptr == NULL)
  {
    return FALSE;
  }

  if(pnn_cache_ptr->record_ef_info.valid_record_list.data_len == 0 ||
     pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr == NULL ||
     pnn_cache_ptr->record_list_ptr == NULL)
  {
    UIM_SELECTIVE_MSG_ERR_3(UIM_LOG_EONS,
                            "valid record list data ptr 0x%x data len 0x%x, record list ptr 0x%x",
                            pnn_cache_ptr->record_ef_info.valid_record_list.data_ptr,
                            pnn_cache_ptr->record_ef_info.valid_record_list.data_len,
                            pnn_cache_ptr->record_list_ptr);

    return FALSE;
  }

  /*copy last valid record position in record_list_ptr from valid_record_list,
    as record_list_ptr only contains info of valid records*/
  last_valid_rec_pos = pnn_cache_ptr->record_ef_info.valid_record_list.data_len - 1;

  /*search record list ptr by comparing record number*/
  for(loop_cnt = 0; loop_cnt <= last_valid_rec_pos; loop_cnt++)
  {
    if(record_number ==
       pnn_cache_ptr->record_list_ptr[loop_cnt].record_number &&
       pnn_cache_ptr->record_list_ptr[loop_cnt].init == MMGSDI_CACHE_INIT &&
       pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr != NULL)
    {
      break;
    }
  }

  if(loop_cnt <= last_valid_rec_pos )
  {
    /*valid record is available in cache */
    mmgsdi_eons_parse_pnn_data(&pnn_record,
                               pnn_cache_ptr->record_list_ptr[loop_cnt].data_ptr,
                               pnn_cache_ptr->record_ef_info.rec_len);
  }
  else
  {
    UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS,
                            "Record number 0x%x not valid( or not cached), last valid record number 0x%x",
                            record_number,
                            pnn_cache_ptr->record_list_ptr[last_valid_rec_pos].record_number);
    return FALSE;
  }

  /*check if long name identifier has valid value*/
  if((pnn_record.full_name_identifier ==
      MMGSDI_EONS_PNN_FULL_NAME_IEI_VAL) &&
     (pnn_record.full_name_length > 0)   &&
     (pnn_record.full_name_ptr != NULL))
  {
    uint8 encoding = pnn_record.full_name_ptr[MMGSDI_EONS_PNN_NAME_INFO_BYTE_POS] &
                     MMGSDI_EONS_PNN_DATA_ENCODING_MASK;

    /*copy long name country initials*/
    if (pnn_record.full_name_ptr[MMGSDI_EONS_PNN_NAME_INFO_BYTE_POS] &
        MMGSDI_EONS_PNN_DATA_CI_MASK)
    {
      plmn_info_ptr->plmn_long_name.plmn_name_ci = TRUE;
    }
    else
    {
      plmn_info_ptr->plmn_long_name.plmn_name_ci = FALSE;
    }

    /*copy long name spare bits*/
    plmn_info_ptr->plmn_long_name.plmn_name_spare_bits =
      pnn_record.full_name_ptr[MMGSDI_EONS_PNN_NAME_INFO_BYTE_POS] &
      MMGSDI_EONS_PNN_DATA_SPARE_BITS_MASK;

    /*check if encoding type is valid*/
    if(encoding == MMGSDI_EONS_PNN_ENC_GSM_DEF || encoding == MMGSDI_EONS_PNN_ENC_UCS2)
    {
      /*copy long name encoding*/
      if(encoding == MMGSDI_EONS_PNN_ENC_GSM_DEF)
      {
        plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding =
          MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_PACKED;
      }
      else if(encoding == MMGSDI_EONS_PNN_ENC_UCS2)
      {
        plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding =
          MMGSDI_EONS_ENC_UCS2;
      }

      /*copy long name length*/
      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len =
        pnn_record.full_name_length - 1;

      if(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len > 0)
      {
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
          plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len);

        if(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr != NULL)
        {
          /*copy long name data buffer*/
          mmgsdi_memscpy(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
                         plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len,
                         &(pnn_record.full_name_ptr[MMGSDI_EONS_PNN_NAME_BUF_POS]),
                         plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len);
          is_ons_retrieved = TRUE;
        }
        else
        {
          plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len = 0;
        }
      }
    }
    else
    {
      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len = 0;
      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr = NULL;
    }

    UIM_SELECTIVE_MSG_HIGH_5(UIM_LOG_EONS,
                             "plmn full name country initial: 0x%x spare bits: 0x%x encoding type: 0x%x length: 0x%x buff ptr: 0x%x",
                             plmn_info_ptr->plmn_long_name.plmn_name_ci,
                             plmn_info_ptr->plmn_long_name.plmn_name_spare_bits,
                             plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding,
                             plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len,
                             plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr);

  }
  else
  {
    UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "full name identifier does not match");
  }

  /*check if short name identifier has valid value*/
  if((pnn_record.short_name_identifier ==
      MMGSDI_EONS_PNN_SHORT_NAME_IEI_VAL) &&
      (pnn_record.short_name_length > 0)  &&
      (pnn_record.short_name_ptr != NULL))
  {
    uint8 encoding = pnn_record.short_name_ptr[MMGSDI_EONS_PNN_NAME_INFO_BYTE_POS] &
                     MMGSDI_EONS_PNN_DATA_ENCODING_MASK;

    /*copy short name country initials*/
    if (pnn_record.short_name_ptr[MMGSDI_EONS_PNN_NAME_INFO_BYTE_POS]&
        MMGSDI_EONS_PNN_DATA_CI_MASK)
    {
      plmn_info_ptr->plmn_short_name.plmn_name_ci = TRUE;
    }
    else
    {
      plmn_info_ptr->plmn_short_name.plmn_name_ci = FALSE;
    }

    /*copy short name spare bits*/
    plmn_info_ptr->plmn_short_name.plmn_name_spare_bits =
      pnn_record.short_name_ptr[MMGSDI_EONS_PNN_NAME_INFO_BYTE_POS] &
      MMGSDI_EONS_PNN_DATA_SPARE_BITS_MASK;

    /*check if encoding type is valid*/
    if(encoding == MMGSDI_EONS_PNN_ENC_GSM_DEF || encoding == MMGSDI_EONS_PNN_ENC_UCS2)
    {
      /*copy short name encoding*/
      if(encoding == MMGSDI_EONS_PNN_ENC_GSM_DEF)
      {
        plmn_info_ptr->plmn_short_name.plmn_name.eons_encoding =
          MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_PACKED;
      }
      else if(encoding == MMGSDI_EONS_PNN_ENC_UCS2)
      {
        plmn_info_ptr->plmn_short_name.plmn_name.eons_encoding =
          MMGSDI_EONS_ENC_UCS2;
      }

      /*copy short name length*/
      plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len =
        pnn_record.short_name_length - 1;

      if(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len > 0)
      {
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
          plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len);

        if(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr != NULL)
        {
          /*copy short name data buffer*/
          mmgsdi_memscpy(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
                         plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len,
                         &(pnn_record.short_name_ptr[MMGSDI_EONS_PNN_NAME_BUF_POS]),
                         plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len);
          is_ons_retrieved = TRUE;
        }
        else
        {
          plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len = 0;
        }
      }
    }
    else
    {
      plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len = 0;
      plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr = NULL;
    }

    UIM_SELECTIVE_MSG_HIGH_5(UIM_LOG_EONS,
                             "plmn short name country initial: 0x%x spare bits: 0x%x encoding type: 0x%x length: 0x%x buff ptr: 0x%x",
                             plmn_info_ptr->plmn_short_name.plmn_name_ci,
                             plmn_info_ptr->plmn_short_name.plmn_name_spare_bits,
                             plmn_info_ptr->plmn_short_name.plmn_name.eons_encoding,
                             plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len,
                             plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr);
  }
  else
  {
    UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "short name identifier does not match");
  }

  /* Check if additional info has valid values (only if short & long names
     are successfully retrieved) */
  plmn_info_ptr->plmn_additional_info.info_encoding      = MMGSDI_EONS_ENC_INVALID;
  plmn_info_ptr->plmn_additional_info.info_data.data_len = 0;
  plmn_info_ptr->plmn_additional_info.info_data.data_ptr = NULL;

  if((is_ons_retrieved == TRUE) &&
     (pnn_record.additional_info_tag == MMGSDI_EONS_PNN_ADD_INFO_TAG_VAL) &&
     (pnn_record.additional_info_length > 1) &&
     (pnn_record.additional_info_ptr != NULL))
  {
    uint8 encoding = pnn_record.additional_info_ptr[MMGSDI_EONS_PNN_ADD_INFO_CODING_POS];

    if (encoding == MMGSDI_UCS2_ENCODING_TYPE_1 ||
        encoding == MMGSDI_UCS2_ENCODING_TYPE_2 ||
        encoding == MMGSDI_UCS2_ENCODING_TYPE_3)
    {
      /* Update the additional info */
      mmgsdi_len_type data_len = pnn_record.additional_info_length - 1;
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        plmn_info_ptr->plmn_additional_info.info_data.data_ptr,
        data_len);

      if(plmn_info_ptr->plmn_additional_info.info_data.data_ptr != NULL)
      {
        plmn_info_ptr->plmn_additional_info.info_encoding = MMGSDI_EONS_ENC_UCS2_PACKED;
        plmn_info_ptr->plmn_additional_info.info_data.data_len = data_len;

        mmgsdi_memscpy(plmn_info_ptr->plmn_additional_info.info_data.data_ptr,
                       plmn_info_ptr->plmn_additional_info.info_data.data_len,
                       &(pnn_record.additional_info_ptr[MMGSDI_EONS_PNN_ADD_INFO_CODING_POS+1]),
                       plmn_info_ptr->plmn_additional_info.info_data.data_len);
      }
    }
    else
    {
      UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                               "Unsupported Additional info encoding format: 0x%x",
                               encoding);
    }
  }
  else
  {
    UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "Additional info not present");
  }

  return is_ons_retrieved;
}/* mmgsdi_eons_get_pnn_ons */



/*==========================================================================
FUNCTION: MMGSDI_EONS_COMPARE_WILDCARD_PLMN

DESCRIPTION
  Function to compare two PLMN IDs with considering wildcard
  (wildcard is specific to OPL record's PLMN)

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_compare_wildcard_plmn(
  mmgsdi_plmn_id_type opl_plmn_id,
  mmgsdi_plmn_id_type plmn_id)
{
  uint32 loop_count = 0;

  /*mask the wildcard in OPL plmn id such that it will match with plmn id*/
  for(loop_count = 0; loop_count< MMGSDI_PLMN_ID_SIZE; loop_count++)
  {
    if (MMGSDI_EONS_PLMN_WILD_CARD_VAL == (opl_plmn_id.plmn_id_val[loop_count] & 0x0F))
    {
      opl_plmn_id.plmn_id_val[loop_count] =
        (opl_plmn_id.plmn_id_val[loop_count] & 0xF0) |
        (plmn_id.plmn_id_val[loop_count] & 0x0F);
    }

    if (MMGSDI_EONS_PLMN_WILD_CARD_VAL  ==
        ((opl_plmn_id.plmn_id_val[loop_count] & 0xF0) >> 4))
    {
      opl_plmn_id.plmn_id_val[loop_count] =
        (plmn_id.plmn_id_val[loop_count] & 0xF0) |
        (opl_plmn_id.plmn_id_val[loop_count] & 0x0F);
    }
  }

  if (mmgsdi_eons_compare_sim_plmn_with_nw_plmn(opl_plmn_id, plmn_id))
  {
    return TRUE;
  }

  return FALSE;
}/* mmgsdi_eons_compare_wildcard_plmn */


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_OPL_PNN_ONS

DESCRIPTION
  Function to get operator name string for opl pnn lookup

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_get_opl_pnn_ons(
  mmgsdi_session_id_type             session_id,
  mmgsdi_session_type_enum_type      session_type,
  const mmgsdi_int_app_info_type   * app_info_ptr,
  mmgsdi_eons_call_stack_enum_type   stack_type,
  boolean                            is_plmn_rplmn,
  mmgsdi_plmn_id_type                hplmn_id,
  mmgsdi_plmn_info_type            * plmn_info_ptr
)
{
  uint32                                         loop_cnt           = 0;
  uint8                                          pnn_record_number  = 0;
  boolean                                        is_match_found     = FALSE;
  mmgsdi_cache_record_ef_data_type             * opl_cache_ptr      = NULL;
  mmgsdi_cache_record_ef_data_type             * pnn_cache_ptr      = NULL;
  const mmgsdi_eons_per_stack_rplmn_info_type  * rplmn_info_ptr     = NULL;

  if (app_info_ptr == NULL)
  {
    return FALSE;
  }

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      opl_cache_ptr = &mmgsdi_pri_gw_opl_cache;
      pnn_cache_ptr = &mmgsdi_pri_gw_pnn_cache;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      opl_cache_ptr = &mmgsdi_sec_gw_opl_cache;
      pnn_cache_ptr = &mmgsdi_sec_gw_pnn_cache;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      opl_cache_ptr = &mmgsdi_ter_gw_opl_cache;
      pnn_cache_ptr = &mmgsdi_ter_gw_pnn_cache;
      break;
    default:
      UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS, "Invalid session type: 0x%x", session_type);
      return FALSE;
  }

  UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                           "pnn cache init status 0x%x, valid records 0x%x, total record 0x%x",
                           pnn_cache_ptr->init,
                           pnn_cache_ptr->record_ef_info.valid_record_list.data_len,
                           pnn_cache_ptr->record_ef_info.num_of_rec);

  if(pnn_cache_ptr->init == MMGSDI_CACHE_INIT &&
     pnn_cache_ptr->record_ef_info.valid_record_list.data_len > 0 &&
     pnn_cache_ptr->record_list_ptr != NULL)
  {
    /* Match plmn with OPL record's plmn*/
    if(opl_cache_ptr->init == MMGSDI_CACHE_INIT &&
       opl_cache_ptr->record_ef_info.valid_record_list.data_len > 0&&
       opl_cache_ptr->record_list_ptr != NULL)
    {
      mmgsdi_plmn_id_type opl_plmn_id;
      uint16              lac_start   = 0;
      uint16              lac_end     = 0;

      for(loop_cnt = 0;
          loop_cnt < (uint32)opl_cache_ptr->record_ef_info.valid_record_list.data_len;
          loop_cnt++)
      {
        if(opl_cache_ptr->record_list_ptr[loop_cnt].init == MMGSDI_CACHE_INIT &&
           opl_cache_ptr->record_list_ptr[loop_cnt].data_ptr != NULL &&
           opl_cache_ptr->record_ef_info.rec_len > MMGSDI_EONS_PNN_RECORD_NUM_POS)
        {
          mmgsdi_memscpy((void *)(opl_plmn_id.plmn_id_val),
            sizeof(opl_plmn_id.plmn_id_val),
            (void *)&opl_cache_ptr->record_list_ptr[loop_cnt].data_ptr[MMGSDI_EONS_OPL_RECORD_PLMN_ID_POS],
            MMGSDI_PLMN_ID_SIZE);

          if(TRUE == mmgsdi_eons_compare_wildcard_plmn(
                       opl_plmn_id,
                       plmn_info_ptr->plmn_id))
          {
            /*do LAC comparision only when asked plmn matches with rplmn*/
            if(is_plmn_rplmn)
            {
              if (stack_type == MMGSDI_EONS_FIRST_CALL_STACK)
              {
                rplmn_info_ptr = &(app_info_ptr->eons_info.rplmn_info.first_stack_rplmn);
              }
              else
              {
                rplmn_info_ptr = &(app_info_ptr->eons_info.rplmn_info.sec_stack_rplmn);
              }

              lac_start =
                (opl_cache_ptr->record_list_ptr[loop_cnt].data_ptr[MMGSDI_EONS_LAC_START_UPPER_BYTE_POS]<<8) +
                opl_cache_ptr->record_list_ptr[loop_cnt].data_ptr[MMGSDI_EONS_LAC_START_LOWER_BYTE_POS];

              lac_end =
                (opl_cache_ptr->record_list_ptr[loop_cnt].data_ptr[MMGSDI_EONS_LAC_END_UPPER_BYTE_POS]<<8) +
                opl_cache_ptr->record_list_ptr[loop_cnt].data_ptr[MMGSDI_EONS_LAC_END_LOWER_BYTE_POS];

              /*LAC1 and LAC2 got in opl entry must be the same with LAC associated with rplmn*/
              if ((lac_start <= rplmn_info_ptr->lac) &&
                       (rplmn_info_ptr->lac <= lac_end))
              {
                is_match_found = TRUE;
                pnn_record_number =
                  opl_cache_ptr->record_list_ptr[loop_cnt].data_ptr[MMGSDI_EONS_PNN_RECORD_NUM_POS];

                UIM_SELECTIVE_MSG_HIGH_4(UIM_LOG_EONS,
                                         "LAC start: 0x%x LAC end: 0x%x, rplmn LAC info: 0x%x opl match is found at record number 0x%x",
                                         lac_start, lac_end, rplmn_info_ptr->lac,
                                         opl_cache_ptr->record_list_ptr[loop_cnt].record_number);
                break;
              }
            }
            else
            {
              is_match_found = TRUE;
              pnn_record_number =
                opl_cache_ptr->record_list_ptr[loop_cnt].data_ptr[MMGSDI_EONS_PNN_RECORD_NUM_POS];

              UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                                       "opl match is found at record number 0x%x",
                                       opl_cache_ptr->record_list_ptr[loop_cnt].record_number);
              break;
            }
          }
        }
        }
      }

    /*If the EF-OPL is not present, then the first record in EF-PNN is
      used for the default network name when registered in the HPLMN
      (if the EHPLMN list is not present or is empty) or an EHPLMN
      (if the EHPLMN list is present).*/
    if(opl_cache_ptr->init == MMGSDI_CACHE_INIT &&
       opl_cache_ptr->record_ef_info.num_of_rec == 0)
    {
      if(is_match_found == FALSE)
      {
        /* If plmn is rplmn and in home network, make is_match_found TRUE*/
        if(is_plmn_rplmn)
        {
          if((stack_type == MMGSDI_EONS_FIRST_CALL_STACK &&
              app_info_ptr->eons_info.ss_info.first_stack_ss.roam_status == SYS_ROAM_STATUS_OFF) ||
             (stack_type == MMGSDI_EONS_SEC_CALL_STACK &&
              app_info_ptr->eons_info.ss_info.sec_stack_ss.roam_status == SYS_ROAM_STATUS_OFF))
          {
            is_match_found = TRUE;
            pnn_record_number = 1;
          }
        }/* Compare plmn with hplmn, if matches make is_match_found TRUE*/
        else if(mmgsdi_eons_compare_sim_plmn_with_nw_plmn(hplmn_id,
                  plmn_info_ptr->plmn_id))
        {
          is_match_found = TRUE;
          pnn_record_number = 1;
        }
        else if(app_info_ptr->app_data.app_type == MMGSDI_APP_USIM)
        {
          mmgsdi_data_type                  ehplmn_data;
          mmgsdi_access_type                ehplmn_file_access;
          mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;

          memset(&ehplmn_data, 0x00, sizeof(mmgsdi_data_type));
          ehplmn_file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
          ehplmn_file_access.file.file_enum = MMGSDI_USIM_EHPLMN;

          mmgsdi_status = mmgsdi_util_read_cache_file(
                            session_id, &ehplmn_file_access, &ehplmn_data);

          /* Comare plmn with EHPLMN, if  match is found, get plmn name from
             pnn record 1 */
          for(loop_cnt = 0;
              (mmgsdi_status == MMGSDI_SUCCESS) && (ehplmn_data.data_len > 0) &&
              (loop_cnt < ((uint32)(ehplmn_data.data_len/MMGSDI_PLMN_ID_SIZE)));
              loop_cnt++)
          {
            mmgsdi_plmn_id_type temp_plmn;

            mmgsdi_memscpy(temp_plmn.plmn_id_val,
              sizeof(temp_plmn.plmn_id_val),
              &ehplmn_data.data_ptr[loop_cnt*MMGSDI_PLMN_ID_SIZE],
              MMGSDI_PLMN_ID_SIZE);

            if(mmgsdi_eons_compare_sim_plmn_with_nw_plmn(temp_plmn,
                 plmn_info_ptr->plmn_id))
            {
              is_match_found = TRUE;
              pnn_record_number = 1;
              break;
            }
          }
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(ehplmn_data.data_ptr);
        }
      }
    }

    UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS,
                             "is opl match found: 0x%x, pnn record number: 0x%x",
                             is_match_found, pnn_record_number);

    if(is_match_found && pnn_record_number > 0)
    {
      if(pnn_record_number <= pnn_cache_ptr->record_ef_info.num_of_rec )
      {
        /*copy pnn_list_pptr[pnn_record_index - 1] record data*/
        if(TRUE == mmgsdi_eons_get_pnn_ons(
                     pnn_record_number,
                     pnn_cache_ptr,
                     plmn_info_ptr))
        {
          return TRUE;
        }
      }
      else
      {
        UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                                 "Record number 0x%x not available in pnn cache",
                                 pnn_record_number);
      }
    }
  }
  return FALSE;
} /* mmgsdi_eons_get_opl_pnn_ons */


/*==========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_CACHE_READY

DESCRIPTION
  Function to check when OPL PNN cache is ready or not. It returns TRUE if
  caching is completed or OPL-PNN is not found in card

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_opl_pnn_cache_ready (
  mmgsdi_session_type_enum_type      session_type
)
{
  uint32                            loop_cnt       = 0;
  mmgsdi_cache_record_ef_data_type *opl_cache_ptr  = NULL;
  mmgsdi_cache_record_ef_data_type *pnn_cache_ptr  = NULL;

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      opl_cache_ptr = &mmgsdi_pri_gw_opl_cache;
      pnn_cache_ptr = &mmgsdi_pri_gw_pnn_cache;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      opl_cache_ptr = &mmgsdi_sec_gw_opl_cache;
      pnn_cache_ptr = &mmgsdi_sec_gw_pnn_cache;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      opl_cache_ptr = &mmgsdi_ter_gw_opl_cache;
      pnn_cache_ptr = &mmgsdi_ter_gw_pnn_cache;
      break;
    default:
      UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS, "Invalid session type: 0x%x", session_type);
      return FALSE;
  }

  /* Return TRUE if PNN cache is in NOT FOUND status */
  if(pnn_cache_ptr->init == MMGSDI_CACHE_NOT_FOUND)
  {
    return TRUE;
  }

  /* Return FALSE if PNN/OPL cache is in NOT INIT status, cache not initialized */
  if(pnn_cache_ptr->init ==  MMGSDI_CACHE_NOT_INIT ||
     opl_cache_ptr->init ==  MMGSDI_CACHE_NOT_INIT)
  {
    return FALSE;
  }

  /* Now check if each valid PNN record is cached */
  if(pnn_cache_ptr->record_ef_info.valid_record_list.data_len > 0 &&
     pnn_cache_ptr->record_list_ptr != NULL)
  {
    for(loop_cnt = 0;
        loop_cnt < (uint32)pnn_cache_ptr->record_ef_info.valid_record_list.data_len;
        loop_cnt++)
    {
      if(pnn_cache_ptr->record_list_ptr[loop_cnt].init == MMGSDI_CACHE_NOT_INIT)
      {
        return FALSE;
      }
    }
  }

  /* Return TRUE if OPL cache is in NOT FOUND status */
  if(opl_cache_ptr->init == MMGSDI_CACHE_NOT_FOUND)
  {
    return TRUE;
  }

  /* Now check if each valid OPL record is cached */
  if(opl_cache_ptr->record_ef_info.valid_record_list.data_len > 0 &&
     opl_cache_ptr->record_list_ptr != NULL)
  {
    for(loop_cnt = 0;
        loop_cnt < (uint32)opl_cache_ptr->record_ef_info.valid_record_list.data_len;
        loop_cnt++)
    {
      if(opl_cache_ptr->record_list_ptr[loop_cnt].init == MMGSDI_CACHE_NOT_INIT)
      {
        return FALSE;
      }
    }
  }

  /* We are here, means all valid OPL-PNN records are cached, return TRUE */
  return TRUE;
}/* mmgsdi_eons_opl_pnn_cache_ready */

