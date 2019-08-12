#ifndef PS_SVC_H
#define PS_SVC_H
/*===========================================================================

                        P S _ S V C . H

DESCRIPTION
  This is the header file for the Data protocol Task. Contained herein are
  all the definitions, functions, and structures for other software tasks
  to communicate with the Data Protocol Task.

EXTERNALIZED FUNCTIONS
  ps_enable_sig()
    Allow a signal to be processed

  ps_disable_sig()
    Disallow a signal to be processed

  ps_set_sig_handler()
    Set a handler to process a signal in PS context

  ps_send_cmd()
    Post a command to the Data Protocol Task

  ps_set_cmd_handler()
    Set a handler to process a cmd in PS context

Copyright (c) 1998-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_svc.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/18/14    skc    Removing usage of PS_RX_UW_FMC_SIGNAL
10/20/14    vl     Changed the PS_COMP_LOGGING_HELPER_SIGNAL to 
                   PS_COMP_LOGGING_HELPER_CMD
04/09/14    ash    Added Prefix Delegation support.
12/09/13    ss     Adding new Signal: PS_COMP_LOGGING_HELPER_SIGNAL
11/03/13    cx     Added support for second rm rx device
12/19/12    da     Added DNS flush cache commands
11/30/12    ss     Adding new command PS_DHCP_SERVER_STARTED
10/18/12    sd     Posting PS command for MIP registration when PS task is not running
10/09/12    sj     8 PDN support changes
01/04/12    hs     Added support for eMBMS
01/17/12    ea     Added commands for resolver's iface events and resolver
                   deletion.
11/28/11    cp     Stateless DHCPv6 server changes.
11/17/11    sa     Added signal for V6 LO_MODE_B_IFACE.
11/29/11    sun    Added new ps command for DPL time out  
10/17/11    ssh    eHRPD Always On support
10/01/11    bvd    Added System event command
09/20/11    cp     Added Broadcast GARP command. 
09/15/11    msr    Added new commands to do flow control in PS task 
06/06/11    ash    Added PPP_DEVICE_STOP enum to support PPP Partial Context
03/14/11    dm     Added Delete DMZ cmd.
12/16/10    am     Added ALG Mgr Tx signal.
12/05/10    ssh    Added PPP NCP Abort command
11/20/10    ssh    Added PPP NCP Resync command
10/23/10    jee    Added a new command for ps utils aka module
10/07/10    dm     Modifying enum for SoftAP split task as part of 
                   throughput optimizations.
06/30/10    rt     DNS Proxy commands added.
06/17/10    vs     Extra signals for simultaneous IPV4/IPV6 over QMI
05/03/10    cp     DHCP Soft AP changes.
04/21/10    sa     Added signal for LO_MODE_B_IFACE.
04/08/10    vk     Added PS QTF sychronization signal
01/15/10    sa     Added PS_3GPP_RMSM_AT_UM_RX_Q_SIGNAL for ATCoP data transfer. 
09/28/09    jee    Added commands for EAP-MMGSDI registration 
09/21/09   mga     Merged from eHRPD branch
09/03/09    pp     Added ps_wait, ps_wait_ext APIs.
08/19/09    pg     Added PS_LAN_WAPI_TX_DATA_Q_SIGNAL signal for WAPI Tx Data.
06/09/09    jy     Added 8 new enums for 3GPP in ps_sig_enum_type
03/26/09    pp     CMI De-featurization.
01/29/09    pp     Added support for SLIP interface.
12/14/08    pp     Common Modem Interface: Public/Private API split.
09/04/08    am     Added new cmd enum for SIM events.
08/04/08    ssh    Added RX signal for UICC
12/19/07    scb    Added protocol processor framer and deframer complete signals.
11/09/07    ssh    Added support for MIPv6
11/07/07    ks     Added 2 new signals for Rmnet for MCAST.
09/07/07    scb    Added REX signal ext code under FEATURE_REX_SIGS_EXT
06/26/07    rs     Added new enums to ps_cmd_enum_type to support multicast
                   JOIN and LEAVE.
05/04/07    hm     Added PS commands for new DNS API and DSS net manager.
04/12/07    ssh    Added PS command to send vendor-specific packets
04/11/07    msr    Added new cmd to handle DSM memory ctrl events
02/28/07    ss/sv  Added PS command to support PING api.
12/04/06   as/msr  MIP dereg support
11/21/06    rt     Added IGMP/MLD support for MBMS.
11/06/06    sv     Added PS command to process socket events.
09/12/06    jd     Support for multiplexing RX signals 32-63 on generic rx sig
08/30/06    es     Added DHCPv6 client enums.
08/25/06    tbh    Added new commands for Link Local addr mgr start,stop,claim
08/24/06    rt     Added a new command PS_DHCP_SERVER_SOCK_EVENT_CM.
08/23/06    ks     Using a separate RX signal for Rmnet, instead of the
                   RmSm one.
05/22/06    ks     Added route selection protocol cmd for EMPA.
05/02/06    mct    L4 APCs occur in DPC context. Added PS_TIMER_CALLBACK_CMD
                   cmd to ensure all timer client cbacks occur in PS context.
04/25/06    mct    Removed tick timer code from old pstimer implementation.
04/14/06    mct    Fixed sigs to not stomp on globally reserved signals.
04/14/06    ss     Added PS_DVBH_RX_DATA_Q_SIGNAL
03/27/06    ks     Removed PS_RX_UM_SN_CMD.
03/07/06    jd     Added LAN_LLC_RECONFIG command enum value
02/24/06    ks     Reordered some signals to restore previous correct values
                   for START, STOP and OFFLINE signals.
02/21/06    ksu    EMPA: hook up forward link with new phys link architecture
12/19/05    msr    Removed PS_SOCKET_FLOW_ENABLED_CMD
12/09/05    sv     Added support for new data path framework.
12/05/05    msr    Added support for PS stats logging
09/28/05    mct    Added PS_FLO_BC_RX_DATA_Q_SIGNAL signal for MediaFLO.
08/16/05    ks     Merged PS_ARP_FLUSH_CMD from tip
08/15/05    ks     Changed PS_SOCKET_FLOW_ENABLED_SIG to CMD.
08/15/05    ks     Removed PS_STATE_TIMER_SIGNAL, PS_MEM_EVENT_SIG_HANDLER
                   and PS_AN_RX_TIMEOUT_SIGNAL as not being used.
06/12/05    hba    Stole PDP6 signal for 802.1X Tx Signal.
04/17/05    msr    Added PS_SOCKET_FLOW_EVENT_CMD.
03/15/05    tmr    Added PS_DUN_PHYS_LINK_RX_DATA_SIG and dial-up networking
                   command definitions
03/14/05    clp    Added new cmd PS_DHCP_CLIENT_SOCK_EVENT_CMD
02/04/05    hba    Remove featurization of WLAN RX queue signal definition
01/07/05    clp    Added DHCP client and LLC command enums
12/23/04    lyr    Added WLAN_RX_SIGNAL
08/22/04    jd     Added Rm network interface receive signal definition
                   Renamed PS_PPP_RM_RX_SIGNAL to PS_RM_RX_SIGNAL
08/13/04    kvd    Added new cmd PS_MIP_PHYS_IFACE_UP_CMD.
07/30/04    ifk    Added IP6 SM commands, fixed numbering
07/15/04    ifk    Added a PPP resync command
05/25/04    vr     Changed PS_BC_TX_DATA_Q_SIGNAL to PS_BC_RX_DATA_Q_SIGNAL
05/18/04    jk     Added a signal for the R-UIM response.
05/16/04    vr     Added PS_BC_TX_DATA_Q_SIGNAL
04/30/04    mct    Fixed some lint errors.
02/10/04    rc     Removed DSSNET6 commands for IPv6.
02/02/04    jd     Removed MIP signal definitions
                   Added MIP command definitions
12/12/03    sv     Consolidated PS commands for iface events into one PS cmd.
10/24/03    sv     Added new PS commands to support transiet state call backs
                   for iface and phys link. Reordered cmds to group together.
10/07/03    vsk    Added PS_PDP2_UM_RX_SIGNAL - PS_PDP6_UM_RX_SIGNAL &
                   reordered signals to be grouped together
04/15/03    ss     Added PS_LO_TX_DATA_Q_SIGNAL
04/24/03    usb    Added PS_PPP_INJECT_PACKET_CMD for replaying a PPP packet
02/11/03    usb    Added commands for IP snooping state machine
01/24/03    usb    Added PS_PPP_SET_MODE_CMD for setting PPP mode
12/29/02    jd     Added PS_MIP_DMU_RAND_SIGNAL
12/24/02    aku    Added PS_SOCKET_PHYS_LINK_UP_EVENT_CMD and
                   PS_SOCKET_PHYS_LINK_DOWN_EVENT_CMD
12/22/02    aku    Added PS_SOCKET_NETWORK_UP_EVENT_CMD and
                   PS_SOCKET_NETWORK_DOWN_EVENT_CMD
11/19/02    jd     Added global ps_nv_status for use in PS task
11/14/02    jd     Added global ps_nv_item for use in PS task
                   Added MIP open and close commands
                   Removed unused PS_IFACE commands
11/13/02    rc     Added SNDCP signals for GPRS.
11/01/02    ifk    Added PS commands for DNS.
10/07/02    aku    Added PS_SOCKET_NETWORK_EVENT_CMD,
                   PS_SOCKET_IFACE_ADDR_CHANGED_CMD and
                   PS_SOCKET_FLOW_ENABLED_SIG.
10/03/02    rc     Added PDP signals for UMTS.
08/19/02    atp    Added PS_707_ASYNC_UM_FLOW_ENABLE_CMD for 707 async.
08/05/02    usb    Moved PS_NV_CMD_SIGNAL out of FEATURE_MIP
07/22/02    aku    Added PS_DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD
07/19/02    na     Fixed START/STOP sig enumeration
07/16/02    aku    Removed SOCKET featurization for dssnet cmds.
07/16/02    atp    Renamed signal PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL.
07/15/02    na     Removed FEATURE_DATA_PS and PS_SOCKET_STATE_TIMER_SIGNAL
07/13/02    atp    Added/changed names of commands, signals for 707 async.
03/14/02    usb    Initial version of file. Combined from other files.
===========================================================================*/
/*--------------------------------------------------------------------------
NOTE: No Platform indepdent header inclusion...eg: rex.h
--------------------------------------------------------------------------*/
#include "comdef.h"
#include "nv.h" /* Should be removed soon! */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
extern nv_item_type         ps_nv_item;
extern nv_stat_enum_type    ps_nv_status;

