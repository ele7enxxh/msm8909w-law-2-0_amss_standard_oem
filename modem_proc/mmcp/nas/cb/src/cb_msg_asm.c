/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     CB SCHEDULING MODULE (CB_MSG_ASM.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cb_msg_asm.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/14/11  vm       Offtarget fix
12/08/11  jbk      Delete Schedule message from cache after processing it 
08/03/11  jbk      NV support to disable CB duplicate detection.
08/03/11  jbk      CB DRX Begin slot interpretation change.
01/21/08  ab       Removing compiler warnings
09/12/07   jbk     BMC RLC Multiple PDU Support under FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
06/15/07   jbk     clean up LINT errors.
10/19/06   kvk     The current message descriptor will be saved and restored during 
                   message reconstruction
10/18/06   kvk     Added guard code if current page ptr is corrupted, the message descriptor has 
                   to be deleted from the cache and page_assembling_state should be set to CB_RECEIVING_FIRST_PB_STATE
03/27/06   vdr     Reset GSM Skip history when NW retx particular CB page
01/31/05   kvk     Now the PDU will be processed if the current_message pointer is not
                   a NULL pointer and SFN exists in the SFN table
08/03/05   kvk     Added passing data_coding_scheme parameter to be stored in the message ptr and
                   will passed along with wms_data_ind.
07/05/05   kvk     Added validation for Number of pages in CB message header. 
05/16/05   kvk     Added support for DRX optimization to store the status of the 
                   PDU received in the current DRX period and use this history
                   to build the optimized Schedule Request(DRX) to WL1 to avoid receiving
                   the PDUs which already processed in the current DRX period.
05/12/05   kvk     Now searching the SFN allocation table based on the
                   message type. In case of CBS message SFN entry should be present and valid
                   page number. In case of Scheduling message only SFN is verified.          
04/29/05   vdr     Now skipping RLC PDUs which are not carring BMC message payload
                   according to the calculations based on the message header info
04/19/05   kvk     Now passing the frame offset (K) for computation of SFN.
03/28/05   kvk     Added support for UMTS BMC.
01/18/05   vdr     Added support for FEATURE_APIONE feature flag
07/20/04   vdr     Now dropping down unnecessary Data Indication if corresponding 
                   FN has already been skipped
07/14/04   vdr     Added extraction of the Data Coding Scheme field
                   Now sending Duplication Indication to WMS when duplication 
                   is detected for Cell wide, immediate display message.
05/19/04   vdr     Fixed duplication detection
04/16/04   vdr     Featurizing out CB DRX mode processing
                   Added some extra comments
07/03/03   jca     Initial version
02/16/07   NR      Renaming the FEATRUE_APIONE with FEATURE_WMS_CM_NAS_INTERFACES
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_CB

#include <string.h>
#include <stringl/stringl.h>
#include "comdef.h"
#include "cbwmsif.h"
#include "cbmsg.h"
#include "cb_task.h"
#include "cbtask.h"
#include "msg.h"
#include "err.h"
#include "cb_schedule.h"
#include "cb_utils.h"
#include "gs_v.h"
#include "event.h"
#include "cb_state.h"
#ifndef FEATURE_GSM_CB_TEST
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#else
#include "wms_modem.h"
#endif
#else
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#else
extern boolean wms_bc_gw_cb_filter
(
  sys_modem_as_id_e_type as_id,
  uint8 * paging_block
);
#endif
#endif

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#ifdef FEATURE_NAS_CBS_DSDS
extern cb_as_id_e_type cb_as_id;
#endif

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
boolean cb_bmc_validate_schedule_message
(
  dword arrival_sfn,
  byte offset_ctch_bs_index,
  byte scheduling_period_length
);
void cb_bmc_skip_next_reception
(
  cb_message_list_type *message_ptr
#ifdef FEATURE_NAS_REL6
  ,dword data_sn
#endif
);
#endif /*FEATURE_GSM_CB_DRX_SUPPORTED*/
extern void cb_send_ota_msg_log_packet(uint8 rat_type,uint32 msg_size,uint8 msg_type, byte   *msg_ptr);
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
/* Search List Length */ 
byte     search_list_len_sim [MAX_AS_IDS];  /* Search List is EMPTY upon Power Up */
/* Pointer to requested Search List (list of message IDs) */
cb_wms_message_id_value_type search_lst_sim[MAX_AS_IDS][CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED];

/* Pointer to current element of CB task's cache memory */
cb_message_list_type     *basic_cbch_list_sim[MAX_AS_IDS];

cb_message_list_type    *current_message_sim[MAX_AS_IDS];

/* Pointer to first element of CB task's cache memory */
cb_message_list_type                 *basic_cbch_list_entry_point_sim[MAX_AS_IDS];

/* GSM Page Assembly state */
cb_page_assembling_state_type page_assembling_state_sim[MAX_AS_IDS]; 
/* Current PLMN */
byte current_plmn_sim[MAX_AS_IDS][PLMN_VALUE_LENGTH] ;
/* Current LAC */
byte current_lac_sim[MAX_AS_IDS][LAC_VALUE_LENGTH] ;
/* Cuppent Cell ID */
byte current_cell_id_sim[MAX_AS_IDS][CELL_ID_VALUE_LENGTH]  ;

/* Current message type */
static byte message_type_sim[MAX_AS_IDS] = {CBS_NO_MESSAGE_TYPE,CBS_NO_MESSAGE_TYPE};
#define message_type  message_type_sim[cb_as_id]

byte  cb_timer_status_sim[MAX_AS_IDS][CB_MAX_TIMERS];

extern dword cb_dup_timer_value_sim[MAX_AS_IDS];
#define   cb_dup_timer_value cb_dup_timer_value_sim[cb_as_id]
#else
/* Search List Length */
byte  search_list_len ; /* Search List is EMPTY upon Power Up */

/* Pointer to requested Search List (list of message IDs) */
cb_wms_message_id_value_type search_lst[CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED];

/* Pointer to current element of CB task's cache memory */
cb_message_list_type *basic_cbch_list ;
cb_message_list_type *current_message ;

/* Pointer to first element of CB task's cache memory */
cb_message_list_type *basic_cbch_list_entry_point ;


/* GSM Page Assembly state */
cb_page_assembling_state_type page_assembling_state ;

/* Current PLMN */
byte cb_current_plmn[PLMN_VALUE_LENGTH] ;
   
/* Current LAC */
byte current_lac[LAC_VALUE_LENGTH] ;

/* Cuppent Cell ID */
byte current_cell_id[CELL_ID_VALUE_LENGTH] ;

byte  cb_timer_status[CB_MAX_TIMERS];

/* Current message type */
static byte message_type = CBS_NO_MESSAGE_TYPE;

extern dword cb_dup_timer_value;
#endif //ifdef FEATURE_NAS_CBS_DSDS

#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
cb_bmc_etws_msg_list_type *basic_etws_list = NULL;

cb_bmc_etws_msg_list_type *basic_etws_list_entry_point = NULL;
#endif


#define CB_MAX_UPDATE_NUMBER 0x0F
#define CB_UPDATE_NUMBER_MAX_CHANGE 0x08

#ifdef FEATURE_UMTS_BMC

/* 
** PDU Distribution Tablel. This table has a size which is equal to
** number of pages constructing entire BMC message. Every element of
** the table is set to number of page fragments which are constructing
** particular page
*/
static byte cb_bmc_pdu_distribution_table[CB_MAX_NUMBER_OF_PAGES] ;

/* Size of RLC PDU SFN Allocation table */
word cb_bmc_msg_sfn_allocation_length = 0 ;

#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
/* RLC SDU SFN Allocation table */
#define CB_BMC_MAX_SDUS_PER_MSG 1
static cb_bmc_pdu_allocation_type 
  cb_bmc_msg_sfn_allocation[CB_BMC_MAX_SDUS_PER_MSG] ;
#else
/* RLC PDU SFN Allocation table */
static cb_bmc_pdu_allocation_type 
  cb_bmc_msg_sfn_allocation[CB_BMC_MAX_MSG_SFN_ALLOCATION_LENGTH] ;

/* Non DRX Skip Allocation */
static dword cb_bmc_skip_allocation[CB_BMC_MAX_MSG_SFN_ALLOCATION_LENGTH] ;

/* SFN that the next expected BMC message should be transmitted from */
static dword cb_bmc_next_expected_message_sfn = 0xFFFFFFFF ;
#endif

/* Scheduling message Reassembling state */
static cb_bmc_reassembling_state_type cb_bmc_reassembling_state = CB_BMC_BIT_MAP_STATE ;

/* Number of remaining octets in Scheduling RLC PDU */
static byte cb_bmc_remaining_length = 0 ;

/* Number of Scheduling Description fields */
static byte cb_bmc_number_of_descriptions = 0 ;

/* Scheduling Period Length */
static word scheduling_period_length_cb_msg_asm;

/* Entire Sheduling message length */
static word cb_bmc_scheduling_msg_length = 0 ;

/* Reassembly Scheduling buffer */
static byte cb_bmc_scheduling_msg[CB_BMC_MAX_SCHEDULING_LENGTH] ;

#ifdef FEATURE_NAS_REL6
/*
**Spec 25.324 Section 11.12 and 11.13
*/
#define CB_BMC_FUTURE_EXTN_BITMAP_IE_LENGTH 0x01
#define CB_BMC_FUTURE_EXTN_BITMAP_SERIAL_NUMBER_LENGTH_IE_LENGTH 0x01
#define CB_BMC_FUTURE_EXTN_SERIAL_NUMBER_LIST_IE_LENGTH 0x03
#endif
#endif /* #ifdef FEATURE_UMTS_BMC */

extern boolean cb_nv_duplication_detection_disabled;

extern boolean cb_drx_info_validation_required;
/*===========================================================================

FUNCTION CB_SEND_DATA_EVENT_PACKET

DESCRIPTION
  This function sends the CB page event packet to DIAG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_send_data_event_packet(cb_message_list_type *msg_ptr)
{
  byte event_packet[5];
  cb_missing_page_type *page_ptr = 
                       msg_ptr->message_header.current_message_descriptor->current_page;
  cb_user_page_info_type *msg_desc = msg_ptr->message_header.current_message_descriptor;

  memset((void *)event_packet,0x00,5);

  /* CB Page Data Event */
  /*
       Message ID                               B4: B3 - 16 bits
       Message Geographical Scope      B2(8-7) - 2 bits
       Message Code                           B2(6-1):B1(8-5) - 10 bits
       Update Number                          B1(4-1) - 4 bits
       No. of pages                              B0(8-5) - 4 bits
       Current Page                             B0(4-1) - 4 bits
  */
  event_packet[4] |= (byte)((msg_ptr->message_header.message_id & 0xFF00) >> 8);
  event_packet[3] |= (byte)(msg_ptr->message_header.message_id & 0x00FF);

  event_packet[2] |= (byte)((msg_desc->gs_indicator & 0x03) << 6);
  event_packet[2] |= (byte)((msg_desc->message_code & 0x3F0) >> 4);
  event_packet[1] |= (byte)((msg_desc->message_code & 0x0F) << 4);
  event_packet[1] |= (byte)((msg_desc->update_number & 0x0F));

  event_packet[0] = (byte)((msg_desc->number_of_pages << 4) | (page_ptr->page_number & 0x0F));
  (void) event_report_payload(EVENT_NAS_CB_PAGE_RECEIVED, 5, (void *)event_packet);

}
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
/*===========================================================================

FUNCTION CB_ETWS_SEND_DATA_EVENT_PACKET

DESCRIPTION
  This function sends the CB page event packet to DIAG when ETWS secondary notification is 
  received

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cb_etws_send_data_event_packet(byte *msg_ptr)
{
  byte event_packet[5];
  memset((void *)event_packet,0x00,5);

  /* CB Page Data Event */
  /*
       Message ID                               B4: B3 - 16 bits
       Message Geographical Scope      B2(8-7) - 2 bits
       Message Code                           B2(6-1):B1(8-5) - 10 bits
       Update Number                          B1(4-1) - 4 bits
       No. of pages                              B0(8-5) - 4 bits
       Current Page                             B0(4-1) - 4 bits

       
  */
  event_packet[4] = (byte)msg_ptr[2];/*Most significant 8 bits of Message ID*/
  event_packet[3] = (byte)msg_ptr[3];/*Least significant 8 bits of Message ID*/
  event_packet[2] = (byte)msg_ptr[0];/*Message code+GS*/
  event_packet[1] = (byte)msg_ptr[1];/*Message code+ Update number*/
  event_packet[0] = (byte)msg_ptr[5];/*Number of pages*/
  (void) event_report_payload(EVENT_NAS_CB_PAGE_RECEIVED, 5, (void *)event_packet);
}
#endif
/*===========================================================================

FUNCTION    CB_PARSE_FIRST_PAGING_BLOCK

DESCRIPTION
  This function parses first paging block of the incoming page in order to
  extract the following information :
    o GS indicator
    o Message ID
    o Message Code
    o Update Number
    o Page Number 
    o Total Number of Pages

DEPENDENCIES
  None

RETURN VALUE
  Paging block length

SIDE EFFECTS
  None

===========================================================================*/
void cb_parse_first_paging_block
(
  byte *paging_block,
  byte *gs,
  word *message_id,
  word *message_code,
  byte *update_number,
  byte *page_number,
  byte *number_of_pages,
  byte *data_coding_scheme
)
{
  word message_code_high ;

  /* Extract message GS */
  *gs = (byte)(paging_block[CB_GS_POS] & CB_GS_MASK) >> 6 ;

  MSG_HIGH_DS_1(cb_as_id,"CB: Receied CB message GS %02X",*gs) ;

  /* Extract Message code */
  message_code_high = (paging_block[CB_MESSAGE_CODE_HIGH_POS] & 
                                        (byte)CB_MESSAGE_CODE_HIGH_MASK) << 4 ;
  *message_code = (word)(message_code_high | 
    ((paging_block[CB_MESSAGE_CODE_LOW_POS] & CB_MESSAGE_CODE_LOW_MASK) >> 4));

  MSG_HIGH_DS_1(cb_as_id,"CB: Receied CB Message Code %02X",*message_code) ;

  /* Extract Update Number */
  *update_number = paging_block[CB_UPDATE_NUMBER_POS] & CB_UPDATE_NUMBER_MASK ;

  MSG_HIGH_DS_1(cb_as_id,"CB: Receied CB Message Update Number %02X",*update_number) ;

  /* Extract Message ID */
  *message_id = (paging_block[CB_MESSAGE_ID_OFFSET] << 8) | 
                                         paging_block[CB_MESSAGE_ID_OFFSET+1] ;

  MSG_HIGH_DS_1(cb_as_id,"CB: Receied CB Message ID %02X",*message_id) ;

  /* Extract Page Number */
  *page_number = (byte)((paging_block[CB_PAGE_PARAMETER_POS] & 
                                             (byte)CB_PAGE_NUMBER_MASK) >> 4) ;

  /* Extract Total Number of Pages sent */
  *number_of_pages = paging_block[CB_PAGE_PARAMETER_POS] & 
                                                      CB_NUMBER_OF_PAGES_MASK ;

  if (*page_number == 0) *page_number = 1 ;

  MSG_HIGH_DS_2(cb_as_id,"CB: Receiving %d of %d pages",*page_number,*number_of_pages) ;

  *data_coding_scheme = paging_block[CB_DATA_CODING_SCHEME] ;

  MSG_HIGH_DS_1(cb_as_id,"CB: Received data coding scheme %X",*data_coding_scheme) ;
} /* end cb_parse_first_paging_block() */

/*===========================================================================

FUNCTION    cb_calculate_page_length

DESCRIPTION
  This function calculates paging block length (in octets)

DEPENDENCIES
  None

RETURN VALUE
  Paging block length

SIDE EFFECTS
  None

===========================================================================*/
byte cb_calculate_page_length
(
  byte missing_paging_block_mask
)
{
  byte shift_index = 0 ;
  byte block_counter = 0 ;

  /* If no pages has been received current page has 0 octet length */
  if (missing_paging_block_mask == 0)
  {
    return 0 ;
  }

  /* Count number of received paging blocks */
  while ((missing_paging_block_mask>>(shift_index++))%2)
  {
    block_counter++ ;
  }

  /* Calculate Page length and return it to caller */
  return (byte)(block_counter * CB_PAGING_BLOCK_LENGTH);
} /* end cb_calculate_page_length() */

/*===========================================================================

FUNCTION    CB_PROCESS_RECEIVED_PAGING_BLOCK

DESCRIPTION
  This function processes already validated incoming paging block and enforce 
  skipping of unnecessary paging blocks if needed

DEPENDENCIES
  None

RETURN VALUE
  Next paging block assembler state

SIDE EFFECTS
  None

===========================================================================*/
cb_page_assembling_state_type cb_process_received_paging_block
(
  byte *received_paging_block,
  byte sequence_number,
  cb_missing_page_type *current_page_ptr,
  dword received_fn
)
{
  byte offset = 0 ;
  byte received_block_mask = 0 ;

  switch (sequence_number)
  {
    case CB_FIRST_SCEDULING_PAGING_BLOCK:
#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
    {
      offset = 0 ;
      received_block_mask = CB_RECEIVED_FIRST_BLOCK_MASK ;
    } 
    break ;
#else
    MSG_ERROR_DS_0(cb_as_id,"CB: Scheduling message is not supported") ;

    cb_skip_message_pagingbloks(
                  current_page_ptr,received_fn,CB_SKIP_SECOND_THIRD_FOURTH) ;

    return CB_RECEIVING_FIRST_PB_STATE ;
#endif /* #ifdef FEATURE_GSM_CB_DRX_SUPPORTED */
    case CB_FIRST_PAGING_BLOCK:
      offset = 0 ;
      received_block_mask = CB_RECEIVED_FIRST_BLOCK_MASK ;
      break ;
    case CB_SECOND_PAGING_BLOCK:
      offset = 1 ;
      received_block_mask = CB_RECEIVED_SECOND_BLOCK_MASK ;
      break ;
    case CB_THIRD_PAGING_BLOCK :
      offset = 2 ;
      received_block_mask = CB_RECEIVED_THIRD_BLOCK_MASK ;
      break ;
    case CB_FOURTH_PAGING_BLOCK :
      offset = 3 ;
      received_block_mask = CB_RECEIVED_FOURTH_BLOCK_MASK ;
      break ;
    default:
      MSG_FATAL_DS( cb_as_id,"Illigal sequence number (%d)",sequence_number,0,0) ;
  }

  /* Copy received paging block into page collector */
  memscpy((void *)&current_page_ptr->
          page_buffer[offset*CB_PAGING_BLOCK_DATA_LENGTH],
          sizeof(current_page_ptr->page_buffer)-(offset*CB_PAGING_BLOCK_DATA_LENGTH),
                    (void *)&received_paging_block[CB_PAGING_BLOCK_OFFSET],
                                                 CB_PAGING_BLOCK_DATA_LENGTH) ;

  /* Mark current paging block as received */
  current_page_ptr->rat_page_properties.
                   gsm_propertie.missing_paging_blocks |= received_block_mask ;

  /* Calculate current page length (in octets) */
  current_page_ptr->page_length = 
        cb_calculate_page_length(current_page_ptr->rat_page_properties.
                                         gsm_propertie.missing_paging_blocks) ;

  /* Skip unnecessary paging block reception if needed */
  return cb_skip_unnecessary_paging_blocks(received_fn,
                                        received_block_mask,current_page_ptr) ;

} /* end cb_process_received_paging_block() */

