#ifndef __QUPTCSR_HWIO__
#define __QUPTCSR_HWIO__

/*
===========================================================================
*/
/**
  @file quptcsr_hwio
  @brief macros for accessing the tcsr routing of the qup interrupt.

*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/qup/hw/quptcsr_hwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

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


/* TCSR BITS, for QUP INTERRUPTs in MPSS*/
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_ADDR(base,n)                                                                          ((base) + 0x0000b140 + 0x10 * (n))
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_OFFS(base,n)                                                                          (0x0000b140 + 0x10 * (n))
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_RMSK                                                                                       0xfff
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MAXn                                                                                           1
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_INI(base,n)        \
        in_dword_masked(HWIO_PHSS_QUP_MSS_INT_SEL_n_ADDR(base,n), HWIO_PHSS_QUP_MSS_INT_SEL_n_RMSK)
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PHSS_QUP_MSS_INT_SEL_n_ADDR(base,n), mask)
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_OUTI(base,n,val)    \
        out_dword(HWIO_PHSS_QUP_MSS_INT_SEL_n_ADDR(base,n),val)
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PHSS_QUP_MSS_INT_SEL_n_ADDR(base,n),mask,val,HWIO_PHSS_QUP_MSS_INT_SEL_n_INI(base,n))
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_6_IRQ_ENABLE_BMSK                                                            0x800
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_6_IRQ_ENABLE_SHFT                                                              0xb
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_5_IRQ_ENABLE_BMSK                                                            0x400
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_5_IRQ_ENABLE_SHFT                                                              0xa
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_4_IRQ_ENABLE_BMSK                                                            0x200
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_4_IRQ_ENABLE_SHFT                                                              0x9
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_3_IRQ_ENABLE_BMSK                                                            0x100
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_3_IRQ_ENABLE_SHFT                                                              0x8
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_2_IRQ_ENABLE_BMSK                                                             0x80
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_2_IRQ_ENABLE_SHFT                                                              0x7
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_1_IRQ_ENABLE_BMSK                                                             0x40
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP2_QUP_1_IRQ_ENABLE_SHFT                                                              0x6
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_6_IRQ_ENABLE_BMSK                                                             0x20
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_6_IRQ_ENABLE_SHFT                                                              0x5
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_5_IRQ_ENABLE_BMSK                                                             0x10
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_5_IRQ_ENABLE_SHFT                                                              0x4
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_4_IRQ_ENABLE_BMSK                                                              0x8
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_4_IRQ_ENABLE_SHFT                                                              0x3
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_3_IRQ_ENABLE_BMSK                                                              0x4
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_3_IRQ_ENABLE_SHFT                                                              0x2
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_2_IRQ_ENABLE_BMSK                                                              0x2
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_2_IRQ_ENABLE_SHFT                                                              0x1
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_1_IRQ_ENABLE_BMSK                                                              0x1
#define HWIO_PHSS_QUP_MSS_INT_SEL_n_MSS_BLSP1_QUP_1_IRQ_ENABLE_SHFT
#endif /* __QUPTCSR_HWIO__ */
