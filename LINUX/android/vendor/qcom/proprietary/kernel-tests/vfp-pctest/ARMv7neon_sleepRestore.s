/* kernel-tests/vfp-pctest/ARMv7neon_sleepRestore.s
 *
 * Copyright (c) 2008-2009 Qualcomm Technologies, Inc.
 * All Rights Reserved.  Qualcomm Technologies Proprietary and Confidential.
 */

	.file "ARMv7neon_sleepRestore.s"
	.text
	.align 4

/***************************************************************************
 * Description : Assembly File having single VFP Instructions listed, for
 *               testing VFP context switching.
 * Inputs      : inputs - register file seeds from the constant array of
 *               floating point operations.
 * Outputs     : reg_dump - the final contents of register file that are
 *               dumped onto memory
 * STATUS      : the version of the code after modifications taking care of
 *               initialising ARM registers before using them in venum
 *               assembly.
 ***************************************************************************/
	.globl neon_regLoad
	.type neon_regLoad, %function
neon_regLoad:
	stmfd    sp!,  {r4-r10}

	vld1.32  {d0,  d1, d2, d3},      [r0]!  @ load from memory to FPU
	vld1.32  {d4,  d5, d6, d7},      [r0]!
	vld1.32  {d8,  d9, d10, d11},    [r0]!
	vld1.32  {d12, d13, d14, d15},   [r0]!
	vld1.32  {d16, d17, d18, d19},   [r0]!
	vld1.32  {d20, d21, d22, d23},   [r0]!
	vld1.32  {d24, d25, d26, d27},   [r0]!
	vld1.32  {d28, d29, d30, d31},   [r0]!


	ldmfd    sp!, {r4-r10}
	bx       r14
.size neon_regLoad, . - neon_regLoad

	.globl neon_regStore
	.type neon_regStore, %function
neon_regStore:
	stmfd    sp!,  {r4-r10}

	vst1.32  {d0-d3},   [r0]!               @ store from fpu to memory
	vst1.32  {d4-d7},   [r0]!
	vst1.32  {d8-d11},  [r0]!
	vst1.32  {d12-d15}, [r0]!

	vst1.32  {d16-d19}, [r0]!
	vst1.32  {d20-d23}, [r0]!
	vst1.32  {d24-d27}, [r0]!
	vst1.32  {d28-d31}, [r0]!

	ldmfd    sp!, {r4-r10}
	bx       r14
.size neon_regStore, . - neon_regStore


