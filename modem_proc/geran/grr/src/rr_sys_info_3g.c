/*==============================================================================

                           R R   S Y S   I N F O   3 G

GENERAL DESCRIPTION
  This module provides functions and variables to allow storage and manipulation
  of 3G system information (neighbour cell lists).

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2013 Qualcomm Technologies, Inc.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_3g.c#1 $

==============================================================================*/


/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

#include <stdlib.h> /* for qsort() */
#include "rr_seg_load.h"
#include "rr_sys_info_3g.h"
#include "rr_general.h"
#include "string.h"
#include "rr_defs.h"
#include "rr_gprs_defs.h"
#include "rr_multi_sim.h"
#include "rr_process_pmo_pcco.h"


/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
static rr_sys_info_3g_data_t  rr_sys_info_3g_data[NUM_GERAN_DATA_SPACES];

/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*!
 * \brief Return a pointer to the module data.
 * 
 * \param gas_id 
 * 
 * \return rr_sys_info_3g_data_t* 
 */
static rr_sys_info_3g_data_t *rr_sys_info_3g_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_sys_info_3g_data[as_index];
}

void rr_init_sys_info_3g(
  const gas_id_t gas_id
)
{
  // Obtain a poiner to the module data
  rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr = rr_sys_info_3g_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_sys_info_3g_data_ptr);

  rr_sys_info_3g_data_ptr->grr_3g_nlist = &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast;
  memset(&rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast, 0, sizeof(grr_3g_nlist_t));
  memset(&rr_sys_info_3g_data_ptr->grr_3g_nlist_modified, 0, sizeof(grr_3g_nlist_t));
  memset(&rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2quater, 0, sizeof(grr_3g_nlist_unindexed_t));
  memset(&rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2ter, 0, sizeof(grr_3g_nlist_unindexed_t));
  memset(&rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_psi3quater, 0, sizeof(grr_3g_nlist_unindexed_t));
#ifdef FEATURE_GSM_TDS
  memset(&rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2quater, 0, sizeof(grr_3g_nlist_unindexed_t));
  memset(&rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2ter, 0, sizeof(grr_3g_nlist_unindexed_t));
#endif /*FEATURE_GSM_TDS*/
}

boolean rr_gprs_3g_nlist_valid(grr_3g_nlist_t *list)
{
  int i;
  for (i = 0; i < MAX_GRR_3G_NLIST_SIZE; i++)
  {
    if (list->entries[i].valid)
    {
      return TRUE;
    }
  }
  return FALSE;
}

static boolean rr_gprs_3g_nlist_delete(
  grr_3g_nlist_t * list,
  int index
)
{
  if (index < MAX_GRR_3G_NLIST_SIZE)
  {
    grr_3g_nlist_entry_t *entry = &list->entries[index];

    if (entry->valid)
    {
      if (entry->deleted)
      {
        MSG_GERAN_MED_1("Entry at index %d is already deleted!", index);
        return FALSE;
      }
      else
      {
        MSG_GERAN_MED_1("Deleting entry at index %d", index);
        entry->deleted = TRUE;
        return TRUE;
      }
    }
    else
    {
      MSG_GERAN_MED_1("Deleting entry at index %d", index);
      entry->deleted = TRUE;
      return TRUE;
    }   
  }
  else
  {
    ERR("Index out of range: %d >= %d", index, MAX_GRR_3G_NLIST_SIZE, 0);
    return FALSE;
  }
}

boolean rr_gprs_3g_nlist_add(
  grr_3g_nlist_t * list, 
  grr_3g_nlist_entry_t * entry, 
  int index,
  const gas_id_t gas_id
)
{
  if (index < MAX_GRR_3G_NLIST_SIZE)
  {
    grr_3g_nlist_entry_t *dst = &list->entries[index];

    if (dst->valid)
    {
      if (entry->instance_received_in > dst->instance_received_in)
      {
        if (rr_si2quater_all_instances_received(rr_get_instances_ptr(SI2Q_INSTANCE,gas_id)))
        {
          if (entry->frequency_only)
          {
#ifdef FEATURE_GSM_TDS
            if (entry->type == UTRAN_FDD)
#endif /*FEATURE_GSM_TDS*/
            {
              MSG_GERAN_MED_3_G("[%d]: FDD UARFCN %d, index for RSSI reporting",
              index,
              entry->uarfcn,
              0);
            }
#ifdef FEATURE_GSM_TDS
            else if (entry->type == UTRAN_TDD)
            {
              MSG_GERAN_MED_3_G("[%d]: TDD UARFCN %d, index not valid for reprting",
              index,
              entry->uarfcn,
              0);
            }
#endif /*FEATURE_GSM_TDS*/
          }
          else
          {
#ifdef FEATURE_GSM_TDS
            if (entry->type == UTRAN_FDD )
#endif /*FEATURE_GSM_TDS*/
            {
              MSG_GERAN_MED_3_G("[%d]: FDD UARFCN %d, Scrambling Code: %d",
              index,
              entry->uarfcn,
              entry->scrambling_code);
            }
#ifdef FEATURE_GSM_TDS
            else if (entry->type == UTRAN_TDD)
            {
              MSG_GERAN_MED_3_G("[%d]: TDD UARFCN %d, CPID: %d",
              index,
              entry->uarfcn,
              entry->cell_parameter_id);
            }
#endif /*FEATURE_GSM_TDS*/
          }
      	}
        *dst = *entry;
        return TRUE;
      }
      else
      {
        MSG_GERAN_MED_0_G("Entry already present is from higher instance, not adding");
        return FALSE;
      }
    }
    else
    {
      if (rr_si2quater_all_instances_received(rr_get_instances_ptr(SI2Q_INSTANCE,gas_id)))
      {
        if (entry->frequency_only)
        {
#ifdef FEATURE_GSM_TDS
          if (entry->type == UTRAN_FDD)
#endif /*FEATURE_GSM_TDS*/
          {
            MSG_GERAN_MED_3_G("[%d]: FDD UARFCN %d, index for RSSI reporting",
            index,
            entry->uarfcn,
            0);
           }
#ifdef FEATURE_GSM_TDS
           else if (entry->type == UTRAN_TDD)
           {
            MSG_GERAN_MED_3_G("[%d]: TDD UARFCN %d, index not valid for reprting",
              index,
              entry->uarfcn,
              0);
           }
#endif /*FEATURE_GSM_TDS*/
        }
        else
        {
#ifdef FEATURE_GSM_TDS
          if (entry->type == UTRAN_FDD )
#endif /*FEATURE_GSM_TDS*/
          {
            MSG_GERAN_MED_3_G("[%d]: FDD UARFCN %d, Scrambling Code: %d",
              index,
              entry->uarfcn,
              entry->scrambling_code);
          }
#ifdef FEATURE_GSM_TDS
          else if (entry->type == UTRAN_TDD)
          {
            MSG_GERAN_MED_3_G("[%d]: TDD UARFCN %d, CPID: %d",
              index,
              entry->uarfcn,
              entry->cell_parameter_id);
          }
#endif /*FEATURE_GSM_TDS*/
        }
      }
      *dst = *entry;
      return TRUE;
    }
  }
  else
  {
    ERR("Index out of range: %d >= %d", index, MAX_GRR_3G_NLIST_SIZE, 0);
    return FALSE;
  }
}

