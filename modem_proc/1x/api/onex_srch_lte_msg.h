/*!
  @file
  onex_srch_lte_msg.h

  Defines the external message interface between LTE ML1 and ONEX SRCH modules
*/
/*===========================================================================
  Copyright (c) 2009 - 2016 Qualcomm Technologies, Inc.
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
                        EDIT HISTORY FOR MODULE
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/onex_srch_lte_msg.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   pk      Added new abort state ONEX_SRCH_MEAS_SAMPLE_CAPTURED
05/26/16   pk      Ensure measurement aborts are handled correctly, when we
                   have already received pilot meas cfg rsp from FW.
12/15/14   ssh     Added L->1x measurement abort support.
10/01/14   cjb     Added support to pass actual device_ids for L21X measurements
11/22/13   bph     Send PILOT MEAS CFG REQ to FW w/i correct window
10/11/13   cjb     Removed legacy SEARCH, MEAS, CLEANUP states from IRAT:1x2L
10/06/13   cjb     Switch to a common TMIED_SEARCH_MEAS API from LTE
04/16/13   cjb     Removed deprecated variables in the interface.
03/06/13   cjb     LTE->1x interface changes to support non-LTE targets.
02/19/13   cjb     Changes to support LTE->1x along with new LTE CA feature.
12/03/12   cjb     Added DIV RF device to be sent in lte_param for L21X meas.
10/05/11   cjb     Added support for SON/ANR.
08/26/11   cjb     Changes for IRAT:1x->LTE. Initial working version.
08/05/11   cjb     Added RxLM handles for pilot_meas_req_msg interface.
05/12/11   cjb     Fixed window_size calculation for Conn mode meas.
03/03/11   cjb     onex_srch_lte_pilot_meas_req_msg_t: Added lte_band &
                   lte_channel. [Renamed source_band to lte_band]
03/03/11   cjb     Added parameter for LTE source band class information in
                   onex_srch_lte_pilot_meas_req_msg_t
10/14/10   bb      Changes to support connected mode measurements for
                   LTE to 1X IRAT feature
09/22/10   bb      Changes to support 40 pilots in IRAT Pilot Measurement
                   messages and added Pilot PN Phase field to
                   onex_srch_lte_pilot_meas_rsp_type structure
07/20/10   bb      Changes to support RF measurement scripts for IRAT
06/18/10   bb      Changed LTE OSTMR count to uin32
01/14/10   bb      Modified 1X to LTE message names
12/18/09   bb      Changed LTE OSTMR count to uin64
10/13/09   bb      Initial rev.
===========================================================================*/

#ifndef ONEX_SRCH_LTE_MSG_H
#define ONEX_SRCH_LTE_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* common */
#include <sys.h>  //! @todo: Temp until everyone is off MODEM_LTE_DEV
#include <sys_v.h>

/* searcher header files */
#include "onex_msgr.h"

/* message router include files */
#include "msgr.h"
#include "msgr_types.h"
#include "msgr_umid.h"

/* other header files */
#include "lm_types.h"
#include "lte_cphy_irat_meas_msg.h"
#include "lte_irat_types.h"
#include "rfcom.h"
#include "rfm_device_types.h"
#include "rfmeas_lte.h"
#include "rfmeas_types.h"
#include "sys.h"

#ifdef __cplusplus
}  // extern "C"
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! The text string for the external CDMA_CP Task Queue. */
#define ONEX_SRCH_LTE_MSG_Q_NAME              "ONEXLTE_Q"

/*-----------------------------------------------------------------------*/
/*     CDMA SRCH Supervisory                                             */
/*-----------------------------------------------------------------------*/

/*! Supervisory commands the CDMA ONEX SRCH module receives.  */
enum
{
  /*! Not used for now, sent from APPMGR */
  MSGR_DEFINE_UMID(ONEX, SRCH, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_struct_type),
  /*! Not used for now, send to APPMGR */
  MSGR_DEFINE_UMID(ONEX, SRCH, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_struct_type)

};

/*-----------------------------------------------------------------------*/

/*! Supervisory Loopback  - This message is sent by the message router
    and is to be sent back to it in a cdma_cp_spr_loopback_reply_t to
    confirm that the umb message queues are operating correctly. */
typedef msgr_spr_loopback_struct_type       onex_srch_spr_loopback_t;

/*! Supervisory Loopback Reply  - This reply is sent back to the
    message router in response to receiving a cdma_cp_spr_loopback_t
    message to confirm that the CDMA CP message queues are operating
    correctly. */
