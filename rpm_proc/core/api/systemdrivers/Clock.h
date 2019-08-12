#ifndef CLOCK_H
#define CLOCK_H
/*
===========================================================================
*/
/**
  @file Clock.h
  @brief Public interface include file for accessing the clock driver.

  The Clock.h file is the public API interface to the clock driver.
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

  Copyright (c) 2012 QUALCOMM Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/Clock.h#3 $
  $DateTime: 2015/07/02 03:40:56 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 
  04/11/12   vph     Initial version
  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDefs.h"
#include "DALSys.h"

/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Interface Declarations
==========================================================================*/

/** @addtogroup clock_functions
@{ */

/* =========================================================================
**  Function : Clock_GetClockId
** =========================================================================*/
/**
  Looks up a clock ID for a specified clock name.

  This function takes the name of a clock and returns a handle that
  can be used for making requests on that clock. Usually, this function 
  must be the first to be invoked prior to using any other clock functions.
  The string name of the clock can be found in the software frequency plan, 
  e.g., cc_uart2_clk.

  @param szClock   [in]  String name of the clock.
  @param pnId      [out] Pointer to the clock ID to be filled in.

  @return
  DAL_SUCCESS -- Clock was found and the ID was filled in.
  DAL_ERROR -- Clock was not found on this target.

  @dependencies
  None.
*/

DALResult Clock_GetClockId( const char * szClock,  ClockIdType * pnId);


/* =========================================================================
**  Function : Clock_EnableClock
** =========================================================================*/
/**
  Enables a clock.

  This function enables a specified clock. It does not reference count
  recursive calls, but does perform tracking between multiple clients. 

  @param nClock  [in] Clock to enable.

  @return
  DAL_SUCCESS -- Clock was enabled. \n
  DAL_ERROR -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  Clock_GetClockId()
*/

DALResult Clock_EnableClock( ClockIdType  nClock);


/* =========================================================================
**  Function : Clock_DisableClock
** =========================================================================*/
/**
  Disables a clock.

  This function removes the client's vote to enable a specified clock.
  There is no support for reference counting recursive calls; however, the
  driver will not disable the clock if a different client has made an
  enable request.

  @param nClock  [in] Clock to disable.

  @return
  DAL_SUCCESS -- Call was successful (although the clock may still be enabled). \n
  DAL_ERROR -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  Clock_GetClockId()
*/

DALResult Clock_DisableClock( ClockIdType  nClock);


/* =========================================================================
**  Function : Clock_IsClockEnabled
** =========================================================================*/
/**
  Checks the enabled state of a clock.

  This function checks whether a specified clock is enabled or disabled. Note
  that this is not necessarily equivalent to the clock being active. A
  clock may be disabled by the local processor, but enabled by another one.
  Or if the hardware is misconfigured the clock may be enabled, but not
  actively toggling.

  @param nClock  [in] Clock to check.

  @return
  TRUE -- Clock is enabled. \n
  FALSE -- Clock is not enabled, or identifier was invalid.

  @dependencies
  None.

  @sa
  Clock_GetClockId(), Clock_IsClockOn()
*/

DALResult Clock_IsClockEnabled(  ClockIdType  nClock,  boolean * pbIsOn);


/* =========================================================================
**  Function : Clock_IsClockOn
** =========================================================================*/
/**
  Checks whether a clock is on.

  If the clock is on, it means the clock is not just enabled, but is 
  actually toggling. 

  @param nClock    [in] Clock to check.

  @return
  TRUE -- Clock is on. \n
  FALSE -- Clock is not on, or identifier was invalid.

  @dependencies
  None.

  @sa
  Clock_GetClockId(), Clock_IsClockEnabled()
*/

DALResult Clock_IsClockOn(  ClockIdType  nClock,  boolean * pbIsOn);

