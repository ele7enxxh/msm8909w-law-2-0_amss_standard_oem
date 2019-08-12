/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                  M M G S D I   E O N S   S S   E V E N T   A N D   N I T Z
                                      F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines related to SS event and NITZ processing

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_eons_nitz.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   ar      EONS selective logging
05/24/16   bcho    EONS F3 reduction
09/07/15   av      Migrate to mcfg EFS APIs
12/03/14   bcho    featurize support to use stored rplmn name across operators
10/29/14   bcho    Modify rplmn_info.txt when long/short name removed in NITZ
09/09/14   kk      Enhancements in NV handling
09/19/14   vdc     Reset the NITZ data when long or short name is not provided
08/27/14   tl      Introduce network related SIM Lock slot policies
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/28/14   vv      Remove unused data types
07/15/14   yt      Support for NV refresh
06/02/14   bcho    Get PLMN from mobility mgnt info if MOBILITY_MGMT_MASK is set
03/04/14   bcho    Run EONS algorithm when roaming status changes
02/17/14   bcho    Run EONS algo if long or short name is removed from NITZ
01/20/14   vdc     Read RPLMN text file in same power up
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
12/24/13   bcho    Handle CM SS event for limited regional service also
12/19/13   yt      Support for new CM SS event interface
12/05/13   bcho    Re-run the EONS algorithm when PLMNID/LAC/RAT is changed
11/22/13   bcho    Use roaming status received in CM SS event in EONS algorithm
11/08/13   tl      Change allocation of NITZ efs rplmn info from stack to heap
11/08/13   vdc     Remove triple SIM feature to suport third slot CM SS events
11/01/13   vdc     Update PLMN list global after reading from RPLMN Info
10/21/13   vdc     Create RPLMN info text file when EONS algo enters NITZ step
10/17/13   df      Convert gsdi_efs functions to return mmgsdi type
10/09/13   rp      F3 message removal
10/04/13   bcho    Use LAC instead of TAC for secondary CM stack
10/03/13   vdc     Improvements in NITZ storage in EFS
09/21/13   av      Fix buffer overflows
09/21/13   vv      Fix featurization in SS evt handler for non-SGLTE mode
08/22/13   vdc     Added support for TSTS
08/21/13   rp      Don't queue EONS_SS_EVENT_REQ when changed field is empty
07/29/13   am      Added support to determine CSG support dynamically
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
#include "mmgsdicache.h"
#include "mmgsdi_efs.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_eons_nitz.h"
#include "mmgsdi_csg.h"
#include "mmgsdi_session.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_eons_opl_pnn.h"
#include "uim_selective_logging.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*File to store EONS info when it is picked from SS Event's NITS info */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#else
static char *mmgsdi_efs_mmgsdi_dir       = "mmgsdi";
static char *mmgsdi_efs_eons_dir         = "mmgsdi/eons";
static char *mmgsdi_efs_eons_rplmn_file  = "mmgsdi/eons/rplmn_info_ext.txt";
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/

/* RPLMN info pointer to store NITZ info */
mmgsdi_nitz_rplmn_info_type *nitz_rplmn_info_ptr;

/*===========================================================================
FUNCTION MMGSDI_EONS_FREE_NITZ_DATA_PTRS

DESCRIPTION
  Function to free pointers in the NITZ INFO

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_free_nitz_data_ptrs(
  mmgsdi_eons_nitz_data_type  * nitz_info_ptr
)
{
  if (nitz_info_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
     nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
     nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr);
} /* mmgsdi_eons_free_nitz_data_ptrs */


