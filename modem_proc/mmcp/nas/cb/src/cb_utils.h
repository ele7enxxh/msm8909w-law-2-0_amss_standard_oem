#ifndef CB_UTILS_H
#define CB_UTILS_H
/*===========================================================================

                    CB TASK UTILS LIBRARY HEADER FILE

DESCRIPTION
  This file contains types, function prototypes and data declarations for
  Cell Broadcast task

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cb_utils.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/09/11   vm      Order of FN within the same skip list ? GL1/WL1 expects FN list in ascending order  
08/03/05   kvk    Prototype for cb_create_message_code_entry() modified to
                  handle extra parameter data_coding_scheme.
06/06/05   kvk    Extern variables defined.
04/19/05   kvk    Added the prototype for flipping the octets.
03/30/05   kvk    Changed the prototype for wms cell change indication.
08/02/04   vdr     Added prototype of the cb_delete_required_page
07/14/04   vdr     Added prototypes for:
                    o cb_skiped_fn()
                    o cb_send_wms_duplication_ind()
05/19/04   vdr     Fixed duplication detection
04/16/04   vdr     Featurizing out CB DRX mode processing
                   Added some extra comments
06/24/03   vdr     Initial version


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_CB

#include "comdef.h"
#include "cb_task.h"
#include "cbtask.h"
#include "cbmsg.h"
#include "cbwmsif.h"
#include "rrcmmif.h"
#include "sys.h"
#ifdef FEATURE_WCDMA_ETWS
#include "cb_state.h"
#endif
#include "sys_v.h"
#include "nasutils.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define CB_ANY_MESSAGE_CODE 0xFFFFFFFF

#define OCTET_BIT_LEN 0x8

#ifdef FEATURE_GSM_CB_PSEUDO_DRX
#define CB_PSEUDO_DRX_DEFAULT_PERIODICITY 16
#define CB_PSEUDO_DRX_MAX_LENGTH (MAX_SCHEDULING_PERIOD_LENGTH-1)

#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
extern byte cb_pseudo_drx_offset_sim[MAX_AS_IDS];
extern boolean cb_is_second_pseudo_mode_sim[MAX_AS_IDS];
extern dword cb_pseudo_drx_start_fn_sim[MAX_AS_IDS];
extern dword cb_pseudo_drx_prev_start_fn_sim[MAX_AS_IDS];
extern dword cb_dup_timer_value_sim[MAX_AS_IDS];

#define cb_pseudo_drx_offset cb_pseudo_drx_offset_sim[cb_as_id]
#define cb_is_second_pseudo_mode cb_is_second_pseudo_mode_sim[cb_as_id]
#define cb_pseudo_drx_start_fn cb_pseudo_drx_start_fn_sim[cb_as_id]
#define cb_pseudo_drx_prev_start_fn cb_pseudo_drx_prev_start_fn_sim[cb_as_id]
#define cb_dup_timer_value cb_dup_timer_value_sim[cb_as_id]
#else
extern byte cb_pseudo_drx_offset;
extern boolean cb_is_second_pseudo_mode;
extern dword cb_pseudo_drx_start_fn;
extern dword cb_pseudo_drx_prev_start_fn;
extern dword cb_dup_timer_value;
#endif

extern boolean cb_pseudo_drx_supported;
extern dword cb_pre_pseudo_drx_scan_length;
extern byte cb_first_pseudo_drx_periodicity;
extern byte cb_second_pseudo_drx_periodicity;

#endif

#ifdef FEATURE_DUAL_SIM

#define MSG_HIGH_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_HIGH,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3))
    
#define MSG_MED_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_MED,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3))
    
#define MSG_LOW_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_LOW,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3))
    
#define MSG_ERROR_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_ERROR,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3))

#define MSG_FATAL_DS( sub , str, p1, p2, p3)\
    ERR_FATAL ("DS: SUB %d " str , (sub+1), (p1), (p2))

#else

#define MSG_HIGH_DS( sub , str, p1, p2, p3)\
    MSG_HIGH (str, (p1), (p2), (p3))
    
#define MSG_MED_DS( sub , str, p1, p2, p3)\
    MSG_MED (str, (p1), (p2), (p3))
    
#define MSG_LOW_DS( sub , str, p1, p2, p3)\
    MSG_LOW (str, (p1), (p2), (p3))
    
#define MSG_ERROR_DS( sub , str, p1, p2, p3)\
    MSG_ERROR (str, (p1), (p2), (p3))

#define MSG_FATAL_DS( sub , str, p1, p2, p3)\
    ERR_FATAL (str , (p1), (p2), (p3))

#endif


/*===========================================================================

FUNCTION    CB_MESSAGE_IS_REQUESTED

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cb_message_is_requested
(
  word incoming_message_id
) ;

/*===========================================================================

FUNCTION    CB_CREATE_MESSAGE_ENTRY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cb_message_list_type *cb_create_message_entry
(
  cb_message_list_type *current_message_ptr,
  word                  message_id,
  dword                 received_fn
) ;

/*===========================================================================

FUNCTION      CB_FIND_EXISTING_PAGE_BUFFER

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cb_missing_page_type *cb_find_existing_page_buffer
(
  cb_missing_page_type *current_page,
  byte page_number
) ;

/*===========================================================================

FUNCTION      CB_CREATE_PAGE_DESCRIPTOR

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cb_missing_page_type *cb_create_page_descriptor
(
  cb_rat_type active_rat,
  cb_message_list_type *message,
  cb_missing_page_type *first_page,
  byte                  page_number
) ;

/*===========================================================================

FUNCTION      CB_DELETE_MESSAGE_FROM_LIST

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cb_message_list_type *cb_delete_message_from_list
(
  cb_message_list_type *message_tobe_deleted
) ;

/*===========================================================================

FUNCTION      CB_SEND_WMS_DATA_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_send_wms_data_ind
(
  byte  page_length,
  byte *page_buffer
) ;

/*===========================================================================

FUNCTION      CB_DELETE_PAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cb_missing_page_type *cb_delete_page
(
  cb_message_list_type *message,
  cb_missing_page_type *page_tobe_deleted
) ;

/*===========================================================================

FUNCTION CB_SEND_SKIP_REQUEST

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_send_skip_request
(
  cb_skip_ind_type skip_type,
  byte skip_list_length,
  dword *skip_list
) ;

/*===========================================================================

FUNCTION    CB_SEND_L1_REQUEST

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_send_l1_request(void * message_ptr) ;

/*===========================================================================

FUNCTION CB_GET_CMD_BUF

DESCRIPTION
  This function gets an mm_cmd_type from MM task with memory already allocated.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/
extern cb_command_type *cb_get_cmd_buf( void ) ;

/*===========================================================================

FUNCTION    CB_FIND_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cb_message_list_type *cb_find_message
(
  cb_message_list_type *current_search_list_entry,
  word message_id
) ;

/*===========================================================================

FUNCTION      CB_SEND_CELL_CHANGE_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_send_wms_cell_change_ind
( 
 byte *incoming_plmn,
 byte *incoming_lac,
 byte *incoming_cell_id
);

/*===========================================================================

FUNCTION      CB_PROCESS_WMS_SEARCH_LIST

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_process_wms_search_list
(
  cb_search_list_req_type *search_list_req
) ;

/*===========================================================================

FUNCTION    CB_PROCESS_SKIP_ARCHIVE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_process_skip_archive(dword received_fn) ;

/*===========================================================================

FUNCTION      CB_CALCULATE_BASE_FN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern dword cb_calculate_base_fn
(
  dword received_fn,
  byte paging_block_number
) ;

/*===========================================================================

FUNCTION    CB_CREATE_MESSAGE_CODE_ENTRY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cb_user_page_info_type *cb_create_message_code_entry
(
  cb_message_list_type *current_message_ptr,
  word                  message_code,
  byte                  gs_indicator,
  byte                  update_number,
  byte                  number_of_pages,
  byte                  data_coding_scheme
) ;

/*===========================================================================

FUNCTION    CB_FIND_MESSAGE_CODE_WITH_GS

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cb_user_page_info_type *cb_find_message_code_with_gs
(
  cb_message_list_type *current_message_ptr,
  word message_code
) ;

/*===========================================================================

FUNCTION    CB_FIND_MESSAGE_CODE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cb_user_page_info_type *cb_find_message_code
(
  cb_message_list_type *current_message_ptr,
  word message_code
) ;

/*===========================================================================

FUNCTION      CB_FILTERS_OUT_OF_DATA_MESSAGES

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_filters_out_of_date_messages
(
  cb_channel_ind_type channel_msg_list_id
) ;

/*===========================================================================

FUNCTION      CB_DEACTIVATE_BMC_MONITORING

DESCRIPTION
  This function sends routes BMC deactivate request to the current RAT

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_deactivate_bmc_monitoring
(
  cb_rat_type current_rat
) ;

/*===========================================================================

FUNCTION    CB_START_GSM_NON_DRX_MODE

DESCRIPTION
  This function sends to GSM L1 CB_SCHEDULING_REQ primitive in order to 
  trigger NON DRX CB message reception

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_start_gsm_non_drx_mode
(
  cb_channel_ind_type channel_type
);

/*===========================================================================

FUNCTION    CB_START_NON_DRX_MODE

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_start_non_drx_mode
(
  cb_rat_type current_rat
) ;

/*===========================================================================

FUNCTION    CB_PUT_MESSAGE_INTO_LIST

DESCRIPTION
  This function inserts new message descriptor into specific Message List

DEPENDENCIES
  None

RETURN VALUE
  Pointer which points out to the inserted Message Descriptor

SIDE EFFECTS
  None

===========================================================================*/
extern cb_message_list_type *cb_put_message_into_list
(
  cb_message_list_type *current_search_list_entry,
  cb_message_list_type *new_list_entry
) ;

