/*===========================================================================
* FILE:         q6zip_uncompress.c
*
* SERVICES:     DL PAGER
*
* DESCRIPTION:  q6zip uncompressor
*
* Copyright (c) 2014 Qualcomm Technologies Incorporated.
* All Rights Reserved. QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE
  
  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/src/q6zip_uncompress.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/14   ao      Initial revision (collaboration with Ric Senior)
03/18/14   rs      Updated to version q6zip3v3
04/25/14   rs      Updated to version q6zip3v4
08/21/14   hz      Made compatible with gcc 3.4.4 for Windows and Linux
8/28/14    rr      Updated to version q6zip3v5, added version info.
01/01/15   ao      Updated to version q6zip3v6
===========================================================================*/

#include "assert.h"
#include <stdint.h>
#include <stdlib.h>
#include "q6zip_uncompress.h"
#if __hexagon__
#include <hexagon_protos.h>
#endif

volatile struct
{
  unsigned char major;
  unsigned char minor;
} q6zip_ro_algorithm = {Q6ZIP_RO_ALGORITHM_VERSION >> 8, Q6ZIP_RO_ALGORITHM_VERSION & 0xFF};

#define debug info
#define info //printf

#ifdef debug
#include <stdio.h>
#endif

#define GET_WORD_FROM(ci)           (*((unsigned int*)ci))
#define GET_WORD()                  ((unsigned int)(hold & 0xFFFFFFFF))
#define GET_DWORD()                 hold
#define GET_WORD_SKIPPING(skip)     ((unsigned int)(hold >> skip))
#define GET_DWORD_SKIPPING(skip)    (hold >> skip)
#define SKIP_HOLD(n);               hold >>= n
#define SKIP_HOLD_N_BITS_W_CHECK(n) SKIP_HOLD(n); SKIP_BITS_W_CHECK(n)
#define GET_BITS(n)                 (unsigned int)(hold & ((1UL << n) - 1))
#define GET_BITS_N_SKIP_HOLD(n)     GET_BITS(n); hold >>= n

/*==================================================================*
 * added by hzhi for new macros                                     *
 *==================================================================*/
//Q6_R_insert_RII.
#define INSERT_BITS(dest,src,n_bits,n_offset)       \
    (((dest)&~(((1UL<<(n_bits))-1)<<(n_offset)))|(((src)&((1UL<<(n_bits))-1))<<(n_offset)))

//Q6_R_extractu_RII.
#define EXTRACTU_BITS(src,n_bits,n_offset)        \
    (((src)>>(n_offset))&((1UL<<(n_bits))-1))

//Q6_R_tableidxb_RII.
#define TABLEIDXB_RII(dest,src,n_bits,n_offset)       \
    (((dest)&~((1UL<<(n_bits))-1))|(((src)>>(n_offset))&((1UL<<(n_bits))-1)))

/*==================================================================*/

//if branch inside is to feed in new 32-bit stream to the 64-bit FIFO buffer.
#if __hexagon__
#define SKIP_BITS_W_CHECK(n)                    \
    bits -= n;                                  \
    if (UNLIKELY(bits < 32)) {                  \
        hold |= (uint64_t)(*(in++)) << bits;    \
        Q6_dcfetch_A(in);                       \
        bits += 32;                             \
    }
#else
#define SKIP_BITS_W_CHECK(n)                    \
    bits -= n;                                  \
    if (UNLIKELY(bits < 32)) {                  \
        hold |= (uint64_t)(*(in++)) << bits;    \
        bits += 32;                             \
    }
#endif

#define OP1_BITS 3
#define OP2_BITS 3
#define OP3_BITS 2

#define DEBUG_START debug("%3u %3u %2d %016llX %08X %016llX %u", \
                          count++, (in - input) * 4, bits, GET_DWORD(), \
                          (unsigned int)(GET_DWORD() >> (bits - 32)), GET_DWORD_SKIPPING(2), \
                          GET_BITS(2));

