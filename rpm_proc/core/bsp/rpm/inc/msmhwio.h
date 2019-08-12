/*============================================================================

FILE:      msmhwio.h

DESCRIPTION: hwio defintions

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

      Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential

$Header: //components/rel/rpm.bf/2.1.1/core/bsp/rpm/inc/msmhwio.h#1 $
$Date: 2015/02/25 $

============================================================================*/

#ifndef HWIO_H
#define HWIO_H
/*===========================================================================
 *
 * HWIO REGISTER INPUT/OUTPUT HEADER FILE
 *
 * DESCRIPTION
 * This interface is a replacement for that represented by 
 * msm<ASIC>reg.h, msm<ASIC>io.h and msm<ASIC>redefs.h. For further background
 * and documentation on this interface, please see word docs and ppt docs at 
 *
 * http://lvcrpprd.qualcomm.com/livelink/livelink?func=ll&objId=22836802&objAction=browse&sort=name
 *
 * This file, together with msmhwioreg.h, provides a portable interface
 * from the perspective of chip address file coupling to DMSS register
 * I/O software. The interface defined by msmhwio.h and msmhwioreg.h
 * encapsulates register name, size, type and access method.
 *
 * msmhwioreg.h is an auto-generated file that may be subsequently edited
 * by hand to handle arbitrary register and bit field name changes, and 
 * register type and size changes. When hand editing, one does not directly
 * hand edit msmhwioreg.h or msm<ASIC>reg.h except to cut/paste registers
 * or bitmasks from msm<ASIC>reg.h to msm<ASIC>reg_port.h or from msmhwioreg.h 
 * to msmhwioreg_port.h. One edits an msmreg.pl input file, possibly named
 * something like msmreg_port.dat, to add patterns which msmreg.pl matches 
 * during chip address file parsing. If a pattern from input the port.dat input
 * file is matched, that register or bitmask is not ouput to the register
 * header files, but is output to the *_port.h versions of the register header
 * files. This mechanism allows hand edit of register header file without loss
 * of hand edits on each successive re-parse of chip address file. See msmreg.pl
 * header comments for more information.
 * 
 * msmhwioreg.h also handles register access method changes by allowing users
 * to change a registers I/O functions on a per register basis. By default, 
 * when auto-generated, all register I/O functions are selected from the
 * inline functions listed below.
 *
 * (Note that currently inline functions are not used. This is because
 * rex_int_lock/rex_int_free are not inline and there the masked IO functions
 * would not inline since the compiler will not inline a function that calls
 * a function. The task of figuring out how to inline rex_int_lock has been
 * deferred. So, for the time being, the functions described below are
 * implemented as #define marcos.)
 *
 * To customize register I/O for a single register, one follows the porting
 * mechanism mentioned above and changes the input/output function for the 
 * desired register.
 * 
 * Direct, un-encapsulated, use of register addresses or register header file
 * components is discouraged. To that end, the interface described here fully
 * encapsulates all the CAF components including register address, bit field mask,
 * and bit field shift values in addition to register input and output.
 *
 * INLINE byte in_byte(dword addr) 
 * INLINE byte in_byte_masked(dword addr, dword mask) 
 * INLINE void out_byte(dword addr, byte val) 
 * INLINE void out_byte_masked(dword io, dword mask, byte val) 
 * 
 * INLINE word in_word(dword addr) 
 * INLINE word in_word_masked(dword addr, dword mask) 
 * INLINE void out_word(dword addr, word val) 
 * INLINE void out_word_masked(dword io, dword mask, word val) 
 * 
 * INLINE dword in_dword(dword addr) 
 * INLINE dword in_dword_masked(dword addr, dword mask) 
 * INLINE void out_dword(dword addr, dword val) 
 * INLINE void out_dword_masked(dword io, dword mask, dword val) 
 *
 * Initially, then there is a one-to-one correspondance between the inline
 * functions above and the macro interface below with the inline functions
 * intended as implementation detail of the macro interface. Register access
 * method changes of arbitrary complexity are handled by writing the 
 * appropriate new I/O function and replacing the existing one for the 
 * register under consideration.
 *
 * The interface described below takes chip address file (CAF) symbols as input.
 * Wherever hwiosym, hsio_regsym, or hwio_fldsym appears below, that is 
 * a CAF register or bit field name.
 * 
 * 
 * #define HWIO_IN(hwiosym)
 *    Perform input on register hwiosym. Replaces MSM_IN, MSM_INH and MSM_INW.
 *
 * #define HWIO_INM(hwiosym, m)
 *    Perform masked input on register hwiosym applying mask m. 
 *    Replaces MSM_INM, MSM_INHM, and MSM_INWM.
 *
 * #define HWIO_INF(hwio_regsym, hwio_fldsym)
 *    Read the value from the given field in the given register.
 *
 * #define HWIO_OUT(hwiosym, val)
 *    Write input val on register hwiosym. Replaces MSM_OUT, MSM_OUTH, and 
 *    MSM_OUTW.
 *
 * #define HWIO_OUTM(hwiosym, mask, val)
 *    Write input val on register hwiosym. Input mask is applied to shadow 
 *    memory and val such that only bits in mask are updated on register hwiosym
 *    and shadow memory. Replaces MSM_OUTM, MSM_OUTHM, and MSM_OUTWM.
 *
 * #define HWIO_OUTF(hwio_regsym, hwio_fldsym, val)
 *    Set the given field in the given register to the given value.
 *
 * #define HWIO_ADDR(hwiosym)
 *    Get the register address of hwiosym. Replaces the unencapulsated use of
 *    of numeric literal HEX constants for register address.
 *
 * #define HWIO_RMSK(hwiosym)
 *    Get the mask describing valid bits in register hwiosym. Replaces direct,
 *    unencapsulated use of mask symbol from register header file.
 *
 * #define HWIO_RSHFT(hwiosym)
 *    Get the shift value of the least significant bit in register hwiosym.a
 *    Replaces SHIFT_FROM_MASK.
 *
 * #define HWIO_FMSK(hwio_regsym, hwio_fldsym)
 *    Get the bit field mask for bit field hwio_fldsym in register hwio_regsym.
 *    Replaces the direct, unencapsulated use of bit field masks from register
 *    header file.
 *
 * #define HWIO_SHFT(hwio_regsym, hwio_fldsym)
 *    Get the shift value of the least significant bit of bit field hwio_fldsym
 *    in register hwio_regsym. Replaces SHIFT_FROM_MASK.
 * 
 * Copyright (c) 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 * Copyright (c) 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 * Copyright (c) 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *===========================================================================*/

