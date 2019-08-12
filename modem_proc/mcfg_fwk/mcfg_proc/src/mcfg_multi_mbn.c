/*==============================================================================

                M O D E M   C O N F I G   M U L T I   M B N

GENERAL DESCRIPTION
  Module for embedded multi MBN processing.

Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_proc/src/mcfg_multi_mbn.c#1 $
$DateTime: 2016/12/13 08:00:27 $
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
09/11/15 cys   Modify module
09/04/15 ap    Initial creation
==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "secapi.h"
#include "secboot_util.h"

#include "modem_mem.h"
#include "memheap.h"

#include "mcfg_feature_config.h"
#include "mcfg_common.h"
#include "mcfg_osal.h"
#include "mcfg_nv.h"
#include "mcfg_proc.h"
#include "mcfg_load.h"
#include "mcfg_int.h"
#include "mcfg_utils.h"
#include "mcfg_trl.h"
#include "mcfg_multi_mbn.h"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


#define MCFG_MULTI_MBN_VERSION               0xEE
#define MCFG_VER_CARRIER_MAJOR_MASK          0x0000FFC0

#define MCFG_SW_VER_COMPAT_VER_BITS	         24
#define MCFG_MULTI_MBN_HEADER_LEN            4    /* the size of Multi MBN */

extern unsigned char mcfg_multi_MBN[MCFG_MULTI_MBN_MAX_SIZE];
extern mem_heap_type modem_mem_heap;


// Typedefs
typedef PACKED struct
{
   uint32	magic1;
   uint16	mcfg_format_ver_num;
   uint16	mcfg_type;
   uint32	mcfg_num_items;
   uint16	mcfg_muxd_carrier_index_info;
   uint16	spare_crc;
}
mcfg_header_type;


typedef struct
{
  uint32 multi_mbn_size;
  mcfg_header_type header;
  uint16 type;
  uint16 length;
  uint32 version;
}mcfg_multi_mbn_header_type;


/*===========================================================================

  FUNCTION mcfg_multi_mbn_generate_id

  DESCRIPTION
    Generates the config_id based on Hash Algo SHA-1

  DEPENDENCIES
    None

  PARAMETERS
    config [in] Pointer to signed image
    
  RETURN VALUE
    TRUE if generates id, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_multi_mbn_generate_id
(
  uint8 *p_img,
  uint32 data_seg_size,
  mcfg_config_id_s_type *config_id
)
{
  secerrno_enum_type          stat;
  secapi_handle_type          sh = NULL;
  secapi_hsh_param_data_type  hash_param;
  boolean                     error;

  secapi_algo_enum_type algo_type = SECAPI_SHA;
  uint32 algo_digest_size = SECAPI_HSH_SHA_DIGEST_SIZE;
  unsigned char digest_out[SECAPI_HSH_SHA_DIGEST_SIZE];
  boolean retVal = FALSE;

  MCFG_MSG_LOW_1("mcfg_multi_mbn_generate_id() size %d", data_seg_size);

  memset((void *)digest_out, 0, sizeof(digest_out));

  stat = secapi_new(&sh, algo_type);
  if (stat)
  {
    MSG_ERROR("HASH SHA: failed secapi handle", 0, 0, 0);
    return FALSE;
  }

  error = FALSE;

  do
  {
    hash_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
    stat = secapi_hsh_set_param(sh,
                                algo_type,
                                SECAPI_HSH_INPUT_MODE,
                                &hash_param);
    if (stat)
    {
      MSG_ERROR("HASH SHA: set param failure", 0, 0, 0);
      break;
    }

    hash_param.common.exec_platform = (data_seg_size < 0x1000)
      ? SECAPI_EXEC_PLATFORM_ARM
      : SECAPI_EXEC_PLATFORM_ACCELERATOR_PREFERRED;
    stat = secapi_hsh_set_param(sh,
                                algo_type,
                                SECAPI_EXEC_PLATFORM,
                                &hash_param);
    if (stat)
    {
      MSG_ERROR("HASH SHA: set param failure", 0, 0, 0);
      break;
    }

    /* compute digest, and append result to seg buf */
    stat = secapi_hsh_create_digest(sh,
                                    algo_type,
                                    p_img,
                                    data_seg_size,
                                    digest_out);
    if (stat)
    {
      MSG_ERROR("HASH SHA: failure", 0, 0, 0);
      error = TRUE;
      break;
    }
    else
    {
      //Need input on this:How to assign config_id with digest_out
      memscpy(&config_id->id, MCFG_CONFIG_ID_SIZE_MAX,
              digest_out, (SECAPI_HSH_SHA_DIGEST_SIZE * sizeof(char)));
      config_id->id_len = SECAPI_HSH_SHA_DIGEST_SIZE;
      retVal = TRUE;
    }
  }
  while (0);

  /* clean up */
  stat = secapi_delete(&sh);
  if (stat || error)
  {
    MSG_ERROR("HASH SHA: secapi delete error", 0, 0, 0);
    return FALSE;
  }

  return retVal;
} /* mcfg_multi_mbn_generate_id() */

