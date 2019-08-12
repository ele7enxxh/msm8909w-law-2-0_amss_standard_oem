/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I  P S M  D E F I N I T I O N S

                      A N D   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the power saving mode supporting functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_psm.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/16   dd      Added support for auto reg SMS when ESN is changed
06/13/16   vdc     Fix compilation issue when FEATURE_SIMLOCK is disabled
01/29/16   bcho    Delete Slot, App and EF cache properly when PSM init fails
01/20/16   bcho    Remove compiler warnings
01/08/16   kv      ADF PIN Status & epsloci in case of ust disabled handling 
01/07/16   kv      UIM PSM handling updates
01/07/16   bcho    Create separate copy of LOCI, EPSLOCI and PSLOCI cache
01/06/16   bcho    Reset early init flag when card error occurs
01/06/16   bcho    EF-ECC cache creation from PSM SFS data
12/25/15   bcho    initial version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "uim_msg.h"

#include "queue.h"
#include "rex.h"

#include "mmgsdi_psm.h"
#include "mmgsdi_efs.h"
#include "mmgsdi_sfs.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_session.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_gen.h"
#include "mmgsdi.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_simlock.h"
#include "mmgsdi_file.h"
#include "mmgsdi_card_init.h"

#ifdef FEATURE_MMGSDI_3GPP2
#include "nvruim_p.h"
#endif /* FEATURE_MMGSDI_3GPP2 */

#include "uim_common_efs.h"
#include "gstk_p.h"

/* PSM UICC profile dir and file paths*/
static char *mmgsdi_safe_dir                 = "/safe";
static char *mmgsdi_sfs_dir                  = "/safe/sfs";
static char *mmgsdi_sfs_uim_dir              = "/safe/sfs/uim";
static char *mmgsdi_sfs_uim_mmgsdi_dir       = "/safe/sfs/uim/mmgsdi";
static char *mmgsdi_sfs_psm_uicc_profile     = "/safe/sfs/uim/mmgsdi/psm_uicc_profile_data";

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* PSM Data written into EFS */
typedef PACKED struct PACKED_POST
{
  mmgsdi_protocol_enum_type card_type;
  mmgsdi_iccid_info_type    iccid_data;
  mmgsdi_aid_type           app_data[MMGSDI_MAX_APP_INFO];
}mmgsdi_psm_data_type;

/*=============================================================================
  EF Cache data format

  1. EF data will be copied from address of first_byte_data
  1. Only ALWAYS, NEVER and SINGLE protection method will be populdated.
  2. AND and OR protection would be mapped to SINGLE and first PIN value will be
     populated
=============================================================================*/
typedef PACKED struct PACKED_POST
{
  mmgsdi_file_enum_type          file_enum;
  uint8                          rec_num;
  uint8                          sfi;
  uint8                          num_of_rec;
  mmgsdi_file_security_enum_type read_protection;
  mmgsdi_pin_enum_type           read_pin;
  mmgsdi_file_security_enum_type write_protection;
  mmgsdi_pin_enum_type           write_pin;
  mmgsdi_file_security_enum_type increase_protection;
  mmgsdi_pin_enum_type           increase_pin;
  mmgsdi_len_type                data_len; /*length for data */
  uint8                          first_byte_data; /* Use this pointer to extract the rest of the Data of the EF */
}mmgsdi_psm_ef_data;

/* EPSLOCI, PSLOCI and LOCI data that is used for comparison with card EFs */
typedef struct
{
  uint8 loci_len;
  uint8 loci_data[MMGSDI_LOCI_LEN];
  uint8 psloci_len;
  uint8 psloci_data[MMGSDI_PSLOCI_LEN];
  uint8 epsloci_len;
  uint8 epsloci_data[MMGSDI_EPSLOCI_LEN];
}mmgsdi_psm_local_cache_type;

/* To carry PSM data retrieved fom SFS */
static mmgsdi_psm_data_type        *mmgsdi_psm_data_ptr        = NULL;
static mmgsdi_psm_local_cache_type *mmgsdi_psm_local_cache_ptr = NULL;

/* PSM mode is enabled or not */
static boolean               mmgsdi_psm_enabled  = FALSE;

/* Indicates if PSM early init is done */
static boolean               mmgsdi_psm_early_init_done = FALSE;

/* USIM CACHE ELEMENTS */
static mmgsdi_file_enum_type mmgsdi_psm_usim_cache_enums[] =
{
  MMGSDI_USIM_IMSI,      MMGSDI_USIM_KEYS,      MMGSDI_USIM_KEYSPS, MMGSDI_USIM_HPLMN,    MMGSDI_USIM_ACM_MAX,
  MMGSDI_USIM_UST,       MMGSDI_USIM_EST,       MMGSDI_USIM_ACM,    MMGSDI_USIM_SPN,      MMGSDI_USIM_PUCT,
  MMGSDI_USIM_ACC,       MMGSDI_USIM_LOCI,      MMGSDI_USIM_PSLOCI, MMGSDI_USIM_EMLPP,    MMGSDI_USIM_AAEM,
  MMGSDI_USIM_START_HFN, MMGSDI_USIM_THRESHOLD, MMGSDI_USIM_KC,     MMGSDI_USIM_PLMNWACT, MMGSDI_USIM_OPLMNWACT,
  MMGSDI_USIM_HPLMNWACT, MMGSDI_USIM_RPLMNACT,  MMGSDI_USIM_FPLMN,  MMGSDI_USIM_LI,       MMGSDI_USIM_KCGPRS,
  MMGSDI_USIM_AD,        MMGSDI_USIM_CBMID,     MMGSDI_USIM_EHPLMN,   MMGSDI_USIM_EHPLMNPI, MMGSDI_USIM_LRPLMNSI,
  MMGSDI_USIM_EPSLOCI,   MMGSDI_USIM_EPSNSC,    MMGSDI_USIM_HIDDENKEY,MMGSDI_USIM_SPDI,     MMGSDI_USIM_ECC,
  MMGSDI_USIM_GID1,      MMGSDI_USIM_GID2,      MMGSDI_USIM_NASCONFIG,MMGSDI_USIM_7F66_PROP1_RAT,
  MMGSDI_USIM_7F66_PROP1_SPT_TABLE, MMGSDI_USIM_7F66_PROP1_ACT_HPLMN
};

#ifdef FEATURE_MMGSDI_3GPP2
/* CSIM Cache Elements */
static mmgsdi_file_enum_type mmgsdi_psm_csim_cache_enums[] =
{
  MMGSDI_CSIM_IMSI_M,            MMGSDI_CSIM_IMSI_T,                   MMGSDI_CSIM_CST,                      MMGSDI_CSIM_LANG_IND,
  MMGSDI_CSIM_EST,               MMGSDI_CSIM_AD,                       MMGSDI_CSIM_RUIM_ID,                  MMGSDI_CSIM_SERVICE_PREF,
  MMGSDI_CSIM_ACCOLC,            MMGSDI_CSIM_PRL,                      MMGSDI_CSIM_EPRL,                     MMGSDI_CSIM_MAXIMUM_PRL,
  MMGSDI_CSIM_HIDDEN_KEY,        MMGSDI_CSIM_SPN,                      MMGSDI_CSIM_ECC,                      MMGSDI_CSIM_3GPD_MIPUPP,
  MMGSDI_CSIM_3GPD_SIPUPP,       MMGSDI_CSIM_HRPDUPP,                  MMGSDI_CSIM_SF_EUIM_ID,               MMGSDI_CSIM_NAM_LOCK,
  MMGSDI_CSIM_CDMA_HOME_SID_NID, MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND, MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND, MMGSDI_CSIM_CDMA_SYS_REGN_IND,
  MMGSDI_CSIM_OTAPA_SPC_ENABLE,  MMGSDI_CSIM_3GPD_MIPSP,               MMGSDI_CSIM_3GPD_MIPFLAGS,            MMGSDI_CSIM_CALL_TERM_MODE_PREF,
  MMGSDI_CSIM_3GPD_3GPDOPM,      MMGSDI_CSIM_3GPD_SIPPAPSS
};
#endif /* FEATURE_MMGSDI_3GPP2 */

