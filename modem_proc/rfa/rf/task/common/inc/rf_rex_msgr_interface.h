#ifndef RF_REX_MSGR_INTERFACE_H
#define RF_REX_MSGR_INTERFACE_H
/*!
  @file
  RF Command interface file.

  @defgroup RF_TASK_CMD_interface
  @ingroup RF_TASK
  @{
*/
/*===========================================================================

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/inc/rf_rex_msgr_interface.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/05/12   av      Expanded rf task architecture to include more tasks. 
11/06/09   mkv	   Moving MSGR registration and de-registration required for RF Task 
				   into LTE files. 
08/26/09   mkv     Integrating REX MSGR interworking into RFA2
07/16/09   mkv     Initial revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "rex.h"
#include "msgr.h"
#include "rf_task_common_services.h"


/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/
typedef msgr_hdr_struct_type rf_msgr_hdr_struct_type;


/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/
boolean rf_create_msgr_client_add_rex_queue(rf_task_num_type task_num, rex_tcb_type *p_rf_tcb,rex_sigs_type sig);
boolean rf_rex_msgr_interface_init(rf_task_num_type task_num, rex_tcb_type *p_rf_tcb,rex_sigs_type sig);
boolean rf_rex_msgr_interface_deinit(rf_task_num_type task_num);
boolean rf_rex_task_msgr_register( rf_task_num_type task_num);

#endif /* RF_CMD_INTERFACE_H */
/*! @} */

