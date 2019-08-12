#ifndef RR_NV_G_H
#define RR_NV_G_H

/*! \file rr_nv_g.h
 
  This is the external header file for rr_nv.c.
  This module contains access functions for NV data.
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/rr_nv_g.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
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

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*!
 * \brief Called from GCOMMON to post GERAN_NV_REFRESH_IND to GRR queue.
 *  
 * \param gas_id - GAS ID
 */
extern void rr_nv_send_nv_refresh_ind(const gas_id_t gas_id);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*!
 * \brief Indicates if eng mode debug is enabled.
 *        
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_eng_mode_debug_is_enabled(const sys_modem_as_id_e_type as_id);

#endif /* #ifndef RR_NV_G_H */

/* EOF */

