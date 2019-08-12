#ifndef intconv_h
#define intconv_h 

/**
  @file intconv.h
  @brief Routines for converting integers.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_number_manip" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*
Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/intconv.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
03/15/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/12/10   EBR     Doxygenated File.
05/10/05   sst     Integer conmversion functions.
===========================================================================*/
/** @addtogroup utils_number_manip
@{ */

#include "comdef.h"

/**
  Compares two boolean values to see if they are equal.

  @param[in] a First value.
  @param[in] b Second value.

  @return
  Nonzero -- Booleans are equal. \n
  0 -- Booleans are not equal.

  @dependencies
  None.
*/
#define bool_equal(a,b) ((a) ? (b) : !(b))

/**
  Converts a boolean to an integer.

  @param[in] i Boolean to be converted.

  @return
  0 -- Boolean was not converted. \n
  1 -- Boolean is converted.

  @dependencies
  None.
*/
#define booltoint(i) ((i) ? 1 : 0)

/**
  Converts an integer to a boolean.

  @note1 Nonzero is false.

  @param[in] i Integer to be converted.

  @return
  The boolean value.

  @dependencies
  None.
*/
#define inttobool(i) (((i) == 0) ? false : true)
/**
  Normalizes the boolean passed in.

  @param[in] b Boolean to be normalized.

  @return
  The normalized value.

  @dependencies
  None.
*/
#define normalizebool(b) ((b) ? true : false)

/** @cond 
*/
#define int8tobit( i, bits ) Int8ToBit( (i), (bits), __FILE__, __LINE__ )
#define int16tobit( i, bits ) Int16ToBit( (i), (bits), __FILE__, __LINE__ )
#define int32tobit( i, bits ) Int32ToBit( (i), (bits), __FILE__, __LINE__ )
#define int64tobit( i, bits ) Int64ToBit( (i), (bits), __FILE__, __LINE__ )

#define int16toint8( i ) Int16ToInt8( (i), __FILE__, __LINE__ )
#define int32toint8( i ) Int32ToInt8( (i), __FILE__, __LINE__ )
#define int64toint8( i ) Int64ToInt8( (i), __FILE__, __LINE__ )

#define int32toint16( i ) Int32ToInt16( (i), __FILE__, __LINE__ )
#define int64toint16( i ) Int64ToInt16( (i), __FILE__, __LINE__ )

#define int64toint32( i ) Int64ToInt32( (i), __FILE__, __LINE__ )


#define uint8tobit( i, bits ) Uint8ToBit( (i), (bits), __FILE__, __LINE__ )
#define uint16tobit( i, bits ) Uint16ToBit( (i), (bits), __FILE__, __LINE__ )
#define uint32tobit( i, bits ) Uint32ToBit( (i), (bits), __FILE__, __LINE__ )
#define uint64tobit( i, bits ) Uint64ToBit( (i), (bits), __FILE__, __LINE__ )

#define uint8toint8( i ) Uint8ToInt8( (i), __FILE__, __LINE__ )
#define uint16toint8( i ) Uint16ToInt8( (i), __FILE__, __LINE__ )
#define uint32toint8( i ) Uint32ToInt8( (i), __FILE__, __LINE__ )
#define uint64toint8( i ) Uint64ToInt8( (i), __FILE__, __LINE__ )

#define uint16toint16( i ) Uint16ToInt16( (i), __FILE__, __LINE__ )
#define uint32toint16( i ) Uint32ToInt16( (i), __FILE__, __LINE__ )
#define uint64toint16( i ) Uint64ToInt16( (i), __FILE__, __LINE__ )

#define uint32toint32( i ) Uint32ToInt32( (i), __FILE__, __LINE__ )
#define uint64toint32( i ) Uint64ToInt32( (i), __FILE__, __LINE__ )

#define uint64toint64( i ) Uint64ToInt64( (i), __FILE__, __LINE__ )


#define int8toubit( i, bits ) Int8ToUbit( (i), (bits), __FILE__, __LINE__ )
#define int16toubit( i, bits ) Int16ToUbit( (i), (bits), __FILE__, __LINE__ )
#define int32toubit( i, bits ) Int32ToUbit( (i), (bits), __FILE__, __LINE__ )
#define int64toubit( i, bits ) Int64ToUbit( (i), (bits), __FILE__, __LINE__ )