#define MATCH_6N_2x0_SQ0_BITS 3
#define MATCH_8N_SQ0_BITS     3
#define MATCH_5N_3x0_SQ0_BITS 4
#define NO_MATCH_BITS         3
#define DICT1_MATCH_BITS      3
#define DICT2_MATCH_BITS      4
#define MATCH_6N_2x0_SQ1_BITS 3
#define MATCH_8N_SQ1_BITS     3
#define MATCH_4N_4x0_SQ1_BITS 6
#define MATCH_4N_4x0_SQ0_BITS 5
#define MATCH_5N_3x0_SQ1_BITS 5
#define MATCH_6N_2x2_SQ0_BITS 6
#define MATCH_6N_2x4_SQ0_BITS 6
#define MATCH_6N_2x2_SQ1_BITS 7
#define MATCH_6N_2x4_SQ1_BITS 7

#define CACHE_LINE_SHIFT (5)
#define CACHE_LINE_SZ  (1 << CACHE_LINE_SHIFT)

#define JUMP_NEXT_DECODE  {void *jumpPtr=jump_table[op1]; if (LIKELY(op1==1)) goto MATCH_8N_SQ0;goto *jumpPtr;}

#define LIKELY(x) __builtin_expect((x), 1)
#define UNLIKELY(x) __builtin_expect((x), 0)

static unsigned int* input;
static int* q6zip_out_buf_size;

int q6zip_uncompress(void* out_buf, void* in_buf, void* dict, q6zip_state_t* p_state)
{
  input = (unsigned int*)in_buf;
  unsigned int* dictionary1 = (unsigned int*)dict;
  unsigned int* dictionary2 = &dictionary1[1<<DICT1_BITS];
  unsigned int* out = (unsigned int*)out_buf;
  unsigned int* raw = (unsigned int*)0x10;
  unsigned int* in = input;
  int bits = p_state->bits_in_hold;
  int lastOut = p_state->last_sequencial_offset;
  uint64_t hold = (((uint64_t)p_state->hold_high) << 32) | (uint64_t)p_state->hold_low;
  int count = 0;
  int prefetched_dictionary=0;

    debug("\n  n off  b hold             added    raw       "
          "       c  out      notes\n");
    {

     void* jump_table[] =  {
            &&MATCH_6N_2x0_SQ0,  //0
            &&MATCH_8N_SQ0,      //1
            &&MATCH_5N_3x0_SQ0,  //2
            &&NO_MATCH,          //3
            &&DICT1_MATCH,       //4
            &&DICT2_MATCH,       //5
            &&MATCH_6N_2x0_SQ1,  //6
            &&MATCH_8N_SQ1,      //7
            &&MATCH_6N_2x0_SQ0,  //8
            &&MATCH_8N_SQ0,      //9
            &&LEVEL_A,           //10 =0xA==0x1010
            &&NO_MATCH,          //11
            &&DICT1_MATCH,       //12
            &&LEVEL_B,           //13 0xD==1101 MATCH_6N_2x2_SQ1  = ( 0b1011010, 7, 0xFFFF00FF ,"MATCH_6N_2x2_SQ1"),     #MATCH_6Nx1
            &&MATCH_6N_2x0_SQ1,  //14
            &&MATCH_8N_SQ1,      //15
                   };
    //asm("hintjr(%0)" : : "r" (&&MATCH_8N_SQ1));

        {
#if     __hexagon__
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
#else /*__hexagon__*/
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
#endif/*__hexagon__*/
       JUMP_NEXT_DECODE
    }
    MATCH_6N_2x0_SQ0:
            {
       DEBUG_START
#if     __hexagon__
      unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,8+MATCH_6N_2x0_SQ0_BITS+LB_BITS);
      lastOut=Q6_R_tableidxb_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_6N_2x0_SQ0_BITS);
      unsigned int masked = Q6_R_extractu_RII((unsigned int)hold,8,MATCH_6N_2x0_SQ0_BITS+LB_BITS);
      *out++ = Q6_R_insert_RII(*(out + lastOut), masked, 8,0);
#else /*__hexagon__*/
      unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,8+MATCH_6N_2x0_SQ0_BITS+LB_BITS);
      lastOut=TABLEIDXB_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_6N_2x0_SQ0_BITS);
      unsigned int masked = EXTRACTU_BITS((unsigned int)hold,8,MATCH_6N_2x0_SQ0_BITS+LB_BITS);
      *out++ = INSERT_BITS(*(out+lastOut), masked, 8, 0);
