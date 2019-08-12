#ifndef TDSRLCRRCIF_H
#define TDSRLCRRCIF_H
/*===========================================================================
              R L C - R R C  I N T E R F A C E  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  RLC - RRC Interface.
  
Copyright (c) 2002-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rlc/inc/tdsrlcrrcif.h#1 $

when      who     what, where, why
--------  ---     ----------------------------------------------------------
11/23/10  cyx     Remove tdsmvsif.h to clean up l2 interface
10/24/10  guo     Initial release Upmerged WCDMA to TDS

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/
#include "tdscdma_variation.h"
#include "tdsuecomdef.h"
#include "tdsrlcdsapif.h"


/* --------------------------------- */
/* PUBLIC APIs of tdsrlcrrcif.h         */
/* --------------------------------- */

/*--------------------------------------------------------------------------- 
  Type for defining the action to take on a logical channel.  
---------------------------------------------------------------------------*/
typedef enum {
  TDSRLC_ESTABLISH = 0,                    /* Initialize the protocol         */ 
                                        /* parameters as specified, and    */
                                        /* enter the data transfer ready   */
                                        /* state.                          */

  TDSRLC_RELEASE,                          /* Reset all the protocol          */
                                        /* parameters, and exit the        */
                                        /* data transfer ready state.      */
  TDSRLC_MODIFY                           /* Modify the indicated protocol   */
                                        /* parameters as specified, and    */
                                        /* remain in the data transfer     */
                                        /* ready state.                    */

} tdsrlc_channel_action_e_type; 

typedef enum {
  TDSRLC_RE_ESTABLISH_DL_UL,               /* Both downlink and uplink of an  */
                                        /* AM entity is re-established     */
  TDSRLC_RE_ESTABLISH_DL,                  /* Only downlink of an AM entity   */
                                        /* is re-established               */
  TDSRLC_RE_ESTABLISH_UL,                  /* Only uplink of an AM entity is  */
                                        /* is re-established               */
  TDSRLC_RE_ESTABLISH_NONE
}tdsrlc_reestablish_e_type;

/*--------------------------------------------------------------------------- 
  Type that identifies if the HFN should be reported for 
  all established UM and AM uplink logical channels.
---------------------------------------------------------------------------*/
typedef enum {
   TDSRLC_ALL = 0,                         /* Report HFN for all UM and AM    */
                                        /* channels                        */
   TDSRLC_ONE                              /* Report HFN for a single UM or AM*/
                                        /* channel.                        */
} tdsrlc_all_or_one_e_type;

/*--------------------------------------------------------------------------- 
  Type that defines the mode for discharge of the RLC buffer at
  the transmitter.
---------------------------------------------------------------------------*/
typedef enum {
   TDSRLC_TBE = 0,                         /* Timer based with explicit       */
                                        /* signalling.                     */                                                                
   TDSRLC_MAXDAT,                          /* Discard after MaxDAT            */
                                        /* retransmissions.                */
   TDSRLC_NO_DISCARD                       /* No Discard (perform reset       */
                                        /* procedure after MaxDAT          */
                                        /* retransmissions.                */  
} tdsrlc_discard_mode_e_type;

/*--------------------------------------------------------------------------- 
  Type that indicates the ciphering algorithm sent to RLC in case of
  RLC Re-establishment.
---------------------------------------------------------------------------*/
typedef enum {
  TDSRLC_UEA_0 = 0,                        /* Indicates UEA_0 is to be used   */
  TDSRLC_UEA_1 = 1,                        /* Indicates UEA_1/Kasumi to use   */
  TDSRLC_UEA_2 = 2,                        /* Indicates UEA_2/Snow3G to use   */
  TDSRLC_UEA_INVALID = 0xFF 
} tdsrlc_cipher_algo_e_type;


  /*-------------------------------------------------------------------------
    Type that indicates whether to use 7 bit LI or 15-bit LI or release 99 
    calculation for obtaining li length.
  -------------------------------------------------------------------------*/
typedef enum {
  TDSLI_7_BITS = 0,                         /* 7 bits L1                     */
  TDSLI_15_BITS,                            /* 15 bits LI                    */
  TDSLI_SAME_AS_BEFORE,                     /* Use the previous one */
  TDSLI_UNSPECIFIED                         /* Calculate as in Rel 99        */
} tdsrlc_li_e_type;

#ifdef FEATURE_TM_LB
typedef enum
{
  TDSRLC_LOOPBACK_NA,     /* No loopback is required */
  TDSRLC_LOOPBACK_MODE_1, /* loopback mode 1 */
  TDSRLC_LOOPBACK_MODE_2  /* loopback mode 2 */
} tdsrlc_loop_back_mode_e_type;
#endif /* FEATURE_TM_LB */

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/
typedef struct {
  uint16 ul_cfg_mask;
  uint16 dl_cfg_mask;
  uint8  act_time;                      /* The CFN at which the specified  */
                                        /* configuration should be         */ 
                                        /* activated. Only valid if the    */
                                        /* activation_time_incl = TRUE;    */
 boolean  act_incl;                    /* Flag indicating whether the     */
                                        /* specified configuration should  */  
                                        /* be activated at the time or not.*/
  tdsrlc_channel_action_e_type action;     /* Action to take on the logical   */ 
                                        /* action : Establisha channel,    */
                                        /* release a channel, or Modify    */
                                        /* parameters a logical channel.   */
 
                                        
} tdsrlc_lc_action_type;

