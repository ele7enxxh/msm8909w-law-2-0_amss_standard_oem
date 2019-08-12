#ifndef DIAGBUF_V_H
#define DIAGBUF_V_H
/*==========================================================================

               Diagnostics Output Buffer Allocation Manager

Description
  Type and function definitions for the diag output allocation manager.

Copyright (c) 1999-2016 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                          Edit History

      $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagbuf_v.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/07/16   ph      Diag log packet support for ULog
04/24/14   tbg     Fixed issues with extended event, log and F3 pkts 
10/14/13   sr      Added support for extended event, log and F3 pkts   
09/20/13   tbg     Added support for Diag over STM 
01/03/13   sg      Command Response Channel separation
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush. 
01/05/11   mad     Changes for dual mask 
11/30/10   hm      Made diagbuf_alloc and diagbuf_commit private. Moved out of diagbuf.h
10/27/10   sg      Moved  DIAG_TX_SLEEP_THRESHOLD_DEFAULT, DIAG_TX_SLEEP_THRESHOLD_DEFAULT
                   to diag_cfg.h
10/26/10   vs      Changes for new encoding protocol
10/25/10   is      Diag feature flag cleanup
09/22/10   mad     Removed  diagbuf_tx_sleep_threshold_set(). Added 
                   diagbuf_ctrl_tx_sleep_parameters(). 
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
05/11/10   JV      Changing the default sleep time and sleep threshold values
                   for better performance on the aDSP. Brew targets get these
                   values from cust files and Sreelakshmi is going to add a
                   feature for WM.
03/02/10   JV      Moved DIAG_TX_SLEEP_TIME_DEFAULT and 
                   DIAG_TX_SLEEP_THRESHOLD_DEFAULT to diagbuf_v.h since these
                   are required even in diagdiag_common.c.
10/21/09   JV      Putting in a new feature so that we can now use the 
                   diagbuf_send_pkt_multi_proc() function to send data out
                   if we require the response to be prioritized.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/17/09   mad     Featurized includion of customer.h for WM.
07/10/09   as      Decoupled code FEATURE_DIAG_NON_STREAMING
06/23/09   JV      Introduced the OS abstraction layer
05/07/09   mad     Clean-up: moved DIAG_TX_SLEEP_TIME_DEFAULT and 
                   DIAG_TX_SLEEP_THRESHOLD_DEFAULT to diagbuf.c, since these
                   are required only in diagbuf.c
05/07/09   vk      moved declerations into diagbuf.h that were being referred 
                   from there
04/27/09   mad     Combined diagbuf_tx_sleep_time_set() and
                   diagbuf_tx_sleep_threshold_set() into one function,
                   diagbuf_tx_sleep_parameters_set().
12/12/08   vg      CMI split diagbuf.h and diagbuf_v.h
10/03/08   vg      Updated code to use PACK() vs. PACKED
06/09/08   mad     change to define default sleep time and threshold 
                   conditionally
02/05/08   pc      Added support for dynamic threshold and sleep values
12/15/06   as      Fixed compiler warnings.
10/31/05   as      Fixed lint errors.
06/27/01   lad     diagbuf_blocking_alloc(): moved 'code' to diagpkt_alloc().
02/23/01   lad     Moved constants to diagtune.h.  Cosmetic cleanup for
                   rearchitected diag release.
11/07/00   jal     Renamed DIAGBUF_DATA_AVAIL_S to DIAGBUF_READY_S
09/01/00   pj      Increased DIAGBUF_MAX_SIZE to 10KB. Such a large buffer is
                   considered a temporary measure until the GPS SRCH code can
                   be sped up (it is currently starving the diag task, thus
                   requiring a large buffer).
08/28/00   lad     Removed use of DIAGBUF_MAX_SIZE.
06/19/00   lad     Moved some macros and prototypes from diagi.h.
05/15/00   lad     Updated load balancing.
05/10/00   lad     Added DIAGBUF_NICE_SIZE for load balancing.  Updated API.
02/28/00   lad     Created file.

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#ifndef FEATURE_WINCE
#endif
#include "osal.h"
#include "diag.h"
#include "diag_v.h"
#include "diagbuf.h"
#include "diagdsmi.h"
#include "ULog.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the diag output allocation manager.

===========================================================================*/

