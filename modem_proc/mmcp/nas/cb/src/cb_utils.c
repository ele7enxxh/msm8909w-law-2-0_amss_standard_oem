/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     CB MESSAGE UTILITIE MODULE (CB_UTILS.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cb_utils.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/28/12   ps      Fixed compilation warnings 
12/09/11   vm      Order of FN within the same skip list ? GL1/WL1 expects FN list in ascending order  
08/03/11  jbk      NV support to disable CB duplicate detection.
04/15/06   kvk     Renamed cb_free and cb_alloc to cb_mem_free and cb_mem_alloc 
                   to make it compatible with L4 builds
09/28/05   kvk     Reset the RLC PDU Allocation table when the message ID is deleted from the list
09/08/05   vdr     Now reseting skip_archive[] when it reaches its max length
08/03/05   kvk     Modified function cb_create_message_code_entry() to take data_coding_scheme as 
                   parameter and store in the message pointer.
07/19/05   vdr     Reset RLC PDU Allocation table upon deletion of the BMC 
                   message(s) from the cache memory
06/14/05   kvk     Removed passing Cell ID along with DL_BMC_BLOCK_IND. During
                   Cell change when BMC registers back with RLC WM, PDUs received on
                   old cell will be discarded.
06/06/05   kvk     Defined Macro MAX_PDU_THRESHOLD to set the Max Pdu counter. If the
                   count exceeds the threshold then BMC will be suspended and WL1 is requested
                   to stop monitoring the CTCH channel.
06/02/05   kvk     cb_bmc_reset_msg_bitmap_info() will called only if the BMC DRX is
                   enabled.
05/19/05   vdr     Now calling memcmp() instead of memcpy() in order to compare
                   new recently received WMS Search List
05/16/05   kvk     Reset the received status of the SFNs corresponding to the message id when 
                   page associated with the message id is deleted. This will allow 
                   the back reception of the page in the future DRX cycles.
04/29/05   vdr     Modified page deletion procedure in order to prevent further 
                   RLC PDU processing when current_message pointer becomes NULL
04/19/05   kvk     Fixed the SFN calculation after the SFN rollover.
03/31/05   kvk     Now passing the type of cell change information to WMS
                   to make sure WMS is in sync with BMC.
03/30/05   kvk     Now passing the cell id where PDU was received along with
                   DL_BMC_BLOCK_IND.
03/28/05   kvk     Added support for UMTS BMC. New Util functions defined for 
                   Non DRX and DRX scheduling.
01/18/05   vdr     Added support for FEATURE_APIONE feature flag
09/14/04   vdr     Modified CB message deleting procedure to fix NULL pointer
                   appearance
08/02/04   vdr     Added processing of the CB_DELETE_PAGE_REQ
07/20/04   vdr     Modified message management procedures
07/14/04   vdr     Modified cb_filter_out_obsolete_id_from_list() to fix memory 
                   leak
                   Added new function cb_send_wms_duplication_ind()
05/26/04   vdr     Fixed linked list message descriptor insertion when message 
                   carring new ID is arrived
05/19/04   vdr     Fixed duplication detection
04/16/04   vdr     Featurizing out CB DRX mode processing
                   Added some extra comments
07/03/03   jca     Initial version
02/16/07   NR      Renaming the FEATRUE_APIONE with FEATURE_WMS_CM_NAS_INTERFACES
03/13/07   ajt     Removing Feature Flag FEATURE_DSM_MEM_CHK.
06/15/07   jbk     clean up LINT errors.
28/07/11   ss      Changed memcpy to memmove as the src/dst region were overlapping
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_CB

#include <string.h>
#include <stringl/stringl.h>
#include "comdef.h"
#include "cb_task.h"
#include "cbtask.h"
#include "msg.h"
#include "err.h"
#include "cbmsg.h"
#include "cbl1if.h"
#include "gs_v.h"
#include "ms.h"
#include "cb_schedule.h"
#include "sys_stru_v.h"
#include "cb_utils.h"
#include "cb_mem.h"
#ifdef FEATURE_UMTS_BMC
#include "rlcbmcif.h"
#include "wcdmamvsif.h"
#include "l1task.h"
#include "wcdmamvsif.h" 
#include "wcdmatmcif.h" 
#include "gs_v.h"
#endif/*FEATURE_UMTS_BMC*/
#include "rrcmmif.h"
#include "sys.h"
#include "sys_v.h"
#include "geran_multi_sim_api.h"

#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#endif
#include "mm.h"
#ifdef FEATURE_DUAL_SIM
#include "cb_state.h"
#endif
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "stdlib.h"
#include "timers_v.h"
typedef enum search_direction_tag
{
  NO_SEARCH,
  UPWARD_SEARCH ,
  DOWNWARD_SEARCH
} search_direction_type ;

void cb_ctch_pdu_decode_error(dword pdu_sfn);
void cb_send_event_ctch_start
(
  byte ctch_mode,
  byte scheduling_period_length
);
void cb_send_event_ctch_stop(void);
void cb_send_ota_msg_log_packet(uint8 rat_type,uint32 msg_size,uint8 msg_type, byte   *msg_ptr);
extern dword cb_calculate_slot_fn(dword reference_fn,byte  slot_number);
#define MAX_SKIP_ARCHIVE_LENGTH 200
#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)
extern cb_as_id_e_type cb_as_id;
#endif

#ifdef FEATURE_TRIPLE_SIM
static  byte skip_archive_length_sim[MAX_AS_IDS] ={ 0 ,0,0};
#define skip_archive_length skip_archive_length_sim[cb_as_id]

static  dword skip_archive_sim[MAX_AS_IDS][MAX_SKIP_ARCHIVE_LENGTH] ;
#define skip_archive skip_archive_sim[cb_as_id]

extern byte cb_bmc_scheduling_period_length_sim[MAX_AS_IDS];
#define  cb_bmc_scheduling_period_length cb_bmc_scheduling_period_length_sim[cb_as_id]

extern cb_bmc_new_msg_bit_map_info_t *curr_new_msg_bitmap_sim[MAX_AS_IDS];
#define  curr_new_msg_bitmap curr_new_msg_bitmap_sim[cb_as_id]

extern cb_bmc_new_msg_bit_map_info_t *prev_new_msg_bitmap_sim[MAX_AS_IDS];
#define  prev_new_msg_bitmap prev_new_msg_bitmap_sim[cb_as_id]

extern cb_rat_type bmc_active_rat_sim[MAX_AS_IDS];
#define   bmc_active_rat bmc_active_rat_sim[cb_as_id]

dword cb_dup_timer_value_sim[MAX_AS_IDS];
#define   cb_dup_timer_value cb_dup_timer_value_sim[cb_as_id]

dword cb_next_min_offset_sim[MAX_AS_IDS];
#define   cb_next_min_offset cb_next_min_offset_sim[cb_as_id]

#elif defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
static  byte skip_archive_length_sim[MAX_AS_IDS] ={ 0 ,0};
#define skip_archive_length skip_archive_length_sim[cb_as_id]

static  dword skip_archive_sim[MAX_AS_IDS][MAX_SKIP_ARCHIVE_LENGTH] ;
#define skip_archive skip_archive_sim[cb_as_id]

extern byte cb_bmc_scheduling_period_length_sim[MAX_AS_IDS];
#define  cb_bmc_scheduling_period_length cb_bmc_scheduling_period_length_sim[cb_as_id]

extern cb_bmc_new_msg_bit_map_info_t *curr_new_msg_bitmap_sim[MAX_AS_IDS];
#define  curr_new_msg_bitmap curr_new_msg_bitmap_sim[cb_as_id]

extern cb_bmc_new_msg_bit_map_info_t *prev_new_msg_bitmap_sim[MAX_AS_IDS];
#define  prev_new_msg_bitmap prev_new_msg_bitmap_sim[cb_as_id]

extern cb_rat_type bmc_active_rat_sim[MAX_AS_IDS];
#define   bmc_active_rat bmc_active_rat_sim[cb_as_id]

dword cb_dup_timer_value_sim[MAX_AS_IDS];
#define   cb_dup_timer_value cb_dup_timer_value_sim[cb_as_id]

dword cb_next_min_offset_sim[MAX_AS_IDS];
#define   cb_next_min_offset cb_next_min_offset_sim[cb_as_id]

#else
static byte skip_archive_length = 0 ;
static dword skip_archive[MAX_SKIP_ARCHIVE_LENGTH] ;
extern byte cb_bmc_scheduling_period_length;
extern cb_bmc_new_msg_bit_map_info_t *curr_new_msg_bitmap;
extern cb_bmc_new_msg_bit_map_info_t *prev_new_msg_bitmap;
extern cb_rat_type bmc_active_rat;
dword cb_dup_timer_value;
dword cb_next_min_offset;
#endif //#ifdef FEATURE_NAS_CBS_DSDS

boolean cb_nv_duplication_detection_disabled = FALSE;

#ifdef FEATURE_UMTS_BMC
#define MAX_PDU_THRESHOLD 128
static bmc_rlc_state_type bmc_rlc_state;
word pdu_cnt = 1;
boolean bmc_rx_suspended = FALSE;
#if defined FEATURE_UMTS_BMC_DRX_SUPPORTED
extern word cb_bmc_schedule_msg_periodicity;
#endif
#endif/*FEATURE_UMTS_BMC*/


#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
extern boolean cb_supported_sim[MAX_AS_IDS];
#define cb_supported cb_supported_sim[cb_as_id]
#else
extern boolean cb_supported;
#endif

boolean cb_drx_info_validation_required = TRUE;

boolean cb_gsm_feature_disabled = FALSE;
static int cb_compare_fn(const void*, const void* );
extern boolean cb_multisim_standby;
extern dword cb_calculate_block_fn(dword reference_fn,byte  block_number);
extern void cb_cancel_scheduling(cb_channel_ind_type  channel_ind);
#ifndef FEATURE_SEGMENT_LOADING
extern void dl_dec_register_ctch_pdu_decode_error_cb( void (*cb_ptr)(dword));
extern void dl_dec_deregister_ctch_pdu_decode_error_cb(void);
#endif
/*===========================================================================

FUNCTION    cb_process_mode_change_indication

DESCRIPTION
  This function sends find whether mode is single or multimode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_process_mode_change_indication(byte val)
{
 
  byte i;
  byte subs_active=0; 
  for(i=CB_AS_ID_1;i<MAX_AS_IDS;i++)
  {
    
    if(val&(1<<i))
    {
       subs_active++;
    }
  }

  if(subs_active==0)
  {
    MSG_HIGH_DS_0(cb_as_id,"CB : Invalid subscription Status none of them are active ");
    return;
  }

  switch(subs_active)
  {
    case 1:
      cb_multisim_standby=FALSE;
      MSG_HIGH_DS_0(cb_as_id,"CB : In Single Standby  ");

      break;
    default :
        MSG_HIGH_DS_0(cb_as_id,"CB : In  Multimode Standby   ");
        cb_multisim_standby=TRUE;
  }
   

}

/*===========================================================================

FUNCTION    CB_SEND_L1_REQUEST

DESCRIPTION
  This function sends CB requests to L1 task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_send_l1_request(void * message_ptr)
{
  gs_status_T    gs_status;

  if(message_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  MSG_HIGH_DS_2(cb_as_id,"CB: Sending l1 request(%d,%d)",
    ((IMH_T *)message_ptr)->message_set,((IMH_T *)message_ptr)->message_id) ;

  gs_status = gs_send_message(
#ifdef FEATURE_DUAL_SIM
                       geran_api_get_l1_queue_id((sys_modem_as_id_e_type)cb_as_id),
#else
                       geran_api_get_l1_queue_id(0),
#endif
                       message_ptr,TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Unable to send message to layer 1");
  }
} /* end cb_send_l1_request() */

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
boolean cb_message_is_requested
(
  word incoming_message_id
)
{
  byte search_index ;

  for (search_index=0; search_index<search_list_len; search_index++)
  {
    switch(search_lst[search_index].current_message_id_type)
    {
      case CB_SINGLE_MESSAGE_ID :
        if (incoming_message_id == 
                search_lst[search_index].single_message_id.message_id)
        {
          return TRUE ;
        }
        break ;
      case CB_MESSAGE_ID_RANGE :
        if ((search_lst[search_index].message_id_range.min_message_id 
                                                     <= incoming_message_id) &&
            (search_lst[search_index].
                       message_id_range.max_message_id >= incoming_message_id))
        {
          return TRUE ;
        }
        break ;
      default:
        MSG_FATAL_DS( cb_as_id,"Wrong Message Id type identifier (%d)",
                                   (int) search_lst[search_index].current_message_id_type,0,0) ;
    }
  }

  return FALSE ;
} /* end cb_message_is_requested() */

/*===========================================================================

FUNCTION      UPWARD_SEARCH_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cb_message_list_type *upward_search_message
(
  cb_message_list_type *current_search_list_entry,
  word message_id
)
{
  if(current_search_list_entry == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  while(current_search_list_entry->next_message != NULL)
  {
    current_search_list_entry = current_search_list_entry->next_message ;

    if (current_search_list_entry->message_header.message_id == 
                                                                    message_id)
    {
      return current_search_list_entry ;
    }
  }

  return NULL ; /* Requested Message ID has not been found */
} /* end upward_search_message() */

/*===========================================================================

FUNCTION      DOWNWARD_SEARCH_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cb_message_list_type *downward_search_message
(
  cb_message_list_type *current_search_list_entry,
  word message_id
)
{
  if(current_search_list_entry == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  while(current_search_list_entry->prev_message != NULL)
  {
    current_search_list_entry = current_search_list_entry->prev_message ;

    if (current_search_list_entry->message_header.message_id == 
                                                                    message_id)
    {
      return current_search_list_entry ;
    }
  }

  return NULL ; /* Requested Message ID has not been found */
} /* end downward_search_message() */

/*===========================================================================

FUNCTION    GET_SEARCH_DIRECTION

DESCRIPTION
  This function determines the search direction 

DEPENDENCIES
  None

RETURN VALUE
  Search direction ID

SIDE EFFECTS
  None

===========================================================================*/
search_direction_type get_search_direction
(
  cb_message_list_type *current_search_list_entry,
  word new_message_id
)
{
  word current_message_id;

  if(current_search_list_entry == NULL)
  {
    return NO_SEARCH ;
  }

  current_message_id = current_search_list_entry->message_header.message_id ;

  if (current_message_id == new_message_id)
  {
    return NO_SEARCH ;
  } else if (current_message_id > new_message_id)
  {
    return DOWNWARD_SEARCH ;
  } else if (current_message_id < new_message_id)
  {
    return UPWARD_SEARCH ;
  }

  return NO_SEARCH ;

} /* end get_search_direction() */

