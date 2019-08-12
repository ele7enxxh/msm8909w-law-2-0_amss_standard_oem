#ifndef DDICLOCK_H
#define DDICLOCK_H
/*
===========================================================================
*/
/**
  @file DDIClock.h
  @brief Public interface include file for accessing the clock device driver.

  The DDIClock.h file is the public API interface to the clock
  Device Access Library (DAL).
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Clock_API_mainpage.dox file contains all file/group descriptions that 
      are in the output PDF generated using Doxygen and LaTeX. To edit or 
      update any of the file/group text in the PDF, edit the 
      Clock_API_mainpage.dox file or contact Tech Pubs.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/DDIClock.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 
  05/17/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "DALSys.h"
#include "com_dtypes.h" /* for "boolean" */
#include "ClockDefs.h"



/*=========================================================================
      Type Definitions
==========================================================================*/


/** @addtogroup clock_functions
@{ */

/**
  Defines the interface version.
 */
#define DALCLOCK_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/** @} */ /* end_addtogroup clock_functions */

/*=========================================================================
      Interface Definitions
==========================================================================*/

/** @cond 
*/

typedef struct DalClock DalClock;
struct DalClock
{
   struct DalDevice DalDevice;
   DALResult (*GetClockId)(DalDeviceHandle * _h,  const char * szClock,  ClockIdType * pnId);
   DALResult (*EnableClock)(DalDeviceHandle * _h,  ClockIdType  nClock);
   DALResult (*DisableClock)(DalDeviceHandle * _h,  ClockIdType  nClock);
   DALResult (*SetClockFrequency)(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32  nFreq,  ClockFrequencyType  eMatch,  uint32 * pnResultFreq);
   DALResult (*SetClockDivider)(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32  nDivider);
   DALResult (*InvertClock)(DalDeviceHandle * _h,  ClockIdType  nClock,  boolean  bInvert);
   DALResult (*ResetClock)(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockResetType  eReset);
   DALResult (*IsClockEnabled)(DalDeviceHandle * _h,  ClockIdType  nClock,  boolean * pbIsOn);
   DALResult (*IsClockOn)(DalDeviceHandle * _h,  ClockIdType  nClock,  boolean * pbIsOn);
   DALResult (*GetClockFrequency)(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 * pnFrequencyHz);
   DALResult (*SelectExternalSource)(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 nSource, uint32  nDivider,  uint32  nM,  uint32  nN,  uint32  n2D);
   DALResult (*GetPowerDomainId)(DalDeviceHandle * _h,  const char * szPowerDomain,  ClockPowerDomainIdType * pnPowerDomainId);
   DALResult (*EnablePowerDomain)(DalDeviceHandle * _h,  ClockPowerDomainIdType  nPowerDomainId);
   DALResult (*DisablePowerDomain)(DalDeviceHandle * _h,  ClockPowerDomainIdType  nPowerDomainId);
   DALResult (*EnableDCVS)(DalDeviceHandle * _h);
   DALResult (*EnableAVS)(DalDeviceHandle * _h);
   DALResult (*LoadNV)(DalDeviceHandle * _h);
   DALResult (*ProcessorSleep)(DalDeviceHandle * _h,  ClockSleepModeType  eMode,  uint32  nFlags);
   DALResult (*ProcessorRestore)(DalDeviceHandle * _h,  ClockSleepModeType  eMode,  uint32  nFlags);
   DALResult (*CalcClockFrequency)(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 * pnFrequencyHz);
   DALResult (*ConfigClock)(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockConfigType  eConfig);
   DALResult (*SetUARTBaudClock)(DalDeviceHandle * _h,  ClockIdType  nClock, uint32 nDI, uint32 nFI, uint32 * pnFrequencyHz);

};

typedef struct DalClockHandle DalClockHandle;
struct DalClockHandle
{
   uint32 dwDalHandleId;
   const DalClock * pVtbl;
   void * pClientCtxt;
};

#define DAL_ClockDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALCLOCK_INTERFACE_VERSION,hDalDevice)

/** @endcond */


/*=========================================================================
      Interface Declarations
==========================================================================*/

/** @addtogroup clock_functions
@{ */