/* UPLINK */
/*---------------------------------------------------------------------------
  Type to establish a single transparent mode logical channel. 
---------------------------------------------------------------------------*/


typedef struct {                          
  uint16  timer_discard;                /* Elapsed time in milliseconds    */
                                           /* before an SDU is discarded at   */ 
                                           /* the transmitter                 */

  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/
                                        /* RLC logical channel             */ 
  boolean seg_ind;                      /* Flag that indicates whether     */
                                        /* segmentation should be performed*/ 
                                        /* on the logical channels being   */
                                        /* configured.                     */
  uecomdef_logchan_e_type lc_type;      /* Type of Uplink Transparent Mode */
                                        /* logical channel                 */
 
} tdsrlc_ul_tm_channel_config_type;


/*---------------------------------------------------------------------------
  Type to establish, release or re-configure Transparent mode uplink 
  logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8  nchan;                         /* Number of transparent mode UL   */ 
                                        /* logical channels to configure   */
  tdsrlc_ul_tm_channel_config_type  chan_info[UE_MAX_UL_TM_CHANNEL];
                                        /* Uplink transparent mode RLC     */
                                        /* configuration for each channel  */
  tdsrlc_lc_action_type ul_tm_act[UE_MAX_UL_TM_CHANNEL];       
                                        /* Action related information for  */
                                        /* RLC UL TM.                      */
#ifdef FEATURE_TM_LB
  tdsrlc_loop_back_mode_e_type loop_back_mode; /* Indicates if channels are to be */
                                            /* configured for loopback or not  */
#endif /* FEATURE_TM_LB */
} tdsrlc_ul_tm_config_type;


/*---------------------------------------------------------------------------
  Type to establish, release or re-configure a single unacknowledged 
  Mode uplink logical channel. 
---------------------------------------------------------------------------*/

typedef struct {
  uint32 rlc_size;                      /* Max. PDU size.                  */
  uint32 hfn;                           /* Hyperframe Number               */
  uint16 timer_discard;                 /* Elapsed time in milliseconds    */
                                          /* before an SDU is discarded at   */
                                          /* the transmitter                 */
  rlc_lc_id_type lc_id;                 /* Unique identifier for the Uplink*/
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uecomdef_logchan_e_type lc_type;      /* Type of Uplink Unacknowledged   */
                                        /* mode logical channel            */


  uecomdef_umts_cipher_algo_e_type ciph_algo; /* Indicates the ciphering algo    */

  

  boolean alternate_e_bit;          /* TRUE - Alternate E-bit interpretation */
                                    /* FALSE - Normal E-bit interpretation   */
#ifdef FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA
  boolean segmentation_enabled;                          /*For CSoHSPA, segmentation is enabled in UL*/
#endif /*FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA*/

} tdsrlc_ul_um_channel_config_type;



/*---------------------------------------------------------------------------
  Type to establish, release or re-configure Unacknowledged mode uplink
  logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  byte  nchan;                          /* Number of Unacknowledged mode   */
                                        /* uplink logical channels to      */
                                        /* configure                       */
  tdsrlc_ul_um_channel_config_type chan_info[TDSUE_MAX_UL_UM_CHANNEL];
                                        /* Uplink Unacknowledged mode RLC  */
                                        /* configuration for each channel  */
  tdsrlc_lc_action_type ul_um_act[TDSUE_MAX_UL_UM_CHANNEL];       
                                        /* Action related information for  */
                                        /* RLC UL UM.                      */
} tdsrlc_ul_um_config_type;


/*---------------------------------------------------------------------------
  Type to enable or disable ciphering on a single unacknowledged or 
  acknowledged mode uplink logical channel. 
---------------------------------------------------------------------------*/

typedef struct {
 uint32  cipher_key_id;                /* Ciphering key Id. Valid if      */
                                           /* ciphering mode is ON            */
  uint32  hfn;                          /* Ciphering Hyperframe number.    */

  uint16  ciph_act_sn;                  /* Serial Number at which Ciphering*/
                                        /* will be activated.              */
                                        
  rlc_lc_id_type lc_id;                 /* Unique identifier for the Uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  rb_id_type  rb_id;                    /* Radio Bearer Id. Valid only if  */
                                      /* Ciphering mode is ON            */

 
  uecomdef_umts_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */ 

  boolean cipher_mode;                  /* Indicates whether this logical  */
                                       /* channel should be ciphered (ON) */
                                       /* or not (OFF)                    */


                                        
} tdsrlc_ul_ch_ciphering_type;


/*---------------------------------------------------------------------------
  Type to enable or disable ciphering on one or more unacknowledged or 
   acknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/
                                        /* channels to configure           */
  tdsrlc_ul_ch_ciphering_type  ciphering[TDSUE_MAX_UL_UM_CHANNEL + 
                                                         TDSUE_MAX_AM_ENTITY];
                                        /* Ciphering Information for each  */
                                        /* unacknowledged or acknowledged  */
                                        /* mode uplink logical channel     */
} tdsrlc_ul_cipher_config_type;

