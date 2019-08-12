#ifndef HAL_HWIO_H
#define HAL_HWIO_H
/*
===========================================================================
*/
/**
  @file HALhwio.h 
  
  Public interface include file for accessing the HWIO HAL definitions.
  
  The HALhwio.h file is the public API interface to the HW I/O (HWIO)
  register access definitions.
*/
/*  
  ====================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/hw_sequence/BIMC/v2.2/HALhwio.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
  ====================================================================
*/ 

/*=========================================================================
      Include Files
==========================================================================*/


/*
 * Common types.
 */
#include "comdef.h"



/* -----------------------------------------------------------------------
** Macros
** ----------------------------------------------------------------------- */

/** 
  @addtogroup macros
  @{ 
*/ 

/**
 * Map a base name to the pointer to access the base.
 *
 * This macro maps a base name to the pointer to access the base.
 * This is generally just used internally.
 *
 */
#ifndef HWIO_BASE_PTR
  #define HWIO_BASE_PTR(base) base##_BASE_PTR
#endif


/**
 * Declare a HWIO base pointer.
 *
 * This macro will declare a HWIO base pointer data structure.  The pointer
 * will always be declared as a weak symbol so multiple declarations will
 * resolve correctly to the same data at link-time.
 */
#ifdef __ARMCC_VERSION
  #ifndef DECLARE_HWIO_BASE_PTR
    #define DECLARE_HWIO_BASE_PTR(base) __weak uint8 *HWIO_BASE_PTR(base)
  #endif
#else
  #ifndef DECLARE_HWIO_BASE_PTR
    #define DECLARE_HWIO_BASE_PTR(base) uint8 *HWIO_BASE_PTR(base)
  #endif
#endif

/**
  @}
*/


/** 
  @addtogroup hwio_macros
  @{ 
*/ 

/**
 * @name Address Macros
 *
 * Macros for getting register addresses.
 * These macros are used for retrieving the address of a register.
 * HWIO_ADDR* will return the directly accessible address (virtual or physical based
 * on environment), HWIO_PHYS* will always return the physical address.
 * The offset from the base region can be retrieved using HWIO_OFFS*.
 * The "X" extension is used for explicit addressing where the base address of
 * the module in question is provided as an argument to the macro.
 *
 * @{
 */
#ifndef HWIO_ADDR
  #define HWIO_ADDR(hwiosym)                               __msmhwio_addr(hwiosym)
#endif
#ifndef HWIO_ADDRI
  #define HWIO_ADDRI(hwiosym, index)                       __msmhwio_addri(hwiosym, index)
#endif
#ifndef HWIO_ADDRI2
  #define HWIO_ADDRI2(hwiosym, index1, index2)             __msmhwio_addri2(hwiosym, index1, index2)
#endif
#ifndef HWIO_ADDRI3
  #define HWIO_ADDRI3(hwiosym, index1, index2, index3)     __msmhwio_addri3(hwiosym, index1, index2, index3)
#endif

#ifndef HWIO_ADDRX
  #define HWIO_ADDRX(base, hwiosym)                           __msmhwio_addrx(base, hwiosym)
#endif
#ifndef HWIO_ADDRXI
  #define HWIO_ADDRXI(base, hwiosym, index)                   __msmhwio_addrxi(base, hwiosym, index)
#endif
#ifndef HWIO_ADDRXI2
  #define HWIO_ADDRXI2(base, hwiosym, index1, index2)         __msmhwio_addrxi2(base, hwiosym, index1, index2)
#endif
#ifndef HWIO_ADDRXI3
  #define HWIO_ADDRXI3(base, hwiosym, index1, index2, index3) __msmhwio_addrxi3(base, hwiosym, index1, index2, index3)
#endif

#ifndef HWIO_PHYS
  #define HWIO_PHYS(hwiosym)                               __msmhwio_phys(hwiosym)
#endif
#ifndef HWIO_PHYSI
  #define HWIO_PHYSI(hwiosym, index)                       __msmhwio_physi(hwiosym, index)
#endif
#ifndef HWIO_PHYSI2
  #define HWIO_PHYSI2(hwiosym, index1, index2)             __msmhwio_physi2(hwiosym, index1, index2)
#endif
#ifndef HWIO_PHYSI3
  #define HWIO_PHYSI3(hwiosym, index1, index2, index3)     __msmhwio_physi3(hwiosym, index1, index2, index3)
#endif

#ifndef HWIO_PHYSX
  #define HWIO_PHYSX(base, hwiosym)                           __msmhwio_physx(base, hwiosym)
#endif
#ifndef HWIO_PHYSXI
  #define HWIO_PHYSXI(base, hwiosym, index)                   __msmhwio_physxi(base, hwiosym, index)
#endif
#ifndef HWIO_PHYSXI2
  #define HWIO_PHYSXI2(base, hwiosym, index1, index2)         __msmhwio_physxi2(base, hwiosym, index1, index2)
