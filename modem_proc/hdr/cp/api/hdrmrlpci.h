#ifndef HDRMRLPCI_H
#define HDRMRLPCI_H

/*===========================================================================

          M U L T I - F L O W   R A D I O   L I N K   P R O T O C O L 
                  C O N F I G U R A T I O N    F I L E

DESCRIPTION
  This contains the declarations for regional functions exported by MRLP and
  any types, macros, or variables used in the different files of MRLP.


Copyright (c) 2004,2005,2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrmrlpci.h#1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/11   smd     Moved more to SU API header files.
09/23/11   smd     SU API cleanup.
07/02/08   pba     Size of FlowNNTimersFwd is different for MMPA and EMPA/MFPA
07/30/07   pba     Obsoleted feature FEATURE_HDR_EMPA_SUPPORT_STD_V115_ATTRIB_ID
07/17/07   pba     RelB MMPA support.
02/23/07   wsh     Disallow flow01 to be activated and flow00 to be deactivated
08/08/06   ksu/yll EMPA: v2.0 session negotiation
05/10/06   yll     Added support for EMPA.
04/10/06   yll     Changed HDRMRLPC_MAX_NUM_OPEN_RESERVATIONS_FWD/REV_DEF
                   to 0xF, based on Rev A Addendum 2 and Rev B standard.
02/28/06   yll     Removed the obsolete attributes.
11/16/05   yll     Added support for new attributes: XmitAbortTimer, 
                   DOSAllowed, MaxRLPFlows and MaxReservations.
08/07/05   mpa     Added AT Initiated GAUP support + new ResKK format.
04/06/04   mpa     Created Module

===========================================================================*/
#include "hdr_variation.h"
#ifdef FEATURE_HDR_QOS
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdrcp_api.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* MAX reservations set to 20 for both Fwd and Rev */
#define HDRMRLPC_N_RES_LABELS_MAX_FWD HDRMRLPC_N_RES_LABELS_MAX
#define HDRMRLPC_N_RES_LABELS_MAX_REV HDRMRLPC_N_RES_LABELS_MAX

/* Default RLP priority for flowNN > 0 (not an actual attribute) */
#define HDRMRLPC_DEFAULT_FWD_RLP_PRIORITY 3
#define HDRMRLPC_DEFAULT_REV_RLP_PRIORITY 3

/* ----------------------------------------------------------------------------
 * Attribute Header Common Sizes
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_BITS_PER_BYTE 8

#define HDRMRLPC_ATTRIB_LEN      8
#define HDRMRLPC_ATTRIB_ID_SIZE 16

#define HDRMRLPC_SIMPLE_ATTRIB_SIZE 8
#define HDRMRLPC_SIMPLE_ATTRIB_16_SIZE  16

#define HDRMRLPC_MIN_ATTRIB_LEN HDRMRLPC_ATTRIB_LEN + \
                                HDRMRLPC_ATTRIB_ID_SIZE
                               
/* Length of the attribute (including the length field) */
#define HDRMRLPC_SIMPLE_ATTRIB_LEN HDRMRLPC_MIN_ATTRIB_LEN + \
                                   HDRMRLPC_SIMPLE_ATTRIB_SIZE

#define HDRMRLPC_SIMPLE_ATTRIB_16_LEN HDRMRLPC_MIN_ATTRIB_LEN + \
                                      HDRMRLPC_SIMPLE_ATTRIB_16_SIZE

/* Length in octets of a simple attribute (excluding the length field )*/
#define HDRMRLPC_SIMPLE_ATTRIB_LEN_BYTES \
        ((HDRMRLPC_ATTRIB_ID_SIZE + HDRMRLPC_SIMPLE_ATTRIB_SIZE) \
        / HDRMRLPC_BITS_PER_BYTE)

/************************** Simple Attributes *******************************/

