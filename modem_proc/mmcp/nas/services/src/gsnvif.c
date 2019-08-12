/*===========================================================================
      COPYRIGHT INFORMATION

Copyright (c) 2002-2006 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/gsnvif.c_v   1.6   17 Jul 2002 11:52:16   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/gsnvif.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/05   hj      Corrected IMEISV encoding (reversed the SVN digit packing).
08/05/05   up      Updated to support L4/UMTS: Interface change.
12/09/02   ks      Setting the A5_3 to AF-7 supported to FALSE in hw_classmark.
10/01/02   rmc     Updates to IMEI reading from NV
09/06/02   JC      Eliminated FEATURE_NAS_NVMEM_ENABLED; read from NV.
09/18/02   sbk     NV access functions are implemented to read, write and replace NV items.
03/12/02   vdr     Marked the A5/3 algorithm as not supported
03/20/02   vdr     Changed the Power Capability of the KOVU to Mobile CLASS 4
04/30/02   AT      Hardcoded IMEI for MMO2 IOT for GSM.
05/02/02   rmc     Fixed bug where result is not initialized for cached NV items
                   where it needs to be used and caching NV IMEI and classmark information
07/08/02   rmc     Changed implementation of IMEI reading from NV
                   Provide independent featurization for each item that can be read from NV

===========================================================================*/

/* Include files */

#include "mmcp_variation.h"
#include <memory.h>
#include <string.h>
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "environ.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "ms.h"
#include "gs_v.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "aconfig.h"
#include "err.h"
#include "nv.h"
#include "ghdi_nvmem.h"
#include "ghdi_nvmem_v.h"
#include "mm_v.h"

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif 

#include "amssassert.h"

#include "mmgsdisessionlib.h"
#include "mmtask_int.h"
#define NAS_ASSERT_UT ASSERT
#ifdef TF_UNIT_TEST
#error code not present
#endif /* TF_UNIT_TEST */


#ifdef FEATURE_DUAL_SIM


#if defined(FEATURE_TRIPLE_SIM) && defined(FEATURE_DUAL_IMEI)
static uint8 cached_imei_mapping[MAX_AS_IDS] = {0, 1, 2};
static byte cached_slot_id_valid_imei_sim[MAX_AS_IDS] = {FALSE,FALSE,FALSE};
static uint8 cached_slot_id_imei_sim[MAX_AS_IDS] = {0, 1, 2};
#elif defined(FEATURE_DUAL_IMEI)
static uint8 cached_imei_mapping[MAX_AS_IDS] = {0, 1};
static byte cached_slot_id_valid_imei_sim[MAX_AS_IDS] = {FALSE,FALSE};
static uint8 cached_slot_id_imei_sim[MAX_AS_IDS] = {0, 1};
#endif

/**
 * Read the IMEI from NV (may use cached value if already read) and store it
 * in the given buffer. Default value is used if there is no IMEI stored in NV
 *
 * @param imei   buffer where the IMEI information should be stored. Caller must provide enough
 *               space for an IMEI in IE format
 *
 * @return status of the read operation. Indicates if it succeeded or failed.
 *
 * Store IMEI in cache as per context. Read IMEI NV from ctx-0 and save in cached index-0
 * Read IMEI NV from ctx-1 and save in cached index - 1.
 * While filling the IMEI in return ptr copy from mapped ctx. Example: AS IDs are mapped as
 * mm_nv_context_id[as_id-0] = 1, mm_nv_context_id[as_id-1] = 0.
 * Then return Cached IMEI[1] when queried for as_id - 0. And other one for as_id - 1.
 */
 ghdi_status_T ghdi_ds_read_imei(byte *imei_ptr,sys_modem_as_id_e_type as_id)
 {

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
   mmgsdi_session_info_query_type session_info_query;
   mmgsdi_session_get_info_type session_get_info_ptr;
   mmgsdi_return_enum_type status = MMGSDI_ERROR;

#if defined(FEATURE_TRIPLE_SIM)
static byte cached_imei_valid_sim[MAX_AS_IDS] ={ FALSE,FALSE,FALSE};
#else
static byte cached_imei_valid_sim[MAX_AS_IDS] ={ FALSE,FALSE};
#endif
#endif


#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
  static byte cached_imei_sim[MAX_AS_IDS][NV_UE_IMEI_SIZE];

  /* read the imei into a local buffer and then copy into the imei byte array */
  nv_item_type itemBuffer;
  ghdi_status_T result = GHDI_FAILURE;
  dword index;
  /* flag to see if the IMEI has been cached */
  boolean is_cached = FALSE;
  uint8 context_id = 0;

  NAS_ASSERT_UT(imei_ptr != NULL);

  session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
  session_info_query.query_key.session_type = mm_sim_mmgsdi_info_sim[as_id].session_type;

  if (cached_slot_id_valid_imei_sim[as_id] == FALSE)
  {
    status = mmgsdi_session_get_info_sync(session_info_query, &session_get_info_ptr);
    if (status == MMGSDI_SUCCESS)
    {
      cached_slot_id_imei_sim[as_id] = session_get_info_ptr.slot_id-1;
      cached_slot_id_valid_imei_sim[as_id] = TRUE;
    }
  }

  rex_enter_crit_sect(&nas_nv_crit_sect);
  if (cached_slot_id_valid_imei_sim[as_id] == TRUE)
  {
    context_id =  cached_slot_id_imei_sim[as_id];

    if(cached_imei_mapping[as_id] != context_id)
    {
      mm_as_id_e_type as_id_local;
      for (as_id_local = MM_AS_ID_1 ; as_id_local < (mm_as_id_e_type)MAX_AS_IDS; as_id_local++)
      {
         if(cached_imei_mapping[as_id_local] == context_id)
         {
            MSG_HIGH_DS(as_id,"=IMEI= Mapping Changed between contexts %d, %d, for context %d",as_id_local,as_id,context_id);
            cached_imei_mapping[as_id_local] = cached_imei_mapping[as_id];
            cached_imei_mapping[as_id] = context_id;
            break;
         }
      }
    }
  }
  else
  {
    context_id = cached_imei_mapping[as_id];
  }
  is_cached = cached_imei_valid_sim[context_id];
  rex_leave_crit_sect(&nas_nv_crit_sect);



  /* if the IMEI is not yet cached */
  if (is_cached == FALSE)
  {
    /* read the IMEI from NV */
    result = ghdi_nvmem_read_per_subs((ghdi_nvmem_parameter_id_T) NV_UE_IMEI_I,&itemBuffer,(uint16)context_id);

    /*************************************************************************************/
    /* if the result is anything other than success, it must be handled depending on the */
    /* result                                                                            */
    /*************************************************************************************/
    if ((result == GHDI_NOT_INITIALISED)|| (result == GHDI_NOT_REGISTERED))
    {
      ERR("NVRead IMEI failed=%d, use default",(int) result,0,0);
      /* provide a default value for the IMEI */
      /* if NV is not enabled, just use the hard coded values */
      /* Default IMEI is = 00440015202000 */
      imei_ptr[0] = 0x08;   /* len */
      imei_ptr[1] = 0x0A;   /* BCD1 / IMEI identifier is always 0xA */
      /* digits p and p+1 are placed with p in low nibble and p+1 in high nibble */
      imei_ptr[2] = 0x40;   /* BCD3 / BCD2 */
      imei_ptr[3] = 0x04;   /* BCD5 / BCD4 */
      imei_ptr[4] = 0x10;
      imei_ptr[5] = 0x25;
      imei_ptr[6] = 0x20;
      imei_ptr[7] = 0x00;
      imei_ptr[8] = 0x00;   /* spare digit in high nibble is always 0 */

      /* if the result indicates that the task was not registered */
      if (result == GHDI_NOT_REGISTERED)
      {
        /* just provide the default value but don't cache it */
        return result;
      }
    } /* if (result == GHDI_NOT_INITIALISED) || (result == GHDI_NOT_REGISTERED) */
    else if(result != GHDI_SUCCESS)
    {
      ERR_FATAL("NVRead IMEI failed=%d",(int) result,0,0);
    }

    // cache the IMEI value as an atomic operation (in case multiple tasks are here)
    rex_enter_crit_sect(&nas_nv_crit_sect);
    if (cached_imei_valid_sim[context_id] == FALSE)
    {
      /* If NV read was not successful because the item was INACTIVE  etc. we want to
         cache the default value and not the value read from NV */
       if(result == GHDI_SUCCESS )
       {
         /* copy whatever is in the item to the cache */
         for (index=0;index < NV_UE_IMEI_SIZE; index++)
         {
            cached_imei_sim[context_id][index] = itemBuffer.ue_imei.ue_imei[index];
         }

       }
       else
       {
         for (index=0;index < NV_UE_IMEI_SIZE; index++)
         {
            cached_imei_sim[context_id][index] = imei_ptr[index];
         }

       }
       // now the cache should be used
       cached_imei_valid_sim[context_id] = TRUE;
    }
    // otherwise, someone else cached ahead, just use cached value below
    rex_leave_crit_sect(&nas_nv_crit_sect);
  }
  // otherwise, imei value is already cached so don't have to read from NV
  else
  {
    result = GHDI_SUCCESS;
  }

  // copy the cached IMEI value
  for (index=0;index < NV_UE_IMEI_SIZE; index++)
  {
    imei_ptr[index] = cached_imei_sim[context_id][index];
  }

  /* if the IMEI length is > 8, change it to 8 */
  if (imei_ptr[0] > 8)
  {
    imei_ptr[0] = 8;
  }

  return result;
#else
  return ghdi_read_imei(imei_ptr);
#endif
 }
