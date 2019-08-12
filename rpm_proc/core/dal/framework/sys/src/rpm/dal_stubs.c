#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "DALSys.h"
#include "DDIIPCInt.h"
#include "DDIInterruptController.h"
#include "DDIHWIO.h"
#include "msmhwiobase.h"
#include "HALhwio.h"
#include "rpm_ipc.h"
#include "comdef.h"
#include "CoreVerify.h"

//      
// DEVICE FUNCTIONS     
//      
// DAL's driver "attach" and vtable paradigm is extremely memory intensive
// (~250 bytes to implement attach, ~500 bytes per driver of context and vtable
// data).  We do our best to support people's common code (which sometimes
// contains "attach" calls, etc.) but mostly we provide specialized
// implementations of their dependencies.
//
// The one exception to this (for now) is clock--they have a "real DAL" so we
// need to jump through a few hoops to support them.
//      
#include "DalDevice.h"
#include "DALFramework.h"


#include "busywait.h"
void DALSYS_BusyWait(uint32 pause_time_us)
{
  busywait(pause_time_us);
}


// SMD Lite required stubs
//
// SMD Lite's common code assumes the presence / proper behavior of the
// following functions:
//
DALResult DalInterruptController_RegisterISR(DalDeviceHandle * _h, DALInterruptID  intrID,
                                             const DALISR  isr, const DALISRCtx  ctx, uint32  IntrFlags)
{
  // Currently we map all the ISR's for a target through a static data table;
  // this lets us exploit the M3's hardware interrupt stacking and
  // simultaneously incur a minimum of code.
  //
  // In short, we don't have to do anything for this; the interrupts will be
  // set up elsewhere.
  return DAL_SUCCESS;
}

//DalIPCInt_Trigger(smd_intr_out_handle, intr->processor, intr->irq_out);
DALResult DalIPCInt_Trigger(DalDeviceHandle *_h, DalIPCIntProcessorType eTarget, DalIPCIntInterruptType eInterrupt)
{
  switch(eInterrupt)
  {
    case DALIPCINT_GP_HIGH:
      switch(eTarget)
      {
        // The Dime modem is a single core.
        case DALIPCINT_PROC_MCPU:
        case DALIPCINT_PROC_MDSPSW:
        case DALIPCINT_PROC_MDSPFW:
          HWIO_OUT(RPM_IPC, (1 << 12));
          return DAL_SUCCESS;

        case DALIPCINT_PROC_ACPU:
          HWIO_OUT(RPM_IPC, (1 << 4));
          busywait(1);
          HWIO_OUT(RPM_IPC, (1 << 4));
          return DAL_SUCCESS;

        case DALIPCINT_PROC_ADSP:
          HWIO_OUT(RPM_IPC, (1 << 8));
          return DAL_SUCCESS;

        // Riva is the old name for Pronto both of which are merely versions of
        // (WConnect).
        case DALIPCINT_PROC_RIVA:
          HWIO_OUT(RPM_IPC, (1 << 16));
          return DAL_SUCCESS;
      }
      break;

    default:
      break;
  }

  return DAL_ERROR;
}

DALResult DalHWIO_MapRegion(DalDeviceHandle *_h, const char *szBase, uint8 **ppAddress)
{
  // The RPM has access to all the MSM peripherals at their proper physical
  // addresses, so no mapping is actually required.  However, SMD code is
  // looking up an address using this function instead of hardcoding it, so
  // we'll provide it to them.
  if(!strncmp("CORE_TOP_CSR", szBase, 13))
  {
    *ppAddress = (uint8 *)CORE_TOP_CSR_BASE;
    return DAL_SUCCESS;
  }
  else if(!strncmp("PMIC_ARB", szBase, 8))
  {
    *ppAddress = (uint8 *)PMIC_ARB_BASE;
    return DAL_SUCCESS;
  }

  return DAL_ERROR;
}
