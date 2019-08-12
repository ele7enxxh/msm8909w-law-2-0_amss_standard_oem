#ifndef TDSL2ULTASK_H
#define TDSL2ULTASK_H
/*===========================================================================

                      UL LAYER 2 TASK H E A D E R    F I L E

DESCRIPTION
  This file contains all of the definitions necessary to interface with
  the uplink layer 2 task.

Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/L2/vcs/l2ultask.h_v   1.18   10 Jul 2002 10:08:14   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l2/inc/tdsl2ultask.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when      who   what, where, why
--------  ---   -------------------------------------------------------------
10/13/14  sarao CFCM changes for RLC
02/02/11  ps    Changed for TD MAC HSUPA
11/23/10  cyx   Remove tdsmvsif.h to clean up l2 interface
10/24/10  guo   Initial release Upmerged WCDMA to TDS

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "tdscdma_variation.h"
#include  "queue.h"
#include  "tdsmacrrcif.h"
#include  "tdsl1sapcommon.h"
#include  "tdsl1macdata.h"
#include  "tdsrlcrrcif.h"
#include  "tdsrlcdsapif.h"
#include  "tdsmacrlcif.h"
#include  "tdsrlci.h"
#include  "nv.h"
#include "tdsmaculhslog.h"
#include "tdstmcif.h"
#include "tdsl2uldatamodem.h"
#include "tdsl2external.h"
#include "tdsrrcdataext.h"
/* --------------------------------- */
/* PUBLIC APIs of tdsl2ultask.h         */
/* --------------------------------- */

/* Uplink layer 2 command ID types */
typedef enum
{
  TDSCMAC_UL_CONFIG_REQ=0x00,         /* Configure UL MAC */

#ifdef FEATURE_TDSCDMA_UPDATE_SIB7_IN_FACH
  TDSMAC_UL_PERSISTENCE_VAL_CHANGE_IND=0x01, /* Update Persistence value */
#endif

  /* Configure UL MAC traffic volume measurements*/
  TDSCMAC_UL_TRAFFIC_VOLUME_CONFIG_REQ=0x02,

  /* RRC can request MAC to stop transmission on DTCH using this command*/
  TDSCMAC_UL_BLOCK_RACH_TRAFFIC_REQ=0x03,

  TDSCMAC_UL_TFC_SUBSET_CFG_REQ=0x04,

  TDSCMAC_HS_UL_PROCESS_HDR_LOG_PKT_REQ =0x05,
  TDSCMAC_HS_UL_PROCESS_ETFCI_STATUS_LOG_PKT_REQ =0x06,


  TDSCRLC_UL_TM_CONFIG_REQ =0x07,      /* Configure UL TM RLC */

  TDSCRLC_UL_UM_CONFIG_REQ =0x08,      /* Configure UL UM RLC */

  TDSCRLC_AM_CONFIG_REQ =0x09,         /* Configure AM RLC */

  TDSCRLC_AM_RESET_REQ =0x0A,          /* RESET AM RLC */

  TDSCRLC_UL_CIPHER_CONFIG_REQ =0x0B,  /* Configure Ciphering on DL RLC*/

  TDSCRLC_UL_RESUME_REQ =0x0C,         /* Resume transmission on RLC channels*/

  TDSCRLC_RELEASE_ALL_REQ =0x0D,       /* Release all non-MBMS RLC entities.  */


  TDSCRLC_UL_REG_FOR_DATA_IND_REQ =0x0E, /* Register incoming data indication. */

  TDSCRLC_UL_UNREG_FOR_DATA_IND_REQ =0x0F, /* Register incoming data indication. */

  TDSRLC_UL_REGISTER_SRVC_REQ =0x10,   /* Request for service registration for RRC */

  TDSRLC_UL_DEREGISTER_SRVC_REQ =0x11,   /* DeRequest for service registration for RRC */

  TDSRLC_REGISTER_AM_SDU_CONFIRM_CALLBACK_REQ =0x12, /* Request for SDU confirm    */
                                            /* callback function for RRC  */

  TDSRLC_OPEN_RX_WIN =0x13,            /* Open the Receiving Window. */

  TDSRLC_CLOSE_RX_WIN =0x14,           /* Close the Receiving Window.  */

  TDSRLCI_UL_RESET_REQ =0x15,          /* RESET request for UL AM RLC  */

  TDSRLCI_DL_CONFIG_CNF =0x16,         /* Confirm the completion of DL config  */

  TDSRLCI_DL_RESET_DONE =0x17,         /* Confirm the completion of the DL RESET */

  TDSRLC_UL_PURGE_WM_REQ =0x18,            /* Command to purge the uplink WM */

  TDSCRLC_UL_ABORT_CIPHER_CONFIG_REQ =0x19, /* Command to abort the new UL ciphering
                                      configuration                                */
  TDSCRLC_UL_CONTINUE_REQ =0x20,    /* Continue transmission on Stopped RLC channels*/
  
  TDSRLCI_UL_TX_AM_STATUS  =0x21,             /* command to indicate UL to send status PDU */

  TDSRLCI_UL_DUP_RESET_REQ =0x22,      /* command to indicate duplicate RESET reception on DL*/

  TDSMAC_RACH_STATUS_IND =0x23,        /* MAC RACH Status Ind from L1 */

  TDSRLCI_DL_RELEASE_ALL_CNF=0x24,	/* Release All CNF in the Downlink */
#ifdef FEATURE_TDSCDMA_TVM_BLOCK
  TDSCMAC_UL_TRAFFIC_VOLUME_BLOCK_REQ = 0x25, /* Block Traffic Volume Measurement */
#endif
  TDSL2_UL_MAX_CMDS                 /* Number of Ul L2 Command types. must be last entry */

} tdsl2_ul_cmd_enum_type;