void ghdi_clear_slot_mapping(sys_modem_as_id_e_type as_id)
{
  cached_slot_id_valid_imei_sim[as_id] = FALSE;
}
#endif
ghdi_status_T ghdi_read_imei(byte *  imei_ptr)
{
  static byte cached_imei[NV_UE_IMEI_SIZE];
  static byte cached_imei_valid = FALSE;
  /* read the imei into a local buffer and then copy into the imei byte array */
  nv_item_type itemBuffer;
  ghdi_status_T result = GHDI_FAILURE;
  dword index;
  /* flag to see if the IMEI has been cached */
  boolean is_cached = FALSE;

  NAS_ASSERT_UT(imei_ptr != NULL);

  rex_enter_crit_sect(&nas_nv_crit_sect);
  is_cached = cached_imei_valid;
  rex_leave_crit_sect(&nas_nv_crit_sect);

  /* if the IMEI is not yet cached */
  if (is_cached == FALSE)
  {
    /* read the IMEI from NV */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
    ERR("when FEATURE_DUAL_IMEI is enabled this function would not have been called",0,0,0);
    result = ghdi_nvmem_read_per_subs((ghdi_nvmem_parameter_id_T) NV_UE_IMEI_I,&itemBuffer,cached_imei_mapping[SYS_MODEM_AS_ID_1]);
#else
    result = ghdi_nvmem_read((ghdi_nvmem_parameter_id_T) NV_UE_IMEI_I,&itemBuffer);
#endif

    /*************************************************************************************/
    /* if the result is anything other than success, it must be handled depending on the */
    /* result                                                                            */
    /*************************************************************************************/
    if ((result == GHDI_NOT_INITIALISED)||(result == GHDI_NOT_REGISTERED))
    {
      ERR("NVRead IMEI failed=%d, use default",(int) result,0,0);
      /* provide a default value for the IMEI */
      /* if NV is not enabled, just use the hard coded values */
      /* Default IMEI is = 00440015202000 */
      imei_ptr[0] = 0x08;   /* len */
      imei_ptr[1] = 0x0A;   /* BCD1 / IMEI identifier is always 0xA */
      /* digits p and p+1 are placed with p in low nibble and p+1 in high nibble */
      imei_ptr[2] = 0x40;   /* BCD3 / BCD2 */
      imei_ptr[3] = 0x04;   /* BCD5 / BCD4 */
      imei_ptr[4] = 0x10;
      imei_ptr[5] = 0x25;
      imei_ptr[6] = 0x20;
      imei_ptr[7] = 0x00;
      imei_ptr[8] = 0x00;   /* spare digit in high nibble is always 0 */

      /* if the result indicates that the task was not registered */
      if (result == GHDI_NOT_REGISTERED)
      {
        /* just provide the default value but don't cache it */
        return result;
      }

    } /* if (result == GHDI_NOT_INITIALISED) || (result == GHDI_NOT_REGISTERED) */
    else if(result != GHDI_SUCCESS)
    {
      ERR_FATAL("NVRead IMEI failed=%d",(int) result,0,0);
    }

    // cache the IMEI value as an atomic operation (in case multiple tasks are here)
    rex_enter_crit_sect(&nas_nv_crit_sect);
    if (cached_imei_valid == FALSE)
    {
      /* If NV read was not successful because the item was INACTIVE  etc. we want to
         cache the default value and not the value read from NV */
       if(result == GHDI_SUCCESS )
       {
         /* copy whatever is in the item to the cache */
         for (index=0;index < NV_UE_IMEI_SIZE; index++)
         {
            cached_imei[index] = itemBuffer.ue_imei.ue_imei[index];
         }

       }
       else
       {
         for (index=0;index < NV_UE_IMEI_SIZE; index++)
         {
            cached_imei[index] = imei_ptr[index];
         }

       }
       // now the cache should be used
       cached_imei_valid = TRUE;
    }
    // otherwise, someone else cached ahead, just use cached value below
    rex_leave_crit_sect(&nas_nv_crit_sect);
  }
  // otherwise, imei value is already cached so don't have to read from NV
  else
  {
    result = GHDI_SUCCESS;
  }

  // copy the cached IMEI value
  for (index=0;index < NV_UE_IMEI_SIZE; index++)
  {
    imei_ptr[index] = cached_imei[index];
  }

  /* if the IMEI length is > 8, change it to 8 */
  if (imei_ptr[0] > 8)
  {
    imei_ptr[0] = 8;
  }

  return result;
} /* ghdi_read_imei */

/*
 *   Function name:  ghdi_read_hw_classmark
 *   --------------
 *
 *   Description:
 *   ------------
 *
 *
 *   Parameters:
 *   -----------
 *
 *
 *
 *   Returns:
 *   --------
 *
 *
 */

/* set of masks to extract the A5_X support information from the bitmap retrieved from NV */
#define A5_1_SUPPORTED_MASK 0x1
#define A5_2_SUPPORTED_MASK 0x2
#define A5_3_SUPPORTED_MASK 0x4
#define A5_4_SUPPORTED_MASK 0x8
#define A5_5_SUPPORTED_MASK 0x10
#define A5_6_SUPPORTED_MASK 0x20
#define A5_7_SUPPORTED_MASK 0x40
#define IS_ALGO_SUPPORTED(x) ((x)?TRUE :FALSE)