/*===========================================================================
 *
 * EDIT HISTORY FOR FILE
 *
 * This section contains comments describing changes made to this file.
 * Notice that changes are listed in reverse chronological order.
 *
 * $Header: //components/rel/rpm.bf/2.1.1/core/bsp/rpm/inc/msmhwio.h#1 $
 *
 * when       who     what, where, why
 * --------   ---     ----------------------------------------------------------
 * 07/29/08   gfr     Support for KxMutex HWIO locking on QDSP6.
 * 07/02/08   gfr     Protect parameters with parenthesis.
 * 04/23/08   gfr     Added HWIO_FVAL macro.
 * 02/01/08   gfr     Wrap passed in mask with parenthesis.
 * 12/06/07   gfr     More depreciated MSM macros.
 * 11/19/07   gfr     Added MSM_R macro.
 * 11/19/07   gfr     Removed obselete hwio_interrupts_locked_by_user.
 * 10/26/07   gfr     Fix MSM_SET macro.
 * 10/10/07   gfr     Added mappings for MSM_R_MASKED and some others.
 * 09/21/07   gfr     Added mapping for MSM_W_MASKED_i.
 * 09/21/07   gfr     Merged in changes for WinCE and 7k.
 * 04/24/07   gfr     Support for HWIO_INTLOCK/INTFREE to remove lint warnings.
 * 04/09/07   gfr     Cleanup, compile support for HWIO_INTLOCK/HWIO_INTFREE.
 * 09/19/06   eav     Added HWIO_PHYS, HWIO_PHYSI, HWIO_PHYSI2
 * 08/28/06   gfr     Added HWIO_RMSKI, HWIO_SHDW, HWIO_SHDWI
 * 06/23/06   gfr     Added HWIO_INF, HWIO_INFI, HWIO_OUTF and HWIO_OUTFI
 * 06/16/05   eav     Changed  __msmhwio_addri to call HWIO_##hwiosym##_ADDRI
 * 10/10/02   aks     Map MSM_OUTSH() used by the MSM6200, to the equivalent
 *                    macro in the MSM6300.
 * 4/29/02    cr      initial revision 
 *===========================================================================*/


