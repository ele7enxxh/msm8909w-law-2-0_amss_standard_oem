#ifndef _MONTMUL_STD_H
#define _MONTMUL_STD_H
/*

  Copyright © 2010 Qualcomm Technologies Inc. 
  All Rights Reserved.
  QUALCOMM Confidential and Proprietary
  
*/
//#define DEBUG_MONTMUL_STD
#define  restrict

#ifdef _MSC_VER
#define  restrict 
#endif

#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif

int montmul_std(const uint32_t *a, const uint32_t *b, const uint32_t *m, 
		size_t N, uint32_t m0_1, uint32_t * restrict z);

int mult_R_mod_m(const uint32_t *a, const uint32_t *m, uint32_t *z, size_t N);

int montmul_one_std(const uint32_t *a, const uint32_t *m, size_t N, 
		    uint32_t m0_1, uint32_t * restrict z);	

#ifdef DEBUG_MONTMUL_STD
#define DEBUG_PRINT printf
#else 
#define DEBUG_PRINT(...) 
#endif //DEBUG_MONTMUL_STD
#ifdef __cpluspuls
}
#endif

#endif
