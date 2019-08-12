#ifndef TDSL2DLTASK_H
#define TDSL2DLTASK_H
/*============================================================================================

                      D L  L A Y E R 2    T A S K    H E A D E R    F I L E

DESCRIPTION
  This file contains all of the definitions necessary to interface with downlink layer 2 task.

Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================================*/

/*============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/L2/vcs/l2dltask.h_v   1.14   11 Jun 2002 18:10:10   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l2/inc/tdsl2dltask.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ----------------------------------------------------------------------------
11/23/10   cyx   Remove tdsmvsif.h to clean up l2 interface 
10/24/10   guo   Initial release Upmerged WCDMA to TDS

============================================================================================*/

#include "tdscdma_variation.h"
#include  "queue.h"
#include  "tdsmacrrcif.h"
#include  "tdsl1sapcommon.h"
#include  "tdsl1macdata.h"
#include  "tdsrlcrrcif.h"
#include  "tdsrlci.h"
#include  "tdsrlcdsapif.h"
#include  "tdsmacrlcif.h"
#include  "tdstmcif.h"
#include "tdsl2dldatamodem.h"
#include "tdsl2external.h"
/* --------------------------------- */
/* PUBLIC APIs of tdsl2dltask.h         */
/* --------------------------------- */

/*============================================================================================
                        DOWNLINK LAYER 2 COMMAND DEFINITIONS
============================================================================================*/

/* Downlink layer 2 command ID types */
typedef enum
{
  TDSRLCI_DL_TX_AM_STATUS=0x00,     /* Copy nak_list_q (DL) to tx_nak_q (UL) */

  TDSCMAC_DL_CONFIG_REQ=0x01,       /* Configure DL MAC */

  TDSCRLC_DL_TM_CONFIG_REQ=0x02,    /* Configure DL TM RLC */

  TDSCRLC_DL_UM_CONFIG_REQ=0x03,    /* Configure DL UM RLC */

  TDSRLCI_DL_CONFIG_REQ=0x04,       /* Configure DL AM RLC */

  TDSRLCI_DL_RESET_REQ=0x05,        /* RESET request for DL AM RLC  */

  TDSRLCI_UL_RESET_DONE=0x06,       /* Confirm the RESET completion of UL AM RLC  */

  TDSRLCI_DL_RELEASE_ALL_REQ=0x07,  /* Release All non-MBMS request to the Downlink */


  TDSCRLC_DL_CONTINUE_REQ=0x08,      /*Continue the stopped RBs in UM mode*/

  TDSCRLC_DL_CIPHER_CONFIG_REQ=0x09,/* Configure Cophering on DL RLC*/

  TDSRLC_DL_REGISTER_SRVC_REQ=0x0A, /* Register Downlink Watermark */

  TDSRLC_DL_DEREGISTER_SRVC_REQ=0x0B, /* DeRegister Downlink Watermark */

  TDSRLC_REGISTER_AM_SDU_DISCARD_CALLBACK_REQ=0x0C, /* Request for registration of SDU  */
                                            /* discard callback function for    */
                                            /* RRC                              */
  TDSRLC_PROCESS_MAC_HS_RESET_REQ=0x0D,

  TDSCRLC_DL_ABORT_CIPHER_CONFIG_REQ=0x0E,          /* Request to abort the new DL
                                               ciphering configuration         */

  TDSRLC_HANDLE_SDU_DISCARD_TIMEOUT_EVENT=0x0F, /* Command to handle SDU discard timer */

  TDSRLC_PDCP_CLEANUP_WM=0x10,


  TDSL2_DL_MAX_CMDS            /* Number of DL L2 Command types. must be last entry */

}tdsl2_dl_cmd_enum_type;

/* Downlink Layer 2 Command header */
typedef struct
{
  q_link_type link; /* Quaue link */

  tdsl2_dl_cmd_enum_type cmd_id; /* Command ID */

}tdsl2_dl_cmd_hdr_type;

/* Downlink Layer 2 command data type */
typedef union
{
  /* DL MAC Configuration Request */
  tdsmac_dl_config_type  mac_config_info;

  /* DL RLC Commands */
  /* for internal reset messages  */
  rlc_lc_id_type  dl_rlc_data_id;

  /* for Internal reset messages */
  rlc_lc_id_type  ul_rlc_data_id;

  /* DL TM RLC Configuration Request */
  tdsrlc_dl_tm_config_type dl_tm_config;

  /* DL UM RLC Configuration Request */
  tdsrlc_dl_um_config_type dl_um_config;

  /*DL UM Continue Request*/
  tdsrlc_dl_continue_type dl_continue;
  
  /* DL AM RLC Configuration Request */
  tdsrlci_dl_am_config_req_type dl_am_config_req;

  /* DL RLC Ciphering Configuration Request */
  tdsrlc_dl_cipher_config_type dl_cipher_config;

  /* DL RLC HFN Configuration Request */
  tdsrlc_dl_hfn_type   dl_hfn;

  /* DL Register Srvc Request */
  tdsrlc_dl_register_srvc_type dl_reg;

  /* DL DeRegister Srvc Request */
  tdsrlc_dl_deregister_srvc_type dl_dereg;

  /* Register AM SDU discard callback function  */
  tdsrlc_register_am_sdu_discard_callback_type dl_sdu_discard_cb;

  tdsmac_hs_dl_reset_req_type  mac_hs_dl_reset_req;


  uint32 dl_timer_param;

  /* Abort the new ciphering configuration */
  tdsrlc_abort_cipher_config_type abort_cipher_info;

  tdsrlci_cleanup_wm_cmd_type    cleanup_wm_cmd;
  

}tdsl2_dl_cmd_data_type;

