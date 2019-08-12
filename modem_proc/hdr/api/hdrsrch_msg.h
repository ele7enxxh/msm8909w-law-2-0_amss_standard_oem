#ifndef HDRSRCH_MSG_H
#define HDRSRCH_MSG_H

/*===========================================================================
      H D R    S R C H    M E S S A G E S    D E F I N I T I O N

GENERAL DESCRIPTION
      This file contains hdr srch messages definitions

  Copyright (c) 2009 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/api/hdrsrch_msg.h#1 $

when         who     what, where, why
----------   ---     -------------------------------------------------------- 
03/13/2013   vlc     Removed RF structs from hdrsrch_lte_pilot_meas_req_msg_t.                  03/06/2013   vlc     Changed hdrsrch_lte_pilot_meas_req_msg_t.
02/19/2013   vlc     Added new parameters for Pilot Meas Req message from LTE.
12/11/2012   mbs     Added new interface to pass diversity device from LTE
03/20/2012   mbs     Added SON search changes
02/24/2012   smd     Removed feature defines from SU API header files.
01/05/2012   wsh     Added support for SVLTE+SHDR
04/27/2011   smd     IRAT L2DO updates for NikeL.
03/31/2011   smd     Added pn phase in PilotMeasResp. 
11/21/2010   pxu     Added window size of 512 in chip size enums. 
11/05/2010   smd     Added LTE bw/band/channel in PilotMeasReq.
06/02/2010   smd     Included hdrmsg.h.
04/21/2010   smd     Added support for reverse time transfer.
03/10/2010   smd     Updated message structure definitions.
03/05/2010   smd     Fixed bugs for LTE pilot measurement. 
09/02/2009   smd     Added messages for LTE to DO pilot measurement.
09/02/2009   smd     Created this file.
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "msgr_umid.h"
#include <sys.h>
#include "hdrmsg.h"

#include "sys_v.h"
#include "rfcom.h"

#include "lm_types.h"

#include "lte_irat_types.h"

#ifdef __cplusplus
}  // extern "C"
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define HDRSRCH_LTE_SECTOR_LIST_MAX_SIZE 32
  /* maximum number of pilot in one LTE pilot measurement req */

#define HDRSRCH_PILOT_MEAS_START_IMMEDIATE               0xFFFFFFFF
  /* Special value in gap to indication gap start immediately */


/*--------------------------------------------------------------------------
  Pilot Strength and Pilot phase
--------------------------------------------------------------------------*/

typedef int16 hdrsrch_pilot_phase_type;        /* Pilot PN Phase Type */
typedef int16  hdrsrch_pilot_strength_type;    /* Pilot Strength Type */

/*--------------------------------------------------------------------------
  Search Window Sizes
    From: Table 6.6.6.5-1. Search Window Sizes
--------------------------------------------------------------------------*/
typedef enum
{
  HDRSRCH_WINSIZE_4_CHIPS,
  HDRSRCH_WINSIZE_6_CHIPS,
  HDRSRCH_WINSIZE_8_CHIPS,
  HDRSRCH_WINSIZE_10_CHIPS,
  HDRSRCH_WINSIZE_14_CHIPS,
  HDRSRCH_WINSIZE_20_CHIPS,
  HDRSRCH_WINSIZE_28_CHIPS,
  HDRSRCH_WINSIZE_40_CHIPS,
  HDRSRCH_WINSIZE_60_CHIPS,
  HDRSRCH_WINSIZE_80_CHIPS,
  HDRSRCH_WINSIZE_100_CHIPS,
  HDRSRCH_WINSIZE_130_CHIPS,
  HDRSRCH_WINSIZE_160_CHIPS,
  HDRSRCH_WINSIZE_226_CHIPS,
  HDRSRCH_WINSIZE_320_CHIPS,
  HDRSRCH_WINSIZE_452_CHIPS,
  HDRSRCH_WINSIZE_512_CHIPS,

  HDRSRCH_WINSIZE_MAX_SIZE,
    /* Number of elements in WinSize Table */

  HDRSRCH_WINSIZE_DEFAULT = HDRSRCH_WINSIZE_MAX_SIZE
    /* Default window size */
}
hdrsrch_winsize_enum_type;

