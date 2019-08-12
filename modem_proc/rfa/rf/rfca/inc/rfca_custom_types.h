/*!
  @file
  rfca_custom_types.h
 
  @brief
  Some numerical type defines used for RFCA.

*/

/*==============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_custom_types.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/10/13   aki     Mainline RFCA
09/14/12   aki     Added MIN and MAX defines
03/01/12   aki     Removed extra comdef.h includes
10/28/11   aki     Initial revision

==============================================================================*/


#ifndef _RFCA_CUSTOM_TYPES_H_
#define _RFCA_CUSTOM_TYPES_H_

#include "rfa_variation.h"
#include "comdef.h"


typedef int16 int16_q8;   //!< Decimal number in Q8 format stored in int16

/*!
 @brief Round a decimal number to int16

 @details Rounds to nearest integer and away from zero.

 @see http://en.wikipedia.org/wiki/Rounding

 @param number Decimal number to round

 @note Function does NOT check if given decimal can fit into int16
 */
#define round_to_int16(number)  ((int16)((number >= 0) ? (int16)(number + 0.5) : (int16)(number - 0.5)))

/*!
 @brief Convert Q8 decimal value stored in int16 to double.
  
 @see http://en.wikipedia.org/wiki/Q_(number_format)

 @param number_as_q8 Q8 number to convert
 */
#define int16_q8_to_double(number_as_q8)     ( (  double   ) ( ( ( int16_q8 ) number_as_q8     ) * ( 1.0 / 256 ) ) )

/*!
 @brief Convert double to Q8 decimal and store into int16 

 @see http://en.wikipedia.org/wiki/Q_(number_format)

 @param number_as_double Double bumber to convert
 */
#define double_to_int16_q8(number_as_double) ( ( int16_q8 ) round_to_int16(( ( ( double    ) number_as_double ) *         256   ) ))

/*!
 @brief Min value that int16 q8 can represent
 */
#define MIN_INT16_Q8_NUMBER (-128.0)

/*!
 @brief Max value that int16 q8 can represent
 */
#define MAX_INT16_Q8_NUMBER (127.996)

#endif //_RFCA_CUSTOM_TYPES_H_