/*===========================================================================
 *
 *                         INCLUDE FILES
 *
 *===========================================================================*/
#ifndef SPS_BOOT_CHAIN 
#include "customer.h"
#endif
#ifdef FEATURE_WINCE_OAL
#include <windows.h>
#include <nkintr.h>
#endif

#ifndef _ARM_ASM_
#include "comdef.h"
#endif

#ifdef IMAGE_QDSP6_PROC
#include <quartz/KxMutex.h>
#endif



/*===========================================================================
 *
 *                         EXTERNAL DECLARATIONS
 *
 *===========================================================================*/

#ifndef _ARM_ASM_
extern dword rex_int_lock(void);
extern dword rex_int_free(void);
#endif

#ifdef IMAGE_QDSP6_PROC
extern KxMutex *hwio_mutex_ptr;
#endif


/*===========================================================================
 *
 *                         MACRO DECLARATIONS
 *
 *===========================================================================*/


#define HWIO_IN(hwiosym)                                 __msmhwio_in(hwiosym)
#define HWIO_INI(hwiosym, index)                         __msmhwio_ini(hwiosym, index)
#define HWIO_INI2(hwiosym, index1, index2)               __msmhwio_ini2(hwiosym, index1, index2)
#define HWIO_INM(hwiosym, mask)                          __msmhwio_inm(hwiosym, mask)
#define HWIO_INMI(hwiosym, index, mask)                  __msmhwio_inmi(hwiosym, index, mask)
#define HWIO_INMI2(hwiosym, index1, index2, mask)        __msmhwio_inmi2(hwiosym, index1, index2, mask)
#define HWIO_OUT(hwiosym, val)                           __msmhwio_out(hwiosym, val)
#define HWIO_OUTI(hwiosym, index, val)                   __msmhwio_outi(hwiosym, index, val)
#define HWIO_OUTI2(hwiosym, index1, index2, val)         __msmhwio_outi2(hwiosym, index1, index2, val)
#define HWIO_OUTM(hwiosym, mask, val)                    __msmhwio_outm(hwiosym, mask, val)
#define HWIO_OUTMI(hwiosym, index, mask, val)            __msmhwio_outmi(hwiosym, index, mask, val)
#define HWIO_OUTMI2(hwiosym, index1, index2, mask, val)  __msmhwio_outmi2(hwiosym, index1, index2, mask, val)
#define HWIO_ADDR(hwiosym)                               __msmhwio_addr(hwiosym)
#define HWIO_ADDRI(hwiosym, index)                       __msmhwio_addri(hwiosym, index)
#define HWIO_ADDRI2(hwiosym, index1, index2)             __msmhwio_addri2(hwiosym, index1, index2)
#define HWIO_RMSK(hwiosym)                               __msmhwio_rmsk(hwiosym)
#define HWIO_RMSKI(hwiosym, index)                       __msmhwio_rmski(hwiosym, index)
#define HWIO_RSHFT(hwiosym)                              __msmhwio_rshft(hwiosym)
#define HWIO_SHFT(hwio_regsym, hwio_fldsym)              __msmhwio_shft(hwio_regsym, hwio_fldsym)
#define HWIO_FMSK(hwio_regsym, hwio_fldsym)              __msmhwio_fmsk(hwio_regsym, hwio_fldsym)
#define HWIO_SHDW(hwiosym)                               __msmhwio_shdw(hwiosym)
#define HWIO_SHDWI(hwiosym, index)                       __msmhwio_shdwi(hwiosym, index)
#define HWIO_PHYS(hwiosym)                               __msmhwio_phys(hwiosym)
#define HWIO_PHYSI(hwiosym, index)                       __msmhwio_physi(hwiosym, index)
#define HWIO_PHYSI2(hwiosym, index1, index2)             __msmhwio_physi2(hwiosym, index1, index2)