/* Commands the HDR SRCH module receives. */
enum
{
  HDR_SRCH_REQ_FIRST = MSGR_UMID_BASE(MSGR_HDR_SRCH, MSGR_TYPE_REQ),
  MSGR_DEFINE_UMID(HDR, SRCH, REQ, TRAN_SYS_TIME,            0x00,      hdrsrch_tran_sys_time_req_msg_t), 
  MSGR_DEFINE_UMID(HDR, SRCH, REQ, LTE_PILOT_MEAS,           0x01,      hdrsrch_lte_pilot_meas_req_msg_t),   
  MSGR_DEFINE_UMID(HDR, SRCH, REQ, LTE_PILOT_MEAS_ABORT,     0x02,      hdrsrch_lte_pilot_meas_abort_msg_t), 
  HDR_SRCH_CMD_LAST = HDR_SRCH_REQ_LTE_PILOT_MEAS_ABORT
};

/* Response of HDR SRCH  */
enum
{
  HDR_SRCH_RSP_FIRST = MSGR_UMID_BASE(MSGR_HDR_SRCH, MSGR_TYPE_RSP),
  MSGR_DEFINE_UMID(HDR, SRCH, RSP, LTE_PILOT_MEAS,             0x0, hdrsrch_lte_pilot_meas_rsp_msg_t),         
  MSGR_DEFINE_UMID(HDR, SRCH, RSP, LTE_PILOT_MEAS_ABORT,       0x1, hdrsrch_lte_pilot_meas_abort_rsp_msg_t),         
  HDR_SRCH_RSP_LAST = HDR_SRCH_RSP_LTE_PILOT_MEAS_ABORT
};

/* Indication from HDR SRCH */
enum
{
  HDR_SRCH_IND_FIRST = MSGR_UMID_BASE(MSGR_HDR_SRCH, MSGR_TYPE_IND),

  MSGR_DEFINE_UMID(HDR, SRCH, IND, RF_STATUS,               0x0, hdrsrch_rf_status_ind_msg_t),

  HDR_SRCH_IND_LAST = HDR_SRCH_IND_RF_STATUS
};

typedef struct {
  msgr_hdr_struct_type               hdr;
    /* Message header */

  uint64                    cdma_sys_time;
    /* CDMA system time paired with the above LTE OSTMR */

  uint32                    lte_ostmr;
    /* LTE OSTMR value in unit of LTE RTC counter */

} hdrsrch_tran_sys_time_req_msg_t;

typedef struct {
  msgr_hdr_struct_type                hdr;
    /* Message header */

  int32                               rot_error;
   /* rotate error in unit of PPT(parts per trillion) */
   
  rfcom_device_enum_type              rf_device;
   /* Which rf device to make measurement */

  rfcom_device_enum_type              div_device;
   /* rf div device used by RF to build script */

  lte_irat_l2x_tuneaway_param_s       l2x_tuneaway_param;
  /* LTE --> X tuneaway parameters */

  uint8                                rf_cleanup_script_idx;
    /* buffer index to store rf cleanup script */

  sys_channel_type                     channel;
    /* channel information - includes band and frequency */
       
  uint8                                sector_count;
    /* sector count - number of cells */
          
  sys_pilot_pn_type                    pn_list[ HDRSRCH_LTE_SECTOR_LIST_MAX_SIZE ];
    /* list of sectors to be searched */

  uint32                               gap_start;
    /* the starting of GAP in unit of LTE OSTMR count 
       value 0xFFFFFFFF indicates immediate state */

  uint32                               gap_length;
    /* the duration of GAP in unit of LTE OSTMR count 
       value 0xFFFFFFFF indicates infinite length */

  hdrsrch_winsize_enum_type            window_size;
    /* Search window size for this channel (Table 6.6.6.5-1) */

  boolean                              online_srch;
    /* Whether it is an online_srch */
  
  rfcom_lte_bw_type                    lte_bw;
    /* LTE bandwidth */

  uint32                               lte_band;
    /* LTE band */

  uint16                               lte_channel;
    /* LTE channel */

  lm_handle_type                       rxlm_handle_lte_0;
    /* RxLM of LTE 0 */

  lm_handle_type                       rxlm_handle_lte_1;
    /* RxLM of LTE 1 */

  lm_handle_type                       rxlm_handle_hdr_0;
    /* RxLM of HDR 0 */

    boolean                              son_search;
    /* Is SON searching enabled? */
} hdrsrch_lte_pilot_meas_req_msg_t;

