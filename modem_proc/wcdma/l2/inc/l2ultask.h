#ifndef L2ULTASK_H
#define L2ULTASK_H
/*===========================================================================

                      UL LAYER 2 TASK H E A D E R    F I L E

DESCRIPTION
  This file contains all of the definitions necessary to interface with
  the uplink layer 2 task.

Copyright (c) 1991,1992,1993 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 1999-2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/L2/vcs/l2ultask.h_v   1.18   10 Jul 2002 10:08:14   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l2/inc/l2ultask.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who   what, where, why
--------  ---   ------------------------------------------------------------- 
07/15/14  ac    NV reorganization
07/01/14  aa    CFCM changes for RLC
05/13/14  ymu   Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
11/27/13  scv   Code changes for UL_Compression Feature
11/07/13  ts    DC-HSUPA: Changes for SI timer redesign by keeping a single base timer and reducing signals
05/22/13  kc    Added DC-HSUPA Changes
07/24/12  geg   Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/24/12  zr    Added API to retrieve WCDMA L2 UL TCB pointer
04/26/12  grk   Added new API file exposing L2 command structures
03/06/12  mrg   RACH staus info from L1 to MAC indicaiton with signal based approch enhanced with double buffer.
02/02/12  Ksr   Mutex lock and unlock sequences are updated in RLC to ensure it will not block ISR for a long time.
02/01/12  grk   Feture cleanup
12/06/11  ksr   Fix to handle back to back RACH ABORT and RACH NO ACK to MAC from L1
                Moving to command based interface from signal based approach.
03/09/11  ssg   MAC-ehs support on Apollo.
12/24/10  mrg   signal MAC_UL_TVM_PERIODIC_TMR_EXP_SIG moved out.from feature HSUPA
12/23/10  ssg   Corrected the signal bit.
12/07/10  pj    Added new signal definition for enqueuing data into UL TM channel
10/18/10  ssg   Added changes for new WCDMA-L2 interface.
10/28/10  kvk   Removed featurization from l2_ul_cmd_enum_type to make it compatible with 
                the defintion in wcdmamvsif.h
10/01/10  mrg   TVM event and periodic measurements handled with different timers
07/09/10  grk   1. Changes to indicate UL ISR to send RESET_ACK only after 
                   finishing UL RESET procedure.
                2. Introduced new command to send RESET_ACK for duplicate RESETs.
07/07/10  pj    Added changes to use a global base timer for status processing
05/06/10  pj    Added function prototypes 
02/02/09  av      Moving DSM & DUP FC to UL task  
12/04/09  ssg   New L2 interface
11/24/09  pj    Added new signal for resetting status prohibit timer 
10/23/09   av   Temperature FC  
05/07/09  ssg   Updated Copyright Information
05/04/09  kvk   Added support to send the FC command for WINDOW size update 
                rather than directly updating the WINDOW size from timer CB function
07/29/08  mrg   MAC-EUL log_packet data structures are taken out from the
                l2_ul_cmd_data_type and maintained interanally to MAC.
03/17/08  ar    Featurized RLC_UL_FC_SIG under FEATURE_UL_CPU_BASED_FC
02/01/08  pj    Changed FEATURE_MBMS flag to FEATURE_MODEM_MBMS.
13/12/07  vg    Added handling for RLCI_UL_TX_AM_STATUS cmd
11/30/07  ssg   Added mbms support under FEATURE_MBMS.
09/18/07  vg    added support to CONTINUE RBs by CRLC_UL_CONTINUE_REQ cmd 
05/03/07  sk    Moved the definition of the MAC UL HS
                timers from the maculhssubtask.h to this file.
02/10/07  sk    Logging support for the internal log packet - 0x4323
01/31/07  sk    Added MAC_HS_UL_STATUS_LOG_TMR_EXP_SIG for log code 0x4322
01/11/07  vg    added RLC_UL_START_LOGGING_SIG
11/23/06  ssg   Removed CRLC_UL_SUSPEND_REQ command.
08/11/06  ttl   Support CPU based flow control feature.
07/03/06  ssg   Added RLC_AM_UL_SDU_TX_CNF_SIG signal.
11/11/05  pdv   Fix for CR79364
11/01/05  aw    Added the functionality to read the L2 release version from NV data.
05/03/05  mv    Added the command CRLC_UL_ABORT_CIPHER_CONFIG_REQ to abort the new
                ciphering configuration.
                Added the parameter rlc_abort_cipher_config_type to
                l2_ul_cmd_data_type.
02/11/05  mv    Added the command RLC_UL_PURGE_WM_REQ to purge the UL WM for
                specified logical channels.
                Added the parameter  rlc_ul_purge_wm_ind_type to
                l2_ul_cmd_data_type.
07/19/04  sk    Changed the value of the signal MAC_UL_TFCS_CONTROL_TMR_EXP_SIG
07/16/04  sk    Added the following under FEATURE_TFCS_CONTROL_ENABLE
                1. Added MAC_UL_TFCS_CONTROL_TMR_EXP_SIG to handle the expiry of the TFCS control
                duration timer.
                2. Added mac_tfc_subset_config_info tp l2_ul_cmd_data_type.
                3. Added CMAC_UL_TFC_SUBSET_CFG_REQ to l2_ul_cmd_enum_type.
01/07/04  ttl   Added CELL_PCH support.
07/29/03  ttl   Added RLC_UL_RESET_REPORT_SIG.
02/21/02  ttl   Added RLC_AM_RESET_REQUEST_SIG.
01/20/03  ttl   Defined RLCI_UL_TX_AM_STATUS.
08/27/02  sk    Removed definition of MAC_DL_CONFIG_CNF_SIG.
07/31/02  ttl   Removed RLC_UL_AM0_TIMER_EXP_SIG, RLC_UL_AM1_TIMER_EXP_SIG,
                RLC_UL_AM2_TIMER_EXP_SIG and RLC_UL_AM3_TIMER_EXP_SIG.
                Removed RLCI_UL_10MS_TIMER.
07/08/02  sk    Removed FEATURE_TRAFFIC_VOLUME_MEAS
06/06/02  sk    Added signals MAC_PHY_UL_CONFIG_IND, MAC_DL_CONFIG_CNF_IND
                defined under FEATURE_MAC_ACTIVATION_TIME
                Added new commands(CMAC_UL_TRAFFIC_VOLUME_CONFIG_REQ,
                MAC_UL_TRAFFIC_VOLUME_CONFIG_CNF,CMAC_BLOCK_RACH_TRAFFIC_REQ,
                for traffic volume measurements and command data structures to
                handle these commands. Defined MAC_UL_TRAFFIC_VOLUME_TMR_EXP_SIG
                10ms timer signal.
05/14/02  ttl   Naming changed of wcdma_l2_ul_task.
05/13/02  ttl   Added RLC_UL_DEREGISTER_SRVC_REQ, RLC_OPEN_RX_WIN and
                RLC_CLOSE_RX_WIN primitives.
03/15/02  sk    Defined values for RLC_UL_AM_LOG_TIMER_EXP_SIG signal.
02/12/02  ps    Added "srb_omit" boolean to "mac_ul_build_frame_ind_type" data structure
11/30/01  ttl   Added RLC_UL_POST_DL_RESET_SIG and RLC_UL_RESET_REPORT_SIG.
09/18/01  ttl   Added L2_UL_DOG_RPT_TIMER_SIG for watchdog.
06/29/01  ttl   Added sign definitions for RLC_UL_AM0_TIMER_EXP_SIG, RLC_UL_AM1_TIMER_EXP_SIG,
                RLC_UL_AM2_TIMER_EXP_SIG and RLC_UL_AM3_TIMER_EXP_SIG.
06/18/01  ps    Converted UL TX Status and Build Frame Indications from commands to signals
06/08/01  rc    Removed primitive RLCI_DL_RELEASE_ALL_CNF.
06/07/01  rc    Added primitives CRLC_RELEASE_ALL_REQ and
                RLCI_DL_RELEASE_ALL_CNF.
05/02/01  ttl   Added primitives RLCI_UL_10MS_TIMER and its data definition.
03/26/01  ttl   Added primitives defintion for CRLC_AM_RESET_REQ,
                RLC_REGISTER_AM_SDU_CONFIRM_CALLBACK_REQ, RLCI_UL_RESET_REQ,
                and RLCI_DL_RESET_DONE.
                Added ul_sdu_cnf_cb in l2_ul_cmd_data_type.
01/06/01  ps    Added MAC_UL_TX_STATUS_IND command type
                Added mac_ul_tx_status_ind_type
12/11/00  ps    Removed "rlcmacif.h" include file
11/22/00  ps    Initial release


===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "wcdma_variation.h"
#include  "queue.h"
#include  "macrrcif_v.h"
#include  "l1sapcommon.h"
#include  "l1macdata.h"
#include  "rlcrrcif_v.h"
#include  "rlcdsapif.h"
#include  "macrlcif.h"
#include  "rlci.h"
#include  "nv.h"
#ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
#include "maculhslog.h"
#endif



/* Public portion of the API exists in the following files */
#include "wcdmamvsif.h"
#include "wcdmatmcif.h"