#endif
#ifndef HWIO_PHYSXI3
  #define HWIO_PHYSXI3(base, hwiosym, index1, index2, index3) __msmhwio_physxi3(base, hwiosym, index1, index2, index3)
#endif

#ifndef HWIO_OFFS
  #define HWIO_OFFS(hwiosym)                               __msmhwio_offs(hwiosym)
#endif
#ifndef HWIO_OFFSI
  #define HWIO_OFFSI(hwiosym, index)                       __msmhwio_offsi(hwiosym, index)
#endif
#ifndef HWIO_OFFSI2
  #define HWIO_OFFSI2(hwiosym, index1, index2)             __msmhwio_offsi2(hwiosym, index1, index2)
#endif
#ifndef HWIO_OFFSI3
  #define HWIO_OFFSI3(hwiosym, index1, index2, index3)     __msmhwio_offsi3(hwiosym, index1, index2, index3)
#endif
/** @} */

/**
 * @name Input Macros
 *
 * These macros are used for reading from a named hardware register.  Register
 * arrays ("indexed") use the macros with the "I" suffix.  The "M" suffix
 * indicates that the input will be masked with the supplied mask.  The HWIO_INF*
 * macros take a field name and will do the appropriate masking and shifting
 * to return just the value of that field.
 * The "X" extension is used for explicit addressing where the base address of
 * the module in question is provided as an argument to the macro.
 *
 * Generally you want to use either HWIO_IN or HWIO_INF (with required indexing).
 *
 * @{
 */
#ifndef HWIO_IN
  #define HWIO_IN(hwiosym)                                         __msmhwio_in(hwiosym)
#endif
#ifndef HWIO_INI
  #define HWIO_INI(hwiosym, index)                                 __msmhwio_ini(hwiosym, index)
#endif
#ifndef HWIO_INI2
  #define HWIO_INI2(hwiosym, index1, index2)                       __msmhwio_ini2(hwiosym, index1, index2)
#endif
#ifndef HWIO_INI3
  #define HWIO_INI3(hwiosym, index1, index2, index3)               __msmhwio_ini3(hwiosym, index1, index2, index3)
#endif

#ifndef HWIO_INM
  #define HWIO_INM(hwiosym, mask)                                  __msmhwio_inm(hwiosym, mask)
#endif
#ifndef HWIO_INMI
  #define HWIO_INMI(hwiosym, index, mask)                          __msmhwio_inmi(hwiosym, index, mask)
#endif
#ifndef HWIO_INMI2
  #define HWIO_INMI2(hwiosym, index1, index2, mask)                __msmhwio_inmi2(hwiosym, index1, index2, mask)
#endif
#ifndef HWIO_INMI3
  #define HWIO_INMI3(hwiosym, index1, index2, index3, mask)        __msmhwio_inmi3(hwiosym, index1, index2, index3, mask)
#endif

#ifndef HWIO_INF
  #define HWIO_INF(io, field)                                      (HWIO_INM(io, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#endif
#ifndef HWIO_INFI
  #define HWIO_INFI(io, index, field)                              (HWIO_INMI(io, index, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#endif
#ifndef HWIO_INFI2
  #define HWIO_INFI2(io, index1, index2, field)                    (HWIO_INMI2(io, index1, index2, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#endif
#ifndef HWIO_INFI3
  #define HWIO_INFI3(io, index1, index2, index3, field)            (HWIO_INMI3(io, index1, index2, index3, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#endif

#ifndef HWIO_INX
  #define HWIO_INX(base, hwiosym)                                  __msmhwio_inx(base, hwiosym)
#endif
#ifndef HWIO_INXI
  #define HWIO_INXI(base, hwiosym, index)                          __msmhwio_inxi(base, hwiosym, index)
#endif
#ifndef HWIO_INXI2
  #define HWIO_INXI2(base, hwiosym, index1, index2)                __msmhwio_inxi2(base, hwiosym, index1, index2)
#endif
#ifndef HWIO_INXI3
  #define HWIO_INXI3(base, hwiosym, index1, index2, index3)        __msmhwio_inxi3(base, hwiosym, index1, index2, index3)
#endif

#ifndef HWIO_INXM
  #define HWIO_INXM(base, hwiosym, mask)                           __msmhwio_inxm(base, hwiosym, mask)
#endif
#ifndef HWIO_INXMI
  #define HWIO_INXMI(base, hwiosym, index, mask)                   __msmhwio_inxmi(base, hwiosym, index, mask)
#endif
#ifndef HWIO_INXMI2
  #define HWIO_INXMI2(base, hwiosym, index1, index2, mask)         __msmhwio_inxmi2(base, hwiosym, index1, index2, mask)