/*===========================================================================

FUNCTION    CB_FIND_MESSAGE

DESCRIPTION
  This function searches for Message Descriptor by Message ID

DEPENDENCIES
  None

RETURN VALUE
  Pointer to Message descriptor if it exists or NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/

cb_message_list_type *cb_find_message
(
  cb_message_list_type *current_search_list_entry,
  word message_id
)
{
  if(current_search_list_entry == NULL)
  {
     return NULL;
  }
  switch (get_search_direction(current_search_list_entry,message_id))
  {
    case NO_SEARCH :
      return current_search_list_entry ;
    case UPWARD_SEARCH :
      return upward_search_message(current_search_list_entry,message_id) ;
    case DOWNWARD_SEARCH :
      return downward_search_message(current_search_list_entry,message_id) ;
    default :
      MSG_FATAL_DS( cb_as_id,"Invalid search direction identifier",0,0,0) ;
  }

  /*lint -e{527} Safe to return if ERR_FATAL does not return */
  return NULL ;

} /* end cb_find_message() */

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
cb_message_list_type *cb_put_message_into_list
(
  cb_message_list_type *current_search_list_entry,
  cb_message_list_type *new_list_entry
)
{
  if(new_list_entry == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  if (current_search_list_entry == NULL)
  {
    new_list_entry->next_message = NULL ; 
    new_list_entry->prev_message = NULL ; 
  }
  else
  {
    switch (get_search_direction(current_search_list_entry,
                                    new_list_entry->message_header.message_id))
    {
      case UPWARD_SEARCH :
        while ((current_search_list_entry->next_message != NULL) &&
               (current_search_list_entry->message_header.message_id < 
                                    new_list_entry->message_header.message_id))
        {
          current_search_list_entry = current_search_list_entry->next_message ;
        }

        break ;
      case DOWNWARD_SEARCH :
        while ((current_search_list_entry->prev_message != NULL) &&
               (current_search_list_entry->message_header.message_id >
                                    new_list_entry->message_header.message_id))
        {
          current_search_list_entry = current_search_list_entry->prev_message ;
        }

        break ;
      case NO_SEARCH:
        /* MSG_ERROR_DS_0(cb_as_id,"CB: Duplication is detected") ; */
        return current_search_list_entry ;
      default :
        MSG_FATAL_DS( cb_as_id,"Invalid search direction identifier",0,0,0) ;
    }

    if (current_search_list_entry->message_header.message_id < 
                                     new_list_entry->message_header.message_id)
    {
      new_list_entry->next_message = current_search_list_entry->next_message ; 
      if (current_search_list_entry->next_message != NULL)
      {
        current_search_list_entry->next_message->prev_message = new_list_entry ;
      }
      new_list_entry->prev_message = current_search_list_entry ; 
      current_search_list_entry->next_message = new_list_entry ;
    }
    else
    {
      new_list_entry->next_message = current_search_list_entry; 
      new_list_entry->prev_message = current_search_list_entry->prev_message ; 
      if (current_search_list_entry->prev_message != NULL)
      {
        current_search_list_entry->prev_message->next_message = new_list_entry ;
      }
      current_search_list_entry->prev_message = new_list_entry ;
    }
  }

  if (new_list_entry->prev_message == NULL)
  {
    basic_cbch_list_entry_point = new_list_entry ;
  }

  return new_list_entry ;

} /* end cb_put_message_into_list() */

/*===========================================================================

FUNCTION    CB_CREATE_MESSAGE_CODE_ENTRY

DESCRIPTION
  This function creates Message code descriptor

DEPENDENCIES
  None

RETURN VALUE
  Pointer to Message code descriptor

SIDE EFFECTS
  None

===========================================================================*/
cb_user_page_info_type *cb_create_message_code_entry
(
  cb_message_list_type *current_message_ptr,
  word                  message_code,
  byte                  gs_indicator,
  byte                  update_number,
  byte                  number_of_pages,
  byte                  data_coding_scheme
)
{
  cb_user_page_info_type *new_message_code_ptr = NULL ;

  if(current_message_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  if ((new_message_code_ptr = (cb_user_page_info_type *)
                      cb_mem_alloc(CB_USER_PAGE_INFO_ALLOC_ID,
                                      sizeof(cb_user_page_info_type))) == NULL)
  {
    return NULL ;
  }

  new_message_code_ptr->message_code = message_code ;

  new_message_code_ptr->gs_indicator = gs_indicator ;

  new_message_code_ptr->update_number = update_number ;

  new_message_code_ptr->number_of_pages = number_of_pages ;

  new_message_code_ptr->data_coding_scheme = data_coding_scheme;

  memscpy((void *)new_message_code_ptr->plmn, sizeof(new_message_code_ptr->plmn),
                                     (void *)cb_current_plmn, PLMN_VALUE_LENGTH) ;
   
  memscpy((void *)new_message_code_ptr->lac, sizeof(new_message_code_ptr->lac),
                                       (void *)current_lac, LAC_VALUE_LENGTH) ;

  memscpy((void *)new_message_code_ptr->cell_id, sizeof(new_message_code_ptr->cell_id),
                               (void *)current_cell_id, CELL_ID_VALUE_LENGTH) ;

  new_message_code_ptr->received_page_indicator = 0 ;

  new_message_code_ptr->missing_page_list = NULL ;

  new_message_code_ptr->current_page = NULL ;
  
  new_message_code_ptr->timer_offset = CB_DUP_TIMER_VALUE_INVALID;

  new_message_code_ptr->next_message_code = NULL ;

  if (current_message_ptr->message_header.message_info == NULL)
  {
    current_message_ptr->message_header.message_info = new_message_code_ptr ;
  }
  else
  {
    cb_user_page_info_type *next_message_code_ptr = 
                             current_message_ptr->message_header.message_info ;

    while (next_message_code_ptr != NULL)
    {
      if (next_message_code_ptr->next_message_code == NULL)
      {
        next_message_code_ptr->next_message_code = new_message_code_ptr ;

        break ;
      }

      next_message_code_ptr = next_message_code_ptr->next_message_code ;
    }
  }

  current_message_ptr->
             message_header.current_message_descriptor = new_message_code_ptr ;

  return new_message_code_ptr ;

} /* end cb_create_message_code_entry() */

/*===========================================================================

FUNCTION    CB_FIND_MESSAGE_CODE

DESCRIPTION
  This function searches for Message Code descriptor

DEPENDENCIES
  None

RETURN VALUE
  Pointer to existing Message Code descriptor if it exists or NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/
cb_user_page_info_type *cb_find_message_code
(
  cb_message_list_type *current_message_ptr,
  word message_code
)
{
  cb_user_page_info_type *next_message_code_ptr = NULL ;

  if(current_message_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  if (current_message_ptr->message_header.message_info == NULL)
  {
    return NULL;
  }
  else
  {
    next_message_code_ptr = current_message_ptr->message_header.message_info;

    while (next_message_code_ptr != NULL)
    {
      if (next_message_code_ptr->message_code == message_code)
      {
        break ;
      }

      next_message_code_ptr = next_message_code_ptr->next_message_code ;
    }
  }

  current_message_ptr->message_header.current_message_descriptor = next_message_code_ptr ;

  return next_message_code_ptr ;

} /* end cb_find_message_code() */

/*===========================================================================

FUNCTION    CB_FIND_MESSAGE_CODE_WITH_GS

DESCRIPTION
  This function searches for Message Code descriptor

DEPENDENCIES
  None

RETURN VALUE
  Pointer to existing Message Code descriptor if it exists or NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/
cb_user_page_info_type *cb_find_message_code_with_gs
(
  cb_message_list_type *current_message_ptr,
  word message_code
)
{
  cb_user_page_info_type *next_message_code_ptr = NULL ;
  boolean plmn_matched = FALSE ;
  boolean lac_matched = FALSE ;
  boolean cell_matched = FALSE ;
  boolean message_tobe_duplicated = FALSE;

  if(current_message_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  if (current_message_ptr->message_header.message_info == NULL)
  {
    return NULL;
  }
  else
  {
    next_message_code_ptr = current_message_ptr->message_header.message_info;

    while (next_message_code_ptr != NULL)
    {
      if (next_message_code_ptr->message_code == message_code)
      {
        plmn_matched = sys_plmn_match (*(sys_plmn_id_s_type *)cb_current_plmn, *(sys_plmn_id_s_type *)next_message_code_ptr->plmn);
        lac_matched =  (memcmp((void *)current_lac,(void *)next_message_code_ptr->lac,LAC_VALUE_LENGTH) == 0);
        cell_matched = (memcmp((void *)current_cell_id,(void *)next_message_code_ptr->cell_id,CELL_ID_VALUE_LENGTH) == 0);

        switch (next_message_code_ptr->gs_indicator)
        {
          case CB_HIGH_PRIORITY_CELL_WIDE:
          case CB_LOW_PRIORITY_CELL_WIDE:
            if (plmn_matched && lac_matched && cell_matched)
            {
              message_tobe_duplicated = TRUE ;
            }
            break ;
          case CB_PLMN_WIDE:
            if (plmn_matched)
            {
              message_tobe_duplicated = TRUE ;
            }
            break ;
          case CB_LOCATION_AREA_WIDE:
            if (plmn_matched && lac_matched)
            {
              message_tobe_duplicated = TRUE ;
            }
            break ;
          default:
             MSG_ERROR_DS_1(cb_as_id,"GS indicator is out of range (%d)",next_message_code_ptr->gs_indicator);
             break ;
        } /* switch (next_message_code_ptr->gs_indicator) */
      }

      if (message_tobe_duplicated)
      {
        break ;
      }

      message_tobe_duplicated = FALSE;
      next_message_code_ptr = next_message_code_ptr->next_message_code ;
    }
  }

  current_message_ptr->message_header.current_message_descriptor = next_message_code_ptr ;

  return next_message_code_ptr ;

} /* end cb_find_message_code_with_gs() */

/*===========================================================================

FUNCTION    CB_CREATE_MESSAGE_ENTRY

DESCRIPTION
  This function creates Message Descriptor

DEPENDENCIES
  None

RETURN VALUE
  Pointer to message descriptor wich has been created

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
cb_message_list_type *cb_create_message_entry
(
  cb_message_list_type *current_message_ptr,
  word                  message_id,
  dword                 received_fn
)
{
  cb_message_list_type *new_message_ptr = NULL ;

  if ((new_message_ptr = (cb_message_list_type *)
          cb_mem_alloc(CB_MESSAGE_LIST_ALLOC_ID,
                                        sizeof(cb_message_list_type))) == NULL)
  {
    return NULL ;
  }

  memset(new_message_ptr,0x00,sizeof(cb_message_list_type));

  new_message_ptr->message_header.message_id = message_id ;

  new_message_ptr->next_message = NULL ;

  new_message_ptr->prev_message = NULL ;

  new_message_ptr->message_header.fn = received_fn ;

  new_message_ptr->message_header.current_message_descriptor = NULL ;

  new_message_ptr->message_header.message_info = NULL ;

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)

  new_message_ptr->message_header.next_first_tx_schedule = NULL ;

  new_message_ptr->message_header.first_tx_schedule = NULL ;

  new_message_ptr->message_header.retx_schedule = NULL ;

#endif /* DRX */

  return new_message_ptr ;
} /* end cb_create_message_entry() */
/*lint +e715*/

/*===========================================================================

FUNCTION CB_TIMER_EXPIRY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_timer_expiry(unsigned long param)
{
   cb_command_body_type    *cmd_ptr;

#ifndef FEATURE_MODEM_HEAP
    if ((cmd_ptr = gs_alloc(sizeof(cb_command_body_type))) == NULL)
#else
    if ((cmd_ptr = modem_mem_calloc(1,sizeof(cb_command_body_type), MODEM_MEM_CLIENT_NAS)) == NULL)
#endif
    {
      MSG_FATAL_DS( cb_as_id,"CB: Not enough memory",0,0,0) ;
    }

    /* Construct message */
   cmd_ptr->message_header.message_set = MS_TIMER;
   cmd_ptr->message_header.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->timer_expiry) - sizeof (cmd_ptr->timer_expiry.IMH),
                     &(cmd_ptr->timer_expiry.IMH));

   cmd_ptr->timer_expiry.data        = (byte)(param & 0x0000FFFF);
   cmd_ptr->timer_expiry.timer_id    = (byte)(param >> 16);

   /* Put on CB command queue */
   cb_put_cmd(cmd_ptr);
}


#if defined (FEATURE_WCDMA_ETWS) && defined (FEATURE_UMTS_BMC)
/*===========================================================================

FUNCTION    IS_ETWS_MESSAGE_ID

DESCRIPTION  

  This function returns TRUE when the message id passed to this function is valid ETWS message ID
  else it will return FALSE


DEPENDENCIES
  None

RETURN VALUE
  Message Id passed is valid ETWS message ID          - return TRUE
  Message Id passed is not a valid ETWS message ID  - return FALSE

SIDE EFFECTS
  None

===========================================================================*/

static boolean is_etws_message_id(cb_wms_message_id_value_type list, boolean single_id)
{
  boolean found_in_etws_list =FALSE;
  if( single_id)
  {
    if(list.single_message_id.message_id == CB_BMC_ETWS_EARTQUAKE_ID ||
       list.single_message_id.message_id == CB_BMC_ETWS_TSUNAMI_ID   ||
       list.single_message_id.message_id == CB_BMC_ETWS_EARTQUAKE_AND_TSUNAMI_ID   ||
       list.single_message_id.message_id == CB_BMC_ETWS_TEST_ID   ||
       list.single_message_id.message_id == CB_BMC_ETWS_OTHER_EMERGENCY_ID   ||
       list.single_message_id.message_id == CB_BMC_ETWS_FUTURE_EXTENSION_01   ||
       list.single_message_id.message_id == CB_BMC_ETWS_FUTURE_EXTENSION_02   ||
       list.single_message_id.message_id == CB_BMC_ETWS_FUTURE_EXTENSION_03 )
    {
      found_in_etws_list = TRUE;
    }
  }
  else
  {
    if((list.message_id_range.min_message_id >= CB_BMC_ETWS_EARTQUAKE_ID) &&
       (list.message_id_range.min_message_id <= CB_BMC_ETWS_FUTURE_EXTENSION_03) &&
        (list.message_id_range.max_message_id >= CB_BMC_ETWS_EARTQUAKE_ID) &&
         (list.message_id_range.max_message_id <= CB_BMC_ETWS_FUTURE_EXTENSION_03) &&
          (list.message_id_range.max_message_id >= list.message_id_range.min_message_id))
    {
      found_in_etws_list = TRUE;
    }
  }
  return found_in_etws_list;
}

/*===========================================================================

FUNCTION    NUMBER_OF_ETWS_IDS_IN_SEARCH_LIST

DESCRIPTION  

This function counts the total number of ETWS IDs in the search list requested from WMS

DEPENDENCIES
  None

RETURN VALUE
  Number of ETWS message ids in the search list

SIDE EFFECTS
  None

===========================================================================*/
byte number_of_etws_ids_in_search_list
(
  void
)
{
  byte search_index ;
  byte etws_id_count=0;

  for (search_index=0; search_index<search_list_len; search_index++)
  {
    switch(search_lst[search_index].current_message_id_type)
    {
      case CB_SINGLE_MESSAGE_ID :
        if (is_etws_message_id (search_lst[search_index], TRUE))
        {
          etws_id_count+=1;
        }
        break ;
      case CB_MESSAGE_ID_RANGE :
        if(is_etws_message_id (search_lst[search_index], FALSE))
        {
          etws_id_count+=1;
        }
        break ;
      default:
        MSG_FATAL_DS( cb_as_id,"Wrong Message Id type identifier (%d)",
                                   (int) search_lst[search_index].current_message_id_type,0,0) ;
    }
  }
  MSG_HIGH_DS_2(cb_as_id," ETWS entries are %d (%d) ",etws_id_count,search_list_len);
  return etws_id_count ;
} /* end cb_message_is_requested() */

/*===========================================================================

FUNCTION CB_ETWS_STOP_TIMER

DESCRIPTION
  This function stops the CB ETWS timer specified by timer_id


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_stop_etws_timer( timer_id_T timer_id )
{
   gs_status_T gs_status;
   if (cb_timer_status[timer_id-1] != TIMER_STOPPED)
   {
      MSG_HIGH_DS_1(cb_as_id, "Stopping %d", timer_id);

      gs_status = gs_cancel_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(timer_id) );

      if (gs_status != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(cb_as_id,"Stop timer failure");
      }
      cb_timer_status[timer_id-1] = TIMER_STOPPED;
   }
 }
/*===========================================================================

FUNCTION MM_START_TIMER

DESCRIPTION
  This function starts the CB ETWS timer specified by timer_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_start_etws_timer( timer_id_T timer_id, dword num_millisecs )
{
   gs_status_T    status;

   if (num_millisecs == 0)
   {
     MSG_HIGH_DS_1(cb_as_id,"Cannot Start Timer %d with zero time out value",timer_id);
     return;
   }

   status = gs_start_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(timer_id), cb_timer_expiry, num_millisecs, (byte) 0 );

   cb_timer_status [timer_id-1]= TIMER_ACTIVE;

   MSG_HIGH_DS_3(cb_as_id, "Start %d, timeout %u:%u",
             timer_id, (word) (num_millisecs /1000L), (word)(num_millisecs % 1000L) );


   if ( status != GS_SUCCESS )
   {
      MSG_ERROR_DS_1(cb_as_id,"Start timer failure: Status %d", status);
   }
}

/*===========================================================================

FUNCTION    CB_FIND_ETWS_MESSAGE

DESCRIPTION
  This finds if the message id requested is already recieved by CB

DEPENDENCIES
  None

RETURN VALUE
  Pointer to message descriptor wich has been created

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
cb_bmc_etws_msg_list_type *cb_find_etws_message
(
  cb_bmc_etws_msg_list_type *current_search_list_entry,
  word message_id
)
{
  while(current_search_list_entry != NULL)
  {
    if (current_search_list_entry->message_header.message_id == message_id)
    {
      return current_search_list_entry ;
    }
    current_search_list_entry = current_search_list_entry->next_message;
  }
  /*lint -e{527} Safe to return if ERR_FATAL does not return */
  return NULL ;
} /* end cb_create_etws_primary_message_entry() */

/*===========================================================================

FUNCTION      CB_DELETE_MESSAGE_FROM_ETWS_LIST

DESCRIPTION
  This function removes the specific message descriptor from a Message 
  Descriptor list

DEPENDENCIES
  None

RETURN VALUE
  Pointer into CB Message Descriptor List which is pointing out the next valid
  Message descriptor, NULL if list is empty

SIDE EFFECTS
  None

===========================================================================*/
static cb_bmc_etws_msg_list_type *cb_delete_message_from_etws_list
(
  cb_bmc_etws_msg_list_type *message_tobe_deleted
)
{

  if (message_tobe_deleted != NULL)
  {
    if(message_tobe_deleted == basic_etws_list_entry_point)
    {
      basic_etws_list_entry_point = message_tobe_deleted->next_message ;
#ifndef FEATURE_MODEM_HEAP
      gs_free((void *)message_tobe_deleted);
#else
      modem_mem_free((void *)message_tobe_deleted, MODEM_MEM_CLIENT_NAS);
#endif
      return basic_etws_list_entry_point;
    }
    else
    {
      basic_etws_list = basic_etws_list_entry_point;
      while(basic_etws_list != NULL)
      {
        if(basic_etws_list->next_message->message_header.message_id == message_tobe_deleted->message_header.message_id)
        {
          basic_etws_list->next_message = message_tobe_deleted->next_message;
          break;
        }
        else
        {
          basic_etws_list = basic_etws_list->next_message;
        }
      }
#ifndef FEATURE_MODEM_HEAP
      gs_free((void *)message_tobe_deleted);
#else
      modem_mem_free((void *)message_tobe_deleted, MODEM_MEM_CLIENT_NAS);
#endif
      return basic_etws_list;
    }
  }
  return basic_etws_list_entry_point;
} /* end cb_delete_message_from_list() */

/*===========================================================================

FUNCTION      CB_DELETE_ETWS_MESSAGE_CODE

DESCRIPTION
  This function removes the specific message descriptor from a Message 
  Descriptor list

DEPENDENCIES
  None

RETURN VALUE
  Pointer into CB Message Descriptor List which is pointing out the next valid
  Message descriptor, NULL if list is empty

SIDE EFFECTS
  None

===========================================================================*/