#include "../../common/inc/wnv_common.h"

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*===========================================================================

                        SIGNAL DEFINITIONS

===========================================================================*/

/* This signal is set when an item is placed on l2_ul_cmd_q */
#define   L2_UL_CMD_Q_SIG             0x0001

/* This segnal is set to request UL MAC HFN */
#define   CMAC_UL_HFN_REQ_SIG         0x0002

/*This signal is set to indicate RACH Tx Status */
#define   MAC_UL_TX_STATUS_IND_SIG    0x0004

/* Free Signal */
#define   MAC_FREE_SIGNAL             0x040000

/* This signal is set to indicate build frame */
#define   MAC_UL_BUILD_FRAME_IND_SIG  0x0008

/* RLC AM RESET REQUEST.  */
#define   RLC_AM_RESET_REQUEST_SIG    0x0010

/* RLC AM UL RESET DONE.  */
#define   RLC_AM_UL_RESET_DONE_SIG    0x0020

/* RLC AM UL SDU tx Confirmation */
#define   RLC_AM_UL_SDU_TX_CNF_SIG    0x0040

#define   RLC_UL_FC_SIG               0x0080
#define   RLC_UL_FC_TIMER_SIG         0x0100

#ifdef FEATURE_DOG
/* L2 UL watchdog report timer timed out sig. */
#define   L2_UL_DOG_RPT_TIMER_SIG     0x0200
#endif

