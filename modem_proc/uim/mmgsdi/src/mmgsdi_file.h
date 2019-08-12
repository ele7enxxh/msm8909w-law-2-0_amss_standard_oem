#ifndef MMGSDI_FILE_H
#define MMGSDI_FILE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I   F I L E   E N U M   F U N C T I O N S


GENERAL DESCRIPTION

  This header file contains the processing of all MMGSDI file mapping related
  functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_file.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/15   vdc     Retry read request for non spec compliant cards
07/14/15   ar      Cache complete data for req's with partial read/write data
04/17/15   nr      Phonebook record read optimization
04/17/14   av      Enhance MMGSDI attributes cache
02/02/14   tkl     Initial version
===========================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "mmgsdilib_common.h"
#include "uim.h"
#include "uim_v.h"

/*=============================================================================

                       DEFINES

=============================================================================*/

#define MMGSDI_INVALID_FILE_SIZE          0x00
#define MMGSDI_INVALID_REC_LEN            0x00

/*---------------------------------------------------------------------------
  MACRO:       MMGSDIFILE_GET_CATEGORY_FROM_FILE
  DESCRIPTION: To get file category enum from the file enum. First nibble
               of the second last byte of file enum contains file category
---------------------------------------------------------------------------*/
#define MMGSDIFILE_GET_CATEGORY_FROM_FILE(mmgsdi_file)                               \
          (mmgsdi_uim_ef_item_category_enum_type)(((uint32)mmgsdi_file >> 8) & 0xF0)

/*---------------------------------------------------------------------------
  MACRO:       MMGSDIFILE_GET_SFI_TABLE_INDEX_FROM_FILE
  DESCRIPTION: To get file index of the table so as to figure out which file
               table to use to extract SFI out of. Second last byte of file
               enum contains that info
---------------------------------------------------------------------------*/
#define MMGSDIFILE_GET_SFI_TABLE_INDEX_FROM_FILE(mmgsdi_file)          \
          (((uint32)mmgsdi_file >> 8) & 0xFF)

/*---------------------------------------------------------------------------
  MACRO:       MMGSDIFILE_GET_TECH_TYPE_FROM_FILE
  DESCRIPTION: To determine the type of file (USIM, CSIM, GSM etc) from the
               file enum. First nibble of the second last byte of file enum
               contains that info
---------------------------------------------------------------------------*/
#define MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(mmgsdi_file)                \
          (((uint32)mmgsdi_file >> 8) & 0xF0)


/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/* ===========================================================================
   STRUCTURE:      MMGSDI_FILE_SFI_TABLE_TYPE

===========================================================================*/
typedef PACKED struct PACKED_POST
{
  mmgsdi_file_enum_type                     mmgsdi_file;
  uint8                                     sfi;
  /* record length for record based file OR total file size for binary file */
  uint8                                     size;
  mmgsdi_file_structure_enum_type           file_type;
} mmgsdi_file_sfi_table_type;


typedef mmgsdi_service_enum_type mmgsdi_file_find_svr_enum_func_type(
  mmgsdi_file_enum_type file,
  mmgsdi_app_enum_type  app_type);

/* ===========================================================================
   STRUCTURE:      MMGSDI_EF_SRV_FROM_APP_TYPE

   DESCRIPTION:
     This structure contains the mmgsdi_file_enum_type and the corresponding
     service table check against this EF for different technologies.
     If there is no governing service for this EF for a particular technology,
     the service enum type should be set to MMGSDI_NONE
===========================================================================*/
typedef struct
{
  mmgsdi_file_enum_type    file;
  mmgsdi_service_enum_type sim_srv;
  mmgsdi_service_enum_type ruim_srv;
  mmgsdi_service_enum_type usim_srv;
  mmgsdi_service_enum_type csim_srv;
} mmgsdi_ef_srv_from_app_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION: MMGSDI_FILE_GET_SRV_FROM_FILE

DESCRIPTION
  Maps MMGSDI File Enum to MMGSDI Service

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_get_srv_from_file(
  mmgsdi_app_enum_type       app_type,
  mmgsdi_file_enum_type      mmgsdi_file,
  mmgsdi_service_enum_type  *mmgsdi_services_type_ptr);