/* ----------------------------------------------------------------------------
 * Default Maximum Abort Timer 
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_MAX_ABORT_TIMER_ATTRIB_ID 0xFFFC
#define HDRMRLPC_MAX_ABORT_TIMER_DEF 0x05

/* ----------------------------------------------------------------------------
 * Default PPPFreeAuthentication Support
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_PPP_FREE_AUTHENTICATION_SUPPORT_ATTRIB_ID 0xFFFB
#define HDRMRLPC_PPP_FREE_AUTHENTICATION_SUPPORT_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default Two Routes Supported  
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_TWO_ROUTES_SUPPORTED_ATTRIB_ID 0xFFFA
#define HDRMRLPC_TWO_ROUTES_SUPPORTED_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default AT NAKDelay Supported  
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_AT_NAKDELAY_SUPPORTED_ATTRIB_ID 0xFFF9
#define HDRMRLPC_AT_NAKDELAY_SUPPORTED_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default FlowNN Nak Enable Fwd  
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_NAK_ENABLE_FWD_ATTRIB_ID 0xFE00
#define HDRMRLPC_NAK_ENABLE_FWD_DEF    0x01 
  /* Forward Link Nak Enabled by Default */

/* ----------------------------------------------------------------------------
 * Default FlowNN Nak Enable Rev  
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_NAK_ENABLE_REV_ATTRIB_ID 0xFD00
#define HDRMRLPC_NAK_ENABLE_REV_DEF    0x01 
  /* Reverse Link Nak Enabled by Default */

/* ----------------------------------------------------------------------------
 * Default FlowNN Higher Layer Protocol Fwd 
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_FWD_ATTRIB_ID 0xFC00
#define HDRMRLPC_HIGHER_LAYER_PROTOCOL_FWD_DEF 0x00
  /* The higher layer protocol is PPP (NotSupported) */

/* ----------------------------------------------------------------------------
 * Default FlowNN Higher Layer Protocol Rev 
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_REV_ATTRIB_ID 0xFB00
#define HDRMRLPC_HIGHER_LAYER_PROTOCOL_REV_DEF 0x00 
  /* The higher layer protocol is PPP (NotSupported) */

/* ----------------------------------------------------------------------------
 * Default FlowNN Physical Layer Nak Enable Rev 
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_PHYS_LAYER_NAK_ENABLED_REV_ATTRIB_ID 0xFA00
#define HDRMRLPC_PHYS_LAYER_NAK_ENABLED_REV_DEF 0x01 
  /* RLP is to retransmit octets when a PhysicalLayer.ReverseTrafficPacketMissed
     indication is received (NotSupported) */

/* ----------------------------------------------------------------------------
 * Default ReservationKK Idle State Fwd 
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_RES_KK_IDLE_STATE_FWD_ATTRIB_ID 0xF900
#define HDRMRLPC_IDLE_STATE_FWD_DEF 0x00
  /* Reservation does not change states when a Connection is closed */

/* ----------------------------------------------------------------------------
 * Default ReservationKK Idle State Rev 
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_RES_KK_IDLE_STATE_REV_ATTRIB_ID 0xF800
#define HDRMRLPC_IDLE_STATE_REV_DEF 0x00
  /* Reservation flow does not change states when a Connection is closed */