typedef msgr_spr_loopback_reply_struct_type onex_srch_spr_loopback_reply_t;


/*! Requests/Response/Indications between LTE ML1 and ONEX SRCH modules.
    (LTE ML1 <--> ONEX SRCH) */

enum
{
  /*! The first message in the enum */
  ONEX_SRCH_REQ_FIRST = MSGR_UMID_BASE(MSGR_ONEX_SRCH, MSGR_TYPE_REQ),
  MSGR_DEFINE_UMID(ONEX, SRCH, REQ, LTE_TRAN_SYSTIME, 0x00,
                   onex_srch_lte_tran_systime_req_msg_t),
                     /* ONEX_SRCH_REQ_LTE_TRAN_SYSTIME */

  MSGR_DEFINE_UMID(ONEX, SRCH, REQ, LTE_PILOT_MEAS,   0x01,
                   onex_srch_lte_pilot_meas_req_msg_t),
                   /* ONEX_SRCH_REQ_LTE_PILOT_MEAS */

  MSGR_DEFINE_UMID(ONEX, SRCH, REQ, LTE_PILOT_MEAS_ABORT, 0x02,
                   onex_srch_lte_pilot_meas_abort_req_msg_t),
                   /* ONEX_SRCH_REQ_LTE_PILOT_MEAS_ABORT */

  /*! The last message in the enum */
  ONEX_SRCH_REQ_LAST = ONEX_SRCH_REQ_LTE_PILOT_MEAS_ABORT
};

enum
{
  /*! The first message in the enum */
  ONEX_SRCH_RSP_FIRST = MSGR_UMID_BASE(MSGR_ONEX_SRCH, MSGR_TYPE_RSP),
  MSGR_DEFINE_UMID(ONEX, SRCH, RSP, LTE_PILOT_MEAS, 0x00,
                   onex_srch_lte_pilot_meas_rsp_msg_t),
                   /*! ONEX_SRCH_RSP_LTE_PILOT_MEAS */

  MSGR_DEFINE_UMID(ONEX, SRCH, RSP, LTE_PILOT_MEAS_ABORT, 0x01,
                   onex_srch_lte_pilot_meas_abort_rsp_msg_t),
                   /*! ONEX_SRCH_RSP_LTE_PILOT_MEAS_ABORT */

  /*! The last message in the enum */
  ONEX_SRCH_RSP_LAST = ONEX_SRCH_RSP_LTE_PILOT_MEAS_ABORT

};

/*! Indications that LTE ML1 module receives */
enum
{
  /*! The first message in the enum */
  ONEX_SRCH_IND_FIRST = MSGR_UMID_BASE(MSGR_ONEX_SRCH, MSGR_TYPE_IND),
  MSGR_DEFINE_UMID(ONEX, SRCH, IND, LTE_MEAS_CAPT_DONE,    0x00,
                   onex_srch_lte_meas_capt_done_ind_msg_t),
                   /*! ONEX_SRCH_IND_LTE_MEAS_CAPT_DONE */

  MSGR_DEFINE_UMID(ONEX, SRCH, IND, LTE_PILOT_MEAS_FAILED,  0x01,
                   onex_srch_lte_pilot_meas_failed_ind_msg_t),
                   /*! ONEX_SRCH_IND_LTE_PILOT_MEAS_FAILED */

  MSGR_DEFINE_UMID(ONEX, SRCH, IND, LTE_TRAN_SYSTIME_FAILED, 0x02,
                   onex_srch_lte_tran_systime_failed_ind_msg_t),
                  /*! ONEX_SRCH_IND_LTE_TRAN_SYSTIME_FAILED */

  /*! The last message in the enum */
  ONEX_SRCH_IND_LAST = ONEX_SRCH_IND_LTE_TRAN_SYSTIME_FAILED

};

/*! Maximum number of sectors for LTE <-> 1X pilot measurement */
#define SRCH_LTE_SECTOR_LIST_MAX_SIZE 40

/*! Default GAP value to do immediate start of pilot strength measurement */
#define GAP_VALUE_FOR_IMMED_START       0xFFFFFFFF

/*! Default GAP value to do immediate start of pilot strength measurement */
#define GAP_LENGTH_FOR_IMMED_START    0xFFFFFFFF

/*! Definition for window size value to indicate invalid value */
#define INVALID_WINDOW_SIZE_VALUE     0xFF

/*! window size for SON/ANR case [in chips] */
#define IRAT_SON_WINDOW_SIZE_VALUE    0x40

#define IRAT_MAX_SOURCE_ITEMS         0x2