/*===========================================================================

  FUNCTION mcfg_multi_mbn_replace_config

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS
 
  RETURN VALUE
 
  SIDE EFFECTS
 
===========================================================================*/
mcfg_error_e_type mcfg_multi_mbn_replace_config
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *new_id
)
{
  mcfg_config_id_s_type   active_id;

  if (mcfg_utils_get_active_config(type, &active_id, MCFG_SUB_ID_FIRST))
  {
    MCFG_MSG_HIGH_1("Deactivate existing active config type %d", type);
    mcfg_utils_print_config(type, &active_id);
    if(!mcfg_utils_deactivate_config(type, MCFG_SUB_ID_FIRST, TRUE))
    {
      MCFG_MSG_ERROR("Deactivation failed");
    }

    if(MCFG_ERR_NONE != mcfg_utils_delete_config(type, &active_id))
    {
      MCFG_MSG_ERROR("Fail to delete config");
    }
  }

  MCFG_MSG_HIGH_1("Select config type %d", type);
  mcfg_utils_print_config(type, new_id);
  if (!mcfg_utils_set_selected_config(type, new_id, MCFG_SUB_ID_FIRST))
  {
    MCFG_MSG_ERROR("Fail to set new config as pending");
    return MCFG_ERR_INTERNAL;
  }

  return MCFG_ERR_NONE;
} /* mcfg_multi_mbn_replace_config() */

/*===========================================================================

  FUNCTION mcfg_multi_mbn_replace_config_by_version

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS
 
  RETURN VALUE
 
  SIDE EFFECTS
 
===========================================================================*/
mcfg_error_e_type mcfg_multi_mbn_replace_config_by_version
(
  mcfg_config_type_e_type  type,
  uint32                   version,
  mcfg_config_id_s_type   *new_id
)
{
  mcfg_config_info_s_type info;
  uint32                  cur_version;
  mcfg_config_id_s_type   active_id;
  mcfg_sub_id_type_e_type sub;

  mcfg_config_id_s_type id_list[MCFG_CONFIG_LIST_SIZE_MAX];
  uint32 num_of_id;
  uint32 i;

  num_of_id = mcfg_utils_list_configs(type, MCFG_CONFIG_LIST_SIZE_MAX, &id_list[0]);
  for (i = 0; i < num_of_id; i++)
  {
    if(mcfg_utils_config_id_matches(new_id, &id_list[i]))
    {
      continue;
    }

    if (!mcfg_utils_get_config_info(type, &id_list[i], &info))
    {
      MCFG_MSG_ERROR("mcfg_multi_mbn_generate_id() returned false, continue");
      continue;
    }

    if(!mcfg_trl_decode_tlv(&info, MCFG_TRL_TLV_ID_CONFIG_VERSION,
                             &cur_version, sizeof(cur_version)))
    {
      MCFG_MSG_ERROR("trl_decode_tlv:config version not found");
      continue;
    }

    MCFG_MSG_MED_1("Found version 0x%08X", cur_version);
    mcfg_utils_print_config(type, &id_list[i]);

    if ((MCFG_TYPE_HW == type) || 
        ((cur_version & MCFG_VER_CARRIER_MAJOR_MASK) == (version & MCFG_VER_CARRIER_MAJOR_MASK)))
    {
      MCFG_MSG_MED_2("Same carrier as version 0x%08X, type %d, replace", version, type);

      /* Active config has to be deactivated before deleting */
      for (sub = MCFG_SUB_ID_FIRST; sub < MCFG_NUM_OF_SUB_ID; sub++)
      {
        if (mcfg_utils_get_active_config(type, &active_id, sub))
        {
          if (mcfg_utils_config_id_matches(&id_list[i], &active_id))
          {
            if(!mcfg_utils_deactivate_config(type, sub, TRUE))
            {
              MCFG_MSG_ERROR("Fail to deactivate HW config");
            }

            if (!mcfg_utils_set_selected_config(type, new_id, sub))
            {
              MCFG_MSG_ERROR("Fail to set new configuration as pending");
              mcfg_utils_print_config(type, new_id);
            }
            else
            {
              MCFG_MSG_HIGH("Set new configuration as pending");  
              mcfg_utils_print_config(type, new_id);
            }
          }
        }
      }

      if(MCFG_ERR_NONE != mcfg_utils_delete_config(type, &id_list[i]))
      {
        MCFG_MSG_ERROR("Fail to delete config");
      }
    }
  }

  return MCFG_ERR_NONE;
} /* mcfg_multi_mbn_replace_config_by_version() */