/* ----------------------------------------------------------------------------
 * Default Transmit Abort Timer Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_REV_ATTRIB_ID 0xF700
#define HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_ATTRIB_MIN_VAL_SIZE 16
#define HDRMRLPC_TRANSMIT_ABORT_TIMER_REV_DEF 0x0000
#define HDRMRLPC_TRANSMIT_ABORT_TIMER_REV_MAX 0x03E8

/* ----------------------------------------------------------------------------
 * Default Data Over Signalling Alowed Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_DATA_OVER_SIG_ALLOWED_REV_ATTRIB_ID 0xF600
#define HDRMRLPC_DATA_OVER_SIG_ALLOWED_REV_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default Activated Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_ACTIVATED_FWD_ATTRIB_ID 0xF300
#define HDRMRLPC_FLOW_00_ACTIVATED_FWD_DEF 0x01
#define HDRMRLPC_FLOW_01_ACTIVATED_FWD_DEF 0x01
#define HDRMRLPC_FLOW_NN_ACTIVATED_FWD_DEF 0x00

/* ----------------------------------------------------------------------------
 * Desired Activated Forward Flow
 * Currently data team's code does not support flow01 even though it is enabled 
 * by default, so we need to propose 0x00 (deactivated) in AT_INIT.
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_01_DEACTIVATED_FWD 0x00

/* ----------------------------------------------------------------------------
 * Default Activated Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_ACTIVATED_REV_ATTRIB_ID 0xF200
#define HDRMRLPC_FLOW_00_ACTIVATED_REV_DEF 0x01
#define HDRMRLPC_FLOW_01_ACTIVATED_REV_DEF 0x01
#define HDRMRLPC_FLOW_NN_ACTIVATED_REV_DEF 0x00

/* ----------------------------------------------------------------------------
 * Desired Activated Reverse Flow
 * Currently data team's code does not support flow01 even though it is enabled
 * by default, so we need to propose 0x00 (deactivated) in AT_INIT.
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_01_DEACTIVATED_REV 0x00

/* ----------------------------------------------------------------------------
 * Default Sequence Length Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID 0xF100
#define HDRMRLPC_SEQ_LEN_FWD_DEF 0x02

/* ----------------------------------------------------------------------------
 * Default Sequence Length Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_SEQ_LEN_REV_ATTRIB_ID 0xF000
#define HDRMRLPC_SEQ_LEN_REV_DEF 0x02

/* ----------------------------------------------------------------------------
 * Default Flow Protocol SDU Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_FWD_ATTRIB_ID 0xEF00
#define HDRMRLPC_FLOW_01_FLOW_PROT_SDU_FWD_DEF 0x01
#define HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_FWD_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default Flow Protocol SDU Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_REV_ATTRIB_ID 0xEE00
#define HDRMRLPC_FLOW_01_FLOW_PROT_SDU_REV_DEF 0x01
#define HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_REV_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default Data Unit Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_DATA_UNIT_FWD_ATTRIB_ID 0xED00
#define HDRMRLPC_DATA_UNIT_FWD_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default Data Unit Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_DATA_UNIT_REV_ATTRIB_ID 0xEC00
#define HDRMRLPC_DATA_UNIT_REV_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default Route Protocol SDU Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_FWD_ATTRIB_ID 0xEB00
#define HDRMRLPC_FLOW_01_ROUTE_PROT_SDU_FWD_DEF 0x01
#define HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_FWD_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default Route Protocol SDU Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_REV_ATTRIB_ID 0xEA00
#define HDRMRLPC_FLOW_01_ROUTE_PROT_SDU_REV_DEF 0x01
#define HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_REV_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default Simultaneous Delivery On Both Routes Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_SIM_DEL_BOTH_ROUTES_FWD_ATTRIB_ID 0xE900
#define HDRMRLPC_SIM_DEL_BOTH_ROUTES_FWD_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default Out Of Order Delivery To Route Protocol Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_OOO_DEL_ROUTE_PROT_FWD_ATTRIB_ID 0xE800
#define HDRMRLPC_OOO_DEL_ROUTE_PROT_FWD_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default FlowNN NAK Delay Time Fwd  
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_ATTRIB_ID 0xE700
#define HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_DEF 0x00

/************************** Complex Attributes *******************************/

/* ----------------------------------------------------------------------------
 * Default FlowNN Identification Fwd  
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_IDENTIFICATION_FWD_ATTRIB_ID 0x0100
#define HDRMRLPC_ID_ATTRIB_MIN_VAL_SIZE 16
#define HDRMRLPC_ID_ATTRIB_MAX_VAL_SIZE 40
#define HDRMRLPC_ID_ATTRIB_MIN_LEN HDRMRLPC_MIN_ATTRIB_LEN + \
                                   HDRMRLPC_ID_ATTRIB_MIN_VAL_SIZE

/* Default values for Fwd RLP flow 0x00 */
#define HDRMRLPC_FLOW_00_ACTIVE_FWD_DEFAULT   0x01 /* By default, the RLP flows are active */
#define HDRMRLPC_FLOW_00_SEQ_LEN_FWD_DEFAULT    21 /* Default Sequence Length Forward RLP FLow 00 */
#define HDRMRLPC_FLOW_00_RLP_ID_LEN_FWD_DEFAULT  1 /* Default RLP ID Length for Forward RLP Flow 00 */
#define HDRMRLPC_FLOW_00_RLP_ID_FWD_DEFAULT      0 /* Default RLP ID for Forward RLP Flow 00 */