/*===========================================================================
FUNCTION MMGSDI_PSM_PURGE_DATA

DESCRIPTION
  To purge the data in EFS and to free heap memory occupied by PSM data

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_purge_data(void)
{
  UIM_MSG_HIGH_0("mmgsdi_psm_purge_data");

  /*Free global PSM data ptr if not already freed */
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_local_cache_ptr);

  /* Delete PSM UICC proflle file from SFS */
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  (void)mmgsdi_sfs_rmfile((const char *)mmgsdi_sfs_psm_uicc_profile);
#else
  #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
}/* mmgsdi_psm_purge_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_BUILD_EF_DATA

DESCRIPTION
  To create EF cache from stored PSM data

PARAMETERS
  ef_data_ptr : address of buffer which contains EFs' data and attribute
  ef_data_len : length of buffer passed

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_build_ef_data
(
  uint8                    *ef_data_ptr,
  uint32                    ef_data_len
)
{
  mmgsdi_return_enum_type          mmgsdi_status    = MMGSDI_SUCCESS;

  if(ef_data_ptr == NULL ||
     ef_data_len == 0)
  {
    return MMGSDI_ERROR;
  }

  while(ef_data_len >= sizeof(mmgsdi_psm_ef_data))
  {
    mmgsdi_file_structure_enum_type  file_type        = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
    mmgsdi_file_size_type            file_size        = 0;
    mmgsdi_session_type_enum_type    session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;
    mmgsdi_session_id_type           session_id       = MMGSDI_SESSION_ID_ZERO;
    mmgsdi_app_enum_type             app_type         = MMGSDI_APP_NONE;
    mmgsdi_psm_ef_data               ef_data;
    mmgsdi_access_type               file_access;
    mmgsdi_file_security_access_type security_access;
    mmgsdi_data_type                 file_data;

    memset(&ef_data, 0x00, sizeof(mmgsdi_psm_ef_data));
    memset(&file_access, 0x00, sizeof(mmgsdi_access_type));
    memset(&security_access, 0x00, sizeof(mmgsdi_file_security_access_type));
    memset(&file_data, 0x00, sizeof(mmgsdi_data_type));

    mmgsdi_memscpy(&ef_data, sizeof(mmgsdi_psm_ef_data),
                   ef_data_ptr, sizeof(mmgsdi_psm_ef_data));

    if(ef_data.data_len + sizeof(mmgsdi_psm_ef_data) - sizeof(uint8) > ef_data_len)
    {
      UIM_MSG_ERR_0("buffer data length is psm ef data + data len");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Get session type based on file enum type */
    if(MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(ef_data.file_enum) == MMGSDI_USIM_EF_START ||
       MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(ef_data.file_enum) == MMGSDI_USIM_GSM_ACCESS_DF_START ||
       MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(ef_data.file_enum) == MMGSDI_CINGULAR_USIM_NETWORK_DF_START ||
       MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(ef_data.file_enum) == MMGSDI_CINGULAR_USIM_EF_START)
    {
      session_type = MMGSDI_GW_PROV_PRI_SESSION;
      app_type     = MMGSDI_APP_USIM;
      session_id   = mmgsdi_generic_data_ptr->pri_gw_session_id;
    }
    else if(((ef_data.file_enum >> 8) & 0x00FF) == MMGSDI_CSIM_EF_START)
    {
      session_type = MMGSDI_1X_PROV_PRI_SESSION;
      app_type     = MMGSDI_APP_CSIM;
      session_id   = mmgsdi_generic_data_ptr->pri_1x_session_id;
    }
    else
    {
      UIM_MSG_HIGH_1("Error in parsing for file enum 0x%x ", ef_data.file_enum);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if(ef_data.num_of_rec)
    {
      file_type = MMGSDI_LINEAR_FIXED_FILE;
      file_size = ef_data.num_of_rec * ef_data.data_len;
    }
    else
    {
      file_type = MMGSDI_TRANSPARENT_FILE;
      file_size = ef_data.data_len;
    }

    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = ef_data.file_enum;

    security_access.read.protection_method = ef_data.read_protection;

    /* If protection method is other that "Always" and "Allowed", it would be
       "Single Allowed" because we only store one PIN in PSM data for ease of
       caching */
    if(security_access.read.protection_method != MMGSDI_ALWAYS_ALLOWED &&
       security_access.read.protection_method != MMGSDI_NEVER_ALLOWED)
    {
      security_access.read.num_protection_pin = 1;
      security_access.read.protection_pin_ptr = &ef_data.read_pin;
    }

    security_access.write.protection_method = ef_data.write_protection;
    if(security_access.write.protection_method != MMGSDI_ALWAYS_ALLOWED &&
       security_access.write.protection_method != MMGSDI_NEVER_ALLOWED)
    {
      security_access.write.num_protection_pin = 1;
      security_access.write.protection_pin_ptr = &ef_data.write_pin;
    }

    security_access.increase.protection_method = ef_data.increase_protection;
    if(security_access.increase.protection_method != MMGSDI_ALWAYS_ALLOWED &&
       security_access.increase.protection_method != MMGSDI_NEVER_ALLOWED)
    {
      security_access.increase.num_protection_pin = 1;
      security_access.increase.protection_pin_ptr = &ef_data.increase_pin;
    }

    /* Update attribute cache */
    mmgsdi_status = mmgsdi_cache_attr_write(
                      session_id,
                      MMGSDI_SLOT_1,
                      &file_access,
                      file_type,
                      file_size,
                      ef_data.data_len,
                      ef_data.num_of_rec,
                      ef_data.sfi,
                      &security_access,
                      NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_2("Not able to write attr for EF 0x%x app 0x%x",
                    file_access.file.file_enum, app_type);
      break;
    }

    /* Update EF cache */
    mmgsdi_status = mmgsdi_cache_write_item(ef_data.file_enum,
                                            app_type,
                                            session_type,
                                            ef_data.data_len,
                                            0,
                                            &((mmgsdi_psm_ef_data *)ef_data_ptr)->first_byte_data,
                                            MMGSDI_DATA_FROM_CARD_COMPLETE);

    /* EF-ECC is a linear fixed file. mmgsdi_cache_write_item does not support
       caching of multiple records of record based EFs; it can only cache one record */
    if(ef_data.file_enum == MMGSDI_USIM_ECC)
    {
      file_data.data_len = ef_data.data_len / ef_data.num_of_rec;
      file_data.data_ptr = &((mmgsdi_psm_ef_data *)ef_data_ptr)->first_byte_data;

    /* Cache the first record read so that it can be used later for
       getting ECC record length when sending out the ECC event */
      mmgsdi_status = mmgsdi_cache_add_record(session_id,
                                 file_access,
                                 1,
                                 &file_data);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_0("Unable to cache USIM ECC record no 1");
        break;
      }

      file_data.data_len = ef_data.data_len;

      mmgsdi_status = mmgsdi_cache_create_usim_ecc_cache_psm_mode(
                        session_type,
                        ef_data.num_of_rec,
                        &file_data);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_0("Unable to update num of records in USIM ECC cache");
        break;
      }
    }

    /* Move to address where we store EF data by adding size of
       mmgsdi_psm_ef_data to ef_data_ptr. Note that
       sizeof(uint8) is substracted because first byte of EF data is part of
       structre mmgsdi_psm_ef_data*/
    ef_data_ptr += sizeof(mmgsdi_psm_ef_data) - sizeof(uint8); 
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_2("Not able to write cache for EF 0x%x app 0x%x",
                    file_access.file.file_enum, app_type);
      break;
    }

    /* To Move to address of next file data, add EF len to ef_data_ptr. Remaining
       length is decreased by size of structure and EF data len. Note that
       sizeof(uint8) is added back to remaining length because first byte of
       EF data is already part of structre mmgsdi_psm_ef_data */
    ef_data_len = ef_data_len - ef_data.data_len - sizeof(mmgsdi_psm_ef_data) + sizeof(uint8);
    ef_data_ptr += ef_data.data_len;    
  }

  return mmgsdi_status;
}/* mmgsdi_psm_build_ef_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_BUILD_APP_DATA

DESCRIPTION
  To build application data from stored PSM data

PARAMETERS
  session_id   : session id to build app data
  slot_id      : slot id
  app_data_ptr : Application data

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_build_app_data
(
  mmgsdi_session_id_type    session_id, 
  mmgsdi_aid_type          *app_data_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_session_info_type  *session_info_ptr    = NULL;
  int32                      channel_index       = 0;
  mmgsdi_slot_data_type     *slot_data_ptr       = NULL;
  int32                      index_session       = 0;
  int32                      index_client        = 0;
  uint8                      app_index           = 0;
  mmgsdi_app_pin_info_type **app_pin1_tbl_pptr   = NULL;
  uint8                      app_pin1_tbl_size   = 0;
  uint32                     pin_index           = 0;

  if(app_data_ptr == NULL ||
     (app_data_ptr->app_type != MMGSDI_APP_CSIM &&
      app_data_ptr->app_type != MMGSDI_APP_USIM))
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_0("mmgsdi_psm_build_app_data");

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_client_and_session_index(session_id,
                                                                &index_client,
                                                                &index_session))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_client_id_reg_table[index_client] == NULL)
  {
    return MMGSDI_ERROR;
  }

  session_info_ptr = mmgsdi_client_id_reg_table[index_client]->session_info_ptr[index_session];
  if(session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* App specific EF cache and attribute cache gets populated here */
  do
  {
    mmgsdi_status = mmgsdi_util_get_free_channel_index(&channel_index);
    if((mmgsdi_status != MMGSDI_SUCCESS) ||
       (channel_index >= MMGSDI_MAX_CHANNEL_INFO) ||
       (mmgsdi_channel_info_ptr_table[channel_index] != NULL))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_channel_info_ptr_table[channel_index],
                                       sizeof(mmgsdi_channel_info_type));

    if (mmgsdi_channel_info_ptr_table[channel_index] == NULL)
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      break;
    }

    /* Wipe off any data that exists at the index in the table */
    memset(mmgsdi_channel_info_ptr_table[channel_index], 0x00,
           sizeof(mmgsdi_channel_info_type));

    if(channel_index > 0)
    {
      mmgsdi_channel_info_ptr_table[channel_index]->channel_id = 1;
    }
    else
    {
      mmgsdi_channel_info_ptr_table[channel_index]->channel_id = MMGSDI_DEFAULT_CHANNEL;
    }

    mmgsdi_channel_info_ptr_table[channel_index]->slot_id = MMGSDI_SLOT_1;
    slot_data_ptr->available_logical_channels--;

    mmgsdi_channel_info_ptr_table[channel_index]->num_connections++;

    /* Map the new channel table entry with the session */
    session_info_ptr->channel_info_index = (uint8)channel_index;

    if (MMGSDI_SUCCESS != mmgsdi_session_activate_app_info(session_info_ptr->session_id,
                                                           app_data_ptr->aid))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Set application status */
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_selected =
      TRUE;
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state =
      MMGSDI_APP_STATE_ACTIVATED;
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->perso_state = 
      MMGSDI_SIMLOCK_VERIFIED;
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pre_perso_files_cached = TRUE;

    (void)mmgsdi_cache_create_prov_file_cache_clear_caches(
            session_info_ptr->session_id,
            TRUE);

    /* As we don't have AID select response we can't update ADF attribute and PIN
       info in app_info_ptr */
    app_pin1_tbl_pptr = mmgsdi_util_get_app_pin1_info_tbl_ptr(&app_pin1_tbl_size,
                                                              MMGSDI_SLOT_1);

    if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL &&
       app_pin1_tbl_pptr != NULL)
    {
      /* Indicate that PIN data for the app was not extracted from FCP */
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->fcp_pin_data_absent = TRUE;
      
      /* No app has been selected on the default channel yet so setting key ref
         for PIN1 as MMGSDI_PIN_APPL_1 and PIN2 as MMGSDI_SEC_PIN_APPL_1
         respectively */

      /* Ignoring return type since app pin table has only 8 entries and
         one of them will match MMGSDI_PIN_APPL_1 key ref */
      (void)mmgsdi_util_find_app_pin_index((const mmgsdi_app_pin_info_type **)app_pin1_tbl_pptr,
                                           app_pin1_tbl_size,
                                           MMGSDI_PIN_APPL_1,
                                           &pin_index);
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin1_ptr = app_pin1_tbl_pptr[pin_index];
      if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin1_ptr != NULL)
      {
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin1_ptr->status =
          MMGSDI_PIN_DISABLED;
      }

      memset((void *)&mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin2,
             0x0, sizeof(mmgsdi_app_pin_info_type));
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin2.pin_id = MMGSDI_PIN2;
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin2.key_ref = MMGSDI_SEC_PIN_APPL_1;
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin2.status =
        MMGSDI_PIN_DISABLED;
    }
  }
  while(0);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_app_index_for_app(app_data_ptr,
                                                         MMGSDI_SLOT_1,
                                                         &app_index))
  {
    return MMGSDI_ERROR;
  }

  if(session_info_ptr->session_type == MMGSDI_GW_PROV_PRI_SESSION)
  {
    slot_data_ptr->prov_app_indices[MMGSDI_PRI_GW_PROV_APP_SEL_INDEX] =
      MMGSDI_PROV_INDEX_VALIDITY_MASK | app_index;
  }
  else 
  {
    slot_data_ptr->prov_app_indices[MMGSDI_PRI_1X_PROV_APP_SEL_INDEX] = 
      MMGSDI_PROV_INDEX_VALIDITY_MASK | app_index;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_build_app_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_BUILD_SLOT_DATA

DESCRIPTION
  To build slot data from stored PSM data

PARAMETERS
  psm_data_ptr : stored PSM data pointer
  slot_id      : slot id

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_build_slot_data
(
  mmgsdi_psm_data_type     *psm_data_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_session_id_type            card_session_id   = MMGSDI_SESSION_ID_ZERO;
  mmgsdi_int_app_info_type        **app_info_pptr     = NULL;
  mmgsdi_key_ref_values_enum_type   slot_pin1_key_ref = MMGSDI_PIN_APPL_1;
  uint8                             pin1_index        = 0;  
  uint32                            num_apps          = 0;
  uint8                             index             = 0;
  mmgsdi_data_type                  file_data;
  mmgsdi_access_type                access_info;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters.
     ----------------------------------------------------------------------- */
  if(psm_data_ptr == NULL )
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_0("mmgsdi_psm_build_slot_data");

  memset(&file_data, 0x00, sizeof(mmgsdi_data_type));
  memset(&access_info, 0x00, sizeof(mmgsdi_access_type));

  /* Allocating memory  for Slot data*/
  if(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX] == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX],
                                       sizeof(mmgsdi_slot_data_type));

    if(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX] == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  /* Clear the Slot specific MMGSDI data */
  mmgsdi_util_preinit_mmgsdi(MMGSDI_SESSION_ID_ZERO,
                             MMGSDI_APP_NONE,
                             MMGSDI_REFRESH_RESET,
                             MMGSDI_SLOT_1,
                             FALSE);

  /* --------------------------------------------------------------------------
  Populate card info to MMGSDI slot data
  ----------------------------------------------------------------------- */

  /* Allocating memory for all index in app pin1 table ptr for slot 1 */
  for(pin1_index = 0; pin1_index < MMGSDI_MAX_PIN1_INFO; pin1_index++)
  {
    if(mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][pin1_index] != NULL)
    {
      MMGSDI_MSG_ERROR_2("PIN1 table is not NULL, slot index 0x%x, pin1 index 0x%x",
                         MMGSDI_SLOT_1_INDEX, pin1_index);
      return MMGSDI_ERROR;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][pin1_index],
      sizeof(mmgsdi_app_pin_info_type));

    if(mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][pin1_index] == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][pin1_index]->pin_id  = MMGSDI_PIN1;
    mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][pin1_index]->key_ref = slot_pin1_key_ref++;
  }

  /* Allocating memory for Univ PIN*/
  if(mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX] != NULL)
  {
    MMGSDI_MSG_ERROR_1("Univ PIN table is not NULL, slot index 0x%x",
                       MMGSDI_SLOT_1_INDEX);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX],
    sizeof(mmgsdi_app_pin_info_type));

  if (mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX] == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX]->pin_id  = MMGSDI_UNIVERSAL_PIN;
  mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX]->key_ref = MMGSDI_UNIVERSAL_PIN_KEY;

  mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->mmgsdi_state = MMGSDI_STATE_NOT_INIT;

  /* Copy the card protocol */
  mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->protocol = psm_data_ptr->card_type;

  app_info_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(MMGSDI_SLOT_1);

  /* Assigning app_info_ptr to App_PIN table */
  mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr = app_info_pptr;

  /* Get the Card Slot Session ID corresponding to the slot */
  card_session_id = mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_1_INDEX];

  /* Retrieve the ICCD */
  memset(&access_info, 0x00, sizeof(mmgsdi_access_type));
  access_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  access_info.file.file_enum = MMGSDI_ICCID;

  file_data.data_len = psm_data_ptr->iccid_data.length;
  file_data.data_ptr = psm_data_ptr->iccid_data.data;

  /* Copy the ICCID to the common cache */
  mmgsdi_status = mmgsdi_cache_write(card_session_id,
                                     &access_info,
                                     file_data,
                                     0,
                                     MMGSDI_DATA_FROM_CARD_COMPLETE);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->mmgsdi_rpm_iccid_has_changed = FALSE;

  /* Set UIM proactive to FALSE here, it will be set to TRUE once UICC mode of
     operation is identified */
  uim_set_proactive_uim(FALSE);

  for(index = 0; index < sizeof(psm_data_ptr->app_data) / sizeof(psm_data_ptr->app_data[0]); index++)
  {
    mmgsdi_key_ref_values_enum_type slot_pin2_key_ref = MMGSDI_SEC_PIN_APPL_1;
    if(psm_data_ptr->app_data[index].app_type == MMGSDI_APP_NONE)
    {
      continue;
    }

    mmgsdi_status = mmgsdi_card_init_populate_app_pin_table(
                      MMGSDI_SLOT_1,
                      num_apps,
                      slot_pin2_key_ref);

    /* PIN2 key reference is incremented to point to next PIN2 for
       assigning this to next app */
    slot_pin2_key_ref++;

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }

    mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->app_data.aid.data_len =
      psm_data_ptr->app_data[index].aid.data_len;

    /* Copy the AID into the global data structure which holds the AID data */
    mmgsdi_memscpy(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->app_data.aid.data_ptr,
      sizeof(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->app_data.aid.data_ptr),
      psm_data_ptr->app_data[index].aid.data_ptr,
      psm_data_ptr->app_data[index].aid.data_len);

    mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->app_selected = FALSE;
    mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->app_data.app_type =
      psm_data_ptr->app_data[index].app_type;
    mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->proprietary_app = FALSE;
    num_apps++;
  }  

  /* If there are valid UICC apps found... */
  if(num_apps > 0)
  {
    UIM_MSG_HIGH_1("No. of Apps present in UICC is 0x%x", num_apps);

    mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->card_apps_available |= MMGSDI_USIM_APP_MASK;

#ifdef FEATURE_MMGSDI_3GPP2
    if(mmgsdi_util_get_csim_apps(MMGSDI_SLOT_1))
    {
      UIM_MSG_HIGH_0("CSIM app. present");
      mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->card_apps_available |= MMGSDI_CSIM_APP_MASK;
      nvruim_set_uim_dir_present(NVRUIM_CDMA_SUBSCRIP_PRESENT);
    }
#endif /* FEATURE_MMGSDI_3GPP2 */
  }

  /* Reset the card error condition to UNKNOWN as we got link established
    notification */
  mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->card_error_condition =
    MMGSDI_CARD_ERR_UNKNOWN_ERROR;

 return MMGSDI_SUCCESS;
}/* mmgsdi_psm_build_slot_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_SLOT_APP_EF_DATA_CLEANUP

DESCRIPTION
  To free and reset slot, app and EF cache if Early Init in PSM mode fails 

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_slot_app_ef_data_cleanup(void)
{
  uint8 channel_index = 0;

  /* Clearing prov and common caches */
  mmgsdi_cache_clear_prov_type_cache_alloc(mmgsdi_generic_data_ptr->pri_1x_session_id);
  mmgsdi_cache_clear_prov_type_cache_alloc(mmgsdi_generic_data_ptr->pri_gw_session_id);
  mmgsdi_cache_delete_common_file_cache(MMGSDI_SLOT_1);

  /* Clearing the Common Cache Table Pointers */
  if(mmgsdi_slot1_common_cache_tbl_ptr != NULL)
  {
    mmgsdi_attr_cache_element_type * tmp_attr_element_ptr   = NULL;

    /* Delete all the File attribute cache elements */
    while(mmgsdi_slot1_common_cache_tbl_ptr->item_ptr != NULL)
    {
      tmp_attr_element_ptr = mmgsdi_slot1_common_cache_tbl_ptr->item_ptr->next_ptr;
      mmgsdi_cache_delete_file_attr_cache_item(&mmgsdi_slot1_common_cache_tbl_ptr->item_ptr);
      mmgsdi_slot1_common_cache_tbl_ptr->item_ptr = tmp_attr_element_ptr;
    }
  }

  /* This frees the Channel Info Pointer Table pointers */
  for (channel_index = 0;
       channel_index < sizeof(mmgsdi_channel_info_ptr_table)/
                       sizeof(mmgsdi_channel_info_ptr_table[0]);
       channel_index++)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_channel_info_ptr_table[channel_index]);
  }

  /* Reset all the app related data */
  mmgsdi_util_reset_app_data(FALSE, MMGSDI_SLOT_1);
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]);
}/* mmgsdi_psm_slot_app_ef_data_cleanup */


