#ifndef CB_SCHEDULE_H
#define CB_SCHEDULE_H
/*===========================================================================

               CB TASK HEADER FILE (CB_TASK.H)

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cb_schedule.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/06   kvk     Moved cb_bmc_rlc_pdu_size outside the FEATURE_UMTS_BMC
05/16/05   kvk     Added enum for OLD message. Changed data structures which
                   store the first transmission and retransmissions. New data structures
                   defined to handle DRX optimization.
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
#include "nas_exp.h"
#include "cbwmsif.h"
#include "cbmsg.h"


#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)

/* 
** Boolean flag indicating whether BMC task is going to 
** process "Reading Advised" SFNs of the BMC Scheduling Message 
*/
extern boolean  cb_bmc_reading_advised_sim[MAX_AS_IDS];

/* 
** Boolean flag indicating whether BMC task is going to 
** process "Reading Optional" SFNs of the BMC Scheduling Message 
*/
extern boolean  cb_bmc_reading_optional_sim[MAX_AS_IDS];

extern byte     cb_bmc_rlc_pdu_size_sim[MAX_AS_IDS];


#define  cb_bmc_reading_advised cb_bmc_reading_advised_sim[cb_as_id]
#define  cb_bmc_reading_optional cb_bmc_reading_optional_sim[cb_as_id]
#define  cb_bmc_rlc_pdu_size cb_bmc_rlc_pdu_size_sim[cb_as_id]

#else

/* 
** Boolean flag indicating whether BMC task is going to 
** process "Reading Advised" SFNs of the BMC Scheduling Message 
*/
extern boolean cb_bmc_reading_advised ;

/* 
** Boolean flag indicating whether BMC task is going to 
** process "Reading Optional" SFNs of the BMC Scheduling Message 
*/
extern boolean cb_bmc_reading_optional ;

extern byte     cb_bmc_rlc_pdu_size;
#endif //#ifdef FEATURE_NAS_CBS_DSDS

#if defined (FEATURE_UMTS_BMC) || defined (FEATURE_GSM_CB_DRX_SUPPORTED)

/* number of radio frame per TTI */
extern byte cb_bmc_ctch_rf_per_tti ;

/* Period of CTCH allocation ( N ) */
extern word cb_bmc_ctch_allocation_period ;

/* CBS frame offset ( K ) */
extern byte cb_bmc_cbs_frame_offset ;

#endif //#if defined (FEATURE_UMTS_BMC) || defined (FEATURE_GSM_CB_DRX_SUPPORTED)

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*GSM CB macros*/
#define CB_SCHEDULING_MESSAGE_TYPE_MASK   0xC0

#define CB_GSM_SCEDULING_MESSAGE_TYPE     0x00

#define CB_BEGIN_END_SCHEDULING_SLOT_MASK 0x3F  

#define CB_NEW_MSG_BITMAP_OFFSET          0x02

#define CB_NEW_MSG_BITMAP_LENGTH          0x06

#define CB_NEW_MSG_DESCRIPTION_OFFSET     0x08

#define CB_SCHEDULING_MESSAGE_PADDING_BYTE  0x2B

#define NEW_MESSAGE_MDT_MASK              0x80

#define RETX_MDT_MASK                     0xC0

#define OPTIONAL_MDT_MASK                 0x40

#define FREE_SLOT_MDT_MASK                0x41

#define ERROR_INDICATION                  (byte)0xFF

#define CB_SCHEDULING_PERIOD_OFFSET       (0x03*CB_MULTIFRAME_LENGTH)


#define CB_MAX_MESSAGES_SCHEDULE_PERIOD   0x30


/*UMTS BMC macros*/
#define CB_BMC_OFFSET_BITMAP              0x03

#define BMC_INVALID_SFN                   0xFFFFFF

