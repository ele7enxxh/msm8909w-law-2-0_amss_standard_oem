/***************************************************************************

 MODULE: mrcmcr

 DESCRIPTION: Convience macros for access the cp registers in the arm.

 AUTHOR: KAS

 REV/DATE: Fri Mar 18 16:34:44 EST 2005

 Copyright (c) 2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*****************************************************************************/

#ifndef __mrcmcr__h_
#define __mrcmcr__h_

/*
 * Define some convience macros to acccess the cp registers from c code
 *
 * Takes the same format as the asm instructions and unfortunatly you cannot
 * use variables to select the crn, crn or op fields...
 *
 * When compiled these compile to single asm instructions (fast)
 * without all the hassel of __asm__ __volatile__ (...)
 *
 * Format is:
 *
 *    register unsigned long reg;   // destination variable
 *    MRC(reg, p15, 0, c1, c0, 0 );
 *
 *   MRC read control register
 *   MCR control register write
 */

/*
 * Some assembly macros so we can use the same macros as in the C version.
 * Turns the ASM code a little C-ish but keeps the code consistant and in
 * one location...
 */
#ifdef _ARM_ASM_

#define MRC(reg, processor, op1, crn, crm, op2) \
      mrc      processor, op1, reg,  crn, crm, op2

#define MCR(reg, processor, op1, crn, crm, op2) \
      mcr      processor, op1, reg,  crn, crm, op2

/*
 * C version of the macros.
 */
#else

#pragma push
#pragma arm
#define MRC(reg, processor, op1, crn, crm, op2) \
   __asm { \
      mrc processor, op1, reg,  crn, crm, op2 \
  }

#define MCR(reg, processor, op1, crn, crm, op2) \
   __asm { \
      mcr processor, op1, reg, crn, crm, op2 \
  }
#pragma pop
#endif

/*
 * Easy access convience function to read CP15 registers from c code
 */
#define MRC15(reg, op1, crn, crm, op2) MRC(reg, p15, op1, crn, crm, op2)
#define MCR15(reg, op1, crn, crm, op2) MCR(reg, p15, op1, crn, crm, op2)


#endif
