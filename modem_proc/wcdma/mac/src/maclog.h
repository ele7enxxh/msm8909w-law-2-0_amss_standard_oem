#ifndef MACLOG_H
#define MACLOG_H

/*===============================================================================================
                             M A C L O G . H

DESCRIPTION
This file contains or includes files that contain MAC layer log packet structure definitions, 
prototypes for MAC logging functions, any constant definitions that are needed, and any external
variable declarations needed for Layer 1 logging.

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
================================================================================================*/


/*================================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/maclog.h_v   1.8   20 Nov 2001 16:25:34   rchar  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/maclog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------------------
06/23/15    aa      Changes for new log packet 0x41CE
08/07/13    kc      Log packet support for UL MAC cipher parameters
05/23/13    geg     WSW decoupling (except for component of QCHAT)
09/22/09    sup     Removed log code assignments and placed them in
                    log_codes_wcdma_int_v.h
05/07/09    ssg     Updated Copyright Information
10/17/08    grk     Merged taxis changes to mainline.
12/10/02    sk      Changed log code of TVM packet from 0x4740 to 0x4002
11/15/02    sk      Added traffic volume report log packet.
11/20/01    rc      Fixed length calculation for TFCS log packet
10/24/01    rc      Reordered the fields of structure mac_log_tfc_type so that 
                    tfc_valid comes first and then the array of TFIs.
09/13/01    rc      Changed log code offsets from 0xXX to 0x1XX. 
08/24/01    rc      Added customer.h 
08/22/01    rc      Changed comment for tfc_valid field
08/21/01    rc      Added MACROs for calculating Length of log packets.
                    Defined LOG_WCDMA_BASE_C.
5/07/01     ps      Changed "hfn" from "uint8" to "uint32"
3/25/01     ps      Initial Release

================================================================================================*/

#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "uecomdef.h"
#include "log.h"
#include "log_codes_wcdma_int.h"

#ifdef T_WINNT
  #error code not present
#endif

#ifndef LOG_WCDMA_BASE_C 
#define LOG_WCDMA_BASE_C 0x4000
#endif

/* Log Cipher Parameter packet every 40 samples */
#define MAC_UL_MAX_CIPHER_LOG_CNT 40

/*----------------------------------------------------------------------------------------------
 Data structure for Transport Format Combination information
 ---------------------------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{

    /* Transport Format Control Flag */
    /* 0 - TFC_ALLOWED; 1 - TFC_NOT_ALLOWED */
    /* 2 - TFC_REMOVED */       
    /* Note - The Control Flag should match mac_ul_tfc_control_e_type */
    /* defined in macrrcif_v.h */
    uint8   tfc_valid;

    /* Transport Format Indicator */
    uint8   tfi[UE_MAX_TRCH];


}mac_log_tfc_type;

/*----------------------------------------------------------------------------------------------
 Data structure for Access Service Class information
 ---------------------------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{

    /* Access Service Class Identifier */
    uint8   identifier;

    /* Persistence value */
    /* 0 to 10 (0 indicates 0.0 and 10 indicates 1.0) */
    uint8   persistence;

}mac_log_asc_type;

/*----------------------------------------------------------------------------------------------
 Data structure for Logical Channel configuration info
 ---------------------------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{

    /* RLC ID */
    uint8   rlc_id;

    /* Logical channel type                                        */
    /* 0 - BCCH, 1- PCCH, 2 - CCCH,                                */
    /* 3 - DCCH, 4 - CTCH, 5 - DTCH                                */
    /* Note - The logical channel type should match the enum type  */
    /* uecomdef_logchan_e_type defined in uecomdef.h               */
    uint8   chan_type;

    /* Logical channel mode   */
    /* 0 - TM, 1 - UM; 2 - AM */
    uint8   chan_mode;

    /* Radio Bearer ID */
    uint8   rb_id;

    /* MAC ID (applicable only in UL ) */
    uint8   mac_id;

    /* MAC priority (applicable only in UL) */
    uint8   mac_priority;

}mac_log_lc_cfg_type;

/*----------------------------------------------------------------------------------------------
 Data structure for Transport channel configuration info
 ---------------------------------------------------------------------------------------------*/