ghdi_status_T ghdi_read_hw_classmark(hw_classmark_T *hw_classmark)
{
  /* Number of times NVMEM is accessed before cached values are hardcoded to avoid NV read */
  #define NV_READ_RETRIES 3

  static hw_classmark_T cached_hw_classmark;
  static boolean cached_hw_classmark_valid = FALSE;
  static byte failure_count = 0;
  /*-------------------------------------------------------------------
  ** the hw_classmark is composed of several items, in this release
  ** only the A5_X part of the hw_classmark structure is available
  ** in NV
  **-----------------------------------------------------------------*/
  /* read the A5 algorithm support into a local buffer and then copy into the
    classmark */
  nv_item_type itemBuffer;
  ghdi_status_T result;
  boolean is_cached = FALSE;

  rex_enter_crit_sect(&nas_nv_crit_sect);
  is_cached = cached_hw_classmark_valid;
  rex_leave_crit_sect(&nas_nv_crit_sect);
  NAS_ASSERT_UT(hw_classmark != NULL);

  if (is_cached == FALSE)
  {
    result = ghdi_nvmem_read((ghdi_nvmem_parameter_id_T) NV_GSM_A5_ALGORITHMS_SUPPORTED_I,&itemBuffer);

    // if the result is anything other than success, just exit with some defaults
    if (result != GHDI_SUCCESS)
    {
      ERR("NVRead HWClassmark failed=%d",(int) result,0,0);
      /* just use the hard coded values since NVMEM is not available */
      hw_classmark->A5_1_supported = TRUE;
      hw_classmark->A5_2_supported = FALSE; /* weak algorithm, not supported */
      hw_classmark->A5_3_supported = TRUE;
      hw_classmark->A5_4_supported = FALSE;
      hw_classmark->A5_5_supported = FALSE;
      hw_classmark->A5_6_supported = FALSE;
      hw_classmark->A5_7_supported = FALSE;
      hw_classmark->freq_capability = 0;
      hw_classmark->default_rf_power_capability = (byte) CLASS_4;
      hw_classmark->current_rf_power_capability = (byte) CLASS_4;

      if (failure_count++ > NV_READ_RETRIES)
      {
        // store the hardcoded value as cached value
        rex_enter_crit_sect(&nas_nv_crit_sect);
        if (cached_hw_classmark_valid == FALSE)
        {
          cached_hw_classmark = (*hw_classmark);
          // now the cache should be used
          cached_hw_classmark_valid = TRUE;
        }
        // otherwise, someone else cached ahead, just use cached value below
        rex_leave_crit_sect(&nas_nv_crit_sect);
      }

      /* As we have now substituted valid hardcoded values we need to return success. */
      return GHDI_SUCCESS;
    }

    // cache the HW_CLASSMARK value as an atomic operation (in case multiple tasks are here)
    rex_enter_crit_sect(&nas_nv_crit_sect);
    if (cached_hw_classmark_valid == FALSE)
    {
      /* copy whatever is in the item to the cache */
      cached_hw_classmark.A5_1_supported = IS_ALGO_SUPPORTED(itemBuffer.gsm_a5_algorithms_supported & A5_1_SUPPORTED_MASK);
      cached_hw_classmark.A5_2_supported = IS_ALGO_SUPPORTED(itemBuffer.gsm_a5_algorithms_supported & A5_2_SUPPORTED_MASK);
      cached_hw_classmark.A5_3_supported = IS_ALGO_SUPPORTED(itemBuffer.gsm_a5_algorithms_supported & A5_3_SUPPORTED_MASK);
      cached_hw_classmark.A5_4_supported = IS_ALGO_SUPPORTED(itemBuffer.gsm_a5_algorithms_supported & A5_4_SUPPORTED_MASK);
      cached_hw_classmark.A5_5_supported = IS_ALGO_SUPPORTED(itemBuffer.gsm_a5_algorithms_supported & A5_5_SUPPORTED_MASK);
      cached_hw_classmark.A5_6_supported = IS_ALGO_SUPPORTED(itemBuffer.gsm_a5_algorithms_supported & A5_6_SUPPORTED_MASK);
      cached_hw_classmark.A5_7_supported = IS_ALGO_SUPPORTED(itemBuffer.gsm_a5_algorithms_supported & A5_7_SUPPORTED_MASK);

      /* the other values remain hard coded for now */
      cached_hw_classmark.freq_capability = 0;
      cached_hw_classmark.default_rf_power_capability = (byte) CLASS_4;
      cached_hw_classmark.current_rf_power_capability = (byte) CLASS_4;

      // now the cache should be used
      cached_hw_classmark_valid = TRUE;
    }
    // otherwise, someone else cached ahead, just use cached value below
    rex_leave_crit_sect(&nas_nv_crit_sect);
  }
  // otherwise, value is already cached so don't have to read from NV

  /* now set the hw_classmark values based on the A5 item information */
  hw_classmark->A5_1_supported = cached_hw_classmark.A5_1_supported;
  hw_classmark->A5_2_supported = cached_hw_classmark.A5_2_supported;
  hw_classmark->A5_3_supported = cached_hw_classmark.A5_3_supported;
  hw_classmark->A5_4_supported = cached_hw_classmark.A5_4_supported;
  hw_classmark->A5_5_supported = cached_hw_classmark.A5_5_supported;
  hw_classmark->A5_6_supported = cached_hw_classmark.A5_6_supported;
  hw_classmark->A5_7_supported = cached_hw_classmark.A5_7_supported;
  hw_classmark->freq_capability = cached_hw_classmark.freq_capability;
  hw_classmark->default_rf_power_capability = cached_hw_classmark.default_rf_power_capability;
  hw_classmark->current_rf_power_capability = cached_hw_classmark.current_rf_power_capability;

  /* As we have now substituted valid hardcoded values we need to return success. */
  return GHDI_SUCCESS;
  
}

ghdi_status_T ghdi_read_ue_gsm_op_class(ghdi_ue_op_class_T *gsm_op_class)
{
#ifdef FEATURE_NAS_NVMEM_UE_GSM_OP_CLASS
  nv_item_type itemBuffer;
  ghdi_status_T result;

  NAS_ASSERT_UT(gsm_op_class != NULL);
  result = ghdi_nvmem_read((ghdi_nvmem_parameter_id_T) NV_GSM_UE_OP_CLASS_I,&itemBuffer);

  /* copy whatever is in the item even if it is not a valid value */
  *gsm_op_class = itemBuffer.gsm_ue_op_class;

  return result;
#else

  /* NVMEM is not enabled, so use hard-coded values for now */
  *gsm_op_class = ue_op_class_c;

  return GHDI_SUCCESS;
#endif /* FEATURE_NAS_NVMEM_UE_GSM_OP_CLASS */
}

ghdi_status_T ghdi_read_ue_umts_op_class(ghdi_ue_op_class_T *umts_op_class)
{

#ifdef FEATURE_NAS_NVMEM_UE_UMTS_OP_CLASS
  nv_item_type itemBuffer;
  ghdi_status_T result;

  NAS_ASSERT_UT(umts_op_class != NULL);
  result = ghdi_nvmem_read((ghdi_nvmem_parameter_id_T) NV_UMTS_UE_OP_CLASS_I,&itemBuffer);

  /* copy whatever is in the item even if it is not a valid value */
  *umts_op_class = itemBuffer.umts_ue_op_class;

  return result;
#else

  /* NVMEM is not enabled, so use hard-coded values for now */

  *umts_op_class = ue_op_class_c;

  return GHDI_SUCCESS;
#endif /* FEATURE_NAS_NVMEM_UE_UMTS_OP_CLASS */
}

ghdi_status_T ghdi_read_ue_msrac_sms_value(ghdi_ue_msrac_sms_value_T *ue_msrac_sms_value)
{
#ifdef FEATURE_NAS_NVMEM_UE_MSRAC_SMS_VALUE
  nv_item_type itemBuffer;
  ghdi_status_T result;

  NAS_ASSERT_UT(ue_msrac_sms_value != NULL);
  result = ghdi_nvmem_read((ghdi_nvmem_parameter_id_T) NV_MSRAC_SMS_VALUE_I,&itemBuffer);

  /* copy whatever is in the item even if it is not a valid value */
  *ue_msrac_sms_value = itemBuffer.msrac_sms_value;

  return result;
#else

  /* NVMEM is not enabled, so use hard-coded values for now */
  *ue_msrac_sms_value = 0;

  return GHDI_SUCCESS;
#endif /* FEATURE_NAS_NVMEM_UE_MSRAC_SMS_VALUE */
}

ghdi_status_T ghdi_read_ue_msrac_sm_value(ghdi_ue_msrac_sm_value_T *ue_msrac_sm_value)
{
#ifdef FEATURE_NAS_NVMEM_UE_MSRAC_SM_VALUE
  nv_item_type itemBuffer;
  ghdi_status_T result;

  NAS_ASSERT_UT(ue_msrac_sm_value != NULL);
  result = ghdi_nvmem_read((ghdi_nvmem_parameter_id_T) NV_MSRAC_SM_VALUE_I,&itemBuffer);

  /* copy whatever is in the item even if it is not a valid value */
  *ue_msrac_sm_value = itemBuffer.msrac_sm_value;

  return result;
#else

  /* NVMEM is not enabled, so use hard-coded values for now */
  *ue_msrac_sm_value = 0;

  return GHDI_SUCCESS;
#endif /* FEATURE_NAS_NVMEM_UE_MSRAC_SM_VALUE */
}

