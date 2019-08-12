/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

	MODEM_CFG PROC - mcfg_load.c

GENERAL DESCRIPTION

  This file makes up the MCFG load part of MCFG PROC module

EXTERNALIZED FUNCTIONS
  modem_cfg_load_and_auth_mcfg
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
 
Copyright (c) 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE/

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_load.c#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $ 
 
when       who     what, where, why
--------   ---    ----------------------------------------------------------
01/30/12   sbt     Initial Creation.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "modem_mem.h"                            /* Modem Mem header file */

#include "fs_public.h"                          /* For handling EFS files */
#include "fs_lib.h"
#include "fs_sys_types.h"

#include "nv_items.h"

#include "mcfg_feature_config.h"
#include "mcfg_common.h"
#include "mcfg_load.h"
#include "mcfg_nv.h"
#include "mcfg_int.h"
#include "mcfg_osal.h"
#include "mcfg_utils.h"
#include "mcfg_auth_i.h"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global definitions for constants, macros, types,
variables

===========================================================================*/

#define MCFG_ELF_PT_NULL  (0x0)
#define MCFG_ELF_PT_LOAD  (0x1)

/*=========================================================================== 
  MACROs, Types defintions cloned from miprogressive.h
===========================================================================*/
#define MI_PBT_NON_PAGED_SEGMENT   0x0
#define MI_PBT_PAGED_SEGMENT       0x1

#define MI_PBT_NOTUSED_SEGMENT     0x3
#define MI_PBT_SHARED_SEGMENT      0x4

#define MI_PBT_HASH_SEGMENT        0x2

#define MI_PBT_FLAG_PAGE_MODE_MASK        0x100000
#define MI_PBT_FLAG_ACCESS_TYPE_MASK      0xE00000
#define MI_PBT_FLAG_SEGMENT_TYPE_MASK     0x7000000

#define MI_PBT_FLAG_PAGE_MODE_SHIFT       0x14
#define MI_PBT_FLAG_ACCESS_TYPE_SHIFT     0x15
#define MI_PBT_FLAG_SEGMENT_TYPE_SHIFT    0x18

#define MI_PBT_PAGE_MODE_VALUE(x) \
         ( ((x) & MI_PBT_FLAG_PAGE_MODE_MASK) >> \
           MI_PBT_FLAG_PAGE_MODE_SHIFT )

#define MI_PBT_ACCESS_TYPE_VALUE(x) \
         ( ((x) & MI_PBT_FLAG_ACCESS_TYPE_MASK) >> \
           MI_PBT_FLAG_ACCESS_TYPE_SHIFT )

#define MI_PBT_SEGMENT_TYPE_VALUE(x) \
         ( ((x) & MI_PBT_FLAG_SEGMENT_TYPE_MASK) >> \
            MI_PBT_FLAG_SEGMENT_TYPE_SHIFT )


/* ELF header  */
#define ELFINFO_MAGIC_SIZE (16)
typedef struct
{
  unsigned char e_ident[ELFINFO_MAGIC_SIZE]; /* Magic number and other info   */
  uint16        e_type;                /* Object file type                    */
  uint16        e_machine;             /* Architecture                        */
  uint32        e_version;             /* Object file version                 */
  uint32        e_entry;               /* Entry point virtual address         */
  uint32        e_phoff;               /* Program header table file offset    */
  uint32        e_shoff;               /* Section header table file offset    */
  uint32        e_flags;               /* Processor-specific flags            */
  uint16        e_ehsize;              /* ELF header size in bytes            */
  uint16        e_phentsize;           /* Program header table entry size     */
  uint16        e_phnum;               /* Program header table entry count    */
  uint16        e_shentsize;           /* Section header table entry size     */
  uint16        e_shnum;               /* Section header table entry count    */
  uint16        e_shstrndx;            /* Section header string table index   */
} Elf32_Ehdr;

typedef struct
{
  uint32 p_type;                   /* Segment type */
  uint32 p_offset;                 /* Segment file offset */
  uint32 p_vaddr;                  /* Segment virtual address */
  uint32 p_paddr;                  /* Segment physical address */
  uint32 p_filesz;                 /* Segment size in file */
  uint32 p_memsz;                  /* Segment size in memory */
  uint32 p_flags;                  /* Segment flags */
  uint32 p_align;                  /* Segment alignment */
} Elf32_Phdr;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

