#ifndef MMGSDICACHE_H
#define MMGSDICACHE_H
/*===========================================================================


           M M G S D I   C A C H E   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2011-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdicache.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
01/07/16   kv      UIM PSM handling updates
01/06/16   bcho    Don't delete attr cache if not re-cached during NAA Refresh
01/06/16   bcho    EF-ECC cache creation from PSM SFS data
09/30/15   ar      Fake the fourth byte of EF AD for buggy card
08/10/15   vdc     Retry read request for non spec compliant cards
07/28/14   vv      Remove unused function
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
06/16/14   av      Delete common attributes cache at card error
04/17/14   av      Enhance MMGSDI attributes cache
04/06/14   am      Added mmgsdi heap alloc clean up logic
04/03/14   am      Fix compile error on SBM due to assert_on_crit_sect_owner
12/16/13   tkl     decouple caching with uim item type
12/16/13   tl      Prevent initalizing the cache with partial files
10/28/13   kb      Add offtarget support to check mutex is owned or not
10/16/13   df      Removing unused function
06/24/13   vdc     Added support for triple SIM
10/21/12   av      Added support for new ECC event
10/13/12   abg     Add utility to delete the contents of the common file cache
09/25/12   av      Skip reading EF-ARR for internal selects
02/18/12   av      Cleaned up remaining ALWAYS ON features
12/21/11   kk      Legacy GSDI removal updates
12/21/11   kk      Added USIM ECC caching support
10/28/11   yt      Initial revision

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/


#include "uim.h"
#include "mmgsdi.h"

/*--------------------------------------------------------------------------
                             Global Variables
--------------------------------------------------------------------------*/
extern mmgsdi_attr_cache_type *mmgsdi_slot1_common_cache_tbl_ptr;
extern mmgsdi_attr_cache_type *mmgsdi_pri_gw_cache_tbl_ptr;
extern mmgsdi_attr_cache_type *mmgsdi_pri_1x_cache_tbl_ptr;

extern mmgsdi_attr_cache_type *mmgsdi_slot2_common_cache_tbl_ptr;
extern mmgsdi_attr_cache_type *mmgsdi_sec_gw_cache_tbl_ptr;
extern mmgsdi_attr_cache_type *mmgsdi_sec_1x_cache_tbl_ptr;

extern mmgsdi_attr_cache_type *mmgsdi_slot3_common_cache_tbl_ptr;
extern mmgsdi_attr_cache_type *mmgsdi_ter_gw_cache_tbl_ptr;
extern mmgsdi_attr_cache_type *mmgsdi_ter_1x_cache_tbl_ptr;

#define MMGSDI_MAX_ECC_RECORDS            255

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION MMGSDI_CACHE_INIT

DESCRIPTION
  Function called to initialize global variables for MMGSDI cache.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
