#ifndef WL2API_H 
#define WL2API_H

/*==========================================================================
             WCDMA L1 Public header file

DESCRIPTION
  This file contains definition, declaration required for to be exposed
  from WCDMA L2 as Public.

  Copyright (c) 2011 - 2011 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/wl2api.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
-------    ---    ---------------------------------------------------------- 
06/09/15   aa     Replaced RLC UM/AM/TM config log packets with new packets
05/15/15   aa     RLC optimization to trigger Status PDUs on all AM channels after LTA
12/20/13   grk    Moved timer callback processing from timer task to L2 UL 
                  task for TM timer based SDU discard.
10/30/13   scv    Enhancement for 3C-HSDPA
05/22/13   oh     Adding DC-HSUPA framework support
05/23/13   geg    WSW decoupling (except for component of QCHAT)
05/17/13   as     Enhanced interface for PURGE_REQ between L2 & DS.
10/31/12   kc     EFACH RRC-MAC interface redesign changes
10/01/12   pm    New enum RLC_SUSPEND/RLC_RESUME added in CMAC_UL_CONFIG
08/23/12   mrg    MAC-iis: Special HE functionality with LI 7 configured and 
                  RLC PDU size greater than 126 octets
08/22/12   kc     Added command for HS RACH MAC timer info logging support
06/19/12   mrg    L2 - MAC-I and HS-RACH code changes
04/26/12   grk    Added new API file exposing L2 command structures


===========================================================================*/

#include "mvssup.h"
#include "mvs.h"
#include "wcdmamvsif.h"
#include "uecomdef.h"


/*From 25.321, section 9.2.2,In FDD, when SDU from BCCH or PCCH logical 
*channel is transmitted on HS-DSCH the LCH-ID field in 
*MAC-ehs header is set to 1111
*/
#define 		BCCH_ON_HS_DSCH_LCH_ID		 0xF
#define 		PCCH_ON_HS_DSCH_LCH_ID		 0xF

#define MAC_EHS_QUEUE_ID_FOR_BCCH_PCCH	0xF 


/* -------------------------------------- */
/* PUBLIC API section of macl1rrcif.h     */
/* -------------------------------------- */

/*--------------------------------------------------------------------------------------*/
/* TFC Control Flag Type                                                                */
/*--------------------------------------------------------------------------------------*/
typedef enum
{
    TFC_ALLOWED = 0x00,     /* TFC is allowed */
    TFC_NOT_ALLOWED,        /* TFC is not allowed */
  TFC_MANDATORY,          /* This TFCI is part of the minimum set and hence can't be
                             subjected to the elimination due to CM by HLS or Tx power
                             restriction */
    TFC_REMOVED             /* TFC is removed */
} mac_ul_tfc_control_e_type;

/*-------------------------------------------------------------------------
  Indicates whether the PDU size is configured as Fixed/Flexible.
-------------------------------------------------------------------------*/
typedef enum 
{
  RLC_FIXED_PDU_SIZE = 0,               /* Fixed PDU size                  */
  RLC_FLEXIBLE_PDU_SIZE,                /* Flexible PDU size               */
  RLC_INVALID_PDU_SIZE                 /* Invalid PDU size                */
} rlc_pdu_size_type;


/*--------------------------------------------------------------------------------------*/
/* Uplink Transport Format                                                              */
/*--------------------------------------------------------------------------------------*/
typedef struct {

    /* Number of transport blocks */
    uint16              ntblks;

    /* Transport block size (includes MAC & RLC headers) in bits */
    uint16              tb_size;

    /* Transport Format Indicator Control Flag */
    /* TRUE indicates that TF is allowed and FALSE indicates TF not allowed */
    boolean             tf_flag;

} mac_ul_tf_type;

/*--------------------------------------------------------------------------------------*/
/* Uplink Transport Format Set                                                          */
/*--------------------------------------------------------------------------------------*/
typedef struct {

    /* TTI type of each transport channel */
    l1_tti_enum_type    tti_info;

    /* Number of transport formats */
    uint16              ntf;

    /* Each known Transport Format Combination */
    mac_ul_tf_type      tf_info[UE_MAX_TF];

} mac_ul_tfs_type;

/*--------------------------------------------------------------------------------------*/
/* Uplink Transport Format Combination                                                  */
/*--------------------------------------------------------------------------------------*/
typedef struct {

    /* Flag indicating whether the TFC is allowed (ALLOWED) or not  */
    /* allowed NOT_ALLOWED) or removed (REMOVED) */
    mac_ul_tfc_control_e_type   control_flag;

    /* CTFC value */
    uint32  ctfc;

    /* Transport format of each transport channel */
    uint8   tfi[UE_MAX_TRCH];

} mac_ul_tfc_type;

/*--------------------------------------------------------------------------------------*/
/* Uplink Transport Format Combination Set                                              */
/*--------------------------------------------------------------------------------------*/
typedef struct {

    /* Number of known transport format combinations */
    uint16  ntfc;

    /* Number of Transport Channels */
    uint8   nchan;

    /* Each known Transport Format Combination */
    mac_ul_tfc_type     tfc_info[UE_MAX_TFC];

    /* Each known Transport Format Combination */
    mac_ul_tfs_type     tfs_info[UE_MAX_TRCH];

    mvs_mode_type orderd_codec_mode;

} mac_ul_tfcs_type;

/* --------------------------------- */
/* PUBLIC APIs of l1rrcif.h          */
/* --------------------------------- */
/*===========================================================================

FUNCTION     l1m_is_connected

DESCRIPTION
  This function is called to check whether the UE is in a call (l1m_state is L1M_DCH)

DEPENDENCIES
  None

RETURN VALUE
  TRUE if l1m_state is L1M_DCH and FALSE otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean l1m_is_connected(void);

/* --------------------------------- */
/* PUBLIC APIs of rlcrrcif.h         */
/* --------------------------------- */

/*--------------------------------------------------------------------------- 
  Type for defining the action to take on a logical channel.  
---------------------------------------------------------------------------*/
typedef enum {
  RLC_ESTABLISH = 0,                    /* Initialize the protocol         */ 
                                        /* parameters as specified, and    */
                                        /* enter the data transfer ready   */
                                        /* state.                          */

  RLC_RELEASE,                          /* Reset all the protocol          */
                                        /* parameters, and exit the        */
                                        /* data transfer ready state.      */
  RLC_MODIFY,                           /* Modify the indicated protocol   */
                                        /* parameters as specified, and    */
                                        /* remain in the data transfer     */
                                        /* ready state.                    */

  RLC_NOOP
} rlc_channel_action_e_type; 

typedef enum {
  RLC_RE_ESTABLISH_DL_UL,               /* Both downlink and uplink of an  */
                                        /* AM entity is re-established     */
  RLC_RE_ESTABLISH_DL,                  /* Only downlink of an AM entity   */
                                        /* is re-established               */
  RLC_RE_ESTABLISH_UL,                  /* Only uplink of an AM entity is  */
                                        /* is re-established               */
  RLC_RE_ESTABLISH_NONE
}rlc_reestablish_e_type;

/*--------------------------------------------------------------------------- 
  Type that identifies if the HFN should be reported for 
  all established UM and AM uplink logical channels.
---------------------------------------------------------------------------*/
typedef enum {
   RLC_ALL = 0,                         /* Report HFN for all UM and AM    */
                                        /* channels                        */
   RLC_ONE                              /* Report HFN for a single UM or AM*/
                                        /* channel.                        */
} rlc_all_or_one_e_type;

/*--------------------------------------------------------------------------- 
  Type that defines the mode for discharge of the RLC buffer at
  the transmitter.
---------------------------------------------------------------------------*/
typedef enum {
   RLC_TBE = 0,                         /* Timer based with explicit       */
                                        /* signalling.                     */                                                                
   RLC_MAXDAT,                          /* Discard after MaxDAT            */
                                        /* retransmissions.                */
   RLC_NO_DISCARD                       /* No Discard (perform reset       */
                                        /* procedure after MaxDAT          */
                                        /* retransmissions.                */  
} rlc_discard_mode_e_type;

/*--------------------------------------------------------------------------- 
  Type that indicates the ciphering algorithm sent to RLC in case of
  RLC Re-establishment.
---------------------------------------------------------------------------*/
typedef enum {
  RLC_UEA_0 = 0,                        /* Indicates UEA_0 is to be used   */
  RLC_UEA_1 = 1,                         /* Indicates UEA_1 is to be used   */
  RLC_UEA_2 = 2,                        /* Indicates UEA_2/Snow3G to use   */
  RLC_UEA_INVALID = 0xFF 
} rlc_cipher_algo_e_type;


  /*-------------------------------------------------------------------------
    Type that indicates whether to use 7 bit LI or 15-bit LI or release 99 
    calculation for obtaining li length.
  -------------------------------------------------------------------------*/
typedef enum {
  LI_7_BITS = 0,                         /* 7 bits L1                     */
  LI_15_BITS,                            /* 15 bits LI                    */
  LI_SAME_AS_BEFORE,                     /* Use the previous one */
  LI_UNSPECIFIED                         /* Calculate as in Rel 99        */
} rlc_li_e_type;


typedef enum
{
  RLC_LOOPBACK_NA,     /* No loopback is required */
  RLC_LOOPBACK_MODE_1, /* loopback mode 1 */
  RLC_LOOPBACK_MODE_2  /* loopback mode 2 */
} rlc_loop_back_mode_e_type;

typedef enum
{
  RLC_PURGE_REQ_SUCCESS = 0,
  RLC_PURGE_REQ_FAIL_WM_NOT_REG,
  RLC_PURGE_REQ_INVALID_RSP
}rlc_ul_wm_purge_rsp_status;

typedef enum
{
  RLC_SUCC_IND,
  RLC_FAIL_IND,
  RLC_TA_SUCC_MAX_ENUM
}rlc_ta_success_e_type;

typedef enum
{
  RLC_LTA_START_IND,
  RLC_LTA_STOP_IND,
  RLC_QTA_START_IND,
  RLC_QTA_STOP_IND,
  RLC_TA_IND_MAX_ENUM
}rlc_ta_ind_e_type;

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/
typedef struct {
  rlc_channel_action_e_type action;     /* Action to take on the logical   */ 
                                        /* action : Establisha channel,    */
                                        /* release a channel, or Modify    */
                                        /* parameters a logical channel.   */
  boolean  act_incl;                    /* Flag indicating whether the     */
                                        /* specified configuration should  */  
                                        /* be activated at the time or not.*/
  uint8  act_time;                      /* The CFN at which the specified  */
                                        /* configuration should be         */ 
                                        /* activated. Only valid if the    */
                                        /* activation_time_incl = TRUE;    */
  uint16 ul_cfg_mask;
  uint16 dl_cfg_mask;
                                        
} rlc_lc_action_type;

/* UPLINK */
/*---------------------------------------------------------------------------
  Type to establish a single transparent mode logical channel. 
---------------------------------------------------------------------------*/


typedef struct {                          
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/
                                        /* RLC logical channel             */ 
  rb_id_type rb_id;                     /* Radio Bearer ID                 */
  boolean seg_ind;                      /* Flag that indicates whether     */
                                        /* segmentation should be performed*/ 
                                        /* on the logical channels being   */
                                        /* configured.                     */
  uecomdef_logchan_e_type lc_type;      /* Type of Uplink Transparent Mode */
                                        /* logical channel                 */
  uint16  timer_discard;                /* Elapsed time in milliseconds    */
                                        /* before an SDU is discarded at   */ 
                                        /* the transmitter                 */
} rlc_ul_tm_channel_config_type;


/*---------------------------------------------------------------------------
  Type to establish, release or re-configure Transparent mode uplink 
  logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8  nchan;                         /* Number of transparent mode UL   */ 
                                        /* logical channels to configure   */
  rlc_ul_tm_channel_config_type  chan_info[UE_MAX_UL_TM_CHANNEL];
                                        /* Uplink transparent mode RLC     */
                                        /* configuration for each channel  */
  rlc_lc_action_type ul_tm_act[UE_MAX_UL_TM_CHANNEL];       
                                        /* Action related information for  */
                                        /* RLC UL TM.                      */
  rlc_loop_back_mode_e_type loop_back_mode; /* Indicates if channels are to be */
                                            /* configured for loopback or not  */
} rlc_ul_tm_config_type;


/*---------------------------------------------------------------------------
  Type to establish, release or re-configure a single unacknowledged 
  Mode uplink logical channel. 
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the Uplink*/
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  rb_id_type rb_id;                     /* Radio Bearer ID                 */
  uecomdef_logchan_e_type lc_type;      /* Type of Uplink Unacknowledged   */
                                        /* mode logical channel            */
  uint16 timer_discard;                 /* Elapsed time in milliseconds    */
                                        /* before an SDU is discarded at   */
                                        /* the transmitter                 */
  uint32 rlc_size;                      /* Max. PDU size.                  */
  uint32 hfn;                           /* Hyperframe Number               */
  uecomdef_wcdma_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */

  boolean alternate_e_bit;          /* TRUE - Alternate E-bit interpretation */
                                    /* FALSE - Normal E-bit interpretation   */
  rlc_pdu_size_type ul_flex_pdu_size;    /* Fixed/Flexible PDU size         */
  uint16            ul_flex_min_pdu_size;/* Minimum PDU size                */
  uint16            ul_flex_max_pdu_size;/* Maximum PDU size                */

} rlc_ul_um_channel_config_type;