boolean rr_gprs_3g_nlist_unindexed_add(
  grr_3g_nlist_unindexed_t * list, 
  uint16 uarfcn
)
{
  if (list->size != MAX_GRR_3G_FREQS)
  {
    MSG_GERAN_MED_1("Adding UTRAN frequency %d to 3G Cell Reselection list", uarfcn);
    list->uarfcns[list->size] = uarfcn;
    list->size++;
  }
  else
  {
    MSG_GERAN_MED_1("Overflow for unindexed UTRAN freqs, not adding %d", uarfcn);
  }
  return FALSE;
}


/*=====================================================================

  Stuff for decoding contents of PSI3quater, PMO, PCCO

=====================================================================*/

/**
 * This function decodes either the FDD or TDD cell information in
 * either RANGE_1024 or RANGE_512 format.
 *
 * @param cell_information    : the encoded cell information in Range 1024 or
 *                              Range 512 format
 * @param num_cells           : should be either 1024 or 512
 * @param decoded_information : the destination for the decoded information
 * @param range_format        : either 1024 or 512 (Range encoding)
 */
static boolean rr_gprs_3g_nlist_range_decode(
  uint16 *W,
  int size,
  uint16 *dst,
  int range_format
)
{
  int range;
  word index;
  int n;
  word k;
  int temp;
  boolean retval = TRUE;

  for (k = 1; k < (size + 1); k++)
  {
    index = k;
    range = (range_format - 1) / greatest_power(index);
    n = W[index];

    while (index > 1)
    {
      range = 2 * range + 1;
      if ((2 * index) < (3 * greatest_power(index)))
      {
        index = (uint16) (index - (greatest_power(index) / 2));
        temp = n + (int) (W[index]) - 1 - ((range - 1) / 2);
        while (temp < 0)
        {
          temp += range;
        }
        if (temp == range)
        {
          n = temp;
        }
        else if (temp == 0)
        {
          n = range;
        }
        else
        {
          n = temp % range;
        }
      }
      else
      {
        index = index - greatest_power(index);
        temp = n + W[index];
        while (temp < 0)
        {
          temp += range;
        }
        if (temp == range)
        {
          n = temp;
        }
        else if (temp == 0)
        {
          n = range;
        }
        else
        {
          n = temp % range;
        }
      }
    }
    if (n > 65535)
    {
      ERR("Error in decode: %d", n, 0, 0);
      n = 0;
      retval = FALSE;
    }
    MSG_GERAN_MED_1("decoded %d from src", n);
    dst[k - 1] = (word) n;
  }
  return retval;
}

/**
 * -----
 * Rules
 * -----
 *
 * If FDD_Indic0 == 1, this indicates the scrambling code/diversity parameter
 * 0000000000 is a member of the set.
 *
 * If nr_of_fdd_cells == 0 and fdd_indic0 == 0, this indicates the 3G Neighbour 
 * Cell list index for report on RSSI, see 3GPP TS 05.08
 *
 * If nr_of_fdd_cells >= 17, this shall not be considered an error, indices of
 * the 3G neighbour cell list shall be incremented accordingly.
 *
 * ----------------
 * Assumptions Made
 * ----------------
 *
 * If nr_of_fdd_cells != 0 and fdd_indic0 == 0, the scrambling code/diversity
 * parameter 0000000000 is not a member of the set.
 *
 * If nr_of_fdd_cells == 0 and fdd_indic0 == 1, there is a single cell on this 
 * frequency to add to the GPRS 3G Neighbour Cell List.
 *
 * If nr_of_fdd_cells == 0, this indicates the frequency is to be added to the
 * GPRS 3G Cell Reselection List.
 *
 * This means that if nr_of_fdd_cells == 0 and fdd_indic0 == 1, an entry will
 * be added to both the 3G Neighbour Cell List and the 3G Cell Reselection List.
 */

