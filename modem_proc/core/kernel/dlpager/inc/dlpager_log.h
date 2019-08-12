#ifndef __DLPAGER_LOG_H__
#define __DLPAGER_LOG_H__
/*===========================================================================
 * FILE:         dlpager_log.h
 *
 * DESCRIPTION:  Dlpager logging
 *
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
 ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/inc/dlpager_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/14   ao      Initial revision
===========================================================================*/

typedef enum
{
    DLP_FAULT_TEXT_PRIMARY = 1,
    DLP_FAULT_TEXT_SECONDARY  ,
    DLP_FAULT_RODATA    ,
    DLP_FAULT_RWDATA    ,
    DLP_FAULT_BSS       ,

    DLP_FAULT_DUPLICATED,
    DLP_WRITE_DETECT    ,
    DLP_POST_FAULT      ,

    DLP_REMOVE_CLK_VOTE ,
    DLP_FIRST_CLK_VOTE  ,
    DLP_MAX_CLK_VOTE    ,

 /* All entries below DLP_DEBUG_TYPE definition are only logged if DLP_DEBUG is defined */
#define DLP_DEBUG_TYPE DLP_LOAD_TEXT_PRIMARY

    DLP_LOAD_TEXT_PRIMARY  ,
    DLP_LOAD_TEXT_SECONDARY,
    DLP_LOAD_RODATA     ,
    DLP_LOAD_RWDATA     ,
    DLP_LOAD_BSS        ,

    DLP_STORE_RW        ,
    DLP_PLRU            ,
    DLP_EVICT           ,

    DLP_RESTART_TIMER   ,
    DLP_REMOVE_CLK_REQ  ,
    DLP_FIRST_CLK_REQ   ,
    DLP_MAX_CLK_REQ     ,
    DLP_TIMER_REQ       ,

} dlpager_log_event_t;

/* Pcycles logged in the last call to dlpager_log_start
   make sure that dlpager_start_log is called recently enough
   before using this global otherwise a old value may be there */
extern uint64_t dlpager_log_last_pcycles;

void dlpager_log_init(void);
unsigned dlpager_log_start(dlpager_log_event_t event, void* fault_addr, unsigned int thread_id, unsigned int ssr);
void dlpager_log_end(dlpager_log_event_t event, unsigned log_idx, unsigned int evicted, unsigned clk);

#endif
