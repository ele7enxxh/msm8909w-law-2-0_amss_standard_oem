#ifndef RFLM_UTIL_H
#define RFLM_UTIL_H
/*!
  @file
  rflm_util.h

  @brief
  RFLM utility inline functions.

  @detail
  This file should not be included standalone - use rflm.h instead,
  which includes this file.

  The intent of these functions is to provide:
  - efficient implementations of common operations (e.g. mod_wrap, div_const)
  - operations that cannot be expressed in C (e.g. add_sat, clb)
  - operations to provide proper fixed-point behavior (e.g. mpy)
  - better/safer implementations of common operations (e.g. max)
  - a subset of functions that could be implemented reasonably fast on any
    processor, if dedicated instructions are not available.

  The intent is to provide the smallest subset that can meet these goals.
  Use C where appropriate (if you want a non-saturating add, use +).
  Use intrinsics where you need specific/unusual functionality.

*/

/*===========================================================================

  Copyright (c) 2013 -2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/includes/inc/rflm_util.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
11/11/14     hdz     Added rflm_test_and_swap
07/18/14   jf      Return TX power in linear region to ML1
2014-04-04   gvn     Changes to gain calculation for intra band CA
2013-08-23   jc      Changes to allow windows QTF compilation
2013-07-18   JC      Port from modem_fw_util.h
===========================================================================*/


/* This file should only be included as part of modem_fw.h. Enforce the 
   order (it's important that this be before the #define RFLM_UTIL_H). */
#include "rflm.h"


#define CONST_DB_TO_ULIN_QUAD_C0  32768  //0.997894  in Q15
#define CONST_DB_TO_ULIN_QUAD_C1  -21869 //-0.667389 in Q15 
#define CONST_DB_TO_ULIN_QUAD_C2  5614   //0.171326  in Q15
#define CONST_PT1_MPY_LOG2_10    0x2A854B42  //0.1*log2(10) in Q31


#include "rflm_api_lte.h"

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


#ifdef TEST_FRAMEWORK
#error code not present
#else

#ifdef __QDSP6_ARCH__
// XXX Temporary until we permanently migrate to hexagon-gcc tools.
#include <q6protos.h>
#else
#include <hexagon_protos.h>
#endif

#endif

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*! @brief Generate a constant multiplicative inverse.
    "den" must be positive and >= 1.
    Result is a 32=bit unsigned value corresponding to (1/den) in Q32.
    To perform a divide, use the result with the mult_inv() function.
    Note than when den==1, result=0. This is interpreted appropriately
    by the mult_inv() function.
*/
#define CONST_MULT_INV(den) \
  ((uint32) ( (0x100000000LL + (den) - 1) / (den) ) )

/*! @brief Integer divide by constant (floor(num/den)) using multiplication
    by (1/den). 
    Inputs must be unsigned. den must be a constant >= 1.
    Result is exact within operating range. To calculate range:
      factor = floor((2^32 + den - 1) / den)
      num must be less than (2^32 / (factor*den - 2^32))
      If (factor*den - 2^32) == 0, then the result is valid over the full
      2^32-1 input range (for example, if den = 2^x).
*/
#define div_const(num, den) mult_inv(num, CONST_MULT_INV(den))

/*! @brief Integer modulo by constant (num % den). 
    Inputs must be unsigned. den must be a constant >= 1.
    See div_const() for operating range.
*/
#define mod_const(num,den) \
   ((num) - (div_const( (num), (den) ) * (den)) )

/*! @brief Divide by applying a multiplicative inverse. 
    Calculated as num*inv, where inv is generated by CONST_MULT_INV.
    Since CONST_MULT_INV returns 0 for an argument of 1, this macro
    treats that as (num / 1). 
    See div_const() for operating range.
*/

typedef struct
{
uint32 result; /*!< Result value */
int32 scale; /*!< Scale factor */
} uint32_result_scale_s;

/* ------------------------------------------------------------------------- */
int32 aprox_ulin_to_dB_linlut8(uint32 input, int32 Qi);
/* ------------------------------------------------------------------------- */
uint32 rflm_approx_dB_to_ulin_linlut8(int32 input);
/* ------------------------------------------------------------------------- */
STATIC_INLINE uint32 mult_inv(uint32 num, uint32 inv) {

  return ( inv == 0 ? num : (uint32) Q6_R_mpyu_RR(num,inv) );
}


