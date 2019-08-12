/*! \file rr_select_bcch_if.c 
 
  This file contains the functionality to send the RR-SELECT-BCCH IMSG/OMSG messages.
 
                Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_select_bcch_if.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_select_bcch_if.h"
#include "rr_rr.h"
#include "rr_defs.h"
#include "gprs_mem.h"
#include "geran_dual_sim.h"
#include "stringl/stringl.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* --------------------------------------------------------------------------*/

/*!
 * \brief Sends RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ.
 *
 *  This message instructs the RR-SELECT-BCCH module to perform a reselection power-scan to verify the
 *  presence of a cell, select it, and acquire the indicate SI messages.
 * 
 * \param arfcn 
 * \param cell_suitable_cb_fn - Call-back function to be called when SI3 is received. If this call-back
 *                              return FALSE, then the cell is deemed not suitable and a fail indication will
 *                              be output.
 * \see rr_cell_suitable_cb_fn_t
 * \param notify_on_si_mask   - When the indicated SIs have been received, a notification indication is sent
 *                              out (but this does not stop the procedure).
 *                              Set to MASK_SI_NONE if not required.
 * \param wait_for_si_mask    - When the indicated SIs have been received, the procedure completes.
 *                              Note: If this is set to MASK_SI_NONE then the module will exit as soon as the 
 *                              cell is selected (no early camping). If early camping is required, include
 *                              include MASK_SI_3.
 * \param gas_id_t 
 */
void rr_select_bcch_send_imsg_pscan_select_req(
  ARFCN_T arfcn,
  rr_cell_suitable_cb_fn_t cell_suitable_cb_fn,
  uint16 notify_on_si_mask,
  uint16 wait_for_si_mask,
  const gas_id_t gas_id
)
{
  rr_select_bcch_imsg_pscan_select_req_t msg;

  MSG_GERAN_HIGH_2_G(
    "IMSG_PSCAN_SELECT_REQ: cb_fn=%d wait_for_si_mask=0x%x",
    (int)(cell_suitable_cb_fn != NULL),
    (int)wait_for_si_mask
  );

  msg.arfcn = arfcn;
  msg.cell_suitable_cb_fn = cell_suitable_cb_fn;
  msg.notify_on_si_mask = notify_on_si_mask;
  msg.wait_for_si_mask = wait_for_si_mask;

  rr_rr_send(
    &msg.header,
    (uint8) RR_SELECT_BCCH_IMSG_PSCAN_SELECT_REQ,
    sizeof(msg),
    RR_SELECT_BCCH_IMSG,
    gas_id
  );
}


/*!
 * \brief Sends RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ.
 *
 *  This message instructs the RR-SELECT-BCCH module to perform a reselection power-scan over a list of
 *  supplied ARFCNs, calling the supplied cell suitability call-back function to determine which cell to pick.
 *  After a suitable cell is found, it is selected and the indicated SIs aquired.
 * 
 * \param cell_list 
 * \param num_of_cells 
 * \param decode_si3_only     - If set to TRUE, only SI3 will be decoded during the parallel list decode.
 * \param cell_suitable_cb_fn - Call-back function to be called when SI3/SI4 is received. If this call-back
 *                              returns FALSE, then the cell is deemed not suitable and the next cell in the 
 *                              list is tried. If at the end of the list, a fail indication will be output.
 * \see rr_cell_suitable_cb_fn_t
 * \param notify_on_si_mask   - When the indicated SIs have been received, a notification indication is sent
 *                              out (but this does not stop the procedure).
 *                              Set to MASK_SI_NONE if not required.
 * \param wait_for_si_mask    - When the indicated SIs have been received, the procedure completes.
 *                              Note: If this is set to MASK_SI_NONE then the module will exit as soon as the
 *                              cell is selected (no early camping).
 * \param max_allowed_time    - Maximum allowed time for the acquisition to complete.
 * \param gas_id_t 
 */
