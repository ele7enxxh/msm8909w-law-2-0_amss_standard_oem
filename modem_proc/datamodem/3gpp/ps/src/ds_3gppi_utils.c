/*!
  @file
  ds_3gppi_utils.c

  @brief
  Internal utility functions and routines

  @detail
  OPTIONAL detailed description of this C file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gppi_utils.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/13   vs      NV item to check call origination is allowed before PS attach
02/06/13   nd      NV item to make dual IP fallback cause codes configurable.
11/22/11   nd      Added NV item ds_3gpp_num_persistent_profiles.
11/22/11   nd      Added NV item ds_3gpp_send_ps_data_avail_on_power_save.
09/12/12   ss      Removing \n character from path string values in 
                   ds_3gppi_cfg_item_info and moving the logic for \n 
                   addition it in ds_3gpp_update_data_config_info()
08/13/12   ss      Moving efs write operation to dsutil performed during 
                   bootup. Added function to be called from dsutil for all 
                   3gpp specific items to be written.
08/08/12   ss      Added NV item 3gpp_tcp_threshold_ack_cnt.
04/09/12   vs      Use strlen instead of sizeof to determine length of item file.
03/13/12   kv      Fixed compilation warnings.
02/23/12   kv      Replaced FEATURE_DATA_TEST_LOOPBACK_HDLR with
                   FEATURE_DATA_TEST_LOOPBACK.
11/22/11   sa      Added NV item 3gpp_auto_answer_enable.
07/15/11   sa      Added NV item 3gpp_test_loopback_enable.
06/06/11   sa      Added NV item 3gpp_data_pkt_orig_str.
05/25/11   sa      Added support for 3gpp_rel_version NV.
04/04/10   vk      Added support for configuration get/set APIs
01/23/10   vk      Initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "amssassert.h"

#include <stringl/stringl.h>
#include "mcfg_fs.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "dsutil.h"
#include "ds3gcfgmgr.h"
#include "nv.h"
#include "ds_system_heap.h"
#include "ds3gpp_api.h"
#include "ds_3gpp_rpm.h"
#include "ds_3gpp_nv_manager.h"
#ifdef FEATURE_DATA_LTE
#include "ds_eps_pdn_context.h"
#endif
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_3GPPI_UTILS_APN_DISCONN_ORDER_LIST_FILE \
            "/nv/item_files/modem/data/3gpp/ps/apn_disconnect_priority.txt"

/*---------------------------------------------------------------------------
  Macro defining the max integer value of a valid configuration item id + 1.
  Used for validation of item
---------------------------------------------------------------------------*/
#define DS_3GPP_CFG_MAX ((int)DS_3GPP_CFG_ILLEGAL)



/*---------------------------------------------------------------------------
  Data type used to hold static info about configuration item
---------------------------------------------------------------------------*/
typedef struct {
    ds_3gpp_cfg_item_e  ident; /* Item identifier */
    const char        * desc;  /* Human readable description of item */
    const char        * path;  /* EFS path where the item resides */
    size_t              size;  /* size of item in bytes */
} ds_3gppi_cfg_item_info_t;

/*---------------------------------------------------------------------------
  Accessor macros to access individual fields from config item info
---------------------------------------------------------------------------*/
#define DS_3GPPI_CFG_ITEM_IDENT(item_info_p) ((item_info_p)->ident)
#define DS_3GPPI_CFG_ITEM_DESC(item_info_p)  ((item_info_p)->desc)
#define DS_3GPPI_CFG_ITEM_PATH(item_info_p)  ((item_info_p)->path)
#define DS_3GPPI_CFG_ITEM_SIZE(item_info_p)  ((item_info_p)->size)

/*---------------------------------------------------------------------------
   List of APN identifiers read in order from the EFS during power up.
   This list of identifiers read, will be used during a prioritized PDN bring
   up when there are not sufficient bearer resources available.
   (Eg: During emergency PDN bring up)
   At the time of usage this pointer will be type casted to
   'ds_3gpp_apn_disconn_order_list_type'
  ---------------------------------------------------------------------------*/
  void *ds_3gpp_apn_disconn_order_list[DS3GSUBSMGR_SUBS_ID_MAX][DS_3GPP_MAX_APN_IDENTIFIERS];

/*---------------------------------------------------------------------------
  Prefix applied to each item path to derive the full path name of config
  items in EFS
---------------------------------------------------------------------------*/
static const char * ds_3gppi_cfg_path_prefix = "/nv/item_files/";


/*---------------------------------------------------------------------------
  Static array of configuration item info. Each configuration item must be
  added here.
---------------------------------------------------------------------------*/

#define DS_3GPPI_CFG_ITEM_ENTRY(                             \
                                  ds_3gppi_cfg_item,         \
                                  ds_3gppi_cfg_description,  \
                                  ds_3gppi_cfg_path,         \
                                  ds_3gppi_cfg_size          \
                                )                            \
                                                             \
                                {                            \
                                   ds_3gppi_cfg_item,        \
                                   ds_3gppi_cfg_description, \
                                   ds_3gppi_cfg_path,        \
                                   ds_3gppi_cfg_size         \
                                },

static const ds_3gppi_cfg_item_info_t ds_3gppi_cfg_item_info[] = {
#ifdef FEATURE_DATA_LTE
    DS_3GPPI_CFG_TABLE_LTE
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_TEST_LOOPBACK
    DS_3GPPI_CFG_TABLE_TEST_LOOPBACK
#endif /* FEATURE_DATA_TEST_LOOPBACK */
    DS_3GPPI_CFG_TABLE
};

/*---------------------------------------------------------------------------
  Accessor macros to access config item from config item array
---------------------------------------------------------------------------*/
#define DS_3GPPI_CFG_ITEM_INFO(item) (&ds_3gppi_cfg_item_info[(int)item])

/*---------------------------------------------------------------------------
  Constant defining the max allowed path name size for config items. The 
  max path defined above including prefix should never exceed this
---------------------------------------------------------------------------*/
#define DS_3GPPI_CFG_MAX_PATH_SIZE   128

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Array to hold mapping of sys_sys_mode_e_type to ds_3gppi_sys_mode_e_type
---------------------------------------------------------------------------*/
int ds_3gppi_mode_index[SYS_SYS_MODE_MAX] = {0};


