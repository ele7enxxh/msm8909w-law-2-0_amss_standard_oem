/******************************** FILE HEADER **********************************
*                                                               
* Purpose:                                                      
*       Routines for converting integers.
*                                                               
*******************************************************************************/
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/intconv.c#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
04/13/09   sri     Replaced the func Power() by a macro POWER_OF_2.
05/10/05   sst     Integer conmversion functions.
===========================================================================*/

#include "intconv.h"
#include "msg.h"
#include "assert.h"

/* The value of y needs to be verified for "< 64" before using POWER_OF_2 
 * But this macro is used in functions that return 32bit values only
 * So it is instead checked for "< 32"
 */
#define POWER_OF_2(y) (1<<(y))

static int32 GetBits( uint64 i, uint8 bits )
{
  uint64 result;
  
  ASSERT(bits<32);
  result = (uint64)(i & (POWER_OF_2(bits)-1));

  // Sign extend the result if it should be negative.
  //
  if ((result >= POWER_OF_2(bits-1)) && ((int64)result > 0)) {
    result = (uint64)(-((int64)result));
  }
  return (int32)result;
}

static uint32 GetUbits( uint64 i, uint8 bits )
{
  ASSERT(bits<32);
  return (uint32)(i & (POWER_OF_2(bits)-1));
}

int32 Int8ToBit( int8 i, uint8 bits, char *file, uint32 line )
{
  const int32 result = GetBits( (uint64)((uint8)i), bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %i (%s, %u)\n", i, result, file, line);
  }
  return result;
}

int32 Int16ToBit( int16 i, uint8 bits, char *file, uint32 line )
{
  const int32 result = GetBits( (uint64)((uint16)i), bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %i (%s, %u)\n", i, result, file, line);
  }
  return result;
}

int32 Int32ToBit( int32 i, uint8 bits, char *file, uint32 line )
{
  const int32 result = GetBits( (uint64)((uint32)i), bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %i (%s, %u)\n", i, result, file, line);
  }
  return result;
}

int32 Int64ToBit( int64 i, uint8 bits, char *file, uint32 line )
{
  const int32 result = GetBits( (uint64)i, bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %lli to %i (%s, %u)\n", i, result, file, line);
  }
  return result;
}

int8 Int16ToInt8( int16 i, char *file, uint32 line )
{
  if ( (int8)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %i (%s, %u)\n", i, (int8)i, file, line);
  }
  return (int8)i;
}

int8 Int32ToInt8( int32 i, char *file, uint32 line )
{
  if ( (int8)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %i (%s, %u)\n", i, (int8)i, file, line);
  }
  return (int8)i;
}

int8 Int64ToInt8( int64 i, char *file, uint32 line )
{
  if ( (int8)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %lli to %i (%s, %u)\n", i, (int8)i, file, line);
  }
  return (int8)i;
}

int16 Int32ToInt16( int32 i, char *file, uint32 line )
{
  if ( (int16)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %i (%s, %u)\n", i, (int16)i, file, line);
  }
  return (int16)i;
}

int16 Int64ToInt16( int64 i, char *file, uint32 line )
{
  if ( (int16)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %lli to %i (%s, %u)\n", i, (int16)i, file, line);
  }
  return (int16)i;
}

int32 Int64ToInt32( int64 i, char *file, uint32 line )
{
  if ( (int32)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %lli to %i (%s, %u)\n", i, (int32)i, file, line);
  }
  return (int32)i;
}

int32 Uint8ToBit( uint8 i, uint8 bits, char *file, uint32 line )
{
  const int32 result = GetBits( (uint64)i, bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %i (%s, %u)\n", i, result, file, line);
  }
  return result;
}

int32 Uint16ToBit( uint16 i, uint8 bits, char *file, uint32 line )
{
  const int32 result = GetBits( (uint64)i, bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %i (%s, %u)\n", i, result, file, line);
  }
  return result;
}

int32 Uint32ToBit( uint32 i, uint8 bits, char *file, uint32 line )
{
  const int32 result = GetBits( (uint64)i, bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %i (%s, %u)\n", i, result, file, line);
  }
  return result;
}

int32 Uint64ToBit( uint64 i, uint8 bits, char *file, uint32 line )
{
  const int32 result = GetBits( (uint64)i, bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %llu to %i (%s, %u)\n", i, result, file, line);
  }
  return result;
}

int8 Uint8ToInt8( uint8 i, char *file, uint32 line )
{
  /* if ( ((int8)i) < 0 ) - modified to suppress lint errors */
  if ( i > 127 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %i (%s, %u)\n", i, (int8)i, file, line);
  }
  return (int8)i;
}

