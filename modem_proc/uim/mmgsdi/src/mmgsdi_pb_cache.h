#ifndef MMGSDI_PB_CACHING_H
#define MMGSDI_PB_CACHING_H
/*===========================================================================


         M M G S D I  P B  R E L A T E D   F U N C T I O N S H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.1.1/mmgsdi/src/mmgsdi_pb_caching.h

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/10/16   sp      Delete PB cache for the file that is being written currently
05/18/15   nr      Fix to distiguish reads on different apps in single slot
04/28/15   nr      Avoid accessing freed globals after session de-activation
02/19/15   nr      Phonebook record read optimization

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"
#include "mmgsdilib_common.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PB_RECORD_ELEMENT_DATA_TYPE
   DESCRIPTION:
      This structure holds each PB record node infomation.
-----------------------------------------------------------------------------*/
typedef struct mmgsdi_pb_record_element{
  mmgsdi_rec_num_type              rec_num;/* Record number */
  mmgsdi_data_type                 record_data; /* Raw data */
  struct mmgsdi_pb_record_element  *next_ptr;
}mmgsdi_pb_record_element_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PB_CACHE_ELEMENT_TYPE
   DESCRIPTION:
      This structure holds each PB file node infomation.
-----------------------------------------------------------------------------*/
typedef struct mmgsdi_pb_cache_element{
  /* MMGSDI Session type */
  mmgsdi_session_type_enum_type    session_type;
  /* File path */
  mmgsdi_path_type                 file_path;
  /*
     This is used to resume the record caching when file switching from one to 
     another (Example below)

     (1) Client issue read for record #1 on file 1.MMGSDI caches record #2 to #10
           before switches to file 2 and stops caching of file 1 and update last record
           cached to #10 here.
     (2) Client issue read for record #1 on file 2.MMGSDI caches record #2 to #5
           before switches back to file 1 and stops caching of file 2 and update last
           record cached to #5 here.
     (3) Client now issue read for record #2 on file 1 MMGSDI has to be give data
           from cache for record #2 and resume PB record caching from record #11
           onwards without waiting for the READ REQUEST for record #11 from client
           to trigger caching.

       SET       > When we detect file path is different from the current file being cached
                   i.e set to the record number in READ response handling.
       RESET     > When the file caching reaches end (i.e cached max no of records present in file)
  */
  mmgsdi_rec_num_type              last_record_cached;
  /*
     This flag is used to determine file cahing is in progress or not and resume
     in case of file switching from one file to another

     1)SET     >   when we issue read request to the card..
     2)RESET   >   When file path switches from one file to another (Lets say File 1 to file 2)
                   When the file caching reaches end (i.e cached max no of records present in file)
  */
  boolean                          caching_in_progress;
  /* Search Data */
  boolean                          *in_use_recs_ptr;
  /* Head node for PB record list */
  mmgsdi_pb_record_element_type    *item_ptr;
  struct mmgsdi_pb_cache_element   *next_ptr;
}mmgsdi_pb_cache_element_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PB_RECORD_ELEMENT_DATA_TYPE
   DESCRIPTION:
      This structure contains list of PB files.
-----------------------------------------------------------------------------*/
typedef struct {
  /* Current File path */
  mmgsdi_path_type               curr_file_path;
  /* Head node for File node list */
  mmgsdi_pb_cache_element_type  *item_ptr;
}mmgsdi_pb_cache_type;

/* MMGSDI Globals to store the PB cache data */
extern mmgsdi_pb_cache_type *mmgsdi_pb_cache_tbl_ptr[MMGSDI_MAX_NUM_SLOTS];

/*===========================================================================
FUNCTION MMGSDI_PB_FREE_PB_CACHE_DATA

DESCRIPTION
  This function clears all phonebook related cache data.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_free_cache_data(
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_session_id_type   session_id,
  boolean                  delete_all_sessions
);

/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_GET_RECORD

DESCRIPTION
  This function is to get PB record from the cache content.

DEPENDENCIES
  None

RETURNS
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_pb_cache_get_record(
  mmgsdi_slot_id_enum_type  slot,
  mmgsdi_rec_num_type       rec_num,
  const mmgsdi_access_type  *file_access_ptr,
  mmgsdi_data_type          *record_data_holder,
  mmgsdi_session_id_type    session_id
);

/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_RECORDS

DESCRIPTION
  This function is to trigger phonebook record caching.

DEPENDENCIES
  None

RETURNS
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_pb_cache_records(
  const mmgsdi_access_type  *file_access_ptr,
  mmgsdi_slot_id_enum_type  slot_id,
  mmgsdi_rec_num_type       rec_num,
  mmgsdi_session_id_type    session_id
);

/*===========================================================================
FUNCTION MMGSDI_PB_FREE_PB_CACHE_DATA

DESCRIPTION
  This function frees phonebook related cache data.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_init_cache_data(
  void
);

/*===========================================================================
FUNCTION MMGSDI_PB_FREE_FCN_CACHE

DESCRIPTION
  This function frees the Phonebook data for the phonebook that are refreshed currently.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_free_fcn_cache(
  uint8                                          slot_index,
  const mmgsdi_internal_refresh_file_list_type  *refresh_files_ptr,
  mmgsdi_session_id_type                         session_id
);

/*===========================================================================
FUNCTION MMGSDI_PB_FREE_EF_CACHE

DESCRIPTION
  This function frees the Phonebook data for the file being passed to this.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_free_ef_cache(
  mmgsdi_path_type                  path_type,
  mmgsdi_session_id_type            session_id,
  mmgsdi_slot_id_enum_type          slot
);

#endif /* MMGSDI_PB_CACHING_H */