#endif
#ifndef HWIO_INXMI3
  #define HWIO_INXMI3(base, hwiosym, index1, index2, index3, mask) __msmhwio_inxmi3(base, hwiosym, index1, index2, index3, mask)
#endif

#ifndef HWIO_INXF
  #define HWIO_INXF(base, io, field)                               (HWIO_INXM(base, io, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#endif
#ifndef HWIO_INXFI
  #define HWIO_INXFI(base, io, index, field)                       (HWIO_INXMI(base, io, index, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#endif
#ifndef HWIO_INXFI2
  #define HWIO_INXFI2(base, io, index1, index2, field)             (HWIO_INXMI2(base, io, index1, index2, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#endif
#ifndef HWIO_INXFI3
  #define HWIO_INXFI3(base, io, index1, index2, index3, field)     (HWIO_INXMI3(base, io, index1, index2, index3, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#endif
/** @} */

/**
 * @name Output Macros
 *
 * These macros are used for writing to a named hardware register.  Register
 * arrays ("indexed") use the macros with the "I" suffix.  The "M" suffix
 * indicates that the output will be masked with the supplied mask (meaning these
 * macros do a read first, mask in the supplied data, then write it back).
 * The "X" extension is used for explicit addressing where the base address of
 * the module in question is provided as an argument to the macro.
 * The HWIO_OUTF* macros take a field name and will do the appropriate masking
 * and shifting to output just the value of that field.
 * HWIO_OUTV* registers take a named value instead of a numeric value and
 * do the same masking/shifting as HWIO_OUTF.
 *
 * Generally you want to use either HWIO_OUT or HWIO_OUTF (with required indexing).
 *
 * @{
 */
#ifndef HWIO_OUT
  #define HWIO_OUT(hwiosym, val)                                   __msmhwio_out(hwiosym, val)
#endif
#ifndef HWIO_OUTI
  #define HWIO_OUTI(hwiosym, index, val)                           __msmhwio_outi(hwiosym, index, val)
#endif
#ifndef HWIO_OUTI2
  #define HWIO_OUTI2(hwiosym, index1, index2, val)                 __msmhwio_outi2(hwiosym, index1, index2, val)
#endif
#ifndef HWIO_OUTI3
  #define HWIO_OUTI3(hwiosym, index1, index2, index3, val)         __msmhwio_outi3(hwiosym, index1, index2, index3, val)
#endif

#ifndef HWIO_OUTM
  #define HWIO_OUTM(hwiosym, mask, val)                            __msmhwio_outm(hwiosym, mask, val)
#endif
#ifndef HWIO_OUTMI
  #define HWIO_OUTMI(hwiosym, index, mask, val)                    __msmhwio_outmi(hwiosym, index, mask, val)
#endif
#ifndef HWIO_OUTMI2
  #define HWIO_OUTMI2(hwiosym, index1, index2, mask, val)          __msmhwio_outmi2(hwiosym, index1, index2, mask, val)
#endif
#ifndef HWIO_OUTMI3
  #define HWIO_OUTMI3(hwiosym, index1, index2, index3, mask, val)  __msmhwio_outmi3(hwiosym, index1, index2, index3, mask, val)
#endif

#ifndef HWIO_OUTF
  #define HWIO_OUTF(io, field, val)                                HWIO_OUTM(io, HWIO_FMSK(io, field), (uint32)(val) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTFI
  #define HWIO_OUTFI(io, index, field, val)                        HWIO_OUTMI(io, index, HWIO_FMSK(io, field), (uint32)(val) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTFI2
  #define HWIO_OUTFI2(io, index1, index2, field, val)              HWIO_OUTMI2(io, index1, index2, HWIO_FMSK(io, field), (uint32)(val) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTFI3
  #define HWIO_OUTFI3(io, index1, index2, index3, field, val)      HWIO_OUTMI3(io, index1, index2, index3, HWIO_FMSK(io, field), (uint32)(val) << HWIO_SHFT(io, field))
#endif

#ifndef HWIO_OUTV
  #define HWIO_OUTV(io, field, val)                                HWIO_OUTM(io, HWIO_FMSK(io, field), (uint32)(HWIO_VAL(io, field, val)) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTVI
  #define HWIO_OUTVI(io, index, field, val)                        HWIO_OUTMI(io, index, HWIO_FMSK(io, field), (uint32)(HWIO_VAL(io, field, val)) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTVI2
  #define HWIO_OUTVI2(io, index1, index2, field, val)              HWIO_OUTMI2(io, index1, index2, HWIO_FMSK(io, field), (uint32)(HWIO_VAL(io, field, val)) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTVI3
  #define HWIO_OUTVI3(io, index1, index2, index3, field, val)      HWIO_OUTMI3(io, index1, index2, index3, HWIO_FMSK(io, field), (uint32)(HWIO_VAL(io, field, val)) << HWIO_SHFT(io, field))