/*---------------------------------------------------------------------------
  Type to establish, release or re-configure Unacknowledged mode uplink
  logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  byte  nchan;                          /* Number of Unacknowledged mode   */
                                        /* uplink logical channels to      */
                                        /* configure                       */
  rlc_ul_um_channel_config_type chan_info[UE_MAX_UL_UM_CHANNEL];
                                        /* Uplink Unacknowledged mode RLC  */
                                        /* configuration for each channel  */
  rlc_lc_action_type ul_um_act[UE_MAX_UL_UM_CHANNEL];       
                                        /* Action related information for  */
                                        /* RLC UL UM.                      */
} rlc_ul_um_config_type;


/*---------------------------------------------------------------------------
  Type to enable or disable ciphering on a single unacknowledged or 
  acknowledged mode uplink logical channel. 
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the Uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  boolean cipher_mode;                  /* Indicates whether this logical  */
                                        /* channel should be ciphered (ON) */
                                        /* or not (OFF)                    */
  uint32  cipher_key_id;                /* Ciphering key Id. Valid if      */
                                        /* ciphering mode is ON            */
  uint32  hfn;                          /* Ciphering Hyperframe number.    */
  rb_id_type  rb_id;                    /* Radio Bearer Id. Valid only if  */
                                        /* Ciphering mode is ON            */
  uint16  ciph_act_sn;                  /* Serial Number at which Ciphering*/
                                        /* will be activated.              */
  uecomdef_wcdma_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */
} rlc_ul_ch_ciphering_type;


/*---------------------------------------------------------------------------
  Type to enable or disable ciphering on one or more unacknowledged or 
   acknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/
                                        /* channels to configure           */
  rlc_ul_ch_ciphering_type  ciphering[UE_MAX_UL_UM_CHANNEL + 
                                                         UE_MAX_AM_ENTITY];
                                        /* Ciphering Information for each  */
                                        /* unacknowledged or acknowledged  */
                                        /* mode uplink logical channel     */
} rlc_ul_cipher_config_type;

/*---------------------------------------------------------------------------
  Type to resume uplink data transmission on one or more 
  unacknowledged or acknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/ 
                                        /* channels to configure           */
  rlc_lc_id_type  channel[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* Uplink logical channels to      */
                                        /* resume                          */
} rlc_ul_resume_type;

/*---------------------------------------------------------------------------
  Type to Continue uplink data transmission on one or more 
  unacknowledged or acknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/ 
                                        /* channels to configure           */
  rlc_lc_id_type  channel[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* Uplink logical channels to      */
                                        /* continue                          */
} rlc_ul_continue_type;

/*---------------------------------------------------------------------------
  Type to Continue DownLink data transmission on one or more 
  unacknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/ 
                                        /* channels to configure           */
  rlc_lc_id_type  channel[UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* Uplink logical channels to      */
                                        /* continue                          */
} rlc_dl_continue_type;


/*---------------------------------------------------------------------------
  Type to request the current HFN value for all established uplink 
  logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_all_or_one_e_type  all_or_one;    /* Identifies if the HFN should be */
                                        /* reported for all channels or    */
                                        /* the specified channel.          */
  rlc_lc_id_type lc_id;                 /* Identifies the uplink channel   */
                                        /* whose HFN value is requested.   */
                                        /* Only valid if all_one is set to */
                                        /* ONE                             */
} rlc_ul_hfn_type;

/*---------------------------------------------------------------------------
  Type to establish a Transparent Mode downlink channel.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the       */
                                        /* Downlink RLC logical channel    */
  rb_id_type rb_id;                     /* Radio Bearer ID                 */
  boolean seg_ind;                      /* Flag that indicates whether     */
                                        /* segmentation should be performed*/
                                        /* on the logical channels being   */
                                        /* configured                      */
  uecomdef_logchan_e_type lc_type;      /* Type of logical channel         */
} rlc_dl_tm_channel_config_type;


/*---------------------------------------------------------------------------
  Type to establish, release or re-configure transparent 
  Mode downlink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8  nchan;                         /* Number of transparent mode      */
                                        /* downlink logical channels to    */
                                        /* configure                       */
  rlc_dl_tm_channel_config_type chan_info[UE_MAX_DL_TM_CHANNEL];
                                        /* Downlink transparent mode RLC   */
                                        /* configuration for each channel  */
  rlc_lc_action_type dl_tm_act[UE_MAX_DL_TM_CHANNEL];
                                        /* Action related information for  */
                                        /* RLC DL TM.                      */

} rlc_dl_tm_config_type;

/*---------------------------------------------------------------------------
  Duplicate Avoidance & Re-ordering parameters.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 dar_window_size;                /* Size of DAR receiving window    */
  uint16 dar_timer;                     /* Trigger tx of PDUs to the next  */
                                        /* in-seq UM-RLC receive function  */
}rlc_dar_info_type;

/*---------------------------------------------------------------------------
  Out of Sequence Delivery parameters.
---------------------------------------------------------------------------*/
 typedef struct
{
  uint8 osd_window_size;                /* Size of DAR receiving window    */
  boolean osd_timer_present;            /* Indicate presence of osd timer  */
  uint16 osd_timer;                     /* Trigger deletion of stored PDUs */
}rlc_osd_info_type;
 
/*---------------------------------------------------------------------------
  Type to establish, release or re-configure a single 
  unacknowledged Mode downlink logical channel.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the       */
                                        /* Downlink RLC logical channel    */
  rb_id_type rb_id;                     /* Radio Bearer ID                 */
  uecomdef_logchan_e_type lc_type;      /* Type of Downlink Unacknowledged */
                                        /* Mode logical channel            */
  uint32 rlc_size;                      /* Max. PDU size.                  */
  uint32 hfn;                           /* Hyperframe Number               */
  uecomdef_wcdma_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */

  rlc_li_e_type li_size;                /* Network can explicitly specify
                                           li size in Release 5            */
  boolean osd_info_present;             /* Boolean to indicate DAR presence*/
  rlc_osd_info_type osd_info;           /* DAR information for MTCH        */
  boolean dar_info_present;             /* Boolean to indicate OSD presence*/
  rlc_dar_info_type dar_info;           /* OSD information for MCCH        */

  boolean alternate_e_bit;          /* TRUE - Alternate E-bit interpretation */
                                    /* FALSE - Normal E-bit interpretation   */
} rlc_dl_um_channel_config_type;


/*---------------------------------------------------------------------------
  Type to establish, release or re-configure unacknowledged 
  Mode downlink logical channels.
---------------------------------------------------------------------------*/
   
typedef struct {
  uint8  nchan;                         /* Number of transparent mode DL   */ 
                                        /* logical channels to configure   */
  rlc_dl_um_channel_config_type chan_info[UE_MAX_DL_UM_CHANNEL];
                                        /* Downlink Unacknowledged mode RLC*/ 
                                        /* configuration for each channel  */
  rlc_lc_action_type dl_um_act[UE_MAX_DL_UM_CHANNEL];       
                                        /* Action related information for  */
                                        /* RLC DL UM.                      */
} rlc_dl_um_config_type;

/*---------------------------------------------------------------------------
  Type to enable or disable ciphering on a single unacknowledged or  
  acknowledged mode downlink logical channel.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the DL    */ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  boolean cipher_mode;                  /* Indicates whether this logical  */
                                        /* channel should be ciphered (ON) */
                                        /* or not (OFF)                    */
  uint32 cipher_key_id;                 /* Ciphering key Id. Valid if      */
                                        /* ciphering mode is ON            */
  uint32 hfn;                           /* Ciphering Hyperframe number.    */
  rb_id_type rb_id;                     /* Radio Bearer Id. Valid only if  */
                                        /* Ciphering mode is ON            */
  uint16  ciph_act_sn;                  /* Serial Number at which Ciphering*/
                                        /* will be activated for Downlink  */
  uecomdef_wcdma_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */
} rlc_dl_ciphering_type;


/*---------------------------------------------------------------------------
  Type to enable or disable ciphering on one or more unacknowledged or 
  acknowledged mode downlink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged mode   */
                                        /* downlink logical channels to    */
                                        /* configure                       */
  rlc_dl_ciphering_type ciphering[UE_MAX_DL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* Ciphering Information for each  */
                                        /* unacknowledged mode downlink    */
                                        /* logical channel                 */
} rlc_dl_cipher_config_type;


/*---------------------------------------------------------------------------
  Type to request the current HFN value for all established
  downlink AM and UM logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_all_or_one_e_type  all_or_one;    /* Identifies if the HFN should be */
                                        /* reported for all channels or    */
                                        /* the specified channel.          */
  rlc_lc_id_type lc_id;                 /* Identifies the downlink channel */
                                        /* whose HFN value is requested.   */
                                        /* Only valid if all_one is set to */
                                        /* ONE                             */
} rlc_dl_hfn_type;

/*--------------------------------------------------------------------------- 
  Common config information for both Uplink and Downlink for AM.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type ul_data_id;            /* Unique identifier for the uplink*/
                                        /* RLC logical channel for         */
                                        /* transporting data PDUs.         */
  rlc_lc_id_type ul_control_id;         /* Unique identifier for the uplink*/
                                        /* RLC logical channel for         */
                                        /* transporting control PDUs.      */
  rlc_lc_id_type dl_data_id;            /* Unique identifier for the       */
                                        /* downlink RLC logical channel    */
                                        /* for transporting data PDUs.     */
  rlc_lc_id_type dl_control_id;         /* Unique identifier for the       */
                                        /* downlink RLC logical channel    */
                                        /* for transporting control PDUs.  */
  rb_id_type  rb_id;                    /* Radio Bear ID                   */
  
  uecomdef_logchan_e_type lc_type;      /* Type of Downlink Acknowledged   */
                                        /* Mode logical channel            */
  boolean rlc_one_sided_reest;          /* One-sided Re-establishment flag */
} rlc_am_common_config_type;

/*---------------------------------------------------------------------------
  Type for the Transmission RLC Discard Information.
---------------------------------------------------------------------------*/
typedef struct {
  boolean tx_discard_incl;              /* Indicates whether transmission  */
                                        /* RLC discard parameters are      */
                                        /* included or not                 */
  rlc_discard_mode_e_type discard_mode; /* Mode for discharge of the RLC   */
                                        /* buffer at the transmitter       */
  uint16 timer_discard;                 /* Elapsed time in ms before a SDU */
                                        /* is discarded. Valid only if     */
                                        /* discard mode is Timer based     */
                                        /* explicit or MaxDAT              */                        
  uint16 timer_mrw;                     /* Used to trigger the retx of a   */
                                        /* STATUS PDU containing           */
                                        /* an MRW SUFI field.              */
  uint8 maxdat;                         /* Maximum number of retx of a PDU */
                                        /* before the SDU that is belongs  */
                                        /* to is discarded.                */
  uint8 maxmrw;                         /* Maximum number of retx of a PDU */
                                        /* before the SDU that is belongs  */
                                        /* to is discarded.                */
} rlc_discard_info_type;


/*---------------------------------------------------------------------------
  Type for polling information.
---------------------------------------------------------------------------*/
typedef struct {
  boolean poll_info_incl;               /* Indicates whether polling info  */
                                        /* parameters are included or not. */
  uint16 poll_prohibit;                 /* Minimum time between polls, in  */
                                        /* milliseconds. A value of '0'    */
                                        /* indicates that this timer is    */
                                        /* disabled.                       */
  uint16 timer_poll;                    /* Poll retransmission timeout, in */
                                        /* milliseconds. A value of '0'    */
                                        /* indicates that this timer is    */
                                        /* disabled.                       */
  uint8 poll_pdu;                       /* Interval between polls, in units*/ 
                                        /* of PDUs. A value of'0'indicates */
                                        /* that the Poll Every Poll_PDU    */
                                        /* mechanism is disabled.          */
  uint8 poll_sdu;                       /* Interval between polls, in units*/
                                        /* of SDUs, A value of  '0'        */
                                        /* indicates that the Poll Every   */
                                        /* Poll_SDU mechanism is disabled. */
  boolean last_tx_pdu_poll;             /* Indicates if a poll should be   */
                                        /* sent when the last PDU in the   */
                                        /* tx buffer is transmitted.       */      
  boolean last_retx_pdu_poll;           /* Indicates if a poll should be   */
                                        /* sent when the last PDU in the   */
                                        /* retx buffer is transmitted.     */
  uint8 poll_window;                    /* When poll_window% of the        */
                                        /* transmission window is reached, */
                                        /* a poll should be transmitted.   */
                                        /* A value of  '0' indicates that  */
                                        /* the Poll_Window% polling        */
                                        /* mechanism is disabled.          */
  uint16 timer_poll_periodic;           /* Interval between polls, in ms,  */
                                        /* for periodic polling. A value   */
                                        /* of '0' indicates that periodic  */
                                        /* polling is disabled.            */
} rlc_polling_info_type;