#define HWIO_INF(io, field)                   (HWIO_INM(io, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#define HWIO_INFI(io, index, field)           (HWIO_INMI(io, index, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#define HWIO_INFI2(io, index1, index2, field) (HWIO_INMI2(io, index1, index2, HWIO_FMSK(io, field)) >> HWIO_SHFT(io, field))
#define HWIO_OUTF(io, field, val)                   HWIO_OUTM(io, HWIO_FMSK(io, field), (val) << HWIO_SHFT(io, field))
#define HWIO_OUTFI(io, index, field, val)           HWIO_OUTMI(io, index, HWIO_FMSK(io, field), (val) << HWIO_SHFT(io, field))
#define HWIO_OUTFI2(io, index1, index2, field, val) HWIO_OUTMI2(io, index1, index2, HWIO_FMSK(io, field), (val) << HWIO_SHFT(io, field))
#define HWIO_FVAL(io, field, val)             (((val) << HWIO_SHFT(io, field)) & HWIO_FMSK(io, field))

#define __msmhwio_in(hwiosym)               HWIO_##hwiosym##_IN
#define __msmhwio_ini(hwiosym, index)       HWIO_##hwiosym##_INI(index)
#define __msmhwio_ini2(hwiosym, index1, index2)  HWIO_##hwiosym##_INI2(index1, index2)
#define __msmhwio_inm(hwiosym, mask)          HWIO_##hwiosym##_INM(mask)
#define __msmhwio_inmi(hwiosym, index, mask)  HWIO_##hwiosym##_INMI(index, mask)
#define __msmhwio_inmi2(hwiosym, index1, index2, mask)  HWIO_##hwiosym##_INMI2(index1, index2, mask)
#define __msmhwio_out(hwiosym, val)         HWIO_##hwiosym##_OUT(val)
#define __msmhwio_outi(hwiosym, index, val) HWIO_##hwiosym##_OUTI(index,val)
#define __msmhwio_outi2(hwiosym, index1, index2, val) HWIO_##hwiosym##_OUTI2(index1, index2, val)
#define __msmhwio_outm(hwiosym, mask, val)  HWIO_##hwiosym##_OUTM(mask, val)
#define __msmhwio_outmi(hwiosym, index, mask, val) HWIO_##hwiosym##_OUTMI(index, mask, val)
#define __msmhwio_outmi2(hwiosym, index1, index2, mask, val) HWIO_##hwiosym##_OUTMI2(index1, index2, mask, val)

#define __msmhwio_shdw(hwiosym) HWIO_##hwiosym##_shadow
#define __msmhwio_shdwi(hwiosym, index) HWIO_##hwiosym##_SHDW(index)
#define __msmhwio_addr(hwiosym) HWIO_##hwiosym##_ADDR
#define __msmhwio_addri(hwiosym, index) HWIO_##hwiosym##_ADDR(index)
#define __msmhwio_addri2(hwiosym, index1, index2) HWIO_##hwiosym##_ADDR(index1, index2)
#define __msmhwio_rmsk(hwiosym) HWIO_##hwiosym##_RMSK
#define __msmhwio_rmski(hwiosym, index) HWIO_##hwiosym##_RMSK(index)
#define __msmhwio_fmsk(hwio_regsym, hwio_fldsym) HWIO_##hwio_regsym##_##hwio_fldsym##_BMSK
#define __msmhwio_rshft(hwio_regsym) HWIO_##hwio_regsym##_SHFT
#define __msmhwio_shft(hwio_regsym, hwio_fldsym) HWIO_##hwio_regsym##_##hwio_fldsym##_SHFT
#define __msmhwio_phys(hwiosym) HWIO_##hwiosym##_PHYS
#define __msmhwio_physi(hwiosym, index) HWIO_##hwiosym##_PHYS(index)
#define __msmhwio_physi2(hwiosym, index1, index2) HWIO_##hwiosym##_PHYS(index1, index2)


/*
 * MSM_x (depreciated)
 *
 * These macros should be replaced with the relevant HWIO equivalent.
 */
