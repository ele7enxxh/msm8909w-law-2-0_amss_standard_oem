#ifndef MCCCSFBSUP_H
#define MCCCSFBSUP_H

/*===========================================================================

               E X T E R N A L   C S F B  S U P
                     D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the CSFB Support operation.

  Copyright (c) 2010 - 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/14   as      Fix IRAT for Dual Receive
03/19/14   ppr     SRVCC Feature Changes
03/14/14   gga     Fixed the bugs which came due to R8 CSFB Call setup 
                   optimization
01/27/14   gga     Added support to move to CSFB_IDLE if no 1x Release Order 
                   is received for MT SO76 SMS.
12/09/13   gga     Added Handling for LTE Abort HO Request
11/22/13   ppr     CSFB:1x CGI Reporting feature changes
01/07/13   gga     Forced C2K params req after getting the SIB8 after 
                   call end.
11/19/12   ssh     Added SMS over CSFB support.
05/25/12   ssh     Removed the optimization of sending unique CSFB 
                   availability/unavailabilty status to MMODE.
04/30/12   jj      CSFB: Changes to prevent multiple handover indications 
                   sent to MMODE.
03/13/12   jj      CSFB: Fix to not map retry failure to SOFT failure count.
03/12/12   ssh     Guarded the PWR DWN reg with a timer.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
10/26/11   jj      CSFB: Fix to conclude CSFB procedure on receipt of any
                   FTC message.
09/13/11   jj      CSFB: Support for Hard/Soft failures for Reg.
08/26/11   jj      CSFB: Support for GCSNA negotiation.
07/22/11   jj      CSFB: Support for slotted mode operation in CSFB mode.
07/21/11   jj      CSFB: Added support for C2K Params receipt timeout.
07/18/11   jj      CSFB: Added support for Registration Reject Order.
05/16/11   jj      CSFB: Added support to send GCSNA ack for incoming 1x
                   CRKT SRV message.
05/03/11   jj      CSFB: Support for handling ECAM failures.
05/02/11   jj      CSFB: Added support for handling GCSNA ack for UL messages.
04/29/11   jj      CSFB: Added support for error scenarios - phase2.
04/28/11   jj      CSFB: Added support for error scenarios - phase1.
04/21/11   jj      CSFB: Added chanes to support SSD operation, Unique Challenge
                   in CSFB mode.
04/19/11   jj      CSFB: Code merge from rel/1H11 branch.
04/05/11   jj      CSFB: Update to report Reg failure to CM incase GCSNA
                   ack doesnt come on time.
03/31/11   jj      CSFB: First cut changes to handle GCSNA prev mismatch.
03/21/11   jj      CSFB: Changes to support failure handling for invalid 1X
                   params.
03/16/11   jj      CSFB: Changes to support PSIST operation in tunnel mode.
03/03/11   jj      CSFB: Support for forced registration in case of context loss.
03/03/11   jj      CSFB: Added support for Reselection with time transfer
                   and ECAM paging channel assignment.
02/21/11   jj      CSFB: Added support for Unified TT approach
                   for eCSFB UHDM MO/MT calls.
02/28/11   jj      CSFB: Support for forced registration in case of context loss.
02/15/11   jj      Fixed compile and lint warnings.
02/11/11   jj      CSFB: Updated exit reasons for context loss.
02/03/11   jj      CSFB: Added changes to parse extra params in 1X Params.
01/19/11   jj      CSFB: Added support for triggering forced 1X registration
                   in case of any LTE activity which will need us to update
                   context.
12/15/10   jj      CSFB: Added default values for CSFB exit.
12/14/10   ssh     CSFB: code clean up.
12/09/10   ssh     CSFB: Added MT support contd. (GPM processing and
                   sending PRM support.)
11/28/10   jj      CSFB: Added support for CSFB -> CDMA handover.
11/19/10   ssh     CSFB: corrected the malformed ORM.
11/15/10   ssh     CSFB: added MO call support.
11/15/10   jj      CSFB: Added support for HO from EUTRA processing.
11/13/10   jj      CSFB:Rectified a compile error.
11/09/10   jj      CSFB state redesign .
11/03/10   jj      Changed nid length to 16 bit value in c2k params.
10/21/10   jj      Added Redirection support.
10/10/10   ssh     Added MO call support.
09/23/10   jj      Support for Reg context update.
09/02/10   ssh     Corrected the C2K parameter msg structure.
09/01/10   jj      Updated code to support tunneled RGM.
08/25/10   ssh     Restructured the SIB8 Processing.
08/19/10   ssh     Updated the comments for C2K parameters.
08/18/10   jj      Added state level implementation.
08/05/10   ssh     Added C2K msg parsing.
08/02/10   jj      CSFB:Added support for CSFB state machine.
07/30/10   ssh     Added support for sib8 msg processing.
07/29/10   jj      Created file to have external functions for LTE-1X CSFB
                   operation.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "modem_1x_defs.h"
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "comdef.h"
#include "queue.h"
#include "customer.h"
#include "mc_v.h"
#include "mc_i.h"
#include "caii_v.h"
#include "caii_i.h"
#include "onex_cp_msg.h"
#include "lte_rrc_irat_msg.h"
#include "msgr_rex.h"
#include "mccgcsna.h"
#include "mcccsfb.h"



/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* The value is mentioned in GCSNA specs(C.S0097)
   in section 3 CDMA2000PARAMETERS */
#define MCC_CSFB_C2K_PARAMS_REC_TYPE   0

/* Maximum dialed digits that could be included in tunneled ORM. */
#define MAX_DIALED_DIGITS_IN_TUNNELED_ORM   MC_MAX_DIALED_DIGITS

/* Maximum number of times to retry C2K Params */
#define MCC_CSFB_MAX_C2K_PARAMS_ATTEMPTS 7

/* MAximum count of SOFT failures */
#define MCC_CSFB_MAX_REG_ATTEMPTS 3

/* Maximum time we can wait without no voice services */
#define MCC_CSFB_T_MAX_NO_VOICE 90000

/* Invalid value of the Global cell ID which can be received in the 
 * Redirection indication from LTE */
#define INVALID_GLOBAL_CELL_ID 0xFFFFFFFF

/* Invalid number of the MNC digits which is received in PLMN ID through
 * Redirection indication from LTE */