/*===========================================================================
                   CONFIGURATION UTILITIES
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro to validate configuration item identifier
---------------------------------------------------------------------------*/
#define DS_3GPPI_VERIFY_CFG_ITEM(item) \
          (((int)item < DS_3GPP_CFG_MAX) ? TRUE : FALSE )


/*===========================================================================
FUNCTION ds_3gpp_is_rel_11_supported

DESCRIPTION
  Determine if UE is release 11 ready or not

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE 
   TRUE - if device is release 11 ready
   FALSE - Otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_3gppi_is_rel_11_supported
(
   void
)
{
#ifdef FEATURE_DATA_REL11
  if( ds_3gpp_nv_manager_get_nv_umts_rel_version() >= NV_NAS_COMPLIANCE_REL11 
#ifdef FEATURE_DATA_LTE
    || ds_3gpp_nv_manager_get_nv_lte_rel_version() >= LTE_3GPP_REL11
#endif /* FEATURE_DATA_LTE */
  )
  {
     return TRUE;
  }
#endif

     return FALSE;
}
/*===========================================================================
FUNCTION DS_3GPPI_CFG_GET_ITEM_INFO

DESCRIPTION
  Returns pointer to configuration item info struct.
  
PARAMETERS  
  item - configuration item identifier
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Pointer to configuration item info, if it exists, NULL otherwise
   
SIDE EFFECTS 
  None.
===========================================================================*/
static const ds_3gppi_cfg_item_info_t * 
ds_3gppi_cfg_get_item_info(ds_3gpp_cfg_item_e item)
{
    const ds_3gppi_cfg_item_info_t * info_p = NULL;

    if (DS_3GPPI_VERIFY_CFG_ITEM(item)) {
        info_p = DS_3GPPI_CFG_ITEM_INFO(item);
    }

    return info_p;
}

/*===========================================================================
FUNCTION DS_3GPPI_CFG_GET_ITEM_PATH

DESCRIPTION
  Returns the full path in EFS where the configuration item is stored.
  
PARAMETERS  
  info_p - pointer to configuration item info struct
  path   - pointer to buffer to return the path name
  len    - size of the passed buffer in bytes
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean - TRUE if item was set successfully, FALSE otherwise.
   
SIDE EFFECTS 
  None.
===========================================================================*/
static boolean ds_3gppi_cfg_get_item_path
(
    const ds_3gppi_cfg_item_info_t * info_p, 
    char                           * path, 
    size_t                           len
)
{
    int result = -1;
    ASSERT(info_p);
    ASSERT(path);
    /* truncation is taken care in strlcat
       If strlcpy fails return False */
    if( strlcpy(path, ds_3gppi_cfg_path_prefix, len) <= 0 ) 
    {
      return FALSE;
    }
    result  = strlcat(path, DS_3GPPI_CFG_ITEM_PATH(info_p), len);
    if( (result >= (int)len) || (result <= 0) )
    {
      return FALSE;
    }

    return TRUE;
}

/*===========================================================================
FUNCTION DS_3GPPI_CFG_VALIDATE

DESCRIPTION
  Checks if configuration item is valid and parameters specified are correct.
  
PARAMETERS  
  item - configuration item to set/get
  val  - location of bytes to read from or write to
  len  - size of the configuration item in bytes
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  ptr to cfg item info, if item and parameters are valid, NULL otherwise.
   
SIDE EFFECTS 
  None.
===========================================================================*/
static const ds_3gppi_cfg_item_info_t * ds_3gppi_cfg_validate
(
    ds_3gpp_cfg_item_e  item, 
    unsigned char     * val, 
    size_t              len
)
{ 
    const ds_3gppi_cfg_item_info_t * info_p = NULL;

    if (!val) 
    {
      DS_3GPP_MSG0_ERROR("val ptr is NULL");
        goto err;
    }

    info_p = ds_3gppi_cfg_get_item_info(item);
    if (!info_p)
    {
      DS_3GPP_MSG1_ERROR("Failed to get item info, item %d", (int)item);
    }

err:
    return info_p;
}

/*===========================================================================
FUNCTION DS_3GPP_CFG_SET

DESCRIPTION
  Sets configuration item in non-volatile store (EFS).
  
PARAMETERS  
  item - configuration item to set
  val  - location of bytes to write to store
  len  - size of the configuration item in bytes
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean - TRUE if item was set successfully, FALSE otherwise.
   
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_3gpp_cfg_set
(
    ds_3gpp_cfg_item_e  item, 
    unsigned char     * val, 
    size_t              len
)
{
  mcfg_fs_status_e_type rval = MCFG_FS_STATUS_EFS_ERR;
    const ds_3gppi_cfg_item_info_t * info_p;
    char path[DS_3GPPI_CFG_MAX_PATH_SIZE];

    info_p = ds_3gppi_cfg_validate(item, val, len);
    if (!info_p)
    {
        goto err;
    }

    if(len > DS_3GPPI_CFG_ITEM_SIZE(info_p))
    {
       goto err;
    }

    DATA_3GPP_MSG_SPRINTF_1
    (
        MSG_LEGACY_HIGH, 
        "Setting cfg item %s",
        DS_3GPPI_CFG_ITEM_DESC(info_p)
    );

    if(!ds_3gppi_cfg_get_item_path(info_p, path, sizeof(path)))
    {
      DS_3GPP_MSG0_HIGH("error in fun ds_3gpp_cfg_set");
      goto err;
    }

  rval = mcfg_fs_write(path, val, len,
                       MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                       MCFG_FS_DEFFILEMODE,
                       MCFG_FS_TYPE_EFS,
                       MCFG_FS_SUBID_NONE);

err:
  return ((rval != MCFG_FS_STATUS_OK) ? FALSE : TRUE);
}

/*===========================================================================
FUNCTION DS_3GPP_CFG_GET

DESCRIPTION
  Gets configuration item from non-volatile store (EFS).
  
PARAMETERS  
  item - configuration item to get
  val  - location where value read should be written
  len  - size of the configuration item in bytes
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  int - number of bytes read successfully from the config item file
        (-1, on error).
   
SIDE EFFECTS 
  None.
===========================================================================*/
int ds_3gpp_cfg_get
(
    ds_3gpp_cfg_item_e  item, 
    unsigned char     * val, 
    size_t              len
)
{
  mcfg_fs_status_e_type rval = MCFG_FS_STATUS_EFS_ERR;
    const ds_3gppi_cfg_item_info_t * info_p;
    char path[DS_3GPPI_CFG_MAX_PATH_SIZE];

    info_p = ds_3gppi_cfg_validate(item, val, len);
    if (!info_p)
    {
        goto err;
    }

    if(len < DS_3GPPI_CFG_ITEM_SIZE(info_p))
    {   
       goto err;
    }
   
    if(!ds_3gppi_cfg_get_item_path(info_p, path, sizeof(path)))
    {
    DS_3GPP_MSG0_LOW("err in ds_3gpp_cfg_get");
      goto err;
    }

  rval = mcfg_fs_read(path, val, len, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE);

err:
  return (rval == MCFG_FS_STATUS_OK)?len:-1;
}

