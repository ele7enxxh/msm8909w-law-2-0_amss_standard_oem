/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      S E C U R I T Y    S E R V I C E S
 

GENERAL DESCRIPTION
Vote for clks for faster crypto operation

EXTERNAL FUNCTIONS

Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header:  
  $DateTime: 
  $Author: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/15   yash    initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "secerrno.h"

/*===========================================================================
*
* FUNCTION: secapi_clk_q6_enable ()
*
* DESCRIPTION:
*
* RETURN IxErrnoType
*
*   See header file
*
===========================================================================*/
IxErrnoType secapi_clk_q6_enable (void);


/*===========================================================================
*
* FUNCTION: secapi_clk_q6_disable ()
*
* DESCRIPTION: Bump up the Q6 CPU clock for cryptographic SW algorithms.
*
* RETURN IxErrnoType
*
*   See header file
*
===========================================================================*/
IxErrnoType secapi_clk_q6_disable (void);



