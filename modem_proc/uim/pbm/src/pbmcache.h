#ifndef PBMCACHE_H
#define PBMCACHE_H

/*===========================================================================

                  INTERNAL HEADER FILE FOR PBM CACHE UTILS

DESCRIPTION
  This is the internal header file for Phonebook Manager cache functions.

  Copyright (c) 2002 - 2007,2009-2010,2014-2015 by QUALCOMM Technologies, Inc(QTI)  
  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbmcache.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/16   sp      Logging improvements
05/26/15    stv    Need not include AAS/GAS in FDN enable/disable clearing
07/07/14   NR      Use v2.0 diag macros
04/30/10  krishnac initialize the caches with proper pb_id
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/04/09   krishna changes for CR 167785
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
02/22/07   cvs     Keep track of filetype for read + write ops
01/16/07   cvs     Add event for phonebooks becoming ready
10/03/06   cvs     Add support for MBN
07/24/06   cvs     Fixes for unequal size multi pb set support
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
02/28/06   cvs     Add support for MBDN
11/15/05   cvs     Lint fixes
08/12/05   AT      Added UCS2 support for strings.
03/23/05   cvs     compilation fixes
03/22/05   PA      Do not update unique id for deletion/update.
                   Changed PBM_MAX_PBR_RECS to PBM_MAX_NUM_PB_SETS.
02/08/05   PA      Support for MSISDN
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
09/29/04   PA      Initial revision.
===========================================================================*/
#include "pbmuim.h"



/* Status of phonebook */
typedef enum
{
   PBM_STATUS_NOT_INIT,        /* Not Initialized */
   PBM_STATUS_NO_SIM=PBM_STATUS_NOT_INIT, /* SIM not inserted */
   PBM_STATUS_PIN_REQ,         /* SIM Requires PIN1 */
   PBM_STATUS_PUC_REQ,         /* SIM Requires PUC1 */
   PBM_STATUS_READY_PUC_REQ,   /*Cache already initialized,but PUC1 required to validate it */   
   PBM_STATUS_PIN2_REQ,        /* SIM Requires PIN2 */
   PBM_STATUS_PUC2_REQ,        /* SIM Requires PUC2 */
   PBM_STATUS_SIM_FAIL,        /* SIM Failure */
   PBM_STATUS_NOT_AVAILABLE,   /* SIM file not available */
   PBM_STATUS_RESTRICTED,      /* SIM file restricted */
   PBM_STATUS_WRITE_SYNC,            /* SIM synchronizing */
   PBM_STATUS_INIT_SYNC,
   PBM_STATUS_NOT_READY,       /* Initializing data */
   PBM_STATUS_READY            /* All set to go */
} pbm_status_e_type;

typedef enum
{
   PBM_CACHE_TYPE_UNKNOWN,
   PBM_CACHE_TYPE_PB,
   PBM_CACHE_TYPE_EXT,
   PBM_CACHE_TYPE_USIM,
   PBM_CACHE_TYPE_INDEX,
   PBM_CACHE_TYPE_SYNC1,
   PBM_CACHE_TYPE_SYNC2,
   PBM_CACHE_TYPE_MAX
} pbm_cache_type_e_type;



typedef struct
{
   pbm_file_type     file_id; /* this file_id is used to fetch the cache etc for particular file */
   mmgsdi_file_structure_enum_type  file_type; /* MMGSDI file type, telling us */
                                               /* how to read + write it */
   int                        num_of_rec;
   int                        ext_rec_length;
   pbm_ext_rec_in_use_s_type *in_use_array;
} pbm_ext_cache_s_type;


/* The puid variable stores the previously used unique identifier. This is
 * used only for UIM entries since EFS based phone books use a global unique
 * identifier that spans across all phonebooks stored in EFS.
 * */

