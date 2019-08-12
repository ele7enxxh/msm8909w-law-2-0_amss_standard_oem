#ifndef QMALLOC_H
#define QMALLOC_H
/*===========================================================================
                              <qmalloc.h>
DESCRIPTION
  Externalised functions for the qmalloc library

  Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/qmalloc.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/05   TJH	   Created
===========================================================================*/
 

/*===========================================================================
                 Includes and Public Data Declarations
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
  Constant / Define Declarations
===========================================================================*/

/*===========================================================================
  Type Declarations
===========================================================================*/

/*===========================================================================
  Global Constant Data Declarations 
===========================================================================*/

/*===========================================================================
                            Macro Definitions
===========================================================================*/

/*===========================================================================
                          Function Declarations
===========================================================================*/

#ifdef __cplusplus /* If this is reused in a C++ environment */
  extern "C" {
#endif

/*===========================================================================
FUNCTION Q_MALLOC

DESCRIPTION
  Allocates memory from the local heap if AMSS is not running, or the BREW
  heap once AMSS is started.

DEPENDENCIES
  None

RETURN VALUE
  Either a pointer to the area of memory allocated or NULL is no memory
  area is available.

SIDE EFFECTS
  None

===========================================================================*/
void *q_malloc (uint32 size);

/*===========================================================================
FUNCTION Q_FREE

DESCRIPTION
  Free memory from the Brew heap. Any memory allocated from the q_heap will
  be left allocated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void q_free (void *ptr);
      
#ifdef __cplusplus /* If this is reused in a C++ environment */
  }
#endif

#endif /* Q_MALLOC_H */
