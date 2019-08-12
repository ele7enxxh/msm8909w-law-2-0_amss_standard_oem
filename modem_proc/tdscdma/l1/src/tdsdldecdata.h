#ifndef TDSDLDECDATA_H
#define TDSDLDECDATA_H

/*============================================================================
                           D L D E C . H
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dldec.h_v   1.33   11 Jul 2002 22:14:10   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdldecdata.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
03/07011     xq     creat file
===========================================================================*/
#include "tdscdma_variation.h"
#ifdef FEATURE_QSH_EVENT_METRIC
#include "tdsl1_qsh_ext.h"
#endif
#ifdef FEATURE_MCAL_TDSCDMA

#ifdef FEATURE_QSH_EVENT_METRIC

#define TDSL1DEC_GET_CRC_STATIC(channel_type,counter_type) \
  (tdsl1dec_global->tdsl1dec_trch_statis_counter.channel_type##_crc_result.crc_##counter_type##_counter) \

#define TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,counter_type) \
  (tdsl1dec_global->tdsl1dec_trch_statis_counter.channel_type##_crc_result.crc_##counter_type##_counter_previous100ms_qsh) \

#define TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,counter_type) \
  (tdsl1dec_global->tdsl1dec_trch_statis_counter.channel_type##_crc_result.crc_##counter_type##_counter_temp_qsh) \

#define TDSL1DEC_UPDATE_CRC_STATIC_QSH(channel_type) \
  if(TDSL1DEC_GET_CRC_STATIC(channel_type,total) > TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,total)) \
  { \
    TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,total) = \
      TDSL1DEC_GET_CRC_STATIC(channel_type,total) - TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,total); \
    if(TDSL1DEC_GET_CRC_STATIC(channel_type,error) > TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,error)) \
    { \
      TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,error) =TDSL1DEC_GET_CRC_STATIC(channel_type,error) - TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,error); \
    } \
    else \
    { \
      TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,error) = 0; \
    } \
  } \
  else \
  { \
    TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,total) = 0; \
    TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,error) = 0; \
  } \
  TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,total) = TDSL1DEC_GET_CRC_STATIC(channel_type,total) ; \
  TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,error) = TDSL1DEC_GET_CRC_STATIC(channel_type,error); \

#define TDSL1DEC_UPDATE_CRC_STATIC_DPA_QSH(channel_type) \
if(pdschRcv > TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,total)) \
   { \
     TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,total) = pdschRcv - TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,total); \
     if (pdschRcv - pdschCRCPass - TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,error) > 0) \
     { \
       TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,error) = pdschRcv - pdschCRCPass - TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,error); \
     } \
     else \
     { \
       TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,error) = 0; \
     } \
   } \
   else \
   { \
     TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,total) = 0; \
     TDSL1DEC_GET_CRC_STATIC_QSH(channel_type,error) = 0; \
   } \
   TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,total) = pdschRcv; \
   TDSL1DEC_GET_CRC_STATIC_TEMP_QSH(channel_type,error) = pdschRcv - pdschCRCPass; \

extern rex_timer_type tdsdldec_qsh_dl_bler_update_timer;

#endif

#define FEATURE_TDSCDMA_DL_DEBUG
#ifdef FEATURE_TDSCDMA_DL_DEBUG
typedef struct
{
  boolean chan_on_flag;
  uint32 crc_total_counter;
  uint32 crc_error_counter;
#ifdef FEATURE_QSH_EVENT_METRIC
  uint32 crc_total_counter_temp_qsh;
  uint32 crc_error_counter_temp_qsh;
  uint32 crc_total_counter_previous100ms_qsh;
  uint32 crc_error_counter_previous100ms_qsh;
#endif
}tdsl1dec_crc_statisitc_struct_type;

typedef struct
{
  uint32 TTI10_ISR_Counter;
  uint32 TTI20_ISR_Counter;
  uint32 TTI40_ISR_Counter;
  uint32 TTI80_ISR_Counter;
}tdsl1dec_isr_statistic_struct_type;

