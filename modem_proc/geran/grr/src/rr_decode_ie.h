#ifndef RR_DECODE_IE_H
#define RR_DECODE_IE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              rr_decode_ie.h

GENERAL DESCRIPTION
   This module contains functions that decodes information elements.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_decode_ie.h_v   1.2   21 May 2002 15:10:28   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_decode_ie.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/04    gc     CR42000: Fixes to handle Early Classmark Sending Restriction
Oct/21/03   VT     For rr_decode_ia_rest_octets(), added a new parameter,
                     starting_time_valid.
11/18/02    RG     Fixed bugs found in unit testing; Changes to include review comments for GSM to WCDMA Reselection
11/07/02    JAC    Added decode multirate config ie function
10/21/02    RG     Added GSM to WCDMA Reselection support (SI2 Qtr decode ).
04/11/02   JAC     added stored plmn support
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "comdef.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
#include "rr_plmn_freq_db.h"
#include "rr_l2_l1.h"
#include "geran_multi_sim.h"
#include "rr_gprs_defs.h"

/*===========================================================================

                         DEFINITIONS

===========================================================================*/

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

void rr_decode_cell_channel_description
(
  byte*                        IE,
  cell_channel_description_T*  cell_allocation,
  boolean*                     decoding_ok,
  const gas_id_t               gas_id
);

void rr_decode_RACH_control_parameters
(
  byte*                       IE,
  RACH_control_parameters_T*  RACH_control_parameters
);

void rr_decode_neighbour_cell_description
(
  byte*                   IE,
  byte*                   EXT_IND,
  BCCH_frequency_list_T*  BCCH_frequency_list,
  boolean*                decoding_ok,
  const gas_id_t          gas_id
);

void rr_decode_neighbour_cell_description_2
(
  byte*                   IE,
  byte*                   multiband_reporting,
  BCCH_frequency_list_T*  BCCH_frequency_list,
  boolean*                decoding_ok,
  const gas_id_t          gas_id
);

word rr_decode_cell_identity
(
  byte* IE
);

void rr_decode_location_area_identification
(
  byte*  IE,
  LAI_T* location_area_identification
);

void rr_decode_control_channel_description
(
  byte*                           IE,
  control_channel_description_T*  control_channel_description,
  boolean*                        decoding_ok
);

void rr_decode_cell_options_on_bcch
(
  byte*            IE,
  cell_options_T*  cell_options
);

void rr_decode_cell_selection_parameters
(
  byte*                         IE,
  cell_selection_parameters_T*  cell_selection_parameters
);

/* temporary structure that holds the encoded cells received in SI2quater */
typedef struct
{
  word uarfcn;
  boolean TDD_Indic0;
  byte    NR_OF_TDD_CELLS;
  byte    num_cells;
  word    encoded_cells[MAX_UTRAN_CELLS_PER_FREQUENCY+1]; /* array index 0 is unused; decoding algorithm starts looking from 1 */
} encoded_TDD_info_T;

typedef struct
{
  word uarfcn;
  boolean FDD_Indic0;
  byte NR_OF_FDD_CELLS;
  byte num_cells;
  word encoded_cells[MAX_UTRAN_CELLS_PER_FREQUENCY+1]; /* array index 0 is unused; decoding algorithm starts looking from 1 */
} encoded_FDD_info_T;

/* rr_decode_FDD_cells is used by GRR only */
void rr_decode_FDD_cells(
  byte                num_FDD_cells,
  word              * encoded_cells,
  word                uarfcn,
  byte                FDD_Indic0,
  wcdma_cell_T      * cell_list,
  byte              * num_cells,
  boolean           * decoding_ok,
  gprs_scell_info_t * cell_info_ptr
);

boolean rr_SI2q_MI_instance_already_received(instances_T *SI2quater, byte SI2quater_INDEX);
boolean rr_SI2q_MI_changed (instances_T *instances);

void rr_decode_si2quater_rest_octets(
  byte                          * IE,
  byte                            IE_length,
  instances_T                   * SI2quater,
  boolean                       * si2quater_received,
  boolean                       * decoding_ok,
  ARFCN_T                       * channel_number,
  system_information_database_T * cell_info_ptr,
  const gas_id_t                gas_id
);

boolean rr_si2ter_3G_BA_changed(si2ter_instances_T *instances, boolean si2ter_3g_change_mark);
boolean rr_si2ter_MP_changed(si2ter_instances_T *instances, boolean si2ter_mp_change_mark);
void rr_update_status_si2ter_instance (
  si2ter_instances_T *instances,
  byte index,
  boolean received,
  boolean changed
);
boolean rr_received_all_si2ter_instances(si2ter_instances_T *instances, byte array_length, byte COUNT);
boolean rr_si2ter_changed (si2ter_instances_T *instances);
void rr_decode_si2ter_rest_octets
(
  byte                          *IE,
  byte                           IE_length,
  si2ter_instances_T            *si2ter_rest_instances,
  boolean                       *si2ter_received,
  boolean                       *decoding_ok,
  system_information_database_T *sys_info_db_ptr,
  const gas_id_t                 gas_id
);

void rr_decode_neighbour_cell_description_MI
(
  byte                  *IE,
  byte                  IE_length,
  instances_T           *MeasInfo,
  boolean               *send_wcdma_neighbor_list_to_L1,
  boolean               *mi_received,
  boolean               *decoding_ok
);