/*===========================================================================

FUNCTION    IS_PAGE_TOBE_DROPED

DESCRIPTION
  This is part of duplication detector

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean is_page_tobe_droped
(
  word page_indicator ,
  byte new_page_number,
  byte number_of_pages,
  byte new_number_of_pages
)
{
  word mask = 1 ;
  byte shift_value = new_page_number - 1 ;

  if (new_number_of_pages != number_of_pages)
  {
    return TRUE ;
  }

  if (new_page_number > number_of_pages)
  {
    return TRUE ;
  }

  if (page_indicator & (mask << shift_value))
  {
    return TRUE ;
  }

  MSG_HIGH_DS_0(cb_as_id,"CB: Received new message page") ;

  return FALSE ;

} /* end is_page_tobe_droped() */

/*===========================================================================

FUNCTION    IS_UPDATE_NUMBER_NOT_CHANGED

DESCRIPTION
  This is part of duplication detector

DEPENDENCIES
  None

RETURN VALUE
  TRUE\FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean is_update_number_not_changed
(
  byte  old_update_number,
  byte  new_update_number
)
{
  /*  
  **Spec 23.041 section 9.4.1.2.1 Serial number  
  **Update number should be same  */
  MSG_HIGH_DS_2(cb_as_id,"Old Update number=%d New Update number=%d",old_update_number, new_update_number);
  return (old_update_number == new_update_number);
}

/*===========================================================================

FUNCTION    CB_DUPLICATION_DETECTOR

DESCRIPTION
  This function implements a CB duplication detector

DEPENDENCIES
  None

RETURN VALUE
  TRUE - current paging block is a duplication of the earlier received one
  FALSE - duplication has not been found

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_duplication_detector
(
  cb_rat_type active_rat,
  cb_message_header_type *existing_message,
  byte new_gs,
  byte new_update_number,
  byte new_page_number,
  byte new_number_of_pages
)
{
  cb_user_page_info_type *current_page_info = 
                                 existing_message->current_message_descriptor ;

  if(cb_nv_duplication_detection_disabled)
  {
    MSG_HIGH_DS_0(cb_as_id,"CB:Duplication detection disabled, dont discard the page") ;
    return FALSE;
  }

  if(is_update_number_not_changed (current_page_info->update_number,
                                  new_update_number))
  {
    if (current_page_info->gs_indicator == new_gs)
    {
      if (active_rat == CB_GSM_MODE)
      {
        return 
          is_page_tobe_droped(
                current_page_info->received_page_indicator, new_page_number, 
                     current_page_info->number_of_pages, new_number_of_pages) ;
      } 
#ifdef FEATURE_UMTS_BMC
      else if (active_rat == CB_UMTS_MODE)
      {
        /* Verify whether the mask of old "no. of pages" and
           received page indicator are same, if both are same then declare
           it duplicate
        */
        word page_mask = cb_bmc_get_mask(current_page_info->number_of_pages) ;
        /* If GS,message id,message code,update number all are same
           then to declare it duplicate message check for page mask
           (earlier received page mask, with mask of newly received 
           number of pages) if both are same or if the number of pages differ 
           (earlier received and newly received) then drop this message
        */

        if (((current_page_info->received_page_indicator & 
             page_mask) == page_mask) ||
             (current_page_info->number_of_pages != new_number_of_pages)
           )
        {         
          return TRUE ;
        }
      }
      else
      {
        MSG_FATAL_DS( cb_as_id,"CB: Illigal actie RAT",0,0,0) ;
      }
#endif

    }
    else
    {
      MSG_HIGH_DS_0(cb_as_id,"GS indicator has been changed") ;
    }
  }
  else
  {
    MSG_HIGH_DS_0(cb_as_id,"Update Number has been changed") ;
  }

  return FALSE ;

} /* end cb_duplication_detector() */

/*===========================================================================

FUNCTION    CB_PROCESS_FIRST_PAGING_BLOCK

DESCRIPTION
  This function processes FIRST paging block of the current CB page by parsing 
  incoming paging block and creating message descriptor (cb_message_list_type)
  if current message has been received for the first time.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to current message descriptor

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e550*/
cb_message_list_type *cb_process_first_paging_block
(
  byte   *paging_block,
  dword   received_fn,
  cb_message_type page_type
)
{
  byte gs ;
  word message_code ;
  byte update_number ;
  word message_id ;
  byte page_number = 0;
  byte number_of_pages;
  byte data_coding_scheme ;
  cb_message_list_type *new_message_ptr      = NULL ;
  cb_message_list_type *current_message_ptr  = NULL ;
  cb_missing_page_type *new_incoming_page = NULL ;

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
  byte scheduling_block_type ;
  byte begin_scheduling_slot, end_scheduling_slot ;
#endif /* FEATURE_GSM_CB_DRX_SUPPORTED */


  /* 
  ** Calculate paging block sequence number based on frame 
  ** number the paging block has been received with
  */
  switch (CB_CALCULATE_PAGING_SLOT(received_fn))
  {
    case CB_FIRST_BLOCK :
    case CB_FIFTH_BLOCK :
      break ;

    default:
      MSG_ERROR_DS_0(cb_as_id,"First paging block sqn. is out of sequence") ;
      return NULL ;
  }

  current_message_ptr = basic_cbch_list;

  if (page_type == CB_SCHEDULING_PAGE)
  {
#ifdef FEATURE_GSM_CB_DRX_SUPPORTED

    cb_prelimenary_scheduling_message_parsing(paging_block+1,
                  &scheduling_block_type,
                                 &begin_scheduling_slot,&end_scheduling_slot) ;

    if ((scheduling_block_type != CB_GSM_SCEDULING_MESSAGE_TYPE) ||
        (begin_scheduling_slot > end_scheduling_slot) ||
        (end_scheduling_slot > CB_MAX_MESSAGES_SCHEDULE_PERIOD))
    {
      MSG_ERROR_DS_0(cb_as_id,"CB DRX: Schedule message is corrupted") ;
      return NULL ;
    }

    if ((current_last_sfn_sch_prd != BMC_INVALID_SFN) && 
        (received_fn != cb_calculate_base_fn(current_last_sfn_sch_prd,CB_PAGING_MAX_BLOCKS-1)) &&
        (cb_bmc_check_sfn_within_sch_prd(received_fn, 
             current_first_sfn_sch_prd, current_last_sfn_sch_prd)))
    {
      MSG_ERROR_DS_0(cb_as_id,"CB DRX: Discarding Unscheduled schedule message in DRX mode") ;
      return NULL ;
    }

    if (!cb_bmc_validate_schedule_message(received_fn, (byte)(begin_scheduling_slot*CB_PAGING_MAX_BLOCKS),
                            (byte)((end_scheduling_slot-begin_scheduling_slot+1)*CB_PAGING_MAX_BLOCKS)))
    {
      MSG_ERROR_DS_0(cb_as_id,"CB DRX: Schedule Message is overlapping with current sch period -> discarding");
      return NULL ;
    }

    if ((new_message_ptr = cb_find_message(
                                        current_message,0xFFFF)) == NULL) 
    {
      if ((new_message_ptr = cb_create_message_entry(current_message,
                                 0xFFFF,received_fn)) == NULL)
      {
        MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;
        return NULL ;
      }
      current_message_ptr = cb_put_message_into_list(current_message_ptr,new_message_ptr) ;
    }
    else
    {
      current_message_ptr = new_message_ptr;
      current_message_ptr->message_header.fn = received_fn ;
    }
#else
    MSG_ERROR_DS_0(cb_as_id,"CB: Scheduling message is not supported") ;

    return NULL ;
#endif
  }
  else
  {

  cb_parse_first_paging_block(paging_block,&gs,
              &message_id,&message_code,&update_number,
                          &page_number, &number_of_pages,&data_coding_scheme) ;

  MSG_HIGH_DS_3(cb_as_id,"CB: Received first paging block with message id = %d message code = %d number_pagers = %d",
           message_id, message_code, number_of_pages);               

  /* Make sure current messaege is in the search list */
  if (!cb_message_is_requested(message_id))
  {
    MSG_HIGH_DS_1(cb_as_id,"Message %02X is not in the search list",message_id); 

    return NULL ;
  }

  /* Call WMS to make sure current paging block has to be processed */
  if (!wms_bc_gw_cb_filter
     (
    #ifdef FEATURE_NAS_CBS_DSDS
         (sys_modem_as_id_e_type)cb_as_id,
    #else
         (sys_modem_as_id_e_type)CB_AS_ID_1, 
    #endif
         paging_block+1
        )
       )
  {
    MSG_HIGH_DS_0(cb_as_id,"CB: WMS filters out current CB page") ;

    return NULL ;
  }

  /* Search for the current message ID in the Message Descriptor list */
  if ((new_message_ptr = cb_find_message(
                                      current_message_ptr,message_id)) == NULL) 
  {
    MSG_HIGH_DS_1(cb_as_id,"CB: Mesage ID %d not found. Creating cache", message_id);
    /* This is a first message ID reception, create new message descriptor */
    if ((new_message_ptr = cb_create_message_entry(current_message,
                                              message_id,received_fn)) == NULL)
    {
      MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;

      return NULL ;
    }

    /* This is a first message CODE reception, create new message descriptor */
    if ((new_message_ptr->message_header.current_message_descriptor = 
                  cb_create_message_code_entry(new_message_ptr,
                       message_code,gs,update_number,number_of_pages, data_coding_scheme)) == NULL)
    {
      MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;

      return NULL ;
    }

    /* MSG_HIGH_DS_0(cb_as_id,"CB: Received Base FN is %d",received_fn) ; */

    /* current_message_ptr = new_message_ptr ; */
    current_message_ptr = cb_put_message_into_list(current_message_ptr,new_message_ptr) ;
  }
  else
  {
    current_message_ptr = new_message_ptr ;

    /* Search for the current message CODE in the Message Descriptor list */
    if (cb_find_message_code_with_gs(new_message_ptr,message_code) == NULL)
    {
      /* 
      ** This is a first message CODE reception,create new message descriptor
      */
      if ((new_message_ptr->message_header.current_message_descriptor = 
                  cb_create_message_code_entry(new_message_ptr,
                       message_code,gs,update_number,number_of_pages, data_coding_scheme)) == NULL)
      {
        MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;

        return NULL ;
      }

      new_message_ptr->message_header.fn = received_fn ;

      /* MSG_HIGH_DS_0(cb_as_id,"CB: Received Base FN is %d",received_fn) ; */
    }
    else
    {
      /* Run a duplication detector */
      if (cb_duplication_detector(CB_GSM_MODE,
                      &current_message_ptr->message_header,
                                 gs,update_number,page_number,number_of_pages))
      {
        MSG_HIGH_DS_2(cb_as_id,"CB: Duplication is detected, Msg. ID %02X, Code %02X",
             current_message_ptr->message_header.message_id,message_code) ;

        if (gs == 0x00)
        {
          cb_send_wms_duplication_ind(message_id,message_code,gs,
                update_number,page_number,number_of_pages,data_coding_scheme) ;
        }

        return NULL ;
      }
      else
      {
        /* Update Message/Code descriptor entry */
        cb_user_page_info_type *current_page_info = 
               current_message_ptr->message_header.current_message_descriptor ;

        /* Refresh base FN */
        current_message_ptr->message_header.fn = received_fn ;

        if ((cb_nv_duplication_detection_disabled) || 
            (current_page_info->gs_indicator != gs) || 
            (current_page_info->update_number != update_number))
        {
          current_page_info->gs_indicator = gs ;

          current_page_info->update_number = update_number ;

          current_page_info->number_of_pages = number_of_pages ;

          current_page_info->received_page_indicator = 0 ;

          if (current_page_info->missing_page_list != NULL)
          {
            do
            {
              current_page_info->missing_page_list= 
                    cb_delete_page(current_message_ptr,
                                        current_page_info->missing_page_list) ;
            }
            while (current_page_info->missing_page_list != NULL) ;
          }
        }
      }
    }
  }
  }

  switch (page_type)
  {
    case CB_NORMAL_PAGE :
      if ((new_incoming_page = 
            cb_find_existing_page_buffer(current_message_ptr->message_header.
                current_message_descriptor->missing_page_list,
                                                         page_number)) == NULL)
      {
          if ((new_incoming_page = cb_create_page_descriptor(CB_GSM_MODE,
            current_message_ptr,
               current_message_ptr->message_header.
                    current_message_descriptor->
                                       missing_page_list,page_number)) == NULL)
          {
            MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;

            return NULL ;
          }
      }
      break ;
    case CB_SCHEDULING_PAGE :
#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
    {
      if (cb_find_message_code(current_message_ptr,0x0) == NULL)
      {
        if ((current_message_ptr->message_header.current_message_descriptor = 
                    cb_create_message_code_entry(current_message_ptr,
                         0,0,0,1, 0)) == NULL)
        {
          MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;

          return NULL ;
        }
      }
      if ((new_incoming_page = 
            cb_find_existing_page_buffer(current_message_ptr->message_header.
                current_message_descriptor->missing_page_list,
                                                         0)) == NULL)
      {
        if ((new_incoming_page = cb_create_page_descriptor(CB_GSM_MODE, current_message_ptr,
                current_message_ptr->message_header.current_message_descriptor->
                                                   missing_page_list,0)) == NULL)
        {
          MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;

          return NULL ;
        }
      }
    }
#else /* FEATURE_GSM_CB_DRX_SUPPORTED */
      MSG_ERROR_DS_0(cb_as_id,"CB: Scheduling message is not supported") ;

      return NULL ;
#endif /* FEATURE_GSM_CB_DRX_SUPPORTED */

    break ;
    default:
      MSG_FATAL_DS( cb_as_id,"Invalid page type",0,0,0) ;
  }
  /*lint -e774*/
  if ((new_incoming_page != NULL) && (current_message_ptr != NULL))
  {
    current_message_ptr->message_header.
                 current_message_descriptor->current_page = new_incoming_page ;

    /* Reset skip history for the current page */
    new_incoming_page->
                  rat_page_properties.gsm_propertie.already_skiped_blocks = 0 ;
  }
  else
  {
    MSG_FATAL_DS( cb_as_id,"Unrecoverable error",0,0,0) ;
  }
  /*lint +e774*/

  if (current_message_ptr == NULL)
  {
    basic_cbch_list_entry_point = NULL ;
  }
  else if (current_message_ptr->prev_message == NULL)
  {
    basic_cbch_list_entry_point = current_message_ptr ;
  }

  /*lint +e774*/
  basic_cbch_list = current_message_ptr ;

  MSG_HIGH_DS_0(cb_as_id,"CB: Finished processing first paging block") ;

  return current_message_ptr ;
} /* end of cb_process_first_paging_block() */
/*lint +e550*/
/*===========================================================================

FUNCTION    CB_VALIDATE_PAGING_BLOCK

DESCRIPTION
  This function performs preliminary paging block validation based on PD and 
  calculated and given paging block sequence number

DEPENDENCIES
  None

RETURN VALUE
  TRUE - paging block is valid
  TRUE - paging block is out of sequence or does not belong to CB feature

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_validate_paging_block
(
  byte   *paging_block,
  dword   received_fn
)
{
  byte sequence_number ;
  boolean out_of_sequence_indicator = TRUE;

  if (((paging_block[CB_PAGING_BLOCK_TYPE] & CB_LPD_MASK) >> 5) != LINK_PD)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Invalid LPD") ;

    return FALSE ;
  }

  sequence_number = 
    paging_block[CB_PAGING_BLOCK_TYPE] & CB_PAGING_BLOCK_SEQUENCE_NUMBER_MASK ;

  if (sequence_number == CB_NULL_MESSAGE_PAGING_BLOCK)
  {
    MSG_HIGH_DS_0(cb_as_id,"CB: Null block -> can skip the rest of blocks") ;

    return FALSE ;
  }

  switch (CB_CALCULATE_PAGING_SLOT(received_fn)) 
  {
    case CB_FIRST_BLOCK :
    case CB_FIFTH_BLOCK :
      out_of_sequence_indicator = 
            ((sequence_number != CB_FIRST_PAGING_BLOCK) && 
                        (sequence_number != CB_FIRST_SCEDULING_PAGING_BLOCK)) ;
      break ;
    case CB_SECOND_BLOCK :
    case CB_SIXTH_BLOCK :
      out_of_sequence_indicator = (sequence_number != CB_SECOND_PAGING_BLOCK) ;
      break ;

    case CB_THIRD_BLOCK :
    case CB_SEVENTH_BLOCK :
      out_of_sequence_indicator = (sequence_number != CB_THIRD_PAGING_BLOCK) ;
      break ;

    case CB_FOURTH_BLOCK :
    case CB_EIGHTH_BLOCK :
      out_of_sequence_indicator = (sequence_number != CB_FOURTH_PAGING_BLOCK) ;
      break ;
    default:
      break;
  }

  if (out_of_sequence_indicator)
  {
    MSG_HIGH_DS_1(cb_as_id,"CB: Received block is out of sequence (%d)",sequence_number) ;

    /*Reset the page assembling state*/
    page_assembling_state = CB_RECEIVING_FIRST_PB_STATE;

    return FALSE ;
  }  

  return TRUE ;
} /* end cb_validate_baging_block() */

