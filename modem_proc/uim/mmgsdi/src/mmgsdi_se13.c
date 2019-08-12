/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                  M M G S D I   S E 1 3  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used to process the SE13 table that is
  used by the EONS algorithm. A static SE13 table is defined as well as a
  dynamic one is created if available in the modem EFS.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_se13.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   ar      EONS selective logging
06/15/16   bcho    Assign middle index only after retrieving
06/10/16   rp      Reduce repeated F3 logging in EONS
06/07/16   ar      Removing string prints from APDU and AUTH
05/24/16   bcho    EONS F3 reduction
05/23/16   lm      Remove F3s causing continuous burst of diag prints
05/22/16   vdc     Remove F3 messages for memory allocation failure
09/23/15   bcho    Use MCFG Context enum instead of typecast
09/07/15   av      Migrate to mcfg EFS APIs
09/09/14   kk      Enhancements in NV handling
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/15/14   yt      Support for NV refresh
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
04/03/14   am      Fix compile error on SBM due to assert_on_crit_sect_owner
02/20/14   ar      Added trace for source name of SE13 Table
01/17/14   bcho    Convert ASCII encoding to GSM 7bit default unpacked
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
11/05/13   df      Fix LLVM compiler issue
10/28/13   kb      Add offtarget support to check mutex is owned or not
10/04/13   rp      F3 message reduction
10/04/13   vv      Added API to return se13 table release date and source
09/05/13   jv      Added binary version 1.1 support
08/30/13   av      Support for Chinese language PLMN names
07/24/13   jv      Removed array and placed in its own header file
07/15/13   vs      EONS updates to support single baseband SGLTE
07/10/13   kb      SE13 table updated 01 July, 2013 GSMA published list
06/28/13   bcho    APIs added to return plmn info from SE13 table.
06/21/13   kb      SE13 table updated 03 June, 2013 GSMA published list
06/14/13   bcho    SE13 table updated 22 May, 2013 GSMA published list
05/29/13   tl      Klockwork fixes
05/28/13   spo     Replaced instances of memcpy with memscpy
05/07/13   kb      SE13 table updated to 9 April, 2013 GSMA published list
05/02/13   at      Klockwork fixes for SE13 related APIs
04/17/13   at      Initial version


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_session.h"
#include "mmgsdi_eons.h"
#include "fs_sys_types.h"
#include "fs_public.h"
#include "mmgsdi_se13.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_se13_table.h"
#include "mmgsdi_se13_addl_lang_table.h"
#include "mcfg_fs.h"
#include "uim_selective_logging.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SE13_PLMN_NODE_TYPE

   DESCRIPTION:
     Structure for linked list node to carry  plmn info of SE13 table entry
-------------------------------------------------------------------------------*/
typedef struct mmgsdi_se13_plmn_node_type{
  uint16               mcc;                     /**< MCC value.*/
  uint16               mnc;                     /**< MNC value.*/
  uint32               network_mask;            /**< RAT type.*/
  mmgsdi_data_type     plmn_long_name;          /**< plmn long name.*/
  mmgsdi_data_type     plmn_short_name;         /**< plmn short name.*/
  struct mmgsdi_se13_plmn_node_type *next_ptr; /**< Ptr to the next node.*/
} mmgsdi_se13_plmn_node_type;

/*=============================================================================

                       MMGSDI MACROS

=============================================================================*/

/* Base SE13 Binary versions supported */
#define MMGSDI_SE13_BINARY_MINOR_VER_0     (0)

/* The bit of the network mask which corresponds to
   the source of the table entry */
#define MMGSDI_SE13_IS_GSMA_ENTRY_BIT      (0x80)

/* EFS location & filename for the dynamic SE13 table */
#define MMGSDI_SE13_TABLE_NV_PATH   "/mmgsdi/eons/se13_table"

#define MMGSDI_SE13_MCC_INVALID_VAL        (0XFFFF)
#define MMGSDI_SE13_MNC_INVALID_VAL        (0XFFFF)
#define MMGSDI_SE13_NET_MASK_INVALID_VAL   (0X0)
#define MMGSDI_SE13_PLMN_LIST_CHUNK_SIZE   (5120)
#define MMGSDI_SE13_RFU_SIZE               (20)

/* ===========================================================================
 MACRO:       MMGSDI_SE13_ENTER_CACHE_CRIT_SECT
 DESCRIPTION: To enter rex critical section for SE13 cache
 ===========================================================================*/
#define MMGSDI_SE13_ENTER_CACHE_CRIT_SECT                                 \
  MMGSDIUTIL_ENTER_CRIT_SECT(&mmgsdi_se13_crit_sect,                      \
                             MMGSDIUTIL_CRIT_SECT_SE13_CACHE);            \

/* ===========================================================================
 MACRO:       MMGSDI_SE13_LEAVE_CACHE_CRIT_SECT
 DESCRIPTION: To leave rex critical section for SE13 cache
 ===========================================================================*/
#define MMGSDI_SE13_LEAVE_CACHE_CRIT_SECT                                 \
  MMGSDIUTIL_LEAVE_CRIT_SECT(&mmgsdi_se13_crit_sect,                      \
                             MMGSDIUTIL_CRIT_SECT_SE13_CACHE);            \

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_SE13_FILE_STATUS_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the status of the SE13 Table binary file.
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_SE13_FILE_STATUS_NOT_INIT     = 0x00,
  MMGSDI_SE13_FILE_STATUS_NOT_PRESENT,
  MMGSDI_SE13_FILE_STATUS_INVALID,
  MMGSDI_SE13_FILE_STATUS_USED
 } mmgsdi_se13_file_status_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SE13_HEADER_TYPE

   DESCRIPTION:
     Contains the info of the header as defined in the format specified in
     the App note 80-NG048-1.
     version:          Version of the format used for binary file encoding.
     num_entries:      Total number of entries in PLMN list block.
     release_date:     Release date of the SE.13 table database by GSMA.
     rfu:              Reserved for future.
     checksum:         Checksum value that could be used to verify the
                       validity of the entire binary file.

-----------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8  version_major;
  uint8  version_minor;
  uint16 num_entries;
  uint8  se13_release_month;
  uint8  se13_release_day;
  uint16 se13_release_year;
  uint8  rfu[MMGSDI_SE13_RFU_SIZE];
  uint32 checksum;
} mmgsdi_se13_header_type;

/* ------------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SE13_PLMN_LIST_ENTRY_TYPE

   DESCRIPTION:
     Contains the info of the PLMN list as defined in the format specified in
     the App note 80-NG048-1.
     mcc:              MCC.
     mnc:              MNC.
     rat_mask:         Bitmask that represents the RAT value.
     short_name_len:   Length in bytes of the short name.
     long_name_len:    Length in bytes of the long name.
     name_offset:      Offset of the short name in the Names list.

-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint16 mcc;
  uint16 mnc;
  uint8  rat_mask;
  uint8  short_name_len;
  uint8  long_name_len;
  uint16 name_offset;
} mmgsdi_se13_plmn_list_entry_type;

/* ------------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SE13_DYNAMIC_TABLE_DATA_TYPE

   DESCRIPTION:
     Contains the info of the dynamic SE13 binary file read from EFS. The format
     of this binary file is specified in the App note 80-NG048-1.
     file_initialized: Indicates if the EFS file was tried to initialize
     se13_header:      Header of the SE13 binary file as defined.
     plmn_list_len:    Length of plmn_list_ptr in bytes.
     plmn_list_ptr:    Pointer to the PLMN list read from the file.

-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_se13_file_status_enum_type      file_status;
  mmgsdi_se13_header_type                se13_header;
  uint32                                 plmn_list_len;
  mmgsdi_se13_plmn_list_entry_type     * plmn_list_ptr;
} mmgsdi_se13_dynamic_table_data_type;


/* Specifies if we already trieSE13 table in EFS */
static mmgsdi_se13_dynamic_table_data_type mmgsdi_se13_file_info =
  {MMGSDI_SE13_FILE_STATUS_NOT_INIT,{0},0,NULL};

/* Critical section to protect accesses to SE13 caches */
static rex_crit_sect_type  mmgsdi_se13_crit_sect;

/* Defines the max minor version supported for every major version
   Each index is a major version, 0th index = Major Version 1.
   The value at each index corresponds to the maximum minor version
   supported for that major version.*/
const uint8 mmgsdi_se13_major_minor_version_support [] = {1};

