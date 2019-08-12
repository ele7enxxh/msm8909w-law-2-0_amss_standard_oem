#ifndef QUBE_H
#define QUBE_H
/*=============================================================================

                 qube.h -- H E A D E R  F I L E

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


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qube/qube.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include <qurt.h>
#include <stdio.h>
#include <stdlib.h>
#include <qmutex.h>





typedef struct _qmsgq_node *qmsgq_t;
typedef qmsgq_t *qch_t;
typedef struct 
{
    unsigned int        magic;
    size_t              max_msg_size; 
    void                *buf;
    unsigned int        buf_size;
    qch_t               channel;
    uint8_t             priority;
}
qmsgq_attr_t;
struct _qmsgq_node {
  struct _qmsgq_node *next;
  qurt_pipe_t *qurtpipe;
  const char *name;
  qmsgq_attr_t attrs;	/**< just to support set/get */
  uint8_t prio;
};
typedef struct 
{
    qmsgq_t             msgq; 
    int                 msg_type;
    const void          *msg;
    size_t              msg_size;
}
qmsgq_callback_t;

#define INVALID_ADDR	       (~0UL)

/* Define Error codes as QuRT error codes preceed with QURT_ */
#ifndef EOK
#define EOK                             QURT_EOK
#endif /* EOK */
#ifndef EVAL
#define EVAL                            QURT_EVAL
#endif /* EVAL */
#ifndef EMEM
#define EMEM                            QURT_EMEM
#endif /* EMEM */
#ifndef EINVALID
#define EINVALID                        QURT_EINVALID
#endif /* EINVALID */
#ifndef EUNKNOWN
#define EUNKNOWN                        QURT_EUNKNOWN
#endif /* EUNKNOWN */
#ifndef EFAILED
#define EFAILED                         QURT_EFAILED
#endif /* EFAILED */
#ifndef ENOTALLOWED
#define ENOTALLOWED                     QURT_ENOTALLOWED
#endif /* ENOTALLOWED */
#ifndef EDUP_CLSID
#define EDUP_CLSID                      QURT_EDUPCLSID
#endif /* EDUP_CLSID */
#ifndef ENO_INTERRUPTS
#define ENO_INTERRUPTS                  QURT_ENOREGISTERED
#endif /* ENO_INTERRUPTS */


/*=============================================================================
                      FUNCTION DECLARATIONS                                
=============================================================================*/

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QUBE_H */