#define INVALID_NUM_MNC_DIGITS 4

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration of possible errors                                          */
typedef enum
{
  /**<-- Invalid SIB8 */
  MCC_CSFB_INVALID_SIB8 = 0,
  /**<-- Failed GCSNA attempt */
  MCC_CSFB_FAIL_GCSNA_ATTEMPT,
  /**<-- Failed max attempts to do GCSNA transmission */
  MCC_CSFB_MAX_GCSNA_ATTEMPT,
  /**<-- Invalid GCSNA message */
  MCC_CSFB_INVALID_GCSNA_MSG,
  /**<-- No Error */
  MCC_CSFB_ERROR_NONE
} mcc_csfb_err_code_type;

/* CSFB Context Loss reasons structure */
typedef enum
{
  MCC_CSFB_CNTX_LOSS_NONE,
  MCC_CSFB_CNTX_LOSS_IMSI_PAGING,
  MCC_CSFB_CNTX_LOSS_IMSI_ATTACH,
  MCC_CSFB_CNTX_LOSS_IDEN_REQ,
} mcccsfb_cntx_loss_type;

/* minimum size of the buffer can be 64 bits.*/
#define C2K_PARMS_MIN_LEN 8

/* Enumeration of possible C2K Params State */
typedef enum
{
  /**<-- Initial state of C2K Params */
  MCC_1X_PARAM_REQ_INIT,
  /**<-- C2K Params Request in progress */
  MCC_1X_PARAM_REQ_PRGS,
  /**<-- C2K Params received.*/
  MCC_1X_PARAM_RSP_RXED,
  /**<-- Done processing C2K Params */
  MCC_1X_PARAM_REQ_DONE,
  /**<-- SRVCC C2K Params received */
  MCC_1X_SRVCC_PARAM_RSP_RXED,
  /**<-- Invalid C2K Params received */
  MCC_1X_PARAM_RSP_INVALID
}c2k_params_state_type;

/* Current CSFB handover condition. */
typedef enum {
  MCC_CSFB_NONE,
  MCC_CSFB_CALL,
  MCC_CSFB_RESEL,
  MCC_CSFB_REDIR
} mcc_csfb_status_e;

/* Enumeration of possible MO HO reasons - for now
 * it is UHDM and ECAM AM=1.
 */
typedef enum
{
  /**<-- MO HO intiated because of ECAM AM =1 */
  MCC_CSFB_MO_HO_ECAM,
  /**<-- MO HO intiated because of UHDM */
  MCC_CSFB_MO_HO_UHDM,
  /**<-- MO HO None */
  MCC_CSFB_MO_HO_NONE,
}mcc_csfb_mo_ho_type;

/* Variables to store time information, which will eventually be used
 * in Time Transfer.
 * These params are either received in MO HO (for ECAM/UHDM) or
 * Reselection Indication.
 */
typedef struct
{
  uint32    lte_ostmr;        /* LTE RTC counter value */
  uint64    cdma_sys_time;    /* CDMA System time */
} mcc_csfb_tt_info_type;

/* Stores SIB8 Information */
extern lte_rrc_1x_sib8_info_ind_s          mcc_csfb_sib8_msg;

/** Buffer to Store TLAC enacapsulated PDU */
extern byte mcc_gcsna_tlac_pdu[];

/* Variable to store TLAC encapsulated PDU's len */
extern word mcc_gcsna_tlac_pdu_len;

/* For redir based MO calls, store the CM ORIGINATION cmd. Internal ORIG cmd
** will be posted once the redir is done.*/
extern mc_msg_type csfb_unprocessed_cmd;

/* Variable to track message seq number of last sent RGM */
extern word mcc_gcsna_last_reg_msg_seq;

/* Variable to track message seq number of last sent UL MSG */
extern word mcc_gcsna_last_ul_msg_seq;

/* Stores the current MO HO cause */
extern mcc_csfb_mo_ho_type mcc_csfb_curr_mo_ho;

/* variable to store time transfer params */
extern mcc_csfb_tt_info_type mcc_csfb_tt_info;

/* Boolean used for: After call end, CM activates 1XCP based on SIB1 (if SIB1 indicates that 
 * SIB8 will be received). In this case, CM is presuming that SIB8 will be 
 * received soon. NAS has a guard timer if SIB1 indicates the SIB8 txn. MC
 * will be in CSFB act state and respond to the NW requests with the last 
 * SIB8 and C2K. However, we need to stop the Autonomous/Forced Reg till
 * we receive the SIB8.
 */
extern boolean mcc_csfb_wait_for_sib8_before_doing_reg;

/* Variable to keep track of if PSIST check for RGM has been performed */
extern boolean mcc_csfb_is_reg_psist_chk_done;

/* Variable to keep track of context loss */
extern mcccsfb_cntx_loss_type mcc_csfb_cntx_loss_ind;

/* Timer for getting c2k parameters */
extern rex_timer_type  mcc_csfb_c2k_param_tmo_timer;

/* Timer for getting c2k parameters */
extern rex_timer_type  mcc_csfb_pwr_dwn_reg_guard_timer;

/* Timer for waking up in a slotted mode in CSFB mode */
extern rex_timer_type mcc_csfb_slotted_mode_timer;

/* Timer to get the acknowledgement of SO76 SMS */
extern rex_timer_type mcc_csfb_so76_sms_access_guard_timer;

/* Timer for getting SRVCC UHDM */
extern rex_timer_type  mcc_srvcc_uhdm_tmo_timer;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                     CDMA2000 Parameters                                  */

