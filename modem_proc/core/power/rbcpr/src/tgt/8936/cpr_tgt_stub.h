#ifndef CPR_TGT_STUB_H
#define CPR_TGT_STUB_H
/*============================================================================
@file cpr_tgt_stub.h

Stub for listing target specific constants and enumerations.
DO NOT DEFINE ANYTHING ELSE IN THIS FILE!

Copyright © 2013 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/tgt/8936/cpr_tgt_stub.h#1 $
=============================================================================*/

/**
 * Voltage modes needed on this HW target.
 */
typedef enum
{
  // CPR_VMODE_LOW_MINUS - not used by MSS rail (and hence not used by CPR). Only added to align with clkrgm enums
  CPR_VMODE_LOW_MINUS,
  CPR_VMODE_LOW,
  CPR_VMODE_LOW_PLUS,
  CPR_VMODE_NOMINAL,
  CPR_VMODE_TURBO,
  CPR_VMODE_SUPER_TURBO,
  CPR_VMODE_COUNT,
  CPR_VMODE_INVALID = 0x7FFFFFFF
} CprVModeEnumType;

/* Number of Oscillators in each sensor. */
#define CPR_OSCILLATOR_COUNT 8

#define CPR_IS_VALID_REFERENCE_CLOCK_RATE(clkKHz) \
 ( ( ((clkKHz) == 19200) || ((clkKHz) == 72000) || ((clkKHz) == 144000) )? 1: 0 )

#endif // CPR_TGT_STUB_H

