#ifndef MACHSLOG_H__
#define MACHSLOG_H__

/*===========================================================================
                             M A C H S L O G . H

DESCRIPTION
   This file contains MAC HS layer log packet structure definitions, 
   prototypes for MAC HS logging functions, any constant definitions that 
   are needed, and any external variable declarations needed for MAC HS logging.

Copyright (c) 2001 - 2004 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2008 - 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/


/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/machslog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
01/30/14    kv      0x421E/0x4220 Logging Fixes for 3C and WM debug prints
05/23/13    geg     WSW decoupling (except for component of QCHAT)
07/13/12    grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
01/02/12    grk     Feature cleanup
09/19/10    av      HS_FACH and Enhanced 0x4220 log changes
10/06/09    sup     Added log_codes_wcdma_int_v.h include 
9/22/09     sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int_v.h
05/07/09    ssg     Updated Copyright Information
10/17/08    grk     Merged taxis changes to mainline.
10/03/08    kvk     Mac-ehs reassembly log packets will now handle 130 PDUs
                    (Max re-ordering SDUs for 5 TBS)
11/10/04    gsc     Changed number of packets in status log report to 100.
10/26/04    gsc     Initial Release

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif
#include "log_codes_wcdma_int.h"

#ifdef FEATURE_MAC_HS_QXDM_LOGGING

#include "uecomdef.h"
#include "log.h"

#ifdef T_WINNT
  #error code not present
#endif

#define MAC_HS_NUM_REPORTS_IN_STATUS_LOG_PKT                           100



/*===========================================================================

                      DATA STRUCTURES FOR LOG PACKETS
                      
===========================================================================*/                      

/*--------------------------------------------------------------------------
 Data structure for Logical Channel configuration info
 --------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
   /* RLC ID */
   uint8   rlc_id;
   
   /* Logical channel type  */
   uint8   chan_type;
   
   /* Logical channel mode   */
   uint8   chan_mode;
}mac_hs_log_dlc_cfg_type;


/*--------------------------------------------------------------------------
 Data structure for HSDPA PDU size configuration
 --------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
   /* PDU Size in MAC HS Header */
   uint16 mac_pdu_size;
   
   /* PDU size index in MAC HS header */
   uint8 mac_pdu_index;
   
}mac_hs_log_pdu_size_info_type;

/*--------------------------------------------------------------------------
 Data structure for HSDPA Reordering Queue configuration 
 --------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
   /* Queue Id */
   uint8 queue_id;
   
   /* Reordering release timer in msecs */
   uint16 release_timer;
   
   /* Receiver Window Size */
   uint8 window_size;
   
   /* No of PDU sizes */
   uint8 no_of_pdu_sizes;
   
   /* MAC PDU size type information for this queue */
   mac_hs_log_pdu_size_info_type pdu_info[UE_MAX_HS_NO_OF_PDU_SIZE_PER_QUEUE];

} mac_hs_log_queue_info_type;

/*--------------------------------------------------------------------------
 Data structure for HSDPA MAC D FLOW configuration Information 
 --------------------------------------------------------------------------*/ 
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /* CCTrCh ID */
  cctrch_id_type cctrch_id;

  /* MAC-d Flow Id */
  uint8 mac_dflow_id; 

  /* No of priority queues */
  uint8 no_of_queues;

  /* Number of dedicated RLC logical channels mapped */
  uint8 ndlchan;

  /* Queue Information */
  mac_hs_log_queue_info_type queue_info[UE_MAX_HS_QUEUE];

  /* Info of dedicated logical channels mapped */
  mac_hs_log_dlc_cfg_type dlchan_info[UE_MAX_LOGCHAN_PER_DFLOW];

} mac_hs_log_mac_dflow_info_type;

/*--------------------------------------------------------------------------
 Data structure for HSDPA Status log packet
 --------------------------------------------------------------------------*/    
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
  /* Queue Id to which this TSN belongs */
  uint8 queue_id;

  /* Received TSN */
  uint8 rcvd_tsn;

  /* Next expected TSN */
  uint8 next_expected_tsn;

  /* Window Upper Edge */
  uint8 window_upper_edge;

  /* TSN to which the timer is associated */
  uint8 t1_tsn;