/* Command header */
typedef struct
{
  q_link_type       link;       /* Quaue link */

  tdsl2_ul_cmd_enum_type   cmd_id; /* Command ID */

}tdsl2_ul_cmd_hdr_type;

/* Uplink Layer 2 command data type */
typedef union
{
  /* UL MAC Configuration Request */
  tdsmac_ul_config_type          mac_config_info;

  /* UL MAC TFCC COnfig Info */
  tdsmac_tfc_subset_restriction_info_type  mac_tfc_subset_config_info;

  /* for internal reset primitive messages */
  rlc_lc_id_type              ul_rlc_data_id;

  /* UL RLC TM Configuration Request */
  tdsrlc_ul_tm_config_type       ul_tm_config;

  /* UL RLC UM Configuration Request */
  tdsrlc_ul_um_config_type       ul_um_config;

  /* UL RLC AM Configuration Request */
  tdsrlc_am_config_req_type      am_config;

  /* UL RLC Ciphering Configuration Request */
  tdsrlc_ul_cipher_config_type   ul_cipher_config;

  /* UL RLC Resume Request */
  tdsrlc_ul_resume_type          ul_resume;

  /* UL RLC Continue Request */
  tdsrlc_ul_continue_type ul_continue;

  /* UL RLC HFN Request */
  tdsrlc_ul_hfn_type             ul_hfn;

  /* UL Register Srvc Request */
  tdsrlc_ul_register_srvc_type   ul_reg;

  /* UL DeRegister Srvc Request */
  tdsrlc_ul_deregister_srvc_type ul_dereg;

  /* register the SDU confirmation callback function  */
  tdsrlc_register_am_sdu_confirm_callback_type ul_sdu_cnf_cb;

  /* Confirmation for DL AM RLC completing its config */
  tdsrlci_dl_config_cnf_type     dl_config_cnf;

  tdsrlc_ul_register_for_data_ind_type ul_reg_data_ind;

  tdsrlc_ul_unregister_for_data_ind_type ul_unreg_data_ind;

  /* Parameter for purging the UL WM when requested by RRC */
  tdsrlc_ul_purge_wm_ind_type  ul_purge_wm_ind;

  /* Parameter for aborting the new ciphering configuration
     when requested by RRC                                 */
  tdsrlc_abort_cipher_config_type abort_cipher_info;

  /* Parameter for Uplink Timer expiration  */
  uint32                      ul_timer_param;


  /* Configure UL MAC for traffic volume measurement */
  tdsmac_ul_traffic_meas_config_type *ul_traffic_volume_cfg;

/* Block time interval length */
#ifdef FEATURE_TDSCDMA_TVM_BLOCK
  uint16    block_time_interval; 
#endif

  boolean                         block_rach;
  
#ifdef FEATURE_TDSCDMA_UPDATE_SIB7_IN_FACH
  /* RRC updates Persistence value  */
  tdsmac_ul_persistence_config_type persistence_info;
#endif

  /* PRACH transmit status */
  tdsl1_prach_status_enum_type   rach_tx_status_ind;


}tdsl2_ul_cmd_data_type;

/* UL Layer 2 command buffer type */
typedef struct
{
  tdsl2_ul_cmd_hdr_type    cmd_hdr;

  tdsl2_ul_cmd_data_type   cmd_data;

}tdsl2_ul_cmd_type;
/*===========================================================================

                      UpLink L2 FUNCTION DECLARATIONS

===========================================================================*/
/*=============================================================================================

FUNCTION  tdsl2_ul_wait

DESCRIPTION
  This function waits for any signal in a specified signal mask to be set.
  If this procedure detects that any signal in the specified signal mask is
  set, it will return the entire mask of signals set in the task's tcb.

DEPENDENCIES
  None.

RETURN VALUE
  Mask of signals returned by the rex_wait call.

SIDE EFFECTS
  None.

=============================================================================================*/

