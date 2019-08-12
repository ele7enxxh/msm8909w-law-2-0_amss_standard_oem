#ifndef __ONEXL3_QSH_EXT_H__
#define __ONEXL3_QSH_EXT_H__
/*===========================================================================

         I N T E R F A C E   W I T H   Q S H    M O D U L E

DESCRIPTION
  This contains all the data and API declarations for the interface with 
  QSH (Qualcomm Sherlock Holmes) module.

  Copyright (c) 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/onex_voice_adapt_if.h   1.8   10 Mar 2015 13:34:04   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/onexl3_qsh_ext.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/03/16   ppr     QSH:Generic Event changes
08/02/16   ppr     QSH:Enable event diagnostic data to QSH
01/27/16   agh     Initial Revision

  ============================================================================*/

/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/
#include "comdef.h"    /* Definition for basic types and macros */
#include "qsh.h"


/* MACRO DEFINITIONS */

#define ONEXL3_QSH_MAJOR_VER 1
#define ONEXL3_QSH_MINOR_VER 2

/* Enumeration for the type of Event sent by 1X
 * 1. Please note that the ENUM order should not be changed and the order
 * of events should be same across all the PL's.
 * 2. New event should be added at the end whenever required to keep the same 
 * order of events across all the PL's */
typedef enum {
  ONEXL3_QSH_EVENT_NONE = -1,

  ONEXL3_QSH_EVENT_FIELD_DEBUG_ANY_FAILURE = 0, 
    /* Generic Event, if this event is enabled then all the failure events
     * are to reported QSH even though individual events are not enabled */
  
  ONEXL3_QSH_EVENT_L1_TIMING_ERR = 1,     /* Timing Error */
  ONEXL3_QSH_EVENT_L1_TUNE_FREQ_ERR = 2,  /* RF Tune Frequency Error */
  ONEXL3_QSH_EVENT_L1_PAGE_MISS = 3,      /* Page Miss */
  ONEXL3_QSH_EVENT_L1_ASDIV_SWITCH = 4,   /* ASDIV ping pongs */
  ONEXL3_QSH_EVENT_L1_EMPTY_QTA = 5, /* Page decode miss due to delay in rel lock */
  ONEXL3_QSH_EVENT_L1_TRM_LOCK_WAIT_TIMEOUT = 6, /* TRM Lock wait time out */
  ONEXL3_QSH_EVENT_L2_PAGE_DEC_FAIL = 7,  /* Continous 3 Page decode miss */
  ONEXL3_QSH_EVENT_L2_HIGH_RXAGC_DELTA = 8, /* High Rx AGC delta b/w Pri and Sec */
  ONEXL3_QSH_EVENT_L3_OOS = 9, /* Idle state, Bad Channel Conds/Fade Scenarios */
  ONEXL3_QSH_EVENT_L3_RLF = 10, /* Traffic, Radio Link Failure due to Fade Scenarios */
  ONEXL3_QSH_EVENT_L3_ACCESS_FAIL = 11, /* Access state, access attempt failed */
  ONEXL3_QSH_EVENT_L3_INTER_FREQ_HO_FAIL = 12, /* Inter Freq HO Failure */
  ONEXL3_QSH_EVENT_L3_ORIG_FAIL = 13, /* Origination Failure */
  ONEXL3_QSH_EVENT_L3_L21X_REDIR_FAIL = 14, /* LTE to 1X Redir Failure */
  ONEXL3_QSH_EVENT_L3_L21X_RESEL_FAIL = 15, /* LTE to 1X Resel Failure */
  ONEXL3_QSH_EVENT_L3_1X2L_RESEL_FAIL = 16, /* 1X to LTE Resel Failure */

  /* New event types to be appended here whenever required */

  ONEXL3_QSH_EVENT_MAX
}onexl3_qsh_event_e;

/* Enumeration for the type of Metrics reported by 1X_CP */
typedef enum {
  ONEXL3_QSH_METRIC_NONE = -1,    
  
  ONEXL3_QSH_METRIC_CELL_INFO = 0,  
  /* This metric is reported on 1X cell change and is not periodic */

  ONEXL3_QSH_METRIC_RF_PARAMS = 1,
  /* This metric is reported periodically every 200 ms */
  
  ONEXL3_QSH_METRIC_MAX
}onexl3_qsh_metric_e;

/* Structures to hold metrics to be reported to QSH module */

/* CDMA cell info to be reported to QSH on cell change */
typedef struct
{    
  qsh_metric_hdr_s hdr;  /* This header has the timestamp (uint16) */
  uint8  cfg_msg_seq;    /* Configuration message sequence number */
  uint8  band_class;     /* Band class of current cell */
  uint16 freq;           /* Downlink Frequency */
  uint16 pilot_pn;       /* Pilot PN of current channel */
  uint16 base_id;        /* Base station identification */
  uint16 reg_zone;       /* Registration zone */
  uint16 sid;            /* System identification */
  uint16 nid;            /* Network identification */
  
} onexl3_qsh_metric_cell_info_s;

/* CDMA RF parameters to be reported periodically */
typedef struct
{
  qsh_metric_hdr_s  hdr; /* This header has the timestamp (uint16) */
  uint16 rssi0; /* RSSI on primary antenna (in dBm) */ 
  uint16 rssi1; /* RSSI on diversity antenna (in dBm) */
  uint16 ecio0; /* EcIo on primary antenna (in dB) */
  uint16 ecio1; /* EcIo on diversity antenna (in dB) */
  uint16 rssi_comb; /* Combined RSSI value */
  uint16 ecio_comb; /* Combined EcIo value */
  int16  tx_pwr;    /* Tx AGC value (in dB) equivalent to 0x119D log packet */
  uint32 l1_state;  /* Current state of 1X-SRCH */
  
} onexl3_qsh_metric_rf_params_s;

#endif /* __ONEXL3_QSH_EXT_H__ */
