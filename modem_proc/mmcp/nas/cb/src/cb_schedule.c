/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     CB SCHEDULAR (CB_SCHEDULE.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cb_schedule.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/09/11   vm      Order of FN within the same skip list ? GL1/WL1 expects FN list in ascending order 
08/03/11   jbk     CB DRX Begin slot interpretation change.
06/15/07   jbk     clean up LINT errors.
04/15/06   kvk     Renamed cb_free and cb_alloc to cb_mem_free and cb_mem_alloc 
                   to make it compatible with L4 builds
03/27/06   vdr     Optimized cb_skip_unnecessary_paging_blocks() in order to speed up
                   CB message reception
12/12/05   kvk     Added check for first_tx_schedule to verify the pointer is NULL before
                   accessing the member prev_ptr
07/26/05   kvk     Added error handling if network sends CTCH offset more than one complte SFN 
                   rollover cycle. 
07/25/05   kvk     Added error handling to trash the schedule message if the schedule period
                   length is set to zero or CTCH offset is set to zero.
05/25/05   kvk     Now adding an invalid SFN in case the last SFN in the 
                   Schedule Request(DRX) is not Last SFN of the DRX cycle. This will
                   delay the Wl1 DRX period and avoid toggling between the DRX and NonDRX modes.
05/24/05   kvk     Now the New Message bitmap is parsed correctly starting from LSB
                   to MSB in each octet.
05/16/05   kvk     Added DRX optimization. Now with the previous DRX schedule period
                   history able to build optimized DRX schedule request to WL1. Added
                   intelligence to the DRX module to detect the first PDU in the
                   DRX period if RLC won't indicate the PDU as first PDU(Without 0x7c config).                   
04/19/05   kvk     Corrected  SFN calculation after the SFN rollover.
03/29/05   kvk     Duplication detectetion added for the scheduling message.
03/15/05   kvk     Added support for UMTS BMC DRX scheduling
04/14/04   vdr     Fixed functions which are processing SKIP request
04/16/04   vdr     Featurizing out CB DRX mode processing
                   Added some extra comments
07/03/03   jca     Initial version
12/13/11   vm      Order of FN within the same skip list ? GL1/WL1 expects FN list in ascending order  

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_CB
#include <string.h>
#include <stringl/stringl.h>
#include "cbmsg.h"
#include "msg.h"
#include "err.h"
#include "gs_v.h"
#include "cbtask.h"
#include "cb_utils.h"
#include "ms.h"
#include "sys_stru_v.h"
#include "cb_schedule.h"
#include "cb_mem.h"
#include "cb_utils.h"
#include "bit.h"
#ifdef FEATURE_DUAL_SIM
#include "cb_state.h"
#endif
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#ifdef FEATURE_DUAL_SIM
extern cb_as_id_e_type cb_as_id;
#endif

boolean cb_extended_cbch_supported = FALSE ;

#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
/* 
** Boolean flag indicating whether BMC task is going to 
** process "Reading Advised" SFNs of the BMC Scheduling Message 
*/
boolean  cb_bmc_reading_advised_sim[MAX_AS_IDS] ;
#define cb_bmc_reading_advised  cb_bmc_reading_advised_sim[cb_as_id]
/* 
** Boolean flag indicating whether BMC task is going to 
** process "Reading Optional" SFNs of the BMC Scheduling Message 
*/
boolean  cb_bmc_reading_optional_sim[MAX_AS_IDS] ;
#define  cb_bmc_reading_optional  cb_bmc_reading_optional_sim[cb_as_id]
/* RLC pdu size */
byte     cb_bmc_rlc_pdu_size_sim[MAX_AS_IDS] ;
#define  cb_bmc_rlc_pdu_size  cb_bmc_rlc_pdu_size_sim[cb_as_id]

#define   bmc_active_rat bmc_active_rat_sim[cb_as_id]

extern cb_rat_type bmc_active_rat_sim[MAX_AS_IDS];

#else
/* 
** Boolean flag indicating whether BMC task is going to 
** process "Reading Advised" SFNs of the BMC Scheduling Message 
*/
boolean cb_bmc_reading_advised ;

/* 
** Boolean flag indicating whether BMC task is going to 
** process "Reading Optional" SFNs of the BMC Scheduling Message 
*/
boolean cb_bmc_reading_optional  ;

/* RLC pdu size */
byte  cb_bmc_rlc_pdu_size;

extern cb_rat_type bmc_active_rat;
#endif //#ifdef FEATURE_NAS_CBS_DSDS

#ifdef FEATURE_UMTS_BMC

#include "l1task.h"
#include "wcdmatmcif.h"

#endif /*#ifdef FEATURE_UMTS_BMC*/

#if defined (FEATURE_UMTS_BMC) || defined (FEATURE_GSM_CB_DRX_SUPPORTED)
/* number of radio frame per TTI */
byte cb_bmc_ctch_rf_per_tti = 0;

/* Period of CTCH allocation ( N ) */
word cb_bmc_ctch_allocation_period = 0;

/* CBS frame offset ( K ) */
byte cb_bmc_cbs_frame_offset = 0;

#endif /* #if defined (FEATURE_UMTS_BMC) || defined (FEATURE_GSM_CB_DRX_SUPPORTED) */

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)

#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM) && (defined FEATURE_GSM_CB_DRX_SUPPORTED)

/*Variable to store the lastest SFN BMC is operating*/
dword cb_current_sfn_sim[MAX_AS_IDS];
#define cb_current_sfn cb_current_sfn_sim[cb_as_id]
cb_scheduling_table_type *cb_bmc_scheduling_table_sim[MAX_AS_IDS];
#define  cb_bmc_scheduling_table cb_bmc_scheduling_table_sim[cb_as_id]
byte cb_bmc_scheduling_period_length_sim[MAX_AS_IDS];
#define  cb_bmc_scheduling_period_length cb_bmc_scheduling_period_length_sim[cb_as_id]

/*Variables to store the first and last SFN in scheduluing period*/
dword current_last_sfn_sch_prd_sim[MAX_AS_IDS] ;
#define current_last_sfn_sch_prd current_last_sfn_sch_prd_sim[cb_as_id]
dword current_first_sfn_sch_prd_sim[MAX_AS_IDS] ;
#define current_first_sfn_sch_prd current_first_sfn_sch_prd_sim[cb_as_id]
dword current_first_sch_msg_sfn_sim[MAX_AS_IDS] ;
#define current_first_sch_msg_sfn current_first_sch_msg_sfn_sim[cb_as_id]
dword next_first_sfn_sch_prd_sim[MAX_AS_IDS] ;
#define next_first_sfn_sch_prd next_first_sfn_sch_prd_sim[cb_as_id]
dword next_last_sfn_sch_prd_sim[MAX_AS_IDS] ;
#define next_last_sfn_sch_prd next_last_sfn_sch_prd_sim[cb_as_id]
dword next_first_sch_msg_sfn_sim[MAX_AS_IDS] ;
#define next_first_sch_msg_sfn next_first_sch_msg_sfn_sim[cb_as_id]
dword next_last_sch_msg_sfn_sim[MAX_AS_IDS] ;
#define next_last_sch_msg_sfn next_last_sch_msg_sfn_sim[cb_as_id]

/*List to store the new message bit map*/
cb_bmc_new_msg_bit_map_info_t *prev_new_msg_bitmap_sim[MAX_AS_IDS] ;
#define  prev_new_msg_bitmap prev_new_msg_bitmap_sim[cb_as_id]
cb_bmc_new_msg_bit_map_info_t *curr_new_msg_bitmap_sim[MAX_AS_IDS] ;
#define  curr_new_msg_bitmap curr_new_msg_bitmap_sim[cb_as_id]
cb_bmc_new_msg_bit_map_info_t *last_curr_new_msg_bitmap_sim[MAX_AS_IDS] ;
#define  last_curr_new_msg_bitmap last_curr_new_msg_bitmap_sim[cb_as_id]

#else
/*Variable to store the lastest SFN BMC is operating*/
dword cb_current_sfn = (dword) BMC_INVALID_SFN;

cb_scheduling_table_type *cb_bmc_scheduling_table ;
byte cb_bmc_scheduling_period_length;

/*Variables to store the first and last SFN in scheduluing period*/
dword current_last_sfn_sch_prd  ;
dword current_first_sfn_sch_prd ;
dword current_first_sch_msg_sfn ;
dword next_first_sfn_sch_prd ;
dword next_last_sfn_sch_prd ;
dword next_first_sch_msg_sfn ;
dword next_last_sch_msg_sfn ;

/*List to store the new message bit map*/
cb_bmc_new_msg_bit_map_info_t *prev_new_msg_bitmap ;
cb_bmc_new_msg_bit_map_info_t *curr_new_msg_bitmap ;
cb_bmc_new_msg_bit_map_info_t *last_curr_new_msg_bitmap;

#endif //#if (defined FEATURE_NAS_CBS_DSDS) && (defined FEATURE_GSM_CB_DRX_SUPPORTED)
#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
/*
** From Spec 44.012 section 3.5
** Begin Slot Number (octet 1): Message slot number, relative to the schedule period, of the message 
** slot following the Schedule Message. The Begin Slot Number field is coded in binary. Value range: 1 to 48.
**
*/
#define CB_DRX_MIN_BEGIN_SLOT_NUMBER 1
#define CB_DRX_MAX_BEGIN_SLOT_NUMBER 48
#endif /*FEATURE_GSM_CB_DRX_SUPPORTED*/

#ifdef FEATURE_UMTS_BMC
extern void cb_ctch_pdu_decode_error(dword pdu_sfn);
#endif

extern dword cb_calculate_block_fn
(
  dword reference_fn,
  byte  block_number
);
extern void cb_send_event_ctch_start
(
  byte ctch_mode,
  byte scheduling_period_length
);
/*===========================================================================

FUNCTION    CB_CALCULATE_FN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dword cb_calculate_slot_fn
(
  dword reference_fn,
  byte  slot_number
)
{
  return (reference_fn + (slot_number * CB_MULTIFRAME_LENGTH * CB_CHANNEL_CYCLE)) % 
                                                       GSM_HYPER_FRAME_LENGTH ;
} /* end cb_calculate_slot_fn() */

#ifdef FEATURE_GSM_CB_DRX_SUPPORTED

/*===========================================================================

FUNCTION    GET_SLOT_NUMBER

DESCRIPTION
This function returns the slot number corresponding to bit set to one or zero
depending upon new_message_bitmap_type

When message_bitmap_type is set to TRUE, this function will return the slot number
corresponding to bit set to 1 in new message bit map.

Otherwise ( message_bit_map_type is set to FALSE ) it will return the slot number
corresponding to bit set to 0 in new message bit map.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte get_slot_number
(
  byte slot_number,
  byte *new_message_bitmap,
  boolean message_bitmap_type
)
{
  byte search_index;
  byte mask ;
  byte shift_value ;
  byte initial_index = (slot_number-1) / 8 ;

  for (search_index=initial_index; 
                          search_index<CB_NEW_MSG_BITMAP_LENGTH; search_index++)
  {
    do
    {
      if((slot_number % 8))
      {
        shift_value = (8 - (slot_number % 8)) ;
      }
      else
      {
        shift_value = 0;
      }

      mask = (byte) (1 << shift_value) ;

      if(message_bitmap_type)
      {
        if (new_message_bitmap[search_index] & mask)
        {
          return slot_number ;
        }
      }
      else
      {
        if (!(new_message_bitmap[search_index] & mask))
        {
          return slot_number ;
        }
      }
      slot_number ++ ;

    } while (((slot_number-1)% 8) != 0) ;
  }

  return 0xFF ;

} /* end get_slot_number() */

#endif /*FEATURE_GSM_CB_DRX_SUPPORTED*/

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)

/*===========================================================================

FUNCTION    CB_BMC_CALCULATE_SFN

DESCRIPTION
  Compute the SFN for the CTCH BS index

DEPENDENCIES
  None

RETURN VALUE
  SFN for CTCH BS index

SIDE EFFECTS
  None

===========================================================================*/
dword cb_bmc_calculate_sfn
(
 dword arrival_sfn,
 word tx_index_number
)
{
  dword sfn;

#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
  if (bmc_active_rat == CB_UMTS_MODE)
  {
    sfn = cb_bmc_get_Ki_sfn(arrival_sfn, tx_index_number, 
                cb_bmc_ctch_allocation_period, cb_bmc_cbs_frame_offset) ;
  }
  else
#endif
  {
    byte reference_block_number = (byte)CB_CALCULATE_PAGING_SLOT(arrival_sfn);

    while((reference_block_number != CB_FIRST_BLOCK) &&
          (reference_block_number != CB_FIFTH_BLOCK))
    {
      reference_block_number--;
      arrival_sfn -= CB_MULTIFRAME_LENGTH;
      tx_index_number++;
    }

    sfn = cb_calculate_block_fn((dword) cb_calculate_slot_fn(arrival_sfn,(byte)(tx_index_number/CB_PAGING_MAX_BLOCKS)),
                                (byte)(tx_index_number%CB_PAGING_MAX_BLOCKS));
  }

  return  sfn;
}

/*===========================================================================

FUNCTION    CB_BMC_VALIDATE_SFN

DESCRIPTION
  This function validates the SFN is TX or RETX list is outdated or needs processing.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_bmc_validate_sfn
(
  dword sfn,
  dword arrival_sfn
)
{
  dword max_sfn;

#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
  if (bmc_active_rat == CB_UMTS_MODE)
  {
    max_sfn = CB_BMC_MAX_ROLLOVER_SFN;
  }
  else
#endif
  {
    max_sfn = GSM_HYPER_FRAME_LENGTH;
  }

  /*If last SFN in the sch period is less then the first SFN(SFN rollover)*/
  if (current_last_sfn_sch_prd < current_first_sfn_sch_prd)
  {
    /*If the arrival SFN is not after rollover then validate the SFN*/
    if (current_last_sfn_sch_prd < arrival_sfn)
    {
      if ( (sfn > arrival_sfn && sfn <= max_sfn) || 
          (sfn <= current_last_sfn_sch_prd ))
      {
        return TRUE;
      }
    }
    else
    {
      /*If the arrival SFN is after rollover and in schedule period*/
      if (sfn > arrival_sfn && sfn <= current_last_sfn_sch_prd)
      {
        return TRUE;
      }
    }
  }
  else
  {
    /*If there is no rollover then verify SFN is in range from srrival SFN
        and last SFN*/
    if (sfn > arrival_sfn && sfn <= current_last_sfn_sch_prd)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION    CB_BMC_CHECK_START_PDU_NEW_MSG

DESCRIPTION
  This function validates the PDU in the msg list
DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_bmc_check_start_pdu_new_msg
(
  cb_bmc_new_msg_bit_map_info_t  *msg_bitmap_ptr,
  dword                           sfn
)
{
  cb_bmc_new_msg_bit_map_info_t *tmp_prev_ptr = NULL;

  while (msg_bitmap_ptr)
  {
    if (msg_bitmap_ptr->sfn == sfn)
    {
      if (tmp_prev_ptr)
      {
        if (!msg_bitmap_ptr->start_pdu &&
            (msg_bitmap_ptr->msg_id == tmp_prev_ptr->msg_id &&
            msg_bitmap_ptr->sfn == cb_bmc_calculate_sfn(tmp_prev_ptr->sfn, 1)))
        {
          return FALSE;
        }
        else{
          return TRUE;
        }
      }
      else{
        return TRUE;
      }
    }
    tmp_prev_ptr = msg_bitmap_ptr;
    MSG_HIGH_DS_3(cb_as_id,"bitmap:  SFN: %d %d %d", msg_bitmap_ptr->sfn, sfn, msg_bitmap_ptr->recd_flag);
    msg_bitmap_ptr = msg_bitmap_ptr->next_ptr;
  }
  return FALSE;
}

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
boolean cb_bmc_validate_first_pdu_drx_mode
(
 dword  received_sfn,
 byte  *rlc_pdu
)
{
  return ((received_sfn == current_first_sfn_sch_prd ||
           received_sfn == current_first_sch_msg_sfn ||
           cb_bmc_check_start_pdu_new_msg(prev_new_msg_bitmap, received_sfn)) &&
          (*rlc_pdu == CBS_MESSAGE_TYPE || *rlc_pdu == CBS_SCHEDULE_TYPE))
          ?TRUE:FALSE;
}
/*===========================================================================

FUNCTION    CB_BMC_GET_INDEX_NUMBER

DESCRIPTION
  Get the index from the current index where the new message exists.

DEPENDENCIES
  None

RETURN VALUE
  index value

SIDE EFFECTS
  None

===========================================================================*/
byte cb_bmc_get_index_number
(
 byte current_index,
 byte *message_bitmap,
 byte scheduling_len,
 byte octet_len
)
{
  byte search_index;
  byte mask ;
  byte shift_value ;
  byte index = current_index ;
  byte initial_index = current_index / 8 ;
  /*Go over all the octets in the message bit map*/
  for (search_index=initial_index; 
                          search_index<octet_len; search_index++)
  {
    do
    {
      /*Get the shift value*/
      shift_value = (8 - (index % 8)) ;
      /*Obtain the mask from the shift value*/
      mask = (byte) (1 << shift_value );
      /*Get the bit value for the index*/
      if (((message_bitmap[search_index] & mask) >> shift_value) % 2)
      {
        return index ;
      }

      index ++ ;
      scheduling_len--;

    } while ((index % 8) != 0 && scheduling_len) ;
  }

  return 0xFF ;

} /* end get_index_number() */



/*===========================================================================

FUNCTION    CB_BMC_INITIATE_SCHEDULING_TABLE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_initiate_scheduling_table
(
  byte scheduling_period_length, 
  cb_scheduling_table_type *scheduling_table
)
{
  byte init_index ;
  MSG_HIGH_DS_0(cb_as_id,"BMC: Initialize the scheduling table");
  /*Initialize the schedule table*/
  for (init_index=0; init_index<scheduling_period_length; init_index++)
  {
    scheduling_table[init_index].element_id = CB_NO_MESSAGE_TX ;
    
#ifdef FEATURE_NAS_REL6
    /*
    **Initialize all the serial numbers as Invalid
    */
    scheduling_table[init_index].schedule_extension.serial_number = BMC_INVALID_SERIAL_NUMBER;
#endif
  }

} /* end cb_bmc_initiate_scheduling_table() */



/*===========================================================================

FUNCTION    CB_BMC_GET_NUMBER_OF_MESSAGE_TX

DESCRIPTION
  Get the number of new message index from the message bit map.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte cb_bmc_get_number_of_message_tx
(
  word message_id,
  byte scheduling_period_length,
  cb_scheduling_table_type *scheduling_table
)
{
  byte search_index, message_counter = 0 ;

  for (search_index=0; search_index<scheduling_period_length; search_index++)
  {
    /*Ge the number of messages index having new tranmission for the given message id*/
    if ((scheduling_table[search_index].element_id == CB_NEW_TX ||
         scheduling_table[search_index].element_id == CB_OLD_TX) &&
        (scheduling_table[search_index].
               scheduling_element.new_tx_message.message_id == message_id))
    {
      message_counter++ ;
    }
  }

  return message_counter ;

} /* end get_number_of_message_tx() */