/*===========================================================================
FUNCTION MMGSDI_SE13_INIT

DESCRIPTION
  Function called to initialize critical section for cached SE13 data.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
void mmgsdi_se13_init(
  void
)
{
  rex_init_crit_sect(&mmgsdi_se13_crit_sect);
} /* mmgsdi_se13_init */


/*==========================================================================
FUNCTION: MMGSDI_SE13_CHECK_IF_FILE_PRESENT

DESCRIPTION:
  Fetches the number of entries in the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, and the read & sanity check of the file is successful, number of
    entries is determined from this file.
  If not, it returns number entries from the statically defined table.

RETURN VALUE
  BOOLEAN

SIDE EFFECTS
  None
==========================================================================*/
static boolean mmgsdi_se13_check_if_file_present
(
  void
)
{
  int                       fd             = 0;
  uint32                    i              = 0;
  fs_off_t                  efs_file_ret   = 0;
  uint16                    num_entries    = 0;
  boolean                   ret_value      = FALSE;
  boolean                   stop_execution = FALSE;
  uint8                     max_major_version_supported = 0;

  MMGSDI_SE13_ENTER_CACHE_CRIT_SECT;
  do
  {
    /* Perform openrations based on the file status */
    switch (mmgsdi_se13_file_info.file_status)
    {
      /* If file is already read & valid, use it */
      case MMGSDI_SE13_FILE_STATUS_USED:
        ret_value = TRUE;
        stop_execution = TRUE;
        break;

      /* If file read was already tried or marked invalid, dont use it */
      case MMGSDI_SE13_FILE_STATUS_INVALID:
      case MMGSDI_SE13_FILE_STATUS_NOT_PRESENT:
        stop_execution = TRUE;
        break;

      /* If file is being read first time, try to read it */
      case MMGSDI_SE13_FILE_STATUS_NOT_INIT:
      default:
        break;
    }

    /* Check if we want to continue */
    if(stop_execution == TRUE)
    {
      break;
    }

    /* Try to open the file */
    fd = mcfg_fopen(MMGSDI_SE13_TABLE_NV_PATH, MCFG_FS_O_RDONLY,
                    MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE);
    if (fd < 0)
    {
      /* File not present */
      mmgsdi_se13_file_info.file_status = MMGSDI_SE13_FILE_STATUS_NOT_PRESENT;
      break;
    }

    /* Step 1: Read the header & validate it */
    efs_file_ret = mcfg_fread(fd, (void *) &mmgsdi_se13_file_info.se13_header,
                              sizeof(mmgsdi_se13_header_type), MCFG_FS_TYPE_EFS);
    if (efs_file_ret != sizeof(mmgsdi_se13_header_type))
    {
      (void)mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
      goto do_cleanup;
    }

    /* Currently we validate only certain binary versions */
    max_major_version_supported = sizeof(mmgsdi_se13_major_minor_version_support)/
      sizeof(mmgsdi_se13_major_minor_version_support[0]);

    if (mmgsdi_se13_file_info.se13_header.version_major > max_major_version_supported)
    {
      UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                               "Unsupported SE13 Binary version: major: 0x%x",
                               mmgsdi_se13_file_info.se13_header.version_major);
      (void)mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
      goto do_cleanup;
    }

    /* Step 2: Allocate memory for PLMN list & read it from EFS file */
    num_entries = mmgsdi_se13_file_info.se13_header.num_entries;
    mmgsdi_se13_file_info.plmn_list_len  = num_entries * sizeof(mmgsdi_se13_plmn_list_entry_type);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_se13_file_info.plmn_list_ptr,
                                       mmgsdi_se13_file_info.plmn_list_len);
    if (mmgsdi_se13_file_info.plmn_list_ptr == NULL)
    {
      goto do_cleanup;
    }

    /* Once the memory block is allocated for the PLMN list, read the EFS file.
       Note that the read needs to be done in 5K blocks since EFS APIs cannot
       read more than 8K in one shot. Currently chunk size is defined as 5K */
    for (i = 0; i < mmgsdi_se13_file_info.plmn_list_len; i += MMGSDI_SE13_PLMN_LIST_CHUNK_SIZE)
    {
      uint8   * read_ptr = (uint8*)mmgsdi_se13_file_info.plmn_list_ptr;
      uint16    read_len = 0;

      /* Make sure how much we want to read */
      if ((i + MMGSDI_SE13_PLMN_LIST_CHUNK_SIZE) < mmgsdi_se13_file_info.plmn_list_len)
      {
        read_len = MMGSDI_SE13_PLMN_LIST_CHUNK_SIZE;
      }
      else
      {
        read_len = (uint16)(mmgsdi_se13_file_info.plmn_list_len - i);
      }

      /* Then read the chunk */
      efs_file_ret = mcfg_fread(fd, (void *) (read_ptr + i),
                                read_len, MCFG_FS_TYPE_EFS);
      if (efs_file_ret != read_len)
      {
        (void)mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
        goto do_cleanup;
      }
    }

    /* No need to worry about close at this point */
    (void)mcfg_fclose(fd, MCFG_FS_TYPE_EFS);

    /* File successfully initialized */
    mmgsdi_se13_file_info.file_status = MMGSDI_SE13_FILE_STATUS_USED;
    ret_value = TRUE;
    break;

do_cleanup:
    /* Mark file as invalid & clean any remaining file info */
    mmgsdi_se13_file_info.file_status = MMGSDI_SE13_FILE_STATUS_INVALID;
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_se13_file_info.plmn_list_ptr);
    mmgsdi_se13_file_info.plmn_list_len = 0;
    memset(&mmgsdi_se13_file_info.se13_header, 0, sizeof(mmgsdi_se13_header_type));
  }while(0);

  MMGSDI_SE13_LEAVE_CACHE_CRIT_SECT;
  return ret_value;
} /* mmgsdi_se13_check_if_file_present */


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_GET_NUM_ENTRIES

DESCRIPTION:
  Fetches the number of entries in the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, and the read & sanity check of the file is successful, number of
    entries is determined from this file.
  If not, it returns number entries from the statically defined table.

RETURN VALUE
  UINT16

SIDE EFFECTS
  None
==========================================================================*/
uint16 mmgsdi_se13_table_get_num_entries
(
  void
)
{
  uint16 num_of_entries = 0;

  /* If SE13 table is present in EFS, return the read length from EFS.
     Otherwise calculate the length from static table */
  if (mmgsdi_se13_check_if_file_present())
  {
    num_of_entries = mmgsdi_se13_file_info.se13_header.num_entries;
  }
#ifndef FEATURE_MMGSDI_SE13_FROM_EFS_ONLY
  else
  {
    num_of_entries = sizeof(mmgsdi_network_table)/
                            sizeof(mmgsdi_eons_network_info_data_type);
  }
#endif /* FEATURE_MMGSDI_SE13_FROM_EFS_ONLY */

  return num_of_entries;
} /* mmgsdi_se13_table_get_num_entries */


/*==========================================================================
FUNCTION: MMGSDI_ADDL_LANG_SE13_TABLE_GET_NUM_ENTRIES

DESCRIPTION:
  Fetches the number of entries in the additional languages SE13 Table.
  It returns number entries from the statically defined table.

RETURN VALUE
  UINT16

SIDE EFFECTS
  None
==========================================================================*/
static uint16 mmgsdi_addl_lang_se13_table_get_num_entries
(
  void
)
{
  uint16 num_of_entries = 0;

  num_of_entries = sizeof(mmgsdi_addl_lang_network_table)/
                     sizeof(mmgsdi_eons_network_info_addl_lang_data_type);

  return num_of_entries;
} /* mmgsdi_addl_lang_se13_table_get_num_entries */


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_GET_MCC

DESCRIPTION:
  Fetches the MCC for the specified index from the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, MCC (of that index) is fetched from this file.
  If not, MCC (of that index) is fetched from the statically defined table.

RETURN VALUE
  UINT16

SIDE EFFECTS
  None