/*--------------------------------------------------------------------------
  Data Protocol Task Signal Definitions.  The value of existing signals
  should not be changed while adding or deleting a signal.  Some signals
  (marked Reserved) have fixed value that should stay as it is.
--------------------------------------------------------------------------*/
typedef enum
{
  PS_DPL_DIAG_SIGNAL              =  0,   /* Incoming TCP data signal      */
  PS_CMD_Q_SIGNAL                 =  1,   /* PS command Q signal           */
  PS_PPP_UM_SN_RX_SIGNAL          =  2,   /* Um PPP Rx data signal         */
  PS_RM_RX_SIGNAL                 =  3,   /* Rm PPP RX data signal         */
  PS_DOG_HB_REPORT_SIGNAL         =  4,   /* Dog heart beat report signal  */
  PS_SOCKET_RX_Q_SIGNAL           =  5,   /* Socket API has data to send   */
  
  PS_EXT1_RX_SIGNAL               =  6,   /* generic RX signal - interface(s)
                                             with data pending are set in
                                             ps_ext1_sig_mask              */
  
  PS_NV_CMD_SIGNAL                =  7,   /* NV cmd event signal           */
  PS_UIM_CMD_SIGNAL               =  8,
  PS_WLAN_RX_DATA_Q_SIGNAL        =  9,   /*WLAN ADP RX Data queue signal  */
  PS_TX_SPLIT_DATA_SIGNAL         = 10,   /* Split Signal for Soft AP      */
  PS_PPP_UM_AN_RX_SIGNAL          = 11,   /* HDR AN AUTH PPP RX            */
  PS_1X_UM_RX_Q_SIGNAL            = 12,

/*---------------------------------------------------------------------------
  BEWARE: DO NOT CHANGE ENUMERATION VALUE FOR OFFLINE, STOP AND START SIGS.
---------------------------------------------------------------------------*/
  PS_TASK_OFFLINE_SIGNAL          = 13,   /* Reserved TASK_OFFLINE_SIG     */
  PS_TASK_STOP_SIGNAL             = 14,   /* Reserved TASK_STOP_SIG        */
  PS_TASK_START_SIGNAL            = 15,   /* Reserved TASK_START_SIG       */

  PS_SNDCP_CMD_Q_SIGNAL           = 16,   /* GPRS CMD Q Signal             */
  PS_SNDCP_LL_DL_Q_SIGNAL         = 17,   /* GPRS DL Data Signal           */

  PS_LO_TX_DATA_Q_SIGNAL          = 18,   /* loopback data q signal        */

  /*------------------------------------------------------------------------
   Signals for Multiple PDP instances : Each signal is used to signal data
   arrival on a primary PDP context : Extend the signals when
  -------------------------------------------------------------------------*/
  PS_PDP0_UM_RX_Q_SIGNAL          = 19,   /* UMTS Um PDP0 Rx data Signal   */
  PS_PDP1_UM_RX_Q_SIGNAL          = 20,   /* UMTS Um PDP1 Rx data signal   */
  PS_PDP2_UM_RX_Q_SIGNAL          = 21,   /* UMTS Um PDP2 Rx data signal   */
  PS_PDP_MAX_UM_RX_Q_SIGNAL       = PS_PDP2_UM_RX_Q_SIGNAL, /* Max instance*/

  PS_RESV_ONCRPC_REPLY_SIGNAL     = 22,   /* Reserved for ONCRPC IPCs      */
  PS_DVBH_RX_DATA_Q_SIGNAL        = 23,   /* DVB-H Rx data q signal        */
  PS_SLIP_RX_DATA_Q_SIGNAL        = 24,   /* SLIP Rx data q signal         */
  PS_LAN_802_1X_TX_DATA_Q_SIGNAL  = 25,   /* 802.1X Tx Data queue signal   */
  PS_SIG_MASK_CHANGE_SIGNAL       = 26,   /* Indicates sig mask has changed*/

  PS_RESV_SEC_OP_COMP_SIGNAL      = 27,   /* Reserved Security signal      */

  PS_RESV_EFS_OP_COMP_SIGNAL      = 28,   /* Reserved EFS -  signal        */
  PS_RESV_DM_FLASH_DRV_SIGNAL     = 29,   /* Reserved Data Mover signal    */
  /*------------------------------------------------------------------------
    Extended Signals #1 (range 32-63) - RX Signals for PS interfaces

    Modules using these need to set/clear them using the PS_EXT
  -------------------------------------------------------------------------*/
  PS_EXT1_RX_SIGNAL_BASE          = 30,   /* PS RX signal base             */
  PS_RX_RMNET1_SIGNAL             = 30,   /* RmNet1 RX signal              */
  PS_RX_RMNET2_SIGNAL             = 31,   /* RmNet2 RX signal              */
  PS_RX_RMNET3_SIGNAL             = 32,   /* RmNet3 RX signal              */
  PS_RX_RMNET4_SIGNAL             = 33,   /* RmNet4 RX signal              */
  PS_RX_RMNET5_SIGNAL             = 34,   /* RmNet5 RX signal              */
  PS_HDLC_PP_FRAMER_SIGNAL        = 35,   /* HDLC PP Framer signal         */
  PS_HDLC_PP_DEFRAMER_SIGNAL      = 36,   /* HDLC PP Deframer signal       */
  PS_RX_UICC_SIGNAL               = 37,

  PS_RX_PROXY1_SIGNAL             = 38,   /* Proxy IFACE1 RX signal        */
  PS_RX_PROXY2_SIGNAL             = 39,   /* Proxy IFACE2 RX signal        */
  PS_RX_PROXY3_SIGNAL             = 40,   /* Proxy IFACE3 RX signal        */
  PS_RX_PROXY4_SIGNAL             = 41,   /* Proxy IFACE4 RX signal        */
  PS_RX_PROXY5_SIGNAL             = 42,   /* Proxy IFACE5 RX signal        */
  PS_3GPP_UM_RX_Q_SIGNAL_0        = 43,   /* 3GPP Bearer 0 signal          */
  PS_3GPP_UM_RX_Q_SIGNAL_1        = 44,   /* 3GPP Bearer 1 signal          */
  PS_3GPP_UM_RX_Q_SIGNAL_2        = 45,   /* 3GPP Bearer 2 signal          */
  PS_3GPP_UM_RX_Q_SIGNAL_3        = 46,   /* 3GPP Bearer 3 signal          */
  PS_3GPP_UM_RX_Q_SIGNAL_4        = 47,   /* 3GPP Bearer 4 signal          */
  PS_3GPP_UM_RX_Q_SIGNAL_5        = 48,   /* 3GPP Bearer 5 signal          */
  PS_3GPP_UM_RX_Q_SIGNAL_6        = 49,   /* 3GPP Bearer 6 signal          */
  PS_3GPP_UM_RX_Q_SIGNAL_7        = 50,   /* 3GPP Bearer 7 signal          */
  PS_LAN_WAPI_TX_DATA_Q_SIGNAL    = 51,   /* WAPI Tx Data queue signal     */
  PS_3GPP_RMSM_AT_UM_RX_Q_SIGNAL  = 52,   /* RMSM AT UM data TX signal     */
  PS_QTF_SYNC_SIGNAL              = 53,   /* PS QTF synchronization signal */
  PS_3GPP_LO_B_UM_RX_Q_SIGNAL     = 54,   /* 3GPP Loopback mode B V4 Rx Sig   */
  PS_RX_RMNET1_LLC1_SIGNAL        = 55,   /* To support simultaneous IPV4/IPV6 over single QMI instance*/
  PS_RX_RMNET1_LLC2_SIGNAL        = 56,
  PS_RX_RMNET2_LLC1_SIGNAL        = 57,
  PS_RX_RMNET2_LLC2_SIGNAL        = 58,
  PS_RX_RMNET3_LLC1_SIGNAL        = 59,
  PS_RX_RMNET3_LLC2_SIGNAL        = 60,
  PS_RX_RMNET4_LLC1_SIGNAL        = 61,
  PS_RX_RMNET4_LLC2_SIGNAL        = 62,
  PS_RX_RMNET5_LLC1_SIGNAL        = 63,
  PS_RX_RMNET5_LLC2_SIGNAL        = 64,
  PS_RX_RMNET6_LLC1_SIGNAL        = 65,
  PS_RX_RMNET6_LLC2_SIGNAL        = 66,
  PS_RX_RMNET7_LLC1_SIGNAL        = 67,
  PS_RX_RMNET7_LLC2_SIGNAL        = 68,
  PS_RX_RMNET8_LLC1_SIGNAL        = 69,
  PS_RX_RMNET8_LLC2_SIGNAL        = 70,
  PS_ALG_TX_SIGNAL                = 71,   /* To support ALG mgr to tx 
                                           * translated payloads */
  PS_3GPP_LO_B_UM_RX_Q_SIGNAL_1   = 72,   /* 3GPP Loopback mode B V6 Rx Sig   */
  PS_3GPP_EMBMS_RX_Q_SIGNAL       = 73,   /**< 3GPP eMBMS DL Data Signal   */
  PS_REV_IP_TRANSPORT_RX_DATA_Q_SIGNAL =  74, /* Reverse IP Transport --> PS data sig        */
  PS_DPM_DPL_SIGNAL                = 75,   /**< IPA DPL packets available Signal  */
  PS_DPM_DL_SIGNAL_0               = 76,
  PS_DPM_DL_SIGNAL_1               = 77,
  PS_DPM_DL_SIGNAL_2               = 78,
  PS_DPM_DL_SIGNAL_3               = 79,
  PS_DPM_DL_SIGNAL_4               = 80,
  PS_DPM_DL_SIGNAL_5               = 81,
  PS_DPM_DL_SIGNAL_6               = 82,
  PS_DPM_DL_SIGNAL_7               = 83,
  PS_DPM_DL_SIGNAL_8               = 84,
  PS_DPM_DL_SIGNAL_9               = 85,
  PS_DPM_DL_SIGNAL_10              = 86,
  PS_DPM_DL_SIGNAL_11              = 87,
  PS_RM_2_RX_SIGNAL                = 88,   /* rx signal for second RM device */ 
  PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL = 89,   /* Incoming TCP data signal      */
  PS_MAX_SIGNALS,                         /* use next value for enum       */
  PS_EXT1_RX_SIGNAL_MAX = PS_MAX_SIGNALS  
} ps_sig_enum_type;

