#ifndef __FLASH_LOG_H__
#define __FLASH_LOG_H__

/*=======================================================================
* FILE:        flash_log.h
*
* SERVICES:    Routines to support flash logging
*
* DESCRIPTION :
*
* Copyright (c) 2010 Qualcomm Technologies Incorporated.
* All Rights Reserved.
* QUALCOMM Confidential and Proprietary
*=======================================================================*/

/*===================================================================
*
*                       EDIT HISTORY FOR FILE
*
*   This section contains comments describing changes made to the
*   module. Notice that changes are listed in reverse chronological
*   order.
*
*   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/nor/flash_log.h#1 $ 
*   $DateTime: 2015/09/01 00:30:35 $ 
*   $Author: pwbldsvc $
*
* when         who     what, where, why
* ----------   ---     ----------------------------------------------
* 09/10/10     sv      Initial version
*==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

/* Include in <stdio.h> when compiling for off target tool */
#ifdef BUILD_FOR_X86
  #include <stdio.h>
  #include <stdlib.h>
#endif

extern int jprintf(const char *fmt,...);

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


#endif /* __FLASH_LOG_H__ */