void rr_select_bcch_send_imsg_pscan_list_select_req(
  ARFCN_T *cell_list,
  uint16 num_of_cells,
  boolean decode_si3_only,
  rr_cell_suitable_cb_fn_t cell_suitable_cb_fn,
  uint16 notify_on_si_mask,
  uint16 wait_for_si_mask,
  uint32 max_allowed_time,
  const gas_id_t gas_id
)
{
  rr_select_bcch_imsg_pscan_list_select_req_t msg;
  uint16 mem_size;

  MSG_GERAN_HIGH_3_G(
    "IMSG_PSCAN_LIST_SELECT_REQ: num_of_cells=%d cb_fn=%d wait_for_si_mask=0x%x",
    (int)num_of_cells,
    (int)(cell_suitable_cb_fn != NULL),
    (int)wait_for_si_mask
  );

  if (num_of_cells > RR_MAX_RES_PSCAN_CELL_LIST_SIZE)
  {
    num_of_cells = RR_MAX_RES_PSCAN_CELL_LIST_SIZE;
  }

  mem_size = num_of_cells * sizeof(ARFCN_T);

  msg.cell_list = GPRS_MEM_MALLOC(mem_size);
  RR_NULL_CHECK_RETURN_VOID(msg.cell_list);

  memscpy(msg.cell_list, mem_size, cell_list, mem_size);
  msg.num_of_cells = num_of_cells;
  msg.cell_suitable_cb_fn = cell_suitable_cb_fn;
  msg.notify_on_si_mask = notify_on_si_mask;
  msg.wait_for_si_mask = wait_for_si_mask;
  msg.max_allowed_time = max_allowed_time;

  rr_rr_send(
    &msg.header,
    (uint8) RR_SELECT_BCCH_IMSG_PSCAN_LIST_SELECT_REQ,
    sizeof(msg),
    RR_SELECT_BCCH_IMSG,
    gas_id
  );

  // Note: The memory allocated for 'cell_list' is freed in rr_select_bcch.c
}


/*!
 * \brief Sends RR_SELECT_BCCH_IMSG_SELECT_REQ.
 *
 *  This message instructs the RR-SELECT-BCCH module to select the specified cell and acquire the
 *  indicated SI messages.
 *  Note: It is assumed that L1 is not tuned to the cell, and a MPH_SELECT_SPECIFIC_BCCH_REQ will be sent to
 *  L1 to do this. If L1 is already tuned to the cell, then use RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ instead.
 * 
 * \param arfcn 
 * \param rxlev_average 
 * \param cell_suitable_cb_fn - Call-back function to be called when SI3 is received. If this call-back
 *                              returns FALSE, then the cell is deemed not suitable and a fail indication will
 *                              be output.
 *                              Set to NULL if not required.
 * \see rr_cell_suitable_cb_fn_t
 * \param early_camp_required - Whether early camping is required or not.
 * \param notify_on_si_mask   - When the indicated SIs have been received, a notification indication is sent
 *                              out (but this does not stop the procedure).
 *                              Set to MASK_SI_NONE if not required.
 * \param wait_for_si_mask    - When the indicated SIs have been received, the procedure completes.
 *                              Note: If this is set to MASK_SI_NONE then the module will exit as soon as the
 *                              cell is selected (no early camping). If early camping is required, then this
 *                              is internally modified to add SI3.
 * \param gas_id 
 */
void rr_select_bcch_send_imsg_select_req(
  ARFCN_T arfcn,
  byte rxlev_average,
  rr_cell_suitable_cb_fn_t cell_suitable_cb_fn,
  boolean early_camp_required,
  uint16 notify_on_si_mask,
  uint16 wait_for_si_mask,
  BSIC_T BSIC,
  const gas_id_t gas_id
)
{
  rr_select_bcch_imsg_select_req_t msg;

  MSG_GERAN_HIGH_3_G(
    "IMSG_SELECT_REQ: cb_fn=%d early_camp=%d wait_for_si_mask=0x%x",
    (int)(cell_suitable_cb_fn != NULL),
    (int)early_camp_required,
    (int)wait_for_si_mask
  );

  msg.arfcn = arfcn;
  msg.rxlev_average = rxlev_average;
  msg.cell_suitable_cb_fn = cell_suitable_cb_fn;
  msg.early_camp_required = early_camp_required;
  msg.notify_on_si_mask = notify_on_si_mask;
  msg.wait_for_si_mask = wait_for_si_mask;
  msg.bsic = BSIC;

  rr_rr_send(
    &msg.header,
    (uint8) RR_SELECT_BCCH_IMSG_SELECT_REQ,
    sizeof(msg),
    RR_SELECT_BCCH_IMSG,
    gas_id
  );
}