#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{

    /* Transport Channel ID (1-32) */
    uint8   trch_id;

    /* Transport channel type                             */
    /* 0 - INVALID,1 - BCH, 4 - PCH, 5- FACH, 6 - DSCH,   */
    /* 7 - RACH, 8 - DCH                                  */
    uint8   trch_type;

    /* Number of logical channels mapped to this transport channel */
    uint8   num_logchan;

    /* Logical channel information */
    mac_log_lc_cfg_type     logical_chan_info[1];       

}mac_log_trch_cfg_info_type;

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{

  /* Radio bearer identity 0-31 */
  uint32     rb_id;

  uint32    raw_payload;

  uint32    avg_payload;

  uint32    var_payload;

}mac_log_tvm_meas_info_type;

#ifdef __GNUC__
typedef PACKED struct PACKED_POST
#else /* __GNUC__*/
typedef PACKED struct 
#endif /* __GNUC__ */
{

  uint32 countc;

  uint8  cfn;

  uint8 key_idx;
  
}mac_log_cipher_pkt_type;

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_UL_MAC_CFG_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/
/* Log code for the UL MAC Configuration log packet */

LOG_RECORD_DEFINE(WCDMA_UL_MAC_CFG_LOG_PACKET)
    
    /* Number of UL Transport channels */
    uint8           num_trch;  

    /* TM Logical Channel Ciphering Control Flag */
    /* TRUE indicates that ciphering is enabled  */
    boolean         ciphering_flag;

    /* Ciphering Key ID */
    uint8           ciphering_key_id;

    /* Hyperframe Number */
    uint32          hfn;

    /* Transport Channel Information */
    mac_log_trch_cfg_info_type       trch_info[UE_MAX_TRCH];

LOG_RECORD_END


/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_DL_MAC_CFG_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/
/* Log code for the DL MAC Configuration log packet */


LOG_RECORD_DEFINE(WCDMA_DL_MAC_CFG_LOG_PACKET)
    
    /* Number of UL Transport channels */
    uint8           num_trch;  

    /* TM Logical Channel Ciphering Control Flag */
    /* TRUE indicates that ciphering is enabled  */
    boolean         ciphering_flag;

    /* Ciphering Key ID */
    uint8           ciphering_key_id;

    /* Hyperframe Number */
    uint32          hfn;

    /* Transport Channel Information */
    mac_log_trch_cfg_info_type       trch_info[UE_MAX_TRCH];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_MAC_RACH_CFG_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/
/* Log code for MAC RACH Configuration log packet */


LOG_RECORD_DEFINE(WCDMA_MAC_RACH_CFG_LOG_PACKET)
    
    /* Maximum Number of RACH Preamble Cycles */
    uint8               max_num_preamble_cycles;  

    /* Minimum amount of Backoff time in TTIs */
    uint8               min_backoff;

    /* Maximum amount of Backoff time in TTIs */
    uint8               max_backoff;

    /* Number of Access Service Classes */
    uint8               num_asc;

    /* Access Service Class information */
    mac_log_asc_type    asc_info[MAX_ASC];

LOG_RECORD_END

/*----------------------------------------------------------------------------------------------
                   LOG PACKET: WCDMA_MAC_UL_TFCS_CFG_LOG_PACKET
 ---------------------------------------------------------------------------------------------*/
/* Log code for UL MAC TFCS Configuration log packet */


LOG_RECORD_DEFINE(WCDMA_MAC_UL_TFCS_CFG_LOG_PACKET)

    /* Number of Transport Format Combinations */
    uint16  num_tfc;

    /* Number of Transport Channels */
    uint8   num_trch;

    /* Transport channel IDs */
    uint8   trch_ids[UE_MAX_TRCH];

    /* Transport Channel TTI info                         */
    /* 0 - 10 msec; 1 - 20 msec; 2 - 40 msec; 3 - 80 msec */
    uint8   tti_types[UE_MAX_TRCH];

    /* Transport Format Combination info */
    mac_log_tfc_type    tfc_info[UE_MAX_TFC];

LOG_RECORD_END

