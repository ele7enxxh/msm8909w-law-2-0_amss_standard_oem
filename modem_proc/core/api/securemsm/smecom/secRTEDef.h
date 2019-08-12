#ifndef SECRTEDEF_H
#define SECRTEDEF_H

/**
   @file secRTEDef.h

   @brief 
   This is the header file contains common types used accorss RTE components.

   Externalized Functions:
   None. 

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                  R T E  C O M M O N  D E F I N I T I O N S
                        H E A D E R    F I L E

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================
                         EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/secRTEDef.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/11   leo     (Tech Pubs) Edited Doxygen comments and markup.
10/27/10   nk      Removed PLATFORM_LTK feature for namespace std.
07/21/10   nk      Added doxgen related descriptions
06/11/06   ssm     Use UxString instead of string
02/23/06   rv      Changes to make file ARM compiler compliant
07/28/05   David   Tsang  Initial Creation.
=============================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include <string>
#include "comdef.h"
#include "msg.h"

/** @cond
*/
/* Namespace - not arm */ 
#ifdef __arm
#define WIN_NAMESPACE( NS )
#else
#define WIN_NAMESPACE( NS ) using namespace NS;
#endif

/* Intializing WIN NAMESPACE */
WIN_NAMESPACE( std );
/** @endcond */

/** @addtogroup SMECOMUTIL_COMDEF 
    @{ 
*/

/**
  Type used for 8-bit Unicode Transformation Format (UTF). The native type is
  unsigned character. 
*/
typedef unsigned char  utf8;

/**
  Type used for 16-bit UTF. The native type is unsigned short. 
*/
typedef unsigned short utf16;

/**
  @brief Defines the Julian time format members.
*/
typedef struct
{
uint32  Year;          /**< Year (1980 to 2100).                           */
uint32  Month;         /**< Month of the year (1 to 12).                   */
uint32  Day;           /**< Day of the month (1 to 31) or day of the year
                            (1 to 366).                                    */
uint32  Hour;          /**< Hour of the day (0 to 23).                     */
uint32  Minute;        /**< Minute of the hour (0 to 59).                  */
uint32  Second;        /**< Second of the minute (0 to 59).                */
uint32  DayOfWeek;     /**< Day of the week (0 to 6); Monday to Sunday.    */
}UxJulian;

/** @} */  /* end_addtogroup SMECOMUTIL_COMDEF */

#endif /* SECRTEDEF_H */

