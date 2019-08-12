#ifndef HAL_SSUSB
#define HAL_SSUSB
/*
===========================================================================

FILE:         HAL_ssusb.h

DESCRIPTION:  
This is the HAL external interface for the SS-USB core.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/HAL_ssusb.h#1 $

===========================================================================

===========================================================================
Copyright © 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "comdef.h"

/* -----------------------------------------------------------------------
**                           Constants and Macros
** ----------------------------------------------------------------------- */

/*
* Global registers
*/
#define HAL_SSUSB_REG_GSBUSCFG0           0xC100
#define HAL_SSUSB_REG_GSBUSCFG1           0xC104
#define HAL_SSUSB_REG_GTXTHRCFG           0xC108
#define HAL_SSUSB_REG_GRXTHRCFG           0xC10C
#define HAL_SSUSB_REG_GCTL                0xC110
#define HAL_SSUSB_REG_GEVTEN              0xC114
#define HAL_SSUSB_REG_GSTS                0xC118
#define HAL_SSUSB_REG_GSNPSID             0xC120
#define HAL_SSUSB_REG_GGPIO               0xC124
#define HAL_SSUSB_REG_GUID                0xC128
#define HAL_SSUSB_REG_GBUSERRADDRLO       0xC130
#define HAL_SSUSB_REG_GBUSERRADDRHI       0xC134
#define HAL_SSUSB_REG_GHWPARAMS0          0xC140
#define HAL_SSUSB_REG_GHWPARAMS1          0xC144
#define HAL_SSUSB_REG_GHWPARAMS2          0xC148
#define HAL_SSUSB_REG_GHWPARAMS3          0xC14C
#define HAL_SSUSB_REG_GHWPARAMS4          0xC150
#define HAL_SSUSB_REG_GHWPARAMS5          0xC154
#define HAL_SSUSB_REG_GHWPARAMS6          0xC158
#define HAL_SSUSB_REG_GHWPARAMS7          0xC15C
#define HAL_SSUSB_REG_GBDBFIFOSPACE       0xC160
#define HAL_SSUSB_REG_GDBGLTSSM           0xC164
#define HAL_SSUSB_GUSB2PHYCFG_QUANTITY    16
#define HAL_SSUSB_REG_GUSB2PHYCFG(n)      (0xC200 + (n * 0x16))
#define HAL_SSUSB_GUSB2I2CCTL_QUANTITY    16
#define HAL_SSUSB_REG_GUSB2I2CCTL(n)      (0xC240 + (n * 0x16))
#define HAL_SSUSB_GUSB2PHYACC_QUANTITY    16
#define HAL_SSUSB_REG_GUSB2PHYACC(n)      (0xC280 + (n * 0x16))
#define HAL_SSUSB_GUSB3PIPECTL_QUANTITY   16
#define HAL_SSUSB_REG_GUSB3PIPECTL(n)     (0xC2C0 + (n * 0x16))
#define HAL_SSUSB_GTXFIFOSIZ_QUANTITY     32
#define HAL_SSUSB_REG_GTXFIFOSIZ(n)       (0xC300 + (n * 0x4))
#define HAL_SSUSB_GRXFIFOSIZ_QUANTITY     32
#define HAL_SSUSB_REG_GRXFIFOSIZ(n)       (0xC380 + (n * 0x4))
#define HAL_SSUSB_GEVNTS_QUANTITY         32
#define HAL_SSUSB_REG_GEVNTADRLO(n)       (0xC400 + (n * 0x10))
#define HAL_SSUSB_REG_GEVNTADRHI(n)       (0xC404 + (n * 0x10))
#define HAL_SSUSB_REG_GEVNTSIZ(n)         (0xC408 + (n * 0x10))
#define HAL_SSUSB_REG_GEVNTCOUNT(n)       (0xC40C + (n * 0x10))

/*
* Device registers
*/
#define HAL_SSUSB_REG_DCFG                0xC700
#define HAL_SSUSB_REG_DCTL                0xC704
#define HAL_SSUSB_REG_DEVTEN              0xC708
#define HAL_SSUSB_REG_DSTS                0xC70C
#define HAL_SSUSB_REG_DGCMDPAR            0xC710
#define HAL_SSUSB_REG_DGCMD               0xC714
#define HAL_SSUSB_REG_DALEPENA            0xC720
#define HAL_SSUSB_DEPS_QUANTITY           32
#define HAL_SSUSB_REG_DEPCMDPAR2(n)       (0xC800 + (n * 0x10))
#define HAL_SSUSB_REG_DEPCMDPAR1(n)       (0xC804 + (n * 0x10))
#define HAL_SSUSB_REG_DEPCMDPAR0(n)       (0xC808 + (n * 0x10))
#define HAL_SSUSB_REG_DEPCMD(n)           (0xC80C + (n * 0x10))

