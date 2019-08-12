/*===========================================================================
   Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/modem_sec/src/modem_rollback_hw_mutex.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/05/12   rs      Initial version.
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <HALhwio.h>

#include "modem_hw_mutex.h"
#include "modem_hwio.h"
#include "msmhwiobase.h"

void fuse_grab_mutex()
{
  do
  {
    /* Attempt to write Processor ID */
    HWIO_OUTI( TCSR_MUTEX_REG_n, MODEM_HW_MUTEX_ID, MODEM_HW_MUTEX_PID );

    /* Check if PID was written (i.e. the hardware mutex is locked) */
  } while( HWIO_INI( TCSR_MUTEX_REG_n, MODEM_HW_MUTEX_ID ) != MODEM_HW_MUTEX_PID );
}

void fuse_release_mutex()
{
  /* Unlock hardware mutex by writing 0*/
  HWIO_OUTI( TCSR_MUTEX_REG_n, MODEM_HW_MUTEX_ID, 0 );
}