/* =========================================================================
**  Function : Clock_SetClockFrequency
** =========================================================================*/
/**
  Sets the frequency of a clock.

  This function requests a frequency change for a specified clock. The
  driver selects the exact frequency based on the matching argument,
  which allows requesting a minimum or approximate frequency. There 
  is no support for concurrent requests; the last request will be serviced 
  and override any other minimum frequency requests. 

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
  Clock_GetClockId()
*/

DALResult Clock_SetClockFrequency(ClockIdType nClock, uint32 nFreq, ClockFrequencyType eMatch, uint32 *pnResultFreq);

/* =========================================================================
**  Function : Clock_ConfigClock
** =========================================================================*/
/**
  Configures a clock.

  This function configures a clock based on predefined configuration modes.

  @param nClock    [in] Clock to check.
  @param eConfig   [in] Clock configuration.

  @return
  DAL_SUCCESS -- Call was successful.\n
  DAL_ERROR -- Clock identifier was invalid or the passed configuration is not
                 supported/invalid.

  @dependencies
  None.
*/

DALResult Clock_ConfigClock( ClockIdType nClock, ClockConfigType eConfig);


/* =========================================================================
**  Function : Clock_GetClockFrequency
** =========================================================================*/
/**
  Gets the frequency of a clock.

  This function retrieves the current frequency of a clock in Hz. This
  is independent of whether or not the clock is running. Note that only clocks
  controlled on the local processor can have the frequency retrieved.

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
  Clock_GetClockId()
*/

DALResult Clock_GetClockFrequency( ClockIdType nClock, uint32 *pnFrequencyHz );


/* =========================================================================
**  Function : Clock_SetDirectBusSpeed
** =========================================================================*/
/**
  Sets the bus frequency directly

  This function allows to set bus frequency directly without go thru NPA request.
  Use this function for request to change bus speed directly without DCVS enable

  @param eBusID [in] - Bus type
  @param nFreq  [in] - Clock frequency in kHz

  @return
  None

  @dependencies
  None.

  @sa
  None
*/

void Clock_SetDirectBusSpeed( ClockBusType eBusID, uint32 nNewSpeedKHz );



/* =========================================================================
**  Function : Clock_RegisterCallback
** =========================================================================*/
/**
  This function registers a callback that will be called before and after every 
  clock switch.

  @param nClock [in] - Clock to register callback.
  @param pHandle [out] - Returned handle to the callback.
  @param pCallback [in] - Function pointer to the callback function.
  @param pCallbackData [in] - Can be used to pass any 32-bit pointer to the callback

  DAL_SUCCESS -- Clock was valid. The frequency may or may not have been 
                 retrieved (see function description). \n
  DAL_ERROR -- Clock identifier was invalid.
  @return

  @dependencies
  None.

  @sa
  None
*/

DALResult Clock_RegisterCallback( ClockIdType nClock, ClockCallbackHandle *pHandle, ClockCallbackType pCallback, void *pCallbackData );

/* =========================================================================
**  Function : Clock_UnregisterCallback
** =========================================================================*/
/**
  This function removes the registered callback.

  @param nClock [in] - Clock to register callback.
  @param pHandle [out] - Returned handle to the callback.

  @return
  DAL_SUCCESS -- Clock was valid. The frequency may or may not have been 
                 retrieved (see function description). \n
  DAL_ERROR -- Clock identifier was invalid.

  @dependencies
  None.

  @sa
  None
*/
DALResult Clock_UnregisterCallback( ClockIdType nClock, ClockCallbackHandle pHandle );

/** @} */ /* end_addtogroup clock_functions */

/** @addtogroup power_domain_functions
@{ */

