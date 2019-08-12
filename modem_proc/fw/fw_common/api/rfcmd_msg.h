 /*!
  @file
  rfcmd_msg.h

  @brief
  Generic interface RF commands to FW

*/

/*===========================================================================

  Copyright (c) 2012-2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_common/api/rfcmd_msg.h#1 $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/13   SYH     Add parameters in txagc override payload for immediate txagc override
                   support
05/22/13   BW      Echo back ASD script token to RFSW through RFCMD message
11/08/12   SYH     Add txlm static setting buffer id to get dm computing parameters 
11/07/12   SYH     Add txagc delay parameter for computing all DM blocks
10/30/12   nbb     Request TQ pair instead of individual TQ 
10/18/12   nbb     Sample capture response messages
09/28/12   nbb     Reinclude fw_rf_irat_intf.h
08/17/12   nbb     Added TXAGC override command
08/15/12   nbb     Increase bitwidth of start_addr and num_words in TXC_TABLE_WRITE
08/01/12   nbb     Add response for free TXC banks
07/30/12   nbb     Messages for request/ free TQ from RFSW
07/19/12   nbb     Added support for event interface and DPD load table
06/15/12   nbb     Initial version with dummy messages
===========================================================================*/

#ifndef RFCMD_MSG_H
#define RFCMD_MSG_H

#include "msgr.h"
#include "fw_rf_irat_intf.h"
#include "fw_rf_common_intf.h"

/*===========================================================================

      Macros

===========================================================================*/
/*! @brief Macro containing header and payload */
#define RFCMD_INTF_REQ_MSG( MSG_NAME )                 \
typedef struct {                                   \
  msgr_hdr_s                        msg_hdr;       \
  rfcmd_##MSG_NAME##_payload        msg_payload;   \
} rfcmd_##MSG_NAME##_t

/*! @brief Macro containing header and response */
#define RFCMD_INTF_RSP_MSG( MSG_NAME )                 \
typedef struct {                                   \
  msgr_hdr_s                        msg_hdr;       \
  rfcmd_##MSG_NAME##_rsp            msg_rsp;   \
} rfcmd_##MSG_NAME##_rsp_t


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief RFA msgr with FW module */
  /* XXX --- confirm that no L1 uses 0x80 */
#define MSGR_RFA_FW         MSGR_TECH_MODULE( MSGR_TECH_RFA, 0x80 )

/*! @brief Maximum number of continuous chunks of memory written while 
  writing DPD tables. If RF has to */
#define RFCMD_MAX_NUM_CONT_DPD_TABLE_WRITES      12
/*! @brief Structure for setting FTM mode */
typedef struct
{
  uint32 ftm_mode;
} rfcmd_set_ftm_mode_payload;

RFCMD_INTF_REQ_MSG(set_ftm_mode);

/*! @brief Task Q index of dedicated CCS RF TQ for RF SW to 
  send async commands (via events on TQ) to RF */
#define RFCMD_DEDICATED_TQ_IDX 0

/*! @brief Task Q index 0 of best effort TQ (RF SW uses it 
  if available, else defaults to dedicated TQ) */
#define RFCMD_BEST_EFFORT_TQ_0_IDX 1

/*! @brief Task Q index 1 of best effort TQ (RF SW uses it 
  if available, else defaults to dedicated TQ) */
#define RFCMD_BEST_EFFORT_TQ_1_IDX 2

/*! @brief Best effort TQ pair */
#define RFCMD_BEST_EFFORT_TQ_PAIR 3

/*! @brief Number of shared TQ */
#define RFCMD_BEST_EFFORT_NUM_TQ   2

/*! @brief NUmber of TQ resources (dedidated TQ and pairs) */
#define RFCMD_NUM_SHARED_DED_TQ_PAIRS 3
/*! @brief Structure for requesting TQ */
typedef struct
{
  /*! Requested TQ IDX */
  uint32 tq_idx;
} rfcmd_request_tq_payload;

RFCMD_INTF_REQ_MSG(request_tq);

/*! @brief Structure for requesting TQ */
typedef struct
{
  /*! Freed TQ IDX */
  uint32 tq_idx;
} rfcmd_free_tq_payload;

RFCMD_INTF_REQ_MSG(free_tq);

/*! @brief Structure for sending event over RF app */
typedef struct
{
  /*! @brief Time at which to send the script */
  uint32 ustmr_time;
  /*! @brief Event will lie in IRAT buffer, index containing the event */
  uint8 buf_idx;
  /*! @brief TQ index to use */
  uint8 tq_idx;
} rfcmd_send_event_payload;

RFCMD_INTF_REQ_MSG(send_event);
/*! @brief Structure for Shutting RF thread */
typedef struct
{
  uint32 shutdown;
} rfcmd_shutdown_payload;

RFCMD_INTF_REQ_MSG(shutdown);