/* DL Layer 2 command buffer type */
typedef struct
{
  tdsl2_dl_cmd_hdr_type    cmd_hdr;

  tdsl2_dl_cmd_data_type   cmd_data;

}tdsl2_dl_cmd_type;


/*===========================================================================

                      Downlink L2-FUNCTION DECLARATIONS

===========================================================================*/
/*=============================================================================================

FUNCTION  tdsl2_dl_wait

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

uint32  tdsl2_dl_wait
(
  /* signals to wait on */
  uint32  wait_sigs
);

/*=============================================================================================

FUNCTION  tdsproc_l2_dl_cmd_queue

DESCRIPTION
  This function processes the downlink layer 2 command queue data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/

void  tdsproc_l2_dl_cmd_queue ( void );

/*=============================================================================================

FUNCTION  tds_l2_dl_task_init

DESCRIPTION
  This task initializes uplink layer 2 queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/

void  tds_l2_dl_task_init ( void );

/*===========================================================================

FUNCTION        tds_l2_dl_task

DESCRIPTION
  This task performs downlink layer 2 processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void  tds_l2_dl_task
(
  dword dummy

    /* Required for REX, ignore */
);


/*===========================================================================

FUNCTION  tdsl2_dl_get_cmd_buf

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
extern  tdsl2_dl_cmd_type  *tdsl2_dl_get_cmd_buf
(
  void
);

/*===========================================================================

FUNCTION        tdsl2_dl_put_cmd

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
extern  void  tdsl2_dl_put_cmd

(
  /* Pointer to the command buffer */
  tdsl2_dl_cmd_type   *cmdptr

);



/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*============================================================================================
                        DOWNLINK LAYER 2 SIGNAL DEFINITIONS
============================================================================================*/

/* This signal is set when an item is placed on tdsl2_dl_cmd_q */
#define     TDSL2_DL_CMD_Q_SIG         0x0001

/* This signal is set when there is data available on the downlink. */
#define     TDSRLC_DL_DATA_IND_SIG     0x0004


#ifdef FEATURE_DOG
// L2 DL watchdog report timer sig.
#define     TDSL2_DL_DOG_RPT_TIMER_SIG 0x0008
#endif

#ifdef FEATURE_TDSCDMA_USE_RLC_BACKUP_TIMER
/* RLC UL backup Timer SIG  */
#define   TDSRLC_DL_TIMER_BACKUP_SIG   0x0010
#endif

#ifdef FEATURE_TDSCDMA_RLC_QXDM_LOGGING

/* Signal to log downlink cipher packet */
#define   TDSRLC_DL_LOG_CIPHER_PKT_SIG 0x0020

/* This signal is set when the log timer expires      */
#define   TDSRLC_DL_AM_LOG_TIMER_EXP_SIG 0x0200


#endif /* FEATURE_TDSCDMA_RLC_QXDM_LOGGING */

#ifdef FEATURE_TDSCDMA_CPU_BASED_FLOW_CONTROL
#define   TDSRLC_DL_FC_SIG               0x0040
#define   TDSRLC_DL_FC_TIMER_SIG         0x0080
#endif /* FEATURE_TDSCDMA_CPU_BASED_FLOW_CONTROL */

/* This signal is set by L1 when DL MAC has to be configured */
#define  TDSMAC_PHY_DL_CONFIG_IND  0x0400

/* This signal is set by L1 for MAC to do any cleanup when config command
   fails at L1. Only applicable id HS channel was to be configured at physical
   layer */
#define TDSMAC_PHY_DL_CONFIG_AT_L1_FAILURE_IND 0x0800


/* This signal is set by L1 when MAC HS has to be configured */
#define TDSMAC_PHY_HS_CONFIG_IND 0x1000




#ifdef FEATURE_Q6_MT

/* This signal is set when there is data available on the downlink. */
#define     TDSRLC_DL_DATA_REASSEMBLY_SIG     0x2000

#endif

#define TDSRLC_DL_AM_BASE_TIMER_SIG 0x80000000

extern void tds_l2_dl_init_tcb(void);

extern rex_tcb_type* tds_l2_dl_get_tcb(void);

#endif /* TDSL2DLTASK_H */