/*==========================================================================
FUNCTION MMGSDI_EONS_COPY_RPLMN_NAME_TO_NITZ

DESCRIPTION
  Function to copy rplmn name to NITZ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_nitz_copy_rplmn_name_to_nitz(
  uint8                        rplmn_index,
  mmgsdi_eons_nitz_data_type  *nitz_info_ptr,
  mmgsdi_plmn_info_type       *plmn_info_ptr
)
{
  if(nitz_info_ptr == NULL ||
     plmn_info_ptr == NULL ||
     rplmn_index >= MMGSDI_MAX_PLMN_NAMES ||
     nitz_rplmn_info_ptr == NULL ||
     nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index] == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                           "Copying RPLMN info to NITZ from index: 0x%x",
                           rplmn_index);

  /* Copy long name */
  nitz_info_ptr->long_name.plmn_name_ci =
    nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_params.plmn_name_ci;
  plmn_info_ptr->plmn_long_name.plmn_name_ci = nitz_info_ptr->long_name.plmn_name_ci;

  nitz_info_ptr->long_name.plmn_name_spare_bits =
    nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_params.plmn_name_spare_bits;
  plmn_info_ptr->plmn_long_name.plmn_name_spare_bits = nitz_info_ptr->long_name.plmn_name_spare_bits;

  nitz_info_ptr->long_name.plmn_name.eons_encoding =
    nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_params.eons_encoding;
  plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding = nitz_info_ptr->long_name.plmn_name.eons_encoding;

  if(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length > 0)
  {
    /* Ensure data_ptr is NULL, before allocating */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
       nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr,
       nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length);

    if (nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
       plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
       nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length);

    if (plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    nitz_info_ptr->long_name.plmn_name.eons_data.data_len =
      nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length;
    plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len =
      nitz_info_ptr->long_name.plmn_name.eons_data.data_len;

    mmgsdi_memscpy(nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_chars,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length);

    mmgsdi_memscpy(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length,
                   nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr,
                   nitz_info_ptr->long_name.plmn_name.eons_data.data_len);

    nitz_info_ptr->long_name_available = TRUE;

    UIM_MSG_MED_5("Full Name:: CI: 0x%x, Spare bits: 0x%x, Encoding: 0x%x Length: 0x%x, Availability: 0x%x",
                  nitz_info_ptr->long_name.plmn_name_ci,
                  nitz_info_ptr->long_name.plmn_name_spare_bits,
                  nitz_info_ptr->long_name.plmn_name.eons_encoding,
                  nitz_info_ptr->long_name.plmn_name.eons_data.data_len,
                  nitz_info_ptr->long_name_available);
  }
  else
  {
    nitz_info_ptr->long_name.plmn_name.eons_data.data_len = 0;
    nitz_info_ptr->long_name_available = FALSE;
    plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len = 0;
  }

  /* Copy short name */
  nitz_info_ptr->short_name.plmn_name_ci =
    nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_params.plmn_name_ci;
  plmn_info_ptr->plmn_short_name.plmn_name_ci = nitz_info_ptr->short_name.plmn_name_ci;

  nitz_info_ptr->short_name.plmn_name_spare_bits =
    nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_params.plmn_name_spare_bits;
  plmn_info_ptr->plmn_short_name.plmn_name_spare_bits = nitz_info_ptr->short_name.plmn_name_spare_bits;

  nitz_info_ptr->short_name.plmn_name.eons_encoding =
    nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_params.eons_encoding;
  plmn_info_ptr->plmn_short_name.plmn_name.eons_encoding = nitz_info_ptr->short_name.plmn_name.eons_encoding;

  if(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length > 0)
  {
    /* Ensure data_ptr is NULL, before allocating */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
       nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr,
       nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length);

    if (nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
       plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
       nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length);

    if (plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    nitz_info_ptr->short_name.plmn_name.eons_data.data_len =
      nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length;
    plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len =
      nitz_info_ptr->short_name.plmn_name.eons_data.data_len;

    mmgsdi_memscpy(nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_chars,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length);

    mmgsdi_memscpy(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length,
                   nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr,
                   nitz_info_ptr->short_name.plmn_name.eons_data.data_len);

    nitz_info_ptr->short_name_available = TRUE;

    UIM_MSG_MED_5("Short Name:: CI: 0x%x, Spare bits: 0x%x, Encoding: 0x%x Length: 0x%x, Availability: 0x%x",
                  nitz_info_ptr->short_name.plmn_name_ci,
                  nitz_info_ptr->short_name.plmn_name_spare_bits,
                  nitz_info_ptr->short_name.plmn_name.eons_encoding,
                  nitz_info_ptr->short_name.plmn_name.eons_data.data_len,
                  nitz_info_ptr->short_name_available);
  }
  else
  {
    nitz_info_ptr->short_name.plmn_name.eons_data.data_len = 0;
    nitz_info_ptr->short_name_available = FALSE;
    plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len = 0;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_eons_nitz_copy_rplmn_name_to_nitz */


/*==========================================================================
FUNCTION MMGSDI_EONS_NITZ_COPY_PLMN_INFO_TO_RPLMN

DESCRIPTION
  Function to copy plmn info to rplmn array

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_SUCCESS - If the copy is successful
  MMGSDI_INCORRECT_PARAMS - copy is unsuccessful


SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_nitz_copy_plmn_info_to_rplmn(
  const mmgsdi_eons_nitz_data_type *nitz_info_ptr,
  uint8                             session_mask,
  uint8                             rplmn_index,
  mmgsdi_plmn_id_type               hplmn_id
)
{
  if(nitz_info_ptr == NULL ||
     rplmn_index >= MMGSDI_MAX_PLMN_NAMES ||
     nitz_rplmn_info_ptr == NULL ||
     nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index] == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                           "Copying PLMN info to RPLMN info at index: 0x%x",
                           rplmn_index);

  /*copy full name country initials*/
  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_params.plmn_name_ci =
    nitz_info_ptr->long_name.plmn_name_ci;

  /*copy full name spare bits*/
  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_params.plmn_name_spare_bits =
    nitz_info_ptr->long_name.plmn_name_spare_bits;

  /*copy full name encoding*/
  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_params.eons_encoding =
    nitz_info_ptr->long_name.plmn_name.eons_encoding;

  /* Set long name availability flag to TRUE */
  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_params.plmn_name_available =
    nitz_info_ptr->long_name_available;

  /* Copy full name if full name is available from NITZ otherwise reset the data that was present */
  if(nitz_info_ptr->long_name_available &&
     nitz_info_ptr->long_name.plmn_name.eons_data.data_len > 0 &&
     nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr != NULL)
  {
    MMGSDIUTIL_SET_MIN(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length,
                       nitz_info_ptr->long_name.plmn_name.eons_data.data_len,
                       MMGSDI_EONS_NITZ_PLMN_FULL_NAME_LENGTH);

    mmgsdi_memscpy(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_chars,
                   sizeof(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_chars),
                   nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length);
  }
  else
  {
    memset(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_chars,
           0x00,
           sizeof(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_chars));
    nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length = 0;
  }

  UIM_MSG_MED_5("Full name::  CI: 0x%x, Spare bits: 0x%x, Encoding: 0x%x Length: 0x%x, Availability: 0x%x",
                nitz_info_ptr->long_name.plmn_name_ci,
                nitz_info_ptr->long_name.plmn_name_spare_bits,
                nitz_info_ptr->long_name.plmn_name.eons_encoding,
                nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_length,
                nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->full_name.full_name_params.plmn_name_available);

  /*copy short name country initials*/
  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_params.plmn_name_ci =
    nitz_info_ptr->short_name.plmn_name_ci;

  /*copy short name spare bits*/
  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_params.plmn_name_spare_bits =
    nitz_info_ptr->short_name.plmn_name_spare_bits;

  /*copy short name encoding*/
  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_params.eons_encoding =
    nitz_info_ptr->short_name.plmn_name.eons_encoding;

  /* Set short name availability flag to TRUE */
  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_params.plmn_name_available =
    nitz_info_ptr->short_name_available;

  /* Copy short name if short name is available from NITZ otherwise reset the data that was present */
  if(nitz_info_ptr->short_name_available &&
     nitz_info_ptr->short_name.plmn_name.eons_data.data_len > 0 &&
     nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr != NULL)
  {
    MMGSDIUTIL_SET_MIN(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length,
                       nitz_info_ptr->short_name.plmn_name.eons_data.data_len,
                       MMGSDI_EONS_NITZ_PLMN_SHORT_NAME_LENGTH);

    mmgsdi_memscpy(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_chars,
                   sizeof(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_chars),
                   nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length);
  }
  else
  {
    memset(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_chars,
           0x00,
           sizeof(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_chars));
    nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length = 0;
  }

  UIM_MSG_MED_5("Short name::  CI: 0x%x, Spare bits: 0x%x, Encoding: 0x%x Length: 0x%x, Availability: 0x%x",
                nitz_info_ptr->short_name.plmn_name_ci,
                nitz_info_ptr->short_name.plmn_name_spare_bits,
                nitz_info_ptr->short_name.plmn_name.eons_encoding,
                nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_length,
                nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->short_name.short_name_params.plmn_name_available);
  
  /* Copy PLMN ID, RAT & CSG ID*/
  mmgsdi_memscpy(&nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id,
                 sizeof(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id),
                 &nitz_info_ptr->rplmn_id,
                 sizeof(nitz_info_ptr->rplmn_id));

  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->lac = nitz_info_ptr->lac;

  /* Assign wildcard plmn to hplmn if it is invalid or feature is enabled */
  if(!MMGSDI_EONS_PLMN_ID_VALIDITY_CHECK(hplmn_id.plmn_id_val) ||
     (mmgsdi_nv_get_feature_status(
        MMGSDI_FEATURE_USE_STORED_RPLMN_NAME_ACROSS_OPERATORS,
        MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED))
  {
    memset(hplmn_id.plmn_id_val, 0xDD, sizeof(hplmn_id.plmn_id_val));
  }

  /* Copy HPLMN ID */
  mmgsdi_memscpy(&nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->hplmn_id,
                 sizeof(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->hplmn_id),
                 &hplmn_id,
                 sizeof(hplmn_id));

  UIM_MSG_MED_5("PLMN ID[0]: 0x%x, PLMN ID[1]: 0x%x, PLMN ID[2]: 0x%x RAT: 0x%x, LAC: 0x%x",
                nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id.plmn_id_val[0],
                nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id.plmn_id_val[1],
                nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id.plmn_id_val[2],
                nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id.rat,
                nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->lac);

  /* Update bit mask */
  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->session_bit_mask |= session_mask;

  return MMGSDI_SUCCESS;
}/* mmgsdi_eons_nitz_copy_plmn_info_to_rplmn */


/*==========================================================================
FUNCTION MMGSDI_EONS_NITZ_READ_RPLMN_INFO

DESCRIPTION
  Get the operator name string from RPLMN info if PLMN ID is matched.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If the PLMN ID is matched and read is successful
  FALSE - No match is found

SIDE EFFECTS
  None
==========================================================================*/
static boolean mmsdi_eons_nitz_read_rplmn_info(
  mmgsdi_eons_nitz_data_type   *nitz_info_ptr,
  mmgsdi_plmn_info_type        *plmn_info_ptr,
  mmgsdi_plmn_id_type           hplmn_id
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uint8                   rplmn_index   = 0;

  if(nitz_info_ptr == NULL || nitz_rplmn_info_ptr == NULL || plmn_info_ptr == NULL)
  {
    return FALSE;
  }

  for(rplmn_index = 0; rplmn_index < MMGSDI_MAX_PLMN_NAMES; rplmn_index++)
  {
    /* Check NITZ's RAT & PLMN ID matches with the rplmn info*/
    if((nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index] != NULL)        &&
       (nitz_info_ptr->rplmn_id.rat ==
          nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id.rat) &&
       (TRUE == mmgsdi_eons_plmn_match(
                  nitz_info_ptr->rplmn_id,
                  nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id)))
    {
      /* If current hplmn id is invalid or it matches with hplmn stored with
         rplmn name, return the rplmn name*/
      if((!MMGSDI_EONS_PLMN_ID_VALIDITY_CHECK(hplmn_id.plmn_id_val)) ||
          (mmgsdi_eons_compare_wildcard_plmn(
                nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->hplmn_id,
                hplmn_id)))
      {
        mmgsdi_status = mmgsdi_eons_nitz_copy_rplmn_name_to_nitz(rplmn_index,
                                                                 nitz_info_ptr,
                                                                 plmn_info_ptr);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          return FALSE;
        }
        UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                                 "Match found with PLMN info: PLMN ID[0]: 0x%x, PLMN ID[1]: 0x%x, PLMN ID[2]: 0x%x",
                                 nitz_info_ptr->rplmn_id.plmn_id_val[0],
                                 nitz_info_ptr->rplmn_id.plmn_id_val[1],
                                 nitz_info_ptr->rplmn_id.plmn_id_val[2]);

        return TRUE;
      }
    }
  }
  UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "No match found in the RPLMN info");
  return FALSE;
}/* mmsdi_eons_nitz_read_rplmn_info */