typedef enum
{
  DIAGBUF_EMPTY_S,        /* Nothing in ring buffer or event buffer */
  DIAGBUF_READY_S,        /* Data from ring buffer queued to DSM, but ring
                             buffer still has more data to be queued--make */
                          /* sure diagbuf_drain() gets called again soon! */
  DIAGBUF_NOT_READY_S     /* Ring/event buffer has data to be queued to 
                             DSM, but we're blocked from queueing because
                             it's uncomitted, we don't have an SIO stream
                             yet, or we're flow-controlled by DSM. */
}
diagbuf_status_enum_type;

/* diagbuf_header_status_enum_type included here to STM_BUFFER type is visible */
/*lint -save -e{641} */
typedef enum
{
  DIAGBUF_UNCOMMIT_S,  /* Notify that the buffer is not commited    */
  DIAGBUF_COMMIT_S,    /* Notify that the buffer is commited        */
  DIAGBUF_WRAP_S,      /* Notify that the buffer has to be wrapped  */
  DIAGBUF_HOLE_S,      /* Notify that the buffer has a hole         */
  DIAGBUF_UNUSED_S,    /* Notify that the buffer is unused          */
  DIAGBUF_STM_BUFFER_S, /* STM Buffer, NOT allocated from diagbuf   */
} diagbuf_header_status_enum_type;
/*lint -restore */


/* The send descriptor type. */
typedef enum 
{
  DIAG_SEND_STATE_START,
  DIAG_SEND_STATE_BUSY,
  DIAG_SEND_STATE_CRC1,
  DIAG_SEND_STATE_CRC2,
  DIAG_SEND_STATE_TERM,
  DIAG_SEND_STATE_COMPLETE
}
diag_send_state_enum_type;

typedef enum {
  DIAG_STATE_START,
  DIAG_STATE_WRITE_FIRST_BYTE_LEN,
  DIAG_STATE_WRITE_SECOND_BYTE_LEN,
  DIAG_STATE_WRITE_PACKET,  
  DIAG_STATE_END
} diag_enhc_encode_state;

typedef struct
{
  const void *pkt;
  const void *last; /* Address of last byte to send. */
  diag_send_state_enum_type state;
  boolean terminate;                 /* True if this fragment 
                                      terminates the packet */
  boolean priority;                                 
  boolean is_cmd_rsp_type; /*set to notify whether it is command response or other data*/
}
diag_send_desc_type;


/* NOTE: First chained pkt payload type in diagbuf for version 1 matches diag_cmd_ext_v1_type*/

/*Diagbuf packets can be chained; the below enum indicates which 
  part of the chain is the current diagbuf pkt*/
typedef enum
{
  DIAGBUF_NO_CHAIN,
  DIAGBUF_FIRST_PKT_CHAIN,
  DIAGBUF_SECOND_PKT_CHAIN,
  DIAGBUF_LAST_PKT_CHAIN = DIAGBUF_SECOND_PKT_CHAIN
}diagbuf_chain_type;

/*===========================================================================

PACKET   DIAG_CMD_EXT_V2_TYPE
PURPOSE  Header used for other debug msg, log, and event packets to add 
         additional information

===========================================================================*/
typedef PACK(struct)
{
  uint8 cmd_code;       /* cmd code for extended packets */
  uint8 version;        /* version of the extended pkt; currently we support version 2 */
  uint16 proc_id;       /* processor ID where the msg is being generated */
  uint8 name_len;       /*  Length of Source DiagBuffer  */
  uint8 time_offset_type;
  uint64 time_offset;
  char ulog_name[ULOG_MAX_NAME_SIZE + 1];
}
diag_cmd_ext_v2_type;


#define DIAG_ULOG_NO_OFFSET         0xFF    /* No offset provided; custom time used by DiagBuffer*/
#define DIAG_ULOG_NO_CUSTOM_OFFSET  0x00    /*No offset provided; Synchronization not required */
#define DIAG_ULOG_SYNC_OFFSET_TYPE  0x01    /* Network time offset provided (8 bytes) */

/*Maps each version of the diagbuf packet to the length of the first packet in the chain*/
extern uint8 diagbuf_ver_len[DIAGBUF_MAX_VERSION_NUM];

extern boolean diag_hdlc_protocol;

/* Output buffer sent to SIO/SMD */
#define DIAGBUF_SIO_TX_ITEM_SIZ (DSMI_DIAG_SIO_TX_ITEM_SIZ)

