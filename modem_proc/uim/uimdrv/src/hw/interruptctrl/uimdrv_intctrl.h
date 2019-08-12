#ifndef UIMDRV_INTCTRL_H
#define UIMDRV_INTCTRL_H
/*============================================================================
  FILE:        uimdrv_intctrl.h

  OVERVIEW:    Contains the implementation of the UARTDM class for uim drivers.

  DEPENDENCIES: N/A

                Copyright (c) 2012-2014 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/hw/interruptctrl/uimdrv_intctrl.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       -----------------------------------------------------------
09/18/14    akv        UIMDRV UT enhancements
11/27/13    akv        Initialization of hw enumeration based on dal chip family
10/08/13    nmb        HW header file cleanup
07/12/13    nmb        Global Reorganization
07/12/13    akv        HW Enumeration - modifying target specific hardware values 
                       and organizing them in value-key pairs
12/05/12    js         UIM Parallel Processing changes
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "comdef.h"
#include "uimdrv_main.h"


/* Table holding the default UIM Controller IRQ number for different UIM instances. */
extern  const uim_controller_irq_num_table_type   uim_controller_irq_num_table[];

/* Table holding the default UARTDM IRQ number for different UIM instances. */
extern const uim_uartdm_irq_num_table_type       uim_uartdm_irq_num_table[];

/* Initialize interrupt controller */
void initialize_intctrl(uim_instance_global_type *uim_ptr);

#endif /* UIMDRV_INTCTRL_H */
