#ifndef _RR_SYS_INFO_3G_H_
#define _RR_SYS_INFO_3G_H_

/*==============================================================================

            R R   S Y S   I N F O   3 G   ---   H E A D E R   F I L E

GENERAL DESCRIPTION
  This module provides functions and variables to allow storage and manipulation
  of 3G system information (neighbour cell lists).

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2013 Qualcomm Technologies, Inc.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_3g.h#1 $

==============================================================================*/


/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

#include "csn.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_defs.h"
#include "geran_multi_sim.h"

/*=====================================================================

  Typedefs

=====================================================================*/

/*!
The maximum number of 3G neighbour cells that may be stored.
*/
#define MAX_GRR_3G_CELLS 96

/*!
The maximum number of standalone 3G frequencies that may be stored.
*/
#define MAX_GRR_3G_FREQS 8

/*!
The maximum size of the GPRS 3G Neighbour Cell List.
*/
#define MAX_GRR_3G_NLIST_SIZE (MAX_GRR_3G_CELLS + MAX_GRR_3G_FREQS)

/**
 * This enum represents the different types of RAT that a 3G neighbour cell or frequency may belong to.  Each 3G neighbour cell or frequency must have a type associated with it.
 */
typedef enum
{
  UTRAN_FDD,
  UTRAN_TDD,
  CDMA_2000
} grr_3g_neighbour_type_e;

/**
 * To indicate normal or high reporting priority for a 3G cell.
 */
typedef enum
{
  NORMAL_REP_PRIORITY,
  HIGH_REP_PRIORITY
} grr_3g_neighbour_rep_priority_e;

/**
 * This enum provides an indication to the source of a set of 3G system information.  It will indicate that it was received on either the BCCH or PBCCH.
 */
typedef enum
{
  GRR_3G_SYS_INFO_SOURCE_NONE,
  GRR_3G_SYS_INFO_SOURCE_BCCH,
  GRR_3G_SYS_INFO_SOURCE_PBCCH
} grr_3g_source_e;

/**
 * This structure represents a single entry in the GPRS 3G Neighbour Cell List.
 */
typedef struct
{
  boolean valid;
  boolean deleted;
  grr_3g_neighbour_type_e type;
  boolean frequency_only;
  uint16 uarfcn;
  uint16 scrambling_code;
  boolean diversity;
  uint8 instance_received_in;
  boolean rep_priority_valid;
  grr_3g_neighbour_rep_priority_e rep_priority;
  uint8 rep_priority_instance_received_in;
#ifdef FEATURE_GSM_TDS
  uint16 cell_parameter_id;
  uint8 sync_case_TSTD;
#endif /*FEATURE_GSM_TDS*/
} grr_3g_nlist_entry_t;

/**
 * Broadcast information will either be received in the BCCH (ba_ind and 
 * ba_ind_3g) or PBCCH (psi3_change_mark).  This information can then be
 * modified by PMO or PCCO messages, but only if they have the same 
 * source/source id.
 */
typedef struct
{
  grr_3g_source_e bcch_or_pbcch;

  uint8 ba_ind;
  uint8 ba_ind_3g;
  uint8 psi3_change_mark;

} grr_3g_source_id_t;

/**
 * This structure represents the GPRS 3G Neighbour Cell List.
 */
typedef struct
{
  grr_3g_source_id_t source;
  uint8 absolute_index_start_EMR_instance;
  boolean absolute_index_start_EMR_valid;
  uint8 absolute_index_start_EMR;
  grr_3g_nlist_entry_t entries[MAX_GRR_3G_NLIST_SIZE];
} grr_3g_nlist_t;

/**
 * This structure stores 3G frequencies that have been received without 
 * associated scrambling codes.
 */
typedef struct
{
  uint8 size;
  uint16 uarfcns[MAX_GRR_3G_FREQS];
} grr_3g_nlist_unindexed_t;

/**
 * This structure represents the 3G Measurement Parameters received in 
 * either SI2quater or PSI3quater.
 */
typedef struct
{
  int instance;

  uint8 THREE_G_BA_IND;
  uint8 Qsearch_I;
  uint8 Qsearch_C_Initial;

  uint8 FDD_Qoffset;
  fdd_rep_quant_T FDD_REP_QUANT;
  uint8 FDD_MULTIRAT_REPORTING;
  uint8 FDD_Qmin;

  uint8 TDD_Qoffset;
  uint8 TDD_MULTIRAT_REPORTING;

} gsm_3g_meas_params_t;

typedef struct
{
  grr_3g_nlist_t           *grr_3g_nlist;
  grr_3g_nlist_t            grr_3g_nlist_broadcast;
  grr_3g_nlist_t            grr_3g_nlist_modified;
  grr_3g_nlist_unindexed_t  grr_3g_nlist_unindexed_si2quater;
  grr_3g_nlist_unindexed_t  grr_3g_nlist_unindexed_si2ter;
  grr_3g_nlist_unindexed_t  grr_3g_nlist_unindexed_psi3quater;
#ifdef FEATURE_GSM_TDS
  grr_3g_nlist_unindexed_t  grr_utran_tdd_nlist_unindexed_si2quater;
  grr_3g_nlist_unindexed_t  grr_utran_tdd_nlist_unindexed_si2ter;
#endif /*FEATURE_GSM_TDS*/
} rr_sys_info_3g_data_t;


/*=====================================================================

  Externally visible functions.

=====================================================================*/

/**
 * Should be called at system initialisation to set up the 3G system information stores.
 */