/*--------------------------------------------------------------------------------------------
Log packet for the traffic volume reporting packet.
---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_UL_TVM_REPORT_LOG_PACKET)
  
  /* Measurement identity */
  uint8     meas_id;

  /* 0-Periodic, 1-Event Triggered */
  uint8     report_mode;

  /* 0- Event_4A, 1-Event_4B */
  uint8     event_id;

  /* 0-RACH, 1-DCH */
  uint8     trch_type;

  /* 1-32*/
  uint8     trch_id;

  /*In periodic mode, represents the number of reports sent */
  uint16    num_reports_sent;

  /* Number of radio bearers */
  uint8    num_rbs;
  
  /* Measurement results per radio bearer */
  mac_log_tvm_meas_info_type    meas_result_per_rb[MAX_RB];

LOG_RECORD_END

      

/*--------------------------------------------------------------------------------------------
Log packet for UL cipher info.
---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_UL_CIPHER_LOG_PACKET)

 uint8 version;
 
 uint8 nsamples;

 mac_log_cipher_pkt_type mac_ul_cipher_log[MAC_UL_MAX_CIPHER_LOG_CNT]; 

LOG_RECORD_END

/*--------------------------------------------------------------------------------------------
Log packet for UL cipher config info.
---------------------------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_MAC_UL_CIPHER_CONFIG_LOG_PACKET)

 uint8 version;
 uint8 cipher_type_algo;
 uint8 key_id;
 uint32 hfn;
 uint32 act_time; 

LOG_RECORD_END

/* UL cipher config info */
#define MAC_UL_CIPHER_CONFIG_PKT_LEN \
  FPOS(WCDMA_MAC_UL_CIPHER_CONFIG_LOG_PACKET_type, version) + \
       3*sizeof(uint8) + 2*sizeof(uint32)
       
/* UL cipher info */      
#define MAC_UL_CIPHER_PKT_LEN(nsamples) ( \
  FPOS(WCDMA_MAC_UL_CIPHER_LOG_PACKET_type, mac_ul_cipher_log)+ \
      (nsamples * sizeof(mac_log_cipher_pkt_type)) \
  )
      
/* Uplink MAC config */
#define MAC_UL_CFG_PACKET_LEN(num_trch, num_logchan) ( \
  FPOS(WCDMA_UL_MAC_CFG_LOG_PACKET_type, trch_info)+ \
      (num_trch * (sizeof(uint8) + sizeof(uint8) + \
       sizeof(uint8)))+ \
      ( num_logchan * sizeof(mac_log_lc_cfg_type))  \
  )

/* Downlink MAC config */
#define MAC_DL_CFG_PACKET_LEN(num_trch, num_logchan) ( \
  FPOS(WCDMA_DL_MAC_CFG_LOG_PACKET_type, trch_info)+ \
      (num_trch * (sizeof(uint8) + sizeof(uint8) + \
       sizeof(uint8)))+ \
      ( num_logchan * sizeof(mac_log_lc_cfg_type))  \
  )
      
/* RACH config */      
#define MAC_RACH_CFG_PACKET_LEN(num_asc) ( \
  FPOS(WCDMA_MAC_RACH_CFG_LOG_PACKET_type, asc_info)+ \
      (num_asc * sizeof(mac_log_asc_type)) \
  )

/* TFCS config */
#define MAC_UL_TFCS_CFG_PACKET_LEN(num_tfc, num_trch)(\
  FPOS(WCDMA_MAC_UL_TFCS_CFG_LOG_PACKET_type, trch_ids) +\
      (num_trch * sizeof(uint8)) + \
      (num_trch * sizeof(uint8)) + \
      (num_tfc * (sizeof(uint8) + (num_trch * sizeof(uint8)))) \
  )

/* Traffic volume report packet */
#define MAC_UL_TVM_REPORT_PACKET_LEN(num_rb)(\
  FPOS(WCDMA_MAC_UL_TVM_REPORT_LOG_PACKET_type, meas_result_per_rb) +\
  (num_rb*(sizeof(uint32)+sizeof(uint32)+sizeof(uint32)+sizeof(uint32)))) 
  
#ifdef T_WINNT
  #error code not present
#endif


#endif