int ds_3gpp_cfg_get_subsid
(
    ds_3gpp_cfg_item_e  item, 
    unsigned char     * val, 
    size_t              len,
    sys_modem_as_id_e_type subs_id
)
{
    int rval = -1;
    const ds_3gppi_cfg_item_info_t * info_p;
    char path[DS_3GPPI_CFG_MAX_PATH_SIZE];

    info_p = ds_3gppi_cfg_validate(item, val, len);
    if (!info_p)
    {
        goto err;
    }

  if(len < DS_3GPPI_CFG_ITEM_SIZE(info_p))
  {   
     goto err;
  }
 
    if(!ds_3gppi_cfg_get_item_path(info_p, path, sizeof(path)))
    {
    DS_3GPP_MSG0_LOW("err in ds_3gpp_cfg_get");
      goto err;
    }

    if (ds3gcfgmgr_read_efs_nv_ex(path, val, len,subs_id) == NV_DONE_S)
    {
      rval = 1;
    }

err:
    return rval;
}

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPPI_UTILS_READ_LTE_ATTACH_DUAL_IP_FB_IP_TYPE

DESCRIPTION
  This NV will be read soon after Init. The IP type read will be used,
  to redial attach request after the initial request fails with ESM cause
  #28 (Unknown PDN Type).
  
  If the NV value is not set, currently, the IP type defaults to IPV4

PARAMETERS
  None
.
DEPENDENCIES
  None.

RETURN VALUE
  None  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gppi_utils_read_lte_attach_dual_ip_fb_ip_type
(
  sys_modem_as_id_e_type  subs_id
)
{
  int                                   ret_val = -1;
  ds_eps_attach_fb_info_type            attach_fb_info;



  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;  
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&attach_fb_info, 0, sizeof(ds_eps_attach_fb_info_type));
  /*---------------------------------------------------------------------- 
    Read from NV the Dual IP fall back type for attach PDN. If the EFS
    item is read from the NV ds_3gpp_cfg_get() returns the number of bytes
    read from the EFS
    ----------------------------------------------------------------------*/
  ret_val = ds_3gpp_cfg_get_subsid(DS_3GPP_DUAL_IP_FALLBACK_ENABLED, 
                      (uint8 *)&(attach_fb_info), 
                       sizeof(ds_eps_attach_fb_info_type),
                       subs_id);

  if ((ret_val <=0)|| (attach_fb_info.fallback_enabled == FALSE))
  {
    /*----------------------------------------------------------------------- 
      Either the NV is not set (or) NV read operation failed. for
      both these cases, disable the feature
      -----------------------------------------------------------------------*/
    DS_3GPP_MSG0_HIGH("LTE Attach Dual IP fall back not set. Disabling feature");
    ds_eps_set_lte_attach_dual_ip_fb_ip_type(FALSE, 
                                             DS_EPS_ATTACH_FB_IP_TYPE_MIN,subs_id);     
  }
  else
  {
    /*-----------------------------------------------------------------------
      NV read successful
      -----------------------------------------------------------------------*/
    DS_3GPP_MSG1_HIGH("LTE Attach Dual IP fall back NV set to %d",
                      attach_fb_info.fallback_ip_type);
    ds_eps_set_lte_attach_dual_ip_fb_ip_type(TRUE, 
                                             attach_fb_info.fallback_ip_type,subs_id);
  }
  return;
}
#endif
/*===========================================================================
FUNCTION DS_3GPPI_UTILS_FILL_APN_DISCONN_ORDER_LIST

DESCRIPTION
  Fills the 'APN disconnect order' list with the 'APN identifiers' read
  from the EFS.
 
  At present 'APN identifier' supported is 'APN Name'. It can be expanded
  to any APN parameter if reqd

PARAMETERS
  identifier_type   - type of Profile identifier that is updated
  from              - Pointer to the start of the 'APN identifier'
  to                - Pointer to the end of the 'APN identifier'

DEPENDENCIES
  None.

RETURN VALUE
  1     -   If the 'APN identifier' is cached successfully in the
            'APN disconnect order' list
  0     -   If the 'APN identifier' is already present in the list
              (or)
            If 'APN identifier' cannot be cached due to other failures
 -1     -   If the list is full

SIDE EFFECTS
  None.

===========================================================================*/