/*===========================================================================

FUNCTION    CB_FINISH_PAGE_PROCESSING

DESCRIPTION
  This function completes paging (either regular or scheduling) block by sending
  data indication to WMS or enforcing CB DRX mode if scheduling message has been
  received

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_finish_page_processing
(
  cb_message_list_type *message_ptr
)
{
  cb_missing_page_type *current_page_ptr = 
         message_ptr->message_header.current_message_descriptor->current_page ;
  dword first_block_fn = message_ptr->message_header.fn ;
  byte missing_blocks = 
    current_page_ptr->rat_page_properties.gsm_propertie.missing_paging_blocks ;

  switch (message_type)
  {
    case CB_NORMAL_PAGE :
      {
        byte page_number = current_page_ptr->page_number;

        message_ptr->message_header.
                  current_message_descriptor->received_page_indicator |= 
                                               (word)((word)1 << (page_number - 1)) ;

        if (missing_blocks != (int)CB_MISSING_NOTHING)
        {
          byte skip_blocks = (0x0F&(~missing_blocks)) ;

          cb_skip_message_pagingbloks(
                                    current_page_ptr,first_block_fn,skip_blocks) ;
        }
        cb_send_ota_msg_log_packet((uint8)CB_GSM_MODE,
                    (uint32)current_page_ptr->page_length,(uint8)CB_NORMAL_PAGE,
                                                  current_page_ptr->page_buffer);
        cb_send_wms_data_ind(current_page_ptr->page_length,
                                                     current_page_ptr->page_buffer) ;

        cb_send_data_event_packet(message_ptr);

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
        cb_bmc_skip_next_reception(message_ptr
#ifdef FEATURE_NAS_REL6
        ,BMC_INVALID_SERIAL_NUMBER
#endif
        );
#endif

        message_ptr->
                message_header.current_message_descriptor->current_page = 
                                       cb_delete_page(message_ptr,current_page_ptr) ;

        if(cb_nv_duplication_detection_disabled)
        {
           if(message_ptr->message_header.current_message_descriptor->missing_page_list==NULL) 
           {
              cb_user_page_info_type *cb_received_message_code = NULL ;
              cb_user_page_info_type *cb_prev_message_code = NULL ;
              cb_prev_message_code = message_ptr->message_header.message_info ;
              cb_received_message_code=message_ptr->message_header.current_message_descriptor;
              while(cb_prev_message_code!=NULL)
               {
                 if(cb_prev_message_code->next_message_code==cb_received_message_code)
                 {
                    break;
                 }
                 cb_prev_message_code=cb_prev_message_code->next_message_code;
               }
              current_message = cb_delete_message_code(message_ptr,
                               cb_prev_message_code,cb_received_message_code) ;
              if(current_message==NULL)
               {
                  basic_cbch_list_entry_point=NULL;
               }
              else if(current_message->prev_message==NULL)
               {
                  basic_cbch_list_entry_point=current_message;
               }
              basic_cbch_list=current_message;
           }
        }
        else
        {
          /* Reset Dup_detection_timer if new page in existing message receieved and timer value is set with this message's timer_offset*/
          if(message_ptr->message_header.current_message_descriptor->timer_offset == 0)
          {
            dword x = gs_enquire_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(CB_TIMER_DUP_DET));
            message_ptr->message_header.current_message_descriptor->timer_offset = cb_dup_timer_value - x;
            if(cb_next_min_offset == CB_DUP_TIMER_VALUE_INVALID)
            {
              cb_next_min_offset = message_ptr->message_header.current_message_descriptor->timer_offset;
            }
            cb_update_cache_timer_offsets(x);
          }
          /* Starting the dup_detection_timer for the first mesaage */
          else if(cb_timer_status[CB_TIMER_DUP_DET-1] == TIMER_STOPPED)
          {
            cb_dup_start_timer( CB_TIMER_DUP_DET, cb_dup_timer_value);
            message_ptr->message_header.current_message_descriptor->timer_offset = 0;
          }
          /* If a new page is received in new message then initializing its timer_offset
                   or if a new page is received in an existing message code and current timer value is not set for this message then resetting its timer_offset*/
          else
          {
            message_ptr->message_header.current_message_descriptor->timer_offset = cb_dup_timer_value - gs_enquire_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(CB_TIMER_DUP_DET));
            if(cb_next_min_offset == CB_DUP_TIMER_VALUE_INVALID)
            {
              cb_next_min_offset = message_ptr->message_header.current_message_descriptor->timer_offset;
            }
          }
        }
      }
      break ;
    case CB_SCHEDULING_PAGE :
#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
    {
      if (missing_blocks != (int)CB_MISSING_NOTHING)
      {
        byte skip_blocks = (0x0F&(~missing_blocks)) ;
        cb_skip_message_pagingbloks(
                                  current_page_ptr,first_block_fn,skip_blocks) ;
      }
      cb_send_ota_msg_log_packet((uint8)CB_GSM_MODE,
                    (uint32)current_page_ptr->page_length,(uint8)CB_SCHEDULING_PAGE,
                                                  current_page_ptr->page_buffer);
      cb_process_scheduling_message(message_ptr,
            message_ptr->message_header.fn,
            (CB_CALCULATE_PAGING_SLOT(message_ptr->message_header.fn)/CB_PAGING_MAX_BLOCKS==0) ? CBCH : ECBCH);

      current_message = cb_delete_message_code(message_ptr,
                                               NULL,
                                               message_ptr->message_header.current_message_descriptor) ;
      basic_cbch_list = current_message ;
      if (current_message == NULL)
      {
        basic_cbch_list_entry_point = NULL ;
      }
      else if (current_message->prev_message == NULL)
      {
        basic_cbch_list_entry_point = current_message ;
      }
    }
#else
      MSG_ERROR_DS_0(cb_as_id,"CB: Scheduling message is not supported") ;
#endif /* FEATURE_GSM_CB_DRX_SUPPORTED */
      break;
    default:
      MSG_FATAL_DS( cb_as_id,"Illegal message type id",0,0,0) ;
  }

} /* end cb_finish_page_processing() */

/*===========================================================================

FUNCTION    CB_MESSAGE_IS_READY

DESCRIPTION
  This function analyzes received paging block masks taking into account Last
  Paging Block indicator in order to make sure that entire Paging Block which
  has CB relevant information has already been received.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - entire Paging Block has already been received
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_message_is_ready
(
  cb_message_list_type *message,
  boolean last_paging_block_ind
)
{
  boolean return_value = FALSE;
  word missing_blocks = message->message_header.
            current_message_descriptor->current_page->
                      rat_page_properties.gsm_propertie.missing_paging_blocks ;

  if (last_paging_block_ind)
  {
    switch (missing_blocks)
    {
      case CB_MISSING_FOURTH_THIRD_SECOND:
      case CB_MISSING_FOURTH_THIRD:
      case CB_MISSING_FOURTH:
      case CB_MISSING_NOTHING:
        return_value = TRUE ;
        break;
      default:
        return_value = FALSE ;
        break;
    }
  }
  else
  {
    if (missing_blocks == (int)CB_MISSING_NOTHING)
    {
      return_value = TRUE ;
    }
    else
    {
      return_value = FALSE ;
    }
  }
  /*lint -e527*/
  return return_value ;
  /*lint +e527*/
} /* end cb_mesage_is_ready() */

/*===========================================================================

FUNCTION    CB_PROCESS_PAGING_BLOCK

DESCRIPTION
  This function processes received paging block

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_process_paging_block
(
  byte  *paging_block,
  dword  received_fn
)
{
  byte sequence_number ;
  cb_message_list_type *new_message_ptr ;
  dword fn;

  /* Get Last Paging Block indicator */
  boolean last_paging_block_ind = ((paging_block[CB_PAGING_BLOCK_TYPE] & LAST_PAGING_BLOCK_MASK) == 
                                                      LAST_PAGING_BLOCK_MASK) ;

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
  cb_current_sfn = received_fn;
  /*In case if it is first SFN recd in the next DRX cycle then clean the old DRX tables*/
  if ((next_first_sfn_sch_prd != BMC_INVALID_SFN) && 
       cb_bmc_check_sfn_within_sch_prd(received_fn, next_first_sfn_sch_prd, next_last_sfn_sch_prd))
  {
    cb_bmc_start_new_drx_cycle();
  }
#endif

  /* Get Paging Block Sequence number */
  sequence_number = paging_block[CB_PAGING_BLOCK_TYPE] &
                                         CB_PAGING_BLOCK_SEQUENCE_NUMBER_MASK ;

  if ((sequence_number == CB_FIRST_PAGING_BLOCK) || 
                          (sequence_number == CB_FIRST_SCEDULING_PAGING_BLOCK))
  {
    /* If either FIRST or FIRST SCHEDULING block has been received reset
    ** CB Page assembler
    */
    page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;
  }

  switch (page_assembling_state)
  {
    case CB_RECEIVING_FIRST_PB_STATE :
      {
        cb_message_type page_type = CB_NORMAL_PAGE ;

        switch (sequence_number)
        {
          case CB_FIRST_PAGING_BLOCK:
            page_type = CB_NORMAL_PAGE ;
            message_type = (byte)CB_NORMAL_PAGE;
            break ;
          case CB_FIRST_SCEDULING_PAGING_BLOCK:
#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
            page_type = CB_SCHEDULING_PAGE ;
            message_type = (byte)CB_SCHEDULING_PAGE;
            break;
#else
            MSG_ERROR_DS_0(cb_as_id,"CB: Scheduling message is not supported") ;

            return ;
#endif /* FEATURE_GSM_CB_DRX_SUPPORTED */
          default:

            if (!cb_skiped_fn(received_fn))
            {
              MSG_ERROR_DS_1(cb_as_id,"CB: Received block num. %d when expected first",
                                                       sequence_number+1) ;

              cb_skip_rest_of_page(cb_calculate_base_fn(
                                received_fn,sequence_number),sequence_number) ;
            }

            return ;
        } /* switch (sequence_number) */

 #ifdef FEATURE_GSM_CB_DRX_SUPPORTED
        /*Update the new msg bit map list in case the message is received*/
        cb_bmc_update_new_msg_bitmap_info(received_fn, TRUE, TRUE);
#endif

        new_message_ptr = cb_process_first_paging_block(
                                          paging_block,received_fn,page_type) ;

        if (new_message_ptr == NULL)
        {
          /* Received paging block is corrupted, skip the rest of the page */
          cb_skip_number_of_pagingbloks(received_fn,
                                                 CB_SKIP_SECOND_THIRD_FOURTH) ;

          return ;
        }
        else
        {
          current_message = new_message_ptr ;
        }
      }
      break ;
    case CB_RECEIVING_SECOND_PB_STATE :
      if (sequence_number != CB_SECOND_PAGING_BLOCK)
      {
        if (!cb_skiped_fn(received_fn))
        {
          MSG_ERROR_DS_1(cb_as_id,"CB: Received block num. %d when expected second",
                                                       sequence_number+1) ;

          cb_skip_rest_of_page(cb_calculate_base_fn(
                                received_fn,sequence_number),sequence_number) ;
          page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;
        }

        return ;
      }

      break ;
    case CB_RECEIVING_THIRD_PB_STATE :
      if (sequence_number != CB_THIRD_PAGING_BLOCK)
      {
        if (!cb_skiped_fn(received_fn))
        {
          MSG_ERROR_DS_1(cb_as_id,"CB: Received block num. %d when expected third",sequence_number+1) ;

          cb_skip_rest_of_page(cb_calculate_base_fn(
                                received_fn,sequence_number),sequence_number) ;
          page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;
        }

        return ;
      }
      break ;
    case CB_RECEIVING_FOURTH_PB_STATE :
      if (sequence_number != CB_FOURTH_PAGING_BLOCK)
      {
        if (!cb_skiped_fn(received_fn))
        {
          MSG_ERROR_DS_1(cb_as_id,"CB: Received block num. %d when expected fourth",sequence_number+1) ;

          cb_skip_rest_of_page(cb_calculate_base_fn(
                                received_fn,sequence_number),sequence_number) ;
          page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;
        }

        return ;
      }
      break ;
    default:
      MSG_FATAL_DS( cb_as_id,"Illigal message assembling state ID",0,0,0) ;
  }

  if (current_message != NULL)
  {
     if (page_assembling_state == CB_RECEIVING_FIRST_PB_STATE)
     {
       /*Set the constructing message descriptor to current message descriptor*/
       current_message->message_header.cb_curr_proces_msg_des = 
           current_message->message_header.current_message_descriptor;
     }
     else
     {

       if (current_message->message_header.fn != 
                     cb_calculate_base_fn(received_fn,sequence_number))
       {
	     fn = cb_calculate_base_fn(received_fn,sequence_number);
         MSG_ERROR_DS_2(cb_as_id,"CB: Received message starting at FN %d when expected %d",
           fn,current_message->message_header.fn);
         if (!cb_skiped_fn(received_fn))
         {
           cb_skip_rest_of_page(cb_calculate_base_fn(
                                 received_fn,sequence_number),sequence_number) ;
         }

         /*Reset the page assembling state*/
         page_assembling_state = CB_RECEIVING_FIRST_PB_STATE;

         return;
       }

       current_message->message_header.current_message_descriptor = 
         current_message->message_header.cb_curr_proces_msg_des;

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
        /*Update the new msg bit map list in case the message is received*/
        cb_bmc_update_new_msg_bitmap_info(received_fn, FALSE, TRUE);
#endif

     }

    /*In case the current page ptr is corrupted, the message descriptor has to be deleted from the cache 
      and page_assembling_state should be set to CB_RECEIVING_FIRST_PB_STATE*/
    if (current_message->message_header.current_message_descriptor->current_page == NULL)        
    {

      MSG_ERROR_DS_0(cb_as_id,"CB: current page ptr is NULL,\
           reset the page_assembling state to CB_RECEIVING_FIRST_PB_STATE");

      /*Find the previous message code*/
      if (current_message->message_header.current_message_descriptor->missing_page_list == NULL)
      {
        cb_user_page_info_type *cb_prev_message_code, *cb_curr_message_code;

        cb_prev_message_code = NULL;
        cb_curr_message_code = current_message->message_header.message_info;
        while (cb_curr_message_code != NULL )
        {
          if (cb_curr_message_code->message_code == 
              current_message->message_header.current_message_descriptor->message_code)
          {
            break;
          }
          cb_prev_message_code = cb_curr_message_code;
          cb_curr_message_code = cb_curr_message_code->next_message_code;

        }
        
        if (cb_curr_message_code == NULL)
        {
          MSG_ERROR_DS_0(cb_as_id,"CB: Could not remove message entry from the list") ;
        }
        else
        {
          /*Delete the message code from the list*/
          current_message = cb_delete_message_code(current_message, cb_prev_message_code, cb_curr_message_code);
        }
      }
      
      /*Reset the page assembling state*/
      page_assembling_state = CB_RECEIVING_FIRST_PB_STATE;
    }
    else{
      page_assembling_state = 
        cb_process_received_paging_block(paging_block,sequence_number,
               current_message->message_header.
                        current_message_descriptor->current_page,received_fn) ;
    }

    /* Make sure that entire page has been received */
    if(current_message!= NULL)
    {
      if (cb_message_is_ready(current_message,last_paging_block_ind))
      {
        cb_finish_page_processing(current_message) ;

        page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;
      }
    }
    else
    {
      MSG_FATAL_DS( cb_as_id,"Invalid current Message",0,0,0);
    }
  }

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
  /*In case if it is last FN in the DRX cycle then clean the old DRX tables*/
  if ((current_last_sfn_sch_prd != BMC_INVALID_SFN) && (received_fn == current_last_sfn_sch_prd))
  {
    cb_bmc_end_current_drx_cycle();
  }
#endif /*FEATURE_UMTS_BMC_DRX_SUPPORTED*/

} /* end cb_process_paging_block() */

/*===========================================================================

FUNCTION    CB_PROCESS_PAGING_BLOCK

DESCRIPTION
  This function enforces the state exchange of the message assembling state
  machine according to the missing FN reported by L1

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_asm_state_handler
(
  dword missing_fn
)
{
  switch (CB_CALCULATE_PAGING_SLOT(missing_fn))
  {
    case CB_FIRST_BLOCK:
    case CB_FIFTH_BLOCK:

      MSG_HIGH_DS_0(cb_as_id,"CB: First paging block is missing") ;

      MSG_HIGH_DS_0(cb_as_id,"CB: Next state is CB_RECEIVING_FIRST_PB_STATE") ;

      cb_skip_number_of_pagingbloks(missing_fn,CB_SKIP_SECOND_THIRD_FOURTH) ;

      page_assembling_state =  CB_RECEIVING_FIRST_PB_STATE ;

      break ;
    case CB_SECOND_BLOCK:
    case CB_SIXTH_BLOCK:

      if (page_assembling_state == CB_RECEIVING_SECOND_PB_STATE)
      {
        MSG_HIGH_DS_0(cb_as_id,"CB: Next state is CB_RECEIVING_THIRD_PB_STATE") ;

        page_assembling_state = CB_RECEIVING_THIRD_PB_STATE ;
      }

      break ;
    case CB_THIRD_BLOCK:
    case CB_SEVENTH_BLOCK:

      if (page_assembling_state == CB_RECEIVING_THIRD_PB_STATE)
      {
        MSG_HIGH_DS_0(cb_as_id,"CB: Next state is CB_RECEIVING_FOURTH_PB_STATE") ;

        page_assembling_state = CB_RECEIVING_FOURTH_PB_STATE ;
      }

      break ;
    case CB_FOURTH_BLOCK:
    case CB_EIGHTH_BLOCK:

      if (page_assembling_state == CB_RECEIVING_FOURTH_PB_STATE)
      {
        MSG_HIGH_DS_0(cb_as_id,"CB: Next state is CB_RECEIVING_FIRST_PB_STATE") ;

        page_assembling_state = CB_RECEIVING_FIRST_PB_STATE ;
      }

      break ;
    default:
      MSG_ERROR_DS_1(cb_as_id,"CB: Out of sequence missing block indication (%d)",missing_fn) ;
      break ;
  }

} /* end of cb_asm_state_handler() */

#ifdef FEATURE_UMTS_BMC

/*===========================================================================

FUNCTION    CB_BMC_FORGE_PAGE_HEADER

DESCRIPTION
  This function encodes GSM page header

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_forge_page_header
(
  /* Pointer to a GSM page header */
  byte *header_ptr,

  /* Page properties */
  word  message_id,
  word  message_code,
  byte  gs_indicator,
  byte  update_number,
  byte  data_coding_scheme,
  byte  page_number,
  byte  number_of_pages
)
{ 
  memset((void *)header_ptr,0x00,6) ;
  /*lint -e734*/
  header_ptr[0] |= gs_indicator << 6;

  header_ptr[0] |= ((message_code >> 4) & CB_MESSAGE_CODE_HIGH_MASK);

  header_ptr[1] |= ((message_code << 4) & CB_MESSAGE_CODE_LOW_MASK);

  header_ptr[1] |= update_number ;

  header_ptr[2] = (byte)((message_id & 0xFF00) >> 8) ;

  header_ptr[3] = (byte)(message_id & 0x00FF) ;

  header_ptr[4] = data_coding_scheme ;

  header_ptr[5] = ((page_number << 4) | number_of_pages) ;
  /*lint +e734*/
} /* end of cb_bmc_forge_page_header() */

/*===========================================================================

FUNCTION    CB_BMC_GET_SFN_ALLOCATION_INDEX

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  The function returns SFN Allocation Index if wanted SFN present in the table
  or 0xFFFF otherwise

SIDE EFFECTS
  None

===========================================================================*/

word cb_bmc_get_sfn_allocation_index
(
  /* Current message type */
  byte msg_type ,

  /* SFN Allocation table length */
  word msg_sfn_allocation_length,

  /* SFN Allocation table pointer */
  cb_bmc_pdu_allocation_type *msg_sfn_allocation,

  /* Wanted SFN */
  dword received_sfn
)
{
  word sfn_search_index ;
  boolean sfn_found = FALSE ;

  for (sfn_search_index=0; 
                  sfn_search_index<msg_sfn_allocation_length; sfn_search_index++)
  {
    if (msg_type == CBS_MESSAGE_TYPE)
    {
      /* Check the SFN */
      if ((msg_sfn_allocation[sfn_search_index].sfn == received_sfn) && 
           (msg_sfn_allocation[sfn_search_index].sfn_page_descriptor[0].page_number != 0))
      {
        /* SFN found */
        sfn_found = TRUE ;
  
        break ;
      }
    }
    else
    {
      /* Check the SFN */
      if (msg_sfn_allocation[sfn_search_index].sfn == received_sfn)
      {
        /* SFN found */
        sfn_found = TRUE ;
  
        break ;
      }
    }
  }

  /* Return search result */
  if (sfn_found)
  {
    return sfn_search_index ;
  }
  else
  {
    return 0xFFFF ;
  }
} /* end of cb_bmc_get_sfn_allocation_index() */