/*===========================================================================
FUNCTION MMGSDI_PSM_CREATE_LOCAL_CACHE

DESCRIPTION
  To create local cache for EPSLOCI, PSLOCI and LOCI

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_create_local_cache
(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_len_type         file_len      = 0;

  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_local_cache_ptr);
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_psm_local_cache_ptr,
                                     sizeof(mmgsdi_psm_local_cache_type));
  if(mmgsdi_psm_local_cache_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_cache_read_item(MMGSDI_USIM_LOCI,
                                         MMGSDI_APP_USIM,
                                         MMGSDI_GW_PROV_PRI_SESSION,
                                         sizeof(mmgsdi_psm_local_cache_ptr->loci_data),
                                         0,
                                         &file_len,
                                         mmgsdi_psm_local_cache_ptr->loci_data,
                                         NULL);

  mmgsdi_psm_local_cache_ptr->loci_len = (uint8)file_len;
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_psm_local_cache_ptr->loci_len = 0;
  }

  mmgsdi_status = mmgsdi_cache_read_item(MMGSDI_USIM_PSLOCI,
                                         MMGSDI_APP_USIM,
                                         MMGSDI_GW_PROV_PRI_SESSION,
                                         sizeof(mmgsdi_psm_local_cache_ptr->psloci_data),
                                         0,
                                         &file_len,
                                         mmgsdi_psm_local_cache_ptr->psloci_data,
                                         NULL);

  mmgsdi_psm_local_cache_ptr->psloci_len = (uint8)file_len;
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_psm_local_cache_ptr->psloci_len = 0;
  }

  mmgsdi_status = mmgsdi_cache_read_item(MMGSDI_USIM_EPSLOCI,
                                         MMGSDI_APP_USIM,
                                         MMGSDI_GW_PROV_PRI_SESSION,
                                         sizeof(mmgsdi_psm_local_cache_ptr->epsloci_data),
                                         0,
                                         &file_len,
                                         mmgsdi_psm_local_cache_ptr->epsloci_data,
                                         NULL);

  mmgsdi_psm_local_cache_ptr->epsloci_len = (uint8)file_len;
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_psm_local_cache_ptr->epsloci_len = 0;
  }
}/* mmgsdi_psm_create_local_cache */


/*===========================================================================
FUNCTION MMGSDI_PSM_SFS_CACHE_INIT

DESCRIPTION
  To initialize slot and app data from stored PSM data

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_sfs_cache_init(void)
{
  uint32                  total_length                 = 0;
  mmgsdi_return_enum_type mmgsdi_status                = MMGSDI_ERROR;
  uint8*                  mmgsdi_uicc_profile_data_ptr = NULL;
  mmgsdi_aid_type         aid_data;

  memset(&aid_data, 0x00, sizeof(mmgsdi_aid_type));

  do
  {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
      /* Retrieve file size*/
    if(MMGSDI_SUCCESS != mmgsdi_sfs_get_size(
                           &total_length,
                           (const char *)mmgsdi_sfs_psm_uicc_profile))
#else
    #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
    {
      UIM_MSG_ERR_0("failed to get SFS file size");
      return MMGSDI_ERROR;
    }

    if(total_length < sizeof(mmgsdi_psm_data_type))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_uicc_profile_data_ptr,
                                       total_length);

    if(mmgsdi_uicc_profile_data_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      break;
    }

#ifndef FEATURE_UIM_TEST_FRAMEWORK
    if(MMGSDI_SUCCESS != mmgsdi_sfs_read(
                          mmgsdi_uicc_profile_data_ptr,
                          total_length,
                          (const char *)mmgsdi_sfs_psm_uicc_profile))
#else
    #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
    {
      UIM_MSG_ERR_0("failed to read psm_uicc_profile from SFS");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_psm_data_ptr,
                                       sizeof(mmgsdi_psm_data_type));
    if(mmgsdi_psm_data_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      break;
    }

    mmgsdi_memscpy(mmgsdi_psm_data_ptr, sizeof(mmgsdi_psm_data_type),
                   mmgsdi_uicc_profile_data_ptr, sizeof(mmgsdi_psm_data_type));
    
    /* Check slot id and card type in SFS data */
    if(mmgsdi_psm_data_ptr->card_type != MMGSDI_UICC)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Build slot data */
    mmgsdi_status = mmgsdi_psm_build_slot_data(mmgsdi_psm_data_ptr);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* App data present in mmgsdi_psn_data_ptr is packed. so can't copy it as a
       whole */
    aid_data.app_type = mmgsdi_psm_data_ptr->app_data[0].app_type;

    aid_data.aid.data_len = mmgsdi_psm_data_ptr->app_data[0].aid.data_len;
    mmgsdi_memscpy(aid_data.aid.data_ptr, sizeof(aid_data.aid.data_ptr), mmgsdi_psm_data_ptr->app_data[0].aid.data_ptr,
                   mmgsdi_psm_data_ptr->app_data[0].aid.data_len);

    aid_data.label.data_len = mmgsdi_psm_data_ptr->app_data[0].label.data_len;
    mmgsdi_memscpy(aid_data.label.data_ptr, sizeof(aid_data.label.data_ptr), mmgsdi_psm_data_ptr->app_data[0].label.data_ptr,
                   mmgsdi_psm_data_ptr->app_data[0].label.data_len);

    /* Build app data present at index 0*/
    mmgsdi_status = mmgsdi_psm_build_app_data(mmgsdi_generic_data_ptr->pri_gw_session_id, &aid_data);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