/*-----------------------------------------------------------------------*/
/* Request messages received by ONEX SRCH */
/*-----------------------------------------------------------------------*/

/*! ONEX_SRCH_REQ_LTE_TRAN_SYSTIME payload structure */
typedef struct
{
  /*! LTE RTC counter */
  uint32        lte_ostmr;

  /*! CDMA System time corresponding to the LTE RTC value */
  uint64        cdma_sys_time;

} onex_srch_lte_tt_info_type;

typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type           msgr_hdr;

  /* Time Transfer information */
  onex_srch_lte_tt_info_type         tt_info;

} onex_srch_lte_tran_systime_req_msg_t;

/*! Structure used for each sector/cell */
typedef struct
{
  /*! Pilot PN Offset.  Range: 0 .. 511 */
  sys_pilot_pn_type        pilot_pn_offset;

} onex_srch_lte_pilot_meas_req_sector_type;

/*! ONEX_SRCH_REQ_LTE_PILOT_MEAS payload structure */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type          msgr_hdr;

  /*! RF primary device for measurement */
  rfm_device_enum_type          rf_pri_dev;

  /*! RF diversity device for measurement */
  rfm_device_enum_type          rf_div_dev;

  /*! Band/channel information */
  sys_channel_type              channel;

  /*! Number of sectors in the list */
  uint8                         sector_count;

  /*! List of sectors to be searched */
  onex_srch_lte_pilot_meas_req_sector_type
                                sector_info[ SRCH_LTE_SECTOR_LIST_MAX_SIZE ];

  /* Rotator value type
     Sensitivity: 2^-10 PPM per LSB */
  int16                         tcxomgr_rotator_type;

  /*! Start of gap time in LTE OSTMR count (CONNECTED MODE)
      Value of GAP_VALUE_FOR_IMMED_START indicates immediate start */
  uint32                        gap_start;

  /*! Duration of gap in LTE OSTMR count (CONNECTED MODE)
      Value of GAP_LENGTH_FOR_IMMED_START indicates immediate start */
  uint32                        gap_length;

  /*! Window size to use for search if specified
      (Table 6.6.6.2.1-1 from CAI 2.0)
      Value of INVALID_WINDOW_SIZE_VALUE indicates invalid value */
  uint8                         window_size;

  /*! buffer index to store rf cleanup script */
  uint8                         rf_cleanup_script_idx;

  /*! RxLM of LTE 0 */
  lm_handle_type                rxlm_handle_lte_0;

  /*! RxLM of LTE 1 */
  lm_handle_type                rxlm_handle_lte_1;

  /*! RxLM of 1X 0 */
  lm_handle_type                rxlm_handle_onex_0;

  /*! Number of RF source items */
  uint8                         num_source_items;

  /*! LTE->1x tune parameters for RF */
  lte_irat_l2x_tuneaway_param_s l2x_tuneaway_param;

} onex_srch_lte_pilot_meas_req_msg_t;

/*! ONEX_SRCH_REQ_LTE_PILOT_MEAS_ABORT payload structure */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type          msgr_hdr;

} onex_srch_lte_pilot_meas_abort_req_msg_t;

/*-----------------------------------------------------------------------*/
/* Responses received by LTE ML1 */
/*-----------------------------------------------------------------------*/


/*! Measurement results structure for 1x pilots */
typedef struct
{
  /*! Pilot PN Offset.  Range: 0 .. 511 */
  sys_pilot_pn_type                 pilot_pn_offset;

  /*! Pilot PN Phase */
  uint16                            pilot_pn_phase;

  /*! Pilot Strength (in -0.5 dB) - no limits applied */
  uint8                             strength;

  /*! Pilot Strength - linear ecio */
  uint32                            strength_lin;

} onex_srch_lte_pilot_meas_rsp_type;

/*! ONEX_SRCH_RSP_LTE_PILOT_MEAS payload structure */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! Band/channel information */
  sys_channel_type                  channel;

  /*! Number of sectors in the list */
  uint8                             sector_count;

  /*! List of measurement results for sectors searched */
  onex_srch_lte_pilot_meas_rsp_type     sector_info[ SRCH_LTE_SECTOR_LIST_MAX_SIZE ];

} onex_srch_lte_pilot_meas_rsp_msg_t;

/*! Enumuration of abort state types */
typedef enum
{
  ONEX_SRCH_MEAS_NONE            = 0,
  ONEX_SRCH_MEAS_RF_SCRIPT_BUILT = 1,
  ONEX_SRCH_MEAS_GAP_STARTED     = 2,
  ONEX_SRCH_MEAS_CFG_RSP_RXED    = 3,
  ONEX_SRCH_MEAS_SAMPLE_CAPTURED = 4
} onexsrch_meas_abort_state_type;