/*---------------------------------------------------------------------------
  Type for defining the RLC information parameters
---------------------------------------------------------------------------*/
typedef struct {
  boolean rlc_info_incl;                /* Indicates whether RLC info      */
                                        /* parameters are included or not. */
                                        /* If not included, the previous   */
                                        /* configuration remains unchanged.*/
  uint16 tx_window_size;                /* Transmission window size- The   */
                                        /* maximum number of RLC PDUs the  */
                                        /* transmitter is permitted to     */ 
                                        /* send, without receiving an      */
                                        /* acknowledgement.                */
  uint16 timer_rst;                     /* Timer for detecting loss of     */
                                        /* RESET ACK PDU                   */
  uint8 maxrst;                         /* Maximum number of retx          */
                                        /* of the RESET PDU                */
  rlc_discard_info_type discard_info;   /* Transmission RLC Discard info   */
  rlc_polling_info_type poll_info;      /* Polling information             */
} rlc_ul_info_type;

/*--------------------------------------------------------------------------- 
  Type to establish, release or re-configure a single acknowledged 
  mode entity.
---------------------------------------------------------------------------*/
typedef struct {
  uint32 hfn;                           /* Hyperframe Number               */
  uint32 ul_rlc_size;                   /* Max. PDU size.                  */
  rlc_ul_info_type rlc_info;            /* RLC information                 */
  uecomdef_wcdma_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */
  boolean special_value_he_config;      /* Special value for HE Config ?   */
  rlc_li_e_type     ul_li_size;          /* LI size                         */
  rlc_pdu_size_type ul_flex_pdu_size;    /* Fixed/Flexible PDU size         */
  uint16            ul_flex_min_pdu_size;/* Minimum PDU size                */
  uint16            ul_flex_max_pdu_size;/* Maximum PDU size                */

} rlc_ul_am_config_type;

/*---------------------------------------------------------------------------
  Type for the Downlink RLC Status information.
---------------------------------------------------------------------------*/
typedef struct {
  boolean status_info_incl;             /* Indicates whether status info   */
                                        /* parameters are included or not. */
                                        /* If not included, the previous   */
                                        /* configuration remains unchanged.*/
  uint16 status_prohibit;               /* Minimum time between status     */
                                        /* reports, in ms. A value         */
                                        /* of '0' indicates that this timer*/ 
                                        /* is disabled.                    */
  uint16 timer_epc;                     /* Time to wait, in milliseconds,  */
                                        /* before the estimated PDU counter*/ 
                                        /* starts to decrement. A value of */
                                        /* '0' indicates that the EPC      */
                                        /* mechanism is disabled.          */
  boolean missing_pdu;                  /* Indicates if the UE should send */ 
                                        /* a STATUS report for each missing*/ 
                                        /* PDU that is detected.           */
  uint16 status_periodic;               /* Time between status reports, in */
                                        /* ms, for periodic status         */ 
                                        /* reporting. A value of '0'       */
                                        /* indicates that status reporting */ 
                                        /* is disabled.                    */
} rlc_status_info_type; 

/*---------------------------------------------------------------------------
  Type for the RLC downlink information parameters
---------------------------------------------------------------------------*/
typedef struct {
  boolean rlc_info_incl;                /* Indicates whether RLC info      */
                                        /* parameters are included or not. */  
                                        /* If not included, the previous   */
                                        /* configuration remains unchanged.*/
  boolean in_sequence;                  /* Indicates if RLC should preserve*/ 
                                        /* the order of higher layer SDUs. */
  uint16 rx_window_size;                /* The maximum number of RLC PDUs  */
                                        /* allowed to be received.         */
  rlc_status_info_type status_info;     /* Status information for a        */
                                        /* downlink AM entity              */
} rlc_dl_info_type;

/*--------------------------------------------------------------------------- 
  Type to establish, release or re-configure a single acknowledged 
  mode downlink entity.
---------------------------------------------------------------------------*/

typedef struct {
  uint32 hfn;                           /* Hyperframe Number               */
  uint32 dl_rlc_size;                   /* Max. PDU size.                  */
  rlc_dl_info_type rlc_info;            /* Typedef for RLC downlink        */
  /* TRUE - FLEXIBLE pdu size, FALSE - FIXED pdu size
   * dl_rlc_size is valid only in case of FIXED pdu size
   * am_li_type is valid only in case of FLEXIBLE pdu size */
  boolean                               am_pdu_size_flexbile;
  rlc_li_e_type                         am_li_type;
  boolean special_value_he_config;      /* Special Value for HE Config ?   */                                        /* information                     */  uecomdef_wcdma_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */
} rlc_dl_am_config_type;

/*--------------------------------------------------------------------------- 
  Type to establish, release or re-configure a single acknowledged 
  mode entity.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_am_common_config_type common_config;/* Common config for both        */
                                          /* Uplink and Downlink for AM    */
  
  uint8  ul_nchan;                        /* Number of Acknowledged mode   */
                                          /* uplink logical channels to    */
                                          /* configure                     */
  rlc_ul_am_config_type rlc_ul_am_config; /* Config info for UL only       */
  uint8  dl_nchan;                        /* Number of Acknowledged mode   */
                                          /* downlink logical channels to  */
                                          /* configure                     */
  rlc_dl_am_config_type rlc_dl_am_config; /* Config info for DL only       */
  rlc_reestablish_e_type reestab_entity;  /* This variable is used when    */
                                          /* action = RLC_RE_ESTABLISH only*/
} rlc_am_config_type;

/*---------------------------------------------------------------------------
  Type to establish, release or re-configure of acknowledged mode entities.
---------------------------------------------------------------------------*/
typedef struct {
  uint8  nentity;                       /* Number of acknowledged mode     */
                                        /* entities to configure           */
  rlc_am_config_type *am_info[UE_MAX_AM_ENTITY];
                                        /* Downlink transparent mode RLC   */
                                        /* configuration for each channel  */
  rlc_lc_action_type am_act[UE_MAX_AM_ENTITY];
                                        /* Action related information for  */
                                        /* RLC UL TM.                      */
} rlc_am_config_req_type;

typedef struct {
  boolean block_status_timer;       /* whether to block status tmr*/
  uint16  num_lc;
  rlc_lc_id_type lc_id[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                   /*plink logical channels         */
} rlc_ul_register_for_data_ind_type ;

typedef struct {
  boolean resume_status_timer;       /* to resume status tmr*/
  uint16  num_lc;
  rlc_lc_id_type lc_id[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                   /*plink logical channels         */
} rlc_ul_unregister_for_data_ind_type ;

typedef struct {

  /* number of logical channels for which watermark is to be purged */
  uint8          num_lc_id;       

  /* LC ID's of the channels for which WM is to be purged           */
  rlc_lc_id_type lc_id[MAX_RB];

  /* Number of SDUs for each LC which are to be purged */
  uint8          num_sdus[MAX_RB];

  void  *bearer_context;

  void (*purge_ind_cb) (rlc_lc_id_type             lc_id,
                        rlc_ul_wm_purge_rsp_status purge_status,
                        void*                      context);

}rlc_ul_purge_wm_ind_type;

/*---------------------------------------------------------------------------
  Type to specify the logical channels for which the ongoing security
  procedure needs to be aborted.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/
                                        /* channels to abort config on.    */
  rlc_lc_id_type  lc_id[UE_MAX_UL_UM_CHANNEL + UE_MAX_AM_ENTITY];
                                        /* Logical channel id's for which 
                                           ciphering configuration needs to 
                                           be aborted. Since MAX UM channels 
                                           for UL and DL are same, we use 
                                           the same struct for UL and DL   */
                                        
} rlc_abort_cipher_config_type;

/*---------------------------------------------------------------------------
  Type to specify the start SFN and end SFN of the modification period that 
  has just ended. 
---------------------------------------------------------------------------*/
typedef struct
{
  rlc_lc_id_type lc_id;
  uint32 mod_prd_start_sfn;
  uint32 mod_prd_end_sfn;
}rlc_mcch_mod_prd_end_ind_type;

/* --------------------------------- */
/* PUBLIC APIs of rlci.h             */
/* --------------------------------- */

/*--------------------------------------------------------------------------- 
  Structure to carry the config information.
---------------------------------------------------------------------------*/
typedef enum {

  RLCI_ESTABLISH = 0,                    /* Initialize the protocol         */ 
                                        /* parameters as specified, and    */
                                        /* enter the data transfer ready   */
                                        /* state.                          */
  RLCI_RE_ESTABLISH,                     /* Re-initialize the protocol      */ 
                                        /* parameters as specified.        */
  RLCI_RELEASE,                          /* Reset all the protocol          */
                                        /* parameters, and exit the        */
                                        /* data transfer ready state.      */
  RLCI_MODIFY,                           /* Modify the indicated protocol   */
                                        /* parameters as specified, and    */
                                        /* remain in the data transfer     */
                                        /* ready state.                    */
  RLCI_STOP,                             /* Stop all of RX/TX for UM/AM     */
  RLCI_CONTINUE,                          /* Resume back to normal after STOP*/
  RLCI_NO_ACTION
} rlc_channel_action_cnf_e_type; 

typedef struct {
  rlc_am_config_req_type am_config_req;
  uecomdef_status_e_type result[UE_MAX_UL_AM_DATA_CHANNEL];        
                                        /* Config success or fail         */
} rlci_dl_am_config_req_type;

typedef struct {
  rb_id_type  rb_id;
  rlc_channel_action_cnf_e_type action;
  uecomdef_status_e_type result;        /* Config success or fail         */
  uint8 ul_data_id;                     /* Unique identifier for the uplink
                                           RLC logical channel for 
                                           transporting data PDUs. */
} rlci_dl_config_cnf_info_type;

typedef struct {
  uint8 nchan;
  rlci_dl_config_cnf_info_type am_cnf[UE_MAX_UL_AM_DATA_CHANNEL];
} rlci_dl_config_cnf_type;

/* --------------------------------- */
/* PUBLIC APIs of rlcsdapif.h        */
/* --------------------------------- */

/*---------------------------------------------------------------------------
  Type for registration of the SDU confirmation callback function type.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                  /* Unique identifier for the      */
                                         /* uplink RLC logical channel.    */
                                         /* Can have a value from 0 to 18. */

  /* the callback function to tell RRC this specific SDU w/ MUI has rx'd   */
  /* correctly or not by the other side.                                   */
  void (*rlc_am_sdu_cnf_cb) (rlc_lc_id_type ul_data_lc_id,
                                         /* data logic channel ID.         */
                             uecomdef_status_e_type status,
                                         /* Successful or failed           */
                             uint8  mui, /* MUI of this SDU.               */
                             rlc_sdu_tx_fail_reason_e_type failure_reason
                            );
} rlc_am_sdu_cnf_cb_fcn_type;

typedef struct {
  uint8 nchan;                          /* Number of uplink logical        */
                                        /* channels to register            */
  rlc_am_sdu_cnf_cb_fcn_type  sdu_cnf_cb[UE_MAX_UL_AM_DATA_CHANNEL];
                                        /* Registration details for each   */
                                        /* logical channel                 */
} rlc_register_am_sdu_confirm_callback_type;


/*---------------------------------------------------------------------------
  Type for registration of the SDU discard callback function type.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                  /* Unique identifier for the      */
                                         /* downlink RLC logical channel.  */
                                         /* value range from 0 to 18.      */
  void (*rlc_am_sdu_discard_cb) (rlc_lc_id_type dl_data_lc_id,
                                         /* downlink data logical chan ID  */
                                 uint8 mui  /* MUI of the SDU              */
                                );
} rlc_am_sdu_discard_cb_fcn_type;

typedef struct {
  uint8 nchan;                           /* Number of downlink logical     */
                                         /* channels to register           */
  rlc_am_sdu_discard_cb_fcn_type  sdu_discard_cb[UE_MAX_DL_AM_DATA_CHANNEL];
                                        /* Registration details for each   */
                                        /* logical channel                 */
} rlc_register_am_sdu_discard_callback_type;

typedef  struct {
  void *cleanup_wm_para;

  rlc_lc_id_type lc_id;

  void  (*cleanup_wm_cb)( void *call_instance);

} rlci_cleanup_wm_cmd_type;

typedef struct{
  rlc_ta_ind_e_type tune_away_ind;
  rlc_ta_success_e_type tune_away_succ;
}rlc_ta_info_type;

/* --------------------------------- */
/* PUBLIC APIs of macrlcif.h         */
/* --------------------------------- */

typedef struct{
   uint8  nchan;

   rlc_lc_id_type rlc_id_list[UE_MAX_DL_LOGICAL_CHANNEL];

} mac_hs_dl_reset_req_type;

/* --------------------------------- */
/* PUBLIC APIs of macrrcif.h         */
/* --------------------------------- */

/*========================================================================================

                       CONSTANT TYPE DATA DECLARATIONS

========================================================================================*/
/* Maximum number of CCTrChs */
  #define         MAC_DL_MAX_CCTRCH                       3

/* Maximum number of domains. Domain Index 0(CS) 1(PS) */
  #define         MAX_CN_DOMAIN                           2

  #define MAC_E_TFCI_MIN_SET_NOT_PRESENT                  0xFF

 #define         MAX_MTCH_PER_TRCH                       8
/*========================================================================================

                       ENUMERATED TYPE DATA DECLARATIONS

========================================================================================*/
/*--------------------------------------------------------------------------------------*/
/* UL CCTrCh type                                                                                                                                               */
/*--------------------------------------------------------------------------------------*/