/* =========================================================================
**  Function : DalClock_GetClockId
** =========================================================================*/
/**
  Looks up a clock ID for a specified clock name.

  This function takes the name of a clock and returns a handle that
  can be used for making requests on that clock. Usually, this function 
  must be the first to be invoked prior to using any other clock functions.
  The string name of the clock can be found in the software frequency plan, 
  e.g., cc_uart2_clk.

  @param _h        [in]  Handle to the DAL device.
  @param szClock   [in]  String name of the clock.
  @param pnId      [out] Pointer to the clock ID to be filled in.

  @return
  DAL_SUCCESS -- Clock was found and the ID was filled in. \n
  DAL_ERROR -- Clock was not found on this target.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_GetClockId(DalDeviceHandle * _h,  const char * szClock,  ClockIdType * pnId)
{
   return ((DalClockHandle *)_h)->pVtbl->GetClockId( _h, szClock, pnId);
}


/* =========================================================================
**  Function : DalClock_EnableClock
** =========================================================================*/
/**
  Enables a clock.

  This function enables a specified clock. It does not reference count
  recursive calls, but does perform tracking between multiple clients. 

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock to enable.

  @return
  DAL_SUCCESS -- Clock was enabled. \n
  DAL_ERROR -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_EnableClock(DalDeviceHandle * _h,  ClockIdType  nClock)
{
   return ((DalClockHandle *)_h)->pVtbl->EnableClock( _h, nClock);
}


/* =========================================================================
**  Function : DalClock_DisableClock
** =========================================================================*/
/**
  Disables a clock.

  This function removes the client's vote to enable a specified clock.
  There is no support for reference counting recursive calls; however, the
  driver will not disable the clock if a different client has made an
  enable request.

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock to disable.

  @return
  DAL_SUCCESS -- Call was successful (although the clock may still be enabled). \n
  DAL_ERROR -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_DisableClock(DalDeviceHandle * _h,  ClockIdType  nClock)
{
   return ((DalClockHandle *)_h)->pVtbl->DisableClock( _h, nClock);
}


/* =========================================================================
**  Function : DalClock_SetClockFrequency
** =========================================================================*/
/**
  Sets the frequency of a clock.

  This function requests a frequency change for a specified clock. The
  driver selects the exact frequency based on the matching argument,
  which allows requesting a minimum or approximate frequency. There 
  is no support for concurrent requests; the last request will be serviced 
  and override any other minimum frequency requests. 

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock for which to set the frequency.
  @param nFreq   [in] Clock frequency in Hz, kHz, or MHz, depending on the
                      eMatch parameter.
  @param eMatch  [in] Type of match request.
  @param pnResultFreq [out] The programmed clock frequency in the same
                            units as nFreq; can be NULL if the resulting 
                            frequency does not need to be checked. Note that 
                            the result is truncated to the units requested.

  @return
  DAL_SUCCESS -- Requested clock frequency was found and programmed. \n
  DAL_ERROR -- Clock identifier was invalid, or a matching frequency 
               was not found.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_SetClockFrequency(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32  nFreq,  ClockFrequencyType  eMatch,  uint32 * pnResultFreq)
{
   return ((DalClockHandle *)_h)->pVtbl->SetClockFrequency( _h, nClock, nFreq, eMatch, pnResultFreq);
}


/* =========================================================================
**  Function : DalClock_SetClockDivider
** =========================================================================*/
/**
  Sets the divider for a clock.

  This function programs the divider for a specified clock. This
  functionality only applies to a small subset of clocks that have
  independent dividers off the main domain.

  @param _h       [in] Handle to the DAL device.
  @param nClock   [in] Clock for which to set the divider.
  @param nDivider [in] Clock divider in logical units; 0 is invalid.

  @return
  DAL_SUCCESS -- Requested clock ID was valid. Divider value may or may not
  be correct. \n
  DAL_ERROR -- Clock identifier was invalid, or divider was 0.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_SetClockDivider(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32  nDivider)
{
   return ((DalClockHandle *)_h)->pVtbl->SetClockDivider( _h, nClock, nDivider);
}


/* =========================================================================
**  Function : DalClock_SetUARTBaudClock
** =========================================================================*/
/**
  Sets UART baud clock frequency.

  This function computes and sets UART clock frequency based on the input
  parameters.  The function should only be called to configure UART or GSBI
  UART clock frequency.  DalClock_SetClockFrequency should be called for
  other common clocks.
 
  @param _h            [in]  Handle to the DAL device.
  @param nClock        [in]  Clock to set.
  @param nDI           [in]  UIM baud rate adjustment factor
  @param nFI           [in]  UIM clock rate conversion factor
  @param pnFrequencyHz [out] Pointer to uint32 to fill in with the frequency.
                             A value of zero indicates that the frequency 
                             could not be determined.

  @return
  DAL_SUCCESS -- Clock frequency was calculated successfully. \n
  DAL_ERROR   -- Clock identifier was invalid, or frequency calculation is not
                 available on this target.

  @sideeffects
  During the calculation, the clock is enabled.

  @dependencies
  The clock being tested must be enabled or controllable from the current
  processor.

  @sa
  DalClock_SetClockFrequency
*/

