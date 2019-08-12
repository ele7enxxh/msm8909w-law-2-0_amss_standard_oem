/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2003 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2007 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2011 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2013 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2015 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/src/ftm_wcdma_ber.c#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/14/15   sd     Initialize sec_e_req_mask to 0 for non DCHSUPA NS calls
01/02/15   ag      Setting some LOW priority messages to HIGH
03/11/13   dw      Add Bolt DCHSUPA support
07/29/13   kr      Fix FTM to online crash when setup RMC fails
05/22/13  ka/ac    dc hsupa changes
05/20/13   kcj     Prevent FTM_WCDMA_BER_STOP_WCDMA_MODE_REQ from issuing L1 stop twice. 
05/07/13   ck      Add waiting until WCDMA finishes stopping
04/10/13   gh      Remove feature for Triton
04/03/13   kcj     Feature out previous changes for Triton. 
04/04/13   kcj     Updated from RRC/L1 interface change. 
01/30/12   dw      Fix for non-signaling BER issue where Rx loopback handler was not processed
11/16/12   gh      Clear l1_setup_cmd before populating and set call_type_mask
11/13/12   ec	    Update to use new geran api for DSDA 
07/29/12   ac      wcdma rumi bringup changes
06/28/12   nv      Initialize l1_cpc_drx_req_mask to zero.
06/01/12   swb     Remove ability for back-to-back ACQ requests
05/15/12   sa      Fix FTM_WCDMA_BER_RX_LEVEL_REQ to use DB10
05/03/12   bm      Modified the FTM WCDMA NS start sequence to exit previous
                   ftm rf mode before enter mode for WCDMA NS is done.
07/28/11   nrk     Fixed crash during HSDPA Non-signaling test
05/30/11   aka     Picked up the fix for W crash in FTM mode by re-ordering
                   RLC releasing sequence
03/22/12   aa      Defined FEATURE_MODEM_MBMS for compilation fix
03/21/12   sb      Replace l2_ul_put_cmd with wcdma_l2_ul_put_cmd and
                   l2_ul_cmd_type with wcdma_l2_ul_cmd_type.
03/19/12   ka      Remove obsolete tmc header 
03/19/12   ka      Replace ftm_tcb with rex_self() 
03/13/12   gh      Add api to clear RRC cmd queue
02/23/12   aa      mac_dl_action set to MAC_DL_CONFIG
02/17/12   swb     Add support for RSCP
02/16/12   gr/ars  Fix compiltion issues
02/15/12   sar     Removed FTM_WCDMA_BER_DEBUG.
02/13/12   swb     move function declarations to .h file
01/28/12   sar     Feature reduction changes.
01/27/11   dw      Added FTM common control in FTM WCDMA BER.
12/28/11   ars     Corrected signal clearing in FTM_WCDMA_BER_EVENT_STATUS_REQ
12/28/11   ars     Correct timing loop for Synchronous diagnostic SE BER/BLER report
12/05/11   ars     WCDMA Abs Power: TxAGC Override feature has a delta
11/07/11   ars     Fix for RMC64 SE/BER
10/28/11   gh      Add APIs to check if RRC cmd is START/STOP_WCDMA_MODE_CNF
10/25/11  ac/ka    Add proper cleanup for stop mode
10/10/11   swb     Added a cause for stop command from FTM to WCDMA L1
08/21/11   ac      changes related to Bc19
04/28/11   sar     Merged 1X API cleanup changes.
04/28/11   vb      Fix compiler warnings
04/28/11   sar     CMI-4 Cleanup.
04/22/11   dw      Replace tmc heap with modem heap allocation
04/18/11   dw      KW fixes
03/10/11   dw/ems     Implemented support for RMC384
02/23/11   dw      Fix compiler warnings
10/21/10   ems     Factory features Rx Lvl, non-signaling events and SE/BER
09/22/10   dw      Klocwork error fix
09/03/10   dw      Fix for non-signaling Tx AGC glitches
05/12/10   ac      BC11 bringup
03/16/10   kguo    Merged from scmm_final_merge branch
02/17/10    vb     Klocwork error fixes
01/25/10    vb     Lint error fixes
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
10/15/09   ckl     Fixed SEBER measurement failure caused by initial slot_num
08/24/09    dw     copy DL SDUs timestamp to UL SDU in app_ptr field of dsm_item
07/22/09    dw     initilize Watermark sturctures through DSM to fix WCDMA Non-Signaling crash
05/11/09   jfc     Add forward declarations to get rid of compiler warnings
05/01/09    dw     Initialize slot_num of SEBER config to 0 for WCDMA mode to fix broken SEBER test.
03/24/08    dw     set new_tfcs_list_present for L2 command to TRUE to fix broken non-signaling.
12/03/08    dw     Enable FW Tx SBI writes in Non-Signaling
06/30/08   dw      Unload 1x mdsp image through SRCH
02/26/08   adm     Added support for new RRC feature, FEATURE_TFCC_ENHNC.
11/06/07    ka     Ensure 1x fw is unloaded to prevent crash.
08/08/07   jfc     Mainlined T_MSM6280 feature
06/25/07   ad      Added support for single ended BER
04/24/07   jfc     Set acq.agc threshold for L1 ACQ Request.
03/08/07   jfc     Set MAC configuration to No-Op in CMAC and CPHY command
01/23/07   jfc     Set mdsp_image field for L1 cmd to combined WCDMA_GSM
12/08/06   dp      Use call_type_mask properly for CPHY_SETUP
10/31/06   ycl     Remove "DEC.h" and change DEC_TOTAL_TRCHT_PER_CCTRCH to L1_MAX_TRCH.
06-06-06    dp     Add TFCS config command.
05/23/06   jac     Fixed LINT errors.
04/03/06   dp      Featurize new band defs to MSM6280, so this code can be shared
                   with 6250/6275 MSM.  Flush DSM buffers on setup/handoff
                   so old data is not looped back.  Formatting changes.
02/21/06  tws      Change WCDMA_1800 to BC3. Add support for BC4 and BC9.
02/15/06   dp      Add stop/start LSM test mode when start/stop WCDMA non-signaling testing
                   occurs.  This should force GPS to release any tcxo pdm locks.
11/16/05   dp      Remove compiler warnings
10/27/05   dp      Add symmetric loopback capabilities (send UL CRC)
10-17-05   dp      Add ftm_umts_ber.h include file.
09-26-05   dp      Update F3 messages.
08-26-05   dp      Add fix to CPHY_SETUP_REQ; need to properly initialize the hho_setup_mask.
07-05-05   dp      Add checks on current state before processing commands.  Add
                   support for reconfigurations of DCH using new DCH Setup command,
                   with extended parameter list.
05-19-05   dp      Add HSDPA support (featurized)
04-13-05   asr     Under FEATURE_OPTIMIZE_SEARCH_FOR_HHO, send ACQ_REQ to WCDMA L1
                   obeying new API
11-18-04   dp      Added ftm_wcdma_ber_cleanup function
11-16-04   dp      Added RMC64 channel
11-8-04    scm     Command confirmation support from WL1 back to FTM.
11-4-04    dp      Added MAC,RLC config. and loopback capabilities.
7-1-04     dp      Created
===========================================================================*/


/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftm_v.h"
#include "rf_test_task_v.h"
#include "ftm_umts_ber.h"
#include "rfumts.h"
#include "ftm_wcdma_ber.h"
#include "rfm_wcdma.h"
#include "modem_mem.h"
#include "wcdma_common_api.h"
#if defined( FEATURE_FTM_WCDMA_BER) && defined(FEATURE_HSDPA)
#include "ftm_hsdpa_bler.h"
#endif
#include "ftm_common_control.h"

#include "l1task.h"
#include "err.h"
#include "msg.h"
#include "rex.h"
#include "task.h"

#include "wfw_mod_intf.h"
#include "wfw_sw_intf.h"

#include "macrrcif.h"
#include "dsmutil.h"

#include "ftm_log.h"

#ifdef FEATURE_MMGPS
#include "lsm.h"
#endif

#ifdef FTM_HAS_SE_BER
#include "ftm_se_ber.h"
#endif
#ifdef FEATURE_CDMA1X
#include "onex_non_sig.h"
#endif

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_WCDMA_BER

extern WfwTxAgcAsyncWriteStruct  *rf_wfw_intf_txagc_async_write_ptr;
extern WfwSwIntfStruct           *rf_fwsw_intf_addr_ptr;

/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                               MACROS

===========================================================================*/
#define FTM_WCDMA_BER_SET_STATE(s)   (ftm_wcdma_ber_current_state = s)
#define FTM_WCDMA_BER_GET_STATE()    (ftm_wcdma_ber_current_state)

/*===========================================================================

                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

/* Queue for external messages (e.g. from RRC) */
static q_type                ftm_wcdma_ber_q;

/* Queue and Free Queue for internal message */
static q_type                ftm_wcdma_ber_int_q;
static q_type                ftm_wcdma_ber_int_free_q;
/* Allocated buffer for internal messages */
static ftm_wcdma_ber_int_cmd_type          ftm_wcdma_ber_int_cmd_buf[10];

/* Watermarks and Queues for loopback of PDUs */

static dsm_watermark_type    ftm_wcdma_ber_ul_wm[FTM_WCDMA_BER_MAX_NUM_LOGICAL_CHANNELS];
static dsm_watermark_type    ftm_wcdma_ber_dl_wm[FTM_WCDMA_BER_MAX_NUM_LOGICAL_CHANNELS];
static q_type                ftm_wcdma_ber_ul_wm_q[FTM_WCDMA_BER_MAX_NUM_LOGICAL_CHANNELS];
static q_type                ftm_wcdma_ber_dl_wm_q[FTM_WCDMA_BER_MAX_NUM_LOGICAL_CHANNELS];



static ftm_wcdma_ber_dl_db_type                  ftm_wcdma_dl_db = {NULL, NULL, NULL, 0, FALSE, FALSE};
static ftm_wcdma_ber_ul_db_type                  ftm_wcdma_ul_db = {NULL, NULL, NULL, FALSE};
static ftm_wcdma_ber_state_type                  ftm_wcdma_ber_current_state = FTM_WCDMA_BER_STATE_NULL;
static ftm_wcdma_ber_dch_config_type             dch_config = {FTM_WCDMA_BER_RMC_TYPE_12_2};

/* Buffers to hold all dl config */
static l1_dl_phychan_db_struct_type       ftm_wcdma_dl_phychan_db;
static l1_dl_cctrch_info_struct_type      ftm_wcdma_dl_cctrch_info;
static l1_dl_trch_info_struct_type        ftm_wcdma_dl_trch[L1_MAX_TRCH];
static l1_dl_tf_info_struct_type          ftm_wcdma_dl_tf_info[L1_MAX_TRCH][L1_MAX_TF_PER_TRCH];
static l1_dl_ctfc_info_struct_type        ftm_wcdma_dl_ctfc_info;
static l1_dl_ctfc_entry_info_struct_type  ftm_wcdma_ctfc_entry_info[L1_MAX_TFC];
static uint8                              ftm_wcdma_tfi_table[L1_MAX_TFC][L1_MAX_TRCH];
static mac_dl_dch_trch_config_type        ftm_wcdma_dl_dch_trch_config[L1_MAX_TRCH];
static mac_dl_dsch_trch_config_type       ftm_wcdma_dl_dsch_trch_config[L1_MAX_TRCH];
static mac_dl_fach_trch_config_type       ftm_wcdma_dl_fach_trch_config[L1_MAX_TRCH];

/* Buffers to hold all ul config */
static l1_ul_phychan_cfg_struct_type        ftm_wcdma_ul_phychan_cfg;
static l1_ul_cctrch_cfg_struct_type         ftm_wcdma_ul_cctrch_cfg;
static l1_ul_semi_static_parms_struct_type  ftm_wcdma_ul_semi_static_parms[L1_UL_TRCH_MAX];
static l1_ul_gain_factor_parms_struct_type  ftm_wcdma_ul_gain_factor_parms[L1_UL_TFC_MAX];
static mac_ul_tfcs_type                     ftm_wcdma_ul_mac_tfcs;
static mac_ul_cctrch_config_type            ftm_wcdma_ul_cctrch_config_type;

/* Structure for events that will be reported synchronously */
typedef struct
{
	uint8 has_updated;
	ftm_wcdma_ber_sync_status_type status;

} ftm_wcdma_ber_sync_status_struct;

/* array to record the events that will be reported synchronously
   via FTM_WCDMA_BER_EVENT_STATUS_REQ
*/
ftm_wcdma_ber_sync_status_struct WCDMA_BER_Sync_Status[  FTM_WCDMA_BER_SYNC_STATUS_MAX ];

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/



void ftm_wcdma_ber_allocate_dl_db( void);
void ftm_wcdma_ber_free_dl_db( void);
void ftm_wcdma_ber_allocate_ul_db(void);
void ftm_wcdma_ber_free_ul_db( void);
void ftm_wcdma_ber_process_msgs( void);
void ftm_wcdma_ber_process_l1_messages( rrc_cmd_type *);

void ftm_wcdma_ber_send_CPHY_START_WCDMA_MODE_REQ(void);
void ftm_wcdma_ber_send_CPHY_IDLE_REQ(void);
void ftm_wcdma_ber_send_CPHY_ACQ_REQ(ftm_wcdma_ber_acquire_config_type *);
void ftm_wcdma_ber_send_ULDL_CPHY_SETUP_REQ(void);
void ftm_wcdma_ber_send_CMAC_DL_CONFIG_REQ(void);
void ftm_wcdma_ber_send_CMAC_UL_CONFIG_REQ(void);
void ftm_wcdma_ber_send_CRLC_DL_CONFIG_REQ(void);
void ftm_wcdma_ber_send_CRLC_DL_REGISTER_REQ(void);
void ftm_wcdma_ber_send_CRLC_UL_CONFIG_REQ(void);
void ftm_wcdma_ber_send_CRLC_DL_RELEASE_REQ(void);
void ftm_wcdma_ber_send_CRLC_UL_RELEASE_REQ(void);
void ftm_wcdma_ber_send_CRLC_DL_DEREGISTER_REQ(void);
void ftm_wcdma_ber_send_CRLC_UL_DEREGISTER_REQ(void);

void ftm_wcdma_ber_send_int_cmd( ftm_wcdma_ber_int_cmd_type *msg);
void ftm_wcdma_ber_loopback_handler(uint8 wm_id);
void ftm_wcdma_ber_flush_dsm_buffers( void);
void ftm_wcdma_ber_clear_sync_status( void);
ftm_wcdma_ber_sync_status_type ftm_wcdma_ber_set_ul_pwr( byte override_enable,int2 tx_power_10th_dbm);

/*===========================================================================

FUNCTION        FTM_WCDMA_BER_CREATE_RSCP_RESPONSE

DESCRIPTION     Creates a RSCP response.


DEPENDENCIES
   None

RETURN VALUE
                ftm_rsp_pkt_type -- FTM packet to send via diag

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_wcdma_ber_create_rscp_response( ftm_wcdma_ber_pkt_type *cmd_ptr,
                                                     uint16 status,
                                                     uint16 cid,
                                                     uint16 scid,
                                                     ftm_wcdma_ber_rscp_rpt_type ftm_wcdma_ber_rscp_rpt)
{

  ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_wcdma_ber_rscp_rsp_pkt_type   *gen_rsp_payload;

  gen_rsp = ftmdiag_create_new_pkt( sizeof(ftm_wcdma_ber_rscp_rsp_pkt_type));

  if( gen_rsp.pkt_payload != NULL)
  {
    gen_rsp_payload = ( ftm_wcdma_ber_rscp_rsp_pkt_type *) gen_rsp.pkt_payload;

    gen_rsp_payload->header                  = cmd_ptr->header;
    gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_WCDMA_BER_GEN_RESPONSE;  // Need to check on this
    gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
    gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
    gen_rsp_payload->status                  = status;
    gen_rsp_payload->rscp_rpt                = ftm_wcdma_ber_rscp_rpt;
  }

  return gen_rsp;
}


/*===========================================================================

FUNCTION        FTM_WCDMA_BER_CONVERT_NXXX

DESCRIPTION     Converts a particular N counter from an enumerated type
                to a numerical value.  Value is clipped if passed in value
                exceeds max enumeration.


DEPENDENCIES
   None

RETURN VALUE
                uint16 -- converted enumeration

SIDE EFFECTS
   None.

===========================================================================*/
static uint16 ftm_wcdma_ber_convert_n312( ftm_wcdma_ber_n312_type  n312_coded)
{

  static const uint16 conv_table[] = { 1, 50, 100, 200, 400, 600, 800, 1000};

  if( n312_coded > FTM_WCDMA_BER_N312_s1000) /*lint !e685 */
  {
    n312_coded = FTM_WCDMA_BER_N312_s1000;
  }

  return( conv_table[ n312_coded]);

}

static uint16 ftm_wcdma_ber_convert_n313( ftm_wcdma_ber_n313_type  n313_coded)
{

  static const uint16 conv_table[] = { 1, 2, 4, 10, 20, 50, 100, 200};

  if( n313_coded > FTM_WCDMA_BER_N313_s200) /*lint !e685 */
  {
    n313_coded = FTM_WCDMA_BER_N313_s200;
  }

  return( conv_table[ n313_coded]);

}

static uint16 ftm_wcdma_ber_convert_n315( ftm_wcdma_ber_n315_type  n315_coded)
{

  static const uint16 conv_table[] = { 1, 50, 100, 200, 400, 600, 800, 1000};

  if( n315_coded > FTM_WCDMA_BER_N315_s1000) /*lint !e685 */
  {
      n315_coded = FTM_WCDMA_BER_N315_s1000;
  }

  return( conv_table[ n315_coded]);
}

/*===========================================================================

FUNCTION        FTM_WCDMA_BER_CREATE_GEN_RESPONSE

DESCRIPTION     Creates a generic pass/fail type of response for a given
                dispatched WCDMA command.


DEPENDENCIES
   None

RETURN VALUE
                ftm_rsp_pkt_type -- FTM packet to send via diag

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_wcdma_ber_create_gen_response( ftm_wcdma_ber_pkt_type *cmd_ptr, uint16 status,
                                                    uint16 cid, uint16 scid)
{

  ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_wcdma_ber_gen_rsp_pkt_type   *gen_rsp_payload;


  gen_rsp = ftmdiag_create_new_pkt( sizeof( ftm_wcdma_ber_gen_rsp_pkt_type));

  if( gen_rsp.pkt_payload != NULL)
  {
    gen_rsp_payload = ( ftm_wcdma_ber_gen_rsp_pkt_type *) gen_rsp.pkt_payload;

    gen_rsp_payload->header                  = cmd_ptr->header;
    gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_WCDMA_BER_GEN_RESPONSE;
    gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
    gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
    gen_rsp_payload->status                  = status;

  }

  return gen_rsp;
}

/*===========================================================================

FUNCTION        FTM_WCDMA_BER_CREATE_RX_LEV_RESPONSE

DESCRIPTION     Creates a rx level response for rx level request.


DEPENDENCIES
   None

RETURN VALUE
                ftm_rsp_pkt_type -- FTM packet to send via diag

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_wcdma_ber_create_rx_lev_response( ftm_wcdma_ber_pkt_type *cmd_ptr, uint16 status,
                                                    uint16 cid, uint16 scid, ftm_wcdma_ber_rx_level_rpt_type ftm_wcdma_ber_rx_lev_rpt)
{

  ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_wcdma_ber_rx_lev_rsp_pkt_type   *gen_rsp_payload;

  gen_rsp = ftmdiag_create_new_pkt( sizeof(ftm_wcdma_ber_rx_lev_rsp_pkt_type));

  if( gen_rsp.pkt_payload != NULL)
  {
    gen_rsp_payload = ( ftm_wcdma_ber_rx_lev_rsp_pkt_type *) gen_rsp.pkt_payload;

    gen_rsp_payload->header                  = cmd_ptr->header;
    gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_WCDMA_BER_GEN_RESPONSE;  // Need to check on this
    gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
    gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
    gen_rsp_payload->status                  = status;
    gen_rsp_payload->rx_lev_rpt              = ftm_wcdma_ber_rx_lev_rpt;
  }

  return gen_rsp;
}


/*===========================================================================

FUNCTION     FTM_WCDMA_BER_LOG_GEN_CNF

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed,
             but no payload needs to be sent.

DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_wcdma_ber_log_gen_cnf( ftm_log_wcdma_ber_id_type  id)
{

  #ifdef FTM_HAS_LOGGING
  ftm_log_wcdma_ber_gen_cnf        *gen_cnf_log;


  gen_cnf_log = ( ftm_log_wcdma_ber_gen_cnf *)
                  ftm_log_malloc( (uint16)FTM_LOG_WCDMA_BER,
                                  sizeof( ftm_log_wcdma_ber_gen_cnf) );

  if( gen_cnf_log != NULL)
  {
    gen_cnf_log->log_id = id;

    log_commit( gen_cnf_log);
  }
  #endif
}


/*===========================================================================

FUNCTION     FTM_WCDMA_BER_LOG_GEN_CNF_WITH_STATUS

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed, and
             pass a generic status byte to the user.

DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_wcdma_ber_log_gen_cnf_with_status( ftm_log_wcdma_ber_id_type  id, uint8 status)
{

  #ifdef FTM_HAS_LOGGING
  ftm_log_wcdma_ber_gen_cnf_with_status        *gen_cnf_log;

  gen_cnf_log = ( ftm_log_wcdma_ber_gen_cnf_with_status *)
                  ftm_log_malloc( (uint16)FTM_LOG_WCDMA_BER,
                                  sizeof( ftm_log_wcdma_ber_gen_cnf_with_status) );

  if( gen_cnf_log != NULL)
  {
    gen_cnf_log->log_id = id;
    gen_cnf_log->status = status;

    log_commit( gen_cnf_log);
  }
  #endif

}


/*===========================================================================

FUNCTION    FTM_GSM_BER_LOG_ACQ_BCCH_CNF

DESCRIPTION
            Logs a select specific bcch confirmation message to the user (PC
            side).  Payload is an indication if acquisition was successful,
            and the SCH data block.  SCH data will only be valid if the
            confirmation indicates the acquisition was successful.

DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_log_acq_cnf( boolean status, uint16 freq, uint16 scr_code)
{

  #ifdef FTM_HAS_LOGGING
  ftm_log_wcdma_ber_acq_cnf_type   *wcdma_ber_log;

  wcdma_ber_log = (ftm_log_wcdma_ber_acq_cnf_type *)
                  ftm_log_malloc( (uint16)FTM_LOG_WCDMA_BER,
                  sizeof( ftm_log_wcdma_ber_acq_cnf_type));

  if( wcdma_ber_log != NULL)
  {
    wcdma_ber_log->log_id   = FTM_LOG_WCDMA_BER_ACQ_CNF;
    wcdma_ber_log->status   = status;
    wcdma_ber_log->freq     = freq;
    wcdma_ber_log->scr_code = scr_code;

    log_commit( wcdma_ber_log);
  }
  #endif
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_ALLOCATE_UL_DB

DESCRIPTION    Sets up static database for handling all UL L1 configuration


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_allocate_ul_db( void)
{

  int i = 0;

  ftm_wcdma_ul_db.add_ul_phychan_db = &ftm_wcdma_ul_phychan_cfg;

  ftm_wcdma_ul_db.ul_cctrch_info    = &ftm_wcdma_ul_cctrch_cfg;

  for( i = 0; i < L1_UL_TRCH_MAX; i++)
  {
    ftm_wcdma_ul_db.ul_cctrch_info->semi_static_parms_ptr[i] = &ftm_wcdma_ul_semi_static_parms[i];
  }

  for( i = 0; i < L1_UL_TFC_MAX; i++)
  {
    ftm_wcdma_ul_db.ul_cctrch_info->gain_factor_parms_ptr[i] = &ftm_wcdma_ul_gain_factor_parms[i];
  }

  ftm_wcdma_ul_db.tfcs_ptr = &ftm_wcdma_ul_mac_tfcs;

  ftm_wcdma_ul_db.isInitialized = TRUE;

}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_FREE_UL_DB

DESCRIPTION


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_free_ul_db( void)
{

  ftm_wcdma_ul_db.isInitialized = FALSE;
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_ALLOCATE_DL_DB

DESCRIPTION    Sets up static database for handling all DL L1 configuration


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_allocate_dl_db( void)
{

  int i = 0;
  int j = 0;


  ftm_wcdma_dl_db.add_dl_phychan_db = &ftm_wcdma_dl_phychan_db;

  ftm_wcdma_dl_db.dl_cctrch_info    = &ftm_wcdma_dl_cctrch_info;

  for( i = 0; i < L1_MAX_TRCH; i++)
  {

    ftm_wcdma_dl_db.dl_cctrch_info->trch_info_array[i] = &ftm_wcdma_dl_trch[i];

    /* for each transport channel, allocate space for the array of Transport Format info structures */

    for( j = 0; j < L1_MAX_TF_PER_TRCH; j++)
    {
      ftm_wcdma_dl_db.dl_cctrch_info->trch_info_array[i]->tf_info_array[j] = &ftm_wcdma_dl_tf_info[i][j];
    }

  }

  /* allocate memory for the dl_ctfc_info structure */

  ftm_wcdma_dl_db.dl_ctfc_info      = &ftm_wcdma_dl_ctfc_info;

  /* for each tfc possible, allocate space for a l1_dl_ctfc_entry_info_struct_type */


  for( i = 0; i < L1_MAX_TFC; i++)
  {
    ftm_wcdma_dl_db.dl_ctfc_info->ctfc_entry_array[i] = &ftm_wcdma_ctfc_entry_info[i];

#ifndef FEATURE_RRC_L1_INF_USE_CTFC
    ftm_wcdma_dl_db.dl_ctfc_info->ctfc_entry_array[i]->tfi_table = &ftm_wcdma_tfi_table[i][0];
#endif
  }


  ftm_wcdma_dl_db.isInitialized = TRUE;
}