/*******************************************************************************
 *
 *  Function name: read_imeisv()
 *  ----------------------------
 *
 *  Description:
 *  ------------
 *  Function to get the IMEISV
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
 void ghdi_ds_read_imeisv(byte *imeisv,sys_modem_as_id_e_type as_id)
 {
  byte index;
  byte digit1;
  byte digit2;
  byte imei[9];
  byte identity_length;
  ghdi_status_T status;

  static byte cached_imeisv_svn_sim[MAX_AS_IDS];
  mmgsdi_session_info_query_type session_info_query;
  mmgsdi_session_get_info_type session_get_info_ptr;

  #if defined(FEATURE_TRIPLE_SIM)
  static byte cached_imeisv_svn_valid_sim[MAX_AS_IDS] = {FALSE,FALSE,FALSE};
  #else
  static byte cached_imeisv_svn_valid_sim[MAX_AS_IDS] = {FALSE,FALSE};
  #endif
    
  /* read the imeisv_svn into a local buffer and then copy into the static imeisv_svn */
  nv_item_type local_imeisv_svn;
  ghdi_status_T result = GHDI_FAILURE;
  /* flag to see if the IMEI has been cached */
  boolean is_cached = FALSE;

  uint8 context_id = 0;

  NAS_ASSERT_UT(imeisv != NULL);

  session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
  session_info_query.query_key.session_type = mm_sim_mmgsdi_info_sim[as_id].session_type;

  rex_enter_crit_sect(&nas_nv_crit_sect);
  if (MMGSDI_SUCCESS == mmgsdi_session_get_info_sync(session_info_query, &session_get_info_ptr))
  {
    context_id = session_get_info_ptr.slot_id-1;

    if(cached_imei_mapping[as_id] != context_id)
    {
      mm_as_id_e_type as_id_local;
      for (as_id_local = MM_AS_ID_1 ; as_id_local < (mm_as_id_e_type)MAX_AS_IDS; as_id_local++)
      {
         if(cached_imei_mapping[as_id_local] == context_id)
         {
            MSG_HIGH_DS(as_id,"=IMEI= Mapping Changed between contexts %d, %d, for context %d",as_id_local,as_id,context_id);
            cached_imei_mapping[as_id_local] = cached_imei_mapping[as_id];
            cached_imei_mapping[as_id] = context_id;
            break;
         }
      }
    }
  }
  else
  {
    context_id = cached_imei_mapping[as_id];
  }
  is_cached = cached_imeisv_svn_valid_sim[context_id];
  rex_leave_crit_sect(&nas_nv_crit_sect);

  /* if the IMEISV_SVN is not yet cached */
  if (is_cached == FALSE)
  {
    /* read the IMEI from NV */
   result = ghdi_nvmem_read_per_subs((ghdi_nvmem_parameter_id_T) NV_UE_IMEISV_SVN_I,&local_imeisv_svn,(uint16)context_id);

    /*************************************************************************************/
    /* if the result is anything other than success, it must be handled  */
    /*************************************************************************************/
    if (result != GHDI_SUCCESS)
    {
      ERR("NVRead IMEISV_SVN failed=%d, use default",(int) result,0,0);
      /* provide a default value for the IMEISV_SVN */
      /* if NV is not enabled, just use the hard coded values */
      /* Default IMEISV_SVN is = 0 */
      local_imeisv_svn.ue_imeisv_svn = SVN;

      /* cache the default value so NV won't be retried */
      /* intentional fallthrough */
    } /* if result != GHDI_SUCCESS */
    else if (local_imeisv_svn.ue_imeisv_svn > 98)
    {
      /* IMEISV_SVN cannot be > 98, if it was wrongly set, use Default value */    
      ERR("NV IMEISV_SVN %d > 98 using default value 0",local_imeisv_svn.ue_imeisv_svn,0,0);
      local_imeisv_svn.ue_imeisv_svn = SVN;    
    }

    // cache the IMEI value as an atomic operation (in case multiple tasks are here)
    rex_enter_crit_sect(&nas_nv_crit_sect);
    if (cached_imeisv_svn_valid_sim[context_id] == FALSE)
    {
       /* copy whatever is in the local to the cache */
       cached_imeisv_svn_sim[context_id] = local_imeisv_svn.ue_imeisv_svn;

       // now the cache should be used
       cached_imeisv_svn_valid_sim[context_id]  = TRUE;
    }
    // otherwise, someone else cached ahead, just use cached value below
    rex_leave_crit_sect(&nas_nv_crit_sect);
  }
  else
  {
    local_imeisv_svn.ue_imeisv_svn = cached_imeisv_svn_sim[context_id];
  }

  // local_imeisv_svn.ue_imeisv_svn now contains either the NV IMEISV_SVN or the default IMEISV_SVN
  status = ghdi_ds_read_imei(imei,as_id);

  if (status!=GHDI_SUCCESS)
  {
    ERR("Unable to read IMEI from NV",0,0,0);
  }

  /***************************/
  /* Get length of imeisv IE */
  /***************************/

  identity_length = imei[0] + 1;

  /***********************************************/
  /* The IMEISV has 16 digits while the IMEI has */
  /* 15 digits                                   */
  /***********************************************/

  imeisv[0] = identity_length;

  for (index = 1; index < identity_length; index++)
  {
    imeisv[index] = imei[index];
  }

  /************************************************/
  /* The number of digits is now even and not odd */
  /* so clear the odd/even bit to indicate even   */
  /************************************************/

  imeisv[1] = imeisv[1] & 0xF7;

  /**************************************************/
  /* The type of the identity is now IMEISV and not */
  /* IMEI                                           */
  /**************************************************/

  imeisv[1] = imeisv[1] | 0x01;

  /******************************************/
  /* Add the SVN (Software Version Number)  */
  /******************************************/

  digit1 = local_imeisv_svn.ue_imeisv_svn / 10;
  digit1 = (byte) digit1 << 4;

  imeisv[8] &= 0x0f;
  imeisv[8] |= digit1;

  digit2 = local_imeisv_svn.ue_imeisv_svn % 10;
  imeisv[9] = digit2;
  imeisv[9] |= 0xF0;      /* set top nibble to all 1's */
 }
 #endif 
VOID_FUNC read_imeisv(byte *imeisv)
{
  byte index;
  byte digit1;
  byte digit2;
  byte imei[9];
  byte identity_length;
  ghdi_status_T status;

  static byte cached_imeisv_svn;
  static byte cached_imeisv_svn_valid = FALSE;
  /* read the imeisv_svn into a local buffer and then copy into the static imeisv_svn */
  nv_item_type local_imeisv_svn;
  ghdi_status_T result = GHDI_FAILURE;
  /* flag to see if the IMEI has been cached */
  boolean is_cached = FALSE;

  NAS_ASSERT_UT(imeisv != NULL);

  rex_enter_crit_sect(&nas_nv_crit_sect);
  is_cached = cached_imeisv_svn_valid;
  rex_leave_crit_sect(&nas_nv_crit_sect);

  /* if the IMEISV_SVN is not yet cached */
  if (is_cached == FALSE)
  {
    /* read the IMEI from NV */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
   ERR("when FEATURE_DUAL_IMEI is enabled this function would not have been called",0,0,0);
   result = ghdi_nvmem_read_per_subs((ghdi_nvmem_parameter_id_T) NV_UE_IMEISV_SVN_I,
                  &local_imeisv_svn,cached_imei_mapping[SYS_MODEM_AS_ID_1]);
#else
    result = ghdi_nvmem_read((ghdi_nvmem_parameter_id_T) NV_UE_IMEISV_SVN_I,
            (ghdi_nvmem_data_T *)&local_imeisv_svn);
#endif

    /*************************************************************************************/
    /* if the result is anything other than success, it must be handled  */
    /*************************************************************************************/
    if (result != GHDI_SUCCESS)
    {
      ERR("NVRead IMEISV_SVN failed=%d, use default",(int) result,0,0);
      /* provide a default value for the IMEISV_SVN */
      /* if NV is not enabled, just use the hard coded values */
      /* Default IMEISV_SVN is = 0 */
      local_imeisv_svn.ue_imeisv_svn = SVN;

      /* cache the default value so NV won't be retried */
      /* intentional fallthrough */
    } /* if result != GHDI_SUCCESS */
    else if (local_imeisv_svn.ue_imeisv_svn > 98)
    {
      /* IMEISV_SVN cannot be > 98, if it was wrongly set, use Default value */    
      ERR("NV IMEISV_SVN %d > 98 using default value 0",local_imeisv_svn.ue_imeisv_svn,0,0);
      local_imeisv_svn.ue_imeisv_svn = SVN;    
    }

    // cache the IMEI value as an atomic operation (in case multiple tasks are here)
    rex_enter_crit_sect(&nas_nv_crit_sect);
    if (cached_imeisv_svn_valid == FALSE)
    {
       /* copy whatever is in the local to the cache */
       cached_imeisv_svn = local_imeisv_svn.ue_imeisv_svn;

       // now the cache should be used
       cached_imeisv_svn_valid = TRUE;
    }
    // otherwise, someone else cached ahead, just use cached value below
    rex_leave_crit_sect(&nas_nv_crit_sect);
  }
  else
  {
    local_imeisv_svn.ue_imeisv_svn = cached_imeisv_svn;
  }

  // local_imeisv_svn.ue_imeisv_svn now contains either the NV IMEISV_SVN or the default IMEISV_SVN
  status = ghdi_read_imei(imei);

  if (status!=GHDI_SUCCESS)
  {
    ERR("Unable to read IMEI from NV",0,0,0);
  }

  /***************************/
  /* Get length of imeisv IE */
  /***************************/

  identity_length = imei[0] + 1;

  /***********************************************/
  /* The IMEISV has 16 digits while the IMEI has */
  /* 15 digits                                   */
  /***********************************************/

  imeisv[0] = identity_length;

  for (index = 1; index < identity_length; index++)
  {
    imeisv[index] = imei[index];
  }

  /************************************************/
  /* The number of digits is now even and not odd */
  /* so clear the odd/even bit to indicate even   */
  /************************************************/

  imeisv[1] = imeisv[1] & 0xF7;

  /**************************************************/
  /* The type of the identity is now IMEISV and not */
  /* IMEI                                           */
  /**************************************************/

  imeisv[1] = imeisv[1] | 0x01;

  /******************************************/
  /* Add the SVN (Software Version Number)  */
  /******************************************/

  digit1 = local_imeisv_svn.ue_imeisv_svn / 10;
  digit1 = (byte) (digit1 << 4);

  imeisv[8] &= 0x0f;
  imeisv[8] |= digit1;

  digit2 = local_imeisv_svn.ue_imeisv_svn % 10;
  imeisv[9] = digit2;
  imeisv[9] |= 0xF0;      /* set top nibble to all 1's */
}