int ds_3gppi_utils_fill_apn_disconn_order_list
(
  ds_profile_3gpp_param_enum_type identifier_type,
  char* from,
  char* to,
  sys_modem_as_id_e_type subs_id
)
{
  uint8                                str_len                  = 0;
  int                                  ret_val                  = 0;
  int                                  idx                      = 0;
  ds_3gpp_apn_disconn_order_list_type *apn_disconn_order_list_p = NULL;
  int                                  free_index               = -1;
  uint8                                num_bytes                = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Perform NULL check of 'from' and 'to locations
    -------------------------------------------------------------------------*/
  if ((from == NULL) || (to == NULL) || (from > to))
  {
    DS_3GPP_MSG2_HIGH("apn disconnect order list:Store APN identifier failed."
                      "Invalid Reference from:0x%x to:0x%x",from,to);
    return ret_val;
  }
  /*------------------------------------------------------------------------- 
    Check if the identifier type is supported
    -------------------------------------------------------------------------*/
  if ((identifier_type <= DS_PROFILE_3GPP_PROFILE_PARAM_INVALID)||
      (identifier_type >DS_PROFILE_3GPP_PROFILE_PARAM_MAX))
  {
    DS_3GPP_MSG1_HIGH("apn disconnect order list:Store APN identifier failed."
                      "Invalid identifier type %d",identifier_type);
    return ret_val;

  }
  /*------------------------------------------------------------------------- 
    Currently we support only 'DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN'
    identifier type. This can however be expanded to any 'profile_parm'
    -------------------------------------------------------------------------*/
  if (identifier_type == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN)
  {
    /*----------------------- 
      Calculate String length
      -----------------------*/
    str_len = (uint8)(to-from);
    str_len = (str_len < DS_UMTS_MAX_APN_STRING_LEN)?
               str_len:DS_UMTS_MAX_APN_STRING_LEN;
    DS_3GPP_MSG2_MED("apn disconnect order list:Identifier type:%d,"
                      "Identifier length:%d",identifier_type,str_len);
  }
             
  /*------------------------------------------------------------ 
    Check if the APN identifier is already in the apn disconnect
    order list
    ------------------------------------------------------------*/
  for (idx =0; idx<DS_3GPP_MAX_APN_IDENTIFIERS;idx++)
  {
    /*-----------------------------------------------------------------------
      'ds_3gpp_apn_disconn_order_list[idx]' is a void pointer which will be
      type cased at the time of usage to 'ds_3gpp_apn_disconn_order_list_type'
      and 'identifer' is pointer within 'ds_3gpp_apn_disconn_order_list_type'
      to hold the APN identifiers.
      For a particular 'idx'. at the time of requesting memory,we request
      for the entire memory reqd (i.e)
      =====================================================================
      sizeof('ds_3gpp_apn_disconn_order_list_type')+num_bytes reqd to store
      the APN identifier.
      =====================================================================
      So here it should be sufficient perform a NULL check only for
      ds_3gpp_apn_disconn_order_list[idx] and the NULL check for 'APN
      identifier' may not be required
      -----------------------------------------------------------------------*/
    if (ds_3gpp_apn_disconn_order_list[subs_id][idx] == NULL)
    {
      free_index = idx;
      break;
    }
    /*-----------------------------------------------------------------------
      Check if there is a match. While looking for a match do not account for
      '\0' character, since 'to' variable read from the EFS doesnt have '\0'
      terminal character.
      ------------------------------------------------------------------------*/
    apn_disconn_order_list_p = (ds_3gpp_apn_disconn_order_list_type*)
                                    ds_3gpp_apn_disconn_order_list[subs_id][idx];
    if (strncasecmp((char*)apn_disconn_order_list_p->identifier,
                    (char*)from,str_len)==0)
    {
      DS_3GPP_MSG1_MED("Matching Identifier found at index %d",idx);
      break;
    }
  }
  /*----------------------------------------------- 
    Return if APN identifier is already present
    -----------------------------------------------*/
  if ((idx<DS_3GPP_MAX_APN_IDENTIFIERS) &&
      (idx!= free_index))
  {

    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"apn disconnect order list: APN "
                           "identifier '%s' already present in the list",
                            apn_disconn_order_list_p->identifier);
    return ret_val;
  }

  /*----------------------------------------------------
    Return if APN disconnect order list is already full
    ----------------------------------------------------*/
  if ((free_index < 0)                          ||
      (free_index >= DS_3GPP_MAX_APN_IDENTIFIERS))
  {
    ret_val = -1;
    return ret_val;
  }

 /*--------------------------------------------------------------------------
    Depending on the profile parameter type, calculate total number of
    bytes required to store information (i.e) for both
    'ds_3gpp_apn_disconn_order_list_type' and the 'APN identifier' within
    'ds_3gpp_apn_disconn_order_list_type'.
   -------------------------------------------------------------------------*/
  if (identifier_type == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN)
  {
    num_bytes = (sizeof(ds_3gpp_apn_disconn_order_list_type) +
                 (sizeof(byte)*(str_len+1)));
  }
  /*------------------------------------------------------------------------- 
    Allocate memory to cache the APN identifier
    -------------------------------------------------------------------------*/
  ds_3gpp_apn_disconn_order_list[subs_id][free_index] = (void*)
                      modem_mem_alloc(num_bytes,MODEM_MEM_CLIENT_DATA);
  /*------------------------------------------------------------------------- 
    Update the identifier if the memory allocation is successful
    Return if mem alloc fails
    -------------------------------------------------------------------------*/
  if (ds_3gpp_apn_disconn_order_list[subs_id][free_index] == NULL)
  {
    DS_3GPP_MSG0_HIGH("apn disconnect order list: Mem alloc failed");
    return ret_val;
  }
  memset(ds_3gpp_apn_disconn_order_list[subs_id][free_index],0,num_bytes);
  /*------------------------------------------------------------------------- 
    Locate the memory reserved for 'apn identifier'
    -------------------------------------------------------------------------*/
  apn_disconn_order_list_p        =(ds_3gpp_apn_disconn_order_list_type*)
                                    ds_3gpp_apn_disconn_order_list[subs_id][free_index];
  

  if ((apn_disconn_order_list_p + 1) == NULL)
  {
    /*-----------------------------------------------------------------------
      Free the entire chunk allocated for
      ds_3gpp_apn_disconn_order_list[free_index]
      --------------------------------f---------------------------------------*/
    modem_mem_free((void*)apn_disconn_order_list_p,MODEM_MEM_CLIENT_DATA);
    DS_3GPP_MSG0_HIGH("apn disconnect order list: Mem alloc failed");
    return ret_val;
  }
  apn_disconn_order_list_p->identifier = (apn_disconn_order_list_p + 1);
  /*----------------------------------------------------------------------- 
    Fill in what type of profile is being read from EFS. Currently
    we support having only APN name as identifier type.
    -----------------------------------------------------------------------*/
  apn_disconn_order_list_p->profile_parm = identifier_type;
  

  /*------------------------------------------------------------------------- 
    Copy the APN identifier into the list depending on the type of
    identifier
    -------------------------------------------------------------------------*/
  if (identifier_type == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN)
  {
    strlcpy(apn_disconn_order_list_p->identifier,from,str_len+1);
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Adding APN ident:'%s'to apn"
                          " disconnect order list",
                          (char*)apn_disconn_order_list_p->identifier);
  }
  ret_val = TRUE;
  
  return ret_val;                                            
}
/*==========================================================================
FUNCTION DS_3GPPI_UTILS_READ_APN_DISCONN_ORDER_LIST_FRM_EFS

DESCRIPTION
  Reads the APN disconnect order list from the EFS.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gppi_utils_read_apn_disconn_order_list_frm_efs(sys_modem_as_id_e_type subs_id)
{
  /*--------------------------------------------------------------------------- 
    File name where the APN disconnect order list for tearing down
    PDN's are stored. This will be used during a prioritized PDN bringup
    if necessary
  ---------------------------------------------------------------------------*/
  char              file_name[] = DS_3GPPI_UTILS_APN_DISCONN_ORDER_LIST_FILE;
  ds3gcfgmgr_efs_token_type efs_db;  /* structure used for tokenizing the file*/
  char             *from    = NULL;    /* pointer to the End of the token*/
  char             *to      = NULL;      /* pointer to the start of the token */
  ds3gcfgmgr_efs_token_parse_status_type   ret_val = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  uint8 idx=0;
  /*--------------------------------------------------------------------------- 
    Identifier type that is read from the apn_disconnect_order list
  ---------------------------------------------------------------------------*/
  ds_profile_3gpp_param_enum_type       apn_identifier_type = 
                                DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_MED("Read APN disconn config from EFS");

  
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;  
  }
  /*--------------------------------------------------------------------------- 
    Open the file and Initialize the EFS State machine
  ---------------------------------------------------------------------------*/
  if(ds3gcfgmgr_efs_file_init(file_name, &efs_db) == -1)
  {
    DS_3GPP_MSG0_HIGH("apn disconnect order list: Error reading list from EFS");
    return;
  }
  /*----------------------------------------------------
    Set the seperator as ';'               
  ----------------------------------------------------*/
  efs_db.seperator = ';';
  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF 
          != (ret_val = ds3gcfgmgr_efs_tokenizer(&efs_db, &from, &to )))
  {
    /*------------------------------------------------------------------------- 
      APN identifier being read. 'from' points to the beginning of the
      identifier and 'to' points to the end of the identifier.

      ds_efs_tokenizer() automatically skips blank lines and comments (lines 
      beginning with #, so no need to check for them here).
    --------------------------------------------------------------------------*/
    if((from == to)|| (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*-------------------------------
        Skip empty tokens.
      -------------------------------*/
      DS_3GPP_MSG1_LOW("Reached End of Line in file 0x%x",&efs_db);
      continue;
    } 
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      /*------------------------------------------------- 
        Received an APN identifer from the EFS file
        Check if we can cache this APN identifier
      --------------------------------------------------*/
      if (idx >= DS_3GPP_MAX_APN_IDENTIFIERS)
      {
        DS_3GPP_MSG1_HIGH("apn disconnect order list: Cannot accept more"
                          "than %d entries",DS_3GPP_MAX_APN_IDENTIFIERS);
        break;
      }
      /*------------------------------------------------------------------- 
        Validate the 'contents' read from the EFS
                    &
        Update the PDN disconnect order list with  these 'contents'
      -------------------------------------------------------------------*/
      if (ds_3gppi_utils_fill_apn_disconn_order_list(apn_identifier_type,
                                                          from,
                                                          to,
                                                     subs_id) < 0)
      {
        DS_3GPP_MSG1_HIGH("apn disconnect order list: Cannot accept more"
                          "than %d entries",DS_3GPP_MAX_APN_IDENTIFIERS);
        break;
      }
    }
  }
  ds3gcfgmgr_efs_file_close(&efs_db);
  return;
}
/*===========================================================================
FUNCTION DS_3GPPI_UTILS_IS_APN_DISCONN_ORDER_LIST_EMPTY

DESCRIPTION
  Checks if the APN disconnect order list is empty 
  The list will be used while applying Bearer mgmt algorithm to allocate
  bearers on priority for upcoming PDN.

PARAMETERS
  None             

DEPENDENCIES
  The list is empty, if there is no EFS configuration

RETURN VALUE
  TRUE     - If the apn disconnect priority list is empty
  FALSE    - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gppi_utils_is_apn_disconn_order_list_empty(sys_modem_as_id_e_type subs_id)
{
  boolean   ret_val = TRUE;
  uint8     index     = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;  
  }

  /*------------------------------------------------------------------------- 
    Loop through the 'APN disconnect order' list. If there is atleast one
    entry found, the list is not empty
    -------------------------------------------------------------------------*/
  for (index = 0; index < DS_3GPP_MAX_APN_IDENTIFIERS; index++) 
  {
    if (ds_3gpp_apn_disconn_order_list[subs_id][index]!= NULL)
    {
      ret_val = FALSE;
      break;
    }
  }
  return ret_val;
}
void ds_3gppi_utils_clear_apn_disconn_order_list_per_subsid
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8     index     = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;  
  }

  /*------------------------------------------------------------------------- 
    Loop through the 'APN disconnect order' list. If there is atleast one
    entry found, the list is not empty
    -------------------------------------------------------------------------*/
  for (index = 0; index < DS_3GPP_MAX_APN_IDENTIFIERS; index++) 
  {
    if (ds_3gpp_apn_disconn_order_list[subs_id][index]!= NULL)
    {
      modem_mem_free(ds_3gpp_apn_disconn_order_list[subs_id][index],MODEM_MEM_CLIENT_DATA);
      ds_3gpp_apn_disconn_order_list[subs_id][index] = NULL;
    }
  }
}

