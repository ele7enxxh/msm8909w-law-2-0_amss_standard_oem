/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                   P S . C

DESCRIPTION
  This is the header file for the Data protocol Task. Contained herein are
  the functions needed to initialize all the modules that execute in PS task
  context and the main task processing loop.


EXTERNALIZED FUNCTIONS
  ps_task()
    Data Protocol Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the PS services can be used unless MC has created the PS task
  and ps_task() has been invoked.

Copyright (c) 2002-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ps.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/18/14    skc    Removing FEATURE_UW_FMC
10/20/14    vl     Removed the signal:PS_COMP_LOGGING_HELPER_SIGNAL and changed 
                   the handling to PS_COMP_LOGGING_HELPER_CMD
08/23/14    skc	   Removing FEATURE_DATA_PS_HDLC_PP
07/30/14    vl     Added support to handle NV refresh for SIM swap case 
                   without reset.
04/09/14    ash    Added Prefix Delegation support.
12/09/13    ss     Adding new Signal: PS_COMP_LOGGING_HELPER_SIGNAL
12/03/13    SaCh   Invoking IPA driver API to indicate that all UL static 
                   filter rules have been added at the end of psi_init
11/22/13    ds     Added support to read the ICMP reply rate parameters from 
                   EFS during startup.
11/20/13    rk     Removed feature mobileap.
11/03/13    cx     Support the second RM device in DUN call
03/02/13    pkp    Dog heart beat implementation.
12/10/12    ssh    IPv6 EFS initialization in ps_efs_init
10/31/12   bb/fn   Moved EFS NV item creation to call ps_efs_init
11/19/12    fn     Moved eap_svc_init to psi_init for RCINIT handshake to 
                   complete so GSDI task is ready for MMGSDI API calls
10/09/12    sj     8 PDN support changes
03/08/12    su     Changes for VToLTE feature
11/17/11    sa     Added support for V6 LO_MODE_B_IFACE.
10/01/11   bvd     Adding rcinit featurization to task function
2011-06-24 scb     Changed CONFIG file name info as part of adding support for
                   EFS NV is_fmc_enabled
05/06/11    hm     Use ps_task_crit_section for all PS_task operations. 
03/22/11    ss     Adding F3 before ASSERT in case signals are left 
                   unprocessed in main loop.
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
10/26/10    op     Added functions to use EFS items.
10/07/10    am     Added processing for Split Tx signal in SoftAp mode.
09/27/10    ss     DYNAMIC WDOG support for DATA tasks.
08/11/10    ss     Rearchitectured PS extension signal mechanism. Any number
                   of signals supported now.
06/15/10    vs     Added code to handle new signals for dual IP support in QMI
04/21/10    sa     Added code to handle PS_3GPP_LO_B_UM_RX_Q_SIGNAL signal.
04/08/10    vk     Added PS QTF sychronization signal
03/16/10    sy     Changed task_* api to tmc_task* api.
02/08/10    sa     Added code to handle PS_3GPP_RMSM_AT_UM_RX_Q_SIGNAL signal.
08/19/09    pg     Added code to handle WAPI Tx packet signal.
05/14/09    pp     Moved Local Heap allocations to Modem Heap. No longer need
                   to call memheap_init for DHCP4/6 clients.
03/26/09    pp     CMI De-featurization.
02/19/09    am     Moved ps_mmgsdi_client_init from PS to DCC Task.
01/29/09    pp     Added support for SLIP interface.
11/28/08    pp     ps_task: Used more syncronized constructs to handle
                   critical section.
10/15/08    scb    Fixed featurization issues for PS_RX_UICC_SIGNAL handling
10/02/08    ssh    Fixed featurization issue for PS_RX_UICC_SIGNAL handling
09/29/08    scb    Fixed crash on END key was press for FEATURE_REX_SIGS_EXT
09/04/08    am     Added ps_mmgsdi_client init.
08/04/08    ssh    Added PS_RX_UICC_SIGNAL for UICC
04/29/08    am     Updated Arbitration/Policy Manager now
                   supports OMH phase-2.
12/19/07    scb    Added protocol processor framer and deframer complete signals.
10/11/07    mct    Added feature FEATURE_GSNDCP_USES_COMPTASK around PDP
                   signals.
09/07/07    scb    Added REX signal ext code under FEATURE_REX_SIGS_EXT
02/27/07    scb    Added changes to fix RCVT compilation warning
02/28/07    ss/sv  Initialize ping module.
01/29/07    hm     Moved initialization not dependent upon NV to psi_powerup()
09/12/06    jd     Support for multiplexing RX signals 32-63 on generic rx sig
08/30/06    es     Added initialization for DHCPv6 client.
08/25/06    tbh    Added initialization the link local address manager module
08/23/06    ks     Using a separate RX signal for Rmnet, instead of Rmsm one.
04/24/06    mct    Removed tick timer code from the old pstimer implementation.
04/19/06    mct    Removed PDP sigs not being used as part of signal cleanup.
04/14/06    ss     Added code to handle DVB-H rx data queue signal
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
12/05/05    msr    Added support for PS stats logging
11/28/05    ifk    Added calls to initialize IPHC and ROHC at powerup to support
                   them for PPP.  These function calls are featurized under
                   FEATURE_DATA_PS_PPP_IPHC and FEATURE_DATA_PS_PPP_ROHC.
09/28/05    mct    Added support for new PS_FLO_BC_RX_DATA_Q_SIGNAL signal.
08/15/05    ks     Changed PS_SOCKET_FLOW_ENABLED_SIG to CMD.
06/12/05    hba    Merge EAP initialization and 802.1X Tx packet sig handler
05/11/05    sv     Movied IPSEC IFACE initialization to IPSEC task.
04/17/05    msr    Added ps_flow_event_init().
03/15/05    tmr    Added code to support dial-up networking iface for MSM7500
                   apps processor
02/25/05    sv     Added calls to IPFILTER INIT and ipsec init functions.
12/13/04    hba    Added snippet of code for WLAN RX Signal processing...
12/08/04    lyr    Included dhcp_client.h for DHCP initialization
11/15/04    aku    Added support for WLAN.
10/31/04    msr/ks Added call to dpl_init() in psi_init() to initialize
                   Data Protocol Logging (DPL).