void rr_decode_channel_description
(
  byte*                   IE,
  channel_information_T*  channel_information,
  boolean*                decoding_ok,
  const gas_id_t          gas_id
);

void rr_decode_channel_description_2
(
  byte*                   IE,
  channel_information_T*  channel_information,
  boolean*                decoding_ok,
  const gas_id_t          gas_id
);

void rr_decode_mobile_allocation
(
  byte*                 IE,
  mobile_allocation_T*  mobile_allocation
);

void rr_decode_cell_options_on_sacch
(
  byte*                 IE,
  cell_options_T*       cell_options
);

void rr_decode_starting_time
(
  byte*  IE,
  word*  starting_time,
  boolean *decode_ok
);

void rr_decode_channel_mode
(
  byte*             IE,
  channel_mode_T*   channel_mode,
  boolean*          decoding_ok
);

void rr_decode_cell_description
(
  byte* IE,
  cell_description_T* cell_description,
  const gas_id_t gas_id
);

void rr_decode_short_freq_list
(
  byte* IE,
  frequency_list_T* frequency_list,
  boolean* decoding_ok,
  const gas_id_t gas_id
);

void rr_decode_frequency_list
(
  byte* IE,
  ARFCN_T channel_number,
  frequency_list_T* frequency_list,
  boolean* decoding_ok,
  const gas_id_t gas_id
);

void rr_decode_multirate_config_ie
(
  byte                *IE_ptr,
  multirate_config_T  *pConfig,
  boolean             *decoding_ok,
  channel_mode_T       channel_mode_in_ass,
  const gas_id_t       gas_id
);

void rr_decode_frequency_channel_sequence
(
  byte* IE,
  frequency_list_T* frequency_list,
  boolean* decoding_ok,
  const gas_id_t gas_id
);


void rr_decode_ia_rest_octets
(
  byte *IE,
  boolean *info_valid,
  byte *MAIO,
  mobile_allocation_T *mobile_allocation,
  boolean starting_time_valid,
  boolean *irat_ho_info_compressed,
  uint8 *end_of_msg_ptr,
  boolean *decode_ok
);

void rr_decode_request_reference
(
  byte *IE,
  byte *access_reference_ptr,
  word *frame_number_ptr
);


extern boolean rr_decode_cell_selection_indicator(
  byte *IE,
  byte IE_length,
  rr_chan_rel_cell_selection_indicator_t * cell_sel_indicator,
  const gas_id_t gas_id
);


int compare(const void *arg1, const void *arg2);

#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/

/**
  @brief Free any memory that has been allocated under the given SI2quater
  parameters structure.

  @param item Pointer to the SI2quater parameters item to free the children of.

  @note This function does not attempt to free the pointer to the object passed
  in.
*/
extern void rr_si2q_params_free_children(
  SI2q_params_T * item
);

// Functions for managing IEs and OTA messages

extern ie_descriptor_T *rr_get_first_ie_and_length(byte, uint8 *, const gas_id_t gas_id);
extern ie_descriptor_T *rr_get_first_ie_of_message(byte, const gas_id_t gas_id);
extern byte * rr_find_start_of_IE(ie_descriptor_T *, byte);



extern uint8 rr_length_to_end_of_ota_msg(
  uint8 msg_type,
  uint8 *csn1_start_ie_ptr,
  csn_rr_decoder_msg_id_t csn_ie_type,
  const gas_id_t gas_id
);


extern void rr_check_for_protocol_errors(
  byte              *layer3_message,
  byte              message_length,
  l2_channel_type_T l2_channel_type,
  boolean           short_header,
  boolean           *message_ok,
  RR_cause_T        *error_type,
  const gas_id_t     gas_id
);

#ifdef FEATURE_LTE

/**
  @brief Decodes an Individual Priorities IE.

  Individual Priorities IEI is defined in 3GPP 44.018 section 10.5.2.75.

  It is possible that the IE will contain no individual priority information
  but will instead cause the UE to delete any existing individual priority
  information.  This is indicated to the caller by the return value of this
  function.
  
  @param ie Pointer to the start of the encoded IE byte array.
  @param length Length of the encoded IE byte array.
  @param individual_priorities_ie Pointer to the storage for the decoded data.
  @return TRUE if valid Individual Priority information is available in the
          IE, FALSE if any existing Individual Priority information should be
          deleted.
*/
extern boolean rr_individual_priorities_ie_decode(
  uint8 *ie,
  uint8 length,
  rr_individual_priorities_ie_t * individual_priorities_ie
);

/**
  @brief Logs the contents of an Individual Priorities IE to diag.

  @ie the Individual Priorities IE to dump to diag.
*/
extern void rr_individual_priorities_ie_dump(
  rr_individual_priorities_ie_t * ie
);

/**
  @brief Frees memory associated with the specified Individual Priorities IE
         data structure.

  @note This function does not free the provided pointer as it has no way of
        knowing whether it points to dynamically allocated storage.

  @param item Pointer to the data structure to free.
*/
extern void rr_individual_priorities_ie_free_children(
  rr_individual_priorities_ie_t * item
);

#endif /* FEATURE_LTE */

#endif /* RR_DECODE_IE_H */