/* =========================================================================
**  Function : Clock_GetPowerDomainId
** =========================================================================*/
/**
  Gets the power domain identifier.

  This function looks up and returns the identifier for a specified power
  domain. This identifier is used for the power domain control functions.
  The string used to get the identifier can be found in the software frequency 
  plan, e.g., VDD_GRP.

  @param szPowerDomain     [in]  Power domain name.
  @param pnPowerDomainId   [out] Pointer to the ID to be filled in. 

  @return
  DAL_SUCCESS -- Power domain was valid, and the ID was filled in. \n
  DAL_ERROR -- Power domain was invalid.

  @dependencies
  None.
*/

DALResult Clock_GetPowerDomainId( const char * szPowerDomain,  ClockPowerDomainIdType * pnPowerDomainId);


/* =========================================================================
**  Function : Clock_EnablePowerDomain
** =========================================================================*/
/**
  Enables a power domain.

  @param nPowerDomainId [in] - Power domain to enable.
         bAsync [in]         - Async function call.  Return right away without
                               without looping for checking enable power bit

  @return
  DAL_SUCCESS -- Power domain was enabled. \n
  DAL_ERROR -- Power domain identifier was invalid.

  @dependencies
  None.

  @sideeffects
  Any clocks inside the power domain are enabled during the enable
  sequence. The state is restored afterward.

  @sa
  Clock_GetPowerDomainId(), Clock_DisablePowerDomain()
*/

DALResult Clock_EnablePowerDomain( ClockPowerDomainIdType  nPowerDomainId, boolean bAsync );


/* =========================================================================
**  Function : Clock_DisablePowerDomain
** =========================================================================*/
/**
  Disables a power domain.

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
  Clock_GetPowerDomainId(), Clock_EnablePowerDomain()
*/

DALResult Clock_DisablePowerDomain( ClockPowerDomainIdType  nPowerDomainId);


/* =========================================================================
**  Function : Clock_IsPowerDomainEnabled
** =========================================================================*/
/**  Check if a power domain is enable or not.

  @param nPowerDomainId   [in] Power domain to check.
  
  @return  
  FALSE -- Power domain was disabled. 
  TRUE  -- Power domain was enabled. 

  @dependencies
  None

  @sideeffects
  None 

  @sa  Clock_GetPowerDomainId()
*/

boolean Clock_IsPowerDomainEnabled( ClockPowerDomainIdType  nPowerDomainId );

/** @} */ /* end_addtogroup power_domain_functions */

/** @addtogroup misc_functions
@{ */


/* =========================================================================
**  Function : Clock_ProcessorSleep
** =========================================================================*/
/**
  Collapse all the clocks and set the processor clock to sleep.

  This function restores the processor clocks after a power collapse.

  @param eMode     [in] Type of sleep; usually 
                        CLOCK_SLEEP_MODE_POWER_COLLAPSE.
  @param nFlags    [in] Mode-specific flags;
                        0 - Collapse all the clocks except BIMC clock
                        1 - Put DDR in self refresh but not BIMC collapsed
                        2 - Put BIMC in power collapse mode

  @return
  Always returns DAL_SUCCESS.

  @dependencies
  None

*/

DALResult Clock_ProcessorSleep( ClockSleepModeType  eMode,  uint32  nFlags);


/* =========================================================================
**  Function : Clock_ProcessorRestore
** =========================================================================*/
/**
  Restores the processor clock state after sleep.

  This function restores the processor clocks after a power collapse.

  @param eMode     [in] Type of sleep to leave; usually 
                        CLOCK_SLEEP_MODE_POWER_COLLAPSE.
  @param nFlags    [in] Mode-specific flags.
                        0 - Restore all the clocks except BIMC clock
                        1 - Restore DDR out of self refresh but not BIMC collapsed
                        2 - Restore BIMC out of power collapse mode

  @return
  Always returns DAL_SUCCESS.

  @dependencies
  Clock_ProcessorSleep() must have been called previously.

  @sa
  Clock_ProcessorSleep()
*/

DALResult Clock_ProcessorRestore( ClockSleepModeType  eMode,  uint32  nFlags);