10/26/04    ifk    Replaced call to ip_reasm_init() with ip_fraghdl_init()
10/26/04    mct    Added processing for three new signals
                   PS_PDP3/4/5_UM_RX_Q_SIGNAL.
10/06/04    msr    Added a counter to the inner loop of task loop to ensure
                   that loop terminated in case an unhandled signal is caught
08/23/04    jd     Renamed PS_PPP_RM_RX_SIGNAL to PS_RM_RX_SIGNAL
08/23/04    sv     Added calls to dss_powerup_init and dssps_init in powerup
                   and init functions.
08/02/04    mct    Fixed init of dssps_init, wherein it was attempting to
                   initialize NV items before the NV task was running.
07/16/04    vp     Added code for initializing token buckets for ICMP and
                   ICMPv6 rate limiting.
06/08/04    vr     Added processing of PS_BC_RX_DATA_Q_SIGNAL
04/30/04    mct    Fixed some lint errors.
04/14/04    vp     Changes to get keepalive_idle_time from NV.
02/09/04    rc     Removed inclusion of ps707_dssnet.h and call to
                   dssnet_sm_init().
02/04/04    ak     Call ps_phys_link_event_init() at initialization.
02/04/04    aku    Changed call to psi_get_nv_item() to ps_get_nv_item() and
                   call to psi_put_nv_item to ps_put_nv_item()
02/03/04    usb    Do not add Lo and Ipsec ACL's here. Do it in respective
                   init functions, order not important anymore, taken care of
                   by appropriate class priority.
02/02/04    jd     Removed MIP signal handling (use commands now)
01/27/04    vp     Modified psi_init to get the value of
                   tcp_graceful_dormant_close flag from the NV item
01/09/04    mct    Moved dssps_init() to psi_powerup() to init queues at
                   powerup instead of in psi_init
10/25/03    ifk    Added call to ppp_task_init()
10/07/03    vsk    Code to handle data on UM iface #3 PS_PDP2_UM_RX_Q_SIGNAL
08/25/03    aku    Initialize ps_mem in psi_powerup() instead of psi_init().
08/15/03    aku    Added calls to initialize ps memory and ps_iface events.
06/24/03    rc     Fixed compiler warning by including header file gsndcp.h.
05/27/03    om     Changed powerup to explicitly order lo and ipsec ACL's.
05/21/03    vas    Added support for HDR. Processing HDR auth signal
04/15/03    ss     Added PS_LO_TX_DATA_Q_SIGNAL processing
02/12/03    om     Added Loopback Device initialization.
02/06/03    mvl    Initializing IP fragmentation at power up.
12/30/02    jd     Removed mip signals from internal enabled mask, done at
                   runtime in ps_mip_task
12/29/02    jd     Moved mip_perf_init() into mip_meta_sm_init()
12/29/02    jd     Add PS_MIP_DMU_RAND_SIGNAL to enabled mask
                   Handle MIP signals via registered callbacks
11/25/02    usb    Included header file for dsmip_perf
11/19/02    jd     Featurized reference to MIP signals
11/14/02    jd     Added commands for mobile ip subtask
11/13/02    rc     Added support for processing SNDCP signals.
11/01/02    ifk    Removed call to dss_init_dns_cache_tbl(), added
                   dss_dns_init()
10/07/02    aku    Added support for processing PS_SOCKET_FLOW_ENABLED_SIG
10/03/02    rc     Added support for processing PDP signals.
09/03/02    usb    Wrapped dssnet specific calls in FEATURE_DATA_IS707
08/06/02    ak     Updated for new task interface.
08/05/02    usb    Externalized ps_dog_rpt_timer since its used in psi.c
07/31/02    usb    Renamed psmisc.h to ps_utils.h
07/25/02    mvl    Updates for PPP renaming.
07/17/02    aku    Added support for processing PS_SOCKET_RX_Q_SIGNAL
07/16/02    atp    Added processing for PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL.
07/15/02    aku    Removed the if 0 in psi_init() and added sockets
                   initialization
06/22/02    aku    Added support for sockets calls
03/14/02    usb    Created Initial Version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "task.h"
#include "rex.h"
#include "queue.h"
#include "msg.h"
#include "err.h"
#include "stringl.h"
#include "dog_hb_rex.h"

#include "ps.h"
#include "ps_crit_sect.h"
#include "ps_svc.h"
#include "psi.h"
#include "ps_utils.h"
#include "ps_ip_fraghdl.h"
#include "ps_lo.h"

#include "ps_utils_init.h"
#include "ps_netiface_init.h"
#include "ps_inet_init.h"
#include "ps_sock_init.h"
#include "dss_init.h"
#include "ds_Net_Init.h"
#include "ds_Sock_Init.h"
#include "ps_icmp.h"
#include "ps_stat_logging.h"
#include "ps_stat_commoni.h"
#include "ps_sock_mem_pool.h"
#include "ds_Utils_DebugMsg.h"

#ifdef FEATURE_DATA_PS_PPP
#include "ps_pppi.h"
#endif

#ifdef FEATURE_DATA_PS_HDLC
#include "ps_hdlc_libi.h"
#endif

#include "ps_ifacei_addr_v6.h"


#include "ps_mcasti.h"

#include "ps_rsvpi.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "ps_mip_taski.h"
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_GSM_GPRS
#include "gsndcp.h"
#endif /* FEATURE_GSM_GPRS */

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ip6i_sm.h"
#include "ps_icmp6_nd.h"
#include "ps_iface_addr_v6.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_clat_sm.h"
#endif /* FEATURE_DATA_PS_464XLAT */
#include "ps_dns_proxy_ext.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_AUDIO_OFFLOAD
#include "PS_Audio_Offload_Client.h"
#endif /*FEATURE_DATA_AUDIO_OFFLOAD*/

#include "ps_logging_diag.h"

#include "ps_ll_addr_mgri.h"

#if defined(FEATURE_WLAN) || defined(FEATURE_DATA_RM_NET)
#include "ps_lan_llci.h"
#endif

#include "ps_sys.h"

#include "rcinit_rex.h"

#include "ps_modem_link_local.h"

#include "dhcp.h"