typedef PACK (struct)
{
  byte rec_type[8];             /* Record Type */
  byte sid_inc[1];              /* System identification included */
  byte sid[15];                 /* System identification */
  byte nid_inc[1];              /* Network identification included */
  byte nid[16];                 /* Network identification */
  byte mul_sids_inc[1];         /* Multiple System identification included */
  byte mul_sids[1];             /* Multiple System identification */
  byte mul_nids_inc[1];         /* Multiple Network identification included */
  byte mul_nids[1];             /* Multiple Network identification */
  byte reg_zone_inc[1];         /* Registration Zone included */
  byte reg_zone[12];            /* Registration Zone */
  byte total_zone_inc[1];       /* Total Zone included */
  byte total_zone[3];           /* Total Zone */
  byte zone_timer_inc[1];       /* Zone Timer included */
  byte zone_timer[3];           /* Zone Timer */
  byte pkt_zone_id_inc[1];      /* Packet Zone Id included */
  byte pkt_zone_id[8];          /* Packet Zone Id */
  byte pz_hyst_parms_inc[1];    /* Packet Zone Hyst Parmeters included */
  byte pz_hyst_enabled[1];      /* Packet Zone Hyst enabled */
  byte pz_hyst_info_inc[1];     /* Packet Zone information included */
  byte pz_hyst_list_len[1];     /* Packet Zone Hyst list length */
  byte pz_hyst_act_timer[8];    /* Packet Zone Hyst act timer */
  byte pz_hyst_timer_mul[3];    /* Packet Zone Hyst timer mul */
  byte pz_hyst_timer_exp[5];    /* Packet Zone Hyst timer expiry */
  byte prev_inc[1];             /* Protocol Rev included */
  byte prev[8];                 /* Protocol Rev */
  byte min_prev_inc[1];         /* Min Protocol Rev included */
  byte min_prev[8];             /* Min Protocol Rev */
  byte neg_sci_sup_inc[1];      /* Negative SCI supported included */
  byte neg_sci_sup[1];          /* Negative SCI supported */
  byte enc_mode_inc[1];         /* Encryption Mode included */
  byte enc_mode[2];             /* Encryption Mode */
  byte enc_sup_inc[1];          /* Encryption Supported included */
  byte enc_sup[1];              /* Encryption Supported */
  byte sig_enc_sup_inc[1];      /* Sig Encryption Supported included */
  byte sig_enc_sup[8];          /* Sig Encryption Supported */
  byte msg_int_sup_inc[1];      /* Message Integrity Supported included */
  byte msg_int_sup[1];          /* Message Integrity Supported */
  byte sig_int_sup_incl_inc[1]; /* Sig Integrity Supported incl included */
  byte sig_int_sup_incl[1];     /* Sig Integrity Supported incl */
  byte sig_int_sup_inc[1];      /* Sig Integrity Supported included */
  byte sig_int_sup[8];          /* Sig Integrity Supported */
  byte auth_inc[1];             /* Authentication included */
  byte auth[2];                 /* Authentication */
  byte max_alt_so_inc[1];       /* Max Alternate Service Options included */
  byte max_alt_so[3];           /* Max Alternate Service Options */
  byte use_sync_id_inc[1];      /* Use Sync Id included */
  byte use_sync_id[1];          /* Use Sync Id */
  byte ms_init_pos_loc_sup_ind_inc[1]; /* MS initiated pos loc det included */
  byte ms_init_pos_loc_sup_ind[1];     /* MS initiated pos loc determination */
  byte mob_qos_inc[1];          /* MOB QOS included */
  byte mob_qos[1];              /* MOB QOS */
  byte band_class_info_req_inc[1];     /* Band Class Info request included */
  byte band_class_info_req[1];         /* Band Class Information request */
  byte band_class_inc[1];       /* Band Class included */
  byte band_class[5];           /* Band Class */
  byte bypass_reg_ind_inc[1];   /* Bypass Registration Indication included */
  byte bypass_reg_ind[5];       /* Bypass Registration Indication */
  byte alt_band_class_inc[1];   /* Alternate Band Class included */
  byte alt_band_class[5];       /* Alternate Band Class */
  byte max_add_srv_inst_inc[1]; /* Max Additional service instance included */
  byte max_add_srv_inst[3];     /* Max Additional service instance */
  byte home_reg_inc[1];         /* Home Registration included */
  byte home_reg[1];             /* Home Registration */
  byte for_sid_reg_inc[1];      /* Foreign SID Registration included */
  byte for_sid_reg[1];          /* Foreign SID Registration */
  byte for_nid_reg_inc[1];      /* Foreign NID Registration included */
  byte for_nid_reg[1];          /* Foreign NID Registration */
  byte powerup_reg_inc[1];      /* Powerup Registration included */
  byte powerup_reg[1];          /* Powerup Registration */
  byte powerdown_reg_inc[1];    /* Power down Registration included */
  byte powerdown_reg[1];        /* Power down Registration */
  byte parameter_reg_inc[1];    /* Parameter Registration included */
  byte parameter_reg[1];        /* Parameter Registration */
  byte reg_prd_inc[1];          /* Registration Periodicity included */
  byte reg_prd[7];              /* Registration Periodicity */
  byte reg_dist_inc[1];         /* Registration Distance included */
  byte reg_dist[11];            /* Registration Distance */
  byte pref_msid_type_inc[1];   /* Pref MS identifier field type included */
  byte pref_msid_type[2];       /* Preferred MS identifier field type */
  byte ext_pref_msid_type_inc[1];  /* Extended Pref MSID field type included */
  byte ext_pref_msid_type[2];      /* Extended Pref MSID field type */
  byte meid_reqd_inc[1];        /* MEID req included */
  byte meid_reqd[1];            /* MEID req */
  byte mcc_inc[1];              /* Mobile Country Code included */
  byte mcc[10];                 /* Mobile Country Code */
  byte imsi_11_12_inc[1];       /* IMSI 11th and 12th bit included */
  byte imsi_11_12[7];           /* IMSI 11th and 12th bit */
  byte imsi_t_sup_inc[1];       /* IMSI_T included */
  byte imsi_t_sup[1];           /* IMSI_T */
  byte reconnect_msg_ind_inc[1];/* Reconnect Message Indication included */
  byte reconnect_msg_ind[1];    /* Reconnect Message Indication */
  byte rer_mode_sup_inc[1];     /* Radio env rept Mode supported included */
  byte rer_mode_sup[1];         /* Radio env rept Mode supported */
  byte tkz_mode_sup_inc[1];     /* Tracking Zone Mode supported included */
  byte tkz_mode_sup[1];         /* Tracking Zone Mode supported */
  byte tkz_id_inc[1];           /* Tracking Zone Id included */
  byte tkz_id[1];               /* Tracking Zone Id */
  byte plt_rep_inc[1];          /* Pilot Report included */
  byte plt_rep[1];              /* Pilot Report */
  byte sdb_sup_inc[1];          /* Short Data Burst supported included */
  byte sdb_sup[1];              /* Short Data Burst supported */
  byte auto_fcso_allowed_inc[1];/* Autonomous Fast Call Setup Order included */
  byte auto_fcso_allowed[1];    /* Autonomous Fast Call Setup Order */
  byte sdb_in_rcnm_ind_inc[1];  /* SDB allowed in Reconnect Message included */
  byte sdb_in_rcnm_ind[1];      /* SDB allowed in Reconnect Message */
  byte fpc_fch_inc[1];          /* Forward Power Control fch included */
  byte fpc_fch_init_setpt_rc3[8];/* Forward Power Control fch init setpt rc3 */
  byte fpc_fch_init_setpt_rc4[8];/* Forward Power Control fch init setpt rc4 */
  byte fpc_fch_init_setpt_rc5[8];/* Forward Power Control fch init setpt rc5 */
  byte fpc_fch_init_setpt_rc11[8];/* Forward Power Control fch init setpt rc11 */
  byte fpc_fch_init_setpt_rc12[8];/* Forward Power Control fch init setpt rc12 */
  byte t_add_inc[1];            /* Pilot detection threshold included */
  byte t_add[6];                /* Pilot detection threshold */
  byte plt_incl_inc[1];         /* Pilot PN seq off index increment included */
  byte plt_incl[4];             /* Pilot PN seq offset index increment */
  byte rand_inc[1];             /* Auth random challenge value included */
  byte rand[32];                /* Authentication random challenge value */
  byte lp_sec_inc[1];           /* Leap Seconds included */
  byte lp_sec[8];               /* Leap Seconds */
  byte ltm_off_inc[1];          /* Local time offset from UTC included */
  byte ltm_off[6];              /* Local time offset from UTC */
  byte daylt_inc[1];            /* Daylight Savings Time indicator included */
  byte daylt[1];                /* Daylight Savings Time indicator */
  byte gcsna_l2_ack_timer_inc[1];      /* GCSNA Layer2 Ack Timer included */
  byte gcsna_l2_ack_timer[8];          /* GCSNA Layer2 Ack Timer */
  byte gcsna_seq_context_timer_inc[1]; /* System identification included */
  byte gcsna_seq_context_timer[8];     /* GCSNA Sequence Context Timer */
} mcc_csfb_c2k_parms_msg_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* common structure for CDMA2000 Parameters and HO FROM EUTRA HO           */


