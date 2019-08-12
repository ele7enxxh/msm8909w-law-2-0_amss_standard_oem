#ifndef QPOWER_H
#define QPOWER_H
/*=============================================================================

                 qpower.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qpower API

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qube/qpower.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include  "qurt_power.h"

/*=============================================================================
                      CONSTANTS AND MACROS                                
=============================================================================*/

#define qpower_shutdown_prepare   qurt_power_shutdown_prepare 
#define qpower_shutdown_fail_exit qurt_power_shutdown_fail_exit
#define qpower_shutdown(a)        qurt_power_shutdown_enter(QURT_POWER_SHUTDOWN_TYPE_L2NORET)
#define qpower_shutdown_exit      qurt_power_shutdown_exit

#define qpower_tcxo_prepare       qurt_power_tcxo_prepare
#define qpower_tcxo_fail_exit     qurt_power_tcxo_fail_exit
#define qpower_tcxo_enter         qurt_power_tcxo_enter
#define qpower_tcxo_exit          qurt_power_tcxo_exit

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QPOWER_H */
