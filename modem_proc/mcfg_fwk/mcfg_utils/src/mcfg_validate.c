/*==============================================================================

                M O D E M   C O N F I G   V A L I D A T I O N
 
GENERAL DESCRIPTION
  Validation library for MCFG to compare configurations with current EFS settings

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_validate.c#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 as/cy Initial creation

==============================================================================*/

#include "comdef.h"

#include "msg.h"
#include "mcfg_osal.h"

#include "mcfg_common.h"
#include "mcfg_sel.h"
#include "mcfg_utils.h"
#include "mcfg_proc.h"

#include "mcfg_auth_i.h"
#include "mcfg_imgsel.h"
#include "mcfg_utils_i.h"
#include "mcfg_default.h"
#include "mcfg_efs_i.h"
#include "mcfg_map_i.h"
#include "mcfg_remote_i.h"

//#include "dsatapi.h"
#include "sys_m_reset.h"
#include "timer.h"

#include "DALSys.h"

#include "err.h"
#include "fs_errno.h"
#include "fs_public.h"

#include "mcfg_validate_i.h" /* for MCFG_HEADER_LEN, MCFG_NV_DATA_OFFSET */
#include "mcfg_validate.h"
#include "mcfg_trl.h"
#include "mcfg_load.h"
#include "mcfg_nv.h" /* mcfg_nv_read */
/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
typedef struct mcfg_config_ver_s {
   uint32 mcfg_cfg_ver;
   uint32 mcfg_base_ver;
}mcfg_config_ver_s_type;

typedef enum mcfg_variant_type_e{
  U_CARRIER_MIN = 0,
  U_CARRIER_NV_ITEM,
  U_CARRIER_NV_EFS_FILE,
  U_CARRIER_SLOT_NV_ITEM,
  U_CARRIER_SLOT_NV_EFS_FILE,
  U_CARRIER_DATA_PROFILE,
  U_CARRIER_MAX
} mcfg_variant_type_e_type;

/*==============================================================================
  Variables
==============================================================================*/
static int xml_fd = -1;

/* originally from mcfg_proc.c */
boolean mcfg_restore_fact_def_validate = FALSE;
uint8   mcfg_ver_compare_result_validate = MCFG_VER_GREATER_MUXD_VERSION;
/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

/*===========================================================================

FUNCTION  mcfg_validate_xml_bulk_write() 

DESCRIPTION
  This routine writes len bytes from buf to the global XML file descriptor

DEPENDENCIES

RETURN VALUE
  boolean - True if len bytes were written, false otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_validate_xml_bulk_write
(
  void *buf, 
  int   len
)
{
  int efs_ret;
  int bytes_written;

  MCFG_CHECK_NULL_PTR_RET_FALSE(buf);
  
  bytes_written = 0;
  do
  {
    efs_ret = efs_write(xml_fd, (uint8 *)buf + bytes_written, 
                        len - bytes_written);
  } while (efs_ret > 0 && (bytes_written += efs_ret) < len);
  
  if (bytes_written < len)
  {
    MCFG_MSG_ERROR_2("only wrote %d bytes for config with len %d",
                     bytes_written, len);
    return FALSE;
  }

  return TRUE;
} /* mcfg_validate_xml_bulk_write() */

/*===========================================================================

FUNCTION  mcfg_validate_xml_bin_write() 

DESCRIPTION
  This routine converts len bytes from buf to base 10 values in ASCII
  and sends it to be written to the XML file

DEPENDENCIES

RETURN VALUE
  boolean - True if len bytes were successfully converted and written,
  false otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_validate_xml_bin_write
(
  uint8 *buf, 
  int   len
)
{
  char *decimal_buf = NULL;
  int offset = 0;
  int i = 0;
  boolean ret = FALSE;

  if (len == 0)
  {
    return TRUE;
  }

  MCFG_CHECK_NULL_PTR_RET_FALSE(buf);

  decimal_buf = mcfg_malloc((4*len)+1);
  if (!decimal_buf)
  {
    return FALSE;
  }
  memset(decimal_buf, 0, (4*len)+1);

  for (i = 0; i < len; i++)
  {
    if(snprintf(decimal_buf+offset, 5, "%d ", buf[i])<0)
    {
      mcfg_free(decimal_buf);
      return FALSE;
    }

    if (buf[i]<10)
    {
      offset += 2;
    }
    else if (buf[i]<100)
    {
      offset += 3;
    }
    else
    {
      offset += 4;
    }
  }

  ret = mcfg_validate_xml_bulk_write(decimal_buf, offset);

  mcfg_free(decimal_buf);

  return ret;
} /* mcfg_validate_xml_bin_write() */

/*===========================================================================

FUNCTION  mcfg_validate_xml_cleanup() 

DESCRIPTION
  This routine removes the XML file used for config validation

DEPENDENCIES

RETURN VALUE
  boolean - always true

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_validate_xml_cleanup
(
  void
)
{
  if(xml_fd >= 0)
  {
    efs_close(xml_fd);
    xml_fd = -1;
  }

  efs_unlink("/mcfg_test.xml"); 
  return TRUE;
}

/*===========================================================================

FUNCTION  mcfg_validate_xml_start() 

DESCRIPTION
  This routine generates and writes the header used for the validation XML

DEPENDENCIES

RETURN VALUE
  boolean - True if generate and write is successful, false otherwiske

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_validate_xml_start
(
  mcfg_config_type_e_type  type,
  uint32                   mcfg_version,
  uint32                   base_version
)
{
  char buf[256];

  mcfg_validate_xml_cleanup();

  xml_fd = efs_open("/mcfg_test.xml", O_CREAT | O_WRONLY); 
  if (xml_fd < 0)
  {
    MCFG_MSG_ERROR_1("Unable to create/open testxml (%d)",efs_errno);
    return FALSE;
  }

  memset(&buf, 0, sizeof(buf));
  if (snprintf(buf, 255, "<?xml version=\"1.0\" encoding=\"utf-8\"?>"\
                         "\r\n<NvData>"\
                         "\r\n\t<NvConfigurationData device_version=\"0x%08X\" base_version=\"0x%08X\" type=\"%d\"/>", 
                         (unsigned int)mcfg_version, (unsigned int)base_version, type) < 0)
  {
    MCFG_MSG_ERROR("mcfg_validate_xml_start failed to generate xml header");
    mcfg_validate_xml_cleanup();
    return FALSE;
  }

  if(!mcfg_validate_xml_bulk_write(buf, strlen(buf)))
  {
    MCFG_MSG_ERROR("mcfg_validate_xml_start failed to write xml header");
    mcfg_validate_xml_cleanup();
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION  mcfg_validate_xml_update() 

DESCRIPTION
  This routine generates and writes an entry in the validation XML. This entry
  consists of MCFG configuration data and target data that have been compared.
  Structures that point to NULL data and have length of 0 are acceptable.

DEPENDENCIES

RETURN VALUE
  boolean - True if the comparison entry was succesfully entered, false otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_validate_xml_update
(
  mcfg_validate_delta_entry *entry
)
{
  char buf[256];
  boolean ret = FALSE;
  int status = -1;

  MCFG_CHECK_NULL_PTR_RET_FALSE(entry);
  MCFG_CHECK_NULL_PTR_RET_FALSE(entry->config_data);
  MCFG_CHECK_NULL_PTR_RET_FALSE(entry->efs_data);

  memset(&buf, 0, sizeof(buf));

  do
  {
    //top+config
    status = snprintf(buf, 255, "\r\n\t<NvItemData id=\"%s\" mcfgAttributes=\"0x%02X\" mcfgVariant=\"%d\">"\
                                "\r\n\t\t<NvItemDataREFERENCE>"\
                                "\r\n\t\t\t<Member sizeOf=\"%d\" type=\"uint8\">", 
                                entry->name, entry->attribute, entry->variant, (int)entry->config_data->data_len );
    if (status < 0)
    {
      break;
    }

    if(!mcfg_validate_xml_bulk_write(buf, strlen(buf)))
    {
      break;
    }

    //config data
    if(!mcfg_validate_xml_bin_write(entry->config_data->data_ptr, entry->config_data->data_len))
    {
      break;
    }

    //efs
    status = snprintf(buf, 255, "</Member>"\
                                "\r\n\t\t</NvItemDataREFERENCE>"\
                                "\r\n\t\t<NvItemDataDEVICE>"\
                                "\r\n\t\t\t<Member sizeOf=\"%d\" type=\"uint8\">", 
                                (int)entry->efs_data->data_len );
    if (status < 0)
    {
      break;
    }

    if(!mcfg_validate_xml_bulk_write(buf, strlen(buf)))
    {
      break;
    }

    //efs data
    if(!mcfg_validate_xml_bin_write(entry->efs_data->data_ptr, entry->efs_data->data_len))
    {
      break;
    }

    //bottom
    status = snprintf(buf, 255, "</Member>\r\n\t\t</NvItemDataDEVICE>\r\n\t</NvItemData>");
    if (status < 0)
    {
      break;
    }

    if(!mcfg_validate_xml_bulk_write(buf, strlen(buf)))
    {
      break;
    }

    ret = TRUE;
  }while (0);


  if(!ret)
  {
    MCFG_MSG_ERROR("mcfg_validate_xml_update failed to write/generate xml content");
    mcfg_validate_xml_cleanup();
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION  mcfg_validate_xml_complete() 

DESCRIPTION
  This routine generates and writes a closing tag to the XML file descriptor

DEPENDENCIES

RETURN VALUE
  boolean - True if the write was successul, false otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_validate_xml_complete
(
  void
)
{
  char buf[256];

  memset(&buf, 0, sizeof(buf));
  if (snprintf(buf, 255, "\r\n</NvData>") < 0)
  {
    MCFG_MSG_ERROR("mcfg_validate_xml_complete failed to generate xml bottom");
    mcfg_validate_xml_cleanup();
    return FALSE;
  }

  if(!mcfg_validate_xml_bulk_write(buf, strlen(buf)))
  {
    MCFG_MSG_ERROR("mcfg_validate_xml_complete failed to write xml bottom");
    mcfg_validate_xml_cleanup();
    return FALSE;
  }

  efs_close(xml_fd);
  return TRUE;
}

/*===========================================================================

FUNCTION  modem_cfg_load_and_auth_mcfg_validate() 

DESCRIPTION
  This routine loads and authenticates a configuration.
 
 
DEPENDENCIES

RETURN VALUE
  mcfg_error_e_type
 
SIDE EFFECTS
  None

===========================================================================*/
mcfg_error_e_type modem_cfg_load_and_auth_mcfg_validate
(
   uint8 cfg_type,
   mcfg_config_id_s_type* current_id_ptr, 
   mcfg_config_s_type* config_ptr,
   boolean active,
   mcfg_sub_id_type_e_type sub,
   boolean validate
)
{
  boolean ret = FALSE;

  mcfg_error_e_type error = MODEM_CFG_PROC_LOAD_FAIL;

  MCFG_CHECK_NULL_PTR_RET_ERROR(current_id_ptr, MCFG_ERR_NULL_POINTER);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config_ptr, MCFG_ERR_NULL_POINTER);

  /* A selected / pending config is not necessary if routine is called for config validation purposes */
  if (!validate)
  {
     if (active)
     {
       ret = mcfg_utils_get_active_config(cfg_type, current_id_ptr, sub);
     }
     else
     {
       ret = mcfg_utils_get_selected_config(cfg_type, current_id_ptr, sub);
     }
  }

  if(validate|| ret)
  {
    MCFG_MSG_HIGH("modem_cfg_load_and_auth_mcfg - 1 ");

    if (mcfg_utils_get_config_size(cfg_type, current_id_ptr, &config_ptr->config_len))
    {
      MCFG_MSG_HIGH_1("modem_cfg_load_and_auth_mcfg - 2, size=%d", config_ptr->config_len);
      if(mcfg_utils_alloc_config_buffer(cfg_type, config_ptr))
      {
        MCFG_MSG_HIGH_1("modem_cfg_load_and_auth_mcfg - 3, config_addr=%p", config_ptr->config_addr);
        if(mcfg_utils_get_config(cfg_type, current_id_ptr, config_ptr))
        {
          error = mcfg_auth_check_config(config_ptr);
          MCFG_MSG_HIGH_1("modem_cfg_load_and_auth_mcfg - 4, %d", error);
        }

        if(error != MCFG_ERR_NONE)
        {
          mcfg_utils_free_config_buffer(config_ptr);
        }
      }
    }
  }

  MCFG_MSG_HIGH_1("modem_cfg_load_and_auth_mcfg 5 - err %d", error);
  return error; 
}/* modem_cfg_load_and_auth_mcfg_validate() */