/* =========================================================================
**  Function : Clock_SetBIMCPowerCollapse
** =========================================================================*/
/**
  This function is used for setting BIMC in Power Collapsed Mode.

  @param bCollapsed [in] 
      TRUE - Put DDR in self-refresh, programming freeze IO, and Disable BIMC GDSC
      FALSE - Put DDR in self-refresh only.

  @return
      None

  @dependencies
      None

  @sa
      None
*/

void Clock_SetBIMCPowerCollapse( boolean bCollapsed );


/* =========================================================================
**  Function : Clock_WaitForBIMCPowerCollapse()
** =========================================================================*/
/**
  This function is polling BIMC status until BIMC collapsed done.

  @param 
      None

  @return
      None

  @dependencies
      Clock_SetBIMCPowerCollapse() must have been called previously.

  @sa
      Clock_SetBIMCPowerCollapse()
*/

void Clock_WaitForBIMCPowerCollapse( void );


/* =========================================================================
**  Function : Clock_SetBIMCPowerRestore
** =========================================================================*/
/**
  This function is used for restore BIMC out of Power Collapsed Mode.

  @param 
      None

  @return
      None

  @dependencies
      Clock_SetBIMCPowerCollapse

  @sa
      Clock_SetBIMCPowerCollapse
*/

void Clock_SetBIMCPowerRestore( void );


/* =========================================================================
**  Function : Clock_WaitForBIMCPowerRestore()
** =========================================================================*/
/**
  This function is polling BIMC status until BIMC restore done.

  @param 
      None

  @return
      None

  @dependencies
      Clock_SetBIMCPowerRestore() must have been called previously.

  @sa
      Clock_SetBIMCPowerRestore()
*/

void Clock_WaitForBIMCPowerRestore( void );


/* =========================================================================
**  Function : Clock_BIMCGetMiscInfo
** =========================================================================*/
/**
  Get GCC_BIMC_MISC register info

  @param 
    None

  @return
    Return GCC_BIMC_MISC info

  @dependencies
    None

  @sa
    None
*/

uint32 Clock_BIMCGetMiscInfo( void );


/* =========================================================================
**  Function : Clock_BIMCToggleJCPLL
** =========================================================================*/
/**
  This function is used for toggle update bit in BIMC_DDR_CMD_RCGR for
  switching between JCPLL0 and JCPLL1.  This API is used for DDR site training
  where it needs to configure BIMC clock to run in both JCPLL0 & JCPLL1

  @param 
    None

  @return
    None

  @dependencies
    None

  @sa
    None
*/

void Clock_BIMCToggleJCPLL( void );


/* =========================================================================
**  Function : Clock_SwitchBusVoltage
** =========================================================================*/
/**
  This function is used for switching all the buses to different voltage
  level (SVS, NOMINAL, HIGH) for DDR SITE training.

  @param 
    eVoltageLevel - Clock Voltage Level (SVS, NOMINAL, HIGH)

  @return
    None

  @dependencies
    None

  @sa
    None
*/
void Clock_SwitchBusVoltage( ClockVRegLevelType eVoltageLevel );

/*=========================================================================
**  Function : Clock_LogState
** =========================================================================*/
/**
  This function is used for logging all the clocks status

  @param 
      None

  @return 
      None

  @dependencies
      None.
*/
void Clock_LogState ( void );

/** @} */ /* end_addtogroup misc_functions */


/** @addtogroup boot_functions
@{ */

/* =========================================================================
**  Function : Clock_Init
** =========================================================================*/
/**
  Performs clock initialization in limited environments.

  This function is the normal driver init function for RPM.

  @return
  TRUE -- Clock_Init was successful. \n
  FALSE -- Critical failure; RPM should not proceed.

  @dependencies
  Varies based on the image under consideration; usually, there are 
  no dependencies.
*/

boolean Clock_Init (void);




/** @} */ /* end_addtogroup boot_functions */


#endif /* !CLOCK_H */