#endif

#ifndef HWIO_OUTX
  #define HWIO_OUTX(base, hwiosym, val)                                   __msmhwio_outx(base, hwiosym, val)
#endif
#ifndef HWIO_OUTXI
  #define HWIO_OUTXI(base, hwiosym, index, val)                           __msmhwio_outxi(base, hwiosym, index, val)
#endif
#ifndef HWIO_OUTXI2
  #define HWIO_OUTXI2(base, hwiosym, index1, index2, val)                 __msmhwio_outxi2(base, hwiosym, index1, index2, val)
#endif
#ifndef HWIO_OUTXI3
  #define HWIO_OUTXI3(base, hwiosym, index1, index2, index3, val)         __msmhwio_outxi3(base, hwiosym, index1, index2, index3, val)
#endif

#ifndef HWIO_OUTXM
  #define HWIO_OUTXM(base, hwiosym, mask, val)                            __msmhwio_outxm(base, hwiosym, mask, val)
#endif
#ifndef HWIO_OUTXMI
  #define HWIO_OUTXMI(base, hwiosym, index, mask, val)                    __msmhwio_outxmi(base, hwiosym, index, mask, val)
#endif
#ifndef HWIO_OUTXMI2
  #define HWIO_OUTXMI2(base, hwiosym, index1, index2, mask, val)          __msmhwio_outxmi2(base, hwiosym, index1, index2, mask, val)
#endif
#ifndef HWIO_OUTXMI3
  #define HWIO_OUTXMI3(base, hwiosym, index1, index2, index3, mask, val)  __msmhwio_outxmi3(base, hwiosym, index1, index2, index3, mask, val)
#endif

#ifndef HWIO_OUTXF
  #define HWIO_OUTXF(base, io, field, val)                                HWIO_OUTXM(base, io, HWIO_FMSK(io, field), (uint32)(val) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTXFI
  #define HWIO_OUTXFI(base, io, index, field, val)                        HWIO_OUTXMI(base, io, index, HWIO_FMSK(io, field), (uint32)(val) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTXFI2
  #define HWIO_OUTXFI2(base, io, index1, index2, field, val)              HWIO_OUTXMI2(base, io, index1, index2, HWIO_FMSK(io, field), (uint32)(val) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTXFI3
  #define HWIO_OUTXFI3(base, io, index1, index2, index3, field, val)      HWIO_OUTXMI3(base, io, index1, index2, index3, HWIO_FMSK(io, field), (uint32)(val) << HWIO_SHFT(io, field))
#endif

#ifndef HWIO_OUTXV
  #define HWIO_OUTXV(base, io, field, val)                                HWIO_OUTXM(base, io, HWIO_FMSK(io, field), (uint32)(HWIO_VAL(io, field, val)) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTXVI
  #define HWIO_OUTXVI(base, io, index, field, val)                        HWIO_OUTXMI(base, io, index, HWIO_FMSK(io, field), (uint32)(HWIO_VAL(io, field, val)) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTXVI2
  #define HWIO_OUTXVI2(base, io, index1, index2, field, val)              HWIO_OUTXMI2(base, io, index1, index2, HWIO_FMSK(io, field), (uint32)(HWIO_VAL(io, field, val)) << HWIO_SHFT(io, field))
#endif
#ifndef HWIO_OUTXVI3
  #define HWIO_OUTXVI3(base, io, index1, index2, index3, field, val)      HWIO_OUTXMI3(base, io, index1, index2, index3, HWIO_FMSK(io, field), (uint32)(HWIO_VAL(io, field, val)) << HWIO_SHFT(io, field))
#endif
/** @} */

/**
 * @name Shift and Mask Macros
 *
 * Macros for getting shift and mask values for fields and registers.
 *  HWIO_RMSK: The mask value for accessing an entire register.  For example:
 *             @code
 *             HWIO_RMSK(REG) -> 0xFFFFFFFF
 *             @endcode
 *  HWIO_RSHFT: The right-shift value for an entire register (rarely necessary).\n
 *  HWIO_SHFT: The right-shift value for accessing a field in a register.  For example:
 *             @code
 *             HWIO_SHFT(REG, FLD) -> 8
 *             @endcode
 *  HWIO_FMSK: The mask value for accessing a field in a register.  For example:
 *             @code
 *             HWIO_FMSK(REG, FLD) -> 0xFF00
 *             @endcode
 *  HWIO_VAL:  The value for a field in a register.  For example:
 *             @code
 *             HWIO_VAL(REG, FLD, ON) -> 0x1
 *             @endcode
 *  HWIO_FVAL: This macro takes a numerical value and will shift and mask it into
 *             the given field position.  For example:
 *             @code
 *             HWIO_FVAL(REG, FLD, 0x1) -> 0x100
 *             @endcode
 *  HWIO_FVALV: This macro takes a logical (named) value and will shift and mask it
 *              into the given field position.  For example:
 *              @code
 *              HWIO_FVALV(REG, FLD, ON) -> 0x100
 *              @endcode
 *
 * @{
 */