/*!
 * \brief Sends RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ.
 *
 *  This message instructs the RR-SELECT-BCCH module to acquire the indicated SI messages of the already
 *  selected cell.
 *  Note: It is assumed that L1 is already tuned to the cell (i.e. L1 has just left dedicated state, or a
 *  MPH_SELECT_SPECIFIC_BCCH_REQ has previously been. sent) and SIs are being reaed. If this is not the case,
 *  then use RR_SELECT_BCCH_IMSG_SELECT_REQ instead.
 *
 * \param arfcn 
 * \param rxlev_average 
 * \param bsic                - BSIC of the selected cell
 * \param cell_suitable_cb_fn - Call-back function to be called when SI3 is received. If this call-back
 *                              returns FALSE, then the cell is deemed not suitable and a fail indication
 *                              will be output.
 *                              Set to NULL if not required.
 * \see rr_cell_suitable_cb_fn_t
 * \param early_camp_required - The type of early camping method required.
 * \see rr_select_bcch_early_camp_e
 * \param notify_on_si_mask   - When the indicated SIs have been received, a notification indication is sent
 *                              out (but this does not stop the procedure).
 *                              Set to MASK_SI_NONE if not required.
 * \param wait_for_si_mask    - When the indicated SIs have been received, the procedure completes.
 *                              Note: If this is set to MASK_SI_NONE then the module will exit as soon as the
 *                              cell is selected (no early camping). If early camping is required, then this
 *                              is automatically modified to add SI3 (MASK_SI_3).
 * \param init_params_ptr     - Initial paramters (optional).
 *                              Set to NULL if not required.
 * \param max_allowed_time_ms - Maximum allowed time (in millisecs) for the acquisition. Set to 0 if not 
 *                              required (i.e. the caller will run a timer)
 * \param gas_id
 */
void rr_select_bcch_send_imsg_acquire_si_req(
  ARFCN_T arfcn,
  byte rxlev_average,
  BSIC_T bsic,
  rr_cell_suitable_cb_fn_t cell_suitable_cb_fn,
  rr_select_bcch_early_camp_e early_camp_required,
  uint16 notify_on_si_mask,
  uint16 wait_for_si_mask,
  rr_select_bcch_init_params_t *init_params_ptr,
  uint32 max_allowed_time_ms,
  const gas_id_t gas_id
)
{
  rr_select_bcch_imsg_acquire_si_req_t msg;

  MSG_GERAN_HIGH_4_G(
    "IMSG_ACQUIRE_SI_REQ: cb_fn=%d early_camp=%d wait_for_si_mask=0x%x init_params=%d",
    (int)(cell_suitable_cb_fn != NULL),
    (int)early_camp_required,
    (int)wait_for_si_mask,
    (int)(init_params_ptr != NULL)
  );

  RR_ZERO_STRUCTURE(msg);

  msg.arfcn = arfcn;
  msg.rxlev_average = rxlev_average;
  msg.bsic = bsic;
  msg.cell_suitable_cb_fn = cell_suitable_cb_fn;
  msg.early_camp_required = early_camp_required;
  msg.notify_on_si_mask = notify_on_si_mask;
  msg.wait_for_si_mask = wait_for_si_mask;

  if (init_params_ptr != NULL)
  {
    msg.init_params = *init_params_ptr;
    msg.init_params_valid = TRUE;
  }
  else
  {
    if (early_camp_required == RR_SELECT_BCCH_EARLY_CAMP_USE_PARAMS)
    {
      MSG_GERAN_ERROR_0_G("init_params_ptr==NULL");
      return;
    }
  }

  msg.max_allowed_time_ms = max_allowed_time_ms;

  rr_rr_send(
    &msg.header,
    (uint8) RR_SELECT_BCCH_IMSG_ACQUIRE_SI_REQ,
    sizeof(msg),
    RR_SELECT_BCCH_IMSG,
    gas_id
  );
}