static int compare_uint16(const void *arg1, const void *arg2)
{
  uint16 l, r;

  if (arg1 == NULL)
  {
    MSG_GERAN_ERROR_0("arg1 == NULL");
    return 0;
  }

  if (arg2 == NULL)
  {
    MSG_GERAN_ERROR_0("arg2 == NULL");
    return 0;
  }

  l = *(const uint16*)arg1;
  r = *(const uint16*)arg2;


  if (l < r)
  {
    return -1;
  }
  else if (l > r)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
 
void rr_gprs_3g_nlist_decode_fdd_cells(
  grr_3g_nlist_t * list,
  grr_3g_nlist_unindexed_t * unindexed_list,
  uint8 instance,
  utran_fdd_description_t * fdd_cells,
  int * index_start_3g,
  const gas_id_t gas_id
)
{
  int i;
  
  MSG_GERAN_MED_0("Decoding UTRAN FDD neighbour cells");
  
  for (i = 0; i < fdd_cells->utran_fdd_neighbour_cell_count; i++)
  {
    utran_fdd_neighbour_cell_t *cells = 
      fdd_cells->utran_fdd_neighbour_cells[i];

    int nr_of_fdd_cells = cells->nr_of_fdd_cells;

    if (nr_of_fdd_cells > 31)
    {
      MSG_GERAN_ERROR_1("nr_of_fdd_cells out of range: %d", nr_of_fdd_cells);
      return;
    }

    /*
    If n=0 and FDD_Indic0 = 0, this indicates the 3G Neighbour Cell list index
    for report on RSSI, see 3GPP TS 05.08
    */
    if (nr_of_fdd_cells == 0 && cells->fdd_indic0 == 0)
    {
      grr_3g_nlist_entry_t entry = {0};

      MSG_GERAN_MED_1("FDD_Indic0 == 0 && NR_OF_FDD_CELLS == 0, add freq %d for RSSI reporting", cells->fdd_arfcn);

      entry.valid = TRUE;
      entry.type = UTRAN_FDD;
      entry.uarfcn = cells->fdd_arfcn;
      entry.frequency_only = TRUE;
      entry.instance_received_in = instance;
 
      (void) rr_gprs_3g_nlist_add(list, &entry, *index_start_3g, gas_id);
      *index_start_3g += 1;
    }

    /*
    If NR_OF_FDD_CELLS is greater than or equal to 17 and less than or equal to
    31 then this should not be considered an error.  The 3G neighbour list
    index should be incremented.
    */
    else if (nr_of_fdd_cells >= 17)
    {
      /* unindexed_list != NULL indicatest that we are decoding PSI3quater, this
      has special requirements for NR_OF_FDD_CELLS = 31:

      "If parameter n in table 11.2.9b.2.a is equal to 31, this indicates that
      the corresponding UARFCN shall be included in the GPRS 3G Cell
      Reselection list (see sub-clause 5.6.3.7); no index shall be allocated in
      the 3G Neighbour Cell list."
      */
      if (unindexed_list != NULL && nr_of_fdd_cells == 31)
      {
        MSG_GERAN_MED_1("NR_OF_FDD_CELLS == 0, add freq %d to unindexed frequency list", cells->fdd_arfcn);
        (void) rr_gprs_3g_nlist_unindexed_add(unindexed_list, cells->fdd_arfcn);
      }
      else
      {
        *index_start_3g += 1;
      }
    }

    else
    {
      int p;
      uint16 scrambling_codes[17];
      boolean decoding_ok;

      decoding_ok = rr_gprs_3g_nlist_range_decode(
        cells->fdd_cell_information,
        cells->nr_of_fdd_cells,
        scrambling_codes,
        1024);
    
      if (!decoding_ok)
      {
        ERR("Decoding failed!", 0, 0, 0);
      }
      else
      {
        /* now need to sort the scrambling codes */
        qsort(scrambling_codes, cells->nr_of_fdd_cells, sizeof(uint16), compare_uint16);

        /* add it to the list of indexed cells to search on */
        if (cells->fdd_indic0)
        {
          grr_3g_nlist_entry_t entry = {0};

          entry.valid = TRUE;
          entry.type = UTRAN_FDD;
          entry.uarfcn = cells->fdd_arfcn;
          entry.scrambling_code = 0;
          entry.diversity = 0;
          entry.instance_received_in = instance;

          (void) rr_gprs_3g_nlist_add(list, &entry, *index_start_3g, gas_id);
          *index_start_3g += 1;
        }
        
        for (p = 0; p < nr_of_fdd_cells; p++)
        {
          grr_3g_nlist_entry_t entry = {0};

          entry.valid = TRUE;
          entry.type = UTRAN_FDD;
          entry.uarfcn = cells->fdd_arfcn,
          entry.scrambling_code = (scrambling_codes[p] & 0x1ff) << 4;
          entry.diversity = (scrambling_codes[p] & 0x200) ? TRUE : FALSE;
          entry.instance_received_in = instance;

          (void) rr_gprs_3g_nlist_add(list, &entry, *index_start_3g, gas_id);
          *index_start_3g += 1;
        }
      }
    }
  }
}


#ifdef FEATURE_GSM_TDS
void rr_gprs_3g_nlist_decode_tdd_cells_for_tds(
  grr_3g_nlist_t * list,
  int instance,
  utran_tdd_description_t * tdd_cells,
  int * index_start_3g,
  const gas_id_t gas_id
  )
{
  int i;
  
  MSG_GERAN_MED_0("Decoding UTRAN TDD neighbour cells");
  
  for (i = 0; i < tdd_cells->utran_tdd_neighbour_cell_count; i++)
  {
    utran_tdd_neighbour_cell_t *cells = 
      tdd_cells->utran_tdd_neighbour_cells[i];

    int nr_of_tdd_cells = cells->nr_of_tdd_cells;

    if (nr_of_tdd_cells > 31)
    {
      ERR("nr_of_tdd_cells out of range: %d", nr_of_tdd_cells, 0, 0);
      return;
    }

    /*
    If m=0 and TDD_Indic0=0, or m is equal or greater than 21, this shall not be 
    considered as an error; the corresponding index in the 3G Neighbour Cell list 
    shall be incremented by one.
    */
    if ((nr_of_tdd_cells == 0 && cells->tdd_indic0 == 0) || nr_of_tdd_cells >= 21)
    {
      MSG_GERAN_MED_3("Incrementing indices for UTRAN TDD neighbour, TDD cell index:%d, nr_tdd_cells:%d, tdd_indic0:%d", 
              i, nr_of_tdd_cells, cells->tdd_indic0);
      *index_start_3g += 1;
    }
    else
    {
      int p;
      uint16 cpid[21];
      boolean decoding_ok;

      decoding_ok = rr_gprs_3g_nlist_range_decode(
        cells->tdd_cell_information,
        cells->nr_of_tdd_cells,
        cpid,
        512);
    
      if (!decoding_ok)
      {
        ERR("Decoding failed!", 0, 0, 0);
      }
      else
      {
        /* now need to sort the scrambling codes */
        qsort(cpid, cells->nr_of_tdd_cells, sizeof(uint16), compare_uint16);

        /* add it to the list of indexed cells to search on */
        if (cells->tdd_indic0)
        {
          grr_3g_nlist_entry_t entry = {0};

          entry.valid = TRUE;
          entry.type = UTRAN_TDD;
          entry.uarfcn = cells->tdd_arfcn;
          entry.cell_parameter_id = 0;
          entry.diversity = 0;
          entry.instance_received_in = instance;

          (void) rr_gprs_3g_nlist_add(list, &entry, *index_start_3g, gas_id);
          *index_start_3g += 1;
        }
        
        for (p = 0; p < nr_of_tdd_cells; p++)
        {
          grr_3g_nlist_entry_t entry = {0};

          entry.valid = TRUE;
          entry.type = UTRAN_TDD;
          entry.uarfcn = cells->tdd_arfcn,
          entry.cell_parameter_id = cpid[p] & 0x007f;
          entry.diversity = (cpid[p] & 0x100) ? TRUE : FALSE;
          entry.sync_case_TSTD = (cpid[p] & 0x0080) >> 7;
          entry.instance_received_in = instance;

          (void) rr_gprs_3g_nlist_add(list, &entry, *index_start_3g, gas_id);
          *index_start_3g += 1;
        }
      }
    }
  }
}
#endif /*FEATURE_GSM_TDS*/

void rr_gprs_3g_nlist_decode_tdd_cells(
/*  grr_3g_nlist_t * list, */
/*  int instance, */
  utran_tdd_description_t * tdd_cells,
  int * index_start_3g
)
{
  int i;
  
  MSG_GERAN_MED_0("Decoding UTRAN TDD neighbour cells");
  
  for (i = 0; i < tdd_cells->utran_tdd_neighbour_cell_count; i++)
  {
    utran_tdd_neighbour_cell_t *cells = 
      tdd_cells->utran_tdd_neighbour_cells[i];

    int nr_of_tdd_cells = cells->nr_of_tdd_cells;

    if (nr_of_tdd_cells > 31)
    {
      ERR("nr_of_tdd_cells out of range: %d", nr_of_tdd_cells, 0, 0);
      return;
    }

    /*
    If m=0 and TDD_Indic0=0, or m is equal or greater than 21, this shall not be 
    considered as an error; the corresponding index in the 3G Neighbour Cell list 
    shall be incremented by one.
    */
    if ((nr_of_tdd_cells == 0 && cells->tdd_indic0 == 0) || nr_of_tdd_cells > 31)
    {
      MSG_GERAN_MED_0("Incrementing indices for UTRAN TDD neighbour");
      *index_start_3g += 1;
    }
    else
    {
      if (nr_of_tdd_cells >= 21 && nr_of_tdd_cells <= 31)
      {
        nr_of_tdd_cells = 0;
      }

      if (nr_of_tdd_cells == 0)
      {
        if (cells->tdd_indic0)
        {
          MSG_GERAN_MED_0("Incrementing indices for UTRAN TDD neighbour");
          *index_start_3g += 1;
        }
      }
      else
      {
        /* add it to the list of indexed cells to search on */
        if (cells->tdd_indic0)
        {
          MSG_GERAN_MED_0("Incrementing indices for UTRAN TDD neighbour");
          *index_start_3g += 1;
        }
        MSG_GERAN_MED_0("Incrementing indices for UTRAN TDD neighbour");
        *index_start_3g += nr_of_tdd_cells;
      }
    }
  }
}

void rr_gprs_3g_nlist_decode_cdma2000_cells(
/*  grr_3g_nlist_t * list, */
/*  int instance, */
  cdma2000_description_t * cdma2000_cells,
  int * index_start_3g
)
{
  *index_start_3g += cdma2000_cells->number_cdma2000_cells;
}



void rr_gprs_3g_nlist_decode_removed_3g_cell_description(
  grr_3g_nlist_t * list,
  removed_3g_cell_description_t * removed_3g_cell_description
)
{
  int i, j;
  removed_cell_sublist_t *removed_cell_sublist;
  removed_cell_t *removed_cell;

  for (i = 0; i < removed_3g_cell_description->number_of_sublists; i++)
  {
    removed_cell_sublist = &removed_3g_cell_description->removed_cell_sublists[i];

    for (j = 0; j < removed_cell_sublist->number_of_removed_cells; j++)
    {
      removed_cell = &removed_cell_sublist->removed_cells[j];

      (void) rr_gprs_3g_nlist_delete(
        list,
        removed_cell->removed_three_g_cell_index
      );
      
      if (removed_cell->three_g_cell_diff_length)
      {
        (void) rr_gprs_3g_nlist_delete(
          list,
          removed_cell->removed_three_g_cell_index + removed_cell->three_g_cell_diff
        );
      }
    }
  }
}


/*=====================================================================

  End of stuff for decoding contents of PSI3quater, PMO, PCCO

=====================================================================*/







/*=====================================================================

  Functions for applying changes in 3G system information.

=====================================================================*/

/**
 * Applies the modifications in mods to the list in src to produce the
 * list in dst.  If there are no modifications to be applied, simply
 * copies the src list to dst.
 *
 * @param dst pointer to the destination structure
 * @param src pointer to the source structure
 * @param mods pointer to the list of modifications to apply
 * @returns boolean TRUE if any modifications are applied to src, FALSE 
 * otherwise.
 *
 * TODO : This does not currently take in to account that modifications
 * should only be applied if the BA_IND and 3G_BA_IND or PSI3quater
 * Change Mark match appropiately.
 */
boolean rr_gprs_3g_nlist_apply_mods(
  grr_3g_nlist_t * dst,
  grr_3g_nlist_t * src,
  grr_3g_nlist_t * mods
)
{
  int i;
  boolean changed = FALSE;

  *dst = *src;

  for (i = 0; i < MAX_GRR_3G_NLIST_SIZE; i++)
  {
    if (src->entries[i].valid)
    {
      if (mods->entries[i].valid)
      {
        if (mods->entries[i].deleted)
        {
          MSG_GERAN_MED_1("Deleting index %d from original 3G neighbour list", i);
          dst->entries[i].deleted = TRUE;
          changed = TRUE;
        }
        else
        {
#ifdef FEATURE_GSM_TDS
          if (mods->entries[i].type == UTRAN_FDD)
#endif /*FEATURE_GSM_TDS*/
          {
            MSG_GERAN_MED_3("Replacing index %d with UTRAN FDD ARFCN %d, sc %d", 
              i, 
              mods->entries[i].uarfcn,
              mods->entries[i].scrambling_code
            );
          }
#ifdef FEATURE_GSM_TDS
          else if (mods->entries[i].type == UTRAN_TDD)
          {
            MSG_GERAN_MED_3("Replacing index %d with UTRAN TDD ARFCN %d, cpid %d", 
               i, 
               mods->entries[i].uarfcn,
               mods->entries[i].cell_parameter_id
             );
          }
#endif /*FEATURE_GSM_TDS*/
          dst->entries[i] = mods->entries[i];
          changed = TRUE;
        }
      }
      else
      {
        /*
        do nothing, as there is no modification to the original source entry
        at this index.
        */
      }
    }
    else
    {
      if (mods->entries[i].valid)
      {
#ifdef FEATURE_GSM_TDS
       if (mods->entries[i].type == UTRAN_FDD)
#endif /*FEATURE_GSM_TDS*/
       {
         MSG_GERAN_MED_3("Inserting index %d, UTRAN FDD ARFCN %d, sc %d",
          i,
          mods->entries[i].uarfcn,
          mods->entries[i].scrambling_code
        );
       }
#ifdef FEATURE_GSM_TDS
       else if (mods->entries[i].type == UTRAN_TDD)
       {
         MSG_GERAN_MED_3("Inserting index %d, UTRAN TDD ARFCN %d, cpid %d",
            i,
            mods->entries[i].uarfcn,
            mods->entries[i].cell_parameter_id
          );
       }
#endif /*FEATURE_GSM_TDS*/
        dst->entries[i] = mods->entries[i];
        changed = TRUE;
      }
      else
      {
        /*
        do nothing, as there is neither a source nor a modified entry at 
        this index.
        */
      }
    }
  }

  if (mods->absolute_index_start_EMR_valid)
  {
    dst->absolute_index_start_EMR_valid = TRUE;
    dst->absolute_index_start_EMR = mods->absolute_index_start_EMR;
  }

  return changed;
}


/*=====================================================================

  These functions deal with storing information received in instances
  of Packet Measurement Order message.

=====================================================================*/


/*=====================================================================

  These functions deal specifically with copying the data received in
  SI2quater instances in to a GPRS 3G System Information database.

=====================================================================*/



/**
 * Updates the 3G neighbour list based on the contents of the SI2quater instances.
 */
void rr_gprs_update_3g_nlist_from_si2quater_instances(
  instances_T *I,
  const gas_id_t gas_id
)
{
  int                    rep_FDD_index;
  uint8                  i;
#ifdef FEATURE_GSM_TDS
  int                    rep_TDD_index;
#endif /*FEATURE_GSM_TDS*/
  rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr;
  pmo_pcco_data_t *pmo_store_ptr;
  boolean		   nlist_3g_modified = FALSE;

  // Obtain a pointer to the module data
  rr_sys_info_3g_data_ptr = rr_sys_info_3g_get_data_ptr(gas_id);
  pmo_store_ptr = rr_psi_get_pmo_store_ptr(gas_id);
  
  RR_NULL_CHECK_FATAL(rr_sys_info_3g_data_ptr);

  /* clear the broadcast 3G neighbour list */
  memset(&rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast, 0, sizeof(grr_3g_nlist_t));
  memset(&rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2quater, 0, sizeof(grr_3g_nlist_unindexed_t));
#ifdef FEATURE_GSM_TDS
  memset(&rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2quater, 0, sizeof(grr_3g_nlist_unindexed_t));
#endif /*FEATURE_GSM_TDS*/

  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    SI2q_MI_instance_T *instance = &I->instance[i];

    /**
     * First, construct the 3G neighbour cell list/cell reselection list.
     */
    byte num_repeated_FDD_info = instance->common_params.num_repeated_FDD_info;

    byte curr_wcdma_cell_list_index = instance->common_params.Index_Start_3G;

#ifdef FEATURE_GSM_TDS
     byte num_repeated_TDD_info = instance->common_params.num_repeated_TDD_info;
#endif /*FEATURE_GSM_TDS*/

    if (!instance->valid_data)
    {
      continue;
    }

    for (
          rep_FDD_index = 0;
          (rep_FDD_index < num_repeated_FDD_info) && 
          (curr_wcdma_cell_list_index < MAX_UTRAN_CELLS);
          ++rep_FDD_index
        )
    {
      FDD_Info_T *fdd_info = &instance->FDD_Info[rep_FDD_index];

      if (fdd_info->FDD_Indic0)
      {
        grr_3g_nlist_entry_t entry = {0};

        entry.valid = TRUE;
        entry.type = UTRAN_FDD;
        entry.uarfcn = fdd_info->uarfcn;
        entry.scrambling_code = 0;
        entry.diversity = FALSE;
        entry.instance_received_in = i;

        (void) rr_gprs_3g_nlist_add(
          &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast,
          &entry,
          curr_wcdma_cell_list_index,
          gas_id
        );

        curr_wcdma_cell_list_index++;
      }


      if (fdd_info->num_cells > 0)
      {
        byte cell_index;
        byte num_cells = fdd_info->num_cells;

        for (cell_index = 1;
             (cell_index <= num_cells) && 
             (cell_index < MAX_UTRAN_CELLS_PER_INSTANCE) &&
             (curr_wcdma_cell_list_index < MAX_UTRAN_CELLS); /* Rule 6 */
             ++cell_index)
        {
          grr_3g_nlist_entry_t entry = {0};

          entry.valid = TRUE;
          entry.type = UTRAN_FDD;
          entry.uarfcn = fdd_info->uarfcn;
          entry.scrambling_code = (fdd_info->decoded_and_sorted_cell_values[cell_index] & 0x1ff) << 4;
          entry.diversity = (fdd_info->decoded_and_sorted_cell_values[cell_index] & 0x200) ? TRUE : FALSE;
          entry.instance_received_in = i;

          (void) rr_gprs_3g_nlist_add(
            &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast,
            &entry,
            curr_wcdma_cell_list_index, 
            gas_id
          );

          curr_wcdma_cell_list_index++;
        }
      }
      else if (fdd_info->num_cells == 0)
      {
        if (
             ((fdd_info->NR_OF_FDD_CELLS == 0) && (fdd_info->FDD_Indic0 == 0)) ||
             ((fdd_info->NR_OF_FDD_CELLS >= 17) && (fdd_info->NR_OF_FDD_CELLS <= 30))
           )
        {   
          grr_3g_nlist_entry_t entry = {0};

          entry.valid = TRUE;
          entry.type = UTRAN_FDD;
          entry.uarfcn = fdd_info->uarfcn;
          entry.frequency_only = TRUE;
          entry.instance_received_in = i;

          (void) rr_gprs_3g_nlist_add(
            &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast,
            &entry,
            curr_wcdma_cell_list_index,
            gas_id
          );

          curr_wcdma_cell_list_index++;
        }

        else if (fdd_info->NR_OF_FDD_CELLS == 31)
        {
          (void) rr_gprs_3g_nlist_unindexed_add(
            &rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2quater,
            fdd_info->uarfcn
          );
        }
      }
    }

#ifdef FEATURE_GSM_TDS
    for (
      rep_TDD_index = 0;
      (rep_TDD_index < num_repeated_TDD_info) && 
      (curr_wcdma_cell_list_index < MAX_UTRAN_CELLS);
      ++rep_TDD_index
    )
    {
      TDD_Info_T *tdd_info = &instance->TDD_Info[rep_TDD_index];

      if (tdd_info->TDD_Indic0)
      {
        grr_3g_nlist_entry_t entry = {0};

        entry.valid = TRUE;
        entry.type = UTRAN_TDD;
        entry.uarfcn = tdd_info->uarfcn;
        entry.cell_parameter_id = 0;
        entry.diversity = FALSE;
        entry.instance_received_in = i;
        entry.sync_case_TSTD = 0;

        (void) rr_gprs_3g_nlist_add(
          &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast,
          &entry,
          curr_wcdma_cell_list_index,
          gas_id
        );

        curr_wcdma_cell_list_index++;
      }


      if (tdd_info->num_cells > 0)
      {
        byte cell_index;
        byte num_cells = tdd_info->num_cells;

        for (cell_index = 1;
             (cell_index <= num_cells) && 
             (cell_index < MAX_UTRAN_CELLS_PER_INSTANCE) &&
             (curr_wcdma_cell_list_index < MAX_UTRAN_CELLS); /* Rule 6 */
             ++cell_index)
        {
          grr_3g_nlist_entry_t entry = {0};

          entry.valid = TRUE;
          entry.type = UTRAN_TDD;
          entry.uarfcn = tdd_info->uarfcn;
          entry.cell_parameter_id = tdd_info->decoded_and_sorted_cell_values[cell_index] & 0x007F;
          entry.diversity = (tdd_info->decoded_and_sorted_cell_values[cell_index] & 0x100) ? TRUE : FALSE;
          entry.instance_received_in = i;
          entry.sync_case_TSTD = (tdd_info->decoded_and_sorted_cell_values[cell_index] & 0x0080) >> 7;

          (void) rr_gprs_3g_nlist_add(
            &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast,
            &entry,
            curr_wcdma_cell_list_index,
            gas_id
          );

          curr_wcdma_cell_list_index++;
        }
      }
      else if (tdd_info->num_cells == 0)
      {
        if (
             ((tdd_info->NR_OF_TDD_CELLS == 0) && (tdd_info->TDD_Indic0 == 0)) ||
             ((tdd_info->NR_OF_TDD_CELLS >= 21) && (tdd_info->NR_OF_TDD_CELLS <= 30))
           )
        {   
          grr_3g_nlist_entry_t entry = {0};

          entry.valid = TRUE;
          entry.type = UTRAN_TDD;
          entry.uarfcn = tdd_info->uarfcn;
          entry.frequency_only = TRUE;
          entry.instance_received_in = i;

          (void) rr_gprs_3g_nlist_add(
            &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast,
            &entry,
            curr_wcdma_cell_list_index,
            gas_id
          );

          curr_wcdma_cell_list_index++;
        }

        else if (tdd_info->NR_OF_TDD_CELLS == 31)
        {
          (void) rr_gprs_3g_nlist_unindexed_add(
            &rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2quater,
            tdd_info->uarfcn
          );
        }
      }
    }

#endif /*FEATURE_GSM_TDS*/

    
    if (instance->common_params.Absolute_Index_Start_EMR_PI)
    {
      /* take absolut_index_start_EMR value from highest instance */
      if (instance->common_params.INDEX > 
          rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast.absolute_index_start_EMR_instance)
      {
        rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast.absolute_index_start_EMR_valid = TRUE;
        rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast.absolute_index_start_EMR = 
          instance->common_params.Absolute_Index_Start_EMR;
        rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast.absolute_index_start_EMR_instance = 
          instance->common_params.INDEX;
      }
    }
    
    rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast.source.ba_ind_3g = instance->common_params.BA_IND_3G;
  }

#ifdef FEATURE_WCDMA
  if ((pmo_store_ptr != NULL)&&( pmo_store_ptr->nlist_3g_valid ))
  { 	 
	 /*We have a Valid PMO data check now 3G BA IND in PMO and SI2q*/
	 if((rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast.source.ba_ind_3g) == (pmo_store_ptr->nlist_3g.source.ba_ind_3g))
	 {
	   MSG_HIGH("SI2q::3G_BA_IND = %d  PMO::3G_BA_IND is %d",
				 rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast.source.ba_ind_3g, 
				 pmo_store_ptr->nlist_3g.source.ba_ind_3g, 
				 0);				  
	   /* merge the PMO 3G BA list with the broadcast BA list to get the modified BA list,
			then send it to L1. */
	   nlist_3g_modified = rr_gprs_3g_nlist_apply_mods(&rr_sys_info_3g_data_ptr->grr_3g_nlist_modified,
															   &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast,
															   &pmo_store_ptr->nlist_3g);
  
	   if (nlist_3g_modified)
	   {
		 rr_sys_info_3g_data_ptr->grr_3g_nlist = &rr_sys_info_3g_data_ptr->grr_3g_nlist_modified;
	   }
	   else
	   {
		 rr_sys_info_3g_data_ptr->grr_3g_nlist = &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast;
	   }
	 }
	 else
	 {
	   MSG_HIGH("3G BA_IND different overwrite PMO",0, 0, 0);	 
	   rr_sys_info_3g_data_ptr->grr_3g_nlist = &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast;		  
	 }
	  
  }/*There is no PMO information hence update as usual*/
  else
#endif /* FEATURE_WCDMA */
  { 
	/* set up the global pointer to point to the default broadcast 3G neighbour list */
	rr_sys_info_3g_data_ptr->grr_3g_nlist = &rr_sys_info_3g_data_ptr->grr_3g_nlist_broadcast;
  }

}