#ifndef HWIO_RMSK
  #define HWIO_RMSK(hwiosym)                               __msmhwio_rmsk(hwiosym)
#endif
#ifndef HWIO_RMSKI
  #define HWIO_RMSKI(hwiosym, index)                       __msmhwio_rmski(hwiosym, index)
#endif
#ifndef HWIO_RSHFT
  #define HWIO_RSHFT(hwiosym)                              __msmhwio_rshft(hwiosym)
#endif
#ifndef HWIO_SHFT
  #define HWIO_SHFT(hwio_regsym, hwio_fldsym)              __msmhwio_shft(hwio_regsym, hwio_fldsym)
#endif
#ifndef HWIO_FMSK
  #define HWIO_FMSK(hwio_regsym, hwio_fldsym)              __msmhwio_fmsk(hwio_regsym, hwio_fldsym)
#endif
#ifndef HWIO_VAL
  #define HWIO_VAL(io, field, val)                         __msmhwio_val(io, field, val)
#endif
#ifndef HWIO_FVAL
  #define HWIO_FVAL(io, field, val)                        (((uint32)(val) << HWIO_SHFT(io, field)) & HWIO_FMSK(io, field))
#endif
#ifndef HWIO_FVALV
  #define HWIO_FVALV(io, field, val)                       (((uint32)(HWIO_VAL(io, field, val)) << HWIO_SHFT(io, field)) & HWIO_FMSK(io, field))
#endif
/** @} */

/**
 * @name Shadow Register Macros
 *
 * These macros are used for directly reading the value stored in a 
 * shadow register.
 * Shadow registers are defined for write-only registers.  Generally these
 * macros should not be necessary as HWIO_OUTM* macros will automatically use
 * the shadow values internally.
 *
 * @{
 */
#ifndef HWIO_SHDW
  #define HWIO_SHDW(hwiosym)                               __msmhwio_shdw(hwiosym)
#endif
#ifndef HWIO_SHDWI
  #define HWIO_SHDWI(hwiosym, index)                       __msmhwio_shdwi(hwiosym, index)
#endif
/** @} */

/** 
  @}
*/ /* end_group */


/** @cond */

/*
 * Map to final symbols.  This remapping is done to allow register 
 * redefinitions.  If we just define HWIO_IN(xreg) as HWIO_##xreg##_IN
 * then remappings like "#ifndef xreg
  #define xreg xregnew" do not work as expected.
#endif
 */
#ifndef __msmhwio_in
  #define __msmhwio_in(hwiosym)                                   HWIO_##hwiosym##_IN
#endif
#ifndef __msmhwio_ini
  #define __msmhwio_ini(hwiosym, index)                           HWIO_##hwiosym##_INI(index)
#endif
#ifndef __msmhwio_ini2
  #define __msmhwio_ini2(hwiosym, index1, index2)                 HWIO_##hwiosym##_INI2(index1, index2)
#endif
#ifndef __msmhwio_ini3
  #define __msmhwio_ini3(hwiosym, index1, index2, index3)         HWIO_##hwiosym##_INI3(index1, index2, index3)
#endif
#ifndef __msmhwio_inm
  #define __msmhwio_inm(hwiosym, mask)                            HWIO_##hwiosym##_INM(mask)
#endif
#ifndef __msmhwio_inmi
  #define __msmhwio_inmi(hwiosym, index, mask)                    HWIO_##hwiosym##_INMI(index, mask)
#endif
#ifndef __msmhwio_inmi2
  #define __msmhwio_inmi2(hwiosym, index1, index2, mask)          HWIO_##hwiosym##_INMI2(index1, index2, mask)
#endif
#ifndef __msmhwio_inmi3
  #define __msmhwio_inmi3(hwiosym, index1, index2, index3, mask)  HWIO_##hwiosym##_INMI3(index1, index2, index3, mask)
#endif
#ifndef __msmhwio_out
  #define __msmhwio_out(hwiosym, val)                             HWIO_##hwiosym##_OUT(val)
#endif
#ifndef __msmhwio_outi
  #define __msmhwio_outi(hwiosym, index, val)                     HWIO_##hwiosym##_OUTI(index,val)
#endif
#ifndef __msmhwio_outi2
  #define __msmhwio_outi2(hwiosym, index1, index2, val)           HWIO_##hwiosym##_OUTI2(index1, index2, val)
#endif
#ifndef __msmhwio_outi3
  #define __msmhwio_outi3(hwiosym, index1, index2, index3, val)   HWIO_##hwiosym##_OUTI2(index1, index2, index3, val)