/*-------------------------------------------------------------------------------------------
** maximum number of tasks that can be registered in NVMEM module. Since open addressing
** hash table is being used, it is important that this number be PRIME and about 1.5 times
** larger than the maximum number of expected tasks
**-----------------------------------------------------------------------------------------*/
#define GHDI_NVMEM_MAX_TASKS 53

/*----------------------------------------------------------------------------------------
** this is the hashing function used for primary key based on the (unsigned long) numeric
** value of the given key
**--------------------------------------------------------------------------------------*/
#define GHDI_NVMEM_HASH_FN1(key,size) (((unsigned long) (key)) % (size))

/*----------------------------------------------------------------------
** this is the hashing function used for probing. It is guaranteed that
** all slots will be probed if this function returns a value smaller
** than 'size'
**----------------------------------------------------------------------*/
#define GHDI_NVMEM_HASH_FN2(key,size) ((((unsigned long) (key)) % ((size)-2)) + 1)

/*---------------------------------------------------------------------------------
** a static list of task registration information for NVMEM module as a hash table
**-------------------------------------------------------------------------------*/
static ghdi_nvmem_registered_task_info_T *ghdi_nvmem_registered_task_list[GHDI_NVMEM_MAX_TASKS] = {0};

/* set to true once nvmem task list has been initialized */
static dword ghdi_nvmem_initialized = 0;

/*-----------------------------------------------------------------
** static array maps NV return status to GHDI return status values
**---------------------------------------------------------------*/
static ghdi_status_T ghdi_nvmem_nv_status_map[(int) NV_NOTALLOC_S + 6] =
{
  GHDI_SUCCESS,        /* NV_DONE_S */
  GHDI_FAILURE,        /* NV_BUSY_S */
  GHDI_INVALID_DATA,   /* NV_BADCMD_S */
  GHDI_BUFFER_FULL,    /* NV_FULL_S */
  GHDI_FAILURE,        /* NV_FAIL_S */
  GHDI_NOT_INITIALISED,/* NV_NOTACTIVE_S */
  GHDI_INVALID_DATA,   /* NV_BADPARM_S */
  GHDI_READONLY,       /* NV_READONLY_S */
  GHDI_FAILURE,        /* NV_BADTG_S */
  GHDI_BUFFER_FULL,    /* NV_NOMEM_S */
  GHDI_INVALID_DATA,   /* NV_NOTALLOC_S */
  GHDI_FAILURE,
  GHDI_FAILURE,
  GHDI_FAILURE,
  GHDI_FAILURE,
  GHDI_FAILURE,
};

/*-----------------------------------------------------------------------------------------
** maps a NV return status to GHDI return status using map... assumes that NV_NOTALLOC_S is
** the largest status value returned by NV module
**-----------------------------------------------------------------------------------------*/
#define GHDI_NVMEM_MAP_STATUS(nvStatus) (((nvStatus) <= NV_NOTALLOC_S) ? ghdi_nvmem_nv_status_map[(nvStatus)] : GHDI_FAILURE)

/*==============================================================================
FUNCTION ghdi_nvmem_initialize

DESCRIPTION
  Initializes the NVMEM module

PARAMETERS

RETURN VALUE
  None

DEPENDENCIES
  None

=================================================================================*/

void ghdi_nvmem_initialize()
{
  boolean initRequired = 0;
  rex_enter_crit_sect(&nas_nv_crit_sect);
  if (!ghdi_nvmem_initialized)
  {
    initRequired = 1;
    ghdi_nvmem_initialized = 1;
  }
  rex_leave_crit_sect(&nas_nv_crit_sect);

  if (initRequired)
  {
    /* not really required */
    memset(ghdi_nvmem_registered_task_list,0,sizeof(ghdi_nvmem_registered_task_list));

    /*-----------------------------------------------------------------------------
    ** verify the mapping between NV return values and GHDI status values
    **---------------------------------------------------------------------------*/

    NAS_ASSERT_UT(NV_DONE_S < NV_NOTALLOC_S);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_DONE_S] == GHDI_SUCCESS);
    NAS_ASSERT_UT(NV_BUSY_S < NV_NOTALLOC_S);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_BUSY_S] == GHDI_FAILURE);
    NAS_ASSERT_UT(NV_BADCMD_S < NV_NOTALLOC_S);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_BADCMD_S] == GHDI_INVALID_DATA);
    NAS_ASSERT_UT(NV_FULL_S < NV_NOTALLOC_S);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_FULL_S] == GHDI_BUFFER_FULL);
    NAS_ASSERT_UT(NV_FAIL_S < NV_NOTALLOC_S);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_FAIL_S] == GHDI_FAILURE);
    NAS_ASSERT_UT(NV_NOTACTIVE_S < NV_NOTALLOC_S);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_NOTACTIVE_S] == GHDI_NOT_INITIALISED);
    NAS_ASSERT_UT(NV_BADPARM_S < NV_NOTALLOC_S);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_BADPARM_S] == GHDI_INVALID_DATA);
    NAS_ASSERT_UT(NV_READONLY_S < NV_NOTALLOC_S);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_READONLY_S] == GHDI_READONLY);
    NAS_ASSERT_UT(NV_BADTG_S < NV_NOTALLOC_S);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_BADTG_S] == GHDI_FAILURE);
    NAS_ASSERT_UT(NV_NOMEM_S < NV_NOTALLOC_S);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_NOMEM_S] == GHDI_BUFFER_FULL);
    NAS_ASSERT_UT(ghdi_nvmem_nv_status_map[NV_NOTALLOC_S] == GHDI_INVALID_DATA);
  }
}

/*==============================================================================
FUNCTION ghdi_nvmem_register_task

DESCRIPTION
  Registers the calling task with the NVMEM module using open addressing hashing. The
  task's thread_id is used as the registration key. The nvmem module stores a pointer
  to the given taskInfo structure, so it is very important that the calling task allocate
  this storage in a way that will be available for the entire lifetime of the nvmem module
  (e.g., static global variable)

PARAMETERS
  taskInfoPtr     Pointer to a task information structure for the task being registered

RETURN VALUE
  GHDI_SUCCESS       if the task is successfully registered
  GHDI_BUFFER_FULL   if there is no more space to register tasks
  GHDI_FAILURE       other failures

DEPENDENCIES
  None

=================================================================================*/