/*===========================================================================

FUNCTION     FTM_WCDMA_BER_FREE_DL_DB

DESCRIPTION  "Frees" static database for handling all DL L1 configuration


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_free_dl_db( void)
{
  ftm_wcdma_dl_db.isInitialized = FALSE;
}


/*===========================================================================

FUNCTION       FTM_WCDMA_DL_RX_DATA0

DESCRIPTION    Handles RLC PDUs placed on the FTM watermarks.  Sends a message
               to FTM so the PDU will be looped back.

               It is assumed that watermark 0 is assigned to DTCH data only.
               The loopback should make sure that DTCH data is constantly
               being sent to UL RLC.  The test boxes base BER calculation
               on the DTCH data only, and require a constant DL->UL delay.
               (Delay is handled at the MAC level)


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_dl_rx_data0( void)
{

  ftm_wcdma_ber_int_cmd_type          cmd;

  cmd.cmd_hdr.cmd_id                  = FTM_WCDMA_BER_INT_CMD_RX_DATA;
  cmd.msg.rx_data_stream_cmd.wm_id    = 0;

  ftm_wcdma_ber_send_int_cmd( &cmd);

}


/*===========================================================================

FUNCTION       FTM_WCDMA_DL_RX_DATA1

DESCRIPTION    Handles RLC PDUs placed on the FTM watermarks.  Sends a message
               to FTM so the PDU will be looped back.


               It is assumed that watermark 1 is assigned to DCCH data only.
               It is not required to loop DCCH data back.

DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_dl_rx_data1( void)
{

  ftm_wcdma_ber_int_cmd_type       cmd;

  cmd.cmd_hdr.cmd_id               = FTM_WCDMA_BER_INT_CMD_RX_DATA;
  cmd.msg.rx_data_stream_cmd.wm_id = 1;

  ftm_wcdma_ber_send_int_cmd( &cmd);

}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_INIT

DESCRIPTION    Initializes memory and data structures for FTM WCDMA BER
               functionality.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_init( void)
{

  int i;

  /* External message Q */
  (void)q_init( &ftm_wcdma_ber_q);

  /* Internal message Q..statically allocated, so uses q and free q */
  (void)q_init( &ftm_wcdma_ber_int_q);
  (void)q_init(&ftm_wcdma_ber_int_free_q);

  for( i = 0; i < 10; i++)
  {
    q_put( &ftm_wcdma_ber_int_free_q,
           q_link(  &ftm_wcdma_ber_int_cmd_buf[i],
                    &ftm_wcdma_ber_int_cmd_buf[i].cmd_hdr.link ));
  }


  /* Set up DL Watermark structures */

  for( i = 0; i < FTM_WCDMA_BER_MAX_NUM_LOGICAL_CHANNELS; i++)
  {

    /* Not sure if hardcoded numbers are adequate for high data rates.  Numbers
     * borrowed from TC code in NAS layer
     */

    dsm_queue_init(&ftm_wcdma_ber_dl_wm[i], 900 * 20, &ftm_wcdma_ber_dl_wm_q[i]);

    ftm_wcdma_ber_dl_wm[i].lo_watermark           = 900;
    ftm_wcdma_ber_dl_wm[i].hi_watermark           = 900 * 17;
    ftm_wcdma_ber_dl_wm[i].gone_empty_func_ptr    = NULL;
    ftm_wcdma_ber_dl_wm[i].lowater_func_ptr       = NULL;
    ftm_wcdma_ber_dl_wm[i].non_empty_func_ptr     = NULL;
    ftm_wcdma_ber_dl_wm[i].non_empty_func_data    = NULL;
    ftm_wcdma_ber_dl_wm[i].each_enqueue_func_ptr  = NULL;
    ftm_wcdma_ber_dl_wm[i].each_enqueue_func_data = NULL;
    ftm_wcdma_ber_dl_wm[i].hiwater_func_ptr       = NULL;
    ftm_wcdma_ber_dl_wm[i].current_cnt            = 0;

  }

  ftm_wcdma_ber_dl_wm[0].non_empty_func_ptr         = (wm_cb_type) ftm_wcdma_dl_rx_data0;
  ftm_wcdma_ber_dl_wm[1].non_empty_func_ptr         = (wm_cb_type) ftm_wcdma_dl_rx_data1;


  /* Set up UL Watermark structures */

  for( i = 0; i < FTM_WCDMA_BER_MAX_NUM_LOGICAL_CHANNELS; i++)
  {

    dsm_queue_init(&ftm_wcdma_ber_ul_wm[i], 900 * 20, &ftm_wcdma_ber_ul_wm_q[i]);

    ftm_wcdma_ber_ul_wm[i].lo_watermark           = 0;
    ftm_wcdma_ber_ul_wm[i].hi_watermark           = 900 * 17;
    ftm_wcdma_ber_ul_wm[i].gone_empty_func_ptr    = NULL;
    ftm_wcdma_ber_ul_wm[i].lowater_func_ptr       = NULL;
    ftm_wcdma_ber_ul_wm[i].non_empty_func_ptr     = NULL;
    ftm_wcdma_ber_ul_wm[i].each_enqueue_func_ptr  = NULL;
    ftm_wcdma_ber_ul_wm[i].hiwater_func_ptr       = NULL;
    ftm_wcdma_ber_ul_wm[i].current_cnt            = 0;

  }

  ftm_wcdma_dl_cctrch_info.cctrch_id = 1;

  /* This table remaps the TFCS table.  This is used as follows:
   * 0xN( DTCH), 0xN( DCCH) => ctfc = 0 => has TFCI stored in dch_config.tfcs_config[0]
   * 0xN( DTCH), 1xN( DCCH) => ctfc = 2 => has TFCI stored in dch_config.tfcs_config[1]
   * 1xN( DTCH), 0xN( DCCH) => ctfc = 1 => has TFCI stored in dch_config.tfcs_config[2]
   * 1xN( DTCH), 1xN( DCCH) => ctfc = 3 => has TFCI stored in dch_config.tfcs_config[3]
   *
   * The user may use an FTM command to change the value of the TFCS->TFCI mapping.  The
   * default mapping is the same as defined in the 3GPP spec.  However, boxes may use
   * a non-standard mapping so this can provide flexibility.  The same TFCS is used for UL and
   * DL
   */

  dch_config.ul_tfcs_config[0] = 0;
  dch_config.ul_tfcs_config[1] = 1;
  dch_config.ul_tfcs_config[2] = 2;
  dch_config.ul_tfcs_config[3] = 3;

  dch_config.dl_tfcs_config[0] = 0;
  dch_config.dl_tfcs_config[1] = 1;
  dch_config.dl_tfcs_config[2] = 2;
  dch_config.dl_tfcs_config[3] = 3;

  #ifdef FEATURE_HSDPA
  ftm_hsdpa_bler_init();
  #endif
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_CLEANUP

DESCRIPTION    This function is used to force a teardown of the WCDMA
               non-signaling mode.  If we are in DCH, or a DCH config is
               pending, then the RLC config must be torn down also.

               The IDLE request should automatically tear down HS channels
               if they are supported and enabled.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_cleanup( void)
{

  if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH                  ||
      FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH_PENDING          ||
      FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH_RECONFIG_PENDING
    )
  {

    /* De-register the RLC watermarks */
    ftm_wcdma_ber_send_CRLC_DL_DEREGISTER_REQ();
    ftm_wcdma_ber_send_CRLC_UL_DEREGISTER_REQ();

    /* Release the RLC logical channels */
    ftm_wcdma_ber_send_CRLC_DL_RELEASE_REQ();
    ftm_wcdma_ber_send_CRLC_UL_RELEASE_REQ();

    ftm_wcdma_ber_flush_dsm_buffers();
  }

  /* Set L1 to IDLE mode */
  ftm_wcdma_ber_send_CPHY_IDLE_REQ();

  /* Issue Stop WCDMA mode command */
  ftm_wcdma_ber_send_CPHY_STOP_WCDMA_MODE_REQ();
}



/*===========================================================================

FUNCTION       FTM_WCDMA_CONFIGURE_UL_RMC12p2

DESCRIPTION    Sets up Uplink L1 configuration for RMC 12.2 kbps channel.
               RMC12.2 is defined in 3GPP TS 25.101.  The user as a limited amount
               of configurability for this RMC loopback.


DEPENDENCIES
  none

===========================================================================*/
boolean ftm_wcdma_configure_UL_RMC12p2( void)
{

  l1_ul_phychan_cfg_struct_type  *ul_phychan;
  l1_ul_cctrch_cfg_struct_type   *ul_cctrch;
  mac_ul_tfcs_type               *tfcs;
  uint16                          counter;

  ul_phychan = ftm_wcdma_ul_db.add_ul_phychan_db;

  /* UL Phychan configuration */

  ul_phychan->max_tx_pwr                       = dch_config.ul_max_tx_pwr;
  ul_phychan->dpch_cfg.num_dpdch               = 1;
  ul_phychan->dpch_cfg.scrambling_code_type    = L1_UL_SC_LONG;
  ul_phychan->dpch_cfg.scrambling_code         = dch_config.ul_dpch_scr_code;
  ul_phychan->dpch_cfg.punct_lim               = 100;
  ul_phychan->dpch_cfg.fbi_len                 = L1_UL_FBI_LEN_0;
  ul_phychan->dpch_cfg.tfci_present            = TRUE;
  ul_phychan->dpch_cfg.sf_min                  = L1_SF_64;


  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.dpcch_pwr_offset = dch_config.dpcch_power_offset;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.pca              = dch_config.ul_pca;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.pcp_len          = 0;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.srb_delay        = 7;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.tpc_step_size    = dch_config.ul_tpc_step;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.valid            = TRUE;


  /* UL CCTrCH configuration */


  ul_cctrch = ftm_wcdma_ul_db.ul_cctrch_info;

  ul_cctrch->chan            = L1_UL_PHYCHAN_DPCH;
  ul_cctrch->filler_val      = L1_UL_RF_EQ_FILLER_1;

  ul_cctrch->num_trch                            = 2;
  ul_cctrch->semi_static_parms_ptr[0]->trch_id   = 6;
  ul_cctrch->semi_static_parms_ptr[0]->staticRM  = 256;
  ul_cctrch->semi_static_parms_ptr[0]->tti       = L1_TTI_20MS;
  ul_cctrch->semi_static_parms_ptr[0]->coding    = L1_UL_CODING_CONV_ONE_THIRD;

  ul_cctrch->semi_static_parms_ptr[1]->trch_id   = 10;
  ul_cctrch->semi_static_parms_ptr[1]->staticRM  = 256;
  ul_cctrch->semi_static_parms_ptr[1]->tti       = L1_TTI_40MS;
  ul_cctrch->semi_static_parms_ptr[1]->coding    = L1_UL_CODING_CONV_ONE_THIRD;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_12_2)
  {
    ul_cctrch->semi_static_parms_ptr[0]->crc       = L1_UL_CRC_0_BITS;
    ul_cctrch->semi_static_parms_ptr[1]->crc       = L1_UL_CRC_0_BITS;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_12_2_SYMMETRIC)
  {
    ul_cctrch->semi_static_parms_ptr[0]->crc       = L1_UL_CRC_16_BITS;
    ul_cctrch->semi_static_parms_ptr[1]->crc       = L1_UL_CRC_12_BITS;
  }


  ul_cctrch->num_tfci        = 4;

  ul_cctrch->gain_factor_parms_ptr[0]->valid_Pp_m = FALSE;
  ul_cctrch->gain_factor_parms_ptr[1]->valid_Pp_m = FALSE;
  ul_cctrch->gain_factor_parms_ptr[2]->valid_Pp_m = FALSE;
  ul_cctrch->gain_factor_parms_ptr[3]->valid_Pp_m = FALSE;

  ul_cctrch->gain_factor_parms_ptr[0]->algo      = L1_UL_GF_CALCULATED;
  ul_cctrch->gain_factor_parms_ptr[1]->algo      = L1_UL_GF_CALCULATED;
  ul_cctrch->gain_factor_parms_ptr[2]->algo      = L1_UL_GF_CALCULATED;
  ul_cctrch->gain_factor_parms_ptr[3]->algo      = L1_UL_GF_SIGNALLED;

  ul_cctrch->gain_factor_parms_ptr[0]->ref_tfc_id_valid = TRUE;
  ul_cctrch->gain_factor_parms_ptr[1]->ref_tfc_id_valid = TRUE;
  ul_cctrch->gain_factor_parms_ptr[2]->ref_tfc_id_valid = TRUE;
  ul_cctrch->gain_factor_parms_ptr[3]->ref_tfc_id_valid = TRUE;

  ul_cctrch->gain_factor_parms_ptr[0]->ref_tfc_id = 0;
  ul_cctrch->gain_factor_parms_ptr[1]->ref_tfc_id = 0;
  ul_cctrch->gain_factor_parms_ptr[2]->ref_tfc_id = 0;
  ul_cctrch->gain_factor_parms_ptr[3]->ref_tfc_id = 0;


  /* Set up the BetaC/BetaD for 4 TFCs.  Only explicitly set up the betaC,betaD
   * for the SIGNALLED case.  We should only use signalled for the max data rate
   * TFC, as a default for this FTM mode.
   */

  for( counter = 0; counter < ul_cctrch->num_tfci; counter++ )
  {
    if( ul_cctrch->gain_factor_parms_ptr[ counter]->algo == L1_UL_GF_SIGNALLED)
    {
      ul_cctrch->gain_factor_parms_ptr[ counter]->BetaC = dch_config.beta_c;
      ul_cctrch->gain_factor_parms_ptr[ counter]->BetaD = dch_config.beta_d;
    }
  }

  /* UL TFCS configuration */


  tfcs = ftm_wcdma_ul_db.tfcs_ptr;

  tfcs->nchan = 2;
  tfcs->ntfc  = ul_cctrch->num_tfci;

  tfcs->tfs_info[0].ntf                = 2;
  tfcs->tfs_info[0].tti_info           = L1_TTI_20MS;
  tfcs->tfs_info[0].tf_info[0].tf_flag = TRUE;
  tfcs->tfs_info[0].tf_info[1].tf_flag = TRUE;
  tfcs->tfs_info[0].tf_info[0].ntblks  = 0;
  tfcs->tfs_info[0].tf_info[1].ntblks  = 1;

  tfcs->tfs_info[1].tti_info           = L1_TTI_40MS;
  tfcs->tfs_info[1].ntf                = 2;
  tfcs->tfs_info[1].tf_info[0].tf_flag = TRUE;
  tfcs->tfs_info[1].tf_info[1].tf_flag = TRUE;
  tfcs->tfs_info[1].tf_info[0].ntblks  = 0;
  tfcs->tfs_info[1].tf_info[1].ntblks  = 1;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_12_2)
  {

    tfcs->tfs_info[0].tf_info[0].tb_size = 260;
    tfcs->tfs_info[0].tf_info[1].tb_size = 260;

    tfcs->tfs_info[1].tf_info[0].tb_size = 112;
    tfcs->tfs_info[1].tf_info[1].tb_size = 112;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_12_2_SYMMETRIC)
  {

    tfcs->tfs_info[0].tf_info[0].tb_size = 244;
    tfcs->tfs_info[0].tf_info[1].tb_size = 244;

    tfcs->tfs_info[1].tf_info[0].tb_size = 100;
    tfcs->tfs_info[1].tf_info[1].tb_size = 100;
  }

  /* Flag all transport format combinations as invalid, and setup the
  ** ones we want afterwards. */
  for (counter = 0; counter < UE_MAX_TFC; counter++)
  {
    tfcs->tfc_info[counter].control_flag = TFC_NOT_ALLOWED;
  }


  /* Remap the TFI combinations to TFCI value via the tfcs_config table */

  /* This set maps TF0,TF0 to TFCI value contained in dch_config.tfcs_config[0] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].ctfc               = 0;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].tfi[0]             = 0;       /* TF0 for DTCH, 0xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].tfi[1]             = 0;       /* TF0 for DCCH, 0xN */

  /* This set maps TF0,TF1 to TFCI value contained in dch_config.tfcs_config[1] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].ctfc               = 2;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].tfi[0]             = 0;       /* TF0 for DTCH, 0xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].tfi[1]             = 1;       /* TF1 for DCCH, 1xN */

  /* This set maps TF1,TF0 to TFCI value contained in dch_config.tfcs_config[2] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].ctfc               = 1;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].tfi[0]             = 1;       /* TF1 for DTCH, 1xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].tfi[1]             = 0;       /* TF0 for DCCH, 0xN*/

  /* This set maps TF0,TF0 to TFCI value contained in dch_config.tfcs_config[3] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].ctfc               = 3;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].tfi[0]             = 1;       /* TF1 for DTCH, 1xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].tfi[1]             = 1;       /* TF1 for DCCH, 1xN */

  return( TRUE);
}


