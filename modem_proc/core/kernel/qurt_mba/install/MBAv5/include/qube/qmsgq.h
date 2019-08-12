#ifndef QMSGQ_H
#define QMSGQ_H
/*=============================================================================

                 qmmsgq.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qmsgq API  

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


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qube/qmsgq.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/
#ifdef __cplusplus
	extern "C" {
#endif
#include <stdlib.h>
#include <string.h>
#include <qurt_error.h>

/*=============================================================================
                        CONSTANTS AND MACROS DECLARATIONS
=============================================================================*/

/**
 * qchannel mode related defines                                            
 */
typedef enum
{ 
    QCHANNEL_BLOCK = 0,
    QCHANNEL_NONBLOCK
} qch_mode;


/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/**
 * qmsgq_t types                                                            
 */
typedef struct _qmsgq_node *qmsgq_t;

/**
 * qch_t types                                                              
 */
typedef qmsgq_t *qch_t;

/**
 * qmsgq_attr_t types                                                      
 */
typedef struct 
{
    /**
     * Magic number to verify qmsgq_attr_t pointer 
     */
    unsigned int        magic;
    
    /**
     * This attribute specifies the maximum message size acceptable in queue. 
     * Sending messages larger than this value will result in error. 
     */
    size_t              max_msg_size; 

    /**
     * This attribute specifies the buffer pointer 
     */
    void                *buf;

    /**
     * This attribute specifies the buffer size 
     */
    unsigned int        buf_size;

    /**
     * This attribute specifies the channel associated with message queue 
     */
    qch_t               channel;
    
    /**
     * This attribute specifies the priority of the message queue 
     */
    uint8_t             priority;

}
qmsgq_attr_t;

typedef union {
  unsigned long long int raw;
  struct {
    void *msgptr;
    unsigned int len;
  };
} _msg_info_t;

typedef union {
  unsigned long long int raw;
  struct {
    unsigned char all_fs;
    unsigned char idx;
    unsigned char len;
    signed char id;
    int msgtype;
  };
} _fast_msg_info_t;

typedef union {
  unsigned long long int raw;
  struct {
    int msgtype;
    unsigned int id;
    unsigned int obj;
  };
} _msg_header_t;

struct _qmsgq_node {
  struct _qmsgq_node *next;
  qurt_pipe_t *qurtpipe;
  const char *name;
  qmsgq_attr_t attrs;	/**< just to support set/get */
  uint8_t prio;
};

/**
 * qmsgq_callback_t structure types                                        
 */
typedef struct 
{
    /**
     * This field specifies the message queue to which the message will
     * be delivered 
     */
    qmsgq_t             msgq; 

    /**
     * This field specifies the message type 
     */
    int                 msg_type;

    /**
     * This field specifies the pointer to the message 
     */
    const void          *msg;

    /**
     * This field specifies the pointer to the message 
     */
    size_t              msg_size;
}
qmsgq_callback_t;

/*=============================================================================
                      FUNCTION DECLARATIONS   
=============================================================================*/
                      
/*=============================================================================*/
/**
 * Creates a message queue with specific attributes
 *
 * The qmsgq_create is used to create to a message queue. The name is used
 * to uniquely identify a message queue. The attributes that should be
 * specified when a message queues is created are priority of the message
 * queue, buffer pointer where messages are stored and maximum size of
 * any message that can be sent to the message queue.
 *
 * @param name        Name of the message queue
 * @param msgq  [OUT] Message queue object
 * @param attr        Attributes object
 * @return            QURT_EOK:       Successful create,
 * @return            QURT_EMEM:      Out of memory error,
 * @return            QURT_EFAILED:   IPC related failures,
 * @return            QURT_EINVALID:  Wrong channel, attribute pointer, etc,
 * @return            QURT_EVAL:      Wrong values for parameters like 
 *                               null value for queue name, channel, buffer, size, etc.
 * @return            QURT_EDUPMSGQ: Duplicate message queue name.
 */
int qmsgq_create(const char *name, qmsgq_t *msgq, qmsgq_attr_t *attr);

/**
 * Attach to a message queue
 *
 * The msgq_attach is used to attach to an already created message queue.
 *
 * @param name        Name of the message queue
 * @param msgq  [OUT] Message queue object
 * @return            QURT_EOK:       Successful Attach,
 * @return            QURT_EMEM:      Out of memory error,
 * @return            QURT_EFAILED:   IPC related failures,
 * @return            QURT_EINVALID:  Wrong channel, attribute pointer, etc,
 * @return            QURT_EVAL:      Wrong values for parameters like 
 *                               null value for queue name, 
 * @return            QURT_ENOMSGQ:  Message Queue doesn't exist.
 */
int qmsgq_attach(const char *name, qmsgq_t *msgq);

/**
 * Initialize attribute object with default values.
 *
 * The default values are 0 for message queue priority and
 * 4 for maximum message size. There are no default values for
 * channel number, message queue buffer address and length
 *
 * @param attr  Attributes object
 */
