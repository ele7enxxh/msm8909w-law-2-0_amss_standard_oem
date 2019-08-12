#ifndef __NAND_LOG_H__
#define __NAND_LOG_H__

/*==================================================================
 *
 * FILE:        hostdl_log.h
 *
 * SERVICES:    Logging for NAND client via T32 DCC
 *
 * DESCRIPTION: Logging for NAND client via T32 DCC
 *
 *        Copyright © 2008-2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *            QUALCOMM Proprietary/GTDR
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/hostdl_log.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 10/29/08     mm      Clean up
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

/* -----------------------------------------------------------------------
** Include files for debug print output
** ----------------------------------------------------------------------- */

extern int jprintf(const char *fmt,...);
extern void term_put(char);

/* -----------------------------------------------------------------------
** Macros for debug print output
** ----------------------------------------------------------------------- */
/* When compiled with TNOISY, these statements will print depending
 * on level of TNOISY.  Only changeable at compile time
 */
#ifdef TNOISY
  #define FLASHLOG(l, x) if (TNOISY >= l) {jprintf x;}
#else
  #define FLASHLOG(l, x) /**/
#endif /* NOISY */

#endif /* __NAND_LOG_H__ */