#endif
#ifndef __msmhwio_outm
  #define __msmhwio_outm(hwiosym, mask, val)                      HWIO_##hwiosym##_OUTM(mask, val)
#endif
#ifndef __msmhwio_outmi
  #define __msmhwio_outmi(hwiosym, index, mask, val)              HWIO_##hwiosym##_OUTMI(index, mask, val)
#endif
#ifndef __msmhwio_outmi2
  #define __msmhwio_outmi2(hwiosym, idx1, idx2, mask, val)        HWIO_##hwiosym##_OUTMI2(idx1, idx2, mask, val)
#endif
#ifndef __msmhwio_outmi3
  #define __msmhwio_outmi3(hwiosym, idx1, idx2, idx3, mask, val)  HWIO_##hwiosym##_OUTMI3(idx1, idx2, idx3, mask, val)
#endif
#ifndef __msmhwio_addr
  #define __msmhwio_addr(hwiosym)                                 HWIO_##hwiosym##_ADDR
#endif
#ifndef __msmhwio_addri
  #define __msmhwio_addri(hwiosym, index)                         HWIO_##hwiosym##_ADDR(index)
#endif
#ifndef __msmhwio_addri2
  #define __msmhwio_addri2(hwiosym, idx1, idx2)                   HWIO_##hwiosym##_ADDR(idx1, idx2)
#endif
#ifndef __msmhwio_addri3
  #define __msmhwio_addri3(hwiosym, idx1, idx2, idx3)             HWIO_##hwiosym##_ADDR(idx1, idx2, idx3)
#endif
#ifndef __msmhwio_phys
  #define __msmhwio_phys(hwiosym)                                 HWIO_##hwiosym##_PHYS
#endif
#ifndef __msmhwio_physi
  #define __msmhwio_physi(hwiosym, index)                         HWIO_##hwiosym##_PHYS(index)
#endif
#ifndef __msmhwio_physi2
  #define __msmhwio_physi2(hwiosym, idx1, idx2)                   HWIO_##hwiosym##_PHYS(idx1, idx2)
#endif
#ifndef __msmhwio_physi3
  #define __msmhwio_physi3(hwiosym, idx1, idx2, idx3)             HWIO_##hwiosym##_PHYS(idx1, idx2, idx3)
#endif
#ifndef __msmhwio_offs
  #define __msmhwio_offs(hwiosym)                                 HWIO_##hwiosym##_OFFS 
#endif
#ifndef __msmhwio_offsi
  #define __msmhwio_offsi(hwiosym, index)                         HWIO_##hwiosym##_OFFS(index)
#endif
#ifndef __msmhwio_offsi2
  #define __msmhwio_offsi2(hwiosym, idx1, idx2)                   HWIO_##hwiosym##_OFFS(idx1, idx2)
#endif
#ifndef __msmhwio_offsi3
  #define __msmhwio_offsi3(hwiosym, idx1, idx2, idx3)             HWIO_##hwiosym##_OFFS(idx1, idx2, idx3)
#endif
#ifndef __msmhwio_rmsk
  #define __msmhwio_rmsk(hwiosym)                                 HWIO_##hwiosym##_RMSK
#endif
#ifndef __msmhwio_rmski
  #define __msmhwio_rmski(hwiosym, index)                         HWIO_##hwiosym##_RMSK(index)
#endif
#ifndef __msmhwio_fmsk
  #define __msmhwio_fmsk(hwiosym, hwiofldsym)                     HWIO_##hwiosym##_##hwiofldsym##_BMSK
#endif
#ifndef __msmhwio_rshft
  #define __msmhwio_rshft(hwiosym)                                HWIO_##hwiosym##_SHFT
#endif
#ifndef __msmhwio_shft
  #define __msmhwio_shft(hwiosym, hwiofldsym)                     HWIO_##hwiosym##_##hwiofldsym##_SHFT
#endif
#ifndef __msmhwio_shdw
  #define __msmhwio_shdw(hwiosym)                                 HWIO_##hwiosym##_shadow
#endif
#ifndef __msmhwio_shdwi
  #define __msmhwio_shdwi(hwiosym, index)                         HWIO_##hwiosym##_SHDW(index)
#endif
#ifndef __msmhwio_val
  #define __msmhwio_val(hwiosym, hwiofld, hwioval)                HWIO_##hwiosym##_##hwiofld##_##hwioval##_FVAL
#endif

#ifndef __msmhwio_inx
  #define __msmhwio_inx(base, hwiosym)                                  HWIO_##hwiosym##_IN(base)
#endif
#ifndef __msmhwio_inxi
  #define __msmhwio_inxi(base, hwiosym, index)                          HWIO_##hwiosym##_INI(base, index)
#endif
#ifndef __msmhwio_inxi2
  #define __msmhwio_inxi2(base, hwiosym, index1, index2)                HWIO_##hwiosym##_INI2(base, index1, index2)