/*===========================================================================
FUNCTION      DS_3GPPI_UTILS_FILL_EXTRACTED_INFO

DESCRIPTION   This function fills the EFS extracted info into the queue passed 
              by the client calling function 

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)
              extracted_info_q_ptr - Queue that will contain extracted EFS info
              subs_id - Current active PS subscription

DEPENDENCIES  None

RETURN VALUE  0  - success
              -1 - failure

SIDE EFFECTS  None
===========================================================================*/
int16 ds_3gppi_utils_fill_extracted_info
(
  char         *from,
  char         *to,
  q_type       *extracted_info_q_ptr,
  sys_modem_as_id_e_type    subs_id
)
{
  int16 ret_val = -1;
  uint8 length = 0;
  ds_3gppi_utils_extracted_info_type      *extracted_info_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == from ||
     NULL == to ||
     NULL == extracted_info_q_ptr ||
     from > to)
  {
    DATA_3GMGR_MSG3(MSG_LEGACY_ERROR, 
                    "Invalid Parameters from: 0x%x to: 0x%x Queue ptr: 0x%x",
                    from, to, extracted_info_q_ptr);
    return ret_val;
  }

  length = (uint8)(to - from);

  /*----------------------------------------------------------------------- 
    This memory will be freed by the calling client function who calls the util
    to extract EFS info
    -----------------------------------------------------------------------*/
  extracted_info_block_ptr = (ds_3gppi_utils_extracted_info_type *)modem_mem_alloc
        (sizeof(ds_3gppi_utils_extracted_info_type), MODEM_MEM_CLIENT_DATA);

  if(NULL != extracted_info_block_ptr)
  {
    memset(extracted_info_block_ptr, 0, sizeof(ds_3gppi_utils_extracted_info_type));
    extracted_info_block_ptr->extracted_data = 
      (char *)modem_mem_alloc(sizeof(char)*(length+1),MODEM_MEM_CLIENT_DATA);
    if(NULL != extracted_info_block_ptr->extracted_data)
    {
      memset(extracted_info_block_ptr->extracted_data, 0, length+1);
      strlcpy((char *)extracted_info_block_ptr->extracted_data, from, length+1);

      extracted_info_block_ptr->extracted_data_length = 
                            strlen(extracted_info_block_ptr->extracted_data);

      q_link(extracted_info_block_ptr, &(extracted_info_block_ptr->link));
      q_put(extracted_info_q_ptr, &(extracted_info_block_ptr->link));
      ret_val = 0;
    }
    else
    {
      modem_mem_free(extracted_info_block_ptr, MODEM_MEM_CLIENT_DATA);
      extracted_info_block_ptr = NULL;
    }
  }
  
  if(-1 == ret_val)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Memeory allocation failure !!!!");
  }

  return ret_val;

}