/*---------------------------------------------------------------------------
  Type to resume uplink data transmission on one or more 
  unacknowledged or acknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/ 
                                        /* channels to configure           */
  rlc_lc_id_type  channel[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                        /* Uplink logical channels to      */
                                        /* resume                          */
} tdsrlc_ul_resume_type;

/*---------------------------------------------------------------------------
  Type to Continue uplink data transmission on one or more 
  unacknowledged or acknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/ 
                                        /* channels to configure           */
  rlc_lc_id_type  channel[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                        /* Uplink logical channels to      */
                                        /* continue                          */
} tdsrlc_ul_continue_type;

/*---------------------------------------------------------------------------
  Type to Continue DownLink data transmission on one or more 
  unacknowledged mode uplink logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/ 
                                        /* channels to configure           */
  rlc_lc_id_type  channel[TDSUE_MAX_DL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                        /* Uplink logical channels to      */
                                        /* continue                          */
} tdsrlc_dl_continue_type;


/*---------------------------------------------------------------------------
  Type to request the current HFN value for all established uplink 
  logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  tdsrlc_all_or_one_e_type  all_or_one;    /* Identifies if the HFN should be */
                                        /* reported for all channels or    */
                                        /* the specified channel.          */
  rlc_lc_id_type lc_id;                 /* Identifies the uplink channel   */
                                        /* whose HFN value is requested.   */
                                        /* Only valid if all_one is set to */
                                        /* ONE                             */
} tdsrlc_ul_hfn_type;

/*---------------------------------------------------------------------------
  Type to establish a Transparent Mode downlink channel.
---------------------------------------------------------------------------*/
typedef struct {
  uecomdef_logchan_e_type lc_type;      /* Type of logical channel         */

  rlc_lc_id_type lc_id;                 /* Unique identifier for the       */
                                        /* Downlink RLC logical channel    */
  boolean seg_ind;                      /* Flag that indicates whether     */
                                        /* segmentation should be performed*/
                                        /* on the logical channels being   */
                                        /* configured                      */
} tdsrlc_dl_tm_channel_config_type;


/*---------------------------------------------------------------------------
  Type to establish, release or re-configure transparent 
  Mode downlink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8  nchan;                         /* Number of transparent mode      */
                                        /* downlink logical channels to    */
                                        /* configure                       */
  tdsrlc_dl_tm_channel_config_type chan_info[UE_MAX_DL_TM_CHANNEL];
                                        /* Downlink transparent mode RLC   */
                                        /* configuration for each channel  */
  tdsrlc_lc_action_type dl_tm_act[UE_MAX_DL_TM_CHANNEL];
                                        /* Action related information for  */
                                        /* RLC DL TM.                      */

} tdsrlc_dl_tm_config_type;

 
 
/*---------------------------------------------------------------------------
  Type to establish, release or re-configure a single 
  unacknowledged Mode downlink logical channel.
---------------------------------------------------------------------------*/

typedef struct {
  uint32 rlc_size;                      /* Max. PDU size.                  */
  uint32 hfn;                           /* Hyperframe Number               */

 
  uecomdef_logchan_e_type lc_type;      /* Type of Downlink Unacknowledged */
                                        /* Mode logical channel            */
 

  uecomdef_umts_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */

  tdsrlc_li_e_type li_size;                /* Network can explicitly specify
                                           li size in Release 5            */
  rlc_lc_id_type lc_id;                 /* Unique identifier for the       */
                                       /* Downlink RLC logical channel    */


  boolean alternate_e_bit;          /* TRUE - Alternate E-bit interpretation */
                                    /* FALSE - Normal E-bit interpretation   */
#ifdef FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA  
  boolean seq_num_delivery_enabled; /* Indicates if RLC needs to indicate the SN to PDCP in DL*/
#endif /* FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA */
} tdsrlc_dl_um_channel_config_type;


/*---------------------------------------------------------------------------
  Type to establish, release or re-configure unacknowledged 
  Mode downlink logical channels.
---------------------------------------------------------------------------*/
   
typedef struct {
  uint8  nchan;                         /* Number of transparent mode DL   */ 
                                        /* logical channels to configure   */
  tdsrlc_dl_um_channel_config_type chan_info[TDSUE_MAX_DL_UM_CHANNEL];
                                        /* Downlink Unacknowledged mode RLC*/ 
                                        /* configuration for each channel  */
  tdsrlc_lc_action_type dl_um_act[TDSUE_MAX_DL_UM_CHANNEL];       
                                        /* Action related information for  */
                                        /* RLC DL UM.                      */
} tdsrlc_dl_um_config_type;

/*---------------------------------------------------------------------------
  Type to enable or disable ciphering on a single unacknowledged or  
  acknowledged mode downlink logical channel.
---------------------------------------------------------------------------*/