/*===========================================================================

FUNCTION    FTM_WCDMA_CONFIGURE_DL_RMC12p2

DESCRIPTION   Sets up Downlink L1 configuration for RMC 12.2 kbps channel.
              RMC12.2 is defined in 3GPP TS 25.101.  The user as a limited amount
              of configurability for this RMC loopback.


DEPENDENCIES
  none

===========================================================================*/
boolean ftm_wcdma_configure_DL_RMC12p2( void)
{

  l1_dl_cctrch_info_struct_type     *dl_cctrch;
  l1_dl_trch_info_struct_type       *trch;
  l1_dl_phychan_db_struct_type      *dl_phychan;
  l1_dl_ctfc_info_struct_type       *ctfc_setup;

  if( !ftm_wcdma_dl_db.isInitialized )
  {
    return FALSE;
  }

  /* DL Physical Channel configuration */

  dl_phychan = ftm_wcdma_dl_db.add_dl_phychan_db;

  dl_phychan->dl_common.comp_mode_info             = NULL;
  dl_phychan->dl_common.default_dpch_offset        = 0;
  dl_phychan->dl_common.opt_parm_presence_bitmask  = ( DL_COMMON_INFO_DEFAULT_DPCH_OFFSET_PRESENT |
                                                       DL_COMMON_INFO_TX_DIV_MODE_PRESENT);
  dl_phychan->dl_common.default_dpch_offset        = 0;
  dl_phychan->dl_common.tx_div_mode                = L1_TX_DIV_NONE;

  dl_phychan->dl_common.dpch_common.opt_parm_present_mask = DL_DPCH_COMMON_INFO_DPCH_PWR_CTRL_INFO_PRESENT;
  dl_phychan->dl_common.dpch_common.dpc_info              = L1_DPC_MODE_SINGLE_TPC;
  dl_phychan->dl_common.dpch_common.num_pilot_bits        = 8;
  dl_phychan->dl_common.dpch_common.po3                   = 0;
  dl_phychan->dl_common.dpch_common.sf                    = L1_SF_128;
  dl_phychan->dl_common.dpch_common.tfci_exists           = TRUE;
  dl_phychan->dl_common.dpch_common.timing_ind            = L1_TIMING_IND_INIT;

  dl_phychan->num_rl                                                           = 1;
  dl_phychan->dl_per_rl[0].pri_cpich_scr_code                                  = dch_config.dl_pri_cpich_scr_code; //9;
  dl_phychan->dl_per_rl[0].dpch_per_rl.opt_parms_present_mask                  = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.use_pri_cpich                           = L1_USE_PRI_CPICH;
  dl_phychan->dl_per_rl[0].dpch_per_rl.frame_offset                            = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.tpc_idx                                 = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.num_dpch                                = 1;
  dl_phychan->dl_per_rl[0].dpch_per_rl.dl_chan_code[0].opt_parm_present_mask   = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.dl_chan_code[0].code_number             = dch_config.dl_chan_code;

  dl_phychan->sync_det.N312 = ftm_wcdma_ber_convert_n312( dch_config.n_312);
  dl_phychan->sync_det.N313 = ftm_wcdma_ber_convert_n313( dch_config.n_313);
  dl_phychan->sync_det.N315 = ftm_wcdma_ber_convert_n315( dch_config.n_315);
  dl_phychan->sync_det.T312 = dch_config.t_312;
  dl_phychan->sync_det.T313 = dch_config.t_312;


  /* DL CCTrCH configuration */

  dl_cctrch = ftm_wcdma_dl_db.dl_cctrch_info;

  dl_cctrch->num_trch          = 2;
  dl_cctrch->rm_method         = L1_DL_RM_FIXED_POS;
  dl_cctrch->trch_ref_type_bf  = L1_DL_TRCH_REFTYPE_DCH_BF;
  dl_cctrch->trch_enable_bf    = L1_DL_TRCH_REFTYPE_DCH_BF;
  dl_cctrch->num_trch          = 2;


  /* Setup TrCH 0 */

  trch                    = dl_cctrch->trch_info_array[0];
  trch->ref_type          = L1_DL_TRCH_DCH_TYPE;
  trch->trch_id           = 6;
  trch->coding_type       = L1_DL_CONV_1_BY_3_CODING;
  trch->crc_length        = L1_CRC_LENGTH_16;
  trch->rate_match_attrib = 256;
  trch->tti_value         = L1_TTI_20MS;
  trch->bler_quality_val  = -20;
  trch->num_tf            = 2;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_12_2)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_2;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_12_2_SYMMETRIC)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_1;
  }


  trch->tf_info_array[0]->num_tb  = 0;
  trch->tf_info_array[0]->tb_size = 244;
  trch->tf_info_array[0]->allowed = TRUE;

  trch->tf_info_array[1]->num_tb  = 1;
  trch->tf_info_array[1]->tb_size = 244;
  trch->tf_info_array[1]->allowed = TRUE;

  /* Setup TrCH 1 */

  trch                    = dl_cctrch->trch_info_array[1];
  trch->ref_type          = L1_DL_TRCH_DCH_TYPE;
  trch->trch_id           = 10;
  trch->coding_type       = L1_DL_CONV_1_BY_3_CODING;
  trch->crc_length        = L1_CRC_LENGTH_12;
  trch->rate_match_attrib = 256;
  trch->tti_value         = L1_TTI_40MS;
  trch->bler_quality_val  = -20;
  trch->num_tf            = 2;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_12_2)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_2;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_12_2_SYMMETRIC)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_1;
  }

  trch->tf_info_array[0]->num_tb  = 0;
  trch->tf_info_array[0]->tb_size = 100;
  trch->tf_info_array[0]->allowed = TRUE;

  trch->tf_info_array[1]->num_tb  = 1;
  trch->tf_info_array[1]->tb_size = 100;
  trch->tf_info_array[1]->allowed = TRUE;


  /* Setup Transport Format Combinations */

  ctfc_setup                                  = ftm_wcdma_dl_db.dl_ctfc_info;

  ctfc_setup->tfci_existence                  = TRUE;
  ctfc_setup->last_ctfc_entry_index           = 3;
  ctfc_setup->pdsch_info_valid                = FALSE;

  ctfc_setup->ctfc_entry_array[0]->valid_ctfc = TRUE;
  ctfc_setup->ctfc_entry_array[1]->valid_ctfc = TRUE;
  ctfc_setup->ctfc_entry_array[2]->valid_ctfc = TRUE;
  ctfc_setup->ctfc_entry_array[3]->valid_ctfc = TRUE;

   /* This set maps TF0,TF0 to TFCI value contained in dch_config.tfcs_config[0] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[0] ]->tfi_table[0] = 0;         /* TF0 for DTCH, 0xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[0] ]->tfi_table[1] = 0;         /* TF0 for DCCH, 0xN */

  /* This set maps TF0,TF1 to TFCI value contained in dch_config.tfcs_config[1] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[1] ]->tfi_table[0] = 0;         /* TF0 for DTCH, 0xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[1] ]->tfi_table[1] = 1;         /* TF1 for DCCH, 1xN */

  /* This set maps TF1,TF0 to TFCI value contained in dch_config.tfcs_config[1] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[2] ]->tfi_table[0] = 1;         /* TF1 for DTCH, 1xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[2] ]->tfi_table[1] = 0;         /* TF0 for DCCH, 0xN */

  /* This set maps TF1,TF1 to TFCI value contained in dch_config.tfcs_config[1] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[3] ]->tfi_table[0] = 1;         /* TF1 for DTCH, 0xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[3] ]->tfi_table[1] = 1;         /* TF1 for DCCH, 0xN */


  return( TRUE);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_CONFIGURE_UL_RMC64

DESCRIPTION    Sets up L1 configuration for RMC 64 kbps channel.


DEPENDENCIES
  none

===========================================================================*/
boolean ftm_wcdma_configure_UL_RMC64( void)
{

  l1_ul_phychan_cfg_struct_type  *ul_phychan;
  l1_ul_cctrch_cfg_struct_type   *ul_cctrch;
  mac_ul_tfcs_type               *tfcs;
  uint16                          counter;

  ul_phychan = ftm_wcdma_ul_db.add_ul_phychan_db;

  /* UL Phychan configuration */

  ul_phychan->max_tx_pwr                       = dch_config.ul_max_tx_pwr;
  ul_phychan->dpch_cfg.num_dpdch               = 1;
  ul_phychan->dpch_cfg.scrambling_code_type    = L1_UL_SC_LONG;
  ul_phychan->dpch_cfg.scrambling_code         = dch_config.ul_dpch_scr_code;
  ul_phychan->dpch_cfg.punct_lim               = 100;
  ul_phychan->dpch_cfg.fbi_len                 = L1_UL_FBI_LEN_0;
  ul_phychan->dpch_cfg.tfci_present            = TRUE;
  ul_phychan->dpch_cfg.sf_min                  = L1_SF_16;


  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.dpcch_pwr_offset = dch_config.dpcch_power_offset;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.pca              = dch_config.ul_pca;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.pcp_len          = 0;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.srb_delay        = 7;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.tpc_step_size    = dch_config.ul_tpc_step;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.valid            = TRUE;


  /* UL CCTrCH configuration */


  ul_cctrch = ftm_wcdma_ul_db.ul_cctrch_info;

  ul_cctrch->chan            = L1_UL_PHYCHAN_DPCH;
  ul_cctrch->filler_val      = L1_UL_RF_EQ_FILLER_1;

  ul_cctrch->num_trch                            = 2;
  ul_cctrch->semi_static_parms_ptr[0]->trch_id   = 6;
  ul_cctrch->semi_static_parms_ptr[0]->staticRM  = 256;
  ul_cctrch->semi_static_parms_ptr[0]->tti       = L1_TTI_20MS;
  ul_cctrch->semi_static_parms_ptr[0]->coding    = L1_UL_CODING_TURBO;

  ul_cctrch->semi_static_parms_ptr[1]->trch_id   = 10;
  ul_cctrch->semi_static_parms_ptr[1]->staticRM  = 256;
  ul_cctrch->semi_static_parms_ptr[1]->tti       = L1_TTI_40MS;
  ul_cctrch->semi_static_parms_ptr[1]->coding    = L1_UL_CODING_CONV_ONE_THIRD;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_64)
  {
    ul_cctrch->semi_static_parms_ptr[0]->crc       = L1_UL_CRC_0_BITS;
    ul_cctrch->semi_static_parms_ptr[1]->crc       = L1_UL_CRC_0_BITS;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_64_SYMMETRIC)
  {
    ul_cctrch->semi_static_parms_ptr[0]->crc       = L1_UL_CRC_16_BITS;
    ul_cctrch->semi_static_parms_ptr[1]->crc       = L1_UL_CRC_12_BITS;
  }


  ul_cctrch->num_tfci        = 4;

  ul_cctrch->gain_factor_parms_ptr[0]->valid_Pp_m = FALSE;
  ul_cctrch->gain_factor_parms_ptr[1]->valid_Pp_m = FALSE;
  ul_cctrch->gain_factor_parms_ptr[2]->valid_Pp_m = FALSE;
  ul_cctrch->gain_factor_parms_ptr[3]->valid_Pp_m = FALSE;

  ul_cctrch->gain_factor_parms_ptr[0]->algo      = L1_UL_GF_CALCULATED;
  ul_cctrch->gain_factor_parms_ptr[1]->algo      = L1_UL_GF_CALCULATED;
  ul_cctrch->gain_factor_parms_ptr[2]->algo      = L1_UL_GF_CALCULATED;
  ul_cctrch->gain_factor_parms_ptr[3]->algo      = L1_UL_GF_SIGNALLED;

  ul_cctrch->gain_factor_parms_ptr[0]->ref_tfc_id_valid = TRUE;
  ul_cctrch->gain_factor_parms_ptr[1]->ref_tfc_id_valid = TRUE;
  ul_cctrch->gain_factor_parms_ptr[2]->ref_tfc_id_valid = TRUE;
  ul_cctrch->gain_factor_parms_ptr[3]->ref_tfc_id_valid = TRUE;

  ul_cctrch->gain_factor_parms_ptr[0]->ref_tfc_id = 0;
  ul_cctrch->gain_factor_parms_ptr[1]->ref_tfc_id = 0;
  ul_cctrch->gain_factor_parms_ptr[2]->ref_tfc_id = 0;
  ul_cctrch->gain_factor_parms_ptr[3]->ref_tfc_id = 0;


  /* Set up the BetaC/BetaD for 4 TFCs.  Only explicitly set up the betaC,betaD
   * for the SIGNALLED case.  We should only use signalled for the max data rate
   * TFC, as a default for this FTM mode.
   */

  for( counter = 0; counter < ul_cctrch->num_tfci; counter++ )
  {
    if( ul_cctrch->gain_factor_parms_ptr[ counter]->algo == L1_UL_GF_SIGNALLED)
    {
      ul_cctrch->gain_factor_parms_ptr[ counter]->BetaC = dch_config.beta_c;
      ul_cctrch->gain_factor_parms_ptr[ counter]->BetaD = dch_config.beta_d;
    }
  }



  /* UL TFCS configuration */


  tfcs = ftm_wcdma_ul_db.tfcs_ptr;

  tfcs->nchan = 2;
  tfcs->ntfc  = 4;

  tfcs->tfs_info[0].ntf                = 2;
  tfcs->tfs_info[0].tti_info           = L1_TTI_20MS;
  tfcs->tfs_info[0].tf_info[0].tf_flag = TRUE;
  tfcs->tfs_info[0].tf_info[1].tf_flag = TRUE;
  tfcs->tfs_info[0].tf_info[0].ntblks  = 0;
  tfcs->tfs_info[0].tf_info[1].ntblks  = 1;

  tfcs->tfs_info[1].tti_info           = L1_TTI_40MS;
  tfcs->tfs_info[1].ntf                = 2;
  tfcs->tfs_info[1].tf_info[0].tf_flag = TRUE;
  tfcs->tfs_info[1].tf_info[1].tf_flag = TRUE;
  tfcs->tfs_info[1].tf_info[0].ntblks  = 0;
  tfcs->tfs_info[1].tf_info[1].ntblks  = 1;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_64)
  {

    tfcs->tfs_info[0].tf_info[0].tb_size = 1296;
    tfcs->tfs_info[0].tf_info[1].tb_size = 1296;

    tfcs->tfs_info[1].tf_info[0].tb_size = 112;
    tfcs->tfs_info[1].tf_info[1].tb_size = 112;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_64_SYMMETRIC)
  {

    tfcs->tfs_info[0].tf_info[0].tb_size = 1280;
    tfcs->tfs_info[0].tf_info[1].tb_size = 1280;

    tfcs->tfs_info[1].tf_info[0].tb_size = 100;
    tfcs->tfs_info[1].tf_info[1].tb_size = 100;
  }


  /* Flag all transport format combinations as invalid, and setup the
  ** ones we want afterwards. */
  for (counter = 0; counter < UE_MAX_TFC; counter++)
  {
    tfcs->tfc_info[counter].control_flag = TFC_NOT_ALLOWED;
  }

  /* Remap the TFI combinations to TFCI value via the tfcs_config table */

  /* This set maps TF0,TF0 to TFCI value contained in dch_config.tfcs_config[0] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].ctfc               = 0;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].tfi[0]             = 0;       /* TF0 for DTCH, 0xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].tfi[1]             = 0;       /* TF0 for DCCH, 0xN */

  /* This set maps TF0,TF1 to TFCI value contained in dch_config.tfcs_config[1] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].ctfc               = 2;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].tfi[0]             = 0;       /* TF0 for DTCH, 0xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].tfi[1]             = 1;       /* TF1 for DCCH, 1xN */

  /* This set maps TF1,TF0 to TFCI value contained in dch_config.tfcs_config[2] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].ctfc               = 1;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].tfi[0]             = 1;       /* TF1 for DTCH, 1xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].tfi[1]             = 0;       /* TF0 for DCCH, 0xN*/

  /* This set maps TF0,TF0 to TFCI value contained in dch_config.tfcs_config[3] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].ctfc               = 3;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].tfi[0]             = 1;       /* TF1 for DTCH, 1xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].tfi[1]             = 1;       /* TF1 for DCCH, 1xN */

  return( TRUE);
}


/*===========================================================================

FUNCTION    FTM_WCDMA_CONFIGURE_DL_RMC64

DESCRIPTION   Sets up L1 DL configuration for RMC 64 kbps.


DEPENDENCIES
  none

===========================================================================*/
boolean ftm_wcdma_configure_DL_RMC64( void)
{

  l1_dl_cctrch_info_struct_type     *dl_cctrch;
  l1_dl_trch_info_struct_type       *trch;
  l1_dl_phychan_db_struct_type      *dl_phychan;
  l1_dl_ctfc_info_struct_type       *ctfc_setup;

  if( !ftm_wcdma_dl_db.isInitialized )
  {
    return FALSE;
  }

  /* DL Physical Channel configuration */

  dl_phychan = ftm_wcdma_dl_db.add_dl_phychan_db;

  dl_phychan->dl_common.comp_mode_info             = NULL;
  dl_phychan->dl_common.default_dpch_offset        = 0;
  dl_phychan->dl_common.opt_parm_presence_bitmask  = ( DL_COMMON_INFO_DEFAULT_DPCH_OFFSET_PRESENT |
                                                       DL_COMMON_INFO_TX_DIV_MODE_PRESENT);
  dl_phychan->dl_common.default_dpch_offset        = 0;
  dl_phychan->dl_common.tx_div_mode                = L1_TX_DIV_NONE;

  dl_phychan->dl_common.dpch_common.opt_parm_present_mask = DL_DPCH_COMMON_INFO_DPCH_PWR_CTRL_INFO_PRESENT;
  dl_phychan->dl_common.dpch_common.dpc_info              = L1_DPC_MODE_SINGLE_TPC;
  dl_phychan->dl_common.dpch_common.num_pilot_bits        = 8;
  dl_phychan->dl_common.dpch_common.po3                   = 0;
  dl_phychan->dl_common.dpch_common.sf                    = L1_SF_32;
  dl_phychan->dl_common.dpch_common.tfci_exists           = TRUE;
  dl_phychan->dl_common.dpch_common.timing_ind            = L1_TIMING_IND_INIT;

  dl_phychan->num_rl                                                           = 1;
  dl_phychan->dl_per_rl[0].pri_cpich_scr_code                                  = dch_config.dl_pri_cpich_scr_code;
  dl_phychan->dl_per_rl[0].dpch_per_rl.opt_parms_present_mask                  = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.use_pri_cpich                           = L1_USE_PRI_CPICH;
  dl_phychan->dl_per_rl[0].dpch_per_rl.frame_offset                            = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.tpc_idx                                 = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.num_dpch                                = 1;
  dl_phychan->dl_per_rl[0].dpch_per_rl.dl_chan_code[0].opt_parm_present_mask   = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.dl_chan_code[0].code_number             = dch_config.dl_chan_code;

  dl_phychan->sync_det.N312 = ftm_wcdma_ber_convert_n312( dch_config.n_312);
  dl_phychan->sync_det.N313 = ftm_wcdma_ber_convert_n313( dch_config.n_313);
  dl_phychan->sync_det.N315 = ftm_wcdma_ber_convert_n315( dch_config.n_315);
  dl_phychan->sync_det.T312 = dch_config.t_312;
  dl_phychan->sync_det.T313 = dch_config.t_312;


  /* DL CCTrCH configuration */

  dl_cctrch = ftm_wcdma_dl_db.dl_cctrch_info;

  dl_cctrch->num_trch          = 2;
  dl_cctrch->rm_method         = L1_DL_RM_FIXED_POS;
  dl_cctrch->trch_ref_type_bf  = L1_DL_TRCH_REFTYPE_DCH_BF;
  dl_cctrch->trch_enable_bf    = L1_DL_TRCH_REFTYPE_DCH_BF;
  dl_cctrch->num_trch          = 2;


  /* Setup TrCH 0 */

  trch                    = dl_cctrch->trch_info_array[0];
  trch->ref_type          = L1_DL_TRCH_DCH_TYPE;
  trch->trch_id           = 6;
  trch->coding_type       = L1_DL_TURBO_CODING;
  trch->crc_length        = L1_CRC_LENGTH_16;
  trch->rate_match_attrib = 256;
  trch->tti_value         = L1_TTI_20MS;
  trch->bler_quality_val  = -20;
  trch->num_tf            = 2;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_64)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_2;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_64_SYMMETRIC)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_1;
  }

  trch->tf_info_array[0]->num_tb  = 0;
  trch->tf_info_array[0]->tb_size = 1280;
  trch->tf_info_array[0]->allowed = TRUE;
  trch->tf_info_array[1]->num_tb  = 1;
  trch->tf_info_array[1]->tb_size = 1280;
  trch->tf_info_array[1]->allowed = TRUE;

  /* Setup TrCH 1 */

  trch                    = dl_cctrch->trch_info_array[1];
  trch->ref_type          = L1_DL_TRCH_DCH_TYPE;
  trch->trch_id           = 10;
  trch->coding_type       = L1_DL_CONV_1_BY_3_CODING;
  trch->crc_length        = L1_CRC_LENGTH_12;
  trch->rate_match_attrib = 256;
  trch->tti_value         = L1_TTI_40MS;
  trch->bler_quality_val  = -20;
  trch->num_tf            = 2;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_64)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_2;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_64_SYMMETRIC)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_1;
  }

  trch->tf_info_array[0]->num_tb  = 0;
  trch->tf_info_array[0]->tb_size = 100;
  trch->tf_info_array[0]->allowed = TRUE;
  trch->tf_info_array[1]->num_tb  = 1;
  trch->tf_info_array[1]->tb_size = 100;
  trch->tf_info_array[1]->allowed = TRUE;


  /* Setup Transport Format Combinations */

  ctfc_setup                                  = ftm_wcdma_dl_db.dl_ctfc_info;

  ctfc_setup->tfci_existence                  = TRUE;
  ctfc_setup->last_ctfc_entry_index           = 3;
  ctfc_setup->pdsch_info_valid                = FALSE;

  ctfc_setup->ctfc_entry_array[0]->valid_ctfc = TRUE;
  ctfc_setup->ctfc_entry_array[1]->valid_ctfc = TRUE;
  ctfc_setup->ctfc_entry_array[2]->valid_ctfc = TRUE;
  ctfc_setup->ctfc_entry_array[3]->valid_ctfc = TRUE;

   /* This set maps TF0,TF0 to TFCI value contained in dch_config.tfcs_config[0] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[0] ]->tfi_table[0] = 0;         /* TF0 for DTCH, 0xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[0] ]->tfi_table[1] = 0;         /* TF0 for DCCH, 0xN */

  /* This set maps TF0,TF1 to TFCI value contained in dch_config.tfcs_config[1] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[1] ]->tfi_table[0] = 0;         /* TF0 for DTCH, 0xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[1] ]->tfi_table[1] = 1;         /* TF1 for DCCH, 1xN */

  /* This set maps TF1,TF0 to TFCI value contained in dch_config.tfcs_config[1] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[2] ]->tfi_table[0] = 1;         /* TF1 for DTCH, 1xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[2] ]->tfi_table[1] = 0;         /* TF0 for DCCH, 0xN */

  /* This set maps TF1,TF1 to TFCI value contained in dch_config.tfcs_config[1] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[3] ]->tfi_table[0] = 1;         /* TF1 for DTCH, 0xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[3] ]->tfi_table[1] = 1;         /* TF1 for DCCH, 0xN */

  return( TRUE);
}

