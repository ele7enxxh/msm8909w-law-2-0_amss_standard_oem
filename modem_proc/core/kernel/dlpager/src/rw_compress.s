	.file	"rw_compress_mod.c"
	.text
	.p2align 2
	.p2align 4,,15
	.globl deltaCompress
	.type	deltaCompress, @function
deltaCompress:
	// pretend args regs size (bytes) = 0
	// saved LR + FP regs size (bytes) = 0
	// callee saved regs size (bytes) = 48
	// local vars size (bytes) = 8
	// fixed args size (bytes) = 0
	{
		memd(r29+#-8) = r17:16
		memd(r29+#-16) = r19:18
		r29 = add(r29,#-56)
	}
	{
		memd(r29+#32) = r21:20
		memd(r29+#24) = r23:22
		r7 = r1
	}
	{
		memd(r29+#16) = r25:24
		memd(r29+#8) = r27:26
		r12 = r0
		r13 = add(r2,#-1)
	}
	{
		r26 = ##1023
		r27 = ##16383
	}
	{
		r4 = r0
		r9:8 = #0
		r6 = #0
		r3 = #0
	}
	{
		r16 = #0
		r28 = #0
		r14 = #0
		r10 = #0
	}
	{
		r17 = #3
		r25:24 = #13
		r23:22 = #9
		r21:20 = #5
	}
	{
		r19:18 = #1
		r5 = r13
		loop0(.L18,r13)
	}
	.falign
.L18:
	{
		r2 = memw(r4++#4)
		r0 = add(r3,#2)
		if (!cmp.eq(r2.new,#0)) jump:t .L3
	}
	{
		p0 = cmp.gtu(r0,#31)
		if (p0.new) memw(r7++#4) = r8
		if (p0.new) r3 = add(r3,#-30)
		if (p0.new) jump:nt .L34
	}
	{
		r3 = r0
	}:endloop0
	.falign
.L4:
	{
		r2 = add(r3,#2)
		r1:0 = #3
	}:endloop0 // start=.L18
	{
		r4 = memw(r12+r13<<#2)
		p0 = cmp.gtu(r2,#31)
		r9:8 |= asl(r1:0,r3)
		if (!p0.new) jump:nt .L35
	}
	{
		r2 = add(r3,#-30)
		memw(r7++#4) = r8
		r6 = add(r6,#1)
		r9:8 = lsr(r9:8,#32)
	}
	.falign
.L35:
	{
		r3 = r2
	}
	.falign
.L20:
	{
		r1:0 = combine(#0,r4)
	}
	{
		r9:8 |= asl(r1:0,r3)
		p0 = cmp.eq(r2,#0)
		r0 = add(r6,#1)
	}
	{
		memw(r7++#4) = r8
		r9:8 = lsr(r9:8,#32)
	}
	{
		if (!p0) memw(r7++#4) = r8
		if (!p0) r0 = add(r6,#2)
	}
	{
		r0 = asl(r0,#2)
		r27:26 = memd(r29+#8)
		r25:24 = memd(r29+#16)
	}
	{
		r23:22 = memd(r29+#24)
		r21:20 = memd(r29+#32)
	}
	{
		r19:18 = memd(r29+#40)
		r17:16 = memd(r29+#48)
		r29 = add(r29,#56)
		jumpr r31
	}
	.falign
.L3:
	{
		r0 = xor(r2,r10)
		r1 = xor(r2,r14)
		r11 = xor(r2,r28)
		if (!cmp.eq(r0.new,#0)) jump:t .L5
	}
	{
		r3 = add(r3,#4)
		r9:8 |= asl(r19:18,r3)
	}
	.falign
.L38:
	{
		p0 = cmp.gtu(r3,#31)
		if (p0.new) memw(r7++#4) = r8
		if (p0.new) r3 = add(r3,#-32)
		if (!p0.new) jump:t .L4
	}
	.falign
.L34:
	{
		r6 = add(r6,#1)
		r9:8 = lsr(r9:8,#32)
		
	}:endloop0 // start=.L18
	{jump .L4}
	.falign
.L5:
	{
		r15 = xor(r2,r16)
		p0 = cmp.eq(r1,#0); if(!p0.new) jump:t .L8
		if(r11==#0) jump:nt .L6
	}
	{
		r3 = add(r3,#4)
		r9:8 |= asl(r21:20,r3)
		jump .L38
	}
	.falign
.L6:
	{
		r3 = add(r3,#4)
		r9:8 |= asl(r23:22,r3)
		jump .L38
	}
	.falign
.L7:
	{
		r3 = add(r3,#4)
		r9:8 |= asl(r25:24,r3)
		jump .L38
	}
	.falign
.L8:
	{	
		if (r15 == #0) jump:nt .L7
		p0 = cmp.gtu(r0,r26)
		if (p0.new) jump:t .L9

	}
	{
		r10 = r2
		r2 =add(#2,asl(r2,#4))
		r3 = add(r3,#14)
		r1 = #0
	}
	{
		r2 = add(r3,#-14)
		r0 = and(r2,r27)
		p0 = cmp.gtu(r3,#31)
	}
	{
		r9:8 |= asl(r1:0,r2)
		
		if (!p0) jump .L4
	}
	{
		memw(r7++#4) = r8
		r3 = add(r3,#-32)
		r6 = add(r6,#1)
		r9:8 = lsr(r9:8,#32)
	}:endloop0 // start=.L18
	{

		jump .L4
	}
	.falign
.L9:
	{
		p0 = cmp.gtu(r1,r26)
		if (p0.new) jump:t .L10
	}
	{
		r14 = r2
		r2 =add(#6,asl(r2,#4))
		r3 = add(r3,#14)
		r1 = #0
	}
	{
		r2 = add(r3,#-14)
		r0 = and(r2,r27)
		p0 = cmp.gtu(r3,#31)
	}
	{
		r9:8 |= asl(r1:0,r2)
		if (!p0) jump .L4
	}
	{
		memw(r7++#4) = r8
		r6 = add(r6,#1)
		r9:8 = lsr(r9:8,#32)
		r3 = add(r3,#-32)
	}:endloop0 // start=.L18
	{
		jump .L4
	}
	.falign
.L10:
	{
		r11 = and(r11,##-1024)
		if (cmp.eq(r11.new,#0)) jump:t .L42
	}
	{
		r15 = and(r15,##-1024)
		if (!cmp.eq(r15.new,#0)) jump:t .L12
	}
	{
		r16 = r2
		r3 = add(r3,#14)
		r2 =add(#14,asl(r2,#4))
		r1 = #0
	}
	{

		r0 = and(r2,r27)
		p0 = cmp.gtu(r3,#31)
		r2 = add(r3,#-14)
	}
	{
		r9:8 |= asl(r1:0,r2)
		if (!p0) jump .L4
	}
	{
		memw(r7++#4) = r8
		r3 = add(r3,#-32)
		r6 = add(r6,#1)
		r9:8 = lsr(r9:8,#32)
	}:endloop0 // start=.L18
	{
		
		jump .L4
	}
	.falign
.L42:
	{
		r28 = r2
		r2 =add(#10,asl(r2,#4))
		r3 = add(r3,#14)
		r1 = #0
	}
	{
		r0 = and(r2,r27)
		p0 = cmp.gtu(r3,#31)
		r2 = add(r3,#-14)
	}
	{
		r9:8 |= asl(r1:0,r2)
		if (!p0) jump .L4
	}
	{
		memw(r7++#4) = r8
		r3 = add(r3,#-32)
		r6 = add(r6,#1)
		r9:8 = lsr(r9:8,#32)
	}:endloop0 // start=.L18
	{
		
		jump .L4
	}
	.falign
.L12:
	{
		r17 = add(r17,#1)		
		r11 = add(r3,#2)
		r1:0 = #3
	}
	{
		r17 = and(r17,#3)
		r9:8 |= asl(r1:0,r3)
	}
	{
		r1:0 = combine(#0,r2)
		p0 = cmp.eq(r17,#2)
		if (p0.new) r28 = r2
		if (p0.new) jump:nt .L13
	}
	{
		p0 = cmp.eq(r17,#3)
		if (p0.new) r16 = r2
		if (p0.new) jump:nt .L13
	}
	{
		p0 = cmp.eq(r17,#1)
		if (!p0.new) r10 = r2
		if (p0.new) r14 = r2
	}
	.falign
.L13:
	{
		
		p0 = cmp.gtu(r11,#31)
		if (!p0.new) r3 = r11
		if (!p0.new) jump:nt .L17
	}
	{
		memw(r7++#4) = r8
		r3 = add(r3,#-30)
		r6 = add(r6,#1)
		r9:8 = lsr(r9:8,#32)
	}
	.falign
.L17:
	{
		r9:8 |= asl(r1:0,r3)
	}
	{
		memw(r7++#4) = r8
		r9:8 = lsr(r9:8,#32)
		r6 = add(r6,#1)
		
	}:endloop0 // start=.L18
	{
		jump .L4
	}
	.size	deltaCompress, .-deltaCompress
	.ident	"GCC: (Sourcery CodeBench Lite 5.1 2012.03-129) 4.6.1"
	.section	.note.GNU-stack,"",@progbits
