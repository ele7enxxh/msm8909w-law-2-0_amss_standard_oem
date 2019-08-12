/*==================================================================
 *
 * FILE:        deviceprogrammer_utils.h
 *
 * DESCRIPTION:
 *   
 *
 *        Copyright © 2008-2013 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/firehose/deviceprogrammer_utils.h#1 $
 *   $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2013-06-03   ah      Added legal header
 * 2013-05-31   ab      Initial checkin
 */

#ifndef DEVICEPROGRAMMER_UTILS_H
#define DEVICEPROGRAMMER_UTILS_H

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "comdef.h"

typedef int sSIZE_T;
typedef unsigned long long int SIZE_T;
#define SIZE_T_FORMAT "%llu"
#define SIZE_T_CONVERTER strtoull

/*=========================================================================== 
  The clock that is used to track time passing.
===========================================================================*/
//#define TIMETICK_CLK  MPM_SLEEP_TIMETICK_COUNT_VAL 
#define TIMETICK_CLK  MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL

/*=========================================================================== 
  Clock frequency is 32.768 KHz
  The number of microseconds per cycle is:
  1 / (32.768 KHz) = 30.52ms
  The int part is 30
===========================================================================*/
#define TIMETICK_CLK_SPEED_INT       30

/*=========================================================================== 
  Clock frequency is 32.768 KHz
  The number of microseconds per cycle is:
  1 / (32.768 KHz) = 30.52ms
  we round 0.52 to 1/2.
  Timestamp is calculated as : count*int + count/2
  Floating point arithmetic should not be used to avoid error and speed penalty
===========================================================================*/
#define CALCULATE_TIMESTAMP(COUNT) \
  ((COUNT)*TIMETICK_CLK_SPEED_INT + (COUNT)/2)

/*
  we are using 32 bits timestamp, the max counter value is:
  0xFFFFFFFF/31 = 138547732
  after this value the timestamp will overflow
*/
#define LOG_MAX_COUNT_VAL  138547732

/**
* return value when timestamp overflow happens
*/
#define LOG_TIMESTAMP_OVERFLOW  0xFFFFFFFF

#define MICROSECONDS_PER_SECOND 1000000

long get_time(void);
long get_rollover_time(void);
void delaySeconds(unsigned int seconds);

boolean simpleStrReplace(char* haystack, const char* needle, const char* thorn);
boolean stringReverse(char* astring);
boolean integerToString(int number, char* buffer, SIZE_T length);
SIZE_T stringToNumber(const char* buffer, boolean *retval);
boolean parseExpression(char* buffer, SIZE_T* result);
uint32 calc_crc32(const byte *data_in, const uint32  nbytes_in);
word calc_crc16_wrapper(byte *data_buf,
                        SIZE_T bytes,
                        word crc_value);

#endif /* BOOT_UTIL_H */