static cb_bmc_etws_msg_list_type *cb_delete_etws_message_code
(
  cb_bmc_etws_msg_list_type *current_message_param,
  cb_bmc_etws_msg_info_type *cb_message_code_tobe_deleted
)
{
  cb_bmc_etws_msg_info_type *root_message_code ;

  if (current_message_param == NULL || cb_message_code_tobe_deleted == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  root_message_code = current_message_param->message_header.message_info;

  if ((root_message_code == cb_message_code_tobe_deleted) && 
                     (cb_message_code_tobe_deleted->next_message_code == NULL))
  {
    current_message_param = cb_delete_message_from_etws_list(current_message_param) ;
  }
  else
  {
    current_message_param->message_header.message_info = 
                              cb_message_code_tobe_deleted->next_message_code;
  }
#ifndef FEATURE_MODEM_HEAP
  gs_free((void *)cb_message_code_tobe_deleted);
#else
  modem_mem_free((void *)cb_message_code_tobe_deleted, MODEM_MEM_CLIENT_NAS);
#endif

  return current_message_param ;
} /* end cb_delete_message_code() */
/*===========================================================================

FUNCTION      CB_FILTER_OUT_OBSOLETE_ID_FROM_ETWS_LIST

DESCRIPTION
  This function filters out the CB message descriptors from a specific list 
  which are not longer included into new Search List and shall be discarded

DEPENDENCIES
  None

RETURN VALUE
  Pointer into CB Message Descriptor List 

SIDE EFFECTS
  None

===========================================================================*/
cb_bmc_etws_msg_list_type *cb_filter_timeout_primary_etws_msg_from_list
(
  cb_bmc_etws_msg_list_type **first_message_entry,
  word                            message_id
  
)
{
  cb_bmc_etws_msg_info_type *cb_received_message_code = NULL ;
  cb_bmc_etws_msg_list_type *list = NULL ;
  cb_bmc_etws_msg_list_type *curr_entry = NULL;
  cb_bmc_etws_msg_list_type *cur_message = NULL ;

  if (first_message_entry == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  list = *first_message_entry;

  if (list == NULL)
  {
    return NULL ; /* Nothing to do */
  }

  if ((curr_entry=cb_find_etws_message(list, message_id))!= NULL)
  {
     cb_received_message_code = curr_entry->message_header.message_info;

     while (cb_received_message_code != NULL)
     {
        MSG_HIGH_DS_2(cb_as_id,"CB: ETWS Message (%d, %d) is obsolete and filtered out",
                  curr_entry->message_header.message_id,curr_entry->message_header.message_info->message_code) ;
          
        cur_message = cb_delete_etws_message_code(curr_entry,curr_entry->message_header.message_info) ;

        if (curr_entry != cur_message)
        {
          curr_entry = cur_message ;

          break ;
        }
        else
        {
          cb_received_message_code = curr_entry->message_header.message_info;
        }
     }
  } /* if (!cb_message_is_requested(list->message_header.message_id)) */
  return cur_message ;
} /* end cb_filter_out_obsolete_id_from_list() */

/*===========================================================================

FUNCTION      CB_FILTER_OUT_OBSOLETE_ID_FROM_ETWS_LIST

DESCRIPTION
  This function filters out the CB message descriptors from a specific list 
  which are not longer included into new Search List and shall be discarded

DEPENDENCIES
  None

RETURN VALUE
  Pointer into CB Message Descriptor List 

SIDE EFFECTS
  None

===========================================================================*/
static cb_bmc_etws_msg_list_type *cb_filter_out_obsolete_id_from_etws_list
(
  cb_bmc_etws_msg_list_type **first_message_entry
)
{
  cb_bmc_etws_msg_info_type *cb_received_message_code = NULL ;
  cb_bmc_etws_msg_list_type *list = NULL ;
  boolean next_message_ptr_aready_exist = FALSE ;
  cb_bmc_etws_msg_list_type *cur_message = NULL ;
  boolean timer_offset_0_deleted = FALSE ;

  if (first_message_entry == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  list = *first_message_entry;

  if (list == NULL)
  {
    return NULL ; /* Nothing to do */
  }

  while (list != NULL)
  {
    next_message_ptr_aready_exist = FALSE ;
    if (!cb_message_is_requested(list->message_header.message_id))
    {
      cb_received_message_code = list->message_header.message_info;
      
      while (cb_received_message_code != NULL)
      {
        MSG_HIGH_DS_2(cb_as_id,"CB: ETWS Message (%d, %d) is obsolete and filtered out",
                   list->message_header.message_id,list->message_header.message_info->message_code) ;

        if(cb_received_message_code->timer_offset == 0)
        {
            timer_offset_0_deleted = TRUE;
        }
          
        cur_message = cb_delete_etws_message_code(list,list->message_header.message_info) ;

        if (list != cur_message)
        {
          list = cur_message ;

          next_message_ptr_aready_exist = TRUE ;

          break ;
        }
        else
        {
           cb_received_message_code = list->message_header.message_info;
        }
      } /* while (cb_received_message_code != NULL) */
    } /* if (!cb_message_is_requested(list->message_header.message_id)) */

    if (!next_message_ptr_aready_exist)
    {
      list = list->next_message ;
    }
  }
  cb_update_cache_for_multiple_message_deletion(timer_offset_0_deleted);
  return cur_message ;
} /* end cb_filter_out_obsolete_id_from_list() */

/*===========================================================================

FUNCTION    CB_CREATE_ETWS_PRIMARY_MESSAGE_ENTRY

DESCRIPTION
  This function creates Message Descriptor

DEPENDENCIES
  None

RETURN VALUE
  Pointer to message descriptor wich has been created

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
cb_bmc_etws_msg_list_type *cb_create_etws_primary_message_entry
(
  word                  message_id
)
{
  cb_bmc_etws_msg_list_type *new_message_ptr = NULL ;

#ifdef FEATURE_MODEM_HEAP
  if ((new_message_ptr = (cb_bmc_etws_msg_list_type *)
          modem_mem_calloc(1,sizeof(cb_bmc_etws_msg_list_type),MODEM_MEM_CLIENT_NAS)) == NULL)
#else
  if ((new_message_ptr = (cb_bmc_etws_msg_list_type *)
          gs_alloc(sizeof(cb_bmc_etws_msg_list_type))) == NULL)
#endif
  {
    return NULL ;
  }
  if(basic_etws_list_entry_point == NULL)
  {
    basic_etws_list_entry_point = new_message_ptr;
  }

  new_message_ptr->message_header.message_id = message_id ;

  new_message_ptr->message_header.message_info = NULL ;

  new_message_ptr->next_message = NULL ;

  return new_message_ptr ;
} /* end cb_create_etws_primary_message_entry() */
/*===========================================================================

FUNCTION    CB_CREATE_ETWS_MESSAGE_CODE_ENTRY

DESCRIPTION
  This function creates Message code descriptor

DEPENDENCIES
  None

RETURN VALUE
  Pointer to Message code descriptor

SIDE EFFECTS
  None

===========================================================================*/
cb_bmc_etws_msg_info_type *cb_create_etws_message_code_entry
(
  cb_bmc_etws_msg_list_type  *current_message_ptr,
  word                message_code,
  byte                gs_indicator,
  byte                update_number,
  const byte*         incoming_plmn
)
{
  cb_bmc_etws_msg_info_type *new_message_code_ptr = NULL ;

  if(current_message_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

#ifdef FEATURE_MODEM_HEAP
  if ((new_message_code_ptr = (cb_bmc_etws_msg_info_type *)
       modem_mem_calloc(1,sizeof(cb_bmc_etws_msg_info_type),MODEM_MEM_CLIENT_NAS)) == NULL)
#else
  if ((new_message_code_ptr = (cb_bmc_etws_msg_info_type *)
       gs_alloc(sizeof(cb_bmc_etws_msg_info_type))) == NULL)
#endif
  {
    return NULL ;
  }

  memscpy((void *)new_message_code_ptr->plmn, sizeof(new_message_code_ptr->plmn),
                                     (void *)incoming_plmn, PLMN_VALUE_LENGTH) ;

  new_message_code_ptr->message_code   = message_code;
  new_message_code_ptr->gs_indicator   = gs_indicator;
  new_message_code_ptr->update_number  = update_number;
  new_message_code_ptr->timer_offset   = CB_DUP_TIMER_VALUE_INVALID;
  new_message_code_ptr->next_message_code = NULL;

  if (current_message_ptr->message_header.message_info == NULL)
  {
    current_message_ptr->message_header.message_info = new_message_code_ptr ;
  }
  else
  {
    cb_bmc_etws_msg_info_type *next_message_code_ptr = 
                             current_message_ptr->message_header.message_info ;

    while (next_message_code_ptr != NULL)
    {
      if (next_message_code_ptr->next_message_code == NULL)
      {
        next_message_code_ptr->next_message_code= new_message_code_ptr ;

        break ;
      }

      next_message_code_ptr = next_message_code_ptr->next_message_code;
    }
  }



  return new_message_code_ptr ;

} /* end cb_create_message_code_entry() */
/*===========================================================================

FUNCTION    CB_PUT_ETWS_MESSAGE_INTO_LIST

DESCRIPTION
  This function inserts new message descriptor into specific Message List

DEPENDENCIES
  None

RETURN VALUE
  Pointer which points out to the inserted Message Descriptor

SIDE EFFECTS
  None

===========================================================================*/
cb_bmc_etws_msg_list_type *cb_put_etws_message_into_list
(
  cb_bmc_etws_msg_list_type *current_search_list_entry,
  cb_bmc_etws_msg_list_type *new_list_entry
)
{
  if(new_list_entry == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  if (current_search_list_entry == NULL)
  {
    new_list_entry->next_message = NULL ; 
  }
  else
  {
    while(current_search_list_entry != NULL)
    {

      if (current_search_list_entry->message_header.message_id ==
          new_list_entry->message_header.message_id)
      {
         /*Duplicate message ID receieved*/
         return current_search_list_entry;
      }
      if ((current_search_list_entry->message_header.message_id < 
          new_list_entry->message_header.message_id))
      {
        new_list_entry->next_message = current_search_list_entry->next_message;
        current_search_list_entry->next_message = new_list_entry;
      }
      else if(current_search_list_entry->next_message == NULL)
      {
        /*reached end of list and hence return*/
        current_search_list_entry->next_message = new_list_entry;
        break;
      }
      current_search_list_entry = current_search_list_entry->next_message;
    }
  }
  return new_list_entry ;
} /* end cb_put_message_into_list() */

/*===========================================================================

FUNCTION    CB_ETWS_FIND_MESSAGE_CODE

DESCRIPTION
  This function searches for Message Code descriptor

DEPENDENCIES
  None

RETURN VALUE
  Pointer to existing Message Code descriptor if it exists or NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/
cb_bmc_etws_msg_info_type *cb_etws_find_message_code
(
  cb_bmc_etws_msg_list_type *current_message_ptr,
  word message_code
)
{
  cb_bmc_etws_msg_info_type *next_message_code_ptr = NULL ;

  if(current_message_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  if (current_message_ptr->message_header.message_info == NULL)
  {
    return NULL ;
  }
  else
  {
    next_message_code_ptr = current_message_ptr->message_header.message_info ;

    while (next_message_code_ptr != NULL)
    {
      if (next_message_code_ptr->message_code == message_code)
      {
        break ;
      }

      next_message_code_ptr = next_message_code_ptr->next_message_code;
    }
  }

  
  return next_message_code_ptr ;

} /* end cb_find_message_code() */
#endif
cb_channel_ind_type cb_calc_channel_id
(
  byte skip_list_length,
  dword *skip_list
)
{
  byte index ;
  cb_channel_ind_type channel_ind = CBCH ;
  boolean cbch_tobe_skiped = FALSE ;
  boolean ecbch_tobe_skiped = FALSE ;

  if (skip_list_length == 0)
  {
    MSG_HIGH_DS_0(cb_as_id,"Skip request is empty") ;

    return CBCH ;
  }

  if (skip_list == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  for (index=0; index<skip_list_length; index++)
  {
    switch (CB_CALCULATE_PAGING_SLOT(skip_list[index]))
    {
      case CB_FIRST_BLOCK:
      case CB_SECOND_BLOCK:
      case CB_THIRD_BLOCK:
      case CB_FOURTH_BLOCK:
        cbch_tobe_skiped = TRUE ;
        break ;
      case CB_FIFTH_BLOCK:
      case CB_SIXTH_BLOCK:
      case CB_SEVENTH_BLOCK:
      case CB_EIGHTH_BLOCK:
        ecbch_tobe_skiped = TRUE ;
        break ;
      default:

        MSG_HIGH_DS_0(cb_as_id,"Invalid FN is to be skiped") ;

        cbch_tobe_skiped = TRUE ;

        break ;
    }
  }

  if (cbch_tobe_skiped && ecbch_tobe_skiped)
  {
    channel_ind = CBCH_ECBCH ;
  }
  else if (cbch_tobe_skiped)
  {
    channel_ind = CBCH ;
  }
  else if (ecbch_tobe_skiped)
  {
    channel_ind = ECBCH ;
  }

  return channel_ind ;

} /* end cb_calc_channel_id() */

/*===========================================================================

FUNCTION      CB_PROCESS_SKIP_LIST

DESCRIPTION
  This function searches for skip requests which have already been sent to L1
  in order to avoid unnecessary traffic between CB and L1 tasks

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte cb_process_skip_list
(
  byte skip_list_length,
  dword *skip_list
)
{
  byte skip_index, archive_index ;
  byte index_entry_point ;

  if (skip_list_length > 0)
  {
    index_entry_point = 0 ;

    if (skip_list == NULL)
    {
      MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
    }

    do
    {
      for (skip_index=index_entry_point; 
                                     skip_index<skip_list_length; skip_index++)
      {
        for (archive_index=0;archive_index<skip_archive_length;archive_index++)
        {
          if (skip_list[skip_index] == skip_archive[archive_index])
          {
            memsmove((void *)&skip_list[skip_index], sizeof(dword)*(skip_list_length-(skip_index+1)),
                          (void *)&skip_list[skip_index+1],
                             sizeof(dword)*(skip_list_length-(skip_index+1))) ;

            skip_list_length--;

            break;
          }
        }
        if((archive_index<skip_archive_length))
        {
          break;
        }
      }
      index_entry_point = skip_index ;

    } while (skip_index<skip_list_length) ;

    if ((skip_archive_length + skip_list_length) >= MAX_SKIP_ARCHIVE_LENGTH)
    {
      MSG_HIGH_DS_0(cb_as_id,"CB: Reseting skip_archive[]") ;
      
      skip_archive_length = 0 ;
    }
    if(skip_archive_length < (MAX_SKIP_ARCHIVE_LENGTH - skip_list_length))
    {
      memscpy((void *)&skip_archive[skip_archive_length], sizeof(skip_archive)-skip_archive_length,
                          (void *)&skip_list[0],sizeof(dword)*skip_list_length) ;
      skip_archive_length += skip_list_length ;
    }
    else
    {
      MSG_ERROR_DS_0(cb_as_id,"Index out of bounds and buffer overflow");
      skip_list_length = 0;
    }
  }

  return skip_list_length ;
} /* end cb_process_skip_list() */

/*===========================================================================

FUNCTION    CB_PROCESS_SKIP_ARCHIVE

DESCRIPTION
  This function removes obsolete entries from Skip Archive 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_process_skip_archive(dword received_fn)
{
  byte archive_index ;

  for (archive_index=0;archive_index<skip_archive_length;archive_index++)
  {
    if (received_fn <= skip_archive[archive_index])
    {
      break ;
    }
  }

  if (archive_index<skip_archive_length)
  {
    skip_archive_length -= (archive_index+1) ;

    if(skip_archive_length  < (MAX_SKIP_ARCHIVE_LENGTH - archive_index) )
    {
      memsmove((void *)&skip_archive[0],sizeof(skip_archive),
                    (void *)&skip_archive[archive_index+1],
                                             sizeof(dword)*skip_archive_length) ;
    }
    else
    {
      MSG_ERROR_DS_0(cb_as_id,"Index out of bounds and buffer overflow");
      skip_archive_length = 0 ;
    }
  }
  else
  {
    skip_archive_length = 0 ;
  }

} /* end cb_process_skip_archive() */

/*===========================================================================

FUNCTION CB_SEND_SKIP_REQUEST

DESCRIPTION
  This function sends SKIP request to L1

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_send_skip_request
(
  cb_skip_ind_type skip_type,
  byte skip_list_length,
  dword *skip_list
)
{

  cb_skip_req_type skip_req ;
  byte skip_index ;

  memset(&skip_req, 0, sizeof(skip_req));
  if(CB_ERASE_LAST_SCHEDULING != skip_type)
  {
    if (skip_list == NULL)
    {
      MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
    }

  #ifdef FEATURE_GSM_CB_PSEUDO_DRX
    /* check if next FN is where pseudo DRX is supposed to start */
    if (cb_pseudo_drx_supported)
    {
      for (skip_index=0; skip_index<skip_list_length; skip_index++)
      {
        if ((cb_pseudo_drx_start_fn != BMC_INVALID_SFN) && 
             (cb_calculate_block_fn(skip_list[skip_index],CB_CHANNEL_CYCLE-CB_FOURTH_BLOCK)
                 == cb_pseudo_drx_start_fn))
        {
          cb_pseudo_drx_prev_start_fn = BMC_INVALID_SFN;
          cb_active_substate_control(CB_STD_PSEUDO_DRX_ACTIVATED);
          break;
        }
      }
    }
  #endif

    if ((skip_list_length=cb_process_skip_list(skip_list_length,skip_list)) == 0)
    {
      cb_mem_free(skip_list) ;

      MSG_HIGH_DS_0(cb_as_id,"CB: CB_SKIP_REQ has been stopped") ;

      return ;
    }
  }
  skip_req.message_header.message_set = MS_CB_L1 ;
  skip_req.message_header.message_id = (int)CB_SKIP_REQ ;

  skip_req.skip_ind = skip_type ;
  skip_req.scheduling_descriptor.scheduling_period_length = skip_list_length ;
  skip_req.scheduling_descriptor.scheduling_period_fn_list = skip_list ;

  skip_req.channel_ind = cb_calc_channel_id(skip_list_length, skip_list) ;

#ifdef FEATURE_DUAL_SIM
  skip_req.as_id = (sys_modem_as_id_e_type) cb_as_id;
#endif

  PUT_IMH_LEN( sizeof(cb_skip_req_type) - 
                                    sizeof(IMH_T),&skip_req.message_header ) ;
  /*Sorting */
  skip_list = cb_sort_list(skip_list, skip_list_length);

  MSG_HIGH_DS_1(cb_as_id,"CB: CB_SKIP_REQ->length == %d",skip_list_length) ;

  for (skip_index=0; skip_index<skip_list_length; skip_index++)
  {
    MSG_HIGH_DS_2(cb_as_id,"CB: CB_SKIP_REQ->FN[%d] == %d",skip_index,skip_list[skip_index]) ;
  }

  MSG_HIGH_DS_0(cb_as_id,"CB: Sending CB_SKIP_REQ request") ;

  cb_send_l1_request((void *)&skip_req) ;
} /* end cb_send_skip_request() */


/*===========================================================================

FUNCTION    CB_FIND_EXISTING_PAGE_BUFFER

DESCRIPTION
  This function searches for existing Page Buffer

DEPENDENCIES
  None

RETURN VALUE
  Pointer to existing Page Buffer if buffer exists or NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/
cb_missing_page_type *cb_find_existing_page_buffer
(
  cb_missing_page_type *current_page,
  byte page_number
)
{
  if (current_page == NULL)
  {
    return NULL ;
  }

  do
  {
    if (current_page->page_number == page_number)
    {
      return current_page ;
    }
    else // if (current_page->next_missing_page != NULL)
    {
      current_page = current_page->next_missing_page ;
    }

  } while(current_page != NULL) ;

  return NULL ;
} /* end cb_find_existing_page_buffer() */

/*===========================================================================

FUNCTION cb_put_page_into_list

DESCRIPTION
  This function puts new page buffer into Page Buffer list

DEPENDENCIES
  None

RETURN VALUE
  Page Buffer list

SIDE EFFECTS
  None

===========================================================================*/
void cb_put_page_into_list
(
  cb_message_list_type *message,
  cb_missing_page_type *first_page,
  cb_missing_page_type *new_incoming_page
)
{

  if (first_page == NULL)
  {
    if (message == NULL)
    {
      MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
    }
    message->message_header.
            current_message_descriptor->missing_page_list = new_incoming_page ;
  }
  else
  {
    while (first_page->next_missing_page != NULL) 
    {
      first_page = first_page->next_missing_page ;
    }

    first_page->next_missing_page = new_incoming_page ;
  }
} /* end cb_put_page_into_list() */

/*===========================================================================

FUNCTION      CB_CREATE_PAGE_DESCRIPTOR

DESCRIPTION
  This function creates new Page buffer

DEPENDENCIES
  None

RETURN VALUE
  Page buffer pointer

SIDE EFFECTS
  None

===========================================================================*/
cb_missing_page_type *cb_create_page_descriptor
(
  cb_rat_type active_rat,
  cb_message_list_type *message,
  cb_missing_page_type *first_page,
  byte                  page_number
)
{
  cb_missing_page_type *new_incoming_page = NULL ;

  if ((new_incoming_page = (cb_missing_page_type *)
            cb_mem_alloc(CB_MISSING_PAGE_ALLOC_ID,
                               sizeof(cb_missing_page_type))) == NULL)
  {
    return NULL ;
  }

  new_incoming_page->received_rat = active_rat ;

  new_incoming_page->
                  rat_page_properties.gsm_propertie.already_skiped_blocks = 0 ;

  new_incoming_page->
        rat_page_properties.gsm_propertie.missing_paging_blocks = 
                                                       (int)CB_MISSING_EVERYTHING ;

  new_incoming_page->next_missing_page = NULL ;

  new_incoming_page->page_length = 0 ;

  new_incoming_page->page_number = page_number ;

  cb_put_page_into_list(message,first_page,new_incoming_page) ;

  return new_incoming_page ;
} /* end cb_create_page_descriptor() */

/*===========================================================================

FUNCTION      CB_DELETE_MESSAGE

DESCRIPTION
  This function removes the specific message descriptor from a Message 
  Descriptor list

DEPENDENCIES
  None

RETURN VALUE
  Pointer into Page Buffer List which is pointing out the next valid Page 
  buffer, NULL if list is empty

SIDE EFFECTS
  None

===========================================================================*/
cb_message_list_type *cb_delete_message
(
  cb_message_list_type *message_tobe_deleted
)
{
  if (message_tobe_deleted != NULL)
  {
    cb_message_list_type *left_message = message_tobe_deleted->prev_message ;
    cb_message_list_type *right_message = message_tobe_deleted->next_message ;

    if (left_message != NULL)
    {
      left_message->next_message = right_message ;
    }

    if (right_message != NULL)
    {
      right_message->prev_message = left_message ;
    }

    cb_mem_free(message_tobe_deleted) ;

    if (left_message != NULL)
    {
      return left_message ;
    }
    else if (right_message != NULL)
    {
      return right_message ;
    }
    else
    {
      return NULL ;
    }
  }
  else
  {
    return NULL ;
  }

} /* end cb_delete_message() */

/*===========================================================================

FUNCTION      CB_SEND_WMS_DATA_IND

DESCRIPTION
  This function sends CB DATA INDICATION to WMS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_send_wms_data_ind
(
  byte  page_length,
  byte *page_buffer
)
{
  cb_data_ind_type data_ind ;


#ifdef FEATURE_WMS_CM_NAS_INTERFACES
  #error code not present
#else
  data_ind.message_header.message_set = MS_WMS_CB ;
  data_ind.message_header.message_id = (int)CB_DATA_IND ;
#endif

  if (page_length > 88)
  {
    page_length = 88 ;

    MSG_HIGH_DS_0(cb_as_id,"CB: Message page has been truncated") ;
  }

  data_ind.page_length = page_length ;

  memscpy((void *)data_ind.cb_page, sizeof(data_ind.cb_page),(void *)page_buffer,page_length) ;

#ifdef FEATURE_DUAL_SIM
  data_ind.as_id = (sys_modem_as_id_e_type) cb_as_id;
#endif

  PUT_IMH_LEN( sizeof(cb_data_ind_type) - sizeof(IMH_T),
                                                  & data_ind.message_header ) ;

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
  #error code not present
#else
  if (gs_send_message(GS_QUEUE_UASMS, (void*)&data_ind, TRUE) != GS_SUCCESS)
#endif
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Failed to send CB data indication to WMS");
  }
  else
  {
    MSG_HIGH_DS_0(cb_as_id,"CB: Sent CB data indication to WMS") ;
  }

} /* end cb_send_wms_data_ind() */

#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
/*===========================================================================

FUNCTION      CB_SEND_WMS_ETWS_PRIM_NOTIFICATION_IND

DESCRIPTION
  This function sends CB ETWS PRIMARY NOTIFICATION INDICATION to WMS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cb_send_wms_etws_prim_notification_ind
(
  const sys_plmn_id_s_type* plmn,
  word                      message_id,
  word                      serial_number,
  const byte*               warning_type,
  boolean                   warning_security_info_present,
  const byte*               warning_security_info
)
{
  cb_etws_prim_notification_ind_type etws_prim_notification_ind;

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#else
  etws_prim_notification_ind.message_header.message_set = MS_WMS_CB ;
  etws_prim_notification_ind.message_header.message_id  = (int)CB_ETWS_PRIM_NOTIFICATION_IND ;
#endif
  /*
  **Construct cb_etws_prim_notification_ind message contents and
  **send the message to WMS
  */
#ifdef FEATURE_DUAL_SIM
  etws_prim_notification_ind.as_id=(sys_modem_as_id_e_type)cb_as_id;
#endif 
  etws_prim_notification_ind.message_id = message_id;
  etws_prim_notification_ind.serial_number = serial_number;
  memscpy((void*)&etws_prim_notification_ind.plmn_id.identity[0],sizeof(etws_prim_notification_ind.plmn_id.identity) , (void*)&plmn->identity[0],PLMN_VALUE_LENGTH);
  memscpy((void*)&etws_prim_notification_ind.warning_type[0], CB_ETWS_WARN_TYPE_LEN,warning_type,RRC_ETWS_WARN_TYPE_LEN);
  etws_prim_notification_ind.warning_security_info_present = warning_security_info_present;
  if(etws_prim_notification_ind.warning_security_info_present)
  {
    memscpy((void*)&etws_prim_notification_ind.warning_security_info[0],CB_ETWS_WARN_SEC_INFO_LEN,warning_security_info,RRC_ETWS_WARN_SEC_INFO_LEN);
  }
  cb_send_ota_msg_log_packet((uint8)CB_UMTS_MODE,
             RRC_ETWS_WARN_SEC_INFO_LEN,(uint8)CB_NORMAL_PAGE,
                                                  &etws_prim_notification_ind.warning_security_info[0]);
  PUT_IMH_LEN( sizeof(cb_etws_prim_notification_ind_type) - sizeof(IMH_T),
                                                  &etws_prim_notification_ind.message_header ) ;

  if (gs_send_message(GS_QUEUE_UASMS, (void*)&etws_prim_notification_ind, TRUE) != GS_SUCCESS)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB ETWS: Failed to send ETWS primary notification to WMS");
  }
  else
  {
    MSG_HIGH_DS_1(cb_as_id,"CB ETWS: Sent CB ETWS primary notification to WMS - Security:%d",warning_security_info_present) ;
  }
} /* end cb_send_wms_etws_prim_ind() */
/*===========================================================================

FUNCTION      CB_SEND_WMS_ETWS_SEC_NOTIFICATION_IND

DESCRIPTION
  This function sends CB ETWS SECONDARY NOTIFICATION INDICATION to WMS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cb_send_wms_etws_sec_notification_ind
(
  word  message_id,
  word  serial_number,
  byte  data_coding_scheme,
  byte  number_of_pages,
  const byte* sec_notification_data
)
{
  cb_etws_sec_notification_ind_type *etws_sec_notification_ind;
#ifndef FEATURE_MODEM_HEAP
  if ((etws_sec_notification_ind = (cb_etws_sec_notification_ind_type *)gs_alloc(sizeof(cb_etws_sec_notification_ind_type)))== NULL)
#else
  if(( (etws_sec_notification_ind = (cb_etws_sec_notification_ind_type *) modem_mem_calloc(1,sizeof(cb_etws_sec_notification_ind_type),MODEM_MEM_CLIENT_NAS) ) == NULL) )
#endif
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to allocate etws_sec_notification_ind") ;
    return ;
  }

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#else
  etws_sec_notification_ind->message_header.message_set = MS_WMS_CB ;
  etws_sec_notification_ind->message_header.message_id  = (int)CB_ETWS_SEC_NOTIFICATION_IND ;
#endif
  /*
  **Construct cb_etws_sec_notification_ind message contents and
  **send the message to WMS
  */

  /*
  ** Copy the PLMN ID recieved in RRC CELL CHANGE IND and update the same to 
  ** update WMS
  */
  memscpy((void*)&etws_sec_notification_ind->plmn_id.identity[0], sizeof(etws_sec_notification_ind->plmn_id.identity), (void *)cb_current_plmn,PLMN_VALUE_LENGTH);
#ifdef FEATURE_DUAL_SIM
  etws_sec_notification_ind->as_id                      = (sys_modem_as_id_e_type)cb_as_id;
#endif
  etws_sec_notification_ind->message_id                 = message_id;
  etws_sec_notification_ind->serial_number              = serial_number;
  etws_sec_notification_ind->current_data_coding_scheme = data_coding_scheme;
  etws_sec_notification_ind->number_of_pages            = number_of_pages;
  /* As many bytes as reported by number pages in BMC message*/
  memscpy((void*)&etws_sec_notification_ind->page_data[0],CB_ETWS_MAX_SEC_NOFICATION_LEN,(void*)(sec_notification_data),
         (number_of_pages*CB_BMC_PAGE_LENGTH));

  PUT_IMH_LEN( sizeof(cb_etws_sec_notification_ind_type) - CB_ETWS_MAX_SEC_NOFICATION_LEN
               + (number_of_pages*CB_BMC_PAGE_LENGTH) - sizeof(IMH_T), 
               &etws_sec_notification_ind->message_header ) ;

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
  #error code not present
#else
  if (gs_send_message(GS_QUEUE_UASMS, (void*)etws_sec_notification_ind, TRUE) != GS_SUCCESS)
#endif
  {
    MSG_ERROR_DS_0(cb_as_id,"CB ETWS: Failed to send ETWS secondary notification to WMS");
  }
  else
  {
    MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Sent CB ETWS Secondary notification to WMS") ;
  }

  if(etws_sec_notification_ind != NULL)
  {
#ifndef FEATURE_MODEM_HEAP
    gs_free((void *)etws_sec_notification_ind);
#else
    modem_mem_free((void *)etws_sec_notification_ind, MODEM_MEM_CLIENT_NAS);
#endif
    etws_sec_notification_ind = NULL;
  }
} /* end cb_send_wms_etws_prim_ind() */
#endif

/*===========================================================================

FUNCTION      CB_SEND_CELL_CHANGE_IND

DESCRIPTION
  This function sends Cell Change indication to WMS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_send_wms_cell_change_ind
( 
 byte *incoming_plmn,
 byte *incoming_lac,
 byte *incoming_cell_id
)
{
  cb_cell_change_ind_type cell_change_ind ;
  byte etalon_cell_id[CELL_ID_VALUE_LENGTH] = {0x00, 0x00, 0x00, 0x00} ;

  

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
  #error code not present
#else
  cell_change_ind.message_header.message_set = MS_WMS_CB ;
  cell_change_ind.message_header.message_id = (int)CB_CELL_CHANGE_IND ;
#endif

  if ((memcmp((void *)incoming_cell_id, (void *)etalon_cell_id, CELL_ID_VALUE_LENGTH) == 0) &&
      (FALSE == cb_supported))
  {
    cell_change_ind.cell_change_type = CB_WMS_NO_SERVICE;
  }
  else{
    if (memcmp((void *)cb_current_plmn,(void *)incoming_plmn, PLMN_VALUE_LENGTH))
    {
      cell_change_ind.cell_change_type = CB_WMS_PLMN_CHANGE;
    }
    else if(memcmp((void *)current_lac,(void *)incoming_lac, LAC_VALUE_LENGTH))
    {
      cell_change_ind.cell_change_type = CB_WMS_LAC_CHANGE;
    }
    else if(memcmp((void *)current_cell_id,(void *)incoming_cell_id, CELL_ID_VALUE_LENGTH))
    {
      cell_change_ind.cell_change_type = CB_WMS_CELL_CHANGE;
    }
    else
    {
      cell_change_ind.cell_change_type = CB_WMS_SAME_CELL;
    }
  }

#ifdef FEATURE_DUAL_SIM
  cell_change_ind.as_id = (sys_modem_as_id_e_type) cb_as_id;
#endif
  if ((cell_change_ind.cell_change_type != CB_WMS_NO_SERVICE) &&
      (cell_change_ind.cell_change_type != CB_WMS_SAME_CELL))
  {
    skip_archive_length = 0;
  }

  PUT_IMH_LEN( sizeof(cb_cell_change_ind_type) - sizeof(IMH_T),
                                           & cell_change_ind.message_header ) ;
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
  #error code not present
#else
  if (gs_send_message(GS_QUEUE_UASMS, (void*)&cell_change_ind, TRUE) != GS_SUCCESS)
#endif
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Failed to send CB Cell Change indication to WMS");
  }
  else
  {
    MSG_HIGH_DS_0(cb_as_id,"CB: Sent CB Cell Change indication to WMS") ;
  }
} /* end cb_send_wms_cell_change_ind() */

/*===========================================================================

FUNCTION      CB_DELETE_PAGE

DESCRIPTION
  This function removes page buffer which is attached to the Message Descriptor

DEPENDENCIES
  None

RETURN VALUE
  Pointer into Page Buffer List which is pointing out the next valid Page 
  buffer, NULL if list is empty

SIDE EFFECTS
  None

===========================================================================*/
cb_missing_page_type *cb_delete_page
(
  cb_message_list_type *message,
  cb_missing_page_type *page_tobe_deleted
)
{
  cb_missing_page_type *missing_page ;

  if (message == NULL || page_tobe_deleted == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  missing_page = 
        message->message_header.current_message_descriptor->missing_page_list ;

  if (missing_page == page_tobe_deleted)
  {
    message->message_header.current_message_descriptor->
                     missing_page_list = page_tobe_deleted->next_missing_page ;
  }
  else
  {
    while (missing_page->next_missing_page != page_tobe_deleted)
    {
      missing_page = missing_page->next_missing_page ;

      if (missing_page == NULL)
      {
        MSG_FATAL_DS( cb_as_id,"CB: Existing page could not be found",0,0,0) ;
      }
    }

    missing_page->next_missing_page = page_tobe_deleted->next_missing_page ;
  }
  
  cb_mem_free(page_tobe_deleted) ;

  return message->
                 message_header.current_message_descriptor->missing_page_list ;
} /* end cb_delete_page() */

/*===========================================================================

FUNCTION      CB_DELETE_MESSAGE_FROM_LIST

DESCRIPTION
  This function removes the specific message descriptor from a Message 
  Descriptor list

DEPENDENCIES
  None

RETURN VALUE
  Pointer into CB Message Descriptor List which is pointing out the next valid
  Message descriptor, NULL if list is empty

SIDE EFFECTS
  None

===========================================================================*/
cb_message_list_type *cb_delete_message_from_list
(
  cb_message_list_type *message_tobe_deleted
)
{

  if (message_tobe_deleted == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)

  if ((search_list_len != 0) &&
      (message_tobe_deleted->message_header.first_tx_schedule != NULL ||
       message_tobe_deleted->message_header.next_first_tx_schedule != NULL))
  {
    cb_bmc_skip_scheduled_reception(message_tobe_deleted) ;   

  }

#endif /* #ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED */

  if (message_tobe_deleted->message_header.current_message_descriptor != NULL &&
        message_tobe_deleted->message_header.current_message_descriptor->missing_page_list != NULL)
  {
    do
    {
      message_tobe_deleted->
        message_header.current_message_descriptor->missing_page_list = 
              cb_delete_page(message_tobe_deleted,
                      message_tobe_deleted->message_header.
                               current_message_descriptor->missing_page_list) ;
    }
    while (message_tobe_deleted->
        message_header.current_message_descriptor->missing_page_list != NULL) ;
  }

  return cb_delete_message(message_tobe_deleted) ;

} /* end cb_delete_message_from_list() */

/*===========================================================================

FUNCTION      CB_DELETE_MESSAGE_CODE

DESCRIPTION
  This function removes the specific message descriptor from a Message 
  Descriptor list

DEPENDENCIES
  None

RETURN VALUE
  Pointer into CB Message Descriptor List which is pointing out the next valid
  Message descriptor, NULL if list is empty

SIDE EFFECTS
  None

===========================================================================*/

cb_message_list_type *cb_delete_message_code
(
  cb_message_list_type *current_message_param,
  cb_user_page_info_type *cb_prev_message_code,
  cb_user_page_info_type *cb_message_code_tobe_deleted
)
{
  cb_user_page_info_type *root_message_code ;

  if (current_message_param == NULL || cb_message_code_tobe_deleted == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  root_message_code = current_message_param->message_header.message_info ;
  if ((root_message_code == cb_message_code_tobe_deleted) && 
                     (cb_message_code_tobe_deleted->next_message_code == NULL))
  {
    current_message_param = cb_delete_message_from_list(current_message_param) ;
  }
  else
  {
    if (cb_prev_message_code == NULL)
    {
      current_message_param->message_header.message_info = 
                              cb_message_code_tobe_deleted->next_message_code ;
    }
    else
    {
      cb_prev_message_code->next_message_code = 
                              cb_message_code_tobe_deleted->next_message_code ;
    }

  }

  if (current_message_param != NULL)
  {
    if (current_message_param->message_header.current_message_descriptor == 
                                                  cb_message_code_tobe_deleted)
    {
      current_message_param->message_header.current_message_descriptor = 
                                 current_message_param->message_header.message_info ;
    }
  }

  cb_mem_free(cb_message_code_tobe_deleted) ;

  return current_message_param ;
} /* end cb_delete_message_code() */

/*===========================================================================

FUNCTION      CB_FILTERS_OUT_OF_DATA_MESSAGES_FROM_LIST

DESCRIPTION
  This function filters out the CB message descriptors which are not longer 
  included into new Search List and shall be discarded

DEPENDENCIES
  None

RETURN VALUE
  Pointer into CB Message Descriptor List 

SIDE EFFECTS
  None

===========================================================================*/
cb_message_list_type *cb_filter_out_of_date_messages_from_list
(
  cb_message_list_type **first_message_entry
)
{
  word message_id ;
  word message_code ;
  cb_user_page_info_type *cb_received_message_code = NULL ;
  cb_user_page_info_type *cb_prev_message_code = NULL ;
  cb_message_list_type *list =NULL;
  boolean message_tobe_deleted = FALSE ;
  boolean plmn_changed = FALSE ;
  boolean lac_changed = FALSE ;
  boolean cell_changed = FALSE ;
  boolean next_message_ptr_aready_exist = FALSE ;
  boolean timer_offset_0_deleted = FALSE ;

  if (first_message_entry == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  list = *first_message_entry;

  if (list == NULL)
  {
    return NULL ; /* Nothing to do */
  }


  while (list != NULL)
  {
    next_message_ptr_aready_exist = FALSE ;

    cb_prev_message_code = NULL ;

    message_id = list->message_header.message_id ;

    cb_received_message_code = list->message_header.message_info ;

    while (cb_received_message_code != NULL)
    {
      message_code = cb_received_message_code->message_code ;

      message_tobe_deleted = FALSE ;

      plmn_changed = 
            !sys_plmn_match (*(sys_plmn_id_s_type *)cb_current_plmn, 
                          *(sys_plmn_id_s_type *)cb_received_message_code->plmn) ;

      lac_changed = 
          (memcmp((void *)current_lac,
                  (void *)cb_received_message_code->lac,
                                                      LAC_VALUE_LENGTH) != 0) ;
      cell_changed =             
          (memcmp((void *)current_cell_id,
                        (void *)cb_received_message_code->cell_id,
                                                  CELL_ID_VALUE_LENGTH) != 0) ;

      switch (cb_received_message_code->gs_indicator)
      {
        case CB_HIGH_PRIORITY_CELL_WIDE:
        case CB_LOW_PRIORITY_CELL_WIDE:
          if (!plmn_changed && (lac_changed || cell_changed))
          {
            message_tobe_deleted = TRUE ;

            MSG_HIGH_DS_2(cb_as_id,"Cell changed, message (%d, %d) is to be deleted",
                                                   message_id,message_code) ;
          }
          break ;
        case CB_PLMN_WIDE:
          break ;
        case CB_LOCATION_AREA_WIDE:
          if (!plmn_changed && lac_changed)
          {
            message_tobe_deleted = TRUE ;

            MSG_HIGH_DS_2(cb_as_id,"LAC changed, message (%d, %d) is to be deleted",
                                                   message_id,message_code) ;
          }
          break ;
        default:
          MSG_ERROR_DS_1(cb_as_id,"GS indicator is out of range (%d)",
                                  cb_received_message_code->gs_indicator) ;
          break ;
      } /* switch (list->message_header.message_info.gs_indicator) */

      if (message_tobe_deleted)
      {  
        page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;

#ifdef FEATURE_UMTS_BMC
        /* Make sure BMC to stop processing remaining message PDUs if any */
        cb_bmc_msg_sfn_allocation_length = 0 ;
#endif

        list->message_header.current_message_descriptor = cb_received_message_code ;
        if(cb_received_message_code->timer_offset == 0)
        {
            timer_offset_0_deleted = TRUE;
        }

        current_message = cb_delete_message_code(list,
                               cb_prev_message_code,cb_received_message_code) ;

        if (current_message == NULL)
        {
          *first_message_entry = NULL ;
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
          if(basic_etws_list_entry_point == NULL)
#endif
          {
            cb_dup_stop_timer((timer_id_T)CB_TIMER_DUP_DET);
            cb_next_min_offset = CB_DUP_TIMER_VALUE_INVALID;
          }

          return NULL ;
        }

        if (current_message->prev_message == NULL)
        {
          *first_message_entry = current_message ;
        }

        if (list != current_message)
        {
          list = current_message ;

          next_message_ptr_aready_exist = TRUE ;

          break ;
        }
        else
        {
          if (cb_prev_message_code == NULL)
          {
            cb_received_message_code = list->message_header.message_info ;
          }
          else
          {
            cb_received_message_code = cb_prev_message_code->next_message_code ;
          }
        }
      }
      else
      {
        cb_prev_message_code = cb_received_message_code ;

         /* Go to the next received CB message */
        cb_received_message_code = cb_received_message_code->next_message_code ;
      }
    }

    if (!next_message_ptr_aready_exist)
    {
      list = list->next_message ;
    }
  }

  cb_update_cache_for_multiple_message_deletion(timer_offset_0_deleted);

  return current_message ;
} /* end cb_filter_out_of_date_messages_from_list() */

/*===========================================================================

FUNCTION      CB_FILTERS_OUT_OF_DATA_MESSAGES

DESCRIPTION
  This function filters out the CB message descriptors which are not longer 
  included into new Search List and shall be discarded

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
void cb_filters_out_of_date_messages
(
  cb_channel_ind_type channel_msg_list_id
)
{
      basic_cbch_list = cb_filter_out_of_date_messages_from_list(
                                             &basic_cbch_list_entry_point) ;
} /* end cb_filters_out_of_date_messages() */
/*lint +e715*/
/*===========================================================================

FUNCTION      CB_FILTER_OUT_OBSOLETE_ID_FROM_LIST

DESCRIPTION
  This function filters out the CB message descriptors from a specific list 
  which are not longer included into new Search List and shall be discarded

DEPENDENCIES
  None

RETURN VALUE
  Pointer into CB Message Descriptor List 

SIDE EFFECTS
  None

===========================================================================*/
cb_message_list_type *cb_filter_out_obsolete_id_from_list
(
  cb_message_list_type **first_message_entry
)
{
  cb_user_page_info_type *cb_received_message_code = NULL ;
  cb_user_page_info_type *cb_prev_message_code = NULL ;
  cb_message_list_type *list = NULL ;
  boolean next_message_ptr_aready_exist = FALSE ;
  boolean timer_offset_0_deleted = FALSE ;

  if (first_message_entry == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  list = *first_message_entry;

  if (list == NULL)
  {
    return NULL ; /* Nothing to do */
  }

  while (list != NULL)
  {
    next_message_ptr_aready_exist = FALSE ;

    cb_prev_message_code = NULL ;

    if (!cb_message_is_requested(list->message_header.message_id))
    {
#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)
      /*If already scheduled the message id generate skip*/
      if ((search_list_len != 0) &&
          ((list->message_header.first_tx_schedule != NULL ||
            list->message_header.next_first_tx_schedule != NULL)))
      {
        cb_bmc_skip_scheduled_reception(list) ;
      }
      /* Indicate the scedular to read message_id again */
      cb_bmc_reset_msg_bitmap_info(list->message_header.message_id) ;
#endif/*DRX*/
      cb_received_message_code = list->message_header.message_info ;

      if (cb_received_message_code == NULL)
      {
        current_message = cb_delete_message(list);

        if (current_message == NULL)
        {
          *first_message_entry = NULL ;
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
          if(basic_etws_list_entry_point == NULL)
#endif          
          {
            cb_dup_stop_timer((timer_id_T)CB_TIMER_DUP_DET);
            cb_next_min_offset = CB_DUP_TIMER_VALUE_INVALID;
          }
          return NULL ;
        }

        if (current_message->prev_message == NULL)
        {
          *first_message_entry = current_message ;
        }

        if (list != current_message)
        {
          list = current_message ;

          next_message_ptr_aready_exist = TRUE ;
        }
      }

      while (cb_received_message_code != NULL)
      {
        page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;

        list->message_header.current_message_descriptor = cb_received_message_code ;

        MSG_HIGH_DS_2(cb_as_id,"CB: Message (%d, %d) is obsolete and filtered out",
                   list->message_header.message_id,cb_received_message_code->message_code) ;
                   
        if(cb_received_message_code->timer_offset == 0)
        {
            timer_offset_0_deleted = TRUE;
        }  
        current_message = cb_delete_message_code(list,
                               cb_prev_message_code,cb_received_message_code) ;

        if (current_message == NULL)
        {
          *first_message_entry = NULL ;
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
           if(basic_etws_list_entry_point == NULL)
#endif
           {
             cb_dup_stop_timer((timer_id_T)CB_TIMER_DUP_DET);
             cb_next_min_offset = CB_DUP_TIMER_VALUE_INVALID;
           }
          return NULL ;
        }

        if (current_message->prev_message == NULL)
        {
          *first_message_entry = current_message ;
        }

        if (list != current_message)
        {
          list = current_message ;

          next_message_ptr_aready_exist = TRUE ;

          break ;
        }
        else
        {
          if (cb_prev_message_code == NULL)
          {
            cb_received_message_code = list->message_header.message_info ;
          }
          else
          {
            cb_received_message_code = cb_prev_message_code->next_message_code ;
          }
        }
      } /* while (cb_received_message_code != NULL) */
    } /* if (!cb_message_is_requested(list->message_header.message_id)) */

    if (!next_message_ptr_aready_exist)
    {
      list = list->next_message ;
    }
  }
#ifdef FEATURE_UMTS_BMC
  /*If the current message pointer is NULL then
     reset the SFN message allocation length*/
  if (current_message == NULL)
  {
    cb_bmc_msg_sfn_allocation_length = 0;
  }
#endif
  cb_update_cache_for_multiple_message_deletion(timer_offset_0_deleted);
  return current_message ;
} /* end cb_filter_out_obsolete_id_from_list() */

/*===========================================================================

FUNCTION      CB_PROCESS_WMS_SEARCH_LIST

DESCRIPTION
  This function implements processing of WMS Search List request

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_process_wms_search_list
(
  cb_search_list_req_type *search_list_req
)
{
  boolean process_search_list = FALSE ;

  if (search_list_req == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  if (search_list_req->search_list_length != search_list_len)
  {
    process_search_list = TRUE ;
  } else if (search_list_req->search_list_length == search_list_len)
  {
    process_search_list = 
        (memcmp((void *)search_lst,(void *)search_list_req->search_list,
               search_list_len*sizeof(cb_wms_message_id_value_type)) != 0) ;
  }

  if (process_search_list)
  {
    search_list_len = search_list_req->search_list_length ;

    if (search_list_len > CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED)
    {
      search_list_len = CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED ;

      MSG_ERROR_DS_0(cb_as_id,"CB: Search list length exceeds max. allowed number, truncated") ;
    }

    if (search_list_len == 0)
    {
      MSG_HIGH_DS_0(cb_as_id,"CB: Search list is empty") ;
    }
    else
    {
      memscpy((void *)search_lst,sizeof(cb_wms_message_id_value_type)*CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED,(void *)search_list_req->search_list,
                      search_list_len*sizeof(cb_wms_message_id_value_type)) ;
    }

    basic_cbch_list = 
            cb_filter_out_obsolete_id_from_list(&basic_cbch_list_entry_point) ;

#ifdef FEATURE_WCDMA_ETWS
    basic_etws_list =
            cb_filter_out_obsolete_id_from_etws_list(&basic_etws_list_entry_point);
#endif
  }
      
} /* end cb_process_wms_search_list() */

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
void cb_delete_all_pages
(
  void
)
{
  /* Copy current search list length into tmp. location */
  byte  copy_search_list_length = search_list_len ;

  /* Clean up current search list */
  search_list_len = 0 ;

  /* Remove all collected messages from the cache memory */
  basic_cbch_list = 
            cb_filter_out_obsolete_id_from_list(&basic_cbch_list_entry_point) ;

#ifdef FEATURE_UMTS_BMC
  /* Make sure BMC to stop processing remaining message PDUs if any */
  cb_bmc_msg_sfn_allocation_length = 0 ;
#endif

  /* Restore search list length to its original value */
  search_list_len = copy_search_list_length ;

} /* end of cb_delete_all_pages */

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
dword cb_calculate_base_fn
(
  dword received_fn,
  byte  paging_block_number
)
{
  long first_paging_block ;

  if ((first_paging_block = (long)((long)received_fn - 
                        (long)(paging_block_number*CB_MULTIFRAME_LENGTH))) < 0)
  {
    first_paging_block += GSM_HYPER_FRAME_LENGTH ;
  }

  return (dword)first_paging_block ;

} /* end cb_calculate_base_fn() */

/*===========================================================================

FUNCTION      CB_DEACTIVATE_CB_MONITORING

DESCRIPTION
  This function sends CB_DEACTIVATE_REQ primitive to L1

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void cb_deactivate_cb_monitoring
(
  cb_channel_ind_type channel_type
)
{
  cb_deactivate_req_type deactivate_req ;

  memset(&deactivate_req,0, sizeof(deactivate_req));
  deactivate_req.message_header.message_set = MS_CB_L1 ;
  deactivate_req.message_header.message_id = (int)CB_DEACTIVATE_REQ ;

  deactivate_req.message_header.message_len_lsb = 0 ;
  deactivate_req.message_header.message_len_msb = 0 ;
  
  deactivate_req.channel_ind = channel_type ;

#ifdef FEATURE_DUAL_SIM
  deactivate_req.as_id = (sys_modem_as_id_e_type) cb_as_id;
#endif

  PUT_IMH_LEN( sizeof(cb_deactivate_req_type) - sizeof(IMH_T),
                                           & deactivate_req.message_header ) ;

  cb_active_substate_control(CB_STD_DEACTIVATED);

  skip_archive_length = 0;

  MSG_HIGH_DS_0(cb_as_id,"CB: Sending CB_DEACTIVATE_REQ request") ;

  cb_send_event_ctch_stop();

  cb_send_l1_request((void *)&deactivate_req) ;
} /* end cb_deactivate_cb_monitoring() */

#ifdef FEATURE_UMTS_BMC
extern sys_modem_as_id_e_type bmc_as_id;
/*===========================================================================

FUNCTION      CB_BMC_DEACTIVATE_CTCH_MONITORING

DESCRIPTION
  This function sends CB_DEACTIVATE_REQ primitive to WCDMA L1

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_deactivate_ctch_monitoring( void )
{
  l1_ext_cmd_type  *cmd_ptr;

#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = get_wcdma_interface();
  interface_t *ptr_W_MMCP = get_wcdma_mmcp_interface();
  if (ptr_W == NULL)
      return;
  cmd_ptr = IWCDMA_l1_get_cmd_buf(ptr_W);
  
  if (ptr_W_MMCP == NULL)
      return;
#else
  cmd_ptr = l1_get_cmd_buf();
#endif

  if (cmd_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id,"BMC: Not enough memory to send WCDMA L1 request",0,0,0) ;
  }

  cmd_ptr->cmd_hdr.cmd_id = CPHY_BMC_DEACTIVATE_REQ ;
  #ifdef FEATURE_DUAL_SIM
  cmd_ptr->cmd.bmc_deactivate_req.as_id=bmc_as_id;
  #endif
  cb_active_substate_control(CB_STD_DEACTIVATED);

  cb_send_event_ctch_stop();
#ifdef FEATURE_SEGMENT_LOADING
  IWCDMA_dl_dec_deregister_ctch_pdu_decode_error_cb(ptr_W_MMCP);
  IWCDMA_l1_put_cmd(ptr_W, cmd_ptr );
#else
  dl_dec_deregister_ctch_pdu_decode_error_cb();

  /* Send the command to WCDMA L1 */
  l1_put_cmd( cmd_ptr );
#endif
}
#endif

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
void cb_deactivate_bmc_monitoring
(
  cb_rat_type current_rat
)
{

#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
  /* clear DRX */
  cb_bmc_cancel_scheduling();
#endif

  if (current_rat == CB_GSM_MODE)
  {
    if (cb_extended_cbch_supported)
    {
      cb_deactivate_cb_monitoring(CBCH_ECBCH) ;
    }
    else
    {
      cb_deactivate_cb_monitoring(CBCH) ;
    }
  }
#ifdef FEATURE_UMTS_BMC
  else if (current_rat == CB_UMTS_MODE)
  {
    cb_bmc_deactivate_ctch_monitoring() ;
  }
#endif
  else
  {
    MSG_FATAL_DS( cb_as_id,"Received (%d) illigal RAT (GSM-0,UMTS-1)",(int)current_rat,0,0) ;
  }
} /* end cb_deactivate_bmc_monitoring() */

#ifdef FEATURE_GSM_CB_PSEUDO_DRX

#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
byte cb_pseudo_drx_offset_sim[] = {0,0}; /* O - wake up at O, O+P, O+2P... */
boolean cb_is_second_pseudo_mode_sim[] = {FALSE,FALSE};

dword cb_pseudo_drx_start_fn_sim[] = {BMC_INVALID_SFN,BMC_INVALID_SFN}; /* psuedo SP start FN */
dword cb_pseudo_drx_prev_start_fn_sim[] = {BMC_INVALID_SFN,BMC_INVALID_SFN};

#define cb_pseudo_drx_offset cb_pseudo_drx_offset_sim[cb_as_id]
#define cb_is_second_pseudo_mode cb_is_second_pseudo_mode_sim[cb_as_id]
#define cb_pseudo_drx_start_fn cb_pseudo_drx_start_fn_sim[cb_as_id]
#define cb_pseudo_drx_prev_start_fn cb_pseudo_drx_prev_start_fn_sim[cb_as_id]

#else
byte cb_pseudo_drx_offset = 0; /* O - wake up at O, O+P, O+2P... */
boolean cb_is_second_pseudo_mode = FALSE;

dword cb_pseudo_drx_start_fn = BMC_INVALID_SFN; /* psuedo SP start FN */
dword cb_pseudo_drx_prev_start_fn = BMC_INVALID_SFN;
#endif

boolean cb_pseudo_drx_supported = TRUE;
dword cb_pre_pseudo_drx_scan_length = MAX_SCHEDULING_PERIOD_LENGTH;

byte cb_first_pseudo_drx_periodicity = CB_PSEUDO_DRX_DEFAULT_PERIODICITY;  /* P - wake up at O, O+P, O+2P... */
byte cb_second_pseudo_drx_periodicity = CB_PSEUDO_DRX_DEFAULT_PERIODICITY;  /* P - wake up at O, O+P, O+2P... */

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

void cb_start_gsm_pseudo_drx_mode(boolean current_cycle_expired)
{
  cb_scheduling_req_type scheduling_req ;
  byte process_index ;
  byte scheduling_command_length = 0;
  dword *scheduled_fn_list ;
  word dindex ;
  word prn_number ;
  byte psuedo_scheduling_slots[CB_PSEUDO_DRX_MAX_LENGTH];
  dword current_base_sfn = cb_calculate_base_fn(cb_current_sfn,CB_CALCULATE_PAGING_SLOT(cb_current_sfn));
  byte pseudo_drx_length = CB_PSEUDO_DRX_MAX_LENGTH;
  byte cb_pseudo_drx_periodicity = 
    (cb_is_second_pseudo_mode ? cb_second_pseudo_drx_periodicity : cb_first_pseudo_drx_periodicity);

  if (cb_pseudo_drx_start_fn == BMC_INVALID_SFN)
  {
    cb_cancel_scheduling(CBCH) ;
    return;
  }

  /* drop duplicate request due to expiry ind */
  if ((cb_pseudo_drx_prev_start_fn != BMC_INVALID_SFN) &&
      (cb_pseudo_drx_prev_start_fn == cb_calculate_slot_fn(current_base_sfn,1)))
  {
    return;
  }

  /* check if current FN is still in current SP only if current cycle didnt expire at GL1 */
  if (!current_cycle_expired &&
      (cb_pseudo_drx_prev_start_fn != BMC_INVALID_SFN) &&
      (current_base_sfn != cb_pseudo_drx_start_fn ) &&
      cb_bmc_check_sfn_within_sch_prd(current_base_sfn,cb_pseudo_drx_prev_start_fn,
                                      cb_pseudo_drx_start_fn))
  {
    return;
  }


  /* move to SP start FN */
  if ((current_cycle_expired) &&
      (cb_pseudo_drx_start_fn != BMC_INVALID_SFN))
  {
    current_base_sfn = cb_pseudo_drx_start_fn;
  }
  else
  {
    current_base_sfn = cb_calculate_slot_fn(current_base_sfn, 1);
  }

  memset(&scheduling_req,0x00,sizeof(cb_scheduling_req_type));

  for (process_index=0;process_index < pseudo_drx_length ;process_index++)
  {
    psuedo_scheduling_slots[process_index] = 
       cb_pseudo_drx_offset + (cb_pseudo_drx_periodicity * process_index);
    if (psuedo_scheduling_slots[process_index] >= pseudo_drx_length )
    {
      break;
    }
    scheduling_command_length++ ;
  }

  scheduling_command_length *= CB_PAGING_MAX_BLOCKS;

  if ((scheduled_fn_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                              sizeof(dword)*scheduling_command_length
                              )) == NULL)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB PSEUDO DRX: Not enough memory to send L1 Scheduling Request") ;

    cb_cancel_scheduling(CBCH) ;

    return ;
  }

  for (process_index=0;process_index<scheduling_command_length/CB_PAGING_MAX_BLOCKS;process_index++)
  {
    scheduled_fn_list[(process_index*4)] = cb_calculate_slot_fn(current_base_sfn,psuedo_scheduling_slots[process_index]);
    scheduled_fn_list[(process_index*4)+1] = cb_calculate_block_fn(scheduled_fn_list[(process_index*4)], 1);
    scheduled_fn_list[(process_index*4)+2] = cb_calculate_block_fn(scheduled_fn_list[(process_index*4)], 2);
    scheduled_fn_list[(process_index*4)+3] = cb_calculate_block_fn(scheduled_fn_list[(process_index*4)], 3);
  }

  scheduling_req.scheduling_period_last_fn = cb_calculate_slot_fn(current_base_sfn, pseudo_drx_length-1)
                                 + (CB_FOURTH_BLOCK*CB_MULTIFRAME_LENGTH);

  scheduling_req.message_header.message_set = MS_CB_L1 ;
  scheduling_req.message_header.message_id = CB_SCHEDULING_REQ ;
  scheduling_req.channel_ind = CBCH ;
  scheduling_req.scheduling_mode = CB_DRX_MODE ;

  scheduling_req.scheduling_descriptor.scheduling_period_length = scheduling_command_length;
  scheduling_req.scheduling_descriptor.scheduling_period_fn_list = scheduled_fn_list ;

  cb_pseudo_drx_prev_start_fn = current_base_sfn;
  cb_pseudo_drx_start_fn = cb_calculate_slot_fn(current_base_sfn, pseudo_drx_length);

  PUT_IMH_LEN( sizeof(cb_scheduling_req_type) - sizeof(IMH_T),
                                           & scheduling_req.message_header ) ;

  MSG_HIGH_DS_3(cb_as_id,"CB PSEUDO DRX: Sending CB_SCHEDULING_REQ(DRX) Curr:%ld(length %d slots) Next:%ld",
     current_base_sfn,pseudo_drx_length,cb_pseudo_drx_start_fn) ;

  prn_number = ((scheduling_command_length%3) == 0) ? scheduling_command_length/3 : scheduling_command_length/3 + 1;
  MSG_HIGH_DS_3(cb_as_id,"CB PSEUDO DRX: Scheduling FN list length is %d (O:%d/P:%d):",
     scheduling_command_length,cb_pseudo_drx_offset,cb_pseudo_drx_periodicity) ;
  for (dindex=0; dindex<prn_number; dindex++)
  {

    MSG_HIGH_DS_3(cb_as_id,"CB PSEUDO DRX: Slot# %lu %lu %lu",
    ((scheduled_fn_list[dindex*3]-current_base_sfn+GSM_HYPER_FRAME_LENGTH)%GSM_HYPER_FRAME_LENGTH)
        /(CB_MULTIFRAME_LENGTH*CB_CHANNEL_CYCLE),
    ((scheduled_fn_list[(dindex*3) + 1]-current_base_sfn+GSM_HYPER_FRAME_LENGTH)%GSM_HYPER_FRAME_LENGTH)
        /(CB_MULTIFRAME_LENGTH*CB_CHANNEL_CYCLE),
    ((scheduled_fn_list[(dindex*3) + 2]-current_base_sfn+GSM_HYPER_FRAME_LENGTH)%GSM_HYPER_FRAME_LENGTH)
        /(CB_MULTIFRAME_LENGTH*CB_CHANNEL_CYCLE)
    ) ;

    MSG_HIGH_DS_3(cb_as_id,"CB PSEUDO DRX: FN# %lu %lu %lu",
    scheduled_fn_list[dindex*3],
    scheduled_fn_list[dindex*3 + 1],
    scheduled_fn_list[dindex*3 + 2] 
    ) ;
    
  }

#ifdef FEATURE_DUAL_SIM
  scheduling_req.as_id = (sys_modem_as_id_e_type) cb_as_id;
#endif

  cb_send_l1_request((void *)&scheduling_req) ;

  /* move to second pseudo DRX mode */
  if (!cb_is_second_pseudo_mode)
  {
    if(cb_second_pseudo_drx_periodicity == cb_first_pseudo_drx_periodicity)
    {
      if(++cb_pseudo_drx_offset >= cb_second_pseudo_drx_periodicity)
      {
        cb_pseudo_drx_offset = 0;
      }
    }
    cb_is_second_pseudo_mode = TRUE;
  }
  else
  {
    if(++cb_pseudo_drx_offset >= cb_second_pseudo_drx_periodicity)
    {
      cb_pseudo_drx_offset = 0;
    }
  }
}
#endif

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
)
{
  cb_scheduling_req_type scheduling_req = {{0}};

#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
  /*Cancel the scheduling*/
  cb_bmc_cancel_scheduling();
#endif

  scheduling_req.message_header.message_set = MS_CB_L1 ;
  scheduling_req.message_header.message_id = (int)CB_SCHEDULING_REQ ;

  scheduling_req.channel_ind = channel_type ;

  scheduling_req.scheduling_mode = CB_NON_DRX_MODE ;

  scheduling_req.scheduling_descriptor.scheduling_period_length = 0 ;
  scheduling_req.scheduling_descriptor.scheduling_period_fn_list = NULL ;
  scheduling_req.scheduling_period_last_fn = 0;

#ifdef FEATURE_DUAL_SIM
  scheduling_req.as_id = (sys_modem_as_id_e_type) cb_as_id;
#endif

  PUT_IMH_LEN( sizeof(cb_scheduling_req_type) - sizeof(IMH_T),
                                           & scheduling_req.message_header ) ;

  cb_active_substate_control(CB_STD_NON_DRX_ACTIVATED);

  MSG_HIGH_DS_0(cb_as_id,"CB: Sending CB_SCHEDULING_REQ(CB_NON_DRX_MODE) request") ;

  cb_send_event_ctch_start(CB_NON_DRX_MODE,0);

  cb_send_l1_request((void *)&scheduling_req) ;
} /* end cb_start_gsm_non_drx_mode() */




/*===========================================================================

FUNCTION    CB_START_NON_DRX_MODE

DESCRIPTION
  This function sends to L1 CB_SCHEDULING_REQ primitive in order to 
  trigger NON DRX CB message reception

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_start_non_drx_mode
(
  cb_rat_type current_rat
)
{
  if (current_rat == CB_GSM_MODE)
  {
    if (cb_extended_cbch_supported)
    {
      cb_start_gsm_non_drx_mode(CBCH_ECBCH) ;
    }
    else
    {
      cb_start_gsm_non_drx_mode(CBCH) ;
    }
  }
#ifdef FEATURE_UMTS_BMC
  else if (current_rat == CB_UMTS_MODE)
  {
    cb_start_umts_non_drx_mode() ;
  }
  else
  {
    MSG_FATAL_DS( cb_as_id,"Received (%d) illigal RAT (GSM-0,UMTS-1)",(int)current_rat,0,0) ;
  }
#else
  else
  {
    MSG_ERROR_DS_1(cb_as_id," Received (%d) RAT not supported",current_rat);
  }
#endif
} /* end cb_start_non_drx_mode() */

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
boolean cb_skiped_fn
(
  dword received_fn
)
{
  byte archive_index ;

  for (archive_index=0;archive_index<skip_archive_length;archive_index++)
  {
    if (received_fn == skip_archive[archive_index])
    {
      return TRUE;
    }
  }

  return FALSE ;

} /* end of cb_skiped_fn() */

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
void cb_send_wms_duplication_ind
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
)
{
  cb_duplication_detection_ind_type duplication_ind ;

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
  #error code not present
#else
  duplication_ind.message_header.message_set = MS_WMS_CB ;
  duplication_ind.message_header.message_id = (int)CB_DUPLICATION_DETECTION_IND ;
#endif

  /* Message Identifier */
  duplication_ind.message_id = message_id ;

  /* Message code */
  duplication_ind.message_code = message_code ;

  /* Geographic scope */
  duplication_ind.gs_indicator = gs_indicator ;

  /* Update Number */
  duplication_ind.update_number = update_number ;

  /* Page Number */
  duplication_ind.page_number = page_number ;

  /* Total number of pages */
  duplication_ind.number_of_pages = number_of_pages ;

  /* Data coding scheme */
  duplication_ind.data_coding_scheme = data_coding_scheme ;

#ifdef FEATURE_DUAL_SIM
  duplication_ind.as_id = (sys_modem_as_id_e_type) cb_as_id;
#endif

  PUT_IMH_LEN( sizeof(cb_duplication_detection_ind_type) - sizeof(IMH_T),
                                           & duplication_ind.message_header ) ;

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
  #error code not present
#else
  if (gs_send_message(GS_QUEUE_UASMS, (void*)&duplication_ind, TRUE) != GS_SUCCESS)
#endif
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Failed to send CB duplication indication to WMS");
  }
  else
  {
    MSG_HIGH_DS_0(cb_as_id,"CB: Sent Duplication indication to WMS") ;
  }

} /* end cb_send_wms_duplication_ind() */