typedef struct {
  mmgsdi_file_security_enum_type   protection_method;
    /**< Protection method: 
     - MMGSDI_ALWAYS_ALLOWED -- num_protection_pin = 0 and protection_pin = NULL
     - MMGSDI_NEVER_ALLOWED -- num_protection_pin = 0 and protection_pin = NULL
     - MMGSDI_AND_ALLOWED -- More than one PIN is required
     - MMGSDI_OR_ALLOWED -- One of multiple PINs is required
     - MMGSDI_SINGLE_ALLOWED -- Only one PIN is required
     */
  uint32           pin_protection_mask; /**< Mask for PINSs to be used
                                                                    bit1 : PIN1
                                                                    bit2 : PIN2
                                                                    bit3 : ADM 
                                                                    rest of the bits are reserved for future use */  
  int32 num_protection_pin;  																
} pbm_file_security_type;


typedef struct pb_cache
{
   pbm_phonebook_type      pb_id;
   pbm_status_e_type       status;          /* Status of cache */
   boolean                 circular;        /* TRUE for circular caches */
   int                     text_length[PBM_MAX_NUM_PB_SETS];     /* Maximum text length */
   int                     file_length;     /* length of UIM file */
   int                     num_of_records;  /* Total records possible */
   int                     record_length[PBM_MAX_NUM_PB_SETS];   /* length of each record */
   int                     records_in_use;  /* Number of records in use */
   int                     start;           /* Next start location (circular) */
   pbm_addr_cache_s_type **pb_cache_array;
   pbm_file_type  file_ids[PBM_MAX_NUM_PB_SETS];
   mmgsdi_file_structure_enum_type  file_type; /* MMGSDI file type, telling us */
                                               /* how to read + write it */
   int                     records_in_pb_set[PBM_MAX_NUM_PB_SETS];
   /* the number of records in each PB set */
   int                     num_files;       /* Number of phonebooks (ADN/ADN1) */
   PB_MASTER_USIM_CACHE   *m_usim_cache[PBM_MAX_NUM_PB_SETS];
   /* USIM Info */
   PB_MASTER_SYNC_CACHE   *m_sync_cache;   /* Synchronization info */
   pbm_ext_cache_s_type   *extension_cache; /* The SIM extension cache */
   pbm_file_security_type        write;    /**< contains info about what PINs are required to write into Phonebook */
} PB_CACHE;

#define PBM_BITS_IN_PIN_PROTECTION_MASK 32

/* Location 0 is never used for storing entries. New entries are added to the
 * start of the queue. Typically entries are not deleted but they are
 * overwritten as the the queue grows. The queue grows from right to left so
 * that first location contains the latest entry.
 */
#define PBM_CIRC_CACHE_NEWLOC(cache)   ((cache)->start)

/* for (i = 1; i <= cache->records_in_use; i++)
 *   nth = cache->pb_cache_array[PBM_CIRC_CACHE_NTHLOC(cache, i)];
 */
#define PBM_CIRC_CACHE_NTHLOC(cache, n) \
                         (((n) && (n) <= (cache)->records_in_use) ? /* valid */\
                           (((cache)->start + (n) > (cache)->num_of_records) ? \
                              (cache)->start + (n) - (cache)->num_of_records : \
                                                       (cache)->start + (n)) : \
                           (0))                                    /* invalid */

/*lint -emacro(506, PBM_CIRC_CACHE_FIRSTLOC) */
#define PBM_CIRC_CACHE_FIRSTLOC(cache) PBM_CIRC_CACHE_NTHLOC((cache), 1)

#define PBM_CIRC_CACHE_LASTLOC(cache)  \
                       PBM_CIRC_CACHE_NTHLOC((cache), ((cache)->records_in_use))

/* inverse of PBM_CIRC_CACHE_NTHLOC */
#define PBM_CIRC_CACHE_LOCTOINDEX(cache, n) (((n) - (cache)->start > 0) ? \
                                                 ((n) - (cache)->start) : \
                      (((n) - (cache)->start) + (cache)->num_of_records))

/* Update the start for an Add operation */
#define PBM_CIRC_CACHE_ADDSTART(cache) \
                               ((cache)->start = (((cache)->start - 1) > 0) ? \
                                                  ((cache)->start - 1) : \
                                                  ((cache)->num_of_records))