/*===========================================================================

FUNCTION    CB_BMC_GET_NUMBER_OF_MESSAGE_RETX

DESCRIPTION
  Get the number of retx index from the scheduling table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte cb_bmc_get_number_of_message_retx
(
  word message_id,
  byte scheduling_period_length,
  cb_scheduling_table_type *scheduling_table
)
{
  byte search_index, message_counter = 0 ;

  for (search_index=0; search_index<scheduling_period_length; search_index++)
  {
    /*Get the index of the new tranmission*/
    if ((scheduling_table[search_index].element_id == CB_NEW_TX ||
        scheduling_table[search_index].element_id == CB_OLD_TX) &&
        (scheduling_table[search_index].
                 scheduling_element.new_tx_message.message_id == message_id))
    {
      byte search_retx_index ;
      /*Based on the new transmission index get the count of the retransmissions*/
      for (search_retx_index=0; 
             search_retx_index<scheduling_period_length; search_retx_index++)
      {
        if ((scheduling_table[search_retx_index].element_id == CB_RETX) &&
            (scheduling_table[search_retx_index].
             scheduling_element.retx_message.original_message_offset == 
                                                               search_index))
        {
          message_counter++ ;
        }
      }
    }
  }

  return message_counter ;

} /* end get_number_of_message_retx() */


/*===========================================================================

FUNCTION    CB_BMC_GET_NEW_MESSAGE_ARRIVAL

DESCRIPTION
   This function calculates the number of new messages from the message
   bit map.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte cb_bmc_get_new_message_arrival
(
  byte *message_bitmap,
  byte schedule_period_len,
  byte bitmap_octet_len
)
{
  byte mask;
  byte shift_value;
  byte octet_index, bit_index;
  byte num_new_messages = 0;
  for (octet_index = 0; octet_index < bitmap_octet_len; octet_index++)
  {
    if (message_bitmap[octet_index] == 0)
    {
      schedule_period_len -= 8;
      continue ;
    }
    shift_value = 7;
    for (bit_index = 0; bit_index < 8 &&  schedule_period_len; bit_index++)
    {
      mask = (byte)(1 << shift_value);
      if (((message_bitmap[octet_index] & mask) >> shift_value)%2)
      {
        num_new_messages++;
      }
      shift_value--;
      schedule_period_len--;
    }
  }
  return num_new_messages;
}

/*===========================================================================

FUNCTION    CB_BMC_GET_LENGTH_RETX_LIST

DESCRIPTION
   This function returns the size of list

DEPENDENCIES
  None

RETURN VALUE
  Return length of  list

SIDE EFFECTS
  None

===========================================================================*/
byte cb_bmc_get_length_retx_list
(
  cb_retx_schedule_type      *retx_item,
  dword                       reference_sfn,
  boolean                     current_sch_prd_flag
)
{
  byte skip_length = 0;
  while (retx_item != NULL)
  {
    /*If the current search period is rolling then validate the SFN*/
    if (current_sch_prd_flag)
    {
      /*If the SFN is valid then increment the count*/
    if (cb_bmc_validate_sfn(retx_item->retx_fn, reference_sfn))
     skip_length++;
    }
    else
    {
      /*Otherwise no validation is required*/
      skip_length++;
    }

    retx_item = retx_item->next_ptr;
  }
  return skip_length;
}

/*===========================================================================

FUNCTION    cb_bmc_get_skip_retx_list

DESCRIPTION
   This function copies the SFNs into retx skip list

DEPENDENCIES
  None

RETURN VALUE
  Return length of  list

SIDE EFFECTS
  None

===========================================================================*/
void  cb_bmc_get_skip_retx_list
(
  cb_retx_schedule_type     *retx_item,
  dword                      *sfn_list,
  byte                      *skip_index,
  dword                      reference_sfn,
  boolean                   current_sch_prd_flag

)
{
  while (retx_item != NULL)
  {
    /*If the current search period is rolling validate the SFN*/
    if (current_sch_prd_flag)
    {
      /*If the SFN is valid then store the SFN in the SFN list*/
    if (cb_bmc_validate_sfn(retx_item->retx_fn, reference_sfn))
    {
     sfn_list[*skip_index] = retx_item->retx_fn;
     (*skip_index)++;
    }
    }
    else
    {
      /*No validation is required store the SFN*/
      sfn_list[*skip_index] = retx_item->retx_fn;
      (*skip_index)++;
    }

    retx_item = retx_item->next_ptr;
  }
  return;
}

#ifdef FEATURE_NAS_REL6
/*===========================================================================

FUNCTION    CB_BMC_IS_SERIAL_NUMBER_MATCHING

DESCRIPTION
   This function returns TRUE when the serial number of already receieved message and that of scheduled message
   are matching so that those slots can be skipped . Otherwise returns FALSE

DEPENDENCIES
  None

RETURN VALUE
  TRUE\FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_bmc_is_serial_number_matching
(
 cb_first_tx_schedule_type *first_tx_item,
 boolean skip_sn, 
 dword data_sn
)
{
  if((data_sn !=BMC_INVALID_SERIAL_NUMBER) &&
     (first_tx_item->serial_number != BMC_INVALID_SERIAL_NUMBER))
  {
    if(first_tx_item->serial_number !=data_sn)
    {
      /*
      **Return FALSE if both the serial numbers are valid and then the expected SN and received SN are
      **different
      */
       skip_sn = FALSE;
    }
    else
    {
       skip_sn = TRUE;
    }
  }
  return skip_sn;
}
#endif
/*===========================================================================

FUNCTION    CB_BMC_GET_LENGTH_TX_LIST

DESCRIPTION
   This function returns the size of list

DEPENDENCIES
  None

RETURN VALUE
  Return length of list

SIDE EFFECTS
  None

===========================================================================*/
byte cb_bmc_get_length_tx_list
(
 cb_message_list_type *message_list
)
{
  cb_first_tx_schedule_type *first_tx_item;
  byte   skip_length = 0;
  
  first_tx_item = message_list->message_header.first_tx_schedule;
  while(first_tx_item != NULL)
  {
    /*Increment the skip count for new transmissions in the current sch list*/
    if (cb_bmc_validate_sfn(first_tx_item->tx_fn, cb_current_sfn))
    {
      skip_length++;
    }
    /*Now increment the count for retransmissions*/
    skip_length += cb_bmc_get_length_retx_list(first_tx_item->retx_schedule, 
                                              cb_current_sfn, TRUE);
    first_tx_item = first_tx_item->next_ptr;
    
    
  }

  first_tx_item = message_list->message_header.next_first_tx_schedule;
  while(first_tx_item != NULL)
  {
    /*Increment the skip count for new transmissions in the next sch list*/
    skip_length += cb_bmc_get_length_retx_list(first_tx_item->retx_schedule,
                                              cb_current_sfn, FALSE);
     /*Now increment the count for retransmissions*/
    first_tx_item = first_tx_item->next_ptr;
    skip_length++;
    
  }
  return skip_length;
}


/*===========================================================================

FUNCTION    CB_BMC_GET_SKIP_LIST_SFN

DESCRIPTION
   This function stores the skip list from the SFN
DEPENDENCIES
  None

RETURN VALUE
  Return length of list

SIDE EFFECTS
  None

===========================================================================*/
 /*lint -e715*/
void cb_bmc_get_skip_list_sfn
(
 cb_message_list_type *message_list,
 byte                  skip_list_length,
 dword                *sfn_list
)
{
  cb_first_tx_schedule_type *first_tx_item;
  byte   skip_index = 0;
  
  first_tx_item = message_list->message_header.first_tx_schedule;
  while(first_tx_item != NULL)
  {
    /*Get the SFN list for the new transmission in the current scheduling period*/
    if (cb_bmc_validate_sfn(first_tx_item->tx_fn, cb_current_sfn))
    {
      sfn_list[skip_index] = first_tx_item->tx_fn;
      skip_index++;
    }

    if(skip_index >= skip_list_length)
    {
      return;
    }

    /*Now get the SFN list for retransmissions*/
    cb_bmc_get_skip_retx_list(first_tx_item->retx_schedule, sfn_list, &skip_index,
                              cb_current_sfn, TRUE);
    first_tx_item = first_tx_item->next_ptr;
    
  }

  first_tx_item = message_list->message_header.next_first_tx_schedule;
  while(first_tx_item != NULL)
  {
    /*Get the SFN list for the new transmission in the next scheduling period*/
    sfn_list[skip_index] = first_tx_item->tx_fn;
    skip_index++;

    if(skip_index >= skip_list_length)
    {
      return;
    }

    /*Now get the SFN list for retransmissions*/
    cb_bmc_get_skip_retx_list(first_tx_item->retx_schedule, sfn_list, &skip_index,
                              cb_current_sfn, FALSE);
    first_tx_item = first_tx_item->next_ptr;
  }
  return ;
}
 /*lint +e715*/



/*===========================================================================

FUNCTION    CB_BMC_FREE_RETX_LIST

DESCRIPTION
   This function free the retx list for the given tx list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_free_retx_list
(
  cb_retx_schedule_type     *retx_item
)
{
  cb_retx_schedule_type   *tmp_ptr;
  while (retx_item != NULL)
  {
    tmp_ptr = retx_item;
    retx_item = retx_item->next_ptr;
    if (retx_item != NULL)
    {
      retx_item->prev_ptr = NULL;
    }

    cb_mem_free((void *)tmp_ptr);
    
  }
  return;
}
#ifdef FEATURE_NAS_REL6
/*===========================================================================

FUNCTION    CB_BMC_SN_FREE_TX_LIST

DESCRIPTION
   This function free the new tx list and retx list from the given message list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_sn_free_tx_list
(
  cb_message_list_type *message_list,
  dword            data_sn
)
{
  cb_first_tx_schedule_type *first_tx_item;
  cb_first_tx_schedule_type     *tmp_ptr;
  boolean skip_sn=TRUE;

  first_tx_item = message_list->message_header.first_tx_schedule;
  while(first_tx_item != NULL)
  {
    /*
    **Dont skip the slots if serial numbers are not matching and 
    **skip the consecutive frame comparisons till we get next start SN
    */
    skip_sn = cb_bmc_is_serial_number_matching(first_tx_item,skip_sn, data_sn);

    /*Now increment the count for retransmissions*/
    if(skip_sn)
    {
      /*Free the retx list for the new transmission*/
      cb_bmc_free_retx_list(first_tx_item->retx_schedule);
      tmp_ptr = first_tx_item;
      /*free the pointer*/
      first_tx_item = first_tx_item->next_ptr;
      if (first_tx_item)
      {
        first_tx_item->prev_ptr = NULL;
      }
      cb_mem_free((void *)tmp_ptr);
    }
    else
    {
      if(first_tx_item->next_ptr == NULL)
      {
          /* End of list detected move towards beginning*/
          while(first_tx_item->prev_ptr !=NULL)
          {
            first_tx_item = first_tx_item->prev_ptr;
          }
          break;
      }
      else
      {
        first_tx_item = first_tx_item->next_ptr;
      }
    }
  }
  message_list->message_header.first_tx_schedule = first_tx_item;
  skip_sn=TRUE;

  /*Now free the next tx list if exists*/
  first_tx_item = message_list->message_header.next_first_tx_schedule;
  while(first_tx_item != NULL)
  {
    /*
    **Dont skip the slots if serial numbers are not matching and 
    **skip the consecutive frame comparisons till we get next start SN
    */
    skip_sn = cb_bmc_is_serial_number_matching(first_tx_item,skip_sn, data_sn);
    /*Now increment the count for retransmissions*/
    if(skip_sn)
    {
      cb_bmc_free_retx_list(first_tx_item->retx_schedule);
      tmp_ptr = first_tx_item;
      first_tx_item = first_tx_item->next_ptr;
      if (first_tx_item)
      {
        first_tx_item->prev_ptr = NULL;
      }

      /*free the pointer*/
      cb_mem_free((void *)tmp_ptr);
    }
    else
    {
      if(first_tx_item->next_ptr == NULL)
      {
        /* End of list detected move towards beginning*/
        while(first_tx_item->prev_ptr != NULL)
        {
          first_tx_item = first_tx_item->prev_ptr;
        }
        break;
      }
      else
      {
        first_tx_item = first_tx_item->next_ptr;
      }
    }
  }
  message_list->message_header.next_first_tx_schedule = first_tx_item;
}
#endif


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
void cb_bmc_free_tx_list
(
  cb_message_list_type *message_list
)
{
  cb_first_tx_schedule_type *first_tx_item;
  cb_first_tx_schedule_type     *tmp_ptr;
  first_tx_item = message_list->message_header.first_tx_schedule;
  while(first_tx_item != NULL)
  {
    /*Free the retx list for the new transmission*/
    cb_bmc_free_retx_list(first_tx_item->retx_schedule);
    tmp_ptr = first_tx_item;
    first_tx_item = first_tx_item->next_ptr;
    if (first_tx_item)
    {
      first_tx_item->prev_ptr = NULL;
    }
    /*free the pointer*/
    cb_mem_free((void *)tmp_ptr);
  }
  message_list->message_header.first_tx_schedule = NULL;
  /*Now free the next tx list if exists*/
  first_tx_item = message_list->message_header.next_first_tx_schedule;
  while(first_tx_item != NULL)
  {

    cb_bmc_free_retx_list(first_tx_item->retx_schedule);
    tmp_ptr = first_tx_item;
    first_tx_item = first_tx_item->next_ptr;
    if (first_tx_item)
    {
      first_tx_item->prev_ptr = NULL;
    }
    /*free the pointer*/
    cb_mem_free((void *)tmp_ptr);
  }
  message_list->message_header.next_first_tx_schedule = NULL;
}


/*===========================================================================

FUNCTION    CB_BMC_FREE_CURRENT_TX_LIST

DESCRIPTION
   This function free the new tx list and retx list from the given message list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_free_current_tx_list
(
  cb_message_list_type *message_list
)
{
  cb_first_tx_schedule_type *first_tx_item;
  cb_first_tx_schedule_type     *tmp_ptr;
  first_tx_item = message_list->message_header.first_tx_schedule;
  while(first_tx_item != NULL)
  {
    cb_bmc_free_retx_list(first_tx_item->retx_schedule);
    tmp_ptr = first_tx_item;
    first_tx_item = first_tx_item->next_ptr;
    if (first_tx_item)
    {
      first_tx_item->prev_ptr = NULL;
    }

    /*free the pointer*/
    cb_mem_free((void *)tmp_ptr);
  }
  message_list->message_header.first_tx_schedule = NULL;

}

/*===========================================================================

FUNCTION    CB_BMC_ADD_NEW_MSG_ENTRY 

DESCRIPTION
  This function will prepare the list with all new messages in the 
current schedule messages.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_add_new_msg_entry
(
  /*New msg bit map pointer*/
  cb_bmc_new_msg_bit_map_info_t **new_msg_bit_map,
  /*message id*/
  word                            msg_id,
  /*SFN*/
  dword                           SFN
)
{
  cb_bmc_new_msg_bit_map_info_t *tmp_new_msg;
  cb_bmc_new_msg_bit_map_info_t *tmp_ref_ptr = *new_msg_bit_map;


  if ((tmp_new_msg = (cb_bmc_new_msg_bit_map_info_t *)cb_mem_alloc(CB_NEW_MSG_ALLOC_ID,
                                                 sizeof(cb_bmc_new_msg_bit_map_info_t))) == NULL)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to save new msg bitmap info received now") ;
    return ;
  }

  tmp_new_msg->msg_id    = msg_id;
  tmp_new_msg->recd_flag = FALSE;
  tmp_new_msg->sfn       = SFN;
#ifdef FEATURE_NAS_REL6/* default value of serial number as Invalid */
  tmp_new_msg->serial_number = BMC_INVALID_SERIAL_NUMBER;
#endif
  tmp_new_msg->start_pdu     = FALSE;
  tmp_new_msg->next_ptr  = NULL;

  if (tmp_ref_ptr == NULL)
  {
    *new_msg_bit_map = tmp_new_msg;
    last_curr_new_msg_bitmap = tmp_new_msg;
  }
  else{
    last_curr_new_msg_bitmap->next_ptr = tmp_new_msg;
    last_curr_new_msg_bitmap = tmp_new_msg;
  }
}

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
  boolean first_pdu,
  boolean recd_status
)
{
  cb_bmc_new_msg_bit_map_info_t *cur_msg_ref_ptr;
  cur_msg_ref_ptr = prev_new_msg_bitmap;
  /*Scan through the entire list and update the field*/
  while (cur_msg_ref_ptr)
  {
    if (cur_msg_ref_ptr->sfn == SFN)
    {
      if (recd_status)
      {
        cur_msg_ref_ptr->recd_flag = TRUE;
        cur_msg_ref_ptr->start_pdu = first_pdu;
      }
      else{
        cur_msg_ref_ptr->recd_flag = FALSE;
        cur_msg_ref_ptr->start_pdu = FALSE;
      }
      break;
    }
    cur_msg_ref_ptr = cur_msg_ref_ptr->next_ptr;
  }

}
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
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

void cb_bmc_skip_unscheduled_empty_slots(dword received_end_sfn)
{
  cb_bmc_new_msg_bit_map_info_t *ref_ptr;
  cb_bmc_new_msg_bit_map_info_t *ref_skip_st_ptr = NULL;
  byte skip_len = 0;

  /*Reset the info in the prev msg bit map*/
  ref_ptr = prev_new_msg_bitmap;

  /*Scan the list and find the end SFN*/
  while (ref_ptr)
  {
    if ((ref_ptr->msg_id == 0xFFFF) &&
        (ref_ptr->sfn == received_end_sfn))
    {
      ref_skip_st_ptr = ref_ptr->next_ptr;
      while ((ref_ptr->next_ptr != NULL) &&
             (ref_ptr->next_ptr->msg_id == 0xFFFF))
      {
        skip_len++;
        ref_ptr = ref_ptr->next_ptr;
      }
      break;
    }
    ref_ptr = ref_ptr->next_ptr;
  }

  if (ref_skip_st_ptr && skip_len>0)
  {
    dword *skip_list=NULL;
    byte index=0;

    if (skip_len > 0)
    {
      if ((skip_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                                            sizeof(dword)*skip_len))
                                                                     == NULL)
      {
        MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to send L1 Skip Request") ;
        return ;
      }

      ref_ptr = ref_skip_st_ptr;
      while(ref_ptr && (index<skip_len))
      {
        skip_list[index++] = ref_ptr->sfn;
        ref_ptr = ref_ptr->next_ptr;
      }

      MSG_HIGH_DS_0(cb_as_id,"CB: Skipping Unscheduled empty slots");
      cb_bmc_send_skip_request((byte)skip_len,skip_list) ;
    }

  }
}
#endif
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
void cb_bmc_reset_msg_bitmap_info
(
  word msg_id
)
{
  cb_bmc_new_msg_bit_map_info_t *ref_ptr;
  /*Reset the info in the prev msg bit map*/
  ref_ptr = prev_new_msg_bitmap;
  /*Scan the list and reset the data*/
  while (ref_ptr)
  {
    if (ref_ptr->msg_id == msg_id)
    {
      ref_ptr->recd_flag = FALSE;
      ref_ptr->start_pdu = FALSE;
    }
    ref_ptr = ref_ptr->next_ptr;
  }

  /*Reset the info in the current msg bit map*/
  ref_ptr = curr_new_msg_bitmap;
  /*Scan the list and reset the data*/
  while (ref_ptr)
  {
    if (ref_ptr->msg_id == msg_id)
    {
      ref_ptr->recd_flag = FALSE;
      ref_ptr->start_pdu = FALSE;
    }
    ref_ptr = ref_ptr->next_ptr;
  }
}