static inline void qmsgq_attr_init(qmsgq_attr_t* attr) { memset(attr,0,sizeof(attr));} 

/**
 * Set channel used to receive messages from message queue
 *
 * @param attr  Attribute object
 * @param ch    Channel object
 */
static inline void qmsgq_attr_setchannel(qmsgq_attr_t* attr, qch_t ch) { attr->channel = ch; }

/**
 * Get channel used to receive messages from message queue
 *
 * @param attr        Attribute object
 * @param ch   [OUT]  Channel object
 */
static inline void qmsgq_attr_getchannel(qmsgq_attr_t* attr, qch_t *ch) { *ch = attr->channel; }

/**
 * Set priority of a message queue
 *
 * @param attr      Attribute object
 * @param priority  Priority of the message queue
 */
static inline void qmsgq_attr_setpriority(qmsgq_attr_t* attr, uint8_t priority) {attr->priority = priority; }

/**
 * Get priority of a message queue
 *
 * @param attr             Attribute object
 * @param priority  [OUT]  Priority of the message queue
 */
static inline void qmsgq_attr_getpriority(qmsgq_attr_t* attr, uint8_t *priority) { *priority = attr->priority; }

/**
 * Setup buffer space used for a message queue
 *
 * The size of the buffer along with maximum message size decide how many
 * messages can be held in a message queue at any point of time.
 *
 * @param attr  Attribute object
 * @param buf   Buffer pointer
 * @param size  Buffer size
 */
static inline void qmsgq_attr_setbuffer(qmsgq_attr_t* attr, void *buf, size_t size) { attr->buf = buf; attr->buf_size = size; }

/**
 * Get details about buffer used for a message queue
 *
 * @param attr         Attribute object
 * @param buf   [OUT]  Buffer pointer
 * @param size  [OUT]  Buffer size
 */
static inline void qmsgq_attr_getbuffer(qmsgq_attr_t* attr, void **buf, size_t *size) { *buf = attr->buf; *size = attr->buf_size; }

/**
 * Set maximum size of any message in a message queue
 *
 * @param attr  Message queue attributes object
 * @param size  Max. size of any message. This should be >= 0
 */
static inline void qmsgq_attr_setmaxmsgsize(qmsgq_attr_t *attr, size_t size) { attr->max_msg_size = size; }

/**
 * Get maximum allowed size of any message in a message queue
 *
 * @param attr  Message queue attributes object
 * @param size  Max. allowed size of any message
 */
static inline void qmsgq_attr_getmaxmsgsize(qmsgq_attr_t *attr, size_t *size) { *size = attr->max_msg_size; }

/**
 * Get attributes of a message queue
 *
 * @param msgq         Message queue object
 * @param attr  [OUT]  Message queue attributes
 * @return             QURT_EOK:       get_attr successful,
 * @return             QURT_EFAILED:   IPC related failures,
 * @return             QURT_EINVALID:  Wrong parameters,
 */
static inline int qmsgq_get_attr(qmsgq_t msgq, qmsgq_attr_t *attr) { memcpy(attr,&msgq->attrs,sizeof(*attr)); return QURT_EOK; }

/**
 * Deletes a message queue
 *
 * @param msgq  Message queue object
 * @return      QURT_EOK:       delete successful,
 * @return      QURT_EFAILED:   IPC related failures,
 * @return      QURT_EINVALID:  Wrong parameters.
 */
int qmsgq_delete(qmsgq_t msgq);

/**
 * Send message to a message queue
 *
 * The qmsgq_send is used to send a message asynchronously to a message queue.
 * The msgq specifies the queue where message should be added. The msgtype, size
 * and data specify the message type, size in bytes and actual data respectively.
 *
 * @param msgq      Message queue object
 * @param msgtype   Message type
 * @param data      Pointer to message data
 * @param size      Size of the message
 * @param obj       Queue object
 *
 * @return          QURT_EOK:       Successful Send,
 * @return          QURT_EMEM:      Out of memory error,
 * @return          QURT_EFAILED:   IPC related failures,
 * @return          QURT_EINVALID:  Wrong parameters,
 * @return          QURT_EMAXMSGS: Message queue is full
 */
int qmsgq_send_fast(qmsgq_t msgq, int msgtype, const void *data, size_t size, qobject_t obj);
int qmsgq_send_slow(qmsgq_t msgq, int msgtype, const void *data, size_t size, qobject_t obj);

static inline int qmsgq_send(qmsgq_t msgq, int msgtype, const void *data, size_t size, qobject_t obj, qperm_t permission __attribute__((unused)))
{
	if (size <= 32) return qmsgq_send_fast(msgq,msgtype,data,size,obj);
	else return qmsgq_send_slow(msgq,msgtype,data,size,obj);
}