/* Update the start for a delete operation */
#define PBM_CIRC_CACHE_DELSTART(cache) \
          ((cache)->start = (((cache)->start + 1) > (cache)->num_of_records) ? \
                             1 : ((cache)->start + 1))

/* initialize a SIM cache, including the pb_id */

#define PBM_CACHE_INIT(cache, pbm_file,pb_session_type)  \
{ \
  memset(&(cache[pb_session_type]), 0, sizeof(cache[pb_session_type])); \
  { \
    (cache)[pb_session_type].pb_id =  pbm_file_id_to_pb_id(pb_session_type, (pbm_file)); \
  } \
}


#define PBM_SET_PB_CACHE_READY(cache) \
{ \
  (cache)->status = PBM_STATUS_READY; \
  if (((cache)->pb_id).device_type != PBM_DEFAULT) \
  { \
    pbm_session_event_data_u_type evt_data = {0}; \
    evt_data.pb_id = (cache)->pb_id; \
    UIM_MSG_HIGH_1("PBM cache ready for %d",((cache)->pb_id).device_type); \
    pbm_add_notify(PBM_EVENT_PB_READY, evt_data);\
  } \
}


/*===========================================================================
                     Declare data for the EFS based files
===========================================================================*/
extern PB_CACHE                 efs_cache;            /* Mobile Equip phone */
extern PB_CACHE                 sdl_cache;            /* Speed Dial List */
extern PB_CACHE                 incoming_calls_cache; /* Received calls */
extern PB_CACHE                 outgoing_calls_cache; /* Dailed calls */
extern PB_CACHE                 missed_calls_cache;   /* Missed calls. */

//emergency lists will be there 2 for UIMs
extern PB_CACHE                 ecc_sim_cache[MAX_UIM_APPS];      /* Emergency numbers */
//Network ECC, NV ECC,Hardcoded ECC
extern PB_CACHE                 ecc_other_cache;      /* Emergency numbers */

/*===========================================================================
                     Declare data for the UIM files
This variables will exist whether or not we have any UIMS.  If we don't have
a UIM, the write will fail, but these variables are around because the PBM
interface supports whether the file name whether or not the card is supported
in the build.
===========================================================================*/
extern PB_CACHE                 adn_cache[MAX_UIM_APPS];
extern PB_CACHE                 sdn_cache[MAX_UIM_APPS];
extern PB_CACHE                 fdn_cache[MAX_UIM_APPS];
extern PB_CACHE                 lnd_cache[MAX_UIM_APPS];
extern PB_CACHE                 grpname_cache[MAX_UIM_APPS];
extern PB_CACHE                 aas_cache[MAX_UIM_APPS];
extern PB_CACHE                 oci_cache[MAX_UIM_APPS];
extern PB_CACHE                 ici_cache[MAX_UIM_APPS];
extern PB_CACHE                 mbdn_cache[MAX_UIM_APPS];
extern PB_CACHE                 hidden_cache[MAX_UIM_APPS];

extern PB_CACHE                 msisdn_cache[MAX_UIM_APPS];
extern PB_CACHE                 mbn_cache[MAX_UIM_APPS];



/*===========================================================================
                     Declare data for the UIM extension records.
===========================================================================*/
extern pbm_ext_cache_s_type     ext1_cache[MAX_UIM_APPS];
extern pbm_ext_cache_s_type     ext2_cache[MAX_UIM_APPS];
extern pbm_ext_cache_s_type     ext3_cache[MAX_UIM_APPS];
extern pbm_ext_cache_s_type     ext5_cache[MAX_UIM_APPS];
extern pbm_ext_cache_s_type     ext6_cache[MAX_UIM_APPS];

extern PB_MASTER_USIM_CACHE     adn_m_usim_cache[MAX_UIM_APPS][PBM_MAX_NUM_PB_SETS];