static boolean mcfg_load_read_data(mcfg_config_s_type* config_ptr, uint32 offset, uint32 size, uint8* data);

/*===========================================================================
=============================================================================
================================= FUNCTIONS =================================
=============================================================================
===========================================================================*/
 
/*===========================================================================

FUNCTION  mcfg_load_read_data() 

DESCRIPTION
  Seeks to <offset> & reads <size> bytes into buffer at <data>.  Calls
  efs_read() as many times as necessary to read the full <size>.
 
DEPENDENCIES

RETURN VALUE
  TRUE if all <size> bytes read from file without error, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_load_read_data(mcfg_config_s_type* config_ptr, uint32 offset, uint32 size, uint8* data)
{
  uint8* mcfg_data_ptr=NULL;

  if ( (data != NULL) && (size <= config_ptr->config_len) &&
       !MCFG_INTERGER_OVERFLOWED(config_ptr->config_addr, offset))
  {
    mcfg_data_ptr=(uint8 *)(config_ptr->config_addr) + offset;
 
    memscpy(data, config_ptr->config_len, mcfg_data_ptr, size);
    return TRUE;
  }
  else
  {
    MCFG_MSG_ERROR("Error in mcfg_load_read_data");
    return FALSE;
  }

} /* mcfg_load_read_data() */

/*===========================================================================

FUNCTION  mcfg_get_seg_info() 

DESCRIPTION
  Parses the ELF and program headers of a MCFG IMAGE (MBN) file to determine the
  location and size of a segment.
 
DEPENDENCIES

RETURN VALUE
  TRUE on success, FALSE on failure

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_get_seg_info(mcfg_config_s_type* config_ptr, 
                                 uint32* offset, 
                                 uint32* data_size,
                                 boolean hash)
{
  Elf32_Ehdr mcfg_ehdr;
  Elf32_Phdr mcfg_phdr;

  int phdrs_read;
  boolean success;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  success = FALSE;

  if (offset != NULL && data_size != NULL)
  {
    /* Start by reading out the ELF header */
    if (!mcfg_load_read_data(config_ptr, 0, sizeof(mcfg_ehdr), (uint8*) &mcfg_ehdr))
    {
      MCFG_MSG_ERROR("Error reading ELF header");
    }
    else
    {
      /* Read each program header until we find the non-paged segment (this is 
       * where the MCFG data resides) */
      phdrs_read = 0;
      *offset = mcfg_ehdr.e_phoff;
      do
      {
        if (!mcfg_load_read_data(config_ptr, *offset, sizeof(mcfg_phdr), (uint8*) &mcfg_phdr))
        {
          MCFG_MSG_ERROR_2("Error reading program header %d (%d)",phdrs_read,efs_errno);
          break;
        }
        else if (hash)
        {
          //Hashed segment
          if ((mcfg_phdr.p_type == MCFG_ELF_PT_LOAD || mcfg_phdr.p_type == MCFG_ELF_PT_NULL) &&
                   MI_PBT_PAGE_MODE_VALUE(mcfg_phdr.p_flags) == MI_PBT_NON_PAGED_SEGMENT &&
                   MI_PBT_SEGMENT_TYPE_VALUE(mcfg_phdr.p_flags) == MI_PBT_HASH_SEGMENT &&
                   MI_PBT_ACCESS_TYPE_VALUE(mcfg_phdr.p_flags) != MI_PBT_NOTUSED_SEGMENT &&
                   MI_PBT_ACCESS_TYPE_VALUE(mcfg_phdr.p_flags) != MI_PBT_SHARED_SEGMENT)
          {
            success = TRUE;
          }
          else
          {
            *offset += sizeof(mcfg_phdr);
          }
        }
        else
        {
          //data segment
          if (mcfg_phdr.p_type == MCFG_ELF_PT_LOAD &&
                 MI_PBT_PAGE_MODE_VALUE(mcfg_phdr.p_flags) == MI_PBT_NON_PAGED_SEGMENT &&
                 MI_PBT_SEGMENT_TYPE_VALUE(mcfg_phdr.p_flags) != MI_PBT_HASH_SEGMENT &&
                 MI_PBT_ACCESS_TYPE_VALUE(mcfg_phdr.p_flags) != MI_PBT_NOTUSED_SEGMENT &&
                 MI_PBT_ACCESS_TYPE_VALUE(mcfg_phdr.p_flags) != MI_PBT_SHARED_SEGMENT)
          {
            success = TRUE;
          }
          else
          {
            *offset += sizeof(mcfg_phdr);
          }
        }
        phdrs_read++;
      } while (phdrs_read < mcfg_ehdr.e_phnum && !success);
    }

    if (success)
    {
      if (!MCFG_SIZE_OFFSET_WITHIN_LIMIT(
            mcfg_phdr.p_filesz, mcfg_phdr.p_offset, config_ptr->config_len))
      {
        MCFG_MSG_ERROR_3("MCFG segment type %d invalid offset %lx size %lx", 
                         hash, mcfg_phdr.p_offset, mcfg_phdr.p_filesz);
        success = FALSE;
      }
      else
      {
        *offset = mcfg_phdr.p_offset;
        *data_size = mcfg_phdr.p_filesz;
      }
    }
    else
    {
      MCFG_MSG_ERROR_1("Couldn't find MCFG segment type %d!", hash);
    }
  }

  return success;
} /* mcfg_get_seg_info() */