/*===========================================================================

FUNCTION       FTM_WCDMA_CONFIGURE_UL_RMC384

DESCRIPTION    Sets up L1 configuration for RMC 384 kbps channel.


DEPENDENCIES
  none

===========================================================================*/
boolean ftm_wcdma_configure_UL_RMC384( void)
{

  l1_ul_phychan_cfg_struct_type  *ul_phychan;
  l1_ul_cctrch_cfg_struct_type   *ul_cctrch;
  mac_ul_tfcs_type               *tfcs;
  uint16                          counter;

  ul_phychan = ftm_wcdma_ul_db.add_ul_phychan_db;

  /* UL Phychan configuration */

  ul_phychan->max_tx_pwr                       = dch_config.ul_max_tx_pwr;
  ul_phychan->dpch_cfg.num_dpdch               = 1;
  ul_phychan->dpch_cfg.scrambling_code_type    = L1_UL_SC_LONG;
  ul_phychan->dpch_cfg.scrambling_code         = dch_config.ul_dpch_scr_code;
  ul_phychan->dpch_cfg.punct_lim               = 100;
  ul_phychan->dpch_cfg.fbi_len                 = L1_UL_FBI_LEN_0;
  ul_phychan->dpch_cfg.tfci_present            = TRUE;
  ul_phychan->dpch_cfg.sf_min                  = L1_SF_16;


  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.dpcch_pwr_offset = dch_config.dpcch_power_offset;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.pca              = dch_config.ul_pca;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.pcp_len          = 0;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.srb_delay        = 7;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.tpc_step_size    = dch_config.ul_tpc_step;
  ul_phychan->dpch_cfg.dpch_pwr_ctrl_info.valid            = TRUE;


  /* UL CCTrCH configuration */


  ul_cctrch = ftm_wcdma_ul_db.ul_cctrch_info;

  ul_cctrch->chan            = L1_UL_PHYCHAN_DPCH;
  ul_cctrch->filler_val      = L1_UL_RF_EQ_FILLER_1;

  ul_cctrch->num_trch                            = 2;
  ul_cctrch->semi_static_parms_ptr[0]->trch_id   = 6;
  ul_cctrch->semi_static_parms_ptr[0]->staticRM  = 256;
  ul_cctrch->semi_static_parms_ptr[0]->tti       = L1_TTI_20MS;
  ul_cctrch->semi_static_parms_ptr[0]->coding    = L1_UL_CODING_TURBO;

  ul_cctrch->semi_static_parms_ptr[1]->trch_id   = 10;
  ul_cctrch->semi_static_parms_ptr[1]->staticRM  = 256;
  ul_cctrch->semi_static_parms_ptr[1]->tti       = L1_TTI_40MS;
  ul_cctrch->semi_static_parms_ptr[1]->coding    = L1_UL_CODING_CONV_ONE_THIRD;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_384)
  {
    ul_cctrch->semi_static_parms_ptr[0]->crc       = L1_UL_CRC_0_BITS;
    ul_cctrch->semi_static_parms_ptr[1]->crc       = L1_UL_CRC_0_BITS;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_384_SYMMETRIC)
  {
    ul_cctrch->semi_static_parms_ptr[0]->crc       = L1_UL_CRC_16_BITS;
    ul_cctrch->semi_static_parms_ptr[1]->crc       = L1_UL_CRC_12_BITS;
  }


  ul_cctrch->num_tfci        = 4;

  ul_cctrch->gain_factor_parms_ptr[0]->valid_Pp_m = FALSE;
  ul_cctrch->gain_factor_parms_ptr[1]->valid_Pp_m = FALSE;
  ul_cctrch->gain_factor_parms_ptr[2]->valid_Pp_m = FALSE;
  ul_cctrch->gain_factor_parms_ptr[3]->valid_Pp_m = FALSE;

  ul_cctrch->gain_factor_parms_ptr[0]->algo      = L1_UL_GF_CALCULATED;
  ul_cctrch->gain_factor_parms_ptr[1]->algo      = L1_UL_GF_CALCULATED;
  ul_cctrch->gain_factor_parms_ptr[2]->algo      = L1_UL_GF_CALCULATED;
  ul_cctrch->gain_factor_parms_ptr[3]->algo      = L1_UL_GF_SIGNALLED;

  ul_cctrch->gain_factor_parms_ptr[0]->ref_tfc_id_valid = TRUE;
  ul_cctrch->gain_factor_parms_ptr[1]->ref_tfc_id_valid = TRUE;
  ul_cctrch->gain_factor_parms_ptr[2]->ref_tfc_id_valid = TRUE;
  ul_cctrch->gain_factor_parms_ptr[3]->ref_tfc_id_valid = TRUE;

  ul_cctrch->gain_factor_parms_ptr[0]->ref_tfc_id = 0;
  ul_cctrch->gain_factor_parms_ptr[1]->ref_tfc_id = 0;
  ul_cctrch->gain_factor_parms_ptr[2]->ref_tfc_id = 0;
  ul_cctrch->gain_factor_parms_ptr[3]->ref_tfc_id = 0;


  /* Set up the BetaC/BetaD for 4 TFCs.  Only explicitly set up the betaC,betaD
   * for the SIGNALLED case.  We should only use signalled for the max data rate
   * TFC, as a default for this FTM mode.
   */

  for( counter = 0; counter < ul_cctrch->num_tfci; counter++ )
  {
    if( ul_cctrch->gain_factor_parms_ptr[ counter]->algo == L1_UL_GF_SIGNALLED)
    {
      ul_cctrch->gain_factor_parms_ptr[ counter]->BetaC = dch_config.beta_c;
      ul_cctrch->gain_factor_parms_ptr[ counter]->BetaD = dch_config.beta_d;
    }
  }

  /* UL TFCS configuration */


  tfcs = ftm_wcdma_ul_db.tfcs_ptr;

  tfcs->nchan = 2;
  tfcs->ntfc  = 4;

  tfcs->tfs_info[0].ntf                = 2;
  tfcs->tfs_info[0].tti_info           = L1_TTI_20MS;
  tfcs->tfs_info[0].tf_info[0].tf_flag = TRUE;
  tfcs->tfs_info[0].tf_info[1].tf_flag = TRUE;
  tfcs->tfs_info[0].tf_info[0].ntblks  = 0;
  tfcs->tfs_info[0].tf_info[1].ntblks  = 1;

  tfcs->tfs_info[1].tti_info           = L1_TTI_40MS;
  tfcs->tfs_info[1].ntf                = 2;
  tfcs->tfs_info[1].tf_info[0].tf_flag = TRUE;
  tfcs->tfs_info[1].tf_info[1].tf_flag = TRUE;
  tfcs->tfs_info[1].tf_info[0].ntblks  = 0;
  tfcs->tfs_info[1].tf_info[1].ntblks  = 1;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_64)
  {

    tfcs->tfs_info[0].tf_info[0].tb_size = 3856;
    tfcs->tfs_info[0].tf_info[1].tb_size = 3856;

    tfcs->tfs_info[1].tf_info[0].tb_size = 112;
    tfcs->tfs_info[1].tf_info[1].tb_size = 112;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_64_SYMMETRIC)
  {

    tfcs->tfs_info[0].tf_info[0].tb_size = 3840;
    tfcs->tfs_info[0].tf_info[1].tb_size = 3840;

    tfcs->tfs_info[1].tf_info[0].tb_size = 100;
    tfcs->tfs_info[1].tf_info[1].tb_size = 100;
  }


  /* Flag all transport format combinations as invalid, and setup the
  ** ones we want afterwards. */
  for (counter = 0; counter < UE_MAX_TFC; counter++)
  {
    tfcs->tfc_info[counter].control_flag = TFC_NOT_ALLOWED;
  }

  /* Remap the TFI combinations to TFCI value via the tfcs_config table */

  /* This set maps TF0,TF0 to TFCI value contained in dch_config.tfcs_config[0] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].ctfc               = 0;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].tfi[0]             = 0;       /* TF0 for DTCH, 0xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[0] ].tfi[1]             = 0;       /* TF0 for DCCH, 0xN */

  /* This set maps TF0,TF1 to TFCI value contained in dch_config.tfcs_config[1] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].ctfc               = 2;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].tfi[0]             = 0;       /* TF0 for DTCH, 0xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[1] ].tfi[1]             = 1;       /* TF1 for DCCH, 1xN */

  /* This set maps TF1,TF0 to TFCI value contained in dch_config.tfcs_config[2] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].ctfc               = 1;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].tfi[0]             = 1;       /* TF1 for DTCH, 1xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[2] ].tfi[1]             = 0;       /* TF0 for DCCH, 0xN*/

  /* This set maps TF0,TF0 to TFCI value contained in dch_config.tfcs_config[3] */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].control_flag       = TFC_ALLOWED;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].ctfc               = 3;
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].tfi[0]             = 1;       /* TF1 for DTCH, 1xN */
  tfcs->tfc_info[ dch_config.ul_tfcs_config[3] ].tfi[1]             = 1;       /* TF1 for DCCH, 1xN */

  return( TRUE);
}


/*===========================================================================

FUNCTION    FTM_WCDMA_CONFIGURE_DL_RMC384

DESCRIPTION   Sets up L1 DL configuration for RMC 384 kbps.


DEPENDENCIES
  none

===========================================================================*/
boolean ftm_wcdma_configure_DL_RMC384( void)
{

  l1_dl_cctrch_info_struct_type     *dl_cctrch;
  l1_dl_trch_info_struct_type       *trch;
  l1_dl_phychan_db_struct_type      *dl_phychan;
  l1_dl_ctfc_info_struct_type       *ctfc_setup;

  if( !ftm_wcdma_dl_db.isInitialized )
  {
    return FALSE;
  }

  /* DL Physical Channel configuration */

  dl_phychan = ftm_wcdma_dl_db.add_dl_phychan_db;

  dl_phychan->dl_common.comp_mode_info             = NULL;
  dl_phychan->dl_common.default_dpch_offset        = 0;
  dl_phychan->dl_common.opt_parm_presence_bitmask  = ( DL_COMMON_INFO_DEFAULT_DPCH_OFFSET_PRESENT |
                                                       DL_COMMON_INFO_TX_DIV_MODE_PRESENT);
  dl_phychan->dl_common.default_dpch_offset        = 0;
  dl_phychan->dl_common.tx_div_mode                = L1_TX_DIV_NONE;

  dl_phychan->dl_common.dpch_common.opt_parm_present_mask = DL_DPCH_COMMON_INFO_DPCH_PWR_CTRL_INFO_PRESENT;
  dl_phychan->dl_common.dpch_common.dpc_info              = L1_DPC_MODE_SINGLE_TPC;
  dl_phychan->dl_common.dpch_common.num_pilot_bits        = 16;
  dl_phychan->dl_common.dpch_common.po3                   = 0;
  dl_phychan->dl_common.dpch_common.sf                    = L1_SF_8;
  dl_phychan->dl_common.dpch_common.tfci_exists           = TRUE;
  dl_phychan->dl_common.dpch_common.timing_ind            = L1_TIMING_IND_INIT;

  dl_phychan->num_rl                                                           = 1;
  dl_phychan->dl_per_rl[0].pri_cpich_scr_code                                  = dch_config.dl_pri_cpich_scr_code;
  dl_phychan->dl_per_rl[0].dpch_per_rl.opt_parms_present_mask                  = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.use_pri_cpich                           = L1_USE_PRI_CPICH;
  dl_phychan->dl_per_rl[0].dpch_per_rl.frame_offset                            = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.tpc_idx                                 = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.num_dpch                                = 1;
  dl_phychan->dl_per_rl[0].dpch_per_rl.dl_chan_code[0].opt_parm_present_mask   = 0;
  dl_phychan->dl_per_rl[0].dpch_per_rl.dl_chan_code[0].code_number             = dch_config.dl_chan_code;

  dl_phychan->sync_det.N312 = ftm_wcdma_ber_convert_n312( dch_config.n_312);
  dl_phychan->sync_det.N313 = ftm_wcdma_ber_convert_n313( dch_config.n_313);
  dl_phychan->sync_det.N315 = ftm_wcdma_ber_convert_n315( dch_config.n_315);
  dl_phychan->sync_det.T312 = dch_config.t_312;
  dl_phychan->sync_det.T313 = dch_config.t_312;


  /* DL CCTrCH configuration */

  dl_cctrch = ftm_wcdma_dl_db.dl_cctrch_info;

  dl_cctrch->num_trch          = 2;
  dl_cctrch->rm_method         = L1_DL_RM_FIXED_POS;
  dl_cctrch->trch_ref_type_bf  = L1_DL_TRCH_REFTYPE_DCH_BF;
  dl_cctrch->trch_enable_bf    = L1_DL_TRCH_REFTYPE_DCH_BF;
  dl_cctrch->num_trch          = 2;


  /* Setup TrCH 0 */

  trch                    = dl_cctrch->trch_info_array[0];
  trch->ref_type          = L1_DL_TRCH_DCH_TYPE;
  trch->trch_id           = 6;
  trch->coding_type       = L1_DL_TURBO_CODING;
  trch->crc_length        = L1_CRC_LENGTH_16;
  trch->rate_match_attrib = 256;
  trch->tti_value         = L1_TTI_10MS;
  trch->bler_quality_val  = -20;
  trch->num_tf            = 2;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_384)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_2;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_384_SYMMETRIC)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_1;
  }

  trch->tf_info_array[0]->num_tb  = 0;
  trch->tf_info_array[0]->tb_size = 3840;
  trch->tf_info_array[0]->allowed = TRUE;
  trch->tf_info_array[1]->num_tb  = 1;
  trch->tf_info_array[1]->tb_size = 3840;
  trch->tf_info_array[1]->allowed = TRUE;

  /* Setup TrCH 1 */

  trch                    = dl_cctrch->trch_info_array[1];
  trch->ref_type          = L1_DL_TRCH_DCH_TYPE;
  trch->trch_id           = 10;
  trch->coding_type       = L1_DL_CONV_1_BY_3_CODING;
  trch->crc_length        = L1_CRC_LENGTH_12;
  trch->rate_match_attrib = 256;
  trch->tti_value         = L1_TTI_40MS;
  trch->bler_quality_val  = -20;
  trch->num_tf            = 2;

  if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_384)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_2;
  }
  else if( dch_config.rmc_setup == FTM_WCDMA_BER_RMC_TYPE_384_SYMMETRIC)
  {
    trch->loopback_mode     = L1_LOOPBACK_MODE_1;
  }

  trch->tf_info_array[0]->num_tb  = 0;
  trch->tf_info_array[0]->tb_size = 100;
  trch->tf_info_array[0]->allowed = TRUE;
  trch->tf_info_array[1]->num_tb  = 1;
  trch->tf_info_array[1]->tb_size = 100;
  trch->tf_info_array[1]->allowed = TRUE;


  /* Setup Transport Format Combinations */

  ctfc_setup                                  = ftm_wcdma_dl_db.dl_ctfc_info;

  ctfc_setup->tfci_existence                  = TRUE;
  ctfc_setup->last_ctfc_entry_index           = 3;
  ctfc_setup->pdsch_info_valid                = FALSE;

  ctfc_setup->ctfc_entry_array[0]->valid_ctfc = TRUE;
  ctfc_setup->ctfc_entry_array[1]->valid_ctfc = TRUE;
  ctfc_setup->ctfc_entry_array[2]->valid_ctfc = TRUE;
  ctfc_setup->ctfc_entry_array[3]->valid_ctfc = TRUE;

   /* This set maps TF0,TF0 to TFCI value contained in dch_config.tfcs_config[0] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[0] ]->tfi_table[0] = 0;         /* TF0 for DTCH, 0xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[0] ]->tfi_table[1] = 0;         /* TF0 for DCCH, 0xN */

  /* This set maps TF0,TF1 to TFCI value contained in dch_config.tfcs_config[1] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[1] ]->tfi_table[0] = 0;         /* TF0 for DTCH, 0xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[1] ]->tfi_table[1] = 1;         /* TF1 for DCCH, 1xN */

  /* This set maps TF1,TF0 to TFCI value contained in dch_config.tfcs_config[1] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[2] ]->tfi_table[0] = 1;         /* TF1 for DTCH, 1xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[2] ]->tfi_table[1] = 0;         /* TF0 for DCCH, 0xN */

  /* This set maps TF1,TF1 to TFCI value contained in dch_config.tfcs_config[1] */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[3] ]->tfi_table[0] = 1;         /* TF1 for DTCH, 0xN */
  ctfc_setup->ctfc_entry_array[ dch_config.dl_tfcs_config[3] ]->tfi_table[1] = 1;         /* TF1 for DCCH, 0xN */

  return( TRUE);
}

/*===========================================================================

FUNCTION      FTM_WCDMA_BER_SEND_CPHY_START_WCDMA_MODE_REQ

DESCRIPTION   Sends a CPHY_START_WCDMA_MODE_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CPHY_START_WCDMA_MODE_REQ(void)
{
  l1_ext_cmd_type *cmd_ptr;

  cmd_ptr = l1_get_cmd_buf();

  cmd_ptr->cmd_hdr.cmd_id = CPHY_START_WCDMA_MODE_REQ;

  #ifdef FEATURE_APP_W2G
  /* Tell L1 to load both W and W2G app -- new on 7600 */
  cmd_ptr->cmd.start.mdsp_image = WL1_MDSP_IMAGE_WCDMA_GSM;
  #endif

  l1_put_cmd( cmd_ptr);
}