ghdi_status_T ghdi_nvmem_register_task
(
  /* signal that should be used to notify task when NV operation is completed */
  ghdi_nvmem_signal_T                 signal,

  /* pointer to task specific wait function for this task (can be NULL) */
  ghdi_nvmem_wait_fn_ptr_T            taskWaitFnPtr,

  /* pointer to task specific default value generation function (can be NULL) */
  ghdi_nvmem_default_value_fn_ptr_T   taskDefaultGenFnPtr
)
{
  dword index;
  boolean done = 0;
  dword visitedSlotCount = 0;
  dword hkey1;
  dword hkey2;

#ifndef FEATURE_MODEM_HEAP
  ghdi_nvmem_registered_task_info_T *taskInfoPtr =
  (ghdi_nvmem_registered_task_info_T*)gs_alloc(sizeof(ghdi_nvmem_registered_task_info_T));
#else
  ghdi_nvmem_registered_task_info_T *taskInfoPtr =
  (ghdi_nvmem_registered_task_info_T*)modem_mem_calloc(1,sizeof(ghdi_nvmem_registered_task_info_T), MODEM_MEM_CLIENT_GSLIB); 
#endif
  
  /*--------------------------------------------------------------------------
  **    taskInfoPtr is a mandatory parameter which must be provided by caller
  **------------------------------------------------------------------------*/
  if (taskInfoPtr == NULL)
  {
    ERR_FATAL("Could not allocated memory", 0,0,0);
    /*lint -e{527} Reviewed with the process group and recommended for suppression */
    return GHDI_FAILURE;
  }

  taskInfoPtr->taskPtr = rex_self();
  taskInfoPtr->signal  = signal;
  taskInfoPtr->taskWaitFnPtr       = taskWaitFnPtr;
  taskInfoPtr->taskDefaultGenFnPtr = taskDefaultGenFnPtr;

  /* initialize if required */
  ghdi_nvmem_initialize();

  /* compute the hash keys using the hash functions... we use the task's thread id as the  */
#ifdef FEATURE_L4
  hkey1 = GHDI_NVMEM_HASH_FN1(L4_THREADID_RAW(taskInfoPtr->taskPtr->thread_id),GHDI_NVMEM_MAX_TASKS);
  hkey2 = GHDI_NVMEM_HASH_FN2(L4_THREADID_RAW(taskInfoPtr->taskPtr->thread_id),GHDI_NVMEM_MAX_TASKS);
#else
  hkey1 = GHDI_NVMEM_HASH_FN1(taskInfoPtr->taskPtr->thread_id,GHDI_NVMEM_MAX_TASKS);
  hkey2 = GHDI_NVMEM_HASH_FN2(taskInfoPtr->taskPtr->thread_id,GHDI_NVMEM_MAX_TASKS);
#endif

  /* loop until sure that a valid slot is found */
  while (!done)
  {
    index = hkey1;
    /* look for an open index to insert task info or to see if this task is already registered */
    for (visitedSlotCount = 0; visitedSlotCount < GHDI_NVMEM_MAX_TASKS; visitedSlotCount++)
    {
      NAS_ASSERT_UT(index < GHDI_NVMEM_MAX_TASKS);
      /* the taskPtr should be valid if the entry is valid */
      NAS_ASSERT_UT((ghdi_nvmem_registered_task_list[index] == NULL) || (ghdi_nvmem_registered_task_list[index]->taskPtr != NULL));
      if (ghdi_nvmem_registered_task_list[index] == NULL)
      {
        /* found open entry task is not yet registered */
        break;
      }
      else if ((ghdi_nvmem_registered_task_list[index]->taskPtr != NULL) &&
#ifdef FEATURE_L4
               (L4_THREADID_RAW(ghdi_nvmem_registered_task_list[index]->taskPtr->thread_id) == L4_THREADID_RAW(taskInfoPtr->taskPtr->thread_id)))
#else
               (ghdi_nvmem_registered_task_list[index]->taskPtr->thread_id == taskInfoPtr->taskPtr->thread_id))
#endif
      {
        /* found an existing entry for this task, use it */
        break;
      }
      /* look for the next available slot based on 2nd hash key */
      index = (index + hkey2) % GHDI_NVMEM_MAX_TASKS;
    }

    if (visitedSlotCount < GHDI_NVMEM_MAX_TASKS)
    {
      rex_enter_crit_sect(&nas_nv_crit_sect);
      /* check with INTLOCK to make sure that some other task didn't steal our spot */
      if ((ghdi_nvmem_registered_task_list[index] == NULL) ||
          ((ghdi_nvmem_registered_task_list[index]->taskPtr != NULL) &&
#ifdef FEATURE_L4
           (L4_THREADID_RAW(ghdi_nvmem_registered_task_list[index]->taskPtr->thread_id) == L4_THREADID_RAW(taskInfoPtr->taskPtr->thread_id)))
#else
           (ghdi_nvmem_registered_task_list[index]->taskPtr->thread_id == taskInfoPtr->taskPtr->thread_id))
#endif
         )
      {
        ghdi_nvmem_registered_task_list[index] = taskInfoPtr;
        done = 1;
      }
      rex_leave_crit_sect(&nas_nv_crit_sect);
      /* otherwise, some other task stole our spot, while loop will try again */
    }
    else
    {
      /* could not find an open spot! */
      return GHDI_BUFFER_FULL;
    }
  } /* while (!done) */

  /* successfully registered task */
  return GHDI_SUCCESS;
}

/*==============================================================================
FUNCTION module private: ghdi_nvmem_get_task_info

DESCRIPTION
  Returns NVMEM task information associated with the task

PARAMETERS
  taskPtr - Pointer to the task for which information is required. If NULL the calling task
            is used

RETURN VALUE
  Pointer to task information structure if task is registered
  NULL if task is not registered

DEPENDENCIES
  None

=================================================================================*/

ghdi_nvmem_registered_task_info_T * ghdi_nvmem_get_task_info(rex_tcb_type *taskPtr)
{
  dword index;
  boolean done = 0;
  dword visitedSlotCount = 0;
  dword hkey1;
  dword hkey2;
  boolean found = 0;

  if (taskPtr == NULL)
  {
    taskPtr = rex_self();
  }

  NAS_ASSERT_UT(taskPtr != NULL);
  /* compute the hash keys using the hash functions */
#ifdef FEATURE_L4
  hkey1 = GHDI_NVMEM_HASH_FN1(L4_THREADID_RAW(taskPtr->thread_id),GHDI_NVMEM_MAX_TASKS);
  hkey2 = GHDI_NVMEM_HASH_FN2(L4_THREADID_RAW(taskPtr->thread_id),GHDI_NVMEM_MAX_TASKS);
#else
  hkey1 = GHDI_NVMEM_HASH_FN1(taskPtr->thread_id,GHDI_NVMEM_MAX_TASKS);
  hkey2 = GHDI_NVMEM_HASH_FN2(taskPtr->thread_id,GHDI_NVMEM_MAX_TASKS);
#endif

  /* loop until sure that the requested task is found */
  while (!done)
  {
    found = 0;
    index = hkey1;

    /* search for entry matching our key in hash table */
    for (visitedSlotCount = 0; visitedSlotCount < GHDI_NVMEM_MAX_TASKS; visitedSlotCount++)
    {
      NAS_ASSERT_UT(index < GHDI_NVMEM_MAX_TASKS);
      /* the taskPtr should be valid if the entry is valid */
      NAS_ASSERT_UT((ghdi_nvmem_registered_task_list[index] == NULL) || (ghdi_nvmem_registered_task_list[index]->taskPtr != NULL));
      if (ghdi_nvmem_registered_task_list[index] == NULL)
      {
        /* found open entry, task is not registered */
        break;
      }
      else if ((ghdi_nvmem_registered_task_list[index]->taskPtr != NULL) &&
#ifdef FEATURE_L4
               (L4_THREADID_RAW(ghdi_nvmem_registered_task_list[index]->taskPtr->thread_id) == L4_THREADID_RAW(taskPtr->thread_id)))
#else
               (ghdi_nvmem_registered_task_list[index]->taskPtr->thread_id == taskPtr->thread_id))
#endif
      {
        /* found an existing entry for this task, use it */
        found = 1;
        break;
      }
      /* look in the next slot for key based on 2nd hash key */
      index = (index + hkey2) % GHDI_NVMEM_MAX_TASKS;
    }

    if (found)
    {
      rex_enter_crit_sect(&nas_nv_crit_sect);
      /* check with INTLOCK to make sure that some other task didn't modify our spot */
      if ((ghdi_nvmem_registered_task_list[index] == NULL) ||
          ((ghdi_nvmem_registered_task_list[index]->taskPtr != NULL) &&
#ifdef FEATURE_L4
           (L4_THREADID_RAW(ghdi_nvmem_registered_task_list[index]->taskPtr->thread_id) == L4_THREADID_RAW(taskPtr->thread_id))
#else
           (ghdi_nvmem_registered_task_list[index]->taskPtr->thread_id == taskPtr->thread_id)
#endif
          )
         )
      {
        done = 1;
      }
      rex_leave_crit_sect(&nas_nv_crit_sect);
      /* otherwise, some other task stole our spot, while loop will try again */

      if (done)
      {
        return ghdi_nvmem_registered_task_list[index];
      }
    }
    else
    {
      NAS_ASSERT_UT(visitedSlotCount >= GHDI_NVMEM_MAX_TASKS || ghdi_nvmem_registered_task_list[index] == NULL);
      /* could not find our task */
      return NULL;
    }
  } /* while (!done) */

  /* requested task was not found */
  return NULL;
}

