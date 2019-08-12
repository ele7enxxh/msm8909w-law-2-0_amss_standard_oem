/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H I G H     D A T A     R A T E

                  MULTI  RADIO   LINK   PROTOCOL  CONFIGURATION  FILE

GENERAL DESCRIPTION
  This file contains the implementation of the IS-856 Multi Radio Link
  Protocol (HDR RLP). This module forms the interface for operations
  regarding all MultiFlow RLP instances or for selecting a particular
  MRLP instance.
  
  
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2004 - 2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrmrlpc.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ------------------------------------------------------- 
05/09/16   vko   Reduce excessive F3 logging
12/14/15   vko   Fixed KW issues 
07/09/14   cnx   Support NV refresh without reset.
02/03/14   vko   Fixed boundary condition for no. of reservation label for the reverse flow
12/10/13   sat   Changed memcpy() to assignment or memscpy() (safer version).
11/07/11   vlc   Fixed Klocwork warnings. 
09/30/11   cnx   Changed %P in F3 to 0x%x.
06/03/11   cnx   Changed eHRPD advertisement conditions.
12/15/10   pba   Added ehrpd over Rev B support.
11/09/10   cnx   Added sanity check before casting to an enum value. 
10/24/10   pxu   Memory heap changes. 
09/29/10   cnx   Fixed a GAUP timer issue.
09/23/10   pxu   Reverted mem heap changes to accomodate for CS. 
09/23/10   pxu   Memory heap changes.  
09/22/10   cnx   Fixed a bug that GAUP timer did not stop when there are 
                 pending timers.
08/10/10   cnx   Fixed bugs in GAUP timer. 
06/21/10   cnx   Fixed compiler warnings.
12/20/09   pba   Send ULN only when data session is transferable
12/07/09   pba   Desired values need to be updated everytime session negotiation is initiated
10/30/09   pba   Added ehrpd support.
09/29/09   wsh   Fixed bug handling FlowNNTimers
09/20/09   sju   Fixed KW warnings.
09/18/09   wsh   CMI: support changes from modem DATA
07/22/09   wsh   Check FlowNNTimers and MaxAbortTimer before send ConfResp
05/13/09   pba   Allow MaxSupportedMaxCID value of 0x0F
03/03/09   pba   Fixed data type inconsistency.
02/02/09   wsh   Fixed Artemis target not accepting 1 for 
                 FlowNNPhysicalLayerNakEnableRev attribute
11/04/08   wsh   Fixed ERR_FATAL when MRLP is not bound to stream
12/18/08   wsh   Fixed: some informative log message marked as errors
12/10/08   smd   Replaced assert.h with amssassert.h
12/08/08   kss   Indent fixes for lint.
12/04/08   pba   Lint and Compiler warning fixes
11/21/08   pba   Fixed issue where AT is sending a ConfigRequest message 
                 without any attributes.
11/04/08   sju   Accepted KKIDLEState atribute for ResLabel 0xff in SCP phase 
10/01/08   wsh   Fixed critical/high lint warnings
10/01/08   pba   Do not ignore PPP_FREE_AUTHENTICATION_SUPPORT_ATTRIB, however
                 accept only default value.
09/24/08   sju   Fixed bug where the AT doesn't exit the critical section.
09/17/08   sju   Fixed bug where GAUP timer doesn't expire.
08/15/08   wsh   Max abort time value set to 0 after AT reset
08/06/08   pba   Klockwork fixes round 2.
07/03/08   pba   Fixed check that makes sure NakDelayTimer is < AbortTimer
07/02/08   pba   Size of FlowNNTimersFwd is different for MMPA and EMPA/MFPA
06/30/08   pba   Allow FlowNNPhyLayerNakEnabled = 1 for MMPA.
05/22/08   pba   Klockwork fixes
03/28/08   sju   Close session when pending_gaup_req_free_q is full
11/26/07   sju   Fixed incorrect index used in rev_res_info table 
                 after processing AUR of rev QosRsp attribute
10/17/07   pba   Updates based on code review comments.
10/17/07   pba   Look at negotiated stream app subtype instead of current
                 while checking whether configured flow is desired
10/03/07   wsh   MRLPC GAUP pending queue not getting freed
09/13/07   sju   Avoided NULL referencing and buffer overrun
09/11/07   wsh   Removed lint high warnings
08/22/07   wsh   Fixed bug where GAUP response is ignored under some
                 situations when GAUP is sent over RTC reliably.
08/16/07   wsh   Removed compiler and lint critical warnings
08/15/07   pba   Accept only default value for PPPFreeAuthenticationSupport
                 attribute.
07/30/07   pba   Featurized MMPA code under FEATURE_HDR_REVB.
07/30/07   pba   Obsoleted feature FEATURE_HDR_EMPA_SUPPORT_STD_V115_ATTRIB_ID
07/30/07   pba   Set inconfig to current before sending AUR.
07/17/07   pba   RelB MMPA support.
06/25/07   ac    Fixed the RSP error when TwoRoutesSupported=0 
06/22/07   sju   Fixed a problem that AT doesn't process GAUP_FAILED ind 
                 in the case of SLP CB return failures 
06/14/07   wsh   Featurize RSP support
05/29/07   wsh   Disallow flow00 to be deactivated for MPA
05/01/07   pba   Look at negotiated stream app subtype instead of current 
                 while checking whether configured flow is desired.
05/01/07   pba   Check if flow00 is activated instead of deactivated.
04/09/07   sju        Added support to handle back to back Qos Request.
02/23/07   wsh   Disallow flow01 to be activated and flow00 to be deactivated
02/12/07   mpa   Fixed RVCT warnings.                         
01/23/07   yll   Fixed a problem where table index from current 
                 instance is used to access in_config instance of 
                 ResKKQoSRequest.
01/11/07   pba   Modified the interfaces that are used to get active 
                 forward and reverse RLPs, to maintain backward compatibility.
01/09/07   ac    Fixed the AT crash due to invalid MaxID
12/19/06   yll   Don't store ResvKKQoSResponse and ResvKKIdleState 
                 attributes if QoSRequest for this reservation label 
                 has not been made. Also clear the verno table when
                 QoS is released.
11/30/06   yll   MaxReservations attribute is not GAUPable
11/30/06   pba   Fixed EMPA featurization.
11/17/06   yll   Fixed processing of Config Response for EMPA attributes
11/16/06   yll   Don't include EMPA attributes in AT-initiated Config 
                 Request if EMPA is not negotiated.
11/02/06   yll   Fixed a problem that AT closes session when receiving
                 an unsupported FlowNNPhysicalLayerNakEnableRev value
                 in a GAUP message.
10/19/06   pba   Fixed an incorrect initialization in AT config init.
10/18/06   wsh   Fixed a problem where AT will not responde to ConfigReq
                 if ReservationKKQosFwd/RevReq attribute is included
10/13/06   yll   Merging from EMPA sandbox onto the tip  
09/26/06   sju   Fixed E_AGAIN case in hdrmrlpc_gaup_req_tx_outcome_cb 
                 to return proper indication to app.
09/21/06   yll   Fixed a problem that AT rejects the GAUPing of 
                 FlowNNFlowProtocolSDUFwd Attribute.
09/05/06   sju   Added support to handle E_TIMER_EXP case in 
                 hdrmrlpc_gaup_req_tx_outcome_cb()
08/16/06   yll   Fixed a compiler warning.
08/08/06 ksu/yll EMPA: ack-based RLP exclusion, V2.0 negotiation
07/26/06   pba   Fixed featurization issue with EMPA code.
07/24/06   mpa   Replaced hdrds_queue_ind with hdrds_queue_ind_ext.
07/21/06   yll   Use NV item to config AT to support V1.15 or 2.0 EMPA
07/18/06   dna   Major RASM changes to allow reliable QoS call setup.
07/18/06   yll   Added support for RoHC.
07/06/06   yll   Changed the attrib_len from uint8 to uint16.
05/11/06   pba   Update SCMDB only when GAUP succeeds.
05/10/06   yll   Added EMPA suport
04/21/06   hal   Clear ConfigResponse and GAUPResponse timers on SessionClose
04/07/06   hal   Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
04/03/06   yll   Updated attrib_val_offset when unpacking the attribute
                 MaxReservations.
03/30/06   yll   Fixed the problem that AT doesn't store AT initiated
                 MaxRLPFlows and MaxReservation attributes into the
                 negotiated instance even after AN accepted them.
03/16/06   yll   Removed the support of 6 bit SeqLength + RLPIDLength 
                 in the FlowNNIdentification attribute.
03/06/06   yll   Set the value of res_idx in hdrmrlpc_map_cfg_rl()
                 properly to avoid uninitialized reservation index.
02/28/06   yll   Removed the obsolete attributes. Changed the value of
                 MaxNumLinkFlowsFwd/Rev in Config Request message to 
                 0x1F as per standard specification.
02/21/06   yll   Changed the debug messages.
02/07/06   yll   Changes to process multiple values for physical layer
                 NAK enable attribute and to accept the preferred one.
02/07/06   yll   Update the verno only when attributes are negotiated.
02/06/06   yll   Set default to support Physical Layer Nak Enabled Rev.
02/01/06   pba   Set negotiated instance to current on session changed 
                 indication.
02/01/06   yll   Initialize the negotiated instance with current 
                 instance upon power up.
01/25/06   yll   When releasing QoS, remove the complete Reservation
                 information for this KK.
01/23/06   yll   Fixed the packing of TransmitAbortTimer, unpacking of
                 ReservationKK attributes.
01/19/06   yll   Clean the QoS request info after it's released.
12/13/05   pba   Changes to support multiple personality negotiation
12/07/05   yll   Changed the function name in maping the reservation
                 label in its current table index.
11/23/05   yll   Added interface to read MaxRLPFlows and MaxServations.
11/21/05   yll   When AT initiates the release of QoS GAUP request and 
                 AN accepts it, remove the RES qos_request entry from 
                 the current, in_config and config_result table. 
11/16/05   yll   Added support for new attributes: XmitAbortTimer, 
                 DOSAllowed, MaxRLPFlows and MaxReservations.
10/28/05   vas   Fixed packing of Phys_layer_nak_enable attribute.
10/04/05   pba   Added support to read/write config parameters from SCMDB/EFS
09/15/05   mpa   Fixed GAUP config.
08/19/05   mpa   Reset config flags on session open
08/07/05   mpa   Added AT Initiated GAUP support + new ResKK format.
03/28/05   mpa   Fixed Config Rsp packing.
03/07/05   sy    Fixed Lint errors.
01/20/05   mpa   Fixed typo in FwdNakEnable GAUP validation
11/16/04   mpa   Fixed flowNN bounds checking.
11/04/04   mpa   Added GAUP support and attribute version numbering.
10/19/04   mpa   Replaced hdrscp_session_is_open with hdrsmp_session_is_open.
09/20/04   mpa   Fixed timer attribute handling and configuration with
                 empty config file.
04/05/04   mpa   Added Attributes Negotiation handling.
03/08/04   vas   Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_mem.h"
#if defined(FEATURE_HDR_QOS)
#include "amssassert.h"
#include "err.h"
#include "rex.h"
#include "msg.h"

#include "hdrbit.h"
#include "hdrdebug.h"
#include "hdrhai.h" 
#include "hdrmci.h"
#include "hdrstream.h"
#include "hdrtrace.h"
#include "hdrutil.h"

#include "hdrscp.h"
#include "hdrsmp.h"

#include "hdrmrlpc.h"
#include "hdrscmdb.h"
#include "hdrhmp.h"

#include "hdrds_api.h"
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRMRLPC_ATTRIB_UPDATE_REQ_MSG  0x52
#define HDRMRLPC_MSG_PRIORITY 40

#define HDRMRLPC_ATTRIB_UPDATE_RSP_TIMER_MS 2000 
  /* Time to wait for an AttributeUpdateResponse (accept or reject) */

#define HDRMRLPC_MSG_ID_SIZE               8
#define HDRMRLPC_TRANSACTION_ID_SIZE       8

#define HDRMRLPC_MAX_TOTAL_RSV \
  ( HDRMRLPC_N_RES_LABELS_MAX_FWD + HDRMRLPC_N_RES_LABELS_MAX_REV )
 

/*-----------------------------------------------------------------------------
                               U N P A C K I N G
-----------------------------------------------------------------------------*/
#define HDRMRLPC_UNPACK_MSG_ID( msg_ptr, val_ptr )  \
          HDRBIT_UNPACK8( (msg_ptr), (val_ptr), 0, 8 )

#define HDRMRLPC_UNPACK_TRANSACTION_ID( msg_ptr, val_ptr )  \
          HDRBIT_UNPACK8( (msg_ptr), (val_ptr), 8, 8 )

#define HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( msg_ptr, val_ptr, \
                                              aoffset, voffset) \
      (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 0), 8))
          
/*----------------------------------------- 
  FlowNN Identification Fwd/Rev unpacking 
-----------------------------------------*/          
#define HDRMRLPC_UNPACK_CON_REQ_ID_ACTIVE( msg_ptr, val_ptr, \
                                           aoffset, voffset) \
      (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_UNPACK_CON_REQ_ID_SEQ_LEN(msg_ptr, val_ptr, \
                                           aoffset, voffset) \
      (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16), 8))

#define HDRMRLPC_UNPACK_CON_REQ_ID_RLPID_LEN(msg_ptr, val_ptr, \
                                            aoffset, voffset) \
      (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 24), 8))

#define HDRMRLPC_UNPACK_CON_REQ_ID_RLPID(msg_ptr, val_ptr, \
                                         aoffset, voffset, len) \
      (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 32), len))

/*----------------------------------------- 
  FlowNN SequenceLength Fwd unpacking (MMPA)
-----------------------------------------*/          
#define HDRMRLPC_UNPACK_CON_REQ_SAR_SEQ_LEN( msg_ptr, val_ptr, \
                                           aoffset, voffset) \
      (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_UNPACK_CON_REQ_SAR_SEQ_LEN_SHORT( msg_ptr, val_ptr, \
                                           aoffset, voffset) \
      (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16), 8))

/*----------------------------------------- 
  FlowNN Timers Fwd/Rev unpacking 
-----------------------------------------*/
#define HDRMRLPC_UNPACK_CON_REQ_TIMERS_ABORT( msg_ptr, val_ptr, \
                                              aoffset, voffset) \
      (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 16))

#define HDRMRLPC_UNPACK_CON_REQ_TIMERS_FLUSH( msg_ptr, val_ptr, \
                                              aoffset, voffset) \
      (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 24), 16))
      
#define HDRMRLPC_UNPACK_CON_REQ_TIMERS_NAK_DELAY( msg_ptr, val_ptr, \
                                                  aoffset, voffset) \
      (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 40), 16))
      
/*----------------------------------------- 
  FlowNN Reservation Fwd/Rev unpacking 
-----------------------------------------*/
#define HDRMRLPC_UNPACK_CON_REQ_RSV_RES_COUNT(msg_ptr, val_ptr, \
                                              aoffset, voffset) \
  (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_UNPACK_CON_REQ_RSV_RES_LABEL(msg_ptr, val_ptr, \
                                              aoffset, voffset, i) \
  (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16 + (i*8)), 8));

/*----------------------------------------- 
  FlowNN Transmit Abort Timer unpacking 
-----------------------------------------*/
#define HDRMRLPC_UNPACK_CON_REQ_XMIT_ABORT_TIMER(msg_ptr, val_ptr, \
                                                 aoffset, voffset) \
  (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 0), 16))

/*----------------------------------------- 
  Supported Higher Layer Prot unpacking 
-----------------------------------------*/
#define HDRMRLPC_UNPACK_CON_REQ_SHLP_COUNT(msg_ptr, val_ptr, \
                                              aoffset, voffset) \
  (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_UNPACK_CON_REQ_SHLP_ID(msg_ptr, val_ptr, \
                                        aoffset, voffset, i) \
  (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16 + (i*8)), 8)); 

/*----------------------------------------- 
  Supported AN QoS Profiles unpacking 
-----------------------------------------*/
#define HDRMRLPC_UNPACK_CON_REQ_ASQP_COUNT(msg_ptr, val_ptr, \
                                              aoffset, voffset) \
  (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_UNPACK_CON_REQ_ASQP_TYPE(msg_ptr, val_ptr, \
                                          aoffset, voffset) \
  (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 0 ), 8)); 
  
#define HDRMRLPC_UNPACK_CON_REQ_ASQP_LENGTH(msg_ptr, val_ptr, \
                                          aoffset, voffset) \
  (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8 ), 8)); 
  
#define HDRMRLPC_UNPACK_CON_REQ_ASQP_VALUE(msg_ptr, val_ptr, \
                                          aoffset, voffset, j) \
  (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16 + (j*8)), 8)); 
  
/*----------------------------------------- 
  Max Reservations unpacking 
-----------------------------------------*/
#define HDRMRLPC_UNPACK_CON_REQ_MAX_RSV_FWD(msg_ptr, val_ptr, \
                                            aoffset, voffset) \
(HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_UNPACK_CON_REQ_MAX_RSV_REV(msg_ptr, val_ptr, \
                                            aoffset, voffset) \
(HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16), 8))

#define HDRMRLPC_UNPACK_CON_REQ_MAX_OPEN_RSV_FWD(msg_ptr, val_ptr, \
                                                 aoffset, voffset) \
(HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 24), 8))

#define HDRMRLPC_UNPACK_CON_REQ_MAX_OPEN_RSV_REV(msg_ptr, val_ptr, \
                                                 aoffset, voffset) \
(HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 32), 8))

/*------------------------------------------------------------ 
 ReservationKK Qos Profile Request/Response Fwd/Rev unpacking 
------------------------------------------------------------*/
#define HDRMRLPC_UNPACK_CON_REQ_RESKK_TYPE(msg_ptr, val_ptr, \
                                         aoffset, voffset) \
 (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_UNPACK_CON_REQ_RESKK_LENGTH(msg_ptr, val_ptr, \
                                            aoffset, voffset) \
 (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16), 16))
  
#define HDRMRLPC_UNPACK_CON_REQ_RESKK_VAL(msg_ptr, val_ptr, \
                                        aoffset, voffset, i) \
 (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 32 + (i*8)), 8));

          
/*-----------------------------------------------------------------------------
                                 P A C K I N G
-----------------------------------------------------------------------------*/
#define HDRMRLPC_PACK_MSG_ID( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 0, 8 )

#define HDRMRLPC_PACK_TRANSACTION_ID( ptr, val )  \
          HDRBIT_PACK8( (ptr), (val), 8, 8 )

/* Configuration Request */
#define HDRMRLPC_PACK_CON_REQ_ATTRIB_LEN(msg_ptr, val_ptr, aoffset) \
        (HDRBIT_PACK8((msg_ptr),(val_ptr),(aoffset + 0), 8))

#define HDRMRLPC_PACK_CON_REQ_ATTRIB_ID(msg_ptr, val_ptr, aoffset) \
        (HDRBIT_PACK16((msg_ptr),(val_ptr),(aoffset + 8), 16))

#define HDRMRLPC_PACK_CON_REQ_ATTRIB_VALUE( msg_ptr, val_ptr, \
                                              aoffset, voffset) \
      (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 0), 8))
      
/* Configuration Response */
#define HDRMRLPC_VALUE_ID_SIZE 8

#define HDRMRLPC_CON_RSP_ATTRIB_LEN (HDRMRLPC_MIN_ATTRIB_LEN + \
                                     HDRMRLPC_VALUE_ID_SIZE)

#define HDRMRLPC_CON_RSP_ATTRIB_16_LEN (HDRMRLPC_MIN_ATTRIB_LEN + \
                                     HDRMRLPC_SIMPLE_ATTRIB_16_SIZE)


/* Config Response Length, not including the length field */
#define HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES \
        ((HDRMRLPC_ATTRIB_ID_SIZE + HDRMRLPC_VALUE_ID_SIZE) \
        / HDRMRLPC_BITS_PER_BYTE)

#define HDRMRLPC_CON_RSP_ATTRIB_16_LEN_BYTES \
        ((HDRMRLPC_ATTRIB_ID_SIZE + HDRMRLPC_SIMPLE_ATTRIB_16_SIZE) \
        / HDRMRLPC_BITS_PER_BYTE)

#define HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(msg_ptr, val_ptr, aoffset) \
        (HDRBIT_PACK8((msg_ptr),(val_ptr),(aoffset + 0), 8))

#define HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(msg_ptr, val_ptr, aoffset) \
        (HDRBIT_PACK16((msg_ptr),(val_ptr),(aoffset + 8), 16))

#define HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE( msg_ptr, val_ptr, \
                                            aoffset) \
      (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + 24), 8))


/*----------------------------------------- 
  FlowNN Transmit Abort Timer Rev packing 
-----------------------------------------*/ 
#define HDRMRLPC_PACK_CON_RSP_XMIT_ABORT_TIMER( msg_ptr, val_ptr, \
                                                aoffset) \
      (HDRBIT_PACK16((msg_ptr), (val_ptr), ((aoffset) + 24), 16))
      
/*----------------------------------------- 
  FlowNN Identification Fwd/Rev packing 
-----------------------------------------*/ 
#define HDRMRLPC_CREQ_ID_ATTRIB_MIN_LEN_BYTES \
        ((HDRMRLPC_ID_ATTRIB_MIN_VAL_SIZE + HDRMRLPC_ATTRIB_ID_SIZE) \
        / HDRMRLPC_BITS_PER_BYTE)
        
#define HDRMRLPC_CREQ_ID_ATTRIB_MAX_LEN_BYTES \
        ((HDRMRLPC_ID_ATTRIB_MAX_VAL_SIZE + HDRMRLPC_ATTRIB_ID_SIZE) \
        / HDRMRLPC_BITS_PER_BYTE)
         
#define HDRMRLPC_PACK_CON_REQ_ID_ACTIVE( msg_ptr, val_ptr, \
                                           aoffset, voffset) \
      (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_PACK_CON_REQ_ID_SEQ_LEN(msg_ptr, val_ptr, \
                                           aoffset, voffset) \
      (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16), 8))

#define HDRMRLPC_PACK_CON_REQ_ID_RLPID_LEN(msg_ptr, val_ptr, \
                                            aoffset, voffset) \
      (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 24), 8))

#define HDRMRLPC_PACK_CON_REQ_ID_RLPID(msg_ptr, val_ptr, \
                                         aoffset, voffset, len) \
      (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 32), len))

/*----------------------------------------- 
  FlowNN Timers Fwd/Rev packing 
-----------------------------------------*/
#define HDRMRLPC_CREQ_TIMERS_ATTRIB_LEN_BYTES \
        ((HDRMRLPC_TIMERS_ATTRIB_VAL_SIZE + HDRMRLPC_ATTRIB_ID_SIZE) \
        / HDRMRLPC_BITS_PER_BYTE)
        
#define HDRMRLPC_PACK_CON_REQ_TIMERS_ABORT( msg_ptr, val_ptr, \
                                            aoffset, voffset) \
      (HDRBIT_PACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 16))

#define HDRMRLPC_PACK_CON_REQ_TIMERS_FLUSH( msg_ptr, val_ptr, \
                                            aoffset, voffset) \
      (HDRBIT_PACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 24), 16))
      
/*----------------------------------------- 
  FlowNN Reservation Fwd/Rev packing 
-----------------------------------------*/
#define HDRMRLPC_PACK_CON_REQ_RSV_RES_COUNT(msg_ptr, val_ptr, \
                                              aoffset, voffset) \
  (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_PACK_CON_REQ_RSV_RES_LABEL(msg_ptr, val_ptr, \
                                              aoffset, voffset, i) \
  (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16 + (i*8)), 8));

/*----------------------------------------- 
  Supported Higher Layer Prot packing 
-----------------------------------------*/
#define HDRMRLPC_PACK_CON_REQ_SHLP_COUNT(msg_ptr, val_ptr, \
                                              aoffset, voffset) \
  (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_PACK_CON_REQ_SHLP_ID(msg_ptr, val_ptr, \
                                        aoffset, voffset, i) \
  (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16 + (i*8)), 8)); 

/*----------------------------------------- 
  ResKK Qos Profile Fwd/Rev packing 
-----------------------------------------*/
#define HDRMRLPC_PACK_CON_REQ_RESKK_TYPE(msg_ptr, val_ptr, \
                                         aoffset, voffset) \
 (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_PACK_CON_REQ_RESKK_LENGTH(msg_ptr, val_ptr, \
                                            aoffset, voffset) \
 (HDRBIT_PACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16), 16))
  
#define HDRMRLPC_PACK_CON_REQ_RESKK_VAL(msg_ptr, val_ptr, \
                                        aoffset, voffset, i) \
 (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 32 + (i*8)), 8));

/*----------------------------------------- 
  Max RLP Flows packing 
-----------------------------------------*/
#define HDRMRLPC_PACK_CON_REQ_MAX_RLP_FWD(msg_ptr, val_ptr, \
                                          aoffset, voffset) \
 (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_PACK_CON_REQ_MAX_RLP_REV(msg_ptr, val_ptr, \
                                          aoffset, voffset) \
 (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16), 8))

#define HDRMRLPC_PACK_CON_REQ_MAX_ACT_RLP_FWD(msg_ptr, val_ptr, \
                                              aoffset, voffset) \
 (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 24), 8))

#define HDRMRLPC_PACK_CON_REQ_MAX_ACT_RLP_REV(msg_ptr, val_ptr, \
                                              aoffset, voffset) \
 (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 32), 8))

/*----------------------------------------- 
  Max Reservations packing 
-----------------------------------------*/
#define HDRMRLPC_PACK_CON_REQ_MAX_RSV_FWD(msg_ptr, val_ptr, \
                                          aoffset, voffset) \
 (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 8), 8))

#define HDRMRLPC_PACK_CON_REQ_MAX_RSV_REV(msg_ptr, val_ptr, \
                                          aoffset, voffset) \
 (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 16), 8))

#define HDRMRLPC_PACK_CON_REQ_MAX_OPEN_RSV_FWD(msg_ptr, val_ptr, \
                                               aoffset, voffset) \
 (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 24), 8))

#define HDRMRLPC_PACK_CON_REQ_MAX_OPEN_RSV_REV(msg_ptr, val_ptr, \
                                               aoffset, voffset) \
 (HDRBIT_PACK8((msg_ptr), (val_ptr), ((aoffset) + (voffset) + 32), 8))

#define HDRMRLPC_PROT_PARM_LARGE_CIDS_SIZE  1 
#define HDRMRLPC_PROT_PARM_FEED_BACK_FOR_INCL_SIZE 1 
#define HDRMRLPC_PROT_PARM_FEED_BACK_FOR_SIZE  5
#define HDRMRLPC_PROT_PARM_TIMER_BASED_COMP_SIZE 1
#define HDRMRLPC_PROT_SUPPORTED_SIZE  8
#define HDRMRLPC_PROT_VALUE_LEN_SIZE  8
/* RoHC parameter is of 112 bits */
#define HDRMRLPC_PROT_ROHC_VALUE_LEN  112

/*=============================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* 
   Keeps tracks of a given attribute's configuration 

   - at_requested: whether the AT proposed this attribute          
   - requested: whether the AN proposed this attribute
   - accepted: whether the AT accepted this attribute
   - value: value/valueID of the accepted attribute
*/
typedef struct
{
  boolean at_requested;
  boolean requested;
  boolean is_accepted;   
  uint8 value; /* Value   for simple attributes,
                  ValueID for complex attributes,
                  TransID for ATInit gaup messages */
} hdrmrlpc_config_attrib_result_type;

typedef struct
{
  boolean at_requested;
  boolean requested;
  boolean is_accepted;   
  uint16  value; /* Value   for simple attributes (16 bits),
                  ValueID for complex attributes,
                  TransID for ATInit gaup messages */
} hdrmrlpc_config_attrib_16_result_type;

typedef struct
{
  /* Simple attributes */
  hdrmrlpc_config_attrib_result_type nak_enable;
  hdrmrlpc_config_attrib_result_type higher_layer_prot;
  hdrmrlpc_config_attrib_result_type activated;
  hdrmrlpc_config_attrib_result_type simple_sequence_length;
  hdrmrlpc_config_attrib_result_type flow_prot_sdu;
  hdrmrlpc_config_attrib_result_type data_unit;
  hdrmrlpc_config_attrib_result_type route_prot_sdu;
  hdrmrlpc_config_attrib_result_type sim_del_both_routes;
  hdrmrlpc_config_attrib_result_type ooo_del_route_prot;
  hdrmrlpc_config_attrib_result_type nak_delay_time;

  /* Complex Attributes */
  hdrmrlpc_config_attrib_result_type identification;
  hdrmrlpc_config_attrib_result_type timers;
  hdrmrlpc_config_attrib_result_type reservation;
  hdrmrlpc_config_attrib_result_type flow_prot_parm;
  hdrmrlpc_config_attrib_result_type route_prot_parm;
  /* Only defined for MMPA for EMPA/MFPA it is a simple attribute */
  hdrmrlpc_config_attrib_result_type complex_seq_len;

  hdrmrlpc_config_attrib_result_type rlp_priority;
} hdrmrlpc_flow_nn_fwd_config_attrib_result_type;

typedef struct
{
  /* Simple Attributes */
  hdrmrlpc_config_attrib_result_type nak_enable;
  hdrmrlpc_config_attrib_result_type higher_layer_prot;
  hdrmrlpc_config_attrib_result_type phys_layer_nak_enable;
  hdrmrlpc_config_attrib_16_result_type xmit_abort_timer; /* 16 bit value */
  hdrmrlpc_config_attrib_result_type dos_allowed;
  hdrmrlpc_config_attrib_result_type activated;
  hdrmrlpc_config_attrib_result_type simple_sequence_length;
  hdrmrlpc_config_attrib_result_type flow_prot_sdu;
  hdrmrlpc_config_attrib_result_type data_unit;
  hdrmrlpc_config_attrib_result_type route_prot_sdu;

  /* Complex Attributes */
  hdrmrlpc_config_attrib_result_type identification;
  hdrmrlpc_config_attrib_result_type timers;
  hdrmrlpc_config_attrib_result_type reservation;
  hdrmrlpc_config_attrib_result_type flow_prot_parm;
  hdrmrlpc_config_attrib_result_type route_prot_parm;

  hdrmrlpc_config_attrib_result_type rlp_priority;

} hdrmrlpc_flow_nn_rev_config_attrib_result_type;

typedef struct
{
  hdrmrlpc_config_attrib_result_type idle_state;
  hdrmrlpc_config_attrib_result_type qos_request;
  hdrmrlpc_config_attrib_result_type qos_response;

} hdrmrlpc_res_kk_config_attrib_result_type;
          
typedef struct
{
  hdrmrlpc_config_attrib_result_type at_sup_flow_prot_parm;
  hdrmrlpc_config_attrib_result_type at_sup_route_prot_parm;
} hdrmrlpc_prot_parm_pp_config_attrib_result_type;

typedef struct
{
  hdrmrlpc_config_attrib_result_type max_abort_timer_attrib;
  hdrmrlpc_config_attrib_result_type ppp_free_authentication_support;
  hdrmrlpc_config_attrib_result_type two_routes_supported;
  hdrmrlpc_config_attrib_result_type at_nak_delay_supported;

  hdrmrlpc_config_attrib_result_type sup_higher_layer_prot_attrib;
  
  hdrmrlpc_config_attrib_result_type an_sup_qos_profiles;
  hdrmrlpc_config_attrib_result_type max_rlp_flows;
  hdrmrlpc_config_attrib_result_type max_reservations;

  hdrmrlpc_flow_nn_fwd_config_attrib_result_type 
    flow_nn_fwd_attrib[HDRMRLPC_N_MRLP_MAX_FWD];
  hdrmrlpc_flow_nn_rev_config_attrib_result_type 
    flow_nn_rev_attrib[HDRMRLPC_N_MRLP_MAX_REV];

  uint8 num_rsv_fwd;
  hdrmrlpc_res_kk_config_attrib_result_type
    res_kk_fwd_attrib[HDRMRLPC_N_RES_LABELS_MAX_FWD];

  uint8 num_rsv_rev;
  hdrmrlpc_res_kk_config_attrib_result_type
    res_kk_rev_attrib[HDRMRLPC_N_RES_LABELS_MAX_REV];

  hdrmrlpc_prot_parm_pp_config_attrib_result_type
    prot_parm_pp_attrib[HDRMRLP_MAX_PROT_COUNT];

  /* Treat reservation label 0xFF differently than the other labels */
  hdrmrlpc_res_kk_config_attrib_result_type res_kk_fwd_attrib_ff;
  hdrmrlpc_res_kk_config_attrib_result_type res_kk_rev_attrib_ff;
} hdrmrlpc_config_req_result_type;

/* Version number is incremented every time a new attribute is
   processed after powerup. It is reset to 0 when the session
   closes */
typedef struct
{
  uint8 max_abort_timer;
  uint8 ppp_free_authentication_support;
  uint8 two_routes_supported;
  uint8 at_nak_delay_supported;

  /* FlowNN Simple Attributes */
  uint8 nak_enable_fwd[HDRMRLPC_N_MRLP_MAX_FWD];
  uint8 nak_enable_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 higher_layer_prot_fwd[HDRMRLPC_N_MRLP_MAX_FWD];
  uint8 higher_layer_prot_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 phys_link_nak_en_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 xmit_abort_timer_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 dos_allowed_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 activated_fwd[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 activated_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 sequence_length_fwd[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 sequence_length_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 flow_prot_sdu_fwd[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 flow_prot_sdu_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 data_unit_fwd[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 data_unit_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 route_prot_sdu_fwd[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 route_prot_sdu_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 sim_del_both_routes_fwd[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 ooo_del_route_prot_fwd[HDRMRLPC_N_MRLP_MAX_REV];

  /* FlowNN Complex Attributes */
  uint8 identification_fwd[HDRMRLPC_N_MRLP_MAX_FWD];
  uint8 identification_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 timers_fwd[HDRMRLPC_N_MRLP_MAX_FWD];
  uint8 timers_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 reservation_fwd[HDRMRLPC_N_MRLP_MAX_FWD];
  uint8 reservation_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 flow_prot_parm_fwd[HDRMRLPC_N_MRLP_MAX_FWD];
  uint8 flow_prot_parm_rev[HDRMRLPC_N_MRLP_MAX_REV];
  uint8 route_prot_parm_fwd[HDRMRLPC_N_MRLP_MAX_FWD];
  uint8 route_prot_parm_rev[HDRMRLPC_N_MRLP_MAX_REV];

  /* ReservationKK attributes */
  uint8 reskk_idle_state_fwd[HDRMRLPC_N_RES_LABELS_MAX_FWD];
  uint8 reskk_idle_state_rev[HDRMRLPC_N_RES_LABELS_MAX_REV];
  /* ReservationKK attributes for rsv label 0xff */
  uint8 reskk_idle_state_fwd_ff;
  uint8 reskk_idle_state_rev_ff;
  /* QoS Request for AT Init only */
  uint8 reskk_qos_req_fwd[HDRMRLPC_N_RES_LABELS_MAX_FWD];
  uint8 reskk_qos_req_rev[HDRMRLPC_N_RES_LABELS_MAX_REV];
  /* QoS Response for AN Init only */
  uint8 reskk_qos_rsp_fwd[HDRMRLPC_N_RES_LABELS_MAX_FWD];
  uint8 reskk_qos_rsp_rev[HDRMRLPC_N_RES_LABELS_MAX_REV];

  /* AT supported Flow Protocol Parameters PP */
  uint8 at_sup_flow_prot_parm_pp[HDRMRLP_MAX_PROT_COUNT];
  uint8 at_sup_route_prot_parm_pp[HDRMRLP_MAX_PROT_COUNT];

  uint8 sup_higher_layer_prot;
  uint8 an_sup_qos_profiles;
  uint8 max_rlp_flows;
  uint8 max_reservations;

  /* AT Defined */
  uint8 rlp_priority_fwd[HDRMRLPC_N_MRLP_MAX_FWD];
  uint8 rlp_priority_rev[HDRMRLPC_N_MRLP_MAX_REV];

} hdrmrlpc_config_verno_type;

/* The following Attribute parameters are required for 
 * AT Initiated Config Request.
 */
typedef struct
{
#ifdef FEATURE_HDR_SUPPORT_RSP
  #error code not present
#endif
  hdrmrlpc_at_sup_prot_parm_pp_type   at_sup_flow_prot[HDRMRLP_MAX_PROT_COUNT];
  hdrmrlpc_at_sup_prot_parm_pp_type   at_sup_route_prot[HDRMRLP_MAX_PROT_COUNT];
  hdrmrlpc_max_rlp_flows_type    max_rlp_flows;
  hdrmrlpc_max_reservations_type max_reservations;
} hdrmrlpc_at_config_req_parm_type;

typedef enum
{
  HDRMRLPC_GAUP_RSP_TIMER          /* GAUP response timer */
} hdrmrlpc_timer_name_enum_type;


/*-----------------------------------------------------------------------------
n-gaup response timer example:

* no timer

* @Time 0 - Send AttributeUpdateRequest w/ TID 0
                              0           
  +----------------+    +------------+
  | GAUP_RSP_TIMER |    |    TID 0   |  
  +----------------+    +------------+
  |       0        |    | TIME_LEFT  |
  +----------------+    | 0xFFFFFFFF |
                        +------------+
 
* @Time 50ms - Send AttributeUpdateRequest w/ TID 1 
                              0             1
  +----------------+    +------------+------------+
  | GAUP_RSP_TIMER |    |    TID 0   |    TID 1   |
  +----------------+    +------------+------------+
  |       0        |    | TIME_LEFT  | TIME_LEFT  |
  +----------------+    | 0xFFFFFFFF | 0xFFFFFFFF |
                        +------------+------------+ 
 
* @Time 100ms - Send AttributeUpdateRequest w/ TID 2  
                              0             1           2
  +----------------+    +------------+------------+------------+
  | GAUP_RSP_TIMER |    |    TID 0   |    TID 1   |   TID 2    |
  +----------------+    +------------+------------+------------+
  |       0        |    | TIME_LEFT  | TIME_LEFT  | TIME_LEFT  |
  +----------------+    | 0xFFFFFFFF | 0xFFFFFFFF | 0xFFFFFFFF |
                        +------------+------------+------------+
 
* @Time 500ms - Rcv SLP ACK w/ TID 0 
                time_left(tid0) = 2000ms - time_left_on_timer = 0ms
                              0             1           2
  +----------------+    +------------+------------+------------+
  | GAUP_RSP_TIMER |    |    TID 0   |    TID 1   |   TID 2    |
  +----------------+    +------------+------------+------------+
  |     2000ms     |    | TIME_LEFT  | TIME_LEFT  | TIME_LEFT  |
  +----------------+    |     0ms    | 0xFFFFFFFF | 0xFFFFFFFF |
                        +------------+------------+------------+
                         
* @Time 1500ms - Rcv SLP ACK w/ TID 1
                 time_left(tid1) = 2000ms - time_left_on_timer = 1000ms
                              0             1           2
  +----------------+    +------------+------------+------------+
  | GAUP_RSP_TIMER |    |    TID 0   |    TID 1   |   TID 2    |
  +----------------+    +------------+------------+------------+
  |     1000ms     |    | TIME_LEFT  | TIME_LEFT  | TIME_LEFT  |
  +----------------+    |     0ms    |   1000ms   | 0xFFFFFFFF |
                        +------------+------------+------------+
                        
* @Time 2000ms - Rcv SLP ACK w/ TID 2
                 time_left(tid2) = 2000ms - time_left_on_timer = 1500ms

                              0             1           2
  +----------------+    +------------+------------+------------+
  | GAUP_RSP_TIMER |    |    TID 0   |    TID 1   |   TID 2    |
  +----------------+    +------------+------------+------------+
  |    500ms       |    | TIME_LEFT  | TIME_LEFT  | TIME_LEFT  |
  +----------------+    |     0ms    |   1000ms   |   1500ms   |
                        +------------+------------+------------+
                        
* @Time 2250ms - Rcv AttributeUpdateAccept w/TID 0
                  -> do processing associating with rxing rsp for tid 0
                     Remove entry from the timer list.
                  -> restart_time = time_left(tid 1) = 1000ms
                  -> Restart timer for time_left_on_timer + restart_time = 1250ms
                  -> Update time_left field for each entry:
                     time_left(tidX) = time_left(tidX) - restart_time 
                     [TID1 = 0ms, TID2 = 500ms]
                  
                              0             1
  +----------------+    +------------+------------+
  | GAUP_RSP_TIMER |    |    TID 1   |    TID 2   |
  +----------------+    +------------+------------+
  |    1250ms      |    | TIME_LEFT  | TIME_LEFT  |
  +----------------+    |    0ms     |    500ms   |
                        +------------+------------+
                        
* @Time 2300ms - Send AttributeUpdateRequest w/ TID 3 
                              0             1           2
  +----------------+    +------------+------------+------------+
  | GAUP_RSP_TIMER |    |    TID 1   |    TID 2   |   TID 3    |
  +----------------+    +------------+------------+------------+
  |    1200ms      |    | TIME_LEFT  | TIME_LEFT  | TIME_LEFT  |
  +----------------+    |    0ms     |   500ms    | 0xFFFFFFFF |
                        +------------+------------+------------+ 
 
* @Time 2500ms - Rcv SLP ACK w/ TID 3 
                 time_left(tid1) = 2000ms - time_left_on_timer = 1000ms

                              0             1           2
  +----------------+    +------------+------------+------------+
  | GAUP_RSP_TIMER |    |    TID 1   |    TID 2   |   TID 3    |
  +----------------+    +------------+------------+------------+
  |    1000ms      |    | TIME_LEFT  | TIME_LEFT  | TIME_LEFT  |
  +----------------+    |    0ms     |   500ms    |   1000ms   |
                        +------------+------------+------------+
 
* @Time 3000ms - Rcv AttributeUpdateAccept w/TID 2
                  -> do processing associating with rxing rsp for tid 2
                     Remove entry from the timer list.
                     
                              0             1
  +----------------+    +------------+------------+
  | GAUP_RSP_TIMER |    |    TID 1   |    TID 3   |
  +----------------+    +------------+------------+
  |     500ms      |    | TIME_LEFT  | TIME_LEFT  |
  +----------------+    |    0ms     |   1000ms   |
                        +------------+------------+
                        
* @Time 3500ms - gaup_rsp_timer expires
                  -> do processing associated with not getting a rsp for tid 1
                     Remove entry from the time list.
                  -> restart_time = time_left(tid 3) = 1000ms
                  -> Restart timer for time_left_on_timer + restart_time = 1000ms
                  -> update time_left field for each entry (no op)
                              
                              0           
  +----------------+    +------------+
  | GAUP_RSP_TIMER |    |    TID 3   |  
  +----------------+    +------------+
  |    1000ms      |    | TIME_LEFT  |
  +----------------+    |    0ms     |
                        +------------+
                        
-----------------------------------------------------------------------------*/

typedef struct
{
  uint8 trans_id;
    /* Transaction ID of the gaup request we are waiting a response for */
  rex_timer_cnt_type time_left_ms; 
    /* Time to restart the timer by when it expires */
} hdrmrlpc_gaup_timer_record_type;

typedef struct
{
  hdrmrlpc_config_params_type     in_config;  
    /* Used while processing Config msg */

  hdrmrlpc_config_req_result_type config_result;

  hdrmrlpc_config_params_type     current;   
    /* Current configuration */

  hdrmrlpc_config_verno_type      verno;      
    /* Current version number */
                                      
  hdrmrlpc_at_config_req_parm_type desired;
    /* Desired values for AT initiated config parameters */

  rex_crit_sect_type              crit_sect; 
    /* Critical Section */

  /*---------------------------------------------------------------------------
  == AT Initiated GAUP ==
  Keep the transaction ID for the GAUP Request and a running timer while 
  waiting for a gaup response (accept or reject).
  ---------------------------------------------------------------------------*/
  uint8                           gaup_trans_id; 
    /* Transaction ID of the next GAUP request msg to send*/
  
  rex_timer_type                  gaup_rsp_timer; 
    /* The AT waits for a response to its AttributeUpdateRequest for 2s.
       If several attribute update request messages are sent back to back, the
       AT keeps track of one running timer for the oldest message. For any
       subsequent message sent before the AT receives a response, it keeps
       track of the transaction ID of that message + the time to restart the
       timer by when the timer expires. See example above. */

  uint8                           gaup_timer_trans_id;
    /* Transcation ID of the current running timer */

  uint8                           gaup_timer_cnt;
  hdrmrlpc_gaup_timer_record_type gaup_timer_list[HDRMRLPC_MAX_TOTAL_RSV];

  /* Queue for pending gaup parameters  */
  q_type pending_gaup_req_q;

} hdrmrlpc_static_data_type;

static hdrmrlpc_static_data_type hdrmrlpc;

/*=============================================================================

                        FUNCTION DEFINITIONS

=============================================================================*/
static boolean hdrmrlpc_is_index_supported
(
  hdrmrlpc_attrib_name_enum_type attrib_name,
  uint8 rlp_flow_nn
);

void hdrmrlpc_gaup_req_tx_outcome_cb
(
  void                 *data_ptr,
  hdrerrno_enum_type    err_val
);

static hdrerrno_enum_type hdrmrlpc_prop_gaup_qos
(
  uint8 num_profile_blobs,
  hdrmrlpc_gaup_qos_request_type *profile_ptr,
  uint8 *transaction_id_ptr,
  uint8 preallocated_trans_id  
);

/*===========================================================================

FUNCTION HDRMRLPC_GET_CUR_RL_IDX

DESCRIPTION
  This helper function maps a Reservation Label to its current table index
  location in the current configuration.

DEPENDENCIES
  None

PARAMETERS  
  res_label - The Reservation Label we wish to look up
  dir - RLP direction (forward or reverse
    
RETURN VALUE
  E_SUCCESS - Found the Reservation Label in the current config table.
              The index location is returned in res_idx
  E_FAILURE - Couldn't find this Reservation Label in the current config
  
SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_cur_rl_idx
(
  uint8 res_label,
  hdrmrlpc_ver_enum_type dir,
  uint8 *res_idx
)
{

  uint8 index; /* iterator */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (dir == HDRMRLPC_FWD_RLP)
  {
    ASSERT(hdrmrlpc.current.num_fwd_rsv <= HDRMRLPC_N_RES_LABELS_MAX_FWD);

    for (index = 0; index < hdrmrlpc.current.num_fwd_rsv; index++)
    {  
      if (hdrmrlpc.current.fwd_res_info[index].rsv_label == res_label )
      {
        *res_idx = index;
        return E_SUCCESS;
      }
    }
  }
  else /* if (dir == HDRMRLPC_REV_RLP) */
  {
    ASSERT(hdrmrlpc.current.num_rev_rsv <= HDRMRLPC_N_RES_LABELS_MAX_REV);

    for (index = 0; index < hdrmrlpc.current.num_rev_rsv; index++)
    {  
      if (hdrmrlpc.current.rev_res_info[index].rsv_label == res_label )
      {
        *res_idx = index;
        return E_SUCCESS;
      }
    }
  }
  
  return E_FAILURE;

} /* hdrmrlpc_get_cur_rl_idx() */
/*===========================================================================

FUNCTION HDRMRLPC_MAP_CUR_RL

DESCRIPTION
  This helper function maps a Reservation Label to its current table index
  location in the current configuration. If the label isn't found, a new
  entry is created.

DEPENDENCIES
  None

PARAMETERS  
  res_label - The Reservation Label we wish to look up
  dir - RLP direction (forward or reverse
    
RETURN VALUE
  E_SUCCESS - Found the Reservation Label in the current config table.
              The index location is returned in res_idx
  E_FAILURE - Couldn't find this Reservation Label in the current config
  
SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_map_cur_rl
(
  uint8 res_label,
  hdrmrlpc_ver_enum_type dir,
  uint8 *res_idx
)
{

  /* Map a reservation label to an index entry in the current config table.
     If an entry doesn't already exist, add one at the end. */
  if (hdrmrlpc_get_cur_rl_idx(res_label,dir, res_idx) == E_FAILURE)
  {
    if (dir == HDRMRLPC_FWD_RLP)
    {
      ASSERT(hdrmrlpc.current.num_fwd_rsv < HDRMRLPC_N_RES_LABELS_MAX_FWD);

      *res_idx = hdrmrlpc.current.num_fwd_rsv;

      hdrmrlpc.current.fwd_res_info[*res_idx].rsv_label = res_label;
      hdrmrlpc.current.num_fwd_rsv++;
    }
    else /* dir == HDRMRLPC_REV_RLP */
    {

      ASSERT(hdrmrlpc.current.num_rev_rsv < HDRMRLPC_N_RES_LABELS_MAX_REV);

      *res_idx = hdrmrlpc.current.num_rev_rsv;

      hdrmrlpc.current.rev_res_info[*res_idx].rsv_label = res_label;
      hdrmrlpc.current.num_rev_rsv++;
    }
  }

  return E_SUCCESS;

} /* hdrmrlpc_map_cur_rl() */


/*===========================================================================

FUNCTION HDRMRLPC_GET_CFG_RL_IDX

DESCRIPTION
  This helper function maps a Reservation Label to a table index location
  for the in configuration instance. 

DEPENDENCIES
  None

PARAMETERS  
  res_label - The Reservation Label we wish to look up
  dir - RLP direction (forward or reverse
    
RETURN VALUE
  E_SUCCESS - Found the Reservation Label in the current config table.
              The index location is returned in res_idx
  E_FAILURE - Couldn't find this Reservation Label in the current config
  
SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_cfg_rl_idx
(
  uint8 res_label,
  hdrmrlpc_ver_enum_type dir,
  uint8 *res_idx
)
{

  uint8 index; /* iterator */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (dir == HDRMRLPC_FWD_RLP)
  {
    ASSERT(hdrmrlpc.in_config.num_fwd_rsv <= HDRMRLPC_N_RES_LABELS_MAX_FWD);

    for (index = 0; index < hdrmrlpc.in_config.num_fwd_rsv; index++)
    {  
      if (hdrmrlpc.in_config.fwd_res_info[index].rsv_label == res_label )
      {
        *res_idx = index;
        return E_SUCCESS;
      }
    }
  }
  else /* if (dir == HDRMRLPC_REV_RLP) */
  {
    ASSERT(hdrmrlpc.in_config.num_rev_rsv <= HDRMRLPC_N_RES_LABELS_MAX_REV);

    for (index = 0; index < hdrmrlpc.in_config.num_rev_rsv; index++)
    {  
      if (hdrmrlpc.in_config.rev_res_info[index].rsv_label == res_label )
      {
        *res_idx = index;
        return E_SUCCESS;
      }
    }
  }
  
  return E_FAILURE;

} /* hdrmrlpc_get_cfg_rl_idx() */


/*===========================================================================

FUNCTION HDRMRLPC_MAP_CFG_RL

DESCRIPTION
  This helper function maps a Reservation Label to a table index location
  for the in configuration instance.  If it isn't in the config table yet,
  add it at the end of the table.

DEPENDENCIES
  None

PARAMETERS  
  res_label - The Reservation Label we wish to look up. 
              If it isn't in the config table yet, add it at the end of the 
              table.

  dir - RLP direction (forward or reverse)
    
RETURN VALUE
  E_SUCCESS - Found or added the Reservation Label in the in config table.
              The index location is returned in res_idx.
              Note that both the in_config and config_result structures are
              treated as parallel arrays and will thus share the same indices.
  
  E_FAILURE - Reached max allowed ReservationKK parameters 
  
SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_map_cfg_rl
(
  uint8 res_label,
  hdrmrlpc_ver_enum_type dir,
  uint8 *res_idx
)
{
  uint8 index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ENH TODO - reclaim table space for ResKK parameters that are negotiated
     with default values */
  if (dir == HDRMRLPC_FWD_RLP)
  {
    ASSERT(hdrmrlpc.in_config.num_fwd_rsv <= HDRMRLPC_N_RES_LABELS_MAX_FWD);

    for (index=0; index < hdrmrlpc.in_config.num_fwd_rsv; index++)
    {
      if (hdrmrlpc.in_config.fwd_res_info[index].rsv_label == res_label)
      {
        *res_idx = index;
        return E_SUCCESS;
      }
    }

    /* We haven't found an entry for this Reservation Label in the current
       table, add a new one at the end of the table */
    if (hdrmrlpc.in_config.num_fwd_rsv < HDRMRLPC_N_RES_LABELS_MAX_FWD)
    {
      hdrmrlpc.in_config.fwd_res_info[index].rsv_label = res_label;
      hdrmrlpc.in_config.num_fwd_rsv++;
      *res_idx = index;
      return E_SUCCESS;
    }
  }
  else /* if (dir == HDRMRLPC_REV_RLP) */
  {
    ASSERT(hdrmrlpc.in_config.num_rev_rsv <= HDRMRLPC_N_RES_LABELS_MAX_REV);

    for (index=0; index < hdrmrlpc.in_config.num_rev_rsv; index++)
    {
      if (hdrmrlpc.in_config.rev_res_info[index].rsv_label == res_label)
      {
        *res_idx = index;
        return E_SUCCESS;
      }
    }

    /* We haven't found an entry for this Reservation Label in the current
       table, add a new one at the end of the table */
    if (hdrmrlpc.in_config.num_rev_rsv < HDRMRLPC_N_RES_LABELS_MAX_REV)
    {
      hdrmrlpc.in_config.rev_res_info[index].rsv_label = res_label;
      hdrmrlpc.in_config.num_rev_rsv++;
      *res_idx = index;
      return E_SUCCESS;
    }
  }

  return E_FAILURE;

} /* hdrmrlpc_map_cfg_rl() */

/*===========================================================================

FUNCTION HDRMRLPC_GET_ACTIVE_FWD_RLPS

DESCRIPTION
  This function returns the maximum number of supported forward link 
  multi-flow RLPs and their associated RLP Identifiers (RLP Flow)
  
  Returns the number of active flows. Use Another function to get total number of flows.

DEPENDENCIES
  The module calling this function needs to make sure that memory is allocated
  for listing upto the maximum supported RLPs.
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the RLP IDs of all the forward link 
              multi-flow RLPs
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e715 unreferenced variable app_subtype*/
hdrerrno_enum_type hdrmrlpc_get_active_fwd_rlps
(
  hdrhai_app_subtype_enum_type   app_subtype,
  uint8 *num_active_fwd_rlps,
  uint8 *rlp_flow
)
{
  int i;
  uint8 active_cnt = 0; /* Number of active RLPs */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (rlp_flow == NULL) return E_FAILURE;

  /* _mpa_ pass in max active cnt for bounds checking */
  for(i=0; i<HDRMRLPC_N_MRLP_MAX_FWD && 
       (active_cnt < hdrmrlpc.current.max_rlp_flows.max_activated_rlp_flows_fwd); i++)
  {
    if (((hdrstream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) || 
          hdrstream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)) &&
         hdrmrlpc.current.fwd_rlp_config_info[i].activated) ||
        (hdrstream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN) &&
         hdrmrlpc.current.fwd_rlp_config_info[i].flow_id.is_active))
    {
      rlp_flow[active_cnt] = hdrmrlpc.current.fwd_rlp_config_info[i].rlp_flow;
      ++active_cnt;
    }
  }

  *num_active_fwd_rlps = active_cnt;

  return E_SUCCESS;

} /* hdrmrlpc_get_active_fwd_rlps() */
/*lint -restore*/
/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_ACTIVE_REV_RLPS

DESCRIPTION
  This function returns the maximum number of supported reverse link 
  multi-flow RLPs and their associated RLP Identifiers (RLP Flow)

DEPENDENCIES
  The module calling this function needs to make sure that memory is allocated
  for listing upto the maximum supported RLPs.
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the RLP IDs of all the forward link 
              multi-flow RLPs
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e715 unreferenced variable app_subtype*/
hdrerrno_enum_type hdrmrlpc_get_active_rev_rlps
(
  hdrhai_app_subtype_enum_type   app_subtype,
  uint8 *num_active_rev_rlps,
  uint8 *rlp_flow
)
{
  int i;
  uint8 active_cnt = 0; /* Number of active RLPs */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (rlp_flow == NULL) return E_FAILURE;

  /* _mpa_ pass in max rlp active cnt for bounds checking */
  for(i=0;i<HDRMRLPC_N_MRLP_MAX_REV && 
       (active_cnt < hdrmrlpc.current.max_rlp_flows.max_activated_rlp_flows_rev);i++)
  {
    if (((hdrstream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
          hdrstream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)) &&
         hdrmrlpc.current.rev_rlp_config_info[i].activated) ||
        (hdrstream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN) &&
         hdrmrlpc.current.rev_rlp_config_info[i].flow_id.is_active))
    {
      rlp_flow[active_cnt] = hdrmrlpc.current.rev_rlp_config_info[i].rlp_flow;
      ++active_cnt;
    }
  }

  *num_active_rev_rlps = active_cnt;

  return E_SUCCESS;

} /* hdrmrlpc_get_active_rev_rlps() */
/*lint -restore*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_SIMPLE_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a simple attribute.
  
  0xFFFE - RLPCountMaxFwd (No GAUP allowed)                  
  0xFFFD - RLPCountMaxRev (No GAUP allowed)
  0xFFFC - MaxAbortTimer  (No GAUP allowed)
  
  0xFENN - FlowNNNakEnableFwd (GAUP)
  0xFDNN - FlowNNNakEnableRev (GAUP)         
  0xFCNN - FlowNNHigherLayerProtocolFwd (GAUP)
  0xFBNN - FlowNNHigherLayerProtocolRev (GAUP)
  0xFANN - FlowNNPhysicalLayerNakEnableRev (GAUP)
  0xF7NN - FlowNNTransmitAbortTimerRev (GAUP)
  0xF6NN - FlowNNDataOverSignalingAllowedRev (GAUP)
  0xF9KK - ReservationKKIdleStateFwd (GAUP)
  0xF8KK - ReservationKKIdleStateRev (GAUP)
  

DEPENDENCIES
  None.
  
PARAMETERS
  attrib_name - The Attribute's Name (per type)
  attrib_idx  - FlowNN or ResKK (if applicable, 
                HDRMLPC_ATTRIB_INVALID passed in otherwise)
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
           
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_simple_attrib
(
  hdrmrlpc_attrib_name_enum_type attrib_name,
  uint8 attrib_idx,
  uint32 * value,
  uint8 * verno
)
{

  uint8 reskk_idx = 0; /* For ReservationKK Attributes only */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !hdrmrlpc_is_index_supported(attrib_name, attrib_idx) )
  {
    ERR("Invalid attrib (name:%d, attrib_idx:%d)",attrib_name, attrib_idx, 0);
    return E_FAILURE;
  }

  switch (attrib_name)
  {
    case HDRMRLPC_ATTRIB_MAX_ABORT_TIMER:
      *value = hdrmrlpc.current.max_abort_timer;
      *verno = hdrmrlpc.verno.max_abort_timer;
    break;

    case HDRMRLPC_ATTRIB_PPP_FREE_AUTHENTICATION_SUPPORT:
      *value = hdrmrlpc.current.ppp_free_authentication_support;
      *verno = hdrmrlpc.verno.ppp_free_authentication_support;
      break;

    case HDRMRLPC_ATTRIB_TWO_ROUTES_SUPPORTED:
      *value = hdrmrlpc.current.two_routes_supported;
      *verno = hdrmrlpc.verno.two_routes_supported;
      break;

    case HDRMRLPC_ATTRIB_AT_NAK_DELAY_SUPPORTED:
      *value = hdrmrlpc.current.at_nak_delay_supported;
      *verno = hdrmrlpc.verno.at_nak_delay_supported;
    break;

    case HDRMRLPC_ATTRIB_NAK_EN_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].nak_enabled;
      *verno = hdrmrlpc.verno.nak_enable_fwd[attrib_idx];
    break;

    case HDRMRLPC_ATTRIB_NAK_EN_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].nak_enabled;
      *verno = hdrmrlpc.verno.nak_enable_rev[attrib_idx];
    break;

    case HDRMRLPC_ATTRIB_HIGH_LAYER_PROT_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].higher_layer_protocol;
      *verno = hdrmrlpc.verno.higher_layer_prot_fwd[attrib_idx];
    break;

    case HDRMRLPC_ATTRIB_HIGH_LAYER_PROT_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].higher_layer_protocol;
      *verno = hdrmrlpc.verno.higher_layer_prot_rev[attrib_idx];
    break;

    case HDRMRLPC_ATTRIB_PHYS_LAYER_NAK_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].phys_link_nak_enabled_rev;
      *verno = hdrmrlpc.verno.phys_link_nak_en_rev[attrib_idx];
    break;

    case HDRMRLPC_ATTRIB_TRANSMIT_ABORT_TIMER_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].xmit_abort_timer;
      *verno = hdrmrlpc.verno.xmit_abort_timer_rev[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_DATA_OVER_SIG_ALLOWED_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].dos_allowed;
      *verno = hdrmrlpc.verno.dos_allowed_rev[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_ACTIVATED_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].activated;
      *verno = hdrmrlpc.verno.activated_fwd[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_ACTIVATED_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].activated;
      *verno = hdrmrlpc.verno.activated_rev[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_SIMPLE_SEQUENCE_LENGTH_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].simple_sequence_length;
      *verno = hdrmrlpc.verno.sequence_length_fwd[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_SEQUENCE_LENGTH_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].simple_sequence_length;
      *verno = hdrmrlpc.verno.sequence_length_rev[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_FLOW_PROT_SDU_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].flow_prot_sdu;
      *verno = hdrmrlpc.verno.flow_prot_sdu_fwd[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_FLOW_PROT_SDU_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].flow_prot_sdu;
      *verno = hdrmrlpc.verno.flow_prot_sdu_rev[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_DATA_UNIT_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].data_unit;
      *verno = hdrmrlpc.verno.data_unit_fwd[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_DATA_UNIT_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].data_unit;
      *verno = hdrmrlpc.verno.data_unit_rev[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_ROUTE_PROT_SDU_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].route_prot_sdu;
      *verno = hdrmrlpc.verno.route_prot_sdu_fwd[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_ROUTE_PROT_SDU_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].route_prot_sdu;
      *verno = hdrmrlpc.verno.route_prot_sdu_rev[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_SIM_DEL_BOTH_ROUTES_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].sim_del_both_routes;
      *verno = hdrmrlpc.verno.sim_del_both_routes_fwd[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_OOO_DEL_ROUTE_PROT_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].ooo_del_route_prot;
      *verno = hdrmrlpc.verno.ooo_del_route_prot_fwd[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_FLOW_NAK_DELAY_TIME_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].nak_delay_time;
      *verno = hdrmrlpc.verno.timers_fwd[attrib_idx];
      break;

    case HDRMRLPC_ATTRIB_RESKK_IDLE_STATE_FWD:
      if ( hdrmrlpc_get_cur_rl_idx(attrib_idx, HDRMRLPC_FWD_RLP, &reskk_idx) == E_SUCCESS )
      {
        *value = hdrmrlpc.current.fwd_res_info[reskk_idx].idle_state;
        *verno = hdrmrlpc.verno.reskk_idle_state_fwd[reskk_idx];
      }
      else
      {
        ERR("Invalid ResKK attrib (name:%d, attrib_idx:%d)",attrib_name, attrib_idx, 0);
        return E_FAILURE;
      }
    break;

    case HDRMRLPC_ATTRIB_RESKK_IDLE_STATE_REV:
      if ( hdrmrlpc_get_cur_rl_idx(attrib_idx, HDRMRLPC_REV_RLP, &reskk_idx) == E_SUCCESS )
      {
        *value = hdrmrlpc.current.rev_res_info[reskk_idx].idle_state;
        *verno = hdrmrlpc.verno.reskk_idle_state_rev[reskk_idx];
      }
      else
      {
        ERR("Invalid ResKK attrib (name:%d, attrib_idx:%d)",attrib_name, attrib_idx, 0);
        return E_FAILURE;
      }
    break;

    case HDRMRLPC_ATTRIB_RLP_PRIORITY_FWD:
      *value = hdrmrlpc.current.fwd_rlp_config_info[attrib_idx].rlp_priority;
      *verno = hdrmrlpc.verno.rlp_priority_fwd[attrib_idx];
    break;

    case HDRMRLPC_ATTRIB_RLP_PRIORITY_REV:
      *value = hdrmrlpc.current.rev_rlp_config_info[attrib_idx].rlp_priority;
      *verno = hdrmrlpc.verno.rlp_priority_rev[attrib_idx];
    break;

    default:
      ERR("Unknown attribute (%d)",attrib_name,0,0);
      return E_FAILURE;
      
    }

  return E_SUCCESS;
} /* hdrmrlpc_get_simple_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_IDENTIFICATION_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNIdentification attribute type. (0x01NN, 0x02NN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute) 
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_identification_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_flow_identification_type * value,
  uint8 * verno
)
{

  if ((direction == HDRMRLPC_FWD_RLP) &&
      hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_IDENTIFICATION_FWD, 
                                     rlp_flow_nn)
     )
  {

#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.fwd_rlp_config_info[rlp_flow_nn].flow_id;
#else
    memcpy(value,
           &hdrmrlpc.current.fwd_rlp_config_info[rlp_flow_nn].flow_id,
           sizeof(hdrmrlpc_flow_identification_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.identification_fwd[rlp_flow_nn];
  }
  else if ((direction == HDRMRLPC_REV_RLP) &&
           hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_IDENTIFICATION_REV, 
                                          rlp_flow_nn)
          )
  {

#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.rev_rlp_config_info[rlp_flow_nn].flow_id; 
#else
    memcpy(value,
           &hdrmrlpc.current.rev_rlp_config_info[rlp_flow_nn].flow_id,
           sizeof(hdrmrlpc_flow_identification_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.identification_rev[rlp_flow_nn];
  }
  else
  {
    ERR("Invalid Identification attrib (dir: %d, rlp_flow: %d)",
        direction,rlp_flow_nn,0);
    return E_FAILURE;
  }

  return E_SUCCESS;
} /* hdrmrlpc_get_identification_attrib() */
  
/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_SEQLEN_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNSequenceLength attribute type.

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_seqlen_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_flow_sequence_length_type * value,
  uint8 * verno
)
{
  hdrhai_app_subtype_enum_type app_subtype = 
      hdrstream_get_negotiated_snpktapp();
    /* Negotiated App subtype */
/*--------------------------------------------------------------------------*/

  if (direction == HDRMRLPC_FWD_RLP)
  {
    if ((app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) &&
         hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_SIMPLE_SEQUENCE_LENGTH_FWD,
                                     rlp_flow_nn))
    {
      value->sar_seq_len = 
          hdrmrlpc.current.fwd_rlp_config_info[rlp_flow_nn].simple_sequence_length;
      value->sar_seq_len_short = 0;

      *verno = hdrmrlpc.verno.sequence_length_fwd[rlp_flow_nn];
    }
    else if ((app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) &&
              hdrmrlpc_is_index_supported(
                  HDRMRLPC_ATTRIB_COMPLEX_SEQUENCE_LENGTH_FWD,
                  rlp_flow_nn))
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      *value = hdrmrlpc.current.fwd_rlp_config_info[rlp_flow_nn].complex_seq_len;
#else
      memcpy(value,
             &hdrmrlpc.current.fwd_rlp_config_info[rlp_flow_nn].complex_seq_len,
             sizeof(hdrmrlpc_flow_sequence_length_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      *verno = hdrmrlpc.verno.sequence_length_fwd[rlp_flow_nn];
    }
  }
  else if ( (direction == HDRMRLPC_REV_RLP) &&
             hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_SEQUENCE_LENGTH_REV,
                                         rlp_flow_nn)
          )
  {
    value->sar_seq_len = 
        hdrmrlpc.current.rev_rlp_config_info[rlp_flow_nn].simple_sequence_length;
    value->sar_seq_len_short = 0;

    *verno = hdrmrlpc.verno.sequence_length_rev[rlp_flow_nn];;
  }
  else
  {
    ERR("Invalid SequenceLength attrib (dir: %d, rlp_flow: %d)", 
        direction, rlp_flow_nn, 0);
    return E_FAILURE;
  }

  return E_SUCCESS;

} /* hdrmrlpc_get_seqlen_attrib() */
  
/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_TIMERS_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNTimers attribute type. (0x03NN, 0x04NN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_timers_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_flow_timers_type * value,
  uint8 * verno
)
{
  hdrhai_app_subtype_enum_type app_subtype = hdrstream_get_negotiated_snpktapp();
/*--------------------------------------------------------------------------*/

  if ((direction == HDRMRLPC_FWD_RLP) &&
      hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_TIMERS_FWD,rlp_flow_nn))
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.fwd_rlp_config_info[rlp_flow_nn].flow_timer;
#else
    memcpy(value,
           &hdrmrlpc.current.fwd_rlp_config_info[rlp_flow_nn].flow_timer,
           sizeof(hdrmrlpc_flow_timers_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.timers_fwd[rlp_flow_nn];

    if (((app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)||
         (app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)) &&
         hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_FLOW_NAK_DELAY_TIME_FWD,
                                     rlp_flow_nn))
    {
      value->nak_delay_timer_val = 
        hdrmrlpc.current.fwd_rlp_config_info[rlp_flow_nn].flow_timer.nak_delay_timer_val;
      *verno = hdrmrlpc.verno.timers_fwd[rlp_flow_nn];
    }
  }
  else if ( (direction == HDRMRLPC_REV_RLP) &&
            hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_TIMERS_REV,
                                           rlp_flow_nn)
          )
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.rev_rlp_config_info[rlp_flow_nn].flow_timer;
#else
    memcpy(value,
           &hdrmrlpc.current.rev_rlp_config_info[rlp_flow_nn].flow_timer,
           sizeof(hdrmrlpc_flow_timers_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.timers_rev[rlp_flow_nn];
  }
  else
  {
    ERR("Invalid Timers attrib (dir: %d, rlp_flow: %d)",
        direction,rlp_flow_nn,0);
    return E_FAILURE;
  }

  return E_SUCCESS;

} /* hdrmrlpc_get_timers_attrib() */
  
/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_RESERVATION_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNReservations attribute type. (0x05NN, 0x06NN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                 
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_reservation_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_flow_reservation_type * value,
  uint8 * verno
)
{

  if ( (direction == HDRMRLPC_FWD_RLP) &&
       hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_RESERV_FWD,rlp_flow_nn)
     )
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.fwd_rlp_config_info[rlp_flow_nn].flow_rsv;
#else
    memcpy(value,
           &hdrmrlpc.current.fwd_rlp_config_info[rlp_flow_nn].flow_rsv,
           sizeof(hdrmrlpc_flow_reservation_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.reservation_fwd[rlp_flow_nn];
  }
  else if ( (direction == HDRMRLPC_REV_RLP) &&
            hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_RESERV_REV,
                                           rlp_flow_nn)
          )
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.rev_rlp_config_info[rlp_flow_nn].flow_rsv;
#else
    memcpy(value,
           &hdrmrlpc.current.rev_rlp_config_info[rlp_flow_nn].flow_rsv,
           sizeof(hdrmrlpc_flow_reservation_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.reservation_rev[rlp_flow_nn];
  }
  else
  {
    ERR("Invalid Reservation attrib (dir: %d, rlp_flow: %d)",
        direction,rlp_flow_nn,0);
    return E_FAILURE;
  }

  return E_SUCCESS;
} /* hdrmrlpc_get_reservation_attrib() */
  
/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_RESKK_QOS_REQUEST_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a ResKKQoSRequest attribute type. (0x07NN, 0x08NN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  res_kk  - ReservationKK of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_reskk_qos_request_attrib
(
  uint8 res_kk,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_res_qos_profile_config_type * value,
  uint8 * verno
)
{

  uint8 res_idx = 0; /* Index of reservation label location */
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrmrlpc_get_cur_rl_idx(res_kk, direction, &res_idx) == E_SUCCESS )
  {
    if ( direction == HDRMRLPC_FWD_RLP ) 
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      *value = hdrmrlpc.current.fwd_res_info[res_idx].qos_request;
#else
      memcpy(value,
             &hdrmrlpc.current.fwd_res_info[res_idx].qos_request,
             sizeof(hdrmrlpc_res_qos_profile_config_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      *verno = hdrmrlpc.verno.reskk_qos_req_fwd[res_idx];
    }
    else /* if ( direction == HDRMRLPC_REV_RLP ) */
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      *value = hdrmrlpc.current.rev_res_info[res_idx].qos_request;
#else
      memcpy(value,
             &hdrmrlpc.current.rev_res_info[res_idx].qos_request,
             sizeof(hdrmrlpc_res_qos_profile_config_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      *verno = hdrmrlpc.verno.reskk_qos_req_rev[res_idx];
    }

    return E_SUCCESS;
  }
  else
  {
    HDR_MSG_PROT_2(MSG_LEGACY_LOW, 
                   "Invalid QoS Request attrib (dir: %d, res_kk: %d)",
                   direction,res_kk);

    return E_FAILURE;
  }

} /* hdrmrlpc_get_reskk_qos_request_attrib() */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_RESKK_QOS_RESPONSE_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a ResKKQoSResponse attribute type. (0x09NN, 0x0ANN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  res_kk  - ReservationKK of the attribute to retrieve
  direction - RLP direction (forward or reverse attribute)                   
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_reskk_qos_response_attrib
(
  uint8 res_kk,
  hdrmrlpc_ver_enum_type direction,
  hdrmrlpc_res_qos_profile_config_type * value,
  uint8 * verno
)
{

  uint8 res_idx = 0; /* Index of reservation label location */
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrmrlpc_get_cur_rl_idx(res_kk, direction, &res_idx) == E_SUCCESS )
  {
    if ( direction == HDRMRLPC_FWD_RLP ) 
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      *value = hdrmrlpc.current.fwd_res_info[res_idx].qos_response;
#else
      memcpy(value,
             &hdrmrlpc.current.fwd_res_info[res_idx].qos_response,
             sizeof(hdrmrlpc_res_qos_profile_config_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      *verno = hdrmrlpc.verno.reskk_qos_rsp_fwd[res_idx];
    }
    else /* if ( direction == HDRMRLPC_REV_RLP ) */
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      *value = hdrmrlpc.current.rev_res_info[res_idx].qos_response;
#else
      memcpy(value,
             &hdrmrlpc.current.rev_res_info[res_idx].qos_response,
             sizeof(hdrmrlpc_res_qos_profile_config_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      *verno = hdrmrlpc.verno.reskk_qos_rsp_rev[res_idx];
    }

    return E_SUCCESS;
  }
  else
  {
    HDR_MSG_PROT_2(MSG_LEGACY_LOW, 
                   "Invalid QoS Response attrib (dir: %d, res_kk: %d)",
                   direction,res_kk);
    return E_FAILURE;
  }

} /* hdrmrlpc_get_reskk_qos_response_attrib() */
  

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_FLOW_PROT_PARM_FWD_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNFlowProtocolParametersFwd attribute type. (0x0BNN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_flow_prot_parm_fwd_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_flow_prot_parm_fwd_type * value,
  uint8 * verno
)
{

  if (hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_FLOW_PROT_PARM_FWD,rlp_flow_nn))
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.fwd_flow_prot[rlp_flow_nn];
#else
    memcpy(value,
           &hdrmrlpc.current.fwd_flow_prot[rlp_flow_nn],
           sizeof(hdrmrlpc_flow_prot_parm_fwd_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.flow_prot_parm_fwd[rlp_flow_nn];
  }
  else
  {
    ERR("Invalid FlowProtParmFwd attrib (rlp_flow: %d)", rlp_flow_nn,0,0);
    return E_FAILURE;
  }

  return E_SUCCESS;
} /* hdrmrlpc_get_flow_prot_parm_fwd_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_FLOW_PROT_PARM_REV_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNFlowProtocolParametersFwd attribute type. (0x0CNN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_flow_prot_parm_rev_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_flow_prot_parm_rev_type * value,
  uint8 * verno
)
{

  if (hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_FLOW_PROT_PARM_REV,rlp_flow_nn))
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.rev_flow_prot[rlp_flow_nn];
#else
    memcpy(value,
           &hdrmrlpc.current.rev_flow_prot[rlp_flow_nn],
           sizeof(hdrmrlpc_flow_prot_parm_rev_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.flow_prot_parm_rev[rlp_flow_nn];
  }
  else
  {
    ERR("Invalid FlowProtParmRev attrib (rlp_flow: %d)", rlp_flow_nn,0,0);
    return E_FAILURE;
  }

  return E_SUCCESS;
} /* hdrmrlpc_get_flow_prot_parm_rev_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_ROUTE_PROT_PARM_FWD_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNFlowProtocolParametersFwd attribute type. (0x0DNN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_route_prot_parm_fwd_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_route_prot_parm_fwd_type * value,
  uint8 * verno
)
{

  if (hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_ROUTE_PROT_PARM_FWD,rlp_flow_nn))
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.fwd_route_prot[rlp_flow_nn];
#else
    memcpy(value,
           &hdrmrlpc.current.fwd_route_prot[rlp_flow_nn],
           sizeof(hdrmrlpc_route_prot_parm_fwd_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.route_prot_parm_fwd[rlp_flow_nn];
  }
  else
  {
    ERR("Invalid RouteProtParmFwd attrib (rlp_flow: %d)", rlp_flow_nn,0,0);
    return E_FAILURE;
  }

  return E_SUCCESS;
} /* hdrmrlpc_get_route_prot_parm_fwd_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_ROUTE_PROT_PARM_REV_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a FlowNNRouteProtocolParametersRev attribute type. (0x0ENN) (GAUP)

DEPENDENCIES
  None.
  
PARAMETERS
  rlp_flow_nn  - FlowNN of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_route_prot_parm_rev_attrib
(
  uint8 rlp_flow_nn,
  hdrmrlpc_route_prot_parm_rev_type * value,
  uint8 * verno
)
{

  if (hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_ROUTE_PROT_PARM_REV,rlp_flow_nn))
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.rev_route_prot[rlp_flow_nn];
#else
    memcpy(value,
           &hdrmrlpc.current.rev_route_prot[rlp_flow_nn],
           sizeof(hdrmrlpc_route_prot_parm_rev_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.route_prot_parm_rev[rlp_flow_nn];
  }
  else
  {
    ERR("Invalid RouteProtParmRev attrib (rlp_flow: %d)", rlp_flow_nn,0,0);
    return E_FAILURE;
  }

  return E_SUCCESS;
} /* hdrmrlpc_get_route_prot_parm_rev_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_AT_SUP_FLOW_PROT_PARM_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a ATSupportedFlowProtocolParameters attribute type. (0x0FPP)

DEPENDENCIES
  None.
  
PARAMETERS
  prot_parm_pp  - Protocol Parm PP of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_at_sup_flow_prot_parm_attrib
(
  uint8 prot_parm_pp,
  hdrmrlpc_at_sup_prot_parm_pp_type * value,
  uint8 * verno
)
{

  if (hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_AT_SUP_FLOW_PROT_PARM,
                                  prot_parm_pp))
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.at_sup_flow_prot[prot_parm_pp];
#else
    memcpy(value,
           &hdrmrlpc.current.at_sup_flow_prot[prot_parm_pp],
           sizeof(hdrmrlpc_at_sup_prot_parm_pp_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.at_sup_flow_prot_parm_pp[prot_parm_pp];
  }
  else
  {
    ERR("Invalid AtSupportedFlowProtParm attrib (pp: %d)", prot_parm_pp,0,0);
    return E_FAILURE;
  }

  return E_SUCCESS;
} /* hdrmrlpc_get_at_sup_flow_prot_parm_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_AT_SUP_ROUTE_PROT_PARM_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  a ATSupportedFlowProtocolParameters attribute type. (0x0FPP)

DEPENDENCIES
  None.
  
PARAMETERS
  prot_parm_pp  - Protocol Parm PP of the attribute to retrieve
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_at_sup_route_prot_parm_attrib
(
  uint8 prot_parm_pp,
  hdrmrlpc_at_sup_prot_parm_pp_type * value,
  uint8 * verno
)
{

  if (hdrmrlpc_is_index_supported(HDRMRLPC_ATTRIB_AT_SUP_ROUTE_PROT_PARM,
                                  prot_parm_pp))
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    *value = hdrmrlpc.current.at_sup_route_prot[prot_parm_pp];
#else
    memcpy(value,
           &hdrmrlpc.current.at_sup_route_prot[prot_parm_pp],
           sizeof(hdrmrlpc_at_sup_prot_parm_pp_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    *verno = hdrmrlpc.verno.at_sup_route_prot_parm_pp[prot_parm_pp];
  }
  else
  {
    ERR("Invalid AtSupportedRouteProtParm attrib (pp: %d)", prot_parm_pp,0,0);
    return E_FAILURE;
  }

  return E_SUCCESS;
} /* hdrmrlpc_get_at_sup_route_prot_parm_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_SHLP_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  the SupportedHigherLayerProtocols attribute (0x0000, No GAUP allowed)

DEPENDENCIES
  None.
  
PARAMETERS
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_shlp_attrib
(
  hdrmrlpc_sup_higher_layer_prot_type * value,
  uint8 * verno
)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  *value = hdrmrlpc.current.sup_higher_layer_prot;
#else
  memcpy( value,
          &hdrmrlpc.current.sup_higher_layer_prot,
          sizeof(hdrmrlpc_sup_higher_layer_prot_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  *verno = hdrmrlpc.verno.sup_higher_layer_prot;

  return E_SUCCESS;

} /* hdrmrlpc_get_shlp_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_MAX_RLP_FLOWS_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  the MaxRLPFlows attribute (0x0004, No GAUP allowed)

DEPENDENCIES
  None.
  
PARAMETERS
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_max_rlp_flows_attrib
(
  hdrmrlpc_max_rlp_flows_type * value,
  uint8 * verno
)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  *value = hdrmrlpc.current.max_rlp_flows;
#else
  memcpy( value,
          &hdrmrlpc.current.max_rlp_flows,
          sizeof(hdrmrlpc_max_rlp_flows_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  *verno = hdrmrlpc.verno.max_rlp_flows;

  return E_SUCCESS;

} /* hdrmrlpc_get_max_rlp_flows_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_MAX_RESERVATIONS_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  the MaxReservations attribute (0x0005, GAUP allowed)

DEPENDENCIES
  None.
  
PARAMETERS
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_max_reservations_attrib
(
  hdrmrlpc_max_reservations_type * value,
  uint8 * verno
)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  *value = hdrmrlpc.current.max_reservations;
#else
  memcpy( value,
          &hdrmrlpc.current.max_reservations,
          sizeof(hdrmrlpc_max_reservations_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  *verno = hdrmrlpc.verno.max_reservations;

  return E_SUCCESS;

} /* hdrmrlpc_get_max_reservations_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_GET_AN_SUP_QOS_PROF_ATTRIB

DESCRIPTION
  This function retrieves the current configuration and version number for 
  the ANSupportedQoSProfiles attribute (0x0003, GAUP allowed)

DEPENDENCIES
  None.
  
PARAMETERS
  value - returns the requested attribute's value
  verno - returns the requested attribute's version number                       
  
RETURN VALUE
  E_SUCCESS - If successfully retrieved the configuration attribute  
  E_FAILURE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_get_an_sup_qos_prof_attrib
(
  hdrmrlpc_supported_qos_profiles * value,
  uint8 * verno
)
{
#ifdef FEATURE_MEMCPY_REMOVAL
  *value = hdrmrlpc.current.an_sup_qos_profiles;
#else
  memcpy( value,
          &hdrmrlpc.current.an_sup_qos_profiles,
          sizeof(hdrmrlpc_supported_qos_profiles) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  *verno = hdrmrlpc.verno.an_sup_qos_profiles;

  return E_SUCCESS;

} /* hdrmrlpc_get_an_sup_qos_prof_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_SET_CONFIG_PARAMS_DEFAULT

DESCRIPTION
  Sets the Current RLP parameters to their default values.

DEPENDENCIES
  None.

PARAMETERS
  current_config - Pointer to RLP's current configuration structure in 
                   SCMDB.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpc_set_config_params_default
( 
  hdrmrlpc_config_params_type *current_config 
)
{
  /* Convenience pointers */
  hdrmrlpc_flow_nn_config_type *flow_ptr;   
  hdrmrlpc_reservation_kk_config_type *res_ptr; 

  /* iterator */
  uint8 flow_nn;   
  uint8 res_kk;
  uint8 prot_pp;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset((void*)current_config, 0, sizeof(hdrmrlpc_config_params_type));

  current_config->max_abort_timer   = HDRMRLPC_MAX_ABORT_TIMER_DEF;
  current_config->ppp_free_authentication_support =
                                      HDRMRLPC_PPP_FREE_AUTHENTICATION_SUPPORT_DEF;
  current_config->two_routes_supported = 
                                      HDRMRLPC_TWO_ROUTES_SUPPORTED_DEF;

  current_config->at_nak_delay_supported = 
                                      HDRMRLPC_AT_NAKDELAY_SUPPORTED_DEF;

  current_config->sup_higher_layer_prot.prot_count = 
                                      HDRMRLPC_PROTOCOL_COUNT_DEF;
  current_config->sup_higher_layer_prot.prot_id[0] = 
                                      HDRMRLPC_PROTOCOL_IDENTIFIER_DEF;
  current_config->an_sup_qos_profiles.profile_count =
                                      HDRMRLPC_AT_SQP_PROFILE_COUNT_DEF;
  current_config->an_sup_qos_profiles.profile_count =
                                      HDRMRLPC_AN_SQP_PROFILE_COUNT_DEF;

  current_config->max_rlp_flows.max_num_rlp_flows_fwd =
                                      HDRMRLPC_MAX_NUM_RLP_FLOWS_FWD_DEF;
  current_config->max_rlp_flows.max_num_rlp_flows_rev =
                                      HDRMRLPC_MAX_NUM_RLP_FLOWS_REV_DEF;
  current_config->max_rlp_flows.max_activated_rlp_flows_fwd =
                                      HDRMRLPC_MAX_ACTIVATED_RLP_FLOWS_FWD_DEF;
  current_config->max_rlp_flows.max_activated_rlp_flows_rev =
                                      HDRMRLPC_MAX_ACTIVATED_RLP_FLOWS_REV_DEF;

  current_config->max_reservations.max_num_rsv_fwd =
                                      HDRMRLPC_MAX_NUM_RESERVATIONS_FWD_DEF;
  current_config->max_reservations.max_num_rsv_rev =
                                      HDRMRLPC_MAX_NUM_RESERVATIONS_REV_DEF;
  current_config->max_reservations.max_num_open_rsv_fwd =
                                      HDRMRLPC_MAX_NUM_OPEN_RESERVATIONS_FWD_DEF;
  current_config->max_reservations.max_num_open_rsv_rev =
                                      HDRMRLPC_MAX_NUM_OPEN_RESERVATIONS_REV_DEF;


  /*=====================*/
  /*  FWD RLP FLOWS      */
  /*=====================*/
  for (flow_nn = 0; flow_nn < HDRMRLPC_N_MRLP_MAX_FWD; flow_nn++ )
  {
    flow_ptr = &current_config->fwd_rlp_config_info[flow_nn];
    
    /* Not configurable attributes */
    flow_ptr->rlp_flow                   = flow_nn;
    flow_ptr->rlp_ver                    = HDRMRLPC_FWD_RLP;
    flow_ptr->rlp_priority               = 0xFF;

    flow_ptr->nak_enabled = HDRMRLPC_NAK_ENABLE_FWD_DEF;
    flow_ptr->higher_layer_protocol = HDRMRLPC_HIGHER_LAYER_PROTOCOL_FWD_DEF;
    flow_ptr->simple_sequence_length   = HDRMRLPC_SEQ_LEN_FWD_DEF;
    flow_ptr->data_unit         = HDRMRLPC_DATA_UNIT_FWD_DEF;
    flow_ptr->sim_del_both_routes = HDRMRLPC_SIM_DEL_BOTH_ROUTES_FWD_DEF;
    flow_ptr->ooo_del_route_prot  = HDRMRLPC_OOO_DEL_ROUTE_PROT_FWD_DEF;
    flow_ptr->nak_delay_time  = HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_DEF;

    if (flow_nn == 0)
    {
      flow_ptr->activated          = HDRMRLPC_FLOW_00_ACTIVATED_FWD_DEF;

      flow_ptr->flow_id.is_active  = HDRMRLPC_FLOW_00_ACTIVE_FWD_DEFAULT;
      flow_ptr->flow_id.seq_len    = HDRMRLPC_FLOW_00_SEQ_LEN_FWD_DEFAULT;
      flow_ptr->flow_id.rlp_id_len = HDRMRLPC_FLOW_00_RLP_ID_LEN_FWD_DEFAULT;
      flow_ptr->flow_id.rlp_id     = HDRMRLPC_FLOW_00_RLP_ID_FWD_DEFAULT;

      flow_ptr->flow_rsv.rsv_count    = HDRMRLPC_FLOW_00_RESERVATION_COUNT_FWD_DEF;
      flow_ptr->flow_rsv.rsv_label[0] = HDRMRLPC_FLOW_00_RESERVATION_LABEL_FWD_DEF;
    }
#if defined(FEATURE_HDR_EMPA) || defined (FEATURE_HDR_REVB)
    /* according to standard, flow01 is activated by default */
    else if (flow_nn == 1)
    {
      flow_ptr->activated          = HDRMRLPC_FLOW_01_ACTIVATED_FWD_DEF;
    }
#endif
    else
    {
      flow_ptr->activated          = HDRMRLPC_FLOW_NN_ACTIVATED_FWD_DEF;

      flow_ptr->flow_id.is_active  = HDRMRLPC_ACTIVE_FWD_DEFAULT;
      flow_ptr->flow_id.seq_len    = 0; /* N/A */
      flow_ptr->flow_id.rlp_id_len = 0; /* N/A */
      flow_ptr->flow_id.rlp_id     = 0; /* N/A */
    }

    if (flow_nn == 1)
    {
      flow_ptr->flow_prot_sdu = HDRMRLPC_FLOW_01_FLOW_PROT_SDU_FWD_DEF;
      flow_ptr->route_prot_sdu = HDRMRLPC_FLOW_01_ROUTE_PROT_SDU_FWD_DEF;
    }
    else
    {
      flow_ptr->flow_prot_sdu = HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_FWD_DEF;
      flow_ptr->route_prot_sdu = HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_FWD_DEF;
    }

    /* Complex SequenceLength Fwd attribute (MMPA) */
    flow_ptr->complex_seq_len.sar_seq_len = HDRMRLPC_SAR_SEQ_LEN_FWD_DEFAULT;
    flow_ptr->complex_seq_len.sar_seq_len_short = HDRMRLPC_SAR_SEQ_LEN_SHORT_FWD_DEFAULT;

    flow_ptr->flow_timer.abort_timer_val = HDRMRLPC_ABORT_TIMER_FWD_DEF;
    flow_ptr->flow_timer.flush_timer_val = HDRMRLPC_FLUSH_TIMER_FWD_DEF;
    /* MMPA has a additional field for FlowNNTimerFwd - NakDelayTimer */
    flow_ptr->flow_timer.nak_delay_timer_val = HDRMRLPC_NAKDELAY_TIMER_FWD_DEF;

    /* FlowProtocolParameters */
    if (flow_nn == 0)
    {
        current_config->fwd_flow_prot[flow_nn].id = HDRMRLPC_FLOW_PROT_ID_HDLC;
    }
    else if (flow_nn == 1)
    {
        current_config->fwd_flow_prot[flow_nn].id = HDRMRLPC_FLOW_PROT_ID_IPV4_IPV6;
      }
    else
    {
      current_config->fwd_flow_prot[flow_nn].id = HDRMRLPC_FLOW_PROT_ID_NULL;
    }
    current_config->fwd_flow_prot[flow_nn].length = 0;

    /* Route Protocol Parameters */
    current_config->fwd_route_prot[flow_nn].id = HDRMRLPC_ROUTE_PROT_ID_NULL;
    current_config->fwd_route_prot[flow_nn].length = 0;
  }

  /*=====================*/
  /*  REV RLP FLOWS      */
  /*=====================*/
  for (flow_nn = 0; flow_nn < HDRMRLPC_N_MRLP_MAX_REV; flow_nn++ )
  {
    flow_ptr = &current_config->rev_rlp_config_info[flow_nn];
    
    /* Not configurable attribute*/
    flow_ptr->rlp_flow                   = flow_nn;
    flow_ptr->rlp_ver                    = HDRMRLPC_REV_RLP;
    flow_ptr->rlp_priority               = 0xFF;

    flow_ptr->nak_enabled = HDRMRLPC_NAK_ENABLE_REV_DEF;
    flow_ptr->higher_layer_protocol = HDRMRLPC_HIGHER_LAYER_PROTOCOL_REV_DEF;
    flow_ptr->phys_link_nak_enabled_rev = HDRMRLPC_PHYS_LAYER_NAK_ENABLED_REV_DEF;
    flow_ptr->xmit_abort_timer = HDRMRLPC_TRANSMIT_ABORT_TIMER_REV_DEF;
    flow_ptr->dos_allowed      = HDRMRLPC_DATA_OVER_SIG_ALLOWED_REV_DEF;
    flow_ptr->simple_sequence_length   = HDRMRLPC_SEQ_LEN_REV_DEF;
    flow_ptr->data_unit         = HDRMRLPC_DATA_UNIT_REV_DEF;

    if (flow_nn == 0)
    {
      flow_ptr->activated          = HDRMRLPC_FLOW_00_ACTIVATED_REV_DEF;

      flow_ptr->flow_id.is_active  = HDRMRLPC_FLOW_00_ACTIVE_REV_DEFAULT;
      flow_ptr->flow_id.seq_len    = HDRMRLPC_FLOW_00_SEQ_LEN_REV_DEFAULT;
      flow_ptr->flow_id.rlp_id_len = HDRMRLPC_FLOW_00_RLP_ID_LEN_REV_DEFAULT;
      flow_ptr->flow_id.rlp_id     = HDRMRLPC_FLOW_00_RLP_ID_REV_DEFAULT;

      flow_ptr->flow_rsv.rsv_count    = HDRMRLPC_FLOW_00_RESERVATION_COUNT_REV_DEF;
      flow_ptr->flow_rsv.rsv_label[0] = HDRMRLPC_FLOW_00_RESERVATION_LABEL_REV_DEF;
    }
#if defined(FEATURE_HDR_EMPA) || defined (FEATURE_HDR_REVB)
    else if (flow_nn == 1)
    {
      flow_ptr->activated          = HDRMRLPC_FLOW_01_ACTIVATED_REV_DEF;
    }
#endif
    else
    {
      flow_ptr->activated          = HDRMRLPC_FLOW_NN_ACTIVATED_REV_DEF;

      flow_ptr->flow_id.is_active  = HDRMRLPC_ACTIVE_REV_DEFAULT;
      flow_ptr->flow_id.seq_len    = 0; /* N/A */
      flow_ptr->flow_id.rlp_id_len = 0; /* N/A */
      flow_ptr->flow_id.rlp_id     = 0; /* N/A */
    }

    if (flow_nn == 1)
    {
      flow_ptr->flow_prot_sdu = HDRMRLPC_FLOW_01_FLOW_PROT_SDU_REV_DEF;
      flow_ptr->route_prot_sdu = HDRMRLPC_FLOW_01_ROUTE_PROT_SDU_REV_DEF;
    }
    else
    {
      flow_ptr->flow_prot_sdu = HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_REV_DEF;
      flow_ptr->route_prot_sdu = HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_REV_DEF;
    }

    flow_ptr->flow_timer.abort_timer_val = HDRMRLPC_ABORT_TIMER_REV_DEF;
    flow_ptr->flow_timer.flush_timer_val = HDRMRLPC_FLUSH_TIMER_REV_DEF;

    /* FlowProtocolParameters */
    if (flow_nn == 0)
    {
        current_config->rev_flow_prot[flow_nn].id = HDRMRLPC_FLOW_PROT_ID_HDLC;
    }
    else if (flow_nn == 1)
    {
        current_config->rev_flow_prot[flow_nn].id = HDRMRLPC_FLOW_PROT_ID_IPV4_IPV6;
      }
    else
    {
      current_config->rev_flow_prot[flow_nn].id = HDRMRLPC_FLOW_PROT_ID_NULL;
    }
    current_config->rev_flow_prot[flow_nn].length = 0;

    /* Route Protocol Parameters */
    current_config->rev_route_prot[flow_nn].id = HDRMRLPC_ROUTE_PROT_ID_NULL;
    current_config->rev_route_prot[flow_nn].length = 0;
  }

  /*=====================*/
  /*  FWD RES LABELS     */
  /*=====================*/
  current_config->num_fwd_rsv = 0;
  for (res_kk = 0; res_kk < HDRMRLPC_N_RES_LABELS_MAX_FWD; res_kk++ )
  {
    res_ptr = &current_config->fwd_res_info[res_kk];

    res_ptr->rsv_label = 0;

    res_ptr->idle_state = HDRMRLPC_IDLE_STATE_FWD_DEF; 

    res_ptr->qos_request.type = HDRMRLPC_PROFILE_TYPE_REQ_FWD_DEF;
    res_ptr->qos_request.length = HDRMRLPC_PROFILE_LENGTH_REQ_FWD_DEF;

    res_ptr->qos_response.type = HDRMRLPC_PROFILE_TYPE_RSP_FWD_DEF;
    res_ptr->qos_response.length = HDRMRLPC_PROFILE_LENGTH_RSP_FWD_DEF;
  }
  /* FWD RES LABEL 0xFF*/
  current_config->fwd_res_info_ff.idle_state = HDRMRLPC_IDLE_STATE_FWD_DEF;
  current_config->fwd_res_info_ff.qos_request.type = HDRMRLPC_PROFILE_TYPE_REQ_FWD_DEF;
  current_config->fwd_res_info_ff.qos_request.length = HDRMRLPC_PROFILE_LENGTH_REQ_FWD_DEF;
  current_config->fwd_res_info_ff.qos_response.type = HDRMRLPC_PROFILE_TYPE_RSP_FWD_DEF;
  current_config->fwd_res_info_ff.qos_response.length = HDRMRLPC_PROFILE_LENGTH_RSP_FWD_DEF;

  /*=====================*/
  /*  REV RES LABELS     */
  /*=====================*/
  current_config->num_rev_rsv = 0;
  for (res_kk = 0; res_kk < HDRMRLPC_N_RES_LABELS_MAX_REV; res_kk++ )
  {
    res_ptr = &current_config->rev_res_info[res_kk];

    res_ptr->rsv_label = 0;

    res_ptr->idle_state = HDRMRLPC_IDLE_STATE_REV_DEF; 

    res_ptr->qos_request.type = HDRMRLPC_PROFILE_TYPE_REQ_REV_DEF;
    res_ptr->qos_request.length = HDRMRLPC_PROFILE_LENGTH_REQ_REV_DEF;

    res_ptr->qos_response.type = HDRMRLPC_PROFILE_TYPE_RSP_REV_DEF;
    res_ptr->qos_response.length = HDRMRLPC_PROFILE_LENGTH_RSP_REV_DEF;
  }
  /* REV RES LABEL 0xFF*/
  current_config->rev_res_info_ff.idle_state = HDRMRLPC_IDLE_STATE_REV_DEF;
  current_config->rev_res_info_ff.qos_request.type = HDRMRLPC_PROFILE_TYPE_REQ_REV_DEF;
  current_config->rev_res_info_ff.qos_request.length = HDRMRLPC_PROFILE_LENGTH_REQ_REV_DEF;
  current_config->rev_res_info_ff.qos_response.type = HDRMRLPC_PROFILE_TYPE_RSP_REV_DEF;
  current_config->rev_res_info_ff.qos_response.length = HDRMRLPC_PROFILE_LENGTH_RSP_REV_DEF;

  /*=====================*/
  /* Protocol Parameters */
  /*=====================*/
  for ( prot_pp = 0; prot_pp < HDRMRLP_MAX_PROT_COUNT; prot_pp++ )
  {
    switch (prot_pp)
    {
      case HDRMRLPC_FLOW_PROT_ID_NULL:
      case HDRMRLPC_FLOW_PROT_ID_HDLC:
      case HDRMRLPC_FLOW_PROT_ID_IPV4:
        current_config->at_sup_flow_prot[prot_pp].prot_supported = TRUE;
        break;

      case HDRMRLPC_FLOW_PROT_ID_IPV4_IPV6:
          current_config->at_sup_flow_prot[prot_pp].prot_supported = TRUE;
        break;

      case HDRMRLPC_FLOW_PROT_ID_IPV6:
      case HDRMRLPC_FLOW_PROT_ID_ROHC:
#ifdef FEATURE_EHRPD
      case HDRMRLPC_FLOW_PROT_ID_VSNP:
      case HDRMRLPC_FLOW_PROT_ID_PDNMUX:
#endif /* FEATURE_EHRPD */
      default:
        current_config->at_sup_flow_prot[prot_pp].prot_supported = FALSE;
        break;
    }
    current_config->at_sup_flow_prot[prot_pp].value_len = 0;
  }

  for ( prot_pp = 0; prot_pp < HDRMRLP_MAX_PROT_COUNT; prot_pp++ )
  {
    if ( prot_pp == HDRMRLPC_ROUTE_PROT_ID_NULL )
    {
      current_config->at_sup_route_prot[prot_pp].prot_supported = TRUE;
    }
    else
    {
      current_config->at_sup_route_prot[prot_pp].prot_supported = FALSE;
    }
    current_config->at_sup_route_prot[prot_pp].value_len = 0;
  }

} /* hdrmrlpc_set_config_params_default() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_IS_RLP_FLOW_SUPPORTED

DESCRIPTION
  This function checks if this RLP Flow number is within the maximum number
  of RLPs allowed.

DEPENDENCIES
  The AT only supports a maximum of:
  - 32 RLP Flows in the fwd direction (3 active RLPs max)
  - 32 RLP Flows in the rev direction (3 active RLPs max)
  - 20 Reservation Labels in the fwd direction
  - 20 Reservation Labels in the rev direction

PARAMETERS
  attrib_name - attribute to validate
  index       - index to check (RLP Flow or Reservation Label)
    
RETURN VALUE
  None.

SIDE EFFECTS
  _mpa_ right now, relies on Max Fwd = Max Rev.

===========================================================================*/
boolean hdrmrlpc_is_index_supported
(
  hdrmrlpc_attrib_name_enum_type attrib_name,
  uint8 index
)
{
   
  boolean is_index_supported = FALSE;

  switch (attrib_name)
  {
    case HDRMRLPC_ATTRIB_MAX_ABORT_TIMER:
    case HDRMRLPC_ATTRIB_TWO_ROUTES_SUPPORTED:

      is_index_supported = TRUE;
    break;

    case HDRMRLPC_ATTRIB_NAK_EN_FWD:
    case HDRMRLPC_ATTRIB_NAK_EN_REV:
    case HDRMRLPC_ATTRIB_HIGH_LAYER_PROT_FWD:
    case HDRMRLPC_ATTRIB_HIGH_LAYER_PROT_REV:
    case HDRMRLPC_ATTRIB_PHYS_LAYER_NAK_REV:
    case HDRMRLPC_ATTRIB_RLP_PRIORITY_FWD:
    case HDRMRLPC_ATTRIB_RLP_PRIORITY_REV:
    case HDRMRLPC_ATTRIB_SUP_HIGH_LAYER_PROT:
    case HDRMRLPC_ATTRIB_IDENTIFICATION_FWD:
    case HDRMRLPC_ATTRIB_IDENTIFICATION_REV:
    case HDRMRLPC_ATTRIB_TIMERS_FWD:
    case HDRMRLPC_ATTRIB_TIMERS_REV:
    case HDRMRLPC_ATTRIB_RESERV_FWD:
    case HDRMRLPC_ATTRIB_RESERV_REV:
    case HDRMRLPC_ATTRIB_SIMPLE_SEQUENCE_LENGTH_FWD:
    case HDRMRLPC_ATTRIB_SEQUENCE_LENGTH_REV:
    case HDRMRLPC_ATTRIB_DATA_UNIT_FWD:
    case HDRMRLPC_ATTRIB_DATA_UNIT_REV:
    case HDRMRLPC_ATTRIB_ROUTE_PROT_SDU_FWD:
    case HDRMRLPC_ATTRIB_ROUTE_PROT_SDU_REV:
    case HDRMRLPC_ATTRIB_SIM_DEL_BOTH_ROUTES_FWD:
    case HDRMRLPC_ATTRIB_OOO_DEL_ROUTE_PROT_FWD:
    case HDRMRLPC_ATTRIB_FLOW_PROT_PARM_FWD:
    case HDRMRLPC_ATTRIB_FLOW_PROT_PARM_REV:
    case HDRMRLPC_ATTRIB_ROUTE_PROT_PARM_FWD:
    case HDRMRLPC_ATTRIB_ROUTE_PROT_PARM_REV:
    case HDRMRLPC_ATTRIB_TRANSMIT_ABORT_TIMER_REV:
    case HDRMRLPC_ATTRIB_DATA_OVER_SIG_ALLOWED_REV:
    case HDRMRLPC_ATTRIB_COMPLEX_SEQUENCE_LENGTH_FWD:
    case HDRMRLPC_ATTRIB_FLOW_NAK_DELAY_TIME_FWD:

      if (index < HDRMRLPC_N_MRLP_MAX)
      {
        is_index_supported = TRUE;
      }
      else
      {
        ERR("RLP flow %d is not valid (attrib %d)",
            index,attrib_name,0);

        is_index_supported = FALSE;
      }

    break;

    case HDRMRLPC_ATTRIB_RESKK_IDLE_STATE_FWD:
    case HDRMRLPC_ATTRIB_RESKK_IDLE_STATE_REV:
    case HDRMRLPC_ATTRIB_RESKK_QOS_REQ_FWD:
    case HDRMRLPC_ATTRIB_RESKK_QOS_REQ_REV:
    case HDRMRLPC_ATTRIB_RESKK_QOS_RSP_FWD:
    case HDRMRLPC_ATTRIB_RESKK_QOS_RSP_REV:
      
      /* No need to range check this because the data type range is 
         the same as the possible valid values. */
      is_index_supported = TRUE;

    break;

    case HDRMRLPC_ATTRIB_AT_SUP_FLOW_PROT_PARM:
    case HDRMRLPC_ATTRIB_AT_SUP_ROUTE_PROT_PARM:
      if ( index < HDRMRLP_MAX_PROT_COUNT)
      {
        is_index_supported = TRUE;
      }
      else
      {
        ERR("AT Sup Prot parm %d is not valid (attrib %d)",
            index,attrib_name,0);

        is_index_supported = FALSE;
      }
      break;

    default:
      ERR("Unknown attribute (%d)",attrib_name,0,0);
    break;
  }

  return is_index_supported;

} /* hdrmrlpc_is_index_supported() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UPDATE_ALL_VERNO

DESCRIPTION
  This function increments every attribute's version number.

DEPENDENCIES
  None

PARAMETERS
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  _mpa_ temporary

===========================================================================*/
void hdrmrlpc_update_all_verno( void )
{

  uint8 index; /* iterator  (either flowNN or resKK)*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  hdrmrlpc.verno.max_abort_timer++;
  hdrmrlpc.verno.ppp_free_authentication_support++;
  hdrmrlpc.verno.two_routes_supported++;
  hdrmrlpc.verno.at_nak_delay_supported++;

  hdrmrlpc.verno.sup_higher_layer_prot++;
  hdrmrlpc.verno.an_sup_qos_profiles++;
  hdrmrlpc.verno.max_rlp_flows++;
  hdrmrlpc.verno.max_reservations++;

  hdrmrlpc.verno.reskk_idle_state_fwd_ff++;
  hdrmrlpc.verno.reskk_idle_state_rev_ff++;

  for (index=0; index < HDRMRLPC_N_MRLP_MAX; index++)
  {
    /* Simple attributes */
    hdrmrlpc.verno.nak_enable_fwd[index]++;
    hdrmrlpc.verno.nak_enable_rev[index]++;
    hdrmrlpc.verno.higher_layer_prot_fwd[index]++;
    hdrmrlpc.verno.higher_layer_prot_rev[index]++;
    hdrmrlpc.verno.phys_link_nak_en_rev[index]++;
    hdrmrlpc.verno.xmit_abort_timer_rev[index]++;
    hdrmrlpc.verno.dos_allowed_rev[index]++;
    hdrmrlpc.verno.activated_fwd[index]++;
    hdrmrlpc.verno.activated_rev[index]++;
    hdrmrlpc.verno.sequence_length_fwd[index]++;
    hdrmrlpc.verno.sequence_length_rev[index]++;
    hdrmrlpc.verno.flow_prot_sdu_fwd[index]++;
    hdrmrlpc.verno.flow_prot_sdu_rev[index]++;
    hdrmrlpc.verno.data_unit_fwd[index]++;
    hdrmrlpc.verno.data_unit_rev[index]++;
    hdrmrlpc.verno.route_prot_sdu_fwd[index]++;
    hdrmrlpc.verno.route_prot_sdu_rev[index]++;
    hdrmrlpc.verno.sim_del_both_routes_fwd[index]++;
    hdrmrlpc.verno.ooo_del_route_prot_fwd[index]++;

    /* Complex Attributes */
    hdrmrlpc.verno.identification_fwd[index]++;
    hdrmrlpc.verno.identification_rev[index]++;
    hdrmrlpc.verno.timers_fwd[index]++;
    hdrmrlpc.verno.timers_rev[index]++;
    hdrmrlpc.verno.reservation_fwd[index]++;
    hdrmrlpc.verno.reservation_rev[index]++;
    hdrmrlpc.verno.flow_prot_parm_fwd[index]++;
    hdrmrlpc.verno.flow_prot_parm_rev[index]++;
    hdrmrlpc.verno.route_prot_parm_fwd[index]++;
    hdrmrlpc.verno.route_prot_parm_rev[index]++;

    hdrmrlpc.verno.rlp_priority_fwd[index]++;
    hdrmrlpc.verno.rlp_priority_rev[index]++;
  }

  for (index=0; index < HDRMRLPC_N_RES_LABELS_MAX; index++)
  {
    hdrmrlpc.verno.reskk_idle_state_fwd[index]++;
    hdrmrlpc.verno.reskk_idle_state_rev[index]++;
    hdrmrlpc.verno.reskk_qos_req_fwd[index]++;
    hdrmrlpc.verno.reskk_qos_req_rev[index]++;
    hdrmrlpc.verno.reskk_qos_rsp_fwd[index]++;
    hdrmrlpc.verno.reskk_qos_rsp_rev[index]++;
  }

  for (index = 0; index < HDRMRLP_MAX_PROT_COUNT; index++)
  {
    hdrmrlpc.verno.at_sup_flow_prot_parm_pp[index]++;
    hdrmrlpc.verno.at_sup_route_prot_parm_pp[index]++;
  }
} /* hdrmrlpc_update_all_verno */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_AT_CONFIG_INIT

DESCRIPTION
  This function initializes the AT Configurable attribute with the
  "desired" value.

DEPENDENCIES
  None

PARAMETERS
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpc_at_config_init ( void )
{

  hdrmrlpc_sup_prot_rohc_parm_value_type *rohc_ptr;
#ifdef FEATURE_EHRPD
  hdrscp_app_subtype_enable_mask_type app_subtype_mask;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_SUPPORT_RSP 
  #error code not present
#endif

  /* Initializes the ATSupportedFlowProtocolParameters
     and ATSupportedRouteProtocolParameters Attributes */
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_NULL].prot_supported = TRUE;
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_NULL].value_len      = 0;
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_HDLC].prot_supported = TRUE;
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_HDLC].value_len      = 0;  
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV4].prot_supported = TRUE;
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV4].value_len      = 0;
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV6].prot_supported = FALSE;
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV6].value_len      = 0;  
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_ROHC].prot_supported = TRUE;

  if ( hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_ROHC].prot_supported )
  {
    /* Set the desired RoHC parameters for Flow Protocol */
    rohc_ptr = &hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_ROHC].value.rohc;

    rohc_ptr->max_max_cid = 0x0F;
    rohc_ptr->large_cid_supported = TRUE;
    rohc_ptr->max_mrru = 0;
    rohc_ptr->timer_comp_supported = TRUE;
    rohc_ptr->profile_count = 4;
    rohc_ptr->profile[0] = 0;
    rohc_ptr->profile[1] = 1;
    rohc_ptr->profile[2] = 2;
    rohc_ptr->profile[3] = 3;

    hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_ROHC].value_len = 
      HDRMRLPC_PROT_ROHC_VALUE_LEN;
  }
  else
  {
    hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_ROHC].value_len = 0;
  }

#ifdef FEATURE_EHRPD
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_RSV1].prot_supported = FALSE;
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_RSV1].value_len      = 0;  

  hdrscp_get_app_subtype_config(&app_subtype_mask);

  /* The advertisement of eHRPD should be consistant with 
   * ATSupportedApplicationSubtypes attribute. 
   */
  if ( (app_subtype_mask.enable_aempa || app_subtype_mask.enable_ammpa) && 
       hdrscp_ehrpd_was_advertised() )
  {
    hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_VSNP].prot_supported = TRUE;
    hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_VSNP].value_len      = 0;  
  }
  else 
  {
    hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_VSNP].prot_supported = FALSE;
    hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_VSNP].value_len      = 0; 
  }

  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_PDNMUX].prot_supported = FALSE;
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_PDNMUX].value_len      = 0;  
#endif /* FEATURE_EHRPD */

  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV4_IPV6].prot_supported = TRUE;
  hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV4_IPV6].value_len      = 0;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_NULL].prot_supported = TRUE;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_NULL].value_len      = 0;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV1].prot_supported = FALSE;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV1].value_len      = 0;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV2].prot_supported = FALSE;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV2].value_len      = 0;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV3].prot_supported = FALSE;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV3].value_len      = 0;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_ROHC].prot_supported = TRUE;

  if ( hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_ROHC].prot_supported )
  {
    /* Set the desired RoHC parameters for Route Protocol */
    rohc_ptr = &hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_ROHC].value.rohc;

    rohc_ptr->max_max_cid = 0x0F;
    rohc_ptr->large_cid_supported = TRUE;
    rohc_ptr->max_mrru = 0;
#ifdef FEATURE_HDR_EMPA_SUPPORT_STD_V115_ATTRIB_ID
    if (hdrmrlpc.support_v115)
    {
      rohc_ptr->max_delay_decomp_depth = 5;
    }
#endif
    rohc_ptr->timer_comp_supported = TRUE;
    rohc_ptr->profile_count = 4;
    rohc_ptr->profile[0] = 0;
    rohc_ptr->profile[1] = 1;
    rohc_ptr->profile[2] = 2;
    rohc_ptr->profile[3] = 3;

    hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_ROHC].value_len = 
      HDRMRLPC_PROT_ROHC_VALUE_LEN;
  }
  else
  {
    hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_ROHC].value_len = 0;
  }

#ifdef FEATURE_EHRPD
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV4].prot_supported = FALSE;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV4].value_len      = 0;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV5].prot_supported = FALSE;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV5].value_len      = 0;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV6].prot_supported = FALSE;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV6].value_len      = 0;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV7].prot_supported = FALSE;
  hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_RSV7].value_len      = 0;
#endif /* FEATURE_EHRPD */

  /* Set the MaxRLPFlow */
  hdrmrlpc.desired.max_rlp_flows.max_num_rlp_flows_fwd = HDRMRLPC_MAX_NUM_RLP_FLOWS_FWD_MAX;
  hdrmrlpc.desired.max_rlp_flows.max_activated_rlp_flows_fwd = HDRMRLP_MAX_FWD_RLPS;
  hdrmrlpc.desired.max_rlp_flows.max_num_rlp_flows_rev = HDRMRLPC_MAX_NUM_RLP_FLOWS_REV_MAX;
  hdrmrlpc.desired.max_rlp_flows.max_activated_rlp_flows_rev = HDRMRLP_MAX_REV_RLPS;

  /* Set the MaxReservation */
  hdrmrlpc.desired.max_reservations.max_num_rsv_fwd = HDRMRLP_MAX_FWD_RESERVATIONS;
  hdrmrlpc.desired.max_reservations.max_num_open_rsv_fwd = HDRMRLP_MAX_FWD_RESERVATIONS;
  hdrmrlpc.desired.max_reservations.max_num_rsv_rev = HDRMRLP_MAX_REV_RESERVATIONS;
  hdrmrlpc.desired.max_reservations.max_num_open_rsv_rev = HDRMRLP_MAX_REV_RESERVATIONS;
} /* hdrmrlpc_at_config_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_PROCESS_IND

DESCRIPTION
  This function processes indications given to MRLPC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for MRLPC to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 unreferenced variable ind_data_ptr*/
void hdrmrlpc_process_ind 
( 
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{

  switch (ind_name)
  {
    case HDRIND_STREAM_CONFIG_COMPLETE:
      break;

    case HDRIND_SCM_SESSION_CHANGED:
      /* Update the local copy of current configuration from SCMDB */
      hdrscmdb_get_inuse_config(HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL, 
                                &hdrmrlpc.current);

      #ifdef FEATURE_EHRPD
      if ( (hdrstream_stream_is_negotiated(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
            hdrstream_stream_is_negotiated(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) )
            &&
           (hdrmrlpc.current.fwd_flow_prot[0].id == 0x07) )
      {
        HDRTRACE_MRLP_EHRPD_PERSONALITY_IS_ACTIVE( TRUE );
      }
      else
      {
        HDR_MSG_PROT_3 (MSG_LEGACY_MED, 
                        "hdrstream_is_negotiated: returned false for app_type (%d) and (%d) and fwd_flow_prot_id - %d",
                         HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN,
                         HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN,
                         hdrmrlpc.current.fwd_flow_prot[0].id);
        HDRTRACE_MRLP_EHRPD_PERSONALITY_IS_ACTIVE( FALSE );
      }
      #endif /* FEATURE_EHRPD */

      /* We use the "in_config" place holder during GAUP negotiation.
       * We initialize it here to be the same as the current version. 
       */
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config = hdrmrlpc.current ; 
#else
      memcpy( &hdrmrlpc.in_config,
              &hdrmrlpc.current,
              sizeof(hdrmrlpc_config_params_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      /* Reset the config params flags */
      memset(&hdrmrlpc.config_result,0,sizeof(hdrmrlpc_config_req_result_type));

      if ( ind_data_ptr->change_reason == HDRHAI_SCHG_CLOSED_NORMAL || 
           ind_data_ptr->change_reason == HDRHAI_SCHG_CLOSED_RESTORE_PRIOR )
      {
        /* If session is closed, set the version numbers to 0 for all the 
         * attributes */
        memset(&hdrmrlpc.verno, 0, sizeof(hdrmrlpc_config_verno_type));

        /* Clear the GAUPResponse timer */
        (void) rex_clr_timer( &hdrmrlpc.gaup_rsp_timer );
      }
      break;

    default:
      ERR_FATAL( "Unknown indication", 0, 0, 0 );
      break;
  }
} /* hdrmrlpc_process_ind() */
/*lint -restore*/

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRMRLPC_TIMER_CB

DESCRIPTION
  This function is the callback for all hdrmrlpc protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void hdrmrlpc_timer_cb
(
  uint32 timer_id
)
{
  hdrmc_queue_timer(HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL, timer_id);
} /* hdrmrlpc_timer_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_POWERUP_INIT

DESCRIPTION
  Does the powerup initialization of the MRLP configuration

DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpc_powerup_init( void )
{

  uint8 index; /* loop index */

#ifdef FEATURE_HDR_EMPA_SUPPORT_STD_V115_ATTRIB_ID
  uint16 support_empa_v115;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrmc_is_first_init() )
  {
    rex_init_crit_sect(&hdrmrlpc.crit_sect);
    
    /* Initialize GAUP n-rsp timer related info */
    rex_def_timer_ex( &hdrmrlpc.gaup_rsp_timer,
                      hdrmrlpc_timer_cb,
                      HDRMRLPC_GAUP_RSP_TIMER);

    /* Fill hdrmrlpc.pending_gaup_req_free_q */
    ( void ) q_init( &hdrmrlpc.pending_gaup_req_q );
  }

  hdrmrlpc.gaup_timer_cnt = 0;
  /* Transcation ID is 0 - 254 inclusive. */
  hdrmrlpc.gaup_timer_trans_id = 0xFF;
  hdrmrlpc.gaup_trans_id  = 0;

  for (index=0; index < HDRMRLPC_MAX_TOTAL_RSV; index++)
  {
    hdrmrlpc.gaup_timer_list[index].trans_id = 0;
    hdrmrlpc.gaup_timer_list[index].time_left_ms = 0;
  }

  /* Get current configuration parameters from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL,
                            &hdrmrlpc.current);

#ifdef FEATURE_MEMCPY_REMOVAL
  hdrmrlpc.in_config = hdrmrlpc.current;
#else
  memcpy( &hdrmrlpc.in_config,
          &hdrmrlpc.current,
          sizeof(hdrmrlpc_config_params_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  hdrmrlpc_config_init();

#ifdef FEATURE_HDR_EMPA_SUPPORT_STD_V115_ATTRIB_ID
  /* Read the NV items to support whether AT should support V1.15 or
   * V2.0 EMPA Attributes */
  if ( hdrutil_read_nv_item ( NV_COLLOC_DISALLOWED_TIME_I, 
         (nv_item_type *)&support_empa_v115 )
       != NV_DONE_S )
  {
    /* If this item is inactive, default to support V2.0 */
    hdrmrlpc.support_v115 = FALSE;
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  " EMPA: Support V1.15 NV item not active, defalut V2.0");
  }
  else
  {
    if ( support_empa_v115 != 0 )
    {
      hdrmrlpc.support_v115 = TRUE;
      HDR_MSG_PROT( MSG_LEGACY_HIGH, " EMPA: Support V1.15" );
    }
    else
    {
      hdrmrlpc.support_v115 = FALSE;
      HDR_MSG_PROT( MSG_LEGACY_HIGH, " EMPA: Support V2.0" );
    }
  }
#endif

} /* hdrmrlpc_powerup_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_CONFIG_IS_NEEDED

DESCRIPTION
  This helper function determines whether configuration is necessary.

DEPENDENCIES
  None
  _mpa_ HDRMC task context

PARAMETERS
  None
  
RETURN VALUE
  TRUE  - Configuration is necessary
  FALSE - Configuration is not necessary

SIDE EFFECTS
  

===========================================================================*/
boolean hdrmrlpc_config_is_needed ( void )
{
  boolean config_is_needed = FALSE;
#if defined(FEATURE_HDR_EMPA) || defined (FEATURE_HDR_REVB)
  boolean empa_is_negotiated = FALSE;
  boolean mmpa_is_negotiated = FALSE;
#endif
/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - */

#if defined(FEATURE_HDR_EMPA) || defined (FEATURE_HDR_REVB)
  empa_is_negotiated = hdrstream_stream_is_negotiated(
                            HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);

  mmpa_is_negotiated = hdrstream_stream_is_negotiated(
                            HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);

  HDR_MSG_PROT_2 (MSG_LEGACY_MED, "empa_is_negotiated:%d mmpa_is_negotiated:%d",
                  empa_is_negotiated,mmpa_is_negotiated);

  if (empa_is_negotiated || mmpa_is_negotiated)
  {
#ifdef FEATURE_HDR_SUPPORT_RSP
    #error code not present
#endif

    /* Check the ATSupportedFlowProtocolParameter */
    if ((hdrmrlpc.in_config.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_NULL].prot_supported !=
         hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_NULL].prot_supported   ) 
        ||
        (hdrmrlpc.in_config.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_HDLC].prot_supported !=
         hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_HDLC].prot_supported   ) 
        ||
        (hdrmrlpc.in_config.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV4].prot_supported !=
         hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV4].prot_supported   ) 
        ||
        (hdrmrlpc.in_config.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV6].prot_supported !=
         hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV6].prot_supported   ) 
        ||
        (hdrmrlpc.in_config.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV4_IPV6].prot_supported !=
         hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_IPV4_IPV6].prot_supported   ) 
        ||
        (hdrmrlpc.in_config.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_ROHC].prot_supported !=
         hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_ROHC].prot_supported   )  
#ifdef FEATURE_EHRPD
      ||
      (hdrmrlpc.in_config.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_VSNP].prot_supported !=
       hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_VSNP].prot_supported   )  
#endif /* FEATURE_EHRPD */
       )
    {
      config_is_needed = TRUE;
    }

    if ((hdrmrlpc.in_config.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_NULL].prot_supported !=
         hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_NULL].prot_supported   ) 
        ||
        (hdrmrlpc.in_config.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_ROHC].prot_supported !=
         hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_ROHC].prot_supported   ) 
       )
    {
      config_is_needed = TRUE;
    }
  }
#endif

  /* If the current maximum and activated number of forward 
   * or reverse RLP flows are not equal to the desired number we can
   * support, the configuration is needed. 
   */
  if ((hdrmrlpc.in_config.max_rlp_flows.max_num_rlp_flows_fwd !=
       hdrmrlpc.desired.max_rlp_flows.max_num_rlp_flows_fwd   )
      ||
      (hdrmrlpc.in_config.max_rlp_flows.max_activated_rlp_flows_fwd !=
       hdrmrlpc.desired.max_rlp_flows.max_activated_rlp_flows_fwd   )
      ||
      (hdrmrlpc.in_config.max_rlp_flows.max_num_rlp_flows_rev !=
       hdrmrlpc.desired.max_rlp_flows.max_num_rlp_flows_rev   )
      ||
      (hdrmrlpc.in_config.max_rlp_flows.max_activated_rlp_flows_rev !=
       hdrmrlpc.desired.max_rlp_flows.max_activated_rlp_flows_rev   )
     )
  {
    config_is_needed = TRUE;
  }

  /* If the current maximum and open number of forward or reverse 
   * reservations are not equal to the desired number we can support,
   * the configuration is needed. 
   */
  if ((hdrmrlpc.in_config.max_reservations.max_num_rsv_fwd !=
       hdrmrlpc.desired.max_reservations.max_num_rsv_fwd   )
      ||
      (hdrmrlpc.in_config.max_reservations.max_num_open_rsv_fwd !=
       hdrmrlpc.desired.max_reservations.max_num_open_rsv_fwd   )
      ||
      (hdrmrlpc.in_config.max_reservations.max_num_rsv_rev !=
       hdrmrlpc.desired.max_reservations.max_num_rsv_rev   )
      ||
      (hdrmrlpc.in_config.max_reservations.max_num_open_rsv_rev !=
       hdrmrlpc.desired.max_reservations.max_num_open_rsv_rev   )
      )
  {
    config_is_needed = TRUE;
  }

  return config_is_needed;

} /* hdrmrlpc_config_is_needed() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_CONFIG_INIT

DESCRIPTION
  Initializes the in-config values to "negotiated"
                          
DEPENDENCIES
  Assumes we are only getting one config req.  Only initialize at start of
  AN INIT otherwise.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrmrlpc_config_init(void)
{

  /* Set the desired values for AT initiated ConfigReq attributes.
     For eHRPD after OTASP desired values can be changed that is 
     AT will start proposing eHRPD when there is valid credential.
     So we initialize desired values everytime before configuration 
     is initiated */
  hdrmrlpc_at_config_init();

  memset(&hdrmrlpc.config_result,0,sizeof(hdrmrlpc_config_req_result_type));

} /* hdrmrlpc_config_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_RLP_CNT_MAX_FWD_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the RLPCountMaxFwd attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_rlp_cnt_max_fwd_attrib_is_valid
(
  uint8 attrib_val
)
{
  boolean attrib_is_valid = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reserved Values in the standard: val < 3 and val > Nrlpmax */
  if ( (attrib_val <= 0x02) || (attrib_val > HDRMRLPC_N_MRLP_MAX) )
  {
    ERR("Invalid value %d for RLPCountMaxFwd attrib",attrib_val,0,0);
  }
  /* AT limitation: only supports default */
  else if (attrib_val > HDRMRLP_MAX_FWD_RLPS)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                   "AT supports a Maximum of %d fwd RLP flows",
                   HDRMRLP_MAX_FWD_RLPS);
  }
  else
  {
    attrib_is_valid = TRUE;
  }

  return attrib_is_valid;

} /* hdrmrlpc_rlp_cnt_max_fwd_attrib_is_valid() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_RLP_CNT_MAX_REV_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the RLPCountMaxRev attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_rlp_cnt_max_rev_attrib_is_valid
(
  uint8 attrib_val
)
{
  boolean attrib_is_valid = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reserved Values in the standard: val < 3 and val > Nrlpmax */
  if ( (attrib_val <= 0x02) || (attrib_val > HDRMRLPC_N_MRLP_MAX) )
  {
    ERR("Invalid value %d for RLPCountMaxRev attrib",attrib_val,0,0);
  }
  /* AT limitation: only supports default */
  else if (attrib_val > HDRMRLP_MAX_REV_RLPS)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                   "AT supports a Maximum of %d rev RLP flows",
                   HDRMRLP_MAX_REV_RLPS);
  }
  else
  {
    attrib_is_valid = TRUE;
  }

  return attrib_is_valid;

} /* hdrmrlpc_rlp_cnt_max_fwd_attrib_is_valid() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_TIMERS_ATTRIB_IS_VALID

DESCRIPTION
  This function assesses a given parameter set attribute to determine if
    it is acceptable or not.
    
DEPENDENCIES
  None

PARAMETERS
  timers_param - FlowNN Timers parameter attribute to evaluate
  max_abort_timer - The maximum abort timer value to check against
  
RETURN VALUE
  TRUE  - attribute is valid
  FALSE - attribute is not valid

SIDE EFFECTS
  None

===========================================================================*/

LOCAL boolean hdrmrlpc_timers_attrib_is_valid
(
  hdrmrlpc_flow_timers_type timers_param,
  uint8 max_abort_timer
)
{
  boolean is_valid = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* AbortTimer shall not have a value greater than MaxAbortTimer
     AbortTimer in units of ms, MaxAbortTimer in units of 100ms */
  if ( timers_param.abort_timer_val <= max_abort_timer * 100)
  {
    /* FlushTimer shall be less then or equal to that of the corresponding 
       AbortTimer*/
    if ( timers_param.flush_timer_val <= timers_param.abort_timer_val )
    {
      /* NakDelayTimer is defined only for MMPA */
      if (hdrstream_stream_is_negotiated(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
      {
        /* NakDelayTimer shall be less then or equal to that of the corresponding 
           AbortTimer*/
        if ( timers_param.nak_delay_timer_val <= timers_param.abort_timer_val )
        {
          is_valid = TRUE;
        }
        else
        {
          ERR("Invalid attrib val NT(%d) > AT(%d)",
              timers_param.nak_delay_timer_val,
              timers_param.abort_timer_val,0);
        }
      }
      else
      {
        is_valid = TRUE;
      }
    }
    else
    {
      ERR("Invalid attrib val FT(%d) > AT(%d)",
          timers_param.flush_timer_val,
          timers_param.abort_timer_val,0);
    }
  }
  else
  {
    ERR("Invalid attrib val AT(%d) > MAT(%d)",
        timers_param.abort_timer_val,
        max_abort_timer * 100,0);
  }

  /* _mpa_  what if AN sends new MaxAbortTimer param later in the config?
            check again? */

  return is_valid;

} /* hdrmrlpc_timers_attrib_is_valid() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_MAX_ABORT_TIMER_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the MaxAbortTimer attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_max_abort_timer_attrib_is_valid
(
  uint8 attrib_val
)
{
  boolean attrib_is_valid = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (attrib_val > 0x64)
  {
    ERR("Invalid value %d for MaxAbortTimer attrib",attrib_val,0,0);
  }
  else
  {
    attrib_is_valid = TRUE;
  }

  return attrib_is_valid;

} /* hdrmrlpc_max_abort_timer_attrib_is_valid() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_NAK_ENABLE_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNNakEnable attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_nak_enable_attrib_is_valid
(
  uint8 attrib_val
)
{
  /* According to the standard, values > 0x01 are reserved */
  return ( (attrib_val == 0x00) || (attrib_val == 0x01) );

} /* hdrmrlpc_nak_enable_attrib_is_valid() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_HIGHER_LAYER_PROT_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNHigherLayerProtocol attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 unreferenced variable attrib_val*/
LOCAL boolean hdrmrlpc_higher_layer_prot_attrib_is_valid
(
  uint8 attrib_val
)
{
  /* Currently accept everything */
  return TRUE;

} /* hdrmrlpc_higher_layer_prot_attrib_is_valid() */
/*lint -restore*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_PHYS_LAYER_NAK_ENABLE_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNPhysLayerNakEnable attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_phys_layer_nak_enable_attrib_is_valid
(
  uint8 attrib_val
)
{
  /* According to the standard, values > 0x01 are reserved */
  return ( (attrib_val == 0x00) || (attrib_val == 0x01) );

} /* hdrmrlpc_phys_layer_nak_enable_attrib_is_valid() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_PHYS_LAYER_NAK_ENABLE_ATTRIB_IS_PREFERRED

DESCRIPTION
  This function checks if the FlowNNPhysLayerNakEnable attribute is 
  preferred.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value

RETURN VALUE
  TRUE  - new value is preferred.
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_phys_layer_nak_enable_attrib_is_preferred
(
  uint8 attrib_val
)
{
   if (hdrstream_stream_is_negotiated(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) ||
       hdrstream_stream_is_negotiated(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
       hdrstream_stream_is_negotiated(HDRHAI_MULTIFLOW_PACKET_APP_SN))
   {
     /* For MPA/EMPA/MMPA AT supports Physical Layer NAK */
     return TRUE;
   }
   else
   {
     /* For DPA AT doesn't support Physical Layer NAK */
     return ( attrib_val == 0x00 );
   }

} /* hdrmrlpc_phys_layer_nak_enable_attrib_is_preferred() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_IDLE_STATE_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNIdleState attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_idle_state_attrib_is_valid
(
  uint8 attrib_val
)
{
  /* According to the standard, values > 0x02 are reserved */
  return ( (attrib_val == 0x00) || 
           (attrib_val == 0x01) ||
           (attrib_val == 0x02)   );

} /* hdrmrlpc_idle_state_attrib_is_valid() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_XMIT_ABORT_TIMER_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNTransmitAbortTimerRev attribute 
  is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_xmit_abort_timer_attrib_is_valid
(
  uint16 attrib_val
)
{
  /* According to the standard, values > 0x03E8 are reserved */
  return ( attrib_val <= HDRMRLPC_TRANSMIT_ABORT_TIMER_REV_MAX );

} /* hdrmrlpc_xmit_abort_timer_attrib_is_valid() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_DOS_ALLOWED_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNDataOverSignallingAllowedRev attribute 
  is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_dos_allowed_attrib_is_valid
(
  uint8 attrib_val
)
{
  /* According to the standard, values > 0x01 are reserved */
  return ( (attrib_val == 0x00) || (attrib_val == 0x01) );

} /* hdrmrlpc_dos_allowed_attrib_is_valid() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_FLOW_ID_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNIdentification attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  flow_id - The flow_id of a flow_nn attribute
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_flow_id_attrib_is_valid
(
  hdrmrlpc_flow_identification_type attrib_val,
  uint8 flow_id
)
{
  boolean attrib_is_valid = FALSE;
  uint8 total_len = 0;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reserved Values in the standard: val < 3 and val > Nrlpmax */
  if (attrib_val.is_active)
  {
    total_len = attrib_val.rlp_id_len + attrib_val.seq_len;

    /* " If the SequenceLength and RLPIDLength fields are included, then the 
         sum of SequenceLength and RLPIDLength shall be 22, 14. " */
    if ( (total_len == 22) || (total_len == 14) )
    {
      attrib_is_valid = TRUE;
    }
    else
    {

      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                     "Invalid val in flow id attrib (seq_len %d, rlp_id_len %d)",
                     attrib_val.seq_len, attrib_val.rlp_id_len);
      attrib_is_valid = FALSE;
    }
  }
  else
  {
    /* flow00 cannot be deactivated. */
    if (flow_id != 0)
    {
      attrib_is_valid = TRUE;
    }
  }

  return attrib_is_valid;

} /* hdrmrlpc_flow_id_attrib_is_valid() */

/*===========================================================================

FUNCTION HDRMRLPC_FLOW_SEQ_LEN_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNSequenceLengthFwd attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_flow_seq_len_attrib_is_valid
(
  hdrmrlpc_flow_sequence_length_type attrib_val
)
{
  boolean attrib_is_valid = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sender shall not set this field to any value other than 5, 13, 21 or 29 */
  if ((attrib_val.sar_seq_len == 5)  ||
      (attrib_val.sar_seq_len == 13) ||
      (attrib_val.sar_seq_len == 21) ||
      (attrib_val.sar_seq_len == 29))
  {
    /* SARSequenceLengthShort shall be less than or equal to SARSeqLength */
    if (attrib_val.sar_seq_len_short <= attrib_val.sar_seq_len)
    {
      attrib_is_valid = TRUE;
    }
    else 
    {
      ERR("Invalid attrib Short SAR seqlen (%d) > SAR seqlen (%d)",
            attrib_val.sar_seq_len_short, attrib_val.sar_seq_len, 0);
    }
  }
  else
  {
    ERR("Invalid SAR seqlen (%d)",
          attrib_val.sar_seq_len, 0, 0);
  }

  return attrib_is_valid;

} /* hdrmrlpc_flow_seq_len_attrib_is_valid() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_RESERVATION_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNReservation attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 unreferenced variable attrib_val*/
LOCAL boolean hdrmrlpc_reservation_attrib_is_valid
(
  hdrmrlpc_flow_reservation_type attrib_val
)
{
  /* Currently accept everything */
  return TRUE;

} /* hdrmrlpc_reservation_attrib_is_valid() */
/*lint -restore*/
/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_RESKK_QOS_RSP_IS_VALID

DESCRIPTION
  This function checks if the ReservationKKQosResponse attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 unreferenced variable attrib_val*/
LOCAL boolean hdrmrlpc_reskk_qos_rsp_is_valid
(
  hdrmrlpc_res_qos_profile_config_type attrib_val
)
{
  /* Currently accept everything */
  return TRUE;

} /* hdrmrlpc_reskk_qos_attrib_is_valid() */
/*lint -restore*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_FLOW_PROT_PARM_FWD_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNFlowProtocolParametersFwd attribute 
  is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_flow_prot_parm_fwd_attrib_is_valid
(
  hdrmrlpc_flow_prot_parm_fwd_type attrib_val
)
{
  hdrmrlpc_sup_prot_rohc_parm_value_type *rohc_ptr;
  boolean attrib_val_is_valid = FALSE;
/*------------------------------------------------------------------------*/

  if (hdrmrlpc.in_config.at_sup_flow_prot[attrib_val.id].prot_supported)
  {
    if (attrib_val.id == HDRMRLPC_FLOW_PROT_ID_ROHC)
    {
      rohc_ptr = 
        &hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_ROHC].value.rohc;

      if (attrib_val.parm.rohc.max_cid <= rohc_ptr->max_max_cid)
      {
        attrib_val_is_valid = TRUE;
      }
    }
    else
    {
      attrib_val_is_valid = TRUE;
    }
  }

  return attrib_val_is_valid;

} /* hdrmrlpc_flow_prot_parm_fwd_attrib_is_valid() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_FLOW_PROT_PARM_REV_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNFlowProtocolParametersRev attribute
  is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_flow_prot_parm_rev_attrib_is_valid
(
  hdrmrlpc_flow_prot_parm_rev_type attrib_val
)
{
  hdrmrlpc_sup_prot_rohc_parm_value_type *rohc_ptr;
  boolean attrib_val_is_valid = FALSE;
/*------------------------------------------------------------------------*/

  if (hdrmrlpc.in_config.at_sup_flow_prot[attrib_val.id].prot_supported)
  {
    if (attrib_val.id == HDRMRLPC_FLOW_PROT_ID_ROHC)
    {
      rohc_ptr = 
        &hdrmrlpc.desired.at_sup_flow_prot[HDRMRLPC_FLOW_PROT_ID_ROHC].value.rohc;

      if (attrib_val.parm.rohc.max_cid <= rohc_ptr->max_max_cid)
      {
        attrib_val_is_valid = TRUE;
      }
    }
    else
    {
      attrib_val_is_valid = TRUE;
    }
  }

  return attrib_val_is_valid;

} /* hdrmrlpc_flow_prot_parm_rev_attrib_is_valid() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_ROUTE_PROT_PARM_FWD_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNRouteProtocolParametersFwd attribute
  is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_route_prot_parm_fwd_attrib_is_valid
(
  hdrmrlpc_route_prot_parm_fwd_type attrib_val
)
{
  hdrmrlpc_sup_prot_rohc_parm_value_type *rohc_ptr;
  rohc_ptr = &hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_ROHC].value.rohc;
  
  if (attrib_val.parm.rohc.max_cid > rohc_ptr->max_max_cid)
  {
    return FALSE;
  }
  else
  {
    /* Currently accept everything */
    return TRUE;
  }

} /* hdrmrlpc_route_prot_parm_fwd_attrib_is_valid() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_ROUTE_PROT_PARM_REV_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the FlowNNRouteProtocolParametersRev attribute 
  is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_route_prot_parm_rev_attrib_is_valid
(
  hdrmrlpc_route_prot_parm_rev_type attrib_val
)
{
  hdrmrlpc_sup_prot_rohc_parm_value_type *rohc_ptr;
  rohc_ptr = &hdrmrlpc.desired.at_sup_route_prot[HDRMRLPC_ROUTE_PROT_ID_ROHC].value.rohc;
  
  if (attrib_val.parm.rohc.max_cid > rohc_ptr->max_max_cid)
  {
    return FALSE;
  }
  else
  {
    /* Currently accept everything */
    return TRUE;
  }

} /* hdrmrlpc_route_prot_parm_rev_attrib_is_valid() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_SUP_HIGH_LAYER_PROT_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the SupportedHigherLayerProtocol attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 unreferenced variable attrib_val*/
LOCAL boolean hdrmrlpc_sup_high_layer_prot_attrib_is_valid
(
  hdrmrlpc_sup_higher_layer_prot_type attrib_val
)
{
  /* Currently accept everything */
  return TRUE;

} /* hdrmrlpc_sup_high_layer_prot_attrib_is_valid() */
/*lint -restore*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_AN_SUP_QOS_PROFILE_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the ANSupportedQoSProfile attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 unreferenced variable attrib_val*/
LOCAL boolean hdrmrlpc_an_sup_qos_profile_attrib_is_valid
(
  hdrmrlpc_supported_qos_profiles attrib_val
)
{
  /* Currently accept everything */
  return TRUE;

} /* hdrmrlpc_an_sup_qos_profile_attrib_is_valid() */
/*lint -restore*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_MAX_RESERVATIONS_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if the MaxReservations attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_max_reservations_attrib_is_valid
(
  hdrmrlpc_max_reservations_type attrib_val
)
{
  boolean attrib_is_valid = TRUE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The max_num_reservations_fwd and _rev are of uint8, so
   * they satisfy the requirements specified in the standard:
   * 0 <= MaxNumReservationsFwd <= 0xFF 
   * 0 <= MaxNumReservationsRev <= 0xFF 
   *
   * But our MSM only supports max 16 forward and reverse
   * reservations. So we accept if the values are less or equal;
   * Otherwise we reject and fall back to the default / previous
   * negotiated value.
   */
  if ((attrib_val.max_num_rsv_fwd > HDRMRLP_MAX_FWD_RESERVATIONS) ||
      (attrib_val.max_num_rsv_rev > HDRMRLP_MAX_REV_RESERVATIONS) ||
      (attrib_val.max_num_open_rsv_fwd > attrib_val.max_num_rsv_fwd) ||
      (attrib_val.max_num_open_rsv_rev > attrib_val.max_num_rsv_rev)   )
  {
    attrib_is_valid = FALSE;
  }

  return attrib_is_valid;

} /* hdrmrlpc_max_reservations_attrib_is_valid() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_MAX_ABORT_TIMER_ATTRIB

DESCRIPTION
  This function processes the max abort timer attribute of a Configuration 
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_max_abort_timer_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset
)
{
 uint8  attrib_val; /* A suggested value for the attribute */
 uint16 attrib_val_offset; /* Bit offset of current attribute value from the 
                              beginning of the current attribute */

 hdrmrlpc_config_req_result_type *c_ptr; /* Convenience pointer */
/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

 attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;
 c_ptr = &hdrmrlpc.config_result;
 
 if (c_ptr->max_abort_timer_attrib.is_accepted)
 {
   return;
 }

 /* Flag that the AN requested this attribute */
 c_ptr->max_abort_timer_attrib.requested = TRUE;

 /* Loop through all the values given for this attribute. */
 while ((attrib_val_offset + HDRMRLPC_SIMPLE_ATTRIB_SIZE) <=
        attrib_len)
 {
   /* There is at least one more attribute value in this attribute */

   if ( HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( item_ptr, 
          &attrib_val, attrib_offset, attrib_val_offset ) != E_SUCCESS )
   {
     ERR("Error extracting attrib_val from ConfigReq Msg", 0, 0, 0 );
     /* If we can't get this attribute value, there is no reason to
      * try to get more values for this attribute.
      */
     break;
   }

   if ( hdrmrlpc_max_abort_timer_attrib_is_valid(attrib_val) )
   {
     hdrmrlpc.in_config.max_abort_timer = attrib_val;

     c_ptr->max_abort_timer_attrib.value = attrib_val;
     c_ptr->max_abort_timer_attrib.is_accepted = TRUE;
   }

   attrib_val_offset += HDRMRLPC_SIMPLE_ATTRIB_SIZE;

   /* If we've accepted a value from this attribute list, no need to
      continue and look at the other ones (if any) */

   if (c_ptr->max_abort_timer_attrib.is_accepted)
   {
     break;
   }
 }

} /* hdrmrlpc_unpack_creq_max_abort_timer_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_NAK_ENABLE_ATTRIB

DESCRIPTION
  This function processes the nak enabled attribute of a Configuration 
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rlp    - TRUE: fwd RLP, FALSE: rev RLP
  flow_id       - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_nak_enable_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  boolean is_fwd_rlp,
  uint8 flow_id
)
{
 uint8  attrib_val; /* A suggested value for the attribute */
 uint16 attrib_val_offset; /* Bit offset of current attribute value from the 
                              beginning of the current attribute */

 hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */
 hdrmrlpc_flow_nn_config_type *f_ptr; /* Convenience pointer */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

 attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

 if (is_fwd_rlp)
 {
   c_ptr = &hdrmrlpc.config_result.flow_nn_fwd_attrib[flow_id].nak_enable;
   f_ptr = &hdrmrlpc.in_config.fwd_rlp_config_info[flow_id];
 }
 else
 {
   c_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[flow_id].nak_enable;
   f_ptr = &hdrmrlpc.in_config.rev_rlp_config_info[flow_id];
 }

 if (c_ptr->is_accepted)
 {
   return;
 }

 /* Flag that the AN requested this attribute */
 c_ptr->requested = TRUE;

 /* Loop through all the values given for this attribute. */
 while ((attrib_val_offset + HDRMRLPC_SIMPLE_ATTRIB_SIZE) <=
        attrib_len)
 {
   /* There is at least one more attribute value in this attribute */

   if ( HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( item_ptr, 
          &attrib_val, attrib_offset, attrib_val_offset ) != E_SUCCESS )
   {
     ERR("Error extracting attrib_val from ConfigReq Msg", 0, 0, 0 );
     /* If we can't get this attribute value, there is no reason to
      * try to get more values for this attribute.
      */
     break;
   }

   if ( hdrmrlpc_nak_enable_attrib_is_valid(attrib_val) )
   {
     c_ptr->is_accepted = TRUE;
     c_ptr->value = attrib_val;
     f_ptr->nak_enabled = attrib_val;
   }
   else
   {
     ERR("Invalid value (%d) for NakEnableFlow%d %d attrib",
          attrib_val,flow_id,is_fwd_rlp);
   }

   attrib_val_offset += HDRMRLPC_SIMPLE_ATTRIB_SIZE;

   /* If we've accepted a value from this attribute list, no need to
      continue and look at the other ones (if any) */

   if (c_ptr->is_accepted)
   {
     break;
   }
 }

 
} /* hdrmrlpc_unpack_creq_nak_enable_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_HIGHER_LAYER_PROT_ATTRIB

DESCRIPTION
  This function processes the higher layer protocol attribute of a 
  Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rlp    - TRUE: fwd RLP, FALSE: rev RLP
  flow_id       - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_high_layer_prot_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  boolean is_fwd_rlp,
  uint8 flow_id
)
{
  uint8  attrib_val; /* A suggested value for the attribute */
  uint16 attrib_val_offset; /* Bit offset of current attribute value from the 
                               beginning of the current attribute */

  hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */
  hdrmrlpc_flow_nn_config_type *f_ptr; /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  if (is_fwd_rlp)
  {
    c_ptr = &hdrmrlpc.config_result.flow_nn_fwd_attrib[flow_id].higher_layer_prot;
    f_ptr = &hdrmrlpc.in_config.fwd_rlp_config_info[flow_id];
  }
  else
  {
    c_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[flow_id].higher_layer_prot;
    f_ptr = &hdrmrlpc.in_config.rev_rlp_config_info[flow_id];
  }

  if (c_ptr->is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_SIMPLE_ATTRIB_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    if ( HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( item_ptr, 
           &attrib_val, attrib_offset, attrib_val_offset ) != E_SUCCESS )
    {
      ERR("Error extracting attrib_val from ConfigReq Msg", 0, 0, 0 );
      /* If we can't get this attribute value, there is no reason to
       * try to get more values for this attribute.
       */
      break;
    }

    if ( hdrmrlpc_higher_layer_prot_attrib_is_valid(attrib_val) )
    {
        c_ptr->is_accepted = TRUE;

        c_ptr->value = attrib_val;
        f_ptr->higher_layer_protocol = attrib_val;
    }

    attrib_val_offset += HDRMRLPC_SIMPLE_ATTRIB_SIZE;

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (c_ptr->is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_high_layer_prot_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_PHYS_LAYER_NAK_EN_ATTRIB

DESCRIPTION
  This function processes the physical layer Nak enabled attribute of a 
  Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  flow_id       - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_phys_layer_nak_en_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  uint8 flow_id
)
{
  uint8  attrib_val; /* A suggested value for the attribute */
  uint16 attrib_val_offset; /* Bit offset of current attribute value from the 
                               beginning of the current attribute */

  hdrmrlpc_flow_nn_rev_config_attrib_result_type *c_ptr; /* Convenience ptr */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;
  c_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[flow_id];

  if (c_ptr->phys_layer_nak_enable.is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->phys_layer_nak_enable.requested = TRUE;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_SIMPLE_ATTRIB_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    if ( HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( item_ptr, 
           &attrib_val, attrib_offset, attrib_val_offset ) != E_SUCCESS )
    {
      ERR("Error extracting attrib_val from ConfigReq Msg", 0, 0, 0 );
      /* If we can't get this attribute value, there is no reason to
       * try to get more values for this attribute.
       */
      break;
    }

    if ( hdrmrlpc_phys_layer_nak_enable_attrib_is_valid(attrib_val) &&
         hdrmrlpc_phys_layer_nak_enable_attrib_is_preferred(attrib_val) )
    {
      hdrmrlpc.in_config.rev_rlp_config_info[flow_id].phys_link_nak_enabled_rev = 
        attrib_val;

      c_ptr->phys_layer_nak_enable.value = attrib_val;
      c_ptr->phys_layer_nak_enable.is_accepted = TRUE;

    }
    else
    {
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH,
                      "Unsupported value (%d) for Flow%dPhysLayerNakEnable Rev attrib",
                      attrib_val,flow_id);
    }

    attrib_val_offset += HDRMRLPC_SIMPLE_ATTRIB_SIZE;

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (c_ptr->phys_layer_nak_enable.is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_phys_layer_nak_en_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_RESKK_IDLE_STATE_ATTRIB

DESCRIPTION
  This function processes the idle state attribute of a Configuration 
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rsv    - TRUE: fwd Res, FALSE: rev Res
  rsv_label     - Reservation Label

RETURN VALUE
  None
   
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_reskk_idle_state_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  boolean is_fwd_rsv,
  uint8 rsv_label
)
{
  uint8  attrib_val; /* A suggested value for the attribute */
  uint16 attrib_val_offset; /* Bit offset of current attribute value from the 
                               beginning of the current attribute */

  hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */
  hdrmrlpc_reservation_kk_config_type *r_ptr; /* Convenience pointer */

  uint8 rsv_idx = 0; /* Reservation Index for this label (rsv_label) */
 
 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  if (is_fwd_rsv &&
      (hdrmrlpc_get_cfg_rl_idx(rsv_label, HDRMRLPC_FWD_RLP, &rsv_idx) == E_SUCCESS) )
  {
    c_ptr = &hdrmrlpc.config_result.res_kk_fwd_attrib[rsv_idx].idle_state;
    r_ptr = &hdrmrlpc.in_config.fwd_res_info[rsv_idx];
  }
  else if (!is_fwd_rsv && 
           (hdrmrlpc_get_cfg_rl_idx(rsv_label, HDRMRLPC_REV_RLP, &rsv_idx) == E_SUCCESS) )
  {
    c_ptr = &hdrmrlpc.config_result.res_kk_rev_attrib[rsv_idx].idle_state;
    r_ptr = &hdrmrlpc.in_config.rev_res_info[rsv_idx];
  }
  else if (rsv_label == HDRMRLPC_FLOW_00_RESERVATION_LABEL_FWD_DEF )
  {
    /* Treat reservation label 0xff differently becasue default flow (0x00) won't 
     * change state based on this attribute. Allowing the AN to negotiate 
     * attribute even there was not QosRequest for this reservation lable
     */
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "Res0x%xIdleState %d", rsv_label, is_fwd_rsv );

    if (is_fwd_rsv)
    {
      c_ptr = &hdrmrlpc.config_result.res_kk_fwd_attrib_ff.idle_state;
      r_ptr = &hdrmrlpc.in_config.fwd_res_info_ff;
      r_ptr->rsv_label = rsv_label;
    }
    else
    {
      c_ptr = &hdrmrlpc.config_result.res_kk_rev_attrib_ff.idle_state;
      r_ptr = &hdrmrlpc.in_config.rev_res_info_ff;
      r_ptr->rsv_label = rsv_label;
    }
  }
  else
  {
    /* This is based on the discussion with QoS team: AN should not 
     * send a ResvKKIdleState attribute if AT hasn't send the QoSRequest
     * for this reservation label unless reservation label is equal to
     * 0xFF (HDRMRLPC_FLOW_00_RESERVATION_LABEL_FWD_DEF or 
     * HDRMRLPC_FLOW_00_RESERVATION_LABEL_REV_DEF)
     * If this happens, we can just ignore. 
     */
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                   "Res0x%xIdleState %d: Can't find rsv label, Ignore!",
                   rsv_label, is_fwd_rsv );
    return;
  }
  
  if (c_ptr->is_accepted)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                   "Res0x%xIdleState %d: Already accepted", 
                   rsv_label, is_fwd_rsv );
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_SIMPLE_ATTRIB_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    if ( HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( item_ptr, 
           &attrib_val, attrib_offset, attrib_val_offset ) != E_SUCCESS )
    {
      ERR("Error extracting attrib_val from ConfigReq Msg", 0, 0, 0 );
      /* If we can't get this attribute value, there is no reason to
       * try to get more values for this attribute.
       */
      break;
    }

    if ( hdrmrlpc_idle_state_attrib_is_valid(attrib_val) )
    {
      r_ptr->idle_state = attrib_val;

      c_ptr->value = attrib_val;
      c_ptr->is_accepted = TRUE;
    }
    else
    {
      ERR("Invalid value (%d) for Reservation%dIdleState %d attrib",
           attrib_val,rsv_label,is_fwd_rsv);
    }
    
    attrib_val_offset += HDRMRLPC_SIMPLE_ATTRIB_SIZE;

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (c_ptr->is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_reskk_idle_state_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_XMIT_ABORT_TIMER_ATTRIB

DESCRIPTION
  This function processes the Transmit Abort Timer attribute of a 
  Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rlp    - TRUE: fwd RLP, FALSE: rev RLP
  flow_id       - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_xmit_abort_timer_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  boolean is_fwd_rlp,
  uint8 flow_id
)
{
 uint16 attrib_val; /* A suggested value for the attribute */
 uint16 attrib_val_offset; /* Bit offset of current attribute value from the 
                              beginning of the current attribute */

 hdrmrlpc_config_attrib_16_result_type *c_ptr; /* Convenience pointer */
 hdrmrlpc_flow_nn_config_type *f_ptr; /* Convenience pointer */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

 attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

 if (!is_fwd_rlp)
 {
   ERR("XmitAbortTimerRev is AN init'ed only", 0, 0, 0);
   return;
 }
 else
 {
   c_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[flow_id].xmit_abort_timer;
   f_ptr = &hdrmrlpc.in_config.rev_rlp_config_info[flow_id];
 }

 if (c_ptr->is_accepted)
 {
   return;
 }

 /* Flag that the AN requested this attribute */
 c_ptr->requested = TRUE;

 /* Loop through all the values given for this attribute. */
 while ((attrib_val_offset + 
         HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_ATTRIB_MIN_VAL_SIZE) 
        <=
        attrib_len)
 {
   /* There is at least one more attribute value in this attribute */
   if ( HDRMRLPC_UNPACK_CON_REQ_XMIT_ABORT_TIMER ( item_ptr, 
          &attrib_val, attrib_offset, attrib_val_offset ) != E_SUCCESS )
   {
     ERR("Error extracting attrib_val from ConfigReq Msg", 0, 0, 0 );
     /* If we can't get this attribute value, there is no reason to
      * try to get more values for this attribute.
      */
     break;
   }

   if ( hdrmrlpc_xmit_abort_timer_attrib_is_valid(attrib_val) )
   {
     c_ptr->is_accepted = TRUE;
     c_ptr->value = attrib_val;
     f_ptr->xmit_abort_timer = attrib_val;
   }
   else
   {
     ERR("Invalid value (%d) for TransmitAbortTimerRev%d %d attrib",
          attrib_val,flow_id,is_fwd_rlp);
   }

   attrib_val_offset += HDRMRLPC_SIMPLE_ATTRIB_16_SIZE;

   /* If we've accepted a value from this attribute list, no need to
      continue and look at the other ones (if any) */

   if (c_ptr->is_accepted)
   {
     break;
   }
 }
} /* hdrmrlpc_unpack_creq_xmit_abort_timer_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_DOS_ALLOWED_ATTRIB

DESCRIPTION
  This function processes the Data Over Signalling allowed attribute of a 
  Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rlp    - TRUE: fwd RLP, FALSE: rev RLP
  flow_id       - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_dos_allowed_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  boolean is_fwd_rlp,
  uint8 flow_id
)
{
 uint8  attrib_val; /* A suggested value for the attribute */
 uint16 attrib_val_offset; /* Bit offset of current attribute value from the 
                              beginning of the current attribute */

 hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */
 hdrmrlpc_flow_nn_config_type *f_ptr; /* Convenience pointer */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

 attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

 if (!is_fwd_rlp)
 {
   ERR("DataOverSignallingAllowedRev is AN inited only", 0, 0, 0);
   return;
 }
 else
 {
   c_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[flow_id].dos_allowed;
   f_ptr = &hdrmrlpc.in_config.rev_rlp_config_info[flow_id];
 }

 if (c_ptr->is_accepted)
 {
   return;
 }

 /* Flag that the AN requested this attribute */
 c_ptr->requested = TRUE;

 /* Loop through all the values given for this attribute. */
 while ((attrib_val_offset + HDRMRLPC_SIMPLE_ATTRIB_SIZE) <=
        attrib_len)
 {
   /* There is at least one more attribute value in this attribute */

   if ( HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( item_ptr, 
          &attrib_val, attrib_offset, attrib_val_offset ) != E_SUCCESS )
   {
     ERR("Error extracting attrib_val from ConfigReq Msg", 0, 0, 0 );
     /* If we can't get this attribute value, there is no reason to
      * try to get more values for this attribute.
      */
     break;
   }

   if ( hdrmrlpc_dos_allowed_attrib_is_valid(attrib_val) )
   {
     c_ptr->is_accepted = TRUE;
     c_ptr->value = attrib_val;
     f_ptr->dos_allowed = attrib_val;
   }
   else
   {
     ERR("Invalid value (%d) for DataOverSignallingRev%d %d attrib",
          attrib_val,flow_id,is_fwd_rlp);
   }

   attrib_val_offset += HDRMRLPC_SIMPLE_ATTRIB_SIZE;

   /* If we've accepted a value from this attribute list, no need to
      continue and look at the other ones (if any) */

   if (c_ptr->is_accepted)
   {
     break;
   }
 }

} /* hdrmrlpc_unpack_creq_dos_allowed_attrib() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_SIMPLE_ATTRIB_IS_VALID

DESCRIPTION
  This function checks if a simple attribute is valid.
  
DEPENDENCIES
  None

PARAMETERS
  attrib_val - The attribute's value
  
RETURN VALUE
  TRUE  - Attribute is valid 
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrmrlpc_simple_attrib_is_valid
(
  hdrmrlpc_attrib_name_enum_type attrib_name,
  uint16                         attrib_index,
  uint8                          attrib_val
)
{
  boolean is_valid;

  switch (attrib_name)
  {
    case HDRMRLPC_ATTRIB_FLOW_PROT_SDU_FWD:
    case HDRMRLPC_ATTRIB_FLOW_PROT_SDU_REV:
    case HDRMRLPC_ATTRIB_DATA_UNIT_FWD:
    case HDRMRLPC_ATTRIB_DATA_UNIT_REV:
    case HDRMRLPC_ATTRIB_ROUTE_PROT_SDU_FWD:
    case HDRMRLPC_ATTRIB_ROUTE_PROT_SDU_REV:
    case HDRMRLPC_ATTRIB_SIM_DEL_BOTH_ROUTES_FWD:
    case HDRMRLPC_ATTRIB_OOO_DEL_ROUTE_PROT_FWD:

      is_valid = ((attrib_val == 0x00) || (attrib_val == 0x01));
      break;


    case HDRMRLPC_ATTRIB_SIMPLE_SEQUENCE_LENGTH_FWD:
    case HDRMRLPC_ATTRIB_SEQUENCE_LENGTH_REV:
      is_valid = ((attrib_val == 0x00) || 
                  (attrib_val == 0x01) ||
                  (attrib_val == 0x02)   );
      break;

    case HDRMRLPC_ATTRIB_ACTIVATED_FWD:
    case HDRMRLPC_ATTRIB_ACTIVATED_REV:
#if defined(FEATURE_HDR_EMPA) || defined (FEATURE_HDR_REVB)
      switch(attrib_index)
      {
      case 0x00:
        is_valid = (attrib_val == 0x01);
        break;

      case 0x01:
        is_valid = (attrib_val == 0x00);
        break;

      default:
        is_valid = TRUE;
      }
#else
      is_valid = FALSE;
#endif
      break;

    case HDRMRLPC_ATTRIB_TWO_ROUTES_SUPPORTED:
      /* if RSP is enabled, AN can propose two_routes_supported
         as 0x00 or 0x00, otherwise, only 0x00 is valid */      
#ifdef FEATURE_HDR_SUPPORT_RSP
      #error code not present
#else
      is_valid = (attrib_val == 0x00);
#endif
      break;

    case HDRMRLPC_ATTRIB_AT_NAK_DELAY_SUPPORTED:
    case HDRMRLPC_ATTRIB_PPP_FREE_AUTHENTICATION_SUPPORT:
      is_valid = (attrib_val == 0x00);
      break;

    case HDRMRLPC_ATTRIB_FLOW_NAK_DELAY_TIME_FWD:
      if (hdrmrlpc.current.at_nak_delay_supported)
      {
        is_valid = (attrib_val >= 0x01);
      }
      else
      {
        is_valid = FALSE;
      }

      break;

    default:
      /* Other attributes don't call this function */
      HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                     "Attrib %d should not call simple_attrib_is_valid()",
                     attrib_name);
      is_valid = FALSE;
      break;
  }

  return is_valid;

} /* hdrmrlpc_simple_attrib_is_valid() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_SIMPLE_ATTRIB

DESCRIPTION
  This function processes the simple attribute of a 
  Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rlp    - TRUE: fwd RLP, FALSE: rev RLP
  flow_id       - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_simple_attrib
( 
  dsm_item_type   *item_ptr,
  uint16          attrib_len, 
  uint16          attrib_offset,
  uint16          attrib_index,
  hdrmrlpc_attrib_name_enum_type      attrib_name,
  hdrmrlpc_config_attrib_result_type  *c_ptr,
  uint8           *value
)
{
  uint8  attrib_val; /* A suggested value for the attribute */
  uint16 attrib_val_offset; /* Bit offset of current attribute value from the 
                               beginning of the current attribute */
 
  /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  if (c_ptr->is_accepted)
  {
    return;
  }
 
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;
 
  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_SIMPLE_ATTRIB_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */
 
    if ( HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( item_ptr, 
           &attrib_val, attrib_offset, attrib_val_offset ) != E_SUCCESS )
    {
      ERR("Error extracting attrib_val from ConfigReq Msg", 0, 0, 0 );
      /* If we can't get this attribute value, there is no reason to
       * try to get more values for this attribute.
       */
      break;
    }
 
    if ( hdrmrlpc_simple_attrib_is_valid(attrib_name, attrib_index, attrib_val) )
    {
      c_ptr->is_accepted = TRUE;
      c_ptr->value = attrib_val;
      *value = attrib_val;
    }
    else
    {
      ERR("Invalid value (%d) for attrib %d", attrib_val, attrib_name, 0);
    }
 
    attrib_val_offset += HDRMRLPC_SIMPLE_ATTRIB_SIZE;
 
    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */
 
    if (c_ptr->is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_simple_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_FLOW_ID_ATTRIB

DESCRIPTION
  This function processes the flow identification attribute of a Configuration 
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rlp    - TRUE: fwd RLP, FALSE: rev RLP
  flow_id       - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_flow_id_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  boolean is_fwd_rlp,
  uint8 flow_id
)
{
  uint16  attrib_val_offset; /* Bit offset of current attribute value from the 
                               beginning of the current attribute */
  struct
  {
    uint8 value_id;
    hdrmrlpc_flow_identification_type value;
  } curr_flow_id; 
  /* Current Attribute Value and Value_ID*/

  int err; /* sum of errno return vals (E_SUCCESS is zero) */

  hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */
  hdrmrlpc_flow_nn_config_type *f_ptr; /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  if (is_fwd_rlp)
  {
    c_ptr = &hdrmrlpc.config_result.flow_nn_fwd_attrib[flow_id].identification;
    f_ptr = &hdrmrlpc.in_config.fwd_rlp_config_info[flow_id];
  }
  else
  {
    c_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[flow_id].identification;
    f_ptr = &hdrmrlpc.in_config.rev_rlp_config_info[flow_id];
  }
  
  if (c_ptr->is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  memset((void*)&curr_flow_id.value, 0, 
         sizeof(hdrmrlpc_flow_identification_type));

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_ID_ATTRIB_MIN_VAL_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_flow_id.value_id, 
            attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_ID_ACTIVE(
             item_ptr, &curr_flow_id.value.is_active,
             attrib_offset, attrib_val_offset );

    if (curr_flow_id.value.is_active)
    {   
      err |= HDRMRLPC_UNPACK_CON_REQ_ID_SEQ_LEN(
        item_ptr, &curr_flow_id.value.seq_len,
        attrib_offset, attrib_val_offset );

      err |= HDRMRLPC_UNPACK_CON_REQ_ID_RLPID_LEN(
        item_ptr, &curr_flow_id.value.rlp_id_len,
        attrib_offset, attrib_val_offset );

      err |= HDRMRLPC_UNPACK_CON_REQ_ID_RLPID(
        item_ptr, &curr_flow_id.value.rlp_id,
        attrib_offset, attrib_val_offset, curr_flow_id.value.rlp_id_len );
      
      attrib_val_offset += HDRMRLPC_ID_ATTRIB_MAX_VAL_SIZE;
    } 
    else
    {
      attrib_val_offset += HDRMRLPC_ID_ATTRIB_MIN_VAL_SIZE;
    }

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }

    if ( hdrmrlpc_flow_id_attrib_is_valid(curr_flow_id.value, flow_id) )
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      f_ptr->flow_id = curr_flow_id.value;
#else
      memcpy((void*)&f_ptr->flow_id,
             (void*)&curr_flow_id.value,
             sizeof(hdrmrlpc_flow_identification_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
      c_ptr->is_accepted = TRUE;
      c_ptr->value = curr_flow_id.value_id;
    }

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */
    if (c_ptr->is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_flow_id_attrib() */      

/* <EJECT> */
/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_SEQ_LEN_FWD_ATTRIB

DESCRIPTION
  This function processes the flow seq length fwd attribute of a Configuration 
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rlp    - TRUE: fwd RLP, FALSE: rev RLP
  flow_id       - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_seq_len_fwd_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  boolean is_fwd_rlp,
  uint8 flow_id
)
{
  uint16  attrib_val_offset; /* Bit offset of current attribute value from the 
                                beginning of the current attribute */
  struct
  {
    uint8 value_id;
    hdrmrlpc_flow_sequence_length_type value;
  } curr_flow_id; 
  /* Current Attribute Value and Value_ID*/

  int err; /* sum of errno return vals (E_SUCCESS is zero) */

  hdrmrlpc_config_attrib_result_type *c_ptr = NULL; /* Convenience pointer */
  hdrmrlpc_flow_nn_config_type *f_ptr = NULL;       /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  ASSERT (is_fwd_rlp == TRUE);

  if (is_fwd_rlp)
  {
    c_ptr = &hdrmrlpc.config_result.flow_nn_fwd_attrib[flow_id].complex_seq_len;
    f_ptr = &hdrmrlpc.in_config.fwd_rlp_config_info[flow_id];
  }
  
  if (c_ptr->is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  memset((void*)&curr_flow_id.value, 0, 
         sizeof(hdrmrlpc_flow_sequence_length_type));

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_SEQ_LEN_ATTRIB_VAL_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_flow_id.value_id, 
            attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_SAR_SEQ_LEN(
             item_ptr, &curr_flow_id.value.sar_seq_len,
             attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_SAR_SEQ_LEN_SHORT(
             item_ptr, &curr_flow_id.value.sar_seq_len_short,
             attrib_offset, attrib_val_offset );

    attrib_val_offset += HDRMRLPC_SEQ_LEN_ATTRIB_VAL_SIZE;

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }

    if ( hdrmrlpc_flow_seq_len_attrib_is_valid(curr_flow_id.value) )
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      f_ptr->complex_seq_len = curr_flow_id.value;
#else
      memcpy((void*)&f_ptr->complex_seq_len,
             (void*)&curr_flow_id.value,
             sizeof(hdrmrlpc_flow_sequence_length_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
      c_ptr->is_accepted = TRUE;
      c_ptr->value = curr_flow_id.value_id;
    }

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */
    if (c_ptr->is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_seq_len_fwd_attrib() */      

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_TIMERS_ATTRIB

DESCRIPTION
  This function processes the flow timers attribute of a Configuration 
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rlp    - TRUE: fwd RLP, FALSE: rev RLP
  flow_id   - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_timers_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  boolean is_fwd_rlp,
  uint8 flow_id
)
{
  uint16  attrib_val_offset; /* Bit offset of current attribute value from the 
                               beginning of the current attribute */
  uint16  attrib_val_size;

  struct
  {
    uint8 value_id;
    hdrmrlpc_flow_timers_type value;
  } curr_flow_timers; 
  /* Current Attribute Value and Value_ID*/

  int err; /* sum of errno return vals (E_SUCCESS is zero) */

  hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */
  hdrmrlpc_flow_nn_config_type *f_ptr; /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (is_fwd_rlp)
  {
    c_ptr = &hdrmrlpc.config_result.flow_nn_fwd_attrib[flow_id].timers;
    f_ptr = &hdrmrlpc.in_config.fwd_rlp_config_info[flow_id];
  }
  else
  {
    c_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[flow_id].timers;
    f_ptr = &hdrmrlpc.in_config.rev_rlp_config_info[flow_id];
  }
  
  if (c_ptr->is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;
  memset((void*)&curr_flow_timers.value, 0, 
         sizeof(hdrmrlpc_flow_timers_type));

  /* MMPA FlowNNTimersFwd attribute has an additional field 
    (NakdelayTimer) hence the attrib value size is different */
  if ((hdrstream_get_negotiated_snpktapp() == 
       HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) && 
       (is_fwd_rlp))
  {
    attrib_val_size = HDRMRLPC_MMPA_TIMERS_FWD_ATTRIB_VAL_SIZE;
  }
  else
  {
    attrib_val_size = HDRMRLPC_TIMERS_ATTRIB_VAL_SIZE;
  }

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + attrib_val_size) <= attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_flow_timers.value_id, 
            attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_TIMERS_ABORT(
             item_ptr, &curr_flow_timers.value.abort_timer_val,
             attrib_offset, attrib_val_offset );
    
    err |= HDRMRLPC_UNPACK_CON_REQ_TIMERS_FLUSH(
             item_ptr, &curr_flow_timers.value.flush_timer_val,
             attrib_offset, attrib_val_offset );

    if ((hdrstream_get_negotiated_snpktapp() == 
         HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) && 
        (is_fwd_rlp))
    {
      err |= HDRMRLPC_UNPACK_CON_REQ_TIMERS_NAK_DELAY(
               item_ptr, &curr_flow_timers.value.nak_delay_timer_val,
               attrib_offset, attrib_val_offset );
    }

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }

#ifdef FEATURE_MEMCPY_REMOVAL
    f_ptr->flow_timer = curr_flow_timers.value;
#else
    memcpy((void*)&f_ptr->flow_timer,
           (void*)&curr_flow_timers.value,
           sizeof(hdrmrlpc_flow_timers_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
  
    c_ptr->is_accepted = TRUE;
    c_ptr->value = curr_flow_timers.value_id;
    
    attrib_val_offset += attrib_val_size;

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (c_ptr->is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_timers_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_RESERVATION_ATTRIB

DESCRIPTION
  This function processes the flow reservation attribute of a Configuration 
  Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rlp    - TRUE: fwd RLP, FALSE: rev RLP
  flow_id   - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_reservation_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  boolean is_fwd_rlp,
  uint8 flow_id
)
{
  uint16  attrib_val_offset; 
    /* Bit offset of current attribute value from the beginning of the current 
       attribute */
                               
  struct
  {
    uint8 value_id;
    hdrmrlpc_flow_reservation_type value;
  } curr_flow_rsv; 
    /* Current Attribute Value and Value_ID*/

  int err; 
    /* sum of errno return vals (E_SUCCESS is zero) */

  uint8 i; 
    /* iterator */

  hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */
  hdrmrlpc_flow_nn_config_type *f_ptr; /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (is_fwd_rlp)
  {
    c_ptr = &hdrmrlpc.config_result.flow_nn_fwd_attrib[flow_id].reservation;
    f_ptr = &hdrmrlpc.in_config.fwd_rlp_config_info[flow_id];
  }
  else
  {
    c_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[flow_id].reservation;
    f_ptr = &hdrmrlpc.in_config.rev_rlp_config_info[flow_id];
  }
  
  if (c_ptr->is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  memset((void*)&curr_flow_rsv.value, 0, 
         sizeof(hdrmrlpc_flow_reservation_type));

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_RES_ATTRIB_MIN_VAL_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_flow_rsv.value_id, 
            attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_RSV_RES_COUNT(
             item_ptr, &curr_flow_rsv.value.rsv_count,
             attrib_offset, attrib_val_offset );
    
    /* The AT supports a maximum of 20 reservation labels. (255 max in std) */
    if (curr_flow_rsv.value.rsv_count > HDRMRLPC_N_RES_LABELS_MAX)
    {
      HDR_MSG_PROT_4(MSG_LEGACY_ERROR,
                     "Flow%dReservation %d: AT cannot support %d rsv (max: %d)",
                     flow_id, is_fwd_rlp,
                     curr_flow_rsv.value.rsv_count,
                     HDRMRLPC_N_RES_LABELS_MAX);
    }
    else
    {
      for (i=0; i < curr_flow_rsv.value.rsv_count; i++)
      {
        err |= HDRMRLPC_UNPACK_CON_REQ_RSV_RES_LABEL(
                 item_ptr, &(curr_flow_rsv.value.rsv_label[i]),
                 attrib_offset, attrib_val_offset, i );
      }

      if (err)
      {
        ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
        break;
      }

      if ( hdrmrlpc_reservation_attrib_is_valid(curr_flow_rsv.value) )
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        f_ptr->flow_rsv = curr_flow_rsv.value;
#else
        memcpy((void*)&f_ptr->flow_rsv,
               (void*)&curr_flow_rsv.value,
               sizeof(hdrmrlpc_flow_reservation_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

        c_ptr->is_accepted = TRUE;
        c_ptr->value = curr_flow_rsv.value_id;
      }

    }

    attrib_val_offset += (HDRMRLPC_RES_ATTRIB_MIN_VAL_SIZE + 
                          curr_flow_rsv.value.rsv_count * 8);

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (c_ptr->is_accepted)
    {
      break;
    }
  }
} /* hdrmrlpc_unpack_creq_reservation_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_SUP_HIGH_LAYER_PROT_ATTRIB

DESCRIPTION
  This function processes the supported higher layer protocols attribute 
  attribute of a Configuration Request message.
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_sup_high_layer_prot_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset
)
{
  uint16  attrib_val_offset; 
    /* Bit offset of current attribute value from the beginning of the current 
       attribute */
                               
  struct
  {
    uint8 value_id;
    hdrmrlpc_sup_higher_layer_prot_type value;
  } curr_flow_sup_prot; 
    /* Current Attribute Value and Value_ID*/

  int err; 
    /* sum of errno return vals (E_SUCCESS is zero) */

  uint8 i; 
    /* iterator */

  hdrmrlpc_config_req_result_type *c_ptr; /* Convenience pointer */
 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset((void*)&curr_flow_sup_prot.value, 0, 
         sizeof(hdrmrlpc_sup_higher_layer_prot_type));

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;
  c_ptr = &hdrmrlpc.config_result;
    
  if (c_ptr->sup_higher_layer_prot_attrib.is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->sup_higher_layer_prot_attrib.requested = TRUE;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_SHLP_ATTRIB_MIN_VAL_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_flow_sup_prot.value_id, 
            attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_SHLP_COUNT(
             item_ptr, &curr_flow_sup_prot.value.prot_count,
             attrib_offset, attrib_val_offset );
    
    /* _mpa_ AT should support everything... 
       but is it reasonable to support 255 protocol IDs? */
    if (curr_flow_sup_prot.value.prot_count > HDRMRLP_MAX_PROT_COUNT)
    {
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                 "SupHighLayerProt attrib: AT cannot support %d prot (max: %d)",
                     curr_flow_sup_prot.value.prot_count,
                     HDRMRLP_MAX_PROT_COUNT);
    }
    else
    {
      for (i=0; i < curr_flow_sup_prot.value.prot_count; i++)
      {
        err |= HDRMRLPC_UNPACK_CON_REQ_SHLP_ID(
                 item_ptr, &(curr_flow_sup_prot.value.prot_id[i]),
                 attrib_offset, attrib_val_offset, i );
      }

      if (err)
      {
        ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
        break;
      }
      
      if ( hdrmrlpc_sup_high_layer_prot_attrib_is_valid(curr_flow_sup_prot.value) )
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.sup_higher_layer_prot = curr_flow_sup_prot.value;
#else
        memcpy((void*)&hdrmrlpc.in_config.sup_higher_layer_prot,
               (void*)&curr_flow_sup_prot.value,
               sizeof(hdrmrlpc_sup_higher_layer_prot_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

        c_ptr->sup_higher_layer_prot_attrib.is_accepted = TRUE;
        c_ptr->sup_higher_layer_prot_attrib.value = 
          curr_flow_sup_prot.value_id;
      }

    }

    attrib_val_offset += (HDRMRLPC_SHLP_ATTRIB_MIN_VAL_SIZE + 
                          curr_flow_sup_prot.value.prot_count * 8);

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (c_ptr->sup_higher_layer_prot_attrib.is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_sup_high_layer_prot_attrib() */

/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_AN_SUPPORTED_QOS_PROFILES_ATTRIB

DESCRIPTION
  This function processes the AN supported QoS Profiles attribute 
  of a Configuration Request message.
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_an_sup_qos_profiles_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset
)
{
  uint16  attrib_val_offset; 
    /* Bit offset of current attribute value from the beginning of the current 
       attribute */
                               
  struct
  {
    uint8 value_id;
    hdrmrlpc_supported_qos_profiles value;
  } curr_an_sup_prof; 
    /* Current Attribute Value and Value_ID*/

  int err; 
    /* sum of errno return vals (E_SUCCESS is zero) */

  uint8 i, j; 
    /* iterator */

  hdrmrlpc_config_req_result_type *c_ptr; /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset((void*)&curr_an_sup_prof.value, 0, 
         sizeof(hdrmrlpc_supported_qos_profiles));

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;
  c_ptr = &hdrmrlpc.config_result;
    
  if (c_ptr->an_sup_qos_profiles.is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->an_sup_qos_profiles.requested = TRUE;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_ASQP_ATTRIB_MIN_VAL_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_an_sup_prof.value_id, 
            attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_ASQP_COUNT(
             item_ptr, &curr_an_sup_prof.value.profile_count,
             attrib_offset, attrib_val_offset );

    /* No need to range check profile_count because the data type range is 
       the same as the possible valid values. */
    attrib_val_offset += HDRMRLPC_ASQP_ATTRIB_MIN_VAL_SIZE;

    for (i=0; i < curr_an_sup_prof.value.profile_count; i++)
    {
      err |= HDRMRLPC_UNPACK_CON_REQ_ASQP_TYPE(
               item_ptr, &(curr_an_sup_prof.value.profile[i].profile_type),
               attrib_offset, attrib_val_offset );

      err |= HDRMRLPC_UNPACK_CON_REQ_ASQP_LENGTH(
        item_ptr, &(curr_an_sup_prof.value.profile[i].profile_length),
        attrib_offset, attrib_val_offset );

      if (curr_an_sup_prof.value.profile[i].profile_length <= HDRMRLP_MAX_AT_SUP_PROFILE_LENGTH)
      {
        for (j=0; j < curr_an_sup_prof.value.profile[i].profile_length; j++)
        {
          err |= HDRMRLPC_UNPACK_CON_REQ_ASQP_VALUE(
                   item_ptr, &(curr_an_sup_prof.value.profile[i].profile_value[j]),
                   attrib_offset, attrib_val_offset, j);
        }
      } 
      else
      {
        HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
         "ANSupQosProfiles attrib: AT cannot support ProfileLength %d (max: %d)",
          curr_an_sup_prof.value.profile[i].profile_length,
          HDRMRLP_MAX_AT_SUP_PROFILE_LENGTH);

        err = E_NOT_ALLOWED;
        break;
      }
      
      attrib_val_offset += (16 +
                            curr_an_sup_prof.value.profile[i].profile_length * 8);
    }
    

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }
      
    if ( hdrmrlpc_an_sup_qos_profile_attrib_is_valid(curr_an_sup_prof.value) )
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.an_sup_qos_profiles = curr_an_sup_prof.value;
#else
      memcpy((void*)&hdrmrlpc.in_config.an_sup_qos_profiles,
             (void*)&curr_an_sup_prof.value,
             sizeof(hdrmrlpc_supported_qos_profiles) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      c_ptr->an_sup_qos_profiles.is_accepted = TRUE;
      c_ptr->an_sup_qos_profiles.value = curr_an_sup_prof.value_id;
          
    }

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */
    if (c_ptr->an_sup_qos_profiles.is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_an_sup_qos_profiles_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_RESKK_QOS_RSP_ATTRIB

DESCRIPTION
  This function processes the ReservationKK QoS Response attribute of a 
  Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  is_fwd_rsv    - TRUE: fwd Rsv, FALSE: rev Rsv
  rsv_label     - Reservation Label

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_reskk_qos_rsp_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset,
  boolean is_fwd_rsv,
  uint8 rsv_label
)
{
  uint16  attrib_val_offset; 
    /* Bit offset of current attribute value from the beginning of the current 
       attribute */
                               
  struct
  {
    uint8 value_id;
    hdrmrlpc_res_qos_profile_config_type value;
  } curr_rsv_qos_rsp; 
    /* Current Attribute Value and Value_ID*/

  int err; 
    /* sum of errno return vals (E_SUCCESS is zero) */

  uint8 i; 
    /* iterator */

  hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */
  hdrmrlpc_reservation_kk_config_type *rsv_ptr; /* Convenience pointer */

  uint8 rsv_idx; /* Table index for this Reservation Label */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* FWD Reservation */
  if (is_fwd_rsv && 
      (hdrmrlpc_get_cfg_rl_idx(rsv_label, HDRMRLPC_FWD_RLP, &rsv_idx) == E_SUCCESS) )
  {
    c_ptr = &hdrmrlpc.config_result.res_kk_fwd_attrib[rsv_idx].qos_response;
    rsv_ptr = &hdrmrlpc.in_config.fwd_res_info[rsv_idx];
  }
  /* REV Reservation */
  else if ( !is_fwd_rsv &&
            (hdrmrlpc_get_cfg_rl_idx(rsv_label, HDRMRLPC_REV_RLP, &rsv_idx) == E_SUCCESS) )
  {
    c_ptr = &hdrmrlpc.config_result.res_kk_rev_attrib[rsv_idx].qos_response;
    rsv_ptr = &hdrmrlpc.in_config.rev_res_info[rsv_idx];
  }
  else
  {
    /* This is based on the discussion with QoS team: AN should not 
     * send a ResvKKQoSResponse attribute if AT hasn't send the QoSRequest
     * for this reservation label. If this happens, we can just ignore even for 
     * Reservation Label 0xFF because 0xFF should not be used in QosRequest/Rsp. 
     */
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                   "Res0x%xQoSRsp %d: Can't find rsv label, Ignore!",
                   rsv_label, is_fwd_rsv );
    return;
  }
  
  if (c_ptr->is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  memset((void*)&curr_rsv_qos_rsp.value, 0, 
         sizeof(hdrmrlpc_res_qos_profile_config_type));

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_RESKK_QOS_ATTRIB_MIN_VAL_SIZE) <=
         attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_rsv_qos_rsp.value_id, 
            attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_RESKK_TYPE(
             item_ptr, &curr_rsv_qos_rsp.value.type,
             attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_RESKK_LENGTH(
             item_ptr, &curr_rsv_qos_rsp.value.length,
             attrib_offset, attrib_val_offset );
    
    /* _mpa_ AT should support everything... 
       ...but is it reasonable to support a profile value of potentially 
       524,280 bits for each RLP? */
    if (curr_rsv_qos_rsp.value.length > HDRMRLP_MAX_PROFILE_LENGTH)
    {
      HDR_MSG_PROT_4(MSG_LEGACY_ERROR,
       "FlowNN%dQosP %d attrib: AT cannot support profile lgth (0x%x, max: 0x%x)",
                     rsv_label, is_fwd_rsv,
                     curr_rsv_qos_rsp.value.length,
                     HDRMRLP_MAX_PROFILE_LENGTH);
    }
    else
    {
      for (i=0; i < curr_rsv_qos_rsp.value.length; i++)
      {
        err |= HDRMRLPC_UNPACK_CON_REQ_RESKK_VAL(
                 item_ptr, &curr_rsv_qos_rsp.value.value[i],
                 attrib_offset, attrib_val_offset, i );
      }

      if (err)
      {
        ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
        break;
      }

      if ( hdrmrlpc_reskk_qos_rsp_is_valid(curr_rsv_qos_rsp.value) )
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        rsv_ptr->qos_response = curr_rsv_qos_rsp.value;
#else
        memcpy((void*)&rsv_ptr->qos_response,
               (void*)&curr_rsv_qos_rsp.value,
               sizeof(hdrmrlpc_res_qos_profile_config_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

        c_ptr->is_accepted = TRUE;
        c_ptr->value = curr_rsv_qos_rsp.value_id;
      }
    }

    attrib_val_offset += (HDRMRLPC_RESKK_QOS_ATTRIB_MIN_VAL_SIZE + 
                          curr_rsv_qos_rsp.value.length * 8);

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */
    if (c_ptr->is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_reskk_qos_rsp_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_FLOW_PROT_PARM_FWD_ATTRIB

DESCRIPTION
  This function processes the flow protocol parameters fwd attribute 
  of a Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  flow_id   - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_flow_prot_parm_fwd_attrib
( 
  dsm_item_type *item_ptr,
  uint8  attrib_len, 
  uint16 attrib_offset,
  uint8 flow_id
)
{
  uint16  attrib_val_offset; 
    /* Bit offset of current attribute value from the beginning of the current 
       attribute */
                               
  struct
  {
    uint8 value_id;
    hdrmrlpc_flow_prot_parm_fwd_type value;
  } curr_flow_prot; 
    /* Current Attribute Value and Value_ID*/

  uint8 protocol_id;

  int err; 
    /* sum of errno return vals (E_SUCCESS is zero) */

  uint8 i; 
    /* iterator */

  hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  c_ptr = &hdrmrlpc.config_result.flow_nn_fwd_attrib[flow_id].flow_prot_parm;
  
  if (c_ptr->is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  memset((void*)&curr_flow_prot.value, 0, 
         sizeof(hdrmrlpc_flow_prot_parm_fwd_type));

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_PROT_PARM_ATTRIB_MIN_VAL_SIZE )
         <= attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_flow_prot.value_id, 
            attrib_offset, attrib_val_offset );
    attrib_val_offset += sizeof(curr_flow_prot.value_id) * 8; 
    
    err |= HDRBIT_UNPACK8(item_ptr, &protocol_id, 
                          (attrib_offset + attrib_val_offset), 
                          sizeof(protocol_id) * 8);
    if (protocol_id >= HDRMRLPC_FLOW_PROT_ID_MAX)
    {
      HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                      "Flow%dFlowProtParmFwd: prot %d is not valid",
                      flow_id, protocol_id );
      break;
    }
    else 
    {
      curr_flow_prot.value.id = (hdrmrlpc_flow_prot_id_type) protocol_id;
    }
    attrib_val_offset += sizeof(protocol_id) * 8;

    err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.length, 
                          (attrib_offset + attrib_val_offset), 
                          sizeof(curr_flow_prot.value.length) * 8);
    curr_flow_prot.value.length *= 8;
    attrib_val_offset += sizeof(curr_flow_prot.value.length) * 8;

    if (curr_flow_prot.value.id != HDRMRLPC_FLOW_PROT_ID_ROHC)
    {
      if (curr_flow_prot.value.length > 0)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "Flow%dFlowProtParmFwd: Parm length %d not valid for prot %d",
                       flow_id, curr_flow_prot.value.length, 
                       curr_flow_prot.value.id );
        break;
      }
    }
    else
    {
      if (curr_flow_prot.value.length < HDRMRLPC_PROT_PARM_REC_FWD_MIN_VAL_SIZE)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "Flow%dFlowProtParmFwd: Parm length %d not valid for prot %d",
                       flow_id, curr_flow_prot.value.length, 
                       curr_flow_prot.value.id );
        break;
      }

      /* Unpack the Protocol Parameters record */
      err |= HDRBIT_UNPACK16(item_ptr, &curr_flow_prot.value.parm.rohc.max_cid,
                             (attrib_offset + attrib_val_offset), 
                             sizeof(curr_flow_prot.value.parm.rohc.max_cid) * 8);
      attrib_val_offset += sizeof(curr_flow_prot.value.parm.rohc.max_cid) * 8;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.parm.rohc.large_cids,
                            (attrib_offset + attrib_val_offset), 
                            HDRMRLPC_PROT_PARM_LARGE_CIDS_SIZE);
      attrib_val_offset += HDRMRLPC_PROT_PARM_LARGE_CIDS_SIZE;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.parm.rohc.feed_back_for_incl,
                            (attrib_offset + attrib_val_offset), 
                            HDRMRLPC_PROT_PARM_FEED_BACK_FOR_INCL_SIZE);
      attrib_val_offset += HDRMRLPC_PROT_PARM_FEED_BACK_FOR_INCL_SIZE;

      if (curr_flow_prot.value.parm.rohc.feed_back_for_incl)
      {
        err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.parm.rohc.feed_back_for,
                              (attrib_offset + attrib_val_offset), 
                              HDRMRLPC_PROT_PARM_FEED_BACK_FOR_SIZE);
        attrib_val_offset += HDRMRLPC_PROT_PARM_FEED_BACK_FOR_SIZE;
      }

      err |= HDRBIT_UNPACK16(item_ptr, &curr_flow_prot.value.parm.rohc.mrru,
                             (attrib_offset + attrib_val_offset), 
                             sizeof(curr_flow_prot.value.parm.rohc.mrru) * 8);
      attrib_val_offset += sizeof(curr_flow_prot.value.parm.rohc.mrru) * 8;

#ifdef FEATURE_HDR_EMPA_SUPPORT_STD_V115_ATTRIB_ID
      if (hdrmrlpc.support_v115)
      {
        err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.parm.rohc.delayed_decomp_depth,
                              (attrib_offset + attrib_val_offset), 
                              sizeof(curr_flow_prot.value.parm.rohc.delayed_decomp_depth) * 8);
        attrib_val_offset += sizeof(curr_flow_prot.value.parm.rohc.delayed_decomp_depth) * 8;
      }
#endif

      err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.parm.rohc.profile_count,
                            (attrib_offset + attrib_val_offset), 
                            sizeof(curr_flow_prot.value.parm.rohc.profile_count) * 8);
      attrib_val_offset += sizeof(curr_flow_prot.value.parm.rohc.profile_count) * 8;

      if (curr_flow_prot.value.parm.rohc.profile_count > HDRMRLPC_MAX_PROT_PROFILE_COUNT)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "FlowNN%dFlowProtParmFwd: profile_count %d over limit %d)",
                       flow_id, curr_flow_prot.value.parm.rohc.profile_count, 
                       HDRMRLPC_MAX_PROT_PROFILE_COUNT);
        break;
      }
      else
      {
        for ( i = 0; i < curr_flow_prot.value.parm.rohc.profile_count; i++ )
        {
          err |= HDRBIT_UNPACK16(item_ptr, &curr_flow_prot.value.parm.rohc.profile[i],
                                 (attrib_offset + attrib_val_offset), 
                                 sizeof(curr_flow_prot.value.parm.rohc.profile[0]) * 8);
          attrib_val_offset += sizeof(curr_flow_prot.value.parm.rohc.profile[0]) * 8;
        }
      }
    }

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }

    if ( hdrmrlpc_flow_prot_parm_fwd_attrib_is_valid(curr_flow_prot.value) )
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.fwd_flow_prot[flow_id] = curr_flow_prot.value;
#else
      memcpy((void*)&hdrmrlpc.in_config.fwd_flow_prot[flow_id],
             (void*)&curr_flow_prot.value,
             sizeof(hdrmrlpc_flow_prot_parm_fwd_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
      
      c_ptr->is_accepted = TRUE;
      c_ptr->value = curr_flow_prot.value_id;
    }

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (c_ptr->is_accepted)
    {
      break;
    }
  }
} /* hdrmrlpc_unpack_creq_flow_prot_parm_fwd_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_FLOW_PROT_PARM_REV_ATTRIB

DESCRIPTION
  This function processes the flow protocol parameters rev attribute 
  of a Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  flow_id   - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_flow_prot_parm_rev_attrib
( 
  dsm_item_type *item_ptr,
  uint8  attrib_len, 
  uint16 attrib_offset,
  uint8 flow_id
)
{
  uint16  attrib_val_offset; 
    /* Bit offset of current attribute value from the beginning of the current 
       attribute */
                               
  struct
  {
    uint8 value_id;
    hdrmrlpc_flow_prot_parm_rev_type value;
  } curr_flow_prot; 
    /* Current Attribute Value and Value_ID*/

  uint8 protocol_id;

  int err; 
    /* sum of errno return vals (E_SUCCESS is zero) */

  uint8 i; 
    /* iterator */

  hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  c_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[flow_id].flow_prot_parm;
  
  if (c_ptr->is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  memset((void*)&curr_flow_prot.value, 0, 
         sizeof(hdrmrlpc_flow_prot_parm_rev_type));

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_PROT_PARM_ATTRIB_MIN_VAL_SIZE )
         <= attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_flow_prot.value_id, 
            attrib_offset, attrib_val_offset );
    attrib_val_offset += sizeof(curr_flow_prot.value_id) * 8;
    
    err |= HDRBIT_UNPACK8(item_ptr, &protocol_id, 
                          (attrib_offset + attrib_val_offset), sizeof(protocol_id) * 8);
    if (protocol_id >= HDRMRLPC_FLOW_PROT_ID_MAX)
    {
      HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                      "Flow%dFlowProtParmRev: prot %d is not valid",
                      flow_id, protocol_id );
      break;
    }
    else 
    {
      curr_flow_prot.value.id = (hdrmrlpc_flow_prot_id_type) protocol_id;
    }
    attrib_val_offset += sizeof(protocol_id) * 8;

    err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.length, 
                          (attrib_offset + attrib_val_offset), 
                          sizeof(curr_flow_prot.value.length) * 8);
    curr_flow_prot.value.length *= 8;
    attrib_val_offset += sizeof(curr_flow_prot.value.length) * 8;

    if (curr_flow_prot.value.id != HDRMRLPC_FLOW_PROT_ID_ROHC)
    {
      if (curr_flow_prot.value.length > 0)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "Flow%dFlowProtParmRev: Parm length %d not valid for prot %d",
                       flow_id, curr_flow_prot.value.length, 
                       curr_flow_prot.value.id );
        break;
      }
    }
    else
    {
      if (curr_flow_prot.value.length < HDRMRLPC_PROT_PARM_REC_REV_MIN_VAL_SIZE)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "Flow%dFlowProtParmRev: Parm length %d not valid for prot %d",
                       flow_id, curr_flow_prot.value.length, 
                       curr_flow_prot.value.id );
        break;
      }

      /* Unpack the Protocol Parameters record */
      err |= HDRBIT_UNPACK16(item_ptr, &curr_flow_prot.value.parm.rohc.max_cid,
                             (attrib_offset + attrib_val_offset), 
                             sizeof(curr_flow_prot.value.parm.rohc.max_cid) * 8);
      attrib_val_offset += sizeof(curr_flow_prot.value.parm.rohc.max_cid) * 8;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.parm.rohc.large_cids,
                            (attrib_offset + attrib_val_offset), 
                            HDRMRLPC_PROT_PARM_LARGE_CIDS_SIZE);
      attrib_val_offset += HDRMRLPC_PROT_PARM_LARGE_CIDS_SIZE;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.parm.rohc.feed_back_for_incl,
                            (attrib_offset + attrib_val_offset), 
                            HDRMRLPC_PROT_PARM_FEED_BACK_FOR_INCL_SIZE);
      attrib_val_offset += HDRMRLPC_PROT_PARM_FEED_BACK_FOR_INCL_SIZE;

      if (curr_flow_prot.value.parm.rohc.feed_back_for_incl)
      {
        err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.parm.rohc.feed_back_for,
                              (attrib_offset + attrib_val_offset), 
                              HDRMRLPC_PROT_PARM_FEED_BACK_FOR_SIZE);
        attrib_val_offset += HDRMRLPC_PROT_PARM_FEED_BACK_FOR_SIZE;
      }

      err |= HDRBIT_UNPACK16(item_ptr, &curr_flow_prot.value.parm.rohc.mrru,
                             (attrib_offset + attrib_val_offset), 
                             sizeof(curr_flow_prot.value.parm.rohc.mrru) * 8);
      attrib_val_offset += sizeof(curr_flow_prot.value.parm.rohc.mrru) * 8;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.parm.rohc.timer_based_comp,
                            (attrib_offset + attrib_val_offset), 
                            HDRMRLPC_PROT_PARM_TIMER_BASED_COMP_SIZE);
      attrib_val_offset += HDRMRLPC_PROT_PARM_TIMER_BASED_COMP_SIZE;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_flow_prot.value.parm.rohc.profile_count,
                            (attrib_offset + attrib_val_offset), 
                            sizeof(curr_flow_prot.value.parm.rohc.profile_count) * 8);
      attrib_val_offset += sizeof(curr_flow_prot.value.parm.rohc.profile_count) * 8;

      if (curr_flow_prot.value.parm.rohc.profile_count > HDRMRLPC_MAX_PROT_PROFILE_COUNT)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "FlowNN%dFlowProtParmRev: profile_count %d over limit %d)",
                       flow_id, curr_flow_prot.value.parm.rohc.profile_count, 
                       HDRMRLPC_MAX_PROT_PROFILE_COUNT);
        break;
      }
      else
      {
        for ( i = 0; i < curr_flow_prot.value.parm.rohc.profile_count; i++ )
        {
          err |= HDRBIT_UNPACK16(item_ptr, &curr_flow_prot.value.parm.rohc.profile[i],
                                 (attrib_offset + attrib_val_offset), 
                                 sizeof(curr_flow_prot.value.parm.rohc.profile[0]) * 8);
          attrib_val_offset += sizeof(curr_flow_prot.value.parm.rohc.profile[0]) * 8;
        }
      }
    }

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }

    if ( hdrmrlpc_flow_prot_parm_rev_attrib_is_valid(curr_flow_prot.value) )
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.rev_flow_prot[flow_id] = curr_flow_prot.value; 
#else
      memcpy((void*)&hdrmrlpc.in_config.rev_flow_prot[flow_id],
             (void*)&curr_flow_prot.value,
             sizeof(hdrmrlpc_flow_prot_parm_rev_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
      
      c_ptr->is_accepted = TRUE;
      c_ptr->value = curr_flow_prot.value_id;
    }

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (c_ptr->is_accepted)
    {
      break;
    }
  }
} /* hdrmrlpc_unpack_creq_flow_prot_parm_rev_attrib() */





/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_ROUTE_PROT_PARM_FWD_ATTRIB

DESCRIPTION
  This function processes the route protocol parameters fwd attribute 
  of a Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  flow_id   - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_route_prot_parm_fwd_attrib
( 
  dsm_item_type *item_ptr,
  uint8  attrib_len, 
  uint16 attrib_offset,
  uint8 flow_id
)
{
  uint16  attrib_val_offset; 
    /* Bit offset of current attribute value from the beginning of the current 
       attribute */
                               
  struct
  {
    uint8 value_id;
    hdrmrlpc_route_prot_parm_fwd_type value;
  } curr_route_prot; 
    /* Current Attribute Value and Value_ID*/

  uint8 protocol_id;

  int err; 
    /* sum of errno return vals (E_SUCCESS is zero) */

  uint8 i; 
    /* iterator */

  hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  c_ptr = &hdrmrlpc.config_result.flow_nn_fwd_attrib[flow_id].route_prot_parm;
  
  if (c_ptr->is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  memset((void*)&curr_route_prot.value, 0, 
         sizeof(hdrmrlpc_route_prot_parm_fwd_type));

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_PROT_PARM_ATTRIB_MIN_VAL_SIZE )
         <= attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_route_prot.value_id, 
            attrib_offset, attrib_val_offset );
    attrib_val_offset += sizeof(curr_route_prot.value_id) * 8;
    
    err |= HDRBIT_UNPACK8(item_ptr, &protocol_id, 
                          (attrib_offset + attrib_val_offset), 
                          sizeof(protocol_id) * 8);
    if (protocol_id >= HDRMRLPC_ROUTE_PROT_ID_MAX)
    {
      HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                      "Flow%dRouteProtParmFwd: prot %d is not valid",
                      flow_id, protocol_id );
      break;
    }
    else 
    {
      curr_route_prot.value.id = (hdrmrlpc_route_prot_id_type) protocol_id;
    }
    attrib_val_offset += sizeof(protocol_id) * 8;

    err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.length, 
                          (attrib_offset + attrib_val_offset), 
                          sizeof(curr_route_prot.value.length) * 8);
    curr_route_prot.value.length *= 8;
    attrib_val_offset += sizeof(curr_route_prot.value.length) * 8;

    if (curr_route_prot.value.id != HDRMRLPC_ROUTE_PROT_ID_ROHC)
    {
      if (curr_route_prot.value.length > 0)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "Flow%dRouteProtParmFwd: Parm length %d not valid for prot %d",
                       flow_id, curr_route_prot.value.length, 
                       curr_route_prot.value.id );
        break;
      }
    }
    else
    {
      if (curr_route_prot.value.length < HDRMRLPC_PROT_PARM_REC_FWD_MIN_VAL_SIZE)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "Flow%dRouteProtParmFwd: Parm length %d not valid for prot %d",
                       flow_id, curr_route_prot.value.length, 
                       curr_route_prot.value.id );
        break;
      }

      /* Unpack the Protocol Parameters record */
      err |= HDRBIT_UNPACK16(item_ptr, &curr_route_prot.value.parm.rohc.max_cid,
                             (attrib_offset + attrib_val_offset), 
                             sizeof(curr_route_prot.value.parm.rohc.max_cid) * 8);
      attrib_val_offset += sizeof(curr_route_prot.value.parm.rohc.max_cid) * 8;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.parm.rohc.large_cids,
                            (attrib_offset + attrib_val_offset), 
                            HDRMRLPC_PROT_PARM_LARGE_CIDS_SIZE);
      attrib_val_offset += HDRMRLPC_PROT_PARM_LARGE_CIDS_SIZE;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.parm.rohc.feed_back_for_incl,
                            (attrib_offset + attrib_val_offset), 
                            HDRMRLPC_PROT_PARM_FEED_BACK_FOR_INCL_SIZE);
      attrib_val_offset += HDRMRLPC_PROT_PARM_FEED_BACK_FOR_INCL_SIZE;

      if (curr_route_prot.value.parm.rohc.feed_back_for_incl)
      {
        err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.parm.rohc.feed_back_for,
                              (attrib_offset + attrib_val_offset), 
                              HDRMRLPC_PROT_PARM_FEED_BACK_FOR_SIZE);
        attrib_val_offset += HDRMRLPC_PROT_PARM_FEED_BACK_FOR_SIZE;
      }

      err |= HDRBIT_UNPACK16(item_ptr, &curr_route_prot.value.parm.rohc.mrru,
                             (attrib_offset + attrib_val_offset), 
                             sizeof(curr_route_prot.value.parm.rohc.mrru) * 8);
      attrib_val_offset += sizeof(curr_route_prot.value.parm.rohc.mrru) * 8;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.parm.rohc.profile_count,
                            (attrib_offset + attrib_val_offset), 
                            sizeof(curr_route_prot.value.parm.rohc.profile_count) * 8);
      attrib_val_offset += sizeof(curr_route_prot.value.parm.rohc.profile_count) * 8;

      if (curr_route_prot.value.parm.rohc.profile_count > HDRMRLPC_MAX_PROT_PROFILE_COUNT)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "FlowNN%dRouteProtParmFwd: profile_count %d over limit %d)",
                       flow_id, curr_route_prot.value.parm.rohc.profile_count, 
                       HDRMRLPC_MAX_PROT_PROFILE_COUNT);
        break;
      }
      else
      {
        for ( i = 0; i < curr_route_prot.value.parm.rohc.profile_count; i++ )
        {
          err |= HDRBIT_UNPACK16(item_ptr, &curr_route_prot.value.parm.rohc.profile[i],
                                 (attrib_offset + attrib_val_offset), 
                                 sizeof(curr_route_prot.value.parm.rohc.profile[0]) * 8);
          attrib_val_offset += sizeof(curr_route_prot.value.parm.rohc.profile[0]) * 8;
        }
      }
    }

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }

    if ( hdrmrlpc_route_prot_parm_fwd_attrib_is_valid(curr_route_prot.value) )
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.fwd_route_prot[flow_id] = curr_route_prot.value; 
#else
      memcpy((void*)&hdrmrlpc.in_config.fwd_route_prot[flow_id],
             (void*)&curr_route_prot.value,
             sizeof(hdrmrlpc_route_prot_parm_fwd_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
      
      c_ptr->is_accepted = TRUE;
      c_ptr->value = curr_route_prot.value_id;
    }

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (c_ptr->is_accepted)
    {
      break;
    }
  }
} /* hdrmrlpc_unpack_creq_route_prot_parm_fwd_attrib() */





/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_ROUTE_PROT_PARM_REV_ATTRIB

DESCRIPTION
  This function processes the route protocol parameters rev attribute 
  of a Configuration Request message.

DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  flow_id   - RLP Flow ID

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_route_prot_parm_rev_attrib
( 
  dsm_item_type *item_ptr,
  uint8  attrib_len, 
  uint16 attrib_offset,
  uint8 flow_id
)
{
  uint16  attrib_val_offset; 
    /* Bit offset of current attribute value from the beginning of the current 
       attribute */
                               
  struct
  {
    uint8 value_id;
    hdrmrlpc_route_prot_parm_rev_type value;
  } curr_route_prot; 
    /* Current Attribute Value and Value_ID*/

  uint8 protocol_id;

  int err; 
    /* sum of errno return vals (E_SUCCESS is zero) */

  uint8 i; 
    /* iterator */

  hdrmrlpc_config_attrib_result_type *c_ptr; /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  c_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[flow_id].route_prot_parm;
  
  if (c_ptr->is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->requested = TRUE;

  memset((void*)&curr_route_prot.value, 0, 
         sizeof(hdrmrlpc_route_prot_parm_rev_type));

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_PROT_PARM_ATTRIB_MIN_VAL_SIZE )
         <= attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_route_prot.value_id, 
            attrib_offset, attrib_val_offset );
    attrib_val_offset += sizeof(curr_route_prot.value_id) * 8;
    
    err |= HDRBIT_UNPACK8(item_ptr, &protocol_id, 
                          (attrib_offset + attrib_val_offset), 
                          sizeof(protocol_id) * 8);
    if (protocol_id >= HDRMRLPC_ROUTE_PROT_ID_MAX)
    {
      HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                      "Flow%dRouteProtParmRev: prot %d is not valid",
                      flow_id, protocol_id );
      break;
    }
    else 
    {
      curr_route_prot.value.id = (hdrmrlpc_route_prot_id_type) protocol_id;
    }
    attrib_val_offset += sizeof(protocol_id) * 8;

    err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.length, 
                          (attrib_offset + attrib_val_offset), 
                          sizeof(curr_route_prot.value.length) * 8);
    curr_route_prot.value.length *= 8;
    attrib_val_offset += sizeof(curr_route_prot.value.length) * 8;

    if (curr_route_prot.value.id != HDRMRLPC_ROUTE_PROT_ID_ROHC)
    {
      if (curr_route_prot.value.length > 0)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "Flow%dRouteProtParmRev: Parm length %d not valid for prot %d",
                       flow_id, curr_route_prot.value.length, 
                       curr_route_prot.value.id );
        break;
      }
    }
    else
    {
      if (curr_route_prot.value.length < HDRMRLPC_PROT_PARM_REC_REV_MIN_VAL_SIZE)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "Flow%dRouteProtParmRev: Parm length %d not valid for prot %d",
                       flow_id, curr_route_prot.value.length, 
                       curr_route_prot.value.id );
        break;
      }

      /* Unpack the Protocol Parameters record */
      err |= HDRBIT_UNPACK16(item_ptr, &curr_route_prot.value.parm.rohc.max_cid,
                             (attrib_offset + attrib_val_offset), 
                             sizeof(curr_route_prot.value.parm.rohc.max_cid) * 8);
      attrib_val_offset += sizeof(curr_route_prot.value.parm.rohc.max_cid) * 8;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.parm.rohc.large_cids,
                            (attrib_offset + attrib_val_offset), 
                            HDRMRLPC_PROT_PARM_LARGE_CIDS_SIZE);
      attrib_val_offset += HDRMRLPC_PROT_PARM_LARGE_CIDS_SIZE;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.parm.rohc.feed_back_for_incl,
                            (attrib_offset + attrib_val_offset), 
                            HDRMRLPC_PROT_PARM_FEED_BACK_FOR_INCL_SIZE);
      attrib_val_offset += HDRMRLPC_PROT_PARM_FEED_BACK_FOR_INCL_SIZE;

      if (curr_route_prot.value.parm.rohc.feed_back_for_incl)
      {
        err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.parm.rohc.feed_back_for,
                              (attrib_offset + attrib_val_offset), 
                              HDRMRLPC_PROT_PARM_FEED_BACK_FOR_SIZE);
        attrib_val_offset += HDRMRLPC_PROT_PARM_FEED_BACK_FOR_SIZE;
      }

      err |= HDRBIT_UNPACK16(item_ptr, &curr_route_prot.value.parm.rohc.mrru,
                             (attrib_offset + attrib_val_offset), 
                             sizeof(curr_route_prot.value.parm.rohc.mrru) * 8);
      attrib_val_offset += sizeof(curr_route_prot.value.parm.rohc.mrru) * 8;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.parm.rohc.timer_based_comp,
                            (attrib_offset + attrib_val_offset), 
                            HDRMRLPC_PROT_PARM_TIMER_BASED_COMP_SIZE);
      attrib_val_offset += HDRMRLPC_PROT_PARM_TIMER_BASED_COMP_SIZE;

      err |= HDRBIT_UNPACK8(item_ptr, &curr_route_prot.value.parm.rohc.profile_count,
                            (attrib_offset + attrib_val_offset), 
                            sizeof(curr_route_prot.value.parm.rohc.profile_count) * 8);
      attrib_val_offset += sizeof(curr_route_prot.value.parm.rohc.profile_count) * 8;

      if (curr_route_prot.value.parm.rohc.profile_count > HDRMRLPC_MAX_PROT_PROFILE_COUNT)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                       "FlowNN%dRouteProtParmRev: profile_count %d over limit %d)",
                       flow_id, curr_route_prot.value.parm.rohc.profile_count, 
                       HDRMRLPC_MAX_PROT_PROFILE_COUNT);
        break;
      }
      else
      {
        for ( i = 0; i < curr_route_prot.value.parm.rohc.profile_count; i++ )
        {
          err |= HDRBIT_UNPACK16(item_ptr, &curr_route_prot.value.parm.rohc.profile[i],
                                 (attrib_offset + attrib_val_offset), 
                                 sizeof(curr_route_prot.value.parm.rohc.profile[0]) * 8);
          attrib_val_offset += sizeof(curr_route_prot.value.parm.rohc.profile[0]) * 8;
        }
      }
    }

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }

    if ( hdrmrlpc_route_prot_parm_rev_attrib_is_valid(curr_route_prot.value) )
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.rev_route_prot[flow_id] = curr_route_prot.value;
#else
      memcpy((void*)&hdrmrlpc.in_config.rev_route_prot[flow_id],
             (void*)&curr_route_prot.value,
             sizeof(hdrmrlpc_route_prot_parm_rev_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */
      
      c_ptr->is_accepted = TRUE;
      c_ptr->value = curr_route_prot.value_id;
    }

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */

    if (c_ptr->is_accepted)
    {
      break;
    }
  }
} /* hdrmrlpc_unpack_creq_route_prot_parm_rev_attrib() */




/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CREQ_MAX_RESERVATIONS_ATTRIB

DESCRIPTION
  This function processes the Max Reservations attribute 
  of a Configuration Request message.
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message
  attrib_len    - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrmrlpc_unpack_creq_max_reservations_attrib
( 
  dsm_item_type *item_ptr,
  uint16 attrib_len, 
  uint16 attrib_offset
)
{
  uint16  attrib_val_offset; 
    /* Bit offset of current attribute value from the beginning of the current 
       attribute */
                               
  struct
  {
    uint8 value_id;
    hdrmrlpc_max_reservations_type value;
  } curr_max_rsv; 
    /* Current Attribute Value and Value_ID*/

  int err; 
    /* sum of errno return vals (E_SUCCESS is zero) */

  hdrmrlpc_config_req_result_type *c_ptr; /* Convenience pointer */

 /*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset((void*)&curr_max_rsv.value, 0, 
         sizeof(hdrmrlpc_max_reservations_type));

  attrib_val_offset = HDRMRLPC_MIN_ATTRIB_LEN;
  c_ptr = &hdrmrlpc.config_result;
    
  if (c_ptr->max_reservations.is_accepted)
  {
    return;
  }
  
  /* Flag that the AN requested this attribute */
  c_ptr->max_reservations.requested = TRUE;

  /* Loop through all the values given for this attribute. */
  while ((attrib_val_offset + HDRMRLPC_MAX_RESERVATIONS_ATTRIB_MIN_VAL_SIZE)
          <= attrib_len)
  {
    /* There is at least one more attribute value in this attribute */

    err = HDRMRLPC_UNPACK_CON_REQ_ATTRIB_VALUE( 
            item_ptr, &curr_max_rsv.value_id, 
            attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_MAX_RSV_FWD(
             item_ptr, &curr_max_rsv.value.max_num_rsv_fwd,
             attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_MAX_RSV_REV(
             item_ptr, &curr_max_rsv.value.max_num_rsv_rev,
             attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_MAX_OPEN_RSV_FWD(
             item_ptr, &curr_max_rsv.value.max_num_open_rsv_fwd,
             attrib_offset, attrib_val_offset );

    err |= HDRMRLPC_UNPACK_CON_REQ_MAX_OPEN_RSV_REV(
             item_ptr, &curr_max_rsv.value.max_num_open_rsv_rev,
             attrib_offset, attrib_val_offset );

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      break;
    }
      
    if ( hdrmrlpc_max_reservations_attrib_is_valid(curr_max_rsv.value) )
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.max_reservations = curr_max_rsv.value;
#else
      memcpy((void*)&hdrmrlpc.in_config.max_reservations,
             (void*)&curr_max_rsv.value,
             sizeof(hdrmrlpc_max_reservations_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      c_ptr->max_reservations.is_accepted = TRUE;
      c_ptr->max_reservations.value = curr_max_rsv.value_id;
          
    }
    else
    {
     ERR ( "Invalid value for MaxReservations Attrib", 0, 0, 0 );
    }

    attrib_val_offset += HDRMRLPC_MAX_RESERVATIONS_ATTRIB_MIN_VAL_SIZE;

    /* If we've accepted a value from this attribute list, no need to
       continue and look at the other ones (if any) */
    if (c_ptr->max_reservations.is_accepted)
    {
      break;
    }
  }

} /* hdrmrlpc_unpack_creq_max_reservations_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_UNPACK_CONFIG_REQ_ATTRIB

DESCRIPTION
  This function processes MRLP attributes included in the Configuration 
  Request Message
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_id - Attribute ID of the attribute to process
  index - Flow ID for FlowNN attributes, 
          Reservation Label for ReservationKK attributes
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  TRUE  - Message unpacked successfully
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_unpack_config_req_attrib
(
  dsm_item_type *item_ptr,
  uint16 attrib_id,
  uint8 index,
  uint16 attrib_len,
  uint16 attrib_offset
)
{
  boolean send_rsp = TRUE;
  uint16 timers_attrib_min_len;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* _mpa_  optimize: use some table lookup of some kind instead */
  switch ( attrib_id )
  {
     /*-----------------------------------------------------------------------
     Unpack Maximum Abort Timer Attribute (MRLP)
     -----------------------------------------------------------------------*/
     case HDRMRLPC_MAX_ABORT_TIMER_ATTRIB_ID: 
       if (attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
       {
          hdrmrlpc_unpack_creq_max_abort_timer_attrib( item_ptr, 
                                                       attrib_len, 
                                                       attrib_offset );
       }
       else
       {
         ERR("Not enough bits for attribute %d",attrib_id,0,0);
         /* If the AN didn't include enough bits for at least one complete
          * attribute value we don't know what the AN intended, so it is 
          * safest not to send a response.
          */
         send_rsp = FALSE;
       }
       break;

      /*-----------------------------------------------------------------------
      Unpack Supported Higher Layer Protocols Attribute (MRLP)
      -----------------------------------------------------------------------*/
      case HDRMRLPC_SUPPORTED_HIGHER_LAYER_PROT_ATTRIB_ID: 
        if (attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
          hdrmrlpc_unpack_creq_sup_high_layer_prot_attrib( item_ptr, 
                                                           attrib_len, 
                                                           attrib_offset );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is 
           * safest not to send a response.
           */
          send_rsp = FALSE;
        }
        break;

      /*-----------------------------------------------------------------------
      Unpack AN Supported QOS Profile Attribute
      -----------------------------------------------------------------------*/
      case HDRMRLPC_AN_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
        if (attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
          hdrmrlpc_unpack_creq_an_sup_qos_profiles_attrib( item_ptr, 
                                                      attrib_len, 
                                                      attrib_offset );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is 
           * safest not to send a response.
           */
          send_rsp = FALSE;
        }
        break;

      /*-----------------------------------------------------------------------
      Unpack Max Reservations Attribute
      -----------------------------------------------------------------------*/
      case HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID:
        if (attrib_len >= HDRMRLPC_MAX_RESERVATIONS_ATTRIB_MIN_VAL_SIZE)
        {
          hdrmrlpc_unpack_creq_max_reservations_attrib( item_ptr, 
                                                        attrib_len, 
                                                        attrib_offset );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is 
           * safest not to send a response.
           */
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN nak Enable Fwd Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_NAK_ENABLE_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
          hdrmrlpc_unpack_creq_nak_enable_attrib( item_ptr, 
                                                  attrib_len, 
                                                  attrib_offset, 
                                                  TRUE,
                                                  index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN nak Enable Rev Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_NAK_ENABLE_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_nak_enable_attrib( item_ptr, 
                                                   attrib_len, 
                                                   attrib_offset, 
                                                   FALSE,
                                                   index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN Higher Layer Protocol Fwd Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_high_layer_prot_attrib( item_ptr, 
                                                        attrib_len, 
                                                        attrib_offset, 
                                                        TRUE,
                                                        index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN Higher Layer Protocol Rev Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_high_layer_prot_attrib( item_ptr, 
                                                        attrib_len, 
                                                        attrib_offset, 
                                                        FALSE,
                                                        index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN Physical Layer Nak Enabled Rev Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_PHYS_LAYER_NAK_ENABLED_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_phys_layer_nak_en_attrib( item_ptr, 
                                                          attrib_len, 
                                                          attrib_offset, 
                                                          index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN Activated Fwd Attributes (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_ACTIVATED_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
          hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,                                             
                                              HDRMRLPC_ATTRIB_ACTIVATED_FWD,
                                              &hdrmrlpc.config_result.flow_nn_fwd_attrib[index].activated,
                                              &hdrmrlpc.in_config.fwd_rlp_config_info[index].activated);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Activated Rev Attributes (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_ACTIVATED_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
          hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_ACTIVATED_REV,
                                              &hdrmrlpc.config_result.flow_nn_rev_attrib[index].activated,
                                              &hdrmrlpc.in_config.rev_rlp_config_info[index].activated);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Sequence Length Fwd Attributes (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_SIMPLE_SEQUENCE_LENGTH_FWD,
                                              &hdrmrlpc.config_result.flow_nn_fwd_attrib[index].simple_sequence_length,
                                              &hdrmrlpc.in_config.fwd_rlp_config_info[index].simple_sequence_length);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Sequence Length Rev Attributes (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_SEQ_LEN_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_SEQUENCE_LENGTH_REV,
                                              &hdrmrlpc.config_result.flow_nn_rev_attrib[index].simple_sequence_length,
                                              &hdrmrlpc.in_config.rev_rlp_config_info[index].simple_sequence_length);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Flow Protocol SDU Fwd Attributes (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_FLOW_PROT_SDU_FWD,
                                              &hdrmrlpc.config_result.flow_nn_fwd_attrib[index].flow_prot_sdu,
                                              &hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_prot_sdu);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Ack Flow Protocol SDU Rev Attributes (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_FLOW_PROT_SDU_REV,
                                              &hdrmrlpc.config_result.flow_nn_rev_attrib[index].flow_prot_sdu,
                                              &hdrmrlpc.in_config.rev_rlp_config_info[index].flow_prot_sdu);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Data Unit Fwd (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_DATA_UNIT_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_DATA_UNIT_FWD,
                                              &hdrmrlpc.config_result.flow_nn_fwd_attrib[index].data_unit,
                                              &hdrmrlpc.in_config.fwd_rlp_config_info[index].data_unit);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Data Unit Rev (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_DATA_UNIT_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_DATA_UNIT_REV,
                                              &hdrmrlpc.config_result.flow_nn_rev_attrib[index].data_unit,
                                              &hdrmrlpc.in_config.rev_rlp_config_info[index].data_unit);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Route Protocol SDU Fwd (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_ROUTE_PROT_SDU_FWD,
                                              &hdrmrlpc.config_result.flow_nn_fwd_attrib[index].route_prot_sdu,
                                              &hdrmrlpc.in_config.fwd_rlp_config_info[index].route_prot_sdu);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Route Protocol SDU Rev (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_ROUTE_PROT_SDU_REV,
                                              &hdrmrlpc.config_result.flow_nn_rev_attrib[index].route_prot_sdu,
                                              &hdrmrlpc.in_config.rev_rlp_config_info[index].route_prot_sdu);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Simultaneous Delivery On Both Routes Fwd (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_SIM_DEL_BOTH_ROUTES_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_SIM_DEL_BOTH_ROUTES_FWD,
                                              &hdrmrlpc.config_result.flow_nn_fwd_attrib[index].sim_del_both_routes,
                                              &hdrmrlpc.in_config.fwd_rlp_config_info[index].sim_del_both_routes);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
      /*---------------------------------------------------------------------
      Unpack FlowNN Out Of Order Delivery to Route Protocol Fwd (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_OOO_DEL_ROUTE_PROT_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_OOO_DEL_ROUTE_PROT_FWD,
                                              &hdrmrlpc.config_result.flow_nn_fwd_attrib[index].ooo_del_route_prot,
                                              &hdrmrlpc.in_config.fwd_rlp_config_info[index].ooo_del_route_prot);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;
    
    

      /*---------------------------------------------------------------------
      Unpack FlowNN Identification Fwd Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_IDENTIFICATION_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_ID_ATTRIB_MIN_LEN)
        {
          hdrmrlpc_unpack_creq_flow_id_attrib( item_ptr, 
                                               attrib_len, 
                                               attrib_offset,
                                               TRUE,
                                               index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN Identification Rev Attributes (MRLP) or 
      FlowNNSequenceLengthFwd Attributes (MMPA).
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_ID_REV_OR_SEQ_LEN_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_ID_ATTRIB_MIN_LEN)
        {
          if (hdrstream_get_negotiated_snpktapp() == 
              HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)
          {
            hdrmrlpc_unpack_creq_seq_len_fwd_attrib( item_ptr, 
                                                     attrib_len, 
                                                     attrib_offset, 
                                                     TRUE,
                                                     index );

          }
          else
          {
            hdrmrlpc_unpack_creq_flow_id_attrib( item_ptr, 
                                                 attrib_len, 
                                                 attrib_offset, 
                                                 FALSE,
                                                 index );
          }
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN Timers Fwd Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_TIMER_FWD_ATTRIB_ID: 

        /* MMPA FlowNNTimersFwd attribute has an additional field 
           (NakdelayTimer) hence the attrib value size is different */
        if (hdrstream_get_negotiated_snpktapp() == 
            HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)
        {
          timers_attrib_min_len = HDRMRLPC_MMPA_TIMERS_FWD_ATTRIB_MIN_LEN;
        }
        else
        {
          timers_attrib_min_len = HDRMRLPC_TIMERS_ATTRIB_MIN_LEN;
        }

        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= timers_attrib_min_len)
        {
          hdrmrlpc_unpack_creq_timers_attrib( item_ptr, 
                                              attrib_len, 
                                              attrib_offset, 
                                              TRUE,
                                              index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN Timers Rev Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_TIMER_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_TIMERS_ATTRIB_MIN_LEN)
        {
          hdrmrlpc_unpack_creq_timers_attrib( item_ptr, 
                                              attrib_len, 
                                              attrib_offset, 
                                              FALSE,
                                              index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN Reservation Fwd Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_RESERVATION_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_RES_ATTRIB_MIN_LEN)
        {
           hdrmrlpc_unpack_creq_reservation_attrib( item_ptr, 
                                                    attrib_len, 
                                                    attrib_offset, 
                                                    TRUE,
                                                    index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN Reservation Rev Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_RESERVATION_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= HDRMRLPC_RES_ATTRIB_MIN_LEN)
        {
           hdrmrlpc_unpack_creq_reservation_attrib( item_ptr, 
                                                    attrib_len, 
                                                    attrib_offset, 
                                                    FALSE,
                                                    index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;


      /*---------------------------------------------------------------------
      Unpack FlowNN Transmit Abort Timer Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_REV_ATTRIB_ID:
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_16_LEN)
        {
          hdrmrlpc_unpack_creq_xmit_abort_timer_attrib( item_ptr,
                                                        attrib_len,
                                                        attrib_offset,
                                                        TRUE,
                                                        index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack FlowNN nak Data Over Signalling Allowed Fwd Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_DATA_OVER_SIG_ALLOWED_REV_ATTRIB_ID:
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
          hdrmrlpc_unpack_creq_dos_allowed_attrib( item_ptr,
                                                   attrib_len,
                                                   attrib_offset,
                                                   TRUE,
                                                   index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack ReservationKK Idle State Fwd Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_RES_KK_IDLE_STATE_FWD_ATTRIB_ID:

        /* No need to range check "index" because the data type range is 
           the same as the possible valid values. */ 
        if (attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_reskk_idle_state_attrib( item_ptr, 
                                                         attrib_len, 
                                                         attrib_offset, 
                                                         TRUE,
                                                         index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack ReservationKK Idle State Rev Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_RES_KK_IDLE_STATE_REV_ATTRIB_ID:

        /* No need to range check "index" because the data type range is 
           the same as the possible valid values. */  
        if (attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_reskk_idle_state_attrib( item_ptr, 
                                                         attrib_len, 
                                                         attrib_offset, 
                                                         FALSE, 
                                                         index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack ReservationKK QoS Response Fwd Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_RES_KK_QOS_RSP_FWD_ATTRIB_ID:

        /* No need to range check "index" because the data type range is 
           the same as the possible valid values. */  
        if (attrib_len >= HDRMRLPC_RESKK_QOS_ATTRIB_MIN_LEN)
        {
           hdrmrlpc_unpack_creq_reskk_qos_rsp_attrib( item_ptr, 
                                                      attrib_len, 
                                                      attrib_offset, 
                                                      TRUE,
                                                      index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      /*---------------------------------------------------------------------
      Unpack ReservationKK QoS Response Rev Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_RES_KK_QOS_RSP_REV_ATTRIB_ID:
        
        /* No need to range check "index" because the data type range is 
           the same as the possible valid values. */  
        if (attrib_len >= HDRMRLPC_RESKK_QOS_ATTRIB_MIN_LEN)
        {
           hdrmrlpc_unpack_creq_reskk_qos_rsp_attrib( item_ptr, 
                                                      attrib_len, 
                                                      attrib_offset, 
                                                      FALSE,
                                                      index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break; 

      /*---------------------------------------------------------------------
      ReservationKK QoS Request Attributes (MRLP)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID:
      case HDRMRLPC_RES_KK_QOS_REQ_REV_ATTRIB_ID:
        /*-------------------------------------------------------------------
         * According to the stanard, AN is not allowed to send those 2 
         * requests. If AT receives them, it will omit those attributes
         * in the ConfigurationResponse message as a way of rejecting,
         * rather than not responding to ConfigurationReq or sending 
         * SessionClose
         ------------------------------------------------------------------*/
        ERR("AN not allowed to configure a ReservationKKQosRequest Attribute",0,0,0);
      break;

      /*---------------------------------------------------------------------
      Unpack FlowNN Flow Protocol Paramters Fwd Attributes (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= (HDRMRLPC_MIN_ATTRIB_LEN+HDRMRLPC_PROT_PARM_ATTRIB_MIN_VAL_SIZE))
        {
           hdrmrlpc_unpack_creq_flow_prot_parm_fwd_attrib( item_ptr, 
                                                           attrib_len, 
                                                           attrib_offset, 
                                                           index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break; 

      /*---------------------------------------------------------------------
      Unpack FlowNN Flow Protocol Paramters Rev Attributes (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= (HDRMRLPC_MIN_ATTRIB_LEN+HDRMRLPC_PROT_PARM_ATTRIB_MIN_VAL_SIZE))
        {
           hdrmrlpc_unpack_creq_flow_prot_parm_rev_attrib( item_ptr, 
                                                           attrib_len, 
                                                           attrib_offset, 
                                                           index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break; 

      /*---------------------------------------------------------------------
      Unpack FlowNN Route Protocol Paramters Fwd Attributes (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_FWD_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_FWD &&
            attrib_len >= (HDRMRLPC_MIN_ATTRIB_LEN+HDRMRLPC_PROT_PARM_ATTRIB_MIN_VAL_SIZE))
        {
           hdrmrlpc_unpack_creq_route_prot_parm_fwd_attrib( item_ptr, 
                                                            attrib_len, 
                                                            attrib_offset, 
                                                            index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break; 

      /*---------------------------------------------------------------------
      Unpack FlowNN Route Protocol Paramters Rev Attributes (EMPA)
      ---------------------------------------------------------------------*/
      case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_REV_ATTRIB_ID: 
        if (index < HDRMRLPC_N_MRLP_MAX_REV &&
            attrib_len >= (HDRMRLPC_MIN_ATTRIB_LEN+HDRMRLPC_PROT_PARM_ATTRIB_MIN_VAL_SIZE))
        {
           hdrmrlpc_unpack_creq_route_prot_parm_rev_attrib( item_ptr, 
                                                            attrib_len, 
                                                            attrib_offset, 
                                                            index );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break; 

      case HDRMRLPC_TWO_ROUTES_SUPPORTED_ATTRIB_ID:
        if (attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_TWO_ROUTES_SUPPORTED,
                                              &hdrmrlpc.config_result.two_routes_supported,
                                              &hdrmrlpc.in_config.two_routes_supported);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      case HDRMRLPC_AT_NAKDELAY_SUPPORTED_ATTRIB_ID:
        if (attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_AT_NAK_DELAY_SUPPORTED,
                                              &hdrmrlpc.config_result.at_nak_delay_supported,
                                              &hdrmrlpc.in_config.at_nak_delay_supported);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      case HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_ATTRIB_ID:
        if (attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_FLOW_NAK_DELAY_TIME_FWD,
                                              &hdrmrlpc.config_result.flow_nn_fwd_attrib[index].nak_delay_time,
                                              &hdrmrlpc.in_config.fwd_rlp_config_info[index].nak_delay_time);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      case HDRMRLPC_PPP_FREE_AUTHENTICATION_SUPPORT_ATTRIB_ID:
        if (attrib_len >= HDRMRLPC_SIMPLE_ATTRIB_LEN)
        {
           hdrmrlpc_unpack_creq_simple_attrib(item_ptr, attrib_len, attrib_offset, index,
                                              HDRMRLPC_ATTRIB_PPP_FREE_AUTHENTICATION_SUPPORT,
                                              &hdrmrlpc.config_result.ppp_free_authentication_support,
                                              &hdrmrlpc.in_config.ppp_free_authentication_support);
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          send_rsp = FALSE;
        }
        break;

      default:
        ERR("Unknown Attribute ID (%d)", attrib_id | index, 0, 0 );
        break;

   } /* end switch */

  return send_rsp;

} /* hdrmrlpc_unpack_config_req_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_PACK_CONFIG_RSP_ATTRIBS

DESCRIPTION
  This function packs the MRLP attributes in a Configuration Response
  Message (AN INIT) 
  
DEPENDENCIES
  None

PARAMETERS
  send_msg_ptr - Pointer to the DSM item chain holding the message
  attrib_offset - Bit offset of the attribute from the beginning of the message

RETURN VALUE
  None
  
SIDE EFFECTS
  (attrib_offset is updated)

===========================================================================*/
void hdrmrlpc_pack_config_rsp_attribs
(
  dsm_item_type *send_msg_ptr,
  uint16 *attrib_offset,
  hdrhai_stream_enum_type stream_id
)
{
  uint8 index, j; /* iterators */
  uint8 rsv_idx;  /* tmp var for Reservation Label Index*/

  hdrmrlpc_config_req_result_type *c_ptr; /* Convenience pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  c_ptr = &hdrmrlpc.config_result;

  /*---------------------------------------------------------------------
  Pack MAX Abort Timer Attribute
  ---------------------------------------------------------------------*/
  if (c_ptr->max_abort_timer_attrib.requested)
  {
    if (c_ptr->max_abort_timer_attrib.is_accepted)
    {
      HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                               HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                               send_msg_ptr,
                               HDRMRLPC_MAX_ABORT_TIMER_ATTRIB_ID,
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                               send_msg_ptr,
                               c_ptr->max_abort_timer_attrib.value,
                               *attrib_offset);
      *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
      hdrmrlpc.verno.max_abort_timer++;
    }
    else
    {
      hdrmrlpc.in_config.max_abort_timer = 
        hdrmrlpc.current.max_abort_timer;
    }
  }

  /*---------------------------------------------------------------------
  Pack PPPFreeAuthenticationSupport Attribute
  ---------------------------------------------------------------------*/
  if (c_ptr->ppp_free_authentication_support.requested)
  {
    if (c_ptr->ppp_free_authentication_support.is_accepted)
    {
      HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                               HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                               send_msg_ptr,
                               HDRMRLPC_PPP_FREE_AUTHENTICATION_SUPPORT_ATTRIB_ID,
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                               send_msg_ptr,
                               c_ptr->ppp_free_authentication_support.value,
                               *attrib_offset);
      *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
      hdrmrlpc.verno.ppp_free_authentication_support++;
    }
    else
    {
      hdrmrlpc.in_config.ppp_free_authentication_support = 
        hdrmrlpc.current.ppp_free_authentication_support;
    }
  }

  /*---------------------------------------------------------------------
  Pack ATNakDelaySupported Attribute
  ---------------------------------------------------------------------*/
  if (c_ptr->at_nak_delay_supported.requested)
  {
    if (c_ptr->at_nak_delay_supported.is_accepted)
    {
      HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                               HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                               send_msg_ptr,
                               HDRMRLPC_AT_NAKDELAY_SUPPORTED_ATTRIB_ID,
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                               send_msg_ptr,
                               c_ptr->at_nak_delay_supported.value,
                               *attrib_offset);
      *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
      hdrmrlpc.verno.at_nak_delay_supported++;
    }
    else
    {
      hdrmrlpc.in_config.at_nak_delay_supported = 
        hdrmrlpc.current.at_nak_delay_supported;
    }
  }

  /*---------------------------------------------------------------------
  Pack Supported Higher Layer Protocol Attribute
  ---------------------------------------------------------------------*/
  if (c_ptr->sup_higher_layer_prot_attrib.requested)
  {
    if (c_ptr->sup_higher_layer_prot_attrib.is_accepted)
    {
      HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                               HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                               send_msg_ptr,
                               HDRMRLPC_SUPPORTED_HIGHER_LAYER_PROT_ATTRIB_ID,
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                               send_msg_ptr,
                               c_ptr->sup_higher_layer_prot_attrib.value,
                               *attrib_offset);
      *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
      hdrmrlpc.verno.sup_higher_layer_prot++;
    }
    else
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.sup_higher_layer_prot = 
                               hdrmrlpc.current.sup_higher_layer_prot;
#else
      memcpy(&hdrmrlpc.in_config.sup_higher_layer_prot,
             &hdrmrlpc.current.sup_higher_layer_prot,
             sizeof(hdrmrlpc_sup_higher_layer_prot_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
  }

  /*---------------------------------------------------------------------
  Pack AN Supported QoS Profiles Attribute
  ---------------------------------------------------------------------*/
  if (c_ptr->an_sup_qos_profiles.requested)
  {
    if (c_ptr->an_sup_qos_profiles.is_accepted)
    {
      HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                               HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                               send_msg_ptr,
                               HDRMRLPC_AN_SUPPORTED_QOS_PROFILES_ATTRIB_ID,
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                               send_msg_ptr,
                               c_ptr->an_sup_qos_profiles.value,
                               *attrib_offset);
      *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
      hdrmrlpc.verno.an_sup_qos_profiles++;
    }
    else
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.an_sup_qos_profiles = 
                         hdrmrlpc.current.an_sup_qos_profiles;
#else
      memcpy(&hdrmrlpc.in_config.an_sup_qos_profiles,
             &hdrmrlpc.current.an_sup_qos_profiles,
             sizeof(hdrmrlpc_supported_qos_profiles));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
  }

  /*---------------------------------------------------------------------
  Pack Max Reservations Attribute
  ---------------------------------------------------------------------*/
  if (c_ptr->max_reservations.requested)
  {
    if (c_ptr->max_reservations.is_accepted)
    {
      HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                               HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                               send_msg_ptr,
                               HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID,
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                               send_msg_ptr,
                               c_ptr->max_reservations.value,
                               *attrib_offset);
      *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
      hdrmrlpc.verno.max_reservations++;
    }
    else
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.max_reservations = hdrmrlpc.current.max_reservations;
#else
      memcpy(&hdrmrlpc.in_config.max_reservations,
             &hdrmrlpc.current.max_reservations,
             sizeof(hdrmrlpc_max_reservations_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
  }

  if (c_ptr->two_routes_supported.requested)
  {
    if (c_ptr->two_routes_supported.is_accepted)
    {
      HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                               HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                               send_msg_ptr,
                               HDRMRLPC_TWO_ROUTES_SUPPORTED_ATTRIB_ID,
                               *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                               send_msg_ptr,
                               c_ptr->two_routes_supported.value,
                               *attrib_offset);
      *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
      hdrmrlpc.verno.two_routes_supported++;
    }
    else
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.two_routes_supported = 
                            hdrmrlpc.current.two_routes_supported;
#else
      memcpy(&hdrmrlpc.in_config.two_routes_supported,
             &hdrmrlpc.current.two_routes_supported,
             sizeof(uint8));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
  }

  /*---------------------------------------------------------------------
  Pack Fwd FlowNN attributes
  ---------------------------------------------------------------------*/
  for (index = 0; index < HDRMRLPC_N_MRLP_MAX_FWD; index++)
  {

    /*---------------------------------------------------------------------
    Pack FlowNN Nak Enable Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].nak_enable.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].nak_enable.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_NAK_ENABLE_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_fwd_attrib[index].nak_enable.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.nak_enable_fwd[index]++;
      }
      else
      {
        hdrmrlpc.in_config.fwd_rlp_config_info[index].nak_enabled = 
          hdrmrlpc.current.fwd_rlp_config_info[index].nak_enabled;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Higher Layer Protocol Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].higher_layer_prot.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].higher_layer_prot.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
           HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                       c_ptr->flow_nn_fwd_attrib[index].higher_layer_prot.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.higher_layer_prot_fwd[index]++;
      }
      else
      {
        hdrmrlpc.in_config.fwd_rlp_config_info[index].higher_layer_protocol =
         hdrmrlpc.current.fwd_rlp_config_info[index].higher_layer_protocol;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Activated Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].activated.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].activated.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_ACTIVATED_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_fwd_attrib[index].activated.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.activated_fwd[index] ++;
      }
      else
      {
        hdrmrlpc.in_config.fwd_rlp_config_info[index].activated = 
          hdrmrlpc.current.fwd_rlp_config_info[index].activated;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Sequence Length Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].simple_sequence_length.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].simple_sequence_length.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_fwd_attrib[index].simple_sequence_length.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.sequence_length_fwd[index]++;
      }
      else
      {
        hdrmrlpc.in_config.fwd_rlp_config_info[index].simple_sequence_length = 
          hdrmrlpc.current.fwd_rlp_config_info[index].simple_sequence_length;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Flow Protocol SDU Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].flow_prot_sdu.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].flow_prot_sdu.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_fwd_attrib[index].flow_prot_sdu.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.flow_prot_sdu_fwd[index]++;
      }
      else
      {
        hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_prot_sdu = 
          hdrmrlpc.current.fwd_rlp_config_info[index].flow_prot_sdu;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Data Unit Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].data_unit.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].data_unit.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_DATA_UNIT_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_fwd_attrib[index].data_unit.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.data_unit_fwd[index] ++;
      }
      else
      {
        hdrmrlpc.in_config.fwd_rlp_config_info[index].data_unit = 
          hdrmrlpc.current.fwd_rlp_config_info[index].data_unit;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Route Protocol SDU Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].route_prot_sdu.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].route_prot_sdu.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_fwd_attrib[index].route_prot_sdu.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.route_prot_sdu_fwd[index]++;
      }
      else
      {
        hdrmrlpc.in_config.fwd_rlp_config_info[index].route_prot_sdu = 
          hdrmrlpc.current.fwd_rlp_config_info[index].route_prot_sdu;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Simultaneous Delivery On Both Routes Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].sim_del_both_routes.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].sim_del_both_routes.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_SIM_DEL_BOTH_ROUTES_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_fwd_attrib[index].sim_del_both_routes.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.sim_del_both_routes_fwd[index]++;
      }
      else
      {
        hdrmrlpc.in_config.fwd_rlp_config_info[index].sim_del_both_routes = 
          hdrmrlpc.current.fwd_rlp_config_info[index].sim_del_both_routes;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Out Of Order Delivery To Route Protocol Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].ooo_del_route_prot.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].ooo_del_route_prot.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_OOO_DEL_ROUTE_PROT_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_fwd_attrib[index].ooo_del_route_prot.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.ooo_del_route_prot_fwd[index]++;
      }
      else
      {
        hdrmrlpc.in_config.fwd_rlp_config_info[index].ooo_del_route_prot = 
          hdrmrlpc.current.fwd_rlp_config_info[index].ooo_del_route_prot;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Nak Delay Time Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].nak_delay_time.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].nak_delay_time.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_fwd_attrib[index].nak_delay_time.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.timers_fwd[index]++;
      }
      else
      {
        hdrmrlpc.in_config.fwd_rlp_config_info[index].nak_delay_time = 
          hdrmrlpc.current.fwd_rlp_config_info[index].nak_delay_time;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Identification Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].identification.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].identification.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
           HDRMRLPC_FLOW_NN_IDENTIFICATION_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                        c_ptr->flow_nn_fwd_attrib[index].identification.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.identification_fwd[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_id = 
                      hdrmrlpc.current.fwd_rlp_config_info[index].flow_id;
#else
        memcpy(&hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_id,
               &hdrmrlpc.current.fwd_rlp_config_info[index].flow_id,
               sizeof(hdrmrlpc_flow_identification_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Sequencelength Fwd Attribute. Note: This attribute is defined
    only for MMPA. For EMPA/MFPA it is a simple attribute.
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].complex_seq_len.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].complex_seq_len.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
           HDRMRLPC_FLOW_NN_ID_REV_OR_SEQ_LEN_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                        c_ptr->flow_nn_fwd_attrib[index].complex_seq_len.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.sequence_length_fwd[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.fwd_rlp_config_info[index].complex_seq_len = 
             hdrmrlpc.current.fwd_rlp_config_info[index].complex_seq_len;
#else
        memcpy(&hdrmrlpc.in_config.fwd_rlp_config_info[index].complex_seq_len,
               &hdrmrlpc.current.fwd_rlp_config_info[index].complex_seq_len,
               sizeof(hdrmrlpc_flow_sequence_length_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Timers Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].timers.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].timers.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                    HDRMRLPC_FLOW_NN_TIMER_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                                 c_ptr->flow_nn_fwd_attrib[index].timers.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.timers_fwd[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_timer = 
            hdrmrlpc.current.fwd_rlp_config_info[index].flow_timer;
#else
        memcpy(&hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_timer,
               &hdrmrlpc.current.fwd_rlp_config_info[index].flow_timer,
               sizeof(hdrmrlpc_flow_timers_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Reservation Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].reservation.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].reservation.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
              HDRMRLPC_FLOW_NN_RESERVATION_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                           c_ptr->flow_nn_fwd_attrib[index].reservation.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.reservation_fwd[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_rsv = 
              hdrmrlpc.current.fwd_rlp_config_info[index].flow_rsv;
#else
        memcpy(&hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_rsv,
               &hdrmrlpc.current.fwd_rlp_config_info[index].flow_rsv,
               sizeof(hdrmrlpc_flow_reservation_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNNFlowProtocolParameters Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].flow_prot_parm.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].flow_prot_parm.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
              HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                           c_ptr->flow_nn_fwd_attrib[index].flow_prot_parm.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.flow_prot_parm_fwd[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.fwd_flow_prot[index] = 
                        hdrmrlpc.current.fwd_flow_prot[index];
#else
        memcpy(&hdrmrlpc.in_config.fwd_flow_prot[index],
               &hdrmrlpc.current.fwd_flow_prot[index],
               sizeof(hdrmrlpc_flow_prot_parm_fwd_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNNRouteProtocolParameters Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_fwd_attrib[index].route_prot_parm.requested)
    {
      if (c_ptr->flow_nn_fwd_attrib[index].route_prot_parm.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
              HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                           c_ptr->flow_nn_fwd_attrib[index].route_prot_parm.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.route_prot_parm_fwd[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.fwd_route_prot[index] = 
                            hdrmrlpc.current.fwd_route_prot[index];
#else
        memcpy(&hdrmrlpc.in_config.fwd_route_prot[index],
               &hdrmrlpc.current.fwd_route_prot[index],
               sizeof(hdrmrlpc_route_prot_parm_fwd_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

  } /* end for: fwd rlp flows */

  /*---------------------------------------------------------------------
  Pack Rev FlowNN attributes
  ---------------------------------------------------------------------*/
  for (index = 0; index < HDRMRLPC_N_MRLP_MAX_REV; index++)
  {

    /* ---------------------------------------------------------------------
    Pack FlowNN Nak Enable Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].nak_enable.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].nak_enable.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                 HDRMRLPC_FLOW_NN_NAK_ENABLE_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                            c_ptr->flow_nn_rev_attrib[index].nak_enable.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.nak_enable_rev[index]++;
      }
      else
      {
        hdrmrlpc.in_config.rev_rlp_config_info[index].nak_enabled = 
          hdrmrlpc.current.rev_rlp_config_info[index].nak_enabled;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Higher Layer Protocol Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].higher_layer_prot.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].higher_layer_prot.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
         HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                     c_ptr->flow_nn_rev_attrib[index].higher_layer_prot.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.higher_layer_prot_rev[index]++;
      }
      else
      {
        hdrmrlpc.in_config.rev_rlp_config_info[index].higher_layer_protocol =
          hdrmrlpc.current.rev_rlp_config_info[index].higher_layer_protocol;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Physical Layer Nak Enabled Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].phys_layer_nak_enable.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].phys_layer_nak_enable.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
        HDRMRLPC_FLOW_NN_PHYS_LAYER_NAK_ENABLED_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                 c_ptr->flow_nn_rev_attrib[index].phys_layer_nak_enable.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.phys_link_nak_en_rev[index]++;
      }
      else
      {
        hdrmrlpc.in_config.rev_rlp_config_info[index].phys_link_nak_enabled_rev =
          hdrmrlpc.current.rev_rlp_config_info[index].phys_link_nak_enabled_rev;
      }
    }

    /* ---------------------------------------------------------------------
    Pack FlowNN TransmitAbortTimer Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].xmit_abort_timer.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].xmit_abort_timer.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_16_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
          HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_XMIT_ABORT_TIMER(
                                 send_msg_ptr,
                 c_ptr->flow_nn_rev_attrib[index].xmit_abort_timer.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_16_LEN;
        hdrmrlpc.verno.xmit_abort_timer_rev[index]++;
      }
      else
      {
        hdrmrlpc.in_config.rev_rlp_config_info[index].xmit_abort_timer =
          hdrmrlpc.current.rev_rlp_config_info[index].xmit_abort_timer;
      }
    }

    /* ---------------------------------------------------------------------
    Pack FlowNN DataOverSignallingAllowed Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].dos_allowed.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].dos_allowed.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
          HDRMRLPC_FLOW_NN_DATA_OVER_SIG_ALLOWED_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                            c_ptr->flow_nn_rev_attrib[index].dos_allowed.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.dos_allowed_rev[index]++;
      }
      else
      {
        hdrmrlpc.in_config.rev_rlp_config_info[index].dos_allowed = 
          hdrmrlpc.current.rev_rlp_config_info[index].dos_allowed;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Activated Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].activated.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].activated.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_ACTIVATED_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_rev_attrib[index].activated.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.activated_rev[index]++;
      }
      else
      {
        hdrmrlpc.in_config.rev_rlp_config_info[index].activated = 
          hdrmrlpc.current.rev_rlp_config_info[index].activated;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Sequence Length Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].simple_sequence_length.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].simple_sequence_length.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_SEQ_LEN_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_rev_attrib[index].simple_sequence_length.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.sequence_length_fwd[index]++;
      }
      else
      {
        hdrmrlpc.in_config.rev_rlp_config_info[index].simple_sequence_length = 
          hdrmrlpc.current.rev_rlp_config_info[index].simple_sequence_length;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Flow Protocol SDU Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].flow_prot_sdu.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].flow_prot_sdu.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_rev_attrib[index].flow_prot_sdu.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.flow_prot_sdu_rev[index]++;
      }
      else
      {
        hdrmrlpc.in_config.rev_rlp_config_info[index].flow_prot_sdu = 
          hdrmrlpc.current.rev_rlp_config_info[index].flow_prot_sdu;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Data Unit Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].data_unit.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].data_unit.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_DATA_UNIT_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_rev_attrib[index].data_unit.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.data_unit_rev[index]++;
      }
      else
      {
        hdrmrlpc.in_config.rev_rlp_config_info[index].data_unit = 
          hdrmrlpc.current.rev_rlp_config_info[index].data_unit;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Route Protocol SDU Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].route_prot_sdu.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].route_prot_sdu.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                   HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                              c_ptr->flow_nn_rev_attrib[index].route_prot_sdu.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.route_prot_sdu_rev[index]++;
      }
      else
      {
        hdrmrlpc.in_config.rev_rlp_config_info[index].route_prot_sdu = 
          hdrmrlpc.current.rev_rlp_config_info[index].route_prot_sdu;
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Identification Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].identification.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].identification.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
           HDRMRLPC_FLOW_NN_ID_REV_OR_SEQ_LEN_FWD_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                        c_ptr->flow_nn_rev_attrib[index].identification.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.identification_rev[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.rev_rlp_config_info[index].flow_id = 
               hdrmrlpc.current.rev_rlp_config_info[index].flow_id;
#else
        memcpy(&hdrmrlpc.in_config.rev_rlp_config_info[index].flow_id,
               &hdrmrlpc.current.rev_rlp_config_info[index].flow_id,
               sizeof(hdrmrlpc_flow_identification_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Timers Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].timers.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].timers.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                    HDRMRLPC_FLOW_NN_TIMER_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                                 c_ptr->flow_nn_rev_attrib[index].timers.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.timers_rev[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.rev_rlp_config_info[index].flow_timer = 
            hdrmrlpc.current.rev_rlp_config_info[index].flow_timer;
#else
        memcpy(&hdrmrlpc.in_config.rev_rlp_config_info[index].flow_timer,
               &hdrmrlpc.current.rev_rlp_config_info[index].flow_timer,
               sizeof(hdrmrlpc_flow_timers_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNN Reservation Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].reservation.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].reservation.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
                HDRMRLPC_FLOW_NN_RESERVATION_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                             c_ptr->flow_nn_rev_attrib[index].reservation.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.reservation_rev[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.rev_rlp_config_info[index].flow_rsv = 
                  hdrmrlpc.current.rev_rlp_config_info[index].flow_rsv;
#else
        memcpy(&hdrmrlpc.in_config.rev_rlp_config_info[index].flow_rsv,
               &hdrmrlpc.current.rev_rlp_config_info[index].flow_rsv,
               sizeof(hdrmrlpc_flow_reservation_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNNFlowProtocolParameters Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].flow_prot_parm.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].flow_prot_parm.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
              HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                           c_ptr->flow_nn_rev_attrib[index].flow_prot_parm.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.flow_prot_parm_rev[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.rev_flow_prot[index] = 
                     hdrmrlpc.current.rev_flow_prot[index];
#else
        memcpy(&hdrmrlpc.in_config.rev_flow_prot[index],
               &hdrmrlpc.current.rev_flow_prot[index],
               sizeof(hdrmrlpc_flow_prot_parm_rev_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

    /*---------------------------------------------------------------------
    Pack FlowNNRouteProtocolParameters Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->flow_nn_rev_attrib[index].route_prot_parm.requested)
    {
      if (c_ptr->flow_nn_rev_attrib[index].route_prot_parm.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(send_msg_ptr, 
                                 HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
                                 send_msg_ptr,
              HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_REV_ATTRIB_ID | (uint16)index,
                                 *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
                                 send_msg_ptr,
                           c_ptr->flow_nn_rev_attrib[index].route_prot_parm.value,
                                 *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.route_prot_parm_rev[index]++;
      }
      else
      {
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.rev_route_prot[index] = 
                           hdrmrlpc.current.rev_route_prot[index];
#else
        memcpy(&hdrmrlpc.in_config.rev_route_prot[index],
               &hdrmrlpc.current.rev_route_prot[index],
               sizeof(hdrmrlpc_route_prot_parm_rev_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

  } /* end for: rev rlp flows */

  /*---------------------------------------------------------------------
  Pack Fwd ReservationKK attributes
  ---------------------------------------------------------------------*/
  for (index = 0; index < hdrmrlpc.in_config.num_fwd_rsv; index++)
  {

    /*---------------------------------------------------------------------
    Pack ReservationKK Idle State Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->res_kk_fwd_attrib[index].idle_state.requested)
    {
      if (c_ptr->res_kk_fwd_attrib[index].idle_state.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(
          send_msg_ptr, 
          HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
          *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
          send_msg_ptr,
          HDRMRLPC_RES_KK_IDLE_STATE_FWD_ATTRIB_ID | 
            (uint16)hdrmrlpc.in_config.fwd_res_info[index].rsv_label,
          *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
          send_msg_ptr,
          c_ptr->res_kk_fwd_attrib[index].idle_state.value,
          *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.reskk_idle_state_fwd[index]++;
      }
      else
      {
        /* AT did not accept the configuration. */
        HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                     "ResKKIdleStateFwd: Config not accpted"); 

        /* If this reservation label was already present in the "current" 
           instance, default back to the current version. */
        if (hdrmrlpc_get_cur_rl_idx(hdrmrlpc.in_config.fwd_res_info[index].rsv_label, 
                                    HDRMRLPC_FWD_RLP, &rsv_idx) == E_SUCCESS)
        {
          HDR_MSG_PROT(MSG_LEGACY_MED, 
                       "ResKKIdleStateFwd: default back to current"); 
          hdrmrlpc.in_config.fwd_res_info[index].idle_state =
            hdrmrlpc.current.fwd_res_info[rsv_idx].idle_state;
        }
        /* If this was a new reservation label, just remove it from the "in_config"
           reservations. */
        else
        {
          HDR_MSG_PROT(MSG_LEGACY_MED, 
                       "ResKKIdleStateFwd: remove from in_config"); 

          for ( j = index; j < hdrmrlpc.in_config.num_fwd_rsv - 1; j++ )
          {
            hdrmrlpc.in_config.fwd_res_info[j] =
              hdrmrlpc.in_config.fwd_res_info[j+1];
          }

          memset(&hdrmrlpc.in_config.fwd_res_info[j],
                 0, sizeof(hdrmrlpc_reservation_kk_config_type));
          
          hdrmrlpc.in_config.num_fwd_rsv--;

        }
      }
    }

    /*---------------------------------------------------------------------
    Pack ReservationKK QoS Profile Response Fwd Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->res_kk_fwd_attrib[index].qos_response.requested)
    {
      if (c_ptr->res_kk_fwd_attrib[index].qos_response.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(
          send_msg_ptr, 
          HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
          *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
          send_msg_ptr,
          HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID | 
            (uint16)hdrmrlpc.in_config.fwd_res_info[index].rsv_label,
          *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
          send_msg_ptr,
          c_ptr->res_kk_fwd_attrib[index].qos_response.value,
          *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.reskk_qos_rsp_fwd[index]++;
      }
      else
      {
        /* AT did not accept the configuration. */

        /* If this reservation label was already present in the "current" 
           instance, default back to the current version. */
        if (hdrmrlpc_get_cur_rl_idx(hdrmrlpc.in_config.fwd_res_info[index].rsv_label, 
                                    HDRMRLPC_FWD_RLP, &rsv_idx) == E_SUCCESS)
        {
          hdrmrlpc.in_config.fwd_res_info[index].qos_response =
            hdrmrlpc.current.fwd_res_info[rsv_idx].qos_response;
        }
        /* If this was a new reservation label, just remove it from the "in_config"
           reservations. */
        else
        {
          for ( j = index; j < hdrmrlpc.in_config.num_fwd_rsv - 1; j++ )
          {
            hdrmrlpc.in_config.fwd_res_info[j] =
              hdrmrlpc.in_config.fwd_res_info[j+1];
          }

          memset(&hdrmrlpc.in_config.fwd_res_info[j],
                 0, sizeof(hdrmrlpc_reservation_kk_config_type));
          
          hdrmrlpc.in_config.num_fwd_rsv--;

        }
      }
    }

  } /* end for: fwd rsv labels */


  /*---------------------------------------------------------------------
  Pack Rev ReservationKK attributes
  ---------------------------------------------------------------------*/
  for (index = 0; index < hdrmrlpc.in_config.num_rev_rsv; index++)
  {

    /*---------------------------------------------------------------------
    Pack ReservationKK Idle State Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->res_kk_rev_attrib[index].idle_state.requested)
    {
      if (c_ptr->res_kk_rev_attrib[index].idle_state.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(
          send_msg_ptr, 
          HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
          *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
         send_msg_ptr,
         HDRMRLPC_RES_KK_IDLE_STATE_REV_ATTRIB_ID | 
           (uint16)hdrmrlpc.in_config.rev_res_info[index].rsv_label,
         *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
         send_msg_ptr,
         c_ptr->res_kk_rev_attrib[index].idle_state.value,
         *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.reskk_idle_state_rev[index]++;
      }
      else
      {
        /* AT did not accept the configuration. */
        HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                     "ResKKIdleStateRev: Config not accpted"); 

        /* If this reservation label was already present in the "current" 
           instance, default back to the current version. */
        if (hdrmrlpc_get_cur_rl_idx(hdrmrlpc.in_config.rev_res_info[index].rsv_label, 
                                    HDRMRLPC_REV_RLP, &rsv_idx) == E_SUCCESS)
        {
          HDR_MSG_PROT(MSG_LEGACY_MED, 
                       "ResKKIdleStateRev: Default back to current"); 
          hdrmrlpc.in_config.rev_res_info[index].idle_state =
            hdrmrlpc.current.rev_res_info[rsv_idx].idle_state;
        }
        /* If this was a new reservation label, just remove it from the "in_config"
           reservations. */
        else
        {
          HDR_MSG_PROT(MSG_LEGACY_MED, 
                       "ResKKIdleStateRev: remove from in_config"); 

          for ( j = index; j < hdrmrlpc.in_config.num_rev_rsv - 1; j++ )
          {
            hdrmrlpc.in_config.rev_res_info[j] =
              hdrmrlpc.in_config.rev_res_info[j+1];
          }

          memset(&hdrmrlpc.in_config.rev_res_info[j],
                 0, sizeof(hdrmrlpc_reservation_kk_config_type));
          
          hdrmrlpc.in_config.num_rev_rsv--;

        }
      }
    }

    /*---------------------------------------------------------------------
    Pack ReservationKK QoS Profile Response Rev Attribute
    ---------------------------------------------------------------------*/
    if (c_ptr->res_kk_rev_attrib[index].qos_response.requested)
    {
      if (c_ptr->res_kk_rev_attrib[index].qos_response.is_accepted)
      {
        HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(
          send_msg_ptr, 
          HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
          *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
          send_msg_ptr,
          HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID | 
            (uint16)hdrmrlpc.in_config.rev_res_info[index].rsv_label,
          *attrib_offset);
        HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
          send_msg_ptr,
          c_ptr->res_kk_rev_attrib[index].qos_response.value,
          *attrib_offset);
        *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
        hdrmrlpc.verno.reskk_qos_rsp_rev[index]++;
      }
      else
      {
        /* AT did not accept the configuration. */

        /* If this reservation label was already present in the "current" 
           instance, default back to the current version. */
        if (hdrmrlpc_get_cur_rl_idx(hdrmrlpc.in_config.rev_res_info[index].rsv_label, 
                                    HDRMRLPC_REV_RLP, &rsv_idx) == E_SUCCESS)
        {
          hdrmrlpc.in_config.rev_res_info[index].qos_response =
            hdrmrlpc.current.rev_res_info[rsv_idx].qos_response;
        }
        /* If this was a new reservation label, just remove it from the "in_config"
           reservations. */
        else
        {
          for ( j = index; j < hdrmrlpc.in_config.num_rev_rsv - 1; j++ )
          {
            hdrmrlpc.in_config.rev_res_info[j] =
              hdrmrlpc.in_config.rev_res_info[j+1];
          }

          memset(&hdrmrlpc.in_config.rev_res_info[j],
                 0, sizeof(hdrmrlpc_reservation_kk_config_type));
          
          hdrmrlpc.in_config.num_rev_rsv--;

        }
      }
    }

  } /* end for: rev rsv labels */

  /*---------------------------------------------------------------------
  Pack ReservationKK Idle State Rev Attribute for default flow
  ---------------------------------------------------------------------*/
  if (c_ptr->res_kk_rev_attrib_ff.idle_state.requested)
  {
    if (c_ptr->res_kk_rev_attrib_ff.idle_state.is_accepted)
    {
      HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(
        send_msg_ptr, 
        HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
        *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
       send_msg_ptr,
       HDRMRLPC_RES_KK_IDLE_STATE_REV_ATTRIB_ID | 
       (uint16)hdrmrlpc.in_config.rev_res_info_ff.rsv_label,
       *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
       send_msg_ptr,
       c_ptr->res_kk_rev_attrib_ff.idle_state.value,
       *attrib_offset);
      *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
      hdrmrlpc.verno.reskk_idle_state_rev_ff++;
    }
    else
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.rev_res_info_ff = hdrmrlpc.current.rev_res_info_ff;
#else
      memcpy(&hdrmrlpc.in_config.rev_res_info_ff,
             &hdrmrlpc.current.rev_res_info_ff,
             sizeof(hdrmrlpc_reservation_kk_config_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
  }

  /*---------------------------------------------------------------------
  Pack ReservationKK Idle State fwd Attribute for default flow
  ---------------------------------------------------------------------*/
  if (c_ptr->res_kk_fwd_attrib_ff.idle_state.requested)
  {
    if (c_ptr->res_kk_fwd_attrib_ff.idle_state.is_accepted)
    {
      HDRMRLPC_PACK_CON_RSP_ATTRIB_LEN(
        send_msg_ptr, 
        HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, 
        *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_ID(
       send_msg_ptr,
       HDRMRLPC_RES_KK_IDLE_STATE_FWD_ATTRIB_ID | 
       (uint16)hdrmrlpc.in_config.fwd_res_info_ff.rsv_label,
       *attrib_offset);
      HDRMRLPC_PACK_CON_RSP_ATTRIB_VALUE(
       send_msg_ptr,
       c_ptr->res_kk_fwd_attrib_ff.idle_state.value,
       *attrib_offset);
      *attrib_offset += HDRMRLPC_CON_RSP_ATTRIB_LEN;
      hdrmrlpc.verno.reskk_idle_state_fwd_ff++;
    }
    else
    {
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.fwd_res_info_ff = hdrmrlpc.current.fwd_res_info_ff;
#else
      memcpy(&hdrmrlpc.in_config.fwd_res_info_ff,
             &hdrmrlpc.current.fwd_res_info_ff,
             sizeof(hdrmrlpc_reservation_kk_config_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }
  }

} /* hdrmrlpc_pack_config_rsp_attribs() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_PACK_CREQ_AT_SUP_PROT_PARM_ATTRIB

DESCRIPTION
  This function packs the ATSupportedFlow/RouteProtocolParameters 
  Attribute that the AT would like to configure

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  offset   - offset from start of message to beginning of current attrib
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpc_pack_creq_at_sup_prot_parm_attrib
(
  dsm_item_type *item_ptr, 
  uint16                          *aoffset,
  hdrmrlpc_attrib_name_enum_type  attrib_name,
  uint8                           prot_pp
)
{
  uint16 voffset; /* Bit offset from start of attrib to current value */
  hdrmrlpc_config_attrib_result_type *c_ptr;  /* Convenience pointer */
  uint16 attrib_len; /* Attribute Length in bits */
  uint16 attrib_id;
  hdrmrlpc_sup_prot_rohc_parm_value_type *rohc_ptr;
  hdrmrlpc_at_sup_prot_parm_pp_type   *p_ptr; /* Protocol pointer */
  int    i;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( attrib_name == HDRMRLPC_ATTRIB_AT_SUP_FLOW_PROT_PARM )
  {
    c_ptr = &hdrmrlpc.config_result.prot_parm_pp_attrib[prot_pp].
             at_sup_flow_prot_parm;
    attrib_id = HDRMRLPC_AT_SUP_FLOW_PROT_PARM_PP_ATTRIB_ID | (uint16) prot_pp;
    p_ptr = &hdrmrlpc.desired.at_sup_flow_prot[prot_pp];
  }
  else if (attrib_name == HDRMRLPC_ATTRIB_AT_SUP_ROUTE_PROT_PARM )
  {
    c_ptr = &hdrmrlpc.config_result.prot_parm_pp_attrib[prot_pp].
             at_sup_route_prot_parm;
    attrib_id = HDRMRLPC_AT_SUP_ROUTE_PROT_PARM_PP_ATTRIB_ID | (uint16) prot_pp;
    p_ptr = &hdrmrlpc.desired.at_sup_route_prot[prot_pp];
  }
  else
  {
    return;
  }

  attrib_len = HDRMRLPC_ATTRIB_ID_SIZE +
               HDRMRLPC_PROT_PARM_ATTRIB_MIN_VAL_SIZE +
               p_ptr->value_len;

// #ifdef FEATURE_HDR_EMPA_SUPPORT_STD_V115_ATTRIB_ID
//   if (!hdrmrlpc.support_v115)
// #endif
//   {
//     if ( (attrib_id == (HDRMRLPC_AT_SUP_FLOW_PROT_PARM_PP_ATTRIB_ID | HDRMRLPC_FLOW_PROT_ID_ROHC)) ||
//          (attrib_id == (HDRMRLPC_AT_SUP_ROUTE_PROT_PARM_PP_ATTRIB_ID | HDRMRLPC_ROUTE_PROT_ID_ROHC)) )
//     {
//       attrib_len += HDRMRLPC_PROT_ROHC_VALUE_LEN;
//     }
//   }
       
  HDRMRLPC_PACK_CON_REQ_ATTRIB_LEN(
    item_ptr, (uint8)attrib_len / HDRMRLPC_BITS_PER_BYTE, *aoffset);

  HDRMRLPC_PACK_CON_REQ_ATTRIB_ID(item_ptr, attrib_id, *aoffset);
    
  /* Increment ValueID*/
  ++c_ptr->value;
      
  /* We only pack one record */
  voffset = HDRMRLPC_MIN_ATTRIB_LEN;

  /* Pack Value ID */
  HDRMRLPC_PACK_CON_REQ_ATTRIB_VALUE(item_ptr, c_ptr->value, *aoffset, voffset);
  voffset += sizeof(c_ptr->value) * 8;
     
  /* Pack ProtocolSupported field. */
  HDRBIT_PACK8(item_ptr, p_ptr->prot_supported, (*aoffset + voffset), 
               HDRMRLPC_PROT_SUPPORTED_SIZE);
  voffset += HDRMRLPC_PROT_SUPPORTED_SIZE;
        
    HDRBIT_PACK8(item_ptr, p_ptr->value_len / HDRMRLPC_BITS_PER_BYTE, 
                 (*aoffset + voffset), HDRMRLPC_PROT_VALUE_LEN_SIZE);
    voffset += HDRMRLPC_PROT_VALUE_LEN_SIZE;

    if ( p_ptr->value_len > 0 )
    {
        /* Only ROHC has the payload. */
        rohc_ptr = &p_ptr->value.rohc;
  
      HDRBIT_PACK16(item_ptr, rohc_ptr->max_max_cid, (*aoffset + voffset),
                      sizeof(rohc_ptr->max_max_cid) * 8);
        voffset += sizeof(rohc_ptr->max_max_cid) * 8;
  
      HDRBIT_PACK8(item_ptr, rohc_ptr->large_cid_supported, 
                   (*aoffset + voffset), 1);
      voffset += 1;
  
      HDRBIT_PACK16(item_ptr, rohc_ptr->max_mrru, (*aoffset + voffset),
                      sizeof(rohc_ptr->max_mrru) * 8);
        voffset += sizeof(rohc_ptr->max_mrru) * 8;
  
      HDRBIT_PACK8(item_ptr, rohc_ptr->timer_comp_supported,
                   (*aoffset + voffset), 1);
      voffset += 1;
  
      HDRBIT_PACK8(item_ptr, rohc_ptr->profile_count, (*aoffset + voffset),
                     sizeof(rohc_ptr->profile_count) * 8);
        voffset += sizeof(rohc_ptr->profile_count) * 8;
  
      for ( i = 0; i < rohc_ptr->profile_count; i++ )
      {
        HDRBIT_PACK16(item_ptr, rohc_ptr->profile[i], (*aoffset + voffset),
                        sizeof(rohc_ptr->profile[0]) * 8);
          voffset += sizeof(rohc_ptr->profile[0]) * 8;
      }
  
      HDRBIT_PACK8(item_ptr, 0, (*aoffset + voffset), 6);
      voffset += 6;
    }

  *aoffset += voffset;
      
} /* hdrmrlpc_pack_creq_at_sup_prot_parm_attrib() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_PACK_CREQ_MAX_RLP_FLOWS_ATTRIB

DESCRIPTION
  This function packs the Max RLP Flows Attribute that the 
  AT would like to configure

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  offset   - offset from start of message to beginning of current attrib
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpc_pack_creq_max_rlp_flows_attrib
(
  dsm_item_type *item_ptr, 
  uint16 *aoffset
)
{
  uint16 voffset; /* Bit offset from start of attrib to current value */

  hdrmrlpc_config_attrib_result_type *c_ptr;  /* Convenience pointer */

  uint16 attrib_len; /* Attribute Length in bits */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  c_ptr = &hdrmrlpc.config_result.max_rlp_flows;

  attrib_len = HDRMRLPC_ATTRIB_ID_SIZE +
               HDRMRLPC_MAX_RLP_ATTRIB_MIN_VAL_SIZE;
       
  HDRMRLPC_PACK_CON_REQ_ATTRIB_LEN(
    item_ptr, (uint8)attrib_len / HDRMRLPC_BITS_PER_BYTE, *aoffset);

  HDRMRLPC_PACK_CON_REQ_ATTRIB_ID(
    item_ptr, 
    HDRMRLPC_MAX_RLP_FLOWS_ATTRIB_ID, 
    *aoffset);
    
  /* Increment ValueID*/
  ++c_ptr->value;
      
  /* We only pack one record */
  voffset = HDRMRLPC_MIN_ATTRIB_LEN;

  HDRMRLPC_PACK_CON_REQ_ATTRIB_VALUE(item_ptr, c_ptr->value, *aoffset, voffset);

  HDRMRLPC_PACK_CON_REQ_MAX_RLP_FWD(item_ptr,
                                    hdrmrlpc.desired.max_rlp_flows.max_num_rlp_flows_fwd,
                                    *aoffset, voffset);

  HDRMRLPC_PACK_CON_REQ_MAX_RLP_REV(item_ptr, 
                                    hdrmrlpc.desired.max_rlp_flows.max_num_rlp_flows_rev,
                                    *aoffset, voffset);

  HDRMRLPC_PACK_CON_REQ_MAX_ACT_RLP_FWD(item_ptr, 
                                        hdrmrlpc.desired.max_rlp_flows.max_activated_rlp_flows_fwd, 
                                        *aoffset, voffset);

  HDRMRLPC_PACK_CON_REQ_MAX_ACT_RLP_REV(item_ptr, 
                                        hdrmrlpc.desired.max_rlp_flows.max_activated_rlp_flows_rev, 
                                        *aoffset, voffset);

  *aoffset += voffset + HDRMRLPC_MAX_RLP_ATTRIB_MIN_VAL_SIZE;
} /* hdrmrlpc_pack_creq_max_rlp_flows_attrib() */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_PACK_CREQ_MAX_RESERVATIONS_ATTRIB

DESCRIPTION
  This function packs the Max Reservations Attribute that the 
  AT would like to configure

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  offset   - offset from start of message to beginning of current attrib
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpc_pack_creq_max_reservations_attrib
(
  dsm_item_type *item_ptr, 
  uint16 *aoffset
)
{
  uint16 voffset; /* Bit offset from start of attrib to current value */

  hdrmrlpc_config_attrib_result_type *c_ptr;  /* Convenience pointer */

  uint16 attrib_len; /* Attribute Length in bits */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  c_ptr = &hdrmrlpc.config_result.max_reservations;

  attrib_len = HDRMRLPC_ATTRIB_ID_SIZE +
               HDRMRLPC_MAX_RESERVATIONS_ATTRIB_MIN_VAL_SIZE;
       
  HDRMRLPC_PACK_CON_REQ_ATTRIB_LEN(
    item_ptr, (uint8)attrib_len / HDRMRLPC_BITS_PER_BYTE, *aoffset);

  HDRMRLPC_PACK_CON_REQ_ATTRIB_ID(
    item_ptr, 
    HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID, 
    *aoffset);
    
  /* Increment ValueID*/
  ++c_ptr->value;
      
  /* We only pack one record */
  voffset = HDRMRLPC_MIN_ATTRIB_LEN;

  HDRMRLPC_PACK_CON_REQ_ATTRIB_VALUE(item_ptr, c_ptr->value, *aoffset, voffset);

  HDRMRLPC_PACK_CON_REQ_MAX_RSV_FWD(item_ptr, 
                                    hdrmrlpc.desired.max_reservations.max_num_rsv_fwd,
                                    *aoffset, voffset);

  HDRMRLPC_PACK_CON_REQ_MAX_RSV_REV(item_ptr, 
                                    hdrmrlpc.desired.max_reservations.max_num_rsv_rev, 
                                    *aoffset, voffset);

  HDRMRLPC_PACK_CON_REQ_MAX_OPEN_RSV_FWD(item_ptr, 
                                         hdrmrlpc.desired.max_reservations.max_num_open_rsv_fwd, 
                                         *aoffset, voffset);

  HDRMRLPC_PACK_CON_REQ_MAX_OPEN_RSV_REV(item_ptr, 
                                         hdrmrlpc.desired.max_reservations.max_num_open_rsv_rev, 
                                         *aoffset, voffset);

  *aoffset += voffset + HDRMRLPC_MAX_RESERVATIONS_ATTRIB_MIN_VAL_SIZE;  
} /* hdrmrlpc_pack_creq_max_reservations_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_PACK_CONFIG_REQ_ATTRIBS

DESCRIPTION
  This function packs the attributes that the AT would like to configure
  (AT INIT)
  
DEPENDENCIES
  None

PARAMETERS
  app      - Application type (MFPA or EMPA)
  item_ptr - Pointer to the DSM item chain holding the message
  offset   - offset from start of message to beginning of current attrib
  
RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpc_pack_config_req_attribs
(
  hdrhai_app_subtype_enum_type app,
  dsm_item_type * item_ptr,
  uint16 *aoffset
)
{
  hdrmrlpc_config_req_result_type * c_ptr; /* Convenience pointer */

  #if defined(FEATURE_HDR_EMPA) || defined (FEATURE_HDR_REVB)
  uint16 voffset; /* Bit offset from start of attrib to current value */
  uint8  prot_pp;
  #endif
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  c_ptr   = &hdrmrlpc.config_result;


  /* Both MFPA and EMPA will propose the following attributes */
  /*---------------------------------------------------------------------------
  Max RLP Flows Attribute
  ---------------------------------------------------------------------------*/
  if ((hdrmrlpc.in_config.max_rlp_flows.max_num_rlp_flows_fwd !=
       hdrmrlpc.desired.max_rlp_flows.max_num_rlp_flows_fwd   )
      ||
      (hdrmrlpc.in_config.max_rlp_flows.max_activated_rlp_flows_fwd !=
       hdrmrlpc.desired.max_rlp_flows.max_activated_rlp_flows_fwd   )
      ||
      (hdrmrlpc.in_config.max_rlp_flows.max_num_rlp_flows_rev !=
       hdrmrlpc.desired.max_rlp_flows.max_num_rlp_flows_rev   )
      ||
      (hdrmrlpc.in_config.max_rlp_flows.max_activated_rlp_flows_rev !=
       hdrmrlpc.desired.max_rlp_flows.max_activated_rlp_flows_rev   )
     )
  {
    c_ptr->max_rlp_flows.at_requested = TRUE;
    hdrmrlpc_pack_creq_max_rlp_flows_attrib(item_ptr, aoffset);
  }

  /*---------------------------------------------------------------------------
  Max Rservations Attribute
  ---------------------------------------------------------------------------*/
  if ((hdrmrlpc.in_config.max_reservations.max_num_rsv_fwd !=
       hdrmrlpc.desired.max_reservations.max_num_rsv_fwd   )
      ||
      (hdrmrlpc.in_config.max_reservations.max_num_open_rsv_fwd !=
       hdrmrlpc.desired.max_reservations.max_num_open_rsv_fwd   )
      ||
      (hdrmrlpc.in_config.max_reservations.max_num_rsv_rev !=
       hdrmrlpc.desired.max_reservations.max_num_rsv_rev   )
      ||
      (hdrmrlpc.in_config.max_reservations.max_num_open_rsv_rev !=
       hdrmrlpc.desired.max_reservations.max_num_open_rsv_rev   )
     )
  {
    c_ptr->max_reservations.at_requested = TRUE;
    hdrmrlpc_pack_creq_max_reservations_attrib(item_ptr, aoffset);
  }

#if defined(FEATURE_HDR_EMPA) || defined (FEATURE_HDR_REVB)
  /* Only for EMPA and MMPA AT will propose the following attributes */
  if ((app == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
      (app == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    /* if RSP is not enabled, there is no need to propose
       two_routes_supported attrib */
#ifdef FEATURE_HDR_SUPPORT_RSP
    #error code not present
#endif
    /*---------------------------------------------------------------------------
    AT Supported Flow Protocol Parameters PP
    ---------------------------------------------------------------------------*/
    for (prot_pp = 0; prot_pp < HDRMRLPC_FLOW_PROT_ID_MAX; prot_pp++)
    {
      if (hdrmrlpc.in_config.at_sup_flow_prot[prot_pp].prot_supported !=
          hdrmrlpc.desired.at_sup_flow_prot[prot_pp].prot_supported   ) 
      {
        c_ptr->prot_parm_pp_attrib[prot_pp].at_sup_flow_prot_parm.at_requested = TRUE;
        hdrmrlpc_pack_creq_at_sup_prot_parm_attrib(item_ptr, aoffset, 
                                   HDRMRLPC_ATTRIB_AT_SUP_FLOW_PROT_PARM, prot_pp);
      }
    }
  
    /*---------------------------------------------------------------------------
    AT Supported Route Protocol Parameters PP
    ---------------------------------------------------------------------------*/
    for (prot_pp = 0; prot_pp < HDRMRLPC_ROUTE_PROT_ID_MAX; prot_pp++)
    {
      if (hdrmrlpc.in_config.at_sup_route_prot[prot_pp].prot_supported !=
          hdrmrlpc.desired.at_sup_route_prot[prot_pp].prot_supported   ) 
      {
        c_ptr->prot_parm_pp_attrib[prot_pp].at_sup_route_prot_parm.at_requested = TRUE;
        hdrmrlpc_pack_creq_at_sup_prot_parm_attrib(item_ptr, aoffset,
                                HDRMRLPC_ATTRIB_AT_SUP_ROUTE_PROT_PARM, prot_pp);
      }
    }

    /* Currently AT does not support flow01 even thought the default is
       that they are enabled. We need propose 0x00 (deactivated). */
    if (hdrmrlpc.in_config.fwd_rlp_config_info[1].activated !=
        HDRMRLPC_FLOW_01_DEACTIVATED_FWD)
    {
      c_ptr->flow_nn_fwd_attrib[1].activated.at_requested = TRUE;
      c_ptr->flow_nn_fwd_attrib[1].activated.value = 
          HDRMRLPC_FLOW_01_DEACTIVATED_FWD;
      /* unconditional turn off flow01activated_fwd/rev*/
      HDRMRLPC_PACK_CON_REQ_ATTRIB_LEN(item_ptr, 
          HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, *aoffset);

      HDRMRLPC_PACK_CON_REQ_ATTRIB_ID(item_ptr,
          HDRMRLPC_FLOW_NN_ACTIVATED_FWD_ATTRIB_ID | (uint16)0x01, *aoffset);

      voffset = HDRMRLPC_MIN_ATTRIB_LEN;

      HDRMRLPC_PACK_CON_REQ_ATTRIB_VALUE(item_ptr, 
          HDRMRLPC_FLOW_01_DEACTIVATED_FWD, *aoffset, voffset);
      *aoffset += HDRMRLPC_SIMPLE_ATTRIB_LEN;
    }

    if (hdrmrlpc.in_config.rev_rlp_config_info[1].activated !=
        HDRMRLPC_FLOW_01_DEACTIVATED_REV)
    {
      c_ptr->flow_nn_rev_attrib[1].activated.at_requested = TRUE;
      c_ptr->flow_nn_rev_attrib[1].activated.value =
          HDRMRLPC_FLOW_01_DEACTIVATED_REV;
      HDRMRLPC_PACK_CON_REQ_ATTRIB_LEN(item_ptr, 
          HDRMRLPC_CON_RSP_ATTRIB_LEN_BYTES, *aoffset);

      HDRMRLPC_PACK_CON_REQ_ATTRIB_ID(item_ptr,
          HDRMRLPC_FLOW_NN_ACTIVATED_REV_ATTRIB_ID | (uint16)0x01, *aoffset);

      voffset = HDRMRLPC_MIN_ATTRIB_LEN;

      HDRMRLPC_PACK_CON_REQ_ATTRIB_VALUE(item_ptr, 
          HDRMRLPC_FLOW_01_DEACTIVATED_REV, *aoffset, voffset);
      *aoffset += HDRMRLPC_SIMPLE_ATTRIB_LEN;
    }
  } /* if EMPA */
#endif /* FEATURE_HDR_EMPA or FEATURE_HDR_REVB*/

} /* hdrmrlpc_pack_config_req_attribs() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_VALIDATE_CONFIG_RSP_ATTRIB

DESCRIPTION
  This function processes the attributes the AN included in its Configuration
  Response Message (AT Init).

DEPENDENCIES
  None

PARAMETERS
  attrib_id - Attribute ID of the attribute to process 
  index     - Protocol_ID for MRLP ProtocolParameterPP attributes
  value_id  - ValueID/Value of the attribute to process
  
RETURN VALUE
  TRUE  - If the attribute is rejected 
  FALSE - otherwise (The attribute is accepted if the valueID matches the one
          the AT proposed.  Including an attribute in a Configuration Response
          that was not proposed in the Request is ground for failure.)

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_validate_config_rsp_attrib
(
  uint16 attrib_id,
  uint8 index,
  uint8 value_id
)
{
  hdrmrlpc_config_req_result_type *c_ptr; /* Convenience Pointer */
  boolean neg_failed;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  c_ptr = &hdrmrlpc.config_result;
  neg_failed = TRUE;

  /* Double check that we support this Protocol ID */
  if (index >= HDRMRLP_MAX_PROT_COUNT)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                   "Protocol%d is not supported (attrib 0x%x)",
                   index, attrib_id | index );
    return neg_failed;
  }

  /* _mpa_  optimize: use some table lookup of some kind instead */
  switch ( attrib_id )
  {
#ifdef FEATURE_HDR_SUPPORT_RSP
    #error code not present
#endif

    case HDRMRLPC_MAX_RLP_FLOWS_ATTRIB_ID: 
      if (c_ptr->max_rlp_flows.at_requested &&
          (value_id == c_ptr->max_rlp_flows.value))
      {
        c_ptr->max_rlp_flows.is_accepted = TRUE;
        hdrmrlpc.verno.max_rlp_flows++;
        neg_failed = FALSE;

        /* Save the attribute into negotiated instance */
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.max_rlp_flows = hdrmrlpc.desired.max_rlp_flows;
#else
        memcpy(&hdrmrlpc.in_config.max_rlp_flows, 
               &hdrmrlpc.desired.max_rlp_flows,
               sizeof(hdrmrlpc_max_rlp_flows_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Storing MaxRLP(Link)Flows attributes");
      }
      else
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                     "MaxRLP(Link)Flow not reqtd or value not accptd");
      }
      break;

    case HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID: 
      if (c_ptr->max_reservations.at_requested &&
          (value_id == c_ptr->max_reservations.value))
      {
        c_ptr->max_reservations.is_accepted = TRUE;
        hdrmrlpc.verno.max_reservations++;
        neg_failed = FALSE;

        /* Save the attribute into negotiated instance */
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.max_reservations = 
                           hdrmrlpc.desired.max_reservations;
#else
        memcpy(&hdrmrlpc.in_config.max_reservations, 
               &hdrmrlpc.desired.max_reservations, 
               sizeof(hdrmrlpc_max_reservations_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "Storing MaxReservation attributes");
      }
      else
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                     "MaxReservation attrib not reqtd or value not accptd");
      }
      break;

    case HDRMRLPC_AT_SUP_FLOW_PROT_PARM_PP_ATTRIB_ID: 
      if (c_ptr->prot_parm_pp_attrib[index].at_sup_flow_prot_parm.at_requested &&
          (value_id == c_ptr->prot_parm_pp_attrib[index].at_sup_flow_prot_parm.value))
      {
        c_ptr->prot_parm_pp_attrib[index].at_sup_flow_prot_parm.is_accepted = TRUE;
        hdrmrlpc.verno.at_sup_flow_prot_parm_pp[index]++;
        neg_failed = FALSE;

        /* Save the attribute into negotiated instance */
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.at_sup_flow_prot[index] = 
                                   hdrmrlpc.desired.at_sup_flow_prot[index];
#else
        memcpy(&hdrmrlpc.in_config.at_sup_flow_prot[index],
               &hdrmrlpc.desired.at_sup_flow_prot[index],
               sizeof(hdrmrlpc_at_sup_prot_parm_pp_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
        HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                     "EMPA: Storing ATSupFlowProtParmPP attrib");
      }
      else
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                     "EMPA: ATSupFlowProtParmPP not reqtd or value unaccptd");
      }
      break;

    case HDRMRLPC_AT_SUP_ROUTE_PROT_PARM_PP_ATTRIB_ID: 
      if (c_ptr->prot_parm_pp_attrib[index].at_sup_route_prot_parm.at_requested &&
          (value_id == c_ptr->prot_parm_pp_attrib[index].at_sup_route_prot_parm.value))
      {
        c_ptr->prot_parm_pp_attrib[index].at_sup_route_prot_parm.is_accepted = TRUE;
        hdrmrlpc.verno.at_sup_route_prot_parm_pp[index]++;
        neg_failed = FALSE;

        /* Save the attribute into negotiated instance */
#ifdef FEATURE_MEMCPY_REMOVAL
        hdrmrlpc.in_config.at_sup_route_prot[index] = 
                        hdrmrlpc.desired.at_sup_route_prot[index];
#else
        memcpy(&hdrmrlpc.in_config.at_sup_route_prot[index],
               &hdrmrlpc.desired.at_sup_route_prot[index],
               sizeof(hdrmrlpc_at_sup_prot_parm_pp_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
        HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                     "EMPA: Storing ATSupRouteProtParmPP attrib");
      }
      else
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                     "EMPA: ATSupRouteProtPP not reqtd or value not accptd");
      }
      break;

#if defined(FEATURE_HDR_EMPA) || defined (FEATURE_HDR_REVB)
    case HDRMRLPC_FLOW_NN_ACTIVATED_FWD_ATTRIB_ID:
      if (c_ptr->flow_nn_fwd_attrib[index].activated.at_requested &&
          (value_id == c_ptr->flow_nn_fwd_attrib[index].activated.value) &&
          (index == 0x01))
      {
        neg_failed = FALSE;
        c_ptr->flow_nn_fwd_attrib[index].activated.is_accepted = TRUE;

        hdrmrlpc.in_config.fwd_rlp_config_info[index].activated = 
          HDRMRLPC_FLOW_01_DEACTIVATED_FWD;
        hdrmrlpc.verno.activated_fwd[index]++;
      }      
      break;

    case HDRMRLPC_FLOW_NN_ACTIVATED_REV_ATTRIB_ID:
      if (c_ptr->flow_nn_rev_attrib[index].activated.at_requested &&
          (value_id == c_ptr->flow_nn_rev_attrib[index].activated.value) &&
          (index == 0x01))
      {
        neg_failed = FALSE;
        c_ptr->flow_nn_rev_attrib[index].activated.is_accepted = TRUE;

        hdrmrlpc.in_config.rev_rlp_config_info[index].activated = 
          HDRMRLPC_FLOW_01_DEACTIVATED_REV;
        hdrmrlpc.verno.activated_rev[index]++;
      }
      break;
#endif

    default:
      /* We should never hit this case! */
      ERR("Invalid MRLP attribute (%d)",attrib_id | index, 0, 0);
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "Invalid MRLP attribute (%d)", attrib_id | index);
      break;

  } /* end switch */

  return neg_failed;

} /* hdrmrlpc_validate_config_rsp_attrib() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRMRLPC_VALIDATE_GAUP_CONFIG_RSP_ATTRIBS

DESCRIPTION
  This function validates the attributes received in the GAUP Attribute
  update reject message.  The AT is not allowed to send an 
  AttributeUpdateReject.  If it encounters an attribute that is unacceptable 
  (for example, if it violates the boundaries defined by the standard), it
  will not send an AttributeUpdateAccept message as a way to recover.
  
DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS

===========================================================================*/
hdrerrno_enum_type hdrmrlpc_validate_gaup_config_rsp_attribs ( void )
{
  uint8 index; 
    /* iterator */

  uint8 rsv_idx;
    /* temp Reservation Label Index */

  hdrmrlpc_flow_nn_fwd_config_attrib_result_type *f_ptr; 
  hdrmrlpc_flow_nn_rev_config_attrib_result_type *r_ptr;
  hdrmrlpc_res_kk_config_attrib_result_type *rsv_ptr;
    /* Convenience pointers */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
    Check for non-GAUP'able attributes.
  ---------------------------------------------------------------------*/
  if (hdrmrlpc.config_result.max_abort_timer_attrib.requested ||
      hdrmrlpc.config_result.ppp_free_authentication_support.requested ||
      hdrmrlpc.config_result.two_routes_supported.requested ||
      hdrmrlpc.config_result.sup_higher_layer_prot_attrib.requested ||
      hdrmrlpc.config_result.max_reservations.requested ||
      hdrmrlpc.config_result.at_nak_delay_supported.requested )
  {
    /* We should never get here (we already check for this in PACC). */
    ERR("AN requested a non GAUPable attribute",0,0,0);
    return E_FAILURE;
  }

  /* Check to make sure that we will be accepted all proposed Attributes
     The AT will not send a response otherwise. */
  if (hdrmrlpc.config_result.an_sup_qos_profiles.requested &&
      !hdrmrlpc.config_result.an_sup_qos_profiles.is_accepted)
  {
    ERR("AN Supported Qos Profile Attribute not accepted for GAUP",0,0,0);
    return E_FAILURE;
  }

  /* Check FlowNN Attributes */
  for (index = 0; index < HDRMRLPC_N_MRLP_MAX; index++)
  {

    f_ptr = &hdrmrlpc.config_result.flow_nn_fwd_attrib[index];

    if ((f_ptr->nak_enable.requested && !f_ptr->nak_enable.is_accepted)  ||
        (f_ptr->higher_layer_prot.requested && 
           !f_ptr->higher_layer_prot.is_accepted)                        ||
        (f_ptr->activated.requested && !f_ptr->activated.is_accepted)    ||
        (f_ptr->simple_sequence_length.requested &&
           !f_ptr->simple_sequence_length.is_accepted)                          ||
        (f_ptr->flow_prot_sdu.requested &&
           !f_ptr->flow_prot_sdu.is_accepted)                          ||
        (f_ptr->data_unit.requested && !f_ptr->data_unit.is_accepted)    ||
        (f_ptr->route_prot_sdu.requested &&
           !f_ptr->route_prot_sdu.is_accepted)                           ||
        (f_ptr->sim_del_both_routes.requested &&
           !f_ptr->sim_del_both_routes.is_accepted)                      ||
        (f_ptr->ooo_del_route_prot.requested &&
           !f_ptr->ooo_del_route_prot.is_accepted)                       ||
        (f_ptr->nak_delay_time.requested &&
           !f_ptr->nak_delay_time.is_accepted)                           ||
        (f_ptr->identification.requested && 
           !f_ptr->identification.is_accepted)                           ||
        (f_ptr->timers.requested && !f_ptr->timers.is_accepted)          ||
        (f_ptr->reservation.requested && !f_ptr->reservation.is_accepted)||
        (f_ptr->flow_prot_parm.requested && 
           !f_ptr->flow_prot_parm.is_accepted)                           ||
        (f_ptr->route_prot_parm.requested &&
           !f_ptr->route_prot_parm.is_accepted)                          ||
        (f_ptr->activated.requested &&
           !f_ptr->activated.is_accepted)                                ||
        (f_ptr->complex_seq_len.requested &&
           !f_ptr->complex_seq_len.is_accepted)
       )
    {
      ERR("Not all Fwd GAUPable FlowNN parameters accepted.",0,0,0);
      return E_FAILURE;
    }

    r_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[index];

    if ((r_ptr->nak_enable.requested && !r_ptr->nak_enable.is_accepted)  ||
        (r_ptr->higher_layer_prot.requested && 
           !r_ptr->higher_layer_prot.is_accepted)                        ||
        /* Treat PhysicalLayerNakEnableRev as a special case. Even
         * though AT doesn't support PhysicalLayerNakEnableRev=1, we
         * don't want to just close the session as per Lucent's request.
         */
        //(r_ptr->phys_layer_nak_enable.requested && 
        //   !r_ptr->phys_layer_nak_enable.is_accepted)                    ||
        (r_ptr->xmit_abort_timer.requested && 
           !r_ptr->xmit_abort_timer.is_accepted)                         ||
        (r_ptr->dos_allowed.requested && !r_ptr->dos_allowed.is_accepted)||
        (r_ptr->activated.requested && !r_ptr->activated.is_accepted)    ||
        (r_ptr->simple_sequence_length.requested &&
           !r_ptr->simple_sequence_length.is_accepted)                          ||
        (r_ptr->flow_prot_sdu.requested &&
           !r_ptr->flow_prot_sdu.is_accepted)                            ||
        (r_ptr->data_unit.requested && !r_ptr->data_unit.is_accepted)    ||
        (r_ptr->route_prot_sdu.requested &&
           !r_ptr->route_prot_sdu.is_accepted)                           ||
        (r_ptr->identification.requested && 
           !r_ptr->identification.is_accepted)                           ||
        (r_ptr->timers.requested && !r_ptr->timers.is_accepted)          ||
        (r_ptr->reservation.requested && !r_ptr->reservation.is_accepted)||
        (r_ptr->flow_prot_parm.requested && 
           !r_ptr->flow_prot_parm.is_accepted)                           ||
        (r_ptr->route_prot_parm.requested &&
           !r_ptr->route_prot_parm.is_accepted)                          ||
        (r_ptr->activated.requested &&
           !r_ptr->activated.is_accepted)
       )
    {
      ERR("Not all Rev GAUPable FlowNN parameters accepted.",0,0,0);
      return E_FAILURE;
    }
  }

  /* Check ReservationKK attributes */
  for (index=0; index < HDRMRLPC_N_RES_LABELS_MAX; index++)
  {
    rsv_ptr = &hdrmrlpc.config_result.res_kk_fwd_attrib[index];

    if ( (rsv_ptr->idle_state.requested && !rsv_ptr->idle_state.is_accepted) ||
         (rsv_ptr->qos_response.requested && !rsv_ptr->qos_response.is_accepted) )
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Not all Fwd GAUPable ResKK parms accepted idx %d",index);
      return E_FAILURE;
    }                            

    rsv_ptr = &hdrmrlpc.config_result.res_kk_rev_attrib[index];

    if ( (rsv_ptr->idle_state.requested && !rsv_ptr->idle_state.is_accepted) ||
         (rsv_ptr->qos_response.requested && !rsv_ptr->qos_response.is_accepted) )
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Not all Rev GAUPable ResKK parms accepted idx %d",index);
      return E_FAILURE;
    }
  }

  /*---------------------------------------------------------------------
  Validate FlowNN GAUPed attributes.
  We've already checked to make sure that the AT was going to accept all 
  proposed parameters. We can copy directly to the "current" instance.
  _mpa_ leaving "negotiated" alone for now.
  ---------------------------------------------------------------------*/
  if (hdrmrlpc.config_result.an_sup_qos_profiles.requested)
  {
    rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
    hdrmrlpc.current.an_sup_qos_profiles = 
                      hdrmrlpc.in_config.an_sup_qos_profiles;
#else
    memcpy(&hdrmrlpc.current.an_sup_qos_profiles,
           &hdrmrlpc.in_config.an_sup_qos_profiles,
           sizeof(hdrmrlpc_supported_qos_profiles));
#endif /* FEATURE_MEMCPY_REMOVAL */
    hdrmrlpc.verno.an_sup_qos_profiles++;

    hdrmrlpc.config_result.an_sup_qos_profiles.requested = FALSE;
    hdrmrlpc.config_result.an_sup_qos_profiles.is_accepted = FALSE;

    rex_leave_crit_sect(&hdrmrlpc.crit_sect);
  }

  for (index = 0; index < HDRMRLPC_N_MRLP_MAX; index++)
  {
    f_ptr = &hdrmrlpc.config_result.flow_nn_fwd_attrib[index];

    /*---------------------------------------------------------------------
    Validate FlowNN Nak Enable Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->nak_enable.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_rlp_config_info[index].nak_enabled = 
       hdrmrlpc.in_config.fwd_rlp_config_info[index].nak_enabled;
      hdrmrlpc.verno.nak_enable_fwd[index]++;

      f_ptr->nak_enable.requested = FALSE;
      f_ptr->nak_enable.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }
    /*---------------------------------------------------------------------
    Validate FlowNN Higher Layer Protocol Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->higher_layer_prot.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_rlp_config_info[index].higher_layer_protocol = 
       hdrmrlpc.in_config.fwd_rlp_config_info[index].higher_layer_protocol;
      hdrmrlpc.verno.higher_layer_prot_fwd[index]++;

      f_ptr->higher_layer_prot.requested = FALSE;
      f_ptr->higher_layer_prot.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Activated Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->activated.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_rlp_config_info[index].activated = 
       hdrmrlpc.in_config.fwd_rlp_config_info[index].activated;
      hdrmrlpc.verno.activated_fwd[index]++;

      f_ptr->activated.requested = FALSE;
      f_ptr->activated.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Sequence Length Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->simple_sequence_length.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_rlp_config_info[index].simple_sequence_length = 
       hdrmrlpc.in_config.fwd_rlp_config_info[index].simple_sequence_length;
      hdrmrlpc.verno.sequence_length_fwd[index]++;

      f_ptr->simple_sequence_length.requested = FALSE;
      f_ptr->simple_sequence_length.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Flow Protocol SDU Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->flow_prot_sdu.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_rlp_config_info[index].flow_prot_sdu = 
       hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_prot_sdu;
      hdrmrlpc.verno.flow_prot_sdu_fwd[index]++;

      f_ptr->flow_prot_sdu.requested = FALSE;
      f_ptr->flow_prot_sdu.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Data Unit Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->data_unit.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_rlp_config_info[index].data_unit = 
       hdrmrlpc.in_config.fwd_rlp_config_info[index].data_unit;
      hdrmrlpc.verno.data_unit_fwd[index]++;

      f_ptr->data_unit.requested = FALSE;
      f_ptr->data_unit.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Route Protocol SDU Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->route_prot_sdu.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_rlp_config_info[index].route_prot_sdu = 
       hdrmrlpc.in_config.fwd_rlp_config_info[index].route_prot_sdu;
      hdrmrlpc.verno.route_prot_sdu_fwd[index]++;

      f_ptr->route_prot_sdu.requested = FALSE;
      f_ptr->route_prot_sdu.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Simultaneous Delivery On Both Routes Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->sim_del_both_routes.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_rlp_config_info[index].sim_del_both_routes = 
       hdrmrlpc.in_config.fwd_rlp_config_info[index].sim_del_both_routes;
      hdrmrlpc.verno.sim_del_both_routes_fwd[index]++;

      f_ptr->sim_del_both_routes.requested = FALSE;
      f_ptr->sim_del_both_routes.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Out Of Order Delivery To Route Protocol Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->ooo_del_route_prot.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_rlp_config_info[index].ooo_del_route_prot = 
       hdrmrlpc.in_config.fwd_rlp_config_info[index].ooo_del_route_prot;
      hdrmrlpc.verno.ooo_del_route_prot_fwd[index]++;

      f_ptr->ooo_del_route_prot.requested = FALSE;
      f_ptr->ooo_del_route_prot.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Nak Delay Time Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->nak_delay_time.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_rlp_config_info[index].nak_delay_time = 
       hdrmrlpc.in_config.fwd_rlp_config_info[index].nak_delay_time;
      hdrmrlpc.verno.timers_fwd[index]++;

      f_ptr->nak_delay_time.requested = FALSE;
      f_ptr->nak_delay_time.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Identification Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->identification.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.fwd_rlp_config_info[index].flow_id = 
                    hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_id;
#else
      memcpy(
       &hdrmrlpc.current.fwd_rlp_config_info[index].flow_id,
       &hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_id,
       sizeof(hdrmrlpc_flow_identification_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.identification_fwd[index]++;

      f_ptr->identification.requested = FALSE;
      f_ptr->identification.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN SequenceLength Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->complex_seq_len.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.fwd_rlp_config_info[index].complex_seq_len = 
          hdrmrlpc.in_config.fwd_rlp_config_info[index].complex_seq_len;
#else
      memcpy(
       &hdrmrlpc.current.fwd_rlp_config_info[index].complex_seq_len,
       &hdrmrlpc.in_config.fwd_rlp_config_info[index].complex_seq_len,
       sizeof(hdrmrlpc_flow_sequence_length_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.sequence_length_fwd[index]++;
      f_ptr->complex_seq_len.requested = FALSE;
      f_ptr->complex_seq_len.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Timers Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->timers.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.fwd_rlp_config_info[index].flow_timer = 
                hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_timer;
#else
      memcpy(
       &hdrmrlpc.current.fwd_rlp_config_info[index].flow_timer,
       &hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_timer,
       sizeof(hdrmrlpc_flow_timers_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.timers_fwd[index]++;
      f_ptr->timers.requested = FALSE;
      f_ptr->timers.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Reservation Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->reservation.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.fwd_rlp_config_info[index].flow_rsv = 
                         hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_rsv;
#else
      memcpy(
       &hdrmrlpc.current.fwd_rlp_config_info[index].flow_rsv,
       &hdrmrlpc.in_config.fwd_rlp_config_info[index].flow_rsv,
       sizeof(hdrmrlpc_flow_reservation_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.reservation_fwd[index]++;
      f_ptr->reservation.requested = FALSE;
      f_ptr->reservation.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Flow Protocol Parameters Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->flow_prot_parm.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.fwd_flow_prot[index] = 
                      hdrmrlpc.in_config.fwd_flow_prot[index];
#else
      memcpy(
        &hdrmrlpc.current.fwd_flow_prot[index],
        &hdrmrlpc.in_config.fwd_flow_prot[index],
        sizeof(hdrmrlpc_flow_prot_parm_fwd_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.flow_prot_parm_fwd[index]++;

      f_ptr->flow_prot_parm.requested = FALSE;
      f_ptr->flow_prot_parm.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Route Protocol Parameters Fwd Attribute
    ---------------------------------------------------------------------*/
    if (f_ptr->route_prot_parm.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.fwd_route_prot[index] = 
                      hdrmrlpc.in_config.fwd_route_prot[index];
#else
      memcpy(
       &hdrmrlpc.current.fwd_route_prot[index],
       &hdrmrlpc.in_config.fwd_route_prot[index],
       sizeof(hdrmrlpc_route_prot_parm_fwd_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.route_prot_parm_fwd[index]++;

      f_ptr->route_prot_parm.requested = FALSE;
      f_ptr->route_prot_parm.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    r_ptr = &hdrmrlpc.config_result.flow_nn_rev_attrib[index];

    /*---------------------------------------------------------------------
    Validate FlowNN Nak Enable Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->nak_enable.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_rlp_config_info[index].nak_enabled =
       hdrmrlpc.in_config.rev_rlp_config_info[index].nak_enabled;

      hdrmrlpc.verno.nak_enable_rev[index]++;
      r_ptr->nak_enable.requested = FALSE;
      r_ptr->nak_enable.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Higher Layer Protocol Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->higher_layer_prot.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_rlp_config_info[index].higher_layer_protocol =
       hdrmrlpc.in_config.rev_rlp_config_info[index].higher_layer_protocol;

      hdrmrlpc.verno.higher_layer_prot_rev[index]++;
      r_ptr->higher_layer_prot.requested = FALSE;
      r_ptr->higher_layer_prot.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Physical Layer Nak Enabled Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->phys_layer_nak_enable.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_rlp_config_info[index].phys_link_nak_enabled_rev =
       hdrmrlpc.in_config.rev_rlp_config_info[index].phys_link_nak_enabled_rev;
      hdrmrlpc.verno.phys_link_nak_en_rev[index]++;
      r_ptr->phys_layer_nak_enable.requested = FALSE;
      r_ptr->phys_layer_nak_enable.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN TransmitAbortTimer Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->xmit_abort_timer.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_rlp_config_info[index].xmit_abort_timer =
       hdrmrlpc.in_config.rev_rlp_config_info[index].xmit_abort_timer;

      hdrmrlpc.verno.xmit_abort_timer_rev[index]++;
      r_ptr->xmit_abort_timer.requested = FALSE;
      r_ptr->xmit_abort_timer.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN DataOverSignallingAllowed Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->dos_allowed.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_rlp_config_info[index].dos_allowed =
       hdrmrlpc.in_config.rev_rlp_config_info[index].dos_allowed;

      hdrmrlpc.verno.dos_allowed_rev[index]++;
      r_ptr->dos_allowed.requested = FALSE;
      r_ptr->dos_allowed.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Activated Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->activated.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_rlp_config_info[index].activated =
       hdrmrlpc.in_config.rev_rlp_config_info[index].activated;

      hdrmrlpc.verno.activated_rev[index]++;
      r_ptr->activated.requested = FALSE;
      r_ptr->activated.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Sequence Length Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->simple_sequence_length.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_rlp_config_info[index].simple_sequence_length =
       hdrmrlpc.in_config.rev_rlp_config_info[index].simple_sequence_length;

      hdrmrlpc.verno.sequence_length_rev[index]++;
      r_ptr->simple_sequence_length.requested = FALSE;
      r_ptr->simple_sequence_length.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Flow Protocol SDU Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->flow_prot_sdu.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_rlp_config_info[index].flow_prot_sdu =
       hdrmrlpc.in_config.rev_rlp_config_info[index].flow_prot_sdu;

      hdrmrlpc.verno.flow_prot_sdu_rev[index]++;
      r_ptr->flow_prot_sdu.requested = FALSE;
      r_ptr->flow_prot_sdu.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Data Unit Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->data_unit.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_rlp_config_info[index].data_unit =
       hdrmrlpc.in_config.rev_rlp_config_info[index].data_unit;

      hdrmrlpc.verno.data_unit_rev[index]++;
      r_ptr->data_unit.requested = FALSE;
      r_ptr->data_unit.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Route Protocol SDU Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->route_prot_sdu.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_rlp_config_info[index].route_prot_sdu =
       hdrmrlpc.in_config.rev_rlp_config_info[index].route_prot_sdu;

      hdrmrlpc.verno.route_prot_sdu_rev[index]++;
      r_ptr->route_prot_sdu.requested = FALSE;
      r_ptr->route_prot_sdu.is_accepted = FALSE;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Identification Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->identification.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.rev_rlp_config_info[index].flow_id = 
                   hdrmrlpc.in_config.rev_rlp_config_info[index].flow_id;
#else
      memcpy(
       &hdrmrlpc.current.rev_rlp_config_info[index].flow_id,
       &hdrmrlpc.in_config.rev_rlp_config_info[index].flow_id,
       sizeof(hdrmrlpc_flow_identification_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.identification_rev[index]++;
      r_ptr->identification.requested = FALSE;
      r_ptr->identification.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Timers Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->timers.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.rev_rlp_config_info[index].flow_timer = 
                     hdrmrlpc.in_config.rev_rlp_config_info[index].flow_timer;
#else
      memcpy(
       &hdrmrlpc.current.rev_rlp_config_info[index].flow_timer,
       &hdrmrlpc.in_config.rev_rlp_config_info[index].flow_timer,
       sizeof(hdrmrlpc_flow_timers_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.timers_rev[index]++;
      r_ptr->timers.requested = FALSE;
      r_ptr->timers.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Reservation Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->reservation.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.rev_rlp_config_info[index].flow_rsv = 
                   hdrmrlpc.in_config.rev_rlp_config_info[index].flow_rsv;
#else
      memcpy(
       &hdrmrlpc.current.rev_rlp_config_info[index].flow_rsv,
       &hdrmrlpc.in_config.rev_rlp_config_info[index].flow_rsv,
       sizeof(hdrmrlpc_flow_reservation_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.reservation_rev[index]++;
      r_ptr->reservation.requested = FALSE;
      r_ptr->reservation.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Flow Protocol Parameters Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->flow_prot_parm.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.rev_flow_prot[index] = 
                      hdrmrlpc.in_config.rev_flow_prot[index];
#else
      memcpy(
       &hdrmrlpc.current.rev_flow_prot[index],
       &hdrmrlpc.in_config.rev_flow_prot[index],
       sizeof(hdrmrlpc_flow_prot_parm_rev_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.flow_prot_parm_rev[index]++;
      r_ptr->flow_prot_parm.requested = FALSE;
      r_ptr->flow_prot_parm.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate FlowNN Route Protocol Parameters Rev Attribute
    ---------------------------------------------------------------------*/
    if (r_ptr->route_prot_parm.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.rev_route_prot[index] = 
                    hdrmrlpc.in_config.rev_route_prot[index];
#else
      memcpy(
       &hdrmrlpc.current.rev_route_prot[index],
       &hdrmrlpc.in_config.rev_route_prot[index],
       sizeof(hdrmrlpc_route_prot_parm_rev_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.route_prot_parm_rev[index]++;
      r_ptr->route_prot_parm.requested = FALSE;
      r_ptr->route_prot_parm.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }
  } /* end for: rlp flows */

  for (index = 0; index < HDRMRLPC_N_RES_LABELS_MAX; index++)
  {
    rsv_ptr = &hdrmrlpc.config_result.res_kk_fwd_attrib[index];

    /* Copy the reservation label index */
    if (rsv_ptr->idle_state.requested ||
        rsv_ptr->qos_response.requested)
    {

      (void)hdrmrlpc_map_cur_rl(hdrmrlpc.in_config.fwd_res_info[index].rsv_label, 
                                HDRMRLPC_FWD_RLP,&rsv_idx);

      hdrmrlpc.current.fwd_res_info[rsv_idx].rsv_label = 
        hdrmrlpc.in_config.fwd_res_info[index].rsv_label;
    }
    /*lint --e{644} suppress rsv_idx not initialized */
    /*---------------------------------------------------------------------
    Validate ReservationKK Idle State Fwd Attribute
    ---------------------------------------------------------------------*/
    if (rsv_ptr->idle_state.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.fwd_res_info[rsv_idx].idle_state =
       hdrmrlpc.in_config.fwd_res_info[index].idle_state;
      hdrmrlpc.verno.reskk_idle_state_fwd[rsv_idx]++;
      rsv_ptr->idle_state.requested = FALSE;
      rsv_ptr->idle_state.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate ReservationKK QoS Profile Fwd Attribute
    ---------------------------------------------------------------------*/
    if (rsv_ptr->qos_response.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.fwd_res_info[rsv_idx].qos_response = 
                    hdrmrlpc.in_config.fwd_res_info[index].qos_response;
#else
      memcpy(
       &hdrmrlpc.current.fwd_res_info[rsv_idx].qos_response,
       &hdrmrlpc.in_config.fwd_res_info[index].qos_response,
       sizeof(hdrmrlpc_res_qos_profile_config_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.reskk_qos_rsp_fwd[rsv_idx]++;
      rsv_ptr->qos_response.requested = FALSE;
      rsv_ptr->qos_response.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    rsv_ptr = &hdrmrlpc.config_result.res_kk_rev_attrib[index];
    
    /* Copy the reservation label index */
    if (rsv_ptr->idle_state.requested ||
        rsv_ptr->qos_response.requested)
    {
      (void)hdrmrlpc_map_cur_rl(hdrmrlpc.in_config.rev_res_info[index].rsv_label, 
                                HDRMRLPC_REV_RLP,&rsv_idx);

      hdrmrlpc.current.rev_res_info[rsv_idx].rsv_label = 
        hdrmrlpc.in_config.rev_res_info[index].rsv_label;
    }

    /*---------------------------------------------------------------------
    Validate ReservationKK Idle State Rev Attribute
    ---------------------------------------------------------------------*/
    if (rsv_ptr->idle_state.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc.current.rev_res_info[rsv_idx].idle_state =
       hdrmrlpc.in_config.rev_res_info[index].idle_state;
      hdrmrlpc.verno.reskk_idle_state_rev[rsv_idx]++;
      rsv_ptr->idle_state.requested = FALSE;
      rsv_ptr->idle_state.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

    /*---------------------------------------------------------------------
    Validate ReservationKK QoS Profile Rev Attribute
    ---------------------------------------------------------------------*/
    if (rsv_ptr->qos_response.requested)
    {
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.current.rev_res_info[rsv_idx].qos_response = 
                    hdrmrlpc.in_config.rev_res_info[index].qos_response;
#else
      memcpy(
       &hdrmrlpc.current.rev_res_info[rsv_idx].qos_response,
       &hdrmrlpc.in_config.rev_res_info[index].qos_response,
       sizeof(hdrmrlpc_res_qos_profile_config_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

      hdrmrlpc.verno.reskk_qos_rsp_rev[rsv_idx]++;
      rsv_ptr->qos_response.requested = FALSE;
      rsv_ptr->qos_response.is_accepted = FALSE;
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    }

  }

  hdrscmdb_set_gauped_config(HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL, 
                             &hdrmrlpc.current);

  return E_SUCCESS;

} /* hdrmrlpc_validate_gaup_config_rsp_attribs() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRMRLPC_GAUP_TIMER_SEARCH

DESCRIPTION
  This function search the list of pending gaup timers based on trans_id
    
DEPENDENCIES
  None

PARAMETERS
  trans_id - Transaction ID of the message we received a response for.

RETURN VALUE  
  if return value == hdrmrlpc.gaup_timer_cnt, no entry found
  otherwise, returns index into the timer table

SIDE EFFECTS
  None

===========================================================================*/
uint8 hdrmrlpc_gaup_timer_search 
(
  uint8 trans_id
)
{
  uint8 timer_idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (timer_idx = 0; timer_idx < hdrmrlpc.gaup_timer_cnt; timer_idx++)
  {
    if (hdrmrlpc.gaup_timer_list[timer_idx].trans_id == trans_id)
    {
      break;
    }
  }
  /* timer_idx is the index to the entry with the sought-after trans_id, 
     or, if timer_idx >= hdrmrlpc.gaup_timer_cnt, no entry was found. */
  HDR_MSG_PROT_3(MSG_LEGACY_HIGH,
    "search GAUP timer trans_id=%d, q_size=%d, found=%d",
    trans_id, hdrmrlpc.gaup_timer_cnt, 
    (timer_idx < hdrmrlpc.gaup_timer_cnt));

  return timer_idx;
} /* hdrmrlpc_gaup_timer_search() */

/* EJECT */
/*===========================================================================
FUNCTION HDRMRLPC_GAUP_TIMER_START

DESCRIPTION
  This function starts the Tturnaround timer for an AttributeUpdateRequest
  message.  If the timer is already running, then we are already waiting
  for a response for a previously queued AttributeUpdateRequest message.
  We just save the time by which we would have to restart the timer with 
  when the timer for the oldest AttributeUpdateRequest message expires.
    
DEPENDENCIES
  None

PARAMETERS
  trans_id - transaction ID of the GAUP message the AT is waiting a response
             for.
  timer_interval - intended timer interval in mS. If set to 0xFFFFFFFF, the 
             timer is placed in gaup_tmer_list[] but not started.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpc_gaup_timer_start 
(
  uint8 trans_id,
  boolean pending_ack
)
{
  uint8 timer_idx = 0;            /* Index into the timer table */
  uint32 timer_interval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(hdrmrlpc.gaup_timer_cnt < HDRMRLPC_MAX_TOTAL_RSV);

  HDR_MSG_PROT_3(MSG_LEGACY_MED,
    "GAUP timer start, trans_id=%d, pending=%d, q_size=%d",
    trans_id, pending_ack, 
    hdrmrlpc.gaup_timer_cnt);

  /* When AT attempts to send GAUP, it inserts a timer into the GAUP timer
     table, but do not start it, it is then started in the xmit_outcome_cb().
     The reason is when GAUP is sent over RTC, AN may responde with 
     GAUPReject/Accept before the xmit_outcome_cb() is called.
     Search for the trans_id to see if it exists */

  if (pending_ack)
  {
    timer_idx = hdrmrlpc.gaup_timer_cnt;
    timer_interval = 0xFFFFFFFF;
    hdrmrlpc.gaup_timer_cnt++;
  }
  else
  {
    timer_idx = hdrmrlpc_gaup_timer_search(trans_id);
    timer_interval = HDRMRLPC_ATTRIB_UPDATE_RSP_TIMER_MS - 
        rex_get_timer(&hdrmrlpc.gaup_rsp_timer);
  }

  if (timer_idx >= hdrmrlpc.gaup_timer_cnt)
  {
    return;
  }

  hdrmrlpc.gaup_timer_list[timer_idx].trans_id = trans_id;
  hdrmrlpc.gaup_timer_list[timer_idx].time_left_ms = timer_interval;
  
  /* Gaup rsp timer isn't running. Start it. First running timer.
     If it is running, then wait until it expires or the AT receives a gaup 
     response to update it. */
  if ((!pending_ack) && (!rex_get_timer(&hdrmrlpc.gaup_rsp_timer)))
  {
    HDR_MSG_PROT_2( MSG_LEGACY_MED, "Current GAUP timer @%d, (TID %d)",
                    timer_idx, trans_id );

    hdrmrlpc.gaup_timer_trans_id = trans_id;
    /* time_left_ms for the current running timer is useless. 
       Set 0 to make this field look consistant. */
    hdrmrlpc.gaup_timer_list[timer_idx].time_left_ms = 0;
    (void) rex_set_timer(&hdrmrlpc.gaup_rsp_timer, timer_interval);
  }

} /* hdrmrlpc_gaup_timer_start() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRMRLPC_GAUP_TIMER_STOP

DESCRIPTION
  This function stops the 
    
DEPENDENCIES
  None

PARAMETERS
  trans_id - Transaction ID of the message we received a response for.

RETURN VALUE
  True - found trans_id in the list of gaup messages pending response
  False - trans_id not found, possible due to timer expire and deleted

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_gaup_timer_stop 
(
  uint8 trans_id
)
{
  uint8 timer_idx, next_timer_idx = 0, i = 0; /* iterators */

  boolean is_current_timer = TRUE;
    /* Whether the timer to be deleted is the current running timer */
  boolean next_timer = FALSE;
    /* Whether we need to restart the timer */

  rex_timer_cnt_type new_timer_interval_ms = 0;    
    /* new interval (in msec) to restart the n-gaup rsp timer by. */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the timer list */
  rex_enter_crit_sect(&hdrmrlpc.crit_sect);

  HDR_MSG_PROT_2(MSG_LEGACY_MED,
    "GAUP timer stop, trans_id=%d, q_size=%d",
    trans_id, hdrmrlpc.gaup_timer_cnt);

  timer_idx = hdrmrlpc_gaup_timer_search(trans_id);

  if (timer_idx >= hdrmrlpc.gaup_timer_cnt)
  {
    rex_leave_crit_sect(&hdrmrlpc.crit_sect);
    return FALSE;
  }

  /* The GAUP timer is always running for the oldest message that is not 
     pending ACK. If the response came in for the oldest message, then
     the time intervals of each entry need to be recomputed based on the
     new timer we are about to start. If not, then no updates are needed. */

  /* If timer to be deleted is the first running timer, find the next
     running timer */
  if ( trans_id == hdrmrlpc.gaup_timer_trans_id )
  {
    /* Any valid running timer has
       time_left_ms <= HDRMRLPC_ATTRIB_UPDATE_RSP_TIMER_MS. */
    new_timer_interval_ms = HDRMRLPC_ATTRIB_UPDATE_RSP_TIMER_MS + 1;

    for (i = 0; i < hdrmrlpc.gaup_timer_cnt; i++)
    {
      if ( (i != timer_idx) &&
           (hdrmrlpc.gaup_timer_list[i].time_left_ms != 0xFFFFFFFF) &&
           (hdrmrlpc.gaup_timer_list[i].time_left_ms < new_timer_interval_ms) )
      {
        next_timer_idx = i;
        new_timer_interval_ms = hdrmrlpc.gaup_timer_list[i].time_left_ms;
        hdrmrlpc.gaup_timer_trans_id = hdrmrlpc.gaup_timer_list[i].trans_id;
      }
    }

    if (new_timer_interval_ms <= HDRMRLPC_ATTRIB_UPDATE_RSP_TIMER_MS)
    {
      /* The current to be deleted is the first running time; and the
         second running timer exists. */
      next_timer = TRUE;
      HDR_MSG_PROT_4( MSG_LEGACY_HIGH,
        "Running GAUP timer @%d (TID %d) deleted, new timer @%d (TID %d) started.",
        timer_idx, trans_id, 
        next_timer_idx, hdrmrlpc.gaup_timer_trans_id );
    }
    else 
    {
      hdrmrlpc.gaup_timer_trans_id = 0xFF;
      HDR_MSG_PROT_2( MSG_LEGACY_MED,
        "Running GAUP timer @%d (TID %d) deleted, no new timer started.",
        timer_idx, trans_id);
    }
  }
  else 
  {
    is_current_timer = FALSE;
    HDR_MSG_PROT_2( MSG_LEGACY_MED,
      "GAUP timer @%d (TID %d) deleted is not the current running timer.",
      timer_idx, trans_id );
  }

  hdrmrlpc.gaup_timer_cnt--;     

  for (i = 0; i < hdrmrlpc.gaup_timer_cnt; i++)
  {
    if (i == timer_idx )
    { 
      /* We do not care about the order of the timer entry, replace the
         entry to be removed with the last entry */
      hdrmrlpc.gaup_timer_list[i].trans_id = 
          hdrmrlpc.gaup_timer_list[hdrmrlpc.gaup_timer_cnt].trans_id;
      hdrmrlpc.gaup_timer_list[i].time_left_ms = 
          hdrmrlpc.gaup_timer_list[hdrmrlpc.gaup_timer_cnt].time_left_ms;
    }

    /* Update the running time only */
    if ( next_timer && 
         (hdrmrlpc.gaup_timer_list[i].time_left_ms != 0xFFFFFFFF) )
    {
      hdrmrlpc.gaup_timer_list[i].time_left_ms -= new_timer_interval_ms;
    }
  }

  /* Reset the last element */
  hdrmrlpc.gaup_timer_list[i].trans_id = 0;
  hdrmrlpc.gaup_timer_list[i].time_left_ms = 0;

  rex_leave_crit_sect(&hdrmrlpc.crit_sect);

  /* Only the current running timer is deleted, there is a need to
     restart or clear the timer. */
  if ( is_current_timer )
  {
    /* Reset timer only if there is a second running timer.
       If we have computed a new timer interval, restart the timer. If there is 
       still time left on the timer, make sure to add it to our new timer 
       interval before restarting the timer. */
    if ( next_timer )
    {
      new_timer_interval_ms += rex_get_timer(&hdrmrlpc.gaup_rsp_timer);
  
      (void)rex_set_timer(&hdrmrlpc.gaup_rsp_timer, new_timer_interval_ms);
    }
    else
    {
      (void)rex_clr_timer(&hdrmrlpc.gaup_rsp_timer);
    }
  }

  /* Abort the pending GAUP message because AT received
   * L3 ACK before L2 ACK                  
   */
  (void) hdrhmp_abort_msg(hdrmrlpc_gaup_req_tx_outcome_cb, 
                   (void*) ((dword) trans_id));

  return TRUE;

} /* hdrmrlpc_gaup_timer_stop() */

/* <EJECT> */
/*=============================================================================
FUNCTION HDRMRLPC_PROCESS_ATTRIB_UPDATE_RESPONSE

DESCRIPTION
  This function processes all outcomes associated with responses to our 
  AttributeUpdateAccept message for the Multiflow packet app.
    
DEPENDENCIES
  None

PARAMETERS
  trans_id  - TransactionID of the response we are handling.
  ind_name  - Indication to give, also signifies the type of response
              (accepted, rejected, failed)
   
    
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrmrlpc_process_attrib_update_response
(
  uint8 trans_id,
  hdrind_ind_name_enum_type ind_name
)
{
  uint8 index; /* iterator */
  uint8 rsv_idx = 0; /* Reservation Index mapping in the current config */
  hdrind_ind_data_union_type ind_data; /* Indication Payload */

  hdrmrlpc_config_attrib_result_type *cfg_ptr;   /* Convenience pointer */
  uint8 i;
  hdrmrlpc_pending_gaup_req_type *pending_gaup_req_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(HDRMRLPC_N_RES_LABELS_MAX_FWD == HDRMRLPC_N_RES_LABELS_MAX_REV);


  /* Stop the timer corresponding to this transaction ID.  Either we've 
     received a response for the AttributeUpdateRequest message bearing 
     this trans ID or we timed out waiting for one. */
  if (!hdrmrlpc_gaup_timer_stop(trans_id) ) 
  {
    if (ind_name != HDRIND_SCM_MRLP_AT_GAUP_FAILED)
    {
      /* if the trans_id is not found in the list of gaup msgs pending response, 
         it must've been processed already, either when response is received or
         when timer expired. No need to send ind. again*/
      return;
    }
    else
    {
      /* If ind_name == GAUP_FAILED and we didn't find gaup timer for this trans_id,
       * it means GAUP_FAILED is due to SLP failed.
       * In this case, we need to continue processing GAUP_FAILED ind
       */
    }
  }

  switch (ind_name)
  {
    case HDRIND_SCM_MRLP_AT_GAUP_ACCEPTED:
      /*---------------------------------------------------------------------
      AttributeUpdateRequest succeeded. Update the current config.
      ---------------------------------------------------------------------*/

      /* Forward */
      for (index=0; index < HDRMRLPC_N_RES_LABELS_MAX; index++)
      {
        /* Forward ReservationKKQosRequest attributes */
        cfg_ptr = &hdrmrlpc.config_result.res_kk_fwd_attrib[index].qos_request;
   
        if (cfg_ptr->at_requested && cfg_ptr->value == trans_id)
        {
          rex_enter_crit_sect(&hdrmrlpc.crit_sect);

          if (hdrmrlpc.in_config.fwd_res_info[index].qos_request.type ==
              HDRMRLPC_RES_QOS_PROFILE_CONFIG_TYPE_NONE )
          {
            /* AT sends the Res QOS profile type NONE to AN to 
             * release the QoS, and AN accepts it.
             * Find the index in the current structure, and remove
             * the entry from there. Then move all the entries
             * from i+1 to i */
            HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                           "AN acks AT to release QoS Fwd, idx %d", index);

            if ( hdrmrlpc_get_cur_rl_idx(
                   hdrmrlpc.in_config.fwd_res_info[index].rsv_label,
                   HDRMRLPC_FWD_RLP, &rsv_idx) == E_SUCCESS )
            {
              HDR_MSG_PROT_1(MSG_LEGACY_MED,
                             "Remove QoS req from cur table idx %d", rsv_idx);

              ASSERT(hdrmrlpc.current.num_fwd_rsv <= HDRMRLPC_N_RES_LABELS_MAX_FWD); 
              ASSERT(rsv_idx < HDRMRLPC_N_RES_LABELS_MAX_FWD);
                /* Array boundary sanity check */
 
              /* If get_cur_rl_idx returns E_SUCCESS, 
               * num_fwd_rsv will be at least 1 */
              for ( i = rsv_idx; i < hdrmrlpc.current.num_fwd_rsv - 1; i++ )
              {
                hdrmrlpc.current.fwd_res_info[i] = 
                  hdrmrlpc.current.fwd_res_info[i+1];

                /* Also remove the verno table */
                hdrmrlpc.verno.reskk_idle_state_fwd[i] = 
                  hdrmrlpc.verno.reskk_idle_state_fwd[i+1];
                hdrmrlpc.verno.reskk_qos_req_fwd[i] = 
                  hdrmrlpc.verno.reskk_qos_req_fwd[i+1];
                hdrmrlpc.verno.reskk_qos_rsp_fwd[i] = 
                  hdrmrlpc.verno.reskk_qos_rsp_fwd[i+1];
              }

              /* Clear the rest of the table entries */
              memset(&hdrmrlpc.current.fwd_res_info[i], 0,
                     sizeof(hdrmrlpc_reservation_kk_config_type) *
                     ( HDRMRLPC_N_RES_LABELS_MAX_FWD - i ));
              
              memset(&hdrmrlpc.verno.reskk_idle_state_fwd[i], 0,
                     ( HDRMRLPC_N_RES_LABELS_MAX_FWD - i ));
              memset(&hdrmrlpc.verno.reskk_qos_req_fwd[i], 0,
                     ( HDRMRLPC_N_RES_LABELS_MAX_FWD - i ));
              memset(&hdrmrlpc.verno.reskk_qos_rsp_fwd[i], 0,
                     ( HDRMRLPC_N_RES_LABELS_MAX_FWD - i ));

              hdrmrlpc.current.num_fwd_rsv --;
            }
          }
          else
          {
            /* If the GAUPed ReservationKK attribute was not in the current 
               table yet, add it */
            (void)hdrmrlpc_map_cur_rl(hdrmrlpc.in_config.fwd_res_info[index].rsv_label,
                                      HDRMRLPC_FWD_RLP, &rsv_idx);

#ifdef FEATURE_MEMCPY_REMOVAL
            hdrmrlpc.current.fwd_res_info[rsv_idx].qos_request = 
                     hdrmrlpc.in_config.fwd_res_info[index].qos_request;
#else
            memcpy(
              &hdrmrlpc.current.fwd_res_info[rsv_idx].qos_request,
              &hdrmrlpc.in_config.fwd_res_info[index].qos_request,
              sizeof(hdrmrlpc_res_qos_profile_config_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

            hdrmrlpc.verno.reskk_qos_req_fwd[rsv_idx]++;

            cfg_ptr->at_requested = FALSE;
            cfg_ptr->value = 0;
          }
          rex_leave_crit_sect(&hdrmrlpc.crit_sect);
        } /* if type == 0 */
      } /* for */

      /* Now handling the in_config and config_result structure */
      index = 0;
      ASSERT(hdrmrlpc.in_config.num_fwd_rsv <= HDRMRLPC_N_RES_LABELS_MAX_FWD);

      while ( index < hdrmrlpc.in_config.num_fwd_rsv )
      {
        cfg_ptr = &hdrmrlpc.config_result.res_kk_fwd_attrib[index].qos_request;

        if (cfg_ptr->at_requested && cfg_ptr->value == trans_id &&
            ( hdrmrlpc.in_config.fwd_res_info[index].qos_request.type ==
              HDRMRLPC_RES_QOS_PROFILE_CONFIG_TYPE_NONE ))
        {
          HDR_MSG_PROT_1(MSG_LEGACY_LOW,
                         "Remove QoS req from in_cfg table idx %d", index);

          rex_enter_crit_sect(&hdrmrlpc.crit_sect);

          /* AT sends the Res QOS profile type NONE to AN to 
           * release the QoS, and AN accepts it.
           * Remove the entry from the in_config and config_result
           * table, and move all the entries from i+1 to i */
          /* num_fwd_rsv will be at least 1, so no overflow here */
            for ( i = index; i < hdrmrlpc.in_config.num_fwd_rsv - 1; i++ )
            {
              hdrmrlpc.in_config.fwd_res_info[i] =
                hdrmrlpc.in_config.fwd_res_info[i+1];

              hdrmrlpc.config_result.res_kk_fwd_attrib[i] =
                hdrmrlpc.config_result.res_kk_fwd_attrib[i+1];
            }

            /* Clear the remaining memory */
            memset(&hdrmrlpc.in_config.fwd_res_info[i], 0,
                   sizeof(hdrmrlpc_reservation_kk_config_type) *
                     ( HDRMRLPC_N_RES_LABELS_MAX_FWD - i ));
            memset(&hdrmrlpc.config_result.res_kk_fwd_attrib[i], 0, 
                   sizeof(hdrmrlpc_res_kk_config_attrib_result_type) *
                     ( HDRMRLPC_N_RES_LABELS_MAX_FWD - i ));

          /* Decrement the num_fwd_rsv. */
          hdrmrlpc.in_config.num_fwd_rsv --;
          hdrmrlpc.config_result.num_rsv_fwd--;

          rex_leave_crit_sect(&hdrmrlpc.crit_sect);
        }
        else
        {
          /* Increment the index */
          index++;
        }
      }

      /* Reverse */
      for (index=0; index < HDRMRLPC_N_RES_LABELS_MAX; index++)
      {
        cfg_ptr = &hdrmrlpc.config_result.res_kk_rev_attrib[index].qos_request;

        /* Reverse ReservationKKQoSRequest attributes */
        if (cfg_ptr->at_requested && cfg_ptr->value == trans_id)
        {
          rex_enter_crit_sect(&hdrmrlpc.crit_sect);

          if (hdrmrlpc.in_config.rev_res_info[index].qos_request.type ==
              HDRMRLPC_RES_QOS_PROFILE_CONFIG_TYPE_NONE )
          {
            /* AT sends the Res QOS profile type NONE to AN to 
             * release the QoS, and AN accepts it. */
            /* Find the index in the current structure, and remove
             * the entry from there. Then move all the entries
             * from i+1 to i */
            HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                           "AN acks AT to release QoS Rev, idx %d", index);

            if ( hdrmrlpc_get_cur_rl_idx(
                   hdrmrlpc.in_config.rev_res_info[index].rsv_label,
                   HDRMRLPC_REV_RLP, &rsv_idx) == E_SUCCESS )
            {
              HDR_MSG_PROT_1(MSG_LEGACY_MED,
                             "Remove QoS req from cur table idx %d", rsv_idx);

              ASSERT(hdrmrlpc.current.num_rev_rsv <= HDRMRLPC_N_RES_LABELS_MAX_REV); 
              ASSERT(rsv_idx < HDRMRLPC_N_RES_LABELS_MAX_REV);
                /* Array boundary sanity check */

              /* If get_cur_rl_idx returns E_SUCCESS, 
               * num_rev_rsv will be at least 1 */
              for ( i = rsv_idx; i < hdrmrlpc.current.num_rev_rsv - 1; i++)
              {
                hdrmrlpc.current.rev_res_info[i] =
                  hdrmrlpc.current.rev_res_info[i+1];

                /* Also remove the verno table */
                hdrmrlpc.verno.reskk_idle_state_rev[i] = 
                  hdrmrlpc.verno.reskk_idle_state_rev[i+1];
                hdrmrlpc.verno.reskk_qos_req_rev[i] = 
                  hdrmrlpc.verno.reskk_qos_req_rev[i+1];
                hdrmrlpc.verno.reskk_qos_rsp_rev[i] = 
                  hdrmrlpc.verno.reskk_qos_rsp_rev[i+1];
              }

              /* Clean the rest of table entries */
              memset(&hdrmrlpc.current.rev_res_info[i], 0, 
                     sizeof(hdrmrlpc_reservation_kk_config_type)*
                     ( HDRMRLPC_N_RES_LABELS_MAX_REV - i ));

              memset(&hdrmrlpc.verno.reskk_idle_state_rev[i], 0,
                     ( HDRMRLPC_N_RES_LABELS_MAX_REV - i ));
              memset(&hdrmrlpc.verno.reskk_qos_req_rev[i], 0,
                     ( HDRMRLPC_N_RES_LABELS_MAX_REV - i ));
              memset(&hdrmrlpc.verno.reskk_qos_rsp_rev[i], 0,
                     ( HDRMRLPC_N_RES_LABELS_MAX_REV - i ));

              hdrmrlpc.current.num_rev_rsv --;
            }
          }
          else
          {
            /* If the GAUPed ReservationKK attribute was not in the current 
               table yet, add it */
            (void)hdrmrlpc_map_cur_rl(hdrmrlpc.in_config.rev_res_info[index].rsv_label,
                                      HDRMRLPC_REV_RLP, &rsv_idx);

#ifdef FEATURE_MEMCPY_REMOVAL
            hdrmrlpc.current.rev_res_info[rsv_idx].qos_request = 
                       hdrmrlpc.in_config.rev_res_info[index].qos_request;
#else
            memcpy(
              &hdrmrlpc.current.rev_res_info[rsv_idx].qos_request,
              &hdrmrlpc.in_config.rev_res_info[index].qos_request,
              sizeof(hdrmrlpc_res_qos_profile_config_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

            hdrmrlpc.verno.reskk_qos_req_rev[rsv_idx]++;

            cfg_ptr->at_requested = FALSE;
            cfg_ptr->value = 0;
          }
          rex_leave_crit_sect(&hdrmrlpc.crit_sect);
        } /* if type == 0 */
      } /* for */

      /* Now handling the in_config and config_result structure */
      index = 0;

      ASSERT(hdrmrlpc.in_config.num_rev_rsv <= HDRMRLPC_N_RES_LABELS_MAX_REV);

      while ( index < hdrmrlpc.in_config.num_rev_rsv )
      {
        cfg_ptr = &hdrmrlpc.config_result.res_kk_rev_attrib[index].qos_request;

        if (cfg_ptr->at_requested && cfg_ptr->value == trans_id &&
            ( hdrmrlpc.in_config.rev_res_info[index].qos_request.type ==
              HDRMRLPC_RES_QOS_PROFILE_CONFIG_TYPE_NONE ))
        {
          HDR_MSG_PROT_1(MSG_LEGACY_LOW,
                         "Remove QoS req from in_cfg table idx %d", index);

          rex_enter_crit_sect(&hdrmrlpc.crit_sect);

          /* AT sends the Res QOS profile type NONE to AN to 
           * release the QoS, and AN accepts it.
           * Remove the entry from the in_config and config_result
           * table, and move all the entries from i+1 to i */
          for ( i = index; i < hdrmrlpc.in_config.num_rev_rsv - 1; i++ )
          {
            hdrmrlpc.in_config.rev_res_info[i] =
              hdrmrlpc.in_config.rev_res_info[i+1],

            hdrmrlpc.config_result.res_kk_rev_attrib[i] =
              hdrmrlpc.config_result.res_kk_rev_attrib[i+1];
          }

          /* Clear the remaining memory */
          memset(&hdrmrlpc.in_config.rev_res_info[i], 0,
                 sizeof(hdrmrlpc_reservation_kk_config_type) *
                   ( HDRMRLPC_N_RES_LABELS_MAX_REV - i ));
          memset(&hdrmrlpc.config_result.res_kk_rev_attrib[i], 0,
                 sizeof(hdrmrlpc_res_kk_config_attrib_result_type) *
                   ( HDRMRLPC_N_RES_LABELS_MAX_REV - i ));

          /* Decrement the num_rev_rsv. */
          hdrmrlpc.in_config.num_rev_rsv --;
          hdrmrlpc.config_result.num_rsv_fwd --;

          rex_leave_crit_sect(&hdrmrlpc.crit_sect);
        }
        else
        {
          /* Increment the index */
          index++;
        }
      }

      hdrscmdb_set_gauped_config(HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL,
                                 &hdrmrlpc.current);

      break;
    
    case HDRIND_SCM_MRLP_AT_GAUP_REJECTED:
    case HDRIND_SCM_MRLP_AT_GAUP_FAILED:
      /*---------------------------------------------------------------------
      AttributeUpdateRequest failed.  Mark attributes as "not requested".
      (It is not necessary to reset the "in_config" values for AT Initiated
       GAUP).
      ---------------------------------------------------------------------*/
      for (index=0; index < HDRMRLPC_N_RES_LABELS_MAX; index++)
      {
        /* Forward ReservationKKQosRequest attributes */
        cfg_ptr = &hdrmrlpc.config_result.res_kk_fwd_attrib[index].qos_request;
   
        if (cfg_ptr->at_requested && cfg_ptr->value == trans_id)
        {
          cfg_ptr->at_requested = FALSE;
          cfg_ptr->value = 0;
        }
 
        cfg_ptr = &hdrmrlpc.config_result.res_kk_rev_attrib[index].qos_request;

        /* Reverse ReservationKKQoSRequest attributes */
        if (cfg_ptr->at_requested && cfg_ptr->value == trans_id)
        {
          cfg_ptr->at_requested = FALSE;
          cfg_ptr->value = 0;
        }
      }
      break;

    default:
      break;
  };

  /*---------------------------------------------------------------------
  Notify data that GAUP is done.
  ---------------------------------------------------------------------*/
  ind_data.gaup_trans_id = trans_id;
  hdrds_queue_ind_ext( HDRHAI_PACKET_APPLICATION_CONTROLLER, 
                       ind_name, 
                       ind_data );

  /* Check hdrmrlpc.pending_gaup_req_q to see if there is any gaup pending to be sent 
   * Send one pending gaup everytime, don't go throught the q
   */
  rex_enter_crit_sect(&hdrmrlpc.crit_sect);

  while ( (pending_gaup_req_ptr =
          (hdrmrlpc_pending_gaup_req_type *) 
           q_get(&hdrmrlpc.pending_gaup_req_q )) != NULL )
  {
    hdrerrno_enum_type ret_val = E_SUCCESS;

    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                   "TX pending GAUP: TransID %d", pending_gaup_req_ptr->transaction_id);

    /* Proposing GAUP from HDRMC context.  Since this function compares and
     * modifies static hdrmrlpc data that is otherwise only used from DS context,
     * lets make sure DS wasn't half way through proposing another GAUP at this instant.
     */

    ret_val = hdrmrlpc_prop_gaup_qos( pending_gaup_req_ptr->num_profile_blobs,
                                      pending_gaup_req_ptr->profile,
                                      NULL,
                                      pending_gaup_req_ptr->transaction_id);

    HDR_MSG_PROT_1(MSG_LEGACY_MED, "hdrmrlpc_prop_gaup_qos returned %d",
                   ret_val);

    if (ret_val == E_SUCCESS)
    {
      /* GAUP has been queued to HMP successfully. The tx cb or GAUPRsp 
       * will 1) send the status report for current GAUP to DS, and 2) 
       * send the next pending GAUP (if any).
       */
      modem_mem_free( pending_gaup_req_ptr, MODEM_MEM_CLIENT_HDR_CRIT );
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Pending gaup req ptr free = 0x%x.",
                      pending_gaup_req_ptr );
      break;
    }
    else
    {
      /* Send GAUP failed ind. to DS. Continue going thru the queue
       * to process remaining GAUPs
       */
      ind_data.gaup_trans_id = pending_gaup_req_ptr->transaction_id;

      rex_leave_crit_sect(&hdrmrlpc.crit_sect); 
      hdrds_queue_ind_ext( HDRHAI_PACKET_APPLICATION_CONTROLLER, 
                       HDRIND_SCM_MRLP_AT_GAUP_FAILED, 
                       ind_data );
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);

      modem_mem_free( pending_gaup_req_ptr, MODEM_MEM_CLIENT_HDR_CRIT );
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Pending gaup req ptr free = 0x%x.",
                      pending_gaup_req_ptr );
    }
  }

  rex_leave_crit_sect(&hdrmrlpc.crit_sect);

} /* hdrmrlpc_process_attrib_update_response() */

/* <EJECT> */
/*=============================================================================
FUNCTION HDRMRLPC_PROCESS_ATTRIB_UPDATE_ACCEPT_MSG

DESCRIPTION
  This function processes an AttributeUpdateAccept message for the
  Multiflow packet app.
    
DEPENDENCIES
  The AT currently does not initiate GAUP for any of the attributes.
  It should not receive an Accept message.

PARAMETERS
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
/*lint -e715 */
void hdrmrlpc_process_attrib_update_accept_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
)
{
  uint8 trans_id;  /* Transaction ID */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (HDRMRLPC_UNPACK_TRANSACTION_ID(item_ptr, &trans_id) != E_SUCCESS)
  {
    ERR("Error unpacking TransID of AttribUpdateAccept msg",0,0,0);
    return;
  }

  HDR_MSG_PROT_2(MSG_LEGACY_MED,
    "Process AttributeUpdateAccept for trans_id %d (num timers=%d)",
    trans_id,
    hdrmrlpc.gaup_timer_cnt);

  /* GAUP attributes were accepted, make the attributes current and notify
     clients. */
  hdrmrlpc_process_attrib_update_response(trans_id,
                                          HDRIND_SCM_MRLP_AT_GAUP_ACCEPTED);

} /* hdrmrlpc_process_attrib_update_accept_msg() */
/*lint +e715 */

/* EJECT */
/*=============================================================================
FUNCTION HDRMRLPC_PROCESS_ATTRIB_UPDATE_REJECT

DESCRIPTION
  This function processes an AttributeUpdateReject message for the
  Multiflow packet app.
    
DEPENDENCIES
  The AT currently does not initiate GAUP for any of the attributes.
  It should not receive an Reject message.

PARAMETERS
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
/*lint -e715 */
void hdrmrlpc_process_attrib_update_reject_msg
(
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id
)
{
  uint8 trans_id;  /* Transaction ID */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (HDRMRLPC_UNPACK_TRANSACTION_ID(item_ptr, &trans_id) != E_SUCCESS)
  {
    ERR("Error unpacking TransID of AttributeUpdateReject msg.",0,0,0);
    return;
  }
  
  HDR_MSG_PROT_2(MSG_LEGACY_MED,
  "Process AttributeUpdateReject for trans_id %d (num timers=%d)",
  trans_id,
  hdrmrlpc.gaup_timer_cnt);


  /* GAUP attributes were rejected, cancel our request and notify clients. */
  hdrmrlpc_process_attrib_update_response(trans_id,
                                          HDRIND_SCM_MRLP_AT_GAUP_REJECTED);

} /* hdrpacc_process_mf_attrib_update_rej_msg() */
/*lint +e715 */

/* EJECT */
/*===========================================================================

FUNCTION HDRMRLP_GAUP_REQ_TX_OUTCOME_CB

DESCRIPTION
  This function is as a callback funtion for transmission outcome of the
  gaup request message.

  E_SUCCESS - SLP Ack was received from the AN in response to transmission 
              on the RTC

  E_FAILURE - a) All reliable delivery SLP retries were attempted and failed
              b) Other unexpected failures (no session open at time of 
                 transmission, Session closed during transmission, AT enters 
                 PWR_SAVE during transmission) 

  E_AGAIN -   a) The message was held pending connection setup and the 
                 connection setup failed.
              b) The connection closed before all 3 SLP retries were attempted

  E_TIMER_EXP - Overall message timer expired (40s)

  E_PROT_Q_FULL - No buffers for the message (unexpected)
              
  Note that E_AGAIN represents all possible connection setup failures,
  including no TCA and no RTC Ack.  If it is desired to not fail the 
  QoSRequest on the first occurance of such a failure, this routine
  should be extended to implement some retry mechanism.

DEPENDENCIES
  None

PARAMETERS
  data        - user data
  status      - message transmission status


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpc_gaup_req_tx_outcome_cb
(
  void                 *data_ptr,
  hdrerrno_enum_type    err_val
)
{

  switch(err_val)
  {
    case E_SUCCESS:
      /* message transmitted successfully */
      HDRTRACE_TXMSG_ACKED(HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL,
                           HDRMRLPC_ATTRIB_UPDATE_REQ_MSG, 0);  /*lint !e506 */

      /* Start the gaup response timer for this request */
      rex_enter_crit_sect(&hdrmrlpc.crit_sect);
      hdrmrlpc_gaup_timer_start( (uint8) ((dword)data_ptr), FALSE );
      rex_leave_crit_sect(&hdrmrlpc.crit_sect);
      break;

    case E_AGAIN: 
    case E_FAILURE:
    case E_TIMER_EXP:
    case E_PROT_Q_FULL:

      HDRTRACE_TXMSG_FAILED(HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL,
                            HDRMRLPC_ATTRIB_UPDATE_REQ_MSG, 0); /*lint !e506 */
      
      /* _mpa_ HMP should try to re-transmit (rules TBD).  
         For now, we just report a failure to the app. */
      hdrmrlpc_process_attrib_update_response((uint8) ((dword)data_ptr),
                                              HDRIND_SCM_MRLP_AT_GAUP_FAILED);
      break;

    case E_CANCELED:
    case E_NOT_ALLOWED:
    case E_NOT_AVAILABLE:
      /* Message retransmission handled by HMP */
      break;

    default:
      ERR( "Unknown err val for Config outcome %d",
            err_val, 0, 0);
      break;
  }

} /* hdrmrlpc_gaup_req_tx_outcome_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION  HDRMRLPC_PROP_GAUP_QOS

DESCRIPTION
  API to send an AttributeUpdateRequest for a ReservationKKQosRequestXXX set
  of attributes
  
DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS     - AT is sending the AttributeUpdateRequest message
                  transaction_id of the message is passed back.
  
  E_NOT_ALLOWED - AT is not allowed to send a GAUP message while it is waiting
                  for a response to AttributeUpdateRequest message.
                                   
   "While the initiator is waiting for a response to an AttributeUpdateRequest 
    message, it shall not transmit another AttributeUpdateRequest message with 
    a different TransactionID field that request reconfiguration of an 
    attribute included in the original AttributeUpdateRequest message. "
  
  E_NOT_AVAILABLE - MRLP not bound to any stream

  E_DATA_TOO_LARGE - num_profiles greater than MAX reservations allowed
                     (20 fwd + 20 rev)
  E_FAILURE - Not able to get a pending GAUP message buffer.
    
SIDE EFFECTS
  None
  
===========================================================================*/
static hdrerrno_enum_type hdrmrlpc_prop_gaup_qos
(
  uint8 num_profile_blobs,
  hdrmrlpc_gaup_qos_request_type *profile_ptr,
  uint8 *transaction_id_ptr,
  uint8 preallocated_trans_id  
)
{
  uint16 voffset = 0; /* Bit offset from start of attrib to current value */
  uint16 aoffset = 0; /* Bit offset from start of message to current value */

  hdrmrlpc_config_attrib_result_type *cfg_ptr;   /* Convenience pointer */

  uint16 attrib_id;  /* Attribute ID */
  uint16 attrib_len; /* Attribute Length in bits */
  uint8 i, j;        /* iterators */

  dsm_item_type * send_msg_ptr = NULL; 
    /* Pointer to gaup req. message to send */

  hdrhai_stream_enum_type stream_id;
    /* Stream to send the GAUP message on */

  uint8 cfg_idx = 0; /* Table Index of the In_config Reservation Label */

  hdrmrlpc_pending_gaup_req_type *pending_gaup_req_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrmlrpc_prop_gaup_qos_request...");

  if ( !hdrstream_get_stream(HDRHAI_MULTIFLOW_PACKET_APP_SN, &stream_id) &&
       !hdrstream_get_stream(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN, &stream_id)&&
       !hdrstream_get_stream(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN , &stream_id))
  {
    /* If session not open, simply ignore request to send QoS */
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                  "Dropping QoS TransID %d due to session close", 
                  preallocated_trans_id);
    return E_NOT_AVAILABLE;
  }
#ifdef FEATURE_HDR_EMPA_SUPPORT_STD_V115_ATTRIB_ID
  else
  {
    /* YLL Hack: Svae the app_subtype to be used in the packing routine,
       when mapping of the attribute IDs is needed */
    hdrmrlpc.app_subtype = hdrstream_get_apptype_for_neg_stream( stream_id );
  }
#endif

  /* Sanity checking - shouldn't happen */
  if ( num_profile_blobs >= HDRMRLPC_N_RES_LABELS_MAX_FWD + 
                       HDRMRLPC_N_RES_LABELS_MAX_REV )
  {
    return E_DATA_TOO_LARGE;
  }

  /* First pass to make sure that we are allowed to send this message. */
  for (i=0; i < num_profile_blobs; i++)
  {
    if (profile_ptr[i].direction == HDRMRLPC_FWD_RLP)
    {
      if (hdrmrlpc_map_cfg_rl(profile_ptr[i].reservation_label,HDRMRLPC_FWD_RLP,&cfg_idx) == E_FAILURE)
      {
        return E_DATA_TOO_LARGE;
      }
      cfg_ptr = &hdrmrlpc.config_result.res_kk_fwd_attrib[cfg_idx].qos_request;
    }
    else /* HDRMRLPC_REV_RLP */
    {
      if (hdrmrlpc_map_cfg_rl(profile_ptr[i].reservation_label,HDRMRLPC_REV_RLP,&cfg_idx) == E_FAILURE)
      {
        return E_DATA_TOO_LARGE;
      }
      cfg_ptr = &hdrmrlpc.config_result.res_kk_rev_attrib[cfg_idx].qos_request;
    }

    /* Previous GAUP is still in the process of transmission. Queue current GAUP and wait
     * for transmission result of previous GAUP before sending current one.
     */
    if (cfg_ptr->at_requested)
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Previous AUR has not completed");

      /* Get a buffer from the queue for pending gaup parameters */
      if ( ( pending_gaup_req_ptr = ( hdrmrlpc_pending_gaup_req_type * )
             modem_mem_alloc( sizeof( hdrmrlpc_pending_gaup_req_type ),
                        MODEM_MEM_CLIENT_HDR_CRIT ) ) == NULL )
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "No free memory for pending_gaup_reg_type");
        hdrsmp_deactivate(HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL, FALSE);

        return E_FAILURE;
      }
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                      "Pending gaup req ptr alloc = 0x%x, size = %d",
                      pending_gaup_req_ptr,
                      sizeof( hdrmrlpc_pending_gaup_req_type ) );

      /* If we already queued the current GAUP once, just re-queue it back */
      if (preallocated_trans_id != HDRMRLPC_NO_PREALOCATED_TRANSACTION_ID)
      {
        HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "Re-queue current AUR(TransId %d)", preallocated_trans_id);

        pending_gaup_req_ptr->transaction_id = preallocated_trans_id;
      }
      /* otherwise, reserve a transaction id and copy contents passed from DS*/
      else
      {
        /* Reserve a trans_id */
        hdrmrlpc.gaup_trans_id++;
        if (hdrmrlpc.gaup_trans_id == 
            HDRMRLPC_NO_PREALOCATED_TRANSACTION_ID)
        {
          hdrmrlpc.gaup_trans_id = 0;
        }

        if (transaction_id_ptr != NULL)
        {
          /* Return the allocated transaction ID to DS as if we were sending the GAUP now */
          *transaction_id_ptr = hdrmrlpc.gaup_trans_id;
        }

        pending_gaup_req_ptr->transaction_id = hdrmrlpc.gaup_trans_id;

        HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "Queue current AUR(TransId %d)", hdrmrlpc.gaup_trans_id);
      }

      /* Copy contents of current GAUP */
      for (i=0; i < num_profile_blobs; i++)
      {
#ifdef FEATURE_MEMCPY_REMOVAL
         pending_gaup_req_ptr->profile[i] = profile_ptr[i];
#else
        (void) memcpy( &pending_gaup_req_ptr->profile[i], &profile_ptr[i],
         sizeof( hdrmrlpc_gaup_qos_request_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */
      }

      pending_gaup_req_ptr->num_profile_blobs = num_profile_blobs;

      q_put( &hdrmrlpc.pending_gaup_req_q, &pending_gaup_req_ptr->link);

      return E_SUCCESS;
    }
  }

  /* If we have not preallocated a transaction_id, allocate one*/
  if (preallocated_trans_id == HDRMRLPC_NO_PREALOCATED_TRANSACTION_ID)
  {
    /* Allocate a trans_id */
    hdrmrlpc.gaup_trans_id++;
    if (hdrmrlpc.gaup_trans_id == 
        HDRMRLPC_NO_PREALOCATED_TRANSACTION_ID)
    {
      hdrmrlpc.gaup_trans_id = 0;
    }

    if (transaction_id_ptr != NULL)
    {
      /* Return the allocated transaction ID to DS*/
      *transaction_id_ptr = hdrmrlpc.gaup_trans_id;
    }

    /* Update preallocated_trans_id */
    preallocated_trans_id = hdrmrlpc.gaup_trans_id;
  }

  send_msg_ptr = hdrutil_new_msg_buffer();

  HDRMRLPC_PACK_MSG_ID( send_msg_ptr, HDRMRLPC_ATTRIB_UPDATE_REQ_MSG);
  HDRMRLPC_PACK_TRANSACTION_ID( send_msg_ptr, preallocated_trans_id);

  /* Set aoffset to the offset where the first attribute is to be packed in */
  aoffset = HDRMRLPC_MSG_ID_SIZE + HDRMRLPC_TRANSACTION_ID_SIZE;

  /* Second pass: pack and send the AttributeUpdateRequest message */
  for (i=0; i < num_profile_blobs; i++)
  {
    if (profile_ptr[i].direction == HDRMRLPC_FWD_RLP)
    {
      if((hdrmrlpc_map_cfg_rl(profile_ptr[i].reservation_label, HDRMRLPC_FWD_RLP, &cfg_idx) != E_SUCCESS))
      {
        ERR_FATAL( "map cfg is not SUCCESS", 0, 0, 0 );
      }

      cfg_ptr = &hdrmrlpc.config_result.res_kk_fwd_attrib[cfg_idx].qos_request;

#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.fwd_res_info[cfg_idx].qos_request = 
                                        profile_ptr[i].qos_profile;
#else
      memcpy((void*)&hdrmrlpc.in_config.fwd_res_info[cfg_idx].qos_request,
             (void*)&profile_ptr[i].qos_profile,
             sizeof(hdrmrlpc_res_qos_profile_config_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      attrib_id = HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID | (uint16)profile_ptr[i].reservation_label;

    }
    else /* HDRMRLPC_REV_RLP */
    {
      if((hdrmrlpc_map_cfg_rl(profile_ptr[i].reservation_label,HDRMRLPC_REV_RLP, &cfg_idx) != E_SUCCESS))
      {
        ERR_FATAL( "map cfg is not SUCCESS", 0, 0, 0 );
      }

      cfg_ptr = &hdrmrlpc.config_result.res_kk_rev_attrib[cfg_idx].qos_request;

#ifdef FEATURE_MEMCPY_REMOVAL
      hdrmrlpc.in_config.rev_res_info[cfg_idx].qos_request = 
                                      profile_ptr[i].qos_profile;
#else
      memcpy((void*)&hdrmrlpc.in_config.rev_res_info[cfg_idx].qos_request,
             (void*)&profile_ptr[i].qos_profile,
             sizeof(hdrmrlpc_res_qos_profile_config_type) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      attrib_id = HDRMRLPC_RES_KK_QOS_REQ_REV_ATTRIB_ID | (uint16)profile_ptr[i].reservation_label;
    }

    /* Mark the attribute as "requested" + save its transaction ID.
       Just send '0' as the value ID since it doesn't matter for GAUP. */
    cfg_ptr->at_requested = TRUE;
    cfg_ptr->value = preallocated_trans_id;

    ASSERT(profile_ptr[i].qos_profile.length <= HDRMRLP_MAX_PROFILE_LENGTH);

    attrib_len = HDRMRLPC_ATTRIB_ID_SIZE +
                 HDRMRLPC_RESKK_QOS_ATTRIB_MIN_VAL_SIZE  +
                 (profile_ptr[i].qos_profile.length * HDRMRLPC_BITS_PER_BYTE);

    HDRMRLPC_PACK_CON_REQ_ATTRIB_LEN(
      send_msg_ptr, (uint8)(attrib_len / HDRMRLPC_BITS_PER_BYTE), aoffset);

    HDRMRLPC_PACK_CON_REQ_ATTRIB_ID(send_msg_ptr, attrib_id, aoffset);

    voffset = HDRMRLPC_MIN_ATTRIB_LEN;

    HDRMRLPC_PACK_CON_REQ_ATTRIB_VALUE(send_msg_ptr, 0, 
                                       aoffset, voffset);

    HDRMRLPC_PACK_CON_REQ_RESKK_TYPE(send_msg_ptr, 
                                     profile_ptr[i].qos_profile.type, 
                                     aoffset, voffset);

    HDRMRLPC_PACK_CON_REQ_RESKK_LENGTH(send_msg_ptr, 
                                       profile_ptr[i].qos_profile.length, 
                                       aoffset, voffset);

    for (j=0; j < profile_ptr[i].qos_profile.length; j++)
    {
      HDRMRLPC_PACK_CON_REQ_RESKK_VAL(send_msg_ptr, 
                                      profile_ptr[i].qos_profile.value[j], 
                                      aoffset, voffset, j);
    } 

    aoffset += HDRMRLPC_ATTRIB_LEN + attrib_len;  
  }

  hdrhmp_app_send_rasm_qos_request( 
                                  HDRHAI_MULTIFLOW_RADIO_LINK_PROTOCOL,       
                                  stream_id,                                  
                                  send_msg_ptr,                               
                                  hdrmrlpc_gaup_req_tx_outcome_cb,            
                                  (void*) ((dword) preallocated_trans_id) ); 

  hdrmrlpc_gaup_timer_start(preallocated_trans_id, TRUE);

  return E_SUCCESS;
} /* hdrmrlpc_prop_gaup_qos() */

/* <EJECT> */
/*===========================================================================

FUNCTION  HDRMRLPC_PROP_GAUP_QOS_REQUEST

DESCRIPTION
  This is a wrapper function to hdrmrlpc_prop_gaup_qos. It enter critical section
  before calling hdrmrlpc_prop_gaup_qos.
  This function is needed because we need to use the same critical section variable,
  hdrmrlpc.crit_sect, when it is called outside hdrmrlpc.
  
DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS     - AT is sending the AttributeUpdateRequest message
                  transaction_id of the message is passed back.
  
  E_NOT_ALLOWED - AT is not allowed to send a GAUP message while it is waiting
                  for a response to AttributeUpdateRequest message.
                                   
   "While the initiator is waiting for a response to an AttributeUpdateRequest 
    message, it shall not transmit another AttributeUpdateRequest message with 
    a different TransactionID field that request reconfiguration of an 
    attribute included in the original AttributeUpdateRequest message. "
  
  E_NOT_AVAILABLE - MRLP not bound to any stream
  
  E_DATA_TOO_LARGE - num_profiles greater than MAX reservations allowed
                     (20 fwd + 20 rev)
    
SIDE EFFECTS
  None
  
===========================================================================*/
hdrerrno_enum_type hdrmrlpc_prop_gaup_qos_request
(
  uint8 num_profile_blobs,
  hdrmrlpc_gaup_qos_request_type *profile_ptr,
  uint8 *transaction_id_ptr,
  uint8 preallocated_trans_id  
)
{
  hdrerrno_enum_type err_val;
  rex_enter_crit_sect(&hdrmrlpc.crit_sect);

  err_val =  hdrmrlpc_prop_gaup_qos( num_profile_blobs,
                                     profile_ptr,
                                     transaction_id_ptr,
                                     preallocated_trans_id );

  rex_leave_crit_sect(&hdrmrlpc.crit_sect);

  return err_val;
} /* hdrmrlpc_prop_gaup_qos_request*/ 

/*===========================================================================
FUNCTION HDRMRLPC_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the Multiflow
  Packet Application
    
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpc_process_timer
(
  uint32 timer_id
)
{

  switch ( (hdrmrlpc_timer_name_enum_type)timer_id )
  {
    case HDRMRLPC_GAUP_RSP_TIMER:

      /* AN hasn't responded to the AT's AttributeUpdateRequest msg within the
         Tturnaround time allowed.  We fail this GAUP update. */

      HDR_MSG_PROT_1( MSG_LEGACY_MED,
                      "Running GAUP timer for TID %d expired.", 
                      hdrmrlpc.gaup_timer_trans_id );

      hdrmrlpc_process_attrib_update_response(
        hdrmrlpc.gaup_timer_trans_id,
        HDRIND_SCM_MRLP_AT_GAUP_FAILED );
      
      break;

    default:
      ERR("Unrecognized timer",timer_id,0,0);
      break;
  }

} /* hdrmrlpc_process_timer() */

/*===========================================================================

FUNCTION HDRMRLPC_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of MRLP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of MRLP

SIDE EFFECTS
  None

===========================================================================*/
void* hdrmrlpc_get_inconfig_handle(void)
{
  return (void*) &hdrmrlpc.in_config;
} /* hdrmrlpc_get_inconfig_handle() */


/*===========================================================================

FUNCTION HDRMRLPC_IS_MFPA_ATTRIBUTE

DESCRIPTION
  This function checks if an attribute is MFPA related attribute

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If the attribute is MFPA attribute
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_is_mfpa_attribute
(
  uint16 attrib_id
)
{
  switch ( attrib_id )
  {
    /* Simple Attributes */
    case HDRMRLPC_MAX_ABORT_TIMER_ATTRIB_ID: 
    case HDRMRLPC_AT_NAKDELAY_SUPPORTED_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_NAK_ENABLE_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_NAK_ENABLE_REV_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_HIGHER_LAYER_PROT_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_PHYS_LAYER_NAK_ENABLED_REV_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_IDLE_STATE_FWD_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_IDLE_STATE_REV_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_DATA_OVER_SIG_ALLOWED_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_ATTRIB_ID:

    /* Complex Attributes */
    case HDRMRLPC_FLOW_NN_IDENTIFICATION_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_ID_REV_OR_SEQ_LEN_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_TIMER_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_TIMER_REV_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_RESERVATION_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_RESERVATION_REV_ATTRIB_ID:
  
    case HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_QOS_REQ_REV_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_QOS_RSP_FWD_ATTRIB_ID:
    case HDRMRLPC_RES_KK_QOS_RSP_REV_ATTRIB_ID:
  
    case HDRMRLPC_SUPPORTED_HIGHER_LAYER_PROT_ATTRIB_ID: 
    case HDRMRLPC_AT_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
    case HDRMRLPC_AN_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
    case HDRMRLPC_MAX_RLP_FLOWS_ATTRIB_ID:
    case HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID:

      return TRUE;

    default:
      return FALSE;
  }
} /* hdrmrlpc_is_mfpa_attribute */

/*===========================================================================

FUNCTION HDRMRLPC_IS_EMPA_ATTRIBUTE

DESCRIPTION
  This function checks if an attribute is EMPA related attribute

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If the attribute is EMPA attribute
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_is_empa_attribute
(
  uint16 attrib_id
)
{
  switch ( attrib_id )
  {
    /* Simple Attributes */
    case HDRMRLPC_MAX_ABORT_TIMER_ATTRIB_ID: 
    case HDRMRLPC_PPP_FREE_AUTHENTICATION_SUPPORT_ATTRIB_ID:
    case HDRMRLPC_TWO_ROUTES_SUPPORTED_ATTRIB_ID:
    case HDRMRLPC_AT_NAKDELAY_SUPPORTED_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_NAK_ENABLE_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_NAK_ENABLE_REV_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_PHYS_LAYER_NAK_ENABLED_REV_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_IDLE_STATE_FWD_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_IDLE_STATE_REV_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_DATA_OVER_SIG_ALLOWED_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ACTIVATED_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ACTIVATED_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_SEQ_LEN_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_SEQ_LEN_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_DATA_UNIT_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_DATA_UNIT_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_SIM_DEL_BOTH_ROUTES_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_OOO_DEL_ROUTE_PROT_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_NAK_DELAY_TIME_FWD_ATTRIB_ID:

    /* Complex Attributes */
    case HDRMRLPC_FLOW_NN_TIMER_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_TIMER_REV_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_RESERVATION_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_RESERVATION_REV_ATTRIB_ID:
  
    case HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_QOS_REQ_REV_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_QOS_RSP_FWD_ATTRIB_ID:
    case HDRMRLPC_RES_KK_QOS_RSP_REV_ATTRIB_ID:
  
    case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_REV_ATTRIB_ID:
  
    case HDRMRLPC_AT_SUP_FLOW_PROT_PARM_PP_ATTRIB_ID:
    case HDRMRLPC_AT_SUP_ROUTE_PROT_PARM_PP_ATTRIB_ID:
  
    case HDRMRLPC_AT_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
    case HDRMRLPC_AN_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
    case HDRMRLPC_MAX_RLP_FLOWS_ATTRIB_ID:
    case HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID:

      return TRUE;

    default:
      return FALSE;
  }
} /* hdrmrlpc_is_empa_attribute */

/*===========================================================================

FUNCTION HDRMRLPC_IS_MMPA_ATTRIBUTE

DESCRIPTION
  This function checks if an attribute is MMPA related attribute

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If the attribute is MMPA attribute
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_is_mmpa_attribute
(
  uint16 attrib_id
)
{
  switch ( attrib_id )
  {
    /* Simple Attributes */
    case HDRMRLPC_MAX_ABORT_TIMER_ATTRIB_ID: 
    case HDRMRLPC_PPP_FREE_AUTHENTICATION_SUPPORT_ATTRIB_ID:
    case HDRMRLPC_TWO_ROUTES_SUPPORTED_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_NAK_ENABLE_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_NAK_ENABLE_REV_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_PHYS_LAYER_NAK_ENABLED_REV_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_IDLE_STATE_FWD_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_IDLE_STATE_REV_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_TRANSMIT_ABORT_TIMER_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_DATA_OVER_SIG_ALLOWED_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ACTIVATED_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ACTIVATED_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_SEQ_LEN_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_FLOW_PROT_SDU_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_DATA_UNIT_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_DATA_UNIT_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ROUTE_PROT_SDU_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_SIM_DEL_BOTH_ROUTES_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_OOO_DEL_ROUTE_PROT_FWD_ATTRIB_ID:

    /* Complex Attributes */
    case HDRMRLPC_FLOW_NN_ID_REV_OR_SEQ_LEN_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_TIMER_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_TIMER_REV_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_RESERVATION_FWD_ATTRIB_ID: 
    case HDRMRLPC_FLOW_NN_RESERVATION_REV_ATTRIB_ID:
  
    case HDRMRLPC_RES_KK_QOS_REQ_FWD_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_QOS_REQ_REV_ATTRIB_ID: 
    case HDRMRLPC_RES_KK_QOS_RSP_FWD_ATTRIB_ID:
    case HDRMRLPC_RES_KK_QOS_RSP_REV_ATTRIB_ID:
  
    case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_FLOW_PROT_PARM_REV_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_FWD_ATTRIB_ID:
    case HDRMRLPC_FLOW_NN_ROUTE_PROT_PARM_REV_ATTRIB_ID:
  
    case HDRMRLPC_AT_SUP_FLOW_PROT_PARM_PP_ATTRIB_ID:
    case HDRMRLPC_AT_SUP_ROUTE_PROT_PARM_PP_ATTRIB_ID:
  
    case HDRMRLPC_AT_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
    case HDRMRLPC_AN_SUPPORTED_QOS_PROFILES_ATTRIB_ID:
    case HDRMRLPC_MAX_RLP_FLOWS_ATTRIB_ID:
    case HDRMRLPC_MAX_RESERVATIONS_ATTRIB_ID:

      return TRUE;

    default:
      return FALSE;
  }
} /* hdrmrlpc_is_mmpa_attribute */

/*===========================================================================

FUNCTION HDRMRLPC_IS_ATTRIB_VALID_FOR_APPSUBTYPE

DESCRIPTION
  This function checks if an attribute is valid for given app subtype.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If the attribute is valid.
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_is_attrib_valid_for_appsubtype
(
  hdrhai_app_subtype_enum_type app_subtype,
  uint16 attrib_id
)
{
  boolean attrib_is_valid = FALSE;
  /* Flag indicating whether a given attrib is valid for a given app subtype */
/*---------------------------------------------------------------------------*/

  switch (app_subtype)
  {
    case HDRHAI_MULTIFLOW_PACKET_APP_AN:
    case HDRHAI_MULTIFLOW_PACKET_APP_SN:
      attrib_is_valid = hdrmrlpc_is_mfpa_attribute(attrib_id);
      break;

#if defined(FEATURE_HDR_EMPA) || defined (FEATURE_HDR_REVB)
    case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_AN:
    case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
      attrib_is_valid = hdrmrlpc_is_empa_attribute(attrib_id);
      break;
#endif /* FEATURE_HDR_EMPA or FEATURE_HDR_REVB */

    case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_AN:
    case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
      attrib_is_valid = hdrmrlpc_is_mmpa_attribute(attrib_id);
      break;

    default:
      attrib_is_valid = FALSE;
      break;
  }

  return attrib_is_valid;

} /* hdrmrlpc_is_attrib_valid_for_appsubtype() */

#endif /* FEATURE_HDR_QOS */

/*===========================================================================

FUNCTION HDRMRLPC_FLOW_CONFIGURED_TO_DESIRED

DESCRIPTION
  This function checks flow00/01 is configured to AT supported values.
  flow00 has to be activated while flow01 has to be deactivated.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If the attribute is EMPA attribute
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_flow_configured_to_desired( void )
{
  boolean retval = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrstream_stream_is_negotiated(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
      hdrstream_stream_is_negotiated(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
#if defined(FEATURE_HDR_EMPA) || defined (FEATURE_HDR_REVB)
    if (!((hdrmrlpc.in_config.fwd_rlp_config_info[0x00].activated == 
           HDRMRLPC_FLOW_00_ACTIVATED_FWD_DEF) &&
      (hdrmrlpc.in_config.rev_rlp_config_info[0x00].activated == 
           HDRMRLPC_FLOW_00_ACTIVATED_REV_DEF) &&
      (hdrmrlpc.in_config.fwd_rlp_config_info[0x01].activated == 
          HDRMRLPC_FLOW_01_DEACTIVATED_FWD) &&
          (hdrmrlpc.in_config.rev_rlp_config_info[0x01].activated == 
           HDRMRLPC_FLOW_01_DEACTIVATED_REV)))
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "EMPA flow00/01 values invalid");
      retval = FALSE;
    }
#endif
  }
  
  if (hdrstream_stream_is_negotiated(HDRHAI_MULTIFLOW_PACKET_APP_SN))
  {
    if (!((hdrmrlpc.in_config.fwd_rlp_config_info[0x00].flow_id.is_active == 
           HDRMRLPC_FLOW_00_ACTIVATED_FWD_DEF) && 
          (hdrmrlpc.in_config.rev_rlp_config_info[0x00].flow_id.is_active ==
           HDRMRLPC_FLOW_00_ACTIVATED_REV_DEF)))
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "MPA flow00/01 values invalid");
      retval = FALSE;
    }
  }
  return retval;
} /* hdrmrlpc_flow_configured_to_desired */

/*===========================================================================

FUNCTION HDRMRLPC_CLEAR_PENDING_GAUP_QUEUE

DESCRIPTION
  Clears pending GAUP queue for QoS requests.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrmrlpc_clear_pending_gaup_queue( void )
{
  hdrmrlpc_pending_gaup_req_type * pending_gaup_req_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect(&hdrmrlpc.crit_sect);

  while ((pending_gaup_req_ptr = (hdrmrlpc_pending_gaup_req_type *) 
              q_get(&hdrmrlpc.pending_gaup_req_q)) != NULL)
  {
    pending_gaup_req_ptr->num_profile_blobs = 0;
    pending_gaup_req_ptr->transaction_id = 
        HDRMRLPC_NO_PREALOCATED_TRANSACTION_ID;
    modem_mem_free( pending_gaup_req_ptr, MODEM_MEM_CLIENT_HDR_CRIT );
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Pending gaup req ptr free = 0x%x.",
                    pending_gaup_req_ptr );
  }

  rex_leave_crit_sect(&hdrmrlpc.crit_sect);
} /* hdrmrlpc_clear_pending_gaup_queue */

#ifdef FEATURE_EHRPD
/*===========================================================================

FUNCTION HDRMRLPC_CURRENT_PERSONALITY_IS_EHRPD

DESCRIPTION
  This function is used to check whether current personality is ehrpd or not.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE if the personality is eHRPD, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrmrlpc_current_personality_is_ehrpd( void )
{

  if ( (hdrstream_stream_is_negotiated(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
        hdrstream_stream_is_negotiated(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
       &&
       (hdrmrlpc.current.fwd_flow_prot[0].id == 0x07) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* hdrmrlpc_current_personality_is_ehrpd() */
#endif /* FEATURE_EHRPD */

/*===========================================================================

FUNCTION HDRMRLPC_FLOWNN_TIMERS_ARE_VALID

DESCRIPTION
  This function checks if the FlowNNTimers are configured to
  valid values.

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  TRUE - If the all timer attributes are valid
  FALSE - If any timer attribute is invalid

SIDE EFFECTS
  Set all proposed timer attributes in current ConReq to rejected
  if any of them is invalid.

===========================================================================*/
boolean hdrmrlpc_flownn_timers_are_valid( void )
{
  uint8 flow;  
  hdrmrlpc_config_req_result_type* cr_ptr = &hdrmrlpc.config_result;
  hdrmrlpc_config_params_type* c_ptr = &hdrmrlpc.in_config;
  uint8 max_abort_timer = c_ptr->max_abort_timer;

  boolean retval = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (flow = 0; flow < c_ptr->max_rlp_flows.max_num_rlp_flows_fwd; flow++)
  {
   retval = hdrmrlpc_timers_attrib_is_valid(
      c_ptr->fwd_rlp_config_info[flow].flow_timer, 
      max_abort_timer);

    if (retval == FALSE)
    {
      HDR_MSG_PROT_4(MSG_LEGACY_HIGH, 
                     "FWD FLOW %d timer invalid: max %d abort %d flush %d",
                     flow, max_abort_timer * 100, 
                     c_ptr->fwd_rlp_config_info[flow].flow_timer.abort_timer_val,
                     c_ptr->fwd_rlp_config_info[flow].flow_timer.flush_timer_val);
      break;
    }
  }

  if ( retval )
  {
    for (flow = 0; flow < c_ptr->max_rlp_flows.max_num_rlp_flows_rev; flow++)
    {
      retval = hdrmrlpc_timers_attrib_is_valid(
        c_ptr->rev_rlp_config_info[flow].flow_timer, 
        max_abort_timer);
  
      if (retval == FALSE)
      {
        HDR_MSG_PROT_4(MSG_LEGACY_HIGH, 
                       "REV FLOW %d timer invalid: max %d abort %d flush %d",
                       flow, max_abort_timer * 100, 
                       c_ptr->rev_rlp_config_info[flow].flow_timer.abort_timer_val,
                       c_ptr->rev_rlp_config_info[flow].flow_timer.flush_timer_val);
        break;
      }
    }
  }

  if (!retval)
  {
    if (cr_ptr->max_abort_timer_attrib.is_accepted)
    {
      cr_ptr->max_abort_timer_attrib.is_accepted = FALSE;
    }

    for (flow = 0; flow < c_ptr->max_rlp_flows.max_num_rlp_flows_fwd; flow++)
    {
      if (cr_ptr->flow_nn_fwd_attrib[flow].timers.is_accepted)
      {
        cr_ptr->flow_nn_fwd_attrib[flow].timers.is_accepted = FALSE;
      }
    }

    for (flow = 0; flow < c_ptr->max_rlp_flows.max_num_rlp_flows_rev; flow++)
    {
      if (cr_ptr->flow_nn_rev_attrib[flow].timers.is_accepted)
      {
        cr_ptr->flow_nn_rev_attrib[flow].timers.is_accepted = FALSE;
      }
    }
  }

  return retval;
} /* hdrmrlpc_flow_configured_to_desired */