/*==========================================================================
FUNCTION MMGSDI_EONS_NITZ_WRITE_RPLMN_ENTRY

DESCRIPTION
  Write PLMN entry into RPLMN file.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_SUCCESS - If the write is successful
  MMGSDI_INCORRECT_PARAM - If incorrect parameters are provided as input
  MMGSDI_ERROR - If write is unsuccessful

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_nitz_write_rplmn_entry(
  mmgsdi_eons_nitz_data_type *nitz_info_ptr,
  uint8                       session_mask,
  uint8                       subs_index,
  boolean                     is_write,
  uint8                       rplmn_index,
  mmgsdi_plmn_id_type         hplmn_id
)
{
  mmgsdi_nitz_rplmn_entry_type *rplmn_info    = NULL;
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_SUCCESS;
  uint8                         replace_index = 0xFF;
  uint8                         start_index   = 0x00;

  if(nitz_info_ptr == NULL || nitz_rplmn_info_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* In case of replace, the start index is the rplmn index */
  if(is_write == FALSE)
  {
    start_index = rplmn_index;
  }

  if(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index] == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index],
      sizeof(mmgsdi_nitz_rplmn_entry_type));

    if(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index] == NULL)
    {
      return MMGSDI_ERROR;
    }
  }

  mmgsdi_status = mmgsdi_eons_nitz_copy_plmn_info_to_rplmn(nitz_info_ptr,
                                                           session_mask,
                                                           rplmn_index,
                                                           hplmn_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* If there is an entry already with the same sub then unmask it & swap*/
  if(nitz_rplmn_info_ptr->plmn_info_index[subs_index] != 0xFF)
  {
    replace_index = nitz_rplmn_info_ptr->plmn_info_index[subs_index];
    while((replace_index != start_index)                                     &&
          (nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[replace_index] != NULL) &&
          (nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[replace_index]->session_bit_mask != 0x00))
    {
      if(replace_index == 0)
      {
        replace_index = MMGSDI_MAX_PLMN_NAMES;
      }
      replace_index = replace_index - 1;
    }

    /* Increment the replace index only if is different from previous PLMN index*/
    if(replace_index != nitz_rplmn_info_ptr->plmn_info_index[subs_index])
    {
      replace_index = (replace_index + 1) % MMGSDI_MAX_PLMN_NAMES;
    }

    /* Unmask the previous masked PLMN index */
    nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[
      nitz_rplmn_info_ptr->plmn_info_index[subs_index]]->session_bit_mask &= ~session_mask;

    /* Swap the replace element (first non-zero element) and the previous masked element of same sub */
    if(replace_index != nitz_rplmn_info_ptr->plmn_info_index[subs_index])
    {
      rplmn_info = nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[replace_index];

      nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[replace_index] =
        nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[nitz_rplmn_info_ptr->plmn_info_index[subs_index]];

      nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[nitz_rplmn_info_ptr->plmn_info_index[subs_index]] =
        rplmn_info;
    }
  }

  /* Store the index into prev PLMN index */
  nitz_rplmn_info_ptr->plmn_info_index[subs_index] = rplmn_index;

  return MMGSDI_SUCCESS;
}/* mmgsdi_eons_nitz_write_rplmn_entry */


