/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                DIAGNOSTIC MESSAGE SERVICE STUB FILE

GENERAL DESCRIPTION

  Diag stub functions for Diag public APIs.

Copyright (c) 2010, 2013 Qualcomm Technologies, Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/diag_api_stub/src/diag_api_stub.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/30/13   sr      Removed SIO_RX pool. Cmd req/rsp will now use CMD_SIO_TX 
                   and CMD_SIO_RX pool even in backwards compatibility case  
07/19/11   hm      Stubbed out DSM for now 
07/19/11   hm      Featurized diagnv.h utilization 
11/16/10   is      Include qube.h to include blast_pipe.h earlier for compilation issue
09/09/10   is      Initial revision
===========================================================================*/

#include "comdef.h"            /* Definitions for byte, word, etc. */

#ifdef FEATURE_QUBE
#include "qube.h"              /* To include blast.h -> blast_pipe.h for blast_pipe_t needed in qmsg.q */
#endif

#include "diag.h"              /* For diag_security_type, diag_sp_type, diag_idle_processing_ftype, diag_event_listener, etc */
#ifdef FEATURE_DIAG_NV
    #include "diagnv.h"            /* For nv_stat_enum_type, nv_items_enum_type, etc. */
#endif
#include "diagpkt.h"           /* For diagpkt_user_table_entry_type, diagpkt_cmd_code_type, etc. */
#include "event_defs.h"        /* For event_id_enum_type */
#include "log.h"               /* For log_code_type */
#include "msg_diag_service.h"  /* For msg_const_type, msg_ext_store_type */
#include "msg_qsr.h"           /* For msg_qsr_const_type, err_msg_qsr_const_type */
#include "diagdsm.h"           /* For dsm_pool_mgmt_table_type */

/*===========================================================================
 *      Stubs for diagdsm.h
 *===========================================================================*/
dsm_pool_mgmt_table_type dsm_diag_sio_tx_item_pool;
dsm_pool_mgmt_table_type dsm_diag_smd_rx_item_pool;
dsm_pool_mgmt_table_type dsm_diag_smd_tx_item_pool;

/*===========================================================================
     Stubs for F3 Messaging Service
===========================================================================*/
void qsr_msg_send (const msg_qsr_const_type * xx_msg_const_ptr) /* msg_qsr.h */
{
  return;
}


void qsr_msg_send_1 (const msg_qsr_const_type * xx_msg_const_ptr, uint32 xx_arg1) /* msg_qsr.h */
{
  return;
}


void qsr_msg_send_2 (const msg_qsr_const_type * xx_msg_const_ptr,
                     uint32 xx_arg1,
                     uint32 xx_arg2) /* msg_qsr.h */
{
  return;
}


void qsr_msg_send_3 (const msg_qsr_const_type * xx_msg_const_ptr, 
                     uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3) /* msg_qsr.h */
{
  return;
}


void qsr_msg_send_var (const msg_qsr_const_type * xx_msg_const_ptr, 
                       uint32 num_args, ...) /* msg_qsr.h */
{
  return;
}


void msg_qsrerrlog_3 (const err_msg_qsr_const_type* const_blk, 
                      uint32 code1, uint32 code2, uint32 code3) /* msg_qsr.h */
{
  return;
}


void msg_qsrerrlog_2 (const err_msg_qsr_const_type* const_blk, 
                      uint32 code1, uint32 code2) /* msg_qsr.h */
{
  return;
}


void msg_qsrerrlog_1 (const err_msg_qsr_const_type* const_blk, uint32 code1) /* msg_qsr.h */
{
  return;
}


void msg_qsrerrlog_0 (const err_msg_qsr_const_type* const_blk) /* msg_qsr.h */
{
  return;
}


void msg_init (void) /* msg_diag_service.h */
{
  return;
}


void msg_send (const msg_const_type * xx_msg_const_ptr) /* msg_diag_service.h */
{
  return;
}


void msg_send_1 (const msg_const_type * xx_msg_const_ptr, 
                 uint32 xx_arg1) /* msg_diag_service.h */
{
  return;
}