/*===========================================================================

FUNCTION      CB_SEND_ERROR_INDICATION

DESCRIPTION
  This function sends CB ERROR INDICATION to WMS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_send_wms_error_indication
(
  cb_error_type_T            error_value
)
{
  cb_error_indication_type cb_error_indication ;

  cb_error_indication.message_header.message_set = MS_WMS_CB ;
  cb_error_indication.message_header.message_id = (byte)CB_ERROR_IND;

  /* Error value */
  cb_error_indication.error_value = error_value;

#ifdef FEATURE_DUAL_SIM
  cb_error_indication.as_id = (sys_modem_as_id_e_type)cb_as_id;
#endif

  PUT_IMH_LEN( sizeof(cb_error_indication) - sizeof(IMH_T),
                                           & cb_error_indication.message_header ) ;

  if (gs_send_message(GS_QUEUE_UASMS, (void*)&cb_error_indication, TRUE) != GS_SUCCESS)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Failed to send CB error indication to WMS");
  }
  else
  {
    MSG_HIGH_DS_1(cb_as_id,"CB: Sent error(%d) indication to WMS",error_value) ;
  }

} /* end cb_send_wms_error_indication() */


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
void cb_delete_required_page
(
  word message_id,
  word message_code,
  byte page_number
)
{
  cb_message_list_type *new_message_ptr = NULL ;
  cb_message_list_type **current_cbch_list_entry_point = NULL ;

  new_message_ptr = cb_find_message(basic_cbch_list,message_id);
  if (new_message_ptr != NULL)
  {
    current_cbch_list_entry_point = &basic_cbch_list_entry_point ;

    if (cb_find_message_code(new_message_ptr,message_code) != NULL)
    {
      cb_user_page_info_type *wanted_page_info = 
                   new_message_ptr->message_header.current_message_descriptor ;

      wanted_page_info->received_page_indicator &= 
                                          ~((word) ((word)1 << (word)(page_number-1))) ;

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)
      /* Indicate the scedular to read message_id again */
      cb_bmc_reset_msg_bitmap_info(message_id) ;
#endif/*FEATURE_UMTS_BMC_DRX_SUPPORTED*/
      MSG_HIGH_DS_3(cb_as_id,"CB: Message (ID %d, Code %d, Page %d) has been cleared",
                                         message_id,message_code,page_number) ;

      if (wanted_page_info->received_page_indicator == 0)
      {
        cb_user_page_info_type *cb_prev_message_code = NULL ;
        cb_user_page_info_type *current_page_info =
                                 new_message_ptr->message_header.message_info ;

        while(current_page_info != NULL)
        {
          if (current_page_info == wanted_page_info)
          {
            break ;
          }

          cb_prev_message_code = current_page_info ;

          current_page_info = current_page_info->next_message_code ;
        }

        if (current_page_info == NULL)
        {
          MSG_ERROR_DS_0(cb_as_id,"CB: Could not remove message entry from the list") ;

          return ;
        }

        MSG_HIGH_DS_3(cb_as_id,"CB: Message (ID %d, Code %d, Page %d) has been deleted",
                                         message_id,message_code,page_number) ;

        new_message_ptr = cb_delete_message_code(
                       new_message_ptr,cb_prev_message_code,wanted_page_info) ;

        if (new_message_ptr != current_message)
        {
          page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;

#ifdef FEATURE_UMTS_BMC
          /* Make sure BMC to stop processing remaining message PDUs if any */
          cb_bmc_msg_sfn_allocation_length = 0 ;
#endif

        }
        else if ((new_message_ptr!=NULL)&&
                 (new_message_ptr->message_header.current_message_descriptor != 
                              current_message->message_header.current_message_descriptor))
                 
        {
          page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;

#ifdef FEATURE_UMTS_BMC
          /* Make sure BMC to stop processing remaining message PDUs if any */
          cb_bmc_msg_sfn_allocation_length = 0 ;
#endif

        }

        current_message = new_message_ptr ;
        /*lint -e613*/
        if (current_message == NULL)
        {
          *current_cbch_list_entry_point = NULL ;
        } else if (current_message->prev_message == NULL)
        {
          *current_cbch_list_entry_point = current_message ;
        }
        /*lint +e613*/
      }

      basic_cbch_list = current_message ;

    }
    else
    {
      MSG_HIGH_DS_2(cb_as_id,"CB: Message (ID %d, Code %d) has not been received yet",
                                                   message_id,message_code) ;

      if (new_message_ptr->message_header.current_message_descriptor == NULL)
      {
        new_message_ptr->message_header.current_message_descriptor = 
                                 new_message_ptr->message_header.message_info ;

        page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;

#ifdef FEATURE_UMTS_BMC
          /* Make sure BMC to stop processing remaining message PDUs if any */
          cb_bmc_msg_sfn_allocation_length = 0 ;
#endif

      }
    }
  }
} /* end cb_delete_required_page() */
/*===========================================================================
FUNCTION    cb_compare_fn

DESCRIPTION This function compares the provided arguments and returns the 
value accordingly 
  
DEPENDENCIES
  None

RETURN VALUE
  comparision result(int type)

SIDE EFFECTS
  None

===========================================================================*/
static int cb_compare_fn(const void* a, const void* b)
{
  if(*(dword*)a > *(dword*)b)
  {
    return 1;
  } 
  else if(*(dword*)a < *(dword*)b)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}