/*===========================================================================
FUNCTION      DS_3GPPI_UTILS_VERIFY_APN_TOKEN_ID

DESCRIPTION   This function verifies the token in the EFS file with the passed 
              comparison string 

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)
              char *comp_string - string against which token id needs to be
                                  compared

DEPENDENCIES  None

RETURN VALUE  0  - success
              -1 - failure

SIDE EFFECTS  None
===========================================================================*/
int16 ds_3gppi_utils_verify_apn_token_id
(
  char *from,
  char *to,
  char *comp_string
)
{
  int16 ret_val = -1; /* return value */
  uint8 length = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == from ||
     NULL == to ||
     NULL == comp_string ||
     from > to)
  {
    DATA_3GMGR_MSG3(MSG_LEGACY_ERROR, 
                    "Invalid Input Params from:0x%x to:0x%x compare string:0x%x",
                    from, to, comp_string);
    return ret_val;
  }

  /* Assumption that the token length will not be greater than 255 */
  length =(uint8)(to-from);

  if(strncasecmp(from,comp_string,length) != 0)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Invalid Token ID");
    return ret_val;
  }
  
  ret_val = 0;
  return ret_val;

} /* ds_apn_reject_verify_token_id() */

/*===========================================================================
FUNCTION      DS_3GPPI_UTILS_READ_APN_INFO_FROM_EFS

DESCRIPTION   This function will read from the efs file to see if the 
              reject/forbidden apn has been set 

              EFS File Format - Param_Name:Param_Val;
              For example     - reject_apn:test.apn;             

DEPENDENCIES  None

RETURN VALUE  TRUE - If parsing is successful 
              FALSE - Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_3gppi_utils_read_apn_info_from_efs 
(  
  char                        *comparison_tag,
  char                        *efs_file_name,
  q_type                      *efs_extracted_info_q,
  sys_modem_as_id_e_type       subs_id
)
{

  ds3gcfgmgr_efs_token_type  efs_db; /* structure used for tokenizing the file*/
  char *from, *to; /* ptrs to start and end of the token */
  ds3gcfgmgr_efs_token_parse_status_type  ret_val 
                                              = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  boolean param_name = TRUE; /* flag to keep track of the parser state 
                                TRUE - parser expects Token Name in the EFS
                                FALSE - parser expects Token Val in the EFS*/
  boolean return_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NULL == comparison_tag ||
     NULL == efs_file_name)
  {
    DATA_3GMGR_MSG3(MSG_LEGACY_ERROR, 
                    "Invalid Input Params, File Name: 0x%x "
                    "Compare String: 0x%x EFS queue 0x%x", 
                    efs_file_name, comparison_tag, efs_extracted_info_q);
    return ret_val;
  }

  if(ds3gcfgmgr_efs_file_init(efs_file_name, &efs_db) == -1)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_HIGH, "Failure reading PDN throttling cnfg");
    return return_val;
  }

  /* Set the seperator as : */
  efs_db.seperator = ':';
/*lint -save -esym(644,token_id) param_name boolean flag takes care of this */
  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF 
          != (ret_val = ds3gcfgmgr_efs_tokenizer(&efs_db, &from, &to)))
  {
    /*------------------------------------------------------------------------
      Token being read. 'from' points to the beginning of the token and 
      'to' point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines 
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }     
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      /*--------------------------------------------------------------------- 
      check if we are looking for param name or param value 
      ---------------------------------------------------------------------*/
      if (param_name == TRUE)
      {
        /*------------------------------------------------------------------
        get the token identifier for this param name 
        ------------------------------------------------------------------*/
        if (ds_3gppi_utils_verify_apn_token_id(from, to, 
                                               comparison_tag) < 0)
        {
          /* This is an error scenario, Skip till the end of the line? */
          DATA_3GMGR_MSG0( MSG_LEGACY_ERROR, "Incorrect Param Name" );
          return return_val;
        }
        else
        {
          /*-----------------------------------------------------------------
          set param_name as FALSE This means the next token is a 
          param value
          -----------------------------------------------------------------*/
          param_name = FALSE;
          /* set the seperator as ; */
          efs_db.seperator = ';';
        }
        
      }
      /*---------------------------------------------------------------------
      This means that the token is a param value
      ---------------------------------------------------------------------*/
      else 
      {
        /*-------------------------------------------------------------------
         pass in the identifier and param value to fill the APN reject SM
         config structure
         ------------------------------------------------------------------*/
        if(ds_3gppi_utils_fill_extracted_info(from, to, efs_extracted_info_q,
                                              subs_id) < 0)
        {
          DATA_3GMGR_MSG0( MSG_LEGACY_ERROR, "Failure filling token contents");
          return return_val;
        }
        /*-------------------------------------------------------------------
         set param_name as TRUE This means that next token is a param name
        -------------------------------------------------------------------*/
        param_name = TRUE;
        /* Set the seperator as : */
        efs_db.seperator = ':';
      }
    }
  } /* end of while loop */

  return_val = TRUE;