#ifdef FEATURE_NAS_REL6
#define BMC_INVALID_SERIAL_NUMBER         0xFFFFFF
/*
**Spec 25.324 Section 11.12
**Future extension bitmap Extension 0 will indicate the 
**inclusion of Serial numbers in the Scheduling message 
*/
#define FUTURE_EXTENSION_BITMAP_EXT0         0x01
#endif
typedef enum cb_scheduling_element_id_tag
{
  CB_NO_MESSAGE_TX,
  CB_NEW_TX ,
  CB_OLD_TX,
  CB_RETX
} cb_scheduling_element_id_type ;

typedef struct cb_new_tx_scheduling_table_tag
{
  dword scheduled_fn ;
  
  word  message_id ;
  
} cb_new_tx_scheduling_table_type ;

typedef struct cb_retx_scheduling_table_tag
{
  dword scheduled_fn ;
  byte  original_message_offset ;  
} cb_retx_scheduling_table_type ;

typedef union cb_scheduling_element_tag
{
  cb_new_tx_scheduling_table_type new_tx_message ;
  cb_retx_scheduling_table_type   retx_message ;
} cb_scheduling_element_type ;
#ifdef FEATURE_NAS_REL6
typedef struct cb_schedule_message_extn_tag
{
  dword       serial_number;
}cb_schedule_message_extn_type;
#endif

typedef struct cb_scheduling_table_tag
{
  cb_scheduling_element_id_type element_id ;

  cb_scheduling_element_type scheduling_element ;
#ifdef FEATURE_NAS_REL6
  cb_schedule_message_extn_type  schedule_extension;
#endif
} cb_scheduling_table_type ;


/*UMTS BMC MDT type*/

typedef enum cb_bmc_msg_description_type_s
{
   RETX_NEW_BMC_WITHIN_SCH_PRD,
   NEW_MESSAGE,
   READING_ADVISED,
   READING_OPTIONAL,
   RETX_OLD_MESSAGE_WITHIN_SCH_PRD,
   RETX_OLD_MESSAGE_PREV_SCH_PRD,
   SCHEDULE_MESSAGE,
   CBS41_MESSAGE,
   NO_MESSAGE

}cb_bmc_msg_description_type;

#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM) && (defined FEATURE_GSM_CB_DRX_SUPPORTED)
/*Variable to store the current SFN BMC is operating*/

extern dword cb_current_sfn_sim[MAX_AS_IDS];

#define  cb_current_sfn cb_current_sfn_sim[cb_as_id]
/*Variables to store the first and last SFN in scheduluing period*/

extern dword current_last_sfn_sch_prd_sim[MAX_AS_IDS];
extern dword current_first_sfn_sch_prd_sim[MAX_AS_IDS];
extern dword current_first_sch_msg_sfn_sim[MAX_AS_IDS];
extern dword next_first_sfn_sch_prd_sim[MAX_AS_IDS];
extern dword next_last_sfn_sch_prd_sim[MAX_AS_IDS];
extern dword next_first_sch_msg_sfn_sim[MAX_AS_IDS];
extern dword next_last_sch_msg_sfn_sim[MAX_AS_IDS];


#define  current_last_sfn_sch_prd    current_last_sfn_sch_prd_sim[cb_as_id]
#define  current_first_sfn_sch_prd   current_first_sfn_sch_prd_sim[cb_as_id]
#define  current_first_sch_msg_sfn   current_first_sch_msg_sfn_sim[cb_as_id]
#define  next_first_sfn_sch_prd      next_first_sfn_sch_prd_sim[cb_as_id]
#define  next_last_sfn_sch_prd       next_last_sfn_sch_prd_sim[cb_as_id]
#define  next_first_sch_msg_sfn      next_first_sch_msg_sfn_sim[cb_as_id]
#define  next_last_sch_msg_sfn       next_last_sch_msg_sfn_sim[cb_as_id]
#else //#if (defined FEATURE_NAS_CBS_DSDS) && (defined FEATURE_GSM_CB_DRX_SUPPORTED)
/*Variable to store the current SFN BMC is operating*/