typedef enum
{
  INVALID_UL_CCTRCH = 0,  /* Invalid CCTrCh */
  RACH,                   /* RACH type CCTrCh */
  DCH,                     /* DCH type CCTrCh */
  EDCH                     /* EDCH type CCTrCh */  
}mac_ul_cctrch_e_type;

/*--------------------------------------------------------------------------------------*/
/* RNTI type                                                                            */
/*--------------------------------------------------------------------------------------*/
typedef enum
{
  NO_RNTI_VALID,          /* No RNTI is valid */
  CRNTI_VALID,            /* C-RNTI is valid */
  URNTI_VALID,            /* U-RNTI is valid */
  BOTH_VALID              /* U-RNTI and C-RNTI are valid */
} mac_rnti_valid_e_type;

/*--------------------------------------------------------------------------------------*/
/* UL Traffic volume reporting mode                                                     */
/*--------------------------------------------------------------------------------------*/
typedef enum
{
  NO_REPORTING = 0x00,        /* reporting disabled */
  PERIODIC_MODE,              /* Periodic reporting */
  EVENT_TRIGGERED_MODE        /* Event-triggered reporting */
}mac_ul_traffic_report_mode_e_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement quantity type                                             */
/*--------------------------------------------------------------------------------------*/
typedef enum
{
  RLC_BUF_PAYLOAD,           /* MAC is reporting the raw buffer payload to RRC */
  AVERAGE_RLC_BUF_PAYLOAD,   /* Average buffer payload */
  VARIANCE_RLC_BUF_PAYLOAD   /* Variance of rlc buffer payload */
}mac_ul_traffic_meas_qty_e_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume event type                                                            */
/*--------------------------------------------------------------------------------------*/
typedef enum
{
  INVALID_EVENT,
  EVENT_4A,         /* The raw rlc buffer payload exceeds a threshold value */
  EVENT_4B          /* The raw rlc buffer payload is smaller than the threshold value*/
}mac_ul_traffic_event_id_e_type;

/*-------------------------------------------------------------------------------------*/
/* RRC may ask MAC to start, modify or delete a traffic volume measurement for a       */
/* measurement identity                                                                */
/*-------------------------------------------------------------------------------------*/

typedef enum
{
  MAC_TRAFFIC_MEAS_START,      /* Start doing traffic volume measurements     */
  MAC_TRAFFIC_MEAS_MODIFY,     /* Modify an existing measurement              */
  MAC_TRAFFIC_MEAS_DELETE,     /* DElete all measurements associated with the */
  /* measurement identity                        */
  MAC_TRAFFIC_MEAS_DELETE_ALL, /* Deletes all the exisiting measurement ids   */
  MAC_TRAFFIC_MEAS_SAVE,       /* Save the measurement info */
  MAC_TRAFFIC_MEAS_RESUME      /* Resume measurement */
}mac_ul_traffic_cmd_e_type;

/*-------------------------------------------------------------------------------------*/
/*  The traffic volume measurement report could be transferred in either UM or AM mode */
/*-------------------------------------------------------------------------------------*/

typedef enum
{
  ACK_MODE,           /* RLC Acknowledged Mode   */
  UNACK_MODE          /* RLC Unacknowledged mode */

}mac_ul_traffic_report_transfer_mode_e_type;

/*-------------------------------------------------------------------------------------*/
/*  Indicates if it is a CS domain or a PS Domain.                                     */
/*-------------------------------------------------------------------------------------*/
typedef enum
{
  CS_DOMAIN,
  PS_DOMAIN
}mac_ul_cipher_domain_e_type;

typedef enum
{
  MAC_LOOPBACK_NA,     /* No loopback is required */
  MAC_LOOPBACK_MODE_1, /* loopback mode 1 */
  MAC_LOOPBACK_MODE_2  /* loopback mode 2 */
} mac_loop_back_mode_e_type;

typedef enum
{
  INVALID_CFG=0,
  NON_HO_CIPHER_CFG,    /* Normal case */
  HO_STEP1_CIPHER_CFG,  /* Donot increment the HFN even if the activation time is reached */
  HO_STEP2_CIPHER_CFG,  /* Increment the HFN by 1 at activation time. Subsequently the behaviour
                        remains the same as in NO_HO_CIPHER_CFG */
  NON_HO_STEP2_CIPHER_CFG, /*Used for new TM RB being set first time. MAC needs to get the HFN 
                             that needs to be applied at activation time from RRC */
  SMC_INITIATED_NON_HO_CIPHER_CFG
                        /* to be sent by RRC when TM RBs exist at the time when
                              ciphering is enabled by the network. On receipt of this
                              config message MAC should not do ciphering(either with
                              uea0 or uea1) till the activation time expires. At
                              activation time MAC should initialize COUNT-C using
                              HFN value sent in the config message */
}mac_cipher_cfg_e_type;

/*--------------------------------------------------------------------------------------
Ciphering algorithms UE_0 and UE_1. When MAC is confirgured with UE_) even if ciphering is
enabled, ciphering should not be performed. UE_1 implies ciphering has to be performed.
---------------------------------------------------------------------------------------*/
typedef enum
{
  MAC_CIPHER_ALGO_UE_0 = 0,
  MAC_CIPHER_ALGO_UE_1

}mac_cipher_algo_e_type;


typedef enum
{
  MIN_ALLOWED_TFCI=0,
  ALLOWED_TFCI,
  NOT_ALLOWED_TFCI,
  RESTRICTED_TFCI,
  FULL_TFCI
}mac_tfc_subset_restriction_choice_e_type;


/* HSDPA Action Variable */
typedef enum
{
  HSDPA_NOOP,
  HSDPA_START,
  HSDPA_RECONFIG,
  HSDPA_STOP
}hsdpa_action_enum_type;

/* HSUPA Action Variable */
typedef enum
{
  MAC_SEC_EUL_NOOP = 0x0,
  MAC_SEC_EUL_START = 0x1,
  MAC_SEC_EUL_CONFIG = 0x2, /* EUL RE-CONFIG */
  MAC_SEC_EUL_STOP = 0x4
} mac_sec_eul_action_e_type;


/* HSUPA Action Variable */
typedef enum
{
  MAC_EUL_NOOP = 0x0,
  MAC_EUL_START = 0x1,
  MAC_EUL_CONFIG = 0x2, /* EUL RE-CONFIG */
  MAC_EUL_STOP = 0x4
} mac_eul_action_e_type;

/* HSUPA Action Variable */
typedef enum
{
  MAC_E_TSN_ACTION_BACKUP = 0x1,
  MAC_E_TSN_ACTION_REVERT = 0x2
} mac_e_tsn_action_e_type;

typedef enum
{
  MAC_E_TX_GRANT_NON_SCHEDULED,
  MAC_E_TX_GRANT_SCHEDULED,
  MAC_E_TX_GRANT_NOT_DEFINED = 0xFF
} mac_e_tx_grant_info_e_type;


/*--------------------------------------------------------------------------------------*/
/* Enum to differentiate when a fully copy of the parameters are neeeded.               */
/*--------------------------------------------------------------------------------------*/
typedef enum
{

  /* MAC cfg not changed */
  MAC_CFG_NOT_CHANGED = 0,

  /* Mac config changed */
  MAC_CFG_CHANGED,

  /* MAC config changed, only copy the loopback information
  nothing else has changed. */
  MAC_LB_CFG_CHANGED,

  MAC_LB_AND_UL_CFG_CHANGED

} mac_cfg_changed_status_e_type;

typedef enum
{
  /* MAC cfg not changed */
  MAC_DL_CONFIG_NOOP = 0,

  MAC_DL_CONFIG

}mac_dl_config_action_enum_type;

typedef enum
{
  /* MAC cfg not changed */
  MAC_MBMS_CONFIG_NOOP = 0,

  MAC_MBMS_CONFIG

}mac_mbms_config_action_enum_type;

/*========================================================================================

                       STRUCTURE TYPE DATA DEFINITIONS

========================================================================================*/

typedef struct
{
  /* Periodicity for Scheduling Info - no grant: uint16.
     It can have following values:
               0: indicates that no report(scheduling information) needs to be sent,
       otherwise: periodically send scheduling information as per its value */
  uint16                     no_grant_periodicity_of_si; /* in ms */

  /* Periodicity for Scheduling Info - grant.
     It can have following values:
               0: indicates that no report(scheduling information) needs to be sent,
       otherwise: periodically send scheduling information as per its value */
  uint16                     grant_periodicity_of_si; /* in ms */

  uint8                      power_offset_of_si; /* 0..6 (in dB) */
} mac_e_scheduling_info_config_s_type;

typedef struct
{

  /* 2ms scheduled transmission grant HARQ process allocation:
     If TTI is set to 2ms and MAC-d PDU's which are configured for SCHEDULED
     transmission (i.e. MAC-d flow for which variable 'e_transn_grant_info' is
     set to E_TRANSMISSION_GRANT_SCHEDULED) are only allowed to be transmitted
     in those processes for which the bit is set to "1".
     Bit 0 corresponds to HARQ process 0, bit 1 corresponds to HARQ process 1
     Bit 0 is the Most Significant Bit(Left most bit) of bitstring */
  uint8                           sched_tx_grant_harq_allocation_2ms; /* BITSTRING */

} mac_e_scheduling_tx_config_s_type;

typedef struct
{
  /* Max MAC-e PDU contents size: max number of non scheduled data
     in bits that can be part of MAC-e PDU */
  /* Should be same for MAC-i PDU as well.Spec doesn't mention explicitly */
  uint16                          max_mac_e_pdu_size_in_bits; /* 1..19982 */

  /* MAC-d PDU's are only allowed to be transmitted in those processes
     for which the bit is set to "1". Bit 0 corresponds to HARQ process
     0, bit 1 corresponds to HARQ process 1. This is valid for 2ms TTI only.
     Bit 0 is Most Significant Bit(Left Most bit) of the BITSTRING */
  uint8                           non_sched_tx_grant_harq_allocation_2ms; /* BITSTRING */

} mac_non_sched_tx_grant_info_s_type;

typedef struct
{
  /* Since there is no header for MAC-D pdus in EUL, so rlc_pdu_size is
     same as mac_d_pdu_size */
  uint16                          rlc_pdu_size_in_bits; /* 16..5000 (size in bits) */

  /* Data Description Identifier (6 bits), it identifies the logical channel,
     MAC-d flow and size of the MAC-d PDUs concatenated into
     the associated MAC-es PDU */
  /* This should be ignored if MAC-i PDU is configured */
  uint8                           ddi; /* 0..62 (63 is reserved) */

} mac_e_rlc_size_ddi_info_s_type;

/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of UL Dedicated logical channel mapped to E-DCH                                                             */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Radio Bearer Identifier (0-31) */
  rb_id_type                         rb_id;

  /* RLC logical channel buffer identifier (0-20) */
  rlc_lc_id_type                     rlc_id;

  /* logical channel identifier (1-15) - as received from NW */
  /* To be used while sending Scheduling Information to NW */
  uint8                             lc_id;

  /* RLC mode (UM or AM) */
  uecomdef_logch_mode_e_type         rlc_mode; /* UM or AM */

  /* MAC logical channel priority (1-8) */
  uint8                              priority;

  /* Logical channel type (DCCH or DTCH) */
  uecomdef_logchan_e_type            chan_type;

  /* if set to false then dont consider this mac_id while sending scheduling
     information*/
  boolean                            include_in_scheduling_info;

  rlc_pdu_size_type  rlc_pdu_type;         /* Flexible or Fixed PDU type */
  rlc_li_e_type      ul_li_size;           /* LI size, only for AM       */
  uint16             ul_flex_min_pdu_size; /* Minimum PDU size           */
  uint16             ul_flex_max_pdu_size; /* Minimum PDU size           */

  /* number of valid fields in rlc_size_ddi_info[] */
  uint8                              num_rlc_size_ddi_info; /* 0..32, 0 is valid only when action is NOOP or STOP */

  /* Mapping of rlc-pdu-size  to ddi. Valid number of fields in following array is
     equal to 'num_rlc_size_ddi_info' */
  mac_e_rlc_size_ddi_info_s_type     rlc_size_ddi_info[MAX_RLC_PDU_SIZE_PER_LOGCHAN];

} mac_e_ul_logch_info_s_type ;

typedef struct
{

  /* E-DCH MAC-d flow identified */
  uint8                                  e_mac_d_flow_id; /* 0..7 */

  /* E-DCH MAC-d flow power offset */
  uint8                                  e_mac_d_flow_power_offset; /* 0..6 (unit in dB) */

  /* E-DCH MAC-d flow maximum number of  retransmissions */
  uint8                                  e_mac_d_flow_max_num_of_retx; /* 0..15 */

  /* E-DCH MAC-d flow multiplexing list:
     Indicates, if this is the first MAC-d flow for which PDU's are placed in
     the MAC-e PDU, the other MAC-d flows from which MAC-d PDU's
     are allowed to be included in the same MAC-e PDU.
     Bit 0 is for MAC-d flow 0, Bit 1 is for MAC-d flow 1, …
     Value '1' for a bit means multiplexing is allowed.
     Bit 0 is Most Significant Bit(Left most bit) in the bitstring
     NOTE: The bit that corresponds to the MAC-d flow itself is ignored */
  uint8                                  e_mac_d_flow_mux_list; /* BITSTRING */

  /* transmission grant type */
  mac_e_tx_grant_info_e_type             e_tx_grant_info;

  /* below field is valid only if mac_e_tx_grant_info_e_type
     indicates NON_SCHEDULED */
  mac_non_sched_tx_grant_info_s_type    non_sched_tx_grant;

  /* number of logical channels */
  uint8                                  num_logch; /* 0..15 */

  /* number of fields valid in following array is equal to 'num_logch' */
  mac_e_ul_logch_info_s_type             e_ul_logch_info[MAX_DED_LOGCHAN_PER_TRCH];

} mac_e_mac_d_flow_info_s_type;

