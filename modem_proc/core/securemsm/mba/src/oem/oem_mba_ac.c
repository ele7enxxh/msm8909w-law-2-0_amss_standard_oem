/*=============================================================================


                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/mba/src/oem/oem_mba_ac.c#1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  07/01/12   mm      Initial Revision for MSM8974
 
=============================================================================*/


/*=============================================================================
                                MODULE INCLUDES
                          ADD NEW ONES UNDER THIS LINE    
=============================================================================*/
#include <IxErrno.h>
#include "HALxpu2.h"
#include <string.h>
#include "mba_bimc.h"
#include "mba_ac.h"
#include "secboot_hw.h"

uint8 mba_oem_seccrtl_allow_unlock_xpu(void)
{
  return FALSE;
}