void msg_send_2 (const msg_const_type * xx_msg_const_ptr, 
                 uint32 xx_arg1, uint32 xx_arg2) /* msg_diag_service.h */
{
  return;
}


void msg_send_3 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1,
                 uint32 xx_arg2, uint32 xx_arg3) /* msg_diag_service.h */
{
  return;
}


void msg_send_var (const msg_const_type * xx_msg_const_ptr, 
                   uint32 num_args, ...) /* msg_diag_service.h */
{
  return;
}


void msg_sprintf(const msg_const_type * const_blk,...) /* msg_diag_service.h */
{
  return;
}


void msg_send_ts(const msg_const_type *const_blk, uint64 timestamp) /* msg_diag_service.h */
{
  return;
}


void msg_save_3(const msg_const_type* const_blk,
                uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3,
                msg_ext_store_type* msg) /* msg_diag_service.h */
{
  return;
}


void msg_errlog_3 (const msg_const_type* const_blk, 
                   uint32 code1, uint32 code2, uint32 code3) /* msg_diag_service.h */
{
  return;
}


void msg_errlog_2 (const msg_const_type* const_blk, 
                   uint32 code1, uint32 code2) /* msg_diag_service.h */
{
  return;
}


void msg_errlog_1 (const msg_const_type* const_blk, uint32 code1) /* msg_diag_service.h */
{
  return;
}

void msg_errlog_0 (const msg_const_type* const_blk) /* msg_diag_service.h */
{
  return;
}


/*===========================================================================
     Stubs for Logging Service
===========================================================================*/
boolean diag_add_log_listener (const unsigned int log_code,
                               const diag_log_listener listener,
                               void *param) /* diag.h */
{
  return FALSE;
}


boolean diag_remove_log_listener (const unsigned int log_code,
                                  const diag_log_listener listener,
                                  void *param) /* diag.h */
{
  return FALSE;
}


PACK(void *) log_alloc (log_code_type code, unsigned int length) /* log.h */
{
  return NULL;
}


void log_shorten (PACK(void *) log_ptr, unsigned int length) /* log.h */
{
  return;
}


void log_commit (PACK(void *) ptr) /* log.h */
{
  return;
}


void log_free (PACK(void *) ptr) /* log.h */
{
  return;
}


boolean log_submit (PACK(void *) ptr) /* log.h */
{
  return FALSE;
}


void log_set_length (PACK(void *) ptr, unsigned int length) /* log.h */
{
  return;
}


void log_set_code (PACK(void *) ptr, log_code_type code) /* log.h */
{
  return;
}


void log_set_timestamp (PACK(void *) ptr) /* log.h */
{
  return;
}


unsigned int log_get_length (PACK(void *) ptr) /* log.h */
{
  return 0;
}


log_code_type log_get_code (PACK(void *) ptr) /* log.h */
{
  return 0;
}


boolean log_status (log_code_type code) /* log.h */
{
  return FALSE;
}


boolean log_on_demand_register (log_code_type log_code,
                                log_on_demand log_on_demand_ptr) /* log.h */
{
  return FALSE;
}


boolean log_on_demand_unregister (log_code_type log_code) /* log.h */
{
  return FALSE;
}


/*===========================================================================
     Stubs for Event Service
===========================================================================*/
boolean diag_add_event_listener (const unsigned int event_id,
                                 const diag_event_listener listener,
                                 void *param) /* diag.h */
{
  return FALSE;
}


boolean diag_remove_event_listener (const unsigned int event_id,
                                    const diag_event_listener listener,
                                    void *param) /* diag.h */
{
  return FALSE;
}


void event_report (event_id_enum_type event_id) /* event.h */
{
  return;
}


void event_report_payload (event_id_enum_type event_id, 
                           uint8 length, 
                           void *data) /* event.h */
{
  return;
}


/*===========================================================================
     Stubs for Req/Rsp Service
===========================================================================*/
boolean diag_cmd_req (const byte *req, unsigned int length, 
                      const diag_cmd_rsp rsp_cb, void *param) /* diag.h */
{
  return FALSE;
}


PACK(void *) diagpkt_err_rsp (diagpkt_cmd_code_type code, 
                              PACK(void *) req_pkt, 
                              uint16 req_len) /* diagpkt.h */
{
  return NULL;
}