/*===========================================================================
FUNCTION MMGSDI_FILE_GET_PATH_FROM_FILE

DESCRIPTION
  Utility functions for finding out the path for a particular EFs

DEPENDENCIES
  None

RETURN VALUE
  uim_path_type : path
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_get_path_from_file(
  mmgsdi_file_enum_type  mmgsdi_file,
  mmgsdi_path_type      *file_path_ptr);

/*===========================================================================
FUNCTION MMGSDI_FILE_GET_FILE_FROM_PATH

DESCRIPTION
  Utility functions for finding out the mmgsdi enum given a path

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_path_type : path
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_get_file_from_path(
  mmgsdi_app_enum_type   app_type,
  mmgsdi_path_type       path,
  mmgsdi_file_enum_type *mmgsdi_file_ptr);

/* ============================================================================
FUNCTION MMGSDI_FILE_SET_DEFAULT_SEC_ATTR

DESCRIPTION
  This function sets the default security attributes for various file
  access operations

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_file_set_default_sec_attr(
  mmgsdi_file_attributes_type * attr_ptr);

/* ============================================================================
FUNCTION MMGSDI_FILE_SET_DEFAULT_SEC_ATTR_PER_SPEC

DESCRIPTION
  This function sets the default security attributes for various file
  access operations DERIVED FROM the access conditions mentioned in the spec
  for the file. For the files for which the access condition is ADM or NEVER
  (per the spec), the access condition is defaulted to MMGSDI_NEVER_ALLOWED.
  For all other access conditions mentioned in the spec for a file, the access
  condition is defaulted to MMGSDI_ALWAYS_ALLOWED.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_file_set_default_sec_attr_per_spec(
  mmgsdi_file_attributes_type * attr_ptr,
  mmgsdi_file_enum_type         mmgsdi_file);

/* ==========================================================================
FUNCTION MMGSDI_FILE_IS_EF_UNDER_MF

DESCRIPTION
  This function determines if the EF is under MF

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  Boolean: TRUE if EF is under MF else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
extern boolean mmgsdi_file_is_ef_under_mf(
  const mmgsdi_access_type *access_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_FILE_IS_EF_UNDER_DF_IN_MF

DESCRIPTION
  This function determines if the EF is under a directory in MF (other than
  the GSM/RUIM DF and ADFs)

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  Boolean: TRUE if EF is under a directory in MF else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
extern boolean mmgsdi_file_is_ef_under_df_in_mf(
  const mmgsdi_access_type *access_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_FILE_IS_DF_UNDER_MF

DESCRIPTION
  This function determines if the DF passed in is under MF

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  Boolean: TRUE if DF is under MF else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
boolean mmgsdi_file_is_df_under_mf(
const mmgsdi_access_type *access_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_FILE_IS_ACCESS_OUTSIDE_ADF

DESCRIPTION
  This function determines if the EF/DF is outside of ADFs

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  Boolean: TRUE if EF/DF access is allowed, else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
boolean mmgsdi_file_is_access_outside_adf(
  const mmgsdi_access_type *access_ptr
);

/* ============================================================================
FUNCTION MMGSDI_FILE_IS_VALID_ACCESS

DESCRIPTION
  This function takes the session_type and MMGSDI File enum and determines
  if the file access is valid for the session type

DEPENDENCIES
  None

LIMITATIONS
  Supports only Provisioning sessions

RETURN VALUE
  boolean:  TRUE : Valid access
            FALSE: Invalid access

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_file_is_valid_access(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_file_enum_type         mmgsdi_file
);

/*===========================================================================
FUNCTION MMGSDI_FILE_SET_UIM_PATH

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_set_uim_path(
  const mmgsdi_access_type                *mmgsdi_access_ptr,
  uim_path_type                           *uim_path
);

/*===========================================================================
FUNCTION MMGSDI_FILE_SET_UIM_SFI

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_set_uim_sfi(
  mmgsdi_session_id_type                   session_id,
  mmgsdi_slot_id_enum_type                 slot_id,
  const mmgsdi_access_type                *mmgsdi_access_ptr,
  uint8                                   *sfi
);

/* ===========================================================================
   FUNCTION:      MMGSDI_FILE_DETERMINE_EF_ENUM_DIR_LEVELS

   DESCRIPTION:
     This function determines the directory level for the EF enum type
     Result will be populated into the passed in pointer

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_determine_ef_enum_df_levels(
  mmgsdi_file_enum_type       file_enum,
  uim_dir_type               *uim_dir_level_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_FILE_DETERMINE_UIM_DIR_TYPE

   DESCRIPTION:
     This function determines the uim directory type for MF, DF, ADF and EFs

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_file_determine_uim_dir_type (
  const mmgsdi_access_type   *mmgsdi_access_ptr,
  uim_dir_type               *uim_dir_ptr
);

/*===========================================================================
FUNCTION MMGSDI_FILE_IS_SFI_SUPPORTED

DESCRIPTION
  Utility functions for finding out whether the SFI support is present in the
  card and whether SFI usage is enabled in NV or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If
        1. Card ATR mentions that SFI is supported AND
        2. SFI NV item is enabled
  FALSE: Otherwise
===========================================================================*/
boolean mmgsdi_file_is_sfi_supported (
  mmgsdi_slot_id_enum_type         slot_id
);

