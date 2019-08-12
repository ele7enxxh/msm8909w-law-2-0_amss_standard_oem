/* kernel-tests/vfp-pctest/arm11vfp_sleepRestore.s
 *
 * Copyright (c) 2008-2009 Qualcomm Technologies, Inc.
 * All Rights Reserved.  Qualcomm Technologies Proprietary and Confidential.
 */

	.file "arm11vfp_sleepRestore.s"
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
 ****************************************************************************/
	.text
	.align 2
	.globl vfpv2_regLoad
	.type vfpv2_regLoad, %function
vfpv2_regLoad:
	stmfd          sp!,  {r4-r10}
	mov            r5,   r0


	vldr           s0, [r0] 	@ load s0 with memory contents
	add            r0, r0, #4
	vldr           s1, [r0]
	add            r0, r0, #4
	vldr           s2, [r0]
	add            r0, r0, #4
	vldr           s3, [r0]
	add            r0, r0, #4
	vldr           s4, [r0]
	add            r0, r0, #4
	vldr           s5, [r0]
	add            r0, r0, #4
	vldr           s6, [r0]
	add            r0, r0, #4
	vldr           s7, [r0]
	add            r0, r0, #4

	vldr           s8, [r0]
	add            r0, r0, #4
	vldr           s9, [r0]
	add            r0, r0, #4
	vldr           s10, [r0]
	add            r0, r0, #4
	vldr           s11, [r0]
	add            r0, r0, #4
	vldr           s12, [r0]
	add            r0, r0, #4
	vldr           s13, [r0]
	add            r0, r0, #4
	vldr           s14, [r0]
	add            r0, r0, #4
	vldr           s15, [r0]
	add            r0, r0, #4

	vldr           s16, [r0]
	add            r0, r0, #4
	vldr           s17, [r0]
	add            r0, r0, #4
	vldr           s18, [r0]
	add            r0, r0, #4
	vldr           s19, [r0]
	add            r0, r0, #4
	vldr           s20, [r0]
	add            r0, r0, #4
	vldr           s21, [r0]
	add            r0, r0, #4
	vldr           s22, [r0]
	add            r0, r0, #4
	vldr           s23, [r0]
	add            r0, r0, #4

	vldr           s24, [r0]
	add            r0, r0, #4
	vldr           s25, [r0]
	add            r0, r0, #4
	vldr           s26, [r0]
	add            r0, r0, #4
	vldr           s27, [r0]
	add            r0, r0, #4
	vldr           s28, [r0]
	add            r0, r0, #4
	vldr           s29, [r0]
	add            r0, r0, #4
	vldr           s30, [r0]
	add            r0, r0, #4
	vldr           s31, [r0]
	add            r0, r0, #4


	ldmfd          sp!, {r4-r10}
	bx             r14
.size vfpv2_regLoad, . - vfpv2_regLoad

       .text
       .align 2
       .globl vfpv2_regStore
       .type vfpv2_regStore, %function
vfpv2_regStore:
	stmfd          sp!,  {r4-r10}
	mov            r5,   r0


	vstr           s0, [r0] 	@ store S0 from memory
	add            r0, r0, #4
	vstr           s1, [r0]
	add            r0, r0, #4
	vstr           s2, [r0]
	add            r0, r0, #4
	vstr           s3, [r0]
	add            r0, r0, #4
	vstr           s4, [r0]
	add            r0, r0, #4
	vstr           s5, [r0]
	add            r0, r0, #4
	vstr           s6, [r0]
	add            r0, r0, #4
	vstr           s7, [r0]
	add            r0, r0, #4

	vstr           s8, [r0]
	add            r0, r0, #4
	vstr           s9, [r0]
	add            r0, r0, #4
	vstr           s10, [r0]
	add            r0, r0, #4
	vstr           s11, [r0]
	add            r0, r0, #4
	vstr           s12, [r0]
	add            r0, r0, #4
	vstr           s13, [r0]
	add            r0, r0, #4
	vstr           s14, [r0]
	add            r0, r0, #4
	vstr           s15, [r0]
	add            r0, r0, #4

	vstr           s16, [r0]
	add            r0, r0, #4
	vstr           s17, [r0]
	add            r0, r0, #4
	vstr           s18, [r0]
	add            r0, r0, #4
	vstr           s19, [r0]
	add            r0, r0, #4
	vstr           s20, [r0]
	add            r0, r0, #4
	vstr           s21, [r0]
	add            r0, r0, #4
	vstr           s22, [r0]
	add            r0, r0, #4
	vstr           s23, [r0]
	add            r0, r0, #4

	vstr           s24, [r0]
	add            r0, r0, #4
	vstr           s25, [r0]
	add            r0, r0, #4
	vstr           s26, [r0]
	add            r0, r0, #4
	vstr           s27, [r0]
	add            r0, r0, #4
	vstr           s28, [r0]
	add            r0, r0, #4
	vstr           s29, [r0]
	add            r0, r0, #4
	vstr           s30, [r0]
	add            r0, r0, #4
	vstr           s31, [r0]
	add            r0, r0, #4

	ldmfd          sp!, {r4-r10}
	bx             r14
.size vfpv2_regStore, . - vfpv2_regStore