typedef struct
{
  tdsl1dec_isr_statistic_struct_type ISR_Counter;
  tdsl1dec_crc_statisitc_struct_type bch_crc_result;
  tdsl1dec_crc_statisitc_struct_type fach_crc_result;
  tdsl1dec_crc_statisitc_struct_type pch_crc_result;
  tdsl1dec_crc_statisitc_struct_type dch_crc_result;
#ifdef FEATURE_QSH_EVENT_METRIC
  tdsl1dec_crc_statisitc_struct_type pdsch_crc_result;
#endif
}tdsl1dec_trch_statistic_infor_struct_type;
#endif

#ifdef FEATURE_TDSCDMA_PLT
  typedef struct 
  {
    uint32 crc_total_num;
    uint32 crc_error_num;
  
  } tdsdldecdata_tplt_dl_status_param_type;
#endif

typedef struct
{
  /* Link for the queue routines to manage this item on the queue */
  q_link_type link;
  /* index of this element in array */
  uint8 index;

  /* information is valid or not */
  boolean valid;

  /* State of DOB handle */
  tdsl1dec_dob_status_fifo_state_enum_type state;
  /* DOB handle from MCAL */
  mcaltds_dec_dob_status_handle_type dob_handle;
} tdsl1dec_dob_status_handle_type;

/* This structure defines the information required to read the TB data */
typedef struct
{
  /* The TB index in the TB set that is to be read in the following buffer */
  uint8 tb_index;
  
  /* TB hesder info index in TB set */
  uint8 tb_hdr_info_index;
  
  /* The TB buffer pointer(s) in which to read the TB data */
  tdsl1_dl_tb_buf_multi_seg tb_data_buf;
} tsdl1dec_tb_data_read_info_struct_type;

/* This structure defines the information required to report to pccpch module */
typedef struct
{
  /*use to record cctrch index of BCH channel.*/
  uint8 cctrch_index;

  /*use to record BCH block number.*/
  uint8 bch_block_num;

  /*use to record sfn decoded by BCH block.*/
  uint16 bch_sfn;

  /*use to record crc result of this BCH block.*/
  boolean crc_status;

  /*use to record energy of BCH tbset.*/
  uint32 bch_tbset_energy_matrix;

  /*use to record wall time counter of BCH block*/
  uint32 stmr_wall_time_cnt;
} tsdl1dec_bch_status_info_struct_type;