/*===========================================================================

FUNCTION  mcfg_get_hash_seg_info() 

DESCRIPTION
  Parses the ELF and program headers of a MCFG IMAGE (MBN) file to determine the
  location and size of the hash segment.
 
DEPENDENCIES

RETURN VALUE
  TRUE on success, FALSE on failure

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_get_hash_seg_info(mcfg_config_s_type* config_ptr, 
                               uint32* offset, 
                               uint32* data_size)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return mcfg_get_seg_info(config_ptr, offset, data_size, TRUE);
} /* mcfg_get_hash_seg_info() */

/*===========================================================================

FUNCTION  mcfg_get_data_seg_info() 

DESCRIPTION
  Parses the ELF and program headers of a MCFG IMAGE (MBN) file to determine the
  location and size of the MCFG data.
 
DEPENDENCIES

RETURN VALUE
  TRUE on success, FALSE on failure

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_get_data_seg_info(mcfg_config_s_type* config_ptr, 
                               uint32* offset, 
                               uint32* data_size)
{
  return mcfg_get_seg_info(config_ptr, offset, data_size, FALSE);
} /* mcfg_get_data_seg_info() */
 
/*===========================================================================

FUNCTION  modem_cfg_load_and_auth_mcfg() 

DESCRIPTION
 
 
DEPENDENCIES

RETURN VALUE
  mcfg_error_e_type
 
SIDE EFFECTS
  None

===========================================================================*/
mcfg_error_e_type modem_cfg_load_and_auth_mcfg(uint8 cfg_type,
                                    mcfg_config_id_s_type* current_id_ptr, 
                                    mcfg_config_s_type* config_ptr,
                                    boolean active,
                                    mcfg_sub_id_type_e_type sub)
{
  boolean ret = FALSE;

  mcfg_error_e_type error = MODEM_CFG_PROC_LOAD_FAIL;

  MCFG_CHECK_NULL_PTR_RET_ERROR(current_id_ptr, MCFG_ERR_NULL_POINTER);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config_ptr, MCFG_ERR_NULL_POINTER);

  if (active)
  {
    ret = mcfg_utils_get_active_config(cfg_type, current_id_ptr, sub);
  }
  else
  {
    ret = mcfg_utils_get_selected_config(cfg_type, current_id_ptr, sub);
  }

  if(ret)
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
}/* mcfg_load_and_auth_seg() */

uint8* mcfg_load_seg(mcfg_config_s_type* config_ptr, uint32* add_info_ptr)
{
  uint32  offset;
  uint32  mcfg_seg_size = 0;
  uint8* mcfg_seg_ptr;

  if (!mcfg_get_data_seg_info(config_ptr, &offset, &mcfg_seg_size))
  {
    MCFG_MSG_ERROR("Error reading MCFG segment");
  }

  if( (mcfg_seg_size >= config_ptr->config_len) || (mcfg_seg_size == 0) ||
        MCFG_INTERGER_OVERFLOWED(config_ptr->config_addr, offset))
  {
    mcfg_seg_ptr = NULL;
  }
  else
  {
    mcfg_seg_ptr = (uint8 *)(config_ptr->config_addr) + offset;
  }

  *add_info_ptr = 0;

  return mcfg_seg_ptr;
}

uint8* mcfg_load_devcfg(mcfg_config_s_type* config_ptr)
{
  uint8* devcfg_ptr=NULL;
  return devcfg_ptr; 
}