/*===========================================================================
FUNCTION    cb_sort_list

DESCRIPTION This function is to sort the list of values.
  
DEPENDENCIES
  None

RETURN VALUE
  sorted list(dword* type)

SIDE EFFECTS
  None

===========================================================================*/
dword* cb_sort_list(dword* fn_list, word fn_list_length)
{
  if(fn_list != NULL)
  {
    qsort(fn_list, fn_list_length, sizeof(dword), cb_compare_fn);
  }
  return fn_list;
}
#ifdef FEATURE_UMTS_BMC
  /*===========================================================================

FUNCTION CB_BMC_SEND_SKIP_REQUEST

DESCRIPTION
  This function sends SKIP request to WL1

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_send_skip_request
(
 word   skip_list_length,
 dword *skip_list
)
{
  /* DEBUG ONLY. MUST BE REMOVED */
  word dindex ;
  word prn_number ;
  /* DEBUG ONLY. MUST BE REMOVED */

  l1_ext_cmd_type  *cmd_ptr;

#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = get_wcdma_interface();
  if (ptr_W == NULL)
      return;
#endif

#ifdef FEATURE_SEGMENT_LOADING
  cmd_ptr = IWCDMA_l1_get_cmd_buf(ptr_W);
#else
  cmd_ptr = l1_get_cmd_buf();