/*===========================================================================

FUNCTION     FTM_WCDMA_BER_SEND_CPHY_STOP_WCDMA_MODE_REQ

DESCRIPTION   Sends a CPHY_STOP_WCDMA_MODE_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CPHY_STOP_WCDMA_MODE_REQ(void)
{
  l1_ext_cmd_type *cmd_ptr;

  cmd_ptr = l1_get_cmd_buf();
  ASSERT(cmd_ptr != NULL);

  cmd_ptr->cmd_hdr.cmd_id = CPHY_STOP_WCDMA_MODE_REQ;
  cmd_ptr->cmd.stop.cause = L1_DEEP_SLEEP;


  l1_put_cmd( cmd_ptr);

  // wait for WL1 task response for CPHY_STOP_WCDMA_MODE_REQ cmd
  ftm_sig_rex_wait_no_clear(FTM_WCDMA_BER_CMD_Q_SIG);

  FTM_MSG_MED("CPHY_STOP_WCDMA_MODE received response",0,0,0);

}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CPHY_IDLE_REQ

DESCRIPTION    Sends a CPHY_IDLE_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CPHY_IDLE_REQ(void)
{

  l1_ext_cmd_type *cmd_ptr;

  cmd_ptr = l1_get_cmd_buf();

  cmd_ptr->cmd_hdr.cmd_id = CPHY_IDLE_REQ;

  l1_put_cmd( cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CPHY_ACQ_REQ

DESCRIPTION    Sends a CPHY_ACQ_REQ primitive to WL1 with the passed in
               acquisition parameters.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CPHY_ACQ_REQ( ftm_wcdma_ber_acquire_config_type *acq_config)
{

  l1_ext_cmd_type *cmd_ptr;

  cmd_ptr = l1_get_cmd_buf();

  cmd_ptr->cmd_hdr.cmd_id                 = CPHY_ACQ_REQ;

  cmd_ptr->cmd.acq.sttd_indicator_present = FALSE;
  cmd_ptr->cmd.acq.acq_mode               = acq_config->acq_mode;
  cmd_ptr->cmd.acq.acq_type               = acq_config->acq_type;
  cmd_ptr->cmd.acq.freq                   = acq_config->freq;

  #ifdef FEATURE_OPTIMIZE_SEARCH_FOR_HHO
  cmd_ptr->cmd.acq.num_rl                 = 1;
  cmd_ptr->cmd.acq.scr_code[0]            = acq_config->scr_code;
  #else
  cmd_ptr->cmd.acq.scr_code               = acq_config->scr_code;
  #endif /* FEATURE_OPTIMIZE_SEARCH_FOR_HHO */

  #ifdef FEATURE_BARRED_CELL_DURING_CELL_SELECTION
  cmd_ptr->cmd.acq.pn_pos                 = (uint32) acq_config->pn;
  #else
  cmd_ptr->cmd.acq.pn_offset              = (uint16) acq_config->pn;
  #endif

  #if defined(FEATURE_FLEXIBLE_DEEP_SLEEP) || defined(FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2)
  cmd_ptr->cmd.acq.agc                    = -103; /* SRCH_FREQ_SCAN_AGC_THRESH_FOR_SRV_REQ */
  #endif


  l1_put_cmd(cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_INIT_CPHY_SETUP_CMD

DESCRIPTION    Initializeds L1 setup command structure.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_init_cphy_setup_cmd( l1_setup_cmd_type *cmd, boolean signal_mac)
{
  cmd->dl_phychan                         = L1_DL_PHYCHAN_NOCHAN;
  cmd->dl_phychan_db                      = NULL;

  cmd->ul_phychan                         = L1_UL_PHYCHAN_NONE;
  cmd->ul_phychan_db                      = NULL;

  cmd->dl_phychan                         = L1_DL_PHYCHAN_NOCHAN;
  cmd->dl_phychan_db                      = ftm_wcdma_dl_db.add_dl_phychan_db;

  cmd->compressed_mode_info               = NULL;

  cmd->dl_cctrch_info                     = NULL;
  cmd->dl_ctfc_info                       = NULL;

  cmd->drop_dl_phychan                    = L1_DL_PHYCHAN_NOCHAN;
  cmd->drop_ul_phychan                    = L1_UL_PHYCHAN_NONE;

  cmd->freq_incl                          = FALSE;

  cmd->signal_dl_mac_at_act_time          = signal_mac;
  cmd->signal_ul_mac_at_act_time          = signal_mac;

  cmd->tfcs_ptr                           = NULL;
  cmd->ul_cctrch_info                     = NULL;

  cmd->req_mask                           = 0;
  cmd->hho_setup_mask                     = CPHY_HHO_NOT_A_HHO_INCL;

  #ifdef FEATURE_HSDPA
  cmd->hsdpa_l1_ops                       = HSDPA_SETUP_OPS_NOOP;
  cmd->hsdpa_l1_info                      = NULL;
  #endif

  #ifdef FEATURE_WCDMA_HSUPA
  cmd->mac_e_reset_indicator              = FALSE;
  cmd->e_req_mask                         = 0;
  cmd->e_info_ptr                         = NULL;
  #endif

  cmd->l1_cpc_drx_req_mask                = 0;    
  
  #ifdef FEATURE_WCDMA_DB_DC_HSDPA 
  cmd->sec_e_req_mask                     = 0; 
  #endif

}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_ADD_DL_CPHY_SETUP_CMD

DESCRIPTION    Adds a DL Phychan and DL CCTrCH configurations to an
               existing L1 setup command structure.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_add_dl_cphy_setup_cmd( l1_setup_cmd_type *cmd)
{
  cmd->dl_phychan                         = L1_DL_PHYCHAN_DPCH;
  cmd->dl_phychan_db                      = ftm_wcdma_dl_db.add_dl_phychan_db;

  cmd->dl_cctrch_info                     = ftm_wcdma_dl_db.dl_cctrch_info;
  cmd->dl_ctfc_info                       = ftm_wcdma_dl_db.dl_ctfc_info;

  cmd->req_mask                          |= ( CPHY_DL_PHYCHAN_ADD_INCL |
                                              CPHY_DL_CCTRCH_INCL
                                            );
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_ADD_UL_CPHY_SETUP_CMD

DESCRIPTION    Adds a UL Phychan and UL CCTrCH configurations to an
               existing L1 setup command structure.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_add_ul_cphy_setup_cmd( l1_setup_cmd_type *cmd)
{
  cmd->ul_phychan                         = L1_UL_PHYCHAN_DPCH;
  cmd->ul_phychan_db                      = ftm_wcdma_ul_db.add_ul_phychan_db;
  cmd->tfcs_ptr                           = ftm_wcdma_ul_db.tfcs_ptr;
  cmd->ul_cctrch_info                     = ftm_wcdma_ul_db.ul_cctrch_info;
  cmd->req_mask                          |= ( CPHY_UL_PHYCHAN_ADD_INCL |
                                              CPHY_UL_CCTRCH_INCL
                                            );
}


/*===========================================================================

FUNCTION      FTM_WCDMA_BER_DROP_DL_CPHY_SETUP_CMD

DESCRIPTION   Requests a PhyChan Removal, using an existing L1 setup command
              structure.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_drop_dl_cphy_setup_cmd( l1_setup_cmd_type *cmd)
{

  cmd->drop_dl_phychan                   = L1_DL_PHYCHAN_DPCH;
  cmd->req_mask                          |= CPHY_DL_PHYCHAN_DROP_INCL;
}


/*===========================================================================

FUNCTION      FTM_WCDMA_BER_DROP_UL_CPHY_SETUP_CMD

DESCRIPTION   Requests a PhyChan Removal, using an existing L1 setup command
              structure.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_drop_ul_cphy_setup_cmd( l1_setup_cmd_type *cmd)
{

  cmd->drop_ul_phychan                   = L1_UL_PHYCHAN_DPCH;
  cmd->req_mask                          |= CPHY_UL_PHYCHAN_DROP_INCL;
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CPHY_SETUP_REQ

DESCRIPTION    Sends a CPHY_SETUP_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CPHY_SETUP_REQ( l1_setup_cmd_type *cmd)
{

  l1_ext_cmd_type *cmd_ptr;

  cmd_ptr = l1_get_cmd_buf();

  cmd_ptr->cmd_hdr.cmd_id                 = CPHY_SETUP_REQ;

  cmd_ptr->cmd_hdr.act_time_type          = L1_ACTIVATION_TIME_NONE;
  cmd_ptr->cmd_hdr.act_time               = 0;

  cmd_ptr->cmd.setup                      = *cmd;

  l1_put_cmd( cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_ULDL_CPHY_SETUP_REQ

DESCRIPTION    Sets up and configures WL1 for UL and DL physical channel
               setups.  Uses the static FTM WCDMA BER database for actual
               configuration parameters.


DEPENDENCIES
               FTM WCDMA database needs to be set up with L1 configuration.

===========================================================================*/
void ftm_wcdma_ber_send_ULDL_CPHY_SETUP_REQ(void)
{

  l1_setup_cmd_type                       cmd;

  /* Clear cmd */
  memset(&cmd, 0, sizeof(l1_setup_cmd_type));
  /* Set call_type_mask */
  cmd.call_type_mask = CPHY_VOICE_CALL_ACTIVE;

  ftm_wcdma_ber_init_cphy_setup_cmd( &cmd, TRUE);
  ftm_wcdma_ber_add_dl_cphy_setup_cmd( &cmd);
  ftm_wcdma_ber_add_ul_cphy_setup_cmd( &cmd);
  ftm_wcdma_ber_send_CPHY_SETUP_REQ( &cmd);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_ULDL_DROP_CPHY_SETUP_REQ

DESCRIPTION    Drops the UL/DL Phychan setups.  Also forces HSDPA stopped
               if it is enabled.


DEPENDENCIES
               DCH should be active

===========================================================================*/
void ftm_wcdma_ber_send_ULDL_RECFG_CPHY_SETUP_REQ( void)
{

  l1_setup_cmd_type                       cmd;

  /* Clear cmd */
  memset(&cmd, 0, sizeof(l1_setup_cmd_type));
  /* Set call_type_mask */
  cmd.call_type_mask = CPHY_VOICE_CALL_ACTIVE;

  if( ftm_wcdma_dl_db.current_rf_chan_active != TRUE)
  {
    FTM_MSG_HIGH(" Attempting Reconfig from inactive RF Channel",0,0,0);
  }

  ftm_wcdma_ber_init_cphy_setup_cmd( &cmd, TRUE);

  /* drop the old configuration */

  ftm_wcdma_ber_drop_dl_cphy_setup_cmd( &cmd);
  ftm_wcdma_ber_drop_ul_cphy_setup_cmd( &cmd);

  /* Add the new configuration */

  ftm_wcdma_ber_add_dl_cphy_setup_cmd( &cmd);
  ftm_wcdma_ber_add_ul_cphy_setup_cmd( &cmd);

  /* If we are using a new frequency, need to send the HHO request */

  if( ftm_wcdma_dl_db.current_rf_chan != dch_config.rf_chan)
  {

     FTM_MSG_MED("Change in DCH ARFCN, attempting InterFreq HHO: arfcn %d to arfcn %d",
                 ftm_wcdma_dl_db.current_rf_chan,dch_config.rf_chan,0);

     cmd.freq_incl      = TRUE;
     cmd.rf_chan        = dch_config.rf_chan;
     cmd.hho_setup_mask = CPHY_HHO_INTERF_TI_HHO_INCL;

  }

  /* Request HSDPA channel stopped.  Function will check for an active
   * HS channel
   */
  #ifdef FEATURE_HSDPA
  (void) ftm_hsdpa_bler_request_stop_hs_channel( &cmd);
  #endif

  /* Send the request to L1 */

  ftm_wcdma_ber_send_CPHY_SETUP_REQ( &cmd);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_ASSIGN_CCTRCH_ID

DESCRIPTION    Used when reconfiguring a DCH channel.  This "allocates" a
               new CCTrCH ID based on the current ID in use.  The current one
               will get dropped while the new one gets added.


DEPENDENCIES
               DCH should be active

===========================================================================*/
void ftm_wcdma_ber_assign_cctrch_id( void)
{

  if( ftm_wcdma_dl_db.dl_cctrch_info->cctrch_id == 1)
  {
      ftm_wcdma_dl_db.dl_cctrch_info->cctrch_id = 2;
  }
  else if( ftm_wcdma_dl_db.dl_cctrch_info->cctrch_id == 2)
  {
      ftm_wcdma_dl_db.dl_cctrch_info->cctrch_id = 1;
  }

  FTM_MSG_MED("Assigned new CCTrCH ID: %d", ftm_wcdma_dl_db.dl_cctrch_info->cctrch_id,0,0);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CRLC_DL_REGISTER_REQ

DESCRIPTION    Registers a DL Watermark with RLC.  Assumes we are only concerned
               with RMC channels which have 2 logical channels (always use ids 0 and 1)


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CRLC_DL_REGISTER_REQ(void)
{

  l2_dl_cmd_type *l2_dl_cmd_ptr;

  if ((l2_dl_cmd_ptr = l2_dl_get_cmd_buf()) == NULL)
  {
      FTM_MSG_ERROR("NULL Ptr when sending CRLC Register SVC",0,0,0);
      return;
  }

  l2_dl_cmd_ptr->cmd_hdr.cmd_id = RLC_DL_REGISTER_SRVC_REQ;

  l2_dl_cmd_ptr->cmd_data.dl_reg.nchan = 2;

  l2_dl_cmd_ptr->cmd_data.dl_reg.rlc_data[0].context                   = TRUE;
  l2_dl_cmd_ptr->cmd_data.dl_reg.rlc_data[0].lc_id                     = 0;
  l2_dl_cmd_ptr->cmd_data.dl_reg.rlc_data[0].rlc_post_rx_func_ptr_para = NULL;
  l2_dl_cmd_ptr->cmd_data.dl_reg.rlc_data[0].rlc_post_rx_proc_func_ptr = NULL;
  l2_dl_cmd_ptr->cmd_data.dl_reg.rlc_data[0].dl_wm_ptr                 = &ftm_wcdma_ber_dl_wm[0];

  l2_dl_cmd_ptr->cmd_data.dl_reg.rlc_data[1].context                   = TRUE;
  l2_dl_cmd_ptr->cmd_data.dl_reg.rlc_data[1].lc_id                     = 1;
  l2_dl_cmd_ptr->cmd_data.dl_reg.rlc_data[1].rlc_post_rx_func_ptr_para = NULL;
  l2_dl_cmd_ptr->cmd_data.dl_reg.rlc_data[1].rlc_post_rx_proc_func_ptr = NULL;
  l2_dl_cmd_ptr->cmd_data.dl_reg.rlc_data[1].dl_wm_ptr                 = &ftm_wcdma_ber_dl_wm[1];


  l2_dl_put_cmd( l2_dl_cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CRLC_DL_DEREGISTER_REQ

DESCRIPTION    De-registers DL WM with RLC.  Assumes setup has been done
               with logical channel ids 0 and 1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CRLC_DL_DEREGISTER_REQ( void)
{

  l2_dl_cmd_type *l2_dl_cmd_ptr;

  if ((l2_dl_cmd_ptr = l2_dl_get_cmd_buf()) == NULL)
  {
      FTM_MSG_ERROR("NULL Ptr when sending CRLC Register SVC",0,0,0);
      return;
  }

  l2_dl_cmd_ptr->cmd_hdr.cmd_id = RLC_DL_DEREGISTER_SRVC_REQ;

  l2_dl_cmd_ptr->cmd_data.dl_dereg.nchan     = 2;
  l2_dl_cmd_ptr->cmd_data.dl_dereg.rlc_id[0] = 0;
  l2_dl_cmd_ptr->cmd_data.dl_dereg.rlc_id[1] = 1;

  l2_dl_put_cmd( l2_dl_cmd_ptr);
}



/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CRLC_UL_REGISTER_REQ

DESCRIPTION    Registers a UL Watermark with RLC.  Assumes we are only concerned
               with RMC channels which have 2 logical channels (always use ids 0 and 1)



DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CRLC_UL_REGISTER_REQ(void)
{

  l2_ul_cmd_type *l2_ul_cmd_ptr;

  if ((l2_ul_cmd_ptr = l2_ul_get_cmd_buf()) == NULL)
  {
      FTM_MSG_ERROR("NULL Ptr when sending CRLC Register SVC",0,0,0);
      return;
  }

  l2_ul_cmd_ptr->cmd_hdr.cmd_id = RLC_UL_REGISTER_SRVC_REQ;

  l2_ul_cmd_ptr->cmd_data.ul_reg.nchan = 2;

  l2_ul_cmd_ptr->cmd_data.ul_reg.rlc_data[0].lc_id     = 0;
  l2_ul_cmd_ptr->cmd_data.ul_reg.rlc_data[1].lc_id     = 1;
  l2_ul_cmd_ptr->cmd_data.ul_reg.rlc_data[0].ul_wm_ptr = &ftm_wcdma_ber_ul_wm[0];
  l2_ul_cmd_ptr->cmd_data.ul_reg.rlc_data[1].ul_wm_ptr = &ftm_wcdma_ber_ul_wm[1];


  l2_ul_put_cmd( l2_ul_cmd_ptr);

}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CRLC_UL_DEREGISTER_REQ

DESCRIPTION    De-registers UL WM with RLC.  Assumes setup has been done
               with logical channel ids 0 and 1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CRLC_UL_DEREGISTER_REQ( void)
{

  l2_ul_cmd_type *l2_ul_cmd_ptr;

  if ((l2_ul_cmd_ptr = l2_ul_get_cmd_buf()) == NULL)
  {
      FTM_MSG_ERROR("NULL Ptr when sending CRLC Register SVC",0,0,0);
      return;
  }

  l2_ul_cmd_ptr->cmd_hdr.cmd_id = RLC_UL_DEREGISTER_SRVC_REQ;

  l2_ul_cmd_ptr->cmd_data.ul_dereg.nchan = 2;

  l2_ul_cmd_ptr->cmd_data.ul_dereg.rlc_id[0] = 0;
  l2_ul_cmd_ptr->cmd_data.ul_dereg.rlc_id[1] = 1;

  l2_ul_put_cmd(l2_ul_cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CRLC_DL_CONFIG_REQ

DESCRIPTION    Establish DL Logical channels with RLC using Transparent Mode.
               Assumes we only use RMC channels with 2 logical channels, and
               only use logical channel ids 0 and 1.

DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CRLC_DL_CONFIG_REQ(void)
{

  wcdma_l2_dl_cmd_type *l2_dl_cmd_ptr;

  if ((l2_dl_cmd_ptr = wcdma_l2_dl_get_cmd_buf()) == NULL)
  {

    FTM_MSG_ERROR("NULL Ptr when sending CRLC_DL_CONFIG_REQ",0,0,0);
    return;
  }

  l2_dl_cmd_ptr->cmd_hdr.cmd_id = CRLC_DL_TM_CONFIG_REQ;

  l2_dl_cmd_ptr->cmd_data.dl_tm_config.nchan = 2;

  l2_dl_cmd_ptr->cmd_data.dl_tm_config.chan_info[0].seg_ind       = FALSE;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.chan_info[0].lc_id         = 0;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.chan_info[0].lc_type       = UE_LOGCHAN_DTCH;

  l2_dl_cmd_ptr->cmd_data.dl_tm_config.chan_info[1].seg_ind       = FALSE;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.chan_info[1].lc_id         = 1;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.chan_info[1].lc_type       = UE_LOGCHAN_DCCH;

  l2_dl_cmd_ptr->cmd_data.dl_tm_config.dl_tm_act[0].act_incl = FALSE;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.dl_tm_act[0].action   = RLC_ESTABLISH;

  l2_dl_cmd_ptr->cmd_data.dl_tm_config.dl_tm_act[1].act_incl = FALSE;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.dl_tm_act[1].action   = RLC_ESTABLISH;

  wcdma_l2_dl_put_cmd( l2_dl_cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CRLC_DL_RELEASE_REQ

DESCRIPTION    Release Logical channels previously established.  Assumes only
               use logical channels 0 and 1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CRLC_DL_RELEASE_REQ( void)
{

  wcdma_l2_dl_cmd_type *l2_dl_cmd_ptr;

  if ((l2_dl_cmd_ptr = wcdma_l2_dl_get_cmd_buf()) == NULL)
  {
    FTM_MSG_ERROR("NULL Ptr when sending CRLC_DL_CONFIG_REQ",0,0,0);
    return;
  }

  l2_dl_cmd_ptr->cmd_hdr.cmd_id = CRLC_DL_TM_CONFIG_REQ;

  l2_dl_cmd_ptr->cmd_data.dl_tm_config.nchan = 2;


  l2_dl_cmd_ptr->cmd_data.dl_tm_config.chan_info[0].lc_id         = 0;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.chan_info[0].lc_type       = UE_LOGCHAN_DTCH;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.dl_tm_act[0].act_incl      = FALSE;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.dl_tm_act[0].action        = RLC_RELEASE;

  l2_dl_cmd_ptr->cmd_data.dl_tm_config.chan_info[1].lc_id         = 1;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.chan_info[1].lc_type       = UE_LOGCHAN_DCCH;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.dl_tm_act[1].act_incl      = FALSE;
  l2_dl_cmd_ptr->cmd_data.dl_tm_config.dl_tm_act[1].action        = RLC_RELEASE;

  wcdma_l2_dl_put_cmd( l2_dl_cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CRLC_UL_CONFIG_REQ

DESCRIPTION    Establish UL Logical channels with RLC using Transparent Mode.
               Assumes we only use RMC channels with 2 logical channels, and
               only use logical channel ids 0 and 1, which always map to DTCH and DCCH
               respectively.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CRLC_UL_CONFIG_REQ(void)
{

  wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr;

  if ((l2_ul_cmd_ptr = wcdma_l2_ul_get_cmd_buf()) == NULL)
  {
    FTM_MSG_ERROR("NULL Ptr when sending CMAC_UL_CONFIG_REQ",0,0,0);
    return;
  }

  l2_ul_cmd_ptr->cmd_hdr.cmd_id = CRLC_UL_TM_CONFIG_REQ;

  l2_ul_cmd_ptr->cmd_data.ul_tm_config.nchan = 2;

  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[0].seg_ind       = FALSE;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[0].timer_discard = 1000;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[0].lc_id         = 0;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[0].lc_type       = UE_LOGCHAN_DTCH;

  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[1].seg_ind       = FALSE;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[1].timer_discard = 1000;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[1].lc_id         = 1;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[1].lc_type       = UE_LOGCHAN_DCCH;

  l2_ul_cmd_ptr->cmd_data.ul_tm_config.ul_tm_act[0].act_incl = FALSE;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.ul_tm_act[0].action   = RLC_ESTABLISH;

  l2_ul_cmd_ptr->cmd_data.ul_tm_config.ul_tm_act[1].act_incl = FALSE;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.ul_tm_act[1].action   = RLC_ESTABLISH;


  wcdma_l2_ul_put_cmd( l2_ul_cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CRLC_UL_RELEASE_REQ

DESCRIPTION    Release Logical channels previously established.  Assumes only
               use logical channels 0 and 1, which always map to DTCH and DCCH
               respectively.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CRLC_UL_RELEASE_REQ( void)
{

  wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr;

  if ((l2_ul_cmd_ptr = wcdma_l2_ul_get_cmd_buf()) == NULL)
  {
    FTM_MSG_ERROR("NULL Ptr when sending CMAC_UL_CONFIG_REQ",0,0,0);
    return;
  }

  l2_ul_cmd_ptr->cmd_hdr.cmd_id = CRLC_UL_TM_CONFIG_REQ;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.nchan = 2;

  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[0].lc_id         = 0;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[0].lc_type       = UE_LOGCHAN_DTCH;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.ul_tm_act[0].act_incl      = FALSE;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.ul_tm_act[0].action        = RLC_RELEASE;

  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[1].lc_id         = 1;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.chan_info[1].lc_type       = UE_LOGCHAN_DCCH;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.ul_tm_act[1].act_incl      = FALSE;
  l2_ul_cmd_ptr->cmd_data.ul_tm_config.ul_tm_act[1].action        = RLC_RELEASE;


  wcdma_l2_ul_put_cmd( l2_ul_cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CMAC_UL_CONFIG_REQ

DESCRIPTION    Configures the UL MAC for processing RMC channels in Loopback
               Mode.  This sets the DTCH priority above DCCH.  This is
               not realistic for a fullstack scenario, but for FTM RMC
               loopback we should prioritize data.  DCCH doesn't carry
               any useful data in FTM mode.  However, FTM may be looping
               back both DTCH and DCCH.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CMAC_UL_CONFIG_REQ(void)
{

  wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr;
  int i = 0;

  /* Fill in the MAC uplink parameters and send the primitive */
  if ((l2_ul_cmd_ptr = wcdma_l2_ul_get_cmd_buf()) == NULL)
  {
    FTM_MSG_ERROR("NULL Ptr when sending CMAC_UL_CONFIG_REQ",0,0,0);
    return;
  }

  /* Fill in the Command Header */
  l2_ul_cmd_ptr->cmd_hdr.cmd_id = CMAC_UL_CONFIG_REQ;

  l2_ul_cmd_ptr->cmd_data.mac_config_info.mac_config_changed = MAC_CFG_CHANGED;
  l2_ul_cmd_ptr->cmd_data.mac_config_info.wait_for_l1_sig    = TRUE;


  for( i = 0; i < MAX_CN_DOMAIN; i++)
  {
    l2_ul_cmd_ptr->cmd_data.mac_config_info.cipher_info[i].cipher_enable = FALSE;
  }

  #ifdef FEATURE_WCDMA_HSUPA
  l2_ul_cmd_ptr->cmd_data.mac_config_info.mac_e_reset_indicator = FALSE;
  l2_ul_cmd_ptr->cmd_data.mac_config_info.mac_eul_action        = MAC_EUL_NOOP;
  l2_ul_cmd_ptr->cmd_data.mac_config_info.mac_e_config_ptr      = NULL;
  #endif

  ftm_wcdma_ul_cctrch_config_type.cctrch_type                                   = DCH;
  ftm_wcdma_ul_cctrch_config_type.dch_info.ndchs                                = 2;
  ftm_wcdma_ul_cctrch_config_type.dch_info.loopback_test_mode                   = MAC_LOOPBACK_MODE_2;

  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].cs_or_ps_domain          = CS_DOMAIN;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].cctrch_id                = ftm_wcdma_dl_db.dl_cctrch_info->cctrch_id;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].trch_id                  = 6;

  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].ndlchan                                    = 1;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].dlchan_info[0].chan_type                   = UE_LOGCHAN_DTCH;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].dlchan_info[0].mac_id                      = 1;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].dlchan_info[0].priority                    = 1;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].dlchan_info[0].rb_id                       = 0;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].dlchan_info[0].rlc_id                      = 0;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].dlchan_info[0].rlc_mode                    = UE_MODE_TRANSPARENT;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[0].dlchan_info[0].rlc_size_info.all_tfi_valid = TRUE;

  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].cs_or_ps_domain          = CS_DOMAIN;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].cctrch_id                = ftm_wcdma_dl_db.dl_cctrch_info->cctrch_id;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].trch_id                  = 10;

  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].ndlchan                                    = 1;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].dlchan_info[0].chan_type                   = UE_LOGCHAN_DCCH;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].dlchan_info[0].mac_id                      = 2;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].dlchan_info[0].priority                    = 1;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].dlchan_info[0].rb_id                       = 1;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].dlchan_info[0].rlc_id                      = 1;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].dlchan_info[0].rlc_mode                    = UE_MODE_TRANSPARENT;
  ftm_wcdma_ul_cctrch_config_type.dch_info.dch_info[1].dlchan_info[0].rlc_size_info.all_tfi_valid = TRUE;

  l2_ul_cmd_ptr->cmd_data.mac_config_info.cctrch_ptr = &ftm_wcdma_ul_cctrch_config_type;
  l2_ul_cmd_ptr->cmd_data.mac_config_info.tfcs_ptr   = ftm_wcdma_ul_db.tfcs_ptr;

  #ifdef FEATURE_TFCC_ENHNC
  l2_ul_cmd_ptr->cmd_data.mac_config_info.new_tfcs_list_present = TRUE;
  #endif

  wcdma_l2_ul_put_cmd(l2_ul_cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CMAC_UL_CONFIG_LOOPBACK_REQ

DESCRIPTION    This function turns on the loopback at the MAC layer.  MAC
               only looks at specific fields in the config.  Make sure
               these fields are set correctly (cctrch_type and
               loopback_test_mode).


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CMAC_UL_CONFIG_LOOPBACK_REQ(void)
{

  wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr;

  /* Fill in the MAC uplink parameters and send the primitive */
  if ((l2_ul_cmd_ptr = wcdma_l2_ul_get_cmd_buf()) == NULL)
  {
    FTM_MSG_ERROR("NULL Ptr when sending CMAC_UL_CONFIG_REQ",0,0,0);
    return;
  }

  /* Fill in the Command Header */
  l2_ul_cmd_ptr->cmd_hdr.cmd_id = CMAC_UL_CONFIG_REQ;

  l2_ul_cmd_ptr->cmd_data.mac_config_info.mac_config_changed = MAC_LB_CFG_CHANGED;
  l2_ul_cmd_ptr->cmd_data.mac_config_info.wait_for_l1_sig    = FALSE;

  #ifdef FEATURE_WCDMA_HSUPA
  l2_ul_cmd_ptr->cmd_data.mac_config_info.mac_e_reset_indicator = FALSE;
  l2_ul_cmd_ptr->cmd_data.mac_config_info.mac_eul_action        = MAC_EUL_NOOP;
  l2_ul_cmd_ptr->cmd_data.mac_config_info.mac_e_config_ptr      = NULL;
  #endif

  ftm_wcdma_ul_cctrch_config_type.cctrch_type                                   = DCH;
  ftm_wcdma_ul_cctrch_config_type.dch_info.loopback_test_mode                   = MAC_LOOPBACK_MODE_2;

  l2_ul_cmd_ptr->cmd_data.mac_config_info.cctrch_ptr = &ftm_wcdma_ul_cctrch_config_type;

  wcdma_l2_ul_put_cmd(l2_ul_cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_CMAC_DL_CONFIG_REQ

DESCRIPTION    Configures the DL MAC for processing RMC channels in Loopback
               Mode.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_CMAC_DL_CONFIG_REQ(void)
{

  wcdma_l2_dl_cmd_type  *l2_dl_cmd_ptr;
  int i;

  /* Fill in the MAC uplink parameters and send the primitive */
  if ((l2_dl_cmd_ptr = wcdma_l2_dl_get_cmd_buf()) == NULL)
  {
    FTM_MSG_ERROR("NULL Ptr when sending CMAC_DL_CONFIG_REQ",0,0,0);
    return;
  }

  /* Fill in the Command Header */
  l2_dl_cmd_ptr->cmd_hdr.cmd_id = CMAC_DL_CONFIG_REQ;

  #ifdef FEATURE_MODEM_MBMS
  #error code not present
#endif

  l2_dl_cmd_ptr->cmd_data.mac_config_info.wait_for_l1_sig = TRUE;


  l2_dl_cmd_ptr->cmd_data.mac_config_info.num_bchs        = 0;
  l2_dl_cmd_ptr->cmd_data.mac_config_info.num_dschs       = 0;
  l2_dl_cmd_ptr->cmd_data.mac_config_info.num_fachs       = 0;
  l2_dl_cmd_ptr->cmd_data.mac_config_info.num_pchs        = 0;

  #ifdef FEATURE_HSDPA
  l2_dl_cmd_ptr->cmd_data.mac_config_info.mac_hsdpa_action       = HSDPA_NOOP;
  l2_dl_cmd_ptr->cmd_data.mac_config_info.mac_hs_reset_indicator = FALSE;
  #endif

  for( i = 0; i < MAX_CN_DOMAIN; i++)
  {

    l2_dl_cmd_ptr->cmd_data.mac_config_info.cipher_info[i].cipher_enable = FALSE;
  }

  l2_dl_cmd_ptr->cmd_data.mac_config_info.num_dchs        = 2;
  l2_dl_cmd_ptr->cmd_data.mac_config_info.dch_info[0]     = &ftm_wcdma_dl_dch_trch_config[0];
  l2_dl_cmd_ptr->cmd_data.mac_config_info.dch_info[1]     = &ftm_wcdma_dl_dch_trch_config[1];
  l2_dl_cmd_ptr->cmd_data.mac_config_info.fach_info[0]    = &ftm_wcdma_dl_fach_trch_config[0];
  l2_dl_cmd_ptr->cmd_data.mac_config_info.dsch_info[0]    = &ftm_wcdma_dl_dsch_trch_config[0];

  ftm_wcdma_dl_dch_trch_config[0].loopback_test_mode          = MAC_LOOPBACK_MODE_2;
  ftm_wcdma_dl_dch_trch_config[0].cctrch_id                   = ftm_wcdma_dl_db.dl_cctrch_info->cctrch_id;
  ftm_wcdma_dl_dch_trch_config[0].cs_or_ps_domain             = CS_DOMAIN;
  ftm_wcdma_dl_dch_trch_config[0].ndlchan                     = 1;
  ftm_wcdma_dl_dch_trch_config[0].trch_id                     = 6;
  ftm_wcdma_dl_dch_trch_config[0].dlchan_info[0].chan_type    = UE_LOGCHAN_DTCH;
  ftm_wcdma_dl_dch_trch_config[0].dlchan_info[0].mac_id       = 1;
  ftm_wcdma_dl_dch_trch_config[0].dlchan_info[0].rb_id        = 0;
  ftm_wcdma_dl_dch_trch_config[0].dlchan_info[0].rlc_id       = 0;
  ftm_wcdma_dl_dch_trch_config[0].dlchan_info[0].rlc_mode     = UE_MODE_TRANSPARENT;

  ftm_wcdma_dl_dch_trch_config[1].loopback_test_mode          = MAC_LOOPBACK_MODE_2;
  ftm_wcdma_dl_dch_trch_config[1].cctrch_id                   = ftm_wcdma_dl_db.dl_cctrch_info->cctrch_id;
  ftm_wcdma_dl_dch_trch_config[1].cs_or_ps_domain             = CS_DOMAIN;
  ftm_wcdma_dl_dch_trch_config[1].ndlchan                     = 1;
  ftm_wcdma_dl_dch_trch_config[1].trch_id                     = 10;
  ftm_wcdma_dl_dch_trch_config[1].dlchan_info[0].chan_type    = UE_LOGCHAN_DCCH;
  ftm_wcdma_dl_dch_trch_config[1].dlchan_info[0].mac_id       = 2;
  ftm_wcdma_dl_dch_trch_config[1].dlchan_info[0].rb_id        = 1;
  ftm_wcdma_dl_dch_trch_config[1].dlchan_info[0].rlc_id       = 1;
  ftm_wcdma_dl_dch_trch_config[1].dlchan_info[0].rlc_mode     = UE_MODE_TRANSPARENT;

  wcdma_l2_dl_put_cmd(l2_dl_cmd_ptr);
}


/*===========================================================================
FUNCTION     FTM_PUT_WL1_CMD

DESCRIPTION
  This function is called by WCDMA L1 to send a message to FTM.  In normal
  phone operation these messages go to RRC.  But in Factory Test Mode,
  FTM is commanding WL1, so FTM gets WL1 messages.

DEPENDENCIES
  None.

PARAMETERS
  rrc_cmd_type *cmd_buf - Pointer to command structure.

RETURN VALUE
  None.

SIDE EFFECTS
  Posts a WL1 message to FTM.
===========================================================================*/
void ftm_put_wl1_cmd(rrc_cmd_type *cmd_ptr)
{

  if (ftm_mode == FTM_MODE)
  {
    /* First initialize the queue link.  This is always done
    ** before putting an item in a queue. */
    (void)q_link(cmd_ptr, &(cmd_ptr->cmd_hdr.link));

    /* Then put the buffer in the queue. */
    q_put(&ftm_wcdma_ber_q, &(cmd_ptr->cmd_hdr.link));

    /* Then set the signal for this queue. */
    (void)rex_set_sigs(TASK_HANDLE(ftm), FTM_WCDMA_BER_CMD_Q_SIG);
  }
  else
  {
    /* This is pretty awful.  WL1 sent a command to FTM because it thinks
    ** the phone is in FTM.  But it's not in FTM!  Toss the command. */
    FTM_MSG_ERROR("WL1 cmd sent to FTM. UE mode:%d not FTM!", ftm_mode, 0, 0);
    rrc_free_cmd_buf(cmd_ptr);

    /* Cleanup state, since this is a "fatal" error. */

    FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_NULL);
  }
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SEND_INT_CMD

DESCRIPTION    Sends a command from FTM->FTM.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_send_int_cmd( ftm_wcdma_ber_int_cmd_type *msg)
{

  ftm_wcdma_ber_int_cmd_type  *newmsg;

  newmsg = (ftm_wcdma_ber_int_cmd_type *) q_get( &ftm_wcdma_ber_int_free_q);

  if( newmsg != NULL)
  {
    *newmsg = *msg;
    q_put( &ftm_wcdma_ber_int_q, q_link( newmsg, &newmsg->cmd_hdr.link));
    (void)rex_set_sigs( TASK_HANDLE(ftm), FTM_WCDMA_BER_INT_CMD_Q_SIG);
  }
  else
  {
      FTM_MSG_HIGH("No free buffers for FTM WBER",0,0,0);
  }
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_TX_DATA_TO_RLC

DESCRIPTION    Take raw data and send to RLC for looping back on UL.


DEPENDENCIES
  none

NOTE: This code has been copied from \nas\tc\loopback.c.  Ideally the code
should be re-used rather than copied.
===========================================================================*/
void ftm_wcdma_ber_tx_data_to_rlc(uint8 wm_id, uint8 *lb_from_rlc_data, uint16 lb_tx_item_size, uint32 app_ptr)
{

  dsm_item_type *dsm_item_ptr = NULL; /* Pointer to a DSM item sent to RLC */

  /*-------------------------------------------------------------------------
  Convert the UL data into a chained DSM item and queue it on
  the DS/RLC watermark.
  -------------------------------------------------------------------------*/

  dsm_item_ptr = dsm_sdu_to_pkt_chain_bit( lb_from_rlc_data, lb_tx_item_size);

  if ( dsm_item_ptr == NULL )
  {
    FTM_MSG_ERROR("Empty UL packet chain?",0,0,0);
  }
  else
  {
    dsm_item_ptr->app_ptr = (void *) app_ptr;
    dsm_enqueue( &ftm_wcdma_ber_ul_wm[wm_id], &dsm_item_ptr);
  }
}



/*===========================================================================

FUNCTION       FTM_WCDMA_BER_LOOPBACK_HANDLER

DESCRIPTION    Handles FTM indications that data is ready to be looped back
               for a particular DL watermark.  The DL data is processed, and
               sent to RLC for transmission.


DEPENDENCIES
  none

NOTE: This code has been copied from \nas\tc\loopback.c.  Ideally the code
should be re-used rather than copied.

===========================================================================*/
void ftm_wcdma_ber_loopback_handler(uint8 wm_id)
{

  uint16 index = 0;
  uint8  dsm_data_byte;
  dsm_item_type *dsm_item_ptr = NULL;
  uint8 *lb_from_rlc_data = NULL;
  /* Place holder for loopback transmit item size in bits */
  uint16 lb_tx_item_size = 0;
  /* Place holder for received DSM item length on the DL */
  static uint16 dsm_item_len = 0;
  uint32 app_ptr;

  #ifdef FTM_HAS_SE_BER
  ftm_se_ber_config_run_type              config;
  #endif

  /*-------------------------------------------------------------------------
  Get a DSM item from the RLC WM.
  -------------------------------------------------------------------------*/

  /* If the wm_id is for logical channel 1 (DCCH), don't bother looping data back.
   * It seems important to not use the max data rate combination (DCCH+DTCH) when
   * transmitting at max power.  This should always keep us in the 0xN case for the
   * transport channel corresponding to the DCCH data.  Also the MAC constant delay
   * loopback feature may only work when the transport channels used are synchronous
   * (same TTI), so we should stick to DTCH only.
   */

  FTM_MSG_LOW( "Rx LB data from wm: %d",  wm_id, 0, 0 );

  if( wm_id == 1)
  {

    FTM_MSG_LOW( "Empty WM1", 0, 0, 0 );
    /* just empty the queue and exit */
    dsm_empty_queue( &ftm_wcdma_ber_dl_wm[1]  );
    return;
  }

  /* Don't start looping data until FTM decides we are in DCH state based on
   * notifications from L1
   */
  if( FTM_WCDMA_BER_GET_STATE() != FTM_WCDMA_BER_STATE_DCH)
  {
    dsm_empty_queue( &ftm_wcdma_ber_dl_wm[wm_id] );
    return;
  }


  dsm_item_ptr = dsm_dequeue( &ftm_wcdma_ber_dl_wm[wm_id] );

  if ( dsm_item_ptr == NULL )
  {
    FTM_MSG_HIGH( "No DSM item on RLC/DS WM?", 0, 0, 0 );
  }

  /*-------------------------------------------------------------------------
  Retrieve all the data on the RLC/DS WM, and process for Loopback.
  -------------------------------------------------------------------------*/
  while ( dsm_item_ptr != NULL )
  {

    /*---------------------------------------------------------------------
    Copy the data in the DSM item into the array lb_from_rlc_data
    --------------------------------------------------------------------*/
    dsm_item_len = (uint16) dsm_length_packet( dsm_item_ptr );
    app_ptr = (uint32) dsm_item_ptr->app_ptr;

    // Set the loopback transmit item length in bits
    lb_tx_item_size = dsm_bit_length_packet(dsm_item_ptr);

    lb_from_rlc_data = modem_mem_alloc(dsm_item_len * sizeof (uint8),MODEM_MEM_CLIENT_RFA );

    if ( lb_from_rlc_data != NULL )
    {
      memset((void*)lb_from_rlc_data, 0, dsm_item_len);

      for ( index = 0; index < dsm_item_len; index++ )
      {
        if ( dsm_pullup( &dsm_item_ptr, &dsm_data_byte, 1 ) != 1 )
        {
          FTM_MSG_ERROR( "Invalid DL RLC SDU", 0, 0, 0);
        }
        else
        {
          lb_from_rlc_data[index] = dsm_data_byte;
        }
      }

      dsm_free_packet( &dsm_item_ptr);
      /*---------------------------------------------------------------------
      Get a DSM item from the RLC/DS WM.
      ---------------------------------------------------------------------*/
      dsm_item_ptr = dsm_dequeue(&ftm_wcdma_ber_dl_wm[wm_id]);

      // Loopback the data
      FTM_MSG_LOW( "Looping back: wm: %d, size: %d",  wm_id,  lb_tx_item_size, 0 );
      ftm_wcdma_ber_tx_data_to_rlc(wm_id, lb_from_rlc_data, lb_tx_item_size, app_ptr);

      #ifdef FTM_HAS_SE_BER
      /* Submit the DL data to the Single Ended BER module */
      config.bit_ordering = FTM_SE_BER_BIT_ORDERING_LEFT_ALIGNED;
      config.run_mode     = FTM_SE_RUN_MODE_WCDMA_RMC;
      config.bfi          = FALSE; /* doesn't apply in WCDMA mode */
      config.slot_num     = 0;

      switch( dch_config.rmc_setup)
      {
        case FTM_WCDMA_BER_RMC_TYPE_12_2:
        case FTM_WCDMA_BER_RMC_TYPE_12_2_SYMMETRIC:

          if( lb_tx_item_size >= 244)
          {
            config.bit_def.num_bits = 244;
            ftm_se_ber_run( lb_from_rlc_data, &config);
          }
          break;

        case FTM_WCDMA_BER_RMC_TYPE_64:
        case FTM_WCDMA_BER_RMC_TYPE_64_SYMMETRIC:

          if( lb_tx_item_size >= 1280)
          {
            config.bit_def.num_bits = 1280;
            ftm_se_ber_run( lb_from_rlc_data, &config);
          }
          break;

        case FTM_WCDMA_BER_RMC_TYPE_384:
        case FTM_WCDMA_BER_RMC_TYPE_384_SYMMETRIC:

          if( lb_tx_item_size >= 3840)
          {
            config.bit_def.num_bits = 3840;
            ftm_se_ber_run( lb_from_rlc_data, &config);
          }
          break;

        default:
          break;
      }
      #endif /* FTM_HAS_SE_BER */

      modem_mem_free(lb_from_rlc_data,MODEM_MEM_CLIENT_RFA);
    }
    else
    {
      FTM_MSG_ERROR ("Failed to allocate memory - discarding data", 0, 0, 0);
    }

  }

} /* lb_mode2_rx_handler() */


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_PROCESS_INT_MSGS

DESCRIPTION    Message handler for FTM->FTM messages.  Currently this
               only processes messages sent from the watermark queuing
               functions, so they can be processed in the FTM task context.


DEPENDENCIES
               ftm_wcdma_ber_init() called.

===========================================================================*/
void ftm_wcdma_ber_process_int_msgs( void)
{
  ftm_wcdma_ber_int_cmd_type  *newmsg;

  uint8          id;

  /* Get all commands off the internal Q */

  while( q_cnt(&ftm_wcdma_ber_int_q) > 0)
  {
    newmsg = q_get(&ftm_wcdma_ber_int_q);

    if( newmsg != NULL)
    {
      switch( newmsg->cmd_hdr.cmd_id)
      {

        /* Rx data indication...process the watermark id */

        case FTM_WCDMA_BER_INT_CMD_RX_DATA:

          id = newmsg->msg.rx_data_stream_cmd.wm_id;
          ftm_wcdma_ber_loopback_handler( id);

          break;

        default:
          break;

      }

      /* Return the message buffer to the free Q */
      q_put( &ftm_wcdma_ber_int_free_q, q_link( newmsg, &newmsg->cmd_hdr.link));
    }
  }
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_PROCESS_MSGS

DESCRIPTION    Message handler for commands that arrive from external tasks
               (e.g. RRC).


DEPENDENCIES
               ftm_wcdma_ber_init() called.

===========================================================================*/
void ftm_wcdma_ber_process_msgs( void)
{

  rrc_cmd_type *msg;

  /* Loop through all messages in the Queue */

  while( q_cnt( &ftm_wcdma_ber_q) > 0)
  {
    msg = q_get( &ftm_wcdma_ber_q);

    if( msg != NULL)
    {

      /* Process RRC messages with WCDMA handler */

      ftm_wcdma_ber_process_l1_messages( msg);

      /* Process RRC messages with HSDPA handler */

      #ifdef FEATURE_HSDPA
      ftm_hsdpa_bler_process_l1_messages( msg);
      #endif
      /* Discard the message */

      rrc_free_cmd_buf( msg);
    }

  }
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_PROCESS_RRC_MESSAGES

DESCRIPTION


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_process_l1_messages( rrc_cmd_type *msg)
{

    switch( msg->cmd_hdr.cmd_id)
    {

/*************************************************************************
*                                                                        *
*              RRC_CPHY_START_WCDMA_MODE_CNF                             *
*                                                                        *
*                 -Normal response to CPHY_START_WCDMA_MODE_REQ          *
*                                                                        *
**************************************************************************/

      case RRC_CPHY_START_WCDMA_MODE_CNF:
      {

        FTM_MSG_HIGH("Received RRC_CPHY_START_WCDMA_MODE_CNF",0,0,0);

        ftm_wcdma_ber_allocate_dl_db();
        ftm_wcdma_ber_allocate_ul_db();

        ftm_wcdma_ber_log_gen_cnf( FTM_LOG_WCDMA_BER_START_WCDMA_MODE_CNF);
        FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_IDLE);

        ftm_wcdma_dl_db.current_rf_chan_active = FALSE;

        // Fill out the entry for the synchronous status message
        WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_START_WCDMA_MODE_SYNC_STATUS ].status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
        WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_START_WCDMA_MODE_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_STOP_WCDMA_MODE_CNF                              *
*                                                                        *
*                 -Normal response to CPHY_STOP_WCDMA_MODE_REQ           *
*                                                                        *
**************************************************************************/

      case RRC_CPHY_STOP_WCDMA_MODE_CNF:
      {

        FTM_MSG_HIGH("Received RRC_CPHY_STOP_WCDMA_MODE_CNF",0,0,0);

        ftm_wcdma_ber_free_dl_db();
        ftm_wcdma_ber_free_ul_db();

        ftm_wcdma_ber_log_gen_cnf( FTM_LOG_WCDMA_BER_STOP_WCDMA_MODE_CNF);
        FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_NULL);

        ftm_wcdma_dl_db.current_rf_chan_active = FALSE;

        // Fill out the entry for the synchronous status message
        WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_STOP_WCDMA_MODE_SYNC_STATUS ].status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
        WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_STOP_WCDMA_MODE_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_IDLE_CNF                                         *
*                                                                        *
*                 -Normal response to CPHY_START_IDLE_MODE_REQ           *
*                                                                        *
**************************************************************************/

      case RRC_CPHY_IDLE_CNF:
      {

        ftm_wcdma_ber_log_gen_cnf( FTM_LOG_WCDMA_BER_IDLE_MODE_CNF);

        FTM_MSG_HIGH("Received RRC_CPHY_IDLE_CNF",0,0,0);

            if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH ||
              FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH_PENDING)
            {
                    /* De-register the RLC watermarks */
                    ftm_wcdma_ber_send_CRLC_DL_DEREGISTER_REQ();
                    ftm_wcdma_ber_send_CRLC_UL_DEREGISTER_REQ();

                    /* Release the RLC logical channels */
                    ftm_wcdma_ber_send_CRLC_DL_RELEASE_REQ();
                    ftm_wcdma_ber_send_CRLC_UL_RELEASE_REQ();

                    ftm_wcdma_ber_flush_dsm_buffers();
            }

        FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_IDLE);

        ftm_wcdma_dl_db.current_rf_chan_active = FALSE;

        // Fill out the entry for the synchronous status message
        WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_IDLE_MODE_SYNC_STATUS ].status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
        WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_IDLE_MODE_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_ACQ_CNF                                          *
*                                                                        *
*                 -Normal response to CPHY_ACQ_REQ                       *
*                                                                        *
**************************************************************************/

      case RRC_CPHY_ACQ_CNF:
      {

        FTM_MSG_HIGH("Received RRC_CPHY_ACQ_CNF",0,0,0);

        if( msg->cmd.acq_cnf.status == TRUE)
        {
           WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_ACQ_SYNC_STATUS ].status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;

          FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_ACQ);   /* Acquired Pilot, demod BCH for SFN read */
          FTM_MSG_MED( "Detect: Freq = %d, SC = %d",
                        msg->cmd.acq_cnf.freq,
                        msg->cmd.acq_cnf.scr_code,
                        0);

          ftm_wcdma_dl_db.current_rf_chan_active = TRUE;
          ftm_wcdma_dl_db.current_rf_chan        = msg->cmd.acq_cnf.freq;
        }
        else
        {
          WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_ACQ_SYNC_STATUS ].status = FTM_WCDMA_BER_SYNC_STATUS_EVENT_STATUS_FAILED;

          FTM_MSG_MED( "Acqusition on Freq = %d FAILED!",
                       msg->cmd.acq_cnf.freq,
                       0,
                       0);

          ftm_wcdma_dl_db.current_rf_chan_active = FALSE;
        }

        ftm_wcdma_ber_log_acq_cnf( msg->cmd.acq_cnf.status,
                                   msg->cmd.acq_cnf.freq,
                                   msg->cmd.acq_cnf.scr_code);

        // Fill out the entry for the synchronous status message
        WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_ACQ_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_SETUP_CNF                                        *
*                                                                        *
*                 -Normal response to CPHY_SETUP_REQ                     *
*                 -If parameters from CPHY_SETUP_REQ were invalid, the   *
*                  status will indicate a failure.                       *
*                 -For a normal ACQ->DCH setup, L1 will stay in ACQ      *
*                  state if Setup Req Fails.                             *
*                 -For a DCH->DCH reconfigure, if Setup Req fails        *
*                  then we tear down everything and attempt to go to     *
*                  IDLE to cleanup.                                      *
*                                                                        *
*                                                                        *
**************************************************************************/

      case RRC_CPHY_SETUP_CNF:
      {

        if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH_PENDING)
        {

          if( msg->cmd.setup_cnf.status == TRUE )
          {

            /* RF chan is same as acq chan, still valid */

            /* Need to wait for PhyChan establishment before declaring DCH state */

            FTM_MSG_HIGH("RRC_CPHY_SETUP_CNF, Status: Success!",0,0,0);
          }
          else
          {

            /* RF chan still valid...should be tracking CPICH */

            ftm_wcdma_ber_send_CRLC_DL_DEREGISTER_REQ();
            ftm_wcdma_ber_send_CRLC_UL_DEREGISTER_REQ();

            /* Release the RLC logical channels */
            ftm_wcdma_ber_send_CRLC_DL_RELEASE_REQ();
            ftm_wcdma_ber_send_CRLC_UL_RELEASE_REQ();

            ftm_wcdma_ber_flush_dsm_buffers();
            FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_ACQ);
            FTM_MSG_HIGH("RRC_CPHY_SETUP_CNF, Status: Fail!",0,0,0);
          }
        }
        else if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH_RECONFIG_PENDING)
        {

          if( msg->cmd.setup_cnf.status == TRUE )
          {

             /* Flush buffers following a reconfig */

             ftm_wcdma_ber_flush_dsm_buffers();

             /* Send Loopback config to MAC to enable the UL loop */



             /* Need to wait for PhyChan Established before declaring DCH state */
             FTM_MSG_HIGH("DCH Reconfig CPHY_SETUP_CNF success",0,0,0);
          }
          else
          {

            ftm_wcdma_dl_db.current_rf_chan_active = FALSE;

            FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_ERROR_OCCURRED_IDLE_PENDING);

            /* De-register the RLC watermarks */

            ftm_wcdma_ber_send_CRLC_DL_DEREGISTER_REQ();
            ftm_wcdma_ber_send_CRLC_UL_DEREGISTER_REQ();

            /* Release the RLC logical channels */
            ftm_wcdma_ber_send_CRLC_DL_RELEASE_REQ();
            ftm_wcdma_ber_send_CRLC_UL_RELEASE_REQ();

            ftm_wcdma_ber_flush_dsm_buffers();

            ftm_wcdma_ber_send_CPHY_IDLE_REQ();

            FTM_MSG_HIGH("DCH Reconfig failure",0,0,0);
          }
        }

        ftm_wcdma_ber_log_gen_cnf_with_status( FTM_LOG_WCDMA_BER_CPHY_SETUP_CNF,
                                               msg->cmd.setup_cnf.status);

        // Fill out the entry for the synchronous status message
        WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_CPHY_SETUP_SYNC_STATUS ].status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
        WCDMA_BER_Sync_Status[ FTM_WCDMA_BER_CPHY_SETUP_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_PHYCHAN_ESTABLISHED_IND                          *
*                                                                        *
*                 -Normal response after a CPHY_SETUP_REQ, if the Setup  *
*                  request is valid.                                     *
*                 -Indicates if the physical channel could successfully  *
*                  be established.                                       *
*                 -For normal ACQ->DCH setup, if status is good, then    *
*                  we are in DCH state.  For DCH->DCH reconfig, we       *
*                  remain in DCH, and also attempt to restore the HS     *
*                  setup if one was active prior to reconfig.            *
*                 -If status is bad, we attempt to go to IDLE to cleanup *
*                                                                        *
*                                                                        *
**************************************************************************/

      case RRC_CPHY_PHYCHAN_ESTABLISHED_IND:
      {

        FTM_MSG_HIGH( "Received RRC_CPHY_PHYCHAN_ESTABLISHED_IND with status %d",
                      msg->cmd.phy_est_ind.status,0,0);

        ftm_wcdma_ber_log_gen_cnf_with_status( FTM_LOG_WCDMA_PHYCHAN_ESTABLISHED_IND,
                                               msg->cmd.phy_est_ind.status);


        /* Physical Channel Established properly */

        if( msg->cmd.phy_est_ind.status == TRUE)
        {

          ftm_wcdma_ber_send_CMAC_UL_CONFIG_LOOPBACK_REQ();

          if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH_PENDING)
          {

            FTM_MSG_HIGH("PhyChan Established: ACQ->DCH success",0,0,0);

            FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_DCH);
          }
          if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH_RECONFIG_PENDING)
          {

            FTM_MSG_HIGH( "Reconfig PhyChan Established: DCH->DCH success",0,0,0);

            /* Now set up on new RF channel */

            ftm_wcdma_dl_db.current_rf_chan = dch_config.rf_chan;

            FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_DCH);

            /* Resume the HSDPA channel.  Function will check if HS channel was
             * active just before HHO, then attempt to start it back up
             */
            #ifdef FEATURE_HSDPA
            ftm_hsdpa_bler_request_resume_hs_channel();
            #endif
          }

          WCDMA_BER_Sync_Status[ FTM_WCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS ].status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
        }

        /* Physical Channel Establishement error */

        else if( msg->cmd.phy_est_ind.status == FALSE)
        {

          FTM_MSG_HIGH("Going to IDLE, PhyChan Establishment error",0,0,0);

          ftm_wcdma_dl_db.current_rf_chan_active = FALSE;

          FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_ERROR_OCCURRED_IDLE_PENDING);

          /* De-register the RLC watermarks */

          ftm_wcdma_ber_send_CRLC_DL_DEREGISTER_REQ();
          ftm_wcdma_ber_send_CRLC_UL_DEREGISTER_REQ();

          /* Release the RLC logical channels */
          ftm_wcdma_ber_send_CRLC_DL_RELEASE_REQ();
          ftm_wcdma_ber_send_CRLC_UL_RELEASE_REQ();

          ftm_wcdma_ber_flush_dsm_buffers();

          ftm_wcdma_ber_send_CPHY_IDLE_REQ();

          WCDMA_BER_Sync_Status[ FTM_WCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS ].status = FTM_WCDMA_BER_SYNC_STATUS_EVENT_STATUS_FAILED;

        }


        // Fill out the entry for the synchronous status message

        WCDMA_BER_Sync_Status[ FTM_WCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;

      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_RL_FAILURE_IND                                   *
*                                                                        *
**************************************************************************/

      case RRC_CPHY_RL_FAILURE_IND:
      {

        ftm_wcdma_ber_log_gen_cnf( FTM_LOG_WCDMA_RL_FAILURE_IND);
        FTM_MSG_HIGH( "Received RRC_CPHY_RL_FAILURE_IND",0,0,0);

        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_ERROR_IND                                        *
*                                                                        *
**************************************************************************/


      case RRC_CPHY_ERROR_IND:
      {

        ftm_wcdma_ber_log_gen_cnf( FTM_LOG_WCDMA_CPHY_ERROR_IND);
        FTM_MSG_HIGH( "Received RRC_CPHY_ERROR_IND",0,0,0);

        break;
      }

/*************************************************************************
*                                                                        *
*              DEFAULT                                                   *
*                                                                        *
**************************************************************************/

      default:
      {

        FTM_MSG_HIGH( "Received Unknown message from L1: %d", msg->cmd_hdr.cmd_id,0,0);

        break;
      }
    }
}