/*! @brief Structure for TXC banks requeuest */
typedef struct
{
  /*! @brief Sequence ID for this message */
  uint32 seq_id;
  union
  {
    /*! @brief Mask of banks freed by RF for DPD tables */
    uint16 bank_mask;
    /*! @brief Interface to be removed (kept for compilation) */
    uint16 num_banks;
  };
} rfcmd_req_txc_banks_payload;

RFCMD_INTF_REQ_MSG(req_txc_banks);

/*! @brief Structure for TXC banks free */
typedef struct
{
  /*! @brief Sequence ID for this message */
  uint32 seq_id;
  /*! @brief Mask of banks freed by RF for DPD tables */
  uint16 bank_mask;
} rfcmd_free_txc_banks_payload;

RFCMD_INTF_REQ_MSG(free_txc_banks);

/*! @brief Information regarding one continuous block of data to be 
  written to the TXC tables */
typedef struct 
{
  /*! @brief TXC bank ID to write data */
  uint8 bank_id;
  /*! @brief Start addr within bank (0-255) */
  uint16 start_addr;
  /*! @brief Number of words to write in a bank (if the table is 16 bit value, 
    RF should still send the writes as 32 bit writes) */
  uint16 num_words;
  /*! @brief Virtual address from where to copy table to TXC memory */
  uint32 *data_phys_addr;
} rfcmd_txc_memory_write_block_t;
/*! @brief Structure for DPD table writes */
typedef struct
{
  /*! @brief Sequence ID for this message */
  uint32 seq_id;
  /*! @brief Number of TXC blocks to write */
  uint16 num_cont_blocks;
  /*! @brief Information regarding that block */
  rfcmd_txc_memory_write_block_t mem_block[RFCMD_MAX_NUM_CONT_DPD_TABLE_WRITES];
} rfcmd_dpd_table_write_payload;

RFCMD_INTF_REQ_MSG(dpd_table_write);

/*! @brief Structure for DPD table writes response */
typedef struct
{
  /*! @brief Sequence ID for this message */
  uint32 seq_id;
} rfcmd_dpd_table_write_rsp;

RFCMD_INTF_RSP_MSG(dpd_table_write);

/*! @brief LABELS of INVALID ARGUMENTS */
#define DP_ENV_SCALE_INVALID 0xFFFFFFFF
#define DP_IQ_GAIN_INVALID 0xFFFFFFFF
#define BANK_ID_INVALID 255
#define TXAGC_MAX_DELAY_ABS 1000

/*! @brief Enum for Power tracking mode */
typedef enum 
{
  /*! @brief APT mode */
  RFCMD_XPT_MODE_APT,
  /*! @brief EPT (DPD) mode */
  RFCMD_XPT_MODE_EPT,
  /*! @brief Envelope tracking mode */
  RFCMD_XPT_MODE_ET,
  /*! @brief INVALID power tracking mode */
  RFCMD_XPT_MODE_INVALID = -1
} rfcmd_xpt_mode_e;

/*! @brief Table size for AMAM/ AMPM/ EPT can be 
  16, 32, 64, 128, 256 32-bit entries */
typedef enum
{
  RFCMD_XPT_BANK_256_WORDS_FVAL,
  RFCMD_XPT_BANK_128_WORDS_FVAL,
  RFCMD_XPT_BANK_64_WORDS_FVAL,
  RFCMD_XPT_BANK_32_WORDS_FVAL,
  RFCMD_XPT_BANK_16_WORDS_FVAL,
  RFCMD_XPT_BANK_NUM_SIZES
} rfcmd_xpt_bank_size_e;
/*! @brief Structure for sending size/bank/base for AMAM/ AMPM/ EPT to FW */
typedef struct
{
  /*! @brief Base addr (offset within bank) */
  uint16 base;
  /*! @brief Number of 16bit writes in table (this is per HWIO) */
  rfcmd_xpt_bank_size_e bank_size;
  /*! @brief Bank ID */
  uint8 bank_id;
} rfcmd_xpt_table_info_t;
/*! @brief Structure for DPD table writes */
typedef struct
{
  /*! @brief Chain to overrider TXAGC on */
  uint8 tx_chain;
  /*! @brief Sequence ID for this message */
  uint32 seq_id;
  /*! @brief DP IQ gain (HW has 16 bit value -- allow to expand) */
  uint32 dp_iq_gain;
  /*! @brief DP envelope scale (HW has 16 bit value -- allow to expand) */
  uint32 dp_env_scale;
  /*! @brief Current txlm buffer id, used for get current dm rate info */
  uint32 curr_txlm_buf_id;
  /*! @brief maximum number of samples in current SF */
  uint32 max_samples_per_sf;
  /*! @brief total amount of delay from all DM blocks, unit in ns, Q21 format, 
             and precision to 0.1ns */
  int32 delay;
  /*! @brief flag that triggers immediately activation of TxC settings, used by LTE only */
  boolean is_imm_act;
  /*! @brief XPT mode */
  rfcmd_xpt_mode_e xpt_mode;
  /*! @brief AMAM table information */
  rfcmd_xpt_table_info_t amam_table;
  /*! @brief AMPM table information */
  rfcmd_xpt_table_info_t ampm_table;
  /*! @brief EPDT (Envelope path detrough table) information */
  rfcmd_xpt_table_info_t epdt_table;
} rfcmd_txagc_override_payload;