==========================================================================*/
uint16 mmgsdi_se13_table_get_mcc
(
  uint16           index
)
{
  uint16  mcc             = MMGSDI_SE13_MCC_INVALID_VAL;
  uint16  num_of_entries  = 0;
  boolean is_file_present = FALSE;

  is_file_present = mmgsdi_se13_check_if_file_present();

  /* If SE13 binary is present in EFS, return the MCC from there,
     if not return it from the static table */
  if (is_file_present)
  {
    num_of_entries = mmgsdi_se13_file_info.se13_header.num_entries;

    if (mmgsdi_se13_file_info.plmn_list_ptr != NULL &&
        index < num_of_entries)
    {
      mcc = (mmgsdi_se13_file_info.plmn_list_ptr + index)->mcc;
    }
  }
#ifndef FEATURE_MMGSDI_SE13_FROM_EFS_ONLY
  else
  {
    num_of_entries = sizeof(mmgsdi_network_table)/
                       sizeof(mmgsdi_eons_network_info_data_type);
    if (index < num_of_entries)
    {
      mcc = mmgsdi_network_table[index].mcc;
    }
  }
#endif /* FEATURE_MMGSDI_SE13_FROM_EFS_ONLY */

  return mcc;
} /* mmgsdi_se13_table_get_mcc */


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_GET_MNC

DESCRIPTION:
  Fetches the MNC for the specified index from the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, MNC (of that index) is fetched from this file.
  If not, MNC (of that index) is fetched from the statically defined table.

RETURN VALUE
  UINT16

SIDE EFFECTS
  None
==========================================================================*/
uint16 mmgsdi_se13_table_get_mnc
(
  uint16           index
)
{
  uint16  mnc             = MMGSDI_SE13_MNC_INVALID_VAL;
  uint16  num_of_entries  = 0;
  boolean is_file_present = FALSE;

  is_file_present = mmgsdi_se13_check_if_file_present();

  /* If SE13 binary is present in EFS, return the MNC from there,
     if not return it from the static table */
  if (is_file_present)
  {
    num_of_entries = mmgsdi_se13_file_info.se13_header.num_entries;

    if (mmgsdi_se13_file_info.plmn_list_ptr != NULL &&
        index < num_of_entries)
    {
      mnc = (mmgsdi_se13_file_info.plmn_list_ptr + index)->mnc;
    }
  }
#ifndef FEATURE_MMGSDI_SE13_FROM_EFS_ONLY
  else
  {
    num_of_entries = sizeof(mmgsdi_network_table)/
                       sizeof(mmgsdi_eons_network_info_data_type);
    if (index < num_of_entries)
    {
      mnc = mmgsdi_network_table[index].mnc;
    }
  }
#endif /* FEATURE_MMGSDI_SE13_FROM_EFS_ONLY */

  return mnc;
} /* mmgsdi_se13_table_get_mnc */


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_GET_NETWORK_MASK

DESCRIPTION:
  Fetches the network type mask for the specified index from the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, network type mask (of that index) is fetched from this file.
  If not, network type mask (of that index) is fetched from the statically
    defined table.

RETURN VALUE
  UINT32

SIDE EFFECTS
  None
==========================================================================*/
uint32 mmgsdi_se13_table_get_network_mask
(
  uint16           index
)
{
  uint32    network_type_mask = MMGSDI_SE13_NET_MASK_INVALID_VAL;
  uint16    num_of_entries  = 0;
  boolean   is_file_present   = FALSE;

  is_file_present = mmgsdi_se13_check_if_file_present();

  /* If SE13 binary is present in EFS, return the mask from there,
     if not return it from the static table */
  if (is_file_present)
  {
    num_of_entries = mmgsdi_se13_file_info.se13_header.num_entries;

    if (mmgsdi_se13_file_info.plmn_list_ptr != NULL &&
        index < num_of_entries)
    {
      network_type_mask = (mmgsdi_se13_file_info.plmn_list_ptr + index)->rat_mask;
    }
  }
#ifndef FEATURE_MMGSDI_SE13_FROM_EFS_ONLY
  else
  {
    num_of_entries = sizeof(mmgsdi_network_table)/
                       sizeof(mmgsdi_eons_network_info_data_type);
    if (index < num_of_entries)
    {
      network_type_mask = mmgsdi_network_table[index].network_type_mask;
    }
  }
#endif /* FEATURE_MMGSDI_SE13_FROM_EFS_ONLY */

  return network_type_mask;
} /* mmgsdi_se13_table_get_network_mask */


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_IS_GSMA_ENTRY

DESCRIPTION:
  Fetches the flag that indicates if entry for the specified index is a
  GSMA entry.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, this function always returns TRUE since EFS entries are per GSMA.
  If not, GSMA entry flag (of that index) is fetched from the statically
    defined table.

RETURN VALUE
  BOOLEAN

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_se13_table_is_gsma_entry
(
  uint16           index
)
{
  uint16    num_of_entries  = 0;
  boolean   is_gsma_entry     = FALSE;
  boolean   is_file_present   = FALSE;

  is_file_present = mmgsdi_se13_check_if_file_present();

   /* If SE13 binary is present in EFS, check if the 0x80 bit in
   * the network mask is on or off. If on, this indicates that
   * the entry is from the gsma table. This is per version 1.1. */
  if (is_file_present)
  {
    if (mmgsdi_se13_file_info.se13_header.version_minor >
       MMGSDI_SE13_BINARY_MINOR_VER_0)
    {
      uint32 net_mask = mmgsdi_se13_table_get_network_mask(index);
      if((net_mask & MMGSDI_SE13_IS_GSMA_ENTRY_BIT)
        == MMGSDI_SE13_IS_GSMA_ENTRY_BIT)
      {
        is_gsma_entry = TRUE;
      }
    }
    else
    {
      is_gsma_entry = TRUE;
    }
  }
#ifndef FEATURE_MMGSDI_SE13_FROM_EFS_ONLY
  else
  {
    num_of_entries = sizeof(mmgsdi_network_table)/
                       sizeof(mmgsdi_eons_network_info_data_type);
    if (index < num_of_entries)
    {
      is_gsma_entry = mmgsdi_network_table[index].gsma_entry;
    }
  }
#endif /* FEATURE_MMGSDI_SE13_FROM_EFS_ONLY */

  return is_gsma_entry;
} /* mmgsdi_se13_table_is_gsma_entry */


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_GET_ENTRY

