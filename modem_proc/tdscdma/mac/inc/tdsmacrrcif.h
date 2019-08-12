#ifndef TDSMACRRCIF_H
  #define TDSMACRRCIF_H

/*========================================================================================

   MAC - RRC INTERFACE H E A D E R    F I L E

DESCRIPTION
  This file contains the declaration of the interface functions and
  data types used by MAC and RRC
  Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

========================================================================================*/

/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/mac/inc/tdsmacrrcif.h#1 $  $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------------------------
02/02/11   ps       Changes for TDS MAC HSUPA
11/23/10   cyx      Remove tdsmvsif.h to clean up l2 interface
10/24/10   guo      Initial release Upmerged WCDMA to TDS

========================================================================================*/

/*=======================================================================================

                     INCLUDE FILES

========================================================================================*/
#include "tdscdma_variation.h"
#include        "customer.h"
#include        "tdsuecomdef.h"
#include        "tdsl1sapcommon.h"
#include        "tdsl1macdata.h"
#include        "tdsmacl1rrcif.h"


/* --------------------------------- */
/* PUBLIC APIs of tdsmacrrcif.h         */
/* --------------------------------- */

/*========================================================================================

                       CONSTANT TYPE DATA DECLARATIONS

========================================================================================*/
/* Maximum number of CCTrChs */
  #define         TDSMAC_DL_MAX_CCTRCH                       3

/* Maximum number of domains. Domain Index 0(CS) 1(PS) */
  #define         TDSMAX_CN_DOMAIN                           2
#ifdef FEATURE_TDSCDMA_HSUPA

  #define TDSMAC_E_TFCI_MIN_SET_NOT_PRESENT                  0xFF

#endif

/*========================================================================================

                       ENUMERATED TYPE DATA DECLARATIONS

========================================================================================*/
/*--------------------------------------------------------------------------------------*/
/* UL CCTrCh type                                                                                                                                               */
/*--------------------------------------------------------------------------------------*/

typedef enum
{
  TDSINVALID_UL_CCTRCH = 0,  /* Invalid CCTrCh */
  TDSRACH,                   /* RACH type CCTrCh */
  TDSDCH,                     /* TDSDCH type CCTrCh */
  TDSEDCH                   /* E-DCH type CCTrCh */

}tdsmac_ul_cctrch_e_type;

/*--------------------------------------------------------------------------------------*/
/* RNTI type                                                                            */
/*--------------------------------------------------------------------------------------*/
typedef enum
{
  TDSNO_RNTI_VALID,          /* No RNTI is valid */
  TDSCRNTI_VALID,            /* C-RNTI is valid */
  TDSURNTI_VALID,            /* U-RNTI is valid */
  TDSBOTH_VALID              /* U-RNTI and C-RNTI are valid */
} tdsmac_rnti_valid_e_type;

/*--------------------------------------------------------------------------------------*/
/* UL Traffic volume reporting mode                                                     */
/*--------------------------------------------------------------------------------------*/
typedef enum
{
  TDSNO_REPORTING = 0x00,        /* reporting disabled */
  TDSPERIODIC_MODE,              /* Periodic reporting */
  TDSEVENT_TRIGGERED_MODE        /* Event-triggered reporting */
}tdsmac_ul_traffic_report_mode_e_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement quantity type                                             */
/*--------------------------------------------------------------------------------------*/
typedef enum
{
  TDSRLC_BUF_PAYLOAD,           /* MAC is reporting the raw buffer payload to RRC */
  TDSAVERAGE_RLC_BUF_PAYLOAD,   /* Average buffer payload */
  TDSVARIANCE_RLC_BUF_PAYLOAD   /* Variance of rlc buffer payload */
}tdsmac_ul_traffic_meas_qty_e_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume event type                                                            */
/*--------------------------------------------------------------------------------------*/
typedef enum
{
  TDSINVALID_EVENT,
  TDSEVENT_4A,         /* The raw rlc buffer payload exceeds a threshold value */
  TDSEVENT_4B          /* The raw rlc buffer payload is smaller than the threshold value*/
}tdsmac_ul_traffic_event_id_e_type;

/*-------------------------------------------------------------------------------------*/
/* RRC may ask MAC to start, modify or delete a traffic volume measurement for a       */
/* measurement identity                                                                */
/*-------------------------------------------------------------------------------------*/

typedef enum
{
  TDSMAC_TRAFFIC_MEAS_START,      /* Start doing traffic volume measurements     */
  TDSMAC_TRAFFIC_MEAS_MODIFY,     /* Modify an existing measurement              */
  TDSMAC_TRAFFIC_MEAS_DELETE,     /* DElete all measurements associated with the */
  /* measurement identity                        */
  TDSMAC_TRAFFIC_MEAS_DELETE_ALL, /* Deletes all the exisiting measurement ids   */
  TDSMAC_TRAFFIC_MEAS_SAVE,       /* Save the measurement info */
  TDSMAC_TRAFFIC_MEAS_RESUME      /* Resume measurement */
}tdsmac_ul_traffic_cmd_e_type;

/*-------------------------------------------------------------------------------------*/
/*  The traffic volume measurement report could be transferred in either UM or AM mode */
/*-------------------------------------------------------------------------------------*/

typedef enum
{
  TDSACK_MODE,           /* RLC Acknowledged Mode   */
  TDSUNACK_MODE          /* RLC Unacknowledged mode */

}tdsmac_ul_traffic_report_transfer_mode_e_type;

/*-------------------------------------------------------------------------------------*/
/*  Indicates if it is a CS domain or a PS Domain.                                     */
/*-------------------------------------------------------------------------------------*/
typedef enum
{
  TDSCS_DOMAIN,
  TDSPS_DOMAIN
}tdsmac_ul_cipher_domain_e_type;

  #ifdef FEATURE_TDSCDMA_TC_CONSTANT_DELAY

typedef enum
{
  TDSMAC_LOOPBACK_NA,     /* No loopback is required */
  TDSMAC_LOOPBACK_MODE_1, /* loopback mode 1 */
  TDSMAC_LOOPBACK_MODE_2  /* loopback mode 2 */
} tdsmac_loop_back_mode_e_type;

  #endif