typedef struct
{
  c2k_params_state_type params_state; /* variable to keep track of what state
                                         c2K params are in */
  uint32 ho_rand;                  /* Rand value which CP will use in authentication for ORM/PRM */
  byte rec_type;                /* Record Type */
  boolean sid_inc;              /* System identification included */
  word sid;                     /* System identification */
  boolean nid_inc;              /* Network identification included */
  word nid;                     /* Network identification */
  boolean mul_sids_inc;         /* Multiple System identification included */
  boolean mul_sids;             /* Multiple System identification */
  boolean mul_nids_inc;         /* Multiple Network identification included */
  boolean mul_nids;             /* Multiple Network identification */
  boolean reg_zone_inc;         /* Registration Zone included */
  word reg_zone;                /* Registration Zone */
  boolean total_zone_inc;       /* Total Zone included */
  byte total_zone;              /* Total Zone */
  boolean zone_timer_inc;       /* Zone Timer included */
  byte zone_timer;              /* Zone Timer */
  boolean pkt_zone_id_inc;      /* Packet Zone Id included */
  byte pkt_zone_id;             /* Packet Zone Id */
  boolean pz_hyst_parms_inc;    /* Packet Zone Hyst Parmeters included */
  boolean pz_hyst_enabled;      /* Packet Zone Hyst enabled */
  boolean pz_hyst_info_inc;     /* Packet Zone information included */
  boolean pz_hyst_list_len;     /* Packet Zone Hyst list length */
  byte pz_hyst_act_timer;       /* Packet Zone Hyst act timer */
  byte pz_hyst_timer_mul;       /* Packet Zone Hyst timer mul */
  byte pz_hyst_timer_exp;       /* Packet Zone Hyst timer expiry */
  boolean prev_inc;             /* Protocol Rev included */
  byte prev;                    /* Protocol Rev */
  boolean min_prev_inc;         /* Min Protocol Rev included */
  byte min_prev;                /* Min Protocol Rev */
  boolean neg_sci_sup_inc;      /* Negative SCI supported included */
  boolean neg_sci_sup;          /* Negative SCI supported */
  boolean enc_mode_inc;         /* Encryption Mode included */
  byte enc_mode;                /* Encryption Mode */
  boolean enc_sup_inc;          /* Encryption Supported included */
  boolean enc_sup;              /* Encryption Supported */
  boolean sig_enc_sup_inc;      /* Sig Encryption Supported included */
  byte sig_enc_sup;             /* Sig Encryption Supported */
  boolean msg_int_sup_inc;      /* Message Integrity Supported included */
  boolean msg_int_sup;          /* Message Integrity Supported */
  boolean sig_int_sup_incl_inc; /* Sig Integrity Supported incl included */
  boolean sig_int_sup_incl;     /* Sig Integrity Supported incl */
  boolean sig_int_sup_inc;      /* Sig Integrity Supported included */
  byte sig_int_sup;             /* Sig Integrity Supported */
  boolean auth_inc;             /* Authentication included */
  byte auth;                    /* Authentication */
  boolean max_alt_so_inc;       /* Max Alternate Service Options included */
  byte max_alt_so;              /* Max Alternate Service Options */
  boolean use_sync_id_inc;      /* Use Sync Id included */
  boolean use_sync_id;          /* Use Sync Id */
  boolean ms_init_pos_loc_sup_ind_inc; /* MS initiated pos loc det included */
  boolean ms_init_pos_loc_sup_ind;     /* MS initiated pos loc determination */
  boolean mob_qos_inc;          /* MOB QOS included */
  boolean mob_qos;              /* MOB QOS */
  boolean band_class_info_req_inc;     /* Band Class Info request included */
  boolean band_class_info_req;         /* Band Class Information request */
  boolean band_class_inc;       /* Band Class included */
  byte band_class;              /* Band Class */
  boolean bypass_reg_ind_inc;   /* Bypass Registration Indication included */
  byte bypass_reg_ind;          /* Bypass Registration Indication */
  boolean alt_band_class_inc;   /* Alternate Band Class included */
  byte alt_band_class;          /* Alternate Band Class */
  boolean max_add_srv_inst_inc; /* Max Additional service instance included */
  byte max_add_srv_inst;        /* Max Additional service instance */
  boolean home_reg_inc;         /* Home Registration included */
  boolean home_reg;             /* Home Registration */
  boolean for_sid_reg_inc;      /* Foreign SID Registration included */
  boolean for_sid_reg;          /* Foreign SID Registration */
  boolean for_nid_reg_inc;      /* Foreign NID Registration included */
  boolean for_nid_reg;          /* Foreign NID Registration */
  boolean powerup_reg_inc;      /* Powerup Registration included */
  boolean powerup_reg;          /* Powerup Registration */
  boolean powerdown_reg_inc;    /* Power down Registration included */
  boolean powerdown_reg;        /* Power down Registration */
  boolean parameter_reg_inc;    /* Parameter Registration included */
  boolean parameter_reg;        /* Parameter Registration */
  boolean reg_prd_inc;          /* Registration Periodicity included */
  byte reg_prd;                 /* Registration Periodicity */
  boolean reg_dist_inc;         /* Registration Distance included */
  word reg_dist;                /* Registration Distance */
  boolean pref_msid_type_inc;   /* Pref MS identifier field type included */
  byte pref_msid_type;          /* Preferred MS identifier field type */
  boolean ext_pref_msid_type_inc;  /* Extended Pref MSID field type included */
  byte ext_pref_msid_type;      /* Extended Pref MSID field type */
  boolean meid_reqd_inc;        /* MEID req included */
  boolean meid_reqd;            /* MEID req */
  boolean mcc_inc;              /* Mobile Country Code included */
  word mcc;                     /* Mobile Country Code */
  boolean imsi_11_12_inc;       /* IMSI 11th and 12th bit included */
  byte imsi_11_12;              /* IMSI 11th and 12th bit */
  boolean imsi_t_sup_inc;       /* IMSI_T included */
  boolean imsi_t_sup;           /* IMSI_T */
  boolean reconnect_msg_ind_inc;/* Reconnect Message Indication included */
  boolean reconnect_msg_ind;    /* Reconnect Message Indication */
  boolean rer_mode_sup_inc;     /* Radio env rept Mode supported included */
  boolean rer_mode_sup;         /* Radio env rept Mode supported */
  boolean tkz_mode_sup_inc;     /* Tracking Zone Mode supported included */
  boolean tkz_mode_sup;         /* Tracking Zone Mode supported */
  boolean tkz_id_inc;           /* Tracking Zone Id included */
  boolean tkz_id;               /* Tracking Zone Id */
  boolean plt_rep_inc;          /* Pilot Report included */
  boolean plt_rep;              /* Pilot Report */
  boolean sdb_sup_inc;          /* Short Data Burst supported included */
  boolean sdb_sup;              /* Short Data Burst supported */
  boolean auto_fcso_allowed_inc;/* Autonomous Fast Call Setup Order included */
  boolean auto_fcso_allowed;    /* Autonomous Fast Call Setup Order */
  boolean sdb_in_rcnm_ind_inc;  /* SDB allowed in Reconnect Message included */
  boolean sdb_in_rcnm_ind;      /* SDB allowed in Reconnect Message */
  boolean fpc_fch_inc;          /* Forward Power Control fch included */
  byte fpc_fch_init_setpt_rc3;  /* Forward Power Control fch init setpt rc3 */
  byte fpc_fch_init_setpt_rc4;  /* Forward Power Control fch init setpt rc4 */
  byte fpc_fch_init_setpt_rc5;  /* Forward Power Control fch init setpt rc5 */
  byte fpc_fch_init_setpt_rc11;  /* Forward Power Control fch init setpt rc11 */
  byte fpc_fch_init_setpt_rc12;  /* Forward Power Control fch init setpt rc12 */
  boolean t_add_inc;            /* Pilot detection threshold included */
  byte t_add;                   /* Pilot detection threshold */
  boolean plt_incl_inc;         /* Pilot PN seq off index increment included */
  byte plt_incl;                /* Pilot PN seq offset index increment */
  boolean rand_inc;             /* Auth random challenge value included */
  dword rand;                   /* Authentication random challenge value */
  boolean lp_sec_inc;           /* Leap Seconds included */
  byte lp_sec;                  /* Leap Seconds */
  boolean ltm_off_inc;          /* Local time offset from UTC included */
  byte ltm_off;                 /* Local time offset from UTC */
  boolean daylt_inc;            /* Daylight Savings Time indicator included */
  boolean daylt;                /* Daylight Savings Time indicator */
  boolean gcsna_l2_ack_timer_inc;      /* GCSNA Layer2 Ack Timer included */
  byte gcsna_l2_ack_timer;             /* GCSNA Layer2 Ack Timer */
  boolean gcsna_seq_context_timer_inc; /* System identification included */
  byte gcsna_seq_context_timer;        /* GCSNA Sequence Context Timer */

} mcc_csfb_c2k_parms_type;