#ifdef FEATURE_MMGSDI_3GPP2
    /* Build app data present at index 1*/
    if(mmgsdi_psm_data_ptr->app_data[1].app_type == MMGSDI_APP_CSIM)
    {
      /* App data present in mmgsdi_psn_data_ptr is packed. so can't copy it as a
         whole */
      aid_data.app_type = mmgsdi_psm_data_ptr->app_data[1].app_type;

      aid_data.aid.data_len = mmgsdi_psm_data_ptr->app_data[1].aid.data_len;
      mmgsdi_memscpy(aid_data.aid.data_ptr, sizeof(aid_data.aid.data_ptr), mmgsdi_psm_data_ptr->app_data[1].aid.data_ptr,
                     mmgsdi_psm_data_ptr->app_data[1].aid.data_len);

      aid_data.label.data_len = mmgsdi_psm_data_ptr->app_data[1].label.data_len;
      mmgsdi_memscpy(aid_data.label.data_ptr, sizeof(aid_data.label.data_ptr), mmgsdi_psm_data_ptr->app_data[1].label.data_ptr,
                     mmgsdi_psm_data_ptr->app_data[1].label.data_len);

      mmgsdi_status = mmgsdi_psm_build_app_data(mmgsdi_generic_data_ptr->pri_1x_session_id, &aid_data);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
    }
#endif /* FEATURE_MMGSDI_3GPP2 */

    /* Build EF cache */
    mmgsdi_status = mmgsdi_psm_build_ef_data(
                      mmgsdi_uicc_profile_data_ptr + sizeof(mmgsdi_psm_data_type),
                      total_length - sizeof(mmgsdi_psm_data_type));

    /* Create local copy of LOCI, EPSLOCI and PSLOCI cache; it will be used for
       comparison with card's EFs. Note that global cache of these files can be
       updated by MMGSDI's clients in their context so we need a local copy */
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      mmgsdi_psm_create_local_cache();
    }
  }
  while(0);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
    mmgsdi_psm_slot_app_ef_data_cleanup();
  }

  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_uicc_profile_data_ptr);
  return mmgsdi_status;
}/* mmgsdi_psm_sfs_cache_init */


/*===========================================================================
FUNCTION MMGSDI_PSM_TRIGGER_EVENTS

DESCRIPTION
  To trigger card/session events.

PARAMETERS
  session_id :  session id

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_trigger_session_events(
  mmgsdi_session_id_type session_id)
{
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type           *sel_app_ptr   = NULL;
  mmgsdi_evt_session_notify_type      notify_type;

  /* Send session changed events for USIM and CSIM if app data is valid */
  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                 NULL,
                                                 NULL,
                                                 &sel_app_ptr,
                                                 NULL,
                                                 NULL,
                                                 NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      sel_app_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_1("Session Events for app: 0x%x",sel_app_ptr->app_data.app_type);

  /* Assign session notify type info */
  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = MMGSDI_SLOT_1;
  notify_type.session_id  = session_id;

  mmgsdi_evt_build_and_send_session_changed(
    notify_type, TRUE, (const mmgsdi_int_app_info_type*)sel_app_ptr);

  UIM_MSG_HIGH_0("MMGSDI_PIN1_EVT");

  mmgsdi_evt_build_and_send_ecc(notify_type);

  /* Send all the PINs event */
  if(sel_app_ptr->pin1_ptr)
  {
    mmgsdi_evt_build_and_send_pin(
      notify_type,
      MMGSDI_PIN1,
      sel_app_ptr->pin1_ptr->status,
      (const mmgsdi_int_app_info_type*)sel_app_ptr);
  }

  mmgsdi_evt_build_and_send_pin(
    notify_type,
    MMGSDI_PIN2,
    sel_app_ptr->pin2.status,
    (const mmgsdi_int_app_info_type*)sel_app_ptr);

  if(sel_app_ptr->universal_pin_ptr)
  {
    mmgsdi_evt_build_and_send_pin(
      notify_type,
      MMGSDI_UNIVERSAL_PIN,
      sel_app_ptr->universal_pin_ptr->status,
      (const mmgsdi_int_app_info_type*)sel_app_ptr);
  }

#ifdef FEATURE_SIMLOCK
  mmgsdi_simlock_build_and_run_algo(TRUE);
#endif /* FEATURE_SIMLOCK */

  mmgsdi_evt_build_and_send_perso(
    notify_type,
    (const mmgsdi_int_app_info_type*)sel_app_ptr,
    MMGSDI_PERSO_STATUS_DONE,
    MMGSDI_MAX_PERSO_FEATURE_ENUM,
    0,
    0);

  UIM_MSG_HIGH_0("MMGSDI_SUBSCRIPTION_READY_EVT");

  mmgsdi_evt_build_and_send_subscription_ready(
    notify_type, (const mmgsdi_int_app_info_type*)sel_app_ptr);

  sel_app_ptr->app_state = MMGSDI_APP_STATE_READY;

  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_trigger_session_events */


/*===========================================================================
FUNCTION MMGSDI_PSM_TRIGGER_EVENTS

DESCRIPTION
  To trigger card/session events.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_trigger_events(void)
{
  mmgsdi_slot_data_type  *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);
  if (slot_data_ptr == NULL)
  {
    return;
  }

  /* Set the early init flag to true since we are triggering events */
  mmgsdi_psm_early_init_done = TRUE;

  slot_data_ptr->mmgsdi_state = MMGSDI_STATE_CARD_INSERTED;

  UIM_MSG_HIGH_0("MMGSDI_CARD_INSERTED_EVT");
  mmgsdi_evt_build_and_send_card_inserted(TRUE, 0, MMGSDI_SLOT_1);

  (void)mmgsdi_psm_trigger_session_events(mmgsdi_generic_data_ptr->pri_gw_session_id);

#ifdef FEATURE_MMGSDI_3GPP2
  (void)mmgsdi_psm_trigger_session_events(mmgsdi_generic_data_ptr->pri_1x_session_id);
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_psm_trigger_events */


/*===========================================================================
FUNCTION MMGSDI_PSM_INITIALIZE

DESCRIPTION
  To trigger reading of EFS data, caching it and seding card/session events.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_initialize
(
  void
)
{
  mmgsdi_return_enum_type   mmgsdi_status  = MMGSDI_SUCCESS;
  uint8                     psm_mode       = 0;
  uint8                     nv_data[8]     = {0};
  nv_stat_enum_type         nv_status      = NV_FAIL_S;
  mmgsdi_nv_item_cache_type mmgsdi_nv_data = {0};     

  if(mmgsdi_generic_data_ptr == NULL ||
     mmgsdi_generic_data_ptr->num_slots != 1)
  {
    return;
  }

  /* First check MMODE PSM NV is enabled on not */
  if (uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_PSM_MODE,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          UIM_COMMON_EFS_DEVICE,
                          (uint8 *)nv_data,
                          sizeof(nv_data)) !=
      UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_LOW_0("MMODE PSM NV is not set");
    return;
  }

  /* Second byte of NV denotes feature status */
  if(nv_data[1] == 0x00)
  {
    UIM_MSG_LOW_0("MMODE PSM NV is not enabled");
    return;
  }

  /* Check UIM PSM NV is enabled on not */
  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_PSM,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          UIM_COMMON_EFS_DEVICE,
                          (uint8 *)&psm_mode,
                          sizeof(psm_mode)) !=
      UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_LOW_0("UIM PSM NV is not set");
    return;
  }

  if(psm_mode == 0x00)
  {
    UIM_MSG_LOW_0("MMODE PSM NV is not enabled");
    return;
  }

  nv_status = mmgsdi_nv_get_item(NV_UIM_SELECT_DEFAULT_USIM_APP_I,
                                 &mmgsdi_nv_data, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  UIM_MSG_LOW_2("NV_UIM_SELECT_DEFAULT_USIM_APP_I Read: Status: 0x%x, Select Default App.: 0x%x",
                nv_status, mmgsdi_nv_data.item_value.uim_select_default_usim_app);

  if ((nv_status == NV_DONE_S) &&
      (mmgsdi_nv_data.item_value.uim_select_default_usim_app == FALSE))
  {
    return;
  }

  /* Set PSM enabled flag as TRUE */
  mmgsdi_psm_enabled = TRUE;

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* initialize slot, app and EF cache from stored PSM data */
  mmgsdi_status = mmgsdi_psm_sfs_cache_init();

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* trigger card/session events based on cached data. */
    mmgsdi_psm_trigger_events();
  }
}/* mmgsdi_psm_initialize */


/*===========================================================================
FUNCTION MMGSDI_PSM_GET_EF_DATA_LEN

DESCRIPTION
  To calculate EFs' data and attribute total length associated with session type

PARAMETERS
  app_type            : app type
  session_type        : session type
  variable_length_ptr : address of length data

DEPENDENCIES
  None

RETURN VALUE
  uint32 length of data and attribute

SIDE EFFECTS
  None
===========================================================================*/
static uint32 mmgsdi_psm_get_ef_data_len
(
  mmgsdi_app_enum_type              app_type,
  mmgsdi_session_type_enum_type     session_type
)
{
  uint32                       variable_length = 0;
  mmgsdi_return_enum_type      mmgsdi_status   = MMGSDI_ERROR;
  uint8                        index           = 0;
  mmgsdi_file_enum_type       *file_array_prr  = NULL;
  uint8                        num_of_files    = 0;
  mmgsdi_len_type              file_len        = 0;
  mmgsdi_cache_init_enum_type  cache_state     = MMGSDI_CACHE_MAX_ENUM;

  if(app_type == MMGSDI_APP_USIM &&
     session_type == MMGSDI_GW_PROV_PRI_SESSION)
  {
    file_array_prr = mmgsdi_psm_usim_cache_enums;
    num_of_files   = sizeof(mmgsdi_psm_usim_cache_enums) /
                     sizeof(mmgsdi_psm_usim_cache_enums[0]);
  }
#ifdef FEATURE_MMGSDI_3GPP2
  else if(app_type == MMGSDI_APP_CSIM &&
          session_type == MMGSDI_1X_PROV_PRI_SESSION)
  {
    file_array_prr = mmgsdi_psm_csim_cache_enums;
    num_of_files   = sizeof(mmgsdi_psm_csim_cache_enums) /
                     sizeof(mmgsdi_psm_csim_cache_enums[0]);
  }
#endif /* FEATURE_MMGSDI_3GPP2 */
  else
  {
    return variable_length;
  }

  for(index = 0; index < num_of_files; index++)
  {
    mmgsdi_status = mmgsdi_cache_read_item_len(file_array_prr[index],
                                               app_type,
                                               session_type,
                                               0,
                                               &file_len,
                                               &cache_state);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      continue;
    }

    variable_length = variable_length + sizeof(mmgsdi_psm_ef_data) + file_len - sizeof(uint8);
  }
  UIM_MSG_HIGH_2("length of EFs data and atttibute cache 0x%x for app 0x%x",
                 variable_length, app_type);
  return variable_length;
}/* mmgsdi_psm_get_ef_data_len */


