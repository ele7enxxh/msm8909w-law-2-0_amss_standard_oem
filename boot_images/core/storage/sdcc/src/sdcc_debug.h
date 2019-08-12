#ifndef __SDCC_DEBUG_H
#define __SDCC_DEBUG_H
/***************************************************************************
* sdcc_debug.h
*
* SDCC(Secure Digital Card Controller) driver debug function header file
*
* This file implements the logging facility for the SDCC driver
*
* Copyright (c) 2004-2010 Qualcomm Technologies Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
****************************************************************************/

/*==========================================================================
                        Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_debug.h#1 $
$DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when          who     what, where, why
--------      ---     ----------------------------------------------------------
2010-04-29    rh      Adjust mapping how DPRINTF map to RETAILMSG
2010-03-02    yg      Added #include "sdcc_api.h" 
2009-05-20    sc      Consolidated physical partition info support to get device info
2009-02-10    vj      OS Abstraction Layer
2009-01-30    rh      Clean up debugging function interface
2007-06-13    dng     Changed the TNOISY level from 3 to 1
2006-08-16    hwu     Merged in support for WINCE.
2005-08-28    hwu     General cleanup. 
2004-07-14    hwu     Added QXDM logging support.
===========================================================================*/

#include SDCC_OSAL_H

#if defined(FEATURE_SDCC_FOR_BOOT)
#undef DPRINTF
#undef TPRINTF
#define DPRINTF(x) 
#define TPRINTF(level, x)
#else /* !FEATURE_SDCC_FOR_BOOT */
#include "qw.h"
#include "sdcc_api.h"

#undef DPRINTF
#undef TPRINTF

#define MMC_MSG_LOGGING
#define SDCC_MSG_LEVEL 1

#if defined(MMC_MSG_LOGGING)
extern  void msg_printf(const char *fmt, ...);
#define DPRINTF(x)    msg_printf x
#define TPRINTF(level, x) 
#else
#define DPRINTF(x) 
#define TPRINTF(level, x)
#endif /* MMC_MSG_LOGGING */

#if defined (FEATURE_SDCC_FOR_WINCE)
#if !defined (DEBUG)
#undef  DPRINTF
#define DPRINTF(x)
#else
#undef  DPRINTF
#define DPRINTF(x)  RETAILMSG(1, x)
#endif /* !DEBUG */
#endif /* FEATURE_SDCC_FOR_WINCE */
#endif /* FEATURE_SDCC_FOR_BOOT */

#if !defined (FEATURE_SDCC_FOR_WINCE)
#define TEXT(x)      x
#endif /* FEATURE_SDCC_FOR_WINCE */

/******************************************************************************
* Name: sdcc_display_mem_device_info
*
* Description:
*    This function simply displays the memory device information on QXDM.
*
* Arguments:
*    handle         [IN] : a pointer to the SDCC device that was returned from
*                          sdcc_handle_open()
*    sdcc_pmem_info [IN] : pointer to the structure that contains the memory
*                          card specific information
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_display_mem_device_info( struct sdcc_device *handle,
                                   const sdcc_mem_info_type *sdcc_pmem_info );

#endif /*ifndef __SDCC_DEBUG_H */

