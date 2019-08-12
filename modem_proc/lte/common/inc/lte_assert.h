#ifndef LTE_ASSERT_H
#define LTE_ASSERT_H
/*==========================================================================

                    L T E   A S S E R T   S E R V I C E S

                          H E A D E R   F I L E

DESCRIPTION
  This header file defines LTE ASSERT related types and interfaces.

Copyright (c) 2008 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/



/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/common/inc/lte_assert.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/08   sm      Added LTE_UNLIKELY_TRUE in LTE_ASSERT and LTE_LOG_ASSERT to
                   benefit from branch prediction
07/23/08   vk      Initial version of the assert macros

===========================================================================*/

#include "err.h"
#include "msg.h"
#include "msgcfg.h"
#include "lte_as.h"


extern const char lte_assert_msg[];

/*===========================================================================

MACRO LTE_ERROR

DESCRIPTION
  This is the LTE_ERROR macro. This is to be used in all places where code
  has detected a critical error and CODE HAS HANDLED IT IN A GRACEFUL AND
  PREDICTABLE WAY like sending back a failure or a reject or otherwise aborting
  the processing.  This macro will help us stop at all points where such
  a critical error is detected so that we can debug easily.
  This macro should never be expected to error fatal AND it is NOT expected to be
  used when there is no error i.e just for output-ing something on the screen.
  For off-target testing this can be set to an error fatal, again, just to
  facilitate debugging.

  This is good to use after error validation at a module's top level fns
  which are probably seeing the inpur arguents from a different module. There
  proper error checks are recommended and a grecful recovery is desirable. There
  use LTE_ERROR macros to catch error right where it is seen.

  If error is not handled, use LTE_ASSERT instead.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

#define LTE_ERROR( fmt, arg1, arg2, arg3 ) \
    /*lint --e{527} */ \
    MSG_3(MSG_SSID_LTE, MSG_LEGACY_FATAL,  fmt, arg1, arg2, arg3 ); \
	lte_error_occured();


/* prototype for the BP Settable function */
extern void lte_error_occured
(
  void
);



/* <EJECT> */
/*===========================================================================

MACRO LTE_ASSERT

DESCRIPTION
  Given a boolean expression, verify that the input expression is TRUE.
  If the input expression is FALSE, flags an error and resets SW stack.

  Use this in all places where you are NOT handling or otherwise checking for
  this error gracefully. This can be used for  out of range
  function parameters, NULL pointers and unintentional fall to the default
  case of a switch statement. The assumption is that no additional error
  recovery mechanism is present in the code at the time these macros are
  placed. If and when error recovery code is developed, default class macros
  are expected to be changed to LTE_ERROR macro.

  Note : This is not good for error validation at a module's top level fns
  which are probably seeing the inpur arguents from a different module. There
  proper error checks are recommended and a grecful recovery is desirable. There
  use LTE_ERROR macros to catch error right where it is seen.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  This may cause error fatal

===========================================================================*/
#define LTE_ASSERT( exp ) \
    /*lint -save -e506 -e774 */ \
    if(LTE_UNLIKELY_TRUE(!(exp)) ) \
    { \
       ERR_FATAL( "Assert " #exp " failed: ", 0, 0, 0 ); \
    } \
    /*lint -restore */ \




/*===========================================================================

MACRO LTE_ASSERT_LOG

DESCRIPTION
  Given a boolean expression, verify that the input expression is TRUE.
  If the input expression is FALSE, flags an error and resets SW stack.
  This is to be used if the developer has determined that this error is critical
  in nature iof assertion fails, No easy recovery mechanism is possible, Only
  action to be taken is to reset.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  This WILL call error fatal

===========================================================================*/
#define LTE_ASSERT_LOG(exp, fmt, arg1, arg2, arg3 ) \
    /*lint -save -e506 -e774 */ \
    if( LTE_UNLIKELY_TRUE(!(exp)) ) \
    { \
      /*lint --e{527} */ \
      ERR_FATAL( "Assert " #exp " failed: " fmt, arg1, arg2, arg3 ); \
    } \
    /*lint -restore */ \

/* <EJECT> */


/* <EJECT> */

#endif /* LTE_ASSERT.H */