static __inline DALResult
DalClock_SetUARTBaudClock(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 nDI, uint32 nFI, uint32 *pnFrequencyHz)
{
   return ((DalClockHandle *)_h)->pVtbl->SetUARTBaudClock( _h, nClock, nDI, nFI, pnFrequencyHz);
}


/* =========================================================================
**  Function : DalClock_InvertClock
** =========================================================================*/
/**
  Sets the Inversion state for a clock.

  This function allows inverting or uninverting a clock. This is rarely
  used. It is generally used just to overcome timing problems in the hardware.

  @param _h       [in] Handle to the DAL device.
  @param nClock   [in] Clock for which to set the Inversion state.
  @param bInvert  [in] Whether to invert the clock or not.

  @return
  DAL_SUCCESS -- Clock inversion was successfully applied. \n
  DAL_ERROR -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_InvertClock(DalDeviceHandle * _h,  ClockIdType  nClock,  boolean  bInvert)
{
  return ((DalClockHandle *)_h)->pVtbl->InvertClock( _h, nClock, bInvert);
}


/* =========================================================================
**  Function : DalClock_ResetClock
** =========================================================================*/
/**
  Controls the reset assertion for a clock domain.

  This function allows resetting the hardware in a specified clock domain. The
  resetting is destructive to the hardware and requires reconfiguration
  after the reset is deasserted.

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock for which to set the Reset state.
  @param eReset  [in] Type of reset: assert, deassert, or pulse.

  @return
  DAL_SUCCESS -- Clock reset was successfully applied. \n
  DAL_ERROR -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_ResetClock(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockResetType  eReset)
{
   return ((DalClockHandle *)_h)->pVtbl->ResetClock( _h, nClock, eReset);
}


/* =========================================================================
**  Function : DalClock_IsClockEnabled
** =========================================================================*/
/**
  Checks the enabled state of a clock.

  This function checks whether a specified clock is enabled or disabled. Note
  that this is not necessarily equivalent to the clock being active. A
  clock may be disabled by the local processor, but enabled by another one.
  Or if the hardware is misconfigured the clock may be enabled, but not
  actively toggling.

  @param _h      [in] Handle to the DAL device.
  @param nClock  [in] Clock to check.

  @return
  TRUE -- Clock is enabled. \n
  FALSE -- Clock is not enabled, or identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId(), DalClock_IsClockOn()
*/

static __inline boolean DalClock_IsClockEnabled
(
  DalDeviceHandle * _h,
  ClockIdType  nClock
)
{
  boolean bIsEnabled = FALSE;
  (void) ((DalClockHandle *)_h)->pVtbl->IsClockEnabled( _h, nClock, &bIsEnabled);
  return bIsEnabled;
}


/* =========================================================================
**  Function : DalClock_IsClockOn
** =========================================================================*/
/**
  Checks whether a clock is on.

  If the clock is on, it means the clock is not just enabled, but is 
  actually toggling. 

  @param _h        [in] Handle to the DAL device.
  @param nClock    [in] Clock to check.

  @return
  TRUE -- Clock is on. \n
  FALSE -- Clock is not on, or identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId(), DalClock_IsClockEnabled()
*/

static __inline boolean DalClock_IsClockOn
(
  DalDeviceHandle * _h,
  ClockIdType  nClock
)
{
  boolean bIsOn = FALSE;
  (void)((DalClockHandle *)_h)->pVtbl->IsClockOn( _h, nClock, &bIsOn);
  return bIsOn;
}