/* Default values for Fwd RLP flow > 0x00 */
#define HDRMRLPC_ACTIVE_FWD_DEFAULT         0 /* RLP flows inactive by default */
//#define HDRMRLPC_SEQ_LEN_FWD_DEFAULT        0
//#define HDRMRLPC_RLP_ID_FWD_DEFAULT         0
//#define HDRMRLPC_RLP_ID_LEN_FWD_DEFAULT     0

/* ----------------------------------------------------------------------------
 * Default FlowNN Identification Rev (For MFPA/EMPA) or
 * Default FlowNN Sequence Length Fwd (For MMPA)
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_ID_REV_OR_SEQ_LEN_FWD_ATTRIB_ID 0x0200

/* Default (and Desired) values for Rev RLP flow 0x00 */
#define HDRMRLPC_FLOW_00_ACTIVE_REV_DEFAULT   0x01 /* By default, the RLP flows are active */
#define HDRMRLPC_FLOW_00_SEQ_LEN_REV_DEFAULT    21 /* Default Sequence Length Reverse RLP FLow 00 */
#define HDRMRLPC_FLOW_00_RLP_ID_LEN_REV_DEFAULT  1 /* Default RLP ID Length for Reverse RLP Flow 00 */
#define HDRMRLPC_FLOW_00_RLP_ID_REV_DEFAULT      0 /* Default RLP ID for Reverse RLP Flow 00 */

/* Default values for Rev RLP flow > 0x00 */
#define HDRMRLPC_ACTIVE_REV_DEFAULT         0    /* RLP flows > 0x00 inactive by default */
//#define HDRMRLPC_SEQ_LEN_REV_DEFAULT        0
//#define HDRMRLPC_RLP_ID_REV_DEFAULT         0
//#define HDRMRLPC_RLP_ID_LEN_REV_DEFAULT     0

/* Note:- SeqLenFwd simple attrib (HDRMRLPC_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID)  *
 * is defined only for MFPA/EMPA. In MMPA SeqLengthFwd is a complex attrib  *
 * (HDRMRLPC_MMPA_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID) and it has the same attrib *
 * id as HDRMRLPC_FLOW_NN_IDENTIFICATION_REV_ATTRIB_ID.
 */

#define HDRMRLPC_SEQ_LEN_ATTRIB_VAL_SIZE 24
#define HDRMRLPC_SEQ_LEN_ATTRIB_MIN_LEN HDRMRLPC_MIN_ATTRIB_LEN + \
                                   HDRMRLPC_SEQ_LEN_ATTRIB_VAL_SIZE

#define HDRMRLPC_SAR_SEQ_LEN_FWD_DEFAULT       21 /* Default SAR Sequence Length */
#define HDRMRLPC_SAR_SEQ_LEN_SHORT_FWD_DEFAULT 21 /* Default SAR Sequence Length short */

/* ----------------------------------------------------------------------------
 * Default FlowNN Timers Fwd 
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_TIMER_FWD_ATTRIB_ID 0x0300 
#define HDRMRLPC_TIMERS_ATTRIB_VAL_SIZE 40
#define HDRMRLPC_MMPA_TIMERS_FWD_ATTRIB_VAL_SIZE 56
#define HDRMRLPC_TIMERS_ATTRIB_MIN_LEN HDRMRLPC_MIN_ATTRIB_LEN + \
                                      HDRMRLPC_TIMERS_ATTRIB_VAL_SIZE
#define HDRMRLPC_MMPA_TIMERS_FWD_ATTRIB_MIN_LEN  HDRMRLPC_MIN_ATTRIB_LEN + \
                                      HDRMRLPC_MMPA_TIMERS_FWD_ATTRIB_VAL_SIZE

#define HDRMRLPC_ABORT_TIMER_FWD_DEF 0x01F4 /* Default Forward Abort Timer (500ms) */
#define HDRMRLPC_FLUSH_TIMER_FWD_DEF 0x012C /* Default Forward Flush Timer: Since 
        this is the Forward link RLP, it does not transmit and so the flush timer 
        value is not relevant (300ms) */

#define HDRMRLPC_NAKDELAY_TIMER_FWD_DEF 0x0000 /* Default Nak Delay Timer. This 
        field is defined only for MMPA, for EMPA/MFPA a new attribute has been 
        backported (HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_ATTRIB_ID) */

