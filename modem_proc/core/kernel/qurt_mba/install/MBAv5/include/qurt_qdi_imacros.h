#ifndef QURT_QDI_IMACROS_H
#define QURT_QDI_IMACROS_H

/*======================================================================
                        qurt_qdi_imacros.h

GENERAL DESCRIPTION
  Internal macros used for QDI.  Mostly consists of tricky (and ugly)
  preprocessor hacks which permit us to do varargs function invocations
  where we pass optional arguments in registers and where we can do
  type casting and checking automatically.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2012
             by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_qdi_imacros.h#1 $
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2012-02-29 	Jack			Add Header file
 ======================================================================*/

#define _QDM1ST(a,...) a
#define _QDMCAR(x) _QDM1ST x
#define _QDMCDR(x,...) (__VA_ARGS__)
#define _QDMCAT(a,b) a##b
#define _QDMDEC(x) _QDMCAT(_QDMP,x)
#define _QDMTCAT(a,b) _QDMCAT(a,b)
#define _QDMCNTH(a,b,c,d,e,f,g,h,i,j,k,l,cnt,...) cnt
#define _QDMCNT(args...) _QDMCNTH(args,10,9,8,7,6,5,4,3,2,1,0)
#define _QDMP1 0
#define _QDMP2 1
#define _QDMP3 2
#define _QDMP4 3
#define _QDMP5 4
#define _QDMP6 5
#define _QDMP7 6
#define _QDMP8 7
#define _QDMP9 8
#define _QDMP10 9
#define _QDMT_QDMV void
#define _QDMT_QDMA
#define _QDME_QDMV
#define _QDME_QDMA
#define _QDME_QDMAI(a) (a)
#define _QDME_QDMCI(a) (int)(a)
#define _QDME_QDMVP(a) (a)
#define _QDME_QDMCP(a) (a)
#define _QDMT_QDMAI(a) int
#define _QDMT_QDMCI(a) int
#define _QDMT_QDMVP(a) void*
#define _QDMT_QDMCP(a) const char*
#define _QDMR0(a,b,c,d,r) _QDMTCAT(d,r)
#define _QDMR1(a,b,c,d,r) _QDMTCAT(d,_QDMCAR(b))(_QDMCAR(c))_QDMTCAT(_QDMR,_QDMDEC(a))(_QDMDEC(a),_QDMCDR b,_QDMCDR c,d,_QDMA)
#define _QDMR2(a,b,c,d,r) _QDMTCAT(d,_QDMCAR(b))(_QDMCAR(c)),_QDMTCAT(_QDMR,_QDMDEC(a))(_QDMDEC(a),_QDMCDR b,_QDMCDR c,d,_QDMA)
#define _QDMR3(a,b,c,d,r) _QDMTCAT(d,_QDMCAR(b))(_QDMCAR(c)),_QDMTCAT(_QDMR,_QDMDEC(a))(_QDMDEC(a),_QDMCDR b,_QDMCDR c,d,_QDMA)
#define _QDMR4(a,b,c,d,r) _QDMTCAT(d,_QDMCAR(b))(_QDMCAR(c)),_QDMTCAT(_QDMR,_QDMDEC(a))(_QDMDEC(a),_QDMCDR b,_QDMCDR c,d,_QDMA)
#define _QDMR5(a,b,c,d,r) _QDMTCAT(d,_QDMCAR(b))(_QDMCAR(c)),_QDMTCAT(_QDMR,_QDMDEC(a))(_QDMDEC(a),_QDMCDR b,_QDMCDR c,d,_QDMA)
#define _QDMR6(a,b,c,d,r) _QDMTCAT(d,_QDMCAR(b))(_QDMCAR(c)),_QDMTCAT(_QDMR,_QDMDEC(a))(_QDMDEC(a),_QDMCDR b,_QDMCDR c,d,_QDMA)
#define _QDMR7(a,b,c,d,r) _QDMTCAT(d,_QDMCAR(b))(_QDMCAR(c)),_QDMTCAT(_QDMR,_QDMDEC(a))(_QDMDEC(a),_QDMCDR b,_QDMCDR c,d,_QDMA)
#define _QDMR8(a,b,c,d,r) _QDMTCAT(d,_QDMCAR(b))(_QDMCAR(c)),_QDMTCAT(_QDMR,_QDMDEC(a))(_QDMDEC(a),_QDMCDR b,_QDMCDR c,d,_QDMA)
#define _QDMR9(a,b,c,d,r) _QDMTCAT(d,_QDMCAR(b))(_QDMCAR(c)),_QDMTCAT(_QDMR,_QDMDEC(a))(_QDMDEC(a),_QDMCDR b,_QDMCDR c,d,_QDMA)
#define _QDMR10(a,b,c,d,r) _QDMTCAT(d,_QDMCAR(b))(_QDMCAR(c)),_QDMTCAT(_QDMR,_QDMDEC(a))(_QDMDEC(a),_QDMCDR b,_QDMCDR c,d,_QDMA)
#define _QDMTA _QDMVP,_QDMAI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI
#define _QDMTN _QDMVP,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI
#define _QDMUA _QDMAI,_QDMAI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI
#define _QDMUO _QDMCP,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI,_QDMCI
#define _QDMLL(a,b,...) (_QDMTCAT(_QDMR,_QDMCNT(a,b,##__VA_ARGS__))(_QDMCNT(a,b,##__VA_ARGS__),a,(__VA_ARGS__),b,_QDMV))

#endif