/*lint -restore (644,token_id)*/
  ds3gcfgmgr_efs_file_close(&efs_db);
  return return_val;

} /* ds_read_apn_reject_from_efs() */

/*===========================================================================
FUNCTION DS_3GPPI_UTILS_GET_APN_DISCONN_ORDER_LIST_INFO

DESCRIPTION
  Given the Index, this utility function provides the APN disconnect
  information available at given index
PARAMETERS
  None             

DEPENDENCIES
  The list is empty, if there is no EFS configuration

RETURN VALUE
  void*     - Returns a pointer to the memory location at queried index
  NULL      - If the index is invalid

SIDE EFFECTS
  None.

===========================================================================*/
void* ds_3gppi_utils_get_apn_disconn_order_list_info
(  
  uint8 index,
  sys_modem_as_id_e_type subs_id
)
{
  return ((index < DS_3GPP_MAX_APN_IDENTIFIERS)?
            ds_3gpp_apn_disconn_order_list[subs_id][index]:NULL);
}

/*===========================================================================
FUNCTION DS_3GPPI_UTILS_READ_ITEMS_FROM_EFS_PER_SUBS_ID

DESCRIPTION 
  This function reads the EFS items  per subsid
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.
===========================================================================*/

void ds_3gppi_utils_read_items_from_efs_per_subs_id(sys_modem_as_id_e_type  subs_id)
{
  ds_3gppi_utils_read_apn_disconn_order_list_frm_efs(subs_id);
#ifdef FEATURE_DATA_LTE 
  ds_3gppi_utils_read_lte_attach_dual_ip_fb_ip_type(subs_id);
#endif
}


/*===========================================================================
FUNCTION DS_3GPPI_UTILS_READ_ITEMS_FROM_EFS

DESCRIPTION 
  This function reads NV items from EFS at Init 
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gppi_utils_read_items_from_efs(void)
{
  sys_modem_as_id_e_type  subs_id = ds3g_get_ps_subs_id();
  ds_3gppi_utils_read_items_from_efs_per_subs_id(subs_id);

}

/*===========================================================================
FUNCTION DS_3GPPI_UTILS_READ_ITEMS_FROM_EFS_AFTER_INIT

DESCRIPTION 
  This function posts a command to DS task to read items from EFS after 
  device Init is successfully done. However the command is posted
  during device Init
 
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gppi_utils_init_after_powerup(void)
{  
  ds_cmd_type *cmd_ptr = NULL;          /* DS Task msg */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO) ) == NULL )
  {
    ASSERT(0);
    return;
  }                                 

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_INIT_AFTER_POWERUP;
  ds_put_cmd(cmd_ptr);
}
/*===========================================================================
FUNCTION DS_3GPPI_UTILS_INIT

DESCRIPTION
  Initializes utilities. Must be done at init time once when task is started.
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gppi_utils_init(void)
{
  int  i;
  ds3gsubsmgr_subs_id_e_type subs_indx =  DS3GSUBSMGR_SUBS_ID_INVALID;
/*----------------------------------------------------------------------*/

  for (i = 0; i < (int)SYS_SYS_MODE_MAX; ++i) {
    switch((sys_sys_mode_e_type)i) {
#ifdef FEATURE_DATA_LTE
    case SYS_SYS_MODE_LTE:
      ds_3gppi_mode_index[i] = (int)DS_3GPPI_SYS_MODE_LTE;
      break;
#endif
#ifdef FEATURE_WCDMA
    case SYS_SYS_MODE_WCDMA:
      ds_3gppi_mode_index[i] = (int)DS_3GPPI_SYS_MODE_WCDMA;
      break;
#endif
#ifdef FEATURE_GSM_GPRS
    case SYS_SYS_MODE_GSM:
      ds_3gppi_mode_index[i] = (int)DS_3GPPI_SYS_MODE_GSM;
      break;
#endif
#ifdef FEATURE_TDSCDMA
    case SYS_SYS_MODE_TDS:
      ds_3gppi_mode_index[i] = (int)DS_3GPPI_SYS_MODE_TDS;
      break;
#endif
    default:
      ds_3gppi_mode_index[i] = (int)DS_3GPPI_SYS_MODE_ILLEGAL;
      break;
    }
  }

  /*------------------------------------------------------------------------- 
    Initialize the APN disconnect order list to NULL
    -------------------------------------------------------------------------*/
  for( subs_indx =  DS3GSUBSMGR_SUBS_ID_MIN;
       subs_indx < DS3GSUBSMGR_SUBS_ID_MAX ;
       subs_indx++ )
  {
    for (i = 0; i < DS_3GPP_MAX_APN_IDENTIFIERS; i++) 
    {
      ds_3gpp_apn_disconn_order_list[subs_indx][i]= NULL;
    }
  }

  ds_3gppi_utils_init_after_powerup();


}

/*===========================================================================
  FUNCTION DS_3GPP_GET_DATA_CONF_BUF_SIZE

  DESCRIPTION
    This function returns the buffer size to allocate for storing 
    3GPP EFS Item File paths.

  PARAMETERS
    None.

  DEPENDENCIES 
    None.

  RETURN VALUE 
    Buffer Size (uint32)

  SIDE EFFECTS 
    None.

===========================================================================*/
uint32 ds_3gpp_get_data_conf_buf_size
(
  void
)
{
  return DS_3GPPI_CFG_MAX_PATH_SIZE * DS_3GPP_CFG_MAX;
}