/* =========================================================================
**  Function : DalClock_GetClockFrequency
** =========================================================================*/
/**
  Gets the frequency of a clock.

  This function retrieves the current frequency of a clock in Hz. This
  is independent of whether or not the clock is running. Note that only clocks
  controlled on the local processor can have the frequency retrieved.

  @param _h            [in]  Handle to the DAL device.
  @param nClock        [in]  Clock to check.
  @param pnFrequencyHz [out] Pointer to uint32 to fill in with the frequency.
                             A value of zero indicates that the frequency 
                             could not be determined.

  @return
  DAL_SUCCESS -- Clock was valid. The frequency may or may not have been 
                 retrieved (see function description). \n
  DAL_ERROR -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  DalClock_GetClockId()
*/

static __inline DALResult
DalClock_GetClockFrequency(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 * pnFrequencyHz)
{
   return ((DalClockHandle *)_h)->pVtbl->GetClockFrequency( _h, nClock, pnFrequencyHz);
}


/* =========================================================================
**  Function : DalClock_SelectExternalSource
** =========================================================================*/
/**
  Selects an external source for a clock.

  This function selects an external source for a clock and configures
  the clock generator dividers. This is only necessary for clock domains
  that can be derived from a source outside the clock block such that
  DalClock_SetClockFrequency() cannot be used.
  Note that because the clock frequency is not known to the clock driver any
  voltage scaling requirements must be managed by the caller.

  @param _h        [in] Handle to the DAL device.
  @param nClock    [in] Clock to configure.
  @param nSource   [in] Source to use.  Pass the following values: \n
                        - 0 -- Selects the first (or only) external source. \n
                        - 1 -- Selects the second external source. \n
                        - 2 -- Etc. \n
                        @note1 If the specified clock does not support the 
                        requested external source, this request selects the first 
                        source (in most cases XO) available to the clock.
  @param nDivider  [in] Integer divider to use.
  @param nM        [in] The M value for any M/N counter (0 to bypass). 
                        Not all clock domains have M/N counters; these 
                        values will be ignored in such cases.
  @param nN        [in] The N value for any M/N counter.
  @param n2D       [in] Twice the D value for any M/N counter. The raw value 
                        is doubled to allow for half-integer duty cycles.

  @return
  DAL_SUCCESS -- Clock was valid, and external source was selected. \n
  DAL_ERROR -- Clock identifier was invalid or does not support any external
    source.

  @dependencies
  Any voltage requirements for the clock must be handled externally.

  @sa
  DalClock_SetClockFrequency()
*/

static __inline DALResult
DalClock_SelectExternalSource(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 nSource, uint32  nDivider,  uint32  nM,  uint32  nN,  uint32  n2D)
{
   return ((DalClockHandle *)_h)->pVtbl->SelectExternalSource( _h, nClock, nSource, nDivider, nM, nN, n2D);
}


/* =========================================================================
**  Function : DalClock_ConfigClock
** =========================================================================*/
/**
  Configures a clock.

  This function configures a clock based on predefined configuration modes.

  @param _h        [in] Handle to the DAL device.
  @param nClock    [in] Clock to check.
  @param eConfig   [in] Clock configuration.

  @return
  DAL_SUCCESS -- Call was successful.\n
  DAL_ERROR -- Clock identifier was invalid or the passed configuration is not
                 supported/invalid.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_ConfigClock(DalDeviceHandle * _h,  ClockIdType  nClock,  ClockConfigType  eConfig)
{
  return ((DalClockHandle *)_h)->pVtbl->ConfigClock( _h, nClock, eConfig);
}

/** @} */ /* end_addtogroup clock_functions */

/** @addtogroup power_domain_functions
@{ */