typedef enum
{
  TDSINVALID_CFG=0,
  TDSNON_HO_CIPHER_CFG,    /* Normal case */
  TDSHO_STEP1_CIPHER_CFG,  /* Donot increment the HFN even if the activation time is reached */
  TDSHO_STEP2_CIPHER_CFG,  /* Increment the HFN by 1 at activation time. Subsequently the behaviour
                        remains the same as in NO_HO_CIPHER_CFG */
  TDSNON_HO_STEP2_CIPHER_CFG, /*Used for new TM RB being set first time. MAC needs to get the HFN 
                             that needs to be applied at activation time from RRC */
  TDSSMC_INITIATED_NON_HO_CIPHER_CFG
                        /* to be sent by RRC when TM RBs exist at the time when
                              ciphering is enabled by the network. On receipt of this
                              config message MAC should not do ciphering(either with
                              uea0 or uea1) till the activation time expires. At
                              activation time MAC should initialize COUNT-C using
                              HFN value sent in the config message */
}tdsmac_cipher_cfg_e_type;

#if 0
/*--------------------------------------------------------------------------------------
Ciphering algorithms UE_0 and UE_1. When MAC is confirgured with UE_) even if ciphering is
enabled, ciphering should not be performed. UE_1 implies ciphering has to be performed.
---------------------------------------------------------------------------------------*/
typedef enum
{
  TDSMAC_CIPHER_ALGO_UE_0 = 0,
  TDSMAC_CIPHER_ALGO_UE_1

}tdsmac_cipher_algo_e_type;
#endif


typedef enum
{
  TDSMIN_ALLOWED_TFCI=0,
  TDSALLOWED_TFCI,
  TDSNOT_ALLOWED_TFCI,
  TDSRESTRICTED_TFCI,
  TDSFULL_TFCI
}tdsmac_tfc_subset_restriction_choice_e_type;



/* HSDPA Action Variable */
typedef enum
{
  TDSHSDPA_NOOP,
  TDSHSDPA_START,
  TDSHSDPA_RECONFIG,
  TDSHSDPA_STOP
}tdshsdpa_action_enum_type;


#ifdef FEATURE_TDSCDMA_HSUPA

/* HSUPA Action Variable */
typedef enum
{
  TDSMAC_EUL_NOOP = 0x0,
  TDSMAC_EUL_START = 0x1,
  TDSMAC_EUL_CONFIG = 0x2, /* EUL RE-CONFIG */
  TDSMAC_EUL_STOP = 0x4
} tdsmac_eul_action_e_type;

/* HSUPA Action Variable */
typedef enum
{
  TDSMAC_E_TSN_ACTION_BACKUP = 0x1,
  TDSMAC_E_TSN_ACTION_REVERT = 0x2
} tdsmac_e_tsn_action_e_type;

typedef enum
{
  TDSMAC_E_TX_GRANT_NON_SCHEDULED,
  TDSMAC_E_TX_GRANT_SCHEDULED,
  TDSMAC_E_TX_GRANT_NOT_DEFINED = 0xFF
} tdsmac_e_tx_grant_info_e_type;

#endif /* FEATURE_TDSCDMA_HSUPA */

/*--------------------------------------------------------------------------------------*/
/* Enum to differentiate when a fully copy of the parameters are neeeded.               */
/*--------------------------------------------------------------------------------------*/
typedef enum
{

  /* MAC cfg not changed */
  TDSMAC_CFG_NOT_CHANGED = 0,

  /* Mac config changed */
  TDSMAC_CFG_CHANGED,

  /* MAC config changed, only copy the loopback information
  nothing else has changed. */
  TDSMAC_LB_CFG_CHANGED,

  TDSMAC_LB_AND_UL_CFG_CHANGED,

  /* MAC config only need to update URNTI */
  TDSMAC_CFG_UPDATE_URNTI_ONLY,

  /* MAC config only need to update cipher config */
  TDSMAC_CFG_UPDATE_CIPH_CFG_ONLY

} tdsmac_cfg_changed_status_e_type;


/*========================================================================================

                       STRUCTURE TYPE DATA DEFINITIONS

========================================================================================*/



/*--------------------------------------------------------------------------------------*/
/* HSDPA HS PRIORITY QUEUE Information                                                         */
/*--------------------------------------------------------------------------------------*/


typedef struct
{
  /* PDU Size in MAC HS Header */
  uint16 mac_pdu_size;  /* 1..5000 */

  /* PDU size index in MAC HS header */
  uint8 mac_pdu_index; /* 0..7 */

} tdsmac_hs_pdu_info_struct_type;

typedef struct
{
  /* Queue Id */
  uint8 queue_id;

  /* No of PDU sizes */
  uint8 no_of_pdu_sizes;

  /* MAC PDU size type information for this queue */
  tdsmac_hs_pdu_info_struct_type pdu_info[UE_MAX_HS_NO_OF_PDU_SIZE_PER_QUEUE];

  /* Reordering release timer in msecs */
  uint16 release_timer;

  /* Receiver Window Size */
  uint8 window_size;

} tdsmac_hs_queue_struct_type;




/*--------------------------------------------------------------------------------------*/
/* MAC Status indication                                                                                                                                */
/*--------------------------------------------------------------------------------------*/
typedef enum
{

  /* Successful transmission on TDSRACH */
  TDSRACH_TX_SUCCESS,

  /* Max number of RACH cycles exccedded */
  TDSMAX_RACH_CYCLES_REACHED,

  /* Transmission on RACH is aborted */
  TDSRACH_TX_ABORTED

#ifdef FEATURE_TDSCDMA_HSUPA
  /* Max number of E-RUCCH sync cycles exceeded Mmax reached  */
  ,TDSMAX_ERUCCH_CYCLES_REACHED,
  /* SI on E-RUCCH tried N_RUCCH times, and Hysteresis timer also expired, but no grant received  */
  TDSERUCCH_HYST_TMR_EXP,
#endif

} tdsmac_status_ind_e_type;

