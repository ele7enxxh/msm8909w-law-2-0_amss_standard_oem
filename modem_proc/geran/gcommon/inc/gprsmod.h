/*****************************************************************************
***
*** TITLE
***
***  GPRS MODULO UTILITY MACROS
***
***
*** DESCRIPTION
***
***  Provides a suite of modulo arithmetic comparision utilities for use
***  by the GPRS protocol. Typical usage would be frame sequence number
***  processing. 
***
*** 
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/gprsmod.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz	   changes
***
*****************************************************************************/

#ifndef INC_GPRSMOD_H
#define INC_GPRSMOD_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         GMODULO() 
===
===  DESCRIPTION
===
===   Simply computes the modulo of a given value.
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   Given modulo of the given value.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

#define GMODULO(mod, val)      ((val) % (mod))


/*===========================================================================
===
===  MACRO         GMODULO_ABS() 
===
===  DESCRIPTION
===
===   Computes the absolute modulo of a value which could be negative.
===
===  DEPENDENCIES
===
===   The mod parameter must be positive.
===  
===  RETURN VALUE
===
===   Absolute modulo of the given value.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

#define GMODULO_ABS(mod, val)                                               \
(                                                                           \
  ((val) >= 0) ? (val)%(mod):(mod)+((val)%(mod))                            \
)


/*===========================================================================
===
===  MACRO         GMODULO_GT() 
===
===  DESCRIPTION
===
===   Determines whether the given modulo value is greater than the given
===   limit.
===
===   For the comparison to be valid, the true difference between the value
===   and comparison limit must be less than half the modulo.
===
===  DEPENDENCIES
===
===   All parameters must be positive. Use GMODULO_ABS() to convert
===   potentially negative values before use.
===  
===  RETURN VALUE
===
===   TRUE if the value is greater than the limit, FALSE otherwise.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

#define GMODULO_GT(mod, val, lim)                                           \
(                                                                           \
  (((val) > (lim)) && ((val)-(lim) < (mod)/2))                              \
  ? TRUE : (((mod)+(val) > (lim)) && ((mod)+(val)-(lim) < (mod)/2))         \
)


/*===========================================================================
===
===  MACRO         GMODULO_GTE() 
===
===  DESCRIPTION
===
===   Determines whether the given modulo value is greater than or equal to
===   the given limit.
===
===   For the comparison to be valid, the true difference between the value
===   and comparison limit must be less than half the modulo.
===
===  DEPENDENCIES
===
===   All parameters must be positive. Use GMODULO_ABS() to convert
===   potentially negative values before use.
===  
===  RETURN VALUE
===
===   TRUE if the value is greater than or equal to the limit, FALSE
===   otherwise.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

#define GMODULO_GTE(mod, val, lim)                                          \
(                                                                           \
  (((val) >= (lim)) && ((val)-(lim) < (mod)/2))                             \
  ? TRUE : (((mod)+(val) > (lim)) && ((mod)+(val)-(lim) < (mod)/2))         \
)


/*===========================================================================
===
===  MACRO         GMODULO_LT() 
===
===  DESCRIPTION
===
===   Determines whether the given modulo value is less than the given limit.
===
===   For the comparison to be valid, the true difference between the value
===   and comparison limit must be less than half the modulo.
===
===  DEPENDENCIES
===
===   All parameters must be positive. Use GMODULO_ABS() to convert
===   potentially negative values before use.
===  
===  RETURN VALUE
===
===   TRUE if the value is less than the limit, FALSE
===   otherwise.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

#define GMODULO_LT(mod, val, lim)                                           \
(                                                                           \
  (((lim) > (val)) && ((lim)-(val) < (mod)/2))                              \
  ? TRUE : (((mod)+(lim) > (val)) && ((mod)+(lim)-(val) < (mod)/2))         \
)


/*===========================================================================
===
===  MACRO         GMODULO_LTE() 
===
===  DESCRIPTION
===
===   Determines whether the given modulo value is less than or equal to the
===   given limit.
===
===   For the comparison to be valid, the true difference between the value
===   and comparison limit must be less than half the modulo.
===
===  DEPENDENCIES
===
===   All parameters must be positive. Use GMODULO_ABS() to convert
===   potentially negative values before use.
===  
===  RETURN VALUE
===
===   TRUE if the value is less than or equal to the limit, FALSE
===   otherwise.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

#define GMODULO_LTE(mod, val, lim)                                          \
(                                                                           \
  (((lim) >= (val)) && ((lim)-(val) < (mod)/2))                             \
  ? TRUE : (((mod)+(lim) >= (val)) && ((mod)+(lim)-(val) < (mod)/2))        \
)


/*===========================================================================
===
===  MACRO         GMODULO_IN_RGE() 
===
===  DESCRIPTION
===
===   Determines whether the given modulo value is less than or equal to the
===   given maximum and greater than or equal to the given minimum.
===
===   For the comparison to be valid, the true difference between the value
===   and comparison limit must be less than half the modulo.
===
===  DEPENDENCIES
===
===   All parameters must be positive. Use GMODULO_ABS() to convert
===   potentially negative values before use.
===  
===  RETURN VALUE
===
===   TRUE if the value is in range, FALSE otherwise.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

#define GMODULO_IN_RGE(mod, val, max, min)                                  \
(                                                                           \
  GMODULO_GTE(mod, val, min) && GMODULO_LTE(mod, val, max)                  \
)



/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


#endif /* INC_GPRSMOD_H	*/

/*** EOF: don't remove! ***/