/* ----------------------------------------------------------------------------
 * Default FlowNN Timers Rev 
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_TIMER_REV_ATTRIB_ID 0x0400
#define HDRMRLPC_ABORT_TIMER_REV_DEF 0x01F4 /* Default Reverse Abort Timer: Since reverse link RLP, this RLP is a transmitter. The abort timer value is not relevant */
#define HDRMRLPC_FLUSH_TIMER_REV_DEF 0x012C /* Default Reverse Flush Timer */

/* ----------------------------------------------------------------------------
 * Default FlowNN Reservation Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_RESERVATION_FWD_ATTRIB_ID 0x0500
#define HDRMRLPC_RES_ATTRIB_MIN_VAL_SIZE 16  /* default min is 24 for flow00 */
#define HDRMRLPC_RES_ATTRIB_MIN_LEN HDRMRLPC_MIN_ATTRIB_LEN + \
                                    HDRMRLPC_RES_ATTRIB_MIN_VAL_SIZE

/* Default values for Fwd RLP flow 0x00 */
#define HDRMRLPC_FLOW_00_RESERVATION_COUNT_FWD_DEF 0x01
#define HDRMRLPC_FLOW_00_RESERVATION_LABEL_FWD_DEF 0xFF

/* Default values for Fwd RLP flow > 0x00 */
#define HDRMRLPC_RESERVATION_COUNT_FWD_DEF 0x00
#define HDRMRLPC_MAX_RES_COUNT_FWD 20
/* ----------------------------------------------------------------------------
 * Default FlowNN Reservation Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_RESERVATION_REV_ATTRIB_ID 0x0600

/* Default values for Rev RLP flow 0x00 */
#define HDRMRLPC_FLOW_00_RESERVATION_COUNT_REV_DEF 0x01
#define HDRMRLPC_FLOW_00_RESERVATION_LABEL_REV_DEF 0xFF

/* Default values for Rev RLP flow > 0x00 */
#define HDRMRLPC_RESERVATION_COUNT_REV_DEF 0x00
#define HDRMRLPC_MAX_RES_COUNT_REV 20