#define int8touint8( i ) Int8ToUint8( (i), __FILE__, __LINE__ )
#define int16touint8( i ) Int16ToUint8( (i), __FILE__, __LINE__ )
#define int32touint8( i ) Int32ToUint8( (i), __FILE__, __LINE__ )
#define int64touint8( i ) Int64ToUint8( (i), __FILE__, __LINE__ )

#define int16touint16( i ) Int16ToUint16( (i), __FILE__, __LINE__ )
#define int32touint16( i ) Int32ToUint16( (i), __FILE__, __LINE__ )
#define int64touint16( i ) Int64ToUint16( (i), __FILE__, __LINE__ )

#define int32touint32( i ) Int32ToUint32( (i), __FILE__, __LINE__ )
#define int64touint32( i ) Int64ToUint32( (i), __FILE__, __LINE__ )

#define int64touint64( i ) Int64ToUint64( (i), __FILE__, __LINE__ )


#define uint8toubit( i, bits ) Uint8ToUbit( (i), (bits), __FILE__, __LINE__ )
#define uint16toubit( i, bits ) Uint16ToUbit( (i), (bits), __FILE__, __LINE__ )
#define uint32toubit( i, bits ) Uint32ToUbit( (i), (bits), __FILE__, __LINE__ )
#define uint64toubit( i, bits ) Uint64ToUbit( (i), (bits), __FILE__, __LINE__ )

#define uint16touint8( i ) Uint16ToUint8( (i), __FILE__, __LINE__ )
#define uint32touint8( i ) Uint32ToUint8( (i), __FILE__, __LINE__ )
#define uint64touint8( i ) Uint64ToUint8( (i), __FILE__, __LINE__ )

#define uint32touint16( i ) Uint32ToUint16( (i), __FILE__, __LINE__ )
#define uint64touint16( i ) Uint64ToUint16( (i), __FILE__, __LINE__ )

#define uint64touint32( i ) Uint64ToUint32( (i), __FILE__, __LINE__ )

#define floattoint8( f ) FloatToInt8( (f), __FILE__, __LINE__ )
#define floattoint16( f ) FloatToInt16( (f), __FILE__, __LINE__ )
#define floattoint32( f ) FloatToInt32( (f), __FILE__, __LINE__ )
#define floattoint64( f ) FloatToInt64( (f), __FILE__, __LINE__ )

#define floattouint8( f ) FloatToUint8( (f), __FILE__, __LINE__ )
#define floattouint16( f ) FloatToUint16( (f), __FILE__, __LINE__ )
#define floattouint32( f ) FloatToUint32( (f), __FILE__, __LINE__ )
#define floattouint64( f ) FloatToUint64( (f), __FILE__, __LINE__ )
/** @endcond */