/** structure to store the c2k parameter msg */
extern mcc_csfb_c2k_parms_type c2k_parms_msg;

/* CSFB exit types  */
typedef enum{
MCC_CSFB_EXIT_NONE,               /* EXIT none */
MCC_CSFB_EXIT_RELEASE,            /* Call Release because of various reasons */
MCC_CSFB_EXIT_CSFB_DEACT,         /* CSFB Deact because of receiving CSFB Deact */
MCC_CSFB_EXIT_TO_CDMA_MODE,       /* Move to CDMA State because of Call */
MCC_CSFB_EXIT_IRAT_SWITCH,        /* Exit because of IRAT Switch because of
                                   * Redir/Resel.
                                   */
MCC_CSFB_EXIT_CNXT_LOSS,          /* Exit because of LTE Context Loss */
MCC_CSFB_EXIT_TT_FAIL,            /* Exit because of Time Transfer failure */
MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE,  /* Exit because of errors seen in 1x stack */
MCC_CSFB_EXIT_GCSNA_MSG_TX,       /* Exit because of sening GCSNA msg */
} mcc_csfb_exit_name_type;

/* Reasons for exiting CSFB mode */
typedef enum{
MCC_CSFB_EXIT_RSN_NONE,
MCC_CSFB_EXIT_RSN_USER_CALL_RELEASE,
MCC_CSFB_EXIT_RSN_NW_CALL_RELEASE,
MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN,
MCC_CSFB_EXIT_RSN_DEACT_OTHER,
MCC_CSFB_EXIT_RSN_MALFORMED_UHDM,
MCC_CSFB_EXIT_RSN_TT_FAIL,
MCC_CSFB_EXIT_RSN_IRAT_SWITCH_FAIL,
MCC_CSFB_EXIT_RSN_IRAT_SWITCH_SUCCESS,
MCC_CSFB_EXIT_RSN_TCH_INIT_FAIL,
MCC_CSFB_EXIT_RSN_ENTER_CDMA_TRAFFIC,
MCC_CSFB_EXIT_RSN_LTE_CNTX_LOSS_IMSI_PAGING,
MCC_CSFB_EXIT_RSN_LTE_CNTX_LOSS_IMSI_ATTACH,
MCC_CSFB_EXIT_RSN_LTE_CNTX_LOSS_IDEN_REQ,
MCC_CSFB_EXIT_RSN_TT_JUMP_FAIL,
MCC_CSFB_EXIT_RSN_MALFORMED_ECAM,
MCC_CSFB_EXIT_RSN_ENTER_CDMA_IDLE,
MCC_CSFB_EXIT_RSN_LTE_ERR,
MCC_CSFB_EXIT_RSN_PSIST_CHK_FAIL,
MCC_CSFB_EXIT_RSN_INVALID_1X_PARAMS,
MCC_CSFB_EXIT_RSN_GCSNA_PREV_MISMATCH,
MCC_CSFB_EXIT_RSN_GCSNA_ACK_FAIL,
MCC_CSFB_EXIT_RSN_RELEASE_DONE,
MCC_CSFB_EXIT_RSN_SO_CHK_FAIL,
MCC_CSFB_EXIT_RSN_GCSNA_MSG_TX_TIMEOUT,
MCC_CSFB_EXIT_RSN_MO_1X_ACQ_FAIL,
MCC_CSFB_EXIT_RSN_MT_1X_ACQ_FAIL,
MCC_CSFB_EXIT_RSN_1X_CALL_END,
MCC_CSFB_EXIT_RSN_REG_REJ_RXED,
MCC_CSFB_EXIT_RSN_C2K_PARAM_TMO,
MCC_CSFB_SLOTTED_MODE_TIMER_ID,
MCC_CSFB_EXIT_RSN_GCSNA_REJ,
MCC_CSFB_EXIT_RSN_GCSNA_ACK,
MCC_CSFB_EXIT_RSN_LTE_ABORT_HO,
MCC_CSFB_EXIT_RSN_MT_SMS_SO76_TMO,
} mcc_csfb_exit_reason_type;

