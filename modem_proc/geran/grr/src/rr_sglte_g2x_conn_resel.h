#ifndef RR_SGLTE_G2X_CONN_RESEL_H
#define RR_SGLTE_G2X_CONN_RESEL_H

/** @file */

/*============================================================================
  @file rr_resel_g2w.h

  This is the header file for the main RR G2W Cell Reselection state machine.

                Copyright (c) 2009 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sglte_g2x_conn_resel.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL

#include "comdef.h"
#include "rr_task.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/
    typedef enum
    {
      RR_SGLTE_G2X_RESEL_CONN_STATUS_UNDEFINED,
    
      /* An SGLTE G2X connected mode reselection procedure completed successfully. */
      RR_SGLTE_G2X_RESEL_CONN_RESELECTION_SUCCESS,
    
      /* An SGLTE G2X connected mode reselection procedure has failed  */
      RR_SGLTE_G2X_RESEL_CONN_RESELECTION_FAILURE,

      RR_SGLTE_G2X_RESEL_CONN_RESELECTION_FAILURE_NO_RESPONSE,
      
      /* An SGLTE G2X connected mode reselection procedure has been aborted, RR/L1 are still camped (although MAC may be suspended). */
      RR_SGLTE_G2X_RESEL_CONN_RESELECTION_ABORTED,
        
      RR_SGLTE_G2X_RESEL_CONN_STATUS_MAX
    } rr_sglte_g2x_resel_conn_status_e;

  /* Output messages. */
  typedef enum
  {
    RR_SGLTE_G2X_RESEL_CONN_OMSG_ABORT_CNF,
    RR_SGLTE_G2X_RESEL_CONN_OMSG_COUNT
  } rr_sglte_g2x_resel_conn_omsg_e;


/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

 extern void rr_sglte_g2x_conn_send_omsg(
 rr_sglte_g2x_resel_conn_omsg_e omsg,  const gas_id_t gas_id) ;



/**
  @brief Initialises the module.
*/
extern void rr_sglte_rr_conn_g2x_resel_init(const gas_id_t gas_id);

/**
  @brief This is the main SGLTE RR G2XConnected Mode Cell Reselection state machine.
*/          
extern rr_event_T rr_sglte_rr_conn_g2x_resel_control(rr_event_T grr_control_event, rr_cmd_bdy_type *new_message, const gas_id_t gas_id );

/*!
  * \brief Indicates if the PLMN search should be inhibited when SGLTE G2X reselection
  *        feature is active.
  */
extern boolean rr_sglte_g2x_resel_conn_inhibit_plmn_search(const gas_id_t gas_id);
extern boolean rr_sglte_g2x_conn_mode_resel_can_process_plmn_search(const gas_id_t gas_id);

#endif /* FEATURE_SGLTE_G2X_CONN_MODE_RESEL */
#endif /* RR_SGLTE_G2X_CONN_RESEL_H */

/* EOF */