/**
 * Free a message
 *
 * The qmsgq_free_message is called when a thread is finished processing a message
 * to release memory used by the message
 *
 * @param msg  Pointer to the message
 * @return     QURT_EOK:       Successful Send,
 * @return     QURT_EINVALID:  Wrong message pointer, message from 
 *                        deleted message queue, etc 
 */
int qmsgq_free_message(const void *msg);

/**
 * Initializes a callback
 *
 * @param cb       [OUT] Callback object
 * @param msgq           Message queue object
 * @param msgtype        Type of the message
 * @param msg            Message data
 * @param size           Size of the message
 * @return               QURT_EOK:       Operation is successful,
 * @return               QURT_EMEM:      Out of memory error,
 * @return               QURT_EFAILED:   IPC related failures,
 * @return               QURT_EINVALID:  Wrong channel, attribute pointer, etc,
 * @return               QURT_EVAL:      Wrong values for parameters like 
 *                                  null value for queue name, 
 * @return               QURT_ENOMSGQ:  Message Queue doesn't exist.
 */
int qmsgq_callback_init(qmsgq_callback_t *cb, qmsgq_t msgq, int msgtype, const void *msg, size_t size);

/**
 * Complete message send operation using message callback
 * Note: This function is executed from message queue server pd.
 *
 * The qmsgq_send is used to send a message asynchronously to a message queue.
 * The node identifies the queue where message should be added. 
 * 
 * @param token     Pointer to the message node structure created using qmsgq_lib_allocate_callback.
 * @return          QURT_EOK if send is successful, error code otherwise
 */
int qmsgq_lib_process_callback ( unsigned int token );
 
/**
 * Allocates a message node structure and copies data from callback
 * 
 * Note: This function is executed from message queue server pd.
 *
 * @param cb          Pointer to the qmsgq_callback_t.
 * @param token [out] Pointer to the message node structure.
 * @return            QURT_EOK if send is successful, error code otherwise
 */
int qmsgq_lib_allocate_callback ( qmsgq_callback_t* cb, unsigned int *token );

/**
 * This function will free the message structure allocated for callback
 * Note: This function is executed from message queue server pd.
 *
 * @param token     Pointer to the message node structure created using qmsgq_lib_allocate_callback.
 * @return          QURT_EOK if send is successful, error code otherwise
 */
int qmsgq_lib_free_callback ( unsigned int token );

/**
 * Create a channel
 *
 * @param  ch  [OUT] Channel object
 * @return           QURT_EOK:       Operation is successful,
 * @return           QURT_EMEM:      Out of memory error,
 */
int qchannel_create(qch_t *ch);

/**
 * Delete a channel
 *
 * @param   ch  [OUT] Channel object
 * @return           QURT_EOK:       Operation is successful,
 * @return           QURT_EINVALID:  Wrong parameters.
 */
int qchannel_delete(qch_t ch);


/**
 * Receive message from one or more message queues conneted to a channel
 *
 * The qchannel_receive is used to get a message from one or more message
 * queues connected to a channel. If more than one message queue has a
 * message, then message from highest priority message queue connected to
 * the channel is given to the receiver. If there are no messages in any of the
 * message queues, then the caller is blocked until a message arrives.
 * 
 * If more than one thread is blocked in qchannel_receive waiting for a message,
 * when a message arrives, the highest priority waiting thread will be awakened.
 *
 * If mode is specified as QCHANNEL_NONBLOCK, then the caller thread is NOT
 * blocked if there are no messages in any of the message queues conneted to
 * the channel
 *
 * @param ch             Channel object
 * @param msgtype  [OUT] Type of the message received
 * @param data     [OUT] Location where message is be stored
 * @param msgsize  [OUT] Size of the message
 * @param obj            Queue object
 *
 * @return               QURT_EOK:       Creation is successful,
 * @return               QURT_EVAL:      Wrong value for parameters like
 *                                  null data pointer, mode != (BLOCK or NONBLOCK)
 * @return               QURT_EINVALID:  Wrong parameters,
 * @return               QURT_ENOMSGS:  No more messages in the queue
 */
int qchannel_recv(qch_t ch, int *msgtype, void **data, size_t *msgsize, qobject_t *obj);
int qchannel_tryrecv(qch_t ch, int *msgtype, void **data, size_t *msgsize, qobject_t *obj);

/* Hopefully this optimizes away into a single correct function call */
/* param mode 					Specifies whether the call should block or not when
 * 											there are no messages in any of the message queues 
 */
static inline int qchannel_receive(qch_t ch, qch_mode mode, int *msgtype, void **data, size_t *msgsize, qobject_t *obj) { 
	// won't optimize so good: if ((msgtype == NULL) || (data == NULL) || (msgsize == NULL)) return EVAL;
	switch (mode) {
		case QCHANNEL_BLOCK: return qchannel_recv(ch,msgtype,data,msgsize, obj);
		case QCHANNEL_NONBLOCK: return qchannel_tryrecv(ch,msgtype,data,msgsize, obj);
		default: return QURT_EVAL;
	}
}

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QMSGQ_H */
