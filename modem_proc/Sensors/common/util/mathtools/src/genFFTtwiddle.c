
/*============================================================================
Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.  All Rights Reserved.
QUALCOMM Proprietary and Confidential
============================================================================*/

/****************************************************/
/*   Generate interleave & bit-reverse order        */
/*   twiddle factor array for radix-4 fixed-point   */
/*   implementation of FFT                          */
/****************************************************/

#ifndef __linux__

#include <stdio.h>
#include <math.h>
#include "fft.h"

#define   PI  (acos(-1.0))


int brev( int x, int BITS )
{  int i;
   int y = 0;
   for ( i= 0; i<BITS; i++)
    {
      y = (y << 1)| (x & 1);
      x >>= 1;
    }
   return y;
}



void gentwiddle( cfloat *twiddle, int NP, int log2NP )
{
  int i, k, k1, k2;
  cfloat x[NP/2];
  cfloat y[NP/4];

  float ar, ai;

 /************************************/
 /*    Generate twiddles             */
 /*    arrange in bit reversed order */
 /************************************/

    for(i=0; i< NP/2; i++) {
        k = brev(i, log2NP-1); 
        ar =  cos((double)(k)*2.0*PI/(double)NP);
        ai = -sin((double)(k)*2.0*PI/(double)NP); 
        x[i] = ar + 1i*ai;
    }

    for(i=0; i< NP/4; i++) {
        k1 = brev((2*i), log2NP-1); 
        k2 = brev(i, log2NP-1); 
        k = k1 + k2;
        ar =  cos((double)(k)*2.0*PI/(double)NP);
        ai = -sin((double)(k)*2.0*PI/(double)NP); 
        y[i] = ar + 1i*ai;
     }


     for(i=0; i<NP/4; i++) {

       twiddle[3*i+0] = x[i];
       twiddle[3*i+1] = x[2*i];
       twiddle[3*i+2] = y[i];
     }
}

#endif