/*===========================================================================

FUNCTION    CB_BMC_PROCESS_INCOMING_DATA

DESCRIPTION
  This function processes incoming RLC PDU and sends CB_DATA_IND to WMS as 
  soon as entire page has been constructed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e715*/
void cb_bmc_process_incoming_data
(
  /* SFN Allocation table length */
  word msg_sfn_allocation_length,

  /* SFN Allocation table pointer */
  cb_bmc_pdu_allocation_type *msg_sfn_allocation,

  /* Incoming SFN */
  dword received_sfn,

  /* PDU Distrribution table pointer */
  byte *pdu_distribution_table,

  /* Iccoming RLC PDU length */
  word data_length,

  /* Incoming RLC PDU pointer */
  byte *data_ptr,

  /* Message is to be constructed */
  cb_message_list_type **current_message_ptr
)
{
  /* 
  ** Contains incoming page fragment
  ** number and boolean Length Indicator 
  ** if any indicator is expected during 
  ** processing of current page fragment
  */
  byte page_properties ;

  /* Length of page fragment */
  byte element_data_length ;

  /* SFN Allocation Table index */
  word sfn_search_index = 0 ;

  /* Current page number */
  byte page_number = 0;

  /* Page fragment index */
  byte page_index = 0 ;

#ifdef FEATURE_NAS_REL6
  dword data_sn = BMC_INVALID_SERIAL_NUMBER;
#endif
#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  /* 
  ** PDU offset that the incoming 
  ** page fragment is to be copied 
  ** from
  */
  word pdu_offset = 0 ;

#else
  /* 
  ** PDU offset that the incoming 
  ** page fragment is to be copied 
  ** from
  */
  byte pdu_offset = 0 ;

#endif
  /*
  ** Page offset that current page 
  ** fragment is to be copied into
  */
  byte page_data_offset = 0 ;

  /* Pointer into list of misiing pages */
  cb_missing_page_type *new_incoming_page = NULL ;

#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
  byte *sec_page_data_buffer;
  word serial_number;
#ifndef FEATURE_MODEM_HEAP
  if ((sec_page_data_buffer = (byte *)gs_alloc(CB_ETWS_MAX_SEC_NOFICATION_LEN+CB_BMC_HEADER_LENGTH))== NULL)
#else
  if(((sec_page_data_buffer = (byte *) modem_mem_calloc(CB_ETWS_MAX_SEC_NOFICATION_LEN+CB_BMC_HEADER_LENGTH,sizeof(byte),MODEM_MEM_CLIENT_NAS)) == NULL))
#endif
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to allocate sec_page_data_buffer") ;
    return ;
  }
#endif

  /* Search for correct SFN Allocation table index */
  if ((sfn_search_index = cb_bmc_get_sfn_allocation_index(
              CBS_MESSAGE_TYPE,msg_sfn_allocation_length,
                                   msg_sfn_allocation,received_sfn)) != 0xFFFF)
  {
    /* Index found */

#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
    if(((*current_message_ptr)->message_header.message_id >= CB_BMC_ETWS_EARTQUAKE_ID) &&
       ((*current_message_ptr)->message_header.message_id <= CB_BMC_ETWS_FUTURE_EXTENSION_03))
    {
      word current_message_id = 
        (*current_message_ptr)->message_header.message_id ;
      word current_message_code = 
        (*current_message_ptr)->message_header.current_message_descriptor->message_code ;
      byte current_gs_indicator = 
        (*current_message_ptr)->message_header.current_message_descriptor->gs_indicator ;
      byte current_update_number = 
        (*current_message_ptr)->message_header.current_message_descriptor->update_number ;
      byte current_data_coding_scheme = 
       (*current_message_ptr)->message_header.current_message_descriptor->data_coding_scheme  ;
      byte current_number_of_pages = 
        (*current_message_ptr)->message_header.current_message_descriptor->number_of_pages ;

      cb_bmc_forge_page_header(sec_page_data_buffer,
                               current_message_id,
                               current_message_code,
                               current_gs_indicator,
                               current_update_number,
                               current_data_coding_scheme,
                               page_number,
                               current_number_of_pages) ;
      MSG_HIGH_DS_1(cb_as_id,"CB ETWS: Received ETWS Seconadry Notification with message id %d",(*current_message_ptr)->message_header.message_id);
      for(page_index = 0;
            ((page_index<msg_sfn_allocation[sfn_search_index].number_of_pages) && ((pdu_offset + CB_BMC_PAGE_LENGTH)<= CB_ETWS_MAX_SEC_NOFICATION_LEN));
                                                                      page_index++)
      {
        
        /* Get page fragment's page number */
        page_number = msg_sfn_allocation[sfn_search_index].
                                        sfn_page_descriptor[page_index].page_number ;
        /* Update reseived page's table */
        (*current_message_ptr)->message_header.
            current_message_descriptor->received_page_indicator |=
                                        (word)((word)1 << (page_number-1)) ;

        /*Copy the CB DATA from the data_ptr*/
        memscpy(&sec_page_data_buffer[pdu_offset+CB_BMC_HEADER_LENGTH], CB_ETWS_MAX_SEC_NOFICATION_LEN-pdu_offset,data_ptr+pdu_offset+CB_BMC_HEADER_LENGTH,CB_BMC_PAGE_LENGTH);
        pdu_offset += CB_BMC_PAGE_LENGTH;


#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
        /*Skip the other slots scheduled for same message id */
        cb_bmc_skip_next_reception((*current_message_ptr)
#ifdef FEATURE_NAS_REL6
        ,BMC_INVALID_SERIAL_NUMBER
#endif 
        );
#endif
      }
      serial_number = sec_page_data_buffer[1];
      serial_number |= (0xff00)&(sec_page_data_buffer[0]<<8);
      cb_send_wms_etws_sec_notification_ind( current_message_id, serial_number,
                                             current_data_coding_scheme,
                                             current_number_of_pages,
                                             &sec_page_data_buffer[CB_BMC_HEADER_LENGTH]);
      cb_send_ota_msg_log_packet((uint8)CB_UMTS_MODE,
                    CB_BMC_PAGE_LENGTH,(uint8)CB_NORMAL_PAGE,   
                                                  &sec_page_data_buffer[CB_BMC_HEADER_LENGTH]);
      /* Send event to Diag */
      cb_etws_send_data_event_packet(&sec_page_data_buffer[0]);
       
#if defined FEATURE_UMTS_BMC_DRX_SUPPORTED
      cb_active_substate_control(CB_STD_DATA_PAGE_RECD);
#endif
    }
    else
#endif
    {
    /* Process incoming page fragments */
    for (page_index=0; 
            page_index<msg_sfn_allocation[sfn_search_index].number_of_pages; 
                                                                  page_index++)
    {
      /* Get page fragment's page number */
      page_number = msg_sfn_allocation[sfn_search_index].
                                  sfn_page_descriptor[page_index].page_number ;

      /* Get page fragment's PDU offset */
      pdu_offset = msg_sfn_allocation[sfn_search_index].
                              sfn_page_descriptor[page_index].pdu_data_offset ;

      /* Get page fragment's page offset */
      page_data_offset = msg_sfn_allocation[sfn_search_index].
                             sfn_page_descriptor[page_index].page_data_offset ;

      /* Get page fragment's length */
      element_data_length = msg_sfn_allocation[sfn_search_index].
                                  sfn_page_descriptor[page_index].data_length ;

      /* Get page fragment's page properties */
      page_properties = msg_sfn_allocation[sfn_search_index].
                              sfn_page_descriptor[page_index].page_properties ;

      /* Search for existing missing page in the existing missing page list */
      if ((new_incoming_page = cb_find_existing_page_buffer(
         (*current_message_ptr)->message_header.
           current_message_descriptor->missing_page_list,page_number)) == NULL)
      {
        /* Missing page is not found */

        /* Create new page entry */
        if ((new_incoming_page = cb_create_page_descriptor(CB_UMTS_MODE,
          (*current_message_ptr),
            (*current_message_ptr)->message_header.current_message_descriptor->
                                       missing_page_list,page_number)) == NULL)
        {
          MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;
#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
          if(sec_page_data_buffer != NULL)
          {
#ifndef FEATURE_MODEM_HEAP
            gs_free((void *)sec_page_data_buffer);
#else
            modem_mem_free((void *)sec_page_data_buffer, MODEM_MEM_CLIENT_NAS);
#endif
            sec_page_data_buffer = NULL;
          }
#endif
          return ; /* Cancel page reception */
        }
      }
      /*lint -e774*/
      if (new_incoming_page != NULL)
      {
        /* 
        ** Received page fragment table 
        */
        word received_paging_bloks ;

        /* 
        ** Expected page fragment table 
        ** state for fully assembled page
        */
        word paging_bloks_mask ;

       (*current_message_ptr)->message_header.
                 current_message_descriptor->current_page = new_incoming_page ;

        MSG_HIGH_DS_3(cb_as_id,"BMC: Processing %d from %d offset into %d",received_sfn,pdu_offset,page_data_offset) ;
        MSG_HIGH_DS_1(cb_as_id,"BMC: Total length %d",element_data_length) ;

        if(page_data_offset < 88 )
        {/* Copy current page fragment into page buffer */
          memscpy((void *)(new_incoming_page->page_buffer+page_data_offset), 
                   sizeof(new_incoming_page->page_buffer)-page_data_offset,
                                   data_ptr+pdu_offset,element_data_length) ;
        }
        /* Check whether current page fragment carries page length */
        if (page_properties & CB_BMC_PAGE_LENGTH_IND_MASK)
        {
          /* Read current page length */
          new_incoming_page->page_length = 
                         *(byte *)(data_ptr+pdu_offset+element_data_length) ;

          MSG_HIGH_DS_1(cb_as_id,"BMC: Received page length is equal to %d",
                                        new_incoming_page->page_length) ;
        }
      
        /* Update page's received page fragment table */
        new_incoming_page->
          rat_page_properties.umts_received_paging_block |= (word)
           ((word)1 << ((page_properties&CB_BMC_PAGING_BLOCK_NUMBER_MASK)-1)) ;      

        /* Read page's received page fragment table */
        received_paging_bloks = 
            new_incoming_page->rat_page_properties.umts_received_paging_block ;

        /* 
        ** Get expected page fragment table state for fully assembled page
        ** based on predicted number of page fragments which are assembling
        ** current page 
        */
        paging_bloks_mask = 
                       cb_bmc_get_mask(pdu_distribution_table[page_number-1]) ;

        if ((received_paging_bloks & paging_bloks_mask) == paging_bloks_mask)
        {
          /* Page has been successfully constructed */

          /* Extract page properties */
          word current_message_id = 
                               (*current_message_ptr)->message_header.message_id ;
          word current_message_code =(*current_message_ptr)->
                      message_header.current_message_descriptor->message_code ;
          byte current_gs_indicator = (*current_message_ptr)->
                      message_header.current_message_descriptor->gs_indicator ;
          byte current_update_number = (*current_message_ptr)->
                     message_header.current_message_descriptor->update_number ;
          byte current_data_coding_scheme =(*current_message_ptr)->
               message_header.current_message_descriptor->data_coding_scheme  ;
          byte current_number_of_pages = (*current_message_ptr)->
                   message_header.current_message_descriptor->number_of_pages ;

          /* Update received page's table */
          (*current_message_ptr)->message_header.
                current_message_descriptor->received_page_indicator |=
                                           (word)((word)1 << (page_number-1)) ;
                  
          /* 
          ** Construct GSM backward compatible page 
          ** header which are expected by WMS
          */
          cb_bmc_forge_page_header(new_incoming_page->page_buffer,
                                    current_message_id,
                                    current_message_code,
                                    current_gs_indicator,
                                    current_update_number,
                                    current_data_coding_scheme,
                                    page_number,
                                    current_number_of_pages) ;
#ifdef FEATURE_NAS_REL6
          MSG_HIGH_DS_2(cb_as_id,"SN is %x %x ",(new_incoming_page->page_buffer[0]),(new_incoming_page->page_buffer[1]));
          data_sn = new_incoming_page->page_buffer[0];
          data_sn |= (0xff00)&(new_incoming_page->page_buffer[1]<<8);
#endif

          cb_send_ota_msg_log_packet((uint8)CB_UMTS_MODE,
                    (uint32)(new_incoming_page->page_length+CB_BMC_PAGE_HEADER_LENGTH),(uint8)CB_NORMAL_PAGE,
                                                  new_incoming_page->page_buffer); 
          /* Send received page to WMS task */
          cb_send_wms_data_ind(
            (byte)(new_incoming_page->page_length+CB_BMC_PAGE_HEADER_LENGTH),
                                              new_incoming_page->page_buffer) ;

          /* Send event to Diag */
          cb_send_data_event_packet((*current_message_ptr));

#if defined FEATURE_UMTS_BMC_DRX_SUPPORTED
          cb_active_substate_control(CB_STD_DATA_PAGE_RECD);
#endif

          /* Delete constructed page from missing page list */
          (*current_message_ptr)->
              message_header.current_message_descriptor->current_page = 
                        cb_delete_page((*current_message_ptr),new_incoming_page) ;
         
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
          /*Skip the other slots scheduled for same message id */
          cb_bmc_skip_next_reception((*current_message_ptr)
#ifdef FEATURE_NAS_REL6
          , data_sn
#endif
          );
#endif
        }
      }
      else
      {
        MSG_FATAL_DS( cb_as_id,"Unrecoverable error",0,0,0) ;
      }
      /*lint +e774*/
    }
    }
    
  //once all page process delete the message descriptor
    if(cb_nv_duplication_detection_disabled)
    {
       if((*current_message_ptr)->message_header. current_message_descriptor->missing_page_list==NULL) 
        {
           cb_user_page_info_type *cb_received_message_code = NULL ;
           cb_user_page_info_type *cb_prev_message_code = NULL ;
           cb_prev_message_code = (*current_message_ptr)->message_header.message_info ;
           cb_received_message_code=(*current_message_ptr)->message_header. current_message_descriptor;
           while(cb_prev_message_code!=NULL)
           {
             if(cb_prev_message_code->next_message_code==cb_received_message_code)
             {
                break;
             }
             cb_prev_message_code=cb_prev_message_code->next_message_code;
           }
           current_message = cb_delete_message_code((*current_message_ptr),
                               cb_prev_message_code,cb_received_message_code) ;
           (*current_message_ptr)=current_message;
           if(current_message==NULL)
           {
              basic_cbch_list_entry_point=NULL;
           }
           else if(current_message->prev_message==NULL)
           {
              basic_cbch_list_entry_point=current_message;
           }
                         
        }
    }
    else
    {
       /* Reset Dup_detection_timer if new update number in existing message receieved and timer value is set with this message's timer_offset   */ 
          if((*current_message_ptr)->message_header.current_message_descriptor->timer_offset == 0)
          {
              dword x = gs_enquire_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(CB_TIMER_DUP_DET));
              (*current_message_ptr)->message_header.current_message_descriptor->timer_offset = cb_dup_timer_value - x;
              if(cb_next_min_offset == CB_DUP_TIMER_VALUE_INVALID)
              {
                  cb_next_min_offset = (*current_message_ptr)->message_header.current_message_descriptor->timer_offset;
              }
              cb_update_cache_timer_offsets(x);
          }
          /* Starting the cb_dup_detection_timer for the first message */ 
          else if(cb_timer_status[CB_TIMER_DUP_DET-1] == TIMER_STOPPED)
          {
            cb_dup_start_timer( CB_TIMER_DUP_DET, cb_dup_timer_value);
            (*current_message_ptr)->message_header.current_message_descriptor->timer_offset = 0;
          }
          /* Setting timer_offset value for a message received while cb_dup_detection_timer was runnning for some other message */
          else
          {
              (*current_message_ptr)->message_header.current_message_descriptor->timer_offset = cb_dup_timer_value - gs_enquire_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(CB_TIMER_DUP_DET));
              if(cb_next_min_offset == CB_DUP_TIMER_VALUE_INVALID)
              {
                  cb_next_min_offset = (*current_message_ptr)->message_header.current_message_descriptor->timer_offset;
              }
          }
    }
  }
  else
  {
    MSG_HIGH_DS_1(cb_as_id,"CB: Received PDU has not been expected (SFN %d)",
                                                            received_sfn) ;
  }

#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
  if(sec_page_data_buffer != NULL)
  {
#ifndef FEATURE_MODEM_HEAP
    gs_free((void *)sec_page_data_buffer);
#else
    modem_mem_free((void *)sec_page_data_buffer, MODEM_MEM_CLIENT_NAS);
#endif
    sec_page_data_buffer = NULL;
  }
#endif
} /* end of cb_bmc_process_incoming_data() */

#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
/*lint +e715*/
/*===========================================================================

FUNCTION    CB_BMC_IDENT_HEADER_PDU

DESCRIPTION
  This function is used in order to identify RLC PDU which carries BMC message
  header

DEPENDENCIES
  None

RETURN VALUE
  The function returns TRUE when BMC Header is present and FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_bmc_ident_header_pdu
(
  dword pdu_sfn,
  word sfn_allocation_length,
  cb_bmc_pdu_allocation_type *sfn_allocation
)
{
  /* No PDU is expected */
  if (sfn_allocation_length == 0)
  {
    return FALSE ;
  } else if (sfn_allocation[0].sfn == pdu_sfn)
  {
    return TRUE ; 
  }

  /* Incoming RLC PDU does not carry BMC message header */
  return FALSE ;

} /* end of cb_bmc_ident_header_pdu() */
#endif

