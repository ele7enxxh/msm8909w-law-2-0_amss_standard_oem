#ifndef QPD_H
#define QPD_H
/*=============================================================================

                 qpd.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qpd API

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


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qube/qpd.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/
#include <qtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/*=============================================================================
                      TYPEDEFS                                
=============================================================================*/

typedef void * qpd_t;

typedef uint8_t qprocessor_t;

/*=============================================================================
                      FUNCTION DECLARATIONS                                
=============================================================================*/
/**
 * Get current PD
 *
 * @return Current PD handle
 */
static inline qpd_t qpd_myself(void) { return NULL; }

/**
 * Get processor identifier from qpd_t
 *  
 * @return  processor identifier from qpd_t
 */
// qprocessor_t qpd_myprocessor( qpd_t my_pd );

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QPD_H */