DESCRIPTION:
  Fetches the entire PLMN entry for the specified index from the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, PLMN entry (of that index) is fetched from this file.
  If not, PLMN entry (of that index) is fetched from the statically
    defined table.

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type    mmgsdi_se13_table_get_entry
(
  uint16                                  index,
  mmgsdi_eons_network_info_data_type    * info_ptr
)
{
  uint8                      short_name_len    = 0;
  uint8                      full_name_len     = 0;
  uint16                     num_of_entries    = 0;
  char                     * short_name_ptr    = NULL;
  char                     * full_name_ptr     = NULL;
  char                     * dynamic_names_ptr = NULL;
  boolean                    is_file_present   = FALSE;

  if (info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  is_file_present = mmgsdi_se13_check_if_file_present();

  /* Now get the name length & addresses
     If SE13 binary file is present in EFS, we need to now dynamically
     read names from EFS */
  if (is_file_present)
  {
    int        fd                 = 0;
    uint16     dynamic_names_len  = 0;
    fs_off_t   efs_file_ret       = 0;
    fs_off_t   efs_file_offset    = 0;

    num_of_entries = mmgsdi_se13_file_info.se13_header.num_entries;
    if (mmgsdi_se13_file_info.plmn_list_ptr == NULL ||
        index >= num_of_entries)
    {
      UIM_SELECTIVE_MSG_ERR_2(UIM_LOG_EONS,
                              "Invalid plmn_list_ptr 0x%x or index 0x%x",
                              mmgsdi_se13_file_info.plmn_list_ptr,
                              index);
      return MMGSDI_ERROR;
    }
    dynamic_names_len = (mmgsdi_se13_file_info.plmn_list_ptr + index)->short_name_len +
                        (mmgsdi_se13_file_info.plmn_list_ptr + index)->long_name_len;

    fd = mcfg_fopen(MMGSDI_SE13_TABLE_NV_PATH, MCFG_FS_O_RDONLY,
                    MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE);
    if (fd < 0)
    {
      return MMGSDI_ERROR;
    }

    /* Calculate the file offset to seek to, which is header + PLMN list + offset
        specified in the indexed PLMN entry */
    efs_file_offset = sizeof(mmgsdi_se13_header_type) +
                      mmgsdi_se13_file_info.plmn_list_len +
                      (mmgsdi_se13_file_info.plmn_list_ptr + index)->name_offset;

    efs_file_ret = mcfg_lseek(fd, efs_file_offset, MCFG_SEEK_SET, MCFG_FS_TYPE_EFS);
    if (efs_file_ret != efs_file_offset)
    {
      return MMGSDI_ERROR;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(dynamic_names_ptr,
                                       dynamic_names_len);
    if (dynamic_names_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    efs_file_ret = mcfg_fread(fd, (void *) dynamic_names_ptr,
                              dynamic_names_len, MCFG_FS_TYPE_EFS);
    if (efs_file_ret != dynamic_names_len)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(dynamic_names_ptr);
      return MMGSDI_ERROR;
    }

    /* Close the file descriptor */
    (void)mcfg_fclose(fd, MCFG_FS_TYPE_EFS);

    info_ptr->mcc = (mmgsdi_se13_file_info.plmn_list_ptr + index)->mcc;
    info_ptr->mnc = (mmgsdi_se13_file_info.plmn_list_ptr + index)->mnc;
    info_ptr->network_type_mask = (mmgsdi_se13_file_info.plmn_list_ptr + index)->rat_mask;
    info_ptr->gsma_entry = TRUE; /* PLMN name extracted from EFS */
    short_name_len = (mmgsdi_se13_file_info.plmn_list_ptr + index)->short_name_len + 1;
    full_name_len  = (mmgsdi_se13_file_info.plmn_list_ptr + index)->long_name_len + 1;
    short_name_ptr = dynamic_names_ptr;
    full_name_ptr  = dynamic_names_ptr + short_name_len - 1;
  }
#ifndef FEATURE_MMGSDI_SE13_FROM_EFS_ONLY
  /* If not, copy names from static table */
  else
  {
    num_of_entries = sizeof(mmgsdi_network_table)/
                       sizeof(mmgsdi_eons_network_info_data_type);
    if (index >= num_of_entries)
    {
      UIM_SELECTIVE_MSG_ERR_1(UIM_LOG_EONS, "Invalid index 0x%x", index);
      return MMGSDI_ERROR;
    }
    info_ptr->mcc = mmgsdi_network_table[index].mcc;
    info_ptr->mnc = mmgsdi_network_table[index].mnc;
    info_ptr->network_type_mask = mmgsdi_network_table[index].network_type_mask;
    info_ptr->gsma_entry = mmgsdi_network_table[index].gsma_entry;
    short_name_len = strlen(mmgsdi_network_table[index].short_name_ptr) + 1;
    full_name_len  = strlen(mmgsdi_network_table[index].full_name_ptr) + 1;
    short_name_ptr = mmgsdi_network_table[index].short_name_ptr;
    full_name_ptr  = mmgsdi_network_table[index].full_name_ptr;
  }
#endif /* FEATURE_MMGSDI_SE13_FROM_EFS_ONLY */

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(info_ptr->short_name_ptr,
                                     short_name_len);
  if (info_ptr->short_name_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(dynamic_names_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(info_ptr->full_name_ptr,
                                     full_name_len);
  if (info_ptr->full_name_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(info_ptr->short_name_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(dynamic_names_ptr);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  memset(info_ptr->short_name_ptr, 0, short_name_len);
  memset(info_ptr->full_name_ptr, 0, full_name_len);

  /* Now copy both the names */
  mmgsdi_memscpy(info_ptr->short_name_ptr, short_name_len,
                 short_name_ptr, short_name_len - 1);
  mmgsdi_memscpy(info_ptr->full_name_ptr, full_name_len,
                 full_name_ptr, full_name_len - 1);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(dynamic_names_ptr);
  UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS, "Is PLMN Entry fetched from SE13 present in EFS : 0x%x",
                 is_file_present);

  return MMGSDI_SUCCESS;
} /* mmgsdi_se13_table_get_entry */


/*===========================================================================
FUNCTION MMGSDI_SE13_PROCESS_GET_SE13_PLMN_INFO_BY_NAME

DESCRIPTION
  This function queues a response for handling get se13 plmn info by name in
  the cnf

PARAMETERS:
  mmgsdi_get_se13_plmn_info_by_name_req_type req_ptr: request pointer

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_se13_process_get_se13_plmn_info_by_name(
  mmgsdi_get_se13_plmn_info_by_name_req_type * req_ptr
)
{
  mmgsdi_sw_status_type               status_word;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  memset(&status_word, 0x00, sizeof(mmgsdi_sw_status_type));

  return mmgsdi_cnf_build_and_queue(MMGSDI_SUCCESS,
                                             &req_ptr->request_header,
                                             NULL,
                                             (const void *)req_ptr,
                                             FALSE,
                                             status_word);
}/* mmgsdi_se13_process_get_se13_plmn_info_by_name */

/*===========================================================================
FUNCTION MMGSDI_EONS_PROCESS_GET_SE13_PLMN_NAMES

DESCRIPTION
  This function queues a response for handling get se13 plmn names in the cnf

PARAMETERS:
  mmgsdi_get_se13_plmn_names_req_type req_ptr: request pointer

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_process_get_se13_plmn_names(
  mmgsdi_get_se13_plmn_names_req_type * req_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_client_req_extra_info_type  *extra_param_ptr  = NULL;
  mmgsdi_sw_status_type               status_word;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  memset(&status_word, 0x00, sizeof(mmgsdi_sw_status_type));

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
    MMGSDI_GET_SE13_PLMN_NAMES_REQ, (void*)req_ptr, &extra_param_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_SUCCESS,
                                             &req_ptr->request_header,
                                             extra_param_ptr, NULL,
                                             FALSE, status_word);

  /* Normally, if we had posted a response to ourselves and had built this cnf
     as part of mmgsdi_process_response() handling, extra_param_ptr would
     get free'd up as part of response processing via function
     mmgsdi_util_free_client_request_table_index(). Since we are directly
     building the cnf here itself (and not going the process_response()
     route, we need to free the extra_param_ptr that we malloc'ed above */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr->get_se13_plmn_names_data.plmn_id_list.plmn_list_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
  return mmgsdi_status;
}/* mmgsdi_eons_process_get_se13_plmn_names */


/*==========================================================================
FUNCTION MMGSDI_EONS_SYS_MODE_NETWORK_TYPE_MATCH

DESCRIPTION
  Function to compare sys mode with network types

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
static boolean mmgsdi_eons_sys_mode_network_type_match(
  sys_sys_mode_e_type              sys_mode,
  uint32                           network_mask)
{
  boolean is_match_found      = FALSE;

  if(sys_mode == SYS_SYS_MODE_GSM||
     sys_mode == SYS_SYS_MODE_GW||
     sys_mode == SYS_SYS_MODE_GWL)
  {
    if((network_mask &
        MMGSDI_PLMN_NETWK_TYPE_GSM_900_MASK)||
       (network_mask &
        MMGSDI_PLMN_NETWK_TYPE_DCS_1800_MASK)||
       (network_mask &
        MMGSDI_PLMN_NETWK_TYPE_PCS_1900_MASK))
    {
      is_match_found = TRUE;
    }
  }
  else if(sys_mode == SYS_SYS_MODE_WCDMA)
  {
    if((network_mask &
        MMGSDI_PLMN_NETWK_TYPE_UMTS_MASK))
    {
      is_match_found = TRUE;
    }
  }
  else if(sys_mode == SYS_SYS_MODE_LTE)
  {
    if((network_mask &
        MMGSDI_PLMN_NETWK_TYPE_LTE_MASK))
    {
      is_match_found = TRUE;
    }
  }
  else
  {
    is_match_found = TRUE;
  }

  return is_match_found;
}/*mmgsdi_eons_sys_mode_network_type_match*/


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_NETWORK_INFO_FROM_SE13

DESCRIPTION
  Function to get network info from the SE13 name table.
  MNC and MCC value(extracted from PLMN ID)should be
  matched with table's record

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
static boolean mmgsdi_eons_get_network_info_from_se13(
  mmgsdi_plmn_id_type                   plmn,
  sys_sys_mode_e_type                   sys_mode,
  mmgsdi_eons_network_info_data_type  * info_ptr)
{
  uint16                  middleIndex      = 0;
  uint16                  endIndex         = 0;
  uint16                  startIndex       = 0;
  uint16                  mnc              = 0;
  uint16                  mcc              = 0;
  uint16                  match_index      = 0;
  uint16                  num_of_entries   = 0;
  boolean                 is_match_found   = FALSE;
  boolean                 is_mcc_mnc_found = FALSE;
  mmgsdi_return_enum_type mmgsdi_status    = MMGSDI_ERROR;
  uint16                  index            = 0;
  boolean                 increase_index   = FALSE;

  /* First fetch the number of entries in teh SE13 table */
  num_of_entries = mmgsdi_se13_table_get_num_entries();
  if(num_of_entries == 0)
  {
    return FALSE;
  }

  mmgsdi_status = mmgsdi_eons_plmn_get_mcc_mnc(plmn, &mcc, &mnc);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return is_match_found;
  }

  /* If NW PLMNID's MCC is in range of 310-316 (BCCH MCC) and MNC's third
     digit is 'F', multiply MNC with 10 */
  if(mcc >= 310 && mcc <= 316 && ((plmn.plmn_id_val[1] & 0xF0) == 0xF0))
  {
    mnc = mnc * 10;
  }

  endIndex = num_of_entries - 1;

  while(startIndex <= endIndex)
  {
    middleIndex = (endIndex + startIndex)/2;
    if(mcc ==  mmgsdi_se13_table_get_mcc(middleIndex))
    {
      is_match_found = TRUE;
      break;
    }
    else if (mcc <  mmgsdi_se13_table_get_mcc(middleIndex))
    {
      /*break if first entry of table is checked*/
      if(middleIndex == 0)
      {
        break;
      }
      endIndex = middleIndex - 1;
    }
    else
    {
      /*break if last entry of table is checked*/
      if(middleIndex == num_of_entries - 1)
      {
        break;
      }
      startIndex = middleIndex + 1;
    }
  }

  if(is_match_found == FALSE)
  {
    UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS,
                             "mnc, mcc or network does not match in table");
    return FALSE;
  }

  index = middleIndex;
  is_match_found = FALSE;

  if(mmgsdi_se13_table_get_mnc(index) < mnc)
  {
    increase_index = TRUE;
  }

  while((index < num_of_entries) &&
        (mcc ==  mmgsdi_se13_table_get_mcc(index)))
  {
    uint16 mnc_at_index = mmgsdi_se13_table_get_mnc(index);

    if(mnc == mnc_at_index)
    {
      is_match_found   = TRUE;
      match_index      = index;
      is_mcc_mnc_found = TRUE;
      break;
    }

    if(increase_index)
    {
      if(index == num_of_entries - 1)
      {
        break;
      }
      index++;
    }
    else
    {
      if(index == 0)
      {
        break;
      }
      index--;
    }
  }

  if(is_match_found &&
     sys_mode != SYS_SYS_MODE_NONE &&
     sys_mode != SYS_SYS_MODE_NO_SRV)
  {
    is_match_found = FALSE;
    do
    {
      if((index < num_of_entries) &&
         (TRUE == mmgsdi_eons_sys_mode_network_type_match(sys_mode,
                    mmgsdi_se13_table_get_network_mask(index))))
      {
        is_match_found = TRUE;
        break;
      }

      /*if sys mode match is not found at index and we are not at last entry
        of table, increase index by one and check for sys mode match when MCC
        and MNC matches*/
      if(index == num_of_entries - 1)
      {
        break;
      }

      index = index + 1;
    }
    while((index < num_of_entries) &&
          (mcc ==  mmgsdi_se13_table_get_mcc(index)) &&
          (mnc ==  mmgsdi_se13_table_get_mnc(index)));

    /*if sys mode match is not found at match_index/below entries and we are not at first entry
      of table, decrease index by one and check for sys mode match when MCC
      and MNC matches*/
    if(is_match_found == FALSE  && match_index > 0 &&
       mcc ==  mmgsdi_se13_table_get_mcc(match_index - 1) &&
       mnc ==  mmgsdi_se13_table_get_mnc(match_index - 1))
    {
      index = match_index - 1;

      do
      {
        if(TRUE == mmgsdi_eons_sys_mode_network_type_match(sys_mode,
                     mmgsdi_se13_table_get_network_mask(index)))
        {
          is_match_found = TRUE;
          break;
        }

        if(index == 0)
        {
          break;
        }

        index = index - 1;
      }
      while(mcc ==  mmgsdi_se13_table_get_mcc(index) &&
            mnc ==  mmgsdi_se13_table_get_mnc(index));
    }
  }

  if(is_match_found == FALSE && is_mcc_mnc_found)
  {
    /* we are here because MCC-MNC match found but network type match didnot.
       check if there is any entry present just below and above match_index
       with same MCC-MNC at match index. */
    is_match_found = TRUE;

    if((match_index > 0) &&
       (mcc ==  mmgsdi_se13_table_get_mcc(match_index - 1)) &&
       (mnc ==  mmgsdi_se13_table_get_mnc(match_index - 1)))
    {
      is_match_found = FALSE;
    }

    if(is_match_found &&
       (match_index < (num_of_entries - 1)) &&
       (mcc ==  mmgsdi_se13_table_get_mcc(match_index + 1)) &&
       (mnc ==  mmgsdi_se13_table_get_mnc(match_index + 1)))
    {
      is_match_found = FALSE;
    }

    /* If MCC-MNC record is present but network type cannot be matched,
       use the MCC-MNC record only if -->There is only a single network name
       entry in the SE-13 table corresponding to the MCC-MNC */
    if(is_match_found)
    {
      index = match_index;
    }
    else
    {
      /*We are here because there are more than one entries with same MCC-MNC
        if among these entries, one entry (and only one) is from GSMA Table
        return PLMN name from that one*/
      boolean is_single_gsma_entry   = FALSE;
      boolean continue_search        = TRUE;
      uint16  single_match_index      = 0;
      uint16  first_gsma_match_index  = 0xFFFF;

      index = match_index;

      /*check if matched entry is part of GSMA table*/
      if(mmgsdi_se13_table_is_gsma_entry(index))
      {
        is_single_gsma_entry = TRUE;
        single_match_index = index;
        first_gsma_match_index = index;
      }

      /*Search below the match index*/
      do
      {
        if(index == num_of_entries - 1)
        {
          break;
        }

        index = index + 1;

        if((mcc ==  mmgsdi_se13_table_get_mcc(index)) &&
           (mnc ==  mmgsdi_se13_table_get_mnc(index)))
        {
          if(mmgsdi_se13_table_is_gsma_entry(index))
          {
            if(is_single_gsma_entry)
            {
              /*More than one entries are from GSMA table, stop lookup*/
              is_single_gsma_entry = FALSE;

              /*No need to continue search the entries above match_index*/
              continue_search = FALSE;
              break;
            }
            else
            {
              is_single_gsma_entry = TRUE;
              single_match_index = index;
              first_gsma_match_index = index;
            }
          }
        }
        else
        {
          break;
        }
      }
      while(index < num_of_entries);

      /*check if we need to search entries above match_index*/
      if(match_index > 0 &&
         (continue_search ||
          mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SE13_TABLE_LOOKUP_GET_FIRST_GSMA_MATCH, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
            MMGSDI_FEATURE_ENABLED))
      {
        index = match_index;

        /*Search above the match index*/
        do
        {
          index = index - 1;
          if((mcc ==  mmgsdi_se13_table_get_mcc(index)) &&
             (mnc ==  mmgsdi_se13_table_get_mnc(index)))
          {
            if(mmgsdi_se13_table_is_gsma_entry(index))
            {
              first_gsma_match_index = index;
              if(is_single_gsma_entry)
              {
                /*More than one entries are from GSMA table, stop lookup*/
                is_single_gsma_entry = FALSE;

                /*dont break if GET_FIRST_GSMA_MATCH feature is enabled, keep searching*/
                if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SE13_TABLE_LOOKUP_GET_FIRST_GSMA_MATCH, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
                     MMGSDI_FEATURE_DISABLED)
                {
                  break;
                }
              }
              else
              {
                is_single_gsma_entry = TRUE;
                single_match_index = index;
              }
            }
          }
          else
          {
            break;
          }
        }
        while(index > 0);
      }

      if(is_single_gsma_entry)
      {
        index = single_match_index;
        is_match_found = TRUE;
      }
      else if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SE13_TABLE_LOOKUP_GET_FIRST_GSMA_MATCH, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
              MMGSDI_FEATURE_ENABLED)
      {
        if(first_gsma_match_index  != 0xFFFF)
        {
          index = first_gsma_match_index;
          is_match_found = TRUE;
        }
      }
    }
  }

  if(is_match_found)
  {
    if (mmgsdi_se13_table_get_entry(index, info_ptr) != MMGSDI_SUCCESS)
    {
      is_match_found = FALSE;
    }
  }

  UIM_SELECTIVE_MSG_HIGH_5(UIM_LOG_EONS,
                           "is SE13 match found: 0x%x for search network type: 0x%x found at index: 0x%x mcc: 0x%x mnc: 0x%x",
                           is_match_found, sys_mode, index, mcc, mnc);

  return is_match_found;
}/*mmgsdi_eons_get_network_info_from_se13*/


