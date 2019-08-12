#ifndef RR_CE_PAGING_H
#define RR_CE_PAGING_H

/*! \file rr_ps_access.h
 
  This is the header file for rr_ce_paging.c.
  This module contains functionality related to CS & PS paging and OTA Page Mode.
 
                Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_ce_paging.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_l1.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

// Maximum size in bytes for a mobile station identity
#define RR_MAX_IDENTITY_SIZE 10

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  RR_PAGE_DOMAIN_NONE,
  RR_PAGE_DOMAIN_CS,
  RR_PAGE_DOMAIN_PS
} rr_page_domain_e;

typedef enum
{
  RR_IDENTITY_TYPE_NONE,
  RR_IDENTITY_TYPE_IMEI,
  RR_IDENTITY_TYPE_IMEISV,
  RR_IDENTITY_TYPE_IMSI,
  RR_IDENTITY_TYPE_TMSI,
  RR_IDENTITY_TYPE_PTMSI
} rr_identity_type_e;

typedef struct
{
  rr_page_domain_e domain;
  rr_identity_type_e identity_type;
  byte identity[RR_MAX_IDENTITY_SIZE];
  byte channel_needed;
} rr_page_info_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Checks to see if the received mobile identity is the same as the P-TMSI.
 * 
 * \param IE - byte array containing P-TMSI IE from OTA message
 * \param gas_id 
 * 
 * \return boolean - TRUE if there is a match; FALSE otherwise
 */
extern boolean rr_ce_compare_ota_ptmsi(
  byte *IE,
  const gas_id_t gas_id
);

/*!
 * \brief Checks to see if the received mobile identity is the same as the IMEI/IMSI/TMSI/P-TMSI.
 * 
 * \param IE - byte array containing Mobile Identity IE from OTA message
 * \param decoding_ok - updated as FALSE if an error occurred; TRUE otherwise
 * \param paging_id_type - updated to show which paging identity type matched
 * \param gas_id 
 * 
 * \return boolean - TRUE if there is a match; FALSE otherwise
 */
extern boolean rr_ce_compare_ota_mobile_identity(
  byte *IE,
  boolean *decoding_ok,
  rr_identity_type_e *paging_id_type,
  const gas_id_t gas_id
);

/*!
 * \brief Processes a Paging Request message received from the network.
 * 
 * \param message_type - the message type
 * \param own_paging_block - set to TRUE if the OTA message was received in the MS's own paging block
 * \param gas_id 
 */
extern void rr_ce_process_page_request(
  byte message_type,
  boolean own_paging_block,
  const gas_id_t gas_id
);

/*!
 * \brief Maps the channel_needed value received in paging request messages to an internal value. 
 *  
 *  See 3GPP 44.018 10.5.2.8 (ChannelNeeded IE) 
 * 
 * \param paging_channel_needed_value - value as received in the OTA message 
 * 
 * \return byte - value to be used internally
 */
extern byte rr_ce_map_paging_request_channel_needed(byte paging_channel_needed_value);

/*!
 * \brief Saves a page so that the indication to NAS can be delayed.
 *  
 *  CS and PS pages are saved separately. Saving an additional page of the same type will overwrite one
 *  already saved.
 *  Saved pages can be sent to NAS by calling rr_ce_send_saved_pages_to_nas().
 * 
 * \param page_info_ptr - details of the page 
 * \param gas_id 
 */
extern void rr_ce_save_page(rr_page_info_t *page_info_ptr, const gas_id_t gas_id);

/*!
 * \brief Discards any saved pages.
 * 
 * \param gas_id 
 */
extern void rr_ce_discard_saved_pages(const gas_id_t gas_id);

/*!
 * \brief Sends any saved pages to NAS, in the order CS page then PS page
 * 
 * \param gas_id 
 */
extern void rr_ce_send_saved_pages_to_nas(const gas_id_t gas_id);

/*!
 * \brief Sends a RR_PAGING_IND message to NAS. 
 *  
 *  If a CS page is indicated, the RR-CONN-ESTABLISH module is also updated with the page details in
 *  preparation for a page response.
 * 
 * \param page_info_ptr 
 * \param gas_id 
 */
extern void rr_ce_send_page_to_nas(rr_page_info_t *page_info_ptr, const gas_id_t gas_id);

/*!
 * \brief Checks if the broadcast Page Mode has been changed. RR_IMSG_PAGE_MODE_IND is output if so. 
 *  
 *  The Page Mode is included in various OTA messages, e.g. Paging Request, Immediate Assignment, etc.
 *  See 3GPP 44.018 3.3.2
 * 
 * \param own_paging_block - Indicates if the OTA message was received in the MS's own paging block. Only 
 *                           messages received in the MS's own paging block should be check for Page Mode
 *                           changes
 * \param ota_page_mode - The Page Mode value received in the OTA message
 * \param gas_id 
 */
extern void rr_ce_check_for_page_mode_update(
  boolean own_paging_block,
  byte ota_page_mode,
  const gas_id_t gas_id
);

/*!
 * \brief Returns the OTA Page Mode.
 * 
 * \param gas_id 
 * 
 * \return paging_mode_T 
 */
extern paging_mode_T rr_ce_get_ota_page_mode(const gas_id_t gas_id);

/*!
 * \brief Sets the OTA Page Mode. 
 *  
 *  This will cause a comparison with the broadcast OTA Page Mode next time it is received, resulting in an
 *  update to L1 if it is different.
 * 
 * \param new_page_mode 
 * \param gas_id 
 */
extern void rr_ce_set_ota_page_mode(paging_mode_T new_page_mode, const gas_id_t gas_id);

/*!
 * \brief Compares the OTA Page Mode with 'compare_page_mode' and if equal, changes the Page Mode to 
 *        'new_page_mode'. 
 * 
 * \param compare_page_mode - Page Mode to compare the OTA Page Mode to
 * \param new_page_mode - The new Page Mode to be used if the compared Page Mode values are equal
 * \param gas_id 
 * 
 * \return boolean 
 */
extern boolean rr_ce_compare_and_update_l1_page_mode(
  paging_mode_T compare_page_mode,
  paging_mode_T new_page_mode,
  const gas_id_t gas_id
);

/*!
 * \brief Updates the L1 Page Mode to 'new_page_mode'.
 *  
 *  'old_page_mode' is also supplied to enable detaction of a transition away from Paging Reorganisation.
 *  The system information acquisition handler is also informed of the change of Page Mode. This is because
 *  if the Page Mode is Paging Reorganisation, all BCCH should be read.
 *  
 * \param old_page_mode - The previous Page Mode 
 * \param new_page_mode - The new Page Mode
 * \param gas_id 
 */
extern void rr_ce_process_page_mode_change(
  paging_mode_T old_page_mode,
  paging_mode_T new_page_mode,
  const gas_id_t gas_id
);

/*!
 * \brief Initialisation function.
 * 
 * \param gas_id 
 */
extern void rr_ce_paging_init(
  const gas_id_t gas_id
);

/*!
 * \brief Initialisation function. Called once at task start.
 */
extern void rr_ce_paging_task_start_init(
  void
);

#endif /* #ifndef RR_CE_PAGING_H */

/* EOF */