/*---------------------------------------------------------------------------
  Data Task Command Definitions. These are the commands that can be sent to
  one.
---------------------------------------------------------------------------*/

typedef enum
{
  PS_707_ASYNC_TCP_CTL_OPEN_ACTIVE_CMD   = 0/* Open async TCP conn (active)*/
  ,PS_707_ASYNC_TCP_CTL_OPEN_PASSIVE_CMD = 1/* Open async TCP conn(passive)*/
  ,PS_707_ASYNC_PTCL_ABORT_CMD           = 2/* Abort async protocols       */
  ,PS_707_ASYNC_PTCL_CLOSE_CMD           = 3/* Close async protocols       */
  ,PS_707_ASYNC_PPP_DOWN_CMD             = 4/* Async handle PPP down       */
  ,PS_707_ASYNC_PPP_UP_CMD               = 5/* Async handle PPP up         */
  ,PS_707_ASYNC_PPP_RESYNC_CMD           = 6/* Async handle PPP resync     */
  ,PS_707_ASYNC_UM_FLOW_ENABLE_CMD       = 7/* Async enable Um flow        */
  ,PS_SOCKET_NET_DOWN_CMD        = 8  /* Network is down for the socket    */
  // why are there two open commands?
  ,PS_SOCKET_OPEN_PPP_CMD        = 9  /* Bring up PPP                      */
  ,PS_DSSNET_PPP_OPEN_CMD        = 10  /* Open PPP command                 */
  ,PS_DSSNET_PHY_IFACE_UP_CMD    = 11 /* Physical layer is up              */
  ,PS_DSSNET_LINK_LAYER_UP_WITH_MIP_CMD = 12 /* Link up with MIP command   */
  ,PS_DSSNET_LINK_LAYER_UP_WITH_SIP_CMD = 13 /* Link up with SIP command   */
  ,PS_DSSNET_MIP_UP_SUCCESS_CMD  = 14 /* MIP registration successful       */
  ,PS_DSSNET_MIP_FAILURE_CMD     = 15 /* MIP failure successful            */
  ,PS_DSSNET_NET_IFACE_DOWN_CMD  = 16 /* Network iface down                */
  ,PS_DSSNET_PPP_CLOSE_CMD       = 17 /* Close PPP command                 */
  ,PS_DSSNET_PHY_IFACE_TIMER_EXP_CMD = 18  /* Timer expiry command         */
  ,PS_DSSNET_PHY_IFACE_DOWN_CMD  = 19 /* Physical layer down               */
  ,PS_DSSNET_PPP_RESYNC_CMD      = 20 /* PPP Resync command                */
  ,PS_DSSNET_PHY_LINK_DOWN_PPP_ABORT_CMD= 21 /* PPP Abort command          */
  ,PS_SOCKET_IFACE_EVENT_CMD = 22       /* Iface event                     */
  ,PS_SOCKET_PHYS_LINK_EVENT_CMD = 23  /* Phys link event                  */


  // These are the new commands for PPP handling - these
  // replace all of the other PPP commands that are in here.
  ,PS_PPP_START_CMD              = 24 /* start a PPP device                */
  ,PS_PPP_STOP_CMD               = 25 /* gracefully close a PPP device     */
  ,PS_PPP_ABORT_CMD              = 26 /* abort a PPP device                */
  ,PS_PPP_SET_MODE_CMD           = 27 /* change PPP operational mode       */
  ,PS_PPP_INJECT_PKT_CMD         = 28 /* inject a PPP pkt into the device  */
  ,PS_PPP_RESYNC_CMD             = 29 /* resync a PPP device               */
  ,PS_PPP_VENDOR_SEND_CMD        = 30 /* send a vendor-specific packet     */
  ,PS_PPP_NCP_START_CMD          = 31 /* start an NCP instance             */
  ,PS_PPP_NCP_STOP_CMD           = 32 /* stop an NCP instance              */



  ,PS_SNOOP_IP_PPP_DEV_UP_CMD    = 33  /* PPP device is up for IP          */
  ,PS_SNOOP_IP_PPP_DEV_DOWN_CMD  = 34  /* PPP device is down               */
  ,PS_SNOOP_IP_FLTR_ENABLED_CMD  = 35  /* IP filtering enabled on iface    */
  ,PS_SNOOP_IP_FLTR_DISABLED_CMD = 36  /* IP filtering disabled on iface   */
  ,PS_SNOOP_IP_COMP_ON_CMD       = 37  /* handle VJ compressed pkts        */
  ,PS_SNOOP_IP_COMP_OFF_CMD      = 38  /* No more VJ compressed pkts       */

  ,PS_MIP_META_SM_MIN_CMD        = 39 /* (minimum metasm command)          */
  ,PS_MIP_PHYS_IFACE_UP_CMD      = 39 /* Phys link UP - starting MIP       */
  ,PS_MIP_CFG_IFACE_CMD          = 40 /* Start MIP reg to configure iface  */
  ,PS_MIP_EXIT_CMD               = 41 /* MIP client no longer needed       */
  ,PS_MIP_REG_FAILURE_CMD        = 42 /* tell Meta SM initial reg failed   */
  ,PS_MIP_BRING_DOWN_CMD         = 43 /* start MIP dereg to teardown       */
  ,PS_MIP_META_SM_MAX_CMD        = 43 /* (maximum metasm command)          */

  ,PS_MIP_SOCKET_EVENT_CMD       = 44 /* Socket event                      */
  ,PS_DMU_PREENCRYPT_CMD         = 45 /* DMU Random event                  */

 /*-------------------------------------------------------------------------
    The IPv6 State Machine commands
  -------------------------------------------------------------------------*/
  ,PS_IP6_SM_BASE_CMD            = 46  /* used to do math                  */
  ,PS_IP6_SM_START_CMD           = PS_IP6_SM_BASE_CMD
                                       /* the IPv6 SM Start event          */
  ,PS_IP6_SM_RS_TIMER_CMD        = 47  /* the IPv6 SM RS Timer event       */
  ,PS_IP6_SM_RA_TIMER_CMD        = 48  /* the IPv6 SM RA Timer event       */
  ,PS_IP6_SM_VALID_RA_CMD        = 49  /* the IPv6 SM Valid RA event       */
  ,PS_IP6_SM_LINK_CHANGED_CMD    = 50  /* the IPv6 SM Link changed event   */
  ,PS_IP6_SM_STOP_CMD            = 51  /* the IPv6 SM Stop event           */
  ,PS_IP6_SM_ABORT_CMD           = 52  /* the IPv6 SM Abort event */  
  ,PS_IP6_SM_UPDATE_CONFIG_CMD      = 53  /* the IPv6 SM Update the config event */  
  ,PS_IP6_SM_DHCP6_PROVISIONED_CMD  = 54  /* the IPv6 SM Link changed event   */
  ,PS_IP6_SM_DHCP6_FAILED_CMD       = 55  /* the IPv6 SM Link changed event   */
  ,PS_IP6_SM_DHCP6_STOPPED_CMD      = 56  /* the IPv6 SM Stopped event */  
  ,PS_IP6_SM_DHCP6_ABORTED_CMD      = 57  /* the IPv6 SM Aborted event */  
  ,PS_IP6_SM_MAX_CMD             = PS_IP6_SM_DHCP6_ABORTED_CMD
                                       /* used to do math                  */
  ,PS_DHCP_START_CMD             = 58
  ,PS_DHCP_STOP_CMD              = 59
  ,PS_DHCP_CLIENT_START_CMD      = 60 /* Start the DHCP client             */
  ,PS_DHCP_CLIENT_STOP_CMD       = 61 /* Stop the DHCP client              */
  ,PS_DHCP_CLIENT_RENEW_CMD      = 62 /* Send a RENEW to the client        */
  ,PS_DHCP_CLIENT_RELEASE_CMD    = 63 /* Send a RELEASE to the client      */
  ,PS_DHCP_CLIENT_INFORM_CMD     = 64 /* Send an INFORM to the client      */
  ,PS_DHCP_CLIENT_SOCK_EVENT_CMD = 65 /* Send notifation of a socket event */

  ,PS_LAN_LLC_START_CMD          = 66 /* Start an LLC instance             */
  ,PS_LAN_LLC_STOP_CMD           = 67 /* Stop an LLC instance              */

  /*-------------------------------------------------------------------------
     Commands for dial-up networking
  -------------------------------------------------------------------------*/
  /* Dial-up packet control module commands */
  ,PS_DUN_PKT_CTL_IFACE_UP_CMD       = 68
  ,PS_DUN_PKT_CTL_IFACE_DOWN_CMD     = 69
  ,PS_DUN_PKT_CTL_IFACE_EVT_CMD      = 70
  ,PS_DUN_PKT_CTL_PHYS_LINK_UP_CMD   = 71
  ,PS_DUN_PKT_CTL_PHYS_LINK_DOWN_CMD = 72
  ,PS_DUN_PKT_CTL_PPP_EVT_CMD        = 73

  /* Dial-up physical link module commands */
  ,PS_DUN_PHYS_LINK_BRING_UP_CMD     = 74
  ,PS_DUN_PHYS_LINK_TEAR_DOWN_CMD    = 75
  ,PS_DUN_PHYS_LINK_DTR_CHANGED_CMD  = 76
  ,PS_DUN_PHYS_LINK_RDM_OPEN_CMD     = 77
  ,PS_DUN_PHYS_LINK_RDM_CLOSE_CMD    = 78
  ,PS_DUN_PHYS_LINK_PORT_CLOSED_CMD  = 79

  ,PS_SOCKET_FLOW_EVENT_CMD          = 80     /* Flow event         */

  /* Commands for ARP */
  ,PS_ARP_FLUSH_CMD                  = 81 /* Flush an ARP instance's cache */

  /* Commands for Stats */
  ,PS_STAT_LOG_ON_DEMAND_CMD         = 82 /* Process log on demand request */

  ,PS_LAN_LLC_RECONFIG_CMD           = 83 /* Reconfigure an LLC instance   */

  ,PS_TIMER_CALLBACK_CMD             = 84 /* PS Timer expired call cback
                                             in PS context.                */
  ,PS_HDRRSP_CMD                     = 85 /* Route Selection Protocol      */

  ,PS_DHCP_SERVER_SOCK_EVENT_CMD     = 86 /* Send notifation of socket event*/

  ,PS_LL_ADDR_MGR_START_CMD          = 87 /* Start the Link local address mgr */

  ,PS_LL_ADDR_MGR_STOP_CMD           = 88 /* Stop the Link local address mgr  */

  ,PS_LL_ADDR_MGR_CLAIM_CMD          = 89 /* CLAIM an IP address        */

  ,PS_DHCP6_CLIENT_START_CMD         = 90  /* Start the DHCPv6 client   */

  ,PS_DHCP6_CLIENT_STOP_CMD          = 91  /* Stop the DHCPv6 client    */

  ,PS_DHCP6_CLIENT_ABORT_CMD         = 92  /* Abort the DHCPv6 client    */

  ,PS_DHCP6_CLIENT_FREE_CMD          = 93  /* Abort the DHCPv6 client    */

  ,PS_DHCP6_CLIENT_INFORM_CMD        = 94  /* Send INFORMATION-REQUEST  */

  ,PS_DHCP6_CLIENT_SOCK_EVENT_CMD    = 95  /* Send notification of
                                            * socket event */

  ,PS_RSVP_SOCKET_EVENT_CMD          = 96  /* RSVP socket event call back */

  /* Commands for IGMP/MLD */
  ,PS_MCAST_SEND_REPORT_CMD          = 97 /* Send IGMP/MLD JOIN Message     */

  ,PS_MCAST_SEND_LEAVE_CMD           = 98 /* Send IGMP/MLD LEAVE Message    */

  ,PS_DSS_PING_SEND_PING_CMD         = 99 /* Send ICMP ECHO_REQUEST */

  ,PS_DSS_PING_NET_CB_CMD            = 100 /* Ping network callback */

  ,PS_DSS_PING_SOCK_CB_CMD           = 101 /* Ping socket callback */

  ,PS_DSS_PING_RSP_TIMEOUT_CB_CMD    = 102 /* Ping response timeout cb */

  ,PS_DSM_MEM_EVENT_CTRL_CMD         = 103 /* Post WRITE_EV on sockets      */

  /*-------------------------------------------------------------------------
     Commands for DSS Net Manager
  -------------------------------------------------------------------------*/
  ,PS_DSS_NET_MGR_NET_UP_CMD         = 104 /* Net mgr net up */
  ,PS_DSS_NET_MGR_NET_DOWN_CMD       = 105 /* net mgr net down cmd          */

  /*-------------------------------------------------------------------------
    Domain Name System resolver command.
  -------------------------------------------------------------------------*/
  ,PS_DNS_RESOLVE_CMD                = 106 /* To start the DNS resolver     */
  ,PS_DNS_IO_MGR_SOCK_EVENT_CMD      = 107 /* IO manager sock event cmd     */
  ,PS_DNS_RESOLVER_TIMEOUT_CMD       = 108 /* Resolver timeout event        */
  ,PS_DNS_DELETE_SESSION_CMD         = 109 /* Delete session command        */

  /* Commands for MCAST */
  ,PS_MCAST_JOIN_CMD                 = 110 /* Send MCAST JOIN Message */

  ,PS_MCAST_LEAVE_CMD                = 111 /* Send MCAST LEAVE Message */

  /*-------------------------------------------------------------------------
    MIPv6 Registration State Machine commands
  -------------------------------------------------------------------------*/
  ,PS_MIP6_RSM_REG_CMD               = 112  /* Trigger registration        */
  ,PS_MIP6_RSM_DEREG_CMD             = 113  /* Trigger de-registration     */
  ,PS_MIP6_RSM_REG_SUCCESS_CMD       = 114  /* Reg success                 */
  ,PS_MIP6_RSM_REG_FAIL_CMD          = 115  /* Reg failure                 */
  ,PS_MIP6_RSM_CLOSE_CMD             = 116  /* Tear-down the RSM           */

  /*-------------------------------------------------------------------------
    Instance statistics logging commands
   ------------------------------------------------------------------------*/
  ,PS_STAT_INST_GET_DESC_CMD         = 117  /* Get instance description    */
  ,PS_STAT_INST_GET_STAT_CMD         = 118  /* Get instance stats          */
  ,PS_STAT_INST_RESET_STAT_CMD       = 119  /* Reset instance statistics   */

  /*-------------------------------------------------------------------------
    SIM event handling commands
   ------------------------------------------------------------------------*/
  ,PS_SIM_EVENT_HANDLER_CMD          = 120  /* Handle SIM_INIT_COMPLETED ev*/

  /*-------------------------------------------------------------------------
    EAP/PPP-MMGSDI registration events handling
  -------------------------------------------------------------------------*/
  ,PS_EAP_CLNT_REG_CMD               = 121  /* EAP-MMGSDI client reg       */
  ,PS_EAP_SESSION_EVT_CB_CMD         = 122  /* EAP-MMGSDI session reg      */
  ,PS_PPP_CLNT_REG_CMD               = 123  /* PPP-MMGSDI client reg       */
  ,PS_PPP_EAP_TASK_CMD               = 124  /* PPP/EAP MMGSDI task switch  */

  /*-------------------------------------------------------------------------
    DSSOCK/DSNET commands to process iface, flow, phys link, DOS_ACK events.
  -------------------------------------------------------------------------*/
  ,PS_DSSOCK_PROCESS_GENERIC_EVENT_CMD  = 125
  ,PS_DSSOCK_PROCESS_DOS_ACK_EVENT_CMD  = 126
  ,PS_DSNET_PROCESS_GENERIC_EVENT_CMD   = 127

 /*-------------------------------------------------------------------------
    DHCP MULTIPLE IP MGR event handling commands
   ------------------------------------------------------------------------*/
  ,PS_DHCP_SERVER_MGR_START_CMD         = 128 /* Multiple IP DHCP mgr start */
  ,PS_DHCP_SERVER_MGR_STOP_CMD          = 129 /* Multiple IP DHCP mgr stop */
  ,PS_DNS_PROXY_SOCK_EVENT_CMD          = 130  /* DNS Proxy Socket ev cmd   */
  ,PS_DNS_PROXY_ENABLE_CMD              = 131  /* DNS Proxy Enable command  */
  ,PS_DNS_PROXY_DISABLE_CMD             = 132  /* DNS Proxy Disable command */
  ,PS_DNS_PROXY_IFACE_EVENT_CMD         = 133  /* DNS Proxy Iface ev command*/

  /*-------------------------------------------------------------------------
    PS UTILS AKA commands
  -------------------------------------------------------------------------*/
  ,PS_UTILS_AKA_CMD                     = 134  /* PS UTILS AKA CMD handler */
   
  /*-------------------------------------------------------------------------
    PPP NCP commands (contd..)
  -------------------------------------------------------------------------*/
  ,PS_PPP_NCP_RESYNC_CMD                = 135 /* resync an NCP instance    */ 
  ,PS_PPP_NCP_ABORT_CMD                 = 136 /* abort an NCP instance     */ 
  ,PS_NAT_DELETE_DMZ_CMD                = 137 /* Delete DMZ command        */

  /*-------------------------------------------------------------------------
    PPP DEVICE STOP command
  -------------------------------------------------------------------------*/
  ,PS_PPP_DEVICE_STOP_CMD               = 138 /* PPP Device STOP command   */
  ,PS_START_BCAST_GARP                  = 139 /* Start broadcasting GARP   */

  /*-------------------------------------------------------------------------
    Commands to flow control ps_iface, ps_flow, and ps_phys_link
  -------------------------------------------------------------------------*/
  ,PS_IFACE_FLOW_CTRL_CMD               = 140
  ,PS_FLOW_FLOW_CTRL_CMD                = 141
  ,PS_PHYS_LINK_FLOW_CTRL_CMD           = 142

  /*-------------------------------------------------------------------------
    PS SYS commands to process sys events.
  -------------------------------------------------------------------------*/
  ,PS_SYS_EVENT_CMD_HANDLER             = 143
  
  /*-------------------------------------------------------------------------
    PPP Trigger Link Status Determination (LSD) command
  -------------------------------------------------------------------------*/
  ,PS_PPP_TRIGGER_LSD_CMD               = 144 /* PPP Trigger LSD command   */

  /*-------------------------------------------------------------------------
    Stateless DHCPv6 Server MGR event handling commands
  -------------------------------------------------------------------------*/
  ,PS_DHCP6_SL_SERVER_MGR_START_CMD     = 145 /* Stateless DHCP6 mgr start */
  ,PS_DHCP6_SL_SERVER_MGR_STOP_CMD      = 146 /* Stateless DHCP6 mgr stop */
  ,PS_DHCP6_SL_SERVER_SOCK_EVENT_CMD    = 147 /* Send notification of
                                                 socket event */
  /*-------------------------------------------------------------------------
    More Domain Name System resolver commands.
  -------------------------------------------------------------------------*/
  ,PS_DNS_RESOLVER_IFACE_DOWN_CMD         = 148 /* Iface Down event received */
  ,PS_DNS_RESOLVER_IFACE_ADDR_CHANGED_CMD = 149 /* Iface Address Changed event
                                                   received */
  ,PS_DNS_RESOLVER_DELETE_INSTANCE_CMD    = 150 /* Delete the resolver */

  /*-------------------------------------------------------------------------
    Send immediate MIP registration command
   ------------------------------------------------------------------------*/
  ,PS_MIP_SEND_IMMED_REG_CMD            = 151

  /*-------------------------------------------------------------------------
    Would be posted once DHCP invokes socket creation and bind APIs
  -------------------------------------------------------------------------*/
  ,PS_DHCP_SERVER_STARTED               = 152

  /*-------------------------------------------------------------------------
    Domain Name System flush cache commands.
  -------------------------------------------------------------------------*/
  ,PS_DNS_FLUSH_CACHE_CMD               = 153 /* Flush cache command       */
  ,PS_DNS_FLUSH_CACHE_ENTRY_CMD         = 154 /* Flush cache-entry command */
  ,PS_AUDIO_OFFLOAD_CLIENT_SIO_EVENT_CMD = 155/* SIO notification */
  ,PS_AUDIO_OFFLOAD_CLIENT_SOCKET_EVENT_CMD = 156
  ,PS_AUDIO_OFFLOAD_CLIENT_DSM_EVENT_CMD = 157
 /*-------------------------------------------------------------------------
    MCFG NV refresh command.
  -------------------------------------------------------------------------*/
  ,PS_UTILS_NV_REFRESH_EVENT_CMD            = 158  /*MCFG NV refresh */
 /*-------------------------------------------------------------------------
    IPDC logging command
  -------------------------------------------------------------------------*/
  ,PS_COMP_LOGGING_HELPER_CMD               = 159  /* Comp Logging Helper command */

  /*-------------------------------------------------------------------------
    IFACE GO NULL CMD
  -------------------------------------------------------------------------*/
  ,PS_IFACE_GO_NULL_CTRL_CMD = 160  
  
  /* Always must be last */
  ,PS_MAX_DEFINED_CMD_TYPES

} ps_cmd_enum_type;