/*===========================================================================

FUNCTION    CB_BMC_PARSE_MSG_HEADER

DESCRIPTION
  This function processes BMC message header

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean cb_bmc_parse_msg_header
(
  /* Pointer to incoming RLC PDU */
  byte *rlc_pdu_ptr,

  /* Parsed incoming page type */
  byte *msg_type,

  /* 
  ** Parsed Scheduling period length if incoming
  ** RLC PDU carries BMC Scheduling message
  */
  word *scheduling_period_length,

  /* Parsed Message ID */
  word *message_id,

  /* Parsed Message Code */
  word *message_code,

  /* Parsed GS indicator */
  byte *gs,

  /* Parsed Updfate number */
  byte *update_number,

  /* Parsed Data Coding Scheme */
  byte *data_coding_scheme,

  /* Parsed number of pages */
  byte *number_of_pages
)
{
  /* Get received Message Type */
  *msg_type = rlc_pdu_ptr[CB_BMC_MESSAGE_TYPE_OFFSET] ;

  if (*msg_type == CBS_SCHEDULE_TYPE)
  {
    MSG_HIGH_DS_0(cb_as_id,"CB: Received Scheduling message") ;
      
    /* Get Scheduling period length */
    *scheduling_period_length = 
                          rlc_pdu_ptr[CB_BMC_SCHEDULING_PERIOD_LENGTH_OFFSET] ;

    MSG_HIGH_DS_1(cb_as_id,"CB: Scheduling Period length = %d",
                                               *scheduling_period_length) ;
  }
  else if (*msg_type == CBS_MESSAGE_TYPE)
  {
    word message_code_high ;
    word serial_number ;

    /* Extract Message ID */
    *message_id = (rlc_pdu_ptr[CB_BMC_MESSAGE_ID_OFFSET] << 8) | 
                                      rlc_pdu_ptr[CB_BMC_MESSAGE_ID_OFFSET+1] ;

    MSG_HIGH_DS_1(cb_as_id,"CB: Receied CB Message ID %02X",*message_id) ;


    /* Extract Serial number */
    serial_number = (rlc_pdu_ptr[CB_BMC_SERIAL_NUMBER_OFFSET] << 8) | 
                                   rlc_pdu_ptr[CB_BMC_SERIAL_NUMBER_OFFSET+1] ;

    /* Extract message GS */
    *gs = (serial_number & CB_BMC_GS_MASK) >> 14 ;

    MSG_HIGH_DS_1(cb_as_id,"CB: Receied CB message GS %02X",*gs) ;

    /* Extract Message code */
    message_code_high =
                   (serial_number & (word)CB_BMC_MESSAGE_CODE_HIGH_MASK) >> 4 ;

    *message_code = (word)message_code_high | 
                        ((serial_number & CB_BMC_MESSAGE_CODE_LOW_MASK) >> 4) ;

    MSG_HIGH_DS_1(cb_as_id,"CB: Receied CB Message Code %02X",*message_code) ;

    /* Extract Update Number */
    *update_number = (byte)(serial_number & CB_BMC_UPDATE_NUMBER_MASK) ;

    MSG_HIGH_DS_1(cb_as_id,"CB: Receied CB Message Update Number %02X",*update_number) ;

    /* Extract Coding Scheme */
    *data_coding_scheme = rlc_pdu_ptr[CB_BMC_DATA_CODING_OFFSET] ;

    MSG_HIGH_DS_1(cb_as_id,"CB: Received data coding scheme %X",*data_coding_scheme) ;


    /* Extract Total Number of Pages sent */
    *number_of_pages = rlc_pdu_ptr[CB_BMC_NUMBER_OF_PAGES_OFFSET] ;

    if (*number_of_pages)
    {
      MSG_HIGH_DS_1(cb_as_id,"CB: Number of pages %d",*number_of_pages) ;
    }
    else{
      MSG_ERROR_DS_1(cb_as_id,"Invalid Number of Pages %d", *number_of_pages);
      return FALSE;
    }   

  }
  return TRUE;
} /* end of cb_bmc_parse_msg_header() */

/*===========================================================================

FUNCTION    CB_BMC_CALCULATE_SFN_MSG_ALLOCATION

DESCRIPTION
  This function processes incoming RLC PDU in order to buils RLC PDU SFN 
  Allocation table according to the incoming BMC message type

DEPENDENCIES
  None

RETURN VALUE
  Function returns size of RLC PDU SFN Allocation table

SIDE EFFECTS
  None

===========================================================================*/

word cb_bmc_calculate_sfn_msg_allocation
(
  /* 
  ** SFN that reception of incoming 
  ** RLC PDU has been started from 
  */
  dword incoming_sfn,

  /* Incoming messaeg type */
  byte msg_type,

  /* 
  ** Incoming BMC message expected 
  ** number of pages
  */
  byte *received_number_of_pages,

  /* 
  ** Scheduling period length, if current 
  ** message is a Scheduling message
  */
  word scheduling_period_length,

  /* Length of incoming RLC PDU */
  word *pdu_length,

  /* Pointer to incoming RLC PDU */
  byte li_length,

  /* Pointed to RLC PDU SFN Allocation table */
  cb_bmc_pdu_allocation_type *pdu_allocation_table,

#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  /* 
  ** SFN that the next expected BMC 
  ** message should be started from
  */
  dword *next_expected_message_sfn,
#endif

  /* Pointer to PDU Distribution Table */
  byte *pdu_distribution_table
)
{
  /* Number of pages */
  byte number_of_pages = *received_number_of_pages ;

  /* 
  ** Estimated length (in octets) of incoming 
  ** BMC message
  */
  word incoming_message_length_draft = 0 ;
  /* 
  ** Number of RLC PDUs which need to be 
  ** transmitted in order to deliver current 
  */
  /* BMC message */
  byte number_of_pdus_per_message = 0;
  byte max_allowed_pdus_per_message = 0 ;

  boolean page_len_missing = FALSE;

  word sfn_index = 0 ;

  switch (msg_type)
  {
    case CBS_MESSAGE_TYPE  :
      {
        /* Current BMC message page number */
        byte page_number = 1 ;
        /* 
        ** Number of page fragment which has 
        ** been transmitted as one RLC PDU 
        */
        byte paging_block_number = 1 ;
        byte page_index = 0 ;

        /* Page length */
        byte page_length = CB_BMC_DATA_PAGE_LENGTH ;

#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
        /* RLC PDU offset */
        word pdu_data_offset = CB_BMC_HEADER_LENGTH ;

        /* RLC PDU remaining length */
        word pdu_remaining_length = (word)(*pdu_length-CB_BMC_HEADER_LENGTH);
#else
        /* RLC PDU offset */
        byte pdu_data_offset = CB_BMC_HEADER_LENGTH ;

        /* RLC PDU remaining length */
        byte pdu_remaining_length = (byte)(*pdu_length-CB_BMC_HEADER_LENGTH);
#endif

        /* BMC Page offset */
        byte page_data_offset = CB_PAGE_HEADER_LENGTH ;

        /* Current page fragment length */
        byte data_length = 0 ;

        if (number_of_pages > CB_MAX_NUMBER_OF_PAGES)
        {
          MSG_HIGH_DS_1(cb_as_id,"BMC: Message too long (%d pages), truncated to 15 pages",
                              number_of_pages) ;

          *received_number_of_pages = CB_MAX_NUMBER_OF_PAGES ;

           number_of_pages = CB_MAX_NUMBER_OF_PAGES ;
        }

        /* Calculate estimated BMC Message length */
        incoming_message_length_draft = 
                    li_length + CB_BMC_HEADER_LENGTH + 
                                         CB_BMC_PAGE_LENGTH * number_of_pages ;

#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
        if(*pdu_length > CB_BMC_MAX_DATA_MESSAGE_LENGTH)
        {
          MSG_HIGH_DS_1(cb_as_id,"Truncating RLC PDU - PDU length %d is greater than MAX supported data message length",*pdu_length);
          *pdu_length = CB_BMC_MAX_DATA_MESSAGE_LENGTH;
        }

        if ( *pdu_length != (incoming_message_length_draft - li_length) ) 
        {
          MSG_HIGH_DS_2(cb_as_id,"BMC: Message length (%d) not equal to expected length (%d)",
                                                         *pdu_length, incoming_message_length_draft - li_length) ;
          return 0;
        }

        max_allowed_pdus_per_message = number_of_pdus_per_message = CB_BMC_MAX_SDUS_PER_MSG;

#else
        /* 
        ** Calculate number of RLC PDU which are to be 
        ** transmitted in order to deliver estimated message
        */
        max_allowed_pdus_per_message = 
          (incoming_message_length_draft%cb_bmc_rlc_pdu_size)==0?
                   (byte)(incoming_message_length_draft/cb_bmc_rlc_pdu_size):
                   (byte)((incoming_message_length_draft/cb_bmc_rlc_pdu_size)+1) ;
        
#endif
        /* 
        ** Scan all expected RLC PDUs calculating for every one of them 
        ** positions of delivered page fragments at expected PDUs and 
        ** message pages
        */
        for (sfn_index=0;sfn_index<max_allowed_pdus_per_message; sfn_index++)
        {
          /* Calculate SFN when this RLC PDU expected to be received */
          pdu_allocation_table[sfn_index].sfn = 
            cb_bmc_get_Ki_sfn(incoming_sfn,sfn_index,
                     cb_bmc_ctch_allocation_period,cb_bmc_cbs_frame_offset) ;

          /* Reset page fragment number */
          page_index = 0 ;

          /*If the first byte carries the last page length information
           then read the information and finish page processing*/
          if(page_len_missing)
          {
            /* Save page number */
            pdu_allocation_table[sfn_index].
              sfn_page_descriptor[page_index].page_number = page_number ;

            /* Save page pragment's page offset */
            pdu_allocation_table[sfn_index].
              sfn_page_descriptor[page_index].page_data_offset = CB_BMC_DATA_PAGE_LENGTH + CB_PAGE_HEADER_LENGTH;

            /* Save page pragment's RLC PDU offset */
            pdu_allocation_table[sfn_index].
                sfn_page_descriptor[page_index].pdu_data_offset = 0;

           
            /* Save page fragment length */
            pdu_allocation_table[sfn_index].
                        sfn_page_descriptor[page_index].data_length = 0;

            /* Save page fragment number */
            pdu_allocation_table[sfn_index].
                    sfn_page_descriptor[page_index].page_properties = 
                                                              paging_block_number ;

            /* Save number of page frugments to compleet current page */
            pdu_distribution_table[page_number-1] = paging_block_number ;

             /* Switch on (TRUE) page length indicator */
             pdu_allocation_table[sfn_index].
                        sfn_page_descriptor[page_index].page_properties |= 1<<7 ;


            paging_block_number = 1 ;

            /* Get next BMC message page */
            page_number++ ;

            /*Increment page index*/
            page_index++;

            pdu_remaining_length--;
            pdu_data_offset++;
            page_len_missing = FALSE;
          }

          /* 
          ** Repeat the following calculation until current RLC PDU still has 
          ** significant octets or page counter exeeded number of expected pages 
          */
          while ((pdu_remaining_length != 0) && 
                                              (page_number <= number_of_pages))
          {
            /* Save page number */
            pdu_allocation_table[sfn_index].
              sfn_page_descriptor[page_index].page_number = page_number ;

            /* Save page pragment's page offset */
            pdu_allocation_table[sfn_index].
              sfn_page_descriptor[page_index].page_data_offset = page_data_offset ;

            /* Save page pragment's RLC PDU offset */
            pdu_allocation_table[sfn_index].
                sfn_page_descriptor[page_index].pdu_data_offset = pdu_data_offset ;

            /* Calculate page fragment length */
            data_length = (pdu_remaining_length <= page_length) ?
                                                (byte) pdu_remaining_length: page_length ;

            /* Save page fragment length */
            pdu_allocation_table[sfn_index].
                        sfn_page_descriptor[page_index].data_length = data_length ;

            /* Save page fragment number */
            pdu_allocation_table[sfn_index].
                    sfn_page_descriptor[page_index].page_properties = 
                                                              paging_block_number ;
            /* Calculate RLC PDU remaining length */
            pdu_remaining_length -= data_length ;

            /* Calculate next page fragment RLC PDU offset */
            pdu_data_offset += data_length ;

            /* Calculate BMC page remaining length */
            page_length -= data_length ;

            /* Calculate next page fragment page offset */
            page_data_offset += data_length ;

            /* 
            ** Check whether current BMC message page still 
            ** has a room to copy more page fragments before 
            ** it gets completed 
            */
            if (page_length == 0)
            {
              /* Current page has been finished */
            
              /* Reset page langth */
               page_length = CB_BMC_DATA_PAGE_LENGTH ;

               /* Reset page fragment's page offset */
               page_data_offset = CB_PAGE_HEADER_LENGTH ;

             
              /* 
              ** Skip page length field 
              */
              if (pdu_remaining_length)
              {              
                /* Save number of page frugments to compleet current page */
                pdu_distribution_table[page_number-1] = paging_block_number ;

                paging_block_number = 1 ;

                /* Get next BMC message page */
                page_number++ ;

                
                /* Switch on (TRUE) page length indicator */
                pdu_allocation_table[sfn_index].
                        sfn_page_descriptor[page_index].page_properties |= 1<<7 ;

                pdu_remaining_length-- ;
                pdu_data_offset++ ;
              }
              else
              {
                /*If PDU ends then increment the block number and
                 set the page_len_missing to FALSE to read the page lenth
                 field from the next PDU*/
                paging_block_number++ ;
                page_len_missing = TRUE;
              }
            }
            else
            {
              /* Increment page fragment number */ 
              paging_block_number++ ;
            }

            /* Get next BMC message page */
            page_index++ ;
          }

          /* Reset RLC PDU remaining length */
          pdu_remaining_length = cb_bmc_rlc_pdu_size ;

          /* Reset page fragment's RLC PDU offset */
          pdu_data_offset = 0;

          /* Save number of page fragments per RLC PDU */
          pdu_allocation_table[sfn_index].number_of_pages = page_index ;
        } /* for (sfn_index=0;sfn_index<number_of_pdus_per_message; ...) */
      }
      break ;
    case CBS_SCHEDULE_TYPE :

      /* Calculate estimated Sheduling message length */
      incoming_message_length_draft = 
          (word)(li_length + CB_BMC_SCHEDULING_MSG_HEADER_LENGTH + 
          (((scheduling_period_length%8)==0)?
                   scheduling_period_length/8:(scheduling_period_length/8)+1) +
          (scheduling_period_length*3)
#ifdef FEATURE_NAS_REL6
          + CB_BMC_FUTURE_EXTN_BITMAP_IE_LENGTH
          + (scheduling_period_length*CB_BMC_FUTURE_EXTN_SERIAL_NUMBER_LIST_IE_LENGTH)
          + CB_BMC_FUTURE_EXTN_BITMAP_SERIAL_NUMBER_LENGTH_IE_LENGTH
#endif
          );

#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
      if(*pdu_length > CB_BMC_MAX_SCHEDULING_LENGTH)
      {
        MSG_HIGH_DS_1(cb_as_id,"Truncating RLC PDU - PDU length %d is greater than MAX supported scheduling message length",*pdu_length);
        *pdu_length = CB_BMC_MAX_SCHEDULING_LENGTH;
      }

      if ( *pdu_length > (incoming_message_length_draft - li_length) ) 
      {
        MSG_HIGH_DS_2(cb_as_id,"BMC: Message length (%d) more than the expected length hence truncating to expected length(%d)",
                                                       *pdu_length, incoming_message_length_draft - li_length) ;
        *pdu_length = incoming_message_length_draft - li_length;
      }

      number_of_pdus_per_message = CB_BMC_MAX_SDUS_PER_MSG ;
#else
      /* 
      ** Nubmer of RLC PDUs which are needed in order to transmit estimated 
      ** Scheduling message
      */
      number_of_pdus_per_message = 
          (incoming_message_length_draft%cb_bmc_rlc_pdu_size)==0?
                      (byte)(incoming_message_length_draft/cb_bmc_rlc_pdu_size):
                      (byte)((incoming_message_length_draft/cb_bmc_rlc_pdu_size)+1) ;
#endif
      /*
      ** Update SFN RLC PDU Allocation table
      */
      for (sfn_index=0;sfn_index<number_of_pdus_per_message; sfn_index++)
      {
        pdu_allocation_table[sfn_index].sfn = 
            cb_bmc_get_Ki_sfn(incoming_sfn,sfn_index,
                       cb_bmc_ctch_allocation_period,cb_bmc_cbs_frame_offset) ;
      }

      max_allowed_pdus_per_message = number_of_pdus_per_message;

      break ;
    default:
      MSG_ERROR_DS_0(cb_as_id,"Unsupported message type") ;
      
      number_of_pdus_per_message = 0 ;

      break ;
  }
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  /*
  ** SFN that next expected BMC message should be started from 
  */
  *next_expected_message_sfn = cb_bmc_get_Ki_sfn(incoming_sfn,
        number_of_pdus_per_message,
     cb_bmc_ctch_allocation_period,cb_bmc_cbs_frame_offset) ;
#endif
  return max_allowed_pdus_per_message ;
} /* end of cb_bmc_calculate_sfn_msg_allocation() */