/* This structure contains the global data */
typedef struct
{

   /* Transport block header and data read related variables */
   /* ------------------------------------------------------ */
   /* Flag to keep track of the multiple Header read events */
   boolean tdsl1dec_header_read_event_called;

   /* Flag to keep track of back-to-back data read event */
   boolean tdsl1dec_tb_data_read_event_called;

   /* the number of DCH or DSCH TrBlk and CRC errors in it */
   uint8 tdsl1dec_num_dch_dsch_tb;

   uint8 tdsl1dec_dch_dsch_crc_err_count;

#ifdef FEATURE_DL_PATH_PROFILING

   uint32 tdsl1dec_hdr_rd_idx;

   uint32 tdsl1dec_skip_hdr_idx;

   uint32 tdsl1dec_data_rd_idx;

#endif

   boolean tdsl1dec_hdr_rd_in_progress;

#ifdef FEATURE_TDSCDMA_PLT

   uint32 tplt_dl_status_total_crc_number;

   uint32 tplt_dl_status_total_error_crc;

#endif

   uint32 tdsl1dec_dob_latest_crc_status[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];



/* This array stores the status fifo information. This information is refered
   to by all other function to get the starting address of the DOB. This
   information is also used by header and data reading function to get the
   transport format Id to get the transport block size and number of transport
   blocks */

   tdsl1dec_dob_status_handle_type tdsl1dec_dob_status_handle[MCALTDS_DEC_MAX_DOB_HANDLES];

/* Q for DOB status FIFO */
   q_type tdsl1dec_dob_status_fifo_q;
   
   q_type tdsl1dec_dob_status_fifo_free_q;

/* This variable contains the CCTrCh sub state for BCH or NBCH CCTrCh type. */
   tdsdldec_bch_sub_state_enum_type tdsl1dec_cctrch_bch_sub_state[TDSDLDEC_TOTAL_CCTRCH_TABLES];

/* This variable is a structure to be passed to L2 that will use the transport
   block information passed in this and report back the transport block data
   offset to the decoder. */
   tdsl1_dl_tb_hdr_decode_struct_type tdsl1dec_tb_hdr_decode_info;

#ifdef TDSL1DEC_FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC
/* call back functions for failed block passing to higher layer info */
   TDSDLDEC_FAILED_BLK_QUERY_CB_FUNC_TYPE
tdsl1dec_failed_blk_query_cb[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];
#endif 

#ifdef FEATURE_DL_PATH_PROFILING
   mcaltds_dec_hdr_rd_isr_struct_type tdsl1dec_hdr_rd_isr[128];

   mcaltds_dec_skip_hdr_struct_type tdsl1dec_dec_skip_hdr[128];

   mcaltds_dec_data_rd_struct_type tdsl1dec_data_rd[128];
#endif

/* The DOB handles used for the header read event */
   mcaltds_dec_dob_status_handle_type tdsl1dec_dob_handles_in[MCALTDS_DEC_MAX_DOB_HANDLES];

   mcaltds_dec_dob_status_handle_type tdsl1dec_dob_handles_out[MCALTDS_DEC_MAX_DOB_HANDLES];
   
   tdsl1dec_dob_status_handle_type* tdsl1dec_dl_dob_status_proc_handle[MCALTDS_DEC_MAX_DOB_HANDLES];

/* This variable is a structure to be passed to L2 that updates the TB data
	  information to L2 */
   tdsl1_dl_tb_data_struct_type tdsdec_dl_tb_data_info;

#ifdef FEATURE_TDSCDMA_DL_DEBUG   
/* Yongh add counter to statistic CRC result of each trch.*/
   tdsl1dec_trch_statistic_infor_struct_type tdsl1dec_trch_statis_counter;
/* Yongh add counter to statistic CRC result of each trch.*/
#endif

} tdsl1dec_global_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

extern tdsl1dec_global_struct_type *tdsl1dec_global;



/* This variable is a structure to be passed to L2 that updates the TB data
   information to L2 */
// extern tdsl1_dl_tb_data_struct_type dl_tb_data_info;


/* DOB handle information */

//#ifndef TDSL1DEC_FEATURE_WCDMA_L1_DEC_ENHANCED_DATA_XFER_MODE
/* This array saves the TB data buffer pointers for the TB to be read in the
   current TB set */
//extern tsdl1dec_tb_data_read_info_struct_type dec_tb_data_read_info[TDS_L1_DEC_MAX_TB];
//#endif /* TDSL1DEC_FEATURE_WCDMA_L1_DEC_ENHANCED_DATA_XFER_MODE */

/*===========================================================================
FUNCTION tdsl1dec_free_status_handle

DESCRIPTION
This function frees the DOB status FIFO in MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_free_status_handle(
  /* status FIFO index in mDSP to be freed */
  uint8 fifo_index);

/*===========================================================================
FUNCTION tdsl1dec_dec_maint_event_handler

DESCRIPTION
  This function do periodic things that are required to be done once
  every 10 ms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_dec_maint_event_handler(void);

/*===========================================================================
FUNCTION tdsl1dec_trblk_header_read_event_handler

DESCRIPTION
  This function is a event handler function for the transport channel block
  header read event. This function is set in the initialization to be called
  periodically every radio frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_trblk_header_read_event_handler(void);


/*===========================================================================
FUNCTION tdsl1dec_read_decode_tb_header

DESCRIPTION
  This function is called from TB header read event handler. It perfoems the
  series of steps using the decoder sub module to do the following functions.
  - Get the decoder output buffer status FIFO.
  - Read the transport block set status and pass the CRC information to
    demod if needed (depends on the CCTrCh type).
  - Read the transport block header and get it decoded from MAC to get the
    transport data offset and ciphering information in each transport block.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_read_decode_tb_header(void);

/*===========================================================================
FUNCTION tdsl1dec_trblk_data_read_event_handler

DESCRIPTION
  This function is a event handler function for the transport channel block
  data read event. This function is set in the initialization to be called
  periodically every radio frame after the header read event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_trblk_data_read_event_handler(void);

/*===========================================================================
FUNCTION  tdsl1dec_bch_ctrl

DESCRIPTION
  This function allows for control of the use of the BCH. The BCH can be
  turned off, which disables the decoder events on the currently running
  timeline. It can be set to be enabled but have dec report crc status to
  demod and have the transport blocks discarded, or it can be set to have
  transport blocks delivered to MAC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The decoder module behavior with respect to handling BCH transport blocks
  may be altered.
===========================================================================*/

