/*===========================================================================
 * FILE:         q6zip_uncompress.S
 *
 * SERVICES:     DL PAGER
 *
 * DESCRIPTION:  q6zip uncompressor
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/src/q6zip_uncompress2.S#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/14   ao      Initial revision (collaboration with Ric Senior)
03/18/14   rs      Updated to version q6zip3v3
04/25/14   rs      Updated to version q6zip3v4
8/28/14    rr      Updated to version q6zip3v5
01/01/15   ao      Updated to version q6zip3v6
===========================================================================*/

// These constants need to match values in "../compressor/q6zip_constants.py"
#define DICT1_BITS 10
#define DICT2_BITS 14
#define LB_BITS 8
#define DCFETCH_DELTA 64

#define CHUNK_END_MARKER 255

#define OP1_BITS 3
#define OP2_BITS 3
#define OP3_BITS 2

#define OP_LEVEL0 4

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
/*
    MATCH_6N_2x0_SQ0    389698       000 0
        MATCH_8N_SQ0    658700       001 1
    MATCH_5N_3x0_SQ0    239840      0010 2
            NO_MATCH    730198       011 3
         DICT1_MATCH    468710       100 4
         DICT2_MATCH    308708      0101 5
    MATCH_6N_2x0_SQ1    526647       110 6
        MATCH_8N_SQ1    758299       111 7
    MATCH_4N_4x0_SQ1     46241    001010 10
    MATCH_4N_4x0_SQ0    173634     01101 13
    MATCH_5N_3x0_SQ1    196326     11101 29
    MATCH_6N_2x2_SQ0     84638    111010 58
    MATCH_6N_2x4_SQ0     52167    101010 42
    MATCH_6N_2x2_SQ1     38398   1011010 90
    MATCH_6N_2x4_SQ1     25603   0011010 26

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
            &&LEVEL_B,           //13 0xD==1101 MATCH_6N_2x2_SQ1 = (0b1011010, 7, 0xFFFF00FF, "MATCH_6N_2x2_SQ1"), #MATCH_6Nx1
            &&MATCH_6N_2x0_SQ1,  //14
            &&MATCH_8N_SQ1,      //15
*/

  .section  .rodata
  .p2align 5 // align to cache line (32 bytes)
.jump_table:
  .word .MATCH_6N_2x0_SQ0
  .word .MATCH_8N_SQ0
  .word .MATCH_5N_3x0_SQ0
  .word .NO_MATCH
  .word .DICT1_MATCH
  .word .DICT2_MATCH
  .word .MATCH_6N_2x0_SQ1
  .word .MATCH_8N_SQ1
  .word .MATCH_6N_2x0_SQ0
  .word .MATCH_8N_SQ0
  .word .LEVEL_A
  .word .NO_MATCH
  .word .DICT1_MATCH
  .word .LEVEL_B
  .word .MATCH_6N_2x0_SQ1
  .word .MATCH_8N_SQ1

// Registers usage through q6zip_uncompress()
// r0       out
// r1       lastOut
// r2       bits
// r3       scratch: opcode to jump to, bits
// r4       scratch: hold
// r5       scratch: *(out + lastOut)
// r6       scratch: op1
// r7       scratch: masked
// r8       scratch: bits
// r10      base of jump_table
// r16      in
// r17      out_buf
// r18/r19  hold
// r20/r21  dict1_base/dict2_base

/***************************************************************************
* q6zip_uncompress()
*   r0=*out_buf
*   r1=*in_buf
*   r2=*dict
*   r3=*state
***************************************************************************/
  .text
  .p2align 4 // align to dword
  .globl q6zip_uncompress
  .type q6zip_uncompress, @function