uint32  tdsl2_ul_wait
  (
/* signals to wait on */
  uint32  wait_sigs
  );
/*=============================================================================================

FUNCTION  tdsproc_l2_ul_cmd_queue

DESCRIPTION
  This function processes the uplink layer 2 command queue data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/

void  tdsproc_l2_ul_cmd_queue ( void );

/*=============================================================================================

FUNCTION  tds_l2_ul_task_init

DESCRIPTION
  This task initializes downlink layer 2 queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/

void  tds_l2_ul_task_init ( void );


/*===========================================================================

FUNCTION        tds_l2_ul_task

DESCRIPTION
  This task performs uplink layer 2 processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void    tds_l2_ul_task
(
  dword dummy
    /* Required for REX, ignore */
);


/*===========================================================================

FUNCTION  tdsl2_ul_get_cmd_buf

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
extern  tdsl2_ul_cmd_type    *tdsl2_ul_get_cmd_buf
(
  void
);

/*===========================================================================

FUNCTION        tdsl2_ul_put_cmd

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
extern  void     tdsl2_ul_put_cmd

(
  /* Pointer to the command buffer */
  tdsl2_ul_cmd_type   *cmd_ptr

);




/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*===========================================================================

                        SIGNAL DEFINITIONS

===========================================================================*/

/* This signal is set when an item is placed on tdsl2_ul_cmd_q */
#define   TDSL2_UL_CMD_Q_SIG             0x0001

/* This segnal is set to request UL MAC HFN */
#define   TDSCMAC_UL_HFN_REQ_SIG         0x0002

/* Free Signal */
#define   TDSMAC_FREE_SIGNAL             0x0004


/* This signal is set to indicate build frame */
#define   TDSMAC_UL_BUILD_FRAME_IND_SIG  0x0008

/* RLC AM RESET REQUEST.  */
#define   TDSRLC_AM_RESET_REQUEST_SIG    0x0010

/* RLC AM UL RESET DONE.  */
#define   TDSRLC_AM_UL_RESET_DONE_SIG    0x0020

/* RLC AM UL SDU tx Confirmation */
#define   TDSRLC_AM_UL_SDU_TX_CNF_SIG    0x0040

#ifdef FEATURE_TDSCDMA_CPU_BASED_FLOW_CONTROL
#define   TDSRLC_UL_FC_SIG               0x0080
#define   TDSRLC_UL_FC_TIMER_SIG         0x0100
#endif /* FEATURE_TDSCDMA_CPU_BASED_FLOW_CONTROL */

#ifdef FEATURE_DOG
/* L2 UL watchdog report timer timed out sig. */
#define   TDSL2_UL_DOG_RPT_TIMER_SIG     0x0200
#endif

/* Report RLC RESET to RRC  */
#define   TDSRLC_UL_RESET_REPORT_SIG     0x0400

/* RLC UL posts RESET messages to RLC DL  */
#define   TDSRLC_UL_POST_DL_RESET_SIG    0x0800

/* Called when the qxdm timer expires. Time to report the statistics to qxdm*/
#ifdef FEATURE_TDSCDMA_RLC_QXDM_LOGGING
#define   TDSRLC_UL_AM_LOG_TIMER_EXP_SIG  0x1000
#endif

/* This signal is set when the 10ms timer expires */
#define TDSMAC_UL_TVM_EVENT_TMR_EXP_SIG 0x2000


/* Thi signal is set by L1 to indicate to MAC to start using the new config parameters*/
/* received from RRC */
#define TDSMAC_PHY_UL_CONFIG_IND        0x80000000



#define TDSMAC_UL_TFCS_CONTROL_TMR_EXP_SIG     0x8000


/* This signal is set when tdsl2_get_nv function returns with nv release flag */
#define TDSL2_NV_CMD_SIG                       0x10000

#ifdef FEATURE_TDSCDMA_MAC_HS_QXDM_LOGGING
#define TDSMAC_HSUPA_STATUS_TIMER_EXP_SIG   0x20000
#endif

#ifdef FEATURE_TDSCDMA_RLC_QXDM_LOGGING
#define TDSRLC_UL_START_LOGGING_SIG    0x040000
#endif


#ifdef FEATURE_TDSCDMA_HSUPA
/*--------------------------------------------------------------------------
Timer Message signals for HSUPA
--------------------------------------------------------------------------*/

