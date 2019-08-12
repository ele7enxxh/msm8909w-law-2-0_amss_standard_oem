#ifndef RF_MDSP_H
#define RF_MDSP_H

/*!
  @file
  rfcommon_core_mdsp.h

  @brief
  Common RF driver interface to MDSP registers.

  @details
  This file provides platform-agnostic read and write access functions to
  the modem firmware register interface.
*/

/*===========================================================================
Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.


                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_core_mdsp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/09   bmg     Added "C" externalization for C++
09/15/09   bmg     Initial version.

============================================================================*/

#include "rfa_variation.h"
#include "mdsp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
  @brief
  Write a value into a modem firware register.

  @details
  In on-target builds, by using the mdsp_write macro this RF interface
  abstraction insures the correct memory size access occurs depending
  on the pointer type of the address parameter.  If it is a 16-bit pointer,
  a half-word memory access will occur.  If it is a 32-bit pointer, a
  full word memory access will occur.

  Off-target builds are directed to the MDSP interface simulation environment.
*/
#ifdef T_WINNT
#error code not present
#else /* T_WINNT */
#define rf_mdsp_write(address, val) mdsp_write((address), (val))
#endif /* else T_WINNT */

/*!
  @brief
  Write a value into a modem firware array register.

  @details
  In on-target builds, by using the mdsp_write macro this RF interface
  abstraction insures the correct memory size access occurs depending
  on the pointer type of the address parameter.  If it is a 16-bit pointer,
  a half-word memory access will occur.  If it is a 32-bit pointer, a
  full word memory access will occur.

  Off-target builds are directed to the MDSP interface simulation environment.

  This function allows access to MDSP registers that are indexed by an offset.
*/
#ifdef T_WINNT
#error code not present
#else /* T_WINNT */
#define rf_mdsp_write_offset(address, offset, val) mdsp_write_offset((address), (val))
#endif /* else T_WINNT */

/*!
  @brief
  Read a value from a modem firmware register.

  @details
  In on-target builds, by using the mdsp_read macro this RF interface
  abstraction insures the correct memory size access occurs depending
  on the pointer type of the address parameter.  If it is a 16-bit pointer,
  a half-word memory access will occur.  If it is a 32-bit pointer, a
  full word memory access will occur.

  Off-target builds are directed to the MDSP interface simulation environment.
*/
#ifdef T_WINNT
#error code not present
#else /* T_WINNT */
#define rf_mdsp_read(address) mdsp_read((address))
#endif /* else T_WINNT */

#ifdef __cplusplus
}
#endif

#endif /* RF_MDSP_H */