/* If the abort request comes within 10ms to gap start, HDR search already calls
   RF to build up scripts. The pilot measurement won't be aborted. The gap will
   still be used for measurement. */
typedef struct
{  
  msgr_hdr_struct_type                 hdr;
    /* Message header */

} hdrsrch_lte_pilot_meas_abort_req_msg_t;

typedef enum {

  PILOT_MEAS_SUCCESS = 0,
  PILOT_MEAS_WRONG_PARAMS,
  PILOT_MEAS_NOT_SRCH_YET,
  PILOT_MEAS_SEARCH_FAIL,
  PILOT_MEAS_CONFIG_TIME_TOO_SHORT,
  PILOT_MEAS_RECV_IN_WRONG_STATE,
  PILOT_MEAS_BE_ABORTED,
  PILOT_MEAS_PENDING_REQ_EXIST,
  PILOT_MEAS_BUILD_SCRIPT_FAIL,
  PILOT_MEAS_TIME_TRAN_FAIL,
  PILOT_MEAS_NO_TRM_LOCK

} hdrsrch_lte_pilot_meas_status_type;

typedef struct {
  msgr_hdr_struct_type                 hdr;
    /* Message header */

  hdrsrch_lte_pilot_meas_status_type   status;
    /* Pilot measurement status */

  sys_channel_type                     channel;
    /* channel information - includes band and frequency */
       
  uint8                                sector_count;
    /* sector count - number of cells */

  sys_pilot_pn_type                   pn_list[ HDRSRCH_LTE_SECTOR_LIST_MAX_SIZE ];
    /* Pilot PN Offset.  Range: 0 .. 511 */

  hdrsrch_pilot_strength_type         strength_db_list[ HDRSRCH_LTE_SECTOR_LIST_MAX_SIZE ];
   /* Pilot Strength (in -0.5 dB) - no limits applied */

  hdrsrch_pilot_strength_type         strength_lin_list[ HDRSRCH_LTE_SECTOR_LIST_MAX_SIZE ];
    /* Pilot Strength - linear ecio */

  hdrsrch_pilot_phase_type            pn_phase[ HDRSRCH_LTE_SECTOR_LIST_MAX_SIZE ];
    /* Pilot Phase - in chips */

  int32                               cdma_time_delta;
    /* The delta of strongest pn peak position and rtc offset, in cdma chip rate */

} hdrsrch_lte_pilot_meas_rsp_msg_t;

typedef struct {
  msgr_hdr_struct_type                 hdr;
    /* Message header */

} hdrsrch_lte_pilot_meas_abort_rsp_msg_t;


typedef enum {

  RF_STATUS_GEN_SCRIPT_SUCCESS = 0,
  RF_STATUS_GEN_SCRIPT_FAIL,
  RF_STATUS_SAMPLE_CAPTURE_DONE,
  RF_STATUS_SAMPLE_CAPTURE_NOT_DONE,
  RF_STATUS_NO_TRM_LOCK
} hdrsrch_irat_rf_status_type;

typedef struct {
  msgr_hdr_struct_type                 hdr;
    /* Message header */

  hdrsrch_irat_rf_status_type          rf_status;
    /* rf status */
} hdrsrch_rf_status_ind_msg_t;

typedef union {
  msgr_hdr_struct_type                           hdr;
    /* message header */

  hdrsrch_tran_sys_time_req_msg_t                tran_sys_time;
    /* tran sys time request message */

  hdrsrch_lte_pilot_meas_req_msg_t               lte_pilot_meas;
    /* lte pilot measurement request message */

  hdrsrch_lte_pilot_meas_abort_req_msg_t         lte_pilot_meas_abort;
    /* lte pilot measurement abort request message */
} hdrsrch_msg_req_u;

typedef union {
  msgr_hdr_struct_type                           hdr;
    /* message header */

  hdrsrch_lte_pilot_meas_rsp_msg_t               lte_pilot_meas;
    /* lte pilot meas response message */

  hdrsrch_lte_pilot_meas_abort_rsp_msg_t         lte_pilot_meas_abort;
    /* lte pilot meas abort response message */
} hdrsrch_msg_rsp_u;

typedef union {
  msgr_hdr_struct_type                            hdr;
    /* message header */

  hdrsrch_rf_status_ind_msg_t                     rf_status;
    /* rf status indication */
} hdrsrch_msg_ind_u;

#endif /* HDRSRCH_MSG_H */