PACK(void *) diagpkt_alloc (diagpkt_cmd_code_type code, 
                            unsigned int length) /* diagpkt.h */
{
  return NULL;
}


void diagpkt_tbl_reg (const diagpkt_master_table_type * tbl_ptr) /* diagpkt.h */
{
  return;
}


void diagpkt_tbl_reg_append_proc (diagpkt_master_table_type * tbl_ptr) /* diagpkt.h */
{
  return;
}


void diagpkt_rsp_q_init (void) /* diagpkt.h */
{
  return;
}


PACK(void *) diagpkt_subsys_alloc (diagpkt_subsys_id_type id,
                                   diagpkt_subsys_cmd_code_type code, 
                                   unsigned int length) /* diagpkt.h */
{
  return NULL;
}


PACK(void *) diagpkt_subsys_alloc_v2 (diagpkt_subsys_id_type id,
                                      diagpkt_subsys_cmd_code_type code, 
                                      unsigned int length) /* diagpkt.h */
{
  return NULL;
}


PACK(void *) diagpkt_subsys_alloc_v2_delay (diagpkt_subsys_id_type id,
                                            diagpkt_subsys_cmd_code_type code, 
                                            diagpkt_subsys_delayed_rsp_id_type delayed_rsp_id,
                                            unsigned int length) /* diagpkt.h */
{
  return NULL;
}


void diagpkt_shorten (PACK(void *) ptr, unsigned int length) /* diagpkt.h */
{
  return;
}


void diagpkt_free (PACK(void *)pkt) /* diagpkt.h */
{
  return;
}


void diagpkt_commit (PACK(void *) ptr) /* diagpkt.h */
{
  return;
}


void diagpkt_delay_commit (PACK(void *) pkt) /* diagpkt.h */
{
  return;
}


diagpkt_cmd_code_type diagpkt_get_cmd_code (PACK(void *) ptr) /* diagpkt.h */
{
  return 0;
}


void diagpkt_set_cmd_code (PACK(void *) ptr, diagpkt_cmd_code_type cmd_code) /* diagpkt.h */
{
  return;
}


diagpkt_subsys_id_type diagpkt_subsys_get_id (PACK(void *) ptr) /* diagpkt.h */
{
  return 0;
}


diagpkt_subsys_cmd_code_type diagpkt_subsys_get_cmd_code (PACK(void *) ptr) /* diagpkt.h */
{
  return 0;
}


diagpkt_subsys_status_type diagpkt_subsys_get_status (PACK(void *) ptr) /* diagpkt.h */
{
  return 0;
}


void diagpkt_subsys_set_status (PACK(void *) ptr, 
                                diagpkt_subsys_status_type status) /* diagpkt.h */
{
  return;
}


diagpkt_subsys_delayed_rsp_id_type diagpkt_subsys_get_delayed_rsp_id (PACK(void *) ptr) /* diagpkt.h */
{
  return 0;
}


void diagpkt_subsys_reset_delayed_rsp_id (PACK(void *) ptr) /* diagpkt.h */
{
  return;
}


void diagpkt_subsys_set_rsp_cnt (PACK(void *) ptr, 
                                 diagpkt_subsys_rsp_cnt rsp_cnt) /* diagpkt.h */
{
  return;
}


/*===========================================================================
     Stubs for diag.h
===========================================================================*/
diag_security_type diag_get_security_state (void)
{
  return (diag_security_type)DIAG_SEC_LOCKED;
}


int diag_alloc_check (unsigned int num_allocs, uint32 *alloc_sizes)
{
  return 0;
}


diag_sp_type diag_get_sp_state (void)
{
  return (diag_sp_type)DIAG_SPC_LOCKED;
}


boolean diag_check_password (void *code_ptr)
{
  return FALSE;
}


void diag_timeout (void)
{
  return;
}


void diag_free (void *mem_ptr)
{
  return;
}


void diag_pause (unsigned int duration, boolean enable_rx_processing)
{
  return;
}
  
  
boolean diag_idle_processing_register (diag_idle_processing_ftype fp, 
                                       void *param)
{
  return FALSE;
}


