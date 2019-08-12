#ifndef ETM_H
#define ETM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               ETM Driver

DESCRIPTION
This driver works for Coresight ARM9 and ETM architecture v3.2

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/api/kernel/arch/cortexm3/etm.h#1 $    

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/03   sho     Created.
===========================================================================*/


/*===========================================================================

                       FUNCTION PROTOTYPES

===========================================================================*/

/*
 * @brief Enable ETM instruction tracing
 *
 * @param None
 *
 * @return None
 *
 * @dependencies
 * QDSSInit is already called.
 */
void ETM_trace_enable(void);

/*
 * @brief Disble ETM instruction tracing
 *
 * @param None
 *
 * @return None
 */

void ETM_trace_disable(void);

/*
 * @brief Resume ETM state after low power mode
 *
 * @param None
 *
 * @return None
 */
void ETM_restore_state(void);

/*
 * @brief Save ETM state before low power mode
 *
 * @param None
 *
 * @return None
 */
void ETM_save_state(void);

#ifdef __cplusplus
}
#endif
#endif /* ETM_H */
