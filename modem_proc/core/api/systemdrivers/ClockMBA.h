#ifndef __CLOCKMBA_H__
#define __CLOCKMBA_H__
/*
  ===========================================================================
*/
/**
  @file ClockMBA.h
  @brief Public interface include file for the MBA clock driver.
  ===========================================================================

*/

/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/ClockMBA.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/12   frv     Added deinitialize and remove non-used functions.
03/29/12   vt      Initial
  ===========================================================================
*/


/*=========================================================================
      Interface Declarations
==========================================================================*/


/** @addtogroup clock_mba_functions
@{ */


/* =========================================================================
**  Function : ClockMBA_Initialize
** =========================================================================*/
/**
  Initialize the MBA clock driver.

  @param
  None
 
  @return
  TRUE -- Initialization was successful.
  FALSE -- Initialization failed.

  @dependencies
  The MSS QTimer must be initialized and the function mba_wait_microseconds()
  is readily available for use by this driver.
*/
boolean ClockMBA_Init(void);


/* =========================================================================
**  Function : ClockMBA_DeInit
** =========================================================================*/
/**
  Deinitialize the MBA clock driver.

  @param
  None
 
  @return
  TRUE -- Deinitialization was successful.
  FALSE -- Deinitialization failed.

  @dependencies
  The MSS QTimer must be initialized and the function mba_wait_microseconds()
  is readily available for use by this driver.
*/
boolean ClockMBA_DeInit(void);


/** @} */ /* end_addtogroup clock_mba_functions */


#endif /* __CLOCKMBA_H__ */