/* Structure containing exit information for CSFB */
typedef struct {
  mcc_csfb_exit_name_type exit;        /* Specifies exit reason                 */
  mcc_csfb_exit_reason_type exit_reason;
} mcc_csfb_exit_type;

/* Exit type to track exits in CSFB mode; */
extern mcc_csfb_exit_type mcc_csfb_exit;
/*===========================================================================

                        Function prototypes

===========================================================================*/

/*===========================================================================

FUNCTION MCC_CSFB_PROC_HS_CMD

DESCRIPTION
  This function processes commands received from the handset while in the
  CSFB Idle State or CSFB Access Substate.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_csfb_proc_hs_cmd
(
  mc_msg_type *cmd_ptr
    /* pointer to message received from the handset */
);

/*===========================================================================

FUNCTION MCC_CSFB_PROC_PENDING_CMD

DESCRIPTION
  This function processes the pending cmds in legacy 1X mode. E.g., for redir
  based MO calls, we need to process the ORIG_F cmd after going to legacy 1X
  mode.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern word mcc_csfb_proc_pending_cmd(
  mc_msg_type *cmd_ptr,
    /* pointer to message received from the handset */
  word new_state
    /* next state to be processed */
);
/*===========================================================================

FUNCTION MCC_CSFB_PROC_MSGR_CMD

DESCRIPTION
  This function processes commands received from the MSGR while in the
  CSFB Idle State or CSFB Access Substate.

  The MSGR commands which could be received in this state are:
  IRAT Switch Commands - Redir/Resel
  SIB8 indication
  CDMA 20000 Params
  GCSNA Messages Tunneled from LTE-RRC
  Reg Reset Context

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mcc_csfb_proc_msgr_cmd
(
  mc_msgr_rcv_msg_type* msgr_ptr,
    /* pointer to message received from the handset */
  word new_state
    /* next state to be processed */
);

/*===========================================================================

FUNCTION MCC_CSFB_GET_ERROR_TYPE

DESCRIPTION
  This function performs error handling and determines if an error is SOFT/HARD
  failure.

DEPENDENCIES
  None.

RETURN VALUE
  SOFT - value 0
  HARD - value 1

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_csfb_is_hard_failure
(
  mcc_csfb_err_code_type err_code,
  /* Failure cause from a broader level */
  mc_msgr_rcv_msg_type *msgr_cmd_ptr
  /* pointer to message received from the msgr */
);

/*===========================================================================
FUNCTION MCC_CSFB_SEND_CGI_RSP

DESCRIPTION
  This API will send the CGI_Req_Response to LTE-RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mcc_csfb_send_cgi_rsp( void );

/*===========================================================================
FUNCTION MCC_CSFB_CGI_WAIT_FOR_SYS_PARM

DESCRIPTION
  This API is to wait for System Parameters Msg.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_cgi_wait_for_sys_parm( void );

/*===========================================================================

FUNCTION MCC_CSFB_PROC_MSGR_RX

DESCRIPTION
  This function processes commands received from the MSGR while in the
  CSFB Idle State or CSFB Access Substate.

  The MSGR commands which could be received in this state are:
  IRAT Switch Commands - Redir/Resel
  SIB8 indication
  GCSNA Messages Tunneled from LTE-RRC
  Reg Reset Context

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE to confirm if the RXed message has been successfully
SIDE EFFECTS
  None.

===========================================================================*/
extern void mcc_csfb_proc_msgr_rx
(
  mc_msgr_rcv_msg_type *msgr_cmd_ptr
  /* pointer to message received from the msgr */
); /* mcc_csfb_proc_msgr_rx */

/*===========================================================================

FUNCTION MCC_CSFB_REG_IDLE_CHECK

DESCRIPTION
  This function performs idle registration procedures called out in
  IS-95A section 6.6.5.5.2.1.

DEPENDENCIES
  None.

RETURN VALUE
  If a registration is required, the type of registration required will be
  returned.  Otherwise the special indicator MCCREG_NO_REG_REQUIRED will
  be returned.

SIDE EFFECTS
  None.

===========================================================================*/
byte mcc_csfb_reg_idle_check( void );

