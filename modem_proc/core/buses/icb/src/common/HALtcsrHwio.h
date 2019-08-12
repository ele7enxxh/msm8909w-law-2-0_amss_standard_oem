#ifndef __HALTCSRHWIO_H__
#define __HALTCSRHWIO_H__
/*
===========================================================================

FILE:         HALtcsrHwio.h

DESCRIPTION:  TCSR HWIO definitions.

===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/HALtcsrHwio.h#1 $

when         who     what, where, why
----------   ---     -------------------------------------------------------- 
2014/02/07   sds     Created.

===========================================================================
        Copyright (c) 2014 QUALCOMM Technologies Incorporated.
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
#define TCSR_IN(base,regsym)                               __tcsrhwio_in(base,regsym)
#define TCSR_INI(base,offset,size,regsym,index)            __tcsrhwio_ini(base,offset,size,regsym,index)
#define TCSR_INI2(base,regsym,in1,in2)                     __tcsrhwio_ini2(base,regsym,in1,in2)
#define TCSR_INM(base,regsym,mask)                         __tcsrhwio_inm(base,regsym,mask)
#define TCSR_INMI(base,offset,size,regsym,index,mask)      __tcsrhwio_inmi(base,offset,size,regsym,index,mask)
#define TCSR_INMI2(base,regsym,in1,in2,mask)               __tcsrhwio_inmi2(base,regsym,in1,in2,mask)
#define TCSR_OUT(base,regsym,val)                          __tcsrhwio_out(base,regsym,val)
#define TCSR_OUTI(base,offset,size,regsym,index,val)       __tcsrhwio_outi(base,offset,size,regsym,index,val)
#define TCSR_OUTI2(base,regsym,in1,in2,val)                __tcsrhwio_outi2(base,regsym,in1,in2,val)
#define TCSR_OUTM(base,regsym,mask,val)                    __tcsrhwio_outm(base,regsym,mask,val)
#define TCSR_OUTMI(base,offset,size,regsym,index,mask,val) __tcsrhwio_outmi(base,offset,size,regsym,index,mask,val)
#define TCSR_OUTMI2(base,regsym,in1,in2,mask,val)          __tcsrhwio_outmi2(base,regsym,in1,in2,mask,val)
#define TCSR_ADDR(base,regsym)                             __tcsrhwio_addr(base,regsym)
#define TCSR_ADDRI(base,offset,size,regsym,index)          __tcsrhwio_addri(base,offset,size,regsym,index)
#define TCSR_RMSK(regsym)                                  __tcsrhwio_rmsk(regsym)
#define TCSR_RSHFT(regsym)                                 __tcsrhwio_rshft(regsym)
#define TCSR_SHFT(regsym, fldsym)                          __tcsrhwio_shft(regsym, fldsym)
#define TCSR_FMSK(regsym, fldsym)                          __tcsrhwio_fmsk(regsym, fldsym)

/* Field manipulation macros */
#define TCSR_INF(base,regsym,field)                         (TCSR_INM(base,regsym,TCSR_FMSK(regsym,field)) >> TCSR_SHFT(regsym,field))
#define TCSR_INFI(base,offset,size,regsym,index,field)      (TCSR_INMI(base,offset,size,regsym,index,TCSR_FMSK(regsym,field)) >> TCSR_SHFT(regsym,field))
#define TCSR_INFI2(base,regsym,in1,in2,field)               (TCSR_INMI2(base,regsym,in1,in2,TCSR_FMSK(regsym,field)) >> TCSR_SHFT(regsym,field))
#define TCSR_OUTF(base,regsym,field,val)                    TCSR_OUTM(base,regsym,TCSR_FMSK(regsym, field), val << TCSR_SHFT(regsym, field))
#define TCSR_OUTFI(base,offset,size,regsym,index,field,val) TCSR_OUTMI(base,offset,size,regsym,index,TCSR_FMSK(regsym, field), val << TCSR_SHFT(regsym, field))
#define TCSR_OUTFI2(base,regsym,in1,in2,field,val)          TCSR_OUTMI2(base,regsym,in1,in2,TCSR_FMSK(regsym, field), val << TCSR_SHFT(regsym, field))

/* Cached field manipulation macro */
#define TCSR_INFC(regval,regsym,field)           ((regval & TCSR_FMSK(regsym,field)) >> TCSR_SHFT(regsym,field))

#define __tcsrhwio_in(base,regsym)                               TCSR_##regsym##_IN(base)
#define __tcsrhwio_ini(base,offset,size,regsym,index)            TCSR_##regsym##_INI(base,offset,size,index)
#define __tcsrhwio_ini2(base,regsym,in1,in2)                     TCSR_##regsym##_INI2(base,in1,in2)
#define __tcsrhwio_inm(base,regsym,mask)                         TCSR_##regsym##_INM(base,mask)
#define __tcsrhwio_inmi(base,offset,size,regsym,index,mask)      TCSR_##regsym##_INMI(base,offset,size,index,mask)
#define __tcsrhwio_inmi2(base,regsym,in1,in2,mask)               TCSR_##regsym##_INMI2(base,in1,in2,mask)
#define __tcsrhwio_out(base,regsym,val)                          TCSR_##regsym##_OUT(base,val)
#define __tcsrhwio_outi(base,offset,size,regsym,index,val)       TCSR_##regsym##_OUTI(base,offset,size,index,val)
#define __tcsrhwio_outi2(base,regsym,in1,in2,val)                TCSR_##regsym##_OUTI2(base,in1,in2,val)
#define __tcsrhwio_outm(base,regsym,mask,val)                    TCSR_##regsym##_OUTM(base,mask,val)
#define __tcsrhwio_outmi(base,offset,size,regsym,index,mask,val) TCSR_##regsym##_OUTMI(base,offset,size,index,mask,val)
#define __tcsrhwio_outmi2(base,regsym,in1,in2,mask,val)          TCSR_##regsym##_OUTMI2(base,in1,in2,mask,val)
#define __tcsrhwio_addr(base,regsym)                             TCSR_##regsym##_ADDR(base)
#define __tcsrhwio_addri(base,offset,size,regsym,index)          TCSR_##regsym##_ADDR(base,offset,size,index)
#define __tcsrhwio_rmsk(regsym)                                  TCSR_##regsym##_RMSK
#define __tcsrhwio_fmsk(regsym, fldsym)                          TCSR_##regsym##_##fldsym##_BMSK
#define __tcsrhwio_rshft(regsym)                                 TCSR_##regsym##_SHFT
#define __tcsrhwio_shft(regsym, fldsym)                          TCSR_##regsym##_##fldsym##_SHFT

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

#endif /* __HALTCSRHWIO_H__ */