typedef struct {
  uint32 cipher_key_id;                 /* Ciphering key Id. Valid if      */
                                           /* ciphering mode is ON            */
  uint32 hfn;                           /* Ciphering Hyperframe number.    */

  uint16  ciph_act_sn;                  /* Serial Number at which Ciphering*/
                                        /* will be activated for Downlink  */

  rlc_lc_id_type lc_id;                 /* Unique identifier for the DL    */ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  boolean cipher_mode;                  /* Indicates whether this logical  */
                                        /* channel should be ciphered (ON) */
                                        /* or not (OFF)                    */
 
  rb_id_type rb_id;                     /* Radio Bearer Id. Valid only if  */
                                        /* Ciphering mode is ON            */

  uecomdef_umts_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */
} tdsrlc_dl_ciphering_type;


/*---------------------------------------------------------------------------
  Type to enable or disable ciphering on one or more unacknowledged or 
  acknowledged mode downlink logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged mode   */
                                        /* downlink logical channels to    */
                                        /* configure                       */
  tdsrlc_dl_ciphering_type ciphering[TDSUE_MAX_DL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                        /* Ciphering Information for each  */
                                        /* unacknowledged mode downlink    */
                                        /* logical channel                 */
} tdsrlc_dl_cipher_config_type;


/*---------------------------------------------------------------------------
  Type to request the current HFN value for all established
  downlink AM and UM logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Identifies the downlink channel */
                                        /* whose HFN value is requested.   */
                                        /* Only valid if all_one is set to */
                                        /* ONE                             */
  tdsrlc_all_or_one_e_type  all_or_one;    /* Identifies if the HFN should be */
                                        /* reported for all channels or    */
                                        /* the specified channel.          */
} tdsrlc_dl_hfn_type;

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
} tdsrlc_am_common_config_type;

/*---------------------------------------------------------------------------
  Type for the Transmission RLC Discard Information.
---------------------------------------------------------------------------*/
typedef struct {
  boolean tx_discard_incl;              /* Indicates whether transmission  */
                                        /* RLC discard parameters are      */
                                        /* included or not                 */
  tdsrlc_discard_mode_e_type discard_mode; /* Mode for discharge of the RLC   */
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
} tdsrlc_discard_info_type;


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
} tdsrlc_polling_info_type;



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
  tdsrlc_discard_info_type discard_info;   /* Transmission RLC Discard info   */
  tdsrlc_polling_info_type poll_info;      /* Polling information             */
} tdsrlc_ul_info_type;

/*--------------------------------------------------------------------------- 
  Type to establish, release or re-configure a single acknowledged 
  mode entity.
---------------------------------------------------------------------------*/
typedef struct {
  uint32 hfn;                           /* Hyperframe Number               */
  uint32 ul_rlc_size;                   /* Max. PDU size.                  */
  tdsrlc_ul_info_type rlc_info;            /* RLC information                 */
  uecomdef_umts_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */
#if defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS) 
#ifdef FEATURE_TDSCDMA_RLC_SPECIAL_HE_VALUE
  boolean special_value_he_config;      /* Special value for HE Config ?   */
#endif
#endif
} tdsrlc_ul_am_config_type;

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
} tdsrlc_status_info_type; 

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
  tdsrlc_status_info_type status_info;     /* Status information for a        */
                                        /* downlink AM entity              */
} tdsrlc_dl_info_type;

/*--------------------------------------------------------------------------- 
  Type to establish, release or re-configure a single acknowledged 
  mode downlink entity.
---------------------------------------------------------------------------*/

typedef struct {
  uint32 hfn;                           /* Hyperframe Number               */
  uint32 dl_rlc_size;                   /* Max. PDU size.                  */
  tdsrlc_dl_info_type rlc_info;            /* Typedef for RLC downlink        */

#if defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS)
 /* TRUE - FLEXIBLE pdu size, FALSE - FIXED pdu size
   * dl_rlc_size is valid only in case of FIXED pdu size
   * am_li_type is valid only in case of FLEXIBLE pdu size */
  boolean                               am_pdu_size_flexbile;
  tdsrlc_li_e_type                         am_li_type;

#ifdef FEATURE_TDSCDMA_RLC_SPECIAL_HE_VALUE
  boolean special_value_he_config;      /* Special Value for HE Config ?   */
#endif
#endif
                                        /* information                     */
  uecomdef_umts_cipher_algo_e_type ciph_algo;     /* Indicates the ciphering algo    */
  
} tdsrlc_dl_am_config_type;

/*--------------------------------------------------------------------------- 
  Type to establish, release or re-configure a single acknowledged 
  mode entity.
---------------------------------------------------------------------------*/
typedef struct {
  tdsrlc_am_common_config_type common_config;/* Common config for both        */
                                          /* Uplink and Downlink for AM    */
  
  uint8  ul_nchan;                        /* Number of Acknowledged mode   */
                                          /* uplink logical channels to    */
                                          /* configure                     */
  tdsrlc_ul_am_config_type rlc_ul_am_config; /* Config info for UL only       */
  uint8  dl_nchan;                        /* Number of Acknowledged mode   */
                                          /* downlink logical channels to  */
                                          /* configure                     */
  tdsrlc_dl_am_config_type rlc_dl_am_config; /* Config info for DL only       */

  tdsrlc_reestablish_e_type reestab_entity;  /* This variable is used when    */
                                          /* action = TDSRLC_RE_ESTABLISH only*/
                                          
} tdsrlc_am_config_type;