void mmgsdi_cache_init(
  void
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_READ_LEN

   DESCRIPTION:
     This function return the total len - offset of the cache item

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, it will return the data
     from the first cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_read_len(
  mmgsdi_session_id_type         session_id,
  const mmgsdi_access_type      *file_access_ptr,
  mmgsdi_len_type               *data_len_ptr,
  mmgsdi_len_type                offset,
  mmgsdi_cache_init_enum_type  *cache_state_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_READ_ITEM_LEN

   DESCRIPTION:
     This function return the cache item len

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect_ptr to protect accesses to
     the File cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_read_item_len(
  mmgsdi_file_enum_type         mmgsdi_file,
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_len_type               data_offset,
  mmgsdi_len_type              *total_len_ptr,
  mmgsdi_cache_init_enum_type  *cache_state_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_WRITE

   DESCRIPTION:
     This function converts the mmgsdi file enum type to uim enum type.
     And write data to different caches based on the applications available
     on the card by calling the corresding util write cache for each
     technology

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, all the caches will be
     updated
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_write(
  mmgsdi_session_id_type      slot_id,
  const mmgsdi_access_type   *file_access_ptr,
  mmgsdi_data_type            data,
  mmgsdi_len_type             offset,
  mmgsdi_data_from_enum_type  data_fr_card
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_WRITE_ITEM

   DESCRIPTION:
     This function writes to the cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect_ptr to protect accesses to
     the File cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_write_item(
  mmgsdi_file_enum_type         mmgsdi_file,
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_len_type               data_len,
  mmgsdi_len_type               data_offset,
  const uint8 *                 data_ptr,
  mmgsdi_data_from_enum_type    data_fr_card
);

/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_DELTE_ITEM

   DESCRIPTION:
     This function delete specific file from cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect_ptr to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete_item(
  mmgsdi_session_id_type        session_id,
  uint16                        num_files,
  const mmgsdi_file_enum_type  *mmgsdi_file_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_DELETE_SELECTIVE_CACHE_ITEMS

   DESCRIPTION:
     This function deletes all the cache items except listed in the input
     parameters

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect_ptr to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete_selective_items(
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type,
  const mmgsdi_file_enum_type  *files_not_to_delete_ptr,
  uint16                        num_files
);

/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_DELETE

   DESCRIPTION:
     This function deletes the cache item

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect_ptr to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete(
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_READ

   DESCRIPTION:
     This function converts the mmgsdi file enum type to uim enum type.
     And read data to different caches based on the applications available
     on the card by calling the corresding util write cache for each
     technology

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, it will return the data
     from the first cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_read(
  mmgsdi_session_id_type        session_id,
  const mmgsdi_access_type     *file_access_ptr,
  mmgsdi_data_type             *data_ptr,
  mmgsdi_len_type               offset,
  mmgsdi_cache_init_enum_type  *cache_state_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_CREATE_PROV_FILE_CACHE_CLEAR_CACHES

   DESCRIPTION:
     If the function is invoked due to a Session activation, this function
     creates the provisioning file cache by allocating the
     exact number of memory space required based on the corresponding
     session type and app type.
     If the provisioning file cache already exists, the file cache is
     cleared and allocated memory freed.
     This function also clears the File attributes cache and the record cache.

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect_ptr to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_create_prov_file_cache_clear_caches(
  mmgsdi_session_id_type        session_id,
  boolean                       session_activate
);

/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_DELETE_COMMON_FILE_CACHE

   DESCRIPTION:
     This function deletes all the files in the common file cache for the slot

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete_common_file_cache(
  mmgsdi_slot_id_enum_type slot_id
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_READ_ITEM

   DESCRIPTION:
     This function reads to the cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect_ptr to protect accesses to
     the File cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_read_item (
  mmgsdi_file_enum_type          mmgsdi_file,
  mmgsdi_app_enum_type           app_type,
  mmgsdi_session_type_enum_type  session_type,
  mmgsdi_len_type                requested_data_len,
  mmgsdi_len_type                data_offset,
  mmgsdi_len_type *              actual_data_len_ptr,
  uint8  *                       data_ptr,
  mmgsdi_cache_init_enum_type   *cache_state_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_ADD_RECORD

   DESCRIPTION:
     This function check if the data requested to be added already exist in
     the cache or not, if not, it will add the data to the cache

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_add_record(
  mmgsdi_session_id_type  session_id,
  mmgsdi_access_type      access,
  mmgsdi_len_type         rec_num,
  const mmgsdi_data_type *data_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_GET_RECORD

   DESCRIPTION:
     This function get the data from the record cache if a match is found

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_get_record(
  mmgsdi_session_id_type session_id,
  mmgsdi_access_type     access,
  mmgsdi_len_type        rec_num,
  mmgsdi_data_type      *data_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_DELETE_RECORD

   DESCRIPTION:
     This function delete the record cache requested by the caller

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete_record(
  mmgsdi_session_id_type session_id,
  mmgsdi_access_type     access,
  mmgsdi_len_type        rec_num
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_DELETE_RECORD_SELECTIVE_ITEMS

   DESCRIPTION:
     This function delete the entire record cache except listed in the input
     parameters' "Not to delete" list

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_cache_delete_record_selective_items(
  mmgsdi_session_id_type        session_id,
  const mmgsdi_file_enum_type  *files_not_to_delete_ptr,
  uint16                        num_files
);

/* ===========================================================================
   FUNCTION:      MMGSDI_CACHE_GET_RECORD_LEN

   DESCRIPTION:
     This function will determine if the file and record are cached and return
     the record len in the pointer provided

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_get_record_len(
  mmgsdi_session_id_type session_id,
  mmgsdi_access_type     access,
  mmgsdi_len_type        rec_num,
  mmgsdi_len_type       *rec_len_p
);

/*===========================================================================
FUNCTION MMGSDI_CACHE_DELETE_PROV_FILE_ATTR_CACHE

DESCRIPTION:
  This function clears the provisioning session specific file attributes cache
  except listed in the input parameters' "Not to delete" list

DEPENDENCIES:
  None

LIMITATIONS:
   None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None

CRITICAL SECTIONS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete_prov_file_attr_cache(
  mmgsdi_session_type_enum_type session_type,
  const mmgsdi_file_enum_type  *files_not_to_delete_ptr,
  uint16                        num_files
);

/*===========================================================================
FUNCTION MMGSDI_CACHE_POPULATE_ATTR_CACHE_IF_NEEDED

DESCRIPTION
  Check the Get File Attribute Cache, if there is any unknown security
  access condition, it will perform a get file attributes on that file

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

CRITICAL SECTIONS:
  The function uses mmgsdi_cache_crit_sect_ptr to protect accesses to the
  File attributes cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_populate_attr_cache_if_needed(
  mmgsdi_client_id_type            client_id,
  mmgsdi_session_id_type           session_id,
  mmgsdi_slot_id_enum_type         slot,
  boolean                          skip_uicc_arr,
  boolean                          is_select_mandatory,
  const mmgsdi_access_type        *access_ptr,
  mmgsdi_attr_cache_element_type **cache_attr_pptr,
  mmgsdi_sw_status_enum_type      *sw_present_ptr
);

/*===========================================================================
FUNCTION MMGSDI_CACHE_DELETE_FILE_ATTR_CACHE_ITEM

DESCRIPTION:
  This function clears specific file attributes cache item

DEPENDENCIES:
  None

LIMITATIONS:
   None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mmgsdi_cache_delete_file_attr_cache_item(
  mmgsdi_attr_cache_element_type ** attr_cache_element_pptr
);

/*===========================================================================
FUNCTION MMGSDI_CACHE_INIT_USIM_ECC

DESCRIPTION
  This function determines the number of ECC records and caches all the records
  upto a maximum of MMGSDI_MAX_ECC_RECORDS (255) records.
  An ECC event is also sent out.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_init_usim_ecc(
  mmgsdi_session_id_type       session_id,
  mmgsdi_slot_id_enum_type     slot_id
);

/*===========================================================================
FUNCTION MMGSDI_CACHE_READ_USIM_ECC

DESCRIPTION
 This function copies ECC data from cache if available

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_read_usim_ecc(
  const mmgsdi_read_req_type   *req_ptr,
  mmgsdi_data_type             *temp_data_holder_ptr
);

/*===========================================================================
FUNCTION MMGSDI_CACHE_GET_ECC_FILE_DATA

DESCRIPTION
 This function copies whole of the ECC data from cache if available.
 The function allocates memory for the ecc data. It is caller's
 responsibility to free the data ptr.
 For RUIM/CSIM/GSM ECC files, the record length returned in ecc_rec_len_ptr
 is zero.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_get_ecc_file_data(
  mmgsdi_session_id_type         session_id,
  mmgsdi_file_enum_type          file_enum,
  mmgsdi_data_type               *ecc_data_ptr,
  mmgsdi_len_type                *ecc_rec_len_ptr
);

/*===========================================================================
FUNCTION MMGSDI_CACHE_ATTR_READ

DESCRIPTION
  Checks for file attributes cache, if found cache ptr is returned in the
  function argument. In the SUCCESS case, *cache_attr_pptr will hold the
  attributes and the calling function should take care of freeing this ptr
  along with its member ptrs.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS
    MMGSDI_ERROR

SIDE EFFECTS
  None

CRITICAL SECTIONS:
  The function uses mmgsdi_cache_crit_sect to protect accesses to
  the File attributes cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_attr_read(
  mmgsdi_session_id_type             session_id,
  mmgsdi_slot_id_enum_type           slot,
  const mmgsdi_access_type          *access_ptr,
  mmgsdi_attr_cache_element_type   **cache_attr_pptr
);

/*===========================================================================
FUNCTION MMGSDI_CACHE_ATTR_WRITE

DESCRIPTION
  Checks for file attributes cache for a file. If the attributes are not
  cached already, it writes the attribtues to the attribtues cache

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS
    MMGSDI_ERROR

SIDE EFFECTS
  None

CRITICAL SECTIONS:
  The function uses mmgsdi_cache_crit_sect to protect accesses to
  the File attributes cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_attr_write(
  mmgsdi_session_id_type                   session_id,
  mmgsdi_slot_id_enum_type                 slot,
  const mmgsdi_access_type                *access_ptr,
  mmgsdi_file_structure_enum_type          file_type,
  mmgsdi_file_size_type                    file_size,
  mmgsdi_len_type                          rec_len,
  mmgsdi_rec_num_type                      num_of_rec,
  uint8                                    sfi,
  const mmgsdi_file_security_access_type * security_access_ptr,
  const boolean                          * increase_ptr
);

/*===========================================================================
FUNCTION MMGSDI_CACHE_DELETE_PKCS15_ATTR_CACHE

DESCRIPTION
 This function deletes all file attributes cached under the PKCS15 DF 7F50.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_cache_delete_pkcs15_attr_cache (
  mmgsdi_slot_id_enum_type  slot_id
);

/*===========================================================================
   FUNCTION      MMGSDI_CACHE_CLEAR_PROV_TYPE_CACHE_ALLOC

   DESCRIPTION:
     Clear transparent and record file cache and file attribute cache
     for provisioning applications

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
==========================================================================*/
void mmgsdi_cache_clear_prov_type_cache_alloc(
  mmgsdi_session_id_type        session_id
);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================
FUNCTION MMGSDI_CACHE_ALLOC_AND_POPULATE_FILE_ATTR_CACHE

INPUT
  mmgsdi_file_security_type  security_status_input
  mmgsdi_file_security_type *security_status_output_ptr

OUTPUT
  void

DESCRIPTION

RETURN VALUE
  void

CRITICAL SECTIONS:
   The calling function must enter mmgsdi_cache_crit_sect crit
   sect when security_status_output_ptr is from attributes cache
   before invoking this util function. Protection for attributes cache.
===========================================================================*/
void mmgsdi_cache_alloc_and_populate_file_attr_cache(
  mmgsdi_file_security_type  security_status_input,
  mmgsdi_file_security_type *security_status_output_ptr
);

/*===========================================================================
FUNCTION MMGSDI_CACHE_CREATE_USIM_ECC_CACHE_PSM_MODE

DESCRIPTION
  To create USIM ECC cache in PSM mode

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_create_usim_ecc_cache_psm_mode(
  mmgsdi_session_type_enum_type    session_type,
  uint8                            num_of_rec,
  const mmgsdi_data_type          *file_data_ptr
);

#endif /* MMGSDICACHE_H */