#include "ps_dpm.h"
#include "ps_dpm_hw.h"

#include "ps_dpm_utils.h"
#include "ps_proto_init.h"

#ifdef FEATURE_DATA_IPA
#include "ipa_ipfltr.h"
#endif /* FEATURE_DATA_IPA */

#include "ps_hashtable.h"

#ifdef FEATURE_DATA_ATP
#include "ds_atp_internal.h"
#endif /* FEATURE_DATA_ATP */

rex_tcb_type  * rex_ps_tcb;

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*--------------------------------------------------------------------------
  Dog data structure for PS task
---------------------------------------------------------------------------*/
uint32  ps_dog_rpt_id = 0;  /*! dog report ID */

uint8  num_used_3gpp2_v4_ifaces = 0; /* Can be moved to iwlan files? */

/*! 
  Defines the PS task critical section.
*/
ps_crit_sect_type ps_task_crit_section;

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
/*===========================================================================
FUNCTION PSI_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the data Protocol task. This
  function will perform all first time thru functions for the Protocol task.
  Note that initializations internal to PS should be performed before
  initializing other components that run in PS task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void psi_powerup
(
  void
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("psi_powerup(): "
                  "Protocol Service Task Powerup");

  /*-------------------------------------------------------------------------
    Initialize the PS task critical section.
  -------------------------------------------------------------------------*/
  PS_INIT_CRIT_SECTION(&ps_task_crit_section);

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  (void) q_init( &psi_cmd_q );
  (void) q_init( &psi_cmd_free_q );

  for( i = 0; i < PSI_CMD_BUF_CNT; i++ )
  {
    q_put( &psi_cmd_free_q, q_link( &psi_cmd_buf[i], &psi_cmd_buf[i].link) );
  }

  /*------------------------------------------------------------------------
    Initialize all the signal and cmd handlers
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);

  for( i = 0; i < (int)PS_MAX_SIGNALS; i++ )
  {
    psi_sig_handler[i].sig_handler = default_sig_handler;
    psi_sig_handler[i].user_data_ptr = NULL;
  }

  for( i = 0; i < (int)PS_MAX_DEFINED_CMD_TYPES; i++ )
  {
    psi_cmd_handler[i] = default_cmd_handler;
  }

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to PS Task
  -------------------------------------------------------------------------*/
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */

  PS_SET_SIG(psi_enabled_sig_mask, PS_TASK_OFFLINE_SIGNAL);
  PS_SET_SIG(psi_enabled_sig_mask, PS_CMD_Q_SIGNAL);
  PS_SET_SIG(psi_enabled_sig_mask, PS_DOG_HB_REPORT_SIGNAL);
  PS_SET_SIG(psi_enabled_sig_mask, PS_SIG_MASK_CHANGE_SIGNAL);

  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);

  /*--------------------------------------------------------------------------
    Perform the power up initialization for PS Modules
  --------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Initialize the modules common to all platforms.
    Intitialization order is important.
    1. PS Sys Init
    2. Utils is independent, needs to be initialized before everyone else.
    3. Netiface after that.
    4. Inet after that.
    5. Pssock after that
    6. DSNET/DSSOCK after that.
    7. DSS at the end. DSS calls into DSNET/DSSOCK.
  -------------------------------------------------------------------------*/
  ps_sys_init();  
  ps_utils_powerup();
  ps_netiface_powerup();
  ps_inet_powerup();
  ps_sock_powerup();

  /*-------------------------------------------------------------------------
    Initialize rearch modules.
  -------------------------------------------------------------------------*/
  DSNetPowerup();
  DSSockPowerup();
  dss_powerup();

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    Initialize IPv6 state machine command handlers.
  -------------------------------------------------------------------------*/
  ip6_sm_powerup_init();
  ps_iface_addr_v6_init();
  ps_icmp6_nd_init();
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_PS_HDLC
  ps_hdlc_lib_init();

  /*-------------------------------------------------------------------------
    Initialize the hdlc subsystem
  -------------------------------------------------------------------------*/
  (void) hdlc_powerup_init();
#endif

#ifdef FEATURE_DATA_PS_PPP
  ppp_task_powerup_init();
#endif

  ip_fraghdl_init();

#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    Mobile IP command handlers
  -------------------------------------------------------------------------*/
  mip_task_powerup_init();

#endif /* FEATURE_DS_MOBILE_IP */
  /*------------------------------------------------------------------------
    Initialise the PS hosted GPRS SNDCP module.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_GSM_GPRS
#ifndef FEATURE_GSNDCP_USES_COMPTASK
  gsndcp_initialise();
#endif /* FEATURE_GSNDCP_USES_COMPTASK */
#endif /* FEATURE_GSM_GPRS */

  /*-------------------------------------------------------------------------
    Initialize Link Local Address Mgr
  -------------------------------------------------------------------------*/
  ll_addr_mgr_init();

#if !defined(IMAGE_APPS_PROC) || !defined(FEATURE_WINMOB_SIM)
#if defined(FEATURE_WLAN) || defined(FEATURE_DATA_RM_NET)

  /*-------------------------------------------------------------------------
    Initialize LLC (LLC in turn initializes ARP).
  -------------------------------------------------------------------------*/
  lan_llc_init();

#endif /* defined(FEATURE_WLAN) || defined(FEATURE_DATA_RM_NET) */
#endif /* !defined(IMAGE_APPS_PROC) || !defined(FEATURE_WINMOB_SIM) */

  ps_mcasti_init();

  ps_rsvpi_init();

  /*-------------------------------------------------------------------------
    Initialize the loopback interface
  -------------------------------------------------------------------------*/
  lo_powerup_init();

  /*-------------------------------------------------------------------------
    Initialize the Hashtable control blocks
  -------------------------------------------------------------------------*/
  ps_ht_powerup_init();

  /*------------------------------------------------------------------------- 
    Initialize the modem link local interface
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  ps_modem_link_local_powerup_init();
#endif

  ps_dpm_hw_dpl_powerup_init();
} /* psi_powerup() */