/*==========================================================================
FUNCTION MMGSDI_EONS_CONVERT_ASCII_TO_7BIT_UNPACKED

DESCRIPTION
  Function to convert char encoding from ascii to GSM 7bit default unpacked

  Range of GSM 7bit default unpacked and ASCII encoding: 0 - 127

  ASCII 0 - 31 : Not used in SE13 table
  ASCII 127    : Delete         : Not used in SE13 table
  ASCII 36     : '$'            : GSM_7BIT_DEFAULT_UNPACKED 2
  ASCII 64     : '@'            : GSM_7BIT_DEFAULT_UNPACKED 0
  ASCII 95     : '_'            : GSM_7BIT_DEFAULT_UNPACKED 17
  ASCII 96     : ` Grave Accent : No equivqlent in GSM_7BIT_DEFAULT_UNPACKED,
                                  mapping it to APOSTROPHE (')
  All other characters are same in ASCII and GSM 7bit default unpacked encoding

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
static void mmgsdi_eons_convert_ascii_to_7bit_unpacked(
  mmgsdi_data_type * eons_data_ptr
)
{
  int32 loop_count = 0;

  if(eons_data_ptr == NULL)
  {
    return;
  }

  if(eons_data_ptr->data_len <= 0 || eons_data_ptr->data_ptr == NULL)
  {
    return;
  }

  for (loop_count = 0; loop_count < eons_data_ptr->data_len; loop_count++)
  {
    switch(eons_data_ptr->data_ptr[loop_count])
    {
      case '$':
        eons_data_ptr->data_ptr[loop_count] = 0x02;
        break;
      case '@':
        eons_data_ptr->data_ptr[loop_count] = 0x00;
        break;
      case '_':
        eons_data_ptr->data_ptr[loop_count] = 0x11;
        break;
      case '`':
        eons_data_ptr->data_ptr[loop_count] = 0x27;
        break;
      default :
        break;
    }
  }
}/* mmgsdi_eons_convert_ascii_to_7bit_unpacked */


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_SE13_TABLE_ONS