/*==============================================================================
FUNCTION ghdi_nvmem_read

DESCRIPTION
  Reads a data item from NV memory and stores it in a caller provided buffer.
  Waits for the response from NV using registered information for the calling task.

PARAMETERS
  parameter_id    Id of the parameter being read from NV memory
  data_ptr        Pointer to caller allocated buffer where read item is stored

RETURN VALUE
  The NV return code mapped to the appropriate GHDI status value

DEPENDENCIES
  The routine is not reentrant for the same task. It is task-safe and can be
  called from different tasks
=================================================================================*/

ghdi_status_T ghdi_nvmem_read
(
  ghdi_nvmem_parameter_id_T           parameter_id,
  ghdi_nvmem_data_T                   *data_ptr
)
{
  rex_tcb_type *taskPtr = NULL;
  /* get the task information for calling task and call the helper */
  ghdi_nvmem_registered_task_info_T *taskInfo;
  ghdi_nvmem_status_T status;

  taskPtr = rex_self();
  taskInfo = ghdi_nvmem_get_task_info(taskPtr);

  if (taskInfo == NULL)
  {
    /* calling task has not been registered! */
    ERR("NVMEM error: calling task not registered with NVMEM module",0,0,0);
    return GHDI_NOT_REGISTERED;
  }

  status = ghdi_nvmem_read_imp
           (
           (nv_items_enum_type) parameter_id,
           (nv_item_type *) data_ptr,
           taskInfo->signal,
           taskInfo->taskWaitFnPtr,
           taskInfo->taskDefaultGenFnPtr
           );

  NAS_ASSERT_UT(status <= NV_NOTALLOC_S);
  return GHDI_NVMEM_MAP_STATUS(status);
}

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
ghdi_status_T ghdi_nvmem_read_per_subs
(
  ghdi_nvmem_parameter_id_T           parameter_id,
  ghdi_nvmem_data_T                   *data_ptr,
  uint16                              context_id
)
{
  rex_tcb_type *taskPtr = NULL;
  /* get the task information for calling task and call the helper */
  ghdi_nvmem_registered_task_info_T *taskInfo;
  ghdi_nvmem_status_T status;

  taskPtr = rex_self();
  taskInfo = ghdi_nvmem_get_task_info(taskPtr);

  if (taskInfo == NULL)
  {
    /* calling task has not been registered! */
    ERR("NVMEM error: calling task not registered with NVMEM module",0,0,0);
    return GHDI_NOT_REGISTERED;
  }

  status = ghdi_nvmem_read_imp_per_subs
           (
           (nv_items_enum_type) parameter_id,
           (nv_item_type *) data_ptr,
           taskInfo->signal,
           taskInfo->taskWaitFnPtr,
           taskInfo->taskDefaultGenFnPtr,
           context_id
           );

  NAS_ASSERT_UT(status <= NV_NOTALLOC_S);
  return GHDI_NVMEM_MAP_STATUS(status);
}
#endif

/*==============================================================================
FUNCTION ghdi_nvmem_read_imp

DESCRIPTION
  Reads a data item from NV memory and stores it in a caller provided buffer.
  Waits for the response from NV using a caller provided signal and optional
  wait function. Handles non-active items by providing a default value if caller
  supplies a default value generation function.

PARAMETERS
  parameter_id    Id of the parameter being read from NV memory
  data_ptr        Pointer to caller allocated buffer where read item is stored
  signal          Signal used to notify calling task that NV memory operation has
                  been completed
  waitfn_ptr      (Optional) pointer to a task specific wait function. Use NULL to
                  simply wait on the given signal
  defaultfn_ptr   (Optional) pointer to a task specific function that generates
                  default values for items that are not active

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled internally if
  a default value generation function is provided

DEPENDENCIES
  The routine is not reentrant for the same task. It is thread-safe and can be
  called from different tasks
=================================================================================*/

ghdi_nvmem_status_T ghdi_nvmem_read_imp
(
  nv_items_enum_type                  parameter_id,
  nv_item_type                        *data_ptr,
  ghdi_nvmem_signal_T                 signal,
  ghdi_nvmem_wait_fn_ptr_T            waitfn_ptr,
  ghdi_nvmem_default_value_fn_ptr_T   defaultfn_ptr
)
{
  nv_cmd_type nvi;
  rex_tcb_type *taskPtr;

  taskPtr = rex_self();

  /* store calling task (as provided by rex_self()) and associated signal
  as the task to notify when the operation is complete */
  nvi.tcb_ptr = taskPtr;
  nvi.sigs = signal;
  /* the command does not go on any queue when done */
  nvi.done_q_ptr = NULL;

  nvi.item = (nv_items_enum_type) parameter_id;
  nvi.cmd = NV_READ_F;

  /* the NV task will read the data into the caller provided buffer */
  nvi.data_ptr = (nv_item_type *) data_ptr;
  nvi.status = NV_BUSY_S;

  /* clear the notification signal from the calling task */
  (void) rex_clr_sigs(taskPtr,signal);

  /* call the NV interface function to queue the command */
  nv_cmd(&nvi);

  /* if the caller provided a task specific wait function, use it to wait for
  notification signal */
  if (waitfn_ptr != NULL)
  {
    (void) (*waitfn_ptr)(signal);
  }
  else
  {
    /* just wait for the specified signal */
    (void) rex_wait(signal);
  }

  /* check for premature exit from wait */
  if (nvi.status == NV_BUSY_S)
  {
    /*------------------------------------------------------------------------
    ** this is a problem, a premature exit from the task wait function,
    ** has the potential of causing stack corruption (should never happen if
    ** correct wait function is implemented)
    **----------------------------------------------------------------------*/
    ERR("Premature exit from task wait function while performing NVMEM operation",0,0,0);
  }

  /* if the caller provided a default value generation function */
  if (defaultfn_ptr != NULL)
  {
    /* call the caller supplied function to get the default value */
    nvi.status = (nv_stat_enum_type) (*defaultfn_ptr)(nvi.status,parameter_id,data_ptr);
  }

  if (nvi.status == NV_FAIL_S)
  {
    ERR("Error while attempting to read NVMEM item (%d)",(dword) parameter_id,0,0);
  }

  return nvi.status;
}

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 

ghdi_nvmem_status_T ghdi_nvmem_read_imp_per_subs
(
  nv_items_enum_type                  parameter_id,
  nv_item_type                        *data_ptr,
  ghdi_nvmem_signal_T                 signal,
  ghdi_nvmem_wait_fn_ptr_T            waitfn_ptr,
  ghdi_nvmem_default_value_fn_ptr_T   defaultfn_ptr,
  uint16                              context_id
)
{
  nv_cmd_type nvi;
  rex_tcb_type *taskPtr;

  nv_cmd_ext_type nvi_ext;
  nvi_ext.nvcmd = &nvi;
  nvi_ext.context = context_id;

  taskPtr = rex_self();

  /* store calling task (as provided by rex_self()) and associated signal
  as the task to notify when the operation is complete */
  nvi.tcb_ptr = taskPtr;
  nvi.sigs = signal;
  /* the command does not go on any queue when done */
  nvi.done_q_ptr = NULL;

  nvi.item = (nv_items_enum_type) parameter_id;
  nvi.cmd = NV_READ_F;

  /* the NV task will read the data into the caller provided buffer */
  nvi.data_ptr = (nv_item_type *) data_ptr;
  nvi.status = NV_BUSY_S;

  /* clear the notification signal from the calling task */
  (void) rex_clr_sigs(taskPtr,signal);

  /* call the NV interface function to queue the command */
  nv_cmd_ext(&nvi_ext);

  /* if the caller provided a task specific wait function, use it to wait for
  notification signal */
  if (waitfn_ptr != NULL)
  {
    (void) (*waitfn_ptr)(signal);
  }
  else
  {
    /* just wait for the specified signal */
    (void) rex_wait(signal);
  }

  /* check for premature exit from wait */
  if (nvi.status == NV_BUSY_S)
  {
    /*------------------------------------------------------------------------
    ** this is a problem, a premature exit from the task wait function,
    ** has the potential of causing stack corruption (should never happen if
    ** correct wait function is implemented)
    **----------------------------------------------------------------------*/
    ERR("Premature exit from task wait function while performing NVMEM operation",0,0,0);
  }

  /* if the caller provided a default value generation function */
  if (defaultfn_ptr != NULL)
  {
    /* call the caller supplied function to get the default value */
    nvi.status = (nv_stat_enum_type) (*defaultfn_ptr)(nvi.status,parameter_id,data_ptr);
  }

  if (nvi.status == NV_FAIL_S)
  {
    ERR("Error while attempting to read NVMEM item (%d)",(dword) parameter_id,0,0);
  }

  return nvi.status;
}
#endif