/*===========================================================================

FUNCTION    CB_BMC_PROCESS_INFO_FIELD

DESCRIPTION
  Function copies number of octets from scheduling RLC PDU into scheduling 
  reassembly buffer. When requested length could not be copied due to 
  remaining PDU length only available number of octets are processed. In this 
  case function alter the *remaining_length in order to inform the caller how 
  many octets still need to be processed to complete requested info field.

DEPENDENCIES
  None

RETURN VALUE
  Number of octets have been copied

SIDE EFFECTS
  None

===========================================================================*/
byte cb_bmc_process_info_field
(
  byte info_length,
  word pdu_length, 
  byte *rlc_pdu_ptr,
  byte *scheduling_msg,
  byte *remaining_length
)
{
  /* Copy length */
  byte copy_length = 
        ((int)((int)pdu_length-(int)info_length) >= 0)?
                            info_length:(byte)pdu_length ;

  /* Copy part or entire info field from RLC PDU into reassembly buffer */
  memscpy(scheduling_msg,copy_length,rlc_pdu_ptr,copy_length) ;

  /* Alter remaining length if needed */
  if (copy_length < info_length)
  {
    *remaining_length = info_length - copy_length ;
  }
  else
  {
    *remaining_length = 0 ;
  }

  /* Return processed length */
  return copy_length ;
} /* end of cb_bmc_process_info_field() */

  
/*===========================================================================

FUNCTION    CB_BMC_PROCESS_SCHEDULING_MESSAGE

DESCRIPTION
  Function builds entire Scheduling message from the incoming RLC PDUs

DEPENDENCIES
  None

RETURN VALUE
  TRUE - when message reassembly has been finished and FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_bmc_preprocess_scheduling_message
(
  /* RLC PDU SFN Allocation table length */
  word msg_sfn_allocation_length,

  /* RLC PDU SFN Allocation table length */
  cb_bmc_pdu_allocation_type *msg_sfn_allocation,

  /* Current SFN */
  dword received_sfn,

  /* Incoming RLC PDU Length */
  word pdu_length, 

  /* INcoming RLC PDU */
  byte *rlc_pdu_ptr,

  /* Scheduling period length */
  word new_scheduling_period_length,

  /* Pointer to Scheduling message length */
  word *scheduling_msg_length,

  /* Pointer to Scheduling message reassembly buffer */
  byte *scheduling_msg,

  /* Pointer to Current Cheduling message reassembly state */
  cb_bmc_reassembling_state_type *reassembling_state,

  /* 
  ** Pointer to number of octets remained to 
  ** complete processing current info field 
  */
  byte *remaining_length,

  /* Pointer to number of message description fields */
  byte *number_of_descriptions
)
{
  /* Scheduling message bit map length in octets */
  byte bit_map_length ;

  /* Number of octets have been copied */
  byte copy_length ;

  /* Index var */
  word sfn_search_index ;

  /* Get PDU SFN allocation index */
  if ((sfn_search_index = cb_bmc_get_sfn_allocation_index(
                CBS_SCHEDULE_TYPE,msg_sfn_allocation_length,
                                   msg_sfn_allocation,received_sfn)) != 0xFFFF)
  {
    /* Index is found */

    if (sfn_search_index > 0)
    {

      /* Make sure that previous RLC PDU has been successfully processed */ 
      if ((msg_sfn_allocation[sfn_search_index-1].sfn & 0x8000) == 0)
      {
        /* 
        ** If we skiped previous RLC PDU stop processing current Scheduling
        ** message
        */

        MSG_HIGH_DS_1(cb_as_id,
          "BMC: Detected missing Scheduling message fragment (SFN %d)",
                              msg_sfn_allocation[sfn_search_index-1].sfn) ;

        /* Stop processing remaining RLC PDUs */
        cb_bmc_msg_sfn_allocation_length = 0 ;

        return FALSE ;
      }
    }
  }
  else
  {
    /* Stop processing remaining RLC PDUs */
    cb_bmc_msg_sfn_allocation_length = 0 ;

    return FALSE ;
  }

  if ((*scheduling_msg_length) == 0)
  {
    /* RLC PDU contains Scheduling message header */

    /* Copy message type */
    *(scheduling_msg++) = *(rlc_pdu_ptr++) ;

    pdu_length-- ; (*scheduling_msg_length)++ ;

    /* Copy Scheduling Period Offset */
    *(scheduling_msg++) = *(rlc_pdu_ptr++) ;

    pdu_length-- ; (*scheduling_msg_length)++ ;

    /* Copy Scheduling Period Length */
    *(scheduling_msg++) = *(rlc_pdu_ptr++) ;

    pdu_length-- ; (*scheduling_msg_length)++ ;

    /* Start processing Scheduling bit map */
    *reassembling_state = CB_BMC_BIT_MAP_STATE ;

    /* Calculate corect Bit Map length in octets */
    bit_map_length = ((new_scheduling_period_length%8)==0)?
            (byte)(new_scheduling_period_length/8):
            (byte)((new_scheduling_period_length/8)+1);

    /* Determine current copy length */ 
    copy_length = 
            cb_bmc_process_info_field(bit_map_length,pdu_length,rlc_pdu_ptr,
                                             scheduling_msg,remaining_length) ;

    /* Correct Scheduling message length and RLC PDU pointer */
    scheduling_msg += copy_length ; rlc_pdu_ptr += copy_length ;

    pdu_length -= copy_length; (*scheduling_msg_length) += copy_length ;

    /* Check whether entire Bit Map belongs to current RLC PDU */
    if (copy_length == bit_map_length)
    {
      /* Entire Bit Map has been copied */

      *remaining_length = 0 ;

      /* Go to process Message Description fields */
      *reassembling_state = CB_BMC_DESCRIPTION_STATE ;
    }
    else
    {
      /* PDU has been ended before we finished Bit Map processing */

      /* Mark current entry as "processed" */
      msg_sfn_allocation[sfn_search_index].sfn |= 0x8000 ;

      return FALSE ;
    }

  }
  else
  {
    /* 
    ** Resume Scheduling message processing from the place where
    ** it has been stop due to end of previous RLC PDU
    */
    scheduling_msg += (*scheduling_msg_length) ;
  }

  if (*reassembling_state == CB_BMC_BIT_MAP_STATE)
  {
    /* Complete Scheduling Bit Map processing */
    copy_length = 
            cb_bmc_process_info_field(
                    *remaining_length,pdu_length,rlc_pdu_ptr,
                                             scheduling_msg,remaining_length) ;

    /* Correct Scheduling message length and RLC PDU pointer */
    scheduling_msg += copy_length ; rlc_pdu_ptr += copy_length ;

    pdu_length -= copy_length; (*scheduling_msg_length) += copy_length ;

    /* Check whether entire Bit Map belongs to current RLC PDU */
    if (copy_length == *remaining_length)
    {
      /* Entire Bit Map has been copied */

      *remaining_length = 0 ;

      /* Go to process Message Description fields */
      *reassembling_state = CB_BMC_DESCRIPTION_STATE ;
    }
    else
    {
      /* PDU has been ended before we finished Bit Map processing */

      /* Mark current entry as "processed" */
      msg_sfn_allocation[sfn_search_index].sfn |= 0x8000 ;

      return FALSE ;
    }
  }

  if (*reassembling_state == CB_BMC_DESCRIPTION_STATE)
  {
    /* Description Field length */
    byte description_length = 0 ;

    /* 
    ** Check whether previous description field belonging 
    ** to previous RLC PDU has been fully processed 
    */
    if (*remaining_length > 0)
    {
      /* Complete previous description field processing */
      copy_length = 
            cb_bmc_process_info_field(
                    *remaining_length,pdu_length,rlc_pdu_ptr,
                                             scheduling_msg,remaining_length) ;

      /* Correct Scheduling message length and RLC PDU pointer */
      scheduling_msg += copy_length ; rlc_pdu_ptr += copy_length ;

      pdu_length -= copy_length; (*scheduling_msg_length) += copy_length ;

      /* 
      ** Check whether entire current description 
      ** field belongs to current RLC PDU 
      */
      if (copy_length < *remaining_length)
      {
        /* Mark current entry as "processed" */
        msg_sfn_allocation[sfn_search_index].sfn |= 0x8000 ;

        return FALSE ;
      }
      else
      {
        /* Previous description field processing has been completed */
        *remaining_length = 0 ;

        (*number_of_descriptions)++ ;
      }
    }
    
    /* 
    ** Process Scheduling Description fields until current RLC PDU or
    ** Scheduling message ends 
    */
    while ((pdu_length != 0) && 
                      (*number_of_descriptions < new_scheduling_period_length))
    {
      /* 
      ** Process current Sceduling Description Field 
      ** according to the field type 
      */
      switch ((cb_bmc_description_field_type)(*rlc_pdu_ptr))
      {
        case CB_BMC_NEW_MESSAGE_TYPE:
        case CB_BMC_OLD_MESSAGE_TYPE:
          description_length = 3 ;
          break ;

        case CB_BMC_NEW_MESSAGE_REPETITION_TYPE:
        case CB_BMC_OLD_MESSAGE_REPETITION_TYPE:
          description_length = 2 ;
          break ;

        case CB_BMC_READING_ADVISED_TYPE:
        case CB_BMC_READING_OPTIONAL_TYPE:
        case CB_BMC_SCHEDULE_MESSAGE_TYPE:
        case CB_BMC_CBS41_MESSAGE_TYPE:
        case CB_BMC_NO_MESSAGE_TYPE:
          description_length = 1 ;
          break ;
        default:
          rlc_pdu_ptr++ ; pdu_length-- ;

          MSG_ERROR_DS_0(cb_as_id,"CB: Illigal message description field id") ;

          /* Stop processing remaining RLC PDUs */
          cb_bmc_msg_sfn_allocation_length = 0 ;

          break ;
      }

      /* Copy current Sceduling Description field */
      copy_length = 
            cb_bmc_process_info_field(
                      description_length,pdu_length,rlc_pdu_ptr,
                                             scheduling_msg,remaining_length) ;

      /* Correct Scheduling message length and RLC PDU pointer */
      scheduling_msg += copy_length ; rlc_pdu_ptr += copy_length ;

      pdu_length -= copy_length; (*scheduling_msg_length) += copy_length ;

      if (copy_length < description_length)
      {
        /* Mark current entry as "processed" */
        msg_sfn_allocation[sfn_search_index].sfn |= 0x8000 ;

        return FALSE ;
      }

      (*number_of_descriptions)++ ;
    }
  }

  /* Mark current entry as "processed" */
  msg_sfn_allocation[sfn_search_index].sfn |= 0x8000 ;

#ifdef FEATURE_NAS_REL6
  if(pdu_length!=0)
  {
    /*Copy the future extension bitmap*/
    MSG_HIGH_DS_0(cb_as_id,"Future extension Bitmap is included");
    memscpy(scheduling_msg,pdu_length,rlc_pdu_ptr,pdu_length) ;
    (*scheduling_msg_length) += pdu_length;
  }
#endif
  if (*number_of_descriptions < new_scheduling_period_length)
  {
    return FALSE;
  }
  else
  {
    /* Entire Scheduling Message has been processed */
    return TRUE ;
  }
} /* end of cb_bmc_preprocess_scheduling_message() */

#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT 
/*===========================================================================

FUNCTION    CB_BMC_filter_out_received_pdus

DESCRIPTION
  This function generates skip requests in order to skip entire incoming BMC
  message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_skip_incoming_message
(
  cb_message_list_type       *message_ptr,
  word                        msg_sfn_allocation_length,
  cb_bmc_pdu_allocation_type *msg_sfn_allocation,
  dword                      *skip_allocation
)
{
  word scan_index ;
  word skip_index = 0 ;

  /* Copy expected RLC PDU SFNs into skip sequence */
  for(scan_index=1; scan_index<msg_sfn_allocation_length; scan_index++)
  {
    skip_allocation[skip_index++] = msg_sfn_allocation[scan_index].sfn ;

    msg_sfn_allocation[scan_index].sfn = 0 ;
  }

  MSG_HIGH_DS_0(cb_as_id,"BMC: Skipping entire message") ;

  /* Send Skip sequence to Scheduler for further proccessing */
  cb_bmc_generate_skip_request(message_ptr,skip_index,skip_allocation) ;

} /* end of cb_bmc_skip_incoming_message() */

/*===========================================================================

FUNCTION    CB_BMC_FILTER_OUT_RECEIVED_PDUS

DESCRIPTION
  This function generates skip requests according to list of current message 
  fragments which have been already successfully received

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_filter_out_received_pdus
(
  /* Pointer into task's memory */
  cb_message_list_type       *message_ptr,

  /* Number of message pages need to be received */
  byte                        number_of_pages,

  /* RLC PDU SFN Allocation table length */
  word                        msg_sfn_allocation_length,

  /* RLC PDU SFN Allocation table pointer */
  cb_bmc_pdu_allocation_type *msg_sfn_allocation,

  /* Pointer to Message Distribution table */
  byte                       *pdu_distribution_table,

  /* Pointer to skip sequence buffer */
  dword                      *skip_allocation
)
{
  /* Index vars */
  word paging_block_index = 0;
  word skip_index = 0 ;
  word allocation_index = 0 ;
  byte page_index = 0;

  /* Page Fragment number */
  word paging_block = 0;

  /* Page number */
  byte page_number = 0 ;

  /* Received pages bit map */
  word received_pages = 
          message_ptr->message_header.current_message_descriptor->
                                                      received_page_indicator ;
  /* Received page fragment bit map */
  word received_paging_blocks = 0 ;

  /* Pointer to missing page list */
  cb_missing_page_type *new_incoming_page = NULL ;

  /* Scan all pages need to be received */
  for (page_index=0; page_index<number_of_pages; page_index++)
  {
    /* Get page number */
    page_number = page_index + 1 ;

    /* Determine whether current page successfully has been received */ 
    if (!(received_pages & ((word)1 << page_index)))
    {
      /* Search for current page buffer */
      if ((new_incoming_page = cb_find_existing_page_buffer(
              message_ptr->message_header.current_message_descriptor->
                                       missing_page_list,page_number)) != NULL)
      {
        /* Update cache data base */
        message_ptr->message_header.
                 current_message_descriptor->current_page = new_incoming_page ;

        /* Get received page fragment bit map */
        received_paging_blocks = 
            new_incoming_page->rat_page_properties.umts_received_paging_block ;

        /* 
        ** Scan all page fragments need to be received in order to complete 
        ** reception of current page 
        */
        for (paging_block_index=0; 
                paging_block_index<pdu_distribution_table[page_index]; 
                                                          paging_block_index++)
        {
          /* Get page fragment number */
          paging_block = paging_block_index + 1 ;

          /* Check whether current page fragment has been received */
          if (received_paging_blocks & ((word)1 << paging_block_index))
          {
            for (allocation_index=1;
                 allocation_index<msg_sfn_allocation_length;allocation_index++)
            {
              if (msg_sfn_allocation[allocation_index].number_of_pages == 1) 
              {
                if ((msg_sfn_allocation[allocation_index].
                        sfn_page_descriptor[0].page_number == page_number) &&
                   ((msg_sfn_allocation[allocation_index].
                        sfn_page_descriptor[0].page_properties & 0x7F) == 
                                                                 paging_block))
                {
                  skip_allocation[skip_index++] = 
                                     msg_sfn_allocation[allocation_index].sfn ;

                  msg_sfn_allocation[allocation_index].sfn = 0 ;
                }
                else
                {
                  if (msg_sfn_allocation[allocation_index].
                             sfn_page_descriptor[0].page_number > page_number)
                  {
                    break ;
                  }
                }
              }
              else
              {
                if ((msg_sfn_allocation[allocation_index].
                        sfn_page_descriptor[0].page_number == page_number) &&
                      ((msg_sfn_allocation[allocation_index].
                        sfn_page_descriptor[0].page_properties & 0x7F) == 
                                                                 paging_block))
                {
                  if (received_pages & ((word)1 << (page_index+1)))
                  {
                    skip_allocation[skip_index++] = 
                                     msg_sfn_allocation[allocation_index].sfn ;

                    msg_sfn_allocation[allocation_index].sfn = 0 ;
                  }
                  else
                  {
                    if ((new_incoming_page = cb_find_existing_page_buffer(
                         message_ptr->message_header.
                          current_message_descriptor->
                            missing_page_list,(byte)(page_number+1))) != NULL)
                    {

                      message_ptr->
                            message_header.current_message_descriptor->
                                             current_page = new_incoming_page ;

                      received_paging_blocks = new_incoming_page->
                               rat_page_properties.umts_received_paging_block ;

                      if (received_paging_blocks & 1)
                      {
                        skip_allocation[skip_index++] = 
                                     msg_sfn_allocation[allocation_index].sfn ;

                        msg_sfn_allocation[allocation_index].sfn = 0 ;
                      }
                    }
                  }
                }
                else
                {
                  if (msg_sfn_allocation[allocation_index].
                             sfn_page_descriptor[0].page_number > page_number)
                  {
                    break ;
                  }
                }
              }
            }
          }
        }
      }
    }
    else
    {
      for (allocation_index=1;
                 allocation_index<msg_sfn_allocation_length;allocation_index++)
      {
        if (msg_sfn_allocation[allocation_index].number_of_pages == 1) 
        {
          if (msg_sfn_allocation[allocation_index].
                             sfn_page_descriptor[0].page_number == page_number)
          {
            skip_allocation[skip_index++] = 
                                     msg_sfn_allocation[allocation_index].sfn ;

            msg_sfn_allocation[allocation_index].sfn = 0 ;
          }
          else
          {
            if (msg_sfn_allocation[allocation_index].
                             sfn_page_descriptor[0].page_number > page_number)
            {
              break ;
            }
          }
        }
        else
        {
          if (msg_sfn_allocation[allocation_index].
                             sfn_page_descriptor[0].page_number == page_number)
          {
            if (received_pages & ((word)1 << (page_index+1)))
            {
              skip_allocation[skip_index++] = 
                                     msg_sfn_allocation[allocation_index].sfn ;

              msg_sfn_allocation[allocation_index].sfn = 0 ;
            }
            else
            {
              if ((new_incoming_page = cb_find_existing_page_buffer(
                     message_ptr->message_header.
                         current_message_descriptor->
                            missing_page_list,(byte)(page_number+1))) != NULL)
              {

                message_ptr->message_header.current_message_descriptor->
                                             current_page = new_incoming_page ;

                received_paging_blocks = new_incoming_page->
                               rat_page_properties.umts_received_paging_block ;

                if (received_paging_blocks & 1)
                {
                  skip_allocation[skip_index++] = 
                                     msg_sfn_allocation[allocation_index].sfn ;

                  msg_sfn_allocation[allocation_index].sfn = 0 ;
                }
              }
            }
          }
          else
          {
            if (msg_sfn_allocation[allocation_index].
                             sfn_page_descriptor[0].page_number > page_number)
            {
              break ;
            }
          }
        }
      }
    }
  }

  if (skip_index > 0)
  {
    MSG_HIGH_DS_0(cb_as_id,"BMC: Skipping SFN list") ;

    cb_bmc_generate_skip_request(message_ptr,skip_index,skip_allocation) ;
  }

} /* end of cb_bmc_filter_out_received_pdus() */
#endif

/*===========================================================================

FUNCTION    CB_BMC_PROCESS_RLC_PDU

DESCRIPTION
  This function recalculates expected PDU message allocation table when 
  the task receives RLC PDU which is shorter or longer then expected

DEPENDENCIES
  None

RETURN VALUE
  New expected PDU allocation length

SIDE EFFECTS
  None

===========================================================================*/
/* 
word cb_bmc_recalculate_sfn_msg_allocation
(
  word received_sfn,
  word pdu_length,
  word msg_sfn_allocation_length,
  cb_bmc_pdu_allocation_type  *pdu_allocation_table,
  dword *next_expected_message_sfn,
  byte *pdu_distribution_table
)
{
  word pdu_index ;
  byte paging_block_index ; 
  word sfn_index = 0 ;
  byte page_number = 1 ;
  byte paging_block_number = 1 ;
  byte page_index = 0 ;
  byte page_length = CB_BMC_DATA_PAGE_LENGTH ;
  byte pdu_data_offset = CB_BMC_HEADER_LENGTH ;
  byte pdu_remaining_length = pdu_length-CB_BMC_HEADER_LENGTH ;
  byte page_data_offset = CB_PAGE_HEADER_LENGTH ;
  byte data_length = 0 ;

  for (pdu_index=0;pdu_index<msg_sfn_allocation_length; pdu_index++)
  {
    if (pdu_allocation_table[pdu_index].sfn == received_sfn)
    {
      pdu_index-- ;

      break ;
    }
  }

  if (pdu_allocation_table[pdu_index].number_of_pages == 1)
  {
    paging_block_index = 0 ;
  }
  else
  {
    paging_block_index = 1 ;
  }

  pdu_remaining_length = (byte)pdu_length ;

  page_data_offset = 
    pdu_allocation_table[pdu_index].
       sfn_page_descriptor[paging_block_index].page_data_offset + 
            pdu_allocation_table[pdu_index].
                       sfn_page_descriptor[paging_block_index].data_length ;

  pdu_data_offset = 0 ;

  page_length = CB_BMC_DATA_PAGE_LENGTH - page_data_offset ;

  page_number = pdu_allocation_table[pdu_index].
                       sfn_page_descriptor[paging_block_index].page_number ;
  
  paging_block_number = (pdu_allocation_table[pdu_index].
           sfn_page_descriptor[paging_block_index].page_properties & 0x7F) ;

  if (pdu_allocation_table[pdu_index].
            sfn_page_descriptor[paging_block_index].page_properties & 0x80)
  {
    page_number++ ;

    paging_block_number = 1 ;
  }

  for (sfn_index=pdu_index;sfn_index<msg_sfn_allocation_length; sfn_index++)
  {
    pdu_allocation_table[sfn_index].sfn = 
            cb_bmc_get_Ki_sfn(received_sfn,sfn_index,
                                               cb_bmc_ctch_allocation_period) ;
    page_index = 0 ;

    while ((pdu_remaining_length != 0) && (page_number <= number_of_pages))
    {
      pdu_allocation_table[sfn_index].
              sfn_page_descriptor[page_index].page_number = page_number ;

      pdu_allocation_table[sfn_index].sfn_page_descriptor[page_index].
                                          page_data_offset = page_data_offset ;

      pdu_allocation_table[sfn_index].sfn_page_descriptor[page_index].
                                            pdu_data_offset = pdu_data_offset ;

      data_length = (pdu_remaining_length <= page_length) ?
                                            pdu_remaining_length: page_length ;

      pdu_allocation_table[sfn_index].
                    sfn_page_descriptor[page_index].data_length = data_length ;

      pdu_allocation_table[sfn_index].
                    sfn_page_descriptor[page_index].page_properties = 
                                                          paging_block_number ;
      pdu_remaining_length -= data_length ;

      pdu_data_offset += data_length ;

      page_length -= data_length ;

      page_data_offset += data_length ;

      if (page_length == 0)
      {
        pdu_distribution_table[page_number-1] = paging_block_number ;

        paging_block_number = 1 ;

        page_number++ ;

        page_length = CB_BMC_DATA_PAGE_LENGTH ;

        page_data_offset = CB_PAGE_HEADER_LENGTH ;

        pdu_allocation_table[sfn_index].
                      sfn_page_descriptor[page_index].page_properties |= 1<<7 ;

        pdu_remaining_length-- ;

        pdu_data_offset++ ;
      }
      else
      {
        paging_block_number++ ;
      }

      page_index++ ;
    }

    pdu_remaining_length = cb_bmc_rlc_pdu_size ;

    pdu_data_offset = 0;

    pdu_allocation_table[sfn_index].number_of_pages = page_index ;
  }

  return msg_sfn_allocation_length ;

} */

