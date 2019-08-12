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


/**@file compiler_arm.h

Holds definitions specific to the ARM&reg; &nbsp; compiler.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/compiler_arm.h#1 $
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


#ifndef _COMPILER_ARM_H_
#define _COMPILER_ARM_H_


/** @ingroup hsusb_jungo_api 
@{ */

/* Global mandatory definitions */
#define __RESTRICT__
#define __VOLATILE__ volatile

/* HSU Temp fix - compilation error for 6290 (redefenition on compiler_cl.h) */
#define __INLINE__ __inline
/* End of HSU Temp fix - compilation error for 6290 (redefenition on compiler_cl.h) */

#define __PACKED_PRE__ __packed
#define __PACKED_POST__ 

#define __FILE_NAME__ __MODULE__
/* Special definitions */
//#pragma anon_unions

/** @} */ /* end_group hsusb_jungo_api */

#endif
