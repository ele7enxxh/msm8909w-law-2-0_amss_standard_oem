#ifndef RR_FTM_IF_H
#define RR_FTM_IF_H

/*============================================================================
  @file rr_ftm_if.h

  Contains functions exported to FTM.


                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_ftm_if.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "rr_l1.h"

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
 * Computes the BSIC value from a cell using the SCH block information
 * See GSM05.02 3.3.2.2 & GSM 03.03 4.3.2
 *
 * @param BSIC_result_ptr [out] Set to the computed BSIC value
 *
 * @param SCH_block       SCH block from which the BSIC is computed
 */
extern void rr_compute_BSIC_from_SCH_block(
  BSIC_T  *BSIC_result_ptr,
  byte    *SCH_block
);

#endif /* RR_FTM_IF_H */
