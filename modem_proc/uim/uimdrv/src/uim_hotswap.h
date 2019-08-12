#ifndef UIM_HOTSWAP_H
#define UIM_HOTSWAP_H
/*==============================================================================
  FILE:         uim_hotswap.h

  OVERVIEW:     FIle conatins the inclusions and definitions needed for
                HOTSWAP related functions.

  DEPENDENCIES: N/A

                Copyright (c) 2014-2015 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_hotswap.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       ---------------------------------------------------------
04/27/15    ssr        Fix to stop recovery process on card removal 
12/11/14    na         Argument is corrected while invoking the macro
08/14/14    ks         Hotswap enhancement for flaky mechanical switches
06/13/14    ssr        Reduce the sample count to 0 for hotswap
01/25/14    yk         The first revision
==============================================================================*/
#define UIM_HOTSWAP_NUM_CMD_BUFS            4
/* Our sampling logic based on the number of the allowed sample
   for card detection and number of debounce retrying count for this.
   We have different number for allowed sample count for card inserted
   and card removed detection but Maximum debounce retry is same for both
   query */
/* Default value was 1, where each sample take .100 second */
#define UIM_HOTSWAP_NUM_OF_SAMPLES_TO_DETECT_CARD_REMOVAL     0
/* default value was 20,  where each sample take .100 second */
#define UIM_HOTSWAP_NUM_OF_SAMPLES_TO_DETECT_CARD_INSERTION   0
#define UIM_HOTSWAP_CARD_INSERTED_SAMPLE_PERIOD               3300 /* IN SCLKS  = .1  seconds */
#define UIM_HOTSWAP_CARD_REMOVED_SAMPLE_PERIOD                15   /* IN SCLKS  = .0005 seconds  */
/* default value is 10 */
#define UIM_HOTSWAP_MAX_DEBOUNCE_RETRY                        2
#define UIM_HOTSWAP_MAXTIME_HW_INTERRUPT                      3  /* ms */
#define UIM_HOTSWAP_MAX_CHECK_FOR_CARD_INSERTION_ON_POWER_UP  10
/* default value is 0 sec, means disabled */
#define UIM_HOTSWAP_AUX_SLOT_PERIOD                           0    /* IN Sec  */

#define UIM_DETECT_CARD_STATUS(uim_ptr) \
  UIMDRV_MSG_HIGH_0((uim_ptr->id), "Uim internal hotswap detection");\
  uim_hotswap_isr(uim_ptr, TRUE, UIM_CARD_UNCHANGED)

/* ---------------------------------------------------------------
   STRUCTURE:  UIM_ME_HOTSWAP_CONFIGURATION_TYPE

   DESCRIPTION:
    It contains the sample count for card insertion and removal and Aux timer.
----------------------------------------------------------------*/

typedef struct {
  uint16 num_of_sample_for_insertion;       /* Count of debounce samples for card insertion */
  uint16 num_of_sample_for_removal;         /* Count of debounce samples for card removal */
  uint16 maximum_debounce_retry_counter;    /* Max retry Count of debounce */
  uint16 auxiliary_period_for_card_detect; /* Auxiliary time period in second for uim internal card detection */
}uim_me_hotswap_configuration_type;

/** 
 * Send the card removed cmd to uim task.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_hotswap_send_card_removed_cmd(
  uim_instance_global_type *uim_ptr);

/** 
 * This procedure initializes the  queues and timers for UIM 
 * HOTSWAP. It should be called only once, at powerup time. 
 * 
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_init(uim_instance_global_type *uim_ptr);

/** 
 * Set hotswap signal to UIM task.
 * 
 * @param hos_new_card_status New HOTSWAP status.
 * @param uim_ptr Pointer to the global data common.
 * 
 * @return boolean Success or Fail.
 */
boolean  uim_hotswap_set_signal(
  uim_card_swap_enum_type   hos_new_card_status,
  uim_instance_global_type *uim_ptr);

/** 
 * Reset the UIM global to its intial value caused by HOTSWAP
 * and re-configure the UARTDM for HOTSWAP.
 * 
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_reset_slot(uim_instance_global_type *uim_ptr);

/** 
 * Process card inserted signal in UIM task. 
 * 
 * @param uim_ptr Pointer to the global data common. 
 * 
 * @return boolean Success or Fail.
 */
boolean uim_hotswap_process_card_inserted_sig(
  uim_instance_global_type *uim_ptr);

/** 
 * Process card removed signal in UIM task.
 * 
 * @param uim_ptr Pointer to the global data common.
 * 
 * @return boolean Success or Fail.
 */
boolean uim_hotswap_process_card_removed_sig(
  uim_instance_global_type  *uim_ptr);

/** 
 * HOTSWAP ISR for the PMIC\GPIO used to communicate with the
 * UIM. 
 * 
 * @param uim_ptr Pointer to the global data common.
 * @param hos_need_to_query Request the current status.
 * @param hos_new_card_status New HOTSWAP status.
 */
void uim_hotswap_isr(uim_instance_global_type *uim_ptr,
                     boolean                   hos_need_to_query,
                     uim_card_swap_enum_type   hos_new_card_status);

/** 
 * HOTSWAP ISR for GPIO interrupt.
 * 
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_gpio_isr(uim_instance_global_type *uim_ptr);

/** 
 * Query the real-time HOTSWAP status
 * 
 * @param uim_ptr Pointer to the global data common.
 * 
 * @return uim_card_swap_enum_type
 */
uim_card_swap_enum_type uim_hotswap_query_card_status(
  uim_instance_global_type *uim_ptr);

/** 
 * Mask the Hotswap interrupt
 * 
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_mask_interrupt(uim_instance_global_type *uim_ptr);

/** 
 * Clear the hotswap interrupt.
 * 
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_clear_interrupt(uim_instance_global_type *uim_ptr);

/** 
 * Unmask the hotswap interrupt.
 * 
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_unmask_interrupt(uim_instance_global_type *uim_ptr);

#endif /* UIM_HOTSWAP_H */