q6zip_uncompress:
  {
    allocframe(#32)                // no stack used by this function, only saves regs
    call __save_r16_through_r21
  }
  r20 = r2                         // save dict1_base
  r17:16 = combine(r0,r1)          // save out_buf/in_buf
#define DICT1_SIZE (1 << (DICT1_BITS + 2))
  r21 = add(r2,#DICT1_SIZE)        // save dict2_base
  r18 = memw(r3)                   // hold = ((uint64_t)state[1]) << 32 | (uint64_t)state[0];
  r19 = memw(r3+#4)                // hold = ((uint64_t)state[1]) << 32 | (uint64_t)state[0];
  r2 = memw(r3+#8)                 // bits = state[2];
  r1 = memw(r3+#12)                // lastOut = state[3];

  r10 = ##.jump_table              // JUMP_NEXT_DECODE
  r3 = extractu(r18,#OP_LEVEL0,#0) // op1 = Q6_R_extractu_RII((unsigned int)hold,OP1_BITS+1,0);
  r3 = memw(r10+r3<<#2)            // JUMP_NEXT_DECODE
  jumpr r3                         // JUMP_NEXT_DECODE

  .falign
.MATCH_8N_SQ0_NO_RELOAD_HOLD:
  {
    r4 = memw(r0+r1<<#2)        // *out++ = *(out+lastOut)
    memw(r0++#4) = r4.new       // *out++ = *(out+lastOut)
    jumpr r3                    // JUMP_NEXT_DECODE
  }
  .falign
.MATCH_6N_2x0_SQ0:
  {
    r4 = r18 // masked = Q6_R_extractu_RII(hold, 8, MATCH_6N_2x0_SQ0_BITS+LB_BITS)
    r3 = add(r2,#-(MATCH_6N_2x0_SQ0_BITS+LB_BITS+8))  // bits -= n (SKIP_BITS_W_CHECK)
    r1=tableidxb(r18,#LB_BITS,#MATCH_6N_2x0_SQ0_BITS) // lastOut=Q6_R_tableidxb_RII(lastOut, hold
    r6 = extractu(r18,#OP_LEVEL0,#(8+MATCH_6N_2x0_SQ0_BITS+LB_BITS)) // op1 = Q6_R_extractu_RII(hold
  }
  {
    r19:18 = lsr(r19:18,#(MATCH_6N_2x0_SQ0_BITS+LB_BITS+8)) // hold >>= n (SKIP_HOLD)
    r5 = memw(r0+r1<<#2) // *out++ = Q6_R_insert_RII(*(out + lastOut), masked, 8,0)
    r7 = extractu(r4,#8,#(MATCH_6N_2x0_SQ0_BITS+LB_BITS)) // masked = Q6_R_extractu_RII(hold, 8
    r6 = memw(r10+r6<<#2)                                 // *jumpPtr=jump_table[op1] (JUMP_NEXT_DECODE)
  }
  {
    p0 = cmp.gt(r3,#31)                    // if (UNLIKELY(bits < 32)) (SKIP_BITS_W_CHECK(n))
    r5 = insert(r7,#8,#0)                  // *out++ = Q6_R_insert_RII(*(out + lastOut), masked, 8,0)
    memw(r0++#4) = r5.new                  // *out++ = Q6_R_insert_RII(*(out + lastOut), masked, 8,0)
    if (!p0.new) jump:t .MATCH_6N_2x0_SQ0_RELOAD_HOLD // if (UNLIKELY(bits < 32)) (SKIP_BITS_W_CHECK(n))
  }
  {
    r2 = r3                     // bits -= n (SKIP_BITS_W_CHECK)
    jumpr r6                    // goto *jumpPtr (JUMP_NEXT_DECODE)
  }
  .falign
.MATCH_8N_SQ0:
  {
    r6 = add(r2,#-(MATCH_8N_SQ0_BITS+LB_BITS))
    r4 = r18
    r19:18 = lsr(r19:18,#(MATCH_8N_SQ0_BITS+LB_BITS))
  }
  {
    p0 = cmp.gt(r6,#31)
    r3 = extractu(r4,#OP_LEVEL0,#(MATCH_8N_SQ0_BITS+LB_BITS))
    r1=tableidxb(r4,#LB_BITS,#MATCH_8N_SQ0_BITS)
    if (p0.new) r2 = r6
  }
  {
    if (!p0) r2 = add(r2,#(32-(MATCH_8N_SQ0_BITS+LB_BITS)))
    if (p0) jump .MATCH_8N_SQ0_NO_RELOAD_HOLD
    r3 = memw(r10+r3<<#2)
  }
  {
    r4 = memw(r16++#4)
    r5 = #0
  }
  {
    r4 = memw(r0+r1<<#2)
    r19:18 |= asl(r5:4,r6)
    dcfetch(r16+#DCFETCH_DELTA)
  }
  {
    memw(r0++#4) = r4
    jumpr r3
  }
  .falign
.MATCH_5N_3x0_SQ0:
  {
    r4 = r18
    r3 = add(r2,#-(MATCH_5N_3x0_SQ0_BITS+LB_BITS+12))
    r19:18 = lsr(r19:18,#(MATCH_5N_3x0_SQ0_BITS+LB_BITS+12))
    r1=tableidxb(r18,#LB_BITS,#MATCH_5N_3x0_SQ0_BITS)
  }
  {
    r5 = extractu(r4,#12,#(MATCH_5N_3x0_SQ0_BITS+LB_BITS))
    p0 = cmp.gt(r3,#31)
    r4 = memw(r0+r1<<#2)
    if (!p0.new) jump:t .MATCH_5N_3x0_SQ0_RELOAD_HOLD
  }
  {
    r4 = insert(r5,#12,#0)
    memw(r0++#4) = r4.new
    r5 = #0
    r6 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    r2 = r3
    p0 = cmp.eq(r6,#1)
    r3 = memw(r10+r6<<#2)
    if (p0.new) jump:t .MATCH_8N_SQ0
  }
  jumpr r3
  .falign
.MATCH_4N_4x0_SQ0_NO_RELOAD_HOLD:
  {
    r2 = r3
    r3 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    p0 = cmp.eq(r3,#1)
    r3 = memw(r10+r3<<#2)
    if (p0.new) jump:t .MATCH_8N_SQ0
  }
  jumpr r3
  .falign
.MATCH_6N_2x4_SQ0_CONT_NO_RELOAD_HOLD:
  {
    r3 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    p0 = cmp.eq(r3,#1)
    r3 = memw(r10+r3<<#2)
    if (p0.new) jump:t .MATCH_8N_SQ0
  }
  jumpr r3
  .falign
.DICT2_MATCH:
  {
    r3 = extractu(r18,#DICT2_BITS,#DICT2_MATCH_BITS)
    r6 = add(r2,#-(DICT2_MATCH_BITS+DICT2_BITS))
    r19:18 = lsr(r19:18,#(DICT2_MATCH_BITS+DICT2_BITS))
  }
  {
    r3 = memw(r21+r3<<#2)
    p0 = cmp.gt(r6,#31)
    if (!p0.new) jump:nt .DICT2_MATCH_RELOAD_HOLD
    r7 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    memw(r0++#4) = r3
    r3 = memw(r10+r7<<#2)
    r2 = r6
  }
  jumpr r3
  .falign
.JUMP_NEXT:
  {
    r2 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    r3 = memw(r10+r2<<#2)
    r2 = r6
  }
  jumpr r3
  .falign
.DICT1_MATCH:
  {
    r3 = r18
    r6 = add(r2,#-(DICT1_MATCH_BITS+DICT1_BITS))
    r19:18 = lsr(r19:18,#(DICT1_MATCH_BITS+DICT1_BITS))
  }
  {
    r4 = extractu(r3,#OP_LEVEL0,#(DICT1_MATCH_BITS+DICT1_BITS))
    p0 = cmp.gt(r6,#31)
    r7 = extractu(r3,#DICT1_BITS,#DICT1_MATCH_BITS)
  }
  {
    r3 = memw(r10+r4<<#2)
    if (!p0) r4 = memw(r16++#4)
    r5 = #0
    if (!p0) jump .DICT1_MATCH_RELOAD_HOLD
  }
  {
    r7 = memw(r20+r7<<#2)
    memw(r0++#4) = r7.new
    r2 = r6
    jumpr r3
  }
  .falign
.MATCH_8N_SQ1:
  {
    r6 = add(r2,#-MATCH_8N_SQ1_BITS)
    r3 = extractu(r18,#OP_LEVEL0,#MATCH_8N_SQ1_BITS)
    r19:18 = lsr(r19:18,#MATCH_8N_SQ1_BITS)
    r5 = #0
  }
  {
    p0 = cmp.gt(r6,#31)
    if (!p0.new) r4 = memw(r16++#4)
    if (!p0.new) jump:nt .MATCH_8N_SQ1_RELOAD_HOLD
    r3 = memw(r10+r3<<#2)
  }
  {
    r4 = memw(r0+r1<<#2)
    r2 = r6
    memw(r0++#4) = r4.new
    jumpr r3
  }
  .falign
.NO_MATCH:
  {
    r7:6 = lsr(r19:18,#NO_MATCH_BITS)
    r8 = add(r2,#-NO_MATCH_BITS)
    r3 = #0
    p0 = cmp.gt(r2, #(31+NO_MATCH_BITS));
  }
  {
    if (!p0) jump .NO_MATCH_RELOAD_HOLD_TWICE
    if (p0) memw(r0++#4) = r6
    if (p0) r18 = add(r8,#-32)
    if (p0) r2 = memw(r16++#4)
  }
  {
    r19:18 = asl(r3:2,r18)
  }
  {
    r19:18 |= lsr(r7:6,#32)
  }
  {
    r2 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    p0 = cmp.eq(r2,#1)
    r3 = memw(r10+r2<<#2)
    r2 = r8
    if (p0.new) jump:t .MATCH_8N_SQ0
  }
  {
    jumpr r3
  }
  .falign
.LEVEL_A:
  {
    r4 = extractu(r18,#2,#4)
    r3 = extractu(r18,#8,#(LB_BITS+MATCH_6N_2x2_SQ0_BITS))
    if (cmp.eq(r4.new,#3)) jump:t .MATCH_6N_2x2_SQ0
  }
  {
    p0 = cmp.eq(r4,#2)
    r3 = extractu(r18,#8,#(LB_BITS+MATCH_6N_2x4_SQ0_BITS))
    r6 = add(r2,#-(LB_BITS+MATCH_6N_2x4_SQ0_BITS+8))
    if (p0.new) jump:nt .MATCH_6N_2x4_SQ0
  }
  {
    p0 = cmp.eq(r4,#0)
    r4 = extractu(r18,#16,#MATCH_4N_4x0_SQ1_BITS)
    if (p0.new) r3 = memw(r0+r1<<#2)
    if (p0.new) jump:nt .MATCH_4N_4x0_SQ1
  }
  {
    p0 = !tstbit(r18,#6)
    if (!p0.new) r3 = memw(r0+r1<<#2)
    r6 = add(r2,#-(MATCH_6N_2x4_SQ1_BITS+8))
    if (p0.new) jump:nt .MATCH_6N_2x4_SQ1
  }
  {
    r4 = extractu(r18,#8,#MATCH_6N_2x4_SQ1_BITS)
    r19:18 = lsr(r19:18,#(MATCH_6N_2x4_SQ1_BITS+8))
    p0 = cmp.gt(r6,#31)
    r5 = #0
  }
  {
    r3 = insert(r4,#8,#8)
    memw(r0++#4) = r3.new
    if (p0) jump .JUMP_NEXT
    if (!p0) r4 = memw(r16++#4)

  }
  {
    r19:18 |= asl(r5:4,r6)
    dcfetch(r16+#DCFETCH_DELTA)
    r2 = add(r2,#(32-(MATCH_6N_2x4_SQ1_BITS+8)))
    r6 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    r3 = memw(r10+r6<<#2)
  }
  jumpr r3
  .falign
.LEVEL_B:
  {
    r3 = and(r18,#16)
    r4 = extractu(r18,#12,#5)
    if (cmp.eq(r3.new,#0)) jump:t .MATCH_4N_4x0_SQ0
  }
  {
    r3 = memw(r0+r1<<#2)
    r6 = add(r2,#-(MATCH_5N_3x0_SQ1_BITS+12))
    r19:18 = lsr(r19:18,#(MATCH_5N_3x0_SQ1_BITS+12))
  }
  {
    r3 = insert(r4,#12,#0)
    p0 = cmp.gt(r6,#31)
    memw(r0++#4) = r3.new
    if (p0.new) jump:t .JUMP_NEXT
  }
  {
    r2 = add(r2,#32-(MATCH_5N_3x0_SQ1_BITS+12))
    r4 = memw(r16++#4)
    r5 = #0
    r3 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    r19:18 |= asl(r5:4,r6)
    dcfetch(r16+#DCFETCH_DELTA)
    r3 = memw(r10+r3<<#2)
  }
  jumpr r3
  .falign
.MATCH_6N_2x0_SQ1:
  {
    r6 = add(r2,#-(MATCH_6N_2x0_SQ1_BITS+8))
    r3 = extractu(r18,#OP_LEVEL0,#(MATCH_6N_2x0_SQ1_BITS+8))
    r5 = extractu(r18,#8,#MATCH_6N_2x0_SQ1_BITS)
  }
  {
    r19:18 = lsr(r19:18,#(MATCH_6N_2x0_SQ1_BITS+8))
    r4 = memw(r0+r1<<#2)
    p0 = cmp.gt(r6,#31)
    r3 = memw(r10+r3<<#2)
  }
  {
    r4 = insert(r5,#8,#0)
    memw(r0++#4) = r4.new
    if (!p0) jump .MATCH_6N_2x0_SQ1_RELOAD_HOLD
  }
  {
    r2 = r6
    jumpr r3
  }
  .falign
.MATCH_6N_2x0_SQ1_RELOAD_HOLD:
  {
    r5 = #0
    r4 = memw(r16++#4)
  }
  {
    r19:18 |= asl(r5:4,r6)
    dcfetch(r16+#DCFETCH_DELTA)
    r2 = add(r2,#(32-(MATCH_6N_2x0_SQ1_BITS+8)))
    jumpr r3
  }
  .falign
.MATCH_4N_4x0_SQ0:
  {
    r4 = r18
    r3 = add(r2,#-(LB_BITS+MATCH_4N_4x0_SQ0_BITS+16))
    r19:18 = lsr(r19:18,#(LB_BITS+MATCH_4N_4x0_SQ0_BITS+16))
  }
  {
    r1=tableidxb(r4,#LB_BITS,#MATCH_4N_4x0_SQ0_BITS)
    r5 = extractu(r4,#16,#(LB_BITS+MATCH_4N_4x0_SQ0_BITS))
    p0 = cmp.gt(r3,#31)
  }
  {
    r4 = memw(r0+r1<<#2)
  }
  {
    r4 = insert(r5,#16,#0)
    memw(r0++#4) = r4.new
    r5 = #0
    if (p0) jump .MATCH_4N_4x0_SQ0_NO_RELOAD_HOLD
  }
  {
    r4 = memw(r16++#4)
    r2 = add(r2,#(32-(LB_BITS+MATCH_4N_4x0_SQ0_BITS+16)))
  }
  {
    r19:18 |= asl(r5:4,r3)
    dcfetch(r16+#DCFETCH_DELTA)
  }
  {
    r3 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    p0 = cmp.eq(r3,#1)
    r3 = memw(r10+r3<<#2)
    if (p0.new) jump:t .MATCH_8N_SQ0
  }
  jumpr r3
  .falign
.MATCH_4N_4x0_SQ1:
  {
    r6 = add(r2,#-(MATCH_4N_4x0_SQ1_BITS+16))
    r19:18 = lsr(r19:18,#(MATCH_4N_4x0_SQ1_BITS+16))
    r3 = insert(r4,#16,#0)
    r5 = #0
  }
  {
    p0 = cmp.gt(r6,#31)
    memw(r0++#4) = r3
    if (p0.new) jump:t .JUMP_NEXT
    if (!p0.new) r4 = memw(r16++#4)
  }
  {
    r19:18 |= asl(r5:4,r6)
    dcfetch(r16+#DCFETCH_DELTA)
    r6 = add(r2,#(32-(MATCH_4N_4x0_SQ1_BITS+16)))
  }
  {
    r2 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    r3 = memw(r10+r2<<#2)
    r2 = r6
  }
  jumpr r3
  .falign
.MATCH_6N_2x2_SQ0:
  {
    r1=tableidxb(r18,#LB_BITS,#MATCH_6N_2x2_SQ0_BITS)
    r6 = add(r2,#-(LB_BITS+MATCH_6N_2x2_SQ0_BITS+8))
    r19:18 = lsr(r19:18,#(LB_BITS+MATCH_6N_2x2_SQ0_BITS+8))
  }
  {
    r4 = memw(r0+r1<<#2)
    p0 = cmp.gt(r6,#31)
  }
  {
    r4 = insert(r3,#8,#8)
  }
  .falign
.MATCH_6N_2x4_SQ0_CONT:
  {
    memw(r0++#4) = r4
    if (p0) r2 = r6
    r5 = #0
    if (p0) jump .MATCH_6N_2x4_SQ0_CONT_NO_RELOAD_HOLD
  }
  {
    r4 = memw(r16++#4)
    r2 = add(r2,#(32-(LB_BITS+MATCH_6N_2x4_SQ0_BITS+8)))
  }
  {
    r19:18 |= asl(r5:4,r6)
    dcfetch(r16+#DCFETCH_DELTA)
  }
  {
    r3 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    p0 = cmp.eq(r3,#1)
    r3 = memw(r10+r3<<#2)
    if (p0.new) jump:t .MATCH_8N_SQ0
  }
  jumpr r3
  .falign
.MATCH_6N_2x4_SQ0:
  {
    r1=tableidxb(r18,#LB_BITS,#MATCH_6N_2x4_SQ0_BITS)
    p0 = cmp.gt(r6,#31)
    r19:18 = lsr(r19:18,#(LB_BITS+MATCH_6N_2x4_SQ0_BITS+8))
  }
  {
    r4 = memw(r0+r1<<#2)
  }
  {
    r4 = insert(r3,#8,#16)
    jump .MATCH_6N_2x4_SQ0_CONT
  }
  .falign
.MATCH_6N_2x4_SQ1:
  {
    r3 = sub(r0,r17)
    r4 = extractu(r18,#8,#MATCH_6N_2x4_SQ1_BITS)
  }
  {
    p0 = cmp.eq(r4,#CHUNK_END_MARKER)
    if (p0.new) jump:nt .EXIT
  }
  {
    r3 = memw(r0+r1<<#2)
    r6 = add(r2,#-(MATCH_6N_2x4_SQ1_BITS+8))
    r19:18 = lsr(r19:18,#(MATCH_6N_2x4_SQ1_BITS+8))
  }
  {
    r3 = insert(r4,#8,#16)
  }
  {
    p0 = cmp.gt(r6,#31)
    memw(r0++#4) = r3
    r5 = #0
    if (p0.new) jump:t .JUMP_NEXT
  }
  {
    r4 = memw(r16++#4)
  }
  {
    r19:18 |= asl(r5:4,r6)
    dcfetch(r16+#DCFETCH_DELTA)
    r6 = add(r2,#(MATCH_6N_2x4_SQ1_BITS+10))
  }
  {
    r2 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    r3 = memw(r10+r2<<#2)
    r2 = r6
  }
  jumpr r3
  .falign
.NO_MATCH_RELOAD_HOLD_TWICE:
  {
    r5 = #0
    r4 = memw(r16++#4)
  }
  {
    r7:6 |= asl(r5:4,r8)
    dcfetch(r16+#DCFETCH_DELTA)
    r8 = add(r2,#(32-NO_MATCH_BITS))
  }
  {
    memw(r0++#4) = r6
    r18 = add(r8,#-32)
    r2 = memw(r16++#4)
    r3 = #0
  }
  {
    r19:18 = asl(r3:2,r18)
  }
  {
    r19:18 |= lsr(r7:6,#32)
  }
  {
    r2 = extractu(r18,#OP_LEVEL0,#0)
  }
  {
    p0 = cmp.eq(r2,#1)
    r3 = memw(r10+r2<<#2)
    r2 = r8
    if (p0.new) jump:t .MATCH_8N_SQ0
  }
  {
    jumpr r3
  }
  .falign
.EXIT:
  r0 = sub(r0, r17) // return output words
  jump __restore_r16_through_r21_and_deallocframe
  .falign
.MATCH_8N_SQ1_RELOAD_HOLD:
  {
    r19:18 |= asl(r5:4,r6)
    r4 = memw(r0+r1<<#2)
  }
  {
    r2 = add(r2,#(32-MATCH_8N_SQ1_BITS))
    memw(r0++#4) = r4
    jumpr r3
  }
  .falign
.DICT1_MATCH_RELOAD_HOLD:
  {
    r19:18 |= asl(r5:4,r6)
    dcfetch(r16+#DCFETCH_DELTA)
    r6 = add(r2,#(32-(DICT1_MATCH_BITS+DICT1_BITS)))
  }
  {
    r7 = memw(r20+r7<<#2)
    memw(r0++#4) = r7.new
    r2 = r6
    jumpr r3
  }
  .falign
.DICT2_MATCH_RELOAD_HOLD:
  {
    memw(r0++#4) = r3
    r4 = memw(r16++#4)
    r5 = #0
  }
  {
    r19:18 |= asl(r5:4,r6)
    dcfetch(r16+#DCFETCH_DELTA)
    r2 = add(r2,#(32-(DICT2_MATCH_BITS+DICT2_BITS)))
    r6 = memw(r10+r7<<#2)
  }
  jumpr r6
  .falign
.MATCH_5N_3x0_SQ0_RELOAD_HOLD:
  {
    r6 = extractu(r18,#OP_LEVEL0,#0)
    r4 = insert(r5,#12,#0)
    memw(r0++#4) = r4.new
  }
  {
    r5 = #0
    r4 = memw(r16++#4)
    r2 = add(r2,#(32-(MATCH_5N_3x0_SQ0_BITS+LB_BITS+12)))
    r6 = memw(r10+r6<<#2)
  }
  {
    r19:18 |= asl(r5:4,r3)
    jumpr r6
  }
.MATCH_6N_2x0_SQ0_RELOAD_HOLD:
  {
    r4 = memw(r16++#4)
    r5 = #0
    r2 = add(r2,#(32-(MATCH_6N_2x0_SQ0_BITS+LB_BITS+8)))
  }
  {
    r19:18 |= asl(r5:4,r3)
    dcfetch(r16+#DCFETCH_DELTA)
    jumpr r6
  }
  .size q6zip_uncompress, .-q6zip_uncompress