extern PB_USIM_CACHE            sne_cache[MAX_UIM_APPS][PBM_MAX_NUM_PB_SETS];
extern PB_USIM_CACHE            anr_cache[MAX_UIM_APPS][PBM_MAX_NUM_SAME_USIM_FILES*PBM_MAX_NUM_PB_SETS];
extern PB_USIM_CACHE            email_cache[MAX_UIM_APPS][PBM_MAX_NUM_PB_SETS];
extern PB_USIM_CACHE            grp_cache[MAX_UIM_APPS][PBM_MAX_NUM_PB_SETS];

extern PB_MASTER_SYNC_CACHE     adn_m_sync_cache[MAX_UIM_APPS];

extern PB_SYNC1_CACHE           psc_cache[MAX_UIM_APPS];
extern PB_SYNC1_CACHE           cc_cache[MAX_UIM_APPS];
extern PB_SYNC1_CACHE           puid_cache[MAX_UIM_APPS];
extern PB_SYNC2_CACHE           uid_cache[MAX_UIM_APPS];
extern PB_SYNC2_CACHE           pbc_cache[MAX_UIM_APPS];


void pbm_fill_pb_cache_info(PB_CACHE *cache,
                                   pbm_session_enum_type session_type,
                                   pbm_pb_set_type pb_set,
                                   int record_length,
                                   int min_record_length,
                                   pbm_file_type file_id,
                                   mmgsdi_file_structure_enum_type file_type,
                                   boolean increment_num_files,
                                   int num_records,
                                   const mmgsdi_file_security_type        *write);

/*===========================================================================
FUNCTION PBM_CACHE_RECORD_NEW

DESCRIPTION
  This function allocates a new cache record, initializes it 0 and sets the
  number of reference to 1.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_addr_cache_s_type *pbm_cache_record_new(void);

/*===========================================================================
FUNCTION PBM_CACHE_RECORD_FILL

DESCRIPTION
  This function populates a cache record with the given values.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_cache_record_fill(pbm_addr_cache_s_type *rec,
                           pbm_record_id_type rec_id, uint16 uid, uint16 cat, uint16 num,
                           pbm_field_s_type *fields);

/*===========================================================================
FUNCTION PBM_CACHE_RECORD_GET

DESCRIPTION
  This function obtains a reference to an existing cache record

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_addr_cache_s_type *pbm_cache_record_get(pbm_record_id_type rec_id);

/*===========================================================================
FUNCTION PBM_CACHE_RECORD_FREE

DESCRIPTION
  This function frees a cache record

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint8 pbm_cache_record_free(pbm_addr_cache_s_type *rec);

/*===========================================================================
FUNCTION PBM_CACHE_INIT

DESCRIPTION
  This function initializes all SIM based caches to zeros.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_cache_init(void);

/*===========================================================================
FUNCTION PBM_CLEAR_CACHE

DESCRIPTION
  This function is called to remove all the entries in the cache.
  It does not change the contents of the SIM, just frees the memory
  associated with the cache.

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
  All cache items are cleared.  Phone books are marked as not initialized.
  Any search item that was conducted will no longer be able to see the
  cache values.
===========================================================================*/
void pbm_clear_cache(PB_CACHE *cache);



/*===========================================================================
FUNCTION PBM_CLEAR_HIDDEN_CACHE

DESCRIPTION
  This function is called to remove all the entries in the cache.
  It does not change the contents of the SIM, just frees the memory
  associated with the cache.

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
  All cache items are cleared.  Phone books are marked as not initialized.
  Any search item that was conducted will no longer be able to see the
  cache values.
===========================================================================*/
void pbm_clear_hidden_cache(pbm_session_enum_type session_type);
/*===========================================================================
FUNCTION PBM_CLEAR_USIM_CACHE

DESCRIPTION

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
===========================================================================*/
void pbm_clear_usim_cache(pbm_session_enum_type session_type, PB_USIM_CACHE *cache);

/*===========================================================================
FUNCTION pbm_clear_index_cache

DESCRIPTION

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
===========================================================================*/
void pbm_clear_index_cache(PB_MASTER_USIM_CACHE *m_usim_cache);