/*===========================================================================

FUNCTION       FTM_WCDMA_FLUSH_DSM_BUFFERS

DESCRIPTION    When DCH is taken down, flush the dsm buffers (UL/DL).
               This insures that no "leftover" data packets are in the
               buffers if a DCH is brought up again.


DEPENDENCIES
               ftm_wcdma_ber_init() has been called.

===========================================================================*/
void ftm_wcdma_ber_flush_dsm_buffers( void)
{

    uint8  i;
    FTM_MSG_HIGH( "Flushing DSM buffers", 0, 0, 0 );

    for( i = 0; i < FTM_WCDMA_BER_MAX_NUM_LOGICAL_CHANNELS; i++)
    {

        dsm_empty_queue(  &ftm_wcdma_ber_dl_wm[i] );
        dsm_empty_queue(  &ftm_wcdma_ber_ul_wm[i] );
    }
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_DISPATCH

DESCRIPTION    Dispatcher for handling commands sent from PC to direct the
               FTM WCDMA BER functionality.


DEPENDENCIES
               ftm_wcdma_ber_init() has been called.

===========================================================================*/
ftm_rsp_pkt_type ftm_wcdma_ber_dispatch( ftm_wcdma_ber_pkt_type *cmd_ptr)

{

  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_wcdma_ber_sync_status_type                cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;

  // Variables for synchronous status
  uint32        start_time     = 0;
  int32         time_to_wait   = 0;
  uint8         TimeOutExpired = FALSE;
  rex_sigs_type ftm_sig_clk_rex_wait_return_sig;

  ftm_wcdma_ber_rx_level_rpt_type  ftm_wcdma_ber_rx_lev_rpt={0,0};
  ftm_wcdma_ber_rscp_rpt_type      ftm_wcdma_ber_rscp_rpt   = {0, 0};

  int32 ecio_rx0 = 0;
  int32 rscp_rx0 = 0;
  int32 ecio_rx1 = 0;
  int32 rscp_rx1 = 0;

  wcdma_signal_info_struct_type *wcdma_sig_info;

  #ifdef FEATURE_HSDPA
  if( cmd_ptr->ftm_cmd_header.cmd_id >= FTM_WCDMA_BER_HSDPA_CMD_BASE)
  {

    rsp_pkt = ftm_hsdpa_bler_dispatch( ( ftm_hsdpa_bler_pkt_type *) cmd_ptr);
  }
  else
  #endif
  {

    /* Remove comparison with FTM_WCDMA_BER_START_WCDMA_MODE_REQ since enum value is 0,
     * resulting in compiler warning about unsigned comparison with 0 */

    if( /*   cmd_ptr->ftm_cmd_header.cmd_id >= FTM_WCDMA_BER_START_WCDMA_MODE_REQ && */
        cmd_ptr->ftm_cmd_header.cmd_id <= FTM_WCDMA_BER_RMC_DCH_SETUP_V2)
    {
      FTM_MSG_MED( "FTM WCDMA BER dispatch, Command Received", 0,0,0);
    }

    switch( cmd_ptr->ftm_cmd_header.cmd_id)
    {

/***********************************************************************
*            FTM_WCDMA_BER_START_WCDMA_MODE_REQ                        *
*                                                                      *
************************************************************************/

      case FTM_WCDMA_BER_START_WCDMA_MODE_REQ:

        /* ftm_rfmode_enter ensures that the previous ftm rfmode is exited before
           sending a start cmd to WL1 for NS operation */
        ftm_set_rf_db_state( FTM_DB_RF_IDLE);
        ftm_rfmode_enter(RFM_DEVICE_0,FTM_STATE_WCDMA_NS);

        /* Start WL1 again only if stopped */

        if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_NULL
            #ifdef FEATURE_FTM_GSM_BER
            && ftm_gsm_ber_get_state() == FTM_GSM_BER_STATE_STOPPED
            #endif
          )
        {
          ftm_wcdma_ber_send_CPHY_START_WCDMA_MODE_REQ();

          cmd_status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
        }
        else
        {

          cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        }

        break;


/***********************************************************************
*            FTM_WCDMA_BER_STOP_WCDMA_MODE_REQ                         *
*                                                                      *
************************************************************************/

      case FTM_WCDMA_BER_STOP_WCDMA_MODE_REQ:

        /* Stop WL1 only if started */
        if( FTM_WCDMA_BER_GET_STATE() != FTM_WCDMA_BER_STATE_NULL)
        {
          /* If in W NS mode just call for a mode exit, that will */
          /* take care of all necessary cleanup. Otherwise we need */
          /* to perform BER cleanup here. */
          if (ftm_get_current_state(RFM_DEVICE_0) == FTM_STATE_WCDMA_NS)
          {
            ftm_rfmode_exit(RFM_DEVICE_0,FTM_STATE_PARK);
          }
          else
          {
            ftm_wcdma_ber_cleanup();
          }
        }

        cmd_status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
        break;

/***********************************************************************
*            FTM_WCDMA_BER_IDLE_REQ                                    *
*                                                                      *
************************************************************************/

      case FTM_WCDMA_BER_IDLE_REQ:

        /* Set L1 to IDLE mode */
        if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_IDLE        ||
            FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH         ||
            FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_DCH_PENDING ||
            FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_ACQ)
        {

          ftm_wcdma_ber_send_CPHY_IDLE_REQ();
          cmd_status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
        }
        else
        {

          cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        }

        break;

/***********************************************************************
*            FTM_WCDMA_BER_ACQUIRE_REQ                                 *
*                                                                      *
************************************************************************/

      case FTM_WCDMA_BER_ACQUIRE_REQ:

        /* Acquire only if idle */

        if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_IDLE )
        {

          ftm_wcdma_ber_send_CPHY_ACQ_REQ( &((ftm_wcdma_ber_acquire_req_type *) cmd_ptr)->acq_config);
          cmd_status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
        }
        else
        {

          cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
          FTM_MSG_HIGH("FTM_WCDMA_BER_ACQUIRE_REQ: Not in IDLE state. Current state: %d",FTM_WCDMA_BER_GET_STATE(),0,0);
        }
        break;

/***********************************************************************
*            FTM_WCDMA_BER_RMC_DCH_SETUP                               *
*                                                                      *
************************************************************************/

      case FTM_WCDMA_BER_RMC_DCH_SETUP:
      case FTM_WCDMA_BER_RMC_DCH_SETUP_V2:
      {

        ftm_wcdma_ber_rmc_dch_setup_config_type  *dch_config_ptr;
        ftm_wcdma_ber_rmc_dch_setup_v2_config_type *dch_config_v2_ptr;

        // Assume pass until failure is detected
        cmd_status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;

        /* Have we:
         *    - successfully acquired pilot already OR
         *    - on DCH and using DCH V2 command for a reconfig
         * If neither, then declare command invalid.
         */

        if( !( FTM_WCDMA_BER_GET_STATE()              == FTM_WCDMA_BER_STATE_ACQ &&
               ftm_wcdma_dl_db.current_rf_chan_active == TRUE)               &&

            !( FTM_WCDMA_BER_GET_STATE()      == FTM_WCDMA_BER_STATE_DCH     &&
               cmd_ptr->ftm_cmd_header.cmd_id == FTM_WCDMA_BER_RMC_DCH_SETUP_V2)

          )
        {

          cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
          break;
        }

        if( cmd_ptr->ftm_cmd_header.cmd_id == FTM_WCDMA_BER_RMC_DCH_SETUP)
        {

          dch_config_ptr = &(( (ftm_wcdma_ber_rmc_dch_setup_type *) cmd_ptr)->rmc_dch_config);

          /* Setup default configuration for parameters not part of setup packet */

          dch_config.rmc_setup              = dch_config_ptr->rmc_setup;
          dch_config.ul_max_tx_pwr          = dch_config_ptr->ul_max_tx_pwr;
          dch_config.ul_dpch_scr_code       = dch_config_ptr->ul_dpch_scr_code;
          dch_config.ul_pca                 = dch_config_ptr->ul_pca;
          dch_config.ul_tpc_step            = dch_config_ptr->ul_tpc_step;
          dch_config.dl_pri_cpich_scr_code  = dch_config_ptr->dl_pri_cpich_scr_code;
          dch_config.dl_chan_code           = dch_config_ptr->dl_chan_code;
          dch_config.beta_c                 = 15;
          dch_config.beta_d                 = 15;
          dch_config.dpcch_power_offset     = -80;
          dch_config.n_312                  = FTM_WCDMA_BER_N312_s1;
          dch_config.n_313                  = FTM_WCDMA_BER_N313_s20;
          dch_config.n_315                  = FTM_WCDMA_BER_N315_s200;
          dch_config.t_312                  = 10;
          dch_config.t_313                  = 10;
          dch_config.rf_chan                = ftm_wcdma_dl_db.current_rf_chan;

        }
        else if( cmd_ptr->ftm_cmd_header.cmd_id == FTM_WCDMA_BER_RMC_DCH_SETUP_V2)
        {

          dch_config_v2_ptr = &(( (ftm_wcdma_ber_rmc_dch_setup_v2_type *) cmd_ptr)->rmc_dch_config);

          /* Setup default configuration for parameters not part of setup packet */

          dch_config.rmc_setup              = dch_config_v2_ptr->rmc_setup;
          dch_config.ul_max_tx_pwr          = dch_config_v2_ptr->ul_max_tx_pwr;
          dch_config.ul_dpch_scr_code       = dch_config_v2_ptr->ul_dpch_scr_code;
          dch_config.ul_pca                 = dch_config_v2_ptr->ul_pca;
          dch_config.ul_tpc_step            = dch_config_v2_ptr->ul_tpc_step;
          dch_config.dl_pri_cpich_scr_code  = dch_config_v2_ptr->dl_pri_cpich_scr_code;
          dch_config.dl_chan_code           = dch_config_v2_ptr->dl_chan_code;
          dch_config.beta_c                 = dch_config_v2_ptr->beta_c;
          dch_config.beta_d                 = dch_config_v2_ptr->beta_d;
          dch_config.dpcch_power_offset     = dch_config_v2_ptr->dpcch_power_offset;
          dch_config.n_312                  = dch_config_v2_ptr->n_312;
          dch_config.n_313                  = dch_config_v2_ptr->n_313;
          dch_config.n_315                  = dch_config_v2_ptr->n_315;
          dch_config.t_312                  = dch_config_v2_ptr->t_312;
          dch_config.t_313                  = dch_config_v2_ptr->t_313;
          dch_config.rf_chan                = dch_config_v2_ptr->rf_chan;


          if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_ACQ)
          {
            /* If we are in ACQ state (CPICH tracking), do not allow independent
             * setting of RF chan.  Must follow the rf chan of CPICH that we aquired
             */

            if( dch_config.rf_chan != ftm_wcdma_dl_db.current_rf_chan)
            {

              FTM_MSG_HIGH("Overriding RF Channel with channel used in ACQ procedure",0,0,0);

              dch_config.rf_chan = ftm_wcdma_dl_db.current_rf_chan;
            }
          }
        }


        FTM_MSG_HIGH("  DCH Setup: RFChan=%d, RMC Type=%d, MaxTxPwr=%d",
                       dch_config.rf_chan,
                       dch_config.rmc_setup,
                       dch_config.ul_max_tx_pwr);

        FTM_MSG_HIGH("  DCH Setup: ScrCode=%d, BetaC=%d, BetaD=%d",
                       dch_config.dl_pri_cpich_scr_code,
                       dch_config.beta_c,
                       dch_config.beta_d);

        FTM_MSG_HIGH("  DCH Setup: Chan Code=%d, DPCCH Power Offset=%d", dch_config.dl_chan_code,
                                                                        dch_config.dpcch_power_offset, 0 );

        switch( dch_config.rmc_setup )
        {

          case FTM_WCDMA_BER_RMC_TYPE_12_2:
          case FTM_WCDMA_BER_RMC_TYPE_12_2_SYMMETRIC:
          {

            /* Setup the UL/DL L1 database */
            if( ftm_wcdma_configure_DL_RMC12p2() == FALSE ||
                ftm_wcdma_configure_UL_RMC12p2() == FALSE )
            {

              cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
            }
            break;
          }
          case FTM_WCDMA_BER_RMC_TYPE_64:
          case FTM_WCDMA_BER_RMC_TYPE_64_SYMMETRIC:
          {

            /* Setup the UL/DL L1 database */
            if( ftm_wcdma_configure_DL_RMC64() == FALSE ||
                ftm_wcdma_configure_UL_RMC64() == FALSE )
            {

              cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
            }


            break;
          }
          case FTM_WCDMA_BER_RMC_TYPE_384:
          case FTM_WCDMA_BER_RMC_TYPE_384_SYMMETRIC:
          {

            /* Setup the UL/DL L1 database */
            if( ftm_wcdma_configure_DL_RMC384() == FALSE ||
                ftm_wcdma_configure_UL_RMC384() == FALSE )
            {
            cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
            }

            break;
          }
          default:
          {

            cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
            break;
          }
        }

        if( cmd_status != FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE )
        {

          if( FTM_WCDMA_BER_GET_STATE() == FTM_WCDMA_BER_STATE_ACQ)
          {

            FTM_MSG_MED( "Setting up DCH from ACQ state",0,0,0);

            FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_DCH_PENDING);

            /* Establish RLC logical channels */
            ftm_wcdma_ber_send_CRLC_UL_CONFIG_REQ();
            ftm_wcdma_ber_send_CRLC_DL_CONFIG_REQ();

            /* Establish RLC Watermarks */
            ftm_wcdma_ber_send_CRLC_UL_REGISTER_REQ();
            ftm_wcdma_ber_send_CRLC_DL_REGISTER_REQ();

            /* assign here so MAC and L1 both get updated cctrch id */
            ftm_wcdma_ber_assign_cctrch_id();

            /* Configure MAC */
            ftm_wcdma_ber_send_CMAC_UL_CONFIG_REQ();
            ftm_wcdma_ber_send_CMAC_DL_CONFIG_REQ();

            /* Send config command to L1 */

            ftm_wcdma_ber_send_ULDL_CPHY_SETUP_REQ();
          }
          else
          {

            /* Try to reconfigure */

            FTM_MSG_MED( "Reconfiguring DCH from DCH state",0,0,0);

            FTM_WCDMA_BER_SET_STATE( FTM_WCDMA_BER_STATE_DCH_RECONFIG_PENDING);

            /* assign here so MAC and L1 both get updated cctrch id */
            ftm_wcdma_ber_assign_cctrch_id();

            /* (Re)Configure MAC */
            ftm_wcdma_ber_send_CMAC_UL_CONFIG_REQ();
            ftm_wcdma_ber_send_CMAC_DL_CONFIG_REQ();

            /* Send CPHY_SETUP_REQ with Drop (old config) and Add (new config) commands */

            ftm_wcdma_ber_send_ULDL_RECFG_CPHY_SETUP_REQ();
          }
        }

        // Explicitly set success status
        cmd_status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;


        break;
      }

    case FTM_WCDMA_BER_RMC_DCH_TFCS_CONFIG_REQ:
    {

      ftm_wcdma_ber_rmc_dch_tfcs_config_req_type  *config = (ftm_wcdma_ber_rmc_dch_tfcs_config_req_type *) cmd_ptr;
      uint8      i;

        // Set to success flag and change status when failure is detected
        cmd_status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;

      /* Config must use unique values spanning 0..3 */

        /* Check if unique */
        if( ( config->tfcs_config[0] == config->tfcs_config[1]) ||
            ( config->tfcs_config[0] == config->tfcs_config[2]) ||
            ( config->tfcs_config[0] == config->tfcs_config[3])
          )
        {

          cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        break;
      }


      /* Check to make sure all values within range */
      if( config->tfcs_config[0] >= FTM_WCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS ||
          config->tfcs_config[1] >= FTM_WCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS ||
          config->tfcs_config[2] >= FTM_WCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS ||
          config->tfcs_config[3] >= FTM_WCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS
        )
      {

          cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        break;
      }

      for( i = 0; i < FTM_WCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS; i++ )
      {

        if( config->tfcs_select == FTM_WCDMA_BER_UL_TFCS_CONFIG)
        {
          dch_config.ul_tfcs_config[i] = config->tfcs_config[i];
        }
        else if( config->tfcs_select == FTM_WCDMA_BER_DL_TFCS_CONFIG)
        {
          dch_config.dl_tfcs_config[i] = config->tfcs_config[i];
        }
        else
        {

            cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;;
          break;
        }
      }

      FTM_MSG_MED( "Configuring TFCS: %d %d %d", config->tfcs_config[0], config->tfcs_config[1], \
                                                 config->tfcs_config[2] );


        cmd_status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
        break;
      }

      case FTM_WCDMA_BER_EVENT_STATUS_CLEAR_REQ:
      {
        /* call the function to clear the status */
        ftm_wcdma_ber_clear_sync_status();

        /* Setup the response value */
        cmd_status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;

        FTM_MSG_MED( "Clearing synchronous WCDMA events", 0,0,0);

        break;
      }

      case FTM_WCDMA_BER_EVENT_STATUS_REQ:
      {
        /* This comand will return the status of events that have occured
           these events are stored in the WCDMA_BER_Sync_Status */

        ftm_wcdma_ber_event_status_req_type *request = (ftm_wcdma_ber_event_status_req_type *) cmd_ptr;

        /* validate the ID of the event_type */
        if ( request->event_type < FTM_WCDMA_BER_SYNC_STATUS_MAX)
        {
          /* Record the start time */
          start_time      = timetick_get();

          /* Setup timeout loop
           * If the event has already reported then the timing loop will not execute
           */
          while (	(  WCDMA_BER_Sync_Status[ request->event_type ].has_updated == FALSE )
                &&  (  !TimeOutExpired ))
          {
            /*
             * Status messages from L1 may not be the one we are looking for.
             * For this case we will wait again adjusting for the time remaining.
             * This is neccessary because all request status messages are sharing the
             * same signal.
             * The first time through the loop the time to wait will be the value passed
             * in by from the user
             */
            time_to_wait = request->timeout_ms - timetick_get_elapsed( start_time, T_MSEC);

            /* Make sure we have more time remaining */
            if( time_to_wait > 0 )
            {
              /* Wait on a request status sig or a timeout. */
              ftm_sig_clk_rex_wait_return_sig = ftm_sig_clk_rex_wait_no_clear(FTM_SEQ_STATUS_REQ_SIG | FTM_WCDMA_BER_INT_CMD_Q_SIG | FTM_WCDMA_BER_CMD_Q_SIG,time_to_wait);

              if( ftm_sig_clk_rex_wait_return_sig & FTM_WCDMA_BER_CMD_Q_SIG)
              {
                (void)rex_clr_sigs( TASK_HANDLE(ftm), FTM_WCDMA_BER_CMD_Q_SIG);
                ftm_wcdma_ber_process_msgs();
              }

              if( ftm_sig_clk_rex_wait_return_sig & FTM_WCDMA_BER_INT_CMD_Q_SIG)
              {
                (void)rex_clr_sigs( TASK_HANDLE(ftm), FTM_WCDMA_BER_INT_CMD_Q_SIG);
                ftm_wcdma_ber_process_int_msgs();
              }

              if( ftm_sig_clk_rex_wait_return_sig & FTM_SEQ_STATUS_REQ_SIG)
              {
                (void)rex_clr_sigs( rex_self(), FTM_SEQ_STATUS_REQ_SIG);
              }

              /* Check to see if we timed out */
              if( (ftm_sig_clk_rex_wait_return_sig & FTM_WAIT_TIMEOUT) == FTM_WAIT_TIMEOUT )
              {
                TimeOutExpired = TRUE;
                cmd_status     = FTM_WCDMA_BER_SYNC_STATUS_TIMEOUT_EXPIRED;
              }
            }
            else
            {
              TimeOutExpired = TRUE;
              cmd_status     = FTM_WCDMA_BER_SYNC_STATUS_TIMEOUT_EXPIRED;
            } /* if( time_to_wait > 0 ) */

          }/* End While timing loop */

          /* If message received then use the status from that message type */
          if(  WCDMA_BER_Sync_Status[ request->event_type ].has_updated == TRUE)
          {
            cmd_status = WCDMA_BER_Sync_Status[ request->event_type ].status;
          }
        }
        else
        {
          /* Error, invalid event_type */
          cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;

          FTM_MSG_HIGH("FTM_WCDMA_BER_EVENT_STATUS_REQ: Invalid Event Type=%d", request->event_type,0,0);
        }/* if ( request->event_type < FTM_WCDMA_BER_SYNC_STATUS_MAX) */

        FTM_MSG_HIGH("FTM_WCDMA_BER_EVENT_STATUS_REQ: Event Type=%d cmd_status =%d",request->event_type,
                                                                                   cmd_status,
                                                                                   0);
      }
      break;

      case FTM_WCDMA_BER_SET_UL_POWER:
      {
        ftm_wcdma_ber_set_ul_pwr_type *request = (ftm_wcdma_ber_set_ul_pwr_type *) cmd_ptr;

        cmd_status = ftm_wcdma_ber_set_ul_pwr( request->override_enable,
                                              request->tx_power_10th_dbm);

      }
      break;

      /***********************************************************************
      *            			FTM_WCDMA_BER_RX_LEVEL_REQ                                              *
      *                                                              									     *
      ************************************************************************/

    case FTM_WCDMA_BER_RX_LEVEL_REQ:
    {
    ftm_wcdma_ber_rx_level_req_type *rx_level_req = (ftm_wcdma_ber_rx_level_req_type *) cmd_ptr;
    if ( FTM_WCDMA_BER_STATE_DCH == ftm_wcdma_ber_current_state)
      {
        if (rx_level_req->rx_chain == PRIMARY_RX_CHAIN)
          {
            ftm_wcdma_ber_rx_lev_rpt.rssi         = rfm_get_receive_agc_val(RFCOM_TRANSCEIVER_0,RFCOM_SERVINGCELL_RXAGC_RAW,RFCOM_RXAGC_IN_DB10);
            ftm_wcdma_ber_rx_lev_rpt.rx_agc_dbm10 = RF_MIN_RX_POWER_DBM*RF_RXAGC_RESOLUTION  + ( rfm_get_rx_raw_agc_val(RFCOM_TRANSCEIVER_0) - RFAGC_MIN_AGC_VAL);
            cmd_status   = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
          }
        else if (rx_level_req->rx_chain == SECONDARY_RX_CHAIN)
          {
            ftm_wcdma_ber_rx_lev_rpt.rssi         = rfm_get_receive_agc_val(RFCOM_RECEIVER_1,RFCOM_SERVINGCELL_RXAGC_RAW,RFCOM_RXAGC_IN_DB10);
            ftm_wcdma_ber_rx_lev_rpt.rx_agc_dbm10 = RF_MIN_RX_POWER_DBM*RF_RXAGC_RESOLUTION  + ( rfm_get_rx_raw_agc_val(RFCOM_RECEIVER_1) - RFAGC_MIN_AGC_VAL);
            cmd_status   = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
          }
        else
          {
            FTM_MSG_HIGH("FTM_WCDMA_BER_RX_LEVEL_REQ: Invalid Rx Chain: %d",rx_level_req->rx_chain,0,0);
            cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
          }
      }
    else
      {
        FTM_MSG_HIGH("FTM_WCDMA_BER_RX_LEVEL_REQ: Wrong UE State:%d",ftm_wcdma_ber_current_state,0,0);
        cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
      }
    break;
    }
      case FTM_WCDMA_BER_RSCP_REQ:
      {
          if(WCDMA_BER_Sync_Status[ FTM_WCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS ].has_updated == TRUE)
          {
             wcdma_sig_info = wcdma_get_signal_info();

             ecio_rx0 = wcdma_sig_info->rx_power.rx_pwr_chain[0].ecio;
             ecio_rx1 = wcdma_sig_info->rx_power.rx_pwr_chain[1].ecio;
             rscp_rx0 = wcdma_sig_info->rx_power.rx_pwr_chain[0].rscp;
             rscp_rx1 = wcdma_sig_info->rx_power.rx_pwr_chain[1].rscp;

             cmd_status = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;
          }
          else
          {
             cmd_status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
          }
      }

      break;