/*===========================================================================

  FUNCTION mcfg_multi_mbn_add_config

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS
 
  RETURN VALUE
 
  SIDE EFFECTS
 
===========================================================================*/
mcfg_error_e_type mcfg_multi_mbn_add_config
(
  mcfg_config_type_e_type  type,
  mcfg_config_s_type      *config
)
{
  mcfg_config_id_s_type   id;
  mcfg_config_info_s_type info;
  uint32 version;

  MCFG_CHECK_NULL_PTR_RET_ERROR(config, MCFG_ERR_INVALID_PARAMETER);

  if (!mcfg_utils_parse_config_info(config, &info))
  {
    MCFG_MSG_ERROR("mcfg_utils_parse_config_info() returned false");
    return MCFG_ERR_INCOMPATIBLE;
  }

  // Generate config id here.
  if (!mcfg_multi_mbn_generate_id(config->config_addr, config->config_len, &id)) 
  {
    MCFG_MSG_ERROR("mcfg_multi_mbn_generate_id() returned false");
    return MCFG_ERR_INTERNAL;
  }

  if(!mcfg_utils_is_config_id_unique(type, &id))
  {
    MCFG_MSG_ERROR("MBN with same ID already exists");
    return MODEM_CFG_UPDATE_NOT_REQUIRED;
  }

  if(MCFG_ERR_NONE != mcfg_utils_add_config(type, &id, config, MCFG_STORAGE_LOCAL))
  {
    MCFG_MSG_ERROR("Failed to store MBN");
    return MCFG_ERR_INTERNAL;
  }

  MCFG_MSG_HIGH_1("Added one config with type %d from multi MBN", type);
  mcfg_utils_print_config(type, &id);

  if (type == MCFG_TYPE_HW)
  {
    //if there's one HW mbn from bundled MBN, replace the existing active HW mbn
    return mcfg_multi_mbn_replace_config(type, &id);
  }

  //Replace existing active SW MBN with version
  if(!mcfg_trl_decode_tlv(&info, MCFG_TRL_TLV_ID_CONFIG_VERSION,
                           &version, sizeof(version)))
  {
    MCFG_MSG_ERROR("trl_decode_tlv:config version not found");
    return MCFG_ERR_INCOMPATIBLE;
  }

  return mcfg_multi_mbn_replace_config_by_version(type, version, &id);
} /* mcfg_multi_mbn_add_config() */

/*===========================================================================
FUNCTION  mcfg_multi_mbn_update() 

DESCRIPTION
  This updates the MultiMbn with new MultiMbn Bundle.
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  Result - Boolean (true or false depending on success / failure)

SIDE EFFECTS
  None
===========================================================================*/

