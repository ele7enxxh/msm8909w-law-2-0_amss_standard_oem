#ifndef SNS_SAM_PM_H
#define SNS_SAM_PM_H

/*============================================================================
  @file sns_sam_pm.h

  @brief
  Data structures and constants used only by the SAM Framework.

  @note
  File line length should generally be limited to <= 80 columns.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

#include "sns_common.h"
#include "sns_debug_str.h"
#include "sns_em.h"
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_pm.h"
#include "sns_sam_algo_api.h"

/*=============================================================================
  MACROS
=============================================================================*/

#ifdef SNS_USES_ISLAND
#define SNS_SAM_UIMAGE_CODE __attribute__((section (".text.uSAM")))
#define SNS_SAM_UIMAGE_DATA __attribute__((section (".data.uSAM")))
#define SNS_SAM_UIMAGE_RODATA __attribute__((section (".rodata.uSAM")))
#else
#define SNS_SAM_UIMAGE_CODE
#define SNS_SAM_UIMAGE_DATA
#define SNS_SAM_UIMAGE_RODATA
#endif /* USES_ISLAND */

#define SNS_SAM_ALGOINST_IN_UIMAGE(algoInst) \
( SNS_OS_IS_UHEAP_PTR(algoInst) && SNS_SAM_ALGOINST_UIMAGE == algoInst->imageMode )

/* Maximum number of preallocated algorithm input and output data blocks*/
#define SNS_SAM_ALGO_MAX_IO 5

// When SAM queue count exceeds this threshold, SAM votes for higher MIPS - Turbo
#define SNS_SAM_Q_UPPER_THRESH  8    // PEND: Set the upper threshold after profiling
// When SAM queue count goes below this threshold, SAM votes for lower MIPS - Normal
#define SNS_SAM_Q_LOWER_THRESH  2    // PEND: Set the lower threshold after profiling

/*=============================================================================
  TYPE DEFINITIONS
=============================================================================*/


typedef enum
{
/**
*  The algorithm code is statically placed in DDR, or it was determined that
*  this algorithm should not (for some reason) run in uImage
*/
  SNS_SAM_ALGOINST_BIGIMAGE,

/**
* This algorithm instance can run in uImage mode indefinitely
*/
  SNS_SAM_ALGOINST_UIMAGE,

/**
* This algorithm instance may be moved into uImage when TCM memory becomes
* available.
*/
  SNS_SAM_ALGOINST_NOMEM_UIMAGE

} sns_sam_algoinst_uimage_mode;

/**
* SAM's internal representation of the current CPU speed
* Default is SNS_SAM_MIPS_NORMAL
*/
typedef enum
{
  SNS_SAM_MIPS_IDLE,
  SNS_SAM_MIPS_NORMAL,
  SNS_SAM_MIPS_TURBO,
  SNS_SAM_MIPS_MAX
}sns_sam_internal_mips_type;

/*
*  Numeric value of the MIPS rates used by SAM. SAM will always attempt to be at
*  the lowest possible MIPS rate.
*/
static const uint32_t sns_sam_mips_table[ SNS_SAM_MIPS_MAX ] = { 0, 20, 200 };

/*=============================================================================
  PUBLIC FUNCTIONS
 ============================================================================*/

/**
*  Exits the uImage mode. This function can be called at any part of the
*  SAM framework.
**
*/
void sns_sam_uimage_exit( void );

/**
*  This function call is used to unblock SAM from entering uImage mode.
*/
void sns_sam_uimage_vote_enter( void );

/**
*  Vote for higher/lower MIPS
*
*  @param[i] newMips  Vote for Normal or Turbo MIPS
*/
void sns_sam_pm_vote_mips( sns_sam_internal_mips_type newMips );

/**
*  Adjust the SAM MIPS vote based on queue size
*/
void sns_sam_pm_adjust_mips( void );


/**
*  To be called during SAM initializations. This function is responsible
*  for initializing the data structures to be maintained by SAM for uImage.
*
* @return SAM_EFAILED  If operation failed
*         SAM_ENONE    If operation was successful
*/
sns_sam_err sns_sam_pm_init( void );

/**
 * Determine whether the AP is in suspend.  Used when determining whether to
 * send indications, and at what rate to send batched indications.
 *
 * @return true if AP in suspend, false otherwise
 */
bool sns_sam_pm_ap_suspend( void );

/**
 * Handle an event from SNS PM where the AP processor state transitions between
 * suspend and awake.
 */
void sns_sam_handle_pm_event( void );

/**
 * broadcast AP wake up to all other PM clients
 * used only when SAM determines it will wake up AP, i.e send QMI msg to AP
 */
void sns_sam_pm_ap_wakeup_broadcast( void );
#endif /* SNS_SAM_PM_H*/
