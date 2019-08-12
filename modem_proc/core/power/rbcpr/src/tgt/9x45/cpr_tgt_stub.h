#ifndef CPR_TGT_STUB_H
#define CPR_TGT_STUB_H
/*============================================================================
@file cpr_tgt_stub.h

Stub for listing target specific constants and enumerations.
DO NOT DEFINE ANYTHING ELSE IN THIS FILE!

Copyright © 2013 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/tgt/9x45/cpr_tgt_stub.h#1 $
=============================================================================*/

//FIX this, the number should be figured out in a different way.  For now just ensuring the struct is big enough
#define CPR_MAX_VMODE_COUNT 9 

#define CPR_VMODE_INVALID 0xffffffff

/** Number of Oscillators in each sensor. */
#define CPR_OSCILLATOR_COUNT 8

#define CPR_IS_VALID_REFERENCE_CLOCK_RATE(clkKHz) \
 ( ( ((clkKHz) == 19200) || ((clkKHz) == 75000) || ((clkKHz) == 72000) || ((clkKHz) == 144000) )? 1: 0 )

#endif // CPR_TGT_STUB_H