/* ----------------------------------------------------------------------------
 * Default ReservationKK QOS Request Fwd 
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID 0x0700
#define HDRMRLPC_RESKK_QOS_ATTRIB_MIN_VAL_SIZE 32
#define HDRMRLPC_RESKK_QOS_ATTRIB_MIN_LEN HDRMRLPC_MIN_ATTRIB_LEN + \
                                    HDRMRLPC_RESKK_QOS_ATTRIB_MIN_VAL_SIZE

#define HDRMRLPC_PROFILE_TYPE_REQ_FWD_DEF 0x00
#define HDRMRLPC_PROFILE_LENGTH_REQ_FWD_DEF 0x0000

/* ----------------------------------------------------------------------------
 * Default ReservationKK QOS Request Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_RES_KK_QOS_REQ_REV_ATTRIB_ID 0x0800
#define HDRMRLPC_PROFILE_TYPE_REQ_REV_DEF 0x00
#define HDRMRLPC_PROFILE_LENGTH_REQ_REV_DEF 0x0000

/* ----------------------------------------------------------------------------
 * Default ReservationKK QOS Response Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_RES_KK_QOS_RSP_FWD_ATTRIB_ID 0x0900
#define HDRMRLPC_PROFILE_TYPE_RSP_FWD_DEF 0x00
#define HDRMRLPC_PROFILE_LENGTH_RSP_FWD_DEF 0x0000

/* ----------------------------------------------------------------------------
 * Default ReservationKK QOS Response Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_RES_KK_QOS_RSP_REV_ATTRIB_ID 0x0A00
#define HDRMRLPC_PROFILE_TYPE_RSP_REV_DEF 0x00
#define HDRMRLPC_PROFILE_LENGTH_RSP_REV_DEF 0x0000

/* ----------------------------------------------------------------------------
 * Default Flow NN Flow Protocol Parameters Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_FWD_ATTRIB_ID 0x0B00
#define HDRMRLPC_PROT_PARM_ATTRIB_MIN_VAL_SIZE 24
#define HDRMRLPC_PROT_PARM_REC_FWD_MIN_VAL_SIZE 50

/* ----------------------------------------------------------------------------
 * Default Flow NN Flow Protocol Parameters Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_REV_ATTRIB_ID 0x0C00
#define HDRMRLPC_PROT_PARM_REC_REV_MIN_VAL_SIZE 43

/* ----------------------------------------------------------------------------
 * Default Flow NN Route Protocol Parameters Fwd
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_FWD_ATTRIB_ID 0x0D00

/* ----------------------------------------------------------------------------
 * Default Flow NN Route Protocol Parameters Rev
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_REV_ATTRIB_ID 0x0E00

/* ----------------------------------------------------------------------------
 * Default AT Supported Flow Protocol Parameters PP
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_AT_SUP_FLOW_PROT_PARM_PP_ATTRIB_ID 0x0F00

/* ----------------------------------------------------------------------------
 * Default AT Supported Route Protocol Parameters PP
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_AT_SUP_ROUTE_PROT_PARM_PP_ATTRIB_ID 0x1000



/* ----------------------------------------------------------------------------
 * Default Supported Higher Layer Protocols
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_SUPPORTED_HIGHER_LAYER_PROT_ATTRIB_ID 0x0000
#define HDRMRLPC_SHLP_ATTRIB_MIN_VAL_SIZE 16
#define HDRMRLPC_SHLP_ATTRIB_MIN_LEN HDRMRLPC_MIN_ATTRIB_LEN + \
                                     HDRMRLPC_SHLP_ATTRIB_MIN_VAL_SIZE

#define HDRMRLPC_PROTOCOL_COUNT_DEF 0x01 
#define HDRMRLPC_PROTOCOL_IDENTIFIER_DEF 0x01

/* ----------------------------------------------------------------------------
 * Default AT Supported QOS Profiles (ONLY ONE ALLOWED)
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_AT_SUPPORTED_QOS_PROFILES_ATTRIB_ID 0x0002
#define HDRMRLPC_ASQP_ATTRIB_MIN_VAL_SIZE 16
#define HDRMRLPC_ASQP_ATTRIB_MIN_LEN HDRMRLPC_MIN_ATTRIB_LEN + \
                                     HDRMRLPC_ASQP_ATTRIB_MIN_VAL_SIZE

#define HDRMRLPC_AT_SQP_PROFILE_COUNT_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default AN Supported QOS Profiles  (ONLY ONE ALLOWED)
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_AN_SUPPORTED_QOS_PROFILES_ATTRIB_ID 0x0003
#define HDRMRLPC_AN_SQP_PROFILE_COUNT_DEF 0x00

/* ----------------------------------------------------------------------------
 * Default MaxRLPFlows Attribute
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_MAX_RLP_FLOWS_ATTRIB_ID 0x0004
#define HDRMRLPC_MAX_LINK_FLOWS_ATTRIB_ID 0x0004

#define HDRMRLPC_MAX_RLP_ATTRIB_MIN_VAL_SIZE 40
#define HDRMRLPC_MAX_NUM_RLP_FLOWS_FWD_DEF 0x06
#define HDRMRLPC_MAX_NUM_RLP_FLOWS_REV_DEF 0x06
#define HDRMRLPC_MAX_ACTIVATED_RLP_FLOWS_FWD_DEF 0x03
#define HDRMRLPC_MAX_ACTIVATED_RLP_FLOWS_REV_DEF 0x03
#define HDRMRLPC_MAX_NUM_RLP_FLOWS_FWD_MAX 0x1F
#define HDRMRLPC_MAX_NUM_RLP_FLOWS_REV_MAX 0x1F

/* ----------------------------------------------------------------------------
 * Default MaxReservations Attribute
 * --------------------------------------------------------------------------*/
#define HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID 0x0005
#define HDRMRLPC_MAX_RESERVATIONS_ATTRIB_MIN_VAL_SIZE 40
#define HDRMRLPC_MAX_NUM_RESERVATIONS_FWD_DEF 0xFF
#define HDRMRLPC_MAX_NUM_RESERVATIONS_REV_DEF 0xFF
#define HDRMRLPC_MAX_NUM_OPEN_RESERVATIONS_FWD_DEF 0x0F
#define HDRMRLPC_MAX_NUM_OPEN_RESERVATIONS_REV_DEF 0x0F

/* <EJECT> */ 

#endif /* FEATURE_HDR_QOS */
#endif /* HDRMRLPCI_H */