int8 Uint16ToInt8( uint16 i, char *file, uint32 line )
{
  if ( (int8)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %i (%s, %u)\n", i, (int8)i, file, line);
  }
  return (int8)i;
}

int8 Uint32ToInt8( uint32 i, char *file, uint32 line )
{
  if ( (int8)i != i ) { /*lint !e737 */
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %i (%s, %u)\n", i, (int8)i, file, line);
  }
  return (int8)i;
}

int8 Uint64ToInt8( uint64 i, char *file, uint32 line )
{
  if ( (int8)i != i ) { /*lint !e737 */
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %llu to %i (%s, %u)\n", i, (int8)i, file, line);
  }
  return (int8)i;
}

int16 Uint16ToInt16( uint16 i, char *file, uint32 line )
{
  /* if ( ((int16)i) < 0 ) - modified to suppress lint errors */
  if ( i > 32767 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %i (%s, %u)\n", i, (int16)i, file, line);
  }
  return (int16)i;
}

int16 Uint32ToInt16( uint32 i, char *file, uint32 line )
{
  if ( (int16)i != i ) { /*lint !e737 */
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %i (%s, %u)\n", i, (int16)i, file, line);
  }
  return (int16)i;
}

int16 Uint64ToInt16( uint64 i, char *file, uint32 line )
{
  if ( (int16)i != i ) { /*lint !e737 */
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %llu to %i (%s, %u)\n", i, (int16)i, file, line);
  }
  return (int16)i;
}

int32 Uint32ToInt32( uint32 i, char *file, uint32 line )
{
  if ( (int32)i < 0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %i (%s, %u)\n", i, (int32)i, file, line);
  }
  return (int32)i;
}

int32 Uint64ToInt32( uint64 i, char *file, uint32 line )
{
  if ( (int32)i != i ) { /*lint !e737 */
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %llu to %i (%s, %u)\n", i, (int32)i, file, line);
  }
  return (int32)i;
}

int64 Uint64ToInt64( uint64 i, char *file, uint32 line )
{
  if ( (int64)i < 0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %llu to %lli (%s, %u)\n", i, (int64)i, file, line);
  }
  return (int64)i;
}

uint32 Int8ToUbit( int8 i, uint8 bits, char *file, uint32 line )
{
  const uint32 result = GetUbits( (uint64)((uint8)i), bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %u (%s, %u)\n", i, result, file, line);
  }
  return result;
}

uint32 Int16ToUbit( int16 i, uint8 bits, char *file, uint32 line )
{
  const uint32 result = GetUbits( (uint64)((uint16)i), bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %u (%s, %u)\n", i, result, file, line);
  }
  return result;
}

uint32 Int32ToUbit( int32 i, uint8 bits, char *file, uint32 line )
{
  const uint32 result = GetUbits( (uint64)((uint32)i), bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %u (%s, %u)\n", i, result, file, line);
  }
  return result;
}

uint32 Int64ToUbit( int64 i, uint8 bits, char *file, uint32 line )
{
  const uint32 result = GetUbits( (uint64)i, bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %lli to %u (%s, %u)\n", i, result, file, line);
  }
  return result;
}

uint8 Int8ToUint8( int8 i, char *file, uint32 line )
{
  if ( i < 0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %u (%s, %u)\n", i, (uint8)i, file, line);
  }
  return (uint8)i;
}

uint8 Int16ToUint8( int16 i, char *file, uint32 line )
{
  if ( (uint8)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %u (%s, %u)\n", i, (uint8)i, file, line);
  }
  return (uint8)i;
}

uint8 Int32ToUint8( int32 i, char *file, uint32 line )
{
  if ( (uint8)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %u (%s, %u)\n", i, (uint8)i, file, line);
  }
  return (uint8)i;
}

uint8 Int64ToUint8( int64 i, char *file, uint32 line )
{
  if ( (uint8)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %lli to %u (%s, %u)\n", i, (uint8)i, file, line);
  }
  return (uint8)i;
}

uint16 Int16ToUint16( int16 i, char *file, uint32 line )
{
  if ( i < 0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %u (%s, %u)\n", i, (uint16)i, file, line);
  }
  return (uint16)i;
}

uint16 Int32ToUint16( int32 i, char *file, uint32 line )
{
  if ( (uint16)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %u (%s, %u)\n", i, (uint16)i, file, line);
  }
  return (uint16)i;
}

uint16 Int64ToUint16( int64 i, char *file, uint32 line )
{
  if ( (uint16)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %lli to %u (%s, %u)\n", i, (uint16)i, file, line);
  }
  return (uint16)i;
}

uint32 Int32ToUint32( int32 i, char *file, uint32 line )
{
  if ( i < 0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %i to %u (%s, %u)\n", i, (uint32)i, file, line);
  }
  return (uint32)i;
}