/*===========================================================================

  FUNCTION mcfg_validate_get_config_ver

  DESCRIPTION
    Retrieves the MCFG & QC versions from TRL

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    boolean - True if both versions are retrieved, false otherwise

  SIDE EFFECTS
    None

===========================================================================*/

boolean mcfg_validate_get_config_ver
( 
   mcfg_config_type_e_type current_mcfg_type,
   mcfg_config_id_s_type *mcfg_current_id,
   mcfg_config_ver_s_type *cfg_ver
)
{
   mcfg_config_info_s_type config_info;
   boolean success = FALSE;

   MCFG_CHECK_NULL_PTR_RET_FALSE(mcfg_current_id);
   MCFG_CHECK_NULL_PTR_RET_FALSE(cfg_ver);

   if (!mcfg_utils_get_config_info(current_mcfg_type, mcfg_current_id, &config_info))
   {
     MCFG_MSG_ERROR("Couldn't get config info, skip validation report!");
   }
   else if (!mcfg_trl_decode_tlv(&config_info, MCFG_TRL_TLV_ID_CONFIG_VERSION, 
                   &cfg_ver->mcfg_cfg_ver, sizeof(cfg_ver->mcfg_cfg_ver)))
   {
     MCFG_MSG_ERROR("Couldn't get config version, validation report!");
   }
   else if (!mcfg_trl_decode_tlv(&config_info, MCFG_TRL_TLV_ID_BASE_VERSION, 
                   &cfg_ver->mcfg_base_ver, sizeof(cfg_ver->mcfg_base_ver)))
   {
     MCFG_MSG_ERROR("Couldn't get base version, validation report!");
   }
   else 
   {
      success = TRUE;
   }

   return success;
}

/*===========================================================================

  FUNCTION mcfg_validate_config

  DESCRIPTION
    This routine initiates a comparison report that lists differences between
    an MCFG configuration and the current settings on a target

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    mcfg_error_e_type

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_validate_config
(
   mcfg_config_type_e_type current_mcfg_type,
   mcfg_config_id_s_type *mcfg_current_id,
   mcfg_sub_id_type_e_type sub
)
{
   uint32 mcfg_load_status  = 0; 
   mcfg_config_ver_s_type mcfg_versions = {0,0};
   mcfg_config_s_type  mcfg_current_buf;
   mcfg_config_s_type*  p_mcfg_buf=NULL;

   p_mcfg_buf = &mcfg_current_buf;
   mcfg_current_buf.config_len = 0;
   mcfg_current_buf.config_addr = NULL;


   uint8* ram_cust_data_ptr;
   uint8 *mcfg_seg_ptr;
   uint32 mcfg_add_info = 0;
   uint32 num_mcfg_items;

   MCFG_MSG_LOW("Validating config...");
   /* LOAD AND AUTHENTICATE MODEM CFG IMAGE */
   mcfg_load_status = modem_cfg_load_and_auth_mcfg_validate(current_mcfg_type, 
                        mcfg_current_id, p_mcfg_buf, FALSE, sub, TRUE);

   do
   {
     if(mcfg_load_status != MCFG_ERR_NONE)
     {
       MCFG_MSG_ERROR_3("Load & Authenticate fail for cfg type %0x, status = %d, subs_id = %d",
                         current_mcfg_type, (int)mcfg_load_status, sub );
       break;
     }

     if(!mcfg_validate_get_config_ver(current_mcfg_type, mcfg_current_id, &mcfg_versions))
     {
        MCFG_MSG_HIGH_3("Cannot find version info for config with ID %02X%02X%02X",
                        mcfg_current_id->id[0], mcfg_current_id->id[1], mcfg_current_id->id[2]);
        mcfg_load_status = MCFG_ERR_INTERNAL;
        break;
     }

     /* Get the MCFG SEG from the Modem Config File buffer */
     mcfg_seg_ptr = mcfg_load_seg(p_mcfg_buf, &mcfg_add_info );

     if (mcfg_seg_ptr == NULL )
     {
       MCFG_MSG_ERROR_2("Cannot find MCFG_SEG for cfg_type %d, subs_id %d",current_mcfg_type, sub);
       mcfg_load_status = MODEM_CFG_PROC_LOAD_FAIL;
       break;
     }

     /* Initialize our RAM ptr to the start of MCFG segment */
     ram_cust_data_ptr = mcfg_seg_ptr;

     /* Validate the MCFG Header Information and get the number of NV items from this data */
     num_mcfg_items = validate_mcfg_header(current_mcfg_type, ram_cust_data_ptr, sub, 
	                                       MCFG_REFRESH_TYPE_SLOT_N_SUBS );

     if (!num_mcfg_items ) 
     {
       MCFG_MSG_ERROR_2("Validate MCFG Header Fail, cfg_type = %0x! subs_id=%d", 
                         current_mcfg_type, sub);
       mcfg_load_status = MODEM_CFG_PROC_LOAD_FAIL;
       break;
     }
     else
     {
       ram_cust_data_ptr+= MCFG_HEADER_LEN; /* the ram_cust_data_ptr points to the first MCFG NV element */
     }

     ram_cust_data_ptr = mcfg_seg_ptr + MCFG_NV_DATA_OFFSET;

     /* TODO: Optimize so if no diffs exist then no XML is created at all */
     if (!mcfg_validate_xml_start(current_mcfg_type, 
           mcfg_versions.mcfg_cfg_ver, mcfg_versions.mcfg_base_ver))
     {
        MCFG_MSG_ERROR("Could not start diff report!");
        mcfg_load_status = MCFG_ERR_INTERNAL;
        break;
     }

     if (mcfg_cfg_diff_nvefs(current_mcfg_type, num_mcfg_items, ram_cust_data_ptr, sub, TRUE) == FALSE)
     {
       MCFG_MSG_ERROR_2("MCFG NVEFS diff fail for cfg type, %0x! subs_id=%d", current_mcfg_type, sub);
       mcfg_load_status = MODEM_CFG_UPDATE_NVEFS_FAIL;
       break;
     }

     if (!mcfg_validate_xml_complete())
     {
        MCFG_MSG_ERROR("Could not complete diff report!");
        mcfg_load_status = MCFG_ERR_INTERNAL;
        break;
     }
   }while(0);

   mcfg_utils_free_config_buffer(p_mcfg_buf);


   return mcfg_load_status;
} /* mcfg_validate_config() */