/*! @brief Wrap the input x to between [0,mod-1]. 
    x must be in the range [-mod,2*mod-1].
    If you are sure only one wraparound direction is possible (e.g. just
    after adding a positive value), use wfw_mod_wrap_high() or 
    wfw_mod_wrap_low().

    DO NOT USE FOR POINTERS. Addresses >= 0x80000000 will not give correct results.
*/
STATIC_INLINE int32 mod_wrap(int32 x, int32 mod) {
  return Q6_R_modwrap_RR(x,mod);
}

/*! @brief Wrap the input x to be between [0,mod-1]. 
    x must be in the range [0,2*mod-1].
    Negative wraparound is not checked.

    DO NOT USE FOR POINTERS. Addresses >= 0x80000000 will not give correct results.
*/
STATIC_INLINE int32 mod_wrap_high(int32 x, int32 mod) {
  return mod_wrap(x,mod);
}

/*! @brief Wrap the input x to be between [0,mod-1].
    x must be in the range [-mod,mod-1].
    Positive wraparound is not checked.

    DO NOT USE FOR POINTERS. Addresses >= 0x80000000 will not give correct results.
*/
STATIC_INLINE int32 mod_wrap_low(int32 x, int32 mod) {
  return mod_wrap(x,mod);
}

/*! @brief Wrap the input x to be from [-mod/2,(mod+1)/2-1]. 
    Useful for calculating the signed difference between two numbers 
    (x = a - b).
    x must be in the range [-(mod+mod/2),mod+(mod+1)/2-1].

    DO NOT USE FOR POINTERS. Addresses >= 0x80000000 will not give correct results.
*/
STATIC_INLINE int32 mod_wrap_signed(int32 x, int32 mod) {
  if (x >= ((mod+1)/2))
  {
    x -= mod; 
  } else if (x < -(mod/2))
  {
    x += mod;
  }
  return x;
}



/*! @brief 16-bit addition, saturate at 16-bits. */
STATIC_INLINE int16 add_sath(int16 x, int16 y) { 
  return (int16)Q6_R_add_RlRl_sat(x,y); 
}

/*! @brief 32-bit addition, saturate at 32-bits. */
STATIC_INLINE int32 add_satw(int32 x, int32 y) { 
  return Q6_R_add_RR_sat(x,y); 
}

/*! @brief 16-bit subtraction, saturate at 16-bits. */
STATIC_INLINE int16 sub_sath(int16 x, int16 y) { 
  return (int16)Q6_R_sub_RlRl_sat(x,y); 
}

/*! @brief 32-bit subtraction, saturate at 32-bits. */
STATIC_INLINE int32 sub_satw(int32 x, int32 y) { 
  return Q6_R_sub_RR_sat(x,y); 
}

/*! @brief 32-bit negate, saturate at 32-bits. */
STATIC_INLINE int32 neg_satw(int32 x) { 
  return Q6_R_neg_R_sat(x); 
}

/*! @brief 32-bit absolute value, saturate at 32-bits. */
STATIC_INLINE int32 abs_satw(int32 x) { 
  return Q6_R_abs_R_sat(x); 
}

/*! @brief 32-bit signed maximum. */
STATIC_INLINE int32 maxw(int32 x, int32 y) { 
  return Q6_R_max_RR(x,y); 
}

/*! @brief 32-bit unsigned maximum. */
STATIC_INLINE uint32 maxuw(uint32 x, uint32 y) { 
  return Q6_R_maxu_RR(x,y); 
}

/*! @brief 64-bit signed maximum. */
STATIC_INLINE int64 maxd(int64 x, int64 y) { 
  return Q6_P_max_PP(x,y); 
}

/*! @brief 64-bit unsigned maximum. */
STATIC_INLINE uint64 maxud(uint64 x, uint64 y) { 
  return Q6_P_maxu_PP(x,y); 
}

/*! @brief 32-bit signed minimum. */
STATIC_INLINE int32 minw(int32 x, int32 y) { 
  return Q6_R_min_RR(x,y); 
}