typedef struct
{

  /* Happy bit delay condition */
  uint16                           happy_bit_delay_in_ms; /* in ms */

} mac_e_dpcch_info_s_type;

typedef struct
{
  /* E-TFCI table index: Indicates which standardised E-TFCI TB size table
     shall be used */
  uint8                                e_tfci_table_index; /* 0..1 */

  /* E-DCH minimum set E-TFCI,
     -1 or 0xFF or MAC_E_TFCI_MIN_SET_NOT_PRESENT indicates that minimum
     set E-TFCI is not provided by NW.
     This field is valid only when DCH is not present.
     For a given 'e_tfci_minimum_set', valid E-TFCI set is 0..'e_tfci_minimum_set' */
  uint8                                e_tfci_min_set; /* 0..127 or -1(for not present) */

  /* Scheduling Information Configuration */
  mac_e_scheduling_info_config_s_type  sched_info_config;

  /* Scheduled Transmission configuration */
  mac_e_scheduling_tx_config_s_type    sched_tx_config;

} mac_e_dpdch_info_s_type;

typedef struct
{

/* Scheduled Transmission configuration */
mac_e_scheduling_tx_config_s_type    sched_tx_config;

  /* E-DCH minimum set E-TFCI,
   -1 or 0xFF or MAC_E_TFCI_MIN_SET_NOT_PRESENT indicates that minimum
   set E-TFCI is not provided by NW.
   For a given 'e_tfci_minimum_set', valid E-TFCI set is 0..'e_tfci_minimum_set' */
uint8                                e_tfci_min_set; /* 0..127 or -1(for not present) */

}mac_sec_eul_info_s_type;

typedef struct
{
  /* E-DPCCH info */
  mac_e_dpcch_info_s_type e_dpcch_info;

  /* E-DPDCH info */
  mac_e_dpdch_info_s_type e_dpdch_info;

} mac_e_dch_info_s_type;

typedef struct
{

  /* E-DCH Transmission Time Interval */
  e_tti_enum_type             e_tti;

  /* Number of E-DCH MAC-d flows in UE */
  uint8                           num_e_mac_d_flow; /* 0..8, 0 will be set ONLY when action is NOOP */

  /* E-DCH MAC-d flow */
  mac_e_mac_d_flow_info_s_type    e_mac_d_flow_info[MAX_E_DCH_MAC_D_FLOW];

  /* E-DCH Info */
  mac_e_dch_info_s_type           e_dch_info;

} mac_e_config_s_type;




/*25.331 10.3.5.3b           Common E-DCH MAC-d flows 
NOTE 1:   For FDD, E-DCH MAC-d flow identity 7 is reserved for CCCH transmission.
*/
#define HS_RACH_CCCH_D_FLOW_ID 7

/*25.321 9.2.4.4 MAC-i header parameters
LCH-ID Field      Designation
1110              Identification of CCCH (SRB0)
*/
/* log_ch id populated from RRC is +1 to the NW expecting,so MAC taken care before filling MAC_I_IS headers by decrementing -1,
 *For CCCH NW expects 14- if we set it to 15 , MAC fills as 14 while forming MAC-I-IS 
 * FOr DCCH/DTCH collision resolution lc_id is 1111
 */
#define HS_RACH_CCCH_LCH_ID 15


#define HS_RACH_COLLISION_RESOLUTION_LCID 15
#define HS_RACH_SPACE_BITS 0x0

/* HS_RACH Action Variable */
typedef enum
{
  MAC_HS_RACH_NOOP = 0x0,
  MAC_HS_RACH_START = 0x1,
  MAC_HS_RACH_CONFIG = 0x2, /* HS-RACH RE-CONFIG */
  MAC_HS_RACH_STOP = 0x4
} mac_hs_rach_action_e_type;

typedef enum
{
  MAC_HS_RACH_TX_BACKOFF_0 =  0,
  MAC_HS_RACH_TX_BACKOFF_4 =  4,  
  MAC_HS_RACH_TX_BACKOFF_8 =  8,
  MAC_HS_RACH_TX_BACKOFF_16 = 16,
  MAC_HS_RACH_TX_BACKOFF_24 = 24,
  MAC_HS_RACH_TX_BACKOFF_40 = 40,
  MAC_HS_RACH_TX_BACKOFF_80 = 80,
  MAC_HS_RACH_TX_BACKOFF_INFINITY = 0xFE,
  MAC_HS_RACH_TX_BACKOFF_INVALID = 0xFF
} l1_e_dch_tx_cont_backoff_enum_type;


/*--------------------------------------------------------------------------------------*/
/* Access Service Class Parameters                                                      */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Persistence value of the partition */
  uint32              pval;

}mac_ul_asc_type;

/*--------------------------------------------------------------------------------------*/
/* RACH Transmit Control Parameters                                                     */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Maximum number of RACH preamble ramping cycles */
  uint8       mmax;

  /* Lower bound for back-off time specified as number of 10 msec */
  /* intervals */
  uint8       nbo1min;

  /* Upper bound for back-off time specified as number of 10 msec */
  /* intervals */
  uint8       nbo1max;

  /* Number of available access service classes */
  uint8       nASC;

  /* Parameters of each valid Access Service Class */
  mac_ul_asc_type asc[MAX_ASC];

}mac_ul_rach_txcntl_config_type;



typedef struct
{
  mac_e_config_s_type hs_rach_config;

  boolean primary_e_rnti_present;
  uint16 primary_e_rnti;

  /*please refer to 10.3.6.54a		PRACH preamble control parameters (for Enhanced Uplink) in 25.331*/
  mac_ul_rach_txcntl_config_type mac_ul_rach_txcntl;

  /*please refer to 10.3.6.54a		PRACH preamble control parameters (for Enhanced Uplink) in 25.331*/
  /* Access Service Class index for CCCH */
  uint8      ccch_asc;

  /*Maximum E-DCH resource allocation for CCCH, values 8, 12, 16, 24, 32, 40, 80, 120 In terms of TTIs*/
  uint8  max_e_dch_res_alloc_ccch;

  /*E-DCH transmission continuation back off, value Enumerated (0, 8, 16, 24, 40, 80, 120, infinity)
  *In terms of TTIs. If set to "infinity", implicit common E-DCH resource release is disabled.
  */
  l1_e_dch_tx_cont_backoff_enum_type   l1_e_dch_tx_cont_backoff;

  /*additional-E-DCH-TransmitBackoff   INTEGER   (0 .  . 15 )*/
uint8     add_e_dch_tx_backoff;

  /*Maximum period for collision resolution phase, Integer (8..24) In terms of TTIs.*/
  uint8  max_collision_resol;
} mac_hs_rach_config_s_type;


/*--------------------------------------------------------------------------------------*/
/* HSDPA HS PRIORITY QUEUE Information                                                         */
/*--------------------------------------------------------------------------------------*/


typedef struct
{
  /* PDU Size in MAC HS Header */
  uint16 mac_pdu_size;  /* 1..5000 */

  /* PDU size index in MAC HS header */
  uint8 mac_pdu_index; /* 0..7 */

} mac_hs_pdu_info_struct_type;

typedef struct
{
  /* Queue Id */
  uint8 queue_id;

  /* No of PDU sizes */
  uint8 no_of_pdu_sizes;

  /* MAC PDU size type information for this queue */
  mac_hs_pdu_info_struct_type pdu_info[UE_MAX_HS_NO_OF_PDU_SIZE_PER_QUEUE];

  /* Reordering release timer in msecs */
  uint16 release_timer;

  /* Receiver Window Size */
  uint8 window_size;

} mac_hs_queue_struct_type;



/*--------------------------------------------------------------------------------------*/
/* MAC Status indication                                                                                                                                */
/*--------------------------------------------------------------------------------------*/
typedef enum
{

  /* Successful transmission on RACH */
  RACH_TX_SUCCESS,

  /* Max number of RACH cycles exccedded */
  MAX_RACH_CYCLES_REACHED,

  /* Transmission on RACH is aborted */
  RACH_TX_ABORTED

} mac_status_ind_e_type;

/*--------------------------------------------------------------------------------------*/
/* RNTI information                                                                                                                                             */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* RNTI validity flag */
  mac_rnti_valid_e_type   rnti_valid;

  /* 32-bit U-RNTI value assigned by U-TRAN */
  /* Valid only if "rnti_valid = URNTI_VALID or BOTH_VALID" */
  uint32      urnti;

  /* 16-bit C-RNTI value assigned by serving cell */
  /* Valid only if "rnti_valid = CRNTI_VALID or BOTH_VALID" */
  uint16      crnti;

} mac_rnti_config_type;

/*--------------------------------------------------------------------------------------*/
/* Ciphering parameters                                                                 */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  mac_cipher_cfg_e_type cipher_cfg_type;

  /* The ciphering algorithm can be either UE_0 or UE_1 */
  uecomdef_wcdma_cipher_algo_e_type cipher_algo;

  /* Cipher Enable flag */
  /* TRUE if ciphering is enabled */
  /* FALSE otherwise */
  boolean     cipher_enable;

  /* Ciphering key index */
  uint8       key_idx;

  /* 20-bit Hyper Frame Number */
  /* HFN is shifted right by 4 bits to make it a 24 bit integer */
  /* and then appended with 8-bit CFN to make it 32-bit COUNT-C */
  uint32      hfn;

  /*key_id and cipher_enable together will indicate whether Ciphering
  configuration has changed and if new hfn and ciph_act_time is to be used. */
  uint32      ciph_act_time;


} mac_cipher_config_type;

/*--------------------------------------------------------------------------------------*/
/* The structure that needs to be filled up when RRC requests MAC for the START values  */
/* MAC sends HFN per domain                                                             */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* hfn : 20-bit Hyper Frame Number (HFN) */
  uint32    cs_hfn;
  uint32    ps_hfn;

} mac_hfn_info_type;




/*--------------------------------------------------------------------------------------*/
/* Configuration parameters indicating the list of rlc sizes that are valid for a       */
/* particular logical channel                                                           */
/*--------------------------------------------------------------------------------------*/
typedef struct
{
  /*TRUE - Indicates all the rlc_sizes are applicable to this logical channel.
  FALSE implies lc_tfi_list list will provide the list of tfi's which are
  applicable to this channel. Note TFI's inturn give the rlc size that is associated */
  boolean                     all_tfi_valid;

  /* Number of Transport Format indicators that are valid for this logical channel.
  Applicable only when it is an explicit list. */
  uint8                       ntfi_per_lc;

  /* Lists the Transport format indicators that are valid for this logical channel.
  Applicable only when it is an explicit list. */
  uint16                      lc_tfi_list[UE_MAX_TF];

}mac_ul_rlc_size_info_type;

/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of UL Dedicated logical channel                                                             */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Radio Bearer Identifier (0-31) */
  rb_id_type                          rb_id;

  /* RLC logical channel buffer identifier (0-20) */
  rlc_lc_id_type                  rlc_id;

  /* MAC logical channel identifier (1-15) */
  /* Distinguishes dedicated logical channels when more than one */
  /* is mapped onto same transport channel */
  uint8                           mac_id;

  /* MAC logical channel priority (1-8) */
  uint8                           priority;

  /* Logical channel type (DCCH or DTCH) */
  uecomdef_logchan_e_type         chan_type;

  /* RLC mode (TM or UM or AM) */
  uecomdef_logch_mode_e_type      rlc_mode;

  /* Lists the rlc sizes that are valid for this logical channel */
  mac_ul_rlc_size_info_type       rlc_size_info;


}mac_ul_ded_logch_config_type;

/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of DL Dedicated logical channel                             */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Radio Bearer Identifier (0-31) */
  rb_id_type          rb_id;

  /* RLC logical channel buffer identifier (0-20) */
  rlc_lc_id_type      rlc_id;

  /* MAC logical channel identifier (1-15) */
  /* Distinguishes dedicated logical channels when more than one */
  /* is mapped onto same transport channel. */
  /* In case of MTCH logical channel this field will indicate the MBMS id */
  uint8               mac_id;
  
  /* RRC sets this value for CS over HS channels */
  boolean opt_timer_enabled;

  /* Logical channel type (DCCH or DTCH) or MSCH or MTCH or MCCH*/
  uecomdef_logchan_e_type         chan_type;

  /* RLC mode (TM or UM or AM) */
  uecomdef_logch_mode_e_type      rlc_mode;

  /*This field is applicable only for MCCH, MSCH and MTCH. It indicates
  if the TCTF is present or not Boolean TRUE indicates that TCTF field
  is present. FALSE otherwise*/
  boolean                        tctf_present;

}mac_dl_ded_logch_config_type;

