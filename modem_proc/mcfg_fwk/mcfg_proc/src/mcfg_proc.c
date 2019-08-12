/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

	MODEM_CFG processing INTERFACE

GENERAL DESCRIPTION

  This file makes up the config processing component of the MODEM_CFG
  
  This component encapsulates all the functionality that is
  associated with processing of the modem_cfg segment and writing to
  Filesystem (NV/EFS) or RAMConfig as required.    

EXTERNALIZED FUNCTIONS
    modem_cfg_process                    
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Called from the modem_cfg_task as a part of RCInit Group 2
 
Copyright (c) 2012-2015 by QUALCOMM Technologies Incorporated.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE/

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_proc/src/mcfg_proc.c#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $ 
 
when       who     what, where, why
--------   ---    ----------------------------------------------------------
01/18/12   sbt     Initial Creation.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "modem_mem.h"                            /* Modem Mem header file */
#include "list.h"                           /* List processing header file */    

#include "fs_public.h"                          /* For handling EFS files */
#include "fs_lib.h"
#include "fs_sys_types.h"

#include "nv_items.h"

#include "mcfg_feature_config.h"
#include "mcfg_common.h"
#include "mcfg_osal.h"
#include "mcfg_nv.h"
#include "mcfg_proc_i.h"
#include "mcfg_proc.h"
#include "mcfg_load.h"
#include "mcfg_int.h"
#include "mcfg_osal.h"
#include "mcfg_utils.h"
#include "mcfg_trl.h"
#include "mcfg_uim.h"

#include "mcfg_default.h"

#ifdef FEATURE_MCFG_REFRESH
#include "mcfg_refresh_i.h"
#endif

#include "sys.h"

#if !defined(FEATURE_MCFG_LEGACY_CLEANUP)
#include "policyman_tools.h"
#endif
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global definitions for constants, macros, types,
variables

===========================================================================*/

/* Modem Cfg : format 

 Modem Config Header (16 BYTES)
 Signature - 4 bytes (MCFG)
 Format version # - 2 bytes (MCFG_FORMAT_NUM)
 CFG Type - 2 bytes (MCFG_TYPE_x)
 Number of Config items - 4 bytes
 (minimum length is 1 byte ... i.e contains at least the MCFG_version_num)

 Muxd index info - 2 bytes
 Spare/reserved (2 bytes) - not used for now
-----
this is followed by ....
NV Items in TLV format 
i.e nv item #1 (type) .. the first NV item should be the MCFG version
    length of NV item
	value (x bytes)
-----
 
 
Modem Cfg Version MSb-LSb  :
 
CFG TYPE = MCFG_SW_CONFIG 
    Compat Version : 8 bits
    RFU            : 3 bits (111 for default config)
    Sys Type       : 5 bits
    MajVer : 8 bits (Carrier Major Version)
    MinVer : 8 bits (Carrier Minor Version)
 
CFG TYPE = MCFG_HW_CONFIG (Platform Config) 
    Compat Version : 8 bits
    RFU3           : 3 bits (111 for default config)
    RFU5           : 5 bits (reserved)
    MajVer : 8 bits (Platform Major Version)
    MinVer : 8 bits (Platform Minor Version)
 
*/

#define MCFG_SW_VER_CARRIER_BITS                16
#define MCFG_SW_VER_CARRIER_MINOR_VER_BITS	    8
#define MCFG_SW_VER_COMPAT_VER_BITS	            24

#define MCFG_SW_VER_SYS_TYPE_MASK               0x0F	

#define MCFG_SYS_C_BIT			0  /* C2K */
#define MCFG_SYS_G_BIT	    1  /* GSM */
#define MCFG_SYS_W_BIT			2  /* WCDMA */ 
#define MCFG_SYS_L_BIT			3  /* LTE */
#define MCFG_SYS_T_BIT			4  /* TD-SCDMA */

#define MCFG_FORMAT_NUM				0x03	
#define MCFG_MAGIC					0x4746434D

/* MCFG carrier version (maj) */
#define MCFG_CARRIER_MAJ_VERSION_FACTORY        0x0000
#define MCFG_CARRIER_MAJ_VERSION_VZW            0x0100
#define MCFG_CARRIER_MAJ_VERSION_SPRINT         0x0200
#define MCFG_CARRIER_MAJ_VERSION_ATT            0x0300
#define MCFG_CARRIER_MAJ_VERSION_VODAFONE       0x0400
#define MCFG_CARRIER_MAJ_VERSION_TMOBILE        0x0500
#define MCFG_CARRIER_MAJ_VERSION_TELUS          0x0600
#define MCFG_CARRIER_MAJ_VERSION_KDDI           0x0700
#define MCFG_CARRIER_MAJ_VERSION_GEN_UMTS_EU    0x0800
#define MCFG_CARRIER_MAJ_VERSION_GEN_UMTS_NA    0x0900
#define MCFG_CARRIER_MAJ_VERSION_GEN_C2K        0x0A00
#define MCFG_CARRIER_MAJ_VERSION_ORANGE         0x0B00
#define MCFG_CARRIER_MAJ_VERSION_TELEFONICA     0x0C00
#define MCFG_CARRIER_MAJ_VERSION_DOCOMO         0x0D00
#define MCFG_CARRIER_MAJ_VERSION_TEL_ITALIA     0x0E00
#define MCFG_CARRIER_MAJ_VERSION_TELSTRA        0x0F00
#define MCFG_CARRIER_MAJ_VERSION_LUCACELL       0x1000
#define MCFG_CARRIER_MAJ_VERSION_BELL_MOB       0x1100
#define MCFG_CARRIER_MAJ_VERSION_TELCOM_NZ      0x1200
#define MCFG_CARRIER_MAJ_VERSION_CHINA_TEL      0x1300
#define MCFG_CARRIER_MAJ_VERSION_C2K_OMH        0x1400
#define MCFG_CARRIER_MAJ_VERSION_CHINA_UNI      0x1500
#define MCFG_CARRIER_MAJ_VERSION_AMX            0x1600
#define MCFG_CARRIER_MAJ_VERSION_NORX           0x1700
#define MCFG_CARRIER_MAJ_VERSION_CMCC           0x1800
#define MCFG_CARRIER_MAJ_VERSION_WONE           0x1900
#define MCFG_CARRIER_MAJ_VERSION_AIRTEL         0x1A00
#define MCFG_CARRIER_MAJ_VERSION_RELIANCE       0x1B00
#define MCFG_CARRIER_MAJ_VERSION_SOFTBANK       0x1C00
#define MCFG_CARRIER_MAJ_VERSION_DT             0x1F00
#define MCFG_CARRIER_MAJ_VERSION_US_CELLULAR    0x2000
#define MCFG_CARRIER_MAJ_VERSION_VIVO           0x2100
#define MCFG_CARRIER_MAJ_VERSION_EE             0x2200
#define MCFG_CARRIER_MAJ_VERSION_CHERRY         0x2300
#define MCFG_CARRIER_MAJ_VERSION_IMOBILE        0x2400
#define MCFG_CARRIER_MAJ_VERSION_SMARTFREN      0x2500
#define MCFG_CARRIER_MAJ_VERSION_LGU            0x2600
#define MCFG_CARRIER_MAJ_VERSION_SKT            0x2700
#define MCFG_CARRIER_MAJ_VERSION_TTA            0x2800

// Modem SW Config (UQCN) Item Type
#define U_CARRIER_NV_ITEM               0x01
#define U_CARRIER_NV_EFS_FILE           0x02
#define U_CARRIER_SLOT_NV_ITEM          0x03
#define U_CARRIER_SLOT_NV_EFS_FILE      0x04
#define U_CARRIER_DATA_PROFILE          0x05
#define U_CARRIER_CONFIG_ITEM           0x06

/* Modem SW Config - NV Categorization Info */

#define U_ITEM_ATTRIB_PWMC_MASK         (U_ITEM_ATTRIB_CONDITIONAL | U_ITEM_ATTRIB_WRITE_ONCE | U_ITEM_ATTRIB_MUXD | U_ITEM_ATTRIB_CFG ) 
#define U_ITEM_ATTRIB_WMC_MASK          (U_ITEM_ATTRIB_WRITE_ONCE | U_ITEM_ATTRIB_MUXD | U_ITEM_ATTRIB_CFG ) 
#define U_ITEM_ATTRIB_MULTI_SIM_MASK    (U_ITEM_ATTRIB_MULTISIM | U_ITEM_ATTRIB_CFG ) 

#define MCFG_INT_ERROR                  0xEF

// MCFG EFS file - Type
#define MCFG_EFS_ITEM_TYPE_FILE_PATH    0x01
#define MCFG_EFS_ITEM_TYPE_FILE_DATA    0x02

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
#define MCFG_LOG_CARRIER_INFO_MAX       32
#define MCFG_LOG_HIST_MAX               16
#define MCFG_LOG_HIST_OFFSET            6
#define MCFG_LOG_RECORD_SIZE            4
#define MCFG_LOG_CARR_INFO_OFFSET       ((MCFG_LOG_HIST_OFFSET)+(MCFG_LOG_HIST_MAX*MCFG_LOG_RECORD_SIZE)) 
#endif


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

// Typedefs
typedef PACKED struct 
{ 
  uint32	magic1;
  uint16	mcfg_format_ver_num;
  uint16	mcfg_type;
  uint32	mcfg_num_items;
  uint16	mcfg_muxd_carrier_index_info;
  uint16	spare_crc;
}mcfg_header_type;

// NV Item entry type for the list above
typedef struct                             
{
  list_link_type        link;
  nv_items_enum_type    eItem;
  uint8                 attrib;
  uint8                 sp_ops;
} mcfg_nv_item_list_entry_type;

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
typedef PACKED struct 
{
  uint32    mcfg_log_magic;
  uint8     mcfg_log_history_index;
  uint8     mcfg_log_carrier_info_count;
  uint32    mcfg_log_history[MCFG_LOG_HIST_MAX];
}mcfg_log_type;

typedef PACKED struct 
{
  uint16    carrier_maj_version;
  uint8     muxd_carrier_index;
  uint8     rfu_spare;
}mcfg_log_carrier_info_type;
#endif /* FEATURE_MODEM_CFG_HISTORY_LOGGING */

// Global flags
boolean mcfg_restore_fact_def = FALSE;
uint8   mcfg_ver_compare_result = MCFG_VER_GREATER_MUXD_VERSION;

// Forward declarations
static boolean mcfg_update_required(uint8 cfg_type, uint8* ram_cust_data_ptr, uint32* ram_mcfg_version_ptr, uint8 subs_id );
static boolean mcfg_extra_cleanup(uint8 cfg_type, uint8 subs_id );

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
static uint8 muxd_carrier_index = 0;
boolean mcfg_read_tlv(mcfg_tlv* dest, uint8* src);
boolean mcfg_verify_efs_item_contents(mcfg_tlv* header, mcfg_tlv* body);
static boolean mcfg_should_modify_efs_file(char* path, uint8 attributes, boolean restore_fact);
static void mcfg_set_muxd_index(uint16 mcfg_muxd_carrier_index);
static uint8 mcfg_compare_w_muxd_version(uint8* ram_cust_data_ptr, uint8 subs_id );

#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */


#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
static uint8 mcfg_curr_subs_id;

#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
static fs_size_t mcfg_read_log_info(int file_handle, mcfg_log_type**	mcfg_log_data_ptr);
static boolean mcfg_update_log_info(mcfg_log_type*	mcfg_log_data_ptr, uint32 ram_mcfg_version, fs_size_t full_size);
static boolean mcfg_write_log_info(int file_handle, mcfg_log_type*	mcfg_log_data_ptr, fs_size_t full_size);
static boolean mcfg_update_history_log(uint8 cfg_type, uint32 ram_mcfg_version);
#endif /* FEATURE_MODEM_CFG_HISTORY_LOGGING */


/*===========================================================================
=============================================================================
================================= FUNCTIONS =================================
=============================================================================
===========================================================================*/
#ifdef FEATURE_MCFG_REFRESH

uint32 modem_cfg_process_single
(
  uint8                    current_mcfg_type,
  mcfg_refresh_type_e_type refresh_type, 
  mcfg_refresh_index_type  mcfg_subs_num
)
{
  uint32 modem_cfg_process_status = MODEM_CFG_PROC_SUCCESS;
  uint32 modem_default_process_status = MODEM_CFG_PROC_SUCCESS;
  uint8* mcfg_seg_ptr; /* RAM Ptr to the MCFG segment */ 
  uint8* ram_cust_data_ptr;
  uint32 ram_mcfg_version,num_mcfg_items ;
  mcfg_nv_status_e_type mcfg_nv_write_status = MCFG_NV_STATUS_OK; 

  uint32 mcfg_load_status = 0; 
  mcfg_config_id_s_type  mcfg_current_id;
  mcfg_config_s_type     mcfg_current_buf;
  mcfg_config_s_type*    p_mcfg_buf=NULL;
  uint32 mcfg_add_info = 0;

/* -------------------------------------------------------------------- */
  MCFG_MSG_HIGH_1("====  Starting modem_cfg_process_single %d ==== ", mcfg_subs_num);
  MCFG_MSG_HIGH_1("refresh_type %d ", refresh_type);

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
  iteration_process_status = MODEM_CFG_PROC_SUCCESS; // assume success at start of each iteration
#endif

  do
  {
    /* Process default configuration */
    modem_default_process_status = 
      mcfg_default_config_process(current_mcfg_type, (mcfg_sub_id_type_e_type)mcfg_subs_num);
    if(modem_default_process_status == MODEM_CFG_UPDATE_NOT_REQUIRED)
    {
      MCFG_MSG_LOW_1("mcfg default config not required for cfg type 0x%0x,",
                     current_mcfg_type);
    }
    else if (modem_default_process_status != MODEM_CFG_PROC_SUCCESS)
    {
       MCFG_MSG_ERROR_1("mcfg default config failed to load for cfg type 0x%0x,",
                       current_mcfg_type);
       modem_cfg_process_status = MODEM_CFG_PROC_LOAD_FAIL; 
       break;
    }

    p_mcfg_buf = &mcfg_current_buf;
    mcfg_current_buf.config_len = 0;
    mcfg_current_buf.config_addr = NULL;

    /* LOAD AND AUTHENTICATE MODEM CFG IMAGE */
    mcfg_load_status = modem_cfg_load_and_auth_mcfg(current_mcfg_type, 
                                                    &mcfg_current_id, 
                                                    p_mcfg_buf, FALSE, 
                                                    (mcfg_sub_id_type_e_type)mcfg_subs_num);

    if(mcfg_load_status != MODEM_CFG_PROC_SUCCESS)
    {
      MCFG_MSG_ERROR_3("mcfg_load_and_auth_seg fail for cfg type %0x, status = %d, subs_id = %d",
                        current_mcfg_type, (int)mcfg_load_status, mcfg_subs_num );
      modem_cfg_process_status = mcfg_load_status;
      break;
    }

    /* Get the MCFG SEG from the Modem Config File buffer */
    mcfg_seg_ptr = mcfg_load_seg(p_mcfg_buf, &mcfg_add_info );
    if (mcfg_seg_ptr == NULL )
    {
      MCFG_MSG_ERROR_2("Cannot find MCFG_SEG for cfg_type %d, subs_id %d",current_mcfg_type, mcfg_subs_num);
      modem_cfg_process_status = MODEM_CFG_PROC_LOAD_FAIL; 
      break;
    }

    /* Initialize our RAM ptr to the start of MCFG segment */
    ram_cust_data_ptr = mcfg_seg_ptr;

    /* Validate the MCFG Header Information and get the number of NV items from this data */
    num_mcfg_items = validate_mcfg_header(current_mcfg_type, ram_cust_data_ptr, mcfg_subs_num, refresh_type);

    if (!num_mcfg_items ) 
    {
      MCFG_MSG_ERROR_2("Validate MCFG Header Fail, cfg_type = %0x! subs_id=%d", 
                        current_mcfg_type, mcfg_subs_num);
      modem_cfg_process_status = MODEM_CFG_HEADER_INVALID;
      break;
    }
    ram_cust_data_ptr+= MCFG_HEADER_LEN; /* the ram_cust_data_ptr points to the first MCFG NV element */

    if( mcfg_update_required(current_mcfg_type, ram_cust_data_ptr, &ram_mcfg_version, mcfg_subs_num))
    {
      if (refresh_type != MCFG_REFRESH_TYPE_SUBS)
      {
        mcfg_extra_cleanup(current_mcfg_type, mcfg_subs_num);
      }

      ram_cust_data_ptr = mcfg_seg_ptr + MCFG_NV_DATA_OFFSET;
          
      if (refresh_type != MCFG_REFRESH_TYPE_SUBS)
      {
        if (mcfg_update_nvefs(current_mcfg_type, num_mcfg_items, ram_cust_data_ptr, 
                              mcfg_subs_num, MCFG_REFRESH_TYPE_SLOT))
        {
          MCFG_MSG_ERROR_2("Update NVEFS Fail for cfg type, %0x! subs_id=%d", current_mcfg_type, mcfg_subs_num);
          modem_cfg_process_status = MODEM_CFG_UPDATE_NVEFS_FAIL;
          break;
        }
      }
                
      if (refresh_type != MCFG_REFRESH_TYPE_SLOT)
      {
        if (mcfg_update_nvefs(current_mcfg_type, num_mcfg_items, ram_cust_data_ptr, 
                              mcfg_subs_num, MCFG_REFRESH_TYPE_SUBS))
        {
          MCFG_MSG_ERROR_2("Update NVEFS Fail for cfg type, %0x! subs_id=%d", current_mcfg_type, mcfg_subs_num);
          modem_cfg_process_status = MODEM_CFG_UPDATE_NVEFS_FAIL;
          break;
        }
      }
      
      /* Update the correct MCFG Version to NV to wrap up */
      if (refresh_type != MCFG_REFRESH_TYPE_SLOT)
      {
        if(current_mcfg_type == MCFG_TYPE_SW)
        {
          mcfg_nv_write_status = mcfg_nv_write( MCFG_SW_VERSION_I, 0, mcfg_subs_num,  
                                                 (uint8 *)&ram_mcfg_version, sizeof (ram_mcfg_version) );
        }
        else /* if (current_mcfg_type == MCFG_TYPE_HW)*/
        {
          mcfg_nv_write_status = mcfg_nv_write( MCFG_HW_VERSION_I, 0,mcfg_subs_num, 
                                             (uint8 *)&ram_mcfg_version, sizeof (ram_mcfg_version) );
        }

        MCFG_MSG_HIGH_2("Update mcfg version to %0lx, subs_id=%d", ram_mcfg_version, mcfg_subs_num);

        if(mcfg_nv_write_status != MCFG_NV_STATUS_OK)
        {
          MCFG_MSG_ERROR_2("Update Cfg version fail for cfg type, %0x!, subs_id=%d", 
                           current_mcfg_type, mcfg_subs_num);
          modem_cfg_process_status = MODEM_CFG_VERSION_UPDATE_FAIL;
          break;
        }

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
        /* Update Muxd MCFG Info structure */
        mcfg_nv_write_status = mcfg_nv_write( MCFG_NV_ITEM_MCFG_SW_MUXD_VERSION_I, 
                                              mcfg_get_muxd_index(), mcfg_subs_num,
                                             (uint8 *)&ram_mcfg_version,
                                             sizeof(ram_mcfg_version) );

        if(mcfg_nv_write_status != MCFG_NV_STATUS_OK)
        {
          MCFG_MSG_ERROR_2("Update Muxd cfg version fail for cfg type, %0x!, subs_id=%d", 
                           current_mcfg_type, mcfg_subs_num);
          modem_cfg_process_status = MODEM_CFG_MUXD_VERSION_UPDATE_FAIL;
          break;
        }
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */

        if(mcfg_restore_fact_def)
        {
          mcfg_restore_fact_def = FALSE; 

          /* Note that following can be technically done here, but this would be going too far for the user */
          /* Set MIN, MDN to 0's to force ui_init_provisioning */
          /* Remove GPS service data files. */
        }

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
        /* For debug purposes, update the details to the MCFG history log */
        iteration_process_status = modem_cfg_process_status;
        MCFG_MSG_HIGH_1("mcfg_proc - iteration_process_status = %d", iteration_process_status);
        if(iteration_process_status == MODEM_CFG_PROC_SUCCESS)
        {
          if(!mcfg_update_history_log(current_mcfg_type, ram_mcfg_version))
          {
            MCFG_MSG_ERROR_3( "MCFG history log unsuccessful", 0, 0, 0 );
          }
        }
#endif /* FEATURE_MODEM_CFG_HISTORY_LOGGING */
      }
    }/* mcfg_update_required */

    if (refresh_type != MCFG_REFRESH_TYPE_SLOT)
    {
      /* Set the new processed config to be the active_config */
      mcfg_utils_set_active_config(current_mcfg_type, &mcfg_current_id, 
                                   (mcfg_sub_id_type_e_type)mcfg_subs_num );
    }
  }
  while (0); 

  if (p_mcfg_buf)
  {
    /* Free the p_mcfg_buf now that the MCFG processing is done for this mcfg_type */
    mcfg_utils_free_config_buffer(p_mcfg_buf);
  }

  return modem_cfg_process_status;
} /* modem_cfg_process_single() */