/*==========================================================================
FUNCTION MMGSDI_EONS_NITZ_WRITE_RPLMN_INFO

DESCRIPTION
  Write PLMN info into RPLMN file.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_SUCCESS - If the write is successful
  MMGSDI_INCORRECT_PARAM - If incorrect parameters are provided as input
  MMGSDI_ERROR - If write is unsuccessful

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_nitz_write_rplmn_info(
  mmgsdi_eons_nitz_data_type       *nitz_info_ptr,
  mmgsdi_session_type_enum_type     session_type,
  mmgsdi_eons_call_stack_enum_type  stack_type,
  mmgsdi_plmn_id_type               hplmn_id
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uint8                   session_mask  = MMGSDI_EONS_NITZ_GW_NO_MASK;
  uint8                   rplmn_index   = 0;
  uint8                   subs_index    = 0;

  if(nitz_info_ptr == NULL || nitz_rplmn_info_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Determine session mask */
  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      if(stack_type == MMGSDI_EONS_FIRST_CALL_STACK)
      {
        session_mask = MMGSDI_EONS_NITZ_GW_PRI_MAIN_MASK;
        subs_index = 0;
      }
      else if(stack_type == MMGSDI_EONS_SEC_CALL_STACK)
      {
        session_mask = MMGSDI_EONS_NITZ_GW_PRI_SEC_MASK;
        subs_index = 1;
      }
      break;

    case MMGSDI_GW_PROV_SEC_SESSION:
      session_mask = MMGSDI_EONS_NITZ_GW_SEC_MAIN_MASK;
      subs_index = 2;
      break;

    case MMGSDI_GW_PROV_TER_SESSION:
      session_mask = MMGSDI_EONS_NITZ_GW_TER_MAIN_MASK;
      subs_index = 3;
      break;

    default:
      UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS,
                               "Session: 0x%x & stack: 0x%x combination is invalid",
                               session_type, stack_type);
      return MMGSDI_INCORRECT_PARAMS;
  }

/* We are filling the array from left to right (from 0 to MAX_PLMN_NAMES - 1).
   We check for an entry which is not free first and if it succeeds then
   check for the parameters for match. In the same loop, if we have free entry
   then that means we have passed all the entries which has some PLMN info and
   there is no match. Now, if we are out of the loop then that means we dont
   have any free entry and none of the entries has matched with the incoming
   NITZ info. So, we go with replacement after the loop. */

  /* Trying to mask with existrying entry or write into free entry if available */
  for(rplmn_index = 0; rplmn_index < MMGSDI_MAX_PLMN_NAMES; rplmn_index++)
  {
    if(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index] == NULL)
    {
      mmgsdi_status = mmgsdi_eons_nitz_write_rplmn_entry(nitz_info_ptr,
                                                         session_mask,
                                                         subs_index,
                                                         TRUE,
                                                         rplmn_index,
                                                         hplmn_id);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
      UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                               "New PLMN info written successfully in RPLMN index: 0x%x",
                               rplmn_index );
      return MMGSDI_SUCCESS;
    }
    /* Check whether any entry is free in the rplmn_info array */
    else
    {
      /* Check whether we have the PLMN info already in the rplmn_info array */
      if((nitz_info_ptr->rplmn_id.rat ==
            nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id.rat) &&
         (TRUE == mmgsdi_eons_plmn_match(
                   nitz_info_ptr->rplmn_id,
                   nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id)))
      {

        /* Unmask the previous entry if any */
        if((nitz_rplmn_info_ptr->plmn_info_index[subs_index] != 0xFF))
        {
          nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[
            nitz_rplmn_info_ptr->plmn_info_index[subs_index]]->session_bit_mask &= ~session_mask;
        }

        nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->session_bit_mask |= session_mask;

        mmgsdi_status = mmgsdi_eons_nitz_copy_plmn_info_to_rplmn(nitz_info_ptr,
                                                                 session_mask,
                                                                 rplmn_index,
                                                                 hplmn_id);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          return mmgsdi_status;
        }

        nitz_rplmn_info_ptr->plmn_info_index[subs_index] = rplmn_index;
        UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                                 "New PLMN info matched with existing RPLMN info at index: 0x%x",
                                 rplmn_index );
        return MMGSDI_SUCCESS;
      }
    }
  }

  mmgsdi_status = mmgsdi_eons_nitz_write_rplmn_entry(nitz_info_ptr,
                                                     session_mask,
                                                     subs_index,
                                                     FALSE,
                                                     nitz_rplmn_info_ptr->start_index,
                                                     hplmn_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                           "New PLMN info overwritten successfully in RPLMN index: 0x%x",
                           nitz_rplmn_info_ptr->start_index );

  nitz_rplmn_info_ptr->start_index = (nitz_rplmn_info_ptr->start_index + 1) % MMGSDI_MAX_PLMN_NAMES;

  return MMGSDI_SUCCESS;
}/* mmgsdi_eons_nitz_write_rplmn_info */


/*==========================================================================
FUNCTION MMGSDI_EONS_READ_STORED_NITZ_INFO

DESCRIPTION
  Function to read rplmn_info.txt on mmgsdi task init and cache it

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_eons_read_stored_nitz_info(
  void
)
{
  mmgsdi_nitz_efs_rplmn_info_type * nitz_efs_rplmn_info_ptr = NULL;
  mmgsdi_return_enum_type           mmgsdi_status           = MMGSDI_SUCCESS;
  uint32                            rplmn_info_read_size    = 0;
  uint8                             rplmn_index             = 0;

  if(MMGSDI_SUCCESS == gsdi_efs_name_test((const char *)mmgsdi_efs_eons_rplmn_file,
                                          UIM_COMMON_EFS_DEVICE))
  {
    /* Initialize the RPLMN info global */
    if(nitz_rplmn_info_ptr == NULL)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(nitz_rplmn_info_ptr,
        sizeof(mmgsdi_nitz_rplmn_info_type));

      if(nitz_rplmn_info_ptr == NULL)
      {
        return MMGSDI_ERROR;
      }
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(nitz_efs_rplmn_info_ptr,
      sizeof(mmgsdi_nitz_efs_rplmn_info_type));

    if (nitz_efs_rplmn_info_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    rplmn_info_read_size = sizeof(nitz_rplmn_info_ptr->plmn_info_index) +
                           sizeof(nitz_rplmn_info_ptr->start_index) +
                           sizeof(mmgsdi_nitz_rplmn_entry_type) * MMGSDI_MAX_PLMN_NAMES;

    if(MMGSDI_SUCCESS == gsdi_efs_read_file_with_len(
                             (const char *)mmgsdi_efs_eons_rplmn_file,
                             rplmn_info_read_size,
                             (uint8 *)nitz_efs_rplmn_info_ptr,
                             UIM_COMMON_EFS_DEVICE))
    {
      UIM_MSG_MED_0("RPLMN text file is read");

      nitz_rplmn_info_ptr->start_index = nitz_efs_rplmn_info_ptr->start_index;

      mmgsdi_memscpy(nitz_rplmn_info_ptr->plmn_info_index,
                     sizeof(nitz_rplmn_info_ptr->plmn_info_index),
                     &(nitz_efs_rplmn_info_ptr->plmn_info_index),
                     sizeof(nitz_efs_rplmn_info_ptr->plmn_info_index));

      for(rplmn_index = 0; rplmn_index < MMGSDI_MAX_PLMN_NAMES; rplmn_index++)
      {
        if(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index] == NULL)
        {
          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
            nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index],
            sizeof(mmgsdi_nitz_rplmn_entry_type));

          if(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index] == NULL)
          {
            MMGSDIUTIL_TMC_MEM_FREE(nitz_efs_rplmn_info_ptr);
            return MMGSDI_ERROR;
          }
        }

        mmgsdi_memscpy(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index],
                       sizeof(mmgsdi_nitz_rplmn_entry_type),
                       &nitz_efs_rplmn_info_ptr->nitz_rplmn_entry[rplmn_index],
                       sizeof(mmgsdi_nitz_rplmn_entry_type));

        if((nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id.plmn_id_val[0] == 0xFF) &&
           (nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id.plmn_id_val[1] == 0xFF) &&
           (nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]->plmn_id.plmn_id_val[2] == 0xFF))
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]);
          break;
        }
      }

      mmgsdi_status = MMGSDI_SUCCESS;
    }
    else
    {
      mmgsdi_status = MMGSDI_ERROR;
    }

    MMGSDIUTIL_TMC_MEM_FREE(nitz_efs_rplmn_info_ptr);
  }

  return mmgsdi_status;
}/* mmgsdi_eons_read_stored_nitz_info */