/*===========================================================================

FUNCTION MCC_CSFB_PROCESS_SIB8_MSG

DESCRIPTION
  This function processes the SIB8 msg. As part or processing, it stores the
  SIB8 msg. It check for the validity of the msg based on the reg parms rxed
  It also performs negative system check with the received SID/NID.
  If required, it also reports the CSFB avail and unavail status to CM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mcc_csfb_process_sib8_msg(mc_msgr_rcv_msg_type *msgr_cmd_ptr);

/*===========================================================================

FUNCTION MCC_CSFB_PROC_C2K_PARMS

DESCRIPTION
  This function parses the received C2K parameters msg.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/
extern byte mcc_csfb_proc_c2k_parms(
  uint8 *buf_ptr,
    /* Pointer to the received message */
  uint32 len
    /* Pointer to the number of bytes received in buf_ptr */
);

/*===========================================================================

FUNCTION  MCC_GCSNA_BUILD_RGM

DESCRIPTION
  This function build a Registration Message (TLAC + L3 Part)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_rgm(void);

/*===========================================================================

FUNCTION MCC_SET_IMSI_CLASS_0
  This function sets the address fields for a class zero imsi.

DEPENDENCIES
  None

RETURN VALUE
  Length of the class zero imsi.

SIDE EFFECTS
  None

===========================================================================*/

byte mcc_set_imsi_class_0(
  caii_imsi_class_0_type *imsi
  /* Pointer to imsi address fields of message to be transmitted */
);

/*===========================================================================

FUNCTION MCC_SET_IMSI_CLASS_1
  This function sets the address fields for a class one imsi.

DEPENDENCIES
  None

RETURN VALUE
  Length of the class one imsi.

SIDE EFFECTS
  None

===========================================================================*/

byte mcc_set_imsi_class_1
(
  caii_imsi_class_1_type *imsi
    /* Pointer to imsi address fields of message to be transmitted */
);
/*===========================================================================

FUNCTION XLATE_GCANA_INT_MSG

DESCRIPTION
  This function translates an GCSNA transmitted to the base station over
  tunnel from internal to external format.

DEPENDENCIES
  mcc_gcsna_int_msg should have been filled before calling this function.

RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_msg(byte msg_type);

/*===========================================================================

FUNCTION XLATE_GCSNA_INT_TLAC_HDR

DESCRIPTION
  This function translates an Access Channel Message header (MSG_TYPE
  field through the MSID field) from internal to external format.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_tlac_hdr(word *len_ptr);

/*===========================================================================

FUNCTION XLATE_GCSNA_INT_REG

DESCRIPTION
  This function translates a Registration Message from internal
  to external format.

DEPENDENCIES
  p_rev_in_use is required in the internal format.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void xlate_gcsna_int_reg(void);

/*===========================================================================

FUNCTION MCC_CSFB_SET_SD_CONTEXT

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
boolean mcc_csfb_set_sd_context(void);


/*===========================================================================

FUNCTION MCC_CSFB_PROCESS_LAYER3_MSG

DESCRIPTION
  This function processes the layer3 part of the message which is received in 1X Circuit service message/ GCSNA ack.
  The layer 3 processing will be similar to legacy 1x's layer3 message processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_process_layer3_msg(void);


/*===========================================================================

FUNCTION MCC_CSFB_PROCESS_DL_MSG

DESCRIPTION
  This function processes GCSNA messages received from LTE over MSGR while in the
  CSFB Idle State or CSFB Access Substate.

  The messages which could be received in this state are:
  1X Param Response
  GCSNA 1X Circuit Service Message
  GCSNA L2 ack
  GCSNA Service Reject

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_process_dl_msg(lte_irat_tunnel_dl_msg_ind_s *rx_msg);

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_ORM

DESCRIPTION
  This function build a Origination Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_orm (void);

/*===========================================================================

FUNCTION  MCC_CSFB_BUILD_TLAC_HDR

DESCRIPTION
  This function build a the TLAC header part of any crkt_srv_msg.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_tlac_hdr(byte msg_type);

/*===========================================================================

FUNCTION  MCC_CSFB_BUILD_L3_PART

DESCRIPTION
  This function build a L3 Part of any crkt_srv_msg msg.
  mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type is used to recognize
  which msg has to be built.

DEPENDENCIES
  mcc_gcsna_int_msg should have been filled before calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_l3_part(byte msg_type);

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_ORM_L3_PART

DESCRIPTION
  This function builds L3 part of Tunneled Origination Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ERR_FATAL if the service option set to originate is not a valid service
    option.

===========================================================================*/
void mcc_csfb_build_orm_l3_part(void);

/*===========================================================================

FUNCTION XLATE_GCSNA_INT_ORM

DESCRIPTION
  This function translates an Origination Message from internal
  to external format.

DEPENDENCIES
  p_rev_in_use is required in the internal format.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_orm(void);

#ifdef FEATURE_IS2000_REL_C
/*===========================================================================

FUNCTION MCCSA_SETUP_MSG_INT_FIELDS

DESCRIPTION
  This function setups message integrity fields. These fields exist starting
  from REL C.

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
/* Following parameters not used with some FEATURE define combinations */
/*lint -esym(715, burst_type) */
/*lint -esym(715, data_len) */
/*lint -esym(715, data_ptr) */
/*lint -esym(715, digit_mode) */
extern void mccsa_setup_msg_int_fields
(
  caii_tx_msg_type *msg_ptr,
  caii_ac_auth_msg_int_type *auth_msg_int_ptr
);
#endif /* FEATURE_IS2000_REL_C */

