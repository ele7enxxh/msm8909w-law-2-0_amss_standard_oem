#ifndef __PRNGEL_ENV_H__
#define __PRNGEL_ENV_H__

/*===========================================================================

                       P R N G E n g i n e D r i v e r

                       H e a d e r  F i l e (e x t e r n a l)

DESCRIPTION
  This header file contains HW Crypto specific declarations.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2010 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/securemsm/cryptodrivers/prng/environment/env/rpm/inc/PrngEL_Env.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/09/11   nk      Added the Mutex Enter and Exit routines
7/25/10    yk     Initial version
============================================================================*/

#include "comdef.h"
#include "PrngCL_DALIntf.h"

//#define PRNGEL_MUTEX_TYPE qurt_mutex_t

#define PRNGEL_MUTEX_ENTER()

#define PRNGEL_MUTEX_EXIT() 

#define PRNGCL_ENABLE_CLK() PrngCL_DAL_Clock_Enable()

#define PRNGCL_DISABLE_CLK() PrngCL_DAL_Clock_Disable()

#define PRNGCL_STATUS_CLK( PTRCLKFLAG ) PrngCL_DAL_Clock_Status( PTRCLKFLAG )


#endif /*__PRNGEL_ENV_H__ */
