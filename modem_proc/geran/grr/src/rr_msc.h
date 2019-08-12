#ifndef RR_MSC_H
#define RR_MSC_H

/*!
 * \file rr_msc.h 
 *  
 * This module contains functionality to manage the advertised multi-slot class. 
 *  
 *              Copyright (c) 2014 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_msc.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "comdef.h"
#include "geran_multi_sim.h"
#include "rr_l1.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

typedef enum
{
  RR_MSC_CLIENT_THERMAL,      // Thermal Mitigation
#ifdef FEATURE_DUAL_SIM
  RR_MSC_CLIENT_L1_COEX,      // L1 coexistence management
#endif
  RR_MSC_CLIENT_MAX_COUNT     // Number of instances of modified MSC data
} rr_msc_client_e_t;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Used to set a specific number of TX slots.
 * 
 * \param msc_client (in)
 * \param tx_slots (in)
 * \param rx_slots (in)
 * \param as_id (in)
 * 
 * \return boolean - TRUE if the MSC has changed, FALSE otherwise
 */
extern boolean rr_msc_set_tx_rx_slots(
  rr_msc_client_e_t msc_client,
  uint8 tx_slots,
  uint8 rx_slots,
  sys_modem_as_id_e_type as_id
);

/*!
 * \brief Used to reduce the number of TX slots.
 * 
 * \param msc_client (in)
 * \param as_id (in)
 * 
 * \return boolean - TRUE if the MSC has changed, FALSE otherwise
 */
extern boolean rr_msc_reduce_tx_slots(rr_msc_client_e_t msc_client, sys_modem_as_id_e_type as_id);

/*!
 * \brief Used to revert the MSC data to the original NV values.
 * 
 * \param msc_client (in)
 * \param as_id (in)
 * 
 * \return boolean - TRUE if the MSC has changed, FALSE otherwise
 */
extern boolean rr_msc_revert_to_orig(rr_msc_client_e_t msc_client, sys_modem_as_id_e_type as_id);

/*!
 * \brief Called when the NV data becomes available.
 * 
 * \param as_id (in)
 */
extern void rr_msc_nv_ready(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the MSC values.
 * 
 * \param egprs_multislot_class_ptr (out)
 * \param dtm_multislot_class_ptr (out)
 * \param gprs_multislot_class_ptr (out)
 * \param as_id 
 */
extern void rr_msc_get_multislot_class(
  uint8 *gprs_multislot_class,
  uint8 *egprs_multislot_class,
#ifdef FEATURE_GSM_DTM
  uint8 *dtm_multislot_subclass,
#endif
  sys_modem_as_id_e_type as_id
);

/*!
 * \brief Converts DTM multislot subclass value to a DTM multislot class value.
 * 
 * \param dtm_multislot_subclass (in)
 * 
 * \return uint8 - DTM multislot class value
 */
extern uint8 rr_msc_conv_dtm_multislot_subclass_to_class(uint8 dtm_multislot_subclass);

/*!
 * \brief Converts DTM multislot class value to a DTM multislot subclass value.
 * 
 * \param dtm_multislot_class (in)
 * 
 * \return uint8 - DTM multislot subclass value
 */
extern uint8 rr_msc_conv_dtm_multislot_class_to_subclass(uint8 dtm_multislot_class);

/*!
 * \brief Process function for MPH_COEX_MSC_CHANGE_IND
 * 
 * \param msg_ptr (in)
 */
extern void rr_msc_process_l1_coex_msc_change_ind(mph_coex_msc_change_ind_T *msg_ptr, const gas_id_t gas_id);

/*!
 * \brief Updates MM with new multi-slot class parameters (if required). 
 *        Note: Also calls into the RR-THERMAL module which will update the service capability (if required). 
 * 
 * \param can_send_service_ind 
 * \param gas_id 
 */
extern void rr_msc_update(boolean can_send_service_ind, const gas_id_t gas_id);

/*!
 * \brief Sends RR_IMSG_THERMAL_MITIGATION_REQ.
 * 
 * \param as_id (in) Note: Not GAS-ID
 */
extern void rr_msc_send_rr_imsg_msc_change_ind(sys_modem_as_id_e_type as_id);

/*!
 * \brief Initialisation function called once at RR task start.
 */
extern void rr_msc_task_start_init(void);

#endif /* #ifndef RR_MSC_H */

