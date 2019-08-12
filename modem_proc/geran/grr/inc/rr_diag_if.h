#ifndef RR_DIAG_IF_H
#define RR_DIAG_IF_H
/*============================================================================
 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/rr_diag_if.h#1 $$DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $

 @file rr_diag_if.h

 Inferace file for DIAG

 Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/**
  @brief Returns the cell id of the current cell

  Returns the cell id of the current cell. The cell id is 16 bits.

  @return void
*/
#if !defined(__GNUC__)
extern void rr_diag_get_cell_id(__packed uint16 *cell_id);
#else
extern void rr_diag_get_cell_id(uint16 *cell_id);
#endif /* ! __GNUC__ */
/**
  @brief Returns the LAI of the current cell

  Returns the LAI of the current cell. The LAI is 5 octets.

  @return void
*/
#if !defined(__GNUC__)
extern void rr_diag_get_lai(__packed uint8 *lai);
#else
extern void rr_diag_get_lai(uint8 *lai);
#endif /* ! __GNUC__ */

#endif /* #ifndef RR_DIAG_IF_H */

/* EOF */