RFCMD_INTF_REQ_MSG(txagc_override);

/*! @brief Structure for DPD table writes response */
typedef struct
{
  /*! @brief Sequence ID for this message */
  uint32 seq_id;
} rfcmd_txagc_override_rsp;

RFCMD_INTF_RSP_MSG(txagc_override);

/*! @brief Structure for ADC VCM CAL */
typedef struct
{
  /*! @brief RxLM buffer ID */
  uint32 buf_id;
  /*! @brief Capture time in usec */
  uint32 capture_time_us;
  /*! @brief Sequence number */
  uint32 snum;
} rfcmd_adc_vcm_cal_payload;

RFCMD_INTF_REQ_MSG(adc_vcm_cal);

/*! @brief Structure to send processing done for ADC VCM CAL */
typedef struct
{
  /*! @brief TRUE means ADC_VCM_CAL success */
  uint32 adc_vcm_cal_success;
  /*! @brief Sequence number given by RFSW */
  uint32 snum;
  /*! @brief WBDC_EST2_CURR_I value */
  uint32 wbdc_est2_curr_i;
  /*! @brief WBDC_EST2_CURR_Q value */
  uint32 wbdc_est2_curr_q;
} rfcmd_adc_vcm_cal_done_rsp;

RFCMD_INTF_RSP_MSG(adc_vcm_cal_done);

/*! @brief Structure for TXC bank request response */
typedef struct
{
  /*! @brief Sequence ID for this message */
  uint32 seq_id;
  /*! @brief Bank mask of allocated banks */
  uint16 allocated_bank_mask;
} rfcmd_alloc_txc_banks_rsp;

RFCMD_INTF_RSP_MSG(alloc_txc_banks);

/*! @brief Structure for freeing TXC bank response */
typedef struct
{
  /*! @brief Sequence ID for this message */
  uint32 seq_id;
} rfcmd_free_txc_banks_rsp;

RFCMD_INTF_RSP_MSG(free_txc_banks);

/*! @brief Msgr indication of success for TQ allocation */
#define RFCMD_REQ_TQ_SUCC 0
/*! @brief Msgr indication of failure for TQ allocation */
#define RFCMD_REQ_TQ_FAIL 1

/*! @brief Structure to send response for TQ allocation request */
typedef struct 
{
  /*! @brief REquested TQ */
  uint8 tq_idx;
  /*! @brief Succress or Failure indication */
  uint8 ind;
} rfcmd_request_tq_rsp;

RFCMD_INTF_RSP_MSG(request_tq);

/*! @brief Msgr indication of success for TQ free */
#define RFCMD_FREE_TQ_SUCC 0
/*! @brief Msgr indication of failure for TQ free */
#define RFCMD_FREE_TQ_FAIL 1


/*! @brief Structure to send response for TQ Free message */
typedef struct 
{
  /*! @brief Freed TQ */
  uint8 tq_idx;
  /*! @brief Succress or Failure indication */
  uint8 ind;
} rfcmd_free_tq_rsp;

RFCMD_INTF_RSP_MSG(free_tq);

/* Sample capture message is sent to each tech separately today
   (Please refer to common structure to be sent in SAPT command
   in fw_dpd_intf.h). Only the results are sent back via
   a common message to RF SW */

/*! @brief Structure to send capture done for a single capture */
typedef struct
{
  /*! @brief ID of capture */
  uint32 seq_id;
  /*! @brief Buffer ID for capture done */
  uint8 capt_buf_id;
} rfcmd_sample_capt_done_rsp;

RFCMD_INTF_RSP_MSG(sample_capt_done);

/*! @brief Structure to send processing done for a single capture */
typedef struct
{
  /*! @brief ID of capture */
  uint32 seq_id;
  /*! @brief Buffer ID for capture done, indicates which capture buffer produced results 
    XXX - Can be removed if required */
  uint8 capt_buf_id;
  /*! @brief Buffer ID for processing done */
  uint8 proc_buf_id;
} rfcmd_xpt_cal_proc_done_rsp;

RFCMD_INTF_RSP_MSG(xpt_cal_proc_done);

/*! @brief Structure to send qtf token for antenna tuner script */
typedef struct
{
  /*! @brief token of antenna tuner script */
  uint32 token;
} rfcmd_qtf_token_rsp;