/*===========================================================================
FUNCTION MMGSDI_PSM_STORE_EF_DATA

DESCRIPTION
  To copy EFs' data and attribute total length associated with session type

PARAMETERS
  uicc_profile_pptr    : address of UICC profile PSM data ptr
  app_type             : app type
  session_type         : session type
  slot_id              : slot id
  remaining_length_ptr : inout-output paramter; provide reamining length left
                         after copied PSM data here for associated app type

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_store_ef_data
(
  uint8                            **uicc_profile_pptr,
  mmgsdi_app_enum_type               app_type,
  mmgsdi_session_type_enum_type      session_type,
  mmgsdi_slot_id_enum_type           slot_id,
  uint32                             remaining_length
)
{
  mmgsdi_return_enum_type          mmgsdi_status     = MMGSDI_SUCCESS;
  uint8                            index             = 0;
  mmgsdi_attr_cache_element_type  *cache_element_ptr = NULL;
  mmgsdi_session_id_type           session_id        = MMGSDI_SESSION_ID_ZERO; 
  mmgsdi_file_enum_type           *file_array_prr    = NULL;
  mmgsdi_cache_init_enum_type      cache_state       = MMGSDI_CACHE_MAX_ENUM;
  uint8                            num_of_files      = 0;
  mmgsdi_len_type                  file_len          = 0;
  uint8                           *data_ptr          = NULL;
  mmgsdi_access_type               access;

  if(uicc_profile_pptr == NULL)
  {
    return MMGSDI_ERROR;
  }
  UIM_MSG_HIGH_2("Remaining length to copy EFs data and atttibute cache 0x%x for app 0x%x",
                 remaining_length, app_type);

  memset(&access, 0x00, sizeof(mmgsdi_access_type));

  if(app_type == MMGSDI_APP_USIM &&
     session_type == MMGSDI_GW_PROV_PRI_SESSION)
  {
    file_array_prr = mmgsdi_psm_usim_cache_enums;
    num_of_files   = sizeof(mmgsdi_psm_usim_cache_enums) /
                     sizeof(mmgsdi_psm_usim_cache_enums[0]);
  }
#ifdef FEATURE_MMGSDI_3GPP2
  else if(app_type == MMGSDI_APP_CSIM &&
          session_type == MMGSDI_1X_PROV_PRI_SESSION)
  {
    file_array_prr = mmgsdi_psm_csim_cache_enums;
    num_of_files   = sizeof(mmgsdi_psm_csim_cache_enums) /
                     sizeof(mmgsdi_psm_csim_cache_enums[0]);
  }
#endif /* FEATURE_MMGSDI_3GPP2 */
  else
  {
    return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_mmgsdi_session_id_from_provisioning_type(
                         session_type, &session_id))
  {
    return MMGSDI_ERROR;
  }

  for(index = 0; index < num_of_files; index++)
  {
    access.access_method = MMGSDI_EF_ENUM_ACCESS;
    access.file.file_enum = file_array_prr[index];

    mmgsdi_status = mmgsdi_cache_read_item_len(file_array_prr[index],
                                               app_type,
                                               session_type,
                                               0,
                                               &file_len,
                                               &cache_state);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      continue;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data_ptr, file_len);
    if(data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    mmgsdi_status = mmgsdi_cache_read_item(file_array_prr[index],
                                           app_type,
                                           session_type,
                                           file_len,
                                           0,
                                           &file_len,
                                           data_ptr,
                                           &cache_state);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
      return MMGSDI_ERROR;
    }

    mmgsdi_status = mmgsdi_cache_attr_read(session_id,
                                           slot_id,
                                           &access,
                                           &cache_element_ptr);

    if((mmgsdi_status != MMGSDI_SUCCESS) || cache_element_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
      mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
      return MMGSDI_ERROR;
    }

    ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->sfi =
      cache_element_ptr->sfi;
    ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->num_of_rec =
      (uint8)cache_element_ptr->num_of_rec;

    /* If protection method is other that "Always" and "Allowed", we map it to
       "Single Allowed" because we only store one PIN in PSM data for ease of
       caching */
    if(cache_element_ptr->read.protection_method != MMGSDI_ALWAYS_ALLOWED &&
       cache_element_ptr->read.protection_method != MMGSDI_NEVER_ALLOWED)
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->read_protection = MMGSDI_SINGLE_ALLOWED;
      if(cache_element_ptr->read.protection_pin_ptr)
      {
        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->read_pin =
          cache_element_ptr->read.protection_pin_ptr[0];
      }
    }
    else
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->read_protection =
        cache_element_ptr->read.protection_method;
    }

    if(cache_element_ptr->write.protection_method != MMGSDI_ALWAYS_ALLOWED &&
       cache_element_ptr->write.protection_method != MMGSDI_NEVER_ALLOWED)
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->write_protection = MMGSDI_SINGLE_ALLOWED;
      if(cache_element_ptr->write.protection_pin_ptr)
      {
        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->write_pin =
          cache_element_ptr->write.protection_pin_ptr[0];
      }
    }
    else
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->write_protection =
        cache_element_ptr->write.protection_method;
    }

    if(cache_element_ptr->increase.protection_method != MMGSDI_ALWAYS_ALLOWED &&
       cache_element_ptr->increase.protection_method != MMGSDI_NEVER_ALLOWED)
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->increase_protection = MMGSDI_SINGLE_ALLOWED;
      if(cache_element_ptr->increase.protection_pin_ptr)
      {
        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->increase_pin =
          cache_element_ptr->increase.protection_pin_ptr[0];
      }
    }
    else
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->increase_protection =
        cache_element_ptr->increase.protection_method;
    }

    mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);

    ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->file_enum = file_array_prr[index];
    ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->rec_num = 0;

    ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len = file_len;

    remaining_length = remaining_length - sizeof(mmgsdi_psm_ef_data) + sizeof(uint8);
    mmgsdi_memscpy(&((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->first_byte_data,
                   remaining_length,
                   data_ptr,
                   ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len);

    remaining_length = remaining_length - ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len;
    *uicc_profile_pptr += (sizeof(mmgsdi_psm_ef_data) +
                           ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len -
                           sizeof(uint8));
    MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_store_ef_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_CHECK_UICC_PROFILE_PATH

DESCRIPTION
  Check if file to store PSM UICC profile exists, or create it with the required path

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE  : if path available
    FALSE : if not available

SIDE EFFECTS
  None
===========================================================================*/
static boolean mmgsdi_psm_check_uicc_profile_path
(
  void
)
{
  boolean is_uicc_profile_path_available = FALSE;

  if(MMGSDI_SUCCESS == gsdi_efs_name_test((const char *)mmgsdi_sfs_psm_uicc_profile,
                                          UIM_COMMON_EFS_DEVICE))
  {
    /* Delete PSM UICC proflle file from SFS to make sure that previous values
        don't remain in the file system when new file is written */
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    (void)mmgsdi_sfs_rmfile((const char *)mmgsdi_sfs_psm_uicc_profile);
#else
    #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
    return TRUE;
  }

  if(MMGSDI_SUCCESS != gsdi_efs_name_test((const char *)mmgsdi_safe_dir,
                                          UIM_COMMON_EFS_DEVICE))
  {
    return FALSE;
  }

  /*mmgsdi directory is available, check availability of psm dir*/
  if(MMGSDI_SUCCESS == gsdi_efs_name_test((const char *)mmgsdi_sfs_dir,
                                          UIM_COMMON_EFS_DEVICE))
  {
    is_uicc_profile_path_available = TRUE;
  }
  else
  {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    if(MMGSDI_SUCCESS == mmgsdi_sfs_mkdir((const char *)mmgsdi_sfs_dir))
#else
    #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
    {
      is_uicc_profile_path_available = TRUE;
    }
  }

  if(is_uicc_profile_path_available)
  {
    /*mmgsdi directory is available, check availability of psm dir*/
    if(MMGSDI_SUCCESS == gsdi_efs_name_test((const char *)mmgsdi_sfs_uim_dir,
                                            UIM_COMMON_EFS_DEVICE))
    {
      is_uicc_profile_path_available = TRUE;
    }
    else
    {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
      if(MMGSDI_SUCCESS == mmgsdi_sfs_mkdir((const char *)mmgsdi_sfs_uim_dir))
#else
      #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
      {
        is_uicc_profile_path_available = TRUE;
      }
    }

    if(is_uicc_profile_path_available)
    {
      /*mmgsdi directory is available, check availability of psm dir*/
      if(MMGSDI_SUCCESS == gsdi_efs_name_test((const char *)mmgsdi_sfs_uim_mmgsdi_dir,
                                              UIM_COMMON_EFS_DEVICE))
      {
        is_uicc_profile_path_available = TRUE;
      }
      else
      {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
        if(MMGSDI_SUCCESS == mmgsdi_sfs_mkdir((const char *)mmgsdi_sfs_uim_mmgsdi_dir))
#else
        #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
        {
          is_uicc_profile_path_available = TRUE;
        }
      }
    } 
  }

  return is_uicc_profile_path_available;
}/* mmgsdi_psm_check_uicc_profile_path */


/*===========================================================================
FUNCTION MMGSDI_PSM_STORE_DATA

DESCRIPTION
  To populate and store PSM UICC profile data on reception of task stop signal

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_store_data
(
  void
)
{
  mmgsdi_return_enum_type      mmgsdi_status                 = MMGSDI_ERROR;
  boolean                      is_psm_uicc_profile_available = FALSE;
  uint8                       *mmgsdi_uicc_profile_data_ptr  = NULL;
  uint8                       *mmgsdi_uicc_profile_curr_ptr  = NULL;
  mmgsdi_slot_data_type       *slot_data_ptr                 = NULL;
  uint8                        index                         = 0;
  uint8                        app_index                     = 0;
  uint8                        usim_app_index                = MMGSDI_MAX_EXT_APP_INFO;
  uint8                        csim_app_index                = MMGSDI_MAX_EXT_APP_INFO;
  uint8                        prov_index                    = 0;
  mmgsdi_cache_init_enum_type  cache_state                   = MMGSDI_CACHE_MAX_ENUM;
  uint32                       total_length                  = 0;
  mmgsdi_nv_item_cache_type    mmgsdi_nv_data                = {0};
  nv_stat_enum_type            nv_status                     = NV_FAIL_S;
  mmgsdi_access_type           access;
  mmgsdi_data_type             file_data;
#ifdef FEATURE_MMGSDI_3GPP2
  boolean                      rtre_nv_only                  = FALSE;
#endif /* FEATURE_MMGSDI_3GPP2 */

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  memset(&file_data, 0x00, sizeof(mmgsdi_data_type));

  if(mmgsdi_generic_data_ptr == NULL ||
     mmgsdi_generic_data_ptr->num_slots != 1)
  {
    return;
  }

  /* First check PSM NVs are enabled on not */
  if (mmgsdi_psm_get_enabled_status() == FALSE)
  {
    UIM_MSG_MED_0("PSM mode is not enabled");
    return;
  }

  /* Add check for Test SIM */
  nv_status = mmgsdi_nv_get_item(NV_GPRS_ANITE_GCF_I, &mmgsdi_nv_data,
                                 MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);
  if (nv_status == NV_DONE_S &&
      mmgsdi_nv_data.item_value.gprs_anite_gcf)
  {
    UIM_MSG_MED_0("GPRS ANITE GCF NV is set to 1");
    return;
  }

  /* Check if SLOT data ptr is available */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);

  if(slot_data_ptr == NULL || slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED ||
     slot_data_ptr->protocol != MMGSDI_UICC)
  {
    return;
  }

#ifdef FEATURE_MMGSDI_3GPP2
  /* Get RTRE configuration and if RTRE is NV_ONLY then don't check
     CSIM app activation status */
  nv_status = mmgsdi_nv_get_item(NV_RTRE_CONFIG_I, &mmgsdi_nv_data,
                                 MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);
  if((nv_status == NV_DONE_S) &&
     (mmgsdi_nv_data.item_value.rtre_config == NV_RTRE_CONFIG_NV_ONLY))
  {
    rtre_nv_only = TRUE;
  }
#endif /* FEATURE_MMGSDI_3GPP2 */

  /* Check if USIM/CSIM app is a provisioning app, in READY state and PIN is
     disabled */
  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO; index++)
  {
    if(slot_data_ptr->app_info_pptr[index] == NULL)
    {
      continue;
    }

    if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_USIM ||
       slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_CSIM)
    {
      if(slot_data_ptr->app_info_pptr[index]->app_data.aid.data_len > 0 &&
         slot_data_ptr->app_info_pptr[index]->app_state == MMGSDI_APP_STATE_READY &&
         slot_data_ptr->app_info_pptr[index]->pin1_ptr != NULL &&
         slot_data_ptr->app_info_pptr[index]->pin1_ptr->status == MMGSDI_PIN_DISABLED)
      {
        for(prov_index = 0; prov_index < MMGSDI_MAX_PROV_APPS; prov_index++)
        {
          if(slot_data_ptr->prov_app_indices[prov_index] ==
             (uint32)(index | MMGSDI_PROV_INDEX_VALIDITY_MASK))
          {
            if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_USIM)
            {
              usim_app_index = index;
            }
            else
            {
              csim_app_index = index;
            }
          }
        }
      }
      else
      {
        /* if USIM/CSIM App index is already found no need to return; we have
           more than one USIM/CSIM app */
#ifdef FEATURE_MMGSDI_3GPP2
        if((slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_USIM &&
            usim_app_index == MMGSDI_MAX_EXT_APP_INFO) ||
           (slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_CSIM &&
            csim_app_index == MMGSDI_MAX_EXT_APP_INFO &&
            rtre_nv_only == FALSE))
#else
        if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_USIM &&
           usim_app_index == MMGSDI_MAX_EXT_APP_INFO)