/*! @brief 32-bit unsigned minimum. */
STATIC_INLINE uint32 minuw(uint32 x, uint32 y) { 
  return Q6_R_minu_RR(x,y); 
}

/*! @brief 64-bit signed minimum. */
STATIC_INLINE int64 mind(int64 x, int64 y) { 
  return Q6_P_min_PP(x,y); 
}

/*! @brief 64-bit unsigned minimum. */
STATIC_INLINE uint64 minud(uint64 x, uint64 y) { 
  return Q6_P_minu_PP(x,y); 
}

/*! @brief Count number of leading 0-bits in a 32-bit word. */
STATIC_INLINE int cl0w(int32 x) { 
  return Q6_R_cl0_R(x); 
}

/*! @brief Count number of leading 0-bits in a 64-bit word. */
STATIC_INLINE int cl0d(int64 x) { 
  return Q6_R_cl0_P(x); 
}

/*! @brief Count number of leading 1-bits in a 32-bit word. */
STATIC_INLINE int cl1w(int32 x) { 
  return Q6_R_cl1_R(x); 
}

/*! @brief Count number of leading 1-bits in a 64-bit word. */
STATIC_INLINE int cl1d(int64 x) { 
  return Q6_R_cl1_P(x); 
}

/*! @brief Count number of leading same-bits in a 32-bit word. */
STATIC_INLINE int clbw(int32 x) { 
  return Q6_R_clb_R(x); 
}

/*! @brief Count number of leading same-bits in a 64-bit word. */
STATIC_INLINE int clbd(int64 x) { 
  return Q6_R_clb_P(x); 
}

/*! @brief Count number of trailing 0-bits in a 32-bit word. */
STATIC_INLINE int ct0w(uint32 x) { 
  return Q6_R_ct0_R(x); 
}
 
/*! @brief Count number of trailing 0-bits in a 64-bit word. */
STATIC_INLINE int ct0d(uint64 x) { 
  return Q6_R_ct0_P(x); 
}
 
/*! @brief Count number of trailing 1-bits in a 32-bit word. */
STATIC_INLINE int ct1w(uint32 x) { 
  return Q6_R_ct1_R(x); 
}

/*! @brief Count number of trailing 1-bits in a 64-bit word. */
STATIC_INLINE int ct1d(uint64 x) { 
  return Q6_R_ct1_P(x); 
}

/*! @brief Clear bit "b" in 32-bit word "x" and return the updated "x". */
STATIC_INLINE int32 clrbitw(int32 x, int b) { 
  return Q6_R_clrbit_RR(x,b); 
}

/*! @brief Set bit "b" in 32-bit word "x" and return the updated "x". */
STATIC_INLINE int32 setbitw(int32 x, int b) { 
  return Q6_R_setbit_RR(x,b); 
}

/*! @brief Toggle bit "b" in 32-bit word "x" and return the updated "x". */
STATIC_INLINE int32 togglebitw(int32 x, int b) { 
  return Q6_R_togglebit_RR(x,b); 
}

/*! @brief Extract a bitfield from 32-bit x and return LSb aligned.
    "width" bits starting from bit "offset". */
STATIC_INLINE uint32 fextractuw(uint32 x, int width, int offset) {
  vect64 f;
  f.w[1] = width;
  f.w[0] = offset;
  return Q6_R_extractu_RP(x,f.d); 
}

/*! @brief Extract a bitfield from 64-bit x and return LSb aligned.
    "width" bits starting from bit "offset". */
STATIC_INLINE uint64 fextractud(uint64 x, int width, int offset) {
  vect64 f;
  f.w[1] = width;
  f.w[0] = offset;
  return Q6_P_extractu_PP(x,f.d); 
}

/*! @brief Insert a bitfield from "src" into "dest", both 32-bit.
    Take "width" LSb from "src", insert into "dest" starting at "offset".
    Returns updated "dest". */
STATIC_INLINE uint32 finsertuw(uint32 dest, uint32 src, int width, int offset) {
  vect64 f;
  f.w[1] = width;
  f.w[0] = offset;
  return Q6_R_insert_RP(dest,src,f.d); 
}