/*--------------------------------------------------------------------------------------*/
/* RNTI information                                                                                                                                             */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* RNTI validity flag */
  tdsmac_rnti_valid_e_type   rnti_valid;

  /* 32-bit U-RNTI value assigned by U-TRAN */
  /* Valid only if "rnti_valid = TDSURNTI_VALID or BOTH_VALID" */
  uint32      urnti;

  /* 16-bit C-RNTI value assigned by serving cell */
  /* Valid only if "rnti_valid = CRNTI_VALID or BOTH_VALID" */
  uint16      crnti;

} tdsmac_rnti_config_type;

/*--------------------------------------------------------------------------------------*/
/* Ciphering parameters                                                                 */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  tdsmac_cipher_cfg_e_type cipher_cfg_type;

  /* The ciphering algorithm can be either UE_0,  UE_1 or UE_2*/
  uecomdef_umts_cipher_algo_e_type cipher_algo;

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


} tdsmac_cipher_config_type;

/*--------------------------------------------------------------------------------------*/
/* The structure that needs to be filled up when RRC requests MAC for the START values  */
/* MAC sends HFN per domain                                                             */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* hfn : 20-bit Hyper Frame Number (HFN) */
  uint32    cs_hfn;
  uint32    ps_hfn;

} tdsmac_hfn_info_type;


/*--------------------------------------------------------------------------------------*/
/* Access Service Class Parameters                                                      */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Persistence value of the partition */
  uint32              pval;

}tdsmac_ul_asc_type;

/*--------------------------------------------------------------------------------------*/
/* RACH Transmit Control Parameters                                                     */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Maximum number of RACH preamble ramping cycles */
  uint8       mmax;

  /* Number of available access service classes */
  uint8       nASC;

  /* Parameters of each valid Access Service Class */
  tdsmac_ul_asc_type asc[MAX_ASC];

}tdsmac_ul_rach_txcntl_config_type;
#ifdef FEATURE_TDSCDMA_UPDATE_SIB7_IN_FACH
/*--------------------------------------------------------------------------------------*/
/* RRC reads SIB7 continuously in CELL FACH state,then updates Persistence value to MAC */
/*--------------------------------------------------------------------------------------*/
typedef struct
{
  /* Number of available access service classes */
  uint8               nASC;
 
  /* Parameters of each valid Access Service Class */
  tdsmac_ul_asc_type  asc[MAX_ASC];
  
}tdsmac_ul_persistence_config_type;
#endif
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
  uint16                      lc_tfi_list[TDSUE_MAX_TF];

}tdsmac_ul_rlc_size_info_type;

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
  tdsmac_ul_rlc_size_info_type       rlc_size_info;


}tdsmac_ul_ded_logch_config_type;

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
#ifdef FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA
  boolean opt_timer_enabled;
#endif

  /* Logical channel type (DCCH or DTCH) or MSCH or MTCH or MCCH*/
  uecomdef_logchan_e_type         chan_type;

  /* RLC mode (TM or UM or AM) */
  uecomdef_logch_mode_e_type      rlc_mode;


}tdsmac_dl_ded_logch_config_type;

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
  tdsmac_hs_queue_struct_type queue_info[UE_MAX_HS_QUEUE];

  /* Number of dedicated RLC logical channels mapped */
  uint8       ndlchan;

  /* Info of dedicated logical channels mapped */
  tdsmac_dl_ded_logch_config_type dlchan_info[UE_MAX_LOGCHAN_PER_DFLOW];

} tdsmac_dflow_struct_type;

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
  tdsmac_ul_ded_logch_config_type    dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];


  /* 0(False) indicates it is CS domain, 1(TRUE) indicates it is PS domain */
  tdsmac_ul_cipher_domain_e_type     cs_or_ps_domain;

}tdsmac_ul_dch_trch_config_type;

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
  tdsmac_dl_ded_logch_config_type  dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];


}tdsmac_dl_fach_trch_config_type;

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
  tdsmac_dl_ded_logch_config_type dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];

  /* 0(False) indicates it is CS domain, 1(TRUE) indicates it is PS domain */
  tdsmac_ul_cipher_domain_e_type   cs_or_ps_domain;

  #ifdef FEATURE_TDSCDMA_TC_CONSTANT_DELAY
  tdsmac_loop_back_mode_e_type loopback_test_mode;
  #endif

}tdsmac_dl_dch_trch_config_type;
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
  tdsmac_dl_ded_logch_config_type        dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];

}tdsmac_dl_dsch_trch_config_type;


/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of uplink dedicated CCTrCh                                  */
/*--------------------------------------------------------------------------------------*/
typedef struct
{

  /* Number of dedicated transport channels */
  uint8       ndchs;

  /* Configuration info of each dedicated transport channel */
  tdsmac_ul_dch_trch_config_type dch_info[TDSUE_MAX_TRCH];

  #ifdef FEATURE_DUMMY_SIGNALING
  #error code not present
#endif

  #ifdef FEATURE_TDSCDMA_TC_CONSTANT_DELAY
  tdsmac_loop_back_mode_e_type loopback_test_mode;
  #endif

}tdsmac_ul_dch_cctrch_config_type;

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

  #ifdef FEATURE_TDSCDMA_TC_CONSTANT_DELAY
  tdsmac_loop_back_mode_e_type loopback_test_mode;
  #endif

  /* Info of each dedicated logical channel mapped */
  tdsmac_ul_ded_logch_config_type    dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];


}tdsmac_ul_rach_cctrch_config_type;

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

}tdsmac_dl_bch_cctrch_config_type;

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

}tdsmac_dl_pch_cctrch_config_type;


/*--------------------------------------------------------------------------------------*/
/* Configuration Parameters of uplink CCTrCh                                            */
/*--------------------------------------------------------------------------------------*/

typedef struct
{

  /* CCTrCh type (RACH or DCH) */
  tdsmac_ul_cctrch_e_type        cctrch_type;

  /* Random Access CCTrCh configuration */
  tdsmac_ul_rach_cctrch_config_type      rach_info;

  /* Uplink Dedicated CCTrCh configuration */
  tdsmac_ul_dch_cctrch_config_type       dch_info;

}tdsmac_ul_cctrch_config_type;
#ifdef FEATURE_TDSCDMA_HSUPA

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
} tdsmac_e_scheduling_info_config_s_type;

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

} tdsmac_e_scheduling_tx_config_s_type;

