#ifndef QINTERRUPT_H
#define QINTERRUPT_H
/*=============================================================================

                 qinterrupth -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qinterrupt API  

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


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qube/qinterrupt.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
                        CONSTANTS AND MACROS DECLARATIONS
=============================================================================*/
/** 
l1_int_num shall NOT be zero: NO L2 interrupt controller connects to L1 interrupt 0
*/
#define L2_INTERRUPT_NUM(l1_int_num, l2_int_num) (l1_int_num*32+l2_int_num)

/*=============================================================================
                        FUNCTION DECLARATIONS
=============================================================================*/

/**
 * Register for an interrupt.  Register number 2-9(inclusive) are 
 * reserved for rtos usage. 
 *
 * @param int_num  Interrupt number
 *
 * @return         EOK if successful
 * @return         EVAL if int_num is not valid interrupt or
 *                  cb is NULL or callback function is NULL
 * @return         EMEM if out of memory allocating internal data
 *                  structure 
 */
int qinterrupt_register(int int_num);

/**
 * Deregister for a interrupt.
 *
 * @param int_num  Interrupt number
 *
 * @return         EOK if successful
 * @return         EVAL if int_num is not valid interrupt number less
 *                  than 31 or reserved by rtos 
 *
 * @return         EINVALID if not a registered interrupt
 */
int qinterrupt_deregister(int int_num);

/**
 * Receive an interrupt.
 *
 * @param int_num  Interrupt number
 *
 * @return         EOK if successful
 * @return         EVAL if int_num is not valid interrupt number less
 *                  than 31 or reserved by rtos 
 * @return         ENO_INTERRUPTS if the calling thread is not registering
 *                  for any interrupts 
 */

int qinterrupt_receive (unsigned int *int_num);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QINTERRUPT_H */
