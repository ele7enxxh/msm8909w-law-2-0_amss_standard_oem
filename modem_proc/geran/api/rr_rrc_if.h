#ifndef RR_RRC_IF_H
#define RR_RRC_IF_H

/*============================================================================
  @file rr_rrc_if.h

  This module exposes various APIs for WCDMA RRC to use when sending messages
  to GSM RR and accessing various

                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_rrc_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif


#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "rrc_rr_types.h"
#include "rrcrrif.h"
#include "sys.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/**
  @brief Interface function used by RRC to send messages to RR.

  This function may be used by RRC to put a message on RR's message queue.

  @param message_ptr: Pointer to the RRC message to be sent to RR.
  @param rrc_cmd_id:  Message Id
  @param length:      Message Length.
*/
void rr_send_rrc_message_to_rr(
  void *            message_ptr,
  rrc_rr_cmd_e_type rrc_cmd_id,
  word              length
);


/**
  @brief Get the band for a GSM ARFCN given the band indicator value provided.

  This function is used to determine the band that the specified ARFCN is a
  member of, using the provided band indicator to resolve any conflict if the
  ARFCN is in the DCS 1800/PCS 1900 overlap region.

  @param arfcn:    The ARFCN that the caller requires the band of.
  @param band_ind: Band indicator value, as received OTA in UMTS.  This
                   parameter is unused if the ARFCN is not in DCS 1800/PCS
                   1900 band.
  @return:         The band that ARFCN is in, or INVALID_BAND if it is in a
                   band that is not currently supported by RR.
*/
sys_band_T rr_rrc_get_arfcn_band(
  uint16                       arfcn,
  rrc_rr_frequence_band_e_type band_ind);

/* equivalent version that includes a subscription ID */
sys_band_T rr_rrc_get_arfcn_band_sub_id(
  uint16                       arfcn,
  rrc_rr_frequence_band_e_type band_ind,
  sys_modem_as_id_e_type       as_id);


/**
  @brief Interface used by RRC to update RR's band_pref.

  This function should be called whenever RRC receives a new band_pref from MM.

  @param band_pref: The band_pref for RR to set.
*/
void rr_rrc_set_band_pref(sys_band_mask_type band_pref);
/* equivalent version that includes a subscription ID */
void rr_rrc_set_band_pref_sub_id(sys_band_mask_type     band_pref,
                                 sys_modem_as_id_e_type as_id);

/**
  @brief Interface used by RRC to update RR's BST band_pref.

  This function should be called whenever RRC receives a new BST band_pref from MM.

  @param band_pref: The band_pref for RR to set.
*/
void rr_rrc_set_bst_band_pref(sys_band_mask_type band_pref);
/* equivalent version that includes a subscription ID */
void rr_rrc_set_bst_band_pref_sub_id(sys_band_mask_type     band_pref,
                                     sys_modem_as_id_e_type as_id);

/**
  @brief Function for RRC to get information on channels on which RR has
         acquired GSM cells successfully.

  RRC will then use this information to avoid searching for WCDMA service on
  these channels.
*/
extern void rr_rrc_get_acq_chnl_info(
  rr_rrc_gsm_cell_list_type *gsm_acq_chnl_info);
/* equivalent version that includes a subscription ID */
extern void rr_rrc_get_acq_chnl_info_sub_id(
  rr_rrc_gsm_cell_list_type *gsm_acq_chnl_info,
  sys_modem_as_id_e_type     as_id);


/**
  @brief Function for RRC to clear the RR acquired channel database.

  RRC will call this function when RRC camps successfully, and when RRC
  receives an RRC_SERVICE_REQ with UOOS_USE_TMR = TRUE.
*/
extern void rr_rrc_clear_acq_chnl_info(void);
/* equivalent version that includes a subscription ID */
extern void rr_rrc_clear_acq_chnl_info_sub_id(sys_modem_as_id_e_type as_id);

/**
  @brief Interface used by RRC to get GSM camped PLMN id.

  This function may be used to access the PLMN of the cell that RR is currently
  camped on.

  @return: PLMN ID of the cell that RR is camped on, or (0xFF 0xFF 0xFF) is
           returned if RR is not camped.
*/
extern PLMN_id_T  rrc_get_rr_curr_camped_plmn(void);
/* equivalent version that includes a subscription ID */
extern PLMN_id_T  rrc_get_rr_curr_camped_plmn_sub_id(sys_modem_as_id_e_type as_id);

/**
  @brief Save RR's acquisition database to NV.

  This function must be called by RRC at power-off to write RR's acquisition
  database to NV.
*/
extern void rr_rrc_acq_db_save(void);
/* equivalent version that includes a subscription ID */
extern void rr_rrc_acq_db_save_sub_id(sys_modem_as_id_e_type as_id);

#endif /* #ifndef RR_RRC_IF_H */
