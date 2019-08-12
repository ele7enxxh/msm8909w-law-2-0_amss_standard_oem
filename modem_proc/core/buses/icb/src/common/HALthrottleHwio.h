#ifndef __HALTHROTTLEHWIO_H__
#define __HALTHROTTLEHWIO_H__
/*
===========================================================================

FILE:         HALthrottleHwio.h

DESCRIPTION:  THROTTLE HWIO definitions.

===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/HALthrottleHwio.h#1 $

when         who     what, where, why
----------   ---     -------------------------------------------------------- 
2015/05/02   sds     Created.

===========================================================================
        Copyright (c) 2015 QUALCOMM Technologies Incorporated.
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
#define THROTTLE_IN(base,regsym)                               __throttlehwio_in(base,regsym)
#define THROTTLE_INI(base,regsym,index)                        __throttlehwio_ini(base,regsym,index)
#define THROTTLE_INI2(base,regsym,in1,in2)                     __throttlehwio_ini2(base,regsym,in1,in2)
#define THROTTLE_INM(base,regsym,mask)                         __throttlehwio_inm(base,regsym,mask)
#define THROTTLE_INMI(base,regsym,index,mask)                  __throttlehwio_inmi(base,regsym,index,mask)
#define THROTTLE_INMI2(base,regsym,in1,in2,mask)               __throttlehwio_inmi2(base,regsym,in1,in2,mask)
#define THROTTLE_OUT(base,regsym,val)                          __throttlehwio_out(base,regsym,val)
#define THROTTLE_OUTI(base,regsym,index,val)                   __throttlehwio_outi(base,regsym,index,val)
#define THROTTLE_OUTI2(base,regsym,in1,in2,val)                __throttlehwio_outi2(base,regsym,in1,in2,val)
#define THROTTLE_OUTM(base,regsym,mask,val)                    __throttlehwio_outm(base,regsym,mask,val)
#define THROTTLE_OUTMI(base,regsym,index,mask,val)             __throttlehwio_outmi(base,regsym,index,mask,val)
#define THROTTLE_OUTMI2(base,regsym,in1,in2,mask,val)          __throttlehwio_outmi2(base,regsym,in1,in2,mask,val)
#define THROTTLE_ADDR(base,regsym)                             __throttlehwio_addr(base,regsym)
#define THROTTLE_ADDRI(base,regsym,index)                      __throttlehwio_addri(base,regsym,index)
#define THROTTLE_RMSK(regsym)                                  __throttlehwio_rmsk(regsym)
#define THROTTLE_RSHFT(regsym)                                 __throttlehwio_rshft(regsym)
#define THROTTLE_SHFT(regsym, fldsym)                          __throttlehwio_shft(regsym, fldsym)
#define THROTTLE_FMSK(regsym, fldsym)                          __throttlehwio_fmsk(regsym, fldsym)

/* Field manipulation macros */
#define THROTTLE_INF(base,regsym,field)                         (THROTTLE_INM(base,regsym,THROTTLE_FMSK(regsym,field)) >> THROTTLE_SHFT(regsym,field))
#define THROTTLE_INFI(base,regsym,index,field)                  (THROTTLE_INMI(base,regsym,index,THROTTLE_FMSK(regsym,field)) >> THROTTLE_SHFT(regsym,field))
#define THROTTLE_INFI2(base,regsym,in1,in2,field)               (THROTTLE_INMI2(base,regsym,in1,in2,THROTTLE_FMSK(regsym,field)) >> THROTTLE_SHFT(regsym,field))
#define THROTTLE_OUTF(base,regsym,field,val)                    THROTTLE_OUTM(base,regsym,THROTTLE_FMSK(regsym, field), val << THROTTLE_SHFT(regsym, field))
#define THROTTLE_OUTFI(base,regsym,index,field,val)             THROTTLE_OUTMI(base,regsym,index,THROTTLE_FMSK(regsym, field), val << THROTTLE_SHFT(regsym, field))
#define THROTTLE_OUTFI2(base,regsym,in1,in2,field,val)          THROTTLE_OUTMI2(base,regsym,in1,in2,THROTTLE_FMSK(regsym, field), val << THROTTLE_SHFT(regsym, field))

/* Cached field manipulation macro */
#define THROTTLE_INFC(regval,regsym,field)           ((regval & THROTTLE_FMSK(regsym,field)) >> THROTTLE_SHFT(regsym,field))

#define __throttlehwio_in(base,regsym)                               THROTTLE_##regsym##_IN(base)
#define __throttlehwio_ini(base,regsym,index)                        THROTTLE_##regsym##_INI(base,index)
#define __throttlehwio_ini2(base,regsym,in1,in2)                     THROTTLE_##regsym##_INI2(base,in1,in2)
#define __throttlehwio_inm(base,regsym,mask)                         THROTTLE_##regsym##_INM(base,mask)
#define __throttlehwio_inmi(base,regsym,index,mask)                  THROTTLE_##regsym##_INMI(base,index,mask)
#define __throttlehwio_inmi2(base,regsym,in1,in2,mask)               THROTTLE_##regsym##_INMI2(base,in1,in2,mask)
#define __throttlehwio_out(base,regsym,val)                          THROTTLE_##regsym##_OUT(base,val)
#define __throttlehwio_outi(base,regsym,index,val)                   THROTTLE_##regsym##_OUTI(base,index,val)
#define __throttlehwio_outi2(base,regsym,in1,in2,val)                THROTTLE_##regsym##_OUTI2(base,in1,in2,val)
#define __throttlehwio_outm(base,regsym,mask,val)                    THROTTLE_##regsym##_OUTM(base,mask,val)
#define __throttlehwio_outmi(base,regsym,index,mask,val)             THROTTLE_##regsym##_OUTMI(base,index,mask,val)
#define __throttlehwio_outmi2(base,regsym,in1,in2,mask,val)          THROTTLE_##regsym##_OUTMI2(base,in1,in2,mask,val)
#define __throttlehwio_addr(base,regsym)                             THROTTLE_##regsym##_ADDR(base)
#define __throttlehwio_addri(base,regsym,index)                      THROTTLE_##regsym##_ADDR(base,index)
#define __throttlehwio_rmsk(regsym)                                  THROTTLE_##regsym##_RMSK
#define __throttlehwio_fmsk(regsym, fldsym)                          THROTTLE_##regsym##_##fldsym##_BMSK
#define __throttlehwio_rshft(regsym)                                 THROTTLE_##regsym##_SHFT
#define __throttlehwio_shft(regsym, fldsym)                          THROTTLE_##regsym##_##fldsym##_SHFT

#define HWIO_INTLOCK()
#define INTLOCK()

#define HWIO_INTFREE()
#define INTFREE()

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

#endif /* __HALTHROTTLEHWIO_H__ */
