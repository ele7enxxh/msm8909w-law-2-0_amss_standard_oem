#ifndef CB_MSG_LIB_H
#define CB_MSG_LIB_H
/*===========================================================================

                          CB MESSAGE LIB HEADER FILE

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cbmsg.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/07   jbk     BMC RLC Multiple PDU Support under FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
10/19/06   kvk     Added new member cb_curr_proces_msg_des to store
                   the message decriptor for current constructing message
06/20/04   kvk     Fixed the lenght of the buffer to store the scheduling message
                   which is causing the buffer over run and corrupting the nearby 
                   global variables.
05/16/05   kvk     New data structure defined to store the history of the 
                   DRX schedule period.
07/14/04   vdr     Added prototype for cb_asm_state_handler() 
05/19/04   vdr     Fixed duplication detection
04/16/04   vdr     Featurizing out CB DRX mode processing
                   Added some extra comments
06/24/03   vdr     Initial version


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
//#include "customer.h"

#include "mmcp_variation.h"
#ifdef FEATURE_GSM_CB

#include "comdef.h"
#include "cbl1if.h"
#include "cb_task.h"
#include "cbtask.h"

/* CB scheduling period is measured in FNs */
#define MAX_SCHEDULING_PERIOD_LENGTH 49

/* Block Type position within received paging block */
#define CB_PAGING_BLOCK_TYPE 0

/* Protocol Discriminator mask */
#define CB_LPD_MASK 0x60

/* Link Protocol Discriminator */
#define LINK_PD 0x01

#define CB_PAGE_HEADER_LENGTH 0x06

#define LAST_PAGING_BLOCK_MASK 0x10

#define CB_PAGING_BLOCK_SEQUENCE_NUMBER_MASK 0x0F

#define CB_GS_POS  0x01
#define CB_GS_MASK 0xC0

#define CB_MESSAGE_CODE_HIGH_POS  0x01
#define CB_MESSAGE_CODE_HIGH_MASK 0x3F
#define CB_MESSAGE_CODE_LOW_POS   0x02
#define CB_MESSAGE_CODE_LOW_MASK  0xF0

#define CB_UPDATE_NUMBER_POS  0x02
#define CB_UPDATE_NUMBER_MASK 0x0F

#define CB_MESSAGE_ID_OFFSET 0x03

#define CB_DATA_CODING_SCHEME 0x05

#define CB_PAGE_PARAMETER_POS   0x06
#define CB_PAGE_NUMBER_MASK     0xF0
#define CB_NUMBER_OF_PAGES_MASK 0x0F

#define CB_PAGING_BLOCK_OFFSET       1
#define CB_PAGING_BLOCK_DATA_LENGTH  22

#define CB_PAGING_MAX_BLOCKS 4

#define CB_MAX_NUMBER_OF_PAGES 15

#define CB_FIRST_PAGING_BLOCK           0x00
#define CB_SECOND_PAGING_BLOCK          0x01
#define CB_THIRD_PAGING_BLOCK           0x02
#define CB_FOURTH_PAGING_BLOCK          0x03
#define CB_FIRST_SCEDULING_PAGING_BLOCK 0x08
#define CB_NULL_MESSAGE_PAGING_BLOCK    0x0F

#define CB_FIRST_BLOCK           0x00
#define CB_SECOND_BLOCK          0x01
#define CB_THIRD_BLOCK           0x02
#define CB_FOURTH_BLOCK          0x03
#define CB_FIFTH_BLOCK           0x04
#define CB_SIXTH_BLOCK           0x05
#define CB_SEVENTH_BLOCK         0x06
#define CB_EIGHTH_BLOCK          0x07

#define CB_RECEIVED_FIRST_BLOCK_MASK     0x01
#define CB_RECEIVED_SECOND_BLOCK_MASK    0x02
#define CB_RECEIVED_THIRD_BLOCK_MASK     0x04
#define CB_RECEIVED_FOURTH_BLOCK_MASK    0x08