/*--------------------------------------------------------------------------------------*/
/* HSDPA MAC D FLOW Information                                                         */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* CCTrCh ID */
  cctrch_id_type      cctrch_id;

  /* MAC-d Flow Id */
  uint8 mac_dflow_id;

  /* No of priority queues */
  uint8 no_of_queues;

  /* Queue Information */
  mac_hs_queue_struct_type queue_info[UE_MAX_HS_QUEUE];

  /* Number of dedicated RLC logical channels mapped */
  uint8       ndlchan;

  /* Info of dedicated logical channels mapped */
  mac_dl_ded_logch_config_type dlchan_info[UE_MAX_LOGCHAN_PER_DFLOW];

} mac_dflow_struct_type;

/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of uplink dedicated transport channel                       */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Transport channel identifier */
  tr_ch_id_type       trch_id;

  /* CCTrCh ID */
  cctrch_id_type      cctrch_id;

  /* Number of dedicated RLC logical channels mapped */
  uint8               ndlchan;

  /* Info of dedicated logical channels mapped */
  mac_ul_ded_logch_config_type    dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];


  /* 0(False) indicates it is CS domain, 1(TRUE) indicates it is PS domain */
  mac_ul_cipher_domain_e_type     cs_or_ps_domain;

}mac_ul_dch_trch_config_type;

/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of FACH-type transport channel                              */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Transport channel identifier */
  tr_ch_id_type             trch_id;

  /* CCTrCh ID */
  cctrch_id_type            cctrch_id;

  /* BCCH enable flag */
  /* TRUE if BCCH is mapped to this transport channel */
  /* FALSE otherwise */
  boolean                           bcch_enable;

  /* Unique identifier of BCCH buffer */
  /* Valid only when bcch_enable = TRUE */
  rlc_lc_id_type                bcch_id;

  /* CCCH enable flag */
  /* TRUE if downlink CCCH is mapped to this transport channel */
  /* FALSE otherwise */
  boolean                           ccch_enable;

  /* Unique identifier of uplink CCCH buffer */
  /* Valid only when ccch_enable = TRUE */
  rlc_lc_id_type                ccch_id;

  /* CTCH enable flag */
  /* TRUE if CTCH is mapped to this transport channel */
  /* FALSE otherwise */
  boolean                           ctch_enable;

  /* Unique identifier of CTCH buffer */
  /* Valid only when ctch_enable = TRUE */
  rlc_lc_id_type                ctch_id;

  /* Number of dedicated logical channels mapped to this transport */
  /* channel */
  uint8                             ndlchan;

  /* Configuration of each dedicated logical channel mapped to */
  /* this transport channel */
  mac_dl_ded_logch_config_type  dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];

  /* Indicates if MCCH is mapped onto this FACH transport channel*/
  /* TRUE indicates that MCCH is mapped to FACH */
  /* FALSE otherwise */
  boolean                                           mcch_enable;

  /* Gives MCCH specific information*/
  mac_dl_ded_logch_config_type          mcch_info;

  /* Indicates if MSCH is mapped onto this FACH transport channel */
  /* TRUE indicates that MSCH is mapped to FACH, FALSE indicates otherwise */
  boolean                                       msch_enable;

  /* Gives MSCH specific information*/
  mac_dl_ded_logch_config_type          msch_info;

  /* Number of MTCH channels mapped to FACH.IF num_mtch_channel = 0
     indicates that NO MTCH channel is mapped onto FACH*/
  uint16                                            num_mtch_channel;

  /* Gives MTCH specific information*/
  mac_dl_ded_logch_config_type          mtch_info[MAX_MTCH_PER_TRCH];

}mac_dl_fach_trch_config_type;

/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of downlink dedicated transport channel                     */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Transport channel identifier */
  tr_ch_id_type       trch_id;

  /* CCTrCh ID */
  cctrch_id_type      cctrch_id;

  /* Number of dedicated RLC logical channels mapped */
  uint8       ndlchan;

  /* Info of dedicated logical channels mapped */
  mac_dl_ded_logch_config_type dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];

  /* 0(False) indicates it is CS domain, 1(TRUE) indicates it is PS domain */
  mac_ul_cipher_domain_e_type   cs_or_ps_domain;

  mac_loop_back_mode_e_type loopback_test_mode;

}mac_dl_dch_trch_config_type;
/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of DSCH type transport channel                              */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Transport channel identifier */
  tr_ch_id_type       trch_id;

  /* CCTrCh ID */
  cctrch_id_type      cctrch_id;

  /* Number of dedicated RLC logical channels mapped */
  uint8       ndlchan;

  /* Info of dedicated logical channels mapped */
  mac_dl_ded_logch_config_type        dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];

}mac_dl_dsch_trch_config_type;


/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of uplink dedicated CCTrCh                                  */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Number of dedicated transport channels */
  uint8       ndchs;

  /* Configuration info of each dedicated transport channel */
  mac_ul_dch_trch_config_type dch_info[UE_MAX_TRCH];
  mac_loop_back_mode_e_type loopback_test_mode;

}mac_ul_dch_cctrch_config_type;

/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of RACH-type CCTrCh                                         */
/*--------------------------------------------------------------------------------------*/

typedef struct
{

  /* Transport channel ID */
  tr_ch_id_type     trch_id;

  /* Flag indicating mapping of uplink CCCH */
  /* TRUE if CCCH is mapped to RACH         */
  /* FALSE otherwise */
  boolean           ccch_enable;

  /* Unique identifier of the uplink CCCH buffer */
  /* Valid only when ccch_enable = TRUE          */
  rlc_lc_id_type    ccch_id;

  /* Access Service Class index for CCCH */
  /* Valid only when ccch_enable = TRUE  */
  uint8             ccch_asc;

  /* Number of dedicated RLC logical channels mapped to CCTrCh */
  uint8             ndlchan;

  mac_loop_back_mode_e_type loopback_test_mode;

  /* Info of each dedicated logical channel mapped */
  mac_ul_ded_logch_config_type    dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];

  boolean atf_present;      /* Indicates if IE "Additional Dynamic Transport Format
                               Information for CCCH" is present in SIB5/6 */
  uint16 atf_ccch_tf_index; /* This index is with respect to tf_info[UE_MAX_TF]
                               present in mac_ul_tf_type in OC  */

}mac_ul_rach_cctrch_config_type;

/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of BCH-type transport channel                               */
/*--------------------------------------------------------------------------------------*/

typedef struct
{

  /* Valid only when "enable = TRUE" */
  cctrch_id_type      cctrch_id;

  /* Unique identifier of BCCH buffer */
  /* Valid only when "enable = TRUE" */
  rlc_lc_id_type      bcch_id;

}mac_dl_bch_cctrch_config_type;

/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of PCH-FACH type CCTrCh                                     */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* CCTrCh ID */
  cctrch_id_type      cctrch_id;

  /* Unique identifier of PCCH buffer */
  /* Valid only when "pcch_enable = TRUE" */
  rlc_lc_id_type      pcch_id;

}mac_dl_pch_cctrch_config_type;


/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of uplink CCTrCh                                            */
/*--------------------------------------------------------------------------------------*/

typedef struct
{

  /* CCTrCh type (RACH or DCH) */
  mac_ul_cctrch_e_type        cctrch_type;

  /* Random Access CCTrCh configuration */
  mac_ul_rach_cctrch_config_type      rach_info;

  /* Uplink Dedicated CCTrCh configuration */
  mac_ul_dch_cctrch_config_type       dch_info;

}mac_ul_cctrch_config_type;


/*------------------------------------------------------------------*/
/*    MAC Uplink PDU Header type                                    */
/*------------------------------------------------------------------*/
typedef enum {

   MAC_E_PDU_HDR = 0,
   MAC_I_PDU_HDR,
   INVALID_MAC_PDU_HDR
} mac_ul_pdu_hdr_e_type;


/*------------------------------------------------------------------*/
/*    RLC Uplink Action                                     */
/*------------------------------------------------------------------*/
typedef enum {

   CRLC_INVALID = 0,
   CRLC_SUSPEND,
   CRLC_RESUME
} mac_ul_rlc_action_type;


/*--------------------------------------------------------------------------------------*/
/* Uplink MAC Configuration Parameters                                                  */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* TRUE indicates that there is an activation time involved with this configuration
  and MAC needs to wait for L1 to indicates us to switch over to the new config. FALSE
  indicates switch to the new configuration that RRC sends us immediately */
  boolean                             wait_for_l1_sig;

  /*
  MAC_CFG_CHANGED indicates that MAC config parameters have changed and we need to configure
  MAC either with activation time (if wait_for_l1_sig is set to TRUE) or immediately.
  This boolean was temporarily created to address the issue when MAC gets configured
  without any parameters getting changed in the MAC layer. This happens because RRC
  sends a ASU complete message, L1 DL params have alone changed. To manage the pointer
  within RRC software, it sends L1 and MAC UL the configuration. If mac_config_changed
  is set to FALSE then we just assign the new TFCS pointer and do not go about actual
  reconfiguration process (i.e verifying the parameters suplied by RRC).
  MAC_TFCS_CFG_NOT_CHANGED indicates that it was loopback mode the confguration was received.
  Copy only the loopback mode information
  */
  mac_cfg_changed_status_e_type       mac_config_changed;

  /* Pointer to UL CCTrCh configuration data */
  /* Uplink CCTrCh mapping information resides in shared memory */
  /* It can be accessed by RRC and MAC */
  mac_ul_cctrch_config_type           *cctrch_ptr;

  /* Pointer to UL Transport Format Combination Set information */
  /* Uplink TFCS info resides in shared memory */
  /* It can be accessed by RRC and MAC */
  mac_ul_tfcs_type                    *tfcs_ptr;

  /* RNTI information */
  /* MAC copies this info to local memory */
  mac_rnti_config_type                rnti_info;

  /* set to MAC_EUL_REVERT_TO_OLD_TSN whenever UE has to reverting back to old
     config. MAC-E should process this information first(before mac-e-reset
     indicator).
     If it is set to MAC_EUL_REVERT_TO_OLD_TSN then MAC-E is expected to
     revert to the old TSN information which was stored on receipt of last
     CMAC_UL_CONFIG_REQ.
     If it is set to MAC_EUL_BACKUP_TSN then back up of
     current config(like TSN) is needed. */
 /* This legacy variable will be reused to set tsn action while reverting MAC conffig due to DC-HSUPA config failure*/
  mac_e_tsn_action_e_type   mac_e_tsn_action;

  /* When this variable is set to TRUE, then all the HARQ processes
  will be flushed in Firmware. MAC will use this variable to flush all
  HARQ process related information that is maintained internally.
  Thsi information needs to be flushed so that happy bit is computed
  correctly in MAC*/

 /* This legacy varibale is used to indicate mac_i/is reset during secondary EUL Activation/deactivation. 
    Check whether is it correct to flush both primary and secondary eul HARQ processes while starting / stopping secodary
    eul alone? */
  boolean                             mac_e_reset_indicator;


  mac_sec_eul_action_e_type           mac_sec_eul_action;

/* When secondary mac eul is configured tsn filed exten should be activated at MAC 
   When secondary mac eul is deconfigured mac should stop using tsn field extn */
  mac_sec_eul_info_s_type             *mac_sec_eul_config_ptr;


  /* MAC-e/es or MAC-i/is PDU */
  mac_ul_pdu_hdr_e_type               ul_pdu_type;

  mac_ul_rlc_action_type               ul_rlc_action;
  
  mac_eul_action_e_type               mac_eul_action;

  /* MAC E-DCH config info - MAC should look into this pointer only if
     eul_action is MAC_EUL_CONFIG */
  mac_e_config_s_type                 *mac_e_config_ptr;

  mac_hs_rach_action_e_type hs_rach_action;

  /*hs_rach_to_eul_trans will be set to TRUE by RRC at hs_rach to eul transistion*/
  boolean hs_rach_to_eul_trans;

  mac_hs_rach_config_s_type *hs_rach_config_ptr;
	/* When this variable is set to TRUE, then all the HARQ processes
  will be flushed in Firmware. MAC will use this variable to flush all
  HARQ process related information that is maintained internally.
  Thsi information needs to be flushed so that happy bit is computed
  correctly in MAC*/
  boolean                             mac_hsrach_e_reset_indicator;

  /* Ciphering control information */
  /* MAC copies this info to local memory Domain index CS(0), PS(1)*/
  mac_cipher_config_type              cipher_info[MAX_CN_DOMAIN];

  /* RACH Transmit Control Parameters */
  /* MAC copies this info to local memory */
  mac_ul_rach_txcntl_config_type      rach_tx_cntl_info;

/*If this flag indicates that new TFCS\ TFCSubset is not present:
  MAC will continue with the old configuration. MAC will ignore
  TFCS table reconfig but apply other LC reconfig Else
  MAC will apply the new configuration present in OC.*/
  boolean   new_tfcs_list_present;

}mac_ul_config_type;

/************* MAC RRC Changes ***********************************************/

typedef enum 
{
  MAC_HS_INVALID_TRANSITION,
  MAC_HS_TO_EHS,
  MAC_EHS_TO_HS,
  MAC_HS_NO_TRANSITION
}mac_hs_transition_e_type;

/* HSDPA Action Variable */
typedef enum
{
  ENH_HSDPA_NOOP,
  ENH_HSDPA_START,
  ENH_HSDPA_RECONFIG,
  ENH_HSDPA_STOP
}enh_hsdpa_action_enum_type;