/*==========================================================================
FUNCTION MMGSDI_EONS_STORE_NITZ_INFO

DESCRIPTION
  Function to write rplmn_info.txt with cached data on reception of TASK_OFFLINE/TASK_STOP

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_store_nitz_info (
  void
)
{
  mmgsdi_nitz_efs_rplmn_info_type * nitz_efs_rplmn_info_ptr      = NULL;
  mmgsdi_return_enum_type           mmgsdi_status                = MMGSDI_SUCCESS;
  boolean                           is_eons_dir_available        = FALSE;
  boolean                           is_rplmn_info_file_available = FALSE;
  uint32                            rplmn_info_write_size        = 0;
  uint8                             rplmn_index                  = 0;

  if(nitz_rplmn_info_ptr != NULL)
  {
    if(MMGSDI_SUCCESS == gsdi_efs_name_test((const char *)mmgsdi_efs_eons_rplmn_file,
                                            UIM_COMMON_EFS_DEVICE))
    {
      is_rplmn_info_file_available = TRUE;
    }
    else
    {
      if(MMGSDI_SUCCESS == gsdi_efs_name_test((const char *)mmgsdi_efs_mmgsdi_dir,
                                              UIM_COMMON_EFS_DEVICE))
      {
        /*mmgsdi directory is available, check availability of eons dir*/
        if(MMGSDI_SUCCESS == gsdi_efs_name_test((const char *)mmgsdi_efs_eons_dir,
                                                UIM_COMMON_EFS_DEVICE))
        {
          is_eons_dir_available = TRUE;
        }
        else
        {
          if(MMGSDI_SUCCESS == gsdi_efs_mkdir((const char *)mmgsdi_efs_eons_dir))
          {
            is_eons_dir_available = TRUE;
          }
        }
      }
      else if(MMGSDI_SUCCESS == gsdi_efs_mkdir((const char *)mmgsdi_efs_mmgsdi_dir))
      {
        /*mmgsdi directory is available, now create eons dir*/
        if(MMGSDI_SUCCESS == gsdi_efs_mkdir((const char *)mmgsdi_efs_eons_dir))
        {
          is_eons_dir_available = TRUE;
        }
      }

      if(TRUE == is_eons_dir_available)
      {
        /*create file*/
        if(MMGSDI_SUCCESS == gsdi_efs_create_file((const char *)mmgsdi_efs_eons_rplmn_file,
                                                  UIM_COMMON_EFS_DEVICE))
        {
          is_rplmn_info_file_available = TRUE;
        }
      }
    }

    if(is_rplmn_info_file_available)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(nitz_efs_rplmn_info_ptr,
        sizeof(mmgsdi_nitz_efs_rplmn_info_type));

      if (nitz_efs_rplmn_info_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(nitz_rplmn_info_ptr);
        return MMGSDI_ERROR;
      }

      memset(nitz_efs_rplmn_info_ptr, 0xFF, sizeof(mmgsdi_nitz_efs_rplmn_info_type));

      rplmn_info_write_size = sizeof(nitz_rplmn_info_ptr->plmn_info_index) +
                              sizeof(nitz_rplmn_info_ptr->start_index) +
                              sizeof(mmgsdi_nitz_rplmn_entry_type) * MMGSDI_MAX_PLMN_NAMES;

      /* Copy the start_index */
      nitz_efs_rplmn_info_ptr->start_index = nitz_rplmn_info_ptr->start_index;

      /* Copy the PLMN info index array */
      mmgsdi_memscpy(nitz_efs_rplmn_info_ptr->plmn_info_index,
                     sizeof(nitz_efs_rplmn_info_ptr->plmn_info_index),
                     nitz_rplmn_info_ptr->plmn_info_index,
                     sizeof(nitz_rplmn_info_ptr->plmn_info_index));

      /* Copy RPLMN info present in every entry */
      while((rplmn_index < MMGSDI_MAX_PLMN_NAMES) &&
            (nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index] != NULL))
      {
        mmgsdi_memscpy(&nitz_efs_rplmn_info_ptr->nitz_rplmn_entry[rplmn_index],
                       sizeof(nitz_efs_rplmn_info_ptr->nitz_rplmn_entry[rplmn_index]),
                       nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index],
                       sizeof(mmgsdi_nitz_rplmn_entry_type));

        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(nitz_rplmn_info_ptr->nitz_rplmn_entry_ptr[rplmn_index]);
        rplmn_index++;
      }

      if(MMGSDI_SUCCESS == gsdi_efs_write_file(
                           (const char *)mmgsdi_efs_eons_rplmn_file,
                            rplmn_info_write_size,
                            (uint8 *)nitz_efs_rplmn_info_ptr,
                            UIM_COMMON_EFS_DEVICE))
      {
        mmgsdi_status = MMGSDI_SUCCESS;
      }
      else
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      UIM_MSG_MED_1("Write status for RPLMN info text file: 0x%x", mmgsdi_status);

      MMGSDIUTIL_TMC_MEM_FREE(nitz_efs_rplmn_info_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(nitz_rplmn_info_ptr);
    }
  }
  return mmgsdi_status;
}/* mmgsdi_eons_store_nitz_info */