/*===========================================================================
FUNCTION PBM_CLEAR_SYNC_CACHE

DESCRIPTION

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
===========================================================================*/
void pbm_clear_sync_cache(PB_MASTER_SYNC_CACHE *m_sync_cache);

/*===========================================================================
FUNCTION PBM_CLEAR_SYNC1_CACHE

DESCRIPTION

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
===========================================================================*/
void pbm_clear_sync1_cache(PB_SYNC1_CACHE *sync1_cache);

/*===========================================================================
FUNCTION PBM_CLEAR_SYNC2_CACHE

DESCRIPTION
  This function will clear the sync2 cache.

DEPENDENCIES
  The caller should call rex_enter_crit_sect(&pbm_crit_sect) Outside

SIDE EFFECTS
===========================================================================*/
void pbm_clear_sync2_cache(PB_SYNC2_CACHE *sync2_cache);

/*===========================================================================
FUNCTION pbm_sim_device_type_to_session_device_type;

DESCRIPTION 
  This API returns the session device based on sim device

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_sim_device_type_to_session_device_type(pbm_device_type device_type);


/*===========================================================================
FUNCTION PBM_PB_ID_TO_CACHE

DESCRIPTION
  This function will find the cache associated with a given pb_id.
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
PB_CACHE *pbm_pb_id_to_cache(pbm_phonebook_type pb_id);

/*===========================================================================
FUNCTION PBM_GET_FIRST_AVAILABLE

DESCRIPTION
Gets the first slot that is empty.  Returns the start index
for circular caches, or returns the index of first unused
area for non-circular.  Finally, returns 0 if not available

In case of deletion of circular cache, this functions returns the last location.
This is not valid for non-circular caches.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_get_first_available(PB_CACHE *pb_cache, boolean delete_only);


/*===========================================================================
FUNCTION PBM_USIM_GET_FIRST_AVAILABLE

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_usim_get_first_available(PB_USIM_CACHE *usim_cache);

/*===========================================================================
FUNCTION PBM_CACHE_ADD

DESCRIPTION
Adds the given record to the given cache.  First allocates a new record
and copies over the info. from the given record.

If a record already exists at the location specified by the record ID, then
the existing record is first deleted before the new record is added at that
same location.

If the "location" part of the record ID is 0, then the record is placed in
the first available location and the record ID is updated accordingly.  (This
is a 1-based array.)

new_flag is used to support updating existing records.

DEPENDENCIES
  Must be called from within pbm critical section.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_cache_add(PB_CACHE *pb_cache,
                              pbm_addr_cache_s_type *record, boolean new_flag);

/*===========================================================================
FUNCTION PBM_CACHE_DELETE

DESCRIPTION
  Deletes the record with the given record ID from the given cache.  The record
  is also free'ed.

DEPENDENCIES
  Must be called from within pbm critical section.

  Make sure that USIM fields are overwritten/deleted before the
  corresponding ADN fields. The field data resides as part of ADN record
  except after init.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_cache_delete(PB_CACHE *pb_cache, pbm_record_id_type record_id);


/*===========================================================================
FUNCTION PBM_USIM_CACHE_ADD

DESCRIPTION

DEPENDENCIES
  Must be called only from PBM task
  and it must be called from within pbm critical section.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_cache_add(pbm_session_enum_type session_type,
                                   PB_USIM_CACHE *usim_cache,
                                   uint16 location,
                                   pbm_usim_cache_s_type *record, boolean init);

/*===========================================================================
FUNCTION PBM_USIM_CACHE_DELETE

DESCRIPTION


DEPENDENCIES
  Must be called from within pbm critical section.

  Make sure that USIM fields are overwritten/deleted before the
  corresponding ADN fields. The field data resides as part of ADN record
  except after init.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_cache_delete(pbm_session_enum_type session_type,
                                      PB_USIM_CACHE *usim_cache,
                                      uint16 location, boolean init);

/*===========================================================================
FUNCTION pbm_master_usim_cache_set

DESCRIPTION


DEPENDENCIES
  Must be called from within pbm critical section.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_master_usim_cache_set(pbm_session_enum_type session_type,
                                          PB_USIM_CACHE *usim_cache,
                                          pbm_adn_record_id_type adn_recid,
                                          uint8 location);
/*===========================================================================
FUNCTION pbm_clear_and_set_session_caches

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_clear_and_set_session_caches(pbm_session_enum_type session_type,
                                      pbm_status_e_type status,
                                      boolean clear_adn_fdn_files_only);
/*===========================================================================
FUNCTION pbm_set_session_caches_status

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_set_session_caches_status(pbm_session_enum_type session_type,
                                      pbm_status_e_type status);

/*===========================================================================
FUNCTION PBM_CLEAR_SIM_CACHES

DESCRIPTION
  This function is called to remove all the entries in the cache
  related to the SIM.  It does not change the contents of the SIM,
  just frees the memory associated with the cache.

DEPENDENCIES
  None.

SIDE EFFECTS
  All cache items are cleared.  Phone books are marked as not initialized.
  Any search item that was conducted will no longer be able to see the
  cache values.
===========================================================================*/
void pbm_clear_sim_caches(void);