typedef enum
{
  TDSRLC_INTER_RNC_HO_NA,     /* No Inter RNC*/
  TDSRLC_INTER_RNC_HO_START, /* Inter RNC start*/
  TDSRLC_INTER_RNC_HO_END  /* Inter RNC start */
} tdsrlc_inter_rnc_ho_action_e_type; 

/*---------------------------------------------------------------------------
  Type to establish, release or re-configure of acknowledged mode entities.
---------------------------------------------------------------------------*/
typedef struct {
  uint8  nentity;                       /* Number of acknowledged mode     */
                                        /* entities to configure           */
  tdsrlc_am_config_type *am_info[TDSUE_MAX_AM_ENTITY];
                                        /* Downlink transparent mode RLC   */
                                        /* configuration for each channel  */
  tdsrlc_lc_action_type am_act[TDSUE_MAX_AM_ENTITY];
                                        /* Action related information for  */
                                        /* RLC UL TM.                      */

  tdsrlc_inter_rnc_ho_action_e_type inter_RNC_ho;

} tdsrlc_am_config_req_type;

typedef struct {
  uint16  num_lc;
  
  boolean block_status_timer;       /* whether to block status tmr*/
  rlc_lc_id_type lc_id[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                   /*plink logical channels         */
} tdsrlc_ul_register_for_data_ind_type ;

typedef struct {
  uint16  num_lc;
  boolean resume_status_timer;       /* to resume status tmr*/
  rlc_lc_id_type lc_id[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                   /*plink logical channels         */
} tdsrlc_ul_unregister_for_data_ind_type ;

typedef struct {

  /* number of logical channels for which watermark is to be purged */
  uint8          num_lc_id;       

  /* LC ID's of the channels for which WM is to be purged           */
  rlc_lc_id_type lc_id[TDS_MAX_RB];

  /* Number of SDUs for each LC which are to be purged */
  uint8          num_sdus[TDS_MAX_RB];

}tdsrlc_ul_purge_wm_ind_type;

/*---------------------------------------------------------------------------
  Type to specify the logical channels for which the ongoing security
  procedure needs to be aborted.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of Unacknowledged and/or */
                                        /* acknowledged mode uplink logical*/
                                        /* channels to abort config on.    */
  rlc_lc_id_type  lc_id[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                        /* Logical channel id's for which 
                                           ciphering configuration needs to 
                                           be aborted. Since MAX UM channels 
                                           for UL and DL are same, we use 
                                           the same struct for UL and DL   */
                                        
} tdsrlc_abort_cipher_config_type;



/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

#define TDSRLC_MODIFY_CFG                0x0001
#define TDSRLC_STOP_CFG                    0x0002
#define TDSRLC_CONTINUE_CFG            0x0004
#define TDSRLC_RE_ESTABLISH_CFG     0x0008



/*--------------------------------------------------------------------------- 
  Type that indicates the type of error that occured on the 
  uplink or downlink channel.
---------------------------------------------------------------------------*/
typedef enum {
  TDSRLC_RESET = 0,                        /* The RLC protocol was reset due  */
                                        /* to an unrecoverable error. Data */
                                        /* loss may occur due to the reset.*/
  TDSRLC_RESET_FAIL                        /* The RLC protocol reset failed,  */
                                        /* i.e., the link is down.         */
} tdsrlc_error_e_type;



typedef enum {
  TDSRLC_NO_CIPHER_CFG_PENDING = 0,         /* no ciphering config pending   */
  TDSRLC_CIPHER_CFG_PENDING,                /* Yes, there is at least one    */
  TDSRLC_CIPHER_STATUS_UNSPECIFIED,         /* Can not find the LC           */
  TDSRLC_MAX_CIPHER_CFG_STATUS
} tdsrlc_cipher_cfg_pending_e_type;

#if defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS)
  typedef enum {
   TDSFLEXIBLE_PDU_SIZE_CONF,
   TDSFIXED_PDU_SIZE_CONF
}tdsrlc_dl_am_pdu_e_type;
#endif

/* Type to indicate the command that triggers the RLC suspension or stop*/
typedef enum
{
  TDSRLC_INVALID,
  TDSRLC_SUSPEND_CMD,
  TDSRLC_STOP_CMD
}tdsrlc_cmd_type;

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

/*---------------------------------------------------------------------------
  Type for notifying RRC about errors. Used only for Acknowledged Mode
  logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  boolean direction;                    /* Indicates whether the error     */
                                        /* occured on the uplink or the    */
                                        /* downlink.                       */
  rlc_lc_id_type lc_id;                 /* Identifies the UL or DL         */
                                        /* logical channel on which the    */
                                        /* error condition occured         */
  tdsrlc_error_e_type error_type;          /* Indicates the type of error that*/
                                        /* occured.                        */
} tdsrlc_status_ind_type;



/* UPLINK */




/*---------------------------------------------------------------------------
  Type to suspend uplink data transmission on a single 
  Unacknowledged or Acknowledged Mode logical channel. 
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uint8  n;                             /* RLC shall not transmit a PDU    */
                                        /* with Sequence number >= Current */
                                        /* Sequence Number+n. The sequence */
                                        /* number should be less than the  */
                                        /* maximum allowed sequence number.*/
                                         /*  incase of TDSRLC_STOP cmd, n should be set to 0*/
} tdsrlc_ul_stop_suspend_info_type;