#define MSM_OUT(io, val)                HWIO_OUT(io, val)
#define MSM_IN(io)                      HWIO_IN(io)
#define MSM_OUTM(io, mask, val)         HWIO_OUTM(io, mask, val)
#define MSM_INM(io, mask)               HWIO_INM(io, mask) 
#define MSM_INF(io, field)              HWIO_INF(io, field)
#define MSM_FIELD(field, val)           (((val) << field##_SHFT) & (field##_BMSK))
#define MSM_OUTSH(io, mask, val)        HWIO_OUTM(io, mask, val)
#define MSM_FOUTSH(io, field, val)      HWIO_OUTM(io, mask, val)
#define MSM_SET(io, field)              HWIO_OUTM(io, HWIO_FMSK(io, field), HWIO_FMSK(io, field))
#define MSM_SET_BIT(io, bit)            HWIO_OUTM(io, (1<<bit), (1<<bit))
#define MSM_SET_i(io, field, index)     HWIO_OUTMI(io, index, HWIO_FMSK(io,field), HWIO_FMSK(io,field))
#define MSM_SET_ij(io, field, index1, index2) HWIO_OUTMI2(io, index1, index2, HWIO_FMSK(io,field), HWIO_FMSK(io,field))
#define MSM_CLEAR(io, field)            HWIO_OUTM(io, HWIO_FMSK(io, field), 0)
#define MSM_CLEAR_BIT(io, bit)          HWIO_OUTM(io, (1<<bit), 0)
#define MSM_CLEAR_i(io, field, n)       HWIO_OUTMI(io, n, HWIO_FMSK(io,field), 0)
#define MSM_CLEAR_ij(io, field, m, n)   HWIO_OUTMI2(io, m, n, HWIO_FMSK(io,field), 0)
#define MSM_SET_MASK(io, mask)          HWIO_OUTM(io, mask, mask)
#define MSM_CLEAR_MASK(io, mask)        HWIO_OUTM(io, mask, 0)
#define MSM_PULSE(io, field)            HWIO_OUTF(io, field, 1); HWIO_OUTF(io, field, 0)
#define MSM_GET(io, field)              HWIO_INF(io, field)
#define MSM_W(io, val)                  HWIO_OUT(io, val)
#define MSM_W_i(io, val, index)         HWIO_OUTI(io, index, val)
#define MSM_W_ij(io, val, index1, index2) HWIO_OUTI2(io, index1, index2, val )
#define MSM_W_MASKED(io, field, val)    HWIO_OUTF(io, field, val)
#define MSM_W_MASKED_i(io, field, val, index)            HWIO_OUTFI(io, index, field, val)
#define MSM_W_MASKED_ij(io, field, val, index1, index2)  HWIO_OUTFI2(io, index1, index2, field, val)
#define MSM_R(io)                       HWIO_IN(io)
#define MSM_R_MASKED(base, field)       HWIO_INF(base, field)
#define MSM_R_MASKED_i(base, field, n)  HWIO_INFI(base, n, field)
#define MSM_R_MASKED_ij(base, field, index1, index2)  HWIO_INFI2(base, index1, index2, field)
#define MSM_GET_BIT(io, bit)            (HWIO_INM(io, (1<<bit) ) >> bit) 
#define MSM_NOT_USED( i )               if(i) {}


/*
 * MSM_LOCK / MSM_UNLOCK (depreciated)
 *
 * These macros are intended to be used if a client will be issuing a
 * series of HWIO writes to avoid the multiple locking/freeing of interrupts
 * that will otherwise occur.
 * They should be replaced with HWIO_LOCK / HWIO_UNLOCK
 */
#define MSM_LOCK_REQUIRED                 \
  /*lint -save -e548 else expected*/      \
  MSM_LOCK_required = MSM_LOCK_required;  \
  /*lint -restore */

#define MSM_LOCK( )                   \
  {                                   \
    boolean MSM_LOCK_required = TRUE; \
    uint32 msm_sav;                   \
    INTLOCK_SAV(msm_sav)

#define MSM_UNLOCK( )      \
    MSM_LOCK_REQUIRED      \
    INTFREE_SAV(msm_sav);  \
  }

#define MSM_UNLOCK_AND_LOCK( ) \
    MSM_LOCK_REQUIRED          \
    INTFREE_SAV( msm_sav ),    \
    INTLOCK_SAV( msm_sav )


/*
 * INPxx / OUTPxx (depreciated)
 *
 */
#define INP32(addr)          in_dword(addr)
#define INP32M(addr, mask)   in_dword_masked(addr,mask)
#define OUTP32(addr, val)    out_dword(addr, val)
#define OUTP32M(addr, mask, val)                      \
   HWIO_INTLOCK();                                    \
   out_dword_masked_ns(addr, mask, val, INP32(addr)); \
   HWIO_INTFREE()


