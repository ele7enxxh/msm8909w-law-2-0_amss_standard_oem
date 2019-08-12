/*!
  @file
  rfca.h
 
  @brief
  Defines RFCA diag and inter task communication functions.

*/

/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/27/14   aki     Added sleep functions
06/10/13   aki     Mainline RFCA 
10/02/12   aki     Remove RFCA_diag_alloc 
05/17/12   aki     Support for immediate diag response allocation in RFCA_diag
01/25/12   aki     Diag V2 delayed response implementation
10/28/11   aki     Initial revision

==============================================================================*/


#ifndef _RFCA_H
#define _RFCA_H

#include "rfa_variation.h"
#include "comdef.h"

#include "queue.h"
#include "ftm_sequencer_definitions.h"
#include "ftmdiag.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
  #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

extern q_type RFCA_task_cmd_q;
extern q_type RFCA_log_msg_q;

typedef PACK(struct) {
    q_link_type    link;               /* Queue link */
    FTM_SEQUENCER_FTM_SequencerLog *log_msg;
} RFCA_log_msg_queue_item_type;


typedef PACK(struct) {
  word size;
  void* data;
} ftm_async_response_type;

extern ftm_async_response_type ftm_async_response;

// Buffer allocated from diag for the immediate RFCA response
extern void  *RFCA_immediate_diag_response_buffer;
extern uint16 RFCA_immediate_diag_response_buffer_size;


/*----------------------------------------------------------------------------*/
void RFCA_req( void );

/*----------------------------------------------------------------------------*/
void *RFCA_diag ( void *msg_ptr, uint16 msg_size );

/*----------------------------------------------------------------------------*/
void RFCA_ftm_async_cmd_resp(ftm_rsp_pkt_type *rsp_pkt);

/*----------------------------------------------------------------------------*/
void RFCA_ftm_seq_log( FTM_SEQUENCER_FTM_SequencerLog *log_ptr);

/*----------------------------------------------------------------------------*/
void RFCA_log_req(void);

/*----------------------------------------------------------------------------*/
void RFCA_sleep_init(void);

/*----------------------------------------------------------------------------*/
void RFCA_allow_sleep(void);

/*----------------------------------------------------------------------------*/
void RFCA_forbid_sleep(void);


#ifdef _WIN32
  #pragma pack(pop) // Restore
#endif

#ifdef __cplusplus
}
#endif

#endif // _RFCA_H