RFCMD_INTF_RSP_MSG(qtf_token);

/*! @brief Structure to send qtf CL measurement for antenna tuner script */
typedef struct
{
  /*! @brief token of antenna tuner script */
  uint32 token;
  /*! @brief QTF tuner update status */
  fw_rf_qtf_ant_tuner_status_type status;
  /*! @brief TX AGC power level when event was triggerred or timeout */
  int32 tx_agc_level; 
} rfcmd_qtf_cl_meas_rsp;

RFCMD_INTF_RSP_MSG(qtf_cl_meas);

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
/*! @brief Structure to send asd token for antenna switch diversity command script */
typedef struct
{
  /*! @brief token of antenna tuner script */
  uint32 token;
} rfcmd_asd_token_rsp;

RFCMD_INTF_RSP_MSG(asd_token);
#endif

/*! @brief Structure to send processing done for all captures */
/* MSGR enum */
enum {
  /* Command messages */
  MSGR_DEFINE_UMID( RFA, FW, CMD,  SET_FTM_MODE,      0x00, rfcmd_set_ftm_mode_t  ),
  MSGR_DEFINE_UMID( RFA, FW, CMD,  SEND_EVENT,        0x01, rfcmd_send_event_t    ),
  MSGR_DEFINE_UMID( RFA, FW, CMD,  REQ_TXC_BANKS,     0x02, rfcmd_req_txc_banks_t  ),
  MSGR_DEFINE_UMID( RFA, FW, CMD,  FREE_TXC_BANKS,    0x03, rfcmd_free_txc_banks_t  ),
  MSGR_DEFINE_UMID( RFA, FW, CMD,  DPD_TABLE_WRITE,   0x04, rfcmd_dpd_table_write_t ),
  MSGR_DEFINE_UMID( RFA, FW, CMD,  REQUEST_TQ,        0x05, rfcmd_request_tq_t ),
  MSGR_DEFINE_UMID( RFA, FW, CMD,  FREE_TQ,           0x06, rfcmd_free_tq_t ),
  MSGR_DEFINE_UMID( RFA, FW, CMD,  TXAGC_OVERRIDE,    0x07, rfcmd_txagc_override_t ),
  MSGR_DEFINE_UMID( RFA, FW, CMD,  ADC_VCM_CAL,       0x08, rfcmd_adc_vcm_cal_t ),

  /* Internal commands */
  MSGR_DEFINE_UMID( RFA, FW, CMDI, SHUTDOWN,          0x00, rfcmd_shutdown_t    ),

  /* Response messages */
  MSGR_DEFINE_UMID( RFA, FW, RSP,  ALLOC_TXC_BANKS,   0x00, rfcmd_alloc_txc_banks_rsp_t ),
  MSGR_DEFINE_UMID( RFA, FW, RSP,  DPD_TABLE_WRITE,   0x01, rfcmd_dpd_table_write_rsp_t ),
  MSGR_DEFINE_UMID( RFA, FW, RSP,  REQUEST_TQ,        0x02, rfcmd_request_tq_rsp_t ),
  MSGR_DEFINE_UMID( RFA, FW, RSP,  FREE_TXC_BANKS,    0x03, rfcmd_free_txc_banks_rsp_t  ),
  MSGR_DEFINE_UMID( RFA, FW, RSP,  TXAGC_OVERRIDE,    0x04, rfcmd_txagc_override_rsp_t ),
  MSGR_DEFINE_UMID( RFA, FW, RSP,  SAMPLE_CAPT_DONE,  0x05, rfcmd_sample_capt_done_rsp_t ),
  MSGR_DEFINE_UMID( RFA, FW, RSP,  XPT_CAL_PROC_DONE, 0x06, rfcmd_xpt_cal_proc_done_rsp_t ),
  MSGR_DEFINE_UMID( RFA, FW, RSP,  FREE_TQ,           0x07, rfcmd_free_tq_rsp_t ),
  MSGR_DEFINE_UMID( RFA, FW, RSP,  QTF_TOKEN,         0x08, rfcmd_qtf_token_rsp_t ),    /* QTF AOL response token message */
  MSGR_DEFINE_UMID( RFA, FW, RSP,  QTF_CL_MEAS,       0x09, rfcmd_qtf_cl_meas_rsp_t ),  /* QTF CL Measurement response message (OCCURRED or TIMTOUT) */
  MSGR_DEFINE_UMID( RFA, FW, RSP,  ADC_VCM_CAL_DONE,  0x0B, rfcmd_adc_vam_cal_done_rsp_t )
  
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  ,MSGR_DEFINE_UMID( RFA, FW, RSP,  ASD_TOKEN,         0x0A, rfcmd_asd_token_rsp_t )     /* ASD response token message */
#endif
};


#endif // RFCMD_H