/***********************************************************************
*            DEFAULT                                                   *
*                                                                      *
************************************************************************/

    default:
    {

      FTM_MSG_HIGH("Unknown FTM WCDMA BER Command: %d",cmd_ptr->ftm_cmd_header.cmd_id,0,0);
        cmd_status = FTM_WCDMA_BER_SYNC_STATUS_UNKNOWN_CMD;

        break;
      }
  }

    if( cmd_status == FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE ||
        cmd_status == FTM_WCDMA_BER_SYNC_STATUS_UNKNOWN_CMD )
  {

     FTM_MSG_HIGH( "FTM WCDMA BER, Command Failure: %d, State: %d", cmd_ptr->ftm_cmd_header.cmd_id,
                                                                    FTM_WCDMA_BER_GET_STATE(),
                                                                    0);
  }
    switch (cmd_ptr->ftm_cmd_header.cmd_id)
    {
        case FTM_WCDMA_BER_RX_LEVEL_REQ:
            rsp_pkt = ftm_wcdma_ber_create_rx_lev_response(cmd_ptr,
                                                (uint16) cmd_status,
                                                cmd_ptr->ftm_cmd_header.cmd_id,
                                                0,
                                                ftm_wcdma_ber_rx_lev_rpt);
            break;
        case FTM_WCDMA_BER_RSCP_REQ:
            ftm_wcdma_ber_rscp_rpt.ecio_rx0 = ecio_rx0;
            ftm_wcdma_ber_rscp_rpt.rscp_rx0 = rscp_rx0;
            ftm_wcdma_ber_rscp_rpt.ecio_rx1 = ecio_rx1;
            ftm_wcdma_ber_rscp_rpt.rscp_rx1 = rscp_rx1;

            rsp_pkt = ftm_wcdma_ber_create_rscp_response(cmd_ptr,
                                                (uint16) cmd_status,
                                                cmd_ptr->ftm_cmd_header.cmd_id,
                                                0,
                                                ftm_wcdma_ber_rscp_rpt);
            break;

        default:
  rsp_pkt = ftm_wcdma_ber_create_gen_response(cmd_ptr,
                                                (uint16) cmd_status,
                                              cmd_ptr->ftm_cmd_header.cmd_id,
                                              0);
  }

  }

  return( rsp_pkt);
}


