/*===========================================================================
                        mcfg_proc.h

DESCRIPTION

   External header file for modem_proc
 
Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
 
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_proc/inc/mcfg_proc.h#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
1/18/12      sbt   Create

===========================================================================*/

#ifndef MCFG_PROC_H
#define MCFG_PROC_H

#include "mcfg_common.h"
#include "nv_items.h"
#include "mcfg_nv.h"
#include "mcfg_refresh_i.h"

/* For testing purposes only, replace with MCFG_NV Items once tested */
#define MCFG_SW_VERSION_I   NV_CUST_UQCN_C2K_VERSION_I
#define MCFG_HW_VERSION_I   NV_CUST_UQCN_UMTS_VERSION_I

#define MCFG_HEADER_LEN                         16

#define MCFG_VER_NV_HEADER_LEN                  4
#define MCFG_VERSION_LEN                        4 
#define MCFG_NV_DATA_OFFSET                     ((MCFG_HEADER_LEN)+(MCFG_VER_NV_HEADER_LEN)+(MCFG_VERSION_LEN))
#define MCFG_PREFIX_LEN                         8 

#define MCFG_RESTORE_FACT_VALUE     0xFACD0000

/* Borrowed from mcfg_proc.c */

#define MCFG_PRL_VER_OFFSET             ((MCFG_VER_NV_HEADER_LEN)+1) 

#define MCFG_VER_EQUALS_MUXD_VERSION    0x00
#define MCFG_VER_GREATER_MUXD_VERSION   0x01
#define MCFG_VER_LESS_MUXD_VERSION      0x02

#define MCFG_COMPAT_PASS            0x01
#define MCFG_COMPAT_FAIL            0x02

#define U_ITEM_ATTRIB_CFG               0x01   /* C */
#define U_ITEM_ATTRIB_MUXD              0x02   /* M */
#define U_ITEM_ATTRIB_WRITE_ONCE        0x04   /* W */
#define U_ITEM_ATTRIB_REST_FACT         0x08   /* R */
#define U_ITEM_ATTRIB_CONDITIONAL       0x10   /* P - Used for PRL - PWMC mask, Legacy */
#define U_ITEM_ATTRIB_MULTISIM          0x10   /* S - For Multi-SIM related NVs */
#define U_ITEM_ATTRIB_INDEXED           0x20   /* I */
#define U_ITEM_ATTRIB_DELETE            0x40   /* D */
#define U_ITEM_ATTRIB_UPDATE_ONLY       0x80   /* U - Read complete the Item first, and update only if not the same - this setting is good for fast read, slow write type devices */

#define MCFG_SUBS_MASK                  (0x0F) 

// NV-CAT defines for internal processing 
#define MCFG_INT_NV_ITEM                0x01
#define MCFG_INT_EFS_FILE               0x02
#define MCFG_INT_SW_ONLY                0x03
#define MCFG_INT_DELETE_NV_ITEM         0x04
#define MCFG_INT_DELETE_EFS_FILE        0x05
#define MCFG_INT_STATIC_WO_EFS_FILE     0x06
#define MCFG_INT_MUXD_NV_ITEM           0x07
#define MCFG_INT_MUXD_SW_ONLY           0x08
#define MCFG_INT_MUXD_EFS_FILE          0x09
#define MCFG_INT_MUXD_SW_ONLY_EFS_FILE  0x0A
#define MCFG_INT_DATA_PROFILE           0x0B
#define MCFG_INT_DELETE_DATA_PROFILE    0x0C
#define MCFG_INT_STATIC_WO_DATA_PROFILE 0x0D
#define MCFG_INT_MUXED_DATA_PROFILE     0x0E
#define MCFG_INT_MUXD_SW_ONLY_DATA_PROFILE  0x0F
#define MCFG_INT_CONFIG_ITEM            0x10
 
#define MCFG_SW_VER_CARRIER_MAJOR_MASK          0x0000FF00

#define MCFG_FILE_PATH_LENGTH 128
#define MCFG_TL_SIZE_BYTES 4

/*==============================================================================
  Typedefs
==============================================================================*/
typedef PACKED struct 
{ 
  uint16	mcfg_nv_item_type;
  uint16	mcfg_nv_item_length;
}mcfg_nv_item_hdr_type;

typedef PACKED struct 
{ 
  uint32    item_length;
  uint8	    item_type;
  uint8     attrib;
  uint8     sp_ops;
  uint8     spare1;
}mcfg_item_prefix_type;


typedef PACKED struct 
{ 
  mcfg_nv_item_hdr_type hdr;
  uint8* value;
} mcfg_tlv;
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
                          mcfg_sub_id_type_e_type sub);

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
uint32 modem_cfg_process(void);

void mcfg_set_curr_subs_id(uint8 subs_id);
boolean mcfg_proc_subs_mask(uint8 subs_mask, uint8 curr_subs_id);
uint8  mcfg_parse_item_prefix(uint8 cfg_type,
                              uint8* ram_cust_data_ptr,
                              mcfg_item_prefix_type * mcfg_item_prefix_ptr,
                             mcfg_refresh_type_e_type refresh_type, 
                             uint8 subs_id);

mcfg_nv_status_e_type mcfgnv_delete(uint32 eItem, uint8 attrib, byte u8Index, uint8 subs_id);
mcfg_nv_status_e_type mcfgnv_update_symlink( uint32 eItem, uint8 attrib, uint8* data_ptr, uint32 item_size);
mcfg_nv_status_e_type mcfgnv_remove_symlink_if_present(uint32 eItem,uint8 attrib,uint8* data_ptr, uint32 item_size, boolean force);

mcfg_nv_status_e_type mcfg_delete_efs_file(uint8* efsData, uint8 attrib);
boolean mcfg_create_efs_file(uint8* efsData, uint8 attributes, boolean restore_fact);
boolean mcfg_create_data_profile(uint8* efsData, uint8 attributes, boolean restore_fact);
mcfg_nv_status_e_type mcfg_delete_data_profile(uint8* efsData, uint8 attrib);

boolean mcfg_read_tlv(mcfg_tlv* dest, uint8* src);
boolean mcfg_verify_efs_item_contents(mcfg_tlv* header, mcfg_tlv* body);

uint32 validate_mcfg_header(uint8 cfg_type , uint8* data_ptr, uint8 subs_id, mcfg_refresh_type_e_type refresh_type);
mcfg_nv_status_e_type mcfgnv_read( uint32 eItem, uint8 attrib, uint8* data_ptr, uint32 size);
mcfg_nv_status_e_type mcfgnv_write( uint32 eItem, uint8 attrib, uint8* data_ptr, uint32 size);

#ifdef FEATURE_MCFG_MULTISIM_SUPPORT
uint8 mcfg_get_curr_subs_id(void);
#endif /* FEATURE_MCFG_MULTISIM_SUPPORT */

boolean mcfg_get_profile_path(char* profile_path, uint8 path_len, char* fullpath, uint8 subs_id);

#ifdef FEATURE_MCFG_REFRESH
uint32 modem_cfg_process_single
(
  uint8                    current_mcfg_type,
  mcfg_refresh_type_e_type refresh_type, 
  mcfg_refresh_index_type  mcfg_subs_num
);
#endif /* FEATURE_MCFG_REFRESH */

#endif /* MCFG_PROC_H */
