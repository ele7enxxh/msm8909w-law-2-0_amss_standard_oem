#ifndef __HALBIMCHWIO_H__
#define __HALBIMCHWIO_H__
/*
===========================================================================

FILE:         HALbimcHwio.h

DESCRIPTION:  BIMC HWIO definitions.

===========================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/common/HALbimcHwio.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------- 
08/03/11   sds     Created.

===========================================================================
             Copyright (c) 2015 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                    QUALCOMM Proprietary
===========================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/
/* Standard IN/OUT/MASK/SHIFT macros */
#define BIMC_IN(base,regsym)                      __bimchwio_in(base,regsym)
#define BIMC_INI(base,regsym,index)               __bimchwio_ini(base,regsym,index)
#define BIMC_INI2(base,regsym,in1,in2)            __bimchwio_ini2(base,regsym,in1,in2)
#define BIMC_INM(base,regsym,mask)                __bimchwio_inm(base,regsym,mask)
#define BIMC_INMI(base,regsym,index,mask)         __bimchwio_inmi(base,regsym,index,mask)
#define BIMC_INMI2(base,regsym,in1,in2,mask)      __bimchwio_inmi2(base,regsym,in1,in2,mask)
#define BIMC_OUT(base,regsym,val)                 __bimchwio_out(base,regsym,val)
#define BIMC_OUTI(base,regsym,index,val)          __bimchwio_outi(base,regsym,index,val)
#define BIMC_OUTI2(base,regsym,in1,in2,val)       __bimchwio_outi2(base,regsym,in1,in2,val)
#define BIMC_OUTM(base,regsym,mask,val)           __bimchwio_outm(base,regsym,mask,val)
#define BIMC_OUTMI(base,regsym,index,mask,val)    __bimchwio_outmi(base,regsym,index,mask,val)
#define BIMC_OUTMI2(base,regsym,in1,in2,mask,val) __bimchwio_outmi2(base,regsym,in1,in2,mask,val)
#define BIMC_ADDR(base,regsym)                    __bimchwio_addr(base,regsym)
#define BIMC_ADDRI(base,regsym,index)             __bimchwio_addri(base,regsym,index)
#define BIMC_RMSK(regsym)                         __bimchwio_rmsk(regsym)
#define BIMC_RSHFT(regsym)                        __bimchwio_rshft(regsym)
#define BIMC_SHFT(regsym, fldsym)                 __bimchwio_shft(regsym, fldsym)
#define BIMC_FMSK(regsym, fldsym)                 __bimchwio_fmsk(regsym, fldsym)

/* Field manipulation macros */
#define BIMC_INF(base,regsym,field)                (BIMC_INM(base,regsym,BIMC_FMSK(regsym,field)) >> BIMC_SHFT(regsym,field))
#define BIMC_INFI(base,regsym,index,field)         (BIMC_INMI(base,regsym,index,BIMC_FMSK(regsym,field)) >> BIMC_SHFT(regsym,field))
#define BIMC_INFI2(base,regsym,in1,in2,field)      (BIMC_INMI2(base,regsym,in1,in2,BIMC_FMSK(regsym,field)) >> BIMC_SHFT(regsym,field))
#define BIMC_OUTF(base,regsym,field,val)           BIMC_OUTM(base,regsym,BIMC_FMSK(regsym, field), val << BIMC_SHFT(regsym, field))
#define BIMC_OUTFI(base,regsym,index,field,val)    BIMC_OUTMI(base,regsym,index,BIMC_FMSK(regsym, field), val << BIMC_SHFT(regsym, field))
#define BIMC_OUTFI2(base,regsym,in1,in2,field,val) BIMC_OUTMI2(base,regsym,in1,in2,BIMC_FMSK(regsym, field), val << BIMC_SHFT(regsym, field))

/* Cached field manipulation macro */
#define BIMC_INFC(regval,regsym,field)           ((regval & BIMC_FMSK(regsym,field)) >> BIMC_SHFT(regsym,field))
#define BIMC_OUTFC(fval,regsym,field)            ((fval << BIMC_SHFT(regsym,field)) & BIMC_FMSK(regsym,field))