#define CB_SKIPED_SECOND_BLOCK_MASK    0x02
#define CB_SKIPED_THIRD_BLOCK_MASK     0x04
#define CB_SKIPED_FOURTH_BLOCK_MASK    0x08

#define CB_SKIP_SECOND_THIRD_FOURTH    0x0E /* 1110 */
#define CB_SKIP_THIRD_FOURTH           0x0C /* 1100 */
#define CB_SKIP_FOURTH                 0x08 /* 1000 */
#define CB_SKIP_SECOND_FOURTH          0x0A /* 1010 */
#define CB_SKIP_SECOND                 0x02 /* 0010 */
#define CB_SKIP_THIRD                  0x04 /* 0100 */
#define CB_SKIP_SECOND_THIRD           0x06 /* 0110 */
#define CB_NOTHING_TO_SKIP             0x00 /* 0000 */


#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)
#define CB_TIMER_ID_SIM_POS 6
#define CB_TIMER_TAG_AS_ID(CB_TIMER) (timer_id_T)(CB_TIMER | ((cb_as_id & 0x11) << (CB_TIMER_ID_SIM_POS)))
#else
#define CB_TIMER_TAG_AS_ID(CB_TIMER)  (CB_TIMER)
#endif



#ifndef FEATURE_GSM_CB_TEST

#define CB_CALCULATE_PAGING_SLOT(x) ((x/51)%8)

#else

extern byte cb_get_paging_slot( void ) ;

#define CB_CALCULATE_PAGING_SLOT(x) (cb_get_paging_slot())

#endif

typedef enum cb_message_geo_scop_tag
{
  CB_HIGH_PRIORITY_CELL_WIDE,
  CB_PLMN_WIDE,
  CB_LOCATION_AREA_WIDE,
  CB_LOW_PRIORITY_CELL_WIDE
} cb_message_geo_scop_type ;

typedef enum cb_missing_paging_block_tag
{
  CB_MISSING_EVERYTHING          = 0  , /* 0000 */
  CB_MISSING_FOURTH_THIRD_SECOND = 1  , /* 0001 */
  CB_MISSING_FOURTH_THIRD        = 3  , /* 0011 */
  CB_MISSING_FOURTH_SECOND       = 5  , /* 0101 */
  CB_MISSING_FOURTH              = 7  , /* 0111 */
  CB_MISSING_THIRD_SECOND        = 9  , /* 1001 */
  CB_MISSING_THIRD               = 11 , /* 1011 */
  CB_MISSING_SECOND              = 13 , /* 1101 */
  CB_MISSING_NOTHING             = 15   /* 1111 */
} cb_missing_paging_block_type ;

typedef enum cb_page_assembling_state_tag
{
  CB_RECEIVING_FIRST_PB_STATE,
  CB_RECEIVING_SECOND_PB_STATE,
  CB_RECEIVING_THIRD_PB_STATE,
  CB_RECEIVING_FOURTH_PB_STATE
} cb_page_assembling_state_type ;

typedef enum cb_message_type_tag
{
  CB_NORMAL_PAGE,
  CB_SCHEDULING_PAGE
} cb_message_type ;


typedef struct cb_bmc_gsm_propertie_tag
{
  byte missing_paging_blocks ;

  byte already_skiped_blocks ;
} cb_bmc_gsm_propertie_type ;

typedef struct cb_missing_page_tag
{ 
  byte page_number ;
  
  cb_rat_type received_rat ;

  union 
  {
    cb_bmc_gsm_propertie_type   gsm_propertie ;
    word                        umts_received_paging_block ;
  } rat_page_properties ;

  byte page_length ;
  
  byte page_buffer[88] ;

  struct cb_missing_page_tag *next_missing_page ;
} cb_missing_page_type ;