/*---------------------------------------------------------------------------
  Type to suspend uplink data transmission on one or more Unacknowledged  
  or Acknowledged Mode logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8  nchan;                         /* Number of Unacknowledged or     */
                                        /* unacknowledged mode uplink      */
/* Type to indicate the command that triggers the RLC suspension or stop*/
  tdsrlc_cmd_type  ul_cmd_type;
                                        /* logical channels to suspend     */
  tdsrlc_ul_stop_suspend_info_type   suspend_stop[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                        /* Suspend Information for each    */
                                        /* uplink logical channel          */
} tdsrlc_ul_stop_suspend_request_type;

/*---------------------------------------------------------------------------
  Type to stop downlink data transmission on one or more Unacknowledged  
  or Acknowledged Mode logical channels.
---------------------------------------------------------------------------*/

typedef struct {
  uint8  nchan;                         /* Number of Unacknowledged or     */
                                        /* unacknowledged mode downlink      */
                                        /* logical channels to stop     */
  rlc_lc_id_type lc_id[TDSUE_MAX_DL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                        /* RLC ID of channels to be     */
                                        /* stopped          */
} tdsrlc_dl_stop_request_type;

/*---------------------------------------------------------------------------
  Type to confirm that uplink data transmission on the specified 
  logical channel has been suspended.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uint16 rlc_sn;                        /* The sequence number of the next */
                                        /* PDU that will be sent when      */
                                        /* data transmission on the        */
                                        /* specified channel is resumed.   */
} tdsrlc_ul_chsuspend_cnf_type;


/*---------------------------------------------------------------------------
  Type to confirm that uplink data transmission on the logical channels 
  specified by RRC, has been suspended.
---------------------------------------------------------------------------*/

typedef struct {
  uint8  nchan;                         /* Number of Unacknowledged or     */
                                        /* unacknowledged mode downlink    */
                                        /* logical channels for which      */
                                        /* suspend confirmation is sent to */
                                        /* RRC                             */
  tdsrlc_ul_chsuspend_cnf_type cnf[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                        /* Suspend Confirmation Information*/
                                        /* for each uplink logical channel */
} tdsrlc_ul_stop_suspend_cnf_type;


/*---------------------------------------------------------------------------
  Type to request the current HFN value.
---------------------------------------------------------------------------*/
typedef struct {
  boolean hfn_valid;
  uint16 rlc_sn;

  uint32 hfn;
  rlc_lc_id_type rlc_id;
} tdsrlc_hfn_type;


/*---------------------------------------------------------------------------
  Type to request the current HFN values for all established 
  logical channels w/ Ciphering being turned on.
---------------------------------------------------------------------------*/
typedef struct {
  uint8  ul_nchan;                       /* Number of UM and/or AM channels*/
                                         /* HFN for  UL                    */
  uint8  dl_nchan;                       /* Number of UM and/or AM channels*/
                                         /* HFN for  DL                    */
  tdsrlc_hfn_type hfn_per_rb_ul[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
  tdsrlc_hfn_type hfn_per_rb_dl[TDSUE_MAX_DL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                         /* Values of all UM and/or AM ch  */
                                         /* for both UL and DL             */
} tdsrlc_hfn_req_type;


/*---------------------------------------------------------------------------
  Type which contains logical channel ID and the current sequence number
  for the corresponding lc_id.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type  lc_id;                 /* Logical ch ID for which SN is  */
                                         /* set                            */
  uint16 rlc_sn;                         /* SN for the lc_id               */
} tdsrlc_sn_type;

/*---------------------------------------------------------------------------
  Type to request Uplink SN's for a number of Logical Channels
---------------------------------------------------------------------------*/
typedef struct {
  uint8  nchan;                          /* Number of UM and/or AM channels*/
                                         /* for which SN is requested      */
  tdsrlc_sn_type rlc_ul_sn[TDSUE_MAX_UL_UM_CHANNEL + TDSUE_MAX_AM_ENTITY];
                                         /* SN type of all UM and/or AM ch */
                                         /* for both UL and DL             */
} tdsrlc_ul_sn_req_type;

/*---------------------------------------------------------------------------
  Type to report the current HFN value for a uplink logical 
  channel to RRC.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uint32 hfn;                           /* The current hfn value (upper 20 */
                                        /* bits of count)                  */
} tdsrlc_ul_hfn_info_type;


/*---------------------------------------------------------------------------
  Type to report the config result for TM in Uplink to RRC.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uecomdef_status_e_type  result;       /* SUCCESS or FAILURE              */
} tdsrlc_ul_tm_config_status_type;


/*---------------------------------------------------------------------------
  Type to report the config result for all TM channels in Uplink to RRC
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of UL logical channels   */
                                        /* for which hfn is to be reported */
  tdsrlc_ul_tm_config_status_type tm_config_result[UE_MAX_UL_TM_CHANNEL];
                                        /* Stores the hyperframe info for  */
                                        /* each logical channel            */
} tdsrlc_ul_tm_config_cnf_type;