/*!
 * \brief Called to inform RR-SELECT-BCCH module that the indicated SI message has been received & decoded.
 * 
 * \param si_received 
 * \param gas_id 
 */
void rr_select_bcch_send_imsg_bsi_received_ind(
  rr_si_type_t si_received,
  const gas_id_t gas_id
)
{
  rr_select_bcch_imsg_bsi_received_ind_t msg;

  msg.si_received = si_received;

  rr_rr_send(
    &msg.header,
    (uint8) RR_SELECT_BCCH_IMSG_BSI_RECEIVED_IND,
    sizeof(msg),
    RR_SELECT_BCCH_IMSG,
    gas_id
  );
}

/*!
 * \brief Sends RR_SELECT_BCCH_IMSG_ABORT_REQ. Called to abort the current procedure.
 * 
 * \param gas_id 
 */
void rr_select_bcch_send_imsg_abort_req(
  const gas_id_t gas_id
)
{
  rr_select_bcch_imsg_abort_req_t msg;

  rr_rr_send(
    &msg.header,
    (uint8) RR_SELECT_BCCH_IMSG_ABORT_REQ,
    sizeof(msg),
    RR_SELECT_BCCH_IMSG,
    gas_id
  );
}

/*!
 * \brief Call to send RR_SELECT_BCCH_OMSG_SELECT_CNF.
 *
 *  The message indicates that the requested cell has been successfully selected, and the specified SIs have
 *  been acquired.
 * 
 * \param bsic 
 * \param early_camp_status - Which early camp method was performed.
 * \see rr_select_bcch_early_camp_e
 * \param si_received_mask  - Which SIs were acquired.
 * \param gas_id 
 */
void rr_select_bcch_send_omsg_select_cnf(
  ARFCN_T arfcn,
  BSIC_T bsic,
  boolean early_camped,
  uint16 si_received_mask,
  const gas_id_t gas_id
)
{
  rr_select_bcch_omsg_select_cnf_t msg;

  msg.arfcn = arfcn;

  msg.bsic = bsic;
  msg.early_camped = early_camped;
  msg.si_received_mask = si_received_mask;

  rr_rr_send(
    &msg.header,
    (uint8) RR_SELECT_BCCH_OMSG_SELECT_CNF,
    sizeof(msg),
    RR_SELECT_BCCH_OMSG,
    gas_id
  );
}

/*!
 * \brief Sends RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND, which indicates the procedure failed.
 * 
 *  This indicates that the requested cell could not be selected, or that the specified SIs could not be
 *  acquired.
 *
 * \param gas_id 
 */
void rr_select_bcch_send_omsg_select_fail_ind(
  ARFCN_T arfcn,
  boolean early_camped,
  rr_select_bcch_fail_reason_e reason,
  const gas_id_t gas_id
)
{
  rr_select_bcch_omsg_select_fail_ind_t msg;

  MSG_GERAN_HIGH_2_G(
    "OMSG_SELECT_FAIL_IND: early_camped=%d reason=%d",
    (int)early_camped,
    (int)reason
  );

  msg.arfcn = arfcn;
  msg.early_camped = early_camped;
  msg.reason = reason;

  rr_rr_send(
    &msg.header,
    (uint8) RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND,
    sizeof(msg),
    RR_SELECT_BCCH_OMSG,
    gas_id
  );
}

/*!
 * \brief Sends RR_SELECT_BCCH_IMSG_SI_NOTIFY_IND.
 *
 *  This message indicates that the SIs specified in notify_si_mask in the original request have been 
 *  acquired.
 *
 * \param bsic 
 * \param gas_id 
 */
void rr_select_bcch_send_omsg_si_notify_ind(
  ARFCN_T arfcn,
  BSIC_T bsic,
  const gas_id_t gas_id
)
{
  rr_select_bcch_omsg_si_notify_ind_t msg;

  msg.arfcn = arfcn;
  msg.bsic = bsic;

  rr_rr_send(
    &msg.header,
    (uint8) RR_SELECT_BCCH_OMSG_SI_NOTIFY_IND,
    sizeof(msg),
    RR_SELECT_BCCH_OMSG,
    gas_id
  );
}

/* EOF */

