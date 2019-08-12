#ifndef TZBSP_JTAG_H
#define TZBSP_JTAG_H

/**
@file tzbsp_jtag.h
@brief Provides API for JTAG printing.
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_jtag.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
08/11/12   tk       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Prints a single character to JTAG term.view. The printed character will not
 * go through under two conditions:
 *      1. JTAG debugging is disabled by fuse configuration.
 *      2. JTAG output port timed out (term.view window not open).
 *
 * After the time out condition happens, JTAG printing will be skipped. To
 * prevent the time out condition, term.view must be open in the T32 debugger
 * during device boot up.
 *
 * @param [in]  ch      Character to be printed.
 */
void tzbsp_jtag_put(const char ch);

#endif /* TZBSP_JTAG_H */
