#ifndef _QRLBN_UTILS_H
#define _QRLBN_UTILS_H

/*

  Copyright © 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  QUALCOMM Confidential and Proprietary
  
*/


#include <stdint.h>

int micromalloc(uint32_t *state, int size);
void microfree(uint32_t *state, size_t size, int mem_item);

#define MICROBAD  0x456789a
#define MICROMALLOC() micromalloc(&mmstate, NUM_TEMPS)
#define MICROFREE(m) microfree(&mmstate, NUM_TEMPS, m); m = MICROBAD
//#define MICROFREE(m) microfree(&mmstate, NUM_TEMPS, m)

int find_MSB(const uint32_t *a, size_t num_words);
void qrlbn_zeroize(volatile void* tgt, size_t len);
void qrlbn_zeroize_stack(void);



#endif
