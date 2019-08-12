#ifndef GL1_HW_LIF_H
#define GL1_HW_LIF_H
/*===========================================================================
                      GSM GPRS NPL1 FRAME LAYER HEADER FILE
DESCRIPTION
   This file defines the interfaces to the GSM/GPRS NPL1 FRAME LAYER.
   The GSM/GPRS NPL1 frame layer abstracts all hardware concepts. The frame
   layer functions typically need to be called periodically every frame.

   The implementation for the functions are distributed in several files.
   gl1_hw_lif.c           - File handling common functionality.


EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_hw_lif.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
09/10/15    jk      CR89990:Enable LIF by default for FTM Cal mode
13/01/14    jk      CR781025:WBEE threshold defined as 3.2e6
16/12/14    jk      CR753423:LIF for Idle,voice and data
===========================================================================*/

#include "geran_variation.h"
#ifdef CUST_H
#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys_type.h"  /* dBx16_T */
#include "rex.h"

#include "rfcom.h"

#include "gl1_defs.h"
#include "gl1_defs_g.h"

#include "gl1_sys_algo.h"
#include "trm.h"
#include "gl1_hw_g.h"
#include "gl1_hw.h"


#include "gl1_hw_clk_ctl_g.h"
#include "mdsp_intf_g.h"
#include "mdsp_dtm.h"

#include "geran_dual_sim.h"

#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
#include "wmgr.h"
#endif /* FEATURE_DUAL_SIM | FEATURE_TRIPLE_SIM */

#include "tcxomgr.h"

#include "slpc.h"

#include "trm.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#ifdef GERAN_L1_ENHANCED_RX

#define GL1_EFS_DEBUG_LIF_SUPPROT "/nv/item_files/gsm/gl1/lif_feature_control"

#define MAX_NUMBER_OF_RX_SLOTS           5
#define INVALID_INDEX                    971
#define GL1_LIF_WBEE_THRESHOLD           3200000
#define GL1_LIF_FREQ_OFFSET              135000

typedef struct
{
uint8 is_lif_enabled       :1;
uint8 lif_freq_offset_sign :1;
uint8 unused               :6;
}gl1_hw_lif_table_t;

typedef struct
{
uint32   WBEE;
uint32   rssi;
  int32   lif_offset_freq;
boolean  ACI400Detected;
ARFCN_T  arfcn;
int32    oobEnergy;
}gl1_lif_params_t;

typedef struct
{
uint8              NoOfSlots;
gl1_lif_params_t   gsm_lif_params[MAX_NUMBER_OF_RX_SLOTS];
}gl1_lif_metrics_t;

typedef enum
{
  GL1_MSG_CS_BCCH              = 0,
  GL1_MSG_CS_CCCH              = 1,
  GL1_MSG_CS_SACCH             = 2,
  GL1_MSG_CS_SDCCH             = 3,
  GL1_MSG_CS_TCH               = 4,
  GL1_MSG_CS_TCH_HS            = 5,
  GL1_MSG_CS_TCH_AFS           = 6,
  GL1_MSG_CS_TCH_AHS           = 7,
  GL1_MSG_PS_PTCCH             = 8,
  GL1_MSG_PS_PACCH_PDTCH       = 9,
  GL1_INVALID_CHAN             = 255
} gl1_msg_lif_chan_type;

typedef struct
{
  uint32     FN;
  gl1_msg_lif_chan_type channel_type;
  uint16     arfcn;
  uint32     rssi;
  dBx16_T    pwr_dBm_x16;
  int16      freq_offset;
  int16      timing_offset;
  uint16     snr;
  byte       gain_state;
  uint32     WBEE_Prx;
  uint32     WBEE_Drx;
  uint32     WBEE_Avg;
  uint32     WBEE_Threshold;
  byte       aci_filter_used;
  boolean    ACI400Detected;
  uint8      NoOfACI400_frame;
  int32      lif_freq_offset_frame;
  boolean    jammer_detected;
  boolean    is_lif_on;
  uint16     jdet_value;
  uint16     SawlessHLLLState;
  int32      nextLifFreq;
  int32      curLifFreq;
  int32      accISqr;
  int32      accQSqr;
  int32      accIQ;  
  int32      rsbImgAciMetric;
  int32      wbdcEstI_PRx; // Wbdc Estimation I Coeff
  int32      wbdcEstQ_PRx; // Wbdc Estimation Q Coeff
  int32      wbdcEstI_DRx; // Wbdc Estimation I Coeff
  int32      wbdcEstQ_DRx; // Wbdc Estimation Q Coeff
  uint32     unused1;
  uint32     unused2;
}gl1_log_lif_params_t;

typedef struct
{
uint8      NoOfSlots;
gl1_log_lif_params_t   gl1_log_lif_params[MAX_NUMBER_OF_RX_SLOTS];
}gl1_log_lif_metrics_t;


typedef struct
{
ARFCN_T             arfcn;
boolean             updated_cs_metrics;
uint32              fn_cs_metrics;
boolean             updated_ps_metrics;
uint32              fn_ps_metrics;
gl1_log_lif_metrics_t  gl1_log_lif_cs_metrics;
gl1_log_lif_metrics_t  gl1_log_lif_ps_metrics;
}gl1_lif_dtm_metrics_t;

typedef struct
{
uint32 gl1_lif_feature_flag;
boolean gl1_enable_lif_in_ftm_cal;
gl1_hw_lif_table_t gl1_hw_lif_table[RR_MAX_PSCAN_FREQ_SCAN];
gl1_lif_dtm_metrics_t gl1_lif_dtm_metrics;
gl1_log_lif_metrics_t  gl1_log_lif_metrics;
}gl1_hw_lif_data_t;


typedef enum {
  UPDATE_LIF_CS_METRICS,
  UPDATE_LIF_PS_METRICS,
  NO_UPDATE_LIf_METRICS
}gl1_hw_update_lif_metrics_t;

uint32 gl1_hw_get_lif_feature_flag(gas_id_t gas_id);
void gl1_hw_get_lif_status(gl1_hw_lif_table_t *gl1_hw_lif_table,uint32 index,gas_id_t gas_id);
void  gl1_hw_log_lif_metrics(gl1_log_lif_metrics_t  *log_lif_metrics,gas_id_t gas_id);
void gl1_hw_update_lif_metrics(gl1_defs_metrics_rpt *metrics_ptr,gl1_defs_rx_pkt_metrics_type **metrics_info,
                                   ARFCN_T arfcn,gl1_msg_lif_chan_type channel_type,uint8 NoOfSlots,
                                   uint32 fn,gl1_hw_update_lif_metrics_t update_metrics,gas_id_t gas_id);

void gl1_hw_populate_lif_metrics(uint32 fn,gas_id_t gas_id);
uint32 gl1_hw_get_arfcn_index(ARFCN_T arfcn, gas_id_t gas_id);
void gl1_hw_update_lif_table(uint32 index, boolean enable_lif, boolean lif_freq_offset, gas_id_t gas_id);
void gl1_hw_reset_lif_table(gas_id_t gas_id);
void gl1_hw_lif_data_init(gas_id_t gas_id);
void gl1_hw_reset_lif_dtm_metrics(gas_id_t gas_id);
void gl1_hw_efs_read_lif_feature_flag(gas_id_t gas_id);
void gl1_hw_enable_lif_in_ftm_cal(boolean enable_lif, gas_id_t gas_id);
boolean gl1_hw_get_enable_lif_in_ftm_cal(gas_id_t gas_id);

#endif /*GERAN_L1_ENHANCED_RX*/

#endif /* GL1_HW_LIF_H */


/* EOF */
