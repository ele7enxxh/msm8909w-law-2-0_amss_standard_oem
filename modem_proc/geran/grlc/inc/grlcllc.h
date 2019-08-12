/*****************************************************************************
***
*** TITLE   grlcllc.h
***
***  GPRS RLC EXPORTED DEFINITIONS TO LLC
***
***
*** DESCRIPTION
***
***  This file contains data structure definitions and access functions
***  provided by RLC to support the interface with LLC.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcllc.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 03/28/01    hv     RLC access function for LLC
***
*****************************************************************************/

#ifndef INC_GRLCLLC_H
#define INC_GRLCLLC_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gllcrlcif.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/*===========================================================================
** Definitions of parameters used in flow control calculations in GPRS mode
**===========================================================================
*/
/*
** Low water flow control point is chosen to ensure that under worst case
** conditions there is always sufficient data available for transmission to
** prevent premature TBF countdown.
**
** Full Transmit Window (Stalled) - 64 Blks
** BS_CV_MAX                      - 15 Blks (Worst Case Assumed)
** UL slots                       - 1 to 4 Slots (Class 12 Max)
** CS1 to CS4 Payload per Block   - 20, 30, 36, 50 Octets Respectively
** Margin of Safety               - 250 Octets
**
** Total = ((64 + (15 x TS)) x CS_PYLD) + Margin = Flow Resumption Point
**
** High Water flow control point is chosen to minimise transmission latency.
**
** NOTE - DSM DUP item event handling should be factored into the LLC flow
**        control via LLC and DSM directly. The DSM DUP item level at which
**        flow control is triggered should be so as to leave sufficient
**        remaining DUP items to cater for a full transmit window worth of
**        ongoing PDU segmentation. This would typically be one DUP item per
**        radio block, which would require the level to be set at 64 plus a
**        a margin of safety. Recommended level would be 85 DSM DUP Items.
*/

/* 4 uplink time slots are required for Multislot Class 33, the highest offered */
#define GRLCLLC_MAX_TS         4

/* This factor is a reduction in percentage which is applied to the overall flow control
** values when transmit bkanking in L1 reaches a sufficiently high level. When this happens
** transmission is slow, hence the flow control level should be lowered accordingly.
** This factor is a percentage that the flow control is lowered to. For example, set to 80 when
** the flow control value should be set to 80% of what it was
*/
#define TX_BLANKING_ADJUSTMENT_FACTOR   80    /* expressed in percentage */

/* Maximum uplink PDU size
*/
#define GRLC_UL_PDU_MAX_LEN     1560

#define MAX_FLOW_CONTROL_NUM_OCTETS                                  \
          (uint32)                                                   \
          ( ((RM_RLC_WINDOW_SIZE + (BS_CV_CONST * GRLCLLC_MAX_TS)) * \
              (GRLC_DATA_BLOCK_LEN[RM_CS_2] - RM_RLC_HDR_LEN)) +     \
            GRLC_UL_PDU_MAX_LEN )                                    \


#define RLC_FLOOR_FCTRL_VALUE   (3*GRLC_UL_PDU_MAX_LEN)

#define GRLCLLC_LOWATER_OCTETS(gas_id)                                    \
  MAX(RLC_FLOOR_FCTRL_VALUE,                                              \
      (                                                                   \
        (ul[gas_id].state == US_ACK_XFER || ul[gas_id].state == US_UNACK_XFER) ?          \
        (                                                                 \
          (uint32)                                                        \
          ( ((RM_RLC_WINDOW_SIZE + (BS_CV_CONST * ul[gas_id].cur_res.num_timeslots)) * \
              (GRLC_DATA_BLOCK_LEN[ul[gas_id].cur_res.cs] - RM_RLC_HDR_LEN)) +    \
            GRLC_UL_PDU_MAX_LEN )                                           \
        ) :                                                               \
        /* maximum level for GPRS mode */                                 \
        MAX_FLOW_CONTROL_NUM_OCTETS                                       \
      ) *                                                                 \
      ((IS_TX_BLANKING_EXCEEDED_THRESHOLD(gas_id) ?                       \
        (TX_BLANKING_ADJUSTMENT_FACTOR) :                                 \
        (100)                                                             \
       ) / 100                                                            \
      )                                                                   \
     )

#define GRLCLLC_HIWATER_OCTETS(gas_id)    ( GRLCLLC_LOWATER_OCTETS(gas_id) + GRLC_UL_PDU_MAX_LEN )

/*================================================================================
** Definitions of parameters used in flow control calculations in EGPRS mode.
** If RLC is in transfer then the level is derived based on the current
** allocation. In the absence of an allocation and hence the TBF mode as well,
** the level used is that of the maximum level for GPRS mode (6450). This level
** will be sufficient not to cause premature countdown should the allocation be
** an EGPRS one and it is also not superfluous to cause any timer expiries should
** the allocation be a GPRS one.
**================================================================================
*/
#define ERLCLLC_MARGIN        10000