#endif
        {
          return;
        }
      }
    }
  }

  if(usim_app_index == MMGSDI_MAX_EXT_APP_INFO)
  {
    UIM_MSG_MED_0("USIM App is not in Ready state or PIN enabled");
    return;
  }

  if(mmgsdi_psm_data_ptr != NULL)
  {
    memset(mmgsdi_psm_data_ptr, 0x00, sizeof(mmgsdi_psm_data_type));
  }
  else
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_psm_data_ptr,
                                       sizeof(mmgsdi_psm_data_type));
    if(mmgsdi_psm_data_ptr == NULL)
    {
      return;
    }
  }

  /* Populate slot id and card type */
  mmgsdi_psm_data_ptr->card_type = slot_data_ptr->protocol;

  /* Populate ICCID */
  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_ICCID;

  file_data.data_len = MMGSDI_ICCID_LEN; 
  file_data.data_ptr = mmgsdi_psm_data_ptr->iccid_data.data;

  mmgsdi_status = mmgsdi_cache_read(mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_1_INDEX],
                                    &access,
                                    &file_data,
                                    0,
                                    &cache_state);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("failed to read cache for ICCID: mmgsdi_status 0x%x, cache_state 0x%x",
                  mmgsdi_status, cache_state);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
    return;
  }

  mmgsdi_psm_data_ptr->iccid_data.length = (uint8)file_data.data_len;

  /* Populate USIM app info. USIM is always on first position of array stored
     in the SFS, regardless of its position in EF-DIR */
  mmgsdi_memscpy(&mmgsdi_psm_data_ptr->app_data[0],
                 sizeof(mmgsdi_psm_data_ptr->app_data[0]),
                 &slot_data_ptr->app_info_pptr[usim_app_index]->app_data,
                 sizeof(slot_data_ptr->app_info_pptr[usim_app_index]->app_data)); 
  app_index++;

#ifdef FEATURE_MMGSDI_3GPP2
  /* Populate CSIM app info. CSIM is always on second position of array stored
     in the SFS if present, regardless of its position in EF-DIR */
  if(csim_app_index != MMGSDI_MAX_EXT_APP_INFO)
  {
    mmgsdi_memscpy(&mmgsdi_psm_data_ptr->app_data[1],
                   sizeof(mmgsdi_psm_data_ptr->app_data[1]),
                   &slot_data_ptr->app_info_pptr[csim_app_index]->app_data,
                   sizeof(slot_data_ptr->app_info_pptr[csim_app_index]->app_data)); 
    app_index++;
  }
#endif /* FEATURE_MMGSDI_3GPP2 */

  /* Populate all other apps listed in EF-DIR */
  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO && app_index < MMGSDI_MAX_APP_INFO; index++)
  {
    if(slot_data_ptr->app_info_pptr[index] == NULL ||
       slot_data_ptr->app_info_pptr[index]->app_not_in_efdir != FALSE ||
       index == usim_app_index ||
       index == csim_app_index)
    {
      continue;
    }
    mmgsdi_memscpy(&mmgsdi_psm_data_ptr->app_data[app_index],
                   sizeof(mmgsdi_psm_data_ptr->app_data[app_index]),
                   &slot_data_ptr->app_info_pptr[index]->app_data,
                   sizeof(slot_data_ptr->app_info_pptr[index]->app_data)); 
    app_index++;
  }

  /* Get USIM App EFs data len */
  total_length = mmgsdi_psm_get_ef_data_len(MMGSDI_APP_USIM,
                                            MMGSDI_GW_PROV_PRI_SESSION);

#ifdef FEATURE_MMGSDI_3GPP2
  /* Get CSIM App EFs data len */
  if(csim_app_index != MMGSDI_MAX_EXT_APP_INFO)
  {
    total_length = total_length +
                     mmgsdi_psm_get_ef_data_len(MMGSDI_APP_CSIM,
                                                MMGSDI_1X_PROV_PRI_SESSION);
  }
#endif /* FEATURE_MMGSDI_3GPP2 */

  /* Allocate memory for UICC profile */
  total_length = sizeof(mmgsdi_psm_data_type) + total_length;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_uicc_profile_data_ptr,
                                     total_length);

  if(mmgsdi_uicc_profile_data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
    return;
  }

  mmgsdi_uicc_profile_curr_ptr = mmgsdi_uicc_profile_data_ptr;

  /* Copy fix sized structure*/
  mmgsdi_memscpy(mmgsdi_uicc_profile_curr_ptr,
                 total_length,
                 mmgsdi_psm_data_ptr,
                 sizeof(mmgsdi_psm_data_type));

  mmgsdi_uicc_profile_curr_ptr += sizeof(mmgsdi_psm_data_type);

  /* Populate USIM App EF data */
  mmgsdi_status = mmgsdi_psm_store_ef_data(&mmgsdi_uicc_profile_curr_ptr,
                                           MMGSDI_APP_USIM,
                                           MMGSDI_GW_PROV_PRI_SESSION,
                                           MMGSDI_SLOT_1,
                                           total_length - 
                                           (mmgsdi_uicc_profile_curr_ptr -
                                            mmgsdi_uicc_profile_data_ptr));
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_uicc_profile_data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
    return;
  }

#ifdef FEATURE_MMGSDI_3GPP2
  /* Populate CSIM App EF data */
  if(csim_app_index != MMGSDI_MAX_EXT_APP_INFO)
  {
    mmgsdi_status = mmgsdi_psm_store_ef_data(&mmgsdi_uicc_profile_curr_ptr,
                                             MMGSDI_APP_CSIM,
                                             MMGSDI_1X_PROV_PRI_SESSION,
                                             MMGSDI_SLOT_1,
                                             total_length - 
                                             (mmgsdi_uicc_profile_curr_ptr -
                                              mmgsdi_uicc_profile_data_ptr));
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_uicc_profile_data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
      return;
    }
  }
#endif /* FEATURE_MMGSDI_3GPP2 */

  /* Check if file to store PSM UICC profile exists, or create it with the required
     path */
  is_psm_uicc_profile_available = mmgsdi_psm_check_uicc_profile_path();

  /* Write UICC profile in SFS is path or file is present */
  if(is_psm_uicc_profile_available)
  {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    mmgsdi_status = mmgsdi_sfs_write(
                        mmgsdi_uicc_profile_data_ptr,
                        &total_length,
                        (const char *)mmgsdi_sfs_psm_uicc_profile);
#else
    #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
  }

  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_uicc_profile_data_ptr);
}/* mmgsdi_psm_store_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_ENABLED_STATUS

DESCRIPTION
   Return the status whether UIM PSM NV, MMODE PSM NV and NV 4398
   auto provisioning are enabled or not.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_psm_get_enabled_status(
  void
)
{
  /* return the status whether PSM is enabled or not */
  return mmgsdi_psm_enabled;
}/* mmgsdi_psm_get_enabled_status */


/* ==========================================================================
FUNCTION MMGSDI_PSM_UTIL_INTERNAL_CB

DESCRIPTION
  This function, defined as mmgsdi_callback_type, will handle the internal
  commands  generated by MMGSDI PSM

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
----------------------------------------------------------------------------*/
static void mmgsdi_psm_util_internal_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  (void)status;
  (void)cnf;
  (void)cnf_ptr;
}/*mmgsdi_psm_util_internal_cb*/

/* ===========================================================================
FUNCTION:      MMGSDI_PSM_UTIL_SELECT_ADF

DESCRIPTION:
  This function sends SELECT for correct ADF 

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS:          SELECT operation was successful
  MMGSDI_ERROR:            SELECT operation was successful

SIDE EFFECTS:
  None.
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_util_select_adf(
  mmgsdi_session_id_type   session_id
)
{
  mmgsdi_return_enum_type              mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type       *select_ptr    = NULL;
  mmgsdi_get_file_attr_cnf_type       *cnf_ptr       = NULL;
  mmgsdi_int_app_info_type            *sel_app_ptr   = NULL;
  mmgsdi_slot_id_enum_type             slot          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_get_file_info_extra_info_type extra_info;

  memset(&extra_info, 0x00, sizeof(mmgsdi_get_file_info_extra_info_type));

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   &slot,
                                                   NULL,
                                                   &sel_app_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (sel_app_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if((sel_app_ptr->app_data.app_type != MMGSDI_APP_USIM) &&
     (sel_app_ptr->app_data.app_type != MMGSDI_APP_CSIM))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
    sizeof(mmgsdi_get_file_attr_req_type));
  if (select_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  select_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  select_ptr->request_header.session_id        = session_id;
  select_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.request_len       =
    uint32toint32(sizeof(mmgsdi_get_file_attr_req_type));
  select_ptr->request_header.payload_len        =
    select_ptr->request_header.request_len -
    uint32toint32(sizeof(mmgsdi_get_file_attr_req_type));
  select_ptr->request_header.slot_id           = slot;
  select_ptr->request_header.client_data       = 0;
  select_ptr->request_header.response_cb       = NULL;
  select_ptr->access.access_method             = MMGSDI_BY_APP_ID_ACCESS;
  /* Since we do not care about the sec attributes, skip the check */
  select_ptr->skip_uicc_arr                    = TRUE;

  mmgsdi_memscpy(&select_ptr->access.file.app_id,
                 sizeof(select_ptr->access.file.app_id),
                 &sel_app_ptr->app_data.aid,
                 sizeof(mmgsdi_static_data_type));

  select_ptr->activate_aid                     = TRUE;
  select_ptr->select_rsp_type                  = MMGSDI_SELECT_RSP_FCP;
  select_ptr->select_rsp_requested             = TRUE;

  mmgsdi_status = mmgsdi_uim_uicc_select (select_ptr,
                                          TRUE,
                                          &cnf_ptr,
                                          TRUE);
  if (cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Cnf Ptr Null after SELECT ADF");
    MMGSDIUTIL_TMC_MEM_FREE(select_ptr);
    return MMGSDI_ERROR;
  }

  if( (mmgsdi_status != MMGSDI_SUCCESS) ||
      (cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_0("Select ADF failed");
    MMGSDIUTIL_TMC_MEM_FREE(select_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
    return MMGSDI_ERROR;
  }

  if(mmgsdi_uicc_rsp_parse_select_response_adf(
       sel_app_ptr,
       cnf_ptr->raw_file_attr_data.data_ptr,
       (uint16)cnf_ptr->raw_file_attr_data.data_len,
       MMGSDI_SELECT_RSP_FCP) !=
     MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("SELECT response could not be parsed");
    MMGSDIUTIL_TMC_MEM_FREE(select_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
    return MMGSDI_ERROR;
  }

  extra_info.select_rsp_type = MMGSDI_SELECT_RSP_FCP;
  extra_info.access.access_method = MMGSDI_BY_APP_ID_ACCESS;
  mmgsdi_memscpy(&extra_info.access.file.app_id,
                 sizeof(extra_info.access.file.app_id),
                 &sel_app_ptr->app_data.aid,
                 sizeof(mmgsdi_static_data_type));

  /* Reset the PIN1 status before decoding */
  sel_app_ptr->pin1_ptr->status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;

  mmgsdi_status =  mmgsdi_uicc_rsp_decode_pin_header_data (
      &(select_ptr->request_header),
      &extra_info,
      cnf_ptr->raw_file_attr_data.data_ptr,
      cnf_ptr->raw_file_attr_data.data_len);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Select ADF failed");
    mmgsdi_status = MMGSDI_ERROR;
  }

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_ptr);
  return mmgsdi_status;
} /* mmgsdi_psm_util_select_adf */