extern void tdsl1dec_bch_ctrl(
     /* the BCH CCTrCh table index of the BCH CCTrCh for which the BCH sub
        state is to be setup */
     uint8 cctrch_table_index,
     /* BCH usage type to be swtiched to */
     tdsdldec_bch_usage_enum_type bch_usage);

/*===========================================================================
FUNCTION  tdsl1dec_flush_all_dob_status_fifo

DESCRIPTION
  This function clear all DOB status FIFO element in decoder S/W. This
  function is intended for cleanup purpose like in prep for sleep

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The decoder module behavior with respect to handling BCH transport blocks
  may be altered.
===========================================================================*/
extern void tdsl1dec_flush_all_dob_status_fifo(void);

/*===========================================================================
FUNCTION  tdsl1dec_flush_hs_info_from_dob

DESCRIPTION
  This function scans thro' HDOB status FIFO Q and checks if it HS type and
  if HS type then if HS info table index is the one to be flushed. If all
  conditions are met then that DOB status FIO is removed from Q and is freed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_flush_hs_info_from_dob(
  /* HS table index that need to be flushed from DOB status FIFO Q */
  uint8 hs_table_index);


#ifdef TDSL1DEC_FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC
/*===========================================================================
FUNCTION  tdsdl_dec_register_failed_trblk_query_cb_func

DESCRIPTION
  This function registers call back funtion to query transport block
  size of TrCh for which TFCI or BTFD decision have failed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsdl_dec_register_failed_trblk_query_cb_func(
  /* CCTrCh id */
  uint8 cctrch_id,
  /* TrCh Id */
  uint8 trch_id,
  /* function pointer to be registered */
  TDSDLDEC_FAILED_BLK_QUERY_CB_FUNC_TYPE cb_func);

#endif /* TDSL1DEC_FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC */

#endif /* FEATURE_MCAL_WCDMA  */
extern void tdsl1dec_header_read_event_called_set(boolean hdr_rd_evt);

extern void tdsl1dec_tb_data_read_event_called_set(boolean tb_data_rd_evt);

extern uint8 tdsl1dec_num_dch_dsch_tb_get(void);

extern void tdsl1dec_num_dch_dsch_tb_set(uint8 num_dch_dsch_tb);

extern uint8 tdsl1dec_dch_dsch_crc_err_count_get(void);

extern void tdsl1dec_dch_dsch_crc_err_count_set(uint8 dch_dsch_crc_err_cnt);

#ifdef FEATURE_TDSCDMA_PLT

extern uint32 tplt_dl_status_total_crc_number_get(void);

extern void tplt_dl_status_total_crc_number_set(uint32 total_crc_number);

extern uint32 tplt_dl_status_total_error_crc_get(void);

extern void  tplt_dl_status_total_error_crc_set(uint32 total_error_crc);

#endif

extern void tdsl1dec_alloc_dynamic_mem(void);

extern void tdsl1dec_free_dynamic_mem(void);

#ifdef FEATURE_QSH_EVENT_METRIC
extern boolean tdsl1dec_get_qsh_dl_bler_metric(tdsl1_qsh_metric_dl_bler_s *dl_bler_ptr);
#endif

#endif /* DLDEC_H */
