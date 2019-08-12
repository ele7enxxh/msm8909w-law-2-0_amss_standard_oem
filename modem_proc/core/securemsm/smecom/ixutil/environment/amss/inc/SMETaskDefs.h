#ifndef __SMETASKDEFS_H__
#define __SMETASKDEFS_H__

/*===========================================================================

                           S M E  T A S K  D E F S

                             H E A D E R  F I L E

FILE:  SMETaskDefs.h


GENERAL DESCRIPTION

  Definitions related to task start-up and and initialization

PUBLIC CLASSES AND STATIC FUNCTIONS
  
INITIALIZATION & SEQUENCING REQUIREMENTS
  
  Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixutil/environment/amss/inc/SMETaskDefs.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  when     who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/09    gr     Created

===========================================================================*/


/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/

#include "comdef.h"

/*
** DUAL_PD is defined for dual PD builds.
** SEC_PD is defined on all single-PD builds, and also on dual PD builds
** if this piece of code is being built as part of the SEC PD.
** AMSS_PD is defined on all single PD builds, and also on dual PD builds
** if this piece of code is being built as part of the AMSS PD.
** MINIMAL_DUAL_PD is defined on builds where two PDs exist, but there is
** minimal functionality in the second PD.
*/

  #define THIS_PD_NAME    "SEC PD"
  #define OTHER_PD_NAME   "AMSS PD"
  #define THIS_PD_ID      5
    #define THIS_PD_IPC_SVC  IPCSVC_IXCONTENT
    #define OTHER_PD_IPC_SVC IPCSVC_IXCONTENT_AMSS


#if defined(FEATURE_IXFILESVC)
  #define FEATURE_IX_FILESVC_TASK
#endif

#if !defined(IMAGE_MODEM_PROC) && defined(FEATURE_IXHTTPSVC)
  #define FEATURE_IX_HTTPSVC_TASK
#endif

#if defined(FEATURE_ICONTENT) && \
    !defined(IMAGE_MODEM_PROC)
  #define FEATURE_IX_CONTENTCLNT_TASK
#endif

#if defined(FEATURE_IXBCASTCONTENTSVC) && \
   !defined(IMAGE_MODEM_PROC)
  #define FEATURE_IX_BCASTCONTENTCLNT_TASK
  #define FEATURE_IX_BCASTKEYTPORTSVC_TASK
#endif

#if defined(FEATURE_IXBCASTCONTENTSVC) && \
   !defined(IMAGE_APPS_PROC)
  #define FEATURE_IX_BCASTCONTENTSVC_TASK
#endif

#if defined(FEATURE_IXDESCRAMBLERSVC) && \
   !defined(IMAGE_APPS_PROC)
  #define FEATURE_IXDESCRAMBLERSVC_TASK
#endif

#if defined(FEATURE_SEC)
  #define FEATURE_SEC_TASK
#endif

#if defined(FEATURE_SEC_ARM_CRYPTO)
  #define FEATURE_SEC_ARM_CRYPTO_TASK
#endif

#if defined(FEATURE_SECRND) && \
    !defined(IMAGE_APPS_PROC)
  #define FEATURE_SECRND_TASK
#endif

#if defined(FEATURE_SEC_USFS) && \
   !defined(IMAGE_APPS_PROC)
  #define FEATURE_SEC_USFS_TASK
#endif

#if defined(FEATURE_SEC_CONTENT_AGENT) && \
   !defined(IMAGE_MODEM_PROC)
  #define FEATURE_SEC_CONTENT_AGENT_TASK
#endif

#if defined(FEATURE_SEC) && defined(FEATURE_IPC_SECTEST) 
  #define FEATURE_IPC_SECTEST_TASK
#endif

/* Enumeration of AMSS functions that need to be accessed from the
** SEC PD.
**
** NOTE: Please make sure that the symbols in the enumeration below are in
** alphabetical order. This is to avoid mismatching functions and their
** addresses in the table of exported functions.
*/
typedef enum
{
  CLK_PAUSE_E,
  DIAG_FS_REGISTER_ACCESS_CHECK_E,
  DOG_MONITOR_PAUSE_E,
  DOG_MONITOR_RESUME_E,
  DOG_REPORT_E,
  FLASH_NAND_DEVICE_PROBE_E,
  QDSP_CURRENT_DMA_TABLE_E,
  AMSS_MAX_FUNC_E
} exportedAMSSFuncE;

/* Enumeration of SEC functions that need to be accessed from the
** AMSS PD.
*/
typedef enum
{
  SEC_HEAP_PTR_E,
  SEC_HEAP_BUF_PTR_E,
  SEC_MAX_FUNC_E
} exportedSECFuncE;

#endif /* __SMETASKDEFS_H__ */