/*===========================================================================

FUNCTION    CB_BMC_MARK_PDUS_RECEIVED 

DESCRIPTION
  This function will update the received status as TRUE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_mark_pdus_received
(
  cb_bmc_new_msg_bit_map_info_t  **new_msg_bit_map,
  dword                            SFN,
  byte                             no_pdus
)
{
  cb_bmc_new_msg_bit_map_info_t *tmp_msg_ptr;
  
  tmp_msg_ptr = *new_msg_bit_map;
  while(tmp_msg_ptr)
  {
    if (tmp_msg_ptr->sfn == SFN)
    {
      tmp_msg_ptr->start_pdu = TRUE;
      break;
    }
    tmp_msg_ptr = tmp_msg_ptr->next_ptr;
  }

  while(tmp_msg_ptr && no_pdus)
  {
    tmp_msg_ptr->recd_flag = TRUE;
    no_pdus--;
    tmp_msg_ptr = tmp_msg_ptr->next_ptr;
  }

  if (tmp_msg_ptr)
  {
    tmp_msg_ptr->start_pdu = TRUE;
  }
  
}


/*===========================================================================

FUNCTION    CB_BMC_DELETE_NEW_MSG_BIT_MAP 

DESCRIPTION
  This function will delete the list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_delete_new_msg_bit_map
(
  /*New msg bit map pointer*/
  cb_bmc_new_msg_bit_map_info_t **new_msg_bit_map
)
{
  cb_bmc_new_msg_bit_map_info_t *tmp_ref_ptr, *tmp_ptr;
  tmp_ref_ptr = *new_msg_bit_map;
  /*Delete the entire list of new message bit map*/
  while(tmp_ref_ptr)
  {
    tmp_ptr = tmp_ref_ptr;
    tmp_ref_ptr = tmp_ref_ptr->next_ptr;

    cb_mem_free((void *)tmp_ptr);
  }
  *new_msg_bit_map = NULL;
}


/*===========================================================================

FUNCTION    CB_BMC_DELETE_ENTRIES_CURR_MSG_BITMAP 

DESCRIPTION
  This function will delete the specified number of PDUs from the list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_delete_entries_curr_msg_bitmap
(
  cb_bmc_new_msg_bit_map_info_t  **new_msg_bit_map,
  dword                            SFN,
  byte                             no_pdus
)
{
  cb_bmc_new_msg_bit_map_info_t *ref_ptr = NULL,*tmp_msg_ptr;
  cb_bmc_new_msg_bit_map_info_t *prev_ptr = NULL, *tmp_del_ptr;
  tmp_msg_ptr = *new_msg_bit_map;
  while(tmp_msg_ptr)
  {
    if (tmp_msg_ptr->sfn == SFN)
    {
      ref_ptr = tmp_msg_ptr;
      break;
    }
    prev_ptr = tmp_msg_ptr;
    tmp_msg_ptr = tmp_msg_ptr->next_ptr;
  }
  
  tmp_msg_ptr = ref_ptr;
  while (no_pdus && ref_ptr)
  {
    tmp_del_ptr = ref_ptr;
    if (prev_ptr)
    {
      prev_ptr->next_ptr = ref_ptr->next_ptr;
    }
    ref_ptr = ref_ptr->next_ptr;
    no_pdus--;

    cb_mem_free(tmp_del_ptr);
  }
  

  if (tmp_msg_ptr == *new_msg_bit_map)
  {
    *new_msg_bit_map = ref_ptr;
  }

}

/*===========================================================================

FUNCTION    CB_BMC_GET_SCH_TABLE_SKIP_LIST_LENGTH 

DESCRIPTION
  This function calculate the length of skip list

DEPENDENCIES
  None

RETURN VALUE
  skip length

SIDE EFFECTS
  None

===========================================================================*/
word cb_bmc_get_sch_table_skip_list_length
(
  cb_scheduling_table_type *scheduling_table,
  word                      sch_len
)
{
  word index,skip_length = 0;
  for (index = 0; index < sch_len; index++)
  {
    if ( scheduling_table[index].element_id == CB_NO_MESSAGE_TX &&
          cb_bmc_check_sfn_within_sch_prd(
          scheduling_table[index].scheduling_element.new_tx_message.scheduled_fn,
          (dword)(cb_current_sfn 
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
          + ((bmc_active_rat == CB_UMTS_MODE) ? cb_bmc_ctch_rf_per_tti:0)
#endif 
          ), next_last_sfn_sch_prd)
         )
    {
      skip_length++;
    }
  }

  return skip_length;
}

/*===========================================================================

FUNCTION    CB_BMC_SKIP_MESSAGES_DRX 

DESCRIPTION
  This function will generate skip request to L1

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_skip_messages_drx
(
  cb_scheduling_table_type *scheduling_table,
  word                      sch_len
)
{
  word index = 0;
  word skip_length = 0;
  dword *skip_list;
  
  skip_length = cb_bmc_get_sch_table_skip_list_length(scheduling_table, sch_len);

 if (skip_length > 0)
 {
    if ((skip_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                                          sizeof(dword)*skip_length))
                                                                   == NULL)
    {
      MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to send L1 Skip Request") ;
      return ;
    }

    /*Copy the skip list*/
    skip_length = 0;
    for (index = 0; index < sch_len; index++)
    {
      if (scheduling_table[index].element_id == CB_NO_MESSAGE_TX &&
          cb_bmc_check_sfn_within_sch_prd(
          scheduling_table[index].scheduling_element.new_tx_message.scheduled_fn,
          (dword)(cb_current_sfn 
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
          + ((bmc_active_rat == CB_UMTS_MODE) ? cb_bmc_ctch_rf_per_tti:0)
#endif
          ), next_last_sfn_sch_prd))
      {
        skip_list[skip_length] = scheduling_table[index].scheduling_element.new_tx_message.scheduled_fn;
        skip_length++;
      }     
    }
    /*Send skip request to Wl1*/
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
    if (bmc_active_rat == CB_UMTS_MODE)
    {
      cb_bmc_send_skip_request(skip_length, skip_list);
    }
    else
#endif
    {
      cb_send_skip_request(CB_SKIP_FN_LIST, (byte)skip_length, skip_list);
    }
 }

}

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
void cb_bmc_start_new_drx_cycle()
{

  MSG_HIGH_DS_1(cb_as_id,"BMC: Start new DRX cycle with Last SFN: %d",next_last_sfn_sch_prd);

   /*Filter all the old messages if we miss anything in the previous scheduling request*/
  if (cb_bmc_scheduling_table)
  {
    byte index=0;

    /* Dont send skip for slots which were marked NO MSG & excluded in DRX request */
    while(index < cb_bmc_scheduling_period_length)
    {
      if (cb_bmc_scheduling_table[index].element_id == CB_NO_MESSAGE_TX)
        cb_bmc_scheduling_table[index].element_id = CB_NEW_TX;
      index++;
    }

    cb_bmc_filter_duplicate_old_message(cb_bmc_scheduling_table, cb_bmc_scheduling_period_length );

    /*Generate Skip request based on the skip information*/
    cb_bmc_skip_messages_drx(cb_bmc_scheduling_table, cb_bmc_scheduling_period_length );

    /*Now free the scheduling table*/
#ifndef FEATURE_MODEM_HEAP
    gs_free((void *)cb_bmc_scheduling_table);
#else
    modem_mem_free((void *)cb_bmc_scheduling_table, MODEM_MEM_CLIENT_NAS);
#endif
    cb_bmc_scheduling_table = NULL;
    cb_bmc_scheduling_period_length  = 0;
  }
 

  /*Free the prev msg bit map if exists*/
  if (prev_new_msg_bitmap)
  {
    cb_bmc_delete_new_msg_bit_map(&prev_new_msg_bitmap);
  }
  /*Copy the current message bitmap to prev msg bitmap*/
  prev_new_msg_bitmap = curr_new_msg_bitmap;

  /*Set the current message bitmap to NULL*/
  curr_new_msg_bitmap = NULL;
  last_curr_new_msg_bitmap = NULL;
 
  /*Update the first tx and retx list*/
  if (next_first_sfn_sch_prd != BMC_INVALID_SFN)
  {
    /*Free the current list and copy the next sch list to current list*/
    cb_message_list_type *message_list;
    //Start the list at entry point
    message_list = basic_cbch_list_entry_point;

    /*Copy the next begining  and last SFNs into current list*/
    current_first_sfn_sch_prd = next_first_sfn_sch_prd;
    current_last_sfn_sch_prd  = next_last_sfn_sch_prd;
    current_first_sch_msg_sfn = next_first_sch_msg_sfn;

    next_first_sfn_sch_prd = BMC_INVALID_SFN;
    next_last_sfn_sch_prd  = BMC_INVALID_SFN;
    next_first_sch_msg_sfn = BMC_INVALID_SFN;
    next_last_sch_msg_sfn = BMC_INVALID_SFN;
    
    while(message_list != NULL)
    {
      /*Check if the TX list not empty*/
      if (message_list->message_header.first_tx_schedule != NULL)
      {
        /*Free the current sch list*/
        cb_bmc_free_current_tx_list(message_list);
      }
      /*Store the next sch list in the current list*/
      message_list->message_header.first_tx_schedule = 
            message_list->message_header.next_first_tx_schedule;
      /*Assign the next scheduling list to NULL*/
      message_list->message_header.next_first_tx_schedule = NULL;

      message_list = message_list->next_message;
    }
  }

}


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
void cb_bmc_end_current_drx_cycle()
{  
  /*Free the current list and copy the next sch list to current list*/
  cb_message_list_type *message_list;

  MSG_HIGH_DS_0(cb_as_id,"BMC: Last SFN of the current DRX cycle");
  //Start the list at entry point
  message_list = basic_cbch_list_entry_point;

  /*Reset the current first SFN and last SFN in current DRX cycle*/
  current_first_sfn_sch_prd = BMC_INVALID_SFN;
  current_last_sfn_sch_prd  = BMC_INVALID_SFN;
  current_first_sch_msg_sfn = BMC_INVALID_SFN;

  /*Free the current scheduling list*/
  while (message_list != NULL)
  {
    cb_bmc_free_current_tx_list(message_list);
    message_list = message_list->next_message;
  }
  return;
}


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

void cb_bmc_free_tx_retx_list()
{
  cb_message_list_type *message_list;
  //Start the list at entry point
  message_list = basic_cbch_list_entry_point;

  while(message_list != NULL)
  {
    /*Free the tx and retx list for this message id*/
    cb_bmc_free_tx_list(message_list);
    message_list = message_list->next_message;
  }
}


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
void cb_bmc_skip_scheduled_reception
(
  cb_message_list_type *message_ptr
)
{
  /*Skip the reception of the message id*/
  byte  skip_list_length;
  dword *skip_list;
  
  skip_list_length = cb_bmc_get_length_tx_list(message_ptr);
  /*Allocate the buffer to store the skip list*/
  if (skip_list_length > 0)
  {
    if ((skip_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                                          sizeof(dword)*skip_list_length))
                                                                   == NULL)
    {
      MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to send L1 Skip Request") ;
       /*Delete Tx and Retx list*/
      cb_bmc_free_tx_list(message_ptr);

      return ;
    }

    cb_bmc_get_skip_list_sfn(message_ptr, skip_list_length, skip_list);

    /*Delete Tx and Retx list*/
    cb_bmc_free_tx_list(message_ptr);

    /*Send skip request to L1*/
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
    if (bmc_active_rat == CB_UMTS_MODE)
    {
      cb_bmc_send_skip_request(skip_list_length, skip_list);
    }
    else
#endif
    {
      cb_send_skip_request(CB_SKIP_FN_LIST, skip_list_length, skip_list);
    }

  }

  return;
}


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
boolean cb_bmc_check_sfn_within_sch_prd
(
 dword sfn,
 dword start_sfn_sch_prd,
 dword end_sfn_sch_prd
)
{
  boolean status = FALSE;
  if (end_sfn_sch_prd < start_sfn_sch_prd)
  {
    /*If the last SFN is less than first SFN then there is rollover*/
    status = (sfn >= start_sfn_sch_prd || sfn <= end_sfn_sch_prd)? TRUE: FALSE;
  }
  else
  {
    /*In case if there is no rollover then verify SFN is in the range*/
    status = (sfn >= start_sfn_sch_prd && sfn <= end_sfn_sch_prd)? TRUE: FALSE;
  }
  return status;
}



/*===========================================================================

FUNCTION    CB_BMC_VALIDATE_SCHEDULE_MESSAGE

DESCRIPTION
    Validate the received schedule message overlaps the current schedule period.
DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
 /*lint -e715*/
boolean cb_bmc_validate_schedule_message
(
 dword arrival_sfn,
 byte  offset_ctch_bs_index, 
 byte  scheduling_period_length
)
{
  dword    start_sfn;
  boolean status = TRUE;
  /*Calculate the start SFN of the schedule message*/
  start_sfn = cb_bmc_calculate_sfn(arrival_sfn, offset_ctch_bs_index);
  if (current_last_sfn_sch_prd != BMC_INVALID_SFN)
  {
    /*If the schedule message for the next schedule period overlaps with the current 
      schedule period then this is not valid schedule message*/
    status = cb_bmc_check_sfn_within_sch_prd(start_sfn, 
             current_first_sfn_sch_prd, current_last_sfn_sch_prd)? FALSE : TRUE;
  }

  return status;
 
}
 /*lint +e715*/
/*===========================================================================

FUNCTION    CB_BMC_DUPLICATE_SCHEDULE_MESSAGE

DESCRIPTION
    Detects the duplicate schedule message.
DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_bmc_duplicate_schedule_message
(
  dword arrival_sfn,
  byte offset_ctch_bs_index,
  byte sch_prd_len
)
{
  return ((cb_bmc_calculate_sfn(arrival_sfn, offset_ctch_bs_index) == next_first_sfn_sch_prd) &&
      (cb_bmc_calculate_sfn(next_first_sfn_sch_prd, (word)(sch_prd_len - 1))
      == next_last_sfn_sch_prd)?TRUE:FALSE);
  
}

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

void cb_bmc_cancel_scheduling(void)
{
  /*Delete the current and prev new msg list*/
  if (prev_new_msg_bitmap)
  {
    cb_bmc_delete_new_msg_bit_map(&prev_new_msg_bitmap);
  }  

  if (curr_new_msg_bitmap)
  {
    cb_bmc_delete_new_msg_bit_map(&curr_new_msg_bitmap);
  }

  last_curr_new_msg_bitmap = NULL;

  if (cb_bmc_scheduling_table)
  {
#ifndef FEATURE_MODEM_HEAP
    gs_free((void *)cb_bmc_scheduling_table);
#else
    modem_mem_free((void *)cb_bmc_scheduling_table, MODEM_MEM_CLIENT_NAS);
#endif
    cb_bmc_scheduling_table = NULL;
    cb_bmc_scheduling_period_length  = 0;
  }
  
 
  /*Free the tx and retx list from the message list*/
  cb_bmc_free_tx_retx_list();

  /*Reset the first and last SFN*/
  current_first_sfn_sch_prd = BMC_INVALID_SFN;
  current_last_sfn_sch_prd  = BMC_INVALID_SFN;
  current_first_sch_msg_sfn = BMC_INVALID_SFN;
  /*Reset the First and Last SFN variables for the next schedule period*/
  next_first_sfn_sch_prd = BMC_INVALID_SFN;
  next_last_sfn_sch_prd  = BMC_INVALID_SFN;
  next_first_sch_msg_sfn = BMC_INVALID_SFN;
  next_last_sch_msg_sfn = BMC_INVALID_SFN;

#ifdef FEATURE_GSM_CB_PSEUDO_DRX
  cb_pseudo_drx_offset = 0;
  cb_pseudo_drx_start_fn = BMC_INVALID_SFN;
  cb_pseudo_drx_prev_start_fn = BMC_INVALID_SFN;
#endif
}

/*===========================================================================

FUNCTION    CB_BMC_VERIFY_MISS_PDUS 

DESCRIPTION
  This function will verify there is any PDUs missing in order
 to receive the first PDU.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_bmc_verify_miss_pdus
(
 cb_scheduling_table_type *scheduling_table,
 byte                    no_pdus
)
{
  byte index = 0;

  for (index = 0; index < no_pdus; index++)
  {
    if (scheduling_table[index].element_id == CB_OLD_TX)
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*===========================================================================

FUNCTION    CB_BMC_MARK_NO_MESSAGE 

DESCRIPTION
  This function will mark the PDUs as the NO message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_mark_no_message
(
  cb_scheduling_table_type *scheduling_table,
  byte                      no_pdus
)
{
  byte index = 0;

  for (index = 0; index < no_pdus; index++)
  {
    scheduling_table[index].element_id = CB_NO_MESSAGE_TX;
  }
}

/*===========================================================================

FUNCTION    CB_BMC_MARK_OLD_MESSAGE 

DESCRIPTION
  This function will mark the PDUs as the OLD message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_mark_old_message
(
  cb_scheduling_table_type *scheduling_table,
  byte                      no_pdus
)
{
  byte index = 0;

  for (index = 0; index < no_pdus; index++)
  {
    scheduling_table[index].element_id = CB_OLD_TX;
  }
}


/*===========================================================================

FUNCTION    CB_BMC_MARK_PDUS 

DESCRIPTION
  This function will mark the PDUs according the status of the received PDU. If
the received status is false the set the pdu as old received pdu.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_mark_pdus
(
  cb_scheduling_table_type      *scheduling_table,
  cb_bmc_new_msg_bit_map_info_t *ref_ptr,
  byte                           no_pdus
)
{
  byte index = 0;
#ifdef FEATURE_BMC_RLC_MULTIPLE_PDU_SUPPORT
  if (ref_ptr->recd_flag == FALSE)
  {
    /*Match found mark the status of all PDUs according to the received status of SDU*/
    for (index = 0; index < no_pdus; index++)
    {
      scheduling_table[index].element_id = CB_OLD_TX;
    }
  }
#else
  for (index = 0; index < no_pdus; index++)
  {
    /*Match found mark the status according to the status of the received PDU*/      
    if (ref_ptr->recd_flag == FALSE)
    {
      scheduling_table[index].element_id = CB_OLD_TX;
    }
    ref_ptr = ref_ptr->next_ptr;
  }
#endif
}