typedef struct cb_retx_schedule_tag
{
  /*byte number_of_retx_left ;*/
  dword  retx_fn ;
  struct cb_retx_schedule_tag *next_ptr;
  struct cb_retx_schedule_tag *prev_ptr;
} cb_retx_schedule_type ;

typedef struct cb_first_tx_schedule_tag
{
  dword tx_fn ;
  struct cb_first_tx_schedule_tag *next_ptr;
  struct cb_first_tx_schedule_tag *prev_ptr;
  cb_retx_schedule_type *retx_schedule ;
#ifdef FEATURE_NAS_REL6
  dword   serial_number;
#endif
} cb_first_tx_schedule_type; 

typedef struct cb_bmc_new_msg_bit_map_info
{
  dword sfn;
  word msg_id;
  byte recd_flag;
  byte start_pdu;
#ifdef FEATURE_NAS_REL6
  dword serial_number;
#endif
  struct cb_bmc_new_msg_bit_map_info *next_ptr;
}cb_bmc_new_msg_bit_map_info_t;

typedef struct cb_user_page_info_tag
{
  word                   message_code ;

  byte                   gs_indicator ;

  byte                   update_number ;

  byte                   number_of_pages ;

  byte                   data_coding_scheme ;

  word                   received_page_indicator ;

  byte                   plmn[3] ;

  byte                   lac[2] ;

  byte                   cell_id[4] ;

  cb_missing_page_type  *current_page ;

  cb_missing_page_type  *missing_page_list ;
  
  dword                  timer_offset;

  struct cb_user_page_info_tag *next_message_code ;
} cb_user_page_info_type ;

/* 
** This structure keeps an information required to reassemble 
** CB message and to deliver it to WMS along with the message
** properties as Message Code and Delivery Indicator
*/
typedef struct cb_message_header_tag
{
  word                   message_id ;

  cb_user_page_info_type *message_info ;

  cb_user_page_info_type *current_message_descriptor ;

  /*GSM current constructing message descriptor*/
  cb_user_page_info_type *cb_curr_proces_msg_des;


  dword                  fn ;

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)
  cb_first_tx_schedule_type *next_first_tx_schedule ;

  cb_first_tx_schedule_type *first_tx_schedule ;
  
  cb_retx_schedule_type *retx_schedule ;

#endif /* DRX */

} cb_message_header_type ;

/*
** The following structure contains an element of a CB message 
** linked list. This list is created upon reception of a 
** WMS cb_search_list_req and is destroied if WMS indicates that
** CB messages is no longer expected
*/
typedef struct cb_message_list_tag
{
  /* Current CB message */
  cb_message_header_type message_header ;

  /* Pointer to the next message in the list */
  struct cb_message_list_tag *next_message ;

  /* Pointer to the next message in the list */
  struct cb_message_list_tag *prev_message ;

} cb_message_list_type ;

#if defined(FEATURE_WCDMA_ETWS) && defined(FEATURE_UMTS_BMC)
typedef struct cb_bmc_etws_message_info_tag
{
  word                   message_code ;

  byte                   gs_indicator ;

  byte                   update_number ;

  byte                   plmn[3] ;

  dword                  timer_offset ;

  struct cb_bmc_etws_message_info_tag *next_message_code ;
} cb_bmc_etws_msg_info_type ;

/* 
** This structure keeps an information required to reassemble 
** CB message and to deliver it to WMS along with the message
** properties as Message Code and Delivery Indicator
*/
typedef struct cb_bmc_etws_msg_header_tag
{
  word                   message_id ;

  cb_bmc_etws_msg_info_type *message_info ;

} cb_bmc_etws_msg_header_type ;

typedef struct cb_bmc_etws_msg_list_tag
{
  /* Current CB message */
  cb_bmc_etws_msg_header_type  message_header ;

  /* Pointer to the next message in the list */
  struct cb_bmc_etws_msg_list_tag *next_message ;

} cb_bmc_etws_msg_list_type ;

#endif

