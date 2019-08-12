#ifndef PBMEFS_H
#define PBMEFS_H

/*===========================================================================

           INTERNAL HEADER FILE FOR PHONEBOOK MANAGER EFS FUNCTIONS

DESCRIPTION
  This is the internal header file for Phonebook Manager EFS related
  fucntions.

 Copyright (c) 2002 - 2014 by QUALCOMM Technologies, Inc(QTI)  
 All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbmefs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/14   NR      FR 19033: Reduce Code Duplication pbm
04/14/14   stv     EFS write optimisation during garbage collection
03/17/10   Krishna Start using fs_public.h instead fs.h
08/02/06   cvs     Header file cleanup for WINCE
11/15/05   cvs     Lint fixes
09/27/05   cvs     Move size parameters to their own file.
                   Mainline USE_BEST_FIT_MEMORY_MANAGER.  Change field sizes.
08/22/05   AT      Increased number of records for ODM_UI to 500.
07/27/05   AT      Increased maximum size of EFS records again.
06/03/05   cvs     Multiproc cleanup, add metacomments
04/19/05   AT      Changing the #define to allow Sirius ATCOP to use the
                   ME phonebook.
03/28/05   RSB,cvs Added Phone Unique ID functionality
01/10/05   PA      Increased maximum size of EFS records.
12/03/04   PA      Add more space for Java records
09/29/04   PA      Initial revision.
===========================================================================*/

/*===========================================================================
                         DEFINITIONS
===========================================================================*/
#include "pbm_tuning.h"
#include "fs_public.h"
#include "pbmcache.h"

typedef struct {
  uint16 count;
  char buffer[PBM_MAX_EFS_REC_LEN];
  pbm_write_record_s_type *write;
  boolean cb_called;
  boolean deleted;
} pbm_async_efs_buffer;

/*===========================================================================
FUNCTION PBM_EFS_INIT

DESCRIPTION
  Initialize EFS based phonebooks.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_efs_init(void);

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_TO_EFS

DESCRIPTION
Add/updates/deletes a record in the address book in EFS.  This is
the new interface that supports many different fields per contact.

To add a record:
1)  Set rec_id = 0xFFFF (and rec_id_ptr=&rec_id).  After a synchronous call has
    completed, look here for the assigned record ID for the new record.
2)  Set items_ptr equal to an array of fields for the new record
3)  Set item_count equal to the length of above array
    To update a record, do the same as when adding a record (above), except set
    rec_id to the record ID of the record to be updated.  Note that the fields
    given in items_list will replace all fields in the current record, i.e. the
    current fields will be deleted and replaced, not added to.

To delete a record:
1)  Set rec_id to the record ID of the record to be deleted.
2)  Set items_ptr = NULL.
3)  Other fields are not used.

When making an asynchronous call, rec_id_ptr is not used as an output parameter,
and success/failure indication is not given by the return value.  Instead,
success always returned immediately by the function, and then success/failure
indication is given in the first argument to the callback function proc_func().

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_write_to_efs(pbm_write_record_s_type *write);

/*===========================================================================
FUNCTION PBM_EFS_GENERATE_UNIQUE_ID

DESCRIPTION
Generates a Unique ID for a record stored in EFS.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
uint16 pbm_efs_generate_unique_id(void);

/*=======================================================================
/FUNCTION pbm_convert_efs_rec_to_buf

DESCRIPTION
  Converts the record read from EFS to buffer format.

DEPENDENCIES

SIDE EFFECTS

=========================================================================*/

pbm_return_type pbm_convert_efs_rec_to_buf(uint16 *buf_offset, const pbm_addr_cache_s_type *new_record);

/*===========================================================================
FUNCTION PBM_EFS_GARBAGE_RECOLLECT

DESCRIPTION
  Does garbage collection at any point after init stage. Actual
  garbage collection occurs only when the number of records deleted after
  the last reboot exceeds a certain value.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_efs_garbage_recollect(void);

/*===========================================================================
FUNCTION PBM_GET_PHONE_UNIQUE_ID_FROM_RECORD

DESCRIPTION
  Given a cache record, retrieves the PH UID value and places it in the
  ph_unique_id parameter.

  Returns PBM_ERROR if not found or the record has an invalid entry.

DEPENDENCIES

SIDE EFFECTS

COMMENTS

SEE ALSO
===========================================================================*/

pbm_return_type pbm_get_phone_unique_id_from_record(const pbm_addr_cache_s_type *record,
                                                    uint64 *ph_unique_id);

/*===========================================================================
FUNCTION PBM_GET_NEXT_PHONE_UNIQUE_ID

DESCRIPTION
  Gets the next phone unique ID.  Increments the counter in EFS.

DEPENDENCIES
  Must be called from within pbm_crit_sect critical section.

SIDE EFFECTS
===========================================================================*/

pbm_return_type pbm_get_next_phone_unique_id(uint64 *ph_unique_id);

/*===========================================================================
FUNCTION PBM_EFS_CACHE_STRUCTURE_INIT

DESCRIPTION
  Initializes a new cache to contain a given number of records.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_efs_cache_structure_init(PB_CACHE *cache,
                                             pbm_phonebook_type pb_id,
                                             int num_of_records,
                                             boolean circular);

#endif /* PBMEFS_H */