#endif

  if (cmd_ptr == NULL)
  {
    MSG_ERROR_DS_0(cb_as_id,"BMC: Not enough memory to send WCDMA L1 request") ;
    return;
  }

  cmd_ptr->cmd_hdr.cmd_id = CPHY_BMC_SKIP_REQ;
  cmd_ptr->cmd.bmc_skip_req.message_header.message_set = MS_BMC_L1;
  cmd_ptr->cmd.bmc_skip_req.message_header.message_id  = (int)CPHY_BMC_SKIP_REQ;

  cmd_ptr->cmd.bmc_skip_req.scheduling_descriptor.scheduling_period_length =
                                         (byte)skip_list_length;
  cmd_ptr->cmd.bmc_skip_req.scheduling_descriptor.scheduling_period_sfn_list = 
                                                 skip_list;
#ifdef FEATURE_DUAL_SIM
  cmd_ptr->cmd.bmc_skip_req.as_id = (sys_modem_as_id_e_type) cb_as_id; 
#endif

  if (skip_list == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }
  /*Sorting */
  skip_list = cb_sort_list(skip_list, skip_list_length); 

  prn_number = skip_list_length/3;
  MSG_HIGH_DS_1(cb_as_id,"BMC: Skip sequence length is %d :",skip_list_length) ;
  for (dindex=0; dindex<prn_number; dindex++)
  {
    MSG_HIGH_DS_3(cb_as_id,"BMC skip: %d %d %d",skip_list[dindex*3],skip_list[dindex*3 + 1],skip_list[dindex*3 + 2]) ;
  }
  for(prn_number = dindex*3;prn_number < skip_list_length;prn_number++)
  {
    MSG_HIGH_DS_1(cb_as_id,"BMC skip: %d",skip_list[prn_number]) ;
  }
  MSG_HIGH_DS_0(cb_as_id,"BMC: Sending L1_BMC_SKIP_REQ");
#ifdef FEATURE_SEGMENT_LOADING
  IWCDMA_l1_put_cmd(ptr_W, cmd_ptr );
#else
  l1_put_cmd(cmd_ptr);
#endif
}
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
void cb_start_umts_non_drx_mode
(
  void
)
{
  l1_ext_cmd_type  *cmd_ptr;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = get_wcdma_interface();
  interface_t *ptr_W_MMCP = get_wcdma_mmcp_interface();
#endif

#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
  /*Cancel the scheduling*/
  cb_bmc_cancel_scheduling();
#endif

#ifdef FEATURE_SEGMENT_LOADING
  if (ptr_W == NULL)
      return;
  cmd_ptr = IWCDMA_l1_get_cmd_buf(ptr_W);
#else
  cmd_ptr = l1_get_cmd_buf();
#endif

  if (cmd_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id,"BMC: Not enough memory to send WCDMA L1 request",0,0,0) ;
  }

  cmd_ptr->cmd_hdr.cmd_id = CPHY_BMC_SCHEDULING_REQ ;
  cmd_ptr->cmd.bmc_scheduling_req.scheduling_mode = CTCH_NON_DRX_MODE ;

#ifdef FEATURE_DUAL_SIM
  cmd_ptr->cmd.bmc_scheduling_req.as_id = (sys_modem_as_id_e_type) cb_as_id;
#endif

#if defined FEATURE_UMTS_BMC_DRX_SUPPORTED
  cmd_ptr->cmd.bmc_scheduling_req.bmc_schedule_msg_periodicity = cb_bmc_schedule_msg_periodicity;

  if(cb_bmc_schedule_msg_periodicity != 0)
  {
    cb_active_substate_control(CB_STD_NON_DRX_P_ACTIVATED);
  }
  else
#endif
  {
    cb_active_substate_control(CB_STD_NON_DRX_ACTIVATED);
  }

  MSG_HIGH_DS_0(cb_as_id,"BMC: Sending CPHY_BMC_SCHEDULING_REQ to WCDMA L1") ;

  cb_send_event_ctch_start(CTCH_NON_DRX_MODE,0);
  
#ifdef FEATURE_SEGMENT_LOADING
  IWCDMA_dl_dec_register_ctch_pdu_decode_error_cb(ptr_W_MMCP, &cb_ctch_pdu_decode_error);
#else
  dl_dec_register_ctch_pdu_decode_error_cb(&cb_ctch_pdu_decode_error);
#endif

#ifdef FEATURE_SEGMENT_LOADING
  IWCDMA_l1_put_cmd(ptr_W, cmd_ptr );
#else
  l1_put_cmd(cmd_ptr);
#endif
} /* end cb_start_umts_non_drx_mode() */

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

sys_plmn_id_s_type cb_convert_rrc_plmn_id_to_sys_plmn_id( rrc_plmn_identity_type plmn_id )
{
   sys_plmn_id_s_type nas_plmn_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if (plmn_id.num_mnc_digits == 2)
   {
      plmn_id.mnc[2] = 0x0F;
   }

   /* ----------------------------------------
   ** PLMN octet 1 = MCC digit 2 | MCC digit 1
   ** ---------------------------------------- */
   nas_plmn_id.identity[0] = (byte) ((plmn_id.mcc[1] << 4) + plmn_id.mcc[0]);

   /* ----------------------------------------
   ** PLMN octet 2 = MNC digit 3 | MCC digit 3
   ** ---------------------------------------- */
   nas_plmn_id.identity[1] = (byte) ((plmn_id.mnc[2] << 4) + plmn_id.mcc[2]);

   /* ----------------------------------------
   ** PLMN octet 3 = MNC digit 2 | MNC digit 1
   ** ---------------------------------------- */
   nas_plmn_id.identity[2] = (byte) ((plmn_id.mnc[1] << 4) + plmn_id.mnc[0]);

   return nas_plmn_id;

} /* end mmumts_convert_rrc_plmn_id_to_nas_plmn_id() */

/*===========================================================================

FUNCTION   CB_BMC_CLEAN_COMMAND_QUEUE

DESCRIPTION
   This function cleans up the messages in the command queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_clean_command_queue
(
  gs_queue_id_T gs_queue_id 
)
{
  gs_clear_messages(gs_queue_id);
}

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
void bmc_rlc_state_init()
{
  /*Initialize the rlc_id*/
  bmc_rlc_state.rlc_lc_id = 0;

  /* Initializes the watermark queue */
  dsm_queue_init( &(bmc_rlc_state.rlc_rx_wm_item),
                             24000,
                             &(bmc_rlc_state.rlc_rx_q) ) ;

  bmc_rlc_state.rlc_wm_reg_flag = FALSE;

}