DESCRIPTION
  Function to get operator name string from the SE13 table

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_get_se13_table_ons(
  mmgsdi_plmn_info_type * plmn_info_ptr,
  mmgsdi_plmn_id_type     plmn_id,
  sys_sys_mode_e_type     sys_mode
)
{
  boolean                              is_ons_retrieved = FALSE;
  mmgsdi_eons_network_info_data_type   network_info     = {0};

  if (plmn_info_ptr == NULL)
  {
    return FALSE;
  }

  if ((TRUE == mmgsdi_eons_get_network_info_from_se13(
                 plmn_id, sys_mode, &network_info)) &&
       ((network_info.full_name_ptr  != NULL) &&
        (network_info.short_name_ptr != NULL)))
  {
    uint8 length                          = 0;

    length = strlen(network_info.full_name_ptr);

    plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len =
      length;

    plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding =
      MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_UNPACKED;

    if(length > 0)
    {
      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr =
        (uint8 *)network_info.full_name_ptr;
      network_info.full_name_ptr = NULL;
      is_ons_retrieved = TRUE;

      mmgsdi_eons_convert_ascii_to_7bit_unpacked(
        &plmn_info_ptr->plmn_long_name.plmn_name.eons_data);
    }

    length = strlen(network_info.short_name_ptr);

    plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len =
      length;

    plmn_info_ptr->plmn_short_name.plmn_name.eons_encoding =
      MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_UNPACKED;

    if(length > 0)
    {
      plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr =
        (uint8 *)network_info.short_name_ptr;
      network_info.short_name_ptr = NULL;
      is_ons_retrieved = TRUE;

      mmgsdi_eons_convert_ascii_to_7bit_unpacked(
        &plmn_info_ptr->plmn_short_name.plmn_name.eons_data);
    }
  }

  return is_ons_retrieved;
}/* mmgsdi_eons_get_se13_table_ons */


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_SE13_ADD_LANG_TABLE_ONS