/*! ONEX_SRCH_RSP_LTE_PILOT_MEAS_ABORT payload structure */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type              msgr_hdr;

  /*! Meas State During Abort */
  onexsrch_meas_abort_state_type    meas_state;

} onex_srch_lte_pilot_meas_abort_rsp_msg_t;

/*! Enumuration of RF status types */
typedef enum {

  ONEX_RF_STATUS_GEN_SCRIPT_SUCCESS = 0,
  ONEX_RF_STATUS_GEN_SCRIPT_FAIL,
  ONEX_RF_STATUS_SAMPLE_CAPTURE_DONE,
  ONEX_RF_STATUS_SAMPLE_CAPTURE_NOT_DONE

} onexsrch_meas_capt_rf_status_type;

/*! ONEX_SRCH_IND_LTE_MEAS_CAPT_DONE payload structure */
typedef struct
{
   /*! Message header */
  msgr_hdr_struct_type               msgr_hdr;

  /*! rf status */
  onexsrch_meas_capt_rf_status_type  rf_status;

} onex_srch_lte_meas_capt_done_ind_msg_t;

/*! Enumeration of pilot measurement rejection types */
typedef enum
{
  ONEX_SRCH_WRONG_MSG_ORDER,      /*!< Wrong message order */
  ONEX_SRCH_WRONG_STATE,          /*!< Wrong 1x SRCH state */
  ONEX_SRCH_UNSUP_BAND_CHANNEL,   /*!< Unsupported band/chan */
  ONEX_SRCH_LOST_DUMP,            /*!< Lost searcher dumps */
  ONEX_SRCH_RF_GEN_SCRIPT_FAILED, /*!< RF script failed */
  ONEX_SRCH_SAMP_CAPT_FAILED,     /*!< Sample capture failed */
  ONEX_SRCH_INSUFFICIENT_TIME,    /*!< Insufficient time before gap start */
  ONEX_SRCH_PILOT_REJECT_MAX,     /*!< Max reject value for bounds checking */
} onex_srch_lte_pilot_reject_type;

/*! ONEX_SRCH_IND_PILOT_MEAS_FAILED message payload structure */
typedef struct
{
   /*! Message header */
  msgr_hdr_struct_type          msgr_hdr;

  /*! Rejection reason for pilot measurement */
  onex_srch_lte_pilot_reject_type   rej_reason;

} onex_srch_lte_pilot_meas_failed_ind_msg_t;

/*! ONEX_SRCH_IND_LTE_TRAN_SYSTIME_FAILED message payload structure */
typedef struct
{
   /*! Message header */
  msgr_hdr_struct_type            msgr_hdr;

} onex_srch_lte_tran_systime_failed_ind_msg_t;

/*! \brief Union of all message received by ONEX SRCH */
typedef union
{

  /*! \brief Message header */
  msgr_hdr_struct_type                       msgr_hdr;
  onex_srch_lte_pilot_meas_rsp_msg_t             pilot_meas_rsp;
  onex_srch_lte_meas_capt_done_ind_msg_t         meas_capt_done_ind;
  onex_srch_lte_pilot_meas_failed_ind_msg_t      pilot_meas_failed_ind;
  onex_srch_lte_tran_systime_failed_ind_msg_t    tran_systime_failed_ind_msg_t;

} cdmalte_rsp_msg_u;

/*! \brief Union of all CNF messages received by ONEX SRCH */
typedef union
{
  /*! \brief Message header */
  msgr_hdr_struct_type                     msgr_hdr;

  lte_cphy_irat_meas_init_cnf_s            init_cnf;
  lte_cphy_irat_meas_deinit_cnf_s          deinit_cnf;
  lte_cphy_irat_meas_timed_srch_meas_cnf_s timed_srch_meas_cnf;
  lte_cphy_irat_meas_abort_cnf_s           abort_cnf;

} cdmalte_msg_cnf_u;

/*! \brief Union of all REQUEST messages received by ONEX SRCH */
typedef union
{

  msgr_hdr_struct_type                           msgr_hdr;
  onex_srch_lte_tran_systime_req_msg_t           tran_systime_req;
  onex_srch_lte_pilot_meas_req_msg_t             pilot_meas_req;
  onex_srch_lte_pilot_meas_abort_req_msg_t       meas_abort_req;

} cdmalte_msg_req_u;

#endif /* ONEX_SRCH_LTE_MSG_H */