/* zhaoweijia mac wll del begin */
typedef struct
{
  /* Max MAC-e PDU contents size: max number of non scheduled data
     in bits that can be part of MAC-e PDU */
  uint16                          max_mac_e_pdu_size_in_bits; /* 1..19982 */

  /* MAC-d PDU's are only allowed to be transmitted in those processes
     for which the bit is set to "1". Bit 0 corresponds to HARQ process
     0, bit 1 corresponds to HARQ process 1. This is valid for 2ms TTI only.
     Bit 0 is Most Significant Bit(Left Most bit) of the BITSTRING */
  uint8                           non_sched_tx_grant_harq_allocation_2ms; /* BITSTRING */

} tdsmac_non_sched_tx_grant_info_s_type;
/* zhaoweijia mac wll del end */

typedef struct
{
  /* Since there is no header for MAC-D pdus in EUL, so rlc_pdu_size is
     same as mac_d_pdu_size */
  uint16                          rlc_pdu_size_in_bits; /* 16..5000 (size in bits) */

  /* Data Description Identifier (6 bits), it identifies the logical channel,
     MAC-d flow and size of the MAC-d PDUs concatenated into
     the associated MAC-es PDU */
  uint8                           ddi; /* 0..62 (63 is reserved) */

} tdsmac_e_rlc_size_ddi_info_s_type;

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

  /* number of valid fields in rlc_size_ddi_info[] */
  uint8                              num_rlc_size_ddi_info; /* 0..32, 0 is valid only when action is NOOP or STOP */

  /* Mapping of rlc-pdu-size  to ddi. Valid number of fields in following array is
     equal to 'num_rlc_size_ddi_info' */
  tdsmac_e_rlc_size_ddi_info_s_type     rlc_size_ddi_info[MAX_RLC_PDU_SIZE_PER_LOGCHAN];

} tdsmac_e_ul_logch_info_s_type ;

typedef struct
{

  /* E-DCH MAC-d flow identified */
  uint8                                  e_mac_d_flow_id; /* 0..7 */

  /* E-DCH MAC-d flow power offset */
  uint8                                  e_mac_d_flow_power_offset; /* 0..6 (unit in dB) */

  /* E-DCH MAC-d flow maximum number of  retransmissions */
  uint8                                  e_mac_d_flow_max_num_of_retx; /* 0..15 */


  /* E-DCH MAC-d flow retransmission timer (units in milli second) */
  uint16                                  e_mac_d_flow_retx_tmr; /*10.. 560 ms*/

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
  tdsmac_e_tx_grant_info_e_type             e_tx_grant_info;

  /* zhaoweijia mac wll del begin */
  /* below field is valid only if tdsmac_e_tx_grant_info_e_type
     indicates NON_SCHEDULED */
  tdsmac_non_sched_tx_grant_info_s_type    non_sched_tx_grant;
  /* zhaoweijia mac wll del end */
  /* number of logical channels */
  uint8                                  num_logch; /* 0..15 */

  /* number of fields valid in following array is equal to 'num_logch' */
  tdsmac_e_ul_logch_info_s_type             e_ul_logch_info[MAX_DED_LOGCHAN_PER_TRCH];

} tdsmac_e_mac_d_flow_info_s_type;





typedef struct
{
  /* Maximum number of retransmissions for Scheduling Info.
  Only used when no MAC-d PDUs are included in the same MAC-e PDU.
  */
  uint8                                  e_max_num_of_retx_for_si_only; /* 0..15 */
  
  /* Retransmission timer for SI  Only used when no MAC-d PDUs
  are included in the same MAC-e PDU.  In ms  */
  uint16                                  e_retx_tmr_for_si_only; /*10.. 560 ms*/
  
  /*T-SI-NST timer. Used sending periodic SI for non-scheduled info, 
  if only non-scheduled flow is configured. */
  
  uint8 t_non_Sched_periodic_si_tmr;
  
  /*Periodic timer for SI when scheduled flow is configured.
  Value in ms.Default 40 ms */
  uint8 t_Sched_periodic_si_tmr;
  
  /*Values in 5ms TTI.
  Indicates how many next consecutive 5ms TTIs the UE should estimate whether there is a Grant valid before Sending Scheduling Infomation via ERUCCH.
  If not present, then an invalid value of 0xFF is set*/
  uint8 ext_est_window;
  
  /* Values in ms.Default = 40ms
  The delay timer T_WAIT shall be started once the Grant expires and shall be stopped and reset when a Grant is received. If T-WAIT expires then UE should
  Send scheduling information on E-RUCCH.*/
  
  uint16 t_wait_tmr;
  /*Indicates if there is a serving cell or working freq changes, so that MAC trigger SI.*/
  boolean srv_cell_or_work_freq_change;
  
  /*Indicates if ERTNI is changed. SO that MAC can restart E-RUCCh procedure*/
  boolean ernti_change;

  
}tdsmac_eul_si_info_s_type;

typedef struct
{
  /* Default = 120 ms  . Value in ms */
  uint16 t_rucch_tmr;
  
          /* Default = 3.Range 0..7
  Maximum number of retransmissions
  of scheduling information on ERUCCH */
  uint8 n_rucch;
  
  /* 1.. 32 The max number of synchronisation attempts allowed */
  uint8 mmax;
  
  /* Number of available access service classes */
  uint8       nASC;
  
  /* Parameters of each valid Access Service Class */
  tdsmac_ul_asc_type asc[MAX_ASC];
  
  /*ERNTI value. MAC will use this for building E-RUCCH data*/
  uint16 ernti;

  
}tdsmac_eul_erucch_info_s_type  ;


typedef struct
{
  
  /* Number of E-DCH MAC-d flows in UE */
  uint8                           num_e_mac_d_flow; /* 0..8, 0 will be set ONLY when action is NOOP */

  /* E-DCH MAC-d flow */
  tdsmac_e_mac_d_flow_info_s_type    e_mac_d_flow_info[TDSMAX_E_DCH_MAC_D_FLOW];
 

  tdsmac_eul_erucch_info_s_type    e_rucch_info;

  tdsmac_eul_si_info_s_type        eul_si_info;


} tdsmac_e_config_s_type;