boolean mcfg_multi_mbn_update
(
  uint32 num_of_mbns, 
  uint8 *pData
)
{
  mcfg_item_prefix_type mcfg_item_prefix;
  uint8 mcfg_item_type_int;
  uint32 i;
  mcfg_nv_item_hdr_type *mcfg_mbn_item_data_hdr;
  mcfg_config_s_type config;

  mcfg_tlv mbn_item_header;
  mcfg_tlv mbn_item_body;
  mcfg_header_type *pHeaderInfo;

  boolean mbn_update_status = TRUE;

  uint32 mcfg_add_info = 0;

  MCFG_MSG_HIGH_1("mcfg_multi_mbn_update %d mbns", (int)num_of_mbns-1);

  /* Starts from 1 as the first element is MCFG_VERSION,
  ram_cust_data_ptr points to the start of MCFG Item after version */
  for (i = 1; i < num_of_mbns; i++) 
  {
    MCFG_MSG_MED_2("process %d mbns - %d", (int)num_of_mbns-1, i);
    /* Parse the MCFG Item Prefix for further processing */
    mcfg_item_type_int =
       mcfg_parse_item_prefix(MCFG_TYPE_SW, pData, &mcfg_item_prefix,  MCFG_REFRESH_TYPE_SLOT_N_SUBS, MCFG_SUB_ID_FIRST);

    MCFG_MSG_LOW_1("mcfg_item_type_int%d", mcfg_item_type_int);
    /* Set PTR to the start of the MCFG item data after prefix */
    pData += sizeof(mcfg_item_prefix);

    switch (mcfg_item_type_int) 
    {
      case MCFG_INT_EFS_FILE:
        // read mbn filename data
        mcfg_read_tlv(&mbn_item_header, pData);
        // read mbn contents data
        mcfg_read_tlv(&mbn_item_body, pData + mbn_item_header.hdr.mcfg_nv_item_length + MCFG_TL_SIZE_BYTES);

        if (!mcfg_verify_efs_item_contents(&mbn_item_header, &mbn_item_body)) 
        {
          MCFG_MSG_ERROR("MBN File item format incorrect");
        }
        else
        {
          config.config_len = mbn_item_body.hdr.mcfg_nv_item_length;
          if (config.config_len < 0 || config.config_len > MCFG_CONFIG_SIZE_MAX)
          {
            MCFG_MSG_ERROR_1("Invalid config size %x", config.config_len);
          }
          else
          {
            config.config_addr = mbn_item_body.value;
            pHeaderInfo = (mcfg_header_type *)mcfg_load_seg(&config, &mcfg_add_info);
            if (!pHeaderInfo)
            {
              MCFG_MSG_ERROR_1("Invalid header %p", mbn_item_body.value);
            }
            else
            {
              mcfg_multi_mbn_add_config(pHeaderInfo->mcfg_type, &config);
            } 
          }
        }

        break;

      case MCFG_INT_CONFIG_ITEM:
      case MCFG_INT_SW_ONLY:
        /* Advance to the next MCFG item */
        break;

      default:
        /* Unknown - Err fatal should never be here */
        //MCFG_ERR_FATAL("MCFG Parsing Error %d", mcfg_item_type_int, 0, 0);
        MCFG_MSG_ERROR_1("MCFG Parsing Error %d", mcfg_item_type_int);
        mbn_update_status = FALSE;
        break;
    } /* Switch */

    pData += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
  } /* for */

  return mbn_update_status;
} /*mcfg_multi_mbn_update()*/

