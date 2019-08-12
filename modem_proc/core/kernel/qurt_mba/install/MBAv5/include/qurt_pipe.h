#ifndef QURT_PIPE_H
#define QURT_PIPE_H
/*=============================================================================

                 qurt_pipe.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of pipe interface API  
	 This is a pipe or message queue
	 It will block if too full (send) or empty (receive)
	 Unless you use a nonblocking option
	 All datagrams are 64 bits.

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


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_pipe.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/
#include <qurt_mutex.h>
#include <qurt_sem.h>

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/
#define QURT_PIPE_MAGIC  0xF1FEF1FE


/*=============================================================================
                        TYPEDEFS
=============================================================================*/
typedef unsigned long long int qurt_pipe_data_t;

typedef struct {
	qurt_mutex_t pipe_lock;
	qurt_sem_t senders;
	qurt_sem_t receiver;
	unsigned int size;
	unsigned int sendidx;
	unsigned int recvidx;
	unsigned int magic;
	qurt_pipe_data_t *data;
} qurt_pipe_t;

/**
 * Pipe attributes
 */
typedef struct {
  qurt_pipe_data_t *buffer;
  unsigned int elements;
} qurt_pipe_attr_t;
/*=============================================================================
                        FUNCTIONS
=============================================================================*/
static inline void qurt_pipe_attr_init(qurt_pipe_attr_t *attr)
{
  attr->buffer = 0;
  attr->elements = 0;
}

static inline void qurt_pipe_attr_set_buffer(qurt_pipe_attr_t *attr, qurt_pipe_data_t *buffer)
{
  attr->buffer = buffer;
}

static inline void qurt_pipe_attr_set_elements(qurt_pipe_attr_t *attr, unsigned int elements)
{
  attr->elements = elements;
}

/**
 * Create a QuRT pipe object with the data buffer
 *
 * @param pipe   Pipe
 * @param attr   Pointer to the pipe attributes
 * 
 * @output Creating a Pipe object and allocating the pipe data buffer
 * 
 * @return Success/Fail
 */
int qurt_pipe_create(qurt_pipe_t **pipe, qurt_pipe_attr_t *attr);

/**
 * Initializes a QuRT pipe object
 *
 * @param pipe Pipe
 * @param attr Pointer to the pipe attributes
 *
 * @output Creating a Pipe object with the user passed data buffer
 *
 * @return Success/Fail
 */
int qurt_pipe_init(qurt_pipe_t *pipe, qurt_pipe_attr_t *attr);

/**
 * Destroy QuRT pipe object
 *
 * @param pipe Pipe
 *
 * @output Free pipe object
 * 
 * @return void
 */
void qurt_pipe_destroy(qurt_pipe_t *pipe); 

/**
 * Delete QuRT pipe object
 *
 * @param pipe Pipe
 *
 * @output Free pipe object along with the data buffer
 *
 * @return void
 */
void qurt_pipe_delete(qurt_pipe_t *pipe);

void qurt_pipe_send(qurt_pipe_t *pipe, qurt_pipe_data_t data);
qurt_pipe_data_t qurt_pipe_receive(qurt_pipe_t *pipe);

int qurt_pipe_try_send(qurt_pipe_t *pipe, qurt_pipe_data_t data);
qurt_pipe_data_t qurt_pipe_try_receive(qurt_pipe_t *pipe, int *success);

#endif  /* QURT_PIPE_H */