/*===========================================================================

FUNCTION    CB_BMC_MESSAGE_VALIDATE_PREV_TX

DESCRIPTION
  This function will compare the existing message in the bitmap with all the
messages got tranmitted in the previous schedule messages and update the schedule
table accordinlgy.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_message_validate_prev_tx
(
 cb_scheduling_table_type *scheduling_table,
 word                      msg_id,
 byte                      no_pdus
#ifdef FEATURE_NAS_REL6
 ,dword                      serial_number
#endif
)
{
  cb_bmc_new_msg_bit_map_info_t *tmp_msg_ptr = prev_new_msg_bitmap;
  byte index = 0;
  dword ref_sfn;
  cb_bmc_new_msg_bit_map_info_t *ref_start_ptr;
  byte match_found_flag = FALSE;

  /*Mark all the messages as CB_NO_MESSAGE_TX*/
  cb_bmc_mark_no_message(scheduling_table, no_pdus);

  /*scan the entire msg bitmap*/
  while(tmp_msg_ptr)
  {
    if ((tmp_msg_ptr->msg_id == msg_id) 
#ifdef FEATURE_NAS_REL6
         &&( (tmp_msg_ptr->serial_number == BMC_INVALID_SERIAL_NUMBER) ||
             (serial_number == BMC_INVALID_SERIAL_NUMBER) ||
         (tmp_msg_ptr->serial_number == serial_number))
#endif
       &&(tmp_msg_ptr->start_pdu == TRUE)
       )
    {
      ref_start_ptr = tmp_msg_ptr;
      ref_sfn       = tmp_msg_ptr->sfn;
    }
    else{
      tmp_msg_ptr = tmp_msg_ptr->next_ptr;
      continue;
    }

    while (tmp_msg_ptr &&
           tmp_msg_ptr->msg_id == msg_id &&
#ifdef FEATURE_NAS_REL6
           ( (tmp_msg_ptr->serial_number == BMC_INVALID_SERIAL_NUMBER) ||
               (serial_number == BMC_INVALID_SERIAL_NUMBER) ||
               (tmp_msg_ptr->serial_number == serial_number))&&
#endif
           tmp_msg_ptr->sfn    == cb_bmc_calculate_sfn(ref_sfn, index))
    {
      index++;
      tmp_msg_ptr = tmp_msg_ptr->next_ptr;
#ifdef FEATURE_NAS_REL6
      if((tmp_msg_ptr) &&
         (tmp_msg_ptr->start_pdu == TRUE))
      {
        break;
      }
#endif
    }

    if (index == no_pdus)
    {  
      /*Set the match found flag to true*/
      match_found_flag = TRUE;
      /*Now mark the messages as received*/
      cb_bmc_mark_pdus(scheduling_table, ref_start_ptr, no_pdus);
    }
    index = 0;
  }

  if (match_found_flag)
  {
    /*In case any PDU is missing then always receive  the start PDU*/
    if (cb_bmc_verify_miss_pdus(scheduling_table, no_pdus))
    {
      scheduling_table[0].element_id = CB_OLD_TX;
    }
    else{
      cb_bmc_mark_pdus_received(&curr_new_msg_bitmap, 
          scheduling_table[0].scheduling_element.new_tx_message.scheduled_fn, no_pdus);
    }
   
  }
  else{
     cb_bmc_mark_old_message(scheduling_table, no_pdus);
  }

}

/*===========================================================================

FUNCTION    CB_BMC_FILTER_OLDMSG_RETX_LIST

DESCRIPTION
  This function will mark the re transmitted pdu as no message in case original
PDUs is skipped.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_filter_oldmsg_retx_list
(
  cb_scheduling_table_type *scheduling_table,
  word                      sch_len
)
{
  byte index = 0;

  for (index = 0; index < sch_len; index++)
  {
    if (scheduling_table[index].element_id == CB_RETX)
    {
      byte ctch_offset = scheduling_table[index].scheduling_element.
                                      retx_message.original_message_offset;
      if ((ctch_offset < sch_len) && (scheduling_table[ctch_offset].element_id == CB_NO_MESSAGE_TX))
      {
        scheduling_table[index].element_id = CB_NO_MESSAGE_TX;
      }
    }
  }
}

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
)
{
  word index = 0;

  MSG_HIGH_DS_0(cb_as_id,"Filter Duplicate messages in the current scheduling message");

  while(index < sch_len)
  {
    if (scheduling_table[index].element_id == CB_OLD_TX)
    {
      byte no_pdus =  1;
      dword sfn    =  scheduling_table[index].scheduling_element.new_tx_message.scheduled_fn;
      word msg_id  =  scheduling_table[index].scheduling_element.new_tx_message.message_id;
#ifdef FEATURE_NAS_REL6
      dword serial_number =  scheduling_table[index].schedule_extension.serial_number;
#endif
      index++;
      while((index < sch_len) &&
            (scheduling_table[index].scheduling_element.new_tx_message.scheduled_fn == cb_bmc_calculate_sfn(sfn, no_pdus)) &&
            (scheduling_table[index].scheduling_element.new_tx_message.message_id == msg_id) &&
            (scheduling_table[index].element_id == CB_OLD_TX) 
#ifdef FEATURE_NAS_REL6
            &&
            ((scheduling_table[index].schedule_extension.serial_number == BMC_INVALID_SERIAL_NUMBER)|
             (serial_number == BMC_INVALID_SERIAL_NUMBER) ||
            (scheduling_table[index].schedule_extension.serial_number == serial_number))
#endif
            )
      {
        no_pdus++;
        index++;
      } 

      /*Verify the message transmission in the previous message bit map*/
      cb_bmc_message_validate_prev_tx(&scheduling_table[index - no_pdus],msg_id, no_pdus
#ifdef FEATURE_NAS_REL6
      ,serial_number
#endif
      );
    }
    else{
     index++;
    }
  }
  
  /*Now filter the RETX messages based on the updated schedule table*/
  cb_bmc_filter_oldmsg_retx_list(scheduling_table, sch_len);
}
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
/*===========================================================================

FUNCTION    CB_BMC_PARSE_MESSAGE_DESCRIPTION

DESCRIPTION
  This function parse the message description and update the scheduling 
  table according to the MDT type in the message description.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte cb_bmc_parse_message_description
(
 dword                 arrival_sfn,
 byte                  ctch_offset,
 byte                  *new_message_description,
 cb_message_list_type  *msg_list,
 byte                  parsing_index,
 cb_scheduling_table_type *scheduling_table,
 byte                  bit_value,
 byte                  tx_index_number
)
{
  cb_bmc_msg_description_type msg_type = 
      (cb_bmc_msg_description_type)new_message_description[parsing_index++];
  /*Get the New message in the schedule period*/
  if (msg_type == NEW_MESSAGE ||
      msg_type == RETX_OLD_MESSAGE_PREV_SCH_PRD)
  {
    word message_id = 0;
    
    switch( msg_type)
    {
    case NEW_MESSAGE:
      /*Copy the Lower part*/
      message_id |= (word)new_message_description[parsing_index++] << 8;
      message_id |= (word)new_message_description[parsing_index++];
    
      if (cb_message_is_requested(message_id))
      {
       
        cb_message_list_type *new_msg_ptr;

        if ((new_msg_ptr=cb_find_message(msg_list,message_id)) == NULL)
        {
          if ((new_msg_ptr = cb_create_message_entry(msg_list,message_id,0)) == NULL)
          {
            return ERROR_INDICATION ;
          }

          current_message = cb_put_message_into_list(msg_list, new_msg_ptr);

          basic_cbch_list = current_message ;
        }
        

        scheduling_table[tx_index_number].element_id = CB_NEW_TX ;

        scheduling_table[tx_index_number].scheduling_element.new_tx_message.message_id =
                                          message_id;

        scheduling_table[tx_index_number].
         scheduling_element.new_tx_message.scheduled_fn = 
                    cb_bmc_calculate_sfn(arrival_sfn, (word)(tx_index_number+ctch_offset));
      
        /*Add the info in new msg bit map*/
        cb_bmc_add_new_msg_entry(&curr_new_msg_bitmap, message_id, scheduling_table[tx_index_number].
              scheduling_element.new_tx_message.scheduled_fn);
      }     
      
      break;

    case RETX_OLD_MESSAGE_PREV_SCH_PRD:
      /*Copy the Lower part*/
      message_id |= (word)new_message_description[parsing_index++] << 8;
      message_id |= (word)new_message_description[parsing_index++];

      if (cb_message_is_requested(message_id))
      {
       
        cb_message_list_type *new_msg_ptr;

        if ((new_msg_ptr=cb_find_message(msg_list,message_id)) == NULL)
        {
          if ((new_msg_ptr = cb_create_message_entry(msg_list,message_id,0)) == NULL)
          {
            return ERROR_INDICATION ;
          }

          current_message = cb_put_message_into_list(msg_list, new_msg_ptr);

          basic_cbch_list = current_message ;
        }
                
        if (bit_value)
        {
          scheduling_table[tx_index_number].element_id = CB_NEW_TX ;
        }
        else{
           scheduling_table[tx_index_number].element_id = CB_OLD_TX ;
        }

        scheduling_table[tx_index_number].scheduling_element.new_tx_message.message_id =
                                          message_id;

        scheduling_table[tx_index_number].
         scheduling_element.new_tx_message.scheduled_fn = 
                    cb_bmc_calculate_sfn(arrival_sfn, (word)(tx_index_number+ctch_offset));

         /*Add the info in new msg bit map*/
        cb_bmc_add_new_msg_entry(&curr_new_msg_bitmap, message_id, scheduling_table[tx_index_number].
              scheduling_element.new_tx_message.scheduled_fn);
      }

      break;

    default:
      break;
    }/*switch*/

    if ((tx_index_number == 0) ||
        (scheduling_table[tx_index_number].scheduling_element.new_tx_message.message_id !=
           scheduling_table[tx_index_number-1].scheduling_element.new_tx_message.message_id))
    {
      MSG_HIGH_DS_3(cb_as_id,"BMC MDT: MSG Type=%d(%d) at SFN=%lu",
                         msg_type,
                         scheduling_table[tx_index_number].element_id,
                         scheduling_table[tx_index_number].scheduling_element.new_tx_message.scheduled_fn);
    }

  }else if (msg_type == RETX_NEW_BMC_WITHIN_SCH_PRD ||
            msg_type == RETX_OLD_MESSAGE_WITHIN_SCH_PRD)
  {
    scheduling_table[tx_index_number].element_id = CB_RETX ;

    scheduling_table[tx_index_number].
        scheduling_element.retx_message.original_message_offset = 
                                     new_message_description[parsing_index++] ;

    scheduling_table[tx_index_number].
        scheduling_element.retx_message.scheduled_fn = 
                  cb_bmc_calculate_sfn(arrival_sfn, (word)(tx_index_number+ctch_offset)) ;

    if ((tx_index_number == 0) ||
        (scheduling_table[tx_index_number].scheduling_element.retx_message.original_message_offset !=
           scheduling_table[tx_index_number-1].scheduling_element.retx_message.original_message_offset))
    {
      MSG_HIGH_DS_2(cb_as_id,"BMC MDT: MSG Type=%d at SFN=%lu",
                         msg_type,
                         scheduling_table[tx_index_number].scheduling_element.retx_message.scheduled_fn);
    }

  }
  else if (msg_type == SCHEDULE_MESSAGE)
  {
    scheduling_table[tx_index_number].element_id = CB_NEW_TX ;

    scheduling_table[tx_index_number].scheduling_element.new_tx_message.message_id =
                                          0xFFFF;

    scheduling_table[tx_index_number].
         scheduling_element.new_tx_message.scheduled_fn = 
                    cb_bmc_calculate_sfn(arrival_sfn, (word)(tx_index_number+ctch_offset)) ;
     
   
       /*Add the info in new msg bit map*/
      cb_bmc_add_new_msg_entry(&curr_new_msg_bitmap, 0xFFFF,
      scheduling_table[tx_index_number].scheduling_element.new_tx_message.scheduled_fn);
    
    /*store the SFN here*/
    if (next_first_sch_msg_sfn == BMC_INVALID_SFN)
    {
      next_first_sch_msg_sfn = scheduling_table[tx_index_number].
                               scheduling_element.new_tx_message.scheduled_fn;

      next_last_sch_msg_sfn = scheduling_table[tx_index_number].
                               scheduling_element.new_tx_message.scheduled_fn;
    }
    else{
      next_last_sch_msg_sfn = scheduling_table[tx_index_number].
                               scheduling_element.new_tx_message.scheduled_fn;
    }
	if ((tx_index_number == 0) ||
        (scheduling_table[tx_index_number].scheduling_element.new_tx_message.message_id !=
           scheduling_table[tx_index_number-1].scheduling_element.new_tx_message.message_id))
    {
      MSG_HIGH_DS_3(cb_as_id,"BMC Schedule Msg MDT: MSG Type=%d(%d) at SFN=%lu",
                         msg_type,
                         scheduling_table[tx_index_number].element_id,
                         scheduling_table[tx_index_number].scheduling_element.new_tx_message.scheduled_fn);
    }
  }
  else if (msg_type == NO_MESSAGE ||
           msg_type == CBS41_MESSAGE)
  {
    
    scheduling_table[tx_index_number].element_id = CB_NO_MESSAGE_TX ;
    scheduling_table[tx_index_number].
         scheduling_element.new_tx_message.scheduled_fn = 
                      cb_bmc_calculate_sfn(arrival_sfn,(word)(tx_index_number+ctch_offset));
  }
  else if (msg_type == READING_ADVISED)
  {
    if (cb_bmc_reading_advised)
    {
      scheduling_table[tx_index_number].element_id = CB_NEW_TX ;

      scheduling_table[tx_index_number].scheduling_element.new_tx_message.message_id =
                                          0xFFFF;
      scheduling_table[tx_index_number].
         scheduling_element.new_tx_message.scheduled_fn = 
                    cb_bmc_calculate_sfn(arrival_sfn, (word)(tx_index_number+ctch_offset));
    }
    else
    {
      scheduling_table[tx_index_number].element_id = CB_NO_MESSAGE_TX ;
      scheduling_table[tx_index_number].
         scheduling_element.new_tx_message.scheduled_fn = 
                        cb_bmc_calculate_sfn(arrival_sfn, (word)(tx_index_number+ctch_offset));
    }
  }
  else if (msg_type == READING_OPTIONAL)
  {
    if (cb_bmc_reading_optional)
    {
      scheduling_table[tx_index_number].element_id = CB_NEW_TX ;

      scheduling_table[tx_index_number].scheduling_element.new_tx_message.message_id =
                                          0xFFFF;
      scheduling_table[tx_index_number].
         scheduling_element.new_tx_message.scheduled_fn = 
                        cb_bmc_calculate_sfn(arrival_sfn, (word)(tx_index_number+ctch_offset));
    }
    else
    {
      scheduling_table[tx_index_number].element_id = CB_NO_MESSAGE_TX ;
      scheduling_table[tx_index_number].
         scheduling_element.new_tx_message.scheduled_fn = 
                                  cb_bmc_calculate_sfn(arrival_sfn, tx_index_number);
    }
  }
  else
  {
    /*Do nothing*/
    
  }

  if (((msg_type != NEW_MESSAGE) &&
       (msg_type != RETX_OLD_MESSAGE_PREV_SCH_PRD) &&
       (msg_type != RETX_NEW_BMC_WITHIN_SCH_PRD) &&
       (msg_type != RETX_OLD_MESSAGE_WITHIN_SCH_PRD)&&
	   (msg_type != SCHEDULE_MESSAGE)) &&
      ((tx_index_number == 0) ||
       (scheduling_table[tx_index_number].element_id !=
         scheduling_table[tx_index_number-1].element_id)))
  {
    MSG_HIGH_DS_3(cb_as_id,"BMC MDT: MSG Type=%d(%d) at SFN=%lu",
                       msg_type,
                       scheduling_table[tx_index_number].element_id,
                       scheduling_table[tx_index_number].scheduling_element.new_tx_message.scheduled_fn);
  }

  return parsing_index;
}
#ifdef FEATURE_NAS_REL6
/*===========================================================================

FUNCTION    CB_BMC_UPDATE_NEW_MSG_BITMAP_FOR_SERIAL_NUMBER
  This function updates the serail number info in the corresponding new message
  bit map info for the required SFN

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE\FALSE
  TRUE - correctly updated bitmap
  FALSE - bitmap is not updated

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_bmc_update_new_msg_bitmap_for_serial_number
(
  dword    sfn,
  dword     serial_number
)
{
  cb_bmc_new_msg_bit_map_info_t *tmp_msg_ptr = curr_new_msg_bitmap;
  boolean status = FALSE;
  while(tmp_msg_ptr)
  {
    if (tmp_msg_ptr->sfn == sfn)
    {
      /*match found and update serial number*/
      tmp_msg_ptr->serial_number = serial_number;
      status = TRUE;
      break;
    }
    else
    {
      tmp_msg_ptr = tmp_msg_ptr->next_ptr;
      continue;
    }
  }
  return status;
}
/*===========================================================================

FUNCTION    CB_SEARCH_SFN_SCHEDULING_TABLE
  This function searches for particular SFN in scheduling table and if it is found, 
  appends the serial number to the corresponding SFN entry in scheduling table

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE\FALSE
  
  TRUE - Serial number appended to correct SFN
  FALSE - Serial number not appended

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_search_sfn_scheduling_table
(
  cb_scheduling_table_type  *scheduling_table,
  dword                       sfn,
  byte                       scheduling_period_length,
  dword                       serial_number
)
{
  byte index;
  boolean found = FALSE;
  for (index=0;index< scheduling_period_length;index++)
  {
     if(scheduling_table[index].element_id == CB_RETX )
     {
       if(scheduling_table[index].
         scheduling_element.retx_message.scheduled_fn == sfn)
       {
         scheduling_table[index].schedule_extension.serial_number = serial_number;
         found = TRUE;
         break;
       }
     }
     else/*get every thing else from New Tx*/
     {
       if(scheduling_table[index].
       scheduling_element.new_tx_message.scheduled_fn == sfn)
       {
         scheduling_table[index].schedule_extension.serial_number = serial_number;
         found = TRUE;
         break;
       }
     }
  }
  if(found == TRUE)
  {
    /*
    **update serial number in current new messge bitmap info
    */
    found = cb_bmc_update_new_msg_bitmap_for_serial_number(sfn,serial_number);
  }
  return found;
}
/*===========================================================================

FUNCTION    CB_BMC_PARSE_FUTURE_EXTENSION_BITMAP
  This function reads the future extension bit map and extarct the 
  sfn and serial numbers from rlc pdu.


DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_parse_future_extension_bitmap
(
  dword                     arrival_sfn,
  byte                      offset_ctch_bs_index,
  byte                     *future_extension_bitmap,
  cb_scheduling_table_type *scheduling_table,
  byte                      scheduling_period_length
)
{
  byte   index = 0;
  byte   future_extension_ie;
  byte   length_of_serial_number_list;
  byte   dindex = 0;
  dword  serial_number;
  byte   CTCH_bs_index;
  dword  sfn;
  if(future_extension_bitmap[index] == NULL)
  {
     MSG_HIGH_DS_0(cb_as_id,"Network didnot send Future extension bit map");
     return;
  }
  else
  {
     future_extension_ie = future_extension_bitmap[index];
     if(future_extension_ie != FUTURE_EXTENSION_BITMAP_EXT0)
     {
       MSG_HIGH_DS_1(cb_as_id,"These message extensions(%d) are not currently supported",future_extension_ie);
       return;
     }
     else
     {
       index ++;
       length_of_serial_number_list = future_extension_bitmap[index];
       MSG_HIGH_DS_1(cb_as_id,"Length of serial numbers is %d",length_of_serial_number_list);
       index ++;
       for(dindex=0;dindex<length_of_serial_number_list;dindex++)
       {
         serial_number = future_extension_bitmap[index];
         serial_number |= (0xff00)&(future_extension_bitmap[index+1]<<8);
         index+=2;
         CTCH_bs_index = future_extension_bitmap[index];
         index++;
         sfn = cb_bmc_calculate_sfn(arrival_sfn, (word)(CTCH_bs_index+offset_ctch_bs_index));
         if(cb_search_sfn_scheduling_table(scheduling_table, sfn, scheduling_period_length, serial_number))
         {
           MSG_HIGH_DS_3(cb_as_id,"serial number = %x appended to SFN = %x, CTCH_bs_index = %x",serial_number,sfn,CTCH_bs_index);
         }
         else
         {
           MSG_HIGH_DS_2(cb_as_id,"serial number = %x for the SFN = %x is dicarded",serial_number,sfn);
         }
       }
     }
  }
}
#endif
#endif
/*===========================================================================

FUNCTION    CB_BMC_DISTRIBUTE_SCHEDULING_TABLE
  This function generate the tx and retx list and appends to the 
  message list.

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_distribute_scheduling_table
(
  cb_message_list_type *msg_list,
  byte scheduling_period_length,
  cb_scheduling_table_type *scheduling_table
)
{
  cb_message_list_type *message_ptr = msg_list;
  cb_first_tx_schedule_type *first_tx_item;
  cb_retx_schedule_type     *cache_ptr;
  byte scheduling_index, retx_index;

  MSG_HIGH_DS_0(cb_as_id,"BMC: Distribute the table across the message list");

  for (scheduling_index = 0; scheduling_index < scheduling_period_length;
                             scheduling_index++)
  {
    if (scheduling_table[scheduling_index].element_id == CB_NEW_TX ||
        scheduling_table[scheduling_index].element_id == CB_OLD_TX)
    {
      /*Get the Message pointer from the Message list*/
      if ((scheduling_table[scheduling_index].scheduling_element.new_tx_message.message_id != 0xFFFF)
         && ((message_ptr = cb_find_message(msg_list, scheduling_table[scheduling_index].
                          scheduling_element.new_tx_message.message_id)) != NULL))
      {
        dword message_sfn = 
             scheduling_table[scheduling_index].scheduling_element.new_tx_message.scheduled_fn;
                          
#ifdef FEATURE_NAS_REL6
        dword message_sn =
             scheduling_table[scheduling_index].schedule_extension.serial_number;
#endif
        /*Create an entry */
        if ((first_tx_item = (cb_first_tx_schedule_type *)cb_mem_alloc(CB_FIRST_TX_SCHEDULE_ALLOC_ID,
                                      sizeof(cb_first_tx_schedule_type))) == NULL)
        {
          MSG_ERROR_DS_0(cb_as_id,"BMC: Unable to allocate memory");
          return;
        }
        first_tx_item->next_ptr = NULL;
        first_tx_item->prev_ptr = NULL;
        first_tx_item->retx_schedule = NULL;
        first_tx_item->tx_fn = message_sfn;
#ifdef FEATURE_NAS_REL6
        first_tx_item->serial_number = message_sn;
#endif
        if (message_ptr->message_header.next_first_tx_schedule == NULL)
        {
          /*This is the first item in the list*/
          message_ptr->message_header.next_first_tx_schedule = first_tx_item;
        }
        else
        {
          /*Place at the end of the List*/
          cb_first_tx_schedule_type *tx_temp_ptr = message_ptr->message_header.next_first_tx_schedule;
          while (tx_temp_ptr->next_ptr)
          {
            tx_temp_ptr = tx_temp_ptr->next_ptr;
          }
          tx_temp_ptr->next_ptr = first_tx_item;
          first_tx_item->prev_ptr = tx_temp_ptr;
        }
        /*reset the cache pointer*/
        cache_ptr = NULL;
        /*Get the Retx index if exists*/
        for (retx_index = 0; retx_index < scheduling_period_length; retx_index++)
        {
          /*if retx exists*/
          if (scheduling_table[retx_index].element_id == CB_RETX &&
              scheduling_table[retx_index].scheduling_element.retx_message.original_message_offset ==
                scheduling_index)
          {
            cb_retx_schedule_type *retx_temp_ptr;
            /*Create an Retx entry*/
            if ((retx_temp_ptr = (cb_retx_schedule_type *)cb_mem_alloc(CB_RETX_SCHEDULE_ALLOC_ID,
                                 sizeof(cb_retx_schedule_type))) == NULL)
            {
              MSG_ERROR_DS_0(cb_as_id,"BMC: Unable to allocate enough memory");
              return;
            }
            retx_temp_ptr->next_ptr = NULL;
            retx_temp_ptr->prev_ptr = NULL;
            retx_temp_ptr->retx_fn = 
               scheduling_table[retx_index].scheduling_element.retx_message.scheduled_fn;
            /*Add to the first tx item*/
            if (first_tx_item->retx_schedule == NULL)
            {
              first_tx_item->retx_schedule = retx_temp_ptr;
              cache_ptr = retx_temp_ptr;
            }
            else
            {
              /*lint -e613*/
              cache_ptr->next_ptr = retx_temp_ptr;
              /*lint +e613*/
              retx_temp_ptr->prev_ptr = cache_ptr;
              cache_ptr = retx_temp_ptr;
            }
          }/*if (scheduling_table...)*/
        }/*for(retx_index ..)*/
      }/*if (message_ptr ==)*/
    }/*if (scheduling_table[scheduling_index]...)*/
  }/*for (scheduling_index = 0 ..)*/
  return;
}