#define __bimchwio_in(base,regsym)                      BIMC_##regsym##_IN(base)
#define __bimchwio_ini(base,regsym,index)               BIMC_##regsym##_INI(base,index)
#define __bimchwio_ini2(base,regsym,in1,in2)            BIMC_##regsym##_INI2(base,in1,in2)
#define __bimchwio_inm(base,regsym,mask)                BIMC_##regsym##_INM(base,mask)
#define __bimchwio_inmi(base,regsym,index,mask)         BIMC_##regsym##_INMI(base,index,mask)
#define __bimchwio_inmi2(base,regsym,in1,in2,mask)      BIMC_##regsym##_INMI2(base,in1,in2,mask)
#define __bimchwio_out(base,regsym,val)                 BIMC_##regsym##_OUT(base,val)
#define __bimchwio_outi(base,regsym,index,val)          BIMC_##regsym##_OUTI(base,index,val)
#define __bimchwio_outi2(base,regsym,in1,in2,val)       BIMC_##regsym##_OUTI2(base,in1,in2,val)
#define __bimchwio_outm(base,regsym,mask,val)           BIMC_##regsym##_OUTM(base,mask,val)
#define __bimchwio_outmi(base,regsym,index,mask,val)    BIMC_##regsym##_OUTMI(base,index,mask,val)
#define __bimchwio_outmi2(base,regsym,in1,in2,mask,val) BIMC_##regsym##_OUTMI2(base,in1,in2,mask,val)
#define __bimchwio_addr(base,regsym)                    BIMC_##regsym##_ADDR(base)
#define __bimchwio_addri(base,regsym,index)             BIMC_##regsym##_ADDR(base,index)
#define __bimchwio_rmsk(regsym)                         BIMC_##regsym##_RMSK
#define __bimchwio_fmsk(regsym, fldsym)                 BIMC_##regsym##_##fldsym##_BMSK
#define __bimchwio_rshft(regsym)                        BIMC_##regsym##_SHFT
#define __bimchwio_shft(regsym, fldsym)                 BIMC_##regsym##_##fldsym##_SHFT

#define HWIO_INTLOCK()
#ifndef INTLOCK
  #define INTLOCK()
#endif

#define HWIO_INTFREE()
#ifndef INTFREE
  #define INTFREE()
#endif

#define __inp(port)         (*((volatile unsigned char *) (port)))
#define __inpw(port)        (*((volatile unsigned short *) (port)))
#define __inpdw(port)       (*((volatile unsigned int *) (port)))

#define __outp(port, val)   (*((volatile unsigned char *) (port)) = ((unsigned char) (val)))
#define __outpw(port, val)  (*((volatile unsigned short *) (port)) = ((unsigned short) (val)))
#define __outpdw(port, val) (*((volatile unsigned int *) (port)) = ((unsigned int) (val)))


#define in_byte(addr)               (__inp(addr))
#define in_byte_masked(addr, mask)  (__inp(addr) & (mask)) 
#define out_byte(addr, val)         __outp(addr,val)
#define out_byte_masked(io, mask, val, shadow)  \
  HWIO_INTLOCK();    \
  (void) out_byte( io, shadow); \
  shadow = (shadow & (unsigned short)(~(mask))) | ((unsigned short)((val) & (mask))); \
  HWIO_INTFREE()
#define out_byte_masked_ns(io, mask, val, current_reg_content)  \
  (void) out_byte( io, ((current_reg_content & (unsigned short)(~(mask))) | \
                       ((unsigned short)((val) & (mask)))) )

#define in_word(addr)              (__inpw(addr))
#define in_word_masked(addr, mask) (__inpw(addr) & (mask))
#define out_word(addr, val)        __outpw(addr,val)
#define out_word_masked(io, mask, val, shadow)  \
  HWIO_INTLOCK( ); \
  shadow = (shadow & (unsigned short)(~(mask))) |  ((unsigned short)((val) & (mask))); \
  (void) out_word( io, shadow); \
  HWIO_INTFREE( )
#define out_word_masked_ns(io, mask, val, current_reg_content)  \
  (void) out_word( io, ((current_reg_content & (unsigned short)(~(mask))) | \
                       ((unsigned short)((val) & (mask)))) )

#define in_dword(addr)              (__inpdw(addr))
#define in_dword_masked(addr, mask) (__inpdw(addr) & (mask))
#define out_dword(addr, val)        __outpdw(addr,val)
#define out_dword_masked(io, mask, val, shadow)  \
   HWIO_INTLOCK( ); \
   shadow = (shadow & (unsigned int)(~(mask))) | ((unsigned int)((val) & (mask))); \
   (void) out_dword( io, shadow); \
   HWIO_INTFREE( )
#define out_dword_masked_ns(io, mask, val, current_reg_content) \
  (void) out_dword( io, ((current_reg_content & (unsigned int)(~(mask))) | ((unsigned int)((val) & (mask)))) )

#endif /* __HALBIMCHWIO_H__ */