/*
** Flow control is now based active difference between Vs and Va , rather than
** fixed large window size. This prevents to reduce RTT under inter SGSSN
** handover , as less number of PDU's will be purged during XID reset.
** Limit the V(s) - V(a) to distance to asigned WS for flow control in Unack mode.
*/
#define ERLCLLC_LOWATER_OCTETS(gas_id)                                    \
  MAX(RLC_FLOOR_FCTRL_VALUE,                                              \
      (                                                                   \
        (ul[gas_id].state == US_ACK_XFER || ul[gas_id].state == US_UNACK_XFER) ?          \
        (                                                                 \
          (uint32)                                                        \
          ( MIN((MOD_DIF_ESNS(ul[gas_id].vs, ul[gas_id].va)),ul[gas_id].cur_res.ws) + (BS_CV_CONST * ul[gas_id].cur_res.num_timeslots)) *    \
          E_GRLC_DATA_BLOCK_LEN[ul[gas_id].cur_res.cs] +                          \
          ERLCLLC_MARGIN                                                  \
        ) :                                                               \
        /* maximum level for GPRS mode */                                 \
        MAX_FLOW_CONTROL_NUM_OCTETS                                       \
      ) *                                                                 \
      ((IS_TX_BLANKING_EXCEEDED_THRESHOLD(gas_id) ?                       \
        (TX_BLANKING_ADJUSTMENT_FACTOR) :                                 \
        (100)                                                             \
       ) / 100                                                            \
      )                                                                   \
     )

/*===============================================================================
** Set the difference between the low and high water marks to be 1000 octets
** instead 500 in order to reduce the frequency of flow control commands to LLC.
** When the queue reaches the low water mark and 500 is used, a flow control
** command is sent on every subsequent PDU being enqueued and freed
**===============================================================================
*/
#define ERLCLLC_HIWATER_OCTETS(gas_id)    (ERLCLLC_LOWATER_OCTETS(gas_id) + GRLC_UL_PDU_MAX_LEN)

/*--------------------------------------------------------------------------
** This is the margin above the normal flow control level for both GPRS and
** EGPRS modes at which the PDUs are discarded to prevent overflow in the
** event flow control commands to LLC have no effect. This level represents
** one LLC PDU of maximum size (1500 from LLC XID) plus the margins which
** include: ERLLLC_MARGIN and 1000 being the difference between HIGH and LOW
** watermarks
**--------------------------------------------------------------------------
*/
#define GRLC_DISCARDING_MARGIN_OCTETS     (1500+1500+1000)


/* Assuming a Data transfer of upto 600 kbps -> 75 KBps times 8 = 76800 bytes
** RLC can flow control based on this assumption to be 80 % .
*/
/* --------------------------------------------------------------------------
** If RLC/MAC/L1 get out-of-sync, RLC could receive
** continuously PDU from LLC , which would Q up and cause memory exhaustion
** In order to prevent crash and see why we got out of sync , panic reset is
** performed. The number arrived considering that it is greater than MAX N3102=32
** --------------------------------------------------------------------------
*/
#define MAX_NUM_SIGNALLING_PDU              40
#define MAX_NUM_BACKLOG_SIGNALLING_PDUS     12

/*------------------------------------------------------------------------------
** Discarding level is set 2.5 LLC PDUs above the highest water octets.
** The highest water octets equate to 512 window size, 4 timesslots at MCS-9
** and BS_CV_MAX at 15 ..
**------------------------------------------------------------------------------
*/
  #define GRLC_DISCARDING_OCTETS  ( (((512+(15*4))*74) + ERLCLLC_MARGIN) +  \
                                  GRLC_UL_PDU_MAX_LEN +                   \
                                  GRLC_DISCARDING_MARGIN_OCTETS           \
                                )

/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_LLC_UL_FLOW_CONTROL()
===
===  DESCRIPTION
===
===    For use by RLC to flow control LLC as a function of the current octet
===    counts. The absolute LLC octet count is used to test against the
===    upper flow control threshold, and the notional LLC octet counts is
===    used to test against the lower flow control threshold.
===
===    During RLC User Plane Suspended state, PDUs are not subjected to
===    flow control.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

void grlc_llc_ul_flow_control( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GRLC_LLC_UL_PUT_MES_BUF
===
===  DESCRIPTION
===
===    For use by the LLC layer implementation to put a dynamically
===    allocated message resource of type gprs_grr_ul_mes_t to RLC's GRR SAP
===    uplink message queue.
===
===  DEPENDENCIES
===
===    LLC entity is responsible for dynamic message resource allocation
===    for the given message.
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    May cause a context switch.
===
===========================================================================*/

extern void grlc_llc_ul_put_mes_buf( gprs_grr_ul_mes_t *msg_ptr );


/*===========================================================================
===
===  FUNCTION      GRLC_LLC_UL_GET_CURRENT_OCTET_COUNT
===
===  DESCRIPTION
===
===    For use by the LLC layer implementation to query the quantity of
===    data and signalling currently awaiting transmission.
===
===    A typical usage of this enquiry would be in support of the Follow
===    On Request feature.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Current octet count of the LLC PDU queue.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern uint32 grlc_llc_get_current_octet_count( gas_id_t gas_id );


#endif /* INC_GRLCLLC_H */

/*** EOF: don't remove! ***/