/*===========================================================================
  FUNCTION DS_3GPP_UPDATE_DATA_CONFIG_INFO

  DESCRIPTION
    Puts all the required nv item file paths in the data_config_info.conf 
    file.
    Would be called from dsutils during powerup.

  PARAMETERS  
    file_paths_buf:     buffer for file path.
    file_paths_buf_len: buffer size
      
  DEPENDENCIES 
    None.

  RETURN VALUE 
    (int32)  length of string copied into file_paths_buf.
    -1 for failure.

  SIDE EFFECTS 
    None.

===========================================================================*/
int32 ds_3gpp_update_data_config_info
(
  char   *file_paths_buf,
  uint32  file_paths_buf_len
)
{
  int32 i;
  const ds_3gppi_cfg_item_info_t *info_p;
  char nv_item_file_path[DS_3GPPI_CFG_MAX_PATH_SIZE];
  uint32 position = 0;
/*-------------------------------------------------------------------------*/

  if( file_paths_buf == NULL )
  {
    DS_3GPP_MSG0_ERROR("Failed to allocate memory");
    return -1;
  }

  /* Initialize buffers */
  memset(nv_item_file_path, 0, DS_3GPPI_CFG_MAX_PATH_SIZE);
  file_paths_buf[0] = '\0';

  /*-------------------------------------------------------------------------
    Update data_config_info.conf with the list of nv item files.
  -------------------------------------------------------------------------*/
  /* Browse through ds_3gppi_cfg_item_info and copy all items into a
     buffer passed by dsutil.  */
  for (i = 0; i < DS_3GPP_CFG_MAX; i ++)
  {
    info_p = ds_3gppi_cfg_get_item_info((ds_3gpp_cfg_item_e)i);
    if (!info_p)
    {
      DS_3GPP_MSG1_ERROR(
        "ds_3gppi_utils_init: Failed to get item info, item %d", i);
      continue;
    }

    if(ds_3gppi_cfg_get_item_path(
         info_p, nv_item_file_path, DS_3GPPI_CFG_MAX_PATH_SIZE) == TRUE)
    {
      /* Concatenate current path in buffer using snprintf and append \n */
      position += snprintf( file_paths_buf + position,
                            file_paths_buf_len - position, 
                            "%s\n", nv_item_file_path );

      /* Check for truncation */
      if( position >= file_paths_buf_len)
      {
        DS_3GPP_MSG1_ERROR("Fail to copy all files path in buffer file_paths_buf_len %d",
                            file_paths_buf_len);
        return -1;
      }
    }/* Only skip items which are failed don't fail init */
    else
    {
      DS_3GPP_MSG1_ERROR("ds_3gppi_cfg_get_item_path failed for item %d skipping",
                          i);
    }
  }

  return position;
}
/*===========================================================================
  FUNCTION DS_3GPP_CHANGE_BYTE_ORDER
  
  DESCRIPTION
    Changes the byte order
    
  PARAMETERS  
    uint32 containing the original byte order
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    unit32 containing the changed (reverse) byte order
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/

uint32 ds_3gpp_chng_byte_order 
(
  uint32 orig_byte_order
)
{
  return (((orig_byte_order >> 24) & 0xff) | ((orig_byte_order >> 8) & 0xff00)
         |((orig_byte_order << 8) & 0xff0000) | 
          ((orig_byte_order << 24) & 0xff000000));
} /* ds_3gpp_chng_byte_order */
/*===========================================================================
  FUNCTION DS_3GPP_EXTRACT_MCC_FROM_PLMN_ID
  
  DESCRIPTION
    This function extracts MCC from PLMN ID
    
  PARAMETERS  
   sys_plmn_id_s_type          *current_plmn_id
   uint16                      *ret_mcc (MCC ptr)
   uint16                      *ret_mnc (MNC ptr)
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    void
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/

void ds_3gpp_extract_mcc_mnc_from_plmn_id
(
   sys_plmn_id_s_type          *current_plmn_id,
   uint16                      *ret_mcc,
   uint16                      *ret_mnc
)
{
  uint8                       mcc_digit[3] = {0};
  uint16                      mcc = 0;
  uint8                       mnc_digit[3] = {0};
  uint16                      mnc = 0;
  /*- - - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((current_plmn_id == NULL) || (ret_mcc == NULL) || 
     (ret_mnc == NULL))
  {
    DS_3GPP_MSG3_ERROR("Input param error:PLMN ID:0x%x, ret_mcc:0x%x, "
                       "ret_mnc:0x%x",current_plmn_id,ret_mnc,ret_mcc);
    return;
  }

  mcc_digit[0] = (current_plmn_id->identity[0] & 0x0F);
  mcc_digit[1] = (current_plmn_id->identity[0] / 0x10);
  mcc_digit[2] = (current_plmn_id->identity[1] & 0x0F);

  mcc = (mcc_digit[0] * 100) + (mcc_digit[1] * 10) + (mcc_digit[2]);

  *ret_mcc = mcc;

  DS_3GPP_MSG1_HIGH("MCC returned is %d",mcc);

  mnc_digit[0] = (current_plmn_id->identity[2] & 0x0F);
  mnc_digit[1] = (current_plmn_id->identity[2] / 0x10);
  mnc_digit[2] = (current_plmn_id->identity[1] / 0x10);

  DS_3GPP_MSG3_HIGH("MNC: %d%d%d",mnc_digit[0],mnc_digit[1],mnc_digit[2]);
  if (mnc_digit[2] == 0xF)
  {
    mnc = 10 * mnc_digit[0] + mnc_digit[1];
  }
  else
  {
    mnc = 100 * mnc_digit[0] + 10 * mnc_digit[1] + mnc_digit[2];
  }

  *ret_mnc = mnc;

  DS_3GPP_MSG1_HIGH("MNC returned is %d",mnc);
   return;

}

/*===========================================================================
FUNCTION DS3G_COMPARE_APN_NAMES
 
DESCRIPTION
  This function compares apn names being and return TRUE if they are same 

PARAMETERS 
  byte apn_src : First APN string
  byte apn_dest: Second APN string
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean TRUE: APN's passed are same
          FALSE: APN's passed are different
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_compare_apn_names
(
  byte *apn_src,
  byte *apn_dest
)
{
  boolean result = FALSE;
/*----------------------------------------------------------------------------*/
  if(strncasecmp((char*)apn_src,(char*)apn_dest,
                                DS_UMTS_MAX_APN_STRING_LEN+1)==0)
  {
    result = TRUE;
  }
  return result;
}/* ds_3gpp_pdn_context_compare_apn_names */