/*===========================================================================

  FUNCTION mcfg_get_item_metadata

  DESCRIPTION
    This routine determines whether special data is present in the item by
    analyzing the attributes. If present, the data is then passed to result

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    boolean - True if data has been passed to result. False otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_get_item_metadata
(
   uint8 *data_ptr,
   uint8 attrib, 
   uint8 attrib_flag,
   uint8 *result_ptr
)
{
  boolean status = FALSE;

  MCFG_CHECK_NULL_PTR_RET_FALSE(data_ptr);
  MCFG_CHECK_NULL_PTR_RET_FALSE(result_ptr);

  if (attrib & (attrib_flag & 0xFF))
  {
    *result_ptr = *data_ptr;
    status = TRUE;
  }
  return status;
}

/*===========================================================================

  FUNCTION mcfg_validate_nvefs_data_match

  DESCRIPTION
    This routine compares the data passed from two buffers, and determines
    if they are the same, even when data is of length 0. 

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    boolean - True if data buffers match, false otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_validate_nvefs_data_match
(
  mcfg_data_len_ptr_type *config_data,
  mcfg_data_len_ptr_type *efs_data
)
{
  boolean status = FALSE;

  MCFG_CHECK_NULL_PTR_RET_FALSE(config_data);
  MCFG_CHECK_NULL_PTR_RET_FALSE(efs_data);

  MCFG_MSG_LOW_2("Comparing config and target data bufs. config_len: %d, target len: %d", config_data->data_len, efs_data->data_len );

  if ((efs_data->data_len == 0) && (config_data->data_len == 0))
  {
    /* No need to create item report if both efs & config data are 0B */
    status = TRUE;
  }
  else if (efs_data->data_len == config_data->data_len)
  {
    if (0 == memcmp(config_data->data_ptr, efs_data->data_ptr, config_data->data_len))
    {
      status = TRUE;
    }
  }
  else
  {
    /* Data lengths are different,  so data itself is different */
  }

  return status;
} /* mcfg_validate_nvefs_data_match */

/*===========================================================================

  FUNCTION mcfg_validate_adjust_xml_data_buf_size

  DESCRIPTION
    This routine adjust the reported buffer length and associated buffer ptr
    dependind on what attributes the item has set

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    boolean - True if updated length is >= 0, false otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_validate_adjust_xml_data_buf_size
(
  mcfg_data_len_ptr_type *data,
  uint8 attrib
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(data);

  if (attrib & U_ITEM_ATTRIB_MULTISIM)
  {
    data->data_len--;
    data->data_ptr = (void *) ((int) data->data_ptr + sizeof(uint8));
  }
  if (attrib & U_ITEM_ATTRIB_INDEXED)
  {
    data->data_len--;
    data->data_ptr = (void *) ((int) data->data_ptr + sizeof(uint8));
  }

  if (data->data_len < 0)
  {
    MCFG_MSG_HIGH_1("MCFG validate: adjusted data buf size < 0. size: %ld", data->data_len);
    return FALSE;
  }
  MCFG_MSG_LOW_1("MCFG validate: adjusted data buf size %d", data->data_len);
  return TRUE;
} /* mcfg_validate_adjust_xml_data_buf_size */

/*===========================================================================

  FUNCTION mcfg_validate_init_xml_data_bufs

  DESCRIPTION
    This routine initializes the data structures intended to store
    comparison data for MCFG validation

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    boolean - True if successfully initialize, false otherwise (ptrs passed == NULL) 

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_validate_init_xml_data_bufs
(
   mcfg_data_len_ptr_type *config_data,
   mcfg_data_len_ptr_type *efs_data
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_data);
  MCFG_CHECK_NULL_PTR_RET_FALSE(efs_data);

  MCFG_MSG_LOW("MCFG Validate: Init xml data bufs");
   /* Initialize */
  config_data->data_len = 0;
  config_data->data_ptr = NULL;
  efs_data->data_len = 0;
  efs_data->data_ptr = NULL;

  return TRUE;
} /* mcfg_validate_init_xml_data_bufs */

/*===========================================================================

  FUNCTION mcfg_get_item_size

  DESCRIPTION
    This routine obtains the size of an item file

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    mcfg_nv_status_e_type   

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_nv_status_e_type mcfg_get_item_size(
  uint32    item,
  uint8     index,
  uint8     subs_id,
  uint32    *file_size
  )
{
  char item_path[MCFG_NV_MAX_EFS_PATH_LEN];
  struct fs_stat mcfg_nvefs_file_stat;

  boolean new_indexed;
  mcfg_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MCFG_MSG_LOW_3("MCFG validate: getting item size - item %d, index %d, subs_id %d", item, index, subs_id);

  status = mcfg_nv_build_path(item, index, subs_id, &new_indexed, item_path, sizeof(item_path));
  if (status != MCFG_NV_STATUS_OK)
  {
    MCFG_MSG_MED_1("MCFG: build item_path failure for item %d", item);
    return status;
  }

  if(efs_lstat((char *)item_path, &mcfg_nvefs_file_stat) != -1) 
  {
    *file_size = mcfg_nvefs_file_stat.st_size;
  }
  else
  {
    switch (efs_errno)
    {
      case ENOENT:
        status = MCFG_NV_STATUS_NOT_ACTIVE;
        break;
    
      case FS_ERANGE:
        status = MCFG_NV_STATUS_BAD_SIZE;
        break;
    
      default:
        status = MCFG_NV_STATUS_EFS_ERR;
    }
    return status;
  }

  return status;
} /* mcfg_get_item_size() */

/*===========================================================================

  FUNCTION mcfg_populate_and_send_xml_entry

  DESCRIPTION
    This routine populates the xml entry structure and then sends it out

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    mcfg_nv_status_e_type   

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_nv_status_e_type mcfg_populate_and_send_xml_entry
(
  uint8                     *name,
  uint8                     attribute,
  mcfg_variant_type_e_type  variant,
  mcfg_data_len_ptr_type    *config_data,
  mcfg_data_len_ptr_type    *efs_data
)
{
  mcfg_validate_delta_entry *delta_entry;
  boolean status = MCFG_NV_STATUS_EFS_ERR;

  MCFG_CHECK_NULL_PTR_RET_FALSE(name)  
  MCFG_CHECK_NULL_PTR_RET_FALSE(config_data)  
  MCFG_CHECK_NULL_PTR_RET_FALSE(efs_data)  

  MCFG_MSG_SPRINTF_1("Populating & sending XML entry for %s", name);

  /* TODO: replace this with macro */
  if (variant <= U_CARRIER_MIN || variant >= U_CARRIER_MAX )
  {
     MCFG_MSG_HIGH_1("MCFG validation update: unknown item type = %d", (uint8) variant);
     return status;
  }

  delta_entry = mcfg_malloc(sizeof(mcfg_validate_delta_entry));
  if( delta_entry == NULL) 
  {
    MCFG_MSG_ERROR("MCFG: Memory allocation failure for XML entry");
    return status;
  }

  delta_entry->name = name;
  delta_entry->attribute = attribute;
  delta_entry->variant = variant;
  delta_entry->config_data = config_data;
  delta_entry->efs_data = efs_data;

  if(!mcfg_validate_xml_update(delta_entry))
  {
     MCFG_MSG_ERROR_1("MCFG update fail for item %s", name);
  }
  else 
  {
    MCFG_MSG_LOW_1("Successfully sent XML entry for item %s", name);
    status = MCFG_NV_STATUS_OK;
  }
  
  mcfg_free(delta_entry);

  return status;
} /* mcfg_populate_and_send_xml_entry */

/*===========================================================================

  FUNCTION mcfg_proc_sub_relevant

  DESCRIPTION
    This routine determines if a multiSIM item is applicable to the
    current subs_id

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    mcfg_nv_status_e_type   

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_nv_status_e_type mcfg_proc_sub_relevant
(
   uint8  subs_mask,
   uint8  subs_id
)
{
  mcfg_nv_status_e_type status = MCFG_NV_STATUS_OK;

  {
    if(subs_mask > MCFG_SUBS_MASK)
    {
       status = MCFG_NV_STATUS_BAD_ITEM;
    }
    else if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
    {
      status = MCFG_NV_STATUS_UNSUPPORTED;
    }
  }

  return status;
} /* mcfg_proc_sub_relevant */

/*===========================================================================
                                  
FUNCTION  mcfg_efs_read() 

DESCRIPTION
  Reads previously existing log data into *mcfg_log_data_ptr or allocates new space

DEPENDENCIES 
  None 

RETURN VALUE 
  -1 on failure, size of data read if successful

SIDE EFFECTS
  None

===========================================================================*/
fs_size_t mcfg_efs_read
(
   int fh,
   uint8*	data_ptr,
   uint32 fsize
)
{
  fs_size_t pos=0;
  int total_bytes_read=0;
  int bytes_read=0;
  uint8 *efs_buf = data_ptr;

  MCFG_MSG_LOW("Entered mcfg_efs_read: reading efs file bytes.....");
  /* Read the data from the log file if not empty */
  if(fsize != 0 ) 
  {
    /* It is permitted for efs_read to return fewer bytes than were requested,
     * even if the data is available in the file, so make sure to read out all the data ! */
    while (pos < fsize) 
    {
      bytes_read = efs_read(fh, (void *)(efs_buf + pos), fsize - pos);
      MCFG_MSG_LOW_1("bytes read = %d", bytes_read);
      if (bytes_read > 0) 
      {
        if (bytes_read == 0)
          break;
        pos += (fs_size_t) bytes_read;
        total_bytes_read += bytes_read;
      } 
      else 
      {
        MCFG_MSG_ERROR("mcfg_efs_read: read ret < 0");
        return -1;
      }
    }
  }

  MCFG_MSG_LOW_2("mcfg_efs_read: data_ptr[0] = %d, efs_ptr[0] = %d", *data_ptr, *efs_buf);

  return total_bytes_read;
} /* mcfg_efs_read */