/*---------------------------------------------------------------------------
  Type to report the config result for UM in Uplink to RRC.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the uplink*/ 
                                        /* RLC logical channel. Can have a */
                                        /* value from 0 to 18.             */
  uecomdef_status_e_type  result;       /* SUCCESS or FAILURE              */
} tdsrlc_ul_um_config_status_type;


/*---------------------------------------------------------------------------
  Type to report the config result for all UM channels in Uplink to RRC
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of UL logical channels   */
                                        /* for which hfn is to be reported */
  tdsrlc_ul_um_config_status_type um_config_result[TDSUE_MAX_UL_UM_CHANNEL];
                                        /* Stores the hyperframe info for  */
                                        /* each logical channel            */
} tdsrlc_ul_um_config_cnf_type;


/* DOWNLINK */

/*---------------------------------------------------------------------------
  Type to report the current HFN value for a downlink logical 
  channel to RRC.
---------------------------------------------------------------------------*/

typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the DL    */ 
                                        /* RLC logical channel.            */
  uint32 hfn;                           /* The current hfn value (upper 20 */
                                        /* bits of count ).                */
} tdsrlc_dl_hfn_info_type;


/*---------------------------------------------------------------------------
  Type to report the current HFN value for all established 
  downlink acknowledged and unacknowledged mode logical channels to RRC.
---------------------------------------------------------------------------*/

typedef struct {
  uint8 nchan;                          /* Number of downlink logical      */
                                        /* channels for which hfn is to be */
                                        /* reported                        */
  tdsrlc_dl_hfn_info_type hfn_info[TDSUE_MAX_DL_UM_CHANNEL + 
                                         UE_MAX_DL_AM_CHANNEL];
                                        /* Stores the hyperframe info for  */
                                        /* each logical channel            */
} tdsrlc_dl_hfn_cnf_type;

 
/*---------------------------------------------------------------------------
  Type to report the config result for TM in Downlink to RRC.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the DL    */ 
                                        /* RLC logical channel. Value range*/
                                        /* is from 0 to 18.                */
  uecomdef_status_e_type  result;       /* SUCCESS or FAILURE              */
} tdsrlc_dl_tm_config_status_type;


/*---------------------------------------------------------------------------
  Type to report the config result for all TM channels in Uplink to RRC
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of downlink TM logical   */
                                        /* channels for which the config   */
                                        /* status is reported              */
  tdsrlc_dl_tm_config_status_type tm_config_result[UE_MAX_DL_TM_CHANNEL];
                                        /* Stores the config status for    */
                                        /* each logical channel            */
} tdsrlc_dl_tm_config_cnf_type;

/*---------------------------------------------------------------------------
  Type to report the config result for UM in Uplink to RRC.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the DL    */
                                        /* RLC logical channel. value range*/ 
                                        /* is from 0 to 18.                */
  uecomdef_status_e_type  result;       /* SUCCESS or FAILURE              */
} tdsrlc_dl_um_config_status_type;


/*---------------------------------------------------------------------------
  Type to report the config result for all UM channels in Uplink to RRC
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of downlink UM logical   */
                                        /* channels for which the config   */
                                        /* status is reported              */        
  tdsrlc_dl_um_config_status_type um_config_result[TDSUE_MAX_DL_UM_CHANNEL];
                                        /* Stores config status for        */
                                        /* each logical channel            */
} tdsrlc_dl_um_config_cnf_type;

/*---------------- Acknowledged Mode --------------------------------------*/



  
/*--------------------------------------------------------------------------- 
  Type to establish, release or re-configure acknowledged mode entity
  in downlink.
---------------------------------------------------------------------------*/
typedef struct {
  rb_id_type rb_id;
  uecomdef_status_e_type  result;
                                        /* SUCCESS or FAILURE              */
} tdsam_entity_config_cnf_type;

/*---------------------------------------------------------------------------
  Type to report the config result for all TM channels in Uplink to RRC
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of UL logical channels   */
                                        /* for which hfn is to be reported */
  tdsam_entity_config_cnf_type am_config_result[TDSUE_MAX_AM_ENTITY];
                                        /* Config confirm information for  */
                                        /* each entity.                    */
} tdsrlc_am_config_cnf_type;



/*---------------------------------------------------------------------------
  Type to request the pending HFN values for a given 
  logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  boolean   ul_pending_hfn_present;     /* UL HFN pending present flag for rb_id */
  boolean   dl_pending_hfn_present;    /* DL HFN pending for rb_id  */
  uint32    ul_pending_hfn;
  uint32    dl_pending_hfn;
} tdsrlc_pending_hfn_req_type;


/*===========================================================================
**                  FUNCTION PROTO-TYPES
**=========================================================================*/

/*===========================================================================

FUNCTION RLC_RRC_GET_HFN_REQ

DESCRIPTION
   This function is called by RRC to get latest HFN's for all established
   RLC UM and AM radio-bearers.  RLC populates the * to struct 
   tdsrlc_hfn_req_type.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS:  If HFN's for all UM and AM rb's are populated.
  FAILURE:  If no UM and AM rb's are present.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type tdsrlc_rrc_get_hfn_req
(
  tdsrlc_hfn_req_type* hfn_req             /*ptr to hfn request type          */
);