typedef struct cb_msg_entry_tag
{
  cb_message_list_type *message_entry ;
  cb_message_list_type **cb_message_list ;
  cb_message_list_type **cb_first_list_entry ;
} cb_msg_entry_type ;

#define CBS_MESSAGE_TYPE     1
#define CBS_SCHEDULE_TYPE    2
#define CBS_41_MESSAGE_TYPE  3
#define CBS_NO_MESSAGE_TYPE  4

/*
One timer for CB Duplication Detection and other for ETWS CTCH reception
*/

#define CB_MAX_TIMERS 2

/*
 ** definitions of timer states
 */
enum
{
   TIMER_STOPPED,
   TIMER_PAUSED,
   TIMER_ACTIVE
};

#ifdef FEATURE_UMTS_BMC
#ifdef FEATURE_WCDMA_ETWS

/*
**Spec 23.041 section 9.3.24 Warning-Type
**Spec defined warning type info to support ETWS message reception
*/
#define CB_BMC_ETWS_WARNING_TYPE_MASK             0xFE
#define CB_BMC_ETWS_EARTQUAKE_WARNING             0x00
#define CB_BMC_ETWS_TSUNAMI_WARNING               0x01
#define CB_BMC_ETWS_EARTQUAKE_AND_TSUNMAI_WARNING 0x02
#define CB_BMC_ETWS_TEST_WARNING                  0x03
#define CB_BMC_ETWS_OTHER_WARNING                 0x04
/*
**Spec 23.041 section 9.4.1.2.2 Message identifier
**Spec defined message IDs to support ETWS message reception
*/
#define CB_BMC_ETWS_EARTQUAKE_ID             0x1100
#define CB_BMC_ETWS_TSUNAMI_ID               0x1101
#define CB_BMC_ETWS_EARTQUAKE_AND_TSUNAMI_ID 0x1102
#define CB_BMC_ETWS_TEST_ID                  0x1103
#define CB_BMC_ETWS_OTHER_EMERGENCY_ID       0x1104
#define CB_BMC_ETWS_FUTURE_EXTENSION_01      0x1105
#define CB_BMC_ETWS_FUTURE_EXTENSION_02      0x1106
#define CB_BMC_ETWS_FUTURE_EXTENSION_03      0x1107


extern cb_bmc_etws_msg_list_type *basic_etws_list ;

extern cb_bmc_etws_msg_list_type *basic_etws_list_entry_point ;

#endif/*FEATURE_WCDMA_ETWS*/

#define CB_BMC_MESSAGE_TYPE_OFFSET 0

#define CB_BMC_MESSAGE_ID_OFFSET       1
#define CB_BMC_SERIAL_NUMBER_OFFSET    3
#define CB_BMC_DATA_CODING_OFFSET      5
#define CB_BMC_NUMBER_OF_PAGES_OFFSET  6
#define CB_BMC_DATA_BLOCK_OFFSET    CB_BMC_NUMBER_OF_PAGES_OFFSET + 1
/* This additional data len for WMS to help them to decode iso_639 standard 
spec 23.038  section 5 "CBS Data Coding Scheme" */
#define CB_WMS_ADDITIONAL_DATA_LEN  2 

#define CB_BMC_GS_MASK 0xC000
#define CB_BMC_MESSAGE_CODE_HIGH_MASK 0x3F00
#define CB_BMC_MESSAGE_CODE_LOW_MASK  0x00F0
#define CB_BMC_UPDATE_NUMBER_MASK     0x000F

#define CB_BMC_HEADER_LENGTH 7 /* BMC Message Header Length */

#define CB_BMC_PAGE_LENGTH 83
#define CB_BMC_DATA_PAGE_LENGTH 82

/* BMC Scheduling message length */
#define CB_BMC_SCHEDULING_MSG_HEADER_LENGTH 3

#define CB_BMC_SCHEDULING_PERIOD_LENGTH_OFFSET 2