#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
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
void cb_bmc_process_scheduling_table
(
  cb_message_list_type *msg_list,
  byte scheduling_period_length,
  cb_scheduling_table_type *scheduling_table
)
{
  byte process_index ;
  byte scheduling_command_length = 0;
  dword *scheduled_fn_list ;
  l1_ext_cmd_type *cmd_ptr;
  dword            fake_sfn;

  /* DEBUG ONLY. MUST BE REMOVED */
  word dindex ;
  word prn_number ;
  /* DEBUG ONLY. MUST BE REMOVED */
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = get_wcdma_interface();
  if (ptr_W == NULL)
      return;
#endif
  
  if(scheduling_table == NULL) /* LINT ERR 613 */
  {
    MSG_HIGH_DS_1(cb_as_id, "BMC: scheduling_table is  NULL, scheduling_period_length is %d",scheduling_period_length);
    return;
  }

  MSG_HIGH_DS_2(cb_as_id,"BMC: Processing Scheduling table (%lu to %lu)", next_first_sfn_sch_prd, next_last_sfn_sch_prd);
  for (process_index=0;process_index<scheduling_period_length;process_index++)
  {
    if (scheduling_table[process_index].element_id != CB_NO_MESSAGE_TX)
    {
      scheduling_command_length++ ;
    }
  }

  if ((scheduled_fn_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                              sizeof(dword)*(scheduling_command_length+1))) == NULL)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to send L1 Scheduling Request") ;

    cb_start_umts_non_drx_mode();

    return ;
  }

  scheduling_command_length = 0 ;

  for (process_index=0;process_index<scheduling_period_length;process_index++)
  {
    if (scheduling_table[process_index].element_id != CB_NO_MESSAGE_TX)
    {
      switch (scheduling_table[process_index].element_id)
      {
        case CB_NEW_TX :
        case CB_OLD_TX:
           /*Adding priority SFN for Schedule Message*/
          if(scheduling_table[process_index].scheduling_element.new_tx_message.message_id == 0xFFFF)
          {
            scheduled_fn_list[scheduling_command_length] = 
                scheduling_table[process_index].scheduling_element.new_tx_message.scheduled_fn  | 0x40000000 ;
          }
          else
          {
            scheduled_fn_list[scheduling_command_length] = 
                scheduling_table[process_index].scheduling_element.new_tx_message.scheduled_fn ;
          }
          break ;
        case CB_RETX :
          scheduled_fn_list[scheduling_command_length] = 
                scheduling_table[process_index].scheduling_element.retx_message.scheduled_fn ;
          break ;
        default:
          MSG_ERROR_DS_0(cb_as_id,"Illegal Scheduling Element Type ID") ;
          break;
      }

      scheduling_command_length++ ;
    }
    else{
      /*Fake the NO MESSAGE as NEW MESSAGE in order to avoid this message consiedered for
        subsequent skip*/
      scheduling_table[process_index].element_id = CB_NEW_TX;
    }

  }

  /*Add the Fake SFN here*/
  if ((scheduled_fn_list[scheduling_command_length - 1] & (~0x40000000)) != next_last_sfn_sch_prd)
  {
    fake_sfn = (dword)(next_last_sfn_sch_prd | 0x80000000);
    scheduled_fn_list[scheduling_command_length++] = fake_sfn;
  }
  else if ((scheduled_fn_list[scheduling_command_length - 1] & (~0x40000000)) == next_last_sch_msg_sfn)
  {
    fake_sfn = (dword)(cb_bmc_calculate_sfn(next_last_sfn_sch_prd, 1) | 0x80000000);
    scheduled_fn_list[scheduling_command_length++] = fake_sfn;
  }
  
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

  cmd_ptr->cmd_hdr.cmd_id = CPHY_BMC_SCHEDULING_REQ;
  cmd_ptr->cmd.bmc_scheduling_req.message_header.message_set = MS_BMC_L1;
  cmd_ptr->cmd.bmc_scheduling_req.message_header.message_id  = (int)CPHY_BMC_SCHEDULING_REQ;
  cmd_ptr->cmd.bmc_scheduling_req.scheduling_mode = CTCH_DRX_MODE;
  cmd_ptr->cmd.bmc_scheduling_req.scheduling_descriptor.scheduling_period_length = 
                                               scheduling_command_length;
  cmd_ptr->cmd.bmc_scheduling_req.scheduling_descriptor.scheduling_period_sfn_list =
                                               scheduled_fn_list;

#ifdef FEATURE_DUAL_SIM
  cmd_ptr->cmd.bmc_scheduling_req.as_id = (sys_modem_as_id_e_type) cb_as_id;
#endif  

  cb_active_substate_control(CB_STD_DRX_ACTIVATED);
#ifdef FEATURE_WCDMA_ETWS
  /* not expecting ETWS state change */
  (void) cb_etws_substate_control(CB_ETWS_STD_DRX_ACTIVATED);
#endif

  MSG_HIGH_DS_0(cb_as_id,"BMC: Sending CB_SCHEDULING_REQ(CTCH_DRX_MODE) request") ;

  cb_send_event_ctch_start(CTCH_DRX_MODE,cb_bmc_scheduling_period_length);

  prn_number = scheduling_command_length/3;
  MSG_HIGH_DS_1(cb_as_id,"BMC: Sceduling sequence length is %d :",scheduling_command_length) ;
  for (dindex=0; dindex<prn_number; dindex++)
  {
    MSG_HIGH_DS_3(cb_as_id,"BMC: %lu %lu %lu",scheduled_fn_list[dindex*3],scheduled_fn_list[dindex*3 + 1],scheduled_fn_list[dindex*3 + 2]) ;
  }
  for(prn_number = dindex*3;prn_number < scheduling_command_length;prn_number++)
  {
    MSG_HIGH_DS_1(cb_as_id,"BMC: %lu",scheduled_fn_list[prn_number]) ;
  }

#ifdef FEATURE_SEGMENT_LOADING
  IWCDMA_l1_put_cmd(ptr_W, cmd_ptr );
#else
  l1_put_cmd(cmd_ptr) ;
#endif
  /*Distribute the scheduling table accross the message list*/
  cb_bmc_distribute_scheduling_table(msg_list, scheduling_period_length, scheduling_table);

  return;
}

/*===========================================================================

FUNCTION    CB_BMC_PROCESS_SCHEDULING_MESSAGE

DESCRIPTION
  This function process the scheduling message and construct the scheduling table with
  new and retx message descriptions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e550*/
void cb_bmc_process_scheduling_message
(
  byte *scheduling_page,
  dword arrival_sfn
#ifdef FEATURE_NAS_REL6
  , word scheduling_msg_length
#endif
)
{
  byte offset_ctch_bs_index;
  byte bitmap_octet_len;
  byte *msg_bitmap = NULL;
  byte  bit_value, bit_pos, byte_offset;
  byte *new_message_description;
  byte parsing_index;
  byte scheduling_index;
  /* DEBUG ONLY. MUST BE REMOVED */
  word dindex ;
  word prn_number ;
  /* DEBUG ONLY. MUST BE REMOVED */
#ifdef FEATURE_NAS_REL6
  byte* future_extension_bitmap = NULL;
#endif

   
  if(scheduling_page == NULL) /* LINT ERR 613 */
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer",0,0,0);
  }

  /*Get the offset to begin the schedule message*/
  offset_ctch_bs_index     = scheduling_page[1];
  cb_bmc_scheduling_period_length  = scheduling_page[2];

  if (cb_bmc_duplicate_schedule_message(arrival_sfn, offset_ctch_bs_index, cb_bmc_scheduling_period_length))
  {
    MSG_HIGH_DS_0(cb_as_id,"BMC: BMC detected Duplicate schedule message");
    return;
  }

  MSG_HIGH_DS_0(cb_as_id,"BMC: Processing Scheduling information");
  MSG_HIGH_DS_2(cb_as_id,"BMC: Offset CTCH BS index = %d and Sch Period length = %d",
    offset_ctch_bs_index, cb_bmc_scheduling_period_length);

  if (cb_bmc_scheduling_period_length == 0 ||  offset_ctch_bs_index == 0)
  {
    MSG_ERROR_DS_0(cb_as_id,"Invalid Schedule Message. Trashing the schedule message..");
    return;
  }

  if(!cb_bmc_validate_schedule_message(arrival_sfn, offset_ctch_bs_index, cb_bmc_scheduling_period_length))
  {
    MSG_ERROR_DS_0(cb_as_id,"BMC: Schedule Message is overlapping with current sch period");
    return;
  }
  /*Calculate bit map length*/
  bitmap_octet_len =  cb_bmc_scheduling_period_length / 8 +  
    (cb_bmc_scheduling_period_length %8 ? 1 : 0);

  /*Allocate memory to store message bitmap*/
#ifndef FEATURE_MODEM_HEAP
  if ((msg_bitmap = (byte *)gs_alloc(bitmap_octet_len))== NULL)
#else
  if ((msg_bitmap = (byte *)modem_mem_calloc(1,bitmap_octet_len, MODEM_MEM_CLIENT_NAS))== NULL)
#endif
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to process Bitmap");
    cb_start_umts_non_drx_mode();
    return ;
  }

  /*Store the msg bitmap*/
  memscpy((void *)msg_bitmap, bitmap_octet_len, (void *)(scheduling_page + CB_BMC_OFFSET_BITMAP), bitmap_octet_len);
  
  prn_number = bitmap_octet_len/3;
  for (dindex=0; dindex<prn_number; dindex++)
  {
    MSG_HIGH_DS_3(cb_as_id,"BMC msgbitmap: %x %x %x",msg_bitmap[dindex*3],msg_bitmap[dindex*3 + 1],msg_bitmap[dindex*3 + 2]) ;
  }
  for(prn_number = dindex*3;prn_number < bitmap_octet_len;prn_number++)
  {
    MSG_HIGH_DS_1(cb_as_id,"BMC msgbitmap: %x",msg_bitmap[prn_number]) ;
  }
  
  if (cb_bmc_scheduling_table)
  {
#ifndef FEATURE_MODEM_HEAP
    gs_free((void *)cb_bmc_scheduling_table);
#else
    modem_mem_free((void *)cb_bmc_scheduling_table, MODEM_MEM_CLIENT_NAS);
#endif
    cb_bmc_scheduling_table = NULL;
  }

  /*Create the scheduling table*/
#ifndef FEATURE_MODEM_HEAP
  if ((cb_bmc_scheduling_table = (cb_scheduling_table_type *)gs_alloc(
                        sizeof(cb_scheduling_table_type)*
                                            cb_bmc_scheduling_period_length)) == NULL)
#else
  if ((cb_bmc_scheduling_table = (cb_scheduling_table_type *)modem_mem_calloc(1,
                        sizeof(cb_scheduling_table_type)*
                                            cb_bmc_scheduling_period_length, MODEM_MEM_CLIENT_NAS)) == NULL)
#endif
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to process Scheduling Message") ;
    cb_start_umts_non_drx_mode();
#ifndef FEATURE_MODEM_HEAP
    gs_free((void *)msg_bitmap);
#else
    modem_mem_free((void *)msg_bitmap, MODEM_MEM_CLIENT_NAS);
#endif
    return ;
  }
  /*Initialize the scheduling table*/
  cb_bmc_initiate_scheduling_table(cb_bmc_scheduling_period_length, cb_bmc_scheduling_table) ;
  /*Go to the new message description*/
  new_message_description = scheduling_page + CB_BMC_OFFSET_BITMAP + bitmap_octet_len;

  parsing_index = 0;

  for (scheduling_index = 0; 
                   scheduling_index < cb_bmc_scheduling_period_length; scheduling_index++)
  {
    bit_pos = scheduling_index % 8;
    byte_offset = scheduling_index/8;
    bit_value = b_unpackb(msg_bitmap + byte_offset, (byte)(8 - (bit_pos + 1)), 1);
    parsing_index = cb_bmc_parse_message_description(arrival_sfn, offset_ctch_bs_index, 
      new_message_description, basic_cbch_list, parsing_index, cb_bmc_scheduling_table, bit_value, scheduling_index);

  }

#ifdef FEATURE_NAS_REL6
  if(scheduling_msg_length > (parsing_index+CB_BMC_OFFSET_BITMAP+bitmap_octet_len))
  {
    future_extension_bitmap = new_message_description + parsing_index;
    (void) cb_bmc_parse_future_extension_bitmap(arrival_sfn,offset_ctch_bs_index,future_extension_bitmap,cb_bmc_scheduling_table,cb_bmc_scheduling_period_length);
  }
  else
  {
    MSG_HIGH_DS_0(cb_as_id,"Future extension Bitmap is not included");
  }