/*==========================================================================
FUNCTION MMGSDI_EONS_GET_NITZ_ONS

DESCRIPTION
  Function to get operator name string from NITZ data

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_get_nitz_ons(
  mmgsdi_eons_call_stack_enum_type    stack_type,
  mmgsdi_session_type_enum_type       session_type,
  mmgsdi_eons_info_type             * eons_info_ptr,
  mmgsdi_plmn_info_type             * plmn_info_ptr,
  mmgsdi_plmn_id_type                 hplmn_id
)
{
  boolean                               is_nitz_ons_available   = FALSE;
  mmgsdi_return_enum_type               mmgsdi_status           = MMGSDI_ERROR;
  mmgsdi_eons_nitz_data_type          * nitz_info_ptr           = NULL;

  UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "Get operator name string from NITZ");

  if(plmn_info_ptr == NULL || eons_info_ptr == NULL ||
     (stack_type != MMGSDI_EONS_FIRST_CALL_STACK &&
      stack_type != MMGSDI_EONS_SEC_CALL_STACK))
  {
    return is_nitz_ons_available;
  }

  if(nitz_rplmn_info_ptr == NULL)
  {
    /* Read stored NITZ info from file system */
    (void)mmgsdi_eons_read_stored_nitz_info();
  }

  if (stack_type == MMGSDI_EONS_FIRST_CALL_STACK)
  {
    nitz_info_ptr = &(eons_info_ptr->ss_info.first_stack_ss.nitz_info);
  }
  else
  {
    nitz_info_ptr = &(eons_info_ptr->ss_info.sec_stack_ss.nitz_info);
  }

  if(nitz_info_ptr == NULL)
  {
    return FALSE;
  }

  if(nitz_info_ptr->long_name_available)
  {
    mmgsdi_status = mmgsdi_eons_copy_plmn_name_info(&(nitz_info_ptr->long_name),
                                                    &(plmn_info_ptr->plmn_long_name),
                                                    TRUE);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return FALSE;
    }

    if (plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len > 0 &&
        plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr != NULL)
    {
       is_nitz_ons_available = TRUE;
    }
  }

  if(nitz_info_ptr->short_name_available)
  {
    mmgsdi_status = mmgsdi_eons_copy_plmn_name_info(&(nitz_info_ptr->short_name),
                                                    &(plmn_info_ptr->plmn_short_name),
                                                    TRUE);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return FALSE;
    }

    if (plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len > 0 &&
        plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr != NULL)
    {
       is_nitz_ons_available = TRUE;
    }
  }

  if(session_type == MMGSDI_GW_PROV_PRI_SESSION ||
     session_type == MMGSDI_GW_PROV_SEC_SESSION ||
     session_type == MMGSDI_GW_PROV_TER_SESSION)
  {
    UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                             "NITZ ONS availability status: 0x%x",
                             is_nitz_ons_available);

    /*structure of RPLMN_INFO.txt*/
    /*=========================================================================
      PLMN Info array of structures of size MMGSDI_MAX_PLMN_NAMES
      =========================================================================
      ==  PLMN Info starts                                                   ==
      ==  PLMN ID                   3 Bytes                                  ==
      ==  RAT                       1 Byte                                   ==
      ==  CSG ID                    1 Byte                                   ==
      ==  LAC                       2 Bytes                                  ==
      ==  Long name available flag  Boolean                                  ==
      ==  Long name CI              Boolean                                  ==
      ==  Long name Spare bit       1 Byte                                   ==
      ==  Long name encoding        1 Byte                                   ==
      ==  Long name length          4 Byte                                   ==
      ==  Long name chars           255 Bytes                                ==
      ==  Long name available flag  Boolean                                  ==
      ==  Long name CI              Boolean                                  ==
      ==  Long name Spare bit       1 Byte                                   ==
      ==  Long name encoding        1 Byte                                   ==
      ==  Long name length          4 Byte                                   ==
      ==  Long name chars           255 Bytes                                ==
      ==  Bit Mask                  1 Byte                                   ==
      =========================================================================
      ==  Previous PLMN index       3 Bytes                                  ==
      ==  Start index               1 Byte                                   ==
      ==  PLMN Info Ends                                                     ==
      ==  *****************************************************************  ==
      =======================================================================*/

    if(is_nitz_ons_available != TRUE)
    {
      UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "No NITZ Info, get ONS from efs file");
      is_nitz_ons_available = mmsdi_eons_nitz_read_rplmn_info(nitz_info_ptr,
                                                              plmn_info_ptr,
                                                              hplmn_id);
    }/*ens if is_nitz_ons_available != TRUE*/
    else /*ONS retrieved*/
    {
      /* Allocate memory for RPLMN info global only when we are writing the NITZ */
      if(nitz_rplmn_info_ptr == NULL)
      {
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(nitz_rplmn_info_ptr,
          sizeof(mmgsdi_nitz_rplmn_info_type));

        if(nitz_rplmn_info_ptr == NULL)
        {
          return MMGSDI_ERROR;
        }
        memset(nitz_rplmn_info_ptr->plmn_info_index, 0xFF, sizeof(nitz_rplmn_info_ptr->plmn_info_index));
      }
      mmgsdi_status =
        mmgsdi_eons_nitz_write_rplmn_info(nitz_info_ptr, session_type,
                                          stack_type, hplmn_id);
      UIM_MSG_LOW_1("Write status of RPLMN info: 0x%x", mmgsdi_status );
    }
  }/*end if session_type == MMGSDI_GW_PROV_PRI_SESSION||
      session_type == MMGSDI_GW_PROV_SEC_SESSION || session_type == MMGSDI_GW_PROV_TER_SESSION */

  return is_nitz_ons_available;
}/* mmgsdi_eons_get_nitz_ons */


/*===========================================================================
  FUNCTION MMGSDI_EONS_MATCH_RPLMN_NAME_WITH_NITZ_NAME

DESCRIPTION
  Function to determine if EONS algo needs to be run and event needs to be sent
  for a call stack

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if RPLMN name matches NITZ name and FALSE if not

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_eons_match_rplmn_name_with_nitz_name(
  const mmgsdi_eons_per_stack_rplmn_info_type  * rplmn_info_ptr,
  const mmgsdi_eons_nitz_data_type             * nitz_info_ptr
)
{
  if (rplmn_info_ptr == NULL || nitz_info_ptr == NULL)
  {
    return FALSE;
  }

  if (nitz_info_ptr->csg_info.csg_id != MMGSDI_EONS_INVALID_CSG_ID)
  {
    if ((nitz_info_ptr->csg_info.hnb_name.length > 0)  &&
         ((rplmn_info_ptr->csg_info.hnb_name.length !=
           nitz_info_ptr->csg_info.hnb_name.length)      ||
          memcmp(rplmn_info_ptr->csg_info.hnb_name.name,
                 nitz_info_ptr->csg_info.hnb_name.name,
                 nitz_info_ptr->csg_info.hnb_name.length)))
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

  if((nitz_info_ptr->long_name_available != rplmn_info_ptr->long_name_available)||
     ((nitz_info_ptr->long_name_available)  &&
      ((rplmn_info_ptr->long_name.plmn_name.eons_encoding !=
        nitz_info_ptr->long_name.plmn_name.eons_encoding)          ||
       (rplmn_info_ptr->long_name.plmn_name_ci !=
        nitz_info_ptr->long_name.plmn_name_ci)                     ||
       (rplmn_info_ptr->long_name.plmn_name_spare_bits !=
        nitz_info_ptr->long_name.plmn_name_spare_bits)             ||
       (rplmn_info_ptr->long_name.plmn_name.eons_data.data_len !=
        nitz_info_ptr->long_name.plmn_name.eons_data.data_len)     ||
       ((rplmn_info_ptr->long_name.plmn_name.eons_data.data_ptr != NULL) &&
        (nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr != NULL)  &&
        memcmp(rplmn_info_ptr->long_name.plmn_name.eons_data.data_ptr,
               nitz_info_ptr->long_name.plmn_name.eons_data.data_ptr,
               nitz_info_ptr->long_name.plmn_name.eons_data.data_len)))))
  {
    return FALSE;
  }

  if ((nitz_info_ptr->short_name_available != rplmn_info_ptr->short_name_available)||
      ((nitz_info_ptr->short_name_available)               &&
       ((rplmn_info_ptr->short_name.plmn_name.eons_encoding !=
         nitz_info_ptr->short_name.plmn_name.eons_encoding)          ||
        (rplmn_info_ptr->short_name.plmn_name_ci !=
         nitz_info_ptr->short_name.plmn_name_ci)                     ||
        (rplmn_info_ptr->short_name.plmn_name_spare_bits !=
         nitz_info_ptr->short_name.plmn_name_spare_bits)             ||
        (rplmn_info_ptr->short_name.plmn_name.eons_data.data_len !=
         nitz_info_ptr->short_name.plmn_name.eons_data.data_len)     ||
        ((rplmn_info_ptr->short_name.plmn_name.eons_data.data_ptr != NULL) &&
         (nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr != NULL)  &&
         memcmp(rplmn_info_ptr->short_name.plmn_name.eons_data.data_ptr,
                nitz_info_ptr->short_name.plmn_name.eons_data.data_ptr,
                nitz_info_ptr->short_name.plmn_name.eons_data.data_len)))))
  {
    return FALSE;
  }

  return TRUE;
} /* mmgsdi_eons_match_rplmn_name_with_nitz_name */