/* Report RLC RESET to RRC  */
#define   RLC_UL_RESET_REPORT_SIG     0x0400

/* RLC UL posts RESET messages to RLC DL  */
#define   RLC_UL_POST_DL_RESET_SIG    0x0800

/* Called when the qxdm timer expires. Time to report the statistics to qxdm*/
#ifdef FEATURE_RLC_QXDM_LOGGING
#define   RLC_UL_AM_LOG_TIMER_EXP_SIG  0x1000
#endif

/* This signal is set when the 10ms timer expires */
#define MAC_UL_TVM_EVENT_TMR_EXP_SIG 0x2000


/* Thi signal is set by L1 to indicate to MAC to start using the new config parameters*/
/* received from RRC */
#define MAC_PHY_UL_CONFIG_IND               0x4000

#define MAC_UL_TFCS_CONTROL_TMR_EXP_SIG     0x8000


/* This signal is set when l2_get_nv function returns with nv release flag */
#define L2_NV_CMD_SIG                       0x10000

#ifdef FEATURE_RLC_QXDM_LOGGING
#define RLC_UL_START_LOGGING_SIG    0x020000
#endif


#ifdef FEATURE_WCDMA_HSUPA
/*--------------------------------------------------------------------------
Timer signals for HSUPA
--------------------------------------------------------------------------*/
 /* Timer sig in case of NO Grant/Low Grant/Periodic Grant condition */
#define MAC_HS_UL_SI_TMR_EXP_SIG                0x40000

#ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
/* Timer sig for status log packets */
#define MAC_HS_UL_STATUS_LOG_TMR_EXP_SIG        0x100000
#endif //FEATURE_MAC_HS_UL_QXDM_LOGGING

/* RLC AM RESET STATUS TIMER REQUEST.  */
#define   RLC_AM_RESET_STS_TMR_REQ_SIG          0x800000

#define RLC_UL_AM_BASE_TIMER_SIG              0x2000000

#endif

/* This signal is set when MAC TVM has valid periodic reports to send */
/* This signal is set when (10ms* MAC_UL_PERIODIC_TVM_TIMER_SCALE)ms  timer expires */
#define MAC_UL_TVM_PERIODIC_TMR_EXP_SIG 0x4000000

#ifdef FEATURE_WCDMA_NEW_L2_INTERFACE
/* This signal is set when an item is placed on wcdma_l2_ul_cmd_q */
#define   WCDMA_L2_UL_CMD_Q_SIG            0x8000000
#endif

#ifdef FEATURE_WCDMA_SIMULATE_AMR_PACKETS  
#define RLC_UL_TM_ENQ_DATA_SIG          0x10000000
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA
#define    MAC_HS_UL_SECONDARY_SI_TMR_EXPIRED_SIG   0x20000000
#endif

#ifdef FEATURE_WCDMA_UL_COMPR
#define RLC_UL_COMP_DSM_ALLOC       0x80000000
#endif

/*===========================================================================

                        COMMAND DEFINITIONS

===========================================================================*/

/* MAC_UL_BUILD_FRAME_IND command */
typedef struct
{
    /* current tti */
    l1_tti_enum_type    tti;

    /* current CFN */
    uint8               cfn;

    boolean             srb_omit;

    /* pointer to Tx data buffer */
    l1_ul_tx_data_type  *tx_buf_ptr;

}mac_ul_build_frame_ind_type;


/*===========================================================================

FUNCTION l2_get_release_flag_indicator

DESCRIPTION
  This  function returns the NV release flag to mac and rlc.

DEPENDENCIES
  None.

RETURN VALUE
  l2_nv_rel_indicator_e_type.

===========================================================================*/
l2_nv_rel_indicator_e_type l2_get_release_flag_indicator
(
  void
);

/*===========================================================================

FUNCTION     L2_FREE_CMD_BUF

DESCRIPTION  Releases the command to HEAP or CRIT section, which is allocated
             earlier in Task context using get_cmd_buf
===========================================================================*/
void l2_free_cmd_buf(void *cmd_buf_ptr);


/*===========================================================================
 FUNCTION     WCDMA_L2_UL_INIT_TCB

 DESCRIPTION
   This function populates the cached WCDMA L2 UL TCB pointer through rcinit. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
extern void wcdma_l2_ul_init_tcb(void);

/*===========================================================================
 FUNCTION     WCDMA_L2_UL_GET_TCB

 DESCRIPTION
   This function returns a pointer to the WCDMA L2 UL TCB. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   A pointer to the WCDMA L2 UL TCB.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
extern rex_tcb_type* wcdma_l2_ul_get_tcb(void);

#endif /* L2ULTASK_H */
