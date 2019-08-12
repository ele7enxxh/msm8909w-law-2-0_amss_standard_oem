#ifndef _HSU_HAL_HELPER_H_
#define _HSU_HAL_HELPER_H_

/*
===========================================================================

FILE:         hsu_hal_helper.h

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_hal_helper.h#1 $

===========================================================================

===========================================================================
Copyright © 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_hal_helper.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  08/25/10  rg 	Initial version
    
=============================================================================*/


/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION      HAL_hsusb_SpinlockInit
=============================================================================*/
/**
Initializes the spinlock used by the HS-USB HAL functions. Must be called
only once.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SpinlockInit(void);

/*=============================================================================
FUNCTION      HAL_hsusb_SetBits32_safe
=============================================================================*/
/**
Sets bits in the HS-USB core 32-bit register.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.
@param bits     [IN] -- Determines the bits to be set.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetBits32_safe(uint8 instance, uint32 offset, uint32 bits);

/*=============================================================================
FUNCTION      HAL_hsusb_ClearBits32_safe
=============================================================================*/
/**
Clears bits in the HS-USB core 32-bit register.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param offset   [IN] -- Determines the register offset. This value must be
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.
@param bits     [IN] -- Determines the bits to be cleared.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_ClearBits32_safe(uint8 instance, uint32 offset, uint32 bits);

/*=============================================================================
FUNCTION      HAL_hsusb_ClearAndSetBits32_safe
=============================================================================*/
/**
Clears and then sets bits in the HS-USB core 32-bit register.

@param instance  [IN] -- Determines which HS-USB core to use. This value should
                         be one less than reported by 
						 HAL_hsusb_SupportedCoresQty().
@param offset    [IN] -- Determines the register offset. This value must be
                        aligned at the 32-bit boundary and in the address 
						ranges defined by the hardware specification.
@param clearBits [IN] -- Determines the bits to be cleared.
@param setBits   [IN] -- Determines the bits to be set.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_ClearAndSetBits32_safe(uint8 instance, uint32 offset, uint32 clearBits, uint32 setBits);

/*=============================================================================
FUNCTION      HAL_hsusb_SetAhbBurstMode_safe
=============================================================================*/
/**
Sets the mode of the AMBA AHB master interface m_hburst signal.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param eMode    [IN] -- Determines mode of the AMBA AHB master interface 
                        m_hburst signal.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetAhbBurstMode_safe(uint8 instance, HAL_HSUSB_AhbBurstMode eMode);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSe0GlitchFixCtrl_safe
=============================================================================*/
/**
Activates/deactivates the SE0 glitch fix mechanism.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Activate the SE0 glitch fix mechanism.
                        - FALSE -- Deactivate the SE0 glitch fix mechanism.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetSe0GlitchFixCtrl_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetFs3Wire2WireSelect_safe
=============================================================================*/
/**
Activates/deactivates the two-wire interface on the fs_dat and fs_se0 pins.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the two wire interface on the fs_dat 
						          and fs_se0 pins.
                        - FALSE -- Disable the two wire interface on the fs_dat
						           and fs_se0 pins.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetFs3Wire2WireSelect_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetXtorBypass_safe
=============================================================================*/
/**
Sets the AHB Transactor bypass mode.

@note This function is valid for HS-USB hardware version 1 only.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Bypass the AHB Transactor.
                        - FALSE -- Do not bypass the AHB Transactor.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetXtorBypass_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetPostedWrites_safe
=============================================================================*/
/**
Sets the USB core AHB posted data writes mode.

@note This function is valid for HS-USB hardware version 1 only.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- The AHB master interface enables posted data
                                  writes.
                        - FALSE -- The AHB master interface disables posted
						           data writes.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetPostedWrites_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetGrantStolen_safe
=============================================================================*/
/**
Sets the arbiter to remove the hgrant signal before completing a transaction.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the arbiter to remove the hgrant
						          signal before completing a transaction.
                        - FALSE -- Disable the arbiter from removing the
						           hgrant signal before completing a
								   transaction.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetGrantStolen_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetHprotMode_safe
=============================================================================*/
/**
Sets the HPROT signal mode.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param eMode    [IN] -- Determines the HPROT signal mode.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetHprotMode_safe(uint8 instance, HAL_HSUSB_HprotMode eMode);

/*=============================================================================
FUNCTION      HAL_hsusb_SetIncrOverride_safe
=============================================================================*/
/**
Sets the USB core INCR bursts transformation.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- All INCR bursts from the USB core will be
                                  internally transformed into SINGLE transfers.
                        - FALSE -- If the USB core issues an INCR burst, it
                                   propogates to the external master AHB port.
							    
@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetIncrOverride_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetAsyncBridgesBypass_safe
=============================================================================*/
/**
Sets the asynchronous bridge bypass mode on the master AHB interface.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- The asynchronous bridge on the master AHB
                                  interface is bypassed.
                        - FALSE -- The asynchronous bridge on the master AHB
                                   interface is issued.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetAsyncBridgesBypass_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetTestBusSelector_safe
=============================================================================*/
/**
Sets the group of internal signals that are to be routed to the test bus.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param eMode    [IN] -- Determines the group of internal signals that are to be
                        routed to the test bus.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetTestBusSelector_safe(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode);

/*=============================================================================
FUNCTION      HAL_hsusb_SetTxBufCollisionMode_safe
=============================================================================*/
/**
Sets the Tx buffer collision mode of the controller.

@note This function is valid for HS-USB hardware version 4 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Tx buffer collision direct mode.
                        - FALSE -- Tx buffer collision revert mode.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetTxBufCollisionMode_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetStreamRxBypassMode_safe
=============================================================================*/
/**
Sets the Rx streaming bypass mode.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable Rx streaming bypass.
                        - FALSE -- Disable Rx streaming bypass.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetStreamRxBypassMode_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetStreamTxAutoMode_safe
=============================================================================*/
/**
Sets the Tx streaming automatic determination mode.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable Tx streaming automatic determination.
                        - FALSE -- Disable Tx streaming automatic determination.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetStreamTxAutoMode_safe (uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetHostSimTimersStdMode_safe
=============================================================================*/
/**
Sets the timers used for USB reset in ULPI mode.

@note This function is valid for HS-USB hardware version 1 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the timers used for USB reset in ULPI
                                  mode.
                        - FALSE -- Disable the timers used for USB reset in
						           ULPI mode.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetHostSimTimersStdMode_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetHostSimTimersSuspMode_safe
=============================================================================*/
/**
Sets the timers used for the USB suspend process mode.

@note This function is valid for HS-USB hardware version 2 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE  -- Set the timers used for the USB suspend
						           process short for faster simulation and ATE
								   times.
                        - FALSE -- Set the timers used for the USB suspend
						           process according to the USB specification.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetHostSimTimersSuspMode_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetDevicePEStateReset_safe
=============================================================================*/
/**
Enables/disables an automatic reset of Device PE State.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE  -- Enable an automatic reset of Device PE
						           state.
                        - FALSE -- Disable an automatic reset of Device PE
						           state.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetDevicePEStateReset_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetIsochronousBugFix_safe
=============================================================================*/
/**
Enables/disables the Isochronous bug fix.

@note This function is valid for HS-USB hardware version 5 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE  -- Enable the Isochronous bug fix.
                        - FALSE -- Disable the Isochronous bug fix.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetIsochronousBugFix_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetAHBBufferableDataAccess_safe
=============================================================================*/
/**
Enables/disables AHB-bufferable data access.

@note This function is valid for HS-USB hardware version 6 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable AHB-bufferable data access.
                        - FALSE -- Disable AHB-bufferable data access.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetAHBBufferableDataAccess_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetLegacyMode_safe
=============================================================================*/
/**
Enables/disables Legacy mode.

@note This function is valid for HS-USB hardware version 6 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable Legacy mode.
                        - FALSE -- Disable Legacy mode.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetLegacyMode_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetEndpointPipeID_safe
=============================================================================*/
/**
Sets the endpoint pipe ID number.

@note This function is valid for HS-USB hardware version 6 and above.

@param instance  [IN] -- Determines which HS-USB core to use. This value should
                         be one less than reported by 
                         HAL_hsusb_SupportedCoresQty().
@param ep        [IN] -- Determines the endpoint number. Value is between 1 and
                         15.
@param direction [IN] -- Determines the endpoint direction. Values are:
                         - TRUE -- IN endpoint.
                         - FALSE -- OUT endpoint.
@param id        [IN] -- Determines the endpoint pipe number. Values are 0-0x1F.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetEndpointPipeID_safe(uint8 instance, uint8 ep, boolean direction, uint8 id);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSessVldOperation_safe
=============================================================================*/
/**
Enables/disables Link Controller operation after switching the interface from
serial to ULPI.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable Link Controller operation.
                        - FALSE -- Disable Link Controller operation.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetSessVldOperation_safe (uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSessVldStatus_safe
=============================================================================*/
/**
Sets/clears the PHY SESS_VLD status.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Set the PHY SESS_VLD status.
                        - FALSE -- Clear the PHY SESS_VLD status.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetSessVldStatus_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSessVldChangeStatus_safe
=============================================================================*/
/**
Sets/clears the PHY SESS_VLD status change.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Set the PHY SESS_VLD status change.
                        - FALSE -- Clear the PHY SESS_VLD status change.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetSessVldChangeStatus_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSessVldChangeInterrupt_safe
=============================================================================*/
/**
Enables/disables the PHY SESS_VLD status change interrupt.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the PHY SESS_VLD status change
						          interrupt.
                        - FALSE -- Disable the PHY SESS_VLD status change
						           interrupt.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetSessVldChangeInterrupt_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSysClkSw_safe
=============================================================================*/
/**
Enables/disables the USB core for/from voting for USB_SYSTEM_CLK.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the USB core to vote for
						          USB_SYSTEM_CLK.
                        - FALSE -- Disable the USB core from voting for
                              USB_SYSTEM_CLK.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetSysClkSw_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetResumeEndInterrupt_safe
=============================================================================*/
/**
Enables/disables the port change interrupt at Resume End in Host mode.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the port change interrupt at Resume
						          End in Host mode.
                        - FALSE -- Disable the port change interrupt at Resume
						           End in Host mode.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetResumeEndInterrupt_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSuspendWrongOpmode_safe
=============================================================================*/
/**
Enables/disables the USB core to assert opmode with the wrong value when the
core enters a Suspend state.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Disable the USB core from asserting opmode
						          with the wrong value when the core enters a
								  Suspend state.
                        - FALSE -- Enable the USB core to assert opmode with
						           the wrong value when the core enters a
								   Suspend state.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetSuspendWrongOpmode_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetNoSofRxFifoFull_safe
=============================================================================*/
/**
Enables/disables whether the USB core will wait until the Rx FIFO is not full
and to generate SOF when it is full. 

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Disable the USB core from waiting until the
						          Rx FIFO becomes not full and to generate SOF
								  when it is full.
                        - FALSE -- Enable the USB core to wait until the Rx
						           FIFO becomes not full and to generate SOF
								   when it is full.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetNoSofRxFifoFull_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetIsoInZlpPrime_safe
=============================================================================*/
/**
Enables/disables the USB core to reply with a zero-length packet before the
last token IN is received when using ISO IN endpoints with MULT=3 and low
bandwidth system bus access.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the USB core to reply with a zero-length
                                  packet before the last token IN is received
								  when using ISO IN endpoints with MULT=3 and
								  low bandwidth system bus access.
                        - FALSE -- Disable the USB core from replying with a
				                   zero-length packet before the last token IN
								   is received when using ISO IN endpoints with
                                   MULT=3 and low bandwidth system bus access.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetIsoInZlpPrime_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetDatalinePulsingReset_safe
=============================================================================*/
/**
Enables/disables the USB core to recognize the attached device's attempts to
perform a Session Request Protocol by using data-line pulsing in Host mode when
VBUS is turned off for the HS-USB core usb_otg_hs_p3q3r26 implementation.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Enable the USB core to recognize the attached
                                  device's attempts to perform a Session
								  Request Protocol by using data-line pulsing
								  in Host mode when the VBUS is turned off.
                        - FALSE -- Disable the USB core from recognizing the
                                   attached device's attempts to perform a
								   Session Request Protocol by using data-line
								   pulsing in Host mode when the VBUS is turned
								   off.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetDatalinePulsingReset_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetDevicePortChirpKTime_safe
=============================================================================*/
/**
Sets the Device Port Control state machine Chirp K time.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Set the Device Port Control state machine
						          Chirp K time to 2 ms.
                        - FALSE -- Set the Device Port Control state machine
						           Chirp K time to 1 ms.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetDevicePortChirpKTime_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_SetSessVldControl_safe
=============================================================================*/
/**
Sets the Link Controller sess_vld signal control.

@note This function is valid for HS-USB hardware version 7 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- Determines the requested action. Values are:
                        - TRUE -- Set the Link Controller sess_vld signal to be
                                  controlled by bit 25 of the USBCMD register.
                        - FALSE -- Set the Link Controller sess_vld signal to
						           be received from the PHY.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
None.
*/
void HAL_hsusb_SetSessVldControl_safe(uint8 instance, boolean state);