/* =========================================================================
**  Function : DalClock_GetPowerDomainId
** =========================================================================*/
/**
  Gets the power domain identifier.

  This function looks up and returns the identifier for a specified power
  domain. This identifier is used for the power domain control functions.
  The string used to get the identifier can be found in the software frequency 
  plan, e.g., VDD_GRP.

  @param _h                [in]  Handle to the DAL device.
  @param szPowerDomain     [in]  Power domain name.
  @param pnPowerDomainId   [out] Pointer to the ID to be filled in. 

  @return
  DAL_SUCCESS -- Power domain was valid, and the ID was filled in. \n
  DAL_ERROR -- Power domain was invalid.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_GetPowerDomainId(DalDeviceHandle * _h,  const char * szPowerDomain,  ClockPowerDomainIdType * pnPowerDomainId)
{
   return ((DalClockHandle *)_h)->pVtbl->GetPowerDomainId( _h, szPowerDomain, pnPowerDomainId);
}


/* =========================================================================
**  Function : DalClock_EnablePowerDomain
** =========================================================================*/
/**
  Enables a power domain.

  @param _h               [in] Handle to the DAL device.
  @param nPowerDomainId   [in] Power domain to enable.

  @return
  DAL_SUCCESS -- Power domain was enabled. \n
  DAL_ERROR -- Power domain identifier was invalid.

  @dependencies
  None.

  @sideeffects
  Any clocks inside the power domain are enabled during the enable
  sequence. The state is restored afterward.

  @sa
  DalClock_GetPowerDomainId(), DalClock_DisablePowerDomain()
*/

static __inline DALResult
DalClock_EnablePowerDomain(DalDeviceHandle * _h,  ClockPowerDomainIdType  nPowerDomainId)
{
  return ((DalClockHandle *)_h)->pVtbl->EnablePowerDomain( _h, nPowerDomainId);
}


/* =========================================================================
**  Function : DalClock_DisablePowerDomain
** =========================================================================*/
/**
  Disables a power domain.

  @param _h               [in] Handle to the DAL device.
  @param nPowerDomainId   [in] Power domain to disable.

  @return
  DAL_SUCCESS -- Power domain was disabled. \n
  DAL_ERROR -- Power domain identifier was invalid.

  @dependencies
  Any bus interfaces for this power domain must be disabled prior to
  collapsing the core or the bus may lock up.

  @sideeffects
  Any clocks inside the power domain are enabled during the collapse
  sequence.

  @sa
  DalClock_GetPowerDomainId(), DalClock_EnablePowerDomain()
*/

static __inline DALResult
DalClock_DisablePowerDomain(DalDeviceHandle * _h,  ClockPowerDomainIdType  nPowerDomainId)
{
  return ((DalClockHandle *)_h)->pVtbl->DisablePowerDomain( _h, nPowerDomainId);
}

/** @} */ /* end_addtogroup power_domain_functions */

/** @addtogroup misc_functions
@{ */

/* =========================================================================
**  Function : DalClock_EnableDCVS
** =========================================================================*/
/**
  Enables the dynamic clock and voltage scaling.

  This function enables the DCVS processing for system resources such as 
  CPU, buses, and memories. The DCVS driver must invoke this function when 
  all system resource requirements have been configured.

  @param _h       [in] Handle to the DAL device.

  @return
  Always returns DAL_SUCCESS.

  @dependencies
  All system resource requests must be set prior to enabling the DCVS.
*/

static __inline DALResult
DalClock_EnableDCVS(DalDeviceHandle * _h)
{
   return ((DalClockHandle *)_h)->pVtbl->EnableDCVS( _h);
}


/* =========================================================================
**  Function : DalClock_EnableAVS
** =========================================================================*/
/**
  Controls the Adaptive Voltage Scaling state.

  This function enables the AVS hardware block for the current processor. 
  Only the current processor's AVS is enabled via this function. In a 
  multicore system, each processor must invoke the function.

  @param _h       [in] Handle to the DAL device.

  @return
  DAL_SUCCESS -- AVS control succeeded. \n
  DAL_ERROR -- AVS control failed.

  @dependencies
  None.
*/

static __inline DALResult
DalClock_EnableAVS(DalDeviceHandle * _h)
{
   return ((DalClockHandle *)_h)->pVtbl->EnableAVS( _h);
}


/* =========================================================================
**  Function : DalClock_LoadNV
** =========================================================================*/
/**
  Loads the clock driver nonvolatile data.

  This function triggers the driver to load configuration information from
  NV storage. It must be invoked once this storage is ready to be used.

  @param _h       [in] Handle to the DAL device.

  @return
  Always returns DAL_SUCCESS.

  @dependencies
  NV must be initialized.
*/