typedef struct
{
  /* CCTrCh ID */
  cctrch_id_type      cctrch_id;

  /* Queue Id */
  uint8 queue_id;

  /* Reordering release timer in msecs */
  uint16 release_timer;

  /* Receiver Window Size */
  uint8 window_size;

  
 /* Reordering reset timer in msecs, Timer in multiples of release_timer values (milliseconds). 
  *Used when MAC-ehs reordering queue is reset in CELL_FACH and CELL_PCH (FDD only). 
  *Default value is 2 */
  
  uint8 reset_timer;
} mac_ehs_queue_struct_type;

/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of DL Dedicated logical channel                             */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /*MAC EHS Q id*/
  uint32 queue_id;
  
  /* Radio Bearer Identifier (0-31) */
  rb_id_type          rb_id;

  /* RLC logical channel buffer identifier (0-20) */
  rlc_lc_id_type      rlc_id;

  /*********************TBD **********************/
  /* MAC logical channel identifier (1-15) */
  /* Distinguishes dedicated logical channels when more than one */
  /* is mapped onto same transport channel */
  uint8               mac_id;
  /*********************TBD **********************/

  /* RRC sets this value for CS over HS channels */
  boolean opt_timer_enabled;
  
  /* Logical channel type (DCCH or DTCH) */
  uecomdef_logchan_e_type         chan_type;

  /* RLC mode (TM or UM or AM) */
  uecomdef_logch_mode_e_type      rlc_mode;

}mac_dl_ehs_logch_config_type;

/*--------------------------------------------------------------------------------------*/
/* Downlink MAC Configuration Parameters                                                */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /*This enum type indicates if the R99 config has changed or not*/
  mac_dl_config_action_enum_type        mac_dl_action;

  /* TRUE indicates that there is an activation time involved with this configuration
  and MAC needs to wait for L1 to indicates us to switch over to the new config. FALSE
  indicates switch to the new configuration that RRC sends us immediately */
  boolean                                   wait_for_l1_sig;

  /* Number of BCH type CCTrChs : Valid range is 0 - MAC_DL_MAX
  _CCTRCH */
  uint8                             num_bchs;

  /* Broadcast CCTrCh configuration */
  mac_dl_bch_cctrch_config_type       bch_info[MAC_DL_MAX_CCTRCH];

  /* Number of PCH/FACH type CCTrChs : Valid range is 0 - MAC_DL_MAX_CCTRCH */
  uint8   num_pchs;

  /* PCH CCTrCh information */
  mac_dl_pch_cctrch_config_type   pch_info[MAC_DL_MAX_CCTRCH];

  /* Number of FACH type TrChs : Valid range is 0 - UE_MAX_TRCH */
  uint8   num_fachs;

  /* FACH TrCh configuration */
  mac_dl_fach_trch_config_type    *fach_info[UE_MAX_TRCH];

  /* Number of DCH type TrChs : Valid range is 0 - UE_MAX_TRCH */
  uint8   num_dchs;

  /* Downlink Dedicated CCTrCh configuration */
  mac_dl_dch_trch_config_type     *dch_info[UE_MAX_TRCH];

  hsdpa_action_enum_type mac_hsdpa_action;

  boolean mac_hs_reset_indicator;

  boolean mac_hs_restore_tsns;

  /*Informs MAC if MAC HS is configured or MAC EHS */
  mac_hs_e_type mac_hs_or_ehs;

  boolean reset_timer_valid;

  /* MAC EHS Action */
  enh_hsdpa_action_enum_type mac_enh_hsdpa_action;

  /*Informs transition between HS and EHS, if present */
  mac_hs_transition_e_type mac_hs_transition_type;

 /* Number of dedicated RLC logical channels mapped */
  uint8       ndlchan_macehs;

  /* Info of dedicated logical channels mapped */
  mac_dl_ehs_logch_config_type * dl_macehs_logchan_info[UE_MAX_DL_LOGICAL_CHANNEL];
  
  uint8 num_mac_ehs_queue;
  
  mac_ehs_queue_struct_type *hsdpa_ehs_queue_info[UE_MAX_HS_QUEUE];

  uint8    efach_ndlchans;

  mac_dl_ehs_logch_config_type    *efach_dl_logchan_info[UE_MAX_DL_LOGICAL_CHANNEL];

  uint8    efach_num_queue;

  mac_ehs_queue_struct_type    *efach_queue_info[UE_MAX_HS_QUEUE];

  boolean    hs_status_in_e_fach;
  
  /* Number of MAC-d Flows. */
  uint8 num_dflow;

  /* HSDPA MAC-d Flow Structure */
  mac_dflow_struct_type *hsdpa_info[UE_MAX_MAC_D_FLOW];

  /* Number of DSCH type TrChs : Valid range is 0 - UE_MAX_TRCH */
  uint8   num_dschs;

  /* Downlink Shared CCTrCh information */
  mac_dl_dsch_trch_config_type        *dsch_info[UE_MAX_TRCH];

  /* RNTI information */
  /* MAC copies this info to local memory */
  mac_rnti_config_type                rnti_info;

  /* Ciphering control info */
  /* MAC copies this info to local memory */
  mac_cipher_config_type              cipher_info[MAX_CN_DOMAIN];

  /* Indicates if the MBMS config is to be applied this time.*/
  mac_mbms_config_action_enum_type             mbms_action;

  /*This indicates the number of FACH's that have MTCH, MCCH or '
  'MSCH mapped onto it. The FACH trch Id could be the same as that
  of some existing R99 FACH's. In that case MAC would take care of
  suitably updating*/
  uint16                                num_mbms_fachs;

  /*Downlink MBMS channel configuration information*/
  mac_dl_fach_trch_config_type          *mbms_info[UE_MAX_TRCH];

  /* If MBMS configuration has an activation time then
  this flag will be set to TRUE and MAC will have to wait for L1 signal*/
  boolean                               mbms_wait_for_l1_sig;

#ifdef FEATURE_WCDMA_3C_HSDPA
  boolean tsn_extn; 
#endif

}mac_dl_config_type;

/*--------------------------------------------------------------------------------------*/
/* Transport channel id and type information                                            */
/*--------------------------------------------------------------------------------------*/
typedef struct
{
  /* Transport channel type */
  mac_ul_cctrch_e_type trch_type;

  /* Transport channel id : Range (1...32) */
  tr_ch_id_type       trch_id;

}mac_ul_traffic_trch_info_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement object type                                               */
/*--------------------------------------------------------------------------------------*/
typedef struct
{
  /* Number of transport channels */
  uint16 num_trch;

  /* Transport channel id and type information for each of these transport channels */
  mac_ul_traffic_trch_info_type  trch_info[UE_MAX_TRCH];

}mac_ul_traffic_meas_obj_type;

/*--------------------------------------------------------------------------------------*/
/* MAC could be asked to measure the raw, average, or variance of rlc payload. In the   */
/* case of average and variance the time interval to take the average & variance is     */
/* given                                                                                */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* One of raw, average or variance */
  mac_ul_traffic_meas_qty_e_type  meas_qty_enum;

  /* Time interval to take the average or variance */
  uint32 meas_interval;

}mac_ul_traffic_meas_qty_type;

/*--------------------------------------------------------------------------------------*/
/* MAC may be asked to report either the raw, average or the variance of the rlc payload*/
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* If set to true include raw rlc payload measurement information in the report*/
  boolean   rlc_payload;

  /* If set to true include average payload measurement information in the report*/
  boolean   avg_payload;

  /* If set to true include variance payload measurement information in the report*/
  boolean   var_payload;

}mac_ul_traffic_meas_report_qty_type;


/*--------------------------------------------------------------------------------------*/
/* Parameters in the case of event trigger specific to a transport channel              */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* the event could be either event_4a or event_4b */
  mac_ul_traffic_event_id_e_type  event_id;

  /* threshold in bytes (8,16,32,64,...) */
  uint32    report_threshold;

  /* Indicates the period of time in ms during which the event has to be satisfied for*/
  /* MAC to trigger a report */
  uint16    time_to_trigger;

  /* Indicates the period of time during which it is forbidden to send any new report */
  /* with the same traffic volume event identity even if the triggering condition is  */
  /* fulfilled */
  uint16    trigger_prohibit_tmr;

  /* Indicates how long the UE shall block DTCH transmissions on the RACH after a   */
  /* measurement report is triggered */
  uint16    tx_interrupt_tmr;

}mac_ul_traffic_trch_event_trigger_param;

/*--------------------------------------------------------------------------------------*/
/* The event trigger parameters for each transport channel                              */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Number of events */
  uint16 num_events;

  /* One for event 4A and other for event 4B */
  mac_ul_traffic_trch_event_trigger_param event_param[2];

  /* The channel type and id of each transport channel */
  mac_ul_traffic_trch_info_type  trch_info;

}mac_ul_traffic_trch_event_trigger_cfg;

/*--------------------------------------------------------------------------------------*/
/* Event trigger related config parameters for the transport channels                   */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Number of transport channels */
  uint16   num_trch;

  /* True implies - the parameters are specific for the transport channels
  specified. False imples apply the event params to all uplink transport
  channels */
  boolean   trch_info_incl;

  /* Event trigger related parameters for each transport channel */
  mac_ul_traffic_trch_event_trigger_cfg   trch_event_cfg[UE_MAX_TRCH];

}mac_ul_traffic_event_trigger_cfg_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume periodic trigger related parameters                                   */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* True indicates there is no limit on the number of reports that we send to RRC    */
  /* False indicates,send only the number of reports that are specified in num_reports*/
  boolean   report_infinity;

  /* The number of reports that can be sent */
  uint16    num_reports;

  /* Indicates in milliseconds the interval of periodic report */
  uint16    report_interval;

}mac_ul_traffic_periodic_report_cfg_type;


/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement report criteria type                                      */
/*--------------------------------------------------------------------------------------*/

typedef union
{
  /* Contains the event trigger related config information for all transport channels */
  /* and for event_4a, event_4b */
  mac_ul_traffic_event_trigger_cfg_type   event_cfg;

  /* Contains all the config information in the case of periodic trigger */
  mac_ul_traffic_periodic_report_cfg_type periodic_cfg;

}mac_ul_traffic_meas_report_criteria_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume report mode type                                                      */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Whether UM or AM is used for transfer */
  mac_ul_traffic_report_transfer_mode_e_type  transfer_mode;

  /* Indicates if it was periodical reporting or event trigger */
  mac_ul_traffic_report_mode_e_type report_mode;

}mac_ul_traffic_report_mode_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic measurement config information sent by RRC                                   */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Measurment identity for this traffic volume report.: Valid range 1..16 */
  uint16       meas_id;

  /* True Indicates that RRC is expecting a confirmation message from MAC    */
  /* False indicates that MAC does not need to send the confirmation message */
  boolean     config_conf_ind;

  /* TRUE indicates Start making measurements as soon as the setup cmd is recvd
  FALSE : save the parameters and wait for a resume cmd to start any measurements
  for this ID . Valid for START cmd only */
  boolean     meas_start_incl;

  /* This number is used when MAC posts the config confirmation message to RRC */
  uint16       config_ack_num;

  /* RRC could ask MAC to start, modify or delete a measurement */
  mac_ul_traffic_cmd_e_type   meas_cmd;

  /* True Indicates measurement object information is included  */
  /* False indicates apply the measurement reporting criteria to all uplink */
  /* transport channels */
  boolean     meas_obj_incl;

  /* The objects for which measurement is requested */
  mac_ul_traffic_meas_obj_type  meas_obj_list;

  /* True indicates measurement quantity information is included */
  boolean     meas_qty_incl;

  /* The quantity that MAC shall measure */
  mac_ul_traffic_meas_qty_type  meas_qty;

  /* True indicates if the measurement reporting information is included or not */
  boolean   meas_report_incl;

  /* Quantities that MAC shall report to RRC  */
  mac_ul_traffic_meas_report_qty_type   meas_report_qty;

  /* Contains the measurement transfer mode and indication of periodic, */
  /*  event trigger, no trigger  */
  mac_ul_traffic_report_mode_type       meas_report_mode;

  /* In the case of MODIFY, True will indicate if the periodic or the event triggered criteria*/
  /* is being modified */
  boolean                               meas_criteria_incl;

  /* Contains the event trigger, periodic trigger related additional parameters */
  mac_ul_traffic_meas_report_criteria_type  meas_report_criteria;

}mac_ul_traffic_meas_config_type;

/*--------------------------------------------------------------------------------------*/
/* RRC to MAC config command for TFCC info. CMAC_UL_TFCC_SUBSET_CFG_REQ                 */
/*--------------------------------------------------------------------------------------*/
typedef struct
{
  tr_ch_id_type           trch_id;

  mac_ul_cctrch_e_type    trch_type;

  /* Number of transport formats */
  uint16                          ntf;

  /* Each known Transport Format Combination */
  uint8                       tfi[UE_MAX_TF];

}mac_tfc_subset_restriction_trch_info_type;

typedef struct
{
  uint8                                         ntrch;

  mac_tfc_subset_restriction_trch_info_type     trch_restriction_info[UE_MAX_TRCH];

}mac_tfc_subset_restriction_trch_list_type;