/*===========================================================================
FUNCTION  mcfg_validate_efs_file() 

DESCRIPTION
  This routine is called to validate the entry for an MCFG configuration
  EFS file and the corresponding EFS file on the target.
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  boolean - True if successful comparison, false otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean mcfg_validate_efs_file
(
   uint8* efsData, 
   mcfg_item_prefix_type* mcfg_item_prefix, 
   uint8 mcfg_item_type_int
)
{
  uint8 attributes = 0x00;
  uint8 item_type = 0;

  char f_name[MCFG_FILE_PATH_LENGTH];
  char* filePath  = NULL;
  uint8* file_ptr = NULL;
  uint8* fileData = NULL;

  uint8  subs_id = 0;

  mcfg_tlv efs_item_header;
  mcfg_tlv efs_item_body;

  mcfg_nv_status_e_type mcfg_result = MCFG_NV_STATUS_OK; 

  mcfg_data_len_ptr_type config_data = {0, NULL};
  mcfg_data_len_ptr_type efs_data    = {0, NULL};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(efsData);
  MCFG_CHECK_NULL_PTR_RET_FALSE(mcfg_item_prefix);

  attributes = mcfg_item_prefix->attrib;
  item_type = mcfg_item_prefix->item_type;

  // read filename data
  mcfg_read_tlv(&efs_item_header, efsData);
  // read file contents data
  mcfg_read_tlv(&efs_item_body, efsData + efs_item_header.hdr.mcfg_nv_item_length + MCFG_TL_SIZE_BYTES);

  if(!mcfg_verify_efs_item_contents(&efs_item_header, &efs_item_body))
  {
    MCFG_MSG_ERROR("MCFG validate: EFS File item format incorrect");
    return FALSE;
  }

  filePath = (char *)efs_item_header.value;
  fileData = efs_item_body.value;

  switch(mcfg_item_type_int)
  {
    case MCFG_INT_DATA_PROFILE:
    case MCFG_INT_STATIC_WO_DATA_PROFILE:
    case MCFG_INT_MUXED_DATA_PROFILE:
    case MCFG_INT_DELETE_DATA_PROFILE:
      if(!mcfg_get_profile_path(filePath, 
           efs_item_header.hdr.mcfg_nv_item_length, f_name, subs_id))
      {
        MCFG_MSG_ERROR("MCFG validate: failure getting profile path");
        return FALSE;
      }
      break;
    case MCFG_INT_EFS_FILE:
    case MCFG_INT_DELETE_EFS_FILE:
    case MCFG_INT_STATIC_WO_EFS_FILE:
    case MCFG_INT_MUXD_EFS_FILE:
    case MCFG_INT_MUXD_SW_ONLY_EFS_FILE:
    /* No path adjustments necessary outside of data profiles.
     * mcfg_item_type_int verified before entering this function */
      memscpy(f_name, MCFG_FILE_PATH_LENGTH, filePath, efs_item_header.hdr.mcfg_nv_item_length); 
      break;
    default: 
      MCFG_MSG_ERROR_1("MCFG validate: Invalid item type for mcfg_validate_efs_file: %d", mcfg_item_type_int);
      return FALSE;
  }
  MCFG_MSG_SPRINTF_1("mcfg_efs_read: EFS file path %s", f_name);

/* No use cases for Indexed EFS Item for now */
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
  uint8  subs_mask = 0;
  subs_id = mcfg_get_curr_subs_id();

  if(attributes & U_ITEM_ATTRIB_MULTISIM)
  {
    subs_mask = *fileData;
    mcfg_result = mcfg_proc_sub_relevant(subs_mask, subs_id);
    if (mcfg_result != MCFG_NV_STATUS_OK)
    {
      MCFG_MSG_ERROR_3("MCFG Validate: ignoring item for current subscription, subs_id: 0x%0x, subs_mask=%d, filepath %s", 
                           subs_id, subs_mask, filePath);
      return FALSE;
    }

    // Update the f_name for subs_id > 0  
    if(subs_id)
    {
      if(mcfg_item_type_int < MCFG_INT_DATA_PROFILE) 
      {
          (void)snprintf((f_name + (efs_item_header.hdr.mcfg_nv_item_length-1) ), 
                          MCFG_FILE_PATH_LENGTH - efs_item_header.hdr.mcfg_nv_item_length,
                          "_Subscription%02d", (int)subs_id);
      }
    }

    if ((MCFG_INT_DELETE_EFS_FILE == mcfg_item_type_int) ||
        (MCFG_INT_DELETE_DATA_PROFILE == mcfg_item_type_int))
    {
      MCFG_MSG_LOW_2("dete item payload - type %d, size: %d", mcfg_item_type_int, efs_item_body.hdr.mcfg_nv_item_length);
    }
    // First byte in case of multi-SIM EFS items is the subs_mask 
    /* Decrement the size by 1 to account for the subs_mask */
     fileData = (void *) (fileData + sizeof(uint8));
     efs_item_body.hdr.mcfg_nv_item_length--;
  }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

  f_name[MCFG_FILE_PATH_LENGTH-1] = '\0';
  // link to file if necessary
  if(attributes & U_ITEM_ATTRIB_MUXD)
  {
    mcfg_result = mcfg_nv_efs_update_symlink(f_name, efs_item_body.hdr.mcfg_nv_item_length);

    if(mcfg_result != MCFG_NV_STATUS_OK)
    {
      MCFG_MSG_ERROR_1("Could not update symlink for EFS File: %s", f_name);
      return FALSE;
    }
  }

  MCFG_MSG_LOW("Populate EFS buf");
  /* start populating efs data sturcutre here*/
  if (mcfg_result == MCFG_NV_STATUS_OK)
  {
    int fd;
    struct fs_stat efs_file_stat;
    int bytes_read;
    fs_size_t file_size=0;

    fd = efs_open(f_name, O_RDONLY); 
    // make sure file handle is valid
    if(fd < 0) 
    {
      switch (efs_errno)
      {
        case ENOENT:
          mcfg_result = MCFG_NV_STATUS_NOT_ACTIVE;
          break;

        case FS_ERANGE:
          mcfg_result = MCFG_NV_STATUS_BAD_SIZE;
          break;

        default:
          mcfg_result = MCFG_NV_STATUS_EFS_ERR;
          break;
      }
      if (mcfg_result != MCFG_NV_STATUS_NOT_ACTIVE)
      {
        MCFG_MSG_LOW_1("mcfg_efs_read: file handle error: %d", mcfg_result);
        return FALSE;
      }
    }
    else /* fd success */
    {
      if((efs_fstat(fd, &efs_file_stat) < 0) && (errno != MCFG_NV_STATUS_NOT_ACTIVE))
      {
        MCFG_MSG_ERROR_1("mcfg_read_log_info: efs_fstat returned negative value. errno = %d", efs_errno);
        return FALSE;
      }
    }

    MCFG_MSG_SPRINTF_1("mcfg_efs_read: full path f_name %s", f_name);
    
    if (mcfg_result == MCFG_NV_STATUS_OK)
    {
      /* Read the data from the log file if not empty */
      if(efs_file_stat.st_size != 0 ) 
      {
        file_size = efs_file_stat.st_size;
        file_ptr = mcfg_malloc(file_size);

        if(file_ptr == NULL) 
        {
          MCFG_MSG_SPRINTF_1("mcfg_validate_efs_file: malloc fail. item %s", f_name);
          efs_close(fd);
          return FALSE;
        }
        MCFG_MSG_SPRINTF_1("mcfg_efs_read: file handle open for %s", (char *) f_name);
        bytes_read = mcfg_efs_read(fd, file_ptr, file_size);

        efs_close(fd);

        if (bytes_read != file_size)
        {
          mcfg_free(file_ptr);
          MCFG_MSG_ERROR_2("mcfg_efs_read: error - unable to read full file. file_size: %lu, bytes_read %d", file_size, bytes_read);
          return FALSE;
        }
        else 
        {
          MCFG_MSG_LOW_1("mcfg_efs_read: read %d bytes from file", bytes_read);
        }
        /* Set comparison buffers */
        if (mcfg_result == MCFG_NV_STATUS_OK)
        {
          MCFG_MSG_LOW("Setting efs_data");
          efs_data.data_len = file_size;
          efs_data.data_ptr = file_ptr;
        }
      }

    }
    else if (mcfg_result == MCFG_NV_STATUS_NOT_ACTIVE)
    {
      /* ok if file not there. do not treat as error. just leave efs_config data as default */
    }
    else 
    {
      /* some error with stat file */
      MCFG_MSG_ERROR_1("mcfg_validate_efs_file: error getting filesize: err_val %d", mcfg_result);
      return FALSE;
    }
  }

  if(!(attributes & U_ITEM_ATTRIB_DELETE))
  {
    config_data.data_len = efs_item_body.hdr.mcfg_nv_item_length;
    config_data.data_ptr = fileData;
  }
  else
  {
    /* leave initialized to 0 */
  }

  if (!mcfg_validate_nvefs_data_match(&config_data, &efs_data))
  {
    MCFG_MSG_SPRINTF_1("MCFG validate: mcfg config & efs data mismatch for item %s", (char *) filePath);
    mcfg_result = mcfg_populate_and_send_xml_entry((uint8*) filePath,
                                     attributes,
                                     item_type,
                                     &config_data, 
                                     &efs_data);
    MCFG_MSG_LOW_1("MCFG validate: mcfg_nv_status after sending xml entry %d", (uint32) mcfg_result);
  }
  else 
  {
    MCFG_MSG_SPRINTF_1("MCFG validate: config & efs buffers match for item %s", filePath);
  }

  if (file_ptr != NULL)
  {
    mcfg_free(file_ptr);
  }

  return TRUE;
} /* mcfg_validate_efs_file */