/*===========================================================================

FUNCTION    CB_SKIPED_FN

DESCRIPTION
  This function searches in skip_archive for the requested FN

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if requested FN has been skip by CB earlier
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cb_skiped_fn
(
  dword received_fn
) ;

/*===========================================================================

FUNCTION      CB_SEND_WMS_DUPLICATION_IND

DESCRIPTION
  This function sends CB DUPLICATION INDICATION to WMS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_send_wms_duplication_ind
(
  /* Message Identifier */
  word                   message_id,

  /* Message code */
  word                   message_code,

  /* Geographic scope */
  byte                   gs_indicator,

  /* Update Number */
  byte                   update_number,

  /* Page Number */
  byte                   page_number,

  /* Total number of pages */
  byte                   number_of_pages,

  /* Data coding scheme */
  byte                   data_coding_scheme
) ;

/*===========================================================================

FUNCTION      CB_DELETE_REQUIRED_PAGE

DESCRIPTION
  This function deletes required page from the message list according to WMS
  request

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_delete_required_page
(
  word message_id,
  word message_code,
  byte page_number
) ;

/*===========================================================================

FUNCTION CB_CONVERT_RRC_PLMN_ID_TO_SYS_PLMN_ID

DESCRIPTION
  This function converts from the RRC PLMN ID format (1 BCD digit per byte)
  to the NAS PLMN_id_T format (2 BCD digits per byte).

DEPENDENCIES
  None

RETURN VALUE
  PLMN ID converted to PLMN_id_T format (3 byte array - 2 BCD digits/byte)

SIDE EFFECTS
  None

===========================================================================*/
extern sys_plmn_id_s_type cb_convert_rrc_plmn_id_to_sys_plmn_id
(
  rrc_plmn_identity_type plmn_id 
) ;

