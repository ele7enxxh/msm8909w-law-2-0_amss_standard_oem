#ifndef _MONTMUL_UTILS_H
#define _MONTMUL_UTILS_H

/*

  Copyright © 2013 Qualcomm Technologies Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/


#ifdef _MSC_VER
  #include "visualc_stdint.h"
#define restrict  
#else
  #include <stdint.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif


uint32_t ecc_inverse32(uint32_t x);

// uint64_t inverse64(uint64_t x);

#ifdef	__cplusplus
}
#endif

#endif

