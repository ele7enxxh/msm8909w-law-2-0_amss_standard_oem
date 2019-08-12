#ifndef _QRLBN_GENERAL_H
#define _QRLBN_GENERAL_H

/*

  Copyright © 2014 Qualcomm Technologies Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/

#define QRLBN_DATA_STRUCTURE_VERSION 1

// The flags arguments for algorithm-level and init functions.
#define QRLBN_NON_SENSITIVE 1
#define QRLBN_PUBLIC_EXPONENT 2

// flags for modulus data structures and init functions
#define QRLBN_NO_MONT 4  // indicates that init functions should not compute the montgomomery values or that the montgomery values are not present

#endif