/*===========================================================================

FUNCTION      CB_DELETE_ALL_PAGES

DESCRIPTION
  This function deletes all pages from the task's cache memory

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_delete_all_pages
(
  void
) ;

#ifdef FEATURE_UMTS_BMC
/*Extern definitions for UMTS BMC goes here*/
extern boolean bmc_rx_suspended;
extern word pdu_cnt;

/*===========================================================================

FUNCTION   BMC_RLC_STATE_INIT

DESCRIPTION
  Init the RLC call state. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void bmc_rlc_state_init(void);

/*===========================================================================

FUNCTION   BMC_REGISTER_RLC_WM

DESCRIPTION
   This function registers the WM 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bmc_register_rlc_wm
(
 uint8 rlc_lc_id
);

/*===========================================================================

FUNCTION   BMC_DEREGISTER_WM_WITH_RLC

DESCRIPTION
   This function sends the register watermark command to RLC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bmc_deregister_wm_with_rlc
(
  /*BMC RLC state pointer*/
  bmc_rlc_state_type    *bmc_rlc_state_ptr
);

/*===========================================================================

FUNCTION   BMC_CLEANUP_RLC_WATERMARKS

DESCRIPTION
   This function cleans up the watermarks that are owned by BMC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bmc_cleanup_rlc_watermarks
(
  /*BMC RLC state pointer*/
  bmc_rlc_state_type    *bmc_rlc_state_ptr
);