/*===========================================================================

FUNCTION  modem_cfg_process() 

DESCRIPTION
  This is the main modem config processing routine called
  during modem config task startup 

DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  uint32 - Initialization success returns 0, othewise returns the error code.

SIDE EFFECTS
  None

===========================================================================*/
uint32 modem_cfg_process( void )
{
  uint32 modem_cfg_process_status = MODEM_CFG_PROC_SUCCESS;
  uint8 current_mcfg_type;
  uint8  mcfg_subs_num = MCFG_SUB_ID_FIRST;
  uint8  mcfg_max_subs = MCFG_SUB_ID_FIRST + 1;

/* -------------------------------------------------------------------- */
  MCFG_MSG_HIGH("====  Starting modem_cfg_process ==== ");

  /* Process Cfg loop */
  for(current_mcfg_type=MCFG_TYPE_MIN; current_mcfg_type <= MCFG_TYPE_MAX; current_mcfg_type++ )
  {
    if(current_mcfg_type == MCFG_TYPE_SW) 
      mcfg_max_subs = MCFG_NUM_OF_SUB_ID;

    for(mcfg_subs_num = MCFG_SUB_ID_FIRST; mcfg_subs_num < mcfg_max_subs; mcfg_subs_num++)
    {
      modem_cfg_process_status = modem_cfg_process_single(
        current_mcfg_type, MCFG_REFRESH_TYPE_SLOT_N_SUBS, mcfg_subs_num);
    } /* for mcfg_subs_num loop */
  }/* for - current_cfg_type loop */

  return modem_cfg_process_status;
} /* modem_cfg_process() */

#else
/*===========================================================================

FUNCTION  modem_cfg_process() 

DESCRIPTION
  This is the main modem config processing routine called
  during modem config task startup 

DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  uint32 - Initialization success returns 0, othewise returns the error code.

SIDE EFFECTS
  None

===========================================================================*/
uint32 modem_cfg_process( void )
{
  uint32 modem_cfg_process_status = MODEM_CFG_PROC_SUCCESS;
  uint32 modem_default_process_status = MODEM_CFG_PROC_SUCCESS;
  /* RAM Ptr to the MCFG segment */ 
  uint8* mcfg_seg_ptr; 
  uint8* ram_cust_data_ptr;
   
  uint32 ram_mcfg_version,num_mcfg_items ;
  mcfg_nv_status_e_type mcfg_nv_write_status; 

  uint32 mcfg_load_status = 0; 
  uint8 current_mcfg_type;
  static mcfg_config_id_s_type  mcfg_current_id;
  static mcfg_config_s_type  mcfg_current_buf;
  mcfg_config_s_type*  p_mcfg_buf=NULL;
  uint32 mcfg_add_info = 0;
  uint8  mcfg_subs_num = MCFG_SUB_ID_FIRST;
  uint8  mcfg_max_subs = MCFG_SUB_ID_FIRST + 1;

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
  uint32 iteration_process_status = MODEM_CFG_PROC_SUCCESS;
#endif

/* -------------------------------------------------------------------- */
  MCFG_MSG_HIGH("====  Starting modem_cfg_process ==== ");

  /* Process Cfg loop */
  for(current_mcfg_type=MCFG_TYPE_MIN; current_mcfg_type <= MCFG_TYPE_MAX; current_mcfg_type++ )
  {

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
    iteration_process_status = MODEM_CFG_PROC_SUCCESS; // assume success at start of each iteration
#endif

    /* Process default configuration */
    modem_default_process_status = 
      mcfg_default_config_process(current_mcfg_type, MCFG_SUB_ID_FIRST);
    if(modem_default_process_status == MODEM_CFG_UPDATE_NOT_REQUIRED)
    {
      MCFG_MSG_LOW_1("mcfg default config not required for cfg type 0x%0x,",
                     current_mcfg_type);
    }
    else if (modem_default_process_status != MODEM_CFG_PROC_SUCCESS)
    {
       MCFG_MSG_ERROR_1("mcfg default config failed to load for cfg type 0x%0x,",
                       current_mcfg_type);
       continue;
    }

    if(current_mcfg_type == MCFG_TYPE_SW) 
      mcfg_max_subs = MCFG_NUM_OF_SUB_ID;

    mcfg_restore_fact_def = FALSE;
    for(mcfg_subs_num = MCFG_SUB_ID_FIRST; mcfg_subs_num < mcfg_max_subs; mcfg_subs_num++)
    {
      p_mcfg_buf = &mcfg_current_buf;
      mcfg_current_buf.config_len = 0;
      mcfg_current_buf.config_addr = NULL;

      /* LOAD AND AUTHENTICATE MODEM CFG IMAGE */
      mcfg_load_status = modem_cfg_load_and_auth_mcfg(current_mcfg_type, &mcfg_current_id, p_mcfg_buf, FALSE, mcfg_subs_num);

      if(mcfg_load_status != MODEM_CFG_PROC_SUCCESS)
      {
        MCFG_MSG_ERROR_3("mcfg_load_and_auth_seg fail for cfg type %0x, status = %d, subs_id = %d",
                          current_mcfg_type, (int)mcfg_load_status, mcfg_subs_num );
        modem_cfg_process_status = mcfg_load_status;
        /* Load the next config type */
        continue;
      }

      /* Get the MCFG SEG from the Modem Config File buffer */
      mcfg_seg_ptr = mcfg_load_seg(p_mcfg_buf, &mcfg_add_info );

      if (mcfg_seg_ptr == NULL )
      {
        MCFG_MSG_ERROR_2("Cannot find MCFG_SEG for cfg_type %d, subs_id %d",current_mcfg_type, mcfg_subs_num);
        modem_cfg_process_status = MODEM_CFG_PROC_LOAD_FAIL; 
        /* Load the next config type */
        continue;
      }

      /* Initialize our RAM ptr to the start of MCFG segment */
      ram_cust_data_ptr = mcfg_seg_ptr;

      /* Validate the MCFG Header Information and get the number of NV items from this data */
      num_mcfg_items = validate_mcfg_header(current_mcfg_type, ram_cust_data_ptr, mcfg_subs_num, 
                                            MCFG_REFRESH_TYPE_SLOT_N_SUBS );

      if (!num_mcfg_items ) 
      {
        MCFG_MSG_ERROR_2("Validate MCFG Header Fail, cfg_type = %0x! subs_id=%d", 
                          current_mcfg_type, mcfg_subs_num);
        mcfg_utils_free_config_buffer(p_mcfg_buf);
        modem_cfg_process_status = MODEM_CFG_HEADER_INVALID;
        /* Load the next config type */
        continue;
      }
      else
      {
        ram_cust_data_ptr+= MCFG_HEADER_LEN; /* the ram_cust_data_ptr points to the first MCFG NV element */
      }

      if( mcfg_update_required(current_mcfg_type, ram_cust_data_ptr, &ram_mcfg_version, mcfg_subs_num))
      {
        mcfg_extra_cleanup(current_mcfg_type, mcfg_subs_num);

        ram_cust_data_ptr = mcfg_seg_ptr + MCFG_NV_DATA_OFFSET;
            
        if (mcfg_update_nvefs(current_mcfg_type, num_mcfg_items, ram_cust_data_ptr, mcfg_subs_num, MCFG_REFRESH_TYPE_SLOT_N_SUBS))
        {
          MCFG_MSG_ERROR_2("Update NVEFS Fail for cfg type, %0x! subs_id=%d", current_mcfg_type, mcfg_subs_num);
          mcfg_utils_free_config_buffer(p_mcfg_buf);
          modem_cfg_process_status = MODEM_CFG_UPDATE_NVEFS_FAIL;
          /* Load the next config type */
          continue;
        }
          
        /* Update the correct MCFG Version to NV to wrap up */
        if(current_mcfg_type == MCFG_TYPE_SW)
        {
          mcfg_nv_write_status = mcfg_nv_write( MCFG_SW_VERSION_I, 0, mcfg_subs_num,  
                                                 (uint8 *)&ram_mcfg_version, sizeof (ram_mcfg_version) );
        }
        else if (current_mcfg_type == MCFG_TYPE_HW)
        {
          mcfg_nv_write_status = mcfg_nv_write( MCFG_HW_VERSION_I, 0,mcfg_subs_num, 
                                             (uint8 *)&ram_mcfg_version, sizeof (ram_mcfg_version) );
        }

        MCFG_MSG_HIGH_2("Update mcfg version to %0lx, subs_id=%d", ram_mcfg_version, mcfg_subs_num);

        if(mcfg_nv_write_status != MCFG_NV_STATUS_OK)
        {
          MCFG_MSG_ERROR_2("Update Cfg version fail for cfg type, %0x!, subs_id=%d", 
                           current_mcfg_type, mcfg_subs_num);
          mcfg_utils_free_config_buffer(p_mcfg_buf);
          modem_cfg_process_status = MODEM_CFG_VERSION_UPDATE_FAIL;
        }

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
      /* Update Muxd MCFG Info structure */

        mcfg_nv_write_status = mcfg_nv_write( MCFG_NV_ITEM_MCFG_SW_MUXD_VERSION_I, 
                                              mcfg_get_muxd_index(), 
                                              mcfg_subs_num,
                                             (uint8 *)&ram_mcfg_version,
                                             sizeof(ram_mcfg_version) );

        if(mcfg_nv_write_status != MCFG_NV_STATUS_OK)
        {
          MCFG_MSG_ERROR_2("Update Muxd cfg version fail for cfg type, %0x!, subs_id=%d", 
                           current_mcfg_type, mcfg_subs_num);
          mcfg_utils_free_config_buffer(p_mcfg_buf);
          modem_cfg_process_status = MODEM_CFG_MUXD_VERSION_UPDATE_FAIL;
        }
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */

      if(mcfg_restore_fact_def)
      {
        mcfg_restore_fact_def = FALSE; 

        /* Note that following can be technically done here, but this would be going too far for the user */
        /* Set MIN, MDN to 0's to force ui_init_provisioning */
        /* Remove GPS service data files. */
      }

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
      /* For debug purposes, update the details to the MCFG history log */
      iteration_process_status = modem_cfg_process_status;
      MCFG_MSG_HIGH_1("mcfg_proc - iteration_process_status = %d", iteration_process_status);
      if(iteration_process_status == MODEM_CFG_PROC_SUCCESS)
      {
        if(!mcfg_update_history_log(current_mcfg_type, ram_mcfg_version))
        {
          MCFG_MSG_ERROR_3( "MCFG history log unsuccessful", 0, 0, 0 );
        }
      }
#endif /* FEATURE_MODEM_CFG_HISTORY_LOGGING */

    }/* mcfg_update_required */

    /* Set the new processed config to be the active_config */
    mcfg_utils_set_active_config(current_mcfg_type, &mcfg_current_id, mcfg_subs_num );
    /* Free the p_mcfg_buf now that the MCFG processing is done for this mcfg_type */
    mcfg_utils_free_config_buffer(p_mcfg_buf);


    } /* for mcfg_subs_num loop */

  }/* for - current_cfg_type loop */

  return modem_cfg_process_status;

} /* modem_cfg_process() */
#endif

#if 0
/*===========================================================================

FUNCTION  modem_cfg_process() 

DESCRIPTION
  This is the main modem config processing routine called
  during modem config task startup 

DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  uint32 - Initialization success returns 0, othewise returns the error code.

SIDE EFFECTS
  None

===========================================================================*/
uint32 mcfg_process_for_refresh
( 
  uint8                    mcfg_type
  mcfg_refresh_type_e_type refresh_type, 
  mcfg_refresh_index_type  slot_index,
  mcfg_refresh_index_type  sub_index,
)
{
  uint32 modem_cfg_process_status = MODEM_CFG_PROC_SUCCESS;
  uint32 modem_default_process_status = MODEM_CFG_PROC_SUCCESS;
  /* RAM Ptr to the MCFG segment */ 
  uint8* mcfg_seg_ptr; 
  uint8* ram_cust_data_ptr;
   
  uint32 ram_mcfg_version,num_mcfg_items ;
  mcfg_nv_status_e_type mcfg_nv_write_status; 

  uint32 mcfg_load_status = 0; 
  static mcfg_config_id_s_type  mcfg_current_id;
  static mcfg_config_s_type  mcfg_current_buf;
  mcfg_config_s_type*  p_mcfg_buf=NULL;
  uint32 mcfg_add_info = 0;
  uint8  mcfg_subs_num = MCFG_SUB_ID_FIRST;
  uint8  mcfg_max_subs = MCFG_SUB_ID_FIRST + 1;

/* -------------------------------------------------------------------- */
  MCFG_MSG_HIGH("====  Starting mcfg_process_for_refresh ==== ");

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
  // assume success at start of each iteration
  iteration_process_status = MODEM_CFG_PROC_SUCCESS; 
#endif

  /* Process default configuration */
  modem_default_process_status = mcfg_default_config_process(mcfg_type);
  if(modem_default_process_status == MODEM_CFG_UPDATE_NOT_REQUIRED)
  {
    MCFG_MSG_LOW_1("mcfg default config not required for cfg type 0x%0x,",
                   current_mcfg_type);
  }
  do
  {
    mcfg_subs_num = (type == MCFG_REFRESH_TYPE_SUBS) 
                    ? index : mcfg_refresh_slot_to_sub_index(index);

    if (type != MCFG_REFRESH_TYPE_SUBS)
    {
      p_mcfg_buf = &mcfg_current_buf;
      mcfg_current_buf.config_len = 0;
      mcfg_current_buf.config_addr = NULL;

      /* LOAD AND AUTHENTICATE MODEM CFG IMAGE */
      mcfg_load_status = modem_cfg_load_and_auth_mcfg(current_mcfg_type, &mcfg_current_id, p_mcfg_buf, FALSE, mcfg_subs_num);

      if(mcfg_load_status != MODEM_CFG_PROC_SUCCESS)
      {
        MCFG_MSG_ERROR_3("mcfg_load_and_auth_seg fail for cfg type %0x, status = %d, subs_id = %d",
                          current_mcfg_type, (int)mcfg_load_status, mcfg_subs_num );
        modem_cfg_process_status = mcfg_load_status;
        /* Load the next config type */
        break;
      }

      /* Get the MCFG SEG from the Modem Config File buffer */
      mcfg_seg_ptr = mcfg_load_seg(p_mcfg_buf, &mcfg_add_info );

      if (mcfg_seg_ptr == NULL )
      {
        MCFG_MSG_ERROR_2("Cannot find MCFG_SEG for cfg_type %d, subs_id %d",current_mcfg_type, mcfg_subs_num);
        modem_cfg_process_status = MODEM_CFG_PROC_LOAD_FAIL; 
        break;
      }


      /* Initialize our RAM ptr to the start of MCFG segment */
      ram_cust_data_ptr = mcfg_seg_ptr;

      /* Validate the MCFG Header Information and get the number of NV items from this data */
      num_mcfg_items = validate_mcfg_header(current_mcfg_type, ram_cust_data_ptr, mcfg_subs_num );
      if (!num_mcfg_items ) 
      {
        MCFG_MSG_ERROR_2("Validate MCFG Header Fail, cfg_type = %0x! subs_id=%d", 
                          current_mcfg_type, mcfg_subs_num);
        mcfg_utils_free_config_buffer(p_mcfg_buf);
        modem_cfg_process_status = MODEM_CFG_HEADER_INVALID;
        /* Load the next config type */
        break;
      }

      ram_cust_data_ptr+= MCFG_HEADER_LEN; /* the ram_cust_data_ptr points to the first MCFG NV element */


    }
  }while(0);

  if( mcfg_update_required(current_mcfg_type, ram_cust_data_ptr, &ram_mcfg_version, mcfg_subs_num))
  {
    mcfg_extra_cleanup(current_mcfg_type, mcfg_subs_num);

    ram_cust_data_ptr = mcfg_seg_ptr + MCFG_NV_DATA_OFFSET;
        
    if (mcfg_update_nvefs(current_mcfg_type, num_mcfg_items, ram_cust_data_ptr, mcfg_subs_num))
    {
      MCFG_MSG_ERROR_2("Update NVEFS Fail for cfg type, %0x! subs_id=%d", current_mcfg_type, mcfg_subs_num);
      mcfg_utils_free_config_buffer(p_mcfg_buf);
      modem_cfg_process_status = MODEM_CFG_UPDATE_NVEFS_FAIL;
      /* Load the next config type */
      continue;
    }
      
    /* Update the correct MCFG Version to NV to wrap up */
    if(current_mcfg_type == MCFG_TYPE_SW)
    {
      mcfg_nv_write_status = mcfg_nv_write( MCFG_SW_VERSION_I, 0, mcfg_subs_num,  
                                             (uint8 *)&ram_mcfg_version, sizeof (ram_mcfg_version) );
    }
    else if (current_mcfg_type == MCFG_TYPE_HW)
    {
      mcfg_nv_write_status = mcfg_nv_write( MCFG_HW_VERSION_I, 0,mcfg_subs_num, 
                                         (uint8 *)&ram_mcfg_version, sizeof (ram_mcfg_version) );
    }

    if(mcfg_nv_write_status != MCFG_NV_STATUS_OK)
    {
      MCFG_MSG_ERROR_2("Update Cfg version fail for cfg type, %0x!, subs_id=%d", 
                       current_mcfg_type, mcfg_subs_num);
      mcfg_utils_free_config_buffer(p_mcfg_buf);
      modem_cfg_process_status = MODEM_CFG_VERSION_UPDATE_FAIL;
    }

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
  /* Update Muxd MCFG Info structure */

    mcfg_nv_write_status = mcfg_nv_write( MCFG_NV_ITEM_MCFG_SW_MUXD_VERSION_I, 0, mcfg_subs_num,
                                         (uint8 *)&ram_mcfg_version,
                                         sizeof(ram_mcfg_version) );

    if(mcfg_nv_write_status != MCFG_NV_STATUS_OK)
    {
      MCFG_MSG_ERROR_2("Update Muxd cfg version fail for cfg type, %0x!, subs_id=%d", 
                       current_mcfg_type, mcfg_subs_num);
      mcfg_utils_free_config_buffer(p_mcfg_buf);
      modem_cfg_process_status = MODEM_CFG_MUXD_VERSION_UPDATE_FAIL;
    }
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */

  if(mcfg_restore_fact_def)
  {
    mcfg_restore_fact_def = FALSE; 

    /* Note that following can be technically done here, but this would be going too far for the user */
    /* Set MIN, MDN to 0's to force ui_init_provisioning */
    /* Remove GPS service data files. */
  }

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
  /* For debug purposes, update the details to the MCFG history log */
  iteration_process_status = modem_cfg_process_status;
  MCFG_MSG_HIGH_1("mcfg_proc - iteration_process_status = %d", iteration_process_status);
  if(iteration_process_status == MODEM_CFG_PROC_SUCCESS)
  {
    if(!mcfg_update_history_log(current_mcfg_type, ram_mcfg_version))
    {
      MCFG_MSG_ERROR_3( "MCFG history log unsuccessful", 0, 0, 0 );
    }
  }
#endif /* FEATURE_MODEM_CFG_HISTORY_LOGGING */

  }/* mcfg_update_required */

  /* Set the new processed config to be the active_config */
  mcfg_utils_set_active_config(current_mcfg_type, &mcfg_current_id, mcfg_subs_num );
  /* Free the p_mcfg_buf now that the MCFG processing is done for this mcfg_type */
  mcfg_utils_free_config_buffer(p_mcfg_buf);

  return modem_cfg_process_status;

} /* modem_cfg_process() */
#endif