#endif
  cb_bmc_filter_duplicate_old_message(cb_bmc_scheduling_table, cb_bmc_scheduling_period_length);

   /*Store the Beginning and Last SFN in the Scheduling period*/
  next_first_sfn_sch_prd = cb_bmc_calculate_sfn(arrival_sfn, offset_ctch_bs_index);
  next_last_sfn_sch_prd  = cb_bmc_calculate_sfn(next_first_sfn_sch_prd, (byte)(cb_bmc_scheduling_period_length - 1));
 
  if ((next_first_sfn_sch_prd > CB_BMC_MAX_ROLLOVER_SFN) || 
      (next_last_sfn_sch_prd > CB_BMC_MAX_ROLLOVER_SFN))
  {
    MSG_ERROR_DS_0(cb_as_id,"Invalid Schedule Message");
    cb_start_umts_non_drx_mode();
#ifndef FEATURE_MODEM_HEAP
    gs_free((void *)msg_bitmap);
#else
    modem_mem_free((void *)msg_bitmap, MODEM_MEM_CLIENT_NAS);
#endif
    return ;
  }

  /*Now process schedule table*/
  cb_bmc_process_scheduling_table(basic_cbch_list, cb_bmc_scheduling_period_length, cb_bmc_scheduling_table);

  /*Free the scheduling table*/
  //gs_free((void *)scheduling_table);

  /*Free the msg bitmap*/
#ifndef FEATURE_MODEM_HEAP
  gs_free((void *)msg_bitmap);
#else
  modem_mem_free((void *)msg_bitmap, MODEM_MEM_CLIENT_NAS);
#endif
}
/*lint +e550*/


/*===========================================================================

FUNCTION    CB_BMC_GET_SKIP_LIST_LENGTH_DRX_MODE

DESCRIPTION
  This function calculates the skip list length in DRX mode..

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e550*/
word cb_bmc_get_skip_list_length_drx_mode
(
  cb_message_list_type         *msg_list,
  word                         list_size,
  dword                        *sfn_list
)
{
  byte index;
  word skip_list_length = 0;
  cb_first_tx_schedule_type *tx_ptr;
  cb_retx_schedule_type     *retx_ptr;


  if(msg_list ==NULL || sfn_list == NULL) /* LINT ERR 613 */
  {
    MSG_HIGH_DS_2(cb_as_id, "BMC: msg_list or sfn_list is  NULL, msg_list is %x and sfn_list is %x",msg_list,sfn_list);
    return skip_list_length;
  }
  for (index = 0; index < list_size; index++)
  {
    tx_ptr = msg_list->message_header.first_tx_schedule;
    while  (tx_ptr != NULL)
    {
      if ( tx_ptr->tx_fn == sfn_list[index])
      {
        retx_ptr = tx_ptr->retx_schedule;
        //skip_list_length++;
        while (retx_ptr != NULL)
        {
          skip_list_length++;          
          retx_ptr = retx_ptr->next_ptr;
        }
      }
      tx_ptr = tx_ptr->next_ptr;
    }
  }
  return skip_list_length;
}
/*lint +e550*/
/*===========================================================================

FUNCTION    CB_BMC_GENERATE_SKIP_REQUEST_DRX_MODE

DESCRIPTION
  This function generates skip request in DRX mode.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e550*/
void cb_bmc_generate_skip_request_drx_mode
(
  cb_message_list_type         *msg_list,
  word                         list_size,
  dword                        *sfn_list
)
{
  byte index;
  word skip_list_length = 0;
  dword *skip_list;
  cb_first_tx_schedule_type *first_tx_schedule;
  cb_first_tx_schedule_type *tx_ptr;
  cb_first_tx_schedule_type *tmp_tx_ptr;
  cb_retx_schedule_type     *retx_ptr;


  if(msg_list == NULL || sfn_list == NULL) /* LINT ERR 613 */
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer",0,0,0);
  }
  skip_list_length = list_size + 
            cb_bmc_get_skip_list_length_drx_mode(msg_list, list_size, sfn_list);

  /*Send the skip request to L1*/
  if ((skip_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                                      sizeof(dword)*skip_list_length)) == NULL)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to send L1 Skip Request") ;
    return ;
  }
  
  skip_list_length = 0;

  while (skip_list_length < list_size)
  {
    skip_list[skip_list_length] = sfn_list[skip_list_length];

     /*Update the message in the new message bit map entry*/
    cb_bmc_update_new_msg_bitmap_info(sfn_list[skip_list_length], FALSE, TRUE);

    skip_list_length++;
  }
  
  for (index = 0; index < list_size; index++)
  {
    first_tx_schedule = tx_ptr = msg_list->message_header.first_tx_schedule;
    while  (tx_ptr != NULL)
    {
      if ( tx_ptr->tx_fn == sfn_list[index])
      {
        cb_retx_schedule_type *tmp_retx_ptr;
        retx_ptr = tx_ptr->retx_schedule;
        //skip_list[skip_list_length] = tx_ptr->tx_fn;
        //skip_list_length++;
        while (retx_ptr != NULL)
        {
          skip_list[skip_list_length++] = retx_ptr->retx_fn;
          /*Delete the entry*/
          tmp_retx_ptr = retx_ptr;
          retx_ptr = retx_ptr->next_ptr;
          if (retx_ptr)
           retx_ptr->prev_ptr = tmp_retx_ptr->prev_ptr;

          cb_mem_free((void *)tmp_retx_ptr);
        }
        /*Free the main entry*/
        if (tx_ptr == first_tx_schedule)
        {
          msg_list->message_header.first_tx_schedule = first_tx_schedule =
             tx_ptr->next_ptr;

          if (first_tx_schedule)
          {
            first_tx_schedule->prev_ptr = NULL;
          }

          cb_mem_free((void *)tx_ptr);

          msg_list->message_header.first_tx_schedule = tx_ptr = first_tx_schedule;
        }
        else
        {
          tmp_tx_ptr = tx_ptr;
          if (tx_ptr->prev_ptr)
          {
            (tx_ptr->prev_ptr)->next_ptr = tx_ptr->next_ptr;
          }
          tx_ptr = tx_ptr->next_ptr;
          if (tx_ptr)
          {
            tx_ptr->prev_ptr = tmp_tx_ptr->prev_ptr;
          }

          cb_mem_free((void *)tmp_tx_ptr);
        }
      }
      else
      {
        tx_ptr = tx_ptr->next_ptr;
      }
    }/*while (tx_ptr)*/
  }/*for*/
  
  if (skip_list_length > 0)
    cb_bmc_send_skip_request((byte)skip_list_length,skip_list) ;

  return;
}
/*lint +e550*/
#endif /*FEATURE_UMTS_BMC_DRX_SUPPORTED*/
#endif /* #ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED */
/*===========================================================================

FUNCTION    CB_BMC_GET_NEXT_SKIP_LIST_SFN

DESCRIPTION
   This function stores the skip list from the SFN
DEPENDENCIES
  None

RETURN VALUE
  Return length of list

SIDE EFFECTS
  None

===========================================================================*/
 /*lint -e715*/
void cb_bmc_get_next_skip_list_sfn
(
 cb_message_list_type *message_list,
 byte                  skip_list_length,
 dword                *sfn_list
#ifdef FEATURE_NAS_REL6
 ,dword               data_sn
#endif
)
{
  cb_first_tx_schedule_type *first_tx_item;
  byte   skip_index = 0;
#ifdef FEATURE_NAS_REL6
  boolean  skip_sn  = TRUE;
#endif

  if(message_list == NULL)
  {
    MSG_HIGH_DS_0(cb_as_id,"There are no messages in the message list to skip");
    return;
  }

  first_tx_item = message_list->message_header.first_tx_schedule;
  while(first_tx_item != NULL)
  {
#ifdef FEATURE_NAS_REL6
    /*
    **Dont skip the slots if serial numbers are not matching and 
    **skip the consecutive frame comparisons till we get next start SN
    */
    skip_sn = cb_bmc_is_serial_number_matching(first_tx_item,skip_sn, data_sn);
     /*Now increment the count for retransmissions*/
    if(skip_sn)
#endif
    {
      /*Now get the SFN list for retransmissions*/
      cb_bmc_get_skip_retx_list(first_tx_item->retx_schedule, sfn_list, &skip_index,
                                cb_current_sfn, TRUE);
    }

    if(skip_index >= skip_list_length)
    {
      return;
    }

    first_tx_item = first_tx_item->next_ptr;
    
  }
#ifdef FEATURE_NAS_REL6
  skip_sn  = TRUE;
#endif
  first_tx_item = message_list->message_header.next_first_tx_schedule;
  while(first_tx_item != NULL)
  {
#ifdef FEATURE_NAS_REL6
    /*
    **Dont skip the slots if serial numbers are not matching and 
    **skip the consecutive frame comparisons till we get next start SN
    */
    skip_sn = cb_bmc_is_serial_number_matching(first_tx_item,skip_sn, data_sn);

    /*Now increment the count for retransmissions*/
    if(skip_sn)
#endif
    {
      /*Get the SFN list for the new transmission in the next scheduling period*/
      sfn_list[skip_index] = first_tx_item->tx_fn;
      skip_index++;

      if(skip_index >= skip_list_length)
      {
        return;
      }

      /*Now get the SFN list for retransmissions*/
      cb_bmc_get_skip_retx_list(first_tx_item->retx_schedule, sfn_list, &skip_index,
                                cb_current_sfn, FALSE);
    }
    first_tx_item = first_tx_item->next_ptr;
  }

  return ;
}
/*lint +e715*/

/*===========================================================================

FUNCTION    CB_BMC_GET_NEXT_LENGTH_TX_LIST

DESCRIPTION
   This function returns the size of list

DEPENDENCIES
  None

RETURN VALUE
  Return length of list

SIDE EFFECTS
  None

===========================================================================*/
byte cb_bmc_get_next_length_tx_list
(
 cb_message_list_type *message_list
#ifdef FEATURE_NAS_REL6
 ,dword                data_sn
#endif
)
{
  cb_first_tx_schedule_type *first_tx_item;
  byte   skip_length = 0;
#ifdef FEATURE_NAS_REL6
  boolean  skip_sn  = TRUE;
#endif

  if(message_list == NULL)
  {
    MSG_HIGH_DS_0(cb_as_id,"There are no messages in the message list to skip");
    return skip_length;
  }

  first_tx_item = message_list->message_header.first_tx_schedule;
  while(first_tx_item != NULL)
  {
#ifdef FEATURE_NAS_REL6
    /*
    **Dont skip the slots if serial numbers are not matching and 
    **skip the consecutive frame comparisons till we get next start SN
    */
    skip_sn = cb_bmc_is_serial_number_matching(first_tx_item,skip_sn, data_sn);
    /*Now increment the count for retransmissions*/
    if(skip_sn)
#endif
    {
      skip_length += cb_bmc_get_length_retx_list(first_tx_item->retx_schedule, 
                                              cb_current_sfn, TRUE);
    }
    first_tx_item = first_tx_item->next_ptr;
  }
#ifdef FEATURE_NAS_REL6
  skip_sn  = TRUE;
#endif
  first_tx_item = message_list->message_header.next_first_tx_schedule;
  while(first_tx_item != NULL)
  {
#ifdef FEATURE_NAS_REL6
    /*
    **Dont skip the slots if serial numbers are not matching and 
    **skip the consecutive frame comparisons till we get next start SN
    */
    skip_sn = cb_bmc_is_serial_number_matching(first_tx_item,skip_sn, data_sn);

    /*Increment the skip count for new transmissions in the next sch list*/
    if(skip_sn)
#endif
    {
      skip_length += cb_bmc_get_length_retx_list(first_tx_item->retx_schedule,
                                              cb_current_sfn, FALSE);
      skip_length++;
    }
     /*Now increment the count for retransmissions*/
    first_tx_item = first_tx_item->next_ptr;
  }
  return skip_length;
}

/*===========================================================================

FUNCTION    CB_BMC_SKIP_NEXT_RECEPTION
  This function generates skip for the slots which are already scheduled (retransmissions in current
  schedule period and for the transmissions and retransmissions in the next schedule period) for the
  received message

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_bmc_skip_next_reception
(
  cb_message_list_type *message_ptr
#ifdef FEATURE_NAS_REL6
  ,dword                 data_sn
#endif
)
{
  /*Skip the reception of the message id*/
  byte  skip_list_length;
  dword *skip_list;
  /*Get the number of slots scheduled for the same message */
  skip_list_length = cb_bmc_get_next_length_tx_list(message_ptr
#ifdef FEATURE_NAS_REL6
                                                 , data_sn
#endif
  );
  /*Allocate the buffer to store the skip list*/
  if (skip_list_length > 0)
  {
    if ((skip_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                                          sizeof(dword)*skip_list_length))
                                                                   == NULL)
    {
      MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to send L1 Skip Request") ;
       /*Delete Tx and Retx list*/
      cb_bmc_free_tx_list(message_ptr);

      return ;
    }

    cb_bmc_get_next_skip_list_sfn(message_ptr, skip_list_length, skip_list 
#ifdef FEATURE_NAS_REL6
                                  , data_sn
#endif
    );

    /*Delete Tx and Retx list*/
#ifdef FEATURE_NAS_REL6
    cb_bmc_sn_free_tx_list(message_ptr,data_sn );
#else
    cb_bmc_free_tx_list(message_ptr );
#endif
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
    if (bmc_active_rat == CB_UMTS_MODE)
    {
      cb_bmc_send_skip_request(skip_list_length, skip_list);
    }
    else
#endif
    {
      cb_send_skip_request(CB_SKIP_FN_LIST, skip_list_length, skip_list);
    }

  }

  return;
}
#endif /* DRX */

#ifdef FEATURE_UMTS_BMC
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
boolean cb_bmc_validate_pdu_sfn
(
  dword pdu_sfn
)
{
  boolean sfn_valid = FALSE ;

  if (pdu_sfn <= CB_BMC_MAX_ROLLOVER_SFN)
  {
    sfn_valid = (((pdu_sfn & 0xFFF) % cb_bmc_ctch_allocation_period) == 
                                                     cb_bmc_cbs_frame_offset) ;
  }

  return sfn_valid ;
} /* end of cb_bmc_validate_pdu_sfn() */

/*===========================================================================

FUNCTION    CB_BMC_GENERATE_SKIP_REQUEST_NONDRX_MODE

DESCRIPTION
  This function generates skip request in non drx mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cb_bmc_generate_skip_request_nondrx_mode
(
  word                         list_size,
  dword                       *sfn_list
)
{
  dword *skip_list;
  word  skip_index;

  if(sfn_list == NULL) /* LINT ERR 613 */
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer",0,0,0);
  }
  /*Send the skip request to L1*/
  if ((skip_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                                      sizeof(dword)*list_size)) == NULL)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to send L1 Skip Request") ;

    return ;
  }

  for(skip_index=0; skip_index<list_size; skip_index++)
  {
    skip_list[skip_index] = sfn_list[skip_index];
  }

  cb_bmc_send_skip_request((byte)list_size,skip_list) ;

  return;
}


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
)
{
#ifdef FEATURE_UMTS_BMC_DRX_SUPPORTED
  if ((msg_list != NULL) && 
        (msg_list->message_header.first_tx_schedule != NULL || 
      msg_list->message_header.next_first_tx_schedule != NULL))
  {
    cb_bmc_generate_skip_request_drx_mode(msg_list, list_size, sfn_list);
  }
  else
#endif
  {
    cb_bmc_generate_skip_request_nondrx_mode(list_size, sfn_list);
  }
  return;
}

#endif


#ifdef FEATURE_GSM_CB_DRX_SUPPORTED

/*===========================================================================

FUNCTION    CB_GET_NEW_ARRIVAL_NUMBER

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte cb_get_new_arrival_number
(
  byte *new_message_bitmap
)
{
  byte search_index, shift_index ;
  byte new_message_counter = 0 ;
 
  for(search_index=0; search_index<CB_NEW_MSG_BITMAP_LENGTH; search_index++)
  {
    if (new_message_bitmap[search_index] == 0)
    {
      continue ;
    }

    for (shift_index = 0; shift_index < 8; shift_index++)
    {
      if (((new_message_bitmap[search_index]>>shift_index)%2) == 1)
      {
        new_message_counter++ ;
      }
    }
  }

  return new_message_counter ;
}

/*===========================================================================

FUNCTION    CB_PARSES_MESSAGE_DESCRIPTION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
word cb_parses_message_description
(
  cb_message_list_type *scheduling_message_descriptor,
  byte *new_message_description,
  word parsing_index,
  cb_scheduling_table_type *scheduling_table,
  byte tx_slot_number,
  byte scheduling_period_offset,
  boolean is_new_msg,
  byte begin_slot
)
{
  dword arrival_fn = scheduling_message_descriptor->message_header.fn ;
  cb_message_list_type *new_msg_ptr=NULL;
  byte block_no;

#ifndef FEATURE_GSM_CB_DRX_SM_FORMAT_DIFFER
  /* Unscheduled schedule message : calculate FN of the first slot */
  if (begin_slot > 1)
  {
    arrival_fn = (GSM_HYPER_FRAME_LENGTH + arrival_fn - 
                    ((begin_slot-1) * CB_MULTIFRAME_LENGTH * CB_CHANNEL_CYCLE)) % GSM_HYPER_FRAME_LENGTH;
  }