/* end of cb_bmc_recalculate_sfn_msg_allocation() */


#ifdef FEATURE_WCDMA_ETWS
/*===========================================================================

FUNCTION    CB_BMC_ETWS_GET_MESSAGE_ID_AND_SERIAL_NUMBER

DESCRIPTION
  This function extracts the ETWS message id and Serial number form RRC ETWS Primary notification indication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void cb_bmc_etws_get_message_id_and_serial_number
(
  word* message_id, 
  word* serial_number, 
  const rrc_etws_prim_notification_ind_type* rrc_etws_prim_notification_ind
)
{
  /* Extract Message ID */
  *message_id = ((rrc_etws_prim_notification_ind->msg_id[0] << 8) | 
                  rrc_etws_prim_notification_ind->msg_id[1]) ;
  MSG_HIGH_DS_1(cb_as_id,"CB ETWS: Received CB Message ID %02X",*message_id) ;

  /* Extract Serial number */
  *serial_number = ((rrc_etws_prim_notification_ind->sr_num[0] << 8) | 
  rrc_etws_prim_notification_ind->sr_num[1]) ;
  MSG_HIGH_DS_1(cb_as_id,"CB ETWS: Received CB Serial number ID %02X",*serial_number) ;
}

/*===========================================================================

FUNCTION    CB_BMC_ETWS_PRIM_DUPLICATION_DETECTOR

DESCRIPTION
  This function implements a BMC ETWS Primary notification duplication detector

DEPENDENCIES
  None

RETURN VALUE
  TRUE - current paging block is a duplication of the earlier received one
  FALSE - duplication has not been found

SIDE EFFECTS
  None

===========================================================================*/
static boolean cb_bmc_etws_prim_duplication_detector
(
  const cb_bmc_etws_msg_info_type* existing_message_code,
  byte new_gs,
  byte new_update_number
)
{
  if(is_update_number_not_changed (existing_message_code->update_number,
                                  new_update_number))
  {
    if (existing_message_code->gs_indicator == new_gs)
    {
      MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Duplication detected and this ETWS message is dropped");
      return TRUE;
    }
    else
    {
      MSG_HIGH_DS_0(cb_as_id,"CB ETWS: GS indicator has been changed") ;
    }
  }
  else
  {
    MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Update Number has been changed") ;
  }
  return FALSE ;
} /* end cb_duplication_detector() */
/*===========================================================================

FUNCTION    IS_ETWS_MESSAGE_ID_REQUESTED

DESCRIPTION
  This function validates the received ETWS message ID
  IF the message is Valid ETWS message id and WMS has requested
  that message id then this function returns TRUE
  Else return FALSE

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/

static boolean is_etws_message_id_requested( word message_id)
{
  boolean message_id_requested = TRUE;
  /*
  **Verify whether received message id is valid and supported 
  **ETWS message or not before searching for the same in WMS
  **Search list
  */
  switch (message_id) 
  {
    case CB_BMC_ETWS_EARTQUAKE_ID:
      MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Received Eartquake ETWS ID");
      break;
    case CB_BMC_ETWS_TSUNAMI_ID :
      MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Received Tsunami ETWS ID");
      break;
    case CB_BMC_ETWS_EARTQUAKE_AND_TSUNAMI_ID :
      MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Received Earthquake and Tsunami ETWS ID");
      break;
    case CB_BMC_ETWS_TEST_ID  :
      MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Received Test ETWS ID");
      break;
    case CB_BMC_ETWS_OTHER_EMERGENCY_ID :
      MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Received Other emergency ETWS ID");
      break;
    case CB_BMC_ETWS_FUTURE_EXTENSION_01 :
    case CB_BMC_ETWS_FUTURE_EXTENSION_02 :
    case CB_BMC_ETWS_FUTURE_EXTENSION_03 :
      MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Received unsupported Future extension ETWS ID");
      message_id_requested = FALSE;
      break;
    default:
      MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Received unknown ETWS ID");
      message_id_requested = FALSE;
      break;
  }
  if(message_id_requested)
  {
    message_id_requested = cb_message_is_requested(message_id);
  }
  return message_id_requested;
}
/*===========================================================================

FUNCTION    CB_BMC_ETWS_PARSE_SERIAL_NUMBER

DESCRIPTION
  This function pasres Serial number and extracts GS, Message code and 
  Update number 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void cb_bmc_etws_parse_serial_number
(
  word serial_number,

  /* Parsed Message Code */
  word *message_code,

  /* Parsed GS indicator */
  byte *gs,

  /* Parsed Update number */
  byte *update_number
)
{
    word message_code_high ;
    /* Extract message GS */
    *gs = (serial_number & CB_BMC_GS_MASK) >> 14 ;
     MSG_HIGH_DS_1(cb_as_id,"CB ETWS: Received CB message GS %02X",*gs) ;

    /* Extract Message code */
    message_code_high =
                   (serial_number & (word)CB_BMC_MESSAGE_CODE_HIGH_MASK) >> 4 ;

    *message_code = (word)message_code_high | 
                        ((serial_number & CB_BMC_MESSAGE_CODE_LOW_MASK) >> 4) ;

    MSG_HIGH_DS_1(cb_as_id,"CB ETWS: Received CB Message Code %02X",*message_code) ;

    /* Extract Update Number */
    *update_number = (byte)(serial_number & CB_BMC_UPDATE_NUMBER_MASK) ;

    MSG_HIGH_DS_1(cb_as_id,"CB ETWS: Received CB Message Update Number %02X",*update_number) ;
}

#ifdef FEATURE_DUAL_SIM
extern unsigned char  mm_dual_standby_mode;
#endif

/*===========================================================================

FUNCTION    CB_BMC_PROCESS_ETWS_PRIM_NOTIFICATION_IND

DESCRIPTION
  This function process the ETWS Prim indication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_process_etws_prim_notification_ind
(
  const rrc_etws_prim_notification_ind_type* rrc_etws_prim_notification_ind,
  const cb_state_type* cur_state 
)
{
  /* Message properties */
  word message_id;
  word serial_number;
  word message_code;
  byte gs ;
  byte update_number ;
  sys_plmn_id_s_type incoming_plmn;
  cb_state_type new_state = *cur_state;
  cb_bmc_etws_msg_list_type *new_message_ptr = NULL ;
  cb_bmc_etws_msg_info_type *current_etws_message_code = NULL;
  /* Extract the ETWS message id and serial number here*/
  cb_bmc_etws_get_message_id_and_serial_number(&message_id, &serial_number, rrc_etws_prim_notification_ind);

  incoming_plmn = cb_convert_rrc_plmn_id_to_sys_plmn_id(
                  rrc_etws_prim_notification_ind->plmn_id) ;

#ifdef FEATURE_DUAL_SIM
  if(mm_dual_standby_mode)
  {
    MSG_HIGH_DS_0(cb_as_id,"CB ETWS:Subscription W+G is enabled SO PrimaryNotification Should  discarded");
    return;
  }
#endif

  /*
  **Need to find whether UE supports it or not and if supports search WMS has
  **requested that message ID or not 
  **If WMS has not requested then return from here
  */
  if(!is_etws_message_id_requested(message_id))
  {
    MSG_HIGH_DS_0(cb_as_id,"CB ETWS: ETWS primary message is not in search list hence discarded");
    return;
  }
  else
  {
    MSG_HIGH_DS_1(cb_as_id,"CB ETWS: Received message id %d is present in search list" ,message_id);
    /*
    **Parse Message code, GS and  Update number
    **from the input serial number from RRC
    */

    cb_bmc_etws_parse_serial_number (serial_number, &message_code,
                                     &gs, &update_number);

    /*search for message id in the current received cache*/
    if((new_message_ptr = cb_find_etws_message( basic_etws_list_entry_point,
                          message_id)) == NULL)
    {
      MSG_HIGH_DS_0(cb_as_id,"CB ETWS: Received New message id - create new message entry" );
      /* 
      ** This is a first message ID reception, create new message 
      ** descriptor 
      */
      if ((new_message_ptr = cb_create_etws_primary_message_entry(message_id)) == NULL)
      {
        MSG_ERROR_DS_0(cb_as_id,"CB ETWS: Not enough memory") ;
        return ;
      }
      /* 
      ** This is a first message code and serial number reception, create a new message 
      ** code descriptor 
      */
      if ((new_message_ptr->message_header.message_info = 
            cb_create_etws_message_code_entry(  new_message_ptr,message_code,
                                              gs, update_number, (byte *)&incoming_plmn)) == NULL)
      {
        MSG_ERROR_DS_0(cb_as_id,"CB ETWS: Not enough memory") ;
        return ;
      }
      /* Put new message descriptor into cache memory */
      (void)cb_put_etws_message_into_list( basic_etws_list_entry_point,new_message_ptr) ;
      current_etws_message_code = new_message_ptr->message_header.message_info;
    }
    else
    {
      /* Search for the current serial in the serail number
      ** Descriptor list 
      */
      if ((current_etws_message_code = cb_etws_find_message_code(new_message_ptr,message_code)) == NULL)
      {
        MSG_HIGH_DS_1(cb_as_id,"CB ETWS: serial_number %d not found",serial_number) ;
        /* 
        ** This is a first serial number reception,create new message 
        ** descriptor
        */
       
         if ((current_etws_message_code = cb_create_etws_message_code_entry(new_message_ptr,message_code,
                                                gs, update_number, (byte *)&incoming_plmn)) == NULL)
         {
           MSG_ERROR_DS_0(cb_as_id,"CB ETWS: Not enough memory") ;
           return ;
         }
      }
      else
      {
         if(cb_bmc_etws_prim_duplication_detector(current_etws_message_code,
                                                  gs, update_number))
         {
           /*check whether duplication detection needs to be sent or not*/
           MSG_HIGH_DS_2(cb_as_id,"CB ETWS: Duplicate ETWS Primary Ind with message id %d and serial number %d", message_id, serial_number);
           return;
         }
         else //The update no has been changed or the GS value has been changed so please update the field 
         {
           current_etws_message_code->update_number=update_number;
           current_etws_message_code->gs_indicator=gs;
           memscpy((void *)current_etws_message_code->plmn,sizeof(current_etws_message_code->plmn),(void *)&incoming_plmn, PLMN_VALUE_LENGTH) ; 
         }
      }
    }
  }

  /* 
  ** Move to active state if we are blocking as only ETWS messages are requested
  **
  */
  cb_etws_state_control
  (
    (cb_state_id_type)cb_etws_substate_control(CB_ETWS_STD_PRIMARY_NOTIFICATION_RECD),
    new_state
  );
  
  cb_send_wms_etws_prim_notification_ind(&incoming_plmn,
                                         message_id,
                                         serial_number, &rrc_etws_prim_notification_ind->warning_type[0],
                                         rrc_etws_prim_notification_ind->warning_sec_info_present, &rrc_etws_prim_notification_ind->warning_sec_info[0]);
  /* Update cache memory pointers */
  basic_etws_list      = new_message_ptr ;
  
  /* Reset Dup_detection_timer if new update number in existing message receieved and timer value is set with this message's timer_offset   */
  if(current_etws_message_code->timer_offset == 0)
  {
    dword x = gs_enquire_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(CB_TIMER_DUP_DET));
    current_etws_message_code->timer_offset = cb_dup_timer_value - x;
    if(cb_next_min_offset == CB_DUP_TIMER_VALUE_INVALID)
    {
      cb_next_min_offset = current_etws_message_code->timer_offset;
    }
    cb_update_cache_timer_offsets(x);
  }
  /* Starting the cb_dup_detection_timer for the first message */  
  else if(cb_timer_status[CB_TIMER_DUP_DET-1] == TIMER_STOPPED)
  {
    cb_dup_start_timer( CB_TIMER_DUP_DET, cb_dup_timer_value);
    current_etws_message_code->timer_offset = 0;
  }
  /* Setting timer_offset value for a message received while cb_etws_dup_detection_timer was runnning for some other message */
  else
  {
    current_etws_message_code->timer_offset = cb_dup_timer_value - gs_enquire_timer( GS_QUEUE_CB, CB_TIMER_TAG_AS_ID(CB_TIMER_DUP_DET));
    if(cb_next_min_offset == CB_DUP_TIMER_VALUE_INVALID)
    {
      cb_next_min_offset = current_etws_message_code->timer_offset;
    }
  }
}
#endif
/*===========================================================================

FUNCTION    CB_BMC_PROCESS_RLC_PDU

DESCRIPTION
  This function processes incoming RLC PDU

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_process_rlc_pdu
(
  /*
  ** SFN number that incoming 
  ** RLC PDU is assosiated with 
  */
  dword    received_sfn, 

  /*
  ** end SFN number that incoming 
  ** RLC PDU is assosiated with 
  */
  dword    received_end_sfn, 

  /* 
  ** Number of RLC PDU octets which 
  ** were usedin order to transmit LIs 
  */
  byte     li_length,

#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  /* 
  ** Identifies incoming RLC PDU as a
  ** first PDU is belonging to a new
  ** BMC message
  */
  boolean  first_pdu_indicator,
#endif

  /* RLC PDU length */
  word     pdu_length,

  /* RLC PDU payload */
  byte    *rlc_pdu_ptr
)
{
  /* Message properties */
  word message_id ;
  word message_code ;
  byte gs ;
  byte update_number ;
  byte data_coding_scheme ;
  byte number_of_pages ;
  /*This additional data len for WMS to help them to 
  decode iso_639 standard spec 23.038  section 5 
  "CBS Data Coding Scheme" */
  byte page_header[6 + CB_WMS_ADDITIONAL_DATA_LEN] = {0} ;

  /* 
  ** Pointer to a structure which containing BMC 
  ** messages which are identified with particular
  ** message ID
  */
  cb_message_list_type *new_message_ptr = NULL ;

#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
  cb_current_sfn = received_sfn;
  /*In case if it is last SFN in the DRX cycle then clean the old DRX tables*/
  if ((next_first_sfn_sch_prd != BMC_INVALID_SFN) && 
       cb_bmc_check_sfn_within_sch_prd(received_sfn, next_first_sfn_sch_prd, next_last_sfn_sch_prd))
  {
    cb_bmc_start_new_drx_cycle();
  }
#endif/*FEATURE_UMTS_BMC_DRX_SUPPORTED*/
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  /* 
  ** Start processing incoming RLC PDUs only when incoming PDU is marked 
  ** as a first one or has been received when BMC task predicted to received
  ** next first RLC PDU which is belonging to the new BMC message
  */
  if (cb_bmc_ident_header_pdu(received_sfn,
                      cb_bmc_msg_sfn_allocation_length,
                                                  cb_bmc_msg_sfn_allocation) ||
      (cb_bmc_next_expected_message_sfn == received_sfn) || first_pdu_indicator 
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
       || cb_bmc_validate_first_pdu_drx_mode(received_sfn, rlc_pdu_ptr))
#else
      )
#endif
#endif  //FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  {
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
    if (first_pdu_indicator ||(cb_bmc_next_expected_message_sfn == received_sfn)
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
        || cb_bmc_validate_first_pdu_drx_mode(received_sfn, rlc_pdu_ptr))

#else
     )