/*===========================================================================
FUNCTION PBM_CLEAR_USIM_CACHES

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_clear_usim_caches(pbm_session_enum_type session_type);
/*===========================================================================
FUNCTION PBM_CACHE_RECORD_ADD

DESCRIPTION
  Takes in an ADN record, adds USIM fields corresponding to it and adds it to
  cache.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_cache_record_add( pbm_phonebook_type pb_id,
                              const pbm_num_text_record_s_type *record,
                              int location );

/*===========================================================================
FUNCTION PBM_USIM_CACHE_RECORD_ADD

DESCRIPTION
  Takes in a generic SIM record, converts it to a USIM field and adds it to the
  appropriate cache.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_usim_cache_record_add(PB_USIM_CACHE *usim_cache,
                                  const pbm_sim_record_s_type *record);

/*===========================================================================
FUNCTION PBM_USIM_FIELDS_ADD

DESCRIPTION
  Takes in a generic SIM record, converts it to a USIM field and adds it to the
  appropriate cache.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_cache_fields_add(pbm_session_enum_type session_type,
                                          PB_USIM_CACHE *usim_cache,
                                          uint16 location,
                                          pbm_record_id_type adn_recid,
                                          const pbm_field_s_type *fields,
                                          int num_fields);

/*===========================================================================
FUNCTION PBM_UIM_GENERATE_SYNC_VALUES

DESCRIPTION
  Generates a new unique id if generate_uid is TRUE.  Also generates other
  sync values ( change counter and psc) for the UIM cache entry regardless.
  Generation of new unique id is done based on previously used unique id.
  If generate_uid is FALSE, the function returns 0.

  Note: Unique ids does not have to be contiguous. So if addition of this
  record fails, we do not have to set puid back to previous value.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_uim_generate_sync_values(PB_CACHE *cache,
                                    pbm_write_sync_s_type *sync,
                                    boolean generate_uid);


/*===========================================================================
FUNCTION pbm_usim_update_fields

DESCRIPTION
  Update the field pointers within cache to point to the specified fields
  array. Returns number of fields updated.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_usim_update_fields(pbm_usim_cache_s_type *cache,
                              const pbm_field_s_type *fields);

/*===========================================================================
FUNCTION PBM_MASTER_USIM_CACHE_UPDATE

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_master_usim_cache_update(
                                            pbm_record_id_type rec_id,
                                            const uint8 *prev_index,
                                            const uint8 *curr_index);


/*===========================================================================
FUNCTION pbm_delete_hidden_contacts_from_adn_cache

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_delete_hidden_contacts_from_adn_cache(
                                            pbm_session_enum_type session_type);

/*===========================================================================
FUNCTION pbm_merge_hidden_contacts

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

pbm_return_type pbm_merge_hidden_contacts(pbm_session_enum_type session_type);


#endif /* PBMCACHE_H */