/**
 * Updates the 3G neighbour list based on the contents of the SI2quater instances.
 */
void rr_gprs_update_3g_nlist_from_si2ter_instances(
  si2ter_instances_T *I,
  const gas_id_t gas_id
)
{
  uint8                  i;
  rr_sys_info_3g_data_t *rr_sys_info_3g_data_ptr = rr_sys_info_3g_get_data_ptr(gas_id);

  memset(&rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2ter, 0, sizeof(grr_3g_nlist_unindexed_t));
#ifdef FEATURE_GSM_TDS
   memset(&rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2ter, 0, sizeof(grr_3g_nlist_unindexed_t));
#endif /*FEATURE_GSM_TDS*/

  for (i = 0; i < MAX_SI2TER_INSTANCES; i++)
  {
    si2ter_rest_T *instance = &I->instance[i];

    if (!instance->valid_data)
    {
      continue;
    }

    if (instance->FDD_INFO_PI)
    {
      (void) rr_gprs_3g_nlist_unindexed_add(
        &rr_sys_info_3g_data_ptr->grr_3g_nlist_unindexed_si2ter,
        instance->FDD_arfcn);
    }

#ifdef FEATURE_GSM_TDS
    if (instance->TDD_INFO_PI)
    {
      (void) rr_gprs_3g_nlist_unindexed_add(
        &rr_sys_info_3g_data_ptr->grr_utran_tdd_nlist_unindexed_si2ter,
        instance->TDD_arfcn);
    }
#endif /*FEATURE_GSM_TDS*/
  }
}