/*
**From 23.341 section 11.12, 11.13 and 11.14
**In REL6 future extention bitmap is added in the end and considering the max possible
**length of serial number list, maximum length of scheduling message 
**is 800+2+3*255 = 1567
*/
#define CB_BMC_MAX_SCHEDULING_LENGTH 1567

#define CB_BMC_MAX_DATA_MESSAGE_LENGTH (CB_BMC_PAGE_LENGTH*CB_MAX_NUMBER_OF_PAGES + CB_BMC_HEADER_LENGTH)
/*
**As Maximum Length of RLC message is equal to length of BMC scheduling message MAX Length
**In REL6 enabled targets and it is equal Maximum data length in targets prior to REL6
**So computing MAX RLC data length as maximum of maximum possible scheduling message length and
**maximum possible data length
*/
#define CB_BMC_MAX_RLC_DATA_LENGTH MAX(CB_BMC_MAX_SCHEDULING_LENGTH,CB_BMC_MAX_DATA_MESSAGE_LENGTH)

#define CB_BMC_MAX_MSG_SFN_ALLOCATION_LENGTH 256

#define CB_BMC_PAGE_LENGTH_IND_MASK 0x80
#define CB_BMC_PAGING_BLOCK_NUMBER_MASK 0x7F
#define CB_BMC_PAGE_HEADER_LENGTH 6

#define CB_BMC_MAX_SFN 4095

/* To increase the SFN limit of BMC-WL1 interface 
** Max SFN cycles supported; Each cycle has CB_BMC_MAX_SFN number of SFNs 
** after which cycle is incremented and SFN is rolled over */
#define CB_BMC_MAX_ROLLOVER_SFN_CYCLE 255

/* Max SFNs supported till SFN cycle is rolled over.
** BMC-WL1 interface will be able to communicate till CB_BMC_MAX_ROLLOVER_SFN
** number of SFNs now*/
#define CB_BMC_MAX_ROLLOVER_SFN (((CB_BMC_MAX_SFN+1) * (CB_BMC_MAX_ROLLOVER_SFN_CYCLE+1))-1)

typedef enum
{
  CB_BMC_BIT_MAP_STATE,
  CB_BMC_DESCRIPTION_STATE
} cb_bmc_reassembling_state_type ;

typedef enum
{
  CB_BMC_NEW_MESSAGE_REPETITION_TYPE,
  CB_BMC_NEW_MESSAGE_TYPE,
  CB_BMC_READING_ADVISED_TYPE,
  CB_BMC_READING_OPTIONAL_TYPE,
  CB_BMC_OLD_MESSAGE_REPETITION_TYPE,
  CB_BMC_OLD_MESSAGE_TYPE,
  CB_BMC_SCHEDULE_MESSAGE_TYPE,
  CB_BMC_CBS41_MESSAGE_TYPE,
  CB_BMC_NO_MESSAGE_TYPE
} cb_bmc_description_field_type ;

/* 
** The structure describes how particular RLC PDU
** is going to be used to construct BMC page(s)
*/
typedef struct cb_bmc_sfn_page_descriptor_tag
{
  /* 
  ** Page number that particular PDU reconstructs 
  */
  byte page_number ;

  /* 
  ** Offset is used to copy message fragment from 
  ** incoming RLC PDU
  */
#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  word pdu_data_offset ;
#else
  byte pdu_data_offset ;
#endif

  /* 
  ** Offset is used to copy message fragment into
  ** current BMC message page buffer
  */
  byte page_data_offset ;

  /* Number of offset is to be copied */
  byte data_length ;

  /* 
  ** The page_properties structure member has two information
  ** fields:
  ** Page length indicator (L) and paging block number (N)
  ** +-------------------------------+
  ** ! L ! N ! N ! N ! N ! N ! N ! N !
  ** +-------------------------------+
  ** where N is an integer number and L is a boolean flag.
  ** If L is equal to 1 RLC PDU has a page length which is 
  ** stored by (RLC PDU pointer+pdu_data_offset+data_length+1) 
  ** offset
  */
  byte page_properties ; 
} cb_bmc_sfn_page_descriptor_type ;