#define TDS_MACULHS_T_WAIT_TMR_EXP_SIG                      0x80000
#define TDS_MACULHS_PERIODIC_SI_TMR_EXP_SIG                 0x100000
#define TDS_MACULHS_PERIODIC_NON_SCHED_SI_TMR_EXP_SIG       0x200000
#define TDS_MACULHS_T_RUCCH_TMR_EXP_SIG                     0x400000
#define TDS_MACULHS_T_RUCCH_HYST_TMR_EXP_SIG                0x800000

/*--------------------------------------------------------------------------
command for HSUPA
--------------------------------------------------------------------------*/
#define TDS_MACULHS_ERUCCH_STATUS_IND_SIG                   0x1000000

#endif

/* RLC AM RESET STATUS TIMER REQUEST.  */
#define   TDSRLC_AM_RESET_STS_TMR_REQ_SIG          0x8000000

#define TDSRLC_UL_AM_BASE_TIMER_SIG               0x20000000

/* This signal is set when MAC TVM has valid periodic reports to send */
/* This signal is set when (10ms* TDSMAC_UL_PERIODIC_TVM_TIMER_SCALE)ms  timer expires */
#define TDSMAC_UL_TVM_PERIODIC_TMR_EXP_SIG        0x40000000

/* This signal is set when TVM block timer expires */
#ifdef FEATURE_TDSCDMA_TVM_BLOCK
#define TDSMAC_UL_TVM_BLOCK_TMR_EXP_SIG           0x4000
#endif


/*===========================================================================

                        COMMAND DEFINITIONS

===========================================================================*/

/* MAC_UL_BUILD_FRAME_IND command */
typedef struct
{
    /* current tti */
    tdsl1_tti_enum_type    tti;

    /* current CFN */
    uint8               cfn;

    /* pointer to Tx data buffer */
    tdsl1_ul_tx_data_type  *tx_buf_ptr;

}tdsmac_ul_build_frame_ind_type;


/* Enum for storing the NV release indicator locally */
typedef enum {
  TDSL2_REL_INDICATOR_R4 = 0,
  TDSL2_REL_INDICATOR_5,
  TDSL2_REL_INDICATOR_6,
  TDSL2_REL_INDICATOR_7,
  TDSL2_REL_INDICATOR_8,
  TDSL2_REL_INDICATOR_9
} tdsl2_nv_rel_indicator_e_type;

typedef enum
{
    TDSL2_SPECIAL_SETTINGS_FOR_DEFAULT = 0,
    TDSL2_SPECIAL_SETTINGS_FOR_CONFORMANCE,
    TDSL2_SPECIAL_SETTINGS_FOR_FIELD
}tdsl2_special_test_nv_status_e_type;

extern tdsl2_special_test_nv_status_e_type  l2_specail_test_value;

extern tdsl2_nv_rel_indicator_e_type  tdsl2_nv_rel_indicator;  /* nv release indicator */
extern uint32 tdsl2_opt_bitmask; /*Opt Bitmask NV */
extern tdsl2_flow_control_value_type tdsl2_flow_control_value;  /* Flow Control NV */
/*===========================================================================

FUNCTION tdsl2_get_release_flag_indicator

DESCRIPTION
  This  function returns the NV release flag to mac and rlc.

DEPENDENCIES
  None.

RETURN VALUE
  tdsl2_nv_rel_indicator_e_type.

===========================================================================*/
tdsl2_nv_rel_indicator_e_type tdsl2_get_release_flag_indicator
(
  void
);

/*===========================================================================

FUNCTION tdsl2_get_optimization_mask

DESCRIPTION
  This  function returns the NV release flag to mac and rlc.

DEPENDENCIES
  None.

RETURN VALUE
  tdsl2_nv_rel_indicator_e_type.

===========================================================================*/
uint32 tdsl2_get_optimization_mask
(
  void
);

/*===========================================================================

FUNCTION tdsl2_get_nv_rel_ver_opt_mask

DESCRIPTION
  This  function stores the NV release flag and Opt mask for use to mac and rlc.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void tdsl2_get_nv_rel_ver_opt_mask
(
  void
);


/*===========================================================================

FUNCTION tdsl2_get_nv

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

DEPENDENCIES
  None.

RETURN VALUE
  Bool.

===========================================================================*/
nv_stat_enum_type tdsl2_get_nv
(
  nv_items_enum_type item,        /* which item */
  nv_item_type *tdsdata_ptr          /* pointer to space for item */
);

extern void tds_l2_ul_init_tcb(void);

extern rex_tcb_type* tds_l2_ul_get_tcb(void);

/*===========================================================================

FUNCTION tdsl2_read_special_test_nv

DESCRIPTION
  Reads special test from EFS
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void tdsl2_read_special_test_nv
(
  void
);

#endif /* TDSL2ULTASK_H */
