#ifndef __IXTYPES_H__
#define __IXTYPES_H__

/*===========================================================================
*
*                               Q X  T Y P E S
*             
*                            H E A D E R  F I L E
*
* FILE:  IxTypes.h
*
* DESCRIPTION :
*
* This header file contains general types and macros that are of use
* to all modules.
*
* DEFINED TYPES:
*
*       Name      Definition
*       -------   --------------------------------------------------------
*       IxByte    8  bit unsigned value
*
*       IxInt8    8  bit unsigned value
*       IxUInt16  16 bit unsigned value
*       IxUInt32  32 bit unsigned value
*
*       IxInt8    8  bit signed value
*       IxInt16   16 bit signed value 
*       IxInt32   32 bit signed value
*
*
* DEFINED CONSTANTS:
*
*       Name      Definition
*       -------   --------------------------------------------------------
*       TRUE      Asserted boolean condition (Logical 1)
*       FALSE     Deasserted boolean condition (Logical 0)
* 
*       ON        Asserted condition
*       OFF       Deasserted condition
*
*       NULL      Pointer to nothing
*
*
* Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixutil/shared/inc/IxTypes.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 2005/09/07 
  $Author: mplcsds1 $ Madhav Chinthala  


when		who			what, where, why
--------	---			----------------------------------------------------
09/07/05	MC			Initial Version
11/29/05	MC			Added some more definations
			
===========================================================================*/

/* ------------------------------------------------------------------------
*  Constants
*  ------------------------------------------------------------------------*/


#define TRUE	1   /* Boolean true value. */
#define FALSE	0   /* Boolean false value. */

#define ON		1   /* On value. */
#define OFF		0   /* Off value. */

#ifndef NULL
#define NULL	'\0'
#endif

#define SUCCESS		0
#define FAILED		-1


// Typedef's for data types
typedef signed int			  IxInt;
typedef unsigned int		  IxUInt;

typedef float             IxFloat;
typedef long              IxLong;
typedef double            IxDouble;

typedef signed short		IxInt16;
typedef unsigned short		IxUInt16;
typedef signed long int		IxInt32;
typedef unsigned long int	IxUInt32;
typedef long long           IxInt64;
typedef unsigned long long  IxUInt64;

typedef const char			  IxCChar;
typedef char				  IxChar;

typedef signed char			  IxInt8;
typedef unsigned char		  IxUInt8;

typedef unsigned char		  IxByte;

//typedef IxUInt16          IxWord;

// MACROS

#ifndef MAX
   #define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

#ifndef MIN
   #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif


#endif // __QXTYPES_H__

