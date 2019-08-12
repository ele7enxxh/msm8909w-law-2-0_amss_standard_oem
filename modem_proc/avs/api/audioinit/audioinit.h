#ifndef __AUDIOINIT_H__
#define __AUDIOINIT_H__

/*===========================================================================

                    A U D I O   I N I T   S E R V I C E S

DESCRIPTION
  This header file defines APIs for initializing all audio-related modules.

REFERENCES
  None.

Copyright (C) 2008 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/audioinit/audioinit.h#1 $
$DateTime: 2016/12/13 07:59:59 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/08   ymc     Initial revision.

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "mmdefs.h"

/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

typedef enum audioinit_callindex_enum_t
{
  /* public */   AUDIOINIT_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */   AUDIOINIT_CALLINDEX_ENUM_INIT_LIBS_BEFORE_TASKS_START  = 1,
                   /**<
                    * Inform to initialize libraries before tasks start.
                    *
                    * \par Params:
                    *   None.
                    *
                    * \retval APR_EOK
                    *   On success.
                    *
                    * \pre
                    *   TBD
                    *
                    * \post
                    *   TBD
                    *
                    * \remarks
                    *   Primative drivers are initialized. PMEM is accessible.
                    */
  /* public */   AUDIOINIT_CALLINDEX_ENUM_INIT_LIBS_BEFORE_SERVICES_START  = 2,
                   /**<
                    * Inform to initialize libraries before service tasks
                    * start.
                    *
                    * \par Params:
                    *   None.
                    *
                    * \retval APR_EOK
                    *   On success.
                    *
                    * \pre
                    *   TBD
                    *
                    * \post
                    *   TBD
                    *
                    * \remarks
                    *   Tasks have been defined but have not yet been started.
                    */
  /* public */   AUDIOINIT_CALLINDEX_ENUM_INIT_LIBS_AFTER_SERVICES_STARTED = 3,
                   /**<
                    * Inform to initialize libraries after service tasks have
                    * started.
                    *
                    * \par Params:
                    *   None.
                    *
                    * \retval APR_EOK
                    *   On success.
                    *
                    * \pre
                    *   TBD
                    *
                    * \post
                    *   TBD
                    *
                    * \remarks
                    *   SMD task is running.
                    */
  /* public */   AUDIOINIT_CALLINDEX_ENUM_INIT_LIBS_AFTER_TASKS_STARTED = 4,
                   /**<
                    * Inform to initialize libraries after all pre-defined
                    * tasks have started.
                    *
                    * \par Params:
                    *   None.
                    *
                    * \retval APR_EOK
                    *   On success.
                    *
                    * \pre
                    *   TBD
                    *
                    * \post
                    *   TBD
                    *
                    * \remarks
                    *   Applications are running.
                    */
  /* private  */ AUDIOINIT_CALLINDEX_ENUM_PREDEINIT = 5,
                   /**<
                    * Inform to pre-deinitialize libraries. This signals tasks
                    * to wake up and begin shut down.
                    *
                    * \par Params:
                    *   None.
                    *
                    * \retval APR_EOK
                    *   On success.
                    *
                    * \pre
                    *   TBD
                    *
                    * \post
                    *   TBD
                    *
                    * \remarks
                    *   TBD
                    */
  /* private  */ AUDIOINIT_CALLINDEX_ENUM_DEINIT = 6,
                   /**<
                    * Inform to deinitialize libraries. This waits for shut
                    * down to complete.
                    *
                    * \par Params:
                    *   None.
                    *
                    * \retval APR_EOK
                    *   On success.
                    *
                    * \pre
                    *   TBD
                    *
                    * \post
                    *   TBD
                    *
                    * \remarks
                    *   TBD
                    */
  /* public */   AUDIOINIT_CALLINDEX_ENUM_INVALID
} audioinit_callindex_enum_t;

/**
 * This is the audioinit function call entry-point.
 *
 * \param[in] index The function call to execute.
 * \param[in] params The parameters of the call.
 * \param[in] size The parameter size.
 *
 * \retval APR_EOK
 *   On success.
 *
 * \pre
 *   TBD
 *
 * \post
 *   TBD
 *
 * \remarks
 *   TBD
 */
int32_t audioinit_call (
  audioinit_callindex_enum_t index,
  void* params,
  uint32_t size
);

/**
 * This is the audioinit task entry-point.
 *
 * \param[in] params The parameters supplied by TMC.
 *
 * \pre
 *   TBD
 *
 * \post
 *   TBD
 *
 * \remarks
 *   This API is intended to be called only once by the TMC during boot-up
 *   initialization.
 */
void audioinit_task (
  dword params
);

#endif  /* !__AUDIOINIT_H__ */