#ifdef FEATURE_WCDMA_HS_FACH
  /*status of TRESET timer*/
  uint8 reset_timer_status;
#endif
}mac_hs_log_status_info_type;

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))

/*--------------------------------------------------------------------------
 Data structure for Enh HSDPA Reordering Queue log packet
 --------------------------------------------------------------------------*/    
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
   /* Queue Id */
   uint8 queue_id;
   
   /* Reordering release timer in msecs */
   uint16 release_timer;
   
   /* Treset timer value - used in CellFACH and CellPCH states */
   uint8 t_reset;
   
   /* Receiver Window Size */
   uint8 window_size;
   
} mac_ehs_log_queue_info_type;

/*--------------------------------------------------------------------------
 Data structure for Enh HSDPA PDU log information
 --------------------------------------------------------------------------*/    
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
#ifndef FEATURE_WCDMA_3C_HSDPA
  /* Received TSN */
  uint8 rcvd_tsn;
#endif
#ifdef FEATURE_WCDMA_HS_FACH
  uint8 h_rnti;
#endif
#ifndef FEATURE_WCDMA_3C_HSDPA
  /* Next Expected TSN */
  uint8 next_expected_tsn;
  
  /* Upper Edge TSN */
  uint8 upper_edge;
  
  /* T1 TSN */
  uint8 t1_tsn;
#endif  
  /* Bit 2..0 - Queue Id
   * Bit 6..3 - LC Id
   * Bit 7 reserved
   */
  uint8 queue_lc_id;
  
  /* Bit 1..0 - Reassembly PDU Type (0 - Complete, 1 - Partial Start, 
   *                                 2 - Partial Middle, 3 - Partial End)
   * Bit 12..2 - Segment Size in Bytes
   * Bit 15..13 - Reserved
   */
  uint16 pdu_seg_info_size;
} mac_ehs_log_pdu_info;

#ifdef FEATURE_WCDMA_3C_HSDPA
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
    /*num of PDU in current TSN*/
    uint8 num_pdu_perTSN;

    /* Received TSN */
    uint16 rcvd_tsn;

    /* Next Expected TSN */
    uint16 next_expected_tsn;

    /* Upper Edge TSN */
    uint16 upper_edge;
	
    /* T1 TSN */
    uint16 t1_tsn;
} mac_ehs_log_tsn_info;
#endif
#endif

#ifdef FEATURE_WCDMA_HS_FACH

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8 h_rnti;
  uint8 rlc_id;  
  uint16 pdu_seg_info_size;
} mac_ehs_log_tm_pdu_info;

#endif

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{
  uint8 queue_id;
  uint8 rb_id;
  uint8 rlc_id;
  uint8 mac_id;
  uint8 chan_type;
  uint8 rlc_mode;
}mac_dl_ehs_log_logch_config_type;

/*===========================================================================

                      DATA STRUCTURES FOR EVENTS
                      
===========================================================================*/                      

/*--------------------------------------------------------------------------
 Data structure for Timer Expiry Event
 --------------------------------------------------------------------------*/    
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct
#endif /* __GNUC__ */
{
   /* Queue for which the timer expired */
   uint8 queue_id;

   /* Associated TSN */
   uint8 t1_tsn;
}mac_hs_log_timer_expiry_event_type;

/*===========================================================================

                      LOG PACKET DEFINITIONS
                      
===========================================================================*/                      
#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
/*--------------------------------------------------------------------------
Log packet for reporting MAC EHS Configuration log packet.
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_HS_CONFIG_LOG_PACKET_V2)

  /* Version Number */
  uint8 version_num;
  
  /* HSDPA Action */
  uint8 mac_hsdpa_action;

  /* Number of MAC-d Flows. */
  uint8 num_dflow;  
  
  /* Indicator whether reset is requested or not */
  boolean mac_hsdpa_reset_indicator;
  
  /* Indicator whether MAC-ehs is configured or not */
  boolean mac_ehs_enabled;
  
  /* Number of MAC-ehs reordering Queues */
  uint8 num_mac_ehs_reord_queue;  
  
  /*Num of Ehs channel*/
  uint8 num_ehs_channel; 

  
  /* HSDPA MAC-d Flow Structure */
  mac_hs_log_mac_dflow_info_type hsdpa_info[UE_MAX_MAC_D_FLOW];

  /* Enh HSDPA Reordering Queue structure */
  mac_ehs_log_queue_info_type enh_hs_queue_info[UE_MAX_HS_QUEUE];

