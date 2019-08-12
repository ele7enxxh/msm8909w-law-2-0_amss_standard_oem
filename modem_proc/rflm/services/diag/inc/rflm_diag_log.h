/*!
  @file
  rflm_diag_log.h

  @brief
  RFLM background diagnostic packet logging.

  @detail
  See rflm_diag_log.cc for details.
*/
/*===========================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/diag/inc/rflm_diag_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2015-02-12   jc      Added API to flush log packet
2014-06-23   hdz     Added rflm_diag_validate_context()         
===========================================================================*/

#ifndef RFLM_DIAG_LOG_H
#define RFLM_DIAG_LOG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rflm.h"
#include "rflm_diag.h"

//#include "log_codes.h"


/***************************************************************************/
/*                   Top level FW_CRM enum/struct                             */
/***************************************************************************/


/*===========================================================================

                    DEFINES AND ENUMS

===========================================================================*/

#define RFLM_LOG_NUM_CONTEXT            7
                                        // # of packet types supported 
                                        // simultaneously (i.e., contexts).

#define RFLM_LOG_NUM_BUFFERS            2
                                        // logging ring buffer size 
                                        // (2 => ping-pong buffer).

#define RFLM_DIAG_LOG_INVALID_C_ID      -1
                                                                                                                        
#define RFLM_DIAG_LOG_MAX_PACKET_SIZE   (4096)

// buffer size in 32-bit words (buffer size + header size <= 4096).
// 32 shoould be enough space for packet headers and other overheads.
#define RFLM_LOG_MAX_BUF_SIZE_WORDS     (RFLM_DIAG_LOG_MAX_PACKET_SIZE/4-32)

#define RFLM_LOG_THREAD_STACK_SZ        RFLM_DEFAULT_STACK_SZ

#define RFLM_LOG_SIG_SHUTDOWN_BIT       RFLM_LOG_NUM_CONTEXT

#define RFLM_LOG_SIG_LAST               RFLM_LOG_SIG_SHUTDOWN_BIT
                                        // Last Signal.

// Max # of signals & the corresponding signal mask
#define RFLM_LOG_MAX_SIGNALS           (RFLM_LOG_SIG_LAST + 1)
#define RFLM_LOG_SIG_MASK              (BITL(RFLM_LOG_MAX_SIGNALS) - 1)


// Function return values
typedef enum {
    RFLM_LOG_BUF_OVERRUN        = -3,          // Buffer overflow detected
    RFLM_LOG_PACK_SIZE_OVERFLOW = -1,          // Unable to allocate memory
    RFLM_SUCCESS                = 0,           // Success
    RFLM_LOG_NEW_PACKET         = 1            // New packet started
} rflm_diag_log_return_code_t;

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/


/*! @brief Logging packet subheader. 
*/
typedef struct ALIGN(8)
{
  uint16 code;      /*!< AMSS log code */ 
  uint16 length;    /*!< Length of subpacket, not including subheader */
  uint32 reserved;  /*!< Pad to 8-bytes. Implied by alignment, but make sure. */

} rflm_diag_log_sub_header_t;


/*! 
  @brief 
  Log buffer with header and payload
*/
typedef struct
{
    rflm_diag_log_packet_header_t   diagHeader;     // Header for FW Diag packet
    
    rflm_diag_log_sub_header_t      subHeader;      // for AMSS log packet
        
    uint32 data[RFLM_LOG_MAX_BUF_SIZE_WORDS];       // Payload: timestamp + data
    
} rflm_diag_log_packet_buffer_t;


/*! 
  @brief Diag Log Context.
  
  A logging context. This structure has the ping-pong log buffer and other 
  "contextual" information.

*/
typedef struct
{
    rflm_diag_log_packet_buffer_t Buffer[RFLM_LOG_NUM_BUFFERS];
    uint16 userTagSize;           //size of user tag in the packet
    uint32 *writePtr;
    uint16 bufferSizeUsed;
    uint16 writeBufferId;         // The buffer number being written into
    uint16 readBufferId;          // The buffer number being read
    uint16 diagPacketId;
    uint16 lastSubmit;            // set to 1 when deallocating context
    uint16 rat;                   // Technology using the log context

    /*!< this flag gets set when the context is allocated, and it cleared
     * once we send CFW_LOG_NEW_PACKET indication for the very first time */
    boolean firstTime;         

} rflm_diag_log_context_t;


/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


extern void rflm_diag_log_init(void);

extern rflm_diag_log_return_code_t rflm_diag_log_alloc(uint32 size_bytes, uint32 context_id, uint32 **p_buffer_user);

extern rflm_diag_log_return_code_t rflm_diag_log_flush( uint32 context_id );

extern void rflm_diag_deallocate_context(int context_id);

extern int rflm_diag_allocate_context(uint16 diag_packet_id, 
                                      uint16 rat, 
                                      uint16 user_tag_size);

extern void rflm_diag_log_release(uint16 rat);

extern boolean rflm_diag_validate_context(int context_id);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // rflm_diag_log.h
