#ifndef HDRCMAC_H
#define HDRCMAC_H
/*! @mainpage This module contains the exported definitions of the HDR Control
              Channel Media Access Control (MAC) Protocol module.
    @file hdrcmac.h
    @brief This module contains definitions and declarations necessary for 
           using the HDR Control Channel MAC Module. */

/*===========================================================================
  Copyright (c) 2000 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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


/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/COMMON/vcs/hdrcmac.h_v   1.11   04 Apr 2003 15:11:32   kevins  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrcmac.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/19/13   rmg     Added support to disable DLNA in TAP mode.
12/10/10   kss     Fixed sync issue by validating sync pkt hstr.
10/12/10   kss     Enable and process of missed scc indication (from hdrrx).
04/21/09   etv     Update for hdrdec_pkt_type_enum_type name change.
11/14/08   kss     Added hdrcmac_get_cc_packet_num, new sleep logging support.
10/06/08   etv     Added support for QPCH packet.
08/05/08   rmg     Added intra scc info struct definition.
01/10/07   etv     Added hdrcmac_process_bad_crc.
10/23/06   kss     Externalized definition of HDRCMAC_INVALID_LINK_ID.
07/21/06   etv     Start with Current CMAC subtype on powerup instead of 
                   Default.
07/06/06   mpa     Added hdrcmac_suspend_and_deactivate().
06/12/06   sq      Added hdrcmac_get_scc_rate
06/21/05   etv     Added support for Rev-A.
04/04/03   kss     Added function hdrcmac_sync_cc_is_missing().
11/03/00   dna     Added HDR event logging
03/30/00   st      Created.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "hdrerrno.h"
#include "hdrdec.h"
#include "hdrhai.h"
#include "hdrind.h"

/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

/** Link ID value that indicates unknown or no sector. */
#define HDRCMAC_INVALID_LINK_ID  0xFF

/** Type used to track entities requesting DLNA disable.
 (See hdrcmac_force_dlna_disable(). ) */
typedef enum
{
  HDRCMAC_FTAP_ID  = 0x01,
  HDRCMAC_RTAP_ID  = 0x02

} hdrcmac_caller_id_enum_type;


/*============================================================================
  HDRCMAC_POWERUP_INIT
*/
/**
  This function initializes the protocol prior to use.  This function should 
  be called upon entering the HDR mode of operation.

  @sideeffects Supervision timer is defined and timestamp status is set to 
  inactive.

  @context Calling task. This routine is reentrant */
/*============================================================================*/
extern void hdrcmac_powerup_init ( void );

/*============================================================================
  HDRCMAC_ACTIVATE
*/
/**
  This function transitions the protocol to the Active state and installs the 
  Control Channel supervision timer.  If the protocol is already in the 
  Active state then no action is taken.

  @dependencies The protocol must be initialized by calling the hdrcmac_init 
  function prior to activation.

  @context CMAC activation will always execute in HDRRX task context by queuing
  a command to HDRRX task even when called from a different task context. */
/*============================================================================*/
extern void hdrcmac_activate 
( 
  hdrhai_protocol_name_enum_type sender 
    /**< The protocol giving this command (only for software trace) */
);

/*============================================================================
  HDRCMAC_SUSPEND_AND_DEACTIVATE
*/
/**
  This function transitions the protocol to the Inactive state and suspends  
  the Control Channel supervision timer.  If the protocol is already in the
  Inactive state then no action is taken.

  @context HDRMC task. */
/*============================================================================*/
extern void hdrcmac_suspend_and_deactivate 
( 
  hdrhai_protocol_name_enum_type sender 
    /**< The protocol giving this command (only for software trace) */
);

/*============================================================================
  HDRCMAC_DEACTIVATE
*/
/**
  This function transitions the protocol to the Inactive state and clears the 
  Control Channel supervision timer.  If the protocol is already in the
  Inactive state then no action is taken.

  @context CMAC deactivation will always execute in HDRRX task context by 
  queuing a command to HDRRX task even when called from a different task 
  context. */
/*============================================================================*/
extern void hdrcmac_deactivate 
( 
  hdrhai_protocol_name_enum_type sender 
    /**< The protocol giving this command (only for software trace) */
);

/*============================================================================
  HDRCMAC_GET_SYNC_CAPS_TIMESTAMP
*/
/**
  This function provides the current value of the Synchronous Control Channel
  Capsule timestamp.  The return value indicates the availability of a valid 
  timestamp value.

  @return E_SUCCESS if timestamp is successfully copied to destination 
  address and E_NOT_AVAILABLE if timestamp value was not available.

  @sideeffects If available, timestamp is copied to client specified 
  destination address

  @context Calling task (HDRMC). This routine is reentrant */
/*============================================================================*/
extern hdrerrno_enum_type hdrcmac_get_sync_caps_timestamp 
( 
  hdrdec_half_slot_time_type msg_hstr,
    /**< HSTR of message */
  hdrdec_half_slot_time_type *timestamp_ptr
    /**< Destination address for timestamp */
);