/*=============================================================================
FUNCTION      HAL_hsusb_ReadUlpiPhy_safe
=============================================================================*/
/**
Reads from a ULPI PHY register of the HS-USB core port.

@note This function is valid for HS-USB hardware version 1 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should 
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param port     [IN] -- Determines which HS-USB core port to use. This value 
                        should be one less than reported by 
						HAL_hsusb_SupportedPortsQty().
@param reg      [IN] -- Determines the register address. This value must be in 
                        the address ranges defined by the hardware 
						specification.
@param data    [OUT] -- Returns the read data.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
HAL_HSUSB_Success           -- The returned data value is valid.\n
HAL_HSUSB_WrongPort         -- The port is out of the scope reported by
                               HAL_hsusb_SupportedPortsQty().\n
HAL_HSUSB_WrongPhyInterface -- The PHY interface is not ULPI.\n
HAL_HSUSB_TimeoutExpired    -- The maximum timeout has expired.
*/
HAL_HSUSB_ErrorType HAL_hsusb_ReadUlpiPhy_safe(uint8 instance, uint8 port, uint8 reg, uint8* data);

/*=============================================================================
FUNCTION      HAL_hsusb_WriteUlpiPhy_safe
=============================================================================*/
/**
Writes to a ULPI PHY register of the HS-USB core port.

@note This function is valid for HS-USB hardware version 1 and above.

@param instance [IN] -- Determines which HS-USB core to use. This value should 
                        be one less than reported by 
						HAL_hsusb_SupportedCoresQty().
@param port     [IN] -- Determines which HS-USB core port to use. This value 
                        should be should be one less than reported by
                        HAL_hsusb_SupportedPortsQty().
@param reg      [IN] -- Determines the register address. This value must be in 
                        the address ranges defined by the hardware 
						specification.
@param data     [IN] -- Determines the write data.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@sideeffects
None.

@return
HAL_HSUSB_Success           -- The returned data value is valid.\n
HAL_HSUSB_WrongPort         -- The port is out of the scope reported by
                               HAL_hsusb_SupportedPortsQty().\n
HAL_HSUSB_WrongPhyInterface -- The PHY interface is not ULPI.\n
HAL_HSUSB_TimeoutExpired    -- The maximum timeout has expired.
*/
HAL_HSUSB_ErrorType HAL_hsusb_WriteUlpiPhy_safe(uint8 instance, uint8 port, uint8 reg, uint8 data);

#endif /* _HSU_HAL_HELPER_H_ */