/*===========================================================================

FUNCTION       FTM_WCDMA_BER_GET_STATE

DESCRIPTION    Returns current state of FTM WCDMA BER operations.


DEPENDENCIES
  none

===========================================================================*/
ftm_wcdma_ber_state_type ftm_wcdma_ber_get_state( void)
{

  return (ftm_wcdma_ber_current_state);
}
/*===========================================================================

FUNCTION       FTM_WCDMA_BER_SET_STATE

DESCRIPTION    Returns current state of FTM WCDMA BER operations.


DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_set_state(ftm_wcdma_ber_state_type state)
{

  ftm_wcdma_ber_current_state = state;
}

/*===========================================================================

FUNCTION       ftm_wcdma_ber_clear_sync_status

DESCRIPTION    This function clears all entries in the synchronous status
               array.  This behavior is called by the FTM_WCDMA_BER_EVENT_STATUS_CLEAR_REQ
               command.

DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_clear_sync_status( void)
{
  uint8 Index;

  /* Interate through the array that stores the synchronous status items */
  for ( Index = 0; Index < FTM_WCDMA_BER_SYNC_STATUS_MAX; Index = Index + 1 )
  {
    /* Reset the updated flag */
    WCDMA_BER_Sync_Status[ Index ].has_updated = FALSE;

    /* Reset the event status */
    WCDMA_BER_Sync_Status[ Index ].status = FTM_WCDMA_BER_SYNC_STATUS_GEN_FAILURE;
  }

}

/*===========================================================================

FUNCTION       ftm_wcdma_ber_set_ul_pwr

DESCRIPTION    This function will set the tx power of the up link to a
               specific dBm level.

DEPENDENCIES
  none

===========================================================================*/
ftm_wcdma_ber_sync_status_type ftm_wcdma_ber_set_ul_pwr( byte override_enable,
                                                         int2 tx_power_10th_dbm)
{
  ftm_wcdma_ber_sync_status_type cmd_status    = FTM_WCDMA_BER_SYNC_STATUS_SUCCESS;

  switch( override_enable )
  {
    case FTM_WCDMA_TX_POWER_CONTROL_TYPE_AGC_SETPOINT:
    {
      if( (RF_UE_MIN_TX_POWER*RF_TXAGC_RESOLUTION > tx_power_10th_dbm) || (tx_power_10th_dbm > RF_UE_MAX_TX_POWER*RF_TXAGC_RESOLUTION) )
      {
        FTM_MSG_HIGH("Warning ftm_wcdma_ber_set_ul_pwr: Power setting out of range %d",tx_power_10th_dbm,0,0);
      }

      rf_wfw_intf_txagc_async_write_ptr->txGainAdjOverride[0] = 1;
      rf_wfw_intf_txagc_async_write_ptr->txGainAdjWr[0] = (tx_power_10th_dbm+700) - 512 - rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99[0].txDipPlusCmDelta -
      (Word16) 100*(log10(rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaCGain*rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaCGain +
      rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaDGain*rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaDGain)
      - log10(rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaCGain*rf_fwsw_intf_addr_ptr->txAsyncWriteBuf.r99Common.txBetaCGain));

      break;
    }

    case FTM_WCDMA_TX_POWER_CONTROL_TYPE_RPC_ENABLE:
    {

      rf_wfw_intf_txagc_async_write_ptr->txGainAdjOverride[0] = 0;
      break;
    }

    default:
    {
      FTM_MSG_HIGH( "ftm_wcdma_ber_set_ul_pwr: Unsupported override_enable value=%d, tx_power_10th_dbm =%d", override_enable,tx_power_10th_dbm,0);
      cmd_status = FTM_WCDMA_BER_SYNC_STATUS_OVERRIDE_VALUE_UNSUPPORTED;
      break;
    }
  }

  FTM_MSG_HIGH( "ftm_wcdma_ber_set_ul_pwr: override_enable=%d, tx_power_10th_dbm =%d", override_enable,tx_power_10th_dbm,0);
  return cmd_status;

}

/*===========================================================================

FUNCTION       ftm_wcdma_ber_is_rrc_cmd_start_wcdma_mode

DESCRIPTION    This function checks if the RRC command is a START_WCDMA_MODE

DEPENDENCIES
  none

===========================================================================*/
boolean ftm_wcdma_ber_is_rrc_cmd_start_wcdma_mode(void)
{
  rrc_cmd_type *msg;
  msg = q_check( &ftm_wcdma_ber_q );

  if((msg != NULL) && (msg->cmd_hdr.cmd_id == RRC_CPHY_START_WCDMA_MODE_CNF))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION       ftm_wcdma_ber_is_rrc_cmd_stop_wcdma_mode

DESCRIPTION    This function checks if the RRC command is a STOP_WCDMA_MODE

DEPENDENCIES
  none

===========================================================================*/
boolean ftm_wcdma_ber_is_rrc_cmd_stop_wcdma_mode(void)
{
  rrc_cmd_type *msg;
  msg = q_check( &ftm_wcdma_ber_q );

  if((msg != NULL) && (msg->cmd_hdr.cmd_id == RRC_CPHY_STOP_WCDMA_MODE_CNF))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION       ftm_wcdma_ber_remove_rrc_cmd_from_q

DESCRIPTION    This function removes RRC cmd from queue

DEPENDENCIES
  none

===========================================================================*/
void ftm_wcdma_ber_clear_rrc_cmd_q(void)
{
  rrc_cmd_type *msg;

  do
  {
    msg = q_get( &ftm_wcdma_ber_q);
    if( msg != NULL)
    {
      /* If msg not NULL then remove it from Q */
      rrc_free_cmd_buf( msg);
    }
  }while(msg != NULL);

}

#endif /* FEATURE_FTM_WCDMA_BER */
#endif /* FEATURE_FACTORY_TESTMODE */