/** Return the index for reporting RSSI on the specified UTRAN FDD frequency.

This function will return the index required for reporting RSSI on the 
specified UTRAN FDD frequency, or MAX_GRR_3G_NLIST_SIZE if the network does
not require RSSI reporting on the frequency.

@param sys_info pointer to the global 3G system information database to use
@param uarfcn the UTRAN FDD frequency
*/
int rr_gprs_3g_nlist_get_fdd_freq_index(grr_3g_nlist_t *nlist_3g, uint16 uarfcn)
{
  int i;
  grr_3g_nlist_entry_t *entry;

  for (i = MAX_GRR_3G_NLIST_SIZE-1; i >= 0; i--)
  {
    entry = &nlist_3g->entries[i];

    if ((entry->valid) && 
        (entry->type == UTRAN_FDD) && 
        (entry->frequency_only) && 
        (entry->uarfcn == uarfcn) &&
        (!entry->deleted))
    {
      MSG_GERAN_MED_2("Found UARFCN %d at index %d", uarfcn, i);
      return i;
    }
  }

  return MAX_GRR_3G_NLIST_SIZE;
}

/** Return the index for reporting RSCP or Ec/Io for the specified UTRAN FDD cell.

This function will return the index required for reporting RSCP or Ec/Io for the
specified UTRAN FDD cell, or MAX_GRR_3G_NLIST_SIZE if the cell is not in the
GPRS 3G Neighbour Cell List.

@param sys_info pointer to the global 3G system information database to use
@param uarfcn the frequency of the specified cell
@param scrambling_code the scrambling code of the specified cell
*/
int rr_gprs_3g_nlist_get_fdd_cell_index(grr_3g_nlist_t *nlist_3g, uint16 uarfcn, uint16 scrambling_code)
{
  int i;
  grr_3g_nlist_entry_t *entry;


  /* search from back to front as we must report the highest index in the case of duplicate entries */
  for (i = MAX_GRR_3G_NLIST_SIZE-1; i >= 0; i--)
  {
    entry = &nlist_3g->entries[i];

    if ((entry->valid) && 
        (entry->type == UTRAN_FDD) && 
        (!entry->frequency_only) && 
        (entry->uarfcn == uarfcn) &&
        (entry->scrambling_code == scrambling_code) &&
        (!entry->deleted))
    {
      MSG_GERAN_MED_3("Found UTRAN FDD cell %d, %d at index %d", uarfcn, scrambling_code, i);
      return i;
    }
  }

  return MAX_GRR_3G_NLIST_SIZE;
}