/*MAC ehs queue info*/
  mac_dl_ehs_log_logch_config_type ehs_logch_cfg_info[UE_MAX_DL_LOGICAL_CHANNEL];
LOG_RECORD_END

/*--------------------------------------------------------------------------
Log packet for reporting MAC EHS Reassembly log packet.
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_HS_REASSEMBLY_LOG_PACKET)
  /* Bit 3..0 - Version number
   * Bit 4: MAC HS type (0 - Mac-hs, 1 - Mac-ehs) 
   * Bit 7..5 - Number of TSNs
   */
  uint8 hs_type_and_other_info;

  /* Num PDUs */
  uint8 num_pdu;
#ifdef FEATURE_WCDMA_HS_FACH
  uint8 efach_tm_mode;
  //tm efach data is smaller than non tm efach data. so keeping the current definition
#endif
  mac_ehs_log_pdu_info pdu_info[15*MCALWCDMA_DEC_HS_REORDER_SDU_MAX_COUNT];
#ifdef FEATURE_WCDMA_3C_HSDPA
  mac_ehs_log_tsn_info tsn_info[9];
#endif
LOG_RECORD_END

#endif /* #if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7))) */

/*--------------------------------------------------------------------------
Log packet for reporting MAC HS Configuration log packet.
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_HS_CONFIG_LOG_PACKET)
  /* HSDPA Action */
  uint8 mac_hsdpa_action;

  /* Indicator whether reset is requested or not */
  boolean mac_hsdpa_reset_indicator;
  
  /* Number of MAC-d Flows. */
  uint8 num_dflow;  

  /* HSDPA MAC-d Flow Structure */
  mac_hs_log_mac_dflow_info_type hsdpa_info[UE_MAX_MAC_D_FLOW];

LOG_RECORD_END


/*--------------------------------------------------------------------------
Log packet for reporting MAC HS STATUS log packet.
---------------------------------------------------------------------------*/


LOG_RECORD_DEFINE(WCDMA_MAC_HS_STATUS_REPORT_LOG_PACKET_V2)
  uint8 version;
  /* Number of accumulated Status log packets */
  uint8 num_pkts;

  /* List of Status packets */
  mac_hs_log_status_info_type pkt_list[MAC_HS_NUM_REPORTS_IN_STATUS_LOG_PKT];
LOG_RECORD_END


/*--------------------------------------------------------------------------
Log packet for reporting MAC HS RESET log packet.
---------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_HS_RESET_LOG_PACKET)

  /* Number of logical channel currently mapped to different Mac-d flows */
  uint8 num_dlc;

  /* List of logical channel Ids */
  uint8 dlc_list[UE_MAX_DL_LOGICAL_CHANNEL];

LOG_RECORD_END


#ifdef FEATURE_WCDMA_HS_FACH
LOG_RECORD_DEFINE(WCDMA_MAC_HS_TRESET_EXPIRY_LOG_PACKET)
  /*is Harq buffer cleared*/
  uint8 is_harq_buffer_cleared;
  
  /*Number of re-ordering queues for which reset timer expired*/
  uint8 num_queues;

  /* List of logical channel Ids */
  uint8 queue_id[MAC_HS_DL_MAX_QUEUES];

LOG_RECORD_END


#endif

#ifdef T_WINNT
  #error code not present
#endif

#endif /* FEATURE_HSDPA && FEATURE_MAC_HS_QXDM_LOGGING */
#endif /* MACHSLOG_H__ */





