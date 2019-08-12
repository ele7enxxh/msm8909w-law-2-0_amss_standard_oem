/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file jerrno.h

Holds all error codes supported by the Jungo USB stack.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/jerrno.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/21/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _JERRNO_H_
#define _JERRNO_H_


/** @ingroup hsusb_jungo_api 
@{ */

#define JEPERM           -1           /**< Operation is not permitted. */
#define JENOENT          -2           /**< No such file or directory. */
#define JEINTR           -4           /**< Interrupted system call. */
#define JEIO             -5           /**< Input/output error. */
#define JENXIO           -6           /**< Device is not configured. */
#define JE2BIG           -7           /**< Argument list is too long. */
#define JEAGAIN          -11          /**< Try again. */
#define JENOMEM          -12          /**< Cannot allocate memory. */
#define JEACCES          -13          /**< Permission is denied. */
#define JEBUSY           -16          /**< Device is busy. */
#define JENODEV          -19          /**< Operation is not supported by the 
                                           device. */
#define JEINVAL          -22          /**< Invalid argument. */
#define JENOTTY          -25          /**< Inappropriate IOCTL for the device. */
#define JEFBIG           -27          /**< File is too large. */
#define JENOSPC          -28          /**< No space is left on the device. */
#define ECANCEL          -29          /**< Transfer is canceled. */
/* HSU addition. */
#define JOUTOFEPS        -30          /**< Not enough endpoints. */
/* End of HSU addition. */
#define JEWOULDBLOCK     JEAGAIN      /**< Operation would block; try again. */
#define JERESTART        -85          /**< Restart the system call. */
#define JEOPNOTSUPP      -95          /**< Operation is not supported. */
#define JENOTSUP         JEOPNOTSUPP  /**< Operation is not supported. */
#define JETIMEDOUT       -110         /**< Operation has timed out. */
#define JENOMEDIUM       -123         /**< No medium was found. */
#define ESUSPENDED      -200          /**< Device is suspended. */
#define EUNKNOWN        -1000         /**< General error. */
#define JETEST_FAILED    -1001        /**< Test failed. */

/** @} */ /* end_group hsusb_jungo_api */

#endif