/*! @brief Insert a bitfield from "src" into "dest", both 64-bit.
    Take "width" LSb from "src", insert into "dest" starting at "offset".
    Returns updated "dest". */
STATIC_INLINE uint64 finsertud(uint64 dest, uint64 src, int width, int offset) {
  vect64 f;
  f.w[1] = width;
  f.w[0] = offset;
  return Q6_P_insert_PP(dest,src,f.d); 
}


/*! @brief Saturate 32-bit signed value to signed 16-bits */
#define sat16 sath
STATIC_INLINE int16 sath(int32 x) {
  return (int16)Q6_R_sath_R(x); 
}

/*! @brief Saturate 32-bit signed value to unsigned 16-bits. */
#define satu16 satuh
STATIC_INLINE uint16 satuh(int32 x) {
  return (int16)Q6_R_satuh_R(x); 
}

/*! @brief Saturate 64-bit signed value to signed 32-bits. */
#define sat32 satw
STATIC_INLINE int32 satw(int64 x) {
  return Q6_R_sat_P(x); 
}

/*! @brief Saturate 32-bit signed value to arbitrary width.
    Width includes the sign bit, i.e. saturate(x,8) returns [-128,127].
    Don't use width == 0. 
*/
STATIC_INLINE int32 saturate(int32 x, int bits) {
  bits = 32 - bits;
  x = Q6_R_asl_RR_sat(x,bits);
  return Q6_R_asr_RR(x,bits);
}

/*! @brief 32-bit signed shift. Positive "s" values shift left, negative 
    shifts right. */
STATIC_INLINE int32 shiftw(int32 x, int s) {
  return Q6_R_asl_RR(x,s); 
}

/*! @brief 32-bit signed shift, saturate to 32 bits. Positive "s" values 
    shift left, negative shifts right. */
STATIC_INLINE int32 shift_satw(int32 x, int s) {
  return Q6_R_asl_RR_sat(x,s); 
}

/*! @brief 64-bit signed shift. Positive "s" values shift left, negative
    shifts right. */
STATIC_INLINE int64 shiftd(int64 x, int s) {
  return Q6_P_asl_PR(x,s); 
}

/*! @brief Multiply: signed 16x16 -> signed 32 bits. */
STATIC_INLINE int32 mpyh(int16 x, int16 y) {
  /* Use a regular multiply. Compiler optimizes to mpy(Rs.L,Rt.L). 
     If we use an intrinsic, GCC adds unnecessary sign extends. */
  return ((int32) x * y);
}

/*! @brief Multiply: unsigned 16x16 -> unsigned 32 bits. */
STATIC_INLINE uint32 mpyuh(uint16 x, uint16 y) {
  return ((uint32) x * y);
}

/*! @brief Multiply: signed 32x32 -> signed 64 bits. */
STATIC_INLINE int64 mpyw(int32 x, int32 y) {
  return Q6_P_mpy_RR(x,y);
}

/*! @brief Multiply: unsigned 32x32 -> unsigned 64 bits. */
STATIC_INLINE uint64 mpyuw(uint32 x, uint32 y) {
  return Q6_P_mpyu_RR(x,y);
}


#ifdef TEST_FRAMEWORK
#error code not present
#else

/*! @brief No operation, useful for inserting delays. */
STATIC_INLINE void nop(void) { 
  asm volatile ("nop" : : ); 
}


/*! @brief Return the current 64-bit PCYCLEHI:LO register count. 
    SUPERVISOR MODE ONLY. See fw_prof_get_pcycle() for user mode.
*/
STATIC_INLINE uint64 fw_get_pcycle_reg(void) {
  uint64 pcycle;
  asm volatile ("%[pcycle] = S31:30" : [pcycle] "=r"(pcycle));
  return pcycle; /*lint !e530  "reg not initialized" */
}


/*! @brief Hexagon USR register bits. Refer to the Hexagon reference manual.
*/
typedef union
{
  uint32  val;
  struct {
    uint32 OVF   : 1;
    uint32       : 7;
    uint32 LPCFG : 2;
    uint32       : 5;
    uint32 HFI   : 2;
    uint32       : 1;
    uint32 HFIL2 : 2;
    uint32       : 11;
    uint32 PFA   : 1;
  };
} fw_usr_reg_t;


