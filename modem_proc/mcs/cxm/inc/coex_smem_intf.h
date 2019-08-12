#ifndef COEX_SMEM_INTF_H
#define COEX_SMEM_INTF_H
/*!
  @file
  cxm_smem.h

  @brief
  APIs for internal WLAN communication over SMEM

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/14   tak     Added data for GSM and TDS
02/10/14   tak     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>

/*===========================================================================

                   DATA TYPES AND MACROS

===========================================================================*/

/*masks for SMP2P*/
#define CXM_SMP2P_STATE_BIT      0x00000001
#define CXM_SMP2P_TOGGLE_BIT     0x00000002
#define CXM_SMP2P_ACK_BIT        0x00000004

#define CXM_SMEM_MDM_TURN        0
#define CXM_SMEM_WCN_TURN        1
#define CXM_SMEM_FLAG_OFF        0
#define CXM_SMEM_FLAG_ON         1

/* Tech masks to be updated when more techs are added */
#define CXM_SMEM_UPDATE_TECH_LTE      0x00000001
#define CXM_SMEM_UPDATE_TECH_TDSCDMA  0x00000002
#define CXM_SMEM_UPDATE_TECH_GSM1     0x00000004
#define CXM_SMEM_UPDATE_TECH_GSM2     0x00000008
#define CXM_SMEM_UPDATE_TECH_GSM3     0x00000010

/* Message masks to be updated when more messages are added */
#define CXM_SMEM_UPDATE_MSG_SLEEP_WAKEUP  0x00000001
#define CXM_SMEM_UPDATE_MSG_FRAME_TIMING  0x00000002
#define CXM_SMEM_UPDATE_MSG_TX_ADVANCED   0x00000004
#define CXM_SMEM_UPDATE_MSG_ACT_ADVANCED  0x00000008
#define CXM_SMEM_UPDATE_MSG_HIGH_PRIO     0x00000010
#define CXM_SMEM_UPDATE_MSG_ACT_TIMELINE  0x00000020

#define CXM_SMEM_MDM_TO_WCN_SMP2P    "coex_mdm2wcn"     
#define CXM_SMEM_WCN_TO_MDM_SMP2P    "coex_wcn2mdm"    
#define CXM_SMEM_MDM_FW_TO_WCN_SMP2P "coex_fw_mdm2wcn"  

/* bit masks for FW to WCN events */
#define CXM_SMEM_FW_EVENT_TX_ACTIVE   0x00000001
#define CXM_SMEM_FW_EVENT_RX_ACTIVE   0x00000002

/* buffer size for FW messages */
#define CXM_SMEM_TDS_FW_BUF_SIZE      4
#define CXM_SMEM_GSM_FW_BUF_SIZE      4

/* number of TDS slots */
#define CXM_SMEM_NUM_TDS_SLOTS        9
#define CXM_SMEM_MAX_HIGH_PRIO_FREQ   8
#define CXM_SMEM_MAX_TIMELINE_SLOTS   32

typedef enum 
{
  CXM_SMEM_LTE_CHAN_RESERVED, 
  CXM_SMEM_LTE_CHAN_PUCCH, 
  CXM_SMEM_LTE_CHAN_PUSCH, 
  CXM_SMEM_LTE_CHAN_PRACH, 
  CXM_SMEM_LTE_CHAN_SRS, 
  CXM_SMEM_LTE_CHAN_SRS_PUCCH, 
  CXM_SMEM_LTE_CHAN_SRS_PUSCH
} cxm_smem_lte_chan_e;

typedef enum
{
  CXM_SMEM_TX_OFF,
  CXM_SMEM_TX_ON,
  CXM_SMEM_TX_ON_NO_CONFL
} cxm_smem_tx_adv_tx_e;

typedef enum 
{
  CXM_SMEM_SLOT_NONE,
  CXM_SMEM_SLOT_RX, 
  CXM_SMEM_SLOT_TX, 
  CXM_SMEM_SLOT_MON 
} cxm_smem_slot_type_e;