/* ===========================================================================
FUNCTION      MMGSDI_PSM_UTIL_SEND_STATUS

DESCRIPTION
  This function will send the STATUS command 

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
  MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                           but the service table indicates the card does not
                           have the support

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_util_send_status(
  mmgsdi_session_id_type    session_id,
  mmgsdi_slot_id_enum_type  slot_id
)
{
  mmgsdi_status_req_type       status_req;
  mmgsdi_status_cnf_type     * cnf_ptr       = NULL;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;

  memset(&status_req, 0, sizeof(status_req));

  status_req.request_header.request_type      = MMGSDI_CARD_STATUS_REQ;
  status_req.request_header.orig_request_type = MMGSDI_CARD_STATUS_REQ;
  status_req.request_header.session_id        = session_id;
  status_req.request_header.slot_id           = slot_id;
  status_req.me_app_status                    = MMGSDI_STATUS_APP_TERMINATED;
  status_req.ret_data_struct                  = MMGSDI_STATUS_NO_DATA;

  mmgsdi_status = mmgsdi_uim_uicc_status(&status_req, TRUE, &cnf_ptr, NULL);

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type *) cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_MED_0("Unable to send STATUS command ");
  }

  return mmgsdi_status;
} /* mmgsdi_psm_util_send_status */


/*===========================================================================
FUNCTION MMGSDI_PSM_UTIL_READ_AND_COMPARE_EF

DESCRIPTION
  Function to read and compare EF data with the early init cache and real card

PARAMETERS
  session id :  session id
  file       : file type
  data_ptr   : pointer to data type
  app_type   : app type

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_util_read_and_compare_ef(
  mmgsdi_session_id_type        session_id,
  mmgsdi_file_enum_type         file,
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type
)
{
  mmgsdi_len_type                     data_len      = 0;
  uint8                              *data_ptr      = NULL;
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_data_type                    data;

  if(mmgsdi_psm_local_cache_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  switch(file)
  {
    case MMGSDI_USIM_LOCI:
      data_len = mmgsdi_psm_local_cache_ptr->loci_len;
      data_ptr = mmgsdi_psm_local_cache_ptr->loci_data;
      break;

    case MMGSDI_USIM_PSLOCI:
      data_len = mmgsdi_psm_local_cache_ptr->psloci_len;
      data_ptr = mmgsdi_psm_local_cache_ptr->psloci_data;
      break;

    case MMGSDI_USIM_EPSLOCI:
      data_len = mmgsdi_psm_local_cache_ptr->epsloci_len;
      data_ptr = mmgsdi_psm_local_cache_ptr->epsloci_data;
      break;

    default:
      /* get the existing cache data into temp location */
      mmgsdi_status = mmgsdi_cache_read_item_len(
                        file,
                        app_type,
                        session_type,
                        0,
                        &data_len,
                        NULL);
      if(mmgsdi_status != MMGSDI_SUCCESS ||
         data_len <= 0)
      {
        return MMGSDI_ERROR;
      }

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data_ptr, data_len);
      if(data_ptr == NULL)
      {
        return MMGSDI_ERROR;
      }

      mmgsdi_status = mmgsdi_cache_read_item(file,
                                             app_type,
                                             session_type,
                                             data_len,
                                             0,
                                             &data_len,
                                             data_ptr,
                                             NULL);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
        return MMGSDI_ERROR;
      }
      break;
  }

  /* Delete Existing data from cache and read from card */
  (void)mmgsdi_cache_delete_item(session_id,
                                 1,
                                 &file);
  
  memset(&data, 0x00, sizeof(mmgsdi_data_type));
  
  mmgsdi_status = mmgsdi_card_init_cache_binary(
                    session_id,
                    app_type, MMGSDI_SLOT_1, file,
                    &data);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      data.data_ptr == NULL ||
      data.data_len == 0)
  {
    if(file != MMGSDI_USIM_LOCI &&
       file != MMGSDI_USIM_PSLOCI &&
       file != MMGSDI_USIM_EPSLOCI)
    {
      MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
    }

    /* In case PSM cache is empty and the access from card also returned
       NOT_SUPPORTED due to UST setting, consider it as sucessful match 
       Other cases like - if psm cache is empty and the current access to 
       card returned data, then comparison is failed and if psm cache is
       not empty but the current access did not return data then comparison
       is failed */
    if(file == MMGSDI_USIM_EPSLOCI &&
       mmgsdi_status == MMGSDI_NOT_SUPPORTED &&
       mmgsdi_psm_local_cache_ptr->epsloci_len == 0)
    {
      return MMGSDI_SUCCESS;
    }
    else
    {
      return MMGSDI_ERROR;
    }
  }

  /* mem cmp EF Data */
  if (data_len != data.data_len ||
      (memcmp(data_ptr,data.data_ptr, data.data_len) != 0))
  {
    mmgsdi_status = MMGSDI_ERROR;
  }

  if(file != MMGSDI_USIM_LOCI &&
     file != MMGSDI_USIM_PSLOCI &&
     file != MMGSDI_USIM_EPSLOCI)
  {
    MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(data.data_ptr);

  return mmgsdi_status;
}/*mmgsdi_psm_util_read_and_compare_ef*/


/*===========================================================================
FUNCTION MMGSDI_PSM_UTIL_OPEN_CHANNEL

DESCRIPTION
  Perform open channel request

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_util_open_channel(
  mmgsdi_session_id_type   session_id,
  mmgsdi_channel_id_type   channel_id
)
{
  mmgsdi_request_header_type          req_header     = {0};
  mmgsdi_return_enum_type             mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_session_deactivate_cnf_type *cnf_ptr        = NULL;
  mmgsdi_channel_id_type              out_channel_id = 0xff;

  req_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  req_header.session_id        = session_id;
  req_header.channel_id        = 0;
  req_header.request_type      = MMGSDI_MANAGE_CHANNEL_REQ;
  req_header.slot_id           = MMGSDI_SLOT_1;
  req_header.orig_request_type = MMGSDI_MANAGE_CHANNEL_REQ;

  /* open channel */
  mmgsdi_status = mmgsdi_uim_uicc_manage_channel_op(&req_header,
                                                    UIM_CHANNEL0,
                                                    UIM_MC_OPEN,
                                                    NULL,
                                                    TRUE,
                                                    &cnf_ptr,
                                                    &out_channel_id);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      cnf_ptr == NULL ||
      (cnf_ptr != NULL && 
       cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_1("Channel id open failed for channel:0x%x:", channel_id);
    mmgsdi_status = MMGSDI_ERROR;
  }

  /* If channel returned from Card is different from previosuly initialized at psm init
     - update channel in channel info table */
  if (mmgsdi_status == MMGSDI_SUCCESS &&
      out_channel_id != channel_id)
  {
    mmgsdi_channel_info_type *channel_info_ptr = NULL;

    mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                     NULL,
                                                     NULL,
                                                     NULL,
                                                     &channel_info_ptr,
                                                     NULL,
                                                     NULL);
    if (mmgsdi_status != MMGSDI_SUCCESS ||
        channel_info_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
    else
    {
      channel_info_ptr->channel_id = out_channel_id;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);

  return mmgsdi_status;
}/*mgsdi_psm_util_open_channel*/


/*===========================================================================
FUNCTION MMGSDI_PSM_UTIL_UPDATE_TST

DESCRIPTION
  Perform EF TST WRITE if applicable

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_util_update_tst(
  void
)
{
  mmgsdi_len_type          data_len      = 0;
  uint8                   *data_ptr      = NULL;
  mmgsdi_write_req_type   *req_ptr       = NULL;
  mmgsdi_write_cnf_type   *read_cnf_ptr  = NULL;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;

  /* get the existing cache data into temp location */
  mmgsdi_status = mmgsdi_cache_read_item_len(
                    MMGSDI_USIM_7F66_PROP1_SPT_TABLE,
                    MMGSDI_APP_USIM,
                    MMGSDI_GW_PROV_PRI_SESSION,
                    0,
                    &data_len,
                    NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS || data_len <= 0)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data_ptr, data_len);
  if (data_ptr == NULL)
  {
    return;
  }
  
  mmgsdi_status = mmgsdi_cache_read_item(MMGSDI_USIM_7F66_PROP1_SPT_TABLE,
                                         MMGSDI_APP_USIM,
                                         MMGSDI_GW_PROV_PRI_SESSION,
                                         data_len,
                                         0,
                                         &data_len,
                                         data_ptr,
                                         NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr,
                                     sizeof(mmgsdi_write_req_type));
  if (req_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr->data.data_ptr, data_len);
  if (req_ptr->data.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return;
  }
  
  req_ptr->request_header.session_id         = mmgsdi_generic_data_ptr->pri_gw_session_id;
  req_ptr->request_header.slot_id            = MMGSDI_SLOT_1;
  req_ptr->request_header.request_type       = MMGSDI_WRITE_REQ;
  req_ptr->request_header.orig_request_type  = MMGSDI_WRITE_REQ;
  req_ptr->access.access_method              = MMGSDI_EF_ENUM_ACCESS;
  req_ptr->access.file.file_enum             = MMGSDI_USIM_7F66_PROP1_SPT_TABLE;
  req_ptr->rec_num                           = 0;
  req_ptr->offset                            = 0;
  req_ptr->file_type                         = MMGSDI_TRANSPARENT_FILE;
  req_ptr->data.data_len                     = data_len;

  mmgsdi_memscpy(req_ptr->data.data_ptr,
                 data_len,
                 data_ptr,
                 data_len);

  mmgsdi_status = mmgsdi_uim_uicc_write_transparent(req_ptr, TRUE, &read_cnf_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("Failed to write to SPT Table: Status = 0x%x", mmgsdi_status);
  }
  
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr->data.data_ptr);
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)read_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
}/*mmgsdi_psm_util_update_tst*/


