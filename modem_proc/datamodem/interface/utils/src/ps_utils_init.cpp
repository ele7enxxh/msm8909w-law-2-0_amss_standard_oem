/*==========================================================================*/
/*!
  @file
  ps_utils_init.c

  @brief
  This file provides functions that are used to perform initializations
  of utils module.

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_utils_init.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  11/30/11    su Adding log code support for DPL logging
  03/01/11   jee Fix to handle MMGSDI reg issue during startup
  11/08/10   mct Added AKAv2 DSS support
  10/26/10   jee Added AKAv2 feature
  2009-07-14 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ps_utils_init.h"
#include "ps_mem.h"
#include "ps_system_heap.h"
#include "ps_crit_sect.h"
#include "pstimer_config.h"
#include "ds_Utils_Signal.h"
#include "ds_Utils_MemManager.h"
#include "ds_Utils_SignalBus.h"
#include "ds_Utils_Atomic.h"
#include "ps_utils_aka.h"
#include "ds_auth_platform.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_CCritSect.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalFactory.h"
#include "ds_Utils_ISignalFactory.h"

extern "C" {
  #include "ps_utils.h"
}

#ifdef FEATURE_DATA_PS
#include "ps_logging_diag.h"

extern "C" {
#include "ps_logging_helper.h"
#include "ps_dpm_dpl.h"
}

#endif

/*===========================================================================
                         LOCAL DATA DECLARATIONS

===========================================================================*/

/*!
 * Create signal factory during power up to avoid race condition.
 */
static ISignalFactory *pSignalFactory = 0;

/*!
 * Defines the global PS critical section used by all modules.
 */
ps_crit_sect_type global_ps_crit_section;

/*!
  This critical section is used by Signal module to make its operations
  mutually exclusive
*/
ICritSect * pDSSigCritSect = 0;


/*===========================================================================

                         PUBLIC MEMBER FUNCTIONS

===========================================================================*/
void ps_utils_powerup
(
  void
)
{
  ds::ErrorType  retVal;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Initialize the PS system heap module.
  -------------------------------------------------------------------------*/
  ps_system_heap_init();

  /*------------------------------------------------------------------------
    Initialize the global PS critical section
  -------------------------------------------------------------------------*/
  PS_INIT_CRIT_SECTION(&global_ps_crit_section);

  /*------------------------------------------------------------------------
    Initialize the PS timer module.
  -------------------------------------------------------------------------*/
  ps_timer_init();

  /*------------------------------------------------------------------------
    Initialize the PS Mem module.
  -------------------------------------------------------------------------*/
  ps_mem_init();

  /*------------------------------------------------------------------------
    Initialize the utils memory pools.
  -------------------------------------------------------------------------*/
  ds::Utils::MemoryManager::MemPoolInit();

  /*------------------------------------------------------------------------
    Initialize the atomic operations module
  -------------------------------------------------------------------------*/
  ds_utils_atomic_Init();

  /*-------------------------------------------------------------------------
    Create global critical section to be used by Signal module
  -------------------------------------------------------------------------*/
  retVal = DS_Utils_CreateInstance(NULL,
                                   AEECLSID_CCritSect,
                                   (void **) &pDSSigCritSect);
  if (AEE_SUCCESS != retVal)
  {
    DATA_ERR_FATAL("Failed to create global signal crit sect");
  }

  /*------------------------------------------------------------------------
    Create signal factory during power up to avoid race condition.
  -------------------------------------------------------------------------*/
  retVal = DS_Utils_CreateInstance(NULL,
                                   AEECLSID_CSignalFactory,
                                   (void**)&pSignalFactory);
  if (AEE_SUCCESS != retVal)
  {
    LOG_MSG_ERROR_1("ps_utils_powerup(): "
                    "Can't create Signal Factory (%d)", retVal);
    ASSERT(0);
  }

} /* ps_utils_powerup() */

void ps_utils_powerdown
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    DeInitialize the PS timer module.
  -------------------------------------------------------------------------*/
  ps_timer_deinit();

  /*------------------------------------------------------------------------
    DeInitialize the PS Mem module.
  -------------------------------------------------------------------------*/
  ps_mem_deinit();

  /*------------------------------------------------------------------------
    DeInitialize the PS system heap module.
  -------------------------------------------------------------------------*/
  ps_system_heap_deinit();

  
  /*------------------------------------------------------------------------
    DeInitialize ds_Utils_Atomic module.
  -------------------------------------------------------------------------*/
  ds_utils_atomic_DeInit();

  /*------------------------------------------------------------------------
    Destroy global PS critical section
  -------------------------------------------------------------------------*/
  PS_DESTROY_CRIT_SECTION(&global_ps_crit_section);

  DS_UTILS_RELEASEIF(pDSSigCritSect);

  /*------------------------------------------------------------------------
    Destroy global signal factory
  -------------------------------------------------------------------------*/
  DS_UTILS_RELEASEIF(pSignalFactory);


} /* ps_utils_powerdown() */

void ps_utils_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Register a Signal dispatcher with DS_SIG task (when CS is not available)
  -------------------------------------------------------------------------*/
  (void) ds_sig_set_cmd_handler (DS_SIG_SIGNAL_DISPATCH_CMD,
                                 ds::Utils::Signal::SignalDispatcher);
  ds::Utils::SignalBus::RegisterCmdHandler();

  /*------------------------------------------------------------------------
    Register with MMGSDI
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI_SESSION_LIB
  ps_utils_aka_reg_mmgsdi_client_id();
#endif /* FEATURE_MMGSDI_SESSION_LIB */

  /*------------------------------------------------------------------------
    Write efs items to file
  -------------------------------------------------------------------------*/
  ps_efs_init();

  ds_auth_init();
} /* ps_utils_init() */

void ps_utils_deinit
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
} /* ps_utils_deinit() */
