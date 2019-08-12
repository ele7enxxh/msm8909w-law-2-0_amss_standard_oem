/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR System Information Cache Module

GENERAL DESCRIPTION
   This module contains functions to handle system information message processing

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_cache.c#1 $ 
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------- 
07/04/2014  RMN     FR 20252 - Full SI caching 
20/01/2014  SJW     Added blacklisting interface
13/05/2013  SJW     Ported to QDSP6
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "rr_sys_info_cache.h"
#include "gprs_mem.h"
#include "gprsdef.h"
#include "rr_general.h"
#include "rr_sys_info_ncell.h"
#include "stringl/stringl.h"
#include "rr_decode_ie.h"
#include "gbitutil.h"
#include "rr_nv.h"
#include "rr_defs.h"

#define RR_CSI_PEND_STORE_SIZE 8

typedef struct
{
  boolean     active;
  csi_entry_t store[RR_CSI_PEND_STORE_SIZE];
} rr_csi_pend_store_t;

#define MAX_NUM_CELLS_TO_CACHE 10
#define MAX_NUM_SI_PER_CELL     15
#define MAX_NUMBER_CACHED_SI    (NUM_GERAN_DATA_SPACES * MAX_NUM_CELLS_TO_CACHE * MAX_NUM_SI_PER_CELL)

#define CSI_SI3_LAI_POS (5) /* see 3GPP 44.018 Table 9.1.35.1   */
#define CSI_SI4_LAI_POS (3) /* see 3GPP 44.018 Table 9.1.36.1   */
#define CSI_LAI_STR_LEN (5) /* see 3GPP 24.008 Section 10.5.1.3 */

/* add SI types here as they are supported in the cache */
#define RR_MSG_IS_SI1(mt) (SYSTEM_INFORMATION_TYPE_1 == (mt))
#define RR_MSG_IS_SI2(mt) (SYSTEM_INFORMATION_TYPE_2 == (mt))
#define RR_MSG_IS_SI2_BIS(mt) (SYSTEM_INFORMATION_TYPE_2_BIS == (mt))
#define RR_MSG_IS_SI2_TER(mt) (SYSTEM_INFORMATION_TYPE_2_TER == (mt))
#define RR_MSG_IS_SI2_QUATER(mt) (SYSTEM_INFORMATION_TYPE_2_QUATER == (mt))
#define RR_MSG_IS_SI3(mt) (SYSTEM_INFORMATION_TYPE_3 == (mt))
#define RR_MSG_IS_SI4(mt) (SYSTEM_INFORMATION_TYPE_4 == (mt))
#define RR_MSG_IS_SI13(mt) (SYSTEM_INFORMATION_TYPE_13 == (mt))

#define IS_CACHED_SYS_INFO_TYPE(mt) (RR_MSG_IS_SI1(mt) || \
                                     RR_MSG_IS_SI2(mt) || \
                                     RR_MSG_IS_SI2_BIS(mt) || \
                                     RR_MSG_IS_SI2_TER(mt) || \
                                     RR_MSG_IS_SI2_QUATER(mt) || \
                                     RR_MSG_IS_SI3(mt) || \
                                     RR_MSG_IS_SI4(mt) || \
                                     RR_MSG_IS_SI13(mt))

#define IS_MULTI_INSTANCE_SI(mt)     (RR_MSG_IS_SI2_TER(mt) || \
                                      RR_MSG_IS_SI2_QUATER(mt))

#define SI2QUATER_INDEX(octet_ptr) ((uint8)((octet_ptr[3] & 0x1e) >> 1))
#define SI2TER_INDEX(octet_ptr) ((uint8)((octet_ptr[19] & 0x1c) >> 2))

#define NUM_SI_TYPES_CACHED 8
#define NUM_SI_TYPES_FAST_ACQ 2

static rr_csi_pend_store_t pend_store[NUM_GERAN_DATA_SPACES];
static csi_entry_t    sys_info_cache[MAX_NUMBER_CACHED_SI];
static uint16         current_counter;
static uint8 si_retrieval_order_normal[NUM_SI_TYPES_CACHED] =  {SYSTEM_INFORMATION_TYPE_1,
                                                                SYSTEM_INFORMATION_TYPE_2,
                                                                SYSTEM_INFORMATION_TYPE_2_BIS,
                                                                SYSTEM_INFORMATION_TYPE_2_TER,
                                                                SYSTEM_INFORMATION_TYPE_2_QUATER,
                                                                SYSTEM_INFORMATION_TYPE_13,
                                                                SYSTEM_INFORMATION_TYPE_4,
                                                                SYSTEM_INFORMATION_TYPE_3
                                                               };

static uint8 si_retrieval_order_fast_acq[NUM_SI_TYPES_FAST_ACQ] = {SYSTEM_INFORMATION_TYPE_1,
                                                                   SYSTEM_INFORMATION_TYPE_3
                                                                  };

#define IS_VALID_OTA_ENTRY(eptr)   ((boolean)(eptr->octet_ptr != NULL))

/*----------------------------------------------------------------------------
 * Static Function Definitions
 * -------------------------------------------------------------------------*/
static void rr_csi_add_entry_to_pend_store(uint8               msg_type,
                                           ARFCN_T             arfcn,
                                           uint8              *msg_ptr,
                                           const gas_id_t      gas_id);

static boolean rr_csi_is_cgi_equal(rr_csi_cgi_t db_entry_cgi, rr_csi_cgi_t target_cgi);

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_csi_pend_store_t*
 */
static rr_csi_pend_store_t *rr_sys_info_cache_get_pend_store_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &pend_store[as_index];
}

/*===========================================================================

FUNCTION  rr_csi_next_counter

DESCRIPTION
  Increment the counter

DEPENDENCIES
  None

RETURN VALUE
  counter

===========================================================================*/
static uint16 rr_csi_next_counter(void)
{
  if (current_counter == 0xffff)
  {
    current_counter = 0;
  }
  else
  {
    current_counter++;
  }

  return current_counter;
}