/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
  extern "C" {
#endif

/*===========================================================================

FUNCTION TYPE DIAGBUF_OUTBOUND_PKT_HANDLER_TYPE

DESCRIPTION
  This function type, if registered with a command code, is called from
  DIAG task context when the queued outbound pakcet is ready to send.

  diagbuf_drain() automatically processes packets in raw format.  However,
  some packets, such as those used in the debug message service, require
  post-processing in DIAG task context prior to sending.

  This system requires that diagbuf_send_pkt is used to send the final
  packet to the comm layer.  This routine maintains a state, which requires
  that all outbound packets be sent via that function.

RETURN VALUE
  Number of bytes used, returned by diagbuf_send_pkt().

===========================================================================*/
typedef void (*diagbuf_outbound_pkt_handler_type) (diag_send_desc_type *desc,byte stream_id);

/*===========================================================================

FUNCTION DIAGBUF_INIT

DESCRIPTION
  This function initializes the diag allocation buffer at startup.

============================================================================*/
void
diagbuf_init (void);


/*===========================================================================
FUNCTION DIAGBUF_ALLOC

DESCRIPTION
 This function allocates the specified amount of space in the diag output
 buffer.


code            	Specifying the command code.
length          	Length requested.
nice            	Padding value
diagbuf_alloc_request 	Type of service requesting buffer space.


DEPENDENCIES

diagbuf_commit() must be called to commit a buffer to the diag pool.  This
must happen reasonably soon after this call completes.  If the buffer is
needed to accumulate data over time, then you should use your own buffer
use this just long enough to copy the data.

SIDEEFFECTS

diagbuf_head may be updated.

RETURN VALUES

Pointer to allocated buffer if successful
NULL if space is not available.

============================================================================*/
PACK (void *)
diagbuf_alloc (uint8 code, uint32 length, int nice, uint8 diagbuf_alloc_request, uint8 chain, uint8 version);

/*===========================================================================
FUNCTION DIAGBUF_ALLOC_CHAIN

DESCRIPTION
    Allocates chained(consecutive) packets in Diag buffer.    
    This function should be called only for extended log pkts
 
PARAMETERS
   length                    Length of requested buffer 
   nice                      Niceness padding value     
   diagbuf_alloc_request     Required for DIAG Health 
   chain                     0 - not chained
                             1 - first pkt in the chain
                             2 - second pkt in the chain
   version                   0 - non extended pkts
                             1 - extended pkts
   id                        Passed in by diag client while
                             calling log APIs and F3 macros

DEPENDENCIES
  This function must only be called from within this file by externalized
  alloc functions.

  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.
 
  diagbuf_commit() should be called seperately for the two chained pkts in
  diagbuf.

RETURN VALUE 
  Pointer to the allocated space in diagbuf 
  NULL if there is no space available in diagbuf

===========================================================================*/
PACK (void *) diagbuf_alloc_chain(
   uint8 code,
   uint32 length, 
   int nice,    
   uint8 diagbuf_alloc_request,   
   uint8 chain, 
   uint8 version,
   void* parameter,
   byte stream_id );


/*===========================================================================
 FUNCTION DIAGBUF_COMMIT

 DESCRITPION
  This function commits previously allocated space in the diag output buffer.
  ptr' must be the address that was returned by a prior call to
  diagbuf_alloc().

  This function signals the diag task and may cause a context switch.

 PARAMETERS
  *ptr             Specifying the ptr to be commited 
 DEPENDENCIES
 None
 SIDEEFFECTS
 None
 RETURN VALUES
 None

=============================================================================*/

void
diagbuf_commit (PACK(void *) ptr);



/*===========================================================================

FUNCTION DIAGBUF_DRAIN

DESCRIPTION
  This function drains the diag allocation buffer to the comm layer.
  Escaping, etc is performed in this routine.

  This function may only be called by the diag task.  It is not re-entrant!
  
  This is considered a background process for the DIAG task.  Periodically,
  this routine checks the DIAG task sigs against 'mask'.  If any bits match,
  the routine exits promptly.
  
RETURN VALUE
  READY if more data can be sent
  NOT_READY if unable to proceed (uncommitted packet, flow control)
  EMPTY nothing to do

===========================================================================*/
diagbuf_status_enum_type
diagbuf_drain (osal_sigs_t mask);



/*===========================================================================

FUNCTION DIAGBUF_SEND_PKT

DESCRIPTION
  Sends a packet, or a fragment of a packet, described by 'desc', to the
  comm layer.
  
  This handles buffer management with the comm layer and all transport layer
  related ops, like CRC calculation HDLC encoding.

===========================================================================*/
diagbuf_status_enum_type
diagbuf_send_pkt (diag_send_desc_type *desc,byte stream_id);


/*===========================================================================

FUNCTION diagbuf_ctrl_tx_sleep_parameters

DESCRIPTION
Control sleep threshold and sleep-time

===========================================================================*/
void diagbuf_ctrl_tx_sleep_parameters (unsigned int threshold_val, unsigned int sleep_val);

/*===========================================================================
FUNCTION diagbuf_ctrl_commit_threshold
DESCRIPTION
Control diagbuf_commit_threshold
===========================================================================*/
void diagbuf_ctrl_commit_threshold(unsigned int val);


/*===========================================================================

FUNCTION diagbuf_tx_sleep_parameters_set

DESCRIPTION
Called during diag initialization.
This initializes the value of diagbuf_tx_sleep_threshold and 
diagbuf_tx_sleep_time, after reading NV(AMSS)/Registry(WM).

===========================================================================*/
void diagbuf_tx_sleep_parameters_set (void);
/*===========================================================================

FUNCTION DIAGBUF_REGISTER_OUTBOUND_PKT_HANDLER

DESCRIPTION
  This function registers a function pointer to handle the sending of a
  packet.  diagbuf_drain() sends packets in raw format.  If processing/
  formatting needs to occur in DIAG task context prior to sending the
  packet, one can register a function pointer with the command code, which
  will be called when diagbuf_drain finds a packet with that command code.

  This system requires that diagbuf_send_pkt is used to send the final
  packet to the comm layer.  This routine maintains a state, which requires
  that all outbound packets be sent via that function.

RETURN VALUE
  TRUE if registration was successful.

===========================================================================*/
boolean
diagbuf_register_outbound_pkt_handler (uint8 cmd_code, 
                                       diagbuf_outbound_pkt_handler_type fp);

/*===========================================================================
FUNCTION DIAGBUF_RESET_DSM_CHAINED_COUNT

DESCRIPTION
    Resets the internal DSM chaining counts for Diag Health.
    
===========================================================================*/
void diagbuf_reset_dsm_chained_count( void );

/*===========================================================================
FUNCTION DIAGBUF_GET_DSM_CHAINED_COUNT

DESCRIPTION
    Returns the internal variable diagbuf_dsm_chained_count for Diag Health.
    
===========================================================================*/
uint32 diagbuf_get_dsm_chained_count( void );

/*===========================================================================
FUNCTION DIAGBUF_GET_DSM_COUNT

DESCRIPTION
    Returns the internal variable diagbuf_dsm_count for Diag Health.
    
===========================================================================*/
uint32 diagbuf_get_dsm_count( void );

/*===========================================================================
FUNCTION DIAGBUF_INCR_DSM_CHAINED_COUNT

DESCRIPTION
    Increments diagbuf_dsm_chained_count for Diag Health.
    
===========================================================================*/
void diagbuf_incr_dsm_chained_count( void );

/*===========================================================================
FUNCTION DIAGBUF_INCR_DSM_COUNT

DESCRIPTION
    Increments diagbuf_dsm_count for Diag Health.
    
===========================================================================*/
void diagbuf_incr_dsm_count( void );

/*===========================================================================
FUNCTION DIAGBUF_WRITE_STREAM_ID

DESCRIPTION
   Writes Stream Id to the packet in diagbuf
    
===========================================================================*/
void diagbuf_write_stream_id (PACK(void *)ptr,byte stream_id);

/*===========================================================================
FUNCTION DIAGBUF_INCR_DSM_COUNT

DESCRIPTION
    Reads Stream Id from the packet in diagbuf
    
===========================================================================*/
byte diagbuf_read_stream_id (const PACK(void *)ptr);

/*===========================================================================

FUNCTION DIAGBUF_FLUSH_TIMEOUT

DESCRIPTION
  This function configures all streaming data items off.  No more allocations
  will occur until these settings are changed. It performs the same operation
  as diagbuf_flush only with optional timeout value.

  Diag will not stop collecting data immediately.  The data currently
  allocated will still be flushed.
 
  diag will block after flushing the data for specified timeout duration or
  time taken by SIO to flush the data(which ever is smaller).
 
  diag_flush_timeout flag will be set to TRUE if diag times out on SIO callback
 
@param[in] timeoutVal    Specifies the timeout duration. If specified as 0, 
timeout will not occur. 

===========================================================================*/
void diagbuf_flush_timeout(unsigned int timeoutVal);


#ifdef __cplusplus
  }
#endif
#endif /*DIAGBUF_V_H*/