/*===========================================================================

FUNCTION RLC_RRC_GET_WM_INFO_REQ

DESCRIPTION
   This function is called by RRC to get number of bytes in RLC Watermark queue.

DEPENDENCIES
  None.

RETURN VALUE
  RLC returns a uint32 which is the number of bytes in the Watermark for this
  (num_bytes_in_wm)+(LI*NUM_OF_SDU)

SIDE EFFECTS
  None.

===========================================================================*/
uint32 tdsrlc_rrc_get_wm_info_req
(
  rlc_lc_id_type lc_id             /* logical channel ID for which length of
                                      WM is needed                          */
);


/*===========================================================================

FUNCTION  RLC_GET_SRB_UL_CIPH_STATUS

DESCRIPTION

  Response to RRC's query for ciphering status.

DEPENDENCIES
  None

RETURN VALUE
  TDSRLC_NO_CIPHER_CFG_PENDING, no ciphering config pending.
  TDSRLC_CIPHER_CFG_PENDING, yes, there is at least one.
  TDSRLC_CIPHER_STATUS_UNSPECIFIED, can not find the LC.
  

SIDE EFFECTS
  None.

===========================================================================*/
tdsrlc_cipher_cfg_pending_e_type tdsrlc_get_srb_ul_ciph_status
(
  rlc_lc_id_type  rlc_id,       /* RLC LC ID for SRB2                     */
  uint32          *vt_s,        /* Current VT(S) of UL SRB2               */
  uint32          *act_sn,      /* Act SN of pendinf config,if there is   */
                                /* something pending                      */ 
  uint32          *wm_size      /* WM size in terms of number of RLC PDUs */
);

/*===========================================================================

FUNCTION RLC_RRC_GET_LATEST_PENDING_HFN_REQ

DESCRIPTION
   This function is called by RRC to get latest pending HFN for indicated 
   logical channel ID.  
   RLC populates the info to the struct tdsrlc_pending_hfn_req_type.
   If there is a pending configuration for UL alone then 
   dl_pending_hfn_present flag will be set to FALSE and vice-versa.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE:  If either dl_pending_hfn_present or ul_pending_hfn_present
                    flag is set to TRUE then function will return SUCCESS.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type tdsrlc_rrc_get_latest_pending_hfn
(
  rlc_lc_id_type rlc_id,                /* queried RLC logical channel ID */
  tdsrlc_pending_hfn_req_type *pending_hfn
);

/*===========================================================================

FUNCTION RLC_RRC_OVERWRITE_HFN

DESCRIPTION
   This function is called by RRC to overwrite the HFN of RLC currently using.

   This function will be called after RRC re-establish RLC to overwrite the 
   new HFN that RRC just comes up before the traffic can be resumed.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE:  Done successful / no such LC or no ciphering at all for
                    this LC.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type tdsrlc_rrc_overwrite_hfn
(
  rlc_lc_id_type rlc_id,                /* queried RLC logical channel ID */
  uint32 hfn,
  tdsrlc_reestablish_e_type direction
);

/*===========================================================================

FUNCTION tdscrlc_ul_perform_suspend_or_stop_functionality

DESCRIPTION
  Suspends or stop one or more uplink unacknowledged or acknowledged mode channels.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdscrlc_ul_perform_suspend_or_stop_functionality
( 
  tdsrlc_ul_stop_suspend_request_type *suspend_stop_ptr,
  tdsrlc_ul_stop_suspend_cnf_type *crlc_suspend_stop_cnf
);

/*===========================================================================

FUNCTION tdscrlc_dl_perform_stop_functionality

DESCRIPTION
  Stops one or more downlink unacknowledged or acknowledged mode channels.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void tdscrlc_dl_perform_stop_functionality
(
  tdsrlc_dl_stop_request_type *rrcsmc_dl_stop_req
);

/*===========================================================================

FUNCTION tdsrlc_rrc_req_to_register_for_data_ind

DESCRIPTION
  Informs RRC about the list of logical channels registered for watermark 
  in Cell_PCH/Ura_PCH states.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlc_rrc_req_to_register_for_data_ind
(
  tdsrlc_ul_register_srvc_type *ul_ptr
);

#ifdef FEATURE_TDSCDMA_DSDS_TUNEAWAY
/*===========================================================================

FUNCTION TDSRLC_CHECK_PENDING_SDU

DESCRIPTION
  Checks if there is any data pending for a given LC, and if there are
  any pending ACKs awaited.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if data is available or ACK is awaited
  otherwise FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean tdsrlc_check_pending_sdu
(
  rlc_lc_id_type lc_id
);

/*===========================================================================

FUNCTION TDSRLC_CHECK_PENDING_DL_SDU

DESCRIPTION
  Checks if there are any PDUs yet to be received or processed
  for a complete SDU.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if more PDUs are expected for a SDU
  FALSE - otherwise 

SIDE EFFECTS
  None.

===========================================================================*/

boolean tdsrlc_check_pending_dl_sdu
(
  rlc_lc_id_type lc_id
);
#endif

#endif //RLCRRCIF_H