#endif

  if(!(new_message_description[parsing_index]) ||
     (new_message_description[parsing_index] == CB_SCHEDULING_MESSAGE_PADDING_BYTE))
  {

    /* 
    **Received NULL message description (0x2B).
    */
    for(block_no = CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset-1);
          block_no < (CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset));block_no++)
    {
      scheduling_table[block_no].element_id = CB_NO_MESSAGE_TX ;
      scheduling_table[block_no].scheduling_element.new_tx_message.scheduled_fn = 
             cb_calculate_block_fn(cb_calculate_slot_fn(arrival_fn, tx_slot_number),
                     (byte)(block_no%CB_PAGING_MAX_BLOCKS));
    }

  }
  else if (new_message_description[parsing_index] & NEW_MESSAGE_MDT_MASK)
  {
    word message_id ;
    new_msg_ptr = (cb_message_list_type *)0xffff;

    message_id = ((word)(new_message_description[parsing_index++] & 
                                                 (~NEW_MESSAGE_MDT_MASK)))<<8 ;

    message_id |= new_message_description[parsing_index] ;

    if (cb_message_is_requested(message_id))
    {
      if (cb_find_message(scheduling_message_descriptor,message_id) == NULL)
      {
        if ((new_msg_ptr = cb_create_message_entry(scheduling_message_descriptor,
                                          message_id,
                                          0)) == NULL)
        {
          return ERROR_INDICATION ;
        }

        current_message = cb_put_message_into_list(scheduling_message_descriptor, new_msg_ptr);
        basic_cbch_list = current_message ;

      }

      for(block_no = CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset-1);
            block_no < (CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset));block_no++)
      {
      scheduling_table[block_no].element_id = 
           ((is_new_msg) ? CB_NEW_TX : CB_OLD_TX);

      scheduling_table[block_no].scheduling_element.
         new_tx_message.message_id = message_id;

      scheduling_table[block_no].scheduling_element.
         new_tx_message.scheduled_fn = 
           cb_calculate_block_fn(cb_calculate_slot_fn(arrival_fn, tx_slot_number),
                                 (byte)(block_no%CB_PAGING_MAX_BLOCKS));

      /*Add the info in new msg bit map*/
      cb_bmc_add_new_msg_entry(&curr_new_msg_bitmap, message_id,
            scheduling_table[block_no].scheduling_element.new_tx_message.scheduled_fn);
      }

    }
    else
    {
      for(block_no = CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset-1);
            block_no < (CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset));block_no++)
      {
      scheduling_table[block_no].element_id = CB_NO_MESSAGE_TX;

      scheduling_table[block_no].scheduling_element.
         new_tx_message.scheduled_fn = 
           cb_calculate_block_fn(cb_calculate_slot_fn(arrival_fn, tx_slot_number),
                                 (byte)(block_no%CB_PAGING_MAX_BLOCKS));
      }
    }

  }
  else if ((new_message_description[parsing_index] & RETX_MDT_MASK) == 0x0)
  {
    for(block_no = CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset-1);
          block_no < (CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset));block_no++)
    {
    scheduling_table[block_no].element_id = CB_RETX ;

    scheduling_table[block_no].
        scheduling_element.retx_message.original_message_offset = 
               (new_message_description[parsing_index]-scheduling_period_offset-1)*CB_PAGING_MAX_BLOCKS ;

    scheduling_table[block_no].
        scheduling_element.retx_message.scheduled_fn = 
             cb_calculate_block_fn(cb_calculate_slot_fn(arrival_fn, tx_slot_number),
                     (byte)(block_no%CB_PAGING_MAX_BLOCKS));
    }
  }
  else if (new_message_description[parsing_index] == FREE_SLOT_MDT_MASK)
  {
    for(block_no = CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset-1);
          block_no < (CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset));block_no++)
    {

    if (cb_bmc_reading_advised)
    {
      scheduling_table[block_no].element_id = CB_NEW_TX ;

      scheduling_table[block_no].
                        scheduling_element.new_tx_message.message_id = 0xFFFF ;
    }
    else
    {
      scheduling_table[block_no].element_id = CB_NO_MESSAGE_TX ;
    }

    scheduling_table[block_no].scheduling_element.new_tx_message.scheduled_fn = 
             cb_calculate_block_fn(cb_calculate_slot_fn(arrival_fn, tx_slot_number),
                     (byte)(block_no%CB_PAGING_MAX_BLOCKS));

    }
  }
  else if (new_message_description[parsing_index] == OPTIONAL_MDT_MASK)
  {
    for(block_no = CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset-1);
          block_no < (CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset));block_no++)
    {

    if (cb_bmc_reading_optional)
    {
      scheduling_table[block_no].element_id = CB_NEW_TX ;

      scheduling_table[block_no].
                          scheduling_element.new_tx_message.message_id = 0xFFFF ;
    }
    else
    {
      scheduling_table[block_no].element_id = CB_NO_MESSAGE_TX ;
    }

      scheduling_table[block_no].scheduling_element.new_tx_message.scheduled_fn = 
             cb_calculate_block_fn(cb_calculate_slot_fn(arrival_fn, tx_slot_number),
                     (byte)(block_no%CB_PAGING_MAX_BLOCKS));

    }
  }
  else
  {
    /* 
    ** Unsupported message description, skip this message description element
    */
    MSG_ERROR_DS_0(cb_as_id,"CB DRX: Unsupported message description");
  }

  parsing_index++;

  MSG_HIGH_DS_3(cb_as_id,"CB DRX:MDT- MSG Type=0x%0x(%d) at FN=%lu",
          ((new_msg_ptr == NULL) ? (new_message_description[parsing_index-1]) : (new_message_description[parsing_index-2])),
           scheduling_table[CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset-1)].element_id,
           scheduling_table[CB_PAGING_MAX_BLOCKS*(tx_slot_number-scheduling_period_offset-1)].scheduling_element.new_tx_message.scheduled_fn);

  return parsing_index ;
} /* end cb_parses_message_description() */

/*===========================================================================

FUNCTION    CB_CANCEL_SCEDULING

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_cancel_scheduling
(
  cb_channel_ind_type  channel_ind
)
{
  switch (channel_ind)
  {
    case CBCH :
      cb_start_gsm_non_drx_mode(CBCH) ;
      break ;
    case ECBCH :
      cb_start_gsm_non_drx_mode(ECBCH) ;
      break ;
    default:
      MSG_FATAL_DS( cb_as_id,"Illegal channel type",0,0,0) ;
  }
} /* and cb_cancel_sceduling() */

/*===========================================================================

FUNCTION    CB_PROCESS_SCHEDULING_TABLE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_process_scheduling_table
(
  cb_message_list_type *scheduling_message_descriptor,
  cb_channel_ind_type  channel_ind,
  byte scheduling_period_length,
  byte scheduling_period_offset,
  cb_scheduling_table_type *scheduling_table,
  byte begin_slot
)
{
  byte process_index ;
  byte scheduling_command_length = 0;
  dword *scheduled_fn_list ;
  cb_scheduling_req_type scheduling_req ;
  word dindex ;
  word prn_number ;

  if(scheduling_table == NULL)
  {
    MSG_HIGH_DS_1(cb_as_id, "CB DRX: scheduling_table is  NULL, scheduling_period_length is %d",scheduling_period_length);
    return;
  }

  MSG_HIGH_DS_2(cb_as_id,"CB DRX: Processing Scheduling table (%lu to %lu)", next_first_sfn_sch_prd, next_last_sfn_sch_prd);

#ifdef FEATURE_GSM_CB_DRX_SM_FORMAT_DIFFER
  process_index = 0;
#else
  /*start from begin slot*/
  process_index = (begin_slot-1)*CB_PAGING_MAX_BLOCKS;
#endif

  memset(&scheduling_req,0, sizeof(scheduling_req));
  for (;process_index<scheduling_period_length;process_index++)
  {
    if (scheduling_table[process_index].element_id != CB_NO_MESSAGE_TX)
    {
      scheduling_command_length++ ;
    }
  }

  scheduling_command_length += CB_PAGING_MAX_BLOCKS;/* for SCH msg at the end */

  if ((scheduled_fn_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                              sizeof(dword)*scheduling_command_length
                              )) == NULL)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB DRX: Not enough memory to send L1 Scheduling Request") ;

    cb_cancel_scheduling(channel_ind) ;

    return ;
  }

  scheduling_command_length = 0 ;

#ifdef FEATURE_GSM_CB_DRX_SM_FORMAT_DIFFER
  process_index = 0;
#else
  /*start from begin slot*/
  process_index = (begin_slot-1)*CB_PAGING_MAX_BLOCKS;
#endif

  for (;process_index<scheduling_period_length;process_index++)
  {
    if (scheduling_table[process_index].element_id != CB_NO_MESSAGE_TX)
    {
      switch (scheduling_table[process_index].element_id)
      {
        case CB_NEW_TX :
        case CB_OLD_TX :
          scheduled_fn_list[scheduling_command_length++] = 
              scheduling_table[process_index].scheduling_element.new_tx_message.scheduled_fn ;
          break ;
        case CB_RETX :
          scheduled_fn_list[scheduling_command_length++] = 
                scheduling_table[process_index].scheduling_element.retx_message.scheduled_fn ;
          break ;
        default:
          MSG_FATAL_DS( cb_as_id,"Illegal Scheduling Element Type ID",0,0,0) ;
      }

    }

  }


#ifdef FEATURE_GSM_CB_DRX_SM_FORMAT_DIFFER
  scheduled_fn_list[scheduling_command_length]
     = cb_calculate_slot_fn(scheduling_message_descriptor->message_header.fn,
         (byte)(scheduling_period_offset+(scheduling_period_length/CB_PAGING_MAX_BLOCKS)+1));
#else
  scheduled_fn_list[scheduling_command_length]
     = cb_calculate_slot_fn(scheduling_message_descriptor->message_header.fn,
         (byte)((scheduling_period_length/CB_PAGING_MAX_BLOCKS)+1-begin_slot+1));
#endif

  for (dindex=1; dindex < CB_PAGING_MAX_BLOCKS; dindex++)
  {
    scheduled_fn_list[scheduling_command_length + dindex]
       = cb_calculate_block_fn(scheduled_fn_list[scheduling_command_length], (byte)dindex);
  }
  scheduling_req.scheduling_period_last_fn = 
         cb_calculate_slot_fn(scheduled_fn_list[scheduling_command_length],1);
  scheduling_command_length += CB_PAGING_MAX_BLOCKS;

  scheduling_req.message_header.message_set = MS_CB_L1 ;
  scheduling_req.message_header.message_id = CB_SCHEDULING_REQ ;

  scheduling_req.channel_ind = channel_ind ;

  scheduling_req.scheduling_mode = CB_DRX_MODE ;

  scheduling_req.scheduling_descriptor.scheduling_period_length = 
                                                     scheduling_command_length ;
  scheduling_req.scheduling_descriptor.scheduling_period_fn_list = 
                                                            scheduled_fn_list ;

#ifdef FEATURE_DUAL_SIM
  scheduling_req.as_id = (sys_modem_as_id_e_type) cb_as_id;
#endif

#ifdef FEATURE_GSM_CB_PSEUDO_DRX
  /* reset pseudo drx on moving to DRX */
  cb_pseudo_drx_start_fn = BMC_INVALID_SFN;
  cb_pseudo_drx_prev_start_fn = BMC_INVALID_SFN;
  cb_pseudo_drx_offset = 0;
#endif

  PUT_IMH_LEN( sizeof(cb_scheduling_req_type) - sizeof(IMH_T),
                                           & scheduling_req.message_header ) ;

  cb_active_substate_control(CB_STD_DRX_ACTIVATED);

  MSG_HIGH_DS_0(cb_as_id,"CB DRX: Sending CB_SCHEDULING_REQ(CB_DRX_MODE) request") ;

  cb_send_event_ctch_start(CB_DRX_MODE,scheduling_req.scheduling_descriptor.scheduling_period_length);

  /*Sorting */
  scheduled_fn_list = cb_sort_list(scheduled_fn_list, scheduling_command_length);

  prn_number = ((scheduling_command_length%3) == 0) ? scheduling_command_length/3 : scheduling_command_length/3 + 1;
  MSG_HIGH_DS_1(cb_as_id,"CB DRX: Scheduling sequence length is %d :",scheduling_command_length) ;
  for (dindex=0; dindex<prn_number; dindex++)
  {
    MSG_HIGH_DS_3(cb_as_id,"CB DRX: Slot# %lu %lu %lu",
    ((scheduled_fn_list[dindex*3]-scheduling_message_descriptor->message_header.fn+GSM_HYPER_FRAME_LENGTH)%GSM_HYPER_FRAME_LENGTH)/(CB_MULTIFRAME_LENGTH*CB_CHANNEL_CYCLE),
    ((scheduled_fn_list[(dindex*3) + 1]-scheduling_message_descriptor->message_header.fn+GSM_HYPER_FRAME_LENGTH)%GSM_HYPER_FRAME_LENGTH)/(CB_MULTIFRAME_LENGTH*CB_CHANNEL_CYCLE),
    ((scheduled_fn_list[(dindex*3) + 2]-scheduling_message_descriptor->message_header.fn+GSM_HYPER_FRAME_LENGTH)%GSM_HYPER_FRAME_LENGTH)/(CB_MULTIFRAME_LENGTH*CB_CHANNEL_CYCLE)
    ) ;

    MSG_HIGH_DS_3(cb_as_id,"CB DRX: FN# %lu %lu %lu",
    scheduled_fn_list[dindex*3],
    scheduled_fn_list[dindex*3 + 1],
    scheduled_fn_list[dindex*3 + 2] 
    ) ;
    
  }

  cb_send_l1_request((void *)&scheduling_req) ;

  cb_bmc_distribute_scheduling_table(scheduling_message_descriptor,
                                    scheduling_period_length,scheduling_table) ;

} /* end cb_process_scheduling_table() */

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
)
{
  byte begin_scheduling_slot ;
  byte end_scheduling_slot ;
  byte number_of_new_messages ;
  byte number_of_other_messages ;
  byte scheduling_period_length,scheduling_period_offset = 0 ;
  cb_scheduling_table_type *scheduling_table = NULL ;
  byte scheduling_index ;
  byte tx_slot_number, last_valid_tx_slot_number = 0;
  byte *message_description ;
  word parsing_index ;
  byte *scheduling_page_buff = scheduling_message_descriptor->
         message_header.current_message_descriptor->current_page->page_buffer ;

  begin_scheduling_slot = (byte)(scheduling_page_buff[0] & 
                                           CB_BEGIN_END_SCHEDULING_SLOT_MASK) ;

  end_scheduling_slot = (byte)(scheduling_page_buff[1] & 
                                           CB_BEGIN_END_SCHEDULING_SLOT_MASK) ;

  /*
  ** From Spec 44.012 section 3.5.1.
  ** Begin Slot Number (octet 1): Message slot number, relative to the schedule period, of the message 
  ** slot following the Schedule Message. The Begin Slot Number field is coded in binary. Value range: 1 to 48.
  */

  if((begin_scheduling_slot < CB_DRX_MIN_BEGIN_SLOT_NUMBER) || (begin_scheduling_slot > CB_DRX_MAX_BEGIN_SLOT_NUMBER))
  {
    MSG_ERROR_DS_1(cb_as_id,"CB DRX: Invalid begin slot number %d", begin_scheduling_slot);
    return;
  }
  if (begin_scheduling_slot > end_scheduling_slot)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB DRX: Invalid Schedule Message -> Trashing the schedule message..");
    return;
  }

  scheduling_period_length = (end_scheduling_slot - begin_scheduling_slot + 1)
                                 * CB_PAGING_MAX_BLOCKS ;

#ifdef FEATURE_GSM_CB_DRX_SM_FORMAT_DIFFER
  scheduling_period_offset = begin_scheduling_slot - 1;
#else
  scheduling_period_offset = 0;
  if (begin_scheduling_slot > 1)
  {
    scheduling_period_length += (begin_scheduling_slot - 1)* CB_PAGING_MAX_BLOCKS ;
  }
#endif

  number_of_new_messages = cb_get_new_arrival_number(
                               scheduling_page_buff+CB_NEW_MSG_BITMAP_OFFSET) ;

  if ((scheduling_period_length/CB_PAGING_MAX_BLOCKS)>=number_of_new_messages)
  {
    number_of_other_messages = (scheduling_period_length/CB_PAGING_MAX_BLOCKS) 
                                   - number_of_new_messages ;
  }
  else
  {
    number_of_other_messages=0;
    number_of_new_messages=scheduling_period_length;
    MSG_HIGH_DS_0(cb_as_id,"CB DRX: BEGIN END SLOT VALUE and NO. of New Mesages are out of Sync");
  }

  MSG_HIGH_DS_0(cb_as_id,"CB DRX: Processing Scheduling information");
  MSG_HIGH_DS_3(cb_as_id,"CB DRX: Slots %d-%d New#%d",
    begin_scheduling_slot, end_scheduling_slot, number_of_new_messages);

#ifdef FEATURE_GSM_CB_DRX_SM_FORMAT_DIFFER
  if (cb_bmc_duplicate_schedule_message(arrival_fn, (begin_scheduling_slot*CB_PAGING_MAX_BLOCKS),
                                         scheduling_period_length+1 ))
  {
    MSG_HIGH_DS_0(cb_as_id,"CB DRX: Duplicate schedule message detected -> discarded");
    return;
  }
#endif

#ifdef FEATURE_GSM_CB_DRX_SM_FORMAT_DIFFER
  if(!cb_bmc_validate_schedule_message(arrival_fn, (begin_scheduling_slot*CB_PAGING_MAX_BLOCKS), 
                                       scheduling_period_length ))
#else
  if(!cb_bmc_validate_schedule_message(arrival_fn, CB_PAGING_MAX_BLOCKS, 
                                       scheduling_period_length ))
#endif
  {
    MSG_ERROR_DS_0(cb_as_id,"CB DRX: Schedule Message is overlapping with current sch period");
    return;
  }

  if (cb_bmc_scheduling_table)
  {
#ifndef FEATURE_MODEM_HEAP
    gs_free((void *)cb_bmc_scheduling_table);
#else
    modem_mem_free((void *)cb_bmc_scheduling_table, MODEM_MEM_CLIENT_NAS);
#endif
    cb_bmc_scheduling_table = NULL;
  }

#ifndef FEATURE_MODEM_HEAP
  if ((scheduling_table = (cb_scheduling_table_type *)gs_alloc(
                        sizeof(cb_scheduling_table_type)*
                                            scheduling_period_length)) == NULL)
#else
  if ((scheduling_table = (cb_scheduling_table_type *)modem_mem_calloc(1,
                        sizeof(cb_scheduling_table_type)*
                                            scheduling_period_length, MODEM_MEM_CLIENT_NAS)) == NULL)
#endif
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to process Scheduling Message") ;

    cb_cancel_scheduling(channel_ind) ;

    return ;
  }

  cb_bmc_scheduling_table = scheduling_table;
  cb_bmc_scheduling_period_length = scheduling_period_length;

  memset(scheduling_table, 0x00, sizeof(cb_scheduling_table_type)*scheduling_period_length);
  cb_bmc_initiate_scheduling_table(scheduling_period_length, scheduling_table) ;

#ifdef FEATURE_GSM_CB_DRX_SM_FORMAT_DIFFER
  tx_slot_number = begin_scheduling_slot ;

  last_valid_tx_slot_number = begin_scheduling_slot-1 ;
#else
  tx_slot_number = 1 ;

  last_valid_tx_slot_number = 0 ;
#endif
  message_description = 
                         scheduling_page_buff + CB_NEW_MSG_DESCRIPTION_OFFSET ;

  parsing_index = 0 ;

  MSG_HIGH_DS_3(cb_as_id,"CB DRX: msgbitmap[0-2]- 0X%02X%02X%02X",
                 *(scheduling_page_buff+CB_NEW_MSG_BITMAP_OFFSET),
                 *(scheduling_page_buff+CB_NEW_MSG_BITMAP_OFFSET+1),
                 *(scheduling_page_buff+CB_NEW_MSG_BITMAP_OFFSET+2));
  MSG_HIGH_DS_3(cb_as_id,"CB DRX: msgbitmap[3-5]- 0X%02X%02X%02X",
                 *(scheduling_page_buff+CB_NEW_MSG_BITMAP_OFFSET+3),
                 *(scheduling_page_buff+CB_NEW_MSG_BITMAP_OFFSET+4),
                 *(scheduling_page_buff+CB_NEW_MSG_BITMAP_OFFSET+5));

  for(scheduling_index=0; 
                   scheduling_index<number_of_new_messages; scheduling_index++)
  {
    /* 44.012 2.1
    The position of a CB message is called the "message slot number" of the CB message,
    and it indicates the position of the CB message within the schedule period
    */
    tx_slot_number = get_slot_number(tx_slot_number- scheduling_period_offset,
                               scheduling_page_buff+CB_NEW_MSG_BITMAP_OFFSET,
                               TRUE) + scheduling_period_offset ;

    if (tx_slot_number == 0xFF)
    {
      MSG_HIGH_DS_0(cb_as_id,"Bitmap already does not have CB slot description") ;
      tx_slot_number = last_valid_tx_slot_number;
      continue ;
    }

    if (tx_slot_number > end_scheduling_slot)
    {
      break;
    }

    /* Save last valid slot number */
    last_valid_tx_slot_number = tx_slot_number ;

    if(parsing_index >= (scheduling_message_descriptor->message_header.current_message_descriptor->current_page->page_length
                        - CB_NEW_MSG_DESCRIPTION_OFFSET-1))/*-1 for two increments corresponding to new mdt*/
    {
      MSG_ERROR_DS_0(cb_as_id,"CB DRX: Trying to read beyond the end of message description buffer");
      break;
    }

    parsing_index = cb_parses_message_description(
            scheduling_message_descriptor,message_description,
                                parsing_index,scheduling_table,tx_slot_number,
                                scheduling_period_offset,TRUE,begin_scheduling_slot) ;

    if (parsing_index == ERROR_INDICATION)
    {
      MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to process Scheduling Message") ;

      cb_cancel_scheduling(channel_ind) ;

      return ;
    }

    tx_slot_number ++ ;
  } /* End of New Message Description parsing */


  /*
  **Now search for other message descriptions (new message bitmap set to 0)
  **in the new message bit map starting from first scheduling slot
  */