#endif /* FEATURE_TDSCDMA_HSUPA */

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
  TDSMAC_CFG_CHANGED indicates that MAC config parameters have changed and we need to configure
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
  tdsmac_cfg_changed_status_e_type       mac_config_changed;

  /* Pointer to UL CCTrCh configuration data */
  /* Uplink CCTrCh mapping information resides in shared memory */
  /* It can be accessed by RRC and MAC */
  tdsmac_ul_cctrch_config_type           *cctrch_ptr;

  /* Pointer to UL Transport Format Combination Set information */
  /* Uplink TFCS info resides in shared memory */
  /* It can be accessed by RRC and MAC */
  tdsmac_ul_tfcs_type                    *tfcs_ptr;

  /* RNTI information */
  /* MAC copies this info to local memory */
  tdsmac_rnti_config_type                rnti_info;

#ifdef FEATURE_TDSCDMA_HSUPA

  /* set to MAC_EUL_REVERT_TO_OLD_TSN whenever UE has to reverting back to old
     config. MAC-E should process this information first(before mac-e-reset
     indicator).
     If it is set to MAC_EUL_REVERT_TO_OLD_TSN then MAC-E is expected to
     revert to the old TSN information which was stored on receipt of last
     TDSCMAC_UL_CONFIG_REQ.
     If it is set to MAC_EUL_BACKUP_TSN then back up of
     current config(like TSN) is needed. */
  tdsmac_e_tsn_action_e_type   mac_e_tsn_action;

  /* When this variable is set to TRUE, then all the HARQ processes
  will be flushed in Firmware. MAC will use this variable to flush all
  HARQ process related information that is maintained internally.
  Thsi information needs to be flushed so that happy bit is computed
  correctly in MAC*/
  boolean                             mac_e_reset_indicator;

  tdsmac_eul_action_e_type               mac_eul_action;

  /* MAC E-DCH config info - MAC should look into this pointer only if
     eul_action is MAC_EUL_CONFIG */
  tdsmac_e_config_s_type                 *mac_e_config_ptr;

#endif

  /* Ciphering control information */
  /* MAC copies this info to local memory Domain index CS(0), PS(1)*/
  tdsmac_cipher_config_type              cipher_info[TDSMAX_CN_DOMAIN];

  /* RACH Transmit Control Parameters */
  /* MAC copies this info to local memory */
  tdsmac_ul_rach_txcntl_config_type      rach_tx_cntl_info;

/*If this flag indicates that new TFCS\ TFCSubset is not present:
  MAC will continue with the old configuration. MAC will ignore
  TFCS table reconfig but apply other LC reconfig Else
  MAC will apply the new configuration present in OC.*/
  boolean   new_tfcs_list_present;


}tdsmac_ul_config_type;

/************* MAC RRC Changes ***********************************************/
#if defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS)

typedef enum 
{
  TDSMAC_HS_INVALID_TRANSITION,
  TDSMAC_HS_TO_EHS,
  TDSMAC_EHS_TO_HS,
  TDSMAC_HS_NO_TRANSITION
}tdsmac_hs_transition_e_type;

/* HSDPA Action Variable */
typedef enum
{
  TDSENH_HSDPA_NOOP,
  TDSENH_HSDPA_START,
  TDSENH_HSDPA_RECONFIG,
  TDSENH_HSDPA_STOP
}tdsenh_hsdpa_action_enum_type;

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
  
} tdsmac_ehs_queue_struct_type;

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
#ifdef FEATURE_TDSCDMA_CS_VOICE_OVER_HSPA
  boolean opt_timer_enabled;
#endif

  /* Logical channel type (DCCH or DTCH) */
  uecomdef_logchan_e_type         chan_type;

  /* RLC mode (TM or UM or AM) */
  uecomdef_logch_mode_e_type      rlc_mode;

}tdsmac_dl_ehs_logch_config_type;

#endif /* ((defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS )) */

/*--------------------------------------------------------------------------------------*/
/* Downlink MAC Configuration Parameters                                                */
/*--------------------------------------------------------------------------------------*/
typedef struct
{


  /* TRUE indicates that there is an activation time involved with this configuration
  and MAC needs to wait for L1 to indicates us to switch over to the new config. FALSE
  indicates switch to the new configuration that RRC sends us immediately */
  boolean                                   wait_for_l1_sig;

  /* Number of BCH type CCTrChs : Valid range is 0 - MAC_DL_MAX
  _CCTRCH */
  uint8                             num_bchs;

  /* Broadcast CCTrCh configuration */
  tdsmac_dl_bch_cctrch_config_type       bch_info[TDSMAC_DL_MAX_CCTRCH];

  /* Number of PCH/FACH type CCTrChs : Valid range is 0 - TDSMAC_DL_MAX_CCTRCH */
  uint8   num_pchs;

  /* PCH CCTrCh information */
  tdsmac_dl_pch_cctrch_config_type   pch_info[TDSMAC_DL_MAX_CCTRCH];

  /* Number of FACH type TrChs : Valid range is 0 - TDSUE_MAX_TRCH */
  uint8   num_fachs;

  /* FACH TrCh configuration */
  tdsmac_dl_fach_trch_config_type    *fach_info[TDSUE_MAX_TRCH];

  /* Number of DCH type TrChs : Valid range is 0 - TDSUE_MAX_TRCH */
  uint8   num_dchs;

  /* Downlink Dedicated CCTrCh configuration */
  tdsmac_dl_dch_trch_config_type     *dch_info[TDSUE_MAX_TRCH];


  tdshsdpa_action_enum_type mac_hsdpa_action;

  boolean mac_hs_reset_indicator;
  boolean mac_hs_restore_tsns;

#if defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS)
  /*Informs MAC if MAC HS is configured or MAC EHS */
  mac_hs_e_type mac_hs_or_ehs;

  /* MAC EHS Action */
  tdsenh_hsdpa_action_enum_type mac_enh_hsdpa_action;

  /*Informs transition between HS and EHS, if present */
  tdsmac_hs_transition_e_type mac_hs_transition_type;

 /* Number of dedicated RLC logical channels mapped */
  uint8       ndlchan_macehs;

  /* Info of dedicated logical channels mapped */
  tdsmac_dl_ehs_logch_config_type * dl_macehs_logchan_info[TDSUE_MAX_DL_LOGICAL_CHANNEL];
  
  uint8 num_mac_ehs_queue;
  
  tdsmac_ehs_queue_struct_type *hsdpa_ehs_queue_info[UE_MAX_HS_QUEUE];
  
#endif /* FEATURE_TDSCDMA_HSDPA_MAC_EHS   */

  /* Number of MAC-d Flows. */
  uint8 num_dflow;

  /* HSDPA MAC-d Flow Structure */
  tdsmac_dflow_struct_type *hsdpa_info[UE_MAX_MAC_D_FLOW];



  /* Number of DSCH type TrChs : Valid range is 0 - TDSUE_MAX_TRCH */
  uint8   num_dschs;

  /* Downlink Shared CCTrCh information */
  tdsmac_dl_dsch_trch_config_type        *dsch_info[TDSUE_MAX_TRCH];

  /* RNTI information */
  /* MAC copies this info to local memory */
  tdsmac_rnti_config_type                rnti_info;

  /* Ciphering control info */
  /* MAC copies this info to local memory */
  tdsmac_cipher_config_type              cipher_info[TDSMAX_CN_DOMAIN];



}tdsmac_dl_config_type;