DESCRIPTION
  Function to get operator name string from the SE13 additional languages
  table

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_get_se13_addl_lang_table_ons(
  mmgsdi_plmn_info_type * plmn_info_ptr,
  mmgsdi_plmn_id_type     plmn_id,
  sys_sys_mode_e_type     sys_mode
)
{
  uint8                             index            = 0;
  uint16                            mcc              = 0;
  uint16                            mnc              = 0;
  mmgsdi_return_enum_type           mmgsdi_status    = MMGSDI_ERROR;
  uint16                            num_of_entries   = 0;

  if(plmn_info_ptr == NULL)
  {
    return FALSE;
  }

  /* First fetch the number of entries in the additional languagaes
     SE13 table */
  num_of_entries = mmgsdi_addl_lang_se13_table_get_num_entries();
  if(num_of_entries == 0)
  {
    UIM_MSG_MED_0("Additional language SE13 table is empty");
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_eons_plmn_get_mcc_mnc(plmn_id, &mcc, &mnc);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* If NW PLMNID's MCC is in range of 310-316 (BCCH MCC) and MNC's third
     digit is 'F', multiply MNC with 10 */
  if(mcc >= 310 && mcc <= 316 && ((plmn_id.plmn_id_val[1] & 0xF0) == 0xF0))
  {
    mnc = mnc * 10;
  }

  plmn_info_ptr->num_plmn_additional_names = 0;

  for(index = 0; index < num_of_entries && plmn_info_ptr->num_plmn_additional_names < MMGSDI_ADDITIONAL_NAMES_MAX; index ++)
  {
    if(mcc == mmgsdi_addl_lang_network_table[index].mcc &&
       mnc == mmgsdi_addl_lang_network_table[index].mnc &&
       TRUE == mmgsdi_eons_sys_mode_network_type_match(sys_mode,
                                                       mmgsdi_addl_lang_network_table[index].network_type_mask))
    {
      /* Copy the contents from hardcoded table */
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_lang.lang_len =
        strlen(mmgsdi_addl_lang_network_table[index].lang_id);
      mmgsdi_memscpy(plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_lang.lang_id,
                     MMGSDI_LANG_ID_LEN_MAX,
                     mmgsdi_addl_lang_network_table[index].lang_id,
                     plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_lang.lang_len);
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_short_name.plmn_name.eons_data.data_len =
        mmgsdi_addl_lang_network_table[index].short_name_ucs2_len;
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_short_name.plmn_name.eons_data.data_ptr =
        mmgsdi_addl_lang_network_table[index].short_name_ucs2;
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_short_name.plmn_name.eons_encoding =
      MMGSDI_EONS_ENC_UCS2;
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_long_name.plmn_name.eons_data.data_len =
        mmgsdi_addl_lang_network_table[index].full_name_ucs2_len;
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_long_name.plmn_name.eons_data.data_ptr =
        mmgsdi_addl_lang_network_table[index].full_name_ucs2;
      plmn_info_ptr->plmn_additional_names[plmn_info_ptr->num_plmn_additional_names].plmn_long_name.plmn_name.eons_encoding =
      MMGSDI_EONS_ENC_UCS2;

      plmn_info_ptr->num_plmn_additional_names = plmn_info_ptr->num_plmn_additional_names + 1;
    }
  }

  return TRUE;
}/* mmgsdi_eons_get_se13_addl_lang_table_ons */


/* ============================================================================
   FUNCTION       MMGSDI_EONS_GET_SE13_PLMN_NAMES

   DESCRIPTION
     This function returns plmn names from SE13 table for provided plmn id list.

   PARAMETERS:
     mmgsdi_plmn_id_list_type plmn_id_list: list of PLMN ids
     mmgsdi_list_plmn_name_type plmn_name_list_ptr: list of plmn long/short name

   DEPENDENCIES:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS: if plmn names are retrieved successfully.

     MMGSDI_ERROR: any error

   SIDE EFFECTS:
     None
===============================================================================*/
mmgsdi_return_enum_type  mmgsdi_eons_get_se13_plmn_names (
  mmgsdi_plmn_id_list_type      plmn_id_list,
  mmgsdi_list_plmn_name_type  * plmn_name_list_ptr
)
{
  uint32                    loop_count    = 0;
  sys_sys_mode_e_type       sys_mode      = SYS_SYS_MODE_MAX;
  mmgsdi_plmn_id_type       plmn_id;

  memset(&plmn_id, 0x00, sizeof(mmgsdi_plmn_id_type));

  if(plmn_id_list.plmn_list_ptr == NULL || plmn_id_list.num_of_plmn_ids == 0 ||
     plmn_name_list_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  plmn_name_list_ptr->num_of_plmn_ids = plmn_id_list.num_of_plmn_ids;

  /*do memory allocation for plmn name*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(plmn_name_list_ptr->plmn_info_ptr,
    (sizeof(mmgsdi_plmn_info_type) * (plmn_id_list.num_of_plmn_ids)));
  if (plmn_name_list_ptr->plmn_info_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  loop_count = 0;

  /*run do while loop for plmn list provided*/
  do
  {
    mmgsdi_memscpy(plmn_id.plmn_id_val,
      sizeof(plmn_id.plmn_id_val),
      plmn_id_list.plmn_list_ptr[loop_count].plmn_id_val,
      MMGSDI_PLMN_ID_SIZE);

    plmn_id.rat = plmn_id_list.plmn_list_ptr[loop_count].rat;

    UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                             "Get ONS for plmn id: 0x%x, 0x%x, 0x%x",
                             plmn_id.plmn_id_val[0],
                             plmn_id.plmn_id_val[1],
                             plmn_id.plmn_id_val[2]);

    mmgsdi_memscpy(plmn_name_list_ptr->plmn_info_ptr[loop_count].plmn_id.plmn_id_val,
      sizeof(plmn_name_list_ptr->plmn_info_ptr[loop_count].plmn_id.plmn_id_val),
      plmn_id.plmn_id_val,
      MMGSDI_PLMN_ID_SIZE);

    /*copy RAT type in output*/
    plmn_name_list_ptr->plmn_info_ptr[loop_count].plmn_id.rat = plmn_id.rat;

    /*set lac to 0xFFFF*/
    plmn_name_list_ptr->plmn_info_ptr[loop_count].lac = 0xFFFF;

    sys_mode = mmgsdi_eons_map_rat_type_to_sys_mode(plmn_id.rat);

    if(FALSE == mmgsdi_eons_get_se13_table_ons(
                 &(plmn_name_list_ptr->plmn_info_ptr[loop_count]),
                 plmn_id,
                 sys_mode))
    {
      memset(&plmn_name_list_ptr->plmn_info_ptr[loop_count].plmn_long_name,
             0x00,
             sizeof(mmgsdi_plmn_name_type));

      memset(&plmn_name_list_ptr->plmn_info_ptr[loop_count].plmn_short_name,
             0x00,
             sizeof(mmgsdi_plmn_name_type));
    }

    /* It is very possible that there was no English name found in the
       regular hardcoded SE13 table above but the additional language
       name was found in the addl_lang_SE13 hardcoded table. In that case,
       the plmn_info_ptr will only have valid additional language
       PLMN names and no english language names from the regular SE13 table */
    (void) mmgsdi_eons_get_se13_addl_lang_table_ons(
             &(plmn_name_list_ptr->plmn_info_ptr[loop_count]),
             plmn_id,
             sys_mode);

    loop_count++;
  }
  while(loop_count < plmn_id_list.num_of_plmn_ids);

  return MMGSDI_SUCCESS;
}/* mmgsdi_eons_get_se13_plmn_names */


/* ============================================================================
FUNCTION       MMGSDI_EONS_GET_SE13_PLMN_INFO_BY_NAME

DESCRIPTION
  This function returns plmn info from SE13 table for given long/short name

PARAMETERS:
  mmgsdi_get_se13_plmn_info_by_name_req_type
    se13_plmn_info_by_name_req_ptr: long name, short name and RAT info
  mmgsdi_get_se13_plmn_info_by_name_cnf_type se13_plmn_info_by_name_cnf_ptr:
    cnf data ptr

DEPENDENCIES:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS: if plmn info is retrieved successfully.

  MMGSDI_ERROR: any error

SIDE EFFECTS:
  None
===============================================================================*/
mmgsdi_return_enum_type  mmgsdi_eons_get_se13_plmn_info_by_name(
  const mmgsdi_get_se13_plmn_info_by_name_req_type *se13_plmn_info_by_name_req_ptr,
  mmgsdi_get_se13_plmn_info_by_name_cnf_type       *se13_plmn_info_by_name_cnf_ptr
)
{
  uint16                              index              = 0;
  mmgsdi_return_enum_type             mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_se13_plmn_node_type         *list_ptr           = NULL;
  mmgsdi_se13_plmn_node_type         *node_ptr           = NULL;
  mmgsdi_se13_plmn_node_type         *temp_ptr           = NULL;
  uint16                              num_se13_entries   = 0;
  uint16                              node_count         = 0;
  boolean                             match_found        = TRUE;
  sys_sys_mode_e_type                 sys_mode           = SYS_SYS_MODE_NONE;
  mmgsdi_eons_network_info_data_type  network_info_data;

  UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "Get SE13 PLMN info by name");

  MMGSDIUTIL_RETURN_IF_NULL_2(se13_plmn_info_by_name_req_ptr,
                              se13_plmn_info_by_name_cnf_ptr);

  memset(&network_info_data, 0x00, sizeof(mmgsdi_eons_network_info_data_type));

  if(se13_plmn_info_by_name_req_ptr->plmn_long_name.data_len == 0 &&
     se13_plmn_info_by_name_req_ptr->plmn_short_name.data_len == 0)
  {
    return MMGSDI_ERROR;
  }
  num_se13_entries = mmgsdi_se13_table_get_num_entries();

  sys_mode =
    mmgsdi_eons_map_rat_type_to_sys_mode(se13_plmn_info_by_name_req_ptr->rat_type);

  for(index = 0; index < num_se13_entries; index++)
  {
    memset(&network_info_data, 0x00, sizeof(mmgsdi_eons_network_info_data_type));
    mmgsdi_status = mmgsdi_se13_table_get_entry (index, &network_info_data);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      continue;
    }

    match_found = TRUE;
    if(se13_plmn_info_by_name_req_ptr->rat_type > MMGSDI_RAT_NONE &&
       se13_plmn_info_by_name_req_ptr->rat_type <= MMGSDI_RAT_TDS)
    {
      if(!mmgsdi_eons_sys_mode_network_type_match(
           sys_mode,
           network_info_data.network_type_mask))
      {
        match_found = FALSE;
      }
    }

    if(match_found &&
       (se13_plmn_info_by_name_req_ptr->plmn_long_name.data_len > 0) &&
       (se13_plmn_info_by_name_req_ptr->plmn_long_name.data_ptr != NULL))
    {
      if((network_info_data.full_name_ptr != NULL) &&
         (strlen(network_info_data.full_name_ptr) ==
          se13_plmn_info_by_name_req_ptr->plmn_long_name.data_len))
      {
        if(memcmp(se13_plmn_info_by_name_req_ptr->plmn_long_name.data_ptr,
                  network_info_data.full_name_ptr,
                  se13_plmn_info_by_name_req_ptr->plmn_long_name.data_len))
        {
          match_found = FALSE;
        }
      }
      else
      {
        match_found = FALSE;
      }
    }

    if(match_found &&
       (se13_plmn_info_by_name_req_ptr->plmn_short_name.data_len > 0) &&
       (se13_plmn_info_by_name_req_ptr->plmn_short_name.data_ptr != NULL))
    {
      if((network_info_data.short_name_ptr!= NULL) &&
          (strlen(network_info_data.short_name_ptr) ==
           se13_plmn_info_by_name_req_ptr->plmn_short_name.data_len))
      {
        if(memcmp(se13_plmn_info_by_name_req_ptr->plmn_short_name.data_ptr,
              network_info_data.short_name_ptr,
              se13_plmn_info_by_name_req_ptr->plmn_short_name.data_len))
        {
          match_found = FALSE;
        }
      }
      else
      {
        match_found = FALSE;
      }
    }

    if(match_found)
    {
      /* Add node to linked list */

      /*Create a new node*/
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(node_ptr,
         sizeof(mmgsdi_se13_plmn_node_type));

      if(node_ptr != NULL)
      {
        node_count++;
        memset(node_ptr, 0x00, sizeof(mmgsdi_se13_plmn_node_type));

        node_ptr->mcc = network_info_data.mcc;
        node_ptr->mnc = network_info_data.mnc;
        node_ptr->network_mask = network_info_data.network_type_mask;

        /* Copy long name */
        if(network_info_data.full_name_ptr != NULL)
        {
          node_ptr->plmn_long_name.data_len =
            strlen(network_info_data.full_name_ptr);
          node_ptr->plmn_long_name.data_ptr =
            (uint8 *)network_info_data.full_name_ptr;
          network_info_data.full_name_ptr = NULL;
        }

        /* Copy short name */
        if(network_info_data.short_name_ptr != NULL)
        {
          node_ptr->plmn_short_name.data_len =
            strlen(network_info_data.short_name_ptr);
          node_ptr->plmn_short_name.data_ptr =
            (uint8 *)network_info_data.short_name_ptr;
          network_info_data.short_name_ptr = NULL;
        }

        /* Add the node to linked list */
        if(list_ptr == NULL)
        {
          list_ptr = node_ptr;
        }
        else
        {
          temp_ptr = list_ptr;
          while(temp_ptr->next_ptr != NULL)
          {
            temp_ptr = temp_ptr->next_ptr;
          }
          temp_ptr->next_ptr = node_ptr;
        }
      }
    }

    /* Free if full & short name pointers were allocated */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(network_info_data.full_name_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(network_info_data.short_name_ptr);
  }

  /* Initialize num_of_plmn_ids with 0 */
  se13_plmn_info_by_name_cnf_ptr->num_of_plmn_ids = 0;

  if (list_ptr == NULL)
  {
    return MMGSDI_SUCCESS;
  }

  /* Allocate memory for plmn info ptr in  cnf ptr*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    se13_plmn_info_by_name_cnf_ptr->se13_plmn_info_ptr,
    sizeof(mmgsdi_se13_plmn_info_type) * node_count);

  if(se13_plmn_info_by_name_cnf_ptr->se13_plmn_info_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  node_ptr = list_ptr;

  /* Run a loop to copy all nodes from linked list to cnf ptr */
  for(index = 0; (index < node_count) && (node_ptr != NULL); index++)
  {
    se13_plmn_info_by_name_cnf_ptr->num_of_plmn_ids++;
    temp_ptr = node_ptr;
    node_ptr = node_ptr->next_ptr;

    se13_plmn_info_by_name_cnf_ptr->se13_plmn_info_ptr[index].mcc = temp_ptr->mcc;
    se13_plmn_info_by_name_cnf_ptr->se13_plmn_info_ptr[index].mnc = temp_ptr->mnc;
    se13_plmn_info_by_name_cnf_ptr->se13_plmn_info_ptr[index].network_mask = temp_ptr->network_mask;

    /* Copy long name */
    se13_plmn_info_by_name_cnf_ptr->se13_plmn_info_ptr[index].plmn_long_name.data_len =
      temp_ptr->plmn_long_name.data_len;
    se13_plmn_info_by_name_cnf_ptr->se13_plmn_info_ptr[index].plmn_long_name.data_ptr =
      temp_ptr->plmn_long_name.data_ptr;
    temp_ptr->plmn_long_name.data_ptr = NULL;

    /* Copy short name */
    se13_plmn_info_by_name_cnf_ptr->se13_plmn_info_ptr[index].plmn_short_name.data_len =
      temp_ptr->plmn_short_name.data_len;
    se13_plmn_info_by_name_cnf_ptr->se13_plmn_info_ptr[index].plmn_short_name.data_ptr =
      temp_ptr->plmn_short_name.data_ptr;
    temp_ptr->plmn_short_name.data_ptr = NULL;
    MMGSDIUTIL_TMC_MEM_FREE(temp_ptr);
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_eons_get_se13_plmn_info_by_name */


/* ============================================================================
FUNCTION       MMGSDI_EONS_GET_SE13_PLMN_INFO_BY_INDEX

DESCRIPTION
  This function returns plmn info from SE13 table for given index

PARAMETERS:
  uint16                        index                : SE13 table index
  mmgsdi_se13_network_info_type se13_network_info_ptr: network info ptr

DEPENDENCIES:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS: if plmn info is retrieved successfully.

  MMGSDI_ERROR: any error

SIDE EFFECTS:
  None
===============================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_get_se13_plmn_info_by_index(
  uint16                          index,
  mmgsdi_se13_network_info_type * se13_network_info_ptr
)
{
  mmgsdi_eons_network_info_data_type network_info_data;
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;
  uint8                              full_name_length  = 0;
  uint8                              short_name_length = 0;

  UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                           "Get SE13 PLMN info by index: 0x%x", index);

  MMGSDIUTIL_RETURN_IF_NULL(se13_network_info_ptr);
  memset(se13_network_info_ptr, 0x00, sizeof(mmgsdi_se13_network_info_type));
  memset(&network_info_data, 0x00, sizeof(mmgsdi_eons_network_info_data_type));

  if(index >= mmgsdi_se13_table_get_num_entries())
  {
    UIM_SELECTIVE_MSG_ERR_0(UIM_LOG_EONS, "Invalid index");
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_se13_table_get_entry (index, &network_info_data);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  se13_network_info_ptr->mcc = network_info_data.mcc;
  se13_network_info_ptr->mnc = network_info_data.mnc;
  se13_network_info_ptr->network_type_mask = network_info_data.network_type_mask;
  se13_network_info_ptr->gsma_entry = network_info_data.gsma_entry;

  if(network_info_data.full_name_ptr != NULL)
  {
    full_name_length = strlen(network_info_data.full_name_ptr);

    if(full_name_length > 0)
    {
      mmgsdi_memscpy(se13_network_info_ptr->long_name,
                     sizeof(se13_network_info_ptr->long_name),
                     network_info_data.full_name_ptr,
                     MIN(full_name_length, MMGSDI_LONG_NAME_MAX_LEN - 1));
    }
  }

  if(network_info_data.short_name_ptr != NULL)
  {
    short_name_length = strlen(network_info_data.short_name_ptr);

    if(short_name_length > 0)
    {
      mmgsdi_memscpy(se13_network_info_ptr->short_name,
                     sizeof(se13_network_info_ptr->short_name),
                     network_info_data.short_name_ptr,
                     MIN(short_name_length, MMGSDI_SHORT_NAME_MAX_LEN - 1));
    }
  }

  UIM_SELECTIVE_MSG_HIGH_4(UIM_LOG_EONS,
                           "length of plmn full name:0x%x Max allowed len:0x%x length of plmn short name:0x%x Max allowed len:0x%x",
                           full_name_length, MMGSDI_LONG_NAME_MAX_LEN,
                           short_name_length, MMGSDI_SHORT_NAME_MAX_LEN);

  /* Free the allocated fulle & short name pointers */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(network_info_data.full_name_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(network_info_data.short_name_ptr);

  return mmgsdi_status;
}/* mmgsdi_eons_get_se13_plmn_info_by_index */


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_SE13_TABLE_DETAILS_INFO

DESCRIPTION
  Function to get the SE13 table release date and source

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_get_se13_table_details_info(
  mmgsdi_se13_table_details_info * se13_table_info_ptr
)
{
  boolean result = FALSE;

  if(se13_table_info_ptr == NULL)
  {
    return result;
  }

  /* If the binary file is in the EFS,
     assume we are reading entries from the EFS */
  if (mmgsdi_se13_check_if_file_present())
  {
    se13_table_info_ptr->se13_source = MMGSDI_SE13_EFS_SOURCE;
    se13_table_info_ptr->se13_date.mmgsdi_se13_release_day =
      mmgsdi_se13_file_info.se13_header.se13_release_day;
    se13_table_info_ptr->se13_date.mmgsdi_se13_release_month =
      mmgsdi_se13_file_info.se13_header.se13_release_month;
    se13_table_info_ptr->se13_date.mmgsdi_se13_release_year =
      mmgsdi_se13_file_info.se13_header.se13_release_year;
    result = TRUE;
  }
#ifndef FEATURE_MMGSDI_SE13_FROM_EFS_ONLY
  else
  {
    se13_table_info_ptr->se13_source = MMGSDI_SE13_HARDCODED_SOURCE;
    se13_table_info_ptr->se13_date.mmgsdi_se13_release_day = EONS_SE13_TABLE_RELEASE_DAY;
    se13_table_info_ptr->se13_date.mmgsdi_se13_release_month = EONS_SE13_TABLE_RELEASE_MONTH;
    se13_table_info_ptr->se13_date.mmgsdi_se13_release_year = EONS_SE13_TABLE_RELEASE_YEAR;
    result = TRUE;
  }
#endif /* FEATURE_MMGSDI_SE13_FROM_EFS_ONLY */

  return result;
}/* mmgsdi_eons_get_se13_release_date */


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
