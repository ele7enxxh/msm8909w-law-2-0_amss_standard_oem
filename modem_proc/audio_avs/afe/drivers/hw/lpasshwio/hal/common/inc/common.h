#ifndef __COMMON_H__
#define __COMMON_H__

/** @file common.h
    Publicizes common functions used in all LPASS HALs. These functions are not
    exposed outside LPASS HAL implementations.

    Copyright (c) 2008 Qualcomm Technologies, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

when       who     what, where, why
------     ---    -------------------------------------------------------
10/30/08    RA     Initial creation

========================================================================== */

#if defined(__cplusplus)
extern "C" {
#endif

#include "typedef_drivers.h"  

void UpdateRegister(uint32 *basePhysAddr, int32 virtOffset, uint32 regOffset, 
                    uint32 mask, uint32 fieldValue, int32 isWriteOnly);
void ReadRegister(uint32 *basePhysAddr, int32 virtOffset, uint32 regOffset, 
                  uint32 *pValue);
void UpdateRegister1(uint32 regPhysAddr, int32 virtOffset, 
                     uint32 mask, uint32 fieldValue, int32 isWriteOnly);
void ReadRegister1(uint32 regPhysAddr, int32 virtOffset, uint32 *pValue);

void UpdateRegisterCSR(uint32 regPhysAddr, int32 virtOffset, 
                     uint32 mask, uint32 fieldValue, int32 isWriteOnly);
void ReadRegisterCSR(uint32 regPhysAddr, int32 virtOffset, uint32 *pValue);

void UpdateRegisterAVS(uint32 regPhysAddr, int32 virtOffset, 
                     uint32 mask, uint32 fieldValue, int32 isWriteOnly);
void ReadRegisterAVS(uint32 regPhysAddr, int32 virtOffset, uint32 *pValue);

/** 
 * Write the input value in to a register specified by the 
 * register address. This register address is a virt address.
 * This function does not write atomically, as there should be 
 * no other HAL writing to the same memory location. 
 * 
 * @param[in] reg_addr     Register virtual address
 * @param[in] value        value to be written 
 * 
 */
static inline void reg_wr(uint32 reg_addr,uint32 value)
{
   volatile uint32 *addr;
   
   addr = (volatile uint32 *)reg_addr; 
   //qurt_printf("reg_wr addr is 0x%x \n",addr);
   *(addr) = value; ///< Write back
}

/** 
 * read the contents of a register specified by reg_addr. This 
 * register address is a virt address. This function does not 
 * write atomically, as there should be no other HAL writing to 
 * the same memory location. 
 * 
 * @param[in] reg_addr     Register virtual address
 * @param[in] *value_ptr   register value.      
 * 
 */
static inline void reg_rd(uint32 reg_addr, uint32 *value_ptr)
{
	volatile uint32 *addr;

	addr = (volatile uint32 *)reg_addr; 
    //qurt_printf("reg_rd addr is 0x%x \n",addr);
	*value_ptr = *(addr);

}

#if defined(__cplusplus)
}
#endif

#endif  // defined(__HAL_MIDI_HW_H__)