/*===========================================================================
FUNCTION  mcfg_cfg_diff_nvefs

DESCRIPTION
  This compares differences between NV-EFS and modem config data
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  Result - Boolean (true or false depending on success / failure)

SIDE EFFECTS
  None
===========================================================================*/
boolean mcfg_cfg_diff_nvefs(uint8 cfg_type, uint32 num_mcfg_items, uint8* ram_cust_data_ptr, uint8 subs_id_input, boolean validate)
{
  nv_items_enum_type nv_mcfg_cust_item;
  uint16  nv_mcfg_cust_item_length;
  
  /* Maximum string length for an NV id + NULL terminator */
  char nv_item_name[5] = {0};

  uint32                 i;
  uint8                  mcfg_item_type_int;
  mcfg_item_prefix_type  mcfg_item_prefix;
  mcfg_nv_item_hdr_type *mcfg_nv_item_data_hdr;
  nv_item_type          *mcfg_nv_item_data_buf;
  nv_item_type          *nv_file_data_buf;
  nv_item_type          *temp_read_buf;
  mcfg_nv_status_e_type mcfg_nv_status = MCFG_NV_STATUS_OK;

  mcfg_data_len_ptr_type config_data = {0};
  mcfg_data_len_ptr_type efs_data    = {0};
  mcfg_data_len_ptr_type temp_data   = {0};
// TODO: Implement PRL support
  uint16  mcfg_prl_version;
  nv_ram_roaming_list_type  *prl_buf_ptr;

  boolean mcfg_update_nv_item_flag = TRUE;
  boolean mcfg_nvefs_update_status = FALSE;
  boolean delete_nv_flag = TRUE;
  uint8 item_index = 0;
  uint8 subs_mask = 0;
  uint8 subs_id;

  MCFG_MSG_MED_2("In mcfg_cfg_diff_nvefs, num_mcfg_items = %d subs_id = %d", (int)num_mcfg_items, (int)subs_id_input);

  /* Starts from 1 as the first element is MCFG_VERSION,
     ram_cust_data_ptr points to the start of MCFG Item after version */
  for (i = 1; i<num_mcfg_items; i++) 
  {
    MCFG_MSG_LOW_2("MCFG_Val: i = %d, ram_ptr = %X", i, ram_cust_data_ptr);
    /* Parse the MCFG Item Prefix for further processing */
    mcfg_item_type_int = mcfg_parse_item_prefix(cfg_type, ram_cust_data_ptr, &mcfg_item_prefix, MCFG_REFRESH_TYPE_SLOT_N_SUBS, subs_id_input);

    subs_id = mcfg_get_curr_subs_id();

    if (!validate)
    {
       if( (mcfg_restore_fact_def_validate == TRUE) && (!(mcfg_item_prefix.attrib & U_ITEM_ATTRIB_REST_FACT)) )
       {
         /* For Reset to Factory default case, make sure the U_ITEM_ATTRIB_REST_FACT attrib flag 
         is set before processing this item */
         mcfg_item_type_int = MCFG_INT_SW_ONLY;
       }
    }

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
    if( (subs_id != MCFG_SUB_ID_FIRST) && ( !(mcfg_item_prefix.attrib & U_ITEM_ATTRIB_MULTISIM) ))
    {
      if (validate)
      {
         /* If current subscription is not the primary sub,
            skip validation of items specific to primary sub*/
         ram_cust_data_ptr += mcfg_item_prefix.item_length;
         continue;
      }
      /* For Secondary and other subscriptions only process the multiSIM NVs from the config */
      mcfg_item_type_int = MCFG_INT_SW_ONLY;
    }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

    /* Set PTR to the start of the MCFG item data after prefix */
    ram_cust_data_ptr += sizeof(mcfg_item_prefix);

    switch (mcfg_item_type_int) 
    {
      /* Carrier Specific NV Item, Static Non-Muxd */
      case MCFG_INT_NV_ITEM:
		    /* Get the hdr */
        mcfg_nv_item_data_hdr =  mcfg_malloc( sizeof(uint8)* MCFG_VER_NV_HEADER_LEN );
        if( mcfg_nv_item_data_hdr == NULL) 
        {
          MCFG_MSG_ERROR("MCFG: Memory allocation failure while reading MCFG_INT_NV_ITEM");
          break;
        }

        memscpy(mcfg_nv_item_data_hdr, MCFG_VER_NV_HEADER_LEN * sizeof(uint8),
                ram_cust_data_ptr, (MCFG_VER_NV_HEADER_LEN * sizeof(uint8)) ); 

        /* Get the NV item type and length */
        nv_mcfg_cust_item = (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_type;
        nv_mcfg_cust_item_length = (uint16) mcfg_nv_item_data_hdr->mcfg_nv_item_length;
          

        MCFG_MSG_LOW_1("MCFG: malloc for size: %d", nv_mcfg_cust_item_length);
        mcfg_nv_item_data_buf =  mcfg_malloc( sizeof(uint8)* nv_mcfg_cust_item_length); 
        if( mcfg_nv_item_data_buf == NULL) 
        {
          MCFG_MSG_ERROR("MCFG: Memory allocation failure while reading MCFG_INT_NV_ITEM");
          mcfg_free(mcfg_nv_item_data_hdr);
          /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
          ram_cust_data_ptr += nv_mcfg_cust_item_length;
          break;
        }

        /* Process the WRITE_ONCE attrib */ 
        /* Check for write once and set the  mcfg_update_nv_item_flag */
        /* if write_once attrib is set - update only if rest fact default */
        /* Note - Write-Once attrib only applies to Primary subs, i.e subs=0 */
        if( (mcfg_item_prefix.attrib & U_ITEM_ATTRIB_WRITE_ONCE) &&
            (mcfg_restore_fact_def_validate == FALSE) && (subs_id == MCFG_SUB_ID_FIRST )) 
        {
          /* If this NV Item exists, i.e read successful set */
          if( mcfgnv_read(nv_mcfg_cust_item, mcfg_item_prefix.attrib, 
                          (uint8*)mcfg_nv_item_data_buf, nv_mcfg_cust_item_length) == MCFG_NV_STATUS_NOT_ACTIVE )
          {
            mcfg_update_nv_item_flag = TRUE;
          }
          else
          {
            mcfg_update_nv_item_flag = FALSE;
          }
        }
        else
        {    
          mcfg_update_nv_item_flag = TRUE;
        }

        /* Write the Mcfg NV item. The mcfgnv_write function uses 
           the indexed/muxd/update attributes for further processing */
        ram_cust_data_ptr += MCFG_VER_NV_HEADER_LEN;

        if(validate || mcfg_update_nv_item_flag)
        {
          memscpy(mcfg_nv_item_data_buf, 
                  (nv_mcfg_cust_item_length*sizeof(uint8)),
                  ram_cust_data_ptr, 
                  (nv_mcfg_cust_item_length*sizeof(uint8)) );

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT           
          if (!validate)
          {
            mcfg_nv_status = mcfgnv_remove_symlink_if_present( nv_mcfg_cust_item,
                                           mcfg_item_prefix.attrib,   
                                           (uint8 *)mcfg_nv_item_data_buf,
                                           nv_mcfg_cust_item_length,
                                           FALSE );
          }
#endif           
          if (validate)
          {
            uint32 size = 0;
            
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
            subs_mask = 0;
            /* Skip multiSIM settings that don't pertain to subs_id */
            if (mcfg_item_prefix.attrib & U_ITEM_ATTRIB_MULTISIM)
            {
              MCFG_MSG_LOW_1("MCFG: mutisim item %d", nv_mcfg_cust_item);
              if(!mcfg_get_item_metadata((uint8 *)mcfg_nv_item_data_buf, mcfg_item_prefix.attrib, U_ITEM_ATTRIB_MULTISIM, &subs_mask))
              {
                MCFG_MSG_LOW_1("MCFG: could not get subs_mask for item %d", nv_mcfg_cust_item);
              }
              else 
              {
                MCFG_MSG_LOW_2("MCFG: got subs_mask %d for item %d", subs_mask, nv_mcfg_cust_item);
                MCFG_MSG_HIGH_3("MultiSIM: mcfg_cfg_diff_nvefs - item %d, subs_mask=%d, curr_subs_id=%d", 
                                 (int)nv_mcfg_cust_item, subs_mask, subs_id);
                if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
                {
                   MCFG_MSG_HIGH_3("MultiSIM: mcfg_cfg_diff_nvefs - ignoring item %d, subs_mask=%d, curr_subs_id=%d", 
                                    (int)nv_mcfg_cust_item, subs_mask, subs_id);

                   MCFG_MSG_LOW("MCFG validate: free header ptr");
                   mcfg_free(mcfg_nv_item_data_hdr);
                   MCFG_MSG_LOW("MCFG validate: free data ptr");
                   mcfg_free(mcfg_nv_item_data_buf);
                   /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
                   ram_cust_data_ptr += nv_mcfg_cust_item_length;
                   break;
                }
              }
            }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

            /* Initialize data bufs */
            (void) mcfg_validate_init_xml_data_bufs(&config_data, &efs_data);

            /* Print the name of the NV item in buffer */
            if(snprintf(nv_item_name, sizeof(nv_item_name), "%d", (int)nv_mcfg_cust_item)<0)
            {
               MCFG_MSG_ERROR_1("MCFG validate: buf print failure for nv %d", nv_mcfg_cust_item);
               mcfg_nv_status = MCFG_NV_STATUS_BAD_SIZE;
            }
            MCFG_MSG_LOW_1("MCFG validate: buf print success for nv %d", nv_mcfg_cust_item);

            if (mcfg_nv_status != MCFG_ERR_NONE)
            {
              MCFG_MSG_LOW("MCFG validate: free header ptr");
              mcfg_free(mcfg_nv_item_data_hdr);
              MCFG_MSG_LOW("MCFG validate: free data ptr");
              mcfg_free(mcfg_nv_item_data_buf);
              /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
              ram_cust_data_ptr += nv_mcfg_cust_item_length;
              break;
            }

            /* get size of actual file be used */
            mcfg_nv_status = mcfg_get_item_size(nv_mcfg_cust_item, 0, subs_id, &size);

            /* OK for the status to be inactive */
            if((mcfg_nv_status != MCFG_NV_STATUS_NOT_ACTIVE) && 
               (mcfg_nv_status != MCFG_NV_STATUS_OK))
            {
              MCFG_MSG_ERROR_1("MCFG Validate: get_item_size fail for item %d", nv_mcfg_cust_item);
              MCFG_MSG_LOW("MCFG validate: about to free header ptr");
              mcfg_free(mcfg_nv_item_data_hdr);
              MCFG_MSG_LOW("MCFG validate: about to free data ptr");
              mcfg_free(mcfg_nv_item_data_buf);
              mcfg_nv_status = MCFG_NV_STATUS_NOT_ACTIVE;
              /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
              ram_cust_data_ptr += nv_mcfg_cust_item_length;
              break;
            }

            nv_file_data_buf =  mcfg_malloc(size * sizeof(uint8)); 
            if(nv_file_data_buf == NULL) 
            {
              MCFG_MSG_ERROR("MCFG Validate: malloc fail for file data buf");
              MCFG_MSG_LOW("MCFG validate: about to free header ptr");
              mcfg_free(mcfg_nv_item_data_hdr);
              MCFG_MSG_LOW("MCFG validate: about to free data ptr");
              mcfg_free(mcfg_nv_item_data_buf);
              mcfg_nv_status = MCFG_NV_NULL_PTR;
              /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
              ram_cust_data_ptr += nv_mcfg_cust_item_length;
              break;
            }

            temp_data.data_len = size;
            temp_data.data_ptr = (uint8 *) nv_file_data_buf;
            /* The index & subs_id aren't included in the NV after it's been written to EFS,
               so no need to adjust buffer as in the case of when item is in MCFG config */


            /* TODO: Find way to determine actualy index instead of defaulting to 0 */
            mcfg_nv_status = mcfg_nv_read(nv_mcfg_cust_item, 0, subs_id, 
                                         (void *)temp_data.data_ptr, temp_data.data_len);
            
            MCFG_MSG_LOW_1("MCFG validate: mcfgnv_read status %d", mcfg_nv_status);

            if (mcfg_nv_status == MCFG_NV_STATUS_OK)
            {
              MCFG_MSG_LOW("Setting efs_data");
              efs_data = temp_data;
            }

            config_data.data_len = nv_mcfg_cust_item_length;
            config_data.data_ptr = (uint8 *)mcfg_nv_item_data_buf;

            (void) mcfg_validate_adjust_xml_data_buf_size(&config_data, mcfg_item_prefix.attrib);

            MCFG_MSG_LOW("MCFG validate: data bufs set");
            
            if (!mcfg_validate_nvefs_data_match(&config_data, &efs_data))
            {
              
              MCFG_MSG_LOW_1("MCFG validate: mcfg config & efs data mismatch for item %d", nv_mcfg_cust_item);
              mcfg_nv_status = mcfg_populate_and_send_xml_entry((uint8*) nv_item_name,
                                               mcfg_item_prefix.attrib,
                                               mcfg_item_prefix.item_type,
                                               &config_data, 
                                               &efs_data);
              MCFG_MSG_LOW_1("MCFG validate: mcfg_nv_status after sending xml entry %d", (uint32) mcfg_nv_status);
            }
            else 
            {
              MCFG_MSG_LOW_1("MCFG validate: config & efs buffers match for item %d", nv_mcfg_cust_item);
            }


            if( mcfg_nv_status != MCFG_NV_STATUS_OK) 
            {
              MCFG_MSG_ERROR("MCFG validate: XML entry failure");
              MCFG_MSG_LOW("MCFG validate: free header ptr");
              mcfg_free(mcfg_nv_item_data_hdr);
              MCFG_MSG_LOW("MCFG validate: free data ptr");
              mcfg_free(mcfg_nv_item_data_buf);
              MCFG_MSG_LOW("MCFG validate: free file ptr");
              mcfg_free(nv_file_data_buf);
              /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
              ram_cust_data_ptr += nv_mcfg_cust_item_length;
              break;
            }
          }
          else 
          {
             mcfg_nv_status = mcfgnv_write( nv_mcfg_cust_item,
                                            mcfg_item_prefix.attrib,   
                                            (uint8 *)mcfg_nv_item_data_buf,
                                            nv_mcfg_cust_item_length );

             if(mcfg_nv_status != MCFG_NV_STATUS_OK )  
             {
               MCFG_MSG_ERROR_2("MCFG Static NV item write failed, item=%d, status=%d",
                                 nv_mcfg_cust_item, mcfg_nv_status );
               mcfg_nvefs_update_status = FALSE;
             }
          }
        }

        MCFG_MSG_LOW_1("MCFG validate: mcfg_nv_status before exiting NV case %d", (uint32) mcfg_nv_status);

        /* Free the MCFG nv hdr and buf */
        if (validate)
        {
          MCFG_MSG_LOW("MCFG validate: free file ptr");
          mcfg_free(nv_file_data_buf);
        }
        MCFG_MSG_LOW("MCFG validate: free header ptr");
        mcfg_free(mcfg_nv_item_data_hdr);
        MCFG_MSG_LOW("MCFG validate: free data ptr");
        mcfg_free(mcfg_nv_item_data_buf);

        /* point to the next MCFG item to be written */
        /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
        ram_cust_data_ptr += nv_mcfg_cust_item_length;
        break;

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
      /* Carrier Specific NV Item Muxd items */
      case MCFG_INT_MUXD_NV_ITEM:

        /* Get the hdr */
        mcfg_nv_item_data_hdr =  mcfg_malloc( sizeof(uint8)* MCFG_VER_NV_HEADER_LEN );
        if( mcfg_nv_item_data_hdr == NULL) 
        {
          MCFG_MSG_ERROR("MCFG: Memory allocation failure while reading MCFG_INT_MUXD_NV_ITEM");
          break;
        }

        memscpy(mcfg_nv_item_data_hdr, (MCFG_VER_NV_HEADER_LEN * sizeof(uint8)),
                ram_cust_data_ptr, (MCFG_VER_NV_HEADER_LEN * sizeof(uint8))); 

        /* Get the NV item type and length */
        nv_mcfg_cust_item = (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_type;
        nv_mcfg_cust_item_length = (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_length;
          
        /* Get the value */
        ram_cust_data_ptr += MCFG_VER_NV_HEADER_LEN;
        mcfg_nv_item_data_buf =  mcfg_malloc( sizeof(uint8)* nv_mcfg_cust_item_length); 
        if( mcfg_nv_item_data_buf == NULL) 
        {
          mcfg_free(mcfg_nv_item_data_hdr);
          MCFG_MSG_ERROR("MCFG: Memory allocation failure");
          break;
        }
        memscpy(mcfg_nv_item_data_buf, sizeof(uint8)* nv_mcfg_cust_item_length,
                ram_cust_data_ptr, (nv_mcfg_cust_item_length*sizeof(uint8)) );

        /* First update the symlink for the Muxd NV Items */
        mcfg_nv_status = mcfgnv_update_symlink( nv_mcfg_cust_item, mcfg_item_prefix.attrib, 
                                                (uint8 *)mcfg_nv_item_data_buf, nv_mcfg_cust_item_length );
        if(mcfg_nv_status != MCFG_NV_STATUS_OK )
        {
          MCFG_MSG_ERROR_2("Unable to update symlink for Muxd NV Item %d, status=%d", 
                            nv_mcfg_cust_item, mcfg_nv_status);
        }
        else
        {
          /* Symlink updated successfully, now go ahead and do the processing, write as before */
 
          /* Process the WRITE_ONCE attrib */ 
          /* Check for write once and set the  mcfg_update_nv_item_flag */
          /* if write_once attrib is set - update only if rest fact default */
          if( (mcfg_item_prefix.attrib & U_ITEM_ATTRIB_WRITE_ONCE) &&
              (mcfg_restore_fact_def_validate == FALSE) && (subs_id == MCFG_SUB_ID_FIRST )) 
          {
            /* using temporary buffer to read to so as not to overwrite mcfg_nv_item_data_buf */
            temp_read_buf =  mcfg_malloc( sizeof(uint8)* nv_mcfg_cust_item_length); 
            if(temp_read_buf == NULL) 
            {
              mcfg_free(mcfg_nv_item_data_hdr);
              mcfg_free(mcfg_nv_item_data_buf);
              MCFG_MSG_ERROR("MCFG: Memory allocation failure");
              break;
            }

            /* If this NV Item exists, i.e read successful set */
            if( mcfgnv_read(nv_mcfg_cust_item, mcfg_item_prefix.attrib, 
                            (uint8*)temp_read_buf, nv_mcfg_cust_item_length) == MCFG_NV_STATUS_NOT_ACTIVE )
            {
              mcfg_update_nv_item_flag = TRUE;
            }
            else
            {
              mcfg_update_nv_item_flag = FALSE;
            }
           
            mcfg_free(temp_read_buf);
          }
          else
          {    
            mcfg_update_nv_item_flag = TRUE;
          }
  
          /* Process the MUXD attrib */ 
          /* MCFG Multiplexed NV Items, update only when Mfg version != Muxd Mcfg version */
          if( (mcfg_ver_compare_result_validate == MCFG_VER_EQUALS_MUXD_VERSION ) &&  
              (mcfg_restore_fact_def_validate == FALSE) 
             )
          {
            MCFG_MSG_HIGH_1("Since Mfg version=Muxd Mcfg version, not updating muxd item= %d", nv_mcfg_cust_item);
            mcfg_update_nv_item_flag = FALSE;
          }
          else
          {
            mcfg_update_nv_item_flag = TRUE;
          }
  
          /* Process the Conditional attrib for PRL if set */ 
          if(mcfg_update_nv_item_flag && (mcfg_item_prefix.attrib & U_ITEM_ATTRIB_CONDITIONAL) )
          {
            /* Evaluate to upadate or not based on more checks, depending on the Item#  */
            /* For PRL the case is */
            MCFG_MSG_HIGH_2("In processing for U_ITEM_ATTRIB_CONDITIONAL, item=%d, flag=%d", nv_mcfg_cust_item,mcfg_update_nv_item_flag );
            if(nv_mcfg_cust_item == NV_ROAMING_LIST_683_I)
            {
              // Get PRL version from MCFG 
              memscpy(&mcfg_prl_version, sizeof(mcfg_prl_version),
                      (ram_cust_data_ptr+MCFG_PRL_VER_OFFSET),sizeof(uint16));
  
              // Read PRL version from NV
              prl_buf_ptr =  mcfg_malloc( sizeof(nv_ram_roaming_list_type) );
              if(prl_buf_ptr)
              {
                prl_buf_ptr->nam = 0;
                mcfg_nv_status = mcfgnv_read( NV_ROAMING_LIST_683_I, mcfg_item_prefix.attrib,
                                              (uint8 *)prl_buf_ptr, sizeof(nv_ram_roaming_list_type) );
                if (mcfg_nv_status == MCFG_NV_STATUS_OK)
                {
                  /* Set mcfg_update_nv_item_flag to FALSE if mcfg_prl_version < nv_prl_version */
                  if(mcfg_prl_version  < prl_buf_ptr->prl_version )
                  {
                    mcfg_update_nv_item_flag = FALSE;
                  }
                }
                else
                {
                  MCFG_MSG_ERROR_1("Unable to read PRL version from NV, status= 0x%0x", mcfg_nv_status);
                }
    
                mcfg_free(prl_buf_ptr);
              }
              else
              {
                MCFG_ERR_FATAL("MCFG: Memory allocation failure while reading PRL version from NV", 0,0,0);
              }
  
            } /* (nv_mcfg_cust_item == NV_ROAMING_LIST_683_I)*/
  
            // Any future conditional items/logic  go here
  
          } /* U_ITEM_ATTRIB_CONDITIONAL processing */
  
          /* Finally write the Muxd NV Item if the mcfg_update_nv_item_flag is set */  
          if(validate || mcfg_update_nv_item_flag)
          {
            if (validate)
            {
              /* TODO: Determine what to do for this type */
            }
            else 
            {
              mcfg_nv_status = mcfgnv_write( nv_mcfg_cust_item,
                                                   mcfg_item_prefix.attrib,   
                                                   (uint8 *)mcfg_nv_item_data_buf,
                                                   nv_mcfg_cust_item_length );

              if(mcfg_nv_status != MCFG_NV_STATUS_OK )  
              {
                MCFG_MSG_ERROR_2("MCFG NV item write failed, item=%d, status=%d",
                                  nv_mcfg_cust_item, mcfg_nv_status );
                mcfg_nvefs_update_status = FALSE;
              }
            }
          }
  
        }
        /* Free the MCFG nv hdr and buf */
        mcfg_free(mcfg_nv_item_data_hdr);
        mcfg_free(mcfg_nv_item_data_buf);

        /* point to the next MCFG item to be written */
        /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
        ram_cust_data_ptr += nv_mcfg_cust_item_length;
        break;
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */

        case MCFG_INT_DELETE_NV_ITEM:
          /* Get the hdr */
          delete_nv_flag = TRUE;
          mcfg_nv_item_data_hdr =  mcfg_malloc( sizeof(uint8)* MCFG_VER_NV_HEADER_LEN );
          if( mcfg_nv_item_data_hdr == NULL) 
          {
            MCFG_MSG_ERROR("MCFG: Memory allocation failure while reading MCFG_INT_DELETE_NV_ITEM");
            break;
          }

          memscpy(mcfg_nv_item_data_hdr, sizeof(uint8)* MCFG_VER_NV_HEADER_LEN,
                  ram_cust_data_ptr, (MCFG_VER_NV_HEADER_LEN * sizeof(uint8)) ); 

          /* Get the NV item type and length */
          nv_mcfg_cust_item = (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_type;
          nv_mcfg_cust_item_length = (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_length;

          ram_cust_data_ptr += MCFG_VER_NV_HEADER_LEN;

          /* If this MCFG item is a legacy or MCFG NV item... */
          if( ( nv_mcfg_cust_item < NV_MAX_I ) ||
              ( (nv_mcfg_cust_item > (nv_items_enum_type) MCFG_NVI_ITEM_FILES_START) && 
                (nv_mcfg_cust_item < (nv_items_enum_type) MCFG_NVI_ITEM_FILES_END) )
            )
          {
            mcfg_nv_status_e_type mcfg_nv_del_status = MCFG_NV_STATUS_OK;
            uint8 index = 0;
            uint8* dataSubID_ptr;
            uint32 size = 0;

            dataSubID_ptr = ram_cust_data_ptr;

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
            /* If the item is MultiSIM type ... */
            uint8 subs_mask = 0;
            if ( ( mcfg_item_prefix.attrib & U_ITEM_ATTRIB_MULTISIM ) && (nv_mcfg_cust_item != NV_ROAMING_LIST_683_I) )
            {
              /* ...then, get the subs_mask from the MCFG data. */
              subs_mask = *(dataSubID_ptr);
              dataSubID_ptr++;

              if(subs_mask > MCFG_SUBS_MASK)
              {
                delete_nv_flag = FALSE;
                mcfg_nv_del_status = MCFG_NV_STATUS_BAD_ITEM;
                MCFG_MSG_ERROR_3("MultiSIM: mcfgnv_update_nvefs MCFG_NV_STATUS_BAD_ITEM - MCFG Item %d, subs_id=%d, subs_mask=%d!!", 
                           (int)nv_mcfg_cust_item, subs_id, subs_mask);
              }

              if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
              {
                delete_nv_flag = FALSE;  
                MCFG_MSG_HIGH_3("MultiSIM NV delete,mcfgnv_update_nvefs - ignoring delete for eItem = %d, subs_mask=%d, curr_subs_id=%d", 
                          (int) nv_mcfg_cust_item, subs_mask, subs_id);
              }
            }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

             /* If the item is indexed... */
            if ( mcfg_item_prefix.attrib & U_ITEM_ATTRIB_INDEXED )
            {
              /* ...then, get the index from the MCFG data. */
              index = *(dataSubID_ptr);
            }

            /* Delete the NV item. */
            if(validate || delete_nv_flag)
            { 
              if (validate)
              {
                MCFG_MSG_LOW_2("MCFG validate: validating delete item %d with attrib 0x%02X",
                            nv_mcfg_cust_item, mcfg_item_prefix.attrib);

                /* Skip multiSIM settings that don't pertain to subs_id */
                if (mcfg_item_prefix.attrib & U_ITEM_ATTRIB_MULTISIM)
                {
                  if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
                  {
                     MCFG_MSG_HIGH_3("MultiSIM: mcfg_cfg_diff_nvefs - ignoring delete item %d, subs_mask=%d, curr_subs_id=%d", 
                                      (int)nv_mcfg_cust_item, subs_mask, subs_id);

                     MCFG_MSG_LOW("MCFG validate: free header ptr");
                     /* Technically still a good item..but need to skip*/
                     mcfg_nv_del_status = MCFG_NV_STATUS_BAD_ITEM;
                  }
                }

                /* Initialize data bufs */
                /* config_data will remain empty for del items, efs_data empty for missing files */
                (void) mcfg_validate_init_xml_data_bufs(&config_data, &efs_data);

                if (mcfg_nv_del_status == MCFG_NV_STATUS_OK)
                {
                  /* Print the name of the NV item in buffer */
                  if(snprintf(nv_item_name, sizeof(nv_item_name), "%d", (int)nv_mcfg_cust_item)<0)
                  {
                     MCFG_MSG_ERROR_1("MCFG validate: buf print failure for nv %d", nv_mcfg_cust_item);
                     mcfg_nv_status = MCFG_NV_STATUS_BAD_SIZE;
                  }
                  MCFG_MSG_ERROR_1("MCFG validate: buf print success for nv %d", nv_mcfg_cust_item);
                }

                if (mcfg_nv_del_status == MCFG_NV_STATUS_OK)
                {
                  mcfg_nv_del_status = mcfg_get_item_size(nv_mcfg_cust_item, index, subs_id, &size);
                  /* OK for the status to be inactive */
                  if (mcfg_nv_del_status == MCFG_NV_STATUS_NOT_ACTIVE)
                  {
                    mcfg_nv_del_status = MCFG_NV_STATUS_OK;
                  }
                  else if (mcfg_nv_del_status == MCFG_NV_STATUS_OK)
                  {
                    /* nature of delete item requires the size of actual file be used */
                    nv_file_data_buf =  mcfg_malloc(size); 
                    if(nv_file_data_buf == NULL) 
                    {
                      MCFG_MSG_ERROR("MCFG Validate: malloc fail for file data buf");
                      mcfg_nv_del_status = MCFG_NV_NULL_PTR;
                    }
                  }
                }

                if (mcfg_nv_del_status == MCFG_NV_STATUS_OK)
                {
                  temp_data.data_len = size;
                  temp_data.data_ptr = (uint8 *)nv_file_data_buf;

                  mcfg_nv_del_status = mcfg_nv_read(nv_mcfg_cust_item, index, subs_id, 
                                               (uint8 *)temp_data.data_ptr, temp_data.data_len);
                  MCFG_MSG_LOW_1("MCFG validate: mcfgnv_read status %d", mcfg_nv_del_status);
                }

                if (mcfg_nv_del_status == MCFG_NV_STATUS_OK)
                {
                  MCFG_MSG_LOW("Setting efs_data");
                  efs_data = temp_data;
                }

                if (!mcfg_validate_nvefs_data_match(&config_data, &efs_data))
                {
                  MCFG_MSG_LOW_1("MCFG validate: mcfg config & efs data mismatch for item %d", nv_mcfg_cust_item);
                  mcfg_nv_del_status = mcfg_populate_and_send_xml_entry((uint8*) nv_item_name,
                                                   mcfg_item_prefix.attrib,
                                                   mcfg_item_prefix.item_type,
                                                   &config_data, 
                                                   &efs_data);
                  MCFG_MSG_LOW_1("MCFG validate: mcfg_nv_status after sending xml entry %d", (uint32) mcfg_nv_del_status);
                }
                else 
                {
                  MCFG_MSG_LOW_1("MCFG validate: config & efs buffers match for item %d", nv_mcfg_cust_item);
                }
              }
              else 
              {
                mcfg_nv_del_status = mcfgnv_delete(nv_mcfg_cust_item, mcfg_item_prefix.attrib, index, subs_id);
                if (mcfg_nv_del_status != MCFG_NV_STATUS_OK)
                {
                  MCFG_MSG_ERROR_2("MCFG NV delete failed, item=%d, err=%d.",
                                 nv_mcfg_cust_item, mcfg_nv_del_status );
                }
              }
            }
          }
          else
          {
            MCFG_MSG_ERROR_1( "Unknown MCFG NV Item for delete, item=%d", nv_mcfg_cust_item);
          }

          if (nv_file_data_buf != NULL)
          {
            mcfg_free(nv_file_data_buf);
          }
          /* Free the MCFG NV hdr */
          mcfg_free(mcfg_nv_item_data_hdr);

          /* point to the next NV or MCFG item to be written */
          /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
          ram_cust_data_ptr += nv_mcfg_cust_item_length;
          break;

        case MCFG_INT_EFS_FILE:
        case MCFG_INT_STATIC_WO_EFS_FILE:
        case MCFG_INT_MUXD_EFS_FILE:
          if (validate)
          {
            if(!mcfg_validate_efs_file(ram_cust_data_ptr, &mcfg_item_prefix, mcfg_item_type_int))
            {
              mcfg_nvefs_update_status = FALSE;
            }
          }
          else 
          {
            if(!mcfg_create_efs_file(ram_cust_data_ptr, mcfg_item_prefix.attrib, mcfg_restore_fact_def_validate))
            {
              mcfg_nvefs_update_status = FALSE;
            }
          }

          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;
        case MCFG_INT_DELETE_EFS_FILE:
          MCFG_MSG_HIGH("Encountered delete EFS File");
          if (validate)
          {
            if(!mcfg_validate_efs_file(ram_cust_data_ptr, &mcfg_item_prefix, mcfg_item_type_int))
            {
              mcfg_nvefs_update_status = FALSE;
            }
          }
          else 
          {
            if(MCFG_NV_STATUS_OK != mcfg_delete_efs_file(ram_cust_data_ptr, mcfg_item_prefix.attrib))
            {
              mcfg_nvefs_update_status = FALSE;
            }
          }

          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

          /* ========= Data Profile ========== */
        case MCFG_INT_DATA_PROFILE:
        case MCFG_INT_STATIC_WO_DATA_PROFILE:
        case MCFG_INT_MUXED_DATA_PROFILE:
          MCFG_MSG_LOW_2("Encountered data profile type %d index %d", 
                         mcfg_item_type_int, i);
          if (validate)
          {
            if(!mcfg_validate_efs_file(ram_cust_data_ptr, &mcfg_item_prefix, mcfg_item_type_int))
            {
              mcfg_nvefs_update_status = FALSE;
            }
          }
          else 
          {
            if(!mcfg_create_data_profile(ram_cust_data_ptr, mcfg_item_prefix.attrib, mcfg_restore_fact_def_validate))
            {
              mcfg_nvefs_update_status = FALSE;
            }
          }
 
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

        case MCFG_INT_DELETE_DATA_PROFILE:
          MCFG_MSG_HIGH("Encountered delete Profile");
          if (validate)
          {
            if(!mcfg_validate_efs_file(ram_cust_data_ptr, &mcfg_item_prefix, mcfg_item_type_int))
            {
              mcfg_nvefs_update_status = FALSE;
            }
          }
          else 
          {
            if(MCFG_NV_STATUS_OK != mcfg_delete_data_profile(ram_cust_data_ptr, mcfg_item_prefix.attrib))
            {
              mcfg_nvefs_update_status = FALSE;
            }
          }
 
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

        case MCFG_INT_MUXD_SW_ONLY:
          /* Muxd Software only - Update the symbolic links for this NV items */

          /* Get the hdr */
          mcfg_nv_item_data_hdr =  mcfg_malloc( sizeof(uint8)* MCFG_VER_NV_HEADER_LEN );
          if( mcfg_nv_item_data_hdr == NULL) 
          {
            MCFG_MSG_ERROR("MCFG: Memory allocation failure while reading MCFG_INT_MUXD_SW_ONLY");
            break;
          }
          memscpy(mcfg_nv_item_data_hdr, sizeof(uint8)* MCFG_VER_NV_HEADER_LEN,
                  ram_cust_data_ptr, (MCFG_VER_NV_HEADER_LEN * sizeof(uint8)) ); 

          /* Get the NV item type and length */
          nv_mcfg_cust_item = (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_type;
          nv_mcfg_cust_item_length = (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_length;


          item_index = 0;

          if(mcfg_item_prefix.attrib & U_ITEM_ATTRIB_INDEXED) {
            nv_mcfg_cust_item_length--;

            // read the index. it is placed right after the header
            item_index = *(ram_cust_data_ptr + MCFG_VER_NV_HEADER_LEN);
          }

          if (validate)
          {
            /* TODO: Determine what to do for this type */
          }
          else 
          {
            /* Update the symlink for the Muxd NV Items */
            mcfg_nv_status = mcfg_nv_update_symlink(nv_mcfg_cust_item, item_index, subs_id, nv_mcfg_cust_item_length);

            if(mcfg_nv_status != MCFG_NV_STATUS_OK )
            {
              MCFG_MSG_ERROR_2("Unable to create symlink for Muxd SW NV Item %d, status=%d", 
                              nv_mcfg_cust_item, mcfg_nv_status);
            }
          }

          /* Free the MCFG NV hdr */
          mcfg_free(mcfg_nv_item_data_hdr);

          /* Advance to the next MCFG item */
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

        case MCFG_INT_SW_ONLY:
          /* Advance to the next MCFG item */
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

        case MCFG_INT_MUXD_SW_ONLY_EFS_FILE:
          /* TODO - not supported for now, advance to the next MCFG item */
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

        default:
          /* Unknown - Err fatal should never be here */
          MCFG_ERR_FATAL("MCFG Parsing Error %d", mcfg_item_type_int, 0, 0); 
          break;
      }

      mcfg_nvefs_update_status = TRUE;
    }

  return mcfg_nvefs_update_status;
  
} /* mcfg_cfg_diff_nvefs() */