/*===========================================================================

FUNCTION   CB_BMC_FLIP_OCTET_BITS_RLC_PDU

DESCRIPTION
  This function flips the bits in evry octet in RLC PDU.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_flip_octet_bits_rlc_pdu
(
 byte *rlc_pdu_ptr,
 word pdu_size
);

/*===========================================================================

FUNCTION   BMC_PROCESS_DSM_DATA_CB

DESCRIPTION
  This is the call back function called by RLC when the PDU is queued up
  in the DSM chain. This function post a command in CB command queue
  so that it can be processed by the BMC task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bmc_process_dsm_data_cb
(
 byte rlc_id,
 byte no_pdus,
 void *callback_data
);

/*===========================================================================

FUNCTION   BMC_GET_DSM_ITEM

DESCRIPTION
  This function will return the dsm item from the watermark.

DEPENDENCIES
  None

RETURN VALUE
  Return the first dsm item in the wm chain

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *bmc_get_dsm_item(void);

/*===========================================================================

FUNCTION   CB_UMTS_GET_RLC_PDU_DATA_PTR

DESCRIPTION
  This compute the RLC length stored in the dsm chain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_umts_get_rlc_pdu_data_ptr
(
  byte          *rlc_pdu_ptr,
  dsm_item_type *item_ptr,
  word          pdu_len
);

/*===========================================================================

FUNCTION   BMC_COMPUTE_RLC_PDU_LEN

DESCRIPTION
  This compute the RLC length stored in the dsm chain

DEPENDENCIES
  None

RETURN VALUE
  Return the RLC PDU lenght

SIDE EFFECTS
  None

===========================================================================*/
word bmc_compute_rlc_pdu_len
(
  dsm_item_type *pkt_head_ptr
);

/*===========================================================================

FUNCTION   CB_BMC_GET_MASK

DESCRIPTION
  This function computes a mask which is based on number of RLC PDUs per 
  BMC page

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern word cb_bmc_get_mask
(
  byte bit_number
) ;

/*===========================================================================

FUNCTION CB_SEND_SKIP_REQUEST

DESCRIPTION
  This function sends SKIP request to WL1

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_send_skip_request
(
 word   skip_list_length,
 dword *skip_list
);


/*===========================================================================

FUNCTION    CB_START_UMTS_NON_DRX_MODE

DESCRIPTION
  This function sends to UMTS L1 CB_SCHEDULING_REQ primitive in order to 
  trigger NON DRX CB message reception

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_start_umts_non_drx_mode
(
  void
);

/*===========================================================================

FUNCTION    CB_BMC_GET_KI_SFN

DESCRIPTION
  This function calculates the SFN that part of the BMC message is supposed to
  be transmited with based on first received SFN, RLC PDU number and CTCH
  allocation period

DEPENDENCIES
  None

RETURN VALUE
  SFN number

SIDE EFFECTS
  None

===========================================================================*/
extern dword cb_bmc_get_Ki_sfn
(
  dword incoming_sfn ,
  word  rlc_pdu_number ,
  word  ctch_allocation_period,
  word  ctch_offset
) ;

/*===========================================================================

FUNCTION      CB_BMC_CLEAN_UP_OBSOLETE_PAGE_FROM_LIST

DESCRIPTION
  This function deletes from the cache memory partionally received pages 
  which have been received over specific RAT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_clean_up_obsolete_page_from_list
(
  cb_rat_type rat_to_be_deleted,
  cb_message_list_type *list
) ;

/*===========================================================================

FUNCTION   CB_BMC_FLIP_OCTET_BITS_RLC_PDU

DESCRIPTION
  This function flips the bits in evry octet in RLC PDU.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_flip_octet_bits_rlc_pdu
(
 byte *rlc_pdu_ptr,
 word pdu_size
) ;

#endif/*FEATURE_UMTS_BMC*/

/*
 * Definition of CB active substate change events
 */

#define CB_STD_NULL_EVENT                             0
#define CB_STD_NON_DRX_ACTIVATED                      1
#define CB_STD_DRX_ACTIVATED                          2
#define CB_STD_NON_DRX_P_ACTIVATED                    3
#define CB_STD_DEACTIVATED                            4
#define CB_STD_RLC_PDU_DROPPED                        5
#define CB_STD_DATA_PAGE_RECD                         6
#define CB_STD_PSEUDO_DRX_ACTIVATED                   7

#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
#define CB_ETWS_STD_NULL_EVENT                             0
#define CB_ETWS_STD_ETWS_MESSAGE_IDS_ENABLED               1
#define CB_ETWS_STD_PRIMARY_NOTIFICATION_RECD              2
#define CB_ETWS_STD_CTCH_TIMER_EXPIRED                     3
#define CB_ETWS_STD_CTCH_SUPPORTED                         4
#define CB_ETWS_STD_ETWS_MESSAGE_IDS_DISABLED              5
#define CB_ETWS_STD_CTCH_DEACTIVATED                       6
#define CB_ETWS_STD_DRX_ACTIVATED                          7
#define CB_ETWS_STD_NDRX_ACTIVATED_AFTR_L1_SCHD_EXPIRY     8
#endif
#define CB_STD_INVALID_EVENT                          255

