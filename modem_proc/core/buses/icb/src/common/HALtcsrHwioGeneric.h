#ifndef __HALTCSRHWIOGENERIC_H__
#define __HALTCSRHWIOGENERIC_H__
/*
===========================================================================
*/
/**
  @file HALtcsrHwioGeneric.h
  @brief Generic hwio for the TCSR HALT block.

*/
/*
  ===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/HALtcsrHwioGeneric.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: TCSR_HALT (generic)
 *--------------------------------------------------------------------------*/
#define TCSR_HALT_REG_BASE(b,o)                              (((uint8_t *)(b))      + (o))

/*------------------------------ HALT_REQn -----------------------------*/
#define TCSR_HALT_REQn_ADDR(b,o,s,n)                   (TCSR_HALT_REG_BASE(b,o) + (s) * (n))
#define TCSR_HALT_REQn_RMSK                                   0x1
#define TCSR_HALT_REQn_MAXn                                    32
#define TCSR_HALT_REQn_INI(b,o,s,n)    \
        in_dword_masked(TCSR_HALT_REQn_ADDR(b,o,s,n), TCSR_HALT_REQn_RMSK)
#define TCSR_HALT_REQn_INMI(b,o,s,n,mask) \
        in_dword_masked(TCSR_HALT_REQn_ADDR(b,o,s,n), (mask))
#define TCSR_HALT_REQn_OUTI(b,o,s,n,val) \
        out_dword(TCSR_HALT_REQn_ADDR(b,o,s,n),val)
#define TCSR_HALT_REQn_OUTMI(b,o,s,n,mask,val) \
        out_dword_masked_ns(TCSR_HALT_REQn_ADDR(b,o,s,n),mask,val,TCSR_HALT_REQn_INI(b,o,s,n))
#define TCSR_HALT_REQn_HALTREQ_BMSK                           0x1
#define TCSR_HALT_REQn_HALTREQ_SHFT                           0x0


/*------------------------------ HALT_ACKn -----------------------------*/
#define TCSR_HALT_ACKn_ADDR(b,o,s,n)                   (TCSR_HALT_REG_BASE(b,o) + 0x4 + (s) * (n))
#define TCSR_HALT_ACKn_RMSK                                   0x1
#define TCSR_HALT_ACKn_MAXn                                    32
#define TCSR_HALT_ACKn_INI(b,o,s,n)    \
        in_dword_masked(TCSR_HALT_ACKn_ADDR(b,o,s,n), TCSR_HALT_ACKn_RMSK)
#define TCSR_HALT_ACKn_INMI(b,o,s,n,mask) \
        in_dword_masked(TCSR_HALT_ACKn_ADDR(b,o,s,n), (mask))
#define TCSR_HALT_ACKn_OUTI(b,o,s,n,val) \
        out_dword(TCSR_HALT_ACKn_ADDR(b,o,s,n),val)
#define TCSR_HALT_ACKn_OUTMI(b,o,s,n,mask,val) \
        out_dword_masked_ns(TCSR_HALT_ACKn_ADDR(b,o,s,n),mask,val,TCSR_HALT_ACKn_INI(b,o,s,n))
#define TCSR_HALT_ACKn_HALTACK_BMSK                           0x1
#define TCSR_HALT_ACKn_HALTACK_SHFT                           0x0

#endif /* __HALTCSRHWIOGENERIC_H__ */