extern dword cb_current_sfn;

/*Variables to store the first and last SFN in scheduluing period*/

extern dword current_last_sfn_sch_prd;
extern dword current_first_sfn_sch_prd;
extern dword current_first_sch_msg_sfn;
extern dword next_first_sfn_sch_prd;
extern dword next_last_sfn_sch_prd;
extern dword next_first_sch_msg_sfn;
extern dword next_last_sch_msg_sfn;

#endif //#if (defined FEATURE_NAS_CBS_DSDS) && (defined FEATURE_GSM_CB_DRX_SUPPORTED)
/*Function Prototypes*/

/*===========================================================================

FUNCTION    CB_BMC_PROCESS_SCHEDULING_TABLE

DESCRIPTION
  This function process the scheduling table and issues scheduling request to 
  Layer1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_process_scheduling_message
(
  byte  *scheduling_page,
  dword  arrival_sfn 
#ifdef FEATURE_NAS_REL6
 , word  cb_bmc_scheduling_msg_length
#endif
) ;

/*===========================================================================

FUNCTION    CB_BMC_CANCEL_SCHEDULING

DESCRIPTION
   This function cancels the scheduling.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_cancel_scheduling(void);

/*===========================================================================

FUNCTION    CB_BMC_CHECK_SFN_WITHIN_SCH_PRD

DESCRIPTION
    Checks the SFN falls within the schedule period or not.
DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cb_bmc_check_sfn_within_sch_prd
(
 dword sfn,
 dword start_sfn_sch_prd,
 dword end_sfn_sch_prd
);

/*===========================================================================

FUNCTION    CB_BMC_VALIDATE_FIRST_PDU_DRX_MODE

DESCRIPTION
  This function validates the PDU received is the First PDU in the DRX cycle
DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cb_bmc_validate_first_pdu_drx_mode
(
 dword  received_sfn,
 byte   *rlc_pdu
);

/*===========================================================================

FUNCTION    CB_BMC_RESET_MSG_BITMAP_INFO 

DESCRIPTION
  This function will reset the message bitmap info in case
  of deleting pages and cell reselction.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_reset_msg_bitmap_info
(
  word msg_id
);

/*===========================================================================

FUNCTION    CB_BMC_FREE_TX_LIST

DESCRIPTION
   This function free the new tx list and retx list from the given message list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_free_tx_list
(
  cb_message_list_type *message_list
);

/*===========================================================================

FUNCTION    CB_BMC_FREE_TX_RETX_LIST

DESCRIPTION
   This function free the new tx list and retx list from the entire
   message list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cb_bmc_free_tx_retx_list(void);

/*===========================================================================
FUNCTION    CB_BMC_SKIP_NEXT_RECEPTION
  This function generates skip for the slots which are already scheduled(Retransmissions in current
  schedule period and for the transmissions and Retransmissions in the next schedule period) for the
  received message

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_skip_next_reception
(
  cb_message_list_type *message_ptr
#ifdef FEATURE_NAS_REL6
  ,dword   data_sn
#endif
);


/*===========================================================================

FUNCTION    CB_BMC_SKIP_SCHEDULED_RECEPTION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_skip_scheduled_reception
(
  cb_message_list_type *message_ptr
);


/*===========================================================================

FUNCTION    CB_BMC_START_NEW_DRX_CYCLE

DESCRIPTION
   This function is called for every new DRX cycle. The function will free the
   current tx list and initialistes the current free list with next tx list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_start_new_drx_cycle(void);

/*===========================================================================

FUNCTION    CB_BMC_END_CURRENT_DRX_CYCLE

DESCRIPTION
   This function is called at the end of  DRX cycle. The function will free the
   current tx list.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_bmc_end_current_drx_cycle(void);

/*===========================================================================

FUNCTION    CB_BMC_UPDATE_NEW_MSG_BITMAP_INFO 

DESCRIPTION
  This function will update the msg bit map field.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_update_new_msg_bitmap_info
(
  dword   SFN,
  byte    first_pdu,
  boolean recd_status
);

/*===========================================================================

FUNCTION    CB_BMC_SKIP_UNSCHEDULED_EMPTY_SLOTS 

DESCRIPTION
  This function will skip the unscheduled empty slots

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_bmc_skip_unscheduled_empty_slots
(
  dword received_end_sfn
);


/*===========================================================================

FUNCTION    CB_BMC_FILTER_DUPLICATE_OLD_MESSAGE

DESCRIPTION
  This function will filter the old messages which already tranmitted in the 
previous schedule period.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_filter_duplicate_old_message
(
  cb_scheduling_table_type *scheduling_table,
  word                      sch_len
);

#endif /* DRX */

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED

/*===========================================================================

FUNCTION    CB_PROCESS_SCHEDULING_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_process_scheduling_message
(
  cb_message_list_type *scheduling_message_descriptor,
  dword arrival_fn ,
  cb_channel_ind_type  channel_ind
);

/*===========================================================================

FUNCTION    CB_PRELIMENARY_SCHEDULING_MESSAGE_PARSING

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_prelimenary_scheduling_message_parsing
(
  byte *scheduling_message_header,
  byte *scheduling_block_type,
  byte *begin_scheduling_slot,
  byte *end_scheduling_slot
);

/*===========================================================================

FUNCTION    CB_SKIP_SCHEDULED_RECEPTION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_skip_scheduled_reception
(
  cb_message_list_type *message_ptr
);

#endif

extern boolean cb_extended_cbch_supported ;

#define CB_MULTIFRAME_LENGTH              51

#define GSM_HYPER_FRAME_LENGTH            2715648

#define CB_CHANNEL_CYCLE                  0x08

/*===========================================================================

FUNCTION      CB_SKIP_NUMBER_OF_PAGINGBLOCKS

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_skip_number_of_pagingbloks
(
  dword received_fn,
  byte  required_blocks
) ;

/*===========================================================================

FUNCTION      CB_SKIP_REST_OF_PAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_skip_rest_of_page
(
  dword received_fn,
  byte received_tobe_skiped_paging_block
) ;

/*===========================================================================

FUNCTION    CB_SKIP_MESSAGE_PAGINGBLOKS

DESCRIPTION
  This function compiles the skip sequence (FN list) based on FN that the FIRST
  paging block of the current page has been received with and paging block 
  sequence (SECOND, THIRD or FOURTH) number which are to be skiped.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cb_skip_message_pagingbloks
(
  cb_missing_page_type *current_page_ptr,
  dword received_fn,
  byte required_blocks
) ;

/*===========================================================================

  FUNCTION    CB_SKIP_UNNECESSARY_PAGING_BLOCKS

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern cb_page_assembling_state_type cb_skip_unnecessary_paging_blocks
(
  dword received_fn,
  byte received_block_mask,
  cb_missing_page_type *current_page_ptr
) ;


/*===========================================================================

FUNCTION    CB_BMC_GENERATE_SKIP_REQUEST

DESCRIPTION
  This function generates skip request 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void cb_bmc_generate_skip_request
(
  cb_message_list_type         *msg_list,
  word                         list_size,
  dword                         *sfn_list
);

/*===========================================================================

FUNCTION    CB_BMC_VALIDATE_PDU_SFN

DESCRIPTION
  This function validates incoming SFN that the current RLC PDU is associated
  with

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the SFN belonging to the SFNs allocated for CTCH (SIB 5 config) and
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cb_bmc_validate_pdu_sfn
(
  dword pdu_sfn
) ;


#endif /* #ifdef FEATURE_GSM_CB */

#endif /* #ifndef CB_SCHEDULE_H */