/*! @brief Read the Hexagon USR register.
*/
STATIC_INLINE fw_usr_reg_t fw_get_usr_reg(void)
{
  fw_usr_reg_t reg;
  asm volatile ("%[reg] = USR" : [reg] "=r"(reg.val));
  return reg; /*lint !e530  "reg not initialized" */
} 

/*! @brief Write the Hexagon USR register.
*/
STATIC_INLINE void fw_set_usr_reg(fw_usr_reg_t reg)
{
  asm volatile ("USR = %[reg]" : : [reg] "r"(reg.val));
} 


/*! @brief Data cache line prefetch. */
STATIC_INLINE void dcfetch(void *addr) {
  Q6_dcfetch_A(addr);
}

/*! @brief Data cache line clean.
    Write the cache line containing "addr" from cache if dirty. */
STATIC_INLINE void dccleana(void *addr) {
  asm volatile ("dccleana(%[addr])" : : [addr] "r" (addr) : "memory");
}

/*! @brief Data cache line clean and invalidate.
    Write the cache line containing "addr" from cache if dirty, then 
    invalidate that line so it is no longer present in cache. */
STATIC_INLINE void dccleaninva(void *addr) {
  asm volatile ("dccleaninva(%[addr])" : : [addr] "r" (addr) : "memory" );
}

/*! @brief Data cache line invalidate. 
    Invalidate the cache line containing "addr" from cache, discarding
    its contents if dirty. 
    NOTE: If addr is not cache line aligned, unrelated data may be
          discarded accidentally. */
STATIC_INLINE void dcinva(void *addr) {
  asm volatile ("dcinva(%[addr])" : : [addr] "r" (addr) : "memory" );
}


/*! @brief Data cache line zero-allocate. addr must be 32-byte aligned.
    Clean the cache line if dirty, then set all 32 bytes to 0 and mark dirty. */
STATIC_INLINE void dczeroa(void *addr) {
  asm volatile ("dczeroa(%[addr])" : : [addr] "r" (addr) : "memory" );
}


/*! @brief Instruction cache line invalidate. 
    Invalidate the cache line containing "addr" from cache.
    NOTE: If addr is not cache line aligned, unrelated data may be
          discarded accidentally. */
STATIC_INLINE void icinva(void *addr) {
  asm volatile ("icinva(%[addr])" : : [addr] "r" (addr) : "memory" );
}


/*! @brief Issue an L2 fetch.
 
    This maps directly to the l2fetch() instruction. Refer to the Hexagon
    programmer's reference for details.
 
    For simple linear transfers, you may want to use l2fetch_buffer() instead,
    as it has simpler inputs.
*/
STATIC_INLINE void l2fetch
(
  void *addr,    /*!< (Virtual) address to start fetch */
  uint16 width,  /*!< Width (in bytes) of memory block */
  uint16 height, /*!< Height (number of width-sized blocks) */
  uint16 stride, /*!< Offset to increment after each width-sized block. */
  boolean direction  /*!< 0=row major, 1=column major */
)
{
  asm volatile ("l2fetch(%[addr],%[dim])" : : 
     [addr] "r" (addr), 
     [dim] "r" ( ((uint64) direction << 48) | 
                 ((uint64) stride    << 32) |
                 ((uint64) width     << 16) | 
                 ((uint64) height) ) : "memory");
}