#endif/*__hexagon__*/
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_6N_2x0_SQ0_BITS+LB_BITS+8);
       debug("  %08X %d mask byte 3 %04X\n", *(out-1), lastOut, masked);
       JUMP_NEXT_DECODE
            }
    MATCH_6N_2x0_SQ1:
            {
       DEBUG_START
#if     __hexagon__
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,MATCH_6N_2x0_SQ1_BITS+8);
       unsigned int masked = Q6_R_extractu_RII((unsigned int)hold,8,MATCH_6N_2x0_SQ1_BITS);
       *out++ = Q6_R_insert_RII(*(out + lastOut), masked, 8,0);
#else /*__hexagon__*/
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,MATCH_6N_2x0_SQ1_BITS+8);
       unsigned int masked = EXTRACTU_BITS((unsigned int)hold,8,MATCH_6N_2x0_SQ1_BITS);
      *out++ = INSERT_BITS(*(out + lastOut),masked,8,0);
#endif/*__hexagon__*/
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_6N_2x0_SQ1_BITS+8);
       debug("  %08X s %d mask byte 3 %04X\n", *(out-1), lastOut, masked);
       JUMP_NEXT_DECODE
            }
    LEVEL_B:
            {
       if ((unsigned int)hold&(1<<4)) goto MATCH_5N_3x0_SQ1;
       goto MATCH_4N_4x0_SQ0;
            }
    LEVEL_A:
            {
       unsigned int temp=(((unsigned int)hold)>>4)&0x3;
       if (temp==3)goto MATCH_6N_2x2_SQ0;
       if (temp==2)goto MATCH_6N_2x4_SQ0;
       if (temp==0)goto MATCH_4N_4x0_SQ1;
       if ((unsigned int)hold&(1<<6)) goto MATCH_6N_2x2_SQ1;
       goto MATCH_6N_2x4_SQ1;
            }
    NO_MATCH:
            {
       static int no_match_seq=0;

       DEBUG_START
       SKIP_HOLD_N_BITS_W_CHECK(NO_MATCH_BITS);
       *out++ = GET_WORD();
       hold = ((uint64_t)(*(in++)) << (bits-32))|(hold>>32);
       debug("  %08X uncompressed %u\n", *(out-1), bits);
#if     __hexagon__
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
#else /*__hexagon__*/
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
#endif/*__hexagon__*/
       JUMP_NEXT_DECODE
            }
    MATCH_8N_SQ1:
            {
       DEBUG_START
#if     __hexagon__
      unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,MATCH_8N_SQ1_BITS);
#else /*__hexagon__*/
      unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,MATCH_8N_SQ1_BITS);

#endif/*__hexagon__*/
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_8N_SQ1_BITS);
       void *jumpPtr=jump_table[op1];
       *out++ = *(out + lastOut);
       debug("  %08X sequential %d\n", *(out-1), lastOut);
       goto *jumpPtr;
            }
    MATCH_8N_SQ0:
            {
       DEBUG_START
#if     __hexagon__
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,MATCH_8N_SQ0_BITS+LB_BITS);
       lastOut=Q6_R_tableidxb_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_8N_SQ0_BITS);
#else /*__hexagon__*/
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,MATCH_8N_SQ0_BITS+LB_BITS);
       lastOut=TABLEIDXB_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_8N_SQ0_BITS);
#endif/*__hexagon__*/
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_8N_SQ0_BITS+LB_BITS);
       void *jumpPtr=jump_table[op1];
       *out++ = *(out + lastOut);
       debug("  %08X lookback %d\n", *(out-1), lastOut);
       goto *jumpPtr;
            }
    DICT1_MATCH:
            {
       DEBUG_START
#if     __hexagon__
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,DICT1_MATCH_BITS+DICT1_BITS);
       void *jumpPtr=jump_table[op1];
       unsigned int entry=Q6_R_extractu_RII((unsigned int)hold,DICT1_BITS,DICT1_MATCH_BITS);