boolean diag_idle_processing_unregister (diag_idle_processing_ftype fp,
                                         void *param)
{
  return FALSE;
}


void diag_tx_notify (void)
{
  return;
}


void diag_otasp_update (void)
{
  return;
}

void diag_task (uint32 param)
{
  return;
}

void diag_early_init(void)
{
  return;
}


/*===========================================================================
     Stubs for Diag_LSM.h
===========================================================================*/
#ifdef FEATURE_WINCE
#error code not present
#endif /* FEATURE_WINCE */


/*===========================================================================
     Stubs for diagbuf.h
===========================================================================*/
PACK (void *) diagbuf_alloc (uint8 code, uint32 length, int nice, 
                             uint8 diagbuf_alloc_request)
{
  return NULL;
}


void diagbuf_shorten (PACK(void *) ptr, unsigned int length)
{
  return;
}


void diagbuf_commit (PACK(void *) ptr)
{
  return;
}


void diagbuf_flush (void)
{
  return;
}


/*===========================================================================
     Stubs for diagdiag.h
===========================================================================*/
boolean diag_register_memory_address_cb (uint32 start, uint32 end,  
                                         const diagpkt_user_table_entry_type * tbl,
                                         int count)
{
  return FALSE;
}


boolean diag_unregister_memory_address_cb (uint32 start, uint32 end,  
                                           const diagpkt_user_table_entry_type * tbl)
{
  return FALSE;
}


/*===========================================================================
     Stubs for diagi.h
===========================================================================*/
dword diag_wait (dword mask)
{
  return 0;
}


/*===========================================================================
     Stubs for diagnv.h
===========================================================================*/
#ifdef FEATURE_DIAG_NV
nv_stat_enum_type diag_nv_read (nv_items_enum_type item,
                                PACK(void *) buf_ptr)
{
  return NV_FAIL_S;
}


nv_stat_enum_type diag_nv_write (nv_items_enum_type item,
                                 PACK(void *) buf_ptr)
{
  return NV_FAIL_S;
}


dword diagnv_get_esn (void)
{
  return 0;
}

#endif

/*===========================================================================
     Stubs for diagpkt.h
===========================================================================*/
uint32 getWrapcount (uint16 wrap_flag)
{
  return 0;
}


void diagpkt_async_block (void)
{
  return;
}


void diagpkt_async_release (void)
{
  return;
}

/*===========================================================================
     Stubs for rbq.h
===========================================================================*/
boolean rbq_init (void *rbq, int size)
{
  return FALSE;
}


void * rbq_alloc (void *rbq, int length)
{
  return NULL;
}


void rbq_shorten (void *rbq, void *ptr, unsigned int length)
{
 return;
}


boolean rbq_commit (void *rbq, void *ptr)
{
  return FALSE;
}


void * rbq_dequeue (void *rbq, int32 *length)
{
  return NULL;
}


boolean rbq_dequeue_return (void *rbq, void *item, boolean free)
{
  return FALSE;
}


int rbq_used (void *rbq)
{
  return 0;
}


int rbq_free (void *rbq)
{
  return 0;
}

#ifdef FEATURE_DIAG_NV

/* Diag NV Stubs  */
nv_stat_enum_type diagnv_peek (nv_peek_type *peek)
{
    return 0;
}

nv_stat_enum_type diagnv_poke (nv_poke_type *poke)
{
    return 0;
}

nv_stat_enum_type diagnv_get_meid  (qword meid)
{
    return 0;
}

nv_stat_enum_type diagnv_get_meid_ext(qword meid, uint16 x)
{
    return 0;
}

void diagnv_refresh_cache()
{
    return;
}

nv_stat_enum_type diag_nv_read_ext ( 
  nv_items_enum_type item,              /* Which NV item to read */
  PACK(void *) buf_ptr,           /* Where to put the data */
  uint16 item_context
)
{
    return 0;
}

nv_stat_enum_type diag_nv_write_ext ( 
  nv_items_enum_type item,
  PACK(void *) buf_ptr,
  uint16 item_context
)
{
    return 0;
}

boolean diag_set_sp_state (diag_sp_type sp)
{
    return FALSE;
}
#endif