/*! @brief Initiate a fetch from external memory to L2 cache. 
 
    This function does a one-dimensional transfer, and has a length limit of
    65535*32=2097120 bytes.  
    For 2-dimensional transfers, or for longer lengths, use l2fetch().
   
    A length of 0 *may* do a single line fetch, depending on the address alignment. 

    Also see l2fetch_is_active() to check if the fetch is complete.
 
    Refer to the Hexagon programmers manual for details on the l2fetch instruction.
    Note that a thread switch, or triggering another l2fetch, may cancel the current
    fetch operation.
    In general, this should be issued before processing a section of data from
    external memory, and whether the data is available when it is accessed
    is best-effort.
*/
STATIC_INLINE void l2fetch_buffer
(
  void *addr,  /*!< (Virtual) address to start fetch */
  uint32 len   /*!< Length (in bytes) to fetch. */
)
{
  /* Cache-align starting address and length. */
  uint32 ofs = ((uint32) addr) & (CACHE_LINE_SZ-1);
  addr = (void *) ((uint32) addr - ofs);
  len  = (len+ofs+CACHE_LINE_SZ-1) / CACHE_LINE_SZ;

  /* Width=cache line, height=# cache lines, stride=cache line */
  asm volatile ("l2fetch(%[addr],%[dim])" : : 
     [addr] "r" (addr), 
     [dim] "r" ( Q6_P_combine_IR(CACHE_LINE_SZ, Q6_R_combine_RlRl(CACHE_LINE_SZ, len)) )
     : "memory");

} /* l2fetch_buffer() */


/*! @brief Check if l2fetch operation is active.
 
    Return TRUE is an l2fetch is in progress, FALSE otherwise.
    Note that if there was a thread switch, the fetch in progress may not be
    the one which is expected.
 
    Polling for a fetch to be complete is counter-productive - the intended
    use of l2fetch is to run in the background while processing a buffer.
    Polling for completion is no better than doing individual loads to each
    buffer line.
    This function would typically be used only for debug/profiling.
*/
STATIC_INLINE boolean l2fetch_is_active(void)
{
  fw_usr_reg_t reg = fw_get_usr_reg();
  return (reg.PFA ? TRUE : FALSE);
}


/*! @brief Ensure that previous instructions have committed. */
STATIC_INLINE void isync(void) {
  asm volatile ("isync" : : : "memory" );
}

/*! @brief Force all pending transactions to complete. */
STATIC_INLINE void syncht(void) {
  asm volatile ("syncht" : : : "memory" );
}

/*! @brief A compiler barrier. Inserting this into the code guarantees
    that the compiler will schedule all instructions preceding it
    before any instructions following it.
    It does not necessarily guarantee that those instructions have completed
    (i.e. memory accesses) as seen by a separate CPU.
*/
STATIC_INLINE void compiler_barrier(void) {
  asm volatile ("" : : : "memory");
}

/* @brief The APi does following:

   if( *target == old_value)    --> atomic start
   {
      if ( *target is not changed)  
	  {
	     *target = expected_value;
		 return old_value;
	  }
	  else
	  {
	    return 0xFFFFFFFF
	  }                           
	  
   }
   else
   {
     return 0xFFFFFFFF;
   }                         --> atomic end
*/
STATIC_INLINE uint32 rflm_test_and_swap
(
  uint32* target, 
  uint32 old_value,    // inc dev_idx and concur_cnt
  uint32 expected_value
)
{
  uint32 result;
    
  asm volatile(
  "1:  %0 = memw_locked(%2)         \n"
  "    p0 = cmp.eq(%0,%3)           \n"
  "    if !p0 jump 2f               \n"
  "    memw_locked(%2,p0) = %4      \n"
  "    if p0 jump 3f                \n"
  "2:  %0 = #-1                     \n"
  "3:                               \n"
  :"=&r" (result), "+m" (*target)
  :"r" (target), "r" (old_value),"r"(expected_value)
  :"p0");

  return result;

}


/*! @brief Wait for the specified number of thread cycles.
    The actual duration is approximate due to effects of DMT and compiler optimization.
    DO NOT use this for precise wait intervals.
*/
STATIC_INLINE void fw_wait_tcycles(uint32 tcycles) {
  while (tcycles--)
  { // Usually the compiler will generate a 2-cycle loop for this, so the wait
    // is overestimated. But for smaller loop counts, it will be unrolled.
    nop();
  }
}

#endif

/*! @brief Wait for the specified number of pcycles.
    ** XXX This function is deprecated. Use fw_wait_tcycles instead.
    This will wait for at least "pcycles" processor cycles. 
    pcycles must be a constant (no variables), and fit within 8 bits (it is 
    not declared uint8 so larger values generate an assembler error, instead 
    of a truncation). 
    See the Hexagon "pause" instruction for more details.
 */
#define fw_pause_pcycles(pcycles) fw_wait_tcycles((pcycles)/FW_NUM_HW_THREADS)