typedef struct cb_bmc_pdu_allocation_tag
{
  dword sfn         ;
  byte number_of_pages ;
#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  cb_bmc_sfn_page_descriptor_type sfn_page_descriptor[CB_MAX_NUMBER_OF_PAGES] ;
#else
  cb_bmc_sfn_page_descriptor_type sfn_page_descriptor[2] ;
#endif
} cb_bmc_pdu_allocation_type ;


#endif /* #ifdef FEATURE_UMTS_BMC */

#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)

extern cb_message_list_type *basic_cbch_list_sim[MAX_AS_IDS] ;

extern cb_message_list_type *basic_cbch_list_entry_point_sim[MAX_AS_IDS] ;

extern cb_message_list_type *current_message_sim[MAX_AS_IDS] ;

extern cb_page_assembling_state_type page_assembling_state_sim[MAX_AS_IDS] ;

extern byte     cb_bmc_rlc_pdu_size_sim[MAX_AS_IDS];

extern cb_rat_type bmc_active_rat_sim[MAX_AS_IDS];

extern byte     cb_bmc_rlc_pdu_size_sim[MAX_AS_IDS];

extern cb_rat_type bmc_active_rat_sim[MAX_AS_IDS];

extern dword cb_next_min_offset_sim[MAX_AS_IDS];

extern byte cb_timer_status_sim[MAX_AS_IDS][CB_MAX_TIMERS];


#define basic_cbch_list   basic_cbch_list_sim[cb_as_id]
#define current_message  current_message_sim[cb_as_id]
#define basic_cbch_list_entry_point  basic_cbch_list_entry_point_sim[cb_as_id]
#define page_assembling_state page_assembling_state_sim[cb_as_id]
#define  cb_bmc_rlc_pdu_size cb_bmc_rlc_pdu_size_sim[cb_as_id]
#define   bmc_active_rat bmc_active_rat_sim[cb_as_id]
#define  cb_bmc_rlc_pdu_size cb_bmc_rlc_pdu_size_sim[cb_as_id]
#define cb_next_min_offset   cb_next_min_offset_sim[cb_as_id]
#define cb_timer_status  cb_timer_status_sim[cb_as_id]

#else
extern cb_message_list_type *basic_cbch_list ;

extern cb_message_list_type *basic_cbch_list_entry_point ;

extern cb_message_list_type *current_message ;

extern cb_page_assembling_state_type page_assembling_state ;

extern dword cb_next_min_offset;

extern byte cb_timer_status[CB_MAX_TIMERS];

#endif //#ifdef FEATURE_NAS_CBS_DSDS
/* number of radio frame per TTI */
extern byte cb_bmc_ctch_rf_per_tti;

/* Period of CTCH allocation ( N ) */
extern word cb_bmc_ctch_allocation_period;

/* CBS frame offset ( K ) */
extern byte cb_bmc_cbs_frame_offset;

/* Current PDU allocation table length */
extern word cb_bmc_msg_sfn_allocation_length  ;


/*===========================================================================

FUNCTION    CB_VALIDATE_PAGING_BLOCK

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cb_validate_paging_block
(
  byte   *paging_block,
  dword   received_fn
) ;

/*===========================================================================

FUNCTION    CB_PROCESS_PAGING_BLOCK

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_process_paging_block
(
  byte  *paging_block,
  dword  received_fn
) ;

/*===========================================================================

FUNCTION    CB_ASM_STATE_HANDLER

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
extern void cb_asm_state_handler
(
  dword missing_fn
) ;


#endif /* #ifdef FEATURE_GSM_CB */

#endif /* #ifndef CB_MSG_LIB_H */