static __inline DALResult
DalClock_LoadNV(DalDeviceHandle * _h)
{
  return ((DalClockHandle *)_h)->pVtbl->LoadNV( _h);
}


/* =========================================================================
**  Function : DalClock_ProcessorSleep
** =========================================================================*/
/**
  Puts the processor in the Sleep mode.

  This function configures the processor clocks for Sleep mode   
  and, depending on the environment, may execute the appropriate 
  Stand by and Wait For Interrupt (SWFI) instruction.

  @param _h        [in] Handle to the DAL device.
  @param eMode     [in] Type of sleep to enter.
  @param nFlags    [in] Mode-specific flags.

  @return
  Always returns DAL_SUCCESS.

  @dependencies
  None.

  @sa
  DalClock_ProcessorRestore()
*/

static __inline DALResult
DalClock_ProcessorSleep(DalDeviceHandle * _h,  ClockSleepModeType  eMode,  uint32  nFlags)
{
  return ((DalClockHandle *)_h)->pVtbl->ProcessorSleep( _h, eMode, nFlags);
}


/* =========================================================================
**  Function : DalClock_ProcessorRestore
** =========================================================================*/
/**
  Restores the processor clock state after sleep.

  This function restores the processor clocks after a power collapse.

  @param _h        [in] Handle to the DAL device.
  @param eMode     [in] Type of sleep to leave; usually 
                        DALCLOCK_SLEEP_MODE_POWER_COLLAPSE().
  @param nFlags    [in] Mode-specific flags; usually 0.

  @return
  Always returns DAL_SUCCESS.

  @dependencies
  DalClock_ProcessorSleep() must have been called previously.

  @sa
  DalClock_ProcessorSleep()
*/

static __inline DALResult
DalClock_ProcessorRestore(DalDeviceHandle * _h,  ClockSleepModeType  eMode,  uint32  nFlags)
{
   return ((DalClockHandle *)_h)->pVtbl->ProcessorRestore( _h, eMode, nFlags);
}

/** @} */ /* end_addtogroup misc_functions */

/** @addtogroup test_functions
@{ */

/* =========================================================================
**  Function : DalClock_CalcClockFrequency
** =========================================================================*/
/**
  Calculates the frequency of a clock.

  This function calculates the current frequency of a clock in Hz using
  hardware counting circuits. This functionality  depends on the presence of 
  this hardware support, as well as being available on the current processor.
  It also requires that the clock can be enabled, which must be possible from
  the current environment.
  
  The frequency calculation is accurate to within a few kHz.
  The calculation takes a long time (at least a few milliseconds) and
  is intended only for testing purposes, not any real modes of operation.

  @param _h            [in]  Handle to the DAL device.
  @param nClock        [in]  Clock to check.
  @param pnFrequencyHz [out] Pointer to uint32 to fill in with the frequency.
                             A value of zero indicates that the frequency 
                             could not be determined.

  @return
  DAL_SUCCESS -- Clock frequency was calculated successfully. \n
  DAL_ERROR -- Clock identifier was invalid, or frequency calculation is not
               available on this target.

  @sideeffects
  During the calculation, the clock is enabled.

  @dependencies
  The clock being tested must be enabled or controllable from the current
  processor.

  @sa
  DalClock_GetClockFrequency()
*/

static __inline DALResult
DalClock_CalcClockFrequency(DalDeviceHandle * _h,  ClockIdType  nClock,  uint32 * pnFrequencyHz)
{
   return ((DalClockHandle *)_h)->pVtbl->CalcClockFrequency( _h, nClock, pnFrequencyHz);
}

/** @} */ /* end_addtogroup test_functions */

/** @addtogroup boot_functions
@{ */

/* =========================================================================
**  Function : Clock_Init
** =========================================================================*/
/**
  Performs clock initialization in limited environments.

  This function is invoked in certain images that do not support the
  full DAL clock driver. It is primarily used in early boot loader
  stages. This function is not actually a DAL function and does not require 
  the DAL framework.

  @return
  TRUE -- Clock_Init was successful. \n
  FALSE -- Critical failure; boot should not proceed.

  @dependencies
  Varies based on the image under consideration; usually, there are 
  no dependencies.
*/

boolean Clock_Init (void);

/** @} */ /* end_addtogroup boot_functions */


#endif /* !DDICLOCK_H */