/*===========================================================================

FUNCTION  rr_csi_delete_oldest_entry

DESCRIPTION
  Searches the database for the oldest entry and deletes it.

DEPENDENCIES
  None

RETURN VALUE
  csi_entry_t* - entry that has been deleted.

===========================================================================*/
static csi_entry_t *rr_csi_delete_oldest_entry(uint16 offset, const gas_id_t gas_id)
{
  uint16 i;
  uint16 before_offset_counter = offset;
  csi_entry_t *lowest_entry_before_offset = NULL;
  uint16 after_offset_counter = 0xffff;
  csi_entry_t *lowest_entry_after_offset = NULL;
  csi_entry_t *flushed_entry = NULL;
  csi_entry_t *entry_ptr = NULL;

  entry_ptr = sys_info_cache;

  /*handle wrap around case*/
  for (i = 0; i < MAX_NUMBER_CACHED_SI; i++, entry_ptr++)
  {
    /* find lowest entry from the offset (current counter) to the max counter*/
    if (entry_ptr->counter > offset &&
        entry_ptr->counter < after_offset_counter)
    {
      after_offset_counter = entry_ptr->counter;
      lowest_entry_after_offset = entry_ptr;
    }

    /* find lowest entry before the offset (current counter)*/
    if (entry_ptr->counter < before_offset_counter)
    {
      before_offset_counter = entry_ptr->counter;
      lowest_entry_before_offset = entry_ptr;
    }
  }

  MSG_HIGH("rr_csi_delete_oldest_entry offset: %d lowest_entry_after_offset: 0x%x, lowest_entry_before_offset: 0x%x",offset, lowest_entry_after_offset, lowest_entry_before_offset);  

  if (lowest_entry_after_offset != NULL)
  {
    rr_csi_flush_data(&lowest_entry_after_offset->cgi.lai,
                      &lowest_entry_after_offset->cgi.ci,
                      CSI_FLUSH_CACHE_FULL,
                      gas_id);
    flushed_entry = lowest_entry_after_offset;
  }
  else if (lowest_entry_before_offset != NULL)
  {
    rr_csi_flush_data(&lowest_entry_before_offset->cgi.lai,
                      &lowest_entry_before_offset->cgi.ci,
                      CSI_FLUSH_CACHE_FULL,
                      gas_id);
    flushed_entry = lowest_entry_before_offset;
  }

  return flushed_entry;
}

/*===========================================================================

FUNCTION  rr_csi_find_free_entry

DESCRIPTION
  Searches the database for an entry with no associated OTA data

DEPENDENCIES
  None

RETURN VALUE
  Pointer to first free entry, or NULL if none was found

===========================================================================*/
static csi_entry_t *rr_csi_find_free_entry(void)
{
  csi_entry_t *entry_ptr;
  int          i;

  entry_ptr = sys_info_cache;
  for (i = 0; i < MAX_NUMBER_CACHED_SI; i++, entry_ptr++)
  {
    if (entry_ptr->octet_ptr == NULL)
    {
      return(entry_ptr);
    }
  }

  return(NULL);
}

/*===========================================================================

FUNCTION  rr_csi_discard_entry_data

DESCRIPTION
  Frees any dynamic memory associated with the specified cache entry

DEPENDENCIES
  None

RETURN VALUE
  1 if an entry was freed, 0 otherwise

===========================================================================*/
static int rr_csi_discard_entry_data(csi_entry_t *entry_ptr)
{
  if (entry_ptr != NULL)
  {
    if (IS_VALID_OTA_ENTRY(entry_ptr))
    {                                                                              
      GPRS_MEM_FREE(entry_ptr->octet_ptr);
      entry_ptr->octet_ptr = NULL;
      return(1);
    }
  }

  return(0);
}


/*===========================================================================

FUNCTION  rr_csi_find_multi_instance_entry

DESCRIPTION
  Searches the database for an entry that matches the CGI and msg_type with
  multi-instance matching index

DEPENDENCIES
  Never returns a blacklist marker entry, by design

RETURN VALUE
  Pointer to entry, or NULL if no match was found. 

===========================================================================*/
static csi_entry_t *rr_csi_find_multi_instance_entry(csi_entry_t *target_ptr)
{
  csi_entry_t *entry_ptr;
  int          i;
  uint8        req_si_index = 0;
  uint8        cached_si_index = 0;

  if (IS_MULTI_INSTANCE_SI(target_ptr->msg_type))
  {
    if (target_ptr->msg_type == SYSTEM_INFORMATION_TYPE_2_QUATER)
    {
      req_si_index = SI2QUATER_INDEX(target_ptr->octet_ptr);
    }
    else if (target_ptr->msg_type == SYSTEM_INFORMATION_TYPE_2_TER)
    {
      uint16 pos = 0; /*first bit of rest octets is L/H bit*/

      if (gunpackb_lh(&target_ptr->octet_ptr[19], &pos) == 1)
      {
        req_si_index = SI2TER_INDEX(target_ptr->octet_ptr);
        MSG_ERROR("rr_csi_find_multi_instance_entry: SI2TER req index:%d",req_si_index,0,0);
      }
      else
      {
        req_si_index = 0;
      }
    }

    entry_ptr = sys_info_cache;
    for (i = 0; i < MAX_NUMBER_CACHED_SI; i++, entry_ptr++)
    {
      if ( (entry_ptr->msg_type == target_ptr->msg_type) &&
            IS_VALID_OTA_ENTRY(entry_ptr)                 &&
            rr_csi_is_cgi_equal(entry_ptr->cgi,target_ptr->cgi))
      {
        if (entry_ptr->msg_type == SYSTEM_INFORMATION_TYPE_2_QUATER)
        {
          cached_si_index = SI2QUATER_INDEX(entry_ptr->octet_ptr);
        }
        else if (entry_ptr->msg_type == SYSTEM_INFORMATION_TYPE_2_TER)
        {
          uint16 pos = 0; /*first bit of rest octets is L/H bit*/

          if (gunpackb_lh(&entry_ptr->octet_ptr[19], &pos) == 1)
          {
            cached_si_index = SI2TER_INDEX(entry_ptr->octet_ptr);
            MSG_ERROR("rr_csi_find_multi_instance_entry: SI2TER found index:%d",cached_si_index,0,0);
          }
          else
          {
            cached_si_index = 0;
          }
        }

        if (cached_si_index == req_si_index)
        {
          return (entry_ptr);
        }
      }
    }
  }

  return(NULL);
}