/*--------------------------------------------------------------------------------------*/
/* Transport channel id and type information                                            */
/*--------------------------------------------------------------------------------------*/
typedef struct
{
  /* Transport channel id : Range (1...32) */
  tr_ch_id_type       trch_id;

  /* Transport channel type */
  tdsmac_ul_cctrch_e_type trch_type;

}tdsmac_ul_traffic_trch_info_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement object type                                               */
/*--------------------------------------------------------------------------------------*/
typedef struct
{
  /* Number of transport channels */
  uint16 num_trch;

  /* Transport channel id and type information for each of these transport channels */
  tdsmac_ul_traffic_trch_info_type  trch_info[TDSUE_MAX_TRCH];

}tdsmac_ul_traffic_meas_obj_type;

/*--------------------------------------------------------------------------------------*/
/* MAC could be asked to measure the raw, average, or variance of rlc payload. In the   */
/* case of average and variance the time interval to take the average & variance is     */
/* given                                                                                */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Time interval to take the average or variance */
  uint32 meas_interval;

  /* One of raw, average or variance */
  tdsmac_ul_traffic_meas_qty_e_type  meas_qty_enum;

}tdsmac_ul_traffic_meas_qty_type;

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

}tdsmac_ul_traffic_meas_report_qty_type;


/*--------------------------------------------------------------------------------------*/
/* Parameters in the case of event trigger specific to a transport channel              */
/*--------------------------------------------------------------------------------------*/

typedef struct
{

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

  /* the event could be either event_4a or event_4b */
  tdsmac_ul_traffic_event_id_e_type  event_id;

}tdsmac_ul_traffic_trch_event_trigger_param;

/*--------------------------------------------------------------------------------------*/
/* The event trigger parameters for each transport channel                              */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Number of events */
  uint16 num_events;

  /* One for event 4A and other for event 4B */
  tdsmac_ul_traffic_trch_event_trigger_param event_param[2];

  /* The channel type and id of each transport channel */
  tdsmac_ul_traffic_trch_info_type  trch_info;

}tdsmac_ul_traffic_trch_event_trigger_cfg;

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
  tdsmac_ul_traffic_trch_event_trigger_cfg   trch_event_cfg[TDSUE_MAX_TRCH];

}tdsmac_ul_traffic_event_trigger_cfg_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume periodic trigger related parameters                                   */
/*--------------------------------------------------------------------------------------*/

typedef struct
{

  /* The number of reports that can be sent */
  uint16    num_reports;

  /* Indicates in milliseconds the interval of periodic report */
  uint16    report_interval;

  /* True indicates there is no limit on the number of reports that we send to RRC    */
  /* False indicates,send only the number of reports that are specified in num_reports*/
  boolean   report_infinity;

}tdsmac_ul_traffic_periodic_report_cfg_type;


/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement report criteria type                                      */
/*--------------------------------------------------------------------------------------*/

typedef union
{
  /* Contains the event trigger related config information for all transport channels */
  /* and for event_4a, event_4b */
  tdsmac_ul_traffic_event_trigger_cfg_type   event_cfg;

  /* Contains all the config information in the case of periodic trigger */
  tdsmac_ul_traffic_periodic_report_cfg_type periodic_cfg;

}tdsmac_ul_traffic_meas_report_criteria_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume report mode type                                                      */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Whether UM or AM is used for transfer */
  tdsmac_ul_traffic_report_transfer_mode_e_type  transfer_mode;

  /* Indicates if it was periodical reporting or event trigger */
  tdsmac_ul_traffic_report_mode_e_type report_mode;

}tdsmac_ul_traffic_report_mode_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic measurement config information sent by RRC                                   */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Measurment identity for this traffic volume report.: Valid range 1..16 */
  uint16       meas_id;

  /* This number is used when MAC posts the config confirmation message to RRC */
  uint16       config_ack_num;


  /* True Indicates that RRC is expecting a confirmation message from MAC    */
  /* False indicates that MAC does not need to send the confirmation message */
  boolean     config_conf_ind;

  /* TRUE indicates Start making measurements as soon as the setup cmd is recvd
  FALSE : save the parameters and wait for a resume cmd to start any measurements
  for this ID . Valid for START cmd only */
  boolean     meas_start_incl;

  /* True Indicates measurement object information is included  */
  /* False indicates apply the measurement reporting criteria to all uplink */
  /* transport channels */
  boolean     meas_obj_incl;

  /* True indicates measurement quantity information is included */
  boolean     meas_qty_incl;

  /* True indicates if the measurement reporting information is included or not */
  boolean   meas_report_incl;

  /* In the case of MODIFY, True will indicate if the periodic or the event triggered criteria*/
  /* is being modified */
  boolean                               meas_criteria_incl;
  /* RRC could ask MAC to start, modify or delete a measurement */
  tdsmac_ul_traffic_cmd_e_type   meas_cmd;
  /* Contains the event trigger, periodic trigger related additional parameters */
  tdsmac_ul_traffic_meas_report_criteria_type  meas_report_criteria;

  /* The objects for which measurement is requested */
  tdsmac_ul_traffic_meas_obj_type  meas_obj_list;

  /* Quantities that MAC shall report to RRC  */
  tdsmac_ul_traffic_meas_report_qty_type   meas_report_qty;

  /* Contains the measurement transfer mode and indication of periodic, */
  /*  event trigger, no trigger  */
  tdsmac_ul_traffic_report_mode_type       meas_report_mode;

  /* The quantity that MAC shall measure */
  tdsmac_ul_traffic_meas_qty_type  meas_qty;

}tdsmac_ul_traffic_meas_config_type;