/*===========================================================================

FUNCTION  validate_mcfg_header() 

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  uint32 - Returns num of MCFG items from the header if valid

SIDE EFFECTS
  None

===========================================================================*/
uint32 validate_mcfg_header(uint8 cfg_type , uint8* mcfg_hdr_ptr, uint8 subs_id,
                            mcfg_refresh_type_e_type refresh_type)
{
  mcfg_header_type  *mcfg_hdr;
  uint32 mcfg_nv_count = 0;

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
  mcfg_item_prefix_type  u_item_prefix;
  uint8* ram_mcfg_data_ptr;
  uint8  mcfg_item_parse_result;
  uint32 i;
#endif

  mcfg_hdr = mcfg_malloc( (sizeof(uint8) * MCFG_HEADER_LEN) );
  if(mcfg_hdr == NULL)
  {
    mcfg_nv_count = 0;
    return mcfg_nv_count;
  }

  memscpy( mcfg_hdr, sizeof(mcfg_header_type),
           mcfg_hdr_ptr, MCFG_HEADER_LEN * sizeof(uint8)); 

  mcfg_nv_count = mcfg_hdr->mcfg_num_items;

  /* Compare magic num, MCFG format ver num and return 0 if not valid */
  if( ( mcfg_hdr->magic1 != MCFG_MAGIC ) || 
      ( mcfg_hdr->mcfg_format_ver_num != MCFG_FORMAT_NUM) ||
      ( mcfg_hdr->mcfg_type > MCFG_TYPE_MAX  ) )
  {
    mcfg_nv_count = 0;
  }
  else
  {
#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
    /* Update the muxd carrier index info */
  
    MCFG_MSG_LOW_1("validate_mcfg_header mcfg_ver_compare_result=%d", mcfg_ver_compare_result);
    MCFG_MSG_LOW_1("validate_mcfg_header mcfg_hdr->mcfg_muxd_carrier_index_info=%d", mcfg_hdr->mcfg_muxd_carrier_index_info);
    if (cfg_type == MCFG_TYPE_SW)
    {
      if( mcfg_hdr->mcfg_format_ver_num >= 0x01 )
      {
        mcfg_set_muxd_index(mcfg_hdr->mcfg_muxd_carrier_index_info);
      }
  
      /* Check if the MCFG Version is same as the Muxd MCFG version before parsing its contents */
      mcfg_ver_compare_result = mcfg_compare_w_muxd_version(mcfg_hdr_ptr, subs_id);
      MCFG_MSG_LOW_1("validate_mcfg_header mcfg_ver_compare_result=%d", mcfg_ver_compare_result);
  
      if ( (mcfg_hdr->mcfg_format_ver_num >= 0x01) && (mcfg_nv_count>1) ) 
      {
        // Parse the MCFG and create the Muxd List here
        ram_mcfg_data_ptr = mcfg_hdr_ptr + MCFG_NV_DATA_OFFSET;
        for (i = 1; i<mcfg_nv_count; i++) 
        {
          /* Parse the MCFG Item Prefix for further processing */
          mcfg_item_parse_result = 
            mcfg_parse_item_prefix(cfg_type, ram_mcfg_data_ptr, &u_item_prefix, refresh_type, subs_id);
          if(mcfg_item_parse_result == MCFG_INT_ERROR)
          {
            MCFG_MSG_ERROR_1("MCFG Item prefix parse error Id=%d", u_item_prefix.item_type);
            mcfg_nv_count=0;
            break;
          }
          ram_mcfg_data_ptr +=  u_item_prefix.item_length;
        }
      }
    } /* cfg_type == MCFG_TYPE_SW */
  
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */
    
    /* Can also add CRC checks here for data integrity etc */
 
  }

  mcfg_free(mcfg_hdr);
  return mcfg_nv_count;

} /* validate_mcfg_header() */

