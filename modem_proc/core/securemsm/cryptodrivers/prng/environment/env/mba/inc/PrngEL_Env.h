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

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/cryptodrivers/prng/environment/env/mba/inc/PrngEL_Env.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/09/11   nk      Added the Mutex Enter and Exit routines
7/25/10    yk     Initial version
============================================================================*/

#include "comdef.h"
#include "rex.h"

#define PRNGEL_MUTEX_TYPE rex_crit_sect_type

#define PRNGEL_MUTEX_INIT()

#define PRNGEL_MUTEX_ENTER() 

#define PRNGEL_MUTEX_EXIT() 

#define PRNGCL_ENABLE_CLK() 0

#define PRNGCL_DISABLE_CLK() 0

#define PRNGCL_STATUS_CLK( PTRCLKFLAG ) 

#define PRNGEL_MEMORY_BARRIER()


#endif /*__PRNGEL_ENV_H__ */