/*
* USB 2.0 OTG registers
*/
#define HAL_SSUSB_REG_OCFG                0xCC00
#define HAL_SSUSB_REG_OCTL                0xCC04
#define HAL_SSUSB_REG_OEVTEN              0xCC08
#define HAL_SSUSB_REG_OSTS                0xCC0C

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
* HAL_SSUSB_ErrorType
*
* Enumeration of possible error types for a SS-USB core.
*
* HAL_SSUSB_Success:        There are no errors.
* HAL_SSUSB_ NotSupported:  The function is not supported by 
*                           the SS-USB core.
*/
typedef enum
{
  HAL_SSUSB_Success,
  HAL_SSUSB_NotSupported
} HAL_SSUSB_ErrorType;

/*
* HAL_SSUSB_BusParamsType
*/
typedef struct
{
  uint32                  BaseAddress;
  uint8                   IntNumber;
} HAL_SSUSB_BusParamsType;

/* -----------------------------------------------------------------------
**                           EXTERNS
** ----------------------------------------------------------------------- */

/*
* SS-USB Cores' bus parameters.
*/
extern HAL_SSUSB_BusParamsType ssusb_cores_bus_params[];

/* -----------------------------------------------------------------------
**                           INTERFACES
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

/* Mandatory API */
void    HAL_ssusb_Init(void);
void    HAL_ssusb_Reset(void);
/* Special API */
uint8   HAL_ssusb_SupportedCoresQty(void);
HAL_SSUSB_ErrorType HAL_ssusb_GetLastError(uint8 instance);
uint8   HAL_ssusb_GetInterruptNumber(uint8 instance);
uint32  HAL_ssusb_Read32(uint8 instance, uint32 offset);
void    HAL_ssusb_Write32(uint8 instance, uint32 offset, uint32 data);
uint32  HAL_ssusb_GetBits32(uint8 instance, uint32 offset, uint32 bits);
void    HAL_ssusb_SetBits32(uint8 instance, uint32 offset, uint32 bits);
void    HAL_ssusb_ClearBits32(uint8 instance, uint32 offset, uint32 bits);
void    HAL_ssusb_ClearAndSetBits32(uint8 instance, uint32 offset, uint32 clearBits, uint32 setBits);

#ifdef __cplusplus
}
#endif

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================
**
**  HAL_ssusb_Init
**
**  Description:
**    This function initializes the SS-USB core Hardware prior 
**    to the first usage after power-up or a call to HAL_ssusb_Reset().
**    
**  Dependencies:
**    None.
**
**  Parameters:
**    None.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_ssusb_Reset
**
**  Description:
**    This function is intended to return the SS-USB hardware to its 
**    power-on status.
**    
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    None.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_ssusb_SupportedCoresQty
**
**  Description:
**    This function reports the SS-USB cores’ amount.
**    
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    None.
**
**  Return:
**     The number of the SS-USB cores attached to the target.
**
** ========================================================================
**
**  HAL_ssusb_GetLastError
**
**  Description:
**    This function gets the last error code of the SS-USB core recorded
**    from the SS-USB HAL failure. It is intended for debug purposes.
**    
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_ssusb_SupportedCoresQty().
**
**  Return:
**     The last error code of the SS-USB core recorded from the SS-USB HAL failure.
**
** ========================================================================
**
**  HAL_ssusb_GetInterruptNumber
**
**  Description:
**    This function gets the interrupt number of the SS-USB core.
**    
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_ssusb_SupportedCoresQty().
**
**  Return:
**     The interrupt number of the SS-USB core.
**
** ========================================================================
**
**  HAL_ssusb_Read32
**
**  Description:
**    This function reads 32 bits from the SS-USB core register.
**    
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_ssusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ========================================================================
**
**  HAL_ssusb_Write32
**
**  Description:
**    This function writes 32 bits to the SS-USB core register.
**    
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_ssusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_ssusb_GetBits32
**
**  Description:
**    This function gets bits f the SS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_ssusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be get.
**
**  Return:
**    The read data masked with <bits>.
**
** ========================================================================
**
**  HAL_ssusb_SetBits32
**
**  Description:
**    This function sets bits in the SS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_ssusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be set.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_ssusb_ClearBits32
**
**  Description:
**    This function clears bits in the SS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_ssusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be cleared.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_ssusb_ClearAndSetBits32
**
**  Description:
**    This function clears and then sets bits in the SS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance  - Determines which SS-USB core to use. 
**              This value should be one less than reported by 
**              the HAL_ssusb_SupportedCoresQty().
**    offset    - Determines the register offset. 
**              This value must be aligned at the 32 bit boundary and 
**              in the address ranges defined by the HW specification.
**    clearBits - Determines the bits to be cleared.
**    setBits   - Determines the bits to be set.
**
**  Return:
**    None.
**
** ========================================================================
*/

#endif /* HAL_SSUSB */