#ifdef __cplusplus
}   // extern "C"

/*! @brief Define aliases for function overloading in C++.
    These allow the functions to be called without the type specifier,
    e.g. add_sat() instead of add_sath(). The compiler will pick the
    appropriate type, or flag an error if it's ambiguous.
*/

STATIC_INLINE int16 add_sat(int16 x, int16 y) { return add_sath(x,y); }
STATIC_INLINE int32 add_sat(int32 x, int32 y) { return add_satw(x,y); }
STATIC_INLINE int16 sub_sat(int16 x, int16 y) { return sub_sath(x,y); }
STATIC_INLINE int32 sub_sat(int32 x, int32 y) { return sub_satw(x,y); }
STATIC_INLINE int32 neg_sat(int32 x) { return neg_satw(x); }
STATIC_INLINE int32 abs_sat(int32 x) { return abs_satw(x); }
#ifndef TEST_FRAMEWORK
STATIC_INLINE  int32 max( int32 x,  int32 y) { return maxw(x,y);  }
STATIC_INLINE uint32 max(uint32 x, uint32 y) { return maxuw(x,y); }
STATIC_INLINE  int64 max( int64 x,  int64 y) { return maxd(x,y);  }
STATIC_INLINE uint64 max(uint64 x, uint64 y) { return maxud(x,y); }
STATIC_INLINE  int32 min( int32 x,  int32 y) { return minw(x,y);  }
STATIC_INLINE uint32 min(uint32 x, uint32 y) { return minuw(x,y); }
STATIC_INLINE  int64 min( int64 x,  int64 y) { return mind(x,y);  }
STATIC_INLINE uint64 min(uint64 x, uint64 y) { return minud(x,y); }
#else
#error code not present
#endif
STATIC_INLINE int cl0(int32 x)  { return cl0w(x); }
STATIC_INLINE int cl0(int64 x)  { return cl0d(x); }
STATIC_INLINE int cl1(int32 x)  { return cl1w(x); }
STATIC_INLINE int cl1(int64 x)  { return cl1d(x); }
STATIC_INLINE int clb(int32 x)  { return clbw(x); }
STATIC_INLINE int clb(int64 x)  { return clbd(x); }
STATIC_INLINE int ct0(uint32 x) { return ct0w(x); }
STATIC_INLINE int ct0(uint64 x) { return ct0d(x); }
STATIC_INLINE int ct1(uint32 x) { return ct1w(x); }
STATIC_INLINE int ct1(uint64 x) { return ct1d(x); }

STATIC_INLINE int32 clrbit(int32 x, int b)    { return clrbitw(x,b); }
STATIC_INLINE int32 setbit(int32 x, int b)    { return setbitw(x,b); }
STATIC_INLINE int32 togglebit(int32 x, int b) { return togglebitw(x,b); }
STATIC_INLINE uint32 fextract(uint32 x, int width, int offset) { return fextractuw(x,width,offset); }
STATIC_INLINE uint64 fextract(uint64 x, int width, int offset) { return fextractud(x,width,offset); }
STATIC_INLINE uint32 finsert(uint32 dest, uint32 src, int width, int offset) { return finsertuw(dest,src,width,offset); }
STATIC_INLINE uint64 finsert(uint64 dest, uint64 src, int width, int offset) { return finsertud(dest,src,width,offset); }

STATIC_INLINE int32 shift(int32 x, int s) { return shiftw(x,s); }
STATIC_INLINE int64 shift(int64 x, int s) { return shiftd(x,s); }
STATIC_INLINE int32 shift_sat(int32 x, int s) { return shift_satw(x,s); }

STATIC_INLINE  int32 mpy( int16 x,  int16 y) { return mpyh(x,y);  }
STATIC_INLINE uint32 mpy(uint16 x, uint16 y) { return mpyuh(x,y); }
STATIC_INLINE  int64 mpy( int32 x,  int32 y) { return mpyw(x,y);  }
STATIC_INLINE uint64 mpy(uint32 x, uint32 y) { return mpyuw(x,y); }

#endif  // __cplusplus


#endif /* RFLM_UTIL_H */