/*--------------------------------------------------------------------------
  Type definition for the signal handler.
  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared
---------------------------------------------------------------------------*/

typedef boolean (*ps_sig_handler_type)
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

/*--------------------------------------------------------------------------
  Type definition for the command handler
---------------------------------------------------------------------------*/

typedef void (*ps_cmd_handler_type)
(
  ps_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr           /* Command specific user parameters       */
);

/*===========================================================================
MACRO PS_SET_SIGNAL()

DESCRIPTION
  Sets a SIGNAL onto ps_tcb
  - Recommended API for all signals including ext signals.

PARAMETERS
  sig - ps_sig_enum_type

RETURN VALUE
  None.
===========================================================================*/
#define PS_SET_SIGNAL ps_set_signal
/*===========================================================================
MACRO PS_CLR_SIGNAL()
- Recommended API for all signals including ext signals.

DESCRIPTION
  Clears a SIGNAL in ps_tcb

PARAMETERS
  sig - ps_sig_enum_type

RETURN VALUE
  None.
===========================================================================*/
#define PS_CLR_SIGNAL ps_clr_signal
#ifdef __cplusplus
extern "C" {
#endif
void ps_set_signal(ps_sig_enum_type sig);
void ps_clr_signal(ps_sig_enum_type sig);
#ifdef __cplusplus
}
#endif

