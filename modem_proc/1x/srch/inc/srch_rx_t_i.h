#ifndef SRCH_RX_T_I_H
#define SRCH_RX_T_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S R C H  _ R X  ---  H E A D E R   F I L E

GENERAL DESCRIPTION
  This module contains header information pertaining to srch_rx.

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005 - 2014 Qualcomm Technologies, Inc.
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


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srch_rx_t_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/14   cjb     NV refresh clean-up
04/24/14   cjb     BOLT revisit cleanup
08/14/12   cjb     Added changes to use SLPC(new sleepctl replacement module).
10/31/11   jtm     Feature clean up.
10/27/11   vks     Add support for scheduled RF disable for offline qpch.
10/25/11   adw     Integrate TRAM support.
10/24/11   jtm     Feature clean up.
09/26/11   vks     Add a flag to indicate immediate request vs req_and_notify
                   in the RX_REQ_AND_NOTIFY_CMD.
08/17/11   vks     Remove references to sample server in SRCH RX module (
                   sample server is not present since 9k).
07/11/11   vks     Mainline FEATURE_SRCH_RX_STM and FEATURE_SRCH_RX_SVDO.
06/20/11   vks     Initial set of changes to get RX DIV working with RX STM.
04/15/11   pk      Added support for SRCH_RX_STATUS
04/12/11   vks     Move rx callback type outside of RX STM featurization.
01/26/11   vks     Remove unncessary rf header file include.
11/01/10   jtm     Modem 1x SU API changes.
10/26/09   sst     Update to RX_STM to work with legacy RF API
10/02/09   sst     SVDO RF & TRMv3 development (featurized)
04/24/09   adw     Added invalid field to chain_name and freq_range enum types.
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Cleaned up incs and eliminated implicit includes.
09/17/08   vks     Added FEATURE_SRCH_HIGHER_BAND_CLASSES_SUPPORTED_BY_RF to
                   cover band classes 16 through 19.
09/09/08   aps     Converted RF chains to enum type
09/03/08   aps     Corrected checkin error
09/03/08   aps     Corrected enum mapping for srch_rx_band_type
08/18/08   adw     Added SINGLE_ANTENNA to featurize alt chain for ULC.
                   Added FEATURE_SRCH_HAS_NO_TRM to remove TRM for ULC.
06/20/08   mca     Merge from //depot
06/11/08   adw     Added Band Class 16, 17, 18, 19
09/25/06   tjc     Added Band Class 15
03/29/06   kwo     Added RX_REQUESTED to enum
01/16/06   awj     Lint cleanup
11/14/05   awj     Added Band Class 14 mapping
09/13/05   grl     Added RX_TUNING enum.
07/19/05   bt      Added srch_rx_bypass_rtc_sync_type and
                   srch_rx_flush_samp_ram_type
07/19/05   grl     Added RX_TUNED enum to srch_rx_status_type.
06/24/05   sfm     Moved NO_CHAIN define to srch_rx.c
06/09/05   sfm     more naming changes and coding standards changes
06/03/05   sfm     srch_rx naming changes
05/31/05   rng     Merged from SHDR Sandbox.
05/31/05   sar     Removed definition of SRCH_RX_BC13 from srch_rx_band_enum_type.
01/20/05   sst     Implementation, first cut.
===========================================================================*/


/*==========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"
#include "customer.h"

/* Srch */
#include "srch_rx_t.h"

/* Other */
#include "stm.h"
#include "trm.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

/* Thresholds for SOOS algorithm. The distance between the two thresholds
   should be larger than the difference in antenna sensitivity */
#define SRCH_RX_CHAIN_SEL_UPPER_HYST_THRESH   -85   /* dBm */
#define SRCH_RX_CHAIN_SEL_LOWER_HYST_THRESH   -95   /* dBm */

/* minimum value for rx agc */
#define SRCH_RX_MIN_AGC_VAL              -127

/*! TRM notion of infinity, in trm_duration_t units */
#define TRM_INFINITY              ((trm_duration_t) ~0)

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

typedef enum
{
  RX_CLIENT_1X_PRI,                         /* primary RX_SM client */
  RX_CLIENT_1X_SEC,                         /* secondary RX_SM client */
  NUM_RX_CLIENT,
  NO_RX_CLIENT

} rx_sm_client_enum_t;

/* Bit mask for devices owned */
typedef enum
{
  CHAIN_NONE  = 0x0,
  CHAIN_0     = 0x1, /* Bit 0 */
  CHAIN_1     = 0x2, /* Bit 1 */
  BOTH_CHAINS = 0x3  /* Bit 0 & 1 */
} srch_rx_chain_owned_type;

typedef enum
{
  RX_UNLOCKED,             /* 1X does not have the chain */
  RX_REQUESTED,            /* TRM request has been sent, but not received */
  RX_LOCKED,               /* 1X has the chain but it's not ready yet */
  RX_TUNING,               /* Receiver in the process of being tuned. */
  RX_WAITING_FOR_COMPLETE, /* Receiver tuned but AGCs not running. */
  RX_READY                 /* sample RAM is flushed and the chain is tuned */

} srch_rx_status_type;

typedef struct
{
  srch_rx_band_type     band;
  srch_rx_chan_type     chan;
} srch_rx_band_chan_t;

typedef void (*srch_rx_callback_t)
(
  void*                      tag            /* payload echoed from orig req */
);

/* Callback function definition for unlock callback events. */
typedef void (*srch_rx_unlock_cb_t)
(
  trm_unlock_event_enum_t event,           /* Event being sent to the client */
  uint32                  unlock_by_sclk   /* Sclk timestamp for UNLOCK_BY*/
);

/* description of the notification method */
typedef struct
{
  srch_rx_callback_t         cb;            /* notify call back function */
  stm_state_machine_type    *stm;           /* state machine response ptr */
  stm_group_type            *stm_group;     /* state machine response ptr */
  void*                      tag;           /* data tag */
} rx_sm_resp_info_t;

/* TRM/RF request sent to RX_SM state machine */
typedef struct
{
  rx_sm_client_enum_t        client;        /* RX State Machine */
  trm_resource_enum_t        resource;      /* Client requested resource */
  trm_timestamp_t            when;          /* Time when lock is req (sclk64) */
  boolean                    req_immed;     /* Do a req and notify or immed
                                               request */
  trm_duration_t             duration;      /* in sclks */
  trm_reason_enum_t          reason;        /* request reason */
  srch_rx_band_chan_t        band_chan;     /* band and chan requested */
  boolean                    ran_tune;      /* tune on req and notify */
  boolean                    exec_wup;      /* exececute the wakeup on tune */
  timetick_type              timeout_ms;    /* max time to wait for lock */
  uint32                     rtc_cx1_trigger; /* rtc value for scheduled
                                                rx disable */
  rx_sm_resp_info_t          notify;        /* notification methods */
  srch_rx_unlock_cb_t        unlock_cb;     /* callback called when TRM
                                               requests for unlock  */
} rx_sm_req_t;

#endif /* SRCH_RX_T_I_H */