/*===========================================================================
FUNCTION PSI_INIT()

DESCRIPTION
  Initialization function for the data services task. This function
  performs the functions needed for the data task to exit disabled state.

DEPENDENCIES
  PS task should have been started.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void psi_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*-------------------------------------------------------------------------
   Initialize Refresh notification framework
 -------------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  ps_utils_refresh_init();
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

  /*-------------------------------------------------------------------------
    Initialize pssock module
  -------------------------------------------------------------------------*/
  ps_sock_init();  
  /*-------------------------------------------------------------------------
    Initialize DSS module
  -------------------------------------------------------------------------*/
  dss_init();

  /*-------------------------------------------------------------------------
    Initialize NETIFACE module
  -------------------------------------------------------------------------*/
  ps_netiface_init();

  /*-------------------------------------------------------------------------
    Initialize DPM module
  -------------------------------------------------------------------------*/
  ps_dpm_init();

  /*-------------------------------------------------------------------------
    Initialize EFS config file for DPM
  -------------------------------------------------------------------------*/  
  ps_dpm_init_efs_config_file();

  /*-------------------------------------------------------------------------
    Initialize DS modules
  -------------------------------------------------------------------------*/
  ps_utils_init();
  DSNetInit();
  DSSockInit();

/*------------------------------------------------------------------------- 
  Initialize the modem link local interface
-------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  ps_modem_link_local_init();
#endif

  dpl_init();

  ps_stat_init();
  ps_stat_logging_init();
  
 /*-------------------------------------------------------------------------
    Initialize all protocol modules
  -------------------------------------------------------------------------*/  
  ps_proto_init();
#ifdef FEATURE_DATA_PS_464XLAT
  dns_proxy_powerup();
  ps_clat_powerup();
#endif /* FEATURE_DATA_PS_464XLAT */

#ifdef FEATURE_DATA_ATP
  MSG_HIGH("atp, set uplink filter for filter_report_src_port", 0,0,0);
  if(!ds_atp_set_uplink_filter(32018))
  {
    MSG_ERROR(" failed to set uplink filter", 0, 0, 0);
  }
#endif /*FEATURE_DATA_ATP*/

#ifdef FEATURE_DATA_IPA  
  ipa_ipfltr_apply_ul_static_rules_done();
#endif /* FEATURE_DATA_IPA */

#ifdef FEATURE_DATA_AUDIO_OFFLOAD
  ps_audio_offload_client_powerup();
#endif

} /* psi_init() */