#endif
#ifndef __msmhwio_inxi3
  #define __msmhwio_inxi3(base, hwiosym, index1, index2, index3)        HWIO_##hwiosym##_INI3(base, index1, index2, index3)
#endif
#ifndef __msmhwio_inxm
  #define __msmhwio_inxm(base, hwiosym, mask)                           HWIO_##hwiosym##_INM(base, mask)
#endif
#ifndef __msmhwio_inxmi
  #define __msmhwio_inxmi(base, hwiosym, index, mask)                   HWIO_##hwiosym##_INMI(base, index, mask)
#endif
#ifndef __msmhwio_inxmi2
  #define __msmhwio_inxmi2(base, hwiosym, index1, index2, mask)         HWIO_##hwiosym##_INMI2(base, index1, index2, mask)
#endif
#ifndef __msmhwio_inxmi3
  #define __msmhwio_inxmi3(base, hwiosym, index1, index2, index3, mask) HWIO_##hwiosym##_INMI3(base, index1, index2, index3, mask)
#endif
#ifndef __msmhwio_outx
  #define __msmhwio_outx(base, hwiosym, val)                            HWIO_##hwiosym##_OUT(base, val)
#endif
#ifndef __msmhwio_outxi
  #define __msmhwio_outxi(base, hwiosym, index, val)                    HWIO_##hwiosym##_OUTI(base, index,val)
#endif
#ifndef __msmhwio_outxi2
  #define __msmhwio_outxi2(base, hwiosym, index1, index2, val)          HWIO_##hwiosym##_OUTI2(base, index1, index2, val)
#endif
#ifndef __msmhwio_outxi3
  #define __msmhwio_outxi3(base, hwiosym, index1, index2, index3, val)  HWIO_##hwiosym##_OUTI2(base, index1, index2, index3, val)
#endif
#ifndef __msmhwio_outxm
  #define __msmhwio_outxm(base, hwiosym, mask, val)                     HWIO_##hwiosym##_OUTM(base, mask, val)
#endif
#ifndef __msmhwio_outxmi
  #define __msmhwio_outxmi(base, hwiosym, index, mask, val)             HWIO_##hwiosym##_OUTMI(base, index, mask, val)
#endif
#ifndef __msmhwio_outxmi2
  #define __msmhwio_outxmi2(base, hwiosym, idx1, idx2, mask, val)       HWIO_##hwiosym##_OUTMI2(base, idx1, idx2, mask, val)
#endif
#ifndef __msmhwio_outxmi3
  #define __msmhwio_outxmi3(base, hwiosym, idx1, idx2, idx3, mask, val) HWIO_##hwiosym##_OUTMI3(base, idx1, idx2, idx3, mask, val)
#endif
#ifndef __msmhwio_addrx
  #define __msmhwio_addrx(base, hwiosym)                                HWIO_##hwiosym##_ADDR(base)
#endif
#ifndef __msmhwio_addrxi
  #define __msmhwio_addrxi(base, hwiosym, index)                        HWIO_##hwiosym##_ADDR(base, index)
#endif
#ifndef __msmhwio_addrxi2
  #define __msmhwio_addrxi2(base, hwiosym, idx1, idx2)                  HWIO_##hwiosym##_ADDR(base, idx1, idx2)
#endif
#ifndef __msmhwio_addrxi3
  #define __msmhwio_addrxi3(base, hwiosym, idx1, idx2, idx3)            HWIO_##hwiosym##_ADDR(base, idx1, idx2, idx3)
#endif
#ifndef __msmhwio_physx
  #define __msmhwio_physx(base, hwiosym)                                HWIO_##hwiosym##_PHYS(base)
#endif
#ifndef __msmhwio_physxi
  #define __msmhwio_physxi(base, hwiosym, index)                        HWIO_##hwiosym##_PHYS(base, index)
#endif
#ifndef __msmhwio_physxi2
  #define __msmhwio_physxi2(base, hwiosym, idx1, idx2)                  HWIO_##hwiosym##_PHYS(base, idx1, idx2)
#endif
#ifndef __msmhwio_physxi3
  #define __msmhwio_physxi3(base, hwiosym, idx1, idx2, idx3)            HWIO_##hwiosym##_PHYS(base, idx1, idx2, idx3)
#endif


/*
 * HWIO_INTLOCK
 *
 * Macro used by autogenerated code for mutual exclusion around
 * read-mask-write operations.  This is not supported in HAL
 * code but can be overridden by non-HAL code.
 */
#ifndef HWIO_INTLOCK
  #define HWIO_INTLOCK()
#endif
#ifndef HWIO_INTFREE
  #define HWIO_INTFREE()
#endif


/*
 * Input/output port macros for memory mapped IO.
 */
#ifndef __inp
  #define __inp(port)         (*((volatile uint8 *) (port)))
