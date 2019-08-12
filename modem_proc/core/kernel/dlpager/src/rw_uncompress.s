	.file	"rw_uncompress.c"
	.section	.rodata
	.p2align 3
.LC0:
	.word	.Condition00
	.word	.Condition01
	.word	.Condition10
	.word	.Condition11
	.text
	.p2align 2
	.p2align 4,,15
	.globl deltaUncompress
	.type	deltaUncompress, @function
deltaUncompress:
	// pretend args regs size (bytes) = 0
	// saved LR + FP regs size (bytes) = 0
	// callee saved regs size (bytes) = 0
	// local vars size (bytes) = 32
	// fixed args size (bytes) = 0
	{
		r29 = add(r29,#-32)
		r10 = r2
		r5 = ##.LC0
		
	}
	{
		r12 = r1
	}
	{
		r2 = add(r29,#16)
		r3 = memw(r5++#4)
		r4 = #3
		r11 = add(r12,#4032)
	}
	.falign
.DCZERO:
	{
		p0 = cmp.gtu(r12,r11)
		r12= add(r12, #32)
		dczeroa(r12)
		if(!p0.new) jump:t .DCZERO
	}
	.falign
.SETUP:
	{
		memw(r2++#4) = r3
		r3 = memw(r5++#4)
		r4 = add(r4,#-1)
		r11 = r0
	}
	{
		p0 = cmp.eq(r4, #0); if (!p0.new) jump:t .SETUP
	}
	{
		memw(r2+#0) = r3
		r2 = memw(r11++#4)
		r0 = #1
		r4 = #32
	}
	{
		memw(r29+#12) = #0
		memw(r29+#0) = #0
		r5 = and(r2,#3)
		r3:2 = combine(#0,r2)
	}
	{
		memw(r29+#8) = #0
		memw(r29+#4) = #0
		r6 = add(r29,#16)
	}
	{
		r7 = memw(r6+r5<<#2)
		r5 = #3
	}
	{
		jumpr r7
	}
	.falign
.All0: 
//32 0s magic.
	{
		
		r6 = memw(r11++#4)
		r12 = add(r4,#-32)
		r7 = #0
		r3:2 = lsr(r3:2,#32)
	}
	{
		r3:2 |= asl(r7:6,r12)
		r1 = add(r1,#68)//add 68 because we did not add 4 for the initial 0 from condition 0
		r0 = add(r0,#17)//add 17 because we did not add 1 for initial 0
	}
	{
		r8 = add(r29,#16)
		r9 = and(r2,#3)
	}
	{
		r7 = memw(r8+r9<<#2)
	}
	{
		jumpr r7
	}
	.falign
.AddWord00:
	{
		r7 = #0
		r6 = memw(r11++#4)
	}
	{
		r3:2 |= asl(r7:6,r4)
		r4 = add(r4, #32)
		jump .Continue00
	}
	.falign
.Condition00:
	{
		r4 = add(r4,#-2)
		r3:2 = lsr(r3:2,#2)
		p0 = cmp.gtu(r4,#34)
		if (!p0.new) jump:nt .AddWord00 
	}
	.falign
.Continue00:
	{
		if(r2==#0) jump:nt .All0
		r6 = and(r2,#3)
		r8 = add(r29,#16)
		
	}
	{	
		
		//if(!p0) jump .LJ3 //uncomment for 32 0
		r7 = memw(r8+r6<<#2)
		if(r6==#0) jump:nt .Condition00
		r1 = add(r1,#4)
		r0 = add(r0,#1)
	}
	{
		jumpr r7
	}
	.falign
.AddWord01:
	{
		r6 = memw(r11++#4)
		r7 = #0
		r8 = add(r4, #-4)
	}
	{
		r3:2 |= asl(r7:6,r8)
		r4 = add(r4,#32)
		jump .Continue01
	}
	.falign
.Condition01:
	{
		p0 = cmp.gtu(r4,#36)
		r9 = and(r2,#12)
		r3:2 = lsr(r3:2,#4)
		if (!p0.new) jump:nt .AddWord01
	}
	.falign
.Continue01:
	{
		r7 = and(r2,#3)
		r9 = memw(r29+r9<<#0)
		
		r6 = add(r29,#16)
	}
	{
		memw(r1+#0) = r9
		r7 = memw(r6+r7<<#2)

	}
	{	
		r4 = add(r4, #-4)
		r0 = add(r0,#1)
		r1 = add(r1,#4)
		jumpr r7
	}
	.falign
.Condition10:
	{	
		
		r6 = and(r2,#12)
		r9:8 = lsr(r3:2,#4)
		p0 = cmp.gtu(r4,#46)
	}

	{	

		r3:2 = lsr(r3:2,#14)
		r7 = and(r8,##1023)
		if (!p0) jump .AddWord10
	}
	.falign
.Continue10:
	{
		r9 = memw(r29+r6<<#0)
		r4 = add(r4,#-14)
		r8 = add(r29,#16)
		r13 = and(r2,#3)
	}
	{
		r9 = or(r7,and(r9,##-1024))
		r0 = add(r0,#1)
		r7 = memw(r8+r13<<#2)
	}
	{
		memw(r29+r6<<#0) = r9
		memw(r1+#0) = r9
		r1 = add(r1,#4)
		jumpr r7
	}
	.falign	
.AddWord10:
	{	
		r9 = #0
		r12 = add(r4,#-14)
		r8 = memw(r11++#4)	
	}
	{
		r3:2 |= asl(r9:8,r12)
		r4 = add(r4,#32)
		jump .Continue10
	}
	.falign
.AddWord11:
	{
		r8 = add(r4, #-2)
		r6 = memw(r11++#4)
		r7 = #0
	}
	{
		r3:2 |= asl(r7:6,r8)
		r4 = add(r4,#32)
		r13 = add(r13,#32)
		jump .Continue11
	}
	.falign
.Condition11:
	{
		r13 = add(r4,#-34)
		p0 = cmp.gtu(r4,#34)
		r3:2 = lsr(r3:2,#2)
		if (!p0.new) jump:nt .AddWord11
	}
	.falign
.Continue11:
	{
		r6 = memw(r11++#4)
		r7 = #0
		r9 = r2
		r3:2 = lsr(r3:2,#32)
	}
	{
		r3:2 |= asl(r7:6,r13)
		r5 = add(r5,#1)
	}
	{
		r5 = and(r5,#3)
		r6 = and(r2,#3)
		memw(r1+#0) = r9
		r8 = add(r29,#16)
	}
	{
		p0 = cmp.gtu(r10,r0)
		r7 = memw(r8+r6<<#2)
		memw(r29+r5<<#2) = r9
		if (!p0.new) jump:nt .Exit
	}
	{	
		r4 = add(r4,#-2)
		r0 = add(r0,#1)
		r1 = add(r1,#4)
		jumpr r7
	}
	.falign
.Exit:
	{
		r29 = add(r29,#32)
		r0 = asl(r0,#2)
		jumpr r31
	}

	.size	deltaUncompress, .-deltaUncompress
	.ident	"GCC: (Sourcery CodeBench Lite 5.1 2012.03-129) 4.6.1"
	.section	.note.GNU-stack,"",@progbits