#ifdef FEATURE_GSM_CB_DRX_SM_FORMAT_DIFFER
  tx_slot_number = begin_scheduling_slot;
#else
  tx_slot_number = 1;
#endif

  for(scheduling_index=0;
                   scheduling_index<number_of_other_messages; scheduling_index++)
  {
    tx_slot_number = get_slot_number(tx_slot_number - scheduling_period_offset,
                     scheduling_page_buff+CB_NEW_MSG_BITMAP_OFFSET,
                     FALSE) + scheduling_period_offset;

    if (tx_slot_number == 0xFF)
    {
      MSG_HIGH_DS_0(cb_as_id,"Bitmap already does not have CB slot description") ;
      tx_slot_number = last_valid_tx_slot_number;
      continue ;
    }

    if (tx_slot_number > end_scheduling_slot)
    {
      MSG_ERROR_DS_0(cb_as_id,"CB DRX: Trying to read beyond the end of message description buffer");
      break;
    }

    parsing_index = cb_parses_message_description(
            scheduling_message_descriptor,message_description,
                                parsing_index,scheduling_table,tx_slot_number,
                                scheduling_period_offset,FALSE,begin_scheduling_slot) ;

    if (parsing_index == ERROR_INDICATION)
    {
      MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to process Scheduling Message") ;

      cb_cancel_scheduling(channel_ind) ;

      return ;
    }

    tx_slot_number++ ;
  }

   /*Store the Beginning and Last SFN in the Scheduling period*/
#ifdef FEATURE_GSM_CB_DRX_SM_FORMAT_DIFFER
  next_first_sfn_sch_prd = cb_calculate_slot_fn(arrival_fn, begin_scheduling_slot);
  next_last_sfn_sch_prd  = cb_calculate_slot_fn(arrival_fn, (byte)(end_scheduling_slot + 1));
#else
  next_first_sfn_sch_prd = cb_calculate_slot_fn(arrival_fn, 1);
  next_last_sfn_sch_prd  = cb_calculate_slot_fn(arrival_fn, (byte)(end_scheduling_slot - begin_scheduling_slot + 2))
                              + ((CB_PAGING_MAX_BLOCKS-1)*CB_MULTIFRAME_LENGTH);
#endif

  cb_bmc_filter_duplicate_old_message(scheduling_table, scheduling_period_length);

  cb_process_scheduling_table(scheduling_message_descriptor,channel_ind,scheduling_period_length,
                              scheduling_period_offset, scheduling_table, begin_scheduling_slot) ;

  /*Add the info for next Schedule Message in new msg bit map inorder to detect dropped pdu for SM*/
  cb_bmc_add_new_msg_entry(&curr_new_msg_bitmap, 0xFFFF,
          cb_calculate_block_fn(cb_calculate_slot_fn(arrival_fn, end_scheduling_slot+1),0));
  last_curr_new_msg_bitmap->start_pdu = TRUE;
  last_curr_new_msg_bitmap->serial_number = 0;
} /* end cb_process_scheduling_message() */

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
)
{
  *scheduling_block_type = (byte)(scheduling_message_header[0] & 
                                          CB_SCHEDULING_MESSAGE_TYPE_MASK)>>6 ;

  *begin_scheduling_slot = (byte)(scheduling_message_header[0] & 
                                           CB_BEGIN_END_SCHEDULING_SLOT_MASK) ;

  *end_scheduling_slot = (byte)(scheduling_message_header[1] & 
                                           CB_BEGIN_END_SCHEDULING_SLOT_MASK) ;
} /* end cb_prelimenary_scheduling_message_parsing() */

#endif /* FEATURE_GSM_CB_DRX_SUPPORTED */

/*===========================================================================

FUNCTION    CB_CALCULATE_BLOCK_FN

DESCRIPTION
  This function calculates absolute FN based on referense FN provided by L1
  and paging block sequence number

DEPENDENCIES
  None

RETURN VALUE
  GSM FN

SIDE EFFECTS
  None

===========================================================================*/
dword cb_calculate_block_fn
(
  dword reference_fn,
  byte  block_number
)
{
  return (reference_fn + (block_number*CB_MULTIFRAME_LENGTH)) % 
                                                       GSM_HYPER_FRAME_LENGTH ;
} /* end cb_calculate_block_fn() */

/*===========================================================================

FUNCTION      CB_SKIP_NUMBER_OF_PAGINGBLOCKS

DESCRIPTION
  This function compiles list of absolute FNs are to be skiped

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_skip_number_of_pagingbloks
(
  dword received_fn,
  byte required_blocks
)
{
  byte  skip_index ;
  byte  skip_block_list [3] ;
  byte  skip_list_length = 0 ;
  dword *skip_list ;

  switch (required_blocks)
  {
    case CB_SKIP_SECOND_THIRD_FOURTH:
      skip_list_length  = 3 ;

      skip_block_list[0] = CB_SECOND_BLOCK ;
      skip_block_list[1] = CB_THIRD_BLOCK ;
      skip_block_list[2] = CB_FOURTH_BLOCK ;
      break ;
    case CB_SKIP_THIRD_FOURTH:
      skip_list_length  = 2 ;

      skip_block_list[0] = CB_THIRD_BLOCK ;
      skip_block_list[1] = CB_FOURTH_BLOCK ;
      break ;
    case CB_SKIP_FOURTH:
      skip_list_length  = 1 ;

      skip_block_list[0] = CB_FOURTH_BLOCK ;
      break ;
    case CB_SKIP_SECOND_FOURTH:
      skip_list_length  = 2 ;

      skip_block_list[0] = CB_SECOND_BLOCK ;
      skip_block_list[1] = CB_FOURTH_BLOCK ;
      break ;
    case CB_SKIP_SECOND:
      skip_list_length  = 1 ;

      skip_block_list[0] = CB_SECOND_BLOCK ;
      break ;
    case CB_SKIP_THIRD:
      skip_list_length  = 1 ;

      skip_block_list[0] = CB_THIRD_BLOCK ;
      break ;
    case CB_SKIP_SECOND_THIRD:
      skip_list_length  = 2 ;

      skip_block_list[0] = CB_SECOND_BLOCK ;
      skip_block_list[1] = CB_THIRD_BLOCK ;
      break ;
    case CB_NOTHING_TO_SKIP:
      return ;
    default:
      MSG_ERROR_DS_1(cb_as_id,"Illigal skip combination (%X)",required_blocks) ;
      return ;
  }

  if ((skip_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                                      sizeof(dword)*skip_list_length)) == NULL)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to send L1 Skip Request") ;

    return ;
  }

  for(skip_index=0; skip_index<skip_list_length; skip_index++)
  {
    skip_list[skip_index] = cb_calculate_block_fn(
                                     received_fn,skip_block_list[skip_index]) ;
  }

  cb_send_skip_request(CB_SKIP_FN_LIST, skip_list_length,skip_list) ;

} /* end cb_skip_number_of_pagingbloks */

/*===========================================================================

FUNCTION      CB_SKIP_REST_OF_PAGE

DESCRIPTION
  This function compiles list of the absolute FNs are to be skiped in order to
  skip the rest of unnecessary message reception

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void cb_skip_rest_of_page
(
  dword received_fn,
  byte received_tobe_skiped_paging_block
)
{
  switch (received_tobe_skiped_paging_block)
  {
    case CB_FIRST_PAGING_BLOCK :
      cb_skip_number_of_pagingbloks(received_fn,CB_SKIP_SECOND_THIRD_FOURTH) ;
      break ;
    case CB_SECOND_PAGING_BLOCK :
      cb_skip_number_of_pagingbloks(received_fn,CB_SKIP_THIRD_FOURTH) ;
      break ;
    case CB_THIRD_PAGING_BLOCK :
      cb_skip_number_of_pagingbloks(received_fn,CB_SKIP_FOURTH) ;
      break ;
    case CB_FOURTH_PAGING_BLOCK :
    case CB_FIRST_SCEDULING_PAGING_BLOCK:
    default: /* Nothing to do */
      break ;
  }
} /* end cb_skip_rest_of_page() */

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
void cb_skip_message_pagingbloks
(
  cb_missing_page_type *current_page_ptr,
  dword received_fn,
  byte required_blocks
)
{
  byte skip_index ;
  byte  skip_block_list [3] ;
  byte  skip_list_length = 0 ;
  dword *skip_list ;

  if(current_page_ptr == NULL) /* LINT ERR 613 */
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer",0,0,0);
  }
  /* We do not need to skip already skiped paging blocks */
  required_blocks &= (~current_page_ptr->
                     rat_page_properties.gsm_propertie.already_skiped_blocks) ;

  /* Save current skip request */
  current_page_ptr->
    rat_page_properties.gsm_propertie.already_skiped_blocks |= required_blocks ;

  switch (required_blocks)
  {
    case CB_SKIP_SECOND_THIRD_FOURTH:
      skip_list_length  = 3 ;

      skip_block_list[0] = CB_SECOND_BLOCK ;
      skip_block_list[1] = CB_THIRD_BLOCK ;
      skip_block_list[2] = CB_FOURTH_BLOCK ;
      break ;
    case CB_SKIP_THIRD_FOURTH:
      skip_list_length  = 2 ;

      skip_block_list[0] = CB_THIRD_BLOCK ;
      skip_block_list[1] = CB_FOURTH_BLOCK ;
      break ;
    case CB_SKIP_FOURTH:
      skip_list_length  = 1 ;

      skip_block_list[0] = CB_FOURTH_BLOCK ;
      break ;
    case CB_SKIP_SECOND_FOURTH:
      skip_list_length  = 2 ;

      skip_block_list[0] = CB_SECOND_BLOCK ;
      skip_block_list[1] = CB_FOURTH_BLOCK ;
      break ;
    case CB_SKIP_SECOND:
      skip_list_length  = 1 ;

      skip_block_list[0] = CB_SECOND_BLOCK ;
      break ;
    case CB_SKIP_THIRD:
      skip_list_length  = 1 ;

      skip_block_list[0] = CB_THIRD_BLOCK ;
      break ;
    case CB_SKIP_SECOND_THIRD:
      skip_list_length  = 2 ;

      skip_block_list[0] = CB_SECOND_BLOCK ;
      skip_block_list[1] = CB_THIRD_BLOCK ;
      break ;
    case CB_NOTHING_TO_SKIP:
      return ;
    default:
      MSG_ERROR_DS_1(cb_as_id,"Illigal skip combination (%X)",required_blocks) ;
      return ;
  }

  if ((skip_list = (dword *)cb_mem_alloc(CB_SKIP_SEQUENCE_ALLOC_ID,
                                      sizeof(dword)*skip_list_length)) == NULL)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Not enough memory to send L1 Skip Request") ;

    return ;
  }

  for(skip_index=0; skip_index<skip_list_length; skip_index++)
  {
    skip_list[skip_index] = cb_calculate_block_fn(
                                     received_fn,skip_block_list[skip_index]) ;
#ifdef FEATURE_GSM_CB_DRX_SUPPORTED
    cb_bmc_update_new_msg_bitmap_info(skip_list[skip_index], FALSE, TRUE);
#endif
  }

  cb_send_skip_request(CB_SKIP_FN_LIST, skip_list_length,skip_list) ;

} /* end cb_skip_message_pagingbloks() ; */

/*===========================================================================

FUNCTION    CB_SKIP_UNNECESSARY_PAGING_BLOCKS

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

cb_page_assembling_state_type cb_skip_unnecessary_paging_blocks
(
  dword received_fn,
  byte received_block_mask,
  cb_missing_page_type *current_page_ptr
)
{
  word missing_paging_blocks;

  if(current_page_ptr == NULL) /* LINT ERR 613 */
  {
    MSG_FATAL_DS( cb_as_id, "Unexpected NULL pointer",0,0,0);
  }
  missing_paging_blocks=current_page_ptr->
                      rat_page_properties.gsm_propertie.missing_paging_blocks ;

  switch (missing_paging_blocks)
  {
    case CB_MISSING_FOURTH_THIRD_SECOND :

      switch (received_block_mask)
      {
        case CB_RECEIVED_FIRST_BLOCK_MASK:
          return CB_RECEIVING_SECOND_PB_STATE ;

        case CB_RECEIVED_SECOND_BLOCK_MASK:
          return CB_RECEIVING_THIRD_PB_STATE;

        case CB_RECEIVED_THIRD_BLOCK_MASK:
          return CB_RECEIVING_FOURTH_PB_STATE;

        case CB_RECEIVED_FOURTH_BLOCK_MASK:
          return CB_RECEIVING_FIRST_PB_STATE;

        default : /* Nothing to do */
          break; 
      }
      break ;

    case CB_MISSING_FOURTH_THIRD:

      switch (received_block_mask)
      {
        case CB_RECEIVED_FIRST_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                                 current_page_ptr,received_fn,CB_SKIP_SECOND) ;

          return CB_RECEIVING_THIRD_PB_STATE ;

        case CB_RECEIVED_SECOND_BLOCK_MASK:
          return CB_RECEIVING_THIRD_PB_STATE;

        case CB_RECEIVED_THIRD_BLOCK_MASK:
          return CB_RECEIVING_FOURTH_PB_STATE;

        case CB_RECEIVED_FOURTH_BLOCK_MASK:
          return CB_RECEIVING_FIRST_PB_STATE;

        default : /* Nothing to do */
          break; 
      }
      break ;

    case CB_MISSING_FOURTH_SECOND:

      switch (received_block_mask)
      {
        case CB_RECEIVED_FIRST_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                                  current_page_ptr,received_fn,CB_SKIP_THIRD) ;
          return CB_RECEIVING_SECOND_PB_STATE;

        case CB_RECEIVED_SECOND_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                                  current_page_ptr,received_fn,CB_SKIP_THIRD) ;
          return CB_RECEIVING_FOURTH_PB_STATE ;

        case CB_RECEIVED_THIRD_BLOCK_MASK:
          return CB_RECEIVING_FOURTH_PB_STATE;

        case CB_RECEIVED_FOURTH_BLOCK_MASK:
          return CB_RECEIVING_FIRST_PB_STATE;

        default : /* Nothing to do */
          break; 
      }
      break ;


    case CB_MISSING_FOURTH:

      switch (received_block_mask)
      {
        case CB_RECEIVED_FIRST_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                           current_page_ptr,received_fn,CB_SKIP_SECOND_THIRD) ;
          return CB_RECEIVING_FOURTH_PB_STATE ;

        case CB_RECEIVED_SECOND_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                                  current_page_ptr,received_fn,CB_SKIP_THIRD) ;
          return CB_RECEIVING_FOURTH_PB_STATE;

        case CB_RECEIVED_THIRD_BLOCK_MASK:
          return CB_RECEIVING_FOURTH_PB_STATE;

        case CB_RECEIVED_FOURTH_BLOCK_MASK:
          return CB_RECEIVING_FIRST_PB_STATE;

        default : /* Nothing to do */
          break; 
      }
      break ;

    case CB_MISSING_THIRD_SECOND:

      switch (received_block_mask)
      {
        case CB_RECEIVED_FIRST_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                                 current_page_ptr,received_fn,CB_SKIP_FOURTH) ;
          return CB_RECEIVING_SECOND_PB_STATE ;

        case CB_RECEIVED_SECOND_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                                 current_page_ptr,received_fn,CB_SKIP_FOURTH) ;
          return CB_RECEIVING_THIRD_PB_STATE;

        case CB_RECEIVED_THIRD_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                                 current_page_ptr,received_fn,CB_SKIP_FOURTH) ;
          return CB_RECEIVING_FIRST_PB_STATE;

        case CB_RECEIVED_FOURTH_BLOCK_MASK:
          return CB_RECEIVING_FIRST_PB_STATE;

        default : /* Nothing to do */
          break; 
      }
      break ;

    case CB_MISSING_THIRD:

      switch (received_block_mask)
      {
        case CB_RECEIVED_FIRST_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                          current_page_ptr,received_fn,CB_SKIP_SECOND_FOURTH) ;
          return CB_RECEIVING_THIRD_PB_STATE;

        case CB_RECEIVED_SECOND_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                                 current_page_ptr,received_fn,CB_SKIP_FOURTH) ;
          return CB_RECEIVING_THIRD_PB_STATE;

        case CB_RECEIVED_THIRD_BLOCK_MASK:
          return CB_RECEIVING_FIRST_PB_STATE;

        case CB_RECEIVED_FOURTH_BLOCK_MASK:
          return CB_RECEIVING_FIRST_PB_STATE;

        default : /* Nothing to do */
          break; 
      }
      break ;

    case CB_MISSING_SECOND:

      switch (received_block_mask)
      {
        case CB_RECEIVED_FIRST_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                           current_page_ptr,received_fn,CB_SKIP_THIRD_FOURTH) ;
          return CB_RECEIVING_SECOND_PB_STATE ;

        case CB_RECEIVED_SECOND_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                           current_page_ptr,received_fn,CB_SKIP_THIRD_FOURTH) ;
          return CB_RECEIVING_FIRST_PB_STATE;

        case CB_RECEIVED_THIRD_BLOCK_MASK:
          cb_skip_message_pagingbloks(
                                 current_page_ptr,received_fn,CB_SKIP_FOURTH) ;
          return CB_RECEIVING_FIRST_PB_STATE;

        case CB_RECEIVED_FOURTH_BLOCK_MASK:
          return CB_RECEIVING_FIRST_PB_STATE;

        default : /* Nothing to do */
          break; 
      }
      break ;

      case CB_MISSING_EVERYTHING:
      case CB_MISSING_NOTHING:
        return CB_RECEIVING_FIRST_PB_STATE ;

    default:
      MSG_FATAL_DS( cb_as_id,"Illigal paging block indicator (%d)",missing_paging_blocks,0,0) ;
      /*lint -e{527} Safe to return if ERR_FATAL does not return */
      return CB_RECEIVING_FIRST_PB_STATE ;

  }

  MSG_ERROR_DS_0(cb_as_id,"CB: No action has been necessary") ;
  return CB_RECEIVING_FIRST_PB_STATE;
} /* end cb_skip_unnecessary_paging_blocks() */


#endif /* #ifdef FEATURE_GSM_CB */