/**
  Selects a variable number of bits from an int8 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Int8ToBit( int8 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from an int16 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Int16ToBit( int16 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from an int32 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Int32ToBit( int32 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from an int64 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Int64ToBit( int64 i, uint8 bits, char *file, uint32 line );

/**
  Converts int16 to int8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int8 Int16ToInt8( int16 i, char *file, uint32 line );
/**
  Converts int32 to int8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int8 Int32ToInt8( int32 i, char *file, uint32 line );
/**
  Converts int64 to int8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int8 Int64ToInt8( int64 i, char *file, uint32 line );

/**
  Converts int32 to int16.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int16 Int32ToInt16( int32 i, char *file, uint32 line );
/**
  Converts int64 to int16.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int16 Int64ToInt16( int64 i, char *file, uint32 line );
/**
  Converts int64 to int32.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Int64ToInt32( int64 i, char *file, uint32 line );

/**
  Selects a variable number of bits from a uint8 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Uint8ToBit( uint8 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from a uint16 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Uint16ToBit( uint16 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from a uint32 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Uint32ToBit( uint32 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from a uint64 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Uint64ToBit( uint64 i, uint8 bits, char *file, uint32 line );

/**
  Converts uint8 to int8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int8 Uint8ToInt8( uint8 i, char *file, uint32 line );
/**
  Converts uint16 to int8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int8 Uint16ToInt8( uint16 i, char *file, uint32 line );
/**
  Converts uint32 to int8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int8 Uint32ToInt8( uint32 i, char *file, uint32 line );
/**
  Converts uint64 to int8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int8 Uint64ToInt8( uint64 i, char *file, uint32 line );

/**
  Converts uint16 to int16.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int16 Uint16ToInt16( uint16 i, char *file, uint32 line );
/**
  Converts uint32 to int16.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int16 Uint32ToInt16( uint32 i, char *file, uint32 line );
/**
  Converts uint64 to int16.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int16 Uint64ToInt16( uint64 i, char *file, uint32 line );

/**
  Converts uint32 to int32.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Uint32ToInt32( uint32 i, char *file, uint32 line );
/**
  Converts uint64 to int32.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 Uint64ToInt32( uint64 i, char *file, uint32 line );

/**
  Converts uint64 to int64.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int64 Uint64ToInt64( uint64 i, char *file, uint32 line );

/**
  Selects a variable number of bits from an int8 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The unsigned number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Int8ToUbit( int8 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from an int16 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The unsigned number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Int16ToUbit( int16 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from an int32 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The unsigned number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Int32ToUbit( int32 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from an int64 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
   The unsigned number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Int64ToUbit( int64 i, uint8 bits, char *file, uint32 line );

/**
  Converts int8 to uint8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint8 Int8ToUint8( int8 i, char *file, uint32 line );
/**
  Converts int16 to uint8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint8 Int16ToUint8( int16 i, char *file, uint32 line );
/**
  Converts int32 to uint8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint8 Int32ToUint8( int32 i, char *file, uint32 line );
/**
  Converts int64 to uint8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint8 Int64ToUint8( int64 i, char *file, uint32 line );

/**
  Converts int16 to uint16.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint16 Int16ToUint16( int16 i, char *file, uint32 line );
/**
  Converts int32 to uint16.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint16 Int32ToUint16( int32 i, char *file, uint32 line );
/**
  Converts int64 to uint16.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint16 Int64ToUint16( int64 i, char *file, uint32 line );

/**
  Converts int32 to uint32.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Int32ToUint32( int32 i, char *file, uint32 line );
/**
  Converts int64 to uint32.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Int64ToUint32( int64 i, char *file, uint32 line );

/**
  Converts int64 to uint64.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint64 Int64ToUint64( int64 i, char *file, uint32 line );

/**
  Selects a variable number of bits from a uint8 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The unsigned number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Uint8ToUbit( uint8 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from a uint16 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The unsigned number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Uint16ToUbit( uint16 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from a uint32 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The unsigned number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Uint32ToUbit( uint32 i, uint8 bits, char *file, uint32 line );
/**
  Selects a variable number of bits from a uint64 number.

  @param[in] i    Number from which the bits are selected.
  @param[in] bits Number of bits to take from the number.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The unsigned number created by the bitmask.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Uint64ToUbit( uint64 i, uint8 bits, char *file, uint32 line );

/**
  Converts uint16 to uint8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint8 Uint16ToUint8( uint16 i, char *file, uint32 line );
/**
  Converts uint32 to uint8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint8 Uint32ToUint8( uint32 i, char *file, uint32 line );
/**
  Converts uint64 to uint8.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint8 Uint64ToUint8( uint64 i, char *file, uint32 line );

/**
  Converts uint32 to uint16.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint16 Uint32ToUint16( uint32 i, char *file, uint32 line );
/**
  Converts uint64 to uint16.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint16 Uint64ToUint16( uint64 i, char *file, uint32 line );

/**
  Converts uint64 to uint32.

  @param[in] i    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 Uint64ToUint32( uint64 i, char *file, uint32 line );

/**
  Converts float to int8.

  @param[in] f    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int8 FloatToInt8( float f, char *file, uint32 line );
/**
  Converts float to int16.

  @param[in] f    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int16 FloatToInt16( float f, char *file, uint32 line );
/**
  Converts float to int32.

  @param[in] f    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int32 FloatToInt32( float f, char *file, uint32 line );
/**
  Converts float to int64.

  @param[in] f    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
int64 FloatToInt64( float f, char *file, uint32 line );

/**
  Converts float to uint8.

  @param[in] f    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint8 FloatToUint8( float f, char *file, uint32 line );
/**
  Converts float to uint16.

  @param[in] f    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint16 FloatToUint16( float f, char *file, uint32 line );
/**
  Converts float to uint32.

  @param[in] f    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint32 FloatToUint32( float f, char *file, uint32 line );
/**
  Converts float to uint64.

  @param[in] f    Value to be converted.
  @param[in] file File from which the function was called.
  @param[in] line Line number from which the function was called.

  @return
  The newly converted value.

  @dependencies
  None.

  @sideeffects
  If the data is lost, a diagnostic message will be sent.
*/
uint64 FloatToUint64( float f, char *file, uint32 line );

/** @} */ /* end_addtogroup utils_number_manip */
#endif