/*===========================================================================

FUNCTION MCCSEC_SET_BS_MI_CAP

DESCRIPTION
  This function sets the base station's Message Integrity capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccsec_set_bs_mi_cap
(
  boolean mi_supported
);
/*===========================================================================

FUNCTION MCC_POST_PROCESS_HANDOVER

DESCRIPTION
  This function does the post processing of the Handover Complete Indication
  receiption. As  a part of post processing, CP will
       o Send the deferred CM commands (like Call_connect, Privacy settings).
       o Get out of the CSFB mode and mark the entry reason as Acquisition.

DEPENDENCIES
  To call this function Handover Complete Confirmation should have been received.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_post_process_handover(void);

/*===========================================================================

FUNCTION MCC_CSFB_EXIT_REASON_PROCESSING

DESCRIPTION
  This function processes the various exir reasons collected over
  operation in various states and take a decision on next state.

  The factors which we consider to take any decision depend on
  the CSFB state we are in + the type of error encountered.

DEPENDENCIES
  None.

RETURN VALUE
  After this function we should be either in CSFB Idle state or in CDMA_EXIT state
  or in CDMA_INIT state or in CDMA_TRAFFIC state.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_csfb_exit_reason_processing(void);

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_PRM_L3_PART

DESCRIPTION
  This function builds L3 part of the Tunneled Page Response Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_prm_l3_part(void);

/*===========================================================================

FUNCTION MCC_CSFB_DEFAULT_1X_PARAMS

DESCRIPTION
  This function update the C2K params msg with the default values as per
  the tunnel requirements.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_default_1x_params(void);

/*===========================================================================

FUNCTION XLATE_GCSNA_INT_PRM

DESCRIPTION
  This function translates an Page response Message from internal
  to external format.

DEPENDENCIES
  p_rev_in_use is required in the internal format.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_prm(void);

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_PRM

DESCRIPTION
  This function builds a Page Response Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_prm (void);

/*===========================================================================
FUNCTION MCC_CSFB_INIT_AFTER_TIME_TRANSFER
  This function performs the necessary intializations required after time
  transfer to CDMA Idle state from CSFB/Inactive state.

DEPENDENCIES
  Prior Time transfer to idle state should have been successful.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_init_after_time_transfer(void);

/*===========================================================================
FUNCTION MCC_CSFB_BUILD_MSG

DESCRIPTION
  This function builds UL Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_build_msg (void);

/*===========================================================================
FUNCTION MCC_CSFB_BUILD_GCSNA_ACK

DESCRIPTION
  This function forms GCSNA ack

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_gcsna_ack(byte gcsna_ack_seq);

/*===========================================================================

FUNCTION  MCC_CSFB_DEACTIVATE_LOWER_LAYERS

DESCRIPTION
  This procedure cleans up the CDMA activation of the lower layes
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RXTX task will go to start mode,  Receive task will exit CDMA mode,
  Transmit task will exit CDMA mode and Search task will go to Start
  mode.

===========================================================================*/
void mcc_csfb_deactivate_lower_layers(void);

/*===========================================================================
FUNCTION MCC_CSFB_SETUP_GCSNA_REJ_MSG

DESCRIPTION
  This function sets up GCSNA Reject message to be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcc_csfb_setup_gcsna_rej_msg(byte rej_cause);

/*===========================================================================
FUNCTION MCC_CSFB_SEND_HANDOVER_COMPLETE_IND

DESCRIPTION
  This function indicates MMODE that handover is complete.
  We conclude handover procedures after we receive any FTC message after
  handoff.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if setting of context is success, FALSE otherwise..

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean mcc_csfb_send_handover_complete_ind(void);

#ifdef FEATURE_MODEM_1X_SRVCC
/*===========================================================================
FUNCTION  MCC_SRVCC_IS_LAYER3_MSG_SUPPORTED

DESCRIPTION
  This function checks if Layer3 Message supported in SRVCCC Mode or not

DEPENDENCIES
  None.

RETURN VALUE
  If suitable GCSNA Option has been found.
  TRUE - If Layer3 Message Supported in SRVCCC Mode
  FALSE - If Layer3 Message Not Supported in SRVCCC Mode

SIDE EFFECTS
  None.
===========================================================================*/
boolean mcc_srvcc_is_layer3_msg_supported(void);

/*===========================================================================
FUNCTION MCC_SEND_CM_SRVCC_HO_FAIL_IND

DESCRIPTION
  This function sends SRVCC Handoff Fail Indication to CM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_send_cm_srvcc_ho_fail_ind(void);

/*===========================================================================
FUNCTION MCC_SEND_LTERRC_SRVCC_HO_FAIL_RSP

DESCRIPTION
  This function sends SRVCC Handoff Fail Rsp to LTE-RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_send_lterrc_srvcc_ho_fail_rsp
(
  lte_irat_1xsrvcc_ho_failure_cause_e ho_fail_cause
);
#endif /* FEATURE_MODEM_1X_SRVCC */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
/*===========================================================================

FUNCTION MCC_CSFB_SETUP_REJECT_ORD

DESCRIPTION
  This function sets up the reject order to be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_setup_reject_ord
(
  byte ordq,
    /* reason for rejecting the message */
  byte rej_msg_type,
    /* the type of message being rejected */
  byte rej_order,
    /* if the rej_msg_type is an order, this is the type of order */
  byte rej_ordq
    /* if the rej_msg_type is an order, this is the ordq of the rej_order */
);

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_DBM_L3

DESCRIPTION
  This function builds L3 part of Data Burst Message.

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mcc_csfb_build_dbm_l3(
  byte burst_type,
    /* Burst Type */
  byte data_len,
    /* Byte length of Data Burst data */
  byte *data_ptr,
    /* Pointer to Data Burst data */
  word curr_state
    /* Current state being processed */
);

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_DBM

DESCRIPTION
  This function builds a Data Burst Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_dbm (void);

/*===========================================================================
FUNCTION MC_CM_SEND_CALL_RESP

DESCRIPTION
  This function sends call response ind to CM. This is required for SO 76 
  call failures as well as normal release indications to CM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_cm_send_call_resp(lte_irat_1xcsfb_ho_failure_cause_e rej_value);

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */


/*===========================================================================

FUNCTION MCC_CSFB_UPDATE_REG_VAR

DESCRIPTION
  This function processes the registration variables of a received SIB8/ C2K message
  and updates reg context as specified in IS-95A section 6.6.5.5.2.2.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/

void mcc_csfb_update_reg_var(void);

/*===========================================================================
FUNCTION MCC_CSFB_FILL_SIB8_NBRS

DESCRIPTION
  This function determines fills in SIB8 neighbors as received in SIB8.
  This information is passed to SD.
  These neighbors will later be used if we hit failure on serving system.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_csfb_fill_sib8_nbrs(sd_cdma_sib8_nbr_cell_info_list_s *sib8_nbr_list);

/*===========================================================================
FUNCTION MCC_CSFB_GET_STATUS

DESCRIPTION
  This function determines current CSFB handover status.

DEPENDENCIES
  None.

RETURN VALUE
  Current CSFB handover status is passed.

SIDE EFFECTS
  None.
===========================================================================*/
mcc_csfb_status_e mcc_csfb_get_status(void);

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#endif /* MCCCSFBSUP_H */