typedef enum 
{
  CXM_SMEM_SLOT_PRIO_LOW,   
  CXM_SMEM_SLOT_PRIO_HIGH 
} cxm_smem_slot_prio_e;

typedef struct
{
  boolean    is_waking_up; 
  uint32     duration; 
} cxm_smem_sleep_wakeup_s;

typedef struct
{
  uint8                       sfn;             
  cxm_smem_tx_adv_tx_e        transmit;        
  cxm_smem_lte_chan_e         channel_type;
  uint8                       rbs;    
  int8                        slot0_power;     
  uint8                       slot0_first_rb;  
  uint8                       slot0_last_rb;   
  int8                        slot1_power;     
  uint8                       slot1_first_rb;  
  uint8                       slot1_last_rb;   
} cxm_smem_lte_tx_adv_s;

typedef struct
{
  uint32            dl_frame_time;  
  uint32            ta;
} cxm_smem_lte_frame_timing_s;

typedef struct {
  
  /*! Frequency (kHz) */
  uint32 frequency;
  
  /*!< Tech operating bandwidth in Hz */
  uint32 bandwidth;

  /*! RX/TX/MON  */          
  cxm_smem_slot_type_e  slot_type;

  /*! Start time (in USTMR units) of the activity in the next  slot (unpadded)*/ 
  uint32 start_time;

  /*! End time (in USTMR units) of the activity in the next  slot  (unpadded)*/ 
  uint32 end_time; 
  
  /*! Relative priority of activity */
  cxm_smem_slot_prio_e  prio;

  /*! Power for slot (dBm * 10) */ 
  int32 pwr; 

} cxm_smem_timing_slot_entry; 

/*! @brief Message for Slot Level Timing Indication sent to MCS */ 
typedef struct { 

  /*!  Number of activity slots in list */ 
  uint8 num_entries;

  /*! List of slots */
  cxm_smem_timing_slot_entry slot_list[CXM_SMEM_MAX_TIMELINE_SLOTS]; 

} cxm_smem_activity_timeline_s; 

typedef struct
{
  /*! RX/TX/MON  */          
  cxm_smem_slot_type_e  slot_type;  
  /*! Tx power for the sub frame with above SFN */     
  int16                 tx_power;  
  /*! Relative priority of the TX/RX/MON activity */      
  cxm_smem_slot_prio_e  priority; 
  /*! If activity is monitoring, the freq that is being monitored, in MHz */       
  uint32                monitor_freq;    
} cxm_smem_tds_slot_info_s;

typedef struct
{
  /*! TDS subframe number*/
  uint16                    sfn;   
  /*! TDS slots */          
  cxm_smem_tds_slot_info_s  slot[CXM_SMEM_NUM_TDS_SLOTS];  
} cxm_smem_tds_act_adv_s;

/*! sent by TDS L1 every 10ms */
typedef struct
{
  /*! subframe number (0 to 8911)*/
  uint32            sfn;   
  /*! Corresponding ustmr value for above subframe*/          
  uint32            ustmr_val; 
  /*! Timing advance in microseconds. WCN can use this to calculate the UL frame timing*/      
  uint32            ta;              
} cxm_smem_tds_frame_timing_s;

/*! this message is used to inform of a high priority event and can be used in 4 ways:
1.) The start/end USTMR times are known - fill in these fields
2.) Start USTMR is know at the beginning of the event but the end is not known - fill in
    the start time and make end time 0xFFFFFFFF
3.) The end time eventually becomes known (it was not known at the beginning) - fill in 
    0xFFFFFFFF for the start time and fill the appropriate end time
4.) A valid start time has been sent (case 2) and the event ends now - fill in 
    start = end = 0
*/
typedef struct
{
  /*! Start time of the high priority event */
  uint32            start;   
  /*! End time of the high prioriy event. */
  uint32            end;
  /*! Number of high priority frequencies. */ 
  uint8             num_freqs;       
  /*! Frequency of high priority event */                 
  uint32            freq[CXM_SMEM_MAX_HIGH_PRIO_FREQ];                 
} cxm_smem_high_prio_s;