/* ============================================================================
FUNCTION MMGSDI_FILE_GET_FILE_INFO_FROM_TABLE

DESCRIPTION
  This utility function gets the file info such as rec_len/file_size and the
  file attributes by table lookup via file_enum

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_file_get_file_info_from_table(
  mmgsdi_file_enum_type              mmgsdi_file,
  mmgsdi_file_sfi_table_type        *file_info
);

/* ============================================================================
FUNCTION MMGSDI_FILE_CHECK_FILE_INFO_AND_UPDATE_LEN

DESCRIPTION
  This utility function performs sanity checks on file size / record size of
  file:
  1. For cyclic/linear files:
    a) Get record length of EF
    b) If len passed into the function is greater than record length, truncate
       the len to the record size.
    c) If len passed into the function is less than the record length, return
       ERROR
    d) If len passed into the function is zero, update len to the record length
       that was retrived in step 1a) above.
  2. For transparent files:
    a) Get file size
    b) If len passed into the function is greater than the file size, truncate
       the len.
    c) If len passed into the function is less than the file size, it is a valid
       usecase.
    d) If len passed into the function is zero, update len to the file size
       that was retrived in step 2a) above.

  How is record length for cyclic/linear file or file size for transparent file
  determined:
  1. First a static table look up is performed for the EF.
  2. If record_len/file_size not found there, get the attributes from the
     attributes cache (as part of this step, if the EF is a cacheable EF,
     and if the attributes are not found to be cached yet, attributes are
     read from the card and cached.
  3. If the EF is step 2 is not a cacheable EF, we send a SELECT to the card
     without caching the attributes upon getting a response.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_file_check_file_info_and_update_len(
  mmgsdi_session_id_type             session_id,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_access_type                 access,
  mmgsdi_file_structure_enum_type    file_type,
  mmgsdi_offset_type                 offset,
  mmgsdi_len_type                    *data_len,
  boolean                            is_select_mandatory
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_COMPARE_ACCESS_TYPE

   DESCRIPTION:
     This function compares the two access type parameters and returns true if
     they are the same.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     boolean

     TRUE:          Access paarmeters are same
     FALSE:         Otherwise

   SIDE EFFECTS:

==========================================================================*/
boolean mmgsdi_file_compare_access_type(
  const mmgsdi_access_type  *access_1_ptr,
  const mmgsdi_access_type  *access_2_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_FILE_COMPARE_PATH_TYPE

DESCRIPTION
  This function determines if the EF file path is same as one that we already
  cached.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean: TRUE if EF is matched else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
boolean mmgsdi_file_compare_path_type(
  mmgsdi_path_type      file1,
  mmgsdi_path_type      file2
);

#endif /* MMGSDI_FILE_H */