/*===========================================================================

FUNCTION  rr_csi_find_entry

DESCRIPTION
  Searches the database for an entry that matches the CGI and msg_type

DEPENDENCIES
  Never returns a blacklist marker entry, by design

RETURN VALUE
  Pointer to entry, or NULL if no match was found. 

===========================================================================*/
static csi_entry_t *rr_csi_find_entry(csi_entry_t *target_ptr)
{
  csi_entry_t *entry_ptr;
  int          i;

  entry_ptr = sys_info_cache;
  for (i = 0; i < MAX_NUMBER_CACHED_SI; i++, entry_ptr++)
  {
    /*match CGI*/
    if ((entry_ptr->msg_type == target_ptr->msg_type) &&
        IS_VALID_OTA_ENTRY(entry_ptr)                 &&
        rr_csi_is_cgi_equal(entry_ptr->cgi,target_ptr->cgi))
    {
      return(entry_ptr);
    }
  }

  return(NULL);
}

/*===========================================================================

FUNCTION  rr_csi_find_offset_entry

DESCRIPTION
  Searches the database for an entry that matches a specified target and has
  valid OTA data associated with it

DEPENDENCIES
  Never returns a blacklist marker entry, by design

RETURN VALUE
  Pointer to entry, or NULL if no match was found. 

===========================================================================*/
static csi_entry_t *rr_csi_find_offset_entry(csi_entry_t *target_ptr, uint16 *offset)
{
  csi_entry_t *entry_ptr;
  int          i;

  if (*offset >= MAX_NUMBER_CACHED_SI)
  {
    MSG_GERAN_HIGH_1("offset out of range: offset:%d, nothing to search",offset);
    return(NULL);
  }

  entry_ptr = &sys_info_cache[*offset];
  for (i = *offset; i < MAX_NUMBER_CACHED_SI; i++, entry_ptr++)
  {
    if ((entry_ptr->msg_type == target_ptr->msg_type) &&
        IS_VALID_OTA_ENTRY(entry_ptr)                 &&
        rr_csi_is_cgi_equal(entry_ptr->cgi,target_ptr->cgi))
    {
      *offset = i+1;
      return(entry_ptr);
    }
  }

  return(NULL);
}