/*===========================================================================

FUNCTION   BMC_SETUP_RLC_TO_BMC_WM

DESCRIPTION
   This function initializes the RLC->BMC watermark

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bmc_setup_rlc_to_bmc_wm
(
  bmc_rlc_state_type *bmc_rlc_ptr
)
{
  
  if (bmc_rlc_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  /*-------------------------------------------------------------------------
   Set up RLC->BMC watermark - used by BMC task
  -------------------------------------------------------------------------*/
  bmc_rlc_ptr->rlc_rx_wm_item.lo_watermark         = 4000;
  bmc_rlc_ptr->rlc_rx_wm_item.hi_watermark         = 20000;
  bmc_rlc_ptr->rlc_rx_wm_item.dont_exceed_cnt      = 24000;


  /*-----------------------------------------------------------------------
  Pass the call instance in watermarks registered so that we
  can identify the call when the callback is called.
  BMC will UM mode and will not register for any callbacks
  -----------------------------------------------------------------------*/

  bmc_rlc_ptr->rlc_rx_wm_item.gone_empty_func_ptr  = NULL;
  bmc_rlc_ptr->rlc_rx_wm_item.lowater_func_ptr     = NULL;
  bmc_rlc_ptr->rlc_rx_wm_item.hiwater_func_ptr     = NULL;               
  bmc_rlc_ptr->rlc_rx_wm_item.non_empty_func_ptr   = NULL;

  /*-------------------------------------------------------------------------
    Reset the flag which keeps track if we have sent a close req to RLC
    to tigger flow controlling the peer.
  -------------------------------------------------------------------------*/
  bmc_rlc_ptr->sent_close_to_rlc = FALSE;

}

/*===========================================================================

FUNCTION   BMC_SEND_RLC_REGISTER_WM_CMD

DESCRIPTION
   This function sends the register watermark command to RLC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void bmc_send_rlc_register_wm_cmd
(
  /*BMC RLC state pointer*/
  bmc_rlc_state_type     *bmc_rlc_state_ptr,
  /*Call back function pointer for DL WM*/
  void                   (*from_rlc_post_rx_fn_ptr)(byte, byte,void*)
)
{

  l2_dl_cmd_type *l2_dl_cmd;
  rlc_dl_register_srvc_type bmc_rlc_dl_reg_params;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = get_wcdma_interface();
  if (ptr_W == NULL)
      return;
#endif
  if (bmc_rlc_state_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

#ifdef FEATURE_SEGMENT_LOADING
  if ((l2_dl_cmd = IWCDMA_l2_dl_get_cmd_buf(ptr_W)) == NULL)
  {
    MSG_FATAL_DS( cb_as_id,"NULL Ptr when sending RLC_DL_REGISTER_SRVC_REQ",0,0,0);
  }
#else
  if ((l2_dl_cmd = l2_dl_get_cmd_buf()) == NULL)
  {
    MSG_FATAL_DS( cb_as_id,"NULL Ptr when sending RLC_DL_REGISTER_SRVC_REQ",0,0,0);
  }
#endif
  else /* a good ptr for RLC Data SAP BCCH data has been received */
  {
    /*-----------------------------------------------------------------------
      Downlink Parameters :
      Number of downlink logical channels to register 
      Downlink RLC logical channel ID 
      Downlink Watermark
    -----------------------------------------------------------------------*/

    bmc_rlc_dl_reg_params.nchan = 1;
    bmc_rlc_dl_reg_params.rlc_data[ 0 ].lc_id = 
                                     bmc_rlc_state_ptr->rlc_lc_id;
    bmc_rlc_dl_reg_params.rlc_data[ 0 ].dl_wm_ptr = 
                                     &bmc_rlc_state_ptr->rlc_rx_wm_item;

    bmc_rlc_dl_reg_params.rlc_data[ 0 ].context = FALSE;  /* Task context */

    /*-----------------------------------------------------------------------
     Set the RLC callback function to bmc_process_dsm_data_cb  
    -----------------------------------------------------------------------*/
    bmc_rlc_dl_reg_params.rlc_data[ 0 ].rlc_post_rx_proc_func_ptr = 
                                     from_rlc_post_rx_fn_ptr;
    bmc_rlc_dl_reg_params.rlc_data[ 0 ].rlc_post_rx_func_ptr_para = 
                                        NULL;

    /*-----------------------------------------------------------------------
     Register the watermarks with RLC 
    -----------------------------------------------------------------------*/
    l2_dl_cmd->cmd_hdr.cmd_id = RLC_DL_REGISTER_SRVC_REQ;
    l2_dl_cmd->cmd_data.dl_reg = bmc_rlc_dl_reg_params;
#ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_l2_dl_put_cmd(ptr_W, l2_dl_cmd);
#else
    l2_dl_put_cmd(l2_dl_cmd);
#endif
  }
}


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
)
{
 
  /*Deregister the old WM and register back with new LC id*/
  
  MSG_HIGH_DS_2(cb_as_id,"LC id changed from %d to %d register back with WM",
            bmc_rlc_state.rlc_lc_id, rlc_lc_id);

  /*Deregister with old water mark in case it is registered*/
  if (bmc_rlc_state.rlc_wm_reg_flag)
  {
    MSG_HIGH_DS_1(cb_as_id,"Deregister with RLC WM with LC id %d", bmc_rlc_state.rlc_lc_id);
    /*Deregister with the old WM*/
    bmc_deregister_wm_with_rlc(&bmc_rlc_state);

    /*Celan up the DL RLC WM*/
     bmc_cleanup_rlc_watermarks(&bmc_rlc_state);

     /*Clean the RLC Queue*/
     cb_bmc_clean_command_queue(GS_QUEUE_RLC_CB);

     /*Reset the PDU counter*/
     pdu_cnt = 1;

    /*Reset the rlc_wm_reg_flag*/
    bmc_rlc_state.rlc_wm_reg_flag = FALSE;      
  }
  
  if (rlc_lc_id != 0xFF)
  {
    /*Update the new LC id*/
    bmc_rlc_state.rlc_lc_id = rlc_lc_id;

    /*Initialize the new water mark*/
    bmc_setup_rlc_to_bmc_wm(&bmc_rlc_state);

    /*-------------------------------------------------------------------------
    Set the Current count in the RLC->BMC watermark to 0.
    -------------------------------------------------------------------------*/
    bmc_rlc_state.rlc_rx_wm_item.current_cnt          = 0;

    bmc_rlc_state.rlc_rx_wm_item.total_rcvd_cnt       = 0;

    bmc_send_rlc_register_wm_cmd(&bmc_rlc_state, bmc_process_dsm_data_cb);

    /*set the rlc_wm_reg_flag*/
    bmc_rlc_state.rlc_wm_reg_flag = TRUE;
  }
  else
  {
    MSG_ERROR_DS_1(cb_as_id,"BMC: Invalid LC ID %d", rlc_lc_id);
  }/*if (rlc_lc_id != 0xFF)*/

}




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
)
{
  l2_dl_cmd_type               *l2_dl_cmd_ptr; /* Pointer to L2 DL command */
  rlc_dl_deregister_srvc_type  bmc_rlc_dl_dereg_params; 
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = get_wcdma_interface();
  if (ptr_W == NULL)
      return;
#endif

  if (bmc_rlc_state_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  /*-------------------------------------------------------------------------
   Deregister the RLC  DL watermarks
  -------------------------------------------------------------------------*/
  
  bmc_rlc_dl_dereg_params.nchan = 1;
  bmc_rlc_dl_dereg_params.rlc_id[ 0 ] = bmc_rlc_state_ptr->rlc_lc_id;

  /*-------------------------------------------------------------------------
    Get an L2 DL command buffer
  -------------------------------------------------------------------------*/
#ifdef FEATURE_SEGMENT_LOADING
  if ((l2_dl_cmd_ptr = IWCDMA_l2_dl_get_cmd_buf(ptr_W)) != NULL)
#else
  if ( ( l2_dl_cmd_ptr = l2_dl_get_cmd_buf() ) != NULL )
#endif
  {
    l2_dl_cmd_ptr->cmd_hdr.cmd_id = RLC_DL_DEREGISTER_SRVC_REQ;
    l2_dl_cmd_ptr->cmd_data.dl_dereg = bmc_rlc_dl_dereg_params;

#ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_l2_dl_put_cmd(ptr_W, l2_dl_cmd_ptr);
#else
    l2_dl_put_cmd( l2_dl_cmd_ptr );
#endif
  }
  else
  {
    MSG_FATAL_DS( cb_as_id, "Cannot get L2 DL cmd buffer",0,0,0);
  }
}

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
)
{
  dsm_item_type      *item_ptr;    /* Payload */

  if (bmc_rlc_state_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  /*Free the DSM packets buffered in WM*/
  while ((item_ptr = 
         (dsm_item_type *)dsm_dequeue(&bmc_rlc_state_ptr->rlc_rx_wm_item)) != NULL)
  {
    dsm_free_packet(&item_ptr);
  }

  /*-----------------------------------------------------------------------
    Set the Current count in the  RLC->BMC watermark to 0.
  -----------------------------------------------------------------------*/
  bmc_rlc_state_ptr->rlc_rx_wm_item.current_cnt          = 0;

  /*-----------------------------------------------------------------------
    Set the total_rcvd_cnt in the  and RLC->BMC watermark to 0.
  -----------------------------------------------------------------------*/
  bmc_rlc_state_ptr->rlc_rx_wm_item.total_rcvd_cnt       = 0;
}

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
/*lint -e715*/
void bmc_process_dsm_data_cb
(
 byte rlc_id,
 byte no_pdus,
 void *callback_data
)
{
  gs_status_T          gs_status;  
  dl_bmc_block_ind_T   bmc_block_ind;

  bmc_block_ind.message_header.message_set = MS_RLC_BMC;
  bmc_block_ind.message_header.message_id = (byte)DL_BMC_BLOCK_IND;
  bmc_block_ind.pdu_cnt = pdu_cnt;

 
  PUT_IMH_LEN(sizeof(dl_bmc_block_ind_T) - sizeof(IMH_T),
              &bmc_block_ind.message_header);
  /*Set the command to cb task so that cb task can process 
    the dsm item*/
  gs_status = gs_send_message(GS_QUEUE_RLC_CB,
                              &bmc_block_ind,
                              TRUE);

  if (gs_status != GS_SUCCESS)
  {
    /*Increment the PDU counter*/
    pdu_cnt++;

    if (pdu_cnt >= MAX_PDU_THRESHOLD)
    {
      MSG_HIGH_DS_0(cb_as_id,"BMC: Stop CTCH monitoring due to PDU buffering exceed threshold");
      /*Enable the suspend flag*/
      bmc_rx_suspended = TRUE;
      /*Deactivate the CTCH monitoring*/
      cb_bmc_deactivate_ctch_monitoring();
    }

    MSG_ERROR_DS_0(cb_as_id,"ERROR: Unable to send dl_cb_block_ind message");
  }
  else{
    pdu_cnt = 1;
  }
  
}
/*lint +e715*/
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
dsm_item_type *bmc_get_dsm_item()
{
  return (dsm_item_type *)dsm_dequeue(&bmc_rlc_state.rlc_rx_wm_item);
}

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
)
{
  word pdu_len = 0;
  
  while(pkt_head_ptr != NULL)
  {
    if(CB_BMC_MAX_RLC_DATA_LENGTH - pdu_len >= pkt_head_ptr->used)
    {
      pdu_len += pkt_head_ptr->used;
      pkt_head_ptr = pkt_head_ptr->pkt_ptr;
    }
    else
    {
      MSG_HIGH_DS_2(cb_as_id,"CB: Truncating RLC PDU as PDU length %d is beyond the MAX possible length %d",pdu_len,CB_BMC_MAX_RLC_DATA_LENGTH) ;
      pdu_len = CB_BMC_MAX_RLC_DATA_LENGTH;
      break;
    }
  }
  return pdu_len;
}

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
)
{
  word  size_moved = 0;

  if (rlc_pdu_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  while(item_ptr != NULL)
  {
    if(item_ptr->used)
    {
      
      /*
      ** Integer overflow is already taken care in the function 
      ** bmc_compute_rlc_pdu_len
      ** Effective length of the buffer available for further copy is
      ** pdu_len - already copied length
      */
      if((pdu_len - size_moved) <= item_ptr->used)
      {
        MSG_HIGH_DS_0(cb_as_id,"CB: Copied RLC PDU to the Truncated length");
        (void)memscpy((void *)(rlc_pdu_ptr + size_moved),(pdu_len - size_moved),(void *)item_ptr->data_ptr,(pdu_len - size_moved));
        break;
      }
      else
      {
        (void)memscpy((void *)(rlc_pdu_ptr + size_moved),item_ptr->used,(void *)item_ptr->data_ptr,item_ptr->used);
        size_moved += item_ptr->used;
        item_ptr = item_ptr->pkt_ptr;
      }
    }
    else
    {
      /*
      ** If length of DSM item is zero then go to next 
      ** DSM item ptr
      */
      item_ptr = item_ptr->pkt_ptr;
    }
  }
}

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
)
{
  byte mask = 0x1;
  byte bit_pos, shift_value;
  word byte_pos;

  if (rlc_pdu_ptr == NULL)
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer", 0,0,0 );
  }

  for (byte_pos = 0; byte_pos < pdu_size; byte_pos++)
  {
    shift_value = rlc_pdu_ptr[byte_pos];
    for (bit_pos = 1; bit_pos <= OCTET_BIT_LEN; bit_pos++)
    {
      if (shift_value & mask)
      {
        rlc_pdu_ptr[byte_pos] |= 
                             (byte)(mask << (OCTET_BIT_LEN - bit_pos));
      }
      else
      {
        rlc_pdu_ptr[byte_pos] &= 
                             ~( byte )(mask << (OCTET_BIT_LEN - bit_pos));
      }
      shift_value >>= 1;
    }
  }
}

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
word cb_bmc_get_mask
(
  byte bit_number
)
{
  byte mask_index; 
  word mask = 2;

  if (bit_number == 0)
  {
    MSG_FATAL_DS( cb_as_id,"Invalid number of PDU per page",0,0,0) ;
  } else if (bit_number == 1)
  {
    return 0x01 ;
  } else
  {
    for (mask_index=0; mask_index<(bit_number-1); mask_index++)
    {
      mask *= 2 ;
    }
  }
    
  return (mask-1) ;

} /* end of cb_bmc_get_mask() */

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
dword cb_bmc_get_Ki_sfn
(
  dword incoming_sfn ,
  word  rlc_pdu_number ,
  word  ctch_allocation_period,
  word  ctch_offset
)
{
  dword absolute_sfn = (dword)(incoming_sfn + rlc_pdu_number * ctch_allocation_period) ;
  word absolute_sfn_cycle_index = (word) (absolute_sfn/(CB_BMC_MAX_SFN+1));
  word current_sfn_cycle_index  = (word) (incoming_sfn/(CB_BMC_MAX_SFN+1));

  if(absolute_sfn_cycle_index > current_sfn_cycle_index)
  {
    dword absolute_sfn_cycle_first_ctch_sfn = ctch_offset + (current_sfn_cycle_index*(CB_BMC_MAX_SFN+1));

    absolute_sfn = incoming_sfn;
    while (rlc_pdu_number > 0)
    {
      absolute_sfn += ctch_allocation_period;
      rlc_pdu_number--;
      if(absolute_sfn >= ((dword)(current_sfn_cycle_index+1)*(CB_BMC_MAX_SFN+1)))
      {
        absolute_sfn_cycle_first_ctch_sfn += (CB_BMC_MAX_SFN+1);
        absolute_sfn = absolute_sfn_cycle_first_ctch_sfn;
        current_sfn_cycle_index++;
      }
    }

  }

  while (absolute_sfn > CB_BMC_MAX_ROLLOVER_SFN) 
  {
    absolute_sfn -= CB_BMC_MAX_ROLLOVER_SFN+1 ;
  }

  return (dword)(absolute_sfn) ;
} /* end of cb_bmc_get_Ki_sfn() */

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
void cb_bmc_clean_up_obsolete_page_from_list
(
  cb_rat_type rat_to_be_deleted,
  cb_message_list_type *list
)
{
  cb_user_page_info_type *cb_received_message_code = NULL ;

  if (list == NULL)
  {
    return ; /* Nothing to do */
  }

  while (list != NULL)
  {
    cb_received_message_code = list->message_header.message_info ;

    while (cb_received_message_code != NULL)
    {
      list->message_header.current_message_descriptor = 
                                                     cb_received_message_code ;

      if (cb_received_message_code->missing_page_list != NULL)
      {
        cb_missing_page_type *missing_page = 
                                 cb_received_message_code->missing_page_list ;

        while (missing_page != NULL)
        {
          if (missing_page->received_rat == rat_to_be_deleted)
          {
            page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;

#ifdef FEATURE_UMTS_BMC
            /* Make sure BMC to stop processing remaining message PDUs if any */
            cb_bmc_msg_sfn_allocation_length = 0 ;
#endif
            cb_received_message_code->missing_page_list = 
                                            cb_delete_page(list,missing_page) ;

            missing_page = cb_received_message_code->missing_page_list ;
          }
          else
          {
            missing_page = missing_page->next_missing_page ;
          }
        }
      }
      cb_received_message_code = cb_received_message_code->next_message_code ;
    } /* while (cb_received_message_code != NULL) */

    list = list->next_message ;
  }
} /* end cb_bmc_clean_up_obsolete_page_from_list() */

void cb_rlc_ctch_pdu_dropped(dword pdu_sfn)
{
  l1_bmc_dropped_pdu_ind_type l1_bmc_dropped_pdu_ind;
  gs_status_T    gs_status;

  MSG_HIGH_DS_0(cb_as_id,"CB: Received Dropped PDU Indication from RLC") ;
  l1_bmc_dropped_pdu_ind.message_header.message_set = MS_BMC_L1;
  l1_bmc_dropped_pdu_ind.message_header.message_id  = L1_BMC_DROPPED_PDU_IND;
  l1_bmc_dropped_pdu_ind.pdu_sfn = pdu_sfn;

  PUT_IMH_LEN( sizeof(l1_bmc_dropped_pdu_ind_type) - 
                                    sizeof(IMH_T),&l1_bmc_dropped_pdu_ind.message_header ) ;

  gs_status = gs_send_message(GS_QUEUE_CB,&l1_bmc_dropped_pdu_ind,TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Unable to post Dropped PDU indication to ourselves");
  }
}

void cb_ctch_pdu_decode_error(dword pdu_sfn)
{
  l1_bmc_dropped_pdu_ind_type l1_bmc_dropped_pdu_ind;
  gs_status_T    gs_status;

  MSG_HIGH_DS_0(cb_as_id,"CB: Received Dropped PDU Indication from WL1") ;
  l1_bmc_dropped_pdu_ind.message_header.message_set = MS_BMC_L1;
  l1_bmc_dropped_pdu_ind.message_header.message_id  = L1_BMC_DROPPED_PDU_IND;
  l1_bmc_dropped_pdu_ind.pdu_sfn = pdu_sfn;

  PUT_IMH_LEN( sizeof(l1_bmc_dropped_pdu_ind_type) - 
                                    sizeof(IMH_T),&l1_bmc_dropped_pdu_ind.message_header ) ;

  gs_status = gs_send_message(GS_QUEUE_CB,&l1_bmc_dropped_pdu_ind,TRUE);

  if (gs_status != GS_SUCCESS)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Unable to post Dropped PDU indication to ourselves");
  }
}
#endif/*FEATURE_UMTS_BMC*/

