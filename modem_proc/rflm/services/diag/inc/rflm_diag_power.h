/*!
  @file
  rflm_diag_power.h

  @brief
  RFLM power change logging.

  @detail

*/

/*===========================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/diag/inc/rflm_diag_power.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2014-11-17   aw      Initial version
===========================================================================*/

#ifndef RFLM_DIAG_POWER_H
#define RFLM_DIAG_POWER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rflm.h"
#include "rflm_diag.h"
#include "rflm_diag_power_rfa.h"
#include <qurt.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define RFLM_DIAG_POWER_BUFFER_CNT           2
#define RFLM_DIAG_POWER_BUFFER_SIZE_WORD     1024
#define RFLM_DIAG_POWER_RD_WR_BUF_MASK       0x60000000
#define RFLM_DIAG_POWER_WRITE_INDEX_MASK     0x3FF
#define RFLM_DIAG_POWER_RD_WR_INDEX_MASK     0xFFFFF
#define RFLM_DIAG_POWER_FUNC_SUCCESS_MASK    0x80000000
#define RFLM_DIAG_POWER_BUF_FULL_SWITCH_MASK 0x10000000
#define RFLM_DIAG_POWER_FUNC_SUCCESS_BIT_POS 31
#define RFLM_DIAG_POWER_WR_BUF_BIT_POS       30
#define RFLM_DIAG_POWER_RD_BUF_BIT_POS       29
#define RFLM_DIAG_POWER_SWITCH_BIT_POS       28
#define RFLM_DIAG_POWER_RD_NUM_BIT_POS       10
#define RFLM_DIAG_POWER_RD_NUM_BIT_WIDTH     10

typedef enum
{
   RFLM_DIAG_POWER_SHUTDOWN_SIG,
   RFLM_DIAG_POWER_BUF_FULL_SWITCH_SIG,
   RFLM_DIAG_POWER_RFA_20MS_SWITCH_SIG,
   RFLM_DIAG_POWER_MAX_SIGNALS
}rflm_diag_power_sig_e;

#define RFLM_DIAG_POWER_SIG_MASK (BITL(RFLM_DIAG_POWER_MAX_SIGNALS) - 1)

typedef struct
{
  uint8 status;           // 0: switch ignored; 1: switch done
  uint8 rbuf_index;       // read buffer index
  uint16 num_of_rd;       // number of valid trans to read
  uint32 * rbuf_ptr;      // starting address of rbuf
} rflm_diag_power_switch_result_t;

#define RFLM_DIAG_POWER_BUFFER_ITEM_SIZE_WORD (sizeof(rflm_diag_power_buffer_item_t)/4)
#define RFLM_DIAG_POWER_BUFFER_MAX_ITEM_CNT (RFLM_DIAG_POWER_BUFFER_SIZE_WORD/RFLM_DIAG_POWER_BUFFER_ITEM_SIZE_WORD)

/* Used by Tech team to report power changing events */
#define RFLM_DIAG_POWER_REPORT(handle_id, timestamp, ... ) \
 _RFLM_DIAG_POWER_REPORT_FUNC(rflm_diag_power_report_, handle_id, timestamp, ## __VA_ARGS__);

#define _RFLM_DIAG_POWER_REPORT_FUNC(func, handle_id, timestamp, ...) \
  _RFLM_DIAG_MSG_FUNC_EXPAND(func, _RFLM_DIAG_NUM_ARGS(__VA_ARGS__), ## __VA_ARGS__)(handle_id, timestamp, ## __VA_ARGS__)

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                    INTERNAL FUNCTIONS

===========================================================================*/
extern void rflm_diag_power_init(void);

extern uint32 rflm_diag_power_switch_wbuf(uint32 *atomic_index_ptr);

extern uint32 rflm_diag_power_alloc_buf(uint32 *atomic_index_ptr);
extern void rflm_diag_power(uint32 arg0, uint32 arg1, uint32 arg2, uint32 arg3,
                            uint32 arg4, uint32 arg5, uint32 arg6, uint32 arg7);

extern void rflm_diag_power_report_2(uint8 handle_id, uint32 timestamp, 
								 rflm_diag_power_type_e type_0, uint16 data_0);
extern void rflm_diag_power_report_4(uint8 handle_id, uint32 timestamp, 
								 rflm_diag_power_type_e type_0, uint16 data_0,
								 rflm_diag_power_type_e type_1, uint16 data_1);
extern void rflm_diag_power_report_6(uint8 handle_id, uint32 timestamp, 
								 rflm_diag_power_type_e type_0, uint16 data_0,
								 rflm_diag_power_type_e type_1, uint16 data_1,
								 rflm_diag_power_type_e type_2, uint16 data_2);
extern void rflm_diag_power_report_8(uint8 handle_id, uint32 timestamp, 
								 rflm_diag_power_type_e type_0, uint16 data_0,
								 rflm_diag_power_type_e type_1, uint16 data_1,
								 rflm_diag_power_type_e type_2, uint16 data_2,
								 rflm_diag_power_type_e type_3, uint16 data_3);
extern void rflm_diag_power_report_10(uint8 handle_id, uint32 timestamp, 
								 rflm_diag_power_type_e type_0, uint16 data_0,
								 rflm_diag_power_type_e type_1, uint16 data_1,
								 rflm_diag_power_type_e type_2, uint16 data_2,
								 rflm_diag_power_type_e type_3, uint16 data_3,
								 rflm_diag_power_type_e type_4, uint16 data_4);
extern void rflm_diag_power_report_12(uint8 handle_id, uint32 timestamp, 
								 rflm_diag_power_type_e type_0, uint16 data_0,
								 rflm_diag_power_type_e type_1, uint16 data_1,
								 rflm_diag_power_type_e type_2, uint16 data_2,
								 rflm_diag_power_type_e type_3, uint16 data_3,
								 rflm_diag_power_type_e type_4, uint16 data_4,
								 rflm_diag_power_type_e type_5, uint16 data_5);

extern void rflm_diag_power_log_thread(void *arg);

extern void rflm_diag_power_release(void);

/* indicate read done on a read buffer*/
extern void rflm_diag_power_read_done_notify(void);

/* return number of valid trans to read and read buffer index*/
extern rflm_diag_power_switch_result_t rflm_diag_power_switch_buffer(void);


/*===========================================================================

                    EXTERNAL INTERFACE FUNCTIONS

===========================================================================*/

/* Used by Tech clients to report power changing events */
#define RFLM_DIAG_POWER_REPORT(handle_id, timestamp, ... ) \
 _RFLM_DIAG_POWER_REPORT_FUNC(rflm_diag_power_report_, handle_id, timestamp, ## __VA_ARGS__);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