/*===========================================================================
  FUNCTION MMGSDI_EONS_NITZ_UPDATE_RPLMN_IDENTIFIERS

DESCRIPTION
  Function to copy RPLMN identifiers

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating whether RPLMN identifiers were updated

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_nitz_update_rplmn_identifiers(
  mmgsdi_eons_info_type   * eons_info_ptr,
  boolean                 * first_stack_rplmn_updated_ptr,
  boolean                 * sec_stack_rplmn_updated_ptr,
  boolean                 * fs_plmn_id_lac_rat_changed_ptr,
  boolean                 * ss_plmn_id_lac_rat_changed_ptr
)
{
  mmgsdi_eons_per_stack_rplmn_info_type  * rplmn_info_ptr   = NULL;
  mmgsdi_eons_nitz_data_type             * nitz_info_ptr    = NULL;

  if(eons_info_ptr == NULL || first_stack_rplmn_updated_ptr == NULL ||
     sec_stack_rplmn_updated_ptr == NULL || fs_plmn_id_lac_rat_changed_ptr == NULL ||
     ss_plmn_id_lac_rat_changed_ptr == NULL)
  {
    return;
  }

  rplmn_info_ptr = &(eons_info_ptr->rplmn_info.first_stack_rplmn);
  nitz_info_ptr  = &(eons_info_ptr->ss_info.first_stack_ss.nitz_info);

  if (eons_info_ptr->ss_info.first_stack_ss.is_valid &&
      nitz_info_ptr->rplmn_available &&
      nitz_info_ptr->lac_available)
  {
    if ((rplmn_info_ptr->rplmn_available == FALSE                                ||
         rplmn_info_ptr->lac_available   == FALSE                                ||
         rplmn_info_ptr->lac != nitz_info_ptr->lac                               ||
         (FALSE == mmgsdi_eons_plmn_match(rplmn_info_ptr->rplmn_id,
                                          nitz_info_ptr->rplmn_id))              ||
         rplmn_info_ptr->rplmn_id.rat != nitz_info_ptr->rplmn_id.rat))
    {
      *fs_plmn_id_lac_rat_changed_ptr = TRUE;
    }

    if ((*fs_plmn_id_lac_rat_changed_ptr == TRUE)                                ||
        (nitz_info_ptr->csg_info.csg_id == MMGSDI_EONS_INVALID_CSG_ID &&
         (rplmn_info_ptr->long_name.plmn_name.eons_data.data_len == 0  &&
          rplmn_info_ptr->short_name.plmn_name.eons_data.data_len == 0))         ||

        ((nitz_info_ptr->csg_info.csg_id != MMGSDI_EONS_INVALID_CSG_ID) &&
         ((rplmn_info_ptr->csg_info.csg_id != nitz_info_ptr->csg_info.csg_id)||
          (rplmn_info_ptr->csg_info.hnb_name.length == 0))))
    {
      rplmn_info_ptr->rplmn_available = TRUE;
      rplmn_info_ptr->lac_available   = TRUE;
      rplmn_info_ptr->lac             = nitz_info_ptr->lac;

      mmgsdi_memscpy(&(rplmn_info_ptr->rplmn_id),
                     sizeof(mmgsdi_plmn_id_type),
                     &(nitz_info_ptr->rplmn_id),
                     sizeof(mmgsdi_plmn_id_type));

      rplmn_info_ptr->csg_info.csg_id = nitz_info_ptr->csg_info.csg_id;

      *first_stack_rplmn_updated_ptr = TRUE;
    }
  }

  rplmn_info_ptr = &(eons_info_ptr->rplmn_info.sec_stack_rplmn);
  nitz_info_ptr  = &(eons_info_ptr->ss_info.sec_stack_ss.nitz_info);

  if (eons_info_ptr->ss_info.sec_stack_ss.is_valid &&
      nitz_info_ptr->rplmn_available &&
      nitz_info_ptr->lac_available)
  {
    if ((rplmn_info_ptr->rplmn_available == FALSE                                ||
         rplmn_info_ptr->lac_available   == FALSE                                ||
         rplmn_info_ptr->lac != nitz_info_ptr->lac                               ||
         (FALSE == mmgsdi_eons_plmn_match(rplmn_info_ptr->rplmn_id,
                                          nitz_info_ptr->rplmn_id))              ||
         rplmn_info_ptr->rplmn_id.rat != nitz_info_ptr->rplmn_id.rat))
    {
      *ss_plmn_id_lac_rat_changed_ptr = TRUE;
    }

    if ((*ss_plmn_id_lac_rat_changed_ptr == TRUE)                                ||
        (nitz_info_ptr->csg_info.csg_id == MMGSDI_EONS_INVALID_CSG_ID &&
         (rplmn_info_ptr->long_name.plmn_name.eons_data.data_len == 0  &&
          rplmn_info_ptr->short_name.plmn_name.eons_data.data_len == 0))         ||

        ((nitz_info_ptr->csg_info.csg_id != MMGSDI_EONS_INVALID_CSG_ID) &&
         ((rplmn_info_ptr->csg_info.csg_id != nitz_info_ptr->csg_info.csg_id)||
          (rplmn_info_ptr->csg_info.hnb_name.length == 0))))
    {
      rplmn_info_ptr->rplmn_available = TRUE;
      rplmn_info_ptr->lac_available   = TRUE;
      rplmn_info_ptr->lac             = nitz_info_ptr->lac;

      mmgsdi_memscpy(&(rplmn_info_ptr->rplmn_id),
                     sizeof(mmgsdi_plmn_id_type),
                     &(nitz_info_ptr->rplmn_id),
                     sizeof(mmgsdi_plmn_id_type));

      rplmn_info_ptr->csg_info.csg_id = nitz_info_ptr->csg_info.csg_id;

      *sec_stack_rplmn_updated_ptr = TRUE;
     }
  }
} /* mmgsdi_eons_nitz_update_rplmn_identifiers */