/*
 * HWIO_LOCK / HWIO_UNLOCK
 *
 * These macros are intended to be used if a client will be issuing a
 * series of HWIO writes to avoid the multiple locking/freeing of interrupts
 * that will otherwise occur.
 */
#define HWIO_LOCK()    \
  {                    \
    uint16 hwio_sav;   \
    INTLOCK_SAV(hwio_sav)

#define HWIO_UNLOCK()       \
    INTFREE_SAV(hwio_sav);  \
  }


#if defined(FEATURE_WINCE_BOOTLOADER)

  #define HWIO_INTLOCK()

  #define HWIO_INTFREE()

#elif defined(FEATURE_WINCE_OAL)

  #define HWIO_INTLOCK() \
   { \
     uint32 ints_already_enabled = INTERRUPTS_ENABLE(FALSE)
  
  #define HWIO_INTFREE() \
     if (ints_already_enabled) INTERRUPTS_ENABLE(TRUE); \
   }

#elif defined(IMAGE_QDSP6_PROC)

  #define HWIO_INTLOCK()              \
   {                                  \
     KxMutex_Lock(hwio_mutex_ptr);
  
  #define HWIO_INTFREE()              \
     KxMutex_Unlock(hwio_mutex_ptr);  \
   }

#else

  #define HWIO_INTLOCK()     \
   {                         \
     uint32 intlock_sav;     \
     INTLOCK_SAV(intlock_sav)
  
  #define HWIO_INTFREE()       \
     INTFREE_SAV(intlock_sav); \
   }

#endif


#define __inp(port)         (*((volatile uint8 *) (port)))
#define __inpw(port)        (*((volatile uint16 *) (port)))
#define __inpdw(port)       (*((volatile uint32 *) (port)))
#define inpdw(port) __inpdw(port)

#define __outp(port, val)   (*((volatile uint8 *) (port)) = ((uint8) (val)))
#define __outpw(port, val)  (*((volatile uint16 *) (port)) = ((uint16) (val)))
#define __outpdw(port, val) (*((volatile uint32 *) (port)) = ((uint32) (val)))
#define outpdw(port, val) __outpdw(port, val)

#define in_byte(addr)               (__inp(addr))
#define in_byte_masked(addr, mask)  (__inp(addr) & (mask)) 
#define out_byte(addr, val)         __outp(addr,val)
#define out_byte_masked(io, mask, val, shadow)  \
  HWIO_INTLOCK();    \
  (void) out_byte( io, shadow); \
  shadow = (shadow & (uint16)(~(mask))) | ((uint16)((val) & (mask))); \
  HWIO_INTFREE()
#define out_byte_masked_ns(io, mask, val, current_reg_content)  \
  (void) out_byte( io, ((current_reg_content & (uint16)(~(mask))) | \
                       ((uint16)((val) & (mask)))) )

#define in_word(addr)              (__inpw(addr))
#define in_word_masked(addr, mask) (__inpw(addr) & (mask))
#define out_word(addr, val)        __outpw(addr,val)
#define out_word_masked(io, mask, val, shadow)  \
  HWIO_INTLOCK( ); \
  shadow = (shadow & (uint16)(~(mask))) |  ((uint16)((val) & (mask))); \
  (void) out_word( io, shadow); \
  HWIO_INTFREE( )
#define out_word_masked_ns(io, mask, val, current_reg_content)  \
  (void) out_word( io, ((current_reg_content & (uint16)(~(mask))) | \
                       ((uint16)((val) & (mask)))) )

#define in_dword(addr)              (__inpdw(addr))
#define in_dword_masked(addr, mask) (__inpdw(addr) & (mask))
#define out_dword(addr, val)        __outpdw(addr,val)
#define out_dword_masked(io, mask, val, shadow)  \
   HWIO_INTLOCK( ); \
   shadow = (shadow & (uint32)(~(mask))) | ((uint32)((val) & (mask))); \
   (void) out_dword( io, shadow); \
   HWIO_INTFREE( )
#define out_dword_masked_ns(io, mask, val, current_reg_content) \
  (void) out_dword( io, ((current_reg_content & (uint32)(~(mask))) | ((uint32)((val) & (mask)))) )


#endif /* HWIO_H */