/*===========================================================================
MACRO PS_SET_EXT1_SIGNAL()

DESCRIPTION
  Sets a SIGNAL onto ps_tcb //extension signals ONLY

PARAMETERS
  sig - ps_sig_enum_type

RETURN VALUE
  None.
===========================================================================*/
#define PS_SET_EXT1_SIGNAL ps_set_ext_signal
/*===========================================================================
MACRO PS_CLR_EXT1_SIGNAL()

DESCRIPTION
  Clears a SIGNAL in ps_tcb //extension signals ONLY
  - Signal must have been set by SET_EXT1 macro

PARAMETERS
  sig - ps_sig_enum_type

RETURN VALUE
  None.
===========================================================================*/
#define PS_CLR_EXT1_SIGNAL ps_clr_ext_signal
#ifdef __cplusplus
extern "C" {
#endif
void ps_set_ext_signal(ps_sig_enum_type sig);
void ps_clr_ext_signal(ps_sig_enum_type sig);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION PS_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the PS task uses for rex_wait()

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void ps_enable_sig
(
  ps_sig_enum_type sig                        /* Signal to be enabled      */
);

/*===========================================================================

FUNCTION PS_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void ps_disable_sig
(
  ps_sig_enum_type sig                        /* Signal to be disabled     */
);