#else /*__hexagon__*/
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,DICT1_MATCH_BITS+DICT1_BITS);
       void *jumpPtr=jump_table[op1];
       unsigned int entry=EXTRACTU_BITS((unsigned int)hold,DICT1_BITS,DICT1_MATCH_BITS);
#endif/*__hexagon__*/
       SKIP_HOLD_N_BITS_W_CHECK(DICT1_MATCH_BITS+DICT1_BITS);
       *out++ = GET_WORD_FROM(dictionary1 + entry);
       //tmp_out = GET_WORD_FROM(dictionary1 + entry);
       //*out++ = ((tmp_out & 0xff000000) >> 24) | ((tmp_out & 0x00ff0000) >> 8) | ((tmp_out & 0x0000ff00) << 8) | ((tmp_out & 0x000000ff) << 24);
       debug("  %08X dict1 %d\n",*(out-1), entry);
       goto *jumpPtr;
            }
    DICT2_MATCH:
            {
       DEBUG_START
#if     __hexagon__
       *out++ = GET_WORD_FROM(dictionary2 + Q6_R_extractu_RII((unsigned int)hold,DICT2_BITS,DICT2_MATCH_BITS));
       debug("  %08X dict2 %d\n",*(out-1), Q6_R_extractu_RII((unsigned int)hold,DICT2_BITS,DICT2_MATCH_BITS));
       SKIP_HOLD_N_BITS_W_CHECK(DICT2_MATCH_BITS+DICT2_BITS);
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#else /*__hexagon__*/
       *out++ = GET_WORD_FROM(dictionary2 + EXTRACTU_BITS((unsigned int)hold,DICT2_BITS,DICT2_MATCH_BITS));
       //tmp_out = GET_WORD_FROM(dictionary2 + EXTRACTU_BITS((unsigned int)hold,DICT2_BITS,DICT2_MATCH_BITS));
       //*out++ = ((tmp_out & 0xff000000) >> 24) | ((tmp_out & 0x00ff0000) >> 8) | ((tmp_out & 0x0000ff00) << 8) | ((tmp_out & 0x000000ff) << 24);
       SKIP_HOLD_N_BITS_W_CHECK(DICT2_MATCH_BITS+DICT2_BITS);
       debug("  %08X dict2 %d\n",*(out-1), EXTRACTU_BITS((unsigned int)hold,DICT2_BITS,DICT2_MATCH_BITS));
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#endif/*__hexagon__*/
                }
    MATCH_5N_3x0_SQ0:
                {
       DEBUG_START
#if    __hexagon__
       lastOut=Q6_R_tableidxb_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_5N_3x0_SQ0_BITS);
       unsigned int masked = Q6_R_extractu_RII((unsigned int)hold,12,MATCH_5N_3x0_SQ0_BITS+LB_BITS);
       *out++ = Q6_R_insert_RII(*(out + lastOut), masked, 12,0);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_5N_3x0_SQ0_BITS+LB_BITS+12);
       debug("  %08X %d mask 12 bit 20 %04X\n", *(out-1), lastOut, masked);
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
#else/*__hexagon__*/
       lastOut=TABLEIDXB_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_5N_3x0_SQ0_BITS);
       unsigned int masked = EXTRACTU_BITS((unsigned int)hold,12,MATCH_5N_3x0_SQ0_BITS+LB_BITS);
       *out++ = INSERT_BITS(*(out + lastOut), masked, 12,0);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_5N_3x0_SQ0_BITS+LB_BITS+12);
       debug("  %08X %d mask 12 bit 20 %04X\n", *(out-1), lastOut, masked);
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
#endif/*__hexagon__*/
       JUMP_NEXT_DECODE
                }
    MATCH_5N_3x0_SQ1:
                {
       DEBUG_START
#if    __hexagon__
       unsigned int masked = Q6_R_extractu_RII((unsigned int)hold,12,MATCH_5N_3x0_SQ1_BITS);
       *out++ = Q6_R_insert_RII(*(out + lastOut), masked, 12,0);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_5N_3x0_SQ1_BITS+12);
       debug("  %08X s %d mask 12 bit 20 %04X\n", *(out-1), lastOut, masked);
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#else/*__hexagon__*/
       unsigned int masked = EXTRACTU_BITS((unsigned int)hold,12,MATCH_5N_3x0_SQ1_BITS);
       *out++ = INSERT_BITS(*(out + lastOut), masked, 12,0);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_5N_3x0_SQ1_BITS+12);
       debug("  %08X s %d mask 12 bit 20 %04X\n", *(out-1), lastOut, masked);
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#endif/*__hexagon__*/
                }
    MATCH_4N_4x0_SQ0:
                {
       DEBUG_START
#if    __hexagon__
            lastOut=Q6_R_tableidxb_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_4N_4x0_SQ0_BITS);
       unsigned int masked = Q6_R_extractu_RII((unsigned int)hold,16,MATCH_4N_4x0_SQ0_BITS+LB_BITS);
       *out++ = Q6_R_insert_RII(*(out + lastOut), masked, 16,0);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_4N_4x0_SQ0_BITS+LB_BITS+16);
       debug("  %08X %d mask 16 bit 16 %2X\n", *(out-1), lastOut, masked);
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#else/*__hexagon__*/
       lastOut=TABLEIDXB_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_4N_4x0_SQ0_BITS);
       unsigned int masked = EXTRACTU_BITS((unsigned int)hold,16,MATCH_4N_4x0_SQ0_BITS+LB_BITS);
       *out++ = INSERT_BITS(*(out + lastOut), masked, 16,0);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_4N_4x0_SQ0_BITS+LB_BITS+16);
       debug("  %08X %d mask 16 bit 16 %2X\n", *(out-1), lastOut, masked);
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#endif/*__hexagon__*/
                }
    MATCH_4N_4x0_SQ1:
           {
       DEBUG_START
#if    __hexagon__
       unsigned int masked = Q6_R_extractu_RII((unsigned int)hold,16,MATCH_4N_4x0_SQ1_BITS);
       *out++ = Q6_R_insert_RII(*(out + lastOut), masked, 16,0);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_4N_4x0_SQ1_BITS+16);
       debug("  %08X s %d mask 16 bit 16 %2X\n", *(out-1), lastOut, masked);
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#else/*__hexagon__*/
       unsigned int masked = EXTRACTU_BITS((unsigned int)hold,16,MATCH_4N_4x0_SQ1_BITS);
       *out++ = INSERT_BITS(*(out + lastOut), masked, 16,0);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_4N_4x0_SQ1_BITS+16);
       debug("  %08X s %d mask 16 bit 16 %2X\n", *(out-1), lastOut, masked);
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#endif/*__hexagon__*/
                }
    MATCH_6N_2x2_SQ0:
                {
       DEBUG_START
#if    __hexagon__
       lastOut=Q6_R_tableidxb_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_6N_2x2_SQ0_BITS);
       unsigned int masked = Q6_R_extractu_RII((unsigned int)hold,8,MATCH_6N_2x2_SQ0_BITS+LB_BITS);
       *out++ = Q6_R_insert_RII(*(out + lastOut), masked,8,8);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_6N_2x2_SQ0_BITS+LB_BITS+8);
       debug("  %08X %d mask byte 2 %04X\n", *(out-1), lastOut, masked);
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#else/*__hexagon__*/
       lastOut=TABLEIDXB_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_6N_2x2_SQ0_BITS);
       unsigned int masked = EXTRACTU_BITS((unsigned int)hold,8,MATCH_6N_2x2_SQ0_BITS+LB_BITS);
       *out++ = INSERT_BITS(*(out + lastOut), masked, 8,8);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_6N_2x2_SQ0_BITS+LB_BITS+8);
       debug("  %08X %d mask byte 2 %04X\n", *(out-1), lastOut, masked);
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#endif/*__hexagon__*/
                }
    MATCH_6N_2x2_SQ1:
                {
       DEBUG_START
#if    __hexagon__
       unsigned int masked = Q6_R_extractu_RII((unsigned int)hold,8,MATCH_6N_2x2_SQ1_BITS);
       *out++ = Q6_R_insert_RII(*(out + lastOut), masked,8,8);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_6N_2x2_SQ1_BITS+8);
       debug("  %08X s %d mask byte 2 %04X\n", *(out-1), lastOut, masked);
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#else/*__hexagon__*/
       unsigned int masked = EXTRACTU_BITS((unsigned int)hold,8,MATCH_6N_2x2_SQ1_BITS);
       *out++ = INSERT_BITS(*(out + lastOut), masked, 8,8);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_6N_2x2_SQ1_BITS+8);
       debug("  %08X s %d mask byte 2 %04X\n", *(out-1), lastOut, masked);
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#endif/*__hexagon__*/
                }
    MATCH_6N_2x4_SQ0:
                {
       DEBUG_START
#if    __hexagon__
       lastOut=Q6_R_tableidxb_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_6N_2x4_SQ0_BITS);
       unsigned int masked = Q6_R_extractu_RII((unsigned int)hold,8,MATCH_6N_2x4_SQ0_BITS+LB_BITS);
       *out++ = Q6_R_insert_RII(*(out + lastOut), masked,8,16);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_6N_2x4_SQ0_BITS+LB_BITS+8);
       debug("  %08X %d mask 16 bit 16 %04X\n", *(out-1), lastOut, masked);
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#else/*__hexagon__*/
       lastOut=TABLEIDXB_RII(lastOut,(unsigned int)hold,LB_BITS,MATCH_6N_2x4_SQ0_BITS);
       unsigned int masked = EXTRACTU_BITS((unsigned int)hold,8,MATCH_6N_2x4_SQ0_BITS+LB_BITS);
       *out++ = INSERT_BITS(*(out + lastOut), masked, 8,16);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_6N_2x4_SQ0_BITS+LB_BITS+8);
       debug("  %08X %d mask 16 bit 16 %04X\n", *(out-1), lastOut, masked);
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#endif/*__hexagon__*/
                    }
    MATCH_6N_2x4_SQ1:
                    {
       DEBUG_START
#if    __hexagon__
       unsigned int masked = Q6_R_extractu_RII((unsigned int)hold,8,MATCH_6N_2x4_SQ1_BITS);
#else/*__hexagon__*/
       unsigned int masked = EXTRACTU_BITS((unsigned int)hold,8,MATCH_6N_2x4_SQ1_BITS);
#endif/*__hexagon__*/

       if (masked == CHUNK_END_MARKER)
       {
          debug("  EXIT output_size: %d\n", (unsigned)out - (unsigned)out_buf);
          return (unsigned)out - (unsigned)out_buf;
       }
#if    __hexagon__
       masked = Q6_R_extractu_RII((unsigned int)hold,8,MATCH_6N_2x4_SQ1_BITS);
       *out++ = Q6_R_insert_RII(*(out + lastOut), masked, 8,16);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_6N_2x4_SQ1_BITS+8);
       debug("  %08X s %d mask 16 bit 16 %04X\n", *(out-1), lastOut, masked);
       unsigned int op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#else/*__hexagon__*/
       masked = EXTRACTU_BITS((unsigned int)hold,8,MATCH_6N_2x4_SQ1_BITS);
       *out++ = INSERT_BITS(*(out + lastOut), masked, 8,16);
       SKIP_HOLD_N_BITS_W_CHECK(MATCH_6N_2x4_SQ1_BITS+8);
       debug("  %08X s %d mask 16 bit 16 %04X\n", *(out-1), lastOut, masked);
       //unsigned int op1 = GET_BITS(OP1_BITS+1);
       unsigned int op1 = EXTRACTU_BITS((unsigned int)hold,OP1_BITS+1,0);
       JUMP_NEXT_DECODE
#endif/*__hexagon__*/
            }
        }
    }