typedef struct
{
  /* Number of TFC's that are allowed/not_allowed based on the choice */
  uint16                              ntfc;

  /* List of TFCI's */
  uint16                              tfci_list[UE_MAX_TFC];

}mac_tfc_subset_restriction_tfci_info_type;

typedef union
{
  uint16                                tfci;

  mac_tfc_subset_restriction_tfci_info_type   tfci_restriction_info;

  mac_tfc_subset_restriction_trch_list_type   trch_restriction_info_list;

}mac_tfc_subset_restriction_u_type;

typedef struct
{
  /* Transaction id of the message that RRC recvd for the TFCC message */
  uint8                               transaction_id;

  rb_id_type                          rb_id;

  /* Choice of the restriction */
  mac_tfc_subset_restriction_choice_e_type  tfci_restriction_choice;

  mac_tfc_subset_restriction_u_type         restriction_info;

  /* Activation time CFN at which this config has to be applied */
  uint16                              activation_time;

  /* Time for which this config is valid */
  uint16                              control_duration;

}mac_tfc_subset_restriction_info_type;


/* Downlink layer 2 command ID types */
typedef enum
{
  RLCI_DL_TX_AM_STATUS = 100,     /* Copy nak_list_q (DL) to tx_nak_q (UL) */

  CMAC_DL_CONFIG_REQ,       /* Configure DL MAC */

  CRLC_DL_TM_CONFIG_REQ,    /* Configure DL TM RLC */

  CRLC_DL_UM_CONFIG_REQ,    /* Configure DL UM RLC */

  RLCI_DL_CONFIG_REQ,       /* Configure DL AM RLC */

  RLCI_DL_RESET_REQ,        /* RESET request for DL AM RLC  */

  RLCI_UL_RESET_DONE,       /* Confirm the RESET completion of UL AM RLC  */

  RLCI_DL_RELEASE_ALL_REQ,  /* Release All non-MBMS request to the Downlink */

  CRLC_DL_CONTINUE_REQ,      /*Continue the stopped RBs in UM mode*/

  CRLC_DL_CIPHER_CONFIG_REQ,/* Configure Cophering on DL RLC*/

  CRLC_DL_HFN_REQ,          /* Configure Cophering on DL RLC*/

  //RLC_DL_REGISTER_SRVC_REQ, /* Register Downlink Watermark */

  //RLC_DL_DEREGISTER_SRVC_REQ, /* DeRegister Downlink Watermark */

  RLC_REGISTER_AM_SDU_DISCARD_CALLBACK_REQ, /* Request for registration of SDU  */
                                            /* discard callback function for    */
                                            /* RRC                              */
  RLC_PROCESS_MAC_HS_RESET_REQ,

  CRLC_DL_ABORT_CIPHER_CONFIG_REQ,          /* Request to abort the new DL
                                               ciphering configuration         */

  RLC_HANDLE_SDU_DISCARD_TIMEOUT_EVENT, /* Command to handle SDU discard timer */

  RLC_PDCP_CLEANUP_WM,

  WCDMA_L2_DL_MAX_CMDS            /* Number of DL L2 Command types. must be last entry */

}wcdma_l2_dl_cmd_enum_type;


/* Downlink Layer 2 Command header */
typedef struct
{
  q_link_type link; /* Quaue link */

  wcdma_l2_dl_cmd_enum_type cmd_id; /* Command ID */

}wcdma_l2_dl_cmd_hdr_type;


/* Downlink Layer 2 command data type */
typedef union
{
  /* DL MAC Configuration Request */
  mac_dl_config_type  mac_config_info;

  /* DL RLC Commands */
  /* for internal reset messages  */
  rlc_lc_id_type  dl_rlc_data_id;

  /* for Internal reset messages */
  rlc_lc_id_type  ul_rlc_data_id;

  /* DL TM RLC Configuration Request */
  rlc_dl_tm_config_type dl_tm_config;

  /* DL UM RLC Configuration Request */
  rlc_dl_um_config_type dl_um_config;

  /*DL UM Continue Request*/
  rlc_dl_continue_type dl_continue;

  /* DL AM RLC Configuration Request */
  rlci_dl_am_config_req_type dl_am_config_req;

  /* DL RLC Ciphering Configuration Request */
  rlc_dl_cipher_config_type dl_cipher_config;

  /* DL RLC HFN Configuration Request */
  rlc_dl_hfn_type   dl_hfn;

  /* Register AM SDU discard callback function  */
  rlc_register_am_sdu_discard_callback_type dl_sdu_discard_cb;

  mac_hs_dl_reset_req_type  mac_hs_dl_reset_req;

  uint32 dl_timer_param;

  /* Abort the new ciphering configuration */
  rlc_abort_cipher_config_type abort_cipher_info;

  rlci_cleanup_wm_cmd_type    cleanup_wm_cmd;
  
}wcdma_l2_dl_cmd_data_type;

/* DL Layer 2 command buffer type */
typedef struct
{
  wcdma_l2_dl_cmd_hdr_type    cmd_hdr;

  wcdma_l2_dl_cmd_data_type   cmd_data;

}wcdma_l2_dl_cmd_type;


/*===========================================================================

                      Downlink L2-FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================================

FUNCTION  proc_wcdma_l2_dl_cmd_queue

DESCRIPTION
  This function processes the downlink layer 2 command queue data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/

void  proc_wcdma_l2_dl_cmd_queue ( void );

/*===========================================================================

FUNCTION  wcdma_l2_dl_get_cmd_buf

DESCRIPTION
  This function should be called to get a command buffer from downlink Layer 2
  task command free queue

DEPENDENCIES
  None.

RETURN VALUE

  Pointer to downlink L2 command buffer if one is available
  NULL  otherwise

SIDE EFFECTS

===========================================================================*/
extern  wcdma_l2_dl_cmd_type  *wcdma_l2_dl_get_cmd_buf
(
  void
);


/*===========================================================================

FUNCTION        wcdma_l2_dl_put_cmd

DESCRIPTION
  This function should be called to send a command to downlink Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Item will be enqueued for downlink Layer 2 task and signal will be set to
  indicate that the item has been enqueued.

===========================================================================*/
extern  void  wcdma_l2_dl_put_cmd

(
  /* Pointer to the command buffer */
  wcdma_l2_dl_cmd_type   *cmdptr

);


/* Uplink layer 2 command ID types */
typedef enum
{
  CMAC_UL_CONFIG_REQ = 100,         /* Configure UL MAC */

  /* Configure UL MAC traffic volume measurements*/
  CMAC_UL_TRAFFIC_VOLUME_CONFIG_REQ,

  /* RRC can request MAC to stop transmission on DTCH using this command*/
  CMAC_UL_BLOCK_RACH_TRAFFIC_REQ,

  CMAC_UL_TFC_SUBSET_CFG_REQ,

  CMAC_HS_UL_PROCESS_HDR_LOG_PKT_REQ,

  CMAC_HS_UL_PROCESS_ETFCI_STATUS_LOG_PKT_REQ,

  CMAC_HS_UL_PROCESS_MAC_I_IS_LOG_PKT_REQ,

  CMAC_HS_UL_PROCESS_MAC_HSRACH_TIMERS_LOG_PKT_REQ,

  CRLC_UL_TM_CONFIG_REQ,      /* Configure UL TM RLC */

  CRLC_UL_UM_CONFIG_REQ,      /* Configure UL UM RLC */

  CRLC_AM_CONFIG_REQ,         /* Configure AM RLC */

  CRLC_AM_RESET_REQ,          /* RESET AM RLC */

  CRLC_UL_CIPHER_CONFIG_REQ,  /* Configure Ciphering on DL RLC*/

  CRLC_UL_RESUME_REQ,         /* Resume transmission on RLC channels*/

  CRLC_UL_HFN_REQ,            /* Get the HFN on DL RLC*/

  CRLC_RELEASE_ALL_REQ,       /* Release all non-MBMS RLC entities.  */

  CRLC_RELEASE_ALL_WITH_MBMS_REQ, /* Release all RLC entities.  */

  CRLC_UL_REG_FOR_DATA_IND_REQ, /* Register incoming data indication. */

  CRLC_UL_UNREG_FOR_DATA_IND_REQ, /* Register incoming data indication. */

//  RLC_UL_REGISTER_SRVC_REQ,   /* Request for service registration for RRC */

//  RLC_UL_DEREGISTER_SRVC_REQ,   /* DeRequest for service registration for RRC */

  RLC_REGISTER_AM_SDU_CONFIRM_CALLBACK_REQ, /* Request for SDU confirm    */
                                            /* callback function for RRC  */

  RLC_OPEN_RX_WIN,            /* Open the Receiving Window. */

  RLC_CLOSE_RX_WIN,           /* Close the Receiving Window.  */

  RLCI_UL_RESET_REQ,          /* RESET request for UL AM RLC  */

  RLCI_DL_CONFIG_CNF,         /* Confirm the completion of DL config  */

  RLCI_DL_RESET_DONE,         /* Confirm the completion of the DL RESET */

  RLC_UL_PURGE_WM_REQ,            /* Command to purge the uplink WM */

  CRLC_UL_ABORT_CIPHER_CONFIG_REQ, /* Command to abort the new UL ciphering
                                      configuration                                */

  CRLC_UL_CONTINUE_REQ,    /* Continue transmission on Stopped RLC channels*/
  
  RLCI_UL_TX_AM_STATUS,             /* command to indicate UL to send status PDU */

  RLCI_UL_DUP_RESET_REQ,      /* command to indicate duplicate RESET reception on DL*/

  RLC_HANDLE_TM_TIMER_BASED_SDU_DISCARD_CB_IND,

  RLC_UL_TA_IND,

  WCDMA_L2_UL_MAX_CMDS              /* Number of Ul L2 Command types. must be last entry */

} wcdma_l2_ul_cmd_enum_type;


/* Command header */
typedef struct
{
  q_link_type       link;       /* Quaue link */

  wcdma_l2_ul_cmd_enum_type   cmd_id; /* Command ID */

}wcdma_l2_ul_cmd_hdr_type;


/* Uplink Layer 2 command data type */
typedef union
{
  /* UL MAC Configuration Request */
  mac_ul_config_type          mac_config_info;

  /* UL MAC TFCC COnfig Info */
  mac_tfc_subset_restriction_info_type  mac_tfc_subset_config_info;

  /* for internal reset primitive messages */
  rlc_lc_id_type              ul_rlc_data_id;

  /* UL RLC TM Configuration Request */
  rlc_ul_tm_config_type       ul_tm_config;

  /* UL RLC UM Configuration Request */
  rlc_ul_um_config_type       ul_um_config;

  /* UL RLC AM Configuration Request */
  rlc_am_config_req_type      am_config;

  /* UL RLC Ciphering Configuration Request */
  rlc_ul_cipher_config_type   ul_cipher_config;

  /* UL RLC Resume Request */
  rlc_ul_resume_type          ul_resume;

  /* UL RLC Continue Request */
  rlc_ul_continue_type ul_continue;

  /* UL RLC HFN Request */
  rlc_ul_hfn_type             ul_hfn;

  /* register the SDU confirmation callback function  */
  rlc_register_am_sdu_confirm_callback_type ul_sdu_cnf_cb;

  /* Confirmation for DL AM RLC completing its config */
  rlci_dl_config_cnf_type     dl_config_cnf;

  rlc_ul_register_for_data_ind_type ul_reg_data_ind;

  rlc_ul_unregister_for_data_ind_type ul_unreg_data_ind;

  /* Parameter for purging the UL WM when requested by RRC */
  rlc_ul_purge_wm_ind_type  ul_purge_wm_ind;

  /* Parameter for aborting the new ciphering configuration
     when requested by RRC                                 */
  rlc_abort_cipher_config_type abort_cipher_info;

  /* Parameter for Uplink Timer expiration  */
  uint32                      ul_timer_param;

  /* Configure UL MAC for traffic volume measurement */
  mac_ul_traffic_meas_config_type *ul_traffic_volume_cfg;

  boolean                         block_rach;
  
  rlc_ta_info_type tune_away_info;

}wcdma_l2_ul_cmd_data_type;


/* UL Layer 2 command buffer type */
typedef struct
{
  wcdma_l2_ul_cmd_hdr_type    cmd_hdr;

  wcdma_l2_ul_cmd_data_type   cmd_data;

} wcdma_l2_ul_cmd_type;



/*===========================================================================

FUNCTION        wcdma_l2_ul_put_cmd

DESCRIPTION
  This function should be called to send a command to uplink Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Item will be enqueued for uplink Layer 2 task and signal will be set to
  indicate that the item has been enqueued.

===========================================================================*/
extern  void     wcdma_l2_ul_put_cmd

(
  /* Pointer to the command buffer */
  wcdma_l2_ul_cmd_type   *cmd_ptr

);


/*===========================================================================

FUNCTION  wcdma_l2_ul_get_cmd_buf

DESCRIPTION
  This function should be called to get a command buffer from uplink Layer 2
  task command free queue

DEPENDENCIES
  None.

RETURN VALUE

  Pointer to the uplink L2 command buffer if one is available
  NULL  otherwise

SIDE EFFECTS

===========================================================================*/
extern  wcdma_l2_ul_cmd_type    *wcdma_l2_ul_get_cmd_buf
(
  void
);
#endif /* WL1API_H */