typedef struct
{
  uint32                       msg_mask;
  cxm_smem_sleep_wakeup_s      state;
  cxm_smem_lte_frame_timing_s  frame_timing;
  cxm_smem_lte_tx_adv_s        tx_adv;
  cxm_smem_high_prio_s         high_prio;
} cxm_smem_lte_params_s;

typedef struct
{
  uint32                        msg_mask;
  cxm_smem_sleep_wakeup_s       state;
  cxm_smem_activity_timeline_s  timeline;
  cxm_smem_high_prio_s          high_prio;
} cxm_smem_gsm_params_s;

typedef struct
{
  uint32                       msg_mask;
  cxm_smem_sleep_wakeup_s      state;
  cxm_smem_tds_frame_timing_s  frame_timing;
  cxm_smem_tds_act_adv_s       act_adv;
  cxm_smem_high_prio_s         high_prio;
} cxm_smem_tds_params_s;

/*! @brief Activity info from CFW. Passed on WCN */
typedef union
{
  struct
  {
    /*!< bit[0:26] - USTMR Start */
    uint32 ustmr_start : 27;
    /*!< Reserved */     
    uint32 resv1       : 2;
    /*!< bit[29] - RX slot if bit value is 1 else no RX */      
    uint32 rx	       : 1;   
    /*!< bit[30] - TX slot if bit value is 1 else no TX */   
    uint32 tx          : 1; 
    /*!< bit[31] - Check Bit 1. This value must be thesame as Check Bit 2. 
         If they are different, abort read operation and retry immediately */     
    uint32 check1      : 1;      
    /*!< bit[32:58] - USTMR Stop */
    uint32 ustmr_stop  : 27;
    /*!< Reserved */     
    uint32 resv2       : 4;
    /*!< bit[63] - Check Bit 2. This value must be thesame as Check Bit 1. 
         If they are different, abort read operation and retry immediately */      
    uint32 check2      : 1;      
  } bitmap;
  struct
  {
    uint32 word0;                     
    uint32 word1;                     
  } words;
  uint64 data;
} cxm_smem_rx_tx_activity_info_u;

typedef struct
{
  /*variables for Peterson's algorithm*/
  volatile uint16        modem_flag;
  volatile uint16        wcn_flag;
  volatile uint32        turn;

  /*mask to indicate what tech has been updated*/
  uint32                 tech_mask;

  /*L1 data data*/
  cxm_smem_lte_params_s  lte;
  cxm_smem_tds_params_s  tds;
  cxm_smem_gsm_params_s  gsm1;
  cxm_smem_gsm_params_s  gsm2;
  cxm_smem_gsm_params_s  gsm3;

  /*FW DATA
    Each FW tech contains a 'check_count' field. To read FW data, follow these steps:
    1.) Read the check_count
    2.) Read ALL entries in the buffer
    3.) Re-read the check_count
    4.) If the check_count values you read both times are thesame, accept the data
        else retry from step 1 immediately.
    5.) Sort the buffer data according to USTMR start if needed
  */
  /*TDS FW data*/
  uint32 cxm_smem_tds_check_count;
  cxm_smem_rx_tx_activity_info_u cxm_smem_tds_fw[CXM_SMEM_TDS_FW_BUF_SIZE];

  /*GSM FW data*/
  uint32 cxm_smem_gsm1_check_count;
  cxm_smem_rx_tx_activity_info_u cxm_smem_gsm1_fw[CXM_SMEM_GSM_FW_BUF_SIZE];

  /*GSM2 FW data*/
  uint32 cxm_smem_gsm2_check_count;
  cxm_smem_rx_tx_activity_info_u cxm_smem_gsm2_fw[CXM_SMEM_GSM_FW_BUF_SIZE];

  /*GSM3 FW data*/
  uint32 cxm_smem_gsm3_check_count;
  cxm_smem_rx_tx_activity_info_u cxm_smem_gsm3_fw[CXM_SMEM_GSM_FW_BUF_SIZE];
  
} cxm_smem_data_s;

#endif /* COEX_SMEM_INTF_H */