void cb_send_event_ctch_start
(
  byte ctch_mode,
  byte scheduling_period_length
)
{
  byte rat_type = bmc_active_rat;
  dword cb_scheduling_period_length_in_sfn = 0;
  byte event_packet[10];

  if(rat_type == CB_UMTS_MODE)
  {
    cb_scheduling_period_length_in_sfn = cb_bmc_scheduling_period_length * cb_bmc_ctch_allocation_period;
  }
  else
  {
    cb_scheduling_period_length_in_sfn = scheduling_period_length * CB_MULTIFRAME_LENGTH;// hgfhgf;
  }


  /* Event Packet(EVENT_UMTS_NAS_CB_CTCH_START) creation*/
  memset((void *)event_packet,0x00,10);
#ifdef FEATURE_DUAL_SIM
  event_packet[0] = cb_as_id;
#else
  event_packet[0] = 0;
#endif
  event_packet[1] = rat_type;
  event_packet[2] = ctch_mode;
  memscpy(event_packet+3,2,(const void*)(&cb_bmc_ctch_allocation_period),2);
  if(rat_type == CB_UMTS_MODE)
  {
    event_packet[5] = cb_bmc_scheduling_period_length;
  }
  else
  {
    event_packet[5] = scheduling_period_length;
  }
  memscpy(event_packet+6,4,(const void*)(&cb_scheduling_period_length_in_sfn),4);
  (void) event_report_payload(EVENT_UMTS_NAS_CB_CTCH_START, 10, (void *)event_packet);
  MSG_HIGH_DS_0(cb_as_id,"CB: Sending event CTCH Start") ;
}

void cb_send_event_ctch_stop(void)
{
  byte event_packet[2];

  /* Event Packet(EVENT_NAS_CB_CTCH_STOP) creation*/
  memset((void *)event_packet,0x00,2);
#ifdef FEATURE_DUAL_SIM
  event_packet[0] = cb_as_id;
#else
  event_packet[0] = 0;
#endif
  event_packet[1] = bmc_active_rat;
  (void) event_report_payload(EVENT_NAS_CB_CTCH_STOP, 2, (void *)event_packet);
    MSG_HIGH_DS_0(cb_as_id,"CB: Sending event CTCH Stop") ;
}


void cb_ctch_pdu_dropped(dword pdu_sfn)
{
  byte rat_type = bmc_active_rat;
  byte  cb_msg_type = 0;
  dword cb_rlc_ctch_dropped_pdu_sfn = pdu_sfn;
  dword cb_dropped_pdu_sdu_start_sfn = 0;
  word  cb_msg_id = 0x0000;
  byte event_packet[13];
  cb_message_list_type * list = basic_cbch_list_entry_point;
  cb_first_tx_schedule_type *tx_list = NULL;
  cb_retx_schedule_type *retx_list = NULL;
  boolean pdu_found = FALSE;
  dword correction = 0;
  cb_bmc_new_msg_bit_map_info_t *bmap = curr_new_msg_bitmap;

  if(rat_type == CB_UMTS_MODE)
  {
    correction = cb_bmc_ctch_allocation_period-1;
  }
  else if(rat_type == CB_GSM_MODE)
  {
    correction = (CB_MULTIFRAME_LENGTH * CB_PAGING_MAX_BLOCKS) - 1;
  }

  if(bmap == NULL)
  {
    bmap = prev_new_msg_bitmap;
  }

  while(bmap != NULL && pdu_found == FALSE)
  {
    if(bmap->sfn <= cb_rlc_ctch_dropped_pdu_sfn
        && cb_rlc_ctch_dropped_pdu_sfn <= bmap->sfn+correction)
    {
      cb_msg_id = bmap->msg_id;
      cb_dropped_pdu_sdu_start_sfn = bmap->sfn;
      if(cb_msg_id == 0xFFFF)
      {
        cb_msg_type = 1;
      }
      else
      {
        cb_msg_type = 0;
      }
      pdu_found = TRUE;
    }
    bmap = bmap->next_ptr;
  }

  if(pdu_found == FALSE && list == NULL)
  {
    return;
  }
  /* TX for Both SM and DM*/
  while(pdu_found == FALSE && list != NULL)
  {
    tx_list = list->message_header.next_first_tx_schedule;
    if(tx_list != NULL)
    {
      retx_list = tx_list->retx_schedule;
    }
    else
    {
      tx_list = list->message_header.first_tx_schedule;
      if(tx_list != NULL)
      {
          retx_list = tx_list->retx_schedule;
      }
    }
    /*RETX*/
    while(retx_list != NULL)
    {
      if(retx_list->retx_fn <= cb_rlc_ctch_dropped_pdu_sfn
          && cb_rlc_ctch_dropped_pdu_sfn <= retx_list->retx_fn+correction)
      {
        cb_msg_id = list->message_header.message_id;
        cb_dropped_pdu_sdu_start_sfn = retx_list->retx_fn;
        pdu_found = TRUE;
        break;
      }
      retx_list = retx_list->next_ptr;
    }
    list = list->next_message;
  }

  if(pdu_found == TRUE)
  {
    /* Event Packet(EVENT_UMTS_NAS_CB_BMC_MSG_DECODE_FAIL) creation*/
    memset((void *)event_packet,0x00,13);
#ifdef FEATURE_DUAL_SIM
    event_packet[0] = cb_as_id;
#else
    event_packet[0] = 0;
#endif
    event_packet[1] = rat_type;
    event_packet[2] = cb_msg_type;
    memscpy(event_packet+3,4,(const void*)(&cb_rlc_ctch_dropped_pdu_sfn),4);
    memscpy(event_packet+7,4,(const void*)(&cb_dropped_pdu_sdu_start_sfn),4);
    memscpy(event_packet+11,2,(const void*)(&cb_msg_id),2);
    (void) event_report_payload(EVENT_UMTS_NAS_CB_BMC_MSG_DECODE_FAIL, 13, (void *)event_packet);
    MSG_HIGH_DS_1(cb_as_id,"CB: Sending event CB_PDU_DROPPED for SFN:%u",pdu_sfn) ;
  }
  else
  {
    MSG_HIGH_DS_1(cb_as_id,"CB:Dropped PDU %u not found",pdu_sfn) ;
  }
}

/*===========================================================================

FUNCTION   CB_GET_NV_ITEMS

DESCRIPTION
  This function gets CB items from NV 
  cb_duplication_detection_disabled : If true, stop discarding CB retransmissions at NAS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_get_nv_items(mm_nas_nv_items_T *mm_nas_nv_items_p)
{
  if(mm_nas_nv_items_p == NULL)
  { 
    return;
  }

  if (mm_nas_nv_items_p->version >= 3)
  {
    cb_nv_duplication_detection_disabled = 
     (mm_nas_nv_items_p->cb_duplication_detection_disabled != 0) ? TRUE : FALSE ;
  }

  if(mm_nas_nv_items_p->version >= 5)
  {
    if(mm_nas_nv_items_p->cb_drx_info_validation_enabled > 0)
    {
      cb_drx_info_validation_required=FALSE;
    }
  }

#ifdef FEATURE_GSM_CB_PSEUDO_DRX
  if (mm_nas_nv_items_p->version >= 1)
  {
    if (mm_nas_nv_items_p->cb_p_drx_enabled == 0)
    {
      cb_pseudo_drx_supported = FALSE;
    }
    if (mm_nas_nv_items_p->cb_p_drx_pre_drx_length <= MAX_SCHEDULING_PERIOD_LENGTH)
    {
      /* pre scan can not be 0, since we need to decode atleast one CBCH to move to pseudo DRX */
      cb_pre_pseudo_drx_scan_length = ((mm_nas_nv_items_p->cb_p_drx_pre_drx_length == 0) ? 1 : mm_nas_nv_items_p->cb_p_drx_pre_drx_length);
    }
    if (mm_nas_nv_items_p->cb_p_drx_first_drx_periodicity > 0 && mm_nas_nv_items_p->cb_p_drx_first_drx_periodicity < MAX_SCHEDULING_PERIOD_LENGTH)
    {
      cb_first_pseudo_drx_periodicity     = mm_nas_nv_items_p->cb_p_drx_first_drx_periodicity;
    }
    if (mm_nas_nv_items_p->cb_p_drx_second_drx_periodicity > 0 && mm_nas_nv_items_p->cb_p_drx_second_drx_periodicity < MAX_SCHEDULING_PERIOD_LENGTH)
    {
      cb_second_pseudo_drx_periodicity     = mm_nas_nv_items_p->cb_p_drx_second_drx_periodicity;
    }
  }
#endif
  if (mm_nas_nv_items_p->version >= 4)
  {
    MSG_HIGH_DS_1(cb_as_id,"CB: GSM CB feature status 0/1 enable/disable : %d",mm_nas_nv_items_p->cb_gsm_feature_disabled) ;
    if (mm_nas_nv_items_p->cb_gsm_feature_disabled > 0)
    {
      cb_gsm_feature_disabled = TRUE;
    }
  }

}

void cb_send_ota_msg_log_packet(uint8 rat_type,uint32 msg_size,uint8 msg_type, byte   *msg_ptr)
{
LOG_UMTS_NAS_CB_LOG_PACKET_C_type *log_ptr;
uint32                            pkt_size;

pkt_size = FPOS(LOG_UMTS_NAS_CB_LOG_PACKET_C_type, cb_ota_log_packet) + msg_size;
log_ptr = (LOG_UMTS_NAS_CB_LOG_PACKET_C_type *)log_alloc(LOG_UMTS_NAS_CB_LOG_PACKET_C, pkt_size);

if (log_ptr == NULL)
  {
        /* Could not allocate buffer */
    return;
  }                
#ifdef FEATURE_DUAL_SIM
log_ptr->as_id = (uint8) cb_as_id;
#else
log_ptr->as_id = (uint8) 0;
#endif
log_ptr->version=0; /* using 0 for now as default, need to check qxdm team and upate*/
log_ptr->rat_type = rat_type;
log_ptr->cb_msg_length    = msg_size;
log_ptr->msg_type= msg_type; 
memscpy((byte*)(log_ptr->cb_ota_log_packet),sizeof(log_ptr->cb_ota_log_packet),msg_ptr,msg_size);

/* Send the packet */
     log_commit(log_ptr);                
}
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
void cb_dup_start_timer( timer_id_T timer_id, dword num_millisecs )
{
   gs_status_T    status;

   if (num_millisecs == 0)
   {
     MSG_HIGH_DS_1(cb_as_id,"Cannot Start Timer %d with zero time out value",timer_id);
     return;
   }

   status = gs_start_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(timer_id), cb_timer_expiry, num_millisecs, (byte) 0 );

   cb_timer_status [timer_id-1]= TIMER_ACTIVE;

   MSG_HIGH_DS_3(cb_as_id,"Start %d, timeout %lu:%lu",
             timer_id, (dword) (num_millisecs /1000L), (dword)(num_millisecs % 1000L) );


   if ( status != GS_SUCCESS )
   {
      MSG_ERROR_DS_1(cb_as_id,"Start timer failure: Status %d", status);
   }
}

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
void cb_dup_stop_timer( timer_id_T timer_id )
{
   gs_status_T gs_status;
   if (cb_timer_status[timer_id-1] != TIMER_STOPPED)
   {
      MSG_HIGH_DS_1(cb_as_id,"Stopping %d", timer_id);

      gs_status = gs_cancel_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(timer_id) );

      if (gs_status != GS_SUCCESS)
      {
         MSG_ERROR_DS_0(cb_as_id,"Stop timer failure");
      }
      cb_timer_status[timer_id-1] = TIMER_STOPPED;
   }
 }

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
void cb_update_cache_timer_offsets
(
  dword num_remaining_ms
)
{
  cb_user_page_info_type *cb_current_message_code = NULL ;
  cb_user_page_info_type *cb_prev_message_code = NULL ;
  cb_message_list_type *list = NULL ;
  cb_message_list_type *prev_list = NULL;
  
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
  cb_bmc_etws_msg_info_type *cb_current_message_code_etws = NULL ;
  cb_bmc_etws_msg_info_type *cb_prev_message_code_etws = NULL ;
  cb_bmc_etws_msg_list_type *list_etws = NULL ;
  cb_bmc_etws_msg_list_type *prev_list_etws = NULL;
#endif
  
  dword min_offset = cb_next_min_offset;
  cb_next_min_offset = CB_DUP_TIMER_VALUE_INVALID;
  
  cb_dup_stop_timer((timer_id_T)CB_TIMER_DUP_DET);
  MSG_HIGH_DS_1(cb_as_id,"CB: Dup Timer -> Min_offset :%d", min_offset/1000L);
  
  list = basic_cbch_list_entry_point;

  while (list != NULL)
  {
    cb_prev_message_code = NULL ;

    cb_current_message_code = list->message_header.message_info ;
	prev_list = list;

    while(cb_current_message_code)
    {
        if(cb_current_message_code->timer_offset == 0)
        {
            list = cb_delete_message_code(list,cb_prev_message_code,cb_current_message_code);

            if (prev_list != list)
            {
                if(current_message == prev_list)
                {
                    current_message = list;
                }
    
                if(basic_cbch_list == prev_list)
                {
                    basic_cbch_list = list;
                }
    
                if(basic_cbch_list_entry_point == prev_list)
                {
                    /* list is right node of previous*/
                    basic_cbch_list_entry_point = list;
                    if(list != NULL)
                    {
                        cb_current_message_code = list->message_header.message_info;
                    }
                    else
                    {
                        cb_current_message_code = NULL;
                    }
                    continue;
                }
            }

            if(cb_prev_message_code)
            {
                cb_current_message_code = cb_prev_message_code->next_message_code;
            }
            else if((prev_list == list && 
                    list && 
                    list->message_header.message_info))
            {
                /* prev code was the header node -> next shall be header */
                cb_current_message_code = list->message_header.message_info;
            }
            else /* list == NULL */
            {
                cb_current_message_code = NULL;
            }
            continue; /* cur code is deleted -> no need to update offset */
        }
       
        cb_prev_message_code = cb_current_message_code;
        cb_current_message_code = cb_current_message_code->next_message_code;
    }
    if(list)
    {
       if(prev_list == list)
		list = list->next_message ;
    }
  }

  list = basic_cbch_list_entry_point;

  while (list != NULL)
  {
     cb_current_message_code = list->message_header.message_info ;
	 while(cb_current_message_code)
    {
       if(min_offset != CB_DUP_TIMER_VALUE_INVALID)
        {
            cb_current_message_code->timer_offset -= min_offset;

            if(cb_current_message_code->timer_offset != 0 && (cb_current_message_code->timer_offset < cb_next_min_offset))
            {
                cb_next_min_offset = cb_current_message_code->timer_offset;
            }
        }
        cb_current_message_code = cb_current_message_code->next_message_code;	  	 
	}
	if(list)
    {
		list = list->next_message ;
    }
  }

  #if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
  
  list_etws = basic_etws_list_entry_point;

  while (list_etws != NULL)
  {
    cb_prev_message_code_etws = NULL ;

    cb_current_message_code_etws = list_etws->message_header.message_info ;
	prev_list_etws = list_etws;

    while(cb_current_message_code_etws)
    {
        if(cb_current_message_code_etws->timer_offset == 0)
        {
            
            list_etws = cb_delete_etws_message_code(list_etws,cb_current_message_code_etws);

            if(prev_list_etws != list_etws)
            {
              if(basic_etws_list == prev_list_etws)
              {
                  basic_etws_list = list_etws;
              }

              if(basic_etws_list_entry_point == prev_list_etws)
              {
                  basic_etws_list_entry_point = list_etws;
                  /* list is right node of previous*/
                  if(list_etws != NULL)
                  {
                      cb_current_message_code_etws = list_etws->message_header.message_info;
                  }
                  else
                  {
                      cb_current_message_code_etws = NULL;
                  }
                  continue;
              }
            }
            if(cb_prev_message_code_etws)
            {
                cb_current_message_code_etws = cb_prev_message_code_etws->next_message_code;
            }
            /* prev code was the header node -> next shall be header */
            else if(prev_list_etws == list_etws && list_etws && list_etws->message_header.message_info)
            {
                cb_current_message_code_etws = list_etws->message_header.message_info;
            }
            else /* list_etws == NULL */
            {
                cb_current_message_code_etws = NULL;
            }
            continue;/* cur code is deleted -> no need to update offset */
        }
       
        cb_prev_message_code_etws = cb_current_message_code_etws;
        cb_current_message_code_etws = cb_current_message_code_etws->next_message_code;
    }
    if(list_etws)
    {
        if(prev_list_etws == list_etws)
		list_etws = list_etws->next_message ;
    }
  }


  list_etws = basic_etws_list_entry_point;
  while (list_etws != NULL)
  {

    cb_current_message_code_etws = list_etws->message_header.message_info ;
	while(cb_current_message_code_etws)
    {
       if(min_offset != CB_DUP_TIMER_VALUE_INVALID)
        {
            cb_current_message_code_etws->timer_offset -= min_offset;
            if(cb_current_message_code_etws->timer_offset != 0 && (cb_current_message_code_etws->timer_offset < cb_next_min_offset))
            {
                cb_next_min_offset = cb_current_message_code_etws->timer_offset;
            }
        }
        cb_current_message_code_etws = cb_current_message_code_etws->next_message_code;	
	}
	if(list_etws)
    {
    	list_etws = list_etws->next_message ;
    }
  }


  #endif
  
  if(min_offset != CB_DUP_TIMER_VALUE_INVALID)
  {
    cb_dup_start_timer((timer_id_T)CB_TIMER_DUP_DET,min_offset+num_remaining_ms);
  }  
} 

void cb_update_cache_for_multiple_message_deletion
(
    boolean timer_offset_0_deleted
)
{
  cb_user_page_info_type *cb_current_message_code = NULL ;
  cb_message_list_type *list = NULL ;

  #if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
  cb_bmc_etws_msg_info_type *cb_current_message_code_etws = NULL ;
  cb_bmc_etws_msg_list_type *list_etws = NULL ;
  #endif

  /*Inside while loop re-calculating the cb_next_min_offset value, so setting to INVALID here */
  cb_next_min_offset = CB_DUP_TIMER_VALUE_INVALID;

  if(basic_cbch_list_entry_point == NULL
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
  && basic_etws_list_entry_point == NULL
#endif
  )
  {
    cb_dup_stop_timer((timer_id_T)CB_TIMER_DUP_DET);
    return;
  }
 
  list = basic_cbch_list_entry_point;

  while (list != NULL)
  {
    cb_current_message_code = list->message_header.message_info ;

    while(cb_current_message_code)
    {
        if(cb_current_message_code->timer_offset != 0 && (cb_current_message_code->timer_offset < cb_next_min_offset))
        {
           cb_next_min_offset = cb_current_message_code->timer_offset;
        }
        cb_current_message_code = cb_current_message_code->next_message_code;
    }
        list = list->next_message ;
  }

#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
  list_etws = basic_etws_list_entry_point;

  while (list_etws != NULL)
  {
    cb_current_message_code_etws = list_etws->message_header.message_info ;

    while(cb_current_message_code_etws)
    {
        if(cb_current_message_code_etws->timer_offset != 0 && (cb_current_message_code_etws->timer_offset < cb_next_min_offset))
        {
           cb_next_min_offset = cb_current_message_code_etws->timer_offset;
        }
        cb_current_message_code_etws = cb_current_message_code_etws->next_message_code;
    }
    list_etws = list_etws->next_message ;
  }
#endif

  if(timer_offset_0_deleted)
  {
    dword x = gs_enquire_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(CB_TIMER_DUP_DET));
    cb_update_cache_timer_offsets(x);
  }
}