#ifdef FEATURE_GSM_TDS
/** Return the index for reporting RSCP for the specified UTRAN TDD cell.

This function will return the index required for reporting RSCP for the
specified UTRAN TDD cell, or MAX_GRR_3G_NLIST_SIZE if the cell is not in the
GPRS 3G Neighbour Cell List.

@param sys_info pointer to the global 3G system information database to use
@param uarfcn the frequency of the specified cell
@param scrambling_code the scrambling code of the specified cell
*/
int rr_gprs_3g_nlist_get_tdd_cell_index(grr_3g_nlist_t *nlist_3g, uint16 uarfcn, uint16 cpid)
{
  int i;
  grr_3g_nlist_entry_t *entry;


  /* search from back to front as we must report the highest index in the case of duplicate entries */
  for (i = MAX_GRR_3G_NLIST_SIZE-1; i >= 0; i--)
  {
    entry = &nlist_3g->entries[i];

    if ((entry->valid) && 
        (entry->type == UTRAN_TDD) && 
        (entry->uarfcn == uarfcn) &&
        (entry->cell_parameter_id == cpid) &&
        (!entry->deleted))
    {
      MSG_GERAN_MED_3("Found UTRAN TDD cell %d, %d at index %d", uarfcn, cpid, i);
      return i;
    }
  }

  return MAX_GRR_3G_NLIST_SIZE;
}
#endif /*FEATURE_GSM_TDS*/


/****************************************************************************
 *   Function name:  rr_get_sys_info_3g_data_ptr
 *   --------------------------
 *
 *   Description: Returns a pointer to rr_sys_info_3g_data.
 *
 *   Parameters: 
 *   -----------
 *
 *   Returns:  rr_sys_info_3g_data_t *
 *   --------
 *
 ****************************************************************************/
rr_sys_info_3g_data_t *rr_get_sys_info_3g_data_ptr(
  const gas_id_t gas_id
)
{
  return rr_sys_info_3g_get_data_ptr(gas_id);
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/