/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
/*===========================================================================

FUNCTION PS_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the Data
  Protocol Service(s) task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_task
(
  uint32 ignored
    /* Parameter received from Main Control task - ignored */
)
{
  /*lint -e506 */
  rex_sigs_type     loop_sigs[PS_MAX_SIG_ARR_LEN]; /* Signals to be processed
                                           within a task loop */
  /*lint -e506 */
  rex_sigs_type     set_sigs[PS_MAX_SIG_ARR_LEN]; /* Currently set signals */
  boolean           sig_result = FALSE; /*The result value of the extension
                                          signal API is stored in this var.*/
  uint32            loop_cnt;       /* # of times task loop is iterated
                                         on a signal mask                  */
  uint32            i = 0, j = 0;

#if (defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE))
  #error code not present
#endif /* (defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE)) */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    In order to avoid lint warning.
  -------------------------------------------------------------------------*/
  (void)ignored;
  rex_ps_tcb = rex_self();

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /*-------------------------------------------------------------------------
    Dog heart beat registration
  -------------------------------------------------------------------------*/
  ps_dog_rpt_id        = dog_hb_register_rex(
                 (rex_sigs_type)1 << (rex_sigs_type)PS_DOG_HB_REPORT_SIGNAL);


  psi_init(); /* Refactor! */
  
  rcinit_handshake_startup();

  memset(set_sigs, 0, sizeof(set_sigs));

  /*-------------------------------------------------------------------------
    Main task loop. Never exit!
  -------------------------------------------------------------------------*/
  for( ;;)
  {
    /*-----------------------------------------------------------------------
      Wait for work to do. No return from wait until one of the enabled
      signals is set. Even if we come out of wait, process only those
      signals which are currently enabled.
    -----------------------------------------------------------------------*/
    memscpy(loop_sigs, sizeof(loop_sigs), set_sigs, sizeof(set_sigs));

#ifdef FEATURE_REX_SIGS_EXT
  
    sig_result = rex_get_sigs_ext(rex_ps_tcb, set_sigs);

    if(sig_result == FALSE)
    {
      DATA_ERR_FATAL("Failed to obtain the signals from the PS TCB");
      return;
    }
#else

    set_sigs[0] = rex_get_sigs( rex_ps_tcb );
  
#endif
    /*-----------------------------------------------------------------------
      Set the extension signal part in set_sigs array if set.
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION(&ps_task_crit_section);
    memscpy(&(set_sigs[PS_SIG_ARR_LEN]), sizeof(set_sigs), ps_ext_sig_mask, 
             sizeof(ps_ext_sig_mask));
    PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);

    /*-----------------------------------------------------------------------
      Restore sigs set from the previous processing loop.
      (We copied set_sigs content into loop_sigs above)
    -----------------------------------------------------------------------*/
    /*lint -e506 */
    for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
    {
      set_sigs[i] |= loop_sigs[i];
    }

    /*-----------------------------------------------------------------------
      Check if any of the enabled signal is set.
    -----------------------------------------------------------------------*/
    sig_result = FALSE;
    /*lint -e506 */
    for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
    {
      set_sigs[i] &= psi_enabled_sig_mask[i];
      if(0 != set_sigs[i])
      {
        sig_result = TRUE;
      }
    }

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( FALSE == sig_result )
    {
#ifdef FEATURE_REX_SIGS_EXT
      sig_result = rex_wait_ext(psi_enabled_sig_mask, set_sigs);
      if(sig_result == FALSE)
      {
        DATA_ERR_FATAL("ps_task():rex_wait_ext() failed.");
        return;
      }
#else
      set_sigs[0] = rex_wait( psi_enabled_sig_mask[0] );
#endif
      PS_ENTER_CRIT_SECTION(&ps_task_crit_section);
      /*---------------------------------------------------------------------
        Read extension signal part in set_sigs array if set.
      ---------------------------------------------------------------------*/
      memscpy(&(set_sigs[PS_SIG_ARR_LEN]), sizeof(set_sigs), ps_ext_sig_mask, 
               sizeof(ps_ext_sig_mask));
      PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);

      /*-----------------------------------------------------------------------
        We need to process only the enabled signals.
      -----------------------------------------------------------------------*/
      /*lint -e506 */
      for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
      {
        set_sigs[i] &= psi_enabled_sig_mask[i];
      }
    }

#ifdef FEATURE_REX_SIGS_EXT

    sig_result = rex_clr_sigs_ext(rex_ps_tcb, set_sigs);

    if(sig_result == FALSE)
    {
      DATA_ERR_FATAL("ps_task():rex_clr_sigs_ext() failed.");
      return;
    }
#else
  
    (void)rex_clr_sigs( rex_ps_tcb, set_sigs[0] );
  
#endif
    /*-----------------------------------------------------------------------
      Now clear the extension signals which we will be processing in this 
      loop.
    -----------------------------------------------------------------------*/
    /*lint -e506 */
    PS_ENTER_CRIT_SECTION(&ps_task_crit_section);
    for(i=0, j=PS_SIG_ARR_LEN; i<PS_EXT_SIG_ARR_LEN; i++, j++)
    {
      ps_ext_sig_mask[i] &= ~set_sigs[j];
    }
    PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);
    /*-----------------------------------------------------------------------
      Clear the signals which don't need to be processed explicitly.
    -----------------------------------------------------------------------*/
    PS_CLR_SIG(set_sigs, PS_SIG_MASK_CHANGE_SIGNAL);
    PS_CLR_SIG(set_sigs, PS_EXT1_RX_SIGNAL); 

    /*-----------------------------------------------------------------------
      Copy set_sigs into loop_sigs for processing and clear set_sigs.
    -----------------------------------------------------------------------*/
    memscpy(loop_sigs, sizeof(loop_sigs), set_sigs, sizeof(set_sigs));
    memset(set_sigs, 0, sizeof(set_sigs));

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than PS_MAX_SIGNALS times, some signal is
      not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    sig_result = TRUE;
    while( sig_result && loop_cnt++ < (int)PS_MAX_SIGNALS )
    {
      sig_result = FALSE;
      /*---------------------------------------------------------------------
        If dog heart beat received, ack the beat. Play ping<->pong.
      ---------------------------------------------------------------------*/
      if(PS_IS_SET_SIG(loop_sigs, PS_DOG_HB_REPORT_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_DOG_HB_REPORT_SIGNAL);
        dog_hb_report( ps_dog_rpt_id );
      }

#ifdef FEATURE_WLAN
      /*---------------------------------------------------------------------
        Incoming WLAN data signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_WLAN_RX_DATA_Q_SIGNAL ))
      {
        PS_CLR_SIG(loop_sigs, PS_WLAN_RX_DATA_Q_SIGNAL);
        if ( psi_sig_handler[PS_WLAN_RX_DATA_Q_SIGNAL].sig_handler
             (
               PS_WLAN_RX_DATA_Q_SIGNAL,
               psi_sig_handler[PS_WLAN_RX_DATA_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_WLAN_RX_DATA_Q_SIGNAL);
        }
      }

      /*---------------------------------------------------------------------
        Incoming 802.1X EAPOL signal for xmit over LLC
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_LAN_802_1X_TX_DATA_Q_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_LAN_802_1X_TX_DATA_Q_SIGNAL);
        if ( psi_sig_handler[ PS_LAN_802_1X_TX_DATA_Q_SIGNAL].sig_handler
             (
               PS_LAN_802_1X_TX_DATA_Q_SIGNAL ,
               psi_sig_handler[PS_LAN_802_1X_TX_DATA_Q_SIGNAL ].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_LAN_802_1X_TX_DATA_Q_SIGNAL);
        }
      }

      /*---------------------------------------------------------------------
        Incoming WAPI data signal for xmit over LLC
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_LAN_WAPI_TX_DATA_Q_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_LAN_WAPI_TX_DATA_Q_SIGNAL);
        if ( psi_sig_handler[ PS_LAN_WAPI_TX_DATA_Q_SIGNAL].sig_handler
             (
               PS_LAN_WAPI_TX_DATA_Q_SIGNAL ,
               psi_sig_handler[PS_LAN_WAPI_TX_DATA_Q_SIGNAL ].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_LAN_WAPI_TX_DATA_Q_SIGNAL);
        }
      }

#endif /* FEATURE_WLAN */

#ifdef FEATURE_DATA_PS_PPP
      /*---------------------------------------------------------------------
        Incoming PPP data signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_PPP_UM_SN_RX_SIGNAL ))
      {
        PS_CLR_SIG(loop_sigs, PS_PPP_UM_SN_RX_SIGNAL);
        if ( psi_sig_handler[PS_PPP_UM_SN_RX_SIGNAL].sig_handler
             (
               PS_PPP_UM_SN_RX_SIGNAL,
               psi_sig_handler[PS_PPP_UM_SN_RX_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_PPP_UM_SN_RX_SIGNAL);
        }
      }
#endif

#if defined(FEATURE_HDR) && defined(FEATURE_DATA_PS_PPP)
      /*---------------------------------------------------------------------
        Incoming PPP data signal for the HDR AN Authentication PPP
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_PPP_UM_AN_RX_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_PPP_UM_AN_RX_SIGNAL);
        if ( psi_sig_handler[PS_PPP_UM_AN_RX_SIGNAL].sig_handler
             (
               PS_PPP_UM_AN_RX_SIGNAL,
               psi_sig_handler[PS_PPP_UM_AN_RX_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_PPP_UM_AN_RX_SIGNAL);
        }
      }

#endif /* FEATURE_HDR */

#ifdef FEATURE_DATA_PS_SLIP
      /*---------------------------------------------------------------------
        Incoming SLIP signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs,PS_SLIP_RX_DATA_Q_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs,PS_SLIP_RX_DATA_Q_SIGNAL);

        if ( psi_sig_handler[PS_SLIP_RX_DATA_Q_SIGNAL].sig_handler
             (
               PS_SLIP_RX_DATA_Q_SIGNAL,
               psi_sig_handler[PS_SLIP_RX_DATA_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_SLIP_RX_DATA_Q_SIGNAL);
        }
      }
#endif /* FEATURE_DATA_PS_SLIP */

#ifdef FEATURE_BCMCS
      /*---------------------------------------------------------------------
        Incoming BCMCS signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_BC_RX_DATA_Q_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_BC_RX_DATA_Q_SIGNAL);
        if ( psi_sig_handler[PS_BC_RX_DATA_Q_SIGNAL].sig_handler
             (
               PS_BC_RX_DATA_Q_SIGNAL,
               psi_sig_handler[PS_BC_RX_DATA_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_BC_RX_DATA_Q_SIGNAL);
        }
      }

#endif /* FEATURE_BCMCS */

#ifdef FEATURE_MFLO
      /*---------------------------------------------------------------------
        Incoming FLO BCMCS signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_FLO_BC_RX_DATA_Q_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_FLO_BC_RX_DATA_Q_SIGNAL);
        if ( psi_sig_handler[PS_FLO_BC_RX_DATA_Q_SIGNAL].sig_handler
             (
               PS_FLO_BC_RX_DATA_Q_SIGNAL,
               psi_sig_handler[PS_FLO_BC_RX_DATA_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_FLO_BC_RX_DATA_Q_SIGNAL);
        }
      }

#endif /* FEATURE_MFLO */

#ifdef FEATURE_DTV_DVBH
      /*---------------------------------------------------------------------
        Incoming DVB-H signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_DVBH_RX_DATA_Q_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_DVBH_RX_DATA_Q_SIGNAL);
        if ( psi_sig_handler[PS_DVBH_RX_DATA_Q_SIGNAL].sig_handler
             (
               PS_DVBH_RX_DATA_Q_SIGNAL,
               psi_sig_handler[PS_DVBH_RX_DATA_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DVBH_RX_DATA_Q_SIGNAL);
        }
      }

#endif /* FEATURE_DTV_DVBH */

#ifdef FEATURE_GSM_GPRS
#ifndef FEATURE_GSNDCP_USES_COMPTASK

      /*---------------------------------------------------------------------
        Incoming SNDCP LL DL Data Q signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_SNDCP_LL_DL_Q_SIGNAL ))
      {
        PS_CLR_SIG(loop_sigs, PS_SNDCP_LL_DL_Q_SIGNAL);
        if ( psi_sig_handler[PS_SNDCP_LL_DL_Q_SIGNAL].sig_handler
             (
               PS_SNDCP_LL_DL_Q_SIGNAL,
               psi_sig_handler[PS_SNDCP_LL_DL_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_SNDCP_LL_DL_Q_SIGNAL);
        }
      }
#endif /* FEATURE_GSNDCP_USES_COMPTASK */
#endif/* FEATURE_GSM_GPRS */
      /*---------------------------------------------------------------------
        Incoming TE2 data signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_RM_RX_SIGNAL ))
      {
        PS_CLR_SIG(loop_sigs, PS_RM_RX_SIGNAL);
        if ( psi_sig_handler[PS_RM_RX_SIGNAL].sig_handler
             (
               PS_RM_RX_SIGNAL,
               psi_sig_handler[PS_RM_RX_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RM_RX_SIGNAL);
        }
      }
      /*---------------------------------------------------------------------
        Incoming second TE2 data signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_RM_2_RX_SIGNAL ))
      {
        PS_CLR_SIG(loop_sigs, PS_RM_2_RX_SIGNAL);
        if ( psi_sig_handler[PS_RM_2_RX_SIGNAL].sig_handler
             (
               PS_RM_2_RX_SIGNAL,
               psi_sig_handler[PS_RM_2_RX_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RM_2_RX_SIGNAL);
        }
      }

      /*---------------------------------------------------------------------
        RX signal processing
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_RX_RMNET1_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_RX_RMNET1_SIGNAL);
        if ( psi_sig_handler[PS_RX_RMNET1_SIGNAL].sig_handler
             (
               PS_RX_RMNET1_SIGNAL,
               psi_sig_handler[PS_RX_RMNET1_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RX_RMNET1_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_RX_RMNET2_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_RX_RMNET2_SIGNAL);
        if ( psi_sig_handler[PS_RX_RMNET2_SIGNAL].sig_handler
             (
               PS_RX_RMNET2_SIGNAL,
               psi_sig_handler[PS_RX_RMNET2_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RX_RMNET2_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_0 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_0);
        if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_0].sig_handler
             (
               PS_3GPP_UM_RX_Q_SIGNAL_0,
               psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_0].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_0);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_1 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_1);
        if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_1].sig_handler
             (
               PS_3GPP_UM_RX_Q_SIGNAL_1,
               psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_1].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_1);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_2 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_2);
        if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_2].sig_handler
             (
               PS_3GPP_UM_RX_Q_SIGNAL_2,
               psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_2].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_2);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_3 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_3);
        if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_3].sig_handler
             (
               PS_3GPP_UM_RX_Q_SIGNAL_3,
               psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_3].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_3);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_4 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_4);
        if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_4].sig_handler
             (
               PS_3GPP_UM_RX_Q_SIGNAL_4,
               psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_4].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_4);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_5 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_5);
        if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_5].sig_handler
             (
               PS_3GPP_UM_RX_Q_SIGNAL_5,
               psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_5].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_5);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_6 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_6);
        if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_6].sig_handler
             (
               PS_3GPP_UM_RX_Q_SIGNAL_6,
               psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_6].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_6);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_7 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_UM_RX_Q_SIGNAL_7);
        if ( psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_7].sig_handler
             (
               PS_3GPP_UM_RX_Q_SIGNAL_7,
               psi_sig_handler[PS_3GPP_UM_RX_Q_SIGNAL_7].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_UM_RX_Q_SIGNAL_7);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_EMBMS_RX_Q_SIGNAL ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_EMBMS_RX_Q_SIGNAL);
        if ( psi_sig_handler[PS_3GPP_EMBMS_RX_Q_SIGNAL].sig_handler
             (
               PS_3GPP_EMBMS_RX_Q_SIGNAL,
               psi_sig_handler[PS_3GPP_EMBMS_RX_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_EMBMS_RX_Q_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_RMSM_AT_UM_RX_Q_SIGNAL ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_RMSM_AT_UM_RX_Q_SIGNAL);
        if ( psi_sig_handler[PS_3GPP_RMSM_AT_UM_RX_Q_SIGNAL].sig_handler
             (
               PS_3GPP_RMSM_AT_UM_RX_Q_SIGNAL,
               psi_sig_handler[PS_3GPP_RMSM_AT_UM_RX_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_RMSM_AT_UM_RX_Q_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_LO_B_UM_RX_Q_SIGNAL ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_LO_B_UM_RX_Q_SIGNAL);
        if ( psi_sig_handler[PS_3GPP_LO_B_UM_RX_Q_SIGNAL].sig_handler
             (
               PS_3GPP_LO_B_UM_RX_Q_SIGNAL,
               psi_sig_handler[PS_3GPP_LO_B_UM_RX_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_LO_B_UM_RX_Q_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_3GPP_LO_B_UM_RX_Q_SIGNAL_1 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_3GPP_LO_B_UM_RX_Q_SIGNAL_1);
        if ( psi_sig_handler[PS_3GPP_LO_B_UM_RX_Q_SIGNAL_1].sig_handler
             (
               PS_3GPP_LO_B_UM_RX_Q_SIGNAL_1,
               psi_sig_handler[PS_3GPP_LO_B_UM_RX_Q_SIGNAL_1].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_3GPP_LO_B_UM_RX_Q_SIGNAL_1);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_DPM_DL_SIGNAL_0 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_DPM_DL_SIGNAL_0);
        if ( psi_sig_handler[PS_DPM_DL_SIGNAL_0].sig_handler
             (
               PS_DPM_DL_SIGNAL_0,
               psi_sig_handler[PS_DPM_DL_SIGNAL_0].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPM_DL_SIGNAL_0);
        }
      }  
      else if(PS_IS_SET_SIG(loop_sigs, PS_DPM_DL_SIGNAL_1 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_DPM_DL_SIGNAL_1);
        if ( psi_sig_handler[PS_DPM_DL_SIGNAL_1].sig_handler
             (
               PS_DPM_DL_SIGNAL_1,
               psi_sig_handler[PS_DPM_DL_SIGNAL_1].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPM_DL_SIGNAL_1);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_DPM_DL_SIGNAL_2 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_DPM_DL_SIGNAL_2);
        if ( psi_sig_handler[PS_DPM_DL_SIGNAL_2].sig_handler
             (
               PS_DPM_DL_SIGNAL_2,
               psi_sig_handler[PS_DPM_DL_SIGNAL_2].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPM_DL_SIGNAL_2);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_DPM_DL_SIGNAL_3 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_DPM_DL_SIGNAL_3);
        if ( psi_sig_handler[PS_DPM_DL_SIGNAL_3].sig_handler
             (
               PS_DPM_DL_SIGNAL_3,
               psi_sig_handler[PS_DPM_DL_SIGNAL_3].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPM_DL_SIGNAL_3);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_DPM_DL_SIGNAL_4 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_DPM_DL_SIGNAL_4);
        if ( psi_sig_handler[PS_DPM_DL_SIGNAL_4].sig_handler
             (
               PS_DPM_DL_SIGNAL_4,
               psi_sig_handler[PS_DPM_DL_SIGNAL_4].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPM_DL_SIGNAL_4);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_DPM_DL_SIGNAL_5 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_DPM_DL_SIGNAL_5);
        if ( psi_sig_handler[PS_DPM_DL_SIGNAL_5].sig_handler
             (
               PS_DPM_DL_SIGNAL_5,
               psi_sig_handler[PS_DPM_DL_SIGNAL_5].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPM_DL_SIGNAL_5);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_DPM_DL_SIGNAL_6 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_DPM_DL_SIGNAL_6);
        if ( psi_sig_handler[PS_DPM_DL_SIGNAL_6].sig_handler
             (
               PS_DPM_DL_SIGNAL_6,
               psi_sig_handler[PS_DPM_DL_SIGNAL_6].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPM_DL_SIGNAL_6);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_DPM_DL_SIGNAL_7 ) )
      {
        PS_CLR_SIG(loop_sigs, PS_DPM_DL_SIGNAL_7);
        if ( psi_sig_handler[PS_DPM_DL_SIGNAL_7].sig_handler
             (
               PS_DPM_DL_SIGNAL_7,
               psi_sig_handler[PS_DPM_DL_SIGNAL_7].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPM_DL_SIGNAL_7);
        }
      }  
      
    #ifdef FEATURE_DATA_PS_UICC
      else if(PS_IS_SET_SIG(loop_sigs, PS_RX_UICC_SIGNAL ) )
      {
        PS_CLR_SIG(loop_sigs, PS_RX_UICC_SIGNAL);
        if ( psi_sig_handler[PS_RX_UICC_SIGNAL].sig_handler
             (
               PS_RX_UICC_SIGNAL,
               psi_sig_handler[PS_RX_UICC_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RX_UICC_SIGNAL);
        }
      }
    #endif /* FEATURE_DATA_PS_UICC */

      /*---------------------------------------------------------------------
        IS707 Async TX data available signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL);
        if ( psi_sig_handler[PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL].sig_handler
           (
            PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL,
            psi_sig_handler[PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL].user_data_ptr
           ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL);
        }
      }

      /*---------------------------------------------------------------------
       Sockets Send signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_SOCKET_RX_Q_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_SOCKET_RX_Q_SIGNAL);
        if ( psi_sig_handler[PS_SOCKET_RX_Q_SIGNAL].sig_handler
           (
            PS_SOCKET_RX_Q_SIGNAL,
            psi_sig_handler[PS_SOCKET_RX_Q_SIGNAL].user_data_ptr
           ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_SOCKET_RX_Q_SIGNAL);
        }
      }

#ifndef IMAGE_MODEM_PROC
      /*---------------------------------------------------------------------
        Proxy IFACE RX signal processing
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_RX_PROXY1_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_RX_PROXY1_SIGNAL);
        if ( psi_sig_handler[PS_RX_PROXY1_SIGNAL].sig_handler
             (
               PS_RX_PROXY1_SIGNAL,
               psi_sig_handler[PS_RX_PROXY1_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RX_PROXY1_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_RX_PROXY2_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_RX_PROXY2_SIGNAL);
        if ( psi_sig_handler[PS_RX_PROXY2_SIGNAL].sig_handler
             (
               PS_RX_PROXY2_SIGNAL,
               psi_sig_handler[PS_RX_PROXY2_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RX_PROXY2_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_RX_PROXY3_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_RX_PROXY3_SIGNAL);
        if ( psi_sig_handler[PS_RX_PROXY3_SIGNAL].sig_handler
             (
               PS_RX_PROXY3_SIGNAL,
               psi_sig_handler[PS_RX_PROXY3_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RX_PROXY3_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_RX_PROXY4_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_RX_PROXY4_SIGNAL);
        if ( psi_sig_handler[PS_RX_PROXY4_SIGNAL].sig_handler
             (
               PS_RX_PROXY4_SIGNAL,
               psi_sig_handler[PS_RX_PROXY4_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RX_PROXY4_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_RX_PROXY5_SIGNAL) )
      {
        PS_CLR_SIG(loop_sigs, PS_RX_PROXY5_SIGNAL);
        if ( psi_sig_handler[PS_RX_PROXY5_SIGNAL].sig_handler
             (
               PS_RX_PROXY5_SIGNAL,
               psi_sig_handler[PS_RX_PROXY5_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_RX_PROXY5_SIGNAL);
        }
      }
#endif /* IMAGE_MODEM_PROC */

      /*---------------------------------------------------------------------
        PS Task command queue signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_CMD_Q_SIGNAL ))
      {
        PS_CLR_SIG(loop_sigs, PS_CMD_Q_SIGNAL);
        if ( psi_process_cmd() == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_CMD_Q_SIGNAL);
        }
      }

#ifdef FEATURE_GSM_GPRS
#ifndef FEATURE_GSNDCP_USES_COMPTASK
      /*---------------------------------------------------------------------
        Incoming SNDCP CMD Q signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_SNDCP_CMD_Q_SIGNAL ))
      {
        PS_CLR_SIG(loop_sigs, PS_SNDCP_CMD_Q_SIGNAL);
        if ( psi_sig_handler[PS_SNDCP_CMD_Q_SIGNAL].sig_handler
             (
               PS_SNDCP_CMD_Q_SIGNAL,
               psi_sig_handler[PS_SNDCP_CMD_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_SNDCP_CMD_Q_SIGNAL);
        }
      }
#endif /* FEATURE_GSNDCP_USES_COMPTASK */
#endif/* FEATURE_GSM_GPRS */

      /*---------------------------------------------------------------------
        Process loop-back tx data queue signal
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_LO_TX_DATA_Q_SIGNAL ))
      {
        PS_CLR_SIG(loop_sigs, PS_LO_TX_DATA_Q_SIGNAL);
        if ( psi_sig_handler[PS_LO_TX_DATA_Q_SIGNAL].sig_handler
           (
            PS_LO_TX_DATA_Q_SIGNAL,
            psi_sig_handler[PS_LO_TX_DATA_Q_SIGNAL].user_data_ptr
           ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_LO_TX_DATA_Q_SIGNAL);
        }
      }

#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */

      /*---------------------------------------------------------------------
        Process packets from IPA for DPL
      ---------------------------------------------------------------------*/
      else if(PS_IS_SET_SIG(loop_sigs, PS_DPM_DPL_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_DPM_DPL_SIGNAL);
        if ( psi_sig_handler[PS_DPM_DPL_SIGNAL].sig_handler
             (
               PS_DPM_DPL_SIGNAL,
               psi_sig_handler[PS_DPM_DPL_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPM_DPL_SIGNAL);
        }
      }
      else if(PS_IS_SET_SIG(loop_sigs, PS_REV_IP_TRANSPORT_RX_DATA_Q_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_REV_IP_TRANSPORT_RX_DATA_Q_SIGNAL);
        if ( psi_sig_handler[PS_REV_IP_TRANSPORT_RX_DATA_Q_SIGNAL].sig_handler
             (
               PS_REV_IP_TRANSPORT_RX_DATA_Q_SIGNAL,
               psi_sig_handler[PS_REV_IP_TRANSPORT_RX_DATA_Q_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_REV_IP_TRANSPORT_RX_DATA_Q_SIGNAL);
        }
      }

      else if(PS_IS_SET_SIG(loop_sigs, PS_DPL_DIAG_SIGNAL))
      {
        PS_CLR_SIG(loop_sigs, PS_DPL_DIAG_SIGNAL);
        if ( psi_sig_handler[PS_DPL_DIAG_SIGNAL].sig_handler
             (
               PS_DPL_DIAG_SIGNAL,
               psi_sig_handler[PS_DPL_DIAG_SIGNAL].user_data_ptr
             ) == FALSE )
        {
          PS_SET_SIG(set_sigs, PS_DPL_DIAG_SIGNAL);
        }
      }

      /*---------------------------------------------------------------------
        Process other signals with registered handlers
      ---------------------------------------------------------------------*/
#ifdef TEST_FRAMEWORK
      #error code not present
#endif
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      if(PS_IS_SET_SIG(loop_sigs, PS_TASK_OFFLINE_SIGNAL))
      {
        LOG_MSG_INFO2_0("ps_task(): PS_TASK_OFFLINE_SIGNAL received");
        PS_CLR_SIG(loop_sigs, PS_TASK_OFFLINE_SIGNAL);
      }

      /*---------------------------------------------------------------------
        Check if we have processed all the signals.
      ---------------------------------------------------------------------*/
      /*lint -e506 */
      for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
      {
        if(0 != loop_sigs[i])
        {
          sig_result = TRUE;
        }
      }
    }

#if (defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE))
    #error code not present
#endif /* (defined(TEST_FRAMEWORK) && defined(FEATURE_QUBE)) */

    /*lint -e506 */
    for(i = 0; i < PS_MAX_SIG_ARR_LEN; i++)
    {
      if(0 != loop_sigs[i])
      {
        LOG_MSG_ERROR_2("ps_task(): "
                        "Signals not completely processed, loop_sigs[%d] = %d",
                        i, loop_sigs[i]);
        ASSERT(0);
      }
    }
  } /* forever */
} /* ps_task() */