/*===========================================================================
  FUNCTION MMGSDI_EONS_NITZ_CHECK_RUN_ALGO_SEND_EVT_NEEDED

DESCRIPTION
  Function to determine if EONS algo needs to be run and event needs to be sent
  for a call stack

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_nitz_check_run_algo_send_evt_needed(
  const mmgsdi_eons_per_stack_ss_data_type * ss_info_ptr,
  boolean                                    rplmn_updated,
  mmgsdi_eons_per_stack_rplmn_info_type    * rplmn_info_ptr,
  boolean                                  * run_algo_ptr,
  boolean                                  * send_evt_ptr
)
{
  const mmgsdi_eons_nitz_data_type     * nitz_info_ptr    = NULL;

  if(rplmn_info_ptr == NULL || ss_info_ptr == NULL || run_algo_ptr == NULL ||
     send_evt_ptr == NULL)
  {
    return;
  }

  nitz_info_ptr  = &(ss_info_ptr->nitz_info);
  *run_algo_ptr  = FALSE;
  *send_evt_ptr  = FALSE;

  /* If the RPLMN has been updated then the EONS algo should be run to
     determine the updated name and method used to find the name */
  if (rplmn_updated)
  {
    *run_algo_ptr = TRUE;
  }
  else
  {
    /* If the RPLMN has not been updated then there are certain requirements to
       follow to determine if the EONS algo needs to be run and evt sent.
       Here are the rules about running the EONS algo:
       1. If roaming status is not matching with home network status. Note that
          while running EONS algorithm it can be found that RPLMN matches with
          SPDI list though here we identify that roaming status is changed from
          OFF to ON.

       2. If lookup was already csg or opl pnn or cphs and rplmn id and lac are
          same (or csg id is valid and same) don't run algo

       3. If lookup was nitz or static table or MCC/MNC string
          a. If csg id is same and nitz has empty csg name don't run algo
          b. If rplmn id and lac are same and new nitz is empty don't run algo
          c. If rplmn id and lac are same and new nitz name is same don't run algo

       4. If lookup was nitz or static table or MCC/MNC string
          a. If csg id was valid but nitz csg id is invalid it's a transition of
             macro -> femto -> macro so clear the csg info and run algo
          b. If rplmn id are lac are same but new nitz name is different run algo
    */

    if ((ss_info_ptr->roam_status == SYS_ROAM_STATUS_ON &&
         rplmn_info_ptr->is_home_network) ||
        (ss_info_ptr->roam_status == SYS_ROAM_STATUS_OFF &&
         rplmn_info_ptr->is_home_network == FALSE))
    {
      *run_algo_ptr = TRUE;
    }
    else if (rplmn_info_ptr->eons_method == MMGSDI_EONS_METHOD_CSG ||
             rplmn_info_ptr->eons_method == MMGSDI_EONS_METHOD_OPL_PNN_LOOKUP ||
             rplmn_info_ptr->eons_method == MMGSDI_EONS_METHOD_CPHS_ONS_LOOKUP)
    {
      *run_algo_ptr = FALSE;
    }
    else /* NITZ or static table or MCC/MNC string */
    {
      if (nitz_info_ptr->csg_info.csg_id != MMGSDI_EONS_INVALID_CSG_ID &&
          rplmn_info_ptr->csg_info.csg_id == nitz_info_ptr->csg_info.csg_id)
      {
        if (nitz_info_ptr->csg_info.hnb_name.length == 0x00)
        {
          *run_algo_ptr = FALSE;
        }
      }
      else if (rplmn_info_ptr->csg_info.csg_id != MMGSDI_EONS_INVALID_CSG_ID &&
               nitz_info_ptr->csg_info.csg_id == MMGSDI_EONS_INVALID_CSG_ID)
      {
        *run_algo_ptr = TRUE;

        /* Macro -> femto -> macro transition. Set the CSG info as invalid */
        rplmn_info_ptr->csg_info.csg_id = MMGSDI_EONS_INVALID_CSG_ID;
        memset(&(rplmn_info_ptr->csg_info.hnb_name),
               0x00,
               sizeof(sys_home_nodeb_name_type));
      }
      else if (nitz_info_ptr->long_name_available == FALSE &&
               nitz_info_ptr->short_name_available == FALSE)
      {
        *run_algo_ptr = FALSE;
      }
      else
      {
        if (TRUE == mmgsdi_eons_match_rplmn_name_with_nitz_name(rplmn_info_ptr,
                                                                nitz_info_ptr))
        {
          *run_algo_ptr = FALSE;
        }
        else
        {
          *run_algo_ptr = TRUE;
        }
      }
    }
  }

  /* Here are the rules about the setting the full service event sent flag:
     1. At boot or during app info re-initialization it would be set to FALSE
     2. If full service event sent flag is FALSE and new SS event indicates
        service status is full flag is set to TRUE and sent event
     3. If service status is currently full and new SS event indicates service
        status has moved to limited the flag is set to FALSE  */

  /* Here are the rules about sending the event:
     1. If algo is being run and service status is full send event
     2. If algo is not being run but service status is full and a full service
          event has not yet been sent */

  if ((ss_info_ptr->srv_status == SYS_SRV_STATUS_LIMITED ||
       ss_info_ptr->srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL) &&
      rplmn_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
  {
    rplmn_info_ptr->full_srv_evt_sent = FALSE;
  }
  else if (ss_info_ptr->srv_status == SYS_SRV_STATUS_SRV &&
           rplmn_info_ptr->full_srv_evt_sent == FALSE)
  {
    *send_evt_ptr = TRUE;
    rplmn_info_ptr->full_srv_evt_sent = TRUE;
  }

  if (*run_algo_ptr == TRUE &&
      ss_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
  {
    *send_evt_ptr = TRUE;
  }

  UIM_SELECTIVE_MSG_HIGH_3(UIM_LOG_EONS,
                           "Curr EONS method 0x%x, run algo 0x%x, send evt 0x%x",
                           rplmn_info_ptr->eons_method, *run_algo_ptr, *send_evt_ptr);
} /* mmgsdi_eons_nitz_check_run_algo_send_evt_needed */