/*==============================================================================
FUNCTION ghdi_nvmem_write

DESCRIPTION
  Writes a data item from a caller provided buffer and stores it in NV memory.
  Waits for the response from NV.

PARAMETERS
  parameter_id    Id of the parameter being written to NV memory
  data_ptr        Pointer to caller allocated buffer where item data is stored

RETURN VALUE
  The NV return code mapped to the appropriate GHDI status value

DEPENDENCIES
  The routine is not reentrant for the same task. It is task-safe and can be
  called from different tasks
=================================================================================*/

ghdi_status_T ghdi_nvmem_write
(
  ghdi_nvmem_parameter_id_T           parameter_id,
  ghdi_nvmem_data_T                   *data_ptr
)
{
  ghdi_nvmem_status_T status;
  rex_tcb_type *taskPtr = rex_self();
  /* get the task information for calling task and call the helper */
  ghdi_nvmem_registered_task_info_T *taskInfo;
  taskInfo = ghdi_nvmem_get_task_info(taskPtr);

  if (taskInfo == NULL)
  {
    /* calling task has not been registered! */
    ERR("NVMEM error: calling task not registered with NVMEM module",0,0,0);
    return GHDI_NOT_REGISTERED;
  }

  status = ghdi_nvmem_write_imp
           (
           (nv_items_enum_type) parameter_id,
           (nv_item_type *) data_ptr,
           taskInfo->signal,
           taskInfo->taskWaitFnPtr
           );

  NAS_ASSERT_UT(status <= NV_NOTALLOC_S);
  return GHDI_NVMEM_MAP_STATUS(status);
}

#ifdef FEATURE_DUAL_SIM
ghdi_status_T ghdi_nvmem_write_per_subs
(
  ghdi_nvmem_parameter_id_T           parameter_id,
  ghdi_nvmem_data_T                   *data_ptr,
  uint16                              context_id

)
{
  ghdi_nvmem_status_T status;
  rex_tcb_type *taskPtr = rex_self();
  /* get the task information for calling task and call the helper */
  ghdi_nvmem_registered_task_info_T *taskInfo;
  taskInfo = ghdi_nvmem_get_task_info(taskPtr);

  if (taskInfo == NULL)
  {
    /* calling task has not been registered! */
    ERR("NVMEM error: calling task not registered with NVMEM module",0,0,0);
    return GHDI_NOT_REGISTERED;
  }

  status = ghdi_nvmem_write_imp_per_subs
           (
           (nv_items_enum_type) parameter_id,
           (nv_item_type *) data_ptr,
           taskInfo->signal,
           taskInfo->taskWaitFnPtr,
           context_id
           );

  NAS_ASSERT_UT(status <= NV_NOTALLOC_S);
  return GHDI_NVMEM_MAP_STATUS(status);
}
#endif

/*==============================================================================
FUNCTION ghdi_nvmem_write_imp

DESCRIPTION
  Writes a data item from a caller provided buffer and stores it in NV memory.
  Waits for the response from NV using a caller provided signal and optional
  wait function.

PARAMETERS
  parameter_id    Id of the parameter being written to NV memory
  data_p          Pointer to the caller provided buffer containing the item to write
  signal          See previous
  waitfn_p        (Optional) See previous

RETURN VALUE
  The NV return code

DEPENDENCIES
  The routine is not reentrant for the same task. It is thread-safe and can be
  called from different tasks
=================================================================================*/

ghdi_nvmem_status_T ghdi_nvmem_write_imp
(
  nv_items_enum_type         parameter_id,
  nv_item_type               *data_ptr,
  ghdi_nvmem_signal_T        signal,
  ghdi_nvmem_wait_fn_ptr_T   waitfn_ptr
)
{
  nv_cmd_type nvi;

  rex_tcb_type *taskPtr;

  taskPtr = rex_self();

  /* store calling task (as provided by rex_self()) and associated signal
  as the task to notify when the operation is complete */
  nvi.tcb_ptr = taskPtr;
  nvi.sigs = signal;
  /* the command does not go on any queue when done */
  nvi.done_q_ptr = NULL;

  nvi.item = (nv_items_enum_type) parameter_id;
  nvi.cmd = NV_WRITE_F;

  /* the NV task will write the data from the caller provided buffer */
  nvi.data_ptr = (nv_item_type *) data_ptr;
  nvi.status = NV_BUSY_S;

  /* clear the notification signal from the calling task */
  (void) rex_clr_sigs(taskPtr,signal);

  /* call the NV interface function to queue the command */
  nv_cmd(&nvi);

  /* if the caller provided a task specific wait function, use it to wait for
  notification signal */
  if (waitfn_ptr != NULL)
  {
    (void)(*waitfn_ptr)(signal);
  }
  else
  {
    /* just wait for the specified signal */
    (void) rex_wait(signal);
  }

  /* check for premature exit from wait */
  if (nvi.status == NV_BUSY_S)
  {
    /*------------------------------------------------------------------------
    ** this is a problem, a premature exit from the task wait function,
    ** has the potential of causing stack corruption (should never happen if
    ** correct wait function is implemented)
    **----------------------------------------------------------------------*/
    ERR("Premature exit from task wait function while performing NVMEM operation",0,0,0);
  }

  return nvi.status;
}


#ifdef FEATURE_DUAL_SIM
ghdi_nvmem_status_T ghdi_nvmem_write_imp_per_subs
(
  nv_items_enum_type         parameter_id,
  nv_item_type               *data_ptr,
  ghdi_nvmem_signal_T        signal,
  ghdi_nvmem_wait_fn_ptr_T   waitfn_ptr,
  uint16                              context_id
)
{
  nv_cmd_type nvi;

  rex_tcb_type *taskPtr;

  nv_cmd_ext_type nvi_ext;
  nvi_ext.nvcmd = &nvi;
  nvi_ext.context = context_id;

  taskPtr = rex_self();

  /* store calling task (as provided by rex_self()) and associated signal
  as the task to notify when the operation is complete */
  nvi.tcb_ptr = taskPtr;
  nvi.sigs = signal;
  /* the command does not go on any queue when done */
  nvi.done_q_ptr = NULL;

  nvi.item = (nv_items_enum_type) parameter_id;
  nvi.cmd = NV_WRITE_F;

  /* the NV task will write the data from the caller provided buffer */
  nvi.data_ptr = (nv_item_type *) data_ptr;
  nvi.status = NV_BUSY_S;

  /* clear the notification signal from the calling task */
  (void) rex_clr_sigs(taskPtr,signal);

  /* call the NV interface function to queue the command */
  nv_cmd_ext(&nvi_ext);

  /* if the caller provided a task specific wait function, use it to wait for
  notification signal */
  if (waitfn_ptr != NULL)
  {
    (void)(*waitfn_ptr)(signal);
  }
  else
  {
    /* just wait for the specified signal */
    (void) rex_wait(signal);
  }

  /* check for premature exit from wait */
  if (nvi.status == NV_BUSY_S)
  {
    /*------------------------------------------------------------------------
    ** this is a problem, a premature exit from the task wait function,
    ** has the potential of causing stack corruption (should never happen if
    ** correct wait function is implemented)
    **----------------------------------------------------------------------*/
    ERR("Premature exit from task wait function while performing NVMEM operation",0,0,0);
  }

  return nvi.status;
}
#endif