/*===========================================================================
FUNCTION MMGSDI_PSM_HANDLE_USIM_INIT

DESCRIPTION
  Hanldes USIM app initialization in PSM mode - 
  Peforms AID selection, read of IMSI, LOCI, PS LOCI and EPS LOCI from
  card and compare them with PSM data.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_handle_usim_init(
  void
)
{
  mmgsdi_int_app_info_type           *sel_app_ptr   = NULL;
  mmgsdi_channel_id_type              channel_id    = 0;
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type            slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;

  mmgsdi_status = mmgsdi_util_get_session_app_info(mmgsdi_generic_data_ptr->pri_gw_session_id,
                                                   &slot_id,
                                                   &channel_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      slot_id != MMGSDI_SLOT_1)
  {
    return MMGSDI_ERROR;
  }

  /* if the channel is non-default, open channel before select AID */
  if (channel_id != MMGSDI_DEFAULT_CHANNEL)
  {
    mmgsdi_status = mmgsdi_psm_util_open_channel(
      mmgsdi_generic_data_ptr->pri_gw_session_id,
      channel_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }
  }

  mmgsdi_status = mmgsdi_psm_util_select_adf(mmgsdi_generic_data_ptr->pri_gw_session_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Check pin status */
  mmgsdi_status = mmgsdi_util_get_session_app_info(mmgsdi_generic_data_ptr->pri_gw_session_id,
                                                   NULL,
                                                   NULL,
                                                   &sel_app_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (sel_app_ptr == NULL ||
      sel_app_ptr->pin1_ptr == NULL ||
      sel_app_ptr->pin1_ptr->status != MMGSDI_PIN_DISABLED)
  {
    UIM_MSG_ERR_0("USIM PIN Status is no DISABLED - end PSM init");
    return MMGSDI_ERROR;
  }

  /* Write EF TST if available in cache */
  mmgsdi_psm_util_update_tst();

  /* Read and compare IMSI Data */
  mmgsdi_status = mmgsdi_psm_util_read_and_compare_ef(mmgsdi_generic_data_ptr->pri_gw_session_id,
                    MMGSDI_USIM_IMSI, MMGSDI_APP_USIM, MMGSDI_GW_PROV_PRI_SESSION);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Read and compare LOCI Data */
  mmgsdi_status = mmgsdi_psm_util_read_and_compare_ef(mmgsdi_generic_data_ptr->pri_gw_session_id,
                    MMGSDI_USIM_LOCI, MMGSDI_APP_USIM, MMGSDI_GW_PROV_PRI_SESSION);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Read and compare PSLOCI Data */
  mmgsdi_status = mmgsdi_psm_util_read_and_compare_ef(mmgsdi_generic_data_ptr->pri_gw_session_id,
                    MMGSDI_USIM_PSLOCI, MMGSDI_APP_USIM, MMGSDI_GW_PROV_PRI_SESSION);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Read and compare EPSLOCI Data */
  mmgsdi_status = mmgsdi_psm_util_read_and_compare_ef(mmgsdi_generic_data_ptr->pri_gw_session_id,
                    MMGSDI_USIM_EPSLOCI, MMGSDI_APP_USIM, MMGSDI_GW_PROV_PRI_SESSION);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Init is done for USIM, send STATUS command */
  mmgsdi_status = mmgsdi_psm_util_send_status(mmgsdi_generic_data_ptr->pri_gw_session_id, slot_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_handle_usim_init */


#ifdef FEATURE_MMGSDI_3GPP2
/*===========================================================================
FUNCTION MMGSDI_PSM_HANDLE_CSIM_INIT

DESCRIPTION
  Hanldes CSIM app initialization in PSM mode - 
  Peforms AID selection, read of IMSI_M from card and compare them with PSM data.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_handle_csim_init(
  void
)
{
  mmgsdi_int_app_info_type           *sel_app_ptr   = NULL;
  mmgsdi_channel_id_type              channel_id    = 0;
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type            slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_store_esn_req_type  *store_esn_req_ptr = NULL;

  mmgsdi_status = mmgsdi_util_get_session_app_info(mmgsdi_generic_data_ptr->pri_1x_session_id,
                                                   &slot_id,
                                                   &channel_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      slot_id != MMGSDI_SLOT_1)
  {
    return MMGSDI_ERROR;
  }

  /* if the channel is non-default, open channel before select AID */
  if (channel_id != MMGSDI_DEFAULT_CHANNEL)
  {
    mmgsdi_status = mmgsdi_psm_util_open_channel(
      mmgsdi_generic_data_ptr->pri_1x_session_id,
      channel_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }
  }

  mmgsdi_status = mmgsdi_psm_util_select_adf(mmgsdi_generic_data_ptr->pri_1x_session_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Check pin status */
  mmgsdi_status = mmgsdi_util_get_session_app_info(mmgsdi_generic_data_ptr->pri_1x_session_id,
                                                   NULL,
                                                   NULL,
                                                   &sel_app_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (sel_app_ptr == NULL ||
      sel_app_ptr->pin1_ptr == NULL ||
      sel_app_ptr->pin1_ptr->status != MMGSDI_PIN_DISABLED)
  {
    UIM_MSG_ERR_0("CSIM PIN Status is no DISABLED - end PSM init");
    return MMGSDI_ERROR;
  }

  /* Read and compare IMSI_M Data */
  mmgsdi_status = mmgsdi_psm_util_read_and_compare_ef(mmgsdi_generic_data_ptr->pri_1x_session_id,
                    MMGSDI_CSIM_IMSI_M, MMGSDI_APP_CSIM, MMGSDI_1X_PROV_PRI_SESSION);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(store_esn_req_ptr,
                                     sizeof(mmgsdi_session_store_esn_req_type));
  if(store_esn_req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  store_esn_req_ptr->request_header.session_id        = mmgsdi_generic_data_ptr->pri_1x_session_id;
  store_esn_req_ptr->request_header.request_type      = MMGSDI_SESSION_STORE_ESN_REQ;
  store_esn_req_ptr->request_header.orig_request_type = MMGSDI_SESSION_STORE_ESN_REQ;
  store_esn_req_ptr->request_header.slot_id           = slot_id;
  store_esn_req_ptr->request_header.client_data       = 0;
  store_esn_req_ptr->request_header.response_cb       = NULL;

  mmgsdi_status = mmgsdi_card_init_store_esn(store_esn_req_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(store_esn_req_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("CSIM Store ESN failed - end PSM init");
    return MMGSDI_ERROR;
  }

  /* Init is done for CSIM, send STATUS command */
  mmgsdi_status = mmgsdi_psm_util_send_status(mmgsdi_generic_data_ptr->pri_1x_session_id, slot_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_handle_csim_init */
#endif /* FEATURE_MMGSDI_3GPP2 */


/*===========================================================================
FUNCTION MMGSDI_PSM_SESSION_CLEANUP_SEND_CARD_ERROR

DESCRIPTION
  This function is invoked when card mismatch is detected, CARD 
  ERR EVENT is triggered and CARD Is reset.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_session_cleanup_send_card_error(
  void
)
{
  mmgsdi_card_err_info_enum_type   prev_err_cause = MMGSDI_CARD_ERR_UNKNOWN_ERROR;
  mmgsdi_slot_data_type           *slot_data_ptr = NULL;

  /* send out card error and session clean up and trigger card reset */


  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);

  if (slot_data_ptr == NULL)
  {
    return;
  }

  /* reset the early init flag */
  mmgsdi_psm_early_init_done = FALSE;

  slot_data_ptr->sim_busy = FALSE;

  prev_err_cause = slot_data_ptr->card_error_condition;
  slot_data_ptr->card_error_condition = MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY;

  /* Send out the Card Error event and perform Session and app clean-up */
  mmgsdi_evt_build_and_send_card_error(TRUE,
                                       0,
                                       MMGSDI_SLOT_1,
                                       prev_err_cause);

  /* Update the session closed cause. The session changed event
     broadcasts the cause */
  mmgsdi_util_update_session_closed_cause_for_slot(MMGSDI_SLOT_1, 
    MMGSDI_SESSION_CLOSED_CAUSE_CARD_POWER_DOWN);

  mmgsdi_util_close_all_sessions_cleanup_and_notify(MMGSDI_SLOT_1);
  mmgsdi_util_reset_app_data(FALSE, MMGSDI_SLOT_1);

  /* Reset Sanity Timer to default value */
  slot_data_ptr->mmgsdi_uim_sanity_timer_value
    = MMGSDI_UIM_SANITY_TIMER_VALUE;
} /* mmgsdi_psm_session_cleanup_send_card_error */


/*===========================================================================
FUNCTION MMGSDI_PSM_HANDLE_CARD_INIT_POST_TP_DL

DESCRIPTION
  This function handles select of AID and other init operations and
  make sure the early init is done on the same card that is inserted.
  if the card is found to be different, CARD ERROR is triggered and 
  card is reset to continue as a new card

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_psm_handle_card_init_post_tp_dl(
  void
)
{
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;

  if (mmgsdi_psm_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* TP DL is done
        - perform select on USIM 
        - check pin
        - read EFs
        - Compare EFs 
        - send Status 
   */
  do
  {
    if (mmgsdi_psm_data_ptr->app_data[0].app_type == MMGSDI_APP_USIM &&
        mmgsdi_psm_data_ptr->app_data[0].aid.data_len != 0)
    {
      mmgsdi_status = mmgsdi_psm_handle_usim_init();
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
    }

#ifdef FEATURE_MMGSDI_3GPP2
    if (mmgsdi_psm_data_ptr->app_data[1].app_type == MMGSDI_APP_CSIM &&
        mmgsdi_psm_data_ptr->app_data[1].aid.data_len != 0)
    {
      mmgsdi_status = mmgsdi_psm_handle_csim_init();
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
    }
#endif /* FEATURE_MMGSDI_3GPP2 */
  }
  while (0);

  /* Set the global for processing other  pending commands */
  mmgsdi_psm_early_init_done = FALSE;

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Send out card error and session cleanup and start new card */
    mmgsdi_psm_session_cleanup_send_card_error();
    
    /* Reset the card */
    mmgsdi_status = mmgsdi_gen_build_card_reset(mmgsdi_generic_data_ptr->client_id,
                                                MMGSDI_SLOT_1,
                                                MMGSDI_CARD_RESET_REQ,
                                                UIM_UICC,
                                                MMGSDI_RESET_CAUSE_GENERAL,
                                                mmgsdi_psm_util_internal_cb,
                                                0);
  }
  else
  {
    /* Update GSTK to enable FETCH if init is successful */
    gstk_gsdi_set_fetch_status(GSTK_SLOT_1, TRUE);
  }

  /* Check the count on the command queue and set signal */
  if (q_cnt(&mmgsdi_high_pri_cmd_q[MMGSDI_TASK_SLOT1]) > 0 ||
      q_cnt(&mmgsdi_task_cmd_q[MMGSDI_TASK_SLOT1]) > 0)
  {
    /* Set the command queue signal */
    (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_TASK_CMD_Q_SIG);
  }

  /* If we are here, either card is same and init successful or card reset 
     is triggered and the use of SFS is done, so purge it */
  mmgsdi_psm_purge_data();

  return mmgsdi_status;  
}/* mmgsdi_psm_handle_card_init_post_tp_dl */


/*===========================================================================
FUNCTION MMGSDI_PSM_GET_STATUS_EARLY_INIT

DESCRIPTION
  This function returns the status of PSM early init during the time 
  between early card and session events are sent and the real card is 
  detected. once the real card is detected, this function returns FALSE,
  indicating no impact to command processing and other functionalities

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_psm_get_status_early_init(
  void
)
{
  /* return the status if the early init was done and sub ready was sent */
  return mmgsdi_psm_early_init_done;
}/* mmgsdi_psm_get_status_early_init */


/*===========================================================================
FUNCTION MMGSDI_PSM_ICCID_AND_PROTOCOL_MATCH_DURING_LINK_EST

DESCRIPTION
  This function is invoked on link est of real card to match ICCID
  with psm iccid data  to ensure the early CARD and session events
  are valid

PARAMETERS:
  ICCID of the real card

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS:
  None
===========================================================================*/
boolean mmgsdi_psm_iccid_and_protocol_match_during_link_est(
  mmgsdi_iccid_info_type    iccid,
  mmgsdi_protocol_enum_type protocol
)
{
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);

  if (slot_data_ptr != NULL &&
      protocol == MMGSDI_UICC &&
      slot_data_ptr->protocol == protocol &&
      iccid.length > 0 &&
      iccid.length <= MMGSDI_ICCID_LEN &&
      mmgsdi_psm_data_ptr != NULL &&
      iccid.length == mmgsdi_psm_data_ptr->iccid_data.length &&
      (memcmp(iccid.data, mmgsdi_psm_data_ptr->iccid_data.data, iccid.length) == 0))
  {
    return TRUE;
  }

  /* Mistmatch in ICCID -Purge PSM data and trigger card error and session cleanup*/
  mmgsdi_psm_reset_early_init();

  mmgsdi_psm_session_cleanup_send_card_error();

  return FALSE;
}/*mmgsdi_psm_iccid_and_protocol_match_during_link_est*/


/*===========================================================================
FUNCTION MMGSDI_PSM_RESET_EARLY_INIT

DESCRIPTION
  This function reset the PSM early init flag and set MMGSDI_TASK_CMD_Q_SIG 
  when card error occurs. MMGSDI Slot1 task does not process this signal until
  early init flag is set to FALSE.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_reset_early_init(
  void
)
{
  if(mmgsdi_psm_early_init_done != FALSE)
  {
    /* Free global PSM data ptr and delete SFS file */
    mmgsdi_psm_purge_data();

    /* reset the flag */
    mmgsdi_psm_early_init_done = FALSE;

    /* Check the count on the command queue and set signal */
    if (q_cnt(&mmgsdi_high_pri_cmd_q[MMGSDI_TASK_SLOT1]) > 0 ||
        q_cnt(&mmgsdi_task_cmd_q[MMGSDI_TASK_SLOT1]) > 0)
    {
      /* Set the command queue signal */
      (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_TASK_CMD_Q_SIG);
    }
  }
}/* mmgsdi_psm_reset_early_init */

