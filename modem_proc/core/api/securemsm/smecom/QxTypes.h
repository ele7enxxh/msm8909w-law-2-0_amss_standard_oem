#ifndef __QXTYPES_H__
#define __QXTYPES_H__


/**
   @file QxTypes.h

   @brief 
   Definitions for general macros and types.

   This header file contains general types and macros that are of use
   to all modules.

   Externalized Functions:
   MAX(x,y)
   MIN(x,y)

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                                Q X  T Y P E S
                             H E A D E R  F I L E

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/QxTypes.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ 2005/09/07 
$Author: mplcsds1 $ Madhav Chinthala  


when        who         what, where, why
--------    ---         ----------------------------------------------------
01/17/11    leo         (Tech Pubs) Edited Doxygen comments and markup.
12/28/10    nk          Doxygenated.
09/07/05    MC          Initial Version
11/29/05    MC          Added some more definations

===========================================================================*/
/** @addtogroup SMECOMUTIL_COMDEF 
    @{
*/

/**
@name Common Defined Constants
@{
*/
 
#define TRUE    1    /**< Asserted Boolean condition (Logical 1) value. */
#define FALSE   0    /**< Deasserted Boolean condition (Logical 0) value. */

#define ON      1    /**< Asserted condition. */
#define OFF     0    /**< Deasserted condition. */

#ifndef NULL
#define NULL   '\0'  /**< NULL string. */
#endif

#define SUCCESS   0    /**< Success case. */
#define FAILED   -1    /**< Failure case. */

/** @} */ /* end_name common constants */

/**
@name Common Type Definitions for Data Types
@{
*/

typedef signed int           QxInt;     /**< Qx signed integer.                */
typedef unsigned int         QxUInt;    /**< Qx unsigned integer.              */

typedef float                QxFloat;   /**< Qx float.                         */
typedef long                 QxLong;    /**< Qx long.                          */
typedef double               QxDouble;  /**< Qx double.                        */

typedef signed short         QxInt16;   /**< Qx 16-bit signed integer value.   */
typedef unsigned short       QxUInt16;  /**< Qx 16-bit unsigned integer value. */
typedef signed long int      QxInt32;   /**< Qx 32-bit signed integer value.   */
typedef unsigned long int    QxUInt32;  /**< Qx 32-bit unsigned integer value. */

typedef const char           QxCChar;   /**< Qx constant character type.       */
typedef char                 QxChar;    /**< Qx character type.                */

typedef signed char          QxInt8;    /**< Qx 8-bit signed integer value.    */
typedef unsigned char        QxUInt8;   /**< Qx 8-bit unsigned integer value.  */

typedef unsigned char        QxByte;    /**< Qx 8-bit unsigned value.       */

//typedef QxUInt16           QxWord;    /* Qx 16-bit signed value.             */

/** @} */ /*  end_name common typedefs */

/**
   @name Helper Macros for Maximum and Minimum of Specified Values

   These defines return the maximum value for MAX and the minimum value for
   MIN, where x is input variable 1 and y is input variable 2.

@{ */

#ifndef MAX
   #define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) ) /**< Gets the maximum of x and y. */
#endif

#ifndef MIN
   #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) ) /**< Gets the minimum of x and y. */
#endif

/** @} */ /* end_name helper macros */

/** @} */  /* end_addtogroup SMECOMUTIL_COMDEF */

#endif // __QXTYPES_H__
