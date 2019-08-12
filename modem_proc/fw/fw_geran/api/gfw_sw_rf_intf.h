/*!
  @file
  gfw_sw_rf_intf.h

  @brief
  Definition of GFW/SW/RF interface data structures

  @details
  This file contains data type definitions for the RF interface between GFW,
  GL1 SW and RFSW

*/

#ifndef GFW_SW_RF_INTF_H
#define GFW_SW_RF_INTF_H
/*===========================================================================

  Copyright (c) 20012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_geran/api/gfw_sw_rf_intf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#include "fw_rf_common_intf.h"
#include "gfw_sw_intf_common.h"

/* Flag indicating to RF drivers which interface format is used */
#define GFW_RXDIV_RF_INTERFACE
// --------------------------------------------------------------------------
// Rx CCS Event definitions
// --------------------------------------------------------------------------

/* Rx CCS Events Structure */
typedef struct tag_GfwRxCcsEvent
{
   uint32 rflmStartData;
   uint32 rflmStopData;
} GfwRxCcsEvent;

// --------------------------------------------------------------------------
// Rx Event definition
// --------------------------------------------------------------------------
typedef struct tag_GfwRxEvent
{
   GfwRxCcsEvent ccsEvent;
   boolean ccsStartIndexUsed;
   boolean ccsStopIndexUsed;
} GfwRxEvent;

// --------------------------------------------------------------------------
// Tx Start, Transition and Stop buffer definitions
// --------------------------------------------------------------------------

typedef struct GfwStartTxBufType
{
  uint32 rflmStart;
  boolean ccsStartIndexUsed;
} GfwStartTxBuf;

#define MDSP_MAX_NUM_TRANS_BUF 2

typedef struct GfwTransTxBufType
{
  uint32 rflmTrans[MDSP_MAX_NUM_TRANS_BUF];
} GfwTransTxBuf;


typedef struct GfwStopTxBufType
{
  uint32 rflmStop;
  boolean ccsStopIndexUsed;
} GfwStopTxBuf;


// --------------------------------------------------------------------------
// Tx Event definition
// --------------------------------------------------------------------------
typedef struct tag_GfwTxEvent
{
  GfwStartTxBuf start;
  GfwTransTxBuf trans[MDSP_MAX_UL_TS + 1]; // (UL_TS +1) To match RF structure and avoid warnings
  GfwStopTxBuf  stop;
} GfwTxEvent;



// --------------------------------------------------------------------------
// GFW Burst Event definitions
// --------------------------------------------------------------------------

/* We can have sync rx and sch or acquisition in one frame.
   FW and SW will always use index 0 for sync rx and index 1 for sch or acq */
enum tag_GfwRxEventIndex
{
   GFW_SYNC_RX_EVENT_INDEX = 0,
   GFW_SCH_OR_ACQ_RX_EVENT_INDEX,
   GFW_PCH_INDEX,
   GFW_NUM_RX_EVENTS
};

/* Number of non standalone (i.e. issued together with Rx and Tx) RSSI measurements per TDMA
   frame. Note that this definition is specific to the GfwRfBurstEvent structure definition below. */
#define MDSP_MAX_NON_STANDALONE_RSSI_MEASUREMENTS  7

/* GFW Burst Event */
typedef union tag_GfwRfBurstEvent
{
   GfwRxEvent monEvent[MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS];
   struct
   {
      GfwRxEvent dummyMonEvent[MDSP_MAX_NON_STANDALONE_RSSI_MEASUREMENTS]; // FW and SW should never use this
      GfwTxEvent txEvent;
      GfwRxEvent rxEvent[GFW_NUM_RX_EVENTS];
   };
} GfwRfBurstEvent;

// --------------------------------------------------------------------------
// Enter Mode Event definition
// --------------------------------------------------------------------------
typedef struct tag_GfwEnterModeEvent
{
   uint32 rflmEnterMode;
} GfwEnterModeEvent;


/* Wakeup CCS Events Structure */
typedef struct tag_GfwWakeUpCcsEvent
{
   uint32 rflmWakeup;
} GfwWakeUpCcsEvent;



// --------------------------------------------------------------------------
// RF Init Event definition (union of Enter Mode Event and Wakeup CCS Event)
// --------------------------------------------------------------------------
typedef union tag_GfwRfInitEvent
{
   GfwEnterModeEvent enterMode;
   GfwWakeUpCcsEvent wakeUp;
} GfwRfInitEvent;


/* Set Tx Band CCS Events Structure */
typedef struct tag_GfwSetTxBandCcsEvent
{
   uint32 rflmTxBand;
} GfwSetTxBandCcsEvent;


// --------------------------------------------------------------------------
// Set Tx Band Event definitions
// --------------------------------------------------------------------------
typedef struct tag_GfwRfSetTxBandEvent
{
   GfwSetTxBandCcsEvent ccsEvent;
} GfwRfSetTxBandEvent;


/* Thermistor Read CCs Event Structure */
typedef struct tag_GfwThermReadCcsEvent
{
   uint32 rflmThermRead;
} GfwThermReadCcsEvent;


// --------------------------------------------------------------------------
// Thermistor Read Event definitions
// --------------------------------------------------------------------------
typedef GfwThermReadCcsEvent GfwRfThermReadEvent;


// --------------------------------------------------------------------------
// GFW RF events table containing all possible RF events
// --------------------------------------------------------------------------
typedef struct tag_GfwRfEventTable
{
    GfwRfBurstEvent burstEvent[3];
    GfwRfInitEvent rfInitEvent;
    GfwRfSetTxBandEvent setTxBandEvent;
    GfwRfThermReadEvent thermReadEvent;
} GfwRfEventTable;

/* end of file gfw_sw_rf_intf.h */
#endif /* GFW_SW_RF_INTF_H */