uint32 Int64ToUint32( int64 i, char *file, uint32 line )
{
  if ( (uint32)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %lli to %u (%s, %u)\n", i, (uint32)i, file, line);
  }
  return (uint32)i;
}

uint64 Int64ToUint64( int64 i, char *file, uint32 line )
{
  if ( i < 0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %lli to %llu (%s, %u)\n", i, (uint64)i, file, line);
  }
  return (uint64)i;
}

uint32 Uint8ToUbit( uint8 i, uint8 bits, char *file, uint32 line )
{
  const uint32 result = GetUbits( (uint64)i, bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %u (%s, %u)\n", i, result, file, line);
  }
  return result;
}

uint32 Uint16ToUbit( uint16 i, uint8 bits, char *file, uint32 line )
{
  const uint32 result = GetUbits( (uint64)i, bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %u (%s, %u)\n", i, result, file, line);
  }
  return result;
}

uint32 Uint32ToUbit( uint32 i, uint8 bits, char *file, uint32 line )
{
  const uint32 result = GetUbits( (uint64)i, bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %u (%s, %u)\n", i, result, file, line);
  }
  return result;
}

uint32 Uint64ToUbit( uint64 i, uint8 bits, char *file, uint32 line )
{
  const uint32 result = GetUbits( (uint64)i, bits );

  if ( (int64)result != (int64)i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %llu to %u (%s, %u)\n", i, result, file, line);
  }
  return result;
}

uint8 Uint16ToUint8( uint16 i, char *file, uint32 line )
{
  if ( (uint8)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %u (%s, %u)\n", i, (uint8)i, file, line);
  }
  return (uint8)i;
}

uint8 Uint32ToUint8( uint32 i, char *file, uint32 line )
{
  if ( (uint8)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %u (%s, %u)\n", i, (uint8)i, file, line);
  }
  return (uint8)i;
}

uint8 Uint64ToUint8( uint64 i, char *file, uint32 line )
{
  if ( (uint8)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %llu to %u (%s, %u)\n", i, (uint8)i, file, line);
  }
  return (uint8)i;
}

uint16 Uint32ToUint16( uint32 i, char *file, uint32 line )
{
  if ( (uint16)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %u to %u (%s, %u)\n", i, (uint16)i, file, line);
  }
  return (uint16)i;
}

uint16 Uint64ToUint16( uint64 i, char *file, uint32 line )
{
  if ( (uint16)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %llu to %u (%s, %u)\n", i, (uint16)i, file, line);
  }
  return (uint16)i;
}

uint32 Uint64ToUint32( uint64 i, char *file, uint32 line )
{
  if ( (uint32)i != i ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %llu to %u (%s, %u)\n", i, (uint32)i, file, line);
  }
  return (uint32)i;
}

int8 FloatToInt8( float f, char *file, uint32 line )
{
  if ( (((float)((int8)f)) - f) > 1.0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %f to %i (%s, %u)\n", f, (int8)f, file, line);
  }
  return (int8)f;
}

int16 FloatToInt16( float f, char *file, uint32 line )
{
  if ( (((float)((int16)f)) - f) > 1.0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %f to %i (%s, %u)\n", f, (int16)f, file, line);
  }
  return (int16)f;
}

int32 FloatToInt32( float f, char *file, uint32 line )
{
  if ( (((float)((int32)f)) - f) > 1.0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %f to %i (%s, %u)\n", f, (int32)f, file, line);
  }
  return (int32)f;
}

int64 FloatToInt64( float f, char *file, uint32 line )
{
  if ( (((float)((int64)f)) - f) > 1.0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %f to %lli (%s, %u)\n", f, (int64)f, file, line);
  }
  return (int64)f;
}

uint8 FloatToUint8( float f, char *file, uint32 line )
{
  if ( (((float)((uint8)f)) - f) > 1.0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %f to %u (%s, %u)\n", f, (uint8)f, file, line);
  }
  return (uint8)f;
}

uint16 FloatToUint16( float f, char *file, uint32 line )
{
  if ( (((float)((uint16)f)) - f) > 1.0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %f to %u (%s, %u)\n", f, (uint16)f, file, line);
  }
  return (uint16)f;
}

uint32 FloatToUint32( float f, char *file, uint32 line )
{
  if ( (((float)((uint32)f)) - f) > 1.0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %f to %u (%s, %u)\n", f, (uint32)f, file, line);
  }
  return (uint32)f;
}

uint64 FloatToUint64( float f, char *file, uint32 line )
{
  if ( (((float)((uint64)f)) - f) > 1.0 ) {
    MSG_SPRINTF_4
      ( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
        "Value truncated from %f to %llu (%s, %u)\n", f, (uint64)f, file, line);
  }
  return (uint64)f;
}