/*--------------------------------------------------------------------------------------*/
/* RRC to MAC config command for TFCC info. CMAC_UL_TFCC_SUBSET_CFG_REQ                 */
/*--------------------------------------------------------------------------------------*/

typedef struct
{

  /* Number of transport formats */
  uint16                          ntf;

  tr_ch_id_type           trch_id;

  /* Each known Transport Format Combination */
  uint8                       tfi[TDSUE_MAX_TF];


  tdsmac_ul_cctrch_e_type    trch_type;


}tdsmac_tfc_subset_restriction_trch_info_type;

typedef struct
{
  uint8                                         ntrch;

  tdsmac_tfc_subset_restriction_trch_info_type     trch_restriction_info[TDSUE_MAX_TRCH];

}tdsmac_tfc_subset_restriction_trch_list_type;

typedef struct
{
  /* Number of TFC's that are allowed/not_allowed based on the choice */
  uint16                              ntfc;

  /* List of TFCI's */
  uint16                              tfci_list[TDSUE_MAX_TFC];

}tdsmac_tfc_subset_restriction_tfci_info_type;

typedef union
{
  uint16                                tfci;

  tdsmac_tfc_subset_restriction_tfci_info_type   tfci_restriction_info;

  tdsmac_tfc_subset_restriction_trch_list_type   trch_restriction_info_list;

}tdsmac_tfc_subset_restriction_u_type;

typedef struct
{

  /* Activation time CFN at which this config has to be applied */
  uint16                              activation_time;

  /* Time for which this config is valid */
  uint16                              control_duration;

  /* Transaction id of the message that RRC recvd for the TFCC message */
  uint8                               transaction_id;

  rb_id_type                          rb_id;

  /* Choice of the restriction */
  tdsmac_tfc_subset_restriction_choice_e_type  tfci_restriction_choice;

  tdsmac_tfc_subset_restriction_u_type         restriction_info;

}tdsmac_tfc_subset_restriction_info_type;



/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*========================================================================================

                       CONSTANT TYPE DATA DECLARATIONS

========================================================================================*/

  #define         TDSMAX_NUM_EVENTS                          2


/*========================================================================================

                       ENUMERATED TYPE DATA DECLARATIONS

========================================================================================*/



/* Indicates what kind of a call it is. CS only, PS only or Multirab call */
typedef enum
{
  TDSNO_RABS,
  TDSCS_RABS,
  TDSPS_RABS,
  TDSCS_PLUS_PS_RABS
}tdsmac_rab_status_e_type;




/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement results for a single radio bearer                         */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Radio bearer identity */
  uint16  rb_id;

  /* Raw rlc buffer payload value */
  uint32  raw_rlc_payload;

  /* average rlc buffer payload value */
  uint32  average_rlc_payload;

  /* variance of rlc buffer payload */
  uint32  variance_rlc_payload;

}tdsmac_ul_traffic_rb_meas_ind_type;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement event results                                             */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Uplink transport channel type causing the event */
  tdsmac_ul_traffic_trch_info_type           trch_info;

  /* Event identity whether it was underflow or overflow */
  tdsmac_ul_traffic_event_id_e_type  event_id;

}tdsmac_ul_traffic_event_result;

/*--------------------------------------------------------------------------------------*/
/* Traffic volume measurement results for all the radio bearers                         */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Measurement identity */
  uint16   meas_id;

  /* Number of Radio bearers */
  uint16   num_rbs;

  /* True indicates that MAC has sent to RRC the maximum number of reports that */
  /* MAC was configured to send for this measurement ID. This is in the case of */
  /* periodic mode reporting criteria                                           */
  boolean num_reports_ind;

  /* Indicates if it is a periodic or event trigger */
  tdsmac_ul_traffic_report_mode_e_type report_mode;

  /* traffic volume measurements of all the radio bearers multiplexed onto the  */
  /* indicated transport channels */
  tdsmac_ul_traffic_rb_meas_ind_type  rb_meas[TDS_MAX_RB];

  /* Event results */
  tdsmac_ul_traffic_event_result  event_result;

  /* Indicates the report transfer mode (UM or AM ) */
  tdsmac_ul_traffic_report_transfer_mode_e_type  report_transfer_mode;

}tdsmac_ul_traffic_meas_ind_type;

/*--------------------------------------------------------------------------------------*/
/* MAC to RRC acknowledgement command. TDSRRC_CMAC_MEAS_CTRL_CNF                           */
/*--------------------------------------------------------------------------------------*/

typedef struct
{
  /* Given in Meas Req command to MAC as config_ack_num;*/
  uint16 config_ack_num;

  /* measurement identity */
  uint16 meas_id;

  /* True indicates that the command was accepted */
  boolean status;

}tdsmac_meas_ctrl_cnf_type;

/*--------------------------------------------------------------------------------------*/
/* RRC to MAC config command for TFCC info. CMAC_UL_TFCC_SUBSET_CFG_REQ                 */
/*--------------------------------------------------------------------------------------*/


typedef struct
{
  uint8             transaction_id;

  rb_id_type        rb_id;
/* Indicates if the validation at MAC was succesful or not*/
  boolean status;
}tdsmac_tfc_subset_cfg_cnf_type;


typedef enum
{
  TDSRRC_NO_CODEC = 0,
  TDSRRC_NB_AMR_CODEC,
  TDSRRC_WB_AMR_CODEC
}tdsrrc_event_ul_amr_codec_type;