/*===========================================================================

FUNCTION PS_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/

extern ps_sig_handler_type ps_set_sig_handler
(
  ps_sig_enum_type sig,              /* Signal assocaited with handler     */
  ps_sig_handler_type sig_handler,   /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
);

/*===========================================================================

FUNCTION PS_RESET_SIG_HANDLER()

DESCRIPTION
  This function resets a handler for the specified signal

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ps_reset_sig_handler
(
  ps_sig_enum_type sig
);

/*===========================================================================

FUNCTION PS_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in PS task context.  The cmd is
  processed by calling the registered cmd handler, if any.

  NOTE: The passed command will be copied to a PS task
  command buffer local to the PS Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ps_send_cmd
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
);

/*===========================================================================

FUNCTION PS_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  The cmd handling
  can be disabled by setting a NULL handler

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/

extern ps_cmd_handler_type ps_set_cmd_handler
(
  ps_cmd_enum_type cmd,              /* Command name                      */
  ps_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
);

#ifdef __cplusplus
}
#endif

/*===========================================================================

FUNCTION PS_WAIT_EXT()

DESCRIPTION
  This function performs the PS Task waiting function(support signal extensions). This function will 
  not return until at least one signal from the requested mask is set. 
  The watchdog is also kicked at least once.
  
DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated 
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

NOTE
  - Uses REX extension signal APIs to set/get signal mask.
  - dword is mapped to rex_sigs_type.
===========================================================================*/
#define PS_WAIT_EXT   ps_wait_ext
extern dword* ps_wait_ext
( 
  dword* requested_mask              /* mask of signals to wait for */
);

/*===========================================================================

FUNCTION PS_WAIT()

DESCRIPTION
  This function performs the PS Task waiting function. This function will 
  not return until at least one signal from the requested mask is set. 
  The watchdog is also kicked at least once.
  
DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated 
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

NOTE
  It does not support extensions.
  dword mapped to rex_sigs_type
===========================================================================*/
#define PSI_WAIT   ps_wait
#define psi_wait   ps_wait // To support old users
extern dword ps_wait
( 
  dword requested_mask              /* mask of signals to wait for */
);
#endif /* PS_SVC_H */