/*===========================================================================

FUNCTION  rr_csi_update_entry_internal

DESCRIPTION
  Updates the database with a message received OTA

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the message was successfully added to the cache (or updated)

===========================================================================*/
static boolean rr_csi_update_entry_internal(uint8               msg_type,
                                            ARFCN_T             arfcn,
                                            uint8              *msg_ptr,
                                            uint8               tc,
                                            LAI_T              *lai_ptr,
                                            uint16             *ci_ptr,
                                            const gas_id_t      gas_id)
{
  csi_entry_t *entry_ptr;
  csi_entry_t  new_entry;
  boolean      cached_ok = FALSE;
  rr_csi_pend_store_t *pend_store_ptr = rr_sys_info_cache_get_pend_store_ptr(gas_id);

  RR_NULL_CHECK_FATAL(pend_store_ptr);

  // Check if sys info cache functionality enabled
  if (rr_nv_sys_info_cache_is_enabled(RR_GAS_ID_TO_AS_ID) == FALSE)
  {
    return FALSE;
  }

  if ( (lai_ptr == NULL) ||
       (ci_ptr == NULL) )
  {
    if (pend_store_ptr->active)
    {
      rr_csi_add_entry_to_pend_store(msg_type,
                                     arfcn,
                                     msg_ptr,
                                     gas_id);
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  MSG_HIGH("rr_csi_update_entry for arfcn=%d msg:%d",arfcn.num, msg_type, 0);
  MSG_HIGH("rr_csi_update_entry for plmn=%6x lac=%d, ci=%d", \
                      ( (lai_ptr->plmn_id.identity[0] << 16) + \
                        (lai_ptr->plmn_id.identity[1] << 8)  + \
                         lai_ptr->plmn_id.identity[2]),
                       lai_ptr->location_area_code,
                       *ci_ptr
                      );

	
  /* sanity check input parameters */
  if (!IS_CACHED_SYS_INFO_TYPE(msg_type) || (msg_ptr == NULL))
  {
    return(FALSE);
  }

  /* do not reprocess messages that were generated from the cache itself */
  if (CACHED_SYS_INFO_TC == tc)
  {
    return(FALSE);
  }


  /* construct a new entry with the supplied parameters */
  new_entry.msg_type = msg_type;
  new_entry.si_arfcn = arfcn;
  new_entry.octet_ptr = msg_ptr;
  new_entry.cgi.lai = *lai_ptr;
  new_entry.cgi.ci = *ci_ptr;

  if (IS_MULTI_INSTANCE_SI(msg_type))
  {
    entry_ptr = rr_csi_find_multi_instance_entry(&new_entry);
  }
  else
  {
    /* scan the cache for a matching entry */
    entry_ptr = rr_csi_find_entry(&new_entry);
  }


  if (entry_ptr != NULL)
  {
    if (ARFCNS_EQUAL(arfcn, entry_ptr->si_arfcn) == FALSE)
    {
      /* ARFCN for the CGI has changed, flush the CGI */
      rr_csi_flush_data(lai_ptr, ci_ptr, CSI_FLUSH_ARFCN_CHANGE, gas_id);

      return(FALSE);
    }
  }

  if (entry_ptr != NULL)
  {    
    /* found a match, update it */
    memscpy(entry_ptr->octet_ptr, N201_MAX, msg_ptr, N201_MAX);
    entry_ptr->counter = rr_csi_next_counter();
    MSG_HIGH("rr_csi_update_entry update existing entry: 0x%x, ARFCN:%d, msg_type:%d",entry_ptr,entry_ptr->si_arfcn.num,entry_ptr->msg_type);
    rr_log_csi_update_entry(entry_ptr,CSI_ENTRY_UPDATED,gas_id);
    cached_ok = TRUE;
  }
  else
  {
    /* entry doesn't exist already, so add it now if there is free space */
    entry_ptr = rr_csi_find_free_entry();

    /*database is full, delete the oldest entry (and all other SI's for the same CGI) and save the new SI*/
    if (entry_ptr == NULL)
    {
      entry_ptr = rr_csi_delete_oldest_entry(current_counter,gas_id);
    }

    if (entry_ptr != NULL)
    {
      /* allocate memory for the sys info content */
      new_entry.octet_ptr = (uint8 *)GPRS_MEM_MALLOC(N201_MAX);
      if (new_entry.octet_ptr != NULL)
      {        
        /* store the new entry in the cache */
        memscpy(new_entry.octet_ptr, N201_MAX, msg_ptr, N201_MAX);
        *entry_ptr = new_entry;
        entry_ptr->counter = rr_csi_next_counter();
        MSG_HIGH("rr_csi_update_entry add to free entry: 0x%x, ARFCN:%d, msg_type:%d",entry_ptr,entry_ptr->si_arfcn.num,entry_ptr->msg_type);
        rr_log_csi_update_entry(entry_ptr,CSI_ENTRY_ADDED,gas_id);
        cached_ok = TRUE;
      }
      else
      {
        MSG_GERAN_HIGH_1("Unable to cache SI msg 0x%02X, modem heap unavailable", (int)msg_type);
      }
    }
    else
    {
      MSG_GERAN_ERROR_1("Unable to cache SI msg 0x%02X, database full", (int)msg_type);
    }
  }
  
  return(cached_ok);
}

/*===========================================================================

FUNCTION  rr_csi_update_entry

DESCRIPTION
  Updates the database with a message received OTA

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the message was successfully added to the cache (or updated)

===========================================================================*/
boolean rr_csi_update_entry(uint8               msg_type,
                            ARFCN_T             arfcn,
                            dl_unit_data_ind_T *msg_ptr,
                            LAI_T              *lai_ptr,
                            uint16             *ci_ptr,
                            const gas_id_t      gas_id)
{
  return rr_csi_update_entry_internal( msg_type,
                                       arfcn,
                                       msg_ptr->layer3_message,
                                       msg_ptr->tc,
                                       lai_ptr,
                                       ci_ptr,
                                       gas_id);
}

/*===========================================================================

FUNCTION  rr_csi_create_sys_info_message

DESCRIPTION
  Generates a DL_UNIT_DATA_IND primitive from a cache entry

DEPENDENCIES
  None

RETURN VALUE
  VOID

===========================================================================*/
static void rr_csi_create_sys_info_message(csi_entry_t *entry_ptr, dl_unit_data_ind_T *msg_ptr, gas_id_t gas_id)
{
  if ((entry_ptr != NULL) && (msg_ptr != NULL))
  {
    /* found a match, create a new inter-task signal with the data */
    msg_ptr->message_header.message_set = MS_RR_L2;
    msg_ptr->message_header.message_id  = (byte)DL_UNIT_DATA_IND;

    /* lint -e{778} */
    PUT_IMH_LEN((sizeof(dl_unit_data_ind_T) - sizeof(IMH_T)), &(msg_ptr->message_header));

    msg_ptr->tc                = CACHED_SYS_INFO_TC;
    msg_ptr->SAPI              = SAPI0;
    msg_ptr->short_header      = FALSE;
    msg_ptr->channel_number    = entry_ptr->si_arfcn;
    msg_ptr->l2_channel_type   = BCCH;
    msg_ptr->own_paging_block  = FALSE;
    msg_ptr->l3_message_length = N201_MAX;
    msg_ptr->gas_id            = gas_id;
    msg_ptr->shared_msg        = TRUE;

    memscpy(msg_ptr->layer3_message, sizeof(msg_ptr->layer3_message), entry_ptr->octet_ptr, N201_MAX);
  }

  return;
}

/*===========================================================================

FUNCTION  rr_csi_map_msg_type_to_mask

DESCRIPTION
  Maps a SI message type to an internal SI mask representation

DEPENDENCIES
  None

RETURN VALUE
  uint16 - SI mask

===========================================================================*/
static uint16 rr_csi_map_msg_type_to_mask(byte msg_type)
{
  switch (msg_type)
  {
    case SYSTEM_INFORMATION_TYPE_1:          return MASK_SI_1;
    case SYSTEM_INFORMATION_TYPE_2:          return MASK_SI_2;
    case SYSTEM_INFORMATION_TYPE_2_BIS:      return MASK_SI_2_BIS;
    case SYSTEM_INFORMATION_TYPE_2_TER:      return MASK_SI_2_TER;
    case SYSTEM_INFORMATION_TYPE_2_QUATER:   return MASK_SI_2_QUATER;
    case SYSTEM_INFORMATION_TYPE_13:         return MASK_SI_13;
    case SYSTEM_INFORMATION_TYPE_4:          return MASK_SI_4;
    case SYSTEM_INFORMATION_TYPE_3:          return MASK_SI_3;
    default:                                 ;
  }

  return MASK_SI_NONE;
}

/*===========================================================================

FUNCTION  rr_csi_flush_data

DESCRIPTION
  Discards all entries for a specified cell

DEPENDENCIES

RETURN VALUE
  VOID

===========================================================================*/
void rr_csi_flush_data(LAI_T   *lai_ptr,
  uint16  *ci_ptr,
  const rr_si_cache_flush_cause_T csi_flush_cause,
  const gas_id_t gas_id)
{
  csi_entry_t      *entry_ptr;
  int           i;
  rr_csi_cgi_t requested_cgi;
  boolean flush_data_logged = FALSE;

  if ( (lai_ptr == NULL) ||
       (ci_ptr == NULL) )
  {
    MSG_GERAN_ERROR_2("NULL ptr supplied can't retrieve SI's, lai ptr:0x%x, cell_id ptr:0x%x", lai_ptr, ci_ptr);
  }
  else
  {

    MSG_GERAN_HIGH_4_G("RR flush for plmn=%6x lac=%d, ci=%d cause=%d", \
                      ( (lai_ptr->plmn_id.identity[0] << 16) + \
                        (lai_ptr->plmn_id.identity[1] << 8)  + \
                         lai_ptr->plmn_id.identity[2]),
                       lai_ptr->location_area_code,
                       *ci_ptr,
                       csi_flush_cause 
                      );

    requested_cgi.lai = *lai_ptr;
    requested_cgi.ci = *ci_ptr;

    entry_ptr = sys_info_cache;
    for (i = 0; i < MAX_NUMBER_CACHED_SI; i++, entry_ptr++)
    {
      if (IS_VALID_OTA_ENTRY(entry_ptr))
      {
        if (rr_csi_is_cgi_equal(entry_ptr->cgi,requested_cgi))
        {
          if(!flush_data_logged)
          {
            (void)rr_log_csi_flush_data(entry_ptr,csi_flush_cause,gas_id);
            flush_data_logged = TRUE;
          }
          (void)rr_csi_discard_entry_data(entry_ptr);
        }
      }
    }
  }

  return;
}


/*===========================================================================

FUNCTION  rr_csi_flush_arfcn_data

DESCRIPTION
  Discards all entries for a specified ARFCN

DEPENDENCIES

RETURN VALUE
  VOID

===========================================================================*/
void rr_csi_flush_arfcn_data(const ARFCN_T arfcn,
                             const rr_si_cache_flush_cause_T csi_flush_cause,
                             const gas_id_t gas_id)
{
  csi_entry_t      *entry_ptr;
  int          i;

  MSG_GERAN_HIGH_2_G("RR flush for arfcn %d cause %d", arfcn.num, csi_flush_cause);

  entry_ptr = sys_info_cache;
  for (i = 0; i < MAX_NUMBER_CACHED_SI; i++, entry_ptr++)
  {
    if (IS_VALID_OTA_ENTRY(entry_ptr))
    {
      if (ARFCNS_EQUAL(arfcn,entry_ptr->si_arfcn))
      {
        (void)rr_log_csi_flush_data(entry_ptr,csi_flush_cause,gas_id);
        (void)rr_csi_discard_entry_data(entry_ptr);
      }
    }
  }

  return;
}

/*===========================================================================

FUNCTION  rr_csi_init_cache

DESCRIPTION
  Sets initial conditions for this module, called once at power-on

DEPENDENCIES
  None

RETURN VALUE
  VOID

===========================================================================*/
void rr_csi_init_cache(void)
{
  csi_entry_t *entry_ptr;
  int          i, j;

  entry_ptr = sys_info_cache;
  for (i = 0; i < MAX_NUMBER_CACHED_SI; i++, entry_ptr++)
  {
    entry_ptr->msg_type  = 0xFF;
    entry_ptr->octet_ptr = NULL;
  }

  for (i = 0; i < NUM_GERAN_DATA_SPACES; i++)
  {
    pend_store[i].active = FALSE;

    for (j = 0; j < RR_CSI_PEND_STORE_SIZE; j++)
    {
      pend_store[i].store[j].msg_type = 0xFF;
      pend_store[i].store[j].octet_ptr = NULL;
    }
  }

  return;
}

/*===========================================================================

FUNCTION  rr_csi_is_retrieval_for_fast_acquisition

DESCRIPTION
  Determines if the SIs need to be retrieved for fast Acquisition.

DEPENDENCIES
  None

RETURN VALUE
  rr_si_cache_SIs_to_retrieve_T

===========================================================================*/
rr_si_cache_SIs_to_retrieve_T rr_csi_is_retrieval_for_fast_acquisition(const gas_id_t gas_id)
{
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  if (rr_is_csfb_call_in_progress(gas_id) &&
      rr_get_nv_fast_si_acq_during_csfb_enabled(as_id) &&
      rr_nv_sys_info_cache_is_enabled(as_id))
  {
    return CSI_RETRIEVE_FOR_FAST_ACQUISITION;
  }
  else
  {
    return CSI_RETRIEVE_ALL_SIS;
  }
}

/*===========================================================================

FUNCTION  rr_csi_retrieve_all_SIs

DESCRIPTION
  Dispatches all SI's in specified order for a given CGI and ARFCN

DEPENDENCIES

RETURN VALUE
  TRUE if any system information message was served from the cache

===========================================================================*/
uint16 rr_csi_retrieve_all_SIs(ARFCN_T arfcn, LAI_T *lai_ptr, uint16 *cell_id_ptr, rr_si_cache_SIs_to_retrieve_T SIs_to_retrieve, const gas_id_t gas_id)
{
  csi_entry_t *entry_ptr;
  csi_entry_t  csi_entry;
  uint16 i = 0;
  uint16 offset = 0;
  dl_unit_data_ind_T new_msg;
  uint16 si_mask = MASK_SI_NONE;
  uint8 *si_retrieval_order_ptr;
  uint16 num_si_types_to_retrieve;
  boolean is_multiple_instance_si_retrieved = FALSE;
  boolean retrieve_cache_logged = FALSE;

  if ( (lai_ptr == NULL) || 
       (cell_id_ptr == NULL) )
  {
    MSG_GERAN_ERROR_2_G("NULL ptr supplied can't retrieve SI's, lai ptr:0x%x, cell_id ptr:0x%x", lai_ptr, cell_id_ptr);
    return FALSE;
  }

  MSG_GERAN_HIGH_3_G("RR retrieve all SIs for plmn=%6x lac=%d, ci=%d", \
                      ( (lai_ptr->plmn_id.identity[0] << 16) + \
                        (lai_ptr->plmn_id.identity[1] << 8)  + \
                         lai_ptr->plmn_id.identity[2]),
                         lai_ptr->location_area_code,
                       *cell_id_ptr
                      );


  csi_entry.si_arfcn =  arfcn;
  csi_entry.cgi.lai  = *lai_ptr;
  csi_entry.cgi.ci   = *cell_id_ptr;

  if (SIs_to_retrieve == CSI_RETRIEVE_FOR_FAST_ACQUISITION)
  {
    si_retrieval_order_ptr = si_retrieval_order_fast_acq;
    num_si_types_to_retrieve = NUM_SI_TYPES_FAST_ACQ;
  }
  else
  {
    si_retrieval_order_ptr = si_retrieval_order_normal;
    num_si_types_to_retrieve = NUM_SI_TYPES_CACHED;
  }

  for (i=0; i<num_si_types_to_retrieve; i++)
  {
    if (IS_MULTI_INSTANCE_SI(si_retrieval_order_ptr[i]))
    {
      offset = 0;
      csi_entry.msg_type = si_retrieval_order_ptr[i];
      entry_ptr = rr_csi_find_offset_entry(&csi_entry,&offset);
      
      /*This check will have already been done during SI3 acquisition, repeated 
        here for robustness*/
      if (entry_ptr != NULL &&
          ARFCNS_EQUAL(arfcn,entry_ptr->si_arfcn) == FALSE)
      {
        MSG_GERAN_ERROR_2_G("Requested ARFCN diff from cached on retrieve, cached:%d, requested:%d",entry_ptr->si_arfcn.num,
                                                                                                    arfcn.num);
        rr_csi_flush_data(lai_ptr, cell_id_ptr, CSI_FLUSH_SI_CHANGE, gas_id);
        return FALSE;
      }

      while (entry_ptr != NULL)
      { 
        /* if a match was found, create a new inter-task signal with the data */
        rr_csi_create_sys_info_message(entry_ptr, &new_msg, gas_id);

        if(!retrieve_cache_logged) /* Event Logging done only once per retrieval .*/
        {
          rr_log_csi_retrieve_entry(entry_ptr,gas_id);
          retrieve_cache_logged = TRUE;
        }
        is_multiple_instance_si_retrieved = TRUE;
        gs_send_message(GS_QUEUE_RR, (void *)&new_msg, FALSE);

        entry_ptr = rr_csi_find_offset_entry(&csi_entry,&offset);
      }        
    }
    else
    {
      csi_entry.msg_type = si_retrieval_order_ptr[i];
      entry_ptr = rr_csi_find_entry(&csi_entry);

      /*This check will have already been done during SI3 acquisition, repeated 
        here for robustness*/
      if (entry_ptr != NULL &&
          ARFCNS_EQUAL(arfcn,entry_ptr->si_arfcn) == FALSE)
      {
        MSG_GERAN_ERROR_2_G("Requested ARFCN diff from cached on retrieve, cached:%d, requested:%d",entry_ptr->si_arfcn.num,
                                                                                                    arfcn.num);
        rr_csi_flush_data(lai_ptr, cell_id_ptr, CSI_FLUSH_SI_CHANGE, gas_id);
        return FALSE;
      }

      /* if a match was found, create a new inter-task signal with the data */
      if (entry_ptr != NULL)
      {
        rr_csi_create_sys_info_message(entry_ptr, &new_msg, gas_id);
        if(!retrieve_cache_logged)
        {
          rr_log_csi_retrieve_entry(entry_ptr,gas_id);
          retrieve_cache_logged = TRUE;
        }
        gs_send_message(GS_QUEUE_RR, (void *)&new_msg, FALSE);
      }
    }

    if (entry_ptr != NULL)
    {
      si_mask |= rr_csi_map_msg_type_to_mask(csi_entry.msg_type);
    }
  }

  if (is_multiple_instance_si_retrieved)
  {
    rr_send_imsg(RR_IMSG_CSI_MULTIPLE_INSTANCES_SIS_RETRIEVED_IND, gas_id);
  }

  if ( si_mask != MASK_SI_NONE )
  {
    MSG_GERAN_MED_1_G("SIs retrieved = %x",si_mask);
  }

  return si_mask;
}


/*===========================================================================

FUNCTION  rr_csi_retrieve_SI3_4

DESCRIPTION
  Dispatches SI3 or 4 for a given CGI and ARFCN

DEPENDENCIES

RETURN VALUE
  TRUE if any system information message was served from the cache

===========================================================================*/
boolean rr_csi_retrieve_SI3_4(ARFCN_T arfcn, LAI_T *lai_ptr, uint16 *cell_id_ptr, const gas_id_t gas_id)
{
  csi_entry_t *entry_ptr;
  csi_entry_t  csi_entry;

  if ( (lai_ptr == NULL) || 
       (cell_id_ptr == NULL) )
  {
    MSG_GERAN_ERROR_2_G("NULL ptr supplied can't retrieve SI's, lai ptr:0x%x, cell_id ptr:0x%x", lai_ptr, cell_id_ptr);
    return FALSE;
  }

  if (!geran_in_multi_standby_mode())
  {
    /* SI cache only available during multi-standby mode */
    return(FALSE);
  }

  MSG_GERAN_HIGH_3_G("RR retrieve SI3/4 for plmn=%6x lac=%d, ci=%d", \
                      ( (lai_ptr->plmn_id.identity[0] << 16) + \
                        (lai_ptr->plmn_id.identity[1] << 8)  + \
                         lai_ptr->plmn_id.identity[2]),
                       lai_ptr->location_area_code,
                       *cell_id_ptr
                      );

  MSG_GERAN_HIGH_1_G("RR retrieve all SIs for arfcn=%d",arfcn.num);

  csi_entry.si_arfcn =  arfcn;
  csi_entry.cgi.lai  = *lai_ptr;
  csi_entry.cgi.ci   = *cell_id_ptr;
  csi_entry.msg_type = SYSTEM_INFORMATION_TYPE_3;

  entry_ptr          = rr_csi_find_entry(&csi_entry);

  if (entry_ptr == NULL)
  {
    /* SI3 not found, perform a search for SI4 with the supplied parameters */
    csi_entry.msg_type = SYSTEM_INFORMATION_TYPE_4;
    csi_entry.si_arfcn = arfcn;
    entry_ptr          = rr_csi_find_entry(&csi_entry);
  }

  /*This check will have already been done during SI3 acquisition, repeated 
    here for robustness*/
  if (entry_ptr != NULL &&
      ARFCNS_EQUAL(arfcn,entry_ptr->si_arfcn) == FALSE)
  {
    MSG_GERAN_ERROR_2_G("Requested ARFCN diff from cached on retrieve, cached:%d, requested:%d",entry_ptr->si_arfcn.num,
                                                                                                arfcn.num);
    rr_csi_flush_data(lai_ptr, cell_id_ptr, CSI_FLUSH_SI_CHANGE, gas_id);
    return FALSE;
  }

  /* if a match was found, create a new inter-task signal with the data */
  if (entry_ptr != NULL)
  {
    dl_unit_data_ind_T new_msg;

    MSG_GERAN_HIGH_3_G("rr_csi_retrieve_SI3_4 Send entry entry_ptr:0x%x ARFCN:%d msg_type:%d",entry_ptr,entry_ptr->si_arfcn.num,entry_ptr->msg_type);
    rr_csi_create_sys_info_message(entry_ptr, &new_msg, gas_id);
    rr_log_csi_retrieve_entry(entry_ptr,gas_id);
    gs_send_message(GS_QUEUE_RR, (void *)&new_msg, FALSE);
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION  rr_csi_clear_pend_store

DESCRIPTION
  Clears the pending store and frees any memory

DEPENDENCIES 
  None 

RETURN VALUE
  None

===========================================================================*/
static void rr_csi_clear_pend_store(rr_csi_pend_store_t *pend_store_ptr)
{
  uint8 i = 0;

  for (i=0; i < NELEMS(pend_store_ptr->store); i++)
  {
    csi_entry_t *entry = &(pend_store_ptr->store[i]);
    if (IS_VALID_OTA_ENTRY(entry))
    {
      (void)rr_csi_discard_entry_data(&(pend_store_ptr->store[i]));
    }
  }
}

/*===========================================================================

FUNCTION  rr_csi_activate_pend_store

DESCRIPTION
  Activates the pend store so SI's received without CGI are added
  to the store.

DEPENDENCIES

RETURN VALUE
  None

===========================================================================*/
void rr_csi_activate_pend_store(const gas_id_t gas_id)
{
  rr_csi_pend_store_t *pend_store_ptr = rr_sys_info_cache_get_pend_store_ptr(gas_id);

  RR_NULL_CHECK_FATAL(pend_store_ptr);

  // Check if sys info cache functionality enabled
  if (rr_nv_sys_info_cache_is_enabled(RR_GAS_ID_TO_AS_ID) == FALSE)
  {
    return;
  }

  if (pend_store_ptr->active == FALSE)
  {
    rr_csi_clear_pend_store(pend_store_ptr); 
    pend_store_ptr->active = TRUE;
    MSG_GERAN_HIGH_0_G("RR sys info cache activate pend store");
  }
}

/*===========================================================================

FUNCTION  rr_csi_deactivate_pend_store

DESCRIPTION
  Deactivates and clear the pend store.

DEPENDENCIES

RETURN VALUE
  None

===========================================================================*/
void rr_csi_deactivate_pend_store(const gas_id_t gas_id)
{
  rr_csi_pend_store_t *pend_store_ptr = rr_sys_info_cache_get_pend_store_ptr(gas_id);

  RR_NULL_CHECK_FATAL(pend_store_ptr);

  if (pend_store_ptr->active == TRUE)
  {
    rr_csi_clear_pend_store(pend_store_ptr); 
    pend_store_ptr->active = FALSE;
    MSG_GERAN_HIGH_0_G("RR sys info cache deactivate pend store");
  }
}

/*===========================================================================

FUNCTION  rr_csi_add_entry_to_pend_store

DESCRIPTION
  Adds an SI with unknown CGI to the pend store.

DEPENDENCIES

RETURN VALUE
  None

===========================================================================*/
static void rr_csi_add_entry_to_pend_store(uint8               msg_type,
                                           ARFCN_T             arfcn,
                                           uint8              *msg_ptr,
                                           const gas_id_t      gas_id)
{
  uint8 i = 0;
  rr_csi_pend_store_t *pend_store_ptr = rr_sys_info_cache_get_pend_store_ptr(gas_id);

  RR_NULL_CHECK_FATAL(pend_store_ptr);

  /*Find next free entry and populate. Note duplicates are added to the pend store. 
    A duplicate SI is however very unlikely as the pend store is deactivated when SI3 is received. 
    Any duplicates have separate storage for each instance. These instances will be resolved when added into the main cache*/
  for (i = 0; i < NELEMS(pend_store_ptr->store); i++)
  {
    csi_entry_t *entry_ptr = &(pend_store_ptr->store[i]);

    /*find a free entry*/
    if (IS_VALID_OTA_ENTRY(entry_ptr) == FALSE)
    {
      pend_store_ptr->store[i].octet_ptr = (uint8 *)GPRS_MEM_MALLOC(N201_MAX);
      if (pend_store_ptr->store[i].octet_ptr != NULL)
      {        
        /* store the new entry in the cache */
        memscpy(pend_store_ptr->store[i].octet_ptr, N201_MAX, msg_ptr, N201_MAX);
        pend_store_ptr->store[i].si_arfcn = arfcn;
        pend_store_ptr->store[i].msg_type = msg_type;

        /*CGI and counter are set when populating the cache*/
        MSG_GERAN_HIGH_3_G("rr_csi_add_entry_to_pend_store add to free entry: 0x%x, ARFCN:%d, msg_type:%d",entry_ptr,entry_ptr->si_arfcn.num,entry_ptr->msg_type);
      }
      else
      {
        MSG_GERAN_ERROR_0_G("rr_csi_add_entry_to_pend_store MALLOC fail");
      }
      break;
    }
    else if (i == (NELEMS(pend_store_ptr->store) -1))
    {
      MSG_GERAN_ERROR_0_G("RR sys info cache pend store full");
      break;
    }
  }
}


/*===========================================================================

FUNCTION  rr_csi_copy_pend_store_to_cache

DESCRIPTION
  Copies the pend store to the SI cache and applies the given CGI to all entries.
  rr_csi_update_entry_internal resolves any duplicate/existing entries.

DEPENDENCIES

RETURN VALUE
  None
===========================================================================*/
void rr_csi_copy_pend_store_to_cache(ARFCN_T             arfcn,
                                     LAI_T              *lai_ptr,
                                     uint16             *ci_ptr,
                                     const gas_id_t      gas_id)
{
  uint8 i = 0;
  rr_csi_pend_store_t *pend_store_ptr = rr_sys_info_cache_get_pend_store_ptr(gas_id);

  RR_NULL_CHECK_FATAL(pend_store_ptr);

  if ( (lai_ptr == NULL) ||
       (ci_ptr == NULL) )
  {
    MSG_GERAN_ERROR_2_G("NULL ptr supplied can't copy pend store, lai ptr:0x%x, cell_id ptr:0x%x", lai_ptr, ci_ptr);
    return;
  }

  MSG_GERAN_HIGH_3_G("RR copy pend store to cache for plmn=%6x lac=%d, ci=%d", \
                      ( (lai_ptr->plmn_id.identity[0] << 16) + \
                        (lai_ptr->plmn_id.identity[1] << 8)  + \
                         lai_ptr->plmn_id.identity[2]),
                       lai_ptr->location_area_code,
                       *ci_ptr
                      );


  for (i = 0; i < NELEMS(pend_store_ptr->store); i++)
  {
    csi_entry_t *entry = &(pend_store_ptr->store[i]);

    if (IS_VALID_OTA_ENTRY(entry) &&
        ARFCNS_EQUAL(arfcn,pend_store_ptr->store[i].si_arfcn))
    {
       rr_csi_update_entry_internal( pend_store_ptr->store[i].msg_type,
                                     pend_store_ptr->store[i].si_arfcn,
                                     pend_store_ptr->store[i].octet_ptr,
                                     0, /*TC default to ensure the entry is updated in the cache*/
                                     lai_ptr,
                                     ci_ptr,
                                     gas_id);
    }
  }
}


/*===========================================================================

FUNCTION  rr_csi_get_si_index

DESCRIPTION
  Finds SI_Index for SI_2QUATER/SI_2TER for the entry passed as input.

DEPENDENCIES

RETURN VALUE
  VOID

===========================================================================*/
void rr_csi_get_si_index(csi_entry_t *log_entry_ptr,
  uint8 *si_index)
{
  RR_NULL_CHECK_RETURN_VOID(log_entry_ptr);
  RR_NULL_CHECK_RETURN_VOID(si_index);

  *si_index = 0;

  if (IS_MULTI_INSTANCE_SI(log_entry_ptr->msg_type))
  {
    if (log_entry_ptr->msg_type == SYSTEM_INFORMATION_TYPE_2_QUATER)
    {
      *si_index = SI2QUATER_INDEX(log_entry_ptr->octet_ptr);
    }
    else if (log_entry_ptr->msg_type == SYSTEM_INFORMATION_TYPE_2_TER)
    {
      uint16 pos = 0; /*first bit of rest octets is L/H bit*/
      if (gunpackb_lh(&log_entry_ptr->octet_ptr[19], &pos) == 1)
      {
        *si_index = SI2TER_INDEX(log_entry_ptr->octet_ptr);
      }
    }    
  }
  return;
}

/*===========================================================================

FUNCTION  rr_csi_get_si_cache_summary

DESCRIPTION
  Fetches cached SI list, SI2QUATER/2TER indices from the Cached SI database.

DEPENDENCIES

RETURN VALUE
  VOID

===========================================================================*/
void rr_csi_get_si_cache_summary(csi_entry_t *entry_ptr,
  uint8     *csi_length,
  uint8     *cached_si_list,
  uint16    *SI_2quater_indices,
  uint16    *SI_2ter_indices)
{
  csi_entry_t *db_entry_ptr;
  int          i;
  uint8        si_cached_index = 0;

  if ((entry_ptr == NULL) || (csi_length == NULL) || (cached_si_list == NULL) ||
       (SI_2quater_indices == NULL) || (SI_2ter_indices == NULL))
  {
    MSG_GERAN_ERROR_5("NULL ptr supplied can't log update/retrieve SI's, entry_ptr:0x%x, csi_length ptr:0x%x,cached_si_list ptr:0x%x, SI_2quater_indices ptr:0x%x, lai ptr:0x%x, SI_2ter_indices ptr:0x%x", 
                                   entry_ptr, csi_length,cached_si_list,SI_2quater_indices,SI_2ter_indices);
    return;
  }


  db_entry_ptr = sys_info_cache;
  *csi_length = 0;
  *SI_2quater_indices = 0;
  *SI_2ter_indices = 0;

  for (i = 0; i < MAX_NUMBER_CACHED_SI; i++, db_entry_ptr++)
  {
    /* match CGI & ARFCN */
    if(IS_VALID_OTA_ENTRY(db_entry_ptr) &&
       rr_csi_is_cgi_equal(db_entry_ptr->cgi,entry_ptr->cgi) &&
       (ARFCNS_EQUAL(db_entry_ptr->si_arfcn,entry_ptr->si_arfcn) == TRUE))
    {
      uint8 si_index = 0;
      if (IS_MULTI_INSTANCE_SI(db_entry_ptr->msg_type))
      {
        if (db_entry_ptr->msg_type == SYSTEM_INFORMATION_TYPE_2_QUATER)
        {
          si_index = SI2QUATER_INDEX(db_entry_ptr->octet_ptr);
          if(!(*SI_2quater_indices)) /* Update these only if they have not been updated before. i.e. indices bitmap is zero */
          {
            cached_si_list[si_cached_index] = db_entry_ptr->msg_type;
            (*csi_length)++;
            si_cached_index++;
          }
          if(si_index < MAX_INSTANCES)
          {
            *SI_2quater_indices |= (1<<si_index);
          }
          else
          {
            MSG_GERAN_ERROR_0("rr_csi_get_si_cache_summary: SI2Quater Index more than maximum value");
          }
        }
        else if (db_entry_ptr->msg_type == SYSTEM_INFORMATION_TYPE_2_TER)
        {
          uint16 pos = 0; /*first bit of rest octets is L/H bit*/
          if (gunpackb_lh(&db_entry_ptr->octet_ptr[19], &pos) == 1)
          {
            si_index = SI2TER_INDEX(db_entry_ptr->octet_ptr);
            if(!(*SI_2ter_indices))  /* Update these only if they have not been updated before. i.e. indices bitmap is zero */
            {
              cached_si_list[si_cached_index] = db_entry_ptr->msg_type;
              (*csi_length)++;
              si_cached_index++;
            }
            if(si_index < MAX_INSTANCES)
            {
              *SI_2ter_indices |= (1<<si_index);
            }
            else
            {
              MSG_GERAN_ERROR_0("rr_csi_get_si_cache_summary: SI2ter Index more than maximum value");
            }
          }
          else /* L bit in rest octets => it is the only instance. so consider it as index 0. */
          {
            cached_si_list[si_cached_index] = db_entry_ptr->msg_type;
            (*csi_length)++;
            si_cached_index++;
            *SI_2ter_indices |= 1;
          }
        }
      }
      else
      {
        cached_si_list[si_cached_index] = db_entry_ptr->msg_type;
        (*csi_length)++;
        si_cached_index++;
      }
    }
    if(si_cached_index >= MAX_CSI_PER_CGI)
    {
      MSG_GERAN_ERROR_0("rr_csi_get_si_cache_summary: cached_si_list(logging) full");
      return;
    }
  }
}

/*===========================================================================

FUNCTION  rr_csi_is_cgi_equal

DESCRIPTION
  Checks if the CGIs passes to the function are the same.

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE based on CGI match.

===========================================================================*/
static boolean rr_csi_is_cgi_equal(rr_csi_cgi_t db_entry_cgi, rr_csi_cgi_t target_cgi)
{
  if((db_entry_cgi.ci == target_cgi.ci) &&
     (db_entry_cgi.lai.location_area_code == target_cgi.lai.location_area_code) &&
     (db_entry_cgi.lai.plmn_id.identity[0] == target_cgi.lai.plmn_id.identity[0]) &&
     (db_entry_cgi.lai.plmn_id.identity[1] == target_cgi.lai.plmn_id.identity[1]) &&
     (db_entry_cgi.lai.plmn_id.identity[2] == target_cgi.lai.plmn_id.identity[2]))
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
}

/* EOF */