typedef enum
 {

  TDSMAC_AMR_MODE_0475,    /* 0 -- 4.75 kbit /s */ 

  TDSMAC_AMR_MODE_0515,    /*1 --  5.15 kbit /s */

  TDSMAC_AMR_MODE_0590,    /* 2 --  5.90 kbit /s */

  TDSMAC_AMR_MODE_0670,    /* 3 --  6.70 kbit /s */

  TDSMAC_AMR_MODE_0740,    /* 4 --  7.40 kbit /s */

  TDSMAC_AMR_MODE_0795,    /* 5 --  7.95 kbit /s */

  TDSMAC_AMR_MODE_1020,    /* 6 --  10.2 kbit /s */

  TDSMAC_AMR_MODE_1220,    /* 7 --  12.2 kbit /s */

  TDSMAC_AMR_MODE_0660,    /* 8 --  AMR-WB 6.60 kbps */

  TDSMAC_AMR_MODE_0885,    /* 9 --   AMR-WB 8.85 kbps */

  TDSMAC_AMR_MODE_1265,    /* 10 --  AMR-WB 12.65 kbps */

  TDSMAC_AMR_MODE_1425,    /* 11 --  AMR-WB 14.25 kbps */

  TDSMAC_AMR_MODE_1585,    /* 12 --  AMR-WB 15.85 kbps */

  TDSMAC_AMR_MODE_1825,    /* 13 --  AMR-WB 18.25 kbps */

  TDSMAC_AMR_MODE_1985,    /* 14 --  AMR-WB 19.85 kbps */

  TDSMAC_AMR_MODE_2305,    /* 15 --  AMR-WB 23.05 kbps */

  TDSMAC_AMR_MODE_2385,    /* 16 --  AMR-WB 23.85 kbps */

  TDSMAC_AMR_MODE_UNDEF =0xFF /* 0xFF --  undefined */

}tdsrrc_event_ul_amr_rate_type;

/*Event structure EVENT_WCDMA_UL_AMR_RATE:*/
typedef PACKED struct
{
  uint8 codec_type;
  uint8 num_rates;
  tdsrrc_event_ul_amr_rate_type amr_rate[9];
}tds_ul_amr_rate_event_type;

typedef enum
{
  TDSCDMA_TRCH_INVALID,
  TDSCDMA_TRCH_BCH, 
  TDSCDMA_TRCH_PCH,
  TDSCDMA_TRCH_FACH,
  TDSCDMA_TRCH_DCH, 
  TDSCDMA_TRCH_HSDSCH, 
  TDSCDMA_TRCH_RACH, 
  TDSCDMA_TRCH_EDCH
}log_tdscdma_trch_type;
/*-------------------------------------------------------------------
FUNCTION tdsmac_get_additional_traffic_vol_msmt_results

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
boolean
tdsmac_get_additional_traffic_vol_msmt_results(uint16 tdsmeas_id, tdsmac_ul_traffic_meas_ind_type *);

/*-------------------------------------------------------------------
FUNCTION tdsmac_rrc_get_hfn_info

DESCRIPTION
 Function interface which will provide RRC with the information of CS and PS HFN
 numbers during the time of RRC connection release
DEPENDENCIES
  None

RETURN VALUE
  Boolean ( TRUE indicates MAC was able to populate the cs_hfn and ps_hfn values.
  FALSE indicates the data was not populated.

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
boolean
tdsmac_rrc_get_hfn_info(tdsmac_hfn_info_type *hfn_info);

/*-------------------------------------------------------------------
FUNCTION tdsrrc_get_start_value_to_transmit()

DESCRIPTION
Call back defined in tdsrrcsmc.c to give MAC the HFN value.

DEPENDENCIES
  None

RETURN VALUE
Returns the HFN value.

SIDE EFFECTS
  None
--------------------------------------------------------------------*/

uint32
tdsrrc_get_start_value_to_transmit(void);

/*===========================================================================
FUNCTION   tdsrrc_get_gcf_flag_status

DESCRIPTION
  This function returns the status of gcf flag. TRUE if GCF NV is set, false
  otherwise

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/

boolean tdsrrc_get_gcf_flag_status(void);

/*===========================================================================
FUNCTION   rrcrb_query_mac_rab_status

DESCRIPTION
This function will be called by MAC at configuration time to determine
if it is a CS only or PS only call. Based on this MAC will determine how
the AMR rate changes need to be applied based on the power.

DEPENDENCIES
  None.

RETURN VALUE
Function returns if the call is of type CS only, PS only or CS+PS call.

SIDE EFFECTS
  None.
===========================================================================*/
tdsmac_rab_status_e_type tdsrrc_query_mac_rab_status
(
  void
);

/*-------------------------------------------------------------------
FUNCTION tdsmac_rrc_get_cipher_pending_status

DESCRIPTION
 This function will be called by RRC in the case when it receives
 a SMC message

DEPENDENCIES
  None

RETURN VALUE
  Boolean ( TRUE indicates MAC has not reached the ciphering
  activation time for the current config under use. FALSE indicates
  that there is no pending configuration in MAC and ciphering is
  ongoing

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
boolean
tdsmac_rrc_get_cipher_pending_status(void);



/*-------------------------------------------------------------------
FUNCTION tdsmac_ul_allocate_mem_ZI_opt

DESCRIPTION
 This function will be called by RRC at init time so that MAC can go for 
 dynamic memory allocation for the required varaibles.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
--------------------------------------------------------------------*/
void  tdsmac_ul_allocate_mem_ZI_opt(void);

/*===========================================================================

FUNCTION tdsmac_is_hs_cfg_active

DESCRIPTION
 This function will be called by RRC to know whether MAC-hs is
 Active or not.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

boolean tdsmac_is_hs_cfg_active
(
  void
);
/*-------------------------------------------------------------------
FUNCTION tdsmac_ul_allocate_mem_ext_cmd

DESCRIPTION
  This function is called by the external modules like RRC in order to allocate 
  dynamic memory. 

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None.

--------------------------------------------------------------------*/
void *  tdsmac_ul_allocate_mem_ext_cmd
( 
  size_t size
);


/*-------------------------------------------------------------------
FUNCTION tdsmac_ul_free_mem_ext_cmd

DESCRIPTION
  This function is called  in order to free 
  dynamic memory allocated for the external commands.
  
DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

--------------------------------------------------------------------*/
void  tdsmac_ul_free_mem_ext_cmd
(
  void * ptr
);

#endif /* TDSMACRRCIF_H */