#endif
#ifndef __inpw
  #define __inpw(port)        (*((volatile uint16 *) (port)))
#endif
#ifndef __inpdw
  #define __inpdw(port)       (*((volatile uint32 *) (port)))
#endif
#ifndef __outp
  #define __outp(port, val)   (*((volatile uint8 *) (port)) = ((uint8) (val)))
#endif
#ifndef __outpw
  #define __outpw(port, val)  (*((volatile uint16 *) (port)) = ((uint16) (val)))
#endif
#ifndef __outpdw
  #define __outpdw(port, val) (*((volatile uint32 *) (port)) = ((uint32) (val)))
#endif


#ifdef HAL_HWIO_EXTERNAL

/*
 * Replace macros with externally supplied functions.
 */
#undef  __inp
#undef  __inpw
#undef  __inpdw
#undef  __outp
#undef  __outpw
#undef  __outpdw

#ifndef __inp
  #define  __inp(port)          __inp_extern(port)         
#endif
#ifndef __inpw
  #define  __inpw(port)         __inpw_extern(port)
#endif
#ifndef __inpdw
  #define  __inpdw(port)        __inpdw_extern(port)
#endif
#ifndef __outp
  #define  __outp(port, val)    __outp_extern(port, val)
#endif
#ifndef __outpw
  #define  __outpw(port, val)   __outpw_extern(port, val)
#endif
#ifndef __outpdw
  #define  __outpdw(port, val)  __outpdw_extern(port, val)
#endif

extern uint8   __inp_extern      ( uint32 nAddr );
extern uint16  __inpw_extern     ( uint32 nAddr );
extern uint32  __inpdw_extern    ( uint32 nAddr );
extern void    __outp_extern     ( uint32 nAddr, uint8  nData );
extern void    __outpw_extern    ( uint32 nAddr, uint16 nData );
extern void    __outpdw_extern   ( uint32 nAddr, uint32 nData );

#endif /* HAL_HWIO_EXTERNAL */


/*
 * Base 8-bit byte accessing macros.
 */
#ifndef in_byte
  #define in_byte(addr)               (__inp(addr))
#endif
#ifndef in_byte_masked
  #define in_byte_masked(addr, mask)  (__inp(addr) & (mask)) 
#endif
#ifndef out_byte
  #define out_byte(addr, val)         __outp(addr,val)
#endif
#ifndef out_byte_masked
  #define out_byte_masked(io, mask, val, shadow)  \
    HWIO_INTLOCK();    \
    out_byte( io, shadow); \
    shadow = (shadow & (uint16)(~(mask))) | ((uint16)((val) & (mask))); \
    HWIO_INTFREE()
#endif
#ifndef out_byte_masked_ns
  #define out_byte_masked_ns(io, mask, val, current_reg_content)  \
    out_byte( io, ((current_reg_content & (uint16)(~(mask))) | \
                  ((uint16)((val) & (mask)))) )
#endif

/*
 * Base 16-bit word accessing macros.
 */
#ifndef in_word
  #define in_word(addr)              (__inpw(addr))
#endif
#ifndef in_word_masked
  #define in_word_masked(addr, mask) (__inpw(addr) & (mask))
#endif
#ifndef out_word
  #define out_word(addr, val)        __outpw(addr,val)
#endif
#ifndef out_word_masked
  #define out_word_masked(io, mask, val, shadow)  \
    HWIO_INTLOCK( ); \
    shadow = (shadow & (uint16)(~(mask))) |  ((uint16)((val) & (mask))); \
    out_word( io, shadow); \
    HWIO_INTFREE( )
#endif
#ifndef out_word_masked_ns
  #define out_word_masked_ns(io, mask, val, current_reg_content)  \
    out_word( io, ((current_reg_content & (uint16)(~(mask))) | \
                  ((uint16)((val) & (mask)))) )
#endif

/*
 * Base 32-bit double-word accessing macros.
 */
#ifndef in_dword
  #define in_dword(addr)              (__inpdw(addr))
#endif
#ifndef in_dword_masked
  #define in_dword_masked(addr, mask) (__inpdw(addr) & (mask))
#endif
#ifndef out_dword
  #define out_dword(addr, val)        __outpdw(addr,val)
#endif
#ifndef out_dword_masked
  #define out_dword_masked(io, mask, val, shadow)  \
     HWIO_INTLOCK(); \
     shadow = (shadow & (uint32)(~(mask))) | ((uint32)((val) & (mask))); \
     out_dword( io, shadow); \
     HWIO_INTFREE()
#endif
#ifndef out_dword_masked_ns
  #define out_dword_masked_ns(io, mask, val, current_reg_content) \
    out_dword( io, ((current_reg_content & (uint32)(~(mask))) | \
                   ((uint32)((val) & (mask)))) )
#endif
/** @endcond */

#endif /* HAL_HWIO_H */