extern void rr_init_sys_info_3g(
  const gas_id_t gas_id
);

/**
 * Returns TRUE if the specified grr_3g_nlist_t is valid, FALSE otherwise.
 */
extern boolean rr_gprs_3g_nlist_valid(
  grr_3g_nlist_t *
);

/**
 * Adds the entry to the list specified, according to the appropriate
 * criteria.
 *
 * @param list the list to add the entry to
 * @param entry the entry to add
 * @param index the index to add the specified entry at
 * @return boolean TRUE if the entry was added to the list, FALSE otherwise
 */
extern boolean rr_gprs_3g_nlist_add(
  grr_3g_nlist_t * list, 
  grr_3g_nlist_entry_t * entry, 
  int index,
  const gas_id_t gas_id
);

/**
 * Adds the entry to the list specified, according to the appropriate
 * criteria.
 *
 * @param list the list to add the entry to
 * @param uarfcn the uarfcn to add
 * @return boolean TRUE if the uarfcn was added to the list, FALSE otherwise
 */
extern boolean rr_gprs_3g_nlist_unindexed_add(
  grr_3g_nlist_unindexed_t * list,
  uint16 uarfcn
);

/**
 * Decodes the 3G Neighbour Cell description (UTRAN FDD cells)in an instance 
 * of PSI3quater or Packet Measurement Order or a Packet Cell Change Order and
 * adds them to the GPRS 3G Cell Reselection list.
 *
 * @param list the destination 3G Neighbour Cell List to add new 3G neighbours to
 * @param unindexed_list the destination list to add any standalone 3G frequencies to
 * @param instance the message instance that is currently being processed
 * @param fdd_cells the encoded FDD cell information
 * @param index_start_3g the index at which to start adding 3G neighbours to the 3G Neighbour Cell List
 */
extern void rr_gprs_3g_nlist_decode_fdd_cells(
  grr_3g_nlist_t * list,
  grr_3g_nlist_unindexed_t * unindexed_list,
  uint8 instance,
  utran_fdd_description_t * fdd_cells,
  int * index_start_3g,
  const gas_id_t gas_id
);

/**
 * Decodes the 3G Neighbour Cell description (UTRAN TDD cells)in an instance 
 * of PSI3quater or Packet Measurement Order or a Packet Cell Change Order and
 * adds them to the GPRS 3G Cell Reselection list.
 *
 * @param list the destination 3G Neighbour Cell List to add new 3G neighbours to
 * @param instance the message instance that is currently being processed
 * @param tdd_cells the encoded TDD cell information
 * @param index_start_3g the index at which to start adding 3G neighborus to the 3G Neighbour Cell List
 */
extern void rr_gprs_3g_nlist_decode_tdd_cells(
/*  grr_3g_nlist_t * list, */
/*  int instance, */
  utran_tdd_description_t * tdd_cells,
  int * index_start_3g
);


/**
 * Decodes the 3G Neighbour Cell description (CDMA2000 cells )in an instance 
 * of PSI3quater or Packet Measurement Order or a Packet Cell Change Order and
 * adds them to the GPRS 3G Cell Reselection list.
 */
extern void rr_gprs_3g_nlist_decode_cdma2000_cells(
/*  grr_3g_nlist_t * list, */
/*  int instance, */
  cdma2000_description_t * cdma2000_cells,
  int * index_start_3g
);

/**
 * Decodes the 'Removed 3G Cell Description' sent in an instance of PMO or PCCO.
 */
extern void rr_gprs_3g_nlist_decode_removed_3g_cell_description(
  grr_3g_nlist_t * list,
  removed_3g_cell_description_t * removed_3g_cell_description
);


extern void rr_gprs_update_3g_nlist_from_si2quater_instances(
  instances_T *I,
  const gas_id_t gas_id
);

extern void rr_gprs_update_3g_nlist_from_si2ter_instances(
  si2ter_instances_T *si2ter_instances,
  const gas_id_t gas_id
);

/**
 * Applies modifications to a 3G neighbour list.
 *
 * @param dst pointer to the location where the modified 3G neighbour list should be stored
 * @param src pointer to the 3G neighbour list to be modified
 * @param mods pointer to the 3G neighobur list modifications to be applied
 * @returns boolean TRUE if any modifications are made to the list, FALSE otherwise
 */
extern boolean rr_gprs_3g_nlist_apply_mods(
  grr_3g_nlist_t * dst,
  grr_3g_nlist_t * src,
  grr_3g_nlist_t * mods
);

extern int rr_gprs_3g_nlist_get_fdd_cell_index(grr_3g_nlist_t *nlist_3g, uint16 uarfcn, uint16 scrambling_code);
extern int rr_gprs_3g_nlist_get_fdd_freq_index(grr_3g_nlist_t *nlist_3g, uint16 uarfcn);

#ifdef FEATURE_GSM_TDS
extern int rr_gprs_3g_nlist_get_tdd_cell_index(grr_3g_nlist_t *nlist_3g, uint16 uarfcn, uint16 cpid);
#endif /*FEATURE_GSM_TDS*/

/****************************************************************************
 *   Function name:  rr_sys_info_3g_data_ptr
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
);

#ifdef FEATURE_GSM_TDS
void rr_gprs_3g_nlist_decode_tdd_cells_for_tds(
  grr_3g_nlist_t * list,
  int instance,
  utran_tdd_description_t * tdd_cells,
  int * index_start_3g,
  const gas_id_t gas_id
);
#endif /*FEATURE_GSM_TDS*/

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/
#endif /* _RR_SYS_INFO_3G_H_ */
