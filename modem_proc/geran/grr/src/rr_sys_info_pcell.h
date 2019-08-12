#ifndef RR_SYS_INFO_PCELL_H
#define RR_SYS_INFO_PCELL_H

/*! \file rr_sys_info_pcell.h 
 
  This is the header file for the rr_sys_info_pcell.c module.
 
                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sys_info_pcell.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sys_type.h"
#include "rr_gprs_defs.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the pending cell system information.
 * 
 * \param gas_id 
 * 
 * \return gprs_scell_info_t* 
 */
extern gprs_scell_info_t *rr_gprs_get_pcell_info(
  const gas_id_t gas_id
);

/*!
 * \brief The pcell is populated with the system information from a ncell.
 * 
 * \param arfcn 
 * \param gas_id 
 */
extern void rr_ncell_ccch_to_pcell(
  ARFCN_T arfcn,
  const gas_id_t gas_id
);

/*!
 * \brief The pending cell (pcell) becomes the serving cell (scell), and the camped_on_flag is set TRUE.
 * 
 * \param gas_id 
 * 
 * \return gprs_scell_info_t* 
 */
extern gprs_scell_info_t *rr_pcell_becomes_scell(
  const gas_id_t gas_id
);

/*!
 * \brief Checks the BCCH ARFCN supplied against the BCCH ARFCN of the pcell.
 * 
 * \param bcch_arfcn 
 * \param gas_id 
 * 
 * \return boolean 
 */
extern boolean rr_is_this_the_pcell(
  ARFCN_T bcch_arfcn,
  const gas_id_t gas_id
);

#endif /* #ifndef RR_SYS_INFO_PCELL_H */

/* EOF */