#endif
#endif  //FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
    {
      if (((rlc_pdu_ptr[CB_BMC_MESSAGE_TYPE_OFFSET] != CBS_MESSAGE_TYPE) || (pdu_length >= CB_BMC_HEADER_LENGTH)) &&
           ((rlc_pdu_ptr[CB_BMC_MESSAGE_TYPE_OFFSET] != CBS_SCHEDULE_TYPE) || (pdu_length >= CB_BMC_SCHEDULING_MSG_HEADER_LENGTH+1+1)))
      {
        byte i ;
        boolean status;

#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
        if (first_pdu_indicator)
        {
          MSG_HIGH_DS_1(cb_as_id,"BMC: Received first PDU by RLC (SFN %d)",received_sfn) ;
        }
        else if ((cb_bmc_next_expected_message_sfn == received_sfn))
        {
          MSG_HIGH_DS_1(cb_as_id,"BMC: Next message (SFN %d)",received_sfn) ;
        }
#endif

        /* Parse incoming BMC message Header */
        status = cb_bmc_parse_msg_header(rlc_pdu_ptr,&message_type,
                &scheduling_period_length_cb_msg_asm,&message_id,&message_code,&gs,
                         &update_number,&data_coding_scheme,&number_of_pages) ;

        if (status == FALSE)
        {
          MSG_ERROR_DS_0(cb_as_id,"Invalid Fields in CB message header");
          return;
        }

        /* Reset SFN Allocation table */
#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
        memset((void *)cb_bmc_msg_sfn_allocation,0x00,
                      sizeof(cb_bmc_pdu_allocation_type)*CB_BMC_MAX_SDUS_PER_MSG) ;
#else
        memset((void *)cb_bmc_msg_sfn_allocation,0x00,
                      sizeof(cb_bmc_pdu_allocation_type)*
                                        CB_BMC_MAX_MSG_SFN_ALLOCATION_LENGTH) ;
#endif
        /* 
        ** Calculate all occurrences of CTCH which need to be allocated to
        ** transmit incoming BMC message 
        */
        cb_bmc_msg_sfn_allocation_length = 
          cb_bmc_calculate_sfn_msg_allocation(received_sfn,message_type,
                                              &number_of_pages,
                                              scheduling_period_length_cb_msg_asm,
                                              &pdu_length,
                                              li_length,
                                              cb_bmc_msg_sfn_allocation,
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
                                              &cb_bmc_next_expected_message_sfn,
#endif
                                              cb_bmc_pdu_distribution_table) ;
        if (message_type == CBS_MESSAGE_TYPE)
        {
          MSG_HIGH_DS_2(cb_as_id,"BMC: Received a new message (%d,%d)",message_id,message_code) ;
        }

        for (i=0; i<cb_bmc_msg_sfn_allocation_length; i++)
        {
          MSG_HIGH_DS_1(cb_as_id,"BMC: Expected SFN %d",cb_bmc_msg_sfn_allocation[i].sfn) ;
        }

#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
        MSG_HIGH_DS_1(cb_as_id,"BMC: Next Expected message SFN %d",cb_bmc_next_expected_message_sfn) ;
#endif


        switch (message_type)
        {
          case CBS_MESSAGE_TYPE: /* Regular Cell Broadcast message */
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
            /*Update the new msg bit map list in case the message is received*/
            cb_bmc_update_new_msg_bitmap_info(received_sfn, TRUE, TRUE);
#endif

            /* Make sure current messaege is in the search list */
            if (!cb_message_is_requested(message_id))
            {
              MSG_HIGH_DS_1(cb_as_id,"Message %02X is not in the search list",message_id); 
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
              cb_bmc_skip_incoming_message(NULL,
                                           cb_bmc_msg_sfn_allocation_length,
                                           cb_bmc_msg_sfn_allocation,
                                           cb_bmc_skip_allocation) ;                  
              /* 
              ** Stop processing RLC PDU which are carring unwanted BMC message 
              */
              cb_bmc_msg_sfn_allocation_length = 0 ;
#endif

              return ;
            }

            /* The CB task fakes the GSM CB Page header */
            cb_bmc_forge_page_header(
                page_header,message_id,message_code,
                          gs,update_number,data_coding_scheme,
                                                           1,number_of_pages) ;
        
            /* copy additional data of CB_WMS_ADDITIONAL_DATA_LEN for 
               WMS to help them to decode iso_639 standard 
               spec 23.038  section 5 "CBS Data Coding Scheme" */
            if(pdu_length >= CB_BMC_HEADER_LENGTH + CB_WMS_ADDITIONAL_DATA_LEN)
             {
               memscpy((void*)&page_header[6],CB_WMS_ADDITIONAL_DATA_LEN,
                      (void *)&rlc_pdu_ptr[CB_BMC_DATA_BLOCK_OFFSET],
                                           CB_WMS_ADDITIONAL_DATA_LEN);
             }
        
            /* Call WMS to make sure current message could be processed by WMS */

            if (!wms_bc_gw_cb_filter
                (
#ifdef FEATURE_NAS_CBS_DSDS
                 (sys_modem_as_id_e_type)cb_as_id,
#else
                 (sys_modem_as_id_e_type)CB_AS_ID_1,           
#endif
                  page_header
                )
               )
            {
              MSG_HIGH_DS_0(cb_as_id,"CB: WMS filters out current CB page") ;
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
              cb_bmc_skip_incoming_message(NULL,
                                           cb_bmc_msg_sfn_allocation_length,
                                           cb_bmc_msg_sfn_allocation,
                                           cb_bmc_skip_allocation) ;                  
              /* 
              ** Stop processing RLC PDU which are carring unwanted BMC message 
              */
              cb_bmc_msg_sfn_allocation_length = 0 ;
#endif
              return ;
            }

            /* 
            ** Search for incoming message ID in the cache memory
            */
            if ((new_message_ptr = cb_find_message(
                                          basic_cbch_list,message_id)) == NULL)
            {
              MSG_HIGH_DS_1(cb_as_id,"Message ID %d not found",message_id) ;

              /* 
              ** This is a first message ID reception, create new message 
              ** descriptor 
              */
              if ((new_message_ptr = cb_create_message_entry(current_message,
                                             message_id,received_sfn)) == NULL)
              {
                MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;

#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
                cb_bmc_skip_incoming_message(new_message_ptr,
                                             cb_bmc_msg_sfn_allocation_length,
                                             cb_bmc_msg_sfn_allocation,
                                             cb_bmc_skip_allocation) ;                  
                /* 
                ** Stop processing RLC PDU which are carring unwanted BMC message 
                */
                cb_bmc_msg_sfn_allocation_length = 0 ;
#endif
                return ;
              }

              /* 
              ** This is a first message CODE reception, create a new message 
              ** code descriptor 
              */
              if ((new_message_ptr->message_header.current_message_descriptor = 
                      cb_create_message_code_entry(
                              new_message_ptr,message_code,
                                    gs,update_number,number_of_pages, data_coding_scheme)) == NULL)
              {
                MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
                cb_bmc_skip_incoming_message(new_message_ptr,
                                             cb_bmc_msg_sfn_allocation_length,
                                             cb_bmc_msg_sfn_allocation,
                                             cb_bmc_skip_allocation) ;                  
                /* 
                ** Stop processing RLC PDU which are carring unwanted BMC message 
                */
                cb_bmc_msg_sfn_allocation_length = 0 ;
#endif
                return ;
              }

              /* Put new message descriptor into cache memory */
              current_message = cb_put_message_into_list(
                                             basic_cbch_list,new_message_ptr) ;
            }
            else
            {
              /* Search for the current message CODE in the Message 
              ** Code Descriptor list 
              */
              if (cb_find_message_code_with_gs(new_message_ptr,message_code) == NULL)
              {

                MSG_HIGH_DS_1(cb_as_id,"Message code %d not found",message_code) ;

                /* 
                ** This is a first message CODE reception,create new message 
                ** descriptor
                */
                if ((new_message_ptr->message_header.current_message_descriptor = 
                      cb_create_message_code_entry(new_message_ptr,
                        message_code,gs,update_number,number_of_pages, data_coding_scheme)) == NULL)
                {
                  MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory") ;
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
                  cb_bmc_skip_incoming_message(new_message_ptr,
                                               cb_bmc_msg_sfn_allocation_length,
                                               cb_bmc_msg_sfn_allocation,
                                               cb_bmc_skip_allocation) ;                  
                  /* 
                  ** Stop processing RLC PDU which are carring unwanted BMC message 
                  */
                  cb_bmc_msg_sfn_allocation_length = 0 ;
#endif
                  return ;
                }

                /* 
                ** Update SFN number that the message transmission has been
                ** started from
                */
                new_message_ptr->message_header.fn = received_sfn ;

                /* MSG_HIGH_DS_0(cb_as_id,"CB: Received Base FN is %d",received_fn) ; */
              }
              else
              {
                /* Run Duplication Detector */
                if (cb_duplication_detector(CB_UMTS_MODE,
                        &new_message_ptr->message_header,
                                           gs,update_number,1,number_of_pages))
                {
                  MSG_HIGH_DS_2(cb_as_id,
                    "CB: Duplication is detected, Msg. ID %02X, Code %02X",
                          new_message_ptr->message_header.message_id,
                                                              message_code) ;


#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
                  cb_bmc_skip_incoming_message(new_message_ptr,
                                               cb_bmc_msg_sfn_allocation_length,
                                               cb_bmc_msg_sfn_allocation,
                                               cb_bmc_skip_allocation) ;

                  /* 
                  ** Stop processing RLC PDU which are carring unwanted BMC message 
                  */
                  cb_bmc_msg_sfn_allocation_length = 0 ;
#endif
                  if (gs == 0x00)
                  {
                    /* 
                    ** Inform WMS that CELL-WIDE-IMMEDIATE-DISPLAY
                    ** has been duplicated
                    */
                    cb_send_wms_duplication_ind(message_id,
                            message_code,gs,update_number,1,
                                          number_of_pages,data_coding_scheme) ;
                  }

                  return ;
                }
                else 
                {
                  /* 
                  ** Dispite the same message ID and message code
                  ** this is a new message transmission because GS
                  ** or Update number has been changed
                  */

                  /* Update Message/Code descriptor entry */
                  cb_user_page_info_type *current_page_info = 
                          new_message_ptr->
                                    message_header.current_message_descriptor ;

                  /* Refresh base SFN */
                  new_message_ptr->message_header.fn = received_sfn ;

                  if ((cb_nv_duplication_detection_disabled) ||
                      (current_page_info->gs_indicator != gs) || 
                      (current_page_info->update_number != update_number))
                  {
                    /* Refresh base message properties */
                    current_page_info->gs_indicator = gs ;

                    current_page_info->update_number = update_number ;

                    current_page_info->number_of_pages = number_of_pages ;

                    current_page_info->data_coding_scheme = data_coding_scheme;

                    /* 
                    ** Reset received page indicator because entire message
                    ** is going to be read again
                    */
                    current_page_info->received_page_indicator = 0 ;

                    /* 
                    ** If we still have incomplete old message pages
                    ** delete them
                    */
                    if (current_page_info->missing_page_list != NULL)
                    {
                      do
                      {
                        current_page_info->missing_page_list= 
                            cb_delete_page(new_message_ptr,
                                        current_page_info->missing_page_list) ;
                      }
                      while (current_page_info->missing_page_list != NULL) ;
                    }
                  }
                  else 
                  {
                    /* 
                    ** Calculate how needs to be looked like received paging 
                    ** block indicator for the current page and current RLC
                    ** PDU size
                    */
                    word page_mask = cb_bmc_get_mask(number_of_pages) ;

                    if ((current_page_info->gs_indicator == gs) && 
                        (current_page_info->update_number == update_number) &&
                        ((current_page_info->received_page_indicator & 
                                                       page_mask) != page_mask))

                    {
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT // message incomplete
                      /* 
                      ** If this is not a first reception of the message and we
                      ** already managed to receive fragments of the current 
                      ** message in the past, skip duplicated RLC PDU
                      */
                      cb_bmc_filter_out_received_pdus(new_message_ptr,
                                                      number_of_pages,
                                                      cb_bmc_msg_sfn_allocation_length,
                                                      cb_bmc_msg_sfn_allocation,
                                                      cb_bmc_pdu_distribution_table,
                                                      cb_bmc_skip_allocation) ;
#endif
                    }
                  }
                }
              }
            }


            /* Process incoming RLC PDU */
            cb_bmc_process_incoming_data(cb_bmc_msg_sfn_allocation_length,
                                         cb_bmc_msg_sfn_allocation,
                                         received_sfn,
                                         cb_bmc_pdu_distribution_table,
                                         pdu_length,
                                         rlc_pdu_ptr,&new_message_ptr) ;

            /* Update cache memory pointers */
            current_message = new_message_ptr ;
            basic_cbch_list = new_message_ptr ;

            
            break ;

          case CBS_SCHEDULE_TYPE: /* Scheduling Cell Broadcast message */
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
            
           /*Update the new msg bit map list in case the message is received*/
           cb_bmc_update_new_msg_bitmap_info(received_sfn, TRUE, TRUE);

           if(cb_drx_info_validation_required==TRUE)
            { 
              cb_bmc_skip_unscheduled_empty_slots(received_end_sfn);
            }
            /* 
            ** Reset Scheduling message reconstruction state vars
            */ 

            /* Reset scheduling message length */
            cb_bmc_scheduling_msg_length = 0 ;

            /* Reset scheduling message IE state */
            cb_bmc_reassembling_state = CB_BMC_BIT_MAP_STATE ; 

            /* Reset scheduling message remaining length */
            cb_bmc_remaining_length = 0 ;

            /* Reset scheduling message descriptor number */
            cb_bmc_number_of_descriptions = 0 ;

            /* 
            ** Copy Scheduling message into single memory buffer
            */
            if (cb_bmc_preprocess_scheduling_message(
                            cb_bmc_msg_sfn_allocation_length,
                            cb_bmc_msg_sfn_allocation,
                            received_sfn,
                            pdu_length, rlc_pdu_ptr,
                            scheduling_period_length_cb_msg_asm,
                            &cb_bmc_scheduling_msg_length,
                            cb_bmc_scheduling_msg,
                            &cb_bmc_reassembling_state,
                            &cb_bmc_remaining_length,
                            &cb_bmc_number_of_descriptions))
            {
              /* 
              ** Sceduling message reconstruction is done, froward it for
              ** further processing
              */
              cb_send_ota_msg_log_packet((uint8)CB_UMTS_MODE,
             (uint32)cb_bmc_scheduling_msg_length,(uint8)CB_SCHEDULING_PAGE,
                                              cb_bmc_scheduling_msg);
              cb_bmc_process_scheduling_message(cb_bmc_scheduling_msg,
                                                                received_sfn
#ifdef FEATURE_NAS_REL6
                                                , cb_bmc_scheduling_msg_length
#endif
                                                                ) ;

              /* Stop processing remaining RLC PDUs */
              cb_bmc_msg_sfn_allocation_length = 0 ;
            }
#else
            MSG_HIGH_DS_0(cb_as_id,"CB: Scheduling is not supported") ;
#endif /* #ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED */
            
            break ; 

          case CBS_41_MESSAGE_TYPE: /* IS-41 Cell Broadcast message */
          default:
            MSG_HIGH_DS_1(cb_as_id,"Message type %02X is not supported",message_type) ;

            return ;
        }

      }
      else
      {
        MSG_HIGH_DS_0(cb_as_id,"CB: Message too short, PDU discarded") ;

        return ; /* skip incoming RLC PDU */
      }
    }
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
    else
    {
      MSG_HIGH_DS_0(cb_as_id,"CB: Unrecognizable PDU format, PDU discarded") ;

      return ; /* skip incoming RLC PDU */
    }
#endif //FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  }
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  else
  {
    if (cb_bmc_get_sfn_allocation_index(
              message_type,cb_bmc_msg_sfn_allocation_length,
                             cb_bmc_msg_sfn_allocation,received_sfn) != 0xFFFF)
    {
      switch (message_type)
      {
        case CBS_MESSAGE_TYPE :  /* Regular Cell Broadcast message */
        
        /*
        ** This processing is required only in case if variable RLC
        ** PDU sizes can be used.
        ** 
        **
          if (cb_bmc_rlc_pdu_size>pdu_length)
          {
            cb_bmc_msg_sfn_allocation_length = 
              cb_bmc_recalculate_sfn_msg_allocation(received_sfn,
                pdu_length,cb_bmc_msg_sfn_allocation_length,
                  cb_bmc_msg_sfn_allocation,
                    &cb_bmc_next_expected_message_sfn,
                                    cb_bmc_pdu_distribution_table) ;
          }
        */
        if (current_message != NULL)
        {
          /* Process incoming RLC PDU */
          cb_bmc_process_incoming_data(cb_bmc_msg_sfn_allocation_length,
                                       cb_bmc_msg_sfn_allocation, 
                                       received_sfn,
                                       cb_bmc_pdu_distribution_table,
                                       pdu_length,
                                       rlc_pdu_ptr,current_message) ;

#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
        /*Update the new msg bit map list in case the message is received*/
          cb_bmc_update_new_msg_bitmap_info(received_sfn, FALSE, TRUE);       
#endif /*FEATURE_UMTS_BMC_DRX_SUPPORTED*/
        }
        else
        {
            MSG_HIGH_DS_0(cb_as_id,"BMC: Reset SFN table") ;
            cb_bmc_msg_sfn_allocation_length = 0; 
        }

        break ;
        case CBS_SCHEDULE_TYPE : /* Sceduling Cell Broadcast message */
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
          /* 
          ** Copy Scheduling message into single memory buffer
          */
          if (cb_bmc_preprocess_scheduling_message(
                            cb_bmc_msg_sfn_allocation_length,
                            cb_bmc_msg_sfn_allocation,
                            received_sfn,
                            pdu_length, rlc_pdu_ptr,
                            scheduling_period_length_cb_msg_asm,
                            &cb_bmc_scheduling_msg_length,
                            cb_bmc_scheduling_msg,
                            &cb_bmc_reassembling_state,
                            &cb_bmc_remaining_length,
                            &cb_bmc_number_of_descriptions))
          {
             /* 
             ** Sceduling message reconstruction is done, froward it for
             ** further processing
             */
             cb_bmc_process_scheduling_message(cb_bmc_scheduling_msg,
                                   (dword)(cb_bmc_msg_sfn_allocation[0].sfn & 0xFFFFF)
#ifdef FEATURE_NAS_REL6
                , cb_bmc_scheduling_msg_length
#endif
               ) ;

             /* Stop processing remaining RLC PDUs */
             cb_bmc_msg_sfn_allocation_length = 0 ;
          }
#else
          MSG_HIGH_DS_0(cb_as_id,"CB: Scheduling is not supported") ;
#endif
          break ;
        case CBS_41_MESSAGE_TYPE: /* IS-41 Cell Broadcast message */
        default:
          MSG_HIGH_DS_1(cb_as_id,"Message type %02X is not supported",message_type) ;

          return ;
      }
    }
    else
    {
      MSG_HIGH_DS_1(cb_as_id,"BMC: Received unexpected RLC PDU (%d)",received_sfn) ;
    }
  }
#endif //FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
  /*In case if it is last SFN in the DRX cycle then clean the old DRX tables*/
  if ((current_last_sfn_sch_prd != BMC_INVALID_SFN) && (received_sfn == current_last_sfn_sch_prd))
  {
    cb_bmc_end_current_drx_cycle();
  }
#endif /*FEATURE_UMTS_BMC_DRX_SUPPORTED*/
} /* end cb_bmc_process_rlc_pdu() */
/*===========================================================================

FUNCTION    BMC_PARSE_RLC_PARAMS

DESCRIPTION
  This function parses the RLC params from the app_ptr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean bmc_parse_rlc_params
(
  dsm_item_type *msg_ptr,
  boolean *start_pdu_ptr,
  byte *no_li_len_ptr,
  dword *received_sfn_ptr,
  dword *received_end_sfn_ptr
)
{
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  if (msg_ptr->app_ptr)
#endif
  {
    *received_sfn_ptr = (dword)((uint32)msg_ptr->app_ptr & 0x0000FFFFF);
    *no_li_len_ptr   = (byte)(((uint32)msg_ptr->app_ptr & 0x60000000) >> 29);
    *start_pdu_ptr   =  (boolean)((((uint32)msg_ptr->app_ptr & 0x80000000) >> 31));
    if(cb_drx_info_validation_required==TRUE)
    { 
    *received_end_sfn_ptr = (dword)((uint32)msg_ptr->app_field & 0x0000FFFFF);
    }
    else
    {
    *received_end_sfn_ptr =0;
    }
    return TRUE;
  }
#ifndef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  else
  {
     MSG_ERROR_DS_0(cb_as_id,"NULL RLC params");
  }
  return FALSE;
#endif
}

#endif/*FEATURE_UMTS_BMC*/

#endif /* #ifdef FEATURE_GSM_CB */