/*===========================================================================

FUNCTION  mcfg_multi_mbn_proc() 

DESCRIPTION
  This is the main modem config processing routine called
  during modem config task startup if Multi mbn feature is enabled.

DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  uint32 - Initialization success returns 0, othewise returns the error code.

SIDE EFFECTS
  None

===========================================================================*/
mcfg_error_e_type mcfg_multi_mbn_proc(void)
{
  mcfg_error_e_type processing_status = MODEM_CFG_PROC_SUCCESS;
  mcfg_multi_mbn_header_type  *pMultiMBN;
  uint32 num_of_mbns;
  uint32 cur_version;
  mcfg_config_type_e_type type;
  mcfg_sub_id_type_e_type sub;

/* -------------------------------------------------------------------- */
  MCFG_MSG_HIGH("==== Starting multi_mbn_process ==== ");

  do
  {
    // Get the pointer of MultiMBN
    pMultiMBN = (mcfg_multi_mbn_header_type *)mcfg_multi_MBN;

    // Validate Header
    num_of_mbns = validate_mcfg_header(MCFG_TYPE_SW,
                                       (uint8 *)&pMultiMBN->header,
                                       MCFG_SUB_ID_FIRST,
                                       MCFG_REFRESH_TYPE_SLOT_N_SUBS);
    if (!num_of_mbns)
    {
      MCFG_MSG_ERROR("Validate MultiMBN Header Fail");
      processing_status = MODEM_CFG_HEADER_INVALID;
      break;
    }

    // check version see if update is required
    if (pMultiMBN->type != MCFG_SW_VERSION_I)
    {
      MCFG_MSG_ERROR_1("Invalid multi MBN verion type %d", pMultiMBN->type);
      processing_status = MODEM_CFG_HEADER_INVALID;
      break;
    }

    if (pMultiMBN->length != sizeof(pMultiMBN->version))
    {
      MCFG_MSG_ERROR_1("Invalid multi MBN verion length %d", pMultiMBN->length);
      processing_status = MODEM_CFG_HEADER_INVALID;
      break;
    }

    if (MCFG_NV_STATUS_OK == mcfg_nv_read(MCFG_NV_ITEM_MULTI_MBN_CONFIG_VERSION_I,
                                          0, MCFG_SUB_ID_FIRST, (uint8 *)&cur_version, sizeof(pMultiMBN->version)))
    {
      if (pMultiMBN->version == cur_version)
      {
        MCFG_MSG_MED_1("Same version %lx, no need to update multi MBN", cur_version);
        processing_status = MODEM_CFG_UPDATE_NOT_REQUIRED;
        break;
      }
    }

    if ((pMultiMBN->version >> MCFG_SW_VER_COMPAT_VER_BITS) != MCFG_MULTI_MBN_VERSION)
    {
      MCFG_MSG_ERROR_1("Version %lx incompatible", pMultiMBN->version);
      processing_status = MCFG_ERR_INCOMPATIBLE;
      break;
    }

    // Deactivate existing mbns
    for (type = MCFG_TYPE_FIRST; type < MCFG_NUM_OF_TYPES; type++)
    {
      if (MCFG_ERR_NONE != mcfg_utils_delete_all(type))
      {
        MCFG_MSG_ERROR("unable to delete existing inactive configs");
      }
    }

    // Process multi mbns
    if (!mcfg_multi_mbn_update(num_of_mbns, (uint8 *)pMultiMBN + sizeof(mcfg_multi_mbn_header_type)))
    {
      MCFG_MSG_ERROR("mcfg_multi_mbn_update Failed");
      processing_status = MODEM_CFG_UPDATE_NVEFS_FAIL;
      break;
    }

    // Save the MBN version
    if (MCFG_NV_STATUS_OK != mcfg_nv_write(MCFG_NV_ITEM_MULTI_MBN_CONFIG_VERSION_I,
                                           0, MCFG_SUB_ID_FIRST, (uint8 *)&pMultiMBN->version, sizeof(pMultiMBN->version)))
    {
      MCFG_MSG_ERROR_2("mcfg_multi_mbn_update Fail for cfg type, %0x! subs_id=%d", MCFG_TYPE_SW, MCFG_SUB_ID_FIRST);
      processing_status = MODEM_CFG_VERSION_UPDATE_FAIL;
      break;
    }
  } while (0);

  // reclaim to modem_mem_heap
  memset((void *)mcfg_multi_MBN, 0, MCFG_MULTI_MBN_MAX_SIZE);
  mem_heap_add_section(&modem_mem_heap, (void *)mcfg_multi_MBN, MCFG_MULTI_MBN_MAX_SIZE);

  return processing_status; 
} /*mcfg_multi_mbn_proc()*/