/*===========================================================================

  FUNCTION mcfg_parse_item_prefix

  DESCRIPTION
    Parses the MCFG item prefix 
    If bInitMuxList is TRUE, then it populates the mcfg_muxd_items[] and 
    mcfg_write_once_items[] lists.
 
    Note : bInitMuxList is set to TRUE during startup - validate header,
           to create the muxd items list. 

  DEPENDENCIES
    None.

  RETURN VALUE
    Current MCFG Enum parsed

  SIDE EFFECTS

===========================================================================*/
uint8 mcfg_parse_item_prefix(uint8 cfg_type,
                                    uint8* ram_cust_data_ptr, 
                                    mcfg_item_prefix_type* mcfg_item_prefix_ptr,
                             mcfg_refresh_type_e_type refresh_type, 
                             uint8 subs_id)
{
  uint8 ret_val=MCFG_INT_ERROR;
  uint8 item_type;
  mcfg_slot_index_type_e_type sloti;
   /* Populate the Item Prefix from the  Data Ptr*/
    memscpy(mcfg_item_prefix_ptr, sizeof(mcfg_item_prefix_type),
            ram_cust_data_ptr, sizeof(mcfg_item_prefix_type)); 
    item_type = mcfg_item_prefix_ptr->item_type;

    if ((item_type == U_CARRIER_NV_ITEM || item_type == U_CARRIER_NV_EFS_FILE || item_type == U_CARRIER_DATA_PROFILE) 
        && (refresh_type == MCFG_REFRESH_TYPE_SLOT))
    {
      //MCFG_MSG_LOW_1("skip item_type %d for non-slot items", item_type);
      return MCFG_INT_SW_ONLY;
    }
    else if ((item_type == U_CARRIER_SLOT_NV_ITEM || item_type == U_CARRIER_SLOT_NV_EFS_FILE)
             && (refresh_type == MCFG_REFRESH_TYPE_SUBS))
    {
      //MCFG_MSG_LOW_1("skip item_type %d for slot items", item_type);
      return MCFG_INT_SW_ONLY;
    }

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
    if (item_type == U_CARRIER_SLOT_NV_ITEM || item_type == U_CARRIER_SLOT_NV_EFS_FILE)
    {
      sloti = mcfg_uim_map_sub_id_to_slot_index(subs_id);
      MCFG_ASSERT_COND_RET_ERROR(sloti < MCFG_NUM_OF_UIM_SLOT, MCFG_INT_SW_ONLY);
      //MCFG_MSG_LOW_1("mcfg_set_curr_subs_id slot_item set to %d", sloti);
      mcfg_set_curr_subs_id( (uint8)sloti);
    }
    else
    {
      //MCFG_MSG_LOW_1("mcfg_set_curr_subs_id subscription_item set to %d", subs_id);
      mcfg_set_curr_subs_id( (uint8)subs_id );
    }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

    /* Validate the Item Type */
    switch(item_type)
    {
      case U_CARRIER_NV_ITEM:
      case U_CARRIER_SLOT_NV_ITEM:
          // NOTE - MCFG update takes priority over delete 
          if ( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_CFG ) 
          {
            ret_val = MCFG_INT_NV_ITEM;
#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
            if ( ((mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_MUXD ) &&
                 (cfg_type == MCFG_TYPE_SW) )
               ret_val = MCFG_INT_MUXD_NV_ITEM;
#endif
          }
          else if ( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_DELETE ) 
          {
            ret_val = MCFG_INT_DELETE_NV_ITEM;
          }
          else
          {
            /* Muxd-SW NV Items */
            if( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_MUXD )
            {
              ret_val = MCFG_INT_MUXD_SW_ONLY;
            }
          }
          break;

      case U_CARRIER_NV_EFS_FILE:
      case U_CARRIER_SLOT_NV_EFS_FILE:
          if ( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_CFG ) 
          {
            ret_val = MCFG_INT_EFS_FILE;
            if( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_WRITE_ONCE ) 
            {
               ret_val = MCFG_INT_STATIC_WO_EFS_FILE;

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT_PDP
               /* EFS Mux for PDP profiles is done in the PDP registry, only decision made here is 
               whether the profiles are to be updated or not based on the conditional attrib */
               if( ( mcfg_item_prefix_ptr->attrib & U_ITEM_ATTRIB_CONDITIONAL) 
                  && (cfg_type == MCFG_TYPE_SW) )  
               {
                  if( mcfg_ver_compare_result != MCFG_VER_EQUALS_MUXD_VERSION )
                  {
                      ret_val = MCFG_INT_EFS_FILE;
                  }
               }
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT_PDP */
            }

            if((mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_MUXD)
            {
              ret_val = MCFG_INT_MUXD_EFS_FILE;
            }
          }
          else if ( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_DELETE ) 
          {
            ret_val = MCFG_INT_DELETE_EFS_FILE;
          }
          else
          {
            /* Muxd-SW EFS files */
            if( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_MUXD )
            {
              ret_val = MCFG_INT_MUXD_SW_ONLY_EFS_FILE;
            }
          }
          break;

      case U_CARRIER_DATA_PROFILE:
         if ( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_CFG ) 
         {
           ret_val = MCFG_INT_DATA_PROFILE;
           if( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_WRITE_ONCE ) 
           {
              ret_val = MCFG_INT_STATIC_WO_DATA_PROFILE;
           }

           /* FIXME: What should be done reg. muxed profiles in light of the data team's refactoring of how profiles work */
           if((mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_MUXD)
           {
             ret_val = MCFG_INT_MUXED_DATA_PROFILE;
           }
         }
         else if ( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_DELETE ) 
         {
           ret_val = MCFG_INT_DELETE_DATA_PROFILE;
         }
         else
         {
           /* Muxd-SW EFS Data profiles */
           if( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_MUXD )
           {
              ret_val = MCFG_INT_MUXD_SW_ONLY_EFS_FILE;
            }
          }
          break;

    case U_CARRIER_CONFIG_ITEM:
#ifdef FEATURE_MCFG_MULTI_MBN_SUPPORT
         if ( (mcfg_item_prefix_ptr->attrib) & U_ITEM_ATTRIB_CFG ) 
         {
           ret_val = MCFG_INT_CONFIG_ITEM;
         }
#endif/*FEATURE_MCFG_MULTI_MBN_SUPPORT*/    
          break;


      default:
          ret_val = MCFG_INT_ERROR; 
          break;
    }/* mcfg_item_prefix_ptr->item_type - attrib processing */

    /* Any sp_ops handled here */
    /* For now no special ops */

    return ret_val;
}/* mcfg_parse_item_prefix() */ 

#ifdef FEATURE_MCFG_LEGACY_CLEANUP
/*===========================================================================

FUNCTION  mcfg_extra_cleanup() 

DESCRIPTION
    Cleanup of auto-generated storage items before activation and/or after
    deactivation, policyman as example.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_extra_cleanup(uint8 cfg_type, uint8 subs_id )
{
  int32 ret = 0;
  int8 i = 0;
  struct fs_stat temp_buf;
  typedef struct {
    boolean is_dir;
    char    f_path[256];
  } cleanup_list_s_type;

  cleanup_list_s_type cleanup_array[] = 
  {
    {TRUE,  "/policyman"},
    {FALSE, "/nv/reg_files/modem/lte/rrc/csp/acq_db"}
  };

  if (cfg_type == MCFG_TYPE_SW && subs_id == 0)
  {
    for (i=0; i < sizeof(cleanup_array)/sizeof(cleanup_list_s_type); i++)
    {
      if(efs_stat(cleanup_array[i].f_path, &temp_buf) != -1) 
      {
        if (cleanup_array[i].is_dir)
        {
          ret = efs_deltree(cleanup_array[i].f_path);
        }
        else
        {
          ret = efs_unlink(cleanup_array[i].f_path);
        }

        if (ret == 0)
        {
          MCFG_MSG_SPRINTF_1("cleanup for %s", cleanup_array[i].f_path);
        }
        else
        {
          MCFG_MSG_SPRINTF_1("failed to cleanup %s", cleanup_array[i].f_path);
        }
      }
    }
  }
  
  return (ret==0);
}/* mcfg_extra_cleanup() */ 
#else
/*===========================================================================

FUNCTION  mcfg_extra_cleanup() 

DESCRIPTION
    Cleanup of auto-generated storage items before activation and/or after
    deactivation, policyman as example.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_extra_cleanup(uint8 cfg_type, uint8 subs_id )
{
  if (cfg_type == MCFG_TYPE_HW && subs_id == 0)
  {
    policyman_efs_obliterate_subs(SYS_MODEM_AS_ID_NONE);
  }
  else if (cfg_type == MCFG_TYPE_SW)
  {
    if (subs_id == 0)
    {
      policyman_efs_obliterate_subs(SYS_MODEM_AS_ID_1);
    }
    else if (subs_id == 1)
    {
      policyman_efs_obliterate_subs(SYS_MODEM_AS_ID_2);
    }
    else if (subs_id == 2)
    {
      policyman_efs_obliterate_subs(SYS_MODEM_AS_ID_3);
    }
  }

  return TRUE;
}/* mcfg_extra_cleanup() */ 
#endif

/*===========================================================================

FUNCTION  mcfg_update_required() 

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  TRUE - if NV update required
  FALSE - if NV already up-to-date  

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_update_required(uint8 cfg_type, uint8* ram_cust_data_ptr, uint32* ram_mcfg_version_ptr, uint8 subs_id )
{
  boolean	  update_required=TRUE;
  uint32    nv_mcfg_version;
  uint32    compat_check_result;

  mcfg_nv_item_hdr_type *nv_mcfg_version_hdr;
  mcfg_nv_status_e_type  nv_read_status;

  /* Extract the MCFG Version information from the ram_cust_data_ptr */
  nv_mcfg_version_hdr =  mcfg_malloc( sizeof(uint8)* MCFG_VER_NV_HEADER_LEN );
  if( nv_mcfg_version_hdr != NULL) 
  {

  memscpy(nv_mcfg_version_hdr, sizeof(mcfg_nv_item_hdr_type),
          ram_cust_data_ptr, (MCFG_VER_NV_HEADER_LEN * sizeof(uint8)) ); 

  /* Get the NV item type and length */
  if  ( ((nv_items_enum_type)nv_mcfg_version_hdr->mcfg_nv_item_type == MCFG_SW_VERSION_I ) ||
		  ((nv_items_enum_type)nv_mcfg_version_hdr->mcfg_nv_item_type == MCFG_HW_VERSION_I ) ) 
  {
    if ((nv_items_enum_type)nv_mcfg_version_hdr->mcfg_nv_item_length != MCFG_VERSION_LEN ) 
	  {
      update_required = FALSE;
      MCFG_MSG_ERROR_2("Incorrect length=%d for NV Item=%d,in the MCFG ver type",
						   nv_mcfg_version_hdr->mcfg_nv_item_length,nv_mcfg_version_hdr->mcfg_nv_item_type );
	  }
  }
  else
  {
	  update_required = FALSE;
	  MCFG_MSG_ERROR_1("Incorrect NV Item type =%d in the first MCFG segment, expected to be MCFG ver type",
	        			   nv_mcfg_version_hdr->mcfg_nv_item_type);
  }

  mcfg_free(nv_mcfg_version_hdr);

  }
  else
  {
    update_required = FALSE;
    MCFG_MSG_ERROR("MCFG: Memory allocation failure while reading MCFG version information");
  }

  if (update_required == FALSE) 
  {
	 return(update_required);
  }

  /* If ok till now, get the MCFG version from ram_cust_data_ptr and save it in ram_mcfg_version */
  ram_cust_data_ptr += MCFG_VER_NV_HEADER_LEN;
  memscpy(ram_mcfg_version_ptr, MCFG_VERSION_LEN, 
          ram_cust_data_ptr, MCFG_VERSION_LEN );

  // Compatibility checks
  compat_check_result = mcfg_compat_check(cfg_type, ram_mcfg_version_ptr); 
  if ( compat_check_result == MCFG_COMPAT_FAIL )
  {
    update_required=FALSE;
    MCFG_MSG_HIGH("MCFG Compat check fail, update_required=false !! ..");
    return(update_required);
  }

  /* Get the MCFG version from NV */
  if (cfg_type == MCFG_TYPE_SW)
  {
    nv_read_status = mcfg_nv_read( MCFG_SW_VERSION_I, 0, subs_id,  
                                  (uint8 *)&nv_mcfg_version,
                                   sizeof(nv_mcfg_version) );
  }
  else if (cfg_type == MCFG_TYPE_HW )
  {
    nv_read_status = mcfgnv_read( MCFG_HW_VERSION_I, 0,
                                  (uint8 *)&nv_mcfg_version,
                                  sizeof(nv_mcfg_version) );
  }
  else
  {
    update_required = FALSE;
    MCFG_MSG_ERROR_1("Unknown MCFG  type = 0x%x", cfg_type);
    return(update_required);
  }

  if(nv_read_status == MCFG_NV_STATUS_OK)
  {
    MCFG_MSG_HIGH_3("compare sub%d mcfg_version %lx to saved %lx", subs_id, (uint32)(*ram_mcfg_version_ptr), (uint32)nv_mcfg_version);
	  /* version comparison logic here */
    if ( (*ram_mcfg_version_ptr) == nv_mcfg_version )
	  {
       update_required=FALSE;
	  }

    MCFG_MSG_LOW_1("nv_mcfg_version %x", nv_mcfg_version);
    if( (nv_mcfg_version == MCFG_RESTORE_FACT_VALUE) && (subs_id == MCFG_SUB_ID_FIRST) )
    {
      MCFG_MSG_HIGH_1("mcfg_restore_fact_def %d", subs_id);
      mcfg_restore_fact_def = TRUE;
    }
    MCFG_MSG_LOW_1("mcfg_restore_fact_def %x", mcfg_restore_fact_def);
  }
  else if(nv_read_status == MCFG_NV_STATUS_NOT_ACTIVE)
  {
	  MCFG_MSG_HIGH("MCFG Ver not active, MCFG needs to be written once ..");
  }
  else
  {
	  MCFG_MSG_ERROR_1("NV read failed for MCFG_xx_VERSION_I, cfg_type =0x%x, ", cfg_type);
    update_required=FALSE;
  }

  MCFG_MSG_HIGH_1("update_required = %d", update_required);
  return(update_required);

} /* mcfg_update_required() */

/*===========================================================================
FUNCTION  mcfg_update_nvefs() 

DESCRIPTION
  This updates the NV-EFS with the modem config data
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  Result - Boolean (true or false depending on success / failure)

SIDE EFFECTS
  None
===========================================================================*/
boolean mcfg_update_nvefs(uint8 cfg_type, 
                          uint32 num_mcfg_items, uint8* ram_cust_data_ptr, 
                          uint8 subs_id_input, 
                          mcfg_refresh_type_e_type refresh_type)
{
  nv_items_enum_type nv_mcfg_cust_item;
  uint16  nv_mcfg_cust_item_length;

  mcfg_item_prefix_type    mcfg_item_prefix;
  uint8    mcfg_item_type_int;
  uint32         i;
  mcfg_nv_item_hdr_type *mcfg_nv_item_data_hdr;
  nv_item_type *mcfg_nv_item_data_buf;
  nv_item_type *temp_read_buf;
  mcfg_nv_status_e_type    mcfg_nv_status;

  uint16  mcfg_prl_version;
  nv_ram_roaming_list_type  *prl_buf_ptr;

  boolean mcfg_update_nv_item_flag = TRUE;
  boolean mcfg_nvefs_update_status = FALSE;
  boolean delete_nv_flag = TRUE;
  uint8 item_index = 0;
  uint8 subs_id;

  MCFG_MSG_MED_2("In mcfg_update_nv_efs, num_mcfg_items = %d subs_id = %d", (int)num_mcfg_items, (int)subs_id_input);

  /* Starts from 1 as the first element is MCFG_VERSION,
     ram_cust_data_ptr points to the start of MCFG Item after version */
  for (i = 1; i<num_mcfg_items; i++) 
  {
    /* Parse the MCFG Item Prefix for further processing */
    mcfg_item_type_int = 
      mcfg_parse_item_prefix(cfg_type, ram_cust_data_ptr, &mcfg_item_prefix, refresh_type, subs_id_input);

    subs_id = mcfg_get_curr_subs_id();
    //MCFG_MSG_MED_2("In mcfg_update_nv_efs, index = %d subs_id = %d", (int)i, (int)subs_id);

    /* Set PTR to the start of the MCFG item data after prefix */
    ram_cust_data_ptr += sizeof(mcfg_item_prefix);

    if( (mcfg_restore_fact_def == TRUE) && (!(mcfg_item_prefix.attrib & U_ITEM_ATTRIB_REST_FACT)) )
    {
      /* For Reset to Factory default case, make sure the U_ITEM_ATTRIB_REST_FACT attrib flag 
      is set before processing this item */
      mcfg_item_type_int = MCFG_INT_SW_ONLY;
    }

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
    if( (subs_id != MCFG_SUB_ID_FIRST) && ( !(mcfg_item_prefix.attrib & U_ITEM_ATTRIB_MULTISIM) ))
    {
      /* For Secondary and other subscriptions only process the multiSIM NVs from the config */
      mcfg_item_type_int = MCFG_INT_SW_ONLY;
    }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

    //MCFG_MSG_MED_3("In mcfg_update_nv_efs, index = %d subs_id = %d mcfg_item_type_int=%d", (int)i, (int)subs_id, mcfg_item_type_int);
    switch (mcfg_item_type_int) 
    {
      /* Carrier Specific NV Item, Static Non-Muxd */
      case MCFG_INT_NV_ITEM:
        MCFG_MSG_LOW_2("Encountered nv item type %d index %d", 
                       mcfg_item_type_int, i);
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
          
        MCFG_MSG_LOW_1("Encountered nv item %d", nv_mcfg_cust_item);
        mcfg_nv_item_data_buf =  mcfg_malloc( sizeof(uint8)* nv_mcfg_cust_item_length); 
        if( mcfg_nv_item_data_buf == NULL) 
        {
          mcfg_free(mcfg_nv_item_data_hdr);
          MCFG_MSG_ERROR("MCFG: Memory allocation failure while reading MCFG_INT_NV_ITEM");
          break;
        }

        /* Process the WRITE_ONCE attrib */ 
        /* Check for write once and set the  mcfg_update_nv_item_flag */
        /* if write_once attrib is set - update only if rest fact default */
        /* Note - Write-Once attrib only applies to Primary subs, i.e subs=0 */
        if( (mcfg_item_prefix.attrib & U_ITEM_ATTRIB_WRITE_ONCE) &&
            (mcfg_restore_fact_def == FALSE) && (subs_id == MCFG_SUB_ID_FIRST )) 
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

        if(mcfg_update_nv_item_flag)
        {
          memscpy(mcfg_nv_item_data_buf, 
                  (nv_mcfg_cust_item_length*sizeof(uint8)),
                  ram_cust_data_ptr, 
                  (nv_mcfg_cust_item_length*sizeof(uint8)) );
#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT           
          mcfg_nv_status = mcfgnv_remove_symlink_if_present( nv_mcfg_cust_item,
                                         mcfg_item_prefix.attrib,   
                                         (uint8 *)mcfg_nv_item_data_buf,
                                         nv_mcfg_cust_item_length,
                                         FALSE );
#endif           
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

        /* Free the MCFG nv hdr and buf */
        mcfg_free(mcfg_nv_item_data_hdr);
        mcfg_free(mcfg_nv_item_data_buf);

        /* point to the next MCFG item to be written */
        /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
        ram_cust_data_ptr += nv_mcfg_cust_item_length;
        break;

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
      /* Carrier Specific NV Item Muxd items */
      case MCFG_INT_MUXD_NV_ITEM:

        MCFG_MSG_LOW_2("Encountered muxd nv item %d index %d", 
                       mcfg_item_type_int, i);
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
          
        MCFG_MSG_LOW_1("Encountered nv item %d", nv_mcfg_cust_item);
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
          MCFG_MSG_LOW_1("Symlink updated successfully item= %d", nv_mcfg_cust_item);
 
          /* Process the WRITE_ONCE attrib */ 
          /* Check for write once and set the  mcfg_update_nv_item_flag */
          /* if write_once attrib is set - update only if rest fact default */
          if( (mcfg_item_prefix.attrib & U_ITEM_ATTRIB_WRITE_ONCE) &&
              (mcfg_restore_fact_def == FALSE) && (subs_id == MCFG_SUB_ID_FIRST )) 
          {
             MCFG_MSG_LOW_1("WRITE_ONCE item= %d", nv_mcfg_cust_item);
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
            MCFG_MSG_LOW_1("WRITE_ONCE mcfg_update_nv_item_flag= %d", mcfg_update_nv_item_flag);
           
            mcfg_free(temp_read_buf);
          }
          else
          {    
            mcfg_update_nv_item_flag = TRUE;
          }
  
          /* Process the MUXD attrib */ 
          /* MCFG Multiplexed NV Items, update only when Mfg version != Muxd Mcfg version */
          if( (mcfg_ver_compare_result == MCFG_VER_EQUALS_MUXD_VERSION ) &&  
              (mcfg_restore_fact_def == FALSE)  && (mcfg_update_nv_item_flag == TRUE)
             )
          {
            MCFG_MSG_HIGH_1("Since Mfg version=Muxd Mcfg version, not updating muxd item= %d", nv_mcfg_cust_item);
            mcfg_update_nv_item_flag = FALSE;
          }
          else
          {
            MCFG_MSG_LOW_1("Multiplexed item= %d update TRUE", nv_mcfg_cust_item);
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
  
          MCFG_MSG_LOW_1("Finally write mcfg_update_nv_item_flag= %d", mcfg_update_nv_item_flag);
          /* Finally write the Muxd NV Item if the mcfg_update_nv_item_flag is set */  
          if(mcfg_update_nv_item_flag)
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
        /* Free the MCFG nv hdr and buf */
        mcfg_free(mcfg_nv_item_data_hdr);
        mcfg_free(mcfg_nv_item_data_buf);

        /* point to the next MCFG item to be written */
        /* i.e advance the ram_cust_data_ptr by the nv item length in this case */
        ram_cust_data_ptr += nv_mcfg_cust_item_length;
        break;
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */

        case MCFG_INT_DELETE_NV_ITEM:
          MCFG_MSG_LOW_2("Encountered delete nv item type %d index %d", 
                         mcfg_item_type_int, i);
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

          MCFG_MSG_LOW_1("Encountered nv item %d", nv_mcfg_cust_item);
          /* If this MCFG item is a legacy or MCFG NV item... */
          if( ( nv_mcfg_cust_item < NV_MAX_I ) ||
              ( (nv_mcfg_cust_item > (nv_items_enum_type) MCFG_NVI_ITEM_FILES_START) && 
                (nv_mcfg_cust_item < (nv_items_enum_type) MCFG_NVI_ITEM_FILES_END) )
            )
          {
            mcfg_nv_status_e_type mcfg_nv_del_status;
            uint8 index = 0;
            uint8* dataSubID_ptr;

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
                MCFG_MSG_HIGH_3("MultiSIM NV delete,mcfgnv_update_nvefs - ignoring delete for eItem = %d, subs_mask=%d, curr_subs_id=%d", 
                          (int) nv_mcfg_cust_item, subs_mask, subs_id);
                delete_nv_flag = FALSE;  
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
            if(delete_nv_flag)
            { 
              mcfg_nv_del_status = mcfgnv_delete(nv_mcfg_cust_item, mcfg_item_prefix.attrib, index, subs_id);
              if (mcfg_nv_del_status != MCFG_NV_STATUS_OK)
              {
                MCFG_MSG_ERROR_2("MCFG NV delete failed, item=%d, err=%d.",
                               nv_mcfg_cust_item, mcfg_nv_del_status );
              }
            }
          }
          else
          {
            MCFG_MSG_ERROR_1( "Unknown MCFG NV Item for delete, item=%d", nv_mcfg_cust_item);
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
          MCFG_MSG_LOW_2("Encountered EFS_FILE type %d index %d", 
                         mcfg_item_type_int, i);
          if(!mcfg_create_efs_file(ram_cust_data_ptr, mcfg_item_prefix.attrib, mcfg_restore_fact_def))
          {
            mcfg_nvefs_update_status = FALSE;
          }

          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;
        case MCFG_INT_DELETE_EFS_FILE:
          MCFG_MSG_HIGH("Encountered delete EFS File");
          if(MCFG_NV_STATUS_OK != mcfg_delete_efs_file(ram_cust_data_ptr, mcfg_item_prefix.attrib))
          {
            mcfg_nvefs_update_status = FALSE;
          }

          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

          /* ========= Data Profile ========== */
        case MCFG_INT_DATA_PROFILE:
        case MCFG_INT_STATIC_WO_DATA_PROFILE:
        case MCFG_INT_MUXED_DATA_PROFILE:
          MCFG_MSG_LOW_2("Encountered data profile type %d index %d", 
                         mcfg_item_type_int, i);
          if(!mcfg_create_data_profile(ram_cust_data_ptr, mcfg_item_prefix.attrib, mcfg_restore_fact_def))
          {
            mcfg_nvefs_update_status = FALSE;
          }
 
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

        case MCFG_INT_DELETE_DATA_PROFILE:
          MCFG_MSG_LOW_2("Encountered delete Profile type %d index %d", 
                         mcfg_item_type_int, i);
          if(MCFG_NV_STATUS_OK != mcfg_delete_data_profile(ram_cust_data_ptr, mcfg_item_prefix.attrib))
          {
            mcfg_nvefs_update_status = FALSE;
          }
 
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

        case MCFG_INT_MUXD_SW_ONLY:
          MCFG_MSG_LOW_2("Encountered MUXD_SW_ONLY type %d index %d", 
                         mcfg_item_type_int, i);
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

           /* Update the symlink for the Muxd NV Items */
          mcfg_nv_status = mcfg_nv_update_symlink(nv_mcfg_cust_item, item_index, subs_id, nv_mcfg_cust_item_length);

          if(mcfg_nv_status != MCFG_NV_STATUS_OK )
          {
            MCFG_MSG_ERROR_2("Unable to create symlink for Muxd SW NV Item %d, status=%d", 
                            nv_mcfg_cust_item, mcfg_nv_status);
          }

          /* Free the MCFG NV hdr */
          mcfg_free(mcfg_nv_item_data_hdr);

          /* Advance to the next MCFG item */
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

        case MCFG_INT_SW_ONLY:
          MCFG_MSG_LOW_2("Encountered MCFG_INT_SW_ONLY type %d index %d", 
                         mcfg_item_type_int, i);
          /* Advance to the next MCFG item */
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

        case MCFG_INT_MUXD_SW_ONLY_EFS_FILE:
          MCFG_MSG_LOW_2("Encountered MUXD_SW_ONLY_EFS_FILE type %d index %d", 
                         mcfg_item_type_int, i);
          /* TODO - not supported for now, advance to the next MCFG item */
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

        default:
          /* Unknown - Err fatal should never be here */
          MCFG_ERR_FATAL("MCFG Parsing Error %d", mcfg_item_type_int, 0, 0); 
          break;
      }
    }

  return mcfg_nvefs_update_status;

} /* mcfg_update_nvefs() */

static boolean mcfg_should_modify_efs_file(char* path, uint8 attributes, boolean restore_fact)
{
  struct fs_stat temp_buf;
  char abs_path[MCFG_FILE_PATH_LENGTH];
  int bytes_written;
  uint8 carrier_index = mcfg_get_muxd_index(); 

  if(restore_fact && (attributes & U_ITEM_ATTRIB_REST_FACT))
  {
    return TRUE;
  }

  if(attributes & U_ITEM_ATTRIB_WRITE_ONCE)
  {
    if(attributes & U_ITEM_ATTRIB_MUXD)
    {
      // build muxd path,
      bytes_written = snprintf(abs_path, sizeof(abs_path), "%s_S%u", path, carrier_index);

      if(bytes_written < 0 || bytes_written >= sizeof(abs_path))
      {
        MCFG_MSG_HIGH("EFS File Path too long");
        return FALSE;
      }

      // check if linked-to path is there
      if(efs_stat(abs_path, &temp_buf) == -1)
      {
        return TRUE;
      }
    }

    // check if normal path is there
    return efs_stat(path, &temp_buf) == -1;
  }

  return TRUE;
}

boolean mcfg_verify_efs_item_contents(mcfg_tlv* header, mcfg_tlv* body)
{
  // TLV1 ( 0x0001, File path Length, File path )
  if(header->hdr.mcfg_nv_item_type != MCFG_EFS_ITEM_TYPE_FILE_PATH)
  {
    return FALSE;
  }

  // TLV2 ( 0x0002, Data Length, Data )
  if(body->hdr.mcfg_nv_item_type != MCFG_EFS_ITEM_TYPE_FILE_DATA)
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION  mcfg_create_efs_file() 

DESCRIPTION
  This creates/updates the EFS item file and uses attribs for deciding on the
  multi-SIM NV items
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  Result - Boolean (true or false depending on success / failure)

SIDE EFFECTS
  None
===========================================================================*/
boolean mcfg_create_efs_file(uint8* efsData, uint8 attributes, boolean restore_fact)
{
  char* filePath = 0;
  uint8* fileData = 0;
  uint8  subs_id = 0;
  mcfg_tlv efs_item_header;
  mcfg_tlv efs_item_body;
  mcfg_nv_status_e_type mcfg_result; 
  int efs_result;
  char f_name[MCFG_FILE_PATH_LENGTH];

  // read filename data
  mcfg_read_tlv(&efs_item_header, efsData);
  // read file contents data
  mcfg_read_tlv(&efs_item_body, efsData + efs_item_header.hdr.mcfg_nv_item_length + MCFG_TL_SIZE_BYTES);

  if(!mcfg_verify_efs_item_contents(&efs_item_header, &efs_item_body))
  {
    MCFG_MSG_ERROR("EFS File item format incorrect");
    return FALSE;
  }

  fileData = efs_item_body.value;
  filePath = (char *)efs_item_header.value;

  if(!mcfg_should_modify_efs_file(filePath, attributes, restore_fact))
  {
    MCFG_MSG_HIGH_1("No need to modify EFS File: %s, skipping...", filePath);
    return TRUE;
  }

  memscpy(f_name, MCFG_FILE_PATH_LENGTH, filePath, efs_item_header.hdr.mcfg_nv_item_length); 
/* No use cases for Indexed EFS Item for now */
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
  uint8  subs_mask = 0;
  subs_id = mcfg_get_curr_subs_id();
  if(attributes & U_ITEM_ATTRIB_MULTISIM)
  {
    subs_mask = *fileData;
    if(subs_mask > MCFG_SUBS_MASK)
    {
       mcfg_result = MCFG_NV_STATUS_BAD_ITEM;
       MCFG_MSG_ERROR_3("MultiSIM: mcfg_create_file - MCFG_NV_STATUS_BAD_ITEM, subs_id: 0x%0x, subs_mask=%d!!, filepath %s", 
                            subs_id, subs_mask, filePath);
       return FALSE;
    }

    if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
    {
       MCFG_MSG_HIGH_3("MultiSIM: mcfg_create_file - ignoring write for file = %s, subs_mask=%d, curr_subs_id=%d", 
                        filePath, subs_mask, subs_id);
       return FALSE;
    }

    // Update the f_name for subs_id > 0  
    if(subs_id)
    {
      // Note the efs_item_header.hdr.mcfg_nv_item_length includes the NULL     
      (void)snprintf((f_name + (efs_item_header.hdr.mcfg_nv_item_length-1) ), 
                      MCFG_FILE_PATH_LENGTH - efs_item_header.hdr.mcfg_nv_item_length,
                      "_Subscription%02d", (int)subs_id);
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
      MCFG_MSG_ERROR_1("Could not update symlink for EFS File: %s", filePath);
      return FALSE;
    }

    if( (mcfg_ver_compare_result == MCFG_VER_EQUALS_MUXD_VERSION ) &&  
        (mcfg_restore_fact_def == FALSE) 
       )
    {
      MCFG_MSG_SPRINTF_1("Since Mfg version=Muxd Mcfg version, not updating muxd efs %s", f_name);
      return TRUE;
    }
  }

  // create file, write data
  // MCFG_MSG_HIGH_3("mcfg_create_efs_file, EFS File: %s, length=%d, subs_id=%d", 
  //                f_name, efs_item_body.hdr.mcfg_nv_item_length, subs_id );
  efs_result = efs_put(f_name, fileData, efs_item_body.hdr.mcfg_nv_item_length, O_CREAT | O_AUTODIR, ALLPERMS);

  if(efs_result == -1)
  {
    MCFG_MSG_ERROR_1("Could not write EFS File: %s", f_name);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION  mcfg_delete_efs_file() 

DESCRIPTION
  This creates/updates the EFS item file and uses attribs for deciding on the
  multi-SIM NV items
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  Result - Boolean (true or false depending on success / failure)

SIDE EFFECTS
  None
===========================================================================*/
mcfg_nv_status_e_type mcfg_delete_efs_file(uint8* efsData, uint8 attrib )
{
  char* filePath = 0;
  uint8* fileData = 0;
  mcfg_tlv efs_item_header;
  mcfg_tlv efs_item_body;

  struct fs_stat temp_buf;
  char f_name[MCFG_FILE_PATH_LENGTH]; 
  mcfg_nv_status_e_type mcfg_nv_status = MCFG_NV_STATUS_OK;
  int bytesRead = 0;
  boolean delete_flag = TRUE;
  char* file_path;
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
  uint8  subs_id = 0;
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

     // read filename data
  mcfg_read_tlv(&efs_item_header, efsData);
  // read file contents data
  mcfg_read_tlv(&efs_item_body, efsData + efs_item_header.hdr.mcfg_nv_item_length + MCFG_TL_SIZE_BYTES);

  if(!mcfg_verify_efs_item_contents(&efs_item_header, &efs_item_body))
  {
    MCFG_MSG_ERROR("EFS File item format incorrect");
    return FALSE;
  }

  fileData = efs_item_body.value;
  filePath = (char *)efs_item_header.value;

  file_path = mcfg_malloc( sizeof(uint8)* MCFG_FILE_PATH_LENGTH );
  if(file_path)
  {
    memscpy(file_path, MCFG_FILE_PATH_LENGTH, filePath, efs_item_header.hdr.mcfg_nv_item_length); 
  }
  else
  {
    MCFG_MSG_ERROR("mcfg_delete_efs_file - malloc error");
    return FALSE;
  }

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
  uint8  subs_mask = 0;
  subs_id = mcfg_get_curr_subs_id();
  if(attrib & U_ITEM_ATTRIB_MULTISIM)
  {
    subs_mask = *fileData;
    if(subs_mask > MCFG_SUBS_MASK)
    {
       mcfg_nv_status = MCFG_NV_STATUS_BAD_ITEM;
       MCFG_MSG_ERROR_3("MultiSIM: mcfg_delete_efs_file - MCFG_NV_STATUS_BAD_ITEM, subs_id: 0x%0x, subs_mask=%d!!, filepath %s", 
                           subs_id, subs_mask, file_path);
       delete_flag = FALSE;
    }

    if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
    {
       MCFG_MSG_HIGH_3("MultiSIM: mcfg_delete_efs_file - ignoring delete for file = %s, subs_mask=%d, curr_subs_id=%d", 
                        file_path, subs_mask, subs_id);
       delete_flag = FALSE;
    }

    // Update the f_name
    if(subs_id)
    {
      // Note the efs_item_header.hdr.mcfg_nv_item_length includes the NULL
      (void)snprintf((file_path + (efs_item_header.hdr.mcfg_nv_item_length-1)), 
                      MCFG_FILE_PATH_LENGTH - efs_item_header.hdr.mcfg_nv_item_length,
                      "_Subscription%02d", (int)subs_id);
    }
    
    // First byte in case of multi-SIM EFS items is the subs_id 
    /* Decrement the size by 1 to account for the subs_id */
    fileData = (void *) (fileData + sizeof(uint8));
    efs_item_body.hdr.mcfg_nv_item_length--;
  }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

  file_path[MCFG_FILE_PATH_LENGTH-1] = '\0';

  if(delete_flag)
  {
    //MCFG_MSG_HIGH_1("Deleting NV File: %s", file_path);

    if(efs_lstat((char *)file_path, &temp_buf) != -1) 
    {
#ifndef FEATURE_MODEM_CFG_MUX_SUPPORT
      if(S_ISLNK(temp_buf.st_mode))
      {
        bytesRead = efs_readlink((char *)file_path, f_name, sizeof(f_name) - 1);

        if(bytesRead == -1)
        {
          mcfg_nv_status = MCFG_NV_STATUS_EFS_ERR; 
          MCFG_MSG_HIGH("Read link failed");
        }
        else
        {
          // efs_readlink does not NUll terminate
          f_name[bytesRead] = '\0';

          if(efs_unlink(f_name) != 0)
          {
            mcfg_nv_status = MCFG_NV_STATUS_EFS_ERR; 
            MCFG_MSG_HIGH("Deletion of linked to file failed");
          }
        }
      } /* S_ISLINK() */
#endif

      if(efs_unlink((char *)file_path) != 0)
      {
        mcfg_nv_status = MCFG_NV_STATUS_EFS_ERR; 
        MCFG_MSG_HIGH("Deletion failed");
      }
    }/* efs_lstat */
  }/* delete_flag */

  mcfg_free(file_path);
  return mcfg_nv_status;
}

/*===========================================================================
FUNCTION  mcfg_get_profile_path() 

DESCRIPTION
  This gets data profile file path for specified subs_id
  
DEPENDENCIES
  

RETURN VALUE
  Result - Boolean (true or false depending on success / failure)

SIDE EFFECTS
  None
===========================================================================*/
boolean mcfg_get_profile_path(char* profile_path, uint8 path_len,
                                     char* fullpath, uint8 subs_id)
{
  uint8 path_offset = 0;

  (void)snprintf(fullpath, MCFG_FILE_PATH_LENGTH, "/Data_Profiles/Subs%d/", (int)subs_id+1);
  path_offset = strlen(fullpath);
  MCFG_ASSERT_COND_RET_FALSE((path_offset+path_len) < MCFG_FILE_PATH_LENGTH);
  memscpy(fullpath + path_offset, MCFG_FILE_PATH_LENGTH - path_offset, profile_path, path_len); 

  MCFG_MSG_SPRINTF_1("profile path %s", fullpath);

  return TRUE;
}

/*===========================================================================
FUNCTION  mcfg_create_data_profile() 

DESCRIPTION
  This creates/updates the EFS item file and uses attribs for deciding on the
  multi-SIM NV items
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  Result - Boolean (true or false depending on success / failure)

SIDE EFFECTS
  None
===========================================================================*/
boolean mcfg_create_data_profile(uint8* efsData, uint8 attributes, boolean restore_fact)
{
  char* filePath = 0;
  uint8* fileData = 0;
  uint8  subs_id = 0;
  mcfg_tlv efs_item_header;
  mcfg_tlv efs_item_body;
  mcfg_nv_status_e_type mcfg_result; 
  int efs_result;
  char f_name[MCFG_FILE_PATH_LENGTH];

  // read filename data
  mcfg_read_tlv(&efs_item_header, efsData);
  // read file contents data
  mcfg_read_tlv(&efs_item_body, efsData + efs_item_header.hdr.mcfg_nv_item_length + MCFG_TL_SIZE_BYTES);

  if(!mcfg_verify_efs_item_contents(&efs_item_header, &efs_item_body))
  {
    MCFG_MSG_ERROR("EFS File item format incorrect");
    return FALSE;
  }

  fileData = efs_item_body.value;
  filePath = (char *)efs_item_header.value;

  if(!mcfg_should_modify_efs_file(filePath, attributes, restore_fact))
  {
    MCFG_MSG_HIGH_1("No need to modify EFS File: %s, skipping...", filePath);
    return TRUE;
  }

/* No use cases for Indexed EFS Item for now */
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
  uint8  subs_mask = 0;
  subs_id = mcfg_get_curr_subs_id();
  if(attributes & U_ITEM_ATTRIB_MULTISIM)
  {
    subs_mask = *fileData;
    if(subs_mask > MCFG_SUBS_MASK)
    {
       mcfg_result = MCFG_NV_STATUS_BAD_ITEM;
       MCFG_MSG_ERROR_3("MultiSIM: mcfg_create_file - MCFG_NV_STATUS_BAD_ITEM, subs_id: 0x%0x, subs_mask=%d!!, filepath %s", 
                            subs_id, subs_mask, filePath);
       return FALSE;
    }

    if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
    {
       MCFG_MSG_HIGH_3("MultiSIM: mcfg_create_file - ignoring write for file = %s, subs_mask=%d, curr_subs_id=%d", 
                        filePath, subs_mask, subs_id);
       return FALSE;
    }

    if(!mcfg_get_profile_path(
        filePath, efs_item_header.hdr.mcfg_nv_item_length, f_name, subs_id))
    {
      return FALSE;
    }

    // First byte in case of multi-SIM EFS items is the subs_mask 
    /* Decrement the size by 1 to account for the subs_mask */
    fileData = (void *) (fileData + sizeof(uint8));
    efs_item_body.hdr.mcfg_nv_item_length--;
  }
  else
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */
  {
    if(!mcfg_get_profile_path(
        filePath, efs_item_header.hdr.mcfg_nv_item_length, f_name, subs_id))
    {
      return FALSE;
    }
  }
  f_name[MCFG_FILE_PATH_LENGTH-1] = '\0';

  // link to file if necessary
  if(attributes & U_ITEM_ATTRIB_MUXD)
  {
    mcfg_result = mcfg_nv_efs_update_symlink(f_name, efs_item_body.hdr.mcfg_nv_item_length);

    if(mcfg_result != MCFG_NV_STATUS_OK)
    {
      MCFG_MSG_ERROR_1("Could not update symlink for EFS File: %s", filePath);
      return FALSE;
    }
  }

  // create file, write data
  MCFG_MSG_LOW_2("mcfg_create_data_profile, length=%d, subs_id=%d", 
                  efs_item_body.hdr.mcfg_nv_item_length, subs_id );
  efs_result = efs_put(f_name, fileData, efs_item_body.hdr.mcfg_nv_item_length, O_CREAT | O_AUTODIR, ALLPERMS);

  if(efs_result == -1)
  {
    MCFG_MSG_ERROR_1("Could not write EFS File: %s", f_name);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION  mcfg_delete_data_profile() 

DESCRIPTION
  This creates/updates the EFS item file and uses attribs for deciding on the
  multi-SIM NV items
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  Result - Boolean (true or false depending on success / failure)

SIDE EFFECTS
  None
===========================================================================*/
mcfg_nv_status_e_type mcfg_delete_data_profile(uint8* efsData, uint8 attrib )
{
  char* filePath = 0;
  uint8* fileData = 0;
  mcfg_tlv efs_item_header;
  mcfg_tlv efs_item_body;

  struct fs_stat temp_buf;
  char f_name[MCFG_FILE_PATH_LENGTH]; 
  mcfg_nv_status_e_type mcfg_nv_status = MCFG_NV_STATUS_OK;
  int bytesRead = 0;
  boolean delete_flag = TRUE;
  char* file_path;
  uint8  subs_id = 0;

     // read filename data
  mcfg_read_tlv(&efs_item_header, efsData);
  // read file contents data
  mcfg_read_tlv(&efs_item_body, efsData + efs_item_header.hdr.mcfg_nv_item_length + MCFG_TL_SIZE_BYTES);

  if(!mcfg_verify_efs_item_contents(&efs_item_header, &efs_item_body))
  {
    MCFG_MSG_ERROR("EFS File item format incorrect");
    return FALSE;
  }

  fileData = efs_item_body.value;
  filePath = (char *)efs_item_header.value;

  file_path = mcfg_malloc( sizeof(uint8)* MCFG_FILE_PATH_LENGTH );
  if(!file_path)
  {
    MCFG_MSG_ERROR("mcfg_delete_efs_file - malloc error");
    return FALSE;
  }

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
  uint8  subs_mask = 0;
  subs_id = mcfg_get_curr_subs_id();
  if(attrib & U_ITEM_ATTRIB_MULTISIM)
  {
    subs_mask = *fileData;
    if(subs_mask > MCFG_SUBS_MASK)
    {
       mcfg_nv_status = MCFG_NV_STATUS_BAD_ITEM;
       MCFG_MSG_ERROR_3("MultiSIM: mcfg_delete_efs_file - MCFG_NV_STATUS_BAD_ITEM, subs_id: 0x%0x, subs_mask=%d!!, filepath %s", 
                           subs_id, subs_mask, file_path);
       delete_flag = FALSE;
    }

    if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
    {
       MCFG_MSG_HIGH_3("MultiSIM: mcfg_delete_efs_file - ignoring delete for file = %s, subs_mask=%d, curr_subs_id=%d", 
                        file_path, subs_mask, subs_id);
       delete_flag = FALSE;
    }

    if(!mcfg_get_profile_path(
        (char *)efs_item_header.value, efs_item_header.hdr.mcfg_nv_item_length, file_path, subs_id))
    {
      delete_flag = FALSE;
    }
    
    // First byte in case of multi-SIM EFS items is the subs_id 
    /* Decrement the size by 1 to account for the subs_id */
    fileData = (void *) (fileData + sizeof(uint8));
    efs_item_body.hdr.mcfg_nv_item_length--;
  }
  else
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */
  {
    if(!mcfg_get_profile_path(
        (char *)efs_item_header.value, efs_item_header.hdr.mcfg_nv_item_length, file_path, subs_id))
    {
      delete_flag = FALSE;
    }
  }

  file_path[MCFG_FILE_PATH_LENGTH-1] = '\0';

  if(delete_flag)
  {
    //MCFG_MSG_HIGH_1("Deleting NV File: %s", file_path);

    if(efs_lstat((char *)file_path, &temp_buf) != -1) 
    {
      if(S_ISLNK(temp_buf.st_mode))
      {
        bytesRead = efs_readlink((char *)file_path, f_name, sizeof(f_name) - 1);

        if(bytesRead == -1)
        {
          mcfg_nv_status = MCFG_NV_STATUS_EFS_ERR; 
          MCFG_MSG_HIGH("Read link failed");
        }
        else
        {
          // efs_readlink does not NUll terminate
          f_name[bytesRead] = '\0';

          if(efs_unlink(f_name) != 0)
          {
            mcfg_nv_status = MCFG_NV_STATUS_EFS_ERR; 
            MCFG_MSG_HIGH("Deletion of linked to file failed");
          }
        }
      } /* S_ISLINK() */

      if(efs_unlink((char *)file_path) != 0)
      {
        mcfg_nv_status = MCFG_NV_STATUS_EFS_ERR; 
        MCFG_MSG_HIGH("Deletion failed");
      }
    }/* efs_lstat */
  }/* delete_flag */

  mcfg_free(file_path);
  return mcfg_nv_status;
}

boolean mcfg_read_tlv(mcfg_tlv* dest, uint8* src)
{
  boolean success = TRUE; // here for future use in case we want to indicate errors

  memscpy(&(dest->hdr.mcfg_nv_item_type), sizeof(uint16), src, sizeof(uint16));
  src += sizeof(uint16);
  memscpy(&(dest->hdr.mcfg_nv_item_length), sizeof(uint16), src, sizeof(uint16));
  src += sizeof(uint16);
  dest->value = src;

  return success;
}

/*===========================================================================

FUNCTION  mcfgnv_read() 

DESCRIPTION
  Reads the MCFG NV Item, looks up attrib for indexed NV items
  Does not look up and act on Attrib for MULTI-SIM items involving subs_mask
 
DEPENDENCIES

RETURN VALUE
  TRUE on success, FALSE on failure

SIDE EFFECTS
  None

===========================================================================*/
mcfg_nv_status_e_type mcfgnv_read( uint32 eItem, uint8 attrib, uint8* data_ptr, uint32 size)
{
  mcfg_nv_status_e_type mcfg_nv_status=MCFG_NV_STATUS_OK;
  uint8                 index=0;
  uint8                 subs_mask=0;
  uint8                 subs_id=0;

  // For PRL
  word               item_size;   /* Size of item (in bytes) */
  int                fd;
  char               f_name[128]; /* Increase size from 120 to 128 for context*/
  struct fs_stat     temp_buf;
  nv_ram_roaming_list_type *prl_ptr;
  char               *prl_read_buf;
  fs_size_t          nbytes;
  fs_size_t          bytes_read;
  boolean            nam_flag;
  fs_ssize_t         result = 0;
       
#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
  uint8 u8CarrierIndex = 0;
#endif       
 
  MCFG_MSG_LOW_1("mcfgnv_read eItem %d", eItem);
  if ( (eItem < NV_MAX_I ) || 
       ((eItem > MCFG_NVI_ITEM_FILES_START) && 
        (eItem < MCFG_NVI_ITEM_FILES_END))
     )
  {
    if( eItem == NV_ROAMING_LIST_683_I )
    {   
      /* PRL case */
      /* No use case for MultiSIM PRLs i.e C+C yet */
      prl_ptr = (nv_ram_roaming_list_type *)data_ptr;
      if(prl_ptr->nam >= NV_MAX_NAMS)
      {
        return MCFG_NV_STATUS_BAD_ITEM;
      }

      item_size = NV_ROAMING_LIST_MAX_SIZE + sizeof(nv_roaming_list_header_type);

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
      if (attrib & U_ITEM_ATTRIB_MUXD )
      {
         u8CarrierIndex = mcfg_get_muxd_index(); 
      /* Use context 0 for now, change it to /nvm/context%d/prl_%d in future if reqd */   
        (void)snprintf(f_name, sizeof(f_name), "/nvm/prl_%d_S%d",
                                   prl_ptr->nam, u8CarrierIndex );
      }
      else 
#endif   
      {
        (void)snprintf(f_name, sizeof(f_name), "/nvm/prl_%d",
                                  prl_ptr->nam);
      }
      
      /* Check if the file is present or not. */
      if(efs_stat(f_name,&temp_buf) == -1) 
      {
        return MCFG_NV_STATUS_NOT_ACTIVE;
      }

      if(temp_buf.st_size <= 1)
      {
        /* In case the symlink is created for a dummy file */
        return MCFG_NV_STATUS_NOT_ACTIVE;
      }

      fd = efs_open(f_name, O_RDWR, 0777 );

      nbytes = item_size;
      bytes_read = 0;
      prl_read_buf = (void *)&(prl_ptr->roaming_list);
      prl_read_buf -= (sizeof(prl_ptr->prl_version) + sizeof(prl_ptr->size) 
                       + sizeof(prl_ptr->valid));

      efs_lseek(fd,
            (sizeof(nam_flag) + sizeof(uint8)),
            SEEK_CUR);

      while (bytes_read < nbytes)
      {
        result = efs_read (fd,
                           (void *) (prl_read_buf + bytes_read),
                           nbytes - bytes_read);
        if (result <= 0)
          break;
        bytes_read += (fs_size_t) result;
      }

      efs_close(fd);

      if(result==-1) 
        mcfg_nv_status = MCFG_NV_STATUS_EFS_ERR;
      else 
        mcfg_nv_status = MCFG_NV_STATUS_OK;
    }
    else
    {
      /* non-PRL case */

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
      if (attrib &  U_ITEM_ATTRIB_MULTISIM)
      {
        /* ...then, get the subs_id from the MCFG data. */
        subs_mask = *data_ptr;

        MCFG_MSG_HIGH_3("MultiSIM: mcfgnv_read - MCFG Item %d, subs_mask: 0x%02x, addr: 0x%08lx", (int)eItem, subs_mask, (uint32)data_ptr);

        if(subs_mask > MCFG_SUBS_MASK)
        {
          mcfg_nv_status = MCFG_NV_STATUS_BAD_ITEM;
          MCFG_MSG_ERROR_2("MultiSIM: mcfgnv_read - MCFG Item %d, failed status: 0x%0x", (int)eItem, mcfg_nv_status);
          return mcfg_nv_status;
        }

        // For read, no need for mcfg_proc_subs_mask, just get the subs_id
        subs_id = mcfg_get_curr_subs_id();

        /* For MultiSIM items in MCFG the first byte is subs_mask.
           This index is not stored in EFS, so adjust the pointer and size for old style NV Items.
           For new style MCFG NV Items this is handled in the MCFG NV layer */
        data_ptr = (void *) ((int) data_ptr + sizeof(uint8));
        size--;
      }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

      if (attrib &  U_ITEM_ATTRIB_INDEXED)
      {
        /* ...then, get the index from the MCFG data. */
        index = *data_ptr;

        MCFG_MSG_HIGH_3("Indexed: mcfgnv_read - MCFG Item %d, index: 0x%02x, addr: 0x%08lx", (int)eItem, index, (uint32)data_ptr);
        /* For Indexed items in MCFG the first byte is index.
           For MultiSIM+Indexed items in MCFG the second byte is the index
           This index is not stored in EFS, so adjust the pointer and size for old style NV Items.
           For new style MCFG NV Items this is handled in the MCFG NV layer */
        if (eItem < NV_MAX_I )
        {
          data_ptr = (void *) ((int) data_ptr + sizeof(uint8));
          size--;
        }
      }

      mcfg_nv_status = mcfg_nv_read(eItem, index, subs_id, data_ptr, size);
    }
  }
  else
  {
    MCFG_MSG_ERROR_1("Unknown MCFG Item %d", (int)eItem);
    mcfg_nv_status = MCFG_NV_STATUS_BAD_ITEM;
  }

  MCFG_MSG_LOW_1("mcfgnv_read status %d", mcfg_nv_status);

  return mcfg_nv_status;
} /* mcfgnv_read() */

/*===========================================================================

FUNCTION  mcfgnv_write() 

DESCRIPTION
  Reads the MCFG NV Item, looks up attrib for indexed NV items
  and MULTI-SIM items
 
DEPENDENCIES

RETURN VALUE
  TRUE on success, FALSE on failure

SIDE EFFECTS
  None

===========================================================================*/
mcfg_nv_status_e_type mcfgnv_write( uint32 eItem, uint8 attrib, uint8* data_ptr, uint32 size)
{
  fs_size_t          nbytes;
  fs_size_t          bytes_written;
  nv_ram_roaming_list_type *prl_ptr;
  uint8*       prl_write_buf; 
  uint8 index=0;
  uint8 subs_mask=0;
  uint8 subs_id=0;
  uint8* read_ptr = NULL;
  mcfg_nv_status_e_type mcfg_nv_status=MCFG_NV_STATUS_OK;
  boolean  write_flag   = TRUE;

  // For PRL manipulation
  word               item_size; 
  int                fd;
  char               f_name[128];
  fs_ssize_t         result = 0;
  boolean            true_flag   = TRUE;
       
#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
  uint8 u8CarrierIndex = 0;
#endif
       
  if ( (eItem < NV_MAX_I ) || 
       ((eItem > MCFG_NVI_ITEM_FILES_START) && 
        (eItem < MCFG_NVI_ITEM_FILES_END))
     )
  {
    if( eItem == NV_ROAMING_LIST_683_I )
    {   
      /* PRL case */
      /* No use case for MultiSIM PRLs i.e C+C yet */
      /* Alternate option : Use legacy NV CMD to update the PRL
         Note : Legacy NV maintains the PRL version data that needs to be updated
         after writing PRL as well
         Policy is to avoid NV calls as much as possible for performance reasons */

      prl_ptr = (nv_ram_roaming_list_type *)data_ptr;
      if(prl_ptr->nam >= NV_MAX_NAMS)
      {
        return MCFG_NV_STATUS_BAD_ITEM;
      }

      item_size = NV_ROAMING_LIST_MAX_SIZE + sizeof(nv_roaming_list_header_type);
   
#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
      if (attrib & U_ITEM_ATTRIB_MUXD )
      {
        u8CarrierIndex = mcfg_get_muxd_index(); 
        (void)snprintf(f_name, sizeof(f_name), "/nvm/prl_%d_S%d",
                                   prl_ptr->nam, u8CarrierIndex);
      }
      else 
#endif   
      {
        (void)snprintf(f_name, sizeof(f_name), "/nvm/prl_%d",
                                  prl_ptr->nam);
      } 
       
      fd = efs_open(f_name, O_RDWR | O_CREAT , 0777 );

      /* nbytes should be one less than item_size as the write_buf is pointing to 
       * prl_version which is second element of the prl data structure and NAM 
       * is not supposed to be updated */
      nbytes = item_size - 1;
      bytes_written = 0;
       
      prl_write_buf = (void *)&(prl_ptr->prl_version);

      efs_write ( fd,
                  (const void *) &(true_flag),
                  sizeof(true_flag));

      efs_lseek(fd, sizeof(true_flag), SEEK_CUR);
      MCFG_MSG_HIGH_3("fd = 0x%0x, nbytes=%d, prl_version=%d",fd, (int)nbytes, prl_ptr->prl_version );
      while (bytes_written < nbytes) 
      {
        result = efs_write ( fd,
                           (const void *) (prl_write_buf + bytes_written),
                           nbytes - bytes_written);
        if (result <= 0)
        {
            
          MCFG_MSG_ERROR_2("efs_write fail error for prl item=%d, efs_err = %d", (int)eItem, efs_errno);
          break;
        }
        bytes_written += (fs_size_t) result;
      } 
       
      if(result==-1) 
        mcfg_nv_status = MCFG_NV_STATUS_EFS_ERR;
      else 
        mcfg_nv_status = MCFG_NV_STATUS_OK;

    }
    else
    {
      /* non-PRL case : Regular NV Items (legacy or MCFG) */
      /* Check if the attrib indicates this is an indexed item */


      /* Read the item first and update only if changed */
      if(attrib & U_ITEM_ATTRIB_UPDATE_ONLY )
      {
        read_ptr = mcfg_malloc(size);
        if(read_ptr)
        {
          mcfg_nv_status = mcfgnv_read(eItem, attrib, read_ptr, size);

          if(mcfg_nv_status == MCFG_NV_STATUS_OK)
          {
            /* If size is not the same, the read would return MCFG_NV_STATUS_BAD_SIZE */
            /* Now compare the two memory regions, and decide whether to write */
            if( memcmp(data_ptr, read_ptr, size) == 0 )
            {
              write_flag = FALSE;  
            }
          }

          mcfg_free(read_ptr);
        }
      }

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
      if (attrib & U_ITEM_ATTRIB_MULTISIM)
      {
        /* ...then, get the subs_id from the MCFG data. */
        subs_mask = *data_ptr;

        if(subs_mask > MCFG_SUBS_MASK)
        {
          mcfg_nv_status = MCFG_NV_STATUS_BAD_ITEM;
          MCFG_MSG_ERROR_3("MultiSIM: mcfgnv_write - MCFG Item %d, failed status: 0x%0x, subs_mask=%d!!", 
                           (int)eItem, mcfg_nv_status, subs_id);
          return mcfg_nv_status;
        }

        subs_id = mcfg_get_curr_subs_id();

        if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
        {
          MCFG_MSG_HIGH_3("MultiSIMV write- ignoring write for eItem = %lu, subs_mask=%d, curr_subs_id=%d", 
                          eItem, subs_mask, subs_id);
          write_flag = FALSE;  
        }

        // First byte in case of multi-SIM NV items is the subs_mask 
        /* Decrement the size by 1 to account for the subs_mask */
        data_ptr = (void *) ((int) data_ptr + sizeof(uint8));
        size--;
        
      }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

      /* Indexed or arrayed item are required to have a uint8 index field 
      as the first byte of their struct */
      /* For MultiSIM indexed items, the second byte is the index field */
      if (attrib &  U_ITEM_ATTRIB_INDEXED)
      {
        /* ...then, get the index from the MCFG data. */
        index = *data_ptr;
        if (eItem < NV_MAX_I )
        {
          // First byte in case of indexed items is the index 
          /* Decrement the size by 1 to account for the index here for old style NV Items and advance the data ptr to
             point to the data to be written */
          /* For new style MCFG NV Items, this is handled byt the mcfg_nv module */
          data_ptr = (void *) ((int) data_ptr + sizeof(uint8));
          size--;
        }
      }
      
      if(write_flag)
      {
        //MCFG_MSG_HIGH_3("Calling MCFG Item write for eItem = %d, subs_id=%d, size=%d", eItem, subs_id, size);
        mcfg_nv_status = mcfg_nv_write(eItem, index, subs_id, data_ptr, size);
        if(mcfg_nv_status != MCFG_NV_STATUS_OK)
        {
          MCFG_MSG_ERROR_2("MCFG Item %d, write failed status: 0x%0x", (int)eItem, mcfg_nv_status);
        }
      }
       
    }/* non-PRL case*/
  }
  else
  {
    MCFG_MSG_ERROR_1("Unknown MCFG Item %d", (int)eItem);
    mcfg_nv_status = MCFG_NV_STATUS_BAD_ITEM;
  }

  return mcfg_nv_status;

}/* mcfgnv_write() */

/*===========================================================================

FUNCTION  mcfgnv_delete()

DESCRIPTION
   Delete the NV Item from MCFG 

DEPENDENCIES

RETURN VALUE
  TRUE / FALSE 
   
SIDE EFFECTS
  None

===========================================================================*/
mcfg_nv_status_e_type mcfgnv_delete(uint32 eItem, uint8 attrib,  byte u8Index, uint8 subs_id)
{
  mcfg_nv_status_e_type  mcfg_nv_status;
  char f_name[MCFG_FILE_PATH_LENGTH];
  uint32 path_offset, path_len;
  struct fs_stat    temp_buf;
#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
  byte              u8CarrierIndex;
#endif

  path_offset=0;
  path_len = sizeof(f_name);
  MCFG_ASSERT_COND_RET_FALSE(subs_id < MCFG_NUM_OF_SUB_ID);

  if (eItem == NV_PREF_MODE_I )
  {
    MCFG_MSG_HIGH("Skip deleting NV 10 Mode Pref");
    return MCFG_NV_STATUS_OK;
  }

  if (eItem < NV_MAX_I )
  {
    /* Create the name of the item file */
#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
    if (attrib & U_ITEM_ATTRIB_MUXD )
    {
      /* Multiplexed Items */
      u8CarrierIndex = mcfg_get_muxd_index();
      /* A: PRL case */
      if(eItem == NV_ROAMING_LIST_683_I)
      {
        /* PRL case */
        (void)snprintf(f_name, sizeof(f_name), "/nvm/prl_%d_S%d",
                                  u8Index, u8CarrierIndex);
      }
      else
      {
        /* B: non-PRL case */
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
        if (subs_id != 0 )
        {
          path_offset  = strlcpy(f_name, "/nvm/context", path_len);
          path_offset += snprintf(&f_name[path_offset], path_len - path_offset,
                                  "%d/", (int)subs_id);
        }
        else
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */
        {
          path_offset  = strlcpy(f_name, "/nvm/num/", path_len);
        }

        // Only indexes > 0 are part of NV filename
        if (0 != u8Index) 
        {
             /* Multiplexed - indexed items */
            path_offset += snprintf(&f_name[path_offset], path_len - path_offset, "%d_%d_S%d",
                            (int)eItem, (int)u8Index, (int)u8CarrierIndex);
        }
        else
        {
            /* Multiplexed non indexed items */
            path_offset += snprintf(&f_name[path_offset], path_len - path_offset, "%d_S%d",
                           (int)eItem, (int)u8CarrierIndex);
        }
      } /* non-PRL case*/
     }
     else                                                   
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */
     {
       if(eItem == NV_ROAMING_LIST_683_I)
       {
         /* PRL case */
         (void)snprintf(f_name, sizeof(f_name), "/nvm/prl_%d",
                        u8Index);
        }
        else
        {
          /* Non-multiplexed items */
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
          if (subs_id != 0 )
          {
            path_offset  = strlcpy(f_name, "/nvm/context", path_len);
            path_offset += snprintf(&f_name[path_offset], path_len - path_offset,
                                  "%d/", (int)subs_id);
          }
          else
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */
          {
            path_offset  = strlcpy(f_name, "/nvm/num/", path_len);
          }

          // Only indexes > 0 are part of NV filename
          if (0 != u8Index)
          {
            /* Non-Multiplexed, non-zero arrayed items */
            path_offset += snprintf(&f_name[path_offset], path_len - path_offset,"%d_%d",
                                    (int)eItem,(int)u8Index); 
          }
          else
          {
            /* Non-Multiplexed, non-arrayed items, or arrayed items with index 0 */
            path_offset += snprintf(&f_name[path_offset], path_len - path_offset,"%d",(int)eItem);
          }
        }
     }
   
     /* Delete the NV Item !! */
     if(efs_stat(f_name,&temp_buf) != -1) 
     {
       // If present delete the file here !!
       if ( efs_unlink(f_name) != 0)
          mcfg_nv_status = MCFG_NV_STATUS_EFS_ERR; 
     }
   
     mcfg_nv_status = MCFG_NV_STATUS_OK;
  }
  else if ( (eItem > MCFG_NVI_ITEM_FILES_START) && 
            (eItem < MCFG_NVI_ITEM_FILES_END) )
  {
    /* New style MCFG Items , no mux support yet */
    mcfg_nv_status = mcfg_nv_delete(eItem, u8Index, subs_id);
  }
  else
  {
    MCFG_MSG_ERROR_1("Unknown MCFG Item %d", (int)eItem);
    mcfg_nv_status = MCFG_NV_STATUS_BAD_ITEM;
  }

  return mcfg_nv_status;
}/* mcfgnv_delete() */ 


#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
/*===========================================================================

FUNCTION  mcfg_compare_w_muxd_version() 

DESCRIPTION

DEPENDENCIES

RETURN VALUE
 
 
SIDE EFFECTS
  None

===========================================================================*/
static uint8 mcfg_compare_w_muxd_version(uint8* ram_cust_data_ptr, uint8 subs_id )
{
  uint8 mcfg_compare_w_muxd_version_result = MCFG_VER_GREATER_MUXD_VERSION;
  uint32    local_mcfg_info;
  uint32    mcfg_version;
  mcfg_nv_status_e_type    nv_read_status;
  uint8 carrier_index;

  /* Get the MCFG version information from RAM */
  ram_cust_data_ptr += (MCFG_HEADER_LEN + MCFG_VER_NV_HEADER_LEN);
  memscpy(&mcfg_version, sizeof(mcfg_version), 
          ram_cust_data_ptr, MCFG_VERSION_LEN );

  carrier_index = mcfg_get_muxd_index();
  MCFG_MSG_LOW_2("mcfg_compare_w_muxd_version subs_id=%d carrier_index=%d", subs_id, carrier_index);
  /* Get the Muxd MCFG info from NV */
  nv_read_status = (mcfg_nv_status_e_type)mcfg_nv_read( MCFG_NV_ITEM_MCFG_SW_MUXD_VERSION_I, 
                                                        carrier_index, subs_id, 
                                                        (uint8 *)&local_mcfg_info, 
                                                        sizeof(local_mcfg_info));

  MCFG_MSG_LOW_1("mcfg_compare_w_muxd_version nv_read_status=%d", nv_read_status);
  if(  nv_read_status == MCFG_NV_STATUS_OK )
  {
    MCFG_MSG_LOW_1("mcfg_compare_w_muxd_version mcfg_version=%x", mcfg_version);
    MCFG_MSG_LOW_1("mcfg_compare_w_muxd_version local_mcfg_info=%x", local_mcfg_info);
    if( mcfg_version  == local_mcfg_info )
    {
      mcfg_compare_w_muxd_version_result = MCFG_VER_EQUALS_MUXD_VERSION;
    }
    else
    {
      if (mcfg_version < local_mcfg_info ) 
      {
        mcfg_compare_w_muxd_version_result = MCFG_VER_LESS_MUXD_VERSION;
      }
    }
  }
  else if(nv_read_status == MCFG_NV_STATUS_NOT_ACTIVE)
  {
    MCFG_MSG_MED("MCFG Mux'd Info needs to be written once ..");
  }
  else
  {
    MCFG_MSG_ERROR_1( "MCFG Muxd NV read failed, item=%d", nv_read_status);
  }

  return(mcfg_compare_w_muxd_version_result);

}/* mcfg_compare_w_muxd_version() */

/*===========================================================================

FUNCTION  mcfg_set_muxd_index() 

DESCRIPTION
  Sets the Muxd Carrier index and akey index from the mcfg hdr information 

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void mcfg_set_muxd_index(uint16 mcfg_muxd_carrier_index)
{

  muxd_carrier_index = (uint8)((mcfg_muxd_carrier_index) & 0xFF);
  
  /* Also write Carrier Index NV Item for NV internal usage, if mandated */ 

}/* mcfg_set_muxd_index */

/*===========================================================================

FUNCTION  mcfg_get_muxd_index() 

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  Returns the Muxd Index (used by NVIO layer) 

SIDE EFFECTS
  None

===========================================================================*/
uint8 mcfg_get_muxd_index()
{
   return(muxd_carrier_index);
}/* mcfg_get_muxd_index */
 

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT

/*===========================================================================

FUNCTION  mcfg_set_curr_subs_id() 

DESCRIPTION
  Sets the Muxd Carrier index and akey index from the mcfg hdr information 

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcfg_set_curr_subs_id(uint8 subs_id)
{

  mcfg_curr_subs_id = subs_id;

}/* mcfg_set_curr_subs_id() */

/*===========================================================================

FUNCTION  mcfg_get_curr_subs_id() 

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  Returns the Muxd Index (used by NVIO layer) 

SIDE EFFECTS
  None

===========================================================================*/
uint8 mcfg_get_curr_subs_id(void)
{
   return(mcfg_curr_subs_id);
}/* mcfg_get_muxd_index */


/*===========================================================================

FUNCTION  mcfg_proc_subs_mask() 

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  Returns TRUE/FALSE based on the subs_mask bit is set for subs_id

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_proc_subs_mask(uint8 subs_mask, uint8 curr_subs_id)
{

  boolean allowed_flag = FALSE;

  // subs_id is a bitmaask
  // Each bit is a slot num when set to 1, implies write allowed for that slot
  if( subs_mask & (1 << curr_subs_id) )
  {
    allowed_flag = TRUE; 
  }

  return allowed_flag;
}

#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

/*===========================================================================

FUNCTION  mcfgnv_update_symlink() 

DESCRIPTION
  Creates the symlink for Muxd NV Items
 
DEPENDENCIES

RETURN VALUE
  TRUE on success, FALSE on failure

SIDE EFFECTS
  None

===========================================================================*/
mcfg_nv_status_e_type mcfgnv_update_symlink( uint32 eItem, uint8 attrib, uint8* data_ptr, uint32 item_size)
{
  mcfg_nv_status_e_type mcfg_nv_status=MCFG_NV_STATUS_OK;
  uint8                 index=0;
  uint8                 subs_id=0;
    // For PRL manipulation
  char               f_name[128];
  nv_ram_roaming_list_type *prl_ptr;

  MCFG_MSG_LOW_1("mcfgnv_update_symlink - MCFG Item %d", (int)eItem);
  if ( ((eItem < NV_MAX_I ) || 
         ((eItem > MCFG_NVI_ITEM_FILES_START) && (eItem < MCFG_NVI_ITEM_FILES_END))) &&
        (attrib & U_ITEM_ATTRIB_MUXD) )
  {
    if( eItem == NV_ROAMING_LIST_683_I )
    {   
      /* PRL case */
      prl_ptr = (nv_ram_roaming_list_type *)data_ptr;
      if(prl_ptr->nam >= NV_MAX_NAMS)
      {
        return MCFG_NV_STATUS_BAD_ITEM;
      }

      (void)snprintf(f_name, sizeof(f_name), "/nvm/prl_%d",
                                  prl_ptr->nam);

      /* For PRL create EFS file, hence use 128 */
      mcfg_nv_status = mcfg_nv_efs_update_symlink(f_name, 128);

    }
    else
    {
      /* non-PRL case */
#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
      /* This code reached if there is a Muxd-MultiSIM NV required */
      uint8 subs_mask = 0;
      subs_id = mcfg_get_curr_subs_id();
      if (attrib & U_ITEM_ATTRIB_MULTISIM)
      {
        /* ...then, get the subs_id from the MCFG data. */
        subs_mask = *data_ptr;
        if(subs_mask > MCFG_SUBS_MASK)
        {
          mcfg_nv_status = MCFG_NV_STATUS_BAD_ITEM;
          MCFG_MSG_ERROR_3("MultiSIM: mcfgnv_update_symlink - MCFG_NV_STATUS_BAD_ITEM, MCFG Item %d subs_id:%d , subs_mask=%d!! ", 
                              (int)eItem, subs_id, subs_mask );
          return mcfg_nv_status;
        }

        if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
        {
           MCFG_MSG_HIGH_3("MultiSIM: mcfgnv_update_symlink - ignoring update_symlink for MCFG item %d, subs_mask=%d, curr_subs_id=%d", 
                              (int)eItem, subs_mask, subs_id);
           return mcfg_nv_status;
        }

        // First byte in case of multi-SIM NV items is the subs_id 
        /* Decrement the size by 1 to account for the subs_id */
        data_ptr = (void *) ((int) data_ptr + sizeof(uint8));
        item_size--;
      }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

      if (attrib &  U_ITEM_ATTRIB_INDEXED)
      {
        /* ...then, get the index from the MCFG data. */
        /* For indexed NV items first byte i.e index is not saved in EFS but just used to form the pathname */
        index = *data_ptr;
        item_size--;
        data_ptr = (void *) ((int) data_ptr + sizeof(uint8));
      }

      MCFG_MSG_LOW_3("mcfgnv_update_symlink-mcfg_nv_update_symlink eitem%d index %d sub id %d", 
                      (int)eItem, index, subs_id);
      mcfg_nv_status = mcfg_nv_update_symlink(eItem, index, subs_id, item_size);
    }
  }
  else
  {
    MCFG_MSG_ERROR_1("mcfgnv_update_symlink - Unknown MCFG Item %d", (int)eItem);
    mcfg_nv_status = MCFG_NV_STATUS_BAD_ITEM;
  }

  return mcfg_nv_status;
}

/*===========================================================================

FUNCTION  mcfgnv_remove_symlink_if_present() 

DESCRIPTION
  Creates the symlink for Muxd NV Items
 
DEPENDENCIES

RETURN VALUE
  TRUE on success, FALSE on failure

SIDE EFFECTS
  None

===========================================================================*/
mcfg_nv_status_e_type mcfgnv_remove_symlink_if_present(uint32 eItem, 
                                                              uint8 attrib, 
                                                              uint8* data_ptr, 
                                                              uint32 item_size, 
                                                              boolean force)
{
  mcfg_nv_status_e_type mcfg_nv_status=MCFG_NV_STATUS_OK;
  uint8 index=0;
  uint8 subs_id=0;

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
  uint8 subs_mask=0;
  subs_id = mcfg_get_curr_subs_id();
  if ( (attrib & U_ITEM_ATTRIB_MULTISIM) && (eItem != NV_ROAMING_LIST_683_I) ) 
  {
     /* ...then, get the subs_id from the MCFG data. */
     subs_mask = *data_ptr;

     if(subs_mask > MCFG_SUBS_MASK)
     {
       mcfg_nv_status = MCFG_NV_STATUS_BAD_ITEM;
       MCFG_MSG_ERROR_3("MultiSIM: mcfgnv_remove_symlink_if_present - MCFG_NV_STATUS_BAD_ITEM, MCFG Item %d subs_id: 0x%0x, subs_mask=%d!! ", 
                           (int)eItem, subs_id, subs_mask );
       return mcfg_nv_status;
     }

     if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
     {
        MCFG_MSG_HIGH_3("MultiSIM: mcfgnv_remove_symlink_if_present - ignoring update_symlink for MCFG item %d, subs_mask=%d, curr_subs_id=%d", 
                         (int)eItem, subs_mask, subs_id);
        return mcfg_nv_status;
     }

     // First byte in case of multi-SIM NV items is the index 
     /* Decrement the size by 1 to account for the subs_id */
     data_ptr = (void *) ((int) data_ptr + sizeof(uint8));
     item_size--;
  }
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

  if (attrib &  U_ITEM_ATTRIB_INDEXED)
  {
    /* ...then, get the index from the MCFG data. */
    index = *data_ptr;
    if (eItem < NV_MAX_I )
    {
      // First byte in case of indexed items is the index 
      /* Decrement the size by 1 to account for the index here for old style NV Items and advance the data ptr to
         point to the data to be written */
      /* For new style MCFG NV Items, this is handled byt the mcfg_nv module */
      data_ptr = (void *) ((int) data_ptr + sizeof(uint8));
      item_size--;
    }
  }

  /* Skip removing MUXD symlink unless it's forced in cleanup process.  */
  if (!force && (attrib & U_ITEM_ATTRIB_MUXD))
  {
    // Do not call mcfg_nv_rm_symlink for muxd items
    MCFG_MSG_LOW_1("mcfgnv_remove_symlink_if_present: eItem %d is muxd, skip mcfg_nv_rm_symlink", eItem);
  }
  else
  {
    mcfg_nv_status = mcfg_nv_rm_symlink(eItem, index, subs_id);
  }

  return mcfg_nv_status;
}

#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */

/*===========================================================================
                                  
FUNCTION  mcfg_compat_check() 

DESCRIPTION
  Compatibility check for the downloaded MCFG

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 mcfg_compat_check(uint8 cfg_type, uint32* ram_mcfg_ver_ptr)
{
  uint32        compat_checks_result = MCFG_COMPAT_FAIL;
  uint8         system_type;
  uint8         compat_ver;
  uint16        carrier_version; 

  // Compatibility version checks
  compat_ver = (*ram_mcfg_ver_ptr) >> MCFG_SW_VER_COMPAT_VER_BITS;
  switch (cfg_type)
  {
    case MCFG_TYPE_SW:
      if (compat_ver == MCFG_SW_COMPAT_VERSION)
      {
        compat_checks_result = MCFG_COMPAT_PASS;
      }
      break;

    case MCFG_TYPE_HW:
      if (compat_ver == MCFG_HW_COMPAT_VERSION)
      {
        compat_checks_result = MCFG_COMPAT_PASS;
      }
      break;

     default:
      MCFG_MSG_ERROR_1( "Unkown MCFG type in mcfg_compat_check cfg_type=%d", cfg_type);
      break;

  }

  if (compat_checks_result == MCFG_COMPAT_FAIL)
  {
    return compat_checks_result;
  }

  // System type, carrier version  checks
  if(cfg_type == MCFG_TYPE_SW )
  {
     system_type = (((*ram_mcfg_ver_ptr) >> MCFG_SW_VER_CARRIER_BITS) & MCFG_SW_VER_SYS_TYPE_MASK ) ;

     /* TODO :
        Other checks like C_BIT needs to be set etc. need to go here, omitted for first implementation */

     /* For example :
     #ifdef FEATURE_CDMA1X
      if (! (system_type & (0x01 >> MCFG_SYS_C_BIT) ) )
         return MCFG_COMPAT_FAIL;
     #endif
     */
     /* Other checks for SYS bits can go here */

     carrier_version = ((*ram_mcfg_ver_ptr) & MCFG_SW_VER_CARRIER_MAJOR_MASK) >> MCFG_SW_VER_CARRIER_MINOR_VER_BITS;  

     MCFG_MSG_HIGH_2( "MCFG SW- carrier major version = 0x%0x sys_type= 0x%0x", carrier_version, system_type);

     /* More carrier specific checks can be added here */
     /* For example :

  #ifdef FEATURE_CDMA1X

    #ifdef FEATURE_GOBI_VZW
    if (carrier_version != MCFG_CARRIER_MAJ_VERSION_VZW) 
    {
      return MCFG_COMPAT_FAIL;
    }
    #endif
 */

  }
  // If reach here then everything's ok 
  return MCFG_COMPAT_PASS;
} /* mcfg_compat_check() */


#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
/*===========================================================================

FUNCTION  mcfg_update_history_log()

DESCRIPTION
  Logs successfully loaded hw and sw configs to a file. Keeps track of carriers

DEPENDENCIES

RETURN VALUE
  boolean - true if update was successful, false otherwise 
   
SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_update_history_log(uint8 cfg_type, uint32 ram_mcfg_version)
{
  char* mcfg_log_path;
  int       file_handle = NULL;
  mcfg_log_type*	mcfg_log_data_ptr;
  fs_size_t full_size=0;
  boolean result = TRUE;
  char* log_paths[MCFG_NUM_OF_TYPES];
  log_paths[MCFG_TYPE_SW] = "/mcfg_log_sw.bin";
  log_paths[MCFG_TYPE_HW] = "/mcfg_log_hw.bin";

  // check if config type is valid
  if(cfg_type != MCFG_TYPE_SW && cfg_type != MCFG_TYPE_HW) {
    MCFG_MSG_ERROR_1("Cannot log invalid config type: %d", cfg_type);
    return FALSE;
  }

  // get handle to corresponding log file
  mcfg_log_path = log_paths[cfg_type];
  file_handle = efs_open(mcfg_log_path, O_CREAT | O_RDWR, S_IWRITE | S_IREAD); 

  // make sure file handle is valid
  if(file_handle < 0) {
    MCFG_MSG_ERROR("File handle invalid, not logging");
    return FALSE;
  }

  full_size = mcfg_read_log_info(file_handle, &mcfg_log_data_ptr);

  if(full_size != -1)
  {
    full_size = mcfg_update_log_info(mcfg_log_data_ptr, ram_mcfg_version, full_size);

    if(full_size != -1)
    {
      mcfg_write_log_info(file_handle, mcfg_log_data_ptr, full_size);
    }
    else
  {   
      MCFG_MSG_ERROR("Log update failed");
      result = FALSE;
    }
  }
  else
  {
    MCFG_MSG_ERROR("Log read failed");
    result = FALSE;
  }

  mcfg_free(mcfg_log_data_ptr);
  efs_close(file_handle);

  return result;
  }

/*===========================================================================
                                  
FUNCTION  mcfg_read_log_info() 

DESCRIPTION
  Reads previously existing log data into *mcfg_log_data_ptr or allocates new space

DEPENDENCIES 
  None 

RETURN VALUE 
  -1 on failure, size of log file if successful

SIDE EFFECTS
  None

===========================================================================*/
static fs_size_t mcfg_read_log_info(int file_handle, mcfg_log_type**	mcfg_log_data_ptr)
  {
  struct fs_stat mcfg_log_file_stat;
  fs_size_t full_size=0;
  fs_size_t pos=0;
  int bytes_read;

  if(efs_fstat(file_handle, &mcfg_log_file_stat) < 0) {
    MCFG_MSG_ERROR("mcfg_read_log_info: efs_fstat returned negative value");
    return -1;
  }

  /* Read the data from the log file if not empty */
  if(mcfg_log_file_stat.st_size != 0 ) 
  {
    full_size = mcfg_log_file_stat.st_size;
    *mcfg_log_data_ptr =  mcfg_malloc(full_size + sizeof(uint32));

    /* It is permitted for efs_read to return fewer bytes than were requested,
     * even if the data is available in the file, so make sure to read out all the data ! */
    while (pos < full_size) 
    {
      bytes_read = efs_read(file_handle, (void *)((uint8 *)(*mcfg_log_data_ptr) + pos), full_size - pos);
      if (bytes_read > 0) 
      {
        if (bytes_read == 0)
          break;
          pos += (fs_size_t) bytes_read;
      } 
      else 
      {
        MCFG_MSG_ERROR("Unable to read from MCFG-sw log file, fs-error");
        return -1;
      }
    }
  }
  else
  {
    /* Empty file - create the records */
    full_size = sizeof(mcfg_log_type); 
    *mcfg_log_data_ptr =  mcfg_malloc(full_size + sizeof(uint32));
    memset(*mcfg_log_data_ptr, 0, sizeof(mcfg_log_type));
    (*mcfg_log_data_ptr)->mcfg_log_magic = (uint32)MCFG_MAGIC;
  }

  return full_size;
  }

/*===========================================================================
                                  
FUNCTION  mcfg_update_log_info() 

DESCRIPTION
  Updates log data based on carrier and history index

DEPENDENCIES 
  None 

RETURN VALUE 
  true on success, false otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_update_log_info(mcfg_log_type*	mcfg_log_data_ptr, uint32 ram_mcfg_version, fs_size_t full_size)
{
  mcfg_log_carrier_info_type  carrier_info_data;
  uint8 history_index, i ,carrier_info_count;
  boolean b_log_carrier_info_update_reqd = FALSE;
  uint16 log_file_carrier_version;
  uint8 carrier_index = mcfg_get_muxd_index(); 

  /* PROCESS / UPDATE the log file data */                                                                              
  carrier_info_data.carrier_maj_version = (ram_mcfg_version & MCFG_SW_VER_CARRIER_MAJOR_MASK) >> MCFG_SW_VER_CARRIER_MINOR_VER_BITS; 
  carrier_info_data.muxd_carrier_index = muxd_carrier_index;

  history_index = mcfg_log_data_ptr->mcfg_log_history_index;
  carrier_info_count = mcfg_log_data_ptr->mcfg_log_carrier_info_count;

  /* Validate the log file data and max bounds */
  if( (mcfg_log_data_ptr->mcfg_log_magic != MCFG_MAGIC ) || 
      (carrier_info_count > MCFG_LOG_CARRIER_INFO_MAX) ||
      (history_index > MCFG_LOG_HIST_MAX) )
  {
    MCFG_MSG_ERROR("Corrupted log file mcfg_log_file, bailing out!");
    full_size = -1;
    return full_size;
    // should exit after this
  }

  /* Update log history and index */
  memscpy( ((uint8 *)mcfg_log_data_ptr + MCFG_LOG_HIST_OFFSET + history_index*sizeof(uint32)), 
           sizeof(uint32),
           &ram_mcfg_version, 
           sizeof(uint32) );

  history_index = (history_index + 1) % MCFG_LOG_HIST_MAX;
  mcfg_log_data_ptr->mcfg_log_history_index = history_index;

  /* Find if carrier info already has an entry for the carrier */
  if( carrier_info_count > 0 ) 
  {
    b_log_carrier_info_update_reqd = TRUE;
    for (i=0; i<carrier_info_count; i++) 
    {
      memscpy(&log_file_carrier_version, sizeof(log_file_carrier_version),
              ((uint8 *) mcfg_log_data_ptr+ MCFG_LOG_CARR_INFO_OFFSET + i*sizeof(uint32)), 
              sizeof(log_file_carrier_version) );
      if(carrier_info_data.carrier_maj_version == log_file_carrier_version)
      {
        b_log_carrier_info_update_reqd=FALSE;
        break;
      }
    }
  }
  else
  {
    b_log_carrier_info_update_reqd = TRUE;
  }

  if(b_log_carrier_info_update_reqd) 
  {
    full_size += sizeof(uint32);
    memscpy( ((uint8 *)mcfg_log_data_ptr+ MCFG_LOG_CARR_INFO_OFFSET + carrier_info_count*sizeof(uint32)),
            sizeof(uint32), &carrier_info_data, sizeof(uint32) );
    mcfg_log_data_ptr->mcfg_log_carrier_info_count = carrier_info_count+1;
  }
  else
  {
    mcfg_log_data_ptr->mcfg_log_carrier_info_count = carrier_info_count;
  }

  return full_size;
}

/*===========================================================================
                                  
FUNCTION  mcfg_write_log_info() 

DESCRIPTION
  Writes updated log data to file indicated by file_handle

DEPENDENCIES 
  None 

RETURN VALUE 
  true on success, false otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_write_log_info(int file_handle, mcfg_log_type*	mcfg_log_data_ptr, fs_size_t full_size)
{
  fs_ssize_t  bytes_written = 0;
  fs_ssize_t  bytes_written_temp = 0;
  fs_off_t offset;
  boolean retval = TRUE;

  /* Rewind to the beginning of the file */
  offset = efs_lseek (file_handle, 0, SEEK_SET);
  if (offset < 0) {
    MCFG_MSG_HIGH ("Cannot seek to the beginning of the mcfg log file");
    return FALSE;
  }

  /* Finally write the updated log_data_buffer back to the EFS */
  while ( bytes_written < full_size )
  {
    bytes_written_temp = efs_write(file_handle, (void *)((uint8 *)mcfg_log_data_ptr + bytes_written), 
                                   full_size - bytes_written);

    if ( bytes_written_temp < 0 )
    {
      MCFG_MSG_ERROR_3("Log Data File Write Error 0x%x",efs_errno,0,0);
      retval = FALSE;
      break;
    }

    if ( bytes_written_temp == 0 )
	  {
	    MCFG_MSG_ERROR_3("Log Data File Write Error, most likely file system full!!!",0,0,0);
      retval = FALSE;
	    break;
	  }

    bytes_written += bytes_written_temp;
  }

  return retval;
}
#endif /* FEATURE_MODEM_CFG_HISTORY_LOGGING */

/*===========================================================================
FUNCTION  mcfg_cleanup_nvefs() 

DESCRIPTION
  This cleans up the NV-EFS based on the list of active modem config data
 
DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  Result - Boolean (true or false depending on success / failure)

SIDE EFFECTS
  None
===========================================================================*/
boolean mcfg_cleanup_nvefs
(
   uint8 cfg_type, 
   uint32 num_mcfg_items, 
   uint8* ram_cust_data_ptr,
   uint8  subs_id_input
)
{
  nv_items_enum_type nv_mcfg_cust_item;
  uint16 nv_mcfg_cust_item_length;

  mcfg_item_prefix_type mcfg_item_prefix;
  uint8  mcfg_item_type_int;
  uint32 i;
  uint8 item_index = 0;
  mcfg_nv_item_hdr_type *mcfg_nv_item_data_hdr;
  nv_item_type *mcfg_nv_item_data_buf;
  mcfg_nv_status_e_type mcfg_nv_status;

  boolean mcfg_nvefs_update_status = FALSE;
  boolean delete_item_flag = TRUE;
  uint8  subs_id;

  MCFG_MSG_HIGH_2("In mcfg_cleanup_nvefs, num_mcfg_items = %d sub%d", 
                  (int)num_mcfg_items, subs_id_input);

  /* Starts from 1 as the first element is MCFG_VERSION,
     ram_cust_data_ptr points to the start of MCFG Item after version */
  for (i = 1; i<num_mcfg_items; i++) 
  {
    /* Parse the MCFG Item Prefix for further processing */
    mcfg_item_type_int = mcfg_parse_item_prefix(
       cfg_type, ram_cust_data_ptr, &mcfg_item_prefix, MCFG_REFRESH_TYPE_SLOT_N_SUBS, subs_id_input);

    subs_id = mcfg_get_curr_subs_id();

    /* Set PTR to the start of the MCFG item data after prefix */
    ram_cust_data_ptr += sizeof(mcfg_item_prefix);

    if( (subs_id != MCFG_SUB_ID_FIRST) && ( !(mcfg_item_prefix.attrib & U_ITEM_ATTRIB_MULTISIM)) )
    {
      /* For Secondary and other subscriptions only cleanup the multiSIM NVs from the config */
      mcfg_item_type_int = MCFG_INT_SW_ONLY;
    }

    if (mcfg_item_prefix.attrib & U_ITEM_ATTRIB_WRITE_ONCE)
    {
      MCFG_MSG_MED_2("ignore %d with writeone attri %x", i, mcfg_item_prefix.attrib);
      mcfg_item_type_int = MCFG_INT_SW_ONLY;
    }

    switch (mcfg_item_type_int) 
    {
      /* Carrier Specific NV Item, Static Non-Muxd */
      case MCFG_INT_NV_ITEM:
       case MCFG_INT_DELETE_NV_ITEM:
          delete_item_flag = TRUE;
          /* Get the hdr */
          mcfg_nv_item_data_hdr =  
            mcfg_malloc( sizeof(uint8)* MCFG_VER_NV_HEADER_LEN );
          if( mcfg_nv_item_data_hdr == NULL) 
          {
            MCFG_MSG_ERROR("mcfg_cleanup_nvefs: Memory allocation failure");
            break;
          }

          memscpy(mcfg_nv_item_data_hdr, (MCFG_VER_NV_HEADER_LEN*sizeof(uint8)), 
                  ram_cust_data_ptr, (MCFG_VER_NV_HEADER_LEN*sizeof(uint8))); 

          /* Get the NV item type and length */
          nv_mcfg_cust_item = 
            (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_type;
          nv_mcfg_cust_item_length = 
            (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_length;

          ram_cust_data_ptr += MCFG_VER_NV_HEADER_LEN;

          MCFG_MSG_LOW_3("mcfg_cleanup_nvefs NV item type % index_num %d item %d", 
                          mcfg_item_type_int, i, (int)nv_mcfg_cust_item);

          /* If this MCFG item is a legacy or MCFG NV item... */
          if( ( nv_mcfg_cust_item < NV_MAX_I ) ||
              ( (nv_mcfg_cust_item > (nv_items_enum_type) MCFG_NVI_ITEM_FILES_START) && 
                (nv_mcfg_cust_item < (nv_items_enum_type) MCFG_NVI_ITEM_FILES_END) )
            )
          {
            mcfg_nv_status_e_type mcfg_nv_del_status;
            uint8 index = 0;
            uint8* dataSubID_ptr;

            dataSubID_ptr = ram_cust_data_ptr;

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
            /* If the item is MultiSIM type ... */
            uint8 subs_mask = 0;
            if (( mcfg_item_prefix.attrib & U_ITEM_ATTRIB_MULTISIM ) && (nv_mcfg_cust_item != NV_ROAMING_LIST_683_I) )
            {
              /* ...then, get the subs_mask from the MCFG data. */
              subs_mask = *(dataSubID_ptr);
              dataSubID_ptr++;

              if(subs_mask > MCFG_SUBS_MASK)
              {
                delete_item_flag = FALSE;
                mcfg_nv_del_status = MCFG_NV_STATUS_BAD_ITEM;
                MCFG_MSG_ERROR_3("MultiSIM: mcfgnv_cleanup_nvefs MCFG_NV_STATUS_BAD_ITEM - MCFG Item %d, subs_id=%d, subs_mask=%d!!", 
                           (int)nv_mcfg_cust_item, subs_id, subs_mask);
              }

              if( !mcfg_proc_subs_mask(subs_mask, subs_id) )
              {
                MCFG_MSG_HIGH_3("MultiSIM NVdelete, mcfgnv_cleanup_nvefs - ignoring delete for eItem = %d, subs_mask=%d, curr_subs_id=%d", 
                               nv_mcfg_cust_item, subs_mask, subs_id);
                delete_item_flag = FALSE;  
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
            if(delete_item_flag)
            {
              mcfg_nv_del_status = 
                mcfgnv_delete(nv_mcfg_cust_item,mcfg_item_prefix.attrib, index, subs_id);
              if (mcfg_nv_del_status != MCFG_NV_STATUS_OK)
              {
                MCFG_MSG_ERROR_2("MCFG NV delete failed, item=%d, err=%d.",
                               nv_mcfg_cust_item, mcfg_nv_del_status );
              }
            }
          }
          else
          {
            MCFG_MSG_ERROR_1( "Unknown MCFG NV Item for delete, item=%d", 
                              nv_mcfg_cust_item);
          }

          /* Free the MCFG NV hdr */
          mcfg_free(mcfg_nv_item_data_hdr);

          /* point to the next NV or MCFG item to be written i.e advance
             the ram_cust_data_ptr by the nv item length in this case */
          ram_cust_data_ptr += nv_mcfg_cust_item_length;
          break;

        case MCFG_INT_EFS_FILE:
        case MCFG_INT_STATIC_WO_EFS_FILE:
        case MCFG_INT_MUXD_EFS_FILE:
        case MCFG_INT_DELETE_EFS_FILE:
          MCFG_MSG_LOW_2("Encountered delete EFS File type %d index %d", 
                         mcfg_item_type_int, i);

          if(MCFG_NV_STATUS_OK != mcfg_delete_efs_file(ram_cust_data_ptr, mcfg_item_prefix.attrib))
          {
            mcfg_nvefs_update_status = FALSE;
          }

          ram_cust_data_ptr += 
            (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;

          /* ========= Data Profile ========== */
        case MCFG_INT_DATA_PROFILE:
        case MCFG_INT_STATIC_WO_DATA_PROFILE:
        case MCFG_INT_MUXED_DATA_PROFILE:
        case MCFG_INT_DELETE_DATA_PROFILE:
          MCFG_MSG_LOW_2("Encountered delete data profile type %d index %d", 
                         mcfg_item_type_int, i);
          if(MCFG_NV_STATUS_OK != mcfg_delete_data_profile(ram_cust_data_ptr, mcfg_item_prefix.attrib))
          {
            mcfg_nvefs_update_status = FALSE;
          }
 
          ram_cust_data_ptr += (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
          break;


#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
      case MCFG_INT_MUXD_NV_ITEM:

        /* Get the hdr */
        mcfg_nv_item_data_hdr = 
          mcfg_malloc( sizeof(uint8)* MCFG_VER_NV_HEADER_LEN );
        if( mcfg_nv_item_data_hdr == NULL) 
        {
          MCFG_MSG_ERROR("mcfg_cleanup_nvefs Memory allocation failure ");
          break;
        }

        memscpy(mcfg_nv_item_data_hdr, (MCFG_VER_NV_HEADER_LEN * sizeof(uint8)),
                ram_cust_data_ptr, (MCFG_VER_NV_HEADER_LEN * sizeof(uint8))); 

        /* Get the NV item type and length */
        nv_mcfg_cust_item = 
          (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_type;
        nv_mcfg_cust_item_length = 
          (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_length;
          
        MCFG_MSG_LOW_3("Encountered delete Muxd item item %d len %d %p",
          nv_mcfg_cust_item, nv_mcfg_cust_item_length, ram_cust_data_ptr);

        /* Get the value */
        ram_cust_data_ptr += MCFG_VER_NV_HEADER_LEN;
        mcfg_nv_item_data_buf = 
          mcfg_malloc( sizeof(uint8)* nv_mcfg_cust_item_length); 
        if( mcfg_nv_item_data_buf == NULL) 
        {
          mcfg_free(mcfg_nv_item_data_hdr); 
          MCFG_MSG_ERROR("mcfg_cleanup_nvefs Memory allocation failure ");
          break;
        }
        memscpy(mcfg_nv_item_data_buf, 
                (nv_mcfg_cust_item_length*sizeof(uint8)),
                ram_cust_data_ptr, 
                (nv_mcfg_cust_item_length*sizeof(uint8)) );

        MCFG_MSG_LOW_3("Encountered delete Muxd item type %d index %d item %d",
                         mcfg_item_type_int, i, nv_mcfg_cust_item);

        mcfg_nv_status = mcfgnv_remove_symlink_if_present( nv_mcfg_cust_item,
                                       mcfg_item_prefix.attrib,   
                                       (uint8 *)mcfg_nv_item_data_buf,
                                       nv_mcfg_cust_item_length,
                                       TRUE );

        if(mcfg_nv_status != MCFG_NV_STATUS_OK )
        {
          MCFG_MSG_ERROR_2("Unable to rmv symlink Muxd NV Item %d status=%d", 
                            nv_mcfg_cust_item, mcfg_nv_status);
        }
        /* Free the MCFG nv hdr and buf */
        mcfg_free(mcfg_nv_item_data_hdr);
        mcfg_free(mcfg_nv_item_data_buf);

        /* point to the next MCFG item to be written i.e advance
           the ram_cust_data_ptr by the nv item length in this case */
        ram_cust_data_ptr += nv_mcfg_cust_item_length;
        MCFG_MSG_LOW_3("Encountered delete Muxd item item %d len %d %p next",
           nv_mcfg_cust_item, nv_mcfg_cust_item_length, ram_cust_data_ptr);
        break;
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */

      case MCFG_INT_MUXD_SW_ONLY:
        /* Muxd Software only - Update the symbolic links for this NV items */

        /* Get the hdr */
        mcfg_nv_item_data_hdr = 
          mcfg_malloc( sizeof(uint8)* MCFG_VER_NV_HEADER_LEN );
        if( mcfg_nv_item_data_hdr == NULL) 
        {
          MCFG_MSG_ERROR("MCFG: Memory allocation failure while reading MCFG_INT_MUXD_SW_ONLY");
          break;
        }
        memscpy(mcfg_nv_item_data_hdr, (MCFG_VER_NV_HEADER_LEN * sizeof(uint8)),
                ram_cust_data_ptr, (MCFG_VER_NV_HEADER_LEN * sizeof(uint8)) ); 

        /* Get the NV item type and length */
        nv_mcfg_cust_item = 
          (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_type;
        nv_mcfg_cust_item_length = 
          (nv_items_enum_type) mcfg_nv_item_data_hdr->mcfg_nv_item_length;

        item_index = 0;
        if(mcfg_item_prefix.attrib & U_ITEM_ATTRIB_INDEXED) {
          nv_mcfg_cust_item_length--;

          /* read the index. it is placed right after the header */
          item_index = *(ram_cust_data_ptr + MCFG_VER_NV_HEADER_LEN);
        }

         /* Remove  the symlink for the Muxd NV Items */
        mcfg_nv_status = mcfg_nv_rm_symlink( nv_mcfg_cust_item, item_index, subs_id );

        if(mcfg_nv_status != MCFG_NV_STATUS_OK )
        {
          MCFG_MSG_ERROR_2(
             "Unable to rmv symlink for Muxd SW NV Item %d, status=%d", 
             nv_mcfg_cust_item, mcfg_nv_status);
        }

        /* Free the MCFG NV hdr */
        mcfg_free(mcfg_nv_item_data_hdr);

        /* Advance to the next MCFG item */
        ram_cust_data_ptr += 
          (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
        break;

      case MCFG_INT_SW_ONLY:
        /* Advance to the next MCFG item */
        MCFG_MSG_LOW("mcfg_cleanup_nvefs MCFG_INT_SW_ONLY");
        ram_cust_data_ptr += 
          (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
        break;

      case MCFG_INT_MUXD_SW_ONLY_EFS_FILE:
        MCFG_MSG_LOW("mcfg_cleanup_nvefs MCFG_INT_MUXD_SW_ONLY_EFS_FILE");
        /* TODO - not supported for now, advance to the next MCFG item */
        ram_cust_data_ptr += 
          (mcfg_item_prefix.item_length - sizeof(mcfg_item_prefix));
        break;

      default:
        /* Unknown - Err fatal should never be here */
        MCFG_ERR_FATAL("mcfg_cleanup_nvefs Parsing Error %d", 
                       mcfg_item_type_int, 0, 0); 
        break;
      }

      mcfg_nvefs_update_status = TRUE;
    }

  return mcfg_nvefs_update_status;
} /* mcfg_cleanup_nvefs() */

/*===========================================================================

FUNCTION  modem_cfg_cleanup() 

DESCRIPTION
  This is the modem config cleanup routine called during modem config
  deactivation

DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  boolean - TRUE if success, otherwise FALSE.

SIDE EFFECTS
  None

===========================================================================*/
boolean modem_cfg_cleanup(mcfg_config_type_e_type type, 
                          mcfg_sub_id_type_e_type sub)
{
  /* RAM Ptr to the MCFG segment */ 
  uint8* mcfg_seg_ptr; 
  uint8* ram_cust_data_ptr;
   
  uint32 ram_mcfg_version,num_mcfg_items ;

  static mcfg_config_id_s_type id;
  static mcfg_config_s_type  mcfg_current_buf;
  mcfg_config_s_type*  p_mcfg_buf=NULL;
  //uint8* p_devcfg_buf=NULL;
  uint32 mcfg_add_info = 0;
  boolean status = FALSE;
/* -------------------------------------------------------------------- */
  MCFG_MSG_HIGH_1("====  Starting modem_cfg_cleanup , subs_id= %d ====  ", (int) sub);

  p_mcfg_buf = &mcfg_current_buf;
  mcfg_current_buf.config_len = 0;
  mcfg_current_buf.config_addr = NULL;

  mcfg_extra_cleanup(type, sub);

  do
  {
    /* LOAD AND AUTHENTICATE MODEM CFG IMAGE */
    if(MODEM_CFG_PROC_SUCCESS != 
         modem_cfg_load_and_auth_mcfg(type, &id, p_mcfg_buf, TRUE, sub))
    {
      MCFG_MSG_ERROR_1("mcfg_load_and_auth_seg fail for cfg type %0x",type);
      break;
    }

    /* Get the MCFG SEG from the Modem Config File buffer */
    mcfg_seg_ptr = mcfg_load_seg(p_mcfg_buf, &mcfg_add_info );
    if (mcfg_seg_ptr == NULL )
    {
      MCFG_MSG_ERROR_2("Cannot find MCFG_SEG for cfg_type %d subs_id %d", type, sub);
      break;
    }

    /* Initialize our RAM ptr to the start of MCFG segment */
    ram_cust_data_ptr = mcfg_seg_ptr;

    /* Validate the MCFG Header Information and get the number of NV items
       from this data */
    num_mcfg_items = 
      validate_mcfg_header(type, ram_cust_data_ptr, sub, MCFG_REFRESH_TYPE_SLOT_N_SUBS);

    if (!num_mcfg_items ) 
    {
      MCFG_MSG_ERROR_2("Validate MCFG Header Fail, cfg_type = %0x!, sub = %d", type, sub);
      break;
    }

    /* the ram_cust_data_ptr points to the first MCFG NV element */
    ram_cust_data_ptr+= MCFG_HEADER_LEN; 

    // Sanity check to make sure the config to be cleaned up matches the saved NV config version
    if( mcfg_update_required(type, ram_cust_data_ptr, &ram_mcfg_version, sub))
    {
      MCFG_MSG_ERROR_2("Update shouldn't be required, type = %0x!, sub = %d", type, sub);
      //break;
    }

    if (ram_mcfg_version == 0)
    {
      MCFG_MSG_ERROR_1("Invalid mcfg version b4 cleanup, type = %0x!", type);
      break;
    }

    ram_cust_data_ptr = mcfg_seg_ptr + MCFG_NV_DATA_OFFSET;
          
    if (!mcfg_cleanup_nvefs(type, num_mcfg_items, ram_cust_data_ptr, sub))
    {
      MCFG_MSG_ERROR_1("Update NVEFS Fail for cfg type, %0x!", type);
      break;
    }
    
    ram_mcfg_version = 0;
    
    if(MCFG_NV_STATUS_OK != mcfg_nv_write(
         (type == MCFG_TYPE_SW)? MCFG_SW_VERSION_I : MCFG_HW_VERSION_I,
         0, sub, (uint8 *)&ram_mcfg_version, sizeof(ram_mcfg_version)))
    {
      MCFG_MSG_ERROR_2("Update Cfg version fail for cfg type, %0x, subs_id = %d!", type, sub);
      break;
    }

#ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
    /* For debug purposes, update the details to the MCFG history log */
    if(!mcfg_update_history_log(type, ram_mcfg_version))
    {
      MCFG_MSG_ERROR_3( "MCFG history log unsuccessful", 0, 0, 0 );
      break;
    }
#endif /* FEATURE_MODEM_CFG_HISTORY_LOGGING */

    status = TRUE;
  }while(0);

  mcfg_utils_free_config_buffer(p_mcfg_buf);

  return status;

} /* modem_cfg_cleanup() */


