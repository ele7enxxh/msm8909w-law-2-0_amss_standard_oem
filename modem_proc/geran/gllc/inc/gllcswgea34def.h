#ifndef GLLCSWGEA34DEF_H
#define GLLCSWGEA34DEF_H


/* DA changes
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

 
typedef enum {
   FALSE,
   TRUE
} boolean;
 
*/  
 
 
#include <hexagon_protos.h>
#include <string.h>
#include <comdef.h>

/*! @brief QDSP6 32-bit vector type.
*/
typedef union {
   int8    b[4];
   uint8  ub[4];
   int16   h[2];
   uint16 uh[2];
   int32   w;
   uint32 uw;
  /* Aliases for halfwords */

/*   struct {
      int16 I;
      int16 Q;
   };
   struct {
      int16 re;
      int16 im;
   };
   struct {
      int16 real;
      int16 imag;
   };
   struct {
      int16 Real;
      int16 Imag;
   };                */

} vect32;

/*! @brief QDSP6 64-bit vector type.
*/
typedef union {
   int8    b[8];
   uint8  ub[8];
   int16   h[4];
   uint16 uh[4];
   int32   w[2];
   uint32 uw[2];
   int64   d;
   vect32  v[2]; /* Access each half as a 32-bit vector */
///* Aliases for words */

/*
   struct {
      int32 I;
      int32 Q;
   };
   struct {
      int32 re;
      int32 im;
   };
   struct {
      int32 real;
      int32 imag;
   };
   struct {
      int32 Real;
      int32 Imag;
   };                   */


} vect64;



#endif /* GLLCSWGEA34DEF_H */ 
