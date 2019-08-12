	.text
.Ltext0:
	.section	.rodata
	.p2align 5
.LC0:
	.word	.L1
	.word	.L2
	.word	.L3
	.word	.L4
	.word	.L5
	.word	.L6
	.word	.L7
	.word	.L8
	.word	.L1
	.word	.L2
	.word	.L9
	.word	.L4
	.word	.L5
	.word	.L10
	.word	.L7
	.word	.L8
	.text
	.p2align 2
	.p2align 4,,15
	.globl q6zip_uncompress
	.type	q6zip_uncompress, @function
q6zip_uncompress:
.LFB3:
	//.file 1 "q6zip_uncompress.c"
	// pretend args regs size (bytes) = 0
	// saved LR + FP regs size (bytes) = 8
	// callee saved regs size (bytes) = 24
	// local vars size (bytes) = 64
	// fixed args size (bytes) = 0
	{
		allocframe(#88)
		call __save_r16_through_r21
	}
	{
		r20 = r4
		r17:16 = combine(r0,r2)
		memw(##input) = r2
	}
	{
		r21 = add(r4,#4096)
                dcfetch(r16)
	}
	{
		r0 = and(r16,#31)
		memw(##q6zip_out_buf_size) = r1
	}
	{
		r1 = add(r3,add(r0,#31))
		r2 = sub(r16,r0)
                r0=##.LC0
	}
	{
                dcfetch(r0)
		r1 = lsr(r1,#5)
		r0 = #32
	}
	{
		r0=combine(r0.l,r1.l)
	}
	{
		r1:0=combine(#32,r0)
		r10 = ##.LC0
		loop0(.L12,#128)
	}
	{
		l2fetch(r2,r1:0)
		r0 = r17
	}
	.falign
.L12:
	{
	        dczeroa(r0)
		r0 = add(r0,#32)
	}:endloop0
	{
		r18 = memw(r16++#4)
		r19 = #0
	}
	{
		r3 = extractu(r18,#4,#0)
	}
	{
		r2 = r18
		r3 = memw(r10+r3<<#2)
	}
	{
		r0 = r17
		r2 = #32
		r1 = #-1
		jumpr r3
	}
	.falign
.L27:
	{
		r4 = memw(r0+r1<<#2)
    		memw(r0++#4) = r4.new
        	jumpr r3
	}
	.falign
.L1:
	{
		r4 = r18
		r3 = add(r2,#-20)
		r1=tableidxb(r18,#9,#3)
		r6 = extractu(r18,#4,#20)
	}
	{
		r19:18 = lsr(r19:18,#20)
		r5 = memw(r0+r1<<#2)
		r7 = extractu(r4,#8,#12)
		r6 = memw(r10+r6<<#2)
	}
	{
		p0 = cmp.gt(r3,#31)
		r5 = insert(r7,#8,#0)
		memw(r0++#4) = r5.new
		if (!p0.new) jump:t .L75
	}
	{
		r2 = r3
	        jumpr r6
	}
	.falign
.L2:
	{
		r6 = add(r2,#-12)
		r4 = r18
		r19:18 = lsr(r19:18,#12)
	}
	{
		p0 = cmp.gt(r6,#31)
		r3 = extractu(r4,#4,#12)
		r1=tableidxb(r4,#9,#3)
		if (p0.new) r2 = r6
	}
	{
		if (!p0) r2 = add(r2,#20)
		if (p0) jump .L27
                r3 = memw(r10+r3<<#2)
	}
	{
		r4 = memw(r16++#4)
		r5 = #0
                hintjr(r3)
	}
	{
		r4 = memw(r0+r1<<#2)
		r19:18 |= asl(r5:4,r6)
		dcfetch(r16+#32) 
	}
	{
		memw(r0++#4) = r4
		jumpr r3
	}
	.falign
.L3:
	{
		r4 = r18
		r3 = add(r2,#-25)
		r19:18 = lsr(r19:18,#25)
		r1=tableidxb(r18,#9,#4)
	}
	{
		r5 = extractu(r4,#12,#13)
		p0 = cmp.gt(r3,#31)
		r4 = memw(r0+r1<<#2)
		if (!p0.new) jump:t .L77
	}
	{
		r4 = insert(r5,#12,#0)
		memw(r0++#4) = r4.new
		r5 = #0
		r6 = extractu(r18,#4,#0)
	}
	{
		r2 = r3
		p0 = cmp.eq(r6,#1)
		r3 = memw(r10+r6<<#2)
		if (p0.new) jump:t .L2
	}
  	jumpr r3


	.falign
.L48:
	{
		r2 = r3
		r3 = extractu(r18,#4,#0)
	}
	{
		p0 = cmp.eq(r3,#1)
		r3 = memw(r10+r3<<#2)
		if (p0.new) jump:t .L2
	}
  	jumpr r3
	.falign
.L37:
	{
		r3 = extractu(r18,#4,#0)
	}
	{
		p0 = cmp.eq(r3,#1)
		r3 = memw(r10+r3<<#2)
		if (p0.new) jump:t .L2
	}
  	jumpr r3

	.falign
.L6:
	{
		r3 = extractu(r18,#12,#4)
		r6 = add(r2,#-16)
		r19:18 = lsr(r19:18,#16)
	}
	{
		r3 = memw(r21+r3<<#2)
		p0 = cmp.gt(r6,#31)
		if (!p0.new) jump:nt .L78
          	r7 = extractu(r18,#4,#0)

	}
	{
		memw(r0++#4) = r3
		r3 = memw(r10+r7<<#2)
      		r2 = r6
	}
	jumpr r3
	.falign
.L39:
	{
		r2 = extractu(r18,#4,#0)
	}
	{
		r3 = memw(r10+r2<<#2)
      		r2 = r6
	}
	jumpr r3
	.falign
.L5:
	{
		r3 = r18
		r6 = add(r2,#-13)
		r19:18 = lsr(r19:18,#13)
	}
	{
		r4 = extractu(r3,#4,#13)
		p0 = cmp.gt(r6,#31)
		r7 = extractu(r3,#10,#3)
	}
	{
		r3 = memw(r10+r4<<#2)
		if (!p0) r4 = memw(r16++#4)
		r5 = #0
		if (!p0) jump .L79
	}
	{
		r7 = memw(r20+r7<<#2)
		memw(r0++#4) = r7.new
		r2 = r6
		jumpr r3
	}
	.falign
.L8:
	{
		r6 = add(r2,#-3)
		r3 = extractu(r18,#4,#3)
		r19:18 = lsr(r19:18,#3)
		r5 = #0
	}
	{
		p0 = cmp.gt(r6,#31)
		if (!p0.new) r4 = memw(r16++#4)
		if (!p0.new) jump:nt .L80
		r3 = memw(r10+r3<<#2)
	}
	{
		r4 = memw(r0+r1<<#2)
		r2 = r6
		memw(r0++#4) = r4.new
		jumpr r3
	}
	.falign
.L4:
	{
		r7:6 = lsr(r19:18,#3)
		r8 = add(r2,#-3)
		r3 = #0
                p0 = cmp.gt(r2, #34);
	}
	{
		if (!p0) jump .L81
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
		r2 = extractu(r18,#4,#0)
	}
	{
		p0 = cmp.eq(r2,#1)
		r3 = memw(r10+r2<<#2)
		r2 = r8
		if (p0.new) jump:t .L2
	}
	{
		jumpr r3
	}
	.falign
.L9:
	{
		r4 = extractu(r18,#2,#4)
		r3 = extractu(r18,#8,#15)
		if (cmp.eq(r4.new,#3)) jump:t .L20
	}
	{
		p0 = cmp.eq(r4,#2)
		r3 = extractu(r18,#8,#15)
		r6 = add(r2,#-23)
		if (p0.new) jump:nt .L21
	}
	{
		p0 = cmp.eq(r4,#0)
		r4 = extractu(r18,#16,#6)
		if (p0.new) r3 = memw(r0+r1<<#2)
		if (p0.new) jump:nt .L22
	}
	{
		p0 = !tstbit(r18,#6)
		if (!p0.new) r3 = memw(r0+r1<<#2)
		r6 = add(r2,#-15)
		if (p0.new) jump:nt .L82
	}
	{
		r4 = extractu(r18,#8,#7)
		r19:18 = lsr(r19:18,#15)
		p0 = cmp.gt(r6,#31)
		r5 = #0
	}
	{
		r3 = insert(r4,#8,#8)
		memw(r0++#4) = r3.new
		if (p0) jump .L39
		if (!p0) r4 = memw(r16++#4)
		
	}
	{
		r19:18 |= asl(r5:4,r6)
		dcfetch(r16+#32) 
		r2 = add(r2,#17)
                r6 = extractu(r18,#4,#0)
	}
	{
		r3 = memw(r10+r6<<#2)
	}
	jumpr r3

	.falign
.L10:
	{
		r3 = and(r18,#16)
		r4 = extractu(r18,#12,#5)
		if (cmp.eq(r3.new,#0)) jump:t .L83
	}
	{
		r3 = memw(r0+r1<<#2)
		r6 = add(r2,#-17)
		r19:18 = lsr(r19:18,#17)
	}
	{
		r3 = insert(r4,#12,#0)
		p0 = cmp.gt(r6,#31)
		memw(r0++#4) = r3.new
		if (p0.new) jump:t .L39
	}
	{
		r2 = add(r2,#15)
		r4 = memw(r16++#4)
		r5 = #0
		r3 = extractu(r18,#4,#0)
	}
	{
		r19:18 |= asl(r5:4,r6)
		dcfetch(r16+#32) 
		r3 = memw(r10+r3<<#2)
	}
	jumpr r3
	.falign
.L7:
	{
		r6 = add(r2,#-11)
		r3 = extractu(r18,#4,#11)
		r5 = extractu(r18,#8,#3)
	}
	{
		r19:18 = lsr(r19:18,#11)
		r4 = memw(r0+r1<<#2)
		p0 = cmp.gt(r6,#31)
		r3 = memw(r10+r3<<#2)
	}
	{
		r4 = insert(r5,#8,#0)
		memw(r0++#4) = r4.new
		if (!p0) jump .L84
	}
	{   
		r2 = r6
		jumpr r3
	}
	.falign
.L84:
	{
		r5 = #0
		r4 = memw(r16++#4)
                hintjr(r3)
	}
	{
		r19:18 |= asl(r5:4,r6)
		dcfetch(r16+#32) 
		r2 = add(r2,#21)
		jumpr r3
	}
	.falign
.L83:
	{
		r4 = r18
		r3 = add(r2,#-30)
		r19:18 = lsr(r19:18,#30)
	}
	{
		r1=tableidxb(r4,#9,#5)
		r5 = extractu(r4,#16,#14)
		p0 = cmp.gt(r3,#31)
	}
	{
		r4 = memw(r0+r1<<#2)
	}
	{
		r4 = insert(r5,#16,#0)
		memw(r0++#4) = r4.new
		r5 = #0
		if (p0) jump .L48
	}
	{
		r4 = memw(r16++#4)
		r2 = add(r2,#2)
	}
	{
		r19:18 |= asl(r5:4,r3)
		dcfetch(r16+#32) 
	}
       	{
		r3 = extractu(r18,#4,#0)
	}
	{
		p0 = cmp.eq(r3,#1)
		r3 = memw(r10+r3<<#2)
		if (p0.new) jump:t .L2
	}
  	jumpr r3

	.falign
.L22:
	{
		r6 = add(r2,#-22)
		r19:18 = lsr(r19:18,#22)
		r3 = insert(r4,#16,#0)
		r5 = #0
	}
	{
		p0 = cmp.gt(r6,#31)
		memw(r0++#4) = r3
		if (p0.new) jump:t .L39
		if (!p0.new) r4 = memw(r16++#4)
	}
	{
		r19:18 |= asl(r5:4,r6)
		dcfetch(r16+#32) 
		r6 = add(r2,#10)
	}
	{
		r2 = extractu(r18,#4,#0)
	}
	{
		r3 = memw(r10+r2<<#2)
      		r2 = r6
	}
	jumpr r3
	.falign
.L20:
	{
		r1=tableidxb(r18,#9,#6)
		r6 = add(r2,#-23)
		r19:18 = lsr(r19:18,#23)
	}
	{
		r4 = memw(r0+r1<<#2)
		p0 = cmp.gt(r6,#31)
	}
	{
		r4 = insert(r3,#8,#8)
	}
	.falign
.L69:
	{
		memw(r0++#4) = r4
		if (p0) r2 = r6
		r5 = #0
		if (p0) jump .L37
	}
	{
		r4 = memw(r16++#4)
		r2 = add(r2,#9)
	}
	{
		r19:18 |= asl(r5:4,r6)
		dcfetch(r16+#32) 
	}
	{
		r3 = extractu(r18,#4,#0)
	}
	{
		p0 = cmp.eq(r3,#1)
		r3 = memw(r10+r3<<#2)
		if (p0.new) jump:t .L2
	}
  	jumpr r3
	.falign
.L21:
	{
		r1=tableidxb(r18,#9,#6)
		p0 = cmp.gt(r6,#31)
		r19:18 = lsr(r19:18,#23)
	}
	{
		r4 = memw(r0+r1<<#2)
	}
	{
		r4 = insert(r3,#8,#16)
		jump .L69
	}
	.falign
.L82:
	{
		r3 = sub(r0,r17)
	}
	{
		p0 = cmp.gt(r3,##4095)
		r4 = extractu(r18,#8,#7)
		if (p0.new) jump:nt .L86
	}
	{
		r3 = memw(r0+r1<<#2)
		r6 = add(r2,#-15)
		r19:18 = lsr(r19:18,#15)
	}
	{
		r3 = insert(r4,#8,#16)
	}
	{
		p0 = cmp.gt(r6,#31)
		memw(r0++#4) = r3
		r5 = #0
		if (p0.new) jump:t .L39
	}
	{
		r4 = memw(r16++#4)
	}
	{
		r19:18 |= asl(r5:4,r6)
		dcfetch(r16+#32) 
		r6 = add(r2,#17)
	}
	{
		r2 = extractu(r18,#4,#0)
	}
	{
		r3 = memw(r10+r2<<#2)
      		r2 = r6
	}
	jumpr r3
	.falign
.L81:
	{
                r5 = #0
		r4 = memw(r16++#4)
	}
	{
		r7:6 |= asl(r5:4,r8)
		dcfetch(r16+#32) 
		r8 = add(r2,#29)
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
		r2 = extractu(r18,#4,#0)
	}
	{
		p0 = cmp.eq(r2,#1)
		r3 = memw(r10+r2<<#2)
		r2 = r8
		if (p0.new) jump:t .L2
	}
	{
		jumpr r3
	}
	.falign
.L86:
	{
		r0 = #0
		r1 = memw(##q6zip_out_buf_size)
		r3 = and(r3,#-4)
	}
	{
		memw(r1+#0) = r3
		jump __restore_r16_through_r21_and_deallocframe
	}
	.falign
.L80:
	{
		r19:18 |= asl(r5:4,r6)
		hintjr(r3)
		r4 = memw(r0+r1<<#2)
	}
	{
		r2 = add(r2,#29)
		memw(r0++#4) = r4
		jumpr r3
	}

	.falign
.L79:
	{
		r19:18 |= asl(r5:4,r6)
		dcfetch(r16+#32) 
                hintjr(r3)
		r6 = add(r2,#19)
	}
	{
		r7 = memw(r20+r7<<#2)
		memw(r0++#4) = r7.new
		r2 = r6
		jumpr r3
	}
	.falign
.L78:
	{
		memw(r0++#4) = r3
		r4 = memw(r16++#4)
		r5 = #0
	}
	{
		r19:18 |= asl(r5:4,r6)
		dcfetch(r16+#32) 
		r2 = add(r2,#16)
		r6 = memw(r10+r7<<#2)
	}
	jumpr r6
	.falign
.L77:
	{
		r6 = extractu(r18,#4,#0)
		r4 = insert(r5,#12,#0)
		memw(r0++#4) = r4.new
	}
	{
		r5 = #0
		r4 = memw(r16++#4)
		r2 = add(r2,#7)
		r6 = memw(r10+r6<<#2)
	}
	{
		r19:18 |= asl(r5:4,r3)
      	        jumpr r6
        }
.L75:
	{
		r4 = memw(r16++#4)
		r5 = #0
		r2 = add(r2,#12)
                hintjr(r6)
	}
	{
		r19:18 |= asl(r5:4,r3)
		dcfetch(r16+#32) 
	        jumpr r6
	}
	.size	q6zip_uncompress, .-q6zip_uncompress
	.section	.bss,"aw",@nobits
	.lcomm	input,4,4
	.lcomm	q6zip_out_buf_size,4,4

