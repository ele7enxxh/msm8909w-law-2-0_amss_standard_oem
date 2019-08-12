/*
===========================================================================

FILE:         HALspmmmap.c

DESCRIPTION:
  This is the hardware abstraction layer for the SPM block which defines
  the memory map function to obtain base address for RIVA_SAW2.

===========================================================================

             Copyright © 2012-2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================

$Header: //components/rel/core.mpss/3.5.c12.3/power/spm/src/hal/HALspmmmap.c#1 $

=============================================================================*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "HALspmPlatform.h"
#include "DALDeviceId.h"
#include "DDIHWIO.h"
#include "msmhwiobase.h"

/* ==================================================================
                          MACRO DEFINITIONS
   ================================================================== */

#ifdef MSS_TOP_BASE
#define SPM_BASE_NAME "MSS_TOP"
#elif defined MODEM_TOP_BASE
#define SPM_BASE_NAME "MODEM_TOP"
#else
#error "Modem Base Address is not defined"
#endif

/* ==================================================================
                     GLOBAL DATA DECLARATIONS
   ================================================================== */
/**
 * Generating base address to send to HWIO
 */
uint8 *SPM_BASE_PTR = NULL;

/* -----------------------------------------------------------------------
**                         FUNCTIONS
** ----------------------------------------------------------------------- */

/*
 *
 * @brief spm_hwio_mmap
 *
*/
void spm_hwio_mmap(void)
{
  static DalDeviceHandle *spmDALHandle;
  DALResult eResult;

  eResult = DAL_DeviceAttach(DALDEVICEID_HWIO, &spmDALHandle);
  if (eResult == DAL_SUCCESS)
  {
    DalHWIO_MapRegion(spmDALHandle, SPM_BASE_NAME, &SPM_BASE_PTR);
  }
}