typedef byte cb_event_T;

/*===========================================================================

FUNCTION    CB_SUBSTATE_CONTROL

DESCRIPTION
  This function handles CB active substate transition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cb_active_substate_control( cb_event_T cb_event );
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
typedef cb_state_id_type (*cb_etws_substate_control_type)( cb_event_T cb_event );

extern cb_etws_substate_control_type cb_etws_substate_control;

extern void cb_send_wms_etws_prim_notification_ind
(
  const sys_plmn_id_s_type* plmn,
  word                      message_id,
  word                      serial_number,
  const byte*               warning_type,
  boolean                   warning_security_info_present,
  const byte*               warning_security_info
);
extern void cb_send_wms_etws_sec_notification_ind
(
  word  message_id,
  word  serial_number,
  byte  data_coding_scheme,
  byte  number_of_pages,
  const byte* sec_notification_data
);

extern cb_bmc_etws_msg_list_type *cb_find_etws_message
(
  cb_bmc_etws_msg_list_type *current_search_list_entry,
  word message_id
);

extern cb_bmc_etws_msg_info_type *cb_etws_find_message_code
(
  cb_bmc_etws_msg_list_type *current_message_ptr,
  word message_code
);
extern cb_bmc_etws_msg_list_type *cb_create_etws_primary_message_entry
(
  word                  message_id
);

cb_bmc_etws_msg_info_type *cb_create_etws_message_code_entry
(
  cb_bmc_etws_msg_list_type  *current_message_ptr,
  word                message_code,
  byte                gs_indicator,
  byte                update_number,
  const byte*         incoming_plmn
);
extern cb_bmc_etws_msg_list_type *cb_put_etws_message_into_list
(
  cb_bmc_etws_msg_list_type *current_search_list_entry,
  cb_bmc_etws_msg_list_type *new_list_entry
);

extern byte number_of_etws_ids_in_search_list
(
  void
);

extern cb_bmc_etws_msg_list_type *cb_filter_timeout_primary_etws_msg_from_list
(
  cb_bmc_etws_msg_list_type **first_message_entry,
  word                      message_id
);

extern void cb_start_etws_timer( timer_id_T timer_id, dword num_millisecs );

extern void cb_stop_etws_timer( timer_id_T timer_id );
#endif

/*===========================================================================

FUNCTION   CB_DELETE_MESSAGE_CODE

DESCRIPTION
  This function deletes the mesasge code from the cache.

DEPENDENCIES
  None

RETURN VALUE
  Message list

SIDE EFFECTS
  None

===========================================================================*/
extern cb_message_list_type *cb_delete_message_code
(
  cb_message_list_type *current_msg,
  cb_user_page_info_type *cb_prev_message_code,
  cb_user_page_info_type *cb_message_code_tobe_deleted
);

#ifdef FEATURE_GSM_CB_PSEUDO_DRX
/*===========================================================================

FUNCTION    CB_START_GSM_PSEUDO_DRX_MODE

DESCRIPTION
  This function sends to GSM L1 CB_SCHEDULING_REQ primitive in order to 
  trigger Pseudo DRX CB message reception

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_start_gsm_pseudo_drx_mode(boolean current_cycle_expired);

#endif /* FEATURE_GSM_CB_PSEUDO_DRX */

/*===========================================================================

FUNCTION    cb_sort_list

DESCRIPTION
  This function is to sort the list of values in ascending order.

DEPENDENCIES
  None

RETURN VALUE
  sorted list(dword* type)

SIDE EFFECTS
  None

===========================================================================*/
dword* cb_sort_list(dword*, word);
#endif /* #ifdef FEATURE_GSM_CB */

/*===========================================================================

FUNCTION    cb_dup_start_timer

DESCRIPTION
  This function is to start duplication detection timer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_dup_start_timer( timer_id_T timer_id, dword num_millisecs );

/*===========================================================================

FUNCTION    cb_dup_stop_timer

DESCRIPTION
  This function is to stop duplication detection timer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_dup_stop_timer( timer_id_T timer_id );
/*===========================================================================

FUNCTION    cb_update_cache_timer_offsets

DESCRIPTION
  This function is to update the cache timer offsets value after expiry of duplication detection timer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_update_cache_timer_offsets(dword num_remaining_ms);
/*===========================================================================

FUNCTION    cb_update_cache_for_multiple_message_deletion

DESCRIPTION
  This function is to update the cache timer offsets value when multiple messages are deleted at once.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_update_cache_for_multiple_message_deletion(boolean timer_offset_0_deleted);
#endif /* #ifndef CB_UTILS_H */