/*============================================================================
  HDRCMAC_GET_SCC_RATE
*/
/**
  This function returns the last saved synchronous control channel rate.

  @return control channel rate.

  @context Calling task (HDRSRCH). This routine is reentrant */
/*============================================================================*/
extern uint8 hdrcmac_get_scc_rate( void );

/*============================================================================
  HDRCMAC_IND_CB
*/
/**
  This function processes incoming indications for this protocol.

  @context Calling task. This routine is reentrant. */
/*============================================================================*/
extern void hdrcmac_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
    /**< Name of the indication from hdrhai.h */
  void *ind_data
    /**< Pointer to any indication-specific data */
);

/*============================================================================
  HDRCMAC_PROCESS_BAD_CRC
*/
/**
  This function processes the bad CRC for RX and detects if a SYNC packet
  has been lost to give appropriate sleep indication.

  @return TRUE if a synchronous or a sub-synchronous control channel packet
  has been lost and FALSE otherwise.

  @context HDRDEC task. This routine is NOT reentrant. */
/*============================================================================*/
extern boolean hdrcmac_process_bad_crc
(
  uint16 pkt_slot_time,
    /**< Slot time of the bad packet */
  boolean pkts_pending,
    /**< Do we have any packets in decoB/CBuf waiting to be processed? */
  hdrdec_pkt_type_e_type pkt_type
    /**< Type of bad CRC CC packet. */
);

/*============================================================================
  HDRCMAC_IS_WAITING_FOR_SYNC_PKT
*/
/**
  Returns whether hdrcmac is waiting for a continued sync packet in the 
  current sync capsule.

  @return TRUE if the synchronous capsule is ongoing and a continuation 
  packet is expected and FALSE otherwise.

  @context NOT USED. @todo Remove? This routine is reentrant. */
/*============================================================================*/
extern boolean hdrcmac_is_waiting_for_sync_pkt( void );

/*============================================================================
  HDRCMAC_GET_CC_PACKET_NUM
*/
/**
  Determined the packet num of a CC packet if it is a continued Sync packet.
  Packet_num for subsync packets is always 1.  Note that this function must
  be called before the CC packet is processed.  Once processed, CC MAC may 
  reset some of the information needed to determine the packet num.

  @assumptions Function should not be called for QPCH. 

  @return TRUE if the synchronous or sub-synchronous control channel packet
  and FALSE if asynchronous control channel packet. 

  @context Calling task (HDRDEC) This routine is reentrant. */
/*============================================================================*/
extern boolean hdrcmac_get_cc_packet_num
(
  uint16 pkt_hstr,
    /**< timestamp of packet start with in Control Channel Cycle; must be in 
         the range 0-511 */

  uint16 scc_rate,
    /**< Decode rate of CC packet. */

  byte * pkt_hdr_ptr,
    /**< Pointer to first byte of payload. */

  uint16  * packet_num,
    /**< Return value: Determined packet number of packet missed if continued 
         packet. (Packet_num n means nth packet of sync capsule missed.) */

  boolean * is_subsync
    /**< Return value: Fills in TRUE if packet is a subsync packet FALSE 
         otherwise. */
);


/*============================================================================
  HDRCMAC_ENABLE_MISSED_SCC_DETECTION
*/
/**
  Enables processing of missed scc indication from firmware.

  @context HDRRX task */
/*============================================================================*/
void hdrcmac_enable_missed_scc_detection
(
  hdrhai_protocol_name_enum_type sender, 
    /**< The protocol giving this command (only for software trace) */ 

  boolean clear_pending
    /**< Whether to clear any pending indication */
);

/*============================================================================
  HDRCMAC_DISABLE_MISSED_SCC_DETECTION
*/
/**
  Disables processing of missed scc indication from firmware.

  @context HDRRX task */
/*============================================================================*/
void hdrcmac_disable_missed_scc_detection
(
  hdrhai_protocol_name_enum_type sender 
    /**< The protocol giving this command (only for software trace) */
);

/*============================================================================
  HDRCMAC_PROCESS_MISSED_SCC_INDICATION
*/
/**
  Process missed scc indication.

  @context HDRRX task. 

  @sideeffects None */
/*============================================================================*/
void hdrcmac_process_missed_scc_indication
( 
  int detectedSccOffset 
    /**< Indicates whether scc was detected */
);

#ifdef FEATURE_HDR_DLNA
/*===========================================================================
  HDRCMAC_FORCE_DLNA_DISABLE
*/
/**
  This function facilitates disabling of DLNA during TAP/test mode operation.
  If the disable parameter passed to this function is TRUE and if DLNA is 
  enabled, it is disabled.
  If the disable parameter passed to this function is FALSE, DLNA is enabled 
  if permitted by NV setting.
 
  @param caller_id entity requesting this command
  @param override forces DLNA disable when set to TRUE
 
  @context HDRRX task
 
  @sideeffects None */
/*=========================================================================*/

void hdrcmac_force_dlna_disable
(
  hdrcmac_caller_id_enum_type caller_id,
    /**< The entity giving this command */

  boolean disable
    /**< Whether to enforce DLNA disable if it is enabled via NV */
);
#endif /* FEATURE_HDR_DLNA */

#endif /* HDRCMAC_H */
